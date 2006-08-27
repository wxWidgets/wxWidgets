/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/pen.cpp
// Purpose:     wxPen class implementation
// Author:      Vaclav Slavik
// Created:     2006-08-04
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/pen.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/colour.h"
#endif

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxObjectRefData
{
public:
    wxPenRefData(const wxColour& clr = wxNullColour, int style = wxSOLID)
    {
        m_colour = clr;
        SetStyle(style);
    }

    wxPenRefData(const wxPenRefData& data)
        : m_style(data.m_style), m_colour(data.m_colour) {}

    void SetStyle(int style)
    {
        if ( m_style != wxSOLID && m_style == wxTRANSPARENT )
        {
            wxFAIL_MSG( _T("only wxSOLID and wxTRANSPARENT styles are supported") );
            style = wxSOLID;
        }

        m_style = style;
    }

    int            m_style;
    wxColour       m_colour;
};

//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)

wxPen::wxPen(const wxColour &colour, int width, int style)
{
    wxASSERT_MSG( width <= 1, _T("only width=0,1 are supported") );

    m_refData = new wxPenRefData(colour, style);
}

wxPen::wxPen(const wxBitmap& WXUNUSED(stipple), int WXUNUSED(width))
{
    wxFAIL_MSG( _T("stipple pens not supported") );

    m_refData = new wxPenRefData();
}

bool wxPen::operator==(const wxPen& pen) const
{
#warning "this is incorrect (MGL too)"
    return m_refData == pen.m_refData;
}

void wxPen::SetColour(const wxColour &colour)
{
    AllocExclusive();
    M_PENDATA->m_colour = colour;
}

void wxPen::SetDashes(int WXUNUSED(number_of_dashes), const wxDash *WXUNUSED(dash))
{
    wxFAIL_MSG( _T("SetDashes not implemented") );
}

void wxPen::SetColour(unsigned char red, unsigned char green, unsigned char blue)
{
    AllocExclusive();
    M_PENDATA->m_colour.Set(red, green, blue);
}

void wxPen::SetCap(int WXUNUSED(capStyle))
{
    wxFAIL_MSG( _T("SetCap not implemented") );
}

void wxPen::SetJoin(int WXUNUSED(joinStyle))
{
    wxFAIL_MSG( _T("SetJoin not implemented") );
}

void wxPen::SetStyle(int style)
{
    AllocExclusive();
    M_PENDATA->SetStyle(style);
}

void wxPen::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( _T("SetStipple not implemented") );
}

void wxPen::SetWidth(int width)
{
    wxASSERT_MSG( width <= 1, _T("only width=0,1 are implemented") );
}

int wxPen::GetDashes(wxDash **ptr) const
{
    wxFAIL_MSG( _T("GetDashes not implemented") );

    *ptr = NULL;
    return 0;
}

int wxPen::GetDashCount() const
{
    wxFAIL_MSG( _T("GetDashCount not implemented") );

    return 0;
}

wxDash* wxPen::GetDash() const
{
    wxFAIL_MSG( _T("GetDash not implemented") );

    return NULL;
}

int wxPen::GetCap() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    wxFAIL_MSG( _T("GetCap not implemented") );
    return -1;
}

int wxPen::GetJoin() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    wxFAIL_MSG( _T("GetJoin not implemented") );
    return -1;
}

int wxPen::GetStyle() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_style;
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return 1;
}

wxColour &wxPen::GetColour() const
{
    wxCHECK_MSG( Ok(), wxNullColour, wxT("invalid pen") );

    return M_PENDATA->m_colour;
}

wxBitmap *wxPen::GetStipple() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid pen") );

    wxFAIL_MSG( _T("GetStipple not implemented") );
    return NULL;
}

bool wxPen::Ok() const
{
    return ((m_refData) && M_PENDATA->m_colour.Ok());
}

wxObjectRefData *wxPen::CreateRefData() const
{
    return new wxPenRefData;
}

wxObjectRefData *wxPen::CloneRefData(const wxObjectRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
