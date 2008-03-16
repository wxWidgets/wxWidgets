/////////////////////////////////////////////////////////////////////////////
// Name:        wx/brush.h
// Purpose:     Includes platform-specific wxBrush file
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_BASE_
#define _WX_BRUSH_H_BASE_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"      // for wxGDIObjListBase

// NOTE: these values cannot be combined together!
enum wxBrushStyle
{
#if WXWIN_COMPATIBILITY_2_8
    /* start of deprecated values */
    wxSOLID = 100,
    wxTRANSPARENT = 106,
    wxSTIPPLE_MASK_OPAQUE = 107,
    wxSTIPPLE_MASK = 108,
    wxSTIPPLE = 109,
    wxBDIAGONAL_HATCH = 110,
    wxCROSSDIAG_HATCH = 111,
    wxFDIAGONAL_HATCH = 112,
    wxCROSS_HATCH = 113,
    wxHORIZONTAL_HATCH = 114,
    wxVERTICAL_HATCH = 115,
    wxFIRST_HATCH = wxBDIAGONAL_HATCH,
    wxLAST_HATCH = wxVERTICAL_HATCH,
    /* end of deprecated values */

    wxBRUSHSTYLE_SOLID = wxSOLID,
    wxBRUSHSTYLE_TRANSPARENT = wxTRANSPARENT,
    wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE = wxSTIPPLE_MASK_OPAQUE,
    wxBRUSHSTYLE_STIPPLE_MASK = wxSTIPPLE_MASK,
    wxBRUSHSTYLE_STIPPLE = wxSTIPPLE,
    wxBRUSHSTYLE_BDIAGONAL_HATCH = wxBDIAGONAL_HATCH,
    wxBRUSHSTYLE_CROSSDIAG_HATCH = wxCROSSDIAG_HATCH,
    wxBRUSHSTYLE_FDIAGONAL_HATCH = wxFDIAGONAL_HATCH,
    wxBRUSHSTYLE_CROSS_HATCH = wxCROSS_HATCH,
    wxBRUSHSTYLE_HORIZONTAL_HATCH = wxHORIZONTAL_HATCH,
    wxBRUSHSTYLE_VERTICAL_HATCH = wxVERTICAL_HATCH,
    wxBRUSHSTYLE_FIRST_HATCH = wxFIRST_HATCH,
    wxBRUSHSTYLE_LAST_HATCH = wxLAST_HATCH,
    wxBRUSHSTYLE_MAX
#else
    wxBRUSHSTYLE_SOLID,
    wxBRUSHSTYLE_TRANSPARENT,

    /*  Brush Stippling. */

    wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE,
        /* mask is used for blitting monochrome using text fore and back ground colors */

    wxBRUSHSTYLE_STIPPLE_MASK,
        /* mask is used for masking areas in the stipple bitmap (TO DO) */

    wxBRUSHSTYLE_STIPPLE,
        /*  drawn with a Pen, and without any Brush -- and it can be stippled. */

    /* In wxWidgets < 2.6 use WX_HATCH macro  */
    /* to verify these wx*_HATCH are in style */
    /* of wxBrush. In wxWidgets >= 2.6 use    */
    /* wxBrush::IsHatch() instead.            */
    wxBRUSHSTYLE_BDIAGONAL_HATCH,
    wxBRUSHSTYLE_CROSSDIAG_HATCH,
    wxBRUSHSTYLE_FDIAGONAL_HATCH,
    wxBRUSHSTYLE_CROSS_HATCH,
    wxBRUSHSTYLE_HORIZONTAL_HATCH,
    wxBRUSHSTYLE_VERTICAL_HATCH,

    wxBRUSHSTYLE_FIRST_HATCH = wxBRUSHSTYLE_BDIAGONAL_HATCH,
    wxBRUSHSTYLE_LAST_HATCH = wxBRUSHSTYLE_VERTICAL_HATCH,

    wxBRUSHSTYLE_MAX
#endif
};


// wxBrushBase
class WXDLLEXPORT wxBrushBase: public wxGDIObject
{
public:
    virtual ~wxBrushBase() { }

    virtual wxBrushStyle GetStyle() const = 0;

    virtual bool IsHatch() const
        { return (GetStyle()>=wxBRUSHSTYLE_FIRST_HATCH) && (GetStyle()<=wxBRUSHSTYLE_LAST_HATCH); }
};

#if defined(__WXPALMOS__)
    #include "wx/palmos/brush.h"
#elif defined(__WXMSW__)
    #include "wx/msw/brush.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
    #include "wx/x11/brush.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/brush.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/brush.h"
#elif defined(__WXMGL__)
    #include "wx/mgl/brush.h"
#elif defined(__WXDFB__)
    #include "wx/dfb/brush.h"
#elif defined(__WXMAC__)
    #include "wx/mac/brush.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/brush.h"
#elif defined(__WXPM__)
    #include "wx/os2/brush.h"
#endif

class WXDLLIMPEXP_CORE wxBrushList: public wxGDIObjListBase
{
public:
    wxBrush *FindOrCreateBrush(const wxColour& colour, wxBrushStyle style = wxBRUSHSTYLE_SOLID);
#if WXWIN_COMPATIBILITY_2_6
    wxDEPRECATED( void AddBrush(wxBrush*) );
    wxDEPRECATED( void RemoveBrush(wxBrush*) );
#endif
};

extern WXDLLEXPORT_DATA(wxBrushList*)   wxTheBrushList;

#endif
    // _WX_BRUSH_H_BASE_
