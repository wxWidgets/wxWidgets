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

#include "wx/dcclient.h"
#include "wx/thread.h"

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
//
//####################THE BIG DIRECTSHOW OVERVIEW############################
//
//
//  OK... this deserves its own little tutorial.  Knowledge of COM and class
//  factories is assumed throughout this code.
//
//  Basically, the way directshow works is that you tell it to render
//  a file, and it builds and connects a bunch of filters together.
//
//  There are many, many ways to do this.
//
//  WAYS TO RENDER A FILE (URLS WORK IN DS ALSO)
//
//  1)  Create an instance of IGraphBuilder and call RenderFile on it
//  2)  Create an instance of IMediaControl and call RenderFile on it
//  3)  Create an instance of IAMMultiMediaStream, call
//      IAMMultiMediaStream::AddStream and pass an IDirectDraw instance for
//      the video, and pass an IDirectSound(Buffer?) instance or use the
//      default sound renderer, then call RenderFile or RenderMoniker
//  4)  Create a Moniker instance for the file and create and build
//      all of the filtergraph manually
//
//  Our issue here is that we can't use the default representation of 1 and 2
//  because the IVideoWindow instance hogs a lot of the useful window
//  messages such as WM_SETCURSOR.
//
//  Solution #1 was to use #3 by creating a seperate IDirectDraw instance
//  for our window and blitting to that through a thread... unfortunately
//  the blitting resizing is very low quality and its quite slow.
//
//  The current way is to use windowless rendering and have directshow
//  do all the DirectDraw-style clipping to our window
//
//  ~~~~~~~~~~~~~~AFTER RENDERING THE FILE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  When done rendering the file, we need to get several interfaces from
//  either a IMediaControl or IGraphBuilder instance -
//
//  IMediaPosition - we can set the rate with this... we can also get
//  positions and set positions through this with REFTIME (double) instead
//  of the normal LONGLONG that IAMMultiMediaStream and IMediaControl use
//
//  IBasicAudio - we need this for setting/getting the volume
//
//  Interfaces that we don't use but might be useful one day -
//
//  IDirectDrawVideo - you can get this through the IFilter returned
//  from L"Video Renderer" filter from FindFilter on the IGraphBuilder.
//  Through this we can set the IDirectDraw instance DrawShow uses.
//
//  ~~~~~~~~~~~~~~STOPPING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  There are two ways we can do this -
//  1)  Have a thread compare the current position to the end position
//  about every 10 milliseconds
//  2)  Have IMediaSeekingEx send a message to a windowproc or signal a
//  windows event
//
//  Note that we can do these both, I.E. if an IMediaSeekingEx interface
//  is unavailable we can check the position instead of an event
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  COM includes
//---------------------------------------------------------------------------
#include "wx/msw/ole/oleutils.h" //wxBasicString, IID etc.
#include "wx/msw/ole/uuid.h" //IID etc..

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
//---------------------------------------------------------------------------
//  IIDS - used by CoCreateInstance and IUnknown::QueryInterface
//  Dumped from amstream.idl, quartz.idl, direct draw and with some
//  confirmation from WINE
//
//  Some of these are not used but are kept here for future reference anyway
//---------------------------------------------------------------------------

//QUARTZ
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
const CLSID CLSID_FilgraphManager = {0xE436EBB3,0x524F,0x11CE,{0x9F,0x53,0x00,0x20,0xAF,0x0B,0xA7,0x70}};

//AMSTREAM
const CLSID CLSID_AMMultiMediaStream = {0x49C47CE5, 0x9BA4, 0x11D0,{0x82, 0x12, 0x00, 0xC0, 0x4F, 0xC3, 0x2C, 0x45}};
const IID IID_IAMMultiMediaStream = {0xBEBE595C, 0x9A6F, 0x11D0,{0x8F, 0xDE, 0x00, 0xC0, 0x4F, 0xD9, 0x18, 0x9D}};
const IID IID_IDirectDrawMediaStream = {0xF4104FCE, 0x9A70, 0x11D0,{0x8F, 0xDE, 0x00, 0xC0, 0x4F, 0xD9, 0x18, 0x9D}};
const GUID MSPID_PrimaryVideo = {0xa35FF56A, 0x9FDA, 0x11D0,{0x8F, 0xDF, 0x00, 0xC0, 0x4F, 0xD9, 0x18, 0x9D}};
const GUID MSPID_PrimaryAudio = {0xa35FF56B, 0x9FDA, 0x11D0,{0x8F, 0xDF, 0x00, 0xC0, 0x4F, 0xD9, 0x18, 0x9D}};

//DDRAW
const IID IID_IDirectDraw = {0x6C14DB80,0xA733,0x11CE,{0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60}};
const CLSID CLSID_DirectDraw = {0xD7B70EE0,0x4340,0x11CF,{0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35}};

//??  QUARTZ Also?
const CLSID CLSID_VideoMixingRenderer = {0xB87BEB7B, 0x8D29, 0x423F,{0xAE, 0x4D, 0x65, 0x82, 0xC1, 0x01, 0x75, 0xAC}};
const IID IID_IVMRWindowlessControl =   {0x0EB1088C, 0x4DCD, 0x46F0,{0x87, 0x8F, 0x39, 0xDA, 0xE8, 0x6A, 0x51, 0xB7}};
const IID IID_IFilterGraph =            {0x56A8689F, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};
const IID IID_IGraphBuilder =           {0x56A868A9, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};
const IID IID_IVMRFilterConfig =        {0x9E5530C5, 0x7034, 0x48B4,{0xBB, 0x46, 0x0B, 0x8A, 0x6E, 0xFC, 0x8E, 0x36}};
const IID IID_IBaseFilter =             {0x56A86895, 0x0AD4, 0x11CE,{0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}};

//---------------------------------------------------------------------------
//  DIRECTDRAW COM INTERFACES
//---------------------------------------------------------------------------
//DDSURFACESDESC - we don't need most of the stuff here, esp. DDPIXELFORMAT,
//so just put stubs in
struct DDPIXELFORMAT {DWORD dw1,dw2,dw3,dw4,dw5,dw6,dw7,dw8;};
struct DDCOLORKEY {DWORD dwLow, dwHigh;};

typedef struct IDirectDrawClipper* LPDIRECTDRAWCLIPPER;
typedef struct IDirectDraw* LPDIRECTDRAW;
typedef struct IDirectDrawSurface* LPDIRECTDRAWSURFACE;
typedef struct DDSURFACEDESC* LPDDSURFACEDESC;
typedef struct IDirectDrawPalette* LPDIRECTDRAWPALETTE;
typedef struct DDSCAPS* LPDDSCAPS;
typedef DDCOLORKEY* LPDDCOLORKEY;
typedef DDPIXELFORMAT* LPDDPIXELFORMAT;
typedef struct DDCAPS* LPDDCAPS;

struct DDSURFACEDESC
{
    DWORD               dwSize;
    DWORD               dwFlags;
    DWORD               dwHeight;
    DWORD               dwWidth;
    union
    {
        LONG            lPitch;
        DWORD           dwLinearSize;
    };
    DWORD               dwBackBufferCount;
    union
    {
        DWORD           dwMipMapCount;
        DWORD           dwZBufferBitDepth;
        DWORD           dwRefreshRate;
    };
    DWORD               dwAlphaBitDepth;
    DWORD               dwReserved;
    LPVOID              lpSurface;
    DDCOLORKEY          ddckCKDestOverlay;
    DDCOLORKEY          ddckCKDestBlt;
    DDCOLORKEY          ddckCKSrcOverlay;
    DDCOLORKEY          ddckCKSrcBlt;
    DDPIXELFORMAT ddpfPixelFormat;
    struct DDSCAPS {DWORD dwCaps;} ddsCaps;
};

struct IDirectDrawClipper : public IUnknown
{
    STDMETHOD(GetClipList)(LPRECT, LPRGNDATA, LPDWORD) PURE;
    STDMETHOD(GetHWnd)(HWND*) PURE;
    STDMETHOD(Initialize)(LPDIRECTDRAW, DWORD) PURE;
    STDMETHOD(IsClipListChanged)(BOOL*) PURE;
    STDMETHOD(SetClipList)(LPRGNDATA,DWORD) PURE;
    STDMETHOD(SetHWnd)(DWORD, HWND) PURE;
};

struct IDirectDrawSurface : public IUnknown
{
    STDMETHOD(AddAttachedSurface)(LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(AddOverlayDirtyRect)(LPRECT) PURE;
    STDMETHOD(Blt)(LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, struct DDBLTFX*) PURE;
    STDMETHOD(BltBatch)(struct DDBLTBATCH*, DWORD, DWORD ) PURE;
    STDMETHOD(BltFast)(DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD) PURE;
    STDMETHOD(DeleteAttachedSurface)(DWORD,LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(EnumAttachedSurfaces)(LPVOID, LPVOID/*LPDDENUMSURFACESCALLBACK*/) PURE;
    STDMETHOD(EnumOverlayZOrders)(DWORD,LPVOID,LPVOID/*LPDDENUMSURFACESCALLBACK*/) PURE;
    STDMETHOD(Flip)(LPDIRECTDRAWSURFACE, DWORD) PURE;
    STDMETHOD(GetAttachedSurface)(LPDDSCAPS, LPDIRECTDRAWSURFACE*) PURE;
    STDMETHOD(GetBltStatus)(DWORD) PURE;
    STDMETHOD(GetCaps)(LPDDSCAPS) PURE;
    STDMETHOD(GetClipper)(LPDIRECTDRAWCLIPPER*) PURE;
    STDMETHOD(GetColorKey)(DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(GetDC)(HDC *) PURE;
    STDMETHOD(GetFlipStatus)(DWORD) PURE;
    STDMETHOD(GetOverlayPosition)(LPLONG, LPLONG ) PURE;
    STDMETHOD(GetPalette)(LPDIRECTDRAWPALETTE FAR*) PURE;
    STDMETHOD(GetPixelFormat)(LPDDPIXELFORMAT) PURE;
    STDMETHOD(GetSurfaceDesc)(LPDDSURFACEDESC) PURE;
    STDMETHOD(Initialize)(LPDIRECTDRAW, LPDDSURFACEDESC) PURE;
    STDMETHOD(IsLost)(THIS) PURE;
    STDMETHOD(Lock)(LPRECT,LPDDSURFACEDESC,DWORD,HANDLE) PURE;
    STDMETHOD(ReleaseDC)(HDC) PURE;
    STDMETHOD(Restore)(THIS) PURE;
    STDMETHOD(SetClipper)(LPDIRECTDRAWCLIPPER) PURE;
    STDMETHOD(SetColorKey)(DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(SetOverlayPosition)(LONG, LONG ) PURE;
    STDMETHOD(SetPalette)(IUnknown*) PURE;
    STDMETHOD(Unlock)(LPVOID) PURE;
    STDMETHOD(UpdateOverlay)(LPRECT, LPDIRECTDRAWSURFACE,LPRECT,
                               DWORD, struct DDOVERLAYFX*) PURE;
    STDMETHOD(UpdateOverlayDisplay)(DWORD) PURE;
    STDMETHOD(UpdateOverlayZOrder)(DWORD, LPDIRECTDRAWSURFACE) PURE;
};

struct IDirectDraw : public IUnknown
{
    STDMETHOD(Compact)() PURE;
    STDMETHOD(CreateClipper)(DWORD, LPDIRECTDRAWCLIPPER*, IUnknown * ) PURE;
    STDMETHOD(CreatePalette)(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown * ) PURE;
    STDMETHOD(CreateSurface)(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE *, IUnknown *) PURE;
    STDMETHOD(DuplicateSurface)(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE * ) PURE;
    STDMETHOD(EnumDisplayModes)(DWORD, LPDDSURFACEDESC, LPVOID, LPVOID ) PURE;
    STDMETHOD(EnumSurfaces)(DWORD, LPDDSURFACEDESC, LPVOID,LPVOID ) PURE;
    STDMETHOD(FlipToGDISurface)() PURE;
    STDMETHOD(GetCaps)(LPDDCAPS, LPDDCAPS) PURE;
    STDMETHOD(GetDisplayMode)(LPDDSURFACEDESC) PURE;
    STDMETHOD(GetFourCCCodes)(LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetGDISurface)(LPDIRECTDRAWSURFACE *) PURE;
    STDMETHOD(GetMonitorFrequency)(LPDWORD) PURE;
    STDMETHOD(GetScanLine)(LPDWORD) PURE;
    STDMETHOD(GetVerticalBlankStatus)(LPBOOL ) PURE;
    STDMETHOD(Initialize)(GUID *) PURE;
    STDMETHOD(RestoreDisplayMode)() PURE;
    STDMETHOD(SetCooperativeLevel)(HWND, DWORD) PURE;
    STDMETHOD(SetDisplayMode)(DWORD, DWORD,DWORD, DWORD, DWORD) PURE;
    STDMETHOD(WaitForVerticalBlank)(DWORD, HANDLE ) PURE;
};

//---------------------------------------------------------------------------
//  AMMEDIA COM INTERFACES
//---------------------------------------------------------------------------
struct IMediaStream;
struct IMultiMediaStream;
struct IStreamSample : public IUnknown
{
public:
    STDMETHOD(GetMediaStream)(IMediaStream **) PURE;
    STDMETHOD(GetSampleTimes)(LONGLONG *, LONGLONG *, LONGLONG *) PURE;
    STDMETHOD(SetSampleTimes)(const LONGLONG *, const LONGLONG *) PURE;
    STDMETHOD(Update)(DWORD, HANDLE, LPVOID, DWORD_PTR) PURE;
    STDMETHOD(CompletionStatus)(DWORD, DWORD) PURE;
};

struct IDirectDrawStreamSample : public IStreamSample
{
public:
    STDMETHOD(GetSurface)(IDirectDrawSurface **, RECT *) PURE;
    STDMETHOD(SetRect)(const RECT *) PURE;
};

struct IMediaStream : public IUnknown
{
    STDMETHOD(GetMultiMediaStream)(IMultiMediaStream **) PURE;
    STDMETHOD(GetInformation)(GUID *, int *) PURE;
    STDMETHOD(SetSameFormat)(IMediaStream *, DWORD) PURE;
    STDMETHOD(AllocateSample)(DWORD, IStreamSample **) PURE;
    STDMETHOD(CreateSharedSample)(IStreamSample *, DWORD,
                                  IStreamSample **) PURE;
    STDMETHOD(SendEndOfStream)(DWORD dwFlags) PURE;
};

struct IDirectDrawMediaStream : public IMediaStream
{
    STDMETHOD(GetFormat)(DDSURFACEDESC *, IDirectDrawPalette **,
                         DDSURFACEDESC *, DWORD *) PURE;
    STDMETHOD(SetFormat)(const DDSURFACEDESC *, IDirectDrawPalette *) PURE;
    STDMETHOD(GetDirectDraw)(IDirectDraw **) PURE;
    STDMETHOD(SetDirectDraw)(IDirectDraw *) PURE;
    STDMETHOD(CreateSample)(IDirectDrawSurface *, const RECT *,
                              DWORD, IDirectDrawStreamSample **) PURE;
    STDMETHOD(GetTimePerFrame)(LONGLONG *) PURE;
};

struct IMultiMediaStream : public IUnknown
{
    STDMETHOD(GetInformation)(DWORD *, int *) PURE;
    STDMETHOD(GetMediaStream)(REFGUID, IMediaStream **) PURE;
    STDMETHOD(EnumMediaStreams)(long, IMediaStream **) PURE;
    STDMETHOD(GetState)(int *pCurrentState) PURE;
    STDMETHOD(SetState)(int NewState) PURE;
    STDMETHOD(GetTime)(LONGLONG *pCurrentTime) PURE;
    STDMETHOD(GetDuration)(LONGLONG *pDuration) PURE;
    STDMETHOD(Seek)(LONGLONG SeekTime) PURE;
    STDMETHOD(GetEndOfStreamEventHandle)(HANDLE *phEOS) PURE;
};

struct IAMMultiMediaStream : public IMultiMediaStream
{
    STDMETHOD(Initialize)(int, DWORD, IUnknown *) PURE;
    STDMETHOD(GetFilterGraph)(IUnknown **) PURE;
    STDMETHOD(GetFilter)(IUnknown **) PURE;
    STDMETHOD(AddMediaStream)(IUnknown *, const GUID*, DWORD,
                              IMediaStream **) PURE;
    STDMETHOD(OpenFile)(LPCWSTR, DWORD) PURE;
    STDMETHOD(OpenMoniker)(IBindCtx *, IMoniker *, DWORD) PURE;
    STDMETHOD(Render)(DWORD) PURE;
};

//---------------------------------------------------------------------------
//  QUARTZ COM INTERFACES (dumped from quartz.idl from MSVC COM Browser)
//---------------------------------------------------------------------------
struct IAMCollection : public IDispatch
{
    STDMETHOD(get_Count)(long *) PURE;
    STDMETHOD(Item)(long, IUnknown **) PURE;
    STDMETHOD(get__NewEnum)(IUnknown **) PURE;
};

struct IMediaControl : public IDispatch
{
    STDMETHOD(Run)() PURE;
    STDMETHOD(Pause)() PURE;
    STDMETHOD(Stop)() PURE;
    STDMETHOD(GetState)(long, long*) PURE;
    STDMETHOD(RenderFile)(BSTR) PURE;
    STDMETHOD(AddSourceFilter)(BSTR, IDispatch **) PURE;
    STDMETHOD(get_FilterCollection)(IDispatch **) PURE;
    STDMETHOD(get_RegFilterCollection)(IDispatch **) PURE;
    STDMETHOD(StopWhenReady)() PURE;
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

struct IMediaEventEx : public IMediaEvent
{
    STDMETHOD(SetNotifyWindow)(LONG_PTR, long, LONG_PTR) PURE;
    STDMETHOD(SetNotifyFlags)(long) PURE;
    STDMETHOD(GetNotifyFlags)(long *) PURE;
};

struct IMediaPosition : public IDispatch
{
    STDMETHOD(get_Duration)(double *) PURE;
    STDMETHOD(put_CurrentPosition)(double) PURE;
    STDMETHOD(get_CurrentPosition)(double *) PURE;
    STDMETHOD(get_StopTime)(double *) PURE;
    STDMETHOD(put_StopTime)(double) PURE;
    STDMETHOD(get_PrerollTime)(double *) PURE;
    STDMETHOD(put_PrerollTime)(double) PURE;
    STDMETHOD(put_Rate)(double) PURE;
    STDMETHOD(get_Rate)(double *) PURE;
    STDMETHOD(CanSeekForward)(long *) PURE;
    STDMETHOD(CanSeekBackward)(long *) PURE;
};

struct IBasicAudio : public IDispatch
{
    STDMETHOD(put_Volume)(long) PURE;
    STDMETHOD(get_Volume)(long *) PURE;
    STDMETHOD(put_Balance)(long) PURE;
    STDMETHOD(get_Balance)(long *) PURE;
};

//---------------------------------------------------------------------------
// MISC COM INTERFACES
//---------------------------------------------------------------------------
struct IVMRWindowlessControl : public IUnknown
{
    STDMETHOD(GetNativeVideoSize)(LONG *, LONG *, LONG *, LONG *) PURE;
    STDMETHOD(GetMinIdealVideoSize)(LONG *, LONG *) PURE;
    STDMETHOD(GetMaxIdealVideoSize)(LONG *, LONG *) PURE;
    STDMETHOD(SetVideoPosition)(const LPRECT,const LPRECT) PURE;
    STDMETHOD(GetVideoPosition)(LPRECT, LPRECT) PURE;
    STDMETHOD(GetAspectRatioMode)(DWORD *) PURE;
    STDMETHOD(SetAspectRatioMode)(DWORD) PURE;
    STDMETHOD(SetVideoClippingWindow)(HWND) PURE;
    STDMETHOD(RepaintVideo)(HWND, HDC) PURE;
    STDMETHOD(DisplayModeChanged)() PURE;
    STDMETHOD(GetCurrentImage)(BYTE **) PURE;
    STDMETHOD(SetBorderColor)(COLORREF) PURE;
    STDMETHOD(GetBorderColor)(COLORREF *) PURE;
    STDMETHOD(SetColorKey)(COLORREF) PURE;
    STDMETHOD(GetColorKey)(COLORREF *) PURE;
};

typedef IUnknown IVMRImageCompositor;

struct IVMRFilterConfig : public IUnknown
{
    STDMETHOD(SetImageCompositor)(IVMRImageCompositor *) PURE;
    STDMETHOD(SetNumberOfStreams)(DWORD) PURE;
    STDMETHOD(GetNumberOfStreams)(DWORD *) PURE;
    STDMETHOD(SetRenderingPrefs)(DWORD) PURE;
    STDMETHOD(GetRenderingPrefs)(DWORD *) PURE;
    STDMETHOD(SetRenderingMode)(DWORD) PURE;
    STDMETHOD(GetRenderingMode)(DWORD *) PURE;
};

typedef IUnknown IBaseFilter;
typedef IUnknown IPin;
typedef IUnknown IEnumFilters;
typedef int AM_MEDIA_TYPE;

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
public:
    STDMETHOD(Connect)(IPin *, IPin *) PURE;
    STDMETHOD(Render)(IPin *) PURE;
    STDMETHOD(RenderFile)(LPCWSTR, LPCWSTR) PURE;
    STDMETHOD(AddSourceFilter)(LPCWSTR, LPCWSTR, IBaseFilter **) PURE;
    STDMETHOD(SetLogFile)(DWORD_PTR) PURE;
    STDMETHOD(Abort)() PURE;
    STDMETHOD(ShouldOperationContinue)() PURE;
};

//------------------------------------------------------------------
// wxAMMediaBackend (Active Movie)
//------------------------------------------------------------------
class WXDLLIMPEXP_MEDIA wxAMMediaThread : public wxThread
{
public:
    virtual ExitCode Entry();

    class wxAMMediaBackend* pThis;
};

//cludgy workaround for wx events.  slots would be nice :)
class WXDLLIMPEXP_MEDIA wxAMMediaEvtHandler : public wxEvtHandler
{
public:
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};

typedef BOOL (WINAPI* LPAMGETERRORTEXT)(HRESULT, wxChar *, DWORD);

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

    virtual double GetVolume();
    virtual bool SetVolume(double);

    void Cleanup();
    void OnStop();
    bool SetWindowlessMode(IGraphBuilder* pGB,
                           IVMRWindowlessControl** ppVMC = NULL);

    wxControl* m_ctrl;

    wxMediaState m_state;
    wxCriticalSection m_rendercs;

    IVMRWindowlessControl* m_pVMC;
    IGraphBuilder* m_pGB;
    IBasicAudio* m_pBA;
    IMediaControl* m_pMC;
    IMediaEvent* m_pME;
    IMediaPosition* m_pMS;
    bool m_bVideo;

    wxAMMediaThread* m_pThread;

    wxSize m_bestSize;

#ifdef __WXDEBUG__
    HMODULE m_hQuartzDll;
    LPAMGETERRORTEXT m_lpAMGetErrorText;
    wxString GetErrorString(HRESULT hrdsv);
#endif

    friend class wxAMMediaThread;
    friend class wxAMMediaEvtHandler;

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
#define Ptr char*
#define Handle Ptr*
#define Fixed long
#define OSType unsigned long
#define CGrafPtr struct GrafPort *
#define TimeScale long
#define TimeBase struct TimeBaseRecord *

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


//Class that utilizes Robert Roeblings Dynamic Library Macros
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

    bool bOk;   //TODO:  Get rid of this, use m_ok instead (not a biggie)

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
    wxDL_METHOD_LOAD( m_dll, GetMovieVolume, bOk );
    wxDL_METHOD_LOAD( m_dll, SetMovieVolume, bOk );

    wxLog::EnableLogging(bWasLoggingEnabled);
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

    virtual double GetVolume();
    virtual bool SetVolume(double);

    void Cleanup();
    void FinishLoad();

    wxSize m_bestSize;              //Original movie size
    Movie m_movie;    //QT Movie handle/instance
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    class _wxQTTimer* m_timer;      //Timer for streaming the movie
    wxQuickTimeLibrary m_lib;

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
#include "wx/log.h"             //wxLogDebug et al.

//Get the error string for Active Movie
wxString wxAMMediaBackend::GetErrorString(HRESULT hrdsv)
{
    wxChar szError[MAX_ERROR_TEXT_LEN];
    if( m_lpAMGetErrorText != NULL &&
       (*m_lpAMGetErrorText)(hrdsv, szError, MAX_ERROR_TEXT_LEN) == 0)
    {
        return wxString::Format(wxT("DirectShow error \"%s\" \n")
                                     wxT("(numeric %i)\n")
                                     wxT("occured at line %i in ")
                                     wxT("mediactrl.cpp"),
                                     szError, (int)hrdsv, __LINE__);
    }
    else
    {
        return wxString::Format(wxT("Unknown error (%i) ")
                                     wxT("occurred at")
                                     wxT(" line %i in mediactrl.cpp."),
                                     (int)hrdsv, __LINE__);
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
// wxAMMediaBackend Constructor
//
// Sets m_hNotifyWnd to NULL to signify that we haven't loaded anything yet
//---------------------------------------------------------------------------
wxAMMediaBackend::wxAMMediaBackend()
                 :m_state(wxMEDIASTATE_STOPPED)
                 ,m_pVMC(NULL)
                 ,m_pGB(NULL)
                 ,m_pBA(NULL)
                 ,m_pMC(NULL)
                 ,m_pME(NULL)
                 ,m_pMS(NULL)
                 ,m_pThread(NULL)
#ifdef __WXDEBUG__
                 ,m_hQuartzDll(NULL)
#endif
{
}

//---------------------------------------------------------------------------
// wxAMMediaBackend Destructor
//
// Cleans up everything
//---------------------------------------------------------------------------
wxAMMediaBackend::~wxAMMediaBackend()
{
    if (m_pVMC)
        Cleanup();
#ifdef __WXDEBUG__
    if(m_hQuartzDll)
        ::FreeLibrary(m_hQuartzDll);
#endif
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::CreateControl
//
// 1) Check to see if Active Movie supports windowless controls
// 2) Connect events to the media control and its TLW
//---------------------------------------------------------------------------
bool wxAMMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
#ifdef __WXDEBUG__
    m_hQuartzDll = ::LoadLibrary(wxT("quartz.dll"));
    if(m_hQuartzDll)
    {
            m_lpAMGetErrorText = (LPAMGETERRORTEXT) ::GetProcAddress(
                m_hQuartzDll,
            wxString::Format(wxT("AMGetErrorText%s"),

#ifdef __WXUNICODE__
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

    //Make sure a valid windowless video mixing interface exists
    IGraphBuilder* pGB;
    if( ::CoCreateInstance(CLSID_FilgraphManager, NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IGraphBuilder, (void**)&pGB) != 0 )
        return false;

    if( !SetWindowlessMode(pGB) )
        return false;

    //clean up
    pGB->Release();

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those, and create with
    // wxCLIP_CHILDREN, so that if the driver/backend
    // is a child window, it refreshes properly
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxBORDER_NONE | wxCLIP_CHILDREN,
                            validator, name) )
        return false;

    // My problem with this was only with a previous patch, probably the third rewrite
    // fixed it as a side-effect. In fact, the erase background style of drawing not
    // only works now, but is much better than paint-based updates (the paint event
    // handler flickers if the wxMediaCtrl shares a sizer with another child window,
    // or is on a notebook)
    //  - Greg Hazel
    ctrl->Connect(ctrl->GetId(), wxEVT_ERASE_BACKGROUND,
        wxEraseEventHandler(wxAMMediaEvtHandler::OnEraseBackground),
        NULL, (wxEvtHandler*) this);

    //
    // done...
    //
    m_ctrl = ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetWindowlessMode
//
// Adds a Video Mixing Renderer to a Filter Graph and obtains the
// windowless control from it
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetWindowlessMode(IGraphBuilder* pGB,
                                         IVMRWindowlessControl** ppVMC)
{
    HRESULT hr;

    //
    // Create and add a custom Video Mixing Render to the graph
    //
    IBaseFilter* pVMR;
    if( ::CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
                CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pVMR) != 0 )
        return false;

    hr = pGB->AddFilter(pVMR, L"Video Mixing Renderer");
    if ( hr != 0)
    {
        wxAMLOG(hr);
        pVMR->Release();
        return false;
    }

    //
    // Set the graph to windowless mode
    //
    IVMRFilterConfig* pConfig;
    hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
    if( hr != 0 )
    {
        wxAMLOG(hr);
        pVMR->Release();
        return false;
    }

    hr = pConfig->SetRenderingMode(2);
    if( hr != 0) //2 == VMRMode_Windowless
    {
        wxAMLOG(hr);
        pConfig->Release();
        pVMR->Release();
        return false;
    }

    pConfig->Release();

    //
    // Obtain the windowless control
    //
    IVMRWindowlessControl* pVMC;
    hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pVMC);
    if( hr != 0 )
    {
        wxAMLOG(hr);
        pVMR->Release();
        return false;
    }

    //
    // Success
    //
    if(ppVMC)
        *ppVMC = pVMC;
    else
        pVMC->Release();

    pVMR->Release();
    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (file version)
//
// 1) Cleans up previously loaded data
// 2) Creates a filter graph
// 3) Add a video mixer, set the graph to windowless mode and clip
//    output to our media control
// 4) Query interfaces to use later
// 5) Get native video size (which becomes our best size)
// 6) Refresh parent's sizers
// 7) Start event/rendering thread
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxString& fileName)
{
    HRESULT hr;

    //if previously loaded cleanup
    if(m_pVMC)
        Cleanup();

    //Create interfaces - we already checked for success in CreateControl
    ::CoCreateInstance(CLSID_FilgraphManager, NULL, CLSCTX_INPROC_SERVER,
                     IID_IGraphBuilder, (void**)&m_pGB);


    // Set and clip output
    SetWindowlessMode(m_pGB, &m_pVMC);
    hr = m_pVMC->SetVideoClippingWindow((HWND)m_ctrl->GetHandle());

    if(hr != 0)
    {
        m_bestSize.x = m_bestSize.y = 0;
        wxAMFAIL(hr);
        return false;
    }

    //load the graph & render
    if( m_pGB->RenderFile(fileName.wc_str(wxConvLocal), NULL) != 0 )
        return false;

    //
    //Get the interfaces, all of them
    //
    hr = m_pGB->QueryInterface(IID_IMediaEvent, (void**)&m_pME);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    hr = m_pGB->QueryInterface(IID_IMediaControl, (void**)&m_pMC);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    hr = m_pGB->QueryInterface(IID_IMediaPosition, (void**)&m_pMS);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    hr = m_pGB->QueryInterface(IID_IBasicAudio, (void**)&m_pBA);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        //not critical
    }

    //
    // Get original video size
    //
    hr = m_pVMC->GetNativeVideoSize((LONG*)&m_bestSize.x, (LONG*)&m_bestSize.y,
                                   NULL, NULL);
    if(hr != 0)
    {
        m_bestSize.x = m_bestSize.y = 0;
        wxAMFAIL(hr);
        return false;
    }

    if(m_bestSize.x == 0 && m_bestSize.y == 0)
        m_bVideo = false;
    else
        m_bVideo = true;

    //
    // Force the parent window of this control to recalculate
    // the size of this if sizers are being used
    // and render the results immediately
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    m_ctrl->SetSize(m_ctrl->GetSize());

    //
    //  Create the event thread
    //
    m_pThread = new wxAMMediaThread;
    m_pThread->pThis = this;
    m_pThread->Create();
    m_pThread->Run();

    //
    //  done
    //
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
// wxAMMediaBackend::Cleanup
//
// Releases all the directshow interfaces we use
// TODO: Maybe only create one instance of IAMMultiMediaStream and reuse it
// rather than recreating it each time?
//---------------------------------------------------------------------------
void wxAMMediaBackend::Cleanup()
{
    // RN:  This could be a bad ptr if load failed after
    // m_pVMC was created
    if(m_pThread)
    {
        m_pThread->Delete();
        m_pThread = NULL;
    }

    // Release and zero DirectShow interfaces
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pME);
    SAFE_RELEASE(m_pMS);
    SAFE_RELEASE(m_pBA);
    SAFE_RELEASE(m_pGB);
    SAFE_RELEASE(m_pVMC);
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
    wxCriticalSectionLocker lock(m_rendercs);

    if( SUCCEEDED(m_pMC->Run()) )
    {
        m_state = wxMEDIASTATE_PLAYING;
        m_ctrl->Refresh(); //videoless control finicky about refreshing
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Pause
//
// Pauses the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Pause()
{
    wxCriticalSectionLocker lock(m_rendercs);

    if( SUCCEEDED(m_pMC->Pause()) )
    {
        m_state = wxMEDIASTATE_PAUSED;
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Stop
//
// Stops the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Stop()
{
    wxCriticalSectionLocker lock(m_rendercs);

    if( SUCCEEDED(m_pMC->Stop()) )
    {
    //We don't care if it can't get to the beginning in directshow -
    //it could be a non-seeking filter (wince midi) in which case playing
    //starts all over again
        wxAMMediaBackend::SetPosition(0);

        m_state = wxMEDIASTATE_STOPPED;
        return true;
    }

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
    HRESULT hr = m_pMS->put_CurrentPosition(
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
    HRESULT hr = m_pMS->get_CurrentPosition(&outCur);
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
    if(m_pBA)
    {
        long lVolume;
        HRESULT hr = m_pBA->get_Volume(&lVolume);
        if(FAILED(hr))
        {
            wxAMLOG(hr);
            return 0.0;
        }

        return (((double)(lVolume + 10000)) / 10000.0);
    }

    wxLogDebug(wxT("No directshow audio interface"));
    return 0.0;
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
    if(m_pBA)
    {
        HRESULT hr = m_pBA->put_Volume( (long) ((dVolume-1.0) * 10000.0) );
        if(FAILED(hr))
        {
            wxAMLOG(hr);
            return false;
        }
        return true;
    }

    wxLogDebug(wxT("No directshow audio interface"));
    return false;
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
    HRESULT hr = m_pMS->get_Duration(&outDuration);
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
    return m_state;
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
    HRESULT hr = m_pMS->get_Rate(&dRate);
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
    HRESULT hr = m_pMS->put_Rate(dRate);
    if(FAILED(hr))
    {
        wxAMLOG(hr);
        return false;
    }

    return true;
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
                            int w, int h)
{
    //don't use deferred positioning on windows
    if(m_pVMC && m_bVideo)
    {
        RECT srcRect, destRect;

        //portion of video to display in window
        srcRect.top = 0; srcRect.left = 0;
        srcRect.bottom = m_bestSize.y; srcRect.right = m_bestSize.x;

        //it happens.
        if (w < 0)
        {
            w = 0;
        }
        if (h < 0)
        {
            h = 0;
        }

        //position in window client coordinates to display and stretch to
        destRect.top = 0; destRect.left = 0;
        destRect.bottom = h; destRect.right = w;

        //set the windowless control positions
        HRESULT hr = m_pVMC->SetVideoPosition(&srcRect, &destRect);
        if(FAILED(hr))
        {
            wxAMLOG(hr);
        }
    }
}

//---------------------------------------------------------------------------
// wxAMMediaThread::Entry
//
// Render the current movie frame
//---------------------------------------------------------------------------
wxThread::ExitCode wxAMMediaThread::Entry()
{
    while(!TestDestroy())
    {
        LONG    evCode,
                evParam1,
                evParam2;

        //
        // DirectShow keeps a list of queued events, and we need
        // to go through them one by one, stopping at (Hopefully only one)
        // EC_COMPLETE message
        //
        while( pThis->m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                                      (LONG_PTR *) &evParam2, 0) == 0 )
        {
            // Cleanup memory that GetEvent allocated
            HRESULT hr = pThis->m_pME->FreeEventParams(evCode,
                                                evParam1, evParam2);
            if(hr != 0)
            {
                //Even though this makes a messagebox this
                //is windows where we can do gui stuff in seperate
                //threads :)
                wxFAIL_MSG(pThis->GetErrorString(hr));
            }
            // If this is the end of the clip, notify handler
            else if(1 == evCode) //EC_COMPLETE
            {
                pThis->OnStop();
            }
        }

        Sleep(10);
    }

    return NULL;
}


//---------------------------------------------------------------------------
// wxAMMediaBackend::OnStop
//
// Handle stopping when the stream ends
//---------------------------------------------------------------------------
void wxAMMediaBackend::OnStop()
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

//---------------------------------------------------------------------------
// wxAMMediaEvtHandler::OnEraseBackground
//
// Tell WX not to erase the background of our control window
//---------------------------------------------------------------------------
void wxAMMediaEvtHandler::OnEraseBackground(wxEraseEvent& evt)
{
    wxAMMediaBackend* pThis = (wxAMMediaBackend*) this;
    if(pThis->m_pVMC && pThis->m_bVideo)
    {
        //TODO: Use wxClientDC?
        HDC hdc = ::GetDC((HWND)pThis->m_ctrl->GetHandle());
        HRESULT hr = pThis->m_pVMC->RepaintVideo((HWND)pThis->m_ctrl->GetHandle(),
                                        hdc);
        if(FAILED(hr))
        {
            wxFAIL_MSG(pThis->GetErrorString(hr));
        }
        ::ReleaseDC((HWND)pThis->m_ctrl->GetHandle(), hdc);
    }
    else
    {
        evt.Skip();
    }
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
                       wxT("registering for DirectShow events!")  );

        return false;
    }

    wxSetWindowProc(m_hNotifyWnd, wxMCIMediaBackend::NotifyWndProc);

    ::SetWindowLong(m_hNotifyWnd, GWL_USERDATA,
                       (LONG) this);

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
                        0x00800000L | 0x01000000L, //MCI_DGV_SETAUDIO+(_ITEM | _VALUE)
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
    wxMCIMediaBackend* backend = (wxMCIMediaBackend*)
#ifdef _WIN32
        ::GetWindowLong(hWnd, GWL_USERDATA);
#else
        ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
#endif
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

    Handle theHandle = m_lib.NewHandleClear(theURI.length() + 1);
    wxASSERT(theHandle);

    m_lib.BlockMove(theURI.mb_str(), *theHandle, theURI.length() + 1);

    //create the movie from the handle that refers to the URI
    OSErr err = m_lib.NewMovieFromDataRef(&m_movie, newMovieActive,
                                NULL, theHandle,
                                URLDataHandlerSubType);

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
    memset(&outRect, 0, sizeof(Rect)); //for annoying VC6 warning
    m_lib.GetMovieNaturalBoundsRect (m_movie, &outRect);
    wxASSERT(m_lib.GetMoviesError() == noErr);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;

    //reparent movie/*AudioMediaCharacteristic*/
    if(m_lib.GetMovieIndTrackType(m_movie, 1,
                            VisualMediaCharacteristic,
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
    //the user opened a separate media file
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
    theTimeRecord.value.lo = where.GetLo();
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

    if(sVolume & (128 << 8)) //negative - no sound
        return 0.0;

    return (sVolume & (127 << 8)) ? 1.0 : ((double)(sVolume & 255)) / 255.0;
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
    short sVolume = (short) (dVolume >= .9999 ? 1 << 8 : (dVolume * 255) );
    m_lib.SetMovieVolume(m_movie, sVolume);
    return true;
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
        Rect theRect = {0, 0, (short)h, (short)w};

        m_lib.SetMovieBox(m_movie, &theRect);
        wxASSERT(m_lib.GetMoviesError() == noErr);
    }
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


