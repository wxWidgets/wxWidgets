/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/filename.cpp
// Purpose:     wxFileName - encapsulates a file path
// Author:      Robert Roebling, Vadim Zeitlin
// Modified by:
// Created:     28.12.2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   Here are brief descriptions of the filename formats supported by this class:

   wxPATH_UNIX: standard Unix format, used under Darwin as well, absolute file
                names have the form:
                /dir1/dir2/.../dirN/filename, "." and ".." stand for the
                current and parent directory respectively, "~" is parsed as the
                user HOME and "~username" as the HOME of that user

   wxPATH_DOS:  DOS/Windows format, absolute file names have the form:
                drive:\dir1\dir2\...\dirN\filename.ext where drive is a single
                letter. "." and ".." as for Unix but no "~".

                There are also UNC names of the form \\share\fullpath

   wxPATH_MAC:  Mac OS 8/9 and Mac OS X under CodeWarrior 7 format, absolute file
                names have the form
                    volume:dir1:...:dirN:filename
                and the relative file names are either
                    :dir1:...:dirN:filename
                or just
                    filename
                (although :filename works as well).
                Since the volume is just part of the file path, it is not
                treated like a separate entity as it is done under DOS and
                VMS, it is just treated as another dir.

   wxPATH_VMS:  VMS native format, absolute file names have the form
                    <device>:[dir1.dir2.dir3]file.txt
                or
                    <device>:[000000.dir1.dir2.dir3]file.txt

                the <device> is the physical device (i.e. disk). 000000 is the
                root directory on the device which can be omitted.

                Note that VMS uses different separators unlike Unix:
                 : always after the device. If the path does not contain : than
                   the default (the device of the current directory) is assumed.
                 [ start of directory specyfication
                 . separator between directory and subdirectory
                 ] between directory and file
 */

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
#include "wx/file.h"
#endif

#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/config.h"          // for wxExpandEnvVars
#include "wx/utils.h"
#include "wx/file.h"
#include "wx/dynlib.h"

// For GetShort/LongPathName
#ifdef __WIN32__
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

#if defined(__WXMAC__)
  #include  "wx/mac/private.h"  // includes mac headers
#endif

// utime() is POSIX so should normally be available on all Unices
#ifdef __UNIX_LIKE__
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef __DJGPP__
#include <unistd.h>
#endif

#ifdef __MWERKS__
#ifdef __MACH__
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <stat.h>
#include <unistd.h>
#include <unix.h>
#endif
#endif

#ifdef __WATCOMC__
#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h>
#endif

#ifdef __VISAGECPP__
#ifndef MAX_PATH
#define MAX_PATH 256
#endif
#endif

#ifdef __EMX__
#define MAX_PATH _MAX_PATH
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// small helper class which opens and closes the file - we use it just to get
// a file handle for the given file name to pass it to some Win32 API function
#if defined(__WIN32__) && !defined(__WXMICROWIN__)

class wxFileHandle
{
public:
    enum OpenMode
    {
        Read,
        Write
    };

    wxFileHandle(const wxString& filename, OpenMode mode)
    {
        m_hFile = ::CreateFile
                    (
                     filename,                      // name
                     mode == Read ? GENERIC_READ    // access mask
                                  : GENERIC_WRITE,
                     0,                             // no sharing
                     NULL,                          // no secutity attr
                     OPEN_EXISTING,                 // creation disposition
                     0,                             // no flags
                     NULL                           // no template file
                    );

        if ( m_hFile == INVALID_HANDLE_VALUE )
        {
            wxLogSysError(_("Failed to open '%s' for %s"),
                          filename.c_str(),
                          mode == Read ? _("reading") : _("writing"));
        }
    }

    ~wxFileHandle()
    {
        if ( m_hFile != INVALID_HANDLE_VALUE )
        {
            if ( !::CloseHandle(m_hFile) )
            {
                wxLogSysError(_("Failed to close file handle"));
            }
        }
    }

    // return TRUE only if the file could be opened successfully
    bool IsOk() const { return m_hFile != INVALID_HANDLE_VALUE; }

    // get the handle
    operator HANDLE() const { return m_hFile; }

private:
    HANDLE m_hFile;
};

#endif // __WIN32__

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

#if wxUSE_DATETIME && defined(__WIN32__) && !defined(__WXMICROWIN__)

// convert between wxDateTime and FILETIME which is a 64-bit value representing
// the number of 100-nanosecond intervals since January 1, 1601.

static void ConvertFileTimeToWx(wxDateTime *dt, const FILETIME &ft)
{
    FILETIME ftcopy = ft;
    FILETIME ftLocal;
    if ( !::FileTimeToLocalFileTime(&ftcopy, &ftLocal) )
    {
        wxLogLastError(_T("FileTimeToLocalFileTime"));
    }

    SYSTEMTIME st;
    if ( !::FileTimeToSystemTime(&ftLocal, &st) )
    {
        wxLogLastError(_T("FileTimeToSystemTime"));
    }

    dt->Set(st.wDay, wxDateTime::Month(st.wMonth - 1), st.wYear,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

static void ConvertWxToFileTime(FILETIME *ft, const wxDateTime& dt)
{
    SYSTEMTIME st;
    st.wDay = dt.GetDay();
    st.wMonth = dt.GetMonth() + 1;
    st.wYear = dt.GetYear();
    st.wHour = dt.GetHour();
    st.wMinute = dt.GetMinute();
    st.wSecond = dt.GetSecond();
    st.wMilliseconds = dt.GetMillisecond();

    FILETIME ftLocal;
    if ( !::SystemTimeToFileTime(&st, &ftLocal) )
    {
        wxLogLastError(_T("SystemTimeToFileTime"));
    }

    if ( !::LocalFileTimeToFileTime(&ftLocal, ft) )
    {
        wxLogLastError(_T("LocalFileTimeToFileTime"));
    }
}

#endif // wxUSE_DATETIME && __WIN32__

// return a string with the volume par
static wxString wxGetVolumeString(const wxString& volume, wxPathFormat format)
{
    wxString path;

    if ( !volume.empty() )
    {
        format = wxFileName::GetFormat(format);

        // Special Windows UNC paths hack, part 2: undo what we did in
        // SplitPath() and make an UNC path if we have a drive which is not a
        // single letter (hopefully the network shares can't be one letter only
        // although I didn't find any authoritative docs on this)
        if ( format == wxPATH_DOS && volume.length() > 1 )
        {
            path << wxFILE_SEP_PATH_DOS << wxFILE_SEP_PATH_DOS << volume;
        }
        else if  ( format == wxPATH_DOS || format == wxPATH_VMS )
        {
            path << volume << wxFileName::GetVolumeSeparator(format);
        }
        // else ignore
    }

    return path;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileName construction
// ----------------------------------------------------------------------------

void wxFileName::Assign( const wxFileName &filepath )
{
    m_volume = filepath.GetVolume();
    m_dirs = filepath.GetDirs();
    m_name = filepath.GetName();
    m_ext = filepath.GetExt();
    m_relative = filepath.m_relative;
}

void wxFileName::Assign(const wxString& volume,
                        const wxString& path,
                        const wxString& name,
                        const wxString& ext,
                        wxPathFormat format )
{
    SetPath( path, format );

    m_volume = volume;
    m_ext = ext;
    m_name = name;
}

void wxFileName::SetPath( const wxString &path, wxPathFormat format )
{
    m_dirs.Clear();

    if ( !path.empty() )
    {
        wxPathFormat my_format = GetFormat( format );
        wxString my_path = path;

        // 1) Determine if the path is relative or absolute.
        wxChar leadingChar = my_path[0u];

        switch (my_format)
        {
            case wxPATH_MAC:
                m_relative = leadingChar == wxT(':');

                // We then remove a leading ":". The reason is in our
                // storage form for relative paths:
                // ":dir:file.txt" actually means "./dir/file.txt" in
                // DOS notation and should get stored as
                // (relative) (dir) (file.txt)
                // "::dir:file.txt" actually means "../dir/file.txt"
                // stored as (relative) (..) (dir) (file.txt)
                // This is important only for the Mac as an empty dir
                // actually means <UP>, whereas under DOS, double
                // slashes can be ignored: "\\\\" is the same as "\\".
                if (m_relative)
                    my_path.erase( 0, 1 );
                break;

            case wxPATH_VMS:
                // TODO: what is the relative path format here?
                m_relative = FALSE;
                break;

            case wxPATH_UNIX:
                // the paths of the form "~" or "~username" are absolute
                m_relative = leadingChar != wxT('/') && leadingChar != _T('~');
                break;

            case wxPATH_DOS:
                m_relative = !IsPathSeparator(leadingChar, my_format);
                break;

            default:
                wxFAIL_MSG( wxT("error") );
                break;
        }

        // 2) Break up the path into its members. If the original path
        //    was just "/" or "\\", m_dirs will be empty. We know from
        //    the m_relative field, if this means "nothing" or "root dir".

        wxStringTokenizer tn( my_path, GetPathSeparators(my_format) );

        while ( tn.HasMoreTokens() )
        {
            wxString token = tn.GetNextToken();

            // Remove empty token under DOS and Unix, interpret them
            // as .. under Mac.
            if (token.empty())
            {
                if (my_format == wxPATH_MAC)
                    m_dirs.Add( wxT("..") );
                // else ignore
            }
            else
            {
               m_dirs.Add( token );
            }
        }
    }
    else // no path at all
    {
        m_relative = TRUE;
    }
}

void wxFileName::Assign(const wxString& fullpath,
                        wxPathFormat format)
{
    wxString volume, path, name, ext;
    SplitPath(fullpath, &volume, &path, &name, &ext, format);

    Assign(volume, path, name, ext, format);
}

void wxFileName::Assign(const wxString& fullpathOrig,
                        const wxString& fullname,
                        wxPathFormat format)
{
    // always recognize fullpath as directory, even if it doesn't end with a
    // slash
    wxString fullpath = fullpathOrig;
    if ( !wxEndsWithPathSeparator(fullpath) )
    {
        fullpath += GetPathSeparator(format);
    }

    wxString volume, path, name, ext;

    // do some consistency checks in debug mode: the name should be really just
    // the filename and the path should be really just a path
#ifdef __WXDEBUG__
    wxString pathDummy, nameDummy, extDummy;

    SplitPath(fullname, &pathDummy, &name, &ext, format);

    wxASSERT_MSG( pathDummy.empty(),
                  _T("the file name shouldn't contain the path") );

    SplitPath(fullpath, &volume, &path, &nameDummy, &extDummy, format);

    wxASSERT_MSG( nameDummy.empty() && extDummy.empty(),
                  _T("the path shouldn't contain file name nor extension") );

#else // !__WXDEBUG__
    SplitPath(fullname, NULL /* no path */, &name, &ext, format);
    SplitPath(fullpath, &volume, &path, NULL, NULL, format);
#endif // __WXDEBUG__/!__WXDEBUG__

    Assign(volume, path, name, ext, format);
}

void wxFileName::AssignDir(const wxString& dir, wxPathFormat format)
{
    Assign(dir, _T(""), format);
}

void wxFileName::Clear()
{
    m_dirs.Clear();

    m_volume =
    m_name =
    m_ext = wxEmptyString;

    // we don't have any absolute path for now
    m_relative = TRUE;
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

bool wxFileName::FileExists() const
{
    return wxFileName::FileExists( GetFullPath() );
}

bool wxFileName::FileExists( const wxString &file )
{
    return ::wxFileExists( file );
}

bool wxFileName::DirExists() const
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

void wxFileName::AssignCwd(const wxString& volume)
{
    AssignDir(wxFileName::GetCwd(volume));
}

/* static */
wxString wxFileName::GetCwd(const wxString& volume)
{
    // if we have the volume, we must get the current directory on this drive
    // and to do this we have to chdir to this volume - at least under Windows,
    // I don't know how to get the current drive on another volume elsewhere
    // (TODO)
    wxString cwdOld;
    if ( !volume.empty() )
    {
        cwdOld = wxGetCwd();
        SetCwd(volume + GetVolumeSeparator());
    }

    wxString cwd = ::wxGetCwd();

    if ( !volume.empty() )
    {
        SetCwd(cwdOld);
    }

    return cwd;
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

void wxFileName::AssignTempFileName(const wxString& prefix, wxFile *fileTemp)
{
    wxString tempname = CreateTempFileName(prefix, fileTemp);
    if ( tempname.empty() )
    {
        // error, failed to get temp file name
        Clear();
    }
    else // ok
    {
        Assign(tempname);
    }
}

/* static */
wxString
wxFileName::CreateTempFileName(const wxString& prefix, wxFile *fileTemp)
{
    wxString path, dir, name;

    // use the directory specified by the prefix
    SplitPath(prefix, &dir, &name, NULL /* extension */);

#if defined(__WINDOWS__) && !defined(__WXMICROWIN__)

#ifdef __WIN32__
    if ( dir.empty() )
    {
        if ( !::GetTempPath(MAX_PATH, wxStringBuffer(dir, MAX_PATH + 1)) )
        {
            wxLogLastError(_T("GetTempPath"));
        }

        if ( dir.empty() )
        {
            // GetTempFileName() fails if we pass it an empty string
            dir = _T('.');
        }
    }
    else // we have a dir to create the file in
    {
        // ensure we use only the back slashes as GetTempFileName(), unlike all
        // the other APIs, is picky and doesn't accept the forward ones
        dir.Replace(_T("/"), _T("\\"));
    }

    if ( !::GetTempFileName(dir, name, 0, wxStringBuffer(path, MAX_PATH + 1)) )
    {
        wxLogLastError(_T("GetTempFileName"));

        path.clear();
    }
#else // Win16
    if ( !::GetTempFileName(NULL, prefix, 0, wxStringBuffer(path, 1025)) )
    {
        path.clear();
    }
#endif // Win32/16

#elif defined(__WXPM__)
    // for now just create a file
    //
    // future enhancements can be to set some extended attributes for file
    // systems OS/2 supports that have them (HPFS, FAT32) and security
    // (HPFS386)
    static const wxChar *szMktempSuffix = wxT("XXX");
    path << dir << _T('/') << name << szMktempSuffix;

    // Temporarily remove - MN
    #ifndef __WATCOMC__
        ::DosCreateDir(wxStringBuffer(path, MAX_PATH), NULL);
    #endif

#else // !Windows, !OS/2
    if ( dir.empty() )
    {
#if defined(__WXMAC__) && !defined(__DARWIN__)
        dir = wxMacFindFolder(  (short) kOnSystemDisk, kTemporaryFolderType, kCreateFolder ) ;
#else // !Mac
        dir = wxGetenv(_T("TMP"));
        if ( dir.empty() )
        {
            dir = wxGetenv(_T("TEMP"));
        }

        if ( dir.empty() )
        {
            // default
            #ifdef __DOS__
                dir = _T(".");
            #else
                dir = _T("/tmp");
            #endif
        }
#endif // Mac/!Mac
    }

    path = dir;

    if ( !wxEndsWithPathSeparator(dir) &&
            (name.empty() || !wxIsPathSeparator(name[0u])) )
    {
        path += wxFILE_SEP_PATH;
    }

    path += name;

#if defined(HAVE_MKSTEMP)
    // scratch space for mkstemp()
    path += _T("XXXXXX");

    // we need to copy the path to the buffer in which mkstemp() can modify it
    wxCharBuffer buf( wxConvFile.cWX2MB( path ) );

    // cast is safe because the string length doesn't change
    int fdTemp = mkstemp( (char*)(const char*) buf );
    if ( fdTemp == -1 )
    {
        // this might be not necessary as mkstemp() on most systems should have
        // already done it but it doesn't hurt neither...
        path.clear();
    }
    else // mkstemp() succeeded
    {
        path = wxConvFile.cMB2WX( (const char*) buf );
        
        // avoid leaking the fd
        if ( fileTemp )
        {
            fileTemp->Attach(fdTemp);
        }
        else
        {
            close(fdTemp);
        }
    }
#else // !HAVE_MKSTEMP

#ifdef HAVE_MKTEMP
    // same as above
    path += _T("XXXXXX");

    wxCharBuffer buf = wxConvFile.cWX2MB( path );
    if ( !mktemp( (const char*) buf ) )
    {
        path.clear();
    }
    else
    {
        path = wxConvFile.cMB2WX( (const char*) buf );
    }
#else // !HAVE_MKTEMP (includes __DOS__)
    // generate the unique file name ourselves
    #if !defined(__DOS__) && (!defined(__MWERKS__) || defined(__DARWIN__) )
    path << (unsigned int)getpid();
    #endif

    wxString pathTry;

    static const size_t numTries = 1000;
    for ( size_t n = 0; n < numTries; n++ )
    {
        // 3 hex digits is enough for numTries == 1000 < 4096
        pathTry = path + wxString::Format(_T("%.03x"), n);
        if ( !wxFile::Exists(pathTry) )
        {
            break;
        }

        pathTry.clear();
    }

    path = pathTry;
#endif // HAVE_MKTEMP/!HAVE_MKTEMP

    if ( !path.empty() )
    {
    }
#endif // HAVE_MKSTEMP/!HAVE_MKSTEMP

#endif // Windows/!Windows

    if ( path.empty() )
    {
        wxLogSysError(_("Failed to create a temporary file name"));
    }
    else if ( fileTemp && !fileTemp->IsOpened() )
    {
        // open the file - of course, there is a race condition here, this is
        // why we always prefer using mkstemp()...
        //
        // NB: GetTempFileName() under Windows creates the file, so using
        //     write_excl there would fail
        if ( !fileTemp->Open(path,
#if defined(__WINDOWS__) && !defined(__WXMICROWIN__)
                             wxFile::write,
#else
                             wxFile::write_excl,
#endif
                             wxS_IRUSR | wxS_IWUSR) )
        {
            // FIXME: If !ok here should we loop and try again with another
            //        file name?  That is the standard recourse if open(O_EXCL)
            //        fails, though of course it should be protected against
            //        possible infinite looping too.

            wxLogError(_("Failed to open temporary file."));

            path.clear();
        }
    }

    return path;
}

// ----------------------------------------------------------------------------
// directory operations
// ----------------------------------------------------------------------------

bool wxFileName::Mkdir( int perm, int flags )
{
    return wxFileName::Mkdir( GetFullPath(), perm, flags );
}

bool wxFileName::Mkdir( const wxString& dir, int perm, int flags )
{
    if ( flags & wxPATH_MKDIR_FULL )
    {
        // split the path in components
        wxFileName filename;
        filename.AssignDir(dir);

        wxString currPath;
        if ( filename.HasVolume())
        {
            currPath << wxGetVolumeString(filename.GetVolume(), wxPATH_NATIVE);
        }

        wxArrayString dirs = filename.GetDirs();
        size_t count = dirs.GetCount();
        for ( size_t i = 0; i < count; i++ )
        {
            if ( i > 0 || 
#if defined(__WXMAC__) && !defined(__DARWIN__)
			// relative pathnames are exactely the other way round under mac...
            	!filename.IsAbsolute() 
#else
            	filename.IsAbsolute() 
#endif
            )
                currPath += wxFILE_SEP_PATH;
            currPath += dirs[i];

            if (!DirExists(currPath))
            {
                if (!wxMkdir(currPath, perm))
                {
                    // no need to try creating further directories
                    return FALSE;
                }
            }
        }

        return TRUE;

    }

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

bool wxFileName::Normalize(int flags,
                           const wxString& cwd,
                           wxPathFormat format)
{
    // the existing path components
    wxArrayString dirs = GetDirs();

    // the path to prepend in front to make the path absolute
    wxFileName curDir;

    format = GetFormat(format);

    // make the path absolute
    if ( (flags & wxPATH_NORM_ABSOLUTE) && !IsAbsolute(format) )
    {
        if ( cwd.empty() )
        {
            curDir.AssignCwd(GetVolume());
        }
        else // cwd provided
        {
            curDir.AssignDir(cwd);
        }

        // the path may be not absolute because it doesn't have the volume name
        // but in this case we shouldn't modify the directory components of it
        // but just set the current volume
        if ( !HasVolume() && curDir.HasVolume() )
        {
            SetVolume(curDir.GetVolume());

            if ( !m_relative )
            {
                // yes, it was the case - we don't need curDir then
                curDir.Clear();
            }
        }
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

                dirs.RemoveAt(0u);
            }
        }
    }

    // transform relative path into abs one
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

        if ( flags & wxPATH_NORM_DOTS )
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

                m_dirs.RemoveAt(m_dirs.GetCount() - 1);
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

    // we do have the path now
    //
    // NB: need to do this before (maybe) calling Assign() below
    m_relative = FALSE;

#if defined(__WIN32__)
    if ( (flags & wxPATH_NORM_LONG) && (format == wxPATH_DOS) )
    {
        Assign(GetLongPath());
    }
#endif // Win32

    return TRUE;
}

// ----------------------------------------------------------------------------
// absolute/relative paths
// ----------------------------------------------------------------------------

bool wxFileName::IsAbsolute(wxPathFormat format) const
{
    // if our path doesn't start with a path separator, it's not an absolute
    // path
    if ( m_relative )
        return FALSE;

    if ( !GetVolumeSeparator(format).empty() )
    {
        // this format has volumes and an absolute path must have one, it's not
        // enough to have the full path to bean absolute file under Windows
        if ( GetVolume().empty() )
            return FALSE;
    }

    return TRUE;
}

bool wxFileName::MakeRelativeTo(const wxString& pathBase, wxPathFormat format)
{
    wxFileName fnBase(pathBase, format);

    // get cwd only once - small time saving
    wxString cwd = wxGetCwd();
    Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, cwd, format);
    fnBase.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, cwd, format);

    bool withCase = IsCaseSensitive(format);

    // we can't do anything if the files live on different volumes
    if ( !GetVolume().IsSameAs(fnBase.GetVolume(), withCase) )
    {
        // nothing done
        return FALSE;
    }

    // same drive, so we don't need our volume
    m_volume.clear();

    // remove common directories starting at the top
    while ( !m_dirs.IsEmpty() && !fnBase.m_dirs.IsEmpty() &&
                m_dirs[0u].IsSameAs(fnBase.m_dirs[0u], withCase) )
    {
        m_dirs.RemoveAt(0);
        fnBase.m_dirs.RemoveAt(0);
    }

    // add as many ".." as needed
    size_t count = fnBase.m_dirs.GetCount();
    for ( size_t i = 0; i < count; i++ )
    {
        m_dirs.Insert(wxT(".."), 0u);
    }

    if ( format == wxPATH_UNIX || format == wxPATH_DOS )
    {
        // a directory made relative with respect to itself is '.' under Unix
        // and DOS, by definition (but we don't have to insert "./" for the
        // files)
        if ( m_dirs.IsEmpty() && IsDir() )
        {
            m_dirs.Add(_T('.'));
        }
    }

    m_relative = TRUE;

    // we were modified
    return TRUE;
}

// ----------------------------------------------------------------------------
// filename kind tests
// ----------------------------------------------------------------------------

bool wxFileName::SameAs(const wxFileName& filepath, wxPathFormat format) const
{
    wxFileName fn1 = *this,
               fn2 = filepath;

    // get cwd only once - small time saving
    wxString cwd = wxGetCwd();
    fn1.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, cwd, format);
    fn2.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, cwd, format);

    if ( fn1.GetFullPath() == fn2.GetFullPath() )
        return TRUE;

    // TODO: compare inodes for Unix, this works even when filenames are
    //       different but files are the same (symlinks) (VZ)

    return FALSE;
}

/* static */
bool wxFileName::IsCaseSensitive( wxPathFormat format )
{
    // only Unix filenames are truely case-sensitive
    return GetFormat(format) == wxPATH_UNIX;
}

/* static */
wxString wxFileName::GetVolumeSeparator(wxPathFormat format)
{
    wxString sepVol;

    if ( (GetFormat(format) == wxPATH_DOS) ||
         (GetFormat(format) == wxPATH_VMS) )
    {
        sepVol = wxFILE_SEP_DSK;
    }
    //else: leave empty

    return sepVol;
}

/* static */
wxString wxFileName::GetPathSeparators(wxPathFormat format)
{
    wxString seps;
    switch ( GetFormat(format) )
    {
        case wxPATH_DOS:
            // accept both as native APIs do but put the native one first as
            // this is the one we use in GetFullPath()
            seps << wxFILE_SEP_PATH_DOS << wxFILE_SEP_PATH_UNIX;
            break;

        default:
            wxFAIL_MSG( _T("unknown wxPATH_XXX style") );
            // fall through

        case wxPATH_UNIX:
            seps = wxFILE_SEP_PATH_UNIX;
            break;

        case wxPATH_MAC:
            seps = wxFILE_SEP_PATH_MAC;
            break;

        case wxPATH_VMS:
            seps = wxFILE_SEP_PATH_VMS;
            break;
    }

    return seps;
}

/* static */
bool wxFileName::IsPathSeparator(wxChar ch, wxPathFormat format)
{
    // wxString::Find() doesn't work as expected with NUL - it will always find
    // it, so it is almost surely a bug if this function is called with NUL arg
    wxASSERT_MSG( ch != _T('\0'), _T("shouldn't be called with NUL") );

    return GetPathSeparators(format).Find(ch) != wxNOT_FOUND;
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
    m_dirs.RemoveAt( (size_t)pos );
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

void wxFileName::SetFullName(const wxString& fullname)
{
    SplitPath(fullname, NULL /* no path */, &m_name, &m_ext);
}

wxString wxFileName::GetFullName() const
{
    wxString fullname = m_name;
    if ( !m_ext.empty() )
    {
        fullname << wxFILE_SEP_EXT << m_ext;
    }

    return fullname;
}

wxString wxFileName::GetPath( int flags, wxPathFormat format ) const
{
    format = GetFormat( format );

    wxString fullpath;

    // return the volume with the path as well if requested
    if ( flags & wxPATH_GET_VOLUME )
    {
        fullpath += wxGetVolumeString(GetVolume(), format);
    }

    // the leading character
    switch ( format )
    {
        case wxPATH_MAC:
            if ( m_relative )
                fullpath += wxFILE_SEP_PATH_MAC;
            break;

        case wxPATH_DOS:
            if (!m_relative)
                fullpath += wxFILE_SEP_PATH_DOS;
            break;

        default:
            wxFAIL_MSG( _T("unknown path format") );
            // fall through

        case wxPATH_UNIX:
            if ( !m_relative )
            {
                // normally the absolute file names starts with a slash with
                // one exception: file names like "~/foo.bar" don't have it
                if ( m_dirs.IsEmpty() || m_dirs[0u] != _T('~') )
                {
                    fullpath += wxFILE_SEP_PATH_UNIX;
                }
            }
            break;

        case wxPATH_VMS:
            // no leading character here but use this place to unset
            // wxPATH_GET_SEPARATOR flag: under VMS it doesn't make sense as,
            // if I understand correctly, there should never be a dot before
            // the closing bracket
            flags &= ~wxPATH_GET_SEPARATOR;
    }

    // then concatenate all the path components using the path separator
    size_t dirCount = m_dirs.GetCount();
    if ( dirCount )
    {
        if ( format == wxPATH_VMS )
        {
            fullpath += wxT('[');
        }

        for ( size_t i = 0; i < dirCount; i++ )
        {
            switch (format)
            {
                case wxPATH_MAC:
                    if ( m_dirs[i] == wxT(".") )
                    {
                        // skip appending ':', this shouldn't be done in this
                        // case as "::" is interpreted as ".." under Unix
                        continue;
                    }

                    // convert back from ".." to nothing
                    if ( m_dirs[i] != wxT("..") )
                         fullpath += m_dirs[i];
                    break;

                default:
                    wxFAIL_MSG( wxT("unexpected path format") );
                    // still fall through

                case wxPATH_DOS:
                case wxPATH_UNIX:
                    fullpath += m_dirs[i];
                    break;

                case wxPATH_VMS:
                    // TODO: What to do with ".." under VMS

                    // convert back from ".." to nothing
                    if ( m_dirs[i] != wxT("..") )
                        fullpath += m_dirs[i];
                    break;
            }

            if ( (flags & wxPATH_GET_SEPARATOR) || (i != dirCount - 1) )
                fullpath += GetPathSeparator(format);
        }

        if ( format == wxPATH_VMS )
        {
            fullpath += wxT(']');
        }
    }

    return fullpath;
}

wxString wxFileName::GetFullPath( wxPathFormat format ) const
{
    // we already have a function to get the path
    wxString fullpath = GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
                                format);

    // now just add the file name and extension to it
    fullpath += GetFullName();

    return fullpath;
}

// Return the short form of the path (returns identity on non-Windows platforms)
wxString wxFileName::GetShortPath() const
{
#if defined(__WXMSW__) && defined(__WIN32__) && !defined(__WXMICROWIN__)
    wxString path(GetFullPath());
    wxString pathOut;
    DWORD sz = ::GetShortPathName(path, NULL, 0);
    bool ok = sz != 0;
    if ( ok )
    {
        ok = ::GetShortPathName
               (
                path,
                pathOut.GetWriteBuf(sz),
                sz
               ) != 0;
        pathOut.UngetWriteBuf();
    }
    if (ok)
        return pathOut;

    return path;
#else
    return GetFullPath();
#endif
}

// Return the long form of the path (returns identity on non-Windows platforms)
wxString wxFileName::GetLongPath() const
{
    wxString pathOut,
             path = GetFullPath();

#if defined(__WIN32__) && !defined(__WXMICROWIN__)
    bool success = FALSE;

#if wxUSE_DYNAMIC_LOADER
    typedef DWORD (WINAPI *GET_LONG_PATH_NAME)(const wxChar *, wxChar *, DWORD);

    static bool s_triedToLoad = FALSE;

    if ( !s_triedToLoad )
    {
        // suppress the errors about missing GetLongPathName[AW]
        wxLogNull noLog;

        s_triedToLoad = TRUE;
        wxDynamicLibrary dllKernel(_T("kernel32"));
        if ( dllKernel.IsLoaded() )
        {
            // may succeed or fail depending on the Windows version
            static GET_LONG_PATH_NAME s_pfnGetLongPathName = NULL;
#ifdef _UNICODE
            s_pfnGetLongPathName = (GET_LONG_PATH_NAME) dllKernel.GetSymbol(_T("GetLongPathNameW"));
#else
            s_pfnGetLongPathName = (GET_LONG_PATH_NAME) dllKernel.GetSymbol(_T("GetLongPathNameA"));
#endif

            if ( s_pfnGetLongPathName )
            {
                DWORD dwSize = (*s_pfnGetLongPathName)(path, NULL, 0);
                bool ok = dwSize > 0;

                if ( ok )
                {
                    DWORD sz = (*s_pfnGetLongPathName)(path, NULL, 0);
                    ok = sz != 0;
                    if ( ok )
                    {
                        ok = (*s_pfnGetLongPathName)
                                (
                                path,
                                pathOut.GetWriteBuf(sz),
                                sz
                                ) != 0;
                        pathOut.UngetWriteBuf();

                        success = TRUE;
                    }
                }
            }
        }
    }

    if (success)
        return pathOut;
#endif // wxUSE_DYNAMIC_LOADER

    if (!success)
    {
        // The OS didn't support GetLongPathName, or some other error.
        // We need to call FindFirstFile on each component in turn.

        WIN32_FIND_DATA findFileData;
        HANDLE hFind;

        if ( HasVolume() )
            pathOut = GetVolume() +
                      GetVolumeSeparator(wxPATH_DOS) +
                      GetPathSeparator(wxPATH_DOS);
        else
            pathOut = wxEmptyString;

        wxArrayString dirs = GetDirs();
        dirs.Add(GetFullName());

        wxString tmpPath;

        size_t count = dirs.GetCount();
        for ( size_t i = 0; i < count; i++ )
        {
            // We're using pathOut to collect the long-name path, but using a
            // temporary for appending the last path component which may be
            // short-name
            tmpPath = pathOut + dirs[i];

            if ( tmpPath.empty() )
                continue;

            // can't see this being necessary? MF
            if ( tmpPath.Last() == GetVolumeSeparator(wxPATH_DOS) )
            {
                // Can't pass a drive and root dir to FindFirstFile,
                // so continue to next dir
                tmpPath += wxFILE_SEP_PATH;
                pathOut = tmpPath;
                continue;
            }

            hFind = ::FindFirstFile(tmpPath, &findFileData);
            if (hFind == INVALID_HANDLE_VALUE)
            {
                // Error: most likely reason is that path doesn't exist, so
                // append any unprocessed parts and return
                for ( i += 1; i < count; i++ )
                    tmpPath += wxFILE_SEP_PATH + dirs[i];

                return tmpPath;
            }

            pathOut += findFileData.cFileName;
            if ( (i < (count-1)) )
                pathOut += wxFILE_SEP_PATH;

            ::FindClose(hFind);
        }
    }
#else // !Win32
    pathOut = path;
#endif // Win32/!Win32

    return pathOut;
}

wxPathFormat wxFileName::GetFormat( wxPathFormat format )
{
    if (format == wxPATH_NATIVE)
    {
#if defined(__WXMSW__) || defined(__WXPM__) || defined(__DOS__)
        format = wxPATH_DOS;
#elif defined(__WXMAC__) && !defined(__DARWIN__)
        format = wxPATH_MAC;
#elif defined(__VMS)
        format = wxPATH_VMS;
#else
        format = wxPATH_UNIX;
#endif
    }
    return format;
}

// ----------------------------------------------------------------------------
// path splitting function
// ----------------------------------------------------------------------------

/* static */
void wxFileName::SplitPath(const wxString& fullpathWithVolume,
                           wxString *pstrVolume,
                           wxString *pstrPath,
                           wxString *pstrName,
                           wxString *pstrExt,
                           wxPathFormat format)
{
    format = GetFormat(format);

    wxString fullpath = fullpathWithVolume;

    // under VMS the end of the path is ']', not the path separator used to
    // separate the components
    wxString sepPath = format == wxPATH_VMS ? wxString(_T(']'))
                                            : GetPathSeparators(format);

    // special Windows UNC paths hack: transform \\share\path into share:path
    if ( format == wxPATH_DOS )
    {
        if ( fullpath.length() >= 4 &&
                fullpath[0u] == wxFILE_SEP_PATH_DOS &&
                    fullpath[1u] == wxFILE_SEP_PATH_DOS )
        {
            fullpath.erase(0, 2);

            size_t posFirstSlash = fullpath.find_first_of(sepPath);
            if ( posFirstSlash != wxString::npos )
            {
                fullpath[posFirstSlash] = wxFILE_SEP_DSK;

                // UNC paths are always absolute, right? (FIXME)
                fullpath.insert(posFirstSlash + 1, wxFILE_SEP_PATH_DOS);
            }
        }
    }

    // We separate the volume here
    if ( format == wxPATH_DOS || format == wxPATH_VMS )
    {
        wxString sepVol = GetVolumeSeparator(format);

        size_t posFirstColon = fullpath.find_first_of(sepVol);
        if ( posFirstColon != wxString::npos )
        {
            if ( pstrVolume )
            {
                *pstrVolume = fullpath.Left(posFirstColon);
            }

            // remove the volume name and the separator from the full path
            fullpath.erase(0, posFirstColon + sepVol.length());
        }
    }

    // find the positions of the last dot and last path separator in the path
    size_t posLastDot = fullpath.find_last_of(wxFILE_SEP_EXT);
    size_t posLastSlash = fullpath.find_last_of(sepPath);

    if ( (posLastDot != wxString::npos) &&
            ((format == wxPATH_UNIX) || (format == wxPATH_VMS)) )
    {
        if ( (posLastDot == 0) ||
             (fullpath[posLastDot - 1] == sepPath[0u] ) )
        {
            // under Unix and VMS, dot may be (and commonly is) the first
            // character of the filename, don't treat the entire filename as
            // extension in this case
            posLastDot = wxString::npos;
        }
    }

    // if we do have a dot and a slash, check that the dot is in the name part
    if ( (posLastDot != wxString::npos) &&
         (posLastSlash != wxString::npos) &&
         (posLastDot < posLastSlash) )
    {
        // the dot is part of the path, not the start of the extension
        posLastDot = wxString::npos;
    }

    // now fill in the variables provided by user
    if ( pstrPath )
    {
        if ( posLastSlash == wxString::npos )
        {
            // no path at all
            pstrPath->Empty();
        }
        else
        {
            // take everything up to the path separator but take care to make
            // the path equal to something like '/', not empty, for the files
            // immediately under root directory
            size_t len = posLastSlash;

            // this rule does not apply to mac since we do not start with colons (sep)
            // except for relative paths
            if ( !len && format != wxPATH_MAC)
                len++;

            *pstrPath = fullpath.Left(len);

            // special VMS hack: remove the initial bracket
            if ( format == wxPATH_VMS )
            {
                if ( (*pstrPath)[0u] == _T('[') )
                    pstrPath->erase(0, 1);
            }
        }
    }

    if ( pstrName )
    {
        // take all characters starting from the one after the last slash and
        // up to, but excluding, the last dot
        size_t nStart = posLastSlash == wxString::npos ? 0 : posLastSlash + 1;
        size_t count;
        if ( posLastDot == wxString::npos )
        {
            // take all until the end
            count = wxString::npos;
        }
        else if ( posLastSlash == wxString::npos )
        {
            count = posLastDot;
        }
        else // have both dot and slash
        {
            count = posLastDot - posLastSlash - 1;
        }

        *pstrName = fullpath.Mid(nStart, count);
    }

    if ( pstrExt )
    {
        if ( posLastDot == wxString::npos )
        {
            // no extension
            pstrExt->Empty();
        }
        else
        {
            // take everything after the dot
            *pstrExt = fullpath.Mid(posLastDot + 1);
        }
    }
}

/* static */
void wxFileName::SplitPath(const wxString& fullpath,
                           wxString *path,
                           wxString *name,
                           wxString *ext,
                           wxPathFormat format)
{
    wxString volume;
    SplitPath(fullpath, &volume, path, name, ext, format);

    if ( path )
    {
        path->Prepend(wxGetVolumeString(volume, format));
    }
}

// ----------------------------------------------------------------------------
// time functions
// ----------------------------------------------------------------------------

#if wxUSE_DATETIME

bool wxFileName::SetTimes(const wxDateTime *dtAccess,
                          const wxDateTime *dtMod,
                          const wxDateTime *dtCreate)
{
#if defined(__WIN32__)
    if ( IsDir() )
    {
        // VZ: please let me know how to do this if you can
        wxFAIL_MSG( _T("SetTimes() not implemented for the directories") );
    }
    else // file
    {
        wxFileHandle fh(GetFullPath(), wxFileHandle::Write);
        if ( fh.IsOk() )
        {
            FILETIME ftAccess, ftCreate, ftWrite;

            if ( dtCreate )
                ConvertWxToFileTime(&ftCreate, *dtCreate);
            if ( dtAccess )
                ConvertWxToFileTime(&ftAccess, *dtAccess);
            if ( dtMod )
                ConvertWxToFileTime(&ftWrite, *dtMod);

            if ( ::SetFileTime(fh,
                               dtCreate ? &ftCreate : NULL,
                               dtAccess ? &ftAccess : NULL,
                               dtMod ? &ftWrite : NULL) )
            {
                return TRUE;
            }
        }
    }
#elif defined(__UNIX_LIKE__) || (defined(__DOS__) && defined(__WATCOMC__))
    if ( !dtAccess && !dtMod )
    {
        // can't modify the creation time anyhow, don't try
        return TRUE;
    }

    // if dtAccess or dtMod is not specified, use the other one (which must be
    // non NULL because of the test above) for both times
    utimbuf utm;
    utm.actime = dtAccess ? dtAccess->GetTicks() : dtMod->GetTicks();
    utm.modtime = dtMod ? dtMod->GetTicks() : dtAccess->GetTicks();
    if ( utime(GetFullPath().fn_str(), &utm) == 0 )
    {
        return TRUE;
    }
#else // other platform
#endif // platforms

    wxLogSysError(_("Failed to modify file times for '%s'"),
                  GetFullPath().c_str());

    return FALSE;
}

bool wxFileName::Touch()
{
#if defined(__UNIX_LIKE__)
    // under Unix touching file is simple: just pass NULL to utime()
    if ( utime(GetFullPath().fn_str(), NULL) == 0 )
    {
        return TRUE;
    }

    wxLogSysError(_("Failed to touch the file '%s'"), GetFullPath().c_str());

    return FALSE;
#else // other platform
    wxDateTime dtNow = wxDateTime::Now();

    return SetTimes(&dtNow, &dtNow, NULL /* don't change create time */);
#endif // platforms
}

bool wxFileName::GetTimes(wxDateTime *dtAccess,
                          wxDateTime *dtMod,
                          wxDateTime *dtCreate) const
{
#if defined(__WIN32__)
    // we must use different methods for the files and directories under
    // Windows as CreateFile(GENERIC_READ) doesn't work for the directories and
    // CreateFile(FILE_FLAG_BACKUP_SEMANTICS) works -- but only under NT and
    // not 9x
    bool ok;
    FILETIME ftAccess, ftCreate, ftWrite;
    if ( IsDir() )
    {
        // implemented in msw/dir.cpp
        extern bool wxGetDirectoryTimes(const wxString& dirname,
                                        FILETIME *, FILETIME *, FILETIME *);

        // we should pass the path without the trailing separator to
        // wxGetDirectoryTimes()
        ok = wxGetDirectoryTimes(GetPath(wxPATH_GET_VOLUME),
                                 &ftAccess, &ftCreate, &ftWrite);
    }
    else // file
    {
        wxFileHandle fh(GetFullPath(), wxFileHandle::Read);
        if ( fh.IsOk() )
        {
            ok = ::GetFileTime(fh,
                               dtCreate ? &ftCreate : NULL,
                               dtAccess ? &ftAccess : NULL,
                               dtMod ? &ftWrite : NULL) != 0;
        }
        else
        {
            ok = FALSE;
        }
    }

    if ( ok )
    {
        if ( dtCreate )
            ConvertFileTimeToWx(dtCreate, ftCreate);
        if ( dtAccess )
            ConvertFileTimeToWx(dtAccess, ftAccess);
        if ( dtMod )
            ConvertFileTimeToWx(dtMod, ftWrite);

        return TRUE;
    }
#elif defined(__UNIX_LIKE__) || defined(__WXMAC__) || (defined(__DOS__) && defined(__WATCOMC__))
    wxStructStat stBuf;
    if ( wxStat( GetFullPath().c_str(), &stBuf) == 0 )
    {
        if ( dtAccess )
            dtAccess->Set(stBuf.st_atime);
        if ( dtMod )
            dtMod->Set(stBuf.st_mtime);
        if ( dtCreate )
            dtCreate->Set(stBuf.st_ctime);

        return TRUE;
    }
#else // other platform
#endif // platforms

    wxLogSysError(_("Failed to retrieve file times for '%s'"),
                  GetFullPath().c_str());

    return FALSE;
}

#endif // wxUSE_DATETIME

#ifdef __WXMAC__

const short kMacExtensionMaxLength = 16 ;
class MacDefaultExtensionRecord
{
public :
  MacDefaultExtensionRecord()
  {
    m_ext[0] = 0 ;
    m_type = m_creator = NULL ;
  }
  MacDefaultExtensionRecord( const MacDefaultExtensionRecord& from )
  {
    wxStrcpy( m_ext , from.m_ext ) ;
    m_type = from.m_type ;
    m_creator = from.m_creator ;
  }
  MacDefaultExtensionRecord( const wxChar * extension , OSType type , OSType creator )
  {
    wxStrncpy( m_ext , extension , kMacExtensionMaxLength ) ;
    m_ext[kMacExtensionMaxLength] = 0 ;
    m_type = type ;
    m_creator = creator ;
  }
  wxChar m_ext[kMacExtensionMaxLength] ;
  OSType m_type ;
  OSType m_creator ;
}  ;

#include "wx/dynarray.h"
WX_DECLARE_OBJARRAY(MacDefaultExtensionRecord, MacDefaultExtensionArray) ;

bool gMacDefaultExtensionsInited = false ;

#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(MacDefaultExtensionArray) ;

MacDefaultExtensionArray gMacDefaultExtensions ;

static void MacEnsureDefaultExtensionsLoaded()
{
  if ( !gMacDefaultExtensionsInited )
  {

    // load the default extensions
    MacDefaultExtensionRecord defaults[1] =
    {
      MacDefaultExtensionRecord( wxT("txt") , 'TEXT' , 'ttxt' ) ,

    } ;
    // we could load the pc exchange prefs here too

    for ( size_t i = 0 ; i < WXSIZEOF( defaults ) ; ++i )
    {
      gMacDefaultExtensions.Add( defaults[i] ) ;
    }
    gMacDefaultExtensionsInited = true ;
  }
}
bool wxFileName::MacSetTypeAndCreator( wxUint32 type , wxUint32 creator )
{
  FInfo fndrInfo ;
  FSSpec spec ;
  wxMacFilename2FSSpec(GetFullPath(),&spec) ;
  OSErr err = FSpGetFInfo( &spec , &fndrInfo ) ;
  wxCHECK( err == noErr , false ) ;

  fndrInfo.fdType = type ;
  fndrInfo.fdCreator = creator ;
  FSpSetFInfo( &spec , &fndrInfo ) ;
  return true ;
}

bool wxFileName::MacGetTypeAndCreator( wxUint32 *type , wxUint32 *creator )
{
  FInfo fndrInfo ;
  FSSpec spec ;
  wxMacFilename2FSSpec(GetFullPath(),&spec) ;
  OSErr err = FSpGetFInfo( &spec , &fndrInfo ) ;
  wxCHECK( err == noErr , false ) ;

  *type = fndrInfo.fdType ;
  *creator = fndrInfo.fdCreator ;
  return true ;
}

bool wxFileName::MacSetDefaultTypeAndCreator()
{
    wxUint32 type , creator ;
    if ( wxFileName::MacFindDefaultTypeAndCreator(GetExt() , &type ,
      &creator ) )
    {
        return MacSetTypeAndCreator( type , creator ) ;
    }
    return false;
}

bool wxFileName::MacFindDefaultTypeAndCreator( const wxString& ext , wxUint32 *type , wxUint32 *creator )
{
  MacEnsureDefaultExtensionsLoaded() ;
  wxString extl = ext.Lower() ;
  for( int i = gMacDefaultExtensions.Count() - 1 ; i >= 0 ; --i )
  {
    if ( gMacDefaultExtensions.Item(i).m_ext == extl )
    {
      *type = gMacDefaultExtensions.Item(i).m_type ;
      *creator = gMacDefaultExtensions.Item(i).m_creator ;
      return true ;
    }
  }
  return false ;
}

void wxFileName::MacRegisterDefaultTypeAndCreator( const wxString& ext , wxUint32 type , wxUint32 creator )
{
  MacEnsureDefaultExtensionsLoaded() ;
  MacDefaultExtensionRecord rec ;
  rec.m_type = type ;
  rec.m_creator = creator ;
  wxStrncpy( rec.m_ext , ext.Lower().c_str() , kMacExtensionMaxLength ) ;
  gMacDefaultExtensions.Add( rec ) ;
}
#endif
