/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dir.cpp
// Purpose:     wxDir implementation for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
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
#include "wx/filefn.h"          // for wxPathExists()

// ----------------------------------------------------------------------------
// define the types and functions used for file searching
// ----------------------------------------------------------------------------

// under Win16 use compiler-specific functions
#ifdef __WIN16__
    #ifdef __VISUALC__
        #include <dos.h>
        #include <errno.h>

        typedef struct _find_t FIND_STRUCT;
    #elif defined(__BORLANDC__)
        #include <dir.h>

        typedef struct ffblk FIND_STRUCT;
    #else
        #error "No directory searching functions for this compiler"
    #endif

    typedef FIND_STRUCT *FIND_DATA;
    typedef char FIND_ATTR;

    static inline FIND_DATA InitFindData() { return (FIND_DATA)NULL; }
    static inline bool IsFindDataOk(FIND_DATA fd) { return fd != NULL; }
    static inline void FreeFindData(FIND_DATA fd) { free(fd); }

    static inline FIND_DATA FindFirst(const wxString& spec,
                                      FIND_STRUCT * WXUNUSED(finddata))
    {
        // attribute to find all files
        static const FIND_ATTR attr = 0x3F;

        FIND_DATA fd = (FIND_DATA)malloc(sizeof(FIND_STRUCT));

        if (
        #ifdef __VISUALC__
            _dos_findfirst(spec, attr, fd) == 0
        #else // Borland
            findfirst(spec, fd, attr) == 0
        #endif
           )
        {
            return fd;
        }
        else
        {
            free(fd);

            return NULL;
        }
    }

    static inline bool FindNext(FIND_DATA fd, FIND_STRUCT * WXUNUSED(finddata))
    {
        #ifdef __VISUALC__
            return _dos_findnext(fd) == 0;
        #else // Borland
            return findnext(fd) == 0;
        #endif
    }

    static const wxChar *GetNameFromFindData(FIND_STRUCT *finddata)
    {
        #ifdef __VISUALC__
            return finddata->name;
        #else // Borland
            return finddata->ff_name;
        #endif
    }

    static const FIND_ATTR GetAttrFromFindData(FIND_STRUCT *finddata)
    {
        #ifdef __VISUALC__
            return finddata->attrib;
        #else // Borland
            return finddata->ff_attrib;
        #endif
    }

    static inline bool IsDir(FIND_ATTR attr)
    {
        return (attr & _A_SUBDIR) != 0;
    }

    static inline bool IsHidden(FIND_ATTR attr)
    {
        return (attr & (_A_SYSTEM | _A_HIDDEN)) != 0;
    }
#else // Win32
    #include <windows.h>

    typedef WIN32_FIND_DATA FIND_STRUCT;
    typedef HANDLE FIND_DATA;
    typedef DWORD FIND_ATTR;

    static inline FIND_DATA InitFindData() { return INVALID_HANDLE_VALUE; }

    static inline bool IsFindDataOk(FIND_DATA fd)
    {
        return fd != INVALID_HANDLE_VALUE;
    }

    static inline void FreeFindData(FIND_DATA fd)
    {
        if ( !::FindClose(fd) )
        {
            wxLogLastError(_T("FindClose"));
        }
    }

    static inline FIND_DATA FindFirst(const wxString& spec,
                                      FIND_STRUCT *finddata)
    {
        return ::FindFirstFile(spec, finddata);
    }

    static inline bool FindNext(FIND_DATA fd, FIND_STRUCT *finddata)
    {
        return ::FindNextFile(fd, finddata) != 0;
    }

    static const wxChar *GetNameFromFindData(FIND_STRUCT *finddata)
    {
        return finddata->cFileName;
    }

    static const FIND_ATTR GetAttrFromFindData(FIND_STRUCT *finddata)
    {
        return finddata->dwFileAttributes;
    }

    static inline bool IsDir(FIND_ATTR attr)
    {
        return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    static inline bool IsHidden(FIND_ATTR attr)
    {
        return (attr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) != 0;
    }
#endif // __WIN16__

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

private:
    FIND_DATA m_finddata;

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
    m_finddata = InitFindData();
}

wxDirData::~wxDirData()
{
    Close();
}

void wxDirData::Close()
{
    if ( IsFindDataOk(m_finddata) )
    {
        FreeFindData(m_finddata);

        m_finddata = InitFindData();
    }
}

void wxDirData::Rewind()
{
    Close();
}

bool wxDirData::Read(wxString *filename)
{
    bool first = FALSE;

#ifdef __WIN32__
    WIN32_FIND_DATA finddata;
    #define PTR_TO_FINDDATA (&finddata)
#else // Win16
    #define PTR_TO_FINDDATA (m_finddata)
#endif

    if ( !IsFindDataOk(m_finddata) )
    {
        // open first
        wxString filespec;
        filespec << m_dirname << _T('\\')
                 << (!m_filespec ? _T("*.*") : m_filespec.c_str());

        m_finddata = FindFirst(filespec, PTR_TO_FINDDATA);

        first = TRUE;
    }

    if ( !IsFindDataOk(m_finddata) )
    {
#ifdef __WIN32__
        DWORD err = ::GetLastError();

        if ( err != ERROR_FILE_NOT_FOUND )
        {
            wxLogSysError(err, _("Can not enumerate files in directory '%s'"),
                          m_dirname.c_str());
        }
#endif // __WIN32__
        //else: not an error, just no (such) files

        return FALSE;
    }

    const wxChar *name;
    FIND_ATTR attr;

    for ( ;; )
    {
        if ( first )
        {
            first = FALSE;
        }
        else
        {
            if ( !FindNext(m_finddata, PTR_TO_FINDDATA) )
            {
#ifdef __WIN32__
                DWORD err = ::GetLastError();

                if ( err != ERROR_NO_MORE_FILES )
                {
                    wxLogLastError(_T("FindNext"));
                }
#endif // __WIN32__
                //else: not an error, just no more (such) files

                return FALSE;
            }
        }

        name = GetNameFromFindData(PTR_TO_FINDDATA);
        attr = GetAttrFromFindData(PTR_TO_FINDDATA);

        // don't return "." and ".." unless asked for
        if ( name[0] == _T('.') &&
             ((name[1] == _T('.') && name[2] == _T('\0')) ||
              (name[1] == _T('\0'))) )
        {
            if ( !(m_flags & wxDIR_DOTDOT) )
                continue;
        }

        // check the type now
        if ( !(m_flags & wxDIR_FILES) && !IsDir(attr) )
        {
            // it's a file, but we don't want them
            continue;
        }
        else if ( !(m_flags & wxDIR_DIRS) && IsDir(attr) )
        {
            // it's a dir, and we don't want it
            continue;
        }

        // finally, check whether it's a hidden file
        if ( !(m_flags & wxDIR_HIDDEN) )
        {
            if ( IsHidden(attr) )
            {
                // it's a hidden file, skip it
                continue;
            }
        }

        *filename = name;

        break;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(const wxString& dir)
{
    return wxPathExists(dir);
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
    delete M_DIR;
    m_data = new wxDirData(dirname);

    return TRUE;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
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
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    M_DIR->Rewind();

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, FALSE, _T("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}
