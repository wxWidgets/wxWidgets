package wxVersion;

=head1 NAME

wxVersion

=head1 SYNOPSIS

  use wxVersion qw(GetVersion);

=head1 METHODS

=cut

use strict;

use base 'Exporter';
use vars qw(@EXPORT_OK);

@EXPORT_OK = qw(GetVersion);

sub GetVersion()
{
    my $filename = "../../../include/wx/version.h";
    open(VERSION_H, $filename) or die "Can't open $filename: $!\n";

    my %versions;
    my $numGot = 0;

    while ( defined($_ = <VERSION_H>) ) {
        chomp;

        if ( /\s*#define\s+wxMAJOR_VERSION\s+(\d+)/ ) {
            $versions{'MAJOR'} = $1;
            $numGot++;
        }
        elsif ( /\s*#define\s+wxMINOR_VERSION\s+(\d+)/ ) {
            $versions{'MINOR'} = $1;
            $numGot++;
        }
        elsif ( /\s*#define\s+wxRELEASE_NUMBER\s+(\d+)/ ) {
            $versions{'MICRO'} = $1;
            $numGot++;
        }

        last if $numGot == 3    # we've got everything we wanted
    }

    $numGot == 3 or die "Failed to read the version from $filename.\n";

    return %versions;
}

