/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

#include "wx/control.h"
#include "wx/panel.h"
#include "wx/app.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/radiobox.h"
#include "wx/spinbutt.h"
#include "wx/scrolbar.h"
#include "wx/button.h"
#include "wx/dialog.h"
#include "wx/statbox.h"
#include "wx/sizer.h"
#include "wx/stattext.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
	EVT_MOUSE_EVENTS( wxControl::OnMouseEvent ) 
	EVT_CHAR( wxControl::OnKeyDown ) 
	EVT_PAINT( wxControl::OnPaint ) 
END_EVENT_TABLE()
#endif

#include <wx/mac/uma.h>

// Item members

ControlActionUPP wxMacLiveScrollbarActionUPP = NULL ;

pascal void wxMacLiveScrollbarActionProc( ControlHandle control , ControlPartCode partCode ) ;
pascal void wxMacLiveScrollbarActionProc( ControlHandle control , ControlPartCode partCode )
{
	if ( partCode != 0)
	{
		wxControl*	wx = (wxControl*) GetControlReference( control ) ;
		if ( wx )
		{
			wx->MacHandleControlClick( control , partCode ) ;
		}
	}
}

wxControl::wxControl()
{
	m_macControl = NULL ;
	m_macHorizontalBorder = 0 ; // additional pixels around the real control
	m_macVerticalBorder = 0 ;
    m_backgroundColour = *wxWHITE;
    m_foregroundColour = *wxBLACK;
#if WXWIN_COMPATIBILITY
  m_callback = 0;
#endif // WXWIN_COMPATIBILITY

	if ( wxMacLiveScrollbarActionUPP == NULL )
	{
#ifdef __UNIX__
        wxMacLiveScrollbarActionUPP = NewControlActionUPP( wxMacLiveScrollbarActionProc );
#else
		wxMacLiveScrollbarActionUPP = NewControlActionProc( wxMacLiveScrollbarActionProc ) ;
#endif
	}
}

bool wxControl::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size, long style,
                       const wxValidator& validator,
                       const wxString& name)
{
	m_macControl = NULL ;
	m_macHorizontalBorder = 0 ; // additional pixels around the real control
	m_macVerticalBorder = 0 ;
    bool rval = wxWindow::Create(parent, id, pos, size, style, name);
    if (rval) {
#if wxUSE_VALIDATORS
        SetValidator(validator);
#endif
    }
    return rval;
}

wxControl::~wxControl()
{
    m_isBeingDeleted = TRUE;
    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if ( panel )
    {
        if (panel->GetDefaultItem() == (wxButton*) this)
            panel->SetDefaultItem(NULL);
    }
    if ( m_macControl )
    {
    	UMADisposeControl( m_macControl ) ;
    	m_macControl = NULL ;
    }
}

void wxControl::SetLabel(const wxString& title)
{
	m_label = title ;

	if ( m_macControl )
	{
		Str255 maclabel ;
		wxString label ;
	
		if( wxApp::s_macDefaultEncodingIsPC )
			label = wxMacMakeMacStringFromPC( title ) ;
		else
			label = title ;
		
#if TARGET_CARBON
		c2pstrcpy( (StringPtr) maclabel , label ) ;
#else
		strcpy( (char *) maclabel , label ) ;
		c2pstr( (char *) maclabel ) ;
#endif
		::SetControlTitle( m_macControl , maclabel ) ;
	}
}

wxSize wxControl::DoGetBestSize() const
{
    return wxSize(20, 20);
}

bool wxControl::ProcessCommand (wxCommandEvent & event)
{
  // Tries:
  // 1) A callback function (to become obsolete)
  // 2) OnCommand, starting at this window and working up parent hierarchy
  // 3) OnCommand then calls ProcessEvent to search the event tables.
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY
    {
      return GetEventHandler()->ProcessEvent(event);
    }
}

// ------------------------
wxList *wxWinMacControlList = NULL;
wxControl *wxFindControlFromMacControl(ControlHandle inControl )
{
    wxNode *node = wxWinMacControlList->Find((long)inControl);
    if (!node)
        return NULL;
    return (wxControl *)node->Data();
}

void wxAssociateControlWithMacControl(ControlHandle inControl, wxControl *control)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inControl != (ControlHandle) NULL, "attempt to add a NULL WindowRef to window list" );

    if ( !wxWinMacControlList->Find((long)inControl) )
        wxWinMacControlList->Append((long)inControl, control);
}

void wxRemoveMacControlAssociation(wxControl *control)
{
    wxWinMacControlList->DeleteObject(control);
}

void wxControl::MacPreControlCreate( wxWindow *parent, wxWindowID id, wxString label , 
			 const wxPoint& pos,
			 const wxSize& size, long style,
			 const wxValidator& validator,
			 const wxString& name , Rect *outBounds , StringPtr maclabel ) 
{
	m_label = label ;
  SetName(name);
  if ( &validator )
  	SetValidator(validator);

  m_windowStyle = style;
  parent->AddChild((wxButton *)this);

  m_backgroundColour = parent->GetBackgroundColour() ;
  m_foregroundColour = parent->GetForegroundColour() ;

  if (id == -1)
      m_windowId = NewControlId();
  else
      m_windowId = id;

	m_width = size.x ;
	m_height = size.y ;
	int x = pos.x ;
	int y = pos.y ;
	AdjustForParentClientOrigin(x, y, wxSIZE_USE_EXISTING);
	m_x = x ;
	m_y = y ;
		

	parent->MacClientToRootWindow( &x , &y ) ;
	outBounds->top = y + m_macVerticalBorder ;
	outBounds->left = x + m_macHorizontalBorder ;
	outBounds->bottom = outBounds->top + m_height - 2 * m_macVerticalBorder;
	outBounds->right = outBounds->left + m_width - 2 * m_macHorizontalBorder ;

	char c_text[255];
	strcpy( c_text , label ) ;
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		wxMacConvertFromPCForControls( c_text ) ;
	}

#if TARGET_CARBON
	c2pstrcpy( (StringPtr) maclabel , c_text ) ;
#else
	strcpy( (char *) maclabel , c_text ) ;
	c2pstr( (char *) maclabel ) ;
#endif
}

void wxControl::MacPostControlCreate()
{
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
	
	if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
	{
		// no font
	}
	else if ( IsKindOf( CLASSINFO( wxStaticBox ) ) || IsKindOf( CLASSINFO( wxRadioBox ) ) || IsKindOf( CLASSINFO( wxButton ) ) )
	{
		ControlFontStyleRec		controlstyle ;
		controlstyle.flags = kControlUseFontMask ;
		controlstyle.font = kControlFontSmallBoldSystemFont ;
		
		::UMASetControlFontStyle( m_macControl , &controlstyle ) ;
	}
	else
	{
		ControlFontStyleRec		controlstyle ;
		controlstyle.flags = kControlUseFontMask ;
		controlstyle.font = kControlFontSmallSystemFont ;
		
		::UMASetControlFontStyle( m_macControl , &controlstyle ) ;
	}
	ControlHandle container = GetParent()->MacGetContainerForEmbedding() ;
	wxASSERT_MSG( container != NULL , "No valid mac container control" ) ;
	::UMAEmbedControl( m_macControl , container ) ;
	m_macControlIsShown  = true ;
	MacAdjustControlRect() ;
	wxAssociateControlWithMacControl( m_macControl , this ) ;
}

void wxControl::MacAdjustControlRect() 
{
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
	if ( m_width == -1 || m_height == -1 )
	{
		Rect 	bestsize = { 0 , 0 , 0 , 0 } ;
		short	baselineoffset ;
		
		UMAGetBestControlRect( m_macControl , &bestsize , &baselineoffset ) ;

		if ( EmptyRect( &bestsize ) )
		{
			baselineoffset = 0;
			bestsize.left = bestsize.top = 0 ;
			bestsize.right = 16 ;
			bestsize.bottom = 16 ;
			if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
			{
				bestsize.bottom = 16 ;
			}
			else if ( IsKindOf( CLASSINFO( wxSpinButton ) ) )
			{
				bestsize.bottom = 24 ; 
			}
		}

		if ( m_width == -1 )
		{
			if ( IsKindOf( CLASSINFO( wxButton ) ) )
			{
				m_width = m_label.Length() * 8 + 12 ;
			}
			else if ( IsKindOf( CLASSINFO( wxStaticText ) ) )
			{
				m_width = m_label.Length() * 8 ;
			}
			else
				m_width = bestsize.right - bestsize.left ;
			
			m_width += 2 * m_macHorizontalBorder ;
		}
		if ( m_height == -1 )
		{
			m_height = bestsize.bottom - bestsize.top ;
			if ( m_height < 10 )
				m_height = 13 ;

			m_height += 2 * m_macVerticalBorder;
		}
		
		wxMacDrawingHelper helper ( wxFindWinFromMacWindow( GetMacRootWindow() ) ) ;
		if ( helper.Ok() )
		{
	    	UMASizeControl( m_macControl , m_width - 2 * m_macHorizontalBorder, m_height -  2 * m_macVerticalBorder ) ;
  		}
	}
}
ControlHandle wxControl::MacGetContainerForEmbedding() 
{
	if ( m_macControl )
		return m_macControl ;

	return wxWindow::MacGetContainerForEmbedding() ;
}

void wxControl::MacSuperChangedPosition() 
{
	if ( m_macControl )
	{
		Rect contrlRect ;		
		GetControlBounds( m_macControl , &contrlRect ) ; 
		int former_mac_x = contrlRect.left ;
		int former_mac_y = contrlRect.top ;
		int mac_x = m_x ;
		int mac_y = m_y ;
		GetParent()->MacClientToRootWindow( & mac_x , & mac_y ) ;
		
		WindowRef rootwindow = GetMacRootWindow() ;
		wxWindow* wxrootwindow = wxFindWinFromMacWindow( rootwindow ) ;
		UMASetThemeWindowBackground( rootwindow , kThemeBrushDialogBackgroundActive , false ) ;
		wxMacDrawingHelper focus( wxrootwindow ) ;
	
		if ( mac_x != former_mac_x || mac_y != former_mac_y )
		{
			{
				Rect inval = { former_mac_y , former_mac_x , former_mac_y + m_height , former_mac_x + m_width } ;
				InvalWindowRect( rootwindow , &inval ) ;
			}
	  		UMAMoveControl( m_macControl , mac_x + m_macHorizontalBorder , mac_y + m_macVerticalBorder ) ;
			{
				Rect inval = { mac_y , mac_x , mac_y + m_height , mac_x + m_width } ;
				InvalWindowRect( rootwindow , &inval ) ;
			}
		}
		if ( wxrootwindow->IsKindOf( CLASSINFO( wxDialog ) ) )
		{
		}
		else
		{
			UMASetThemeWindowBackground( rootwindow , kThemeBrushDocumentWindowBackground , false ) ;
		}
	}

	wxWindow::MacSuperChangedPosition() ;
}

void wxControl::MacSuperEnabled( bool enabled ) 
{
/*
	if ( m_macControl )
	{
		if ( UMAHasAppearance() )
		{
			if ( !enabled )
			{
				::DeactivateControl( m_macControl ) ; 
			}
			else
			{
				if ( m_macEnabled )
					::ActivateControl( m_macControl ) ;
			}
		}
		else
		{
			if ( !enabled )
			{
				::HiliteControl( m_macControl , 255 ) ;
			}
			else
			{
				if ( m_macEnabled )
					::HiliteControl( m_macControl , 0 ) ;
			}
		}
	}
	wxWindow::MacSuperEnabled( enabled ) ;
*/
}

void  wxControl::MacSuperShown( bool show ) 
{
	if ( m_macControl )
	{
		if ( !show )
		{
			if ( m_macControlIsShown )
			{
				::UMAHideControl( m_macControl ) ;
				m_macControlIsShown = false ;
			}
		}
		else
		{
			if ( MacIsReallyShown() && !m_macControlIsShown )
			{
				::UMAShowControl( m_macControl ) ;
				m_macControlIsShown = true ;
			}
		}
	}
		
	wxWindow::MacSuperShown( show ) ;
}

void  wxControl::DoSetSize(int x, int y,
            int width, int height,
            int sizeFlags )
{
	if ( m_macControl == NULL )
	{
		wxWindow::DoSetSize( x , y ,width , height ,sizeFlags ) ;
		return ;
	}

	WindowRef rootwindow = GetMacRootWindow() ;
	wxWindow* wxrootwindow = wxFindWinFromMacWindow( rootwindow ) ;
	UMASetThemeWindowBackground( rootwindow , kThemeBrushDialogBackgroundActive , false ) ;
		
	int former_x = m_x ;
	int former_y = m_y ;
	int former_w = m_width ;
	int former_h = m_height ;
	
	Rect contrlRect ;		
	GetControlBounds( m_macControl , &contrlRect ) ; 
	int former_mac_x = contrlRect.left ;
	int former_mac_y = contrlRect.top ;
	
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
		return ;
		
	AdjustForParentClientOrigin(actualX, actualY, sizeFlags);
	WindowRef macrootwindow = GetMacRootWindow() ;
	wxMacDrawingHelper focus( wxFindWinFromMacWindow( macrootwindow ) ) ;

	int mac_x = actualX ;
	int mac_y = actualY ;
	GetParent()->MacClientToRootWindow( & mac_x , & mac_y ) ;
	
	if ( mac_x != former_mac_x || mac_y != former_mac_y )
	{
		{
			Rect inval = { former_mac_y , former_mac_x , former_mac_y + m_height , former_mac_x + m_width } ;
			InvalWindowRect( macrootwindow, &inval ) ;
		}
  		UMAMoveControl( m_macControl , mac_x + m_macHorizontalBorder , mac_y  + m_macVerticalBorder ) ;
		{
			Rect inval = { mac_y , mac_x , mac_y + m_height , mac_x + m_width } ;
			InvalWindowRect(macrootwindow, &inval ) ;
		}
	}

	if ( actualX != former_x || actualY != former_y )
	{
		m_x = actualX ;
		m_y = actualY ;

		MacRepositionScrollBars() ;
		// To consider -> should the parameters be the effective or the virtual coordinates (AdjustForParent..)
    wxMoveEvent event(wxPoint(m_x, m_y), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
	}		
	if ( actualWidth != former_w || actualHeight != former_h )
	{
		{
			Rect inval = { mac_y , mac_x , mac_y + former_h , mac_x + former_w } ;
			InvalWindowRect( macrootwindow, &inval ) ;
		}
		m_width = actualWidth ;
		m_height = actualHeight ;

   		UMASizeControl( m_macControl ,  m_width - 2 * m_macHorizontalBorder, m_height -  2 * m_macVerticalBorder ) ;
		{
			Rect inval = { mac_y , mac_x , mac_y + m_height , mac_x + m_width } ;
			InvalWindowRect( macrootwindow , &inval ) ;
		}

		MacRepositionScrollBars() ;
    wxSizeEvent event(wxSize(m_width, m_height), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
	}
	if ( wxrootwindow->IsKindOf( CLASSINFO( wxDialog ) ) )
	{
	}
	else
	{
		UMASetThemeWindowBackground( rootwindow , kThemeBrushDocumentWindowBackground , false ) ;
	}
}

bool  wxControl::Show(bool show) 
{
	if ( !wxWindow::Show( show ) )
		return FALSE ;
		
	if ( m_macControl )
	{
		if ( !show )
		{
			if ( m_macControlIsShown )
			{
				::UMAHideControl( m_macControl ) ;
				m_macControlIsShown = false ;
			}
		}
		else
		{
			if ( MacIsReallyShown() && !m_macControlIsShown )
			{
				::UMAShowControl( m_macControl ) ;
				m_macControlIsShown = true ;
			}
		}
	}
	return TRUE ;
}

bool  wxControl::Enable(bool enable) 
{
    if ( !wxWindow::Enable(enable) )
        return FALSE;

	if ( m_macControl )
	{
		
		if ( UMAHasAppearance() )
		{
			if ( enable )
				::ActivateControl( m_macControl ) ;
			else
				::DeactivateControl( m_macControl ) ;
		}
		else
		{
			if ( enable )
				::HiliteControl( m_macControl , 0 ) ;
			else
				::HiliteControl( m_macControl , 255 ) ;
		}
	}
	return TRUE ;
}

void wxControl::Refresh(bool eraseBack, const wxRect *rect)
{
  if ( m_macControl )
  {
  	wxWindow::Refresh( eraseBack , rect ) ;
	}
  else
  {
  	wxWindow::Refresh( eraseBack , rect ) ;
  }
}

void wxControl::MacRedrawControl()
{
	if ( m_macControl )
	{
		WindowRef window = GetMacRootWindow() ;
		if ( window )
		{
			wxWindow* win = wxFindWinFromMacWindow( window ) ;
			if ( win )
			{
				wxMacDrawingHelper help( win ) ;
				// the mac control manager always assumes to have the origin at 0,0
				SetOrigin( 0 , 0 ) ;
				
				bool			hasTabBehind = false ;
				wxWindow* parent = GetParent() ;
				while ( parent )
				{
					if( parent->MacGetWindowData() )
					{
						UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
						break ;
					}
					
					if( parent->IsKindOf( CLASSINFO( wxNotebook ) ) ||  parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
					{
						if ( ((wxControl*)parent)->m_macControl )
							SetUpControlBackground( ((wxControl*)parent)->m_macControl , -1 , true ) ;
						break ;
					}
					
					parent = parent->GetParent() ;
				} 
				
				UMADrawControl( m_macControl ) ;
				UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
				wxDC::MacInvalidateSetup() ;
			}
		}
	}
}

void wxControl::OnPaint(wxPaintEvent& event)
{
	if ( m_macControl )
	{
		WindowRef window = GetMacRootWindow() ;
		if ( window )
		{
			wxWindow* win = wxFindWinFromMacWindow( window ) ;
			if ( win )
			{
				wxMacDrawingHelper help( win ) ;
				// the mac control manager always assumes to have the origin at 0,0
				SetOrigin( 0 , 0 ) ;
				
				bool			hasTabBehind = false ;
				wxWindow* parent = GetParent() ;
				while ( parent )
				{
					if( parent->MacGetWindowData() )
					{
						UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
						break ;
					}
					
					if( parent->IsKindOf( CLASSINFO( wxNotebook ) ) ||  parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
					{
						if ( ((wxControl*)parent)->m_macControl )
							SetUpControlBackground( ((wxControl*)parent)->m_macControl , -1 , true ) ;
						break ;
					}
					
					parent = parent->GetParent() ;
				} 
				
				UMADrawControl( m_macControl ) ;
				UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
				wxDC::MacInvalidateSetup() ;
			}
		}
	}
	else
	{
		// wxWindow::OnPaint( event ) ;
	}
}
void wxControl::OnEraseBackground(wxEraseEvent& event)
{
    // In general, you don't want to erase the background of a control,
    // or you'll get a flicker.
    // TODO: move this 'null' function into each control that
    // might flicker.
}


void  wxControl::OnKeyDown( wxKeyEvent &event ) 
{
	if ( m_macControl == NULL )
		return ;
	
	EventRecord *ev = wxTheApp->MacGetCurrentEvent() ;
	short keycode ;
	short keychar ;
	keychar = short(ev->message & charCodeMask);
	keycode = short(ev->message & keyCodeMask) >> 8 ;

	UMAHandleControlKey( m_macControl , keycode , keychar , ev->modifiers ) ;
}

void  wxControl::OnMouseEvent( wxMouseEvent &event ) 
{
	if ( m_macControl == NULL )
	{
		event.Skip() ;
		return ;
	}
		
	if (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK )
	{
			
		int x = event.m_x ;
		int y = event.m_y ;
		
		MacClientToRootWindow( &x , &y ) ;
			
		ControlHandle	control ;
		Point		localwhere ;
		GrafPtr		port ;
		SInt16		controlpart ;
		WindowRef	window = GetMacRootWindow() ;
		
		localwhere.h = x ;
		localwhere.v = y ;
	
		short modifiers = 0;
		
		if ( !event.m_leftDown && !event.m_rightDown )
			modifiers  |= btnState ;
	
		if ( event.m_shiftDown )
			modifiers |= shiftKey ;
			
		if ( event.m_controlDown )
			modifiers |= controlKey ;
	
		if ( event.m_altDown )
			modifiers |= optionKey ;
	
		if ( event.m_metaDown )
			modifiers |= cmdKey ;
	
		controlpart = FindControl( localwhere , window , &control ) ;
		{
			if ( AcceptsFocus() && FindFocus() != this )
			{
				SetFocus() ;
			}
			if ( control && UMAIsControlActive( control ) )
			{
				{
					if ( controlpart == kControlIndicatorPart && !UMAHasAppearance() )
						controlpart = UMAHandleControlClick( control , localwhere , modifiers , (ControlActionUPP) NULL ) ;
					else
						controlpart = UMAHandleControlClick( control , localwhere , modifiers , (ControlActionUPP) -1 ) ;
					wxTheApp->s_lastMouseDown = 0 ;
					if ( controlpart && ! ( ( UMAHasAppearance() || (controlpart != kControlIndicatorPart) ) 
						&& (IsKindOf( CLASSINFO( wxScrollBar ) ) ) ) ) // otherwise we will get the event twice
					{
						MacHandleControlClick( control , controlpart ) ;
					}
				}
			}
		}
	}
}

bool wxControl::MacCanFocus() const
{
		{ if ( m_macControl == NULL ) 
				return true ; 
			else
			return false ; 
		}
}

void wxControl::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
}

