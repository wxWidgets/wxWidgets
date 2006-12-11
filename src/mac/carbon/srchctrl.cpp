///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/srchctrl.cpp
// Purpose:     implements mac carbon wxSearchCtrl
// Author:      Vince Harron
// Modified by:
// Created:     2006-02-19
// RCS-ID:      
// Copyright:   Vince Harron
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "srchctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif //WX_PRECOMP

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#if USE_NATIVE_SEARCH_CONTROL

#include "wx/mac/uma.h"
#include "wx/mac/carbon/private/mactext.h"

BEGIN_EVENT_TABLE(wxSearchCtrl, wxSearchCtrlBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase)

// ============================================================================
// wxMacSearchFieldControl
// ============================================================================

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2


static const EventTypeSpec eventList[] =
{
    { kEventClassSearchField, kEventSearchFieldCancelClicked } ,
    { kEventClassSearchField, kEventSearchFieldSearchClicked } ,
};

class wxMacSearchFieldControl : public wxMacUnicodeTextControl
{
public :
    wxMacSearchFieldControl( wxTextCtrl *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) : wxMacUnicodeTextControl( wxPeer )
    {
        Create( wxPeer, str, pos, size, style );
    }

    // search field options
    virtual void SetSearchButtonVisible( bool show );
    virtual bool GetSearchButtonVisible() const;

    virtual void SetCancelButtonVisible( bool show );
    virtual bool GetCancelButtonVisible() const;

    virtual void SetSearchMenu( wxMenu* menu );
    virtual wxMenu* GetSearchMenu() const;
protected :
    virtual void CreateControl( wxTextCtrl* peer, const Rect* bounds, CFStringRef crf );

private:
    wxMenu* m_menu;
} ;

void wxMacSearchFieldControl::CreateControl( wxTextCtrl* /*peer*/, const Rect* bounds, CFStringRef crf )
{
    OptionBits attributes = 0;
    if ( UMAGetSystemVersion() >= 0x1040 )
    {
		attributes = kHISearchFieldAttributesSearchIcon;
	}
    HIRect hibounds = { { bounds->left, bounds->top }, { bounds->right-bounds->left, bounds->bottom-bounds->top } };
    verify_noerr( HISearchFieldCreate( 
        &hibounds,
        attributes,
        0, // MenuRef
        CFSTR("Search"),
        &m_controlRef
        ) );
    HIViewSetVisible (m_controlRef, true); 
}

// search field options
void wxMacSearchFieldControl::SetSearchButtonVisible( bool show )
{
    if ( UMAGetSystemVersion() >= 0x1040 )
    {
        OptionBits set = 0;
        OptionBits clear = 0;
        if ( show )
        {
            set |= kHISearchFieldAttributesSearchIcon;
        }
        else
        {
            clear |= kHISearchFieldAttributesSearchIcon;
        }
        HISearchFieldChangeAttributes( m_controlRef, set, clear );
    }
}

bool wxMacSearchFieldControl::GetSearchButtonVisible() const
{ 
    OptionBits attributes = 0;
    verify_noerr( HISearchFieldGetAttributes( m_controlRef, &attributes ) );
    return ( attributes & kHISearchFieldAttributesSearchIcon ) != 0;
}

void wxMacSearchFieldControl::SetCancelButtonVisible( bool show )
{
    OptionBits set = 0;
    OptionBits clear = 0;
    if ( show )
    {
        set |= kHISearchFieldAttributesCancel;
    }
    else
    {
        clear |= kHISearchFieldAttributesCancel;
    }
    HISearchFieldChangeAttributes( m_controlRef, set, clear );
}

bool wxMacSearchFieldControl::GetCancelButtonVisible() const
{ 
    OptionBits attributes = 0;
    verify_noerr( HISearchFieldGetAttributes( m_controlRef, &attributes ) );
    return ( attributes & kHISearchFieldAttributesCancel ) != 0;
}

void wxMacSearchFieldControl::SetSearchMenu( wxMenu* menu )
{
    m_menu = menu;
    if ( m_menu )
    {
        verify_noerr( HISearchFieldSetSearchMenu( m_controlRef, MAC_WXHMENU(m_menu->GetHMenu()) ) );
    }
    else
    {
        verify_noerr( HISearchFieldSetSearchMenu( m_controlRef, 0 ) );
    }
}

wxMenu* wxMacSearchFieldControl::GetSearchMenu() const
{
    return m_menu;
}

#endif

// ============================================================================
// implementation
// ============================================================================

static pascal OSStatus wxMacSearchControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    wxSearchCtrl* thisWindow = (wxSearchCtrl*) data ;
    wxTextCtrl* textCtrl = wxDynamicCast( thisWindow , wxTextCtrl ) ;
    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    switch( GetEventKind( event ) )
    {
        case kEventSearchFieldCancelClicked :
            thisWindow->MacSearchFieldCancelHit( handler , event ) ;
            break ;
        case kEventSearchFieldSearchClicked :
            thisWindow->MacSearchFieldSearchHit( handler , event ) ;
            break ;
    }

    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacSearchControlEventHandler )


// ----------------------------------------------------------------------------
// wxSearchCtrl creation
// ----------------------------------------------------------------------------

// creation
// --------

wxSearchCtrl::wxSearchCtrl()
{ 
    Init();
}

wxSearchCtrl::wxSearchCtrl(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Init();

    Create(parent, id, value, pos, size, style, validator, name);
}

void wxSearchCtrl::Init()
{
    m_menu = 0;
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxTextCtrl::Create(parent, id, wxEmptyString, pos, size, wxBORDER_NONE | style, validator, name) )
    {
        return false;
    }

    EventHandlerRef searchEventHandler;
    InstallControlEventHandler( m_peer->GetControlRef(), GetwxMacSearchControlEventHandlerUPP(),
        GetEventTypeCount(eventList), eventList, this,
        (EventHandlerRef *)&searchEventHandler);

    return true;
}

wxSearchCtrl::~wxSearchCtrl()
{
    delete m_menu;
}

wxSize wxSearchCtrl::DoGetBestSize() const
{
    return wxWindow::DoGetBestSize();
}

// search control specific interfaces
// wxSearchCtrl owns menu after this call
void wxSearchCtrl::SetMenu( wxMenu* menu )
{
    if ( menu == m_menu )
    {
        // no change
        return;
    }

    if ( m_menu )
    {
        m_menu->SetInvokingWindow( 0 );
    }

    delete m_menu;
    m_menu = menu;

    if ( m_menu )
    {
        m_menu->SetInvokingWindow( this );
    }

    GetPeer()->SetSearchMenu( m_menu );
}

wxMenu* wxSearchCtrl::GetMenu()
{
    return m_menu;
}

void wxSearchCtrl::SetSearchButtonVisible( bool show )
{
    if ( GetSearchButtonVisible() == show )
    {
        // no change
        return;
    }
    GetPeer()->SetSearchButtonVisible( show );
}

bool wxSearchCtrl::GetSearchButtonVisible() const
{
    return GetPeer()->GetSearchButtonVisible();
}


void wxSearchCtrl::SetCancelButtonVisible( bool show )
{
    if ( GetCancelButtonVisible() == show )
    {
        // no change
        return;
    }
    GetPeer()->SetCancelButtonVisible( show );
}

bool wxSearchCtrl::GetCancelButtonVisible() const
{
    return GetPeer()->GetCancelButtonVisible();
}

wxInt32 wxSearchCtrl::MacSearchFieldSearchHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    wxCommandEvent event(wxEVT_COMMAND_SEARCHCTRL_SEARCH, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
    return eventNotHandledErr ;
}

wxInt32 wxSearchCtrl::MacSearchFieldCancelHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    wxCommandEvent event(wxEVT_COMMAND_SEARCHCTRL_CANCEL, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
    return eventNotHandledErr ;
}


void wxSearchCtrl::CreatePeer(
           const wxString& str,
           const wxPoint& pos,
           const wxSize& size, long style )
{
#ifdef __WXMAC_OSX__
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        m_peer = new wxMacSearchFieldControl( this , str , pos , size , style );
    }
#endif
#endif
    if ( !m_peer )
    {
        wxTextCtrl::CreatePeer( str, pos, size, style );
    }
}

#endif // USE_NATIVE_SEARCH_CONTROL

#endif // wxUSE_SEARCHCTRL
