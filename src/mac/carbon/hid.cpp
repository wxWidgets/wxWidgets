/////////////////////////////////////////////////////////////////////////////
// Name:        hid.cpp
// Purpose:     DARWIN HID layer for WX Implementation
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "hid.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//DARWIN _ONLY_
#ifdef __DARWIN__

#include "wx/mac/carbon/private/hid.h"
#include "wx/string.h"
#include "wx/log.h"


// ---------------------------------------------------------------------------
// assertion macros
// ---------------------------------------------------------------------------

#define wxFORCECHECK_MSG(arg, msg)  \
{\
	if (arg) \
	{\
		wxLogSysError(wxString::Format(wxT("Message:%s\nHID: %s failed!"), wxT(msg), wxT(#arg)));\
		return false;\
	}\
}
#define wxIOCHECK(arg, msg)  wxFORCECHECK_MSG(arg != kIOReturnSuccess, msg)
#define wxKERNCHECK(arg, msg) wxFORCECHECK_MSG(arg != KERN_SUCCESS, msg)
#define wxSCHECK(arg, msg) wxFORCECHECK_MSG(arg != S_OK, msg)

#ifdef __WXDEBUG___
#	define wxVERIFY(arg)	wxASSERT(arg)
#else
#	define wxVERIFY(arg)	arg
#endif

/*
void CFShowTypeIDDescription(CFTypeRef pData)
{
	if(!pData)
	{
		wxASSERT(false);
		return;
	}
	
	wxMessageBox(
		CFStringGetCStringPtr(
			CFCopyTypeIDDescription(CFGetTypeID(pData)),CFStringGetSystemEncoding()
							 )
				);	
}
*/

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxHIDDevice
// ---------------------------------------------------------------------------

bool wxHIDDevice::Create (const int& nClass, const int& nType)
{
	//Create the mach port
	wxIOCHECK(IOMasterPort(bootstrap_port, &m_pPort), "Could not create mach port");

	//Dictionary that will hold first
	//the matching dictionary for determining which kind of devices we want,
	//then later some registry properties from an iterator (see below)
	CFMutableDictionaryRef pDictionary;

	//Create a dictionary
	//The call to IOServiceMatching filters down the
	//the services we want to hid services (and also eats the
	//dictionary up for us (consumes one reference))
	wxVERIFY((pDictionary = IOServiceMatching(kIOHIDDeviceKey)) != NULL );

	//Here we'll filter down the services to what we want
	if (nType != -1)
	{
		CFNumberRef pType = CFNumberCreate(kCFAllocatorDefault,
									kCFNumberIntType, &nType);
		CFDictionarySetValue(pDictionary, CFSTR(kIOHIDPrimaryUsageKey), pType);
		CFRelease(pType);
	}
	if (nClass != -1)
	{
		CFNumberRef pClass = CFNumberCreate(kCFAllocatorDefault,
									kCFNumberIntType, &nClass);
		CFDictionarySetValue(pDictionary, CFSTR(kIOHIDPrimaryUsagePageKey), pClass);
		CFRelease(pClass);
	}

	//Now get the maching services
	io_iterator_t pIterator;
	wxIOCHECK(IOServiceGetMatchingServices(m_pPort, pDictionary, &pIterator), "No Matching HID Services");
	wxASSERT(pIterator != 0);

	//Now we iterate through them
	io_object_t pObject;
	while ( (pObject = IOIteratorNext(pIterator)) != 0)
	{
		wxVERIFY(IORegistryEntryCreateCFProperties(pObject, &pDictionary,
											kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS);

		//Just for sanity :)
		wxASSERT(CFGetTypeID(CFDictionaryGetValue(pDictionary, CFSTR(kIOHIDProductKey))) == CFStringGetTypeID());
			
		//Get [product] name
		m_szName = CFStringGetCStringPtr	(
						(CFStringRef) CFDictionaryGetValue(pDictionary, CFSTR(kIOHIDProductKey)), 
						CFStringGetSystemEncoding()
										);

		//
		//Now the hard part - in order to scan things we need "cookies" -
		//
		wxCFArray CookieArray = CFDictionaryGetValue(pDictionary, CFSTR(kIOHIDElementKey));
		BuildCookies(CookieArray);
		if (m_ppQueue != NULL)
			wxVERIFY((*m_ppQueue)->start(m_ppQueue) == S_OK);

		//Create the interface (good grief - long function names!)
		SInt32 nScore;
		IOCFPlugInInterface** ppPlugin;
		wxIOCHECK(IOCreatePlugInInterfaceForService(pObject, kIOHIDDeviceUserClientTypeID,
											kIOCFPlugInInterfaceID, &ppPlugin, &nScore), "");
											   
		//Now, the final thing we can check before we fall back to asserts
		//(because the dtor only checks if the device is ok, so if anything
		//fails from now on the dtor will delete the device anyway, so we can't break from this).
			
		//Get the HID interface from the plugin to the mach port
		wxSCHECK((*ppPlugin)->QueryInterface(ppPlugin,
							CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), (void**) &m_ppDevice), "");
							
		//release the plugin
		(*ppPlugin)->Release(ppPlugin);
		
		//open the HID interface...
		wxVERIFY((*m_ppDevice)->open(m_ppDevice, 0) == S_OK);
		
		//cleanup
		CFRelease(pDictionary);
		IOObjectRelease(pObject);
		break;
	}
	//iterator cleanup
	IOObjectRelease(pIterator);
		
	return true;
}//end Create()
	
void wxHIDDevice::AddCookie(CFTypeRef Data, const int& i)
{
	CFNumberGetValue(
				(CFNumberRef) CFDictionaryGetValue	( (CFDictionaryRef) Data
										, CFSTR(kIOHIDElementCookieKey)
										),	
				kCFNumberIntType,
				&m_pCookies[i]
				);
}

void wxHIDDevice::AddCookieInQueue(CFTypeRef Data, const int& i)
{
	AddCookie(Data, i);
	wxVERIFY((*m_ppQueue)->addElement(m_ppQueue, m_pCookies[i], 0) == S_OK);//3rd Param flags (none yet)
}
	
void wxHIDDevice::InitCookies(const size_t& dwSize, bool bQueue)
{
	m_pCookies = new IOHIDElementCookie[dwSize];
	if (bQueue)
	{
		wxASSERT( m_ppQueue != NULL);
		wxVERIFY(  (m_ppQueue = (*m_ppDevice)->allocQueue(m_ppDevice)) != NULL);
		wxVERIFY(  (*m_ppQueue)->create(m_ppQueue, 0, 512) == S_OK); //Param 2, flags, none yet
	}		
}

bool wxHIDDevice::IsActive(const int& nIndex)
{
	wxASSERT(m_pCookies[nIndex] != NULL);
	IOHIDEventStruct Event;
	(*m_ppDevice)->getElementValue(m_ppDevice, m_pCookies[nIndex], &Event);
	return !!Event.value;
}
	

wxHIDDevice::~wxHIDDevice()
{
	if (m_ppDevice != NULL)
	{
		(*m_ppDevice)->close(m_ppDevice);
		(*m_ppDevice)->Release(m_ppDevice);
		mach_port_deallocate(mach_task_self(), m_pPort);
	}
	
	if (m_pCookies != NULL)
	{
		delete [] m_pCookies;
		if (m_ppQueue != NULL)
		{
			(*m_ppQueue)->stop(m_ppQueue);
			(*m_ppQueue)->dispose(m_ppQueue);
			(*m_ppQueue)->Release(m_ppQueue);
		}
	}
}

// ---------------------------------------------------------------------------
// wxHIDKeyboard
// ---------------------------------------------------------------------------

enum
{
	WXK_RSHIFT = 400,
	WXK_RALT,
	WXK_RCONTROL,
	WXK_RMENU
	
};

bool wxHIDKeyboard::Create()
{
	return wxHIDDevice::Create(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
}

void wxHIDKeyboard::BuildCookies(wxCFArray& Array)
{
	Array = CFDictionaryGetValue((CFDictionaryRef)Array[0], CFSTR(kIOHIDElementKey));
	InitCookies(500);
	int i,
		nUsage;
	for (i = 0; i < Array.Count(); ++i)
	{
		CFNumberGetValue(
			(CFNumberRef) CFDictionaryGetValue((CFDictionaryRef) Array[i], CFSTR(kIOHIDElementUsageKey)), 
				kCFNumberLongType, &nUsage);
			
		if (nUsage >= kHIDUsage_KeyboardA && nUsage <= kHIDUsage_KeyboardZ)
			AddCookie(Array[i], 'A' + (nUsage - kHIDUsage_KeyboardA) );
		else if (nUsage >= kHIDUsage_Keyboard1 && nUsage <= kHIDUsage_Keyboard9)
			AddCookie(Array[i], '1' + (nUsage - kHIDUsage_Keyboard1) );
		else if (nUsage >= kHIDUsage_KeyboardF1 && nUsage <= kHIDUsage_KeyboardF12)
			AddCookie(Array[i], WXK_F1 + (nUsage - kHIDUsage_KeyboardF1) );
		else if (nUsage >= kHIDUsage_KeyboardF13 && nUsage <= kHIDUsage_KeyboardF24)
			AddCookie(Array[i], WXK_F13 + (nUsage - kHIDUsage_KeyboardF13) );
		else if (nUsage >= kHIDUsage_Keypad1 && nUsage <= kHIDUsage_Keypad9)
			AddCookie(Array[i], WXK_NUMPAD1 + (nUsage - kHIDUsage_Keypad1) );
		else switch (nUsage)
		{
			//0's (wx & ascii go 0-9, but HID goes 1-0)
			case kHIDUsage_Keyboard0:
				AddCookie(Array[i],'0');
				break;
			case kHIDUsage_Keypad0:
				AddCookie(Array[i],WXK_NUMPAD0);
				break;
				
			//Basic
			case kHIDUsage_KeyboardReturnOrEnter:
				AddCookie(Array[i], WXK_RETURN);
				break;
			case kHIDUsage_KeyboardEscape:
				AddCookie(Array[i], WXK_ESCAPE);
				break;
			case kHIDUsage_KeyboardDeleteOrBackspace:
				AddCookie(Array[i], WXK_BACK);
				break;
			case kHIDUsage_KeyboardTab:
				AddCookie(Array[i], WXK_TAB);
				break;
			case kHIDUsage_KeyboardSpacebar:
				AddCookie(Array[i], WXK_SPACE);
				break;
			case kHIDUsage_KeyboardPageUp:
				AddCookie(Array[i], WXK_PRIOR);
				break;
			case kHIDUsage_KeyboardEnd:
				AddCookie(Array[i], WXK_END);
				break;
			case kHIDUsage_KeyboardPageDown:
				AddCookie(Array[i], WXK_NEXT);
				break;
			case kHIDUsage_KeyboardRightArrow:
				AddCookie(Array[i], WXK_RIGHT);
				break;
			case kHIDUsage_KeyboardLeftArrow:
				AddCookie(Array[i], WXK_LEFT);
				break;
			case kHIDUsage_KeyboardDownArrow:
				AddCookie(Array[i], WXK_DOWN);
				break;
			case kHIDUsage_KeyboardUpArrow:
				AddCookie(Array[i], WXK_UP);
				break;
						
			//LEDS
			case kHIDUsage_KeyboardCapsLock:
				AddCookie(Array[i],WXK_CAPITAL);
				break;
			case kHIDUsage_KeypadNumLock:
				AddCookie(Array[i],WXK_NUMLOCK);
				break;
			case kHIDUsage_KeyboardScrollLock:
				AddCookie(Array[i],WXK_SCROLL);
				break;
					
			//Menu keys, Shift, other specials
			case kHIDUsage_KeyboardLeftControl:
				AddCookie(Array[i],WXK_CONTROL);
				break;
			case kHIDUsage_KeyboardLeftShift:
				AddCookie(Array[i],WXK_SHIFT);
				break;
			case kHIDUsage_KeyboardLeftAlt:
				AddCookie(Array[i],WXK_ALT);
				break;
			case kHIDUsage_KeyboardLeftGUI:
				AddCookie(Array[i],WXK_MENU);
				break;
			case kHIDUsage_KeyboardRightControl:
				AddCookie(Array[i],WXK_RCONTROL);
				break;
			case kHIDUsage_KeyboardRightShift:
				AddCookie(Array[i],WXK_RSHIFT);
				break;
			case kHIDUsage_KeyboardRightAlt:
				AddCookie(Array[i],WXK_RALT);
				break;
			case kHIDUsage_KeyboardRightGUI:
				AddCookie(Array[i],WXK_RMENU);
				break;
 						
			//Default	
			default:
			//not in wx keycodes - do nothing....
			break;
		}
	}
}//end buildcookies

#endif //__DARWIN__
