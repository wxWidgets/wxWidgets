/////////////////////////////////////////////////////////////////////////////
// Name:        wx/volume.h
// Purpose:     wxFSVolume - encapsulates system volume information
// Author:      George Policello
// Modified by:
// Created:     28 Jan 02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 George Policello
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// wxFSVolume represents a volume/drive/mount point in a file system
// ----------------------------------------------------------------------------

#ifndef _WX_FSVOLUME_H_
#define _WX_FSVOLUME_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "fsvolume.h"
#endif

#include "wx/defs.h"

#if wxUSE_FSVOLUME

#if wxUSE_GUI
    #include "wx/iconbndl.h" // for wxIconArray
#endif // wxUSE_GUI

// the volume flags
enum
{
    // is the volume mounted?
    wxFS_VOL_MOUNTED = 0x0001,

    // is the volume removable (floppy, CD, ...)?
    wxFS_VOL_REMOVABLE = 0x0002,

    // read only? (otherwise read write)
    wxFS_VOL_READONLY = 0x0004,

    // network resources
    wxFS_VOL_REMOTE = 0x0008
};

// the volume types
enum wxFSVolumeKind
{
    wxFS_VOL_FLOPPY,
    wxFS_VOL_DISK,
    wxFS_VOL_CDROM,
    wxFS_VOL_DVDROM,
    wxFS_VOL_NETWORK,
    wxFS_VOL_OTHER,
    wxFS_VOL_MAX
};

#if wxUSE_GUI

#include "wx/icon.h"

enum wxFSIconType
{
    wxFS_VOL_ICO_SMALL = 0,
    wxFS_VOL_ICO_LARGE,
    wxFS_VOL_ICO_SEL_SMALL,
    wxFS_VOL_ICO_SEL_LARGE,
    wxFS_VOL_ICO_MAX
};

#endif // wxUSE_GUI

class WXDLLEXPORT wxFSVolume
{
public:
    // return the array containing the names of the volumes
    //
    // only the volumes with the flags such that
    //  (flags & flagsSet) == flagsSet && !(flags & flagsUnset)
    // are returned (by default, all mounted ones)
    static wxArrayString GetVolumes(int flagsSet = wxFS_VOL_MOUNTED,
                                    int flagsUnset = 0);

    // stop execution of GetVolumes() called previously (should be called from
    // another thread, of course)
    static void CancelSearch();

    // create the volume object with this name (should be one of those returned
    // by GetVolumes()).
    wxFSVolume();
    wxFSVolume(const wxString& name);
    bool Create(const wxString& name);

    // accessors
    // ---------

    // is this a valid volume?
    bool IsOk() const;

    // kind of this volume?
    wxFSVolumeKind GetKind() const;

    // flags of this volume?
    int GetFlags() const;

    // can we write to this volume?
    bool IsWritable() const { return !(GetFlags() & wxFS_VOL_READONLY); }

    // get the name of the volume and the name which should be displayed to the
    // user
    wxString GetName() const { return m_volName; }
    wxString GetDisplayName() const { return m_dispName; }

#if wxUSE_GUI
    wxIcon GetIcon(wxFSIconType type) const;
#endif

    // TODO: operatios (Mount(), Unmount(), Eject(), ...)?

private:
    wxString m_volName;
    wxString m_dispName;
#if wxUSE_GUI
    wxIconArray m_icons;
#endif
    bool m_isOk;

};

#endif // wxUSE_FSVOLUME

#endif // _WX_FSVOLUME_H_

