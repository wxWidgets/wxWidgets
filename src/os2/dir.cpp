/////////////////////////////////////////////////////////////////////////////
// Name:        os2/dir.cpp
// Purpose:     wxDir implementation for OS/2
// Author:      Vadim Zeitlin
// Modified by: Stefan Neis
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

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxMatchWild

#include <sys/types.h>

#define INCL_DOSFILEMGR
#include <os2.h>

#ifdef __EMX__
#include <dirent.h>
#endif

// ----------------------------------------------------------------------------
// define the types and functions used for file searching
// ----------------------------------------------------------------------------

typedef FILEFINDBUF3                FIND_STRUCT;
typedef HDIR                        FIND_DATA;
typedef ULONG                       FIND_ATTR;

static inline FIND_DATA InitFindData() { return ERROR_INVALID_HANDLE; }

static inline bool IsFindDataOk(
  FIND_DATA                     vFd
)
{
    return vFd != ERROR_INVALID_HANDLE;
}

static inline void FreeFindData(
  FIND_DATA                     vFd
)
{
    if (!::DosFindClose(vFd))
    {
        wxLogLastError(_T("DosFindClose"));
    }
}

static inline FIND_DATA FindFirst(
  const wxString&               rsSpec
, FIND_STRUCT*                  pFinddata
)
{
    ULONG                       ulFindCount = 1;
    FIND_DATA                   hDir;
    FIND_ATTR                   rc;

    rc = ::DosFindFirst( rsSpec.c_str()
                        ,&hDir
                        ,FILE_NORMAL
                        ,pFinddata
                        ,sizeof(FILEFINDBUF3)
                        ,&ulFindCount
                        ,FIL_STANDARD
                       );
    if (rc != 0)
        return 0;
    return hDir;
}

static inline bool FindNext(
  FIND_DATA                         vFd
, FIND_STRUCT*                      pFinddata
)
{
    ULONG                           ulFindCount = 1;

    return ::DosFindNext( vFd
                         ,pFinddata
                         ,sizeof(FILEFINDBUF3)
                         ,&ulFindCount
                         ) != 0;
}

static const wxChar* GetNameFromFindData(
  FIND_STRUCT*                      pFinddata
)
{
    return pFinddata->achName;
}

static const FIND_ATTR GetAttrFromFindData(
  FIND_STRUCT*                      pFinddata
)
{
    return pFinddata->attrFile;
}

static inline bool IsDir(
  FIND_ATTR                         vAttr
)
{
    return (vAttr & FILE_DIRECTORY) != 0;
}

static inline bool IsHidden(
  FIND_ATTR                         vAttr
)
{
    return (vAttr & (FILE_HIDDEN | FILE_SYSTEM)) != 0;
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef MAX_PATH
    #define MAX_PATH 260        // from PM++ headers
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
    wxDirData(const wxString& rsDirname);
    ~wxDirData();

    void SetFileSpec(const wxString& rsFilespec) { m_sFilespec = rsFilespec; }
    void SetFlags(int nFlags) { m_nFlags = nFlags; }

    void Close();
    void Rewind();
    bool Read(wxString* rsFilename);

private:
    FIND_DATA                       m_vFinddata;
    wxString                        m_sDirname;
    wxString                        m_sFilespec;
    int                             m_nFlags;
}; // end of CLASS wxDirData

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

wxDirData::wxDirData(
  const wxString&                   rsDirname
)
: m_sDirname(rsDirname)
{
    m_vFinddata = InitFindData();
} // end of wxDirData::wxDirData

wxDirData::~wxDirData()
{
    Close();
} // end of wxDirData::~wxDirData

void wxDirData::Close()
{
    if ( IsFindDataOk(m_vFinddata) )
    {
        FreeFindData(m_vFinddata);
        m_vFinddata = InitFindData();
    }
} // end of wxDirData::Close

void wxDirData::Rewind()
{
    Close();
} // end of wxDirData::Rewind

bool wxDirData::Read(
  wxString*                         psFilename
)
{
    bool                            bFirst = FALSE;

    FILEFINDBUF3                    vFinddata;
    #define PTR_TO_FINDDATA (&vFinddata)

    if (!IsFindDataOk(m_vFinddata))
    {
        //
        // Open first
        //
        wxString                    sFilespec = m_sDirname;

        if ( !wxEndsWithPathSeparator(sFilespec) )
        {
            sFilespec += _T('\\');
        }
        sFilespec += (!m_sFilespec ? _T("*.*") : m_sFilespec.c_str());

        m_vFinddata = FindFirst( sFilespec
                                ,PTR_TO_FINDDATA
                               );
        bFirst = TRUE;
    }

    if ( !IsFindDataOk(m_vFinddata) )
    {
        return FALSE;
    }

    const wxChar*                   zName;
    FIND_ATTR                       vAttr;

    for ( ;; )
    {
        if (bFirst)
        {
            bFirst = FALSE;
        }
        else
        {
            if (!FindNext( m_vFinddata
                          ,PTR_TO_FINDDATA
                         ))
            {
                return FALSE;
            }
        }

        zName = GetNameFromFindData(PTR_TO_FINDDATA);
        vAttr = GetAttrFromFindData(PTR_TO_FINDDATA);

        //
        // Don't return "." and ".." unless asked for
        //
        if ( zName[0] == _T('.') &&
             ((zName[1] == _T('.') && zName[2] == _T('\0')) ||
              (zName[1] == _T('\0'))) )
        {
            if (!(m_nFlags & wxDIR_DOTDOT))
                continue;
        }

        //
        // Check the type now
        //
        if (!(m_nFlags & wxDIR_FILES) && !IsDir(vAttr))
        {
            //
            // It's a file, but we don't want them
            //
            continue;
        }
        else if (!(m_nFlags & wxDIR_DIRS) && IsDir(vAttr) )
        {
            //
            // It's a dir, and we don't want it
            //
            continue;
        }

        //
        // Finally, check whether it's a hidden file
        //
        if (!(m_nFlags & wxDIR_HIDDEN))
        {
            if (IsHidden(vAttr))
            {
                //
                // It's a hidden file, skip it
                //
                continue;
            }
        }
        *psFilename = zName;
        break;
    }
    return TRUE;
} // end of wxDirData::Read

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(
  const wxString&                   rsDir
)
{
    return wxPathExists(rsDir);
} // end of wxDir::Exists

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(
  const wxString&                   rsDirname
)
{
    m_data = NULL;

    (void)Open(rsDirname);
} // end of wxDir::wxDir

bool wxDir::Open(
  const wxString&                   rsDirname
)
{
    delete M_DIR;
    m_data = new wxDirData(rsDirname);
    return TRUE;
} // end of wxDir::Open

bool wxDir::IsOpened() const
{
    return m_data != NULL;
} // end of wxDir::IsOpen

wxDir::~wxDir()
{
    delete M_DIR;
} // end of wxDir::~wxDir

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(
  wxString*                         psFilename
, const wxString&                   rsFilespec
, int                               nFlags
) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );
    M_DIR->Rewind();
    M_DIR->SetFileSpec(rsFilespec);
    M_DIR->SetFlags(nFlags);
    return GetNext(psFilename);
} // end of wxDir::GetFirst

bool wxDir::GetNext(
  wxString*                         psFilename
) const
{
    wxCHECK_MSG( IsOpened(), FALSE, _T("must wxDir::Open() first") );
    wxCHECK_MSG( psFilename, FALSE, _T("bad pointer in wxDir::GetNext()") );
    return M_DIR->Read(psFilename);
} // end of wxDir::GetNext

