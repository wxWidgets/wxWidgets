#!#############################################################################
#! File:    filelist.t
#! Purpose: tmake template file containig Perl code to parse the filelist.txt
#!          file - this is used by all other templates.
#! Author:  Vadim Zeitlin
#! Created: 14.07.99
#! Version: $Id$
#!#############################################################################
#${
    use lib './lib';
    use wxFileInfo;

    open(FILELIST, "filelist.txt") or die "Can't open filelist file: $!\n";

    #! maps file types to array names, for example an entry of the form
    #!      FooH => 'wxXYZ' means that all files with type "FooH" will be
    #! added to an array named @wxXYZ
    my %type_2_array = (
                         Common    => "wxCommon",
                         Generic   => "wxGeneric",
                         GenericH  => "wxGenericInclude",
                         HTML      => "wxHtml",
                         HtmlH     => "wxHtmlInclude",
                         Motif     => "wxMotif",
                         MotifH    => "wxMotifInclude",
                         ProtoH    => "wxProtocolInclude",
                         Unix      => "wxUnix",
                         UnixH     => "wxUnixInclude",
                         WXH       => "wxWxInclude",
                       );

    line: while ( defined($_ = <FILELIST>) ) {
        chomp;

        #! comment or blank line, skip
        next line if ( $_ eq "" or /^#/ );

        #! if ( $verbose ) {
        #!      print STDERR "Processing line: '$_'\n";
        #! }

        my @fields = split /\t/;

        #! first column is filename, second is type, third is flags
        my ($filename, $filetype, $fileflags) = @fields;

        if ( $#fields > 2 ) {
            warn "Ignoring malformed line $_ in the filelist file.\n";
            next line;
        } elsif ( $#fields == 1 ) {
            #! add an empty flags string
            $fileflags = "";
        }

        if ( $verbose ) {
            print STDERR "File $filename: type '$filetype', flags '$fileflags'\n";
        }

        #! save all information in @wxALL
        my $fileinfo = new wxFileInfo( $filename, $filetype, $fileflags );
        push @wxALL, $fileinfo;

        #! this is a bit stupid but all templates are written using the old
        #! single letter flags which became so unreadable that I decided to
        #! replace them with more readable strings, but it was easier to do
        #! the translation here instead of changing all *.t files
        $fileflags =~ s/Base/B/;
        $fileflags =~ s/NotWin32/16/;
        $fileflags =~ s/Win32Only/32/;
        $fileflags =~ s/Generic/G/;
        $fileflags =~ s/OLE/O/;
        $fileflags =~ s/Socket/S/;
        $fileflags =~ s/NotMSW/U/;
        $fileflags =~ s/NotOS2/P/;
        $fileflags =~ s/LowLevel/L/;
        $fileflags =~ s/Theme/T/;

        if ( $filetype eq "Common" ) {
            $wxCommon{$filename} = $fileflags;
        } elsif ( $filetype eq "Generic" ) {
            $wxGeneric{$filename} = $fileflags;
        } elsif ( $filetype eq "MSW" ) {
            $wxMSW{$filename} = $fileflags;
        } elsif ( $filetype eq "Mac" ) {
            $wxMAC{$filename} = $fileflags;
        } elsif ( $filetype eq "Cocoa" ) {
            $wxCOCOA{$filename} = $fileflags;
        } elsif ( $filetype eq "Motif" ) {
            $wxMOTIF{$filename} = $fileflags;
        } elsif ( $filetype eq "GTK" ) {
            $wxGTK{$filename} = $fileflags;
        } elsif ( $filetype eq "Univ" ) {
            $wxUNIV{$filename} = $fileflags;
        } elsif ( $filetype eq "MGL" ) {
            $wxMGL{$filename} = $fileflags;
        } elsif ( $filetype eq "Micro" ) {
            $wxMICRO{$filename} = $fileflags;
        } elsif ( $filetype eq "OS2" ) {
            $wxOS2PM{$filename} = $fileflags;
        } elsif ( $filetype eq "X11" ) {
            $wxX11{$filename} = $fileflags;
        } elsif ( $filetype eq "HTML" ) {
            $wxHTML{$filename} = $fileflags;
        } elsif ( $filetype eq "Unix" ) {
            $wxUNIX{$filename} = $fileflags;
        } elsif ( $filetype eq "BaseOnly" ) {
            $wxBase{$filename} = $fileflags;
        } elsif ( $filetype eq "WXH" ) {
            $wxWXINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "ProtoH" ) {
            $wxPROTOCOLINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "HtmlH" ) {
            $wxHTMLINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MacH" ) {
            $wxMACINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "CocoaH" ) {
            $wxCOCOAINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MotifH" ) {
            $wxMOTIFINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MSWH" && $fileflags =~ m/O/ ) {
            $wxOLEINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MSWH" ) {
            $wxMSWINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "GTKH" ) {
            $wxGTKINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "OS2H" ) {
            $wxOS2PMINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MGLH" ) {
            $wxMGLINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "X11H" ) {
            $wxX11INCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "UnivH" ) {
            $wxUNIVINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "UnixH" ) {
            $wxUNIXINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "GenericH" ) {
            $wxGENERICINCLUDE{$filename} = $fileflags;
        } elsif ( $filetype eq "MacR" ) {
            $wxMACRESOURCE{$filename} = $fileflags;
        } elsif ( $filetype eq "CocoaR" ) {
            $wxCOCOARESOURCE{$filename} = $fileflags;
        } else {
            warn "Unknown file type $filetype for $filename, ignoring.\n";
            next line;
        }
    }

    close(FILELIST);
#$}
#! vim:sw=4:ts=4:list:et:ft=perl
