/////////////////////////////////////////////////////////////////////////////
// Name:        os2/dir.cpp
// Purpose:     wxDir implementation for OS/2
// Author:      Vadim Zeitlin
// Modified by: Stefan Neis
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

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxMatchWild

#include <sys/types.h>

#ifdef __EMX__
#include <dirent.h>
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

    bool IsOk() const { return m_dir != NULL; }

    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    void Rewind() { rewinddir(m_dir); }
    bool Read(wxString *filename);

private:
    DIR     *m_dir;

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
    m_dir = NULL;

    // throw away the trailing slashes
    size_t n = m_dirname.length();
    wxCHECK_RET( n, _T("empty dir name in wxDir") );

    while ( n > 0 && m_dirname[--n] == '/' )
        ;

    m_dirname.Truncate(n + 1);

    // do open the dir
    m_dir = opendir(m_dirname.fn_str());
}

wxDirData::~wxDirData()
{
    if ( m_dir )
    {
        if ( closedir(m_dir) != 0 )
        {
            wxLogLastError(_T("closedir"));
        }
    }
}

bool wxDirData::Read(wxString *filename)
{
    dirent *de;
    bool matches = FALSE;

    while ( !matches )
    {
        de = readdir(m_dir);
        if ( !de )
            return FALSE;

        // don't return "." and ".." unless asked for
        if ( de->d_name[0] == '.' &&
             ((de->d_name[1] == '.' && de->d_name[2] == '\0') ||
              (de->d_name[1] == '\0')) )
        {
            if ( !(m_flags & wxDIR_DOTDOT) )
                continue;
        }

        // check the type now
        if ( !(m_flags & wxDIR_FILES) &&
             !wxDir::Exists(m_dirname + _T('/') + de->d_name) )
        {
            // it's a file, but we don't want them
            continue;
        }
        else if ( !(m_flags & wxDIR_DIRS) &&
                  wxDir::Exists(m_dirname + _T('/') + de->d_name) )
        {
            // it's a dir, and we don't want it
            continue;
        }

        // finally, check the name
        if ( !m_filespec )
        {
            matches = m_flags & wxDIR_HIDDEN ? TRUE : de->d_name[0] != '.';
        }
        else
        {
            // test against the pattern
            matches = wxMatchWild(m_filespec, de->d_name,
                                  !(m_flags & wxDIR_HIDDEN));
        }
    }

    *filename = de->d_name;

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

    if ( !M_DIR->IsOk() )
    {
        wxLogSysError(_("Can not enumerate files in directory '%s'"),
                      dirname.c_str());

        delete M_DIR;
        m_data = NULL;

        return FALSE;
    }

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
