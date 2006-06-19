/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/display.cpp
// Purpose:     Mac implementation of wxDisplay class
// Author:      Brian Victor
// Modified by: Royce Mitchell III & Ryan Norton, Vadim Zeitlin
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

// For compilers that support precompilation, includes "wx.h".
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
    #include <Video.h>  //for VDSwitchInfoRec
    #include <FixMath.h>
#endif

#include "wx/display_impl.h"

// ----------------------------------------------------------------------------
// display implementation classes
// ----------------------------------------------------------------------------

class wxDisplayImplMac : public wxDisplayImpl
{
public:
    wxDisplayImplMac(size_t n, GDHandle hndl)
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

    virtual wxDisplayImpl *CreateDisplay(size_t n);
    virtual size_t GetCount();
    virtual int GetFromPoint(const wxPoint& pt);

protected:
    DECLARE_NO_COPY_CLASS(wxDisplayFactoryMac)
};

// ============================================================================
// wxDisplayFactoryMac implementation
// ============================================================================

size_t wxDisplayFactoryMac::GetCount()
{
    size_t num = 0;
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
    size_t num = 0;
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

wxDisplayImpl *wxDisplayFactoryMac::CreateDisplay(size_t n)
{
    size_t nOrig = n;

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

pascal void DMModeListIteratorProc (    void* pData,
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

pascal void DMModeInfoProc (    void* pData,
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

pascal void DMModeTransProc (    void* pData,
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

wxArrayVideoModes wxDisplayImplMac::GetModes(const wxVideoMode& mode) const
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

        wxASSERT(DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false) == noErr);
        //Create a new list...
        wxASSERT_MSG(DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes,                             &pModes) == noErr, wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeListIteratorProc);
        wxASSERT(uppMLI);

        DMModeIteratorRec sModeInfo;
        sModeInfo.pModes = &Modes;
        sModeInfo.pMatchMode = &mode;
        for (DMListIndexType i = 0; i < nNumModes; ++i)
        {
            wxASSERT(DMGetIndexedDisplayModeFromList(pModes, i, NULL,
                              uppMLI, &sModeInfo) == noErr);
        }
        DisposeDMDisplayModeListIteratorUPP(uppMLI);
        wxASSERT(DMDisposeList(pModes) == noErr);
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

wxVideoMode wxDisplayImplMac::GetCurrentMode() const
{
    unsigned long dwDMVer;
    wxVideoMode RetMode;

    Gestalt(gestaltDisplayMgrVers, (long*) &dwDMVer);
    //Check DM version (for backward compatibility only - 7.5.3+ use 2.0)
    if (dwDMVer >= 0x020000) //version 2?
    {
        VDSwitchInfoRec sMode; //Note - csMode member also contains the bit depth
        if (DMGetDisplayMode(m_hndl, &sMode) == noErr)
        {
            DMListIndexType nNumModes;
            DMListType pModes;
            DMDisplayModeListIteratorUPP uppMLI;
            DisplayIDType nDisplayID;

            wxASSERT(DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false) == noErr);
            //Create a new list...
            wxASSERT_MSG(DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes) == noErr,
                      wxT("Could not create a new display mode list") );

            uppMLI = NewDMDisplayModeListIteratorUPP(DMModeTransProc);
            wxASSERT(uppMLI);

            DMModeTransRec sModeInfo;
            sModeInfo.bMatched = false;
            sModeInfo.psMode = &sMode;
            for (DMListIndexType i = 0; i < nNumModes; ++i)
            {
                wxASSERT(DMGetIndexedDisplayModeFromList(pModes, i, NULL,
                                                uppMLI, &sModeInfo) == noErr);

                if ( sModeInfo.bMatched == true )
                {
                    RetMode = sModeInfo.Mode;
                    break;
                }
            }

            DisposeDMDisplayModeListIteratorUPP(uppMLI);
            wxASSERT(DMDisposeList(pModes) == noErr);
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

bool wxDisplayImplMac::ChangeMode(const wxVideoMode& mode)
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
//                {
//                wxLogSysError(wxT("Could not lock display for display mode changing!"));
//                return false;
//                }
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

        wxASSERT(DMGetDisplayIDByGDevice(m_hndl, &nDisplayID, false) == noErr);
        //Create a new list...
        wxASSERT_MSG(DMNewDisplayModeList(nDisplayID, NULL, NULL, &nNumModes, &pModes) == noErr,
                  wxT("Could not create a new display mode list") );

        uppMLI = NewDMDisplayModeListIteratorUPP(DMModeInfoProc);
        wxASSERT(uppMLI);

        DMModeInfoRec sModeInfo;
        sModeInfo.bMatched = false;
        sModeInfo.pMode = &mode;
        unsigned int i;
        for(i = 0; i < nNumModes; ++i)
        {
            wxASSERT(DMGetIndexedDisplayModeFromList(pModes, i, NULL,
                                               uppMLI, &sModeInfo) == noErr);
            if (sModeInfo.bMatched == true)
            {
                sMode = sModeInfo.sMode;
                break;
            }
        }
        if(i == nNumModes)
            return false;

        DisposeDMDisplayModeListIteratorUPP(uppMLI);
        wxASSERT(DMDisposeList(pModes) == noErr);

        // For the really paranoid -
        //         unsigned long flags;
        //      Boolean bok;
        //     wxASSERT(noErr == DMCheckDisplayMode(m_hndl, sMode.csData,
        //                                  sMode.csMode, &flags, NULL, &bok));
        //     wxASSERT(bok);

        Handle hDisplayState;
        if (DMBeginConfigureDisplays(&hDisplayState) != noErr)
        {
            wxLogSysError(wxT("Could not lock display for display mode changing!"));
            return false;
        }

        unsigned long dwBPP = (unsigned long) mode.bpp;
        if (DMSetDisplayMode(m_hndl, sMode.csData,
                            (unsigned long*) &(dwBPP), NULL
                           //(unsigned long) &sMode
                           , hDisplayState
                           )  != noErr)
        {
            DMEndConfigureDisplays(hDisplayState);
            wxLogError(wxT("Could not set the display mode"));
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

// ============================================================================
// wxDisplay::CreateFactory()
// ============================================================================

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryMac;
}

#endif // wxUSE_DISPLAY
