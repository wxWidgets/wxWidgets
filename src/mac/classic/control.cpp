/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/control.cpp
// Purpose:     wxControl class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/control.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/button.h"
    #include "wx/dialog.h"
    #include "wx/scrolbar.h"
    #include "wx/stattext.h"
    #include "wx/statbox.h"
    #include "wx/radiobox.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/spinbutt.h"

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_MOUSE_EVENTS( wxControl::OnMouseEvent )
    EVT_PAINT( wxControl::OnPaint )
END_EVENT_TABLE()

#include "wx/mac/uma.h"
#include "wx/mac/private.h"

// Item members


#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

typedef struct {
 unsigned short instruction;
 void (*function)();
} cdefRec, *cdefPtr, **cdefHandle;

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

ControlActionUPP wxMacLiveScrollbarActionUPP = NULL ;
wxControl *wxFindControlFromMacControl(ControlHandle inControl ) ;

pascal void wxMacLiveScrollbarActionProc( ControlHandle control , ControlPartCode partCode ) ;
pascal void wxMacLiveScrollbarActionProc( ControlHandle control , ControlPartCode partCode )
{
    if ( partCode != 0)
    {
        wxControl*  wx = (wxControl*) GetControlReference( control ) ;
        if ( wx )
        {
            wx->MacHandleControlClick( (WXWidget) control , partCode , true /* stillDown */ ) ;
        }
    }
}

ControlColorUPP wxMacSetupControlBackgroundUPP = NULL ;
ControlDefUPP wxMacControlActionUPP = NULL ;

pascal SInt32  wxMacControlDefinition(SInt16 varCode, ControlRef theControl, ControlDefProcMessage message, SInt32 param)
{

    wxControl*  wx = (wxControl*) wxFindControlFromMacControl( theControl ) ;
    if ( wx != NULL && wx->IsKindOf( CLASSINFO( wxControl ) ) )
    {
        if( message == drawCntl )
        {
            wxMacWindowClipper clip( wx ) ;
            return InvokeControlDefUPP( varCode , theControl , message , param , (ControlDefUPP) wx->MacGetControlAction() ) ;
        }
        else
            return InvokeControlDefUPP( varCode , theControl , message , param , (ControlDefUPP) wx->MacGetControlAction() ) ;
    }
    return  NULL ;
}

pascal OSStatus wxMacSetupControlBackground( ControlRef iControl , SInt16 iMessage , SInt16 iDepth , Boolean iIsColor )
{
    OSStatus status = noErr ;
    switch( iMessage )
    {
        case kControlMsgSetUpBackground :
            {
                wxControl*  wx = (wxControl*) GetControlReference( iControl ) ;
                if ( wx != NULL && wx->IsKindOf( CLASSINFO( wxControl ) ) )
                {
                    wxDC::MacSetupBackgroundForCurrentPort( wx->MacGetBackgroundBrush() ) ;
#if TARGET_CARBON
                    // under classic this would lead to partial redraws
                    RgnHandle clip = NewRgn() ;
                    int x = 0 , y = 0;

                    wx->MacWindowToRootWindow( &x,&y ) ;
                    CopyRgn( (RgnHandle) wx->MacGetVisibleRegion(false).GetWXHRGN() , clip ) ;
                    OffsetRgn( clip , x , y ) ;
                    SetClip( clip ) ;
                    DisposeRgn( clip ) ;
#endif
                }
                else
                {
                    status = paramErr ;
                }
            }
            break ;
        default :
            status = paramErr ;
            break ;
    }
    return status ;
}

wxControl::wxControl()
{
    m_macControl = NULL ;
    m_macControlAction = NULL ;
    m_macHorizontalBorder = 0 ; // additional pixels around the real control
    m_macVerticalBorder = 0 ;
    m_backgroundColour = *wxWHITE;
    m_foregroundColour = *wxBLACK;

    if ( wxMacLiveScrollbarActionUPP == NULL )
    {
#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0340)
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
    if ( parent )
    {
        m_backgroundColour = parent->GetBackgroundColour() ;
        m_foregroundColour = parent->GetForegroundColour() ;
    }
    if (rval) {
#if wxUSE_VALIDATORS
        SetValidator(validator);
#endif
    }
    return rval;
}

wxControl::~wxControl()
{
    m_isBeingDeleted = true;
    wxRemoveMacControlAssociation( this ) ;
    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
    if ( tlw )
    {
        if ( tlw->GetDefaultItem() == (wxButton*) this)
            tlw->SetDefaultItem(NULL);
    }
    if ( (ControlHandle) m_macControl )
    {
        // in case the callback might be called during destruction
        ::SetControlColorProc( (ControlHandle) m_macControl , NULL ) ;
        ::DisposeControl( (ControlHandle) m_macControl ) ;
        m_macControl = NULL ;
    }
}

void wxControl::SetLabel(const wxString& title)
{
    m_label = GetLabelText(title) ;

    if ( m_macControl )
    {
        UMASetControlTitle( (ControlHandle) m_macControl , m_label , m_font.GetEncoding() ) ;
    }
    Refresh() ;
}

wxSize wxControl::DoGetBestSize() const
{
    if ( (ControlHandle) m_macControl == NULL )
        return wxWindow::DoGetBestSize() ;

    Rect    bestsize = { 0 , 0 , 0 , 0 } ;
    short   baselineoffset ;
    int bestWidth, bestHeight ;
    ::GetBestControlRect( (ControlHandle) m_macControl , &bestsize , &baselineoffset ) ;

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

    bestWidth = bestsize.right - bestsize.left ;

    bestWidth += 2 * m_macHorizontalBorder ;

    bestHeight = bestsize.bottom - bestsize.top ;
    if ( bestHeight < 10 )
        bestHeight = 13 ;

    bestHeight += 2 * m_macVerticalBorder;


    return wxSize(bestWidth, bestHeight);
}

bool wxControl::ProcessCommand (wxCommandEvent & event)
{
    // Tries:
    // 1) OnCommand, starting at this window and working up parent hierarchy
    // 2) OnCommand then calls ProcessEvent to search the event tables.
    return GetEventHandler()->ProcessEvent(event);
}

// ------------------------
wxList *wxWinMacControlList = NULL;
wxControl *wxFindControlFromMacControl(ControlHandle inControl )
{
    wxNode *node = wxWinMacControlList->Find((long)inControl);
    if (!node)
        return NULL;
    return (wxControl *)node->GetData();
}

void wxAssociateControlWithMacControl(ControlHandle inControl, wxControl *control)
{
    // adding NULL WindowRef is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( inControl != (ControlHandle) NULL, wxT("attempt to add a NULL WindowRef to window list") );

    if ( !wxWinMacControlList->Find((long)inControl) )
        wxWinMacControlList->Append((long)inControl, control);
}

void wxRemoveMacControlAssociation(wxControl *control)
{
    if ( wxWinMacControlList )
        wxWinMacControlList->DeleteObject(control);
}

void wxControl::MacPreControlCreate( wxWindow *parent, wxWindowID id, wxString label ,
             const wxPoint& pos,
             const wxSize& size, long style,
             const wxValidator& validator,
             const wxString& name , WXRECTPTR outBounds , unsigned char* maclabel )
{
    m_label = label ;

    // These sizes will be adjusted in MacPostControlCreate
    m_width = size.x ;
    m_height = size.y ;
    m_x = pos.x ;
    m_y = pos.y ;

    ((Rect*)outBounds)->top = -10;
    ((Rect*)outBounds)->left = -10;
    ((Rect*)outBounds)->bottom = 0;
    ((Rect*)outBounds)->right = 0;

    wxMacStringToPascal( GetLabelText(label) , maclabel ) ;
}

void wxControl::MacPostControlCreate()
{
    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;
    DoSetWindowVariant( m_windowVariant ) ;
   /*
    if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
    {
        // no font
    }
    else if ( !UMAHasAquaLayout() && (IsKindOf( CLASSINFO( wxStaticBox ) ) || IsKindOf( CLASSINFO( wxRadioBox ) ) || IsKindOf( CLASSINFO( wxButton ) ) ) )
    {
        ControlFontStyleRec     controlstyle ;
        controlstyle.flags = kControlUseFontMask ;
        controlstyle.font = kControlFontSmallBoldSystemFont ;

        ::SetControlFontStyle( (ControlHandle) m_macControl , &controlstyle ) ;
    }
    else
    {
        ControlFontStyleRec     controlstyle ;
        controlstyle.flags = kControlUseFontMask ;

        if (IsKindOf( CLASSINFO( wxButton ) ) )
            controlstyle.font = kControlFontBigSystemFont ; // eventually kControlFontBigSystemFont ;
        else
            controlstyle.font = kControlFontSmallSystemFont ;

        ::SetControlFontStyle( (ControlHandle) m_macControl , &controlstyle ) ;
    }
    */
    ControlHandle container = (ControlHandle) GetParent()->MacGetContainerForEmbedding() ;
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    ::EmbedControl( (ControlHandle) m_macControl , container ) ;
    m_macControlIsShown  = MacIsReallyShown() ;

    wxAssociateControlWithMacControl( (ControlHandle) m_macControl , this ) ;
    if ( wxMacSetupControlBackgroundUPP == NULL )
    {
        wxMacSetupControlBackgroundUPP = NewControlColorUPP( wxMacSetupControlBackground ) ;
    }
    if ( wxMacControlActionUPP == NULL )
    {
        wxMacControlActionUPP = NewControlDefUPP( wxMacControlDefinition ) ;
    }
    // The following block of code is responsible for crashes when switching
    // back to windows, which can be seen in the dialogs sample.
    // It is disabled until a proper solution can be found.
#if 0
#if TARGET_CARBON
/*
    only working under classic carbon
    m_macControlAction = *(**(ControlHandle)m_macControl).contrlDefProc ;
    (**(ControlHandle)m_macControl).contrlDefProc = (Handle) &wxMacControlActionUPP ;
*/
#else
    m_macControlAction = *(**(ControlHandle)m_macControl).contrlDefProc ;

    cdefHandle cdef ;
    cdef = (cdefHandle) NewHandle( sizeof(cdefRec) ) ;
    if (  (**(ControlHandle)m_macControl).contrlDefProc != NULL )
    {
        (**cdef).instruction = 0x4EF9;  /* JMP instruction */
        (**cdef).function = (void(*)()) wxMacControlActionUPP;
        (**(ControlHandle)m_macControl).contrlDefProc = (Handle) cdef ;
    }
#endif
#endif
    SetControlColorProc( (ControlHandle) m_macControl , wxMacSetupControlBackgroundUPP ) ;

    // Adjust the controls size and position
    wxPoint pos(m_x, m_y);
    wxSize best_size( DoGetBestSize() );
    wxSize new_size( m_width, m_height );

    m_x = m_y = m_width = m_height = -1;  // Forces SetSize to move/size the control

    if (new_size.x == -1) {
        new_size.x = best_size.x;
    }
    if (new_size.y == -1) {
        new_size.y = best_size.y;
    }

    SetSize(pos.x, pos.y, new_size.x, new_size.y);

#if wxUSE_UNICODE
    UMASetControlTitle( (ControlHandle) m_macControl , GetLabelText(m_label) , m_font.GetEncoding() ) ;
#endif

    if ( m_macControlIsShown )
        UMAShowControl( (ControlHandle) m_macControl ) ;

    SetCursor( *wxSTANDARD_CURSOR ) ;

    Refresh() ;
}

void wxControl::MacAdjustControlRect()
{
    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;
    if ( m_width == -1 || m_height == -1 )
    {
        Rect    bestsize = { 0 , 0 , 0 , 0 } ;
        short   baselineoffset ;

        ::GetBestControlRect( (ControlHandle) m_macControl , &bestsize , &baselineoffset ) ;

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
                m_width = m_label.length() * 8 + 12 ;
                if ( m_width < 70 )
                  m_width = 70 ;
            }
            else if ( IsKindOf( CLASSINFO( wxStaticText ) ) )
            {
                m_width = m_label.length() * 8 ;
            }
            else
                m_width = bestsize.right - bestsize.left ;

            m_width += 2 * m_macHorizontalBorder + MacGetLeftBorderSize() + MacGetRightBorderSize() ;
        }
        if ( m_height == -1 )
        {
            m_height = bestsize.bottom - bestsize.top ;
            if ( m_height < 10 )
                m_height = 13 ;

            m_height += 2 * m_macVerticalBorder + MacGetTopBorderSize() + MacGetBottomBorderSize() ;
        }
        MacUpdateDimensions() ;
    }
}

WXWidget wxControl::MacGetContainerForEmbedding()
{
    if ( m_macControl )
        return m_macControl ;

    return wxWindow::MacGetContainerForEmbedding() ;
}

void wxControl::MacUpdateDimensions()
{
    // actually in the current systems this should never be possible, but later reparenting
    // may become a reality

    if ( (ControlHandle) m_macControl == NULL )
        return ;

    if ( GetParent() == NULL )
        return ;

    WindowRef rootwindow = (WindowRef) MacGetRootWindow() ;
    if ( rootwindow == NULL )
        return ;

    Rect oldBounds ;
    GetControlBounds( (ControlHandle) m_macControl , &oldBounds ) ;

    int new_x = m_x + MacGetLeftBorderSize() + m_macHorizontalBorder ;
    int new_y = m_y + MacGetTopBorderSize() + m_macVerticalBorder ;
    int new_width = m_width - MacGetLeftBorderSize() - MacGetRightBorderSize() - 2 * m_macHorizontalBorder ;
    int new_height = m_height - MacGetTopBorderSize() - MacGetBottomBorderSize() - 2 * m_macVerticalBorder ;

    GetParent()->MacWindowToRootWindow( & new_x , & new_y ) ;
    bool doMove = new_x != oldBounds.left || new_y != oldBounds.top ;
    bool doResize =  ( oldBounds.right - oldBounds.left ) != new_width || (oldBounds.bottom - oldBounds.top ) != new_height ;
    if ( doMove || doResize )
    {
        InvalWindowRect( rootwindow, &oldBounds ) ;
        if ( doMove )
        {
            UMAMoveControl( (ControlHandle) m_macControl , new_x , new_y ) ;
        }
        if ( doResize )
        {
            UMASizeControl( (ControlHandle) m_macControl , new_width , new_height ) ;
        }
    }
}

void wxControl::MacSuperChangedPosition()
{
     MacUpdateDimensions() ;
    wxWindow::MacSuperChangedPosition() ;
}

void wxControl::MacSuperEnabled( bool enabled )
{
    Refresh(false) ;
    wxWindow::MacSuperEnabled( enabled ) ;
}

void wxControl::MacSuperShown( bool show )
{
    if ( (ControlHandle) m_macControl )
    {
        if ( !show )
        {
            if ( m_macControlIsShown )
            {
                ::UMAHideControl( (ControlHandle) m_macControl ) ;
                m_macControlIsShown = false ;
            }
        }
        else
        {
            if ( MacIsReallyShown() && !m_macControlIsShown )
            {
                ::UMAShowControl( (ControlHandle) m_macControl ) ;
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
    wxWindow::DoSetSize( x , y ,width , height ,sizeFlags ) ;
#if 0
    {
        Rect meta , control ;
        GetControlBounds( (ControlHandle) m_macControl , &control ) ;
        RgnHandle rgn = NewRgn() ;
        GetControlRegion( (ControlHandle) m_macControl , kControlStructureMetaPart , rgn ) ;
        GetRegionBounds( rgn , &meta ) ;
        if ( !EmptyRect( &meta ) )
        {
            wxASSERT( meta.left >= control.left - m_macHorizontalBorder ) ;
            wxASSERT( meta.right <= control.right + m_macHorizontalBorder ) ;
            wxASSERT( meta.top >= control.top - m_macVerticalBorder ) ;
            wxASSERT( meta.bottom <= control.bottom + m_macVerticalBorder ) ;
        }
        DisposeRgn( rgn ) ;
    }
#endif
    return ;
}

bool  wxControl::Show(bool show)
{
    if ( !wxWindow::Show( show ) )
        return false ;

    if ( (ControlHandle) m_macControl )
    {
        if ( !show )
        {
            if ( m_macControlIsShown )
            {
                ::UMAHideControl( (ControlHandle) m_macControl ) ;
                m_macControlIsShown = false ;
            }
        }
        else
        {
            if ( MacIsReallyShown() && !m_macControlIsShown )
            {
                ::UMAShowControl( (ControlHandle) m_macControl ) ;
                m_macControlIsShown = true ;
            }
        }
    }
    return true ;
}

bool  wxControl::Enable(bool enable)
{
    if ( !wxWindow::Enable(enable) )
        return false;

    if ( (ControlHandle) m_macControl )
    {
        if ( enable )
            UMAActivateControl( (ControlHandle) m_macControl ) ;
        else
            UMADeactivateControl( (ControlHandle) m_macControl ) ;
    }
    return true ;
}

void wxControl::Refresh(bool eraseBack, const wxRect *rect)
{
    wxWindow::Refresh( eraseBack , rect ) ;
}

void wxControl::MacRedrawControl()
{
    if ( (ControlHandle) m_macControl && MacGetRootWindow() && m_macControlIsShown )
    {
        wxClientDC dc(this) ;
        wxMacPortSetter helper(&dc) ;
        wxMacWindowClipper clipper(this) ;
        wxDC::MacSetupBackgroundForCurrentPort( MacGetBackgroundBrush() ) ;
        UMADrawControl( (ControlHandle) m_macControl ) ;
    }
}

void wxControl::OnPaint(wxPaintEvent& event)
{
    if ( (ControlHandle) m_macControl )
    {
        wxPaintDC dc(this) ;
        wxMacPortSetter helper(&dc) ;
        wxMacWindowClipper clipper(this) ;
        wxDC::MacSetupBackgroundForCurrentPort( MacGetBackgroundBrush() ) ;
        UMADrawControl( (ControlHandle) m_macControl ) ;
    }
    else
    {
      event.Skip() ;
    }
}
void wxControl::OnEraseBackground(wxEraseEvent& event)
{
    wxWindow::OnEraseBackground( event ) ;
}

void  wxControl::OnKeyDown( wxKeyEvent &event )
{
    if ( (ControlHandle) m_macControl == NULL )
        return ;

#if TARGET_CARBON

    char charCode ;
    UInt32 keyCode ;
    UInt32 modifiers ;

    GetEventParameter( (EventRef) wxTheApp->MacGetCurrentEvent(), kEventParamKeyMacCharCodes, typeChar, NULL,sizeof(char), NULL,&charCode );
    GetEventParameter( (EventRef) wxTheApp->MacGetCurrentEvent(), kEventParamKeyCode, typeUInt32, NULL,  sizeof(UInt32), NULL, &keyCode );
       GetEventParameter((EventRef) wxTheApp->MacGetCurrentEvent(), kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);

    ::HandleControlKey( (ControlHandle) m_macControl , keyCode , charCode , modifiers ) ;

#else
    EventRecord *ev = (EventRecord*) wxTheApp->MacGetCurrentEvent() ;
    short keycode ;
    short keychar ;
    keychar = short(ev->message & charCodeMask);
    keycode = short(ev->message & keyCodeMask) >> 8 ;

    ::HandleControlKey( (ControlHandle) m_macControl , keycode , keychar , ev->modifiers ) ;
#endif
}

void  wxControl::OnMouseEvent( wxMouseEvent &event )
{
    if ( (ControlHandle) m_macControl == NULL )
    {
        event.Skip() ;
        return ;
    }

    if (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK )
    {

        int x = event.m_x ;
        int y = event.m_y ;

        MacClientToRootWindow( &x , &y ) ;

        ControlHandle   control ;
        Point       localwhere ;
        SInt16      controlpart ;

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
        {
            control = (ControlHandle) m_macControl ;
            if ( control && ::IsControlActive( control ) )
            {
                {
                    controlpart = ::HandleControlClick( control , localwhere , modifiers , (ControlActionUPP) -1 ) ;
                    wxTheApp->s_lastMouseDown = 0 ;
                    if ( control && controlpart != kControlNoPart )
                    {
                        MacHandleControlClick( (WXWidget) control , controlpart , false /* mouse not down anymore */ ) ;
                    }
                }
            }
        }
    }
    else
    {
        event.Skip() ;
    }
}

bool wxControl::MacCanFocus() const
{
    if ( (ControlHandle) m_macControl == NULL )
        return true ;
    else
        return false ;
}

void wxControl::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED( mouseStillDown ) )
{
    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;
}

void wxControl::DoSetWindowVariant( wxWindowVariant variant )
{
    if ( m_macControl == NULL )
    {
        wxWindow::SetWindowVariant( variant ) ;
        return ;

    }
    m_windowVariant = variant ;

    ControlSize size ;
    ControlFontStyleRec fontStyle;
    fontStyle.flags = kControlUseFontMask  ;

    // we will get that from the settings later
    // and make this NORMAL later, but first
    // we have a few calculations that we must fix

    if ( variant == wxWINDOW_VARIANT_NORMAL )
    {
        if ( IsKindOf( CLASSINFO( wxScrollBar ) ) )
            variant  = wxWINDOW_VARIANT_NORMAL ;
        else
            variant = wxWINDOW_VARIANT_SMALL ;
    }

    switch ( variant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            size = kControlSizeNormal;
            fontStyle.font = kControlFontBigSystemFont;
            break ;
        case wxWINDOW_VARIANT_SMALL :
            size = kControlSizeSmall;
            fontStyle.font = kControlFontSmallSystemFont;
            break ;
        case wxWINDOW_VARIANT_MINI :
           if (UMAGetSystemVersion() >= 0x1030 )
            {
                size = 3 ; // not always defined in the header
                fontStyle.font = -5 ; // not always defined in the header
            }
            else
            {
                size = kControlSizeSmall;
                fontStyle.font = kControlFontSmallSystemFont;
            }
            break;
            break ;
        case wxWINDOW_VARIANT_LARGE :
            size = kControlSizeLarge;
            fontStyle.font = kControlFontBigSystemFont;
            break ;
        default:
            wxFAIL_MSG(_T("unexpected window variant"));
            break ;
    }
    ::SetControlData( (ControlHandle) m_macControl , kControlEntireControl, kControlSizeTag, sizeof( ControlSize ), &size );
    ::SetControlFontStyle( (ControlHandle) m_macControl , &fontStyle );
}
