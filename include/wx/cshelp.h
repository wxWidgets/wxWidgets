/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cshelp.h
// Purpose:     Context-sensitive help support classes
// Author:      Julian Smart, Vadim Zeitlin
// Modified by:
// Created:     08/09/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Julian Smart, Vadim Zeitlin
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CSHELPH__
#define _WX_CSHELPH__

#ifdef __GNUG__
    #pragma interface "cshelp.h"
#endif

#include "wx/defs.h"
#include "wx/help.h"

#if wxUSE_HELP

#include "wx/bmpbuttn.h"

// ----------------------------------------------------------------------------
// classes used to implement context help UI
// ----------------------------------------------------------------------------

/*
 * wxContextHelp
 * Invokes context-sensitive help. When the user
 * clicks on a window, a wxEVT_HELP event will be sent to that
 * window for the application to display help for.
 */

class WXDLLEXPORT wxContextHelp : public wxObject
{
public:
    wxContextHelp(wxWindow* win = NULL, bool beginHelp = TRUE);
    virtual ~wxContextHelp();

    bool BeginContextHelp(wxWindow* win);
    bool EndContextHelp();

    bool EventLoop();
    bool DispatchEvent(wxWindow* win, const wxPoint& pt);

    void SetStatus(bool status) { m_status = status; }

protected:
    bool    m_inHelp;
    bool    m_status; // TRUE if the user left-clicked

private:
    DECLARE_DYNAMIC_CLASS(wxContextHelp)
};

/*
 * wxContextHelpButton
 * You can add this to your dialogs (especially on non-Windows platforms)
 * to put the application into context help mode.
 */

class WXDLLEXPORT wxContextHelpButton : public wxBitmapButton
{
public:
    wxContextHelpButton(wxWindow* parent,
                        wxWindowID id = wxID_CONTEXT_HELP,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxBU_AUTODRAW);

    void OnContextHelp(wxCommandEvent& event);

private:
    DECLARE_CLASS(wxContextHelpButton)
    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_HELP

// ----------------------------------------------------------------------------
// classes used to implement context help support
// ----------------------------------------------------------------------------

// wxHelpProvider is an abstract class used by the program implementing context help to
// show the help text (or whatever: it may be HTML page or anything else) for
// the given window.
//
// The current help provider must be explicitly set by the application using
// wxHelpProvider::Set().
class WXDLLEXPORT wxHelpProvider
{
public:
    // get/set the current (application-global) help provider (Set() returns
    // the previous one)
    static wxHelpProvider *Set(wxHelpProvider *helpProvider)
    {
        wxHelpProvider *helpProviderOld = ms_helpProvider;
        ms_helpProvider = helpProvider;
        return helpProviderOld;
    }

    // unlike some other class, the help provider is not created on demand,
    // this must be explicitly done by the application
    static wxHelpProvider *Get() { return ms_helpProvider; }

    // get the help string (whose interpretation is help provider dependent
    // except that empty string always means that no help is associated with
    // the window) for this window
    virtual wxString GetHelp(const wxWindowBase *window) = 0;

    // do show help for the given window (uses GetHelp() internally if
    // applicable), return TRUE if it was done or FALSE if no help available
    // for this window
    virtual bool ShowHelp(wxWindowBase *window) = 0;

    // associate the text with the given window or id: although all help
    // providers have these functions to allow making wxWindow::SetHelpText()
    // work, not all of them implement them
    virtual void AddHelp(wxWindowBase *window, const wxString& text);

    // this version associates the given text with all window with this id
    // (may be used to set the same help string for all [Cancel] buttons in
    // the application, for example)
    virtual void AddHelp(wxWindowID id, const wxString& text);

    // virtual dtor for any base class
    virtual ~wxHelpProvider();

private:
    static wxHelpProvider *ms_helpProvider;
};

// wxSimpleHelpProvider is an implementation of wxHelpProvider which supports
// only plain text help strings and shows the string associated with the
// control (if any) in a tooltip
class WXDLLEXPORT wxSimpleHelpProvider : public wxHelpProvider
{
public:
    // implement wxHelpProvider methods
    virtual wxString GetHelp(const wxWindowBase *window);
    virtual bool ShowHelp(wxWindowBase *window);
    virtual void AddHelp(wxWindowBase *window, const wxString& text);
    virtual void AddHelp(wxWindowID id, const wxString& text);

protected:
    // we use 2 hashes for storing the help strings associated with windows
    // and the ids
    wxStringHashTable m_hashWindows,
                     m_hashIds;
};

// wxHelpControllerHelpProvider is an implementation of wxHelpProvider which supports
// both context identifiers and plain text help strings. If the help text is an integer,
// it is passed to wxHelpController::DisplayContextPopup. Otherwise, it shows the string
// in a tooltip as per wxSimpleHelpProvider.
class WXDLLEXPORT wxHelpControllerHelpProvider : public wxSimpleHelpProvider
{
public:
    // Note that it doesn't own the help controller. The help controller
    // should be deleted separately.
    wxHelpControllerHelpProvider(wxHelpControllerBase* hc = (wxHelpControllerBase*) NULL);

    // implement wxHelpProvider methods
    virtual bool ShowHelp(wxWindowBase *window);

    // Other accessors
    void SetHelpController(wxHelpControllerBase* hc) { m_helpController = hc; }
    wxHelpControllerBase* GetHelpController() const { return m_helpController; }

protected:
    wxHelpControllerBase*   m_helpController;
};

// Convenience function for turning context id into wxString
WXDLLEXPORT wxString wxContextId(int id);

#endif // _WX_CSHELPH__

