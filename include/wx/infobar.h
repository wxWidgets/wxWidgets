///////////////////////////////////////////////////////////////////////////////
// Name:        wx/infobar.h
// Purpose:     declaration of wxInfoBarBase defining common API of wxInfoBar
// Author:      Vadim Zeitlin
// Created:     2009-07-28
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_INFOBAR_H_
#define _WX_INFOBAR_H_

#include "wx/defs.h"

#if wxUSE_INFOBAR

#include "wx/control.h"

#include <memory>

class wxInfoBarDismissTimer;

// ----------------------------------------------------------------------------
// wxInfoBar shows non-critical but important information to the user
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxInfoBarBase : public wxControl
{
public:
    // real ctors are provided by the derived classes, just notice that unlike
    // most of the other windows, info bar is created hidden and must be
    // explicitly shown when it is needed (this is done because it is supposed
    // to be shown only intermittently and hiding it after creating it from the
    // user code would result in flicker)
    wxInfoBarBase();
    virtual ~wxInfoBarBase();


    // show the info bar with the given message and optionally an icon
    virtual void ShowMessage(const wxString& msg,
                             int flags = wxICON_INFORMATION) = 0;

    // hide the info bar
    virtual void Dismiss() = 0;

#if wxUSE_TIMER
    // special values for ShowMessageFor() timeout
    enum
    {
        Timeout_Auto = -1,  // use the default timeout
        Timeout_Never = 0   // don't hide the bar automatically
    };

    // show the info bar with the given message and hide it automatically
    // after the given number of seconds
    void ShowMessageFor(const wxString& msg,
                        int timeoutInSeconds,
                        int flags = wxICON_INFORMATION);
#endif // wxUSE_TIMER

    // add an extra button to the bar, near the message (replacing the default
    // close button which, by default, is only shown if no extra buttons are
    // used)
    virtual void AddButton(wxWindowID btnid,
                           const wxString& label = wxString()) = 0;

    // remove a button previously added by AddButton()
    virtual void RemoveButton(wxWindowID btnid) = 0;

    // remove all buttons added by AddButton(), restoring the default close one
    void RemoveAllButtons();

    // get information about the currently shown buttons
    virtual size_t GetButtonCount() const = 0;
    virtual wxWindowID GetButtonId(size_t idx) const = 0;
    virtual bool HasButtonId(wxWindowID btnid) const = 0;

    // possible values for ShowCloseButton() argument
    enum class ShowButton
    {
        Auto,   // only if there are no other buttons (default)
        Always, // always, after all the other buttons
        Never   // never, even if there are no other buttons
    };

    // choose when the default close button is shown
    virtual void ShowCloseButton(ShowButton show) = 0;

#if wxUSE_TIMER
protected:
    // must be called by ShowMessage() and Dismiss() implementations in the
    // derived classes to cancel hiding the bar if ShowMessageFor() was used
    void StopDismissTimer();

private:
    // timer used by ShowMessageFor(), only created when it's called
    std::unique_ptr<wxInfoBarDismissTimer> m_dismissTimer;
#else // !wxUSE_TIMER
protected:
    void StopDismissTimer() { }

private:
#endif // wxUSE_TIMER/!wxUSE_TIMER

    wxDECLARE_NO_COPY_CLASS(wxInfoBarBase);
};

// currently only GTK+ has a native implementation
#if defined(__WXGTK218__) && !defined(__WXUNIVERSAL__)
    #include "wx/gtk/infobar.h"
    #define wxHAS_NATIVE_INFOBAR
#endif // wxGTK2

// if the generic version is the only one we have, use it
#ifndef wxHAS_NATIVE_INFOBAR
    #include "wx/generic/infobar.h"
    #define wxInfoBar wxInfoBarGeneric
#endif

#endif // wxUSE_INFOBAR

#endif // _WX_INFOBAR_H_
