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
#include "wx/hashmap.h"

typedef wxStringToStringHashMap QueryMap;

QueryMap QueryStringToQueryMap(wxString query)
{
    QueryMap map;

    if(query.substr(0, 1) == "?")
        query = query.substr(1);

    wxStringTokenizer tokenizer(query, ";");
    while(tokenizer.HasMoreTokens())
    {
        wxString token = tokenizer.GetNextToken();
        size_t pos = token.find('=');
        map[token.substr(0, pos)] = token.substr(pos + 1);
    }
    return map;
}

wxString QueryMapToQueryString(QueryMap map)
{
    wxString query = "?";

    QueryMap::iterator it;
    for(it = map.begin(); it != map.end(); ++it)
    {
        query = query + it->first + "=" + it->second + ";";
    }

    //Chop the end ; off
    return query.substr(0, query.length() - 1);
}

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
        wxString lefturi = uri.substr(0, pos);

        //We extract the query parts that we need
        QueryMap map = QueryStringToQueryMap(uri.substr(pos));
        wxString protocol = map["protocol"], path = map["path"];

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
        QueryMap map = QueryStringToQueryMap(baseuri.substr(baseuri.find('?')));
        //As the path is absolue simply replace the old path with the new one
        map["path"] = newuri;
        wxString newquery = QueryMapToQueryString(map);
        return baseuri.substr(0, baseuri.find('?')) + newquery;
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
        QueryMap map = QueryStringToQueryMap(baseuri.substr(baseuri.find('?')));
        wxString path = map["path"];
        //Then we remove the last filename
        path = path.BeforeLast('/') + '/';

        //We can now use wxFileName to perform the normalisation
        wxFileName fn(path + newuri);
        fn.Normalize(wxPATH_NORM_DOTS, "", wxPATH_UNIX);
        map["path"] = fn.GetFullPath(wxPATH_UNIX);

        wxString newquery = QueryMapToQueryString(map);
        return baseuri.substr(0, baseuri.find('?')) + newquery;
    }
}
#endif // wxUSE_WEB
