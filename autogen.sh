#!/bin/sh

aclocal-1.4
libtoolize-1.4 --force
automake-1.4 --foreign --add-missing
autoconf

echo Please run ./configure now.

