/////////////////////////////////////////////////////////////////////////////
// Name:        const_wxusedef.h
// Purpose:     wxUSE preprocessor symbols
// Author:      Tim Stahlhut
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page wxusedef wxUSE Preprocessor symbols defined by wxWidgets

 This section documents the wxUSE preprocessor symbols used in the wxWidgets
 source, grouped by category (and sorted by alphabetical order inside each
 category). These symbols are always defined and whether the given feature is
 active or not depends on their value: if defined as @c 1, feature is active,
 otherwise it is disabled. Because of this these symbols should be always tested
 using @c #if and not @c #ifdef.
 
 @li @ref wxusedefmulti
 @li @ref wxusedefunix
 @li @ref wxusedefx11
 @li @ref wxusedefgtk
 @li @ref wxusedefmac
 @li @ref wxusedefmotif
 @li @ref wxusedefcocoa
 @li @ref wxusedefos2
 @li @ref wxusedefmsw
 @li @ref wxusedefuniv


 <hr>


 TODO: what can we use here instead of \twocolitem to keep text readable??


 @section wxusedefmulti Generic wxUSE preprocessor symbols
 
 \twocolitem{wxUSE_ABOUTDLG}{Use \helpref{wxAboutDialogInfo}{wxaboutdialoginfo} class.}
 \twocolitem{wxUSE_ACCEL}{Use \helpref{wxAcceleratorTable}{wxacceleratortable}/Entry classes and support for them in \helpref{wxMenu}{wxmenu}(Bar).}
 \twocolitem{wxUSE_AFM_FOR_POSTSCRIPT}{In \helpref{wxPostScriptDC}{wxpostscriptdc} class use AFM (adobe font metrics) file for character widths.}
 \twocolitem{wxUSE_ANIMATIONCTRL}{Use \helpref{wxAnimationCtrl}{wxanimationctrl} class.}
 \twocolitem{wxUSE_APPLE_IEEE}{IEEE Extended to/from double routines; see src/common/extended.c file.}
 \twocolitem{wxUSE_ARCHIVE_STREAMS}{Enable streams for archive formats.}
 \twocolitem{wxUSE_AUI}{Use AUI (dockable windows) library.}
 \twocolitem{wxUSE_BASE64}{Enables Base64 support.}
 \twocolitem{wxUSE_BITMAPCOMBOBOX}{Use \helpref{wxBitmapComboBox}{wxbitmapcombobox} class.}
 \twocolitem{wxUSE_BMPBUTTON}{Use \helpref{wxBitmapButton}{wxbitmapbutton} class.}
 \twocolitem{wxUSE_BUSYINFO}{Use \helpref{wxBusyInfo}{wxbusyinfo} class.}
 \twocolitem{wxUSE_BUTTON}{Use \helpref{wxButton}{wxbutton} class.}
 \twocolitem{wxUSE_CALENDARCTRL}{Use \helpref{wxCalendarCtrl}{wxcalendarctrl} class.}
 \twocolitem{wxUSE_CARET}{Use \helpref{wxCaret}{wxcaret} class.}
 \twocolitem{wxUSE_CHECKBOX}{Use \helpref{wxCheckBox}{wxcheckbox} class.}
 \twocolitem{wxUSE_CHECKLISTBOX}{Use \helpref{wxCheckListBox}{wxchecklistbox} class.}
 \twocolitem{wxUSE_CHOICE}{Use \helpref{wxChoice}{wxchoice} class.}
 \twocolitem{wxUSE_CHOICEBOOK}{Use \helpref{wxChoicebook}{wxchoicebook} class.}
 \twocolitem{wxUSE_CHOICEDLG}{Use \helpref{wxSingleChoiceDialog}{wxsinglechoicedialog} or \helpref{wxMultiChoiceDialog}{wxmultichoicedialog} classes.}
 \twocolitem{wxUSE_CLIPBOARD}{Use \helpref{wxClipboard}{wxclipboard} class.}
 \twocolitem{wxUSE_CMDLINE_PARSER}{Use \helpref{wxCmdLineParser}{wxcmdlineparser} class.}
 \twocolitem{wxUSE_COLLPANE}{Use \helpref{wxCollapsiblePane}{wxcollapsiblepane} class.}
 \twocolitem{wxUSE_COLOURDLG}{Use \helpref{wxColourDialog}{wxcolourdialog} class.}
 \twocolitem{wxUSE_COLOURPICKERCTRL}{Use \helpref{wxColourPickerCtrl}{wxcolourpickerctrl} class.}
 \twocolitem{wxUSE_COMBOBOX}{Use \helpref{wxComboBox}{wxcombobox} class.}
 \twocolitem{wxUSE_COMBOCTRL}{Use \helpref{wxComboCtrl}{wxcomboctrl} class.}
 \twocolitem{wxUSE_CONFIG}{Use \helpref{wxConfig}{wxconfigbase} and related classes.}
 \twocolitem{wxUSE_CONFIG_NATIVE}{When enabled use native OS configuration instead of the \helpref{wxFileConfig}{wxfileconfig} class.}
 \twocolitem{wxUSE_CONSOLE_EVENTLOOP}{Enable event loop in console programs.}
 \twocolitem{wxUSE_CONSTRAINTS}{Use \helpref{wxLayoutConstraints}{wxlayoutconstraints}}
 \twocolitem{wxUSE_CONTROLS}{If set to $0$, no classes deriving from \helpref{wxControl}{wxcontrol} can be used.}
 \twocolitem{wxUSE_DATAOBJ}{Use \helpref{wxDataObject}{wxdataobject} and related classes.}
 \twocolitem{wxUSE_DATAVIEWCTRL}{Use \helpref{wxDataViewCtrl}{wxdataviewctrl} class.}
 \twocolitem{wxUSE_DATEPICKCTRL}{Use \helpref{wxDatePickerCtrl}{wxdatepickerctrl} class.}
 \twocolitem{wxUSE_DATETIME}{Use \helpref{wxDateTime}{wxdatetime} and related classes.}
 \twocolitem{wxUSE_DBGHELP}{Use wxDbgHelpDLL class.}
 \twocolitem{wxUSE_DEBUG_CONTEXT}{Use \helpref{wxDebugContext}{wxdebugcontext} class.}
 \twocolitem{wxUSE_DEBUG_NEW_ALWAYS}{See \helpref{Debugging overview}{wxdebugcontextoverview}}
 \twocolitem{wxUSE_DEBUGREPORT}{Use \helpref{wxDebugReport}{wxdebugreport} class.}
 \twocolitem{wxUSE_DIALUP_MANAGER}{Use \helpref{wxDialUpManager}{wxdialupmanager} and related classes.}
 \twocolitem{wxUSE_DIRDLG}{Use \helpref{wxDirDialog}{wxdirdialog} class.}
 \twocolitem{wxUSE_DIRPICKERCTRL}{Use \helpref{wxDirPickerCtrl}{wxdirpickerctrl} class.}
 \twocolitem{wxUSE_DISPLAY}{Use \helpref{wxDisplay}{wxdisplay} and related classes.}
 \twocolitem{wxUSE_DOC_VIEW_ARCHITECTURE}{Use \helpref{wxDocument}{wxdocument} and related classes.}
 \twocolitem{wxUSE_DRAG_AND_DROP}{Use \helpref{Drag and drop}{wxdndoverview} classes.}
 \twocolitem{wxUSE_DRAGIMAGE}{Use \helpref{wxDragImage}{wxdragimage} class.}
 \twocolitem{wxUSE_DYNAMIC_LOADER}{Use wxPluginManager and related classes. Requires \helpref{wxDynamicLibrary}{wxdynamiclibrary}}
 \twocolitem{wxUSE_DYNLIB_CLASS}{Use \helpref{wxDynamicLibrary}{wxdynamiclibrary}}
 \twocolitem{wxUSE_EDITABLELISTBOX}{Use \helpref{wxEditableListBox}{wxeditablelistbox} class.}
 \twocolitem{wxUSE_EXCEPTIONS}{Use \helpref{exception handling}{exceptionsoverview}.}
 \twocolitem{wxUSE_EXPAT}{enable XML support using expat parser.}
 \twocolitem{wxUSE_EXTENDED_RTTI}{Use extended RTTI, see also \helpref{Runtime class information (RTTI)}{runtimeclassoverview}}
 \twocolitem{wxUSE_FFILE}{Use \helpref{wxFFile}{wxffile} class.}
 \twocolitem{wxUSE_FILE}{Use \helpref{wxFile}{wxfile} class.}
 \twocolitem{wxUSE_FILECONFIG}{Use \helpref{wxFileConfig}{wxfileconfig} class.}
 \twocolitem{wxUSE_FILECTRL}{Use \helpref{wxFileCtrl}{wxfilectrl} class.}
 \twocolitem{wxUSE_FILEDLG}{Use \helpref{wxFileDialog}{wxfiledialog} class.}
 \twocolitem{wxUSE_FILEPICKERCTRL}{Use \helpref{wxFilePickerCtrl}{wxfilepickerctrl} class.}
 \twocolitem{wxUSE_FILESYSTEM}{Use \helpref{wxFileSystem}{wxfilesystem} and related classes.}
 \twocolitem{wxUSE_FINDREPLDLG}{Use \helpref{wxFindReplaceDialog}{wxfindreplacedialog} class.}
 \twocolitem{wxUSE_FONTDLG}{Use \helpref{wxFontDialog}{wxfontdialog} class.}
 \twocolitem{wxUSE_FONTENUM}{Use \helpref{wxFontEnumerator}{wxfontenumerator} class.}
 \twocolitem{wxUSE_FONTMAP}{Use \helpref{wxFontMapper}{wxfontmapper} class.}
 \twocolitem{wxUSE_FONTPICKERCTRL}{Use \helpref{wxFontPickerCtrl}{wxfontpickerctrl} class.}
 \twocolitem{wxUSE_FS_ARCHIVE}{Use virtual archive filesystems like wxArchiveFSHandler in \helpref{wxFileSystem}{wxfilesystem} class.}
 \twocolitem{wxUSE_FS_INET}{Use virtual HTTP/FTP filesystems like wxInternetFSHandler in \helpref{wxFileSystem}{wxfilesystem} class.}
 \twocolitem{wxUSE_FS_ZIP}{Please use wxUSE_FS_ARCHIVE instead.}
 \twocolitem{wxUSE_FSVOLUME}{Use wxFSVolume class.}
 \twocolitem{wxUSE_GAUGE}{Use \helpref{wxGauge}{wxgauge} class.}
 \twocolitem{wxUSE_GENERIC_DRAGIMAGE}{Used in wxDragImage sample.}
 \twocolitem{wxUSE_GENERIC_DRAWELLIPSE}{See comment in wx/dc.h file.}
 \twocolitem{wxUSE_GEOMETRY}{Use common geometry classes}
 \twocolitem{wxUSE_GIF}{Use GIF \helpref{wxImageHandler}{wximagehandler}}
 \twocolitem{wxUSE_GLCANVAS}{Enables OpenGL support.}
 \twocolitem{wxUSE_GLOBAL_MEMORY_OPERATORS}{Override global operators \texttt{new} and \texttt{delete} to use wxWidgets memory leak detection}
 \twocolitem{wxUSE_GRAPHICS_CONTEXT}{Use \helpref{wxGraphicsContext}{wxgraphicscontext} and related classes.}
 \twocolitem{wxUSE_GRID}{Use \helpref{wxGrid}{wxgrid} and related classes.}
 \twocolitem{wxUSE_GUI}{Use the GUI classes; if set to $0$ only non-GUI classes are available.}
 \twocolitem{wxUSE_HELP}{Use \helpref{wxHelpController}{wxhelpcontroller} and related classes.}
 \twocolitem{wxUSE_HTML}{Use \helpref{wxHtmlWindow}{wxhtmlwindow} and related classes.}
 \twocolitem{wxUSE_HYPERLINKCTRL}{Use \helpref{wxHyperlinkCtrl}{wxhyperlinkctrl}}
 \twocolitem{wxUSE_ICO_CUR}{Support Windows ICO and CUR formats.}
 \twocolitem{wxUSE_IFF}{Enables the \helpref{wxImage}{wximage} handler for Amiga IFF images.}
 \twocolitem{wxUSE_IMAGE}{Use \helpref{wxImage}{wximage} and related classes.}
 \twocolitem{wxUSE_IMAGLIST}{Use \helpref{wxImageList}{wximagelist} class.}
 \twocolitem{wxUSE_INTL}{Use \helpref{wxLocale}{wxlocale} and related classes.}
 \twocolitem{wxUSE_IOSTREAMH}{Use header <iostream.h> instead of <iostream>.}
 \twocolitem{wxUSE_IPC}{Use interprocess communication classes.}
 \twocolitem{wxUSE_IPV6}{Use experimental \helpref{wxIPV6address}{wxipaddress} and related classes.}
 \twocolitem{wxUSE_JOYSTICK}{Use \helpref{wxJoystick}{wxjoystick} class.}
 \twocolitem{wxUSE_LIBJPEG}{Enables JPEG format support (requires libjpeg).}
 \twocolitem{wxUSE_LIBPNG}{Enables PNG format support (requires libpng). Also requires wxUSE_ZLIB.}
 \twocolitem{wxUSE_LIBTIFF}{Enables TIFF format support (requires libtiff).}
 \twocolitem{wxUSE_LISTBOOK}{Use \helpref{wxListbook}{wxlistbook} class.}
 \twocolitem{wxUSE_LISTBOX}{Use \helpref{wxListBox}{wxlistbox} class.}
 \twocolitem{wxUSE_LISTCTRL}{Use \helpref{wxListCtrl}{wxlistctrl} class.}
 \twocolitem{wxUSE_LOG}{Use \helpref{wxLog}{wxlog} and related classes.}
 \twocolitem{wxUSE_LOG_DEBUG}{Enabled when wxLog used with __WXDEBUG__ defined.}
 \twocolitem{wxUSE_LOG_DIALOG}{Use wxLogDialog class.}
 \twocolitem{wxUSE_LOGGUI}{Use \helpref{wxLogGui}{wxloggui} class.}
 \twocolitem{wxUSE_LOGWINDOW}{Use wxLogFrame class.}
 \twocolitem{wxUSE_LONGLONG}{Use \helpref{wxLongLong}{wxlonglong} class.}
 \twocolitem{wxUSE_LONGLONG_NATIVE}{Use native \texttt{long long} type in \helpref{wxLongLong}{wxlonglong} implementation.}
 \twocolitem{wxUSE_LONGLONG_WX}{Use generic wxLongLong implementation.}
 \twocolitem{wxUSE_MDI}{Use \helpref{wxMDIParentFrame}{wxmdiparentframe} and \helpref{wxMDIChildFrame}{wxmdichildframe}}
 \twocolitem{wxUSE_MDI_ARCHITECTURE}{Use MDI-based document-view classes.}
 \twocolitem{wxUSE_MEDIACTRL}{Use \helpref{wxMediaCtrl}{wxmediactrl}.}
 \twocolitem{wxUSE_MEMORY_TRACING}{Use wxWidgets memory leak detection, not recommended if using another memory debugging tool.}
 \twocolitem{wxUSE_MENUS}{Use \helpref{wxMenu}{wxmenu} and related classes.}
 \twocolitem{wxUSE_METAFILE}{Use \helpref{wxMetaFile}{wxmetafile} and related classes.}
 \twocolitem{wxUSE_MIMETYPE}{Use \helpref{wxFileType}{wxfiletype} class.}
 \twocolitem{wxUSE_MINIFRAME}{Use \helpref{wxMiniFrame}{wxminiframe} class.}
 \twocolitem{wxUSE_MOUSEWHEEL}{Support mouse wheel events.}
 \twocolitem{wxUSE_MSGDLG}{Use \helpref{wxMessageDialog}{wxmessagedialog} class and \helpref{wxMessageBox}{wxmessagebox} function.}
 \twocolitem{wxUSE_NATIVE_STATUSBAR}{Use native \helpref{wxStatusBar}{wxstatusbar} class.}
 \twocolitem{wxUSE_NOTEBOOK}{Use \helpref{wxNotebook}{wxnotebook} and related classes.}
 \twocolitem{wxUSE_NUMBERDLG}{Use wxNumberEntryDialog class.}
 \twocolitem{wxUSE_ODCOMBOBOX}{Use \helpref{wxOwnerDrawnComboBox}{wxownerdrawncombobox} class.}
 \twocolitem{wxUSE_ON_FATAL_EXCEPTION}{Catch signals in \helpref{wxApp::OnFatalException}{wxapponfatalexception} method.}
 \twocolitem{wxUSE_OPENGL}{Please use wxUSE_GLCANVAS to test for enabled OpenGL support instead.}
 \twocolitem{wxUSE_OWNER_DRAWN}{Use interface for owner-drawn GUI elements.}
 \twocolitem{wxUSE_PALETTE}{Use \helpref{wxPalette}{wxpalette} and related classes.}
 \twocolitem{wxUSE_PCX}{Enables \helpref{wxImage}{wximage} PCX handler.}
 \twocolitem{wxUSE_PNM}{Enables \helpref{wxImage}{wximage} PNM handler.}
 \twocolitem{wxUSE_POPUPWIN}{Use wxPopupWindow class.}
 \twocolitem{wxUSE_POSTSCRIPT}{Use wxPostScriptPrinter class.}
 \twocolitem{wxUSE_PRINTF_POS_PARAMS}{Use \helpref{wxVsnprintf}{wxvsnprintf} which supports positional parameters.}
 \twocolitem{wxUSE_PRINTING_ARCHITECTURE}{Enable printer classes.}
 \twocolitem{wxUSE_PROGRESSDLG}{Enables progress dialog classes.}
 \twocolitem{wxUSE_PROTOCOL}{Use \helpref{wxProtocol}{wxprotocol} and derived classes.}
 \twocolitem{wxUSE_PROTOCOL_FILE}{Use wxFileProto class. (requires \helpref{wxProtocol}{wxprotocol})}
 \twocolitem{wxUSE_PROTOCOL_FTP}{Use \helpref{wxFTP}{wxftp} class. (requires \helpref{wxProtocol}{wxprotocol})}
 \twocolitem{wxUSE_PROTOCOL_HTTP}{Use \helpref{wxHTTP}{wxhttp} class. (requires \helpref{wxProtocol}{wxprotocol})}
 \twocolitem{wxUSE_RADIOBOX}{Use \helpref{wxRadioBox}{wxradiobox} class.}
 \twocolitem{wxUSE_RADIOBTN}{Use \helpref{wxRadioButton}{wxradiobutton} class.}
 \twocolitem{wxUSE_REGEX}{Use \helpref{wxRegEx}{wxregex} class.}
 \twocolitem{wxUSE_RICHTEXT}{Use \helpref{wxRichTextCtrl}{wxrichtextctrl} class.}
 \twocolitem{wxUSE_RICHTEXT_XML_HANDLER}{See src/xrc/xh_richtext.cpp file.}
 \twocolitem{wxUSE_SASH}{Use \helpref{wxSashWindow}{wxsashwindow} class.}
 \twocolitem{wxUSE_SCROLLBAR}{Use \helpref{wxScrollBar}{wxscrollbar} class.}
 \twocolitem{wxUSE_SEARCHCTRL}{Use \helpref{wxSearchCtrl}{wxsearchctrl} class.}
 \twocolitem{wxUSE_SELECT_DISPATCHER}{Use wxSelectDispatcher class.}
 \twocolitem{wxUSE_SLIDER}{Use \helpref{wxSlider}{wxslider} class.}
 \twocolitem{wxUSE_SNGLINST_CHECKER}{Use \helpref{wxSingleInstanceChecker}{wxsingleinstancechecker} class.}
 \twocolitem{wxUSE_SOCKETS}{Enables Network address classes.}
 \twocolitem{wxUSE_SOUND}{Use \helpref{wxSound}{wxsound} class.}
 \twocolitem{wxUSE_SPINBTN}{Use \helpref{wxSpinButton}{wxspinbutton} class.}
 \twocolitem{wxUSE_SPINCTRL}{Use \helpref{wxSpinCtrl}{wxspinctrl} class.}
 \twocolitem{wxUSE_SPLASH}{Use \helpref{wxSplashScreen}{wxsplashscreen} class.}
 \twocolitem{wxUSE_SPLINES}{Provide methods for spline drawing in wxDC.}
 \twocolitem{wxUSE_SPLITTER}{Use \helpref{wxSplitterWindow}{wxsplitterwindow} class.}
 \twocolitem{wxUSE_STACKWALKER}{Enables \helpref{wxStackWalker}{wxstackwalker} and related classes.}
 \twocolitem{wxUSE_STARTUP_TIPS}{Use startup tips, \helpref{wxTipProvider}{wxtipprovider} class.}
 \twocolitem{wxUSE_STATBMP}{Use \helpref{wxStaticBitmap}{wxstaticbitmap} class.}
 \twocolitem{wxUSE_STATBOX}{Use \helpref{wxStaticBox}{wxstaticbox} class.}
 \twocolitem{wxUSE_STATLINE}{Use \helpref{wxStaticLine}{wxstaticline} class.}
 \twocolitem{wxUSE_STATTEXT}{Use \helpref{wxStaticText}{wxstatictext} class.}
 \twocolitem{wxUSE_STATUSBAR}{Use \helpref{wxStatusBar}{wxstatusbar} class.}
 \twocolitem{wxUSE_STC}{Use wxStyledTextCtrl.}
 \twocolitem{wxUSE_STD_IOSTREAM}{Use standard C++ stream classes.}
 \twocolitem{wxUSE_STD_STRING}{Use standard C++ string classes.}
 \twocolitem{wxUSE_STDPATHS}{Use \helpref{wxStandardPaths}{wxstandardpaths} class.}
 \twocolitem{wxUSE_STL}{Use Standard Template Library for the container classes and \helpref{wxString}{wxstring} implementation.}
 \twocolitem{wxUSE_STOPWATCH}{Use \helpref{wxStopWatch}{wxstopwatch} class.}
 \twocolitem{wxUSE_STREAMS}{Enable stream classes.}
 \twocolitem{wxUSE_SVG}{Use wxSVGFileDC class.}
 \twocolitem{wxUSE_SYSTEM_OPTIONS}{Use \helpref{wxSystemOptions}{wxsystemoptions} class.}
 \twocolitem{wxUSE_TAB_DIALOG}{Use the obsolete wxTabControl class.}
 \twocolitem{wxUSE_TARSTREAM}{Enable Tar files support.}
 \twocolitem{wxUSE_TASKBARICON}{Use \helpref{wxTaskBarIcon}{wxtaskbaricon} class.}
 \twocolitem{wxUSE_TEXTBUFFER}{Use wxTextBuffer class.}
 \twocolitem{wxUSE_TEXTCTRL}{Use \helpref{wxTextCtrl}{wxtextctrl} class.}
 \twocolitem{wxUSE_TEXTDLG}{Use \helpref{wxTextEntryDialog}{wxtextentrydialog} class.}
 \twocolitem{wxUSE_TEXTFILE}{Use \helpref{wxTextFile}{wxtextfile} class.}
 \twocolitem{wxUSE_TGA}{Enable \helpref{wxImage}{wximage} TGA handler.}
 \twocolitem{wxUSE_THREADS}{Use \helpref{wxThread}{wxthread} and related classes.}
 \twocolitem{wxUSE_TIMER}{Use \helpref{wxTimer}{wxtimer} class.}
 \twocolitem{wxUSE_TIPWINDOW}{Use \helpref{wxTipWindow}{wxtipwindow} class.}
 \twocolitem{wxUSE_TOGGLEBTN}{Use \helpref{wxToggleButton}{wxtogglebutton} class.}
 \twocolitem{wxUSE_TOOLBAR}{Use \helpref{wxToolBar}{wxtoolbar} class.}
 \twocolitem{wxUSE_TOOLBAR_NATIVE}{Use native \helpref{wxToolBar}{wxtoolbar} class.}
 \twocolitem{wxUSE_TOOLBOOK}{Use \helpref{wxToolbook}{wxtoolbook} class.}
 \twocolitem{wxUSE_TOOLTIPS}{Use \helpref{wxToolTip}{wxtooltip} class.}
 \twocolitem{wxUSE_TREEBOOK}{Use \helpref{wxTreebook}{wxtreebook} class.}
 \twocolitem{wxUSE_TREECTRL}{Use \helpref{wxTreeCtrl}{wxtreectrl} class.}
 \twocolitem{wxUSE_TTM_WINDOWFROMPOINT}{Obsolete, do not use.}
 \twocolitem{wxUSE_UNICODE}{Compiled with Unicode support.}
 \twocolitem{wxUSE_UNICODE_UTF8}{Compiled with UTF8 support.}
 \twocolitem{wxUSE_UNICODE_WCHAR}{Compiled with Unicode support and using wchar_t type.}
 \twocolitem{wxUSE_URL}{Use \helpref{wxURL}{wxurl} class.}
 \twocolitem{wxUSE_URL_NATIVE}{Use native support for some operations with \helpref{wxURL}{wxurl}.}
 \twocolitem{wxUSE_UTF8_LOCALE_ONLY}{Build wxWidgets to support running only under UTF-8 (and C) locale. This eliminates the code necessary for conversions from the other locales and reduces the library size; useful for embedded systems.}
 \twocolitem{wxUSE_VALIDATORS}{Use \helpref{wxValidator}{wxvalidator} class.}
 \twocolitem{wxUSE_VARIANT}{Use \helpref{wxVariant}{wxvariant} class.}
 \twocolitem{wxUSE_WIZARDDLG}{Use \helpref{wxWizard}{wxwizard} class.}
 \twocolitem{wxUSE_WXHTML_HELP}{Use \helpref{wxHtmlHelpController}{wxhtmlhelpcontroller} and related classes.}
 \twocolitem{wxUSE_XML}{Use XML parsing classes.}
 \twocolitem{wxUSE_XPM}{Enable XPM reader for \helpref{wxImage}{wximage} and \helpref{wxBitmap}{wxbitmap} classes.}
 \twocolitem{wxUSE_XRC}{Use XRC XML-based resource system.}
 \twocolitem{wxUSE_ZIPSTREAM}{Enable streams for Zip files.}
 \twocolitem{wxUSE_ZLIB}{Use \helpref{wxZlibInput}{wxzlibinputstream} and \helpref{wxZlibOutputStream}{wxzliboutputstream} classes, required by wxUSE_LIBPNG.}
 
 
 @section wxusedefunix wxUSE preprocessor symbols used only under Unix platforms
 
 \twocolitem{wxUSE_EPOLL_DISPATCHER}{Use wxEpollDispatcher class. See also wxUSE_SELECT_DISPATCHER.}
 \twocolitem{wxUSE_GSTREAMER}{Use GStreamer library in \helpref{wxMediaCtrl}{wxmediactrl}.}
 \twocolitem{wxUSE_LIBMSPACK}{Use libmspack library.}
 \twocolitem{wxUSE_LIBSDL}{Use SDL for wxSound implementation.}
 \twocolitem{wxUSE_PLUGINS}{See also wxUSE_LIBSDL.}
 \twocolitem{wxUSE_UNIX}{Enabled on Unix Platform.}
 
 
 @section wxusedefx11 wxUSE preprocessor symbols used only in wxX11 Platform
 
 \twocolitem{wxUSE_NANOX}{Use NanoX.}
 \twocolitem{wxUSE_UNIV_TEXTCTRL}{Use wxUniv's implementation of \helpref{wxTextCtrl}{wxtextctrl} class.}
 
 
 @section wxusedefgtk wxUSE preprocessor symbols used only in wxGTK port
 
 \twocolitem{wxUSE_DETECT_SM}{Use code to detect X11 session manager.}
 \twocolitem{wxUSE_GTKPRINT}{Use GTK+ printing support.}
 \twocolitem{wxUSE_LIBGNOMEPRINT}{Use GNOME printing support.}
 \twocolitem{wxUSE_LIBGNOMEVFS}{Use GNOME VFS support. Currently has no effect. }
 \twocolitem{wxUSE_LIBHILDON}{Use Hildon framework for Nokia 770. Currently has no effect. }
 
 
 @section wxusedefmac wxUSE preprocessor symbols used only in wxMac port
 
 \twocolitem{wxUSE_MAC_CRITICAL_REGION_MUTEX}{See src/mac/carbon/thread.cpp file.}
 \twocolitem{wxUSE_MAC_PTHREADS_MUTEX}{See src/mac/carbon/thread.cpp file.}
 \twocolitem{wxUSE_MAC_SEMAPHORE_MUTEX}{See src/mac/carbon/thread.cpp file.}
 \twocolitem{wxUSE_WEBKIT}{Use wxWebKitCtrl class.}
 
 
 @section wxusedefmotif wxUSE preprocessor symbols used only in wxMotif port
 
 \twocolitem{wxUSE_GADGETS}{Use xmCascadeButtonGadgetClass, xmLabelGadgetClass, xmPushButtonGadgetClass and xmToggleButtonGadgetClass classes.}
 \twocolitem{wxUSE_INVISIBLE_RESIZE}{See src/motif/dialog.cpp file.}
 
 
 @section wxusedefcocoa wxUSE preprocessor symbols used only in Cocoa port
 
 \twocolitem{wxUSE_OBJC_UNIQUIFYING}{Enable Objective-C class name uniquifying.}
 
 
 @section wxusedefos2 wxUSE preprocessor symbols used only in OS2 port
 
 \twocolitem{wxUSE_CONSOLEDEBUG}{See src/os2/app.cpp file.}
 \twocolitem{wxUSE_DDE}{See src/os2/mimetype.cpp file.}
 \twocolitem{wxUSE_IMAGE_LOADING_IN_MSW}{See src/os2/clipbrd.cpp file.}
 \twocolitem{wxUSE_IMAGE_LOADING_IN_OS2}{See src/os2/gdiimage.cpp file.}
 \twocolitem{wxUSE_NET_API}{Use NetBios32GetInfo API call.}
 \twocolitem{wxUSE_RESOURCE_LOADING_IN_OS2}{See src/os2/gdiimage.cpp file.}
 
 
 @section wxusedefmsw wxUSE preprocessor symbols used only in wxMSW port
 
 \twocolitem{wxUSE_ACCESSIBILITY}{Enable accessibility support}
 \twocolitem{wxUSE_ACTIVEX}{Use \helpref{wxActiveXContainer}{wxactivexcontainer} and related classes.}
 \twocolitem{wxUSE_COMBOCTRL_POPUP_ANIMATION}{See wx/msw/combo.h file.}
 \twocolitem{wxUSE_COMCTL32_SAFELY}{See src/msw/treectrl.cpp file.}
 \twocolitem{wxUSE_COMMON_DIALOGS}{Enable use of windows common dialogs from header commdlg.h; example PRINTDLG.}
 \twocolitem{wxUSE_CRASHREPORT}{Use wxCrashReport class.}
 \twocolitem{wxUSE_DATEPICKCTRL_GENERIC}{Use generic \helpref{wxDatePickerCtrl}{wxdatepickerctrl} implementation in addition to the native one.}
 \twocolitem{wxUSE_DC_CACHEING}{cache temporary \helpref{wxDC}{wxdc} objects.}
 \twocolitem{wxUSE_DIRECTDRAW}{Enable use of the system include file ddraw.h.}
 \twocolitem{wxUSE_DDE_FOR_IPC}{See wx/ipc.h file.}
 \twocolitem{wxUSE_ENH_METAFILE}{Use wxEnhMetaFile.}
 \twocolitem{wxUSE_HOTKEY}{Use \helpref{wxWindow::RegisterHotKey()}{wxwindowregisterhotkey} and \helpref{UnregisterHotKey}{wxwindowunregisterhotkey}}
 \twocolitem{wxUSE_INKEDIT}{Use InkEdit library. Related to Tablet PCs.}
 \twocolitem{wxUSE_MS_HTML_HELP}{Use \helpref{wxCHMHelpController}{wxhelpcontroller} class.}
 \twocolitem{wxUSE_NO_MANIFEST}{Use to prevent the auto generation, under MSVC, of manifest file needed by windows XP and above.}
 \twocolitem{wxUSE_NORLANDER_HEADERS}{Using headers whose author is Anders Norlander.}
 \twocolitem{wxUSE_OLE}{Enables OLE helper routines.}
 \twocolitem{wxUSE_OLE_AUTOMATION}{Enable OLE automation utilities.}
 \twocolitem{wxUSE_OLE_CLIPBOARD}{Use OLE clipboard.}
 \twocolitem{wxUSE_PENWINDOWS}{See src/msw/penwin.cpp file.}
 \twocolitem{wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW}{Use PS printing in wxMSW.}
 \twocolitem{wxUSE_PS_PRINTING}{See src/msw/dcprint.cpp file.}
 \twocolitem{wxUSE_REGKEY}{Use \helpref{wxRegKey}{wxregkey} class.}
 \twocolitem{wxUSE_RICHEDIT}{Enable use of riched32.dll in \helpref{wxTextCtrl}{wxtextctrl}}
 \twocolitem{wxUSE_RICHEDIT2}{Enable use of riched20.dll in \helpref{wxTextCtrl}{wxtextctrl}}
 \twocolitem{wxUSE_VC_CRTDBG}{See wx/msw/msvcrt.h file.}
 \twocolitem{wxUSE_UNICODE_MSLU}{Use MSLU for Unicode support under Windows 9x systems.}
 \twocolitem{wxUSE_UXTHEME}{Enable support for XP themes.}
 \twocolitem{wxUSE_WIN_METAFILES_ALWAYS}{Use \helpref{wxMetaFile}{wxmetafile} even when wxUSE_ENH_METAFILE=$1$.}
 \twocolitem{wxUSE_WXDIB}{Use wxDIB class.}
 \twocolitem{wxUSE_XPM_IN_MSW}{See also wxUSE_XPM}
 
 
 @section wxusedefuniv wxUSE preprocessor symbols used only in wxUniversal
 
 \twocolitem{wxUSE_ALL_THEMES}{Use all themes in wxUniversal; See wx/univ/theme.h file.}
 \twocolitem{wxUSE_THEME_GTK}{Use GTK+ 1-like theme in wxUniversal}
 \twocolitem{wxUSE_THEME_METAL}{Use GTK+ 2-like theme in wxUniversal}
 \twocolitem{wxUSE_THEME_MONO}{Use simple monochrome theme in wxUniversal}
 \twocolitem{wxUSE_THEME_WIN32}{Use Win32-like theme in wxUniversal}
 

*/