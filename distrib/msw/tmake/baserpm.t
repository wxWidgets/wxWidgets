#!################################################################################
#! File:    base.t
#! Purpose: tmake template file from which src/rpmfiles.lst containing the
#!          list of files for wxBase.spec RPM generation script is generated
#! Author:  Vadim Zeitlin
#! Created: 28.01.00
#! Version: $Id$
#!################################################################################
#${
    IncludeTemplate("filelist.t");

    foreach $file (sort keys %wxWXINCLUDE) {
        next unless $wxWXINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "include/wx/" . $file . " "
    }

    foreach $file (sort keys %wxUNIXINCLUDE) {
        next unless $wxWXINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "include/wx/unix/" . $file . " "
    }

    foreach $file (sort keys %wxPROTOCOLINCLUDE) {
        next unless $wxPROTOCOLINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "include/wx/protocol/" . $file . " "
    }
#$}
#$ ExpandGlue("BASE_HEADERS", "/usr/", "\n/usr/", "");
