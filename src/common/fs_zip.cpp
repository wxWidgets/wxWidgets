/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.cpp
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


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

#include "wx/filesys.h"
#include "wx/zipstream.h"
#include "wx/fs_zip.h"


//--------------------------------------------------------------------------------
// wxZipFSHandler
//--------------------------------------------------------------------------------



bool wxZipFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == "zip");
}




wxFSFile* wxZipFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetRightLocation(location);
    wxString left = GetLeftLocation(location);
    wxInputStream *s;

    if (GetProtocol(left) != "file") {
        return NULL;
    }

    s = new wxZipInputStream(left, right);
    if (s && (s -> LastError() == wxStream_NOERROR)) {
        return new wxFSFile(s,
                            left + "#zip:" + right,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location));
    }
    else return NULL;
}



wxZipFSHandler::~wxZipFSHandler()
{
}







