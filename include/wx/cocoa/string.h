///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/string.h
// Purpose:     String conversion methods
// Author:      David Elliott
// Modified by:
// Created:     2003/04/13
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_STRING_H__
#define __WX_COCOA_STRING_H__

#import <Foundation/NSString.h>
#include "wx/string.h"

// Return an autoreleased NSString
inline NSString* wxNSStringWithWxString(const wxString &wxstring)
{
    return [NSString stringWithCString: wxstring.c_str() length:wxstring.Len()];
}

// Intialize an NSString which has already been allocated
inline NSString* wxInitNSStringWithWxString(NSString *nsstring, const wxString &wxstring)
{
    return [nsstring initWithCString: wxstring.c_str() length:wxstring.Len()];
}

#endif // __WX_COCOA_STRING_H__
