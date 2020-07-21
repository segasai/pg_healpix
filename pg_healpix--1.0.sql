/* pg_healpix/pg_healpix--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_healpix" to load this file. \quit

CREATE FUNCTION healpix_ang2ipix_nest(bigint, double precision, double precision)
RETURNS bigint
AS 'MODULE_PATHNAME', 'pgheal_ang2ipix_nest'
LANGUAGE C IMMUTABLE STRICT;

COMMENT ON FUNCTION healpix_ang2ipix_nest (bigint, double precision, double precision)
IS 'Function converting nside, Ra and Dec to the healpix nested ipix value';

CREATE OR REPLACE FUNCTION healpix_ang2ipix_ring(bigint, double precision, double precision)
RETURNS bigint
AS 'MODULE_PATHNAME', 'pgheal_ang2ipix_ring'
LANGUAGE C IMMUTABLE STRICT;

COMMENT ON FUNCTION healpix_ang2ipix_ring (bigint, double precision, double precision)
IS 'Function converting nside, Ra and Dec to the healpix ring ipix value';

CREATE OR REPLACE FUNCTION healpix_ipix2ang_nest(nside bigint, ipix bigint)
RETURNS double precision[]
AS 'MODULE_PATHNAME', 'pgheal_ipix2ang_nest'
LANGUAGE C IMMUTABLE STRICT;

COMMENT ON FUNCTION healpix_ipix2ang_nest(bigint, bigint)
IS 'Function converting the Healpix nested ipix value to Ra, Dec';

CREATE OR REPLACE FUNCTION healpix_ipix2ang_ring(nside bigint, ipix bigint)
RETURNS double precision[]
AS 'MODULE_PATHNAME', 'pgheal_ipix2ang_ring'
LANGUAGE C IMMUTABLE STRICT;

COMMENT ON FUNCTION healpix_ipix2ang_ring(bigint, bigint)
IS 'Function converting the Healpix ring ipix value to Ra, Dec';
