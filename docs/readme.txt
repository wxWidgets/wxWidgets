wxWindows 2.1.11
----------------

Welcome to wxWindows 2, a sophisticated cross-platform C++
framework for writing advanced GUI applications using (where
possible) the native controls.

In addition to common and advanced GUI facilities such as frames, scrolling
windows, toolbars, tree controls, icons, device contexts, printing,
splitter windows and so on, there are wrappers for common file operations,
and facilities for writing TCP/IP applications, thread handling, and more.
Where certain features are not available on a platform, such as MDI and
tree controls on Unix, they are emulated.

A detailed 1100-page reference manual is supplied in HTML, PDF and
Windows Help form: see the docs hierarchy.

For a quick start, point your Web browser at docs/html/index.htm for a list of
important documents and samples.

Changes in this release
-----------------------

These are some of the major improvements:

wxWindows 2.1.11
================

- Numerous bug fixes and consistency improvements.
- Further samples.
- Factoring out of some code into base classes, for easier
  maintenance.
- Ability to compile library in console (non-GUI) mode.
- Integration of wxHTML widget and help controller into
  wxWindows. wxHTML allows HTML viewing and printing (wxGTK
  and wxMSW, partial support in wxMotif).
- New classes wxChrono, wxDialUpManager, wxFontEnumerator,
  wxWizard, wxStaticLine, etc.
- wxShowTip for showing 'startup tips' to the user.
- wxSocket and wxThread classes rewritten.
- New, consistent drag and drop API (not wxMotif).
- Better-looking dialogs in wxGTK; native message box used
  in wxMotif.
- wxSizers reimplemented and used to specify window layout
  for some generic dialogs.
- New, more sophisticated wxGrid class (in beta). The old grid
  class can still be used.
- Text alignment options in wxStaticText.
- wxImage class extended to read PCX and GIF files.
- Documentation improvements.
- Revised configure system for wxGTK and wxMotif; tmake-based
  system for generating wxMSW makefiles.
- Installer for Windows as alternative to zip archives.

Platforms supported
-------------------

wxWindows 2 currently supports the following platforms:

- Windows 3.1, Windows 95/98, Windows NT
- Most Unix variants with Motif/Lesstif
- Most Unix variants with GTK+
- Mac (not covered here: please see the wxWindows web site
  for details)

Most popular C++ compilers are supported; see the release notes
(available via docs/html/index.htm) for details.

Files
-----

The distribution is available in a number of formats.
The most convenient is the platform-specific distribution,
but the zip set forms a more complete distribution, for example if
you wish to take part in wxWindows development. Also, add-ons
such as OGL may only be available in zip form.

Zip set
-------

Depending on what you downloaded, you may have one or more of
these ports. You may unarchive any or all of the ports into
the same directory hierarchy. The zip archive set comprises the
following, where x is the minor version and y the release number:

wx2_x_y_gen.zip            Generic source code and samples (required)
wx2_x_y_msw.zip            Windows-specific source code
wx2_x_y_mot.zip            Motif-specific source code
wx2_x_y_gtk.zip            GTK-specific source code
wx2_x_y_stubs.zip          Stubs ('empty port') source. Needs
                           wx2_x_y_gen.zip/tgz.
wx2_x_y_doc.zip            Documentation source code (not required)
wx2_x_y_hlp.zip            WinHelp documentation
wx2_x_y_pdf.zip            Acrobat PDF documentation
wx2_x_y_htm.zip            HTML documentation
wx2_x_y_vc.zip             MS VC++ 5/6 project files
wx2_x_y_bc.zip             Borland C++ 5 project files
wx2_x_y_cw.zip             Metrowerks CodeWarrior 4.1 project files
jpeg.zip                   Optional JPEG library
ogl3.zip                   Optional Object Graphics Library
glcanvas.zip               Optional wxGLCanvas class (Motif, GTK, MSW)
tex2rtf2.zip               Tex2RTF documentation tool

wxWindows for GTK distribution
------------------------------

wxGTK-2.x.y.tgz            wxGTK source distribution. You will
                           need the HTML and/or PDF documentation
                           from the zip set (above).

wxWindows for Motif distribution
--------------------------------

wxMotif-2.x.y.tgz          wxMotif source distribution. You will
                           need the HTML and/or PDF documentation
   -- OR --                from the zip set (above).

wx2_x_y_gen.zip
wx2_x_y_mot.zip
jpeg.zip

wxWindows for Windows distribution
----------------------------------

As well as the core source, the Windows setup contains:

- Windows Help versions of the documentation (docs/winhelp);
- wxGLCanvas (OpenGL integration, in utils/glcanvas);
- OGL 3 (Object Graphics Library, in utils/ogl);
- wxTreeLayout (for tree drawing, in utils/wxtree);
- a Dialog Editor binary;
- the JPEG library.

setup.exe               Setup file (Windows 95/98, NT)
setup.*                 Other setup files

Installation
------------

wxWindows 2 needs to be compiled before you can test out
the samples or write your own applications.
For installation information, please see the install.txt file
in the individual directories:

  docs/msw
  docs/gtk
  docs/motif

Licence information
-------------------

For licensing information, please see the files:

  docs/preamble.txt
  docs/licence.txt
  docs/licendoc.txt
  docs/gpl.txt
  docs/lgpl.txt

Although this may seem complex, it is there to allow authors of
proprietary/commercial applications to use wxWindows in
addition to those writing GPL'ed applications. In summary,
the licence is L-GPL plus a clause allowing unrestricted
distribution of application binaries. To answer a FAQ, you
don't have to distribute any source if you wish to write
commercial applications using wxWindows.

Documentation
-------------

See docs/release.txt for some notes about the current release.

See docs/bugs.txt for a bug list: this file is new and will initially
  be edited by hand, later generated from a web-based bug database.

See docs/html/index.htm for an HTML index of the major documents.

See docs/changes.txt for a detailed history of changes to wxWindows 2
  (not up-to-date).

The Windows help files are located in docs/winhelp.
The PDF help files are located in docs/pdf.

Further information
-------------------

The wxWindows Web site is located at:

  http://www.wxwindows.org

The wxGTK Web site (with further wxGTK-specific files and
information) is located at:

  http://www.freiburg.linux.de/~wxxt

The main wxWindows ftp site is at:

  ftp://www.remstar.com/pub/wxwin

A wxWindows CD-ROM with the latest distribution plus an HTML
front-end and hundreds of MB of compilers, utilities and other
material may be ordered from the CD-ROM page (see the wxWindows
web site) or contact Julian Smart <julian.smart@ukonline.co.uk>.

Good luck!

The wxWindows Team, November 1999

