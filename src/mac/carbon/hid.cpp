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

#include "wx/defs.h"

//DARWIN _ONLY_
#ifdef __DARWIN__

#include "wx/mac/carbon/private/hid.h"
#include "wx/string.h"
#include "wx/log.h"

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
	wxASSERT(pIterator != NULL);

	//Now we iterate through them
	io_object_t pObject;
	while ( (pObject = IOIteratorNext(pIterator)) != NULL)
	{
		wxVERIFY(IORegistryEntryCreateCFProperties(pObject, &pDictionary,
											kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS);

		//Just for sanity :)
		wxVERIFY(CFGetTypeID(CFDictionaryGetValue(pDictionary, CFSTR(kIOHIDProductKey))) == CFStringGetTypeID());
			
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
/*
enum
{
	kHIDUsage_KeyboardHyphen	= 0x2D,	
	kHIDUsage_KeyboardEqualSign	= 0x2E,	
	kHIDUsage_KeyboardOpenBracket	= 0x2F,	
	kHIDUsage_KeyboardCloseBracket	= 0x30,
	kHIDUsage_KeyboardBackslash	= 0x31,	//* \ or | *
	kHIDUsage_KeyboardNonUSPound	= 0x32,	/* Non-US # or _ *
	kHIDUsage_KeyboardSemicolon	= 0x33,	/* ; or : *
	kHIDUsage_KeyboardQuote	= 0x34,	/* ' or " *
	kHIDUsage_KeyboardGraveAccentAndTilde	= 0x35,	/* Grave Accent and Tilde *
	kHIDUsage_KeyboardComma	= 0x36,	/* , or < *
	kHIDUsage_KeyboardPeriod	= 0x37,	/* . or > *
	kHIDUsage_KeyboardSlash	= 0x38,	/* / or ? *
	kHIDUsage_KeyboardCapsLock	= 0x39,	/* Caps Lock *

	kHIDUsage_KeyboardPrintScreen	= 0x46,	/* Print Screen *
	kHIDUsage_KeyboardScrollLock	= 0x47,	/* Scroll Lock *
	kHIDUsage_KeyboardPause	= 0x48,	/* Pause *
	kHIDUsage_KeyboardInsert	= 0x49,	/* Insert *
	kHIDUsage_KeyboardHome	= 0x4A,	/* Home *
	kHIDUsage_KeyboardDeleteForward	= 0x4C,	/* Delete Forward *

	kHIDUsage_KeyboardUpArrow
	kHIDUsage_KeypadNumLock
	kHIDUsage_KeypadSlash
	kHIDUsage_KeypadAsterisk
	kHIDUsage_KeypadHyphen
	kHIDUsage_KeypadPlus
	kHIDUsage_KeypadEnter
	kHIDUsage_KeypadPeriod
	kHIDUsage_KeyboardNonUSBackslash
	kHIDUsage_KeyboardApplication
	kHIDUsage_KeyboardPower
	kHIDUsage_KeypadEqualSign
};
/*
	enum wxKeyCode
	{

		WXK_START   = 300,
		WXK_LBUTTON,
		WXK_RBUTTON,
		WXK_CANCEL,
		WXK_MBUTTON,
		WXK_CLEAR,
		WXK_SHIFT,
		WXK_ALT,
		WXK_CONTROL,
		WXK_MENU,
		WXK_PAUSE,
		WXK_PRIOR,  *  Page up *
		WXK_NEXT,   *  Page down *
		WXK_END,
		WXK_HOME,
		WXK_LEFT,
		WXK_UP,
		WXK_RIGHT,
		WXK_DOWN,
		WXK_SELECT,
		WXK_PRINT,
		WXK_EXECUTE,
		WXK_SNAPSHOT,
		WXK_INSERT,
		WXK_HELP,
		WXK_MULTIPLY,
		WXK_ADD,
		WXK_SEPARATOR,
		WXK_SUBTRACT,
		WXK_DECIMAL,
		WXK_DIVIDE,
		WXK_PAGEUP,
		WXK_PAGEDOWN,

		WXK_NUMPAD_SPACE,
		WXK_NUMPAD_TAB,
		WXK_NUMPAD_ENTER,
		WXK_NUMPAD_HOME,
		WXK_NUMPAD_LEFT,
		WXK_NUMPAD_UP,
		WXK_NUMPAD_RIGHT,
		WXK_NUMPAD_DOWN,
		WXK_NUMPAD_PRIOR,
		WXK_NUMPAD_PAGEUP,
		WXK_NUMPAD_NEXT,
		WXK_NUMPAD_PAGEDOWN,
		WXK_NUMPAD_END,
		WXK_NUMPAD_BEGIN,
		WXK_NUMPAD_INSERT,
		WXK_NUMPAD_DELETE,
		WXK_NUMPAD_EQUAL,
		WXK_NUMPAD_MULTIPLY,
		WXK_NUMPAD_ADD,
		WXK_NUMPAD_SEPARATOR,
		WXK_NUMPAD_SUBTRACT,
		WXK_NUMPAD_DECIMAL,
		WXK_NUMPAD_DIVIDE,

		WXK_WINDOWS_LEFT,
		WXK_WINDOWS_RIGHT,
		WXK_WINDOWS_MENU ,
		WXK_COMMAND
	};

 */
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
