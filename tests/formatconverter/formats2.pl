#!/usr/bin/perl -w
#
# Creates longer test formats for wxFormatConverter, containing two '%'s. The
# output is basically the cross product of the output of two './formats.pl's.
#
#   ./formats2.pl | ./formattest
#
use strict;

open IN, './formats.pl |';

while (<IN>) {
    chomp;
    my ($format, $expected) = split "\t";
    open IN2, './formats.pl |';

    while (<IN2>) {
        chomp;
        my ($format2, $expected2) = split "\t";
        print "$format $format2\t$expected $expected2\n";
    }

    close IN2;
}

close IN;
