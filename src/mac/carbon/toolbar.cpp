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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "toolbar.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_TOOLBAR

#include "wx/wx.h"
#include "wx/bitmap.h"
#include "wx/toolbar.h"

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
    EVT_PAINT( wxToolBar::OnPaint ) 
END_EVENT_TABLE()

#include "wx/mac/uma.h"
#include "wx/geometry.h"

#ifdef __WXMAC_OSX__
const short kwxMacToolBarToolDefaultWidth = 16 ;
const short kwxMacToolBarToolDefaultHeight = 16 ;
const short kwxMacToolBarTopMargin = 4 ; // 1 ;      // used to be 4
const short kwxMacToolBarLeftMargin =  4 ; //1 ;     // used to be 4
const short kwxMacToolBorder = 0 ;                // used to be 0
const short kwxMacToolSpacing = 6 ; // 2 ;               // used to be 6
#else
const short kwxMacToolBarToolDefaultWidth = 24 ;
const short kwxMacToolBarToolDefaultHeight = 22 ;
const short kwxMacToolBarTopMargin = 2 ;
const short kwxMacToolBarLeftMargin = 2 ;
const short kwxMacToolBorder = 4 ;
const short kwxMacToolSpacing = 0 ;
#endif

#pragma mark -
#pragma mark Tool Implementation


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// We have a dual implementation for each tool, ControlRef and HIToolbarItemRef

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
        if (control != NULL)
            SetControlHandle( (ControlRef) control->GetHandle() ) ;
    }
    
    ~wxToolBarTool()
    {
    	ClearControl() ;
        if ( m_controlHandle )
            DisposeControl( m_controlHandle ) ;
#if wxMAC_USE_NATIVE_TOOLBAR
        if ( m_toolbarItemRef )
            CFRelease( m_toolbarItemRef ) ;
#endif
    }
    
    WXWidget GetControlHandle() 
    { 
        return (WXWidget) m_controlHandle ; 
    }
    
    void SetControlHandle( ControlRef handle ) 
    { 
    	m_controlHandle = handle ; 
    }

    void SetPosition( const wxPoint& position ) ;
    
    void ClearControl() 
    { 
    	m_control = NULL ; 
#if wxMAC_USE_NATIVE_TOOLBAR
        m_toolbarItemRef = NULL ; 
#endif   
    }
    
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
            // separator size
            wxSize sz = GetToolBar()->GetToolSize() ;
            if ( GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL )
                sz.y /= 4 ;
            else
                sz.x /= 4 ;
            return sz ;
        }
    }
    wxPoint GetPosition() const
    {
        return wxPoint(m_x, m_y);
    }    
    bool DoEnable( bool enable ) ;
    
    void UpdateToggleImage( bool toggle ) ;
    
#if wxMAC_USE_NATIVE_TOOLBAR    
    void SetToolbarItemRef( HIToolbarItemRef ref ) 
    { 
        if ( m_controlHandle )
            HideControl( m_controlHandle ) ;
        if ( m_toolbarItemRef )
            CFRelease( m_toolbarItemRef ) ;
        m_toolbarItemRef = ref ; 
        if ( m_toolbarItemRef )
        {
            HIToolbarItemSetHelpText(
                m_toolbarItemRef, wxMacCFStringHolder( GetShortHelp() , GetToolBar()->GetFont().GetEncoding() ) ,
                wxMacCFStringHolder( GetLongHelp() , GetToolBar()->GetFont().GetEncoding() ) ) ;
        }
    }
    HIToolbarItemRef GetToolbarItemRef() const 
    { 
        return m_toolbarItemRef ; 
    }
#endif

private :
    void Init() 
    {
        m_controlHandle = NULL ;
#if wxMAC_USE_NATIVE_TOOLBAR
        m_toolbarItemRef = NULL ;
#endif
    }
    ControlRef m_controlHandle ;
#if wxMAC_USE_NATIVE_TOOLBAR
    HIToolbarItemRef m_toolbarItemRef ;
#endif
    wxCoord     m_x;
    wxCoord     m_y;
};

static const EventTypeSpec eventList[] =
{
	{ kEventClassControl , kEventControlHit } ,
#ifdef __WXMAC_OSX__
	{ kEventClassControl , kEventControlHitTest } ,
#endif
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
                wxToolBar   *tbar = tbartool != NULL ? ( wxToolBar * ) ( tbartool->GetToolBar() ) : NULL ;
                if ((tbartool != NULL) && tbartool->CanBeToggled() )
                {
                    bool    shouldToggle;
#ifdef __WXMAC_OSX__
                    shouldToggle = !tbartool->IsToggled();
#else
                    shouldToggle = ( GetControl32BitValue((ControlRef) tbartool->GetControlHandle()) != 0 );
#endif
                    tbar->ToggleTool( tbartool->GetId(), shouldToggle );
                }
                if (tbartool != NULL)
                    tbar->OnLeftClick( tbartool->GetId(), tbartool->IsToggled() );
                result = noErr; 
            }
            break ;

#ifdef __WXMAC_OSX__
        case kEventControlHitTest :
            {
                HIPoint pt = cEvent.GetParameter<HIPoint>(kEventParamMouseLocation) ;
                HIRect rect ;
                HIViewGetBounds( controlRef , &rect ) ;
                
                ControlPartCode pc = kControlNoPart ;
                if ( CGRectContainsPoint( rect , pt ) )
                    pc = kControlIconPart ;
                cEvent.SetParameter( kEventParamControlPart , typeControlPartCode, pc ) ;
                result = noErr ;
            }
            break ;
#endif

        default :
            break ;
    }
    return result ;
}

static pascal OSStatus wxMacToolBarToolEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
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

#if wxMAC_USE_NATIVE_TOOLBAR

//
// native toolbar
//

static const EventTypeSpec toolBarEventList[] =
{
	{ kEventClassToolbarItem , kEventToolbarItemPerformAction } ,
} ;

static pascal OSStatus wxMacToolBarCommandEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    
    switch( GetEventKind( event ) )
    {
        case kEventToolbarItemPerformAction :
            {
                wxToolBarTool* tbartool = (wxToolBarTool*) data ;
                if ( tbartool != NULL )
                {
                    int         toolID = tbartool->GetId();
                    wxToolBar   *tbar = ( wxToolBar * ) ( tbartool->GetToolBar() );
                    if ( tbartool->CanBeToggled() )
                    {
                        tbar->ToggleTool(toolID, !tbartool->IsToggled() );
                    }
                    tbar->OnLeftClick( toolID , tbartool -> IsToggled() ) ;
                    result = noErr; 
                }
            }
            break ;

        default :
            break ;
    }
    return result ;
}

static pascal OSStatus wxMacToolBarEventHandler( EventHandlerCallRef handler, EventRef event, void *data )
{
    OSStatus result = eventNotHandledErr ;
    switch( GetEventClass( event ) )
    {
        case kEventClassToolbarItem :
            result = wxMacToolBarCommandEventHandler( handler, event, data ) ;
            break ;

        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacToolBarEventHandler )

#endif

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
#if wxMAC_USE_NATIVE_TOOLBAR    
        if ( m_toolbarItemRef )
            HIToolbarItemSetEnabled( m_toolbarItemRef , enable ) ;
#endif

        if ( m_controlHandle )
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
    }
    return true ;
}

void wxToolBarTool::SetPosition(const wxPoint& position)
{
    m_x = position.x;
    m_y = position.y;

    int x , y ;
    x = y = 0 ;
    int mac_x = position.x ;
    int mac_y = position.y ;

    if ( ! GetToolBar()->MacGetTopLevelWindow()->MacUsesCompositing() )
    {
        GetToolBar()->MacWindowToRootWindow( &x , &y ) ;
        mac_x += x;
        mac_y += y;
    }

    if ( IsButton() )
    {
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
    else
    {
        // separator 
#ifdef __WXMAC_OSX__
        Rect contrlRect ;       
        GetControlBounds( m_controlHandle , &contrlRect ) ; 
        int former_mac_x = contrlRect.left ;
        int former_mac_y = contrlRect.top ;
        
        if ( mac_x != former_mac_x || mac_y != former_mac_y )
        {
            UMAMoveControl( m_controlHandle , mac_x , mac_y ) ;
        }
#endif
    }
}

void wxToolBarTool::UpdateToggleImage( bool toggle ) 
{
#if wxMAC_USE_NATIVE_TOOLBAR    

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
#define kHIToolbarItemSelected   (1 << 7)
#endif

    // FIXME: this should be a OSX v10.4 runtime check
    if (m_toolbarItemRef != NULL)
    {
        OptionBits addAttrs, removeAttrs;
        OSStatus result;

        if (toggle)
        {
            addAttrs = kHIToolbarItemSelected;
            removeAttrs = kHIToolbarItemNoAttributes;
        }
        else
        {
            addAttrs = kHIToolbarItemNoAttributes;
            removeAttrs = kHIToolbarItemSelected;
        }

        result = HIToolbarItemChangeAttributes( m_toolbarItemRef, addAttrs, removeAttrs );
    }
#endif

#ifdef __WXMAC_OSX__
    if ( toggle )
    {
        int w = m_bmpNormal.GetWidth() ;
        int h = m_bmpNormal.GetHeight() ;
        wxBitmap bmp( w , h ) ;
        wxMemoryDC dc ;
        dc.SelectObject( bmp ) ;
        dc.SetPen( wxNullPen ) ;
        dc.SetBackground( *wxWHITE ) ;
        dc.DrawRectangle( 0 , 0 , w , h ) ;
        dc.DrawBitmap( m_bmpNormal , 0 , 0 , true) ;
        dc.SelectObject( wxNullBitmap ) ;
        ControlButtonContentInfo info ;
        wxMacCreateBitmapButton( &info , bmp ) ;
    	SetControlData( m_controlHandle , 0, kControlIconContentTag, sizeof( info ),
    			(Ptr)&info );
        wxMacReleaseBitmapButton( &info ) ;
    }
    else
    {
        ControlButtonContentInfo info ;
        wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
    	SetControlData( m_controlHandle , 0, kControlIconContentTag, sizeof( info ),
    			(Ptr)&info );
        wxMacReleaseBitmapButton( &info ) ;
    }

    IconTransformType transform = toggle ? kTransformSelected : kTransformNone ;
	SetControlData( m_controlHandle, 0, kControlIconTransformTag, sizeof( transform ),
			(Ptr)&transform );
    HIViewSetNeedsDisplay( m_controlHandle , true ) ;

#else
    ::SetControl32BitValue( m_controlHandle , toggle ) ;
#endif
}

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
}

#pragma mark -
#pragma mark Toolbar Implementation

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
#if wxMAC_USE_NATIVE_TOOLBAR
    m_macHIToolbarRef = NULL ;
    m_macUsesNativeToolbar = false ;
#endif
}

// also for the toolbar we have the dual implementation:
// only when MacInstallNativeToolbar is called is the native toolbar set as the window toolbar
//
bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    if ( !wxToolBarBase::Create( parent , id , pos , size , style ) )
        return false ;

    OSStatus err = 0;

#if wxMAC_USE_NATIVE_TOOLBAR
    wxString labelStr;
    labelStr.Format(wxT("%xd"), (int)this);
    err = HIToolbarCreate( wxMacCFStringHolder(labelStr, wxFont::GetDefaultEncoding() ) , 0 ,
                    (HIToolbarRef*) &m_macHIToolbarRef  );

    if (m_macHIToolbarRef != NULL)
    {	
        HIToolbarDisplayMode mode = kHIToolbarDisplayModeDefault ;
        HIToolbarDisplaySize displaySize = kHIToolbarDisplaySizeSmall ;

        if ( style & wxTB_NOICONS )
            mode = kHIToolbarDisplayModeLabelOnly ;
        else if ( style & wxTB_TEXT )
            mode = kHIToolbarDisplayModeIconAndLabel ;
        else
            mode = kHIToolbarDisplayModeIconOnly ;

        displaySize = kHIToolbarDisplaySizeDefault ;

        HIToolbarSetDisplayMode( (HIToolbarRef) m_macHIToolbarRef , mode ) ;
        HIToolbarSetDisplaySize( (HIToolbarRef) m_macHIToolbarRef , displaySize ) ;
    }
#endif

    return (err == 0);
}

wxToolBar::~wxToolBar()
{    
#if wxMAC_USE_NATIVE_TOOLBAR
    if ( m_macHIToolbarRef )
    {
        // if this is the installed toolbar, then deinstall it
        if (m_macUsesNativeToolbar)
            MacInstallNativeToolbar( false );

        CFRelease( (HIToolbarRef) m_macHIToolbarRef );
        m_macHIToolbarRef = NULL;
    }
#endif
}

bool wxToolBar::Show( bool show )
{
    bool bResult, ownToolbarInstalled = false;
    WindowRef tlw = MAC_WXHWND(MacGetTopLevelWindowRef());
    
    bResult = (tlw != NULL);
    if (bResult)
    {
#if wxMAC_USE_NATIVE_TOOLBAR
        MacTopLevelHasNativeToolbar( &ownToolbarInstalled );
        if (ownToolbarInstalled)
        {
            bResult = (HIViewIsVisible( (HIViewRef)m_macHIToolbarRef ) != show);
            ShowHideWindowToolbar( tlw, show, false );
        }
        else
#endif
            bResult = wxToolBarBase::Show( show );
    }
        
    return bResult;
}

bool wxToolBar::IsShown() const
{
    bool bResult;

#if wxMAC_USE_NATIVE_TOOLBAR
    bool ownToolbarInstalled ;
    MacTopLevelHasNativeToolbar( &ownToolbarInstalled );
    if (ownToolbarInstalled)
        bResult = HIViewIsVisible( (HIViewRef)m_macHIToolbarRef );
    else
#endif
        bResult = wxToolBarBase::IsShown();
    
    return bResult;
}

void wxToolBar::DoGetSize( int *width, int *height ) const
{
#if wxMAC_USE_NATIVE_TOOLBAR
    Rect    boundsR;
    bool    ownToolbarInstalled;
    
    MacTopLevelHasNativeToolbar( &ownToolbarInstalled );
    if ( ownToolbarInstalled )
    {
        // TODO is this really a control ?
        GetControlBounds( (ControlRef) m_macHIToolbarRef, &boundsR );
        if ( width != NULL )
            *width = boundsR.right - boundsR.left;
        if ( height != NULL )
            *height = boundsR.bottom - boundsR.top;
    }
    else
#endif
        wxToolBarBase::DoGetSize( width, height );
}

void wxToolBar::SetWindowStyleFlag( long style ) 
{
    wxToolBarBase::SetWindowStyleFlag( style );
#if wxMAC_USE_NATIVE_TOOLBAR
    if (m_macHIToolbarRef != NULL)
    {
        HIToolbarDisplayMode mode = kHIToolbarDisplayModeDefault;

        if ( style & wxTB_NOICONS )
            mode = kHIToolbarDisplayModeLabelOnly;
        else if ( style & wxTB_TEXT )
            mode = kHIToolbarDisplayModeIconAndLabel;
        else
            mode = kHIToolbarDisplayModeIconOnly;
 
       HIToolbarSetDisplayMode( (HIToolbarRef) m_macHIToolbarRef, mode );
    }
#endif
}

#if wxMAC_USE_NATIVE_TOOLBAR
bool wxToolBar::MacWantsNativeToolbar()
{
    return m_macUsesNativeToolbar;
}

bool wxToolBar::MacTopLevelHasNativeToolbar(bool *ownToolbarInstalled) const
{
    bool bResultV = false;

    if (ownToolbarInstalled != NULL)
        *ownToolbarInstalled = false;

    WindowRef tlw = MAC_WXHWND(MacGetTopLevelWindowRef());
    if (tlw != NULL)
    {
        HIToolbarRef curToolbarRef = NULL;
        OSStatus err = GetWindowToolbar( tlw, &curToolbarRef );
        bResultV = ((err == 0) && (curToolbarRef != NULL));
        if (bResultV && (ownToolbarInstalled != NULL))
            *ownToolbarInstalled = (curToolbarRef == m_macHIToolbarRef);
    }

    return bResultV;
}

bool wxToolBar::MacInstallNativeToolbar(bool usesNative) 
{
    bool	bResult = false;

    WindowRef tlw = MAC_WXHWND(MacGetTopLevelWindowRef());
    if (tlw == NULL)
        return bResult;

    if (usesNative && (m_macHIToolbarRef == NULL))
        return bResult;
 
    if (usesNative && ((GetWindowStyleFlag() & wxTB_VERTICAL) != 0))
        return bResult;
 
    // check the existing toolbar
    HIToolbarRef curToolbarRef = NULL;
    OSStatus err = GetWindowToolbar( tlw, &curToolbarRef );
    if (err != 0)
        curToolbarRef = NULL;

    m_macUsesNativeToolbar = usesNative;

    if (m_macUsesNativeToolbar)
    {
        // only install toolbar if there isn't one installed already
        if (curToolbarRef == NULL)
        {
            bResult = true;

            SetWindowToolbar( tlw, (HIToolbarRef) m_macHIToolbarRef );
            ShowHideWindowToolbar( tlw, true, false );
            ChangeWindowAttributes( tlw, kWindowToolbarButtonAttribute, 0 );
            SetAutomaticControlDragTrackingEnabledForWindow( tlw, true );
            
            // FIXME: which is best, which is necessary?
            //
            // m_peer->SetVisibility( false, true );
            //
            //
            Rect r = { 0 , 0 , 0 , 0 };
            //
            //
            m_peer->SetRect( &r );
            //
            // FIXME: which is best, which is necessary?
            //
            SetSize( wxSIZE_AUTO_WIDTH, 0 );
            //
            m_peer->SetVisibility( false, true );
            wxToolBarBase::Show( false );
        }
    }
    else
    {
        // only deinstall toolbar if this is the installed one
        if (m_macHIToolbarRef == curToolbarRef)
        {
            bResult = true;

            ShowHideWindowToolbar( tlw, false, false );
            ChangeWindowAttributes( tlw, 0 , kWindowToolbarButtonAttribute );
            SetWindowToolbar( tlw, NULL );
            
            // FIXME: which is best, which is necessary?
            m_peer->SetVisibility( true, true );
            
            //
            // wxToolBarBase::Show( true );
            //
        }
    }

    if (bResult)
        InvalidateBestSize();

// wxLogDebug( wxT("    --> [%lx] - result [%s]"), (long)this, bResult ? wxT("T") : wxT("F") );
    return bResult;
}
#endif

bool wxToolBar::Realize()
{
    if (m_tools.GetCount() == 0)
        return false;

    int x = m_xMargin + kwxMacToolBarLeftMargin;
    int y = m_yMargin + kwxMacToolBarTopMargin;
    
    int tw, th;
    GetSize( &tw, &th );
    
    int maxWidth = 0;
    int maxHeight = 0;
    
    int maxToolWidth = 0;
    int maxToolHeight = 0;
    
    // find the maximum tool width and height
    wxToolBarToolsList::compatibility_iterator  node = m_tools.GetFirst();
    while ( node != NULL )
    {
        wxToolBarTool   *tool = (wxToolBarTool *) node->GetData();
        
        if ( tool != NULL )
        {
            wxSize  sz = tool->GetSize();
            
            if ( sz.x > maxToolWidth )
                maxToolWidth = sz.x;
            if ( sz.y > maxToolHeight )
                maxToolHeight = sz.y;
        }
        
        node = node->GetNext();
    }
    
    bool    lastIsRadio = false;
    bool    curIsRadio = false;
    bool    setChoiceInGroup = false;
    
    node = m_tools.GetFirst();
    while ( node != NULL )
    {
        wxToolBarTool   *tool = (wxToolBarTool *) node->GetData();
        
        if ( tool == NULL )
        {
            node = node->GetNext();
            continue;
        }
        
        // set tool position
        // for the moment just perform a single row/column alignment
        wxSize  cursize = tool->GetSize();
        if ( x + cursize.x > maxWidth )
            maxWidth = x + cursize.x;
        if ( y + cursize.y > maxHeight )
            maxHeight = y + cursize.y;
        
        if ( GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            int x1 = x + ( maxToolWidth - cursize.x ) / 2;
            tool->SetPosition( wxPoint(x1, y) );
        }
        else
        {
            int y1 = y + ( maxToolHeight - cursize.y ) / 2;
            tool->SetPosition( wxPoint(x, y1) );
        }
        
        // update the item positioning state
        if ( GetWindowStyleFlag() & wxTB_VERTICAL )
            y += cursize.y + kwxMacToolSpacing;
        else
            x += cursize.x + kwxMacToolSpacing;
        
#if wxMAC_USE_NATIVE_TOOLBAR
        // install in native HIToolbar
        if ( m_macHIToolbarRef != NULL )
        {
            HIToolbarItemRef    hiItemRef = tool->GetToolbarItemRef();
            if ( hiItemRef != NULL )
            {
                OSStatus    result = HIToolbarAppendItem( (HIToolbarRef) m_macHIToolbarRef, hiItemRef );
                if ( result == 0 )
                {
                    InstallEventHandler( HIObjectGetEventTarget(hiItemRef), GetwxMacToolBarEventHandlerUPP(),
                                         GetEventTypeCount(toolBarEventList), toolBarEventList, tool, NULL );
                }
            }
        }
#endif
        
        // update radio button (and group) state
        lastIsRadio = curIsRadio;
        curIsRadio = ( tool->IsButton() && (tool->GetKind() == wxITEM_RADIO) );
        
        if ( !curIsRadio )
        {
            if ( tool->IsToggled() )
                DoToggleTool( tool, true );
            
            setChoiceInGroup = false;
        }
        else
        {
            if ( !lastIsRadio )
            {
                if ( tool->Toggle(true) )
                {
                    DoToggleTool( tool, true );
                    setChoiceInGroup = true;
                }
            }
            else if ( tool->IsToggled() )
            {
                if ( tool->IsToggled() )
                    DoToggleTool( tool, true );
                
                wxToolBarToolsList::compatibility_iterator  nodePrev = node->GetPrevious();
                while ( nodePrev != NULL )
                {
                    wxToolBarToolBase   *toggleTool = nodePrev->GetData();
                    if ( (toggleTool == NULL) || !toggleTool->IsButton() || (toggleTool->GetKind() != wxITEM_RADIO) )
                        break;
                    
                    if ( toggleTool->Toggle(false) )
                        DoToggleTool( toggleTool, false );
                    
                    nodePrev = nodePrev->GetPrevious();
                }
            }
        }
        
        node = node->GetNext();
    }
    
    if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
    {
        // if not set yet, only one row
        if ( m_maxRows <= 0 )
            SetRows( 1 );
        
        m_minWidth = maxWidth;
        maxWidth = tw;
        maxHeight += m_yMargin + kwxMacToolBarTopMargin;
        m_minHeight = m_maxHeight = maxHeight;
    }
    else
    {
        // if not set yet, have one column
        if ( (GetToolsCount() > 0) && (m_maxRows <= 0) )
            SetRows( GetToolsCount() );
        
        m_minHeight = maxHeight;
        maxHeight = th;
        maxWidth += m_xMargin + kwxMacToolBarLeftMargin;
        m_minWidth = m_maxWidth = maxWidth;
    }

#if 0    
    // FIXME: should this be OSX-only?
    {
        bool wantNativeToolbar, ownToolbarInstalled;

        // attempt to install the native toolbar
        wantNativeToolbar = ((GetWindowStyleFlag() & wxTB_VERTICAL) == 0);
        MacInstallNativeToolbar( wantNativeToolbar );
        (void)MacTopLevelHasNativeToolbar( &ownToolbarInstalled );
        if (!ownToolbarInstalled)
        {
           SetSize( maxWidth, maxHeight );
           InvalidateBestSize();
        }
    }
#else   
    SetSize( maxWidth, maxHeight );
    InvalidateBestSize();
#endif
    return true;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x + kwxMacToolBorder;
    m_defaultHeight = size.y + kwxMacToolBorder;
    
#if wxMAC_USE_NATIVE_TOOLBAR
    if (m_macHIToolbarRef != NULL)
    {
        int maxs = wxMax( size.x, size.y );
        // TODO CHECK
        HIToolbarDisplaySize sizeSpec = ((maxs > 16) ? kHIToolbarDisplaySizeNormal : kHIToolbarDisplaySizeSmall);
        HIToolbarSetDisplaySize( (HIToolbarRef) m_macHIToolbarRef, sizeSpec );
    }
#endif
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    return wxSize(m_defaultWidth + kwxMacToolBorder, m_defaultHeight + kwxMacToolBorder);
}

void wxToolBar::SetRows(int nRows)
{
    // avoid resizing the frame uselessly
    if ( nRows != m_maxRows )
    {
        m_maxRows = nRows;
    }
}

void wxToolBar::MacSuperChangedPosition() 
{
    wxWindow::MacSuperChangedPosition();
#if wxMAC_USE_NATIVE_TOOLBAR
    if (! m_macUsesNativeToolbar )
#endif
    {
        Realize();
    }
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
    while ( node != NULL )
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData() ;

        if (tool != NULL)
        {
            wxRect2DInt r( tool->GetPosition(), tool->GetSize() );
            if ( r.Contains( wxPoint( x, y ) ) )
                return tool;
        }

        node = node->GetNext();
    }

    return (wxToolBarToolBase *)NULL;
}

wxString wxToolBar::MacGetToolTipString( wxPoint &pt )
{
    wxToolBarToolBase* tool = FindToolForPosition( pt.x , pt.y ) ;
    if ( tool != NULL )
        return tool->GetShortHelp() ;

    return wxEmptyString ;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *t, bool enable)
{
    if ( t != NULL )
        ((wxToolBarTool*)t)->DoEnable( enable ) ;
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *t, bool toggle)
{
    wxToolBarTool *tool = (wxToolBarTool *)t;
    if ( ( tool != NULL ) && tool->IsButton() )
        tool->UpdateToggleImage( toggle );
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *toolBase)
{
    wxToolBarTool* tool = wx_static_cast( wxToolBarTool* , toolBase );
    if (tool == NULL)
        return false;

    WindowRef window = (WindowRef) MacGetTopLevelWindowRef();
    wxSize toolSize = GetToolSize();
    Rect toolrect = { 0, 0 , toolSize.y , toolSize.x };
    ControlRef controlHandle = NULL;
    OSStatus err = 0;

    switch (tool->GetStyle())
    {
        case wxTOOL_STYLE_SEPARATOR :
            {
                wxASSERT( tool->GetControlHandle() == NULL );
                toolSize.x /= 4;
                toolSize.y /= 4;
                if ( GetWindowStyleFlag() & wxTB_VERTICAL )
                    toolrect.bottom = toolSize.y;
                else
                    toolrect.right = toolSize.x;

        #ifdef __WXMAC_OSX__
                // in flat style we need a visual separator
		#if wxMAC_USE_NATIVE_TOOLBAR
                HIToolbarItemRef item;
                err = HIToolbarItemCreate( kHIToolbarSeparatorIdentifier, kHIToolbarItemCantBeRemoved | kHIToolbarItemIsSeparator | kHIToolbarItemAllowDuplicates, &item );
                if (err == noErr)
                    tool->SetToolbarItemRef( item );
		#endif
                CreateSeparatorControl( window, &toolrect, &controlHandle );
                tool->SetControlHandle( controlHandle );
        #endif
            }
            break;

        case wxTOOL_STYLE_BUTTON :
            {
                wxASSERT( tool->GetControlHandle() == NULL ) ;
                ControlButtonContentInfo info ;
                wxMacCreateBitmapButton( &info , tool->GetNormalBitmap()  , kControlContentIconRef ) ;
                
        		if ( UMAGetSystemVersion() >= 0x1000)
	                CreateIconControl( window , &toolrect , &info , false , &controlHandle ) ;
        		else
        		{
			        SInt16 behaviour = kControlBehaviorOffsetContents ;
			        if ( tool->CanBeToggled() )
			            behaviour += kControlBehaviorToggles ;
			        CreateBevelButtonControl( window , &toolrect , CFSTR("") , kControlBevelButtonNormalBevel , behaviour , &info , 
			            0 , 0 , 0 , &controlHandle ) ;
        		}                    

#if wxMAC_USE_NATIVE_TOOLBAR
                HIToolbarItemRef item ;
                wxString	labelStr;
                labelStr.Format(wxT("%xd"), (int)tool);
                err = HIToolbarItemCreate(
                    wxMacCFStringHolder(labelStr, wxFont::GetDefaultEncoding()),
                    kHIToolbarItemCantBeRemoved | kHIToolbarItemAnchoredLeft | kHIToolbarItemAllowDuplicates, &item );
                if (err  == noErr)
                {
                    HIToolbarItemSetLabel( item, wxMacCFStringHolder(tool->GetLabel(), m_font.GetEncoding()) );
                    HIToolbarItemSetIconRef( item, info.u.iconRef );
                    HIToolbarItemSetCommandID( item, tool->GetId() );
                    tool->SetToolbarItemRef( item );
                }
#endif

                wxMacReleaseBitmapButton( &info ) ;
                /*
                SetBevelButtonTextPlacement( m_controlHandle , kControlBevelButtonPlaceBelowGraphic ) ;
                UMASetControlTitle(  m_controlHandle , label , wxFont::GetDefaultEncoding() ) ;
                */
                
                InstallControlEventHandler( (ControlRef) controlHandle, GetwxMacToolBarToolEventHandlerUPP(),
                    GetEventTypeCount(eventList), eventList, tool, NULL );

                tool->SetControlHandle( controlHandle );
            }
            break;

        case wxTOOL_STYLE_CONTROL :
            wxASSERT( tool->GetControl() != NULL );
#if 0 // wxMAC_USE_NATIVE_TOOLBAR
            // FIXME: doesn't work yet...
            {
                HIToolbarItemRef    item;
                wxString            labelStr;
                labelStr.Format( wxT("%xd"), (int) tool );
                result = HIToolbarItemCreate( wxMacCFStringHolder(labelStr, wxFont::GetDefaultEncoding()),
                                              kHIToolbarItemCantBeRemoved | kHIToolbarItemAnchoredLeft | kHIToolbarItemAllowDuplicates,
                                              &item );
                if ( result == 0 )
                {
                    HIToolbarItemSetLabel( item, wxMacCFStringHolder(tool->GetLabel(), m_font.GetEncoding()) );
                    HIToolbarItemSetCommandID( item, tool->GetId() );
                    tool->SetToolbarItemRef( item );
                    
                    controlHandle = ( ControlRef ) tool->GetControlHandle();
                    wxASSERT_MSG( controlHandle != NULL, wxT("NULL tool control") );
                    
                    // FIXME: is this necessary ??
                    ::GetControlBounds( controlHandle, &toolrect );
                    UMAMoveControl( controlHandle, -toolrect.left, -toolrect.top );
                    
                    // FIXME: is this necessary ??
                    InstallControlEventHandler( controlHandle, GetwxMacToolBarToolEventHandlerUPP(),
                                                GetEventTypeCount(eventList), eventList, tool, NULL );
                }
            }
                
#else
                // FIXME: right now there's nothing to do here
#endif
                break;

        default :
            break;
    }
 
    if ( err == 0 )
    {
        if ( controlHandle )
        {
            ControlRef container = (ControlRef) GetHandle();
            wxASSERT_MSG( container != NULL, wxT("No valid mac container control") );

            UMAShowControl( controlHandle );
            ::EmbedControl( controlHandle, container );
        }

        if ( tool->CanBeToggled() && tool->IsToggled() )
            tool->UpdateToggleImage( true );

        // nothing special to do here - we relayout in Realize() later
        tool->Attach(this);
        InvalidateBestSize();
    }
    else
    {
        wxString    errMsg;
        errMsg.Format( wxT("wxToolBar::DoInsertTool - failure [%ld]"), (long) err );
        wxASSERT_MSG( false, errMsg.c_str() );
    }
    
    return( err == 0 );
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    wxFAIL_MSG( _T("not implemented") );
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *toolbase)
{
    wxToolBarTool* tool = wx_static_cast( wxToolBarTool* , toolbase ) ;
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

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_CONTROL:
            {
                tool->GetControl()->Destroy();
                tool->ClearControl() ;
            }
            break;

        case wxTOOL_STYLE_BUTTON:
        case wxTOOL_STYLE_SEPARATOR:
            if ( tool->GetControlHandle() )
            {
                DisposeControl( (ControlRef) tool->GetControlHandle() ) ;
#if wxMAC_USE_NATIVE_TOOLBAR
				if ( tool->GetToolbarItemRef() )
                	CFRelease( tool->GetToolbarItemRef() ) ;
#endif
                tool->ClearControl() ;
            }
            break;

        default:
            break;
    }

    // and finally reposition all the controls after this one
    
    for ( /* node -> first after deleted */ ; node; node = node->GetNext() )
    {
        wxToolBarTool *tool2 = (wxToolBarTool*) node->GetData();
        wxPoint pt = tool2->GetPosition() ;

        if ( GetWindowStyleFlag() & wxTB_VERTICAL )
            pt.y -= sz.y ;
        else
            pt.x -= sz.x ;

        tool2->SetPosition( pt ) ;
    }
    
    InvalidateBestSize();
    return true ;
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
#if wxMAC_USE_NATIVE_TOOLBAR
    if ( m_macUsesNativeToolbar )
    {
        event.Skip(true) ;
        return ;
    }
#endif
    
    wxPaintDC dc(this) ;

    int w, h ;
    GetSize( &w , &h ) ;
#if wxMAC_USE_CORE_GRAPHICS && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    if ( !MacGetTopLevelWindow()->MacGetMetalAppearance() )
    {
        if ( UMAGetSystemVersion() >= 0x1030 )
        {
            HIThemePlacardDrawInfo info ;
            memset( &info, 0 , sizeof( info ) ) ;
            info.version = 0 ;
            info.state = IsEnabled() ? kThemeStateActive : kThemeStateInactive ;
            
            CGContextRef cgContext = (CGContextRef) MacGetCGContextRef() ;
            HIRect rect = CGRectMake( 0 , 0 , w , h ) ;
            HIThemeDrawPlacard( &rect , & info , cgContext, kHIThemeOrientationNormal) ;
        }
    }
    else
    {
        // leave the background as it is (striped or metal)
    }
#else
    wxMacPortSetter helper(&dc) ;
    
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
#endif

    event.Skip() ;
}

#endif // wxUSE_TOOLBAR

