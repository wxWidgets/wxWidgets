///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/webrequest_curl.h
// Purpose:     wxWebRequest implementation using libcurl
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST_CURL

#include "wx/webrequest_curl.h"

wxWebSessionCURL::wxWebSessionCURL()
{

}

wxWebSessionCURL::~wxWebSessionCURL()
{

}

wxWebRequest* wxWebSessionCURL::CreateRequest(const wxString& url, int id)
{
	wxFAIL_MSG("not implemented");
	return NULL;
}

#endif // wxUSE_WEBREQUEST_CURL
