#!/usr/bin/env perl

use v5.10;

use strict;
use warnings;

use autodie;
use File::Spec;
use File::Temp::Rename;
use Getopt::Long;
use IO::Handle;

use UUID::Tiny qw(:std);

my $force = 0;
my $verbose = 0;
GetOptions(
        'force|f'        => \$force,
        'verbose|v'      => \$verbose,
    ) and (@ARGV == 1 or @ARGV == 2) or die <<EOF
Usage: $0 [options] <new-project.vcxproj> [<new-project-source-file-name>]

Create a new project by copying the minimal sample .vcxproj file to the given
new file and replacing the minimal.cpp with the specified new source file name
(defaults to the project file name).

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

my ($orig_name, $orig_project) = ('minimal', 'samples/minimal/minimal.vcxproj');
my ($new_name, $new_project) = get_name_and_project(shift @ARGV);

my $new_source_name = @ARGV ? shift @ARGV : $new_name;

if (!$force && -e $new_project) {
    die qq{File "$new_project" already exists, not overwriting without -f.\n}
}

open my $fh_in, '<', $orig_project;
open my $fh_out, '>', $new_project;

say "Cloning $orig_project to $new_project" if $verbose;

my $new_uuid = uc(create_uuid_as_string(UUID_RANDOM));

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

        # And replace the source file name.
        s@<ClCompile Include="$orig_name.cpp" />@<ClCompile Include="$new_source_name.cpp" />@;
    }

    print $fh_out $_;
}

close $fh_out;
close $fh_in;

say "Copying $orig_project.filters to $new_project.filters" if $verbose;

open $fh_in, '<', "$orig_project.filters";
open $fh_out, '>', "$new_project.filters";

while (<$fh_in>) {
    s@<ClCompile Include="$orig_name.cpp">@<ClCompile Include="$new_source_name.cpp">@;

    print $fh_out $_;
}

close $fh_out;
close $fh_in;

say "Adding $new_project to the solution file" if $verbose;

my $solution_file = 'samples/samples_vc17.sln';

my $solution_new = File::Temp::Rename->new(FILE => $solution_file, CLOBBER => 1);

open $fh_in, '<', $solution_file;
open $fh_out, '>', $solution_new;

(my $new_project_relative = $new_project) =~ s@^samples/@@;

while (<$fh_in>) {
    if (/^Global\s*/) {
        print $fh_out <<EOF;
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "$new_name", "$new_project_relative", "{$new_uuid}"\r
\tProjectSection(ProjectDependencies) = postProject\r
\t\t{01F4CE10-2CFB-41A8-B41F-E54337868A1D} = {01F4CE10-2CFB-41A8-B41F-E54337868A1D}\r
\t\t{3FCC50C2-81E9-5DB2-B8D8-2129427568B1} = {3FCC50C2-81E9-5DB2-B8D8-2129427568B1}\r
\t\t{56A4B526-BB81-5D01-AAA9-16D23BBB169D} = {56A4B526-BB81-5D01-AAA9-16D23BBB169D}\r
\t\t{6053CC38-CDEE-584C-8BC8-4B000D800FC7} = {6053CC38-CDEE-584C-8BC8-4B000D800FC7}\r
\t\t{6744DAD8-9C70-574A-BFF2-9F8DDDB24A75} = {6744DAD8-9C70-574A-BFF2-9F8DDDB24A75}\r
\t\t{75596CE6-5AE7-55C9-B890-C07B0A657A83} = {75596CE6-5AE7-55C9-B890-C07B0A657A83}\r
\t\t{8ACC122A-CA6A-5AA6-9C97-9CDD2E533DB0} = {8ACC122A-CA6A-5AA6-9C97-9CDD2E533DB0}\r
\t\t{8B867186-A0B5-5479-B824-E176EDD27C40} = {8B867186-A0B5-5479-B824-E176EDD27C40}\r
\t\t{A1A8355B-0988-528E-9CC2-B971D6266669} = {A1A8355B-0988-528E-9CC2-B971D6266669}\r
\tEndProjectSection\r
EndProject\r
EOF
    }

    print $fh_out $_;
}

close $fh_out;
close $fh_in;

exit 0
