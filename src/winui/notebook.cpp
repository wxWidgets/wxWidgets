/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/notebook.cpp
// Purpose:     wxWinUI wxNotebook implementation (WinUI TabView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

#include <cmath>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

class wxWinUINotebookImpl
{
public:
    wxWinUIControlHost host;
    MUXC::TabView tabView{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token layoutUpdatedToken{};

    // One TabViewItem and one content-area marker (an empty Border that fills
    // the TabView content region) per page, kept parallel to wxBookCtrlBase's
    // m_pages array.  The marker is measured to position the wx page window.
    std::vector<MUXC::TabViewItem> items;
    std::vector<MUXC::Border> markers;
};

void wxNotebook::Init()
{
    m_tabStripHeightDIP = -1;
    m_updating = false;
}

wxNotebook::wxNotebook()
{
    Init();
}

wxNotebook::wxNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, name);
}

wxNotebook::~wxNotebook()
{
}

bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    // The TabView draws its own surface; suppress the native control border.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUINotebookImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->tabView = MUXC::TabView();
        m_winui->tabView.IsAddTabButtonVisible(false);
        m_winui->tabView.CanReorderTabs(false);
        m_winui->tabView.CanDragTabs(false);
        m_winui->tabView.TabWidthMode(MUXC::TabViewWidthMode::SizeToContent);

        m_winui->selectionChangedToken = m_winui->tabView.SelectionChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUXC::SelectionChangedEventArgs const&)
            {
                if ( !m_winui || m_updating )
                    return;
                OnTabViewSelectionChanged(m_winui->tabView.SelectedIndex());
            });

        m_winui->layoutUpdatedToken = m_winui->tabView.LayoutUpdated(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&)
            {
                if ( m_winui )
                    UpdateTabStripHeightFromLayout();
            });

        m_winui->host.SetContent(m_winui->tabView);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView creation", e);
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// page management
// ----------------------------------------------------------------------------

bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxCHECK_MSG( pPage, false, wxT("null page in wxNotebook::InsertPage()") );
    wxCHECK_MSG( nPage <= m_pages.size(), false,
                 wxT("invalid page index in wxNotebook::InsertPage()") );

    MUXC::TabViewItem item;
    MUXC::Border marker;
    try
    {
        item.Header(winrt::box_value(wxWinUIToHString(strText)));
        item.IsClosable(false);
        item.Content(marker);

        m_updating = true;
        m_winui->tabView.TabItems().InsertAt(static_cast<uint32_t>(nPage), item);
        m_updating = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        m_updating = false;
        wxWinUILogException("WinUI TabView InsertPage", e);
        return false;
    }

    m_pages.insert(m_pages.begin() + nPage, pPage);
    m_pageTexts.Insert(strText, nPage);
    m_pageImages.Insert(imageId, nPage);
    m_winui->items.insert(m_winui->items.begin() + nPage, item);
    m_winui->markers.insert(m_winui->markers.begin() + nPage, marker);

    if ( imageId != NO_IMAGE )
        UpdateTabIcon(nPage);

    // The page is hidden until it becomes the selected one.
    pPage->Show(false);
    pPage->SetSize(GetPageRect());

    DoInvalidateBestSize();

    if ( !DoSetSelectionAfterInsertion(nPage, bSelect) )
        pPage->Show(false);

    return true;
}

wxWindow *wxNotebook::DoRemovePage(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), nullptr,
                 wxT("invalid page index in wxNotebook::DoRemovePage()") );

    wxWindow *page = m_pages[nPage];

    try
    {
        m_updating = true;
        m_winui->tabView.TabItems().RemoveAt(static_cast<uint32_t>(nPage));
        m_updating = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        m_updating = false;
        wxWinUILogException("WinUI TabView DoRemovePage", e);
    }

    m_pages.erase(m_pages.begin() + nPage);
    m_pageTexts.RemoveAt(nPage);
    m_pageImages.RemoveAt(nPage);
    m_winui->items.erase(m_winui->items.begin() + nPage);
    m_winui->markers.erase(m_winui->markers.begin() + nPage);

    DoSetSelectionAfterRemoval(nPage);
    DoInvalidateBestSize();

    return page;
}

bool wxNotebook::DeleteAllPages()
{
    try
    {
        m_updating = true;
        m_winui->tabView.TabItems().Clear();
        m_updating = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        m_updating = false;
        wxWinUILogException("WinUI TabView DeleteAllPages", e);
    }

    m_winui->items.clear();
    m_winui->markers.clear();
    m_pageTexts.Clear();
    m_pageImages.Clear();

    return wxNotebookBase::DeleteAllPages();
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxNotebook::SetSelection(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxNotebook::SetSelection()") );

    return DoSetSelection(nPage, SetSelection_SendEvent);
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxNotebook::ChangeSelection()") );

    return DoSetSelection(nPage);
}

void wxNotebook::UpdateSelectedPage(size_t newsel)
{
    if ( !m_winui || !m_winui->tabView )
        return;

    m_updating = true;
    try
    {
        m_winui->tabView.SelectedIndex(static_cast<int>(newsel));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;

    m_winui->host.ForceRender();

    if ( wxWindow *page = GetPage(newsel) )
        page->SetFocus();
}

void wxNotebook::OnTabViewSelectionChanged(int sel)
{
    if ( sel == wxNOT_FOUND || sel == m_selection )
        return;

    DoSetSelection(static_cast<size_t>(sel), SetSelection_SendEvent);
}

wxBookCtrlEvent* wxNotebook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_NOTEBOOK_PAGE_CHANGING, GetId());
}

void wxNotebook::MakeChangedEvent(wxBookCtrlEvent& event)
{
    event.SetEventType(wxEVT_NOTEBOOK_PAGE_CHANGED);
}

// ----------------------------------------------------------------------------
// page attributes
// ----------------------------------------------------------------------------

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    wxCHECK_MSG( nPage < m_pages.size(), false, wxT("invalid notebook page") );

    m_pageTexts[nPage] = strText;
    try
    {
        m_winui->items[nPage].Header(winrt::box_value(wxWinUIToHString(strText)));
        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return true;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    wxCHECK_MSG( nPage < m_pageTexts.GetCount(), wxString(), wxT("invalid notebook page") );
    return m_pageTexts[nPage];
}

int wxNotebook::GetPageImage(size_t nPage) const
{
    wxCHECK_MSG( nPage < m_pageImages.GetCount(), NO_IMAGE, wxT("invalid notebook page") );
    return m_pageImages[nPage];
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
    wxCHECK_MSG( nPage < m_pageImages.GetCount(), false, wxT("invalid notebook page") );

    m_pageImages[nPage] = nImage;
    UpdateTabIcon(nPage);
    return true;
}

void wxNotebook::UpdateTabIcon(size_t nPage)
{
    if ( !m_winui || nPage >= m_winui->items.size() )
        return;

    try
    {
        auto& item = m_winui->items[nPage];

        const int imageId = m_pageImages[nPage];
        wxBitmap bmp;
        if ( imageId != NO_IMAGE )
            bmp = GetImageBitmapFor(this, imageId);

        if ( bmp.IsOk() )
        {
            if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
            {
                MUXC::ImageIconSource icon;
                icon.ImageSource(source);
                item.IconSource(icon);
            }
        }
        else
        {
            item.IconSource(nullptr);
        }

        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView page icon", e);
    }
}

void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
    // Tab padding is controlled by the WinUI TabView template.
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
    // Not supported: WinUI sizes tabs to content.
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // Leave room for the tab strip on top and a small margin around the page.
    return wxSize(sizePage.x + FromDIP(8),
                  sizePage.y + FromDIP(48));
}

wxRect wxNotebook::GetPageRect() const
{
    // Derive the page area deterministically from the current client size and
    // the (cached) tab-strip height.  Computing it from the live size on every
    // call -- rather than resizing pages from the asynchronous layout callback
    // -- avoids repeated SetSize() calls that would otherwise reset the scroll
    // position of a scrolled page on each TabView layout pass.
    const wxSize client = GetClientSize();
    const int tabH = FromDIP(m_tabStripHeightDIP > 0 ? m_tabStripHeightDIP : 40);

    wxRect rect(0, tabH, client.x, client.y - tabH);
    if ( rect.height < 0 )
        rect.height = 0;
    return rect;
}

void wxNotebook::DoSize()
{
    const wxRect rect = GetPageRect();
    for ( size_t i = 0; i < m_pages.size(); ++i )
    {
        if ( wxWindow *page = m_pages[i] )
            page->SetSize(rect);
    }
}

void wxNotebook::UpdateTabStripHeightFromLayout()
{
    if ( !m_winui || !m_winui->tabView || m_selection == wxNOT_FOUND )
        return;

    if ( m_selection < 0 ||
         static_cast<size_t>(m_selection) >= m_winui->markers.size() )
        return;

    try
    {
        // The content marker fills the TabView content region, so the vertical
        // offset of its top edge (in DIPs) is exactly the tab-strip height.
        const MUXC::Border marker = m_winui->markers[m_selection];
        if ( marker.ActualHeight() <= 0 )
            return;

        const auto transform = marker.TransformToVisual(m_winui->tabView);
        const auto origin =
            transform.TransformPoint(winrt::Windows::Foundation::Point{ 0, 0 });

        const int tabStrip = static_cast<int>(std::lround(origin.Y));
        if ( tabStrip <= 0 || tabStrip == m_tabStripHeightDIP )
            return;

        m_tabStripHeightDIP = tabStrip;

        // Shrink the TabView island to the tab strip only, so the page area
        // below it (and the pages' native scrollbars) is not covered by the
        // island's composition surface.
        m_winui->host.SetBridgeHeightLimit(FromDIP(tabStrip));

        DoSize();
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

#endif // wxUSE_NOTEBOOK
