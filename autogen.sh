#!/bin/sh

aclocal
libtoolize --force
autoheader
automake --foreign --add-missing
autoconf

echo Please run ./configure now.

