#!/bin/sh

aclocal
libtoolize
automake --foreign --add-missing
autoconf

echo Please run ./configure now.

