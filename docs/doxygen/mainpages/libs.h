/////////////////////////////////////////////////////////////////////////////
// Name:        libs.h
// Purpose:     Libraries page of the Doxygen manual
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_libs Library List

@tableofcontents

wxWidgets can be built either as a single large library (this is called a
<em>monolithic build</em>) or as several smaller libraries
(<em>multilib build</em>). Multilib build is the default.

wxWidgets library is divided into libraries briefly described below. This
diagram shows the dependencies between them:

@dot
digraph Dependencies
{
    node [shape = ellipse, fontname = "Courier", fontsize = 10, style = filled];

    wxBase     [URL = "\ref page_libs_wxbase"];

    wxCore     [fillcolor = deepskyblue, URL = "\ref page_libs_wxcore"];
    wxNet      [fillcolor = deepskyblue, URL = "\ref page_libs_wxnet"];
    wxXML      [fillcolor = deepskyblue, URL = "\ref page_libs_wxxml"];

    wxAUI      [fillcolor = green, URL = "\ref page_libs_wxaui"];
    wxGL       [fillcolor = green, URL = "\ref page_libs_wxgl"];
    wxHTML     [fillcolor = green, URL = "\ref page_libs_wxhtml"];
    wxMedia    [fillcolor = green, URL = "\ref page_libs_wxmedia"];
    wxPropertyGrid [fillcolor = green, URL = "\ref page_libs_wxpropgrid"];
    wxQA       [fillcolor = green, URL = "\ref page_libs_wxqa"];
    wxRibbon   [fillcolor = green, URL = "\ref page_libs_wxribbon"];
    wxRichText [fillcolor = green, URL = "\ref page_libs_wxrichtext"];
    wxSTC      [fillcolor = green, URL = "\ref page_libs_wxstc"];
    wxXRC      [fillcolor = green, URL = "\ref page_libs_wxxrc"];
    wxWebView  [fillcolor = green, URL = "\ref page_libs_wxwebview"];

    wxCore -> wxBase;
    wxNet -> wxBase;
    wxXML -> wxBase;

    wxAUI -> wxCore;
    wxGL -> wxCore;
    wxHTML -> wxCore;
    wxMedia -> wxCore;
    wxPropertyGrid -> wxCore;
    wxQA -> wxCore; wxQA -> wxXML;
    wxRibbon -> wxCore;
    wxRichText -> wxCore; wxRichText -> wxHTML; wxRichText -> wxXML;
    wxSTC -> wxCore;
    wxXRC -> wxCore; wxXRC -> wxHTML; wxXRC -> wxXML;
    wxWebView -> wxCore;
}
@enddot

Please note that arrows indicate the "depends from" relation and that all blue
libraries depend on the @ref page_libs_wxbase library (i.e. they are non-GUI
libraries), and all green libraries depend on the @ref page_libs_wxcore library
(i.e. they are GUI libraries).

@note Until wxWidgets 3.1.2 some core GUI classes were in a separate wxAdvanced
    library, but this library was merged into wxCore and, while it's still
    preserved as an empty library for compatibility, should be never used any
    longer and will disappear completely in the future.


@section page_libs_wxaui wxAui

This contains the Advanced User Interface docking library.

Requires @ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxbase wxBase

Every wxWidgets application must link against this library. It contains
mandatory classes that any wxWidgets code depends on (e.g. wxString) and
portability classes that abstract differences between platforms. wxBase can be
used to develop console mode applications, it does not require any GUI
libraries or running X Window System on Unix.


@section page_libs_wxcore wxCore

Basic GUI classes such as GDI classes or controls are in this library. All
wxWidgets GUI applications must link against this library, only console mode
applications don't.

Requires @ref page_libs_wxbase.


@section page_libs_wxgl wxGL

This library contains wxGLCanvas class for integrating OpenGL library with
wxWidgets. Unlike all others, this library is @b not part of the monolithic
library, it is always built as separate library.

Requires @ref page_libs_wxcore and @ref page_libs_wxbase.


@section page_libs_wxhtml wxHTML

Simple HTML renderer and other @ref overview_html are contained in this
library, as well as wxHtmlHelpController, wxBestHelpController and
wxHtmlListBox.

Requires @ref page_libs_wxcore and @ref page_libs_wxbase.


@section page_libs_wxmedia wxMedia

Miscellaneous classes related to multimedia. Currently this library only
contains wxMediaCtrl but more classes will be added in the future.

Requires @ref page_libs_wxcore and @ref page_libs_wxbase.


@section page_libs_wxnet wxNet

Classes for network access:

@li wxWebRequest
@li wxSocket classes (wxSocketClient, wxSocketServer and related classes)
@li wxSocketOutputStream and wxSocketInputStream
@li sockets-based IPC classes (wxTCPServer, wxTCPClient and wxTCPConnection)
@li wxURL
@li wxInternetFSHandler (a wxFileSystem handler)

Requires @ref page_libs_wxbase.


@section page_libs_wxpropgrid wxPropertyGrid

This contains the wxPropertyGrid control.

Requires @ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxqa wxQA

This is the library containing extra classes for quality assurance. Currently
it only contains wxDebugReport and related classes, but more will be added to
it in the future.

Requires @ref page_libs_wxxml, @ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxribbon wxRibbon

This contains the Ribbon User Interface components library.

Requires @ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxrichtext wxRichText

This contains generic rich text control functionality.

Requires @ref page_libs_wxhtml, @ref page_libs_wxxml,
@ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxstc wxSTC

STC (Styled Text Control) is a wrapper around Scintilla, a syntax-highlighting
text editor. See <http://www.scintilla.org/> for more info about Scintilla.

Requires @ref page_libs_wxcore, @ref page_libs_wxbase.

@section page_libs_wxwebview wxWebView

The wxWebView library contains the wxWebView control and its associated classes.

Requires @ref page_libs_wxcore, @ref page_libs_wxbase.


@section page_libs_wxxml wxXML

This library contains simple classes for parsing XML documents.

Requires @ref page_libs_wxbase.


@section page_libs_wxxrc wxXRC

This library contains wxXmlResource class that provides access to XML resource
files in XRC format.

Requires @ref page_libs_wxhtml, @ref page_libs_wxxml,
@ref page_libs_wxcore, @ref page_libs_wxbase.

*/
