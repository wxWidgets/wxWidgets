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

// gcc does not yet support modern Apple ObjC++ syntax.
#ifndef __clang__
#define USE_LEGACY_OBJC
#endif

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

// Fallbacks for legacy systems
#ifndef NSURLVolumeLocalizedNameKey
#define NSURLVolumeLocalizedNameKey @"NSURLVolumeLocalizedNameKey"
#endif
#ifndef NSURLVolumeIsLocalKey
#define NSURLVolumeIsLocalKey @"NSURLVolumeIsLocalKey"
#endif
#ifndef NSURLVolumeIsReadOnlyKey
#define NSURLVolumeIsReadOnlyKey @"NSURLVolumeIsReadOnlyKey"
#endif
#ifndef NSURLVolumeIsRemovableKey
#define NSURLVolumeIsRemovableKey @"NSURLVolumeIsRemovableKey"
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// wxFSVolume
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

wxArrayString wxFSVolumeBase::GetVolumes(int flagsSet, int flagsUnset)
{
#ifdef USE_LEGACY_OBJC
    NSArray* nativeVolumes = [[NSFileManager defaultManager]
#else
    auto nativeVolumes = [[NSFileManager defaultManager]
#endif
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
#ifdef USE_LEGACY_OBJC
        for (NSUInteger i = 0; i < [nativeVolumes count]; ++i)
        {
            NSURL* url = [nativeVolumes objectAtIndex:i];
            const char* path = [[url path] fileSystemRepresentation];
            wxFSVolumeBase volume(path);
#else
        for (NSURL* url in nativeVolumes)
        {
            wxFSVolumeBase volume(url.fileSystemRepresentation);
#endif
            int flags = volume.GetFlags();
            if ((flags & flagsSet) == flagsSet && !(flags & flagsUnset))
                volumePaths.push_back(volume.GetName());
        }
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
#ifdef USE_LEGACY_OBJC
    NSArray* keys = [NSArray arrayWithObject:NSURLVolumeLocalizedNameKey];
    NSDictionary* values = [url resourceValuesForKeys:keys error:nil];
#else
    auto values = [url resourceValuesForKeys:@[NSURLVolumeLocalizedNameKey] error:nil];
#endif
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
#ifdef USE_LEGACY_OBJC
    NSArray* keys = [NSArray arrayWithObjects:NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey, nil];
    NSDictionary* values = [url resourceValuesForKeys:keys error:nil];
#else
    auto values = [url resourceValuesForKeys:@[NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey] error:nil];
#endif
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
#ifdef USE_LEGACY_OBJC
    NSArray* keys = [NSArray arrayWithObjects:NSURLVolumeIsRemovableKey, NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey, nil];
    NSDictionary* values = [url resourceValuesForKeys:keys error:nil];
#else
    auto values = [url resourceValuesForKeys:@[NSURLVolumeIsRemovableKey, NSURLVolumeIsLocalKey, NSURLVolumeIsReadOnlyKey] error:nil];
#endif
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
void wxFSVolume::InitIcons()
{
    // no support for icons on apple
}

wxIcon wxFSVolume::GetIcon(wxFSIconType WXUNUSED(type)) const
{
    // no support for icons on apple
    return wxNullIcon;
}
#endif // wxUSE_GUI


#endif // wxUSE_FSVOLUME
