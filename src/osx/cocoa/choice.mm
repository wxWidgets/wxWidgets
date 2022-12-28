/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/choice.mm
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"

@interface wxNSPopUpButton : NSPopUpButton
{
}

@end

@implementation wxNSPopUpButton

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (int) intValue
{
   return [self indexOfSelectedItem];
}

- (void) setIntValue: (int) v
{
    [self selectItemAtIndex:v];
}

@end

@interface NSView(PossibleSizeMethods)
- (NSControlSize)controlSize;
@end

class wxChoiceCocoaImpl : public wxWidgetCocoaImpl, public wxChoiceWidgetImpl
{
public:
    wxChoiceCocoaImpl(wxWindowMac *wxpeer, wxNSPopUpButton *v)
    : wxWidgetCocoaImpl(wxpeer, v)
    {
        m_popUpMenu = new wxMenu();
        m_popUpMenu->SetNoEventsMode(true);
        [v setMenu: m_popUpMenu->GetHMenu()];
        [v setAutoenablesItems:NO];

    }

    ~wxChoiceCocoaImpl()
    {
        delete m_popUpMenu;
    }
    
    void InsertItem( size_t pos, int itemid, const wxString& text) override
    {
        m_popUpMenu->Insert( pos, itemid, text );
    }

    size_t GetNumberOfItems() const override
    {
        return m_popUpMenu->GetMenuItemCount();
    }

    void RemoveItem( size_t pos ) override
    {
        m_popUpMenu->Delete( m_popUpMenu->FindItemByPosition( pos ) );
    }

    void SetItem(int pos, const wxString& s) override
    {
        m_popUpMenu->FindItemByPosition( pos )->SetItemLabel( s ) ;
    }

private:
    wxMenu* m_popUpMenu;
};

wxWidgetImplType* wxWidgetImpl::CreateChoice( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    wxMenu* WXUNUSED(menu),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long WXUNUSED(style),
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSPopUpButton* v = [[wxNSPopUpButton alloc] initWithFrame:r pullsDown:NO];
    wxWidgetCocoaImpl* c = new wxChoiceCocoaImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_CHOICE
