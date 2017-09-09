/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pen.h
// Purpose:     Base header for wxPen
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_BASE_
#define _WX_PEN_H_BASE_

#include "wx/bitmap.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

enum wxPenStyle
{
    wxPENSTYLE_INVALID = -1,

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

    wxPENSTYLE_BDIAGONAL_HATCH = wxHATCHSTYLE_BDIAGONAL,
    wxPENSTYLE_CROSSDIAG_HATCH = wxHATCHSTYLE_CROSSDIAG,
    wxPENSTYLE_FDIAGONAL_HATCH = wxHATCHSTYLE_FDIAGONAL,
    wxPENSTYLE_CROSS_HATCH = wxHATCHSTYLE_CROSS,
    wxPENSTYLE_HORIZONTAL_HATCH = wxHATCHSTYLE_HORIZONTAL,
    wxPENSTYLE_VERTICAL_HATCH = wxHATCHSTYLE_VERTICAL,
    wxPENSTYLE_FIRST_HATCH = wxHATCHSTYLE_FIRST,
    wxPENSTYLE_LAST_HATCH = wxHATCHSTYLE_LAST
};

enum wxPenJoin
{
    wxJOIN_INVALID = -1,

    wxJOIN_BEVEL = 120,
    wxJOIN_MITER,
    wxJOIN_ROUND
};

enum wxPenCap
{
    wxCAP_INVALID = -1,

    wxCAP_ROUND = 130,
    wxCAP_PROJECTING,
    wxCAP_BUTT
};

// ----------------------------------------------------------------------------
// wxPenInfoBase is a common base for wxPenInfo and wxGraphicsPenInfo
// ----------------------------------------------------------------------------

// This class uses CRTP, the template parameter is the derived class itself.
template <class T>
class wxPenInfoBase
{
public:
    wxPenInfoBase(const wxColour& colour, wxPenStyle style)
    {
        m_nb_dashes = 0;
        m_dash = NULL;
        m_join = wxJOIN_ROUND;
        m_cap = wxCAP_ROUND;

        m_colour = colour;
        m_style = style;
    }

    // Setters for the various attributes. All of them return the object itself
    // so that the calls to them could be chained.

    T& Colour(const wxColour& colour)
        { m_colour = colour; return This(); }

    T& Style(wxPenStyle style)
        { m_style = style; return This(); }
    T& Stipple(const wxBitmap& stipple)
        { m_stipple = stipple; m_style = wxPENSTYLE_STIPPLE; return This(); }
    T& Dashes(int nb_dashes, const wxDash *dash)
        { m_nb_dashes = nb_dashes; m_dash = (wxDash *)dash; return This(); }
    T& Join(wxPenJoin join)
        { m_join = join; return This(); }
    T& Cap(wxPenCap cap)
        { m_cap = cap; return This(); }

    // Accessors are mostly meant to be used by wxWidgets itself.

    wxColour GetColour() const { return m_colour; }
    wxBitmap GetStipple() const { return m_stipple; }
    wxPenStyle GetStyle() const { return m_style; }
    wxPenJoin GetJoin() const { return m_join; }
    wxPenCap GetCap() const { return m_cap; }
    int GetDashes(wxDash **ptr) const { *ptr = m_dash; return m_nb_dashes; }

    int GetDashCount() const { return m_nb_dashes; }
    wxDash* GetDash() const { return m_dash; }

    // Convenience

    bool IsTransparent() const { return m_style == wxPENSTYLE_TRANSPARENT; }

private:
    // Helper to return this object itself cast to its real type T.
    T& This() { return static_cast<T&>(*this); }

    wxColour m_colour;
    wxBitmap m_stipple;
    wxPenStyle m_style;
    wxPenJoin m_join;
    wxPenCap m_cap;

    int m_nb_dashes;
    wxDash* m_dash;
};

// ----------------------------------------------------------------------------
// wxPenInfo contains all parameters describing a wxPen
// ----------------------------------------------------------------------------

class wxPenInfo : public wxPenInfoBase<wxPenInfo>
{
public:
    explicit wxPenInfo(const wxColour& colour = wxColour(),
                       int width = 1,
                       wxPenStyle style = wxPENSTYLE_SOLID)
        : wxPenInfoBase(colour, style)
    {
        m_width = width;
    }

    // Setters

    wxPenInfo& Width(int width)
        { m_width = width; return *this; }

    // Accessors

    int GetWidth() const { return m_width; }

private:
    int m_width;
};


class WXDLLIMPEXP_CORE wxPenBase : public wxGDIObject
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

    virtual wxColour GetColour() const = 0;
    virtual wxBitmap *GetStipple() const = 0;
    virtual wxPenStyle GetStyle() const = 0;
    virtual wxPenJoin GetJoin() const = 0;
    virtual wxPenCap GetCap() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetDashes(wxDash **ptr) const = 0;

    // Convenient helpers for testing whether the pen is a transparent one:
    // unlike GetStyle() == wxPENSTYLE_TRANSPARENT, they work correctly even if
    // the pen is invalid (they both return false in this case).
    bool IsTransparent() const
    {
        return IsOk() && GetStyle() == wxPENSTYLE_TRANSPARENT;
    }

    bool IsNonTransparent() const
    {
        return IsOk() && GetStyle() != wxPENSTYLE_TRANSPARENT;
    }
};

#if defined(__WXMSW__)
    #include "wx/msw/pen.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
    #include "wx/x11/pen.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/pen.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/pen.h"
#elif defined(__WXDFB__)
    #include "wx/dfb/pen.h"
#elif defined(__WXMAC__)
    #include "wx/osx/pen.h"
#elif defined(__WXQT__)
    #include "wx/qt/pen.h"
#endif

class WXDLLIMPEXP_CORE wxPenList: public wxGDIObjListBase
{
public:
    wxPen *FindOrCreatePen(const wxColour& colour,
                           int width = 1,
                           wxPenStyle style = wxPENSTYLE_SOLID);

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen *FindOrCreatePen(const wxColour& colour, int width, int style)
        { return FindOrCreatePen(colour, width, (wxPenStyle)style); }
};

extern WXDLLIMPEXP_DATA_CORE(wxPenList*)   wxThePenList;

// provide comparison operators to allow code such as
//
//      if ( pen.GetStyle() == wxTRANSPARENT )
//
// to compile without warnings which it would otherwise provoke from some
// compilers as it compares elements of different enums

// Unfortunately some compilers have ambiguity issues when enum comparisons are
// overloaded so we have to disable the overloads in this case, see
// wxCOMPILER_NO_OVERLOAD_ON_ENUM definition in wx/platform.h for more details.
#ifndef wxCOMPILER_NO_OVERLOAD_ON_ENUM

wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
inline bool operator==(wxPenStyle s, wxDeprecatedGUIConstants t)
{
    return static_cast<int>(s) == static_cast<int>(t);
}

wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
inline bool operator!=(wxPenStyle s, wxDeprecatedGUIConstants t)
{
    return static_cast<int>(s) != static_cast<int>(t);
}

#endif // wxCOMPILER_NO_OVERLOAD_ON_ENUM

#endif // _WX_PEN_H_BASE_
