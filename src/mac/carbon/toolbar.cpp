/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     The wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "toolbar.h"
#endif

#include "wx/wx.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/bitmap.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
    EVT_PAINT( wxToolBar::OnPaint ) 
END_EVENT_TABLE()
#endif

#include "wx/mac/uma.h"
#include "wx/geometry.h"
// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmap& bmpNormal,
                  const wxBitmap& bmpDisabled,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelp,
                  const wxString& longHelp) ;
                  
    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init() ;
    }
    
    ~wxToolBarTool()
    {
        if ( m_controlHandle )
            DisposeControl( m_controlHandle ) ;
    }
    
    WXWidget GetControlHandle() { return (WXWidget) m_controlHandle ; }
    void SetControlHandle( ControlRef handle ) { m_controlHandle = handle ; }

    void SetSize(const wxSize& size) ;
    void SetPosition( const wxPoint& position ) ;
    
    wxSize GetSize() const
    {
        if ( IsControl() )
        {
            return GetControl()->GetSize() ;
        }
        else if ( IsButton() )
        {
            return GetToolBar()->GetToolSize() ;
        }
        else
        {
            wxSize sz = GetToolBar()->GetToolSize() ;
            sz.x /= 4 ;
            sz.y /= 4 ;
            return sz ;
        }
    }
    wxPoint GetPosition() const
    {
        return wxPoint(m_x, m_y);
    }    
    bool DoEnable( bool enable ) ;
private :
    void Init() 
    {
        m_controlHandle = NULL ;
    }
    ControlRef m_controlHandle ;

    wxCoord     m_x;
    wxCoord     m_y;
};

static const EventTypeSpec eventList[] =
{
	{ kEventClassControl , kEventControlHit } ,
} ;

static pascal OSStatus wxMacToolBarToolControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;
    
    ControlRef controlRef ;

    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    switch( GetEventKind( event ) )
    {
        case kEventControlHit :
            {
                wxToolBarTool* tbartool = (wxToolBarTool*)data ;
                if ( tbartool->CanBeToggled() )
                {
                    ((wxToolBar*)tbartool->GetToolBar())->ToggleTool(tbartool->GetId(), GetControl32BitValue((ControlRef)tbartool->GetControlHandle()));
                }
                ((wxToolBar*)tbartool->GetToolBar())->OnLeftClick( tbartool->GetId() , tbartool -> IsToggled() ) ;
                result = noErr; 
            }
            break ;
        default :
            break ;
    }
    return result ;
}

pascal OSStatus wxMacToolBarToolEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassControl :
            result = wxMacToolBarToolControlEventHandler( handler, event, data ) ;
            break ;
        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacToolBarToolEventHandler )

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

bool wxToolBarTool::DoEnable(bool enable)
{
    if ( IsControl() )
    {
        GetControl()->Enable( enable ) ;
    }
    else if ( IsButton() )
    {
#if TARGET_API_MAC_OSX
        if ( enable )
            EnableControl( m_controlHandle ) ;
        else
            DisableControl( m_controlHandle ) ;
#else
        if ( enable )
            ActivateControl( m_controlHandle ) ;
        else
            DeactivateControl( m_controlHandle ) ;
#endif
    }
    return true ;
}
void wxToolBarTool::SetSize(const wxSize& size)
{
    if ( IsControl() )
    {
        GetControl()->SetSize( size ) ;
    }
}

void wxToolBarTool::SetPosition(const wxPoint& position)
{
    m_x = position.x;
    m_y = position.y;

    if ( IsButton() )
    {
        int x , y ;
        x = y = 0 ;
        int mac_x = position.x ;
        int mac_y = position.y ;
#if !TARGET_API_MAC_OSX
        WindowRef rootwindow = (WindowRef) GetToolBar()->MacGetTopLevelWindowRef() ;    
        GetToolBar()->MacWindowToRootWindow( &x , &y ) ;
        mac_x += x;
        mac_y += y;
#endif
        Rect contrlRect ;       
        GetControlBounds( m_controlHandle , &contrlRect ) ; 
        int former_mac_x = contrlRect.left ;
        int former_mac_y = contrlRect.top ;
        GetToolBar()->GetToolSize() ;
        
        if ( mac_x != former_mac_x || mac_y != former_mac_y )
        {
            UMAMoveControl( m_controlHandle , mac_x , mac_y ) ;
        }
    }
    else if ( IsControl() )
    {
        GetControl()->Move( position ) ;
    }
}

const short kwxMacToolBarToolDefaultWidth = 24 ;
const short kwxMacToolBarToolDefaultHeight = 22 ;
const short kwxMacToolBarTopMargin = 2 ;
const short kwxMacToolBarLeftMargin = 2 ;

wxToolBarTool::wxToolBarTool(wxToolBar *tbar,
  int id,
  const wxString& label,
  const wxBitmap& bmpNormal,
  const wxBitmap& bmpDisabled,
  wxItemKind kind,
  wxObject *clientData,
  const wxString& shortHelp,
  const wxString& longHelp)
        : wxToolBarToolBase(tbar, id, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp)
{
    Init();
    
    if (id == wxID_SEPARATOR) return;
    
    WindowRef window = (WindowRef) tbar->MacGetTopLevelWindowRef() ;    
    wxSize toolSize = tbar->GetToolSize() ;    
    Rect toolrect = { 0, 0 , toolSize.y , toolSize.x } ;
    
    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , GetNormalBitmap() ) ;
    
    SInt16 behaviour = kControlBehaviorOffsetContents ;
    if ( CanBeToggled() )
        behaviour += kControlBehaviorToggles ;
        
    CreateBevelButtonControl( window , &toolrect , CFSTR("") , kControlBevelButtonNormalBevel , behaviour , &info , 
        0 , 0 , 0 , &m_controlHandle ) ;
        
    InstallControlEventHandler( (ControlRef) m_controlHandle, GetwxMacToolBarToolEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this,NULL);
        
    UMAShowControl( m_controlHandle ) ;

    if ( CanBeToggled() && IsToggled() )
        ::SetControl32BitValue( m_controlHandle , 1 ) ;
    else
        ::SetControl32BitValue( m_controlHandle , 0 ) ;
    
    ControlRef container = (ControlRef) tbar->GetHandle() ;
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    ::EmbedControl( m_controlHandle , container ) ;
}


wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxString& label,
                                         const wxBitmap& bmpNormal,
                                         const wxBitmap& bmpDisabled,
                                         wxItemKind kind,
                                         wxObject *clientData,
                                         const wxString& shortHelp,
                                         const wxString& longHelp)
{
    return new wxToolBarTool(this, id, label, bmpNormal, bmpDisabled, kind,
                             clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

void wxToolBar::Init()
{
    m_maxWidth = -1;
    m_maxHeight = -1;
    m_defaultWidth = kwxMacToolBarToolDefaultWidth;
    m_defaultHeight = kwxMacToolBarToolDefaultHeight;
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{  
    if ( !wxToolBarBase::Create( parent , id , pos , size , style ) )
        return FALSE ;
    
    return TRUE;
}

wxToolBar::~wxToolBar()
{    
    // we must refresh the frame size when the toolbar is deleted but the frame
    // is not - otherwise toolbar leaves a hole in the place it used to occupy
}

bool wxToolBar::Realize()
{
    if (m_tools.GetCount() == 0)
        return FALSE;

    int x = m_xMargin + kwxMacToolBarLeftMargin ;
    int y = m_yMargin + kwxMacToolBarTopMargin ;

    int tw, th;
    GetSize(& tw, & th);
    
    int maxWidth = 0 ;
    int maxHeight = 0 ;
    
    int maxToolWidth = 0;
    int maxToolHeight = 0;

    // Find the maximum tool width and height
    wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        wxSize sz = tool->GetSize() ;

        if ( sz.x > maxToolWidth )
            maxToolWidth = sz.x ;
        if (sz.y> maxToolHeight)
            maxToolHeight = sz.y;

        node = node->GetNext();
    }

    bool lastWasRadio = FALSE;
    node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        wxSize cursize = tool->GetSize() ;
        
        bool isRadio = FALSE;

        if ( tool->IsButton() && tool->GetKind() == wxITEM_RADIO )
        {
            if ( !lastWasRadio )
            {
                if (tool->Toggle(true))
                {
                    DoToggleTool(tool, true);
                }
            }
            isRadio = TRUE;
        }
        else
        {
            isRadio = FALSE;
        }
        lastWasRadio = isRadio;

        // for the moment we just do a single row/column alignement
        if ( x + cursize.x > maxWidth )
            maxWidth = x + cursize.x ;
        if ( y + cursize.y > maxHeight )
            maxHeight = y + cursize.y ;
            
        tool->SetPosition( wxPoint( x , y ) ) ;
        
        if ( GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            y += cursize.y ;
        }
        else
        {
            x += cursize.x ;
        }

        node = node->GetNext();
    }
    
    if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
    {
        if ( m_maxRows == 0 )
        {
            // if not set yet, only one row
            SetRows(1);
        }
        maxWidth = tw ; 
        maxHeight += m_yMargin + kwxMacToolBarTopMargin;
        m_maxHeight = maxHeight ;
    }
    else
    {
        if ( GetToolsCount() > 0 && m_maxRows == 0 )
        {
            // if not set yet, have one column
            SetRows(GetToolsCount());
        }
        maxHeight = th ;
        maxWidth += m_xMargin + kwxMacToolBarLeftMargin;
        m_maxWidth = maxWidth ;
    }
    
    SetSize( maxWidth, maxHeight );
    InvalidateBestSize();
    
    return TRUE;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x+4; m_defaultHeight = size.y+4;
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    return wxSize(m_defaultWidth + 4, m_defaultHeight + 4);
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

void wxToolBar::MacSuperChangedPosition() 
{
    wxWindow::MacSuperChangedPosition() ;
    Realize() ;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData() ;
        wxRect2DInt r( tool->GetPosition() , tool->GetSize() ) ;
        if ( r.Contains( wxPoint( x , y ) ) )
        {
            return tool;
        }

        node = node->GetNext();
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
    return wxEmptyString ;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *t, bool enable)
{
    ((wxToolBarTool*)t)->DoEnable( enable ) ;
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *t, bool toggle)
{
    wxToolBarTool *tool = (wxToolBarTool *)t;
    if ( tool->IsButton() )
    {
        ::SetControl32BitValue( (ControlRef) tool->GetControlHandle() , toggle ) ;
    }
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // nothing special to do here - we relayout in Realize() later
    tool->Attach(this);
    InvalidateBestSize();

    return TRUE;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    wxFAIL_MSG( _T("not implemented") );
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    wxToolBarToolsList::compatibility_iterator node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        wxToolBarToolBase *tool2 = node->GetData();
        if ( tool2 == tool )
        {
            // let node point to the next node in the list
            node = node->GetNext();

            break;
        }
    }

    wxSize sz = ((wxToolBarTool*)tool)->GetSize() ;

    tool->Detach();

    // and finally reposition all the controls after this one
    
    for ( /* node -> first after deleted */ ; node; node = node->GetNext() )
    {
        wxToolBarTool *tool2 = (wxToolBarTool*) node->GetData();
        wxPoint pt = tool2->GetPosition() ;

        if ( GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            pt.y -= sz.y ;
        }
        else
        {
            pt.x -= sz.x ;
        }
        tool2->SetPosition( pt ) ;
    }
    
    InvalidateBestSize();
    return TRUE ;
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this) ;
    wxMacPortSetter helper(&dc) ;
    int w, h ;
    GetSize( &w , &h ) ;
    
    Rect toolbarrect = { dc.YLOG2DEVMAC(0) , dc.XLOG2DEVMAC(0) , 
        dc.YLOG2DEVMAC(h) , dc.XLOG2DEVMAC(w) } ;
/*
    if( toolbarrect.left < 0 )
        toolbarrect.left = 0 ;
    if ( toolbarrect.top < 0 )
        toolbarrect.top = 0 ;
*/
    if ( !MacGetTopLevelWindow()->MacGetMetalAppearance() )
    {
        UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
    }
    else
    {
#if TARGET_API_MAC_OSX
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        HIRect hiToolbarrect = CGRectMake( dc.YLOG2DEVMAC(0) , dc.XLOG2DEVMAC(0) , 
        dc.YLOG2DEVREL(h) , dc.XLOG2DEVREL(w) );
        CGContextRef cgContext ;
        Rect bounds ;
        GetPortBounds( (CGrafPtr) dc.m_macPort , &bounds ) ;
        QDBeginCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
        CGContextTranslateCTM( cgContext , 0 , bounds.bottom - bounds.top ) ;
        CGContextScaleCTM( cgContext , 1 , -1 ) ;

        {
            HIThemeBackgroundDrawInfo drawInfo ;
            drawInfo.version = 0 ;
            drawInfo.state = kThemeStateActive ;
            drawInfo.kind = kThemeBackgroundMetal ;
            HIThemeApplyBackground( &hiToolbarrect, &drawInfo , cgContext,kHIThemeOrientationNormal) ;
        }
        QDEndCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
    }
    else
#endif
    {
        UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
    }
#endif
    }
    event.Skip() ;
}

#endif // wxUSE_TOOLBAR

