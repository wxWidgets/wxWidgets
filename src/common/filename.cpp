/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/filename.cpp
// Purpose:     wxFileName - encapsulates a file path
// Author:      Robert Roebling, Vadim Zeitlin
// Modified by:
// Created:     28.12.2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "filename.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/config.h"          // for wxExpandEnvVars
#include "wx/utils.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the character separating the extension from the base name
#define EXT_SEP _T('.')

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileName construction
// ----------------------------------------------------------------------------

void wxFileName::Assign( const wxFileName &filepath )
{
    m_ext = filepath.GetExt();
    m_name = filepath.GetName();
    m_dirs = filepath.GetDirs();
}

void wxFileName::Assign( const wxString& path,
                         const wxString& name,
                         const wxString& ext,
                         wxPathFormat format )
{
    wxStringTokenizer tn(path, GetPathSeparators(format),
                         wxTOKEN_RET_EMPTY_ALL);
    bool first = TRUE;
    m_dirs.Clear();
    while ( tn.HasMoreTokens() )
    {
        wxString token = tn.GetNextToken();

        // If the path starts with a slash, we need the first
        // dir entry to be an empty for later reassembly.
        if (first || !token.IsEmpty())
            m_dirs.Add( token );

        first = FALSE;
    }

    m_ext = ext;
    m_name = name;
}

void wxFileName::Assign(const wxString& fullpath,
                        wxPathFormat format)
{
    wxString path, name, ext;
    wxSplitPath(fullpath, &path, &name, &ext);

    Assign(path, name, ext, format);
}

void wxFileName::Assign(const wxString& path,
                        const wxString& fullname,
                        wxPathFormat format)
{
    wxString name, ext;
    wxSplitPath(fullname, NULL /* no path */, &name, &ext);

    Assign(path, name, ext, format);
}

void wxFileName::Clear()
{
    m_dirs.Clear();
    m_name =
    m_ext = wxEmptyString;
}

/* static */
wxFileName wxFileName::FileName(const wxString& file)
{
    return wxFileName(file);
}

/* static */
wxFileName wxFileName::DirName(const wxString& dir)
{
    wxFileName fn;
    fn.AssignDir(dir);
    return fn;
}

// ----------------------------------------------------------------------------
// existence tests
// ----------------------------------------------------------------------------

bool wxFileName::FileExists()
{
    return wxFileName::FileExists( GetFullPath() );
}

bool wxFileName::FileExists( const wxString &file )
{
    return ::wxFileExists( file );
}

bool wxFileName::DirExists()
{
    return wxFileName::DirExists( GetFullPath() );
}

bool wxFileName::DirExists( const wxString &dir )
{
    return ::wxDirExists( dir );
}

// ----------------------------------------------------------------------------
// CWD and HOME stuff
// ----------------------------------------------------------------------------

void wxFileName::AssignCwd()
{
    AssignDir(wxFileName::GetCwd());
}

/* static */
wxString wxFileName::GetCwd()
{
    return ::wxGetCwd();
}

bool wxFileName::SetCwd()
{
    return wxFileName::SetCwd( GetFullPath() );
}

bool wxFileName::SetCwd( const wxString &cwd )
{
    return ::wxSetWorkingDirectory( cwd );
}

void wxFileName::AssignHomeDir()
{
    AssignDir(wxFileName::GetHomeDir());
}

wxString wxFileName::GetHomeDir()
{
    return ::wxGetHomeDir();
}

void wxFileName::AssignTempFileName( const wxString &prefix )
{
    wxString fullname;
    if ( wxGetTempFileName(prefix, fullname) )
    {
        Assign(fullname);
    }
    else // error
    {
        Clear();
    }
}

// ----------------------------------------------------------------------------
// directory operations
// ----------------------------------------------------------------------------

bool wxFileName::Mkdir( int perm )
{
    return wxFileName::Mkdir( GetFullPath(), perm );
}

bool wxFileName::Mkdir( const wxString &dir, int perm )
{
    return ::wxMkdir( dir, perm );
}

bool wxFileName::Rmdir()
{
    return wxFileName::Rmdir( GetFullPath() );
}

bool wxFileName::Rmdir( const wxString &dir )
{
    return ::wxRmdir( dir );
}

// ----------------------------------------------------------------------------
// path normalization
// ----------------------------------------------------------------------------

bool wxFileName::Normalize(wxPathNormalize flags,
                           const wxString& cwd,
                           wxPathFormat format)
{
    // the existing path components
    wxArrayString dirs = GetDirs();

    // the path to prepend in front to make the path absolute
    wxFileName curDir;

    format = GetFormat(format);

    // make the path absolute
    if ( (flags & wxPATH_NORM_ABSOLUTE) && !IsAbsolute() )
    {
        if ( cwd.empty() )
            curDir.AssignCwd();
        else
            curDir.AssignDir(cwd);
    }

    // handle ~ stuff under Unix only
    if ( (format == wxPATH_UNIX) && (flags & wxPATH_NORM_TILDE) )
    {
        if ( !dirs.IsEmpty() )
        {
            wxString dir = dirs[0u];
            if ( !dir.empty() && dir[0u] == _T('~') )
            {
                curDir.AssignDir(wxGetUserHome(dir.c_str() + 1));

                dirs.Remove(0u);
            }
        }
    }

    if ( curDir.IsOk() )
    {
        wxArrayString dirsNew = curDir.GetDirs();
        size_t count = dirs.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            dirsNew.Add(dirs[n]);
        }

        dirs = dirsNew;
    }

    // now deal with ".", ".." and the rest
    m_dirs.Empty();
    size_t count = dirs.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString dir = dirs[n];

        if ( flags && wxPATH_NORM_DOTS )
        {
            if ( dir == wxT(".") )
            {
                // just ignore
                continue;
            }

            if ( dir == wxT("..") )
            {
                if ( m_dirs.IsEmpty() )
                {
                    wxLogError(_("The path '%s' contains too many \"..\"!"),
                               GetFullPath().c_str());
                    return FALSE;
                }

                m_dirs.Remove(m_dirs.GetCount() - 1);
                continue;
            }
        }

        if ( flags & wxPATH_NORM_ENV_VARS )
        {
            dir = wxExpandEnvVars(dir);
        }

        if ( (flags & wxPATH_NORM_CASE) && !IsCaseSensitive(format) )
        {
            dir.MakeLower();
        }

        m_dirs.Add(dir);
    }

    if ( (flags & wxPATH_NORM_CASE) && !IsCaseSensitive(format) )
    {
        // VZ: expand env vars here too?

        m_name.MakeLower();
        m_ext.MakeLower();
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// filename kind tests
// ----------------------------------------------------------------------------

bool wxFileName::SameAs( const wxFileName &filepath, wxPathFormat format)
{
    wxFileName fn1 = *this,
               fn2 = filepath;

    // get cwd only once - small time saving
    wxString cwd = wxGetCwd();
    fn1.Normalize(wxPATH_NORM_ALL, cwd, format);
    fn2.Normalize(wxPATH_NORM_ALL, cwd, format);

    if ( fn1.GetFullPath() == fn2.GetFullPath() )
        return TRUE;

    // TODO: compare inodes for Unix, this works even when filenames are
    //       different but files are the same (symlinks) (VZ)

    return FALSE;
}

/* static */
bool wxFileName::IsCaseSensitive( wxPathFormat format )
{
    // only DOS filenames are case-sensitive
    return GetFormat(format) != wxPATH_DOS;
}

bool wxFileName::IsRelative( wxPathFormat format )
{
    return !IsAbsolute(format);
}

bool wxFileName::IsAbsolute( wxPathFormat format )
{
    wxChar ch = m_dirs.IsEmpty() ? _T('\0') : m_dirs[0u][0u];

    // the path is absolute if it starts with a path separator or, only for
    // Unix filenames, with "~" or "~user"
    return IsPathSeparator(ch, format) ||
           (GetFormat(format) == wxPATH_UNIX && ch == _T('~') );
}

/* static */
wxString wxFileName::GetPathSeparators(wxPathFormat format)
{
    wxString seps;
    switch ( GetFormat(format) )
    {
        case wxPATH_DOS:
            // accept both as native APIs do
            seps = _T("/\\");
            break;

        default:
            wxFAIL_MSG( _T("unknown wxPATH_XXX style") );
            // fall through

        case wxPATH_UNIX:
            seps = _T("/");
            break;

        case wxPATH_MAC:
            seps = _T(":");
            break;
    }

    return seps;
}

/* static */
bool wxFileName::IsPathSeparator(wxChar ch, wxPathFormat format)
{
    return GetPathSeparators(format).Find(ch) != wxNOT_FOUND;
}

bool wxFileName::IsWild( wxPathFormat format )
{
    // FIXME: this is probably false for Mac and this is surely wrong for most
    //        of Unix shells (think about "[...]")
    return m_name.find_first_of(_T("*?")) != wxString::npos;
}

// ----------------------------------------------------------------------------
// path components manipulation
// ----------------------------------------------------------------------------

void wxFileName::AppendDir( const wxString &dir )
{
    m_dirs.Add( dir );
}

void wxFileName::PrependDir( const wxString &dir )
{
    m_dirs.Insert( dir, 0 );
}

void wxFileName::InsertDir( int before, const wxString &dir )
{
    m_dirs.Insert( dir, before );
}

void wxFileName::RemoveDir( int pos )
{
    m_dirs.Remove( (size_t)pos );
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

wxString wxFileName::GetFullName() const
{
    wxString fullname = m_name;
    if ( !m_ext.empty() )
    {
        fullname << EXT_SEP << m_ext;
    }

    return fullname;
}

wxString wxFileName::GetPath( bool add_separator, wxPathFormat format ) const
{
    format = GetFormat( format );

    wxString ret;
    size_t count = m_dirs.GetCount();
    for ( size_t i = 0; i < count; i++ )
    {
        ret += m_dirs[i];
        if ( add_separator || (i < count) )
            ret += wxFILE_SEP_PATH;
    }

    return ret;
}

wxString wxFileName::GetFullPath( wxPathFormat format ) const
{
    return GetPathWithSep() + GetFullName();
}

wxPathFormat wxFileName::GetFormat( wxPathFormat format )
{
    if (format == wxPATH_NATIVE)
    {
#if defined(__WXMSW__) || defined(__WXPM__)
        format = wxPATH_DOS;
#elif defined(__WXMAC__)
        format = wxPATH_MAC;
#else
        format = wxPATH_UNIX;
#endif
    }
    return format;
}

