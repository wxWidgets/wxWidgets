/////////////////////////////////////////////////////////////////////////////
// Name:        filename.cpp
// Purpose:     wxFileName - encapsulates candy
// Author:      Robert Roebling
// Modified by:
// Created:     28.12.2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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
#include "wx/utils.h"

//----------------------------------------------------------------------------
// wxFileName
//----------------------------------------------------------------------------

wxFileName::wxFileName( const wxFileName &filepath )
{
    m_ext = filepath.GetExt();
    m_name = filepath.GetName();
    const wxArrayString &dirs = filepath.GetDirs();
    for (size_t i = 0; i < dirs.GetCount(); i++)
        m_dirs.Add( dirs[i] );
}

void wxFileName::Assign( const wxFileName &filepath )
{
    m_dirs.Clear();
    m_ext = filepath.GetExt();
    m_name = filepath.GetName();
    const wxArrayString &dirs = filepath.GetDirs();
    for (size_t i = 0; i < dirs.GetCount(); i++)
        m_dirs.Add( dirs[i] );
}

void wxFileName::Assign( const wxString &path, bool dir_only, wxPathFormat format )
{
    m_ext = wxEmptyString;
    m_name = wxEmptyString;
    m_dirs.Clear();

    format = GetFormat( format );

    wxString seps;
    if (format == wxPATH_DOS)
    {
        seps = "/\\";
    }
    else
    if (format == wxPATH_UNIX)
    {
        seps = "/";
    }
    else
    {
        seps = ":";
    }
    
    wxStringTokenizer tn( path, seps );
    while (tn.HasMoreTokens())
    {
        wxString token( tn.GetNextToken() );
        if (!token.IsEmpty())
            m_dirs.Add( token );
    }
    
    if (!dir_only)
    {
        // make last m_dir -> m_name 
        size_t last = m_dirs.GetCount();
        if (last == 0) return;
        last--;
        m_name = m_dirs[last];
        m_dirs.Remove( last );
        
        if (m_name == wxT(".")) return;
        if (m_name == wxT("..")) return;
        
        // ext?
        int pos = m_name.Find( wxT('.') );
        if (pos == -1) return;
        
        bool has_starting_dot = (pos == 0);
        if (has_starting_dot && (format == wxPATH_UNIX))
        {
            // remove dot
            m_name.Remove(0,1);  
            
            // search again
            pos = m_name.Find( wxT('.') );
            if (pos == -1)
            {
                // add dot back
                m_name.Prepend( "." );
                return;
            }
        }
        m_ext = m_name;
        m_ext.Remove( 0, pos+1 );
        
        m_name.Remove( pos, m_name.Len()-pos );
        
        if (has_starting_dot && (format == wxPATH_UNIX))
        {
            // add dot back
            m_name.Prepend( "." );
            return;
        }
    }
}

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

void wxFileName::AssignCwd()
{
    Assign( wxFileName::GetCwd(), TRUE );
}

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
    Assign( wxFileName::GetHomeDir(), TRUE );
}
    
wxString wxFileName::GetHomeDir()
{
    return ::wxGetHomeDir();
}
    
void wxFileName::AssignTempFileName( const wxString &prefix )
{
}

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

bool wxFileName::Normalize( const wxString &cwd, const wxString &home )
{
    wxFileName tmp( *this );
    m_dirs.Clear();
    const wxArrayString &dirs = tmp.GetDirs();
    
    if (dirs.GetCount() == 0) return FALSE;
    
    size_t start = 0;
    
    if (dirs[0] == wxT("."))
    {
        if (cwd == wxEmptyString)
            Assign( wxFileName::GetCwd(), TRUE );
        else
            Assign( cwd );
        start = 1;
    }
    else
    if (dirs[0] == wxT(".."))
    {
        if (cwd == wxEmptyString)
            Assign( wxFileName::GetCwd(), TRUE );
        else
            Assign( cwd );
        m_dirs.Remove( m_dirs.GetCount()-1 );
        start = 1;
    }
    else
    if (dirs[0] == wxT("~"))
    {
        if (home == wxEmptyString)
            Assign( wxFileName::GetHomeDir(), TRUE );
        else
            Assign( home );
        start = 1;
    }
    
    for (size_t i = start; i < dirs.GetCount(); i++)
    {
        if (dirs[i] == wxT(".")) continue;
        
        if (dirs[i] == wxT(".."))
        {
            m_dirs.Remove( m_dirs.GetCount()-1 );
            continue;
        }
        
        // expand env vars here ?
        
        m_dirs.Add( dirs[i] );
    }
    
    m_name = tmp.GetName();
    m_ext = tmp.GetExt();
    
    return TRUE;
}

bool wxFileName::SameAs( const wxFileName &filepath, bool upper_case )
{
    wxString file1( GetFullPath() );
    wxString file2( filepath.GetFullPath() );
   
    if (upper_case)
    {
        file1.MakeUpper();  // what does MSW do to non-ascii chars etc? native funcs?
        file2.MakeUpper();
    }
   
    return (file1 == file2);
}

bool wxFileName::IsCaseSensitive( wxPathFormat format )
{
    format = GetFormat( format );
    
    return (format != wxPATH_DOS);
}

bool wxFileName::IsRelative( wxPathFormat format )
{
    format = GetFormat( format );
    
    for (size_t i = 0; i < m_dirs.GetCount(); i++)
    {
        if ((format == wxPATH_UNIX) && (i == 0) && (m_dirs[0] == wxT("~"))) return TRUE;
    
        if (m_dirs[i] == wxT(".")) return TRUE;
        if (m_dirs[i] == wxT("..")) return TRUE;
    }

    return FALSE;
}

bool wxFileName::IsAbsolute( wxPathFormat format )
{
    return (!IsRelative(format));
}

bool wxFileName::IsWild( wxPathFormat format )
{
    format = GetFormat( format );
    
    if (format == wxPATH_DOS)
    {
       if (m_name.Find( wxT('*') ) != -1) return TRUE;
       if (m_name.Find( wxT('?') ) != -1) return TRUE;
    }
    else
    {
       if (m_name.Find( wxT('*') ) != -1) return TRUE;
    }
    
    return FALSE;
}

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

void wxFileName::SetFullName( const wxString name, wxPathFormat format )
{
    format = GetFormat( format );
    
    m_name = name;
    
    if (m_name == wxT(".")) return;
    if (m_name == wxT("..")) return;
        
    // ext?
    int pos = m_name.Find( wxT('.') );
    if (pos == -1) return;
        
    bool has_starting_dot = (pos == 0);
    if (has_starting_dot && (format == wxPATH_UNIX))
    {
            // remove dot
            m_name.Remove(0,1);  
            
            // search again
            pos = m_name.Find( wxT('.') );
            if (pos == -1)
            {
                // add dot back
                m_name.Prepend( "." );
                return;
            }
    }
    
    m_ext = m_name;
    m_ext.Remove( 0, pos+1 );
        
    m_name.Remove( pos, m_name.Len()-pos );
       
    if (has_starting_dot && (format == wxPATH_UNIX))
    {
            // add dot back
            m_name.Prepend( "." );
            return;
    }
}

wxString wxFileName::GetFullName()
{
    wxString ret( m_name );
    if (!m_ext.IsEmpty())
    { 
        ret += '.';
        ret += m_ext;
    }
    return ret;
}

wxString wxFileName::GetPath( bool add_separator, wxPathFormat format ) const
{
    format = GetFormat( format );
    
    wxString ret;
    if (format == wxPATH_DOS)
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (add_separator || (i != m_dirs.GetCount()-1))
                ret += '\\';
        }
    }
    else
    if (format == wxPATH_UNIX)
    {
        ret = '/'; // FIXME: must be absolute
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (add_separator || (i != m_dirs.GetCount()-1))
                ret += '/';
        }
    }
    else
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (add_separator || (i != m_dirs.GetCount()-1))
                ret += ":";
        }
    }
    
    return ret;
}

wxString wxFileName::GetFullPath( wxPathFormat format ) const
{
    format = GetFormat( format );
    
    wxString ret;
    if (format == wxPATH_DOS)
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            ret += '\\';
        }
    }
    else
    if (format == wxPATH_UNIX)
    {
        ret = '/'; // FIXME: must be absolute
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            ret += '/';
        }
    }
    else
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            ret += ':';
        }
    }
    
    ret += m_name;
    
    if (!m_ext.IsEmpty())
    {
        ret += '.';
        ret += m_ext;
    }
    
    return ret;
}

wxPathFormat wxFileName::GetFormat( wxPathFormat format )
{
    if (format == wxPATH_NATIVE)
    {
#if defined(__WXMSW__) || defined(__WXPM__)
        format = wxPATH_DOS;
#endif
#if defined(__WXMAC__)
        format = wxPATH_MAC;
#endif
#if !defined(__WXMSW__) && !defined(__WXPM__) && !defined(__WXMAC__)
        format = wxPATH_UNIX;
#endif
    }
    return format;
}

