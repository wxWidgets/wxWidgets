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
#    include <FixMath.h>
#    include <CoreServices.h>
#endif

#if UNIVERSAL_INTERFACES_VERSION < 0x0342
    #error "please update to Apple's lastest universal headers from http://developer.apple.com/sdk/"
#endif

#ifndef MAC_OS_X_VERSION_10_3
    #define MAC_OS_X_VERSION_10_3 1030
#endif

#ifndef MAC_OS_X_VERSION_10_4
    #define MAC_OS_X_VERSION_10_4 1040
#endif

#ifdef __WXMAC_CARBON__
#include "wx/mac/corefoundation/cfstring.h"
#endif

#ifndef FixedToInt
// as macro in FixMath.h for 10.3
inline Fixed    IntToFixed( int inInt )
{
    return (((SInt32) inInt) << 16);
}

inline int    FixedToInt( Fixed inFixed )
{
    return (((SInt32) inFixed) >> 16);
}
#endif

#if wxUSE_GUI

#include "wx/dc.h"
#include "wx/window.h"
#include "wx/toplevel.h"

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

#if wxMAC_USE_CORE_GRAPHICS
class WXDLLEXPORT wxMacCGContextStateSaver 
{
    DECLARE_NO_COPY_CLASS(wxMacCGContextStateSaver)
        
public:
    wxMacCGContextStateSaver( CGContextRef cg ) 
    {
        m_cg = cg ;
        CGContextSaveGState( cg ) ;
    }
    ~wxMacCGContextStateSaver() 
    {
        CGContextRestoreGState( m_cg ) ;
    }
private:
    CGContextRef m_cg ;
} ;

#endif
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
WXDLLEXPORT void wxMacFilename2FSSpec( const wxString &path , FSSpec *spec ) ;

// utils.h
WXDLLEXPORT wxString wxMacFindFolder(short vRefNum,
                                     OSType folderType,
                                     Boolean createFolder);

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
template<> inline EventParamType wxMacGetEventParamType<HICommand>() { return typeHICommand ; }
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
    wxMacCarbonEvent()
    {
        m_eventRef = 0 ;
        m_release = false ;
    }
    
    wxMacCarbonEvent( EventRef event , bool release = false ) 
    {
        m_eventRef = event ;
        m_release = release ;
    }
    
    wxMacCarbonEvent(UInt32 inClassID,UInt32 inKind,EventTime inWhen = 0 /*now*/,EventAttributes inAttributes=kEventAttributeNone) 
    {
        m_eventRef = NULL ;
        verify_noerr( MacCreateEvent( NULL , inClassID, inKind,inWhen,inAttributes,&m_eventRef) ) ;
        m_release = true ;
    }
    
    ~wxMacCarbonEvent()
    {
        if ( m_release )
            ReleaseEvent( m_eventRef ) ;
    }
    
    OSStatus Create(UInt32 inClassID,UInt32 inKind,EventTime inWhen = 0 /*now*/,EventAttributes inAttributes=kEventAttributeNone) 
    {
        verify( (m_eventRef == NULL) || m_release ) ;
        if ( m_eventRef && m_release )
        {
            ReleaseEvent( m_eventRef ) ;
            m_release = false ;
            m_eventRef = NULL ;
            
        }
        OSStatus err = MacCreateEvent( NULL , inClassID, inKind,inWhen,inAttributes,&m_eventRef) ;
        if ( err == noErr )
            m_release = true ;
        return err ;
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
    
    OSStatus SetParameter( EventParamName inName, EventParamType inType, UInt32 inSize, const void * inData) ;
    template <typename T> OSStatus SetParameter( EventParamName inName, EventParamType inDesiredType , const T *data )
    {
        return SetParameter( inName, inDesiredType , sizeof( T ) , data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, EventParamType inDesiredType , const T& data )
    {
        return SetParameter<T>( inName, inDesiredType , &data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, const T *data )
    {
        return SetParameter<T>( inName, wxMacGetEventParamType<T>() , data ) ;
    }
    template <typename T> OSStatus SetParameter( EventParamName inName, const T& data )
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
    OSStatus SetTime( EventTime inWhen = 0 /*now*/ ) 
    {
        return ::SetEventTime( m_eventRef , inWhen ? inWhen : GetCurrentEventTime() ) ;
    }
    operator EventRef () { return m_eventRef; }
    
    bool IsValid() { return m_eventRef != 0 ; }  
protected :
    EventRef m_eventRef ;
    bool     m_release ;
} ;

//
// helper class for allocating and deallocating Universal Proc Ptrs
//

template <typename procType, typename uppType , uppType (*newUPP)(procType) , void (*disposeUPP)(uppType) > class wxMacUPP
{
public :
    wxMacUPP( procType proc )
    {
        m_upp = NULL ;
        m_upp = (*newUPP)( NULL ) ;
    }
    ~wxMacUPP()
    {
        if ( m_upp )
            disposeUPP( m_upp ) ;
    }
    operator uppType() { return m_upp ; }
private :
    uppType m_upp ;
} ;

typedef wxMacUPP<NMProcPtr,NMUPP,NewNMUPP,DisposeNMUPP> wxMacNMUPP ;

#if wxUSE_GUI
/*
GWorldPtr         wxMacCreateGWorld( int width , int height , int depth ) ;
void                 wxMacDestroyGWorld( GWorldPtr gw ) ;
PicHandle         wxMacCreatePict( GWorldPtr gw , GWorldPtr mask = NULL ) ;
CIconHandle     wxMacCreateCIcon(GWorldPtr image , GWorldPtr mask , short dstDepth , short iconSize  ) ;
void                 wxMacSetColorTableEntry( CTabHandle newColors , int index , int red , int green ,  int blue ) ;
CTabHandle         wxMacCreateColorTable( int numColors ) ;
*/
void wxMacCreateBitmapButton( ControlButtonContentInfo*info , const wxBitmap& bitmap , int forceType = 0 ) ;
void wxMacReleaseBitmapButton( ControlButtonContentInfo*info ) ;

#define MAC_WXCOLORREF(a) (*((RGBColor*)&(a)))
#define MAC_WXHBITMAP(a) (GWorldPtr(a))
#define MAC_WXHMETAFILE(a) (PicHandle(a))
#define MAC_WXHICON(a) (IconRef(a))
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

void wxMacRectToNative( const wxRect *wx , Rect *n ) ;
void wxMacNativeToRect( const Rect *n , wxRect* wx ) ;
void wxMacPointToNative( const wxPoint* wx , Point *n ) ;
void wxMacNativeToPoint( const Point *n , wxPoint* wx ) ;

wxWindow *              wxFindControlFromMacControl(ControlRef inControl ) ;
wxTopLevelWindowMac*    wxFindWinFromMacWindow( WindowRef inWindow ) ;
wxMenu*                 wxFindMenuFromMacMenu(MenuRef inMenuRef) ;

extern wxWindow* g_MacLastWindow ;
pascal OSStatus wxMacTopLevelMouseEventHandler( EventHandlerCallRef handler , EventRef event , void *data ) ;
Rect wxMacGetBoundsForControl( wxWindow* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin = true ) ;

ControlActionUPP GetwxMacLiveScrollbarActionProc() ;

class wxMacControl
{
public :
    wxMacControl( wxWindow* peer , bool isRootControl = false ) ;    
    wxMacControl( wxWindow* peer , ControlRef control ) ;
    wxMacControl( wxWindow* peer , WXWidget control ) ;
    virtual ~wxMacControl() ;
    
    void Init() ;
    
    virtual void Dispose() ;

    bool Ok() const { return GetControlRef() != NULL ; }
    
    virtual ControlRef * GetControlRefAddr() { return &m_controlRef; } 
    virtual ControlRef GetControlRef() const { return m_controlRef ; }  

    virtual void SetReference( SInt32 data ) ; 
    /*
    void operator= (ControlRef c) { m_controlRef = c ; }
    operator ControlRef () { return m_controlRef; }   
    operator ControlRef * () { return &m_controlRef; }   
    */
    // accessing data and values

    virtual OSStatus SetData( ControlPartCode inPartCode , ResType inTag , Size inSize , const void * inData ) ;
    virtual OSStatus GetData( ControlPartCode inPartCode , ResType inTag , Size inBufferSize , void * inOutBuffer , Size * outActualSize ) const ;
    virtual OSStatus GetDataSize( ControlPartCode inPartCode , ResType inTag , Size * outActualSize ) const ;
    virtual OSStatus SendEvent(  EventRef ref , OptionBits inOptions = 0 ) ;
    virtual OSStatus SendHICommand( HICommand &command , OptionBits inOptions = 0 ) ;

    virtual OSStatus SendHICommand( UInt32 commandID , OptionBits inOptions = 0 ) ;

    virtual SInt32 GetValue() const ;
    virtual SInt32 GetMaximum() const ;
    virtual SInt32 GetMinimum() const ;
    
    virtual void SetValue( SInt32 v ) ;
    virtual void SetMinimum( SInt32 v ) ;
    virtual void SetMaximum( SInt32 v ) ;

    virtual void SetValueAndRange( SInt32 value , SInt32 minimum , SInt32 maximum ) ;
    virtual void SetRange( SInt32 minimum , SInt32 maximum ) ;
    
    virtual OSStatus SetFocus( ControlFocusPart focusPart ) ;
    virtual bool HasFocus() const ;
    virtual bool NeedsFocusRect() const ;
    virtual void SetNeedsFocusRect( bool needs ) ;
    
    // templated helpers

    Size GetDataSize( ControlPartCode inPartCode , ResType inTag ) const
    {
        Size sz ;
        verify_noerr( GetDataSize( inPartCode , inTag , &sz ) ) ;
        return sz ;
    }
    template <typename T> OSStatus SetData( ControlPartCode inPartCode , ResType inTag , const T *data )
    {
        return SetData( inPartCode , inTag , sizeof( T ) , data ) ;
    }
    template <typename T> OSStatus SetData( ControlPartCode inPartCode , ResType inTag , const T& data )
    {
        return SetData( inPartCode , inTag , sizeof( T ) , &data ) ;
    }
    template <typename T> OSStatus GetData( ControlPartCode inPartCode , ResType inTag , T *data ) const
    {
        Size dummy ;
        return GetData( inPartCode , inTag , sizeof( T ) , data , &dummy ) ;
    }
    template <typename T> T GetData( ControlPartCode inPartCode , ResType inTag ) const
    {
        T value ;
        verify_noerr( GetData<T>( inPartCode , inTag , &value ) ) ;
        return value ;
    }
       
    // Flash the control for the specified amount of time
    virtual void Flash( ControlPartCode part , UInt32 ticks = 8 ) ;

    virtual void VisibilityChanged( bool shown ) ;
    virtual void SuperChangedPosition() ;


    virtual void SetFont( const wxFont & font , const wxColour& foreground , long windowStyle ) ;
    virtual void SetBackground( const wxBrush &brush ) ;
    virtual ControlPartCode HandleKey(  SInt16 keyCode,  SInt16 charCode, EventModifiers modifiers ) ;
    void SetActionProc( ControlActionUPP   actionProc ) ;
    void SetViewSize( SInt32 viewSize ) ;
    SInt32 GetViewSize() const ;
    
    virtual bool IsVisible() const ;
    virtual void SetVisibility( bool visible , bool redraw ) ;
    virtual bool IsEnabled() const ;
    virtual bool IsActive() const ;
    virtual void Enable( bool enable ) ;
    
    // invalidates this control and all children
    virtual void InvalidateWithChildren() ;
    virtual void SetDrawingEnabled( bool enable ) ;
#ifdef __WXMAC_OSX__
    virtual bool GetNeedsDisplay() const ;
#endif
    // where is in native window relative coordinates
    virtual void SetNeedsDisplay( RgnHandle where ) ;
    // where is in native window relative coordinates
    virtual void SetNeedsDisplay( Rect* where = NULL ) ;

	// if rect = NULL, entire view
    virtual void ScrollRect( wxRect *rect , int dx , int dy ) ;

    // in native parent window relative coordinates
    virtual void GetRect( Rect *r ) ;

    // in native parent window relative coordinates
    virtual void SetRect( Rect *r ) ;

    virtual void GetRectInWindowCoords( Rect *r ) ;
    virtual void GetBestRect( Rect *r ) ;
    virtual void SetTitle( const wxString &title ) ;
    // converts from Toplevel-Content relative to local
    static void Convert( wxPoint *pt , wxMacControl *convert , wxMacControl *to ) ;
    
    virtual void GetFeatures( UInt32 *features ) ;
    virtual OSStatus GetRegion( ControlPartCode partCode , RgnHandle region ) ;
    virtual OSStatus SetZOrder( bool above , wxMacControl* other ) ;
    // to be moved into a databrowser subclass
    
    virtual OSStatus SetSelectionFlags( DataBrowserSelectionFlags ) ;
    virtual OSStatus AddListViewColumn( DataBrowserListViewColumnDesc *columnDesc,
        DataBrowserTableViewColumnIndex position ) ;
    virtual OSStatus AutoSizeListViewColumns() ;
    virtual OSStatus SetHasScrollBars( bool horiz , bool vert ) ;
    virtual OSStatus SetTableViewHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle ) ;
    virtual OSStatus SetListViewHeaderBtnHeight(UInt16 height) ;
    virtual OSStatus SetCallbacks(const DataBrowserCallbacks *  callbacks) ;
    virtual OSStatus UpdateItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID* items,                
            DataBrowserPropertyID preSortProperty,
            DataBrowserPropertyID propertyID ) ;
    virtual OSStatus AddItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID* items,                
            DataBrowserPropertyID preSortProperty ) ;
    virtual OSStatus RemoveItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID* items,                
            DataBrowserPropertyID preSortProperty ) ;
    virtual OSStatus RevealItem( DataBrowserItemID item,
            DataBrowserPropertyID propertyID,
            DataBrowserRevealOptions options ) ;
    virtual OSStatus GetSelectionAnchor( DataBrowserItemID * first, DataBrowserItemID * last ) ;
    virtual bool IsItemSelected( DataBrowserItemID item ) ;
    virtual OSStatus SetSelectedItems(UInt32 numItems,
            const DataBrowserItemID * items,
            DataBrowserSetOption operation ) ;
            
    // to be moved into a tab control class
    
    virtual OSStatus SetTabEnabled( SInt16 tabNo , bool enable ) ;
    bool    IsCompositing() { return m_isCompositing ; }
    bool    IsRootControl() { return m_isRootControl ; }
protected :
    ControlRef  m_controlRef ;
    wxFont      m_font ;
    long        m_windowStyle ; 
    wxWindow*   m_peer ;
    bool        m_needsFocusRect ;
    bool        m_isCompositing ;
    bool        m_isRootControl ;
} ;

#if wxMAC_USE_CORE_GRAPHICS

class WXDLLEXPORT wxMacCGPath : public wxGraphicPath 
{
    DECLARE_NO_COPY_CLASS(wxMacCGPath)
public :
    wxMacCGPath() ;
    ~wxMacCGPath() ;
    
    //  Starts a new subpath at
    void MoveToPoint( wxCoord x1 , wxCoord y1 ) ;
    void AddLineToPoint( wxCoord x1 , wxCoord y1 ) ;
    void AddRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h ) ;
    void AddCircle( wxCoord x, wxCoord y , wxCoord r ) ;
    
    // closes the current subpath
    void CloseSubpath() ;
    
    CGPathRef GetPath() const ;
private :
    CGMutablePathRef m_path ;
} ;

class WXDLLEXPORT wxMacCGContext : public wxGraphicContext
{
    DECLARE_NO_COPY_CLASS(wxMacCGContext)
        
public:
    wxMacCGContext( CGrafPtr port ) ;
    wxMacCGContext( CGContextRef cgcontext ) ;
    wxMacCGContext() ;
    ~wxMacCGContext() ;

    virtual void Clip( const wxRegion &region ) ;
    virtual void StrokePath( const wxGraphicPath *p ) ;
    virtual void DrawPath( const wxGraphicPath *p , int fillStyle = wxWINDING_RULE ) ;
    virtual void FillPath( const wxGraphicPath *p , const wxColor &fillColor , int fillStyle = wxWINDING_RULE ) ;
     
    virtual wxGraphicPath* CreatePath() ;
    virtual void SetPen( const wxPen &pen )  ;    
    virtual void SetBrush( const wxBrush &brush ) ;
    CGContextRef GetNativeContext() ;
    void SetNativeContext( CGContextRef cg ) ;
    CGPathDrawingMode GetDrawingMode() const { return m_mode ; }
private:
    CGContextRef m_cgContext ;
    CGrafPtr m_qdPort ;
    CGPathDrawingMode m_mode ;
    wxPen m_pen ;
    wxBrush m_brush ;
} ;

#endif // wxMAC_USE_CORE_GRAPHICS

#ifdef __WXMAC_OSX__

CGColorSpaceRef wxMacGetGenericRGBColorSpace(void) ;
void wxMacMemoryBufferReleaseProc(void *info, const void *data, size_t size) ;

#endif

class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
{
    DECLARE_NO_COPY_CLASS(wxBitmapRefData)
        
    friend class WXDLLEXPORT wxIcon;
    friend class WXDLLEXPORT wxCursor;
public:
    wxBitmapRefData(int width , int height , int depth);
    wxBitmapRefData();
    ~wxBitmapRefData();

    void Free() ;
    bool Ok() const { return m_ok ; }
    void SetOk( bool isOk) { m_ok = isOk ; }
    
    void SetWidth( int width ) { m_width = width ; }
    void SetHeight( int height ) { m_height = height ; }
    void SetDepth( int depth ) { m_depth = depth ; }

    int GetWidth() const { return m_width ; }
    int GetHeight() const { return m_height ; }
    int GetDepth() const { return m_depth ; }
    
    void *GetRawAccess() const ;
    void *BeginRawAccess() ;
    void EndRawAccess() ;

    bool HasAlpha() const { return m_hasAlpha ; }
    void UseAlpha( bool useAlpha ) ;

public:
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE
    
    wxMask *      m_bitmapMask; // Optional mask
#ifdef __WXMAC_OSX__
    CGImageRef    CGImageCreate() const ;
#endif
     
    // returns true if the bitmap has a size that
    // can be natively transferred into a true icon
    // if no is returned GetIconRef will still produce
    // an icon but it will be generated via a PICT and
    // rescaled to 16 x 16   
    bool          HasNativeSize() ;
    
    // caller should increase ref count if needed longer
    // than the bitmap exists
    IconRef       GetIconRef() ;
    
    // returns a Pict from the bitmap content
    PicHandle     GetPictHandle() ;
    GWorldPtr     GetHBITMAP(GWorldPtr * mask = NULL ) const ;
    void          UpdateAlphaMask() const ;

private :
    bool Create(int width , int height , int depth) ;
    void Init() ;

    int           m_width;
    int           m_height;
    int           m_bytesPerRow ;
    int           m_depth;
    bool          m_hasAlpha;
    wxMemoryBuffer m_memBuf ;
    int           m_rawAccessCount ;
    bool          m_ok;
#ifdef __WXMAC_OSX__
    mutable CGImageRef    m_cgImageRef ;
#endif
    IconRef       m_iconRef ;
    PicHandle     m_pictHandle ;
    GWorldPtr     m_hBitmap;
    GWorldPtr     m_hMaskBitmap ;
    wxMemoryBuffer m_maskMemBuf ;
    int            m_maskBytesPerRow ;
};

class WXDLLEXPORT wxIconRefData : public wxGDIRefData
{
public:
    wxIconRefData() ;
    wxIconRefData( WXHICON ) ;
    virtual ~wxIconRefData() { Free(); }

    void Init() ;
    virtual void Free();

    void SetWidth( int width ) { m_width = width ; }
    void SetHeight( int height ) { m_height = height ; }

    int GetWidth() const { return m_width ; }
    int GetHeight() const { return m_height ; }

    WXHICON GetHICON() const { return (WXHICON) m_iconRef ; }
private :
    IconRef m_iconRef ;
    int m_width ;
    int m_height ;
};

// toplevel.cpp

ControlRef wxMacFindControlUnderMouse( wxTopLevelWindowMac* toplevelWindow, Point location , WindowRef window , ControlPartCode *outPart ) ;

#endif // wxUSE_GUI

#define wxMAC_DEFINE_PROC_GETTER( UPP , x ) \
UPP Get##x()                                \
{                                           \
    static UPP sHandler = NULL;             \
    if ( sHandler == NULL )                 \
        sHandler = New##UPP( x );           \
    return sHandler;                        \
}

//---------------------------------------------------------------------------
// wxMac string conversions
//---------------------------------------------------------------------------

void wxMacSetupConverters() ;
void wxMacCleanupConverters() ;

void wxMacStringToPascal( const wxString&from , StringPtr to ) ;
wxString wxMacMakeStringFromPascal( ConstStringPtr from ) ;

// filefn.cpp

wxString wxMacFSRefToPath( const FSRef *fsRef , CFStringRef additionalPathComponent = NULL ) ;
OSStatus wxMacPathToFSRef( const wxString&path , FSRef *fsRef ) ;
wxString wxMacHFSUniStrToString( ConstHFSUniStr255Param uniname ) ;

#endif
    // _WX_PRIVATE_H_
