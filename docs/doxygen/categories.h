/////////////////////////////////////////////////////////////////////////////
// Name:        categories.h
// Purpose:     Classes-by-category page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page categories_page Classes by category

 A classification of wxWidgets classes by category.

 @li @ref cat_manwnd
 @li @ref cat_miscwnd
 @li @ref cat_cmndlg
 @li @ref cat_ctrl
 @li @ref cat_miscpickers
 @li @ref cat_menus
 @li @ref cat_wxaui
 @li @ref cat_winlayout
 @li @ref cat_dc
 @li @ref cat_gdi
 @li @ref cat_events
 @li @ref cat_val
 @li @ref cat_data
 @li @ref cat_containers
 @li @ref cat_smartpointers
 @li @ref cat_rtti
 @li @ref cat_logging
 @li @ref cat_debugging
 @li @ref cat_net
 @li @ref cat_ipc
 @li @ref cat_dvf
 @li @ref cat_printing
 @li @ref cat_dd
 @li @ref cat_file
 @li @ref cat_streams
 @li @ref cat_threading
 @li @ref cat_html
 @li @ref cat_rich
 @li @ref cat_stc
 @li @ref cat_vfs
 @li @ref cat_xml
 @li @ref cat_xrc
 @li @ref cat_help
 @li @ref cat_media
 @li @ref cat_gl
 @li @ref cat_appmanagement
 @li @ref cat_misc


 <hr>



 @section cat_manwnd Managed windows

 There are several types of window that are directly controlled by the
 window manager (such as MS Windows, or the Motif Window Manager).
 Frames and dialogs are similar in wxWidgets, but only dialogs may be modal.

 @li wxTopLevelWindow: Any top level window, dialog or frame
 @li wxDialog: Dialog box
 @li wxFrame: Normal frame
 @li wxMDIChildFrame: MDI child frame
 @li wxMDIParentFrame: MDI parent frame
 @li wxMiniFrame: A frame with a small title bar
 @li wxPropertySheetDialog: Property sheet dialog
 @li wxSplashScreen: Splash screen class
 @li wxTipWindow: Shows text in a small window
 @li wxWizard: A wizard dialog

 See also the @ref commondialogs_overview.


 @section cat_miscwnd Miscellaneous windows

 The following are a variety of classes that are derived from wxWindow.

 @li wxPanel: A window whose colour changes according to current user settings
 @li wxScrolledWindow: Window with automatically managed scrollbars
 @li wxGrid: A grid (table) window
 @li wxSplitterWindow: Window which can be split vertically or horizontally
 @li wxStatusBar: Implements the status bar on a frame
 @li wxToolBar: Toolbar class
 @li wxNotebook: Notebook class
 @li wxListbook: Similar to notebook but using list control
 @li wxChoicebook: Similar to notebook but using choice control
 @li wxTreebook: Similar to notebook but using tree control
 @li wxSashWindow: Window with four optional sashes that can be dragged
 @li wxSashLayoutWindow: Window that can be involved in an IDE-like layout arrangement
 @li wxVScrolledWindow: As wxScrolledWindow but supports lines of variable height
 @li wxWizardPage: A base class for the page in wizard dialog.
 @li wxWizardPageSimple: A page in wizard dialog.


 @section cat_cmndlg Common dialogs

 @ref commondialogs_overview

 Common dialogs are ready-made dialog classes which are frequently used
 in an application.

 @li wxDialog: Base class for common dialogs
 @li wxColourDialog: Colour chooser dialog
 @li wxDirDialog: Directory selector dialog
 @li wxFileDialog: File selector dialog
 @li wxFindReplaceDialog: Text search/replace dialog
 @li wxMultiChoiceDialog: Dialog to get one or more selections from a list
 @li wxSingleChoiceDialog: Dialog to get a single selection from a list and return the string
 @li wxTextEntryDialog: Dialog to get a single line of text from the user
 @li wxPasswordEntryDialog: Dialog to get a password from the user
 @li wxFontDialog: Font chooser dialog
 @li wxPageSetupDialog: Standard page setup dialog
 @li wxPrintDialog: Standard print dialog
 @li wxProgressDialog: Progress indication dialog
 @li wxMessageDialog: Simple message box dialog
 @li wxSymbolPickerDialog: Symbol selector dialog
 @li wxRichTextFormattingDialog: A dialog for formatting the content of a wxRichTextCtrl
 @li wxWizard: A wizard dialog.


 @section cat_ctrl Controls

 Typically, these are small windows which provide interaction with the user. Controls
 that are not static can have wxValidator associated with them.

 @li wxAnimationCtrl: A control to display an animation
 @li wxControl: The base class for controls
 @li wxButton: Push button control, displaying text
 @li wxBitmapButton: Push button control, displaying a bitmap
 @li wxBitmapComboBox: A combobox with bitmaps next to text items
 @li wxToggleButton: A button which stays pressed when clicked by user.
 @li wxBitmapToggleButton: A toggle button with bitmaps.
 @li wxCalendarCtrl: Control showing an entire calendar month
 @li wxCheckBox: Checkbox control
 @li wxCheckListBox: A listbox with a checkbox to the left of each item
 @li wxChoice: Choice control (a combobox without the editable area)
 @li wxCollapsiblePane: A panel which can be shown/hidden by the user
 @li wxComboBox: A choice with an editable area
 @li wxComboCtrl: A combobox with application defined popup
 @li wxDataViewCtrl: A control to tabular or tree like data
 @li wxDataViewTreeCtrl: A specialized wxDataViewCtrl with wxTreeCtrl-like API
 @li wxGauge: A control to represent a varying quantity, such as time remaining
 @li wxGenericDirCtrl: A control for displaying a directory tree
 @li wxHtmlListBox: An abstract class for creating listboxes showing HTML content
 @li wxSimpleHtmlListBox: A listbox showing HTML content
 @li wxStaticBox: A static, or group box for visually grouping related controls
 @li wxListBox: A list of strings for single or multiple selection
 @li wxListCtrl: A control for displaying lists of strings and/or icons, plus a multicolumn report view
 @li wxListView: A simpler interface (fa&ccedil;ade) for wxListCtrl in report mode
 @li wxOwnerDrawnComboBox: A combobox with owner-drawn list items
 @li wxRichTextCtrl: Generic rich text editing control
 @li wxTextCtrl: Single or multiline text editing control
 @li wxTreeCtrl: Tree (hierarchy) control
 @li wxScrollBar: Scrollbar control
 @li wxSpinButton: A spin or `up-down' control
 @li wxSpinCtrl: A spin control - i.e. spin button and text control
 @li wxStaticText: One or more lines of non-editable text
 @li wxHyperlinkCtrl: A static text which opens an URL when clicked
 @li wxStaticBitmap: A control to display a bitmap
 @li wxRadioBox: A group of radio buttons
 @li wxRadioButton: A round button to be used with others in a mutually exclusive way
 @li wxSlider: A slider that can be dragged by the user
 @li wxVListBox: A listbox supporting variable height rows



 @section cat_miscpickers Miscellaneous pickers

 A picker control is a control whose appearance and behaviour is highly platform-dependent.

 @li wxColourPickerCtrl: A control which allows the user to choose a colour
 @li wxDirPickerCtrl: A control which allows the user to choose a directory
 @li wxFilePickerCtrl: A control which allows the user to choose a file
 @li wxFontPickerCtrl: A control which allows the user to choose a font
 @li wxDatePickerCtrl: Small date picker control



 @section cat_menus Menus

 @li wxMenu: Displays a series of menu items for selection
 @li wxMenuBar: Contains a series of menus for use with a frame
 @li wxMenuItem: Represents a single menu item



 @section cat_wxaui wxAUI - advanced user interface

 This is a new set of classes for writing a customizable application
 interface with built-in docking, floatable panes and a flexible
 MDI-like interface. Further classes for custom notebooks with
 draggable tabs etc. are in progress. See also @ref wxaui_overview.

 @li wxAuiManager: The central class for managing the interface
 @li wxAuiNotebook: A replacement notebook class with extra features
 @li wxAuiPaneInfo: Describes a single pane
 @li wxAuiDockArt: Art and metrics provider for customizing the docking user interface
 @li wxAuiTabArt: Art and metrics provider for customizing the notebook user interface



 @section cat_winlayout Window layout

 There are two different systems for laying out windows (and dialogs in particular).
 One is based upon so-called sizers and it requires less typing, thinking and calculating
 and will in almost all cases produce dialogs looking equally well on all platforms, the
 other is based on so-called constraints and is deprecated, though still available.

 @ref sizer_overview describes sizer-based layout.

 These are the classes relevant to sizer-based layout.

 @li wxSizer: Abstract base class
 @li wxGridSizer: A sizer for laying out windows in a grid with all fields having the same size
 @li wxFlexGridSizer: A sizer for laying out windows in a flexible grid
 @li wxGridBagSizer: Another grid sizer that lets you specify the cell an item is in, and items can span rows and/or columns.
 @li wxBoxSizer: A sizer for laying out windows in a row or column
 @li wxStaticBoxSizer: Same as wxBoxSizer, but with a surrounding static box
 @li wxWrapSizer: A sizer which wraps its child controls as size permits

 @ref constraints_overview describes constraints-based layout.

 These are the classes relevant to constraints-based window layout.

 @li wxIndividualLayoutConstraint: Represents a single constraint dimension
 @li wxLayoutConstraints: Represents the constraints for a window class

 Other layouting classes:

 @li wxLayoutAlgorithm: An alternative window layout facility


 @section cat_dc Device contexts

 @ref dc_overview

 Device contexts are surfaces that may be drawn on, and provide an
 abstraction that allows parameterisation of your drawing code
 by passing different device contexts.

 @li wxAutoBufferedPaintDC: A helper device context for double buffered drawing inside @b OnPaint.
 @li wxBufferedDC: A helper device context for double buffered drawing.
 @li wxBufferedPaintDC: A helper device context for double buffered drawing inside @b OnPaint.
 @li wxClientDC: A device context to access the client area outside @b OnPaint events
 @li wxPaintDC: A device context to access the client area inside @b OnPaint events
 @li wxWindowDC: A device context to access the non-client area
 @li wxScreenDC: A device context to access the entire screen
 @li wxDC: The device context base class
 @li wxMemoryDC: A device context for drawing into bitmaps
 @li wxMetafileDC: A device context for drawing into metafiles
 @li wxMirrorDC: A proxy device context allowing for simple mirroring.
 @li wxPostScriptDC: A device context for drawing into PostScript files
 @li wxPrinterDC: A device context for drawing to printers


 @section cat_gdi Graphics device interface

 @ref bitmap_overview

 These classes are related to drawing on device contexts and windows.

 @li wxColour: Represents the red, blue and green elements of a colour
 @li wxDCClipper: Wraps the operations of setting and destroying the clipping region
 @li wxBitmap: Represents a bitmap
 @li wxBrush: Used for filling areas on a device context
 @li wxBrushList: The list of previously-created brushes
 @li wxCursor: A small, transparent bitmap representing the cursor
 @li wxFont: Represents fonts
 @li wxFontList: The list of previously-created fonts
 @li wxIcon: A small, transparent bitmap for assigning to frames and drawing on device contexts
 @li wxImage: A platform-independent image class
 @li wxImageList: A list of images, used with some controls
 @li wxMask: Represents a mask to be used with a bitmap for transparent drawing
 @li wxPen: Used for drawing lines on a device context
 @li wxPenList: The list of previously-created pens
 @li wxPalette: Represents a table of indices into RGB values
 @li wxRegion: Represents a simple or complex region on a window or device context
 @li wxRendererNative: Abstracts high-level drawing primitives


 @section cat_events Events

 @ref eventhandling_overview

 An event object contains information about a specific event. Event handlers
 (usually member functions) have a single, event argument.

 @li wxActivateEvent: A window or application activation event
 @li wxCalendarEvent: Used with wxCalendarCtrl
 @li wxCalculateLayoutEvent: Used to calculate window layout
 @li wxChildFocusEvent: A child window focus event
 @li wxClipboardTextEvent: A clipboard copy/cut/paste treebook event event
 @li wxCloseEvent: A close window or end session event
 @li wxCommandEvent: An event from a variety of standard controls
 @li wxContextMenuEvent: An event generated when the user issues a context menu command
 @li wxDateEvent: Used with wxDatePickerCtrl
 @li wxDialUpEvent: Event send by wxDialUpManager
 @li wxDropFilesEvent: A drop files event
 @li wxEraseEvent: An erase background event
 @li wxEvent: The event base class
 @li wxFindDialogEvent: Event sent by wxFindReplaceDialog
 @li wxFocusEvent: A window focus event
 @li wxKeyEvent: A keypress event
 @li wxIconizeEvent: An iconize/restore event
 @li wxIdleEvent: An idle event
 @li wxInitDialogEvent: A dialog initialisation event
 @li wxJoystickEvent: A joystick event
 @li wxListEvent: A list control event
 @li wxMaximizeEvent: A maximize event
 @li wxMenuEvent: A menu event
 @li wxMouseCaptureChangedEvent: A mouse capture changed event
 @li wxMouseCaptureLostEvent: A mouse capture lost event
 @li wxMouseEvent: A mouse event
 @li wxMoveEvent: A move event
 @li wxNavigationKeyEvent: An event set by navigation keys such as tab
 @li wxNotebookEvent: A notebook control event
 @li wxNotifyEvent: A notification event, which can be vetoed
 @li wxPaintEvent: A paint event
 @li wxProcessEvent: A process ending event
 @li wxQueryLayoutInfoEvent: Used to query layout information
 @li wxRichTextEvent: A rich text editing event
 @li wxScrollEvent: A scroll event from sliders, stand-alone scrollbars and spin buttons
 @li wxScrollWinEvent: A scroll event from scrolled windows
 @li wxSizeEvent: A size event
 @li wxSocketEvent: A socket event
 @li wxSpinEvent: An event from wxSpinButton
 @li wxSplitterEvent: An event from wxSplitterWindow
 @li wxSysColourChangedEvent: A system colour change event
 @li wxTimerEvent: A timer expiration event
 @li wxTreebookEvent: A treebook control event
 @li wxTreeEvent: A tree control event
 @li wxUpdateUIEvent: A user interface update event
 @li wxWindowCreateEvent: A window creation event
 @li wxWindowDestroyEvent: A window destruction event
 @li wxWizardEvent: A wizard event


 @section cat_val Validators

 @ref validator_overview

 These are the window validators, used for filtering and validating
 user input.

 @li wxValidator: Base validator class
 @li wxTextValidator: Text control validator class
 @li wxGenericValidator: Generic control validator class


 @section cat_data Data structures

 These are the data structure classes supported by wxWidgets.

 @li wxCmdLineParser: Command line parser class
 @li wxDateSpan: A logical time interval.
 @li wxDateTime: A class for date/time manipulations
 @li wxLongLong: A portable 64 bit integer type
 @li wxObject: The root class for most wxWidgets classes
 @li wxPathList: A class to help search multiple paths
 @li wxPoint: Representation of a point
 @li wxRect: A class representing a rectangle
 @li wxRegEx: Regular expression support
 @li wxRegion: A class representing a region
 @li wxString: A string class
 @li wxStringTokenizer: A class for interpreting a string as a list of tokens or words
 @li wxRealPoint: Representation of a point using floating point numbers
 @li wxSize: Representation of a size
 @li wxTimeSpan: A time interval.
 @li wxURI: Represents a Uniform Resource Identifier
 @li wxVariant: A class for storing arbitrary types that may change at run-time


 @section cat_containers Container classes

 @ref container_overview

 These are classes, templates and class macros are used by wxWidgets. Most
 of these classes provide a subset or almost complete STL API.

 @li wxArray<T>: A type-safe dynamic array implementation (macro based)
 @li wxArrayString: An efficient container for storing wxString objects
 @li wxHashMap<T>: A type-safe hash map implementation (macro based)
 @li wxHashSet<T>: A type-safe hash set implementation(macro based)
 @li wxHashTable: A simple hash table implementation (deprecated, use wxHashMap)
 @li wxList<T>: A type-safe linked list implementation (macro based)
 @li wxVector<T>: Template base vector implementation identical to std::vector


 @section cat_smartpointers Smart pointers

 wxWidgets provides a few smart pointer class templates.

 @li wxObjectDataPtr<T>: A shared pointer (using intrusive reference counting)
 @li wxScopedPtr<T>: A scoped pointer
 @li wxSharedPtr<T>: A shared pointer (using non-intrusive reference counting)
 @li wxWeakRef<T>: A weak reference


 @section cat_rtti Run-time class information system

 @ref runtimeclass_overview

 wxWidgets supports run-time manipulation of class information, and dynamic
 creation of objects given class names.

 @li wxClassInfo: Holds run-time class information
 @li wxObject: Root class for classes with run-time information
 @li RTTI macros: Macros for manipulating run-time information


 @section cat_logging Logging features

 @ref log_overview

 wxWidgets provides several classes and functions for message logging.
 Please see the @ref log_overview for more details.

 @li wxLog: The base log class
 @li wxLogStderr: Log messages to a C STDIO stream
 @li wxLogStream: Log messages to a C++ iostream
 @li wxLogTextCtrl: Log messages to a wxTextCtrl
 @li wxLogWindow: Log messages to a log frame
 @li wxLogGui: Default log target for GUI programs
 @li wxLogNull: Temporarily suppress message logging
 @li wxLogChain: Allows to chain two log targets
 @li wxLogInterposer: Allows to filter the log messages
 @li wxLogInterposerTemp: Allows to filter the log messages
 @li wxStreamToTextRedirector: Allows to redirect output sent to @c cout to a wxTextCtrl
 @li Log functions: Error and warning logging functions


 @section cat_debugging Debugging features

 @ref debugging_overview

 wxWidgets supports some aspects of debugging an application through
 classes, functions and macros.

 @li wxDebugContext: Provides memory-checking facilities
 @li Debugging macros: Debug macros for assertion and checking
 @li WXDEBUG\_NEW: Use this macro to give further debugging information
 @li wxDebugReport: Base class for creating debug reports in case of a program crash.
 @li wxDebugReportCompress: Class for creating compressed debug reports.
 @li wxDebugReportUpload: Class for uploading compressed debug reports via HTTP.
 @li wxDebugReportPreview: Abstract base class for previewing the contents of a debug report.
 @li wxDebugReportPreviewStd: Standard implementation of wxDebugReportPreview.


 @section cat_net Networking classes

 wxWidgets provides its own classes for socket based networking.

 @li wxDialUpManager: Provides functions to check the status of network connection and to establish one
 @li wxIPV4address: Represents an Internet address
 @li wxIPaddress: Represents an Internet address
 @li wxSocketBase: Represents a socket base object
 @li wxSocketClient: Represents a socket client
 @li wxSocketServer: Represents a socket server
 @li wxSocketEvent: A socket event
 @li wxFTP: FTP protocol class
 @li wxHTTP: HTTP protocol class
 @li wxURL: Represents a Universal Resource Locator



 @section cat_ipc Interprocess communication

 @ref ipc_overview

 wxWidgets provides simple interprocess communications facilities
 based on Windows DDE, but available on most platforms using TCP.

 @li wxClient, wxDDEClient: Represents a client
 @li wxConnection, wxDDEConnection: Represents the connection between a client and a server
 @li wxServer, wxDDEServer: Represents a server



 @section cat_dvf Document-view framework

 @ref docview_overview

 wxWidgets supports a document/view framework which provides
 housekeeping for a document-centric application.

 @li wxDocument: Represents a document
 @li wxView: Represents a view
 @li wxDocTemplate: Manages the relationship between a document class and a view class
 @li wxDocManager: Manages the documents and views in an application
 @li wxDocChildFrame: A child frame for showing a document view
 @li wxDocParentFrame: A parent frame to contain views
<!--  %@li wxMDIDocChildFrame: An MDI child frame for showing a document view
 %@li wxMDIDocParentFrame: An MDI parent frame to contain views  -->


 @section cat_printing Printing framework

 @ref printing_overview

 A printing and previewing framework is implemented to
 make it relatively straightforward to provide document printing
 facilities.

 @li wxPreviewFrame: Frame for displaying a print preview
 @li wxPreviewCanvas: Canvas for displaying a print preview
 @li wxPreviewControlBar: Standard control bar for a print preview
 @li wxPrintDialog: Standard print dialog
 @li wxPageSetupDialog: Standard page setup dialog
 @li wxPrinter: Class representing the printer
 @li wxPrinterDC: Printer device context
 @li wxPrintout: Class representing a particular printout
 @li wxPrintPreview: Class representing a print preview
 @li wxPrintData: Represents information about the document being printed
 @li wxPrintDialogData: Represents information about the print dialog
 @li wxPageSetupDialogData: Represents information about the page setup dialog



 @section cat_dd Drag and drop and clipboard classes

 @ref dnd_overview

 @li wxDataObject: Data object class
 @li wxDataFormat: Represents a data format
 @li wxTextDataObject: Text data object class
 @li wxFileDataObject: File data object class
 @li wxBitmapDataObject: Bitmap data object class
 @li wxURLDataObject: URL data object class
 @li wxCustomDataObject: Custom data object class
 @li wxClipboard: Clipboard class
 @li wxDropTarget: Drop target class
 @li wxFileDropTarget: File drop target class
 @li wxTextDropTarget: Text drop target class
 @li wxDropSource: Drop source class


 @section cat_file File related classes

 wxWidgets has several small classes to work with disk files, see wxfileoverview for more details.

 @li wxFileName: Operations with the file name and attributes
 @li wxDir: Class for enumerating files/subdirectories.
 @li wxDirTraverser: Class used together with wxDir for recursively enumerating the files/subdirectories
 @li wxFile: Low-level file input/output class.
 @li wxFFile: Another low-level file input/output class.
 @li wxTempFile: Class to safely replace an existing file
 @li wxTextFile: Class for working with text files as with arrays of lines
 @li wxStandardPaths: Paths for standard directories
 @li wxPathList: A class to help search multiple paths


 @section cat_streams Stream classes

 wxWidgets has its own set of stream classes, as an alternative to often buggy standard stream
 libraries, and to provide enhanced functionality.

 @li wxStreamBase: Stream base class
 @li wxStreamBuffer: Stream buffer class
 @li wxInputStream: Input stream class
 @li wxOutputStream: Output stream class
 @li wxCountingOutputStream: Stream class for querying what size a stream would have.
 @li wxFilterInputStream: Filtered input stream class
 @li wxFilterOutputStream: Filtered output stream class
 @li wxBufferedInputStream: Buffered input stream class
 @li wxBufferedOutputStream: Buffered output stream class
 @li wxMemoryInputStream: Memory input stream class
 @li wxMemoryOutputStream: Memory output stream class
 @li wxDataInputStream: Platform-independent binary data input stream class
 @li wxDataOutputStream: Platform-independent binary data output stream class
 @li wxTextInputStream: Platform-independent text data input stream class
 @li wxTextOutputStream: Platform-independent text data output stream class
 @li wxFileInputStream: File input stream class
 @li wxFileOutputStream: File output stream class
 @li wxFFileInputStream: Another file input stream class
 @li wxFFileOutputStream: Another file output stream class
 @li wxTempFileOutputStream: Stream to safely replace an existing file
 @li wxStringInputStream: String input stream class
 @li wxStringOutputStream: String output stream class
 @li wxZlibInputStream: Zlib and gzip (compression) input stream class
 @li wxZlibOutputStream: Zlib and gzip (compression) output stream class
 @li wxZipInputStream: Input stream for reading from ZIP archives
 @li wxZipOutputStream: Output stream for writing from ZIP archives
 @li wxTarInputStream: Input stream for reading from tar archives
 @li wxTarOutputStream: Output stream for writing from tar archives
 @li wxSocketInputStream: Socket input stream class
 @li wxSocketOutputStream: Socket output stream class


 @section cat_threading Threading classes

 @ref thread_overview

 wxWidgets provides a set of classes to make use of the native thread
 capabilities of the various platforms.

 @li wxThread: Thread class
 @li wxThreadHelper: Manages background threads easily
 @li wxMutex: Mutex class
 @li wxMutexLocker: Mutex locker utility class
 @li wxCriticalSection: Critical section class
 @li wxCriticalSectionLocker: Critical section locker utility class
 @li wxCondition: Condition class
 @li wxSemaphore: Semaphore class


 @section cat_html HTML classes

 wxWidgets provides a set of classes to display text in HTML format. These
 class include a help system based on the HTML widget.

 @li wxHtmlHelpController: HTML help controller class
 @li wxHtmlWindow: HTML window class
 @li wxHtmlEasyPrinting: Simple class for printing HTML
 @li wxHtmlPrintout: Generic HTML wxPrintout class
 @li wxHtmlParser: Generic HTML parser class
 @li wxHtmlTagHandler: HTML tag handler, pluginable into wxHtmlParser
 @li wxHtmlWinParser: HTML parser class for wxHtmlWindow
 @li wxHtmlWinTagHandler: HTML tag handler, pluginable into wxHtmlWinParser


 @section cat_rich Rich text classes

 wxWidgets provides a set of generic classes to edit and print simple rich text with character
 and paragraph formatting.

 @li wxTextAttr: Attributes specifying text styling.
 @li wxRichTextCtrl: A rich text control.
 @li wxRichTextBuffer: The content of a rich text control.
 @li wxRichTextCharacterStyleDefinition: Definition of character styling.
 @li wxRichTextParagraphStyleDefinition: Definition of paragraph styling.
 @li wxRichTextListStyleDefinition: Definition of list styling.
 @li wxRichTextStyleSheet: A set of style definitions.
 @li wxRichTextStyleComboCtrl: A drop-down control for applying styles.
 @li wxRichTextStyleListBox: A listbox for applying styles.
 @li wxRichTextStyleOrganiserDialog: A dialog that can be used for managing or browsing styles.
 @li wxRichTextEvent: A rich text event.
 @li wxRichTextRange: Specification for ranges in a rich text control or buffer.
 @li wxRichTextFileHandler: File handler base class.
 @li wxRichTextHTMLHandler: A handler for converting rich text to HTML.
 @li wxRichTextXMLHandler: A handler for loading and saving rich text XML.
 @li wxRichTextFormattingDialog: A dialog for rich text formatting.
 @li wxRichTextPrinting: A class for easy printing of rich text buffers.
 @li wxRichTextPrintout: A class used by wxRichTextPrinting.
 @li wxRichTextHeaderFooterData: Header and footer data specification.


 @section cat_stc Scintilla text editor classes

 wxWidgets also provides a wrapper around the Scintilla text editor control, which is
 a control for plain-text editing with support for highlighting, smart indentation, etc.

 @li wxStyledTextCtrl: A wxWidgets implementation of the Scintilla source code editing component.


 @section cat_vfs Virtual file system classes

 wxWidgets provides a set of classes that implement an extensible virtual file system,
 used internally by the HTML classes.

 @li wxFSFile: Represents a file in the virtual file system
 @li wxFileSystem: Main interface for the virtual file system
 @li wxFileSystemHandler: Class used to announce file system type


 @section cat_xml XML classes

 @li wxXmlDocument: A class to parse XML files
 @li wxXmlNode: A class which represents XML nodes
 @li wxXmlAttribute: A class which represent an XML attribute


 @section cat_xrc XML-based resource system classes

 @ref xrc_overview

 Resources allow your application to create controls and other user interface elements
 from specifications stored in an XML format.

 @li wxXmlResource: The main class for working with resources
 @li wxXmlResourceHandler: The base class for XML resource handlers


 @section cat_help Online help

 @li wxHelpController: Family of classes for controlling help windows
 @li wxHtmlHelpController: HTML help controller class
 @li wxContextHelp: Class to put application into context-sensitive help mode
 @li wxContextHelpButton: Button class for putting application into context-sensitive help mode
 @li wxHelpProvider: Abstract class for context-sensitive help provision
 @li wxSimpleHelpProvider: Class for simple context-sensitive help provision
 @li wxHelpControllerHelpProvider: Class for context-sensitive help provision via a help controller
 @li wxToolTip: Class implementing tooltips


 @section cat_media Multimedia classes

 @li wxMediaCtrl: Display multimedia contents.


 @section cat_gl OpenGL classes

 @li wxGLCanvas: Canvas that you can render OpenGL calls to.
 @li wxGLContext: Class to ease sharing of OpenGL data resources.


 @section cat_appmanagement Application and process-management classes

 @li wxApp: Application class
 @li wxCmdLineParser: Command line parser class
 @li wxDllLoader: Class to work with shared libraries.
 @li wxProcess: Process class


 @section cat_misc Miscellaneous

 @li wxCaret: A caret (cursor) object
 @li wxConfig: Classes for configuration reading/writing (using either INI files or registry)
 @li wxTimer: Timer class
 @li wxStopWatch: Stop watch class
 @li wxMimeTypesManager: MIME-types manager class
 @li wxSystemSettings: System settings class for obtaining various global parameters
 @li wxSystemOptions: System options class for run-time configuration
 @li wxAcceleratorTable: Accelerator table
 @li wxAutomationObject: OLE automation class
 @li wxFontMapper: Font mapping, finding suitable font for given encoding
 @li wxEncodingConverter: Encoding conversions
 @li wxCalendarDateAttr: Used with wxCalendarCtrl
 @li wxQuantize: Class to perform quantization, or colour reduction
 @li wxSingleInstanceChecker: Check that only single program instance is running

*/