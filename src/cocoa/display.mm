/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/display.mm
// Purpose:     Cocoa implementation of wxDisplay class
// Author:      Ryan Norton
// Modified by: 
// Created:     2004-10-03
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/dynarray.h"
#endif

#include "wx/display.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

#import <Foundation/Foundation.h>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

size_t wxDisplayBase::GetCount()
{
    CGDisplayCount count;
#ifdef __WXDEBUG__
    CGDisplayErr err = 
#endif
    CGGetActiveDisplayList(0, NULL, &count);

    wxASSERT(err == CGDisplayNoErr);
    return count;
}

int wxDisplayBase::GetFromPoint(const wxPoint &p)
{   
    CGPoint thePoint = {(float)p.x, (float)p.y};
    CGDirectDisplayID theID;
    CGDisplayCount theCount;
    CGDisplayErr err = CGGetDisplaysWithPoint(thePoint, 1, &theID, &theCount);
    wxASSERT(err == CGDisplayNoErr);
    int nWhich = -1;
    
    if (theCount)
    {
        theCount = GetCount();
        CGDirectDisplayID* theIDs = new CGDirectDisplayID[theCount];
        err = CGGetActiveDisplayList(theCount, theIDs, &theCount);
        wxASSERT(err == CGDisplayNoErr);

        for(nWhich = 0; nWhich < (int) theCount; ++nWhich)
        {
            if(theIDs[nWhich] == theID)
                break;
        }
        
        delete[] theIDs;
        
        if(nWhich == (int) theCount)
        {
            wxFAIL_MSG(wxT("Failed to find display in display list"));
            nWhich = -1;
        }
    }
    
    return nWhich;
}//CFUserNotification[NSBundle bundleForClass:[self class]]

wxDisplay::wxDisplay(size_t index) : wxDisplayBase ( index )
{
    CGDisplayCount theCount = GetCount();
    CGDirectDisplayID* theIDs = new CGDirectDisplayID[theCount];
#ifdef __WXDEBUG__
    CGDisplayErr err = 
#endif
    CGGetActiveDisplayList(theCount, theIDs, &theCount);

    wxASSERT(err == CGDisplayNoErr);
    wxASSERT(index < theCount);
    
    m_id = theIDs[index];
    
    delete[] theIDs;
}

wxRect wxDisplay::GetGeometry() const
{
    CGRect theRect = CGDisplayBounds(m_id);
    return wxRect(	(int)theRect.origin.x,
                    (int)theRect.origin.y,
                    (int)theRect.size.width,
                    (int)theRect.size.height  ); //floats
}

int wxDisplay::GetDepth() const
{
    return (int) CGDisplayBitsPerPixel(m_id); //size_t
}

wxString wxDisplay::GetName() const
{
    // Macs don't name their displays...
    return wxEmptyString;
}

static int wxCFDictKeyToInt( CFDictionaryRef desc, CFStringRef key )
{
    CFNumberRef value;
    int num = 0;

    if ( (value = (CFNumberRef) CFDictionaryGetValue(desc, key)) == NULL )
        return 0;
    CFNumberGetValue(value, kCFNumberIntType, &num);
    return num;
}

wxArrayVideoModes
    wxDisplay::GetModes(const wxVideoMode& mode) const
{
    wxArrayVideoModes Modes;
    
    CFArrayRef theArray = CGDisplayAvailableModes(m_id);

    for(CFIndex i = 0; i < CFArrayGetCount(theArray); ++i)
    {
        CFDictionaryRef theValue = (CFDictionaryRef) CFArrayGetValueAtIndex(theArray, i);
        
        wxVideoMode theMode(wxCFDictKeyToInt(theValue, kCGDisplayWidth),
                            wxCFDictKeyToInt(theValue, kCGDisplayHeight),
                            wxCFDictKeyToInt(theValue, kCGDisplayBitsPerPixel),
                            wxCFDictKeyToInt(theValue, kCGDisplayRefreshRate));
        
        if (theMode.Matches(mode))
            Modes.Add(theMode);
    }
    
    return Modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    CFDictionaryRef theValue = CGDisplayCurrentMode (m_id);
    
    return wxVideoMode(wxCFDictKeyToInt(theValue, kCGDisplayWidth),
                            wxCFDictKeyToInt(theValue, kCGDisplayHeight),
                            wxCFDictKeyToInt(theValue, kCGDisplayBitsPerPixel),
                            wxCFDictKeyToInt(theValue, kCGDisplayRefreshRate));
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    //Changing to default mode (wxDefualtVideoMode) doesn't
    //work because we don't have access to the system's 'scrn'
    //resource which holds the user's mode which the system
    //will return to after this app is done
    boolean_t bExactMatch;
    CFDictionaryRef theCGMode = CGDisplayBestModeForParametersAndRefreshRate (
                                        m_id,
                                        (size_t)mode.bpp,
                                        (size_t)mode.w,
                                        (size_t)mode.h,
                                        (double)mode.refresh,
                                        &bExactMatch);
    
    bool bOK = bExactMatch;
    
    if(bOK)
        bOK = CGDisplaySwitchToMode(m_id, theCGMode) == CGDisplayNoErr;

    return bOK;
}

wxDisplay::~wxDisplay()
{
}

#endif // wxUSE_DISPLAY
