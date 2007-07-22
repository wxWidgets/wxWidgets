#ifndef __WX_PEN_H__
#define __WX_PEN_H__

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"

class WXDLLEXPORT wxPen: public wxGDIObject {
public:
    wxPen();
    wxPen(const wxColour &colour, int width = 1, int style = wxSOLID);
    wxPen(const wxBitmap& stipple, int width);
    ~wxPen();

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const;

    void SetColour(const wxColour &colour);
    void SetColour(unsigned char red, unsigned char green, unsigned char blue);
    void SetCap(int capStyle);
    void SetJoin(int joinStyle);
    void SetStyle(int style);
    void SetWidth(int width);
    void SetDashes(int number_of_dashes, const wxDash *dash);

    wxColour &GetColour() const;
    int GetCap() const;
    int GetJoin() const;
    int GetStyle() const;
    int GetWidth() const;
    int GetDashes(wxDash **ptr) const;
    int GetDashCount() const;
    wxDash* GetDash() const;

    bool Ok() const;
    bool IsOk() const { return Ok(); }

protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData* data) const;
    DECLARE_DYNAMIC_CLASS(wxPen)
};

#endif // __WX_PEN_H__
