///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/tooltip.cpp
// Purpose:     Rich tooltips shown by the ribbon controls
// Author:      Blake Madden
// Created:     2026-10-21
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/tooltip.h"
#include "wx/ribbon/private/tooltip.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/control.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/toplevel.h"
#endif

#include "wx/display.h"
#include "wx/graphics.h"
#include "wx/image.h"
#include "wx/popupwin.h"
#include "wx/time.h"

#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
    #include "wx/generic/hyperlink.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

#include <memory>
#include <string.h>

namespace
{

// Delay before the tooltip is shown when hovering over an item.
constexpr int SHOW_DELAY_MS = 600;
// How often the shown tooltip checks the mouse position and its owner.
constexpr int POLL_INTERVAL_MS = 250;
// How long the mouse may stay away from the item and the tooltip before the
// tooltip is hidden.
constexpr int HIDE_DELAY_MS = 400;
// How long the mouse must stay over another item before the shown tooltip
// switches to it. Without this, moving down from an item to its tooltip
// would switch it to the items in the rows below.
constexpr int SWITCH_DELAY_MS = 150;

// The image is scaled down to fit in this box (in DIPs), but never up.
constexpr int MAX_IMAGE_SIZE = 128;

#if wxUSE_HYPERLINKCTRL
// Size (in DIPs) of the help icon.
constexpr int HELP_ICON_SIZE = 16;
#endif

// Width (in DIPs) at which the description wraps.
constexpr int DESCRIPTION_WIDTH = 260;
constexpr int DESCRIPTION_WIDTH_WITH_IMAGE = 220;

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxRibbonToolTipInfo
// ----------------------------------------------------------------------------

wxString wxRibbonToolTipInfo::GetHelpLinkText() const
{
    return m_helpLinkText.empty() ? _("Click here for more help")
                                  : m_helpLinkText;
}

// ----------------------------------------------------------------------------
// wxRibbonToolTipPopup: the window showing the tooltip contents
// ----------------------------------------------------------------------------

// The popup contains a hyperlink, so it needs wxPU_CONTAINS_CONTROLS, but it
// must never become active, as this would take the focus from the window
// under the ribbon. This is done in the same way as wxSTC calltips.
class wxRibbonToolTipPopup : public wxPopupWindow
{
public:
    wxRibbonToolTipPopup(wxWindow* parent, wxRibbonToolTipHelper* helper)
        : wxPopupWindow(parent, wxPU_CONTAINS_CONTROLS | wxBORDER_NONE),
          m_helper(helper)
    {
        Bind(wxEVT_PAINT, &wxRibbonToolTipPopup::OnPaint, this);
    }

    void SetContent(const wxRibbonToolTipInfo& info,
                    const wxString& helpString);

#ifdef __WXMSW__
    virtual bool Show(bool show = true) override;
    virtual bool MSWHandleMessage(WXLRESULT* result, WXUINT message,
                                  WXWPARAM wParam, WXLPARAM lParam) override;
#endif // __WXMSW__

private:
    void OnPaint(wxPaintEvent& event);

    // Returns the image scaled down to fit in MAX_IMAGE_SIZE.
    wxBitmapBundle GetScaledImage(const wxBitmapBundle& image) const;

#if wxUSE_HYPERLINKCTRL
    void OnHyperlink(wxHyperlinkEvent& event);

    // Returns the help icon: a question mark in a circle, drawn with our own
    // colors, as the stock icon may be hard to see on our background.
    wxBitmapBundle MakeHelpIcon(const wxColour& circleColour,
                                const wxColour& markColour) const;
#endif // wxUSE_HYPERLINKCTRL

    wxRibbonToolTipHelper* const m_helper;
    wxColour m_outlineColour;

    wxDECLARE_NO_COPY_CLASS(wxRibbonToolTipPopup);
};

void wxRibbonToolTipPopup::SetContent(const wxRibbonToolTipInfo& info,
                                      const wxString& helpString)
{
    Freeze();
    DestroyChildren();

    // Use high contrast colours, which are also used in dark mode.
    const bool dark = wxSystemSettings::GetAppearance().IsDark();
    const wxColour bgColour = dark ? *wxBLACK : *wxWHITE;
    const wxColour fgColour = dark ? *wxWHITE : *wxBLACK;
    m_outlineColour = dark ? *wxWHITE : wxColour(64, 64, 64);

    SetBackgroundColour(bgColour);
    SetForegroundColour(fgColour);

    const int padding = FromDIP(8);
    const int gap = FromDIP(6);

    wxBoxSizer* const contents = new wxBoxSizer(wxVERTICAL);

    const wxString& title = info.GetTitle();
    if ( !title.empty() )
    {
        wxStaticText* const titleText = new wxStaticText(this, wxID_ANY, title);
        titleText->SetFont(GetFont().Bold());
        titleText->SetForegroundColour(fgColour);
        titleText->SetBackgroundColour(bgColour);
        contents->Add(titleText);
    }

    const wxString description = info.GetDescription().empty()
                                    ? helpString
                                    : info.GetDescription();
    const wxBitmapBundle& image = info.GetImage();
    if ( !description.empty() || image.IsOk() )
    {
        wxBoxSizer* const row = new wxBoxSizer(wxHORIZONTAL);
        if ( image.IsOk() )
        {
            wxStaticBitmap* const bitmap =
                new wxStaticBitmap(this, wxID_ANY, GetScaledImage(image));
            bitmap->SetBackgroundColour(bgColour);
            row->Add(bitmap, wxSizerFlags().Top());
        }

        if ( !description.empty() )
        {
            wxStaticText* const descText =
                new wxStaticText(this, wxID_ANY, description);
            descText->SetForegroundColour(fgColour);
            descText->SetBackgroundColour(bgColour);
            descText->Wrap(FromDIP(image.IsOk() ? DESCRIPTION_WIDTH_WITH_IMAGE
                                                : DESCRIPTION_WIDTH));
            const int imageGap = image.IsOk() ? gap : 0;
            row->Add(descText, wxSizerFlags().Top().Border(wxLEFT, imageGap));
        }

        // Indent the description below the title.
        const int indent = title.empty() || image.IsOk() ? 0 : gap;
        const int titleGap = title.empty() ? 0 : gap;
        contents->Add(row, wxSizerFlags().Border(wxLEFT, indent)
                                         .Border(wxTOP, titleGap));
    }

#if wxUSE_HYPERLINKCTRL
    if ( info.HasHelpLink() )
    {
        const wxColour linkColour = dark ? wxColour(138, 180, 248)
                                         : wxColour(0, 51, 153);

        // A plain window is used as the divider, instead of wxStaticLine, as
        // its colour can be set on all platforms.
        wxWindow* const divider = new wxWindow(this, wxID_ANY,
                                               wxDefaultPosition,
                                               wxSize(-1, FromDIP(1)));
        divider->SetBackgroundColour(m_outlineColour);
        contents->Add(divider, wxSizerFlags().Expand().Border(wxTOP, gap));

        wxBoxSizer* const row = new wxBoxSizer(wxHORIZONTAL);
        wxStaticBitmap* const helpIcon = new wxStaticBitmap(this, wxID_ANY,
            MakeHelpIcon(linkColour, bgColour));
        helpIcon->SetBackgroundColour(bgColour);
        row->Add(helpIcon, wxSizerFlags().CentreVertical());

        // The URL is never used, as the click is handled below, but it can't
        // be empty.
        wxGenericHyperlinkCtrl* const link =
            new wxGenericHyperlinkCtrl(this, wxID_ANY,
                                       info.GetHelpLinkText(), "help",
                                       wxDefaultPosition, wxDefaultSize,
                                       wxHL_ALIGN_LEFT | wxNO_BORDER);
        link->SetFont(GetFont().Bold());
        link->SetNormalColour(linkColour);
        link->SetHoverColour(linkColour);
        link->SetVisitedColour(linkColour);
        link->SetBackgroundColour(bgColour);
        link->Bind(wxEVT_HYPERLINK, &wxRibbonToolTipPopup::OnHyperlink, this);
        row->Add(link, wxSizerFlags().CentreVertical().Border(wxLEFT, gap));

        contents->Add(row, wxSizerFlags().Border(wxTOP, gap));
    }
#endif // wxUSE_HYPERLINKCTRL

    // Leave room for the outline drawn in OnPaint().
    wxBoxSizer* const outer = new wxBoxSizer(wxVERTICAL);
    outer->Add(contents, wxSizerFlags().Border(wxALL, padding + FromDIP(1)));
    SetSizerAndFit(outer);

    // Fit() only lays out the new children if the size changes, which it
    // doesn't when the same tooltip is shown again.
    Layout();

    Thaw();
    Refresh();
}

wxBitmapBundle
wxRibbonToolTipPopup::GetScaledImage(const wxBitmapBundle& image) const
{
    wxSize size = image.GetDefaultSize();
    if ( size.x <= 0 || size.y <= 0 )
        return image;

    if ( size.x > MAX_IMAGE_SIZE || size.y > MAX_IMAGE_SIZE )
    {
        const double scale = wxMin(double(MAX_IMAGE_SIZE) / size.x,
                                   double(MAX_IMAGE_SIZE) / size.y);
        size.x = wxMax(1, wxRound(size.x * scale));
        size.y = wxMax(1, wxRound(size.y * scale));
    }

    // Get the bitmap at the physical size for this window's DPI, and tell it
    // its scale factor, so that it is shown at the size in DIPs computed
    // above.
    const double scaleFactor = GetDPIScaleFactor();
    wxBitmap bitmap = image.GetBitmap(wxSize(wxRound(size.x * scaleFactor),
                                             wxRound(size.y * scaleFactor)));
    if ( !bitmap.IsOk() )
        return image;
    bitmap.SetScaleFactor(scaleFactor);
    return wxBitmapBundle::FromBitmap(bitmap);
}

#if wxUSE_HYPERLINKCTRL

wxBitmapBundle
wxRibbonToolTipPopup::MakeHelpIcon(const wxColour& circleColour,
                                   const wxColour& markColour) const
{
    const double scaleFactor = GetDPIScaleFactor();
    const int size = wxRound(HELP_ICON_SIZE * scaleFactor);

    // Start with a fully transparent image.
    wxImage image(size, size);
    image.InitAlpha();
    memset(image.GetAlpha(), 0, size * size);

    {
        std::unique_ptr<wxGraphicsContext>
            gc(wxGraphicsContext::Create(image));
        if ( gc == nullptr )
            return wxBitmapBundle();

        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->SetBrush(circleColour);
        gc->DrawEllipse(0, 0, size, size);

        wxFont font = GetFont().Bold();
        font.SetPixelSize(wxSize{ 0, wxRound(size * 0.8) });
        gc->SetFont(font, markColour);

        wxDouble width, height;
        gc->GetTextExtent("?", &width, &height);
        gc->DrawText("?", (size - width) / 2, (size - height) / 2);
    }

    wxBitmap bitmap{ image };
    bitmap.SetScaleFactor(scaleFactor);
    return wxBitmapBundle::FromBitmap(bitmap);
}

void wxRibbonToolTipPopup::OnHyperlink(wxHyperlinkEvent& WXUNUSED(event))
{
    // Don't skip the event, so that the URL is not opened.
    m_helper->OnHelpLinkClicked();
}

#endif // wxUSE_HYPERLINKCTRL

void wxRibbonToolTipPopup::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.SetPen(wxPen(m_outlineColour, FromDIP(1)));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(GetClientRect());
}

#ifdef __WXMSW__

bool wxRibbonToolTipPopup::Show(bool show)
{
    if ( !show )
        return wxPopupWindow::Show(false);

    // Show the window without activating it.
    if ( !wxWindowBase::Show(true) )
        return false;

    const HWND hwnd = GetHwnd();
    ::ShowWindow(hwnd, SW_SHOWNA);
    ::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return true;
}

bool wxRibbonToolTipPopup::MSWHandleMessage(WXLRESULT* result, WXUINT message,
                                            WXWPARAM wParam, WXLPARAM lParam)
{
    // Don't become active when clicked.
    if ( message == WM_MOUSEACTIVATE )
    {
        *result = MA_NOACTIVATE;
        return true;
    }

    return wxPopupWindow::MSWHandleMessage(result, message, wParam, lParam);
}

#endif // __WXMSW__

// ----------------------------------------------------------------------------
// wxRibbonToolTipHelper
// ----------------------------------------------------------------------------

wxRibbonToolTipHelper* wxRibbonToolTipHelper::ms_shown = nullptr;

wxRibbonToolTipHelper::wxRibbonToolTipHelper(wxRibbonControl* owner)
    : m_owner(owner),
      m_timer(this)
{
    Bind(wxEVT_TIMER, &wxRibbonToolTipHelper::OnTimer, this);
    // This timer has no owner, so it sends its events to itself.
    m_switchTimer.Bind(wxEVT_TIMER, &wxRibbonToolTipHelper::OnSwitchTimer,
                       this);
    m_owner->Bind(wxEVT_SHOW, &wxRibbonToolTipHelper::OnOwnerShow, this);
}

wxRibbonToolTipHelper::~wxRibbonToolTipHelper()
{
    Dismiss();

    m_owner->Unbind(wxEVT_SHOW, &wxRibbonToolTipHelper::OnOwnerShow, this);

    if ( m_popup != nullptr )
        m_popup->Destroy();
}

bool wxRibbonToolTipHelper::IsShown() const
{
    return m_popup != nullptr && m_popup->IsShown();
}

void wxRibbonToolTipHelper::ItemHovered(int id,
                                        const wxRect& rect,
                                        const wxRibbonToolTipInfo& info,
                                        const wxString& helpString)
{
    const bool sameItem = m_hasItem && m_itemId == id;
    if ( !sameItem )
        m_suppressed = false;

    m_hasItem = true;
    m_itemId = id;
    m_itemRect = rect;

    if ( m_suppressed )
        return;

    if ( info.IsEmpty() && helpString.empty() )
    {
        // Nothing to show for this item.
        Dismiss();
        return;
    }

    if ( IsShown() )
    {
        if ( m_shownId == id )
        {
            // Back on the shown item. The layout may have changed, so keep
            // the rect up to date.
            m_switchTimer.Stop();
            m_shownRect = rect;
            return;
        }
    }
    else if ( sameItem && m_timer.IsRunning() )
    {
        // Still waiting to show it.
        return;
    }

    m_info = info;
    m_helpString = helpString;

    if ( IsShown() || (ms_shown != nullptr && ms_shown != this) )
    {
        // A tooltip is already shown, so switch to this item soon, without
        // the usual delay, unless the mouse is only passing over it.
        if ( !(sameItem && m_switchTimer.IsRunning()) )
            m_switchTimer.StartOnce(SWITCH_DELAY_MS);
    }
    else
    {
        m_timer.StartOnce(SHOW_DELAY_MS);
    }
}

void wxRibbonToolTipHelper::ItemLeft()
{
    m_suppressed = false;
    m_hasItem = false;
    m_switchTimer.Stop();

    // If the tooltip is shown, the timer hides it unless the mouse goes over
    // it, otherwise just cancel showing it.
    if ( !IsShown() )
        m_timer.Stop();
}

void wxRibbonToolTipHelper::Dismiss(bool suppress)
{
    m_timer.Stop();
    m_switchTimer.Stop();
    UnbindTopLevel();

    if ( m_popup != nullptr && m_popup->IsShown() )
        m_popup->Hide();

    if ( ms_shown == this )
        ms_shown = nullptr;

    m_suppressed = suppress && m_hasItem;
}

/* static */
void wxRibbonToolTipHelper::DismissShown()
{
    if ( ms_shown != nullptr )
        ms_shown->Dismiss(true);
}

void wxRibbonToolTipHelper::OnHelpLinkClicked()
{
    const int id = m_shownId;
    Dismiss(true);

    // Send the event later, as the handler could destroy the owner, and so
    // this helper and the popup, while we're still inside the hyperlink's
    // event handler. If this helper is destroyed before, the call is simply
    // discarded.
    CallAfter([this, id]()
        {
            m_owner->SendToolTipHelpEvent(id);
        });
}

void wxRibbonToolTipHelper::ShowPopup()
{
    if ( !m_owner->IsShownOnScreen() )
    {
        Dismiss();
        return;
    }

    if ( ms_shown != nullptr && ms_shown != this )
        ms_shown->Dismiss();

    if ( m_popup == nullptr )
        m_popup = new wxRibbonToolTipPopup(m_owner, this);

    m_shownId = m_itemId;
    m_shownRect = m_itemRect;

    // Move the popup to the item's monitor before laying it out, so that it
    // uses that monitor's DPI if it's different from the one it was on.
    m_popup->Move(m_owner->ClientToScreen(m_shownRect.GetBottomLeft()));

    m_popup->SetContent(m_info, m_helpString);
    PositionPopup();

    if ( !m_popup->IsShown() )
        m_popup->Show();

    ms_shown = this;
    m_lastInside = wxGetLocalTimeMillis();
    BindTopLevel();
    m_timer.Start(POLL_INTERVAL_MS);
}

void wxRibbonToolTipHelper::PositionPopup()
{
    const wxRect itemRect(m_owner->ClientToScreen(m_shownRect.GetPosition()),
                          m_shownRect.GetSize());
    const wxSize size = m_popup->GetSize();
    const int gap = m_owner->FromDIP(2);

    // Show the tooltip below the ribbon, unless the owner is not shown in it
    // (e.g., in an expanded panel).
    int bottom = m_owner->GetScreenRect().GetBottom();
    wxRibbonBar* const bar = m_owner->GetAncestorRibbonBar();
    if ( bar != nullptr &&
            wxGetTopLevelParent(bar) == wxGetTopLevelParent(m_owner) )
    {
        bottom = wxMax(bottom, bar->GetScreenRect().GetBottom());
    }

    wxPoint pos(itemRect.GetLeft(), bottom + gap);

    const wxRect area = wxDisplay(m_owner).GetClientArea();
    if ( pos.x + size.x > area.GetRight() )
        pos.x = area.GetRight() - size.x;
    if ( pos.x < area.GetLeft() )
        pos.x = area.GetLeft();
    if ( pos.y + size.y > area.GetBottom() )
    {
        // Not enough room below, show it above the item instead.
        pos.y = itemRect.GetTop() - size.y - gap;

        // If there is no room above either, keep it on screen, even if this
        // means covering the ribbon.
        if ( pos.y < area.GetTop() )
            pos.y = wxMax(area.GetTop(), area.GetBottom() - size.y);
    }

    m_popup->Move(pos);
}

bool wxRibbonToolTipHelper::IsMouseOverPopup() const
{
    return m_popup != nullptr && m_popup->IsShown() &&
           m_popup->GetScreenRect().Contains(wxGetMousePosition());
}

bool wxRibbonToolTipHelper::IsMouseOverItemOrPopup() const
{
    if ( IsMouseOverPopup() )
        return true;

    const wxRect itemRect{ m_owner->ClientToScreen(m_shownRect.GetPosition()),
                          m_shownRect.GetSize() };
    return itemRect.Contains(wxGetMousePosition());
}

void wxRibbonToolTipHelper::BindTopLevel()
{
    wxWindow* const topLevel = wxGetTopLevelParent(m_owner);
    if ( topLevel == m_topLevel )
        return;

    UnbindTopLevel();

    m_topLevel = topLevel;
    if ( m_topLevel == nullptr )
        return;

    m_topLevel->Bind(wxEVT_SHOW,
                     &wxRibbonToolTipHelper::OnTopLevelShow, this);
    m_topLevel->Bind(wxEVT_ACTIVATE,
                     &wxRibbonToolTipHelper::OnTopLevelActivate, this);
    m_topLevel->Bind(wxEVT_ICONIZE,
                     &wxRibbonToolTipHelper::OnTopLevelIconize, this);
    m_topLevel->Bind(wxEVT_CHAR_HOOK,
                     &wxRibbonToolTipHelper::OnTopLevelCharHook, this);
}

void wxRibbonToolTipHelper::UnbindTopLevel()
{
    if ( m_topLevel == nullptr )
        return;

    m_topLevel->Unbind(wxEVT_SHOW,
                       &wxRibbonToolTipHelper::OnTopLevelShow, this);
    m_topLevel->Unbind(wxEVT_ACTIVATE,
                       &wxRibbonToolTipHelper::OnTopLevelActivate, this);
    m_topLevel->Unbind(wxEVT_ICONIZE,
                       &wxRibbonToolTipHelper::OnTopLevelIconize, this);
    m_topLevel->Unbind(wxEVT_CHAR_HOOK,
                       &wxRibbonToolTipHelper::OnTopLevelCharHook, this);
    m_topLevel = nullptr;
}

void wxRibbonToolTipHelper::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if ( !IsShown() )
    {
        // The show delay has expired.
        if ( m_hasItem && !m_suppressed )
            ShowPopup();
        return;
    }

    // The owner, or one of its parents, may have been hidden without any
    // event being sent to us (e.g., when switching the ribbon pages).
    if ( !m_owner->IsShownOnScreen() )
    {
        Dismiss();
        return;
    }

    const wxLongLong now = wxGetLocalTimeMillis();
    if ( IsMouseOverItemOrPopup() )
        m_lastInside = now;
    else if ( now - m_lastInside >= HIDE_DELAY_MS )
        Dismiss();
}

void wxRibbonToolTipHelper::OnSwitchTimer(wxTimerEvent& WXUNUSED(event))
{
    if ( !m_hasItem || m_suppressed )
        return;

    // Don't switch if the mouse has reached the shown tooltip, which may
    // belong to another control.
    if ( ms_shown != nullptr && ms_shown->IsMouseOverPopup() )
        return;

    ShowPopup();
}

void wxRibbonToolTipHelper::OnOwnerShow(wxShowEvent& event)
{
    event.Skip();

    if ( !event.IsShown() )
        Dismiss();
}

void wxRibbonToolTipHelper::OnTopLevelShow(wxShowEvent& event)
{
    event.Skip();

    if ( !event.IsShown() )
        Dismiss();
}

void wxRibbonToolTipHelper::OnTopLevelActivate(wxActivateEvent& event)
{
    event.Skip();

    if ( !event.GetActive() )
    {
        // Under macOS, clicking the popup makes it the key window and so
        // deactivates the frame. Keep the popup open so that the click can
        // reach the help link.
        if ( IsMouseOverPopup() )
            return;

        Dismiss();
    }
}

void wxRibbonToolTipHelper::OnTopLevelIconize(wxIconizeEvent& event)
{
    event.Skip();

    if ( event.IsIconized() )
        Dismiss();
}

void wxRibbonToolTipHelper::OnTopLevelCharHook(wxKeyEvent& event)
{
    // Any key, including Alt showing the KeyTips, hides the tooltip.
    event.Skip();

    Dismiss(true);
}

#endif // wxUSE_RIBBON
