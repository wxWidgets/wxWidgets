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
// Includes
//---------------------------------------------------------------------------
#include "wx/mediactrl.h"

//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

//---------------------------------------------------------------------------
// Externals (somewhere in src/msw/app.cpp)
//---------------------------------------------------------------------------
extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance(void);
#ifdef __WXWINCE__
extern WXDLLIMPEXP_CORE       wxChar *wxCanvasClassName;
#else
extern WXDLLIMPEXP_CORE const wxChar *wxCanvasClassName;
#endif

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxAMMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Compilation guard for DirectShow
//---------------------------------------------------------------------------
#if wxUSE_DIRECTSHOW

//---------------------------------------------------------------------------
//  COM includes
//---------------------------------------------------------------------------
#include "wx/msw/ole/oleutils.h" //wxBasicString, IID etc.
#include "wx/msw/ole/uuid.h" //IID etc..

//---------------------------------------------------------------------------
//  IIDS - used by CoCreateInstance and IUnknown::QueryInterface
//---------------------------------------------------------------------------
const IID LIBID_QuartzTypeLib = {0x56A868B0,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IAMCollection = {0x56A868B9,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaControl = {0x56A868B1,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaEvent = {0x56A868B6,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaEventEx = {0x56A868C0,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaPosition = {0x56A868B2,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IBasicAudio = {0x56A868B3,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IVideoWindow = {0x56A868B4,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IBasicVideo = {0x56A868B5,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IBasicVideo2 = {0x329BB360,0xF6EA,0x11D1,{0x90,0x38,0x00,0xA0,0xC9,0x69,0x72,0x98}};
const IID IID_IDeferredCommand = {0x56A868B8,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IQueueCommand = {0x56A868B7,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IFilterInfo = {0x56A868BA,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IRegFilterInfo = {0x56A868BB,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IMediaTypeInfo = {0x56A868BC,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IPinInfo = {0x56A868BD,0x0AD4,0x11CE,{0xB0,0x3A,0x00,0x20,0xAF,0x0B,0xA7,0x70}};
const IID IID_IAMStats = {0xBC9BCF80,0xDCD2,0x11D2,{0xAB,0xF6,0x00,0xA0,0xC9,0x05,0xF3,0x75}};

//TODO:  These 4 lines needed?
#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

//COM Class Factory
const CLSID CLSID_FilgraphManager = {0xE436EBB3,0x524F,0x11CE,{0x9F,0x53,0x00,0x20,0xAF,0x0B,0xA7,0x70}};

//---------------------------------------------------------------------------
//  COM INTERFACES (dumped from midl from quartz.idl from MSVC COM Browser)
//---------------------------------------------------------------------------
MIDL_INTERFACE("56A868B9-0AD4-11CE-B03A-0020AF0BA770")
IAMCollection : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
        /* [retval][out] */ long __RPC_FAR *plCount) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
        /* [in] */ long lItem,
        /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
        /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
};

MIDL_INTERFACE("56A868B1-0AD4-11CE-B03A-0020AF0BA770")
IMediaControl : public IDispatch
{
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Run( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetState( 
        /* [in] */ long msTimeout,
        /* [out] */ long __RPC_FAR *pfs) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE RenderFile( 
        /* [in] */ BSTR strFilename) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddSourceFilter( 
        /* [in] */ BSTR strFilename,
        /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FilterCollection( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RegFilterCollection( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE StopWhenReady( void) = 0;
    
};

MIDL_INTERFACE("56A868B6-0AD4-11CE-B03A-0020AF0BA770")
IMediaEvent : public IDispatch
{
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetEventHandle( 
        /* [out] */ LONG_PTR __RPC_FAR *hEvent) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetEvent( 
        /* [out] */ long __RPC_FAR *lEventCode,
        /* [out] */ LONG_PTR __RPC_FAR *lParam1,
        /* [out] */ LONG_PTR __RPC_FAR *lParam2,
        /* [in] */ long msTimeout) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE WaitForCompletion( 
        /* [in] */ long msTimeout,
        /* [out] */ long __RPC_FAR *pEvCode) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CancelDefaultHandling( 
        /* [in] */ long lEvCode) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE RestoreDefaultHandling( 
        /* [in] */ long lEvCode) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE FreeEventParams( 
        /* [in] */ long lEvCode,
        /* [in] */ LONG_PTR lParam1,
        /* [in] */ LONG_PTR lParam2) = 0;
    
};

MIDL_INTERFACE("56A868C0-0AD4-11CE-B03A-0020AF0BA770")
IMediaEventEx : public IMediaEvent
{
public:
    virtual HRESULT __stdcall SetNotifyWindow( 
        /* [in] */ LONG_PTR hwnd,
        /* [in] */ long lMsg,
        /* [in] */ LONG_PTR lInstanceData) = 0;
    
    virtual HRESULT __stdcall SetNotifyFlags( 
        /* [in] */ long lNoNotifyFlags) = 0;
    
    virtual HRESULT __stdcall GetNotifyFlags( 
        /* [out] */ long __RPC_FAR *lplNoNotifyFlags) = 0;
    
};
    
MIDL_INTERFACE("56A868B2-0AD4-11CE-B03A-0020AF0BA770")
IMediaPosition : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Duration( 
        /* [retval][out] */ double __RPC_FAR *plength) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
        /* [in] */ double pllTime) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
        /* [retval][out] */ double __RPC_FAR *pllTime) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_StopTime( 
        /* [retval][out] */ double __RPC_FAR *pllTime) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_StopTime( 
        /* [in] */ double pllTime) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_PrerollTime( 
        /* [retval][out] */ double __RPC_FAR *pllTime) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_PrerollTime( 
        /* [in] */ double pllTime) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Rate( 
        /* [in] */ double pdRate) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rate( 
        /* [retval][out] */ double __RPC_FAR *pdRate) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CanSeekForward( 
        /* [retval][out] */ long __RPC_FAR *pCanSeekForward) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CanSeekBackward( 
        /* [retval][out] */ long __RPC_FAR *pCanSeekBackward) = 0;
    
};

MIDL_INTERFACE("56A868B3-0AD4-11CE-B03A-0020AF0BA770")
IBasicAudio : public IDispatch
{
public:
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Volume( 
        /* [in] */ long plVolume) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Volume( 
        /* [retval][out] */ long __RPC_FAR *plVolume) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Balance( 
        /* [in] */ long plBalance) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Balance( 
        /* [retval][out] */ long __RPC_FAR *plBalance) = 0;
    
};
    
MIDL_INTERFACE("56A868B4-0AD4-11CE-B03A-0020AF0BA770")
IVideoWindow : public IDispatch
{
public:
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Caption( 
        /* [in] */ BSTR strCaption) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Caption( 
        /* [retval][out] */ BSTR __RPC_FAR *strCaption) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowStyle( 
        /* [in] */ long WindowStyle) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowStyle( 
        /* [retval][out] */ long __RPC_FAR *WindowStyle) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowStyleEx( 
        /* [in] */ long WindowStyleEx) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowStyleEx( 
        /* [retval][out] */ long __RPC_FAR *WindowStyleEx) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoShow( 
        /* [in] */ long AutoShow) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoShow( 
        /* [retval][out] */ long __RPC_FAR *AutoShow) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
        /* [in] */ long WindowState) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
        /* [retval][out] */ long __RPC_FAR *WindowState) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BackgroundPalette( 
        /* [in] */ long pBackgroundPalette) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BackgroundPalette( 
        /* [retval][out] */ long __RPC_FAR *pBackgroundPalette) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
        /* [in] */ long pVisible) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
        /* [retval][out] */ long __RPC_FAR *pVisible) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Left( 
        /* [in] */ long pLeft) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Left( 
        /* [retval][out] */ long __RPC_FAR *pLeft) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
        /* [in] */ long pWidth) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
        /* [retval][out] */ long __RPC_FAR *pWidth) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Top( 
        /* [in] */ long pTop) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Top( 
        /* [retval][out] */ long __RPC_FAR *pTop) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
        /* [in] */ long pHeight) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
        /* [retval][out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Owner( 
        /* [in] */ LONG_PTR Owner) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Owner( 
        /* [retval][out] */ LONG_PTR __RPC_FAR *Owner) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_MessageDrain( 
        /* [in] */ LONG_PTR Drain) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_MessageDrain( 
        /* [retval][out] */ LONG_PTR __RPC_FAR *Drain) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
        /* [retval][out] */ long __RPC_FAR *Color) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
        /* [in] */ long Color) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FullScreenMode( 
        /* [retval][out] */ long __RPC_FAR *FullScreenMode) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FullScreenMode( 
        /* [in] */ long FullScreenMode) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetWindowForeground( 
        /* [in] */ long Focus) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE NotifyOwnerMessage( 
        /* [in] */ LONG_PTR hwnd,
        /* [in] */ long uMsg,
        /* [in] */ LONG_PTR wParam,
        /* [in] */ LONG_PTR lParam) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetWindowPosition( 
        /* [in] */ long Left,
        /* [in] */ long Top,
        /* [in] */ long Width,
        /* [in] */ long Height) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetWindowPosition( 
        /* [out] */ long __RPC_FAR *pLeft,
        /* [out] */ long __RPC_FAR *pTop,
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMinIdealImageSize( 
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMaxIdealImageSize( 
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetRestorePosition( 
        /* [out] */ long __RPC_FAR *pLeft,
        /* [out] */ long __RPC_FAR *pTop,
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE HideCursor( 
        /* [in] */ long HideCursor) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsCursorHidden( 
        /* [out] */ long __RPC_FAR *CursorHidden) = 0;
    
};

MIDL_INTERFACE("56A868B5-0AD4-11CE-B03A-0020AF0BA770")
IBasicVideo : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AvgTimePerFrame( 
        /* [retval][out] */ double __RPC_FAR *pAvgTimePerFrame) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BitRate( 
        /* [retval][out] */ long __RPC_FAR *pBitRate) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BitErrorRate( 
        /* [retval][out] */ long __RPC_FAR *pBitErrorRate) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_VideoWidth( 
        /* [retval][out] */ long __RPC_FAR *pVideoWidth) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_VideoHeight( 
        /* [retval][out] */ long __RPC_FAR *pVideoHeight) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SourceLeft( 
        /* [in] */ long pSourceLeft) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceLeft( 
        /* [retval][out] */ long __RPC_FAR *pSourceLeft) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SourceWidth( 
        /* [in] */ long pSourceWidth) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceWidth( 
        /* [retval][out] */ long __RPC_FAR *pSourceWidth) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SourceTop( 
        /* [in] */ long pSourceTop) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceTop( 
        /* [retval][out] */ long __RPC_FAR *pSourceTop) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_SourceHeight( 
        /* [in] */ long pSourceHeight) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceHeight( 
        /* [retval][out] */ long __RPC_FAR *pSourceHeight) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DestinationLeft( 
        /* [in] */ long pDestinationLeft) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DestinationLeft( 
        /* [retval][out] */ long __RPC_FAR *pDestinationLeft) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DestinationWidth( 
        /* [in] */ long pDestinationWidth) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DestinationWidth( 
        /* [retval][out] */ long __RPC_FAR *pDestinationWidth) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DestinationTop( 
        /* [in] */ long pDestinationTop) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DestinationTop( 
        /* [retval][out] */ long __RPC_FAR *pDestinationTop) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DestinationHeight( 
        /* [in] */ long pDestinationHeight) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DestinationHeight( 
        /* [retval][out] */ long __RPC_FAR *pDestinationHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetSourcePosition( 
        /* [in] */ long Left,
        /* [in] */ long Top,
        /* [in] */ long Width,
        /* [in] */ long Height) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSourcePosition( 
        /* [out] */ long __RPC_FAR *pLeft,
        /* [out] */ long __RPC_FAR *pTop,
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetDefaultSourcePosition( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetDestinationPosition( 
        /* [in] */ long Left,
        /* [in] */ long Top,
        /* [in] */ long Width,
        /* [in] */ long Height) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDestinationPosition( 
        /* [out] */ long __RPC_FAR *pLeft,
        /* [out] */ long __RPC_FAR *pTop,
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetDefaultDestinationPosition( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoSize( 
        /* [out] */ long __RPC_FAR *pWidth,
        /* [out] */ long __RPC_FAR *pHeight) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoPaletteEntries( 
        /* [in] */ long StartIndex,
        /* [in] */ long Entries,
        /* [out] */ long __RPC_FAR *pRetrieved,
        /* [out] */ long __RPC_FAR *pPalette) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCurrentImage( 
        /* [out][in] */ long __RPC_FAR *pBufferSize,
        /* [out] */ long __RPC_FAR *pDIBImage) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsUsingDefaultSource( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsUsingDefaultDestination( void) = 0;
    
};
    
MIDL_INTERFACE("329BB360-F6EA-11D1-9038-00A0C9697298")
IBasicVideo2 : public IBasicVideo
{
public:
    virtual HRESULT __stdcall GetPreferredAspectRatio( 
        /* [out] */ long __RPC_FAR *plAspectX,
        /* [out] */ long __RPC_FAR *plAspectY) = 0;
    
};
    
MIDL_INTERFACE("56A868B8-0AD4-11CE-B03A-0020AF0BA770")
IDeferredCommand : public IUnknown
{
public:
    virtual HRESULT __stdcall Cancel( void) = 0;
    
    virtual HRESULT __stdcall Confidence( 
        /* [out] */ long __RPC_FAR *pConfidence) = 0;
    
    virtual HRESULT __stdcall Postpone( 
        /* [in] */ double newtime) = 0;
    
    virtual HRESULT __stdcall GetHResult( 
        /* [out] */ HRESULT __RPC_FAR *phrResult) = 0;
    
};

MIDL_INTERFACE("56A868B7-0AD4-11CE-B03A-0020AF0BA770")
IQueueCommand : public IUnknown
{
public:
    virtual HRESULT __stdcall InvokeAtStreamTime( 
        /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
        /* [in] */ double time,
        /* [in] */ GUID __RPC_FAR *iid,
        /* [in] */ long dispidMethod,
        /* [in] */ short wFlags,
        /* [in] */ long cArgs,
        /* [in] */ VARIANT __RPC_FAR *pDispParams,
        /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
        /* [out] */ short __RPC_FAR *puArgErr) = 0;
    
    virtual HRESULT __stdcall InvokeAtPresentationTime( 
        /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
        /* [in] */ double time,
        /* [in] */ GUID __RPC_FAR *iid,
        /* [in] */ long dispidMethod,
        /* [in] */ short wFlags,
        /* [in] */ long cArgs,
        /* [in] */ VARIANT __RPC_FAR *pDispParams,
        /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
        /* [out] */ short __RPC_FAR *puArgErr) = 0;
    
};
    
MIDL_INTERFACE("56A868BA-0AD4-11CE-B03A-0020AF0BA770")
IFilterInfo : public IDispatch
{
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindPin( 
        /* [in] */ BSTR strPinID,
        /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
        /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_VendorInfo( 
        /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Filter( 
        /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Pins( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IsFileSource( 
        /* [retval][out] */ long __RPC_FAR *pbIsSource) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Filename( 
        /* [retval][out] */ BSTR __RPC_FAR *pstrFilename) = 0;
    
    virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Filename( 
        /* [in] */ BSTR pstrFilename) = 0;
    
};
    
MIDL_INTERFACE("56A868BB-0AD4-11CE-B03A-0020AF0BA770")
IRegFilterInfo : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
        /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Filter( 
        /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
};
    
MIDL_INTERFACE("56A868BC-0AD4-11CE-B03A-0020AF0BA770")
IMediaTypeInfo : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
        /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Subtype( 
        /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
    
};
    
MIDL_INTERFACE("56A868BD-0AD4-11CE-B03A-0020AF0BA770")
IPinInfo : public IDispatch
{
public:
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Pin( 
        /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ConnectedTo( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ConnectionMediaType( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FilterInfo( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
        /* [retval][out] */ BSTR __RPC_FAR *ppUnk) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Direction( 
        /* [retval][out] */ long __RPC_FAR *ppDirection) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_PinID( 
        /* [retval][out] */ BSTR __RPC_FAR *strPinID) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_MediaTypes( 
        /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Connect( 
        /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectDirect( 
        /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectWithType( 
        /* [in] */ IUnknown __RPC_FAR *pPin,
        /* [in] */ IDispatch __RPC_FAR *pMediaType) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Render( void) = 0;
    
};

MIDL_INTERFACE("BC9BCF80-DCD2-11D2-ABF6-00A0C905F375")
IAMStats : public IDispatch
{
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
    
    virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
        /* [retval][out] */ long __RPC_FAR *plCount) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValueByIndex( 
        /* [in] */ long lIndex,
        /* [out] */ BSTR __RPC_FAR *szName,
        /* [out] */ long __RPC_FAR *lCount,
        /* [out] */ double __RPC_FAR *dLast,
        /* [out] */ double __RPC_FAR *dAverage,
        /* [out] */ double __RPC_FAR *dStdDev,
        /* [out] */ double __RPC_FAR *dMin,
        /* [out] */ double __RPC_FAR *dMax) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValueByName( 
        /* [in] */ BSTR szName,
        /* [out] */ long __RPC_FAR *lIndex,
        /* [out] */ long __RPC_FAR *lCount,
        /* [out] */ double __RPC_FAR *dLast,
        /* [out] */ double __RPC_FAR *dAverage,
        /* [out] */ double __RPC_FAR *dStdDev,
        /* [out] */ double __RPC_FAR *dMin,
        /* [out] */ double __RPC_FAR *dMax) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIndex( 
        /* [in] */ BSTR szName,
        /* [in] */ long lCreate,
        /* [out] */ long __RPC_FAR *plIndex) = 0;
    
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddValue( 
        /* [in] */ long lIndex,
        /* [in] */ double dValue) = 0;
    
};

//------------------------------------------------------------------
// wxAMMediaBackend (Active Movie)
//------------------------------------------------------------------
class WXDLLIMPEXP_MEDIA wxAMMediaBackend : public wxMediaBackend
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
    virtual bool SetPlaybackRate(double);

    void Cleanup();

    bool m_bVideo;

    static LRESULT CALLBACK NotifyWndProc(HWND hWnd, UINT nMsg,
                                          WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                          WPARAM wParam, LPARAM lParam);

    wxControl* m_ctrl;

    IBasicAudio* m_pBA;
    IBasicVideo* m_pBV;
    IMediaControl* m_pMC;
    IMediaEventEx* m_pME;
    IMediaPosition* m_pMS;
    IVideoWindow* m_pVW;

    HWND m_hNotifyWnd;
    wxSize m_bestSize;

    DECLARE_DYNAMIC_CLASS(wxAMMediaBackend);
};

#endif //wxUSE_DIRECTSHOW

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
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  QT Compilation Guard
//---------------------------------------------------------------------------
#if wxUSE_QUICKTIME

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
#define Ptr char* 
#define Handle Ptr*
#define Fixed long
#define OSType unsigned long
#define CGrafPtr struct GrafPort *                      
#define TimeScale long
#define TimeBase struct TimeBaseRecord *                

struct FSSpec {
    short                           vRefNum;
    long                            parID;
    Str255                           name;  /*Str63 on mac, Str255 on msw */
};

struct Rect {
    short                           top;
    short                           left;
    short                           bottom;
    short                           right;
};

struct wide {
    wxInt32                          hi;
    wxUint32                          lo;
};

struct TimeRecord {
    wide                  value;                      /* units */
    TimeScale                       scale;                      /* units per second */
    TimeBase                        base;
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
    if (!success) return false;

//Class that utilizes Robert Roeblings Dynamic Library Macros
class wxQuickTimeLibrary 
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
};

bool wxQuickTimeLibrary::Initialize()
{
    m_ok = false;

    if(!m_dll.Load(wxT("qtmlClient.dll")))
        return false;

    bool bOk;

    wxDL_METHOD_LOAD( m_dll, StartMovie, bOk );
    wxDL_METHOD_LOAD( m_dll, StopMovie, bOk );
    wxDL_METHOD_LOAD( m_dll, IsMovieDone, bOk );
    wxDL_METHOD_LOAD( m_dll, GoToBeginningOfMovie, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMoviesError, bOk );
    wxDL_METHOD_LOAD( m_dll, EnterMovies, bOk );
    wxDL_METHOD_LOAD( m_dll, ExitMovies, bOk );
    wxDL_METHOD_LOAD( m_dll, InitializeQTML, bOk );
    wxDL_METHOD_LOAD( m_dll, TerminateQTML, bOk );
    wxDL_METHOD_LOAD( m_dll, NativePathNameToFSSpec, bOk );
    wxDL_METHOD_LOAD( m_dll, OpenMovieFile, bOk );
    wxDL_METHOD_LOAD( m_dll, CloseMovieFile, bOk );
    wxDL_METHOD_LOAD( m_dll, NewMovieFromFile, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieRate, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieRate, bOk );
    wxDL_METHOD_LOAD( m_dll, MoviesTask, bOk );
    wxDL_METHOD_LOAD( m_dll, BlockMove, bOk );
    wxDL_METHOD_LOAD( m_dll, NewHandleClear, bOk );
    wxDL_METHOD_LOAD( m_dll, NewMovieFromDataRef, bOk );
    wxDL_METHOD_LOAD( m_dll, DisposeHandle, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieNaturalBoundsRect, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieIndTrackType, bOk );
    wxDL_METHOD_LOAD( m_dll, CreatePortAssociation, bOk );
    wxDL_METHOD_LOAD( m_dll, GetNativeWindowPort, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieGWorld, bOk );
    wxDL_METHOD_LOAD( m_dll, DisposeMovie, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieBox, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieTimeScale, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieDuration, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieTimeBase, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieTimeScale, bOk );
    wxDL_METHOD_LOAD( m_dll, GetMovieTime, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieTime, bOk );
    
    m_ok = true;

    return true;
}

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

    void Cleanup();
    void FinishLoad();

    wxSize m_bestSize;              //Original movie size
    Movie m_movie;    //QT Movie handle/instance
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    class _wxQTTimer* m_timer;      //Timer for streaming the movie
    wxQuickTimeLibrary m_lib;


    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend);
};

//---------------------------------------------------------------------------
//  End QT Compilation Guard
//---------------------------------------------------------------------------
#endif //wxUSE_QUICKTIME

//===========================================================================
//  IMPLEMENTATION
//===========================================================================

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxAMMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Only use if user wants it -
//---------------------------------------------------------------------------
#if wxUSE_DIRECTSHOW

IMPLEMENT_DYNAMIC_CLASS(wxAMMediaBackend, wxMediaBackend);

// Numerical value for when the graph reaches the stop position
#define WM_GRAPHNOTIFY  WM_USER+13

//---------------------------------------------------------------------------
// Usual debugging macros
//---------------------------------------------------------------------------
#ifdef __WXDEBUG__
#define wxAMVERIFY(x) \
{ \
    HRESULT hrdsv = (x); \
    if ( FAILED(hrdsv) ) \
    { \
        /*TCHAR szError[MAX_ERROR_TEXT_LEN];*/ \
        /*if( AMGetErrorText(hrdsv, szError, MAX_ERROR_TEXT_LEN) == 0)*/ \
        /*{*/ \
            /*wxFAIL_MSG( wxString::Format(wxT("DirectShow error \"%s\" ")*/\
                                         /*wxT("occured at line %i in ")*/ \
                                         /*wxT("mediactrl.cpp"),*/ \
                                            /*szError, __LINE__) );*/ \
        /*}*/ \
        /*else*/ \
            wxFAIL_MSG( wxString::Format(wxT("Unknown error (%i) ") \
                                         wxT("occured at") \
                                         wxT(" line %i in mediactrl.cpp."), \
                                         (int)hrdsv, __LINE__) ); \
    } \
}
#define wxVERIFY(x) wxASSERT((x))
#else
#define wxAMVERIFY(x) (x)
#define wxVERIFY(x) (x)
#endif

//---------------------------------------------------------------------------
// Standard macros for ease of use
//---------------------------------------------------------------------------
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

//---------------------------------------------------------------------------
// wxAMMediaBackend Constructor
//
// Sets m_hNotifyWnd to NULL to signify that we haven't loaded anything yet
//---------------------------------------------------------------------------
wxAMMediaBackend::wxAMMediaBackend() : m_hNotifyWnd(NULL)
{
}

//---------------------------------------------------------------------------
// wxAMMediaBackend Destructor
//
// Cleans up everything
//---------------------------------------------------------------------------
wxAMMediaBackend::~wxAMMediaBackend()
{
    if (m_hNotifyWnd)
        Cleanup();
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::CreateControl
//
// ActiveMovie does not really have any native control to speak of,
// so we just create a normal control.
//
// We also check to see if ActiveMovie is installed
//---------------------------------------------------------------------------
bool wxAMMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    //create our filter graph - the beuty of COM is that it loads
    //quartz.dll for us :)
    HRESULT hr = CoCreateInstance(CLSID_FilgraphManager, NULL, CLSCTX_INPROC_SERVER,
                      IID_IMediaControl, (void**)&m_pMC);

   //directshow not installed?
    if ( FAILED(hr) )
        return false;

    //release the filter graph - we don't need it yet
    m_pMC->Release();
    m_pMC = NULL;

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
// wxAMMediaBackend::Load (file version)
//
// Creates an Active Movie filter graph from a file or url
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxString& fileName)
{
    //if previously loaded cleanup
    if(m_hNotifyWnd)
        Cleanup();

    //We already checked for success in CreateControl
    CoCreateInstance(CLSID_FilgraphManager, NULL, CLSCTX_INPROC_SERVER,
                      IID_IMediaControl, (void**)&m_pMC);

    //load the graph & render
    if( FAILED(m_pMC->RenderFile(wxBasicString(fileName).Get())) )
        return false;

    //get the interfaces, all of them
    wxAMVERIFY( m_pMC->QueryInterface(IID_IMediaEventEx, (void**)&m_pME) );
    wxAMVERIFY( m_pMC->QueryInterface(IID_IMediaPosition, (void**)&m_pMS) );
    wxAMVERIFY( m_pMC->QueryInterface(IID_IVideoWindow, (void**)&m_pVW) );
    wxAMVERIFY( m_pMC->QueryInterface(IID_IBasicAudio, (void**)&m_pBA) );
    wxAMVERIFY( m_pMC->QueryInterface(IID_IBasicVideo, (void**)&m_pBV) );

    //We could tell if the media has audio or not by
    //something like
    //-----
    //long lVolume;
    //pBA->get_Volume(&lVolume) == E_NOTIMPL
    //-----
    //here...

    //
    //Obtain the _actual_ size of the movie & remember it
    //
    long    nX,
            nY;

    m_bestSize.x = m_bestSize.y = 0;

    m_bVideo = SUCCEEDED( m_pVW->GetWindowPosition( &nX,
                                                    &nY,
                                                    (long*)&m_bestSize.x,
                                                    (long*)&m_bestSize.y) );

    //
    //If we have video in the media - set it up so that
    //its a child window of the control, its visible,
    //and that the control is the owner of the video window
    //
    if (m_bVideo)
    {
        wxAMVERIFY( m_pVW->put_Owner((LONG_PTR)m_ctrl->GetHandle()) );
        wxAMVERIFY( m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS) );
        wxAMVERIFY( m_pVW->put_Visible(-1) ); //OATRUE == -1
    }

    //
    // Create a hidden window and register to handle
    // directshow events for this graph
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
                       wxT("registering for DirectShow events!")  );

        return false;
    }

    ::SetWindowLongPtr(m_hNotifyWnd, GWLP_WNDPROC,
                       (LONG_PTR)wxAMMediaBackend::NotifyWndProc);

    ::SetWindowLong(m_hNotifyWnd, GWL_USERDATA,
                       (LONG) this);

    wxAMVERIFY( m_pME->SetNotifyWindow((LONG_PTR)m_hNotifyWnd,
                                       WM_GRAPHNOTIFY, 0) );

    //
    // Force the parent window of this control to recalculate
    // the size of this if sizers are being used
    // and render the results immediately
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (URL Version)
//
// Loads media from a URL.  Interestingly enough DirectShow
// appears (?) to escape the URL for us, at least on normal
// files
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxURI& location)
{
    return Load(location.BuildUnescapedURI());
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Play
//
// Plays the stream.  If it is non-seekable, it will restart it.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Play()
{
    return SUCCEEDED( m_pMC->Run() );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Pause
//
// Pauses the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Pause()
{
    return SUCCEEDED( m_pMC->Pause() );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Stop
//
// Stops the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Stop()
{
    bool bOK = SUCCEEDED( m_pMC->Stop() );

    //We don't care if it can't get to the beginning in directshow -
    //it could be a non-seeking filter (wince midi) in which case playing
    //starts all over again
    SetPosition(0);
    return bOK;
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
    return SUCCEEDED( m_pMS->put_CurrentPosition(
                        ((LONGLONG)where.GetValue()) / 1000
                                                ) 
                    );
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
    wxAMVERIFY( m_pMS->get_CurrentPosition(&outCur) );

    //h,m,s,milli - outdur is in 1 second (double)
    return (outCur*1000);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetDuration
//
// 1) Obtains the duration of the media from the IMediaSeeking interface
// 2) Converts that value to our time base, and returns it
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetDuration()
{
    double outDuration;
    wxAMVERIFY( m_pMS->get_Duration(&outDuration) );

    //h,m,s,milli - outdur is in 1 second (double)
    return (outDuration*1000);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetState
//
// Obtains the state from the IMediaControl interface.
// Note that it's enumeration values for stopping/playing
// etc. are the same as ours, so we just do a straight cast.
// TODO: MS recommends against INFINITE here for
//       IMediaControl::GetState- do it in stages
//---------------------------------------------------------------------------
wxMediaState wxAMMediaBackend::GetState()
{
    HRESULT hr;
    long theState; //OAFilterState
    hr = m_pMC->GetState(INFINITE, &theState);

    wxASSERT( SUCCEEDED(hr) );

    //MSW state is the same as ours
    //State_Stopped   = 0,
    //State_Paused    = State_Stopped + 1,
    //State_Running   = State_Paused + 1

    return (wxMediaState) theState;
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
    wxAMVERIFY( m_pMS->get_Rate(&dRate) );
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
    return SUCCEEDED( m_pMS->put_Rate(dRate) );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::NotifyWndProc
//
// Here we check to see if DirectShow tells us we've reached the stop
// position in our stream - if it has, it may not actually stop
// the stream - which we need to do...
//---------------------------------------------------------------------------
LRESULT CALLBACK wxAMMediaBackend::NotifyWndProc(HWND hWnd, UINT nMsg,
                                                    WPARAM wParam,
                                                    LPARAM lParam)
{
    wxAMMediaBackend* backend = (wxAMMediaBackend*)
        ::GetWindowLong(hWnd, GWL_USERDATA);

    return backend->OnNotifyWndProc(hWnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK wxAMMediaBackend::OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                                    WPARAM wParam,
                                                    LPARAM lParam)
{
    if (nMsg == WM_GRAPHNOTIFY)
    {
        LONG    evCode,
                evParam1,
                evParam2;

        //
        // DirectShow keeps a list of queued events, and we need
        // to go through them one by one, stopping at (Hopefully only one)
        // EC_COMPLETE message
        //
        while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                                       (LONG_PTR *) &evParam2, 0)
                       )
             )
        {
            // Cleanup memory that GetEvent allocated
            wxAMVERIFY( m_pME->FreeEventParams(evCode, evParam1, evParam2) );

            // If this is the end of the clip, notify handler
            if(1 == evCode) //EC_COMPLETE
            {
                //send the event to our child
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP, m_ctrl->GetId());
                m_ctrl->ProcessEvent(theEvent);

                //if the user didn't veto it, stop the stream
                if (theEvent.IsAllowed())
                {
                    //Interestingly enough, DirectShow does not actually stop
                    //the filters - even when it reaches the end!
                    wxVERIFY( Stop() );

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                          m_ctrl->GetId());
                    m_ctrl->ProcessEvent(theEvent);
                }
            }
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Cleanup
//
// 1) Hide/disowns the video window (MS says bad things will happen if
//    you don't)
// 2) Releases all the directshow interfaces we use
// TODO: Maybe there's a way to redirect the IMediaControl each time
//       we load, rather then creating and destroying the interfaces
//       each time?
//---------------------------------------------------------------------------
void wxAMMediaBackend::Cleanup()
{
    // Hide then disown the window
    if(m_pVW)
    {
        m_pVW->put_Visible(0); //OSFALSE == 0
        m_pVW->put_Owner(NULL);
    }

    // Release and zero DirectShow interfaces
    SAFE_RELEASE(m_pME);
    SAFE_RELEASE(m_pMS);
    SAFE_RELEASE(m_pBA);
    SAFE_RELEASE(m_pBV);
    SAFE_RELEASE(m_pVW);
    SAFE_RELEASE(m_pMC);

    // Get rid of our hidden Window
    DestroyWindow(m_hNotifyWnd);
    m_hNotifyWnd = NULL;
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
// Resizes the IVideoWindow to the size of the control window
//---------------------------------------------------------------------------
void wxAMMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y), int w, int h)
{
    if(m_hNotifyWnd && m_bVideo)
    {
        wxAMVERIFY( m_pVW->SetWindowPosition(0, 0, w, h) );
    }
}

//---------------------------------------------------------------------------
// End of wxAMMediaBackend
//---------------------------------------------------------------------------
#endif //wxUSE_DIRECTSHOW

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
                       wxT("registering for DirectShow events!")  );

        return false;
    }

    ::SetWindowLong(m_hNotifyWnd, GWL_WNDPROC,
                       (LONG)wxMCIMediaBackend::NotifyWndProc);

    ::SetWindowLong(m_hNotifyWnd, GWL_USERDATA,
                       (LONG) this);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a seperate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

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
        putParms.rc.top = 0;
        putParms.rc.bottom = 0;
        putParms.rc.right = w;
        putParms.rc.bottom = h;

        wxMCIVERIFY( mciSendCommand(m_hDev, MCI_PUT,
                                   0x00040000L, //MCI_DGV_PUT_DESTINATION
                                   (DWORD)(LPSTR)&putParms) );
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
    wxMCIMediaBackend* backend = (wxMCIMediaBackend*)
        ::GetWindowLong(hWnd, GWL_USERDATA);
    wxASSERT(backend);

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
// TODO: Dynamically load from qtml.dll
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if wxUSE_QUICKTIME

IMPLEMENT_DYNAMIC_CLASS(wxQTMediaBackend, wxMediaBackend);

//Time between timer calls
#define MOVIE_DELAY 100

#include "wx/timer.h"

// --------------------------------------------------------------------------
//          wxQTTimer - Handle Asyncronous Playing
// --------------------------------------------------------------------------
class _wxQTTimer : public wxTimer
{
public:
    _wxQTTimer(Movie movie, wxQTMediaBackend* parent, wxQuickTimeLibrary* pLib) :
        m_movie(movie), m_bPaused(false), m_parent(parent), m_pLib(pLib)
    {
    }

    ~_wxQTTimer()
    {
    }

    bool GetPaused() {return m_bPaused;}
    void SetPaused(bool bPaused) {m_bPaused = bPaused;}

    //-----------------------------------------------------------------------
    // _wxQTTimer::Notify
    //
    // 1) Checks to see if the movie is done, and if not continues
    //    streaming the movie
    // 2) Sends the wxEVT_MEDIA_STOP event if we have reached the end of
    //    the movie.
    //-----------------------------------------------------------------------
    void Notify()
    {
        if (!m_bPaused)
        {
            if(!m_pLib->IsMovieDone(m_movie))
                m_pLib->MoviesTask(m_movie, MOVIE_DELAY);
            else
            {
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                                      m_parent->m_ctrl->GetId());
                m_parent->m_ctrl->ProcessEvent(theEvent);

                if(theEvent.IsAllowed())
                {
                    Stop();
                    m_parent->Stop();
                    wxASSERT(m_pLib->GetMoviesError() == noErr);

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                          m_parent->m_ctrl->GetId());
                    m_parent->m_ctrl->ProcessEvent(theEvent);
                }
            }
        }
    }

protected:
    Movie m_movie;                  //Our movie instance
    bool m_bPaused;                 //Whether we are paused or not
    wxQTMediaBackend* m_parent;     //Backend pointer
    wxQuickTimeLibrary* m_pLib;         //Interfaces
};

//---------------------------------------------------------------------------
// wxQTMediaBackend Destructor
//
// Sets m_timer to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxQTMediaBackend::wxQTMediaBackend() : m_timer(NULL)
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
    if(m_timer)
        Cleanup();

    if(m_lib.IsOk())
    {
        //Note that ExitMovies() is not neccessary, but
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
    
    int nError;
    if ((nError = m_lib.InitializeQTML(0)) != noErr)    //-2093 no dll
    {
        wxFAIL_MSG(wxString::Format(wxT("Couldn't Initialize Quicktime-%i"), nError));
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

    m_ctrl = ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Load (file version)
//
// 1) Get an FSSpec from the Windows path name
// 2) Open the movie
// 3) Obtain the movie instance from the movie resource
// 4)
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxString& fileName)
{
    if(m_timer)
        Cleanup();

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;

    if (m_lib.NativePathNameToFSSpec ((char*) (const char*) fileName.mb_str(),
                                &sfFile, 0) != noErr)
        return false;

    if (m_lib.OpenMovieFile (&sfFile, &movieResFile, fsRdPerm) != noErr)
        return false;

    short movieResID = 0;
    Str255 movieName;

    err = m_lib.NewMovieFromFile (
    &m_movie,
    movieResFile,
    &movieResID,
    movieName,
    newMovieActive,
    NULL); //wasChanged

    m_lib.CloseMovieFile (movieResFile);

    if (err != noErr)
        return false;

    FinishLoad();

    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxURI& location)
{
    if(m_timer)
        Cleanup();

    wxString theURI = location.BuildURI();

    OSErr err = noErr;

    Handle theHandle = m_lib.NewHandleClear(theURI.length() + 1);
    wxASSERT(theHandle);

    m_lib.BlockMove(theURI.mb_str(), *theHandle, theURI.length() + 1);

    //create the movie from the handle that refers to the URI
    err = m_lib.NewMovieFromDataRef(&m_movie, newMovieActive,
                                NULL, theHandle,
                                'url'); //URLDataHandlerSubType

    m_lib.DisposeHandle(theHandle);

    if (err != noErr)
        return false;

    //preroll movie for streaming
    //TODO:Async this?
 /*
     TimeValue timeNow;
    Fixed playRate;
    timeNow = GetMovieTime(m_movie, NULL);
    playRate = GetMoviePreferredRate(m_movie);
    PrePrerollMovie(m_movie, timeNow, playRate, NULL, NULL);
    PrerollMovie(m_movie, timeNow, playRate);    
    m_lib.SetMovieRate(m_movie, playRate);
*/

    FinishLoad();

    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::FinishLoad()
{
    m_timer = new _wxQTTimer(m_movie, (wxQTMediaBackend*) this, &m_lib);
    wxASSERT(m_timer);

    //get the real size of the movie
    Rect outRect;
    m_lib.GetMovieNaturalBoundsRect (m_movie, &outRect);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;

    //reparent movie/*AudioMediaCharacteristic*/
    if(m_lib.GetMovieIndTrackType(m_movie, 1,
                            'eyes', //VisualMediaCharacteristic,
                            (1 << 1) //movieTrackCharacteristic 
                            | (1 << 2) //movieTrackEnabledOnly
                            ) != NULL)
    {
        m_lib.CreatePortAssociation(m_ctrl->GetHWND(), NULL, 0L);

        m_lib.SetMovieGWorld(m_movie,
                       (CGrafPtr) m_lib.GetNativeWindowPort(m_ctrl->GetHWND()),
                       NULL);
    }

    //we want millisecond precision
    m_lib.SetMovieTimeScale(m_movie, 1000);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a seperate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Play()
{
    m_lib.StartMovie(m_movie);
    m_timer->SetPaused(false);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Pause()
{
    m_lib.StopMovie(m_movie);
    m_timer->SetPaused(true);
    m_timer->Stop();
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Stop()
{
    m_timer->SetPaused(false);
    m_timer->Stop();

    m_lib.StopMovie(m_movie);
    if(m_lib.GetMoviesError() != noErr)
        return false;

    m_lib.GoToBeginningOfMovie(m_movie);
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetPlaybackRate()
{
    return ( ((double)m_lib.GetMovieRate(m_movie)) / 0x10000);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    m_lib.SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return m_lib.GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPosition(wxLongLong where)
{
    TimeRecord theTimeRecord;
    memset(&theTimeRecord, 0, sizeof(TimeRecord));
    theTimeRecord.value.lo = where.GetValue();
    theTimeRecord.scale = m_lib.GetMovieTimeScale(m_movie);
    theTimeRecord.base = m_lib.GetMovieTimeBase(m_movie);
    m_lib.SetMovieTime(m_movie, &theTimeRecord);

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
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDuration()
{
    return m_lib.GetMovieDuration(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
wxMediaState wxQTMediaBackend::GetState()
{
    if ( !m_timer || (m_timer->IsRunning() == false &&
                      m_timer->GetPaused() == false) )
        return wxMEDIASTATE_STOPPED;

    if( m_timer->IsRunning() == true )
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_PAUSED;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::Cleanup()
{
    delete m_timer;
    m_timer = NULL;

    m_lib.StopMovie(m_movie);
    m_lib.DisposeMovie(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
wxSize wxQTMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y), int w, int h)
{
    if(m_timer)
    {
        Rect theRect = {0, 0, h, w};

        m_lib.SetMovieBox(m_movie, &theRect);
        wxASSERT(m_lib.GetMoviesError() == noErr);
    }
}

//---------------------------------------------------------------------------
//  End QT Compilation Guard
//---------------------------------------------------------------------------
#endif //wxUSE_QUICKTIME

//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

//---------------------------------------------------------------------------
//  End wxMediaCtrl Compilation Guard and this file
//---------------------------------------------------------------------------
#endif //wxUSE_MEDIACTRL


