/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWindows itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"

#if defined(__POWERPC__) && defined(__DARWIN__)
#    include <Carbon/Carbon.h>
#    define TARGET_CARBON 1
#else
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
    GrafPtr	      m_currentPort ;
    GrafPtr	      m_oldPort ;
    RgnHandle	      m_clip ;
    ThemeDrawingState m_drawingState ;
    short	      m_textFont ;
    short	      m_textSize ;
    short	      m_textStyle ;
    short 	      m_textMode ;
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

class WXDLLEXPORT wxMacWindowClipper
{
    DECLARE_NO_COPY_CLASS(wxMacWindowClipper)
        
public:
    wxMacWindowClipper( const wxWindow* win ) ;
    ~wxMacWindowClipper() ;
private:
    RgnHandle m_formerClip ;
    RgnHandle m_newClip ;
} ;

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

// app.h
bool wxMacConvertEventToRecord( EventRef event , EventRecord *rec) ;

// filefn.h
WXDLLEXPORT wxString wxMacFSSpec2MacFilename( const FSSpec *spec ) ;
WXDLLEXPORT void wxMacFilename2FSSpec( const char *path , FSSpec *spec ) ;
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

GWorldPtr 	    wxMacCreateGWorld( int width , int height , int depth ) ;
void 		        wxMacDestroyGWorld( GWorldPtr gw ) ;
PicHandle 	    wxMacCreatePict( GWorldPtr gw , GWorldPtr mask = NULL ) ;
CIconHandle     wxMacCreateCIcon(GWorldPtr image , GWorldPtr mask , short dstDepth , short iconSize  ) ;
void 		        wxMacSetColorTableEntry( CTabHandle newColors , int index , int red , int green ,  int blue ) ;
CTabHandle 	    wxMacCreateColorTable( int numColors ) ;
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

#if TARGET_CARBON

class wxMacCFStringHolder                                                             
{                                                                           
public:      
	wxMacCFStringHolder()
	{
    	m_cfs = NULL ;
    	m_release = false ;                                                                 
	}
	                                                               
    wxMacCFStringHolder(const wxString &str)                                          
    {      
    	m_cfs = NULL ;
    	m_release = false ;  
    	Assign( str ) ;                                                               
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
      
    wxMacCFStringHolder& operator=(const wxString& str)                               
    {                                                                       
        Release() ;                                                        
		Assign( str ) ;
        return *this;                                                       
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

    operator CFStringRef () { return m_cfs; }   
    wxString AsString() ;
             
private:             
	void Assign( const wxString &str ) ;
    	                                                       
    CFStringRef m_cfs;
    bool m_release ;                                                        
} ;
#endif

#endif
    // _WX_PRIVATE_H_
