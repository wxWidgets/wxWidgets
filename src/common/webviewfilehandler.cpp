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
#include "wx/tokenzr.h"

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
    size_t pos = uri.find('?');
    //There is no query string so we can load the file directly
    if(pos == wxString::npos)
    {
        size_t doubleslash = uri.find("//");
        //The path is incorrectly formed without // after the first protocol
        if(doubleslash == wxString::npos)
            return NULL;

        wxString fspath = "file:" + 
                          EscapeFileNameCharsInURL(uri.substr(doubleslash + 2));
        return m_fileSystem->OpenFile(fspath);
    }
    //Otherwise we have a query string of some kind that we need to extract
    else{
        //First we extract the query string, this should have two parameters, 
        //protocol=type and path=path
        wxString query = uri.substr(pos + 1), protocol, path;
        //We also trim the query off the end as we handle it alone
        wxString lefturi = uri.substr(0, pos);
        wxStringTokenizer tokenizer(query, ";");
        while(tokenizer.HasMoreTokens() && (protocol == "" || path == ""))
        {
            wxString token = tokenizer.GetNextToken();
            if(token.substr(0, 9) == "protocol=")
            {
                protocol = token.substr(9);
            }
            else if(token.substr(0, 5) == "path=")
            {
                path = token.substr(5);
            }
        }
        if(protocol == "" || path == "")
            return NULL;

        //We now have the path and the protocol and so can format a correct uri
        //to pass to wxFileSystem to get a wxFSFile
        size_t doubleslash = uri.find("//");
        //The path is incorrectly formed without // after the first protocol
        if(doubleslash == wxString::npos)
            return NULL;

        wxString fspath = "file:" + 
                          EscapeFileNameCharsInURL(lefturi.substr(doubleslash + 2))
                          + "#" + protocol +":" + path;
        return m_fileSystem->OpenFile(fspath);
    }
}

wxString wxWebFileHandler::CombineURIs(const wxString &baseuri, 
                                               const wxString &newuri)
{
    //If there is a colon in the path then we just return it
    if(newuri.find(':') != wxString::npos)
    {
        return newuri;
    }
    //We have an absolute path and no query string
    else if(newuri.substr(0, 1) == "/" && baseuri.find('?') == wxString::npos)
    {
        //By finding the next / after file:// we get to the end of the 
        //(optional) hostname
        size_t pos = baseuri.find('/', 7);
        //So we return up to the end of the hostname, plus the newuri
        return baseuri.substr(0, pos) + newuri;
    }
    //We have an absolute path and a query string
    else if(newuri.substr(0, 1) == "/" && baseuri.find('?') != wxString::npos)
    {
        wxString query = baseuri.substr(baseuri.find('?') + 1);
        wxString newquery;
        wxStringTokenizer tokenizer(query, ";");
        while(tokenizer.HasMoreTokens())
        {
            wxString token = tokenizer.GetNextToken();
            if(token.substr(0, 5) == "path=")
            {
                //As the path is absolue simply replace the old path with the
                //new one
                newquery = newquery + "path=" + newuri;
            }
            else
            {
                newquery += token;
            }
            //We need to add the separators back
            if(tokenizer.HasMoreTokens())
                newquery += ';';
        }
        return baseuri.substr(0, baseuri.find('?')) + "?" + newquery;
    }
    //We have a relative path and no query string
    else if(baseuri.find('?') == wxString::npos)
    {
        //By finding the next / after file:// we get to the end of the 
        //(optional) hostname
        size_t pos = baseuri.find('/', 7);
        wxString path = baseuri.substr(pos);
        //Then we remove the last filename
        path = path.BeforeLast('/') + '/';
        //Ensure that we have the leading / so we can normalise properly
        if(path.substr(0, 1) != "/")
            path = "/" + path;

        //If we have a colon in the path (i.e. we are on windows) we need to 
        //handle it specially
        if(path.find(':') != wxString::npos)
        {
            wxFileName fn(path.AfterFirst('/').AfterFirst('/') + newuri);
            fn.Normalize(wxPATH_NORM_DOTS, "", wxPATH_UNIX);
            return baseuri.substr(0, pos) + '/' + 
                   path.AfterFirst('/').BeforeFirst('/') + '/' + 
                   fn.GetFullPath(wxPATH_UNIX);
        }
        else
        {
            //We can now use wxFileName to perform the normalisation
            wxFileName fn(path + newuri);
            fn.Normalize(wxPATH_NORM_DOTS, "", wxPATH_UNIX);
            return baseuri.substr(0, pos) + fn.GetFullPath(wxPATH_UNIX);
        }
    }
    //We have a relative path and a query string
    else
    {
        wxString query = baseuri.substr(baseuri.find('?') + 1);
        wxString newquery;
        wxStringTokenizer tokenizer(query, ";");
        while(tokenizer.HasMoreTokens())
        {
            wxString token = tokenizer.GetNextToken();
            if(token.substr(0, 5) == "path=")
            {
                wxString path = token.substr(6);
                //Then we remove the last filename
                path = path.BeforeLast('/') + '/';
                //Ensure that we have the leading / so we can normalise properly
                //if(path.substr(0, 1) != "/")
                //    path = "/" + path;

                //We can now use wxFileName to perform the normalisation
                wxFileName fn(path + newuri);
                fn.Normalize(wxPATH_NORM_DOTS, "", wxPATH_UNIX);
                newquery = newquery + "path=" + fn.GetFullPath(wxPATH_UNIX);
            }
            else
            {
                newquery += token;
            }
            //We need to add the separators back
            if(tokenizer.HasMoreTokens())
                newquery += ';';
        }
        return baseuri.substr(0, baseuri.find('?')) + "?" + newquery;
    }
}
#endif // wxUSE_WEB
