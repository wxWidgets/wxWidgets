///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/popupwin.h
// Purpose:     wxPopupWindow class for wxMSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_POPUPWIN_H_
#define _WX_MSW_POPUPWIN_H_

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow() { }

    wxPopupWindow(wxWindow *parent) { (void)Create(parent); }

    bool Create(wxWindow *parent)
    {
        return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, -1,
                                wxDefaultPosition, wxDefaultSize,
                                // don't use the border - child will have its
                                // own
                                wxBORDER_NONE | wxPOPUP_WINDOW);
    }
};

#endif // _WX_MSW_POPUPWIN_H_

