/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "pen.h"
#endif

#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/mgl/private.h"

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxObjectRefData
{
    public:
        wxPenRefData();
        wxPenRefData(const wxPenRefData& data);

        int            m_width;
        int            m_style;
        wxColour       m_colour;
        wxBitmap       m_stipple;
        pixpattern24_t m_pixPattern;

        // not used by wxMGL, but we want to preserve values
        int            m_joinStyle;
        int            m_capStyle;
        int            m_countDashes;
        wxDash        *m_dash;
};

wxPenRefData::wxPenRefData()
{
    m_width = 1;
    m_style = wxSOLID;
    m_joinStyle = wxJOIN_ROUND;
    m_capStyle = wxCAP_ROUND;
    m_dash = (wxDash*) NULL;
    m_countDashes = 0;

    int x, y, c;
    for (y = 0; y < 8; y++)
        for (x = 0; x < 8; x++)
            for (c = 0; c < 3; c++)
                m_pixPattern.p[x][y][c] = 0;
}

wxPenRefData::wxPenRefData(const wxPenRefData& data)
{
    m_style = data.m_style;
    m_width = data.m_width;
    m_joinStyle = data.m_joinStyle;
    m_capStyle = data.m_capStyle;
    m_colour = data.m_colour;
    m_countDashes = data.m_countDashes;
    m_dash = data.m_dash;
    m_stipple = data.m_stipple;

    int x, y, c;
    for (y = 0; y < 8; y++)
        for (x = 0; x < 8; x++)
            for (c = 0; c < 3; c++)
                m_pixPattern.p[x][y][c] = data.m_pixPattern.p[x][y][c];
}

//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject)

wxPen::wxPen()
{
    if ( wxThePenList ) 
        wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxColour &colour, int width, int style)
{
    m_refData = new wxPenRefData();
    M_PENDATA->m_width = width;
    M_PENDATA->m_style = style;
    M_PENDATA->m_colour = colour;

    if ( wxThePenList )
        wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxBitmap& stipple, int width)
{
    wxCHECK_RET( stipple.Ok(), _T("invalid bitmap") );
    wxCHECK_RET( stipple.GetWidth() == 8 && stipple.GetHeight() == 8, 
                  _T("stipple bitmap must be 8x8") );

    m_refData = new wxPenRefData();
    M_PENDATA->m_width = width;
    M_PENDATA->m_style = wxSTIPPLE;
    M_PENDATA->m_stipple = stipple;
    wxBitmapToPixPattern(stipple, &(M_PENDATA->m_pixPattern), NULL);

    if ( wxThePenList ) 
        wxThePenList->AddPen(this);
}

wxPen::wxPen(const wxPen& pen)
{
    Ref(pen);
    if ( wxThePenList )
        wxThePenList->AddPen(this);
}

wxPen::~wxPen()
{
    if ( wxThePenList )
        wxThePenList->RemovePen(this);
}

wxPen& wxPen::operator = (const wxPen& pen)
{
    if (*this == pen) return (*this);
    Ref(pen);
    return *this;
}

bool wxPen::operator == (const wxPen& pen) const
{
    return m_refData == pen.m_refData;
}

bool wxPen::operator != (const wxPen& pen) const
{
    return m_refData != pen.m_refData;
}

void wxPen::SetColour(const wxColour &colour)
{
    Unshare();
    M_PENDATA->m_colour = colour;
}

void wxPen::SetDashes(int number_of_dashes, const wxDash *dash)
{
    Unshare();
    M_PENDATA->m_countDashes = number_of_dashes;
    M_PENDATA->m_dash = (wxDash *)dash; /* TODO */
}

void wxPen::SetColour(int red, int green, int blue)
{
    Unshare();
    M_PENDATA->m_colour.Set(red, green, blue);
}

void wxPen::SetCap(int capStyle)
{
    Unshare();
    M_PENDATA->m_capStyle = capStyle;
}

void wxPen::SetJoin(int joinStyle)
{
    Unshare();
    M_PENDATA->m_joinStyle = joinStyle;
}

void wxPen::SetStyle(int style)
{
    Unshare();
    M_PENDATA->m_style = style;
}

void wxPen::SetStipple(const wxBitmap& stipple)
{
    wxCHECK_RET( stipple.Ok(), _T("invalid bitmap") );
    wxCHECK_RET( stipple.GetWidth() == 8 && stipple.GetHeight() == 8, 
                  _T("stipple bitmap must be 8x8") );

    Unshare();
    M_PENDATA->m_stipple = stipple;
    wxBitmapToPixPattern(stipple, &(M_PENDATA->m_pixPattern), NULL);
}

void wxPen::SetWidth(int width)
{
    Unshare();
    M_PENDATA->m_width = width;
}

int wxPen::GetDashes(wxDash **ptr) const 
{
     *ptr = (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*) NULL); 
     return (M_PENDATA ? M_PENDATA->m_countDashes : 0);
}

int wxPen::GetDashCount() const 
{ 
    return (M_PENDATA->m_countDashes); 
}

wxDash* wxPen::GetDash() const 
{ 
    return (wxDash*)M_PENDATA->m_dash; 
}

int wxPen::GetCap() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_capStyle;
}

int wxPen::GetJoin() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_joinStyle;
}

int wxPen::GetStyle() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_style;
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_width;
}

wxColour &wxPen::GetColour() const
{
    wxCHECK_MSG( Ok(), wxNullColour, wxT("invalid pen") );

    return M_PENDATA->m_colour;
}

wxBitmap *wxPen::GetStipple() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid pen") );

    return &(M_PENDATA->m_stipple);
}

void* wxPen::GetPixPattern() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid pen") );

    return (void*)&(M_PENDATA->m_pixPattern);
}


bool wxPen::Ok() const
{
    return (m_refData != NULL);
}

void wxPen::Unshare()
{
    if (!m_refData)
    {
        m_refData = new wxPenRefData();
    }
    else
    {
        wxPenRefData* ref = new wxPenRefData( *(wxPenRefData*)m_refData );
        UnRef();
        m_refData = ref;
    }
}

