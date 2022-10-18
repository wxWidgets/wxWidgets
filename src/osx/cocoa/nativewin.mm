///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/nativewin.mm
// Purpose:     wxNativeWindow implementation for wxOSX/Cocoa
// Author:      Vadim Zeitlin
// Created:     2015-08-01
// Copyright:   (c) 2015 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/nativewin.h"

#include "wx/osx/private.h"

// ============================================================================
// implementation
// ============================================================================

namespace
{

class wxNativeWidgetCocoaImpl : public wxWidgetCocoaImpl
{
public:
    wxNativeWidgetCocoaImpl(wxWindowMac* peer, WXWidget w) : wxWidgetCocoaImpl(peer, w)
    {}

    virtual void SetInitialLabel(const wxString& WXUNUSED(title), wxFontEncoding WXUNUSED(encoding)) override
    {
        // Don't set initial label, because the control was created by the
        // caller and is already fully setup. And some controls (notably
        // NSPathControl) assert if an unexpected string value, such as an empty
        // string, is set.
    }
};

} // anonymous namespace


bool
wxNativeWindow::Create(wxWindow* parent,
                       wxWindowID winid,
                       wxNativeWindowHandle view)
{
    wxCHECK_MSG( view, false, wxS("null NSView pointer") );

    DontCreatePeer();

    if ( !wxWindow::Create(parent, winid) )
        return false;

    // We have to ensure that the internal label is synchronized with the label
    // at the native window, otherwise calling SetLabel() later might not work
    // and, even worse, the native label would be reset to match the (empty) wx
    // label by SetPeer().
    //
    // Notice that the selectors tested here are the same ones currently used
    // by wxWidgetCocoaImpl::SetLabel() and this code would need to be updated
    // if that method is.
    //
    // Also note the casts to "id" needed to suppress the "NSView may not
    // respond to selector" warnings: we do test that it responds to them, so
    // these warnings are not useful here.
    if ( [view respondsToSelector:@selector(title)] )
        m_label = wxCFStringRef::AsString([(id)view title]);
    else if ( [view respondsToSelector:@selector(stringValue)] )
        m_label = wxCFStringRef::AsString([(id)view stringValue]);

    // As wxWidgets will release the view when this object is destroyed, retain
    // it here to avoid destroying the view owned by the user code.
    [view retain];
    SetPeer(new wxNativeWidgetCocoaImpl(this, view));

    // It doesn't seem necessary to use MacPostControlCreate() here as we never
    // change the native control geometry here.

    return true;
}

void wxNativeWindow::DoDisown()
{
    [GetPeer()->GetWXWidget() release];
}
