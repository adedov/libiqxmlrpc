#!/bin/sh

aclocal
libtoolize --force
automake --foreign --add-missing
autoconf

echo Please run ./configure now.

