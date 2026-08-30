///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/auitest.cpp
// Purpose:     wxAui control tests
// Author:      Sebastian Walderich
// Created:     2018-12-19
// Copyright:   (c) 2018 Sebastian Walderich
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_AUI


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
#endif // WX_PRECOMP

#include "wx/panel.h"
#include "wx/dcmemory.h"

#include "wx/aui/auibar.h"
#include "wx/aui/auibook.h"
#include "wx/aui/framemanager.h"
#include "wx/aui/serializer.h"
#include "wx/weakref.h"

#include "asserthelper.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace
{

struct WindowDestroyingTabArtState
{
    wxWindow* victim = nullptr;
    bool armed = false;
    unsigned int calls = 0;
};

class WindowDestroyingTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit WindowDestroyingTabArt(
        const std::shared_ptr<WindowDestroyingTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        return new WindowDestroyingTabArt(m_state);
    }

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount,
                       wxWindow* wnd = nullptr) override
    {
        // Keep the state alive because the callback below destroys the art
        // provider currently executing this method.
        const auto state = m_state;
        wxAuiDefaultTabArt::SetSizingInfo(tabCtrlSize, tabCount, wnd);

        if ( state->armed && state->victim )
        {
            wxWindow* const victim = state->victim;
            state->victim = nullptr;
            state->armed = false;
            ++state->calls;
            delete victim;
        }
    }

private:
    std::shared_ptr<WindowDestroyingTabArtState> m_state;
};

enum class TabArtCallbackBoundary
{
    BestSize,
    Clone
};

struct BoundaryCallbackTabArtState
{
    TabArtCallbackBoundary boundary = TabArtCallbackBoundary::BestSize;
    std::function<void()> callback;
    bool armed = false;
    int heightDelta = 0;
    unsigned int bestSizeCalls = 0;
    unsigned int cloneCalls = 0;
    unsigned int callbackCalls = 0;
};

class BoundaryCallbackTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit BoundaryCallbackTabArt(
        const std::shared_ptr<BoundaryCallbackTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        // Keep all state needed after the callback in locals: the callback is
        // deliberately allowed to delete the window owning this art object.
        const auto state = m_state;
        ++state->cloneCalls;
        wxAuiTabArt* const clone = new BoundaryCallbackTabArt(state);
        InvokeCallback(state, TabArtCallbackBoundary::Clone);
        return clone;
    }

    int GetBestTabCtrlSize(wxWindow* wnd,
                           const wxAuiNotebookPageArray& pages,
                           const wxSize& requiredBmpSize) override
    {
        const auto state = m_state;
        const int height = wxAuiDefaultTabArt::GetBestTabCtrlSize(
            wnd, pages, requiredBmpSize) + state->heightDelta;
        ++state->bestSizeCalls;
        InvokeCallback(state, TabArtCallbackBoundary::BestSize);
        return height;
    }

private:
    static void InvokeCallback(
        const std::shared_ptr<BoundaryCallbackTabArtState>& state,
        TabArtCallbackBoundary boundary)
    {
        if ( !state->armed || state->boundary != boundary )
            return;

        // Disarm before entering application code so any nested sizing pass
        // observes the stable art metrics without recursively firing again.
        state->armed = false;
        ++state->callbackCalls;
        const auto callback = state->callback;
        if ( callback )
            callback();
    }

    std::shared_ptr<BoundaryCallbackTabArtState> m_state;
};

enum class StyleCallbackBoundary
{
    SetFlags,
    Sizing
};

struct StyleCallbackTabArtState
{
    StyleCallbackBoundary boundary = StyleCallbackBoundary::SetFlags;
    std::function<void()> callback;
    bool armed = false;
    unsigned int callbackCalls = 0;
    std::vector<unsigned int> flags;
};

class StyleCallbackTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit StyleCallbackTabArt(
        const std::shared_ptr<StyleCallbackTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        return new StyleCallbackTabArt(m_state);
    }

    void SetFlags(unsigned int flags) override
    {
        // Publish and copy all continuation state before the callback: it is
        // intentionally allowed to delete the tab control owning this art.
        const auto state = m_state;
        wxAuiDefaultTabArt::SetFlags(flags);
        state->flags.push_back(flags);
        InvokeCallback(state, StyleCallbackBoundary::SetFlags);
    }

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount,
                       wxWindow* wnd = nullptr) override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::SetSizingInfo(tabCtrlSize, tabCount, wnd);
        InvokeCallback(state, StyleCallbackBoundary::Sizing);
    }

private:
    static void InvokeCallback(
        const std::shared_ptr<StyleCallbackTabArtState>& state,
        StyleCallbackBoundary boundary)
    {
        if ( !state->armed || state->boundary != boundary )
            return;

        state->armed = false;
        ++state->callbackCalls;
        const auto callback = state->callback;
        if ( callback )
            callback();
    }

    std::shared_ptr<StyleCallbackTabArtState> m_state;
};

enum class FontCallbackBoundary
{
    GetNormal,
    SetNormal,
    SetSelected
};

struct FontCallbackTabArtState
{
    FontCallbackBoundary boundary = FontCallbackBoundary::GetNormal;
    std::function<void()> callback;
    wxFont getNormalResult;
    wxFont getSelectedResult;
    // A default-constructed wxFont is valid on some ports (notably Qt), so
    // font validity cannot indicate whether a query override was requested.
    bool overrideNormalResult = false;
    bool overrideSelectedResult = false;
    int heightDelta = 0;
    bool armed = false;
    unsigned int callbackCalls = 0;
    std::vector<wxFont> normalFonts;
    std::vector<wxFont> selectedFonts;
    std::vector<const wxAuiTabArt*> instances;

    bool Owns(const wxAuiTabArt* art) const
    {
        return std::find(instances.begin(), instances.end(), art) !=
               instances.end();
    }
};

class FontCallbackTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit FontCallbackTabArt(
        const std::shared_ptr<FontCallbackTabArtState>& state)
        : m_state(state)
    {
        m_state->instances.push_back(this);
    }

    ~FontCallbackTabArt() override
    {
        const auto it = std::find(m_state->instances.begin(),
                                  m_state->instances.end(), this);
        if ( it != m_state->instances.end() )
            m_state->instances.erase(it);
    }

    wxAuiTabArt* Clone() override
    {
        return new FontCallbackTabArt(m_state);
    }

    int GetBestTabCtrlSize(wxWindow* wnd,
                           const wxAuiNotebookPageArray& pages,
                           const wxSize& requiredBmpSize) override
    {
        return wxAuiDefaultTabArt::GetBestTabCtrlSize(
                   wnd, pages, requiredBmpSize) + m_state->heightDelta;
    }

    wxFont GetNormalFont() const override
    {
        const auto state = m_state;
        const wxFont result =
            state->armed &&
                    state->boundary == FontCallbackBoundary::GetNormal &&
                    state->overrideNormalResult
                ? state->getNormalResult
                : wxAuiDefaultTabArt::GetNormalFont();
        InvokeCallback(state, FontCallbackBoundary::GetNormal);
        return result;
    }

    wxFont GetSelectedFont() const override
    {
        return m_state->overrideSelectedResult
            ? m_state->getSelectedResult
            : wxAuiDefaultTabArt::GetSelectedFont();
    }

    void SetNormalFont(const wxFont& font) override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::SetNormalFont(font);
        state->normalFonts.push_back(font);
        InvokeCallback(state, FontCallbackBoundary::SetNormal);
    }

    void SetSelectedFont(const wxFont& font) override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::SetSelectedFont(font);
        state->selectedFonts.push_back(font);
        InvokeCallback(state, FontCallbackBoundary::SetSelected);
    }

private:
    static void InvokeCallback(
        const std::shared_ptr<FontCallbackTabArtState>& state,
        FontCallbackBoundary boundary)
    {
        if ( !state->armed || state->boundary != boundary )
            return;

        state->armed = false;
        ++state->callbackCalls;
        const auto callback = state->callback;
        if ( callback )
            callback();
    }

    std::shared_ptr<FontCallbackTabArtState> m_state;
};

enum class ReplacingTabArtBoundary
{
    SetFlags,
    GetNormal
};

struct ReplacingTabArtState
{
    wxAuiNotebook* book = nullptr;
    wxAuiTabArt* primary = nullptr;
    ReplacingTabArtBoundary boundary = ReplacingTabArtBoundary::SetFlags;
    bool armed = false;
    bool destroyBook = false;
    bool readoptPrimary = false;
    bool primaryVirtualActive = false;
    bool primarySurvivedToVirtualReturn = false;
    bool replacementReadopted = false;
    unsigned int callbackCalls = 0;
    unsigned int primaryDestructions = 0;
    unsigned int primaryDestroyedInsideVirtual = 0;
    unsigned int replacementDestructions = 0;
    std::vector<const wxAuiTabArt*> primaryInstances;
    std::vector<const wxAuiTabArt*> replacementInstances;

    bool OwnsPrimary(const wxAuiTabArt* const art) const
    {
        return std::find(primaryInstances.begin(), primaryInstances.end(),
                         art) != primaryInstances.end();
    }

    bool OwnsReplacement(const wxAuiTabArt* const art) const
    {
        return std::find(replacementInstances.begin(),
                         replacementInstances.end(), art) !=
               replacementInstances.end();
    }
};

class ReplacingTabArtReplacement final : public wxAuiDefaultTabArt
{
public:
    explicit ReplacingTabArtReplacement(
        const std::shared_ptr<ReplacingTabArtState>& state)
        : m_state(state)
    {
        m_state->replacementInstances.push_back(this);
    }

    ~ReplacingTabArtReplacement() override
    {
        const auto it = std::find(m_state->replacementInstances.begin(),
                                  m_state->replacementInstances.end(), this);
        if ( it != m_state->replacementInstances.end() )
            m_state->replacementInstances.erase(it);
        ++m_state->replacementDestructions;
    }

    wxAuiTabArt* Clone() override
    {
        return new ReplacingTabArtReplacement(m_state);
    }

    void SetFlags(unsigned int flags) override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::SetFlags(flags);

        if ( !state->readoptPrimary || state->replacementReadopted ||
                !state->book || !state->primary )
        {
            return;
        }

        state->replacementReadopted = true;
        state->book->SetArtProvider(state->primary);
    }

private:
    std::shared_ptr<ReplacingTabArtState> m_state;
};

class ReplacingTabArt final : public wxAuiDefaultTabArt
{
public:
    ReplacingTabArt(const std::shared_ptr<ReplacingTabArtState>& state,
                    const bool primary = true)
        : m_state(state), m_primary(primary)
    {
        m_state->primaryInstances.push_back(this);
        if ( m_primary )
            m_state->primary = this;
    }

    ~ReplacingTabArt() override
    {
        const auto it = std::find(m_state->primaryInstances.begin(),
                                  m_state->primaryInstances.end(), this);
        if ( it != m_state->primaryInstances.end() )
            m_state->primaryInstances.erase(it);

        if ( m_primary )
        {
            ++m_state->primaryDestructions;
            if ( m_state->primaryVirtualActive )
                ++m_state->primaryDestroyedInsideVirtual;
        }
    }

    wxAuiTabArt* Clone() override
    {
        return new ReplacingTabArt(m_state, false);
    }

    void SetFlags(unsigned int flags) override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::SetFlags(flags);
        InvokeCallback(state, ReplacingTabArtBoundary::SetFlags, m_primary);
    }

    wxFont GetNormalFont() const override
    {
        const auto state = m_state;
        const wxFont result = wxAuiDefaultTabArt::GetNormalFont();
        InvokeCallback(state, ReplacingTabArtBoundary::GetNormal, m_primary);
        return result;
    }

private:
    static void InvokeCallback(
        const std::shared_ptr<ReplacingTabArtState>& state,
        const ReplacingTabArtBoundary boundary,
        const bool primary)
    {
        if ( !primary || !state->armed || state->boundary != boundary )
            return;

        state->armed = false;
        state->primaryVirtualActive = true;
        ++state->callbackCalls;

        wxAuiNotebook* const book = state->book;
        if ( state->destroyBook )
        {
            state->book = nullptr;
            delete book;
        }
        else
        {
            book->SetArtProvider(new ReplacingTabArtReplacement(state));
        }

        // This continuation deliberately uses only shared state. It proves
        // that the primary provider wasn't deleted while its virtual method
        // was still on the stack, without dereferencing the provider itself.
        state->primarySurvivedToVirtualReturn =
            state->primaryDestructions == 0;
        state->primaryVirtualActive = false;
    }

    std::shared_ptr<ReplacingTabArtState> m_state;
    bool m_primary;
};

enum class ThrowingTabArtBoundary
{
    Flags,
    NormalFont,
    SelectedFont,
    Sizing
};

struct ThrowingTabArtState
{
    ThrowingTabArtBoundary boundary = ThrowingTabArtBoundary::Flags;
    bool clonesOnly = false;
    bool armed = false;
    bool throwAfterCallback = true;
    std::function<void()> callback;
    wxAuiTabArt* callbackArt = nullptr;
    unsigned int callbackCalls = 0;
    unsigned int throws = 0;
    unsigned int cloneFlagsCalls = 0;
    unsigned int cloneNormalFontCalls = 0;
    unsigned int cloneSelectedFontCalls = 0;
    unsigned int cloneSizingCalls = 0;
    unsigned int primaryDestructions = 0;
    unsigned int cloneDestructions = 0;
    std::vector<const wxAuiTabArt*> primaryInstances;
    std::vector<const wxAuiTabArt*> cloneInstances;
};

class ThrowingTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit ThrowingTabArt(
        const std::shared_ptr<ThrowingTabArtState>& state,
        bool clone = false)
        : m_state(state), m_clone(clone)
    {
        Instances().push_back(this);
    }

    ~ThrowingTabArt() override
    {
        auto& instances = Instances();
        const auto it = std::find(instances.begin(), instances.end(), this);
        if ( it != instances.end() )
            instances.erase(it);
        if ( m_clone )
            ++m_state->cloneDestructions;
        else
            ++m_state->primaryDestructions;
    }

    wxAuiTabArt* Clone() override
    {
        return new ThrowingTabArt(m_state, true);
    }

    void SetFlags(unsigned int flags) override
    {
        wxAuiDefaultTabArt::SetFlags(flags);
        if ( m_clone )
            ++m_state->cloneFlagsCalls;
        ThrowAt(ThrowingTabArtBoundary::Flags);
    }

    void SetNormalFont(const wxFont& font) override
    {
        wxAuiDefaultTabArt::SetNormalFont(font);
        if ( m_clone )
            ++m_state->cloneNormalFontCalls;
        ThrowAt(ThrowingTabArtBoundary::NormalFont);
    }

    void SetSelectedFont(const wxFont& font) override
    {
        wxAuiDefaultTabArt::SetSelectedFont(font);
        if ( m_clone )
            ++m_state->cloneSelectedFontCalls;
        ThrowAt(ThrowingTabArtBoundary::SelectedFont);
    }

    void SetSizingInfo(const wxSize& size,
                       size_t pageCount,
                       wxWindow* wnd = nullptr) override
    {
        wxAuiDefaultTabArt::SetSizingInfo(size, pageCount, wnd);
        if ( m_clone )
            ++m_state->cloneSizingCalls;
        ThrowAt(ThrowingTabArtBoundary::Sizing);
    }

private:
    std::vector<const wxAuiTabArt*>& Instances()
    {
        return m_clone ? m_state->cloneInstances
                       : m_state->primaryInstances;
    }

    void ThrowAt(ThrowingTabArtBoundary boundary)
    {
        if ( !m_state->armed || boundary != m_state->boundary ||
                (m_state->clonesOnly && !m_clone) )
        {
            return;
        }

        m_state->armed = false;
        m_state->callbackArt = this;
        const auto callback = m_state->callback;
        if ( callback )
            callback();
        m_state->callbackArt = nullptr;
        ++m_state->callbackCalls;
        if ( m_state->throwAfterCallback )
        {
            ++m_state->throws;
            throw std::runtime_error(
                "intentional wxAuiTabArt setter failure");
        }
    }

    std::shared_ptr<ThrowingTabArtState> m_state;
    bool m_clone;
};

class RenderableTabContainer final : public wxAuiTabContainer
{
public:
    void RenderForTesting(wxDC& dc, wxWindow* wnd)
    {
        Render(&dc, wnd);
    }

    int LayoutForTesting(const wxRect& rect, wxWindow* wnd)
    {
        return LayoutMultiLineTabs(rect, wnd);
    }
};

enum class RenderDestroyBoundary
{
    Background,
    Button,
    PageSize,
    Page
};

struct RenderDestroyingTabArtState
{
    RenderDestroyBoundary boundary = RenderDestroyBoundary::Background;
    RenderableTabContainer* victim = nullptr;
    bool armed = false;
    bool virtualActive = false;
    bool survivedToVirtualReturn = false;
    unsigned int calls = 0;
    unsigned int destructions = 0;
    unsigned int destructionsInsideVirtual = 0;
};

class RenderDestroyingTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit RenderDestroyingTabArt(
        const std::shared_ptr<RenderDestroyingTabArtState>& state)
        : m_state(state)
    {
    }

    ~RenderDestroyingTabArt() override
    {
        ++m_state->destructions;
        if ( m_state->virtualActive )
            ++m_state->destructionsInsideVirtual;
    }

    wxAuiTabArt* Clone() override
    {
        return new RenderDestroyingTabArt(m_state);
    }

    void DrawBackground(wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override
    {
        wxAuiDefaultTabArt::DrawBackground(dc, wnd, rect);
        DestroyAt(RenderDestroyBoundary::Background);
    }

    void DrawButton(wxDC& dc,
                    wxWindow* wnd,
                    const wxRect& rect,
                    int bitmapId,
                    int buttonState,
                    int orientation,
                    wxRect* outRect) override
    {
        wxAuiDefaultTabArt::DrawButton(
            dc, wnd, rect, bitmapId, buttonState, orientation, outRect);
        DestroyAt(RenderDestroyBoundary::Button);
    }

    wxSize GetPageTabSize(wxReadOnlyDC& dc,
                          wxWindow* wnd,
                          const wxAuiNotebookPage& page,
                          int* xExtent = nullptr) override
    {
        const wxSize size =
            wxAuiDefaultTabArt::GetPageTabSize(dc, wnd, page, xExtent);
        DestroyAt(RenderDestroyBoundary::PageSize);
        return size;
    }

    int DrawPageTab(wxDC& dc,
                    wxWindow* wnd,
                    wxAuiNotebookPage& page,
                    const wxRect& rect) override
    {
        const int extent =
            wxAuiDefaultTabArt::DrawPageTab(dc, wnd, page, rect);
        DestroyAt(RenderDestroyBoundary::Page);
        return extent;
    }

private:
    void DestroyAt(RenderDestroyBoundary boundary)
    {
        const auto state = m_state;
        if ( !state->armed || state->boundary != boundary || !state->victim )
            return;

        state->armed = false;
        state->virtualActive = true;
        ++state->calls;
        RenderableTabContainer* const victim = state->victim;
        state->victim = nullptr;
        delete victim;
        state->survivedToVirtualReturn = state->destructions == 0;
        state->virtualActive = false;
    }

    std::shared_ptr<RenderDestroyingTabArtState> m_state;
};

struct SiblingDestroyingTabArtState
{
    RenderDestroyBoundary boundary = RenderDestroyBoundary::Background;
    wxWindow* trigger = nullptr;
    wxWindow* victim = nullptr;
    bool armed = false;
    unsigned int calls = 0;
    unsigned int boundaryCalls = 0;
};

class SiblingDestroyingTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit SiblingDestroyingTabArt(
        const std::shared_ptr<SiblingDestroyingTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        return new SiblingDestroyingTabArt(m_state);
    }

    void DrawBackground(wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override
    {
        wxAuiDefaultTabArt::DrawBackground(dc, wnd, rect);
        DestroyAt(RenderDestroyBoundary::Background, nullptr);
    }

    void DrawButton(wxDC& dc,
                    wxWindow* wnd,
                    const wxRect& rect,
                    int bitmapId,
                    int buttonState,
                    int orientation,
                    wxRect* outRect) override
    {
        wxAuiDefaultTabArt::DrawButton(
            dc, wnd, rect, bitmapId, buttonState, orientation, outRect);
        DestroyAt(RenderDestroyBoundary::Button, nullptr);
    }

    wxSize GetPageTabSize(wxReadOnlyDC& dc,
                          wxWindow* wnd,
                          const wxAuiNotebookPage& page,
                          int* xExtent = nullptr) override
    {
        const wxSize size =
            wxAuiDefaultTabArt::GetPageTabSize(dc, wnd, page, xExtent);
        DestroyAt(RenderDestroyBoundary::PageSize, page.window);
        return size;
    }

    int DrawPageTab(wxDC& dc,
                    wxWindow* wnd,
                    wxAuiNotebookPage& page,
                    const wxRect& rect) override
    {
        const int extent =
            wxAuiDefaultTabArt::DrawPageTab(dc, wnd, page, rect);
        DestroyAt(RenderDestroyBoundary::Page, page.window);
        return extent;
    }

private:
    void DestroyAt(RenderDestroyBoundary boundary, wxWindow* page)
    {
        const auto state = m_state;
        if ( state->boundary == boundary )
            ++state->boundaryCalls;
        if ( !state->armed || state->boundary != boundary ||
                (page && page != state->trigger) || !state->victim )
        {
            return;
        }

        state->armed = false;
        wxWindow* const victim = state->victim;
        state->victim = nullptr;
        ++state->calls;
        delete victim;
    }

    std::shared_ptr<SiblingDestroyingTabArtState> m_state;
};

struct ColourCallbackTabArtState
{
    std::function<void()> callback;
    bool armed = false;
    unsigned int calls = 0;
};

class ColourCallbackTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit ColourCallbackTabArt(
        const std::shared_ptr<ColourCallbackTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        return new ColourCallbackTabArt(m_state);
    }

    void UpdateColoursFromSystem() override
    {
        const auto state = m_state;
        wxAuiDefaultTabArt::UpdateColoursFromSystem();
        if ( !state->armed )
            return;

        state->armed = false;
        ++state->calls;
        const auto callback = state->callback;
        if ( callback )
            callback();
    }

private:
    std::shared_ptr<ColourCallbackTabArtState> m_state;
};

struct DropDownCallbackTabArtState
{
    std::function<void(const wxAuiNotebookPageArray&)> callback;
    int result = wxNOT_FOUND;
    bool throwAfterCallback = false;
    unsigned int calls = 0;
};

class DropDownCallbackTabArt final : public wxAuiDefaultTabArt
{
public:
    explicit DropDownCallbackTabArt(
        const std::shared_ptr<DropDownCallbackTabArtState>& state)
        : m_state(state)
    {
    }

    wxAuiTabArt* Clone() override
    {
        return new DropDownCallbackTabArt(m_state);
    }

    int ShowDropDown(wxWindow*,
                     const wxAuiNotebookPageArray& pages,
                     int) override
    {
        const auto state = m_state;
        ++state->calls;
        const auto callback = state->callback;
        if ( callback )
            callback(pages);
        if ( state->throwAfterCallback )
            throw std::runtime_error("intentional ShowDropDown failure");
        return state->result;
    }

private:
    std::shared_ptr<DropDownCallbackTabArtState> m_state;
};

} // anonymous namespace

// This peer is named in wxAuiNotebook's private test seam. It deliberately
// lives in the global namespace so the friendship doesn't expose a protected
// extension point to arbitrary subclasses.
class wxAuiNotebookTestPeer final : public wxAuiNotebook
{
public:
    using wxAuiNotebook::wxAuiNotebook;

    void EndDrag(wxAuiTabCtrl* const source,
                 const int sourceIndex,
                 wxAuiTabCtrl* const destination,
                 const int destinationIndex,
                 const bool createNewPane = false)
    {
        OnTabEndDragForTesting(source, sourceIndex, destination,
                               destinationIndex, createNewPane);
    }

    wxBitmapBundle GetCanonicalBitmap(size_t page) const
    {
        return m_tabs.GetPage(page).bitmap;
    }

    wxBitmapBundle GetVisualBitmap(size_t page) const
    {
        const wxAuiNotebookPosition position = GetPagePosition(page);
        return position
            ? position.tabCtrl->GetPage(
                  static_cast<size_t>(position.tabIdx)).bitmap
            : wxBitmapBundle();
    }

    const wxFont& GetNormalFontForTesting() const { return m_normalFont; }
    const wxFont& GetSelectedFontForTesting() const { return m_selectedFont; }

    int CalculateCurrentTabHeightForTesting()
    {
        return CalculateTabCtrlHeight();
    }

    void ClickWindowListForTesting(wxAuiTabCtrl* const tabs)
    {
        tabs->OnButton(wxNOT_FOUND, wxAUI_BUTTON_WINDOWLIST);
    }
};

// ----------------------------------------------------------------------------
// test fixtures
// ----------------------------------------------------------------------------

class AuiNotebookTestCase
{
public:
    AuiNotebookTestCase()
        : nb(new wxAuiNotebook(wxTheApp->GetTopWindow()))
    {
    }


protected:
    const std::unique_ptr<wxAuiNotebook> nb;
};

class AuiManagerTestCase
{
public:
    AuiManagerTestCase()
        : frame(new wxFrame(nullptr, wxID_ANY, "wxAuiManager test"))
        , manager(frame.get())
    {
        frame->SetClientSize(800, 600);
    }

    ~AuiManagerTestCase()
    {
        manager.UnInit();
    }

protected:
    std::unique_ptr<wxFrame> frame;
    wxAuiManager manager;
};

class TestAuiNotebook : public wxAuiNotebook
{
public:
    TestAuiNotebook()
        : wxAuiNotebook(wxTheApp->GetTopWindow())
    {
    }

    using wxAuiNotebook::OnTabMiddleDown;
    using wxAuiNotebook::OnTabMiddleUp;
    using wxAuiNotebook::OnTabRightDown;
    using wxAuiNotebook::OnTabRightUp;
};

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(AuiManagerTestCase, "wxAuiManager::AddPaneBestSize", "[aui]")
{
    wxWindow* const pane = new wxPanel(frame.get());
    wxWindow* const center = new wxPanel(frame.get());

    wxAuiPaneInfo paneInfo;
    paneInfo.BestSize(320, 200).Left().CaptionVisible(false).PaneBorder(false);

    REQUIRE( manager.AddPane(pane, paneInfo) );
    REQUIRE( manager.AddPane(center, wxAuiPaneInfo().CenterPane()) );

    manager.Update();

    CHECK( pane->GetSize().x == 320 );
}

TEST_CASE_METHOD(AuiManagerTestCase, "wxAuiManager::AddPaneDockSize", "[aui]")
{
    wxWindow* const pane = new wxPanel(frame.get());
    wxWindow* const center = new wxPanel(frame.get());

    wxAuiPaneInfo paneInfo;
    paneInfo.BestSize(320, 200).Left().CaptionVisible(false).PaneBorder(false);
    paneInfo.dock_size = 180;

    REQUIRE( manager.AddPane(pane, paneInfo) );
    REQUIRE( manager.AddPane(center, wxAuiPaneInfo().CenterPane()) );

    manager.Update();

    CHECK( pane->GetSize().x == 180 );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::DoGetBestSize", "[aui]")
{
    wxPanel *p = new wxPanel(nb.get());
    p->SetMinSize(wxSize(100, 100));
    REQUIRE( nb->AddPage(p, "Center Pane") );

    const int tabHeight = nb->GetTabCtrlHeight();

    SECTION( "Single pane with multiple tabs" )
    {
        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(300, 100));
        nb->AddPage(p, "Center Tab 2");

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(100, 200));
        nb->AddPage(p, "Center Tab 3");

        CHECK( nb->GetBestSize() == wxSize(300, 200 + tabHeight) );
    }

    SECTION( "Horizontal split" )
    {
        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(25, 0));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(125, 100 + tabHeight) );

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(50, 0));
        nb->AddPage(p, "Right Pane 1");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(175, 100 + tabHeight) );

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(100, 0));
        nb->AddPage(p, "Right Pane 2");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(275, 100 + tabHeight) );
    }

    SECTION( "Vertical split" )
    {
        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(0, 100));
        nb->AddPage(p, "Top Pane 1");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(0, 50));
        nb->AddPage(p, "Top Pane 2");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        CHECK( nb->GetBestSize() == wxSize(100, 250 + 3*tabHeight) );

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(0, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        CHECK( nb->GetBestSize() == wxSize(100, 275 + 4*tabHeight) );
    }

    SECTION( "Surrounding panes" )
    {
        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(50, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(50, 120));
        nb->AddPage(p, "Right Pane");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(225, 50));
        nb->AddPage(p, "Top Pane");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb.get());
        p->SetMinSize(wxSize(25, 105));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(250, 175 + 3*tabHeight) );
    }
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::RTTI", "[aui][rtti]")
{
    wxBookCtrlBase* const book = nb.get();
    CHECK( wxDynamicCast(book, wxAuiNotebook) == nb.get() );

    CHECK( wxDynamicCast(nb.get(), wxBookCtrlBase) == book );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::FindPage", "[aui]")
{
    wxPanel *p1 = new wxPanel(nb.get());
    wxPanel *p2 = new wxPanel(nb.get());
    wxPanel *p3 = new wxPanel(nb.get());
    REQUIRE( nb->AddPage(p1, "Page 1") );
    REQUIRE( nb->AddPage(p2, "Page 2") );

    CHECK( nb->FindPage(nullptr) == wxNOT_FOUND );
    CHECK( nb->FindPage(p1) == 0 );
    CHECK( nb->FindPage(p2) == 1 );
    CHECK( nb->FindPage(p3) == wxNOT_FOUND );
}

TEST_CASE_METHOD(AuiNotebookTestCase,
                 "wxAuiNotebook uses the polymorphic book model",
                 "[aui][bookctrl][lifetime][transaction]")
{
    wxBookCtrlBase* const book = nb.get();

    wxPanel* const first = new wxPanel(nb.get());
    wxPanel* const second = new wxPanel(nb.get());
    wxPanel* const third = new wxPanel(nb.get());
    REQUIRE(book->wxBookCtrlBase::AddPage(first, "first", true));
    REQUIRE(book->wxBookCtrlBase::AddPage(second, "second"));
    REQUIRE(book->wxBookCtrlBase::AddPage(third, "third"));
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == first);
    CHECK(book->GetPage(1) == second);
    CHECK(book->GetPage(2) == third);

    REQUIRE(book->wxBookCtrlBase::RemovePage(1));
    CHECK(book->GetPageCount() == 2);
    CHECK(book->FindPage(second) == wxNOT_FOUND);
    CHECK_FALSE(second->IsShown());
    delete second;

    const wxWeakRef<wxWindow> firstLifetime(first);
    REQUIRE(book->wxBookCtrlBase::DeletePage(0));
    CHECK_FALSE(firstLifetime);
    REQUIRE(book->GetPageCount() == 1);
    CHECK(book->GetPage(0) == third);

    const wxWeakRef<wxWindow> thirdLifetime(third);
    REQUIRE(book->wxBookCtrlBase::DeleteAllPages());
    CHECK(book->GetPageCount() == 0);
    CHECK_FALSE(thirdLifetime);
}

TEST_CASE("wxAuiNotebook destructor bounds external-model cleanup",
          "[aui][bookctrl][lifetime]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    delete book;
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook teardown tolerates nested page destruction",
          "[aui][bookctrl][lifetime][reentrancy]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());

    std::vector<wxPanel*> pages;
    std::vector<wxWeakRef<wxWindow>> lifetimes;
    for ( int i = 0; i < 3; ++i )
    {
        wxPanel* const page = new wxPanel(book);
        pages.push_back(page);
        lifetimes.emplace_back(page);
        REQUIRE(book->AddPage(page, wxString::Format("page %d", i), i == 0));
    }

    std::vector<bool> destroying(pages.size());
    unsigned int destroyCallbacks = 0;
    unsigned int nestedDeletes = 0;
    for ( size_t i = 0; i < pages.size(); ++i )
    {
        pages[i]->Bind(
            wxEVT_DESTROY,
            [&, i](wxWindowDestroyEvent& event)
            {
                event.Skip();
                destroying[i] = true;
                ++destroyCallbacks;

                // wxAuiNotebook clears both its canonical and display models
                // before child destructors can call back into it.
                CHECK(book->GetPageCount() == 0);
                CHECK(book->GetSelection() == wxNOT_FOUND);

                // Cascade through the other pages while DestroyChildren() is
                // already active. This also removes them from the parent's
                // child list underneath the outer teardown loop.
                for ( size_t j = 0; j < pages.size(); ++j )
                {
                    if ( !destroying[j] && lifetimes[j].get() == pages[j] )
                    {
                        ++nestedDeletes;
                        delete pages[j];
                        break;
                    }
                }
            });
    }

    delete book;

    CHECK(destroyCallbacks == pages.size());
    CHECK(nestedDeletes == pages.size() - 1);
    for ( const auto& lifetime : lifetimes )
        CHECK_FALSE(lifetime);
}

TEST_CASE("wxAuiNotebook split repairs a sibling page in another pane",
          "[aui][lifetime][split][reentrancy]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const victim = new wxPanel(book);
    wxPanel* const splitCandidate = new wxPanel(book);
    wxPanel* const survivor = new wxPanel(book);
    const wxWeakRef<wxWindow> bookLifetime(book);
    const wxWeakRef<wxWindow> victimLifetime(victim);
    const wxWeakRef<wxWindow> splitCandidateLifetime(splitCandidate);
    const wxWeakRef<wxWindow> survivorLifetime(survivor);
    REQUIRE(book->AddPage(victim, "victim"));
    REQUIRE(book->AddPage(splitCandidate, "split", true));
    REQUIRE(book->AddPage(survivor, "survivor"));

    // Put the victim in a different pre-existing pane. The Split() under test
    // must snapshot all manager projections, not just its own source tabs.
    book->Split(0, wxLEFT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);
    book->SetSelection(1);
    REQUIRE(book->GetSelection() == 1);

    const wxAuiNotebookPosition originalPosition =
        book->GetPagePosition(1);
    REQUIRE(originalPosition);
    const wxAuiNotebookPosition victimPosition =
        book->GetPagePosition(0);
    REQUIRE(victimPosition);
    REQUIRE(victimPosition.tabCtrl != originalPosition.tabCtrl);

    const auto state = std::make_shared<WindowDestroyingTabArtState>();
    book->SetArtProvider(new WindowDestroyingTabArt(state));
    state->victim = victim;
    state->armed = true;

    book->Split(1, wxRIGHT);

    CHECK(state->calls == 1);
    CHECK(bookLifetime);
    CHECK_FALSE(victimLifetime);
    REQUIRE(splitCandidateLifetime);
    REQUIRE(survivorLifetime);
    REQUIRE(book->GetPageCount() == 2);
    CHECK(book->GetPage(0) == splitCandidate);
    CHECK(book->GetPage(1) == survivor);
    CHECK(book->GetSelection() == 0);

    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 1);
    CHECK(tabs[0] == originalPosition.tabCtrl);
    CHECK(book->GetPagesInDisplayOrder(tabs[0]) ==
          std::vector<size_t>{0, 1});

    wxPanel* const appended = new wxPanel(book);
    REQUIRE(book->AddPage(appended, "appended"));
    CHECK(book->GetPageCount() == 3);

    delete book;
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(splitCandidateLifetime);
    CHECK_FALSE(survivorLifetime);
}

TEST_CASE("wxAuiNotebook end drag moves a tab to an existing pane",
          "[aui][drag][split][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    wxPanel* const third = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(book->AddPage(third, "third"));
    book->Split(2, wxRIGHT);

    const wxAuiNotebookPosition source = book->GetPagePosition(0);
    const wxAuiNotebookPosition destination = book->GetPagePosition(2);
    REQUIRE(source);
    REQUIRE(destination);
    REQUIRE(source.tabCtrl != destination.tabCtrl);

    book->EndDrag(source.tabCtrl, source.tabIdx,
                  destination.tabCtrl, 0);

    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == first);
    CHECK(book->GetPage(1) == second);
    CHECK(book->GetPage(2) == third);
    CHECK(book->GetPagesInDisplayOrder(source.tabCtrl) ==
          std::vector<size_t>{1});
    CHECK(book->GetPagesInDisplayOrder(destination.tabCtrl) ==
          std::vector<size_t>{0, 2});
    CHECK(book->GetSelection() == 0);

    delete book;
}

TEST_CASE("wxAuiNotebook end drag creates and commits a new pane",
          "[aui][drag][split][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    const wxAuiNotebookPosition source = book->GetPagePosition(1);
    REQUIRE(source);
    book->EndDrag(source.tabCtrl, source.tabIdx,
                  nullptr, 0, true);

    REQUIRE(book->GetPageCount() == 2);
    const wxAuiNotebookPosition firstPosition = book->GetPagePosition(0);
    const wxAuiNotebookPosition secondPosition = book->GetPagePosition(1);
    REQUIRE(firstPosition);
    REQUIRE(secondPosition);
    REQUIRE(firstPosition.tabCtrl != secondPosition.tabCtrl);
    CHECK(book->GetAllTabCtrls().size() == 2);
    CHECK(book->GetPagesInDisplayOrder(firstPosition.tabCtrl) ==
          std::vector<size_t>{0});
    CHECK(book->GetPagesInDisplayOrder(secondPosition.tabCtrl) ==
          std::vector<size_t>{1});
    CHECK(book->GetSelection() == 1);

    delete book;
}

TEST_CASE("wxAuiNotebook external drag locally rolls back a damaged source",
          "[aui][drag][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const sourceBook =
        new wxAuiNotebookTestPeer(
            wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition,
            wxDefaultSize,
            wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE);
    wxPanel* const dragged = new wxPanel(sourceBook);
    wxPanel* const survivor = new wxPanel(sourceBook);
    wxPanel* const sibling = new wxPanel(sourceBook);
    REQUIRE(sourceBook->AddPage(dragged, "dragged", true));
    REQUIRE(sourceBook->AddPage(survivor, "survivor"));
    REQUIRE(sourceBook->AddPage(sibling, "sibling"));
    sourceBook->Split(2, wxRIGHT);

    const wxAuiNotebookPosition source = sourceBook->GetPagePosition(0);
    const wxAuiNotebookPosition siblingPosition =
        sourceBook->GetPagePosition(2);
    REQUIRE(source);
    REQUIRE(siblingPosition);
    REQUIRE(source.tabCtrl != siblingPosition.tabCtrl);
    const wxWeakRef<wxAuiTabCtrl> siblingTabsLifetime(
        siblingPosition.tabCtrl);

    const auto state = std::make_shared<WindowDestroyingTabArtState>();
    sourceBook->SetArtProvider(new WindowDestroyingTabArt(state));
    state->victim = siblingPosition.tabCtrl;
    state->armed = true;

    wxAuiNotebook* const destinationBook =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const resident = new wxPanel(destinationBook);
    REQUIRE(destinationBook->AddPage(resident, "resident", true));
    wxAuiTabCtrl* const destinationTabs =
        destinationBook->GetMainTabCtrl();
    REQUIRE(destinationTabs);
    destinationBook->Bind(
        wxEVT_AUINOTEBOOK_ALLOW_DND,
        [](wxAuiNotebookEvent& event)
        {
            event.Allow();
        });

    sourceBook->EndDrag(source.tabCtrl, source.tabIdx,
                        destinationTabs, -1);

    CHECK(state->calls == 1);
    CHECK_FALSE(siblingTabsLifetime);
    REQUIRE(sourceBook->GetPageCount() == 3);
    CHECK(sourceBook->GetPage(0) == dragged);
    CHECK(sourceBook->GetPage(1) == survivor);
    CHECK(sourceBook->GetPage(2) == sibling);
    CHECK(dragged->GetParent() == sourceBook);
    CHECK(dragged->IsShown());
    CHECK(sourceBook->GetPagesInDisplayOrder(source.tabCtrl) ==
          std::vector<size_t>{0, 1});
    REQUIRE(destinationBook->GetPageCount() == 1);
    CHECK(destinationBook->GetPage(0) == resident);

    delete destinationBook;
    delete sourceBook;
}

TEST_CASE("wxAuiNotebook external drag rejects a reparented destination",
          "[aui][drag][ownership][reentrancy][transaction]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxAuiNotebookTestPeer* const sourceBook =
        new wxAuiNotebookTestPeer(
            top, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE);
    wxPanel* const dragged = new wxPanel(sourceBook);
    wxPanel* const sourceResident = new wxPanel(sourceBook);
    REQUIRE(sourceBook->AddPage(dragged, "dragged", true));
    REQUIRE(sourceBook->AddPage(sourceResident, "source resident"));
    const wxAuiNotebookPosition source =
        sourceBook->GetPagePosition(0);
    REQUIRE(source);

    wxPanel* const newOwner = new wxPanel(top);
    wxAuiNotebook* const destinationBook = new wxAuiNotebook(top);
    wxPanel* const destinationResident = new wxPanel(destinationBook);
    const wxWeakRef<wxWindow> destinationResidentLifetime(
        destinationResident);
    REQUIRE(destinationBook->AddPage(
        destinationResident, "destination resident", true));
    wxAuiTabCtrl* const destinationTabs =
        destinationBook->GetMainTabCtrl();
    REQUIRE(destinationTabs);
    const wxWeakRef<wxAuiTabCtrl> destinationTabsLifetime(
        destinationTabs);
    bool reparented = false;
    destinationBook->Bind(
        wxEVT_AUINOTEBOOK_ALLOW_DND,
        [destinationTabs, newOwner, &reparented](wxAuiNotebookEvent& event)
        {
            reparented = destinationTabs->Reparent(newOwner);
            event.Allow();
        });

    // ALLOW_DND runs after the drag path has hidden the hint and reset the
    // cursor. The captured destination parent is no longer current, so the
    // transfer must stop without consuming either projection.
    sourceBook->EndDrag(source.tabCtrl, source.tabIdx,
                        destinationTabs, -1);

    CHECK(reparented);
    REQUIRE(destinationTabsLifetime.get() == destinationTabs);
    CHECK(destinationTabs->GetParent() == newOwner);
    REQUIRE(sourceBook->GetPageCount() == 2);
    CHECK(sourceBook->GetPage(0) == dragged);
    CHECK(sourceBook->GetPage(1) == sourceResident);
    CHECK(dragged->GetParent() == sourceBook);
    REQUIRE(destinationBook->GetPageCount() == 1);
    CHECK(destinationBook->GetPage(0) == destinationResident);

    delete destinationBook;
    CHECK_FALSE(destinationResidentLifetime);
    REQUIRE(destinationTabsLifetime.get() == destinationTabs);
    delete sourceBook;
    delete newOwner;
    CHECK_FALSE(destinationTabsLifetime);
}

TEST_CASE("wxAuiNotebook external drag rejects a destroy-scheduled destination",
          "[aui][drag][lifetime][transaction]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxAuiNotebookTestPeer* const sourceBook =
        new wxAuiNotebookTestPeer(
            top, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE);
    wxPanel* const dragged = new wxPanel(sourceBook);
    wxPanel* const sourceResident = new wxPanel(sourceBook);
    REQUIRE(sourceBook->AddPage(dragged, "dragged", true));
    REQUIRE(sourceBook->AddPage(sourceResident, "source resident"));
    const wxAuiNotebookPosition source =
        sourceBook->GetPagePosition(0);
    REQUIRE(source);

    wxAuiNotebook* const destinationBook = new wxAuiNotebook(top);
    wxPanel* const destinationResident = new wxPanel(destinationBook);
    REQUIRE(destinationBook->AddPage(
        destinationResident, "destination resident", true));
    wxAuiTabCtrl* const destinationTabs =
        destinationBook->GetMainTabCtrl();
    REQUIRE(destinationTabs);
    const wxWeakRef<wxAuiTabCtrl> destinationTabsLifetime(
        destinationTabs);

    // Keep the C++ object deliberately weak-live while making it unavailable
    // to callback transactions, exactly as a delayed Destroy() does.
    wxTheApp->ScheduleForDestruction(destinationTabs);
    REQUIRE(destinationTabsLifetime.get() == destinationTabs);
    REQUIRE(wxTheApp->IsScheduledForDestruction(destinationTabs));

    sourceBook->EndDrag(source.tabCtrl, source.tabIdx,
                        destinationTabs, -1);

    REQUIRE(sourceBook->GetPageCount() == 2);
    CHECK(sourceBook->GetPage(0) == dragged);
    CHECK(sourceBook->GetPage(1) == sourceResident);
    CHECK(dragged->GetParent() == sourceBook);
    REQUIRE(destinationBook->GetPageCount() == 1);
    CHECK(destinationBook->GetPage(0) == destinationResident);

    delete destinationBook;
    CHECK_FALSE(destinationTabsLifetime);
    delete sourceBook;
}

TEST_CASE("wxAuiNotebook unsplits three panes with exact display order",
          "[aui][split][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    for ( int i = 0; i < 4; ++i )
    {
        REQUIRE(book->AddPage(new wxPanel(book),
                              wxString::Format("page-%d", i), i == 0));
        REQUIRE(book->SetPageToolTip(
            static_cast<size_t>(i), wxString::Format("tip-%d", i)));
    }

    book->Split(1, wxLEFT);
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 3);

    wxAuiTabCtrl* const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    std::vector<size_t> expected =
        book->GetPagesInDisplayOrder(mainTabs);
    for ( wxAuiTabCtrl* const tabs : book->GetAllTabCtrls() )
    {
        if ( tabs == mainTabs )
            continue;
        const std::vector<size_t> pages =
            book->GetPagesInDisplayOrder(tabs);
        expected.insert(expected.end(), pages.begin(), pages.end());
    }
    REQUIRE(expected.size() == book->GetPageCount());

    const size_t selectedMainPage = expected.front();
    book->SetSelection(selectedMainPage);
    REQUIRE(book->GetSelection() == static_cast<int>(selectedMainPage));

    book->UnsplitAll();

    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 1);
    CHECK(tabs[0] == mainTabs);
    CHECK(book->GetPagesInDisplayOrder(mainTabs) == expected);
    CHECK(book->GetSelection() == static_cast<int>(selectedMainPage));
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        CHECK(book->GetPageToolTip(i) == wxString::Format("tip-%zu", i));
    }

    delete book;
}

TEST_CASE("wxAuiNotebook unsplit stops when art destroys another pane tabs",
          "[aui][lifetime][split][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    wxPanel* const third = new wxPanel(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    const wxWeakRef<wxWindow> thirdLifetime(third);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(book->AddPage(third, "third"));
    book->Split(1, wxLEFT);
    book->Split(2, wxRIGHT);

    wxAuiTabCtrl* const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    std::vector<wxAuiTabCtrl*> nonMain;
    for ( wxAuiTabCtrl* const tabs : book->GetAllTabCtrls() )
    {
        if ( tabs != mainTabs )
            nonMain.push_back(tabs);
    }
    REQUIRE(nonMain.size() == 2);
    wxAuiTabCtrl* const victimTabs = nonMain.back();
    const wxWeakRef<wxAuiTabCtrl> victimLifetime(victimTabs);

    const auto state = std::make_shared<WindowDestroyingTabArtState>();
    book->SetArtProvider(new WindowDestroyingTabArt(state));
    state->victim = victimTabs;
    state->armed = true;

    book->UnsplitAll();

    CHECK(state->calls == 1);
    CHECK_FALSE(victimLifetime);

    delete book;
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
    CHECK_FALSE(thirdLifetime);
}

TEST_CASE("wxAuiNotebook split stops when Clone destroys another pane tabs",
          "[aui][lifetime][split][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const splitCandidate = new wxPanel(book);
    wxPanel* const third = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(splitCandidate, "candidate"));
    REQUIRE(book->AddPage(third, "third"));
    book->Split(0, wxLEFT);

    wxAuiTabCtrl* const victimTabs = book->GetPagePosition(0).tabCtrl;
    wxAuiTabCtrl* const sourceTabs = book->GetPagePosition(1).tabCtrl;
    REQUIRE(victimTabs);
    REQUIRE(sourceTabs);
    REQUIRE(victimTabs != sourceTabs);
    const wxWeakRef<wxAuiTabCtrl> victimLifetime(victimTabs);

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));
    state->boundary = TabArtCallbackBoundary::Clone;
    state->callback = [victimTabs]
    {
        delete victimTabs;
    };
    state->armed = true;

    book->Split(1, wxRIGHT);

    CHECK(state->callbackCalls == 1);
    CHECK_FALSE(victimLifetime);
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == first);
    CHECK(book->GetPage(1) == splitCandidate);
    CHECK(book->GetPage(2) == third);

    delete book;
}

TEST_CASE("wxAuiNotebook split-pane insertion stops on sibling tabs loss",
          "[aui][lifetime][split][reentrancy][transaction]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxAuiNotebook* const book = new wxAuiNotebook(top);
    wxPanel* const victimPage = new wxPanel(book);
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    REQUIRE(book->AddPage(victimPage, "victim"));
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));
    book->Split(0, wxLEFT);
    book->SetSelection(1);

    wxAuiTabCtrl* const victimTabs = book->GetPagePosition(0).tabCtrl;
    wxAuiTabCtrl* const insertionTabs = book->GetPagePosition(1).tabCtrl;
    REQUIRE(victimTabs);
    REQUIRE(insertionTabs);
    REQUIRE(victimTabs != insertionTabs);
    const wxWeakRef<wxAuiTabCtrl> victimLifetime(victimTabs);

    const auto state = std::make_shared<WindowDestroyingTabArtState>();
    book->SetArtProvider(new WindowDestroyingTabArt(state));
    state->victim = victimTabs;
    state->armed = true;

    wxPanel* const candidate = new wxPanel(book);
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    candidate->Hide();
    REQUIRE_FALSE(candidate->IsShown());
    CHECK_FALSE(book->InsertPage(1, candidate, "candidate", true));

    CHECK(state->calls == 1);
    CHECK_FALSE(victimLifetime);
    REQUIRE(candidateLifetime);
    CHECK(candidate->GetParent() == book);
    CHECK_FALSE(candidate->IsShown());
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == victimPage);
    CHECK(book->GetPage(1) == first);
    CHECK(book->GetPage(2) == second);
    CHECK(book->GetSelection() == 1);
    CHECK(first->IsShown());
    CHECK_FALSE(second->IsShown());

    delete book;
    CHECK_FALSE(candidateLifetime);
}

TEST_CASE("wxAuiNotebook insertion locally restores physical visibility",
          "[aui][lifetime][selection][reentrancy][transaction]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxAuiNotebook* const book = new wxAuiNotebook(top);
    wxPanel* const victim = new wxPanel(book);
    wxPanel* const selected = new wxPanel(book);
    wxPanel* const inactive = new wxPanel(book);
    REQUIRE(book->AddPage(victim, "victim"));
    REQUIRE(book->AddPage(selected, "selected", true));
    REQUIRE(book->AddPage(inactive, "inactive"));
    book->Split(0, wxLEFT);
    book->SetSelection(1);
    REQUIRE(selected->IsShown());
    REQUIRE_FALSE(inactive->IsShown());

    wxAuiTabCtrl* const victimTabs =
        book->GetPagePosition(0).tabCtrl;
    REQUIRE(victimTabs);
    const wxWeakRef<wxAuiTabCtrl> victimTabsLifetime(victimTabs);

    wxPanel* const candidate = new wxPanel(book);
    candidate->Hide();
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    unsigned int callbacks = 0;
    book->Bind(
        wxEVT_AUINOTEBOOK_PAGE_CHANGED,
        [&, victimTabs](wxAuiNotebookEvent& event)
        {
            event.Skip();
            const int selection = event.GetSelection();
            if ( callbacks != 0 || selection < 0 ||
                    static_cast<size_t>(selection) >=
                        book->GetPageCount() ||
                    book->GetPage(static_cast<size_t>(selection)) !=
                        candidate )
            {
                return;
            }

            ++callbacks;
            // At this late boundary the candidate has already become the
            // physically visible selected page. Damage only an unrelated
            // pane: local rollback must still restore the target pane.
            CHECK(candidate->IsShown());
            if ( book->IsShownOnScreen() )
                CHECK(candidate->IsShownOnScreen());
            delete victimTabs;
        });

    CHECK_FALSE(book->InsertPage(1, candidate, "candidate", true));

    CHECK(callbacks == 1);
    CHECK_FALSE(victimTabsLifetime);
    REQUIRE(candidateLifetime);
    CHECK(candidate->GetParent() == book);
    CHECK_FALSE(candidate->IsShown());
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == victim);
    CHECK(book->GetPage(1) == selected);
    CHECK(book->GetPage(2) == inactive);
    CHECK(book->GetSelection() == 1);
    CHECK(selected->IsShown());
    CHECK_FALSE(inactive->IsShown());
    if ( book->IsShownOnScreen() )
    {
        CHECK(selected->IsShownOnScreen());
        CHECK_FALSE(inactive->IsShownOnScreen());
        CHECK_FALSE(candidate->IsShownOnScreen());
    }
    const wxAuiNotebookPosition selectedPosition =
        book->GetPagePosition(1);
    REQUIRE(selectedPosition);
    CHECK(selectedPosition.tabCtrl->GetActivePage() ==
          selectedPosition.tabIdx);

    delete book;
    CHECK_FALSE(candidateLifetime);
}

TEST_CASE("wxAuiNotebook teardown tolerates a callback-destroyed tab control",
          "[aui][lifetime][layout][reentrancy]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxWindow> bookLifetime(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    wxAuiTabCtrl* const victimTabs = book->GetPagePosition(0).tabCtrl;
    REQUIRE(victimTabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(victimTabs);

    const auto state = std::make_shared<WindowDestroyingTabArtState>();
    book->SetArtProvider(new WindowDestroyingTabArt(state));
    state->victim = victimTabs;
    state->armed = true;

    book->SetTabCtrlHeight(book->GetTabCtrlHeight() + 1);

    CHECK(state->calls == 1);
    CHECK_FALSE(tabsLifetime);
    REQUIRE(bookLifetime);

    delete book;
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook teardown preserves a reparented tab control",
          "[aui][lifetime][reentrancy][ownership]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxPanel* const newOwner = new wxPanel(top);
    wxAuiNotebook* const book = new wxAuiNotebook(top);
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    wxAuiTabCtrl* const tabs = book->GetMainTabCtrl();
    REQUIRE(tabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(tabs);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);

    // WinUI deliberately rejects Reparent() once an ancestor is being
    // deleted, so establish the external ownership while both hierarchies are
    // live. The notebook teardown must neither delete this control nor leave
    // it retaining page windows still owned by the notebook.
    REQUIRE(tabs->Reparent(newOwner));
    REQUIRE(tabs->GetParent() == newOwner);

    delete book;

    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
    REQUIRE(tabsLifetime.get() == tabs);
    CHECK(tabs->GetParent() == newOwner);
    CHECK(tabs->GetPageCount() == 0);

    delete tabs;
    delete newOwner;
    CHECK_FALSE(tabsLifetime);
}

TEST_CASE("wxAuiNotebook insertion stops after hostile best-size callback",
          "[aui][lifetime][layout][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    wxAuiTabCtrl* const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(mainTabs);

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));
    const unsigned int bestSizeCallsBefore = state->bestSizeCalls;

    wxPanel* const candidate = new wxPanel(book);
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    state->boundary = TabArtCallbackBoundary::BestSize;
    state->callback = [candidate]
    {
        delete candidate;
    };
    state->armed = true;

    // InsertPage() reports success because the callback consumed the
    // candidate, but it must roll the partially published page out of both
    // the canonical and visual projections before returning.
    CHECK(book->InsertPage(1, candidate, "candidate"));

    CHECK(state->callbackCalls == 1);
    CHECK(state->bestSizeCalls > bestSizeCallsBefore);
    CHECK_FALSE(candidateLifetime);
    REQUIRE(bookLifetime);
    REQUIRE(firstLifetime);
    REQUIRE(secondLifetime);
    REQUIRE(book->GetPageCount() == 2);
    CHECK(book->GetPage(0) == first);
    CHECK(book->GetPage(1) == second);
    CHECK(book->GetSelection() == 0);
    REQUIRE(tabsLifetime.get() == mainTabs);
    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 1);
    CHECK(tabs[0] == mainTabs);
    CHECK(book->GetPagesInDisplayOrder(mainTabs) ==
          std::vector<size_t>{0, 1});

    delete book;
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(tabsLifetime);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook insertion rollback restores selection by identity",
          "[aui][lifetime][selection][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    wxPanel* const selected = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first"));
    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(book->AddPage(selected, "selected", true));
    REQUIRE(book->GetSelection() == 2);
    REQUIRE(selected->IsShown());
    REQUIRE_FALSE(first->IsShown());
    REQUIRE_FALSE(second->IsShown());

    wxPanel* const candidate = new wxPanel(book);
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    unsigned int destructiveCallbacks = 0;
    book->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,
               [&](wxAuiNotebookEvent& event)
               {
                   event.Skip();
                   if ( candidateLifetime.get() == candidate &&
                           event.GetSelection() == 1 )
                   {
                       ++destructiveCallbacks;
                       delete candidate;
                   }
               });

    CHECK(book->InsertPage(1, candidate, "candidate", true));

    CHECK(destructiveCallbacks == 1);
    CHECK_FALSE(candidateLifetime);
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == first);
    CHECK(book->GetPage(1) == second);
    CHECK(book->GetPage(2) == selected);
    CHECK(book->GetSelection() == 2);
    CHECK(selected->IsShown());
    CHECK_FALSE(first->IsShown());
    CHECK_FALSE(second->IsShown());

    const wxAuiNotebookPosition selectedPosition =
        book->GetPagePosition(2);
    REQUIRE(selectedPosition);
    CHECK(selectedPosition.tabCtrl->GetActivePage() ==
          selectedPosition.tabIdx);

    delete book;
}

TEST_CASE("wxAuiNotebook bitmap update stops after hostile art clone",
          "[aui][lifetime][layout][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    wxAuiTabCtrl* const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(mainTabs);

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));
    const unsigned int cloneCallsBefore = state->cloneCalls;

    bool pageDeleted = false;
    state->boundary = TabArtCallbackBoundary::Clone;
    state->heightDelta = 11;
    state->callback = [book, &pageDeleted]
    {
        pageDeleted = book->DeletePage(0);
    };
    state->armed = true;

    // The metric change forces UpdateTabCtrlHeight() through Clone(). The
    // nested public deletion changes the exact page projection, so the outer
    // bitmap writer must not continue into FindTab()/Refresh() with its old
    // page identity.
    CHECK_FALSE(book->SetPageBitmap(0, wxBitmapBundle()));

    CHECK(state->callbackCalls == 1);
    CHECK(state->cloneCalls > cloneCallsBefore);
    CHECK(pageDeleted);
    CHECK_FALSE(firstLifetime);
    REQUIRE(bookLifetime);
    REQUIRE(secondLifetime);
    REQUIRE(book->GetPageCount() == 1);
    CHECK(book->GetPage(0) == second);
    CHECK(book->GetSelection() == 0);
    REQUIRE(tabsLifetime.get() == mainTabs);
    CHECK(book->GetPagesInDisplayOrder(mainTabs) ==
          std::vector<size_t>{0});

    delete book;
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(tabsLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook bitmap update stops when Clone destroys its tabs",
          "[aui][lifetime][layout][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    wxAuiTabCtrl* const tabs = book->GetMainTabCtrl();
    REQUIRE(tabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(tabs);

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));
    state->boundary = TabArtCallbackBoundary::Clone;
    state->heightDelta = 13;
    state->callback = [tabs]
    {
        delete tabs;
    };
    state->armed = true;

    CHECK_FALSE(book->SetPageBitmap(0, wxBitmapBundle()));
    CHECK(state->callbackCalls == 1);
    CHECK_FALSE(tabsLifetime);
    REQUIRE(bookLifetime);

    delete book;
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook tab height keeps the latest nested request",
          "[aui][layout][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const page = new wxPanel(book);
    REQUIRE(book->AddPage(page, "page", true));

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));

    const int nestedHeight = book->GetTabCtrlHeight() + 23;
    state->boundary = TabArtCallbackBoundary::BestSize;
    state->callback = [book, nestedHeight]
    {
        book->SetTabCtrlHeight(nestedHeight);
    };
    state->armed = true;

    // The outer calculation captured automatic sizing, while the callback
    // publishes a newer explicit height through a complete nested update.
    // The stale outer writer must abort instead of restoring its old result.
    book->SetUniformBitmapSize(wxDefaultSize);

    CHECK(state->callbackCalls == 1);
    CHECK(book->GetTabCtrlHeight() == nestedHeight);

    delete book;
}

TEST_CASE("wxAuiNotebook tab height keeps a nested bitmap publication",
          "[aui][layout][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const page = new wxPanel(book);
    REQUIRE(book->AddPage(page, "page", true));

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));

    const wxBitmapBundle largeBitmap = wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(16, 96), 32));
    REQUIRE(book->SetPageBitmap(0, largeBitmap));
    const int largeHeight = book->GetTabCtrlHeight();
    REQUIRE(book->SetPageBitmap(0, wxBitmapBundle()));
    const int smallHeight = book->GetTabCtrlHeight();
    REQUIRE(largeHeight > smallHeight);

    // Make the current published height equal to the result of the nested
    // large-bitmap calculation. Without the input epoch, the nested update is
    // then observationally a no-op and the outer automatic calculation can
    // overwrite it with the stale small-bitmap height computed before the
    // callback.
    book->SetTabCtrlHeight(largeHeight);
    bool bitmapPublished = false;
    state->boundary = TabArtCallbackBoundary::BestSize;
    state->callback = [book, &bitmapPublished, &largeBitmap]
    {
        bitmapPublished = book->SetPageBitmap(0, largeBitmap);
    };
    state->armed = true;

    book->SetTabCtrlHeight(-1);

    CHECK(state->callbackCalls == 1);
    CHECK(bitmapPublished);
    CHECK(book->GetTabCtrlHeight() == largeHeight);

    delete book;
}

TEST_CASE("wxAuiNotebook bitmap publication keeps the latest nested writer",
          "[aui][layout][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "page", true));

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));

    const wxBitmapBundle outerBitmap = wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(16, 48), 32));
    const wxBitmapBundle latestBitmap = wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(16, 96), 32));
    REQUIRE(book->SetPageBitmap(0, latestBitmap));
    const int latestHeight = book->GetTabCtrlHeight();
    REQUIRE(book->SetPageBitmap(0, wxBitmapBundle()));
    REQUIRE(book->GetTabCtrlHeight() < latestHeight);

    bool nestedPublished = false;
    state->boundary = TabArtCallbackBoundary::BestSize;
    state->callback = [book, &latestBitmap, &nestedPublished]
    {
        nestedPublished = book->SetPageBitmap(0, latestBitmap);
    };
    state->armed = true;

    // The outer writer reaches GetBestTabCtrlSize(A), where the nested writer
    // publishes B completely. It must not then publish A to the visual tab.
    CHECK_FALSE(book->SetPageBitmap(0, outerBitmap));

    CHECK(state->callbackCalls == 1);
    CHECK(nestedPublished);
    CHECK(book->GetCanonicalBitmap(0).IsSameAs(latestBitmap));
    CHECK(book->GetVisualBitmap(0).IsSameAs(latestBitmap));
    CHECK(book->GetTabCtrlHeight() == latestHeight);

    delete book;
}

TEST_CASE("wxAuiNotebook bitmap writers on different pages both commit",
          "[aui][layout][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "outer", true));
    REQUIRE(book->AddPage(new wxPanel(book), "nested"));

    const auto state = std::make_shared<BoundaryCallbackTabArtState>();
    book->SetArtProvider(new BoundaryCallbackTabArt(state));

    const wxBitmapBundle outerBitmap = wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(16, 48), 32));
    const wxBitmapBundle nestedBitmap = wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(16, 96), 32));
    bool nestedPublished = false;
    state->boundary = TabArtCallbackBoundary::BestSize;
    state->callback = [book, &nestedBitmap, &nestedPublished]
    {
        nestedPublished = book->SetPageBitmap(1, nestedBitmap);
    };
    state->armed = true;

    // The nested write changes the shared height input, but it targets a
    // different page and must not invalidate page zero's visual publication.
    CHECK(book->SetPageBitmap(0, outerBitmap));

    CHECK(state->callbackCalls == 1);
    CHECK(nestedPublished);
    CHECK(book->GetCanonicalBitmap(0).IsSameAs(outerBitmap));
    CHECK(book->GetVisualBitmap(0).IsSameAs(outerBitmap));
    CHECK(book->GetCanonicalBitmap(1).IsSameAs(nestedBitmap));
    CHECK(book->GetVisualBitmap(1).IsSameAs(nestedBitmap));
    CHECK(book->GetTabCtrlHeight() ==
          book->CalculateCurrentTabHeightForTesting());

    delete book;
}

TEST_CASE("wxAuiNotebook style publication keeps the latest nested writer",
          "[aui][style][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    const auto state = std::make_shared<StyleCallbackTabArtState>();
    book->SetArtProvider(new StyleCallbackTabArt(state));
    state->flags.clear();

    const long baseStyle = book->GetWindowStyleFlag();
    const long outerStyle =
        (baseStyle | wxAUI_NB_CLOSE_ON_ALL_TABS) &
        ~static_cast<long>(wxAUI_NB_WINDOWLIST_BUTTON);
    const long latestStyle =
        (baseStyle | wxAUI_NB_WINDOWLIST_BUTTON) &
        ~static_cast<long>(wxAUI_NB_CLOSE_ON_ALL_TABS);
    state->callback = [book, latestStyle]
    {
        book->SetWindowStyleFlag(latestStyle);
    };
    state->armed = true;

    book->SetWindowStyleFlag(outerStyle);

    CHECK(state->callbackCalls == 1);
    CHECK(book->GetWindowStyleFlag() == latestStyle);
    for ( wxAuiTabCtrl* const tabs : book->GetAllTabCtrls() )
        CHECK(tabs->GetFlags() == static_cast<unsigned int>(latestStyle));

    delete book;
}

TEST_CASE("wxAuiNotebook style sizing callback keeps its nested writer",
          "[aui][style][layout][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    const auto state = std::make_shared<StyleCallbackTabArtState>();
    book->SetArtProvider(new StyleCallbackTabArt(state));

    const long baseStyle = book->GetWindowStyleFlag();
    const long outerStyle =
        (baseStyle | wxAUI_NB_CLOSE_ON_ALL_TABS) &
        ~static_cast<long>(wxAUI_NB_WINDOWLIST_BUTTON);
    const long latestStyle =
        (baseStyle | wxAUI_NB_WINDOWLIST_BUTTON) &
        ~static_cast<long>(wxAUI_NB_CLOSE_ON_ALL_TABS);
    state->boundary = StyleCallbackBoundary::Sizing;
    state->callback = [book, latestStyle]
    {
        book->SetWindowStyleFlag(latestStyle);
    };
    state->armed = true;

    book->SetWindowStyleFlag(outerStyle);

    CHECK(state->callbackCalls == 1);
    CHECK(book->GetWindowStyleFlag() == latestStyle);
    for ( wxAuiTabCtrl* const tabs : book->GetAllTabCtrls() )
        CHECK(tabs->GetFlags() == static_cast<unsigned int>(latestStyle));

    delete book;
}

TEST_CASE("wxAuiNotebook style callback may destroy a captured tab control",
          "[aui][style][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    book->Split(1, wxRIGHT);

    const auto state = std::make_shared<StyleCallbackTabArtState>();
    book->SetArtProvider(new StyleCallbackTabArt(state));
    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 2);
    wxAuiTabCtrl* const victim = tabs.front();
    const wxWeakRef<wxAuiTabCtrl> victimLifetime(victim);
    state->callback = [victim]
    {
        delete victim;
    };
    state->armed = true;

    book->SetWindowStyleFlag(
        book->GetWindowStyleFlag() | wxAUI_NB_WINDOWLIST_BUTTON);

    CHECK(state->callbackCalls == 1);
    CHECK_FALSE(victimLifetime);
    CHECK(bookLifetime);

    delete book;
    CHECK_FALSE(bookLifetime);
}

TEST_CASE("wxAuiNotebook retires a replaced art after its virtual returns",
          "[aui][art][lifetime][reentrancy][transaction]")
{
    ReplacingTabArtBoundary boundary = ReplacingTabArtBoundary::SetFlags;
    SECTION("SetFlags")
    {
    }
    SECTION("GetNormalFont")
    {
        boundary = ReplacingTabArtBoundary::GetNormal;
    }

    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    const auto state = std::make_shared<ReplacingTabArtState>();
    state->book = book;
    state->boundary = boundary;
    state->armed = true;

    book->SetArtProvider(new ReplacingTabArt(state));

    CHECK(state->callbackCalls == 1);
    CHECK(state->primarySurvivedToVirtualReturn);
    CHECK(state->primaryDestroyedInsideVirtual == 0);
    CHECK(state->primaryDestructions == 1);
    CHECK(state->primaryInstances.empty());

    wxAuiTabArt* const canonicalArt = book->GetArtProvider();
    REQUIRE(state->OwnsReplacement(canonicalArt));
    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(state->replacementInstances.size() == tabs.size() + 1);
    std::vector<wxAuiTabArt*> liveArt{canonicalArt};
    for ( wxAuiTabCtrl* const tab : tabs )
    {
        wxAuiTabArt* const visualArt = tab->GetArtProvider();
        REQUIRE(state->OwnsReplacement(visualArt));
        CHECK(visualArt != canonicalArt);
        CHECK(std::find(liveArt.begin(), liveArt.end(), visualArt) ==
              liveArt.end());
        liveArt.push_back(visualArt);
    }

    const size_t liveReplacementCount =
        state->replacementInstances.size();
    const unsigned int destructionsBeforeDelete =
        state->replacementDestructions;
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    delete book;

    CHECK_FALSE(bookLifetime);
    CHECK(state->replacementInstances.empty());
    CHECK(state->replacementDestructions ==
          destructionsBeforeDelete + liveReplacementCount);
}

TEST_CASE("wxAuiNotebook can re-adopt an active retired art",
          "[aui][art][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    const auto state = std::make_shared<ReplacingTabArtState>();
    state->book = book;
    state->readoptPrimary = true;
    state->armed = true;
    auto* const primary = new ReplacingTabArt(state);

    book->SetArtProvider(primary);

    CHECK(state->callbackCalls == 1);
    CHECK(state->replacementReadopted);
    CHECK(state->primarySurvivedToVirtualReturn);
    CHECK(state->primaryDestroyedInsideVirtual == 0);
    CHECK(state->primaryDestructions == 0);
    CHECK(state->replacementInstances.empty());
    CHECK(state->replacementDestructions == 1);
    REQUIRE(book->GetArtProvider() == primary);

    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(state->primaryInstances.size() == tabs.size() + 1);
    for ( wxAuiTabCtrl* const tab : tabs )
    {
        REQUIRE(state->OwnsPrimary(tab->GetArtProvider()));
        CHECK(tab->GetArtProvider() != primary);
    }

    delete book;

    CHECK(state->primaryDestructions == 1);
    CHECK(state->primaryDestroyedInsideVirtual == 0);
    CHECK(state->primaryInstances.empty());
    CHECK(state->replacementInstances.empty());
}

TEST_CASE("wxAuiTabContainer setter exception preserves exact art ownership",
          "[aui][art][exception][lifetime][transaction]")
{
    SECTION("replacement throws")
    {
        const auto oldState = std::make_shared<ThrowingTabArtState>();
        oldState->boundary = ThrowingTabArtBoundary::Sizing;
        const auto throwingState =
            std::make_shared<ThrowingTabArtState>();
        throwingState->boundary = ThrowingTabArtBoundary::Flags;
        throwingState->armed = true;

        {
            wxAuiTabContainer tabs;
            tabs.SetArtProvider(new ThrowingTabArt(oldState));
            auto* const throwing = new ThrowingTabArt(throwingState);

            CHECK_THROWS_AS(tabs.SetArtProvider(throwing),
                            std::runtime_error);
            CHECK(tabs.GetArtProvider() == throwing);
            CHECK(oldState->primaryDestructions == 1);
            CHECK(throwingState->primaryDestructions == 0);
            CHECK(throwingState->primaryInstances.size() == 1);

            // The container remains usable after the propagated exception.
            tabs.SetArtProvider(new wxAuiDefaultTabArt);
            CHECK(throwingState->primaryDestructions == 1);
            CHECK(throwingState->primaryInstances.empty());
        }
    }

    SECTION("replacement re-adopts old provider then throws")
    {
        const auto oldState = std::make_shared<ThrowingTabArtState>();
        oldState->boundary = ThrowingTabArtBoundary::Sizing;
        const auto throwingState =
            std::make_shared<ThrowingTabArtState>();
        throwingState->boundary = ThrowingTabArtBoundary::Flags;
        throwingState->armed = true;

        {
            wxAuiTabContainer tabs;
            auto* const old = new ThrowingTabArt(oldState);
            tabs.SetArtProvider(old);
            throwingState->callback = [&tabs, old]
            {
                tabs.SetArtProvider(old);
            };

            CHECK_THROWS_AS(
                tabs.SetArtProvider(new ThrowingTabArt(throwingState)),
                std::runtime_error);
            CHECK(tabs.GetArtProvider() == old);
            CHECK(oldState->primaryDestructions == 0);
            CHECK(throwingState->primaryDestructions == 1);
            CHECK(throwingState->primaryInstances.empty());
        }

        CHECK(oldState->primaryDestructions == 1);
        CHECK(oldState->primaryInstances.empty());
    }
}

TEST_CASE("wxAuiNotebook stages every callback-capable cloned art setter",
          "[aui][art][exception][lifetime][transaction]")
{
    ThrowingTabArtBoundary boundary = ThrowingTabArtBoundary::NormalFont;
    SECTION("normal font")
    {
    }
    SECTION("selected font")
    {
        boundary = ThrowingTabArtBoundary::SelectedFont;
    }
    SECTION("sizing")
    {
        boundary = ThrowingTabArtBoundary::Sizing;
    }
    SECTION("flags after transfer")
    {
        boundary = ThrowingTabArtBoundary::Flags;
    }

    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    REQUIRE(book->AddPage(new wxPanel(book), "page", true));
    REQUIRE_FALSE(book->GetAllTabCtrls().empty());

    // Keep UpdateTabCtrlHeight() from publishing an unrelated clone before
    // SetArtProvider() reaches its explicit clone-initialization transaction.
    book->SetTabCtrlHeight(book->GetTabCtrlHeight());

    const auto state = std::make_shared<ThrowingTabArtState>();
    state->boundary = boundary;
    state->clonesOnly = true;
    state->armed = true;
    CHECK_THROWS_AS(book->SetArtProvider(new ThrowingTabArt(state)),
                    std::runtime_error);
    CHECK(bookLifetime);
    CHECK(state->throws == 1);
    CHECK(state->primaryInstances.size() == 1);

    if ( boundary == ThrowingTabArtBoundary::Flags )
    {
        // SetArtProvider() adopted this clone before SetFlags() threw. The
        // staging token is stale and must not double-delete live ownership.
        CHECK(state->cloneInstances.size() == 1);
        CHECK(state->cloneDestructions == 0);
    }
    else
    {
        // The clone never transferred, so staging retires it on unwind.
        CHECK(state->cloneInstances.empty());
        CHECK(state->cloneDestructions == 1);
    }

    // A later complete transaction must recover all panes and retire each
    // partially installed provider exactly once.
    book->SetArtProvider(new wxAuiDefaultTabArt);
    CHECK(state->cloneInstances.empty());
    CHECK(state->cloneDestructions == 1);
    CHECK(state->primaryInstances.empty());
    CHECK(state->primaryDestructions == 1);

    delete book;
    CHECK_FALSE(bookLifetime);
}

TEST_CASE("wxAuiNotebook rejects a staged clone consumed by a callback",
          "[aui][art][lifetime][reentrancy][transaction]")
{
    ThrowingTabArtBoundary boundary = ThrowingTabArtBoundary::NormalFont;
    unsigned int expectedNormalFontCalls = 1;
    unsigned int expectedSelectedFontCalls = 0;
    unsigned int expectedSizingCalls = 0;
    unsigned int expectedFlagsCalls = 1;
    SECTION("normal font")
    {
    }
    SECTION("selected font")
    {
        boundary = ThrowingTabArtBoundary::SelectedFont;
        expectedSelectedFontCalls = 1;
    }
    SECTION("sizing")
    {
        boundary = ThrowingTabArtBoundary::Sizing;
        expectedSelectedFontCalls = 1;
        expectedSizingCalls = 1;
    }
    SECTION("flags during publication")
    {
        boundary = ThrowingTabArtBoundary::Flags;
        expectedSelectedFontCalls = 1;
        expectedSizingCalls = 1;
        // The first call is the attempted publication. The second comes from
        // the hostile transfer to the external container inside that call.
        expectedFlagsCalls = 2;
    }

    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    REQUIRE(book->AddPage(new wxPanel(book), "page", true));
    const std::vector<wxAuiTabCtrl*> initialTabs = book->GetAllTabCtrls();
    REQUIRE(initialTabs.size() == 1);
    wxAuiTabCtrl* const visualTabs = initialTabs.front();
    const wxWeakRef<wxAuiTabCtrl> visualTabsLifetime(visualTabs);
    wxAuiTabArt* const originalVisualArt = visualTabs->GetArtProvider();

    // Keep UpdateTabCtrlHeight() from introducing a separate clone before
    // the explicit clone-initialization transaction exercised below.
    book->SetTabCtrlHeight(book->GetTabCtrlHeight());

    wxAuiTabContainer externalContainer;
    const auto state = std::make_shared<ThrowingTabArtState>();
    state->boundary = boundary;
    state->clonesOnly = true;
    state->throwAfterCallback = false;
    ThrowingTabArtState* const statePtr = state.get();
    state->callback = [&externalContainer, statePtr]
    {
        wxAuiTabArt* const transferred = statePtr->callbackArt;
        REQUIRE(transferred);
        externalContainer.SetArtProvider(transferred);
        REQUIRE(externalContainer.GetArtProvider() == transferred);
        externalContainer.SetArtProvider(new wxAuiDefaultTabArt);
    };
    state->armed = true;

    CHECK_NOTHROW(book->SetArtProvider(new ThrowingTabArt(state)));
    CHECK(bookLifetime);
    CHECK(state->callbackCalls == 1);
    CHECK(state->callbackArt == nullptr);
    CHECK(state->throws == 0);
    CHECK(state->cloneNormalFontCalls == expectedNormalFontCalls);
    CHECK(state->cloneSelectedFontCalls == expectedSelectedFontCalls);
    CHECK(state->cloneSizingCalls == expectedSizingCalls);
    CHECK(state->cloneFlagsCalls == expectedFlagsCalls);
    CHECK(state->cloneInstances.empty());
    CHECK(state->cloneDestructions == 1);

    // A pre-publication transfer must stop the next setter. A transfer from
    // SetFlags() must roll the target back to its old provider. In both cases
    // no dangling clone may be observable from the visual tab container.
    REQUIRE(visualTabsLifetime.get() == visualTabs);
    const std::vector<wxAuiTabCtrl*> currentTabs = book->GetAllTabCtrls();
    REQUIRE(currentTabs.size() == 1);
    CHECK(currentTabs.front() == visualTabs);
    CHECK(visualTabs->GetArtProvider() == originalVisualArt);

    // A complete subsequent transaction must retire the canonical primary
    // and recover the visual projection without touching the consumed clone.
    book->SetArtProvider(new wxAuiDefaultTabArt);
    CHECK(state->primaryInstances.empty());
    CHECK(state->primaryDestructions == 1);
    CHECK(state->cloneInstances.empty());
    CHECK(state->cloneDestructions == 1);

    delete book;
    CHECK_FALSE(bookLifetime);
}

TEST_CASE("wxAuiNotebook failed tab-frame art setup leaves no orphan",
          "[aui][art][exception][split][lifetime][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));

    const auto state = std::make_shared<ThrowingTabArtState>();
    state->boundary = ThrowingTabArtBoundary::Flags;
    state->clonesOnly = true;
    book->SetArtProvider(new ThrowingTabArt(state));
    REQUIRE(book->GetAllTabCtrls().size() == 1);
    const size_t liveClonesBefore = state->cloneInstances.size();
    const unsigned int cloneDestructionsBefore =
        state->cloneDestructions;

    state->armed = true;
    CHECK_THROWS_AS(book->Split(1, wxRIGHT), std::runtime_error);
    CHECK(book->GetAllTabCtrls().size() == 1);
    CHECK(book->GetPageCount() == 2);
    CHECK(book->GetPagePosition(0).tabCtrl ==
          book->GetPagePosition(1).tabCtrl);
    CHECK(state->cloneInstances.size() == liveClonesBefore);
    CHECK(state->cloneDestructions == cloneDestructionsBefore + 1);

    // ThrowAt() is one-shot. The same operation must now complete, proving
    // that neither an unparented tab control nor a manager pane survived.
    book->Split(1, wxRIGHT);
    CHECK(book->GetAllTabCtrls().size() == 2);

    delete book;
    CHECK(state->primaryInstances.empty());
    CHECK(state->cloneInstances.empty());
}

TEST_CASE("wxAuiTabContainer render callbacks may destroy the container",
          "[aui][art][render][lifetime][reentrancy]")
{
    RenderDestroyBoundary boundary = RenderDestroyBoundary::Background;
    SECTION("DrawBackground")
    {
    }
    SECTION("DrawButton")
    {
        boundary = RenderDestroyBoundary::Button;
    }
    SECTION("GetPageTabSize")
    {
        boundary = RenderDestroyBoundary::PageSize;
    }
    SECTION("DrawPageTab")
    {
        boundary = RenderDestroyBoundary::Page;
    }

    wxPanel* const owner = new wxPanel(wxTheApp->GetTopWindow());
    wxPanel* const page = new wxPanel(owner);
    auto* const tabs = new RenderableTabContainer;
    tabs->SetFlags(wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS);
    wxAuiNotebookPage pageInfo;
    pageInfo.window = page;
    pageInfo.caption = "page";
    pageInfo.active = true;
    REQUIRE(tabs->AddPage(pageInfo));
    tabs->SetRect(wxRect(0, 0, 320, 48), owner);
    tabs->SetRowHeight(32);

    const auto state = std::make_shared<RenderDestroyingTabArtState>();
    state->boundary = boundary;
    tabs->SetArtProvider(new RenderDestroyingTabArt(state));
    state->victim = tabs;
    state->armed = true;

    wxBitmap bitmap(320, 64);
    wxMemoryDC dc(bitmap);
    tabs->RenderForTesting(dc, owner);

    CHECK(state->calls == 1);
    CHECK(state->survivedToVirtualReturn);
    CHECK(state->destructionsInsideVirtual == 0);
    CHECK(state->destructions == 1);

    delete owner;
}

TEST_CASE("wxAuiTabContainer render rejects a destroyed sibling page",
          "[aui][art][render][lifetime][reentrancy][transaction]")
{
    RenderDestroyBoundary boundary = RenderDestroyBoundary::Background;
    SECTION("DrawBackground")
    {
    }
    SECTION("DrawButton")
    {
        boundary = RenderDestroyBoundary::Button;
    }
    SECTION("GetPageTabSize")
    {
        boundary = RenderDestroyBoundary::PageSize;
    }
    SECTION("DrawPageTab")
    {
        boundary = RenderDestroyBoundary::Page;
    }

    wxPanel* const owner = new wxPanel(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(owner);
    wxPanel* const second = new wxPanel(owner);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    auto* const tabs = new RenderableTabContainer;
    tabs->SetFlags(wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS);
    wxAuiNotebookPage firstInfo;
    firstInfo.window = first;
    firstInfo.caption = "first";
    firstInfo.active = true;
    REQUIRE(tabs->AddPage(firstInfo));
    wxAuiNotebookPage secondInfo;
    secondInfo.window = second;
    secondInfo.caption = "second";
    REQUIRE(tabs->AddPage(secondInfo));
    tabs->SetRect(wxRect(0, 0, 320, 48), owner);
    tabs->SetRowHeight(32);

    const wxAuiTabContainer& constTabs = *tabs;
    const wxRect originalFirstRect = constTabs.GetPage(0).rect;
    std::vector<wxRect> originalFirstButtonRects;
    for ( const wxAuiTabContainerButton& button :
          constTabs.GetPage(0).buttons )
    {
        originalFirstButtonRects.push_back(button.rect);
    }

    const auto state = std::make_shared<SiblingDestroyingTabArtState>();
    state->boundary = boundary;
    state->trigger = first;
    state->victim = second;
    tabs->SetArtProvider(new SiblingDestroyingTabArt(state));
    state->armed = true;

    wxBitmap bitmap(320, 64);
    wxMemoryDC dc(bitmap);
    tabs->RenderForTesting(dc, owner);

    CHECK(state->calls == 1);
    CHECK(state->boundaryCalls == 1);
    REQUIRE(firstLifetime);
    CHECK_FALSE(secondLifetime);
    CHECK(constTabs.GetPage(0).rect == originalFirstRect);
    REQUIRE(constTabs.GetPage(0).buttons.size() ==
            originalFirstButtonRects.size());
    for ( size_t i = 0; i < originalFirstButtonRects.size(); ++i )
    {
        CHECK(constTabs.GetPage(0).buttons[i].rect ==
              originalFirstButtonRects[i]);
    }

    delete tabs;
    delete owner;
}

TEST_CASE("wxAuiTabContainer multiline layout rejects a destroyed sibling",
          "[aui][art][layout][lifetime][reentrancy][transaction]")
{
    wxPanel* const owner = new wxPanel(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(owner);
    wxPanel* const second = new wxPanel(owner);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    auto* const tabs = new RenderableTabContainer;
    tabs->SetFlags(wxAUI_NB_MULTILINE | wxAUI_NB_CLOSE_ON_ALL_TABS);
    wxAuiNotebookPage firstInfo;
    firstInfo.window = first;
    firstInfo.caption = "first";
    firstInfo.active = true;
    REQUIRE(tabs->AddPage(firstInfo));
    wxAuiNotebookPage secondInfo;
    secondInfo.window = second;
    secondInfo.caption = "second";
    REQUIRE(tabs->AddPage(secondInfo));
    tabs->SetRect(wxRect(0, 0, 320, 48), owner);

    const wxAuiTabContainer& constTabs = *tabs;
    const bool originalRowEnd = constTabs.GetPage(0).rowEnd;
    const auto state = std::make_shared<SiblingDestroyingTabArtState>();
    state->boundary = RenderDestroyBoundary::PageSize;
    state->trigger = first;
    state->victim = second;
    tabs->SetArtProvider(new SiblingDestroyingTabArt(state));
    state->armed = true;

    CHECK(tabs->LayoutForTesting(wxRect(0, 0, 320, 48), owner) == 0);
    CHECK(state->calls == 1);
    CHECK(state->boundaryCalls == 1);
    REQUIRE(firstLifetime);
    CHECK_FALSE(secondLifetime);
    CHECK(constTabs.GetPage(0).rowEnd == originalRowEnd);

    delete tabs;
    delete owner;
}

TEST_CASE("wxAuiNotebook system colour pass snapshots every sibling",
          "[aui][art][colour][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);

    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 2);
    wxAuiTabCtrl* const victim = tabs.back();
    const wxWeakRef<wxAuiTabCtrl> victimLifetime(victim);

    const auto state = std::make_shared<ColourCallbackTabArtState>();
    book->SetArtProvider(new ColourCallbackTabArt(state));

    bool destroysSibling = false;
    const size_t expectedMutatedOffset = victim->GetTabOffset() + 1;
    SECTION("destroy sibling")
    {
        destroysSibling = true;
        state->callback = [victim]
        {
            delete victim;
        };
    }
    SECTION("mutate sibling")
    {
        state->callback = [victim]
        {
            victim->SetTabOffset(victim->GetTabOffset() + 1);
        };
    }
    state->armed = true;

    wxSysColourChangedEvent event;
    event.SetEventObject(book);
    book->GetEventHandler()->ProcessEvent(event);

    CHECK(state->calls == 1);
    CHECK(bookLifetime);
    if ( destroysSibling )
        CHECK_FALSE(victimLifetime);
    else
    {
        REQUIRE(victimLifetime);
        CHECK(victim->GetTabOffset() == expectedMutatedOffset);
    }

    delete book;
    CHECK_FALSE(bookLifetime);
}

TEST_CASE("wxAuiNotebook window menu validates the selected projection",
          "[aui][art][menu][exception][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(book->GetSelection() == 0);

    const auto state = std::make_shared<DropDownCallbackTabArtState>();
    state->result = 1;
    book->SetArtProvider(new DropDownCallbackTabArt(state));

    SECTION("provider mutates its page projection copy")
    {
        state->callback = [](const wxAuiNotebookPageArray& pages)
        {
            REQUIRE(pages.GetCount() == 2);
            auto& mutablePages =
                const_cast<wxAuiNotebookPageArray&>(pages);
            mutablePages[1].caption = "not the captured item";
        };
        CHECK_FALSE(book->ShowWindowMenu());
        CHECK(book->GetSelection() == 0);
    }

    SECTION("provider mutates the live topology")
    {
        state->callback = [book](const wxAuiNotebookPageArray&)
        {
            REQUIRE(book->SetPageText(1, "new live caption"));
        };
        CHECK_FALSE(book->ShowWindowMenu());
        CHECK(book->GetSelection() == 0);
    }

    SECTION("provider returns an out-of-range item")
    {
        state->result = 2;
        CHECK_FALSE(book->ShowWindowMenu());
        CHECK(book->GetSelection() == 0);
    }

    SECTION("provider destroys the returned live page")
    {
        const wxWeakRef<wxWindow> firstLifetime(first);
        const wxWeakRef<wxWindow> secondLifetime(second);
        state->callback = [second](const wxAuiNotebookPageArray&)
        {
            delete second;
        };
        state->result = 1;

        CHECK_FALSE(book->ShowWindowMenu());
        REQUIRE(firstLifetime);
        CHECK_FALSE(secondLifetime);
        CHECK(book->GetSelection() == 0);
    }

    SECTION("provider throws and the next request recovers")
    {
        state->throwAfterCallback = true;
        CHECK_THROWS_AS(book->ShowWindowMenu(), std::runtime_error);
        CHECK(book->GetSelection() == 0);

        state->throwAfterCallback = false;
        state->result = wxNOT_FOUND;
        CHECK_FALSE(book->ShowWindowMenu());
        CHECK(book->GetSelection() == 0);
    }

    CHECK(state->calls >= 1);
    delete book;
}

TEST_CASE("wxAuiTabCtrl window-list button validates the selected projection",
          "[aui][art][menu][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(book->GetSelection() == 0);

    const auto state = std::make_shared<DropDownCallbackTabArtState>();
    state->result = 1;
    book->SetArtProvider(new DropDownCallbackTabArt(state));
    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 1);

    unsigned int changingEvents = 0;
    unsigned int changedEvents = 0;
    book->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING,
        [&changingEvents](wxAuiNotebookEvent& event)
        {
            ++changingEvents;
            event.Skip();
        });
    book->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,
        [&changedEvents](wxAuiNotebookEvent& event)
        {
            ++changedEvents;
            event.Skip();
        });

    SECTION("stable returned page is selected")
    {
        book->ClickWindowListForTesting(tabs.front());

        CHECK(state->calls == 1);
        REQUIRE(firstLifetime);
        REQUIRE(secondLifetime);
        CHECK(book->GetSelection() == 1);
        CHECK(changingEvents == 1);
        CHECK(changedEvents == 1);
    }

    SECTION("provider destroys the returned live page")
    {
        state->callback = [second](const wxAuiNotebookPageArray&)
        {
            delete second;
        };

        book->ClickWindowListForTesting(tabs.front());

        CHECK(state->calls == 1);
        REQUIRE(firstLifetime);
        CHECK_FALSE(secondLifetime);
        CHECK(book->GetSelection() == 0);
        CHECK(changingEvents == 0);
        CHECK(changedEvents == 0);
    }

    SECTION("provider mutates the returned menu payload")
    {
        state->callback = [](const wxAuiNotebookPageArray& pages)
        {
            REQUIRE(pages.GetCount() == 2);
            auto& mutablePages =
                const_cast<wxAuiNotebookPageArray&>(pages);
            mutablePages[1].caption = "stale menu payload";
        };

        book->ClickWindowListForTesting(tabs.front());

        CHECK(state->calls == 1);
        REQUIRE(firstLifetime);
        REQUIRE(secondLifetime);
        CHECK(book->GetSelection() == 0);
        CHECK(changingEvents == 0);
        CHECK(changedEvents == 0);
    }

    delete book;
}

TEST_CASE("wxAuiNotebook destruction retires art after its callback",
          "[aui][art][lifetime][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "page", true));
    const wxWeakRef<wxAuiNotebook> bookLifetime(book);

    const auto state = std::make_shared<ReplacingTabArtState>();
    state->book = book;
    state->destroyBook = true;
    state->armed = true;

    book->SetArtProvider(new ReplacingTabArt(state));

    CHECK_FALSE(bookLifetime);
    CHECK(state->callbackCalls == 1);
    CHECK(state->primarySurvivedToVirtualReturn);
    CHECK(state->primaryDestroyedInsideVirtual == 0);
    CHECK(state->primaryDestructions == 1);
    CHECK(state->primaryInstances.empty());
    CHECK(state->replacementInstances.empty());
}

TEST_CASE("wxAuiNotebook art font query preserves a nested font writer",
          "[aui][font][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    int fixedHeight = wxNOT_FOUND;
    SECTION("automatic height changes")
    {
    }
    SECTION("unchanged fixed height")
    {
        fixedHeight = book->GetTabCtrlHeight() + 19;
        book->SetTabCtrlHeight(fixedHeight);
        REQUIRE(book->GetTabCtrlHeight() == fixedHeight);
    }
    const int heightBeforeArt = book->GetTabCtrlHeight();

    wxFont staleFont(*wxNORMAL_FONT);
    staleFont.SetPointSize(staleFont.GetPointSize() + 2);
    wxFont latestFont(*wxNORMAL_FONT);
    latestFont.SetPointSize(latestFont.GetPointSize() + 5);
    wxFont selectedFont(*wxNORMAL_FONT);
    selectedFont.SetPointSize(selectedFont.GetPointSize() + 8);
    selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    const auto state = std::make_shared<FontCallbackTabArtState>();
    state->heightDelta = fixedHeight == wxNOT_FOUND ? 17 : 0;
    state->boundary = FontCallbackBoundary::GetNormal;
    state->getNormalResult = staleFont;
    state->getSelectedResult = selectedFont;
    state->overrideNormalResult = true;
    state->overrideSelectedResult = true;
    state->callback = [book, latestFont]
    {
        book->SetNormalFont(latestFont);
    };
    state->armed = true;

    book->SetArtProvider(new FontCallbackTabArt(state));

    CHECK(state->callbackCalls == 1);
    CHECK(book->GetNormalFontForTesting() == latestFont);
    CHECK(book->GetSelectedFontForTesting() == selectedFont);
    REQUIRE(state->Owns(book->GetArtProvider()));
    CHECK(book->GetArtProvider()->GetNormalFont() == latestFont);
    CHECK(book->GetArtProvider()->GetSelectedFont() == selectedFont);
    CHECK(state->instances.size() ==
          book->GetAllTabCtrls().size() + 1);
    for ( wxAuiTabCtrl* const tabs : book->GetAllTabCtrls() )
    {
        wxAuiTabArt* const visualArt = tabs->GetArtProvider();
        REQUIRE(state->Owns(visualArt));
        CHECK(visualArt != book->GetArtProvider());
        CHECK(visualArt->GetNormalFont() == latestFont);
        CHECK(visualArt->GetSelectedFont() == selectedFont);
    }
    CHECK(book->GetTabCtrlHeight() ==
          book->CalculateCurrentTabHeightForTesting());
    if ( fixedHeight != wxNOT_FOUND )
        CHECK(book->GetTabCtrlHeight() == fixedHeight);
    else
        CHECK(book->GetTabCtrlHeight() != heightBeforeArt);

    delete book;
}

TEST_CASE("wxAuiNotebook SetFont is one latest-writer transaction",
          "[aui][font][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "page", true));

    const auto state = std::make_shared<FontCallbackTabArtState>();
    book->SetArtProvider(new FontCallbackTabArt(state));

    wxFont outerFont(*wxNORMAL_FONT);
    outerFont.SetPointSize(outerFont.GetPointSize() + 2);
    wxFont latestFont(*wxNORMAL_FONT);
    latestFont.SetPointSize(latestFont.GetPointSize() + 6);
    wxFont latestSelected(latestFont);
    latestSelected.SetWeight(wxFONTWEIGHT_BOLD);
    state->boundary = FontCallbackBoundary::SetNormal;
    state->callback = [book, latestFont]
    {
        CHECK(book->SetFont(latestFont));
    };
    state->armed = true;

    CHECK_FALSE(book->SetFont(outerFont));

    CHECK(state->callbackCalls == 1);
    CHECK(book->GetNormalFontForTesting() == latestFont);
    CHECK(book->GetSelectedFontForTesting() == latestSelected);
    CHECK(book->GetArtProvider()->GetNormalFont() == latestFont);
    CHECK(book->GetArtProvider()->GetSelectedFont() == latestSelected);

    delete book;
}

TEST_CASE("wxAuiNotebook selection stops stale font propagation",
          "[aui][font][selection][reentrancy][transaction]")
{
    wxAuiNotebookTestPeer* const book =
        new wxAuiNotebookTestPeer(wxTheApp->GetTopWindow());
    REQUIRE(book->AddPage(new wxPanel(book), "first", true));
    REQUIRE(book->AddPage(new wxPanel(book), "second"));
    REQUIRE(book->AddPage(new wxPanel(book), "third"));
    book->Split(2, wxRIGHT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);

    const auto state = std::make_shared<FontCallbackTabArtState>();
    book->SetArtProvider(new FontCallbackTabArt(state));
    state->selectedFonts.clear();

    wxFont latestFont(*wxNORMAL_FONT);
    latestFont.SetPointSize(latestFont.GetPointSize() + 7);
    state->boundary = FontCallbackBoundary::SetSelected;
    state->callback = [book, latestFont]
    {
        book->SetSelectedFont(latestFont);
    };
    state->armed = true;

    book->SetSelection(1);

    CHECK(state->callbackCalls == 1);
    // One visual callback triggered the nested canonical publication. The
    // font epoch prevents stale propagation to the remaining pane.
    CHECK(state->selectedFonts.size() == 2);
    CHECK(book->GetSelectedFontForTesting() == latestFont);
    CHECK(book->GetArtProvider()->GetSelectedFont() == latestFont);

    delete book;
}

TEST_CASE("wxAuiNotebook teardown rejects destroy-event topology writers",
          "[aui][bookctrl][lifetime][reentrancy][transaction]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    wxPanel* const candidate =
        new wxPanel(wxTheApp->GetTopWindow());
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    wxWindow* const candidateParent = candidate->GetParent();
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(second, "second"));

    unsigned int destroyEvents = 0;
    book->Bind(
        wxEVT_DESTROY,
        [&](wxWindowDestroyEvent& event)
        {
            event.Skip();
            if ( event.GetEventObject() != book )
                return;

            ++destroyEvents;

            // SendDestroyEvent() runs while the derived page model is still
            // intact, but the teardown transaction must reject every public
            // writer before any part of that model can be changed.
            CHECK(book->GetPageCount() == 2);
            CHECK(book->GetPage(0) == first);
            CHECK(book->GetPage(1) == second);
            CHECK(book->GetSelection() == 0);
            CHECK_FALSE(book->InsertPage(1, candidate, "candidate"));
            CHECK_FALSE(book->RemovePage(0));
            CHECK(book->GetPageCount() == 2);
            CHECK(book->GetPage(0) == first);
            CHECK(book->GetPage(1) == second);
            CHECK(candidate->GetParent() == candidateParent);

            // A reader which normally creates the main pane must degrade to
            // a non-mutating null result once destruction has begun.
            CHECK(book->GetMainTabCtrl() == nullptr);
        },
        book->GetId());

    delete book;

    CHECK(destroyEvents == 1);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
    REQUIRE(candidateLifetime);
    CHECK(candidate->GetParent() == candidateParent);
    delete candidate;
    CHECK_FALSE(candidateLifetime);
}

TEST_CASE("wxAuiNotebook keeps one weakly stable main tab control",
          "[aui][lifetime][layout]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const second = new wxPanel(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> secondLifetime(second);

    REQUIRE(book->AddPage(first, "first", true));
    const wxAuiNotebookPosition firstPosition = book->GetPagePosition(0);
    REQUIRE(firstPosition);
    wxAuiTabCtrl* const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    const wxWeakRef<wxAuiTabCtrl> tabsLifetime(mainTabs);
    CHECK(firstPosition.tabCtrl == mainTabs);
    REQUIRE(book->GetAllTabCtrls().size() == 1);

    REQUIRE(book->AddPage(second, "second"));
    REQUIRE(tabsLifetime.get() == mainTabs);
    CHECK(book->GetMainTabCtrl() == mainTabs);
    const std::vector<wxAuiTabCtrl*> tabs = book->GetAllTabCtrls();
    REQUIRE(tabs.size() == 1);
    CHECK(tabs[0] == mainTabs);
    CHECK(book->GetPagePosition(0).tabCtrl == mainTabs);
    CHECK(book->GetPagePosition(1).tabCtrl == mainTabs);
    CHECK(book->GetPagesInDisplayOrder(mainTabs) ==
          std::vector<size_t>{0, 1});

    delete book;
    CHECK_FALSE(tabsLifetime);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(secondLifetime);
}

TEST_CASE("wxAuiNotebook layout stops when orphan callback destroys the book",
          "[aui][lifetime][layout][reentrancy]")
{
    wxAuiNotebook* const book =
        new wxAuiNotebook(wxTheApp->GetTopWindow());
    wxPanel* const first = new wxPanel(book);
    wxPanel* const orphan = new wxPanel(book);
    const wxWeakRef<wxWindow> bookLifetime(book);
    const wxWeakRef<wxWindow> firstLifetime(first);
    const wxWeakRef<wxWindow> orphanLifetime(orphan);
    REQUIRE(book->AddPage(first, "first", true));
    REQUIRE(book->AddPage(orphan, "orphan"));

    class BookDestroyingDeserializer final : public wxAuiBookDeserializer
    {
    public:
        explicit BookDestroyingDeserializer(wxAuiNotebook* book)
            : m_book(book)
        {
        }

        std::vector<wxAuiTabLayoutInfo>
        LoadNotebookTabs(const wxString&) override
        {
            wxAuiTabLayoutInfo mainTab;
            mainTab.dock_direction = wxAUI_DOCK_CENTER;
            mainTab.pages = {0};
            return {mainTab};
        }

        bool HandleOrphanedPage(wxAuiNotebook&,
                                int page,
                                wxAuiTabCtrl**,
                                int*) override
        {
            CHECK(page == 1);
            m_called = true;
            wxAuiNotebook* const book = m_book;
            m_book = nullptr;
            delete book;
            return true;
        }

        wxAuiNotebook* m_book;
        bool m_called = false;
    } deserializer(book);

    book->LoadLayout("hostile", deserializer);

    CHECK(deserializer.m_called);
    CHECK_FALSE(bookLifetime);
    CHECK_FALSE(firstLifetime);
    CHECK_FALSE(orphanLifetime);

    if ( bookLifetime )
        delete book;
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::RemoveLastPageEvent", "[aui]")
{
    wxPanel *p = new wxPanel(nb.get());
    REQUIRE( nb->AddPage(p, "Page 1") );
    CHECK( nb->GetSelection() == 0 );

    int numChanged = 0;
    int oldSelection = wxNOT_FOUND;
    int selection = wxNOT_FOUND;

    nb->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,
             [&](wxAuiNotebookEvent& event)
             {
                 numChanged++;
                 oldSelection = event.GetOldSelection();
                 selection = event.GetSelection();
             });

    SECTION( "DeletePage" )
    {
        REQUIRE( nb->DeletePage(0) );
    }

    SECTION( "RemovePage" )
    {
        REQUIRE( nb->RemovePage(0) );
    }

    CHECK( nb->GetSelection() == wxNOT_FOUND );
    CHECK( numChanged == 1 );
    CHECK( oldSelection == 0 );
    CHECK( selection == wxNOT_FOUND );
}

TEST_CASE("wxAuiNotebook::SplitTabEventSelections", "[aui]")
{
    TestAuiNotebook nb;
    wxPanel *p1 = new wxPanel(&nb);
    wxPanel *p2 = new wxPanel(&nb);
    REQUIRE( nb.AddPage(p1, "Page 1") );
    REQUIRE( nb.AddPage(p2, "Page 2") );

    nb.Split(1, wxRIGHT);

    wxAuiTabCtrl *tabCtrl = nullptr;
    int tabIdx = wxNOT_FOUND;
    REQUIRE( nb.FindTab(p2, &tabCtrl, &tabIdx) );
    REQUIRE( tabCtrl );
    CHECK( tabIdx == 0 );

    std::vector<int> selections;

    SECTION( "Middle down" )
    {
        nb.Bind(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN,
                [&](wxAuiNotebookEvent& event)
                {
                    selections.push_back(event.GetSelection());
                });

        nb.OnTabMiddleDown(tabCtrl, tabIdx);
    }

    SECTION( "Middle up" )
    {
        nb.Bind(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP,
                [&](wxAuiNotebookEvent& event)
                {
                    selections.push_back(event.GetSelection());
                });

        nb.OnTabMiddleUp(tabCtrl, tabIdx);
    }

    SECTION( "Right down" )
    {
        nb.Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN,
                [&](wxAuiNotebookEvent& event)
                {
                    selections.push_back(event.GetSelection());
                });

        nb.OnTabRightDown(tabCtrl, tabIdx);
    }

    SECTION( "Right up" )
    {
        nb.Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP,
                [&](wxAuiNotebookEvent& event)
                {
                    selections.push_back(event.GetSelection());
                });

        nb.OnTabRightUp(tabCtrl, tabIdx);
    }

    REQUIRE( selections.size() == 1 );
    CHECK( selections[0] == 1 );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::Layout", "[aui]")
{
    const auto addPage = [this](int n)
    {
        return nb->AddPage(new wxPanel(nb.get()),
                           wxString::Format("Page %d", n + 1));
    };

    for ( int n = 0; n < 5; n++ )
    {
        REQUIRE( addPage(n) );
    }

    using Ints = std::vector<int>;
    using Indices = std::vector<size_t>;

    // This serializer allows the code below to tweak its data before using it
    // as deserializer.
    class TestSerializer : public wxAuiBookSerializer,
                           public wxAuiBookDeserializer
    {
    public:
        virtual void BeforeSaveNotebook(const wxString& name) override
        {
            m_name = name;
            m_afterSaveCalled = false;
            m_tabsLayoutInfo.clear();
        }

        virtual void
        SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) override
        {
            m_tabsLayoutInfo.push_back(tab);
        }

        virtual void AfterSaveNotebook() override
        {
            m_afterSaveCalled = true;
        }

        virtual std::vector<wxAuiTabLayoutInfo>
        LoadNotebookTabs(const wxString& name) override
        {
            CHECK( name == m_name );

            m_orphanedPages.clear();

            return m_tabsLayoutInfo;
        }

        virtual bool
        HandleOrphanedPage(wxAuiNotebook& WXUNUSED(book),
                           int page,
                           wxAuiTabCtrl** WXUNUSED(tabCtrl),
                           int* tabIndex) override
        {
            m_orphanedPages.push_back(page);

            *tabIndex = m_orphanedPageReturnIndex;

            return m_orphanedPageReturnValue;
        }

        wxString m_name;
        std::vector<wxAuiTabLayoutInfo> m_tabsLayoutInfo;
        bool m_afterSaveCalled = false;

        Ints m_orphanedPages;
        bool m_orphanedPageReturnValue = true;
        int m_orphanedPageReturnIndex = wxNOT_FOUND;
    } ser;

    // Just for convenience.
    auto& info = ser.m_tabsLayoutInfo;

    // Check the default layout has expected representation.
    nb->SaveLayout("layout", ser);
    CHECK( ser.m_name == "layout" );
    CHECK( ser.m_afterSaveCalled );
    REQUIRE( info.size() == 1 );

    CHECK( info[0].pages == Ints{} );
    CHECK( info[0].pinned == Ints{} );
    CHECK( info[0].active == 0 );


    // Check that the active page is restored correctly.
    info[0].active = 1;
    nb->LoadLayout("layout", ser);

    CHECK( nb->GetSelection() == 1 );


    // Check that page order is serialized as expected.
    auto* mainTabCtrl = nb->GetMainTabCtrl();
    REQUIRE( mainTabCtrl );
    CHECK( mainTabCtrl->MovePage(1, 4) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );
    CHECK( info[0].pages == Ints{0, 2, 3, 4, 1} );


    // Check that pinned pages are serialized as expected.
    REQUIRE( nb->SetPageKind(2, wxAuiTabKind::Pinned) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );

    // Note that pinning a page moves it in front of all other pages.
    CHECK( info[0].pages == Ints{2, 0, 3, 4, 1} );
    CHECK( info[0].pinned == Ints{2} );


    // Check a more complicated case with both locked and pinned pages.
    REQUIRE( nb->SetPageKind(4, wxAuiTabKind::Locked) );
    REQUIRE( nb->SetPageKind(3, wxAuiTabKind::Pinned) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );

    // Note that pinning a page moves it in front of all other pages.
    CHECK( info[0].pages == Ints{4, 2, 3, 0, 1} );
    CHECK( info[0].pinned == Ints{2, 3} );


    // Check that restoring existing layout after adding some pages works.
    addPage(5);
    addPage(6);
    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    // By default, orphaned pages should have been appended.
    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{4, 2, 3, 0, 1, 5, 6} );

    // But we can change this by telling deserializer to insert them in front.
    ser.m_orphanedPageReturnIndex = 0;

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{6, 5, 4, 2, 3, 0, 1} );

    // Or drop them entirely.
    ser.m_orphanedPageReturnValue = false;

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{4, 2, 3, 0, 1} );


    // Finally, check that invalid data is handled gracefully.
    info[0].active = 100;
    info[0].pages = Ints{10, 0, 1, 2, 3, 4};
    info[0].pinned = Ints{2, 99, 0};

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{} );

    // Locked tab should have remained first.
    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) == Indices{4, 0, 1, 2, 3} );

    // And selection should have been set to it because the specified value was
    // invalid.
    CHECK( nb->GetSelection() == 4 );

    // And only tabs appearing before the normal ones can be pinned.
    CHECK( nb->GetPageKind(0) == wxAuiTabKind::Pinned );
    CHECK( nb->GetPageKind(1) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(2) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(3) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(4) == wxAuiTabKind::Locked );
}

TEST_CASE("wxAuiToolBar::Items", "[aui][toolbar]")
{
    std::unique_ptr<wxAuiToolBar> tbar{new wxAuiToolBar(wxTheApp->GetTopWindow())};

    // Check that adding more toolbar elements doesn't invalidate the existing
    // pointers.
    auto first = tbar->AddLabel(wxID_ANY, "first");
    tbar->AddLabel(wxID_ANY, "second");
    CHECK( first->GetLabel() == "first" );
}

#endif
