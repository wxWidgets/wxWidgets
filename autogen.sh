#!/bin/sh
aclocal -I .
autoheader
automake --verbose
autoconf
