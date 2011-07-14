///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/alertsheetdlg.mm
// Purpose:     implements mac iphone wxAlertSheetDialog
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

#include "wx/sheetdlg.h"


#pragma mark -
#pragma mark Cocoa class

@interface wxUIAlertView : UIAlertView <UIAlertViewDelegate>
{
    
}

// delegate is the alert view itself
- (id)initWithTitle:(NSString *)initTitle
            message:(NSString *)initMessage
  cancelButtonTitle:(NSString *)cancelButtonTitle
  otherButtonTitles:(NSString *)otherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION;

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;
- (void)willPresentAlertView:(UIAlertView *)alertView;
- (void)didPresentAlertView:(UIAlertView *)alertView;
- (void)alertView:(UIAlertView *)alertView willDismissWithButtonIndex:(NSInteger)buttonIndex;
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
- (void)alertViewCancel:(UIAlertView *)alertView;

@end

@implementation wxUIAlertView

- (id)initWithTitle:(NSString *)initTitle
            message:(NSString *)initMessage
  cancelButtonTitle:(NSString *)initCancelButtonTitle
  otherButtonTitles:(NSString *)initOtherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION {
    
    if ((self = [super initWithTitle:initTitle
                             message:initMessage
                            delegate:nil
                   cancelButtonTitle:initCancelButtonTitle
                   otherButtonTitles:initOtherButtonTitles])) {
        
        [self setDelegate:self];
    }
    
    return self;
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    
}

- (void)willPresentAlertView:(UIAlertView *)alertView {
    
}

- (void)didPresentAlertView:(UIAlertView *)alertView {
    
}

- (void)alertView:(UIAlertView *)alertView willDismissWithButtonIndex:(NSInteger)buttonIndex {
    
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
    
}

- (void)alertViewCancel:(UIAlertView *)alertView {
    
}

@end



#pragma mark -
#pragma mark Peer implementation

class wxAlertSheetCtrlIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxAlertSheetCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
        m_alertView = w;
    }
        
    bool Show(wxWindow* animateFrom)
    {
        if ( !m_alertView ) {
            return false;
        }

        [m_alertView show];
        
        return true;
    }
    
    bool Dismiss(wxWindowID id)
    {
        if ( !m_alertView ) {
            return false;
        }
        
        // temporary remove delegate to avoid sending "button clicked" events to it
        [m_alertView setDelegate:nil];
        [m_alertView dismissWithClickedButtonIndex:0
                                          animated:YES];
        [m_alertView setDelegate:m_alertView];
        
        return true;
    }
    
    
private:
    wxUIAlertView *m_alertView;
};


#pragma mark -
#pragma mark wxAlertSheetDialog implementation

IMPLEMENT_DYNAMIC_CLASS(wxAlertSheetDialog, wxSheetDialogBase)

BEGIN_EVENT_TABLE(wxAlertSheetDialog, wxSheetDialogBase)
END_EVENT_TABLE()


bool wxAlertSheetDialog::Create(wxWindow *parent,
                                const wxString& title,
                                const wxString& msg,
                                const wxString& cancelButtonTitle,
                                const wxArrayString& otherButtonTitles,
                                long style)
{
    DontCreatePeer();

    if ( !CreateSheet(wxSHEET_DIALOG_TYPE_ALERT, parent, title, msg, cancelButtonTitle, otherButtonTitles, style) ) {
        return false;
    }
    
    // Don't create actual wxUIAlertSheet just now - the parameters for it (title, buttons)
    // might change before actually showing the sheet
    
    return true;
}

void wxAlertSheetDialog::Init()
{
    m_sheetImpl = NULL;
}

bool wxAlertSheetDialog::ShowSheetDialog(wxWindow* WXUNUSED(animateFrom))
{
    NSString *stringTitle = [NSString stringWithString:wxCFStringRef(m_sheetDialogTitle).AsNSString()];
    if ([stringTitle isEqualToString:@""]) {
        stringTitle = nil;
    }
    
    NSString *stringMessage = [NSString stringWithString:wxCFStringRef(m_sheetDialogMessage).AsNSString()];
    if ([stringMessage isEqualToString:@""]) {
        stringMessage = nil;
    }
    
    NSString *stringCancelButtonTitle = [NSString stringWithString:wxCFStringRef(m_cancelButtonTitle).AsNSString()];
    if ([stringCancelButtonTitle isEqualToString:@""]) {
        stringCancelButtonTitle = nil;
    }
    
    wxUIAlertView *alertView = [[wxUIAlertView alloc] initWithTitle:stringTitle
                                                            message:stringMessage
                                                  cancelButtonTitle:stringCancelButtonTitle
                                                  otherButtonTitles:nil];
    
    if ( !alertView ) {
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
            [alertView addButtonWithTitle:otherButtonTitle];
        }
    }
    
    
    // Not using SetPeer() because it calls Embed(), and we don't want this action sheet to be a subview of anything
    m_sheetImpl = new wxAlertSheetCtrlIPhoneImpl(this, alertView);
    
    return m_sheetImpl->Show(NULL);
}

bool wxAlertSheetDialog::DismissSheetDialog(wxWindowID id)
{
    return true;
}

