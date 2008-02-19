/////////////////////////////////////////////////////////////////////////////
// Name:        samples
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!
 
 @page samples_overview wxWidgets samples
 
 Probably the best way to learn wxWidgets is by reading the source of some 50+
 samples provided with it. Many aspects of wxWidgets programming can be learnt
 from them, but sometimes it is not simple to just choose the right sample to
 look at. This overview aims at describing what each sample does/demonstrates to
 make it easier to find the relevant one if a simple grep through all sources
 didn't help. They also provide some notes about using the samples and what
 features of wxWidgets are they supposed to test.
 There are currently more than 50 different samples as part of wxWidgets and
 this list is not complete. You should start your tour of wxWidgets with the
 @ref sampleminimal_overview which is the wxWidgets version of
 "Hello, world!". It shows the basic structure of wxWidgets program and is the
 most commented sample of all - looking at its source code is recommended.
 The next most useful samples are probably #widgets
 and #controls which show many of wxWidgets native and
 generic controls, such as buttons, listboxes, checkboxes, comboboxes etc.
 Other, more complicated controls, have their own samples. In this category you
 may find the following samples showing the corresponding controls:
 
 
 
 
 
 
 #wxCalendarCtrl
 
 
 
 
 Calendar a.k.a. date picker control
 
 
 
 
 
 #wxListCtrl
 
 
 
 
 List view control
 
 
 
 
 
 #wxTreeCtrl
 
 
 
 
 Tree view control
 
 
 
 
 
 #wxGrid
 
 
 
 
 Grid control
 
 
 
 
 
 Finally, it might be helpful to do a search in the entire sample directory if
 you can't find the sample showing the control you are interested in by
 name. Most classes contained in wxWidgets occur in at least one of the samples.
 
 @ref sampleminimal_overview
 @ref sampleanimate_overview
 @ref sampleartprovider_overview
 @ref samplecalendar_overview
 @ref sampleconfig_overview
 @ref samplecontrols_overview
 @ref sampledebugrpt_overview
 @ref sampledialogs_overview
 @ref sampledialup_overview
 @ref samplednd_overview
 @ref sampleevent_overview
 #Except(ions) sample
 @ref sampleexec_overview
 @ref samplefont_overview
 @ref samplegrid_overview
 @ref samplehtml_overview
 @ref sampleimage_overview
 #Internat(ionalization) sample
 @ref samplelayout_overview
 @ref samplelistctrl_overview
 @ref samplemediaplayer_overview
 @ref samplenotebook_overview
 @ref samplerender_overview
 @ref samplescrollsub_overview
 @ref samplesockets_overview
 @ref samplesound_overview
 @ref samplestatbar_overview
 @ref sampletaborder_overview
 @ref sampletext_overview
 @ref samplethread_overview
 @ref sampletoolbar_overview
 @ref sampletreectrl_overview
 @ref samplewidgets_overview
 @ref samplewizard_overview
 
 
 @section sampleminimal Minimal sample
 
 The minimal sample is what most people will know under the term Hello World,
 i.e. a minimal program that doesn't demonstrate anything apart from what is
 needed to write a program that will display a "hello" dialog. This is usually
 a good starting point for learning how to use wxWidgets.
 
 
 @section sampleanimate Animate sample
 
 The @c animate sample shows how you can use #wxAnimationCtrl
 control and shows concept of a platform-dependent animation encapsulated
 in #wxAnimation.
 
 
 @section sampleartprovider Art provider sample
 
 The @c artprov sample shows how you can customize the look of standard
 wxWidgets dialogs by replacing default bitmaps/icons with your own versions.
 It also shows how you can use wxArtProvider to
 get stock bitmaps for use in your application.
 
 
 @section samplecalendar Calendar sample
 
 This font shows the @ref calendarctrl_overview in action. It
 shows how to configure the control (see the different options in the calendar
 menu) and also how to process the notifications from it.
 
 
 @section sampleconfig Config sample
 
 This sample demonstrates the #wxConfig classes in a platform
 independent way, i.e. it uses text based files to store a given configuration under
 Unix and uses the Registry under Windows.
 See @ref config_overview for the descriptions of all
 features of this class.
 
 
 @section samplecontrols Controls sample
 
 The controls sample is the main test program for most simple controls used in
 wxWidgets. The sample tests their basic functionality, events, placement,
 modification in terms of colour and font as well as the possibility to change
 the controls programmatically, such as adding an item to a list box etc. Apart
 from that, the sample uses a #wxNotebook and tests most
 features of this special control (using bitmap in the tabs, using
 #wxSizers and #constraints within
 notebook pages, advancing pages programmatically and vetoing a page change
 by intercepting the #wxNotebookEvent.
 The various controls tested are listed here:
 
 
 
 
 
 
 #wxButton
 
 
 
 
 Push button control, displaying text
 
 
 
 
 
 #wxBitmapButton
 
 
 
 
 Push button control, displaying a bitmap
 
 
 
 
 
 #wxCheckBox
 
 
 
 
 Checkbox control
 
 
 
 
 
 #wxChoice
 
 
 
 
 Choice control (a combobox without the editable area)
 
 
 
 
 
 #wxComboBox
 
 
 
 
 A choice with an editable area
 
 
 
 
 
 #wxGauge
 
 
 
 
 A control to represent a varying quantity, such as time remaining
 
 
 
 
 
 #wxStaticBox
 
 
 
 
 A static, or group box for visually grouping related controls
 
 
 
 
 
 #wxListBox
 
 
 
 
 A list of strings for single or multiple selection
 
 
 
 
 
 wxSpinCtrl
 
 
 
 
 A spin ctrl with a text field and a 'up-down' control
 
 
 
 
 
 #wxSpinButton
 
 
 
 
 A spin or 'up-down' control
 
 
 
 
 
 #wxStaticText
 
 
 
 
 One or more lines of non-editable text
 
 
 
 
 
 #wxStaticBitmap
 
 
 
 
 A control to display a bitmap
 
 
 
 
 
 #wxRadioBox
 
 
 
 
 A group of radio buttons
 
 
 
 
 
 #wxRadioButton
 
 
 
 
 A round button to be used with others in a mutually exclusive way
 
 
 
 
 
 #wxSlider
 
 
 
 
 A slider that can be dragged by the user
 
 
 
 
 
 
 
 @section sampledebugrpt DebugRpt sample
 
 This sample shows how to use #wxDebugReport class to
 generate a debug report in case of a program crash or otherwise. On start up,
 it proposes to either crash itself (by dereferencing a @NULL pointer) or
 generate debug report without doing it. Next it initializes the debug report
 with standard information adding a custom file to it (just a timestamp) and
 allows to view the information gathered using
 #wxDebugReportPreview.
 For the report processing part of the sample to work you should make available
 a Web server accepting form uploads, otherwise
 #wxDebugReportUpload will report an error.
 
 
 @section sampledialogs Dialogs sample
 
 This sample shows how to use the common dialogs available from wxWidgets. These
 dialogs are described in detail in the @ref commondialogs_overview.
 
 
 @section sampledialup Dialup sample
 
 This sample shows the #wxDialUpManager
 class. In the status bar, it displays the information gathered through its
 interface: in particular, the current connection status (online or offline) and
 whether the connection is permanent (in which case a string 'LAN' appears in
 the third status bar field - but note that you may be on a LAN not
 connected to the Internet, in which case you will not see this) or not.
 Using the menu entries, you may also dial or hang up the line if you have a
 modem attached and (this only makes sense for Windows) list the available
 connections.
 
 
 @section samplednd DnD sample
 
 This sample shows both clipboard and drag and drop in action. It is quite non
 trivial and may be safely used as a basis for implementing the clipboard and
 drag and drop operations in a real-life program.
 When you run the sample, its screen is split in several parts. On the top,
 there are two listboxes which show the standard derivations of
 #wxDropTarget:
 #wxTextDropTarget and
 #wxFileDropTarget.
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
 private #wxDataFormat which means that you may cut and
 paste it or drag and drop (between one and the same or different shapes) from
 one sample instance to another (or the same). However, chances are that no
 other program supports this format and so shapes can also be rendered as
 bitmaps which allows them to be pasted/dropped in many other applications
 (and, under Windows, also as metafiles which are supported by most of Windows
 programs as well - try Write/Wordpad, for example).
 Take a look at DnDShapeDataObject class to see how you may use
 #wxDataObject to achieve this.
 
 
 @section sampleevent Event sample
 
 The event sample demonstrates various features of the wxWidgets events. It
 shows using dynamic events and connecting/disconnecting the event handlers
 during run time and also using
 #PushEventHandler() and
 #PopEventHandler().
 
 
 @section sampleexcept Except(ions) sample
 
 This very simple sample shows how to use C++ exceptions in wxWidgets programs,
 i.e. where to catch the exception which may be thrown by the program code. It
 doesn't do anything very exciting by itself, you need to study its code to
 understand what goes on.
 You need to build the library with @c wxUSE_EXCEPTIONS being set to 1
 and compile your code with C++ exceptions support to be able to build this
 sample.
 
 
 @section sampleexec Exec sample
 
 The exec sample demonstrates the #wxExecute and
 #wxShell functions. Both of them are used to execute the
 external programs and the sample shows how to do this synchronously (waiting
 until the program terminates) or asynchronously (notification will come later).
 It also shows how to capture the output of the child process in both
 synchronous and asynchronous cases and how to kill the processes with
 wxProcess::Kill and test for their existence with
 wxProcess::Exists.
 
 
 @section samplefont Font sample
 
 The font sample demonstrates #wxFont,
 #wxFontEnumerator and
 #wxFontMapper classes. It allows you to see the fonts
 available (to wxWidgets) on the computer and shows all characters of the
 chosen font as well.
 
 
 @section samplegrid Grid sample
 
 TODO.
 
 
 @section samplehtml HTML samples
 
 Eight HTML samples (you can find them in directory @c samples/html)
 cover all features of the HTML sub-library.
 @b Test demonstrates how to create #wxHtmlWindow
 and also shows most supported HTML tags.
 @b Widget shows how you can embed ordinary controls or windows within an
 HTML page. It also nicely explains how to write new tag handlers and extend
 the library to work with unsupported tags.
 @b About may give you an idea how to write good-looking About boxes.
 @b Zip demonstrates use of virtual file systems in wxHTML. The zip archives
 handler (ships with wxWidgets) allows you to access HTML pages stored
 in a compressed archive as if they were ordinary files.
 @b Virtual is yet another virtual file systems demo. This one generates pages at run-time.
 You may find it useful if you need to display some reports in your application.
 @b Printing explains use of #wxHtmlEasyPrinting
 class which serves as as-simple-as-possible interface for printing HTML
 documents without much work. In fact, only few function calls are sufficient.
 @b Help and @b Helpview are variations on displaying HTML help
 (compatible with MS HTML Help Workshop). @e Help shows how to embed
 #wxHtmlHelpController in your application
 while @e Helpview is a simple tool that only pops up the help window and
 displays help books given at command line.
 
 
 @section sampleimage Image sample
 
 The image sample demonstrates use of the #wxImage class
 and shows how to download images in a variety of formats, currently PNG, GIF,
 TIFF, JPEG, BMP, PNM and PCX. The top of the sample shows two rectangles, one
 of which is drawn directly in the window, the other one is drawn into a
 #wxBitmap, converted to a wxImage, saved as a PNG image
 and then reloaded from the PNG file again so that conversions between wxImage
 and wxBitmap as well as loading and saving PNG files are tested.
 At the bottom of the main frame there is a test for using a monochrome bitmap by
 drawing into a #wxMemoryDC. The bitmap is then drawn
 specifying the foreground and background colours with
 wxDC::SetTextForeground and
 wxDC::SetTextBackground (on the left). The
 bitmap is then converted to a wxImage and the foreground colour (black) is
 replaced with red using wxImage::Replace.
 This sample also contains the code for testing the image rotation and resizing
 and using raw bitmap access, see the corresponding menu commands.
 
 
 @section sampleinternat Internat(ionalization) sample
 
 The not very clearly named internat sample demonstrates the wxWidgets
 internationalization (i18n for short from now on) features. To be more
 precise, it only shows localization support, i.e. support for translating the
 program messages into another language while @true i18n would also involve
 changing the other aspects of the programs behaviour.
 More information about this sample can be found in the @c readme.txt file in
 its directory. Please see also @ref internationalization_overview.
 
 
 @section samplelayout Layout sample
 
 The layout sample demonstrates the two different layout systems offered
 by wxWidgets. When starting the program, you will see a frame with some
 controls and some graphics. The controls will change their size whenever
 you resize the entire frame and the exact behaviour of the size changes
 is determined using the #wxLayoutConstraints
 class. See also the #overview and the
 #wxIndividualLayoutConstraint
 class for further information.
 The menu in this sample offers two more tests, one showing how to use
 a #wxBoxSizer in a simple dialog and the other one
 showing how to use sizers in connection with a #wxNotebook
 class. See also #wxSizer.
 
 
 @section samplelistctrl Listctrl sample
 
 This sample shows the #wxListCtrl control. Different modes
 supported by the control (list, icons, small icons, report) may be chosen from
 the menu.
 The sample also provides some timings for adding/deleting/sorting a lot of
 (several thousands) items into the control.
 
 
 @section samplemediaplayer Mediaplayer sample
 
 This sample demonstrates how to use all the features of
 #wxMediaCtrl and play various types of sound, video,
 and other files.
 
 It replaces the old dynamic sample.
 
 @section samplenotebook Notebook sample
 
 This samples shows #wxBookCtrl family of controls.
 Although initially it was written to demonstrate #wxNotebook
 only, it can now be also used to see #wxListbook,
 #wxChoicebook and #wxTreebook in action.
 Test each of the controls, their orientation, images and pages using commands through menu.
 
 
 @section samplerender Render sample
 
 This sample shows how to replace the default wxWidgets
 #renderer and also how to write a shared library
 (DLL) implementing a renderer and load and unload it during the run-time.
 
 
 @section samplescrollsub Scroll subwindow sample
 
 This sample demonstrates use of the #wxScrolledWindow
 class including placing subwindows into it and drawing simple graphics. It uses the
 #SetTargetWindow method and thus the effect
 of scrolling does not show in the scrolled window itself, but in one of its subwindows.
 Additionally, this samples demonstrates how to optimize drawing operations in wxWidgets,
 in particular using the wxWindow::IsExposed method with
 the aim to prevent unnecessary drawing in the window and thus reducing or removing
 flicker on screen.
 
 
 @section samplesockets Sockets sample
 
 The sockets sample demonstrates how to use the communication facilities
 provided by #wxSocket. There are two different
 applications in this sample: a server, which is implemented using a
 #wxSocketServer object, and a client, which
 is implemented as a #wxSocketClient.
 The server binds to the local address, using TCP port number 3000,
 sets up an event handler to be notified of incoming connection requests
 (@b wxSOCKET_CONNECTION events), and sits there, waiting for clients
 (@e listening, in socket parlance). For each accepted connection,
 a new #wxSocketBase object is created. These
 socket objects are independent from the server that created them, so
 they set up their own event handler, and then request to be notified
 of @b wxSOCKET_INPUT (incoming data) or @b wxSOCKET_LOST
 (connection closed at the remote end) events. In the sample, the event
 handler is the same for all connections; to find out which socket the
 event is addressed to, the #GetSocket function
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
 how to use the basic IO calls in #wxSocketBase,
 such as #Read, #Write,
 #ReadMsg and #WriteMsg,
 and how to set up the correct IO flags depending on what you are going to
 do. See the comments in the code for more information. Note that because
 both clients and connection objects in the server set up an event handler
 to catch @b wxSOCKET_LOST events, each one is immediately notified
 if the other end closes the connection.
 There is also a URL test which shows how to use
 the #wxURL class to fetch data from a given URL.
 The sockets sample is work in progress. Some things to do:
 
 
  More tests for basic socket functionality.
  More tests for protocol classes (wxProtocol and its descendants).
  Tests for the recently added (and still in alpha stage) datagram sockets.
  New samples which actually do something useful (suggestions accepted).
 
 
 
 
 @section samplesound Sound sample
 
 The @c sound sample shows how to use #wxSound for simple
 audio output (e.g. notifications).
 
 
 @section samplestatbar Statbar sample
 
 This sample shows how to create and use wxStatusBar. Although most of the
 samples have a statusbar, they usually only create a default one and only
 do it once.
 Here you can see how to recreate the statusbar (with possibly different number
 of fields) and how to use it to show icons/bitmaps and/or put arbitrary
 controls into it.
 
 
 @section sampletaborder Tab order sample
 
 This sample allows to test keyboard navigation (mostly done using the 
 @c TAB key, hence the sample name) between different controls.
 It shows the use of 
 wxWindow::MoveBeforeInTabOrder() and 
 #MoveAfterInTabOrder() methods to change
 the default order of the windows in the navigation chain and of 
 wxWindow::Navigate() for moving focus along this
 chain.
 
 
 @section sampletext Text sample
 
 This sample demonstrates four features: firstly the use and many variants of
 the #wxTextCtrl class (single line, multi line, read only,
 password, ignoring TAB, ignoring ENTER).
 Secondly it shows how to intercept a #wxKeyEvent in both
 the raw form using the @c EVT_KEY_UP and @c EVT_KEY_DOWN macros and the
 higher level from using the @c EVT_CHAR macro. All characters will be logged
 in a log window at the bottom of the main window. By pressing some of the function
 keys, you can test some actions in the text ctrl as well as get statistics on the
 text ctrls, which is useful for testing if these statistics actually are correct.
 Thirdly, on platforms which support it, the sample will offer to copy text to the
 #wxClipboard and to paste text from it. The GTK version will
 use the so called PRIMARY SELECTION, which is the pseudo clipboard under X and
 best known from pasting text to the XTerm program.
 Last not least: some of the text controls have tooltips and the sample also shows
 how tooltips can be centrally disabled and their latency controlled.
 
 
 @section samplethread Thread sample
 
 This sample demonstrates use of threads in connection with GUI programs.
 There are two fundamentally different ways to use threads in GUI programs and
 either way has to take care of the fact that the GUI library itself usually
 is not multi-threading safe, i.e. that it might crash if two threads try to
 access the GUI class simultaneously. One way to prevent that is have a normal
 GUI program in the main thread and some worker threads which work in the
 background. In order to make communication between the main thread and the
 worker threads possible, wxWidgets offers the #wxPostEvent
 function and this sample makes use of this function.
 The other way to use a so called Mutex (such as those offered in the #wxMutex
 class) that prevent threads from accessing the GUI classes as long as any other
 thread accesses them. For this, wxWidgets has the #wxMutexGuiEnter
 and #wxMutexGuiLeave functions, both of which are
 used and tested in the sample as well.
 See also @ref thread_overview and #wxThread.
 
 
 @section sampletoolbar Toolbar sample
 
 The toolbar sample shows the #wxToolBar class in action.
 The following things are demonstrated:
 
 
  Creating the toolbar using wxToolBar::AddTool
 and wxToolBar::AddControl: see
 MyApp::InitToolbar in the sample.
  Using @c EVT_UPDATE_UI handler for automatically enabling/disabling
 toolbar buttons without having to explicitly call EnableTool. This is done
 in MyFrame::OnUpdateCopyAndCut.
  Using wxToolBar::DeleteTool and
 wxToolBar::InsertTool to dynamically update the
 toolbar.
 
 
 Some buttons in the main toolbar are check buttons, i.e. they stay checked when
 pressed. On the platforms which support it, the sample also adds a combobox
 to the toolbar showing how you can use arbitrary controls and not only buttons
 in it.
 If you toggle another toolbar in the sample (using @c Ctrl-A) you will also
 see the radio toolbar buttons in action: the first three buttons form a radio
 group, i.e. checking any of them automatically unchecks the previously
 checked one.
 
 
 @section sampletreectrl Treectrl sample
 
 This sample demonstrates using the #wxTreeCtrl class. Here
 you may see how to process various notification messages sent by this control
 and also when they occur (by looking at the messages in the text control in
 the bottom part of the frame).
 Adding, inserting and deleting items and branches from the tree as well as
 sorting (in default alphabetical order as well as in custom one) is
 demonstrated here as well - try the corresponding menu entries.
 
 
 @section samplewidgets Widgets sample
 
 The widgets sample is the main presentation program for most simple and advanced
 native controls and complex generic widgets provided by wxWidgets.
 The sample tests their basic functionality, events, placement, modification
 in terms of colour and font as well as the possibility to change
 the controls programmatically, such as adding an item to a list box etc.
 All widgets are categorized for easy browsing.
 
 @section samplewizard Wizard sample
 
 This sample shows the so-called wizard dialog (implemented using
 #wxWizard and related classes). It shows almost all
 features supported:
 
 
  Using bitmaps with the wizard and changing them depending on the page
 shown (notice that wxValidationPage in the sample has a different image from
 the other ones)
  Using #TransferDataFromWindow
 to verify that the data entered is correct before passing to the next page
 (done in wxValidationPage which forces the user to check a checkbox before
 continuing).
  Using more elaborated techniques to allow returning to the previous
 page, but not continuing to the next one or vice versa (in wxRadioboxPage)
  This (wxRadioboxPage) page also shows how the page may process the @c Cancel button itself instead of relying on the wizard parent to do it.
  Normally, the order of the pages in the wizard is known at compile-time,
 but sometimes it depends on the user choices: wxCheckboxPage shows how to
 dynamically decide which page to display next (see also
 #wxWizardPage)
 
 */
 
 
