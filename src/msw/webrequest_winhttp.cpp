///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/webrequest_winhttp.h
// Purpose:     wxWebRequest WinHTTP implementation
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST

#include "wx/msw/webrequest_winhttp.h"

wxWebRequest* wxWebSessionWinHTTP::CreateRequest(const wxString& url, int id)
{
    return NULL;
}

void wxWebSessionWinHTTP::SetHeader(const wxString& name, const wxString& value)
{

}

#endif // wxUSE_WEBREQUEST
