///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/hyperlink.mm
// Purpose:     wxHyperlinkCtrl for wxOSX
// Author:      Quin Gillespie
// Created:     2026-09-22
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/hyperlink.h"

#include "wx/osx/private.h"

bool wxHyperlinkCtrl::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxString& label,
                             const wxString& url,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    CheckParams(label, url, style);

    DontCreatePeer();

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    SetURL(url.empty() ? label : url);
    m_labelOrig = m_label = label.empty() ? url : label;

    NSRect r = wxOSXGetFrameForControl(this, pos, size);
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    // There is no native hyperlink control, so use a borderless button looking
    // like a link, but still report it as a link to VoiceOver.
    [v setButtonType:NSButtonTypeMomentaryChange];
    [v setBordered:NO];
    [v setAccessibilityRole:NSAccessibilityLinkRole];

    SetPeer(new wxWidgetCocoaImpl(this, v));

    MacPostControlCreate(pos, size);

    // Setting the font and the colour makes the button use an attributed
    // title showing the label as a link.
    wxFont f = GetFont();
    f.SetUnderlined(true);
    SetFont(f);
    SetForegroundColour(GetNormalColour());

    if ( HasFlag(wxHL_CONTEXTMENU) )
        ConnectMenuHandlers();

    return true;
}

void wxHyperlinkCtrl::SetVisited(bool visited)
{
    wxGenericHyperlinkCtrl::SetVisited(visited);

    SetForegroundColour(visited ? GetVisitedColour() : GetNormalColour());
}

bool wxHyperlinkCtrl::SetForegroundColour(const wxColour& colour)
{
    if ( !wxGenericHyperlinkCtrl::SetForegroundColour(colour) )
        return false;

    // Changing the colour of the button doesn't update its title on its own.
    if ( GetPeer() )
        GetPeer()->SetLabel(GetLabelText());

    return true;
}

bool wxHyperlinkCtrl::OSXHandleClicked(double WXUNUSED(timestampsec))
{
    SetVisited();
    SendEvent();

    return true;
}

#endif // wxUSE_HYPERLINKCTRL
