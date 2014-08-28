package Text::Upmake;

use strict;
use warnings;
use autodie;

use Exporter qw(import);

our @EXPORT = qw(read_files_list upmake);

=head1 NAME

Text::Upmake - Update make files.

=head1 SYNOPSIS

=head1 AUTHOR

Vadim Zeitlin

=cut

# Reads the file containing the file lists definitions and returns a hash ref
# with variable names as keys and refs to arrays of the file names as values.
#
# Takes an (open) file handle as argument.
sub read_files_list
{
    my ($fh) = @_;

    my ($var, %vars);
    while (<$fh>) {
        chomp;
        s/#.*$//;
        s/^\s+//;
        s/\s+$//;
        next if !$_;

        if (/^(\w+)\s*=$/) {
            $var = $1;
        } else {
            die "Unexpected contents outside variable definition at line $.\n"
                unless defined $var;
            push @{$vars{$var}}, $_;
        }
    }

    return \%vars;
}

# Update the file with the given name in place using the specified function and
# passing it the rest of the arguments.
#
# This is meant to be used with update_xxx() below.
sub upmake
{
    my ($fname, $updater, @args) = @_;

    my $fname_new = "$fname.upmake.new"; # TODO make it more unique

    open my $in, '<', $fname;
    open my $out, '>', $fname_new;

    my $changed = $updater->($in, $out, @args);

    close $in;
    close $out;

    if ($changed) {
        rename $fname_new, $fname;
    } else {
        unlink $fname_new;
    }

    $changed
}

1;
