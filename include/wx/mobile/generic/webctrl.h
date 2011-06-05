/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/webctrl.h
// Purpose:     wxMoWebCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_WEBCTRL_H_
#define _WX_MOBILE_GENERIC_WEBCTRL_H_

#include "wx/bitmap.h"
#include "wx/control.h"

#ifdef __WXMSW__
class wxIEHtmlWin;
#elif defined(__WXMAC__) && wxUSE_WEBKIT
#include "wx/html/webkit.h"
#else
#include "wx/html/htmlwin.h"
#endif

extern WXDLLEXPORT_DATA(const wxChar) wxWebCtrlNameStr[];

/**
    @class wxMoWebCtrl

    A control for displaying remote web pages.
    The simulated control is implemented by an ActiveX control
    on Windows, a web kit control on Mac, and wxHtmlWindow on other
    platforms. wxHtmlWindow is only capable of limited HTML formatting,
    and may not necessarily retrieve a remote URL.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoWebCtrl : public wxControl
{
public:
    /// Default constructor.
    wxMoWebCtrl() { Init(); }

    /// Constructor.
    wxMoWebCtrl(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxWebCtrlNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWebCtrlNameStr);

    virtual ~wxMoWebCtrl();

    void Init();

    /// Begins loading the specified URL.
    bool LoadURL(const wxString& url);

    /// Returns true if it is possible to go back in the browser history.
    bool CanGoBack();

    /// Returns true if it is possible to go forward in the browser history.
    bool CanGoForward();

    /// Goes back in the browser history.
    bool GoBack();

    /// Goes forward in the browser history.
    bool GoForward();

    /// If scaling is true, the user will be able to adjust the zoom.
    /// The default is false.
    void SetUserScaling(bool scaling) { m_userScaling = scaling; }

    /// Returns true if the user can adjust the zoom.
    bool GetUserScaling() const { return m_userScaling; }

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);

protected:
    virtual wxSize DoGetBestSize() const;
    void OnSize(wxSizeEvent& event);

#ifdef __WXMSW__
    wxIEHtmlWin*    m_htmlWindow;
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    wxWebKitCtrl*   m_htmlWindow;
#else
    wxHtmlWindow*   m_htmlWindow;
#endif

    bool            m_userScaling;
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoWebCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_GENERIC_WEBCTRL_H_
