/////////////////////////////////////////////////////////////////////////////
// Name:        filename.h
// Purpose:     wxFileName - encapsulates ice cream
// Author:      Robert Roebling
// Modified by:
// Created:     28.12.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_FILENAME_H_
#define   _WX_FILENAME_H_

#ifdef __GNUG__
    #pragma interface "filename.h"
#endif

#ifndef WX_PRECOMP
  #include  "wx/string.h"
#endif

// ridiculously enough, this will replace DirExists with wxDirExists etc
#include "wx/filefn.h"

enum wxPathFormat
{
  wxPATH_NATIVE = 0,
  wxPATH_UNIX,
  wxPATH_MAC,
  wxPATH_DOS,

  wxPATH_BEOS = wxPATH_UNIX,
  wxPATH_WIN = wxPATH_DOS,
  wxPATH_OS2 = wxPATH_DOS

};

class WXDLLEXPORT wxFileName
{
public:
    // constructors and assignment
    wxFileName() 
        { }
    wxFileName( const wxFileName &filepath );
    wxFileName( const wxString &path, bool dir_only = FALSE, wxPathFormat format = wxPATH_NATIVE )
        { Assign( path, dir_only, format ); }
    void Assign( const wxString &path, bool dir_only = FALSE, wxPathFormat format = wxPATH_NATIVE );
    void Assign( const wxFileName &filepath );
    
    // Only native form
    bool FileExists();
    static bool FileExists( const wxString &file );
    
    bool DirExists();
    static bool DirExists( const wxString &dir );
    
    void AssignCwd();
    static wxString GetCwd();
    
    bool SetCwd();
    static bool SetCwd( const wxString &cwd );
    
    void AssignHomeDir();
    static wxString GetHomeDir();
    
    void AssignTempFileName( const wxString &prefix );
    
    bool Mkdir( int perm = 0777 );
    static bool Mkdir( const wxString &dir, int perm = 0777 );
    
    bool Rmdir();
    static bool Rmdir( const wxString &dir );
    
    // Remove . and .. (under Unix ~ as well)
    bool Normalize( const wxString &cwd = wxEmptyString, const wxString &home = wxEmptyString );
    
    // Comparison
    bool SameAs( const wxFileName &filepath, bool upper_case = TRUE );
    
    // Tests
    bool IsCaseSensitive( wxPathFormat format = wxPATH_NATIVE );
    bool IsRelative( wxPathFormat format = wxPATH_NATIVE );
    bool IsAbsolute( wxPathFormat format = wxPATH_NATIVE );
    bool IsWild( wxPathFormat format = wxPATH_NATIVE );
    
    // Dir accessors
    void AppendDir( const wxString &dir );
    void PrependDir( const wxString &dir );
    void InsertDir( int before, const wxString &dir );
    void RemoveDir( int pos );
    size_t GetDirCount()             { return m_dirs.GetCount(); }
    
    // Other accessors
    void SetExt( const wxString &ext )          { m_ext = ext; }
    wxString GetExt() const                     { return m_ext; }
    bool HasExt() const                         { return !m_ext.IsEmpty(); }
    
    void SetName( const wxString &name )        { m_name = name; }
    wxString GetName() const                    { return m_name; }
    bool HasName() const                        { return !m_name.IsEmpty(); }
    
    // name and ext
    void SetFullName( const wxString name, wxPathFormat format = wxPATH_NATIVE );
    wxString GetFullName();
    
    const wxArrayString &GetDirs() const        { return m_dirs; }
    
    // Construct path only
    wxString GetPath( bool add_separator = FALSE, wxPathFormat format = wxPATH_NATIVE ) const;
    
    // Construct full path with name and ext
    wxString GetFullPath( wxPathFormat format = wxPATH_NATIVE ) const;
    
    
    static wxPathFormat GetFormat( wxPathFormat format = wxPATH_NATIVE );
    
private:
    wxArrayString   m_dirs;
    wxString        m_name;
    wxString        m_ext;
};



#endif // _WX_FFILENAME_H_

