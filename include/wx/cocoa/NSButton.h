///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSButton.h
// Purpose:     wxCocoaNSButton class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSBUTTON_H__
#define __WX_COCOA_NSBUTTON_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

WX_DECLARE_OBJC_HASHMAP(NSButton);

class wxCocoaNSButton
{
    WX_DECLARE_OBJC_INTERFACE_HASHMAP(NSButton);
public:
    void AssociateNSButton(WX_NSButton cocoaNSButton);
    inline void DisassociateNSButton(WX_NSButton cocoaNSButton)
    {
        sm_cocoaHash.erase(cocoaNSButton);
    }

public:
    virtual void Cocoa_wxNSButtonAction(void) = 0;
};

#endif // _WX_COCOA_NSBUTTON_H_
