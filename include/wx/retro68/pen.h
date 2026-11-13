/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/pen.h
// Purpose:     wxPen class declaration
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_PEN_H_
#define _WX_RETRO68_PEN_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxPen;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPen : public wxPenBase
{
public:
    wxPen() = default;
    wxPen(const wxColour &colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID);
    wxPen(const wxBitmap& stipple, int width);
    wxPen(const wxPenInfo& info);

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const { return !(*this == pen); }

    virtual void SetColour(const wxColour &colour) override;
    virtual void SetColour(unsigned char red, unsigned char green, unsigned char blue) override;
    virtual void SetCap(wxPenCap capStyle) override;
    virtual void SetJoin(wxPenJoin joinStyle) override;
    virtual void SetStyle(wxPenStyle style) override;
    virtual void SetWidth(int width) override;
    virtual void SetDashes(int number_of_dashes, const wxDash *dash) override;
    virtual void SetStipple(const wxBitmap& stipple) override;

    virtual wxColour GetColour() const override;
    virtual wxPenCap GetCap() const override;
    virtual wxPenJoin GetJoin() const override;
    virtual wxPenStyle GetStyle() const override;
    virtual int GetWidth() const override;
    virtual int GetDashes(wxDash **ptr) const override;
    int GetDashCount() const;
    wxDash* GetDash() const;
    virtual wxBitmap *GetStipple() const override;

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_RETRO68_PEN_H_
