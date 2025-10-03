/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/notebook.mm
// Purpose:     wxNotebook
// Author:      Robert Roebling
// Modified by:
// Created:     08/08/25
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"
#include "wx/private/bmpbndl.h"

#include <stdio.h>


@interface wxTabBarControllerDelegate : NSObject <UITabBarControllerDelegate>
{
}

-(BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController;
-(void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController;

@end

@implementation wxTabBarControllerDelegate

- (id) init
{
    self = [super init];
    return self;
}

-(BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController
{
    // wxLogMessage( "shouldSelectUIViewController" );
    return YES;
}

-(void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController;
{
    UITabBarController* controller = tabBarController;
    UIViewController *subController = viewController;

    wxWidgetIPhoneImpl* viewimpl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( controller.view );
    if ( viewimpl )
    {
        wxNotebook* wxpeer = (wxNotebook*) viewimpl->GetWXPeer();
        wxpeer->OSXHandleClicked( 0 );  // parameter is a timestamp
    }
}

@end



class wxNotebookIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxNotebookIPhoneImpl(wxWindowMac *wxpeer, UIView *v, UITabBarController *c)
    : wxWidgetIPhoneImpl(wxpeer, v)
    {
        m_controller = c;
    }
    
    wxInt32  GetValue() const wxOVERRIDE
    {
        return [m_controller selectedIndex ] + 1; // notebook_osx starts with 1
    }
    
    void SetValue( wxInt32 v ) wxOVERRIDE
    {
        [m_controller setSelectedIndex: v-1 ];    // notebook_osx starts with 1
    }

    void SetupTabs( const wxNotebook& notebook) wxOVERRIDE
    {
        UIView* slf = (UIView*) m_osxView;
        // if (controller.view != slf) return; 

        NSMutableArray *viewControllers = [[NSMutableArray alloc] init ];

        for (int i = 0; i < notebook.GetPageCount(); i++)
        {
            wxNotebookPage* page = notebook.GetPage(i);

            UIViewController *subController =[[UIViewController alloc] init];

            wxCFStringRef text( wxControl::RemoveMnemonics(notebook.GetPageText(i)) );
            UIImage *nbImage = nil;
            const wxBitmapBundle bitmap = notebook.GetPageBitmapBundle(i);
            if ( bitmap.IsOk() ) {
                nbImage = wxOSXGetImageFromBundle(bitmap);
            }
            UITabBarItem *item = [[UITabBarItem alloc] initWithTitle: text.AsNSString() image:nbImage tag: i ];
            [subController setTabBarItem: item];

            UIView *subView = [[UIView alloc] init ];
            [slf addSubview: subView];
            [subController setView: subView];
            [subView addSubview: page->GetHandle()];

            [viewControllers addObject: subController];
        }        

        [m_controller setViewControllers: viewControllers];

    }

protected:
    UITabBarController  *m_controller;

};


wxWidgetImplType* wxWidgetImpl::CreateTabView( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    UITabBarController *controller = [[UITabBarController alloc] init];
   
    wxTabBarControllerDelegate *delegate = [[wxTabBarControllerDelegate alloc] init];
    [controller setDelegate: delegate];

    UIView *v = controller.view;

    /*
    if (style & wxALIGN_CENTER)
        [v setTextAlignment: NSTextAlignmentCenter];
    else if (style & wxALIGN_RIGHT)
        [v setTextAlignment: NSTextAlignmentRight];
    */

    wxWidgetIPhoneImpl* c = new wxNotebookIPhoneImpl( wxpeer, v, controller );
    return c;
}

#endif //if wxUSE_NOTEBOOK
