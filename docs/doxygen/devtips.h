/////////////////////////////////////////////////////////////////////////////
// Name:        devtips.h
// Purpose:     Cross-platform development page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page multiplatform_page Multi-platform development with wxWidgets

 This chapter describes the practical details of using wxWidgets. Please
 see the file install.txt for up-to-date installation instructions, and
 changes.txt for differences between versions.

 @li @ref includefiles
 @li @ref libraries
 @li @ref configuration
 @li @ref makefiles
 @li @ref windowsfiles
 @li @ref allocatingobjects
 @li @ref architecturedependency
 @li @ref conditionalcompilation
 @li @ref cpp
 @li @ref filehandling

 <hr>


 @section includefiles Include files

 The main include file is {\tt "wx/wx.h"}; this includes the most commonly
 used modules of wxWidgets.

 To save on compilation time, include only those header files relevant to the
 source file. If you are using precompiled headers, you should include
 the following section before any other includes:

 @verbatim
 // For compilers that support precompilation, includes "wx.h".
 #include <wx/wxprec.h>

 #ifdef __BORLANDC__
 #pragma hdrstop
 #endif

 #ifndef WX_PRECOMP
 // Include your minimal set of headers here, or wx.h
 #include <wx/wx.h>
 #endif

 ... now your other include files ...
 @endverbatim

 The file {\tt "wx/wxprec.h"} includes {\tt "wx/wx.h"}. Although this incantation
 may seem quirky, it is in fact the end result of a lot of experimentation,
 and several Windows compilers to use precompilation which is largely automatic for
 compilers with necessary support. Currently it is used for Visual C++ (including
 embedded Visual C++), Borland C++, Open Watcom C++, Digital Mars C++
 and newer versions of GCC.
 Some compilers might need extra work from the application developer to set the
 build environment up as necessary for the support.



 @section libraries Libraries

 Most ports of wxWidgets can create either a static library or a shared
 library. wxWidgets can also be built in multilib and monolithic variants.
 See the \helpref{libraries list}{librarieslist} for more
 information on these.



 @section configuration Configuration

 When using project files and makefiles directly to build wxWidgets,
 options are configurable in the file
 \rtfsp{\tt "wx/XXX/setup.h"} where XXX is the required platform (such as msw, motif, gtk, mac). Some
 settings are a matter of taste, some help with platform-specific problems, and
 others can be set to minimize the size of the library. Please see the setup.h file
 and {\tt install.txt} files for details on configuration.

 When using the 'configure' script to configure wxWidgets (on Unix and other platforms where
 configure is available), the corresponding setup.h files are generated automatically
 along with suitable makefiles. When using the RPM packages
 for installing wxWidgets on Linux, a correct setup.h is shipped in the package and
 this must not be changed.



 @section makefiles Makefiles

 On Microsoft Windows, wxWidgets has a different set of makefiles for each
 compiler, because each compiler's 'make' tool is slightly different.
 Popular Windows compilers that we cater for, and the corresponding makefile
 extensions, include: Microsoft Visual C++ (.vc), Borland C++ (.bcc),
 OpenWatcom C++ (.wat) and MinGW/Cygwin (.gcc). Makefiles are provided
 for the wxWidgets library itself, samples, demos, and utilities.

 On Linux, Mac and OS/2, you use the 'configure' command to
 generate the necessary makefiles. You should also use this method when
 building with MinGW/Cygwin on Windows.

 We also provide project files for some compilers, such as
 Microsoft VC++. However, we recommend using makefiles
 to build the wxWidgets library itself, because makefiles
 can be more powerful and less manual intervention is required.

 On Windows using a compiler other than MinGW/Cygwin, you would
 build the wxWidgets library from the build/msw directory
 which contains the relevant makefiles.

 On Windows using MinGW/Cygwin, and on Unix, MacOS X and OS/2, you invoke
 'configure' (found in the top-level of the wxWidgets source hierarchy),
 from within a suitable empty directory for containing makefiles, object files and
 libraries.

 For details on using makefiles, configure, and project files,
 please see docs/xxx/install.txt in your distribution, where
 xxx is the platform of interest, such as msw, gtk, x11, mac.



 @section windowsfiles Windows-specific files

 wxWidgets application compilation under MS Windows requires at least one
 extra file: a resource file.

 @subsection resources Resource file

 The least that must be defined in the Windows resource file (extension RC)
 is the following statement:

 @verbatim
 #include "wx/msw/wx.rc"
 @endverbatim

 which includes essential internal wxWidgets definitions.  The resource script
 may also contain references to icons, cursors, etc., for example:

 @verbatim
 wxicon icon wx.ico
 @endverbatim

 The icon can then be referenced by name when creating a frame icon. See
 the MS Windows SDK documentation.

 \normalbox{Note: include wx.rc {\it after} any ICON statements
 so programs that search your executable for icons (such
 as the Program Manager) find your application icon first.}



 @section allocatingobjects Allocating and deleting wxWidgets objects

 In general, classes derived from wxWindow must dynamically allocated
 with {\it new} and deleted with {\it delete}. If you delete a window,
 all of its children and descendants will be automatically deleted,
 so you don't need to delete these descendants explicitly.

 When deleting a frame or dialog, use {\bf Destroy} rather than {\bf delete} so
 that the wxWidgets delayed deletion can take effect. This waits until idle time
 (when all messages have been processed) to actually delete the window, to avoid
 problems associated with the GUI sending events to deleted windows.

 Don't create a window on the stack, because this will interfere
 with delayed deletion.

 If you decide to allocate a C++ array of objects (such as wxBitmap) that may
 be cleaned up by wxWidgets, make sure you delete the array explicitly
 before wxWidgets has a chance to do so on exit, since calling {\it delete} on
 array members will cause memory problems.

 wxColour can be created statically: it is not automatically cleaned
 up and is unlikely to be shared between other objects; it is lightweight
 enough for copies to be made.

 Beware of deleting objects such as a wxPen or wxBitmap if they are still in use.
 Windows is particularly sensitive to this: so make sure you
 make calls like wxDC::SetPen(wxNullPen) or wxDC::SelectObject(wxNullBitmap) before deleting
 a drawing object that may be in use. Code that doesn't do this will probably work
 fine on some platforms, and then fail under Windows.



 @section architecturedependency Architecture dependency

 A problem which sometimes arises from writing multi-platform programs is that
 the basic C types are not defined the same on all platforms. This holds true
 for both the length in bits of the standard types (such as int and long) as
 well as their byte order, which might be little endian (typically
 on Intel computers) or big endian (typically on some Unix workstations). wxWidgets
 defines types and macros that make it easy to write architecture independent
 code. The types are:

 wxInt32, wxInt16, wxInt8, wxUint32, wxUint16 = wxWord, wxUint8 = wxByte

 where wxInt32 stands for a 32-bit signed integer type etc. You can also check
 which architecture the program is compiled on using the wxBYTE\_ORDER define
 which is either wxBIG\_ENDIAN or wxLITTLE\_ENDIAN (in the future maybe wxPDP\_ENDIAN
 as well).

 The macros handling bit-swapping with respect to the applications endianness
 are described in the \helpref{Byte order macros}{byteordermacros} section.



 @section conditionalcompilation Conditional compilation

 One of the purposes of wxWidgets is to reduce the need for conditional
 compilation in source code, which can be messy and confusing to follow.
 However, sometimes it is necessary to incorporate platform-specific
 features (such as metafile use under MS Windows). The \helpref{wxUSE\_*}{wxusedef}
 symbols listed in the file {\tt setup.h} may be used for this purpose,
 along with any user-supplied ones.



 @section cpp C++ issues

 The following documents some miscellaneous C++ issues.

 @subsection templates Templates

 wxWidgets does not use templates (except for some advanced features that
 are switched off by default) since it is a notoriously unportable feature.

 @subsection rtti RTTI

 wxWidgets does not use C++ run-time type information since wxWidgets provides
 its own run-time type information system, implemented using macros.

 @subsection null Type of NULL

 Some compilers (e.g. the native IRIX cc) define NULL to be 0L so that
 no conversion to pointers is allowed. Because of that, all these
 occurrences of NULL in the GTK+ port use an explicit conversion such
 as

 {\small
 @verbatim
   wxWindow *my_window = (wxWindow*) NULL;
 @endverbatim
 }%

 It is recommended to adhere to this in all code using wxWidgets as
 this make the code (a bit) more portable.

 @subsection precompiledheaders Precompiled headers

 Some compilers, such as Borland C++ and Microsoft C++, support
 precompiled headers. This can save a great deal of compiling time. The
 recommended approach is to precompile {\tt "wx.h"}, using this
 precompiled header for compiling both wxWidgets itself and any
 wxWidgets applications. For Windows compilers, two dummy source files
 are provided (one for normal applications and one for creating DLLs)
 to allow initial creation of the precompiled header.

 However, there are several downsides to using precompiled headers. One
 is that to take advantage of the facility, you often need to include
 more header files than would normally be the case. This means that
 changing a header file will cause more recompilations (in the case of
 wxWidgets, everything needs to be recompiled since everything includes {\tt "wx.h"}!)

 A related problem is that for compilers that don't have precompiled
 headers, including a lot of header files slows down compilation
 considerably. For this reason, you will find (in the common
 X and Windows parts of the library) conditional
 compilation that under Unix, includes a minimal set of headers;
 and when using Visual C++, includes {\tt wx.h}. This should help provide
 the optimal compilation for each compiler, although it is
 biased towards the precompiled headers facility available
 in Microsoft C++.



 @section filehandling File handling

 When building an application which may be used under different
 environments, one difficulty is coping with documents which may be
 moved to different directories on other machines. Saving a file which
 has pointers to full pathnames is going to be inherently unportable.

 One approach is to store filenames on their own, with no directory
 information. The application then searches into a list of standard
 paths (platform-specific) through the use of \helpref{wxStandardPaths}{wxstandardpaths}.

 Eventually you may want to use also the \helpref{wxPathList}{wxpathlist} class.

 Nowadays the limitations of DOS 8+3 filenames doesn't apply anymore.
 Most modern operating systems allow at least 255 characters in the filename;
 the exact maximum length, as well as the characters allowed in the filenames,
 are OS-specific so you should try to avoid extremely long (> 255 chars) filenames
 and/or filenames with non-ANSI characters.

 Another thing you need to keep in mind is that all Windows operating systems
 are case-insensitive, while Unix operating systems (Linux, Mac, etc) are
 case-sensitive.

 Also, for text files, different OSes use different End Of Lines (EOL).
 Windows uses CR+LF convention, Linux uses LF only, Mac CR only.

 The \helpref{wxTextFile}{wxtextfile},\helpref{wxTextInputStream}{wxtextinputstream},
 \helpref{wxTextOutputStream}{wxtextoutputstream} classes help to abstract
 from these differences.
 Of course, there are also 3rd party utilities such as \tt{dos2unix} and \tt{unix2dos}
 which do the EOL conversions.

 See also the \helpref{File Functions}{filefunctions} section of the reference
 manual for the description of miscellaneous file handling functions.

*/

/*!

 @page utilities_page Utilities and libraries supplied with wxWidgets

 In addition to the \helpref{wxWidgets libraries}{librarieslist}, some
 additional utilities are supplied in the \tt{utils} hierarchy.

 For other user-contributed packages, please see the Contributions page
 on the \urlref{wxWidgets Web site}{http://www.wxwidgets.org}.

 \begin{description}\itemsep=0pt
 \item[{\bf Helpview}]
 Helpview is a program for displaying wxWidgets HTML
 Help files. In many cases, you may wish to use the wxWidgets HTML
 Help classes from within your application, but this provides a
 handy stand-alone viewer. See \helpref{wxHTML Notes}{wxhtml} for more details.
 You can find it in {\tt samples/html/helpview}.
 \item[{\bf Tex2RTF}]
 Supplied with wxWidgets is a utility called Tex2RTF for converting\rtfsp
 \LaTeX\ manuals HTML, MS HTML Help, wxHTML Help, RTF, and Windows
 Help RTF formats. Tex2RTF is used for the wxWidgets manuals and can be used independently
 by authors wishing to create on-line and printed manuals from the same\rtfsp
 \LaTeX\ source. Please see the separate documentation for Tex2RTF.
 You can find it under {\tt utils/tex2rtf}.
 \item[{\bf Helpgen}]
 Helpgen takes C++ header files and generates a Tex2RTF-compatible
 documentation file for each class it finds, using comments as appropriate.
 This is a good way to start a reference for a set of classes.
 Helpgen can be found in {\tt utils/HelpGen}.
 \item[{\bf Emulator}]
 Xnest-based display emulator for X11-based PDA applications. On some
 systems, the Xnest window does not synchronise with the
 'skin' window. This program can be found in {\tt utils/emulator}.
 \end{description}

*/

/*!

 @page strategies_page Programming strategies

 This chapter is intended to list strategies that may be useful when
 writing and debugging wxWidgets programs. If you have any good tips,
 please submit them for inclusion here.

 @li @ref reducingerrors
 @li @ref cpp
 @li @ref portability
 @li @ref debugstrategies

 <hr>

 @section reducingerrors Strategies for reducing programming errors

 @subsection useassert Use ASSERT

 It is good practice to use ASSERT statements liberally, that check for conditions
 that should or should not hold, and print out appropriate error messages.

 These can be compiled out of a non-debugging version of wxWidgets
 and your application. Using ASSERT is an example of `defensive programming':
 it can alert you to problems later on.

 See \helpref{wxASSERT}{wxassert} for more info.

 @subsection usewxstring Use wxString in preference to character arrays

 Using \helpref{wxString}{wxstring} can be much safer and more convenient than using wxChar *.

 You can reduce the possibility of memory leaks substantially, and it is much more
 convenient to use the overloaded operators than functions such as \tt{strcmp}.
 wxString won't add a significant overhead to your program; the overhead is compensated
 for by easier manipulation (which means less code).

 The same goes for other data types: use classes wherever possible.



 @section portability Strategies for portability

 @subsection usesizers Use sizers

 Don't use absolute panel item positioning if you can avoid it. Different GUIs have
 very differently sized panel items. Consider using the \helpref{sizers}{sizeroverview} instead.

 @subsection useresources Use wxWidgets resource files

 Use .xrc (wxWidgets resource files) where possible, because they can be easily changed
 independently of source code. See the \helpref{XRC overview}{xrcoverview} for more info.



 @section debugstrategies Strategies for debugging

 @subsection positivethinking Positive thinking

 It is common to blow up the problem in one's imagination, so that it seems to threaten
 weeks, months or even years of work. The problem you face may seem insurmountable:
 but almost never is. Once you have been programming for some time, you will be able
 to remember similar incidents that threw you into the depths of despair. But
 remember, you always solved the problem, somehow!

 Perseverance is often the key, even though a seemingly trivial problem
 can take an apparently inordinate amount of time to solve. In the end,
 you will probably wonder why you worried so much. That's not to say it
 isn't painful at the time. Try not to worry -- there are many more important
 things in life.

 @subsection simplifyproblem Simplify the problem

 Reduce the code exhibiting the problem to the smallest program possible
 that exhibits the problem. If it is not possible to reduce a large and
 complex program to a very small program, then try to ensure your code
 doesn't hide the problem (you may have attempted to minimize the problem
 in some way: but now you want to expose it).

 With luck, you can add a small amount of code that causes the program
 to go from functioning to non-functioning state. This should give a clue
 to the problem. In some cases though, such as memory leaks or wrong
 deallocation, this can still give totally spurious results!

 @subsection usedebugger Use a debugger

 This sounds like facetious advice, but it is surprising how often people
 don't use a debugger. Often it is an overhead to install or learn how to
 use a debugger, but it really is essential for anything but the most
 trivial programs.

 @subsection uselogging Use logging functions

 There is a variety of logging functions that you can use in your program:
 see \helpref{Logging functions}{logfunctions}.

 Using tracing statements may be more convenient than using the debugger
 in some circumstances (such as when your debugger doesn't support a lot
 of debugging code, or you wish to print a bunch of variables).

 @subsection usedebuggingfacilities Use the wxWidgets debugging facilities

 You can use \helpref{wxDebugContext}{wxdebugcontext} to check for
 memory leaks and corrupt memory: in fact in debugging mode, wxWidgets will
 automatically check for memory leaks at the end of the program if wxWidgets is suitably
 configured. Depending on the operating system and compiler, more or less
 specific information about the problem will be logged.

 You should also use \helpref{debug macros}{debugmacros} as part of a `defensive programming' strategy,
 scattering wxASSERTs liberally to test for problems in your code as early as possible. Forward thinking
 will save a surprising amount of time in the long run.

 See the \helpref{debugging overview}{debuggingoverview} for further information.

*/
