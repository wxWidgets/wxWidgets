/////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/dir.cpp
// Purpose:     wxDir implementation for Symbian
// Author:      Jordan Langholz
// Modified by: Andrei Matuk <Veon.UA@gmail.com>
// Created:     10.13.04
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include "wx/filefn.h"          // for wxDirExists()
#include "f32file.h"            // Must link with efsrv.lib

// ----------------------------------------------------------------------------
// define the types and functions used for file searching
// ----------------------------------------------------------------------------

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

    const wxString& GetName() const { return m_dirname; }

private:
    RFs           m_fs;
    TFindFile *m_finddata;
    CDir      *m_matches;
    int           m_nextmatch;

    wxString m_dirname;
    wxString m_filespec;

    int      m_flags;

    DECLARE_NO_COPY_CLASS(wxDirData)
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
    m_fs.Connect();
    m_finddata = new TFindFile(m_fs);
    m_matches = NULL;
    m_nextmatch = 0;
}

wxDirData::~wxDirData()
{
    delete m_finddata;
    delete m_matches;
    m_fs.Close();
    Close();
}

void wxDirData::Close()
{
}

void wxDirData::Rewind()
{
    delete m_matches;
    m_matches = NULL;
    m_nextmatch = 0;

    TBuf<255> filespec;
    TBuf<255> dirname;

    if (m_filespec.IsEmpty())
        filespec.Copy((TUint16 *)wxT("*"));
    else
        filespec.Copy((TUint16 *) (m_filespec.c_str().AsWChar()));
    dirname.Copy((TUint16 *) (m_dirname.c_str().AsWChar()) );

    m_finddata->FindWildByPath(filespec, &dirname, m_matches);
}

bool wxDirData::Read(wxString *filename)
{
    while (m_nextmatch < m_matches->Count())
    {
        const TEntry thisfile = (*m_matches)[m_nextmatch];
        m_nextmatch ++;

        // check the type now
        if ( !(m_flags & wxDIR_FILES) && !thisfile.IsDir() )
        {
            // it's a file, but we don't want them
            continue;
        }
        else if ( !(m_flags & wxDIR_DIRS) && thisfile.IsDir() )
        {
            // it's a dir, and we don't want it
            continue;
        }

        // finally, check whether it's a hidden file
        if ( !(m_flags & wxDIR_HIDDEN) && thisfile.IsHidden() )
        {
            // it's a hidden file, skip it
            continue;
        }

        *filename = m_dirname + wxString((wxChar *)thisfile.iName.Ptr(), thisfile.iName.Length());
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(const wxString& dir)
{
    return wxDirExists(dir);
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(const wxString& dirname)
{
    m_data = NULL;
    Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxDirData(dirname);
    return true;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
}

wxString wxDir::GetName() const
{
    return M_DIR->GetName();
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
    wxCHECK_MSG( IsOpened(), false, wxT("must wxDir::Open() first") );

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);
    M_DIR->Rewind();

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), false, wxT("must wxDir::Open() first") );
    wxCHECK_MSG( filename, false, wxT("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}

