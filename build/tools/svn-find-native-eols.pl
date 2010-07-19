#!/usr/bin/perl
#
# This script must be ran from svn checkout and will produce the list of all
# files using native svn:eol-style on output. It's used as a helper for
# distribution creation as this is also the list of files which need to have
# their line endings converted for the use on the platform other than the
# current one.
#
# Notice that the script requires Perl 5.10 (which could be easily avoided but
# as this is for my personal use mostly so far, I didn't bother) and Perl svn
# bindings.
use 5.10.0;
use strict;
use warnings;
use SVN::Client;

my $ctx = SVN::Client->new
    or die "Failed to create svn context, do you have svn auth stored?\n";

# For testing purposes a single parameter may be specified to restrict the list
# of files with native EOLs to just this directory (recursively) or even a
# single file. In normal use no parameters should be given.
my $path = $ARGV[0] // '';
my $props = $ctx->proplist($path, undef, 1)
    or die "Failed to list properties for $path.\n";

foreach my $prop (@$props) {
    my $eol = ${$prop->prop_hash()}{'svn:eol-style'};
    if ( defined $eol && ($eol eq 'native') ) {
        say $prop->node_name();
    }
}
