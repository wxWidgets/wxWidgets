/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/pen.mm
// Purpose:     wxPen
// Author:      David Elliott
// Modified by:
// Created:     2003/08/02
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"
#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/colour.h"

#import <AppKit/NSColor.h>

// ========================================================================
// wxPenRefData
// ========================================================================
class WXDLLEXPORT wxPenRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPen;
public:
    wxPenRefData(const wxColour& colour = wxNullColour,
        int width = 1, int style = wxSOLID,
        const wxBitmap& stipple = wxNullBitmap);
    wxPenRefData(const wxPenRefData& data);
    ~wxPenRefData() { Free(); }

    void SetWidth(int Width) { m_width = Width; }
    void SetStyle(int Style) { m_style = Style; }
    void SetJoin(int Join) { m_join = Join; }
    void SetCap(int Cap) { m_cap = Cap; }
    void SetColour(const wxColour& col) { Free(); m_colour = col; }
    void SetDashes(int nb_dashes, const wxDash *Dash)
    {
        m_nbDash = nb_dashes;
        m_dash = (wxDash *)Dash;
    }
    void SetStipple(const wxBitmap& Stipple)
    {
        Free();
        m_stipple = Stipple;
        m_style = wxSTIPPLE;
    }
    WX_NSColor GetNSColor();
protected:
    void Free();

    int             m_width;
    int             m_style;
    int             m_join;
    int             m_cap;
    wxColour        m_colour;
    int             m_nbDash;
    wxDash         *m_dash;
    wxBitmap        m_stipple;
    WX_NSColor      m_cocoaNSColor;
private:
    // Don't allow assignment
    wxPenRefData& operator=(const wxPenRefData& data);
};

#define M_PENDATA ((wxPenRefData *)m_refData)

inline wxPenRefData::wxPenRefData(const wxColour& colour,
        int width, int style, const wxBitmap& stipple)
{
    m_width = width;
    m_style = style;
    m_join = wxJOIN_ROUND;
    m_cap = wxCAP_ROUND;
    m_colour = colour;
    m_nbDash = 0;
    m_dash = 0;
    m_stipple = stipple;
    m_cocoaNSColor = nil;
}

inline wxPenRefData::wxPenRefData(const wxPenRefData& data)
{
    m_width = data.m_width;
    m_style = data.m_style;
    m_join = data.m_join;
    m_cap = data.m_cap;
    m_colour = data.m_colour;
    m_nbDash = data.m_nbDash;
    m_dash = data.m_dash;
    m_stipple = data.m_stipple;
    m_cocoaNSColor = [data.m_cocoaNSColor retain];
}

inline void wxPenRefData::Free()
{
    [m_cocoaNSColor release];
    m_cocoaNSColor = nil;
}

inline WX_NSColor wxPenRefData::GetNSColor()
{
    if(!m_cocoaNSColor)
    {
        switch( m_style )
        {
        case wxTRANSPARENT:
            m_cocoaNSColor = [[NSColor clearColor] retain];
            break;
        case wxSTIPPLE:
//  wxBitmap isn't implemented yet
//            m_cocoaNSColor = [[NSColor colorWithPatternImage: m_stipple.GetNSImage()] retain];
//            break;
        // The hatch brushes are going to be tricky
        case wxBDIAGONAL_HATCH:
        case wxCROSSDIAG_HATCH:
        case wxFDIAGONAL_HATCH:
        case wxCROSS_HATCH:
        case wxHORIZONTAL_HATCH:
        case wxVERTICAL_HATCH:
        default:
        // Dot/dashed pens use solid colors
        case wxDOT:
        case wxLONG_DASH:
        case wxSHORT_DASH:
        case wxDOT_DASH:
        case wxUSER_DASH:
        case wxSOLID:
            NSColor *colour_NSColor = m_colour.GetNSColor();
            if(!colour_NSColor)
                colour_NSColor = [NSColor clearColor];
            m_cocoaNSColor = [colour_NSColor copyWithZone:nil];
            break;
        }
    }
    return m_cocoaNSColor;
}

// ========================================================================
// wxPen
// ========================================================================
IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)

wxPen::wxPen()
{
}

wxPen::~wxPen()
{
}

// Should implement Create
wxPen::wxPen(const wxColour& colour, int width, int style)
{
    m_refData = new wxPenRefData(colour,width,style);
}

wxPen::wxPen(const wxBitmap& stipple, int width)
{
    m_refData = new wxPenRefData(wxNullColour,width,wxSTIPPLE,stipple);
}

wxObjectRefData *wxPen::CreateRefData() const
{
    return new wxPenRefData;
}

wxObjectRefData *wxPen::CloneRefData(const wxObjectRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}

void wxPen::SetWidth(int Width)
{
    AllocExclusive();
    M_PENDATA->SetWidth(Width);
}

void wxPen::SetStyle(int Style)
{
    AllocExclusive();
    M_PENDATA->SetStyle(Style);
}

void wxPen::SetJoin(int Join)
{
    AllocExclusive();
    M_PENDATA->SetJoin(Join);
}

void wxPen::SetCap(int Cap)
{
    AllocExclusive();
    M_PENDATA->SetCap(Cap);
}

void wxPen::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_PENDATA->SetColour(col);
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_PENDATA->SetColour(wxColour(r, g, b));
}

void wxPen::SetDashes(int nb_dashes, const wxDash *Dash)
{
    AllocExclusive();
    M_PENDATA->SetDashes(nb_dashes, Dash);
}

void wxPen::SetStipple(const wxBitmap& Stipple)
{
    AllocExclusive();
    M_PENDATA->SetStipple(Stipple);
}

wxColour& wxPen::GetColour() const
{
    return (M_PENDATA ? M_PENDATA->m_colour : wxNullColour); 
}

int wxPen::GetWidth() const
{
    return (M_PENDATA ? M_PENDATA->m_width : 0); 
}

int wxPen::GetStyle() const
{
    return (M_PENDATA ? M_PENDATA->m_style : 0); 
}

int wxPen::GetJoin() const
{
    return (M_PENDATA ? M_PENDATA->m_join : 0); 
}

int wxPen::GetCap() const
{
    return (M_PENDATA ? M_PENDATA->m_cap : 0); 
}

int wxPen::GetDashes(wxDash **ptr) const
{
    *ptr = (M_PENDATA ? M_PENDATA->m_dash : (wxDash*) NULL); return (M_PENDATA ? M_PENDATA->m_nbDash : 0);
}

wxBitmap *wxPen::GetStipple() const
{
    return (M_PENDATA ? (& M_PENDATA->m_stipple) : (wxBitmap*) NULL); 
}

WX_NSColor wxPen::GetNSColor()
{
    return (M_PENDATA ? M_PENDATA->GetNSColor() : nil);
}

