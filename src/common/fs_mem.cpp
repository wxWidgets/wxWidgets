/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fs_mem.cpp
// Purpose:     in-memory file system
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILESYSTEM && wxUSE_STREAMS

#include "wx/fs_mem.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/hashmap.h"
    #include "wx/wxcrtvararg.h"
    #if wxUSE_GUI
        #include "wx/image.h"
    #endif // wxUSE_GUI
#endif

#include "wx/mstream.h"

class MemFSHashObj
{
public:
    MemFSHashObj(const void *data, size_t len, const wxString& mime)
    {
        m_Data = new char[len];
        memcpy(m_Data, data, len);
        m_Len = len;
        m_MimeType = mime;
        InitTime();
    }

    MemFSHashObj(const wxMemoryOutputStream& stream, const wxString& mime)
    {
        m_Len = stream.GetSize();
        m_Data = new char[m_Len];
        stream.CopyTo(m_Data, m_Len);
        m_MimeType = mime;
        InitTime();
    }

    virtual ~MemFSHashObj()
    {
        delete[] m_Data;
    }

    char *m_Data;
    size_t m_Len;
    wxString m_MimeType;
#if wxUSE_DATETIME
    wxDateTime m_Time;
#endif // wxUSE_DATETIME

DECLARE_NO_COPY_CLASS(MemFSHashObj)

private:
    void InitTime()
    {
#if wxUSE_DATETIME
        m_Time = wxDateTime::Now();
#endif // wxUSE_DATETIME
    }
};

WX_DECLARE_STRING_HASH_MAP(MemFSHashObj *, wxMemoryFSHash);

#if wxUSE_BASE


//--------------------------------------------------------------------------------
// wxMemoryFSHandler
//--------------------------------------------------------------------------------


wxMemoryFSHash *wxMemoryFSHandlerBase::m_Hash = NULL;


wxMemoryFSHandlerBase::wxMemoryFSHandlerBase() : wxFileSystemHandler()
{
}

wxMemoryFSHandlerBase::~wxMemoryFSHandlerBase()
{
    // as only one copy of FS handler is supposed to exist, we may silently
    // delete static data here. (There is no way how to remove FS handler from
    // wxFileSystem other than releasing _all_ handlers.)

    if (m_Hash)
    {
        WX_CLEAR_HASH_MAP(wxMemoryFSHash, *m_Hash);
        delete m_Hash;
        m_Hash = NULL;
    }
}

bool wxMemoryFSHandlerBase::CanOpen(const wxString& location)
{
    return GetProtocol(location) == "memory";
}

wxFSFile * wxMemoryFSHandlerBase::OpenFile(wxFileSystem& WXUNUSED(fs),
                                           const wxString& location)
{
    if ( !m_Hash )
        return NULL;

    wxMemoryFSHash::const_iterator i = m_Hash->find(GetRightLocation(location));
    if ( i == m_Hash->end() )
        return NULL;

    const MemFSHashObj * const obj = i->second;

    return new wxFSFile
               (
                    new wxMemoryInputStream(obj->m_Data, obj->m_Len),
                    location,
                    obj->m_MimeType,
                    GetAnchor(location)
#if wxUSE_DATETIME
                    , obj->m_Time
#endif // wxUSE_DATETIME
               );
}

wxString wxMemoryFSHandlerBase::FindFirst(const wxString& WXUNUSED(spec),
                                          int WXUNUSED(flags))
{
    wxFAIL_MSG(wxT("wxMemoryFSHandlerBase::FindFirst not implemented"));

    return wxEmptyString;
}

wxString wxMemoryFSHandlerBase::FindNext()
{
    wxFAIL_MSG(wxT("wxMemoryFSHandlerBase::FindNext not implemented"));

    return wxEmptyString;
}

bool wxMemoryFSHandlerBase::CheckHash(const wxString& filename)
{
    if ( !m_Hash )
        m_Hash = new wxMemoryFSHash;

    if ( m_Hash->count(filename) )
    {
        wxLogError(_("Memory VFS already contains file '%s'!"), filename);
        return false;
    }

    return true;
}


/*static*/
void wxMemoryFSHandlerBase::AddFileWithMimeType(const wxString& filename,
                                                const wxString& textdata,
                                                const wxString& mimetype)
{
    AddFileWithMimeType(filename,
                        (const void*) textdata.mb_str(), textdata.length(),
                        mimetype);
}


/*static*/
void wxMemoryFSHandlerBase::AddFileWithMimeType(const wxString& filename,
                                                const void *binarydata, size_t size,
                                                const wxString& mimetype)
{
    if ( !CheckHash(filename) )
        return;

    (*m_Hash)[filename] = new MemFSHashObj(binarydata, size, mimetype);
}

/*static*/
void wxMemoryFSHandlerBase::AddFile(const wxString& filename,
                                    const wxString& textdata)
{
    AddFileWithMimeType(filename, textdata, wxEmptyString);
}


/*static*/
void wxMemoryFSHandlerBase::AddFile(const wxString& filename,
                                    const void *binarydata, size_t size)
{
    AddFileWithMimeType(filename, binarydata, size, wxEmptyString);
}



/*static*/ void wxMemoryFSHandlerBase::RemoveFile(const wxString& filename)
{
    if ( m_Hash )
    {
        wxMemoryFSHash::iterator i = m_Hash->find(filename);
        if ( i != m_Hash->end() )
        {
            delete i->second;
            m_Hash->erase(i);
        }
    }

    wxLogError(_("Trying to remove file '%s' from memory VFS, "
                 "but it is not loaded!"),
               filename);
}

#endif // wxUSE_BASE

#if wxUSE_GUI

#if wxUSE_IMAGE
/*static*/ void
wxMemoryFSHandler::AddFile(const wxString& filename,
                           const wxImage& image,
                           wxBitmapType type)
{
    if ( !CheckHash(filename) )
        return;

    wxMemoryOutputStream mems;
    if ( image.Ok() && image.SaveFile(mems, type) )
    {
        (*m_Hash)[filename] = new MemFSHashObj
                                  (
                                    mems,
                                    wxImage::FindHandler(type)->GetMimeType()
                                  );
    }
    else
    {
        wxLogError(_("Failed to store image '%s' to memory VFS!"), filename);
    }
}

/*static*/ void
wxMemoryFSHandler::AddFile(const wxString& filename,
                           const wxBitmap& bitmap,
                           wxBitmapType type)
{
    wxImage img = bitmap.ConvertToImage();
    AddFile(filename, img, type);
}

#endif // wxUSE_IMAGE

#endif // wxUSE_GUI


#endif // wxUSE_FILESYSTEM && wxUSE_FS_ZIP
