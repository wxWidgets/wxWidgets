/////////////////////////////////////////////////////////////////////////////
// Name:        msw/mediactrl.cpp
// Purpose:     Built-in Media Backends for Windows
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mediactrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//---------------------------------------------------------------------------
// MediaCtrl include
//---------------------------------------------------------------------------
#include "wx/mediactrl.h"

//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

//---------------------------------------------------------------------------
// WX Includes
//---------------------------------------------------------------------------
#include "wx/log.h"         //wxLogDebug
#include "wx/math.h"        //log10 & pow
#include "wx/msw/private.h" //user info and wndproc setting/getting

//---------------------------------------------------------------------------
// Externals (somewhere in src/msw/app.cpp and src/msw/window.cpp)
//---------------------------------------------------------------------------
extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance(void);
#ifdef __WXWINCE__
extern WXDLLIMPEXP_CORE       wxChar *wxCanvasClassName;
#else
extern WXDLLIMPEXP_CORE const wxChar *wxCanvasClassName;
#endif

LRESULT WXDLLIMPEXP_CORE APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxAMMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  COM includes
//---------------------------------------------------------------------------
#include "wx/msw/ole/oleutils.h" //wxBasicString, IID etc.
#include "wx/msw/ole/uuid.h" //IID etc..
#include <oleidl.h>
#include <olectl.h>
#include <exdisp.h>
#include <docobj.h>

//
//  These defines are from another ole header - but its not in the
//  latest sdk.  Also the ifndef DISPID_READYSTATE is here because at
//  least on my machine with the latest sdk olectl.h defines these 3
//
#ifndef DISPID_READYSTATE
    #define DISPID_READYSTATE                               -525
    #define DISPID_READYSTATECHANGE                         -609
    #define DISPID_AMBIENT_TRANSFERPRIORITY                 -728
#endif

#define DISPID_AMBIENT_OFFLINEIFNOTCONNECTED            -5501
#define DISPID_AMBIENT_SILENT                           -5502

#ifndef DISPID_AMBIENT_CODEPAGE
#   define DISPID_AMBIENT_CODEPAGE                         -725
#   define DISPID_AMBIENT_CHARSET                          -727
#endif

//---------------------------------------------------------------------------
//  COM compatability definitions
//---------------------------------------------------------------------------
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif
#ifndef STDMETHOD
#define STDMETHOD(funcname)  virtual HRESULT STDMETHODCALLTYPE funcname
#endif
#ifndef PURE
#define PURE = 0
#endif
#ifndef __RPC_FAR
#define __RPC_FAR FAR
#endif

//---------------------------------------------------------------------------
//  IIDS - used by CoCreateInstance and IUnknown::QueryInterface
//
//  [idl name]          [idl decription]
//  amcompat.idl        Microsoft Active Movie Control (Ver 2.0)
//  nscompat.idl        Microsoft NetShow Player (Ver 1.0)
//  msdxm.idl           Windows Media Player (Ver 1.0)
//  quartz.idl
//
//  First, when I say I "from XXX.idl", I mean I go into the COM Browser
//  ($Microsoft Visual Studio$/Common/Tools/OLEVIEW.EXE), open
//  "type libraries", open a specific type library (for quartz for example its
//  "ActiveMovie control type library (V1.0)"), save it as an .idl, compile the
//  idl using the midl compiler that comes with visual studio
//  ($Microsoft Visual Studio$/VC98/bin/midl.exe on VC6) with the /h argument
//  to make it generate stubs (a .h & .c file), then clean up the generated
//  interfaces I want with the STDMETHOD wrappers and then put them into
//  mediactrl.cpp.
//
//  According to the MSDN docs, IMediaPlayer requires Windows 98 SE
//  or greater.  NetShow is available on Windows 3.1 and I'm guessing
//  IActiveMovie is too.  IMediaPlayer is essentially the Windows Media
//  Player 6.4 SDK.
//
//  Some of these are not used but are kept here for future reference anyway
//---------------------------------------------------------------------------
const IID IID_IActiveMovie          = {0x05589FA2,0xC356,0x11CE,{0xBF,0x01,0x00,0xAA,0x00,0x55,0x59,0x5A}};
const IID IID_IActiveMovie2         = {0xB6CD6554,0xE9CB,0x11D0,{0x82,0x1F,0x00,0xA0,0xC9,0x1F,0x9C,0xA0}};
const IID IID_IActiveMovie3         = {0x265EC140,0xAE62,0x11D1,{0x85,0x00,0x00,0xA0,0xC9,0x1F,0x9C,0xA0}};

const IID IID_INSOPlay              = {0x2179C5D1,0xEBFF,0x11CF,{0xB6,0xFD,0x00,0xAA,0x00,0xB4,0xE2,0x20}};
const IID IID_INSPlay               = {0xE7C4BE80,0x7960,0x11D0,{0xB7,0x27,0x00,0xAA,0x00,0xB4,0xE2,0x20}};
const IID IID_INSPlay1              = {0x265EC141,0xAE62,0x11D1,{0x85,0x00,0x00,0xA0,0xC9,0x1F,0x9C,0xA0}};

const IID IID_IMediaPlayer          = {0x22D6F311,0xB0F6,0x11D0,{0x94,0xAB,0x00,0x80,0xC7,0x4C,0x7E,0x95}};
const IID IID_IMediaPlayer2         = {0x20D4F5E0,0x5475,0x11D2,{0x97,0x74,0x00,0x00,0xF8,0x08,0x55,0xE6}};

const CLSID CLSID_ActiveMovie       = {0x05589FA1,0xC356,0x11CE,{0xBF,0x01,0x00,0xAA,0x00,0x55,0x59,0x5A}};
const CLSID CLSID_MediaPlayer       = {0x22D6F312,0xB0F6,0x11D0,{0x94,0xAB,0x00,0x80,0xC7,0x4C,0x7E,0x95}};
const CLSID CLSID_NSPlay            = {0x2179C5D3,0xEBFF,0x11CF,{0xB6,0xFD,0x00,0xAA,0x00,0xB4,0xE2,0x20}};

const IID IID_IAMOpenProgress = {0x8E1C39A1, 0xDE53, 0x11CF,{0xAA, 0x63, 0x00, 0x80, 0xC7, 0x44, 0x52, 0x8D}};

// QUARTZ
const CLSID CLSID_FilgraphManager = {0xE436EBB3,0x524F,0x11CE,{0x9F,0x53,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaEvent = {0x56A868B6,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};

//??  QUARTZ Also?
const CLSID CLSID_VideoMixingRenderer9 ={0x51B4ABF3, 0x748F, 0x4E3B,{0xA2, 0x76, 0xC8, 0x28, 0x33, 0x0E, 0x92, 0x6A}};
const IID IID_IVMRWindowlessControl9 =  {0x8F537D09, 0xF85E, 0x4414,{0xB2, 0x3B, 0x50, 0x2E, 0x54, 0xC7, 0x99, 0x27}};
const IID IID_IFilterGraph =            {0x56A8689F, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};
const IID IID_IGraphBuilder =           {0x56A868A9, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};
const IID IID_IVMRFilterConfig9 =       {0x5A804648, 0x4F66, 0x4867,{0x9C, 0x43, 0x4F, 0x5C, 0x82, 0x2C, 0xF1, 0xB8}};
const IID IID_IBaseFilter =             {0x56A86895, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};

//---------------------------------------------------------------------------
//  QUARTZ COM INTERFACES (dumped from quartz.idl from MSVC COM Browser)
//---------------------------------------------------------------------------

struct IAMOpenProgress : public IUnknown
{
    STDMETHOD(QueryProgress)(LONGLONG *pllTotal, LONGLONG *pllCurrent) PURE;
    STDMETHOD(AbortOperation)(void) PURE;
};

struct IMediaEvent : public IDispatch
{
    STDMETHOD(GetEventHandle)(LONG_PTR *) PURE;
    STDMETHOD(GetEvent)(long *, LONG_PTR *, LONG_PTR *, long) PURE;
    STDMETHOD(WaitForCompletion)(long, long *) PURE;
    STDMETHOD(CancelDefaultHandling)(long) PURE;
    STDMETHOD(RestoreDefaultHandling)(long) PURE;
    STDMETHOD(FreeEventParams)(long, LONG_PTR, LONG_PTR) PURE;
};

//---------------------------------------------------------------------------
//  ACTIVEMOVIE COM INTERFACES (dumped from amcompat.idl from MSVC COM Browser)
//---------------------------------------------------------------------------

enum ReadyStateConstants
{
    amvUninitialized    = 0,
    amvLoading    = 1,
    amvInteractive    = 3,
    amvComplete    = 4
};

enum StateConstants
{
    amvNotLoaded    = -1,
    amvStopped    = 0,
    amvPaused    = 1,
    amvRunning    = 2
};

enum DisplayModeConstants
{
    amvTime    = 0,
    amvFrames    = 1
};

enum WindowSizeConstants
{
    amvOriginalSize    = 0,
    amvDoubleOriginalSize    = 1,
    amvOneSixteenthScreen    = 2,
    amvOneFourthScreen    = 3,
    amvOneHalfScreen    = 4
};

enum AppearanceConstants
{
    amvFlat    = 0,
    amv3D    = 1
};

enum BorderStyleConstants
{
    amvNone    = 0,
    amvFixedSingle    = 1
};

struct IActiveMovie : public IDispatch
{
    STDMETHOD(AboutBox)( void) PURE;
    STDMETHOD(Run)( void) PURE;
    STDMETHOD(Pause)( void) PURE;
    STDMETHOD(Stop)( void) PURE;
    STDMETHOD(get_ImageSourceWidth)(long __RPC_FAR *pWidth) PURE;
    STDMETHOD(get_ImageSourceHeight)(long __RPC_FAR *pHeight) PURE;
    STDMETHOD(get_Author)(BSTR __RPC_FAR *pbstrAuthor) PURE;
    STDMETHOD(get_Title)(BSTR __RPC_FAR *pbstrTitle) PURE;
    STDMETHOD(get_Copyright)(BSTR __RPC_FAR *pbstrCopyright) PURE;
    STDMETHOD(get_Description)(BSTR __RPC_FAR *pbstrDescription) PURE;
    STDMETHOD(get_Rating)(BSTR __RPC_FAR *pbstrRating) PURE;
    STDMETHOD(get_FileName)(BSTR __RPC_FAR *pbstrFileName) PURE;
    STDMETHOD(put_FileName)(BSTR pbstrFileName) PURE;
    STDMETHOD(get_Duration)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(get_CurrentPosition)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_CurrentPosition)(double pValue) PURE;
    STDMETHOD(get_PlayCount)(long __RPC_FAR *pPlayCount) PURE;
    STDMETHOD(put_PlayCount)(long pPlayCount) PURE;
    STDMETHOD(get_SelectionStart)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_SelectionStart)(double pValue) PURE;
    STDMETHOD(get_SelectionEnd)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_SelectionEnd)(double pValue) PURE;
    STDMETHOD(get_CurrentState)(StateConstants __RPC_FAR *pState) PURE;
    STDMETHOD(get_Rate)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_Rate)(double pValue) PURE;
    STDMETHOD(get_Volume)(long __RPC_FAR *pValue) PURE;
    STDMETHOD(put_Volume)(long pValue) PURE;
    STDMETHOD(get_Balance)(long __RPC_FAR *pValue) PURE;
    STDMETHOD(put_Balance)(long pValue) PURE;
    STDMETHOD(get_EnableContextMenu)(VARIANT_BOOL __RPC_FAR *pEnable) PURE;
    STDMETHOD(put_EnableContextMenu)(VARIANT_BOOL pEnable) PURE;
    STDMETHOD(get_ShowDisplay)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowDisplay)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowPositionControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowPositionControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowSelectionControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowSelectionControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowTracker)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowTracker)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_EnablePositionControls)(VARIANT_BOOL __RPC_FAR *Enable) PURE;
    STDMETHOD(put_EnablePositionControls)(VARIANT_BOOL Enable) PURE;
    STDMETHOD(get_EnableSelectionControls)(VARIANT_BOOL __RPC_FAR *Enable) PURE;
    STDMETHOD(put_EnableSelectionControls)(VARIANT_BOOL Enable) PURE;
    STDMETHOD(get_EnableTracker)(VARIANT_BOOL __RPC_FAR *Enable) PURE;
    STDMETHOD(put_EnableTracker)(VARIANT_BOOL Enable) PURE;
    STDMETHOD(get_AllowHideDisplay)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_AllowHideDisplay)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_AllowHideControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_AllowHideControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_DisplayMode)(DisplayModeConstants __RPC_FAR *pValue) PURE;
    STDMETHOD(put_DisplayMode)(DisplayModeConstants pValue) PURE;
    STDMETHOD(get_AllowChangeDisplayMode)(VARIANT_BOOL __RPC_FAR *fAllow) PURE;
    STDMETHOD(put_AllowChangeDisplayMode)(VARIANT_BOOL fAllow) PURE;
    STDMETHOD(get_FilterGraph)(IUnknown __RPC_FAR *__RPC_FAR *ppFilterGraph) PURE;
    STDMETHOD(put_FilterGraph)(IUnknown __RPC_FAR *ppFilterGraph) PURE;
    STDMETHOD(get_FilterGraphDispatch)(IDispatch __RPC_FAR *__RPC_FAR *pDispatch) PURE;
    STDMETHOD(get_DisplayForeColor)(unsigned long __RPC_FAR *ForeColor) PURE;
    STDMETHOD(put_DisplayForeColor)(unsigned long ForeColor) PURE;
    STDMETHOD(get_DisplayBackColor)(unsigned long __RPC_FAR *BackColor) PURE;
    STDMETHOD(put_DisplayBackColor)(unsigned long BackColor) PURE;
    STDMETHOD(get_MovieWindowSize)(WindowSizeConstants __RPC_FAR *WindowSize) PURE;
    STDMETHOD(put_MovieWindowSize)(WindowSizeConstants WindowSize) PURE;
    STDMETHOD(get_FullScreenMode)(VARIANT_BOOL __RPC_FAR *pEnable) PURE;
    STDMETHOD(put_FullScreenMode)(VARIANT_BOOL pEnable) PURE;
    STDMETHOD(get_AutoStart)(VARIANT_BOOL __RPC_FAR *pEnable) PURE;
    STDMETHOD(put_AutoStart)(VARIANT_BOOL pEnable) PURE;
    STDMETHOD(get_AutoRewind)(VARIANT_BOOL __RPC_FAR *pEnable) PURE;
    STDMETHOD(put_AutoRewind)(VARIANT_BOOL pEnable) PURE;
    STDMETHOD(get_hWnd)(long __RPC_FAR *hWnd) PURE;
    STDMETHOD(get_Appearance)(AppearanceConstants __RPC_FAR *pAppearance) PURE;
    STDMETHOD(put_Appearance)(AppearanceConstants pAppearance) PURE;
    STDMETHOD(get_BorderStyle)(BorderStyleConstants __RPC_FAR *pBorderStyle) PURE;
    STDMETHOD(put_BorderStyle)(BorderStyleConstants pBorderStyle) PURE;
    STDMETHOD(get_Enabled)(VARIANT_BOOL __RPC_FAR *pEnabled) PURE;
    STDMETHOD(put_Enabled)(VARIANT_BOOL pEnabled) PURE;
    STDMETHOD(get_Info)(long __RPC_FAR *ppInfo) PURE;
};



struct IActiveMovie2 : public IActiveMovie
{
    STDMETHOD(IsSoundCardEnabled)(VARIANT_BOOL __RPC_FAR *pbSoundCard) PURE;
    STDMETHOD(get_ReadyState)(ReadyStateConstants __RPC_FAR *pValue) PURE;
};

struct IActiveMovie3 : public IActiveMovie2
{
    STDMETHOD(get_MediaPlayer)(IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) PURE;
};


//---------------------------------------------------------------------------
//  MEDIAPLAYER COM INTERFACES (dumped from msdxm.idl from MSVC COM Browser)
//---------------------------------------------------------------------------

enum MPPlayStateConstants
{
    mpStopped = 0,
    mpPaused    = 1,
    mpPlaying    = 2,
    mpWaiting    = 3,
    mpScanForward    = 4,
    mpScanReverse    = 5,
    mpClosed    = 6
};

enum MPDisplaySizeConstants
{
    mpDefaultSize = 0,
    mpHalfSize    = 1,
    mpDoubleSize    = 2,
    mpFullScreen    = 3,
    mpFitToSize    = 4,
    mpOneSixteenthScreen    = 5,
    mpOneFourthScreen    = 6,
    mpOneHalfScreen    = 7
};

enum MPReadyStateConstants
{
    mpReadyStateUninitialized = 0,
    mpReadyStateLoading    = 1,
    mpReadyStateInteractive    = 3,
    mpReadyStateComplete    = 4
};

typedef unsigned long VB_OLE_COLOR;

enum MPDisplayModeConstants
{
    mpTime = 0,
    mpFrames    = 1
};

enum MPMoreInfoType
{
    mpShowURL = 0,
    mpClipURL    = 1,
    mpBannerURL    = 2
};

enum MPMediaInfoType
{
    mpShowFilename = 0,
    mpShowTitle    = 1,
    mpShowAuthor    = 2,
    mpShowCopyright    = 3,
    mpShowRating    = 4,
    mpShowDescription    = 5,
    mpShowLogoIcon    = 6,
    mpClipFilename    = 7,
    mpClipTitle    = 8,
    mpClipAuthor    = 9,
    mpClipCopyright    = 10,
    mpClipRating    = 11,
    mpClipDescription    = 12,
    mpClipLogoIcon    = 13,
    mpBannerImage    = 14,
    mpBannerMoreInfo    = 15,
    mpWatermark    = 16
};

enum DVDMenuIDConstants
{
    dvdMenu_Title    = 2,
    dvdMenu_Root    = 3,
    dvdMenu_Subpicture    = 4,
    dvdMenu_Audio    = 5,
    dvdMenu_Angle    = 6,
    dvdMenu_Chapter    = 7
};

enum MPShowDialogConstants
{
    mpShowDialogHelp = 0,
    mpShowDialogStatistics    = 1,
    mpShowDialogOptions    = 2,
    mpShowDialogContextMenu    = 3
};


struct IMediaPlayer : public IDispatch
{
    STDMETHOD(get_CurrentPosition)(double __RPC_FAR *pCurrentPosition) PURE;
    STDMETHOD(put_CurrentPosition)(double pCurrentPosition) PURE;
    STDMETHOD(get_Duration)(double __RPC_FAR *pDuration) PURE;
    STDMETHOD(get_ImageSourceWidth)(long __RPC_FAR *pWidth) PURE;
    STDMETHOD(get_ImageSourceHeight)(long __RPC_FAR *pHeight) PURE;
    STDMETHOD(get_MarkerCount)(long __RPC_FAR *pMarkerCount) PURE;
    STDMETHOD(get_CanScan)(VARIANT_BOOL __RPC_FAR *pCanScan) PURE;
    STDMETHOD(get_CanSeek)(VARIANT_BOOL __RPC_FAR *pCanSeek) PURE;
    STDMETHOD(get_CanSeekToMarkers)(VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers) PURE;
    STDMETHOD(get_CurrentMarker)(long __RPC_FAR *pCurrentMarker) PURE;
    STDMETHOD(put_CurrentMarker)(long pCurrentMarker) PURE;
    STDMETHOD(get_FileName)(BSTR __RPC_FAR *pbstrFileName) PURE;
    STDMETHOD(put_FileName)(BSTR pbstrFileName) PURE;
    STDMETHOD(get_SourceLink)(BSTR __RPC_FAR *pbstrSourceLink) PURE;
    STDMETHOD(get_CreationDate)(DATE __RPC_FAR *pCreationDate) PURE;
    STDMETHOD(get_ErrorCorrection)(BSTR __RPC_FAR *pbstrErrorCorrection) PURE;
    STDMETHOD(get_Bandwidth)(long __RPC_FAR *pBandwidth) PURE;
    STDMETHOD(get_SourceProtocol)(long __RPC_FAR *pSourceProtocol) PURE;
    STDMETHOD(get_ReceivedPackets)(long __RPC_FAR *pReceivedPackets) PURE;
    STDMETHOD(get_RecoveredPackets)(long __RPC_FAR *pRecoveredPackets) PURE;
    STDMETHOD(get_LostPackets)(long __RPC_FAR *pLostPackets) PURE;
    STDMETHOD(get_ReceptionQuality)(long __RPC_FAR *pReceptionQuality) PURE;
    STDMETHOD(get_BufferingCount)(long __RPC_FAR *pBufferingCount) PURE;
    STDMETHOD(get_IsBroadcast)(VARIANT_BOOL __RPC_FAR *pIsBroadcast) PURE;
    STDMETHOD(get_BufferingProgress)(long __RPC_FAR *pBufferingProgress) PURE;
    STDMETHOD(get_ChannelName)(BSTR __RPC_FAR *pbstrChannelName) PURE;
    STDMETHOD(get_ChannelDescription)(BSTR __RPC_FAR *pbstrChannelDescription) PURE;
    STDMETHOD(get_ChannelURL)(BSTR __RPC_FAR *pbstrChannelURL) PURE;
    STDMETHOD(get_ContactAddress)(BSTR __RPC_FAR *pbstrContactAddress) PURE;
    STDMETHOD(get_ContactPhone)(BSTR __RPC_FAR *pbstrContactPhone) PURE;
    STDMETHOD(get_ContactEmail)(BSTR __RPC_FAR *pbstrContactEmail) PURE;
    STDMETHOD(get_BufferingTime)(double __RPC_FAR *pBufferingTime) PURE;
    STDMETHOD(put_BufferingTime)(double pBufferingTime) PURE;
    STDMETHOD(get_AutoStart)(VARIANT_BOOL __RPC_FAR *pAutoStart) PURE;
    STDMETHOD(put_AutoStart)(VARIANT_BOOL pAutoStart) PURE;
    STDMETHOD(get_AutoRewind)(VARIANT_BOOL __RPC_FAR *pAutoRewind) PURE;
    STDMETHOD(put_AutoRewind)(VARIANT_BOOL pAutoRewind) PURE;
    STDMETHOD(get_Rate)(double __RPC_FAR *pRate) PURE;
    STDMETHOD(put_Rate)(double pRate) PURE;
    STDMETHOD(get_SendKeyboardEvents)(VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents) PURE;
    STDMETHOD(put_SendKeyboardEvents)(VARIANT_BOOL pSendKeyboardEvents) PURE;
    STDMETHOD(get_SendMouseClickEvents)(VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents) PURE;
    STDMETHOD(put_SendMouseClickEvents)(VARIANT_BOOL pSendMouseClickEvents) PURE;
    STDMETHOD(get_SendMouseMoveEvents)(VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents) PURE;
    STDMETHOD(put_SendMouseMoveEvents)(VARIANT_BOOL pSendMouseMoveEvents) PURE;
    STDMETHOD(get_PlayCount)(long __RPC_FAR *pPlayCount) PURE;
    STDMETHOD(put_PlayCount)(long pPlayCount) PURE;
    STDMETHOD(get_ClickToPlay)(VARIANT_BOOL __RPC_FAR *pClickToPlay) PURE;
    STDMETHOD(put_ClickToPlay)(VARIANT_BOOL pClickToPlay) PURE;
    STDMETHOD(get_AllowScan)(VARIANT_BOOL __RPC_FAR *pAllowScan) PURE;
    STDMETHOD(put_AllowScan)(VARIANT_BOOL pAllowScan) PURE;
    STDMETHOD(get_EnableContextMenu)(VARIANT_BOOL __RPC_FAR *pEnableContextMenu) PURE;
    STDMETHOD(put_EnableContextMenu)(VARIANT_BOOL pEnableContextMenu) PURE;
    STDMETHOD(get_CursorType)(long __RPC_FAR *pCursorType) PURE;
    STDMETHOD(put_CursorType)(long pCursorType) PURE;
    STDMETHOD(get_CodecCount)(long __RPC_FAR *pCodecCount) PURE;
    STDMETHOD(get_AllowChangeDisplaySize)(VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize) PURE;
    STDMETHOD(put_AllowChangeDisplaySize)( VARIANT_BOOL pAllowChangeDisplaySize) PURE;
    STDMETHOD(get_IsDurationValid)(VARIANT_BOOL __RPC_FAR *pIsDurationValid) PURE;
    STDMETHOD(get_OpenState)(long __RPC_FAR *pOpenState) PURE;
    STDMETHOD(get_SendOpenStateChangeEvents)(VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents) PURE;
    STDMETHOD(put_SendOpenStateChangeEvents)(VARIANT_BOOL pSendOpenStateChangeEvents) PURE;
    STDMETHOD(get_SendWarningEvents)( VARIANT_BOOL __RPC_FAR *pSendWarningEvents) PURE;
    STDMETHOD(put_SendWarningEvents)(VARIANT_BOOL pSendWarningEvents) PURE;
    STDMETHOD(get_SendErrorEvents)(VARIANT_BOOL __RPC_FAR *pSendErrorEvents) PURE;
    STDMETHOD(put_SendErrorEvents)(VARIANT_BOOL pSendErrorEvents) PURE;
    STDMETHOD(get_PlayState)(MPPlayStateConstants __RPC_FAR *pPlayState) PURE;
    STDMETHOD(get_SendPlayStateChangeEvents)(VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents) PURE;
    STDMETHOD(put_SendPlayStateChangeEvents)(VARIANT_BOOL pSendPlayStateChangeEvents) PURE;
    STDMETHOD(get_DisplaySize)(MPDisplaySizeConstants __RPC_FAR *pDisplaySize) PURE;
    STDMETHOD(put_DisplaySize)(MPDisplaySizeConstants pDisplaySize) PURE;
    STDMETHOD(get_InvokeURLs)(VARIANT_BOOL __RPC_FAR *pInvokeURLs) PURE;
    STDMETHOD(put_InvokeURLs)(VARIANT_BOOL pInvokeURLs) PURE;
    STDMETHOD(get_BaseURL)(BSTR __RPC_FAR *pbstrBaseURL) PURE;
    STDMETHOD(put_BaseURL)(BSTR pbstrBaseURL) PURE;
    STDMETHOD(get_DefaultFrame)(BSTR __RPC_FAR *pbstrDefaultFrame) PURE;
    STDMETHOD(put_DefaultFrame)(BSTR pbstrDefaultFrame) PURE;
    STDMETHOD(get_HasError)(VARIANT_BOOL __RPC_FAR *pHasError) PURE;
    STDMETHOD(get_ErrorDescription)(BSTR __RPC_FAR *pbstrErrorDescription) PURE;
    STDMETHOD(get_ErrorCode)(long __RPC_FAR *pErrorCode) PURE;
    STDMETHOD(get_AnimationAtStart)(VARIANT_BOOL __RPC_FAR *pAnimationAtStart) PURE;
    STDMETHOD(put_AnimationAtStart)(VARIANT_BOOL pAnimationAtStart) PURE;
    STDMETHOD(get_TransparentAtStart)( VARIANT_BOOL __RPC_FAR *pTransparentAtStart) PURE;
    STDMETHOD(put_TransparentAtStart)(VARIANT_BOOL pTransparentAtStart) PURE;
    STDMETHOD(get_Volume)(long __RPC_FAR *pVolume) PURE;
    STDMETHOD(put_Volume)(long pVolume) PURE;
    STDMETHOD(get_Balance)(long __RPC_FAR *pBalance) PURE;
    STDMETHOD(put_Balance)(long pBalance) PURE;
    STDMETHOD(get_ReadyState)(MPReadyStateConstants __RPC_FAR *pValue) PURE;
    STDMETHOD(get_SelectionStart)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_SelectionStart)(double pValue) PURE;
    STDMETHOD(get_SelectionEnd)(double __RPC_FAR *pValue) PURE;
    STDMETHOD(put_SelectionEnd)(double pValue) PURE;
    STDMETHOD(get_ShowDisplay)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowDisplay)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowPositionControls)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowPositionControls)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_ShowTracker)(VARIANT_BOOL __RPC_FAR *Show) PURE;
    STDMETHOD(put_ShowTracker)(VARIANT_BOOL Show) PURE;
    STDMETHOD(get_EnablePositionControls)(VARIANT_BOOL __RPC_FAR *Enable) PURE;
    STDMETHOD(put_EnablePositionControls)(VARIANT_BOOL Enable) PURE;
    STDMETHOD(get_EnableTracker)(VARIANT_BOOL __RPC_FAR *Enable) PURE;
    STDMETHOD(put_EnableTracker)(VARIANT_BOOL Enable) PURE;
    STDMETHOD(get_Enabled)(VARIANT_BOOL __RPC_FAR *pEnabled) PURE;
    STDMETHOD(put_Enabled)(VARIANT_BOOL pEnabled) PURE;
    STDMETHOD(get_DisplayForeColor)(VB_OLE_COLOR __RPC_FAR *ForeColor) PURE;
    STDMETHOD(put_DisplayForeColor)(VB_OLE_COLOR ForeColor) PURE;
    STDMETHOD(get_DisplayBackColor)(VB_OLE_COLOR __RPC_FAR *BackColor) PURE;
    STDMETHOD(put_DisplayBackColor)(VB_OLE_COLOR BackColor) PURE;
    STDMETHOD(get_DisplayMode)(MPDisplayModeConstants __RPC_FAR *pValue) PURE;
    STDMETHOD(put_DisplayMode)(MPDisplayModeConstants pValue) PURE;
    STDMETHOD(get_VideoBorder3D)(VARIANT_BOOL __RPC_FAR *pVideoBorderWidth) PURE;
    STDMETHOD(put_VideoBorder3D)(VARIANT_BOOL pVideoBorderWidth) PURE;
    STDMETHOD(get_VideoBorderWidth)(long __RPC_FAR *pVideoBorderWidth) PURE;
    STDMETHOD(put_VideoBorderWidth)(long pVideoBorderWidth) PURE;
    STDMETHOD(get_VideoBorderColor)(VB_OLE_COLOR __RPC_FAR *pVideoBorderWidth) PURE;
    STDMETHOD(put_VideoBorderColor)(VB_OLE_COLOR pVideoBorderWidth) PURE;
    STDMETHOD(get_ShowGotoBar)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_ShowGotoBar)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(get_ShowStatusBar)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_ShowStatusBar)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(get_ShowCaptioning)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_ShowCaptioning)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(get_ShowAudioControls)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_ShowAudioControls)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(get_CaptioningID)( BSTR __RPC_FAR *pstrText) PURE;
    STDMETHOD(put_CaptioningID)(BSTR pstrText) PURE;
    STDMETHOD(get_Mute)(VARIANT_BOOL __RPC_FAR *vbool) PURE;
    STDMETHOD(put_Mute)(VARIANT_BOOL vbool) PURE;
    STDMETHOD(get_CanPreview)(VARIANT_BOOL __RPC_FAR *pCanPreview) PURE;
    STDMETHOD(get_PreviewMode)(VARIANT_BOOL __RPC_FAR *pPreviewMode) PURE;
    STDMETHOD(put_PreviewMode)(VARIANT_BOOL pPreviewMode) PURE;
    STDMETHOD(get_HasMultipleItems)(VARIANT_BOOL __RPC_FAR *pHasMuliItems) PURE;
    STDMETHOD(get_Language)(long __RPC_FAR *pLanguage) PURE;
    STDMETHOD(put_Language)(long pLanguage) PURE;
    STDMETHOD(get_AudioStream)(long __RPC_FAR *pStream) PURE;
    STDMETHOD(put_AudioStream)(long pStream) PURE;
    STDMETHOD(get_SAMIStyle)(BSTR __RPC_FAR *pbstrStyle) PURE;
    STDMETHOD(put_SAMIStyle)(BSTR pbstrStyle) PURE;
    STDMETHOD(get_SAMILang)(BSTR __RPC_FAR *pbstrLang) PURE;
    STDMETHOD(put_SAMILang)(BSTR pbstrLang) PURE;
    STDMETHOD(get_SAMIFileName)(BSTR __RPC_FAR *pbstrFileName) PURE;
    STDMETHOD(put_SAMIFileName)(BSTR pbstrFileName) PURE;
    STDMETHOD(get_StreamCount)( long __RPC_FAR *pStreamCount) PURE;
    STDMETHOD(get_ClientId)(BSTR __RPC_FAR *pbstrClientId) PURE;
    STDMETHOD(get_ConnectionSpeed)(long __RPC_FAR *plConnectionSpeed) PURE;
    STDMETHOD(get_AutoSize)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_AutoSize)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(get_EnableFullScreenControls)(VARIANT_BOOL __RPC_FAR *pbVal) PURE;
    STDMETHOD(put_EnableFullScreenControls)(VARIANT_BOOL pbVal) PURE;
    STDMETHOD(get_ActiveMovie)(IDispatch __RPC_FAR *__RPC_FAR *ppdispatch) PURE;
    STDMETHOD(get_NSPlay)(IDispatch __RPC_FAR *__RPC_FAR *ppdispatch) PURE;
    STDMETHOD(get_WindowlessVideo)(VARIANT_BOOL __RPC_FAR *pbool) PURE;
    STDMETHOD(put_WindowlessVideo)(VARIANT_BOOL pbool) PURE;
    STDMETHOD(Play)(void) PURE;
    STDMETHOD(Stop)(void) PURE;
    STDMETHOD(Pause)(void) PURE;
    STDMETHOD(GetMarkerTime)(long MarkerNum,
                             double __RPC_FAR *pMarkerTime) PURE;
    STDMETHOD(GetMarkerName)(long MarkerNum,
                             BSTR __RPC_FAR *pbstrMarkerName) PURE;
    STDMETHOD(AboutBox)(void) PURE;
    STDMETHOD(GetCodecInstalled)(long CodecNum,
                              VARIANT_BOOL __RPC_FAR *pCodecInstalled) PURE;
    STDMETHOD(GetCodecDescription)(long CodecNum,
                                 BSTR __RPC_FAR *pbstrCodecDescription) PURE;
    STDMETHOD(GetCodecURL)(long CodecNum,
                           BSTR __RPC_FAR *pbstrCodecURL) PURE;
    STDMETHOD(GetMoreInfoURL)(MPMoreInfoType MoreInfoType,
                              BSTR __RPC_FAR *pbstrMoreInfoURL) PURE;
    STDMETHOD(GetMediaInfoString)(MPMediaInfoType MediaInfoType,
                                  BSTR __RPC_FAR *pbstrMediaInfo) PURE;
    STDMETHOD(Cancel)(void) PURE;
    STDMETHOD(Open)(BSTR bstrFileName) PURE;
    STDMETHOD(IsSoundCardEnabled)(VARIANT_BOOL __RPC_FAR *pbSoundCard) PURE;
    STDMETHOD(Next)(void) PURE;
    STDMETHOD(Previous)(void) PURE;
    STDMETHOD(StreamSelect)(long StreamNum) PURE;
    STDMETHOD(FastForward)(void) PURE;
    STDMETHOD(FastReverse)(void) PURE;
    STDMETHOD(GetStreamName)(long StreamNum,
                             BSTR __RPC_FAR *pbstrStreamName) PURE;
    STDMETHOD(GetStreamGroup)(long StreamNum,
                              long __RPC_FAR *pStreamGroup) PURE;
    STDMETHOD(GetStreamSelected)(long StreamNum, VARIANT_BOOL __RPC_FAR *pStreamSelected) PURE;
};

struct IMediaPlayer2 : public IMediaPlayer
{
    STDMETHOD(get_DVD)(struct IMediaPlayerDvd __RPC_FAR *__RPC_FAR *ppdispatch) PURE;
    STDMETHOD(GetMediaParameter)(long EntryNum, BSTR bstrParameterName, BSTR __RPC_FAR *pbstrParameterValue) PURE;
    STDMETHOD(GetMediaParameterName(long EntryNum, long Index, BSTR __RPC_FAR *pbstrParameterName) PURE;
    STDMETHOD(get_EntryCount)(long __RPC_FAR *pNumberEntries) PURE;
    STDMETHOD(GetCurrentEntry)(long __RPC_FAR *pEntryNumber) PURE;
    STDMETHOD(SetCurrentEntry)(long EntryNumber) PURE;
    STDMETHOD(ShowDialog)(MPShowDialogConstants mpDialogIndex) PURE;
};

//---------------------------------------------------------------------------
//  NETSHOW COM INTERFACES (dumped from nscompat.idl from MSVC COM Browser)
//---------------------------------------------------------------------------

struct INSOPlay : public IDispatch
{
    STDMETHOD(get_ImageSourceWidth)(long __RPC_FAR *pWidth) PURE;
    STDMETHOD(get_ImageSourceHeight)(long __RPC_FAR *pHeight) PURE;
    STDMETHOD(get_Duration)(double __RPC_FAR *pDuration) PURE;
    STDMETHOD(get_Author)(BSTR __RPC_FAR *pbstrAuthor) PURE;
    STDMETHOD(get_Copyright)(BSTR __RPC_FAR *pbstrCopyright) PURE;
    STDMETHOD(get_Description)(BSTR __RPC_FAR *pbstrDescription) PURE;
    STDMETHOD(get_Rating)(BSTR __RPC_FAR *pbstrRating) PURE;
    STDMETHOD(get_Title)(BSTR __RPC_FAR *pbstrTitle) PURE;
    STDMETHOD(get_SourceLink)(BSTR __RPC_FAR *pbstrSourceLink) PURE;
    STDMETHOD(get_MarkerCount)(long __RPC_FAR *pMarkerCount) PURE;
    STDMETHOD(get_CanScan)(VARIANT_BOOL __RPC_FAR *pCanScan) PURE;
    STDMETHOD(get_CanSeek)(VARIANT_BOOL __RPC_FAR *pCanSeek) PURE;
    STDMETHOD(get_CanSeekToMarkers)(VARIANT_BOOL __RPC_FAR *pCanSeekToMarkers) PURE;
    STDMETHOD(get_CreationDate)(DATE __RPC_FAR *pCreationDate) PURE;
    STDMETHOD(get_Bandwidth)(long __RPC_FAR *pBandwidth) PURE;
    STDMETHOD(get_ErrorCorrection)(BSTR __RPC_FAR *pbstrErrorCorrection) PURE;
    STDMETHOD(get_AutoStart)(VARIANT_BOOL __RPC_FAR *pAutoStart) PURE;
    STDMETHOD(put_AutoStart)(VARIANT_BOOL pAutoStart) PURE;
    STDMETHOD(get_AutoRewind)(VARIANT_BOOL __RPC_FAR *pAutoRewind) PURE;
    STDMETHOD(put_AutoRewind)(VARIANT_BOOL pAutoRewind) PURE;
    STDMETHOD(get_AllowChangeControlType)(VARIANT_BOOL __RPC_FAR *pAllowChangeControlType) PURE;
    STDMETHOD(put_AllowChangeControlType)(VARIANT_BOOL pAllowChangeControlType) PURE;
    STDMETHOD(get_InvokeURLs)(VARIANT_BOOL __RPC_FAR *pInvokeURLs) PURE;
    STDMETHOD(put_InvokeURLs)(VARIANT_BOOL pInvokeURLs) PURE;
    STDMETHOD(get_EnableContextMenu)(VARIANT_BOOL __RPC_FAR *pEnableContextMenu) PURE;
    STDMETHOD(put_EnableContextMenu)(VARIANT_BOOL pEnableContextMenu) PURE;
    STDMETHOD(get_TransparentAtStart)(VARIANT_BOOL __RPC_FAR *pTransparentAtStart) PURE;
    STDMETHOD(put_TransparentAtStart)(VARIANT_BOOL pTransparentAtStart) PURE;
    STDMETHOD(get_TransparentOnStop)(VARIANT_BOOL __RPC_FAR *pTransparentOnStop) PURE;
    STDMETHOD(put_TransparentOnStop)(VARIANT_BOOL pTransparentOnStop) PURE;
    STDMETHOD(get_ClickToPlay)(VARIANT_BOOL __RPC_FAR *pClickToPlay) PURE;
    STDMETHOD(put_ClickToPlay)(VARIANT_BOOL pClickToPlay) PURE;
    STDMETHOD(get_FileName)(BSTR __RPC_FAR *pbstrFileName) PURE;
    STDMETHOD(put_FileName)(BSTR pbstrFileName) PURE;
    STDMETHOD(get_CurrentPosition)(double __RPC_FAR *pCurrentPosition) PURE;
    STDMETHOD(put_CurrentPosition)(double pCurrentPosition) PURE;
    STDMETHOD(get_Rate)(double __RPC_FAR *pRate) PURE;
    STDMETHOD(put_Rate)(double pRate) PURE;
    STDMETHOD(get_CurrentMarker)(long __RPC_FAR *pCurrentMarker) PURE;
    STDMETHOD(put_CurrentMarker)(long pCurrentMarker) PURE;
    STDMETHOD(get_PlayCount)(long __RPC_FAR *pPlayCount) PURE;
    STDMETHOD(put_PlayCount)(long pPlayCount) PURE;
    STDMETHOD(get_CurrentState)(long __RPC_FAR *pCurrentState) PURE;
    STDMETHOD(get_DisplaySize)(long __RPC_FAR *pDisplaySize) PURE;
    STDMETHOD(put_DisplaySize)(long pDisplaySize) PURE;
    STDMETHOD(get_MainWindow)(long __RPC_FAR *pMainWindow) PURE;
    STDMETHOD(get_ControlType)(long __RPC_FAR *pControlType) PURE;
    STDMETHOD(put_ControlType)(long pControlType) PURE;
    STDMETHOD(get_AllowScan)(VARIANT_BOOL __RPC_FAR *pAllowScan) PURE;
    STDMETHOD(put_AllowScan)(VARIANT_BOOL pAllowScan) PURE;
    STDMETHOD(get_SendKeyboardEvents)(VARIANT_BOOL __RPC_FAR *pSendKeyboardEvents) PURE;
    STDMETHOD(put_SendKeyboardEvents)(VARIANT_BOOL pSendKeyboardEvents) PURE;
    STDMETHOD(get_SendMouseClickEvents)(VARIANT_BOOL __RPC_FAR *pSendMouseClickEvents) PURE;
    STDMETHOD(put_SendMouseClickEvents)(VARIANT_BOOL pSendMouseClickEvents) PURE;
    STDMETHOD(get_SendMouseMoveEvents)(VARIANT_BOOL __RPC_FAR *pSendMouseMoveEvents) PURE;
    STDMETHOD(put_SendMouseMoveEvents)(VARIANT_BOOL pSendMouseMoveEvents) PURE;
    STDMETHOD(get_SendStateChangeEvents)(VARIANT_BOOL __RPC_FAR *pSendStateChangeEvents) PURE;
    STDMETHOD(put_SendStateChangeEvents)(VARIANT_BOOL pSendStateChangeEvents) PURE;
    STDMETHOD(get_ReceivedPackets)(long __RPC_FAR *pReceivedPackets) PURE;
    STDMETHOD(get_RecoveredPackets)(long __RPC_FAR *pRecoveredPackets) PURE;
    STDMETHOD(get_LostPackets)(long __RPC_FAR *pLostPackets) PURE;
    STDMETHOD(get_ReceptionQuality)(long __RPC_FAR *pReceptionQuality) PURE;
    STDMETHOD(get_BufferingCount)(long __RPC_FAR *pBufferingCount) PURE;
    STDMETHOD(get_CursorType)(long __RPC_FAR *pCursorType) PURE;
    STDMETHOD(put_CursorType)(long pCursorType) PURE;
    STDMETHOD(get_AnimationAtStart)(VARIANT_BOOL __RPC_FAR *pAnimationAtStart) PURE;
    STDMETHOD(put_AnimationAtStart)(VARIANT_BOOL pAnimationAtStart) PURE;
    STDMETHOD(get_AnimationOnStop)(VARIANT_BOOL __RPC_FAR *pAnimationOnStop) PURE;
    STDMETHOD(put_AnimationOnStop)(VARIANT_BOOL pAnimationOnStop) PURE;
    STDMETHOD(Play)(void) PURE;
    STDMETHOD(Pause)(void) PURE;
    STDMETHOD(Stop)(void) PURE;
    STDMETHOD(GetMarkerTime)(long MarkerNum, double __RPC_FAR *pMarkerTime) PURE;
    STDMETHOD(GetMarkerName)(long MarkerNum, BSTR __RPC_FAR *pbstrMarkerName) PURE;
};

struct INSPlay : public INSOPlay
{
    STDMETHOD(get_ChannelName)(BSTR __RPC_FAR *pbstrChannelName) PURE;
    STDMETHOD(get_ChannelDescription)(BSTR __RPC_FAR *pbstrChannelDescription) PURE;
    STDMETHOD(get_ChannelURL)(BSTR __RPC_FAR *pbstrChannelURL) PURE;
    STDMETHOD(get_ContactAddress)(BSTR __RPC_FAR *pbstrContactAddress) PURE;
    STDMETHOD(get_ContactPhone)(BSTR __RPC_FAR *pbstrContactPhone) PURE;
    STDMETHOD(get_ContactEmail)(BSTR __RPC_FAR *pbstrContactEmail) PURE;
    STDMETHOD(get_AllowChangeDisplaySize)(VARIANT_BOOL __RPC_FAR *pAllowChangeDisplaySize) PURE;
    STDMETHOD(put_AllowChangeDisplaySize)(VARIANT_BOOL pAllowChangeDisplaySize) PURE;
    STDMETHOD(get_CodecCount)(long __RPC_FAR *pCodecCount) PURE;
    STDMETHOD(get_IsBroadcast)(VARIANT_BOOL __RPC_FAR *pIsBroadcast) PURE;
    STDMETHOD(get_IsDurationValid)(VARIANT_BOOL __RPC_FAR *pIsDurationValid) PURE;
    STDMETHOD(get_SourceProtocol)(long __RPC_FAR *pSourceProtocol) PURE;
    STDMETHOD(get_OpenState)(long __RPC_FAR *pOpenState) PURE;
    STDMETHOD(get_SendOpenStateChangeEvents)(VARIANT_BOOL __RPC_FAR *pSendOpenStateChangeEvents) PURE;
    STDMETHOD(put_SendOpenStateChangeEvents)(VARIANT_BOOL pSendOpenStateChangeEvents) PURE;
    STDMETHOD(get_SendWarningEvents)(VARIANT_BOOL __RPC_FAR *pSendWarningEvents) PURE;
    STDMETHOD(put_SendWarningEvents)(VARIANT_BOOL pSendWarningEvents) PURE;
    STDMETHOD(get_SendErrorEvents)(VARIANT_BOOL __RPC_FAR *pSendErrorEvents) PURE;
    STDMETHOD(put_SendErrorEvents)(VARIANT_BOOL pSendErrorEvents) PURE;
    STDMETHOD(get_HasError)(VARIANT_BOOL __RPC_FAR *pHasError) PURE;
    STDMETHOD(get_ErrorDescription)(BSTR __RPC_FAR *pbstrErrorDescription) PURE;
    STDMETHOD(get_ErrorCode)(long __RPC_FAR *pErrorCode) PURE;
    STDMETHOD(get_PlayState)(long __RPC_FAR *pPlayState) PURE;
    STDMETHOD(get_SendPlayStateChangeEvents)(VARIANT_BOOL __RPC_FAR *pSendPlayStateChangeEvents) PURE;
    STDMETHOD(put_SendPlayStateChangeEvents)(VARIANT_BOOL pSendPlayStateChangeEvents) PURE;
    STDMETHOD(get_BufferingTime)(double __RPC_FAR *pBufferingTime) PURE;
    STDMETHOD(put_BufferingTime)(double pBufferingTime) PURE;
    STDMETHOD(get_UseFixedUDPPort)(VARIANT_BOOL __RPC_FAR *pUseFixedUDPPort) PURE;
    STDMETHOD(put_UseFixedUDPPort)(VARIANT_BOOL pUseFixedUDPPort) PURE;
    STDMETHOD(get_FixedUDPPort)(long __RPC_FAR *pFixedUDPPort) PURE;
    STDMETHOD(put_FixedUDPPort)(long pFixedUDPPort) PURE;
    STDMETHOD(get_UseHTTPProxy)(VARIANT_BOOL __RPC_FAR *pUseHTTPProxy) PURE;
    STDMETHOD(put_UseHTTPProxy)(VARIANT_BOOL pUseHTTPProxy) PURE;
    STDMETHOD(get_EnableAutoProxy)(VARIANT_BOOL __RPC_FAR *pEnableAutoProxy) PURE;
    STDMETHOD(put_EnableAutoProxy)(VARIANT_BOOL pEnableAutoProxy) PURE;
    STDMETHOD(get_HTTPProxyHost)(BSTR __RPC_FAR *pbstrHTTPProxyHost) PURE;
    STDMETHOD(put_HTTPProxyHost)(BSTR pbstrHTTPProxyHost) PURE;
    STDMETHOD(get_HTTPProxyPort)(long __RPC_FAR *pHTTPProxyPort) PURE;
    STDMETHOD(put_HTTPProxyPort)(long pHTTPProxyPort) PURE;
    STDMETHOD(get_EnableMulticast)(VARIANT_BOOL __RPC_FAR *pEnableMulticast) PURE;
    STDMETHOD(put_EnableMulticast)(VARIANT_BOOL pEnableMulticast) PURE;
    STDMETHOD(get_EnableUDP)(VARIANT_BOOL __RPC_FAR *pEnableUDP) PURE;
    STDMETHOD(put_EnableUDP)(VARIANT_BOOL pEnableUDP) PURE;
    STDMETHOD(get_EnableTCP)(VARIANT_BOOL __RPC_FAR *pEnableTCP) PURE;
    STDMETHOD(put_EnableTCP)(VARIANT_BOOL pEnableTCP) PURE;
    STDMETHOD(get_EnableHTTP)(VARIANT_BOOL __RPC_FAR *pEnableHTTP) PURE;
    STDMETHOD(put_EnableHTTP)(VARIANT_BOOL pEnableHTTP) PURE;
    STDMETHOD(get_BufferingProgress)(long __RPC_FAR *pBufferingProgress) PURE;
    STDMETHOD(get_BaseURL)(BSTR __RPC_FAR *pbstrBaseURL) PURE;
    STDMETHOD(put_BaseURL)(BSTR pbstrBaseURL) PURE;
    STDMETHOD(get_DefaultFrame)(BSTR __RPC_FAR *pbstrDefaultFrame) PURE;
    STDMETHOD(put_DefaultFrame)(BSTR pbstrDefaultFrame) PURE;
    STDMETHOD(AboutBox))(void) PURE;
    STDMETHOD(Cancel)(void) PURE;
    STDMETHOD(GetCodecInstalled)(long CodecNum, VARIANT_BOOL __RPC_FAR *pCodecInstalled) PURE;
    STDMETHOD(GetCodecDescription)(long CodecNum, BSTR __RPC_FAR *pbstrCodecDescription) PURE;
    STDMETHOD(GetCodecURL)(long CodecNum, BSTR __RPC_FAR *pbstrCodecURL) PURE;
    STDMETHOD(Open)(BSTR bstrFileName) PURE;
};


struct INSPlay1 : public INSPlay
{
    STDMETHOD(get_MediaPlayer)(IDispatch __RPC_FAR *__RPC_FAR *ppdispatch) PURE;
};

//---------------------------------------------------------------------------
// MISC COM INTERFACES
//---------------------------------------------------------------------------
typedef enum _FilterState
{
    State_Stopped,
    State_Paused,
    State_Running
} FILTER_STATE;
typedef enum _PinDirection {
    PINDIR_INPUT,
    PINDIR_OUTPUT
} PIN_DIRECTION;

typedef struct _FilterInfo {
    WCHAR        achName[128];
    struct IFilterGraph *pGraph;
} FILTER_INFO;

typedef struct _PinInfo {
    struct IBaseFilter *pFilter;
    PIN_DIRECTION dir;
    WCHAR achName[128];
} PIN_INFO;

struct IBaseFilter;
struct IPin;
struct IEnumFilters;
typedef struct  _MediaType {
    GUID      majortype;
    GUID      subtype;
    BOOL      bFixedSizeSamples;
    BOOL      bTemporalCompression;
    ULONG     lSampleSize;
    GUID      formattype;
    IUnknown  *pUnk;
    ULONG     cbFormat;
    BYTE *pbFormat;
} AM_MEDIA_TYPE;

struct IFilterGraph : public IUnknown
{
    STDMETHOD(AddFilter)(IBaseFilter *, LPCWSTR) PURE;
    STDMETHOD(RemoveFilter)(IBaseFilter *) PURE;
    STDMETHOD(EnumFilters)(IEnumFilters **) PURE;
    STDMETHOD(FindFilterByName)(LPCWSTR, IBaseFilter **) PURE;
    STDMETHOD(ConnectDirect)(IPin *, IPin *, const AM_MEDIA_TYPE *) PURE;
    STDMETHOD(Reconnect)(IPin *) PURE;
    STDMETHOD(Disconnect)(IPin *) PURE;
    STDMETHOD(SetDefaultSyncSource)() PURE;
};

struct IGraphBuilder : public IFilterGraph
{
    STDMETHOD(Connect)(IPin *, IPin *) PURE;
    STDMETHOD(Render)(IPin *) PURE;
    STDMETHOD(RenderFile)(LPCWSTR, LPCWSTR) PURE;
    STDMETHOD(AddSourceFilter)(LPCWSTR, LPCWSTR, IBaseFilter **) PURE;
    STDMETHOD(SetLogFile)(DWORD_PTR) PURE;
    STDMETHOD(Abort)() PURE;
    STDMETHOD(ShouldOperationContinue)() PURE;
};

struct IReferenceClock;
struct IEnumPins;
#define REFERENCE_TIME LONGLONG
struct IMediaFilter : public IPersist
{
    STDMETHOD(Stop)( void) PURE;
    STDMETHOD(Pause)( void) PURE;
    STDMETHOD(Run)(REFERENCE_TIME tStart) PURE;
    STDMETHOD(GetState)(DWORD dwMilliSecsTimeout,
                       FILTER_STATE *State) PURE;
    STDMETHOD(SetSyncSource)(IReferenceClock *pClock) PURE;
    STDMETHOD(GetSyncSource)(IReferenceClock **pClock) PURE;
};

struct IBaseFilter : public IMediaFilter
{
    STDMETHOD(EnumPins)(IEnumPins **ppEnum) PURE;
    STDMETHOD(FindPin)(LPCWSTR Id, IPin **ppPin) PURE;
    STDMETHOD(QueryFilterInfo)(FILTER_INFO *pInfo) PURE;
    STDMETHOD(JoinFilterGraph)(IFilterGraph *pGraph, LPCWSTR pName) PURE;
    STDMETHOD(QueryVendorInfo)(LPWSTR *pVendorInfo) PURE;
};

//---------------------------------------------------------------------------
//
//  wxActiveX (Ryan Norton's version :))
//  wxActiveX is (C) 2003 Lindsay Mathieson
//
//---------------------------------------------------------------------------
#define WX_DECLARE_AUTOOLE(wxAutoOleInterface, I) \
class wxAutoOleInterface \
{   \
    protected: \
    I *m_interface; \
\
    public: \
    explicit wxAutoOleInterface(I *pInterface = NULL) : m_interface(pInterface) {} \
    wxAutoOleInterface(REFIID riid, IUnknown *pUnk) : m_interface(NULL) \
    {   QueryInterface(riid, pUnk); } \
    wxAutoOleInterface(REFIID riid, IDispatch *pDispatch) : m_interface(NULL) \
    {   QueryInterface(riid, pDispatch); } \
    wxAutoOleInterface(REFCLSID clsid, REFIID riid) : m_interface(NULL)\
    {   CreateInstance(clsid, riid); }\
    wxAutoOleInterface(const wxAutoOleInterface& ti) : m_interface(NULL)\
    {   operator = (ti); }\
\
    wxAutoOleInterface& operator = (const wxAutoOleInterface& ti)\
    {\
        if (ti.m_interface)\
            ti.m_interface->AddRef();\
        Free();\
        m_interface = ti.m_interface;\
        return *this;\
    }\
\
    wxAutoOleInterface& operator = (I *&ti)\
    {\
        Free();\
        m_interface = ti;\
        return *this;\
    }\
\
    ~wxAutoOleInterface() {   Free();   }\
\
    inline void Free()\
    {\
        if (m_interface)\
            m_interface->Release();\
        m_interface = NULL;\
    }\
\
    HRESULT QueryInterface(REFIID riid, IUnknown *pUnk)\
    {\
        Free();\
        wxASSERT(pUnk != NULL);\
        return pUnk->QueryInterface(riid, (void **) &m_interface);\
    }\
\
    HRESULT CreateInstance(REFCLSID clsid, REFIID riid)\
    {\
        Free();\
        return CoCreateInstance(clsid, NULL, CLSCTX_ALL, riid, (void **) &m_interface);\
    }\
\
    inline operator I *() const {return m_interface;}\
    inline I* operator ->() {return m_interface;}\
    inline I** GetRef()    {return &m_interface;}\
    inline bool Ok() const    {return m_interface != NULL;}\
};

WX_DECLARE_AUTOOLE(wxAutoIDispatch, IDispatch)
WX_DECLARE_AUTOOLE(wxAutoIOleClientSite, IOleClientSite)
WX_DECLARE_AUTOOLE(wxAutoIUnknown, IUnknown)
WX_DECLARE_AUTOOLE(wxAutoIOleObject, IOleObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceObject, IOleInPlaceObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceActiveObject, IOleInPlaceActiveObject)
WX_DECLARE_AUTOOLE(wxAutoIOleDocumentView, IOleDocumentView)
WX_DECLARE_AUTOOLE(wxAutoIViewObject, IViewObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceSite, IOleInPlaceSite)
WX_DECLARE_AUTOOLE(wxAutoIOleDocument, IOleDocument)
WX_DECLARE_AUTOOLE(wxAutoIPersistStreamInit, IPersistStreamInit)
WX_DECLARE_AUTOOLE(wxAutoIAdviseSink, IAdviseSink)

class wxActiveX : public wxWindow
{
public:
    wxActiveX(wxWindow * parent, REFIID iid, IUnknown* pUnk);
    virtual ~wxActiveX();

    void OnSize(wxSizeEvent&);
    void OnPaint(wxPaintEvent&);
    void OnSetFocus(wxFocusEvent&);
    void OnKillFocus(wxFocusEvent&);

protected:
    friend class FrameSite;

    wxAutoIDispatch            m_Dispatch;
    wxAutoIOleClientSite      m_clientSite;
    wxAutoIUnknown         m_ActiveX;
    wxAutoIOleObject            m_oleObject;
    wxAutoIOleInPlaceObject    m_oleInPlaceObject;
    wxAutoIOleInPlaceActiveObject m_oleInPlaceActiveObject;
    wxAutoIOleDocumentView    m_docView;
    wxAutoIViewObject            m_viewObject;
    HWND m_oleObjectHWND;
    bool m_bAmbientUserMode;
    DWORD m_docAdviseCookie;
    wxWindow* m_realparent;

    void CreateActiveX(REFIID, IUnknown*);
};

#define DECLARE_OLE_UNKNOWN(cls)\
    private:\
    class TAutoInitInt\
    {\
        public:\
        LONG l;\
        TAutoInitInt() : l(0) {}\
    };\
    TAutoInitInt refCount, lockCount;\
    static void _GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc);\
    public:\
    LONG GetRefCount();\
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);\
    ULONG STDMETHODCALLTYPE AddRef();\
    ULONG STDMETHODCALLTYPE Release();\
    ULONG STDMETHODCALLTYPE AddLock();\
    ULONG STDMETHODCALLTYPE ReleaseLock()

#define DEFINE_OLE_TABLE(cls)\
    LONG cls::GetRefCount() {return refCount.l;}\
    HRESULT STDMETHODCALLTYPE cls::QueryInterface(REFIID iid, void ** ppvObject)\
    {\
        if (! ppvObject)\
        {\
            return E_FAIL;\
        };\
        const char *desc = NULL;\
        cls::_GetInterface(this, iid, ppvObject, desc);\
        if (! *ppvObject)\
        {\
            return E_NOINTERFACE;\
        };\
        ((IUnknown * )(*ppvObject))->AddRef();\
        return S_OK;\
    };\
    ULONG STDMETHODCALLTYPE cls::AddRef()\
    {\
        InterlockedIncrement(&refCount.l);\
        return refCount.l;\
    };\
    ULONG STDMETHODCALLTYPE cls::Release()\
    {\
        if (refCount.l > 0)\
        {\
            InterlockedDecrement(&refCount.l);\
            if (refCount.l == 0)\
            {\
                delete this;\
                return 0;\
            };\
            return refCount.l;\
        }\
        else\
            return 0;\
    }\
    ULONG STDMETHODCALLTYPE cls::AddLock()\
    {\
        InterlockedIncrement(&lockCount.l);\
        return lockCount.l;\
    };\
    ULONG STDMETHODCALLTYPE cls::ReleaseLock()\
    {\
        if (lockCount.l > 0)\
        {\
            InterlockedDecrement(&lockCount.l);\
            return lockCount.l;\
        }\
        else\
            return 0;\
    }\
    DEFINE_OLE_BASE(cls)

#define DEFINE_OLE_BASE(cls)\
    void cls::_GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc)\
    {\
        *_interface = NULL;\
        desc = NULL;

#define OLE_INTERFACE(_iid, _type)\
    if (IsEqualIID(iid, _iid))\
    {\
        *_interface = (IUnknown *) (_type *) self;\
        desc = # _iid;\
        return;\
    }

#define OLE_IINTERFACE(_face) OLE_INTERFACE(IID_##_face, _face)

#define OLE_INTERFACE_CUSTOM(func)\
    if (func(self, iid, _interface, desc))\
    {\
        return;\
    }

#define END_OLE_TABLE\
    }


class FrameSite :
    public IOleClientSite,
    public IOleInPlaceSiteEx,
    public IOleInPlaceFrame,
    public IOleItemContainer,
    public IDispatch,
    public IOleCommandTarget,
    public IOleDocumentSite,
    public IAdviseSink,
    public IOleControlSite
{
private:
    DECLARE_OLE_UNKNOWN(FrameSite);

public:
    FrameSite(wxWindow * win, wxActiveX * win2)
    {
        m_window = win2;
        m_bSupportsWindowlessActivation = true;
        m_bInPlaceLocked = false;
        m_bUIActive = false;
        m_bInPlaceActive = false;
        m_bWindowless = false;

        m_nAmbientLocale = 0;
        m_clrAmbientForeColor = ::GetSysColor(COLOR_WINDOWTEXT);
        m_clrAmbientBackColor = ::GetSysColor(COLOR_WINDOW);
        m_bAmbientShowHatching = true;
        m_bAmbientShowGrabHandles = true;
        m_bAmbientAppearance = true;

        m_hDCBuffer = NULL;
        m_hWndParent = (HWND)win->GetHWND();
    }
    virtual ~FrameSite(){}
    //***************************IDispatch*****************************
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, OLECHAR ** ,
                                            unsigned int , LCID ,
                                            DISPID * )
    {   return E_NOTIMPL;   }
    STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo **)
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int *)
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID,
                            WORD wFlags, DISPPARAMS *,
                            VARIANT * pVarResult, EXCEPINFO *,
                            unsigned int *)
    {
        if (!(wFlags & DISPATCH_PROPERTYGET))
            return S_OK;

        if (pVarResult == NULL)
            return E_INVALIDARG;

        //The most common case is boolean, use as an initial type
        V_VT(pVarResult) = VT_BOOL;

        switch (dispIdMember)
        {
            case DISPID_AMBIENT_MESSAGEREFLECT:
                V_BOOL(pVarResult)= FALSE;
                return S_OK;

            case DISPID_AMBIENT_DISPLAYASDEFAULT:
                V_BOOL(pVarResult)= TRUE;
                return S_OK;

            case DISPID_AMBIENT_OFFLINEIFNOTCONNECTED:
                V_BOOL(pVarResult) = TRUE;
                return S_OK;

            case DISPID_AMBIENT_SILENT:
                V_BOOL(pVarResult)= TRUE;
                return S_OK;

            case DISPID_AMBIENT_APPEARANCE:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientAppearance;
                break;

            case DISPID_AMBIENT_FORECOLOR:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_clrAmbientForeColor;
                break;

            case DISPID_AMBIENT_BACKCOLOR:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_clrAmbientBackColor;
                break;

            case DISPID_AMBIENT_LOCALEID:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_nAmbientLocale;
                break;

            case DISPID_AMBIENT_USERMODE:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_window->m_bAmbientUserMode;
                break;

            case DISPID_AMBIENT_SHOWGRABHANDLES:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientShowGrabHandles;
                break;

            case DISPID_AMBIENT_SHOWHATCHING:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientShowHatching;
                break;

            default:
                return DISP_E_MEMBERNOTFOUND;
        }

        return S_OK;
    }

    //**************************IOleWindow***************************
    HRESULT STDMETHODCALLTYPE GetWindow(HWND * phwnd)
    {
        if (phwnd == NULL)
            return E_INVALIDARG;
        (*phwnd) = m_hWndParent;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL)
    {return S_OK;}
    //**************************IOleInPlaceUIWindow*****************
    HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder)
    {
        if (lprectBorder == NULL)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
    {
        if (pborderwidths == NULL)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS)
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE SetActiveObject(
        IOleInPlaceActiveObject *pActiveObject, LPCOLESTR)
    {
        if (pActiveObject)
            pActiveObject->AddRef();

        m_window->m_oleInPlaceActiveObject = pActiveObject;
        return S_OK;
    }

    //********************IOleInPlaceFrame************************

    STDMETHOD(InsertMenus)(HMENU, LPOLEMENUGROUPWIDTHS){return S_OK;}
    STDMETHOD(SetMenu)(HMENU, HOLEMENU, HWND){  return S_OK;}
    STDMETHOD(RemoveMenus)(HMENU){return S_OK;}
    STDMETHOD(SetStatusText)(LPCOLESTR){ return S_OK;}
    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL){return S_OK;}
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD)
    {
        // TODO: send an event with this id
        if (m_window->m_oleInPlaceActiveObject.Ok())
            m_window->m_oleInPlaceActiveObject->TranslateAccelerator(lpmsg);
        return S_FALSE;
    }

    //*******************IOleInPlaceSite**************************
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate(){return S_OK;}
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate()
    {   m_bInPlaceActive = true;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnUIActivate()
    {   m_bUIActive = true;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame,
                                        IOleInPlaceUIWindow **ppDoc,
                                        LPRECT lprcPosRect,
                                        LPRECT lprcClipRect,
                                        LPOLEINPLACEFRAMEINFO lpFrameInfo)
    {
        if (ppFrame == NULL || ppDoc == NULL || lprcPosRect == NULL ||
            lprcClipRect == NULL || lpFrameInfo == NULL)
        {
            if (ppFrame != NULL)
                (*ppFrame) = NULL;
            if (ppDoc != NULL)
                (*ppDoc) = NULL;
            return E_INVALIDARG;
        }

        HRESULT hr = QueryInterface(IID_IOleInPlaceFrame, (void **) ppFrame);
        if (! SUCCEEDED(hr))
        {
            return E_UNEXPECTED;
        };

        hr = QueryInterface(IID_IOleInPlaceUIWindow, (void **) ppDoc);
        if (! SUCCEEDED(hr))
        {
            (*ppFrame)->Release();
            *ppFrame = NULL;
            return E_UNEXPECTED;
        };

        RECT rect;
        ::GetClientRect(m_hWndParent, &rect);
        if (lprcPosRect)
        {
            lprcPosRect->left = lprcPosRect->top = 0;
            lprcPosRect->right = rect.right;
            lprcPosRect->bottom = rect.bottom;
        };
        if (lprcClipRect)
        {
            lprcClipRect->left = lprcClipRect->top = 0;
            lprcClipRect->right = rect.right;
            lprcClipRect->bottom = rect.bottom;
        };

        memset(lpFrameInfo, 0, sizeof(OLEINPLACEFRAMEINFO));
        lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
        lpFrameInfo->hwndFrame = m_hWndParent;

        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Scroll(SIZE){return S_OK;}
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL)
    {   m_bUIActive = false;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate()
    {   m_bInPlaceActive = false;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE DiscardUndoState(){return S_OK;}
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo(){return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect)
    {
        if (m_window->m_oleInPlaceObject.Ok() && lprcPosRect)
        {
            m_window->m_oleInPlaceObject->SetObjectRects(
                lprcPosRect, lprcPosRect);
        }
        return S_OK;
    }
    //*************************IOleInPlaceSiteEx***********************
    HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD)
    {
        OleLockRunning(m_window->m_ActiveX, TRUE, FALSE);
        if (pfNoRedraw)
            (*pfNoRedraw) = FALSE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL)
    {
        OleLockRunning(m_window->m_ActiveX, FALSE, FALSE);
        return S_OK;
    }
    STDMETHOD(RequestUIActivate)(){ return S_OK;}
    //*************************IOleClientSite**************************
    HRESULT STDMETHODCALLTYPE SaveObject(){return S_OK;}
    const char *OleGetMonikerToStr(DWORD dwAssign)
    {
        switch (dwAssign)
        {
        case OLEGETMONIKER_ONLYIFTHERE  : return "OLEGETMONIKER_ONLYIFTHERE";
        case OLEGETMONIKER_FORCEASSIGN  : return "OLEGETMONIKER_FORCEASSIGN";
        case OLEGETMONIKER_UNASSIGN     : return "OLEGETMONIKER_UNASSIGN";
        case OLEGETMONIKER_TEMPFORUSER  : return "OLEGETMONIKER_TEMPFORUSER";
        default                         : return "Bad Enum";
        };
    };

    const char *OleGetWhicMonikerStr(DWORD dwWhichMoniker)
    {
        switch(dwWhichMoniker)
        {
        case OLEWHICHMK_CONTAINER   : return "OLEWHICHMK_CONTAINER";
        case OLEWHICHMK_OBJREL      : return "OLEWHICHMK_OBJREL";
        case OLEWHICHMK_OBJFULL     : return "OLEWHICHMK_OBJFULL";
        default                     : return "Bad Enum";
        };
    };
    STDMETHOD(GetMoniker)(DWORD, DWORD, IMoniker **){return E_FAIL;}
    HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER * ppContainer)
    {
        if (ppContainer == NULL)
            return E_INVALIDARG;
        HRESULT hr = QueryInterface(
            IID_IOleContainer, (void**)(ppContainer));
        wxASSERT(SUCCEEDED(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE ShowObject()
    {
        if (m_window->m_oleObjectHWND)
            ::ShowWindow(m_window->m_oleObjectHWND, SW_SHOW);
        return S_OK;
    }
    STDMETHOD(OnShowWindow)(BOOL){return S_OK;}
    STDMETHOD(RequestNewObjectLayout)(){return E_NOTIMPL;}
    //********************IParseDisplayName***************************
    HRESULT STDMETHODCALLTYPE ParseDisplayName(
        IBindCtx *, LPOLESTR, ULONG *, IMoniker **){return E_NOTIMPL;}
    //********************IOleContainer*******************************
    STDMETHOD(EnumObjects)(DWORD, IEnumUnknown **){return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockContainer(BOOL){return S_OK;}
    //********************IOleItemContainer***************************
    HRESULT STDMETHODCALLTYPE
    #ifdef _UNICODE
    GetObjectW
    #else
    GetObjectA
    #endif
    (LPOLESTR pszItem, DWORD, IBindCtx *, REFIID, void ** ppvObject)
    {
        if (pszItem == NULL || ppvObject == NULL)
            return E_INVALIDARG;
        *ppvObject = NULL;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE GetObjectStorage(
        LPOLESTR pszItem, IBindCtx * , REFIID, void ** ppvStorage)
    {
        if (pszItem == NULL || ppvStorage == NULL)
            return E_INVALIDARG;
        *ppvStorage = NULL;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE IsRunning(LPOLESTR pszItem)
    {
        if (pszItem == NULL)
            return E_INVALIDARG;
        return MK_E_NOOBJECT;
    }
    //***********************IOleControlSite*****************************
    HRESULT STDMETHODCALLTYPE OnControlInfoChanged()
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE LockInPlaceActive(BOOL fLock)
    {
        m_bInPlaceLocked = (fLock) ? true : false;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetExtendedControl(IDispatch **)
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE TransformCoords(
        POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD)
    {
        if (pPtlHimetric == NULL || pPtfContainer == NULL)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, DWORD)
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE OnFocus(BOOL){return S_OK;}
    HRESULT STDMETHODCALLTYPE ShowPropertyFrame(){return E_NOTIMPL;}
    //**************************IOleCommandTarget***********************
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID *, ULONG cCmds,
                                OLECMD prgCmds[], OLECMDTEXT *)
    {
        if (prgCmds == NULL) return E_INVALIDARG;
        for (ULONG nCmd = 0; nCmd < cCmds; nCmd++)
        {
            // unsupported by default
            prgCmds[nCmd].cmdf = 0;
        }
        return OLECMDERR_E_UNKNOWNGROUP;
    }

    HRESULT STDMETHODCALLTYPE Exec(const GUID *, DWORD,
                            DWORD, VARIANTARG *, VARIANTARG *)
    {return OLECMDERR_E_NOTSUPPORTED;}

    //**********************IAdviseSink************************************
    void STDMETHODCALLTYPE OnDataChange(FORMATETC *, STGMEDIUM *) {}
    void STDMETHODCALLTYPE OnViewChange(DWORD, LONG) {}
    void STDMETHODCALLTYPE OnRename(IMoniker *){}
    void STDMETHODCALLTYPE OnSave(){}
    void STDMETHODCALLTYPE OnClose(){}

    //**********************IOleDocumentSite***************************
    HRESULT STDMETHODCALLTYPE ActivateMe(
        IOleDocumentView __RPC_FAR *pViewToActivate)
    {
        wxAutoIOleInPlaceSite inPlaceSite(
            IID_IOleInPlaceSite, (IDispatch *) this);
        if (!inPlaceSite.Ok())
            return E_FAIL;

        if (pViewToActivate)
        {
            m_window->m_docView = pViewToActivate;
            m_window->m_docView->SetInPlaceSite(inPlaceSite);
        }
        else
        {
            wxAutoIOleDocument oleDoc(
                IID_IOleDocument, m_window->m_oleObject);
            if (! oleDoc.Ok())
                return E_FAIL;

            HRESULT hr = oleDoc->CreateView(inPlaceSite, NULL,
                                    0, m_window->m_docView.GetRef());
            if (hr != S_OK)
                return E_FAIL;

            m_window->m_docView->SetInPlaceSite(inPlaceSite);
        };

        m_window->m_docView->UIActivate(TRUE);
        return S_OK;
    };


protected:
    wxActiveX * m_window;

    HDC m_hDCBuffer;
    HWND m_hWndParent;

    bool m_bSupportsWindowlessActivation;
    bool m_bInPlaceLocked;
    bool m_bInPlaceActive;
    bool m_bUIActive;
    bool m_bWindowless;

    LCID m_nAmbientLocale;
    COLORREF m_clrAmbientForeColor;
    COLORREF m_clrAmbientBackColor;
    bool m_bAmbientShowHatching;
    bool m_bAmbientShowGrabHandles;
    bool m_bAmbientAppearance;
};

DEFINE_OLE_TABLE(FrameSite)
    OLE_INTERFACE(IID_IUnknown, IOleClientSite)
    OLE_IINTERFACE(IOleClientSite)
    OLE_INTERFACE(IID_IOleWindow, IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSiteEx)
    OLE_IINTERFACE(IOleInPlaceUIWindow)
    OLE_IINTERFACE(IOleInPlaceFrame)
    OLE_IINTERFACE(IParseDisplayName)
    OLE_IINTERFACE(IOleContainer)
    OLE_IINTERFACE(IOleItemContainer)
    OLE_IINTERFACE(IDispatch)
    OLE_IINTERFACE(IOleCommandTarget)
    OLE_IINTERFACE(IOleDocumentSite)
    OLE_IINTERFACE(IAdviseSink)
    OLE_IINTERFACE(IOleControlSite)
END_OLE_TABLE;


wxActiveX::wxActiveX(wxWindow * parent, REFIID iid, IUnknown* pUnk)
    : m_realparent(parent)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    CreateActiveX(iid, pUnk);
}

wxActiveX::~wxActiveX()
{
    // disconnect connection points
    if (m_oleInPlaceObject.Ok())
    {
        m_oleInPlaceObject->InPlaceDeactivate();
        m_oleInPlaceObject->UIDeactivate();
    }

    if (m_oleObject.Ok())
    {
        if (m_docAdviseCookie != 0)
            m_oleObject->Unadvise(m_docAdviseCookie);

        m_oleObject->DoVerb(
            OLEIVERB_HIDE, NULL, m_clientSite, 0, (HWND) GetHWND(), NULL);
        m_oleObject->Close(OLECLOSE_NOSAVE);
        m_oleObject->SetClientSite(NULL);
    }
}

void wxActiveX::CreateActiveX(REFIID iid, IUnknown* pUnk)
{
    HRESULT hret;
    hret = m_ActiveX.QueryInterface(iid, pUnk);
    wxASSERT(SUCCEEDED(hret));

    // FrameSite
    FrameSite *frame = new FrameSite(m_realparent, this);
    // oleClientSite
    hret = m_clientSite.QueryInterface(
        IID_IOleClientSite, (IDispatch *) frame);
    wxASSERT(SUCCEEDED(hret));
    // adviseSink
    wxAutoIAdviseSink adviseSink(IID_IAdviseSink, (IDispatch *) frame);
    wxASSERT(adviseSink.Ok());

    // Get Dispatch interface
    hret = m_Dispatch.QueryInterface(IID_IDispatch, m_ActiveX);

    // Get IOleObject interface
    hret = m_oleObject.QueryInterface(IID_IOleObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // get IViewObject Interface
    hret = m_viewObject.QueryInterface(IID_IViewObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // document advise
    m_docAdviseCookie = 0;
    hret = m_oleObject->Advise(adviseSink, &m_docAdviseCookie);
    m_oleObject->SetHostNames(L"wxActiveXContainer", NULL);
    OleSetContainedObject(m_oleObject, TRUE);
    OleRun(m_oleObject);


    // Get IOleInPlaceObject interface
    hret = m_oleInPlaceObject.QueryInterface(
        IID_IOleInPlaceObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // status
    DWORD dwMiscStatus;
    m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    wxASSERT(SUCCEEDED(hret));

    // set client site first ?
    if (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
        m_oleObject->SetClientSite(m_clientSite);


    // stream init
    wxAutoIPersistStreamInit
        pPersistStreamInit(IID_IPersistStreamInit, m_oleObject);

    if (pPersistStreamInit.Ok())
    {
        hret = pPersistStreamInit->InitNew();
    }

    if (! (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
        m_oleObject->SetClientSite(m_clientSite);


    RECT posRect;
    ::GetClientRect((HWND)m_realparent->GetHWND(), &posRect);

    m_oleObjectHWND = 0;

    if (m_oleInPlaceObject.Ok())
    {
        hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
        if (SUCCEEDED(hret))
            ::SetActiveWindow(m_oleObjectHWND);
    }


    if (! (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME))
    {
        if (posRect.right > 0 && posRect.bottom > 0 &&
            m_oleInPlaceObject.Ok())
                m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

        hret = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL,
            m_clientSite, 0, (HWND)m_realparent->GetHWND(), &posRect);
        hret = m_oleObject->DoVerb(OLEIVERB_SHOW, 0, m_clientSite, 0,
            (HWND)m_realparent->GetHWND(), &posRect);
    }

    if (! m_oleObjectHWND && m_oleInPlaceObject.Ok())
    {
        hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
    }

    if (m_oleObjectHWND)
    {
        ::SetActiveWindow(m_oleObjectHWND);
        ::ShowWindow(m_oleObjectHWND, SW_SHOW);

        this->AssociateHandle(m_oleObjectHWND);
        this->Reparent(m_realparent);

        wxWindow* pWnd = m_realparent;
        int id = m_realparent->GetId();

        pWnd->Connect(id, wxEVT_SIZE,
            wxSizeEventHandler(wxActiveX::OnSize), 0, this);
        pWnd->Connect(id, wxEVT_SET_FOCUS,
            wxFocusEventHandler(wxActiveX::OnSetFocus), 0, this);
        pWnd->Connect(id, wxEVT_KILL_FOCUS,
            wxFocusEventHandler(wxActiveX::OnKillFocus), 0, this);
    }
}

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))

static void PixelsToHimetric(SIZEL &sz)
{
    static int logX = 0;
    static int logY = 0;

    if (logY == 0)
    {
        // initaliase
        HDC dc = GetDC(NULL);
        logX = GetDeviceCaps(dc, LOGPIXELSX);
        logY = GetDeviceCaps(dc, LOGPIXELSY);
        ReleaseDC(NULL, dc);
    };

#define HIMETRIC_INCH   2540
#define CONVERT(x, logpixels)   MulDiv(HIMETRIC_INCH, (x), (logpixels))

    sz.cx = CONVERT(sz.cx, logX);
    sz.cy = CONVERT(sz.cy, logY);

#undef CONVERT
#undef HIMETRIC_INCH
}


void wxActiveX::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetParent()->GetClientSize(&w, &h);

    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    if (w <= 0 && h <= 0)
        return;

    // extents are in HIMETRIC units
    if (m_oleObject.Ok())
    {
        SIZEL sz = {w, h};
        PixelsToHimetric(sz);

        SIZEL sz2;

        m_oleObject->GetExtent(DVASPECT_CONTENT, &sz2);
        if (sz2.cx !=  sz.cx || sz.cy != sz2.cy)
            m_oleObject->SetExtent(DVASPECT_CONTENT, &sz);
    };

    if (m_oleInPlaceObject.Ok())
        m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

    event.Skip();
}

void wxActiveX::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    // Draw only when control is windowless or deactivated
    if (m_viewObject)
    {
        dc.BeginDrawing();
        int w, h;
        GetParent()->GetSize(&w, &h);
        RECT posRect;
        posRect.left = 0;
        posRect.top = 0;
        posRect.right = w;
        posRect.bottom = h;

        ::RedrawWindow(m_oleObjectHWND, NULL, NULL, RDW_INTERNALPAINT);
        RECTL *prcBounds = (RECTL *) &posRect;
        m_viewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL,
            (HDC)dc.GetHDC(), prcBounds, NULL, NULL, 0);

        dc.EndDrawing();
    }

//  We've got this one I think
//    event.Skip();
}

void wxActiveX::OnSetFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(TRUE);

    event.Skip();
}

void wxActiveX::OnKillFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(FALSE);

    event.Skip();
}

//###########################################################################
//
//
//  wxAMMediaBackend
//
//
//###########################################################################

typedef BOOL (WINAPI* LPAMGETERRORTEXT)(HRESULT, wxChar *, DWORD);

//cludgy workaround for wx events.  slots would be nice :)
class WXDLLIMPEXP_MEDIA wxAMMediaEvtHandler : public wxEvtHandler
{
public:
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};

class WXDLLIMPEXP_MEDIA wxAMMediaBackend : public wxMediaBackend
{
public:
    wxAMMediaBackend();
    virtual ~wxAMMediaBackend();
    void Clear();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);
    virtual bool Load(const wxURI& location, const wxURI& proxy);

    bool DoLoad(const wxString& location);
    void FinishLoad();

    virtual wxMediaState GetState();

    virtual bool SetPosition(wxLongLong where);
    virtual wxLongLong GetPosition();
    virtual wxLongLong GetDuration();

    virtual void Move(int x, int y, int w, int h);
    wxSize GetVideoSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double);

    virtual double GetVolume();
    virtual bool SetVolume(double);

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags);
    void Cleanup();

    void DoGetDownloadProgress(wxLongLong*, wxLongLong*);
    virtual wxLongLong GetDownloadProgress()
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return progress;
    }
    virtual wxLongLong GetDownloadTotal()
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return total;
    }

    wxControl* m_ctrl;
    wxActiveX* m_pAX;
    IActiveMovie* m_pAM;
    IMediaPlayer* m_pMP;
    wxTimer* m_pTimer;
    wxSize m_bestSize;
#ifdef __WXDEBUG__
    HMODULE m_hQuartzDll;
    LPAMGETERRORTEXT m_lpAMGetErrorText;
    wxString GetErrorString(HRESULT hrdsv);
#endif

    DECLARE_DYNAMIC_CLASS(wxAMMediaBackend)
};

//---------------------------------------------------------------------------
//
//  wxMCIMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MCI Includes
//---------------------------------------------------------------------------
#include <mmsystem.h>

class WXDLLIMPEXP_MEDIA wxMCIMediaBackend : public wxMediaBackend
{
public:

    wxMCIMediaBackend();
    ~wxMCIMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(wxLongLong where);
    virtual wxLongLong GetPosition();
    virtual wxLongLong GetDuration();

    virtual void Move(int x, int y, int w, int h);
    wxSize GetVideoSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double dRate);

    virtual double GetVolume();
    virtual bool SetVolume(double);

    static LRESULT CALLBACK NotifyWndProc(HWND hWnd, UINT nMsg,
                                             WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                     WPARAM wParam, LPARAM lParam);

    MCIDEVICEID m_hDev;     //Our MCI Device ID/Handler
    wxControl* m_ctrl;      //Parent control
    HWND m_hNotifyWnd;      //Window to use for MCI events
    bool m_bVideo;          //Whether or not we have video

    DECLARE_DYNAMIC_CLASS(wxMCIMediaBackend)
};

//---------------------------------------------------------------------------
//
//  wxQTMediaBackend
//
// We don't include Quicktime headers here and define all the types
// ourselves because looking for the quicktime libaries etc. would
// be tricky to do and making this a dependency for the MSVC projects
// would be unrealistic.
//
// Thanks to Robert Roebling for the wxDL macro/library idea
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  QT Includes
//---------------------------------------------------------------------------
//#include <qtml.h>                   //Windoze QT include
//#include <QuickTimeComponents.h>    //Standard QT stuff
#include "wx/dynlib.h"

//---------------------------------------------------------------------------
//  QT Types
//---------------------------------------------------------------------------
typedef struct MovieRecord* Movie;
typedef wxInt16 OSErr;
typedef wxInt32 OSStatus;
#define noErr 0
#define fsRdPerm 1
typedef unsigned char                   Str255[256];
#define StringPtr unsigned char*
#define newMovieActive 1
#define newMovieAsyncOK (1 << 8)
#define Ptr char*
#define Handle Ptr*
#define Fixed long
#define OSType unsigned long
#define CGrafPtr struct GrafPort *
#define TimeScale long
#define TimeBase struct TimeBaseRecord *
typedef struct ComponentInstanceRecord * ComponentInstance;
#define kMovieLoadStatePlayable 10000
#define Boolean int
#define MovieController ComponentInstance

#ifndef URLDataHandlerSubType
#if defined(__WATCOMC__) || defined(__MINGW32__)
// use magic numbers for compilers which complain about multicharacter integers
const OSType URLDataHandlerSubType     = 1970433056;
const OSType VisualMediaCharacteristic = 1702454643;
#else
const OSType URLDataHandlerSubType     = 'url ';
const OSType VisualMediaCharacteristic = 'eyes';
#endif
#endif

struct FSSpec {
    short      vRefNum;
    long       parID;
    Str255     name;  /*Str63 on mac, Str255 on msw */
};

struct Rect {
    short      top;
    short      left;
    short      bottom;
    short      right;
};

struct wide {
    wxInt32    hi;
    wxUint32   lo;
};

struct TimeRecord {
    wide       value; /* units */
    TimeScale  scale; /* units per second */
    TimeBase   base;
};

struct Point {
    short                           v;
    short                           h;
};

struct EventRecord {
    wxUint16                       what;
    wxUint32                          message;
    wxUint32                          when;
    Point                           where;
    wxUint16                  modifiers;
};

enum {
    mcTopLeftMovie              = 1,
    mcScaleMovieToFit           = 2,
    mcWithBadge                 = 4,
    mcNotVisible                = 8,
    mcWithFrame                 = 16
};

//---------------------------------------------------------------------------
//  QT Library
//---------------------------------------------------------------------------
#define wxDL_METHOD_DEFINE( rettype, name, args, shortargs, defret ) \
    typedef rettype (* name ## Type) args ; \
    name ## Type pfn_ ## name; \
    rettype name args \
    { if (m_ok) return pfn_ ## name shortargs ; return defret; }

#define wxDL_VOIDMETHOD_DEFINE( name, args, shortargs ) \
    typedef void (* name ## Type) args ; \
    name ## Type pfn_ ## name; \
    void name args \
    { if (m_ok) pfn_ ## name shortargs ; }

#define wxDL_METHOD_LOAD( lib, name, success ) \
    pfn_ ## name = (name ## Type) lib.GetSymbol( wxT(#name), &success ); \
    if (!success) { wxLog::EnableLogging(bWasLoggingEnabled); return false; }


class WXDLLIMPEXP_MEDIA wxQuickTimeLibrary
{
public:
    ~wxQuickTimeLibrary()
    {
        if(m_dll.IsLoaded())
            m_dll.Unload();
    }

    bool Initialize();
    bool IsOk() const {return m_ok;}

protected:
    wxDynamicLibrary m_dll;
    bool m_ok;

public:
    wxDL_VOIDMETHOD_DEFINE( StartMovie, (Movie m), (m) );
    wxDL_VOIDMETHOD_DEFINE( StopMovie, (Movie m), (m) );
    wxDL_METHOD_DEFINE( bool, IsMovieDone, (Movie m), (m), false);
    wxDL_VOIDMETHOD_DEFINE( GoToBeginningOfMovie, (Movie m), (m) );
    wxDL_METHOD_DEFINE( OSErr, GetMoviesError, (), (), -1);
    wxDL_METHOD_DEFINE( OSErr, EnterMovies, (), (), -1);
    wxDL_VOIDMETHOD_DEFINE( ExitMovies, (), () );
    wxDL_METHOD_DEFINE( OSErr, InitializeQTML, (long flags), (flags), -1);
    wxDL_VOIDMETHOD_DEFINE( TerminateQTML, (), () );

    wxDL_METHOD_DEFINE( OSErr, NativePathNameToFSSpec,
                        (char* inName, FSSpec* outFile, long flags),
                        (inName, outFile, flags), -1);

    wxDL_METHOD_DEFINE( OSErr, OpenMovieFile,
                        (const FSSpec * fileSpec, short * resRefNum, wxInt8 permission),
                        (fileSpec, resRefNum, permission), -1 );

    wxDL_METHOD_DEFINE( OSErr, CloseMovieFile,
                        (short resRefNum), (resRefNum), -1);

    wxDL_METHOD_DEFINE( OSErr, NewMovieFromFile,
                            (Movie * theMovie, short resRefNum, short *  resId,
                             StringPtr resName, short newMovieFlags,
                             bool * dataRefWasChanged),
                             (theMovie, resRefNum, resId, resName, newMovieFlags,
                              dataRefWasChanged), -1);

    wxDL_VOIDMETHOD_DEFINE( SetMovieRate, (Movie m, Fixed rate), (m, rate) );
    wxDL_METHOD_DEFINE( Fixed, GetMovieRate, (Movie m), (m), 0);
    wxDL_VOIDMETHOD_DEFINE( MoviesTask, (Movie m, long maxms), (m, maxms) );
    wxDL_VOIDMETHOD_DEFINE( BlockMove,
        (const char* p1, const char* p2, long s), (p1,p2,s) );
    wxDL_METHOD_DEFINE( Handle, NewHandleClear, (long s), (s), NULL );

    wxDL_METHOD_DEFINE( OSErr, NewMovieFromDataRef,
                           (Movie * m, short flags, short * id,
                            Handle  dataRef, OSType  dataRefType),
                            (m,flags,id,dataRef,dataRefType), -1 );

    wxDL_VOIDMETHOD_DEFINE( DisposeHandle, (Handle h), (h) );
    wxDL_VOIDMETHOD_DEFINE( GetMovieNaturalBoundsRect, (Movie m, Rect* r), (m,r) );
    wxDL_METHOD_DEFINE( void*, GetMovieIndTrackType,
                        (Movie m, long index, OSType type, long flags),
                        (m,index,type,flags), NULL );
    wxDL_VOIDMETHOD_DEFINE( CreatePortAssociation,
            (void* hWnd, void* junk, long morejunk), (hWnd, junk, morejunk) );
    wxDL_METHOD_DEFINE(void*, GetNativeWindowPort, (void* hWnd), (hWnd), NULL);
    wxDL_VOIDMETHOD_DEFINE(SetMovieGWorld, (Movie m, CGrafPtr port, void* whatever),
                            (m, port, whatever) );
    wxDL_VOIDMETHOD_DEFINE(DisposeMovie, (Movie m), (m) );
    wxDL_VOIDMETHOD_DEFINE(SetMovieBox, (Movie m, Rect* r), (m,r));
    wxDL_VOIDMETHOD_DEFINE(SetMovieTimeScale, (Movie m, long s), (m,s));
    wxDL_METHOD_DEFINE(long, GetMovieDuration, (Movie m), (m), 0);
    wxDL_METHOD_DEFINE(TimeBase, GetMovieTimeBase, (Movie m), (m), 0);
    wxDL_METHOD_DEFINE(TimeScale, GetMovieTimeScale, (Movie m), (m), 0);
    wxDL_METHOD_DEFINE(long, GetMovieTime, (Movie m, void* cruft), (m,cruft), 0);
    wxDL_VOIDMETHOD_DEFINE(SetMovieTime, (Movie m, TimeRecord* tr), (m,tr) );
    wxDL_METHOD_DEFINE(short, GetMovieVolume, (Movie m), (m), 0);
    wxDL_VOIDMETHOD_DEFINE(SetMovieVolume, (Movie m, short sVolume), (m,sVolume) );
    wxDL_VOIDMETHOD_DEFINE(SetMovieTimeValue, (Movie m, long s), (m,s));
    wxDL_METHOD_DEFINE(ComponentInstance, NewMovieController, (Movie m, const Rect* mr, long fl), (m,mr,fl), 0);
    wxDL_VOIDMETHOD_DEFINE(DisposeMovieController, (ComponentInstance ci), (ci));
    wxDL_METHOD_DEFINE(int, MCSetVisible, (ComponentInstance m, int b), (m, b), 0);


    wxDL_VOIDMETHOD_DEFINE(PrePrerollMovie, (Movie m, long t, Fixed r, WXFARPROC p1, void* p2), (m,t,r,p1,p2) );
    wxDL_VOIDMETHOD_DEFINE(PrerollMovie, (Movie m, long t, Fixed r), (m,t,r) );
    wxDL_METHOD_DEFINE(Fixed, GetMoviePreferredRate, (Movie m), (m), 0);
    wxDL_METHOD_DEFINE(long, GetMovieLoadState, (Movie m), (m), 0);
    wxDL_METHOD_DEFINE(void*, NewRoutineDescriptor, (WXFARPROC f, int l, void* junk), (f, l, junk), 0);
    wxDL_VOIDMETHOD_DEFINE(DisposeRoutineDescriptor, (void* f), (f));
    wxDL_METHOD_DEFINE(void*, GetCurrentArchitecture, (), (), 0);
    wxDL_METHOD_DEFINE(int, MCDoAction, (ComponentInstance ci, long f, void* p), (ci,f,p), 0);
    wxDL_VOIDMETHOD_DEFINE(MCSetControllerBoundsRect, (ComponentInstance ci, Rect* r), (ci,r));
    wxDL_VOIDMETHOD_DEFINE(DestroyPortAssociation, (CGrafPtr g), (g));
    wxDL_VOIDMETHOD_DEFINE(NativeEventToMacEvent, (MSG* p1, EventRecord* p2), (p1,p2));
    wxDL_VOIDMETHOD_DEFINE(MCIsPlayerEvent, (ComponentInstance ci, EventRecord* p2), (ci, p2));
    wxDL_METHOD_DEFINE(int, MCSetMovie, (ComponentInstance ci, Movie m, void* p1, Point w),
                          (ci,m,p1,w),0);
    wxDL_VOIDMETHOD_DEFINE(MCPositionController,
        (ComponentInstance ci, Rect* r, void* junk, void* morejunk), (ci,r,junk,morejunk));
    wxDL_VOIDMETHOD_DEFINE(MCSetActionFilterWithRefCon,
        (ComponentInstance ci, WXFARPROC cb, void* ref), (ci,cb,ref));
    wxDL_VOIDMETHOD_DEFINE(MCGetControllerInfo, (MovieController mc, long* flags), (mc,flags));
    wxDL_VOIDMETHOD_DEFINE(BeginUpdate, (CGrafPtr port), (port));
    wxDL_VOIDMETHOD_DEFINE(UpdateMovie, (Movie m), (m));
    wxDL_VOIDMETHOD_DEFINE(EndUpdate, (CGrafPtr port), (port));
    wxDL_METHOD_DEFINE( OSErr, GetMoviesStickyError, (), (), -1);
};

bool wxQuickTimeLibrary::Initialize()
{
    m_ok = false;

    bool bWasLoggingEnabled = wxLog::EnableLogging(false);    //Turn off the wxDynamicLibrary logging

    if(!m_dll.Load(wxT("qtmlClient.dll")))
    {
        wxLog::EnableLogging(bWasLoggingEnabled);
        return false;
    }

    wxDL_METHOD_LOAD( m_dll, StartMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, StopMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, IsMovieDone, m_ok );
    wxDL_METHOD_LOAD( m_dll, GoToBeginningOfMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMoviesError, m_ok );
    wxDL_METHOD_LOAD( m_dll, EnterMovies, m_ok );
    wxDL_METHOD_LOAD( m_dll, ExitMovies, m_ok );
    wxDL_METHOD_LOAD( m_dll, InitializeQTML, m_ok );
    wxDL_METHOD_LOAD( m_dll, TerminateQTML, m_ok );
    wxDL_METHOD_LOAD( m_dll, NativePathNameToFSSpec, m_ok );
    wxDL_METHOD_LOAD( m_dll, OpenMovieFile, m_ok );
    wxDL_METHOD_LOAD( m_dll, CloseMovieFile, m_ok );
    wxDL_METHOD_LOAD( m_dll, NewMovieFromFile, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieRate, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieRate, m_ok );
    wxDL_METHOD_LOAD( m_dll, MoviesTask, m_ok );
    wxDL_METHOD_LOAD( m_dll, BlockMove, m_ok );
    wxDL_METHOD_LOAD( m_dll, NewHandleClear, m_ok );
    wxDL_METHOD_LOAD( m_dll, NewMovieFromDataRef, m_ok );
    wxDL_METHOD_LOAD( m_dll, DisposeHandle, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieNaturalBoundsRect, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieIndTrackType, m_ok );
    wxDL_METHOD_LOAD( m_dll, CreatePortAssociation, m_ok );
    wxDL_METHOD_LOAD( m_dll, DestroyPortAssociation, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetNativeWindowPort, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieGWorld, m_ok );
    wxDL_METHOD_LOAD( m_dll, DisposeMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieBox, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieTimeScale, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieDuration, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieTimeBase, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieTimeScale, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieTime, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieTime, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieVolume, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieVolume, m_ok );
    wxDL_METHOD_LOAD( m_dll, SetMovieTimeValue, m_ok );
    wxDL_METHOD_LOAD( m_dll, NewMovieController, m_ok );
    wxDL_METHOD_LOAD( m_dll, DisposeMovieController, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCSetVisible, m_ok );
    wxDL_METHOD_LOAD( m_dll, PrePrerollMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, PrerollMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMoviePreferredRate, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMovieLoadState, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCDoAction, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCSetControllerBoundsRect, m_ok );
    wxDL_METHOD_LOAD( m_dll, NativeEventToMacEvent, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCIsPlayerEvent, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCSetMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCSetActionFilterWithRefCon, m_ok );
    wxDL_METHOD_LOAD( m_dll, MCGetControllerInfo, m_ok );
    wxDL_METHOD_LOAD( m_dll, BeginUpdate, m_ok );
    wxDL_METHOD_LOAD( m_dll, UpdateMovie, m_ok );
    wxDL_METHOD_LOAD( m_dll, EndUpdate, m_ok );
    wxDL_METHOD_LOAD( m_dll, GetMoviesStickyError, m_ok );

    wxLog::EnableLogging(bWasLoggingEnabled);
    m_ok = true;

    return true;
}

//cludgy workaround for wx events.  slots would be nice :)
class WXDLLIMPEXP_MEDIA wxQTMediaEvtHandler : public wxEvtHandler
{
public:
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};

class WXDLLIMPEXP_MEDIA wxQTMediaBackend : public wxMediaBackend
{
public:
    wxQTMediaBackend();
    ~wxQTMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(wxLongLong where);
    virtual wxLongLong GetPosition();
    virtual wxLongLong GetDuration();

    virtual void Move(int x, int y, int w, int h);
    wxSize GetVideoSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double dRate);

    virtual double GetVolume();
    virtual bool SetVolume(double);

    void Cleanup();
    void FinishLoad();

    static void PPRMProc (Movie theMovie, OSErr theErr, void* theRefCon);
    //TODO: Last param actually long - does this work on 64bit machines?
    static Boolean MCFilterProc (MovieController theController,
        short action, void *params, LONG_PTR refCon);

    static LRESULT CALLBACK QTWndProc(HWND, UINT, WPARAM, LPARAM);

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags);

    wxSize m_bestSize;              //Original movie size
    Movie m_movie;    //QT Movie handle/instance
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    bool m_bPlaying;                //Whether or not movie is playing
    wxTimer* m_timer;               //Load or Play timer
    wxQuickTimeLibrary m_lib;       //DLL to load functions from
    ComponentInstance m_pMC;        //Movie Controller

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend)
};


//===========================================================================
//  IMPLEMENTATION
//===========================================================================

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxAMMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxAMMediaBackend, wxMediaBackend);

//---------------------------------------------------------------------------
// Usual debugging macros
//---------------------------------------------------------------------------
#ifdef __WXDEBUG__
#define MAX_ERROR_TEXT_LEN 160

//Get the error string for Active Movie
wxString wxAMMediaBackend::GetErrorString(HRESULT hrdsv)
{
    wxChar szError[MAX_ERROR_TEXT_LEN];
    if( m_lpAMGetErrorText != NULL &&
       (*m_lpAMGetErrorText)(hrdsv, szError, MAX_ERROR_TEXT_LEN) == 0)
    {
        return wxString::Format(wxT("DirectShow error \"%s\" \n")
                                     wxT("(numeric %X)\n")
                                     wxT("occured"),
                                     szError, (int)hrdsv);
    }
    else
    {
        return wxString::Format(wxT("Unknown error \n")
                                     wxT("(numeric %X)\n")
                                     wxT("occured"),
                                     (int)hrdsv);
    }
}

#define wxAMFAIL(x) wxFAIL_MSG(GetErrorString(x));
#define wxVERIFY(x) wxASSERT((x))
#define wxAMLOG(x) wxLogDebug(GetErrorString(x))
#else
#define wxAMVERIFY(x) (x)
#define wxVERIFY(x) (x)
#define wxAMLOG(x)
#define wxAMFAIL(x)
#endif

//---------------------------------------------------------------------------
// Standard macros for ease of use
//---------------------------------------------------------------------------
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

//---------------------------------------------------------------------------
// wxAMLoadTimer
//
// Queries the control periodically to see if it has reached the point
// in its loading cycle where we can begin playing the media - if so
// then we finish up some things like getting the original size of the video
// and then sending the loaded event to our handler
//---------------------------------------------------------------------------
class wxAMLoadTimer : public wxTimer
{
public:
    wxAMLoadTimer(wxAMMediaBackend* parent) :
      m_parent(parent) {}

    void Notify()
    {
        if(m_parent->m_pMP)
        {
            MPReadyStateConstants nState;
            m_parent->m_pMP->get_ReadyState(&nState);
            if(nState != mpReadyStateLoading)
            {
                Stop();
                m_parent->FinishLoad();
                delete this;
            }
        }
        else
        {
            IActiveMovie2* pAM2 = NULL;
            ReadyStateConstants nState;
            if(m_parent->m_pAM->QueryInterface(IID_IActiveMovie2,
                                              (void**)&pAM2) == 0 &&
                pAM2->get_ReadyState(&nState) == 0)
            {
                pAM2->Release();
                if(nState != amvLoading)
                {
                    Stop();
                    m_parent->FinishLoad();
                    delete this;
                }
            }
            else
            {
                if(pAM2)
                    pAM2->Release();

                Stop();
                m_parent->FinishLoad();
                delete this;
            }
        }

    }

protected:
    wxAMMediaBackend* m_parent;     //Backend pointer
};

//---------------------------------------------------------------------------
// wxAMPlayTimer
//
// Sets m_hNotifyWnd to NULL to signify that we haven't loaded anything yet
// Queries the control periodically to see if it has stopped -
// if it has it sends the stop event
//---------------------------------------------------------------------------
class wxAMPlayTimer : public wxTimer
{
public:
    wxAMPlayTimer(wxAMMediaBackend* parent) :
      m_parent(parent) {}

    void Notify()
    {
        if(m_parent->GetState() == wxMEDIASTATE_STOPPED &&
           //NB:  Stop events could get triggered by the interface
           //if ShowPlayerControls is enabled,
           //so we need this hack here to make an attempt
           //at it not getting sent - but its far from ideal -
           //they can still get sent in some cases
           m_parent->GetPosition() == m_parent->GetDuration())
        {
            wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                                  m_parent->m_ctrl->GetId());
            m_parent->m_ctrl->ProcessEvent(theEvent);

            if(theEvent.IsAllowed())
            {
                //Seek to beginning of movie
                m_parent->wxAMMediaBackend::SetPosition(0);
                Stop();

                //send the event to our child
                wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                      m_parent->m_ctrl->GetId());
                m_parent->m_ctrl->AddPendingEvent(theEvent);
            }
        }
    }

protected:
    wxAMMediaBackend* m_parent;     //Backend pointer
};


/*
// The following is an alternative way - but it doesn't seem
// to work with the IActiveMovie control - it probably processes
// its own events
//---------------------------------------------------------------------------
// wxAMPlayTimer
//
// Query the IMediaEvent interface from the embedded WMP's
// filtergraph, then process the events from it - sending
// EC_COMPLETE events as stop events to the media control.
//---------------------------------------------------------------------------
class wxAMPlayTimer : public wxTimer
{
public:
    wxAMPlayTimer(wxAMMediaBackend* pBE) : m_pBE(pBE), m_pME(NULL)
    {
        HRESULT hr;
        IUnknown* pGB;
        hr = m_pBE->m_pAM->get_FilterGraph(&pGB);
        wxASSERT(SUCCEEDED(hr));
        hr = pGB->QueryInterface(IID_IMediaEvent, (void**)&m_pME);
        wxASSERT(SUCCEEDED(hr));
        pGB->Release();
    }

    ~wxAMPlayTimer()
    {
        SAFE_RELEASE(m_pME);
    }

    void Notify()
    {
        LONG        evCode;
        LONG_PTR    evParam1,
                    evParam2;

        //
        // DirectShow keeps a list of queued events, and we need
        // to go through them one by one, stopping at (Hopefully only one)
        // EC_COMPLETE message
        //
        while( m_pME->GetEvent(&evCode, &evParam1, &evParam2, 0) == 0 )
        {
            // Cleanup memory that GetEvent allocated
            HRESULT hr = m_pME->FreeEventParams(evCode,
                                                evParam1, evParam2);
            if(hr != 0)
            {
                //Even though this makes a messagebox this
                //is windows where we can do gui stuff in seperate
                //threads :)
                wxFAIL_MSG(m_pBE->GetErrorString(hr));
            }
            // If this is the end of the clip, notify handler
            else if(1 == evCode) //EC_COMPLETE
            {
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                                    m_pBE->m_ctrl->GetId());
                m_pBE->m_ctrl->ProcessEvent(theEvent);

                if(theEvent.IsAllowed())
                {
                    Stop();

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                        m_pBE->m_ctrl->GetId());
                    m_pBE->m_ctrl->AddPendingEvent(theEvent);
                }
            }
        }
    }

protected:
    wxAMMediaBackend* m_pBE;     //Backend pointer
    IMediaEvent* m_pME;          //To determine when to send stop event
};
*/

//---------------------------------------------------------------------------
// wxAMMediaBackend Constructor
//---------------------------------------------------------------------------
wxAMMediaBackend::wxAMMediaBackend()
                 :m_pAX(NULL),
                  m_pAM(NULL),
                  m_pMP(NULL),
                  m_pTimer(NULL)
#ifdef __WXDEBUG__
                 ,m_hQuartzDll(NULL)
#endif
{
}

//---------------------------------------------------------------------------
// wxAMMediaBackend Destructor
//---------------------------------------------------------------------------
wxAMMediaBackend::~wxAMMediaBackend()
{
    Clear(); //Free memory from Load()

    if(m_pAX)
    {
        m_pAX->DissociateHandle();
        delete m_pAX;
        m_pAM->Release();

        if(m_pMP)
            m_pMP->Release();
    }
#ifdef __WXDEBUG__
    if(m_hQuartzDll)
        ::FreeLibrary(m_hQuartzDll);
#endif
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Clear
//
// Free up interfaces and memory allocated by LoadXXX
//---------------------------------------------------------------------------
void wxAMMediaBackend::Clear()
{
    if(m_pTimer)
        delete m_pTimer;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::CreateControl
//---------------------------------------------------------------------------
bool wxAMMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    // First get the AMGetErrorText procedure in debug
    // mode for more meaningful messages
#ifdef __WXDEBUG__
    m_hQuartzDll = ::LoadLibrary(wxT("quartz.dll"));
    if(m_hQuartzDll)
    {
            m_lpAMGetErrorText = (LPAMGETERRORTEXT) ::GetProcAddress(
                m_hQuartzDll,
            wxString::Format(wxT("AMGetErrorText%s"),

#if wxUSE_UNICODE
            wxT("W")
#else
            wxT("A")
#endif
#ifdef __WXWINCE__
                             )
#else
                             ).mb_str(wxConvLocal)
#endif
                             );
    }
#endif
    // Now determine which (if any) media player interface is
    // available - IMediaPlayer or IActiveMovie
    if( ::CoCreateInstance(CLSID_MediaPlayer, NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IMediaPlayer, (void**)&m_pMP) != 0 )
    {
        if( ::CoCreateInstance(CLSID_ActiveMovie, NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IActiveMovie, (void**)&m_pAM) != 0 )
        return false;
        m_pAM->QueryInterface(IID_IMediaPlayer, (void**)&m_pMP);
    }
    else
    {
        m_pMP->QueryInterface(IID_IActiveMovie, (void**)&m_pAM);
    }
    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxBORDER_NONE,
                            validator, name) )
        return false;

    //
    // Now create the ActiveX container along with the media player
    // interface and query them
    //
    m_ctrl = ctrl;
    m_pAX = new wxActiveX(ctrl,
                m_pMP ? IID_IMediaPlayer : IID_IActiveMovie,
                m_pAM);


    //
    //  Here we set up wx-specific stuff for the default
    //  settings wxMediaCtrl says it will stay to
    //
    if(m_pMP)
    {
        m_pMP->put_DisplaySize(mpFitToSize);
        // TODO: Unsure what actual effect this has
        m_pMP->put_WindowlessVideo(VARIANT_TRUE);
    }
    else
        m_pAM->put_MovieWindowSize(amvDoubleOriginalSize);

    //by default true
    m_pAM->put_AutoStart(VARIANT_FALSE);
    //by default enabled
    wxAMMediaBackend::ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_NONE);
    //by default with AM only 0.5
    wxAMMediaBackend::SetVolume(1.0);

    // My problem with this was only with a previous patch, probably the
    // third rewrite fixed it as a side-effect. In fact, the erase
    // background style of drawing not only works now, but is much better
    // than paint-based updates (the paint event handler flickers if the
    // wxMediaCtrl shares a sizer with another child window, or is on a
    // notebook)
    //  - Greg Hazel
    m_ctrl->Connect(m_ctrl->GetId(), wxEVT_ERASE_BACKGROUND,
        wxEraseEventHandler(wxAMMediaEvtHandler::OnEraseBackground),
        NULL, (wxEvtHandler*) this);

    // success
    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (file version)
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxString& fileName)
{
    return DoLoad(fileName);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (URL Version)
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxURI& location)
{
    //  Turn off loading from a proxy as user
    //  may have set it previously
    INSPlay* pPlay = NULL;
    m_pAM->QueryInterface(IID_INSPlay, (void**) &pPlay);
    if(pPlay)
    {
        pPlay->put_UseHTTPProxy(VARIANT_FALSE);
        pPlay->Release();
    }

    return DoLoad(location.BuildURI());
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (URL Version with Proxy)
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxURI& location, const wxURI& proxy)
{
    // Set the proxy of the NETSHOW interface
    INSPlay* pPlay = NULL;
    m_pAM->QueryInterface(IID_INSPlay, (void**) &pPlay);

    if(pPlay)
    {
        pPlay->put_UseHTTPProxy(VARIANT_TRUE);
        pPlay->put_HTTPProxyHost(wxBasicString(proxy.GetServer()).Get());
        pPlay->put_HTTPProxyPort(wxAtoi(proxy.GetPort()));
        pPlay->Release();
    }

    return DoLoad(location.BuildURI());
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::DoLoad
//
// Called by all functions - this actually renders
// the file and sets up the filter graph
//---------------------------------------------------------------------------
bool wxAMMediaBackend::DoLoad(const wxString& location)
{
    Clear(); //Clear up previously allocated memory

    HRESULT hr;

    // Play the movie the normal way through the embedded
    // WMP.  Supposively Open is better in theory because
    // the docs say its async and put_FileName is not -
    // but in practice they both seem to be async anyway
    if(m_pMP)
        hr = m_pMP->Open( wxBasicString(location).Get() );
    else
        hr = m_pAM->put_FileName( wxBasicString(location).Get() );

    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    // In AM playing will FAIL if
    // the user plays before the media is loaded
    m_pTimer = new wxAMLoadTimer(this);
    m_pTimer->Start(20);
    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::FinishLoad
//
// Called by our wxAMLoadTimer when the
// embedded WMP tells its the media is ready to play.
//
// Here we get the original size of the video and
// send the loaded event to our watcher :).
//---------------------------------------------------------------------------
void wxAMMediaBackend::FinishLoad()
{
    //Get the original video size
    m_pAM->get_ImageSourceWidth((long*)&m_bestSize.x);
    m_pAM->get_ImageSourceHeight((long*)&m_bestSize.y);

    //
    //Start the play timer to catch stop events
    //Previous load timer cleans up itself
    //
    m_pTimer = new wxAMPlayTimer(this);

    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a separate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    m_ctrl->SetSize(m_ctrl->GetSize());

    //Send event to our children
    wxMediaEvent theEvent(wxEVT_MEDIA_LOADED,
                            m_ctrl->GetId());
    m_ctrl->AddPendingEvent(theEvent);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::ShowPlayerControls
//---------------------------------------------------------------------------
bool wxAMMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    // Note that IMediaPlayer doesn't have a statusbar by
    // default but IActiveMovie does - so lets try to keep
    // the interface consistant
    if(!flags)
    {
        m_pAM->put_Enabled(VARIANT_FALSE);
        m_pAM->put_ShowControls(VARIANT_FALSE);
        if(m_pMP)
            m_pMP->put_ShowStatusBar(VARIANT_FALSE);
    }
    else
    {
        m_pAM->put_Enabled(VARIANT_TRUE);
        m_pAM->put_ShowControls(VARIANT_TRUE);

        m_pAM->put_ShowPositionControls(
                (flags & wxMEDIACTRLPLAYERCONTROLS_STEP) ?
                VARIANT_TRUE : VARIANT_FALSE);

        if(m_pMP)
        {
            m_pMP->put_ShowStatusBar(VARIANT_TRUE);
            m_pMP->put_ShowAudioControls(
                (flags & wxMEDIACTRLPLAYERCONTROLS_VOLUME) ?
                VARIANT_TRUE : VARIANT_FALSE);
        }
    }

    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Play
//
// Plays the stream.  If it is non-seekable, it will restart it (implicit).
//
// Note that we use SUCCEEDED here because run/pause/stop tend to be overly
// picky and return warnings on pretty much every call
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Play()
{
    // if the movie isn't done loading yet
    // go into an sync getmessage loop until it is :)
    if(m_pMP)
    {
        MPReadyStateConstants nState;
        m_pMP->get_ReadyState(&nState);
        while(nState == mpReadyStateLoading && wxYieldIfNeeded())
        {
          m_pMP->get_ReadyState(&nState);
        }
    }
    else
    {
        IActiveMovie2* pAM2;
        ReadyStateConstants nState;
        if(m_pAM->QueryInterface(IID_IActiveMovie2, (void**)&pAM2) == 0 &&
            pAM2->get_ReadyState(&nState) == 0)
        {
            while(nState == amvLoading && wxYieldIfNeeded())
            {
                pAM2->get_ReadyState(&nState);
            }
            pAM2->Release();
        }
    }

    //Actually try to play the movie
    HRESULT hr = m_pAM->Run();
    if(SUCCEEDED(hr))
    {
        m_pTimer->Start(20);
        return true;
    }
    wxAMLOG(hr);
    return false;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Pause
//
// Pauses the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Pause()
{
    HRESULT hr = m_pAM->Pause();
    if(SUCCEEDED(hr))
        return true;
    wxAMLOG(hr);
    return false;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Stop
//
// Stops the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Stop()
{
    HRESULT hr = m_pAM->Stop();
    if(SUCCEEDED(hr))
    {
        //Seek to beginning
        wxAMMediaBackend::SetPosition(0);
        //Stop stop event timer
        m_pTimer->Stop();
        return true;
    }
    wxAMLOG(hr);
    return false;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetPosition
//
// 1) Translates the current position's time to directshow time,
//    which is in a scale of 1 second (in a double)
// 2) Sets the play position of the IMediaSeeking interface -
//    passing NULL as the stop position means to keep the old
//    stop position
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetPosition(wxLongLong where)
{
    HRESULT hr = m_pAM->put_CurrentPosition(
                        ((LONGLONG)where.GetValue()) / 1000.0
                                     );
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetPosition
//
// 1) Obtains the current play and stop positions from IMediaSeeking
// 2) Returns the play position translated to our time base
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetPosition()
{
    double outCur;
    HRESULT hr = m_pAM->get_CurrentPosition(&outCur);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return 0;
    }

    //h,m,s,milli - outdur is in 1 second (double)
    outCur *= 1000;
    wxLongLong ll;
    ll.Assign(outCur);

    return ll;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetVolume
//
// Gets the volume through the IBasicAudio interface -
// value ranges from 0 (MAX volume) to -10000 (minimum volume).
// -100 per decibel.
//---------------------------------------------------------------------------
double wxAMMediaBackend::GetVolume()
{
        long lVolume;
    HRESULT hr = m_pAM->get_Volume(&lVolume);
        if(FAILED(hr))
        {
            wxAMLOG(hr);
            return 0.0;
    }
    return pow(10.0, lVolume/2000.0);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetVolume
//
// Sets the volume through the IBasicAudio interface -
// value ranges from 0 (MAX volume) to -10000 (minimum volume).
// -100 per decibel.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetVolume(double dVolume)
{
    //pow(10.0, -80.0) to correct 0 == -INF
    long lVolume = (2000.0 * log10(pow(10.0, -80.0)+dVolume));
    HRESULT hr = m_pAM->put_Volume( lVolume );
        if(FAILED(hr))
        {
            wxAMLOG(hr);
            return false;
        }
        return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetDuration
//
// 1) Obtains the duration of the media from IAMMultiMediaStream
// 2) Converts that value to our time base, and returns it
//
// NB: With VBR MP3 files the default DirectShow MP3 render does not
// read the Xing header correctly, resulting in skewed values for duration
// and seeking
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetDuration()
{
    double outDuration;
    HRESULT hr = m_pAM->get_Duration(&outDuration);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return 0;
    }

    //h,m,s,milli - outdur is in 1 second (double)
    outDuration *= 1000;
    wxLongLong ll;
    ll.Assign(outDuration);

    return ll;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetState
//
// Returns the cached state
//---------------------------------------------------------------------------
wxMediaState wxAMMediaBackend::GetState()
{
    StateConstants nState;
    HRESULT hr = m_pAM->get_CurrentState(&nState);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return wxMEDIASTATE_STOPPED;
    }

    return (wxMediaState)nState;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetPlaybackRate
//
// Pretty simple way of obtaining the playback rate from
// the IMediaSeeking interface
//---------------------------------------------------------------------------
double wxAMMediaBackend::GetPlaybackRate()
{
    double dRate;
    HRESULT hr = m_pAM->get_Rate(&dRate);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return 0.0;
    }
    return dRate;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetPlaybackRate
//
// Sets the playback rate of the media - DirectShow is pretty good
// about this, actually
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetPlaybackRate(double dRate)
{
    HRESULT hr = m_pAM->put_Rate(dRate);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetDownloadXXX
//
// Queries for and gets the total size of the file and the current
// progress in downloading that file from the IAMOpenProgress
// interface from the media player interface's filter graph
//---------------------------------------------------------------------------
void wxAMMediaBackend::DoGetDownloadProgress(wxLongLong* pLoadProgress,
                                             wxLongLong* pLoadTotal)
{
    LONGLONG loadTotal = 0, loadProgress = 0;
    IUnknown* pFG;
    IAMOpenProgress* pOP;
    HRESULT hr;
    hr = m_pAM->get_FilterGraph(&pFG);
    if(SUCCEEDED(hr))
        {
        hr = pFG->QueryInterface(IID_IAMOpenProgress, (void**)&pOP);
        if(SUCCEEDED(hr))
        {
            hr = pOP->QueryProgress(&loadTotal, &loadProgress);
            pOP->Release();
        }
        pFG->Release();
    }

    if(SUCCEEDED(hr))
            {
        *pLoadProgress = loadProgress;
        *pLoadTotal = loadTotal;
            }
    else
            {
        //When not loading from a URL QueryProgress will return
        //E_NOINTERFACE or whatever
        //wxAMFAIL(hr);
        *pLoadProgress = 0;
        *pLoadTotal = 0;
    }
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetVideoSize
//
// Obtains the cached original video size
//---------------------------------------------------------------------------
wxSize wxAMMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Move
//
// We take care of this in our redrawing
//---------------------------------------------------------------------------
void wxAMMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y),
                            int WXUNUSED(w), int WXUNUSED(h))
{
}

//---------------------------------------------------------------------------
// wxAMMediaEvtHandler::OnEraseBackground
//
// Tell WX not to erase the background of our control window
// so that resizing is a bit smoother
//---------------------------------------------------------------------------
void wxAMMediaEvtHandler::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
}

//---------------------------------------------------------------------------
// End of wxAMMediaBackend
//---------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMCIMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxMCIMediaBackend, wxMediaBackend);

//---------------------------------------------------------------------------
// Usual debugging macros for MCI returns
//---------------------------------------------------------------------------

#ifdef __WXDEBUG__
#define wxMCIVERIFY(arg) \
{ \
    DWORD nRet; \
    if ( (nRet = (arg)) != 0) \
    { \
        TCHAR sz[5000]; \
        mciGetErrorString(nRet, sz, 5000); \
        wxFAIL_MSG(wxString::Format(_T("MCI Error:%s"), sz)); \
    } \
}
#else
#define wxMCIVERIFY(arg) (arg);
#endif

//---------------------------------------------------------------------------
// Simulation for <digitalv.h>
//
// Mingw and possibly other compilers don't have the digitalv.h header
// that is needed to have some essential features of mci work with
// windows - so we provide the declarations for the types we use here
//---------------------------------------------------------------------------

typedef struct {
    DWORD_PTR   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else
    RECT    rc;
#endif
} MCI_DGV_RECT_PARMS;

typedef struct {
    DWORD_PTR   dwCallback;
    HWND    hWnd;
#ifndef _WIN32
    WORD    wReserved1;
#endif
    UINT    nCmdShow;
#ifndef _WIN32
    WORD    wReserved2;
#endif
    wxChar*   lpstrText;
} MCI_DGV_WINDOW_PARMS;

typedef struct {
    DWORD_PTR dwCallback;
    DWORD     dwTimeFormat;
    DWORD     dwAudio;
    DWORD     dwFileFormat;
    DWORD     dwSpeed;
} MCI_DGV_SET_PARMS;

typedef struct {
    DWORD_PTR   dwCallback;
    DWORD   dwItem;
    DWORD   dwValue;
    DWORD   dwOver;
    wxChar*   lpstrAlgorithm;
    wxChar*   lpstrQuality;
} MCI_DGV_SETAUDIO_PARMS;

//---------------------------------------------------------------------------
// wxMCIMediaBackend Constructor
//
// Here we don't need to do much except say we don't have any video :)
//---------------------------------------------------------------------------
wxMCIMediaBackend::wxMCIMediaBackend() : m_hNotifyWnd(NULL), m_bVideo(false)
{
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend Destructor
//
// We close the mci device - note that there may not be an mci device here,
// or it may fail - but we don't really care, since we're destructing
//---------------------------------------------------------------------------
wxMCIMediaBackend::~wxMCIMediaBackend()
{
    if(m_hNotifyWnd)
    {
        mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);
        DestroyWindow(m_hNotifyWnd);
        m_hNotifyWnd = NULL;
    }
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Create
//
// Here we just tell wxMediaCtrl that mci does exist (which it does, on all
// msw systems, at least in some form dating back to win16 days)
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those, and create with
    // wxCLIP_CHILDREN, so that if the driver/backend
    // is a child window, it refereshes properly
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxBORDER_NONE | wxCLIP_CHILDREN,
                            validator, name) )
        return false;

    m_ctrl = ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Load (file version)
//
// Here we have MCI load a file and device, set the time format to our
// default (milliseconds), and set the video (if any) to play in the control
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Load(const wxString& fileName)
{
    //
    //if the user already called load close the previous MCI device
    //
    if(m_hNotifyWnd)
    {
        mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);
        DestroyWindow(m_hNotifyWnd);
        m_hNotifyWnd = NULL;
    }

    //
    //Opens a file and has MCI select a device.  Normally you'd put
    //MCI_OPEN_TYPE in addition to MCI_OPEN_ELEMENT - however if you
    //omit this it tells MCI to select the device instead.  This is
    //good because we have no reliable way of "enumerating" the devices
    //in MCI
    //
    MCI_OPEN_PARMS openParms;
    openParms.lpstrElementName = (wxChar*) fileName.c_str();

    if ( mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT,
                        (DWORD)(LPVOID)&openParms) != 0)
        return false;

    m_hDev = openParms.wDeviceID;

    //
    //Now set the time format for the device to milliseconds
    //
    MCI_SET_PARMS setParms;
    setParms.dwCallback = 0;
    setParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;

    if (mciSendCommand(m_hDev, MCI_SET, MCI_SET_TIME_FORMAT,
                         (DWORD)(LPVOID)&setParms) != 0)
        return false;

    //
    //Now tell the MCI device to display the video in our wxMediaCtrl
    //
    MCI_DGV_WINDOW_PARMS windowParms;
    windowParms.hWnd = (HWND)m_ctrl->GetHandle();

    m_bVideo = (mciSendCommand(m_hDev, MCI_WINDOW,
                               0x00010000L, //MCI_DGV_WINDOW_HWND
                               (DWORD)(LPVOID)&windowParms) == 0);

    //
    // Create a hidden window and register to handle
    // MCI events
    // Note that wxCanvasClassName is already registered
    // and used by all wxWindows and normal wxControls
    //
    m_hNotifyWnd = ::CreateWindow
                    (
                        wxCanvasClassName,
                        NULL,
                        0, 0, 0, 0,
                        0,
                        (HWND) NULL,
                        (HMENU)NULL,
                        wxGetInstance(),
                        (LPVOID) NULL
                    );

    if(!m_hNotifyWnd)
    {
        wxLogSysError( wxT("Could not create hidden needed for ")
                       wxT("registering for MCI events!")  );

        return false;
    }

    wxSetWindowProc(m_hNotifyWnd, wxMCIMediaBackend::NotifyWndProc);
    wxSetWindowUserData(m_hNotifyWnd, this);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a separate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    m_ctrl->SetSize(m_ctrl->GetSize());

    //send loaded event
    wxMediaEvent theEvent(wxEVT_MEDIA_LOADED,
                            m_ctrl->GetId());
    m_ctrl->AddPendingEvent(theEvent);

    return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Load (URL version)
//
// MCI doesn't support URLs directly (?)
//
// TODO:  Use wxURL/wxFileSystem and mmioInstallProc
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Load(const wxURI& WXUNUSED(location))
{
    return false;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Play
//
// Plays/Resumes the MCI device... a couple notes:
// 1) Certain drivers will crash and burn if we don't pass them an
//    MCI_PLAY_PARMS, despite the documentation that says otherwise...
// 2) There is a MCI_RESUME command, but MCI_PLAY does the same thing
//    and will resume from a stopped state also, so there's no need to
//    call both, for example
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Play()
{
    MCI_PLAY_PARMS playParms;
    playParms.dwCallback = (DWORD)m_hNotifyWnd;

    bool bOK = ( mciSendCommand(m_hDev, MCI_PLAY, MCI_NOTIFY,
                            (DWORD)(LPVOID)&playParms) == 0 );

    if(bOK)
        m_ctrl->Show(m_bVideo);

    return bOK;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Pause
//
// Pauses the MCI device - nothing special
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Pause()
{
    return (mciSendCommand(m_hDev, MCI_PAUSE, MCI_WAIT, 0) == 0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Stop
//
// Stops the MCI device & seeks to the beginning as wxMediaCtrl docs outline
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Stop()
{
    return (mciSendCommand(m_hDev, MCI_STOP, MCI_WAIT, 0) == 0) &&
           (mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0) == 0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetState
//
// Here we get the state and convert it to a wxMediaState -
// since we use direct comparisons with MCI_MODE_PLAY and
// MCI_MODE_PAUSE, we don't care if the MCI_STATUS call
// fails or not
//---------------------------------------------------------------------------
wxMediaState wxMCIMediaBackend::GetState()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_MODE;

    mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                         (DWORD)(LPVOID)&statusParms);

    if(statusParms.dwReturn == MCI_MODE_PAUSE)
        return wxMEDIASTATE_PAUSED;
    else if(statusParms.dwReturn == MCI_MODE_PLAY)
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_STOPPED;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::SetPosition
//
// Here we set the position of the device in the stream.
// Note that MCI actually stops the device after you seek it if the
// device is playing/paused, so we need to play the file after
// MCI seeks like normal APIs would
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::SetPosition(wxLongLong where)
{
    MCI_SEEK_PARMS seekParms;
    seekParms.dwCallback = 0;
#if wxUSE_LONGLONG_NATIVE && !wxUSE_LONGLONG_WX
    seekParms.dwTo = (DWORD)where.GetValue();
#else /* wxUSE_LONGLONG_WX */
    /* no way to return it in one piece */
    wxASSERT( where.GetHi()==0 );
    seekParms.dwTo = (DWORD)where.GetLo();
#endif /* wxUSE_LONGLONG_* */

    //device was playing?
    bool bReplay = GetState() == wxMEDIASTATE_PLAYING;

    if( mciSendCommand(m_hDev, MCI_SEEK, MCI_TO,
                       (DWORD)(LPVOID)&seekParms) != 0)
        return false;

    //If the device was playing, resume it
    if (bReplay)
        return Play();
    else
        return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetPosition
//
// Gets the position of the device in the stream using the current
// time format... nothing special here...
//---------------------------------------------------------------------------
wxLongLong wxMCIMediaBackend::GetPosition()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_POSITION;

    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                       (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetVolume
//
// Gets the volume of the current media via the MCI_DGV_STATUS_VOLUME
// message.  Value ranges from 0 (minimum) to 1000 (maximum volume).
//---------------------------------------------------------------------------
double wxMCIMediaBackend::GetVolume()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwCallback = 0;
    statusParms.dwItem = 0x4019; //MCI_DGV_STATUS_VOLUME

    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                       (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return ((double)statusParms.dwReturn) / 1000.0;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::SetVolume
//
// Sets the volume of the current media via the MCI_DGV_SETAUDIO_VOLUME
// message.  Value ranges from 0 (minimum) to 1000 (maximum volume).
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::SetVolume(double dVolume)
{
    MCI_DGV_SETAUDIO_PARMS audioParms;
    audioParms.dwCallback = 0;
    audioParms.dwItem = 0x4002; //MCI_DGV_SETAUDIO_VOLUME
    audioParms.dwValue = (DWORD) (dVolume * 1000.0);
    audioParms.dwOver = 0;
    audioParms.lpstrAlgorithm = NULL;
    audioParms.lpstrQuality = NULL;

    if (mciSendCommand(m_hDev, 0x0873, //MCI_SETAUDIO
                        //MCI_DGV_SETAUDIO+(_ITEM | _VALUE)
                        0x00800000L | 0x01000000L,
                       (DWORD)(LPSTR)&audioParms) != 0)
        return false;
    return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetDuration
//
// Gets the duration of the stream... nothing special
//---------------------------------------------------------------------------
wxLongLong wxMCIMediaBackend::GetDuration()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_LENGTH;

    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                        (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Move
//
// Moves the window to a location
//---------------------------------------------------------------------------
void wxMCIMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y),
                                       int w,           int h)
{
    if (m_hNotifyWnd && m_bVideo)
    {
        MCI_DGV_RECT_PARMS putParms; //ifdefed MCI_DGV_PUT_PARMS
        memset(&putParms, 0, sizeof(MCI_DGV_RECT_PARMS));
        putParms.rc.bottom = h;
        putParms.rc.right = w;

        //wxStackWalker will crash and burn here on assert
        //and mci doesn't like 0 and 0 for some reason (out of range )
        //so just don't it in that case
        if(w || h)
        {
            wxMCIVERIFY( mciSendCommand(m_hDev, MCI_PUT,
                                   0x00040000L, //MCI_DGV_PUT_DESTINATION
                                   (DWORD)(LPSTR)&putParms) );
        }
    }
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetVideoSize
//
// Gets the original size of the movie for sizers
//---------------------------------------------------------------------------
wxSize wxMCIMediaBackend::GetVideoSize() const
{
    if(m_bVideo)
    {
        MCI_DGV_RECT_PARMS whereParms; //ifdefed MCI_DGV_WHERE_PARMS

        wxMCIVERIFY( mciSendCommand(m_hDev, MCI_WHERE,
                       0x00020000L, //MCI_DGV_WHERE_SOURCE
                       (DWORD)(LPSTR)&whereParms) );

        return wxSize(whereParms.rc.right, whereParms.rc.bottom);
    }
    return wxSize(0,0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
double wxMCIMediaBackend::GetPlaybackRate()
{
    return 1.0;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::SetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::SetPlaybackRate(double WXUNUSED(dRate))
{
/*
    MCI_WAVE_SET_SAMPLESPERSEC
    MCI_DGV_SET_PARMS setParms;
    setParms.dwSpeed = (DWORD) (dRate * 1000.0);

    return (mciSendCommand(m_hDev, MCI_SET,
                       0x00020000L, //MCI_DGV_SET_SPEED
                       (DWORD)(LPSTR)&setParms) == 0);
*/
    return false;
}

//---------------------------------------------------------------------------
// [static] wxMCIMediaBackend::MSWWindowProc
//
// Here we process a message when MCI reaches the stopping point
// in the stream
//---------------------------------------------------------------------------
LRESULT CALLBACK wxMCIMediaBackend::NotifyWndProc(HWND hWnd, UINT nMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam)
{
    wxMCIMediaBackend* backend =
        (wxMCIMediaBackend*)wxGetWindowUserData(hWnd);

    return backend->OnNotifyWndProc(hWnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK wxMCIMediaBackend::OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam)
{
    if(nMsg == MM_MCINOTIFY)
    {
        wxASSERT(lParam == (LPARAM) m_hDev);
        if(wParam == MCI_NOTIFY_SUCCESSFUL && lParam == (LPARAM)m_hDev)
        {
            wxMediaEvent theEvent(wxEVT_MEDIA_STOP, m_ctrl->GetId());
            m_ctrl->ProcessEvent(theEvent);

            if(theEvent.IsAllowed())
            {
                wxMCIVERIFY( mciSendCommand(m_hDev, MCI_SEEK,
                                            MCI_SEEK_TO_START, 0) );

                //send the event to our child
                wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                      m_ctrl->GetId());
                m_ctrl->ProcessEvent(theEvent);
            }
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxQTMediaBackend
//
// TODO: Use a less cludgy way to pause/get state/set state
// FIXME: Greg Hazel reports that sometimes files that cannot be played
// with this backend are treated as playable anyway - not verifyed though.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxQTMediaBackend, wxMediaBackend);

//Time between timer calls - this is the Apple recommondation to the TCL
//team I believe
#define MOVIE_DELAY 20

#include "wx/timer.h"


//---------------------------------------------------------------------------
//          wxQTLoadTimer
//
//  QT, esp. QT for Windows is very picky about how you go about
//  async loading.  If you were to go through a Windows message loop
//  or a MoviesTask or both and then check the movie load state
//  it would still return 1000 (loading)... even (pre)prerolling doesn't
//  help.  However, making a load timer like this works
//---------------------------------------------------------------------------
class wxQTLoadTimer : public wxTimer
{
public:
    wxQTLoadTimer(Movie movie, wxQTMediaBackend* parent, wxQuickTimeLibrary* pLib) :
      m_movie(movie), m_parent(parent), m_pLib(pLib) {}

    void Notify()
    {
        m_pLib->MoviesTask(m_movie, 0);
        //kMovieLoadStatePlayable
        if(m_pLib->GetMovieLoadState(m_movie) >= 10000)
    {
            m_parent->FinishLoad();
            delete this;
    }
    }

protected:
    Movie m_movie;                  //Our movie instance
    wxQTMediaBackend* m_parent;     //Backend pointer
    wxQuickTimeLibrary* m_pLib;     //Interfaces
};


// --------------------------------------------------------------------------
//          wxQTPlayTimer - Handle Asyncronous Playing
//
// 1) Checks to see if the movie is done, and if not continues
//    streaming the movie
// 2) Sends the wxEVT_MEDIA_STOP event if we have reached the end of
//    the movie.
// --------------------------------------------------------------------------
class wxQTPlayTimer : public wxTimer
{
public:
    wxQTPlayTimer(Movie movie, wxQTMediaBackend* parent,
                  wxQuickTimeLibrary* pLib) :
        m_movie(movie), m_parent(parent), m_pLib(pLib) {}

    void Notify()
    {
        //
        //  OK, a little explaining - basically originally
        //  we only called MoviesTask if the movie was actually
        //  playing (not paused or stopped)... this was before
        //  we realized MoviesTask actually handles repainting
        //  of the current frame - so if you were to resize
        //  or something it would previously not redraw that
        //  portion of the movie.
        //
        //  So now we call MoviesTask always so that it repaints
        //  correctly.
        //
        m_pLib->MoviesTask(m_movie, 0);

        //
        //  Handle the stop event - if the movie has reached
        //  the end, notify our handler
        //
        //  m_bPlaying == !(Stopped | Paused)
        //
        if (m_parent->m_bPlaying)
        {
            if(m_pLib->IsMovieDone(m_movie))
            {
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                                      m_parent->m_ctrl->GetId());
                m_parent->m_ctrl->ProcessEvent(theEvent);

                if(theEvent.IsAllowed())
                {
                    m_parent->Stop();
                    wxASSERT(m_pLib->GetMoviesError() == noErr);

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                          m_parent->m_ctrl->GetId());
                    m_parent->m_ctrl->AddPendingEvent(theEvent);
                }
            }
        }
    }

protected:
    Movie m_movie;                  //Our movie instance
    wxQTMediaBackend* m_parent;     //Backend pointer
    wxQuickTimeLibrary* m_pLib;         //Interfaces
};


//---------------------------------------------------------------------------
// wxQTMediaBackend::QTWndProc
//
// Forwards events to the Movie Controller so that it can
// redraw itself/process messages etc..
//---------------------------------------------------------------------------
LRESULT CALLBACK wxQTMediaBackend::QTWndProc(HWND hWnd, UINT nMsg,
                                             WPARAM wParam, LPARAM lParam)
{
    wxQTMediaBackend* pThis = (wxQTMediaBackend*)wxGetWindowUserData(hWnd);

    MSG msg;
    msg.hwnd = hWnd;
    msg.message = nMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.time = 0;
    msg.pt.x = 0;
    msg.pt.y = 0;
    EventRecord theEvent;
    pThis->m_lib.NativeEventToMacEvent(&msg, &theEvent);
    pThis->m_lib.MCIsPlayerEvent(pThis->m_pMC, &theEvent);
    return pThis->m_ctrl->MSWWindowProc(nMsg, wParam, lParam);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend Destructor
//
// Sets m_timer to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxQTMediaBackend::wxQTMediaBackend()
: m_movie(NULL), m_bPlaying(false), m_timer(NULL), m_pMC(NULL)
{
}

//---------------------------------------------------------------------------
// wxQTMediaBackend Destructor
//
// 1) Cleans up the QuickTime movie instance
// 2) Decrements the QuickTime reference counter - if this reaches
//    0, QuickTime shuts down
// 3) Decrements the QuickTime Windows Media Layer reference counter -
//    if this reaches 0, QuickTime shuts down the Windows Media Layer
//---------------------------------------------------------------------------
wxQTMediaBackend::~wxQTMediaBackend()
{
    if(m_movie)
        Cleanup();

    if(m_lib.IsOk())
    {
        if(m_pMC)
        {
            m_lib.DisposeMovieController(m_pMC);
        //    m_pMC = NULL;
        }

        m_lib.DestroyPortAssociation(
            (CGrafPtr)m_lib.GetNativeWindowPort(m_ctrl->GetHWND()));

        //Note that ExitMovies() is not necessary, but
        //the docs are fuzzy on whether or not TerminateQTML is
        m_lib.ExitMovies();
        m_lib.TerminateQTML();
    }
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::CreateControl
//
// 1) Intializes QuickTime
// 2) Creates the control window
//---------------------------------------------------------------------------
bool wxQTMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    if(!m_lib.Initialize())
        return false;

    int nError = m_lib.InitializeQTML(0);
    if (nError != noErr)    //-2093 no dll
    {
        wxFAIL_MSG(wxString::Format(wxT("Couldn't Initialize Quicktime-%i"),
                                    nError));
        return false;
    }
    m_lib.EnterMovies();

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxBORDER_NONE,
                            validator, name) )
        return false;


    m_ctrl = ctrl; //assign the control to our member

    // Create a port association for our window so we
    // can use it as a WindowRef
    m_lib.CreatePortAssociation(m_ctrl->GetHWND(), NULL, 0L);

    //Part of a suggestion from Greg Hazel to repaint
    //movie when idle
    m_ctrl->Connect(m_ctrl->GetId(), wxEVT_ERASE_BACKGROUND,
        wxEraseEventHandler(wxQTMediaEvtHandler::OnEraseBackground),
        NULL, (wxEvtHandler*) this);

    // done
    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Load (file version)
//
// 1) Get an FSSpec from the Windows path name
// 2) Open the movie
// 3) Obtain the movie instance from the movie resource
// 4) Close the movie resource
// 5) Finish loading
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxString& fileName)
{
    if(m_movie)
        Cleanup();

    short movieResFile = 0; //= 0 because of annoying VC6 warning
    FSSpec sfFile;

    if (m_lib.NativePathNameToFSSpec ((char*) (const char*) fileName.mb_str(),
                                &sfFile, 0) != noErr)
        return false;

    if (m_lib.OpenMovieFile (&sfFile, &movieResFile, fsRdPerm) != noErr)
        return false;

    short movieResID = 0;
    Str255 movieName;

    OSErr err = m_lib.NewMovieFromFile (
                   &m_movie,
                   movieResFile,
                   &movieResID,
                   movieName,
                   newMovieActive,
                   NULL
                ); //wasChanged

    //m_lib.GetMoviesStickyError() because it may not find the
    //proper codec and play black video and other strange effects,
    //not to mention mess up the dynamic backend loading scheme
    //of wxMediaCtrl - so it just does what the QuickTime player does
    if(err == noErr  && m_lib.GetMoviesStickyError() == noErr)
    {
    m_lib.CloseMovieFile (movieResFile);

        FinishLoad();
        return true;
    }
    else
        return false;
}


//---------------------------------------------------------------------------
// wxQTMediaBackend::PPRMProc (static)
//
// Called when done PrePrerolling the movie.
// Note that in 99% of the cases this does nothing...
// Anyway we set up the loading timer here to tell us when the movie is done
//---------------------------------------------------------------------------
void wxQTMediaBackend::PPRMProc (Movie theMovie, OSErr theErr, void* theRefCon)
{
    wxASSERT( theMovie );
    wxASSERT( theRefCon );
    wxASSERT( theErr == noErr );

    wxQTMediaBackend* pBE = (wxQTMediaBackend*) theRefCon;

    long lTime = pBE->m_lib.GetMovieTime(theMovie,NULL);
    Fixed rate = pBE->m_lib.GetMoviePreferredRate(theMovie);
    pBE->m_lib.PrerollMovie(theMovie,lTime,rate);
    pBE->m_timer = new wxQTLoadTimer(pBE->m_movie, pBE, &pBE->m_lib);
    pBE->m_timer->Start(MOVIE_DELAY);
}


//---------------------------------------------------------------------------
// wxQTMediaBackend::Load (URL Version)
//
// 1) Build an escaped URI from location
// 2) Create a handle to store the URI string
// 3) Put the URI string inside the handle
// 4) Make a QuickTime URL data ref from the handle with the URI in it
// 5) Clean up the URI string handle
// 6) Do some prerolling
// 7) Finish Loading
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxURI& location)
{
    if(m_movie)
        Cleanup();

    wxString theURI = location.BuildURI();

    Handle theHandle = m_lib.NewHandleClear(theURI.length() + 1);
    wxASSERT(theHandle);

    m_lib.BlockMove(theURI.mb_str(), *theHandle, theURI.length() + 1);

    //create the movie from the handle that refers to the URI
    OSErr err = m_lib.NewMovieFromDataRef(&m_movie, newMovieActive |
                                                    newMovieAsyncOK
                                                    /*|newMovieIdleImportOK*/,
                                NULL, theHandle,
                                URLDataHandlerSubType);

    m_lib.DisposeHandle(theHandle);

    if (err == noErr)
    {
        long timeNow;
    Fixed playRate;

        timeNow = m_lib.GetMovieTime(m_movie, NULL);
        wxASSERT(m_lib.GetMoviesError() == noErr);

        playRate = m_lib.GetMoviePreferredRate(m_movie);
        wxASSERT(m_lib.GetMoviesError() == noErr);

        //
        //  Note that the callback here is optional,
        //  but without it PrePrerollMovie can be buggy
        //  (see Apple ml).  Also, some may wonder
        //  why we need this at all - this is because
        //  Apple docs say QuickTime streamed movies
        //  require it if you don't use a Movie Controller,
        //  which we don't by default.
        //
        m_lib.PrePrerollMovie(m_movie, timeNow, playRate,
                              (WXFARPROC)wxQTMediaBackend::PPRMProc,
                              (void*)this);
        return true;
    }
    else
        return false;
}


//---------------------------------------------------------------------------
// wxQTMediaBackend::FinishLoad
//
// 1) Create the movie timer
// 2) Get real size of movie for GetBestSize/sizers
// 3) Set the movie time scale to something usable so that seeking
//    etc.  will work correctly
// 4) Set our Movie Controller to display the movie if it exists,
//    otherwise set the bounds of the Movie
// 5) Refresh parent window
//---------------------------------------------------------------------------
void wxQTMediaBackend::FinishLoad()
{
    // Create the playing/streaming timer
    m_timer = new wxQTPlayTimer(m_movie, (wxQTMediaBackend*) this, &m_lib);
    wxASSERT(m_timer);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);

    //get the real size of the movie
    Rect outRect;
    memset(&outRect, 0, sizeof(Rect)); //for annoying VC6 warning
    m_lib.GetMovieNaturalBoundsRect (m_movie, &outRect);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;

    //
    // Handle the movie GWorld
    //
    if(m_pMC)
    {
        Point thePoint;
        thePoint.h = thePoint.v = 0;
        m_lib.MCSetMovie(m_pMC, m_movie,
               m_lib.GetNativeWindowPort(m_ctrl->GetHandle()),
               thePoint);
        m_lib.MCSetVisible(m_pMC, true);
        m_bestSize.y += 16;
    }
    else
    {
        m_lib.SetMovieGWorld(m_movie,
                       (CGrafPtr) m_lib.GetNativeWindowPort(m_ctrl->GetHWND()),
                       NULL);
    }

    //
    // Set the movie to millisecond precision
    //
    m_lib.SetMovieTimeScale(m_movie, 1000);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a separate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    m_ctrl->SetSize(m_ctrl->GetSize());

    //loaded - note that MoviesTask must and will be called before this
    //by the previous timer since this gets appended to the event list after
    //the timer's first go
    wxMediaEvent theEvent(wxEVT_MEDIA_LOADED,
                            m_ctrl->GetId());
    m_ctrl->AddPendingEvent(theEvent);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Play
//
// 1) Start the QT movie
// 2) Start the movie loading timer
//
// NOTE:  This will still return success even when
// the movie is still loading, and as mentioned in wxQTLoadTimer
// I don't know of a way to force this to be sync - so if its
// still loading the function will return true but the movie will
// still be in the stopped state
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Play()
{
    m_lib.StartMovie(m_movie);
    m_bPlaying = true;
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Pause
//
// 1) Stop the movie
// 2) Stop the movie timer
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Pause()
{
    m_bPlaying = false;
    m_lib.StopMovie(m_movie);
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Stop
//
// 1) Stop the movie
// 2) Stop the movie timer
// 3) Seek to the beginning of the movie
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Stop()
{
    m_bPlaying = false;

    m_lib.StopMovie(m_movie);
    if(m_lib.GetMoviesError() != noErr)
        return false;

    m_lib.GoToBeginningOfMovie(m_movie);
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetPlaybackRate
//
// 1) Get the movie playback rate from ::GetMovieRate
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetPlaybackRate()
{
    return ( ((double)m_lib.GetMovieRate(m_movie)) / 0x10000);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPlaybackRate
//
// 1) Convert dRate to Fixed and Set the movie rate through SetMovieRate
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    m_lib.SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPosition
//
// 1) Create a time record struct (TimeRecord) with appropriate values
// 2) Pass struct to SetMovieTime
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPosition(wxLongLong where)
{
    //NB:  For some reason SetMovieTime does not work
    //correctly with the Quicktime Windows SDK (6)
    //From Muskelkatermann at the wxForum
    //http://www.solidsteel.nl/users/wxwidgets/viewtopic.php?t=2957
    //RN - note that I have not verified this but there
    //is no harm in calling SetMovieTimeValue instead
#if 0
    TimeRecord theTimeRecord;
    memset(&theTimeRecord, 0, sizeof(TimeRecord));
    theTimeRecord.value.lo = where.GetLo();
    theTimeRecord.scale = m_lib.GetMovieTimeScale(m_movie);
    theTimeRecord.base = m_lib.GetMovieTimeBase(m_movie);
    m_lib.SetMovieTime(m_movie, &theTimeRecord);
#else
    m_lib.SetMovieTimeValue(m_movie, where.GetLo());
#endif
    if (m_lib.GetMoviesError() != noErr)
        return false;

    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetPosition
//
// 1) Calls GetMovieTime to get the position we are in in the movie
// in milliseconds (we called
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetPosition()
{
    return m_lib.GetMovieTime(m_movie, NULL);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetVolume
//
// Gets the volume through GetMovieVolume - which returns a 16 bit short -
//
// +--------+--------+
// +   (1)  +   (2)  +
// +--------+--------+
//
// (1) first 8 bits are value before decimal
// (2) second 8 bits are value after decimal
//
// Volume ranges from -1.0 (gain but no sound), 0 (no sound and no gain) to
// 1 (full gain and sound)
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetVolume()
{
    short sVolume = m_lib.GetMovieVolume(m_movie);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    if(sVolume & (128 << 8)) //negative - no sound
        return 0.0;

    return sVolume/256.0;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetVolume
//
// Sets the volume through SetMovieVolume - which takes a 16 bit short -
//
// +--------+--------+
// +   (1)  +   (2)  +
// +--------+--------+
//
// (1) first 8 bits are value before decimal
// (2) second 8 bits are value after decimal
//
// Volume ranges from -1.0 (gain but no sound), 0 (no sound and no gain) to
// 1 (full gain and sound)
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetVolume(double dVolume)
{
    m_lib.SetMovieVolume(m_movie, (short) (dVolume * 256));
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetDuration
//
// Calls GetMovieDuration
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDuration()
{
    return m_lib.GetMovieDuration(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetState
//
// Determines the current state - if we are at the beginning we
// are stopped
//---------------------------------------------------------------------------
wxMediaState wxQTMediaBackend::GetState()
{
    if (m_bPlaying == true)
        return wxMEDIASTATE_PLAYING;
    else if ( !m_movie || wxQTMediaBackend::GetPosition() == 0)
        return wxMEDIASTATE_STOPPED;
    else
        return wxMEDIASTATE_PAUSED;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Cleanup
//
// Diposes of the movie timer, Disassociates the Movie Controller with
// movie and hides it if it exists, and stops and disposes
// of the QT movie
//---------------------------------------------------------------------------
void wxQTMediaBackend::Cleanup()
{
    m_bPlaying = false;

    if(m_timer)
    {
    delete m_timer;
    m_timer = NULL;
    }

    m_lib.StopMovie(m_movie);

    if(m_pMC)
    {
        Point thePoint;
        thePoint.h = thePoint.v = 0;
        m_lib.MCSetVisible(m_pMC, false);
        m_lib.MCSetMovie(m_pMC, NULL, NULL, thePoint);
    }

    m_lib.DisposeMovie(m_movie);
    m_movie = NULL;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::ShowPlayerControls
//
// Creates a movie controller for the Movie if the user wants it
//---------------------------------------------------------------------------
bool wxQTMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if(m_pMC)
    {
        //restore old wndproc
        wxSetWindowProc((HWND)m_ctrl->GetHWND(), wxWndProc);
        m_lib.DisposeMovieController(m_pMC);
        m_pMC = NULL;
        m_bestSize.y -= 16; //movie controller height
    }

    if(flags && m_movie)
    {
        Rect rect;
        wxRect wxrect = m_ctrl->GetClientRect();

        //make room for controller
        if(wxrect.width < 320)
            wxrect.width = 320;

        rect.top = wxrect.y;
        rect.left = wxrect.x;
        rect.right = rect.left + wxrect.width;
        rect.bottom = rect.top + wxrect.height;

        if(!m_pMC)
        {
            m_pMC = m_lib.NewMovieController(m_movie, &rect, mcTopLeftMovie |
                                //                        mcScaleMovieToFit |
                                //                        mcWithBadge |
                                                        mcWithFrame);
            m_lib.MCDoAction(m_pMC, 32, (void*)true); //mcActionSetKeysEnabled
            m_lib.MCSetActionFilterWithRefCon(m_pMC,
                (WXFARPROC)wxQTMediaBackend::MCFilterProc, (void*)this);
            m_bestSize.y += 16; //movie controller height

            //
            // By default the movie controller uses its own color
            // pallette for the movie which can be bad on some files -
            // so turn it off.  Also turn off its frame/border for
            // the movie
            //
            // Also we take care of a couple of the interface flags here
            //
            long mcFlags = 0;
            m_lib.MCDoAction(m_pMC, 39/*mcActionGetFlags*/, (void*)&mcFlags);
            mcFlags |= (  //(1<<0)/*mcFlagSuppressMovieFrame*/ |
                 (1<<3)/*mcFlagsUseWindowPalette*/
                   | ((flags & wxMEDIACTRLPLAYERCONTROLS_STEP)
                      ? 0 : (1<<1)/*mcFlagSuppressStepButtons*/)
                   | ((flags & wxMEDIACTRLPLAYERCONTROLS_VOLUME)
                      ? 0 : (1<<2)/*mcFlagSuppressSpeakerButton*/)
     //              | (1<<4) /*mcFlagDontInvalidate*/ //if we take care of repainting ourselves
                      );
            m_lib.MCDoAction(m_pMC, 38/*mcActionSetFlags*/, (void*)mcFlags);

            //intercept the wndproc of our control window
            wxSetWindowProc((HWND)m_ctrl->GetHWND(),
                wxQTMediaBackend::QTWndProc);

            //set the user data of our window
            wxSetWindowUserData((HWND)m_ctrl->GetHWND(), this);
        }
    }

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a separate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    m_ctrl->SetSize(m_ctrl->GetSize());

    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::MCFilterProc (static)
//
// Callback for when the movie controller recieves a message
//---------------------------------------------------------------------------
Boolean
wxQTMediaBackend::MCFilterProc(MovieController WXUNUSED(theController),
                               short action,
                               void * WXUNUSED(params),
                               LONG_PTR refCon)
{
    if(action != 1) //don't process idle events
    {
        wxQTMediaBackend* pThis = (wxQTMediaBackend*)refCon;

        switch(action)
        {
        case 8: //play button triggered - MC will set movie to opposite state
                //of current - playing ? paused : playing
            pThis->m_bPlaying = !(pThis->m_bPlaying);

            // NB: Sometimes it doesn't redraw properly -
            // if you click on the button but don't move the mouse
            // the button will not change its state until you move
            // mcActionDraw and Refresh/Update combo do nothing
            // to help this unfortunately
            break;
        default:
            break;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetVideoSize
//
// Returns the actual size of the QT movie
//---------------------------------------------------------------------------
wxSize wxQTMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// Sets the bounds of either the Movie or Movie Controller
//---------------------------------------------------------------------------
void wxQTMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y), int w, int h)
{
    if(m_movie)
    {
        //make room for controller
        if(m_pMC)
        {
            if(w < 320)
                w = 320;

            Rect theRect = {0, 0, (short)h, (short)w};
            m_lib.MCSetControllerBoundsRect(m_pMC, &theRect);
        }
        else
        {
            Rect theRect = {0, 0, (short)h, (short)w};
        m_lib.SetMovieBox(m_movie, &theRect);
        }

        wxASSERT(m_lib.GetMoviesError() == noErr);
    }
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::OnEraseBackground
//
// Suggestion from Greg Hazel to repaint the movie when idle
// (on pause also)
//
// TODO:  We may be repainting too much here - under what exact circumstances
// do we need this?  I think Move also repaints correctly for the Movie
// Controller, so in that instance we don't need this either
//---------------------------------------------------------------------------
void wxQTMediaEvtHandler::OnEraseBackground(wxEraseEvent& evt)
{
    wxQTMediaBackend* qtb = (wxQTMediaBackend*)this;
    wxQuickTimeLibrary* m_pLib = &(qtb->m_lib);

    if(qtb->m_pMC)
    {
        //repaint movie controller
        m_pLib->MCDoAction(qtb->m_pMC, 2 /*mcActionDraw*/,
            m_pLib->GetNativeWindowPort(qtb->m_ctrl->GetHWND())
                           );
    }
    else if(qtb->m_movie)
    {
        CGrafPtr port = (CGrafPtr)m_pLib->GetNativeWindowPort
        (qtb->m_ctrl->GetHWND());

        m_pLib->BeginUpdate(port);
        m_pLib->UpdateMovie(qtb->m_movie);
        wxASSERT(m_pLib->GetMoviesError() == noErr);
        m_pLib->EndUpdate(port);
    }

    evt.Skip(); //repaint with window background (TODO: maybe !m_movie?)
}

//---------------------------------------------------------------------------
//  End QT Backend
//---------------------------------------------------------------------------

//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

//---------------------------------------------------------------------------
//  End wxMediaCtrl Compilation Guard and this file
//---------------------------------------------------------------------------
#endif //wxUSE_MEDIACTRL


