# File:     mwvars.sh
# Author:   David Elliott
# Id:       $Id$
CWINSTALL="/Applications/Metrowerks CodeWarrior 8.0/Metrowerks CodeWarrior"
if ! test -d "$CWINSTALL"; then
    CWINSTALL="$HOME/Applications/Metrowerks CodeWarrior 8.0/Metrowerks CodeWarrior"
fi
if ! test -d "$CWINSTALL"; then
    echo "WARNING: Could not find CodeWarrior" 1>&2
fi

# NOTE: Do not use this with CW < 9.3
# If you do have CW 9.3, please read the documentation.before
# attempting this as I don't have it so haven't tested it.
#CW_NEXT_ROOT="/Developer/SDKs/MacOSX10.2.8.sdk"
CW_NEXT_ROOT=""

# Turn this on to use MW's CRT instead of BSD CRT.w/ mwcc/mwld
CW_MWCC_USE_MW_CRT=no
# NOTE: When you turn this on you MUST define _MSL_USING_MW_C_HEADERS
# You probably also want to define _MSL_NEEDS_EXTRAS (for strcasecmp)
# Furthermore, you can't use UNIXy stuff like popen when you use these!
# That is why it is off by default.

###########################################################################
## Metrowerks Mac OS X and Mach-O

##### MWCMachPPCIncludes #####
# Path containing machine/ansi.h fixed to work with Panther headers
# standard /usr/include directory
# MSL C++ headers
# MSL C headers (MSL C++ needs this for mslGlobals.h in particular)
# MW compiler specific headers
export MWCMachPPCIncludes=\
"$CWINSTALL/MacOS X Support/Headers/(wchar_t Support Panther fix):"\
"$CW_NEXT_ROOT/usr/include:"\
"$CWINSTALL/MSL/MSL_C++/MSL_Common/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_Common/Include:"\
"$CWINSTALL/MacOS X Support/Headers/PPC Specific:"\
 #end MWCMachPPCIncludes

##### MWCMacOSXPPCIncludes #####
# If using BSD CRT then our includes are like those for the Mach-O compiler.
export MWCMacOSXPPCIncludes="$MWCMachPPCIncludes"
if test "x$CW_MWCC_USE_MW_CRT" = "xyes"; then
# Path containing machine/ansi.h fixed to work with Panther headers
# MSL C++ and C headers
# standard /usr/include directory
# MSL Extra headers (extras.h defines strcasecmp!)
# MW compiler specific headers
#NOTE: MSL Extras must be included after /usr/include so mode_t
#      doesn't get redefined incorrectly for OS X.
    export MWCMacOSXPPCIncludes=\
"$CWINSTALL/MacOS X Support/Headers/(wchar_t Support Panther fix):"\
"$CWINSTALL/MSL/MSL_C++/MSL_Common/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_Common/Include:"\
"$CW_NEXT_ROOT/usr/include:"\
"$CWINSTALL/MSL/MSL_Extras/MSL_Common/Include:"\
"$CWINSTALL/MacOS X Support/Headers/PPC Specific"
 #end MWCMacOSXPPCIncludes
fi

##### MWMacOSXPPCLibraries #####
# First we must have the path containing mwcrt1.o (see note)
# Then we have the normal /usr/lib.
#
# NOTE: For CW 8.3 users using Panther you must link mwcrt1.o to crt1.o
# so that the crt1.o from /usr/lib does not get used.
export MWMacOSXPPCLibraries=\
"$CWINSTALL/MacOS X Support/Libraries/Startup:"\
"$CW_NEXT_ROOT/usr/lib"
 #end MWMacOSXPPCLibraries

##### MWMachPPCLibraries #####
# Mach-O/PPC linker uses the same system library path as Mac OS X/PPC
export MWMachPPCLibraries="$MWMacOSXPPCLibraries"

##### MWMachPPCLibraryFiles #####
# First we need the MSL Runtime for basic C++ support.
# Second we need the MSL C++ library.
# There doesn't appear to be any MSL C for this configuration.
export MWMachPPCLibraryFiles=\
"$CWINSTALL/MacOS X Support/Libraries/Runtime/Libs/MSL_Runtime_Mach-O.a:"\
"$CWINSTALL/MSL/MSL_C++/MSL_MacOS/Lib/Mach-O/MSL_C++_Mach-O.a:"\
 #end MWMachPPCLibraryFiles


##### MWMacOSXPPCLibraryFiles #####
export MWMacOSXPPCLibraryFiles="$MWMachPPCLibraryFiles"
if test "x$CW_MWCC_USE_MW_CRT" = "xyes"; then
# First of all we need the MSL Runtime which appears to implement
# the very basic C++ functions like operator new/delete as well
# as Metrowerks C++ standard RTTI.
# Second we need the MSL C++ library.
# Since we're using MSL C++ library we also need the C library.
# Finally, we need the console_OS_X.c file which you'll need to compile
# for this to work (a normal project would have console_OS_X.c in it)
    export MWMacOSXPPCLibraryFiles=\
"$CWINSTALL/MacOS X Support/Libraries/Runtime/Libs/MSL_Runtime_Mach-O.lib:"\
"$CWINSTALL/MSL/MSL_C++/MSL_MacOS/Lib/Mach-O/MSL_C++_Mach-O.lib:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Lib/Mach-O/MSL_C_Mach-O.lib:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Src/console_OS_X.c.o:"\
 #end MWMacOSXPPCLibraryFiles
fi

##### MWFrameworkPaths #####
# We need /System/Library/Frameworks for the OS X stuff
export MWFrameworkPaths=\
"$CW_NEXT_ROOT/System/Library/Frameworks"
 #end MWFrameworkPaths


###########################################################################
## PEF
export MWPEFCIncludes=\
"$CWINSTALL/MSL/MSL_C++/MSL_Common/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Include:"\
"$CWINSTALL/MSL/MSL_C/MSL_Common/Include:"\
"$CWINSTALL/MSL/MSL_Extras/MSL_Common/Include:"\
"$CWINSTALL/MSL/MSL_Extras/MSL_MacOS/Include:"\
"$CWINSTALL/MacOS Support/Headers/PPC Specific:"\
"$CWINSTALL/MacOS Support/Universal/Interfaces/CIncludes:"\
"$CWINSTALL/MacOS Support/MetroNub Utilities/:"\
 #end MWPEFCIncludes

export MWPEFLibraries=\
"$CWINSTALL"/MacOS\ Support/Universal/Libraries/StubLibraries:\
"$CWINSTALL"/MacOS\ Support/Libraries/Runtime/Libs:\
"$CWINSTALL"/MSL/MSL_C/MSL_MacOS/Lib/PPC:\
"$CWINSTALL"/MSL/MSL_C++/MSL_MacOS/Lib/PPC
 #end MWPEFLibraries

export MWPEFLibraryFiles=\
"$CWINSTALL/MacOS Support/Libraries/Runtime/Libs/MSL_All_Carbon.Lib:"\
"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Src/console.stubs.o:"\
"$CWINSTALL/MacOS Support/MetroNub Utilities/MNU Carbon.Lib:"\
"CarbonLib:"\
"CarbonFrameworkLib:"\
 #end MWPEFLibraryFiles
# I don't need the following but here they are for reference:
#"$CWINSTALL/MacOS Support/Libraries/Runtime/Libs/MSL_StdCRuntime_PPC.lib:"\
#"$CWINSTALL/MacOS Support/Libraries/Runtime/Libs/MSL_Runtime_PPC.lib:"\
#"$CWINSTALL/MSL/MSL_C++/MSL_MacOS/Lib/PPC/MSL_C++_PPC.lib:"\
#"$CWINSTALL/MSL/MSL_C/MSL_MacOS/Lib/PPC/MSL_C_PPC.lib:"\

