/////////////////////////////////////////////////////////////////////////////
// Name:        webviewfilehandler.cpp
// Purpose:     Custom handler for the file scheme to allow archive browsing
// Author:      Steven Lamerton
// Id:          $Id$
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEB

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#include "wx/webviewfilehandler.h"
#include "wx/filesys.h"

//Taken from wx/filesys.cpp
static wxString EscapeFileNameCharsInURL(const char *in)
{
    wxString s;

    for ( const unsigned char *p = (const unsigned char*)in; *p; ++p )
    {
        const unsigned char c = *p;

        if ( c == '/' || c == '-' || c == '.' || c == '_' || c == '~' ||
             (c >= '0' && c <= '9') ||
             (c >= 'a' && c <= 'z') ||
             (c >= 'A' && c <= 'Z') )
        {
            s << c;
        }
        else
        {
            s << wxString::Format("%%%02x", c);
        }
    }

    return s;
}

wxWebFileHandler::wxWebFileHandler()
{
    m_name = "test";
    m_fileSystem = new wxFileSystem();
}

wxFSFile* wxWebFileHandler::GetFile(const wxString &uri)
{
    //We iterate through the string to see if there is a protocol description
    int start = -1;
    for(int i = 0; i < uri.length(); i++)
    {
        if(uri[i] == ';' && uri.substr(i, 10) == ";protocol=")
        {
            start = i;
            break;
        }
    }

    //We do not have a protocol string so we just pass the path withouth the 
    if(start == -1)
    {
        size_t doubleslash = uri.find("//");
        //The path is incorrectly formed without // after the scheme
        if(doubleslash == wxString::npos)
            return NULL;

        wxString fspath = "file:" + 
                          EscapeFileNameCharsInURL(uri.substr(doubleslash + 2));
        return m_fileSystem->OpenFile(fspath);
    }
    //Otherwise we need to extract the protocol
    else
    {
        int end = uri.find('/', start);
        //For the path to be valid there must to a path after the protocol
        if(end == wxString::npos)
        {
            return NULL;
        }
        wxString mainpath = uri.substr(0, start);
        wxString archivepath = uri.substr(end);
        wxString protstring = uri.substr(start, end - start);
        wxString protocol = protstring.substr(10);
        //We can now construct the correct path
        size_t doubleslash = uri.find("//");
        //The path is incorrectly formed without // after the first protocol
        if(doubleslash == wxString::npos)
            return NULL;

        wxString fspath = "file:" + 
                          EscapeFileNameCharsInURL(mainpath.substr(doubleslash + 2))
                          + "#" + protocol +":" + archivepath;
        return m_fileSystem->OpenFile(fspath);
    }
}

#endif // wxUSE_WEB
