/////////////////////////////////////////////////////////////////////////////
// Name:        fs_mem.cpp
// Purpose:     in-memory file system
// Author:      Vaclav Slavik
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_FILESYSTEM && wxUSE_STREAMS

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"
#include "wx/fs_mem.h"
#include "wx/mstream.h"


class MemFSHashObj : public wxObject
{
    public:
        
        MemFSHashObj(const void *data, size_t len)
        {
            m_Data = new char[len];
            memcpy(m_Data, data, len);
            m_Len = len;
            m_Time = wxDateTime::Now();
        }
        
        MemFSHashObj(wxMemoryOutputStream& stream)
        {
            m_Len = stream.GetSize();
            m_Data = new char[m_Len];
            stream.CopyTo(m_Data, m_Len);
            m_Time = wxDateTime::Now();
        }
        
        ~MemFSHashObj()
        {
            delete[] m_Data;
        }
        
        char *m_Data;
        size_t m_Len;
        wxDateTime m_Time;
};


//--------------------------------------------------------------------------------
// wxMemoryFSHandler
//--------------------------------------------------------------------------------


wxHashTable *wxMemoryFSHandler::m_Hash = NULL;


wxMemoryFSHandler::wxMemoryFSHandler() : wxFileSystemHandler()
{
    m_Hash = NULL;
}



wxMemoryFSHandler::~wxMemoryFSHandler()
{
    // as only one copy of FS handler is supposed to exist, we may silently
    // delete static data here. (There is no way how to remove FS handler from
    // wxFileSystem other than releasing _all_ handlers.)
    
    if (m_Hash) delete m_Hash; 
    m_Hash = NULL;
}



bool wxMemoryFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == wxT("memory"));
}




wxFSFile* wxMemoryFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    if (m_Hash)
    {
        MemFSHashObj *obj = (MemFSHashObj*) m_Hash -> Get(GetRightLocation(location));
        if (obj == NULL)  return NULL;
        else return new wxFSFile(new wxMemoryInputStream(obj -> m_Data, obj -> m_Len),
                            location,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location),
                            obj -> m_Time);
    }
    else return NULL;
}



wxString wxMemoryFSHandler::FindFirst(const wxString& spec, int flags)
{
    wxLogWarning(wxT("wxMemoryFSHandler::FindFirst not implemented"));
    return wxEmptyString;
}



wxString wxMemoryFSHandler::FindNext()
{
    wxLogWarning(wxT("wxMemoryFSHandler::FindNext not implemented"));
    return wxEmptyString;
}



bool wxMemoryFSHandler::CheckHash(const wxString& filename)
{
    if (m_Hash == NULL) 
    {
        m_Hash = new wxHashTable(wxKEY_STRING);
        m_Hash -> DeleteContents(TRUE);
    }
    
    if (m_Hash -> Get(filename) != NULL)
    {
        wxString s;
        s.Printf(_("Memory VFS already contains file '%s'!"), filename.c_str());
        wxLogError(s);
        return FALSE;
    }
    else
        return TRUE;
}



#if wxUSE_GUI

/*static*/ void wxMemoryFSHandler::AddFile(const wxString& filename, wxImage& image, long type)
{
    if (!CheckHash(filename)) return;

    
    wxMemoryOutputStream mems;
    if (image.Ok() && image.SaveFile(mems, type))
        m_Hash -> Put(filename, new MemFSHashObj(mems));
    else
    {
        wxString s;
        s.Printf(_("Failed to store image '%s' to memory VFS!"), filename.c_str());
        printf("'%s'\n", s.c_str());
        wxLogError(s);
    }
}


/*static*/ void wxMemoryFSHandler::AddFile(const wxString& filename, const wxBitmap& bitmap, long type)
{
    wxImage img(bitmap);
    AddFile(filename, img, type);
}

#endif

/*static*/ void wxMemoryFSHandler::AddFile(const wxString& filename, const wxString& textdata)
{
    AddFile(filename, (const void*) textdata.mb_str(), textdata.Length());
}


/*static*/ void wxMemoryFSHandler::AddFile(const wxString& filename, const void *binarydata, size_t size)
{
    if (!CheckHash(filename)) return;
    m_Hash -> Put(filename, new MemFSHashObj(binarydata, size));
}



/*static*/ void wxMemoryFSHandler::RemoveFile(const wxString& filename)
{
    if (m_Hash == NULL ||
        m_Hash -> Get(filename) == NULL)
    {
        wxString s;
        s.Printf(_("Trying to remove file '%s' from memory VFS, but it is not loaded!"), filename.c_str());
        wxLogError(s);
    }

    else
        delete m_Hash -> Delete(filename);
}



#endif // wxUSE_FILESYSTEM && wxUSE_FS_ZIP
