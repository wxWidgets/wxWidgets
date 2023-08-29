/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/tooltip.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TOOLTIP_H_
#define _WX_QT_TOOLTIP_H_

#include "wx/object.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxToolTip : public wxObject
{
public:
    // controlling tooltip behaviour: globally change tooltip parameters
        // enable or disable the tooltips globally
    static void Enable(bool flag);
        // set the delay after which the tooltip appears
    static void SetDelay(long milliseconds);
        // set the delay after which the tooltip disappears or how long the
        // tooltip remains visible
    static void SetAutoPop(long milliseconds);
        // set the delay between subsequent tooltips to appear
    static void SetReshow(long milliseconds);

    wxToolTip(const wxString &tip);

    void SetTip(const wxString& tip);
    const wxString& GetTip() const;

    // the window we're associated with
    void SetWindow(wxWindow *win);
    wxWindow *GetWindow() const { return m_window; }

private:
    wxString m_text;
    wxWindow* m_window;         // main window we're associated with
};

#endif // _WX_QT_TOOLTIP_H_
