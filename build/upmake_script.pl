#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use Getopt::Long;

use FindBin qw($Bin);

use Makefile::Update;
use Makefile::Update::Bakefile0;
use Makefile::Update::CMakefile;
use Makefile::Update::MSBuild;
use Makefile::Update::VCProj;

my $verbose = 0;
my $quiet = 0;
my ($only_bkl, $only_msvs, $only_project, $only_version);

GetOptions(
        'verbose|v'      => \$verbose,
        'quiet|q'        => \$quiet,
        'only-bkl'       => \$only_bkl,
        'only-project=s' => sub { $only_msvs = 1; $only_project = $_[1] },
        'only-version=i' => sub { $only_msvs = 1; $only_version = $_[1] },
    ) or die <<EOF
Usage: $0 [--verbose] [--quiet] [--only-bkl] [--only-project=<name>] [--only-version=<ver>]

Update the files used by bakefile and MSVC projects from the master list
of files in build/files.

If --no-xxx option is specified, the corresponding outputs are not updated.
By default everything is.

The version argument of --only-version can be 8, 9 or 10 with the latter
selecting the MSBuild projects.
EOF
;

if ($only_bkl && $only_msvs) {
    die qq{Options --only-bkl and --only-project or --only-version can't be used together.\n}
}

sub call_upmake
{
    my ($fname, @args) = @_;

    upmake({file => $fname, quiet => $quiet, verbose => $verbose}, @args)
}

open my $files, '<', "$Bin/files";
my $vars = read_files_list($files);

if (!$only_msvs) {
    if (call_upmake("$Bin/bakefiles/files.bkl", \&update_bakefile_0, $vars)) {
        print qq{Don't forget to run "bakefile_gen -b wx.bkl".\n};
    }
}

if (!$only_msvs && !$only_bkl) {
    call_upmake("$Bin/cmake/files.cmake", \&update_cmakefile, $vars);
}

if (!$only_bkl) {
    # Path to the project root directory from the directory containing the
    # projects.
    my $top_srcdir = '../../';

    # The base names of all our projects with the list of variables
    # containing the files that should appear in them.
    my %projects_vars = (
            adv      => [qw(ADVANCED_CMN ADVANCED_MSW ADVANCED_MSW_DESKTOP ADVANCED_MSW_NATIVE)],
            aui      => [qw(AUI_CMN AUI_MSW)],
            base     => [qw(BASE_CMN BASE_AND_GUI_CMN BASE_WIN32 BASE_AND_GUI_WIN32)],
            core     => [qw(BASE_AND_GUI_CMN BASE_AND_GUI_WIN32 MSW_LOWLEVEL MSW_DESKTOP_LOWLEVEL MSW MSW_DESKTOP GUI_CMN)],
            gl       => [qw(OPENGL_CMN OPENGL_MSW)],
            html     => [qw(HTML_CMN HTML_MSW)],
            media    => [qw(MEDIA_CMN MEDIA_MSW MEDIA_MSW_DESKTOP)],
            net      => [qw(NET_CMN NET_WIN32)],
            propgrid => [qw(PROPGRID)],
            qa       => [qw(QA)],
            ribbon   => [qw(RIBBON)],
            richtext => [qw(RICHTEXT)],
            stc      => [qw(STC_CMN)],
            webview  => [qw(WEBVIEW_CMN WEBVIEW_MSW)],
            xml      => [qw(XML)],
            xrc      => [qw(XRC)],
        );

    # The versions of non-MSBuild projects (MSBuild ones all use version "10").
    my @vcproj_versions = qw(8 9);

    # Return the "filter" to use for the given file.
    sub filter_cb
    {
        my ($file) = @_;

        my %filters = (
                'src/common/.*'         => 'Common Sources',
                'src/gtk/.*'            => 'GTK+ Sources',
                'src/msw/.*'            => 'MSW Sources',
                'src/generic/.*'        => 'Generic Sources',
                'src/univ/.*'           => 'wxUniv Sources',
                'src/html/.*'           => 'wxHTML Sources',
                'include/.*/setup.h'    => 'Setup Headers',
                'include/wx/gtk/.*'     => 'GTK+ Headers',
                'include/wx/msw/.*'     => 'MSW Headers',
                'include/wx/generic/.*' => 'Generic Headers',
                'include/wx/univ/.*'    => 'wxUniv Headers',
                'include/wx/html/.*'    => 'wxHTML Headers',
            );

        foreach (keys %filters) {
            return $filters{$_} if $file =~ qr{^${top_srcdir}$_$};
        }

        # Two fall backs which can't be used in the hash as they must be
        # checked after the other patterns.
        return 'Source Files' if $file =~ q{src/.*};
        return 'Common Headers' if $file =~ q{include/wx/.*};

        warn qq{No filter defined for the file "$file".\n};

        undef
    }

    foreach my $proj (sort keys %projects_vars) {
        next if defined $only_project && $proj ne $only_project;

        my (@sources, @headers);

        # All our projects use the special dummy file for PCH creation, but it's
        # not included in the file lists.
        push @sources, "${top_srcdir}src/common/dummy.cpp";

        foreach my $var (@{$projects_vars{$proj}}) {
            # The paths in the files lists are relative to the project root,
            # so add relative path to it from the projects directory.
            push @sources, "${top_srcdir}$_" for @{$vars->{"${var}_SRC"}};

            # It is possible that we don't have any headers of some kind at all.
            if (exists $vars->{"${var}_HDR"}) {
                # Our files lists don't use the full path for the headers, the
                # common "include/" prefix is omitted, add it back here.
                push @headers, "${top_srcdir}include/$_" for @{$vars->{"${var}_HDR"}};
            }
        }

        my @args = (\@sources, \@headers, \&filter_cb);

        # First deal with MSBuild project, it's the simplest case.
        if (!defined $only_version || $only_version == 10) {
            call_upmake("$Bin/msw/wx_${proj}.vcxproj", \&update_msbuild, @args);
            call_upmake("$Bin/msw/wx_${proj}.vcxproj.filters", \&update_msbuild_filters, @args);
        }

        # Pre-MSBuild projects also put this header together with all the other
        # ones, so it needs to be added (with MSBuild it's a <CustomBuild>
        # element and so is completely separate from the real headers that use
        # <ClInclude>).
        push @headers, "${top_srcdir}include/wx/msw/genrcdefs.h";

        # And core project also includes all GUI headers instead of just those
        # it really uses.
        if ($proj eq 'core') {
            foreach my $gui_proj (grep { $_ ne 'base' &&
                                         $_ ne 'core' &&
                                         $_ ne 'net' &&
                                         $_ ne 'xml' } keys %projects_vars) {
                foreach my $var (@{$projects_vars{$gui_proj}}) {
                    push @headers, "${top_srcdir}include/$_" for @{$vars->{"${var}_HDR"}};
                }
            }
        }

        # For compatibility with the existing bakefile-generated projects,
        # don't include *.cpp files in the list of headers -- even these files
        # are actually used as headers (i.e. they are #include'd).
        if ($proj eq 'base') {
            @headers = grep { $_ !~ /\.cpp$/ } @headers;
        }

        foreach my $ver (@vcproj_versions) {
            next if defined $only_version && $ver != $only_version;

            call_upmake("$Bin/msw/wx_vc${ver}_${proj}.vcproj", \&update_vcproj, @args);
        }
    }
}
