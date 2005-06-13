/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     Mac implementation of wxDisplay class
// Author:      Ryan Norton & Brian Victor
// Modified by: Royce Mitchell III
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
   #include "wx/log.h"
   #include "wx/msgdlg.h"
#endif

#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <Gestalt.h>
    #include <Displays.h>
    #include <Quickdraw.h>
    #include <Video.h>  //for VDSwitchInfoRec
    #include <FixMath.h>
#endif

#include "wx/display.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#ifdef __WXMAC_OSX__

class wxDisplayMacPriv
{
public:
    CGDirectDisplayID m_id;
};

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

wxDisplay::wxDisplay(size_t index) : wxDisplayBase ( index ) ,
    m_priv ( new wxDisplayMacPriv() )
{
    CGDisplayCount theCount = GetCount();
    CGDirectDisplayID* theIDs = new CGDirectDisplayID[theCount];
#ifdef __WXDEBUG__
    CGDisplayErr err =
#endif
    CGGetActiveDisplayList(theCount, theIDs, &theCount);

    wxASSERT(err == CGDisplayNoErr);
    wxASSERT(index < theCount);

    m_priv->m_id = theIDs[index];

    delete[] theIDs;
}

wxRect wxDisplay::GetGeometry() const
{
    CGRect theRect = CGDisplayBounds(m_priv->m_id);
    return wxRect( (int)theRect.origin.x,
                   (int)theRect.origin.y,
                   (int)theRect.size.width,
                   (int)theRect.size.height  ); //floats
}

int wxDisplay::GetDepth() const
{
    return (int) CGDisplayBitsPerPixel(m_priv->m_id); //size_t
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

    CFArrayRef theArray = CGDisplayAvailableModes(m_priv->m_id);

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
    CFDictionaryRef theValue = CGDisplayCurrentMode (m_priv->m_id);

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
                                        m_priv->m_id,
                                        (size_t)mode.bpp,
                                        (size_t)mode.w,
                                        (size_t)mode.h,
                                        (double)mode.refresh,
                                        &bExactMatch);

    bool bOK = bExactMatch;

    if(bOK)
        bOK = CGDisplaySwitchToMode(m_priv->m_id, theCGMode) == CGDisplayNoErr;

    return bOK;
}

wxDisplay::~wxDisplay()
{
    if ( m_priv )
    {
        delete m_priv;
        m_priv = 0;
    }
}

#else

class wxDisplayMacPriv
{
public:
    GDHandle m_hndl;
};

size_t wxDisplayBase::GetCount()
{
    GDHandle hndl;
    size_t num = 0;
    hndl = DMGetFirstScreenDevice(true);
    while(hndl)
    {
        num++;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
    return num;
}

int wxDisplayBase::GetFromPoint(const wxPoint &p)
{
    GDHandle hndl;
    size_t num = 0;
    hndl = DMGetFirstScreenDevice(true);
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
    return -1;
}

wxDisplay::wxDisplay(size_t index) : wxDisplayBase ( index ),
    m_priv ( new wxDisplayMacPriv() )
{
    GDHandle hndl;
    hndl = DMGetFirstScreenDevice(true);
    m_priv->m_hndl = NULL;
    while(hndl)
    {
        if (index == 0)
        {
            m_priv->m_hndl = hndl;
        }
        index--;
        hndl = DMGetNextScreenDevice(hndl, true);
    }
}

wxRect wxDisplay::GetGeometry() const
{
    if (!(m_priv)) return wxRect(0, 0, 0, 0);
    if (!(m_priv->m_hndl)) return wxRect(0, 0, 0, 0);
    Rect screenrect = (*(m_priv->m_hndl))->gdRect;
    return wxRect( screenrect.left, screenrect.top,
                   screenrect.right - screenrect.left, screenrect.bottom - screenrect.top);
}

int wxDisplay::GetDepth() const
{
    if (!(m_priv)) return 0;
    if (!(m_priv->m_hndl)) return 0;

    // This cryptic looking code is based on Apple's sample code:
    // http://developer.apple.com/samplecode/Sample_Code/Graphics_2D/GDevVideo/Gen.cp.htm

    //RN - according to the docs
    //gdPMap is a bitmap-type representation of the GDevice, and all
    //0x0000FFFF does is get the lower 16 bits of pixelSize.  However,
    //since pixelSize is only 16 bits (a short)...
    return ((*(*(m_priv->m_hndl))->gdPMap)->pixelSize) & 0x0000FFFF;
}

wxString wxDisplay::GetName() const
{
    // Macs don't name their displays...
    return wxEmptyString;
}

struct DMModeIteratorRec
{
    wxArrayVideoModes* pModes;
    const wxVideoMode* pMatchMode;
};

pascal void DMModeListIteratorProc ( void* pData,
                                     DMListIndexType nIndex,
                                     DMDisplayModeListEntryPtr pInfo)
{
    DMModeIteratorRec* pInfoData = (DMModeIteratorRec*) pData;

    //Note that in testing the refresh rate is always 0 on my ibook - RN
    int refresh = (int) Fix2Long(pInfo->displayModeResolutionInfo->csRefreshRate);

    for(unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock

        if (wxVideoMode((int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                        (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                        (int) pDBI->vpPixelSize,
                        refresh).Matches(*pInfoData->pMatchMode) )
        {
            pInfoData->pModes->Add(wxVideoMode((int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                                               (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                                               (int) pDBI->vpPixelSize,
                                               refresh));
        }
#undef pDBI
    }
}

struct DMModeInfoRec
{
    const wxVideoMode* pMode;
    VDSwitchInfoRec sMode;
    bool bMatched;
};

pascal void DMModeInfoProc ( void* pData,
                             DMListIndexType nIndex,
                             DMDisplayModeListEntryPtr pInfo)
{
    DMModeInfoRec* pInfoData = (DMModeInfoRec*) pData;
    Fixed refresh = Long2Fix(pInfoData->pMode->refresh);

    for(unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock
        if (pInfoData->pMode->w == (int&) pInfo->displayModeResolutionInfo->csHorizontalPixels &&
            pInfoData->pMode->h == (int&) pInfo->displayModeResolutionInfo->csVerticalLines &&
            pInfoData->pMode->bpp == (int) pDBI->vpPixelSize &&
            refresh == pInfo->displayModeResolutionInfo->csRefreshRate)
        {
            memcpy(&pInfoData->sMode, pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthSwitchInfo,
                   sizeof(VDSwitchInfoRec));
            pInfoData->sMode.csMode = pDBI->vpPixelSize;
            pInfoData->bMatched = true;
            break;
        }
#undef pDBI
    }
}

struct DMModeTransRec
{
    wxVideoMode Mode;
    const VDSwitchInfoRec* psMode;
    bool bMatched;
};

pascal void DMModeTransProc ( void* pData,
                              DMListIndexType nIndex,
                              DMDisplayModeListEntryPtr pInfo)
{
    DMModeTransRec* pInfoData = (DMModeTransRec*) pData;

    for(unsigned long i = 0; i < pInfo->displayModeDepthBlockInfo->depthBlockCount; ++i)
    {
#define pDBI pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthVPBlock
        if (pInfoData->psMode->csData == pInfo->displayModeDepthBlockInfo->depthVPBlock[i].depthSwitchInfo->csData)
        {
            pInfoData->Mode = wxVideoMode((int) pInfo->displayModeResolutionInfo->csHorizontalPixels,
                                          (int) pInfo->displayModeResolutionInfo->csVerticalLines,
                                          (int) pDBI->vpPixelSize,
                                          (int) Fix2Long(pInfo->displayModeResolutionInfo->csRefreshRate) );
            pInfoData->bMatched = true;
            break;
        }
#undef pDBI
    }
}

wxArrayVideoModes
    wxDisplay::GetModes(const wxVideoMode& mode) const
{

    wxArrayVideoModes Modes;

    unsigned long dwDMVer;
    Gestalt(gestaltDisplayMgrVers, (long*) &dwDMVer);

    //Check DM version (for backward compatibility only - 7.5.3+ use 2.0)
    if (dwDMVer >= 0x020000) //version 2?
    {

        DMListIndexType nNumModes;
        DMListType pModes;
        DMDisplayModeListIteratorUPP uppMLI;
        DisplayIDType nDisplayID;
        OSErr err;

        err = DMGetDisplayIDByGDevice(m_priv->m_hndl, &nDisplayID, false);
        wxASSERT(err == noErr);

        //Create a new list...
        err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
        wxASSERT_MSG(err == noErr, wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeListIteratorProc);
        wxASSERT(uppMLI);

        DMModeIteratorRec sModeInfo;
        sModeInfo.pModes = &Modes;
        sModeInfo.pMatchMode = &mode;
        for (DMListIndexType i = 0; i < nNumModes; ++i)
        {
            err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
            wxASSERT(err == noErr);
        }
        DisposeDMDisplayModeListIteratorUPP(uppMLI);

        err = DMDisposeList(pModes);
        wxASSERT(err == noErr);
    }
    else //DM 1.0, 1.2, 1.x
    {
        wxLogSysError(wxString::Format(wxT("Display Manager Version %u Not Supported!  Present? %s"),
                      (unsigned int) dwDMVer / 0x10000,
                      (dwDMVer & (1 << gestaltDisplayMgrPresent) ? wxT("Yes") : wxT("No"))  )
                     );
    }

    return Modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    unsigned long dwDMVer;
    wxVideoMode RetMode;

    Gestalt(gestaltDisplayMgrVers, (long*) &dwDMVer);
    //Check DM version (for backward compatibility only - 7.5.3+ use 2.0)
    if (dwDMVer >= 0x020000) //version 2?
    {
        VDSwitchInfoRec sMode; //Note - csMode member also contains the bit depth
        if (DMGetDisplayMode(m_priv->m_hndl, &sMode) == noErr)
        {
            DMListIndexType nNumModes;
            DMListType pModes;
            DMDisplayModeListIteratorUPP uppMLI;
            DisplayIDType nDisplayID;
            OSErr err;

            err = DMGetDisplayIDByGDevice(m_priv->m_hndl, &nDisplayID, false);
            wxASSERT(err == noErr);

            //Create a new list...
            err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
            wxASSERT_MSG(err == noErr, wxT("Could not create a new display mode list") );

            uppMLI = NewDMDisplayModeListIteratorUPP(DMModeTransProc);
            wxASSERT(uppMLI);

            DMModeTransRec sModeInfo;
            sModeInfo.bMatched = false;
            sModeInfo.psMode = &sMode;
            for (DMListIndexType i = 0; i < nNumModes; ++i)
            {
                err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
                wxASSERT(err == noErr);

                if ( sModeInfo.bMatched )
                {
                    RetMode = sModeInfo.Mode;
                    break;
                }
            }

            DisposeDMDisplayModeListIteratorUPP(uppMLI);

            err = DMDisposeList(pModes);
            wxASSERT(err == noErr);
        }
        else //Can't get current mode?
        {
            wxLogSysError(wxString::Format(wxT("Couldn't obtain current display mode!!!\ndwDMVer:%u"),
                                           (unsigned int) dwDMVer));
        }
    }
    else //DM ver 1
    {
        wxLogSysError(wxString::Format(wxT("Display Manager Version %u Not Supported!  Present? %s"),
                      (unsigned int) dwDMVer / 0x10000,
                      (dwDMVer & (1 << gestaltDisplayMgrPresent) ? wxT("Yes") : wxT("No")) )
                     );
    }

    return RetMode;
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    unsigned long dwDMVer;
    Gestalt(gestaltDisplayMgrVers, (long*)&dwDMVer);
    if (GetCount() == 1 || dwDMVer >= 0x020000)
    {
        if (mode == wxDefaultVideoMode)
        {
//#ifndef __DARWIN__
//            Handle hDisplayState;
//            if (DMBeginConfigureDisplays(&hDisplayState) != noErr)
//            {
//                wxLogSysError(wxT("Could not lock display for display mode changing!"));
//                return false;
//            }
//            wxASSERT( DMUseScreenPrefs(true, hDisplayState) == noErr);
//            DMEndConfigureDisplays(hDisplayState);
//            return true;
//#else
             //hmmmmm....
             return true;
//#endif
        }

        //0 & NULL for params 2 & 3 of DMSetVideoMode signal it to use defaults (current mode)
        //DM 2.0+ doesn't use params 2 & 3 of DMSetDisplayMode
        //so we have to use this icky structure
        VDSwitchInfoRec sMode;
        memset(&sMode, 0, sizeof(VDSwitchInfoRec) );

        DMListIndexType nNumModes;
        DMListType pModes;
        DMDisplayModeListIteratorUPP uppMLI;
        DisplayIDType nDisplayID;
        OSErr err;

        err = DMGetDisplayIDByGDevice(m_priv->m_hndl, &nDisplayID, false);
        wxASSERT(err == noErr);

        //Create a new list...
        err = DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes);
        wxASSERT_MSG(err == noErr, wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeInfoProc);
        wxASSERT(uppMLI);

        DMModeInfoRec sModeInfo;
        sModeInfo.bMatched = false;
        sModeInfo.pMode = &mode;
        unsigned int i;
        for(i = 0; i < nNumModes; ++i)
        {
            err = DMGetIndexedDisplayModeFromList(pModes, i, NULL, uppMLI, &sModeInfo);
            wxASSERT(err == noErr);

            if (sModeInfo.bMatched)
            {
                sMode = sModeInfo.sMode;
                break;
            }
        }
        if(i == nNumModes)
            return false;

        DisposeDMDisplayModeListIteratorUPP(uppMLI);

        err = DMDisposeList(pModes);
        wxASSERT(err == noErr);

        // For the really paranoid -
        //     unsigned long flags;
        //      Boolean bok;
        //     wxASSERT(noErr == DMCheckDisplayMode(m_priv->m_hndl, sMode.csData,
        //                                          sMode.csMode, &flags, NULL, &bok));
        //     wxASSERT(bok);

        Handle hDisplayState;
        if (DMBeginConfigureDisplays(&hDisplayState) != noErr)
        {
            wxLogSysError(wxT("Could not lock display for display mode changing!"));
            return false;
        }

        unsigned long dwBPP = (unsigned long) mode.bpp;
        if (DMSetDisplayMode(m_priv->m_hndl, sMode.csData,
                             (unsigned long*) &(dwBPP), NULL
                             //(unsigned long) &sMode
                             , hDisplayState
                            )  != noErr)
        {
            DMEndConfigureDisplays(hDisplayState);
            wxMessageBox(wxString::Format(wxT("Could not set the display mode")));
            return false;
        }
        DMEndConfigureDisplays(hDisplayState);
    }
    else  //DM 1.0, 1.2, 1.x
    {
        wxLogSysError(wxString::Format(wxT("Monitor gravitation not supported yet.  dwDMVer:%u"),
                      (unsigned int) dwDMVer));
        return false;
    }

    return true;
}

wxDisplay::~wxDisplay()
{
    if ( m_priv )
    {
        delete m_priv;
        m_priv = 0;
    }
}

#endif // !OSX

#endif // wxUSE_DISPLAY
