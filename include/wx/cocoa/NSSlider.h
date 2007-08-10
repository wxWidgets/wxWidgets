/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSSlider.h
// Purpose:     wxCocoaNSSlider class
// Author:      Mark Oxenham
// Modified by:
// Created:     2007/08/10
// RCS-ID:      $Id: $
// Copyright:   (c) 2007 Software 2000 Ltd. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXNSSLIDER_H_
#define _WXNSSLIDER_H_

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"
#include "wx/cocoa/ObjcRef.h"

DECLARE_WXCOCOA_OBJC_CLASS(NSSlider);

WX_DECLARE_OBJC_HASHMAP(NSSlider);

class wxCocoaNSSlider
{
    WX_DECLARE_OBJC_INTERFACE_HASHMAP(NSSlider);
public:
    void AssociateNSSlider(WX_NSSlider cocoaNSSlider);
    void DisassociateNSSlider(WX_NSSlider cocoaNSSlider);

    virtual void Cocoa_wxNSSliderUpArrowKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderDownArrowKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderLeftArrowKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderRightArrowKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderPageUpKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderPageDownKeyDown(void) = 0;
    virtual void Cocoa_wxNSSliderMoveUp(void) = 0;
    virtual void Cocoa_wxNSSliderMoveDown(void) = 0;
    virtual void Cocoa_wxNSSliderMoveLeft(void) = 0;
    virtual void Cocoa_wxNSSliderMoveRight(void) = 0;
    virtual void Cocoa_wxNSSliderPageUp(void) = 0;
    virtual void Cocoa_wxNSSliderPageDown(void) = 0;
    virtual void CocoaNotification_startTracking(WX_NSNotification notification) = 0;
    virtual void CocoaNotification_continueTracking(WX_NSNotification notification) = 0;
    virtual void CocoaNotification_stopTracking(WX_NSNotification notification) = 0;
    virtual ~wxCocoaNSSlider() { }

protected:
    static const wxObjcAutoRefFromAlloc<struct objc_object*> sm_cocoaTarget;
    static struct objc_object *sm_cocoaObserver;

};

#endif
