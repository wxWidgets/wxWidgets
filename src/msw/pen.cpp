/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/pen.cpp
// Purpose:     wxPen
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: refactored wxPen code to wxPenRefData
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/pen.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/utils.h"
#endif

#include "wx/msw/private.h"

#define M_PENDATA ((wxPenRefData*)m_refData)

// ----------------------------------------------------------------------------
// wxPenRefData: contains information about an HPEN and its handle
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPenRefData : public wxGDIRefData
{
public:
    // ctors and dtor
    // --------------

    wxPenRefData();
    wxPenRefData(const wxPenRefData& data);
    wxPenRefData(const wxPenInfo& info);
    virtual ~wxPenRefData();

    bool operator==(const wxPenRefData& data) const
    {
        // we intentionally don't compare m_hPen fields here
        return m_style == data.m_style &&
               m_width == data.m_width &&
               m_join == data.m_join &&
               m_cap == data.m_cap &&
               m_colour == data.m_colour &&
               (m_style != wxPENSTYLE_STIPPLE || m_stipple.IsSameAs(data.m_stipple)) &&
               (m_style != wxPENSTYLE_USER_DASH ||
                (m_nbDash == data.m_nbDash &&
                    memcmp(m_dash, data.m_dash, m_nbDash*sizeof(wxDash)) == 0));
    }


    // accessors and setters
    // ---------------------

    wxColour& GetColour() const { return const_cast<wxColour&>(m_colour); }
    int GetWidth() const { return m_width; }
    wxPenStyle GetStyle() const { return m_style; }
    wxPenJoin GetJoin() const { return m_join; }
    wxPenCap GetCap() const { return m_cap; }
    wxDash* GetDash() const { return m_dash; }
    int GetDashCount() const { return m_nbDash; }
    wxBitmap* GetStipple() const { return const_cast<wxBitmap *>(&m_stipple); }

    void SetColour(const wxColour& col) { Free(); m_colour = col; }
    void SetWidth(int width) { Free(); m_width = width; }
    void SetStyle(wxPenStyle style) { Free(); m_style = style; }
    void SetStipple(const wxBitmap& stipple)
    {
        Free();

        m_style = wxPENSTYLE_STIPPLE;
        m_stipple = stipple;
    }

    void SetDashes(int nb_dashes, const wxDash *dash)
    {
        Free();

        m_nbDash = nb_dashes;
        m_dash = const_cast<wxDash *>(dash);
    }

    void SetJoin(wxPenJoin join) { Free(); m_join = join; }
    void SetCap(wxPenCap cap) { Free(); m_cap = cap; }


    // HPEN management
    // ---------------

    // create the HPEN if we don't have it yet
    bool Alloc();

    // get the HPEN creating it on demand
    WXHPEN GetHPEN() const;

    // return true if we have a valid HPEN
    bool HasHPEN() const { return m_hPen != 0; }

    // return true if we had a valid handle before, false otherwise
    bool Free();

private:
    // initialize the fields which have reasonable default values
    //
    // doesn't initialize m_width and m_style which must be initialize in ctor
    void Init()
    {
        m_join = wxJOIN_ROUND;
        m_cap = wxCAP_ROUND;
        m_nbDash = 0;
        m_dash = NULL;
        m_hPen = 0;
    }

    int           m_width;
    wxPenStyle    m_style;
    wxPenJoin     m_join;
    wxPenCap      m_cap;
    wxBitmap      m_stipple;
    int           m_nbDash;
    wxDash *      m_dash;
    wxColour      m_colour;
    HPEN          m_hPen;

    wxDECLARE_NO_ASSIGN_CLASS(wxPenRefData);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPenRefData ctors/dtor
// ----------------------------------------------------------------------------

wxPenRefData::wxPenRefData()
{
    Init();

    m_style = wxPENSTYLE_SOLID;
    m_width = 1;
}

wxPenRefData::wxPenRefData(const wxPenRefData& data)
             :wxGDIRefData()
    , m_colour(data.m_colour)
{
    m_style = data.m_style;
    m_width = data.m_width;
    m_join = data.m_join;
    m_cap = data.m_cap;
    m_nbDash = data.m_nbDash;
    m_dash = data.m_dash;
    m_hPen = 0;
}

wxPenRefData::wxPenRefData(const wxPenInfo& info)
    : m_stipple(info.GetStipple())
    , m_colour(info.GetColour())
{
    Init();

    m_style = info.GetStyle();
    m_width = info.GetWidth();
    m_join = info.GetJoin();
    m_cap = info.GetCap();
    m_nbDash = info.GetDashes(&m_dash);
}

wxPenRefData::~wxPenRefData()
{
    if ( m_hPen )
        ::DeleteObject(m_hPen);
}

// ----------------------------------------------------------------------------
// wxPenRefData HPEN management
// ----------------------------------------------------------------------------

static int ConvertPenStyle(wxPenStyle style)
{
    switch ( style )
    {
        case wxPENSTYLE_SHORT_DASH:
        case wxPENSTYLE_LONG_DASH:
            return PS_DASH;

        case wxPENSTYLE_TRANSPARENT:
            return PS_NULL;

        default:
            wxFAIL_MSG( wxT("unknown pen style") );
            wxFALLTHROUGH;

        case wxPENSTYLE_DOT:
            return PS_DOT;

        case wxPENSTYLE_DOT_DASH:
            return PS_DASHDOT;

        case wxPENSTYLE_USER_DASH:
            return PS_USERSTYLE;

        case wxPENSTYLE_STIPPLE:
        case wxPENSTYLE_BDIAGONAL_HATCH:
        case wxPENSTYLE_CROSSDIAG_HATCH:
        case wxPENSTYLE_FDIAGONAL_HATCH:
        case wxPENSTYLE_CROSS_HATCH:
        case wxPENSTYLE_HORIZONTAL_HATCH:
        case wxPENSTYLE_VERTICAL_HATCH:
        case wxPENSTYLE_SOLID:

            return PS_SOLID;
    }
}

static int ConvertJoinStyle(wxPenJoin join)
{
    switch( join )
    {
        case wxJOIN_BEVEL:
            return PS_JOIN_BEVEL;

        case wxJOIN_MITER:
            return PS_JOIN_MITER;

        default:
            wxFAIL_MSG( wxT("unknown pen join style") );
            wxFALLTHROUGH;

        case wxJOIN_ROUND:
            return PS_JOIN_ROUND;
    }
}

static int ConvertCapStyle(wxPenCap cap)
{
    switch ( cap )
    {
        case wxCAP_PROJECTING:
            return PS_ENDCAP_SQUARE;

        case wxCAP_BUTT:
            return PS_ENDCAP_FLAT;

        default:
            wxFAIL_MSG( wxT("unknown pen cap style") );
            wxFALLTHROUGH;

        case wxCAP_ROUND:
            return PS_ENDCAP_ROUND;
    }
}

bool wxPenRefData::Alloc()
{
   if ( m_hPen )
       return false;

   if ( m_style == wxPENSTYLE_TRANSPARENT )
   {
       m_hPen = (HPEN)::GetStockObject(NULL_PEN);
       return true;
   }

   const COLORREF col = m_colour.GetPixel();

   // check if it's a standard kind of pen which can be created with just
   // CreatePen()
   if ( m_join == wxJOIN_ROUND &&
            m_cap == wxCAP_ROUND &&
                m_style != wxPENSTYLE_USER_DASH &&
                    m_style != wxPENSTYLE_STIPPLE &&
                        (m_width <= 1 || m_style == wxPENSTYLE_SOLID) )
   {
       m_hPen = ::CreatePen(ConvertPenStyle(m_style), m_width, col);
   }
   else // need to use ExtCreatePen()
   {
       DWORD styleMSW = PS_GEOMETRIC |
                        ConvertPenStyle(m_style) |
                        ConvertJoinStyle(m_join) |
                        ConvertCapStyle(m_cap);

       LOGBRUSH lb;
       switch( m_style )
       {
           case wxPENSTYLE_STIPPLE:
               lb.lbStyle = BS_PATTERN;
               lb.lbHatch = wxPtrToUInt(m_stipple.GetHBITMAP());
               break;

           case wxPENSTYLE_BDIAGONAL_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_BDIAGONAL;
               break;

           case wxPENSTYLE_CROSSDIAG_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_DIAGCROSS;
               break;

           case wxPENSTYLE_FDIAGONAL_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_FDIAGONAL;
               break;

           case wxPENSTYLE_CROSS_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_CROSS;
               break;

           case wxPENSTYLE_HORIZONTAL_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_HORIZONTAL;
               break;

           case wxPENSTYLE_VERTICAL_HATCH:
               lb.lbStyle = BS_HATCHED;
               lb.lbHatch = HS_VERTICAL;
               break;

           default:
               lb.lbStyle = BS_SOLID;
               // this should be unnecessary (it's unused) but suppresses the
               // Purify messages about uninitialized memory read
               lb.lbHatch = 0;
               break;
       }

       lb.lbColor = col;

       DWORD *dash;
       if ( m_style == wxPENSTYLE_USER_DASH && m_nbDash && m_dash )
       {
           dash = new DWORD[m_nbDash];
           int rw = m_width > 1 ? m_width : 1;
           for ( int i = 0; i < m_nbDash; i++ )
               dash[i] = m_dash[i] * rw;
       }
       else
       {
           dash = NULL;
       }

       m_hPen = ::ExtCreatePen(styleMSW, m_width, &lb, m_nbDash, (LPDWORD)dash);

       delete [] dash;
   }

   return m_hPen != 0;
}

bool wxPenRefData::Free()
{
    if ( !m_hPen )
        return false;

    ::DeleteObject(m_hPen);
    m_hPen = 0;

    return true;
}

WXHPEN wxPenRefData::GetHPEN() const
{
    if ( !m_hPen )
        const_cast<wxPenRefData *>(this)->Alloc();

    return (WXHPEN)m_hPen;
}

// ----------------------------------------------------------------------------
// wxPen
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject);

wxPen::wxPen(const wxColour& col, int width, wxPenStyle style)
{
    m_refData = new wxPenRefData(wxPenInfo(col, width).Style(style));
}

wxPen::wxPen(const wxColour& colour, int width, int style)
{
    m_refData = new wxPenRefData
                    (
                        wxPenInfo(colour, width).Style((wxPenStyle)style)
                    );
}

wxPen::wxPen(const wxBitmap& stipple, int width)
{
    m_refData = new wxPenRefData(wxPenInfo().Stipple(stipple).Width(width));
}

wxPen::wxPen(const wxPenInfo& info)
{
    m_refData = new wxPenRefData(info);
}

bool wxPen::operator==(const wxPen& pen) const
{
    const wxPenRefData *
        penData = static_cast<const wxPenRefData *>(pen.m_refData);

    // an invalid pen is only equal to another invalid pen
    return m_refData ? penData && *M_PENDATA == *penData : !penData;
}

bool wxPen::RealizeResource()
{
    return M_PENDATA && M_PENDATA->Alloc();
}

WXHANDLE wxPen::GetResourceHandle() const
{
    return M_PENDATA ? M_PENDATA->GetHPEN() : 0;
}

bool wxPen::FreeResource(bool WXUNUSED(force))
{
    return M_PENDATA && M_PENDATA->Free();
}

bool wxPen::IsFree() const
{
    return M_PENDATA && !M_PENDATA->HasHPEN();
}

wxGDIRefData* wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData* wxPen::CloneGDIRefData(const wxGDIRefData* data) const
{
    return new wxPenRefData(*static_cast<const wxPenRefData*>(data));
}

void wxPen::SetColour(const wxColour& col)
{
    AllocExclusive();

    M_PENDATA->SetColour(col);
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    SetColour(wxColour(r, g, b));
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();

    M_PENDATA->SetWidth(width);
}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();

    M_PENDATA->SetStyle(style);
}

void wxPen::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();

    M_PENDATA->SetStipple(stipple);
}

void wxPen::SetDashes(int nb_dashes, const wxDash *dash)
{
    AllocExclusive();

    M_PENDATA->SetDashes(nb_dashes, dash);
}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();

    M_PENDATA->SetJoin(join);
}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();

    M_PENDATA->SetCap(cap);
}

wxColour wxPen::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid pen") );

    return M_PENDATA->GetColour();
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid pen") );

    return M_PENDATA->GetWidth();
}

wxPenStyle wxPen::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxPENSTYLE_INVALID, wxT("invalid pen") );

    return M_PENDATA->GetStyle();
}

wxPenJoin wxPen::GetJoin() const
{
    wxCHECK_MSG( IsOk(), wxJOIN_INVALID, wxT("invalid pen") );

    return M_PENDATA->GetJoin();
}

wxPenCap wxPen::GetCap() const
{
    wxCHECK_MSG( IsOk(), wxCAP_INVALID, wxT("invalid pen") );

    return M_PENDATA->GetCap();
}

int wxPen::GetDashes(wxDash** ptr) const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid pen") );

    *ptr = M_PENDATA->GetDash();
    return M_PENDATA->GetDashCount();
}

wxDash* wxPen::GetDash() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid pen") );

    return m_refData ? M_PENDATA->GetDash() : NULL;
}

int wxPen::GetDashCount() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid pen") );

    return m_refData ? M_PENDATA->GetDashCount() : 0;
}

wxBitmap* wxPen::GetStipple() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid pen") );

    return m_refData ? M_PENDATA->GetStipple() : NULL;
}
