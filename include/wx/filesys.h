/////////////////////////////////////////////////////////////////////////////
// Name:        filesys.h
// Purpose:     class for opening files - virtual file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __FILESYS_H__
#define __FILESYS_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/stream.h>
#include <wx/mimetype.h>
#include <wx/url.h>


class wxFSFile;
class wxFileSystemHandler;
class wxFileSystem;

//--------------------------------------------------------------------------------
// wxFSFile
//                  This class is a file opened using wxFileSystem. It consists of
//                  input stream, location, mime type & optional anchor
//                  (in 'index.htm#chapter2', 'chapter2' is anchor)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxFSFile : public wxObject
{
    private:
        wxInputStream *m_Stream;
        wxString m_Location;
        wxString m_MimeType;
        wxString m_Anchor;

    public:
        wxFSFile(wxInputStream *stream, const wxString& loc, const wxString& mimetype, const wxString& anchor)
            {
                m_Stream = stream;
                m_Location = loc;
                m_MimeType = mimetype; m_MimeType.MakeLower();
                m_Anchor = anchor;
            }
        virtual ~wxFSFile()
            {
                if (m_Stream) delete m_Stream;
            }

        wxInputStream *GetStream() const {return m_Stream;}
                // returns stream. This doesn't _create_ stream, it only returns
                // pointer to it!!

        const wxString& GetMimeType() const {return m_MimeType;}
                // returns file's mime type

        const wxString& GetLocation() const {return m_Location;}
                // returns the original location (aka filename) of the file

        const wxString& GetAnchor() const {return m_Anchor;}
};





//--------------------------------------------------------------------------------
// wxFileSystemHandler
//                  This class is FS handler for wxFileSystem. It provides
//                  interface to access certain
//                  kinds of files (HTPP, FTP, local, tar.gz etc..)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxFileSystemHandler : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxFileSystemHandler)

    public:
        wxFileSystemHandler() : wxObject() {}

        virtual bool CanOpen(const wxString& location) = 0;
                // returns TRUE if this handler is able to open given location

        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location) = 0;
                // opens given file and returns pointer to input stream.
                // Returns NULL if opening failed.
                // The location is always absolute path.

    protected:
        wxString GetProtocol(const wxString& location) const;
                // returns protocol ("file", "http", "tar" etc.) The last (most right)
                // protocol is used:
                // {it returns "tar" for "file:subdir/archive.tar.gz#tar:/README.txt"}

        wxString GetLeftLocation(const wxString& location) const;
                // returns left part of address:
                // {it returns "file:subdir/archive.tar.gz" for "file:subdir/archive.tar.gz#tar:/README.txt"}

        wxString GetAnchor(const wxString& location) const;
                // returns anchor part of address:
                // {it returns "anchor" for "file:subdir/archive.tar.gz#tar:/README.txt#anchor"}
                // NOTE:  anchor is NOT a part of GetLeftLocation()'s return value

        wxString GetRightLocation(const wxString& location) const;
                // returns right part of address:
                // {it returns "/README.txt" for "file:subdir/archive.tar.gz#tar:/README.txt"}

        wxString GetMimeTypeFromExt(const wxString& location);
                // Returns MIME type of the file - w/o need to open it
                // (default behaviour is that it returns type based on extension)

    private:
        static wxMimeTypesManager m_MimeMng;
                // MIME manager
        // (it's static and thus shared by all instances and derived classes)
};




//--------------------------------------------------------------------------------
// wxFileSystem
//                  This class provides simple interface for opening various
//                  kinds of files (HTPP, FTP, local, tar.gz etc..)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxFileSystem : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxFileSystem)

    private:
        wxString m_Path;
                // the path (location) we are currently in
                // this is path, not file!
                // (so if you opened test/demo.htm, it is
                // "test/", not "test/demo.htm")
        wxString m_LastName;
                // name of last opened file (full path)
        static wxList m_Handlers;
                // list of FS handlers

    public:
        wxFileSystem() : wxObject() {m_Path = m_LastName = wxEmptyString; m_Handlers.DeleteContents(TRUE);}

        void ChangePathTo(const wxString& location, bool is_dir = FALSE);
                // sets the current location. Every call to OpenFile is
                // relative to this location.
                // NOTE !!
                // unless is_dir = TRUE 'location' is *not* the directory but
                // file contained in this directory
                // (so ChangePathTo("dir/subdir/xh.htm") sets m_Path to "dir/subdir/")
		
	wxString GetPath() const {return m_Path;}

        wxFSFile* OpenFile(const wxString& location);
                // opens given file and returns pointer to input stream.
                // Returns NULL if opening failed.
                // It first tries to open the file in relative scope
                // (based on ChangePathTo()'s value) and then as an absolute
                // path.

        static void AddHandler(wxFileSystemHandler *handler);
                // Adds FS handler.
                // In fact, this class is only front-end to the FS hanlers :-)
};


/*

'location' syntax:

To determine FS type, we're using standard KDE notation:
file:/absolute/path/file.htm
file:relative_path/xxxxx.html
/some/path/x.file               ('file:' is default)
http://www.gnome.org
file:subdir/archive.tar.gz#tar:/README.txt

special characters :
  ':' - FS identificator is before this char
  '#' - separator. It can be either HTML anchor ("index.html#news")
            (in case there is no ':' in the string to the right from it)
        or FS separator
            (example : http://www.wxhtml.org/wxhtml-0.1.tar.gz#tar:/include/wxhtml/filesys.h"
             this would access tgz archive stored on web)
  '/' - directory (path) separator. It is used to determine upper-level path.
        HEY! Don't use \ even if you're on Windows!

*/

#endif // __FILESYS_H__
