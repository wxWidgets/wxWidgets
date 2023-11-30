/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/pen.h
// Purpose:     wxPen class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

// Pen
class WXDLLIMPEXP_CORE wxPen : public wxPenBase
{
public:
    wxPen();
    wxPen(const wxColour& col, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID);

    wxPen(const wxBitmap& stipple, int width);

    wxPen(const wxPenInfo& info);

    virtual ~wxPen();

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const { return !(*this == pen); }

    // Override in order to recreate the pen
    void SetColour(const wxColour& col) override;
    void SetColour(unsigned char r, unsigned char g, unsigned char b) override;

    void SetWidth(int width) override;
    void SetStyle(wxPenStyle style) override;
    void SetStipple(const wxBitmap& stipple)  override;
    void SetDashes(int nb_dashes, const wxDash *dash) override;
    void SetJoin(wxPenJoin join) override;
    void SetCap(wxPenCap cap) override;

    wxColour GetColour() const override;
    int GetWidth() const override;
    wxPenStyle GetStyle() const override;
    wxPenJoin GetJoin() const override;
    wxPenCap GetCap() const override;
    int GetDashes(wxDash **ptr) const override;
    int GetDashCount() const;

    wxBitmap *GetStipple() const override;


    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

    // Implementation

    // Useful helper: create the brush resource
    bool RealizeResource();

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    void Unshare();

    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif
    // _WX_PEN_H_
