///////////////////////////////////////////////////////////////////////////////
// Name:        wx/infobar.h
// Purpose:     declaration of wxInfoBarBase defining common API of wxInfoBar
// Author:      Vadim Zeitlin
// Created:     2009-07-28
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_INFOBAR_H_
#define _WX_INFOBAR_H_

#include "wx/defs.h"

#if wxUSE_INFOBAR

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxInfoBar shows non-critical but important information to the user
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxInfoBarBase : public wxWindow
{
public:
    // real ctors are provided by the derived classes, just notice that unlike
    // most of the other windows, info bar is created hidden and must be
    // explicitly shown when it is needed (this is done because it is supposed
    // to be shown only intermittently and hiding it after creating it from the
    // user code would result in flicker)
    wxInfoBarBase() { }


    // show the info bar with the given message and optionally an icon
    virtual void ShowMessage(const wxString& msg, int flags = wxICON_NONE) = 0;

    // add an extra button to the bar, near the message
    virtual void AddButton(wxWindowID btnid,
                           const wxString& label = wxString()) = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxInfoBarBase);
};

// include platform-dependent implementation
//
// TODO-GTK: implement a native version using GtkInfoBar (GTK+ 2.18+)
#include "wx/generic/infobar.h"

#endif // wxUSE_INFOBAR

#endif // _WX_INFOBAR_H_
