#!/bin/sh
aclocal -I .
autoheader
automake --foreign --verbose
autoconf
