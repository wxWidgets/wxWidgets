#ifndef _WX_SCROLWIN_H_BASE_
#define _WX_SCROLWIN_H_BASE_

#ifdef __WXGTK__
#include "wx/gtk/scrolwin.h"
#else
#include "wx/generic/scrolwin.h"
// For non-GTK+ platforms, always use wxGenericScrolledWindow
class WXDLLEXPORT wxScrolledWindow: public wxGenericScrolledWindow
{
public:
    DECLARE_CLASS(wxScrolledWindow)
    wxScrolledWindow() {};
    wxScrolledWindow(wxWindow *parent,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
    {
        wxGenericScrolledWindow::Create(parent, id, pos, size, style, name);
    }
};
#endif

#endif
    // _WX_SCROLWIN_H_BASE_
