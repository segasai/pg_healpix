MODULE_big=pg_healpix
OBJS=ang2pix_nest.o ang2pix_ring.o pix2ang_nest.o pix2ang_ring.o mk_pix2xy.o mk_xy2pix.o pg_healpix.o
DATA_built=pg_healpix.sql
DOCS=README.pg_healpix

OPT=-O3
OPT_LOW=-O2
DEBUG=
#DEBUG=-g3 -ggdb
PG_CPPFLAGS = $(DEBUG) $(OPT) -D_GNU_SOURCE -D__STDC_FORMAT_MACROS
SHLIB_LINK += $(filter -lm, $(LIBS))

ifdef NO_PGXS
subdir = contrib/pg_healpix
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
else
PGXS := $(shell pg_config --pgxs)
ifndef PGXS
$(error You should have `pg_config` program in your PATH or compile Q3C with\
'make NO_PGXS=1' \
after putting it in the contrib subdirectory of Postgres sources)
endif
include $(PGXS)

endif

test: all
	createdb pgheal_test
	psql pgheal_test -c '\i pg_healpix.sql'
	cat tests/simple1.sql | psql pgheal_test > tests/simple1.out
	cat tests/simple2.sql | psql pgheal_test > tests/simple2.out
	cat tests/simple3.sql | psql pgheal_test > tests/simple3.out
	cat tests/simple4.sql | psql pgheal_test > tests/simple4.out
	diff tests/simple1.out tests/simple1.expected
	diff tests/simple2.out tests/simple2.expected
	diff tests/simple3.out tests/simple3.expected
	diff tests/simple4.out tests/simple4.expected
	dropdb pgheal_test

dist: clean
	mkdir -p dist
	cp *.c *.h *.sql.in Makefile README.pgheal COPYING dist
	mkdir -p dist/tests
	cp tests/*.expected tests/*.sql dist/tests
	cat pg_healpix.sql.in | perl create_drops.pl > dist/drop_pghealpix.sql

