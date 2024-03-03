/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#include "wx/msw/darkmode.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/winstyle.h"

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("STATIC"), wxEmptyString, pos, size) )
        return false;

    // we set the label here and not through MSWCreateControl() because we
    // need to do many operation on it for ellipsization&markup support
    SetLabel(label);

    // as we didn't pass the correct label to MSWCreateControl(), it didn't set
    // the initial size correctly -- do it now
    SetInitialSize(size);

    // NOTE: if the label contains ampersand characters which are interpreted as
    //       accelerators, they will be rendered (at least on WinXP) only if the
    //       static text is placed inside a window class which correctly handles
    //       focusing by TAB traversal (e.g. wxPanel).

    return true;
}

WXDWORD wxStaticText::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // translate the alignment flags to the Windows ones
    //
    // note that both wxALIGN_LEFT and SS_LEFT are equal to 0 so we shouldn't
    // test for them using & operator
    if ( style & wxALIGN_CENTRE_HORIZONTAL )
        msStyle |= SS_CENTER;
    else if ( style & wxALIGN_RIGHT )
        msStyle |= SS_RIGHT;
    else
        msStyle |= SS_LEFT;

#ifdef SS_ENDELLIPSIS
    // for now, add the SS_ENDELLIPSIS style if wxST_ELLIPSIZE_END is given;
    // we may need to remove it later in ::SetLabel() if the given label
    // has newlines
    if ( style & wxST_ELLIPSIZE_END )
        msStyle |= SS_ENDELLIPSIS;
#endif // SS_ENDELLIPSIS

    // this style is necessary to receive mouse events
    msStyle |= SS_NOTIFY;

    return msStyle;
}

wxSize wxStaticText::DoGetBestClientSize() const
{
    wxClientDC dc(const_cast<wxStaticText *>(this));
    wxFont font(GetFont());
    if (!font.IsOk())
        font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    dc.SetFont(font);

    wxCoord widthTextMax, heightTextTotal;
    dc.GetMultiLineTextExtent(GetLabelText(), &widthTextMax, &heightTextTotal);

    // This extra pixel is a hack we use to ensure that a wxStaticText
    // vertically centered around the same position as a wxTextCtrl shows its
    // text on exactly the same baseline. It is not clear why is this needed
    // nor even whether this works in all cases, but it does work, at least
    // with the default fonts, under Windows XP, 7 and 8, so just use it for
    // now.
    //
    // In the future we really ought to provide a way for each of the controls
    // to provide information about the position of the baseline for the text
    // it shows and use this information in the sizer code when centering the
    // controls vertically, otherwise we simply can't ensure that the text is
    // always on the same line, e.g. even with this hack wxComboBox text is
    // still not aligned to the same position.
    heightTextTotal += 1;

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    // Keep track of the size before so we can see if it changed
    const wxSize sizeBefore = GetSize();

    // note: we first need to set the size and _then_ call UpdateLabel
    wxStaticTextBase::DoSetSize(x, y, w, h, sizeFlags);

    // Avoid flicker by not refreshing or updating the label if the size didn't
    // change.
    if ( sizeBefore == GetSize() )
        return;

#ifdef SS_ENDELLIPSIS
    // do we need to ellipsize the contents?
    long styleReal = ::GetWindowLong(GetHwnd(), GWL_STYLE);
    if ( !(styleReal & SS_ENDELLIPSIS) )
    {
        // we don't have SS_ENDELLIPSIS style:
        // we need to (eventually) do ellipsization ourselves
        UpdateLabel();
    }
    //else: we don't or the OS will do it for us
#endif // SS_ENDELLIPSIS

    // we need to refresh the window after changing its size as the standard
    // control doesn't always update itself properly
    Refresh();
}

bool
wxStaticText::MSWHandleMessage(WXLRESULT *result,
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam)
{
    if ( wxStaticTextBase::MSWHandleMessage(result, message, wParam, lParam) )
        return true;

    switch ( message )
    {
        case WM_PAINT:
            // We only customize drawing of disabled labels in dark mode.
            if ( IsEnabled() || !wxMSWDarkMode::IsActive() )
                break;

            // For them, the default "greying out" of the text for the disabled
            // controls looks ugly and unreadable in dark mode, so we draw it
            // as normal text but use a different colour for it.
            //
            // We could alternatively make the control owner-drawn, which would
            // be slightly cleaner, but would require more effort.
            wxMSWWinStyleUpdater updateStyle(GetHwnd());
            updateStyle.TurnOff(WS_DISABLED).Apply();

            // Don't use Get/SetForegroundColour() here as they do more than we
            // need, we just want to change m_foregroundColour temporarily
            // without any side effects.
            const auto colFgOrig = m_foregroundColour;
            wxDarkModeSettings darkModeSettings;
            m_foregroundColour = darkModeSettings.GetMenuColour(wxMenuColour::DisabledFg);

            *result = MSWDefWindowProc(WM_PAINT, wParam, lParam);

            updateStyle.TurnOn(WS_DISABLED).Apply();
            if ( m_hasFgCol )
                m_foregroundColour = colFgOrig;

            return true;
    }

    return false;
}

void wxStaticText::SetLabel(const wxString& label)
{
    // If the label doesn't really change, avoid flicker by not doing anything.
    if ( label == m_labelOrig )
        return;

#ifdef SS_ENDELLIPSIS
    wxMSWWinStyleUpdater updateStyle(GetHwnd());
    if ( HasFlag(wxST_ELLIPSIZE_END) )
    {
        // adding SS_ENDELLIPSIS or SS_ENDELLIPSIS "disables" the correct
        // newline handling in static texts: the newlines in the labels are
        // shown as square. Thus we don't use it even on newer OS when
        // the static label contains a newline.
        updateStyle.TurnOnOrOff(!label.Contains(wxT('\n')), SS_ENDELLIPSIS);
    }
    else // style not supported natively
    {
        updateStyle.TurnOff(SS_ENDELLIPSIS);
    }

    updateStyle.Apply();
#endif // SS_ENDELLIPSIS

    // save the label in m_labelOrig with both the markup (if any) and
    // the mnemonics characters (if any)
    m_labelOrig = label;

#ifdef SS_ENDELLIPSIS
    if ( updateStyle.IsOn(SS_ENDELLIPSIS) )
        WXSetVisibleLabel(GetLabel());
    else
#endif // SS_ENDELLIPSIS
        WXSetVisibleLabel(GetEllipsizedLabel());

    AutoResizeIfNecessary();
}

bool wxStaticText::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return false;

    AutoResizeIfNecessary();

    return true;
}

// These functions are used by wxST_ELLIPSIZE_* supporting code in
// wxStaticTextBase which requires us to implement them, but actually the base
// wxWindow methods already do exactly what we need under this platform.

wxString wxStaticText::WXGetVisibleLabel() const
{
    return wxWindow::GetLabel();
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    wxWindow::SetLabel(str);
}


#endif // wxUSE_STATTEXT
