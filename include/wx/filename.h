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
#include "filefn.h"

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
    wxFileName( const wxFileName &filename );
    wxFileName( const wxString &path, bool dir_only = FALSE, wxPathFormat format = wxPATH_NATIVE )
        { Assign( path, dir_only, format ); }
    void Assign( const wxString &path, bool dir_only = FALSE, wxPathFormat format = wxPATH_NATIVE );
    
    // Only native form
    bool FileExists();
    bool DirExists();
    
    void AssignCwd();
    void SetCwd();
    
    void AssignTempFileName( const wxString &prefix );
    
    void Mkdir( int perm = 0777 );
    void Rmdir();
    
    // Remove . and .. (under Unix ~ as well)
    void MakeAbsolute();
    
    // Comparison
    bool SameAs( const wxFileName &filename, bool upper_on_dos = TRUE );
    
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
    
    const wxArrayString &GetDirs() const        { return m_dirs; }
    
    // Construct path only
    wxString GetPath( wxPathFormat format = wxPATH_NATIVE ) const;
    
    // Construct full path with name and ext
    wxString GetFullPath( wxPathFormat format = wxPATH_NATIVE ) const;
    
    
    static wxPathFormat GetFormat( wxPathFormat format = wxPATH_NATIVE );
    
private:
    wxArrayString   m_dirs;
    wxString        m_name;
    wxString        m_ext;
};



#endif // _WX_FFILENAME_H_

