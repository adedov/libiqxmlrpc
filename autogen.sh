#!/bin/sh

aclocal
automake --foreign --add-missing
autoconf

echo Please run ./configure now.

