					PG_HEALPIX
Author: Sergey Koposov, Institute of Astronomy, Cambridge
	and multiple HEALPIX authors    (C) 2012
Email: koposov@ast.cam.ac.uk

This software provides an SQL (PostgreSQL) interface for several 
HEALPIX functions, which allows you to convert the ra,dec coordinates 
into Healpix IDs and back. 

---------------------------------------------------------------------------------
Example:
wsdb=> select healpix_ang2ipix_nest(1024, 12., 25.);
 healpix_ang2ipix_nest
-----------------------
               5102321
(1 row)

wsdb=> select healpix_ang2ipix_ring(1024, 12., 25.);
 healpix_ang2ipix_ring
-----------------------
               3627144
(1 row)

wsdb=> select healpix_ipix2ang_nest(1024, 4323);
     healpix_ipix2ang_nest
--------------------------------
 {47.63671875,3.24700649515993}
(1 row)

wsdb=> select healpix_ipix2ang_ring(1024, 4323);
        healpix_ipix2ang_ring
-------------------------------------
 {359.021739130435,87.8983556468228}
(1 row)
--------------------------------------------------------------------------------

Installation is standard:

1) Download the package 
2) Compile the code using simple
	make
Please check that you have pg_config in your path
3) Install the packge using
	make install
4) Load the functions into PostgreSQL by doing 
	\i pg_healpix.sql
from the psql client
