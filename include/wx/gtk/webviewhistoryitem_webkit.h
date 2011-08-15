/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/gtk/webviewhistoryitem.h
// Purpose:     wxWebViewHistoryItem header for GTK
// Author:      Steven Lamerton
// Id:          $Id$
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WEBVIEWHISTORYITEM_H_
#define _WX_GTK_WEBVIEWHISTORYITEM_H_

#include "wx/setup.h"

#if wxUSE_WEBVIEW_WEBKIT && defined(__WXGTK__)

#include "webkit/webkit.h"

class WXDLLIMPEXP_WEB wxWebViewHistoryItem
{
public:
    wxWebViewHistoryItem(const wxString& url, const wxString& title) : 
                     m_url(url), m_title(title) {}
    wxString GetUrl() { return m_url; }
    wxString GetTitle() { return m_title; }

    friend class wxWebViewWebKit;

private:
    wxString m_url, m_title;
    WebKitWebHistoryItem* m_histItem;
};

#endif // wxUSE_WEBVIEW_WEBKIT && defined(__WXGTK__)

#endif // _WX_GTK_WEBVIEWHISTORYITEM_H_
