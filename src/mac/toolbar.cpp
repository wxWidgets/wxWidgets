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
#include "wx/bitmap.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
    EVT_MOUSE_EVENTS( wxToolBar::OnMouse ) 
    EVT_PAINT( wxToolBar::OnPaint ) 
END_EVENT_TABLE()
#endif

#include "wx/mac/uma.h"

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
                  const wxString& longHelp)
        : wxToolBarToolBase(tbar, id, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp)
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

    int     m_index ;
private:
    size_t m_nSepCount;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

const short kwxMacToolBarToolDefaultWidth = 24 ;
const short kwxMacToolBarToolDefaultHeight = 22 ;
const short kwxMacToolBarTopMargin = 2 ;
const short kwxMacToolBarLeftMargin = 2 ;


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

  if (id == -1)
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
  
  return TRUE;
}

wxToolBar::~wxToolBar()
{
    size_t index = 0 ;
    for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
    {
        // Delete the control as we get ghosts otherwise
        ::DisposeControl( (ControlHandle) m_macToolHandles[index] );
    }
    
    // we must refresh the frame size when the toolbar is deleted but the frame
    // is not - otherwise toolbar leaves a hole in the place it used to occupy
}

bool wxToolBar::Realize()
{
    if (m_tools.GetCount() == 0)
        return FALSE;

    Point localOrigin ;
    //    Rect clipRect ;
    WindowRef window = (WindowRef) MacGetRootWindow() ;
    //    wxWindow *win ;
    
    int lx , ly ;
    lx = ly = 0 ;
    MacWindowToRootWindow( &lx , &ly ) ;
    localOrigin.v = ly ;
    localOrigin.h = lx ;
    
//    GetParent()->MacGetPortParams( &localOrigin , &clipRect , &window , &win ) ;

    Rect toolbarrect = { localOrigin.v ,localOrigin.h , 
        m_height + localOrigin.v  , m_width + localOrigin.h} ;
    ControlFontStyleRec                controlstyle ;

    controlstyle.flags = kControlUseFontMask ;
    controlstyle.font = kControlFontSmallSystemFont ;
    
    wxwxToolBarToolsListNode *node = m_tools.GetFirst();
    int noButtons = 0;
    int x = 0 ;
    int y = 0 ;
    wxSize toolSize = GetToolSize() ;
    int tw, th;
    GetSize(& tw, & th);
    
    int maxWidth = 0 ;
    int maxHeight = 0 ;
    
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        
        if(  !tool->IsSeparator()  )
        {
            Rect toolrect = { toolbarrect.top + y + m_yMargin + kwxMacToolBarTopMargin,
                              toolbarrect.left + x + m_xMargin + kwxMacToolBarLeftMargin , 0 , 0 } ;
            toolrect.right = toolrect.left + toolSize.x ;
            toolrect.bottom = toolrect.top + toolSize.y ;
            
            ControlButtonContentInfo info ;
            wxMacCreateBitmapButton( &info , tool->GetNormalBitmap() ) ;
            ControlHandle m_macToolHandle ;
            
            SInt16 behaviour = kControlBehaviorOffsetContents ;
            if ( tool->CanBeToggled() )
                behaviour += kControlBehaviorToggles ;
            
            if ( info.contentType != kControlNoContent ) 
            {
                m_macToolHandle = ::NewControl( window , &toolrect , "\p" , false , 0 , 
                                                behaviour + info.contentType , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
                
                ::SetControlData( m_macToolHandle , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
            }
            else
            {
                m_macToolHandle = ::NewControl( window , &toolrect , "\p" , false , 0 , 
                                                behaviour  , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
            }
            UMAShowControl( m_macToolHandle ) ;
            m_macToolHandles.Add( m_macToolHandle ) ;
            tool->m_index = m_macToolHandles.Count() -1 ;
            if ( !tool->IsEnabled() )
            {
                UMADeactivateControl( m_macToolHandle ) ;
            }
            if ( tool->CanBeToggled() && tool->IsToggled() )
            {
                ::SetControl32BitValue( m_macToolHandle , 1 ) ;
            }
            else
            {
                ::SetControl32BitValue( m_macToolHandle , 0 ) ;
            }
            /*
              ::SetControlFontStyle( m_macToolHandle , &controlstyle ) ;
            */
            ControlHandle container = (ControlHandle) GetParent()->MacGetContainerForEmbedding() ;
            wxASSERT_MSG( container != NULL , "No valid mac container control" ) ;
            ::EmbedControl( m_macToolHandle , container ) ;
            
            if ( GetWindowStyleFlag() & wxTB_VERTICAL )
            {
                y += (int)toolSize.y;
            }
            else
            {
                x += (int)toolSize.x;
            }
            noButtons ++;
        }
        else
        {
            m_macToolHandles.Add( NULL ) ;
            
            if ( GetWindowStyleFlag() & wxTB_VERTICAL )
            {
                y += (int)toolSize.y / 4;
            }
            else
            {
                x += (int)toolSize.x / 4;
            }
        }
        if ( toolbarrect.left + x + m_xMargin + kwxMacToolBarLeftMargin - m_x - localOrigin.h > maxWidth) {
            maxWidth = toolbarrect.left + x + m_xMargin + kwxMacToolBarLeftMargin - m_x - localOrigin.h;
        }
        if (toolbarrect.top + y + m_yMargin + kwxMacToolBarTopMargin - m_y - localOrigin.v > maxHeight) {
            maxHeight = toolbarrect.top + y + m_yMargin + kwxMacToolBarTopMargin - m_y - localOrigin.v ;
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
        maxHeight += toolSize.y;
        maxHeight += m_yMargin + kwxMacToolBarTopMargin;
        m_maxHeight = maxHeight ;
    }
    else
    {
        if ( noButtons > 0 && m_maxRows == 0 )
        {
            // if not set yet, have one column
            SetRows(noButtons);
        }
        maxHeight = th ;
        maxWidth += toolSize.x;
        maxWidth += m_xMargin + kwxMacToolBarLeftMargin;
        m_maxWidth = maxWidth ;
    }
    
    SetSize(maxWidth, maxHeight);
    
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

void wxToolBar::MacHandleControlClick( WXWidget control , wxInt16 controlpart ) 
{
    size_t index = 0 ;
    for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
    {
        if ( m_macToolHandles[index] == (void*) control )
        {
            wxToolBarTool *tool = (wxToolBarTool *)m_tools.Item( index )->GetData();
            if ( tool->CanBeToggled() )
            {
                tool->Toggle( GetControl32BitValue( (ControlHandle) control ) ) ;
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

void wxToolBar::MacSuperChangedPosition() 
{
  if (m_tools.GetCount() > 0)
  {

    Point localOrigin ;
    //    Rect clipRect ;
    //    WindowRef window ;
    //    wxWindow *win ;
    int lx , ly ;
    lx = ly = 0 ;
    MacWindowToRootWindow( &lx , &ly ) ;
    localOrigin.v = ly ;
    localOrigin.h = lx ;
    
//    GetParent()->MacGetPortParams( &localOrigin , &clipRect , &window , &win ) ;

    Rect toolbarrect = { localOrigin.v ,localOrigin.h , 
        m_height + localOrigin.v  , m_width + localOrigin.h} ;
    ControlFontStyleRec     controlstyle ;

    controlstyle.flags = kControlUseFontMask ;
    controlstyle.font = kControlFontSmallSystemFont ;
    
    wxwxToolBarToolsListNode *node = m_tools.GetFirst();
    int noButtons = 0;
    int x = 0 ;
    wxSize toolSize = GetToolSize() ;
    int tw, th;
    GetSize(& tw, & th);
    
    int maxWidth = 0 ;
    int maxHeight = 0 ;
    int toolcount = 0 ;
    {
      WindowRef rootwindow = (WindowRef) MacGetRootWindow() ;
        while (node)
        {
            wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
            
            if(  !tool->IsSeparator()  )
            {
                Rect toolrect = { toolbarrect.top + m_yMargin + kwxMacToolBarTopMargin, toolbarrect.left + x + m_xMargin + kwxMacToolBarLeftMargin , 0 , 0 } ;
                toolrect.right = toolrect.left + toolSize.x ;
                toolrect.bottom = toolrect.top + toolSize.y ;
                            
                ControlHandle m_macToolHandle = (ControlHandle) m_macToolHandles[toolcount++] ;
                
                {
                    Rect contrlRect ;       
                    GetControlBounds( m_macToolHandle , &contrlRect ) ; 
                    int former_mac_x = contrlRect.left ;
                    int former_mac_y = contrlRect.top ;
                    int mac_x = toolrect.left ;
                    int mac_y = toolrect.top ;
                                
                    if ( mac_x != former_mac_x || mac_y != former_mac_y )
                    {
                        {
                            Rect inval = { former_mac_y , former_mac_x , former_mac_y + toolSize.y , former_mac_x + toolSize.y } ;
                            InvalWindowRect( rootwindow , &inval ) ;
                        }
                        UMAMoveControl( m_macToolHandle , mac_x , mac_y ) ;
                        {
                            Rect inval = { mac_y , mac_x , mac_y + toolSize.y , mac_x + toolSize.y } ;
                            InvalWindowRect( rootwindow , &inval ) ;
                        }
                    }
                }
                
                x += (int)toolSize.x;
                noButtons ++;
            }
            else
            {
                toolcount++ ;
                x += (int)toolSize.x / 4;
            }
            if ( toolbarrect.left + x + m_xMargin  + kwxMacToolBarLeftMargin- m_x - localOrigin.h > maxWidth)
                maxWidth = toolbarrect.left + x  + kwxMacToolBarLeftMargin+ m_xMargin - m_x - localOrigin.h;
            if (toolbarrect.top + m_yMargin  + kwxMacToolBarTopMargin - m_y - localOrigin.v > maxHeight)
                maxHeight = toolbarrect.top  + kwxMacToolBarTopMargin + m_yMargin - m_y - localOrigin.v ;

            node = node->GetNext();
        }
    }

     if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
     {
       if ( m_maxRows == 0 )
       {
           // if not set yet, only one row
           SetRows(1);
       }
        maxWidth = tw ; 
       maxHeight += toolSize.y;
        maxHeight += m_yMargin + kwxMacToolBarTopMargin;
        m_maxHeight = maxHeight ;
     }
     else
     {
       if ( noButtons > 0 && m_maxRows == 0 )
       {
           // if not set yet, have one column
           SetRows(noButtons);
       }
       maxHeight = th ;
       maxWidth += toolSize.x;
        maxWidth += m_xMargin + kwxMacToolBarLeftMargin;
        m_maxWidth = maxWidth ;
     }

     SetSize(maxWidth, maxHeight);
    }

    wxWindow::MacSuperChangedPosition() ;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    MacClientToRootWindow( &x , &y ) ;
    Point pt = { y ,x } ;

    size_t index = 0 ;
    for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
    {
        if ( m_macToolHandles[index] )
        {
            Rect bounds ;
            GetControlBounds((ControlHandle) m_macToolHandles[index], &bounds ) ;
            if ( PtInRect( pt , &bounds ) )
            {
                return  (wxToolBarTool*) (m_tools.Item( index )->GetData() ) ;
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
    if (!IsShown())
        return ;

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
    if (!IsShown())
        return ;

    wxToolBarTool *tool = (wxToolBarTool *)t;
    if ( tool->m_index < 0 )
        return ;
        
    ControlHandle control = (ControlHandle) m_macToolHandles[ tool->m_index ] ;
        ::SetControl32BitValue( control , toggle ) ;
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // nothing special to do here - we really create the toolbar buttons in
    // Realize() later
    tool->Attach(this);

    return TRUE;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    wxFAIL_MSG( _T("not implemented") );
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *WXUNUSED(tool))
{
    wxFAIL_MSG( _T("not implemented") );
    return TRUE ;
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this) ;
  wxMacPortSetter helper(&dc) ;
  
    Rect toolbarrect = { dc.YLOG2DEVMAC(0) , dc.XLOG2DEVMAC(0) , 
        dc.YLOG2DEVMAC(m_height) , dc.XLOG2DEVMAC(m_width) } ;
    UMADrawThemePlacard( &toolbarrect , IsEnabled() ? kThemeStateActive : kThemeStateInactive) ;
    {
        size_t index = 0 ;
        for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
        {
            if ( m_macToolHandles[index] )
            {
                UMADrawControl( (ControlHandle) m_macToolHandles[index] ) ;
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
                    if ( controlpart == kControlIndicatorPart && !UMAHasAppearance() )
                        controlpart = ::HandleControlClick( control , localwhere , modifiers , (ControlActionUPP) NULL ) ;
                    else
                        controlpart = ::HandleControlClick( control , localwhere , modifiers , (ControlActionUPP) -1 ) ;
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

