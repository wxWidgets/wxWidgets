/////////////////////////////////////////////////////////////////////////////
// Name:        defsext.h extensions
// Purpose:     STC test declarations
// Maintainer:  Wyo
// Created:     2003-09-01
// Copyright:   (c) wxGuide
// Licence:     wxWindows licence
//////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEFSEXT_H_
#define _WX_DEFSEXT_H_

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

//! wxWidgets headers
#include "wx/print.h"    // printing support
#include "wx/printdlg.h" // printing dialog


//============================================================================
// declarations
//============================================================================

#define DEFAULT_LANGUAGE "<default>"
#define STYLE_TYPES_COUNT 32


// ----------------------------------------------------------------------------
// standard IDs
// ----------------------------------------------------------------------------

enum {
    // menu IDs
    myID_PROPERTIES = wxID_HIGHEST,
    myID_EDIT_FIRST,
    myID_INDENTINC = myID_EDIT_FIRST,
    myID_INDENTRED,
    myID_FINDNEXT,
    myID_REPLACE,
    myID_REPLACENEXT,
    myID_BRACEMATCH,
    myID_GOTO,
    myID_DISPLAYEOL,
    myID_INDENTGUIDE,
    myID_LINENUMBER,
    myID_LONGLINEON,
    myID_WHITESPACE,
    myID_FOLDTOGGLE,
    myID_OVERTYPE,
    myID_READONLY,
    myID_WRAPMODEON,
    myID_ANNOTATION_ADD,
    myID_ANNOTATION_REMOVE,
    myID_ANNOTATION_CLEAR,
    myID_ANNOTATION_STYLE_HIDDEN,
    myID_ANNOTATION_STYLE_STANDARD,
    myID_ANNOTATION_STYLE_BOXED,

    myID_INDICATOR_FILL,
    myID_INDICATOR_CLEAR,
    myID_INDICATOR_STYLE_FIRST,
    myID_INDICATOR_STYLE_PLAIN = myID_INDICATOR_STYLE_FIRST,
    myID_INDICATOR_STYLE_SQUIGGLE,
    myID_INDICATOR_STYLE_TT,
    myID_INDICATOR_STYLE_DIAGONAL,
    myID_INDICATOR_STYLE_STRIKE,
    myID_INDICATOR_STYLE_HIDDEN,
    myID_INDICATOR_STYLE_BOX,
    myID_INDICATOR_STYLE_ROUNDBOX,
    myID_INDICATOR_STYLE_STRAIGHTBOX,
    myID_INDICATOR_STYLE_DASH,
    myID_INDICATOR_STYLE_DOTS,
    myID_INDICATOR_STYLE_SQUIGGLELOW,
    myID_INDICATOR_STYLE_DOTBOX,
    myID_INDICATOR_STYLE_SQUIGGLEPIXMAP,
    myID_INDICATOR_STYLE_COMPOSITIONTHICK,
    myID_INDICATOR_STYLE_COMPOSITIONTHIN,
    myID_INDICATOR_STYLE_FULLBOX,
    myID_INDICATOR_STYLE_TEXTFORE,
    myID_INDICATOR_STYLE_POINT,
    myID_INDICATOR_STYLE_POINTCHARACTER,
    myID_INDICATOR_STYLE_LAST = myID_INDICATOR_STYLE_POINTCHARACTER,

    myID_CHANGECASE,
    myID_CHANGELOWER,
    myID_CHANGEUPPER,
    myID_HIGHLIGHTLANG,
    myID_HIGHLIGHTFIRST,
    myID_HIGHLIGHTLAST = myID_HIGHLIGHTFIRST + 99,
    myID_CONVERTEOL,
    myID_CONVERTCR,
    myID_CONVERTCRLF,
    myID_CONVERTLF,
    myID_MULTIPLE_SELECTIONS,
    myID_MULTI_PASTE,
    myID_MULTIPLE_SELECTIONS_TYPING,
    myID_TECHNOLOGY_DEFAULT,
    myID_TECHNOLOGY_DIRECTWRITE,
    myID_CUSTOM_POPUP,
    myID_USECHARSET,
    myID_CHARSETANSI,
    myID_CHARSETMAC,
    myID_SELECTLINE,
    myID_EDIT_LAST = myID_SELECTLINE,
    myID_WINDOW_MINIMAL,

    // other IDs
    myID_ABOUTTIMER,
};

// ----------------------------------------------------------------------------
// global items
// ----------------------------------------------------------------------------

//! global application name
extern wxString *g_appname;

#if wxUSE_PRINTING_ARCHITECTURE

//! global print data, to remember settings during the session
extern wxPrintData *g_printData;
extern wxPageSetupDialogData *g_pageSetupData;

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif // _WX_DEFSEXT_H_
