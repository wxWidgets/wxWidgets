#ifndef __WX_BRUSH_H__
#define __WX_BRUSH_H__

class WXDLLEXPORT wxBrush: public wxBrushBase
{
public:
    wxBrush();
    wxBrush(const wxColour &colour, int style = wxSOLID);
    wxBrush(const wxBitmap &stippleBitmap);
    virtual ~wxBrush();

    bool operator==(const wxBrush& brush) const;
    bool operator!=(const wxBrush& brush) const;

    wxColour &GetColour() const;
    wxBitmap *GetStipple() const;
    int GetStyle() const;
    bool IsOk() const { return Ok(); }
    bool Ok() const;

    void SetColour(const wxColour& colour);
    void SetColour(const wxString& colourName);
    void SetColour(unsigned char red, unsigned char green, unsigned char blue);
    void SetStyle(int style);
    void SetStipple(const wxBitmap& stipple);

protected:
    virtual wxObjectRefData* CreateRefData() const;
    virtual wxObjectRefData* CloneRefData(const wxObjectRefData* data) const;
    
    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif // __WX_BRUSH_H__
