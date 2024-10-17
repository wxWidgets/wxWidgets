/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/pen.h
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PEN_H_
#define _WX_QT_PEN_H_

class QPen;

class WXDLLIMPEXP_CORE wxPen : public wxPenBase
{
public:
    wxPen();

    wxPen( const wxColour &colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID );

    wxPen( const wxPenInfo& info );

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const;

    virtual void SetColour(const wxColour& col) override;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) override;

    virtual void SetWidth(int width) override;
    virtual void SetStyle(wxPenStyle style) override;
    virtual void SetStipple(const wxBitmap& stipple) override;
    virtual void SetDashes(int nb_dashes, const wxDash *dash) override;
    virtual void SetJoin(wxPenJoin join) override;
    virtual void SetCap(wxPenCap cap) override;

    virtual wxColour GetColour() const override;
    virtual wxBitmap *GetStipple() const override;
    virtual wxPenStyle GetStyle() const override;
    virtual wxPenJoin GetJoin() const override;
    virtual wxPenCap GetCap() const override;
    virtual int GetWidth() const override;
    virtual int GetDashes(wxDash **ptr) const override;

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

    QPen GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_QT_PEN_H_
