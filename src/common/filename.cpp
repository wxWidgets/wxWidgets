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
#include "wx/filefn.h"

//----------------------------------------------------------------------------
// wxFileName
//----------------------------------------------------------------------------

wxFileName::wxFileName( const wxFileName &filename )
{
    m_ext = filename.GetExt();
    m_name = filename.GetName();
    const wxArrayString &dirs = filename.GetDirs();
    for (size_t i = 0; i < dirs.GetCount(); i++)
    {
        m_dirs.Add( dirs[i] );
    }
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
        seps = "/";  // or maybe ":" or both ?
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
        if (has_starting_dot)
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
        
        if (has_starting_dot)
        {
            // add dot back
            m_name.Prepend( "." );
            return;
        }
    }
}

bool wxFileName::FileExists()
{
    return ::wxFileExists( GetFullPath() );
}

bool wxFileName::DirExists()
{
    return ::wxDirExists( GetFullPath() );
}

void wxFileName::AssignCwd()
{
    Assign( wxGetCwd(), TRUE );
}

void wxFileName::SetCwd()
{
    wxSetWorkingDirectory( GetFullPath() );
}

void wxFileName::AssignTempFileName( const wxString &prefix )
{
}

void wxFileName::Mkdir( int perm )
{
    wxMkdir( GetFullPath(), perm );
}

void wxFileName::Rmdir()
{
    wxRmdir( GetFullPath() );
}

void wxFileName::MakeAbsolute()
{
}

bool wxFileName::SameAs( const wxFileName &filename, bool upper_on_dos )
{
    wxString file1( GetFullPath() );
    wxString file2( filename.GetFullPath() );
   
#ifdef __WXMSW__
    if (upper_on_dos)
    {
        file1.MakeUpper();
        file2.MakeUpper();
    }
#endif
   
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

wxString wxFileName::GetPath( wxPathFormat format ) const
{
    format = GetFormat( format );
    
    wxString ret;
    if (format == wxPATH_DOS)
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (i != m_dirs.GetCount()-1) ret += '\\';
        }
    }
    else
    if (format == wxPATH_DOS)
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (i != m_dirs.GetCount()-1) ret += '/';
        }
    }
    else
    {
        for (size_t i = 0; i < m_dirs.GetCount(); i++)
        {
            ret += m_dirs[i];
            if (i != m_dirs.GetCount()-1) ret += "//";  // or maybe ":" ?
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
    if (format == wxPATH_DOS)
    {
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
            ret += '/';  // or maybe ":" ?
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
