/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/ObjcPose.h
// Purpose:     Macros for initializing poseAs, among other things
// Author:      David Elliott
// Modified by: 
// Created:     2002/12/03
// RCS-ID:      $Id:
// Copyright:   (c) 2002 David Elliott <dfe@cox.net>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_PRIVATE_POSER_H__
#define __WX_COCOA_PRIVATE_POSER_H__

/*-------------------------------------------------------------------------
Objective-C Poser class initialization
-------------------------------------------------------------------------*/
#ifdef __OBJC__
#import <objc/objc-class.h>

class wxPoseAsInitializer
{
public:
	wxPoseAsInitializer(Class poser)
	: m_poser(poser)
	, m_next(sm_first)
	{
		sm_first = this;
	}
	~wxPoseAsInitializer()
	{
		class_poseAs(m_poser,m_poser->super_class);
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
	Class m_poser;
	wxPoseAsInitializer *m_next;
	static wxPoseAsInitializer *sm_first;
};

class wxDummyForPoseAsInitializer
{
public:
	wxDummyForPoseAsInitializer(void*) {}
};

#define WX_IMPLEMENT_POSER(poser) \
wxDummyForPoseAsInitializer wxDummyPoseAsInitializerFor##poser(new wxPoseAsInitializer([poser class]))

#else // __OBJC__
#warning "Objective-C++ Only!"
#endif // __OBJC__

#endif // __WX_COCOA_PRIVATE_POSER_H__

