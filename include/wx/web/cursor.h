#ifndef __WX_CURSOR_H__
#define __WX_CURSOR_H__

#if wxUSE_IMAGE
#include "wx/image.h"
#endif //wxUSE_IMAGE

class WXDLLEXPORT wxCursor: public wxObject {
public:

    wxCursor();
    wxCursor(int cursorId);
    wxCursor(const char bits[], int width, int  height,
              int hotSpotX=-1, int hotSpotY=-1,
              const char maskBits[]=NULL, const wxColour *fg=NULL, const wxColour *bg=NULL);
    wxCursor(const wxString& name,
             long type,
             int hotSpotX = 0, int hotSpotY = 0);
#if wxUSE_IMAGE
    wxCursor(const wxImage& image);
#endif //wxUSE_IMAGE
    ~wxCursor();
    bool IsOk() { return Ok(); }
    bool Ok() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif // __WX_CURSOR_H__
