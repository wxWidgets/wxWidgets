/////////////////////////////////////////////////////////////////////////////
// Name:        include/gtk/wx/webview.h
// Purpose:     GTK webkit backend for web view component
// Author:      Robert Roebling, Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WEBKITCTRL_H_
#define _WX_GTK_WEBKITCTRL_H_

#include "wx/setup.h"

#if wxUSE_WEBVIEW_WEBKIT && defined(__WXGTK__)

#include "webkit/webkit.h"
#include "wx/sharedptr.h"
#include "wx/webview.h"

//-----------------------------------------------------------------------------
// wxWebViewWebKit
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_WEB wxWebViewWebKit : public wxWebView
{
public:
    wxWebViewWebKit() { Init(); }

    wxWebViewWebKit(wxWindow *parent,
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
    virtual void Reload(wxWebViewReloadFlags flags = wxWEB_VIEW_RELOAD_DEFAULT);
    virtual bool CanGoBack();
    virtual bool CanGoForward();
    virtual void ClearHistory();
    virtual void EnableHistory(bool enable = true);
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetBackwardHistory();
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetForwardHistory();
    virtual void LoadHistoryItem(wxSharedPtr<wxWebHistoryItem> item);
    virtual wxString GetCurrentURL();
    virtual wxString GetCurrentTitle();
    virtual wxString GetPageSource();
    virtual wxString GetPageText();
    //We do not want to hide the other overloads
    using wxWebView::SetPage;
    virtual void SetPage(const wxString& html, const wxString& baseUrl);
    virtual void Print();
    virtual bool IsBusy();

    void SetZoomType(wxWebViewZoomType);
    wxWebViewZoomType GetZoomType() const;
    bool CanSetZoomType(wxWebViewZoomType) const;
    virtual wxWebViewZoom GetZoom();
    virtual void SetZoom(wxWebViewZoom);

    //Clipboard functions
    virtual bool CanCut();
    virtual bool CanCopy();
    virtual bool CanPaste();
    virtual void Cut();
    virtual void Copy();
    virtual void Paste();

    //Undo / redo functionality
    virtual bool CanUndo();
    virtual bool CanRedo();
    virtual void Undo();
    virtual void Redo();

    //Editing functions
    virtual void SetEditable(bool enable = true);
    virtual bool IsEditable();

    //Selection
    virtual void DeleteSelection();
    virtual bool HasSelection();
    virtual void SelectAll();
    virtual wxString GetSelectedText();
    virtual wxString GetSelectedSource();
    virtual void ClearSelection();

    virtual void RunScript(const wxString& javascript);
    
    //Virtual Filesystem Support
    virtual void RegisterHandler(wxWebHandler* handler);
    virtual wxVector<wxWebHandler*> GetHandlers() { return m_handlerList; }

    /** FIXME: hack to work around signals being received too early */
    bool m_ready;


    /** TODO: check if this can be made private
     * The native control has a getter to check for busy state, but except in
     * very recent versions of webkit this getter doesn't say everything we need
     * (namely it seems to stay indefinitely busy when loading is cancelled by
     * user)
     */
    bool m_busy;

    //We use this flag to stop recursion when we load a page from the navigation
    //callback, mainly when loading a VFS page
    bool m_guard;

protected:

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

private:

    // focus event handler: calls GTKUpdateBitmap()
    void GTKOnFocus(wxFocusEvent& event);

    GtkWidget *web_view;
    gint m_historyLimit;

    wxVector<wxWebHandler*> m_handlerList;

    wxDECLARE_DYNAMIC_CLASS(wxWebViewWebKit);
};

#endif // wxUSE_WEBVIEW_WEBKIT && defined(__WXGTK__)

#endif
