#!/bin/sh

aclocal
libtoolize --force
automake --foreign --add-missing
autoheader
autoconf

echo Please run ./configure now.

