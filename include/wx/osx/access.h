/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/access.h
// Purpose:     declaration of the wxAccessible class
// Copyright:   (c) 2026 Joshua Lee Ockert
// Licence:     wxWindows licence
// SPDX-License-Identifier: wxWindows OR LGPL-2.0-or-later WITH WxWindows-exception-3.1
// Dedicated in loving memory to Mark John Ockert 1961-2025
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_ACCESS_H_
#define   _WX_ACCESS_H_

#if wxUSE_ACCESSIBILITY

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxWindow;

// ----------------------------------------------------------------------------
// wxAccessible implements accessibility behaviour.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAccessible : public wxAccessibleBase
{
public:
    wxAccessible(wxWindow *win = nullptr);
    virtual ~wxAccessible();

// Operations

    // Sends an event when something changes in an accessible object.
    static void NotifyEvent(int eventType, wxWindow* window, wxAccObject objectType,
                            int objectId);

private:
    wxDECLARE_NO_COPY_CLASS(wxAccessible);
};

#endif  //wxUSE_ACCESSIBILITY

#endif  //_WX_ACCESS_H_
