/////////////////////////////////////////////////////////////////////////////
// Name:        fs_inet.cpp
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*

REMARKS : 

This FS creates local cache (in /tmp directory). The cache is freed
on program exit.

Size of cache is limited to cca 1000 items (due to GetTempFileName
limitation)


*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/filesys.h"
#include "wx/fs_inet.h"

class wxInetCacheNode : public wxObject
{
    private:
        wxString m_Temp;
        wxString m_Mime;

    public:
        wxInetCacheNode(const wxString& l, const wxString& m) : wxObject() {m_Temp = l; m_Mime = m;}
        const wxString& GetTemp() const {return m_Temp;}
        const wxString& GetMime() const {return m_Mime;}
};





//--------------------------------------------------------------------------------
// wxInternetFSHandler
//--------------------------------------------------------------------------------


bool wxInternetFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == "http") || (p == "ftp");
}




wxFSFile* wxInternetFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetProtocol(location) + ":" + GetRightLocation(location);
    wxInputStream *s;
    wxString content;
    wxInetCacheNode *info;

    info = (wxInetCacheNode*) m_Cache.Get(right);

    // Add item into cache:
    if (info == NULL) {
        wxURL url(right);
        s = url.GetInputStream();
        content = url.GetProtocol().GetContentType();
        if (content == wxEmptyString) content = GetMimeTypeFromExt(location);
        if (s) {
            char buf[256];

            wxGetTempFileName("wxhtml", buf);
            info = new wxInetCacheNode(buf, content);
            m_Cache.Put(right, info);

            {   // ok, now copy it:
                wxFileOutputStream sout(buf);
                s -> Read(sout); // copy the stream
            }
            delete s;
        }
        else return NULL; //we can't open the URL
    }

    // Load item from cache:
    s = new wxFileInputStream(info -> GetTemp());
    if (s) {
        return new wxFSFile(s,
                            right,
                            info -> GetMime(),
                            GetAnchor(location));
    }
    else return NULL;
}



wxInternetFSHandler::~wxInternetFSHandler()
{
    wxNode *n;
    wxInetCacheNode *n2;

    m_Cache.BeginFind();
    while ((n = m_Cache.Next()) != NULL) {
        n2 = (wxInetCacheNode*) n -> GetData();
        wxRemoveFile(n2 -> GetTemp());
        delete n2;
    }
}




