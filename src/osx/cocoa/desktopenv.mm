/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/desktopenv.cpp
// Purpose:     wxDesktopEnv
// Author:      Igor Korot
// Modified by:
// Created:     17/11/15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#import <Foundation/NSArray.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSFileManager.h>
#import <AppKit/NSWorkspace.h>

#include "wx/filefn.h"
#include "wx/log.h"
#include "wx/osx/core/cfstring.h"
#include "wx/desktopenv.h"

#include "wx/osx/private.h"

wxDesktopEnv::wxDesktopEnv()
{
}

wxDesktopEnv::~wxDesktopEnv()
{
}

bool wxDesktopEnv::MoveToRecycleBin(wxString &path)
{
    bool ret = true;
    wxString temp = "file:///" + path;
    NSURL *url = [NSURL URLWithString:wxCFStringRef( temp ).AsNSString()];
    NSArray *files = [NSArray arrayWithObject:url];
    BOOL result = [[NSFileManager defaultManager] trashItemAtURL:url resultingItemURL:nil error:nil];
    if( result == NO )
    {
        wxLogError( _( "Failed to move file '%s' to Recycle Bin" ), fileName );
        ret = false;
    }
    return ret;
}
