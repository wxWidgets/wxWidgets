/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxPen;
class WXDLLIMPEXP_FWD_CORE wxColour;
class WXDLLIMPEXP_FWD_CORE wxBitmap;

typedef char wxX11Dash;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPen: public wxPenBase
{
public:
    wxPen() = default;

    wxPen( const wxColour &colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID );

    wxPen( const wxBitmap &stipple, int width );

    wxPen( const wxPenInfo& info );

    bool operator == ( const wxPen& pen ) const;
    bool operator != (const wxPen& pen) const { return !(*this == pen); }

    void SetColour( const wxColour &colour ) override;
    void SetColour( unsigned char red, unsigned char green, unsigned char blue ) override;
    void SetCap( wxPenCap capStyle ) override;
    void SetJoin( wxPenJoin joinStyle ) override;
    void SetStyle( wxPenStyle style ) override;
    void SetWidth( int width ) override;
    void SetDashes( int number_of_dashes, const wxDash *dash ) override;
    void SetStipple( const wxBitmap& stipple ) override;

    wxColour GetColour() const override;
    wxPenCap GetCap() const override;
    wxPenJoin GetJoin() const override;
    wxPenStyle GetStyle() const override;
    int GetWidth() const override;
    int GetDashes(wxDash **ptr) const override;
    int GetDashCount() const;
    wxDash* GetDash() const;
    wxBitmap* GetStipple() const override;


    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);
    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_PEN_H_
