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

sub dec_rev($)
{
    my $rev = $_[0];

    # decrement the revision number to get the previos one
    # (FIXME this is totally bogus, won't work with branches)
    my $revlen = length($rev) - rindex($rev, '.') - 1;
    my $m = 10**$revlen;

    return int($rev) . "." . ($rev*$m - int($rev)*$m - 1)
}

sub get_last_rev($)
{
    my $file = $_[0];

    my $basename = $file;
    $basename =~ s@^.*/([^/]\+)@$1@;

    # first get the current version: try the Id RCS tag in the file itself
    # first, use "cvs status" if this fails
    if ( open(INPUT, $file) ) {
        while (<INPUT>) {
            # notice that we shouldn't have '$' followed by 'Id' or cvs will
            # substitute it!
            if ( /\$(Id): $basename,v (\d+\.\d+)/ ) {
                return &dec_rev($2);
            }
        }
    }

    open(INPUT, "$CVS -q status $file |") or return 0;

    while (<INPUT>) {
        if ( /Working revision:\s+(\d+\.\d+)/ ) {
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
    elsif ( $revlast =~ "\.0" ) {
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
