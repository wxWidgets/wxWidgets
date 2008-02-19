/////////////////////////////////////////////////////////////////////////////
// Name:        libs.h
// Purpose:     Libraries page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page libraries_page Libraries list

 wxWidgets can be built either as a single large
 library (this is called the <em>monolithic build</em>) or as several smaller
 libraries (<em>multilib build</em>}). Multilib build is the default.

 wxWidgets library is divided into libraries briefly described below. This
 diagram show dependencies between them:

 @image html libs.gif

 Please note that arrows indicate the "depends from" relation and that
 all blue boxes depend from wxBase library (i.e. they are non-GUI libraries),
 and all green boxes depend from wxCore library (i.e. they are GUI libraries).


 @section wxBase wxBase

 Every wxWidgets application must link against this library. It contains
 mandatory classes that any wxWidgets code depends on (e.g. wxString) and
 portability classes that abstract differences between platforms.
 wxBase can be used to develop console mode applications, it does not require
 any GUI libraries or running X Window System on Unix.


 @section wxCore wxCore

 Basic GUI classes such as GDI classes or controls are in this library. All
 wxWidgets GUI applications must link against this library, only console mode
 applications don't.

 Requires wxBase.


 @section wxAui wxAui

 This contains the Advanced User Interface docking library.

 Requires wxAdvanced, wxHTML, wxXML, wxCore, wxBase.


 @section wxNet wxNet

 Classes for network access:

 @li wxSocket classes (wxSocketClient, wxSocketServer and related classes)
 @li wxSocketOutputStream and wxSocketInputStream
 @li sockets-based IPC classes (wxTCPServer, wxTCPClient and wxTCPConnection)
 @li wxURL
 @li wxInternetFSHandler (a wxFileSystem handler)

 Requires wxBase.


 @section wxRichText wxRichText

 This contains generic rich text control functionality.

 Requires wxAdvanced, wxHTML, wxXML, wxCore, wxBase.


 @section wxXML wxXML

 This library contains simple classes for parsing XML documents.

 Requires wxBase.


 @section wxAdvanced wxAdvanced

 Advanced or rarely used GUI classes:

 @li wxCalendarCtrl
 @li wxGrid classes
 @li wxJoystick
 @li wxLayoutAlgorithm
 @li wxSplashScreen
 @li wxTaskBarIcon
 @li wxSound
 @li wxWizard
 @li wxSashLayoutWindow
 @li wxSashWindow
 @li ...others

 Requires wxCore and wxBase.


 @section wxMedia wxMedia

 Miscellaneous classes related to multimedia. Currently this library only
 contains wxMediaCtrl but more classes will be added in
 the future.

 Requires wxCore and wxBase.


 @section wxGL wxGL

 This library contains wxGLCanvas class for integrating
 OpenGL library with wxWidgets. Unlike all others, this library is {\em not}
 part of the monolithic library, it is always built as separate library.

 Requires wxCore and wxBase.


 @section wxHTML wxHTML

 Simple HTML renderer and other @ref html_overview are contained in this library, 
 as well as wxHtmlHelpController, wxBestHelpController and wxHtmlListBox.

 Requires wxCore and wxBase.


 @section wxQA wxQA

 This is the library containing extra classes for quality assurance. Currently
 it only contains wxDebugReport and related classes,
 but more will be added to it in the future.

 Requires wxXML, wxCore, wxBase.


 @section wxXRC wxXRC

 This library contains wxXmlResource class that
 provides access to XML resource files in XRC format

 Requires wxAdvanced, wxHTML, wxXML, wxCore, wxBase.


 @section wxSTC wxSTC

 STC (Styled Text Control) is a wrapper around Scintilla, a syntax-highlighting text editor.

 Requires wxCore, wxBase.

*/
