/////////////////////////////////////////////////////////////////////////////
// Name:        wx/filename.h
// Purpose:     wxFileName - encapsulates a file path
// Author:      Robert Roebling, Vadim Zeitlin
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

/*
    TODO:

    1. support for drives under Windows
    2. more file operations:
        a) chmod()
        b) [acm]time() - get and set
        c) file size
        d) file permissions with readable accessors for most common bits
           such as IsReadable() &c
        e) rename()?
    3. SameFileAs() function to compare inodes under Unix
 */

// ridiculously enough, this will replace DirExists with wxDirExists etc
#include "wx/filefn.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the various values for the path format: this mainly affects the path
// separator but also whether or not the path has the drive part (as under
// Windows)
enum wxPathFormat
{
    wxPATH_NATIVE = 0,      // the path format for the current platform
    wxPATH_UNIX,
    wxPATH_MAC,
    wxPATH_DOS,

    wxPATH_BEOS = wxPATH_UNIX,
    wxPATH_WIN = wxPATH_DOS,
    wxPATH_OS2 = wxPATH_DOS
};

// the kind of normalization to do with the file name: these values can be
// or'd together to perform several operations at once
enum wxPathNormalize
{
    wxPATH_NORM_ENV_VARS = 0x0001,  // replace env vars with their values
    wxPATH_NORM_DOTS     = 0x0002,  // squeeze all .. and . and prepend cwd
    wxPATH_NORM_TILDE    = 0x0004,  // Unix only: replace ~ and ~user
    wxPATH_NORM_CASE     = 0x0008,  // if case insensitive => tolower
    wxPATH_NORM_ABSOLUTE = 0x0010,  // make the path absolute
    wxPATH_NORM_LONG =     0x0020,  // make the path the long form
    wxPATH_NORM_ALL      = 0x003f
};

// ----------------------------------------------------------------------------
// wxFileName: encapsulates a file path
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileName
{
public:
    // constructors and assignment

        // the usual stuff
    wxFileName() { }
    wxFileName( const wxFileName &filepath ) { Assign(filepath); }

        // from a full filename: if it terminates with a '/', a directory path
        // is contructed (the name will be empty), otherwise a file name and
        // extension are extracted from it
    wxFileName( const wxString& fullpath, wxPathFormat format = wxPATH_NATIVE )
        { Assign( fullpath, format ); }

        // from a directory name and a file name
    wxFileName(const wxString& path,
               const wxString& name,
               wxPathFormat format = wxPATH_NATIVE)
        { Assign(path, name, format); }

        // from a directory name, file base name and extension
    wxFileName(const wxString& path,
               const wxString& name,
               const wxString& ext,
               wxPathFormat format = wxPATH_NATIVE)
        { Assign(path, name, ext, format); }

        // assorted assignment operators

    wxFileName& operator=(const wxFileName& filename)
        { Assign(filename); return *this; }

    wxFileName& operator=(const wxString& filename)
        { Assign(filename); return *this; }

        // the same for delayed initialization

        // VZ: wouldn't it be better to call this Create() for consistency with
        //     all GUI classes? Personally, I like Set() more than Assign() too

    void Assign(const wxFileName& filepath);
    void Assign(const wxString& fullpath,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& path,
                const wxString& name,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& path,
                const wxString& name,
                const wxString& ext,
                wxPathFormat format = wxPATH_NATIVE);
    void AssignDir(const wxString& dir, wxPathFormat format = wxPATH_NATIVE)
        { Assign(dir, _T(""), format); }

        // reset all components to default, uninitialized state
    void Clear();

        // static pseudo constructors
    static wxFileName FileName(const wxString& file);
    static wxFileName DirName(const wxString& dir);

    // test for existence

        // is the filename valid at all?
    bool IsOk() const { return !m_dirs.IsEmpty() || !m_name.IsEmpty(); }

        // does the file with this name exists?
    bool FileExists();
    static bool FileExists( const wxString &file );

        // does the directory with this name exists?
    bool DirExists();
    static bool DirExists( const wxString &dir );

        // VZ: also need: IsDirWritable(), IsFileExecutable() &c (TODO)

    // various file/dir operations

        // retrieve the value of the current working directory
    void AssignCwd();
    static wxString GetCwd();

        // change the current working directory
    bool SetCwd();
    static bool SetCwd( const wxString &cwd );

        // get the value of user home (Unix only mainly)
    void AssignHomeDir();
    static wxString GetHomeDir();

        // get a temp file name starting with thespecified prefix
    void AssignTempFileName( const wxString &prefix );

    // directory creation and removal
    bool Mkdir( int perm = 0777 );
    static bool Mkdir( const wxString &dir, int perm = 0777 );

    bool Rmdir();
    static bool Rmdir( const wxString &dir );

    // operations on the path

        // normalize the path: with the default flags value, the path will be
        // made absolute, without any ".." and "." and all environment
        // variables will be expanded in it
        //
        // this may be done using another (than current) value of cwd
    bool Normalize(wxPathNormalize flags = wxPATH_NORM_ALL,
                   const wxString& cwd = wxEmptyString,
                   wxPathFormat format = wxPATH_NATIVE);

    // Comparison

        // uses the current platform settings
    bool operator==(const wxFileName& filename) { return SameAs(filename); }
    bool operator==(const wxString& filename)
        { return *this == wxFileName(filename); }

        // compares with the rules of this platform
    bool SameAs(const wxFileName &filepath,
                wxPathFormat format = wxPATH_NATIVE);

    // Tests
    static bool IsCaseSensitive( wxPathFormat format = wxPATH_NATIVE );
    bool IsRelative( wxPathFormat format = wxPATH_NATIVE );
    bool IsAbsolute( wxPathFormat format = wxPATH_NATIVE );

    // get the string of path separators for this format
    static wxString GetPathSeparators(wxPathFormat format = wxPATH_NATIVE);

    // is the char a path separator for this format?
    static bool IsPathSeparator(wxChar ch, wxPathFormat format = wxPATH_NATIVE);

    // FIXME: what exactly does this do?
    bool IsWild( wxPathFormat format = wxPATH_NATIVE );

    // Dir accessors
    void AppendDir( const wxString &dir );
    void PrependDir( const wxString &dir );
    void InsertDir( int before, const wxString &dir );
    void RemoveDir( int pos );
    size_t GetDirCount() const { return m_dirs.GetCount(); }

    // Other accessors
    void SetExt( const wxString &ext )          { m_ext = ext; }
    wxString GetExt() const                     { return m_ext; }
    bool HasExt() const                         { return !m_ext.IsEmpty(); }

    void SetName( const wxString &name )        { m_name = name; }
    wxString GetName() const                    { return m_name; }
    bool HasName() const                        { return !m_name.IsEmpty(); }

    // full name is the file name + extension (but without the path)
    void SetFullName(const wxString& fullname);
    wxString GetFullName() const;

    const wxArrayString &GetDirs() const        { return m_dirs; }

    // Construct path only - possibly with the trailing separator
    wxString GetPath( bool add_separator = FALSE,
                      wxPathFormat format = wxPATH_NATIVE ) const;

    // more readable synonym
    wxString GetPathWithSep(wxPathFormat format = wxPATH_NATIVE ) const
        { return GetPath(TRUE /* add separator */, format); }

    // Construct full path with name and ext
    wxString GetFullPath( wxPathFormat format = wxPATH_NATIVE ) const;

    // Return the short form of the path (returns identity on non-Windows platforms)
    wxString GetShortPath() const;

    // Return the long form of the path (returns identity on non-Windows platforms)
    wxString GetLongPath() const;

    // various helpers

        // get the canonical path format for this platform
    static wxPathFormat GetFormat( wxPathFormat format = wxPATH_NATIVE );

        // split a fullpath into path, (base) name and ext (all of the pointers
        // can be NULL)
    static void SplitPath(const wxString& fullpath,
                          wxString *path,
                          wxString *name,
                          wxString *ext,
                          wxPathFormat format = wxPATH_NATIVE);

private:
    // the path components of the file
    wxArrayString   m_dirs;

    // the file name and extension (empty for directories)
    wxString        m_name,
                    m_ext;
};

#endif // _WX_FILENAME_H_

