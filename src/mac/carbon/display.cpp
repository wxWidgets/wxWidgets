/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/display.cpp
// Purpose:     Mac implementation of wxDisplay class
// Author:      Ryan Norton & Brian Victor
// Modified by: Royce Mitchell III, Vadim Zeitlin
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#include "wx/display.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/log.h"
    #include "wx/string.h"
    #include "wx/gdicmn.h"
#endif

#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <Gestalt.h>
    #include <Displays.h>
    #include <Quickdraw.h>
    #include <Video.h>  // for VDSwitchInfoRec
    #include <FixMath.h>
    #include <Debugging.h>
#endif

#include "wx/display_impl.h"

// ----------------------------------------------------------------------------
// display classes implementation
// ----------------------------------------------------------------------------

#ifdef __WXMAC_OSX__

class wxDisplayImplMacOSX : public wxDisplayImpl
{
public:
    wxDisplayImplMacOSX(unsigned n, CGDirectDisplayID id)
        : wxDisplayImpl(n),
          m_id(id)
    {
    }

    virtual wxRect GetGeometry() const;
    virtual wxRect GetClientArea() const;
    virtual wxString GetName() const { return wxString(); }

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode);

    virtual bool IsPrimary() const;

private:
    CGDirectDisplayID m_id;

    DECLARE_NO_COPY_CLASS(wxDisplayImplMacOSX)
};

class wxDisplayFactoryMacOSX : public wxDisplayFactory
{
public:
    wxDisplayFactoryMacOSX() {}

    virtual wxDisplayImpl *CreateDisplay(unsigned n);
    virtual unsigned GetCount();
    virtual int GetFromPoint(const wxPoint& pt);

protected:
    DECLARE_NO_COPY_CLASS(wxDisplayFactoryMacOSX)
};

// ============================================================================
// wxDisplayFactoryMacOSX implementation
// ============================================================================

// gets all displays that are not mirror displays

static CGDisplayErr wxOSXGetDisplayList(CGDisplayCount maxDisplays,
                                   CGDirectDisplayID *displays,
                                   CGDisplayCount *displayCount)
{
    CGDisplayErr error = kCGErrorSuccess;
    CGDisplayCount onlineCount;
    
    error = CGGetOnlineDisplayList(0,NULL,&onlineCount);
    if ( error == kCGErrorSuccess )
    {
        *displayCount = 0;
        if ( onlineCount > 0 )
        {
            CGDirectDisplayID *onlineDisplays = new CGDirectDisplayID[onlineCount];
            error = CGGetOnlineDisplayList(onlineCount,onlineDisplays,&onlineCount);
            if ( error == kCGErrorSuccess )
            {
                for ( CGDisplayCount i = 0; i < onlineCount; ++i )
                {
                    if ( CGDisplayMirrorsDisplay(onlineDisplays[i]) != kCGNullDirectDisplay )
                        continue;
                    
                    if ( displays == NULL )
                        *displayCount += 1;
                    else
                    {
                        if ( *displayCount < maxDisplays )
                        {
                            displays[*displayCount] = onlineDisplays[i];
                            *displayCount += 1;
                        }
                    }
                }
            }
            delete[] onlineDisplays;
        }
            
    }
    return error;
}

unsigned wxDisplayFactoryMacOSX::GetCount()
{
    CGDisplayCount count;
    CGDisplayErr err = wxOSXGetDisplayList(0, NULL, &count);

    wxCHECK_MSG( err == CGDisplayNoErr, 0, wxT("wxOSXGetDisplayList() failed") );

    return count;
}

int wxDisplayFactoryMacOSX::GetFromPoint(const wxPoint& p)
{
    CGPoint thePoint = {(float)p.x, (float)p.y};
    CGDirectDisplayID theID;
    CGDisplayCount theCount;
    CGDisplayErr err = CGGetDisplaysWithPoint(thePoint, 1, &theID, &theCount);
    wxASSERT(err == CGDisplayNoErr);

    int nWhich = wxNOT_FOUND;

    if (theCount)
    {
        theCount = GetCount();
        CGDirectDisplayID* theIDs = new CGDirectDisplayID[theCount];
        err = wxOSXGetDisplayList(theCount, theIDs, &theCount);
        wxASSERT(err == CGDisplayNoErr);

        for (nWhich = 0; nWhich < (int) theCount; ++nWhich)
        {
            if (theIDs[nWhich] == theID)
                break;
        }

        delete [] theIDs;

        if (nWhich == (int) theCount)
        {
            wxFAIL_MSG(wxT("Failed to find display in display list"));
            nWhich = wxNOT_FOUND;
        }
    }

    return nWhich;
}

wxDisplayImpl *wxDisplayFactoryMacOSX::CreateDisplay(unsigned n)
{
    CGDisplayCount theCount = GetCount();
    CGDirectDisplayID* theIDs = new CGDirectDisplayID[theCount];

    CGDisplayErr err = wxOSXGetDisplayList(theCount, theIDs, &theCount);
    wxCHECK_MSG( err == CGDisplayNoErr, NULL, wxT("wxOSXGetDisplayList() failed") );

    wxASSERT( n < theCount );

    wxDisplayImplMacOSX *display = new wxDisplayImplMacOSX(n, theIDs[n]);

    delete [] theIDs;

    return display;
}

// ============================================================================
// wxDisplayImplMacOSX implementation
// ============================================================================

bool wxDisplayImplMacOSX::IsPrimary() const
{
    return CGDisplayIsMain(m_id);
}

wxRect wxDisplayImplMacOSX::GetGeometry() const
{
    CGRect theRect = CGDisplayBounds(m_id);
    return wxRect( (int)theRect.origin.x,
                   (int)theRect.origin.y,
                   (int)theRect.size.width,
                   (int)theRect.size.height ); //floats
}

wxRect wxDisplayImplMacOSX::GetClientArea() const
{
    // VZ: I don't know how to get client area for arbitrary display but
    //     wxGetClientDisplayRect() does work correctly for at least the main
    //     one (TODO: do it correctly for the other displays too)
    if ( IsPrimary() )
        return wxGetClientDisplayRect();

    return wxDisplayImpl::GetClientArea();
}

static int wxCFDictKeyToInt( CFDictionaryRef desc, CFStringRef key )
{
    CFNumberRef value = (CFNumberRef) CFDictionaryGetValue( desc, key );
    if (value == NULL)
        return 0;

    int num = 0;
    CFNumberGetValue( value, kCFNumberIntType, &num );

    return num;
}

wxArrayVideoModes wxDisplayImplMacOSX::GetModes(const wxVideoMode& mode) const
{
    wxArrayVideoModes resultModes;

    CFArrayRef theArray = CGDisplayAvailableModes( m_id );

    for (CFIndex i = 0; i < CFArrayGetCount(theArray); ++i)
    {
        CFDictionaryRef theValue = (CFDictionaryRef) CFArrayGetValueAtIndex( theArray, i );

        wxVideoMode theMode(
            wxCFDictKeyToInt( theValue, kCGDisplayWidth ),
            wxCFDictKeyToInt( theValue, kCGDisplayHeight ),
            wxCFDictKeyToInt( theValue, kCGDisplayBitsPerPixel ),
            wxCFDictKeyToInt( theValue, kCGDisplayRefreshRate ));

        if (theMode.Matches( mode ))
            resultModes.Add( theMode );
    }

    return resultModes;
}

wxVideoMode wxDisplayImplMacOSX::GetCurrentMode() const
{
    CFDictionaryRef theValue = CGDisplayCurrentMode( m_id );

    return wxVideoMode(
        wxCFDictKeyToInt( theValue, kCGDisplayWidth ),
        wxCFDictKeyToInt( theValue, kCGDisplayHeight ),
        wxCFDictKeyToInt( theValue, kCGDisplayBitsPerPixel ),
        wxCFDictKeyToInt( theValue, kCGDisplayRefreshRate ));
}

bool wxDisplayImplMacOSX::ChangeMode( const wxVideoMode& mode )
{
    // Changing to default mode (wxDefaultVideoMode) doesn't
    // work because we don't have access to the system's 'scrn'
    // resource which holds the user's mode which the system
    // will return to after this app is done
    boolean_t bExactMatch;
    CFDictionaryRef theCGMode = CGDisplayBestModeForParametersAndRefreshRate(
        m_id,
        (size_t)mode.bpp,
        (size_t)mode.w,
        (size_t)mode.h,
        (double)mode.refresh,
        &bExactMatch );

    bool bOK = bExactMatch;

    if (bOK)
        bOK = CGDisplaySwitchToMode( m_id, theCGMode ) == CGDisplayNoErr;

    return bOK;
}

// ============================================================================
// wxDisplay::CreateFactory()
// ============================================================================

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryMacOSX;
}

#else // !__WXMAC_OSX__

class wxDisplayImplMac : public wxDisplayImpl
{
public:
    wxDisplayImplMac(unsigned n, GDHandle hndl)
        : wxDisplayImpl(n),
          m_hndl(hndl)
    {
    }

    virtual wxRect GetGeometry() const;
    virtual wxString GetName() const { return wxString(); }

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode);

private:
    GDHandle m_hndl;

    DECLARE_NO_COPY_CLASS(wxDisplayImplMac)
};

class wxDisplayFactoryMac : public wxDisplayFactory
{
public:
    wxDisplayFactoryMac();

    virtual wxDisplayImpl *CreateDisplay(unsigned n);
    virtual unsigned GetCount();
    virtual int GetFromPoint(const wxPoint& pt);

protected:
    DECLARE_NO_COPY_CLASS(wxDisplayFactoryMac)
};

// ============================================================================
// wxDisplayFactoryMac implementation
// ============================================================================

unsigned wxDisplayFactoryMac::GetCount()
{
    unsigned num = 0;
    GDHandle hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        num++;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
    return num;
}

int wxDisplayFactoryMac::GetFromPoint(const wxPoint &p)
{
    unsigned num = 0;
    GDHandle hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        Rect screenrect = (*hndl)->gdRect;
        if (p.x >= screenrect.left &&
            p.x <= screenrect.right &&
            p.y >= screenrect.top &&
            p.y <= screenrect.bottom)
        {
            return num;
        }
        num++;
        hndl = DMGetNextScreenDevice(hndl, true);
    }

    return wxNOT_FOUND;
}

wxDisplayImpl *wxDisplayFactoryMac::CreateDisplay(unsigned n)
{
    unsigned nOrig = n;

    GDHandle hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        if (n == 0)
        {
            return new wxDisplayImplMac(nOrig, hndl);
        }
        n--;
        hndl = DMGetNextScreenDevice(hndl, true);
    }

    return NULL;
}

// ============================================================================
// wxDisplayImplMac implementation
// ============================================================================

wxRect wxDisplayImplMac::GetGeometry() const
{
    Rect screenrect = (*m_hndl)->gdRect;
    return wxRect(screenrect.left, screenrect.top,
                  screenrect.right - screenrect.left,
                  screenrect.bottom - screenrect.top);
}

struct DMModeIteratorRec
{
    wxArrayVideoModes* pModes;
    const wxVideoMode* pMatchMode;
};

pascal void DMModeListIteratorProc(
    void* pData,
    DMListIndexType nIndex,
    DMDisplayModeListEntryPtr pInfo)
{
    DMModeIteratorRec* pInfoData = (DMModeIteratorRec*) pData;

    // Note that in testing the refresh rate is always 0 on my ibook - RN
    int refresh = (int) Fix2Long(pInfo->displayModeResolutionInfo->csRefreshRate);

#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock

    for (unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
        if (wxVideoMode( (int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                        (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                        (int) pDBI->vpPixelSize,
                        refresh).Matches(*pInfoData->pMatchMode) )
        {
            pInfoData->pModes->Add(
                wxVideoMode(
                    (int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                    (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                    (int) pDBI->vpPixelSize,
                    refresh ) );
        }
    }

#undef pDBI
}

struct DMModeInfoRec
{
    const wxVideoMode* pMode;
    VDSwitchInfoRec sMode;
    bool bMatched;
};

pascal void DMModeInfoProc(
    void* pData,
    DMListIndexType nIndex,
    DMDisplayModeListEntryPtr pInfo )
{
    DMModeInfoRec* pInfoData = (DMModeInfoRec*) pData;
    Fixed refresh = Long2Fix(pInfoData->pMode->refresh);

#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock

    for (unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
        if (pInfoData->pMode->w == (int&) pInfo->displayModeResolutionInfo->csHorizontalPixels &&
            pInfoData->pMode->h == (int&) pInfo->displayModeResolutionInfo->csVerticalLines &&
            pInfoData->pMode->bpp == (int) pDBI->vpPixelSize &&
            refresh == pInfo->displayModeResolutionInfo->csRefreshRate)
        {
            memcpy(
                &pInfoData->sMode,
                pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthSwitchInfo,
                sizeof(VDSwitchInfoRec));
            pInfoData->sMode.csMode = pDBI->vpPixelSize;
            pInfoData->bMatched = true;
            break;
        }
    }

#undef pDBI
}

struct DMModeTransRec
{
    wxVideoMode Mode;
    const VDSwitchInfoRec* psMode;
    bool bMatched;
};

pascal void DMModeTransProc(
    void* pData,
    DMListIndexType nIndex,
    DMDisplayModeListEntryPtr pInfo)
{
    DMModeTransRec* pInfoData = (DMModeTransRec*) pData;

#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock

    for (unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
        if (pInfoData->psMode->csData == pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthSwitchInfo->csData)
        {
            pInfoData->Mode = wxVideoMode(
                (int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                (int) pDBI->vpPixelSize,
                (int) Fix2Long(pInfo->displayModeResolutionInfo->csRefreshRate) );
            pInfoData->bMatched = true;
            break;
        }
    }

#undef pDBI
}

wxArrayVideoModes wxDisplayImplMac::GetModes(const wxVideoMode& mode) const
{
    wxArrayVideoModes Modes;
    unsigned long dwDMVer;

    // Check DM version == 2
    // (for backward compatibility only - 7.5.3+ use 2.0)
    Gestalt( gestaltDisplayMgrVers, (long*) &dwDMVer );
    if (dwDMVer >= 0x020000)
    {
        DMListIndexType nNumModes;
        DMListType pModes;
        DMDisplayModeListIteratorUPP uppMLI;
        DisplayIDType nDisplayID;
        OSErr err;

        err = DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false);
        verify_noerr( err );

        // Create a new list...
        err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
        wxASSERT_MSG( err == noErr, wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeListIteratorProc);
        wxASSERT( uppMLI );

        DMModeIteratorRec sModeInfo;
        sModeInfo.pModes = &Modes;
        sModeInfo.pMatchMode = &mode;

        for (DMListIndexType i = 0; i < nNumModes; ++i)
        {
            err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
            verify_noerr( err );
        }

        DisposeDMDisplayModeListIteratorUPP(uppMLI);
        err = DMDisposeList(pModes);
        verify_noerr( err );
    }
    else // DM 1.0, 1.2, 1.x
    {
        wxLogSysError(
            wxString::Format(
                wxT("Display Manager Version %u Not Supported!  Present? %s"),
                (unsigned int) dwDMVer / 0x10000,
                (dwDMVer & (1 << gestaltDisplayMgrPresent) ? wxT("Yes") : wxT("No")) ) );
    }

    return Modes;
}

wxVideoMode wxDisplayImplMac::GetCurrentMode() const
{
    unsigned long dwDMVer;
    wxVideoMode RetMode;

    // Check DM version == 2
    // (for backward compatibility only - 7.5.3+ use 2.0)
    Gestalt( gestaltDisplayMgrVers, (long*) &dwDMVer );
    if (dwDMVer >= 0x020000)
    {
        VDSwitchInfoRec sMode; // Note: csMode member also contains the bit depth
        OSErr err;

        err = DMGetDisplayMode( m_hndl, &sMode );
        if (err == noErr)
        {
            DMListIndexType nNumModes;
            DMListType pModes;
            DMDisplayModeListIteratorUPP uppMLI;
            DisplayIDType nDisplayID;

            err = DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false);
            verify_noerr( err );

            // Create a new list...
            err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
            wxASSERT_MSG( err == noErr, wxT("Could not create a new display mode list") );

            uppMLI = NewDMDisplayModeListIteratorUPP(DMModeTransProc);
            wxASSERT( uppMLI );

            DMModeTransRec sModeInfo;
            sModeInfo.bMatched = false;
            sModeInfo.psMode = &sMode;
            for (DMListIndexType i = 0; i < nNumModes; ++i)
            {
                err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
                verify_noerr( err );

                if ( sModeInfo.bMatched )
                {
                    RetMode = sModeInfo.Mode;
                    break;
                }
            }

            DisposeDMDisplayModeListIteratorUPP(uppMLI);
            err = DMDisposeList(pModes);
            verify_noerr( err );
        }
        else // Can't get current mode?
        {
            wxLogSysError(
                wxString::Format(
                    wxT("Couldn't obtain current display mode!!!\ndwDMVer:%u"),
                    (unsigned int) dwDMVer));
        }
    }
    else // DM ver 1
    {
        wxLogSysError(
            wxString::Format(
                wxT("Display Manager Version %u Not Supported!  Present? %s"),
                (unsigned int) dwDMVer / 0x10000,
                (dwDMVer & (1 << gestaltDisplayMgrPresent) ? wxT("Yes") : wxT("No")) ) );
    }

    return RetMode;
}

bool wxDisplayImplMac::ChangeMode(const wxVideoMode& mode)
{
    unsigned long dwDMVer;

    Gestalt( gestaltDisplayMgrVers, (long*)&dwDMVer );
    if (GetCount() == 1 || dwDMVer >= 0x020000)
    {
        if (mode == wxDefaultVideoMode)
        {
             return true;

#if 0
//#ifndef __DARWIN__
//            Handle hDisplayState;
//            if (DMBeginConfigureDisplays(&hDisplayState) != noErr)
//            {
//                wxLogSysError(wxT("Could not lock display for display mode changing!"));
//                return false;
//            }
//
//            wxASSERT( DMUseScreenPrefs(true, hDisplayState) == noErr);
//            DMEndConfigureDisplays(hDisplayState);
//            return true;
//#else
             // hmmmmm....
//           return true;
//#endif
#endif
        }

        //0 & NULL for params 2 & 3 of DMSetVideoMode signal it to use defaults (current mode)
        //DM 2.0+ doesn't use params 2 & 3 of DMSetDisplayMode
        //so we have to use this icky structure
        VDSwitchInfoRec sMode;
        memset( &sMode, 0, sizeof(VDSwitchInfoRec) );

        DMListIndexType nNumModes;
        DMListType pModes;
        DMDisplayModeListIteratorUPP uppMLI;
        DisplayIDType nDisplayID;
        OSErr err;

        err = DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false);
        verify_noerr( err );

        // Create a new list...
        err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
        wxASSERT_MSG(err == noErr, wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeInfoProc);
        wxASSERT(uppMLI);

        DMModeInfoRec sModeInfo;
        sModeInfo.bMatched = false;
        sModeInfo.pMode = &mode;
        unsigned int i;

        for (i = 0; i < nNumModes; ++i)
        {
            err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
            verify_noerr( err );

            if (sModeInfo.bMatched)
            {
                sMode = sModeInfo.sMode;
                break;
            }
        }

        if (i == nNumModes)
            return false;

        DisposeDMDisplayModeListIteratorUPP(uppMLI);

        err = DMDisposeList(pModes);
        verify_noerr( err );

        // For the really paranoid -
        //     unsigned long flags;
        //      Boolean bok;
        //     wxASSERT(noErr == DMCheckDisplayMode(m_hndl, sMode.csData,
        //                                          sMode.csMode, &flags, NULL, &bok));
        //     wxASSERT(bok);

        Handle hDisplayState;
        if (DMBeginConfigureDisplays(&hDisplayState) != noErr)
        {
            wxLogSysError(wxT("Could not lock display for display mode changing!"));

            return false;
        }

        unsigned long dwBPP = (unsigned long) mode.bpp;
        err = DMSetDisplayMode(
            m_hndl, sMode.csData,
            (unsigned long*) &(dwBPP),
            NULL, //(unsigned long) &sMode
            hDisplayState );

        if (err != noErr)
        {
            DMEndConfigureDisplays(hDisplayState);
            wxLogError(wxT("Could not set the display mode"));

            return false;
        }

        DMEndConfigureDisplays(hDisplayState);
    }
    else  // DM 1.0, 1.2, 1.x
    {
        wxLogSysError(
            wxString::Format(
                wxT("Monitor gravitation not supported yet.  dwDMVer:%u"),
                (unsigned int) dwDMVer));

        return false;
    }

    return true;
}

// ============================================================================
// wxDisplay::CreateFactory()
// ============================================================================

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryMac;
}

#endif // !OSX

#endif // wxUSE_DISPLAY
