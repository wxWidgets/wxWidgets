/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/brush.h
// Purpose:     wxBrush class declaration
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_BRUSH_H_
#define _WX_RETRO68_BRUSH_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxBrush;

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBrush : public wxBrushBase
{
public:
    wxBrush() = default;
    wxBrush(const wxColour &colour, wxBrushStyle style = wxBRUSHSTYLE_SOLID);
    wxBrush(const wxBitmap &stippleBitmap);

    bool operator==(const wxBrush& brush) const;
    bool operator!=(const wxBrush& brush) const { return !(*this == brush); }

    virtual wxBrushStyle GetStyle() const override;
    virtual wxColour GetColour() const override;
    virtual wxBitmap *GetStipple() const override;

    virtual void SetColour(const wxColour& col) override;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) override;
    virtual void SetStyle(wxBrushStyle style) override;
    virtual void SetStipple(const wxBitmap& stipple) override;

    wxDEPRECATED_MSG("use wxBRUSHSTYLE_XXX constants")
    wxBrush(const wxColour& col, int style);

    wxDEPRECATED_MSG("use wxBRUSHSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxBrushStyle)style); }

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    wxDECLARE_DYNAMIC_CLASS(wxBrush);
};

#endif // _WX_RETRO68_BRUSH_H_
