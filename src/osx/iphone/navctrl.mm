///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/navctrl.mm
// Purpose:     implements mac iphone wxMoNavigationCtrl
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

#include "wx/osx/iphone/private/navctrlimpl.h"


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
#pragma mark wx

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
    
bool wxNavigationCtrlIPhoneImpl::PushViewController(wxMoViewController *controller)
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
