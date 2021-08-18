/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: fixed operator=(), ==(), !=()
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// Pen
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPen : public wxPenBase
{
public:
    wxPen() { }
    wxPen(const wxColour& col, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID);

    wxPen(const wxBitmap& stipple, int width);

    wxPen(const wxPenInfo& info);

    virtual ~wxPen() { }

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const { return !(*this == pen); }

    // Override in order to recreate the pen
    void SetColour(const wxColour& col) wxOVERRIDE;
    void SetColour(unsigned char r, unsigned char g, unsigned char b) wxOVERRIDE;

    void SetWidth(int width) wxOVERRIDE;
    void SetStyle(wxPenStyle style) wxOVERRIDE;
    void SetStipple(const wxBitmap& stipple) wxOVERRIDE;
    void SetDashes(int nb_dashes, const wxDash *dash) wxOVERRIDE;
    void SetJoin(wxPenJoin join) wxOVERRIDE;
    void SetCap(wxPenCap cap) wxOVERRIDE;
    void SetQuality(wxPenQuality quality) wxOVERRIDE;

    wxColour GetColour() const wxOVERRIDE;
    int GetWidth() const wxOVERRIDE;
    wxPenStyle GetStyle() const wxOVERRIDE;
    wxPenJoin GetJoin() const wxOVERRIDE;
    wxPenCap GetCap() const wxOVERRIDE;
    wxPenQuality GetQuality() const wxOVERRIDE;
    int GetDashes(wxDash** ptr) const wxOVERRIDE;
    wxDash* GetDash() const;
    int GetDashCount() const;
    wxBitmap* GetStipple() const wxOVERRIDE;


    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }


    // internal: wxGDIObject methods
    virtual bool RealizeResource() wxOVERRIDE;
    virtual bool FreeResource(bool force = false) wxOVERRIDE;
    virtual WXHANDLE GetResourceHandle() const wxOVERRIDE;
    virtual bool IsFree() const wxOVERRIDE;

protected:
    virtual wxGDIRefData* CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData* CloneGDIRefData(const wxGDIRefData* data) const wxOVERRIDE;

    // same as FreeResource() + RealizeResource()
    bool Recreate();

    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_PEN_H_
