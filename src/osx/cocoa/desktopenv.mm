///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#import <Foundation/NSArray.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSFileManager.h>
#import <AppKit/NSWorkspace.h>

#include "wx/filefn.h"
#include "wx/log.h"
#include "wx/osx/core/cfstring.h"
#include "wx/desktopenv.h"

#include "wx/osx/private.h"

/* static */
bool wxDesktopEnv::MoveToRecycleBin(const wxString &path)
{
    bool result = false;
    if( wxDesktopEnvBase::MoveToRecycleBin( path ) )
    {
        wxString temp = "file:///" + path;
        NSURL *url = [NSURL URLWithString:wxCFStringRef( temp ).AsNSString()];
        NSArray *files = [NSArray arrayWithObject:url];
        BOOL result = [[NSFileManager defaultManager] trashItemAtURL:url resultingItemURL:nil error:nil];
        if( result == NO )
        {
            wxLogSysError( _( "Failed to move '%s' to Trash" ), path );
            result = true;
        }
    }
    return result;
}
