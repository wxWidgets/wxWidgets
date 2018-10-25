///////////////////////////////////////////////////////////////////////////////
// Name:        wx/webrequest_curl.h
// Purpose:     wxWebRequest implementation using libcurl
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBREQUEST_CURL_H
#define _WX_WEBREQUEST_CURL_H

#if wxUSE_WEBREQUEST_CURL

class WXDLLIMPEXP_NET wxWebSessionCURL: public wxWebSession
{
public:
	wxWebSessionCURL();

	~wxWebSessionCURL();

	wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) wxOVERRIDE;

private:
	wxDECLARE_NO_COPY_CLASS(wxWebSessionCURL);
};

class WXDLLIMPEXP_NET wxWebSessionFactoryCURL: public wxWebSessionFactory
{
public:
	wxWebSession* Create() wxOVERRIDE
	{ return new wxWebSessionCURL(); }
};

#endif // wxUSE_WEBREQUEST_CURL

#endif
