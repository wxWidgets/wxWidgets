#ifndef __WX_BRUSH_H__
#define __WX_BRUSH_H__

#include "wx/brush.h"
#include "wx/string.h"

class WXDLLEXPORT wxColour;
class WXDLLEXPORT wxBitmap;

class WXDLLEXPORT wxBrush: public wxBrushBase
{
public:
    wxBrush() {}
    wxBrush(const wxColour &colour, int style = wxSOLID);
    wxBrush(const wxBitmap &stippleBitmap);
    ~wxBrush() {}

    wxColour &GetColour() const;
    wxBitmap *GetStipple() const;
    int GetStyle() const;
    bool IsHatch() const;
    bool Ok() const;

    void SetColour(const wxColour& colour);
    void SetColour(const wxString& colourName);
    void SetColour(unsigned char red, unsigned char green, unsigned char blue);
    void SetStyle(int style);
    void SetStipple(const wxBitmap& stipple);

private:
    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif // __WX_BRUSH_H__
