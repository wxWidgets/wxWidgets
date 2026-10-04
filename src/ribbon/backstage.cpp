///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/backstage.cpp
// Purpose:     Backstage view (as used by a ribbon's "File" tab)
// Author:      Blake Madden
// Created:     2026-09-20
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RIBBON

#include "wx/ribbon/backstage.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/dcclient.h"
    #include "wx/log.h"
#endif

#include "wx/access.h"
#include "wx/dcbuffer.h"

#include <algorithm>
#include <array>
#include <functional>
#include <utility>

wxDEFINE_EVENT(wxEVT_BACKSTAGE_CLICKED, wxNotifyEvent);

//-------------------------------------------
wxBackstagePage::wxBackstagePage(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxHSCROLL | wxVSCROLL | wxTAB_TRAVERSAL, "wxBackstagePage")
{
    SetScrollRate(0, FromDIP(12));
    // Only scroll vertically. The content should wrap or shrink to fit horizontally
    ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
}

#if wxUSE_ACCESSIBILITY
//-------------------------------------------
// Exposes the nav buttons to assistive technology.
class wxBackstage::Accessible final : public wxAccessible
{
public:
    explicit Accessible(wxBackstage* backstage) : wxAccessible(backstage), m_backstage(backstage)
    {
    }

    wxAccStatus GetChildCount(int* childCount) override
    {
        *childCount = PageAreaId();
        return wxACC_OK;
    }

    wxAccStatus GetChild(const int childId, wxAccessible** child) override
    {
        *child = (childId == PageAreaId()) ? m_backstage->m_book->GetOrCreateAccessible() :
                 (childId == wxACC_SELF)   ? this :
                                             nullptr;
        return wxACC_OK;
    }

    wxAccStatus HitTest(const wxPoint& pt, int* childId, wxAccessible** childObject) override
    {
        *childId = wxACC_SELF;
        *childObject = nullptr;
        const wxPoint clientPt = m_backstage->ScreenToClient(pt);
        if ( !m_backstage->GetClientRect().Contains(clientPt) )
        {
            return wxACC_FALSE;
        }
        const long hit = m_backstage->HitTest(clientPt);
        if ( hit != wxNOT_FOUND )
        {
            *childId = m_backstage->GetChildId(static_cast<size_t>(hit));
        }
        else if ( m_backstage->m_book->GetRect().Contains(clientPt) )
        {
            *childObject = m_backstage->m_book->GetOrCreateAccessible();
        }
        return wxACC_OK;
    }

    wxAccStatus GetLocation(wxRect& rect, const int elementId) override
    {
        if ( elementId == wxACC_SELF )
        {
            rect = wxRect{ m_backstage->ClientToScreen(wxPoint{ 0, 0 }),
                           m_backstage->GetClientSize() };
            return wxACC_OK;
        }
        const long idx = m_backstage->GetItemFromChildId(elementId);
        if ( idx == wxNOT_FOUND )
        {
            return wxACC_INVALID_ARG;
        }
        rect = m_backstage->m_items[idx].m_rect;
        rect.SetPosition(m_backstage->ClientToScreen(rect.GetPosition()));
        return wxACC_OK;
    }

    wxAccStatus GetName(const int childId, wxString* name) override
    {
        if ( childId == wxACC_SELF )
        {
            *name = _("Backstage");
            return wxACC_OK;
        }
        const long idx = m_backstage->GetItemFromChildId(childId);
        if ( idx == wxNOT_FOUND )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        *name = m_backstage->m_items[idx].m_label;
        return wxACC_OK;
    }

    wxAccStatus GetRole(const int childId, wxAccRole* role) override
    {
        if ( childId == wxACC_SELF )
        {
            *role = wxROLE_SYSTEM_PAGETABLIST;
            return wxACC_OK;
        }
        const long idx = m_backstage->GetItemFromChildId(childId);
        if ( idx == wxNOT_FOUND )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        *role = (m_backstage->m_items[idx].m_page.get() != nullptr) ? wxROLE_SYSTEM_PAGETAB :
                                                                wxROLE_SYSTEM_PUSHBUTTON;
        return wxACC_OK;
    }

    wxAccStatus GetState(const int childId, long* state) override
    {
        if ( childId == wxACC_SELF )
        {
            *state = wxACC_STATE_SYSTEM_FOCUSABLE |
                (m_backstage->HasFocus() ? wxACC_STATE_SYSTEM_FOCUSED : 0);
            return wxACC_OK;
        }
        const long idx = m_backstage->GetItemFromChildId(childId);
        if ( idx == wxNOT_FOUND )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        const auto& item = m_backstage->m_items[idx];
        if ( !item.m_enabled )
        {
            *state = wxACC_STATE_SYSTEM_UNAVAILABLE;
            return wxACC_OK;
        }
        *state = wxACC_STATE_SYSTEM_SELECTABLE | wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( item.m_page.get() != nullptr && item.m_id == m_backstage->m_selectedId )
        {
            *state |= wxACC_STATE_SYSTEM_SELECTED;
        }
        if ( m_backstage->HasFocus() && m_backstage->m_focusIndex == idx )
        {
            *state |= wxACC_STATE_SYSTEM_FOCUSED;
        }
        if ( m_backstage->m_hoverIndex == idx )
        {
            *state |= wxACC_STATE_SYSTEM_HOTTRACKED;
        }
        return wxACC_OK;
    }

    wxAccStatus GetDefaultAction(const int childId, wxString* actionName) override
    {
        *actionName = (childId == wxACC_SELF) ? wxString{} : _("Press");
        return wxACC_OK;
    }

    wxAccStatus DoDefaultAction(const int childId) override
    {
        const long idx = m_backstage->GetItemFromChildId(childId);
        if ( idx == wxNOT_FOUND || !m_backstage->IsSelectable(static_cast<size_t>(idx)) )
        {
            return wxACC_NOT_SUPPORTED;
        }
        m_backstage->ActivateItem(static_cast<size_t>(idx));
        return wxACC_OK;
    }

    wxAccStatus GetFocus(int* childId, wxAccessible** child) override
    {
        *child = nullptr;
        if ( !m_backstage->HasFocus() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }
        *childId = (m_backstage->m_focusIndex == wxNOT_FOUND) ?
            wxACC_SELF :
            m_backstage->GetChildId(static_cast<size_t>(m_backstage->m_focusIndex));
        return wxACC_OK;
    }

    wxAccStatus Select(const int childId, const wxAccSelectionFlags selectFlags) override
    {
        const long idx = m_backstage->GetItemFromChildId(childId);
        if ( idx == wxNOT_FOUND || !m_backstage->IsSelectable(static_cast<size_t>(idx)) )
        {
            return wxACC_NOT_SUPPORTED;
        }
        if ( (selectFlags & (wxACC_SEL_TAKEFOCUS | wxACC_SEL_TAKESELECTION)) != 0 )
        {
            m_backstage->m_focusIndex = idx;
            m_backstage->SetFocus();
            m_backstage->NotifyAccessibility(AccessibleEvent::Focus, idx);
            m_backstage->Refresh();
        }
        return wxACC_OK;
    }

private:
    wxNODISCARD
    int PageAreaId() const noexcept
    {
        return m_backstage->GetChildId(m_backstage->m_items.size()) + 1;
    }

    wxBackstage* m_backstage{ nullptr };
};
#endif

//-------------------------------------------
int wxBackstage::GetChildId(const size_t index) const noexcept
{
    int id{ 0 };
    for ( size_t i = 0; i < m_items.size() && i <= index; ++i )
    {
        if ( m_items[i].m_kind == ItemKind::Button )
        {
            ++id;
        }
    }
    return id;
}

//-------------------------------------------
long wxBackstage::GetItemFromChildId(const int childId) const noexcept
{
    int id{ 0 };
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        if ( m_items[i].m_kind == ItemKind::Button && ++id == childId )
        {
            return static_cast<long>(i);
        }
    }
    return wxNOT_FOUND;
}

//-------------------------------------------
void wxBackstage::NotifyAccessibility(const AccessibleEvent event, const long index)
{
#if wxUSE_ACCESSIBILITY
    if ( index < 0 || static_cast<size_t>(index) >= m_items.size() )
    {
        return;
    }
    const int eventType = (event == AccessibleEvent::Focus)      ? wxACC_EVENT_OBJECT_FOCUS :
                          (event == AccessibleEvent::Selection)  ? wxACC_EVENT_OBJECT_SELECTION :
                          (event == AccessibleEvent::NameChange) ? wxACC_EVENT_OBJECT_NAMECHANGE :
                                                                   wxACC_EVENT_OBJECT_STATECHANGE;
    wxAccessible::NotifyEvent(eventType, this, wxOBJID_CLIENT,
                              GetChildId(static_cast<size_t>(index)));
#else
    wxUnusedVar(event);
    wxUnusedVar(index);
#endif
}

//-------------------------------------------
wxBackstage::wxBackstage(wxWindow* parent, wxWindowID id /*= wxID_ANY*/)
    : wxWindow(parent, id, wxDefaultPosition, wxDefaultSize,
        wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, "wxBackstage")
{
    m_navWidth = FromDIP(200);

    wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetCanFocus(true);

    m_book = new wxSimplebook{ this, wxID_ANY };
    UpdatePageColours();
#if wxUSE_ACCESSIBILITY
    SetAccessible(new Accessible{ this });
#endif

    m_book->Bind(wxEVT_DESTROY, &wxBackstage::OnPageDestroyed, this);

    Bind(wxEVT_PAINT, &wxBackstage::OnPaintWindow, this);
    Bind(wxEVT_SIZE, &wxBackstage::OnResize, this);
    Bind(wxEVT_MOTION, &wxBackstage::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &wxBackstage::OnMouseDown, this);
    Bind(wxEVT_LEFT_DCLICK, &wxBackstage::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &wxBackstage::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &wxBackstage::OnMouseWheel, this);
    Bind(wxEVT_LEAVE_WINDOW, &wxBackstage::OnMouseLeave, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxBackstage::OnMouseCaptureLost, this);
    Bind(wxEVT_KEY_DOWN, &wxBackstage::OnKeyDown, this);
    Bind(wxEVT_NAVIGATION_KEY, &wxBackstage::OnNavigationKey, this);
    Bind(wxEVT_SET_FOCUS, &wxBackstage::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &wxBackstage::OnKillFocus, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED, &wxBackstage::OnSysColourChanged, this);
    Bind(wxEVT_DPI_CHANGED, &wxBackstage::OnDPIChanged, this);
}

//-------------------------------------------
wxBackstage::~wxBackstage()
{
    // our members are gone before the pages are destroyed, so stop listening
    m_book->Unbind(wxEVT_DESTROY, &wxBackstage::OnPageDestroyed, this);
}

//-------------------------------------------
void wxBackstage::OnPageDestroyed(wxWindowDestroyEvent& event)
{
    event.Skip();
    const auto itemIt = std::find_if(m_items.begin(), m_items.end(),
        [&event](const Item& item) noexcept { return item.m_page.get() == event.GetEventObject(); });
    if ( itemIt == m_items.end() )
    {
        return;
    }

    const wxWindowID destroyedId = itemIt->m_id;
    const int bookIndex = m_book->FindPage(itemIt->m_page.get());
    if ( bookIndex != wxNOT_FOUND )
    {
        m_book->RemovePage(static_cast<size_t>(bookIndex));
    }
    itemIt->m_page.Release();

    if ( m_selectedId == destroyedId )
    {
        m_selectedId = wxNOT_FOUND;
        for ( const auto& item : m_items )
        {
            if ( item.m_page.get() != nullptr && ShowPage(item.m_id) )
            {
                break;
            }
        }
    }
    Refresh();
}

//-------------------------------------------
const wxBackstage* wxBackstage::FindBackstage(const wxWindow* window)
{
    while ( window != nullptr )
    {
        if ( const auto* backstage = dynamic_cast<const wxBackstage*>(window) )
        {
            return backstage;
        }
        window = window->GetParent();
    }
    return nullptr;
}

//-------------------------------------------
void wxBackstage::GetPageColours(const wxWindow* window, wxColour& background,
                                wxColour& foreground)
{
    if ( const auto* backstage = FindBackstage(window) )
    {
        background = backstage->GetPageBackgroundColour();
        foreground = backstage->GetPageForegroundColour();
    }
    else
    {
        background = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        foreground = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    }
}

//-------------------------------------------
wxBackstagePaintBuffer::wxBackstagePaintBuffer(wxWindow* win) : m_buffer(win)
{
    wxBackstage::GetPageColours(win, m_background, m_foreground);
    m_buffer.SetBackground(wxBrush{ m_background });
    m_buffer.Clear();
}

//-------------------------------------------
wxBackstagePaintGraphics::wxBackstagePaintGraphics(wxWindow* win, const wxFont& font)
    : wxBackstagePaintBuffer(win), m_graphics(m_buffer)
{
    m_graphics.SetFont(font);
}

//-------------------------------------------
void wxBackstage::DrawBitmapFit(wxDC& dc, const wxWindow* window,
                                const wxBitmapBundle& bundle, const wxRect& rect)
{
    if ( !bundle.IsOk() || rect.IsEmpty() )
    {
        return;
    }
    wxSize defaultSize = bundle.GetDefaultSize();
    if ( defaultSize.GetWidth() <= 0 || defaultSize.GetHeight() <= 0 )
    {
        defaultSize = wxSize{ 1, 1 };
    }
    const double scale = std::min(
        static_cast<double>(rect.GetWidth()) / defaultSize.GetWidth(),
        static_cast<double>(rect.GetHeight()) / defaultSize.GetHeight());
    const wxSize fitted{ std::max(1, wxRound(defaultSize.GetWidth() * scale)),
                         std::max(1, wxRound(defaultSize.GetHeight() * scale)) };

    double contentScale = window->GetContentScaleFactor();
    if ( !(contentScale > 0.0) )
    {
        contentScale = 1.0;
    }
    const wxSize wantedPhys{ wxRound(fitted.GetWidth() * contentScale),
                             wxRound(fitted.GetHeight() * contentScale) };
    wxBitmap bmp = bundle.GetBitmap(wantedPhys);
    if ( bmp.IsOk() )
    {
        bmp.SetScaleFactor(contentScale);
        const wxSize logical = bmp.GetLogicalSize();
        dc.DrawBitmap(bmp,
            rect.GetLeft() + ((rect.GetWidth() - logical.GetWidth()) / 2),
            rect.GetTop() + ((rect.GetHeight() - logical.GetHeight()) / 2), true);
    }
}

//-------------------------------------------
wxSize wxBackstage::MeasureText(const wxWindow* window, const wxString& text,
                                const wxFont& font)
{
    if ( text.empty() )
    {
        return wxSize{ 0, 0 };
    }
    int lineHeight{ 0 };
    window->GetTextExtent("Ag", nullptr, &lineHeight, nullptr, nullptr, &font);

    const wxArrayString lines = wxSplit(text, '\n', '\0');
    int width{ 0 };
    for ( const auto& line : lines )
    {
        int lineWidth{ 0 };
        window->GetTextExtent(line, &lineWidth, nullptr, nullptr, nullptr, &font);
        width = std::max(width, lineWidth);
    }
    return wxSize{ width, lineHeight * static_cast<int>(lines.size()) };
}

//-------------------------------------------
long wxBackstage::FindItem(const wxWindowID id) const noexcept
{
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        if ( m_items[i].m_kind == ItemKind::Button && m_items[i].m_id == id )
        {
            return static_cast<long>(i);
        }
    }
    return wxNOT_FOUND;
}

//-------------------------------------------
long wxBackstage::HitTest(const wxPoint& pt) const noexcept
{
    if ( pt.x < 0 || pt.x >= m_navWidth )
    {
        return wxNOT_FOUND;
    }
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        if ( IsSelectable(i) && m_items[i].m_rect.Contains(pt) )
        {
            return static_cast<long>(i);
        }
    }
    return wxNOT_FOUND;
}

//-------------------------------------------
void wxBackstage::RefreshItem(const long index)
{
    if ( index >= 0 && static_cast<size_t>(index) < m_items.size() )
    {
        RefreshRect(m_items[index].m_rect);
    }
}

//-------------------------------------------
bool wxBackstage::AddButton(const wxWindowID id, const wxString& label,
                            const wxBitmapBundle& icon /*= wxBitmapBundle{}*/)
{
    wxASSERT_MSG(id != wxID_ANY, "Backstage buttons need an explicit ID!");
    wxASSERT_MSG(FindItem(id) == wxNOT_FOUND, "Duplicate backstage button ID!");
    if ( id == wxID_ANY || FindItem(id) != wxNOT_FOUND )
    {
        return false;
    }

    Item item;
    item.m_kind = ItemKind::Button;
    item.m_id = id;
    item.m_label = label;
    item.m_icon = icon;
    m_items.push_back(std::move(item));
    CalcLayout();
    Refresh();
    return true;
}

//-------------------------------------------
void wxBackstage::AddSeparator()
{
    Item item;
    item.m_kind = ItemKind::Separator;
    m_items.push_back(std::move(item));
    CalcLayout();
    Refresh();
}

//-------------------------------------------
void wxBackstage::AddFlexibleSpace()
{
    const bool hasSpace = std::any_of(m_items.cbegin(), m_items.cend(),
        [](const Item& item) noexcept { return item.m_kind == ItemKind::FlexibleSpace; });
    if ( hasSpace )
    {
        return;
    }
    Item item;
    item.m_kind = ItemKind::FlexibleSpace;
    m_items.push_back(std::move(item));
    CalcLayout();
    Refresh();
}

//-------------------------------------------
void wxBackstage::EnableButton(const wxWindowID id, const bool enable /*= true*/)
{
    const auto idx = FindItem(id);
    if ( idx == wxNOT_FOUND )
    {
        return;
    }
    m_items[idx].m_enabled = enable;
    NotifyAccessibility(AccessibleEvent::StateChange, idx);
    if ( !enable )
    {
        if ( m_hoverIndex == idx )
        {
            m_hoverIndex = wxNOT_FOUND;
        }
        if ( m_focusIndex == idx )
        {
            m_focusIndex = wxNOT_FOUND;
        }
    }
    RefreshItem(idx);
}

//-------------------------------------------
bool wxBackstage::IsButtonEnabled(const wxWindowID id) const
{
    const auto idx = FindItem(id);
    return idx != wxNOT_FOUND && m_items[idx].m_enabled;
}

//-------------------------------------------
void wxBackstage::SetButtonLabel(const wxWindowID id, const wxString& label)
{
    const auto idx = FindItem(id);
    if ( idx == wxNOT_FOUND )
    {
        return;
    }
    m_items[idx].m_label = label;
    NotifyAccessibility(AccessibleEvent::NameChange, idx);
    CalcLayout();
    Refresh();
}

//-------------------------------------------
wxBackstagePage* wxBackstage::AddPage(const wxWindowID buttonId)
{
    auto* page = new wxBackstagePage{ m_book };
    if ( !SetPage(buttonId, page) )
    {
        page->Destroy();
        return nullptr;
    }
    return page;
}

//-------------------------------------------
bool wxBackstage::SetPage(const wxWindowID buttonId, wxWindow* page)
{
    const auto idx = FindItem(buttonId);
    wxASSERT_MSG(idx != wxNOT_FOUND, "Add the button before adding its page!");
    if ( idx == wxNOT_FOUND || page == nullptr || m_items[idx].m_page.get() != nullptr )
    {
        return false;
    }

    if ( page->GetParent() != m_book )
    {
        page->Reparent(m_book);
    }
    ApplyPageColours(page);
    m_items[idx].m_page = page;
    m_book->AddPage(page, m_items[idx].m_label, false);
    if ( m_selectedId == wxNOT_FOUND )
    {
        ShowPage(buttonId);
    }
    return true;
}

//-------------------------------------------
bool wxBackstage::RemovePage(const wxWindowID buttonId)
{
    auto* page = GetPage(buttonId);
    if ( page == nullptr )
    {
        return false;
    }
    page->Destroy();
    return true;
}

//-------------------------------------------
wxWindow* wxBackstage::GetPage(const wxWindowID buttonId) const
{
    const auto idx = FindItem(buttonId);
    return idx == wxNOT_FOUND ? nullptr : m_items[idx].m_page.get();
}

//-------------------------------------------
bool wxBackstage::ShowPage(const wxWindowID buttonId)
{
    const auto idx = FindItem(buttonId);
    if ( idx == wxNOT_FOUND || m_items[idx].m_page.get() == nullptr )
    {
        return false;
    }
    const int bookIndex = m_book->FindPage(m_items[idx].m_page.get());
    if ( bookIndex == wxNOT_FOUND )
    {
        return false;
    }

    m_book->ChangeSelection(static_cast<size_t>(bookIndex));
    m_selectedId = buttonId;
    NotifyAccessibility(AccessibleEvent::Selection, idx);
    Refresh();
    return true;
}

//-------------------------------------------
void wxBackstage::SetNavBackgroundColour(const wxColour& colour)
{
    m_navBackgroundColour = colour;
    Refresh();
}

//-------------------------------------------
void wxBackstage::SetHighlightColour(const wxColour& colour)
{
    m_highlightColour = colour;
    Refresh();
}

//-------------------------------------------
void wxBackstage::SetHighlightStyle(const wxBackstageHighlightStyle style)
{
    m_highlightStyle = style;
    Refresh();
}

//-------------------------------------------
void wxBackstage::SetPageBackgroundColour(const wxColour& colour)
{
    m_pageBackgroundColour = colour;
    UpdatePageColours();
    Refresh();
}

//-------------------------------------------
void wxBackstage::KeepWindowColours(wxWindow* window)
{
    if ( window != nullptr && !AreColoursKept(window) )
    {
        m_keptColourWindows.emplace_back(window);
    }
}

//-------------------------------------------
bool wxBackstage::AreColoursKept(const wxWindow* window) const
{
    return std::any_of(m_keptColourWindows.cbegin(), m_keptColourWindows.cend(),
        [window](const wxWeakRef<wxWindow>& ref) noexcept { return ref.get() == window; });
}

//-------------------------------------------
void wxBackstage::ApplyPageColours(wxWindow* page) const
{
    page->SetBackgroundColour(GetPageBackgroundColour());
    page->SetForegroundColour(GetPageForegroundColour());
}

//-------------------------------------------
void wxBackstage::UpdatePageColours()
{
    const wxColour bg = GetPageBackgroundColour();
    const wxColour fg = GetPageForegroundColour();
    const wxColour oldBg = m_appliedPageBackground;
    const wxColour oldFg = m_appliedPageForeground;

    m_book->SetBackgroundColour(bg);

    // Controls that inherited the old page colors follow the new ones.
    std::function<void(wxWindow*)> apply;
    apply = [&](wxWindow* window)
        {
            for ( auto* child : window->GetChildren() )
            {
                if ( AreColoursKept(child) )
                {
                    continue;
                }
                bool changed{ false };
                if ( oldBg.IsOk() && child->GetBackgroundColour() == oldBg )
                {
                    child->SetBackgroundColour(bg);
                    changed = true;
                }
                if ( oldFg.IsOk() && child->GetForegroundColour() == oldFg )
                {
                    child->SetForegroundColour(fg);
                    changed = true;
                }
                if ( changed )
                {
                    child->Refresh();
                }
                apply(child);
            }
        };

    for ( const auto& item : m_items )
    {
        auto* page = item.m_page.get();
        if ( page != nullptr )
        {
            ApplyPageColours(page);
            apply(page);
            page->Refresh();
        }
    }

    m_appliedPageBackground = bg;
    m_appliedPageForeground = fg;
}

//-------------------------------------------
void wxBackstage::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    UpdatePageColours();
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstage::OnDPIChanged(wxDPIChangedEvent& event)
{
    CalcLayout();
    Refresh();
    event.Skip();
}

//-------------------------------------------
wxBackstage::NavMetrics wxBackstage::GetNavMetrics() const
{
    const bool anyIcons = std::any_of(m_items.cbegin(), m_items.cend(),
        [](const Item& item) { return item.m_icon.IsOk(); });
    return NavMetrics{ FromDIP(24), FromDIP(11), FromDIP(10), FromDIP(12),
                       FromDIP(17), GetIconSize(), anyIcons };
}

//-------------------------------------------
void wxBackstage::CalcLayout()
{
    const wxSize clientSize = GetClientSize();
    wxClientDC dc{ this };
    dc.SetFont(GetNavFont());

    const NavMetrics metrics = GetNavMetrics();
    const wxCoord textHeight = dc.GetTextExtent("Ag").GetHeight();
    const wxCoord buttonHeight =
        std::max(textHeight, metrics.m_anyIcons ? metrics.m_iconSize.GetHeight() : 0) +
        (2 * metrics.m_padY);

    wxCoord width = FromDIP(180);
    for ( const auto& item : m_items )
    {
        if ( item.m_kind == ItemKind::Button )
        {
            width = std::max(width, dc.GetTextExtent(item.m_label).GetWidth() +
                (2 * metrics.m_padX) +
                (metrics.m_anyIcons ? metrics.m_iconSize.GetWidth() + metrics.m_iconGap : 0));
        }
    }
    const wxCoord maxWidth = std::min(FromDIP(320), std::max(0, clientSize.GetWidth() / 2));
    m_navWidth = std::min(width, maxWidth);

    const auto heightOf = [&](const Item& item)
        {
            return item.m_kind == ItemKind::Button ? buttonHeight :
                item.m_kind == ItemKind::Separator ? metrics.m_separatorHeight : 0;
        };

    const auto flexIt = std::find_if(m_items.cbegin(), m_items.cend(),
        [](const Item& item) noexcept { return item.m_kind == ItemKind::FlexibleSpace; });
    const size_t flexIndex = static_cast<size_t>(std::distance(m_items.cbegin(), flexIt));
    const bool hasFlex = (flexIt != m_items.cend());

    wxCoord y = metrics.m_border;
    for ( size_t i = 0; i < std::min(flexIndex, m_items.size()); ++i )
    {
        m_items[i].m_rect = wxRect{ 0, y, m_navWidth, heightOf(m_items[i]) };
        y += m_items[i].m_rect.GetHeight();
    }
    const wxCoord topEnd = y;

    if ( hasFlex )
    {
        m_items[flexIndex].m_rect = wxRect{};
        wxCoord bottomHeight{ 0 };
        for ( size_t i = flexIndex + 1; i < m_items.size(); ++i )
        {
            bottomHeight += heightOf(m_items[i]);
        }
        y = std::max(topEnd, clientSize.GetHeight() - metrics.m_border - bottomHeight);
        for ( size_t i = flexIndex + 1; i < m_items.size(); ++i )
        {
            m_items[i].m_rect = wxRect{ 0, y, m_navWidth, heightOf(m_items[i]) };
            y += m_items[i].m_rect.GetHeight();
        }
    }
    m_contentHeight = y + metrics.m_border;

    const int maxScroll = std::max(0, m_contentHeight - clientSize.GetHeight());
    m_scrollOffset = std::min(std::max(m_scrollOffset, 0), maxScroll);
    if ( m_scrollOffset != 0 )
    {
        for ( auto& item : m_items )
        {
            item.m_rect.Offset(0, -m_scrollOffset);
        }
    }

    m_book->SetSize(m_navWidth, 0,
        std::max(0, clientSize.GetWidth() - m_navWidth), clientSize.GetHeight());
}

//-------------------------------------------
void wxBackstage::OnResize(wxSizeEvent& event)
{
    CalcLayout();
    event.Skip();
}

//-------------------------------------------
void wxBackstage::DrawGlossyRect(wxDC& dc, const wxRect& rect, const wxColour& colour)
{
    const int topHeight = rect.GetHeight() / 2;
    const wxRect topRect{ rect.GetX(), rect.GetY(), rect.GetWidth(), topHeight };
    const wxRect bottomRect{ rect.GetX(), rect.GetY() + topHeight, rect.GetWidth(),
                             rect.GetHeight() - topHeight };
    dc.GradientFillLinear(topRect, Blend(colour, *wxWHITE, 0.35), Blend(colour, *wxWHITE, 0.10),
                          wxSOUTH);
    dc.GradientFillLinear(bottomRect, Blend(colour, *wxBLACK, 0.06), Blend(colour, *wxWHITE, 0.12),
                          wxSOUTH);
    const wxDCPenChanger pc{ dc, wxPen{ Blend(colour, *wxBLACK, 0.30), 1 } };
    dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight() + 1, rect.GetTop());
    dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());
}

//-------------------------------------------
void wxBackstage::DrawFocusRect(wxDC& dc, const wxRect& rect, const wxColour& colour)
{
    const wxDCPenChanger pc{ dc, wxPen{ colour, 1, wxPENSTYLE_DOT } };
    const wxDCBrushChanger bc{ dc, *wxTRANSPARENT_BRUSH };
    dc.DrawRectangle(rect);
}

//-------------------------------------------
void wxBackstage::OnPaintWindow(wxPaintEvent& WXUNUSED(event))
{
    wxBackstagePaintGraphics paint{ this, GetNavFont() };
    wxDC& dc = paint.m_graphics;

    const wxColour navColour = GetNavBackgroundColour();
    const wxRect navRect{ 0, 0, m_navWidth, GetClientSize().GetHeight() };
    {
        const wxDCPenChanger pc{ dc, *wxTRANSPARENT_PEN };
        const wxDCBrushChanger bc{ dc, wxBrush{ navColour } };
        dc.DrawRectangle(navRect);
    }
    dc.SetClippingRegion(navRect);

    const wxColour navTextColour = BlackOrWhiteContrast(navColour);
    const bool glossy =
        (m_highlightStyle == wxBackstageHighlightStyle::wxBackstageHighlightGlossy);
    const bool customFill = glossy || m_highlightColour.IsOk();
    const wxColour highlightColour =
        m_highlightColour.IsOk() ? m_highlightColour : ShadeOrTint(navColour, 0.22);
    const NavMetrics metrics = GetNavMetrics();

    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        const auto& item = m_items[i];
        if ( item.m_kind == ItemKind::FlexibleSpace ||
            item.m_rect.GetBottom() < 0 || item.m_rect.GetTop() > navRect.GetBottom() )
        {
            continue;
        }

        if ( item.m_kind == ItemKind::Separator )
        {
            const wxDCPenChanger pc{ dc,
                wxPen{ Blend(navColour, navTextColour, 0.3), std::max(1, FromDIP(1)) } };
            const wxCoord lineY = item.m_rect.GetTop() + (item.m_rect.GetHeight() / 2);
            dc.DrawLine(metrics.m_padX, lineY, m_navWidth - metrics.m_padX, lineY);
            continue;
        }

        const bool isSelected = (item.m_page.get() != nullptr && item.m_id == m_selectedId);
        wxColour fillColour = navColour;
        if ( item.m_enabled )
        {
            if ( static_cast<long>(i) == m_pressedIndex )
            {
                fillColour = customFill ? Blend(highlightColour, *wxBLACK, 0.15) :
                                         ShadeOrTint(navColour, 0.3);
            }
            else if ( isSelected )
            {
                fillColour = highlightColour;
            }
            else if ( static_cast<long>(i) == m_hoverIndex )
            {
                fillColour = customFill ? Blend(highlightColour, *wxWHITE, 0.25) :
                                         ShadeOrTint(navColour, 0.12);
            }
        }
        if ( fillColour != navColour )
        {
            if ( glossy )
            {
                DrawGlossyRect(dc, item.m_rect, fillColour);
            }
            else
            {
                const wxDCPenChanger pc{ dc, *wxTRANSPARENT_PEN };
                const wxDCBrushChanger bc{ dc, wxBrush{ fillColour } };
                dc.DrawRectangle(item.m_rect);
            }
        }

        wxColour textColour = BlackOrWhiteContrast(fillColour);
        if ( !item.m_enabled )
        {
            textColour = Blend(textColour, fillColour, 0.55);
        }
        const wxDCTextColourChanger tcc{ dc, textColour };

        wxCoord textX = item.m_rect.GetLeft() + metrics.m_padX;
        if ( item.m_icon.IsOk() )
        {
            DrawBitmapFit(dc, this, item.m_icon,
                wxRect{ wxPoint{ textX, item.m_rect.GetTop() +
                                    ((item.m_rect.GetHeight() - metrics.m_iconSize.GetHeight()) / 2) },
                        metrics.m_iconSize });
        }
        if ( metrics.m_anyIcons )
        {
            textX += metrics.m_iconSize.GetWidth() + metrics.m_iconGap;
        }
        const wxString label = wxControl::Ellipsize(item.m_label, dc, wxELLIPSIZE_END,
                                                     std::max(0, m_navWidth - textX - metrics.m_padX));
        const wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label, textX,
            item.m_rect.GetTop() + ((item.m_rect.GetHeight() - textSize.GetHeight()) / 2));

        if ( HasFocus() && m_showFocusRect && static_cast<long>(i) == m_focusIndex )
        {
            DrawFocusRect(dc, item.m_rect.Deflate(FromDIP(2), FromDIP(2)), textColour);
        }

        if ( isSelected && item.m_enabled )
        {
            const wxCoord arrowSize = std::min(FromDIP(9), item.m_rect.GetHeight() / 2);
            const wxCoord edgeX = item.m_rect.GetRight() + 1;
            const wxCoord midY = item.m_rect.GetTop() + (item.m_rect.GetHeight() / 2);
            const std::array<wxPoint, 3> notch = {{ wxPoint{ edgeX, midY - arrowSize },
                                                    wxPoint{ edgeX - arrowSize, midY },
                                                    wxPoint{ edgeX, midY + arrowSize } }};
            const wxDCPenChanger pc{ dc, *wxTRANSPARENT_PEN };
            const wxDCBrushChanger bc{ dc, wxBrush{ GetPageBackgroundColour() } };
            dc.DrawPolygon(static_cast<int>(notch.size()), notch.data());
        }
    }
}

//-------------------------------------------
void wxBackstage::OnMouseMove(wxMouseEvent& event)
{
    const long hit = HitTest(event.GetPosition());
    if ( hit != m_hoverIndex )
    {
        const long previous = m_hoverIndex;
        m_hoverIndex = hit;
        RefreshItem(previous);
        RefreshItem(hit);
    }
    event.Skip();
}

//-------------------------------------------
void wxBackstage::OnMouseDown(wxMouseEvent& event)
{
    SetFocus();
    m_showFocusRect = false;
    const long hit = HitTest(event.GetPosition());
    if ( hit != wxNOT_FOUND )
    {
        m_pressedIndex = hit;
        m_focusIndex = hit;
        NotifyAccessibility(AccessibleEvent::Focus, hit);
        if ( !HasCapture() )
        {
            CaptureMouse();
        }
        RefreshItem(hit);
    }
}

//-------------------------------------------
void wxBackstage::OnMouseUp(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        ReleaseMouse();
    }
    const long pressed = m_pressedIndex;
    m_pressedIndex = wxNOT_FOUND;
    RefreshItem(pressed);
    if ( pressed != wxNOT_FOUND && HitTest(event.GetPosition()) == pressed )
    {
        ActivateItem(static_cast<size_t>(pressed));
    }
}

//-------------------------------------------
void wxBackstage::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    RefreshItem(m_pressedIndex);
    m_pressedIndex = wxNOT_FOUND;
}

//-------------------------------------------
void wxBackstage::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
    if ( m_hoverIndex != wxNOT_FOUND )
    {
        RefreshItem(m_hoverIndex);
        m_hoverIndex = wxNOT_FOUND;
    }
}

//-------------------------------------------
void wxBackstage::OnMouseWheel(wxMouseEvent& event)
{
    if ( m_contentHeight <= GetClientSize().GetHeight() || event.GetX() >= m_navWidth )
    {
        event.Skip();
        return;
    }
    // add all small touchpad rotations into whole lines
    const int delta = event.GetWheelDelta() > 0 ? event.GetWheelDelta() : 120;
    m_wheelRotation += event.GetWheelRotation();
    const int lines = m_wheelRotation / delta;
    m_wheelRotation -= lines * delta;
    if ( lines == 0 )
    {
        return;
    }
    m_scrollOffset -= lines * FromDIP(40);
    CalcLayout();
    m_hoverIndex = HitTest(event.GetPosition());
    Refresh();
}

//-------------------------------------------
void wxBackstage::OnSetFocus(wxFocusEvent& event)
{
    m_showFocusRect = true;
    if ( m_focusIndex == wxNOT_FOUND || !IsSelectable(static_cast<size_t>(m_focusIndex)) )
    {
        m_focusIndex = wxNOT_FOUND;
        const auto selectedIdx = FindItem(m_selectedId);
        if ( selectedIdx != wxNOT_FOUND && IsSelectable(static_cast<size_t>(selectedIdx)) )
        {
            m_focusIndex = selectedIdx;
        }
        else
        {
            for ( size_t i = 0; i < m_items.size(); ++i )
            {
                if ( IsSelectable(i) )
                {
                    m_focusIndex = static_cast<long>(i);
                    break;
                }
            }
        }
    }
    NotifyAccessibility(AccessibleEvent::Focus, m_focusIndex);
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstage::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

//-------------------------------------------
void wxBackstage::MoveFocus(const int direction)
{
    if ( m_items.empty() )
    {
        return;
    }
    const long count = static_cast<long>(m_items.size());
    long idx = m_focusIndex;
    if ( idx == wxNOT_FOUND )
    {
        idx = direction > 0 ? -1 : count;
    }
    for ( long attempts = 0; attempts < count; ++attempts )
    {
        idx = (idx + direction + count) % count;
        if ( IsSelectable(static_cast<size_t>(idx)) )
        {
            m_focusIndex = idx;
            const auto& rect = m_items[idx].m_rect;
            if ( rect.GetTop() < 0 )
            {
                m_scrollOffset += rect.GetTop();
            }
            else if ( rect.GetBottom() >= GetClientSize().GetHeight() )
            {
                m_scrollOffset += rect.GetBottom() - GetClientSize().GetHeight() + 1;
            }
            CalcLayout();
            NotifyAccessibility(AccessibleEvent::Focus, idx);
            Refresh();
            return;
        }
    }
}

//-------------------------------------------
void wxBackstage::OnNavigationKey(wxNavigationKeyEvent& event)
{
    // leave window-change keys (e.g., Ctrl+Tab) to the default handling
    if ( event.IsWindowChange() )
    {
        event.Skip();
        return;
    }
    // The page asks its parents where to put the focus when it runs out of
    // controls. Take it back onto the nav (the left sidebar).
    for ( wxWindow* focus = event.GetCurrentFocus(); focus != nullptr;
         focus = focus->GetParent() )
    {
        if ( focus == m_book )
        {
            SetFocus();
            event.Skip(false);
            return;
        }
        if ( focus == this )
        {
            break;
        }
    }
    event.Skip();
}

//-------------------------------------------
void wxBackstage::OnKeyDown(wxKeyEvent& event)
{
    if ( SkipIfShortcutKey(event) )
    {
        return;
    }
    m_showFocusRect = true;
    switch ( event.GetKeyCode() )
    {
    case WXK_UP:
        MoveFocus(-1);
        break;
    case WXK_DOWN:
        MoveFocus(1);
        break;
    case WXK_HOME:
        m_focusIndex = wxNOT_FOUND;
        MoveFocus(1);
        break;
    case WXK_END:
        m_focusIndex = wxNOT_FOUND;
        MoveFocus(-1);
        break;
    case WXK_TAB:
        {
            auto* selectedPage = GetPage(m_selectedId);
            if ( !event.ShiftDown() && selectedPage != nullptr )
            {
                selectedPage->SetFocus();
            }
            else
            {
                Navigate(event.ShiftDown() ?
                    wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
            }
        }
        break;
    default:
        if ( IsActivateKey(event.GetKeyCode()) && !event.IsAutoRepeat() &&
            m_focusIndex != wxNOT_FOUND &&
            IsSelectable(static_cast<size_t>(m_focusIndex)) )
        {
            ActivateItem(static_cast<size_t>(m_focusIndex));
        }
        else
        {
            event.Skip();
        }
        break;
    }
}

//-------------------------------------------
void wxBackstage::ActivateItem(const size_t index)
{
    if ( !IsSelectable(index) )
    {
        return;
    }
    // the handler could change or destroy items, so hold onto them
    const wxWindowID id = m_items[index].m_id;
    const wxWeakRef<wxBackstage> self{ this };

    wxNotifyEvent event{ wxEVT_BACKSTAGE_CLICKED, id };
    event.SetEventObject(this);
    event.SetInt(id);
    event.SetString(m_items[index].m_label);
    GetEventHandler()->ProcessEvent(event);

    if ( self && event.IsAllowed() )
    {
        self->ShowPage(id);
    }
}

#endif // wxUSE_RIBBON
