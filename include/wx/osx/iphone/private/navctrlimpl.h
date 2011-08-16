/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private/navctrlimpl.h
// Purpose:     wxNavigationCtrl implementation classes that have to be exposed
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-06-30
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_IPHONE_PRIVATE_NAVCTRL_H_
#define _WX_OSX_IPHONE_PRIVATE_NAVCTRL_H_

#include "wx/osx/private.h"
#include "wx/navctrl.h"
#include "wx/viewcontroller.h"


// Fake root view controller for wxUINavigationController
@interface wxFakeRootViewControllerForWxUINavigationController : UIViewController
{
}

@end

// UINavigationController subclass
@interface wxUINavigationController : UINavigationController
{
@private
    BOOL initializedWithFakeRootViewController;
    unsigned int numberOfActualViewControllers;
    wxFakeRootViewControllerForWxUINavigationController *fakeRootViewController;
}

// Initializer which creates a "fake" root view controller instead of the actual root VC
// 
// Reason: when creating wxMoNavigationCtrl(), the library still is not aware of what's
// the first (root) VC is going to be, and we need to provide a native control for the 
// library straight away.
//
// So, we'll keep a fake view controller by our side and will replace the root VC with it
// when we're out of "actual" view controllers and also initially (when nothing's pushed).
- (id)initWithFakeRootViewController;

- (UIViewController *)topViewController;
- (UIViewController *)visibleViewController;
- (NSArray *)viewControllers;
- (void)setViewControllers:(NSArray *)viewControllers animated:(BOOL)animated;
- (void)pushViewController:(UIViewController *)viewController animated:(BOOL)animated;
- (UIViewController *)popViewControllerAnimated:(BOOL)animated;
- (NSArray *)popToRootViewControllerAnimated:(BOOL)animated;

@end





class wxNavigationCtrlIPhoneImpl : public wxWidgetIPhoneImpl, public wxNavigationControllerImpl
{
public:
    
    wxNavigationCtrlIPhoneImpl(wxWindowMac* peer, void* navController, WXWidget w);
    
    ~wxNavigationCtrlIPhoneImpl();
    
    virtual bool PushViewController(wxViewController *controller);
    virtual bool PopViewController();
    
    // Implementation: get Cocoa's wxUINavigationController
    void* GetNativeNavigationController() { return m_navcontroller; }
    
private:
    void* m_navcontroller;
};

#endif // _WX_OSX_IPHONE_PRIVATE_NAVCTRL_H_
