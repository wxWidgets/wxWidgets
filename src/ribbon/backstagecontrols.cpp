///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/backstagecontrols.cpp
// Purpose:     Controls for the pages of a wxBackstage
// Author:      Blake Madden
// Created:     2026-09-24
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RIBBON

#include "wx/ribbon/backstagecontrols.h"
#include "wx/ribbon/private/backstage.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/dcclient.h"
    #include "wx/log.h"
#endif

#include "wx/access.h"
#include "wx/dcbuffer.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

#include <algorithm>
#include <utility>

wxDEFINE_EVENT(wxEVT_BACKSTAGE_ITEM_CLICKED, wxCommandEvent);

//-------------------------------------------
// wxBackstageButton
//-------------------------------------------
#if wxUSE_ACCESSIBILITY
//-------------------------------------------
// Exposes the button to assistive technology.
class wxBackstageButton::Accessible final : public wxAccessible
{
public:
    explicit Accessible(wxBackstageButton* button) : wxAccessible(button), m_button(button)
    {
    }

    wxAccStatus GetName(const int, wxString* name) override
    {
        *name = m_button->GetLabel();
        name->Replace("\n", " ");
        return wxACC_OK;
    }

    wxAccStatus GetDescription(const int, wxString* description) override
    {
        *description = m_button->m_description;
        description->Replace("\n", " ");
        return wxACC_OK;
    }

    wxAccStatus GetRole(const int, wxAccRole* role) override
    {
        *role = wxROLE_SYSTEM_PUSHBUTTON;
        return wxACC_OK;
    }

    wxAccStatus GetState(const int, long* state) override
    {
        if ( !m_button->IsEnabled() )
        {
            *state = wxACC_STATE_SYSTEM_UNAVAILABLE;
            return wxACC_OK;
        }
        *state = wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( m_button->HasFocus() )
        {
            *state |= wxACC_STATE_SYSTEM_FOCUSED;
        }
        if ( m_button->m_pressed )
        {
            *state |= wxACC_STATE_SYSTEM_PRESSED;
        }
        if ( m_button->m_hover )
        {
            *state |= wxACC_STATE_SYSTEM_HOTTRACKED;
        }
        return wxACC_OK;
    }

    wxAccStatus GetDefaultAction(const int, wxString* actionName) override
    {
        *actionName = _("Press");
        return wxACC_OK;
    }

    wxAccStatus DoDefaultAction(const int) override
    {
        if ( !m_button->IsEnabled() )
        {
            return wxACC_NOT_SUPPORTED;
        }
        m_button->Activate();
        return wxACC_OK;
    }

private:
    wxBackstageButton* m_button{ nullptr };
};
#endif

wxBackstageButton::wxBackstageButton(wxWindow* parent, wxWindowID id, const wxString& label,
    const wxBitmapBundle& icon /*= wxBitmapBundle{}*/,
    const wxBackstageButtonStyle style /*= wxBackstageButtonStyle::Tile*/,
    wxString description /*= wxString{}*/)
    : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS,
        wxDefaultValidator, "wxBackstageButton"),
    m_icon(icon), m_description(std::move(description)), m_style(style)
{
    m_iconSizeDIP = (style == wxBackstageButtonStyle::Card) ?
        wxSize{ 180, 110 } : wxSize{ 32, 32 };

    wxControl::SetLabel(label);
    wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindow::SetCanFocus(true);

    Bind(wxEVT_PAINT, &wxBackstageButton::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &wxBackstageButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &wxBackstageButton::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &wxBackstageButton::OnMouseDown, this);
    Bind(wxEVT_LEFT_DCLICK, &wxBackstageButton::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &wxBackstageButton::OnMouseUp, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxBackstageButton::OnMouseCaptureLost, this);
    Bind(wxEVT_KEY_DOWN, &wxBackstageButton::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &wxBackstageButton::OnFocusChange, this);
    Bind(wxEVT_KILL_FOCUS, &wxBackstageButton::OnFocusChange, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED, &wxBackstageButton::OnSysColourChanged, this);

#if wxUSE_ACCESSIBILITY
    SetAccessible(new Accessible{ this });
#endif
    SetInitialSize(wxDefaultSize);
}

//-------------------------------------------
void wxBackstageButton::ResetBestSize()
{
    InvalidateBestSize();
    Refresh();
}

//-------------------------------------------
void wxBackstageButton::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    ResetBestSize();
}

//-------------------------------------------
void wxBackstageButton::SetDescription(const wxString& description)
{
    m_description = description;
    ResetBestSize();
}

//-------------------------------------------
void wxBackstageButton::SetIcon(const wxBitmapBundle& icon)
{
    m_icon = icon;
    ResetBestSize();
}

//-------------------------------------------
void wxBackstageButton::SetIconSize(const wxSize& size)
{
    m_iconSizeDIP = size;
    ResetBestSize();
}

//-------------------------------------------
void wxBackstageButton::ShowDropDownArrow(const bool show /*= true*/)
{
    m_dropDownArrow = show;
    ResetBestSize();
}

//-------------------------------------------
void wxBackstageButton::SetCalloutColour(const wxColour& colour)
{
    if ( m_calloutColour == colour )
    {
        return;
    }
    m_calloutColour = colour;
    Refresh();
}

//-------------------------------------------
wxString wxBackstageButton::GetDisplayLabel() const
{
    return m_dropDownArrow ? GetLabel() + wxString::FromUTF8(" ▾") : GetLabel();
}

//-------------------------------------------
wxSize wxBackstageButton::DoGetBestSize() const
{
    const wxCoord pad = FromDIP(8);
    const wxCoord gap = FromDIP(6);
    const wxSize iconSize = FromDIP(m_iconSizeDIP);
    const bool hasIcon = m_icon.IsOk();
    const wxSize labelSize = wxBackstageHelpers::MeasureText(this, GetDisplayLabel(),
        m_style == wxBackstageButtonStyle::Tile ? GetFont() : GetFont().Bold());
    const wxSize descSize = wxBackstageHelpers::MeasureText(this, m_description, GetFont());

    switch ( m_style )
    {
    case wxBackstageButtonStyle::Wide:
        {
            const wxCoord textHeight = labelSize.GetHeight() + descSize.GetHeight() +
                (descSize.GetHeight() > 0 ? gap / 2 : 0);
            return wxSize{ pad + (hasIcon ? iconSize.GetWidth() + (2 * gap) : 0) +
                               std::max(labelSize.GetWidth(), descSize.GetWidth()) + pad,
                           (2 * pad) + std::max(hasIcon ? iconSize.GetHeight() : 0, textHeight) };
        }
    case wxBackstageButtonStyle::Card:
        return wxSize{ std::max({ iconSize.GetWidth(), labelSize.GetWidth(),
                                  descSize.GetWidth() }) + (2 * pad),
                       pad + iconSize.GetHeight() + gap + labelSize.GetHeight() +
                           descSize.GetHeight() + pad };
    case wxBackstageButtonStyle::Tile:
    default:
        return wxSize{ std::max(std::max(hasIcon ? iconSize.GetWidth() : 0,
                                         labelSize.GetWidth()) + (2 * pad), FromDIP(80)),
                       pad + (hasIcon ? iconSize.GetHeight() + gap : 0) +
                           labelSize.GetHeight() + pad };
    }
}

//-------------------------------------------
void wxBackstageButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxBackstagePaintGraphics paint{ this, GetFont() };
    wxDC& dc = paint.m_graphics;
    const wxColour bg = paint.m_background;
    const wxColour fg = paint.m_foreground;

    const wxRect rect = GetClientRect();
    const wxCoord pad = FromDIP(8);
    const wxCoord gap = FromDIP(6);
    const wxSize iconSize = FromDIP(m_iconSizeDIP);
    const bool isCard = (m_style == wxBackstageButtonStyle::Card);
    const bool enabled = IsEnabled();

    wxColour fill = m_calloutColour.IsOk() ? m_calloutColour : bg;
    const bool highlighted = enabled && (m_pressed || m_hover);
    if ( highlighted && !isCard )
    {
        fill = wxBackstageHelpers::ShadeOrTint(fill, m_pressed ? 0.22 : 0.10);
    }
    wxColour textColour = wxBackstageHelpers::BlackOrWhiteContrast(fill);
    wxColour dimColour = wxBackstageHelpers::Blend(textColour, fill, 0.35);
    if ( !enabled )
    {
        textColour = wxBackstageHelpers::Blend(textColour, fill, 0.55);
        dimColour = wxBackstageHelpers::Blend(dimColour, fill, 0.55);
    }
    const wxColour frameColour = wxBackstageHelpers::Blend(bg, fg, 0.25);

    dc.SetTextForeground(textColour);

    switch ( m_style )
    {
    case wxBackstageButtonStyle::Tile:
        {
            {
                const wxDCPenChanger pc{ dc,
                    wxPen{ highlighted ? wxBackstageHelpers::Blend(bg, fg, 0.55) : frameColour, 1 } };
                const wxDCBrushChanger bc{ dc, wxBrush{ fill } };
                dc.DrawRectangle(rect.Deflate(0, 0));
            }
            wxCoord y = rect.GetTop() + pad;
            if ( m_icon.IsOk() )
            {
                wxBackstageHelpers::DrawBitmapFit(dc, this, m_icon,
                    wxRect{ rect.GetLeft() + ((rect.GetWidth() - iconSize.GetWidth()) / 2),
                            y, iconSize.GetWidth(), iconSize.GetHeight() });
                y += iconSize.GetHeight() + gap;
            }
            dc.DrawLabel(GetDisplayLabel(),
                wxRect{ rect.GetLeft() + pad, y, rect.GetWidth() - (2 * pad),
                        rect.GetBottom() - y },
                wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP);
        }
        break;
    case wxBackstageButtonStyle::Wide:
        {
            {
                const wxDCPenChanger pc{ dc,
                    (highlighted || m_calloutColour.IsOk()) ?
                        wxPen{ wxBackstageHelpers::Blend(bg, fg, 0.3), 1 } : *wxTRANSPARENT_PEN };
                const wxDCBrushChanger bc{ dc, wxBrush{ fill } };
                dc.DrawRectangle(rect);
            }
            wxCoord textX = rect.GetLeft() + pad;
            if ( m_icon.IsOk() )
            {
                wxBackstageHelpers::DrawBitmapFit(dc, this, m_icon,
                    wxRect{ textX,
                            rect.GetTop() + ((rect.GetHeight() - iconSize.GetHeight()) / 2),
                            iconSize.GetWidth(), iconSize.GetHeight() });
                textX += iconSize.GetWidth() + (2 * gap);
            }
            const wxFont titleFont = GetFont().Bold();
            const wxSize titleSize = wxBackstageHelpers::MeasureText(this, GetDisplayLabel(), titleFont);
            const wxSize descSize = wxBackstageHelpers::MeasureText(this, m_description, GetFont());
            const wxCoord blockHeight = titleSize.GetHeight() + descSize.GetHeight() +
                (descSize.GetHeight() > 0 ? gap / 2 : 0);
            wxCoord y = rect.GetTop() + ((rect.GetHeight() - blockHeight) / 2);
            {
                const wxDCFontChanger fc{ dc, titleFont };
                dc.DrawLabel(GetDisplayLabel(),
                    wxRect{ textX, y, rect.GetRight() - textX, titleSize.GetHeight() },
                    wxALIGN_LEFT | wxALIGN_TOP);
            }
            if ( !m_description.empty() )
            {
                y += titleSize.GetHeight() + (gap / 2);
                const wxDCTextColourChanger tcc{ dc, dimColour };
                dc.DrawLabel(m_description,
                    wxRect{ textX, y, rect.GetRight() - textX, descSize.GetHeight() },
                    wxALIGN_LEFT | wxALIGN_TOP);
            }
        }
        break;
    case wxBackstageButtonStyle::Card:
        {
            const wxRect thumbRect{ rect.GetLeft() + pad, rect.GetTop() + pad,
                                    rect.GetWidth() - (2 * pad), iconSize.GetHeight() };
            {
                const wxColour accent = wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT);
                const wxDCPenChanger pc{ dc,
                    wxPen{ highlighted ? accent : frameColour,
                           highlighted ? std::max(2, FromDIP(2)) : 1 } };
                const wxDCBrushChanger bc{ dc, wxBrush{ m_pressed && enabled ?
                    wxBackstageHelpers::ShadeOrTint(bg, 0.12) : bg } };
                dc.DrawRectangle(thumbRect);
            }
            wxBackstageHelpers::DrawBitmapFit(dc, this, m_icon, thumbRect.Deflate(FromDIP(2), FromDIP(2)));

            const wxFont titleFont = GetFont().Bold();
            const wxSize titleSize = wxBackstageHelpers::MeasureText(this, GetDisplayLabel(), titleFont);
            wxCoord y = thumbRect.GetBottom() + gap;
            {
                const wxDCFontChanger fc{ dc, titleFont };
                dc.DrawLabel(GetDisplayLabel(),
                    wxRect{ thumbRect.GetLeft(), y, thumbRect.GetWidth(), titleSize.GetHeight() },
                    wxALIGN_LEFT | wxALIGN_TOP);
            }
            if ( !m_description.empty() )
            {
                y += titleSize.GetHeight();
                const wxDCTextColourChanger tcc{ dc, dimColour };
                dc.DrawLabel(m_description,
                    wxRect{ thumbRect.GetLeft(), y, thumbRect.GetWidth(),
                            rect.GetBottom() - y },
                    wxALIGN_LEFT | wxALIGN_TOP);
            }
        }
        break;
    }

    if ( HasFocus() && m_showFocusRect )
    {
        wxBackstageHelpers::DrawFocusRect(dc, rect.Deflate(FromDIP(2), FromDIP(2)), textColour);
    }
}

//-------------------------------------------
void wxBackstageButton::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
{
    m_hover = true;
    Refresh();
}

//-------------------------------------------
void wxBackstageButton::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
    m_hover = false;
    Refresh();
}

//-------------------------------------------
void wxBackstageButton::OnMouseDown(wxMouseEvent& WXUNUSED(event))
{
    SetFocus();
    m_showFocusRect = false;
    m_pressed = true;
    if ( !HasCapture() )
    {
        CaptureMouse();
    }
    Refresh();
}

//-------------------------------------------
void wxBackstageButton::OnMouseUp(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        ReleaseMouse();
    }
    const bool wasPressed = m_pressed;
    m_pressed = false;
    Refresh();
    if ( wasPressed && GetClientRect().Contains(event.GetPosition()) )
    {
        Activate();
    }
}

//-------------------------------------------
void wxBackstageButton::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    m_pressed = false;
    Refresh();
}

//-------------------------------------------
void wxBackstageButton::OnKeyDown(wxKeyEvent& event)
{
    if ( wxBackstageHelpers::SkipIfShortcutKey(event) )
    {
        return;
    }
    if ( !m_showFocusRect )
    {
        m_showFocusRect = true;
        Refresh();
    }
    if ( wxBackstageHelpers::IsActivateKey(event.GetKeyCode()) )
    {
        if ( !event.IsAutoRepeat() )
        {
            Activate();
        }
    }
    else if ( event.GetKeyCode() == WXK_TAB )
    {
        Navigate(event.ShiftDown() ?
            wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
    }
    else
    {
        event.Skip();
    }
}

//-------------------------------------------
void wxBackstageButton::OnFocusChange(wxFocusEvent& event)
{
    m_showFocusRect = (event.GetEventType() == wxEVT_SET_FOCUS);
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstageButton::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstageButton::Activate()
{
    if ( !IsEnabled() )
    {
        return;
    }
    wxCommandEvent event{ wxEVT_BUTTON, GetId() };
    event.SetEventObject(this);
    event.SetString(GetLabel());
    ProcessWindowEvent(event);
}

//-------------------------------------------
// wxBackstageHeading
//-------------------------------------------
#if wxUSE_ACCESSIBILITY
//-------------------------------------------
// Exposes the heading to assistive technology.
class wxBackstageHeading::Accessible final : public wxAccessible
{
public:
    explicit Accessible(wxBackstageHeading* heading) : wxAccessible(heading), m_heading(heading)
    {
    }

    wxAccStatus GetName(const int, wxString* name) override
    {
        *name = m_heading->GetLabel();
        name->Replace("\n", " ");
        return wxACC_OK;
    }

    wxAccStatus GetRole(const int, wxAccRole* role) override
    {
        *role = wxROLE_SYSTEM_STATICTEXT;
        return wxACC_OK;
    }

    wxAccStatus GetState(const int, long* state) override
    {
        *state = wxACC_STATE_SYSTEM_READONLY;
        return wxACC_OK;
    }

private:
    wxBackstageHeading* m_heading{ nullptr };
};
#endif

wxBackstageHeading::wxBackstageHeading(wxWindow* parent, wxWindowID id, const wxString& label,
    const wxBackstageHeadingStyle style /*= wxBackstageHeadingStyle::Title*/)
    : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE,
        wxDefaultValidator, "wxBackstageHeading"),
    m_style(style)
{
    wxControl::SetLabel(label);
    wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &wxBackstageHeading::OnPaint, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED, &wxBackstageHeading::OnSysColourChanged, this);

#if wxUSE_ACCESSIBILITY
    SetAccessible(new Accessible{ this });
#endif
    SetInitialSize(wxDefaultSize);
}

//-------------------------------------------
wxFont wxBackstageHeading::GetHeadingFont() const
{
    return m_style == wxBackstageHeadingStyle::Title ?
        GetFont().Scaled(2.2F) : GetFont().Scaled(1.25F).Bold();
}

//-------------------------------------------
void wxBackstageHeading::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    Refresh();
}

//-------------------------------------------
wxSize wxBackstageHeading::DoGetBestSize() const
{
    const wxSize textSize = wxBackstageHelpers::MeasureText(this, GetLabel(), GetHeadingFont());
    return wxSize{ textSize.GetWidth() + FromDIP(2), textSize.GetHeight() + FromDIP(8) };
}

//-------------------------------------------
void wxBackstageHeading::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxBackstagePaintBuffer paint{ this };
    wxDC& dc = paint.m_buffer;
    dc.SetFont(GetHeadingFont());
    dc.SetTextForeground(paint.m_foreground);
    const wxSize textSize = dc.GetTextExtent(GetLabel());
    dc.DrawText(GetLabel(), 0, (GetClientSize().GetHeight() - textSize.GetHeight()) / 2);
}

//-------------------------------------------
void wxBackstageHeading::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    Refresh();
    event.Skip();
}

//-------------------------------------------
// wxBackstageCallout
//-------------------------------------------
wxBackstageCallout::wxBackstageCallout(wxWindow* parent, wxWindowID id, const wxString& title,
                                       const wxString& message /*= wxString{}*/)
    : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL | wxFULL_REPAINT_ON_RESIZE, "wxBackstageCallout"),
      m_title(title), m_message(message)
{
    wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindow::SetLabel(message.empty() ? title : title + ". " + message);
    Bind(wxEVT_PAINT, &wxBackstageCallout::OnPaint, this);
    Bind(wxEVT_SIZE, &wxBackstageCallout::OnSize, this);
}

//-------------------------------------------
void wxBackstageCallout::SetTile(wxBackstageButton* tile)
{
    m_tile = tile;
    ApplyButtonColours();
    InvalidateBestSize();
    PositionChildren();
    Refresh();
}

//-------------------------------------------
void wxBackstageCallout::SetAction(wxBackstageButton* action)
{
    m_action = action;
    ApplyButtonColours();
    InvalidateBestSize();
    PositionChildren();
    Refresh();
}

//-------------------------------------------
void wxBackstageCallout::ApplyButtonColours()
{
    wxColour bg, fg;
    wxBackstageHelpers::GetPageColours(this, bg, fg);
    const wxColour fill = wxBackstageHelpers::Blend(bg, fg, 0.02);
    if ( m_tile.get() != nullptr )
    {
        m_tile->SetCalloutColour(fill);
    }
    if ( m_action.get() != nullptr )
    {
        m_action->SetCalloutColour(fill);
    }
}

//-------------------------------------------
wxFont wxBackstageCallout::GetTitleFont() const
{
    return GetFont().Bold();
}

//-------------------------------------------
wxSize wxBackstageCallout::GetTextSize() const
{
    const wxSize titleSize = wxBackstageHelpers::MeasureText(this, m_title, GetTitleFont());
    const wxSize messageSize = wxBackstageHelpers::MeasureText(this, m_message, GetFont());
    return wxSize{ std::max(titleSize.GetWidth(), messageSize.GetWidth()),
                   titleSize.GetHeight() +
                       (messageSize.GetHeight() > 0 ? FromDIP(4) + messageSize.GetHeight() : 0) };
}

//-------------------------------------------
wxPoint wxBackstageCallout::GetTextOrigin() const
{
    const wxCoord pad = FromDIP(10);
    const wxCoord tileWidth = (m_tile.get() != nullptr) ?
        m_tile->GetBestSize().GetWidth() + FromDIP(12) : 0;
    return wxPoint{ pad + tileWidth, pad };
}

//-------------------------------------------
wxSize wxBackstageCallout::DoGetBestSize() const
{
    const wxCoord pad = FromDIP(10);
    const wxSize textSize = GetTextSize();
    const wxSize tileSize = (m_tile.get() != nullptr) ? m_tile->GetBestSize() : wxSize{ 0, 0 };
    const wxSize actionSize = (m_action.get() != nullptr) ? m_action->GetBestSize() : wxSize{ 0, 0 };
    const wxCoord textBlockHeight = textSize.GetHeight() +
        ((m_action.get() != nullptr) ? FromDIP(8) + actionSize.GetHeight() : 0);
    return wxSize{ GetTextOrigin().x + std::max(textSize.GetWidth(), actionSize.GetWidth()) + pad,
                   (2 * pad) + std::max(tileSize.GetHeight(), textBlockHeight) };
}

//-------------------------------------------
void wxBackstageCallout::PositionChildren()
{
    const wxCoord pad = FromDIP(10);
    if ( m_tile.get() != nullptr )
    {
        const wxSize tileSize = m_tile->GetBestSize();
        m_tile->SetSize(pad, pad, tileSize.GetWidth(), tileSize.GetHeight());
    }
    if ( m_action.get() != nullptr )
    {
        const wxPoint origin = GetTextOrigin();
        const wxSize actionSize = m_action->GetBestSize();
        m_action->SetSize(origin.x, origin.y + GetTextSize().GetHeight() + FromDIP(8),
                          actionSize.GetWidth(), actionSize.GetHeight());
    }
}

//-------------------------------------------
void wxBackstageCallout::OnSize(wxSizeEvent& event)
{
    PositionChildren();
    event.Skip();
}

//-------------------------------------------
void wxBackstageCallout::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxColour bg, fg;
    wxBackstageHelpers::GetPageColours(this, bg, fg);
    const bool isDark = wxBackstageHelpers::IsDark(bg);
    ApplyButtonColours();

    wxAutoBufferedPaintDC dc{ this };
    const wxRect rect = GetClientRect();
    dc.GradientFillLinear(rect,
        wxBackstageHelpers::Blend(bg, wxColour{ 255, 214, 102 }, isDark ? 0.32 : 0.6), bg, wxSOUTH);
    {
        const wxDCPenChanger pc{ dc,
            wxPen{ wxBackstageHelpers::Blend(bg, wxColour{ 224, 178, 48 }, isDark ? 0.5 : 0.55), 1 } };
        const wxDCBrushChanger bc{ dc, *wxTRANSPARENT_BRUSH };
        dc.DrawRectangle(rect);
    }

    const wxPoint origin = GetTextOrigin();
    const wxSize titleSize = wxBackstageHelpers::MeasureText(this, m_title, GetTitleFont());
    const wxSize messageSize = wxBackstageHelpers::MeasureText(this, m_message, GetFont());
    const wxCoord textWidth = std::max(0, rect.GetRight() - origin.x);
    dc.SetFont(GetTitleFont());
    dc.SetTextForeground(isDark ? wxColour{ 255, 176, 64 } : wxColour{ 204, 102, 0 });
    dc.DrawLabel(m_title, wxRect{ origin.x, origin.y, textWidth, titleSize.GetHeight() },
                 wxALIGN_LEFT | wxALIGN_TOP);
    dc.SetFont(GetFont());
    dc.SetTextForeground(fg);
    dc.DrawLabel(m_message,
                 wxRect{ origin.x, origin.y + titleSize.GetHeight() + FromDIP(4), textWidth,
                         messageSize.GetHeight() },
                 wxALIGN_LEFT | wxALIGN_TOP);
}

//-------------------------------------------
// wxBackstageItemList
//-------------------------------------------
#if wxUSE_ACCESSIBILITY
//-------------------------------------------
// Exposes the rows to assistive technology.
class wxBackstageItemList::Accessible final : public wxAccessible
{
public:
    explicit Accessible(wxBackstageItemList* list) : wxAccessible(list), m_list(list)
    {
    }

    wxAccStatus GetChildCount(int* childCount) override
    {
        *childCount = static_cast<int>(m_list->m_rows.size());
        return wxACC_OK;
    }

    wxAccStatus GetChild(const int childId, wxAccessible** child) override
    {
        *child = (childId == wxACC_SELF) ? this : nullptr;
        return wxACC_OK;
    }

    wxAccStatus HitTest(const wxPoint& pt, int* childId, wxAccessible** childObject) override
    {
        *childId = wxACC_SELF;
        *childObject = nullptr;
        const wxPoint clientPt = m_list->ScreenToClient(pt);
        if ( !m_list->GetClientRect().Contains(clientPt) )
        {
            return wxACC_FALSE;
        }
        const long row = m_list->RowAt(clientPt);
        if ( row != wxNOT_FOUND )
        {
            *childId = static_cast<int>(row) + 1;
        }
        return wxACC_OK;
    }

    wxAccStatus GetLocation(wxRect& rect, const int elementId) override
    {
        if ( elementId == wxACC_SELF )
        {
            rect = wxRect{ m_list->ClientToScreen(wxPoint{ 0, 0 }), m_list->GetClientSize() };
            return wxACC_OK;
        }
        const auto* row = RowOf(elementId);
        if ( row == nullptr )
        {
            return wxACC_INVALID_ARG;
        }
        const wxCoord top = m_list->CalcScrolledPosition(
            wxPoint{ 0, m_list->GetRowTop(static_cast<size_t>(elementId) - 1) }).y;
        rect = wxRect{ m_list->ClientToScreen(wxPoint{ 0, top }),
                       wxSize{ m_list->GetClientSize().GetWidth(), m_list->GetRowHeight(*row) } };
        return wxACC_OK;
    }

    wxAccStatus GetName(const int childId, wxString* name) override
    {
        if ( childId == wxACC_SELF )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        const auto* row = RowOf(childId);
        if ( row == nullptr )
        {
            return wxACC_INVALID_ARG;
        }
        *name = row->m_title;
        if ( !row->m_subtitle.empty() )
        {
            *name += ", " + row->m_subtitle;
        }
        if ( !row->m_rightText.empty() )
        {
            *name += ", " + row->m_rightText;
        }
        return wxACC_OK;
    }

    wxAccStatus GetValue(const int childId, wxString* value) override
    {
        if ( childId != wxACC_SELF || !m_list->m_rows.empty() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        *value = m_list->m_emptyText;
        return wxACC_OK;
    }

    wxAccStatus GetRole(const int childId, wxAccRole* role) override
    {
        if ( childId == wxACC_SELF )
        {
            *role = wxROLE_SYSTEM_LIST;
            return wxACC_OK;
        }
        const auto* row = RowOf(childId);
        if ( row == nullptr )
        {
            return wxACC_INVALID_ARG;
        }
        *role = row->m_isHeader ? wxROLE_SYSTEM_STATICTEXT : wxROLE_SYSTEM_LISTITEM;
        return wxACC_OK;
    }

    wxAccStatus GetState(const int childId, long* state) override
    {
        if ( childId == wxACC_SELF )
        {
            *state = wxACC_STATE_SYSTEM_FOCUSABLE |
                (m_list->HasFocus() ? wxACC_STATE_SYSTEM_FOCUSED : 0);
            return wxACC_OK;
        }
        const auto* row = RowOf(childId);
        if ( row == nullptr )
        {
            return wxACC_INVALID_ARG;
        }
        if ( row->m_isHeader )
        {
            *state = wxACC_STATE_SYSTEM_READONLY;
            return wxACC_OK;
        }
        *state = wxACC_STATE_SYSTEM_SELECTABLE | wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( (childId - 1) == m_list->m_selectedRow )
        {
            *state |= wxACC_STATE_SYSTEM_SELECTED;
            if ( m_list->HasFocus() )
            {
                *state |= wxACC_STATE_SYSTEM_FOCUSED;
            }
        }
        if ( (childId - 1) == m_list->m_hoverRow )
        {
            *state |= wxACC_STATE_SYSTEM_HOTTRACKED;
        }
        return wxACC_OK;
    }

    wxAccStatus GetDefaultAction(const int childId, wxString* actionName) override
    {
        const auto* row = RowOf(childId);
        *actionName = (row != nullptr && !row->m_isHeader) ? _("Open") : wxString{};
        return wxACC_OK;
    }

    wxAccStatus DoDefaultAction(const int childId) override
    {
        const auto* row = RowOf(childId);
        if ( row == nullptr || row->m_isHeader )
        {
            return wxACC_NOT_SUPPORTED;
        }
        m_list->Activate(static_cast<size_t>(childId) - 1);
        return wxACC_OK;
    }

    wxAccStatus GetFocus(int* childId, wxAccessible** child) override
    {
        *child = nullptr;
        if ( !m_list->HasFocus() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        *childId = (m_list->m_selectedRow == wxNOT_FOUND) ?
            wxACC_SELF : static_cast<int>(m_list->m_selectedRow) + 1;
        return wxACC_OK;
    }

    wxAccStatus Select(const int childId, const wxAccSelectionFlags selectFlags) override
    {
        const auto* row = RowOf(childId);
        if ( row == nullptr || row->m_isHeader )
        {
            return wxACC_NOT_SUPPORTED;
        }
        if ( (selectFlags & (wxACC_SEL_TAKEFOCUS | wxACC_SEL_TAKESELECTION)) != 0 )
        {
            m_list->m_selectedRow = childId - 1;
            m_list->EnsureRowVisible(static_cast<size_t>(childId) - 1);
            m_list->SetFocus();
            m_list->NotifySelectionChanged(m_list->m_selectedRow);
            m_list->Refresh();
        }
        return wxACC_OK;
    }

private:
    wxNODISCARD
    const Row* RowOf(const int childId) const noexcept
    {
        return (childId >= 1 && static_cast<size_t>(childId) <= m_list->m_rows.size()) ?
            &m_list->m_rows[static_cast<size_t>(childId) - 1] : nullptr;
    }

    wxBackstageItemList* m_list{ nullptr };
};
#endif

wxBackstageItemList::wxBackstageItemList(wxWindow* parent, wxWindowID id /*= wxID_ANY*/)
    : wxScrolledCanvas(parent, id, wxDefaultPosition, wxDefaultSize,
        wxHSCROLL | wxVSCROLL | wxWANTS_CHARS | wxBORDER_NONE, "wxBackstageItemList")
{
    wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetCanFocus(true);
    SetScrollRate(0, FromDIP(12));
    ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

    Bind(wxEVT_PAINT, &wxBackstageItemList::OnPaint, this);
    Bind(wxEVT_SIZE, &wxBackstageItemList::OnSize, this);
    Bind(wxEVT_MOTION, &wxBackstageItemList::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &wxBackstageItemList::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &wxBackstageItemList::OnMouseDown, this);
    Bind(wxEVT_LEFT_DCLICK, &wxBackstageItemList::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &wxBackstageItemList::OnMouseUp, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxBackstageItemList::OnMouseCaptureLost, this);
    Bind(wxEVT_KEY_DOWN, &wxBackstageItemList::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &wxBackstageItemList::OnFocusChange, this);
    Bind(wxEVT_KILL_FOCUS, &wxBackstageItemList::OnFocusChange, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED, &wxBackstageItemList::OnSysColourChanged, this);
    Bind(wxEVT_DPI_CHANGED,
         [this](wxDPIChangedEvent& event)
         {
             RebuildRowLayout();
             event.Skip();
         });

    RebuildRowLayout();
#if wxUSE_ACCESSIBILITY
    SetAccessible(new Accessible{ this });
#endif
}

//-------------------------------------------
void wxBackstageItemList::NotifySelectionChanged(const long row)
{
#if wxUSE_ACCESSIBILITY
    if ( row >= 0 && static_cast<size_t>(row) < m_rows.size() )
    {
        const int childId = static_cast<int>(row) + 1;
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_FOCUS, this, wxOBJID_CLIENT, childId);
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_SELECTION, this, wxOBJID_CLIENT, childId);
    }
#else
    wxUnusedVar(row);
#endif
}

//-------------------------------------------
wxSize wxBackstageItemList::DoGetBestSize() const
{
    return FromDIP(wxSize{ 420, 300 });
}

//-------------------------------------------
void wxBackstageItemList::AddHeader(const wxString& text)
{
    Row row;
    row.m_isHeader = true;
    row.m_title = text;
    m_rowTops.push_back(m_rowTops.back() + GetRowHeight(row));
    m_rows.push_back(std::move(row));
    UpdateVirtualSize();
    Refresh();
}

//-------------------------------------------
size_t wxBackstageItemList::AddItem(const wxString& title, const wxString& subtitle /*= wxString{}*/,
    const wxString& rightText /*= wxString{}*/, const wxBitmapBundle& icon /*= wxBitmapBundle{}*/,
    const wxString& userString /*= wxString{}*/)
{
    Row row;
    row.m_title = title;
    row.m_subtitle = subtitle;
    row.m_rightText = rightText;
    row.m_icon = icon;
    row.m_userString = userString;
    row.m_itemIndex = m_itemCount++;
    m_anyIcons = m_anyIcons || row.m_icon.IsOk();
    m_rowTops.push_back(m_rowTops.back() + GetRowHeight(row));
    m_rows.push_back(std::move(row));
    UpdateVirtualSize();
    Refresh();
    return m_itemCount - 1;
}

//-------------------------------------------
void wxBackstageItemList::Clear()
{
    m_rows.clear();
    m_rowTops.assign(1, 0);
    m_anyIcons = false;
    m_itemCount = 0;
    m_hoverRow = m_selectedRow = m_pressedRow = wxNOT_FOUND;
    UpdateVirtualSize();
    Scroll(0, 0);
    Refresh();
}

//-------------------------------------------
wxString wxBackstageItemList::GetItemString(const size_t index) const
{
    const auto rowIt = std::find_if(m_rows.cbegin(), m_rows.cend(),
        [index](const Row& row) noexcept { return !row.m_isHeader && row.m_itemIndex == index; });
    return rowIt == m_rows.cend() ? wxString{} : rowIt->m_userString;
}

//-------------------------------------------
void wxBackstageItemList::SetEmptyText(const wxString& text)
{
    m_emptyText = text;
    Refresh();
}

//-------------------------------------------
wxCoord wxBackstageItemList::GetRowHeight(const Row& row) const
{
    if ( row.m_isHeader )
    {
        return wxRound(m_charHeight * 1.3) + (2 * GetPadding());
    }
    const wxCoord textHeight = m_charHeight * (row.m_subtitle.empty() ? 1 : 2);
    return std::max(GetIconSize(), textHeight) + (2 * GetPadding());
}

//-------------------------------------------
void wxBackstageItemList::RebuildRowLayout()
{
    m_charHeight = GetCharHeight();
    m_rowTops.assign(1, 0);
    m_rowTops.reserve(m_rows.size() + 1);
    for ( const auto& row : m_rows )
    {
        m_rowTops.push_back(m_rowTops.back() + GetRowHeight(row));
    }
    UpdateVirtualSize();
    Refresh();
}

//-------------------------------------------
bool wxBackstageItemList::SetFont(const wxFont& font)
{
    const bool changed = wxScrolledCanvas::SetFont(font);
    RebuildRowLayout();
    return changed;
}

//-------------------------------------------
wxCoord wxBackstageItemList::GetRowTop(const size_t row) const
{
    return m_rowTops[std::min(row, m_rows.size())];
}

//-------------------------------------------
size_t wxBackstageItemList::FirstRowAtOrBelow(const wxCoord y) const
{
    const auto it = std::upper_bound(m_rowTops.cbegin(), m_rowTops.cend() - 1, y);
    const auto count = static_cast<size_t>(std::distance(m_rowTops.cbegin(), it));
    return count == 0 ? 0 : count - 1;
}

//-------------------------------------------
long wxBackstageItemList::RowAt(const wxPoint& clientPt) const
{
    const wxCoord y = CalcUnscrolledPosition(clientPt).y;
    if ( y < 0 || y >= m_rowTops.back() )
    {
        return wxNOT_FOUND;
    }
    return static_cast<long>(FirstRowAtOrBelow(y));
}

//-------------------------------------------
void wxBackstageItemList::UpdateVirtualSize()
{
    SetVirtualSize(GetClientSize().GetWidth(), m_rowTops.back());
}

//-------------------------------------------
void wxBackstageItemList::OnSize(wxSizeEvent& event)
{
    UpdateVirtualSize();
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstageItemList::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxBackstagePaintGraphics paint{ this, GetFont() };
    wxDC& dc = paint.m_graphics;
    const wxColour bg = paint.m_background;
    const wxColour fg = paint.m_foreground;
    // scroll offset needs to go on the DC that we are drawing with
    DoPrepareDC(dc);

    const wxColour dimColour = wxBackstageHelpers::Blend(fg, bg, 0.4);
    const wxCoord width = GetClientSize().GetWidth();
    const wxCoord pad = GetPadding();
    const wxCoord iconSize = GetIconSize();

    if ( m_rows.empty() )
    {
        if ( !m_emptyText.empty() )
        {
            dc.SetTextForeground(dimColour);
            dc.DrawLabel(m_emptyText, wxRect{ pad, pad, width - (2 * pad), GetClientSize().GetHeight() },
                wxALIGN_LEFT | wxALIGN_TOP);
        }
        return;
    }

    const bool anyIcons = m_anyIcons;

    const wxCoord viewTop = CalcUnscrolledPosition(wxPoint{ 0, 0 }).y;
    const wxCoord viewBottom = viewTop + GetClientSize().GetHeight();

    for ( size_t i = FirstRowAtOrBelow(viewTop); i < m_rows.size(); ++i )
    {
        const auto& row = m_rows[i];
        const wxCoord top = m_rowTops[i];
        const wxCoord height = m_rowTops[i + 1] - top;
        if ( top > viewBottom )
        {
            break;
        }
        const wxRect rowRect{ 0, top, width, height };

        if ( row.m_isHeader )
        {
            const wxDCFontChanger fc{ dc, GetFont().Scaled(1.15F).Bold() };
            dc.SetTextForeground(fg);
            const wxSize textSize = dc.GetTextExtent(row.m_title);
            dc.DrawText(row.m_title, pad, top + ((height - textSize.GetHeight()) / 2));
            continue;
        }

        const bool isSelected = (static_cast<long>(i) == m_selectedRow);
        const bool isHover = (static_cast<long>(i) == m_hoverRow);
        wxColour fill = bg;
        if ( static_cast<long>(i) == m_pressedRow )
        {
            fill = wxBackstageHelpers::ShadeOrTint(bg, 0.14);
        }
        else if ( isSelected && HasFocus() )
        {
            fill = wxBackstageHelpers::ShadeOrTint(bg, 0.08);
        }
        else if ( isHover )
        {
            fill = wxBackstageHelpers::ShadeOrTint(bg, 0.06);
        }
        if ( fill != bg )
        {
            const wxDCPenChanger pc{ dc, *wxTRANSPARENT_PEN };
            const wxDCBrushChanger bc{ dc, wxBrush{ fill } };
            dc.DrawRectangle(rowRect);
        }
        const wxColour textColour = wxBackstageHelpers::BlackOrWhiteContrast(fill);
        const wxColour rowDimColour = wxBackstageHelpers::Blend(textColour, fill, 0.4);

        wxCoord textX = pad;
        if ( anyIcons )
        {
            wxBackstageHelpers::DrawBitmapFit(dc, this, row.m_icon,
                wxRect{ pad, top + ((height - iconSize) / 2), iconSize, iconSize });
            textX += iconSize + pad;
        }

        wxCoord textRight = width - pad;
        if ( !row.m_rightText.empty() )
        {
            const wxSize rightSize = dc.GetTextExtent(row.m_rightText);
            textRight -= rightSize.GetWidth();
            dc.SetTextForeground(rowDimColour);
            dc.DrawText(row.m_rightText, textRight,
                top + ((height - rightSize.GetHeight()) / 2));
            textRight -= pad;
        }

        {
            const wxDCClipper clipper{ dc,
                wxRect{ textX, top, std::max(0, textRight - textX), height } };
            const wxCoord lineHeight = m_charHeight;
            const wxCoord blockHeight = lineHeight * (row.m_subtitle.empty() ? 1 : 2);
            const wxCoord y = top + ((height - blockHeight) / 2);
            dc.SetTextForeground(textColour);
            dc.DrawText(row.m_title, textX, y);
            if ( !row.m_subtitle.empty() )
            {
                dc.SetTextForeground(rowDimColour);
                dc.DrawText(row.m_subtitle, textX, y + lineHeight);
            }
        }

        if ( isSelected && HasFocus() )
        {
            wxBackstageHelpers::DrawFocusRect(dc, rowRect.Deflate(FromDIP(2), FromDIP(1)), textColour);
        }
    }
}

//-------------------------------------------
void wxBackstageItemList::OnMouseMove(wxMouseEvent& event)
{
    long row = RowAt(event.GetPosition());
    if ( row != wxNOT_FOUND && m_rows[static_cast<size_t>(row)].m_isHeader )
    {
        row = wxNOT_FOUND;
    }
    if ( row != m_hoverRow )
    {
        m_hoverRow = row;
        Refresh();
    }
    event.Skip();
}

//-------------------------------------------
void wxBackstageItemList::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
    if ( m_hoverRow != wxNOT_FOUND )
    {
        m_hoverRow = wxNOT_FOUND;
        Refresh();
    }
}

//-------------------------------------------
void wxBackstageItemList::OnMouseDown(wxMouseEvent& event)
{
    // hit-test first because getting focus can scroll the list
    const long row = RowAt(event.GetPosition());
    m_focusFromMouse = true;
    SetFocus();
    m_focusFromMouse = false;
    if ( row != wxNOT_FOUND && !m_rows[static_cast<size_t>(row)].m_isHeader )
    {
        m_pressedRow = row;
        m_selectedRow = row;
        NotifySelectionChanged(row);
        if ( !HasCapture() )
        {
            CaptureMouse();
        }
        Refresh();
    }
}

//-------------------------------------------
void wxBackstageItemList::OnMouseUp(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        ReleaseMouse();
    }
    const long pressed = m_pressedRow;
    m_pressedRow = wxNOT_FOUND;
    Refresh();
    if ( pressed != wxNOT_FOUND && RowAt(event.GetPosition()) == pressed )
    {
        Activate(static_cast<size_t>(pressed));
    }
}

//-------------------------------------------
void wxBackstageItemList::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    m_pressedRow = wxNOT_FOUND;
    Refresh();
}

//-------------------------------------------
void wxBackstageItemList::EnsureRowVisible(const size_t row)
{
    if ( row >= m_rows.size() )
    {
        return;
    }
    int unitX{ 0 }, unitY{ 0 };
    GetScrollPixelsPerUnit(&unitX, &unitY);
    if ( unitY <= 0 )
    {
        return;
    }
    const wxCoord top = GetRowTop(row);
    const wxCoord bottom = top + GetRowHeight(m_rows[row]);
    const wxCoord viewTop = CalcUnscrolledPosition(wxPoint{ 0, 0 }).y;
    const wxCoord viewHeight = GetClientSize().GetHeight();
    if ( top < viewTop )
    {
        Scroll(0, top / unitY);
    }
    else if ( bottom > viewTop + viewHeight )
    {
        Scroll(0, (bottom - viewHeight + unitY - 1) / unitY);
    }
}

//-------------------------------------------
void wxBackstageItemList::MoveSelection(const int direction)
{
    if ( m_rows.empty() )
    {
        return;
    }
    const long count = static_cast<long>(m_rows.size());
    long row = m_selectedRow;
    if ( row == wxNOT_FOUND )
    {
        row = direction > 0 ? -1 : count;
    }
    for ( long attempts = 0; attempts < count; ++attempts )
    {
        row += direction;
        if ( row < 0 || row >= count )
        {
            return;
        }
        if ( !m_rows[static_cast<size_t>(row)].m_isHeader )
        {
            m_selectedRow = row;
            EnsureRowVisible(static_cast<size_t>(row));
            NotifySelectionChanged(row);
            Refresh();
            return;
        }
    }
}

//-------------------------------------------
void wxBackstageItemList::OnKeyDown(wxKeyEvent& event)
{
    if ( wxBackstageHelpers::SkipIfShortcutKey(event) )
    {
        return;
    }
    switch ( event.GetKeyCode() )
    {
    case WXK_UP:
        MoveSelection(-1);
        break;
    case WXK_DOWN:
        MoveSelection(1);
        break;
    case WXK_HOME:
        m_selectedRow = wxNOT_FOUND;
        MoveSelection(1);
        break;
    case WXK_END:
        m_selectedRow = wxNOT_FOUND;
        MoveSelection(-1);
        break;
    case WXK_TAB:
        Navigate(event.ShiftDown() ?
            wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
        break;
    default:
        if ( wxBackstageHelpers::IsActivateKey(event.GetKeyCode()) && !event.IsAutoRepeat() &&
            m_selectedRow != wxNOT_FOUND )
        {
            Activate(static_cast<size_t>(m_selectedRow));
        }
        else
        {
            event.Skip();
        }
        break;
    }
}

//-------------------------------------------
void wxBackstageItemList::OnFocusChange(wxFocusEvent& event)
{
    // start on the first item when tabbed into, but not when clicked (that would scroll the list)
    if ( event.GetEventType() == wxEVT_SET_FOCUS && m_selectedRow == wxNOT_FOUND &&
        !m_focusFromMouse )
    {
        MoveSelection(1);
    }
    else if ( event.GetEventType() == wxEVT_SET_FOCUS && m_selectedRow != wxNOT_FOUND )
    {
        NotifySelectionChanged(m_selectedRow);
    }
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstageItemList::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstageItemList::Activate(const size_t row)
{
    if ( row >= m_rows.size() || m_rows[row].m_isHeader )
    {
        return;
    }
    wxCommandEvent event{ wxEVT_BACKSTAGE_ITEM_CLICKED, GetId() };
    event.SetEventObject(this);
    event.SetInt(static_cast<int>(m_rows[row].m_itemIndex));
    event.SetString(m_rows[row].m_userString);
    ProcessWindowEvent(event);
}

//-------------------------------------------
// wxBackstageMRUList
//-------------------------------------------

constexpr size_t wxBackstageMRUList::UNLIMITED_MRU_LIST;

//-------------------------------------------
wxBackstageMRUList::Section wxBackstageMRUList::GetSection(const wxDateTime& modified,
                                                           const wxDateTime& now)
{
    if ( !modified.IsValid() )
    {
        return Section::Older;
    }
    // Future date? Call it 'today.'
    if ( modified.IsLaterThan(now) || modified.IsSameDate(now) )
    {
        return Section::Today;
    }
    if ( modified.IsSameDate(now - wxDateSpan::Day()) )
    {
        return Section::Yesterday;
    }
    wxDateTime weekStart = now.GetDateOnly();
    weekStart.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
    if ( modified >= weekStart )
    {
        return Section::ThisWeek;
    }
    if ( modified >= (weekStart - wxDateSpan::Week()) )
    {
        return Section::LastWeek;
    }
    if ( modified.GetYear() == now.GetYear() && modified.GetMonth() == now.GetMonth() )
    {
        return Section::ThisMonth;
    }
    return Section::Older;
}

//-------------------------------------------
wxString wxBackstageMRUList::GetSectionLabel(const Section section)
{
    switch ( section )
    {
    case Section::Today:
        return _("Today");
    case Section::Yesterday:
        return _("Yesterday");
    case Section::ThisWeek:
        return _("This Week");
    case Section::LastWeek:
        return _("Last Week");
    case Section::ThisMonth:
        return _("Earlier This Month");
    case Section::Older:
    default:
        return _("Older");
    }
}

//-------------------------------------------
wxString wxBackstageMRUList::FormatTimeOfDay(const wxDateTime& time)
{
    wxString am, pm;
    wxDateTime::GetAmPmStrings(&am, &pm);
    // locales that use 24-hour time have no AM/PM strings
    if ( am.empty() || pm.empty() )
    {
        return wxString::Format("%02d:%02d", time.GetHour(), time.GetMinute());
    }
    const int hour12 = (time.GetHour() % 12 == 0) ? 12 : time.GetHour() % 12;
    return wxString::Format("%d:%02d %s", hour12, time.GetMinute(),
                            time.GetHour() < 12 ? am : pm);
}

//-------------------------------------------
wxString wxBackstageMRUList::SimplifyFolderPath(wxString path)
{
    const bool caseSensitive = wxFileName::IsCaseSensitive();
    const wxString separator{ wxFileName::GetPathSeparator() };
    const auto trimSeparators = [&separator](wxString text) -> wxString
        {
            while ( text.length() > 1 && text.EndsWith(separator) )
            {
                text.RemoveLast();
            }
            return text;
        };
    const wxString homeDir = trimSeparators(wxFileName::GetHomeDir());

    // shorten the standard user folder that the path is in (if any)
    const struct
    {
        wxStandardPathsBase::Dir m_dir;
        wxString m_name;
    } folders[] = {
        { wxStandardPathsBase::Dir::Dir_Documents, _("Documents") },
        { wxStandardPathsBase::Dir::Dir_Desktop, _("Desktop") },
        { wxStandardPathsBase::Dir::Dir_Pictures, _("Pictures") },
        { wxStandardPathsBase::Dir::Dir_Videos, _("Videos") },
        { wxStandardPathsBase::Dir::Dir_Music, _("Music") },
        { wxStandardPathsBase::Dir::Dir_Downloads, _("Downloads") }
    };
    size_t bestLength{ 0 };
    wxString bestName;
    for ( const auto& folder : folders )
    {
        const wxString dirPath = trimSeparators(wxStandardPaths::Get().GetUserDir(folder.m_dir));
        if ( dirPath.length() <= 1 || dirPath.IsSameAs(homeDir, caseSensitive) )
        {
            continue;
        }
        if ( dirPath.length() > bestLength && path.length() >= dirPath.length() &&
            path.Left(dirPath.length()).IsSameAs(dirPath, caseSensitive) &&
            (path.length() == dirPath.length() ||
             path.Mid(dirPath.length()).StartsWith(separator)) )
        {
            bestLength = dirPath.length();
            bestName = folder.m_name;
        }
    }
    if ( bestLength > 0 )
    {
        path = bestName + path.Mid(bestLength);
    }
    while ( path.StartsWith(separator) )
    {
        path.Remove(0, separator.length());
    }
    path.Replace(separator, wxString::FromUTF8(" » "), true);
    return path;
}

//-------------------------------------------
wxBackstageMRUList::wxBackstageMRUList(wxWindow* parent, wxWindowID id /*= wxID_ANY*/)
    : wxBackstageItemList(parent, id)
{
    Bind(wxEVT_SHOW,
        [this](wxShowEvent& event)
        {
            if ( event.IsShown() && !m_entries.empty() )
            {
                Rebuild();
            }
            event.Skip();
        });
}

//-------------------------------------------
wxString wxBackstageMRUList::FormatModifiedDate(const wxDateTime& modified, const wxDateTime& now)
{
    if ( !modified.IsValid() )
    {
        return {};
    }

    const wxTimeSpan elapsed = now - modified;
    if ( modified.IsLaterThan(now) || elapsed.GetMinutes() < 1 )
    {
        return _("Just now");
    }
    if ( modified.IsSameDate(now) )
    {
        const int minutes = elapsed.GetMinutes();
        if ( minutes < 60 )
        {
            return wxString::Format(
                wxPLURAL("%d minute ago", "%d minutes ago", static_cast<unsigned>(minutes)),
                minutes);
        }
        return FormatTimeOfDay(modified);
    }
    if ( modified.IsSameDate(now - wxDateSpan::Day()) )
    {
        return wxString::Format(_("Yesterday at %s"), FormatTimeOfDay(modified));
    }
    // compare calendar days, as a time span across a DST change isn't a multiple of 24 hours
    if ( modified.GetDateOnly() > (now.GetDateOnly() - wxDateSpan::Week()) )
    {
        return wxString::Format(_("%s at %s"),
            wxDateTime::GetWeekDayName(modified.GetWeekDay(), wxDateTime::Name_Abbr),
            FormatTimeOfDay(modified));
    }
    if ( modified.GetYear() == now.GetYear() )
    {
        // TRANSLATORS: Month (abbreviated) and day of the month (e.g., "Mar 03").
        return modified.Format(_("%b %d"));
    }
    return modified.FormatDate();
}

//-------------------------------------------
void wxBackstageMRUList::SetFiles(const wxArrayString& paths)
{
    m_entries.clear();
    for ( const auto& path : paths )
    {
        const wxFileName fileName{ path };
        if ( fileName.FileExists() )
        {
            m_entries.push_back(Entry{ path, fileName.GetModificationTime() });
        }
    }
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::AddFile(const wxString& path, const wxDateTime& modified)
{
    m_entries.push_back(Entry{ path, modified });
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::ClearFiles()
{
    m_entries.clear();
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::SetIconProvider(std::function<wxBitmapBundle(const wxString&)> provider)
{
    m_iconProvider = std::move(provider);
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::ShowSectionHeaders(const bool show /*= true*/)
{
    m_showSectionHeaders = show;
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::SetMaxFiles(const size_t maxFiles)
{
    m_maxFiles = maxFiles;
    Rebuild();
}

//-------------------------------------------
void wxBackstageMRUList::Rebuild()
{
    Clear();

    auto sorted = m_entries;
    std::stable_sort(sorted.begin(), sorted.end(),
        [](const Entry& lhv, const Entry& rhv) -> bool
        {
            if ( !lhv.m_modified.IsValid() )
            {
                return false;
            }
            if ( !rhv.m_modified.IsValid() )
            {
                return true;
            }
            return lhv.m_modified.IsLaterThan(rhv.m_modified);
        });
    if ( sorted.size() > m_maxFiles )
    {
        sorted.resize(m_maxFiles);
    }

    const wxDateTime now = wxDateTime::Now();
    const wxBitmapBundle defaultIcon =
        wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_LIST);
    bool isFirst{ true };
    auto currentSection{ Section::Today };
    for ( const auto& entry : sorted )
    {
        const Section section = GetSection(entry.m_modified, now);
        if ( m_showSectionHeaders && (isFirst || section != currentSection) )
        {
            AddHeader(GetSectionLabel(section));
        }
        isFirst = false;
        currentSection = section;

        const wxFileName fileName{ entry.m_path };
        AddItem(fileName.GetFullName(), SimplifyFolderPath(fileName.GetPath()),
                FormatModifiedDate(entry.m_modified, now),
                m_iconProvider ? m_iconProvider(entry.m_path) : defaultIcon, entry.m_path);
    }
}

#endif // wxUSE_RIBBON
