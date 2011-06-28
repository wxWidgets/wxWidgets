/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/notebook.mm
// Purpose:     wxNotebook
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-06-27
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
#include "wx/string.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/image.h"
#endif // WX_PRECOMP

#include "wx/string.h"
#include "wx/imaglist.h"
#include "wx/osx/private.h"
#include "wx/mobile/defs.h"

#include <stdio.h>


#pragma mark -
#pragma mark Cocoa

#pragma mark UITabBarController

@interface wxUITabBarController : UITabBarController <UITabBarControllerDelegate, UITabBarDelegate>
{

}

- (BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController;
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController;

@end

@implementation wxUITabBarController

- (BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController {
    return YES;
}

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController {
    
}

@end


#pragma mark -
#pragma mark wx

class wxNotebookIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxNotebookIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
        m_isUserPane = true;
        m_isRootControl = true;
    }
    
    // Ignore attempts to set position/size
    void Move(int x, int y, int width, int height)
    {
        
    }
    
    // Don't know nothing about labels too
    void SetLabel(const wxString& title, wxFontEncoding encoding)
    {
        
    }
    
    void GetContentArea( int &left , int &top , int &width , int &height ) const
    {
        UITabBar *tabBar = (UITabBar *)m_osxView;

        CGRect r = tabBar.bounds;
        left = (int)r.origin.x;
        top = (int)r.origin.y;
        width = (int)r.size.width;
        height = (int)r.size.height;
    }
    
    void SetValue( wxInt32 value )
    {
        UITabBar *tabBar = (UITabBar *) m_osxView;
        wxUITabBarController *tabBarController = (wxUITabBarController *)[tabBar delegate];

        // avoid 'changed' events when setting the tab programmatically
        [tabBarController setDelegate:nil];
        [tabBarController setSelectedIndex:(value-1)];
        [tabBarController setDelegate:tabBarController];
    }
    
    wxInt32 GetValue() const
    {
        UITabBar *tabBar = (UITabBar *) m_osxView;
        wxUITabBarController *tabBarController = (wxUITabBarController *)[tabBar delegate];
        return [tabBarController selectedIndex]+1;
    }
    
    void SetMaximum( wxInt32 maximum )
    {
        UITabBar *tabBar = (UITabBar *) m_osxView;
        wxUITabBarController *tabBarController = (wxUITabBarController *)[tabBar delegate];
        
        int tabCount = [[tabBarController viewControllers] count];
        
        if (maximum != tabCount) {
        
            NSMutableArray *controllers = [NSMutableArray arrayWithArray:[tabBarController viewControllers]];
            
            // avoid 'changed' events when setting the tab programmatically
            [tabBarController setDelegate:nil];
            
            if ( maximum > tabCount )
            {
                for ( int i = tabCount ; i < maximum ; ++i )
                {
                    UIViewController *viewController = [[UIViewController alloc] init];
                    [viewController setTitle:@"-"];
                    [controllers addObject:viewController];
                    [viewController release];
                }
            }
            else if ( maximum < tabCount )
            {
                NSRange removeRange;
                removeRange.location = maximum;
                removeRange.length = tabCount-maximum;
                [controllers removeObjectsInRange:removeRange];
            }
            
            [tabBarController setViewControllers:controllers];
            [tabBarController setDelegate:tabBarController];
        }
    }

    void SetupTabs( const wxNotebook& notebook )
    {
        int pageCount = notebook.GetPageCount();
        
        SetMaximum( pageCount );
                
        UITabBar *tabBar = (UITabBar *) m_osxView;
        wxUITabBarController *tabBarController = (wxUITabBarController *)[tabBar delegate];
                
        // avoid 'changed' events when setting the tab programmatically
        [tabBarController setDelegate:nil];
        
        for ( int i = 0 ; i < pageCount ; ++i )
        {
            wxNotebookPage* page = notebook.GetPage(i);
            UIViewController *controller = [[tabBarController viewControllers] objectAtIndex:i];
                        
            int pageImage = notebook.GetPageImage(i);
            UITabBarSystemItem systemItem = UITabBarSystemItemHistory;
            
            switch (pageImage) {
                case wxID_FAVORITES:    systemItem = UITabBarSystemItemFavorites;   break;
                case wxID_FEATURED:     systemItem = UITabBarSystemItemFeatured;    break;
                case wxID_TOPRATED:     systemItem = UITabBarSystemItemTopRated;    break;
                case wxID_RECENTS:      systemItem = UITabBarSystemItemRecents;     break;
                case wxID_CONTACTS:     systemItem = UITabBarSystemItemContacts;    break;
                case wxID_HISTORY:      systemItem = UITabBarSystemItemBookmarks;   break;
                case wxID_BOOKMARKS:    systemItem = UITabBarSystemItemBookmarks;   break;
                case wxID_SEARCH:       systemItem = UITabBarSystemItemSearch;      break;
                case wxID_DOWNLOADS:    systemItem = UITabBarSystemItemDownloads;   break;
                case wxID_MOSTRECENT:   systemItem = UITabBarSystemItemMostRecent;  break;
                case wxID_MOSTVIEWED:   systemItem = UITabBarSystemItemMostViewed;  break;
                default:                systemItem = UITabBarSystemItemHistory;     break;
            };
             
            controller.tabBarItem = [[UITabBarItem alloc] initWithTabBarSystemItem:systemItem
                                                                               tag:i];
            
            [controller setView:page->GetPeer()->GetWXWidget()];
            wxCFStringRef cf( page->GetLabel() , notebook.GetFont().GetEncoding() );
            NSString *tabTitle = cf.AsNSString();
            [controller setTitle:tabTitle];
        }
        
        [tabBarController setDelegate:tabBarController];
    }
    
};

wxWidgetImplType* wxWidgetImpl::CreateTabView( wxWindowMac* wxpeer,
                                                 wxWindowMac* WXUNUSED(parent),
                                                 wxWindowID WXUNUSED(id),
                                                 const wxPoint& pos,
                                                 const wxSize& size,
                                                 long style,
                                                 long WXUNUSED(extraStyle))
{
    wxUITabBarController* controller = [[wxUITabBarController alloc] init];
    
    // Status bar (20 px)
    CGRect tabBarFrame = controller.tabBar.frame;
    tabBarFrame.origin.y -= 20;
    [controller.tabBar setFrame:tabBarFrame];
    
    [controller setDelegate:controller];
        
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    
    wxWidgetIPhoneImpl* c = new wxNotebookIPhoneImpl( wxpeer, controller.tabBar );
    return c;
}

#endif //if wxUSE_NOTEBOOK
