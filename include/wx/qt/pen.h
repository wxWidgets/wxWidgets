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

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const;

    virtual void SetColour(const wxColour& col) wxOVERRIDE;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) wxOVERRIDE;

    virtual void SetWidth(int width) wxOVERRIDE;
    virtual void SetStyle(wxPenStyle style) wxOVERRIDE;
    virtual void SetStipple(const wxBitmap& stipple) wxOVERRIDE;
    virtual void SetDashes(int nb_dashes, const wxDash *dash) wxOVERRIDE;
    virtual void SetJoin(wxPenJoin join) wxOVERRIDE;
    virtual void SetCap(wxPenCap cap) wxOVERRIDE;

    virtual wxColour GetColour() const wxOVERRIDE;
    virtual wxBitmap *GetStipple() const wxOVERRIDE;
    virtual wxPenStyle GetStyle() const wxOVERRIDE;
    virtual wxPenJoin GetJoin() const wxOVERRIDE;
    virtual wxPenCap GetCap() const wxOVERRIDE;
    virtual int GetWidth() const wxOVERRIDE;
    virtual int GetDashes(wxDash **ptr) const wxOVERRIDE;

    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

    QPen GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_QT_PEN_H_
