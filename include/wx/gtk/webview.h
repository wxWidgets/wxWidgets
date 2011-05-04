/////////////////////////////////////////////////////////////////////////////
// Name:        include/gtk/wx/webview.h
// Purpose:     GTK webkit backend for web view component
// Author:      Robert Roebling, Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 200 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WEBKITCTRL_H_
#define _WX_GTK_WEBKITCTRL_H_

#include "wx/setup.h"

#if wxHAVE_WEB_BACKEND_GTK_WEBKIT

#include "wx/webview.h"

//-----------------------------------------------------------------------------
// wxWebViewGTKWebKit
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWebViewGTKWebKit : public wxWebView
{
public:
    wxWebViewGTKWebKit() { Init(); }

    wxWebViewGTKWebKit(wxWindow *parent,
           wxWindowID id = wxID_ANY,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxWebViewNameStr)
    {
        Init();

        Create(parent, id, url, pos, size, style, name);
    }

    virtual bool Create(wxWindow *parent,
           wxWindowID id = wxID_ANY,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxWebViewNameStr);

    virtual bool Enable( bool enable = true );

    // implementation
    // --------------

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // helper to allow access to protected member from GTK callback
    void MoveWindow(int x, int y, int width, int height)
    {
        DoMoveWindow(x, y, width, height);
    }

    void ZoomIn();
    void ZoomOut();
    void SetWebkitZoom(float level);
    float GetWebkitZoom();

    virtual void Stop();
    virtual void LoadUrl(const wxString& url);
    virtual void GoBack();
    virtual void GoForward();
    virtual void Reload(wxWebViewReloadFlags flags = 0);
    virtual bool CanGoBack();
    virtual bool CanGoForward();
    virtual wxString GetCurrentURL();
    virtual wxString GetCurrentTitle();
    virtual wxString GetPageSource();
    virtual void SetPage(const wxString& html, const wxString& baseUrl);
    virtual void Print();
    virtual bool IsBusy();

    void SetZoomType(wxWebViewZoomType);
    wxWebViewZoomType GetZoomType() const;
    bool CanSetZoomType(wxWebViewZoomType) const;
    virtual wxWebViewZoom GetZoom();
    virtual void SetZoom(wxWebViewZoom);



    /** FIXME: hack to work around signals being received too early */
    bool m_ready;


    /** TODO: check if this can be made private
     * The native control has a getter to check for busy state, but except in
     * very recent versions of webkit this getter doesn't say everything we need
     * (namely it seems to stay indefinitely busy when loading is cancelled by
     * user)
     */
    bool m_busy;

protected:

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

private:

    // focus event handler: calls GTKUpdateBitmap()
    void GTKOnFocus(wxFocusEvent& event);

    GtkWidget *web_view;

    // FIXME: try to get DECLARE_DYNAMIC_CLASS macros & stuff right
    //DECLARE_DYNAMIC_CLASS(wxWebViewGTKWebKit)
};

#endif // if wxHAVE_WEB_BACKEND_GTK_WEBKIT

