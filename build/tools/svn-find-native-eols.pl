#!/usr/bin/perl
#
# This script produces the list of all files using native svn:eol-style.
#
# It's used as a helper for distribution creation as this is also the
# list of files which need to have their line endings converted for
# the use on the platform other than the current one.
#
# Notice that the script requires Perl 5.10 (which could be easily avoided but
# as this is for my personal use mostly so far, I didn't bother) and Perl svn
# bindings.
use 5.10.0;
use strict;
use warnings;
use SVN::Client;

# Normally we get the list directly from the server but this is slow,
# so if you already have an (up to date!) svn checkout, you can also
# pass a path to it here, the script will work much faster then.
my $root = $ARGV[0] // 'https://svn.wxwidgets.org/svn/wx/wxWidgets/trunk';

my $ctx = SVN::Client->new
    or die "Failed to create svn context, do you have svn auth stored?\n";

my $props = $ctx->proplist($root, undef, 1)
    or die "Failed to list properties for files under $root.\n";

foreach my $prop (@$props) {
    my $eol = ${$prop->prop_hash()}{'svn:eol-style'};
    if ( defined $eol && ($eol eq 'native') ) {
        my $rel = $prop->node_name();
        substr($rel, 0, length($root) + 1, ''); # +1 for leading slash

        say $rel;
    }
}
