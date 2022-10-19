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

private:
};

wxWidgetImplType* wxWidgetImpl::CreateChoice( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    wxMenu* menu,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long WXUNUSED(style),
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUIPickerView* v = [[wxUIPickerView alloc] initWithFrame:r];

    wxChoiceIPhoneImpl* c = new wxChoiceIPhoneImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_CHOICE
