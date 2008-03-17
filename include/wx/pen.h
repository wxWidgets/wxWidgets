/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pen.h
// Purpose:     Base header for wxPen
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_BASE_
#define _WX_PEN_H_BASE_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

#if WXWIN_COMPATIBILITY_2_8
#include "wx/brush.h"       // needed for some deprecated declarations
#endif

enum wxPenStyle
{
#if WXWIN_COMPATIBILITY_2_8
    /* start of deprecated values */
    /* wxSOLID, wxTRANSPARENT, wxSTIPPLE are already defined in wxBrushStyle */
    wxDOT = 101,
    wxLONG_DASH = 102,
    wxSHORT_DASH = 103,
    wxDOT_DASH = 104,
    wxUSER_DASH = 105,
    /* end of deprecated values */

    wxPENSTYLE_SOLID = wxSOLID,
    wxPENSTYLE_DOT = wxDOT,
    wxPENSTYLE_LONG_DASH = wxLONG_DASH,
    wxPENSTYLE_SHORT_DASH = wxSHORT_DASH,
    wxPENSTYLE_DOT_DASH = wxDOT_DASH,
    wxPENSTYLE_USER_DASH = wxUSER_DASH,

    wxPENSTYLE_TRANSPARENT = wxTRANSPARENT,

    wxPENSTYLE_STIPPLE_MASK_OPAQUE = wxSTIPPLE_MASK_OPAQUE,
    wxPENSTYLE_STIPPLE_MASK = wxSTIPPLE_MASK,
    wxPENSTYLE_STIPPLE = wxSTIPPLE,

    wxPENSTYLE_BDIAGONAL_HATCH = wxBDIAGONAL_HATCH,
    wxPENSTYLE_CROSSDIAG_HATCH = wxCROSSDIAG_HATCH,
    wxPENSTYLE_FDIAGONAL_HATCH = wxFDIAGONAL_HATCH,
    wxPENSTYLE_CROSS_HATCH = wxCROSS_HATCH,
    wxPENSTYLE_HORIZONTAL_HATCH = wxHORIZONTAL_HATCH,
    wxPENSTYLE_VERTICAL_HATCH = wxVERTICAL_HATCH,

    wxPENSTYLE_FIRST_HATCH = wxFIRST_HATCH,
    wxPENSTYLE_LAST_HATCH = wxLAST_HATCH,

    wxPENSTYLE_MAX
#else
    wxPENSTYLE_SOLID,
    wxPENSTYLE_DOT,
    wxPENSTYLE_LONG_DASH,
    wxPENSTYLE_SHORT_DASH,
    wxPENSTYLE_DOT_DASH,
    wxPENSTYLE_USER_DASH,

    wxPENSTYLE_TRANSPARENT,

    /*  Pen Stippling. */
    wxPENSTYLE_STIPPLE_MASK_OPAQUE,
        /* mask is used for blitting monochrome using text fore and back ground colors */

    wxPENSTYLE_STIPPLE_MASK,
        /* mask is used for masking areas in the stipple bitmap (TO DO) */

    wxPENSTYLE_STIPPLE,
        /*  drawn with a Pen, and without any Brush -- and it can be stippled. */

    /* In wxWidgets < 2.6 use WX_HATCH macro  */
    /* to verify these wx*_HATCH are in style */
    /* of wxBrush. In wxWidgets >= 2.6 use    */
    /* wxBrush::IsHatch() instead.            */
    wxPENSTYLE_BDIAGONAL_HATCH,
    wxPENSTYLE_CROSSDIAG_HATCH,
    wxPENSTYLE_FDIAGONAL_HATCH,
    wxPENSTYLE_CROSS_HATCH,
    wxPENSTYLE_HORIZONTAL_HATCH,
    wxPENSTYLE_VERTICAL_HATCH,

    wxPENSTYLE_FIRST_HATCH = wxPENSTYLE_BDIAGONAL_HATCH,
    wxPENSTYLE_LAST_HATCH = wxPENSTYLE_VERTICAL_HATCH,

    wxPENSTYLE_MAX
#endif
};

enum wxPenJoin
{
    wxJOIN_INVALID = -1,

    wxJOIN_BEVEL = 120,
    wxJOIN_MITER,
    wxJOIN_ROUND,
};

enum wxPenCap
{
    wxCAP_INVALID = -1,

    wxCAP_ROUND = 130,
    wxCAP_PROJECTING,
    wxCAP_BUTT
};


class WXDLLEXPORT wxPenBase : public wxGDIObject
{
public:
    virtual ~wxPenBase() { }

    virtual void SetColour(const wxColour& col) = 0;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) = 0;

    virtual void SetWidth(int width) = 0;
    virtual void SetStyle(wxPenStyle style) = 0;
    virtual void SetStipple(const wxBitmap& stipple) = 0;
    virtual void SetDashes(int nb_dashes, const wxDash *dash) = 0;
    virtual void SetJoin(wxPenJoin join) = 0;
    virtual void SetCap(wxPenCap cap) = 0;

    virtual wxColour& GetColour() const = 0;
    virtual wxBitmap *GetStipple() const = 0;
    virtual wxPenStyle GetStyle() const = 0;
    virtual wxPenJoin GetJoin() const = 0;
    virtual wxPenCap GetCap() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetDashes(wxDash **ptr) const = 0;

#if WXWIN_COMPATIBILITY_2_8
    void SetStyle(wxBrushStyle style)
        { SetStyle((wxPenStyle)style); }
#endif
};

#if defined(__WXPALMOS__)
#include "wx/palmos/pen.h"
#elif defined(__WXMSW__)
#include "wx/msw/pen.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
#include "wx/x11/pen.h"
#elif defined(__WXGTK20__)
#include "wx/gtk/pen.h"
#elif defined(__WXGTK__)
#include "wx/gtk1/pen.h"
#elif defined(__WXMGL__)
#include "wx/mgl/pen.h"
#elif defined(__WXDFB__)
#include "wx/dfb/pen.h"
#elif defined(__WXMAC__)
#include "wx/mac/pen.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/pen.h"
#elif defined(__WXPM__)
#include "wx/os2/pen.h"
#endif

class WXDLLIMPEXP_CORE wxPenList: public wxGDIObjListBase
{
public:
    wxPen *FindOrCreatePen(const wxColour& colour, int width, wxPenStyle style);
#if WXWIN_COMPATIBILITY_2_6
    wxDEPRECATED( void AddPen(wxPen*) );
    wxDEPRECATED( void RemovePen(wxPen*) );
#endif
};

extern WXDLLEXPORT_DATA(wxPenList*)   wxThePenList;

#endif
    // _WX_PEN_H_BASE_
