/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/mediactrl_am.cpp
// Purpose:     ActiveMovie/WMP6/PocketPC 2000 Media Backend for Windows
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     01/29/05
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO: Actually test HTTP proxies...

//-----------------Introduction----------------------------------------------
// This is the media backend for Windows Media Player 6 and ActiveMovie.
//
// We use a combination of the WMP 6 IMediaPlayer interface as well as the
// ActiveMovie interface IActiveMovie.
//
// For supporting HTTP proxies and such we query the media player
// interface (IActiveMovie) for the INSPlay (NetShow) interface.
//
// The IMediaPlayer/IActiveMovie are rather clean and straightforward
// interfaces that are fairly simplistic.
//
// Docs for IMediaPlayer are at
// http://msdn.microsoft.com/library/en-us/wmp6sdk/htm/microsoftwindowsmediaplayercontrolversion64sdk.asp

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MEDIACTRL && wxUSE_ACTIVEX

#include "wx/mediactrl.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/math.h"        // log10 & pow
    #include "wx/stopwatch.h"
#endif

#include "wx/msw/private.h" // user info and wndproc setting/getting
#include "wx/dynlib.h"

//---------------------------------------------------------------------------
//  wxActiveXContainer - includes all the COM-specific stuff we need
//---------------------------------------------------------------------------
#include "wx/msw/ole/activex.h"

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
//  idl using the midl compiler that comes with visual studio with the /h
//  argument to make it generate stubs (a .h & .c file), then clean up the
//  generated interfaces I want with the STDMETHOD wrappers and then put them
//  into mediactrl.cpp.
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
    amvUninitialized  = 0,
    amvLoading        = 1,
    amvInteractive    = 3,
    amvComplete       = 4
};

enum StateConstants
{
    amvNotLoaded    = -1,
    amvStopped      = 0,
    amvPaused       = 1,
    amvRunning      = 2
};

enum DisplayModeConstants
{
    amvTime      = 0,
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
    STDMETHOD(GetMediaParameterName)(long EntryNum, long Index, BSTR __RPC_FAR *pbstrParameterName) PURE;
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
    STDMETHOD(AboutBox)(void) PURE;
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
//  wxAMMediaBackend
//
//---------------------------------------------------------------------------

typedef BOOL (WINAPI* LPAMGETERRORTEXT)(HRESULT, wxChar *, DWORD);

class WXDLLIMPEXP_MEDIA wxAMMediaBackend : public wxMediaBackendCommonBase
{
public:
    wxAMMediaBackend();
    virtual ~wxAMMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name) wxOVERRIDE;

    virtual bool Play() wxOVERRIDE;
    virtual bool Pause() wxOVERRIDE;
    virtual bool Stop() wxOVERRIDE;

    virtual bool Load(const wxString& fileName) wxOVERRIDE;
    virtual bool Load(const wxURI& location) wxOVERRIDE;
    virtual bool Load(const wxURI& location, const wxURI& proxy) wxOVERRIDE;

    bool DoLoad(const wxString& location);
    void FinishLoad();

    virtual wxMediaState GetState() wxOVERRIDE;

    virtual bool SetPosition(wxLongLong where) wxOVERRIDE;
    virtual wxLongLong GetPosition() wxOVERRIDE;
    virtual wxLongLong GetDuration() wxOVERRIDE;

    virtual void Move(int x, int y, int w, int h) wxOVERRIDE;
    wxSize GetVideoSize() const wxOVERRIDE;

    virtual double GetPlaybackRate() wxOVERRIDE;
    virtual bool SetPlaybackRate(double) wxOVERRIDE;

    virtual double GetVolume() wxOVERRIDE;
    virtual bool SetVolume(double) wxOVERRIDE;

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags) wxOVERRIDE;

    void DoGetDownloadProgress(wxLongLong*, wxLongLong*);
    virtual wxLongLong GetDownloadProgress() wxOVERRIDE
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return progress;
    }
    virtual wxLongLong GetDownloadTotal() wxOVERRIDE
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return total;
    }

    wxActiveXContainer* m_pAX;   // ActiveX host
    IActiveMovie* m_pAM;
    IMediaPlayer* m_pMP;

    IMediaPlayer* GetMP() {return m_pMP;}
    IActiveMovie* GetAM() {return m_pAM;}
    wxSize m_bestSize;  // Cached size

    // Stuff for getting useful debugging strings
#if wxDEBUG_LEVEL
    wxDynamicLibrary m_dllQuartz;
    LPAMGETERRORTEXT m_lpAMGetErrorText;
    wxString GetErrorString(HRESULT hrdsv);
#endif // wxDEBUG_LEVEL
    wxEvtHandler* m_evthandler;

    friend class wxAMMediaEvtHandler;
    wxDECLARE_DYNAMIC_CLASS(wxAMMediaBackend);
};

class WXDLLIMPEXP_MEDIA wxAMMediaEvtHandler : public wxEvtHandler
{
public:
    wxAMMediaEvtHandler(wxAMMediaBackend *amb) :
       m_amb(amb), m_bLoadEventSent(false)
    {
        m_amb->m_pAX->Bind(
            wxEVT_ACTIVEX,
            &wxAMMediaEvtHandler::OnActiveX, this,
            m_amb->m_pAX->GetId()
                              );
    }

    void OnActiveX(wxActiveXEvent& event);

private:
    wxAMMediaBackend *m_amb;
    bool m_bLoadEventSent; // Whether or not FinishLoaded was already called
                           // prevents it being called multiple times

    wxDECLARE_NO_COPY_CLASS(wxAMMediaEvtHandler);
};

//===========================================================================
//  IMPLEMENTATION
//===========================================================================

//---------------------------------------------------------------------------
//
// wxAMMediaBackend
//
//---------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxAMMediaBackend, wxMediaBackend);

//---------------------------------------------------------------------------
// Usual debugging macros
//---------------------------------------------------------------------------
#if wxDEBUG_LEVEL
#define MAX_ERROR_TEXT_LEN 160

// Get the error string for Active Movie
wxString wxAMMediaBackend::GetErrorString(HRESULT hrdsv)
{
    wxChar szError[MAX_ERROR_TEXT_LEN];
    if( m_lpAMGetErrorText != NULL &&
       (*m_lpAMGetErrorText)(hrdsv, szError, MAX_ERROR_TEXT_LEN) == 0)
    {
        return wxString::Format(wxT("DirectShow error \"%s\" \n")
                                     wxT("(numeric %X)\n")
                                     wxT("occurred"),
                                     szError, (int)hrdsv);
    }
    else
    {
        return wxString::Format(wxT("Unknown error \n")
                                     wxT("(numeric %X)\n")
                                     wxT("occurred"),
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
// wxAMMediaBackend Constructor
//---------------------------------------------------------------------------
wxAMMediaBackend::wxAMMediaBackend()
                 :m_pAX(NULL),
                  m_pAM(NULL),
                  m_pMP(NULL),
                  m_bestSize(wxDefaultSize)
{
   m_evthandler = NULL;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend Destructor
//---------------------------------------------------------------------------
wxAMMediaBackend::~wxAMMediaBackend()
{
    if(m_pAX)
    {
        m_pAX->DissociateHandle();
        delete m_pAX;
        m_pAM->Release();

        if (GetMP())
            GetMP()->Release();

        if (m_evthandler)
        {
            m_ctrl->RemoveEventHandler(m_evthandler);
            delete m_evthandler;
        }
    }
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
#if wxDEBUG_LEVEL
    if ( m_dllQuartz.Load(wxT("quartz.dll"), wxDL_VERBATIM) )
    {
        m_lpAMGetErrorText = (LPAMGETERRORTEXT)
                                m_dllQuartz.GetSymbolAorW(wxT("AMGetErrorText"));
    }
#endif // wxDEBUG_LEVEL



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
    m_ctrl = wxStaticCast(ctrl, wxMediaCtrl);
    m_pAX = new wxActiveXContainer(ctrl,
                m_pMP ? IID_IMediaPlayer : IID_IActiveMovie, m_pAM
                                  );
    // Connect for events
    m_evthandler = new wxAMMediaEvtHandler(this);
    m_ctrl->PushEventHandler(m_evthandler);

    //
    //  Here we set up wx-specific stuff for the default
    //  settings wxMediaCtrl says it will stay to
    //
    if(GetMP())
    {
        GetMP()->put_DisplaySize(mpFitToSize);
        // TODO: Unsure what actual effect this has
        // In DirectShow Windowless video results in less delay when
        // dragging, for example - but this doesn't appear to really do anything
        // in practice (it may be something different...)...
        GetMP()->put_WindowlessVideo(VARIANT_TRUE);

    }
    else
        GetAM()->put_MovieWindowSize(amvDoubleOriginalSize);

    // by default true
    GetAM()->put_AutoStart(VARIANT_FALSE);
    // by default enabled
    wxAMMediaBackend::ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_NONE);
    // by default with AM only 0.5
    wxAMMediaBackend::SetVolume(1.0);

    // don't erase the background of our control window so that resizing is a
    // bit smoother
    m_ctrl->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

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
    GetAM()->QueryInterface(IID_INSPlay, (void**) &pPlay);
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
    GetAM()->QueryInterface(IID_INSPlay, (void**) &pPlay);

    if(pPlay)
    {
        pPlay->put_UseHTTPProxy(VARIANT_TRUE);
        pPlay->put_HTTPProxyHost(wxBasicString(proxy.GetServer()));
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
    HRESULT hr;

    // Play the movie the normal way through the embedded
    // WMP.  Supposively Open is better in theory because
    // the docs say its async and put_FileName is not -
    // but in practice they both seem to be async anyway
    if(GetMP())
        hr = GetMP()->Open( wxBasicString(location) );
    else
        hr = GetAM()->put_FileName( wxBasicString(location) );

    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    m_bestSize = wxDefaultSize;
    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::FinishLoad
//
// Called when the media has finished loaded and is ready to play
//
// Here we get the original size of the video and
// send the loaded event to our watcher :).
//---------------------------------------------------------------------------
void wxAMMediaBackend::FinishLoad()
{
    NotifyMovieLoaded();
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::ShowPlayerControls
//---------------------------------------------------------------------------
bool wxAMMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    // Note that IMediaPlayer doesn't have a statusbar by
    // default but IActiveMovie does - so let's try to keep
    // the interface consistent.
    if(!flags)
    {
        GetAM()->put_Enabled(VARIANT_FALSE);
        GetAM()->put_ShowControls(VARIANT_FALSE);
        if(GetMP())
            GetMP()->put_ShowStatusBar(VARIANT_FALSE);
    }
    else
    {
        GetAM()->put_Enabled(VARIANT_TRUE);
        GetAM()->put_ShowControls(VARIANT_TRUE);

        GetAM()->put_ShowPositionControls(
                (flags & wxMEDIACTRLPLAYERCONTROLS_STEP) ?
                VARIANT_TRUE : VARIANT_FALSE);

        if(GetMP())
        {
            GetMP()->put_ShowStatusBar(VARIANT_TRUE);
            GetMP()->put_ShowAudioControls(
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
    // Actually try to play the movie (will fail if not loaded completely)
    HRESULT hr = GetAM()->Run();
    if(SUCCEEDED(hr))
    {
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
    HRESULT hr = GetAM()->Pause();
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
    HRESULT hr = GetAM()->Stop();
    if(SUCCEEDED(hr))
    {
        // Seek to beginning
        wxAMMediaBackend::SetPosition(0);
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
// 2) Sets the play position of the IActiveMovie interface -
//    passing NULL as the stop position means to keep the old
//    stop position
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetPosition(wxLongLong where)
{
    HRESULT hr = GetAM()->put_CurrentPosition(
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
    HRESULT hr = GetAM()->get_CurrentPosition(&outCur);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return 0;
    }

    // h,m,s,milli - outCur is in 1 second (double)
    outCur *= 1000;
    wxLongLong ll;
    ll.Assign(outCur);

    return ll;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetVolume and SetVolume()
//
// Notice that for the IActiveMovie interface value ranges from 0 (MAX volume)
// to -10000 (minimum volume) and the scale is logarithmic in 0.01db per step.
//---------------------------------------------------------------------------

double wxAMMediaBackend::GetVolume()
{
    long lVolume;
    HRESULT hr = GetAM()->get_Volume(&lVolume);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return 0.0;
    }

    double dVolume = lVolume / 2000.; // volume is now in [-5..0] range
    dVolume = pow(10.0, dVolume);     //                 [10^-5, 1]
    dVolume -= 0.00001;               //                [0, 1-10^-5]
    dVolume /= 1 - 0.00001;           //                   [0, 1]

    return dVolume;
}

bool wxAMMediaBackend::SetVolume(double dVolume)
{
    // inverse the transformation above
    long lVolume = static_cast<long>(2000*log10(dVolume + (1 - dVolume)*0.00001));

    HRESULT hr = GetAM()->put_Volume(lVolume);
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
// 1) Obtains the duration of the media from IActiveMovie
// 2) Converts that value to our time base, and returns it
//
// NB: With VBR MP3 files the default DirectShow MP3 render does not
// read the Xing header correctly, resulting in skewed values for duration
// and seeking
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetDuration()
{
    double outDuration;
    HRESULT hr = GetAM()->get_Duration(&outDuration);
    switch ( hr )
    {
        default:
            wxAMLOG(hr);
            // fall through

        case S_FALSE:
            return 0;

        case S_OK:
            // outDuration is in seconds, we need milliseconds
#ifdef wxLongLong_t
            return static_cast<wxLongLong_t>(outDuration * 1000);
#else
            // In principle it's possible to have video of duration greater
            // than ~1193 hours which corresponds LONG_MAX in milliseconds so
            // cast to wxLongLong first and multiply by 1000 only then to avoid
            // the overflow (resulting in maximal duration of ~136 years).
            return wxLongLong(static_cast<long>(outDuration)) * 1000;
#endif
    }
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetState
//
// Returns the cached state
//---------------------------------------------------------------------------
wxMediaState wxAMMediaBackend::GetState()
{
    StateConstants nState;
    HRESULT hr = GetAM()->get_CurrentState(&nState);
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
// the IActiveMovie interface
//---------------------------------------------------------------------------
double wxAMMediaBackend::GetPlaybackRate()
{
    double dRate;
    HRESULT hr = GetAM()->get_Rate(&dRate);
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
    HRESULT hr = GetAM()->put_Rate(dRate);
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
    IUnknown* pFG = NULL;

    HRESULT hr = m_pAM->get_FilterGraph(&pFG);

    // notice that the call above may return S_FALSE and leave pFG NULL
    if(SUCCEEDED(hr) && pFG)
    {
        IAMOpenProgress* pOP = NULL;
        hr = pFG->QueryInterface(IID_IAMOpenProgress, (void**)&pOP);
        if(SUCCEEDED(hr) && pOP)
        {
            LONGLONG
                loadTotal = 0,
                loadProgress = 0;
            hr = pOP->QueryProgress(&loadTotal, &loadProgress);
            pOP->Release();

            if(SUCCEEDED(hr))
            {
                *pLoadProgress = loadProgress;
                *pLoadTotal = loadTotal;
                pFG->Release();
                return;
            }
        }
        pFG->Release();
    }

    *pLoadProgress = 0;
    *pLoadTotal = 0;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetVideoSize
//
// Obtains the cached original video size
//---------------------------------------------------------------------------
wxSize wxAMMediaBackend::GetVideoSize() const
{
    if (m_bestSize == wxDefaultSize)
    {
        wxAMMediaBackend* self = wxConstCast(this, wxAMMediaBackend);
        long w = 0;
        long h = 0;

        self->GetAM()->get_ImageSourceWidth(&w);
        self->GetAM()->get_ImageSourceHeight(&h);

        if (w != 0 && h != 0)
            self->m_bestSize.Set(w, h);
        else
            return wxSize(0,0);
    }

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
// wxAMMediaBackend::OnActiveX
//
// Handle events sent from our activex control (IActiveMovie/IMediaPlayer).
//
// The weird numbers in the switch statement here are "dispatch ids"
// (the numbers in the id field like ( id(xxx) ) ) from amcompat.idl
// and msdxm.idl.
//---------------------------------------------------------------------------
void wxAMMediaEvtHandler::OnActiveX(wxActiveXEvent& event)
{
    // cast to unsigned long to fix narrowing error with case 0xfffffd9f
    // when using clang
    switch (static_cast<unsigned long>(event.GetDispatchId()))
    {
    case 0x00000001: // statechange in IActiveMovie
    case 0x00000bc4: // playstatechange in IMediaPlayer
        if(event.ParamCount() >= 2)
        {
            switch (event[1].GetInteger())
            {
            case 0: // stopping
                if( m_amb->wxAMMediaBackend::GetPosition() ==
                    m_amb->wxAMMediaBackend::GetDuration() )
                {
                    if ( m_amb->SendStopEvent() )
                    {
                        // Seek to beginning of movie
                        m_amb->wxAMMediaBackend::SetPosition(0);

                        // send the event to our child
                        m_amb->QueueFinishEvent();
                    }
                }
                else
                {
                    m_amb->QueueStopEvent();
                }
                break;
            case 1: // pause
                m_amb->QueuePauseEvent();
                break;
            case 2: // play
                m_amb->QueuePlayEvent();
                break;
            default:
                break;
            }
        }
        else
            event.Skip();
        break;

    case 0x00000032: // opencomplete in IActiveMovie
        if(!m_bLoadEventSent)
        {
            m_amb->FinishLoad();
        }
        break;

    case 0xfffffd9f: // readystatechange in IActiveMovie2 and IMediaPlayer
        if(event.ParamCount() >= 1)
        {
            if(event[0].GetInteger() == 0)
            {
                m_bLoadEventSent = false;
            }
            // Originally this was >= 3 here but on 3 we can't get the
            // size of the video (will error) - however on 4
            // it won't play on downloaded things until it is
            // completely downloaded so we use the lesser of two evils...
            else if(event[0].GetInteger() == 3 &&
                !m_bLoadEventSent)
            {
                m_bLoadEventSent = true;
                m_amb->FinishLoad();
            }
        }
        else
            event.Skip();
        break;

    default:
        event.Skip();
        return;
    }
}

//---------------------------------------------------------------------------
// End of wxAMMediaBackend
//---------------------------------------------------------------------------

// Allow the user code to use wxFORCE_LINK_MODULE() to ensure that this object
// file is not discarded by the linker.
#include "wx/link.h"
wxFORCE_LINK_THIS_MODULE(wxmediabackend_am)

#endif // wxUSE_MEDIACTRL && wxUSE_ACTIVEX
