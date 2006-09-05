/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/toolbar.cpp
// Purpose:     wxToolBar
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     The wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/notebook.h"
#include "wx/tabctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
    EVT_MOUSE_EVENTS( wxToolBar::OnMouse )
    EVT_PAINT( wxToolBar::OnPaint )
END_EVENT_TABLE()

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

    virtual ~wxToolBarTool()
    {
        if ( m_controlHandle )
            DisposeControl( m_controlHandle ) ;
    }

    ControlHandle   GetControlHandle() { return m_controlHandle ; }
    void SetControlHandle( ControlHandle handle ) { m_controlHandle = handle ; }

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
private :
    void Init()
    {
        m_controlHandle = NULL ;
    }
    ControlHandle m_controlHandle ;

    wxCoord     m_x;
    wxCoord     m_y;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

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
        WindowRef rootwindow = (WindowRef) GetToolBar()->MacGetRootWindow() ;
        GetToolBar()->MacWindowToRootWindow( &x , &y ) ;
        int mac_x = x + position.x ;
        int mac_y = y + position.y ;


        Rect contrlRect ;
        GetControlBounds( m_controlHandle , &contrlRect ) ;
        int former_mac_x = contrlRect.left ;
        int former_mac_y = contrlRect.top ;
        wxSize sz = GetToolBar()->GetToolSize() ;

        if ( mac_x != former_mac_x || mac_y != former_mac_y )
        {
            {
                Rect inval = { former_mac_y , former_mac_x , former_mac_y + sz.y , former_mac_x + sz.x } ;
                InvalWindowRect( rootwindow , &inval ) ;
            }
            UMAMoveControl( m_controlHandle , mac_x , mac_y ) ;
            {
                Rect inval = { mac_y , mac_x , mac_y + sz.y , mac_x + sz.x } ;
                InvalWindowRect( rootwindow , &inval ) ;
            }
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

    WindowRef window = (WindowRef) tbar->MacGetRootWindow() ;
    wxSize toolSize = tbar->GetToolSize() ;
    Rect toolrect = { 0, 0 , toolSize.y , toolSize.x } ;

    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , GetNormalBitmap() ) ;

    SInt16 behaviour = kControlBehaviorOffsetContents ;
    if ( CanBeToggled() )
        behaviour += kControlBehaviorToggles ;

    if ( info.contentType != kControlNoContent )
    {
        m_controlHandle = ::NewControl( window , &toolrect , "\p" , false , 0 ,
                                        behaviour + info.contentType , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;

        ::SetControlData( m_controlHandle , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
    else
    {
        m_controlHandle = ::NewControl( window , &toolrect , "\p" , false , 0 ,
                                        behaviour  , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
    }
    UMAShowControl( m_controlHandle ) ;
    if ( !IsEnabled() )
    {
        UMADeactivateControl( m_controlHandle ) ;
    }
    if ( CanBeToggled() && IsToggled() )
    {
        ::SetControl32BitValue( m_controlHandle , 1 ) ;
    }
    else
    {
        ::SetControl32BitValue( m_controlHandle , 0 ) ;
    }

    ControlHandle container = (ControlHandle) tbar->MacGetContainerForEmbedding() ;
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

    SetName(name);

    m_windowStyle = style;
    parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    if (id == wxID_ANY)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    {
        m_width = size.x ;
        m_height = size.y ;
        int x = pos.x ;
        int y = pos.y ;
        AdjustForParentClientOrigin(x, y, wxSIZE_USE_EXISTING);
        m_x = x ;
        m_y = y ;
    }

    return true;
}

wxToolBar::~wxToolBar()
{
    // we must refresh the frame size when the toolbar is deleted but the frame
    // is not - otherwise toolbar leaves a hole in the place it used to occupy
}

bool wxToolBar::Realize()
{
    if (m_tools.GetCount() == 0)
        return false;

    int x = m_xMargin + kwxMacToolBarLeftMargin ;
    int y = m_yMargin + kwxMacToolBarTopMargin ;

    int tw, th;
    GetSize(& tw, & th);

    int maxWidth = 0 ;
    int maxHeight = 0 ;

    int maxToolWidth = 0;
    int maxToolHeight = 0;

    // Find the maximum tool width and height
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
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

    node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        wxSize cursize = tool->GetSize() ;

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

    SetSize(maxWidth, maxHeight);
    InvalidateBestSize();

    return true;
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

void wxToolBar::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED( mouseStillDown ) )
{
    wxToolBarToolsList::Node *node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        wxToolBarTool* tool = (wxToolBarTool*) node->GetData() ;
        if ( tool->IsButton() )
        {
           if( (WXWidget) tool->GetControlHandle() == control )
           {
                if ( tool->CanBeToggled() )
                {
                    tool->Toggle( GetControl32BitValue( (ControlHandle) control ) ) ;
                }
                OnLeftClick( tool->GetId() , tool -> IsToggled() ) ;
                break ;
           }
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

void wxToolBar::MacSuperChangedPosition()
{
    wxWindow::MacSuperChangedPosition() ;
    Realize() ;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
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
    if (!IsShown())
        return ;

    wxToolBarTool *tool = (wxToolBarTool *)t;
    if ( tool->IsControl() )
    {
        tool->GetControl()->Enable( enable ) ;
    }
    else if ( tool->IsButton() )
    {
        if ( enable )
            UMAActivateControl( tool->GetControlHandle() ) ;
        else
            UMADeactivateControl( tool->GetControlHandle() ) ;
    }
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *t, bool toggle)
{
    if (!IsShown())
        return ;

    wxToolBarTool *tool = (wxToolBarTool *)t;
    if ( tool->IsButton() )
    {
        ::SetControl32BitValue( tool->GetControlHandle() , toggle ) ;
    }
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // nothing special to do here - we relayout in Realize() later
    tool->Attach(this);
    InvalidateBestSize();

    return true;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    wxFAIL_MSG( _T("not implemented") );
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    wxToolBarToolsList::Node *node;
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
    return true ;
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this) ;
    wxMacPortSetter helper(&dc) ;

    Rect toolbarrect = { dc.YLOG2DEVMAC(0) , dc.XLOG2DEVMAC(0) ,
        dc.YLOG2DEVMAC(m_height) , dc.XLOG2DEVMAC(m_width) } ;
    UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
    {
        wxToolBarToolsList::Node *node;
        for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
        {
            wxToolBarTool* tool = (wxToolBarTool*) node->GetData() ;
            if ( tool->IsButton() )
            {
               UMADrawControl( tool->GetControlHandle() ) ;
            }
        }
    }
}

void  wxToolBar::OnMouse( wxMouseEvent &event )
{
    if (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK )
    {

        int x = event.m_x ;
        int y = event.m_y ;

        MacClientToRootWindow( &x , &y ) ;

        ControlHandle   control ;
        Point       localwhere ;
        SInt16      controlpart ;
        WindowRef   window = (WindowRef) MacGetRootWindow() ;

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

        controlpart = ::FindControl( localwhere , window , &control ) ;
        {
            if ( control && ::IsControlActive( control ) )
            {
                {
                    controlpart = ::HandleControlClick( control , localwhere , modifiers , (ControlActionUPP) -1 ) ;
                    wxTheApp->s_lastMouseDown = 0 ;
                    if ( control && controlpart != kControlNoPart ) // otherwise we will get the event twice
                    {
                        MacHandleControlClick( (WXWidget) control , controlpart , false /* not down anymore */ ) ;
                    }
                }
            }
        }
    }
}

#endif // wxUSE_TOOLBAR
