package Text::Upmake::MSBuild;

use Exporter qw(import);
our @EXPORT = qw(update_msbuild update_msbuild_filters);

=head1 NAME

Text::Upmake::MSBuild - Update list of sources and headers in MSBuild projects.

=head1 SYNOPSIS

Given an MSBuild project C<project.vcxproj> and its associated filters file
C<projects.vcxproj.filters>, the functions in this module can be used to update
the list of files in them to correspond to the given ones.

    use Text::Upmake::Bakefile0;
    Text::Upmake::upmake('projects.vcxproj', \&update_msbuild, \@sources, \@headers);
    Text::Upmake::upmake('projects.vcxproj.filters', \&update_msbuild_filters, \@sources, \@headers);

=head1 SEE ALSO

Text::Upmake

=head1 AUTHOR

Vadim Zeitlin

=cut

# Update sources and headers in an MSBuild project.
#
# Parameters: input and output file handles and array references to the sources
# and the headers to be used in this project.
#
# Returns 1 if any changes were made.
sub update_msbuild
{
    my ($in, $out, $sources, $headers) = @_;

    # Hashes mapping the sources/headers names to 1 if they have been seen in
    # the project or 0 otherwise.
    my %sources = map { $_ => 0 } @$sources;
    my %headers = map { $_ => 0 } @$headers;

    # Reference to the hash corresponding to the files currently being
    # processed.
    my $files;

    # Set to 1 when we are inside any <ItemGroup> tag.
    my $in_group = 0;

    # Set to 1 when we are inside an item group containing sources or headers
    # respectively.
    my ($in_sources, $in_headers) = 0;

    # Set to 1 if we made any changes.
    my $changed = 0;
    while (my $line_with_eol = <$in>) {
        (my $line = $line_with_eol) =~ s/\r?\n?$//;

        if ($line =~ /^\s*<ItemGroup>$/) {
            $in_group = 1;
        } elsif ($line =~ m{^\s*</ItemGroup>$}) {
            if (defined $files) {
                my $kind = $in_sources ? 'Compile' : 'Include';

                # Check if we have any new files.
                #
                # TODO Insert them in alphabetical order.
                while (my ($file, $seen) = each(%$files)) {
                    if (!$seen) {
                        # Convert path separator to the one used by MSBuild.
                        $file =~ s@/@\\@g;

                        print $out qq{    <Cl$kind Include="$file" />\n};

                        $changed = 1;
                    }
                }

                $in_sources = $in_headers = 0;
            }

            $in_group = 0;
        } elsif ($in_group) {
            if ($line =~ m{^\s*<Cl(?<kind>Compile|Include) Include="(?<file>[^"]+)"\s*(?<slash>/)?>$}) {
                if ($+{kind} eq 'Compile') {
                    warn "Mix of sources and headers at line $.\n" if $in_headers;
                    $in_sources = 1;
                    $files = \%sources;
                } else {
                    warn "Mix of headers and sources at line $.\n" if $in_sources;
                    $in_headers = 1;
                    $files = \%headers;
                }

                my $closed_tag = defined $+{slash};

                # Normalize the path separator, we always use Unix ones but the
                # project files use Windows one.
                my $file = $+{file};
                $file =~ s@\\@/@g;

                if (not exists $files->{$file}) {
                    # This file was removed.
                    $changed = 1;

                    if (!$closed_tag) {
                        # We have just the opening <ClCompile> tag, ignore
                        # everything until the next </ClCompile>
                        while (<$in>) {
                            last if m{^\s*</ClCompile>$};
                        }
                    }

                    # In any case skip either this line containing the full
                    # <ClCompile/> tag or the line with the closing tag.
                    next;
                } else {
                    if ($files->{$file}) {
                        warn qq{Duplicate file "$file" in the project at line $.\n};
                    } else {
                        $files->{$file} = 1;
                    }
                }
            }
        }

        print $out $line_with_eol;
    }

    $changed
}

# Update sources and headers in an MSBuild filters file.
#
# Parameters: input and output file handles, array references to the sources
# and the headers to be used in this project and a callback used to determine
# the filter for the new files.
#
# Returns 1 if any changes were made.
sub update_msbuild_filters
{
    my ($in, $out, $sources, $headers, $filter_cb) = @_;

    # Hashes mapping the sources/headers names to the text representing them in
    # the input file if they have been seen in it or nothing otherwise.
    my %sources = map { $_ => undef } @$sources;
    my %headers = map { $_ => undef } @$headers;

    # Reference to the hash corresponding to the files currently being
    # processed.
    my $files;

    # Set to 1 when we are inside any <ItemGroup> tag.
    my $in_group = 0;

    # Set to 1 when we are inside an item group containing sources or headers
    # respectively.
    my ($in_sources, $in_headers) = 0;

    # Set to 1 if we made any changes.
    my $changed = 0;
    while (my $line_with_eol = <$in>) {
        (my $line = $line_with_eol) =~ s/\r?\n?$//;

        if ($line =~ /^\s*<ItemGroup>?$/) {
            $in_group = 1;
        } elsif ($line =~ m{^\s*</ItemGroup>?$}) {
            if (defined $files) {
                # Output the group contents now, all at once, inserting any new
                # files: we must do it like this to ensure that they are
                # inserted in alphabetical order.
                my $kind = $in_sources ? 'Compile' : 'Include';

                foreach my $file (sort keys %$files) {
                    if (defined $files->{$file}) {
                        print $out $files->{$file};
                    } else {
                        my $filter = defined $filter_cb ? $filter_cb->($file) : undef;

                        # Convert path separator to the one used by MSBuild.
                        $file =~ s@/@\\@g;

                        my $indent = ' ' x 2;

                        print $out qq{$indent$indent<Cl$kind Include="$file"};
                        if (defined $filter) {
                            print $out ">\n$indent$indent$indent<Filter>$filter</Filter>\n$indent$indent</Cl$kind>\n";
                        } else {
                            print $out " />\n";
                        }

                        $changed = 1;
                    }
                }

                $in_sources = $in_headers = 0;
                $files = undef;
            }

            $in_group = 0;
        } elsif ($in_group &&
                 $line =~ m{^\s*<Cl(?<kind>Compile|Include) Include="(?<file>[^"]+)"\s*(?<slash>/)?>?$}) {
            my $kind = $+{kind};
            if ($kind eq 'Compile') {
                warn "Mix of sources and headers at line $.\n" if $in_headers;
                $in_sources = 1;
                $files = \%sources;
            } else {
                warn "Mix of headers and sources at line $.\n" if $in_sources;
                $in_headers = 1;
                $files = \%headers;
            }

            my $closed_tag = defined $+{slash};

            # Normalize the path separator, we always use Unix ones but the
            # project files use Windows one.
            my $file = $+{file};
            $file =~ s@\\@/@g;

            my $text = $line_with_eol;
            if (!$closed_tag) {
                # We have just the opening <ClCompile> tag, get everything
                # until the next </ClCompile>.
                while (<$in>) {
                    $text .= $_;
                    last if m{^\s*</Cl$kind>\r?\n?$};
                }
            }

            if (not exists $files->{$file}) {
                # This file was removed.
                $changed = 1;
            } else {
                if ($files->{$file}) {
                    warn qq{Duplicate file "$file" in the project at line $.\n};
                } else {
                    $files->{$file} = $text;
                }
            }

            # Don't output this line yet, wait until the end of the group.
            next
        }

        print $out $line_with_eol;
    }

    $changed
}
