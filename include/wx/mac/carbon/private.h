/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"
#include "wx/app.h"

#ifdef __DARWIN__
#    include <Carbon/Carbon.h>
#else
#    include <Debugging.h>
#    include <Quickdraw.h>
#    include <Appearance.h>
#    include <Folders.h>
#    include <Controls.h>
#    include <ControlDefinitions.h>
#    include <LowMem.h>
#    include <Gestalt.h>
#endif

#if UNIVERSAL_INTERFACES_VERSION < 0x0340
    #error "please update to Apple's lastest universal headers from http://developer.apple.com/sdk/"
#endif

#ifndef MAC_OS_X_VERSION_10_3
    #define MAC_OS_X_VERSION_10_3 1030
#endif

#if wxUSE_GUI

#include "wx/window.h"

class wxMacPortStateHelper 
{
    DECLARE_NO_COPY_CLASS(wxMacPortStateHelper)
        
public:
    wxMacPortStateHelper( GrafPtr newport) ; 
    wxMacPortStateHelper() ;
    ~wxMacPortStateHelper() ;

    void Setup( GrafPtr newport ) ;
    void Clear() ;
    bool IsCleared() { return m_clip == NULL ; }
    GrafPtr GetCurrentPort() { return m_currentPort ; }

private:
    GrafPtr          m_currentPort ;
    GrafPtr          m_oldPort ;
    RgnHandle          m_clip ;
    ThemeDrawingState m_drawingState ;
    short          m_textFont ;
    short          m_textSize ;
    short          m_textStyle ;
    short           m_textMode ;
} ;

class WXDLLEXPORT wxMacPortSaver
{
    DECLARE_NO_COPY_CLASS(wxMacPortSaver)
        
public:
    wxMacPortSaver( GrafPtr port ) 
    {
        ::GetPort( &m_port ) ;
        ::SetPort( port ) ;
    }
    ~wxMacPortSaver()
    {
        ::SetPort( m_port ) ;
    }
private :
    GrafPtr m_port ;
} ;

class WXDLLEXPORT wxMacPortSetter
{
    DECLARE_NO_COPY_CLASS(wxMacPortSetter)
        
public:
    wxMacPortSetter( const wxDC* dc ) ;
    ~wxMacPortSetter() ;
private:
    wxMacPortStateHelper m_ph ;
    const wxDC* m_dc ;
} ;

/*
 Clips to the visible region of a control within the current port
 */
 
class WXDLLEXPORT wxMacWindowClipper : public wxMacPortSaver
{
    DECLARE_NO_COPY_CLASS(wxMacWindowClipper)
        
public:
    wxMacWindowClipper( const wxWindow* win ) ;
    ~wxMacWindowClipper() ;
private:
    GrafPtr   m_newPort ;
    RgnHandle m_formerClip ;
    RgnHandle m_newClip ;
} ;

class WXDLLEXPORT wxMacWindowStateSaver : public wxMacWindowClipper
{
    DECLARE_NO_COPY_CLASS(wxMacWindowStateSaver)
        
public:
    wxMacWindowStateSaver( const wxWindow* win ) ;
    ~wxMacWindowStateSaver() ;
private:
    GrafPtr   m_newPort ;
    ThemeDrawingState m_themeDrawingState ;
} ;

/*
class wxMacDrawingHelper
{
    DECLARE_NO_COPY_CLASS(wxMacDrawingHelper)
        
public:
    wxMacDrawingHelper( wxWindowMac * theWindow , bool clientArea = false ) ;
    ~wxMacDrawingHelper() ;
    bool Ok() { return m_ok ; }
    void LocalToWindow( Rect *rect) { OffsetRect( rect , m_origin.h , m_origin.v ) ; }
    void LocalToWindow( Point *pt ) { AddPt( m_origin , pt ) ; }
    void LocalToWindow( RgnHandle rgn ) { OffsetRgn( rgn , m_origin.h , m_origin.v ) ; }
    const Point& GetOrigin() { return m_origin ; }
private:
    Point     m_origin ;
    GrafPtr   m_formerPort ;
    GrafPtr   m_currentPort ;
    PenState  m_savedPenState ;
    bool      m_ok ;
} ;
*/

// app.h
bool wxMacConvertEventToRecord( EventRef event , EventRecord *rec) ;

#endif // wxUSE_GUI

// filefn.h
WXDLLEXPORT wxString wxMacFSSpec2MacFilename( const FSSpec *spec ) ;
WXDLLEXPORT void wxMacFilename2FSSpec( const char *path , FSSpec *spec ) ;
WXDLLEXPORT void wxMacFilename2FSSpec( const wxChar *path , FSSpec *spec ) ;
#  ifndef __DARWIN__
// Mac file names are POSIX (Unix style) under Darwin, so these are not needed
WXDLLEXPORT wxString wxMacFSSpec2UnixFilename( const FSSpec *spec ) ;
WXDLLEXPORT void wxUnixFilename2FSSpec( const char *path , FSSpec *spec ) ;
WXDLLEXPORT wxString wxMac2UnixFilename( const char *s) ;
WXDLLEXPORT wxString wxUnix2MacFilename( const char *s);
#  endif

// utils.h
WXDLLEXPORT wxString wxMacFindFolder(short vRefNum,
                                     OSType folderType,
                                     Boolean createFolder);

#if wxUSE_GUI

GWorldPtr         wxMacCreateGWorld( int width , int height , int depth ) ;
void                 wxMacDestroyGWorld( GWorldPtr gw ) ;
PicHandle         wxMacCreatePict( GWorldPtr gw , GWorldPtr mask = NULL ) ;
CIconHandle     wxMacCreateCIcon(GWorldPtr image , GWorldPtr mask , short dstDepth , short iconSize  ) ;
void                 wxMacSetColorTableEntry( CTabHandle newColors , int index , int red , int green ,  int blue ) ;
CTabHandle         wxMacCreateColorTable( int numColors ) ;
void wxMacCreateBitmapButton( ControlButtonContentInfo*info , const wxBitmap& bitmap , int forceType = 0 ) ;

#define MAC_WXCOLORREF(a) (*((RGBColor*)&(a)))
#define MAC_WXHBITMAP(a) (GWorldPtr(a))
#define MAC_WXHMETAFILE(a) (PicHandle(a))
#define MAC_WXHICON(a) (CIconHandle(a))
#define MAC_WXHCURSOR(a) (CursHandle(a))
#define MAC_WXHRGN(a) (RgnHandle(a))
#define MAC_WXHWND(a) (WindowPtr(a))
#define MAC_WXRECPTR(a) ((Rect*)a)
#define MAC_WXPOINTPTR(a) ((Point*)a)
#define MAC_WXHMENU(a) ((MenuHandle)a)

struct wxOpaqueWindowRef
{
    wxOpaqueWindowRef( WindowRef ref ) { m_data = ref ; }
    operator WindowRef() { return m_data ; } 
private :
    WindowRef m_data ;
} ;

wxWindow *              wxFindControlFromMacControl(ControlRef inControl ) ;
wxTopLevelWindowMac*    wxFindWinFromMacWindow( WindowRef inWindow ) ;
wxMenu*                 wxFindMenuFromMacMenu(MenuRef inMenuRef) ;

extern wxWindow* g_MacLastWindow ;
pascal OSStatus wxMacTopLevelMouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data ) ;
Rect wxMacGetBoundsForControl( wxWindow* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin = true ) ;

template<typename T> EventParamType wxMacGetEventParamType() { wxFAIL_MSG( wxT("Unknown Param Type") ) ; return 0 ; }
template<> inline EventParamType wxMacGetEventParamType<RgnHandle>() { return typeQDRgnHandle ; }
template<> inline EventParamType wxMacGetEventParamType<ControlRef>() { return typeControlRef ; }
template<> inline EventParamType wxMacGetEventParamType<WindowRef>() { return typeWindowRef ; }
template<> inline EventParamType wxMacGetEventParamType<MenuRef>() { return typeMenuRef ; }
template<> inline EventParamType wxMacGetEventParamType<EventRef>() { return typeEventRef ; }
template<> inline EventParamType wxMacGetEventParamType<Point>() { return typeQDPoint ; }
template<> inline EventParamType wxMacGetEventParamType<Rect>() { return typeQDRectangle ; }
template<> inline EventParamType wxMacGetEventParamType<Boolean>() { return typeBoolean ; }
#if TARGET_API_MAC_OSX
template<> inline EventParamType wxMacGetEventParamType<HIPoint>() { return typeHIPoint ; }
template<> inline EventParamType wxMacGetEventParamType<HISize>() { return typeHISize ; }
template<> inline EventParamType wxMacGetEventParamType<HIRect>() { return typeHIRect ; }
template<> inline EventParamType wxMacGetEventParamType<void*>() { return typeVoidPtr ; }
#endif
template<> inline EventParamType wxMacGetEventParamType<Collection>() { return typeCollection ; }
template<> inline EventParamType wxMacGetEventParamType<CGContextRef>() { return typeCGContextRef ; }
/*
These are ambiguous
template<> EventParamType wxMacGetEventParamType<GrafPtr>() { return typeGrafPtr ; }
template<> EventParamType wxMacGetEventParamType<OSStatus>() { return typeOSStatus ; }
template<> EventParamType wxMacGetEventParamType<CFIndex>() { return typeCFIndex ; }
template<> EventParamType wxMacGetEventParamType<GWorldPtr>() { return typeGWorldPtr ; }
*/

class wxMacCarbonEvent
{
    
public :
    wxMacCarbonEvent( EventRef event ) 
    {
        m_eventRef = event ;
    }
    
    OSStatus GetParameter( EventParamName inName, EventParamType inDesiredType, UInt32 inBufferSize, void * outData) ;
    
    template <typename T> OSStatus GetParameter( EventParamName inName, EventParamType type , T *data )
    {
        return GetParameter( inName, type , sizeof( T ) , data ) ;
    }
    template <typename T> OSStatus GetParameter( EventParamName inName, T *data )
    {
        return GetParameter<T>( inName, wxMacGetEventParamType<T>() , data ) ;
    }
    
    template <typename T> T GetParameter( EventParamName inName )
    {
        T value ;
        verify_noerr( GetParameter<T>( inName, &value ) ) ;
        return value ;
    }
    template <typename T> T GetParameter( EventParamName inName, EventParamType inDesiredType )
    {
        T value ;
        verify_noerr( GetParameter<T>( inName, inDesiredType , &value ) ) ;
        return value ;
    }


    OSStatus SetParameter( EventParamName inName, EventParamType inType, UInt32 inSize, void * inData) ;
    template <typename T> OSStatus SetParameter( EventParamName inName, EventParamType inDesiredType , T *data )
    {
        return SetParameter( inName, inDesiredType , sizeof( T ) , data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, EventParamType inDesiredType , T data )
    {
        return SetParameter<T>( inName, inDesiredType , &data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, T *data )
    {
        return SetParameter<T>( inName, wxMacGetEventParamType<T>() , data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, T data )
    {
        return SetParameter<T>( inName, wxMacGetEventParamType<T>() , &data ) ;
    }
    UInt32 GetClass()
    {
        return ::GetEventClass( m_eventRef ) ;
    }
    UInt32 GetKind()
    {
        return ::GetEventKind( m_eventRef ) ;
    }
    EventTime GetTime()
    {
        return ::GetEventTime( m_eventRef ) ;
    }
    UInt32 GetTicks()
    {
        return EventTimeToTicks( GetTime() ) ;
    }
protected :
    EventRef m_eventRef ;
} ;

class wxMacControl
{
public :
    wxMacControl( ControlRef control ) 
    {
        m_controlRef = control ;
    }
    wxMacControl( WXWidget control )
    {
        m_controlRef = (ControlRef) control ;
    }
    
    OSStatus SetData( ControlPartCode inPartCode , ResType inTag , Size inSize , const void * inData ) ;
    OSStatus GetData( ControlPartCode inPartCode , ResType inTag , Size inBufferSize , void * inOutBuffer , Size * outActualSize ) ;
    OSStatus GetDataSize( ControlPartCode inPartCode , ResType inTag , Size * outActualSize ) ;
    Size GetDataSize( ControlPartCode inPartCode , ResType inTag )
    {
        Size sz ;
        verify_noerr( GetDataSize( inPartCode , inTag , &sz ) ) ;
        return sz ;
    }
    template <typename T> OSStatus SetData( ControlPartCode inPartCode , ResType inTag , T *data )
    {
        return SetData( inPartCode , inTag , sizeof( T ) , data ) ;
    }
    template <typename T> OSStatus SetData( ControlPartCode inPartCode , ResType inTag , T data )
    {
        return SetData( inPartCode , inTag , sizeof( T ) , &data ) ;
    }
    template <typename T> OSStatus GetData( ControlPartCode inPartCode , ResType inTag , T *data )
    {
        Size dummy ;
        return GetData( inPartCode , inTag , sizeof( T ) , data , &dummy ) ;
    }
    template <typename T> T GetData( ControlPartCode inPartCode , ResType inTag )
    {
        T value ;
        verify_noerr( GetData<T>( inPartCode , inTag , &value ) ) ;
        return value ;
    }
    operator ControlRef () { return m_controlRef; }   
    operator ControlRef * () { return &m_controlRef; }   
protected :
    ControlRef m_controlRef ;
} ;

#endif // wxUSE_GUI

//---------------------------------------------------------------------------
// wxMac string conversions
//---------------------------------------------------------------------------

void wxMacSetupConverters() ;
void wxMacCleanupConverters() ;

void wxMacStringToPascal( const wxString&from , StringPtr to ) ;
wxString wxMacMakeStringFromPascal( ConstStringPtr from ) ;

void wxMacConvertNewlines13To10( char * data ) ;
void wxMacConvertNewlines10To13( char * data ) ;
void wxMacConvertNewlines13To10( wxString *data ) ;
void wxMacConvertNewlines10To13( wxString *data ) ;

#if wxUSE_UNICODE
void wxMacConvertNewlines13To10( wxChar * data ) ;
void wxMacConvertNewlines10To13( wxChar * data ) ;
#endif

#if TARGET_CARBON

class wxMacCFStringHolder                                                             
{                                                                           
public:      
	wxMacCFStringHolder()
	{
    	m_cfs = NULL ;
    	m_release = false ;                                                                 
	}
	                                                               
    wxMacCFStringHolder(const wxString &str , wxFontEncoding encoding )                                          
    {      
    	m_cfs = NULL ;
    	m_release = false ;  
    	Assign( str , encoding ) ;
    }                                                                       
                                                                            
    wxMacCFStringHolder(CFStringRef ref , bool release = true )                                                   
    {                                                                       
        m_cfs = ref ;
        m_release = release ;                                           
    }                                                                       
                                                                            
    ~wxMacCFStringHolder() 
    { 
    	Release() ;
    }                                           

    CFStringRef Detach()
    {
    	CFStringRef retval = m_cfs ;
    	m_release = false ;
    	m_cfs = NULL ;
    	return retval ;
    }         
                                                                   
    void Release()
    {
    	if ( m_release && m_cfs)
    		CFRelease( m_cfs ) ;
    	m_cfs = NULL ;
    }         

	void Assign( const wxString &str , wxFontEncoding encoding ) ;

    operator CFStringRef () { return m_cfs; }   
    wxString AsString( wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;
             
private:             
    	                                                       
    CFStringRef m_cfs;
    bool m_release ;                                                        
} ;

#endif

// utils.cpp

wxUint32 wxMacGetSystemEncFromFontEnc(wxFontEncoding encoding) ;
wxFontEncoding wxMacGetFontEncFromSystemEnc(wxUint32 encoding) ;
void wxMacWakeUp() ;

// toplevel.cpp

ControlRef wxMacFindControlUnderMouse( Point location , WindowRef window , ControlPartCode *outPart ) ;

#endif
    // _WX_PRIVATE_H_
