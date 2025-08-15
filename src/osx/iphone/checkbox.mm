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

    void Move(int x, int y, int width, int height) override
    {
        wxWidgetIPhoneImpl::Move( x, y, width, height );
        m_label->SetPosition( wxPoint(x + 65, y+5) );  // TODO: align properly
    }

    void GetSize( int &width, int &height ) const override
    {
        wxWidgetIPhoneImpl::GetSize( width, height );
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
    return c;
}

#endif
