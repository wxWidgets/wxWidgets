/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/osx/webhistoryitem.h
// Purpose:     wxWebHistoryItem header for OSX
// Author:      Steven Lamerton
// Id:          $Id$
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_WEBHISTORYITEM_H_
#define _WX_OSX_WEBHISTORYITEM_H_

#include "wx/setup.h"

#if wxUSE_WEBVIEW_WEBKIT

class WXDLLIMPEXP_WEB wxWebHistoryItem
{
public:
    wxWebHistoryItem(const wxString& url, const wxString& title) : 
                     m_url(url), m_title(title) {}
    wxString GetUrl() { return m_url; }
    wxString GetTitle() { return m_title; }

private:
    wxString m_url, m_title;
};

#endif // wxUSE_WEBVIEW_WEBKIT

#endif // _WX_OSX_WEBHISTORYITEM_H_
