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
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
// TODO remove the line below, just for lookup-up convenience CS
#include "wx/mac/window.h"

#include "wx/menuitem.h"
#include "wx/log.h"

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
  EVT_CHAR(wxWindow::OnChar)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
  EVT_PAINT(wxWindow::OnPaint)
END_EVENT_TABLE()

#endif


// Constructor
wxWindow::wxWindow()
{
	Init() ;
}

void wxWindow::Init()
{
	m_macWindowData = NULL ;
  m_isWindow = TRUE;
	m_x = 0;
	m_y = 0 ;	
	m_width = 0 ;
	m_height = 0 ;
	// these are the defaults for MSW
	m_macShown = true ;
	m_macEnabled = true ;
  // Generic
  m_windowId = 0;
  m_windowStyle = 0;
  m_windowParent = NULL;
  m_windowEventHandler = this;
  m_windowName = "";
  m_windowCursor = *wxSTANDARD_CURSOR;
  m_children = new wxWindowList;
  m_constraints = NULL;
  m_constraintsInvolvedIn = NULL;
  m_windowSizer = NULL;
  m_sizerParent = NULL;
  m_autoLayout = FALSE;
  m_windowValidator = NULL;
  m_defaultItem = NULL;
  m_returnCode = 0;
  m_caretWidth = 0; m_caretHeight = 0;
  m_caretEnabled = FALSE;
  m_caretShown = FALSE;
  m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;
  // m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
  m_foregroundColour = *wxBLACK;
	m_hScrollBar = NULL ;
	m_vScrollBar = NULL ;
  m_mouseInWindow = FALSE;

#if  wxUSE_DRAG_AND_DROP
  m_pDropTarget = NULL;
#endif
}

// Destructor
wxWindow::~wxWindow()
{
	if ( s_lastMouseWindow == this )
	{
		s_lastMouseWindow = NULL ;
	}
	// Have to delete constraints/sizer FIRST otherwise
	// sizers may try to look at deleted windows as they
	// delete themselves.
#if wxUSE_CONSTRAINTS
    DeleteRelatedConstraints();
    if (m_constraints)
    {
        // This removes any dangling pointers to this window
        // in other windows' constraintsInvolvedIn lists.
        UnsetConstraints(m_constraints);
        delete m_constraints;
        m_constraints = NULL;
    }
    if (m_windowSizer)
    {
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
    // If this is a child of a sizer, remove self from parent
    if (m_sizerParent)
        m_sizerParent->RemoveChild((wxWindow *)this);
#endif

		if ( FindFocus() == this )
		{
			// really a bad thing - maybe an error ?
			// we cannot even send it a kill focus message at this stage
			gFocusWindow = NULL ;
		}

    if (m_windowParent)
        m_windowParent->RemoveChild(this);

    DestroyChildren();

		if ( m_macWindowData )
		{
    	UMADisposeWindow( m_macWindowData->m_macWindow ) ;
    	delete m_macWindowData ;
    	wxRemoveMacWindowAssociation( this ) ;
    }

    delete m_children;
    m_children = NULL;

    // Just in case the window has been Closed, but
    // we're then deleting immediately: don't leave
    // dangling pointers.
    wxPendingDelete.DeleteObject(this);

    if ( m_windowValidator )
	    delete m_windowValidator;
}

// Destroy the window (delayed, if a managed window)
bool wxWindow::Destroy()
{
    delete this;
    return TRUE;
}

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
   m_isWindow = TRUE;
     // Generic
    m_windowId = 0;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;

#if wxUSE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif

    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_minSizeX = -1;
    m_minSizeY = -1;
    m_maxSizeX = -1;
    m_maxSizeY = -1;
    m_defaultItem = NULL;
    m_windowParent = NULL;
    if (!parent)
        return FALSE;

    if (parent) parent->AddChild(this);

    m_returnCode = 0;

    SetName(name);

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
			m_windowId = id;

    // m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;
    m_foregroundColour = *wxBLACK;

    m_windowStyle = style;

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
			m_windowId = id;

  	m_x = (int)pos.x;
  	m_y = (int)pos.y;
		AdjustForParentClientOrigin(m_x, m_y, wxSIZE_USE_EXISTING);
  	m_width = size.x;
	  m_height = size.y;

    MacCreateScrollBars( style ) ;

    return TRUE;
}

void wxWindow::SetFocus()
{
	if ( AcceptsFocus() )
	{
		if (gFocusWindow )
		{
			wxControl* control = wxDynamicCast( gFocusWindow , wxControl ) ;
			if ( control && control->GetMacControl() )
			{
				UMASetKeyboardFocus( gFocusWindow->GetMacRootWindow() , control->GetMacControl()  , kControlFocusNoPart ) ;
			}
	    wxFocusEvent event(wxEVT_KILL_FOCUS, gFocusWindow->m_windowId);
	    event.SetEventObject(gFocusWindow);
			gFocusWindow->GetEventHandler()->ProcessEvent(event) ;
		}
		gFocusWindow = this ;
		{
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

void wxWindow::Enable(bool enable)
{
	if ( m_macEnabled == enable )
		return ;
		
  m_macEnabled = enable ;
		
	MacSuperEnabled( enable ) ;
  return;
}

void wxWindow::CaptureMouse()
{
    wxTheApp->s_captureWindow = this ;
}

void wxWindow::ReleaseMouse()
{
    wxTheApp->s_captureWindow = NULL ;
}

// Push/pop event handler (i.e. allow a chain of event handlers
// be searched)
void wxWindow::PushEventHandler(wxEvtHandler *handler)
{
	handler->SetNextHandler(GetEventHandler());
	SetEventHandler(handler);
}

wxEvtHandler *wxWindow::PopEventHandler(bool deleteHandler)
{
	if ( GetEventHandler() )
	{
		wxEvtHandler *handlerA = GetEventHandler();
		wxEvtHandler *handlerB = handlerA->GetNextHandler();
		handlerA->SetNextHandler(NULL);
		SetEventHandler(handlerB);
		if ( deleteHandler )
		{
			delete handlerA;
			return NULL;
		}
		else
			return handlerA;
	}
	else
		return NULL;
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
void wxWindow::GetSize(int *x, int *y) const
{
    *x = m_width ;
    *y = m_height ;
}

void wxWindow::GetPosition(int *x, int *y) const
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

void wxWindow::ScreenToClient(int *x, int *y) const
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

void wxWindow::ClientToScreen(int *x, int *y) const
{
	WindowRef window = GetMacRootWindow() ;
	
	MacClientToRootWindow( x , y ) ;
	
	Point		localwhere ;
	localwhere.h = * x ;
	localwhere.v = * y ;
	
	GrafPtr		port ;	
	::GetPort( &port ) ;
	::SetPort( UMAGetWindowPort( window ) ) ;
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

void wxWindow::SetCursor(const wxCursor& cursor)
{
  m_windowCursor = cursor;
  if (m_windowCursor.Ok())
  {
		// since this only affects the window-cursor, we adopt the same
		// behaviour as windows -> it will only change on mouse moved events
		// otherwise the ::WxSetCursor routine will have to be used
  }
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::GetClientSize(int *x, int *y) const
{
    *x = m_width ;
    *y = m_height ;

  if (m_vScrollBar && m_vScrollBar->IsShown() )
  	(*x) -= MAC_SCROLLBAR_SIZE;
  if (m_hScrollBar  && m_hScrollBar->IsShown() )
  	(*y) -= MAC_SCROLLBAR_SIZE;
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
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
      actualX = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
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
			{
				wxMacDrawingClientHelper focus( this ) ;
				if ( focus.Ok() )
				{
			  	Rect clientrect = { 0 , 0 , m_height , m_width } ;
			    InvalRect( &clientrect ) ;
				}
			}
		}
		m_x = actualX ;
		m_y = actualY ;
		m_width = actualWidth ;
		m_height = actualHeight ;
		if ( m_macWindowData )
		{
			if ( doMove )
				::MoveWindow(m_macWindowData->m_macWindow, m_x, m_y, false); // don't make frontmost
			
			if ( doResize )
				::SizeWindow(m_macWindowData->m_macWindow, m_width, m_height, true); 
			
			// the OS takes care of invalidating and erasing	
			
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
				wxMacDrawingClientHelper focus( this ) ;
				if ( focus.Ok() )
				{
			  	Rect clientrect = { 0 , 0 , m_height , m_width } ;
			    InvalRect( &clientrect ) ;
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
    return wxPoint(0, 0);
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
	if ( m_macWindowData )
	{
    	char title[256] ;
		wxString label ;
    	UMAGetWTitleC( m_macWindowData->m_macWindow , title ) ;
		if( wxApp::s_macDefaultEncodingIsPC )
			label = wxMacMakePCStringFromMac( title ) ;
		else
			label = title ;
    	return label;
  	}
  
  return wxEmptyString ;
}

void wxWindow::Centre(int direction)
{
  int x_offset,y_offset ;
  int display_width, display_height;
  int  width, height, x, y;
  wxWindow *parent = GetParent();
  if ((direction & wxCENTER_FRAME) && parent)
  {
      parent->GetPosition(&x_offset,&y_offset) ;
      parent->GetSize(&display_width,&display_height) ;
  }
  else
  {
    wxDisplaySize(&display_width, &display_height);
    x_offset = 0 ;
    y_offset = LMGetMBarHeight() + LMGetMBarHeight() / 2 ; // approx. the window title height
  }

  GetSize(&width, &height);
  GetPosition(&x, &y);

  if (direction & wxHORIZONTAL)
    x = (int)((display_width - width)/2);
  if (direction & wxVERTICAL)
    y = (int)((display_height - height)/2);

  SetSize(x+x_offset, y+y_offset, width, height);
}


bool wxWindow::Show(bool show)
{
	if ( m_macShown == show )
		return TRUE ;
		
	m_macShown = show ;
	if ( m_macWindowData )
	{
	  if (show)
	  {
	  	UMAShowWindow( m_macWindowData->m_macWindow ) ;
	  	UMASelectWindow( m_macWindowData->m_macWindow ) ;
			// no need to generate events here, they will get them triggered by macos
	    wxSizeEvent event(wxSize(m_width, m_height), m_windowId);
	    event.SetEventObject(this);
	    GetEventHandler()->ProcessEvent(event);
	  }
	  else
	  {
	  	UMAHideWindow( m_macWindowData->m_macWindow ) ;
	  }
	}
	Refresh() ;
	MacSuperShown( show ) ;
  return TRUE;
}

bool wxWindow::IsShown() const
{
    return m_macShown;
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
                           int *descent, int *externalLeading, const wxFont *theFont, bool) const
{
  wxFont *fontToUse = (wxFont *)theFont;
  if (!fontToUse)
    fontToUse = (wxFont *) & m_windowFont;

    // TODO
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
						ApplyThemeBackground			(kThemeBackgroundTabPane, rect, kThemeStateActive,8,true);
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
	
	for (wxNode *node = m_children->First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
//			int width ;
//			int height ;
		
//			child->GetClientSize( &width , &height ) ;
		
		Rect clientrect = { child->m_x , child->m_y , child->m_x +child->m_width , child->m_y + child->m_height } ;
		SectRect( &clientrect , rect , &clientrect ) ;    	

		OffsetRect( &clientrect , -child->m_x , -child->m_y ) ;
		if ( child->GetMacRootWindow() == window && child->IsReallyShown() )
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
	wxMacDrawingClientHelper focus( this ) ;
	if ( focus.Ok() )
	{
  	int width , height ;
  	GetClientSize( &width , &height ) ;
  	Rect clientrect = { 0 , 0 , height , width } ;
  	ClipRect( &clientrect ) ;

    if ( rect )
    {
    	Rect r = { rect->y , rect->x , rect->y + rect->height , rect->x + rect->width } ;
			SectRect( &clientrect , &r , &clientrect ) ;    	
    }
    InvalRect( &clientrect ) ;
    /*
    if ( eraseBack )
    {
    	MacEraseBackground( &clientrect ) ;
   	}
   	*/
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

// This can be called by the app (or wxWindows) to do default processing for the current
// event. Save message/event info in wxWindow so they can be used in this function.
long wxWindow::Default()
{
    // TODO
    return 0;
}

void wxWindow::InitDialog()
{
  wxInitDialogEvent event(GetId());
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

// Default init dialog behaviour is to transfer data to window
void wxWindow::OnInitDialog(wxInitDialogEvent& event)
{
  TransferDataToWindow();
}

// Caret manipulation
void wxWindow::CreateCaret(int w, int h)
{
  m_caretWidth = w;
  m_caretHeight = h;
  m_caretEnabled = TRUE;
}

void wxWindow::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    // TODO
}

void wxWindow::ShowCaret(bool show)
{
    // TODO
}

void wxWindow::DestroyCaret()
{
    // TODO
    m_caretEnabled = FALSE;
}

void wxWindow::SetCaretPos(int x, int y)
{
    // TODO
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    // TODO
}

wxWindow *wxGetActiveWindow()
{
    // actually this is a windows-only concept
    return NULL;
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int WXUNUSED(incW), int WXUNUSED(incH))
{
  m_minSizeX = minW;
  m_minSizeY = minH;
  m_maxSizeX = maxW;
  m_maxSizeY = maxH;
}


// Coordinates relative to the window
void wxWindow::WarpPointer (int x_pos, int y_pos)
{
    // We really dont move the mouse programmatically under mac
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO : probably we would adopt the EraseEvent structure
    Default();
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
    InvalRgn( updateRgn ) ;
    DisposeRgn( updateRgn ) ;
	}
}

void wxWindow::SetFont(const wxFont& font)
{
    m_windowFont = font;

    if (!m_windowFont.Ok())
	    return;
    // TODO
}

void wxWindow::OnChar(wxKeyEvent& event)
{
    if ( event.KeyCode() == WXK_TAB ) {
        // propagate the TABs to the parent - it's up to it to decide what
        // to do with it
        if ( GetParent() ) {
            if ( GetParent()->ProcessEvent(event) )
                return;
        }
    }
}

void wxWindow::OnPaint(wxPaintEvent& event)
{
/*
	if ( m_macWindowData )
	{
		wxMacDrawingClientHelper helper ( this ) ;
		long x ,y ,w ,h ;
		GetUpdateRegion().GetBox( x , y , w , h ) ;
		UMASetThemeWindowBackground( m_macWindowData->m_macWindow , m_macWindowData->m_macWindowBackgroundTheme , false ) ;
		Rect r = { y , x, y+h , x+w } ;
		EraseRect( &r ) ;
	}
	else
	{
		wxMacDrawingClientHelper helper ( this ) ;
		long x ,y ,w ,h ;
		GetUpdateRegion().GetBox( x , y , w , h ) ;
		RGBBackColor( &m_backgroundColour.GetPixel() ) ;
		Rect r = { y , x, y+h , x+w } ;
		EraseRect( &r ) ;
	}
*/
}

bool wxWindow::IsEnabled() const
{
	return m_macEnabled ;
}

// Dialog support: override these and call
// base class members to add functionality
// that can't be done using validators.
// NOTE: these functions assume that controls
// are direct children of this window, not grandchildren
// or other levels of descendant.

// Transfer values to controls. If returns FALSE,
// it's an application error (pops up a dialog)
bool wxWindow::TransferDataToWindow()
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() &&
		     !child->GetValidator()->TransferToWindow() )
		{
			wxMessageBox("Application Error", "Could not transfer data to window", wxOK|wxICON_EXCLAMATION);
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Transfer values from controls. If returns FALSE,
// validation failed: don't quit
bool wxWindow::TransferDataFromWindow()
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && !child->GetValidator()->TransferFromWindow() )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

bool wxWindow::Validate()
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->Validate(this) )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Get the window with the focus
wxWindow *wxWindow::FindFocus()
{
	return gFocusWindow ;
}

// ----------------------------------------------------------------------------
// RTTI
// ----------------------------------------------------------------------------

bool wxWindow::IsTopLevel() const
{
    return wxDynamicCast(this, wxFrame) || wxDynamicCast(this, wxDialog);
}

void wxWindow::AddChild(wxWindow *child)
{
    GetChildren().Append(child);
    child->m_windowParent = this;
}

void wxWindow::RemoveChild(wxWindow *child)
{
    GetChildren().DeleteObject(child);
    child->m_windowParent = NULL;
}

void wxWindow::DestroyChildren()
{
    wxNode *node;
    while ((node = GetChildren().First()) != (wxNode *)NULL) {
      wxWindow *child;
      if ((child = (wxWindow *)node->Data()) != (wxWindow *)NULL) {
        delete child;
		if ( GetChildren().Find(child) )
			delete node;
      }
    } /* while */
}

void wxWindow::MakeModal(bool modal)
{
  // Disable all other windows
  if (this->IsKindOf(CLASSINFO(wxDialog)) || this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxNode *node = wxTopLevelWindows.First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win != this)
        win->Enable(!modal);

      node = node->Next();
    }
  }
}

// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (GetEventHandler()->ProcessEvent(event) )
    return;
  if (m_windowParent)
    m_windowParent->GetEventHandler()->OnCommand(win, event);
}

// ----------------------------------------------------------------------------
// constraints and sizers
// ----------------------------------------------------------------------------

#if wxUSE_CONSTRAINTS

void wxWindow::SetConstraints( wxLayoutConstraints *constraints )
{
    if ( m_constraints )
    {
        UnsetConstraints(m_constraints);
        delete m_constraints;
    }
    m_constraints = constraints;
    if ( m_constraints )
    {
        // Make sure other windows know they're part of a 'meaningful relationship'
        if ( m_constraints->left.GetOtherWindow() && (m_constraints->left.GetOtherWindow() != this) )
            m_constraints->left.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->top.GetOtherWindow() && (m_constraints->top.GetOtherWindow() != this) )
            m_constraints->top.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->right.GetOtherWindow() && (m_constraints->right.GetOtherWindow() != this) )
            m_constraints->right.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->bottom.GetOtherWindow() && (m_constraints->bottom.GetOtherWindow() != this) )
            m_constraints->bottom.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->width.GetOtherWindow() && (m_constraints->width.GetOtherWindow() != this) )
            m_constraints->width.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->height.GetOtherWindow() && (m_constraints->height.GetOtherWindow() != this) )
            m_constraints->height.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->centreX.GetOtherWindow() && (m_constraints->centreX.GetOtherWindow() != this) )
            m_constraints->centreX.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->centreY.GetOtherWindow() && (m_constraints->centreY.GetOtherWindow() != this) )
            m_constraints->centreY.GetOtherWindow()->AddConstraintReference(this);
    }
}

// This removes any dangling pointers to this window in other windows'
// constraintsInvolvedIn lists.
void wxWindow::UnsetConstraints(wxLayoutConstraints *c)
{
    if ( c )
    {
        if ( c->left.GetOtherWindow() && (c->top.GetOtherWindow() != this) )
            c->left.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->top.GetOtherWindow() && (c->top.GetOtherWindow() != this) )
            c->top.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->right.GetOtherWindow() && (c->right.GetOtherWindow() != this) )
            c->right.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->bottom.GetOtherWindow() && (c->bottom.GetOtherWindow() != this) )
            c->bottom.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->width.GetOtherWindow() && (c->width.GetOtherWindow() != this) )
            c->width.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->height.GetOtherWindow() && (c->height.GetOtherWindow() != this) )
            c->height.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->centreX.GetOtherWindow() && (c->centreX.GetOtherWindow() != this) )
            c->centreX.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->centreY.GetOtherWindow() && (c->centreY.GetOtherWindow() != this) )
            c->centreY.GetOtherWindow()->RemoveConstraintReference(this);
    }
}

// Back-pointer to other windows we're involved with, so if we delete this
// window, we must delete any constraints we're involved with.
void wxWindow::AddConstraintReference(wxWindow *otherWin)
{
    if ( !m_constraintsInvolvedIn )
        m_constraintsInvolvedIn = new wxWindowList;
    if ( !m_constraintsInvolvedIn->Find(otherWin) )
        m_constraintsInvolvedIn->Append(otherWin);
}

// REMOVE back-pointer to other windows we're involved with.
void wxWindow::RemoveConstraintReference(wxWindow *otherWin)
{
    if ( m_constraintsInvolvedIn )
        m_constraintsInvolvedIn->DeleteObject(otherWin);
}

// Reset any constraints that mention this window
void wxWindow::DeleteRelatedConstraints()
{
    if ( m_constraintsInvolvedIn )
    {
        wxWindowList::Node *node = m_constraintsInvolvedIn->GetFirst();
        while (node)
        {
            wxWindow *win = node->GetData();
            wxLayoutConstraints *constr = win->GetConstraints();

            // Reset any constraints involving this window
            if ( constr )
            {
                constr->left.ResetIfWin(this);
                constr->top.ResetIfWin(this);
                constr->right.ResetIfWin(this);
                constr->bottom.ResetIfWin(this);
                constr->width.ResetIfWin(this);
                constr->height.ResetIfWin(this);
                constr->centreX.ResetIfWin(this);
                constr->centreY.ResetIfWin(this);
            }

            wxWindowList::Node *next = node->GetNext();
            delete node;
            node = next;
        }

        delete m_constraintsInvolvedIn;
        m_constraintsInvolvedIn = (wxWindowList *) NULL;
    }
}

void wxWindow::SetSizer(wxSizer *sizer)
{
    if (m_windowSizer) delete m_windowSizer;

    m_windowSizer = sizer;
}

bool wxWindow::Layout()
{
    int w, h;
    GetClientSize(&w, &h);
    
    // If there is a sizer, use it instead of the constraints
    if ( GetSizer() )
    {
        GetSizer()->SetDimension( 0, 0, w, h );
        return TRUE;
    }
    
    if ( GetConstraints() )
    {
        GetConstraints()->width.SetValue(w);
        GetConstraints()->height.SetValue(h);
    }
	
    // Evaluate child constraints
    ResetConstraints();   // Mark all constraints as unevaluated
    DoPhase(1);           // Just one phase need if no sizers involved
    DoPhase(2);
    SetConstraintSizes(); // Recursively set the real window sizes
    
    return TRUE;
}


// Do a phase of evaluating constraints: the default behaviour. wxSizers may
// do a similar thing, but also impose their own 'constraints' and order the
// evaluation differently.
bool wxWindow::LayoutPhase1(int *noChanges)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        return constr->SatisfyConstraints(this, noChanges);
    }
    else
        return TRUE;
}

bool wxWindow::LayoutPhase2(int *noChanges)
{
    *noChanges = 0;

    // Layout children
    DoPhase(1);
    DoPhase(2);
    return TRUE;
}

// Do a phase of evaluating child constraints
bool wxWindow::DoPhase(int phase)
{
    int noIterations = 0;
    int maxIterations = 500;
    int noChanges = 1;
    int noFailures = 0;
    wxWindowList succeeded;
    while ((noChanges > 0) && (noIterations < maxIterations))
    {
        noChanges = 0;
        noFailures = 0;
        wxWindowList::Node *node = GetChildren().GetFirst();
        while (node)
        {
            wxWindow *child = node->GetData();
            if ( !child->IsTopLevel() )
            {
                wxLayoutConstraints *constr = child->GetConstraints();
                if ( constr )
                {
                    if ( !succeeded.Find(child) )
                    {
                        int tempNoChanges = 0;
                        bool success = ( (phase == 1) ? child->LayoutPhase1(&tempNoChanges) : child->LayoutPhase2(&tempNoChanges) ) ;
                        noChanges += tempNoChanges;
                        if ( success )
                        {
                            succeeded.Append(child);
                        }
                    }
                }
            }
            node = node->GetNext();
        }

        noIterations++;
    }

    return TRUE;
}

void wxWindow::ResetConstraints()
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        constr->left.SetDone(FALSE);
        constr->top.SetDone(FALSE);
        constr->right.SetDone(FALSE);
        constr->bottom.SetDone(FALSE);
        constr->width.SetDone(FALSE);
        constr->height.SetDone(FALSE);
        constr->centreX.SetDone(FALSE);
        constr->centreY.SetDone(FALSE);
    }
    wxWindowList::Node *node = GetChildren().GetFirst();
    while (node)
    {
        wxWindow *win = node->GetData();
        if ( !win->IsTopLevel() )
            win->ResetConstraints();
        node = node->GetNext();
    }
}

// Need to distinguish between setting the 'fake' size for windows and sizers,
// and setting the real values.
void wxWindow::SetConstraintSizes(bool recurse)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr && constr->left.GetDone() && constr->right.GetDone( ) &&
            constr->width.GetDone() && constr->height.GetDone())
    {
        int x = constr->left.GetValue();
        int y = constr->top.GetValue();
        int w = constr->width.GetValue();
        int h = constr->height.GetValue();

        if ( (constr->width.GetRelationship() != wxAsIs ) ||
             (constr->height.GetRelationship() != wxAsIs) )
        {
            SetSize(x, y, w, h);
        }
        else
        {
            // If we don't want to resize this window, just move it...
            Move(x, y);
        }
    }
    else if ( constr )
    {
        char *windowClass = GetClassInfo()->GetClassName();

        wxString winName;
        if ( GetName() == _T("") )
            winName = _T("unnamed");
        else
            winName = GetName();
        wxLogDebug( _T("Constraint(s) not satisfied for window of type %s, name %s:\n"),
                (const char *)windowClass,
                (const char *)winName);
        if ( !constr->left.GetDone()) wxLogDebug( _T("  unsatisfied 'left' constraint.\n")  );
        if ( !constr->right.GetDone()) wxLogDebug( _T("  unsatisfied 'right' constraint.\n")  );
        if ( !constr->width.GetDone()) wxLogDebug( _T("  unsatisfied 'width' constraint.\n")  );
        if ( !constr->height.GetDone())  wxLogDebug( _T("  unsatisfied 'height' constraint.\n")  );
        wxLogDebug( _T("Please check constraints: try adding AsIs() constraints.\n") );
    }

    if ( recurse )
    {
        wxWindowList::Node *node = GetChildren().GetFirst();
        while (node)
        {
            wxWindow *win = node->GetData();
            if ( !win->IsTopLevel() )
                win->SetConstraintSizes();
            node = node->GetNext();
        }
    }
}

// Only set the size/position of the constraint (if any)
void wxWindow::SetSizeConstraint(int x, int y, int w, int h)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        if ( x != -1 )
        {
            constr->left.SetValue(x);
            constr->left.SetDone(TRUE);
        }
        if ( y != -1 )
        {
            constr->top.SetValue(y);
            constr->top.SetDone(TRUE);
        }
        if ( w != -1 )
        {
            constr->width.SetValue(w);
            constr->width.SetDone(TRUE);
        }
        if ( h != -1 )
        {
            constr->height.SetValue(h);
            constr->height.SetDone(TRUE);
        }
    }
}

void wxWindow::MoveConstraint(int x, int y)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        if ( x != -1 )
        {
            constr->left.SetValue(x);
            constr->left.SetDone(TRUE);
        }
        if ( y != -1 )
        {
            constr->top.SetValue(y);
            constr->top.SetDone(TRUE);
        }
    }
}

void wxWindow::GetSizeConstraint(int *w, int *h) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *w = constr->width.GetValue();
        *h = constr->height.GetValue();
    }
    else
        GetSize(w, h);
}

void wxWindow::GetClientSizeConstraint(int *w, int *h) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *w = constr->width.GetValue();
        *h = constr->height.GetValue();
    }
    else
        GetClientSize(w, h);
}

void wxWindow::GetPositionConstraint(int *x, int *y) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *x = constr->left.GetValue();
        *y = constr->top.GetValue();
    }
    else
        GetPosition(x, y);
}

#endif // wxUSE_CONSTRAINTS

bool wxWindow::Close(bool force)
{
  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
#if WXWIN_COMPATIBILITY
  event.SetForce(force);
#endif
  event.SetCanVeto(!force);

  return GetEventHandler()->ProcessEvent(event);
}

wxObject* wxWindow::GetChild(int number) const
{
  // Return a pointer to the Nth object in the window
  wxNode *node = GetChildren().First();
  int n = number;
  while (node && n--)
    node = node->Next() ;
  if (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    return(obj) ;
  }
  else
    return NULL ;
}

void wxWindow::OnDefaultAction(wxControl *initiatingItem)
{
    // Obsolete function
}

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

// Fits the panel around the items
void wxWindow::Fit()
{
	int maxX = 0;
	int maxY = 0;
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *win = (wxWindow *)node->Data();
		int wx, wy, ww, wh;
		win->GetPosition(&wx, &wy);
		win->GetSize(&ww, &wh);
		if ( wx + ww > maxX )
			maxX = wx + ww;
		if ( wy + wh > maxY )
			maxY = wy + wh;

		node = node->Next();
	}
	SetClientSize(maxX + 5, maxY + 5);
}

void wxWindow::SetValidator(const wxValidator& validator)
{
	if ( m_windowValidator )
		delete m_windowValidator;
	m_windowValidator = validator.Clone();

	if ( m_windowValidator )
		m_windowValidator->SetWindow(this) ;
}

void wxWindow::SetAcceleratorTable(const wxAcceleratorTable& accel)
{
    m_acceleratorTable = accel;
}

// Find a window by id or name
wxWindow *wxWindow::FindWindow(long id)
{
	if ( GetId() == id)
		return this;

	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(id);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
}

wxWindow *wxWindow::FindWindow(const wxString& name)
{
	if ( GetName() == name)
		return this;

	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(name);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
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

bool wxWindow::AcceptsFocus() const
{
  return IsShown() && IsEnabled() && MacCanFocus() ;
}

// Update region access
wxRegion wxWindow::GetUpdateRegion() const
{
    return m_updateRegion;
}

bool wxWindow::IsExposed(int x, int y, int w, int h) const
{
    return (m_updateRegion.Contains(x, y, w, h) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxPoint& pt) const
{
    return (m_updateRegion.Contains(pt) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxRect& rect) const
{
    return (m_updateRegion.Contains(rect) != wxOutRegion);
}

/*
 * Allocates control IDs
 */

int wxWindow::NewControlId()
{
    static int s_controlId = 0;
    s_controlId ++;
    return s_controlId;
}

void wxWindow::DoSetClientSize(int width, int height)
{
	if ( width != -1 || height != -1 )
	{
		
		if ( width != -1 && m_vScrollBar )
			width += MAC_SCROLLBAR_SIZE ;
		if ( height != -1 && m_vScrollBar )
			height += MAC_SCROLLBAR_SIZE ;

		DoSetSize( -1 , -1 , width , height ) ;
	}
}

// ------------------------
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
	
	if ( m_children )
	{
		for (wxNode *node = m_children->First(); node; node = node->Next())
		{
			wxWindow *child = (wxWindow*)node->Data();
			if ( child->GetMacRootWindow() == window )
			{
				if (child->MacGetWindowFromPointSub(newPoint , outWin ))
					return TRUE;
			}
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
			win->ScreenToClient( point ) ;
			return win->MacGetWindowFromPointSub( point , outWin ) ;
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

	if ( m_children )
	{
		for (wxNode *node = m_children->First(); node; node = node->Next())
		{
			wxWindow *child = (wxWindow*)node->Data();
			if ( child->GetMacRootWindow() == window && child->IsReallyShown() && child->IsReallyEnabled() )
			{
				if (child->MacDispatchMouseEvent(event))
					return TRUE;
			}
		}
	}

	event.m_x = x ;
	event.m_y = y ;
	
	if ( wxBusyCursorCount == 0 )
	{
		m_windowCursor.MacInstall() ;
	}
	GetEventHandler()->ProcessEvent( event ) ;
	return TRUE;
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
				m_windowCursor.MacInstall() ;
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
		wxMacDrawingClientHelper focus( this ) ;
		if ( focus.Ok() )
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
								ApplyThemeBackground			(kThemeBackgroundTabPane, &(**updatergn).rgnBBox , kThemeStateActive,8,true);
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
			SetClip( updatergn ) ;
			EraseRgn( updatergn ) ;	
		}
	}

	
	m_updateRegion = updatergn ;
	wxPaintEvent event;
	event.m_timeStamp = time ;
	event.SetEventObject(this);
	
	GetEventHandler()->ProcessEvent(event);
	
	RgnHandle childupdate = NewRgn() ;

	for (wxNode *node = m_children->First(); node; node = node->Next())
	{
		wxWindow *child = (wxWindow*)node->Data();
		int width ;
		int height ;
		
		child->GetClientSize( &width , &height ) ;
		
		SetRectRgn( childupdate , child->m_x , child->m_y , child->m_x +width ,  child->m_y + height ) ;
		SectRgn( childupdate , m_updateRegion.GetWXHRGN() , childupdate ) ;
		OffsetRgn( childupdate , -child->m_x , -child->m_y ) ;
		if ( child->GetMacRootWindow() == window && child->IsReallyShown() )
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
		BeginUpdate( window ) ;
		if ( win )
		{
		#if ! TARGET_CARBON
			if ( !EmptyRgn( window->visRgn ) )
		#endif
			{
				win->MacRedraw( window->visRgn , wxTheApp->sm_lastMessageTime ) ;
/*
				{
					wxMacDrawingHelper help( win ) ;
					SetOrigin( 0 , 0 ) ;
					UMASetThemeWindowBackground( win->m_macWindowData->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
					UMAUpdateControls( window , window->visRgn ) ;
					UMASetThemeWindowBackground( win->m_macWindowData->m_macWindow , win->m_macWindowData->m_macWindowBackgroundTheme , false ) ;
				}
*/
			}
		}
		EndUpdate( window ) ;
	}
}

void wxWindow::MacUpdate( EventRecord *ev )
{
	WindowRef window = (WindowRef) ev->message ;
	wxWindow * win = wxFindWinFromMacWindow( window ) ;
	
	BeginUpdate( window ) ;
	if ( win )
	{
		// if windowshade gives incompatibility , take the follwing out
		#if ! TARGET_CARBON
		if ( !EmptyRgn( window->visRgn ) )
		#endif
		{
			MacRedraw( window->visRgn , ev->when ) ;
			/*
			{
				wxMacDrawingHelper help( this ) ;
				SetOrigin( 0 , 0 ) ;
				UMASetThemeWindowBackground( m_macWindowData->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
				UMAUpdateControls( window , window->visRgn ) ;
				UMASetThemeWindowBackground( m_macWindowData->m_macWindow , m_macWindowData->m_macWindowBackgroundTheme , false ) ;
			}
			*/
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
	
	if ( style & wxVSCROLL )
	{
		m_vScrollBar = new wxScrollBar(this, wxWINDOW_VSCROLL, wxPoint(m_width-MAC_SCROLLBAR_SIZE, 0), 
			wxSize(MAC_SCROLLBAR_SIZE, m_height - adjust), wxVERTICAL);
	}
	if ( style  & wxHSCROLL )
	{
		m_hScrollBar = new wxScrollBar(this, wxWINDOW_HSCROLL, wxPoint(0 , m_height-MAC_SCROLLBAR_SIZE ), 
			wxSize( m_width - adjust, MAC_SCROLLBAR_SIZE), wxHORIZONTAL);
	}
	// because the create does not take into account the client area origin
	MacRepositionScrollBars() ; // we might have a real position shift
}

void wxWindow::MacRepositionScrollBars()
{
	bool hasBoth = ( m_hScrollBar && m_hScrollBar->IsShown()) && ( m_vScrollBar && m_vScrollBar->IsShown()) ;
	int adjust = hasBoth ? MAC_SCROLLBAR_SIZE - 1 : 0 ;
	
	if ( m_vScrollBar )
	{
		m_vScrollBar->SetSize( m_width-MAC_SCROLLBAR_SIZE, 0, MAC_SCROLLBAR_SIZE, m_height - adjust , wxSIZE_USE_EXISTING);
	}
	if ( m_hScrollBar )
	{
		m_hScrollBar->SetSize( 0 , m_height-MAC_SCROLLBAR_SIZE ,m_width - adjust, MAC_SCROLLBAR_SIZE, wxSIZE_USE_EXISTING);
	}
}

void wxWindow::MacKeyDown( EventRecord *ev ) 
{
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

bool wxWindow::IsReallyShown() const
{
	if ( m_macWindowData )
		return m_macShown ;
	else
		return m_macShown && GetParent()->IsReallyShown() ;
}

bool wxWindow::IsReallyEnabled() const
{
	if ( m_macWindowData )
		return m_macEnabled ;
	else
		return m_macEnabled && GetParent()->IsReallyEnabled() ;
}

void wxWindow::MacSuperEnabled( bool enabled ) 
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->m_macEnabled )
			child->MacSuperEnabled( enabled ) ;
		node = node->Next();
	}
}
void wxWindow::MacSuperShown( bool show ) 
{
	wxNode *node = GetChildren().First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->m_macShown )
			child->MacSuperShown( show ) ;
		node = node->Next();
	}
}
	
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
				
	::SetOrigin(-localOrigin.h, -localOrigin.v);
	::ClipRect(&clipRect);

	::PenNormal() ;
	::RGBBackColor(& win->GetBackgroundColour().GetPixel() ) ;
	::RGBForeColor(& win->GetForegroundColour().GetPixel() ) ;
	::BackPat( &qd.white ) ;
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

void wxWindow::MacGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindow** rootwin ) 
{
	int width , height ;
	GetClientSize( &width , &height ) ;
	
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
		
		GetParent()->MacGetPortClientParams( localOrigin , clipRect , window, rootwin) ;

		localOrigin->h += m_x;
		localOrigin->v += m_y;
		OffsetRect(clipRect, -m_x, -m_y);
	
		Rect myClip;
		myClip.left = 0;
		myClip.top = 0;
		myClip.right = width;
		myClip.bottom = height;
		SectRect(clipRect, &myClip, clipRect);
	}
}

wxMacFocusHelper::wxMacFocusHelper( wxWindow * theWindow ) 
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
		theWindow->MacSetPortFocusParams( localOrigin, clipRect, window , rootwin ) ; 
		m_ok = true ;
	}
}
	
wxMacFocusHelper::~wxMacFocusHelper() 
{
	if ( m_ok )
	{
		SetOrigin( 0 , 0 ) ;
	}
	if ( m_formerPort != m_currentPort )
		SetPort( m_formerPort ) ;
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
		SetPenState( &m_savedPenState ) ;
		SetOrigin( 0 , 0 ) ;
		ClipRect( &m_currentPort->portRect ) ;
	}
		
	if ( m_formerPort != m_currentPort )
		SetPort( m_formerPort ) ;
}

wxMacFocusClientHelper::wxMacFocusClientHelper( wxWindow * theWindow ) 
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
		theWindow->MacSetPortFocusParams( localOrigin, clipRect, window , rootwin ) ; 
		m_ok = true ;
	}
}
	
wxMacFocusClientHelper::~wxMacFocusClientHelper() 
{
	if ( m_ok )
	{
		SetOrigin( 0 , 0 ) ;
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
		SetPenState( &m_savedPenState ) ;
		SetOrigin( 0 , 0 ) ;
		ClipRect( &m_currentPort->portRect ) ;
	}
		
	if ( m_formerPort != m_currentPort )
		SetPort( m_formerPort ) ;
}

// ----------------------------------------------------------------------------
// list classes implementation
// ----------------------------------------------------------------------------

void wxWindowListNode::DeleteData()
{
    delete (wxWindow *)GetData();
}

