#!
#! This TMAKE template - Microsoft Visual C++ 6.0 applications
#!
#${
    if ( Config("qt") || Config("opengl") ) {
	Project('CONFIG += windows');
    }
    if ( Config("qt") ) {
	$moc_aware = 1;
	AddIncludePath(Project('TMAKE_INCDIR_QT'));
	if ( Config("opengl") ) {
	    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_OPENGL');
	}
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT');
    }
    if ( Config("opengl") ) {
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_OPENGL');
    }
    #! wxWindows specific things (added by VZ)
    #! (1) set -I and -L options
    #! (2) be sure we have the same project settings as wxWindows.dsp, here it
    #!     means that we must disable C++ exception handling
    #! (3) define the necessary constants
    if ( Config("wx") ) {
	Project('CONFIG += windows');
    }
    if ( Config("wxbase") ) {
	Project('CONFIG += wx');
    }
    if ( Config("wx") ) {
	#! VC 6.0 supports env vars in include path
	#! $WXDIR = $ENV{'WX'};
	if ( $ENV{'wx'} ) { $WXDIR = "\$(wx)"; }
	else { $WXDIR = "\$(WXWIN)"; }
	$TMAKE_INCDIR_WX = $WXDIR . "\\include";
	AddIncludePath($TMAKE_INCDIR_WX);
    }

    if ( Config("dll") ) {
	$DLL="Dll";
	$DLL_OR_LIB=(Config("wxbase") ? "wxbase" : "wxmsw") . "221";
	$DLL_FLAGS="/D WXUSINGDLL ";
	$EXTRA_LIBS="";
    }
    else {
	$DLL="";
	$DLL_OR_LIB="wxWindows";
	$DLL_FLAGS=" ";
	#! actually this depends on the contents of setup.h
	$EXTRA_LIBS=Config("wxbase") ? "" : "xpm zlib png jpeg tiff";
    }

    #! let's be smarter: first of all, if no extension is given, add .lib
    #! (this allows for LIBS=libname in project files which map either on
    #! -l libname.lib under Windows or on -llibname under Unix).
    @libs = split(/\s+/, Project('LIBS'));
    foreach $lib (@libs) {
	if ( $lib !~ "\.lib\$" ) { $lib .= ".lib"; }
	Project('TMAKE_LIBS *= ' . $lib);
    }

    if ( Config("windows") ) {
	$project{"VC_PROJ_TYPE"} = 'Win32 (x86) Application';
	$project{"VC_PROJ_CODE"} = '0x0101';
	$vc_base_libs = 'kernel32.lib user32.lib gdi32.lib winspool.lib ' .
			'comdlg32.lib advapi32.lib shell32.lib ole32.lib ' .
			'oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ';
	if ( Config("wx") ) {
	    $vc_base_libs .= "comctl32.lib rpcrt4.lib wsock32.lib ";

	    $vc_link_release = "$WXDIR\\Release$DLL\\$DLL_OR_LIB.lib ";
	    $vc_link_debug = "$WXDIR\\Debug$DLL\\$DLL_OR_LIB" . "d.lib ";
	    foreach ( split(/ /, $EXTRA_LIBS) ) {
		$vc_link_release .= "$WXDIR\\src\\$_\\Release\\$_.lib ";
		$vc_link_debug .= "$WXDIR\\src\\$_\\Debug\\$_.lib ";
	    }
	}
	$vc_link_release .= '/nologo /subsystem:windows /machine:I386';
	$vc_link_debug   .= '/nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept';

	$vc_cpp_def_common = '/D "WIN32" /D "_WINDOWS" ' . $DLL_FLAGS;
	$vc_cpp_def_release = '/D "NDEBUG" ' . $vc_cpp_def_common;
	$vc_cpp_def_debug   = '/D "_DEBUG" ' . $vc_cpp_def_common;
    } else {
	$project{"VC_PROJ_TYPE"} = 'Win32 (x86) Console Application';
	$project{"VC_PROJ_CODE"} = '0x0103';
	$vc_base_libs = 'kernel32.lib user32.lib advapi32.lib ';
	if ( Config("wx") ) {
	    $vc_base_libs .= 'wsock32.lib ';
	    $vc_link_release = "$WXDIR\\Base${DLL}Release\\$DLL_OR_LIB.lib ";
	    $vc_link_debug = "$WXDIR\\Base${DLL}Debug\\$DLL_OR_LIB" . "d.lib ";
	}
	$vc_link_release .= '/nologo /subsystem:console /machine:I386';
	$vc_link_debug   .= '/nologo /subsystem:console /debug /machine:I386 /pdbtype:sept';

	$vc_cpp_def_common = '/D "WIN32" /D "_CONSOLE" ' . $DLL_FLAGS;
	$vc_cpp_def_release = '/D "NDEBUG" ' . $vc_cpp_def_common; 
	$vc_cpp_def_debug   = '/D "_DEBUG" ' . $vc_cpp_def_common;
    }
    #! define wxWin debug flags in debug build
    if ( Config("wx") ) {
	$vc_cpp_def_debug .= '/MDd /D "__WXDEBUG__" /D "WXDEBUG=1" ';
	$vc_cpp_def_release .= '/MD '
    }

    $project{"VC_BASE_LINK_RELEASE"} = $vc_base_libs . $vc_link_release;
    $project{"VC_BASE_LINK_DEBUG"}   = $vc_base_libs . $vc_link_debug;
    $tmake_libs = Project('TMAKE_LIBS') ? (Project('TMAKE_LIBS') . " ") : "";
    $project{"VC_LINK_RELEASE"} = $vc_base_libs . $tmake_libs . $vc_link_release;
    $project{"VC_LINK_DEBUG"}   = $vc_base_libs . $tmake_libs . $vc_link_debug;

    $vc_cpp_opt_common1 = '/nologo /W4 ';
    if ( !Config("wx") ) {
	$vc_cpp_opt_common1 = $vc_cpp_opt_common1 . '/GX ';
    }
    #! else: disable C++ exception handling for wxWindows

    $vc_cpp_opt_release = $vc_cpp_opt_common1 . '/O2 ';
    $vc_cpp_opt_debug   = $vc_cpp_opt_common1 . '/Zi /Od ';
    $vc_cpp_opt_common  = '/YX /FD /c';
    $project{"VC_BASE_CPP_RELEASE"} = $vc_cpp_opt_release . $vc_cpp_def_release . $vc_cpp_opt_common;
    $project{"VC_BASE_CPP_DEBUG"}   = $vc_cpp_opt_debug   . $vc_cpp_def_debug   . $vc_cpp_opt_common;
    ExpandGlue("INCPATH",'/I "','" /I "','"');
    if ( $text ne "" ) { $vc_inc = $text . " ";  $text = ""; } else { $vc_inc = ""; }
    ExpandGlue("DEFINES",'/D "','" /D "','"');
    if ( $text ne "" ) { $vc_def = $text . " ";  $text = ""; } else { $vc_def = ""; }
    if ( Config("wx") ) {
	#! define wxWindows compilation flags
	$vc_def .= '/D _WIN32 /D __WIN32__ /D WINVER=0x400 /D __WINDOWS__ /D __WXMSW__ /D __WIN95__ /D __WIN32__ /D _MT ';

	if ( Config("wxbase") ) {
	    $vc_def .= '/D wxUSE_GUI=0 ';
	}
	else {
	    $vc_def .= '/D wxUSE_GUI=1 ';
	}
    }

    $project{"VC_CPP_INCLUDE"} = $vc_inc;
    $project{"VC_CPP_RELEASE"} = $vc_cpp_opt_release . $vc_inc . $vc_cpp_def_release . $vc_def . $vc_cpp_opt_common;
    $project{"VC_CPP_DEBUG"}   = $vc_cpp_opt_debug   . $vc_inc . $vc_cpp_def_debug   . $vc_def . $vc_cpp_opt_common;

    if ( Project('RES_FILE') ) {
	tmake_error(".res files are not supported, use .rc.");
    }

    $project{"MAKEFILE"}  = $project{"PROJECT"} . ".mak";
    $project{"TARGETAPP"} = $project{"TARGET"}  . ".exe";
    Project('TMAKE_FILETAGS = HEADERS SOURCES TARGET DESTDIR $$FILETAGS');
    foreach ( split(/\s/,Project('TMAKE_FILETAGS')) ) {
	$project{$_} =~ s-/-\\-g;
    }
    StdInit();
    if ( defined($project{"DESTDIR"}) ) {
	$project{"TARGETAPP"} = $project{"DESTDIR"} . "\\" . $project{"TARGETAPP"};
	$project{"TARGETAPP"} =~ s/\\+/\\/g;
    }
    %all_files = ();
    @files = split(/\s+/,$project{"HEADERS"});
    foreach ( @files ) { $all_files{$_} = "h" };
    @files = split(/\s+/,$project{"SOURCES"});
    foreach ( @files ) { $all_files{$_} = "s" };
    @files = split(/\s+/,$project{"RC_FILE"});
    foreach ( @files ) { $all_files{$_} = "r" };

    if ( $moc_aware ) {
        @files = split(/\s+/,$project{"_HDRMOC"});
	foreach ( @files ) { $all_files{$_} = "m"; }
	@files = split(/\s+/,$project{"_SRCMOC"});
	foreach ( @files ) { $all_files{$_} = "i"; }
    }
    %file_names = ();
    foreach $f ( %all_files ) {
	$n = $f;
	$n =~ s/^.*\\//;
	$file_names{$n} = $f;
	$file_path{$n}  = ".\\" . $f;
	$file_path2{$n} = (($f =~ /^\./) ? "" : ".\\") . $f;
    }

#$}
# Microsoft Developer Studio Project File - #$ Substitute('Name="$$TARGET" - Package Owner=<4>');
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE #$ Substitute('"$$VC_PROJ_TYPE" $$VC_PROJ_CODE');

CFG=#$ Substitute('$$TARGET - Win32 Debug');
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "#$ ExpandGlue('MAKEFILE','','','".');
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f #$ Substitute('"$$MAKEFILE" CFG="$$TARGET - Win32 Debug"');
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE #$ Substitute('"$$TARGET - Win32 Release" (based on "$$VC_PROJ_TYPE")');
!MESSAGE #$ Substitute('"$$TARGET - Win32 Debug" (based on "$$VC_PROJ_TYPE")');
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
#$ Config("windows") && ($text='MTL=midl.exe');
RSC=rc.exe

!IF  "$(CFG)" == #$ Substitute('"$$TARGET - Win32 Release"');

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLL"
# PROP BASE Intermediate_Dir "ReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
#$ Config("windows") && ($text='# PROP Ignore_Export_Lib 0');
# PROP Target_Dir ""
# ADD BASE CPP #$ Expand("VC_BASE_CPP_RELEASE");
# ADD CPP #$ Expand("VC_CPP_RELEASE");
#$ Config("windows") || DisableOutput();
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
#$ Config("windows") || EnableOutput();
# ADD BASE RSC /l 0x409 /d "NDEBUG" #$ Expand("VC_CPP_INCLUDE");
# ADD RSC /l 0x409 /d "NDEBUG" #$ Expand("VC_CPP_INCLUDE");
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 #$ Expand("VC_BASE_LINK_RELEASE");
# ADD LINK32 #$ Expand("VC_LINK_RELEASE");

!ELSEIF  "$(CFG)" == #$ Substitute('"$$TARGET - Win32 Debug"');

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLL"
# PROP BASE Intermediate_Dir "DebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
#$ Config("windows") && ($text='# PROP Ignore_Export_Lib 0');
# PROP Target_Dir ""
# ADD BASE CPP #$ Expand("VC_BASE_CPP_DEBUG");
# ADD CPP #$ Expand("VC_CPP_DEBUG");
#$ Config("windows") || DisableOutput();
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
#$ Config("windows") || EnableOutput();
# ADD BASE RSC /l 0x409 /d "_DEBUG" #$ Expand("VC_CPP_INCLUDE");
# ADD RSC /l 0x409 /d "_DEBUG" #$ Expand("VC_CPP_INCLUDE");
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 #$ Expand("VC_BASE_LINK_DEBUG");
# ADD LINK32 #$ Expand("VC_LINK_DEBUG");

!ENDIF 

# Begin Target

# Name #$Substitute('"$$TARGET - Win32 Release"');
# Name #$Substitute('"$$TARGET - Win32 Debug"');
#${
    foreach $n ( sort keys %file_names ) {
	$f  = $file_names{$n};
	$p  = $file_path{$n};
	$p2 = $file_path2{$n};
	$t  = $all_files{$f};
	if ( ($t eq "h") && $moc_output{$f} ) {
	    my($build);
	    $build = "\n\n# Begin Custom Build - Running moc...\nInputPath=" . $p2 . "\n\n"
		   . '"' . $moc_output{$f} . '" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"'
		   . "\n\tmoc $p2 -o " . $moc_output{$f} . "\n\n"
		   . "# End Custom Build\n\n";
	    $text .= ("# Begin Source File\n\nSOURCE=$p\n\n"
		   . '!IF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Release"'
		   . $build);
	    $text .= ('!ELSEIF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Debug"'
		   . $build
		   . "!ENDIF \n\n# End Source File\n");
	} elsif ( $t eq "i" ) {
	    my($build,$dn);
	    $build = "\n\n# Begin Custom Build - Running moc...\nInputPath=" . $p2 . "\n\n"
		   . '"' . $f . '" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"'
		   . "\n\tmoc ". $moc_input{$f} . " -o $f\n\n"
		   . "# End Custom Build\n\n";
	    $dn = $n;
	    $dn =~ s/\..*//;
	    $dn =~ tr/a-z/A-Z/;
	    $text .= ("# Begin Source File\n\nSOURCE=$p\n"
		   . "USERDEP__$dn=" . '"' . $moc_input{$f} . '"' . "\n\n"
		   . '!IF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Release"'
		   . $build);
	    $text .= ('!ELSEIF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Debug"'
		   . $build
		   . "!ENDIF \n\n# End Source File\n");
	} elsif ( $t eq "s" || $t eq "m" || $t eq "h" || $t eq "r" ) {
	    $text .= "# Begin Source File\n\nSOURCE=$p\n";
	    $text .= "# End Source File\n";
	}
    }
    chop $text;
#$}
# End Target
# End Project
    
#! vim:sta:sw=4:ts=8:nolist:noet
