/////////////////////////////////////////////////////////////////////////////
// Name:        mac/dirmac.cpp
// Purpose:     wxDir implementation for Mac
// Author:      Stefan Csomor
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Stefan Csomor <csomor@advanced.ch>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dir.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/filefn.h"          // for wxDirExists()
#include "wx/filename.h"
#include "wx/mac/private.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxDirData
{
public:
    wxDirData(const wxString& dirname);
    ~wxDirData();

    void Close() ;
    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    bool Read(wxString *filename); // reads the next
    void Rewind() ;

    const wxString& GetName() const { return m_dirname; }

private:
    FSIterator              m_iterator ;

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
    // throw away the trailing slashes
    size_t n = m_dirname.length();
    wxCHECK_RET( n, _T("empty dir name in wxDir") );

    while ( n > 0 && wxIsPathSeparator(m_dirname[--n]) )
        ;

    m_dirname.Truncate(n + 1);
    m_iterator = NULL ;
}

wxDirData::~wxDirData()
{
    Close() ;
}

void wxDirData::Close()
{
    if ( m_iterator )
    {
        FSCloseIterator( m_iterator ) ;
        m_iterator = NULL ;
    }
}

void wxDirData::Rewind()
{
    Close() ;
}

bool wxDirData::Read(wxString *filename)
{
    wxString result;
    OSStatus err = noErr ;
    if ( NULL == m_iterator )
    {
        FSRef dirRef;
        err = wxMacPathToFSRef( m_dirname , &dirRef ) ;
        if ( err == noErr )
        {
            err = FSOpenIterator(&dirRef, kFSIterateFlat, &m_iterator);
        }
        if ( err )
        {
            Close() ;
            return false ;
        }
    }

    wxString name ;

    while( noErr == err )
    {
        HFSUniStr255 uniname ;
        FSRef fileRef;
        FSCatalogInfo catalogInfo;
        UInt32 fetched = 0;

        err = FSGetCatalogInfoBulk( m_iterator, 1, &fetched, NULL, kFSCatInfoNodeFlags | kFSCatInfoFinderInfo , &catalogInfo , &fileRef, NULL, &uniname );
        
        // expected error codes 
        
        if ( errFSNoMoreItems == err )
            return false ;
        if ( afpAccessDenied == err )
            return false ;

        if ( noErr != err )
            break ;

        name = wxMacHFSUniStrToString( &uniname ) ;

        if ( ( name == wxT(".") || name == wxT("..") ) && !(m_flags & wxDIR_DOTDOT) )
            continue;

        if ( ( name[0U] == '.' ) && !(m_flags & wxDIR_HIDDEN ) )
            continue ;

        if ( (((FileInfo*)&catalogInfo.finderInfo)->finderFlags & kIsInvisible ) && !(m_flags & wxDIR_HIDDEN ) )
            continue ;

        // its a dir and we don't want it
        if ( (catalogInfo.nodeFlags & kFSNodeIsDirectoryMask)  && !(m_flags & wxDIR_DIRS) )
            continue ;

        // its a file but we don't want it
        if ( (catalogInfo.nodeFlags & kFSNodeIsDirectoryMask) == 0  && !(m_flags & wxDIR_FILES ) )
            continue ;

        if ( m_filespec.empty() || m_filespec == wxT("*.*") || m_filespec == wxT("*") )
        {
        }
        else if ( !wxMatchWild(m_filespec, name , false) )
        {
            continue ;
        }

        break ;
    }
    if ( err != noErr )
    {
        return false ;
    }

    *filename = name ;
    return true;
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

    (void)Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete m_data;
    m_data = new wxDirData(dirname);

    return true;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
}

wxString wxDir::GetName() const
{
    wxString name;
    if ( m_data )
    {
        name = m_data->GetName();
        if ( !name.empty() && (name.Last() == _T('/')) )
        {
            // chop off the last (back)slash
            name.Truncate(name.length() - 1);
        }
    }

    return name;
}

wxDir::~wxDir()
{
    delete m_data;
    m_data = NULL;
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    m_data->Rewind();

    m_data->SetFileSpec(filespec);
    m_data->SetFlags(flags);

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, false, _T("bad pointer in wxDir::GetNext()") );

    return m_data->Read(filename);
}
