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

#if wxUSE_DYNLIB_CLASS
    #include "wx/dynlib.h"
#endif

// For GetShort/LongPathName
#ifdef __WIN32__
    #include <windows.h>

    #include "wx/msw/winundef.h"
#endif

// utime() is POSIX so should normally be available on all Unices
#ifdef __UNIX_LIKE__
    #include <sys/types.h>
    #include <utime.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// small helper class which opens and closes the file - we use it just to get
// a file handle for the given file name to pass it to some Win32 API function
#ifdef __WIN32__

class wxFileHandle
{
public:
    wxFileHandle(const wxString& filename)
    {
        m_hFile = ::CreateFile
                    (
                     filename,          // name
                     GENERIC_READ,      // access mask
                     0,                 // no sharing
                     NULL,              // no secutity attr
                     OPEN_EXISTING,     // creation disposition
                     0,                 // no flags
                     NULL               // no template file
                    );

        if ( m_hFile == INVALID_HANDLE_VALUE )
        {
            wxLogSysError(_("Failed to open '%s' for reading"),
                          filename.c_str());
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

#ifdef __WIN32__

// convert between wxDateTime and FILETIME which is a 64-bit value representing
// the number of 100-nanosecond intervals since January 1, 1601.

static void ConvertWxToFileTime(FILETIME *ft, const wxDateTime& dt)
{
    // TODO
}

static void ConvertFileTimeToWx(wxDateTime *dt, const FILETIME &ft)
{
    // TODO
}

#endif // __WIN32__

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
    int i = 0;
    m_dirs.Clear();
    while ( tn.HasMoreTokens() )
    {
        wxString token = tn.GetNextToken();

        // If the path starts with a slash (or two for a network path),
        // we need the first dir entry to be an empty for later reassembly.
        if ((i < 2) || !token.IsEmpty())
            m_dirs.Add( token );

        i ++;
    }

    m_ext = ext;
    m_name = name;
}

void wxFileName::Assign(const wxString& fullpath,
                        wxPathFormat format)
{
    wxString path, name, ext;
    SplitPath(fullpath, &path, &name, &ext, format);

    Assign(path, name, ext, format);
}

void wxFileName::Assign(const wxString& path,
                        const wxString& fullname,
                        wxPathFormat format)
{
    wxString name, ext;
    SplitPath(fullname, NULL /* no path */, &name, &ext, format);

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

bool wxFileName::Mkdir( int perm, bool full )
{
    return wxFileName::Mkdir( GetFullPath(), perm, full );
}

bool wxFileName::Mkdir( const wxString &dir, int perm, bool full )
{
    if (full)
    {
        wxFileName filename(dir);
        wxArrayString dirs = filename.GetDirs();
        dirs.Add(filename.GetName());

        size_t count = dirs.GetCount();
        size_t i;
        wxString currPath;
        int noErrors = 0;
        for ( i = 0; i < count; i++ )
        {
            currPath += dirs[i];

            if (currPath.Last() == wxT(':'))
            {
                // Can't create a root directory so continue to next dir
                currPath += wxFILE_SEP_PATH;
                continue;
            }

            if (!DirExists(currPath))
                if (!wxMkdir(currPath, perm))
                    noErrors ++;

            if ( (i < (count-1)) )
                currPath += wxFILE_SEP_PATH;
        }

        return (noErrors == 0);

    }
    else
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

                dirs.RemoveAt(0u);
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

#if defined(__WXMSW__) && defined(__WIN32__)
    if (flags & wxPATH_NORM_LONG)
    {
        Assign(GetLongPath());
    }
#endif

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

    // Hack to cope with e.g. c:\thing - need something better
    wxChar driveSep = _T('\0');
    if (!m_dirs.IsEmpty() && m_dirs[0].Length() > 1)
        driveSep = m_dirs[0u][1u];

    // the path is absolute if it starts with a path separator or, only for
    // Unix filenames, with "~" or "~user"
    return IsPathSeparator(ch, format) ||
           driveSep == _T(':') ||
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
            seps << wxFILE_SEP_PATH_UNIX << wxFILE_SEP_PATH_DOS;
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
	(void)format;
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
#if defined(__WXMSW__) && defined(__WIN32__) && !defined(__WXMICROWIN__)
    wxString path(GetFullPath());
    wxString pathOut;
    bool success = FALSE;

    // VZ: this code was disabled, why?
#if 0 // wxUSE_DYNLIB_CLASS
    typedef DWORD (*GET_LONG_PATH_NAME)(const wxChar *, wxChar *, DWORD);

    static bool s_triedToLoad = FALSE;

    if ( !s_triedToLoad )
    {
        s_triedToLoad = TRUE;
        wxDllType dllKernel = wxDllLoader::LoadLibrary(_T("kernel32"));
        if ( dllKernel )
        {
            // may succeed or fail depending on the Windows version
			static GET_LONG_PATH_NAME s_pfnGetLongPathName = NULL;
#ifdef _UNICODE
            s_pfnGetLongPathName = (GET_LONG_PATH_NAME) wxDllLoader::GetSymbol(dllKernel, _T("GetLongPathNameW"));
#else
            s_pfnGetLongPathName = (GET_LONG_PATH_NAME) wxDllLoader::GetSymbol(dllKernel, _T("GetLongPathNameA"));
#endif

            wxDllLoader::UnloadLibrary(dllKernel);

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
#endif // wxUSE_DYNLIB_CLASS

    if (!success)
    {
        // The OS didn't support GetLongPathName, or some other error.
        // We need to call FindFirstFile on each component in turn.

        WIN32_FIND_DATA findFileData;
        HANDLE hFind;
        pathOut = wxEmptyString;

        wxArrayString dirs = GetDirs();
        dirs.Add(GetFullName());

        size_t count = dirs.GetCount();
        size_t i;
        wxString tmpPath;

        for ( i = 0; i < count; i++ )
        {
            // We're using pathOut to collect the long-name path,
            // but using a temporary for appending the last path component which may be short-name
            tmpPath = pathOut + dirs[i];

            if (tmpPath.Last() == wxT(':'))
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
                // Error: return immediately with the original path
                return path;
            }
            else
            {
                pathOut += findFileData.cFileName;
                if ( (i < (count-1)) )
                    pathOut += wxFILE_SEP_PATH;

                ::FindClose(hFind);
            }
        }
    }

    return pathOut;
#else
    return GetFullPath();
#endif
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

// ----------------------------------------------------------------------------
// path splitting function
// ----------------------------------------------------------------------------

void wxFileName::SplitPath(const wxString& fullpath,
                           wxString *pstrPath,
                           wxString *pstrName,
                           wxString *pstrExt,
                           wxPathFormat format)
{
    format = GetFormat(format);

    // find the positions of the last dot and last path separator in the path
    size_t posLastDot = fullpath.find_last_of(wxFILE_SEP_EXT);
    size_t posLastSlash = fullpath.find_last_of(GetPathSeparators(format));

    if ( (posLastDot != wxString::npos) && (format == wxPATH_UNIX) )
    {
        if ( (posLastDot == 0) ||
             (fullpath[posLastDot - 1] == wxFILE_SEP_PATH_UNIX) )
        {
            // under Unix, dot may be (and commonly is) the first character of
            // the filename, don't treat the entire filename as extension in
            // this case
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
            // take all until the separator
            *pstrPath = fullpath.Left(posLastSlash);
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

// ----------------------------------------------------------------------------
// time functions
// ----------------------------------------------------------------------------

bool wxFileName::SetTimes(const wxDateTime *dtCreate,
                          const wxDateTime *dtAccess,
                          const wxDateTime *dtMod)
{
#if defined(__UNIX_LIKE__)
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
    if ( utime(GetFullPath(), &utm) == 0 )
    {
        return TRUE;
    }
#elif defined(__WIN32__)
    wxFileHandle fh(GetFullPath());
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
    if ( utime(GetFullPath(), NULL) == 0 )
    {
        return TRUE;
    }

    wxLogSysError(_("Failed to touch the file '%s'"), GetFullPath().c_str());

    return FALSE;
#else // other platform
    wxDateTime dtNow = wxDateTime::Now();

    return SetTimes(NULL /* don't change create time */, &dtNow, &dtNow);
#endif // platforms
}

bool wxFileName::GetTimes(wxDateTime *dtAccess,
                          wxDateTime *dtMod,
                          wxDateTime *dtChange) const
{
#if defined(__UNIX_LIKE__)
    wxStructStat stBuf;
    if ( wxStat(GetFullPath(), &stBuf) == 0 )
    {
        if ( dtAccess )
            dtAccess->Set(stBuf.st_atime);
        if ( dtMod )
            dtMod->Set(stBuf.st_mtime);
        if ( dtChange )
            dtChange->Set(stBuf.st_ctime);

        return TRUE;
    }
#elif defined(__WIN32__)
    wxFileHandle fh(GetFullPath());
    if ( fh.IsOk() )
    {
        FILETIME ftAccess, ftCreate, ftWrite;

        if ( ::GetFileTime(fh,
                           dtMod ? &ftCreate : NULL,
                           dtAccess ? &ftAccess : NULL,
                           dtChange ? &ftWrite : NULL) )
        {
            if ( dtMod )
                ConvertFileTimeToWx(dtMod, ftCreate);
            if ( dtAccess )
                ConvertFileTimeToWx(dtAccess, ftAccess);
            if ( dtChange )
                ConvertFileTimeToWx(dtChange, ftWrite);

            return TRUE;
        }
    }
#else // other platform
#endif // platforms

    wxLogSysError(_("Failed to retrieve file times for '%s'"),
                  GetFullPath().c_str());

    return FALSE;
}

