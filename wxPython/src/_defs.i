/////////////////////////////////////////////////////////////////////////////
// Name:        _defs.i
// Purpose:     Definitions and stuff
//
// Author:      Robin Dunn
//
// Created:     6/24/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////



//---------------------------------------------------------------------------
// Forward declares...

class wxAcceleratorEntry;
class wxAcceleratorTable;
class wxActivateEvent;
class wxBitmapButton;
class wxBitmap;
class wxBrush;
class wxButton;
class wxCalculateLayoutEvent;
class wxCaret;
class wxCheckBox;
class wxCheckListBox;
class wxChoice;
class wxClientDC;
class wxCloseEvent;
class wxColourData;
class wxColourDialog;
class wxColour;
class wxComboBox;
class wxCommandEvent;
class wxConfig;
class wxControl;
class wxCursor;
class wxDC;
class wxDialog;
class wxDirDialog;
class wxDropFilesEvent;
class wxEraseEvent;
class wxEvent;
class wxEvtHandler;
class wxFileDialog;
class wxFocusEvent;
class wxFontData;
class wxFontDialog;
class wxFont;
class wxFrame;
class wxGauge;
class wxGridCell;
class wxGridEvent;
class wxGrid;
class wxIconizeEvent;
class wxIcon;
class wxIdleEvent;
class wxImageList;
class wxIndividualLayoutConstraint;
class wxInitDialogEvent;
class wxJoystickEvent;
class wxKeyEvent;
class wxLayoutAlgorithm;
class wxLayoutConstraints;
class wxListBox;
class wxListCtrl;
class wxListEvent;
class wxListItem;
class wxMDIChildFrame;
class wxMDIClientWindow;
class wxMDIParentFrame;
class wxMask;
class wxMaximizeEvent;
class wxMemoryDC;
class wxMenuBar;
class wxMenuEvent;
class wxMenuItem;
class wxMenu;
class wxMessageDialog;
class wxMetaFileDC;
class wxMiniFrame;
class wxMouseEvent;
class wxMoveEvent;
class wxNotebookEvent;
class wxNotebook;
class wxPageSetupData;
class wxPageSetupDialog;
class wxPaintDC;
class wxPaintEvent;
class wxPalette;
class wxPanel;
class wxPen;
class wxPoint;
class wxPostScriptDC;
class wxPrintData;
class wxPrintDialog;
class wxPrinterDC;
class wxQueryLayoutInfoEvent;
class wxRadioBox;
class wxRadioButton;
class wxRealPoint;
class wxRect;
class wxRegionIterator;
class wxRegion;
class wxSashEvent;
class wxSashLayoutWindow;
class wxSashWindow;
class wxScreenDC;
class wxScrollBar;
class wxScrollEvent;
class wxScrollWinEvent;
class wxScrolledWindow;
class wxShowEvent;
class wxSingleChoiceDialog;
class wxSizeEvent;
class wxSize;
class wxSlider;
class wxSpinButton;
class wxSpinEvent;
class wxSplitterWindow;
class wxStaticBitmap;
class wxStaticBox;
class wxStaticText;
class wxStatusBar;
class wxSysColourChangedEvent;
class wxTaskBarIcon;
class wxTextCtrl;
class wxTextEntryDialog;
class wxTimer;
class wxToolBarTool;
class wxToolBar;
class wxToolTip;
class wxTreeCtrl;
class wxTreeEvent;
class wxTreeItemData;
class wxTreeItemId;
class wxUpdateUIEvent;
class wxWindowDC;
class wxWindow;
class wxSizer;
class wxBoxSizer;
class wxStaticBoxSizer;

class wxPyApp;
class wxPyMenu;
class wxPyTimer;


//---------------------------------------------------------------------------

// some definitions for SWIG only
typedef unsigned char   byte;
typedef short int       WXTYPE;
typedef int             wxWindowID;
typedef unsigned int    uint;
typedef signed   int    EBool;
typedef unsigned int    size_t
typedef unsigned int    time_t
typedef int             wxPrintQuality;
typedef int             wxCoord;
typedef char            wxChar;


//---------------------------------------------------------------------------

// General numeric #define's and etc.  Making them all enums makes SWIG use the
// real macro when making the Python Int

enum {
    wxMAJOR_VERSION,
    wxMINOR_VERSION,
    wxRELEASE_NUMBER,

    wxNOT_FOUND,

    wxVSCROLL,
    wxHSCROLL,
    wxCAPTION,
    wxDOUBLE_BORDER,
    wxSUNKEN_BORDER,
    wxRAISED_BORDER,
    wxBORDER,
    wxSIMPLE_BORDER,
    wxSTATIC_BORDER,
    wxTRANSPARENT_WINDOW,
    wxNO_BORDER,
    wxUSER_COLOURS,
    wxNO_3D,

    wxTAB_TRAVERSAL,
    wxWANTS_CHARS,
    wxPOPUP_WINDOW,
    wxHORIZONTAL,
    wxVERTICAL,
    wxBOTH,
    wxCENTER_FRAME,
    wxCENTRE_ON_SCREEN,
    wxCENTER_ON_SCREEN,

    wxSTAY_ON_TOP,
    wxICONIZE,
    wxMINIMIZE,
    wxMAXIMIZE,
    wxCLOSE_BOX,
    wxTHICK_FRAME,
    wxSYSTEM_MENU,
    wxMINIMIZE_BOX,
    wxMAXIMIZE_BOX,
    wxTINY_CAPTION_HORIZ,
    wxTINY_CAPTION_VERT,
    wxRESIZE_BOX,
    wxRESIZE_BORDER,
    wxDIALOG_MODAL,
    wxDIALOG_MODELESS,
    wxDIALOG_NO_PARENT,
    wxDEFAULT_FRAME_STYLE,
    wxDEFAULT_DIALOG_STYLE,

    wxFRAME_TOOL_WINDOW,
    wxFRAME_FLOAT_ON_PARENT,
    wxFRAME_NO_WINDOW_MENU,
    wxFRAME_NO_TASKBAR,
    wxFRAME_SHAPED,

    wxED_CLIENT_MARGIN,
    wxED_BUTTONS_BOTTOM,
    wxED_BUTTONS_RIGHT,
    wxED_STATIC_LINE,
    wxEXT_DIALOG_STYLE,

    wxCLIP_CHILDREN,
    wxCLIP_SIBLINGS,

    wxRETAINED,
    wxBACKINGSTORE,

    wxCOLOURED,
    wxFIXED_LENGTH,
    wxALIGN_LEFT,
    wxALIGN_CENTER_HORIZONTAL,
    wxALIGN_CENTRE_HORIZONTAL,
    wxALIGN_RIGHT,
    wxALIGN_BOTTOM,
    wxALIGN_CENTER_VERTICAL,
    wxALIGN_CENTRE_VERTICAL,
    wxALIGN_TOP,
    wxALIGN_CENTER,
    wxALIGN_CENTRE,
    wxSHAPED,
    wxADJUST_MINSIZE,

    wxLB_NEEDED_SB,
    wxLB_ALWAYS_SB,
    wxLB_SORT,
    wxLB_SINGLE,
    wxLB_MULTIPLE,
    wxLB_EXTENDED,
    wxLB_OWNERDRAW,
    wxLB_HSCROLL,
    wxPROCESS_ENTER,
    wxPASSWORD,

    wxCB_SIMPLE,
    wxCB_DROPDOWN,
    wxCB_SORT,
    wxCB_READONLY,
    wxRA_HORIZONTAL,
    wxRA_VERTICAL,
    wxRA_SPECIFY_ROWS,
    wxRA_SPECIFY_COLS,
    wxRB_GROUP,
    wxRB_SINGLE,
    wxGA_PROGRESSBAR,
    wxGA_HORIZONTAL,
    wxGA_VERTICAL,
    wxGA_SMOOTH,
    wxSL_HORIZONTAL,
    wxSL_VERTICAL,
    wxSL_AUTOTICKS,
    wxSL_LABELS,
    wxSL_LEFT,
    wxSL_TOP,
    wxSL_RIGHT,
    wxSL_BOTTOM,
    wxSL_BOTH,
    wxSL_SELRANGE,
    wxSB_HORIZONTAL,
    wxSB_VERTICAL,
    wxST_SIZEGRIP,
    wxST_NO_AUTORESIZE,

    wxBU_NOAUTODRAW,
    wxBU_AUTODRAW,
    wxBU_LEFT,
    wxBU_TOP,
    wxBU_RIGHT,
    wxBU_BOTTOM,
    wxBU_EXACTFIT,

    wxFLOOD_SURFACE,
    wxFLOOD_BORDER,
    wxODDEVEN_RULE,
    wxWINDING_RULE,
    wxTOOL_TOP,
    wxTOOL_BOTTOM,
    wxTOOL_LEFT,
    wxTOOL_RIGHT,
    wxOK,
    wxYES_NO,
    wxCANCEL,
    wxYES,
    wxNO,
    wxNO_DEFAULT,
    wxYES_DEFAULT,
    wxICON_EXCLAMATION,
    wxICON_HAND,
    wxICON_QUESTION,
    wxICON_INFORMATION,
    wxICON_STOP,
    wxICON_ASTERISK,
    wxICON_MASK,
    wxICON_WARNING,
    wxICON_ERROR,

    wxFORWARD,
    wxBACKWARD,
    wxRESET,
    wxHELP,
    wxMORE,
    wxSETUP,


    wxCENTRE,
    wxCENTER,
    wxSIZE_AUTO_WIDTH,
    wxSIZE_AUTO_HEIGHT,
    wxSIZE_AUTO,
    wxSIZE_USE_EXISTING,
    wxSIZE_ALLOW_MINUS_ONE,
    wxPORTRAIT,
    wxLANDSCAPE,
    wxPRINT_QUALITY_HIGH,
    wxPRINT_QUALITY_MEDIUM,
    wxPRINT_QUALITY_LOW,
    wxPRINT_QUALITY_DRAFT,

    wxID_ANY,
    wxID_SEPARATOR,

    wxID_OPEN,
    wxID_CLOSE,
    wxID_NEW,
    wxID_SAVE,
    wxID_SAVEAS,
    wxID_REVERT,
    wxID_EXIT,
    wxID_UNDO,
    wxID_REDO,
    wxID_HELP,
    wxID_PRINT,
    wxID_PRINT_SETUP,
    wxID_PREVIEW,
    wxID_ABOUT,
    wxID_HELP_CONTENTS,
    wxID_HELP_COMMANDS,
    wxID_HELP_PROCEDURES,
    wxID_HELP_CONTEXT,
    wxID_CLOSE_ALL,

    wxID_CUT,
    wxID_COPY,
    wxID_PASTE,
    wxID_CLEAR,
    wxID_FIND,
    wxID_DUPLICATE,
    wxID_SELECTALL,

    wxID_DELETE,
    wxID_REPLACE,
    wxID_REPLACE_ALL,
    wxID_PROPERTIES,

    wxID_VIEW_DETAILS,
    wxID_VIEW_LARGEICONS,
    wxID_VIEW_SMALLICONS,
    wxID_VIEW_LIST,
    wxID_VIEW_SORTDATE,
    wxID_VIEW_SORTNAME,
    wxID_VIEW_SORTSIZE,
    wxID_VIEW_SORTTYPE,

    wxID_FILE1,
    wxID_FILE2,
    wxID_FILE3,
    wxID_FILE4,
    wxID_FILE5,
    wxID_FILE6,
    wxID_FILE7,
    wxID_FILE8,
    wxID_FILE9,

    wxID_OK,
    wxID_CANCEL,
    wxID_APPLY,
    wxID_YES,
    wxID_NO,
    wxID_STATIC,
    wxID_FORWARD,
    wxID_BACKWARD,
    wxID_DEFAULT,
    wxID_MORE,
    wxID_SETUP,
    wxID_RESET,
    wxID_CONTEXT_HELP,
    wxID_YESTOALL,
    wxID_NOTOALL,
    wxID_ABORT,
    wxID_RETRY,
    wxID_IGNORE,

    wxOPEN,
    wxSAVE,
    wxHIDE_READONLY,
    wxOVERWRITE_PROMPT,
    wxFILE_MUST_EXIST,
    wxMULTIPLE,
    wxCHANGE_DIR,

    wxACCEL_ALT,
    wxACCEL_CTRL,
    wxACCEL_SHIFT,
    wxACCEL_NORMAL,

    wxPD_AUTO_HIDE,
    wxPD_APP_MODAL,
    wxPD_CAN_ABORT,
    wxPD_ELAPSED_TIME,
    wxPD_ESTIMATED_TIME,
    wxPD_REMAINING_TIME,

    wxDD_NEW_DIR_BUTTON,
    wxDD_DEFAULT_STYLE,

    wxMENU_TEAROFF,
    wxMB_DOCKABLE,
    wxNO_FULL_REPAINT_ON_RESIZE,

    wxLEFT,
    wxRIGHT,
    wxUP,
    wxDOWN,
    wxALL,
    wxTOP,
    wxBOTTOM,

    wxNORTH,
    wxSOUTH,
    wxEAST,
    wxWEST,

    wxSTRETCH_NOT,
    wxSHRINK,
    wxGROW,
    wxEXPAND,

    wxLI_HORIZONTAL,
    wxLI_VERTICAL,

    wxJOYSTICK1,
    wxJOYSTICK2,
    wxJOY_BUTTON1,
    wxJOY_BUTTON2,
    wxJOY_BUTTON3,
    wxJOY_BUTTON4,
    wxJOY_BUTTON_ANY,

    wxWS_EX_VALIDATE_RECURSIVELY,
    wxWS_EX_BLOCK_EVENTS,
    wxWS_EX_TRANSIENT,

    wxWS_EX_THEMED_BACKGROUND,
    wxWS_EX_PROCESS_IDLE,
    wxWS_EX_PROCESS_UI_UPDATES,


    // Mapping modes (as per Windows)
    wxMM_TEXT,
    wxMM_LOMETRIC,
    wxMM_HIMETRIC,
    wxMM_LOENGLISH,
    wxMM_HIENGLISH,
    wxMM_TWIPS,
    wxMM_ISOTROPIC,
    wxMM_ANISOTROPIC,
    wxMM_POINTS,
    wxMM_METRIC,

    wxTIMER_CONTINUOUS,
    wxTIMER_ONE_SHOT,

    // the symbolic names for the mouse buttons
    wxMOUSE_BTN_ANY,
    wxMOUSE_BTN_NONE,
    wxMOUSE_BTN_LEFT,
    wxMOUSE_BTN_MIDDLE,
    wxMOUSE_BTN_RIGHT,

    // It looks like wxTabCtrl may rise from the dead.  Uncomment these if
    // it gets an implementation for all platforms...
//     wxTC_RIGHTJUSTIFY,
//     wxTC_FIXEDWIDTH,
//     wxTC_TOP,
//     wxTC_LEFT,
//     wxTC_RIGHT,
//     wxTC_BOTTOM,
//     wxTC_MULTILINE,
//     wxTC_OWNERDRAW,

};


enum wxBorder
{
    wxBORDER_DEFAULT,
    wxBORDER_NONE,
    wxBORDER_STATIC,
    wxBORDER_SIMPLE,
    wxBORDER_RAISED,
    wxBORDER_SUNKEN,
    wxBORDER_DOUBLE,
    wxBORDER_MASK,
};


//  // Standard error codes
//  enum  ErrCode
//  {
//    ERR_PARAM = (-4000),
//    ERR_NODATA,
//    ERR_CANCEL,
//    ERR_SUCCESS = 0
//  };


enum {
  wxDEFAULT ,
  wxDECORATIVE,
  wxROMAN,
  wxSCRIPT,
  wxSWISS,
  wxMODERN,
  wxTELETYPE,
  wxVARIABLE,
  wxFIXED,
  wxNORMAL,
  wxLIGHT,
  wxBOLD,
  wxITALIC,
  wxSLANT,
  wxSOLID,
  wxDOT,
  wxLONG_DASH,
  wxSHORT_DASH,
  wxDOT_DASH,
  wxUSER_DASH,
  wxTRANSPARENT,
  wxSTIPPLE,
  wxBDIAGONAL_HATCH,
  wxCROSSDIAG_HATCH,
  wxFDIAGONAL_HATCH,
  wxCROSS_HATCH,
  wxHORIZONTAL_HATCH,
  wxVERTICAL_HATCH,
  wxJOIN_BEVEL,
  wxJOIN_MITER,
  wxJOIN_ROUND,
  wxCAP_ROUND,
  wxCAP_PROJECTING,
  wxCAP_BUTT
};

typedef enum {
  wxCLEAR,      // 0
  wxXOR,        // src XOR dst
  wxINVERT,     // NOT dst
  wxOR_REVERSE, // src OR (NOT dst)
  wxAND_REVERSE,// src AND (NOT dst)
  wxCOPY,       // src
  wxAND,        // src AND dst
  wxAND_INVERT, // (NOT src) AND dst
  wxNO_OP,      // dst
  wxNOR,        // (NOT src) AND (NOT dst)
  wxEQUIV,      // (NOT src) XOR dst
  wxSRC_INVERT, // (NOT src)
  wxOR_INVERT,  // (NOT src) OR dst
  wxNAND,       // (NOT src) OR (NOT dst)
  wxOR,         // src OR dst
  wxSET,        // 1
//  wxSRC_OR,     // source _bitmap_ OR destination
//  wxSRC_AND     // source _bitmap_ AND destination
} form_ops_t;

enum wxKeyCode {
  WXK_BACK    =    8,
  WXK_TAB     =    9,
  WXK_RETURN  =    13,
  WXK_ESCAPE  =    27,
  WXK_SPACE   =    32,
  WXK_DELETE  =    127,

  WXK_START   = 300,
  WXK_LBUTTON,
  WXK_RBUTTON,
  WXK_CANCEL,
  WXK_MBUTTON,
  WXK_CLEAR,
  WXK_SHIFT,
  WXK_ALT,
  WXK_CONTROL,
  WXK_MENU,
  WXK_PAUSE,
  WXK_CAPITAL,
  WXK_PRIOR,  /* Page up */
  WXK_NEXT,   /* Page down */
  WXK_END,
  WXK_HOME,
  WXK_LEFT,
  WXK_UP,
  WXK_RIGHT,
  WXK_DOWN,
  WXK_SELECT,
  WXK_PRINT,
  WXK_EXECUTE,
  WXK_SNAPSHOT,
  WXK_INSERT,
  WXK_HELP,
  WXK_NUMPAD0,
  WXK_NUMPAD1,
  WXK_NUMPAD2,
  WXK_NUMPAD3,
  WXK_NUMPAD4,
  WXK_NUMPAD5,
  WXK_NUMPAD6,
  WXK_NUMPAD7,
  WXK_NUMPAD8,
  WXK_NUMPAD9,
  WXK_MULTIPLY,
  WXK_ADD,
  WXK_SEPARATOR,
  WXK_SUBTRACT,
  WXK_DECIMAL,
  WXK_DIVIDE,
  WXK_F1,
  WXK_F2,
  WXK_F3,
  WXK_F4,
  WXK_F5,
  WXK_F6,
  WXK_F7,
  WXK_F8,
  WXK_F9,
  WXK_F10,
  WXK_F11,
  WXK_F12,
  WXK_F13,
  WXK_F14,
  WXK_F15,
  WXK_F16,
  WXK_F17,
  WXK_F18,
  WXK_F19,
  WXK_F20,
  WXK_F21,
  WXK_F22,
  WXK_F23,
  WXK_F24,
  WXK_NUMLOCK,
  WXK_SCROLL,
  WXK_PAGEUP,
  WXK_PAGEDOWN,

  WXK_NUMPAD_SPACE,
  WXK_NUMPAD_TAB,
  WXK_NUMPAD_ENTER,
  WXK_NUMPAD_F1,
  WXK_NUMPAD_F2,
  WXK_NUMPAD_F3,
  WXK_NUMPAD_F4,
  WXK_NUMPAD_HOME,
  WXK_NUMPAD_LEFT,
  WXK_NUMPAD_UP,
  WXK_NUMPAD_RIGHT,
  WXK_NUMPAD_DOWN,
  WXK_NUMPAD_PRIOR,
  WXK_NUMPAD_PAGEUP,
  WXK_NUMPAD_NEXT,
  WXK_NUMPAD_PAGEDOWN,
  WXK_NUMPAD_END,
  WXK_NUMPAD_BEGIN,
  WXK_NUMPAD_INSERT,
  WXK_NUMPAD_DELETE,
  WXK_NUMPAD_EQUAL,
  WXK_NUMPAD_MULTIPLY,
  WXK_NUMPAD_ADD,
  WXK_NUMPAD_SEPARATOR,
  WXK_NUMPAD_SUBTRACT,
  WXK_NUMPAD_DECIMAL,
  WXK_NUMPAD_DIVIDE,

  WXK_WINDOWS_LEFT,
  WXK_WINDOWS_RIGHT,
  WXK_WINDOWS_MENU

};


// Bitmap flags
enum wxBitmapType
{
    wxBITMAP_TYPE_INVALID,          // should be == 0 for compatibility!
    wxBITMAP_TYPE_BMP,
    wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_RESOURCE = wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_ICO,
    wxBITMAP_TYPE_ICO_RESOURCE,
    wxBITMAP_TYPE_CUR,
    wxBITMAP_TYPE_CUR_RESOURCE,
    wxBITMAP_TYPE_XBM,
    wxBITMAP_TYPE_XBM_DATA,
    wxBITMAP_TYPE_XPM,
    wxBITMAP_TYPE_XPM_DATA,
    wxBITMAP_TYPE_TIF,
    wxBITMAP_TYPE_TIF_RESOURCE,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_GIF_RESOURCE,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_PNG_RESOURCE,
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_JPEG_RESOURCE,
    wxBITMAP_TYPE_PNM,
    wxBITMAP_TYPE_PNM_RESOURCE,
    wxBITMAP_TYPE_PCX,
    wxBITMAP_TYPE_PCX_RESOURCE,
    wxBITMAP_TYPE_PICT,
    wxBITMAP_TYPE_PICT_RESOURCE,
    wxBITMAP_TYPE_ICON,
    wxBITMAP_TYPE_ICON_RESOURCE,
    wxBITMAP_TYPE_ANI,
    wxBITMAP_TYPE_IFF,
    wxBITMAP_TYPE_MACCURSOR,
    wxBITMAP_TYPE_MACCURSOR_RESOURCE,
    wxBITMAP_TYPE_ANY = 50
};




// Standard cursors
enum wxStockCursor
{
    wxCURSOR_NONE,
    wxCURSOR_ARROW,
    wxCURSOR_RIGHT_ARROW,
    wxCURSOR_BULLSEYE,
    wxCURSOR_CHAR,
    wxCURSOR_CROSS,
    wxCURSOR_HAND,
    wxCURSOR_IBEAM,
    wxCURSOR_LEFT_BUTTON,
    wxCURSOR_MAGNIFIER,
    wxCURSOR_MIDDLE_BUTTON,
    wxCURSOR_NO_ENTRY,
    wxCURSOR_PAINT_BRUSH,
    wxCURSOR_PENCIL,
    wxCURSOR_POINT_LEFT,
    wxCURSOR_POINT_RIGHT,
    wxCURSOR_QUESTION_ARROW,
    wxCURSOR_RIGHT_BUTTON,
    wxCURSOR_SIZENESW,
    wxCURSOR_SIZENS,
    wxCURSOR_SIZENWSE,
    wxCURSOR_SIZEWE,
    wxCURSOR_SIZING,
    wxCURSOR_SPRAYCAN,
    wxCURSOR_WAIT,
    wxCURSOR_WATCH,
    wxCURSOR_BLANK,
    wxCURSOR_DEFAULT,
    wxCURSOR_ARROWWAIT,
    wxCURSOR_MAX
};



typedef enum {
    wxPAPER_NONE,               // Use specific dimensions
    wxPAPER_LETTER,             // Letter, 8 1/2 by 11 inches
    wxPAPER_LEGAL,              // Legal, 8 1/2 by 14 inches
    wxPAPER_A4,                 // A4 Sheet, 210 by 297 millimeters
    wxPAPER_CSHEET,             // C Sheet, 17 by 22 inches
    wxPAPER_DSHEET,             // D Sheet, 22 by 34 inches
    wxPAPER_ESHEET,             // E Sheet, 34 by 44 inches
    wxPAPER_LETTERSMALL,        // Letter Small, 8 1/2 by 11 inches
    wxPAPER_TABLOID,            // Tabloid, 11 by 17 inches
    wxPAPER_LEDGER,             // Ledger, 17 by 11 inches
    wxPAPER_STATEMENT,          // Statement, 5 1/2 by 8 1/2 inches
    wxPAPER_EXECUTIVE,          // Executive, 7 1/4 by 10 1/2 inches
    wxPAPER_A3,                 // A3 sheet, 297 by 420 millimeters
    wxPAPER_A4SMALL,            // A4 small sheet, 210 by 297 millimeters
    wxPAPER_A5,                 // A5 sheet, 148 by 210 millimeters
    wxPAPER_B4,                 // B4 sheet, 250 by 354 millimeters
    wxPAPER_B5,                 // B5 sheet, 182-by-257-millimeter paper
    wxPAPER_FOLIO,              // Folio, 8-1/2-by-13-inch paper
    wxPAPER_QUARTO,             // Quarto, 215-by-275-millimeter paper
    wxPAPER_10X14,              // 10-by-14-inch sheet
    wxPAPER_11X17,              // 11-by-17-inch sheet
    wxPAPER_NOTE,               // Note, 8 1/2 by 11 inches
    wxPAPER_ENV_9,              // #9 Envelope, 3 7/8 by 8 7/8 inches
    wxPAPER_ENV_10,             // #10 Envelope, 4 1/8 by 9 1/2 inches
    wxPAPER_ENV_11,             // #11 Envelope, 4 1/2 by 10 3/8 inches
    wxPAPER_ENV_12,             // #12 Envelope, 4 3/4 by 11 inches
    wxPAPER_ENV_14,             // #14 Envelope, 5 by 11 1/2 inches
    wxPAPER_ENV_DL,             // DL Envelope, 110 by 220 millimeters
    wxPAPER_ENV_C5,             // C5 Envelope, 162 by 229 millimeters
    wxPAPER_ENV_C3,             // C3 Envelope, 324 by 458 millimeters
    wxPAPER_ENV_C4,             // C4 Envelope, 229 by 324 millimeters
    wxPAPER_ENV_C6,             // C6 Envelope, 114 by 162 millimeters
    wxPAPER_ENV_C65,            // C65 Envelope, 114 by 229 millimeters
    wxPAPER_ENV_B4,             // B4 Envelope, 250 by 353 millimeters
    wxPAPER_ENV_B5,             // B5 Envelope, 176 by 250 millimeters
    wxPAPER_ENV_B6,             // B6 Envelope, 176 by 125 millimeters
    wxPAPER_ENV_ITALY,          // Italy Envelope, 110 by 230 millimeters
    wxPAPER_ENV_MONARCH,        // Monarch Envelope, 3 7/8 by 7 1/2 inches
    wxPAPER_ENV_PERSONAL,       // 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    wxPAPER_FANFOLD_US,         // US Std Fanfold, 14 7/8 by 11 inches
    wxPAPER_FANFOLD_STD_GERMAN, // German Std Fanfold, 8 1/2 by 12 inches
    wxPAPER_FANFOLD_LGL_GERMAN, // German Legal Fanfold, 8 1/2 by 13 inches

    wxPAPER_ISO_B4,             // B4 (ISO) 250 x 353 mm
    wxPAPER_JAPANESE_POSTCARD,  // Japanese Postcard 100 x 148 mm
    wxPAPER_9X11,               // 9 x 11 in
    wxPAPER_10X11,              // 10 x 11 in
    wxPAPER_15X11,              // 15 x 11 in
    wxPAPER_ENV_INVITE,         // Envelope Invite 220 x 220 mm
    wxPAPER_LETTER_EXTRA,       // Letter Extra 9 \275 x 12 in
    wxPAPER_LEGAL_EXTRA,        // Legal Extra 9 \275 x 15 in
    wxPAPER_TABLOID_EXTRA,      // Tabloid Extra 11.69 x 18 in
    wxPAPER_A4_EXTRA,           // A4 Extra 9.27 x 12.69 in
    wxPAPER_LETTER_TRANSVERSE,  // Letter Transverse 8 \275 x 11 in
    wxPAPER_A4_TRANSVERSE,      // A4 Transverse 210 x 297 mm
    wxPAPER_LETTER_EXTRA_TRANSVERSE, // Letter Extra Transverse 9\275 x 12 in
    wxPAPER_A_PLUS,             // SuperA/SuperA/A4 227 x 356 mm
    wxPAPER_B_PLUS,             // SuperB/SuperB/A3 305 x 487 mm
    wxPAPER_LETTER_PLUS,        // Letter Plus 8.5 x 12.69 in
    wxPAPER_A4_PLUS,            // A4 Plus 210 x 330 mm
    wxPAPER_A5_TRANSVERSE,      // A5 Transverse 148 x 210 mm
    wxPAPER_B5_TRANSVERSE,      // B5 (JIS) Transverse 182 x 257 mm
    wxPAPER_A3_EXTRA,           // A3 Extra 322 x 445 mm
    wxPAPER_A5_EXTRA,           // A5 Extra 174 x 235 mm
    wxPAPER_B5_EXTRA,           // B5 (ISO) Extra 201 x 276 mm
    wxPAPER_A2,                 // A2 420 x 594 mm
    wxPAPER_A3_TRANSVERSE,      // A3 Transverse 297 x 420 mm
    wxPAPER_A3_EXTRA_TRANSVERSE // A3 Extra Transverse 322 x 445 mm

} wxPaperSize ;

typedef enum {
    wxDUPLEX_SIMPLEX, // Non-duplex
    wxDUPLEX_HORIZONTAL,
    wxDUPLEX_VERTICAL
} wxDuplexMode;



// menu and toolbar item kinds
enum wxItemKind
{
    wxITEM_SEPARATOR = -1,
    wxITEM_NORMAL,
    wxITEM_CHECK,
    wxITEM_RADIO,
    wxITEM_MAX
};

enum wxHitTest
{
    wxHT_NOWHERE,

    // scrollbar
    wxHT_SCROLLBAR_FIRST = wxHT_NOWHERE,
    wxHT_SCROLLBAR_ARROW_LINE_1,    // left or upper arrow to scroll by line
    wxHT_SCROLLBAR_ARROW_LINE_2,    // right or down
    wxHT_SCROLLBAR_ARROW_PAGE_1,    // left or upper arrow to scroll by page
    wxHT_SCROLLBAR_ARROW_PAGE_2,    // right or down
    wxHT_SCROLLBAR_THUMB,           // on the thumb
    wxHT_SCROLLBAR_BAR_1,           // bar to the left/above the thumb
    wxHT_SCROLLBAR_BAR_2,           // bar to the right/below the thumb
    wxHT_SCROLLBAR_LAST,

    // window
    wxHT_WINDOW_OUTSIDE,            // not in this window at all
    wxHT_WINDOW_INSIDE,             // in the client area
    wxHT_WINDOW_VERT_SCROLLBAR,     // on the vertical scrollbar
    wxHT_WINDOW_HORZ_SCROLLBAR,     // on the horizontal scrollbar
    wxHT_WINDOW_CORNER,             // on the corner between 2 scrollbars

    wxHT_MAX
};


%{
#if ! wxUSE_HOTKEY
enum wxHotkeyModifier
{
    wxMOD_NONE = 0,
    wxMOD_ALT = 1,
    wxMOD_CONTROL = 2,
    wxMOD_SHIFT = 4,
    wxMOD_WIN = 8
};
#define wxEVT_HOTKEY 9999
#endif
%}

enum wxHotkeyModifier
{
    wxMOD_NONE = 0,
    wxMOD_ALT = 1,
    wxMOD_CONTROL = 2,
    wxMOD_SHIFT = 4,
    wxMOD_WIN = 8
};


enum wxUpdateUI
{
    wxUPDATE_UI_NONE          = 0x0000,
    wxUPDATE_UI_RECURSE       = 0x0001,
    wxUPDATE_UI_FROMIDLE      = 0x0002 // Invoked from On(Internal)Idle
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/*
 * Event types
 *
 */
enum wxEventType {
 wxEVT_NULL = 0,
 wxEVT_FIRST = 10000,

 // New names
 wxEVT_COMMAND_BUTTON_CLICKED,
 wxEVT_COMMAND_CHECKBOX_CLICKED,
 wxEVT_COMMAND_CHOICE_SELECTED,
 wxEVT_COMMAND_LISTBOX_SELECTED,
 wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
 wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
 wxEVT_COMMAND_SPINCTRL_UPDATED,
 wxEVT_COMMAND_TEXT_UPDATED,
 wxEVT_COMMAND_TEXT_ENTER,
 wxEVT_COMMAND_TEXT_URL,
 wxEVT_COMMAND_TEXT_MAXLEN,
 wxEVT_COMMAND_MENU_SELECTED,
 wxEVT_COMMAND_SLIDER_UPDATED,
 wxEVT_COMMAND_RADIOBOX_SELECTED,
 wxEVT_COMMAND_RADIOBUTTON_SELECTED,
// wxEVT_COMMAND_SCROLLBAR_UPDATED is now obsolete since we use wxEVT_SCROLL... events
 wxEVT_COMMAND_SCROLLBAR_UPDATED,
 wxEVT_COMMAND_VLBOX_SELECTED,
 wxEVT_COMMAND_COMBOBOX_SELECTED,
 wxEVT_COMMAND_TOOL_CLICKED,
 wxEVT_COMMAND_TOOL_RCLICKED,
 wxEVT_COMMAND_TOOL_ENTER,
 wxEVT_SET_FOCUS,
 wxEVT_KILL_FOCUS,
 wxEVT_CHILD_FOCUS,
 wxEVT_MOUSEWHEEL,

/* Mouse event types */
 wxEVT_LEFT_DOWN,
 wxEVT_LEFT_UP,
 wxEVT_MIDDLE_DOWN,
 wxEVT_MIDDLE_UP,
 wxEVT_RIGHT_DOWN,
 wxEVT_RIGHT_UP,
 wxEVT_MOTION,
 wxEVT_ENTER_WINDOW,
 wxEVT_LEAVE_WINDOW,
 wxEVT_LEFT_DCLICK,
 wxEVT_MIDDLE_DCLICK,
 wxEVT_RIGHT_DCLICK,

 wxEVT_MOUSE_CAPTURE_CHANGED,

 // Non-client mouse events
 wxEVT_NC_LEFT_DOWN,
 wxEVT_NC_LEFT_UP,
 wxEVT_NC_MIDDLE_DOWN,
 wxEVT_NC_MIDDLE_UP,
 wxEVT_NC_RIGHT_DOWN,
 wxEVT_NC_RIGHT_UP,
 wxEVT_NC_MOTION,
 wxEVT_NC_ENTER_WINDOW,
 wxEVT_NC_LEAVE_WINDOW,
 wxEVT_NC_LEFT_DCLICK,
 wxEVT_NC_MIDDLE_DCLICK,
 wxEVT_NC_RIGHT_DCLICK,

 wxEVT_SET_CURSOR,

/* Character input event type  */
 wxEVT_CHAR,
 wxEVT_KEY_DOWN,
 wxEVT_KEY_UP,
 wxEVT_CHAR_HOOK,
 wxEVT_HOTKEY,

 /*
  * Scrollbar event identifiers
  */
 wxEVT_SCROLL_TOP,
 wxEVT_SCROLL_BOTTOM,
 wxEVT_SCROLL_LINEUP,
 wxEVT_SCROLL_LINEDOWN,
 wxEVT_SCROLL_PAGEUP,
 wxEVT_SCROLL_PAGEDOWN,
 wxEVT_SCROLL_THUMBTRACK,
 wxEVT_SCROLL_THUMBRELEASE,
 wxEVT_SCROLL_ENDSCROLL,

 /*
  * Scrolled Window
  */
 wxEVT_SCROLLWIN_TOP,
 wxEVT_SCROLLWIN_BOTTOM,
 wxEVT_SCROLLWIN_LINEUP,
 wxEVT_SCROLLWIN_LINEDOWN,
 wxEVT_SCROLLWIN_PAGEUP,
 wxEVT_SCROLLWIN_PAGEDOWN,
 wxEVT_SCROLLWIN_THUMBTRACK,
 wxEVT_SCROLLWIN_THUMBRELEASE,

 wxEVT_SIZE = wxEVT_FIRST + 200,
 wxEVT_MOVE,
 wxEVT_SIZING,
 wxEVT_MOVING,
 wxEVT_CLOSE_WINDOW,
 wxEVT_END_SESSION,
 wxEVT_QUERY_END_SESSION,
 wxEVT_ACTIVATE_APP,
 wxEVT_POWER,
 wxEVT_ACTIVATE,
 wxEVT_CREATE,
 wxEVT_DESTROY,
 wxEVT_SHOW,
 wxEVT_ICONIZE,
 wxEVT_MAXIMIZE,
 wxEVT_PAINT,
 wxEVT_ERASE_BACKGROUND,
 wxEVT_NC_PAINT,
 wxEVT_PAINT_ICON,
 wxEVT_MENU_OPEN,
 wxEVT_MENU_CLOSE,
 wxEVT_MENU_HIGHLIGHT,
 wxEVT_CONTEXT_MENU,
 wxEVT_SYS_COLOUR_CHANGED,
 wxEVT_DISPLAY_CHANGED,
 wxEVT_SETTING_CHANGED,
 wxEVT_QUERY_NEW_PALETTE,
 wxEVT_PALETTE_CHANGED,
 wxEVT_JOY_BUTTON_DOWN,
 wxEVT_JOY_BUTTON_UP,
 wxEVT_JOY_MOVE,
 wxEVT_JOY_ZMOVE,
 wxEVT_DROP_FILES,
 wxEVT_DRAW_ITEM,
 wxEVT_MEASURE_ITEM,
 wxEVT_COMPARE_ITEM,
 wxEVT_INIT_DIALOG,
 wxEVT_IDLE,
 wxEVT_UPDATE_UI,


 /* Generic command events */
 // Note: a click is a higher-level event
 // than button down/up
 wxEVT_COMMAND_LEFT_CLICK,
 wxEVT_COMMAND_LEFT_DCLICK,
 wxEVT_COMMAND_RIGHT_CLICK,
 wxEVT_COMMAND_RIGHT_DCLICK,
 wxEVT_COMMAND_SET_FOCUS,
 wxEVT_COMMAND_KILL_FOCUS,
 wxEVT_COMMAND_ENTER,

 wxEVT_NAVIGATION_KEY,

 wxEVT_TIMER,

};




//----------------------------------------------------------------------
