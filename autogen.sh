#!/bin/sh

aclocal
libtoolize --force
autoheader
automake --foreign --add-missing
[ -x /usr/bin/autoconf2.50 ] && autoconf2.50 || aclocal

echo Please run ./configure now.

