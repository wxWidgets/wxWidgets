#!#############################################################################
#! File:    filelist.t
#! Purpose: tmake template file containig Perl code to parse the filelist.txt
#!          file - this is used by all other templates.
#! Author:  Vadim Zeitlin
#! Created: 14.07.99
#! Version: $Id$
#!#############################################################################
#${
    open(FILELIST, "filelist.txt") or die "Can't open filelist file: $!\n";

    line: while ( defined($_ = <FILELIST>) ) {
        chomp;

        #! comment or blank line, skip
        next line if ( $_ eq "" or /^#/ );

        #! if ( $verbose ) {
        #!     print STDERR "Processing line: '$_'\n";
        #! }

        my @fields = split "\t";
        if ( $#fields > 2 ) {
            warn "Ignoring malformed line $_ in the filelist file.\n";
            next line;
        } elsif ( $#fields == 1 ) {
            #! add an empty flags string
            $fields[2] = "";
        }

        if ( $verbose ) {
            print STDERR "File $fields[0]: type '$fields[1]', flags '$fields[2]'\n";
        }

        #! first column is filename, second is type, third is flags
        if ( $fields[1] eq "C" ) {
            $wxCommon{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "G" ) {
            $wxGeneric{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "M" ) {
            $wxMSW{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "X" ) {
            $wxMOTIF{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "R" ) {
            $wxGTK{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "H" ) {
            $wxHTML{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "U" ) {
            $wxUNIX{$fields[0]} = $fields[2];
        } elsif ( $fields[1] eq "I" ) {
            $wxINCLUDE{$fields[0]} = $fields[2];
        } else {
            warn "Unknown file type $fields[1] for $fields[0], ignoring.\n";
            next line;
        }
    }

    close(FILELIST);
#$}
#! vim:sw=4:ts=4:list:et
