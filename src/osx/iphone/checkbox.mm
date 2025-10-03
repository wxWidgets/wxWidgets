/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/checkbox.mm
// Purpose:     wxCheckBox
// Author:      Stefan Csomor
// Created:     2008-08-20
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/osx/private.h"

@interface wxUISwitch : UISwitch
{
}

@end

@implementation wxUISwitch

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

class wxCheckBoxIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxCheckBoxIPhoneImpl(wxWindowMac *wxpeer, UISwitch *v, wxStaticText *l)
    : wxWidgetIPhoneImpl(wxpeer, v)
    {
        m_control = v;
        m_label = l;
    }

    wxInt32  GetValue() const override
    {
        return [m_control isOn] ? 1 : 0;
    }

    void SetValue( wxInt32 v ) override
    {
        [m_control setOn:v != 0 animated:NO];
    }

    // a blog on spacing in iOS
    // https://uxdesign.cc/designing-widgets-for-ios-macos-and-ipados-the-ultimate-guide-737fb284a9df
    const int widthOfSpace = 9;

    void Move(int x, int y, int width, int height) override
    {
        wxWidgetIPhoneImpl::Move( x, y, width, height );

        int checkboxWidth = 0;
        int checkboxHeight = 0;
        // get size of checkbox
        wxWidgetIPhoneImpl::GetSize( checkboxWidth, checkboxHeight );

        // get size of label
        wxSize sz = m_label->GetSize();

        int offset = (checkboxHeight-sz.y) / 2;

        m_label->SetPosition( wxPoint(x + checkboxWidth+widthOfSpace, y+offset) ); 
    }

    void GetSize( int &width, int &height ) const override
    {
        wxWidgetIPhoneImpl::GetSize( width, height );
        wxSize sz = m_label->GetSize();
        width += widthOfSpace + sz.x;
    }

private:
    UISwitch*      m_control;
    wxStaticText*  m_label;
};

wxWidgetImplType* wxWidgetImpl::CreateCheckBox( wxWindowMac* wxpeer,
                                    wxWindowMac* parent,
                                    wxWindowID WXUNUSED(id),
                                    const wxString& label,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUISwitch* v = [[wxUISwitch alloc] initWithFrame:r];

    wxStaticText* text = new wxStaticText( parent, -1, label );

//    if (style & wxCHK_3STATE)
//        [v setAllowsMixedState:YES];

    wxCheckBoxIPhoneImpl* c = new wxCheckBoxIPhoneImpl( wxpeer, v, text );

    c->Move( pos.x, pos.y, size.y, size.y );

    return c;
}

#endif
