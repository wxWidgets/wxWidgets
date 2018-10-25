///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/webrequest_urlsession.h
// Purpose:     wxWebRequest implementation using URLSession
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

#if wxUSE_WEBREQUEST_URLSESSION

#import <Foundation/Foundation.h>

#include "wx/osx/webrequest_urlsession.h"

wxWebSessionURLSession::wxWebSessionURLSession()
{
	m_session = [NSURLSession sessionWithConfiguration:
				 [NSURLSessionConfiguration defaultSessionConfiguration]];
}

wxWebSessionURLSession::~wxWebSessionURLSession()
{
	[m_session release];
}

wxWebRequest* wxWebSessionURLSession::CreateRequest(const wxString& url, int id)
{
	wxFAIL_MSG("not implemented");
	return NULL;
}

#endif // wxUSE_WEBREQUEST_URLSESSION
