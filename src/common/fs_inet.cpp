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

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if !wxUSE_SOCKETS
    #undef wxUSE_FS_INET
    #define wxUSE_FS_INET 0
#endif

#if wxUSE_FILESYSTEM && wxUSE_FS_INET

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/filesys.h"
#include "wx/fs_inet.h"
#include "wx/module.h"

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


static wxString StripProtocolAnchor(const wxString& location)
{
    wxString myloc(location.BeforeLast(wxT('#')));
    if (myloc.IsEmpty()) myloc = location.AfterFirst(wxT(':'));
    else myloc = myloc.AfterFirst(wxT(':'));

    // fix malformed url:
    if (myloc.Left(2) != wxT("//")) 
    {
        if (myloc.GetChar(0) != wxT('/')) myloc = wxT("//") + myloc;
        else myloc = wxT("/") + myloc;
    }
    if (myloc.Mid(2).Find(wxT('/')) == wxNOT_FOUND) myloc << wxT('/');

    return myloc;
}



bool wxInternetFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    if ((p == wxT("http")) || (p == wxT("ftp"))) 
    {
        wxURL url(p + wxT(":") + StripProtocolAnchor(location));
        return (url.GetError() == wxURL_NOERR);
    }
    else 
        return FALSE;
}


wxFSFile* wxInternetFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetProtocol(location) + wxT(":") + StripProtocolAnchor(location);
    wxInputStream *s;
    wxString content;
    wxInetCacheNode *info;

    info = (wxInetCacheNode*) m_Cache.Get(right);

    // Add item into cache:
    if (info == NULL)
    {
        wxURL url(right);
        if (url.GetError() == wxURL_NOERR) 
        {
            s = url.GetInputStream();
            content = url.GetProtocol().GetContentType();
            if (content == wxEmptyString) content = GetMimeTypeFromExt(location);
            if (s)
            {
                wxChar buf[256];

                wxGetTempFileName( wxT("wxhtml"), buf);
                info = new wxInetCacheNode(buf, content);
                m_Cache.Put(right, info);

                {   // ok, now copy it:
                    wxFileOutputStream sout((wxString)buf);
                    s -> Read(sout); // copy the stream
                }
                delete s;
            }
            else
                return (wxFSFile*) NULL; // we can't open the URL
        }
        else
            return (wxFSFile*) NULL; // incorrect URL
    }

    // Load item from cache:
    s = new wxFileInputStream(info->GetTemp());
    if (s)
    {
        return new wxFSFile(s,
                            right,
                            info->GetMime(),
                            GetAnchor(location),
                            wxDateTime::Now());
    }
    else return (wxFSFile*) NULL;
}



wxInternetFSHandler::~wxInternetFSHandler()
{
    wxNode *n;
    wxInetCacheNode *n2;

    m_Cache.BeginFind();
    while ((n = m_Cache.Next()) != NULL)
    {
        n2 = (wxInetCacheNode*) n->GetData();
        wxRemoveFile(n2->GetTemp());
        delete n2;
    }
}

class wxFileSystemInternetModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxFileSystemInternetModule)

    public:
        virtual bool OnInit()
        {
            wxFileSystem::AddHandler(new wxInternetFSHandler);
            return TRUE;
        }
        virtual void OnExit() {}
};

IMPLEMENT_DYNAMIC_CLASS(wxFileSystemInternetModule, wxModule)

#endif // wxUSE_FILESYSTEM && wxUSE_FS_INET
