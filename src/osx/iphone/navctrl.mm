///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/navctrl.mm
// Purpose:     implements mac iphone wxNavigationCtrl
// Author:      Linas Valiukas
// Created:     2011-06-30
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

#include "wx/viewcontroller.h"
#include "wx/osx/iphone/private/navctrlimpl.h"


#pragma mark -
#pragma mark Cocoa class

@implementation wxFakeRootViewControllerForWxUINavigationController

- (id)init {
    if ((self = [super init])) {
        self.title = @"-";
    }
    
    return self;
}

@end


@implementation wxUINavigationController

- (id)initWithFakeRootViewController {
    
    wxFakeRootViewControllerForWxUINavigationController *fake = [[wxFakeRootViewControllerForWxUINavigationController alloc] init];
    if (! fake) {
        return nil;
    }
    
    if ((self = [super initWithRootViewController:fake])) {
        fakeRootViewController = fake;
        initializedWithFakeRootViewController = YES;
        numberOfActualViewControllers = 0;
    }
    
    return self;
}

- (void)dealloc {
    if (initializedWithFakeRootViewController) {
        [fakeRootViewController release];
        fakeRootViewController = nil;
    }
    
    [super dealloc];
}

- (UIViewController *)topViewController {
    if (! initializedWithFakeRootViewController) {
        return [super topViewController];
    }
    
    if ([super topViewController] == fakeRootViewController) {
        return nil;
    } else {
        return [super topViewController];
    }
}

- (UIViewController *)visibleViewController {
    if (! initializedWithFakeRootViewController) {
        return [super visibleViewController];
    }
    
    if ([super visibleViewController] == fakeRootViewController) {
        return nil;
    } else {
        return [super visibleViewController];
    }
}

- (NSArray *)viewControllers {
    if (! initializedWithFakeRootViewController) {
        return [super viewControllers];
    }
    
    // All but the fake
    NSRange range;
    range.location = 1;
    range.length = [[super viewControllers] count]-1;
    return [[super viewControllers] objectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
}

- (void)setViewControllers:(NSArray *)viewControllers animated:(BOOL)animated {
    if (! initializedWithFakeRootViewController) {
        [super setViewControllers:viewControllers animated:animated];
        return;
    }
    
    // Fake, then others
    NSMutableArray *controllers = [NSMutableArray arrayWithObject:fakeRootViewController];
    [controllers addObjectsFromArray:viewControllers];
    [super setViewControllers:controllers];
}

// FIXME don't animate the pushing of "fake" VC and the "root" (first one after fake) VC
- (void)pushViewController:(UIViewController *)viewController animated:(BOOL)animated {
    if (! initializedWithFakeRootViewController) {
        [super pushViewController:viewController animated:animated];
        return;
    }

    [super pushViewController:viewController animated:animated];
    if ([[super viewControllers] count] == 2) { // fake plus "root"
        viewController.navigationItem.hidesBackButton = YES;
    }
}

- (UIViewController *)popViewControllerAnimated:(BOOL)animated {
    if (! initializedWithFakeRootViewController) {
        return [super popViewControllerAnimated:animated];
    }
    
    if ([[super viewControllers] count] == 1) { // only fake
        return nil;
    } else {
        return [super popViewControllerAnimated:animated];
    }
}

- (NSArray *)popToRootViewControllerAnimated:(BOOL)animated {
    if (! initializedWithFakeRootViewController) {
        return [super popToRootViewControllerAnimated:animated];
    }
    
    return [super popToViewController:fakeRootViewController animated:animated];
}

@end


#pragma mark -
#pragma mark Peer implementation

wxNavigationCtrlIPhoneImpl::wxNavigationCtrlIPhoneImpl(wxWindowMac* peer, void *navController, WXWidget w) :
    wxWidgetIPhoneImpl( peer, w ), wxNavigationControllerImpl()
{
    m_navcontroller = navController;
}
    
wxNavigationCtrlIPhoneImpl::~wxNavigationCtrlIPhoneImpl()
{
    if (m_navcontroller) {
        wxUINavigationController *navController = (wxUINavigationController *)m_navcontroller;
        [navController release];
        navController = nil;
    }
        
}
    
bool wxNavigationCtrlIPhoneImpl::PushViewController(wxViewController *controller)
{
    UIViewController *viewController = (UIViewController *)controller->GetUIViewController();
    if (! viewController) {
        return false;
    }
    
    wxUINavigationController *navController = (wxUINavigationController *)m_navcontroller;
    [navController pushViewController:viewController
                               animated:YES];        
    return true;
}
    
bool wxNavigationCtrlIPhoneImpl::PopViewController()
{
    wxUINavigationController *navController = (wxUINavigationController *)m_navcontroller;
    if ([navController popViewControllerAnimated:YES]) {
        return true;
    } else {
        return false;
    }
}
        


wxWidgetImplType* wxWidgetImpl::CreateNavigationController(wxWindowMac* wxpeer,
                                                           wxWindowMac* parent,
                                                           wxWindowID WXUNUSED(id),
                                                           const wxPoint& pos,
                                                           const wxSize& size,
                                                           long WXUNUSED(style),
                                                           long WXUNUSED(extraStyle))
{
    wxUINavigationController* v = [[wxUINavigationController alloc] initWithFakeRootViewController];
    
    // Adjust to parent's size
    if (parent) {
        UIView *parentView = parent->GetPeer()->GetWXWidget();
        if (parentView) {
            CGRect parentViewFrame = parentView.frame;
            parentViewFrame.origin.x = 0;
            parentViewFrame.origin.y = 0;
            [v.view setFrame:parentViewFrame];
        }        
    }
    
    wxWidgetIPhoneImpl* c = new wxNavigationCtrlIPhoneImpl( wxpeer, v, v.view );
    return c;
}


#pragma mark -
#pragma mark wxNavigationCtrl implementation

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationCtrlNameStr[] = wxT("NavigationCtrl");

IMPLEMENT_DYNAMIC_CLASS(wxNavigationCtrl, wxNavigationCtrlBase)

BEGIN_EVENT_TABLE(wxNavigationCtrl, wxNavigationCtrlBase)
EVT_SIZE(wxNavigationCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNavigationCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHING)


/// Default constructor.
wxNavigationCtrl::wxNavigationCtrl()
{
    Init();
}

/// Constructor.
wxNavigationCtrl::wxNavigationCtrl(wxWindow *parent,
                                       wxWindowID id,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long style,
                                       const wxValidator& validator,
                                       const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, validator, name);
}

wxNavigationCtrl::~wxNavigationCtrl()
{
    ClearControllers();
}

void wxNavigationCtrl::Init()
{
    m_navBar = NULL;
}

/// Creation function.
bool wxNavigationCtrl::Create(wxWindow *parent,
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
    
    SetPeer(wxWidgetImpl::CreateNavigationController( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;    
}

// Pushes a controller onto the stack.
bool wxNavigationCtrl::PushController(wxViewController* controller)
{
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_PUSHING,
                                       GetId(), GetTopController(), controller);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent))
        {
            if (!navEvent.IsAllowed())
                return false;
        }
    }
    
    m_controllers.Add(controller);
    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();
    if (! peer->PushViewController(controller)) {
        return false;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_PUSHED,
                                       GetId(), GetTopController(), controller);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxViewController* wxNavigationCtrl::PopController()
{
    if (m_controllers.GetCount() == 0) {
        return NULL;
    }
    
    wxViewController* backItem = GetBackController();
    wxViewController* topItem = GetTopController();
    
    if (! (topItem && backItem)) {
        return NULL;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPING,
                                       GetId(), GetTopController(), backItem);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent)) {
            if (!navEvent.IsAllowed()) {
                return NULL;
            }
        }
    }
    
    if (topItem->GetNavigationItem()) {
        (void) m_navBar->PopItem();
    }
    
    m_controllers.RemoveAt(m_controllers.GetCount()-1);
    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();
    if (! peer->PopViewController()) {
        return NULL;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPED,
                                       GetId(), topItem, backItem);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return topItem;
}

// Sets the item stack.
void wxNavigationCtrl::SetControllers(const wxViewControllerArray& controllers)
{
    ClearControllers();
    
    size_t i;
    for (i = 0; i < controllers.GetCount(); i++) {
        PushController(controllers[i]);
    }
}

// Returns the top item.
wxViewController* wxNavigationCtrl::GetTopController() const
{
    if (m_controllers.GetCount() > 0) {
        wxViewController* controller = m_controllers[m_controllers.GetCount()-1];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the back item.
wxViewController* wxNavigationCtrl::GetBackController() const
{
    if (m_controllers.GetCount() > 1) {
        wxViewController* controller = m_controllers[m_controllers.GetCount()-2];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the root item.
wxViewController* wxNavigationCtrl::GetRootController() const
{
    if (m_controllers.GetCount() > 1) {
        wxViewController* controller = m_controllers[0];
        return controller;
    } else {
        return NULL;
    }
}

// Clears the item stack, deleting the items.
void wxNavigationCtrl::ClearControllers()
{
    m_navBar->ClearItems(false);
    
    size_t i;
    size_t count = m_controllers.GetCount();
    
    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();
    
    for (i = 0; i < count; i++) {
        wxViewController* controller = m_controllers[0];
        
        peer->PopViewController();
        m_controllers.RemoveAt(0);
        
        if (controller->OnDelete()) {
            delete controller;
        }
    }
}

#pragma mark Events

void wxNavigationCtrl::OnBack(wxCommandEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxNavigationCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}
