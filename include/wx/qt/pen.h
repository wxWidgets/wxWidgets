/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/pen.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PEN_H_
#define _WX_QT_PEN_H_

class WXDLLIMPEXP_CORE wxPen : public wxPenBase
{
public:
    wxPen();

    wxPen( const wxColour &colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID );
#if FUTURE_WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_FUTURE( wxPen(const wxColour& col, int width, int style) );
#endif

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const;

    virtual void SetColour(const wxColour& col);
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b);

    virtual void SetWidth(int width);
    virtual void SetStyle(wxPenStyle style);
    virtual void SetStipple(const wxBitmap& stipple);
    virtual void SetDashes(int nb_dashes, const wxDash *dash);
    virtual void SetJoin(wxPenJoin join);
    virtual void SetCap(wxPenCap cap);

    virtual wxColour GetColour() const;
    virtual wxBitmap *GetStipple() const;
    virtual wxPenStyle GetStyle() const;
    virtual wxPenJoin GetJoin() const;
    virtual wxPenCap GetCap() const;
    virtual int GetWidth() const;
    virtual int GetDashes(wxDash **ptr) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
};

#endif // _WX_QT_PEN_H_
