/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h" 
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/scrolbar.h"
#include "wx/statbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/tooltip.h"
#include "wx/statusbr.h"
// TODO remove the line below, just for lookup-up convenience CS
#include "wx/window.h"

#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#define wxWINDOW_HSCROLL 5998
#define wxWINDOW_VSCROLL 5997
#define MAC_SCROLLBAR_SIZE 16

#include <wx/mac/uma.h>

#if  wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include <string.h>

extern wxList wxPendingDelete;
wxWindow* gFocusWindow = NULL ;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)
BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
//  EVT_SCROLL(wxWindow::OnScroll)
END_EVENT_TABLE()

#endif



// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWindow utility functions
// ---------------------------------------------------------------------------

// Find an item given the Macintosh Window Reference

wxList *wxWinMacWindowList = NULL;
wxWindow *wxFindWinFromMacWindow(WindowRef inWindowRef)
{
    wxNode *node = wxWinMacWindowList->Find((long)inWindowRef);
    if (!node)
        return NULL;
    return (wxWindow *)node->Data();
}

void wxAssociateWinWithMacWindow(WindowRef inWindowRef, wxWindow *win)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inWindowRef != (WindowRef) NULL, "attempt to add a NULL WindowRef to window list" );

    if ( !wxWinMacWindowList->Find((long)inWindowRef) )
        wxWinMacWindowList->Append((long)inWindowRef, win);
}

void wxRemoveMacWindowAssociation(wxWindow *win)
{
    wxWinMacWindowList->DeleteObject(win);
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindow::Init()
{
    // generic
    InitBase();

    // MSW specific
    m_doubleClickAllowed = 0;
    m_winCaptured = FALSE;

    m_isBeingDeleted = FALSE;

    m_useCtl3D = FALSE;
    m_mouseInWindow = FALSE;

    m_xThumbSize = 0;
    m_yThumbSize = 0;
    m_backgroundTransparent = FALSE;

    // as all windows are created with WS_VISIBLE style...
    m_isShown = TRUE;

	m_macWindowData = NULL ;

	m_x = 0;
	m_y = 0 ;	
	m_width = 0 ;
	m_height = 0 ;

	m_hScrollBar = NULL ;
	m_vScrollBar = NULL ;

#if  wxUSE_DRAG_AND_DROP
  m_pDropTarget = NULL;
#endif
}

// Destructor
wxWindow::~wxWindow()
{
    m_isBeingDeleted = TRUE;

	if ( s_lastMouseWindow == this )
	{
		s_lastMouseWindow = NULL ;
	}

    if ( gFocusWindow == this )
    {
    	gFocusWindow = NULL ;
    }

    if ( m_parent )
        m_parent->RemoveChild(this);

    DestroyChildren();

	if ( m_macWindowData )
	{
		wxToolTip::NotifyWindowDelete(m_macWindowData->m_macWindow) ;
    	UMADisposeWindow( m_macWindowData->m_macWindow ) ;
    	delete m_macWindowData ;
    	wxRemoveMacWindowAssociation( this ) ;
    }
}

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    parent->AddChild(this);

  	m_x = (int)pos.x;
  	m_y = (int)pos.y;
	AdjustForParentClientOrigin(m_x, m_y, wxSIZE_USE_EXISTING);
  	m_width = WidthDefault( size.x );
  	m_height = HeightDefault( size.y ) ;

	if ( ! IsKindOf( CLASSINFO ( wxControl ) ) && ! IsKindOf( CLASSINFO( wxStatusBar ) ) )
	{
    	MacCreateScrollBars( style ) ;
	}

    return TRUE;
}

void wxWindow::SetFocus()
{
	if ( AcceptsFocus() )
	{
		if (gFocusWindow )
		{
			#if wxUSE_CARET
			    // Deal with caret
			    if ( gFocusWindow->m_caret )
			    {
			          gFocusWindow->m_caret->OnKillFocus();
			    }
			#endif // wxUSE_CARET
			wxControl* control = wxDynamicCast( gFocusWindow , wxControl ) ;
			if ( control && control->GetMacControl() )
			{
				UMASetKeyboardFocus( gFocusWindow->GetMacRootWindow() , control->GetMacControl()  , kControlFocusNoPart ) ;
				control->MacRedrawControl() ;
			}
	    	wxFocusEvent event(wxEVT_KILL_FOCUS, gFocusWindow->m_windowId);
	    	event.SetEventObject(gFocusWindow);
			gFocusWindow->GetEventHandler()->ProcessEvent(event) ;
		}
		gFocusWindow = this ;
		{
			#if wxUSE_CARET
		    // Deal with caret
		    if ( m_caret )
		    {
		        m_caret->OnSetFocus();
		    }
			#endif // wxUSE_CARET
   			// panel wants to track the window which was the last to have focus in it
    		wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    		if ( panel )
    		{
        		panel->SetLastFocus(this);
    		}
			wxControl* control = wxDynamicCast( gFocusWindow , wxControl ) ;
			if ( control && control->GetMacControl() )
			{
				UMASetKeyboardFocus( gFocusWindow->GetMacRootWindow() , control->GetMacControl()  , kControlEditTextPart ) ;
			}

	    	wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
	    	event.SetEventObject(this);
			GetEventHandler()->ProcessEvent(event) ;
		}
	}
}

bool wxWindow::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();
        child->Enable(enable);

        node = node->GetNext();
    }

    return TRUE;
}

void wxWindow::CaptureMouse()
{
    wxTheApp->s_captureWindow = this ;
}

void wxWindow::ReleaseMouse()
{
    wxTheApp->s_captureWindow = NULL ;
}

#if    wxUSE_DRAG_AND_DROP

void wxWindow::SetDropTarget(wxDropTarget *pDropTarget)
{
  if ( m_pDropTarget != 0 ) {
    delete m_pDropTarget;
  }

  m_pDropTarget = pDropTarget;
  if ( m_pDropTarget != 0 )
  {
    // TODO
  }
}

#endif

// Old style file-manager drag&drop
void wxWindow::DragAcceptFiles(bool accept)
{
    // TODO
}

// Get total size
void wxWindow::DoGetSize(int *x, int *y) const
{
    *x = m_width ;
    *y = m_height ;
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
    *x = m_x ;
    *y = m_y ;
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        *x -= pt.x;
        *y -= pt.y;
    }
}


bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
	menu->SetInvokingWindow(this);
    menu->UpdateUI();
	ClientToScreen( &x , &y ) ;

	::InsertMenu( menu->GetHMenu() , -1 ) ;
  	long menuResult = ::PopUpMenuSelect(menu->GetHMenu() ,y,x, 0) ;
 	menu->MacMenuSelect( this , TickCount() , HiWord(menuResult) , LoWord(menuResult) ) ;
	::DeleteMenu( menu->MacGetMenuId() ) ;
 	menu->SetInvokingWindow(NULL);

  return TRUE;
}

void wxWindow::DoScreenToClient(int *x, int *y) const
{
	WindowRef window = GetMacRootWindow() ;

	Point		localwhere ;
	localwhere.h = * x ;
	localwhere.v = * y ;

	GrafPtr		port ;	
	::GetPort( &port ) ;
	::SetPort( UMAGetWindowPort( window ) ) ;
	::GlobalToLocal( &localwhere ) ;
	::SetPort( port ) ;

	*x = localwhere.h ;
	*y = localwhere.v ;
	
	MacRootWindowToClient( x , y ) ;
}

void wxWindow::DoClientToScreen(int *x, int *y) const
{
	WindowRef window = GetMacRootWindow() ;
	
	MacClientToRootWindow( x , y ) ;
	
	Point		localwhere ;
	localwhere.h = * x ;
	localwhere.v = * y ;
	
	GrafPtr		port ;	
	::GetPort( &port ) ;
	::SetPort( UMAGetWindowPort( window ) ) ;
	::SetOrigin( 0 , 0 ) ;
	::LocalToGlobal( &localwhere ) ;
	::SetPort( port ) ;
	*x = localwhere.h ;
	*y = localwhere.v ;
}

void wxWindow::MacClientToRootWindow( int *x , int *y ) const
{
	if ( m_macWindowData )
	{
	}
	else
	{
		*x += m_x ;
		*y += m_y ;
		GetParent()->MacClientToRootWindow( x , y ) ;
	}
}

void wxWindow::MacRootWindowToClient( int *x , int *y ) const
{
	if ( m_macWindowData )
	{
	}
	else
	{
		*x -= m_x ;
		*y -= m_y ;
		GetParent()->MacRootWindowToClient( x , y ) ;
	}
}

bool wxWindow::SetCursor(const wxCursor& cursor)
{
   if ( !wxWindowBase::SetCursor(cursor) )
   {
       // no change
       return FALSE;
   }

  wxASSERT_MSG( m_cursor.Ok(),
                  wxT("cursor must be valid after call to the base version"));

  Point pt ;
  wxWindow *mouseWin ;
  GetMouse( &pt ) ;
  
  // Change the cursor NOW if we're within the correct window

  if ( MacGetWindowFromPoint( wxPoint( pt.h , pt.v ) , &mouseWin ) )
  {
  	if ( mouseWin == this && !wxIsBusy() )
  	{
		cursor.MacInstall() ;
  	}
  }

  return TRUE ;
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::DoGetClientSize(int *x, int *y) const
{
    *x = m_width ;
    *y = m_height ;

	*x -= MacGetLeftBorderSize(  )  + MacGetRightBorderSize(  ) ;
	*y -= MacGetTopBorderSize(  ) + MacGetBottomBorderSize( );
	
  if ( (m_vScrollBar && m_vScrollBar->IsShown()) || (m_hScrollBar  && m_hScrollBar->IsShown()) )
  {
	int x1 = 0 ; 
	int y1 = 0 ;
	int w = m_width ;
	int h = m_height ;
	
	MacClientToRootWindow( &x1 , &y1 ) ;
	MacClientToRootWindow( &w , &h ) ;
	
	WindowRef window = NULL ;
	wxWindow *iter = (wxWindow*)this ;
	
	int totW = 10000 , totH = 10000;
	while( iter )
	{
		if ( iter->m_macWindowData )
		{
			totW = iter->m_width ;
			totH = iter->m_height ;
			break ;
		}

		iter = iter->GetParent() ;
	} 
	
  	if (m_hScrollBar  && m_hScrollBar->IsShown() )
  	{
  		(*y) -= MAC_SCROLLBAR_SIZE;
		if ( h-y1 >= totH )
		{
			(*y)+= 1 ;
		}
    }
  	if (m_vScrollBar  && m_vScrollBar->IsShown() )
  	{
    	(*x) -= MAC_SCROLLBAR_SIZE;
		if ( w-x1 >= totW )
		{
    		(*x) += 1 ;
    	}
  	}
  }
}


// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindow::DoSetToolTip(wxToolTip *tooltip)
{
    wxWindowBase::DoSetToolTip(tooltip);

	if ( m_tooltip )
		m_tooltip->SetWindow(this);
}

#endif // wxUSE_TOOLTIPS

void wxWindow::DoMoveWindow(int x, int y, int width, int height)
{
	DoSetSize( x,y, width, height ) ;
}

void wxWindow::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{

	int former_x = m_x ;
	int former_y = m_y ;
	int former_w = m_width ;
	int former_h = m_height ;
	
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int currentW,currentH;
  GetSize(&currentW, &currentH);

  int actualWidth = width;
  int actualHeight = height;
  int actualX = x;
  int actualY = y;
  if (x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
      actualX = currentX;
  if (y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
      actualY = currentY;
  if (width == -1)
      actualWidth = currentW ;
  if (height == -1)
      actualHeight = currentH ;

	if ( actualX == currentX && actualY == currentY && actualWidth == currentW && actualHeight == currentH)
	{
		MacRepositionScrollBars() ; // we might have a real position shift
		return ;
	}

	AdjustForParentClientOrigin(actualX, actualY, sizeFlags);
	
	
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
		if ( m_macWindowData )
		{
		}
		else
		{
			// erase former position
			wxMacDrawingHelper focus( this ) ;
			if ( focus.Ok() )
			{
		  		Rect clientrect = { 0 , 0 , m_height , m_width } ;
				ClipRect( &clientrect ) ;
		    	InvalWindowRect( GetMacRootWindow() , &clientrect ) ;
			}
		}
		m_x = actualX ;
		m_y = actualY ;
		m_width = actualWidth ;
		m_height = actualHeight ;
		if ( m_macWindowData )
		{
			if ( doMove )
				::MoveWindow(m_macWindowData->m_macWindow, m_x, m_y  , false); // don't make frontmost
			
			if ( doResize )
				::SizeWindow(m_macWindowData->m_macWindow, m_width, m_height  , true); 
			
			// the OS takes care of invalidating and erasing the new area
			// we have erased the old one	
			
			if ( IsKindOf( CLASSINFO( wxFrame ) ) )
			{
				wxFrame* frame = (wxFrame*) this ;
			  	frame->PositionStatusBar();
  				frame->PositionToolBar();
			}
		}
		else
		{
			// erase new position
			
			{
				wxMacDrawingHelper focus( this ) ;
				if ( focus.Ok() )
				{
			  		Rect clientrect = { 0 , 0 , m_height , m_width } ;
  					ClipRect( &clientrect ) ;
			    	InvalWindowRect( GetMacRootWindow() , &clientrect ) ;
				}
			}
			
			if ( doMove )
				wxWindow::MacSuperChangedPosition() ; // like this only children will be notified
		}
		MacRepositionScrollBars() ;
		if ( doMove )
		{
    		wxMoveEvent event(wxPoint(m_x, m_y), m_windowId);
    		event.SetEventObject(this);
    		GetEventHandler()->ProcessEvent(event) ;
    	}
    	if ( doResize )
    	{
			MacRepositionScrollBars() ;
	    	wxSizeEvent event(wxSize(m_width, m_height), m_windowId);
	    	event.SetEventObject(this);
	    	GetEventHandler()->ProcessEvent(event);
    	}
	}
}
// For implementation purposes - sometimes decorations make the client area
// smaller

wxPoint wxWindow::GetClientAreaOrigin() const
{
    return wxPoint(MacGetLeftBorderSize(  ) , MacGetTopBorderSize(  ) );
}

// Makes an adjustment to the window position (for example, a frame that has
// a toolbar that it manages itself).
void wxWindow::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
	if( !m_macWindowData )
	{
    if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        x += pt.x; y += pt.y;
    }
   }
}

void wxWindow::SetTitle(const wxString& title)
{
	m_label = title ;
	
	wxString label ;
	
	if( wxApp::s_macDefaultEncodingIsPC )
		label = wxMacMakeMacStringFromPC( title ) ;
	else
		label = title ;

	if ( m_macWindowData )
    	UMASetWTitleC( m_macWindowData->m_macWindow , label ) ;
}

wxString wxWindow::GetTitle() const
{
	return m_label ;
}

bool wxWindow::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

	if ( m_macWindowData )
	{
	  if (show)
	  {
	  	UMAShowWindow( m_macWindowData->m_macWindow ) ;
	  	UMASelectWindow( m_macWindowData->m_macWindow ) ;
		// no need to generate events here, they will get them triggered by macos
		// actually they should be , but apparently they are not
	    wxSizeEvent event(wxSize(m_width, m_height), m_windowId);
	    event.SetEventObject(this);
	    GetEventHandler()->ProcessEvent(event);
	  }
	  else
	  {
	  	UMAHideWindow( m_macWindowData->m_macWindow ) ;
	  }
	}
	MacSuperShown( show ) ;
	Refresh() ;
	if(m_macWindowData)
		MacUpdateImmediately() ;

    return TRUE;
}

void wxWindow::MacSuperShown( bool show ) 
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->m_isShown )
			child->MacSuperShown( show ) ;
		node = node->Next();
	}
}

bool wxWindow::MacIsReallyShown() const 
{
	bool status = m_isShown ;
	wxWindow * win = this ;
	while ( status && win->m_parent != NULL )
	{
		win = win->m_parent ;
		status = win->m_isShown ;
	}
	return status ;
}

int wxWindow::GetCharHeight() const
{
	wxClientDC dc ( (wxWindow*)this ) ;
	return dc.GetCharHeight() ;
}

int wxWindow::GetCharWidth() const
{
	wxClientDC dc ( (wxWindow*)this ) ;
	return dc.GetCharWidth() ;
}

void wxWindow::GetTextExtent(const wxString& string, int *x, int *y,
                           int *descent, int *externalLeading, const wxFont *theFont ) const
{
    const wxFont *fontToUse = theFont;
    if ( !fontToUse )
        fontToUse = &m_font;
        
    wxClientDC dc( (wxWindow*) this ) ;
    long lx,ly,ld,le ;
    dc.GetTextExtent( string , &lx , &ly , &ld, &le, fontToUse ) ;
    if ( externalLeading )
    	*externalLeading = le ;
    if ( descent )
    	*descent = ld ;
    if ( x )
    	*x = lx ;
    if ( y )
    	*y = ly ;
}

void wxWindow::MacEraseBackground( Rect *rect )
{
	WindowRef window = GetMacRootWindow() ;
	if ( m_backgroundColour == wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE) )
	{
			UMASetThemeWindowBackground( window , kThemeBrushDocumentWindowBackground , false ) ;
	}
	else if (  m_backgroundColour == wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE ) )
	{
		// on mac we have the difficult situation, that 3dface gray can be different colours, depending whether
		// it is on a notebook panel or not, in order to take care of that we walk up the hierarchy until we have
		// either a non gray background color or a non control window
		
			wxWindow* parent = GetParent() ;
			while( parent )
			{
				if ( parent->m_backgroundColour != wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE ) )
				{
					// if we have any other colours in the hierarchy
  				RGBBackColor( &parent->m_backgroundColour.GetPixel()) ;
  				break ;
				}
				if( parent->IsKindOf( CLASSINFO( wxControl ) ) && ((wxControl*)parent)->GetMacControl() )
				{
					// if we have the normal colours in the hierarchy but another control etc. -> use it's background
					if ( parent->IsKindOf( CLASSINFO( wxNotebook ) ) || parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
					{
						UMAApplyThemeBackground(kThemeBackgroundTabPane, rect, kThemeStateActive,8,true);
						break ;
					}
				}
				else
				{
					// we have arrived at a non control item
					parent = NULL ;
					break ;
				}
				parent = parent->GetParent() ;
			}
			if ( !parent )
			{
				// if there is nothing special -> use default
				UMASetThemeWindowBackground( window , kThemeBrushDialogBackgroundActive , false ) ;
			}
	}
	else
	{
  		RGBBackColor( &m_backgroundColour.GetPixel()) ;
	}

	EraseRect( rect ) ;	
	
	for (wxNode *node = GetChildren().First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		
		Rect clientrect = { child->m_x , child->m_y , child->m_x +child->m_width , child->m_y + child->m_height } ;
		SectRect( &clientrect , rect , &clientrect ) ;    	

		OffsetRect( &clientrect , -child->m_x , -child->m_y ) ;
		if ( child->GetMacRootWindow() == window && child->IsShown() )
		{
			wxMacDrawingClientHelper focus( this ) ;
			if ( focus.Ok() )
			{
				child->MacEraseBackground( &clientrect ) ;
			}
		}
	}
}

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
{
	wxMacDrawingHelper focus( this ) ;
	if ( focus.Ok() )
	{
	  	Rect clientrect = { 0 , 0 , m_height , m_width } ;
  		ClipRect( &clientrect ) ;

    	if ( rect )
    	{
    		Rect r = { rect->y , rect->x , rect->y + rect->height , rect->x + rect->width } ;
			SectRect( &clientrect , &r , &clientrect ) ;    	
    	}
    	InvalWindowRect( GetMacRootWindow() , &clientrect ) ;
	}
}

// Responds to colour changes: passes event on to children.
void wxWindow::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxNode *node = GetChildren().First();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = (wxWindow *)node->Data();
        if ( win->GetParent() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->Next();
    }
}

#if wxUSE_CARET && WXWIN_COMPATIBILITY
// ---------------------------------------------------------------------------
// Caret manipulation
// ---------------------------------------------------------------------------

void wxWindow::CreateCaret(int w, int h)
{
    SetCaret(new wxCaret(this, w, h));
}

void wxWindow::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    wxFAIL_MSG("not implemented");
}

void wxWindow::ShowCaret(bool show)
{
    wxCHECK_RET( m_caret, "no caret to show" );

    m_caret->Show(show);
}

void wxWindow::DestroyCaret()
{
    SetCaret(NULL);
}

void wxWindow::SetCaretPos(int x, int y)
{
    wxCHECK_RET( m_caret, "no caret to move" );

    m_caret->Move(x, y);
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    wxCHECK_RET( m_caret, "no caret to get position of" );

    m_caret->GetPosition(x, y);
}
#endif // wxUSE_CARET

wxWindow *wxGetActiveWindow()
{
    // actually this is a windows-only concept
    return NULL;
}

// Coordinates relative to the window
void wxWindow::WarpPointer (int x_pos, int y_pos)
{
    // We really dont move the mouse programmatically under mac
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO : probably we would adopt the EraseEvent structure
}

int wxWindow::GetScrollPos(int orient) const
{
		if ( orient == wxHORIZONTAL )
		{
			if ( m_hScrollBar )
				return m_hScrollBar->GetThumbPosition() ;
		}
		else
		{
			if ( m_vScrollBar )
				return m_vScrollBar->GetThumbPosition() ;
		}
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindow::GetScrollRange(int orient) const
{
		if ( orient == wxHORIZONTAL )
		{
			if ( m_hScrollBar )
				return m_hScrollBar->GetRange() ;
		}
		else
		{
			if ( m_vScrollBar )
				return m_vScrollBar->GetRange() ;
		}
    return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
		if ( orient == wxHORIZONTAL )
		{
			if ( m_hScrollBar )
				return m_hScrollBar->GetThumbSize() ;
		}
		else
		{
			if ( m_vScrollBar )
				return m_vScrollBar->GetThumbSize() ;
		}
    return 0;
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
		if ( orient == wxHORIZONTAL )
		{
			if ( m_hScrollBar )
				m_hScrollBar->SetThumbPosition( pos ) ;
		}
		else
		{
			if ( m_vScrollBar )
				m_vScrollBar->SetThumbPosition( pos ) ;
		}
}

void  wxWindow::MacCreateRealWindow( const wxString& title,
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

	m_macWindowData = new MacWindowData() ;

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
	else if ( HasFlag( wxTHICK_FRAME ) )
	{
		if ( HasFlag( wxDIALOG_MODAL ) )
		{
			wclass = kMovableModalWindowClass ;
		}
		else if ( HasFlag( wxDIALOG_MODELESS ) )
		{
			wclass = kDocumentWindowClass ;
		}
		else
		{
			if ( HasFlag( wxCAPTION ) )
			{
				wclass = kDocumentWindowClass ;
			}
			else
			{
				wclass = kModalWindowClass ;
			}
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
	
	UMACreateNewWindow( wclass , attr , &theBoundsRect , &m_macWindowData->m_macWindow ) ;
	wxAssociateWinWithMacWindow( m_macWindowData->m_macWindow , this ) ;
	wxString label ;
	if( wxApp::s_macDefaultEncodingIsPC )
		label = wxMacMakeMacStringFromPC( title ) ;
	else
		label = title ;
	UMASetWTitleC( m_macWindowData->m_macWindow , label ) ;
	UMACreateRootControl( m_macWindowData->m_macWindow , &m_macWindowData->m_macRootControl ) ;

	m_macWindowData->m_macFocus = NULL ;
}

void wxWindow::MacPaint( wxPaintEvent &event ) 
{
}

void wxWindow::MacPaintBorders( ) 
{
	if( m_macWindowData )
		return ;
		
	RGBColor white = { 0xFFFF, 0xFFFF , 0xFFFF } ;
	RGBColor black = { 0x0000, 0x0000 , 0x0000 } ;
	RGBColor face = { 0xDDDD, 0xDDDD , 0xDDDD } ;
	RGBColor shadow = { 0x4444, 0x4444 , 0x4444 } ;
	PenNormal() ;

    if (HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) || HasFlag(wxDOUBLE_BORDER) )
    {
    	bool sunken = HasFlag( wxSUNKEN_BORDER ) ;
    	RGBColor pen1 = sunken ? white : black ;
    	RGBColor pen2 = sunken ? shadow : face ;
    	RGBColor pen3 = sunken ? face : shadow ;
    	RGBColor pen4 = sunken ? black : white ;
    	
    	RGBForeColor( &pen1 ) ;
    	{
			Rect rect = { 0 , 0 , m_height , m_width } ;
    		FrameRect( &rect ) ;
    	}
    	RGBForeColor( &pen2 ) ;
    	{
			Rect rect = { 1 , 1 , m_height -1 , m_width -1} ;
    		FrameRect( &rect ) ;
    	}
    	RGBForeColor( &pen3 ) ;
    	{
			Rect rect = { 0 , 0 , m_height -2 , m_width -2} ;
    		FrameRect( &rect ) ;
    	}
    	RGBForeColor( &pen4 ) ;
    	{
    		MoveTo( 0 , 0 ) ;
    		LineTo( m_width - 3 , 0 ) ;
    		MoveTo( 0 , 0 ) ;
    		LineTo( 0 , m_height - 3 ) ;
    	}
    }
    else if (HasFlag(wxSIMPLE_BORDER))
    {
		Rect rect = { 0 , 0 , m_height , m_width } ;
		RGBForeColor( &black ) ;
    	FrameRect( &rect ) ;
    }
/*
	if ( this->GetParent() )
	{
	    wxPaintDC dc(GetParent());
	    GetParent()->PrepareDC(dc);
	
	    if (HasFlag(wxRAISED_BORDER) || HasFlag( wxSUNKEN_BORDER) )
	    {
	    	bool sunken = HasFlag( wxSUNKEN_BORDER ) ;
	
			wxPen m_penButton3DShadow( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_3DSHADOW ), 1, wxSOLID ) ;
			wxPen m_penButton3DFace( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_3DFACE ), 1, wxSOLID ) ;
	    	
			wxPen wxPen1 = sunken ? *wxWHITE_PEN : *wxBLACK_PEN;
			wxPen wxPen2 = sunken ? m_penButton3DShadow : m_penButton3DShadow;
			wxPen wxPen3 = sunken ? m_penButton3DFace : m_penButton3DShadow;
			wxPen wxPen4 = sunken ? *wxBLACK_PEN : *wxWHITE_PEN;
		
			dc.SetPen(wxPen1);
			dc.DrawRectangle(m_x, m_y, m_width, m_height);          // outer - right and button
		
		    dc.SetPen(wxPen2);
			dc.DrawRectangle(m_x+1, m_y+1, m_width-1, m_height-1);      // outer - left and top
		
		    dc.SetPen(wxPen3);
			dc.DrawRectangle(m_x, m_y, m_width-2, m_height-2);          // inner - right and button
		
		    dc.SetPen(wxPen4);
			dc.DrawLine(m_x, m_y, m_x + m_width-3, m_y);                 // inner - left and top
			dc.DrawLine(m_x, m_y, m_x, m_y + m_height-3);
	    }
	    else if (HasFlag(wxDOUBLE_BORDER))
	    {
	    	bool sunken = HasFlag( wxSUNKEN_BORDER ) ;
	
			wxPen m_penButton3DShadow( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_3DSHADOW ), 1, wxSOLID ) ;
			wxPen m_penButton3DFace( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_3DFACE ), 1, wxSOLID ) ;
	    	
			wxPen wxPen1 = sunken ? *wxWHITE_PEN : *wxBLACK_PEN;
			wxPen wxPen2 = sunken ? m_penButton3DShadow : m_penButton3DShadow;
			wxPen wxPen3 = sunken ? m_penButton3DFace : m_penButton3DShadow;
			wxPen wxPen4 = sunken ? *wxBLACK_PEN : *wxWHITE_PEN;
		
			dc.SetPen(wxPen1);
			dc.DrawRectangle(m_x, m_y, m_width, m_height);          // outer - right and button
		
		    dc.SetPen(wxPen2);
			dc.DrawRectangle(m_x+1, m_y+1, m_width-1, m_height-1);      // outer - left and top
		
		    dc.SetPen(wxPen3);
			dc.DrawRectangle(m_x, m_y, m_width-2, m_height-2);          // inner - right and button
		
		    dc.SetPen(wxPen4);
			dc.DrawLine(m_x, m_y, m_x + m_width-3, m_y);                 // inner - left and top
			dc.DrawLine(m_x, m_y, m_x, m_y + m_height-3);
	    }
	    else if (HasFlag(wxSIMPLE_BORDER))
	    {
			dc.SetPen(*wxBLACK_PEN);
			dc.DrawRectangle(m_x, m_y, m_width, m_height);         
	    }
    }
 */
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
		if ( orient == wxHORIZONTAL )
		{
			if ( m_hScrollBar )
			{
				if ( range == 0 || thumbVisible >= range )
				{
					if ( m_hScrollBar->IsShown() )
						m_hScrollBar->Show(false) ;
				}
				else
				{
					if ( !m_hScrollBar->IsShown() )
						m_hScrollBar->Show(true) ;
					m_hScrollBar->SetScrollbar( pos , thumbVisible , range , refresh ) ;
				}
			}
		}
		else
		{
			if ( m_vScrollBar )
			{
				if ( range == 0 || thumbVisible >= range )
				{
					if ( m_vScrollBar->IsShown() )
						m_vScrollBar->Show(false) ;
				}
				else
				{
					if ( !m_vScrollBar->IsShown() )
						m_vScrollBar->Show(true) ;
					m_vScrollBar->SetScrollbar( pos , thumbVisible , range , refresh ) ;
				}
			}
		}
		MacRepositionScrollBars() ;
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRect *rect)
{
	wxMacDrawingClientHelper focus( this ) ;
	if ( focus.Ok() )
	{
  		int width , height ;
  		GetClientSize( &width , &height ) ;

		Rect scrollrect = { 0 , 0 , height , width } ;
  	
  		RgnHandle updateRgn = NewRgn() ;
  		ClipRect( &scrollrect ) ;
    	if ( rect )
    	{
    		Rect r = { rect->y , rect->x , rect->y + rect->height , rect->x + rect->width } ;
			SectRect( &scrollrect , &r , &scrollrect ) ;    	
    	}
   		ScrollRect( &scrollrect , dx , dy , updateRgn ) ;
    	InvalWindowRgn( GetMacRootWindow() ,  updateRgn ) ;
    	DisposeRgn( updateRgn ) ;
	}
	
	for (wxNode *node = GetChildren().First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		if (child == m_vScrollBar) continue;
		if (child == m_hScrollBar) continue;
		if (child->IsTopLevel()) continue;
		int x,y;
		child->GetPosition( &x, &y );
		int w,h;
		child->GetSize( &w, &h );
		child->SetSize( x+dx, y+dy, w, h );
	}
	
}

void wxWindow::MacOnScroll(wxScrollEvent &event )
{
	if ( event.m_eventObject == m_vScrollBar || event.m_eventObject == m_hScrollBar )
	{
	    wxScrollWinEvent wevent;
	    wevent.SetPosition(event.GetPosition());
	    wevent.SetOrientation(event.GetOrientation());
	    wevent.m_eventObject = this;
	
    	if (event.m_eventType == wxEVT_SCROLL_TOP) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_TOP;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_BOTTOM) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_BOTTOM;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_LINEUP) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_LINEUP;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_LINEDOWN) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_LINEDOWN;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_PAGEUP) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_PAGEUP;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_PAGEDOWN) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_PAGEDOWN;
	    } else
    	if (event.m_eventType == wxEVT_SCROLL_THUMBTRACK) {
	        wevent.m_eventType = wxEVT_SCROLLWIN_THUMBTRACK;
	    }
		
	    GetEventHandler()->ProcessEvent(wevent);
    }
}

bool wxWindow::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    return TRUE;
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
	return gFocusWindow ;
}

#if WXWIN_COMPATIBILITY
// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    if ( GetEventHandler()->ProcessEvent(event)  )
        return;
    if ( m_parent )
        m_parent->GetEventHandler()->OnCommand(win, event);
}
#endif // WXWIN_COMPATIBILITY_2

#if WXWIN_COMPATIBILITY
wxObject* wxWindow::GetChild(int number) const
{
    // Return a pointer to the Nth object in the Panel
    wxNode *node = GetChildren().First();
    int n = number;
    while (node && n--)
        node = node->Next();
    if ( node )
    {
        wxObject *obj = (wxObject *)node->Data();
        return(obj);
    }
    else
        return NULL;
}
#endif // WXWIN_COMPATIBILITY

void wxWindow::Clear()
{
	if ( m_macWindowData )
	{
		wxMacDrawingClientHelper helper ( this ) ;
		int w ,h ;
		wxPoint origin = GetClientAreaOrigin() ;
		GetClientSize( &w , &h ) ;
		UMASetThemeWindowBackground( m_macWindowData->m_macWindow , m_macWindowData->m_macWindowBackgroundTheme , false ) ;
		Rect r = { origin.y , origin.x, origin.y+h , origin.x+w } ;
		EraseRect( &r ) ;
	}
	else
	{
		wxClientDC dc(this);
    	wxBrush brush(GetBackgroundColour(), wxSOLID);
    	dc.SetBackground(brush);
    	dc.Clear();
	}
}

// Setup background and foreground colours correctly
void wxWindow::SetupColours()
{
    if ( GetParent() )
        SetBackgroundColour(GetParent()->GetBackgroundColour());
}

void wxWindow::OnIdle(wxIdleEvent& event)
{
/*
   // Check if we need to send a LEAVE event
   if (m_mouseInWindow)
   {
       POINT pt;
       ::GetCursorPos(&pt);
       if (::WindowFromPoint(pt) != (HWND) GetHWND())
       {
           // Generate a LEAVE event
           m_mouseInWindow = FALSE;
           MSWOnMouseLeave(pt.x, pt.y, 0);
       }
    }
*/

    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
	UpdateWindowUI();
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
    // TODO
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    // TODO
}

void wxWindow::DoSetClientSize(int width, int height)
{
	if ( width != -1 || height != -1 )
	{
		
		if ( width != -1 && m_vScrollBar )
			width += MAC_SCROLLBAR_SIZE ;
		if ( height != -1 && m_vScrollBar )
			height += MAC_SCROLLBAR_SIZE ;

		width += MacGetLeftBorderSize(  ) + MacGetRightBorderSize( ) ;
		height += MacGetTopBorderSize(  ) + MacGetBottomBorderSize( ) ;

		DoSetSize( -1 , -1 , width , height ) ;
	}
}


wxWindow* wxWindow::s_lastMouseWindow = NULL ;

bool wxWindow::MacGetWindowFromPointSub( const wxPoint &point , wxWindow** outWin ) 
{
	if ((point.x < m_x) || (point.y < m_y) ||
		(point.x > (m_x + m_width)) || (point.y > (m_y + m_height)))
		return FALSE;
	
	WindowRef window = GetMacRootWindow() ;

	wxPoint newPoint( point ) ;

	newPoint.x -= m_x;
	newPoint.y -= m_y;
	
	for (wxNode *node = GetChildren().First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		// added the m_isShown test --dmazzoni
		if ( child->GetMacRootWindow() == window && child->m_isShown )
		{
			if (child->MacGetWindowFromPointSub(newPoint , outWin ))
				return TRUE;
		}
	}

	*outWin = this ;
	return TRUE;
}

bool wxWindow::MacGetWindowFromPoint( const wxPoint &screenpoint , wxWindow** outWin ) 
{
	WindowRef window ;
	Point pt = { screenpoint.y , screenpoint.x } ;
	if ( ::FindWindow( pt , &window ) == 3 )
	{
			wxPoint point( screenpoint ) ;
			wxWindow* win = wxFindWinFromMacWindow( window ) ;
			if ( win )
			{
			win->ScreenToClient( point ) ;
			return win->MacGetWindowFromPointSub( point , outWin ) ;
	}
	}
	return FALSE ;
}

extern int wxBusyCursorCount ;

bool wxWindow::MacDispatchMouseEvent(wxMouseEvent& event)
{
	if ((event.m_x < m_x) || (event.m_y < m_y) ||
		(event.m_x > (m_x + m_width)) || (event.m_y > (m_y + m_height)))
		return FALSE;
	

	if ( IsKindOf( CLASSINFO ( wxStaticBox ) ) )
		return FALSE ; 
	
	WindowRef window = GetMacRootWindow() ;

	event.m_x -= m_x;
	event.m_y -= m_y;
	
	int x = event.m_x ;
	int y = event.m_y ;

	for (wxNode *node = GetChildren().First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		if ( child->GetMacRootWindow() == window && child->IsShown() && child->IsEnabled() )
		{
			if (child->MacDispatchMouseEvent(event))
				return TRUE;
		}
	}

	event.m_x = x ;
	event.m_y = y ;
	
	if ( wxBusyCursorCount == 0 )
	{
		m_cursor.MacInstall() ;
	}

#if wxUSE_TOOLTIPS
    if ( event.GetEventType() == wxEVT_MOTION 
        	|| event.GetEventType() == wxEVT_ENTER_WINDOW
        	|| event.GetEventType() == wxEVT_LEAVE_WINDOW )
        wxToolTip::RelayEvent( this , event);
#endif // wxUSE_TOOLTIPS
	GetEventHandler()->ProcessEvent( event ) ;
	return TRUE;
}

Point lastWhere ;
long lastWhen = 0 ;

wxString wxWindow::MacGetToolTipString( wxPoint &pt )
{
	if ( m_tooltip )
	{
		return m_tooltip->GetTip() ;
	}
	return "" ;
}
void wxWindow::MacFireMouseEvent( EventRecord *ev )
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

	Point		localwhere = ev->where ;
		
	GrafPtr		port ;	
	::GetPort( &port ) ;
	::SetPort( UMAGetWindowPort( m_macWindowData->m_macWindow ) ) ;
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
		}
		lastWhen = ev->when ;
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

void wxWindow::MacMouseDown( EventRecord *ev , short part)
{
	MacFireMouseEvent( ev ) ;
}

void wxWindow::MacMouseUp( EventRecord *ev , short part)
{
	WindowPtr frontWindow ;
	switch (part)
	{
		case inContent:		
			{
				MacFireMouseEvent( ev ) ;
			}
			break ;
	}
}

void wxWindow::MacMouseMoved( EventRecord *ev , short part)
{
	WindowPtr frontWindow ;
	switch (part)
	{
		case inContent:		
			{
				MacFireMouseEvent( ev ) ;
			}
			break ;
	}
}
void wxWindow::MacActivate( EventRecord *ev , bool inIsActivating )
{
	wxActivateEvent event(wxEVT_ACTIVATE, inIsActivating);
	event.m_timeStamp = ev->when ;
	event.SetEventObject(this);
	
	GetEventHandler()->ProcessEvent(event);
	
	UMAHighlightAndActivateWindow( m_macWindowData->m_macWindow , inIsActivating ) ;
}

void wxWindow::MacRedraw( RgnHandle updatergn , long time)
{
	// updatergn is always already clipped to our boundaries
	WindowRef window = GetMacRootWindow() ;
	wxWindow* win = wxFindWinFromMacWindow( window ) ;
	{
		wxMacDrawingHelper focus( this ) ; // was client
		if ( focus.Ok() )
		{
			WindowRef window = GetMacRootWindow() ;
			bool eraseBackground = false ;
			if ( m_macWindowData )
				eraseBackground = true ;
			if ( m_backgroundColour == wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE) )
			{
					UMASetThemeWindowBackground( window , kThemeBrushDocumentWindowBackground , false ) ;
			}
			else if (  m_backgroundColour == wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE ) )
			{
					// on mac we have the difficult situation, that 3dface gray can be different colours, depending whether
					// it is on a notebook panel or not, in order to take care of that we walk up the hierarchy until we have
					// either a non gray background color or a non control window
		

					wxWindow* parent = GetParent() ;
					while( parent )
					{
						if ( parent->GetMacRootWindow() != window )
						{
							// we are in a different window on the mac system
							parent = NULL ;
							break ;
						}
						
						if( parent->IsKindOf( CLASSINFO( wxControl ) ) && ((wxControl*)parent)->GetMacControl() )
						{
							if ( parent->m_backgroundColour != wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE ) )
							{
								// if we have any other colours in the hierarchy
			  					RGBBackColor( &parent->m_backgroundColour.GetPixel()) ;
			  					break ;
							}
							// if we have the normal colours in the hierarchy but another control etc. -> use it's background
							if ( parent->IsKindOf( CLASSINFO( wxNotebook ) ) || parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
							{
								Rect box ;
								GetRegionBounds( updatergn , &box) ;
								UMAApplyThemeBackground(kThemeBackgroundTabPane, &box , kThemeStateActive,8,true);
								break ;
							}
						}
						else
						{
							parent = NULL ;
							break ;
						}
						parent = parent->GetParent() ;
					}
					if ( !parent )
					{
						// if there is nothing special -> use default
						UMASetThemeWindowBackground( window , kThemeBrushDialogBackgroundActive , false ) ;
					}
			}
			else
			{
		  		RGBBackColor( &m_backgroundColour.GetPixel()) ;
			}
	  		if ( GetParent() && m_backgroundColour != GetParent()->GetBackgroundColour() )
	  			eraseBackground = true ;
			SetClip( updatergn ) ;
			if ( eraseBackground )
			{
				EraseRgn( updatergn ) ;	
			}
		}

		m_macUpdateRgn = updatergn ;
		{
			RgnHandle newupdate = NewRgn() ;
			wxSize point = GetClientSize() ;
			wxPoint origin = GetClientAreaOrigin() ;

			SetRectRgn( newupdate , origin.x , origin.y , origin.x + point.x , origin.y+point.y ) ;
			SectRgn( newupdate , m_macUpdateRgn , newupdate ) ;
			OffsetRgn( newupdate , -origin.x , -origin.y ) ;
			m_updateRegion = newupdate ;
			DisposeRgn( newupdate ) ;
		}

		MacPaintBorders() ;
		wxPaintEvent event;
		event.m_timeStamp = time ;
		event.SetEventObject(this);
		GetEventHandler()->ProcessEvent(event);
	}
	
	
	RgnHandle childupdate = NewRgn() ;
			
	for (wxNode *node = GetChildren().First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		SetRectRgn( childupdate , child->m_x , child->m_y , child->m_x + child->m_width ,  child->m_y + child->m_height ) ;
		SectRgn( childupdate , m_macUpdateRgn , childupdate ) ;
		OffsetRgn( childupdate , -child->m_x , -child->m_y ) ;
		if ( child->GetMacRootWindow() == window && child->IsShown() && !EmptyRgn( childupdate ) )
		{
			// because dialogs may also be children
			child->MacRedraw( childupdate , time ) ;
		}
	}
	DisposeRgn( childupdate ) ;
	// eventually a draw grow box here
}

void wxWindow::MacUpdateImmediately()
{
	WindowRef window = GetMacRootWindow() ;
	if ( window )
	{
		wxWindow* win = wxFindWinFromMacWindow( window ) ;
		#if TARGET_CARBON
		AGAPortHelper help( GetWindowPort(window) ) ;
		#else
		AGAPortHelper help( (window) ) ;
		#endif
		SetOrigin( 0 , 0 ) ;
		BeginUpdate( window ) ;
		if ( win )
		{
			RgnHandle		region = NewRgn();
			
			if ( region )
			{
	            GetPortVisibleRegion( GetWindowPort( window ), region );

				// if windowshade gives incompatibility , take the follwing out
	            if ( !EmptyRgn( region ) )
	            {
					win->MacRedraw( region , wxTheApp->sm_lastMessageTime ) ;
	            }
	            DisposeRgn( region );
	        }
		}
		EndUpdate( window ) ;
	}
}

void wxWindow::MacUpdate( EventRecord *ev )
{
	WindowRef window = (WindowRef) ev->message ;
	wxWindow * win = wxFindWinFromMacWindow( window ) ;
	#if TARGET_CARBON
	AGAPortHelper help( GetWindowPort(window) ) ;
	#else
	AGAPortHelper help( (window) ) ;
	#endif
	SetOrigin( 0 , 0 ) ;
	BeginUpdate( window ) ;
	if ( win )
	{
		RgnHandle		region = NewRgn();
		
		if ( region )
		{
            GetPortVisibleRegion( GetWindowPort( window ), region );

			// if windowshade gives incompatibility , take the follwing out
            if ( !EmptyRgn( region ) )
            {
				MacRedraw( region , ev->when ) ;
            }
            DisposeRgn( region );
        }
	}
	EndUpdate( window ) ;
}

WindowRef wxWindow::GetMacRootWindow() const
{
	WindowRef window = NULL ;
	wxWindow *iter = (wxWindow*)this ;
	
	while( iter )
	{
		if ( iter->m_macWindowData )
			return iter->m_macWindowData->m_macWindow ;

		iter = iter->GetParent() ;
	} 
	wxASSERT_MSG( 1 , "No valid mac root window" ) ;
	return NULL ;
}

void wxWindow::MacCreateScrollBars( long style ) 
{
	wxASSERT_MSG( m_vScrollBar == NULL && m_hScrollBar == NULL , "attempt to create window twice" ) ;
	
	bool hasBoth = ( style & wxVSCROLL ) && ( style & wxHSCROLL ) ;
	int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1: 0 ;
	int width, height ;
	GetClientSize( &width , &height ) ;
	
	wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
	wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
	wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
	wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;
	
	m_vScrollBar = new wxScrollBar(this, wxWINDOW_VSCROLL, vPoint, 
		vSize , wxVERTICAL);

	if ( style & wxVSCROLL )
	{
		
	}
	else
	{
		m_vScrollBar->Show(false) ;
	}
	m_hScrollBar = new wxScrollBar(this, wxWINDOW_HSCROLL, hPoint, 
		hSize , wxHORIZONTAL);
	if ( style  & wxHSCROLL )
	{
	}
	else
	{
		m_hScrollBar->Show(false) ;
	}
	
	// because the create does not take into account the client area origin
	MacRepositionScrollBars() ; // we might have a real position shift
}

void wxWindow::MacRepositionScrollBars()
{
	bool hasBoth = ( m_hScrollBar && m_hScrollBar->IsShown()) && ( m_vScrollBar && m_vScrollBar->IsShown()) ;
	int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1 : 0 ;
	
	// get real client area
	
	int width = m_width ; 
	int height = m_height ;

	width -= MacGetLeftBorderSize() + MacGetRightBorderSize();
	height -= MacGetTopBorderSize() + MacGetBottomBorderSize();
	
	wxPoint vPoint(width-MAC_SCROLLBAR_SIZE, 0) ;
	wxSize vSize(MAC_SCROLLBAR_SIZE, height - adjust) ;
	wxPoint hPoint(0 , height-MAC_SCROLLBAR_SIZE ) ;
	wxSize hSize( width - adjust, MAC_SCROLLBAR_SIZE) ;
	
	int x = 0 ; 
	int y = 0 ;
	int w = m_width ;
	int h = m_height ;
	
	MacClientToRootWindow( &x , &y ) ;
	MacClientToRootWindow( &w , &h ) ;
	
	WindowRef window = NULL ;
	wxWindow *iter = (wxWindow*)this ;
	
	int totW = 10000 , totH = 10000;
	while( iter )
	{
		if ( iter->m_macWindowData )
		{
			totW = iter->m_width ;
			totH = iter->m_height ;
			break ;
		}

		iter = iter->GetParent() ;
	} 
	
	if ( x == 0 )
	{
		hPoint.x = -1 ;
		hSize.x += 1 ;
	}
	if ( y == 0 )
	{
		vPoint.y = -1 ;
		vSize.y += 1 ;
	}
	
	if ( w-x >= totW )
	{
		hSize.x += 1 ;
		vPoint.x += 1 ;
	}
	
	if ( h-y >= totH )
	{
		vSize.y += 1 ;
		hPoint.y += 1 ;
	}

	if ( m_vScrollBar )
	{
		m_vScrollBar->SetSize( vPoint.x , vPoint.y, vSize.x, vSize.y , wxSIZE_ALLOW_MINUS_ONE);
	}
	if ( m_hScrollBar )
	{
		m_hScrollBar->SetSize( hPoint.x , hPoint.y, hSize.x, hSize.y, wxSIZE_ALLOW_MINUS_ONE);
	}
}

void wxWindow::MacKeyDown( EventRecord *ev ) 
{

}


bool wxWindow::AcceptsFocus() const
{
    return MacCanFocus() && wxWindowBase::AcceptsFocus();
}

ControlHandle wxWindow::MacGetContainerForEmbedding() 
{
	if ( m_macWindowData )
		return m_macWindowData->m_macRootControl ;
	else
		return GetParent()->MacGetContainerForEmbedding() ;
}

void wxWindow::MacSuperChangedPosition() 
{
	// only window-absolute structures have to be moved i.e. controls

	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		child->MacSuperChangedPosition() ;
		node = node->Next();
	}
}
/*

bool wxWindow::MacSetupFocusPort(  ) 
{
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	GrafPtr port ;
	
	MacGetPortParams( &localOrigin , &clipRect , &window , &rootwin) ;
	return 	MacSetPortFocusParams( localOrigin, clipRect, window , rootwin ) ; 
}

bool wxWindow::MacSetupFocusClientPort(  ) 
{
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	GrafPtr port ;
	
	MacGetPortClientParams( &localOrigin , &clipRect , &window , &rootwin) ;
	return 	MacSetPortFocusParams( localOrigin, clipRect, window , rootwin ) ; 
}

bool wxWindow::MacSetupDrawingPort(  ) 
{
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	GrafPtr port ;
	
	MacGetPortParams( &localOrigin , &clipRect , &window , &rootwin) ;
	return 	MacSetPortDrawingParams( localOrigin, clipRect, window , rootwin ) ; 
}

bool wxWindow::MacSetupDrawingClientPort(  ) 
{
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	GrafPtr port ;
	
	MacGetPortClientParams( &localOrigin , &clipRect , &window , &rootwin) ;
	return 	MacSetPortDrawingParams( localOrigin, clipRect, window , rootwin ) ; 
}
*/

bool wxWindow::MacSetPortFocusParams( const Point & localOrigin, const Rect & clipRect, WindowRef window , wxWindow* win ) 
{
	if ( window == NULL )
		return false ;
		
	GrafPtr currPort;
	GrafPtr port ;

	::GetPort(&currPort);
	port = UMAGetWindowPort( window) ;
	if (currPort != port )
			::SetPort(port);
				
//	wxASSERT( port->portRect.left == 0 && port->portRect.top == 0 ) ; 
	::SetOrigin(-localOrigin.h, -localOrigin.v);
	return true;			
}

bool wxWindow::MacSetPortDrawingParams( const Point & localOrigin, const Rect & clipRect, WindowRef window , wxWindow* win ) 
{
	if ( window == NULL )
		return false ;
		
	GrafPtr currPort;
	GrafPtr port ;
	::GetPort(&currPort);
	port = UMAGetWindowPort( window) ;
	if (currPort != port )
			::SetPort(port);
//	wxASSERT( port->portRect.left == 0 && port->portRect.top == 0 ) ; 
	::SetOrigin(-localOrigin.h, -localOrigin.v);
	::ClipRect(&clipRect);

	::PenNormal() ;
	::RGBBackColor(& win->GetBackgroundColour().GetPixel() ) ;
	::RGBForeColor(& win->GetForegroundColour().GetPixel() ) ;
	Pattern whiteColor ;
	
	::BackPat( GetQDGlobalsWhite( &whiteColor) ) ;
	::UMASetThemeWindowBackground(  win->m_macWindowData->m_macWindow , win->m_macWindowData->m_macWindowBackgroundTheme ,  false ) ;
	return true;			
}

void wxWindow::MacGetPortParams(Point* localOrigin, Rect* clipRect, WindowRef *window  , wxWindow** rootwin) 
{
	if ( m_macWindowData )
	{
		localOrigin->h = 0;
		localOrigin->v = 0;
		clipRect->left = 0;
		clipRect->top = 0;
		clipRect->right = m_width;
		clipRect->bottom = m_height;
		*window = m_macWindowData->m_macWindow ;
		*rootwin = this ;
	}
	else
	{
		wxASSERT( GetParent() != NULL ) ;
		GetParent()->MacGetPortParams( localOrigin , clipRect , window, rootwin) ;
		localOrigin->h += m_x;
		localOrigin->v += m_y;
		OffsetRect(clipRect, -m_x, -m_y);
	
		Rect myClip;
		myClip.left = 0;
		myClip.top = 0;
		myClip.right = m_width;
		myClip.bottom = m_height;
		SectRect(clipRect, &myClip, clipRect);
	}
}

void wxWindow::MacDoGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindow** rootwin ) 
{
//	int width , height ;
//	GetClientSize( &width , &height ) ;
	
	if ( m_macWindowData )
	{
		localOrigin->h = 0;
		localOrigin->v = 0;
		clipRect->left = 0;
		clipRect->top = 0;
		clipRect->right = m_width ;//width;
		clipRect->bottom = m_height ;// height;
		*window = m_macWindowData->m_macWindow ;
		*rootwin = this ;
	}
	else
	{
		wxASSERT( GetParent() != NULL ) ;
		
		GetParent()->MacDoGetPortClientParams( localOrigin , clipRect , window, rootwin) ;

		localOrigin->h += m_x;
		localOrigin->v += m_y;
		OffsetRect(clipRect, -m_x, -m_y);
	
		Rect myClip;
		myClip.left = 0;
		myClip.top = 0;
		myClip.right = m_width ;//width;
		myClip.bottom = m_height ;// height;
		SectRect(clipRect, &myClip, clipRect);
	}
}

void wxWindow::MacGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindow** rootwin ) 
{
	MacDoGetPortClientParams( localOrigin , clipRect , window , rootwin ) ;

  	int width , height ;
  	GetClientSize( &width , &height ) ;
  	wxPoint client ;
	client = GetClientAreaOrigin( ) ;
	
	localOrigin->h += client.x;
	localOrigin->v += client.y;
	OffsetRect(clipRect, -client.x, -client.y);

	Rect myClip;
	myClip.left = 0;
	myClip.top = 0;
	myClip.right = width;
	myClip.bottom = height;
	SectRect(clipRect, &myClip, clipRect);
}

long wxWindow::MacGetLeftBorderSize( ) const
{
	if( m_macWindowData )
		return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
		return 2 ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
		return 2 ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
    	return 1 ;
    }
	return 0 ;
}

long wxWindow::MacGetRightBorderSize( ) const
{
	if( m_macWindowData )
		return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
		return 3 ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
		return 3 ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
    	return 3 ;
    }
	return 0 ;
}

long wxWindow::MacGetTopBorderSize( ) const
{
	if( m_macWindowData )
		return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
		return 2 ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
		return 2 ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
    	return 1 ;
    }
	return 0 ;
}

long wxWindow::MacGetBottomBorderSize( ) const
{
	if( m_macWindowData )
		return 0 ;

    if (m_windowStyle & wxRAISED_BORDER || m_windowStyle & wxSUNKEN_BORDER )
    {
		return 3 ;
    }
    else if (  m_windowStyle &wxDOUBLE_BORDER)
    {
		return 3 ;
    }
    else if (m_windowStyle &wxSIMPLE_BORDER)
    {
    	return 3 ;
    }
	return 0 ;
}

long wxWindow::MacRemoveBordersFromStyle( long style ) 
{
	return style & ~( wxDOUBLE_BORDER | wxSUNKEN_BORDER | wxRAISED_BORDER | wxBORDER | wxSTATIC_BORDER ) ;
}


wxMacDrawingHelper::wxMacDrawingHelper( wxWindow * theWindow ) 
{
	m_ok = false ;
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	m_currentPort = NULL ;
	
	GetPort( &m_formerPort ) ;
	if ( theWindow )
	{
		theWindow->MacGetPortParams( &localOrigin , &clipRect , &window , &rootwin) ;
		m_currentPort = UMAGetWindowPort( window ) ;
		if ( m_formerPort != m_currentPort )
			SetPort( m_currentPort ) ;
		GetPenState( &m_savedPenState ) ;
		theWindow->MacSetPortDrawingParams( localOrigin, clipRect, window , rootwin ) ; 
		m_ok = true ;
	}
}
	
wxMacDrawingHelper::~wxMacDrawingHelper() 
{
	if ( m_ok )
	{
		SetPort( m_currentPort ) ;
		SetPenState( &m_savedPenState ) ;
		SetOrigin( 0 , 0 ) ;
		Rect portRect ;
		GetPortBounds( m_currentPort , &portRect ) ;
		ClipRect( &portRect ) ;
		wxDC::MacInvalidateSetup() ;	
	}
		
	if ( m_formerPort != m_currentPort )
		SetPort( m_formerPort ) ;
}

wxMacDrawingClientHelper::wxMacDrawingClientHelper( wxWindow * theWindow ) 
{
	m_ok = false ;
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *rootwin ;
	m_currentPort = NULL ;
	
	GetPort( &m_formerPort ) ;

	if ( theWindow )
	{
		theWindow->MacGetPortClientParams( &localOrigin , &clipRect , &window , &rootwin) ;
		m_currentPort = UMAGetWindowPort( window ) ;
		if ( m_formerPort != m_currentPort )
			SetPort( m_currentPort ) ;
		GetPenState( &m_savedPenState ) ;
		theWindow->MacSetPortDrawingParams( localOrigin, clipRect, window , rootwin ) ; 
		m_ok = true ;
	}
}
	
wxMacDrawingClientHelper::~wxMacDrawingClientHelper() 
{
	if ( m_ok )
	{
		SetPort( m_currentPort ) ;
		SetPenState( &m_savedPenState ) ;
		SetOrigin( 0 , 0 ) ;
		Rect portRect ;
		GetPortBounds( m_currentPort , &portRect ) ;
		ClipRect( &portRect ) ;
	}
		
	if ( m_formerPort != m_currentPort )
		SetPort( m_formerPort ) ;
}

// Find the wxWindow at the current mouse position, returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    wxWindow* found = wxFindWindowAtPoint(pt);
    return found;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    int x, y;
    wxGetMousePosition(& x, & y);
    return wxPoint(x, y);
}

