/////////////////////////////////////////////////////////////////////////////
// Name:        hid.h
// Purpose:     DARWIN HID layer for WX
// Author:      Ryan Norton
// Modified by:
// Created:     11/11/2003
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifndef _WX_MACCARBONHID_H_
#define _WX_MACCARBONHID_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "hid.h"
#endif

#include "wx/defs.h"


// ---------------------------------------------------------------------------
// definitions
// ---------------------------------------------------------------------------

//Mac OSX only
#ifdef __DARWIN__

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

#include <mach/mach.h>

//Utility wrapper around CFArray
class wxCFArray
{
public:
	wxCFArray(CFTypeRef pData) : pArray((CFArrayRef) pData) {}
	CFTypeRef operator [] (const int& nIndex) {return CFArrayGetValueAtIndex(pArray, nIndex); }
	int Count() {return CFArrayGetCount(pArray);}
private:
	CFArrayRef pArray;
};

//
//	A wrapper around OS X HID Manager procedures.
//	The tutorial "Working With HID Class Device Interfaces" Is
//	Quite good, as is the sample program associated with it 
//  (Depite the author's protests!).
class wxHIDDevice
{
public:
	wxHIDDevice() : m_ppDevice(NULL), m_ppQueue(NULL), m_pCookies(NULL) {}
	//kHIDPage_GenericDesktop
	//kHIDUsage_GD_Joystick,kHIDUsage_GD_Mouse,kHIDUsage_GD_Keyboard
	bool Create (const int& nClass = -1, const int& nType = -1);

	inline void AddCookie(CFTypeRef Data, const int& i);
	inline void AddCookieInQueue(CFTypeRef Data, const int& i);	
	inline void InitCookies(const size_t& dwSize, bool bQueue = false);

	//Must be implemented by derived classes
	//builds the cookie array -
	//first call InitCookies to initialize the cookie
	//array, then AddCookie to add a cookie at a certain point in an array
	virtual void BuildCookies(wxCFArray& Array) = 0;
		
	//checks to see whether the cookie at index nIndex is active (element value != 0)
	bool IsActive(const int& nIndex);
	
	//closes the device and cleans the queue and cookies
	virtual ~wxHIDDevice();
private:
	IOHIDDeviceInterface** 	m_ppDevice; //this, essentially
	IOHIDQueueInterface**	m_ppQueue;  //queue (if we want one)
	IOHIDElementCookie* 	m_pCookies; //cookies

	const char* 			m_szName; //(product) name
	mach_port_t 			m_pPort;
};

class wxHIDKeyboard : public wxHIDDevice
{
public:
	bool Create();
	virtual void BuildCookies(wxCFArray& Array);
};

#endif //__DARWIN__

#endif 
        //WX_MACCARBONHID_H