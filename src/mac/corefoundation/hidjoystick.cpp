/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.cpp
// Purpose:     wxJoystick class
// Author:      Ryan Norton
// Modified by:
// Created:     2/13/2005
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "joystick.h"
#endif

#include "wx/wxprec.h"

#ifdef __DARWIN__

#if wxUSE_JOYSTICK

#include "wx/event.h"
#include "wx/log.h"
#include "wx/joystick.h"
#include "wx/thread.h"
#include "wx/window.h"

#include "wx/mac/corefoundation/hid.h"

#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

enum {
    wxJS_AXIS_X = 41,
    wxJS_AXIS_Y,
    wxJS_AXIS_Z,
    wxJS_AXIS_RUDDER,
    wxJS_AXIS_U,
    wxJS_AXIS_V,

    wxJS_AXIS_MAX = 32767,
    wxJS_AXIS_MIN = -32767
};

class wxHIDJoystick : public wxHIDDevice
{
public:
	bool Create(int nWhich);
	virtual void BuildCookies(wxCFArray& Array);
    IOHIDElementCookie* GetCookies() {return m_pCookies;}
    
    IOHIDQueueInterface** GetQueue() {return m_ppQueue;}
};


bool wxHIDJoystick::Create(int nWhich)
{
    int nJoysticks = GetCount(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
    
    if (nWhich <= nJoysticks)
        return wxHIDDevice::Create(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
    else
        nWhich -= nJoysticks;
    
    int nGamePads = GetCount(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
    
    if (nWhich <= nGamePads)
        return wxHIDDevice::Create(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
    else
        return false;
}

void wxHIDJoystick::BuildCookies(wxCFArray& Array)
{
	Array = CFDictionaryGetValue((CFDictionaryRef)Array[0], CFSTR(kIOHIDElementKey));
	InitCookies(50, true);
	int i,
		nUsage,
        nPage;
	for (i = 0; i < Array.Count(); ++i)
	{
		CFNumberGetValue(
			(CFNumberRef) CFDictionaryGetValue((CFDictionaryRef) Array[i], CFSTR(kIOHIDElementUsageKey)), 
				kCFNumberLongType, &nUsage);
			
		CFNumberGetValue(
			(CFNumberRef) CFDictionaryGetValue((CFDictionaryRef) Array[i], CFSTR(kIOHIDElementUsagePageKey)), 
				kCFNumberLongType, &nPage);

		if (nPage == kHIDPage_Button && nUsage <= 40)
			AddCookieInQueue(Array[i], nUsage );
		else if (nPage == kHIDPage_GenericDesktop)
        {
            switch(nUsage)
            {
                case kHIDUsage_GD_X:
                    AddCookieInQueue(Array[i], wxJS_AXIS_X);
                    break;                    
                case kHIDUsage_GD_Y:
                    AddCookieInQueue(Array[i], wxJS_AXIS_Y);
                    break;
                case kHIDUsage_GD_Z:
                    AddCookieInQueue(Array[i], wxJS_AXIS_Z);
                    break;
                default:
                    break;
            }
        }
        else if (nPage == kHIDPage_Simulation && nUsage == kHIDUsage_Sim_Rudder)
			AddCookieInQueue(Array[i], wxJS_AXIS_RUDDER );
	}
}//end buildcookies




IMPLEMENT_DYNAMIC_CLASS(wxJoystick, wxObject)


////////////////////////////////////////////////////////////////////////////
// Background thread for reading the joystick device
////////////////////////////////////////////////////////////////////////////

class wxJoystickThread : public wxThread
{
public:
    wxJoystickThread(wxHIDJoystick* hid, int joystick);
    void* Entry();

private:
    wxHIDJoystick*       m_hid;
    int       m_joystick;
    wxPoint   m_lastposition;
    int       m_axe[15];
    int       m_buttons;
    wxWindow* m_catchwin;
    int       m_polling;

    friend class wxJoystick;
};


wxJoystickThread::wxJoystickThread(wxHIDJoystick* hid, int joystick)
    : m_hid(hid),
      m_joystick(joystick),
      m_lastposition(wxDefaultPosition),
      m_buttons(0),
      m_catchwin(NULL),
      m_polling(0)
{
    for (int i=0; i<15; i++)
        m_axe[i] = 0;
}


#	define wxJSVERIFY(arg)	if(!(arg)) {wxLogSysError(wxT(#arg)); return NULL;}
#	define wxJSASSERT(arg)	wxJSVERIFY(arg)

void* wxJoystickThread::Entry()
{
    CFRunLoopSourceRef pRLSource = NULL;

    wxJSVERIFY( (*m_hid->GetQueue())->createAsyncEventSource(m_hid->GetQueue(), &pRLSource) 
                        == kIOReturnSuccess );
    wxJSASSERT(pRLSource != NULL);

    //attach runloop source to main run loop in thread
    CFRunLoopRef pRL = CFRunLoopGetCurrent();  
    CFRunLoopAddSource(pRL, pRLSource, kCFRunLoopDefaultMode);
      
//    wxJSVERIFY( (*m_hid->GetQueue())->start(m_hid->GetQueue()) == kIOReturnSuccess ); 
    
    double dTime;
    IOHIDEventStruct hidevent;
    AbsoluteTime bogustime = {0,0};
    IOReturn ret;
    
    while(true)
    {
        if (TestDestroy())
            break;

        if (m_polling)
            dTime = 0.0001 * m_polling;
        else
            dTime = 0.0001 * 10;  // check at least every 10 msec in blocking case

        CFRunLoopRunInMode(kCFRunLoopDefaultMode, dTime, m_polling);        
        
        while (    (ret = (*m_hid->GetQueue())->getNextEvent(m_hid->GetQueue(), 
                        &hidevent, bogustime, 0)) != kIOReturnUnderrun )
        {
            if (TestDestroy())
                break;

            wxJSASSERT(ret == kIOReturnSuccess);
            wxJoystickEvent wxevent;
            
            int nIndex = 0;
            IOHIDElementCookie* pCookies = m_hid->GetCookies();
            while(nIndex < 50)
            {
                if(hidevent.elementCookie == pCookies[nIndex])
                    break;
            } 
            wxASSERT(nIndex != 50);
            
            if (nIndex < 40)
            {
                if (hidevent.value)
                {
                    m_buttons |= (1 << nIndex);
                    wxevent.SetEventType(wxEVT_JOY_BUTTON_DOWN);
                }
                else
                {
                    m_buttons &= ~(1 << nIndex);
                    wxevent.SetEventType(wxEVT_JOY_BUTTON_UP);
                }

                wxevent.SetButtonChange(nIndex);
            }
            else if (nIndex == wxJS_AXIS_X)
            {
                m_lastposition.x = hidevent.value;
                wxevent.SetEventType(wxEVT_JOY_MOVE);
                m_axe[nIndex - 39] = hidevent.value;
            }
            else if (nIndex == wxJS_AXIS_Y)
            {
                m_lastposition.y = hidevent.value;
                wxevent.SetEventType(wxEVT_JOY_MOVE);
                m_axe[nIndex - 39] = hidevent.value;
            }
            else if (nIndex == wxJS_AXIS_Z)
            {
                wxevent.SetEventType(wxEVT_JOY_ZMOVE);
                m_axe[nIndex - 39] = hidevent.value;
            }
            else
                wxevent.SetEventType(wxEVT_JOY_MOVE);            

            Nanoseconds timestamp = AbsoluteToNanoseconds(hidevent.timestamp);
            
            wxULongLong llTime(timestamp.hi, timestamp.lo);
            
            llTime /= 1000000;
            
            wxevent.SetTimestamp(llTime.GetValue());
            wxevent.SetJoystick(m_joystick);
            wxevent.SetButtonState(m_buttons);
            wxevent.SetPosition(m_lastposition);
            wxevent.SetZPosition(m_axe[3]);
            wxevent.SetEventObject(m_catchwin);

            if (m_catchwin)
                m_catchwin->AddPendingEvent(wxevent);            
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////

wxJoystick::wxJoystick(int joystick)
    : m_joystick(joystick),
      m_thread(NULL)
{
    m_hid = new wxHIDJoystick();

    if (m_hid->Create(m_joystick))
    {
        m_thread = new wxJoystickThread(m_hid, m_joystick);
        m_thread->Create();
        m_thread->Run();
    }
    else
    {
        delete m_hid;
        m_hid = NULL;
    }
}


wxJoystick::~wxJoystick()
{
    ReleaseCapture();
    if (m_thread)
        m_thread->Delete();  // It's detached so it will delete itself
        
    if (m_hid)
        delete m_hid;
}


////////////////////////////////////////////////////////////////////////////
// State
////////////////////////////////////////////////////////////////////////////

wxPoint wxJoystick::GetPosition() const
{
    wxPoint pos(wxDefaultPosition);
    if (m_thread) pos = m_thread->m_lastposition;
    return pos;
}

int wxJoystick::GetZPosition() const
{
    if (m_thread)
        return m_thread->m_axe[wxJS_AXIS_Z];
    return 0;
}

int wxJoystick::GetButtonState() const
{
    if (m_thread)
        return m_thread->m_buttons;
    return 0;
}

int wxJoystick::GetPOVPosition() const
{	return -1;				}

int wxJoystick::GetPOVCTSPosition() const
{	return -1;				}

int wxJoystick::GetRudderPosition() const
{	
    if (m_thread)
        return m_thread->m_axe[wxJS_AXIS_RUDDER];
    return 0;
}

int wxJoystick::GetUPosition() const
{
    if (m_thread)
        return m_thread->m_axe[wxJS_AXIS_U];
    return 0;
}

int wxJoystick::GetVPosition() const
{
    if (m_thread)
        return m_thread->m_axe[wxJS_AXIS_V];
    return 0;
}

int wxJoystick::GetMovementThreshold() const
{	return 0;				}

void wxJoystick::SetMovementThreshold(int threshold)
{							}

////////////////////////////////////////////////////////////////////////////
// Capabilities
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::IsOk() const
{	return m_hid != NULL;	}

int wxJoystick::GetNumberJoysticks() const
{    return wxHIDDevice::GetCount(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick) +
            wxHIDDevice::GetCount(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);   }
    
int wxJoystick::GetManufacturerId() const
{	return 0;				}	

int wxJoystick::GetProductId() const
{	return 0;				}

wxString wxJoystick::GetProductName() const
{	return wxT("unknown");	}

int wxJoystick::GetXMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetYMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetZMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetXMax() const
{	return wxJS_AXIS_MAX;	}

int wxJoystick::GetYMax() const
{	return wxJS_AXIS_MAX;	}

int wxJoystick::GetZMax() const
{	return wxJS_AXIS_MAX;	}

int wxJoystick::GetNumberButtons() const
{
    int nCount = 0;
    
    for(int nIndex = 0; nIndex < 40; ++nIndex)
    {
        if(m_hid->HasElement(nIndex))
            ++nCount;
    }
    
    return nCount;
}

int wxJoystick::GetNumberAxes() const
{
    int nCount = 0;
    
    for(int nIndex = 40; nIndex < 50; ++nIndex)
    {
        if(m_hid->HasElement(nIndex))
            ++nCount;
    }
    
    return nCount;
}

//
// internal
//
int wxJoystick::GetMaxButtons() const
{	return 15; 	}

int wxJoystick::GetMaxAxes() const
{	return 10; 	}

int wxJoystick::GetPollingMin() const
{	return 10;	}

int wxJoystick::GetPollingMax() const
{	return 1000;	}

int wxJoystick::GetRudderMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetRudderMax() const
{	return wxJS_AXIS_MAX;	}

int wxJoystick::GetUMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetUMax() const
{	return wxJS_AXIS_MAX;	}

int wxJoystick::GetVMin() const
{	return wxJS_AXIS_MIN;	}

int wxJoystick::GetVMax() const
{	return wxJS_AXIS_MAX;	}

bool wxJoystick::HasRudder() const
{	return m_hid->HasElement(wxJS_AXIS_RUDDER);	}

bool wxJoystick::HasZ() const
{	return m_hid->HasElement(wxJS_AXIS_Z);	}

bool wxJoystick::HasU() const
{	return m_hid->HasElement(wxJS_AXIS_U);	}

bool wxJoystick::HasV() const
{	return m_hid->HasElement(wxJS_AXIS_V);	}

bool wxJoystick::HasPOV() const
{	return false;	}

bool wxJoystick::HasPOV4Dir() const
{	return false;	}

bool wxJoystick::HasPOVCTS() const
{	return false;	}

////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////

bool wxJoystick::SetCapture(wxWindow* win, int pollingFreq)
{
    if (m_thread)
    {
        m_thread->m_catchwin = win;
        m_thread->m_polling = pollingFreq;
        return true;
    }
    return false;
}

bool wxJoystick::ReleaseCapture()
{
    if (m_thread)
    {
        m_thread->m_catchwin = NULL;
        m_thread->m_polling = 0;
        return true;
    }
    return false;
}
#endif
    //OSX 
    
#endif
    // wxUSE_JOYSTICK

