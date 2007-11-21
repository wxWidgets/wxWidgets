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
    my $filename = $ENV{"WXWIN"} . "/include/wx/version.h";
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

    # release number if used in the DLL file names only for the unstable branch
    # as for the stable branches the micro releases are supposed to be
    # backwards compatible and so should have the same name or otherwise it
    # would be impossible to use them without recompiling the applications
    # (which is the whole goal of keeping them backwards compatible in the
    # first place)
    #
    # and the final piece of the puzzle: stable branches are those with even
    # minor version number (and unstable -- with odd)
    $versions{'MICRO_IF_UNSTABLE'} = $versions{'MINOR'} % 2 ? $versions{'MICRO'}
                                                            : "";

    return %versions;
}

