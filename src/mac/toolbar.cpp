/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHORy
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "toolbar.h"
#endif

#include "wx/wx.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
	EVT_MOUSE_EVENTS( wxToolBar::OnMouse ) 
	EVT_PAINT( wxToolBar::OnPaint ) 
END_EVENT_TABLE()
#endif

#include <wx/mac/uma.h>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
        m_nSepCount = 0;
        m_index = -1 ;
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        m_nSepCount = 1;
        m_index = -1 ;
    }

    // set/get the number of separators which we use to cover the space used by
    // a control in the toolbar
    void SetSeparatorsCount(size_t count) { m_nSepCount = count; }
    size_t GetSeparatorsCount() const { return m_nSepCount; }

    int		m_index ;
private:
    size_t m_nSepCount;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

const short defwidth = 24 ;
const short defheight = 22 ;

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

void wxToolBar::Init()
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_defaultWidth = defwidth;
  m_defaultHeight = defheight;
  // TODO
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{

  m_defaultWidth = defwidth;
  m_defaultHeight = defheight;
  
  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
#if 1
	{
	  SetName(name);

	  m_windowStyle = style;
	  parent->AddChild(this);

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
	}
#else
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , wxPoint( x , y ) , wxSize( width , height ) ,style, wxDefaultValidator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
	  	kControlPlacardProc , (long) this ) ;
	MacPostControlCreate() ;
#endif
  return TRUE;
}

wxToolBar::~wxToolBar()
{
    // we must refresh the frame size when the toolbar is deleted but the frame
    // is not - otherwise toolbar leaves a hole in the place it used to occupy
}

PicHandle MakePict(GWorldPtr wp, GWorldPtr mask ) ;
PicHandle MakePict(GWorldPtr wp, GWorldPtr mask ) 
{
	CGrafPtr		origPort ;
	GDHandle		origDev ;
	
	PicHandle		pict;				// this is the Picture we give back

	RGBColor		gray = { 0xCCCC ,0xCCCC , 0xCCCC } ;
	RGBColor		white = { 0xffff ,0xffff , 0xffff } ;
	RGBColor		black = { 0x0000 ,0x0000 , 0x0000 } ;
	
	unsigned char *maskimage = NULL ;
	Rect portRect ;
	GetPortBounds( wp , &portRect ) ;
	int width = portRect.right - portRect.left ;
	int height = portRect.bottom - portRect.top ;
	
	LockPixels( GetGWorldPixMap( wp ) ) ;
	GetGWorld( &origPort , &origDev ) ;

	if ( mask )
	{
		
		maskimage = (unsigned char*) malloc( width * height ) ;
		SetGWorld( mask , NULL ) ;
		LockPixels( GetGWorldPixMap( mask ) ) ;
		for ( int y = 0 ; y < height ; ++y )
		{
			for( int x = 0 ; x < width ; ++x )
			{
				RGBColor col ;
				
				GetCPixel( x + portRect.left , y + portRect.top , &col ) ;
				maskimage[y*width + x] = ( col.red == 0 ) ; // for monochrome masks
			}
		}
		UnlockPixels( GetGWorldPixMap( mask ) ) ;
	}
	
	SetGWorld( wp , NULL ) ;
	
	pict = OpenPicture(&portRect);	// open a picture, this disables drawing
	if(!pict)	
		return NULL;

	RGBBackColor( &gray ) ;
	RGBForeColor( &black ) ;
	EraseRect(&portRect) ;
	RGBBackColor( &white ) ;

	if ( maskimage )
	{
		for ( int y = 0 ; y < height ; ++y )
		{
			for( int x = 0 ; x < width ; ++x )
			{
				if ( maskimage[y*width + x] )
				{
					RGBColor col ;
				
					GetCPixel( x + portRect.left , y + portRect.top , &col ) ;
					SetCPixel( x + portRect.left , y + portRect.top , &col ) ;
				}
			}
		}
		free( maskimage ) ;
		maskimage = NULL ;
	}
	else
	{
		CopyBits(GetPortBitMapForCopyBits(wp),			// src PixMap	- we copy image over itself -
				GetPortBitMapForCopyBits(wp),		// dst PixMap	- no drawing occurs -
				&portRect,			// srcRect		- it will be recorded and compressed -
				&portRect,			// dstRect		- into the picture that is open -
				srcCopy,NULL);			// copyMode and no clip region

	}
	ClosePicture();						// We are done recording the picture
	UnlockPixels( GetGWorldPixMap( wp ) ) ;
	SetGWorld( origPort , origDev ) ;
	return pict;						// return our groovy pict handle
}

const short kwxMacToolBarTopMargin = 2 ;
const short kwxMacToolBarLeftMargin = 2 ;

bool wxToolBar::Realize()
{
  if (m_tools.Number() == 0)
      return FALSE;

	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *win ;
	
	GetParent()->MacGetPortParams( &localOrigin , &clipRect , &window , &win ) ;

	Rect toolbarrect = { m_y + localOrigin.v , m_x  + localOrigin.h , 
		m_y + m_height + localOrigin.v  , m_x + m_width + localOrigin.h} ;
	ControlFontStyleRec		controlstyle ;

	controlstyle.flags = kControlUseFontMask ;
	controlstyle.font = kControlFontSmallSystemFont ;
	
	wxNode *node = m_tools.First();
	int noButtons = 0;
	int x = 0 ;
	wxSize toolSize = GetToolSize() ;
    int tw, th;
    GetSize(& tw, & th);
    m_maxWidth = m_maxHeight = 0 ;
	while (node)
	{
		wxToolBarTool *tool = (wxToolBarTool *)node->Data();
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( tool->GetBitmap1().GetRefData()) ;
		
		if(  !tool->IsSeparator()  )
		{
			Rect toolrect = { toolbarrect.top + kwxMacToolBarTopMargin , toolbarrect.left + x + kwxMacToolBarLeftMargin , 0 , 0 } ;
			toolrect.right = toolrect.left + toolSize.x ;
			toolrect.bottom = toolrect.top + toolSize.y ;
			
			PicHandle	icon = NULL ;
			if ( bmap )
			{
				if ( bmap->m_bitmapType == kMacBitmapTypePict )
					icon = bmap->m_hPict ;
				else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
				{
					if ( tool->GetBitmap1().GetMask() )
					{
						icon = MakePict( bmap->m_hBitmap , tool->GetBitmap1().GetMask()->GetMaskBitmap() ) ;
					}
					else
					{
						icon = MakePict( bmap->m_hBitmap , NULL ) ;
					}
				}
			}
			
			ControlHandle m_macToolHandle ;
			
			SInt16 behaviour = kControlBehaviorOffsetContents ;
			if ( tool->CanBeToggled() )
				behaviour += kControlBehaviorToggles ;
				
			if ( icon )
			{
				m_macToolHandle = UMANewControl( window , &toolrect , "\p" , true , 0 , 
					behaviour + kControlContentPictHandle , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
				ControlButtonContentInfo info ;
				
				info.contentType = kControlContentPictHandle ;
				info.u.picture = icon ;
				
				UMASetControlData( m_macToolHandle , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
			}
			else
			{
						m_macToolHandle = UMANewControl( window , &toolrect , "\p" , true , 0 , 
						behaviour  , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
			}
			m_macToolHandles.Add( m_macToolHandle ) ;
			tool->m_index = m_macToolHandles.Count() -1 ;
			if ( !tool->IsEnabled() )
			{
				UMADeactivateControl( m_macToolHandle ) ;
			}
			if ( tool->CanBeToggled() && tool->IsToggled() )
			{
   				::SetControlValue( m_macToolHandle , 1 ) ;
			}
			UMASetControlFontStyle( m_macToolHandle , &controlstyle ) ;
			ControlHandle container = GetParent()->MacGetContainerForEmbedding() ;
			wxASSERT_MSG( container != NULL , "No valid mac container control" ) ;
			UMAEmbedControl( m_macToolHandle , container ) ;
			
			x += (int)toolSize.x;
			noButtons ++;
		}
		else
		{
			m_macToolHandles.Add( NULL ) ;
			x += (int)toolSize.x / 4;
		}
	    if ( toolbarrect.left + x + kwxMacToolBarLeftMargin - m_x - localOrigin.h > m_maxWidth)
    	  	m_maxWidth = toolbarrect.left + x + kwxMacToolBarLeftMargin - m_x - localOrigin.h;
    	if (toolbarrect.top + kwxMacToolBarTopMargin  - m_y - localOrigin.v > m_maxHeight)
      		m_maxHeight = toolbarrect.top + kwxMacToolBarTopMargin - m_y - localOrigin.v ;

		node = node->Next();
	}

  if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
  {
    if ( m_maxRows == 0 )
    {
        // if not set yet, only one row
        SetRows(1);
    }
    m_maxWidth = clipRect.right - m_x ;
   	//m_maxWidth = tw ; // +=toolSize.x;
   	//m_maxWidth += m_xMargin ;
    m_maxHeight += toolSize.y;
  	m_maxHeight += m_yMargin;
  }
  else
  {
    if ( noButtons > 0 && m_maxRows == 0 )
    {
        // if not set yet, have one column
        SetRows(noButtons);
    }
    m_maxHeight = clipRect.bottom - m_y ;
    //m_maxHeight += m_yMargin ;
    m_maxWidth += toolSize.x;
  	m_maxWidth += m_xMargin;
  }

  SetSize(m_maxWidth, m_maxHeight);

  return TRUE;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x; m_defaultHeight = size.y;
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    return wxSize(m_defaultWidth + 4, m_defaultHeight + 4);
}

void wxToolBar::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	int index = 0 ;
	for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
	{
		if ( m_macToolHandles[index] == (void*) control )
		{
   			wxToolBarTool *tool = (wxToolBarTool *)m_tools.Nth( index )->Data();
			if ( tool->CanBeToggled() )
    		{
        		tool->Toggle( GetControlValue( control ) ) ;
    		}
			OnLeftClick( tool->GetId() , tool -> IsToggled() ) ;
			break ;
		}
	}
}

void wxToolBar::SetRows(int nRows)
{
    if ( nRows == m_maxRows )
    {
        // avoid resizing the frame uselessly
        return;
    }

    m_maxRows = nRows;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    MacClientToRootWindow( &x , &y ) ;
    Point pt = { y ,x } ;

	int index = 0 ;
	for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
	{
		if ( m_macToolHandles[index] )
		{
			Rect bounds ;
			GetControlBounds((ControlHandle) m_macToolHandles[index], &bounds ) ;
			if ( PtInRect( pt , &bounds ) )
			{
				return  (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ;
			}
		}
	}

    return (wxToolBarToolBase *)NULL;
}

wxString wxToolBar::MacGetToolTipString( wxPoint &pt )
{
	wxToolBarToolBase* tool = FindToolForPosition( pt.x , pt.y ) ;
	if ( tool )
	{
		return tool->GetShortHelp() ;
	}
	return "" ;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *t, bool enable)
{
        wxToolBarTool *tool = (wxToolBarTool *)t;
        if ( tool->m_index < 0 )
        	return ;
        	
        ControlHandle control = (ControlHandle) m_macToolHandles[ tool->m_index ] ;

		if ( enable )
			UMAActivateControl( control ) ;
		else
			UMADeactivateControl( control ) ;
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *t, bool toggle)
{
       wxToolBarTool *tool = (wxToolBarTool *)t;
        if ( tool->m_index < 0 )
        	return ;
        
       ControlHandle control = (ControlHandle) m_macToolHandles[ tool->m_index ] ;
   		::SetControlValue( control , toggle ) ;
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // nothing special to do here - we really create the toolbar buttons in
    // Realize() later
    tool->Attach(this);

    return TRUE;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *t, bool toggle)
{
       wxToolBarTool *tool = (wxToolBarTool *)t;
       // TODO: set toggle state
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarToolBase *tool)
{
		return TRUE ;
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
	Point localOrigin ;
	Rect clipRect ;
	WindowRef window ;
	wxWindow *win ;
	
	GetParent()->MacGetPortParams( &localOrigin , &clipRect , &window , &win ) ;
	if ( window && win )
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
				if ( ((wxControl*)parent)->GetMacControl() )
					SetUpControlBackground( ((wxControl*)parent)->GetMacControl() , -1 , true ) ;
				break ;
			}
			
			parent = parent->GetParent() ;
		} 
		int w = m_width ;
		int h = m_height ;
		
		if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
		{
	    	w = clipRect.right - m_x ;
		}
  		else
  		{
    		h = clipRect.bottom - m_y ;
		}
		
		Rect toolbarrect = { m_y + localOrigin.v , m_x + localOrigin.h , 
			m_y  + localOrigin.v + h , m_x + localOrigin.h + w } ;

		UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
		{
			int index = 0 ;
			for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
			{
				if ( m_macToolHandles[index] )
				{
					UMADrawControl( (ControlHandle) m_macToolHandles[index] ) ;
				}
			}
		}
		UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
		wxDC::MacInvalidateSetup() ;
	}
	/*
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
					if ( ((wxControl*)parent)->GetMacControl() )
						SetUpControlBackground( ((wxControl*)parent)->GetMacControl() , -1 , true ) ;
					break ;
				}
				
				parent = parent->GetParent() ;
			} 
			Rect toolbarrect = { m_y , m_x , m_y + m_height , m_x + m_width } ;

			UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
			{
				int index = 0 ;
				for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
				{
					if ( m_macToolHandles[index] )
					{
						UMADrawControl( (ControlHandle) m_macToolHandles[index] ) ;
 					}
				}
			}
			UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
			wxDC::MacInvalidateSetup() ;
		}
	}
	*/
}
void  wxToolBar::OnMouse( wxMouseEvent &event ) 
{
		
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

#endif // wxUSE_TOOLBAR

