/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dir.h
// Purpose:     wxDir is a class for enumerating the files in a directory
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIR_H_
#define _WX_DIR_H_

#ifdef __GNUG__
    #pragma interface "dir.h"
#endif

#ifndef WX_PRECOMP
    #include  "wx/string.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// these flags define what kind of filenames is included in the list of files
// enumerated by GetFirst/GetNext
enum
{
    wxDIR_FILES     = 0x0001,       // include files
    wxDIR_DIRS      = 0x0002,       // include directories
    wxDIR_HIDDEN    = 0x0004,       // include hidden files
    wxDIR_DOTDOT    = 0x0008,       // include '.' and '..'

    // by default, enumerate everything except '.' and '..'
    wxDIR_DEFAULT   = wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN
};

// ----------------------------------------------------------------------------
// wxDir: portable equivalent of {open/read/close}dir functions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDir
{
public:
    // test for existence of a directory with the given name
    static bool Exists(const wxString& dir);

    // ctors
    // -----

    // default, use Open()
    wxDir() { m_data = NULL; }

    // opens the directory for enumeration, use IsOpened() to test success
    wxDir(const wxString& dir);

    // dtor cleans up the associated ressources
    ~wxDir();

    // open the directory for enumerating
    bool Open(const wxString& dir);

    // returns TRUE if the directory was successfully opened
    bool IsOpened() const;

    // file enumeration routines
    // -------------------------

    // start enumerating all files matching filespec (or all files if it is
    // empty) and flags, return TRUE on success
    bool GetFirst(wxString *filename,
                  const wxString& filespec = wxEmptyString,
                  int flags = wxDIR_DEFAULT) const;

    // get next file in the enumeration started with either GetFirst() or
    // GetFirstNormal()
    bool GetNext(wxString *filename) const;

    // TODO using scandir() when available later, emulating it otherwise
#if 0
    // get all files in the directory into an array, return TRUE on success
    //
    // this function uses Select() function to select the files
    // unless the filespec is explicitly given and Compare() function to sort
    // them
    bool Read(wxArrayString& filenames,
              const wxString& filespec = wxEmptyString) const;

protected:
    // this function is called by Read() if filespec is not specified in
    // Read(): it should return TRUE if the file matches our selection
    // criteria and FALSE otherwise
    virtual bool Select(const wxChar* filename);

    // This function is called by Read() to sort the array: it should return
    // -1, 0 or +1 if the first file is less than, equal to or greater than
    // the second. The base class version does 
    virtual int Compare(const wxChar *filename1, const wxChar *filename2);
#endif // 0

private:
    friend class WXDLLEXPORT wxDirData;

    wxDirData *m_data;
};

#endif // _WX_DIR_H_
