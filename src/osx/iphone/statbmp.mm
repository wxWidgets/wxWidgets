/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/statbmp.mm
// Purpose:     wxStaticBitmap
// Author:      Stefan Csomor
// Created:     28.06.99
// Copyright:   (c) 2008 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STATBMP

#include "wx/statline.h"

#ifndef WX_PRECOMP
#include "wx/statbmp.h"
#endif

#include "wx/osx/private.h"

class wxStaticBitmapIPhoneImpl : public wxWidgetIPhoneImpl
{
public :
    wxStaticBitmapIPhoneImpl(wxWindowMac* peer , WXWidget w) :
    wxWidgetIPhoneImpl(peer, w)
    {
    }

    ~wxStaticBitmapIPhoneImpl()
    {
    }

    void SetLabel( const wxString& title, wxFontEncoding encoding ) wxOVERRIDE
    {
        // although NSControl has this method, NSImageView throws an exception if it is called
    }

    void SetScaleMode(wxStaticBitmap::ScaleMode scaleMode)
    {
        UIImageView* v = (UIImageView*) m_osxView;

        UIViewContentMode scaling = UIViewContentModeRedraw;
        switch ( scaleMode )
        {
            case wxStaticBitmap::Scale_Fill:
                scaling = UIViewContentModeScaleToFill;
                break;
            case wxStaticBitmap::Scale_AspectFill:
                scaling =  UIViewContentModeScaleAspectFill;
                break;
            case wxStaticBitmap::Scale_AspectFit:
                scaling =  UIViewContentModeScaleAspectFit;
                break;
            default:
                break;
        }
        [v setContentMode:scaling];
    }
};

void wxStaticBitmap::SetScaleMode(ScaleMode scaleMode)
{
    m_scaleMode = scaleMode;

    dynamic_cast<wxStaticBitmapIPhoneImpl*>(GetPeer())->SetScaleMode(scaleMode);

    Refresh();
}


wxWidgetImplType* wxWidgetImpl::CreateStaticBitmap( wxWindowMac* wxpeer,
                                                   wxWindowMac* WXUNUSED(parent),
                                                   wxWindowID WXUNUSED(id),
                                                   const wxBitmap& bitmap,
                                                   const wxPoint& pos,
                                                   const wxSize& size,
                                                   long WXUNUSED(style),
                                                   long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    UIImageView* v = [[UIImageView alloc] initWithFrame:r];

    wxWidgetIPhoneImpl* c = new wxStaticBitmapIPhoneImpl( wxpeer, v );
    return c;
}

#endif //wxUSE_STATBMP
