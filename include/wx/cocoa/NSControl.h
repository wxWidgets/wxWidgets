///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSControl.h
// Purpose:     wxCocoaNSControl class
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSCONTROL_H__
#define __WX_COCOA_NSCONTROL_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

WX_DECLARE_OBJC_HASHMAP(NSControl);
class wxCocoaNSControl
{
    WX_DECLARE_OBJC_INTERFACE(NSControl)
protected:
//    virtual void Cocoa_didChangeText(void) = 0;
};

#endif // _WX_COCOA_NSCONTROL_H_
