/*
 * Copyright (C) 2004-2012 Sergey Koposov
 *	
 * Author: Sergey Koposov, Institute of Astronomy, Cambridge
 * Multiple authors of the HEALPIX code are given in the LICENSE.healpix file
 * Email: koposov@ast.cam.ac.uk
 * http://ast.cam.ac.uk/~koposov
 * 
 * This file is part of pg_healpix
 * 
 * pg_healpix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * pg_healpix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pg_healpix; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <string.h>
#include <math.h>

/* Postgres stuff */
#include "postgres.h"
#include "executor/spi.h"
#include "utils/lsyscache.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "fmgr.h"

/* For PostgreSQL versions >= 8.2 */
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif
/* End of Postgres stuff */

#include "chealpix.h"

#define RADIAN(x) ((M_PI / 180.) * (x))
#define DEGREE(x) ((180. / M_PI) * (x))
#define NSIDEMAX 8192			/* so they say */

typedef double q3c_coord_t;
typedef long q3c_ipix_t;

void		nside_check(q3c_ipix_t);
void		ipix_check(q3c_ipix_t, q3c_ipix_t);
void		angle_check(q3c_coord_t);
void		get_theta_phi(q3c_coord_t, q3c_coord_t, q3c_coord_t *,
						  q3c_coord_t *);
void		get_ra_dec(q3c_coord_t, q3c_coord_t, q3c_coord_t *, q3c_coord_t *);

ArrayType  *build_array(q3c_coord_t, q3c_coord_t);

inline void
nside_check(q3c_ipix_t nside)
{
	if (
		((nside & (nside - 1)) != 0) || /* test if power of two */
		(nside <= 0) ||
		(nside > NSIDEMAX)
		)
	{
		elog(ERROR, "Invalid value of nside. Must be >0, <=8192 and power of two");
	}

}

inline void
ipix_check(q3c_ipix_t nside, q3c_ipix_t ipix)
{
	if ((ipix < 0) || (ipix >= (12 * nside * nside)))
	{
		elog(ERROR, "Invalid value of ipix. Must be >=0 and be < 12*NSIDE^2");
	}

}

inline void
angle_check(q3c_coord_t theta)
{
	if (theta < 0. || theta > M_PI) /* theta is like polar distance */
	{
		elog(ERROR, "Invalid angle");
	}
}

inline void
get_theta_phi(q3c_coord_t ra, q3c_coord_t dec, q3c_coord_t *theta,
			  q3c_coord_t *phi)
{
	*theta = M_PI_2 - RADIAN(dec);	/* should be between 0 and M_PI */
	*phi = RADIAN(ra);			/* in theory between 0 and 2*M_PI but could
								 * wrap more than that */
}

inline void
get_ra_dec(q3c_coord_t theta, q3c_coord_t phi, q3c_coord_t *ra,
		   q3c_coord_t *dec)
{
	*dec = DEGREE(M_PI_2 - theta);
	*ra = DEGREE(phi);			/* in theory between 0 and 2*M_PI but could
								 * wrap more than that */
}

ArrayType *
build_array(q3c_coord_t ra, q3c_coord_t dec)
{
	Datum	   *data;
	int16		typlen;
	bool		typbyval;
	char		typalign;
	int			nelems;
	ArrayType  *result;

	nelems = 2;
	data = (Datum *) palloc(sizeof(Datum) * nelems);

	data[0] = Float8GetDatum(ra);
	data[1] = Float8GetDatum(dec);

	/* get required info about the element type */
	get_typlenbyvalalign(FLOAT8OID, &typlen, &typbyval, &typalign);

	/* now build the array */
	result = construct_array(data, nelems, FLOAT8OID, typlen, typbyval, typalign);
	return result;
}


/* Postgres functions */
Datum		pgheal_ang2ipix_nest(PG_FUNCTION_ARGS);
Datum		pgheal_ang2ipix_ring(PG_FUNCTION_ARGS);
Datum		pgheal_ipix2ang_nest(PG_FUNCTION_ARGS);
Datum		pgheal_ipix2ang_ring(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pgheal_ang2ipix_nest);
Datum
pgheal_ang2ipix_nest(PG_FUNCTION_ARGS)
{

	q3c_ipix_t	nside = PG_GETARG_INT64(0);
	q3c_coord_t ra = PG_GETARG_FLOAT8(1);
	q3c_coord_t dec = PG_GETARG_FLOAT8(2);
	q3c_ipix_t	ipix;
	q3c_coord_t theta;
	q3c_coord_t phi;

	if ((!isfinite(ra)) || (!isfinite(dec)))
	{
		PG_RETURN_NULL();
	}
	get_theta_phi(ra, dec, &theta, &phi);
	nside_check(nside);
	angle_check(theta);
	ang2pix_nest(nside, theta, phi, &ipix);
	PG_RETURN_INT64(ipix);
}


PG_FUNCTION_INFO_V1(pgheal_ang2ipix_ring);
Datum
pgheal_ang2ipix_ring(PG_FUNCTION_ARGS)
{
	q3c_ipix_t	nside = PG_GETARG_INT64(0);
	q3c_coord_t ra = PG_GETARG_FLOAT8(1);
	q3c_coord_t dec = PG_GETARG_FLOAT8(2);
	q3c_ipix_t	ipix;
	q3c_coord_t theta,
				phi;

	if ((!isfinite(ra)) || (!isfinite(dec)))
	{
		PG_RETURN_NULL();
	}
	get_theta_phi(ra, dec, &theta, &phi);
	nside_check(nside);
	angle_check(theta);
	ang2pix_ring(nside, theta, phi, &ipix);
	PG_RETURN_INT64(ipix);
}


PG_FUNCTION_INFO_V1(pgheal_ipix2ang_nest);
Datum
pgheal_ipix2ang_nest(PG_FUNCTION_ARGS)
{
	q3c_ipix_t	ipix;
	q3c_coord_t ra,
				dec,
				theta,
				phi;
	q3c_ipix_t	nside = PG_GETARG_INT64(0);

	nside_check(nside);
	ipix = PG_GETARG_INT64(1);
	ipix_check(nside, ipix);
	pix2ang_nest(nside, ipix, &theta, &phi);
	get_ra_dec(theta, phi, &ra, &dec);
	PG_RETURN_ARRAYTYPE_P(build_array(ra, dec));
}

PG_FUNCTION_INFO_V1(pgheal_ipix2ang_ring);
Datum
pgheal_ipix2ang_ring(PG_FUNCTION_ARGS)
{
	q3c_ipix_t	ipix;
	q3c_coord_t ra,
				dec,
				theta,
				phi;
	q3c_ipix_t	nside = PG_GETARG_INT64(0);

	nside_check(nside);
	ipix = PG_GETARG_INT64(1);
	ipix_check(nside, ipix);
	pix2ang_ring(nside, ipix, &theta, &phi);
	get_ra_dec(theta, phi, &ra, &dec);
	PG_RETURN_ARRAYTYPE_P(build_array(ra, dec));
}
