///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/volume.cpp
// Purpose:     wxFSVolume - encapsulates system volume information
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "fsvolume.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FSVOLUME

#ifndef WX_PRECOMP
    #if wxUSE_GUI
        #include "wx/icon.h"
    #endif
    #include "wx/intl.h"
#endif // WX_PRECOMP

#include "wx/dir.h"
#include "wx/hashmap.h"
#include "wx/dynlib.h"
#include "wx/arrimpl.cpp"

#include "wx/volume.h"

#include "wx/palmos/missing.h"

#if wxUSE_BASE

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Dynamic library function defs.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static wxDynamicLibrary s_mprLib;

typedef DWORD (WINAPI* WNetOpenEnumPtr)(DWORD, DWORD, DWORD, LPNETRESOURCE, LPHANDLE);
typedef DWORD (WINAPI* WNetEnumResourcePtr)(HANDLE, LPDWORD, LPVOID, LPDWORD);
typedef DWORD (WINAPI* WNetCloseEnumPtr)(HANDLE);

static WNetOpenEnumPtr s_pWNetOpenEnum;
static WNetEnumResourcePtr s_pWNetEnumResource;
static WNetCloseEnumPtr s_pWNetCloseEnum;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Globals/Statics
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static long s_cancelSearch = FALSE;

struct FileInfo
{
    FileInfo(unsigned flag=0, wxFSVolumeKind type=wxFS_VOL_OTHER) :
        m_flags(flag), m_type(type) {}

    FileInfo(const FileInfo& other) { *this = other; }
    FileInfo& operator=(const FileInfo& other)
    {
        m_flags = other.m_flags;
        m_type = other.m_type;
        return *this;
    }

    unsigned m_flags;
    wxFSVolumeKind m_type;
};
WX_DECLARE_STRING_HASH_MAP(FileInfo, FileInfoMap);
// Cygwin bug (?) destructor for global s_fileInfo is called twice...
static FileInfoMap& GetFileInfoMap()
{
    static FileInfoMap s_fileInfo(25);

    return s_fileInfo;
}
#define s_fileInfo (GetFileInfoMap())

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Local helper functions.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//=============================================================================
// Function: GetBasicFlags
// Purpose: Set basic flags, primarily wxFS_VOL_REMOTE and wxFS_VOL_REMOVABLE.
// Notes: - Local and mapped drives are mounted by definition.  We have no
//          way to determine mounted status of network drives, so assume that
//          all drives are mounted, and let the caller decide otherwise.
//        - Other flags are 'best guess' from type of drive.  The system will
//          not report the file attributes with any degree of accuracy.
//=============================================================================
static unsigned GetBasicFlags(const wxChar* filename)
{
    unsigned flags = wxFS_VOL_MOUNTED;

    return flags;
} // GetBasicFlags

//=============================================================================
// Function: FilteredAdd
// Purpose: Add a file to the list if it meets the filter requirement.
// Notes: - See GetBasicFlags for remarks about the Mounted flag.
//=============================================================================
static bool FilteredAdd(wxArrayString& list, const wxChar* filename, 
                        unsigned flagsSet, unsigned flagsUnset)
{
    return false;
} // FilteredAdd

//=============================================================================
// Function: BuildListFromNN
// Purpose: Append or remove items from the list
// Notes: - There is no way to find all disconnected NN items, or even to find
//          all items while determining which are connected and not.  So this
//          function will find either all items or connected items.
//=============================================================================
static void BuildListFromNN(wxArrayString& list, NETRESOURCE* pResSrc, 
                            unsigned flagsSet, unsigned flagsUnset)
{
} // BuildListFromNN

//=============================================================================
// Function: CompareFcn
// Purpose: Used to sort the NN list alphabetically, case insensitive.
//=============================================================================
static int CompareFcn(wxString* first, wxString* second)
{
    return wxStricmp(first->c_str(), second->c_str());
} // CompareFcn

//=============================================================================
// Function: BuildRemoteList
// Purpose: Append Network Neighborhood items to the list.
// Notes: - Mounted gets transalated into Connected.  FilteredAdd is told
//          to ignore the Mounted flag since we need to handle it in a weird
//          way manually.
//        - The resulting list is sorted alphabetically.
//=============================================================================
static bool BuildRemoteList(wxArrayString& list, NETRESOURCE* pResSrc, 
                            unsigned flagsSet, unsigned flagsUnset)
{
    return false;
} // BuildRemoteList

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// wxFSVolume
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//=============================================================================
// Function: GetVolumes
// Purpose: Generate and return a list of all volumes (drives) available.
// Notes:
//=============================================================================
wxArrayString wxFSVolumeBase::GetVolumes(int flagsSet, int flagsUnset)
{
    wxArrayString list;

    return list;
} // GetVolumes

//=============================================================================
// Function: CancelSearch
// Purpose: Instruct an active search to stop.
// Notes: - This will only sensibly be called by a thread other than the one
//          performing the search.  This is the only thread-safe function
//          provided by the class.
//=============================================================================
void wxFSVolumeBase::CancelSearch()
{
} // CancelSearch

//=============================================================================
// Function: constructor
// Purpose: default constructor
//=============================================================================
wxFSVolumeBase::wxFSVolumeBase()
{
} // wxVolume

//=============================================================================
// Function: constructor
// Purpose: constructor that calls Create
//=============================================================================
wxFSVolumeBase::wxFSVolumeBase(const wxString& name)
{
} // wxVolume

//=============================================================================
// Function: Create
// Purpose: Finds, logs in, etc. to the request volume.
//=============================================================================
bool wxFSVolumeBase::Create(const wxString& name)
{
    return false;
} // Create

//=============================================================================
// Function: IsOk
// Purpose: returns TRUE if the volume is legal.
// Notes: For fixed disks, it must exist.  For removable disks, it must also
//        be present.  For Network Shares, it must also be logged in, etc.
//=============================================================================
bool wxFSVolumeBase::IsOk() const
{
    return false;
} // IsOk

//=============================================================================
// Function: GetKind
// Purpose: Return the type of the volume.
//=============================================================================
wxFSVolumeKind wxFSVolumeBase::GetKind() const
{
    return wxFS_VOL_OTHER;
}

//=============================================================================
// Function: GetFlags
// Purpose: Return the caches flags for this volume.
// Notes: - Returns -1 if no flags were cached.
//=============================================================================
int wxFSVolumeBase::GetFlags() const
{
    return -1;
} // GetFlags

#endif // wxUSE_BASE

// ============================================================================
// wxFSVolume
// ============================================================================

#if wxUSE_GUI

void wxFSVolume::InitIcons()
{
}

//=============================================================================
// Function: GetIcon
// Purpose: return the requested icon.
//=============================================================================

wxIcon wxFSVolume::GetIcon(wxFSIconType type) const
{
    return m_icons[type];
} // GetIcon

#endif // wxUSE_GUI

#endif // wxUSE_FSVOLUME

