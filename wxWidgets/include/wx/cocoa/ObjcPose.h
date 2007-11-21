/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/ObjcPose.h
// Purpose:     Macros for initializing poseAs, among other things
// Author:      David Elliott
// Modified by:
// Created:     2002/12/03
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott <dfe@cox.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_PRIVATE_POSER_H__
#define __WX_COCOA_PRIVATE_POSER_H__

/*-------------------------------------------------------------------------
Objective-C Poser class initialization
-------------------------------------------------------------------------*/
#ifdef __OBJC__
#import <objc/objc-class.h>
#import <Foundation/NSObjCRuntime.h>

class wxPoseAsInitializer
{
public:
    wxPoseAsInitializer()
    : m_next(sm_first)
    {
        sm_first = this;
    }
    virtual ~wxPoseAsInitializer()
    {
        sm_first = m_next;
    }
    static void InitializePosers()
    {
        while(sm_first)
        {
            delete sm_first;
        }
    };
protected:
    wxPoseAsInitializer *m_next;
    static wxPoseAsInitializer *sm_first;
};

class wxDummyForPoseAsInitializer
{
public:
    wxDummyForPoseAsInitializer(void*) {}
};

#define WX_IMPLEMENT_POSER(poser) \
class wxPoseAsInitializerFor##poser: public wxPoseAsInitializer \
{ \
protected: \
    virtual ~wxPoseAsInitializerFor##poser() \
    { \
        class_poseAs([poser class],[poser superclass]); \
    } \
}; \
wxDummyForPoseAsInitializer wxDummyPoseAsInitializerFor##poser(new wxPoseAsInitializerFor##poser)

#else // __OBJC__
#warning "Objective-C++ Only!"
#endif // __OBJC__

#endif // __WX_COCOA_PRIVATE_POSER_H__
