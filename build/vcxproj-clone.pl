#!/usr/bin/env perl

use v5.10;

use strict;
use warnings;

use autodie;
use File::Copy qw(copy);
use File::Spec;
use Getopt::Long;
use IO::Handle;

use UUID::Tiny qw(:std);

my $force = 0;
my $verbose = 0;
GetOptions(
        'force|f'        => \$force,
        'verbose|v'      => \$verbose,
    ) and (@ARGV == 2) or die <<EOF
Usage: $0 [options] <existing-project.vcxproj> <new-name>

Clone an existing MSBuild project file with .vcxproj extension by creating a
new project with the given name.

Options:
    -f, --force         Overwrite the file with the new name even if it exists.
    -v, --verbose       Show more details about what the program is doing.
EOF
    ;

# Show progress messages immediately.
STDOUT->autoflush(1) if $verbose;

# Returns just the name of the project, i.e. the part without any path nor
# extension and the full path to it, which will always have .vcxproj extension
# whether it is already present in input or not.
sub get_name_and_project
{
    my ($file) = @_;

    # If a .filters file was passed on input by mistake, accept it as if the
    # project itself was given.
    $file =~ s/\.filters$//;

    my ($volume, $dir, $name) = File::Spec->splitpath($file);

    if ($file !~ /\.vcxproj$/) {
        $file .= '.vcxproj';
    } else {
        $name =~ s/\.vcxproj$//;
    }

    return ($name, $file)
}

my ($orig_name, $orig_project) = get_name_and_project(shift @ARGV);
my ($new_name, $new_project) = get_name_and_project(shift @ARGV);

if (!$force && -e $new_project) {
    die qq{File "$new_project" already exists, not overwriting without -f.\n}
}

open my $fh_in, '<', $orig_project;
open my $fh_out, '>', $new_project;

say "Cloning $orig_project to $new_project" if $verbose;

# Don't bother with parsing XML, just mangle the text directly.
while (<$fh_in>) {
    # Note: don't use chomp to preserve the same kind of EOLs.
    if (m|
            ^
            (?<leading_space>\s*)
            <ProjectGuid>
            \{
                (?<orig_uuid>[[:xdigit:]]{8}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{12})
            \}
            </ProjectGuid>
            (?<trailing_space>\s*)
            $
         |x) {
        my $new_uuid = uc(create_uuid_as_string(UUID_RANDOM));
        say qq(Replacing old GUID "$+{orig_uuid}" with "$new_uuid".);
        $_ = "$+{leading_space}<ProjectGuid>{$new_uuid}</ProjectGuid>$+{trailing_space}"
    } else {
        # Replace the project name for the tags that use it.
        s@
            ^
            (\s*)
            \K
            <(?<tag>ProjectName|IntDir|\w+File(?:Name)?)>
            (?<before>.*)\Q$orig_name\E(?<after>.*)
            </\g{tag}>
            (?<trailing_space>\s*)
            $
         @<$+{tag}>$+{before}$new_name$+{after}</$+{tag}>$+{trailing_space}@ix;
    }

    print $fh_out $_;
}

close $fh_out;

say "And copying $orig_project.filters to $new_project.filters" if $verbose;
copy("$orig_project.filters", "$new_project.filters") or
    die qq{Failed to copy "$orig_project.filters" to "$new_project.filters: $!\n};

exit 0
