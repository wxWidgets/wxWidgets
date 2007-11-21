#!/usr/bin/perl -w

###############################################################################
# Name:     difflast.pl
# Purpose:  Shows the difference between the current local version of the file
#           and the last cvs revision
# Version:  $Id$
# Author:   VZ
# Created:  23.12.99
# Copyright:(c) Vadim Zeitlin 1999
###############################################################################

use strict;

my $CVS = "cvs -z3";    # the cvs command

# the regexp for cvs revision number
my $RE_CVS_REV = "\\d+(?:\\.\\d+)+";

sub dec_rev($)
{
    my $rev = $_[0];

    # decrement the revision number to get the previos one
    $rev =~ s/(\d+)$/$1 - 1/e;

    return $rev;
}

sub get_last_rev($)
{
    my $file = $_[0];

    my $basename = $file;
    $basename =~ s@^.*/@@;

    # first get the current version: try the Id RCS tag in the file itself
    # first, use "cvs status" if this fails
    if ( open(INPUT, $file) ) {
        while (<INPUT>) {
            # notice that we shouldn't have '$' followed by 'Id' or cvs will
            # substitute it!
            if ( /\$(Id): $basename,v ($RE_CVS_REV)/ ) {
                return &dec_rev($2);
            }
        }
    }

    open(INPUT, "$CVS -q status $file |") or return 0;

    while (<INPUT>) {
        if ( /Working revision:\s+($RE_CVS_REV)/ ) {
            return &dec_rev($1);
        }
    }

    return 0;
}

sub process_file($)
{
    my $file = $_[0];
    my $revlast = &get_last_rev($file);

    if ( !$revlast ) {
        warn "Failed to get the last revision for $file, skipping.\n"
    }
    elsif ( $revlast =~ '\.0' ) {
        warn "No previous revision of the file $file.\n"
    }
    else {
        print `$CVS diff -b -kk -r $revlast $file`;
    }
}

# entry point

die "Usage: $0 <filenames...>\n" if ( $#ARGV == -1 );

foreach my $file (@ARGV) { process_file($file); }

exit 0;
