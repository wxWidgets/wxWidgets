/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "tooltip.h"
#endif

#include "wx/setup.h"

#if wxUSE_TOOLTIPS

#include "wx/app.h"
#include "wx/dc.h"
#include "wx/window.h"
#include "wx/tooltip.h"
#include "wx/timer.h"
#include "wx/geometry.h"
#include "wx/mac/aga.h"
#include "wx/mac/uma.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

class wxMacToolTip
{
	public :
		wxMacToolTip( ) ;
		~wxMacToolTip() ;
		
		void			Setup( WindowRef window  , wxString text , wxPoint localPosition ) ;
		long			GetMark() { return m_mark ; }
		void 			Draw() ;
		void			Clear() ;
		bool			IsShown() { return m_shown  ; }
	private :
		
		wxString	m_label ;
		wxPoint m_position ;
		Rect			m_rect ;
		WindowRef	m_window ;	
		PicHandle	m_backpict ;
		bool		m_shown ;
		long		m_mark ;
} ;

class wxMacToolTipTimer : wxTimer
{
public:
	wxMacToolTipTimer(wxMacToolTip* tip, int iMilliseconds) ;

	void Notify()
	{
		if ( m_mark == m_tip->GetMark() )
			m_tip->Draw() ;

		delete this;
	}
	
protected:
	wxMacToolTip* 	m_tip;
	long			m_mark ;
};

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------
static long s_ToolTipDelay = 500 ;
static bool s_ShowToolTips = true ;
static wxMacToolTip s_ToolTip ;
static wxWindow* s_LastWindowEntered = NULL ;
static wxRect2DInt s_ToolTipArea ;
static WindowRef s_ToolTipWindowRef = NULL ;
wxToolTip::wxToolTip( const wxString &tip )
{
    m_text = tip;
    m_window = (wxWindow*) NULL;
}

wxToolTip::~wxToolTip()
{
}

void wxToolTip::SetTip( const wxString &tip )
{
     m_text = tip;

    if ( m_window )
    {
    	/*
        // update it immediately
        wxToolInfo ti(GetHwndOf(m_window));
        ti.lpszText = (wxChar *)m_text.c_str();

        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
        */
    }
}

void wxToolTip::SetWindow( wxWindow *win )
{
    m_window = win;
}

void wxToolTip::Enable( bool flag )
{
	if ( s_ShowToolTips != flag )
	{
		s_ShowToolTips = flag ;
		if ( s_ShowToolTips )
		{
		}
		else
		{
			s_ToolTip.Clear() ;
		}
	}
}

void wxToolTip::SetDelay( long msecs )
{
	s_ToolTipDelay = msecs ;
}

void wxToolTip::RelayEvent( wxWindow *win , wxMouseEvent &event )
{
	if ( s_ShowToolTips )
	{
		if ( event.GetEventType() == wxEVT_LEAVE_WINDOW )
		{
			s_ToolTip.Clear() ;
		}
		else if (event.GetEventType() == wxEVT_ENTER_WINDOW || event.GetEventType() == wxEVT_MOTION )
		{
			wxPoint2DInt where( event.m_x , event.m_y ) ;
			if ( s_LastWindowEntered == win && s_ToolTipArea.Contains( where ) )
			{
			}
			else
			{
				s_ToolTip.Clear() ;
				s_ToolTipArea = wxRect2DInt( event.m_x - 2 , event.m_y - 2 , 4 , 4 ) ;
				s_LastWindowEntered = win ;
				
				WindowRef window = win->GetMacRootWindow() ;
				int x = event.m_x ;
				int y = event.m_y ;
				wxPoint local( x , y ) ;
				win->MacClientToRootWindow( &x, &y ) ;
				wxPoint windowlocal( x , y ) ;
				s_ToolTip.Setup( window , win->MacGetToolTipString( local ) , windowlocal ) ;
			}
		}
	}
}

void wxToolTip::RemoveToolTips()
{
	s_ToolTip.Clear() ;
}
// --- mac specific

wxMacToolTipTimer::wxMacToolTipTimer( wxMacToolTip *tip , int msec )
{
	m_tip = tip;
	m_mark = tip->GetMark() ;
	Start(msec, true);
}

wxMacToolTip::wxMacToolTip()
{
	m_window = NULL ;
	m_backpict = NULL ;
	m_mark = 0 ;
	m_shown = false ;
}

void wxMacToolTip::Setup( WindowRef window  , wxString text , wxPoint localPosition ) 
{
	m_mark++ ;
	Clear() ;
	m_position = localPosition ; 
	m_label = wxMacMakeMacStringFromPC( text ) ;
	m_window = window ;
	s_ToolTipWindowRef = window ;
	m_backpict = NULL ;
	new wxMacToolTipTimer( this , s_ToolTipDelay ) ;
}

wxMacToolTip::~wxMacToolTip() 
{
	if ( m_backpict ) 
		Clear() ;
}

const short kTipBorder = 2 ;
const short kTipOffset = 5 ;

void wxMacToolTip::Draw()
{
	if ( m_label.Length() == 0 )
		return ;
		
	if ( m_window == s_ToolTipWindowRef )
	{
		#if TARGET_CARBON
		AGAPortHelper help( GetWindowPort( m_window ) );
		#else
		AGAPortHelper help( ( m_window ) );
		#endif
		m_shown = true ;

		SetOrigin( 0 , 0 ) ;
		TextFont( kFontIDGeneva ) ;
		TextSize( 9 ) ;
		TextFace( 0 ) ;
		FontInfo fontInfo;
		::GetFontInfo(&fontInfo);
		short lineh = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
		short height = 0 ;
	//	short width = TextWidth( m_label , 0 ,m_label.Length() ) ;
		
		int i = 0 ;
		int length = m_label.Length() ;
		int width = 0 ;
		int thiswidth = 0 ;
		int laststop = 0 ;
		const char *text = m_label ;
		while( i < length )
		{
			if( text[i] == 13 || text[i] == 10)
			{
				thiswidth = ::TextWidth( text , laststop , i - laststop ) ;
				if ( thiswidth > width )
					width = thiswidth ;
					
				height += lineh ;
				laststop = i+1 ;
			}
			i++ ;
		}
		if ( i - laststop > 0 )
		{
			thiswidth = ::TextWidth( text , laststop , i - laststop ) ;
			if ( thiswidth > width )
				width = thiswidth ;
			height += lineh ;
		}


		m_rect.left = m_position.x + kTipOffset;
		m_rect.top = m_position.y + kTipOffset;
		m_rect.right = m_rect.left + width + 2 * kTipBorder;
		m_rect.bottom = m_rect.top + height + 2 * kTipBorder;
		ClipRect( &m_rect ) ;
		BackColor( whiteColor ) ;
		ForeColor(blackColor ) ;
		m_backpict = OpenPicture(&m_rect);

		CopyBits(GetPortBitMapForCopyBits(GetWindowPort(m_window)), 
				   GetPortBitMapForCopyBits(GetWindowPort(m_window)), 
				   &m_rect, 
				   &m_rect, 
				   srcCopy, 
				   NULL);

		ClosePicture();
		RGBColor yellow = { 0xFFFF  , 0xFFFF , (153<<8)+153 } ;
		RGBBackColor( &yellow ) ;
		EraseRect( &m_rect ) ;
		FrameRect( &m_rect ) ;
		BackColor( whiteColor ) ;
		ForeColor(blackColor ) ;
		::MoveTo( m_rect.left + kTipBorder , m_rect.top + fontInfo.ascent + kTipBorder);

		i = 0 ;
		laststop = 0 ;
		height = 0 ;
		while( i < length )
		{
			if( text[i] == 13 || text[i] == 10)
			{
				::DrawText( text , laststop , i - laststop ) ;
				height += lineh ;
				::MoveTo( m_rect.left + kTipBorder , m_rect.top + fontInfo.ascent + kTipBorder + height );
				laststop = i+1 ;
			}
			i++ ;
		}
					
		::DrawText( text , laststop , i - laststop ) ;
		::TextMode( srcOr ) ;
		wxDC::MacInvalidateSetup() ;
		
	//	DrawText( m_label , 0 , m_label.Length() ) ;
	}
}

void wxToolTip::NotifyWindowDelete( WindowRef win ) 
{
	if ( win == s_ToolTipWindowRef )
	{
		s_ToolTipWindowRef = NULL ;
	}
}

void wxMacToolTip::Clear()
{
	m_mark++ ;
	if ( !m_shown )
		return ;
		 
	if ( m_window == s_ToolTipWindowRef && m_backpict )
	{
		#if TARGET_CARBON
		AGAPortHelper help( GetWindowPort(m_window) ) ;
		#else
		AGAPortHelper help( (m_window) ) ;
		#endif
		m_shown = false ;

		SetOrigin( 0 , 0 ) ;
		BackColor( whiteColor ) ;
		ForeColor(blackColor ) ;
		DrawPicture(m_backpict, &m_rect);
		KillPicture(m_backpict);
		m_backpict = NULL ;
		wxDC::MacInvalidateSetup() ;
	}
}

#endif

