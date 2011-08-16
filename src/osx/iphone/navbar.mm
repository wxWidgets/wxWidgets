///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/navbar.mm
// Purpose:     implements mac iphone wxNavigationBar
// Author:      Linas Valiukas
// Created:     2011-08-15
// RCS-ID:      $Id$
// Copyright:   Linas Valiukas
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/osx/private.h"
#include "wx/osx/iphone/private.h"
#include "wx/osx/iphone/private/navctrlimpl.h"

#include "wx/navctrl.h"
#include "wx/navbar.h"


#pragma mark -
#pragma mark Peer implementation

class wxNavigationBarIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxNavigationBarIPhoneImpl(wxWindowMac* peer, wxUINavigationController* navCtrl, WXWidget w) : wxWidgetIPhoneImpl(peer, w)
    {
        m_navCtrl = navCtrl;
        m_navBar = w;
    }
    
    void SetForegroundColour(const wxColour &colour)
    {
        // FIXME does nothing, UINavigationBar title needs to be replaced with
        // UILabel
    }
    
    void SetBackgroundColour(const wxColour &colour)
    {
        UIColor *cocoaColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
        [m_navBar setTintColor:cocoaColor];
    }
    
    bool PushItem(wxNavigationItem* item)
    {
        // FIXME TODO
    }

    bool PopItem()
    {
        // FIXME TODO
    }
    
    bool SetItems(const wxNavigationItemArray& items)
    {
        // FIXME TODO
    }
    
private:
    wxUINavigationController* m_navCtrl;
    UINavigationBar* m_navBar;
    
};


wxWidgetImplType* wxWidgetImpl::CreateNavigationBar(wxWindowMac* wxpeer,
                                                    wxWindowMac* parent,
                                                    wxWindowID WXUNUSED(id),
                                                    const wxPoint& pos,
                                                    const wxSize& size,
                                                    long style,
                                                    long WXUNUSED(extraStyle))
{
    // "Parent" is wxNavigationController
    wxASSERT_MSG(parent, "No parent was passed");
    wxNavigationCtrl* navCtrl = (wxNavigationCtrl *)parent;
    
    wxUINavigationController* cocoaNavCtrl = (wxUINavigationController *)(parent->GetPeer()->GetWXWidget());
    wxASSERT_MSG(cocoaNavCtrl, "Parent does not contain Cocoa control (widget)");
    wxASSERT_MSG([cocoaNavCtrl isKindOfClass:[wxUINavigationController class]], "Parent's native widget is not wxUINavigationController");
    
    UINavigationBar* cocoaNavBar = [cocoaNavCtrl navigationBar];
    wxASSERT_MSG(cocoaNavBar, "No native navigation bar is present");
    
    // Style
    if (style != 0) {
        UIBarStyle navBarStyle = UIBarStyleDefault;
        
        if (style & wxNAVCTRL_NORMAL_BG) {
            navBarStyle = UIBarStyleDefault;
        } else if (style & wxNAVCTRL_BLACK_OPAQUE_BG) {
            navBarStyle = UIBarStyleBlackOpaque;
        } else if (style & wxNAVCTRL_BLACK_TRANSLUCENT_BG) {
            navBarStyle = UIBarStyleBlackTranslucent;
        }
        
        [cocoaNavBar setBarStyle:navBarStyle];
    }    
        
    wxWidgetIPhoneImpl* c = new wxNavigationBarIPhoneImpl( wxpeer, cocoaNavCtrl, cocoaNavBar );
    return c;
}


#pragma mark -
#pragma mark wxNavigationBar implementation

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationBarNameStr[] = wxT("NavigationBar");

IMPLEMENT_DYNAMIC_CLASS(wxNavigationBar, wxNavigationBarBase)

BEGIN_EVENT_TABLE(wxNavigationBar, wxNavigationBarBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNavigationBarEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHING)

bool wxNavigationBar::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxControl::Create( parent, id, pos, size, style, validator, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateNavigationBar( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;    
}

wxNavigationBar::~wxNavigationBar()
{
    ClearItems();
}

void wxNavigationBar::Init()
{

}

bool wxNavigationBar::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) ) {
        // nothing to do
        return false;
    }

    wxNavigationBarIPhoneImpl* peer = (wxNavigationBarIPhoneImpl *)GetPeer();
    if ( !peer ) {
        return false;
    }
    
    peer->SetBackgroundColour(colour);
    return true;
}

bool wxNavigationBar::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) ) {
        // nothing to do
        return false;
    }
    
    wxNavigationBarIPhoneImpl* peer = (wxNavigationBarIPhoneImpl *)GetPeer();
    if ( !peer ) {
        return false;
    }
    
    peer->SetForegroundColour(colour);
    return true;
}

bool wxNavigationBar::Enable(bool enable)
{
    wxControl::Enable(enable);
    return true;
}

bool wxNavigationBar::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);    
    return true;
}

// Pushes an item onto the stack.
bool wxNavigationBar::PushItem(wxNavigationItem* item)
{
    wxNavigationBarIPhoneImpl* peer = (wxNavigationBarIPhoneImpl *)GetPeer();
    if ( !peer ) {
        return false;
    }
    
    {
        wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_PUSHING,
                                      GetId(), GetTopItem(), item);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent))
        {
            if (!navEvent.IsAllowed())
                return false;
        }
    }
    
    if ( !peer->PushItem(item) ) {
        return false;
    }
    m_items.Add(item);
    
    {
        wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_PUSHED,
                                      GetId(), GetTopItem(), item);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxNavigationItem* wxNavigationBar::PopItem()
{
    wxNavigationBarIPhoneImpl* peer = (wxNavigationBarIPhoneImpl *)GetPeer();
    if ( !peer ) {
        return false;
    }
    
    if (m_items.GetCount() > 0) {
        wxNavigationItem* backItem = GetBackItem();
        wxNavigationItem* topItem = GetTopItem();
        if (topItem && backItem) {

            {
                wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_POPPING,
                                              GetId(), GetTopItem(), backItem);
                navEvent.SetEventObject(this);
                navEvent.Allow();
                if (GetEventHandler()->ProcessEvent(navEvent))
                {
                    if (!navEvent.IsAllowed())
                        return NULL;
                }
            }
            
            if ( !peer->PopItem() ) {
                return false;
            }            
            m_items.RemoveAt(m_items.GetCount()-1);
            
            {
                wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_POPPED,
                                              GetId(), topItem, backItem);
                navEvent.SetEventObject(this);
                GetEventHandler()->ProcessEvent(navEvent);
            }
            
            return topItem;
        }
    }
    
    return NULL;
}

// Returns the top item.
wxNavigationItem* wxNavigationBar::GetTopItem() const
{
    if (m_items.GetCount() > 0) {
        wxNavigationItem* item = m_items[m_items.GetCount()-1];
        return item;
    }
    
    return NULL;
}

// Returns the back item.
wxNavigationItem* wxNavigationBar::GetBackItem() const
{
    if (m_items.GetCount() > 1) {
        wxNavigationItem* item = m_items[m_items.GetCount()-2];
        return item;
    }

    return NULL;
}

// Sets the item stack.
void wxNavigationBar::SetItems(const wxNavigationItemArray& items)
{
    wxNavigationBarIPhoneImpl* peer = (wxNavigationBarIPhoneImpl *)GetPeer();
    if ( !peer ) {
        return;
    }
    
    if ( !peer->SetItems(items) ) {
        return;
    }
    
    m_items = items;
}

// Clears the item stack, deleting the items.
void wxNavigationBar::ClearItems(bool deleteItems)
{
    size_t i;
    size_t count = m_items.GetCount();
    for (i = 0; i < count; i++) {
        wxNavigationItem* item = m_items[0];
        m_items.RemoveAt(0);
        
        if (deleteItems) {
            delete item;
        }
    }
    
    SetItems(m_items);
}

wxBarButton* wxNavigationBar::GetLeftButton()
{
    wxNavigationItem* item = GetTopItem();
    if (item) {
        if (item->GetLeftButton()) {
            return item->GetLeftButton();
        } else if (GetBackItem()) {
            if (GetBackItem()->GetBackButton()) {
                return GetBackItem()->GetBackButton();
            } else {
                return & m_backButton;
            }
        }
    }
    
    return NULL;
}

wxBarButton* wxNavigationBar::GetRightButton()
{
    wxNavigationItem* item = GetTopItem();
    if (item) {
        if (item->GetRightButton()) {
            return item->GetRightButton();
        }
    }
    return NULL;
}

wxString wxNavigationBar::GetCurrentTitle()
{
    wxNavigationItem* item = GetTopItem();
    if (item) {
        return item->GetTitle();
    }
    
    return wxEmptyString;
}
