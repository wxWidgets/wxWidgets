#ifndef __WX_CURSOR_H__
#define __WX_CURSOR_H__

class WXDLLEXPORT wxString;
class WXDLLEXPORT wxImage;
class WXDLLEXPORT wxColour;

class WXDLLEXPORT wxCursor: public wxObject {
public:

    wxCursor();
    wxCursor(int cursorId);
    wxCursor(const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=NULL, wxColour *fg=NULL, wxColour *bg=NULL);
    wxCursor(const wxString& name,
             long type,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(const wxImage& image);
    ~wxCursor();
    bool Ok() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // __WX_CURSOR_H__
