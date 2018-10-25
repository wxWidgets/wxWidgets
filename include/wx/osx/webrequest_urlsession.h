///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/webrequest_urlsession.h
// Purpose:     wxWebRequest implementation using URLSession
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_WEBREQUEST_URLSESSION_H
#define _WX_OSX_WEBREQUEST_URLSESSION_H

#if wxUSE_WEBREQUEST_URLSESSION

DECLARE_WXCOCOA_OBJC_CLASS(NSURLSession);
DECLARE_WXCOCOA_OBJC_CLASS(NSURLTask);

class WXDLLIMPEXP_NET wxWebSessionURLSession: public wxWebSession
{
public:
	wxWebSessionURLSession();

	~wxWebSessionURLSession();

	wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) wxOVERRIDE;

private:
	WX_NSURLSession m_session;

	wxDECLARE_NO_COPY_CLASS(wxWebSessionURLSession);
};

class WXDLLIMPEXP_NET wxWebSessionFactoryURLSession: public wxWebSessionFactory
{
public:
	wxWebSession* Create() wxOVERRIDE
	{ return new wxWebSessionURLSession(); }
};

#endif // wxUSE_WEBREQUEST_URLSESSION

#endif // _WX_OSX_WEBREQUEST_URLSESSION_H
