///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/webrequest_winhttp.h
// Purpose:     wxWebRequest WinHTTP implementation
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WEBREQUEST_WINHTTP_H
#define _WX_MSW_WEBREQUEST_WINHTTP_H

class WXDLLIMPEXP_NET wxWebSessionWinHTTP: public wxWebSession
{
public:
    wxWebSessionWinHTTP() { }

    wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY) wxOVERRIDE;

    void SetHeader(const wxString& name, const wxString& value) wxOVERRIDE;

private:

};

class WXDLLIMPEXP_NET wxWebSessionFactoryWinHTTP: public wxWebSessionFactory
{
public:
    wxWebSession* Create() wxOVERRIDE
        { return new wxWebSessionWinHTTP(); }
};

#endif // _WX_MSW_WEBREQUEST_WINHTTP_H
