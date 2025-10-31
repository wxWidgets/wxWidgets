///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/volume.cpp
// Purpose:     apple implementation of wxFSVolume
// Author:      Tobias Taschner
// Created:     2021-11-17
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_FSVOLUME

#include "wx/volume.h"

#ifndef WX_PRECOMP
    #if wxUSE_GUI
        #include "wx/icon.h"
    #endif
#endif // WX_PRECOMP

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#import <Foundation/NSString.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSURL.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7

#define NSURLVolumeLocalizedNameKey @"NSURLVolumeLocalizedNameKey"
#define NSURLVolumeIsLocalKey @"NSURLVolumeIsLocalKey"
#define NSURLVolumeIsReadOnlyKey @"NSURLVolumeIsReadOnlyKey"
#define NSURLVolumeIsRemovableKey @"NSURLVolumeIsRemovableKey"

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// wxFSVolume
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

wxArrayString wxFSVolumeBase::GetVolumes(int flagsSet, int flagsUnset)
{
    NSArray* nativeVolumes = [[NSFileManager defaultManager]
                          mountedVolumeURLsIncludingResourceValuesForKeys:nil
                          options:NSVolumeEnumerationSkipHiddenVolumes];


    wxArrayString volumePaths;
    if ( nativeVolumes == nil )
    {
        wxFSVolumeBase volume("/");
        int flags = volume.GetFlags();
        if ((flags & flagsSet) == flagsSet && !(flags & flagsUnset))
                volumePaths.push_back(volume.GetName());
    }
    else
    {
        wxOBJC_FOR_LOOP(NSURL* url, nativeVolumes)
        {
            wxFSVolumeBase volume(url.fileSystemRepresentation);
            int flags = volume.GetFlags();
            if ((flags & flagsSet) == flagsSet && !(flags & flagsUnset))
                volumePaths.push_back(volume.GetName());
        }
        wxOBJC_END_FOR_LOOP
    }
    return volumePaths;
}

void wxFSVolumeBase::CancelSearch()
{
}

wxFSVolumeBase::wxFSVolumeBase()
{
    m_isOk = false;
}

wxFSVolumeBase::wxFSVolumeBase(const wxString& name)
{
    Create(name);
}

bool wxFSVolumeBase::Create(const wxString& name)
{
    m_isOk = false;
    m_volName = name;

    NSURL* url = [NSURL fileURLWithPath:wxCFStringRef(name).AsNSString()];
    NSDictionary* values = [url resourceValuesForKeys:[NSArray arrayWithObjects: NSURLVolumeLocalizedNameKey, nil] error:nil];
    if (values)
    {
        m_isOk = true;
        m_dispName = wxCFStringRef::AsString((CFStringRef)[values objectForKey:NSURLVolumeLocalizedNameKey]);
    }

    return m_isOk;
}

bool wxFSVolumeBase::IsOk() const
{
    return m_isOk;
}

wxFSVolumeKind wxFSVolumeBase::GetKind() const
{
    NSURL* url = [NSURL fileURLWithPath:wxCFStringRef(GetName()).AsNSString()];
    NSDictionary* values = [url resourceValuesForKeys:[NSArray arrayWithObjects: NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey, nil] error:nil];

    // Assume disk for local volumes
    if ([(NSNumber*)[values objectForKey:NSURLVolumeIsLocalKey] boolValue])
    {
        if ([(NSNumber*)[values objectForKey:NSURLVolumeIsReadOnlyKey] boolValue])
            return wxFS_VOL_CDROM;
        else
            return wxFS_VOL_DISK;
    }
    else
        return wxFS_VOL_NETWORK;
}

int wxFSVolumeBase::GetFlags() const
{
    NSURL* url = [NSURL fileURLWithPath:wxCFStringRef(GetName()).AsNSString()];
    NSDictionary* values = [url resourceValuesForKeys:[NSArray arrayWithObjects: NSURLVolumeIsRemovableKey, NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey, nil] error:nil];
    if (values)
    {
        // mounted status cannot be determined, assume mounted
        int flags = wxFS_VOL_MOUNTED;
        if ([(NSNumber*)[values objectForKey:NSURLVolumeIsRemovableKey] boolValue])
            flags |= wxFS_VOL_REMOVABLE;
        if ([(NSNumber*)[values objectForKey:NSURLVolumeIsReadOnlyKey] boolValue])
            flags |= wxFS_VOL_READONLY;
        if (![(NSNumber*)[values objectForKey:NSURLVolumeIsLocalKey] boolValue])
            flags |= wxFS_VOL_REMOTE;
        return flags;
    }
    else
        return -1;
}

#if wxUSE_GUI
wxIcon wxFSVolume::GetIcon(wxFSIconType WXUNUSED(type)) const
{
    // no support for icons on apple
    return wxNullIcon;
}
#endif // wxUSE_GUI


#endif // wxUSE_FSVOLUME
