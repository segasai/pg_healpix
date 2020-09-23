MODULE_big = pg_healpix
OBJS = ang2pix_nest.o ang2pix_ring.o pix2ang_nest.o pix2ang_ring.o mk_pix2xy.o mk_xy2pix.o pg_healpix.o
EXTRA_CLEAN = pg_healpix.typedefs

EXTENSION = pg_healpix
DATA = pg_healpix--1.0.sql
PGFILEDESC = "pg_healpix - HEALPIX functions for PostgreSQL"

REGRESS = simple1 simple2 simple3 simple4

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

test: all
	dropdb --if-exists pghpx_test
	createdb pghpx_test
	psql -c 'create extension pg_healpix;' pghpx_test
