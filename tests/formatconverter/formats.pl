#!/usr/bin/perl -w
#
# Prints test formats for wxFormatConverter. The output is in two columns (tab
# separated), the first is the test input and the second is the expected
# output.
#
# run the output thought formattest like this:
#   ./formats.pl | ./formattest
#
use strict;

my %transform = (
    s  => [ 'l', 's' ],
    S  => [ '',  's' ],
    hS => [ '',  's' ],
    lS => [ 'l', 's' ],
    c  => [ 'l', 'c' ],
    C  => [ '',  'c' ],
    hC => [ '',  'c' ],
    lC => [ 'l', 'c' ]
);

print "%%\t%%\n";

for my $type ('d', 's', 'S', 'c', 'C')
{
    for my $mod ('', 'h', 'l', 'hh', 'll', 'j') #, 'z', 't', 'L')
    {
        for my $prec ('', '.*', '.10')
        {
            for my $width ('', '*', '10')
            {
                for my $flag ('', '#') #, '0', '-', ' ', '+' )
                {
                    my ($newmod, $newtype) = ($mod, $type);

                    if ($transform{$mod.$type}) {
                        ($newmod, $newtype) = @{$transform{$mod.$type}};
                    }

                    print "%$flag$width$prec$mod$type\t".
                          "%$flag$width$prec$newmod$newtype\n";
                }
            }
        }
    }
}
