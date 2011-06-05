/////////////////////////////////////////////////////////////////////////////
// Name:        compiling.h
// Purpose:     Compilation page for the wxMobile Doxygen manual
// Author:      Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_compiling Compiling

Currently the following sample is available:

@li samples/mobile/demo: The main wxMobile sample

There is no proper build system as yet but the sample has a DialogBlocks
project file (.pjd) which can be used along with the DialogBlocks demo
to create makefiles and project files for most compilers. The projects
simply include all the required files and create a statically linked
executable.

You need to install wxWidgets 2.8.8 or above. In DialogBlocks,
select a suitable project configuration or create your own. Then
in Settings/C++ Generation, set 'Target wxWidgets version' to
the appropriate installed version, such as 2.8.10.

If you're using VC++, go to Configurations and for each
VC++ Project configuration, set the 'Compiler version' property to
a suitable number (for example, 6 for VC++ 6, or 8 for Visual Studio 2005).
Then press OK and press Build | Generate Makefile. Now you can either build
the project from DialogBlocks, or open the project file from Visual Studio
and build from there.

DialogBlocks can compile wxWidgets to match the sample project settings.
If the correct version of wxWidgets is not found, it will automatically
ask whether you wish to build it.

*/
