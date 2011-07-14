/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private/notebookimpl.h
// Purpose:     wxNotebook implementation classes that have to be exposed
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_IPHONE_PRIVATE_NOTEBOOK_H_
#define _WX_OSX_IPHONE_PRIVATE_NOTEBOOK_H_

#include "wx/osx/private.h"


#pragma mark Cocoa

@interface wxUITabBarController : UITabBarController <UITabBarControllerDelegate>
{

}

- (BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController;
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController;

@end


#pragma mark Peer implementation

class wxNotebookIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxNotebookIPhoneImpl( wxWindowMac* peer , wxUITabBarController *tabBarController, WXWidget w );
    void Move(int x, int y, int width, int height);
    void SetLabel(const wxString& title, wxFontEncoding encoding);
    void GetContentArea( int &left , int &top , int &width , int &height ) const;
    void SetValue( wxInt32 value );
    wxInt32 GetValue() const;
    void SetMaximum( wxInt32 maximum );
    bool SetBadge(int item, const wxString& badge);
    wxString GetBadge(int item) const;
    void SetupTabs( const wxNotebook& notebook );
    wxUITabBarController* GetTabBarController();
    
private:
    wxUITabBarController *m_tabBarController;
};

#endif // _WX_OSX_IPHONE_PRIVATE_NOTEBOOK_H_
