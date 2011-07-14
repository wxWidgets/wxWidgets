///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/actionsheetdlg.mm
// Purpose:     implements mac iphone wxActionSheetDialog
// Author:      Linas Valiukas
// Created:     2011-07-13
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

// wxNotebook for showFromTabBar:
#include "wx/notebook.h"
#include "wx/osx/iphone/private/notebookimpl.h"

#include "wx/sheetdlg.h"


#pragma mark -
#pragma mark Cocoa class

@interface wxUIActionSheet : UIActionSheet <UIActionSheetDelegate>
{
    
}

// delegate is the action sheet itself
- (id)initWithTitle:(NSString *)initTitle
  cancelButtonTitle:(NSString *)initCancelButtonTitle
destructiveButtonTitle:(NSString *)initDestructiveButtonTitle
  otherButtonTitles:(NSString *)initOtherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION;

@end

@implementation wxUIActionSheet

- (id)initWithTitle:(NSString *)initTitle
  cancelButtonTitle:(NSString *)initCancelButtonTitle
destructiveButtonTitle:(NSString *)initDestructiveButtonTitle
  otherButtonTitles:(NSString *)initOtherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION {
 
    if ((self = [super initWithTitle:initTitle
                            delegate:nil
                   cancelButtonTitle:initCancelButtonTitle
              destructiveButtonTitle:initDestructiveButtonTitle
                   otherButtonTitles:initOtherButtonTitles])) {
        
        [self setDelegate:self];
    }
    
    return self;
}


- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    
}

- (void)willPresentActionSheet:(UIActionSheet *)actionSheet {
    
}

- (void)didPresentActionSheet:(UIActionSheet *)actionSheet {
    
}

- (void)actionSheet:(UIActionSheet *)actionSheet willDismissWithButtonIndex:(NSInteger)buttonIndex {
    
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex {
    
}

- (void)actionSheetCancel:(UIActionSheet *)actionSheet {
    
}

@end




#pragma mark -
#pragma mark Peer implementation

class wxActionSheetCtrlIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxActionSheetCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
        m_actionSheet = w;
    }
    
    ~wxActionSheetCtrlIPhoneImpl()
    {
        if (m_actionSheet) {
            [m_actionSheet release];
            m_actionSheet = nil;
        }
    }
        
    bool Show(wxWindow* animateFrom)
    {
        if ( !m_actionSheet ) {
            return false;
        }
        if ( !animateFrom ) {
            return false;
        }
        
        // Analyze parent, choose a way to show an action sheet in it
        if (animateFrom->IsKindOf(wxCLASSINFO(wxNotebook))) {
            
            // wxNotebook (UITabBar)
            wxNotebookIPhoneImpl *peer = (wxNotebookIPhoneImpl *)animateFrom->GetPeer();
            if ( !peer ) {
                return false;
            }
            
            wxUITabBarController *tabBarController = peer->GetTabBarController();
            if ( !tabBarController ) {
                return false;
            }
            
            [m_actionSheet showFromTabBar:tabBarController.tabBar];
            
        } else if (animateFrom->IsKindOf(wxCLASSINFO(wxToolBar))) {
            
            // wxToolbar (UIToolbar)
            wxOSXWidgetImpl *peer = animateFrom->GetPeer();
            if ( !peer ) {
                return false;
            }
            
            UIToolbar *toolbar = peer->GetWXWidget();
            if ( !toolbar ) {
                return false;
            }
            
            if (! [toolbar isKindOfClass:[UIToolbar class]]) {
                return false;
            }
            
            [m_actionSheet showFromToolbar:toolbar];
            
        } else {
        
            wxOSXWidgetImpl *peer = animateFrom->GetPeer();
            if ( !peer ) {
                return false;
            }
            
            UIView *view = peer->GetWXWidget();
            if ( !view ) {
                return false;
            }
            
            [m_actionSheet showInView:view];
            
        }
        
        // FIXME TODO: showFromBarButtonItem
                        
        return true;
    }
    
    bool Dismiss(wxWindowID id)
    {
        if ( !m_actionSheet ) {
            return false;
        }
        
        // temporary remove delegate to avoid sending "button clicked" events to it
        [m_actionSheet setDelegate:nil];
        [m_actionSheet dismissWithClickedButtonIndex:0
                                            animated:YES];
        [m_actionSheet setDelegate:m_actionSheet];
        
        return true;
    }
    
    
private:
    wxUIActionSheet *m_actionSheet;
};



#pragma mark -
#pragma mark wxActionSheetDialog implementation

IMPLEMENT_DYNAMIC_CLASS(wxActionSheetDialog, wxSheetDialogBase)

BEGIN_EVENT_TABLE(wxActionSheetDialog, wxSheetDialogBase)
END_EVENT_TABLE()

/// Creation function.
bool wxActionSheetDialog::Create(wxWindow *parent,
                                 const wxString& title,
                                 const wxString& cancelButtonTitle,
                                 const wxArrayString& otherButtonTitles,
                                 long style)
{
    DontCreatePeer();
    
    m_sheetParent = parent;
    
    if ( !CreateSheet(wxSHEET_DIALOG_TYPE_ACTION, parent, title, wxEmptyString, cancelButtonTitle, otherButtonTitles, style)) {
        return false;
    }
    
    // Don't create actual wxUIActionSheet just now - the parameters for it (title, buttons)
    // might change before actually showing the sheet
    
    return true;
}

void wxActionSheetDialog::Init()
{
    m_sheetImpl = NULL;
    m_sheetParent = NULL;
}

bool wxActionSheetDialog::ShowSheetDialog(wxWindow* animateFrom)
{
    NSString *stringTitle = [NSString stringWithString:wxCFStringRef(m_sheetDialogTitle).AsNSString()];
    if ([stringTitle isEqualToString:@""]) {
        stringTitle = nil;
    }
    NSString *stringCancelButtonTitle = [NSString stringWithString:wxCFStringRef(m_cancelButtonTitle).AsNSString()];
    if ([stringCancelButtonTitle isEqualToString:@""]) {
        stringCancelButtonTitle = nil;
    }
    
    wxUIActionSheet *actionSheet = [[wxUIActionSheet alloc] initWithTitle:stringTitle
                                                        cancelButtonTitle:stringCancelButtonTitle
                                                   destructiveButtonTitle:nil   // FIXME
                                                        otherButtonTitles:nil];
    if ( !actionSheet ) {
        return false;
    }
    
    // Add remaining buttons ("other buttons")
    size_t i;
    NSString *otherButtonTitle = nil;
    for (i = 0; i < m_buttonTitles.GetCount(); i++) {
        otherButtonTitle = [NSString stringWithString:wxCFStringRef(m_buttonTitles[i]).AsNSString()];
        if ([otherButtonTitle isEqualToString:@""]) {
            otherButtonTitle = nil;
        }
        
        if (otherButtonTitle) {
            [actionSheet addButtonWithTitle:otherButtonTitle];
        }
    }
    
    
    // Not using SetPeer() because it calls Embed(), and we don't want this action sheet to be a subview of anything
    m_sheetImpl = new wxActionSheetCtrlIPhoneImpl(this, actionSheet);
    
    if (m_sheetParent) {
        return m_sheetImpl->Show(m_sheetParent);
    } else {
        return m_sheetImpl->Show(animateFrom);
    }
}

bool wxActionSheetDialog::DismissSheetDialog(wxWindowID id)
{
    if ( !m_sheetImpl ) {
        return false;
    }
    
    if ( !m_sheetImpl->Dismiss(id) ) {
        return false;
    }
    
    delete m_sheetImpl;
    m_sheetImpl = NULL;
    
    return true;
}
