/////////////////////////////////////////////////////////////////////////////
// Name:        fs_mem.h
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

#if (wxUSE_HTML || wxUSE_FS_INET || wxUSE_FS_ZIP) && wxUSE_STREAMS

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"
#include "wx/image.h"
#include "wx/bitmap.h"

//--------------------------------------------------------------------------------
// wxMemoryFSHandler
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxMemoryFSHandler : public wxFileSystemHandler
{
    public:
        wxMemoryFSHandler();
        ~wxMemoryFSHandler();

        // Add file to list of files stored in memory. Stored data (bitmap, text or raw data)
        // will be copied into private memory stream and available under name "memory:" + filename
        static void AddFile(const wxString& filename, wxImage& image, long type);
        static void AddFile(const wxString& filename, const wxBitmap& bitmap, long type);
        static void AddFile(const wxString& filename, const wxString& textdata);        
        static void AddFile(const wxString& filename, const void *binarydata, size_t size);
        
        // Remove file from memory FS and free occupied memory
        static void RemoveFile(const wxString& filename);
        
        virtual bool CanOpen(const wxString& location);
        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
        virtual wxString FindFirst(const wxString& spec, int flags = 0);
        virtual wxString FindNext();
        
    private:
        static wxHashTable *m_Hash;
        
        static bool CheckHash(const wxString& filename);
};

#endif 
  // (wxUSE_HTML || wxUSE_FS_INET || wxUSE_FS_ZIP) && wxUSE_STREAMS


