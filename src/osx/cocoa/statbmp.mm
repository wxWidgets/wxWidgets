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

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_STATBMP

#include "wx/statline.h"

#ifndef WX_PRECOMP
#include "wx/statbmp.h"
#endif

#include "wx/osx/private.h"

class wxStaticBitmapCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxStaticBitmapCocoaImpl(wxWindowMac* peer , WXWidget w) :
    wxWidgetCocoaImpl(peer, w)
    {
    }

    ~wxStaticBitmapCocoaImpl()
    {
    }

	void SetLabel( const wxString& title, wxFontEncoding encoding ) wxOVERRIDE
    {
        // although NSControl has this method, NSImageView throws an exception if it is called
    }

    void SetScaleMode(wxStaticBitmap::ScaleMode scaleMode)
    {
        NSImageView* v = (NSImageView*) m_osxView;

        NSImageScaling scaling = NSImageScaleNone;
        switch ( scaleMode )
        {
            case wxStaticBitmap::Scale_Fill:
                scaling = NSImageScaleAxesIndependently;
                break;
            case wxStaticBitmap::Scale_AspectFill:
                scaling = NSImageScaleProportionallyUpOrDown;
                break;
            case wxStaticBitmap::Scale_AspectFit:
                scaling = NSImageScaleProportionallyUpOrDown;
                break;
            default:
                break;
        }
        [v setImageScaling:scaling];
    }
};

void wxStaticBitmap::SetScaleMode(ScaleMode scaleMode)
{
    m_scaleMode = scaleMode;

    dynamic_cast<wxStaticBitmapCocoaImpl*>(GetPeer())->SetScaleMode(scaleMode);

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
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    NSImageView* v = [[NSImageView alloc] initWithFrame:r];

    wxWidgetCocoaImpl* c = new wxStaticBitmapCocoaImpl( wxpeer, v );
    return c;
}

#endif //wxUSE_STATBMP
