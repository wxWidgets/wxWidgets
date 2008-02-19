/////////////////////////////////////////////////////////////////////////////
// Name:        python
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page python_overview wxPython overview

 This topic was written by Robin Dunn, author of the wxPython wrapper.
 @ref pwhat_overview
 @ref pwhy_overview
 @ref pother_overview
 @ref pusing_overview
 @ref pclasses_overview
 @ref phelp_overview


 @section wxpwhat What is wxPython?

 wxPython is a blending of the wxWidgets GUI classes and the
 #Python programming language.
 @b Python
 So what is Python?  Go to
 #http://www.python.org to learn more,
 but in a nutshell Python is an interpreted,
 interactive, object-oriented programming language. It is often
 compared to Tcl, Perl, Scheme or Java.
 Python combines remarkable power with very clear syntax. It has
 modules, classes, exceptions, very high level dynamic data types, and
 dynamic typing. There are interfaces to many system calls and
 libraries, and new built-in modules are easily written in C or
 C++. Python is also usable as an extension language for applications
 that need a programmable interface.
 Python is copyrighted but freely usable and distributable, even for
 commercial use.
 @b wxPython
 wxPython is a Python package that can be imported at runtime that
 includes a collection of Python modules and an extension module
 (native code). It provides a series of Python classes that mirror (or
 shadow) many of the wxWidgets GUI classes. This extension module
 attempts to mirror the class hierarchy of wxWidgets as closely as
 possible. This means that there is a wxFrame class in wxPython that
 looks, smells, tastes and acts almost the same as the wxFrame class in
 the C++ version.
 wxPython is very versatile. It can be used to create standalone GUI
 applications, or in situations where Python is embedded in a C++
 application as an internal scripting or macro language.
 Currently wxPython is available for Win32 platforms and the GTK
 toolkit (wxGTK) on most Unix/X-windows platforms. See the wxPython
 website #http://wxPython.org/ for
 details about getting wxPython working for you.

 @section wxpwhy Why use wxPython?

 So why would you want to use wxPython over just C++ and wxWidgets?
 Personally I prefer using Python for everything. I only use C++ when I
 absolutely have to eke more performance out of an algorithm, and even
 then I usually code it as an extension module and leave the majority
 of the program in Python.
 Another good thing to use wxPython for is quick prototyping of your
 wxWidgets apps. With C++ you have to continuously go though the
 edit-compile-link-run cycle, which can be quite time consuming. With
 Python it is only an edit-run cycle. You can easily build an
 application in a few hours with Python that would normally take a few
 days or longer with C++. Converting a wxPython app to a C++/wxWidgets app
 should be a straight forward task.

 @section wxpother Other Python GUIs

 There are other GUI solutions out there for Python.
 @b Tkinter
 Tkinter is the de facto standard GUI for Python. It is available
 on nearly every platform that Python and Tcl/TK are. Why Tcl/Tk?
 Well because Tkinter is just a wrapper around Tcl's GUI toolkit, Tk.
 This has its upsides and its downsides...
 The upside is that Tk is a pretty versatile toolkit. It can be made
 to do a lot of things in a lot of different environments. It is fairly
 easy to create new widgets and use them interchangeably in your
 programs.
 The downside is Tcl. When using Tkinter you actually have two
 separate language interpreters running, the Python interpreter and the
 Tcl interpreter for the GUI. Since the guts of Tcl is mostly about
 string processing, it is fairly slow as well. (Not too bad on a fast
 Pentium II, but you really notice the difference on slower machines.)
 It wasn't until the latest version of Tcl/Tk that native Look and
 Feel was possible on non-Motif platforms. This is because Tk
 usually implements its own widgets (controls) even when there are
 native controls available.
 Tkinter is a pretty low-level toolkit. You have to do a lot of work
 (verbose program code) to do things that would be much simpler with a higher
 level of abstraction.
 @b PythonWin
 PythonWin is an add-on package for Python for the Win32 platform. It
 includes wrappers for MFC as well as much of the Win32 API. Because
 of its foundation, it is very familiar for programmers who have
 experience with MFC and the Win32 API. It is obviously not compatible
 with other platforms and toolkits. PythonWin is organized as separate
 packages and modules so you can use the pieces you need without having
 to use the GUI portions.
 @b Others
 There are quite a few other GUI modules available for Python, some in
 active use, some that haven't been updated for ages. Most are simple
 wrappers around some C or C++ toolkit or another, and most are not
 cross-platform compatible. See @ref Graphics_overview
 for a listing of a few of them.

 @section wxpusing Using wxPython

 @b First things first...
 I'm not going to try and teach the Python language here. You can do
 that at the http://www.python.org/doc/tut/tut.html.
 I'm also going to assume that you know a bit about wxWidgets already,
 enough to notice the similarities in the classes used.
 Take a look at the following wxPython program. You can find a similar
 program in the @c wxPython/demo directory, named @c DialogUnits.py. If your
 Python and wxPython are properly installed, you should be able to run
 it by issuing this command:


     @b @c python DialogUnits.py





 @code
 001: ## import all of the wxPython GUI package
 002: from wxPython.wx import *
 003:
 004: ## Create a new frame class, derived from the wxPython Frame.
 005: class MyFrame(wxFrame):
 006:
 007:     def __init__(self, parent, id, title):
 008:         # First, call the base class' __init__ method to create the frame
 009:         wxFrame.__init__(self, parent, id, title,
 010:                          wxPoint(100, 100), wxSize(160, 100))
 011:
 012:         # Associate some events with methods of this class
 013:         EVT_SIZE(self, self.OnSize)
 014:         EVT_MOVE(self, self.OnMove)
 015:
 016:         # Add a panel and some controls to display the size and position
 017:         panel = wxPanel(self, -1)
 018:         wxStaticText(panel, -1, "Size:",
 019:                      wxDLG_PNT(panel, wxPoint(4, 4)),  wxDefaultSize)
 020:         wxStaticText(panel, -1, "Pos:",
 021:                      wxDLG_PNT(panel, wxPoint(4, 14)), wxDefaultSize)
 022:         self.sizeCtrl = wxTextCtrl(panel, -1, "",
 023:                                    wxDLG_PNT(panel, wxPoint(24, 4)),
 024:                                    wxDLG_SZE(panel, wxSize(36, -1)),
 025:                                    wxTE_READONLY)
 026:         self.posCtrl = wxTextCtrl(panel, -1, "",
 027:                                   wxDLG_PNT(panel, wxPoint(24, 14)),
 028:                                   wxDLG_SZE(panel, wxSize(36, -1)),
 029:                                   wxTE_READONLY)
 030:
 031:
 032:     # This method is called automatically when the CLOSE event is
 033:     # sent to this window
 034:     def OnCloseWindow(self, event):
 035:         # tell the window to kill itself
 036:         self.Destroy()
 037:
 038:     # This method is called by the system when the window is resized,
 039:     # because of the association above.
 040:     def OnSize(self, event):
 041:         size = event.GetSize()
 042:         self.sizeCtrl.SetValue("%s, %s" % (size.width, size.height))
 043:
 044:         # tell the event system to continue looking for an event handler,
 045:         # so the default handler will get called.
 046:         event.Skip()
 047:
 048:     # This method is called by the system when the window is moved,
 049:     # because of the association above.
 050:     def OnMove(self, event):
 051:         pos = event.GetPosition()
 052:         self.posCtrl.SetValue("%s, %s" % (pos.x, pos.y))
 053:
 054:
 055: # Every wxWidgets application must have a class derived from wxApp
 056: class MyApp(wxApp):
 057:
 058:     # wxWidgets calls this method to initialize the application
 059:     def OnInit(self):
 060:
 061:         # Create an instance of our customized Frame class
 062:         frame = MyFrame(@NULL, -1, "This is a test")
 063:         frame.Show(@true)
 064:
 065:         # Tell wxWidgets that this is our main window
 066:         self.SetTopWindow(frame)
 067:
 068:         # Return a success flag
 069:         return @true
 070:
 071:
 072: app = MyApp(0)     # Create an instance of the application class
 073: app.MainLoop()     # Tell it to start processing events
 074:
 @endcode



 @b Things to notice


  At line 2 the wxPython classes, constants, and etc. are imported
 into the current module's namespace. If you prefer to reduce
 namespace pollution you can use "@c from wxPython import wx" and
 then access all the wxPython identifiers through the wx module, for
 example, "@c wx.wxFrame".
  At line 13 the frame's sizing and moving events are connected to
 methods of the class. These helper functions are intended to be like
 the event table macros that wxWidgets employs. But since static event
 tables are impossible with wxPython, we use helpers that are named the
 same to dynamically build the table. The only real difference is
 that the first argument to the event helpers is always the window that
 the event table entry should be added to.
  Notice the use of @c wxDLG_PNT and @c wxDLG_SZE in lines 19
 - 29 to convert from dialog units to pixels. These helpers are unique
 to wxPython since Python can't do method overloading like C++.
  There is an @c OnCloseWindow method at line 34 but no call to
 EVT_CLOSE to attach the event to the method. Does it really get
 called?  The answer is, yes it does. This is because many of the
 standard events are attached to windows that have the associated
 standard method names. I have tried to follow the lead of the
 C++ classes in this area to determine what is standard but since
 that changes from time to time I can make no guarantees, nor will it
 be fully documented. When in doubt, use an EVT_*** function.
  At lines 17 to 21 notice that there are no saved references to
 the panel or the static text items that are created. Those of you
 who know Python might be wondering what happens when Python deletes
 these objects when they go out of scope. Do they disappear from the GUI?  They
 don't. Remember that in wxPython the Python objects are just shadows of the
 corresponding C++ objects. Once the C++ windows and controls are
 attached to their parents, the parents manage them and delete them
 when necessary. For this reason, most wxPython objects do not need to
 have a __del__ method that explicitly causes the C++ object to be
 deleted. If you ever have the need to forcibly delete a window, use
 the Destroy() method as shown on line 36.
  Just like wxWidgets in C++, wxPython apps need to create a class
 derived from @c wxApp (line 56) that implements a method named
 @c OnInit, (line 59.) This method should create the application's
 main window (line 62) and use @c wxApp.SetTopWindow() (line 66) to
 inform wxWidgets about it.
  And finally, at line 72 an instance of the application class is
 created. At this point wxPython finishes initializing itself, and calls
 the @c OnInit method to get things started. (The zero parameter here is
 a flag for functionality that isn't quite implemented yet. Just
 ignore it for now.)  The call to @c MainLoop at line 73 starts the event
 loop which continues until the application terminates or all the top
 level windows are closed.



 @section wxpclasses wxWidgets classes implemented in wxPython

 The following classes are supported in wxPython. Most provide nearly
 full implementations of the public interfaces specified in the C++
 documentation, others are less so. They will all be brought as close
 as possible to the C++ spec over time.


  #wxAcceleratorEntry
  #wxAcceleratorTable
  #wxActivateEvent
  #wxBitmap
  #wxBitmapButton
  #wxBitmapDataObject
  wxBMPHandler
  #wxBoxSizer
  #wxBrush
  #wxBusyInfo
  #wxBusyCursor
  #wxButton
  #wxCalculateLayoutEvent
  #wxCalendarCtrl
  #wxCaret
  #wxCheckBox
  #wxCheckListBox
  #wxChoice
  #wxClientDC
  #wxClipboard
  #wxCloseEvent
  #wxColourData
  #wxColourDialog
  #wxColour
  #wxComboBox
  #wxCommandEvent
  #wxConfig
  #wxControl
  #wxCursor
  #wxCustomDataObject
  #wxDataFormat
  #wxDataObject
  #wxDataObjectComposite
  #wxDataObjectSimple
  #wxDateTime
  #wxDateSpan
  #wxDC
  #wxDialog
  #wxDirDialog
  #wxDragImage
  #wxDropFilesEvent
  #wxDropSource
  #wxDropTarget
  #wxEraseEvent
  #wxEvent
  #wxEvtHandler
  #wxFileConfig
  #wxFileDataObject
  #wxFileDialog
  #wxFileDropTarget
  #wxFileSystem
  #wxFileSystemHandler
  #wxFocusEvent
  #wxFontData
  #wxFontDialog
  #wxFont
  #wxFrame
  #wxFSFile
  #wxGauge
  wxGIFHandler
  #wxGLCanvas
  #wxHtmlCell
  #wxHtmlContainerCell
  #wxHtmlDCRenderer
  #wxHtmlEasyPrinting
  #wxHtmlParser
  #wxHtmlTagHandler
  #wxHtmlTag
  #wxHtmlWinParser
  #wxHtmlPrintout
  #wxHtmlWinTagHandler
  #wxHtmlWindow
  #wxIconizeEvent
  #wxIcon
  #wxIdleEvent
  #wxImage
  #wxImageHandler
  #wxImageList
  #wxIndividualLayoutConstraint
  #wxInitDialogEvent
  #wxInputStream
  #wxInternetFSHandler
  #wxJoystickEvent
  wxJPEGHandler
  #wxKeyEvent
  #wxLayoutAlgorithm
  #wxLayoutConstraints
  #wxListBox
  #wxListCtrl
  #wxListEvent
  #wxListItem
  #wxMask
  #wxMaximizeEvent
  #wxMDIChildFrame
  #wxMDIClientWindow
  #wxMDIParentFrame
  #wxMemoryDC
  #wxMemoryFSHandler
  #wxMenuBar
  #wxMenuEvent
  #wxMenuItem
  #wxMenu
  #wxMessageDialog
  #wxMetaFileDC
  #wxMiniFrame
  #wxMouseEvent
  #wxMoveEvent
  #wxNotebookEvent
  #wxNotebook
  #wxPageSetupDialogData
  #wxPageSetupDialog
  #wxPaintDC
  #wxPaintEvent
  #wxPalette
  #wxPanel
  #wxPen
  wxPNGHandler
  #wxPoint
  #wxPostScriptDC
  #wxPreviewFrame
  #wxPrintData
  #wxPrintDialogData
  #wxPrintDialog
  #wxPrinter
  #wxPrintPreview
  #wxPrinterDC
  #wxPrintout
  #wxProcess
  #wxQueryLayoutInfoEvent
  #wxRadioBox
  #wxRadioButton
  #wxRealPoint
  #wxRect
  #wxRegionIterator
  #wxRegion
  #wxSashEvent
  #wxSashLayoutWindow
  #wxSashWindow
  #wxScreenDC
  #wxScrollBar
  #wxScrollEvent
  #wxScrolledWindow
  #wxScrollWinEvent
  wxShowEvent
  #wxSingleChoiceDialog
  #wxSizeEvent
  #wxSize
  #wxSizer
  #wxSizerItem
  #wxSlider
  #wxSpinButton
  #wxSpinEvent
  #wxSplitterWindow
  #wxStaticBitmap
  #wxStaticBox
  #wxStaticBoxSizer
  #wxStaticLine
  #wxStaticText
  #wxStatusBar
  #wxSysColourChangedEvent
  #wxTaskBarIcon
  #wxTextCtrl
  #wxTextDataObject
  #wxTextDropTarget
  #wxTextEntryDialog
  #wxTimer
  #wxTimerEvent
  #wxTimeSpan
  #wxTipProvider
  wxToolBarTool
  #wxToolBar
  #wxToolTip
  #wxTreeCtrl
  #wxTreeEvent
  #wxTreeItemData
  wxTreeItemId
  #wxUpdateUIEvent
  #wxValidator
  #wxWindowDC
  #wxWindow
  #wxZipFSHandler



 @section wxphelp Where to go for help

 Since wxPython is a blending of multiple technologies, help comes from
 multiple sources. See
 #http://wxpython.org/ for details on
 various sources of help, but probably the best source is the
 wxPython-users mail list. You can view the archive or subscribe by
 going to
 #http://lists.wxwindows.org/mailman/listinfo/wxpython-users
 Or you can send mail directly to the list using this address:
 wxpython-users@lists.wxwindows.org

 */


