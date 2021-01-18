/////////////////////////////////////////////////////////////////////////////
// Name:        samples.h
// Purpose:     Samples page of the Doxygen manual
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_samples Samples Overview

@tableofcontents

Probably the best way to learn wxWidgets is by reading the source of some 80+
samples provided with it. Many aspects of wxWidgets programming can be learned
from them, but sometimes it is not simple to just choose the right sample to
look at. This overview aims at describing what each sample does/demonstrates to
make it easier to find the relevant one if a simple grep through all sources
didn't help. They also provide some notes about using the samples and what
features of wxWidgets are they supposed to test.

There are currently more than 80 different samples as part of wxWidgets: the
list in this page is not complete! You should start your tour of wxWidgets with
the @ref page_samples_minimal which is the wxWidgets version of "Hello,
world!". It shows the basic structure of wxWidgets program and is the most
commented sample of all - looking at its source code is recommended.

The next most useful sample is @ref page_samples_widgets which shows many of
wxWidgets controls, such as buttons, text entry zones, list boxes, check boxes,
combo boxes etc. It is organized in many different source files, one per each
control, which makes it easier to study it, and also allows to change various
control styles and call its methods interactively.

Other, more complicated controls, have their own samples. In this category you
may find the following samples showing the corresponding controls:

@li wxCalendarCtrl: @ref page_samples_calendar
@li wxListCtrl: @ref page_samples_listctrl
@li wxTreeCtrl: @ref page_samples_treectrl
@li wxGrid: @ref page_samples_grid
@li wxDataViewCtrl: @ref page_samples_dataview
@li wxWebView: @ref page_samples_webview

Notice that all wxWidgets samples mentioned above can be found in @c samples
subdirectory of the library distribution. When a @c foobar sample is mentioned
below, its sources can be found in @c samples/foobar directory of your
wxWidgets tree. If you installed wxWidgets from a binary package, you might not
have this directory. In this case, you may view the samples online at
https://github.com/wxWidgets/wxWidgets/tree/master/samples/ but you need to
download the source distribution in order to be able to build them (highly
recommended).

Final advice is to do a search in the entire samples directory if you can't
find the sample showing the control you are interested in by name. Most classes
contained in wxWidgets occur in at least one of the samples.

@todo Write descriptions for the samples who description started with
 "This sample demonstrates", they are semi-auto generated.



@section page_samples_access Accessibility Sample

This sample shows how you can use the wxAccessible classes in a
simple GUI program.

@buildwith{wxUSE_ACCESSIBILITY}

@sampledir{access}

@section page_samples_animate Animation Sample

This sample shows how you can use wxAnimationCtrl
control and shows concept of a platform-dependent animation encapsulated
in wxAnimation.

@sampledir{animate}

@section page_samples_archive Archive Sample

This sample shows how you can use wxArchiveClassFactory, wxArchiveOutputStream
and wxArchiveInputStream. This shows how to process ZIP and TAR archives using
wxZipOutputStream and wxTarOutputStream

@sampledir{archive}

@section page_samples_artprov Art Provider Sample

This sample shows how you can customize the look of standard
wxWidgets dialogs by replacing default bitmaps/icons with your own versions.
It also shows how you can use wxArtProvider to
get stock bitmaps for use in your application.

@sampledir{artprov}

@section page_samples_aui Advanced User Interface Sample

@sampleabout{@ref overview_aui "AUI classes"}

@sampledir{aui}

@section page_samples_calendar Calendar Sample

This sample shows the calendar control in action. It
shows how to configure the control (see the different options in the calendar
menu) and also how to process the notifications from it.

@sampledir{calendar}

@section page_samples_caret Caret Sample

@sampleabout{wxCaret}

@sampledir{caret}

@section page_samples_collpane Collapsible Pane Sample

@sampleabout{wxCollapsiblePane}

@sampledir{collpane}

@section page_samples_combo Combo Sample

@sampleabout{wxComboBox\, wxComboCtrl and wxOwnerDrawnComboBox etc}

@sampledir{combo}

@section page_samples_config Configuration Sample

This sample demonstrates the wxConfig classes in a platform
independent way, i.e. it uses text based files to store a given configuration under
Unix and uses the Registry under Windows.

See @ref overview_config for the descriptions of all features of this class.

@sampledir{config}

@section page_samples_console Console Program Sample

@sampleabout{a console program}

@sampledir{console}

@section page_samples_dataview wxDataViewCtrl Sample

@sampleabout{wxDataViewCtrl}

@sampledir{dataview}

@section page_samples_clipboard Clipboard Sample

@sampleabout{wxClipboard}

@sampledir{clipboard}

@section page_samples_debugrpt Debug Reporter Sample

This sample shows how to use wxDebugReport class to
generate a debug report in case of a program crash or otherwise. On start up,
it proposes to either crash itself (by dereferencing a NULL pointer) or
generate debug report without doing it. Next it initializes the debug report
with standard information adding a custom file to it (just a timestamp) and
allows to view the information gathered using
wxDebugReportPreview.

For the report processing part of the sample to work you should make available
a Web server accepting form uploads, otherwise
wxDebugReportUpload will report an error.

@buildwith{wxUSE_DEBUGREPORT, wxUSE_ON_FATAL_EXCEPTION}

@sampledir{debugrpt}

@section page_samples_dialogs Dialogs Sample

This sample shows how to use the common dialogs available from wxWidgets. These
dialogs are described in detail in the @ref overview_cmndlg.

In addition to the dialogs accessible from the sample menus, you can also run
it with a <code>\--progress=style</code> command line option to show a
wxProgressDialog with the given style (try 0 for the default style) on program
startup, before the main window is shown.

@sampledir{dialogs}

@section page_samples_dialup Dialup Sample

This sample shows the wxDialUpManager
class. In the status bar, it displays the information gathered through its
interface: in particular, the current connection status (online or offline) and
whether the connection is permanent (in which case a string `LAN' appears in
the third status bar field - but note that you may be on a LAN not
connected to the Internet, in which case you will not see this) or not.

Using the menu entries, you may also dial or hang up the line if you have a
modem attached and (this only makes sense for Windows) list the available
connections.

@sampledir{dialup}

@section page_samples_display Display Sample

@sampleabout{wxDisplay}

@sampledir{display}

@section page_samples_dnd Drag & Drop Sample

This sample shows both clipboard and drag and drop in action. It is quite non
trivial and may be safely used as a basis for implementing the clipboard and
drag and drop operations in a real-life program.

When you run the sample, its screen is split in several parts. On the top,
there are two listboxes which show the standard derivations of
wxDropTarget:
wxTextDropTarget and
wxFileDropTarget.

The middle of the sample window is taken by the log window which shows what is
going on (of course, this only works in debug builds) and may be helpful to see
the sequence of steps of data transfer.

Finally, the last part is used for dragging text from it to either one of the
listboxes (only one will accept it) or another application. The last
functionality available from the main frame is to paste a bitmap from the
clipboard (or, in the case of the Windows version, also a metafile) - it will be
shown in a new frame.

So far, everything we mentioned was implemented with minimal amount of code
using standard wxWidgets classes. The more advanced features are demonstrated
if you create a shape frame from the main frame menu. A shape is a geometric
object which has a position, size and color. It models some
application-specific data in this sample. A shape object supports its own
private wxDataFormat which means that you may cut and
paste it or drag and drop (between one and the same or different shapes) from
one sample instance to another (or the same). However, chances are that no
other program supports this format and so shapes can also be rendered as
bitmaps which allows them to be pasted/dropped in many other applications
(and, under Windows, also as metafiles which are supported by most of Windows
programs as well - try Write/Wordpad, for example).

Take a look at DnDShapeDataObject class to see how you may use
wxDataObject to achieve this.

@sampledir{dnd}

@section page_samples_docview Document/View Sample

@sampleabout{@ref overview_docview}

@sampledir{docview}

@see @sample{mdi}

@section page_samples_dragimag Drag Image Sample

@sampleabout{wxDragImage}

@sampledir{dragimag}

@section page_samples_drawing Drawing Sample

@sampleabout{the drawing ability of wxDC}

@sampledir{drawing}

@section page_samples_erase Erase Event Sample

@sampleabout{wxEraseEvent}

@sampledir{erase}

@section page_samples_event Event Sample

This sample demonstrates various features of the wxWidgets events. It
shows how to dynamic events and connecting/disconnecting the event handlers
during run time by using wxEvtHandler::Bind() and wxEvtHandler::Unbind(),
and also how to use wxWindow::PushEventHandler() and wxWindow::PopEventHandler().

@sampledir{event}

@section page_samples_except Exception Sample

This very simple sample shows how to use C++ exceptions in wxWidgets programs,
i.e. where to catch the exception which may be thrown by the program code. It
doesn't do anything very exciting by itself, you need to study its code to
understand what goes on.

<b>Build Note:</b>
You need to build the library with @c wxUSE_EXCEPTIONS being set to @c 1
and compile your code with C++ exceptions support to be able to build this
sample.

@sampledir{except}

@section page_samples_exec External Program Execution Sample

The exec sample demonstrates the wxExecute and
wxShell functions. Both of them are used to execute the
external programs and the sample shows how to do this synchronously (waiting
until the program terminates) or asynchronously (notification will come later).

It also shows how to capture the output of the child process in both
synchronous and asynchronous cases and how to kill the processes with
wxProcess::Kill() and test for their existence with
wxProcess::Exists().

@sampledir{exec}

@section page_samples_font Font Sample

The font sample demonstrates wxFont,
wxFontEnumerator and
wxFontMapper classes. It allows you to see the fonts
available (to wxWidgets) on the computer and shows all characters of the
chosen font as well.

@sampledir{font}

@section page_samples_grid Grid Sample

@sampleabout{wxGrid}

@sampledir{grid}

@section page_samples_help Help Sample

@sampleabout{wxHelpController}

@sampledir{help}

@section page_samples_html HTML Sample

Eight HTML samples cover all features of the HTML sub-library.

@li @b Test demonstrates how to create wxHtmlWindow
and also shows most supported HTML tags.

@li @b Widget shows how you can embed ordinary controls or windows within an
HTML page. It also nicely explains how to write new tag handlers and extend
the library to work with unsupported tags.

@li @b About may give you an idea how to write good-looking About boxes.

@li @b Zip demonstrates use of virtual file systems in wxHTML. The zip archives
handler (ships with wxWidgets) allows you to access HTML pages stored
in a compressed archive as if they were ordinary files.

@li @b Virtual is yet another virtual file systems demo. This one generates pages at run-time.
You may find it useful if you need to display some reports in your application.

@li @b Printing explains use of wxHtmlEasyPrinting
class which serves as as-simple-as-possible interface for printing HTML
documents without much work. In fact, only few function calls are sufficient.

@li @b Help and @b Helpview are variations on displaying HTML help
(compatible with MS HTML Help Workshop). @e Help shows how to embed
wxHtmlHelpController in your application
while @e Helpview is a simple tool that only pops up the help window and
displays help books given at command line.

@sampledir{html}

@section page_samples_htlbox HTML List Box Sample

@sampleabout{wxHtmlListBox}

@sampledir{htlbox}

@section page_samples_image Image Sample

The image sample demonstrates use of the wxImage class
and shows how to download images in a variety of formats, currently PNG, GIF,
TIFF, JPEG, BMP, PNM and PCX. The top of the sample shows two rectangles, one
of which is drawn directly in the window, the other one is drawn into a
wxBitmap, converted to a wxImage, saved as a PNG image
and then reloaded from the PNG file again so that conversions between wxImage
and wxBitmap as well as loading and saving PNG files are tested.

At the bottom of the main frame there is a test for using a monochrome bitmap by
drawing into a wxMemoryDC. The bitmap is then drawn
specifying the foreground and background colours with
wxDC::SetTextForeground() and
wxDC::SetTextBackground() (on the left). The
bitmap is then converted to a wxImage and the foreground colour (black) is
replaced with red using wxImage::Replace().

This sample also contains the code for testing the image rotation and resizing
and using raw bitmap access, see the corresponding menu commands.

@sampledir{image}

@section page_samples_internat Internationalization Sample

The not very clearly named internat sample demonstrates the wxWidgets
internationalization (i18n for short from now on) features. To be more
precise, it only shows localization support, i.e. support for translating the
program messages into another language while true i18n would also involve
changing the other aspects of the program's behaviour.

More information about this sample can be found in the @c readme.txt file in
its directory. Please also see the @ref overview_i18n overview.

@sampledir{internat}

@section page_samples_ipc Connection Sample

@sampleabout{wxConnection}

@sampledir{ipc}

@section page_samples_joytest Joystick Sample

@sampleabout{wxJoystick}

@sampledir{joytest}

@section page_samples_keyboard Key Event Sample

@sampleabout{wxKeyEvent}

This sample can be used to interactively test the events produced by pressing
various keyboard keys. It also shows the interaction between accelerators and
the normal keyboard events (which are overridden by any defined accelerators)
and finally allows to test that not skipping an event in EVT_KEY_DOWN handler
suppresses the subsequent EVT_CHAR event.

@sampledir{keyboard}

@section page_samples_layout Layout Sample

The layout sample demonstrates the two different layout systems offered
by wxWidgets. When starting the program, you will see a frame with some
controls and some graphics. The controls will change their size whenever
you resize the entire frame and the exact behaviour of the size changes
is determined using the wxLayoutConstraints
class. See also the overview and the
wxIndividualLayoutConstraint
class for further information.

The menu in this sample offers two more tests, one showing how to use
a wxBoxSizer in a simple dialog and the other one
showing how to use sizers in connection with a wxNotebook
class. See also wxSizer.

@sampledir{layout}

@section page_samples_listctrl List Control Sample

This sample shows the wxListCtrl control. Different modes
supported by the control (list, icons, small icons, report) may be chosen from
the menu.

The sample also provides some timings for adding/deleting/sorting a lot of
(several thousands) items into the control.

@sampledir{listctrl}

@section page_samples_mdi MDI Sample

@sampleabout{MDI}

@see @sample{docview}

@sampledir{mdi}

@section page_samples_mediaplayer Mediaplayer Sample

This sample demonstrates how to use all the features of
wxMediaCtrl and play various types of sound, video,
and other files.

It replaces the old @c dynamic sample.

@sampledir{mediaplayer}

@section page_samples_memcheck Memory Checking Sample

@sampleabout{memory tracing using wxDebugContext}

@sampledir{memcheck}

@buildwith{wxUSE_MEMORY_TRACING, wxUSE_DEBUG_CONTEXT}

@section page_samples_menu Menu Sample

@sampleabout{wxMenu classes}

@sampledir{menu}

@section page_samples_mfc MFC Sample

@sampleabout{how to mix MFC and wxWidgets code}
It pops up an initial wxWidgets frame, with a menu item
that allows a new MFC window to be created.

For build instructions please read IMPORTANT NOTES in @c mfctest.cpp.

@onlyfor{wxmsw}

@sampledir{mfc}

@section page_samples_minimal Minimal Sample

The minimal sample is what most people will know under the term Hello World,
i.e. a minimal program that doesn't demonstrate anything apart from what is
needed to write a program that will display a "hello" dialog. This is usually
a good starting point for learning how to use wxWidgets.

@sampledir{minimal}

@section page_samples_nativdlg Native Windows Dialog Sample

@sampleabout{native windows dialog}

@onlyfor{wxmsw}

@sampledir{nativdlg}

@section page_samples_notebook Notebook Sample

This samples shows wxBookCtrl family of controls.
Although initially it was written to demonstrate wxNotebook
only, it can now be also used to see wxListbook,
wxChoicebook, wxTreebook and wxToolbook in action.
Test each of the controls, their orientation, images and pages using
commands through the menu.

@sampledir{notebook}

@section page_samples_oleauto OLE Automation Sample

@sampleabout{OLE automation using wxAutomationObject}

@onlyfor{wxmsw}

@sampledir{oleauto}

@section page_samples_opengl OpenGL Sample

@sampleabout{wxGLCanvas}

@li @b cube Draws a cube to demonstrate how to write a basic wxWidgets OpenGL program.
    Arrow keys rotate the cube. Space bar toggles spinning.
@li @b isosurf Draws a surface by reading coordinates from a DAT file.
@li @b penguin Draws a rotatable penguin by reading data from a DXF file.
@li @b pyramid Draws a rotatable tetrahedron and some strings.
    It uses OpenGL 3.2 Core Profile context.

@sampledir{opengl}

@section page_samples_ownerdrw Owner-drawn Sample

@sampleabout{owner-drawn wxMenuItem\, wxCheckList and wxListBox}

<!--It's weird that it doesn't need <wx/ownerdrw.h> and class wxOwnerDrawn!-->

@sampledir{ownerdrw}

@section page_samples_popup Popup Transient Window Sample

@sampleabout{wxPopupTransientWindow}

@sampledir{popup}

@section page_samples_power Power Management Sample

@sampleabout{wxWidgets power management}

@see wxPowerEvent

@sampledir{power}

@section page_samples_printing Printing Sample

@sampleabout{printing}

@see @ref overview_printing, @ref overview_unixprinting

@buildwith{wxUSE_PRINTING_ARCHITECTURE}

@sampledir{printing}

@section page_samples_propgrid wxPropertyGrid Sample

Sample application has following additional examples of custom properties:
- wxFontDataProperty ( edits wxFontData )
- wxPointProperty ( edits wxPoint )
- wxSizeProperty ( edits wxSize )
- wxAdvImageFileProperty ( like wxImageFileProperty, but also has a drop-down
  for recent image selection )
- wxDirsProperty ( edits a wxArrayString consisting of directory strings)
- wxArrayDoubleProperty ( edits wxArrayDouble )

@sampleabout{wxPropertyGrid}

@sampledir{propgrid}

@section page_samples_regtest Registry Sample

@sampleabout{wxRegKey}

<!--Its directory name doesn't seem to be proper.-->

@onlyfor{wxmsw}

@sampledir{regtest}

@section page_samples_render Render Sample

This sample shows how to replace the default wxWidgets
renderer and also how to write a shared library
(DLL) implementing a renderer and load and unload it during the run-time.

@sampledir{render}

@section page_samples_richtext wxRichTextCtrl Sample

@sampleabout{wxRichTextCtrl}

@sampledir{richtext}

@section page_samples_sashtest Sash Sample

@sampleabout{wxSashWindow classes}

@sampledir{sashtest}

@section page_samples_scroll Scroll Window Sample

@sampleabout{wxScrolledWindow}

This sample demonstrates use of the ::wxScrolledWindow
class including placing subwindows into it and drawing simple graphics. It uses
the SetTargetWindow method and thus the effect of scrolling does not show in
the scrolled window itself, but in one of its subwindows.

Additionally, this samples demonstrates how to optimize drawing operations in
wxWidgets, in particular using the wxWindow::IsExposed() method with the aim to
prevent unnecessary drawing in the window and thus reducing or removing flicker
on screen.

@sampledir{scroll}

@section page_samples_secretstore Secret Store Sample

@sampleabout{wxSecretStore}

This console-mode sample shows the use of wxSecretStore class for remembering
the user-entered passwords. It should be run from the command line with the
first argument of @c save, @c load or @c delete, followed by the "service" and
"user" arguments as used by wxSecretStore methods. After storing some password,
you can check that it can be retrieved later and also that it can be seen in
the OS-provided password manager (e.g. credential manager under MSW or keychain
utility under macOS).

@sampledir{secretstore}

@section page_samples_shaped Shaped Window Sample

@sampleabout{how to implement a shaped or transparent window\, and a window showing/hiding with effect}

@see wxTopLevelWindow::SetShape(), wxTopLevelWindow::SetTransparent(),
wxWindow::ShowWithEffect(), wxWindow::HideWithEffect()

@sampledir{shaped}

@section page_samples_sockets Sockets Sample

The sockets sample demonstrates how to use the communication facilities
provided by wxSocket. There are two different
applications in this sample: a server, which is implemented using a
wxSocketServer object, and a client, which
is implemented as a wxSocketClient.

The server binds to the local address, using TCP port number 3000,
sets up an event handler to be notified of incoming connection requests
(@b wxSOCKET_CONNECTION events), and sits there, waiting for clients
(@e listening, in socket parlance). For each accepted connection,
a new wxSocketBase object is created. These
socket objects are independent from the server that created them, so
they set up their own event handler, and then request to be notified
of @b wxSOCKET_INPUT (incoming data) or @b wxSOCKET_LOST
(connection closed at the remote end) events. In the sample, the event
handler is the same for all connections; to find out which socket the
event is addressed to, the GetSocket function
is used.

Although it might take some time to get used to the event-oriented
system upon which wxSocket is built, the benefits are many. See, for
example, that the server application, while being single-threaded
(and of course without using fork() or ugly select() loops) can handle
an arbitrary number of connections.

The client starts up unconnected, so you can use the Connect... option
to specify the address of the server you are going to connect to (the
TCP port number is hard-coded as 3000). Once connected, a number of
tests are possible. Currently, three tests are implemented. They show
how to use the basic IO calls in wxSocketBase,
such as wxSocketBase::Read(), wxSocketBase::Write(),
wxSocketBase::ReadMsg() and wxSocketBase::WriteMsg(),
and how to set up the correct IO flags depending on what you are going to
do. See the comments in the code for more information. Note that because
both clients and connection objects in the server set up an event handler
to catch @b wxSOCKET_LOST events, each one is immediately notified
if the other end closes the connection.

There is also a URL test which shows how to use
the wxURL class to fetch data from a given URL.

The sockets sample is work in progress. Some things to do:

@li More tests for basic socket functionality.
@li More tests for protocol classes (wxProtocol and its descendants).
@li Tests for the recently added (and still in alpha stage) datagram sockets.
@li New samples which actually do something useful (suggestions accepted).

@sampledir{sockets}

@section page_samples_sound Sound Sample

The @c sound sample shows how to use wxSound for simple
audio output (e.g. notifications).

@sampledir{sound}

@section page_samples_splash Splash Screen Sample

@sampleabout{wxSplashScreen}

@sampledir{splash}

@section page_samples_splitter Splitter Window Sample

@sampleabout{wxSplitterWindow}

@sampledir{splitter}

@section page_samples_statbar Status Bar Sample

This sample shows how to create and use wxStatusBar. Although most of the
samples have a statusbar, they usually only create a default one and only
do it once.

Here you can see how to recreate the statusbar (with possibly different number
of fields) and how to use it to show icons/bitmaps and/or put arbitrary
controls into it.

@sampledir{statbar}

@section page_samples_stc wxStyledTextCtrl Sample

@sampleabout{wxStyledTextCtrl}

@sampledir{stc}

@section page_samples_svg SVG Sample

@sampleabout{wxSVGFileDC}

@sampledir{svg}

@section page_samples_taborder Tab Order Sample

This sample allows to test keyboard navigation (mostly done using the
@c TAB key, hence the sample name) between different controls.
It shows the use of wxWindow::MoveBeforeInTabOrder() and
MoveAfterInTabOrder() methods to change
the default order of the windows in the navigation chain and of
wxWindow::Navigate() for moving focus along this
chain.

@sampledir{taborder}

@section page_samples_taskbar Task Bar Icon Sample

@sampleabout{wxTaskBarIcon}

@sampledir{taskbar}

@section page_samples_text Text Sample

This sample demonstrates four features: firstly the use and many variants of
the wxTextCtrl class (single line, multi line, read only,
password, ignoring TAB, ignoring ENTER).

Secondly it shows how to intercept a wxKeyEvent in both
the raw form using the @c EVT_KEY_UP and @c EVT_KEY_DOWN macros and the
higher level from using the @c EVT_CHAR macro. All characters will be logged
in a log window at the bottom of the main window. By pressing some of the function
keys, you can test some actions in the text ctrl as well as get statistics on the
text ctrls, which is useful for testing if these statistics actually are correct.

Thirdly, on platforms which support it, the sample will offer to copy text to the
wxClipboard and to paste text from it. The GTK version will
use the so called PRIMARY SELECTION, which is the pseudo clipboard under X and
best known from pasting text to the XTerm program.

Last but not least: some of the text controls have tooltips and the sample also
shows how tooltips can be centrally disabled and their latency controlled.

@sampledir{text}

@section page_samples_thread Thread Sample

This sample demonstrates use of threads in connection with GUI programs.

There are two fundamentally different ways to use threads in GUI programs and
either way has to take care of the fact that the GUI library itself usually
is not multi-threading safe, i.e. that it might crash if two threads try to
access the GUI class simultaneously.

One way to prevent that is have a normal GUI program in the main thread and some
worker threads which work in the background. In order to make communication between
the main thread and the worker threads possible, wxWidgets offers the ::wxQueueEvent
function and this sample demonstrates its usage.

The other way is to use a ::wxMutexGuiEnter and ::wxMutexGuiLeave functions, but
this is not currently shown in the sample.

See also @ref overview_thread and wxThread.

@sampledir{thread}

@section page_samples_toolbar Tool Bar Sample

The toolbar sample shows the wxToolBar class in action.

The following things are demonstrated:

@li Creating the toolbar using wxToolBar::AddTool() and wxToolBar::AddControl(): see
    MyApp::InitToolbar() in the sample.
@li Using @c EVT_UPDATE_UI handler for automatically enabling/disabling
    toolbar buttons without having to explicitly call EnableTool. This is done
    in MyFrame::OnUpdateCopyAndCut().
@li Using wxToolBar::DeleteTool() and wxToolBar::InsertTool() to dynamically update the
    toolbar.

Some buttons in the main toolbar are check buttons, i.e. they stay checked when
pressed. On the platforms which support it, the sample also adds a combobox
to the toolbar showing how you can use arbitrary controls and not only buttons
in it.

If you toggle another toolbar in the sample (using @c Ctrl-A) you will also
see the radio toolbar buttons in action: the first three buttons form a radio
group, i.e. checking any of them automatically unchecks the previously
checked one.

@sampledir{toolbar}

@section page_samples_treectrl wxTreeCtrl Sample

This sample demonstrates using the wxTreeCtrl class. Here
you may see how to process various notification messages sent by this control
and also when they occur (by looking at the messages in the text control in
the bottom part of the frame).

Adding, inserting and deleting items and branches from the tree as well as
sorting (in default alphabetical order as well as in custom one) is
demonstrated here as well - try the corresponding menu entries.

@sampledir{treectrl}

@section page_samples_typetest Types Sample

@sampleabout{wxWidgets types}

@todo This sample isn't very didactive; it's more than a set of tests rather
      than a sample and thus should be rewritten with CppUnit and moved under "tests"

@sampledir{typetest}

@section page_samples_uiaction wxUIActionSimulator Sample

@sampleabout{wxUIActionSimulator}

This sample shows some features of wxUIActionSimulator class. When a simulation
is run using its menu items, you can see that the button is pressed
programmatically and the characters generated by the program appear in the text
control.

@sampledir{uiaction}

@section page_samples_validate Validator Sample

@sampleabout{wxValidator}

@sampledir{validate}

@section page_samples_vscroll VScrolled Window Sample

@sampleabout{wxVScrolledWindow}

@sampledir{vscroll}

@section page_samples_webview wxWebView Sample

The wxWebView sample demonstarates the various capabilities of the wxWebView
control. It is set up as a simple single window web browser, but with support
for many of the more complex wxWebView features, including browsing through
archives.

@sampledir{webview}

@section page_samples_webrequest Web Request Sample

This sample demonstrates the various capabilities of the
wxWebRequest class. It shows how to handle simple text HTTP and HTTPS requests,
downloading files, showing download progress and processing downloaded
data while it's being downloaded.

@sampledir{webrequest}

@section page_samples_widgets Widgets Sample

The widgets sample is the main presentation program for most simple and advanced
native controls and complex generic widgets provided by wxWidgets.
The sample tests their basic functionality, events, placement, modification
in terms of colour and font as well as the possibility to change
the controls programmatically, such as adding an item to a list box etc.
All widgets are categorized for easy browsing.

@sampledir{widgets}

@section page_samples_wizard Wizard Sample

This sample shows the so-called wizard dialog (implemented using
wxWizard and related classes). It shows almost all
features supported:

@li Using bitmaps with the wizard and changing them depending on the page
    shown (notice that wxValidationPage in the sample has a different image from
    the other ones)
@li Using TransferDataFromWindow
    to verify that the data entered is correct before passing to the next page
    (done in wxValidationPage which forces the user to check a checkbox before
    continuing).
@li Using more elaborated techniques to allow returning to the previous
    page, but not continuing to the next one or vice versa (in wxRadioboxPage)
@li This (wxRadioboxPage) page also shows how the page may process the
    @e Cancel button itself instead of relying on the wizard parent to do it.
@li Normally, the order of the pages in the wizard is known at compile-time,
    but sometimes it depends on the user choices: wxCheckboxPage shows how to
    dynamically decide which page to display next (see also
    wxWizardPage)

@sampledir{wizard}

@section page_samples_wrapsizer wxWrapSizer Sample

@sampleabout{wxWrapSizer}

@sampledir{wrapsizer}

@section page_samples_xrc XRC Sample

This sample shows how to use the various features of the @ref overview_xrc to
create the gui of your program. It starts by loading and showing a frame and
other resources. From its menu or toolbar you can then run the following dialogs:

@li A non-derived wxDialog
@li A derived dialog
@li A dialog containing a large number of controls
@li An uncentred dialog
@li A dialog demonstrating the use of object references and ID ranges
@li A dialog that contains a custom class
@li A dialog with platform-specific features
@li A dialog demonstrating wxArtProvider
@li A dialog saying "VARIABLE EXPANSION ISN'T IMPLEMENTED CURRENTLY" :/

@sampledir{xrc}

*/
