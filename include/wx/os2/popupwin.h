///////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/popupwin.h
// Purpose:     wxPopupWindow class for wxPM
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PM_POPUPWIN_H_
#define _WX_PM_POPUPWIN_H_

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow() { }

    wxPopupWindow(wxWindow *parent) { (void)Create(parent); }

    bool Create(wxWindow *parent, int flags = wxBORDER_NONE)
    {
        return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, -1,
                                wxDefaultPosition, wxDefaultSize,
                                (flags & wxBORDER_MASK) | wxPOPUP_WINDOW);
    }
};

#endif // _WX_PM_POPUPWIN_H_
