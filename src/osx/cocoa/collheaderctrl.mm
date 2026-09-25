///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/collheaderctrl.mm
// Purpose:     wxCollapsibleHeaderCtrl for wxOSX
// Author:      Quin Gillespie
// Created:     2026-09-22
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COLLPANE

#include "wx/collheaderctrl.h"

#include "wx/osx/private.h"

bool wxCollapsibleHeaderCtrl::Create(wxWindow *parent,
                                     wxWindowID id,
                                     const wxString& label,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    DontCreatePeer();

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_labelOrig = m_label = label;

    NSRect r = wxOSXGetFrameForControl(this, pos, size);
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    // The native disclosure triangle can't show any label, so use a toggle
    // button showing a chevron before its label instead, but still report it
    // as a disclosure triangle to VoiceOver, so that it announces its state.
    [v setButtonType:NSButtonTypeToggle];
    [v setBordered:NO];
    [v setImagePosition:NSImageLeft];
    [v setImage:wxOSXGetSystemImage("chevron.right")];
    [v setAlternateImage:wxOSXGetSystemImage("chevron.down")];
    [v setAccessibilityRole:NSAccessibilityDisclosureTriangleRole];

    SetPeer(new wxWidgetCocoaImpl(this, v));

    MacPostControlCreate(pos, size);

    return true;
}

void wxCollapsibleHeaderCtrl::SetCollapsed(bool collapsed)
{
    GetPeer()->SetValue(collapsed ? 0 : 1);
}

bool wxCollapsibleHeaderCtrl::IsCollapsed() const
{
    return GetPeer()->GetValue() == 0;
}

bool wxCollapsibleHeaderCtrl::OSXHandleClicked(double WXUNUSED(timestampsec))
{
    wxCommandEvent event(wxEVT_COLLAPSIBLEHEADER_CHANGED, GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    return true;
}

#endif // wxUSE_COLLPANE
