///////////////////////////////////////////////////////////////////////////////
// Name:        mac/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/frame.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/mac/uma.h"
#include "wx/mac/aga.h"
#include "wx/tooltip.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList wxModelessWindows;

// ============================================================================
// wxTopLevelWindowMac implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxWindowMac utility functions
// ---------------------------------------------------------------------------

// Find an item given the Macintosh Window Reference

wxList *wxWinMacWindowList = NULL;
wxTopLevelWindowMac *wxFindWinFromMacWindow(WindowRef inWindowRef)
{
    wxNode *node = wxWinMacWindowList->Find((long)inWindowRef);
    if (!node)
        return NULL;
    return (wxTopLevelWindowMac *)node->Data();
}

void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxTopLevelWindowMac *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, "attempt to add a NULL WindowRef to window list" );

    if ( !wxWinMacWindowList->Find((long)inWindowRef) )
        wxWinMacWindowList->Append((long)inWindowRef, win);
}

void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win)
{
    wxWinMacWindowList->DeleteObject(win);
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac creation
// ----------------------------------------------------------------------------

WindowRef wxTopLevelWindowMac::s_macWindowInUpdate = NULL;

void wxTopLevelWindowMac::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;
    m_macNoEraseUpdateRgn = NewRgn() ;
    m_macNeedsErasing = false ;
}

bool wxTopLevelWindowMac::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    return TRUE;
}

wxTopLevelWindowMac::~wxTopLevelWindowMac()
{
    wxToolTip::NotifyWindowDelete(m_macWindow) ;
    UMADisposeWindow( m_macWindow ) ;

    wxRemoveMacWindowAssociation( this ) ;

    wxTopLevelWindows.DeleteObject(this);

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        wxTheApp->SetTopWindow(NULL);

        if ( wxTheApp->GetExitOnFrameDelete() )
        {
          wxTheApp->ExitMainLoop() ;        
        }
    }
    DisposeRgn( m_macNoEraseUpdateRgn ) ;
}


// ----------------------------------------------------------------------------
// wxTopLevelWindowMac maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::Maximize(bool maximize)
{
    // not available on mac
}

bool wxTopLevelWindowMac::IsMaximized() const
{
    return false ; 
}

void wxTopLevelWindowMac::Iconize(bool iconize)
{
    // not available on mac
}

bool wxTopLevelWindowMac::IsIconized() const
{
	// mac dialogs cannot be iconized
    return FALSE;
}

void wxTopLevelWindowMac::Restore()
{
    // not available on mac
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowMac::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);
}

void  wxTopLevelWindowMac::MacCreateRealWindow( const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name ) 
{
  SetName(name);
  m_windowStyle = style;
  m_isShown = FALSE;

  // create frame.

    Rect theBoundsRect;

  m_x = (int)pos.x;
  m_y = (int)pos.y;
  if ( m_y < 50 )
    m_y = 50 ;
  if ( m_x < 20 )
    m_x = 20 ;
    
  m_width = size.x;
    if (m_width == -1) 
        m_width = 20;
  m_height = size.y;
    if (m_height == -1) 
        m_height = 20;

    ::SetRect(&theBoundsRect, m_x, m_y , m_x + m_width, m_y + m_height);

    // translate the window attributes in the appropriate window class and attributes

    WindowClass wclass = 0;
    WindowAttributes attr = kWindowNoAttributes ;
    
    if ( HasFlag(wxTINY_CAPTION_HORIZ) ||  HasFlag(wxTINY_CAPTION_VERT) )
    {
        wclass = kFloatingWindowClass ;
        if ( HasFlag(wxTINY_CAPTION_VERT) )
        {
            attr |= kWindowSideTitlebarAttribute ;
        }
    }
    else if ( HasFlag( wxCAPTION ) )
    {
        if ( HasFlag( wxDIALOG_MODAL ) )
        {
            wclass = kMovableModalWindowClass ;
        }
        else 
        {
            wclass = kDocumentWindowClass ;
        }
    }
    else
    {
        wclass = kModalWindowClass ;
    }
    
    if ( HasFlag( wxMINIMIZE_BOX ) || HasFlag( wxMAXIMIZE_BOX ) )
    {
        attr |= kWindowFullZoomAttribute ;
        attr |= kWindowCollapseBoxAttribute ;
    }
    if ( HasFlag( wxRESIZE_BORDER ) )
    {
        attr |= kWindowResizableAttribute ;
    }
    if ( HasFlag( wxSYSTEM_MENU ) )
    {
        attr |= kWindowCloseBoxAttribute ;
    }
    
    ::CreateNewWindow( wclass , attr , &theBoundsRect , &m_macWindow ) ;
    wxAssociateWinWithMacWindow( m_macWindow , this ) ;
    wxString label ;
    if( wxApp::s_macDefaultEncodingIsPC )
        label = wxMacMakeMacStringFromPC( title ) ;
    else
        label = title ;
    UMASetWTitleC( m_macWindow , label ) ;
    ::CreateRootControl( m_macWindow , &m_macRootControl ) ;

    m_macFocus = NULL ;
}

void wxTopLevelWindowMac::MacDoGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindowMac** rootwin ) 
{
    localOrigin->h = 0;
    localOrigin->v = 0;
    clipRect->left = 0;
    clipRect->top = 0;
    clipRect->right = m_width ;//width;
    clipRect->bottom = m_height ;// height;
    *window = m_macWindow ;
    *rootwin = this ;
}

void wxTopLevelWindowMac::MacGetPortParams(Point* localOrigin, Rect* clipRect, WindowRef *window  , wxWindowMac** rootwin) 
{
    localOrigin->h = 0;
    localOrigin->v = 0;
    clipRect->left = 0;
    clipRect->top = 0;
    clipRect->right = m_width;
    clipRect->bottom = m_height;
    *window = m_macWindow ;
    *rootwin = this ;
}

void wxTopLevelWindowMac::Clear()
{
    wxMacDrawingClientHelper helper ( this ) ;
    int w ,h ;
    wxPoint origin = GetClientAreaOrigin() ;
    GetClientSize( &w , &h ) ;
    ::SetThemeWindowBackground( m_macWindow , m_macWindowBackgroundTheme , false ) ;
    Rect r = { origin.y , origin.x, origin.y+h , origin.x+w } ;
    EraseRect( &r ) ;
}

ControlHandle wxTopLevelWindowMac::MacGetContainerForEmbedding() 
{
    return m_macRootControl ;
}


void wxTopLevelWindowMac::MacUpdate( long timestamp)
{
    #if TARGET_CARBON
    AGAPortHelper help( GetWindowPort(m_macWindow) ) ;
    #else
    AGAPortHelper help( (m_macWindow) ) ;
    #endif
    SetOrigin( 0 , 0 ) ;
    BeginUpdate( m_macWindow ) ;

    RgnHandle       updateRgn = NewRgn();    
    RgnHandle       diffRgn = NewRgn() ;
    if ( updateRgn && diffRgn )
    {
        GetPortVisibleRegion( GetWindowPort( m_macWindow ), updateRgn );
        DiffRgn( updateRgn , m_macNoEraseUpdateRgn , diffRgn ) ;
        if ( !EmptyRgn( updateRgn ) )
        {
            MacRedraw( updateRgn , timestamp , m_macNeedsErasing || !EmptyRgn( diffRgn )  ) ;
        }
    }
    if ( updateRgn )
        DisposeRgn( updateRgn );
    if ( diffRgn )
        DisposeRgn( diffRgn );
    EndUpdate( m_macWindow ) ;
    SetEmptyRgn( m_macNoEraseUpdateRgn ) ;
    m_macNeedsErasing = false ;
}


// Raise the window to the top of the Z order
void wxTopLevelWindowMac::Raise()
{
    ::BringToFront( m_macWindow ) ;
}

// Lower the window to the bottom of the Z order
void wxTopLevelWindowMac::Lower()
{
    ::SendBehind( m_macWindow , NULL ) ;
}

Point lastWhere ;
long lastWhen = 0 ;
extern int wxBusyCursorCount ;

void wxTopLevelWindowMac::MacFireMouseEvent( EventRecord *ev )
{
    wxMouseEvent event(wxEVT_LEFT_DOWN);
    bool isDown = !(ev->modifiers & btnState) ; // 1 is for up
    bool controlDown = ev->modifiers & controlKey ; // for simulating right mouse
    
    event.m_leftDown = isDown && !controlDown;
    
    event.m_middleDown = FALSE;
    event.m_rightDown = isDown && controlDown;

    if ( ev->what == mouseDown )
    {
        if ( controlDown )
            event.SetEventType(wxEVT_RIGHT_DOWN ) ;
        else
            event.SetEventType(wxEVT_LEFT_DOWN ) ;
    }
    else if ( ev->what == mouseUp )
    {
        if ( controlDown )
            event.SetEventType(wxEVT_RIGHT_UP ) ;
        else
            event.SetEventType(wxEVT_LEFT_UP ) ;
    }
    else
    {
        event.SetEventType(wxEVT_MOTION ) ;
    }

    event.m_shiftDown = ev->modifiers & shiftKey;
    event.m_controlDown = ev->modifiers & controlKey;
    event.m_altDown = ev->modifiers & optionKey;
    event.m_metaDown = ev->modifiers & cmdKey;

    Point       localwhere = ev->where ;
        
    GrafPtr     port ;  
    ::GetPort( &port ) ;
    ::SetPort( UMAGetWindowPort( m_macWindow ) ) ;
    ::GlobalToLocal( &localwhere ) ;
    ::SetPort( port ) ;

    if ( ev->what == mouseDown )
    {
        if ( ev->when - lastWhen <= GetDblTime() )
        {
            if ( abs( localwhere.h - lastWhere.h ) < 3 || abs( localwhere.v - lastWhere.v ) < 3 )
            {
                if ( controlDown )
                    event.SetEventType(wxEVT_RIGHT_DCLICK ) ;
                else
                    event.SetEventType(wxEVT_LEFT_DCLICK ) ;
            }
            lastWhen = 0 ;
        }
        else
        {
            lastWhen = ev->when ;
        }
        lastWhere = localwhere ;
    }

    event.m_x = localwhere.h;
    event.m_y = localwhere.v;
    event.m_x += m_x;
    event.m_y += m_y;

/*
    wxPoint origin = GetClientAreaOrigin() ;

    event.m_x += origin.x ;
    event.m_y += origin.y ;
*/
    
    event.m_timeStamp = ev->when;
    event.SetEventObject(this);
    if ( wxTheApp->s_captureWindow )
    {
        int x = event.m_x ;
        int y = event.m_y ;
        wxTheApp->s_captureWindow->ScreenToClient( &x , &y ) ;
        event.m_x = x ;
        event.m_y = y ;
        wxTheApp->s_captureWindow->GetEventHandler()->ProcessEvent( event ) ;
        if ( ev->what == mouseUp )
        {
            wxTheApp->s_captureWindow = NULL ;
            if ( wxBusyCursorCount == 0 )
            {
                m_cursor.MacInstall() ;
            }
        }
    }
    else
    {
        MacDispatchMouseEvent( event ) ;
    }
}

void wxTopLevelWindowMac::MacMouseDown( EventRecord *ev , short part)
{
    MacFireMouseEvent( ev ) ;
}

void wxTopLevelWindowMac::MacMouseUp( EventRecord *ev , short part)
{
    switch (part)
    {
        case inContent:     
            {
                MacFireMouseEvent( ev ) ;
            }
            break ;
    }
}

void wxTopLevelWindowMac::MacMouseMoved( EventRecord *ev , short part)
{
    switch (part)
    {
        case inContent:     
            {
                MacFireMouseEvent( ev ) ;
            }
            break ;
    }
}
void wxTopLevelWindowMac::MacActivate( EventRecord *ev , bool inIsActivating )
{
    wxActivateEvent event(wxEVT_ACTIVATE, inIsActivating , m_windowId);
    event.m_timeStamp = ev->when ;
    event.SetEventObject(this);
    
    GetEventHandler()->ProcessEvent(event);
    
    UMAHighlightAndActivateWindow( m_macWindow , inIsActivating ) ;
    
    MacSuperEnabled( inIsActivating ) ;
}

void wxTopLevelWindowMac::MacKeyDown( EventRecord *ev ) 
{
}

void wxTopLevelWindowMac::SetTitle(const wxString& title)
{
    wxWindow::SetTitle( title ) ;
    
    wxString label ;
    
    if( wxApp::s_macDefaultEncodingIsPC )
        label = wxMacMakeMacStringFromPC( m_label ) ;
    else
        label = m_label ;

    UMASetWTitleC( m_macWindow , label ) ;
}

bool wxTopLevelWindowMac::Show(bool show)
{
    if ( !wxWindow::Show(show) )
        return FALSE;

    if (show)
    {
      ::ShowWindow( m_macWindow ) ;
      ::SelectWindow( m_macWindow ) ;
      // no need to generate events here, they will get them triggered by macos
      // actually they should be , but apparently they are not
      wxSize size(m_width, m_height);
      wxSizeEvent event(size, m_windowId);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
    else
    {
      ::HideWindow( m_macWindow ) ;
    }

    if ( !show )
    {
    }
    else
    {
        Refresh() ; 
    }

    return TRUE;
}

void wxTopLevelWindowMac::DoMoveWindow(int x, int y, int width, int height)
{
    int former_x = m_x ;
    int former_y = m_y ;
    int former_w = m_width ;
    int former_h = m_height ;
    
  int actualWidth = width;
  int actualHeight = height;
  int actualX = x;
  int actualY = y;
  
    if ((m_minWidth != -1) && (actualWidth < m_minWidth)) 
        actualWidth = m_minWidth;
    if ((m_minHeight != -1) && (actualHeight < m_minHeight)) 
        actualHeight = m_minHeight;
    if ((m_maxWidth != -1) && (actualWidth > m_maxWidth)) 
        actualWidth = m_maxWidth;
    if ((m_maxHeight != -1) && (actualHeight > m_maxHeight)) 
        actualHeight = m_maxHeight;

    bool doMove = false ;
    bool doResize = false ;
    
    if ( actualX != former_x || actualY != former_y )
    {
        doMove = true ;
    }
    if ( actualWidth != former_w || actualHeight != former_h )
    {
        doResize = true ;
    }

    if ( doMove || doResize )
    {
        m_x = actualX ;
        m_y = actualY ;
        m_width = actualWidth ;
        m_height = actualHeight ;

        if ( doMove )
            ::MoveWindow(m_macWindow, m_x, m_y  , false); // don't make frontmost
        
        if ( doResize )
            ::SizeWindow(m_macWindow, m_width, m_height  , true); 
        
        // the OS takes care of invalidating and erasing the new area
        // we have erased the old one   
        
        if ( IsKindOf( CLASSINFO( wxFrame ) ) )
        {
            wxFrame* frame = (wxFrame*) this ;
            frame->PositionStatusBar();
            frame->PositionToolBar();
        }
        if ( doMove )
            wxWindowMac::MacTopLevelWindowChangedPosition() ; // like this only children will be notified

        MacRepositionScrollBars() ;
        if ( doMove )
        {
            wxPoint point(m_x, m_y);
            wxMoveEvent event(point, m_windowId);
            event.SetEventObject(this);
            GetEventHandler()->ProcessEvent(event) ;
        }
        if ( doResize )
        {
             MacRepositionScrollBars() ;
             wxSize size(m_width, m_height);
             wxSizeEvent event(size, m_windowId);
             event.SetEventObject(this);
             GetEventHandler()->ProcessEvent(event);
        }
    }
    
}

/*
 * Invalidation Mechanism
 *
 * The update mechanism reflects exactely the windows mechanism
 * the rect gets added to the window invalidate region, if the eraseBackground flag
 * has been true for any part of the update rgn the background is erased in the entire region
 * not just in the specified rect.
 *
 * In order to achive this, we also have an internal m_macNoEraseUpdateRgn, all rects that have 
 * the eraseBackground flag set to false are also added to this rgn. upon receiving an update event
 * the update rgn is compared to the m_macNoEraseUpdateRgn and in case they differ, every window
 * will get the eraseBackground event first
 */
 
void wxTopLevelWindowMac::MacInvalidate( const Rect * rect, bool eraseBackground ) 
{
  GrafPtr formerPort ;
  GetPort( &formerPort ) ;
  SetPortWindowPort( m_macWindow ) ;
  
  m_macNeedsErasing |= eraseBackground ;
  
  // if we already know that we will have to erase, there's no need to track the rest
  if ( !m_macNeedsErasing)
  {
    // we end only here if eraseBackground is false
    // if we already have a difference between m_macNoEraseUpdateRgn and UpdateRgn
    // we will have to erase anyway
      
    RgnHandle       updateRgn = NewRgn();    
    RgnHandle       diffRgn = NewRgn() ;
    if ( updateRgn && diffRgn )
    {
        GetWindowUpdateRgn( m_macWindow , updateRgn );
        Point pt = {0,0} ;
        LocalToGlobal( &pt ) ;
        OffsetRgn( updateRgn , -pt.h , -pt.v ) ;
        DiffRgn( updateRgn , m_macNoEraseUpdateRgn , diffRgn ) ;
        if ( !EmptyRgn( diffRgn ) )
        {
            m_macNeedsErasing = true ;
        }
    }
    if ( updateRgn )
        DisposeRgn( updateRgn );
    if ( diffRgn )
        DisposeRgn( diffRgn );

    if ( !m_macNeedsErasing )
    {
      RgnHandle rectRgn = NewRgn() ;
      SetRectRgn( rectRgn , rect->left , rect->top , rect->right , rect->bottom ) ;
      UnionRgn( m_macNoEraseUpdateRgn , rectRgn , m_macNoEraseUpdateRgn ) ;
      DisposeRgn( rectRgn ) ;
    }
  }
  InvalWindowRect( m_macWindow , rect ) ;
  // turn this on to debug the refreshing cycle
#if 0
  PaintRect( rect ) ;
#endif
  SetPort( formerPort ) ;
}
