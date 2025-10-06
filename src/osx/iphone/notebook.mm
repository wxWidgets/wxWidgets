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
    // TODO: Can we veto here?
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
    wxNotebookIPhoneImpl(wxWindowMac *wxpeer, UIView *v, UITabBarController *controller, UISegmentedControl* segmented )
    : wxWidgetIPhoneImpl(wxpeer, v)
    {
        m_controller = controller;
        m_segmented = segmented;
    }

    wxInt32  GetValue() const wxOVERRIDE
    {
        if (GetWXPeer()->HasFlag( wxBK_BOTTOM ))
        {
            return [m_controller selectedIndex ] + 1; // notebook_osx starts with 1
        }
        else
        {
            return [m_segmented selectedSegmentIndex ] + 1;
        }
    }

    void SetValue( wxInt32 value ) wxOVERRIDE
    {
        if (GetWXPeer()->HasFlag( wxBK_BOTTOM ))
        {
            [m_controller setSelectedIndex: value-1 ];    // notebook_osx starts with 1
        }
        else
        {
            [m_segmented setSelectedSegmentIndex: value-1 ];
        }
    }

    void SetupTabs( const wxNotebook& notebook) wxOVERRIDE
    {
        UIView* slf = (UIView*) m_osxView;


        if (GetWXPeer()->HasFlag( wxBK_BOTTOM ))
        {
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
        else
        {
            [m_segmented removeAllSegments];
            for (int i = 0; i < notebook.GetPageCount(); i++)
            {
                wxNotebookPage* page = notebook.GetPage(i);

                wxCFStringRef text( wxControl::RemoveMnemonics(notebook.GetPageText(i)) );
                [m_segmented insertSegmentWithTitle:text.AsNSString() atIndex:i animated:NO];

                if (notebook.GetPageCount() > 0)
                  [m_segmented setSelectedSegmentIndex: 0 ];
            }
        }
    }

    // TODO: find out what the proper border is
    const int borderAround = 4;

    void Move(int x, int y, int width, int height) wxOVERRIDE
    {
        wxWidgetIPhoneImpl::Move(x, y, width, height);

        if (!GetWXPeer()->HasFlag( wxBK_BOTTOM ))
        {
            CGRect r = CGRectMake( borderAround, borderAround, width-(2*borderAround), 35) ;
            [m_segmented setFrame:r];
        }
    }

    void SegmentedSelected( int selected )
    {
        wxNotebook *notebook = (wxNotebook*) GetWXPeer();
        wxNotebookPage *page = notebook->GetPage( selected );
        page->Show( true );
        for (int i = 0; i < notebook->GetPageCount(); i++)
        {
            if (i != selected)
            {
                wxNotebookPage* page = notebook->GetPage(i);
                page->Hide();
            }
        }
        wxNotebookEvent event(  wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, notebook->GetId(), selected );
        notebook->HandleWindowEvent( event );
    }

protected:
    UITabBarController  *m_controller;
    UISegmentedControl  *m_segmented;

};

@interface wxUISegmentedControl : UISegmentedControl
{
}

@property  wxNotebookIPhoneImpl *owner;

@end

@implementation wxUISegmentedControl

@end



@interface UIControl (wxUIControlActionSupport)

- (void) segmentedValueChangedAction:(id)sender event:(UIEvent*)event;

@end

@implementation UIControl (wxUIControlActionSupport)

- (void) segmentedValueChangedAction:(id)sender event:(UIEvent*)event
{
    wxUISegmentedControl *segmented = (wxUISegmentedControl*) self;
    wxNotebookIPhoneImpl* impl = [segmented owner];
    impl->SegmentedSelected( [segmented selectedSegmentIndex] );
}

@end


wxWidgetImplType* wxWidgetImpl::CreateTabView( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    UITabBarController *controller = nullptr;
    wxUISegmentedControl *segmented = nullptr;
    UIView *v = nullptr;

    if (style & wxBK_BOTTOM)
    {
        controller = [[UITabBarController alloc] init];

        wxTabBarControllerDelegate *delegate = [[wxTabBarControllerDelegate alloc] init];
        [controller setDelegate: delegate];

        v = controller.view;
    }
    else
    {
        CGRect r = CGRectMake( pos.x, pos.y, size.x, size.y) ;
        v = [[UIView alloc] initWithFrame:r];

        CGRect rs = CGRectMake( 0, 0, size.x, 50) ;
        segmented = [[wxUISegmentedControl alloc] initWithFrame:r];

        [segmented addTarget:segmented action:@selector(segmentedValueChangedAction:event:) forControlEvents:UIControlEventValueChanged];

        [v addSubview:segmented];
    }

    /*
    if (style & wxALIGN_CENTER)
        [v setTextAlignment: NSTextAlignmentCenter];
    else if (style & wxALIGN_RIGHT)
        [v setTextAlignment: NSTextAlignmentRight];
    */

    wxNotebookIPhoneImpl* impl = new wxNotebookIPhoneImpl( wxpeer, v, controller, segmented );

    if (segmented != nullptr)
        [segmented setOwner: impl];

    return impl;
}

#endif //if wxUSE_NOTEBOOK
