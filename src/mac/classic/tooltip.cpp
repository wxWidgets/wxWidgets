/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/dc.h"
    #include "wx/timer.h"
#endif

#include "wx/geometry.h"
#include "wx/mac/uma.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

class wxMacToolTipTimer ;

class wxMacToolTip
{
    public :
        wxMacToolTip( ) ;
        ~wxMacToolTip() ;

        void            Setup( WindowRef window  , const wxString& text , wxPoint localPosition ) ;
        long            GetMark() { return m_mark ; }
        void             Draw() ;
        void            Clear() ;
        bool            IsShown() { return m_shown  ; }
    private :

        wxString    m_label ;
        wxPoint m_position ;
        Rect            m_rect ;
        WindowRef    m_window ;
        PicHandle    m_backpict ;
        bool        m_shown ;
        long        m_mark ;
        wxMacToolTipTimer* m_timer ;
#if TARGET_CARBON
        wxMacCFStringHolder m_helpTextRef ;
#endif
} ;

class wxMacToolTipTimer : public wxTimer
{
public:
    wxMacToolTipTimer() {} ;
    wxMacToolTipTimer(wxMacToolTip* tip, int iMilliseconds) ;
    virtual ~wxMacToolTipTimer() {} ;
    void Notify()
    {
        if ( m_mark == m_tip->GetMark() )
            m_tip->Draw() ;
    }
protected:
    wxMacToolTip*     m_tip;
    long        m_mark ;
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

IMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject)

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
    m_window = win ;
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

                WindowRef window = MAC_WXHWND( win->MacGetRootWindow() ) ;
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
    m_timer = NULL ;
}

void wxMacToolTip::Setup( WindowRef win  , const wxString& text , wxPoint localPosition )
{
    m_mark++ ;
    Clear() ;
    m_position = localPosition ;
    m_label = text ;
    m_window =win;
    s_ToolTipWindowRef = m_window ;
    m_backpict = NULL ;
    if ( m_timer )
        delete m_timer ;
    m_timer = new wxMacToolTipTimer( this , s_ToolTipDelay ) ;
}

wxMacToolTip::~wxMacToolTip()
{
    if ( m_timer ) {
        delete m_timer ;
        m_timer = NULL;
    }
    if ( m_backpict )
        Clear() ;
}

const short kTipBorder = 2 ;
const short kTipOffset = 5 ;

void wxMacToolTip::Draw()
{
    if ( m_label.empty() )
        return ;

    if ( m_window == s_ToolTipWindowRef )
    {
        m_shown = true ;
#if TARGET_CARBON
        HMHelpContentRec tag ;
        tag.version = kMacHelpVersion;
        SetRect( &tag.absHotRect , m_position.x - 2 , m_position.y - 2 , m_position.x + 2 , m_position.y + 2 ) ;
        GrafPtr port ;
        GetPort( &port ) ;
        SetPortWindowPort(m_window) ;
        LocalToGlobal( (Point *) &tag.absHotRect.top );
        LocalToGlobal( (Point *) &tag.absHotRect.bottom );
        SetPort( port );
        m_helpTextRef.Assign( m_label  , wxFONTENCODING_DEFAULT ) ;
        tag.content[kHMMinimumContentIndex].contentType = kHMCFStringContent ;
        tag.content[kHMMinimumContentIndex].u.tagCFString = m_helpTextRef ;
        tag.content[kHMMaximumContentIndex].contentType = kHMCFStringContent ;
        tag.content[kHMMaximumContentIndex].u.tagCFString = m_helpTextRef ;
        tag.tagSide = kHMDefaultSide;
        HMDisplayTag( &tag );
#else
        wxMacPortStateHelper help( (GrafPtr) GetWindowPort( m_window ) );
        FontFamilyID fontId ;
        Str255 fontName ;
        SInt16 fontSize ;
        Style fontStyle ;
        GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
        GetFNum( fontName, &fontId );

        TextFont( fontId ) ;
        TextSize( fontSize ) ;
        TextFace( fontStyle ) ;
        FontInfo fontInfo;
        ::GetFontInfo(&fontInfo);
        short lineh = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
        short height = 0 ;

        int i = 0 ;
        int length = m_label.length() ;
        int width = 0 ;
        int thiswidth = 0 ;
        int laststop = 0 ;
        wxCharBuffer text = m_label.mb_str( wxConvLocal)  ;

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
        Rect r ;
        GetPortBounds( GetWindowPort( m_window ) , &r ) ;
        if ( m_rect.top < 0 )
        {
            m_rect.bottom += -m_rect.top ;
            m_rect.top = 0 ;
        }
        if ( m_rect.left < 0 )
        {
            m_rect.right += -m_rect.left ;
            m_rect.left = 0 ;
        }
        if ( m_rect.right > r.right )
        {
            m_rect.left -= (m_rect.right - r.right ) ;
            m_rect.right = r.right ;
        }
        if ( m_rect.bottom > r.bottom )
        {
            m_rect.top -= (m_rect.bottom - r.bottom) ;
            m_rect.bottom = r.bottom ;
        }
        ClipRect( &m_rect ) ;
        BackColor( whiteColor ) ;
        ForeColor(blackColor ) ;
        GWorldPtr port ;
        NewGWorld( &port , wxDisplayDepth() , &m_rect , NULL , NULL , 0 ) ;
        CGrafPtr    origPort ;
        GDHandle    origDevice ;

        GetGWorld( &origPort , &origDevice ) ;
        SetGWorld( port , NULL ) ;

        m_backpict = OpenPicture(&m_rect);

        CopyBits(GetPortBitMapForCopyBits(GetWindowPort(m_window)),
            GetPortBitMapForCopyBits(port),
            &m_rect,
            &m_rect,
            srcCopy,
            NULL);
        ClosePicture();
        SetGWorld( origPort , origDevice ) ;
        DisposeGWorld( port ) ;
        PenNormal() ;

        RGBColor tooltipbackground = { 0xFFFF , 0xFFFF , 0xC000 } ;
        BackColor( whiteColor ) ;
        RGBForeColor( &tooltipbackground ) ;

        PaintRect( &m_rect ) ;
        ForeColor(blackColor ) ;
        FrameRect( &m_rect ) ;
        SetThemeTextColor(kThemeTextColorNotification,wxDisplayDepth(),true) ;
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
#endif
    }
}

void wxToolTip::NotifyWindowDelete( WXHWND win )
{
    if ( win == s_ToolTipWindowRef )
    {
        s_ToolTipWindowRef = NULL ;
    }
}

void wxMacToolTip::Clear()
{
    m_mark++ ;
    if ( m_timer )
    {
        delete m_timer ;
        m_timer = NULL ;
    }
    if ( !m_shown )
        return ;
#if TARGET_CARBON
    HMHideTag() ;
    m_helpTextRef.Release() ;
#else
    if ( m_window == s_ToolTipWindowRef && m_backpict )
    {
        wxMacPortStateHelper help( (GrafPtr) GetWindowPort(m_window) ) ;

        m_shown = false ;

        BackColor( whiteColor ) ;
        ForeColor(blackColor ) ;
        DrawPicture(m_backpict, &m_rect);
        KillPicture(m_backpict);
        m_backpict = NULL ;
    }
#endif
}

#endif // wxUSE_TOOLTIPS
