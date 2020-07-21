#!/bin/sh

#
# Based on Andrew Dunstan's blog post ideas:
# - http://adpgtech.blogspot.com/2015/05/running-pgindent-on-non-core-code-or.html
#

if [ -z $1 ]; then
    echo "ERROR: Unrecognized argument"
    exit 1
fi

if [ ! -d $1 ]; then
    echo "ERROR: Argument $1 should be a directory"
    exit 1
fi

PGINDENT="$1/src/tools/pgindent/pgindent"
FILES=${2:-'*.h *.c'}

if [ ! -f $PGINDENT ]; then
    echo "ERROR: File $PGINDENT not found"
    exit 1
fi

if [ ! -f pg_healpix.so ]; then
    echo "ERROR: File pg_healpix.so not found! Run make and try again"
    exit 1
fi

objdump -W pg_healpix.so | \
    egrep -A3 DW_TAG_typedef | \
    perl -e ' while (<>) { chomp; @flds = split;next unless (1 < @flds); \
        next if $flds[0]  ne "DW_AT_name" && $flds[1] ne "DW_AT_name"; \
        next if $flds[-1] =~ /^DW_FORM_str/; \
        print $flds[-1],"\n"; }'  | \
    sort | uniq > pg_healpix.typedefs

for FILE in ${FILES}; do
    $PGINDENT --typedefs=pg_healpix.typedefs $FILE
done 

