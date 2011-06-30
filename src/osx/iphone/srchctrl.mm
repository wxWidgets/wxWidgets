///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/srchctrl.mm
// Purpose:     implements mac iphone wxSearchCtrl
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

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#if wxUSE_NATIVE_SEARCH_CONTROL

#include "wx/osx/private.h"
#include "wx/osx/iphone/private.h"


@interface wxUISearchBar : UISearchBar
{
    
}

@end

@implementation wxUISearchBar

+ (void)initialize {
    
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods(self);
    }
}

@end


class wxSearchCtrlIPhoneImpl : public wxWidgetIPhoneImpl, public wxSearchWidgetImpl
{
    public:
    
    wxSearchCtrlIPhoneImpl(wxWindowMac* peer , UISearchBar* w) :
        wxWidgetIPhoneImpl(peer, w)
    {
        m_control = w;
    }
    
    ~wxSearchCtrlIPhoneImpl()
    {
        m_control = NULL;
    }
    
    // search field options
    void ShowSearchButton( bool show )
    {
        if (show) {
            [m_control setShowsSearchResultsButton:YES];
        } else {
            [m_control setShowsSearchResultsButton:NO];
        }
    }
    
    virtual bool IsSearchButtonVisible() const
    {
        if (m_control.showsSearchResultsButton) {
            return true;
        } else {
            return false;
        }
    }
    
    virtual void ShowCancelButton( bool show )
    {
        if (show) {
            [m_control setShowsCancelButton:YES];
        } else {
            [m_control setShowsCancelButton:NO];
        }
    }
    
    virtual bool IsCancelButtonVisible() const
    {
        if (m_control.showsCancelButton) {
            return true;
        } else {
            return false;
        }
    }
    
#if wxUSE_MENUS
    virtual void SetSearchMenu( wxMenu* menu )
    {
        // noop
    }
#endif  // wxUSE_MENUS
    
    virtual void SetDescriptiveText(const wxString& text)
    {
        [m_control setPrompt:wxCFStringRef(text).AsNSString()];
    }
    
    virtual bool SetFocus()
    {
        if ([m_control becomeFirstResponder]) {
            return true;
        } else {
            return false;
        }
    }
                
private:
    UISearchBar* m_control;
};


wxWidgetImplType* wxWidgetImpl::CreateSearchControl(wxSearchCtrl* wxpeer,
                                                    wxWindowMac* WXUNUSED(parent),
                                                    wxWindowID WXUNUSED(id),
                                                    const wxString& str,
                                                    const wxPoint& pos,
                                                    const wxSize& size,
                                                    long WXUNUSED(style),
                                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    UISearchBar* v = [[UISearchBar alloc] initWithFrame:r];
    
    wxWidgetIPhoneImpl* c = new wxSearchCtrlIPhoneImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_NATIVE_SEARCH_CONTROL

#endif // wxUSE_SEARCHCTRL
