/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fs_inet.cpp
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if !wxUSE_SOCKETS
    #undef wxUSE_FS_INET
    #define wxUSE_FS_INET 0
#endif

#if wxUSE_FILESYSTEM && wxUSE_FS_INET

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif

#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/filesys.h"
#include "wx/fs_inet.h"

// ----------------------------------------------------------------------------
// Helper classes
// ----------------------------------------------------------------------------

// This stream deletes the file when destroyed
class wxTemporaryFileInputStream : public wxFileInputStream
{
public:
    wxTemporaryFileInputStream(const wxString& filename) :
        wxFileInputStream(filename), m_filename(filename) {}

    virtual ~wxTemporaryFileInputStream()
    {
        // NB: copied from wxFileInputStream dtor, we need to do it before
        //     wxRemoveFile
        if (m_file_destroy)
        {
            delete m_file;
            m_file_destroy = false;
        }
        wxRemoveFile(m_filename);
    }

protected:
    wxString m_filename;
};


// ----------------------------------------------------------------------------
// wxInternetFSHandler
// ----------------------------------------------------------------------------

static wxString StripProtocolAnchor(const wxString& location)
{
    wxString myloc(location.BeforeLast(wxT('#')));
    if (myloc.empty()) myloc = location.AfterFirst(wxT(':'));
    else myloc = myloc.AfterFirst(wxT(':'));

    // fix malformed url:
    if (!myloc.Left(2).IsSameAs(wxT("//")))
    {
        if (myloc.GetChar(0) != wxT('/')) myloc = wxT("//") + myloc;
        else myloc = wxT("/") + myloc;
    }
    if (myloc.Mid(2).Find(wxT('/')) == wxNOT_FOUND) myloc << wxT('/');

    return myloc;
}


bool wxInternetFSHandler::CanOpen(const wxString& location)
{
#if wxUSE_URL
    wxString p = GetProtocol(location);
    if ((p == wxT("http")) || (p == wxT("ftp")))
    {
        wxURL url(p + wxT(":") + StripProtocolAnchor(location));
        return (url.GetError() == wxURL_NOERR);
    }
#endif
    return false;
}


wxFSFile* wxInternetFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs),
                                        const wxString& location)
{
#if !wxUSE_URL
    return NULL;
#else
    wxString right =
        GetProtocol(location) + wxT(":") + StripProtocolAnchor(location);

    wxURL url(right);
    if (url.GetError() == wxURL_NOERR)
    {
        wxInputStream *s = url.GetInputStream();
        if (s)
        {
            wxString tmpfile =
                wxFileName::CreateTempFileName(wxT("wxhtml"));

            {   // now copy streams content to temporary file:
                wxFileOutputStream sout(tmpfile);
                s->Read(sout);
            }
            delete s;

            // Content-Type header, as defined by the RFC 2045, has the form of
            // "type/subtype" optionally followed by (multiple) "; parameter"
            // and we need just the MIME type here.
            const wxString& content = url.GetProtocol().GetContentType();
            wxString mimetype = content.BeforeFirst(';');
            mimetype.Trim();

            return new wxFSFile(new wxTemporaryFileInputStream(tmpfile),
                                right,
                                mimetype,
                                GetAnchor(location)
#if wxUSE_DATETIME
                                , wxDateTime::Now()
#endif // wxUSE_DATETIME
                        );
        }
    }

    return NULL; // incorrect URL
#endif
}


class wxFileSystemInternetModule : public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxFileSystemInternetModule);

    public:
        wxFileSystemInternetModule() :
           wxModule(),
           m_handler(NULL)
        {
        }

        virtual bool OnInit() wxOVERRIDE
        {
            m_handler = new wxInternetFSHandler;
            wxFileSystem::AddHandler(m_handler);
            return true;
        }

        virtual void OnExit() wxOVERRIDE
        {
            delete wxFileSystem::RemoveHandler(m_handler);
        }

    private:
        wxFileSystemHandler* m_handler;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxFileSystemInternetModule, wxModule);

#endif // wxUSE_FILESYSTEM && wxUSE_FS_INET
