/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/brush.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BRUSH_H_
#define _WX_QT_BRUSH_H_

class QBrush;

class WXDLLIMPEXP_CORE wxBrush : public wxBrushBase
{
public:
    wxBrush();
    wxBrush(const wxColour& col, wxBrushStyle style = wxBRUSHSTYLE_SOLID);

    wxDEPRECATED_MSG("use wxBRUSHSTYLE_XXX constants")
    wxBrush(const wxColour& col, int style);

    wxBrush(const wxBitmap& stipple);

    virtual void SetColour(const wxColour& col);
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b);
    virtual void SetStyle(wxBrushStyle style);
    virtual void SetStipple(const wxBitmap& stipple);

    bool operator==(const wxBrush& brush) const;
    bool operator!=(const wxBrush& brush) const { return !(*this == brush); }

    virtual wxColour GetColour() const;
    virtual wxBrushStyle GetStyle() const;
    virtual wxBitmap *GetStipple() const;

    wxDEPRECATED_MSG("use wxBRUSHSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxBrushStyle)style); }
    
    QBrush GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
};

#endif // _WX_QT_BRUSH_H_
