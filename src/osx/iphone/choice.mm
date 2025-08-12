/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/choice.mm
// Purpose:     wxChoice
// Author:      Stefan Csomor
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
#include "wx/log.h"

// --------------------- UIPickerView implementation ----------------------


@interface wxUIPickerView : UIPickerView<UIPickerViewDelegate,UIPickerViewDataSource>
{
}

@property (readonly) NSMutableArray *rows;

@end

@implementation wxUIPickerView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (id)initWithFrame:(CGRect)rectBox
{
    if ( self = [super initWithFrame:rectBox] )
    {
        _rows = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)dealloc
{
    [_rows dealloc];
    [super dealloc];
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    if ( component == 0 )
        return [_rows count];

    return 0;
}

- (nullable NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [_rows objectAtIndex:row];
}

- (int) intValue
{
   return (int) [self selectedRowInComponent:0];
}

- (void) setIntValue: (int) v
{
    [self selectRow:v inComponent:0 animated:NO];
}

- (void) pickerView: (UIPickerView *)pickerView didSelectRow: (NSInteger)row inComponent: (NSInteger)component
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( pickerView );
    wxChoice *choice = (wxChoice*) impl->GetWXPeer();
    choice->SendSelectionChangedEvent(wxEVT_CHOICE);
}


@end

class wxChoiceIPhoneImpl : public wxWidgetIPhoneImpl, public wxChoiceWidgetImpl
{
public:
    wxChoiceIPhoneImpl(wxWindowMac *wxpeer, wxUIPickerView *v)
    : wxWidgetIPhoneImpl(wxpeer, v)
    {
        [v setDelegate:v];
        [v setDataSource:v];
    }

    void InsertItem( size_t pos, int itemid, const wxString& text) override
    {
        wxCFStringRef cftext(text);
        [((wxUIPickerView*)m_osxView).rows insertObject:cftext.AsNSString() atIndex:pos];
    }

    size_t GetNumberOfItems() const override
    {
        return [((wxUIPickerView*)m_osxView).rows count];
    }

    void RemoveItem( size_t pos ) override
    {
        [((wxUIPickerView*)m_osxView).rows removeObjectAtIndex:pos];
    }

    void SetItem(int pos, const wxString& text) override
    {
        wxCFStringRef cftext(text);
        [((wxUIPickerView*)m_osxView).rows replaceObjectAtIndex:pos withObject:cftext.AsNSString()];
    }

    wxInt32 GetValue() const override
    {
        return [((wxUIPickerView*)m_osxView) selectedRowInComponent:0 ];
    }

private:
};

// --------------------- UISegmentedControl implementation ----------------------


@interface wxUISegmentedControl : UISegmentedControl
{
}

@end

@implementation wxUISegmentedControl

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

@end


class wxSegmentedChoiceIPhoneImpl : public wxWidgetIPhoneImpl, public wxChoiceWidgetImpl
{
public:
    wxSegmentedChoiceIPhoneImpl(wxWindowMac *wxpeer, wxUISegmentedControl *v)
    : wxWidgetIPhoneImpl(wxpeer, v)
    {
    }

    void InsertItem( size_t pos, int itemid, const wxString& text) override
    {
        wxCFStringRef cftext(text);
        [((wxUISegmentedControl*)m_osxView) insertSegmentWithTitle:cftext.AsNSString() atIndex:pos animated:NO];
        [((wxUISegmentedControl*)m_osxView) setEnabled: YES forSegmentAtIndex: pos ];
    }

    size_t GetNumberOfItems() const override
    {
        int value = [((wxUISegmentedControl*)m_osxView) numberOfSegments];
        wxLogMessage( "GetNumberOfItems()", value );
        return [((wxUISegmentedControl*)m_osxView) numberOfSegments];
    }

    void RemoveItem( size_t pos ) override
    {
        [((wxUISegmentedControl*)m_osxView) removeSegmentAtIndex:pos animated:NO];
    }

    void SetItem(int pos, const wxString& text) override
    {
        wxCFStringRef cftext(text);
        [((wxUISegmentedControl*)m_osxView) setTitle: cftext.AsNSString() forSegmentAtIndex: pos ];
    }

    wxInt32 GetValue() const override
    {
        return [((wxUISegmentedControl*)m_osxView) selectedSegmentIndex ];
    }

    void SetValue( wxInt32 value ) override
    {
        return [((wxUISegmentedControl*)m_osxView) setSelectedSegmentIndex: value ];
    }


private:
};

wxWidgetImplType* wxWidgetImpl::CreateChoice( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    wxMenu* menu,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;

    wxWidgetImplType *c = NULL;

    if ((style & wxCH_BUTTONS) != 0) {
        wxUISegmentedControl* v = [[wxUISegmentedControl alloc] initWithFrame:r];
        // [v setMomentary: NO];
        c = new wxSegmentedChoiceIPhoneImpl( wxpeer, v );
    } else { 
        wxUIPickerView* v = [[wxUIPickerView alloc] initWithFrame:r];
        c = new wxChoiceIPhoneImpl( wxpeer, v );
    }
    return c;
}

#endif // wxUSE_CHOICE
