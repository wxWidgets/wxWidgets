/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webhistoryitem.h
// Purpose:     wxWebHistoryItem header for MSW
// Author:      Steven Lamerton
// Id:          $Id$
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WEBHISTORYITEM_H_
#define _WX_MSW_WEBHISTORYITEM_H_

#include "wx/setup.h"

#if wxUSE_WEBVIEW_IE && defined(__WXMSW__)

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

#endif // wxUSE_WEBVIEW_IE && defined(__WXMSW__)

#endif // _WX_MSW_WEBHISTORYITEM_H_
