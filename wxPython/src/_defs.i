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
// Globally turn on the autodoc feature

%feature("autodoc", "1");  // 0 == no param types, 1 == show param types

//---------------------------------------------------------------------------
// Tell SWIG to wrap all the wrappers with our thread protection by default

%exception {
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    $action
    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) SWIG_fail;
}


// This one can be used to add a check for an existing wxApp before the real
// work is done.  An exception is raised if there isn't one.
%define MustHaveApp(name)
%exception name {
    if (!wxPyCheckForApp()) SWIG_fail;
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    $action
    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) SWIG_fail;
}
%enddef

    

//---------------------------------------------------------------------------
// some type definitions to simplify things for SWIG

typedef int             wxEventType;
typedef unsigned int    size_t;
typedef unsigned int    time_t;
typedef unsigned char   byte;
typedef unsigned long   wxUIntPtr;

#define wxWindowID      int
#define wxCoord         int
#define wxInt32         int
#define wxUint32        unsigned int


//----------------------------------------------------------------------
// Various SWIG macros and such

#define %pythonAppend   %feature("pythonappend")
#define %pythonPrepend  %feature("pythonprepend")
#define %kwargs         %feature("kwargs")
#define %nokwargs       %feature("nokwargs")
#define %noautodoc %feature("noautodoc")


//#ifndef %shadow
//#define %shadow         %insert("shadow")
//#endif

#ifndef %pythoncode
#define %pythoncode     %insert("python")
#endif

#define WXUNUSED(x)     x


// Given the name of a wxChar (or wxString) constant in C++, make
// a static wxString for wxPython, and also let SWIG wrap it.
%define MAKE_CONST_WXSTRING(strname)
    %{ static const wxString wxPy##strname(wx##strname); %}
    %immutable;
    %rename(strname) wxPy##strname;
    const wxString wxPy##strname;
    %mutable;
%enddef

%define MAKE_CONST_WXSTRING2(strname, val)
    %{ static const wxString wxPy##strname(val); %}
    %immutable;
    %rename(strname) wxPy##strname;
    const wxString wxPy##strname;
    %mutable;
%enddef

%define MAKE_CONST_WXSTRING_NOSWIG(strname)
    %{ static const wxString wxPy##strname(wx##strname); %}
%enddef

// Generate code in the module init for the event types, since they may not be
// initialized yet when they are used in the static swig_const_table.
%typemap(consttab) wxEventType; // TODO: how to prevent code inserted into the consttab?
%typemap(constcode) wxEventType "PyDict_SetItemString(d, \"$symname\", PyInt_FromLong($value));";



//----------------------------------------------------------------------
// Macros for the docstring and autodoc features of SWIG.  These will
// help make the code look more readable, and pretty, as well as help
// reduce typing in some cases.

// Set the docsring for the given full or partial declaration
#ifdef _DO_FULL_DOCS
    %define DocStr(decl, docstr, details)
        %feature("docstring") decl docstr details;
    %enddef
#else
    %define DocStr(decl, docstr, details)
        %feature("docstring") decl docstr;
    %enddef
#endif


// Set the autodoc string for a full or partial declaration
%define DocA(decl, astr)
    %feature("autodoc") decl astr;
%enddef


// Set both the autodoc and docstring for a full or partial declaration
#ifdef _DO_FULL_DOCS
    %define DocAStr(decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr details
    %enddef
#else
    %define DocAStr(decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr
    %enddef
#endif

        

    
// Set the docstring for a decl and then define the decl too.  Must use the
// full declaration of the item.
#ifdef _DO_FULL_DOCS
    %define DocDeclStr(type, decl, docstr, details)
        %feature("docstring") decl docstr details;
        type decl
    %enddef
#else
    %define DocDeclStr(type, decl, docstr, details)
        %feature("docstring") decl docstr;
        type decl
    %enddef
#endif

        
    
// As above, but also give the decl a new %name    
#ifdef _DO_FULL_DOCS
    %define DocDeclStrName(type, decl, docstr, details, newname)
        %feature("docstring") decl docstr details;
        %rename(newname) decl;
        type decl
    %enddef
#else
    %define DocDeclStrName(type, decl, docstr, details, newname)
        %feature("docstring") decl docstr;
        %rename(newname) decl;
        type decl
    %enddef
#endif
        
    
// Set the autodoc string for a decl and then define the decl too.  Must use the
// full declaration of the item.
%define DocDeclA(type, decl, astr)
    %feature("autodoc") decl astr;
    type decl
%enddef

// As above, but also give the decl a new %name    
%define DocDeclAName(type, decl, astr, newname)
    %feature("autodoc") decl astr;
    %rename(newname) decl;
    type decl
%enddef



// Set the autodoc and the docstring for a decl and then define the decl too.
// Must use the full declaration of the item.
#ifdef _DO_FULL_DOCS
    %define DocDeclAStr(type, decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr details;
        type decl
    %enddef
#else
    %define DocDeclAStr(type, decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr;
        type decl
    %enddef
#endif

        
// As above, but also give the decl a new %name    
#ifdef _DO_FULL_DOCS
    %define DocDeclAStrName(type, decl, astr, docstr, details, newname)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr details;
        %rename(newname) decl;
        type decl
    %enddef
#else
    %define DocDeclAStrName(type, decl, astr, docstr, details, newname)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr;
        %rename(newname) decl;
        type decl
    %enddef
#endif



// Set the docstring for a constructor decl and then define the decl too.
// Must use the full declaration of the item.
#ifdef _DO_FULL_DOCS
    %define DocCtorStr(decl, docstr, details)
        %feature("docstring") decl docstr details;
        decl
    %enddef
#else
    %define DocCtorStr(decl, docstr, details)
        %feature("docstring") decl docstr;
        decl
    %enddef
#endif

        
// As above, but also give the decl a new %name    
#ifdef _DO_FULL_DOCS
    %define DocCtorStrName(decl, docstr, details, newname)
        %feature("docstring") decl docstr details;
        %rename(newname) decl;
        decl
    %enddef
#else
    %define DocCtorStrName(decl, docstr, details, newname)
        %feature("docstring") decl docstr;
        %rename(newname) decl;
        decl
    %enddef
#endif


        
// Set the autodoc string for a constructor decl and then define the decl too.
// Must use the full declaration of the item.
%define DocCtorA(decl, astr)
    %feature("autodoc") decl astr;
    decl
%enddef

// As above, but also give the decl a new %name    
%define DocCtorAName(decl, astr, newname)
    %feature("autodoc") decl astr;
    %rename(newname) decl;
    decl
%enddef



// Set the autodoc and the docstring for a constructor decl and then define
// the decl too.  Must use the full declaration of the item.
#ifdef _DO_FULL_DOCS
    %define DocCtorAStr(decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr details;
        decl
    %enddef
#else
    %define DocCtorAStr(decl, astr, docstr, details)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr;
        decl
    %enddef
#endif


        
// As above, but also give the decl a new %name    
#ifdef _DO_FULL_DOCS
    %define DocCtorAStrName(decl, astr, docstr, details, newname)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr details;
        %rename(newname) decl;
        decl
    %enddef
#else
    %define DocCtorAStrName(decl, astr, docstr, details, newname)
        %feature("autodoc") decl astr;
        %feature("docstring") decl docstr;
        %rename(newname) decl;
        decl
    %enddef
#endif

       
    
%define %newgroup
%pythoncode {
%#---------------------------------------------------------------------------
}
%enddef


// A set of macros to make using %rename easier, since %name has been
// deprecated...
%define %Rename(newname, type, decl)
    %rename(newname) decl;
    type decl
%enddef

%define %RenameCtor(newname, decl)
    %rename(newname) decl;
    decl
%enddef


//---------------------------------------------------------------------------
// Forward declarations and %renames for some classes, so the autodoc strings
// will be able to use the right types even when the real class declaration is
// not in the module being processed or seen by %import's.

#ifdef BUILDING_RENAMERS
    #define FORWARD_DECLARE(wxName, Name)
#else
    %define FORWARD_DECLARE(wxName, Name)
        %rename(Name) wxName;
        class wxName;
    %enddef
#endif

FORWARD_DECLARE(wxString,         String);
FORWARD_DECLARE(wxBitmap,         Bitmap);
FORWARD_DECLARE(wxDateTime,       DateTime);
FORWARD_DECLARE(wxInputStream,    InputStream);
FORWARD_DECLARE(wxDC,             DC);
FORWARD_DECLARE(wxCursor,         Cursor);
FORWARD_DECLARE(wxRegion,         Region);
FORWARD_DECLARE(wxColour,         Colour);
FORWARD_DECLARE(wxFont,           Font);
FORWARD_DECLARE(wxCaret,          Caret);
FORWARD_DECLARE(wxToolTip,        ToolTip);
FORWARD_DECLARE(wxPyDropTarget,   DropTarget);
FORWARD_DECLARE(wxImageList,      ImageList);
FORWARD_DECLARE(wxMemoryDC,       MemoryDC);
FORWARD_DECLARE(wxHtmlTagHandler, HtmlTagHandler);
FORWARD_DECLARE(wxConfigBase,     ConfigBase);
FORWARD_DECLARE(wxIcon,           Icon);
FORWARD_DECLARE(wxStaticBox,      StaticBox);


//---------------------------------------------------------------------------

// General numeric #define's and etc.  Making them all enums makes SWIG use the
// real macro when making the Python Int

enum {
//     wxMAJOR_VERSION,
//     wxMINOR_VERSION,
//     wxRELEASE_NUMBER,

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

    wxTAB_TRAVERSAL,
    wxWANTS_CHARS,
    wxPOPUP_WINDOW,
    wxCENTER_FRAME,
    wxCENTRE_ON_SCREEN,
    wxCENTER_ON_SCREEN,

    wxED_CLIENT_MARGIN,
    wxED_BUTTONS_BOTTOM,
    wxED_BUTTONS_RIGHT,
    wxED_STATIC_LINE,
    wxEXT_DIALOG_STYLE,

    wxCLIP_CHILDREN,
    wxCLIP_SIBLINGS,

    wxALWAYS_SHOW_SB,
    
    wxRETAINED,
    wxBACKINGSTORE,

    wxCOLOURED,
    wxFIXED_LENGTH,

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

    wxID_LOWEST,
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
    wxID_PREFERENCES,

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

    wxID_ADD,
    wxID_REMOVE,

    wxID_UP,
    wxID_DOWN,
    wxID_HOME,
    wxID_REFRESH,
    wxID_STOP,
    wxID_INDEX,

    wxID_BOLD,
    wxID_ITALIC,
    wxID_JUSTIFY_CENTER,
    wxID_JUSTIFY_FILL,
    wxID_JUSTIFY_RIGHT,
    wxID_JUSTIFY_LEFT,
    wxID_UNDERLINE,
    wxID_INDENT,
    wxID_UNINDENT,
    wxID_ZOOM_100,
    wxID_ZOOM_FIT,
    wxID_ZOOM_IN,
    wxID_ZOOM_OUT,
    wxID_UNDELETE,
    wxID_REVERT_TO_SAVED,
   
    wxID_HIGHEST,

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
    wxFULL_REPAINT_ON_RESIZE,
    
    wxLI_HORIZONTAL,
    wxLI_VERTICAL,

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


#ifdef __WXGTK__
#define wxDEFAULT_STATUSBAR_STYLE wxST_SIZEGRIP|wxFULL_REPAINT_ON_RESIZE
#else
#define wxDEFAULT_STATUSBAR_STYLE wxST_SIZEGRIP
#endif



enum wxGeometryCentre
{
    wxCENTRE                  = 0x0001,
    wxCENTER                  = wxCENTRE
};


enum wxOrientation
{
    wxHORIZONTAL,
    wxVERTICAL,
    wxBOTH
};

enum wxDirection
{
    wxLEFT,
    wxRIGHT,
    wxUP,
    wxDOWN,

    wxTOP,
    wxBOTTOM,

    wxNORTH,
    wxSOUTH,
    wxWEST,
    wxEAST,

    wxALL
};

enum wxAlignment
{
    wxALIGN_NOT,
    wxALIGN_CENTER_HORIZONTAL,
    wxALIGN_CENTRE_HORIZONTAL,
    wxALIGN_LEFT,
    wxALIGN_TOP,
    wxALIGN_RIGHT,
    wxALIGN_BOTTOM,
    wxALIGN_CENTER_VERTICAL,
    wxALIGN_CENTRE_VERTICAL,

    wxALIGN_CENTER,
    wxALIGN_CENTRE,

    wxALIGN_MASK,
};

enum wxStretch
{
    wxSTRETCH_NOT,
    wxSHRINK,
    wxGROW,
    wxEXPAND,
    wxSHAPED,
    wxFIXED_MINSIZE,
    wxTILE,
    wxADJUST_MINSIZE,
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


enum wxBackgroundStyle
{
  wxBG_STYLE_SYSTEM,
  wxBG_STYLE_COLOUR,
  wxBG_STYLE_CUSTOM
};


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
    wxITEM_SEPARATOR,
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

