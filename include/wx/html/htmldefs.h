/////////////////////////////////////////////////////////////////////////////
// Name:        htmldefs.h
// Purpose:     constants for wxhtml library
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __HTMLDEFS_H__
#define __HTMLDEFS_H__

#include "wx/defs.h"

#if wxUSE_HTML

//--------------------------------------------------------------------------------
// ALIGNMENTS
//                  Describes alignment of text etc. in containers
//--------------------------------------------------------------------------------

#define HTML_ALIGN_LEFT            0x0000
#define HTML_ALIGN_RIGHT           0x0002

#define HTML_ALIGN_TOP             0x0004
#define HTML_ALIGN_BOTTOM          0x0008

#define HTML_ALIGN_CENTER          0x0001



//--------------------------------------------------------------------------------
// COLOR MODES
//                  Used by wxHtmlColourCell to determine clr of what is changing
//--------------------------------------------------------------------------------

#define HTML_CLR_FOREGROUND        0x0001
#define HTML_CLR_BACKGROUND        0x0002



//--------------------------------------------------------------------------------
// UNITS
//                  Used to specify units
//--------------------------------------------------------------------------------

#define HTML_UNITS_PIXELS          0x0001
#define HTML_UNITS_PERCENT         0x0002



//--------------------------------------------------------------------------------
// INDENTS
//                  Used to specify indetation relatives
//--------------------------------------------------------------------------------

#define HTML_INDENT_LEFT           0x0010
#define HTML_INDENT_RIGHT          0x0020
#define HTML_INDENT_TOP            0x0040
#define HTML_INDENT_BOTTOM         0x0080

#define HTML_INDENT_HORIZONTAL     HTML_INDENT_LEFT | HTML_INDENT_RIGHT
#define HTML_INDENT_VERTICAL       HTML_INDENT_TOP | HTML_INDENT_BOTTOM
#define HTML_INDENT_ALL            HTML_INDENT_VERTICAL | HTML_INDENT_HORIZONTAL




//--------------------------------------------------------------------------------
// FIND CONDITIONS
//                  Identifiers of wxHtmlCell's Find() conditions
//--------------------------------------------------------------------------------

#define HTML_COND_ISANCHOR              1
        // Finds the anchor of 'param' name (pointer to wxString).
	
#define HTML_COND_ISIMAGEMAP            2
        // Finds imagemap of 'param' name (pointer to wxString).
	// (used exclusively by mod_image.cpp)

#define HTML_COND_USER              10000
        // User-defined conditions should start from this number


//--------------------------------------------------------------------------------
// INTERNALS
//                  wxHTML internal constants
//--------------------------------------------------------------------------------

#define HTML_SCROLL_STEP               16
    /* size of one scroll step of wxHtmlWindow in pixels */
#define HTML_BUFLEN                  1024
    /* size of temporary buffer used during parsing */
#define HTML_REALLOC_STEP              32
    /* steps of array reallocation */

#endif
#endif