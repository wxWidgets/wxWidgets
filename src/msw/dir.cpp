/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dir.cpp
// Purpose:     wxDir implementation for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dir.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxPathExists()

#include <windows.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef MAX_PATH
    #define MAX_PATH 260        // from VC++ headers
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_DIR       ((wxDirData *)m_data)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxDirData
{
public:
    wxDirData(const wxString& dirname);
    ~wxDirData();

    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    void Close();
    void Rewind();
    bool Read(wxString *filename);

private:
    HANDLE   m_handle;

    wxString m_dirname;
    wxString m_filespec;

    int      m_flags;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

wxDirData::wxDirData(const wxString& dirname)
         : m_dirname(dirname)
{
    m_handle = INVALID_HANDLE_VALUE;

    // throw away the trailing slashes
    size_t n = m_dirname.length();
    wxCHECK_RET( n, _T("empty dir name in wxDir") );

    while ( n > 0 && wxIsPathSeparator(m_dirname[--n]) )
        ;

    m_dirname.Truncate(n + 1);
}

wxDirData::~wxDirData()
{
    Close();
}

void wxDirData::Close()
{
    if ( m_handle != INVALID_HANDLE_VALUE )
    {
        if ( !::FindClose(m_handle) )
        {
            wxLogLastError(_T("FindClose"));
        }
    }
}

void wxDirData::Rewind()
{
    Close();
}

bool wxDirData::Read(wxString *filename)
{
    bool first = FALSE;

    WIN32_FIND_DATA finddata;
    if ( m_handle == INVALID_HANDLE_VALUE )
    {
        // open first
        m_handle = ::FindFirstFile(!m_filespec ? _T("*.*") : m_filespec.c_str(),
                                   &finddata);

        first = TRUE;
    }

    if ( m_handle == INVALID_HANDLE_VALUE )
    {
        DWORD err = ::GetLastError();

        if ( err != ERROR_NO_MORE_FILES )
        {
            wxLogSysError(err, _("Can not enumerate files in directory '%s'"),
                          m_dirname.c_str());
        }
        //else: not an error, just no (such) files

        return FALSE;
    }

    bool matches = FALSE;

    const wxChar *name;
    DWORD attr;

    while ( !matches )
    {
        if ( first )
        {
            first = FALSE;
        }
        else
        {
            if ( !::FindNextFile(m_handle, &finddata) )
            {
                DWORD err = ::GetLastError();

                if ( err != ERROR_NO_MORE_FILES )
                {
                    wxLogLastError(_T("FindNext"));
                }
                //else: not an error, just no more (such) files

                return FALSE;
            }
        }

        name = finddata.cFileName;
        attr = finddata.dwFileAttributes;

        // don't return "." and ".." unless asked for
        if ( name[0] == _T('.') &&
             ((name[1] == _T('.') && name[2] == _T('\0')) ||
              (name[1] == _T('\0'))) )
        {
            if ( !(m_flags & wxDIR_DOTDOT) )
                continue;
        }

        // check the type now
        if ( !(m_flags & wxDIR_FILES) && !(attr & FILE_ATTRIBUTE_DIRECTORY) )
        {
            // it's a file, but we don't want them
            continue;
        }
        else if ( !(m_flags & wxDIR_DIRS) && (attr & FILE_ATTRIBUTE_DIRECTORY) )
        {
            // it's a dir, and we don't want it
            continue;
        }

        // finally, check whether it's a hidden file
        if ( !(m_flags & wxDIR_HIDDEN) )
        {
            matches = !(attr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
        }
    }

    *filename = name;

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(const wxString& dir)
{
    return wxPathExists(dir);
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(const wxString& dirname)
{
    m_data = NULL;

    (void)Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxDirData(dirname);

    return TRUE;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
}

wxDir::~wxDir()
{
    delete M_DIR;
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    M_DIR->Rewind();

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, FALSE, _T("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}
