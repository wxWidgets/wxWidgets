#ifndef __WX_DCSCREEN_H__
#define __WX_DCSCREEN_H__

#include "wx/dc.h"

class WXDLLEXPORT wxScreenDC: public wxDC {
public:
    wxScreenDC();
    ~wxScreenDC();

    static bool StartDrawingOnTop(wxWindow *WXUNUSED(window)) { return true; }
    static bool StartDrawingOnTop(wxRect *WXUNUSED(rect) = NULL) { return true; }
    static bool EndDrawingOnTop() { return true; }

private:
    DECLARE_DYNAMIC_CLASS(wxScreenDC)
};

#endif
    // __WX_DCSCREEN_H__
