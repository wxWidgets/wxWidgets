///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibook.cpp
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Jens Lody
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_AUI

#include "wx/aui/auibook.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/frame.h"
#endif

#include "wx/aui/serializer.h"
#include "wx/aui/tabmdi.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#include "wx/dcbuffer.h" // just for wxALWAYS_NATIVE_DOUBLE_BUFFER

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif

#include <algorithm>
#include <memory>
#include <new>
#include <stack>
#include <unordered_map>
#include <unordered_set>

wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_CANCEL_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);

wxIMPLEMENT_CLASS(wxAuiNotebook, wxBookCtrlBase);
wxIMPLEMENT_CLASS(wxAuiTabCtrl, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxAuiNotebookEvent, wxBookCtrlEvent);


// Local helper functions
namespace
{

// Art providers are owned by their tab container, but all of their methods
// are virtual and may synchronously replace the provider (or destroy the
// container) from application code. Keep this state out of the public classes
// to preserve their ABI, and defer deletion of a retired provider until the
// outermost virtual call using it has returned.
using wxAuiTabArtAdoption = unsigned long long;

struct wxAuiTabArtLifetime
{
    unsigned int leaseDepth = 0;
    wxAuiTabArtAdoption adoption = 0;
    bool retired = false;
    // Null denotes a private staging adoption; a published provider records
    // the one tab container which currently owns its raw pointer.
    const wxAuiTabContainer* owner = nullptr;
};

std::unordered_map<wxAuiTabArt*, wxAuiTabArtLifetime>&
wxAuiGetTabArtLifetimes()
{
    // AUI windows and their art providers are confined to the GUI thread.
    static thread_local std::unordered_map<
        wxAuiTabArt*, wxAuiTabArtLifetime> lifetimes;
    return lifetimes;
}

wxAuiTabArtAdoption wxAuiNextTabArtAdoption()
{
    // Use one sequence for the entire GUI thread, rather than one sequence
    // per pointer, so an allocator reusing an address cannot make a stale
    // retirement token match an unrelated provider.
    static thread_local wxAuiTabArtAdoption next = 0;
    if ( ++next == 0 )
        ++next;
    return next;
}

class wxAuiTabArtLease final
{
public:
    explicit wxAuiTabArtLease(wxAuiTabArt* const art)
        : m_art(art)
    {
        if ( m_art )
            ++wxAuiGetTabArtLifetimes()[m_art].leaseDepth;
    }

    ~wxAuiTabArtLease()
    {
        if ( !m_art )
            return;

        auto& lifetimes = wxAuiGetTabArtLifetimes();
        const auto it = lifetimes.find(m_art);
        if ( it == lifetimes.end() )
        {
            wxFAIL_MSG("missing wxAuiTabArt lease state");
            return;
        }
        if ( it->second.leaseDepth == 0 )
        {
            wxFAIL_MSG("unbalanced wxAuiTabArt lease");
            return;
        }

        if ( --it->second.leaseDepth != 0 )
            return;

        const bool retired = it->second.retired;
        if ( retired || it->second.adoption == 0 )
            lifetimes.erase(it);
        if ( retired )
            delete m_art;
    }

private:
    wxAuiTabArt* const m_art;

    wxDECLARE_NO_COPY_CLASS(wxAuiTabArtLease);
};

wxAuiTabArtAdoption wxAuiAdoptTabArt(
    wxAuiTabArt* const art,
    const wxAuiTabContainer* const owner = nullptr)
{
    if ( !art )
        return 0;

    wxAuiTabArtLifetime& lifetime = wxAuiGetTabArtLifetimes()[art];
    lifetime.adoption = wxAuiNextTabArtAdoption();
    lifetime.retired = false;
    lifetime.owner = owner;
    return lifetime.adoption;
}

wxAuiTabArtAdoption wxAuiGetTabArtAdoption(wxAuiTabArt* const art)
{
    if ( !art )
        return 0;

    const auto& lifetimes = wxAuiGetTabArtLifetimes();
    const auto it = lifetimes.find(art);
    return it == lifetimes.end() ? 0 : it->second.adoption;
}

bool wxAuiHasLiveTabArtAdoption(
    wxAuiTabArt* const art,
    const wxAuiTabArtAdoption adoption,
    const wxAuiTabContainer* const owner)
{
    if ( !art )
        return adoption == 0;

    const auto& lifetimes = wxAuiGetTabArtLifetimes();
    const auto it = lifetimes.find(art);
    return it != lifetimes.end() && !it->second.retired &&
           it->second.adoption == adoption && it->second.owner == owner;
}

bool wxAuiIsTabArtOwnedBy(wxAuiTabArt* const art,
                          const wxAuiTabContainer* const owner)
{
    if ( !art )
        return false;

    const auto& lifetimes = wxAuiGetTabArtLifetimes();
    const auto it = lifetimes.find(art);
    return it != lifetimes.end() && !it->second.retired &&
           it->second.owner == owner;
}

void wxAuiRetireTabArt(wxAuiTabArt* const art,
                       const wxAuiTabArtAdoption adoption)
{
    if ( !art )
        return;

    auto& lifetimes = wxAuiGetTabArtLifetimes();
    const auto it = lifetimes.find(art);
    if ( it == lifetimes.end() )
    {
        // Providers installed before this sidecar existed (notably the
        // constructor default) have adoption zero until explicitly adopted.
        if ( adoption == 0 )
            delete art;
        return;
    }

    // A callback may re-adopt a provider which an outer writer intended to
    // retire (A -> B -> A). That later publication owns it now, so the stale
    // retirement must not poison the new installation.
    if ( it->second.adoption != adoption || it->second.retired )
        return;

    // Reentrant replacement can encounter an already retired provider. The
    // first retirement owns its eventual deletion.
    it->second.retired = true;
    if ( it->second.leaseDepth == 0 )
    {
        lifetimes.erase(it);
        delete art;
    }
}

// Own a newly produced provider across callback-capable initialization. The
// staging adoption is deliberately not "released" when the provider is
// passed to SetArtProvider(): that publication adopts it again, making this
// token stale. If publication never happens (including because a setter
// throws), the staging token still owns and retires the provider.
class wxAuiTabArtStaging final
{
public:
    explicit wxAuiTabArtStaging(wxAuiTabArt* const art)
        : m_art(art)
    {
        if ( !m_art )
            return;

        try
        {
            m_adoption = wxAuiAdoptTabArt(m_art);
        }
        catch ( ... )
        {
            delete m_art;
            m_art = nullptr;
            throw;
        }
    }

    wxAuiTabArtStaging(wxAuiTabArtStaging&& other) noexcept
        : m_art(other.m_art), m_adoption(other.m_adoption)
    {
        other.m_art = nullptr;
        other.m_adoption = 0;
    }

    ~wxAuiTabArtStaging()
    {
        wxAuiRetireTabArt(m_art, m_adoption);
    }

    wxAuiTabArt* Get() const { return m_art; }

    // This is a sidecar lookup only: m_art may already have been deleted by a
    // lease after a virtual setter transferred it to another container.
    bool StillOwnsArt() const
    {
        return m_art &&
               wxAuiHasLiveTabArtAdoption(m_art, m_adoption, nullptr);
    }

private:
    wxAuiTabArt* m_art;
    wxAuiTabArtAdoption m_adoption = 0;

    wxDECLARE_NO_COPY_CLASS(wxAuiTabArtStaging);
};

using wxAuiTabContainerLifetime = unsigned long long;
using wxAuiTabContainerRevision = unsigned long long;

struct wxAuiTabContainerLifetimeState
{
    wxAuiTabContainerLifetime lifetime = 0;
    wxAuiTabContainerRevision revision = 0;
};

std::unordered_map<const wxAuiTabContainer*,
                   wxAuiTabContainerLifetimeState>&
wxAuiGetTabContainerLifetimes()
{
    static thread_local std::unordered_map<
        const wxAuiTabContainer*, wxAuiTabContainerLifetimeState> lifetimes;
    return lifetimes;
}

wxAuiTabContainerLifetime wxAuiNextTabContainerLifetime()
{
    static thread_local wxAuiTabContainerLifetime next = 0;
    if ( ++next == 0 )
        ++next;
    return next;
}

void wxAuiRegisterTabContainer(const wxAuiTabContainer* const container)
{
    wxAuiTabContainerLifetimeState& state =
        wxAuiGetTabContainerLifetimes()[container];
    state.lifetime = wxAuiNextTabContainerLifetime();
    state.revision = 1;
}

void wxAuiForgetTabContainer(const wxAuiTabContainer* const container)
{
    wxAuiGetTabContainerLifetimes().erase(container);
}

void wxAuiBumpTabContainerRevision(
    const wxAuiTabContainer* const container)
{
    auto& lifetimes = wxAuiGetTabContainerLifetimes();
    const auto it = lifetimes.find(container);
    if ( it == lifetimes.end() )
        return;

    if ( ++it->second.revision == 0 )
        ++it->second.revision;
}

bool wxAuiGetTabContainerLifetimeState(
    const wxAuiTabContainer* const container,
    wxAuiTabContainerLifetimeState* const state)
{
    const auto& lifetimes = wxAuiGetTabContainerLifetimes();
    const auto it = lifetimes.find(container);
    if ( it == lifetimes.end() )
        return false;

    if ( state )
        *state = it->second;
    return true;
}

bool wxAuiHasExactTabContainerLifetime(
    const wxAuiTabContainer* const container,
    const wxAuiTabContainerLifetimeState& expected)
{
    wxAuiTabContainerLifetimeState current;
    return wxAuiGetTabContainerLifetimeState(container, &current) &&
           current.lifetime == expected.lifetime &&
           current.revision == expected.revision;
}

template <typename F>
auto wxAuiInvokeTabArt(wxAuiTabArt* const art, F&& fn)
    -> decltype(fn(art))
{
    wxAuiTabArtLease lease(art);
    return fn(art);
}

void wxAuiSetTabArtSizingInfo(wxAuiTabArt* const art,
                              const wxSize& size,
                              const size_t pageCount,
                              wxWindow* const window)
{
    if ( !art )
        return;

    wxAuiInvokeTabArt(
        art, [&size, pageCount, window](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSizingInfo(size, pageCount, window);
        });
}

// DrawPageTab() is specified to publish geometry only. Invoke it with a page
// copy and reject a provider which changes the copied identity/state before
// indexing its output vectors.
bool wxAuiHasSamePagePayloadExceptGeometry(
    const wxAuiNotebookPage& expected,
    const wxAuiNotebookPage& rendered)
{
    if ( rendered.window != expected.window ||
            rendered.caption != expected.caption ||
            rendered.tooltip != expected.tooltip ||
            !rendered.bitmap.IsSameAs(expected.bitmap) ||
            rendered.kind != expected.kind ||
            rendered.active != expected.active ||
            rendered.hover != expected.hover ||
            rendered.rowEnd != expected.rowEnd ||
            rendered.buttons.size() != expected.buttons.size() )
    {
        return false;
    }

    for ( size_t i = 0; i < expected.buttons.size(); ++i )
    {
        const wxAuiTabContainerButton& lhs = expected.buttons[i];
        const wxAuiTabContainerButton& rhs = rendered.buttons[i];
        if ( rhs.id != lhs.id || rhs.curState != lhs.curState ||
                rhs.location != lhs.location ||
                !rhs.bitmap.IsSameAs(lhs.bitmap) ||
                !rhs.disBitmap.IsSameAs(lhs.disBitmap) )
        {
            return false;
        }
    }
    return true;
}

bool wxAuiHasSamePagePayload(const wxAuiNotebookPage& expected,
                             const wxAuiNotebookPage& actual)
{
    if ( !wxAuiHasSamePagePayloadExceptGeometry(expected, actual) ||
            actual.rect != expected.rect )
    {
        return false;
    }

    for ( size_t i = 0; i < expected.buttons.size(); ++i )
    {
        if ( actual.buttons[i].rect != expected.buttons[i].rect )
            return false;
    }
    return true;
}

bool wxAuiTryCopyNotebookPage(const wxAuiNotebookPage& source,
                              wxAuiNotebookPage* const copy) noexcept
{
    try
    {
        *copy = source;
        return true;
    }
    catch ( const std::bad_alloc& )
    {
        // Paint/layout entry points have no useful way to report allocation
        // failure. Preserve the current committed geometry and try again on
        // the next invalidation instead of unwinding through event dispatch.
        return false;
    }
}

// A tab container doesn't own its page windows. A custom art callback can
// therefore destroy one of them without going through any container mutator
// (and without bumping the container revision). Capture every raw identity
// and its weak parent witness once, before the first virtual call, and only
// dereference a raw pointer after its matching weak reference has resolved.
class wxAuiPageWindowProjection final
{
public:
    bool Capture(const wxAuiNotebookPageArray& pages) noexcept
    {
        m_pages.clear();
        try
        {
            m_pages.reserve(pages.GetCount());
            for ( const wxAuiNotebookPage& page : pages )
            {
                if ( page.window &&
                        wxWindowIsUnavailableForCallbacks(page.window) )
                {
                    m_pages.clear();
                    return false;
                }

                wxWindow* const parent =
                    page.window ? page.window->GetParent() : nullptr;
                m_pages.emplace_back(page.window, parent);
            }
        }
        catch ( const std::bad_alloc& )
        {
            m_pages.clear();
            return false;
        }

        return true;
    }

    bool Matches(const wxAuiNotebookPageArray& pages,
                 const size_t index) const
    {
        if ( pages.GetCount() != m_pages.size() ||
                index >= m_pages.size() )
        {
            return false;
        }

        const Page& expected = m_pages[index];
        wxWindow* const window = expected.windowLifetime.get();
        if ( window != expected.window )
            return false;

        // Only the weakly resolved pointer may be dereferenced here.
        if ( window &&
                (wxWindowIsUnavailableForCallbacks(window) ||
                 window->GetParent() != expected.parent) )
        {
            return false;
        }

        wxWindow* const parent = expected.parentLifetime.get();
        if ( parent != expected.parent ||
                (parent && wxWindowIsUnavailableForCallbacks(parent)) )
        {
            return false;
        }

        return pages[index].window == expected.window;
    }

    bool MatchesAll(const wxAuiNotebookPageArray& pages) const
    {
        if ( pages.GetCount() != m_pages.size() )
            return false;

        for ( size_t i = 0; i < m_pages.size(); ++i )
        {
            if ( !Matches(pages, i) )
                return false;
        }
        return true;
    }

private:
    struct Page
    {
        Page(wxWindow* const pageWindow, wxWindow* const pageParent)
            : window(pageWindow),
              windowLifetime(pageWindow),
              parent(pageParent),
              parentLifetime(pageParent)
        {
        }

        wxWindow* window;
        wxWeakRef<wxWindow> windowLifetime;
        wxWindow* parent;
        wxWeakRef<wxWindow> parentLifetime;
    };

    std::vector<Page> m_pages;
};

class wxAuiTabContainerSnapshot final
{
public:
    wxAuiTabContainerSnapshot(
        const wxAuiTabContainer* const container,
        wxAuiTabArt* const art,
        const wxRect& rect,
        const size_t tabOffset,
        const unsigned int flags,
        const int rowHeight,
        const wxAuiNotebookPageArray& pages,
        const std::vector<wxAuiTabContainerButton>& buttons,
        wxWindow* const window)
        : m_container(container),
          m_art(art),
          m_artAdoption(wxAuiGetTabArtAdoption(art)),
          m_rect(rect),
          m_tabOffset(tabOffset),
          m_flags(flags),
          m_rowHeight(rowHeight),
          m_pageCount(pages.GetCount()),
          m_buttonCount(buttons.size()),
          m_window(window),
          m_windowLifetime(window),
          m_windowParent(window ? window->GetParent() : nullptr),
          m_windowParentLifetime(m_windowParent)
    {
        wxAuiGetTabContainerLifetimeState(container, &m_lifetime);
    }

    // This check never dereferences the container. Call it before reading any
    // member after a virtual art callback.
    bool IsAlive() const
    {
        if ( !wxAuiHasExactTabContainerLifetime(m_container, m_lifetime) ||
                !wxAuiHasLiveTabArtAdoption(
                    m_art, m_artAdoption, m_container) )
        {
            return false;
        }

        if ( m_window )
        {
            if ( m_windowLifetime.get() != m_window ||
                    wxWindowIsUnavailableForCallbacks(m_window) ||
                    m_window->GetParent() != m_windowParent )
            {
                return false;
            }
            if ( m_windowParent &&
                    (m_windowParentLifetime.get() != m_windowParent ||
                     wxWindowIsUnavailableForCallbacks(m_windowParent)) )
            {
                return false;
            }
        }
        return true;
    }

    // IsAlive() must have succeeded before calling this function.
    bool Matches(const wxAuiTabArt* const art,
                 const wxRect& rect,
                 const size_t tabOffset,
                 const unsigned int flags,
                 const int rowHeight,
                 const wxAuiNotebookPageArray& pages,
                 const std::vector<wxAuiTabContainerButton>& buttons) const
    {
        if ( art != m_art || rect != m_rect || tabOffset != m_tabOffset ||
                flags != m_flags || rowHeight != m_rowHeight ||
                pages.GetCount() != m_pageCount ||
                buttons.size() != m_buttonCount )
        {
            return false;
        }
        return true;
    }

    wxAuiTabArt* GetArt() const { return m_art; }
    wxAuiTabArtAdoption GetArtAdoption() const { return m_artAdoption; }

private:
    const wxAuiTabContainer* const m_container;
    wxAuiTabContainerLifetimeState m_lifetime;
    wxAuiTabArt* const m_art;
    const wxAuiTabArtAdoption m_artAdoption;
    const wxRect m_rect;
    const size_t m_tabOffset;
    const unsigned int m_flags;
    const int m_rowHeight;
    const size_t m_pageCount;
    const size_t m_buttonCount;
    wxWindow* const m_window;
    const wxWeakRef<wxWindow> m_windowLifetime;
    wxWindow* const m_windowParent;
    const wxWeakRef<wxWindow> m_windowParentLifetime;
};

// Keep the sizing-input generation out of wxAuiNotebook itself: adding a
// member to this public class would change its ABI. GUI windows are confined
// to their creating thread, so a thread-local sidecar gives each live notebook
// an ABA-safe latest-writer token without synchronization or public layout
// changes.
using wxAuiNotebookMutationEpoch = unsigned long long;
using wxAuiNotebookHeightInputEpoch = wxAuiNotebookMutationEpoch;
using wxAuiNotebookBitmapEpoch = wxAuiNotebookMutationEpoch;
using wxAuiNotebookArtEpoch = wxAuiNotebookMutationEpoch;
using wxAuiNotebookStyleEpoch = wxAuiNotebookMutationEpoch;
using wxAuiNotebookFontEpoch = wxAuiNotebookMutationEpoch;

struct wxAuiNotebookMutationEpochs
{
    wxAuiNotebookHeightInputEpoch heightInput = 0;
    std::unordered_map<const wxWindow*, wxAuiNotebookBitmapEpoch>
        bitmapByPage;
    wxAuiNotebookArtEpoch art = 0;
    wxAuiNotebookStyleEpoch style = 0;
    wxAuiNotebookFontEpoch font = 0;
    wxAuiNotebookFontEpoch normalFont = 0;
    wxAuiNotebookFontEpoch selectedFont = 0;
    wxAuiNotebookFontEpoch measuringFont = 0;
};

std::unordered_map<const wxAuiNotebook*, wxAuiNotebookMutationEpochs>&
wxAuiGetNotebookMutationEpochs()
{
    static thread_local std::unordered_map<
        const wxAuiNotebook*, wxAuiNotebookMutationEpochs> epochs;
    return epochs;
}

wxAuiNotebookHeightInputEpoch
wxAuiGetNotebookHeightInputEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.heightInput;
}

wxAuiNotebookMutationEpoch
wxAuiBumpEpoch(wxAuiNotebookMutationEpoch& epoch)
{
    if ( ++epoch == 0 )
        ++epoch;
    return epoch;
}

wxAuiNotebookHeightInputEpoch
wxAuiBumpNotebookHeightInputEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(
        wxAuiGetNotebookMutationEpochs()[book].heightInput);
}

wxAuiNotebookBitmapEpoch
wxAuiGetNotebookBitmapEpoch(const wxAuiNotebook* const book,
                            const wxWindow* const page)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    if ( it == epochs.end() )
        return 0;
    const auto pageIt = it->second.bitmapByPage.find(page);
    return pageIt == it->second.bitmapByPage.end() ? 0 : pageIt->second;
}

wxAuiNotebookBitmapEpoch
wxAuiBumpNotebookBitmapEpoch(const wxAuiNotebook* const book,
                             const wxWindow* const page)
{
    return wxAuiBumpEpoch(
        wxAuiGetNotebookMutationEpochs()[book].bitmapByPage[page]);
}

wxAuiNotebookArtEpoch
wxAuiGetNotebookArtEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.art;
}

wxAuiNotebookArtEpoch
wxAuiBumpNotebookArtEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(wxAuiGetNotebookMutationEpochs()[book].art);
}

wxAuiNotebookStyleEpoch
wxAuiGetNotebookStyleEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.style;
}

wxAuiNotebookStyleEpoch
wxAuiBumpNotebookStyleEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(wxAuiGetNotebookMutationEpochs()[book].style);
}

wxAuiNotebookFontEpoch
wxAuiGetNotebookFontEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.font;
}

wxAuiNotebookFontEpoch
wxAuiBumpNotebookFontEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(wxAuiGetNotebookMutationEpochs()[book].font);
}

wxAuiNotebookFontEpoch
wxAuiGetNotebookNormalFontEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.normalFont;
}

wxAuiNotebookFontEpoch
wxAuiBumpNotebookNormalFontEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(
        wxAuiGetNotebookMutationEpochs()[book].normalFont);
}

wxAuiNotebookFontEpoch
wxAuiGetNotebookSelectedFontEpoch(const wxAuiNotebook* const book)
{
    const auto& epochs = wxAuiGetNotebookMutationEpochs();
    const auto it = epochs.find(book);
    return it == epochs.end() ? 0 : it->second.selectedFont;
}

wxAuiNotebookFontEpoch
wxAuiBumpNotebookSelectedFontEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(
        wxAuiGetNotebookMutationEpochs()[book].selectedFont);
}

wxAuiNotebookFontEpoch
wxAuiBumpNotebookMeasuringFontEpoch(const wxAuiNotebook* const book)
{
    return wxAuiBumpEpoch(
        wxAuiGetNotebookMutationEpochs()[book].measuringFont);
}

void wxAuiForgetNotebookMutationEpochs(const wxAuiNotebook* const book)
{
    wxAuiGetNotebookMutationEpochs().erase(book);
}

// Mouse hit-testing is the only non-deterministic input to OnTabEndDrag().
// Tests install this narrow, synchronous override and still execute the real
// drag transaction and all of its callbacks.
struct wxAuiNotebookEndDragOverride
{
    wxAuiNotebookEndDragOverride(wxAuiNotebook* const book_,
                                 wxAuiTabCtrl* const sourceTabs_,
                                 wxAuiTabCtrl* const destinationTabs_,
                                 const int destinationIndex_,
                                 const bool createNewPane_)
        : book(book_),
          bookLifetime(book_),
          sourceTabs(sourceTabs_),
          sourceTabsLifetime(sourceTabs_),
          destinationTabs(destinationTabs_),
          destinationTabsLifetime(destinationTabs_),
          destinationParent(destinationTabs_
                                ? destinationTabs_->GetParent()
                                : nullptr),
          destinationParentLifetime(destinationParent),
          destinationIndex(destinationIndex_),
          createNewPane(createNewPane_)
    {
    }

    wxAuiNotebook* book;
    wxWeakRef<wxAuiNotebook> bookLifetime;
    wxAuiTabCtrl* sourceTabs;
    wxWeakRef<wxAuiTabCtrl> sourceTabsLifetime;
    wxAuiTabCtrl* destinationTabs;
    wxWeakRef<wxAuiTabCtrl> destinationTabsLifetime;
    wxWindow* destinationParent;
    wxWeakRef<wxWindow> destinationParentLifetime;
    int destinationIndex;
    bool createNewPane;
};

thread_local const wxAuiNotebookEndDragOverride*
    gs_auiNotebookEndDragOverride = nullptr;

class wxAuiNotebookTopologyTransaction final
{
public:
    // A transaction normally rejects every nested topology writer for the
    // same notebook. Some compound operations still have to enter one of the
    // already-transactional primitive writers. This narrow scope lets that
    // primitive pass its initial guard; as soon as it creates its own
    // transaction, callbacks are protected by the inner transaction again.
    class InternalCall final
    {
    public:
        explicit InternalCall(wxAuiNotebookTopologyTransaction& transaction)
            : m_transaction(transaction)
        {
            ++m_transaction.m_internalCallDepth;
        }

        ~InternalCall()
        {
            wxASSERT(m_transaction.m_internalCallDepth != 0);
            --m_transaction.m_internalCallDepth;
        }

    private:
        wxAuiNotebookTopologyTransaction& m_transaction;

        wxDECLARE_NO_COPY_CLASS(InternalCall);
    };

    explicit wxAuiNotebookTopologyTransaction(wxAuiNotebook* const book)
        : m_book(book),
          m_lifetime(book),
          m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxAuiNotebookTopologyTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(const wxAuiNotebook* const book)
    {
        for ( wxAuiNotebookTopologyTransaction* transaction = GetActive();
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book &&
                    transaction->m_internalCallDepth == 0 )
            {
                return true;
            }
        }
        return false;
    }

private:
    static wxAuiNotebookTopologyTransaction*& GetActive()
    {
        static thread_local wxAuiNotebookTopologyTransaction* active = nullptr;
        return active;
    }

    wxAuiNotebook* const m_book;
    const wxWeakRef<wxAuiNotebook> m_lifetime;
    wxAuiNotebookTopologyTransaction* const m_previous;
    unsigned int m_internalCallDepth = 0;
};

// A cross-notebook drag removes the source projection before publishing the
// destination projection. Keep its exact tab control managed across that
// narrow interval so a rejected destination can be rolled back to the same
// visual slot instead of recreating it in whichever pane happens to be active.
class wxAuiNotebookPreserveTabCtrl final
{
public:
    wxAuiNotebookPreserveTabCtrl(wxAuiNotebook* const book,
                                 wxAuiTabCtrl* const tabs)
        : m_book(book), m_tabs(tabs), m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxAuiNotebookPreserveTabCtrl()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(const wxAuiNotebook* const book,
                            const wxAuiTabCtrl* const tabs)
    {
        for ( wxAuiNotebookPreserveTabCtrl* guard = GetActive(); guard;
              guard = guard->m_previous )
        {
            if ( guard->m_book == book && guard->m_tabs == tabs )
                return true;
        }
        return false;
    }

private:
    static wxAuiNotebookPreserveTabCtrl*& GetActive()
    {
        static thread_local wxAuiNotebookPreserveTabCtrl* active = nullptr;
        return active;
    }

    wxAuiNotebook* const m_book;
    wxAuiTabCtrl* const m_tabs;
    wxAuiNotebookPreserveTabCtrl* const m_previous;

    wxDECLARE_NO_COPY_CLASS(wxAuiNotebookPreserveTabCtrl);
};

// wxWindowUpdateLocker retains a raw pointer and would call Thaw() after an
// art/layout/selection callback had destroyed the notebook. Keep the same
// one-level Freeze()/Thaw() contract, but make the release weak-aware. A
// destroy-scheduled window is deliberately left frozen for its teardown.
class wxAuiNotebookUpdateLocker final
{
public:
    explicit wxAuiNotebookUpdateLocker(wxAuiNotebook* const book)
        : m_book(book)
    {
        book->Freeze();
    }

    ~wxAuiNotebookUpdateLocker()
    {
        wxAuiNotebook* const book = m_book.get();
        if ( book && !wxWindowIsUnavailableForCallbacks(book) &&
                book->IsFrozen() )
        {
            book->Thaw();
        }
    }

private:
    wxWeakRef<wxAuiNotebook> m_book;

    wxDECLARE_NO_COPY_CLASS(wxAuiNotebookUpdateLocker);
};

// Selection can recurse from focus, layout and page-changing callbacks. Keep
// this state outside wxAuiNotebook to preserve its ABI. Each accepted request
// becomes the latest writer for its synchronous chain, while a chain-wide
// budget prevents alternating Show()/focus handlers from overflowing the
// stack forever.
class wxAuiNotebookSelectionTransaction final
{
public:
    explicit wxAuiNotebookSelectionTransaction(wxAuiNotebook* const book)
        : m_book(book),
          m_lifetime(book),
          m_previous(GetActive()),
          m_state(&m_ownState)
    {
        for ( wxAuiNotebookSelectionTransaction* transaction = m_previous;
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book )
            {
                m_state = transaction->m_state;
                break;
            }
        }

        if ( !m_state->sealed &&
                m_state->acceptedRequests < MaxAcceptedRequests )
        {
            ++m_state->acceptedRequests;
            m_epoch = ++m_state->latest;
            m_accepted = true;
        }
        else
        {
            m_state->sealed = true;
            m_epoch = m_state->latest;
        }

        GetActive() = this;
    }

    ~wxAuiNotebookSelectionTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    bool IsAccepted() const { return m_accepted; }

    bool IsLatest() const
    {
        return m_lifetime.get() == m_book && m_accepted &&
               m_epoch == m_state->latest;
    }

private:
    static constexpr unsigned int MaxAcceptedRequests = 64;

    struct State
    {
        unsigned long long latest = 0;
        unsigned int acceptedRequests = 0;
        bool sealed = false;
    };

    static wxAuiNotebookSelectionTransaction*& GetActive()
    {
        static thread_local wxAuiNotebookSelectionTransaction* active = nullptr;
        return active;
    }

    wxAuiNotebook* const m_book;
    const wxWeakRef<wxAuiNotebook> m_lifetime;
    wxAuiNotebookSelectionTransaction* const m_previous;
    State m_ownState;
    State* m_state;
    unsigned long long m_epoch = 0;
    bool m_accepted = false;

    wxDECLARE_NO_COPY_CLASS(wxAuiNotebookSelectionTransaction);
};

// An exact, ABA-safe view of a tab-container projection. Keeping this helper
// local preserves the public ABI while making callback boundaries use the
// same identity/order/parent contract everywhere.
struct wxAuiPageProjectionEntry
{
    explicit wxAuiPageProjectionEntry(wxWindow* const page_)
        : page(page_), lifetime(page_)
    {
    }

    wxWindow* page;
    wxWeakRef<wxWindow> lifetime;
};

using wxAuiPageProjection = std::vector<wxAuiPageProjectionEntry>;

bool wxAuiCapturePageProjection(const wxAuiTabContainer& tabs,
                                wxAuiNotebook* const book,
                                wxAuiPageProjection& projection)
{
    projection.clear();
    projection.reserve(tabs.GetPageCount());
    for ( const wxAuiNotebookPage& info : tabs.GetPages() )
    {
        wxWindow* const page = info.window;
        if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
                page->GetParent() != book )
        {
            projection.clear();
            return false;
        }
        projection.emplace_back(page);
    }
    return true;
}

bool wxAuiMatchesPageProjection(const wxAuiTabContainer& tabs,
                                wxAuiNotebook* const book,
                                const wxAuiPageProjection& projection)
{
    if ( tabs.GetPageCount() != projection.size() )
        return false;
    for ( size_t i = 0; i < projection.size(); ++i )
    {
        const wxAuiPageProjectionEntry& expected = projection[i];
        if ( expected.lifetime.get() != expected.page ||
                wxWindowIsUnavailableForCallbacks(expected.page) ||
                expected.page->GetParent() != book ||
                tabs.GetWindowFromIdx(i) != expected.page )
        {
            return false;
        }
    }
    return true;
}

// Build the vector of buttons for the page depending on the notebook flags.
std::vector<wxAuiTabContainerButton> MakePageButtons(unsigned int flags)
{
    std::vector<wxAuiTabContainerButton> buttons;

    // Pin/unpin button can be only shown if one of the styles enabling it is
    // used and depends on the current tab state, so make it hidden by default.
    if (flags & (wxAUI_NB_PIN_ON_ACTIVE_TAB | wxAUI_NB_UNPIN_ON_ALL_PINNED))
        buttons.push_back({wxAUI_BUTTON_PIN, wxRIGHT, wxAUI_BUTTON_STATE_HIDDEN});

    // Close button is hidden by default, it will be shown depending on the
    // exact style used and, for wxAUI_NB_CLOSE_ON_ACTIVE_TAB, on whether the
    // tab is current.
    if (flags & (wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB))
        buttons.push_back({wxAUI_BUTTON_CLOSE, wxRIGHT, wxAUI_BUTTON_STATE_HIDDEN});

    return buttons;
}

} // anonymous namespace


// wxAuiTabEventSource
//
// This class is the "attorney" in "attorney-client" idiom used to allow
// wxAuiTabCtrl to call some (but not all) wxAuiNotebook private functions.
class wxAuiTabEventSource
{
private:
    static void TabClicked(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabClicked(ctrl, tabIdx);
    }

    static void TabBeginDrag(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabBeginDrag(ctrl, tabIdx);
    }

    static void TabDragMotion(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabDragMotion(ctrl, tabIdx);
    }

    static void TabEndDrag(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabEndDrag(ctrl, tabIdx);
    }

    static void TabCancelDrag(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabCancelDrag(ctrl, tabIdx);
    }

    static void TabButton(wxAuiTabCtrl* ctrl, int tabIdx, int button_id)
    {
        ctrl->GetBook()->OnTabButton(ctrl, tabIdx, button_id);
    }

    static void TabMiddleDown(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabMiddleDown(ctrl, tabIdx);
    }

    static void TabMiddleUp(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabMiddleUp(ctrl, tabIdx);
    }

    static void TabRightDown(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabRightDown(ctrl, tabIdx);
    }

    static void TabRightUp(wxAuiTabCtrl* ctrl, int tabIdx)
    {
        ctrl->GetBook()->OnTabRightUp(ctrl, tabIdx);
    }

    static void TabBgDClick(wxAuiTabCtrl* ctrl)
    {
        ctrl->GetBook()->OnTabBgDClick(ctrl);
    }

    friend class wxAuiTabCtrl;
};

// -- wxAuiTabContainer class implementation --


// wxAuiTabContainer is a class which contains information about each
// tab.  It also can render an entire tab control to a specified DC.
// It's not a window class itself, because this code will be used by
// the wxAuiManager, where it is disadvantageous to have separate
// windows for each tab control in the case of "docked tabs"

// A derived class, wxAuiTabCtrl, is an actual wxWindow-derived window
// which can be used as a tab control in the normal sense.


wxAuiTabContainer::wxAuiTabContainer()
{
    m_tabOffset = 0;
    m_flags = 0;
    m_tabRowHeight = 0;
    m_art = nullptr;

    wxAuiRegisterTabContainer(this);
    wxScopeGuard forgetContainer = wxMakeGuard([this]()
    {
        wxAuiForgetTabContainer(this);
    });

    wxAuiTabArtStaging defaultArt(new wxAuiDefaultTabArt);
    m_art = defaultArt.Get();
    const wxAuiTabArtAdoption defaultAdoption =
        wxAuiAdoptTabArt(m_art, this);
    wxScopeGuard retireDefaultArt = wxMakeGuard(
        [this, defaultAdoption]()
        {
            wxAuiRetireTabArt(m_art, defaultAdoption);
        });

    AddButton(wxAUI_BUTTON_LEFT, wxLEFT);
    AddButton(wxAUI_BUTTON_RIGHT, wxRIGHT);
    AddButton(wxAUI_BUTTON_WINDOWLIST, wxRIGHT);
    AddButton(wxAUI_BUTTON_CLOSE, wxRIGHT);

    retireDefaultArt.Dismiss();
    forgetContainer.Dismiss();
}

wxAuiTabContainer::~wxAuiTabContainer()
{
    // Publish the empty state first so destructor-time callbacks cannot see a
    // provider which has already entered retirement.
    wxAuiTabArt* const art = m_art;
    const wxAuiTabArtAdoption adoption = wxAuiGetTabArtAdoption(art);
    m_art = nullptr;
    wxAuiForgetTabContainer(this);
    wxAuiRetireTabArt(art, adoption);
}

void wxAuiTabContainer::SetArtProvider(wxAuiTabArt* art)
{
    if ( art == m_art )
    {
        if ( art )
        {
            wxAuiTabContainerLifetimeState lifetime;
            if ( !wxAuiGetTabContainerLifetimeState(this, &lifetime) )
                return;
            wxAuiAdoptTabArt(art, this);
            wxAuiBumpTabContainerRevision(this);
            const unsigned int flags = m_flags;
            wxScopeGuard validatePublishedArt = wxMakeGuard(
                [this, art, lifetime]()
                {
                    wxAuiTabContainerLifetimeState current;
                    if ( wxAuiGetTabContainerLifetimeState(this, &current) &&
                            current.lifetime == lifetime.lifetime &&
                            m_art == art &&
                            !wxAuiIsTabArtOwnedBy(art, this) )
                    {
                        m_art = nullptr;
                        wxAuiBumpTabContainerRevision(this);
                    }
                });
            wxUnusedVar(validatePublishedArt);
            wxAuiInvokeTabArt(art, [flags](wxAuiTabArt* const leasedArt)
            {
                leasedArt->SetFlags(flags);
            });
        }
        return;
    }

    wxAuiTabArt* const oldArt = m_art;
    const wxAuiTabArtAdoption oldAdoption =
        wxAuiGetTabArtAdoption(oldArt);
    const unsigned int flags = m_flags;

    // Stage the incoming raw ownership first. A second adoption publishes it
    // to this container; if that adoption throws, staging retires it and the
    // old provider remains installed.
    wxAuiTabArtStaging incomingArt(art);
    wxAuiTabContainerLifetimeState lifetime;
    if ( !wxAuiGetTabContainerLifetimeState(this, &lifetime) )
        return;
    wxAuiAdoptTabArt(art, this);

    // The new provider is the latest writer before entering application code.
    // Its lease also makes deleting the entire container from SetFlags() safe.
    m_art = art;
    wxAuiBumpTabContainerRevision(this);

    // This guard must survive SetFlags(): exceptions are propagated and the
    // old provider is normally retired. If the callback transfers the new
    // provider away and retires it, restore the still-owned old provider
    // instead. Its adoption token also makes stale A -> B -> A retirements
    // harmless.
    bool retainOldArt = false;
    wxScopeGuard retireOldArt = wxMakeGuard(
        [oldArt, oldAdoption, &retainOldArt]()
        {
            if ( !retainOldArt )
                wxAuiRetireTabArt(oldArt, oldAdoption);
        });
    wxUnusedVar(retireOldArt);
    wxScopeGuard validatePublishedArt = wxMakeGuard(
        [this, art, oldArt, oldAdoption, lifetime, &retainOldArt]()
        {
            wxAuiTabContainerLifetimeState current;
            if ( wxAuiGetTabContainerLifetimeState(this, &current) &&
                    current.lifetime == lifetime.lifetime && art &&
                    m_art == art &&
                    !wxAuiIsTabArtOwnedBy(art, this) )
            {
                if ( wxAuiHasLiveTabArtAdoption(
                        oldArt, oldAdoption, this) )
                {
                    m_art = oldArt;
                    retainOldArt = true;
                }
                else
                    m_art = nullptr;
                wxAuiBumpTabContainerRevision(this);
            }
        });
    wxUnusedVar(validatePublishedArt);
    if ( art )
    {
        wxAuiInvokeTabArt(art, [flags](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetFlags(flags);
        });
    }

    // incomingArt's staging retirement is now stale because the container
    // adopted the provider before publication.
}

wxAuiTabArt* wxAuiTabContainer::GetArtProvider() const
{
    return m_art;
}

void wxAuiTabContainer::SetFlags(unsigned int flags)
{
    // Prepare every allocation before publishing the new configuration. If
    // allocation fails, the previous flags/buttons remain a coherent model;
    // once publication starts, all remaining operations are non-throwing.
    std::vector<wxAuiTabContainerButton> buttons;
    if ((flags & wxAUI_NB_SCROLL_BUTTONS) && !(flags & wxAUI_NB_MULTILINE))
    {
        buttons.push_back({wxAUI_BUTTON_LEFT, wxLEFT});
        buttons.push_back({wxAUI_BUTTON_RIGHT, wxRIGHT});
    }

    if (flags & wxAUI_NB_WINDOWLIST_BUTTON)
        buttons.push_back({wxAUI_BUTTON_WINDOWLIST, wxRIGHT});

    if (flags & wxAUI_NB_CLOSE_BUTTON)
        buttons.push_back({wxAUI_BUTTON_CLOSE, wxRIGHT});

    // Also synchronize the tabs buttons with the new settings if any of the
    // flags affecting them changed.
    const auto flagsAffectingButtons =
        wxAUI_NB_PIN_ON_ACTIVE_TAB |
        wxAUI_NB_UNPIN_ON_ALL_PINNED |
        wxAUI_NB_CLOSE_ON_ALL_TABS |
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    const bool replacePageButtons =
        (m_flags & flagsAffectingButtons) !=
            (flags & flagsAffectingButtons);
    std::vector<std::vector<wxAuiTabContainerButton>> pageButtons;
    if ( replacePageButtons )
    {
        pageButtons.reserve(m_pages.GetCount());
        for ( size_t i = 0; i < m_pages.GetCount(); ++i )
            pageButtons.push_back(MakePageButtons(flags));
    }

    // Publish the container state before entering the virtual art provider.
    // If it performs a nested SetFlags(), that newer writer must remain the
    // final value; and if it destroys this container there must be no member
    // access after the callback returns.
    m_buttons.swap(buttons);
    if ( replacePageButtons )
    {
        for ( size_t i = 0; i < m_pages.GetCount(); ++i )
            m_pages[i].buttons = std::move(pageButtons[i]);
    }
    m_flags = flags;
    wxAuiBumpTabContainerRevision(this);

    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [flags](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetFlags(flags);
        });
    }
}

void wxAuiTabContainer::SetNormalFont(const wxFont& font)
{
    wxAuiBumpTabContainerRevision(this);
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [&font](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetNormalFont(font);
        });
    }
}

void wxAuiTabContainer::SetSelectedFont(const wxFont& font)
{
    wxAuiBumpTabContainerRevision(this);
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [&font](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSelectedFont(font);
        });
    }
}

void wxAuiTabContainer::SetMeasuringFont(const wxFont& font)
{
    wxAuiBumpTabContainerRevision(this);
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [&font](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetMeasuringFont(font);
        });
    }
}

void wxAuiTabContainer::SetColour(const wxColour& colour)
{
    wxAuiBumpTabContainerRevision(this);
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [&colour](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetColour(colour);
        });
    }
}

void wxAuiTabContainer::SetActiveColour(const wxColour& colour)
{
    wxAuiBumpTabContainerRevision(this);
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        wxAuiInvokeTabArt(art, [&colour](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetActiveColour(colour);
        });
    }
}

void wxAuiTabContainer::SetRect(const wxRect& rect, wxWindow* wnd)
{
    m_rect = rect;
    wxAuiBumpTabContainerRevision(this);

    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        const size_t pageCount = m_pages.GetCount();
        wxAuiInvokeTabArt(art, [&rect, pageCount, wnd](
                                    wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSizingInfo(rect.GetSize(), pageCount, wnd);
        });
    }
}

void wxAuiTabContainer::SetRowHeight(int rowHeight)
{
    m_tabRowHeight = rowHeight;
    wxAuiBumpTabContainerRevision(this);
}

bool wxAuiTabContainer::AddPage(const wxAuiNotebookPage& info)
{
    return InsertPage(info, m_pages.GetCount());
}

bool wxAuiTabContainer::InsertPage(const wxAuiNotebookPage& info,
                                   size_t idx)
{
    auto buttons = MakePageButtons(m_flags);
    const auto it = m_pages.insert(m_pages.begin() + idx, info);
    it->buttons = std::move(buttons);
    wxAuiBumpTabContainerRevision(this);

    // let the art provider know how many pages we have
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        const wxSize size = m_rect.GetSize();
        const size_t pageCount = m_pages.GetCount();
        wxAuiInvokeTabArt(art, [size, pageCount, &info](
                                    wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSizingInfo(size, pageCount, info.window);
        });
    }

    return true;
}

bool wxAuiTabContainer::MovePage(wxWindow* page,
                                 size_t new_idx)
{
    int idx = GetIdxFromWindow(page);
    if (idx == -1)
        return false;

    return MovePage(static_cast<size_t>(idx), new_idx);
}

bool wxAuiTabContainer::MovePage(size_t old_idx, size_t new_idx)
{
    const auto b = m_pages.begin();
    if (old_idx < new_idx)
        std::rotate(b + old_idx, b + old_idx + 1, b + new_idx + 1);
    else if (old_idx > new_idx)
        std::rotate(b + new_idx, b + old_idx, b + old_idx + 1);
    else // nothing to do
        return false;

    wxAuiBumpTabContainerRevision(this);
    return true;
}

bool wxAuiTabContainer::RemovePage(wxWindow* page)
{
    int idx = GetIdxFromWindow(page);
    if (idx == -1)
        return false;

    RemovePageAt(idx);

    return true;
}

void wxAuiTabContainer::RemovePageAt(size_t idx)
{
    wxCHECK_RET( idx < m_pages.GetCount(), "invalid page index" );

    wxWindow* const wnd = m_pages[idx].window;

    m_pages.RemoveAt(idx);
    wxAuiBumpTabContainerRevision(this);

    // let the art provider know how many pages we have
    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        const wxSize size = m_rect.GetSize();
        const size_t pageCount = m_pages.GetCount();
        wxAuiInvokeTabArt(art, [size, pageCount, wnd](
                                    wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSizingInfo(size, pageCount, wnd);
        });
    }
}

void wxAuiTabContainer::RemoveAll()
{
    m_pages.Clear();
    wxAuiBumpTabContainerRevision(this);
}

bool wxAuiTabContainer::SetActivePage(const wxWindow* wnd)
{
    bool found = false;

    for ( auto& page : m_pages )
    {
        if (page.window == wnd)
        {
            page.active = true;
            found = true;
        }
        else
        {
            page.active = false;
        }
    }

    wxAuiBumpTabContainerRevision(this);
    return found;
}

void wxAuiTabContainer::SetNoneActive()
{
    for ( auto& page : m_pages )
    {
        page.active = false;
    }
    wxAuiBumpTabContainerRevision(this);
}

bool wxAuiTabContainer::SetActivePage(size_t page)
{
    if (page >= m_pages.GetCount())
        return false;

    return SetActivePage(m_pages.Item(page).window);
}

int wxAuiTabContainer::GetActivePage() const
{
    size_t i, page_count = m_pages.GetCount();
    for (i = 0; i < page_count; ++i)
    {
        wxAuiNotebookPage page = m_pages.Item(i);
        if (page.active)
            return i;
    }

    return -1;
}

wxWindow* wxAuiTabContainer::GetWindowFromIdx(size_t idx) const
{
    if (idx >= m_pages.GetCount())
        return nullptr;

    return m_pages[idx].window;
}

int wxAuiTabContainer::GetIdxFromWindow(const wxWindow* wnd) const
{
    const size_t page_count = m_pages.GetCount();
    for ( size_t i = 0; i < page_count; ++i )
    {
        wxAuiNotebookPage& page = m_pages.Item(i);
        if (page.window == wnd)
            return i;
    }
    return wxNOT_FOUND;
}

wxAuiNotebookPage& wxAuiTabContainer::GetPage(size_t idx)
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    // Returning mutable storage is itself a potential write boundary. This
    // makes direct public mutations visible to O(1) callback snapshots.
    wxAuiBumpTabContainerRevision(this);
    return m_pages[idx];
}

const wxAuiNotebookPage& wxAuiTabContainer::GetPage(size_t idx) const
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    return m_pages[idx];
}

const wxAuiNotebookPageArray& wxAuiTabContainer::GetPages() const
{
    return m_pages;
}

size_t wxAuiTabContainer::GetPageCount() const
{
    return m_pages.GetCount();
}

void wxAuiTabContainer::AddButton(int id,
                                  int location,
                                  const wxBitmapBundle& WXUNUSED(normalBitmap),
                                  const wxBitmapBundle& WXUNUSED(disabledBitmap))
{
    // We ignore the bitmaps as they are never used currently.
    m_buttons.push_back({id, location});
    wxAuiBumpTabContainerRevision(this);
}

void wxAuiTabContainer::RemoveButton(int id)
{
    for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        if (it->id == id)
        {
            m_buttons.erase(it);
            wxAuiBumpTabContainerRevision(this);
            return;
        }
    }
}



size_t wxAuiTabContainer::GetTabOffset() const
{
    return m_tabOffset;
}

void wxAuiTabContainer::SetTabOffset(size_t offset)
{
    // It doesn't make sense to set the offset when multiple lines of tabs are
    // used as it is not used in this case.
    wxASSERT( !IsFlagSet(wxAUI_NB_MULTILINE) );

    m_tabOffset = offset;
    wxAuiBumpTabContainerRevision(this);
}


namespace
{

bool wxAuiUpdateButtonsState(unsigned int flags,
                             wxAuiNotebookPage& page,
                             bool forceActive)
{
    const bool isActive = forceActive || page.active;
    bool changed = false;
    for (auto& button : page.buttons)
    {
        const int oldState = button.curState;
        bool shown = false;
        switch ( page.kind )
        {
            case wxAuiTabKind::Normal:
            case wxAuiTabKind::Pinned:
                switch ( button.id )
                {
                    case wxAUI_BUTTON_CLOSE:
                        if (flags & wxAUI_NB_CLOSE_ON_ALL_TABS)
                        {
                            shown = true;
                        }
                        else if (flags & wxAUI_NB_CLOSE_ON_ACTIVE_TAB)
                        {
                            if (isActive)
                                shown = true;
                        }
                        break;

                    case wxAUI_BUTTON_PIN:
                        if (flags & wxAUI_NB_PIN_ON_ACTIVE_TAB)
                        {
                            if (isActive)
                            {
                                shown = true;

                                if ( page.kind == wxAuiTabKind::Pinned )
                                    button.curState |= wxAUI_BUTTON_STATE_CHECKED;
                                else
                                    button.curState &= ~wxAUI_BUTTON_STATE_CHECKED;
                            }
                        }

                        // This is not an "else" of the above "if" as when both
                        // styles are used, we show "pin" button only on the
                        // active tab, but also show "unpin" on all the pinned
                        // tabs.
                        if (flags & wxAUI_NB_UNPIN_ON_ALL_PINNED)
                        {
                            if ( page.kind == wxAuiTabKind::Pinned )
                            {
                                shown = true;

                                button.curState |= wxAUI_BUTTON_STATE_CHECKED;
                            }
                        }
                        break;
                }
                break;

            case wxAuiTabKind::Locked:
                // A locked page can't be closed or pinned, leave the button
                // in its default hidden state.
                break;
        }

        // Leave the other flags unchanged, this doesn't matter when hiding
        // the button but does when showing it as it could be pressed or
        // under the mouse.
        if ( shown )
            button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
        else
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;

        if ( button.curState != oldState )
            changed = true;
    }

    return changed;
}

} // anonymous namespace

void
wxAuiTabContainer::UpdateButtonsState(wxAuiNotebookPage& page, bool forceActive)
{
    if ( wxAuiUpdateButtonsState(m_flags, page, forceActive) )
        wxAuiBumpTabContainerRevision(this);
}

// Combined border between the tabs and the buttons in DIPs.
static const int wxAUI_BUTTONS_BORDER = 2;

int wxAuiTabContainer::GetAvailableForTabs(const wxRect& rect,
                                           wxReadOnlyDC& dc,
                                           wxWindow* wnd)
{
    /*
        Note that this function always ignores LEFT/RIGHT buttons because
        either it's called from LayoutMultiLineTabs() in which case the buttons
        are never used (tabs are multiline XOR scrollable) or it is called from
        RenderButtons() to determine the maximum available width for the tabs,
        which corresponds to the situation when these buttons are hidden.
     */

    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return 0;

    wxAuiTabArt* const art = m_art;
    if ( !art )
        return 0;
    wxAuiTabArtLease artLease(art);
    const auto captureRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, wnd);
    };
    const auto hasExactRevision = [&](const wxAuiTabContainerSnapshot& state)
    {
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };

    size_t i;
    const size_t button_count = m_buttons.size();

    int right_buttons_width = 0;

    // measure the buttons on the right side
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;
        if (button.id == wxAUI_BUTTON_RIGHT) // See the block comment above.
            continue;

        wxRect button_rect = rect;
        button_rect.SetY(1);
        button_rect.SetWidth(rect.width - right_buttons_width);

        const wxAuiTabContainerSnapshot revision = captureRevision();
        const int buttonWidth = art->GetButtonRect(dc,
                                                   wnd,
                                                   button_rect,
                                                   button.id,
                                                   button.curState,
                                                   wxRIGHT);
        if ( !hasExactRevision(revision) )
            return 0;

        right_buttons_width += buttonWidth;
    }


    int left_buttons_width = 0;

    // measure the buttons on the left side

    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;
        if (button.id == wxAUI_BUTTON_LEFT) // See the block comment above.
            continue;

        wxRect button_rect(left_buttons_width, 1, 1000, rect.height);

        const wxAuiTabContainerSnapshot revision = captureRevision();
        const int buttonWidth = art->GetButtonRect(dc,
                                                   wnd,
                                                   button_rect,
                                                   button.id,
                                                   button.curState,
                                                   wxLEFT);
        if ( !hasExactRevision(revision) )
            return 0;

        left_buttons_width += buttonWidth;
    }

    if (left_buttons_width == 0)
    {
        const wxAuiTabContainerSnapshot revision = captureRevision();
        left_buttons_width = art->GetIndentSize();
        if ( !hasExactRevision(revision) )
            return 0;
    }

    return rect.width - left_buttons_width - right_buttons_width - wnd->FromDIP(wxAUI_BUTTONS_BORDER);
}

int wxAuiTabContainer::LayoutMultiLineTabs(const wxRect& rect, wxWindow* wnd)
{
    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return 0;

    wxInfoDC dc(wnd);
    auto* tabCtrl = wxDynamicCast(wnd, wxAuiTabCtrl);
    if ( tabCtrl && static_cast<wxAuiTabContainer*>(tabCtrl) != this )
        tabCtrl = nullptr;

    const wxWeakRef<wxAuiTabCtrl> weakTabCtrl(tabCtrl);
    wxAuiNotebook* const originalBook = tabCtrl ? tabCtrl->GetBook() : nullptr;
    const wxWeakRef<wxAuiNotebook> weakBook(originalBook);
    wxAuiTabArt* const art = m_art;
    if ( !art )
        return 0;
    wxAuiTabArtLease artLease(art);
    const auto captureRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, wnd);
    };
    const auto hasExactContainerRevision = [
        &](const wxAuiTabContainerSnapshot& state)
    {
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };
    wxAuiPageWindowProjection pageProjection;
    if ( !pageProjection.Capture(m_pages) )
        return 0;
    std::vector<wxAuiNotebookPage> measuredPages;
    std::vector<wxAuiTabContainerButton> buttons;
    try
    {
        measuredPages.assign(m_pages.begin(), m_pages.end());
        buttons = m_buttons;
    }
    catch ( const std::bad_alloc& )
    {
        return 0;
    }

    const auto hasExactProjection = [&]()
    {
        if ( m_art != art ||
                m_pages.GetCount() != measuredPages.size() ||
                m_buttons.size() != buttons.size() )
        {
            return false;
        }

        if ( !tabCtrl )
            return true;

        wxAuiNotebook* const book = weakBook.get();
        if ( weakTabCtrl.get() != tabCtrl || !book ||
                wxWindowIsUnavailableForCallbacks(tabCtrl) ||
                wxWindowIsUnavailableForCallbacks(book) ||
                tabCtrl->GetParent() != book )
        {
            return false;
        }

        return true;
    };

    const auto hasExactFullProjection = [&]()
    {
        if ( !hasExactProjection() )
            return false;

        wxAuiNotebook* const book = tabCtrl ? weakBook.get() : nullptr;
        for ( size_t i = 0; i < measuredPages.size(); ++i )
        {
            wxWindow* const expectedPage = measuredPages[i].window;
            if ( !pageProjection.Matches(m_pages, i) ||
                    (tabCtrl &&
                     (!expectedPage || expectedPage->GetParent() != book)) ||
                    m_pages[i].window != expectedPage ||
                    m_pages[i].active != measuredPages[i].active ||
                    m_pages[i].kind != measuredPages[i].kind )
            {
                return false;
            }
        }
        for ( size_t i = 0; i < buttons.size(); ++i )
        {
            if ( m_buttons[i].id != buttons[i].id ||
                    m_buttons[i].location != buttons[i].location )
            {
                return false;
            }
        }
        return true;
    };

    int availableWidth = 0;
    if ( tabCtrl )
    {
        int rightButtonsWidth = 0;
        for ( auto it = buttons.rbegin(); it != buttons.rend(); ++it )
        {
            const wxAuiTabContainerButton& button = *it;
            if ( button.location != wxRIGHT ||
                    (button.curState & wxAUI_BUTTON_STATE_HIDDEN) ||
                    button.id == wxAUI_BUTTON_RIGHT )
            {
                continue;
            }
            wxRect buttonRect = rect;
            buttonRect.SetY(1);
            buttonRect.SetWidth(rect.width - rightButtonsWidth);
            const wxAuiTabContainerSnapshot revision = captureRevision();
            rightButtonsWidth += art->GetButtonRect(
                dc, wnd, buttonRect, button.id, button.curState, wxRIGHT);
            if ( !hasExactContainerRevision(revision) ||
                    !hasExactProjection() )
                return 0;
        }

        int leftButtonsWidth = 0;
        for ( auto it = buttons.rbegin(); it != buttons.rend(); ++it )
        {
            const wxAuiTabContainerButton& button = *it;
            if ( button.location != wxLEFT ||
                    (button.curState & wxAUI_BUTTON_STATE_HIDDEN) ||
                    button.id == wxAUI_BUTTON_LEFT )
            {
                continue;
            }
            const wxRect buttonRect(leftButtonsWidth, 1, 1000, rect.height);
            const wxAuiTabContainerSnapshot revision = captureRevision();
            leftButtonsWidth += art->GetButtonRect(
                dc, wnd, buttonRect, button.id, button.curState, wxLEFT);
            if ( !hasExactContainerRevision(revision) ||
                    !hasExactProjection() )
                return 0;
        }
        if ( leftButtonsWidth == 0 )
        {
            const wxAuiTabContainerSnapshot revision = captureRevision();
            leftButtonsWidth = art->GetIndentSize();
            if ( !hasExactContainerRevision(revision) ||
                    !hasExactProjection() )
                return 0;
        }
        availableWidth = rect.width - leftButtonsWidth - rightButtonsWidth -
                         wnd->FromDIP(wxAUI_BUTTONS_BORDER);
        if ( !hasExactProjection() )
            return 0;
    }
    else
    {
        const wxAuiTabContainerSnapshot revision = captureRevision();
        availableWidth = GetAvailableForTabs(rect, dc, wnd);
        if ( !hasExactContainerRevision(revision) )
            return 0;
    }

    int extraHeight = 0;
    int widthRow = 0;
    bool firstTabInRow = true;
    int lastRowEnd = wxNOT_FOUND;
    for ( size_t i = 0; i < measuredPages.size(); ++i )
    {
        wxAuiNotebookPage& page = measuredPages[i];
        if ( page.kind != wxAuiTabKind::Locked )
        {
            wxAuiUpdateButtonsState(m_flags, page, firstTabInRow);
            firstTabInRow = false;
        }

        // The previous art callback may have destroyed this page without
        // touching the container. Validate its pre-callback weak identity
        // before passing the raw page record to the next virtual call.
        if ( !pageProjection.Matches(m_pages, i) )
            return 0;

        const wxAuiTabContainerSnapshot revision = captureRevision();
        const wxSize size = art->GetPageTabSize(dc, wnd, page);
        if ( !hasExactContainerRevision(revision) ||
                !hasExactProjection() ||
                !pageProjection.Matches(m_pages, i) )
            return 0;

        widthRow += size.x;
        page.rowEnd = false;
        if ( widthRow > availableWidth )
        {
            widthRow = size.x;
            extraHeight += size.y;
            firstTabInRow = true;
            if ( lastRowEnd != wxNOT_FOUND )
                measuredPages[static_cast<size_t>(lastRowEnd)].rowEnd = true;
        }
        lastRowEnd = static_cast<int>(i);
    }

    if ( lastRowEnd != wxNOT_FOUND )
        measuredPages[static_cast<size_t>(lastRowEnd)].rowEnd = true;

    if ( !hasExactFullProjection() )
        return 0;
    for ( size_t i = 0; i < measuredPages.size(); ++i )
    {
        m_pages[i].buttons = std::move(measuredPages[i].buttons);
        m_pages[i].rowEnd = measuredPages[i].rowEnd;
    }
    wxAuiBumpTabContainerRevision(this);

    return extraHeight;
}

void wxAuiTabContainer::RenderButtons(wxDC& dc, wxWindow* wnd,
                                      int& left_buttons_width,
                                      int& right_buttons_width)
{
    // A negative width is the completion marker consumed by Render(). Keep
    // it negative if any callback invalidates the captured container.
    left_buttons_width = -1;
    right_buttons_width = -1;

    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return;

    wxAuiTabArt* const art = m_art;
    if ( !art )
        return;

    wxAuiPageWindowProjection pageProjection;
    if ( !pageProjection.Capture(m_pages) )
        return;

    wxAuiTabArtLease artLease(art);
    const auto captureRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, wnd);
    };
    const auto hasExactRevision = [&](const wxAuiTabContainerSnapshot& state)
    {
        // Keep this short-circuit: no container member may be evaluated after
        // its standalone lifetime token has disappeared.
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };

    size_t i;
    const size_t page_count = m_pages.GetCount();
    const size_t button_count = m_buttons.size();
    std::vector<wxRect> stagedButtonRects;
    std::vector<unsigned char> stagedButtonRectReady;
    try
    {
        stagedButtonRects.resize(button_count);
        stagedButtonRectReady.assign(button_count, 0);
    }
    catch ( const std::bad_alloc& )
    {
        return;
    }

    const size_t normalizedOffset = page_count
        ? std::min(m_tabOffset, page_count - 1)
        : 0;
    if ( normalizedOffset != m_tabOffset )
    {
        m_tabOffset = normalizedOffset;
        wxAuiBumpTabContainerRevision(this);
    }

    // ensure we show as many tabs as possible
    while ( page_count && m_tabOffset > 0 )
    {
        const wxAuiTabContainerSnapshot revision = captureRevision();
        const bool visible =
            IsTabVisible(page_count - 1, m_tabOffset - 1, &dc, wnd);
        if ( !hasExactRevision(revision) )
            return;
        if ( !visible )
            break;

        --m_tabOffset;
        wxAuiBumpTabContainerRevision(this);
    }

    // find out if size of tabs is larger than can be
    // afforded on screen
    int total_width = 0;
    int visible_width = 0;
    for (i = 0; i < page_count; ++i)
    {
        if ( !pageProjection.Matches(m_pages, i) )
            return;
        UpdateButtonsState(m_pages.Item(i));
        wxAuiNotebookPage page;
        if ( !wxAuiTryCopyNotebookPage(m_pages.Item(i), &page) )
            return;

        int x_extent = 0;
        const wxAuiTabContainerSnapshot revision = captureRevision();
        const wxSize size = art->GetPageTabSize(dc, wnd, page, &x_extent);
        if ( !hasExactRevision(revision) ||
                !pageProjection.Matches(m_pages, i) )
            return;

        if (i+1 < page_count)
            total_width += x_extent;
        else
            total_width += size.x;

        if (i >= m_tabOffset)
        {
            if (i+1 < page_count)
                visible_width += x_extent;
            else
                visible_width += size.x;
        }
    }

    bool stateChanged = false;
    const int availableWidth = GetAvailableForTabs(m_rect, dc, wnd);

    if (total_width > availableWidth || m_tabOffset != 0)
    {
        // show left/right buttons
        for (i = 0; i < button_count; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.at(i);
            if (button.id == wxAUI_BUTTON_LEFT ||
                button.id == wxAUI_BUTTON_RIGHT)
            {
                const int oldState = button.curState;
                button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
                stateChanged = stateChanged || oldState != button.curState;
            }
        }
    }
    else
    {
        // hide left/right buttons
        for (i = 0; i < button_count; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.at(i);
            if (button.id == wxAUI_BUTTON_LEFT ||
                button.id == wxAUI_BUTTON_RIGHT)
            {
                const int oldState = button.curState;
                button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
                stateChanged = stateChanged || oldState != button.curState;
            }
        }
    }

    // determine whether various buttons should be enabled
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(i);
        const int oldState = button.curState;
        if (button.id == wxAUI_BUTTON_LEFT)
        {
            if (m_tabOffset == 0)
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;
        }
        else if (button.id == wxAUI_BUTTON_RIGHT)
        {
            int button_width = 0;
            for ( const auto& b : m_buttons )
                button_width += b.rect.GetWidth();

            if (visible_width < m_rect.GetWidth() - button_width)
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;
        }
        else if (button.id == wxAUI_BUTTON_CLOSE)
        {
            button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;

            // Disable "Close" button if the current page is locked, as such
            // pages can't be closed.
            for (const auto& page : m_pages)
            {
                if (page.active)
                {
                    switch ( page.kind )
                    {
                        case wxAuiTabKind::Normal:
                        case wxAuiTabKind::Pinned:
                            break;

                        case wxAuiTabKind::Locked:
                            button.curState |= wxAUI_BUTTON_STATE_DISABLED;
                            break;
                    }
                    break;
                }
            }
        }
        stateChanged = stateChanged || oldState != button.curState;
    }

    if ( stateChanged )
        wxAuiBumpTabContainerRevision(this);

    int measuredRightButtonsWidth = 0;

    // draw the buttons on the right side
    for (i = 0; i < button_count; ++i)
    {
        const size_t buttonIndex = button_count - i - 1;
        wxAuiTabContainerButton& button = m_buttons.at(buttonIndex);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        const int id = button.id;
        const int state = button.curState;
        wxRect button_rect = m_rect;
        button_rect.SetY(1);
        button_rect.SetWidth(m_rect.width - measuredRightButtonsWidth);

        wxRect drawnRect;
        const wxAuiTabContainerSnapshot revision = captureRevision();
        art->DrawButton(dc,
                        wnd,
                        button_rect,
                        id,
                        state,
                        wxRIGHT,
                        &drawnRect);
        if ( !hasExactRevision(revision) )
            return;

        stagedButtonRects[buttonIndex] = drawnRect;
        stagedButtonRectReady[buttonIndex] = 1;
        measuredRightButtonsWidth += drawnRect.GetWidth();
    }

    int measuredLeftButtonsWidth = 0;

    // draw the buttons on the left side

    for (i = 0; i < button_count; ++i)
    {
        const size_t buttonIndex = button_count - i - 1;
        wxAuiTabContainerButton& button = m_buttons.at(buttonIndex);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        const int id = button.id;
        const int state = button.curState;
        wxRect button_rect(measuredLeftButtonsWidth, 1, 1000,
                           m_rect.height);

        wxRect drawnRect;
        const wxAuiTabContainerSnapshot revision = captureRevision();
        art->DrawButton(dc,
                        wnd,
                        button_rect,
                        id,
                        state,
                        wxLEFT,
                        &drawnRect);
        if ( !hasExactRevision(revision) )
            return;

        stagedButtonRects[buttonIndex] = drawnRect;
        stagedButtonRectReady[buttonIndex] = 1;
        measuredLeftButtonsWidth += drawnRect.GetWidth();
    }

    if (measuredLeftButtonsWidth == 0)
    {
        const wxAuiTabContainerSnapshot revision = captureRevision();
        measuredLeftButtonsWidth = art->GetIndentSize();
        if ( !hasExactRevision(revision) )
            return;
    }


    // update the tab buttons visibility
    // make sure tab button entries which aren't used are marked as hidden
    bool pageStateChanged = false;
    for (i = page_count; i < m_pages.GetCount(); ++i)
    {
        for (auto& button : m_pages[i].buttons)
        {
            const int oldState = button.curState;
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
            pageStateChanged =
                pageStateChanged || oldState != button.curState;
        }
    }

    // buttons before the tab offset must be set to hidden
    for (i = 0; i < m_tabOffset && i < m_pages.GetCount(); ++i)
    {
        for (auto& button : m_pages[i].buttons)
        {
            const int oldState = button.curState;
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
            pageStateChanged =
                pageStateChanged || oldState != button.curState;
        }
    }

    if ( pageStateChanged )
        wxAuiBumpTabContainerRevision(this);

    // Geometry is committed only after every page weak identity survived all
    // button and sizing callbacks. A callback deleting a sibling page leaves
    // the previously committed hit rectangles untouched.
    if ( !pageProjection.MatchesAll(m_pages) )
        return;

    bool buttonGeometryChanged = false;
    for ( size_t buttonIndex = 0;
          buttonIndex < button_count;
          ++buttonIndex )
    {
        if ( stagedButtonRectReady[buttonIndex] &&
                m_buttons[buttonIndex].rect !=
                    stagedButtonRects[buttonIndex] )
        {
            m_buttons[buttonIndex].rect = stagedButtonRects[buttonIndex];
            buttonGeometryChanged = true;
        }
    }
    if ( buttonGeometryChanged )
        wxAuiBumpTabContainerRevision(this);

    left_buttons_width = measuredLeftButtonsWidth;
    right_buttons_width = measuredRightButtonsWidth;
}


// Render() renders the tab catalog to the specified DC
// It is a virtual function and can be overridden to
// provide custom drawing capabilities
void wxAuiTabContainer::Render(wxDC* pdc, wxWindow* wnd)
{
    if ( !pdc || !wnd || wxWindowIsUnavailableForCallbacks(wnd) ||
            m_rect.IsEmpty() )
        return;

    wxDC& dc = *pdc;
    wxAuiTabArt* const art = m_art;
    if ( !art )
        return;

    wxAuiPageWindowProjection pageProjection;
    if ( !pageProjection.Capture(m_pages) )
        return;

    wxAuiTabArtLease artLease(art);
    const auto captureRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, wnd);
    };
    const auto hasExactRevision = [&](const wxAuiTabContainerSnapshot& state)
    {
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };

    // draw background
    const wxRect backgroundRect = m_rect;
    const wxAuiTabContainerSnapshot backgroundRevision = captureRevision();
    art->DrawBackground(dc, wnd, backgroundRect);
    if ( !hasExactRevision(backgroundRevision) ||
            !pageProjection.MatchesAll(m_pages) )
        return;

    // draw buttons
    wxAuiTabContainerLifetimeState lifetimeBeforeButtons;
    if ( !wxAuiGetTabContainerLifetimeState(this, &lifetimeBeforeButtons) )
        return;
    const wxAuiTabArtAdoption artAdoption = wxAuiGetTabArtAdoption(art);
    const wxWeakRef<wxWindow> windowLifetime(wnd);
    wxWindow* const windowParent = wnd->GetParent();
    const wxWeakRef<wxWindow> windowParentLifetime(windowParent);

    int left_buttons_width = -1;
    int right_buttons_width = -1;
    RenderButtons(dc, wnd, left_buttons_width, right_buttons_width);

    // RenderButtons() intentionally changes rectangles and button states, so
    // only its lifetime witness can span the call. Its negative completion
    // marker tells us whether every nested art callback was validated.
    wxAuiTabContainerLifetimeState lifetimeAfterButtons;
    if ( !wxAuiGetTabContainerLifetimeState(this, &lifetimeAfterButtons) ||
            lifetimeAfterButtons.lifetime != lifetimeBeforeButtons.lifetime ||
            left_buttons_width < 0 || right_buttons_width < 0 ||
            windowLifetime.get() != wnd ||
            wxWindowIsUnavailableForCallbacks(wnd) ||
            wnd->GetParent() != windowParent ||
            (windowParent &&
             (windowParentLifetime.get() != windowParent ||
              wxWindowIsUnavailableForCallbacks(windowParent))) ||
            wxAuiGetTabArtAdoption(art) != artAdoption || m_art != art ||
            !pageProjection.MatchesAll(m_pages) )
    {
        return;
    }

    const size_t page_count = m_pages.GetCount();

    struct StagedPageGeometry
    {
        wxRect rect;
        std::vector<wxRect> buttonRects;
        bool ready = false;
    };
    std::vector<StagedPageGeometry> stagedGeometry;
    try
    {
        stagedGeometry.resize(page_count);
        for ( size_t i = 0; i < page_count; ++i )
            stagedGeometry[i].buttonRects.resize(m_pages[i].buttons.size());
    }
    catch ( const std::bad_alloc& )
    {
        return;
    }

    int offset = left_buttons_width;

    // draw the tabs

    size_t active = (size_t)-1;
    wxRect active_rect;

    wxRect rect = m_rect;
    rect.y = 0;

    if (IsFlagSet(wxAUI_NB_MULTILINE) && page_count)
    {
        // We assume vertical size of all tabs is the same, so it doesn't
        // matter which one we use for measuring.
        if ( !pageProjection.Matches(m_pages, 0) )
            return;
        wxAuiNotebookPage page;
        if ( !wxAuiTryCopyNotebookPage(m_pages.Item(0), &page) )
            return;
        const wxAuiTabContainerSnapshot revision = captureRevision();
        const auto size = art->GetPageTabSize(dc, wnd, page);
        if ( !hasExactRevision(revision) ||
                !pageProjection.Matches(m_pages, 0) )
            return;

        rect.height = size.y;
    }
    else
    {
        rect.height = m_tabRowHeight;
    }

    // Note that this must be consistent with GetAvailableForTabs().
    const int rightBorder = m_rect.width - right_buttons_width - wnd->FromDIP(wxAUI_BUTTONS_BORDER);

    size_t i;
    for (i = m_tabOffset; i < page_count; ++i)
    {
        if ( !pageProjection.Matches(m_pages, i) )
            return;
        UpdateButtonsState(m_pages.Item(i));
        wxAuiNotebookPage renderedPage;
        if ( !wxAuiTryCopyNotebookPage(m_pages.Item(i), &renderedPage) )
            return;

        // Check if this tab is at least partially visible when using a single
        // row (otherwise all rows are visible).
        if (!IsFlagSet(wxAUI_NB_MULTILINE) && offset >= rightBorder)
        {
            // This (and, hence, all the subsequent) tab(s) would be completely
            // hidden, stop drawing.
            break;
        }

        rect.x = offset;
        rect.width = rightBorder - offset;

        const wxAuiTabContainerSnapshot revision = captureRevision();
        const int extent = art->DrawPageTab(dc, wnd, renderedPage, rect);
        if ( !hasExactRevision(revision) ||
                !pageProjection.Matches(m_pages, i) ||
                !wxAuiHasSamePagePayloadExceptGeometry(m_pages.Item(i),
                                                       renderedPage) )
            return;

        StagedPageGeometry& geometry = stagedGeometry[i];
        geometry.rect = renderedPage.rect;
        for ( size_t buttonIndex = 0;
              buttonIndex < renderedPage.buttons.size();
              ++buttonIndex )
        {
            geometry.buttonRects[buttonIndex] =
                renderedPage.buttons[buttonIndex].rect;
        }
        geometry.ready = true;

        offset += extent;

        if (renderedPage.active)
        {
            active = i;
            active_rect = rect;
        }

        // Start a new row if necessary when using multiple rows.
        if (IsFlagSet(wxAUI_NB_MULTILINE) && renderedPage.rowEnd)
        {
            offset = left_buttons_width;

            rect.y += rect.height;
        }
    }

    if ( !pageProjection.MatchesAll(m_pages) )
        return;

    // make sure to deactivate buttons which are off the screen to the right
    bool pageStateChanged = false;
    const size_t firstHidden = i < page_count ? i + 1 : page_count;
    for (i = firstHidden; i < m_pages.GetCount(); ++i)
    {
        for (auto& button : m_pages[i].buttons)
        {
            const int oldState = button.curState;
            button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
            pageStateChanged =
                pageStateChanged || oldState != button.curState;
        }
    }
    if ( pageStateChanged )
        wxAuiBumpTabContainerRevision(this);


    // draw the active tab again so it stands in the foreground
    if (active >= m_tabOffset && active < m_pages.GetCount())
    {
        if ( !pageProjection.Matches(m_pages, active) )
            return;
        wxAuiNotebookPage renderedPage;
        if ( !wxAuiTryCopyNotebookPage(m_pages.Item(active), &renderedPage) )
            return;

        const StagedPageGeometry& previousGeometry =
            stagedGeometry[active];
        if ( previousGeometry.ready )
        {
            renderedPage.rect = previousGeometry.rect;
            for ( size_t buttonIndex = 0;
                  buttonIndex < renderedPage.buttons.size();
                  ++buttonIndex )
            {
                renderedPage.buttons[buttonIndex].rect =
                    previousGeometry.buttonRects[buttonIndex];
            }
        }

        const wxAuiTabContainerSnapshot revision = captureRevision();
        art->DrawPageTab(dc, wnd, renderedPage, active_rect);
        if ( !hasExactRevision(revision) ||
                !pageProjection.Matches(m_pages, active) ||
                !wxAuiHasSamePagePayloadExceptGeometry(
                    m_pages.Item(active),
                    renderedPage) )
            return;

        StagedPageGeometry& geometry = stagedGeometry[active];
        geometry.rect = renderedPage.rect;
        for ( size_t buttonIndex = 0;
              buttonIndex < renderedPage.buttons.size();
              ++buttonIndex )
        {
            geometry.buttonRects[buttonIndex] =
                renderedPage.buttons[buttonIndex].rect;
        }
        geometry.ready = true;
    }

    // Publish the complete geometry transaction only after every page which
    // existed before DrawBackground() still resolves to the same live window
    // and parent. This prevents a page-0 callback deleting page 1 from leaving
    // page 0 with newly committed hit rectangles in an invalid projection.
    if ( !pageProjection.MatchesAll(m_pages) )
        return;

    for ( size_t pageIndex = 0;
          pageIndex < stagedGeometry.size();
          ++pageIndex )
    {
        const StagedPageGeometry& geometry = stagedGeometry[pageIndex];
        if ( geometry.ready &&
                m_pages.Item(pageIndex).buttons.size() !=
                    geometry.buttonRects.size() )
        {
            return;
        }
    }

    bool geometryChanged = false;
    for ( size_t pageIndex = 0;
          pageIndex < stagedGeometry.size();
          ++pageIndex )
    {
        const StagedPageGeometry& geometry = stagedGeometry[pageIndex];
        if ( !geometry.ready )
            continue;

        wxAuiNotebookPage& currentPage = m_pages.Item(pageIndex);
        if ( currentPage.rect != geometry.rect )
        {
            currentPage.rect = geometry.rect;
            geometryChanged = true;
        }
        for ( size_t buttonIndex = 0;
              buttonIndex < currentPage.buttons.size();
              ++buttonIndex )
        {
            if ( currentPage.buttons[buttonIndex].rect !=
                    geometry.buttonRects[buttonIndex] )
            {
                currentPage.buttons[buttonIndex].rect =
                    geometry.buttonRects[buttonIndex];
                geometryChanged = true;
            }
        }
    }
    if ( geometryChanged )
        wxAuiBumpTabContainerRevision(this);
}

// Is the tab visible?
bool wxAuiTabContainer::IsTabVisible(int tabPage, int tabOffset, wxReadOnlyDC* dc, wxWindow* wnd)
{
    if (!dc || !dc->IsOk())
        return false;

    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return false;

    const wxWeakRef<wxWindow> weakWindow(wnd);
    wxAuiTabArt* const art = m_art;
    if ( !art )
        return false;
    wxAuiPageWindowProjection pageProjection;
    if ( !pageProjection.Capture(m_pages) )
        return false;
    wxAuiTabArtLease artLease(art);
    const auto captureContainerRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, wnd);
    };
    const auto hasExactContainerRevision = [
        &](const wxAuiTabContainerSnapshot& state)
    {
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };
    const wxRect containerRect = m_rect;
    const size_t containerOffset = m_tabOffset;
    const unsigned int containerFlags = m_flags;
    const size_t page_count = m_pages.GetCount();
    const size_t button_count = m_buttons.size();

    const auto isExactRevision = [&]()
    {
        if ( weakWindow.get() != wnd ||
                wxWindowIsUnavailableForCallbacks(wnd) || m_art != art ||
                m_rect != containerRect || m_tabOffset != containerOffset ||
                m_flags != containerFlags ||
                m_pages.GetCount() != page_count ||
                m_buttons.size() != button_count )
        {
            return false;
        }

        return true;
    };

    // All tabs are always visible when multiple lines are used.
    if (IsFlagSet(wxAUI_NB_MULTILINE))
        return true;

    size_t i;

    // First check if both buttons are disabled - if so, there's no need to
    // check further for visibility.
    int arrowButtonVisibleCount = 0;
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(i);
        if (button.id == wxAUI_BUTTON_LEFT ||
            button.id == wxAUI_BUTTON_RIGHT)
        {
            if ((button.curState & wxAUI_BUTTON_STATE_HIDDEN) == 0)
                arrowButtonVisibleCount ++;
        }
    }

    // Tab must be visible
    if (arrowButtonVisibleCount == 0)
        return true;

    // If tab is less than the given offset, it must be invisible by definition
    if (tabPage < tabOffset)
        return false;

    // draw buttons
    int left_buttons_width = 0;
    int right_buttons_width = 0;

    // calculate size of the buttons on the right side
    int offset = m_rect.x + m_rect.width;
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxRIGHT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        offset -= button.rect.GetWidth();
        right_buttons_width += button.rect.GetWidth();
    }

    offset = 0;

    // calculate size of the buttons on the left side
    for (i = 0; i < button_count; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.at(button_count - i - 1);

        if (button.location != wxLEFT)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        offset += button.rect.GetWidth();
        left_buttons_width += button.rect.GetWidth();
    }

    offset = left_buttons_width;

    if (offset == 0)
    {
        const wxAuiTabContainerSnapshot revision =
            captureContainerRevision();
        offset += art->GetIndentSize();
        if ( !hasExactContainerRevision(revision) ||
                !isExactRevision() ||
                !pageProjection.MatchesAll(m_pages) )
            return false;
    }

    wxRect rect = m_rect;
    const int buttonBorder = wnd->FromDIP(wxAUI_BUTTONS_BORDER);
    if ( !isExactRevision() )
        return false;

    // See if the given page is visible at the given tab offset (effectively scroll position)
    for (i = tabOffset; i < page_count; ++i)
    {
        // Measuring visibility must not publish transient button state into
        // the container: callers use an exact revision across this function.
        if ( !pageProjection.Matches(m_pages, i) )
            return false;
        wxAuiNotebookPage page;
        if ( !wxAuiTryCopyNotebookPage(m_pages.Item(i), &page) )
            return false;

        rect.width = containerRect.width - right_buttons_width - offset -
                     buttonBorder;

        if (rect.width <= 0)
            return false; // haven't found the tab, and we've run out of space, so return false

        wxAuiUpdateButtonsState(containerFlags, page, false);

        int x_extent = 0;
        const wxAuiTabContainerSnapshot revision =
            captureContainerRevision();
        art->GetPageTabSize(*dc, wnd, page, &x_extent);
        if ( !hasExactContainerRevision(revision) ||
                !isExactRevision() ||
                !pageProjection.Matches(m_pages, i) )
            return false;

        offset += x_extent;

        if (i == (size_t) tabPage)
        {
            if ( !pageProjection.MatchesAll(m_pages) )
                return false;
            // If not all of the tab is visible, and supposing there's space to display it all,
            // we could do better so we return false.
            if (((containerRect.width - right_buttons_width - offset -
                  buttonBorder) <= 0) &&
                ((containerRect.width - right_buttons_width -
                  left_buttons_width) > x_extent))
                return false;
            else
                return true;
        }
    }

    // Shouldn't really get here, but if it does, assume the tab is visible to prevent
    // further looping in calling code.
    return pageProjection.MatchesAll(m_pages);
}

// Make the tab visible if it wasn't already
void wxAuiTabContainer::MakeTabVisible(int tabPage, wxWindow* win)
{
    if ( !win || wxWindowIsUnavailableForCallbacks(win) )
        return;

    const wxWeakRef<wxWindow> weakWindow(win);
    wxAuiTabArt* const art = m_art;
    const size_t pageCount = m_pages.GetCount();
    const size_t buttonCount = m_buttons.size();
    const wxRect containerRect = m_rect;
    const unsigned int containerFlags = m_flags;

    const auto isExactRevision = [&]()
    {
        if ( weakWindow.get() != win ||
                wxWindowIsUnavailableForCallbacks(win) || m_art != art ||
                m_rect != containerRect || m_flags != containerFlags ||
                m_pages.GetCount() != pageCount ||
                m_buttons.size() != buttonCount )
        {
            return false;
        }
        return true;
    };
    const auto captureContainerRevision = [&]()
    {
        return wxAuiTabContainerSnapshot(
            this, m_art, m_rect, m_tabOffset, m_flags, m_tabRowHeight,
            m_pages, m_buttons, win);
    };
    const auto hasExactContainerRevision = [
        &](const wxAuiTabContainerSnapshot& state)
    {
        return state.IsAlive() &&
               state.Matches(m_art, m_rect, m_tabOffset, m_flags,
                             m_tabRowHeight, m_pages, m_buttons);
    };

    wxInfoDC dc(win);
    const wxAuiTabContainerSnapshot initialRevision =
        captureContainerRevision();
    const bool initiallyVisible =
        IsTabVisible(tabPage, GetTabOffset(), &dc, win);
    if ( !hasExactContainerRevision(initialRevision) ||
            !isExactRevision() )
        return;

    if (!initiallyVisible)
    {
        for (int i = 0; i < static_cast<int>(pageCount); i++)
        {
            const wxAuiTabContainerSnapshot revision =
                captureContainerRevision();
            const bool visible = IsTabVisible(tabPage, i, &dc, win);
            if ( !hasExactContainerRevision(revision) ||
                    !isExactRevision() )
                return;
            if (visible)
            {
                SetTabOffset(i);
                win->Refresh();
                return;
            }
        }
    }
}

// TabHitTest() tests if a tab was hit, returning the struct containing the
// window that was hit together with its position or null pointer otherwise.
wxAuiTabContainer::HitTestResult
wxAuiTabContainer::TabHitTest(const wxPoint& pt, int flags) const
{
    if (!m_rect.Contains(pt))
        return {};

    const wxAuiTabContainerButton* const btn = ButtonHitTest(pt);
    if (btn && !(btn->curState & wxAUI_BUTTON_STATE_DISABLED))
    {
        for ( const auto& button : m_buttons )
        {
            if ( btn == &button )
                return {};
        }
    }

    const size_t page_count = m_pages.GetCount();
    for (size_t i = m_tabOffset; i < page_count; ++i)
    {
        wxAuiNotebookPage& page = m_pages.Item(i);
        if (page.rect.Contains(pt))
        {
            return { page.window, static_cast<int>(i) };
        }

        // Also optionally check if the point lies over the blank space after
        // the last tab in the row if this should be allowed.
        if ((flags & HitTest_AllowAfterTab) && page.rowEnd)
        {
            if (pt.x >= page.rect.x &&
                    pt.y >= page.rect.y &&
                        pt.y < page.rect.y + page.rect.height)
            {
                return { page.window, static_cast<int>(i) };
            }
        }
    }

    return {};
}

// ButtonHitTest() tests if a button was hit. The function returns
// the button if one was hit and null pointer otherwise
wxAuiTabContainerButton*
wxAuiTabContainer::ButtonHitTest(const wxPoint& pt) const
{
    if (!m_rect.Contains(pt))
        return nullptr;

    for ( auto& button : m_buttons )
    {
        if (button.rect.Contains(pt) &&
            !(button.curState & wxAUI_BUTTON_STATE_HIDDEN ))
        {
            wxAuiBumpTabContainerRevision(this);
            return const_cast<wxAuiTabContainerButton*>(&button);
        }
    }

    for ( const auto& page : m_pages )
    {
        for ( const auto& button : page.buttons )
        {
            if (button.rect.Contains(pt) &&
                !(button.curState & (wxAUI_BUTTON_STATE_HIDDEN |
                                       wxAUI_BUTTON_STATE_DISABLED)))
            {
                wxAuiBumpTabContainerRevision(this);
                return const_cast<wxAuiTabContainerButton*>(&button);
            }
        }
    }

    return nullptr;
}



// the utility function ShowWnd() is the same as show,
// except it handles wxAuiMDIChildFrame windows as well,
// as the Show() method on this class is "unplugged"
static void ShowWnd(wxWindow* wnd, bool show)
{
#if wxUSE_MDI
    if (wxDynamicCast(wnd, wxAuiMDIChildFrame))
    {
        wxAuiMDIChildFrame* cf = (wxAuiMDIChildFrame*)wnd;
        cf->wxWindow::Show(show);
    }
    else
#endif
    {
        wnd->Show(show);
    }
}


// DoShowHide() this function shows the active window, then
// hides all of the other windows (in that order)
void wxAuiTabContainer::DoShowHide()
{
    const wxAuiNotebookPageArray& pages = GetPages();

    // show new active page first
    for ( const auto& page : pages )
    {
        if (page.active)
        {
            ShowWnd(page.window, true);
            break;
        }
    }

    // hide all other pages
    for ( const auto& page : pages )
    {
        if (!page.active)
            ShowWnd(page.window, false);
    }
}


int wxAuiTabContainer::GetFirstTabOfKind(wxAuiTabKind kind) const
{
    int pos = 0;
    for ( const auto& page : m_pages )
    {
        if ( page.kind == kind )
            break;

        pos++;
    }

    return pos;
}

int wxAuiTabContainer::GetFirstTabNotOfKind(wxAuiTabKind kind) const
{
    int pos = 0;
    for ( const auto& page : m_pages )
    {
        if ( page.kind != kind )
            break;

        pos++;
    }

    return pos;
}


// -- wxAuiTabCtrl class implementation --



wxBEGIN_EVENT_TABLE(wxAuiTabCtrl, wxControl)
    EVT_PAINT(wxAuiTabCtrl::OnPaint)
    EVT_SIZE(wxAuiTabCtrl::OnSize)
    EVT_LEFT_DOWN(wxAuiTabCtrl::OnLeftDown)
    EVT_LEFT_DCLICK(wxAuiTabCtrl::OnLeftDClick)
    EVT_LEFT_UP(wxAuiTabCtrl::OnLeftUp)
    EVT_MIDDLE_DOWN(wxAuiTabCtrl::OnMiddleDown)
    EVT_MIDDLE_UP(wxAuiTabCtrl::OnMiddleUp)
    EVT_RIGHT_DOWN(wxAuiTabCtrl::OnRightDown)
    EVT_RIGHT_UP(wxAuiTabCtrl::OnRightUp)
    EVT_MOTION(wxAuiTabCtrl::OnMotion)
    EVT_LEAVE_WINDOW(wxAuiTabCtrl::OnLeaveWindow)
    EVT_SET_FOCUS(wxAuiTabCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxAuiTabCtrl::OnKillFocus)
    EVT_CHAR(wxAuiTabCtrl::OnChar)
    EVT_MOUSE_CAPTURE_LOST(wxAuiTabCtrl::OnCaptureLost)
    EVT_SYS_COLOUR_CHANGED(wxAuiTabCtrl::OnSysColourChanged)
    EVT_DPI_CHANGED(wxAuiTabCtrl::OnDPIChanged)
wxEND_EVENT_TABLE()


wxAuiTabCtrl::wxAuiTabCtrl(wxAuiNotebook* parent, wxWindowID id)
            : wxControl(parent, id,
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxNO_BORDER | wxWANTS_CHARS,
                        wxDefaultValidator,
                        wxT("wxAuiTabCtrl"))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

wxAuiTabCtrl::wxAuiTabCtrl() = default;

bool wxAuiTabCtrl::Create(wxAuiNotebook* parent, wxWindowID id)
{
    if ( !wxControl::Create(parent, id,
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxNO_BORDER | wxWANTS_CHARS,
                            wxDefaultValidator,
                            wxT("wxAuiTabCtrl")) )
    {
        return false;
    }

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    return true;
}

wxAuiTabCtrl::~wxAuiTabCtrl() = default;

// Our parent is always wxAuiNotebook, by construction, so the cast is safe.
wxAuiNotebook* wxAuiTabCtrl::GetBook() const
{
    return static_cast<wxAuiNotebook*>(GetParent());
}

void wxAuiTabCtrl::DoShowTab(int idx)
{
    if ( idx < 0 || static_cast<size_t>(idx) >= m_pages.GetCount() )
        return;

    wxWindow* const page = m_pages[static_cast<size_t>(idx)].window;
    const wxWeakRef<wxAuiTabCtrl> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    const wxWeakRef<wxAuiNotebook> weakBook(GetBook());
    const size_t pageCount = m_pages.GetCount();

    DoUpdateActive();

    wxAuiNotebook* const book = weakBook.get();
    if ( weakThis.get() != this || !book ||
            wxWindowIsUnavailableForCallbacks(this) ||
            wxWindowIsUnavailableForCallbacks(book) ||
            weakPage.get() != page ||
            wxWindowIsUnavailableForCallbacks(page) ||
            GetParent() != book || page->GetParent() != book ||
            m_pages.GetCount() != pageCount ||
            static_cast<size_t>(idx) >= pageCount ||
            m_pages[static_cast<size_t>(idx)].window != page ||
            GetActivePage() != idx )
    {
        return;
    }

    MakeTabVisible(idx, this);
}

void wxAuiTabCtrl::DoUpdateActive()
{
    const wxWeakRef<wxAuiTabCtrl> weakThis(this);
    wxAuiNotebook* const originalBook = GetBook();
    const wxWeakRef<wxAuiNotebook> weakBook(originalBook);

    struct PageSnapshot
    {
        explicit PageSnapshot(const wxAuiNotebookPage& info)
            : page(info.window), lifetime(info.window), active(info.active)
        {
        }

        wxWindow* const page;
        const wxWeakRef<wxWindow> lifetime;
        const bool active;
    };

    std::vector<PageSnapshot> pages;
    pages.reserve(m_pages.GetCount());
    for ( const wxAuiNotebookPage& page : m_pages )
        pages.emplace_back(page);

    const auto hasExactProjection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakBook.get();
        if ( weakThis.get() != this || !book ||
                wxWindowIsUnavailableForCallbacks(this) ||
                wxWindowIsUnavailableForCallbacks(book) ||
                GetParent() != book || m_pages.GetCount() != pages.size() )
        {
            return nullptr;
        }

        for ( size_t i = 0; i < pages.size(); ++i )
        {
            const PageSnapshot& expected = pages[i];
            if ( expected.lifetime.get() != expected.page ||
                    wxWindowIsUnavailableForCallbacks(expected.page) ||
                    expected.page->GetParent() != book ||
                    m_pages[i].window != expected.page ||
                    m_pages[i].active != expected.active )
            {
                return nullptr;
            }
        }
        return book;
    };

    if ( !hasExactProjection() )
        return;

    // Show the active page first to avoid a visible gap, then hide all other
    // pages. Each wxEVT_SHOW boundary is followed by an ABA-safe projection
    // check before the next vector access.
    for ( const PageSnapshot& page : pages )
    {
        if ( page.active )
        {
            ShowWnd(page.page, true);
            if ( !hasExactProjection() )
                return;
            break;
        }
    }

    for ( const PageSnapshot& page : pages )
    {
        if ( !page.active )
        {
            ShowWnd(page.page, false);
            if ( !hasExactProjection() )
                return;
        }
    }

    Refresh();
}

void wxAuiTabCtrl::DoEndDragging()
{
    m_clickPt = wxDefaultPosition;
    m_isDragging = false;
    m_clickTab = nullptr;
}

void wxAuiTabCtrl::DoApplyRect(const wxRect& rect, int tabCtrlHeight)
{
    const wxWeakRef<wxAuiTabCtrl> weakThis(this);
    wxAuiNotebook* const book = GetBook();
    const wxWeakRef<wxAuiNotebook> weakBook(book);
    std::vector<wxWeakRef<wxWindow>> pages;
    pages.reserve(m_pages.GetCount());
    for ( const wxAuiNotebookPage& page : m_pages )
        pages.emplace_back(page.window);

    const auto hasExactProjection = [&]()
    {
        wxAuiNotebook* const currentBook = weakBook.get();
        if ( weakThis.get() != this || !currentBook ||
                wxWindowIsUnavailableForCallbacks(this) ||
                wxWindowIsUnavailableForCallbacks(currentBook) ||
                GetParent() != currentBook ||
                m_pages.GetCount() != pages.size() )
        {
            return false;
        }
        for ( size_t i = 0; i < pages.size(); ++i )
        {
            wxWindow* const page = pages[i].get();
            if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
                    page->GetParent() != currentBook ||
                    m_pages[i].window != page )
            {
                return false;
            }
        }
        return true;
    };

    // Save the full rectangle for GetHintScreenRect().
    m_fullRect = rect;

    // Save the height of a single tab row before possibly changing it below in
    // multi-line case.
    SetRowHeight(tabCtrlHeight);

    if (IsFlagSet(wxAUI_NB_MULTILINE))
    {
        tabCtrlHeight += LayoutMultiLineTabs(rect, this);
        if ( !hasExactProjection() )
            return;
    }

    // Publish the art-provider geometry before resizing the native window.
    // SetRect() is a virtual-callback boundary and can destroy this tab
    // control; doing it from a synchronous wxQt resize event would make Qt
    // resume dispatch on an already deleted QWidget.
    SetRect(wxRect(0, 0, rect.width, tabCtrlHeight));
    if ( !hasExactProjection() )
        return;

    if (IsFlagSet(wxAUI_NB_BOTTOM))
    {
        SetSize(rect.x, rect.y + rect.height - tabCtrlHeight,
                rect.width, tabCtrlHeight);
    }
    else //TODO: if (IsFlagSet(wxAUI_NB_TOP))
    {
        SetSize(rect.x, rect.y, rect.width, tabCtrlHeight);
    }
    // TODO: else if (IsFlagSet(wxAUI_NB_LEFT)){}
    // TODO: else if (IsFlagSet(wxAUI_NB_RIGHT)){}

    if ( !hasExactProjection() )
        return;
    Refresh();
    Update();
}

wxRect wxAuiTabCtrl::GetHintScreenRect() const
{
    wxRect rect = m_fullRect;
    GetParent()->ClientToScreen(&rect.x, &rect.y);
    return rect;
}

void wxAuiTabCtrl::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);

    if (GetPageCount() > 0)
        Render(&dc, this);
}

void wxAuiTabCtrl::OnSysColourChanged(wxSysColourChangedEvent &event)
{
    event.Skip();

    wxAuiTabArt* const art = m_art;
    if ( art )
    {
        const wxAuiTabContainerSnapshot revision(
            this, m_art, wxAuiTabContainer::m_rect, m_tabOffset, m_flags,
            m_tabRowHeight,
            m_pages, m_buttons, this);
        wxAuiInvokeTabArt(art, [](wxAuiTabArt* const leasedArt)
        {
            leasedArt->UpdateColoursFromSystem();
        });

        // Do not inspect this window after a custom provider has destroyed or
        // structurally changed it. The event can safely finish on the stack.
        if ( !revision.IsAlive() )
            return;
    }
}

void wxAuiTabCtrl::OnSize(wxSizeEvent& evt)
{
    wxSize s = evt.GetSize();
    wxRect r(0, 0, s.GetWidth(), s.GetHeight());
    if ( r != wxAuiTabContainer::m_rect )
        SetRect(r);
}

void wxAuiTabCtrl::OnLeftDown(wxMouseEvent& evt)
{
    CaptureMouse();

    // Reset any previous values first.
    DoEndDragging();

    const wxPoint pos = evt.GetPosition();
    wxAuiTabContainerButton* const buttonUnderMouse = ButtonHitTest(pos);

    if ( auto const tabInfo = TabHitTest(pos) )
    {
        int new_selection = tabInfo.pos;

        // wxAuiNotebooks always want to receive this event
        // even if the tab is already active, because they may
        // have multiple tab controls
        if ((new_selection != GetActivePage()) && !buttonUnderMouse)
        {
            wxAuiTabEventSource::TabClicked(this, new_selection);
        }

        m_clickPt.x = evt.m_x;
        m_clickPt.y = evt.m_y;
        m_clickTab = tabInfo.window;
    }

    auto* const pressedButton = FindPressedButton();
    if ( buttonUnderMouse != pressedButton )
    {
        if ( pressedButton )
            ClearButtonState(*pressedButton, wxAUI_BUTTON_STATE_PRESSED);

        if ( buttonUnderMouse )
            SetButtonState(*buttonUnderMouse, wxAUI_BUTTON_STATE_PRESSED);
    }
    //else: No button state to change.
}

void wxAuiTabCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    if (m_isDragging)
    {
        const auto clickTab = m_clickTab;

        DoEndDragging();

        wxAuiTabEventSource::TabCancelDrag(this, GetIdxFromWindow(clickTab));
    }

    auto* const pressedButton = FindPressedButton();
    if ( pressedButton )
        ClearButtonState(*pressedButton, wxAUI_BUTTON_STATE_PRESSED);

    auto* const hoverButton = FindHoverButton();
    if ( hoverButton )
        ClearButtonState(*hoverButton, wxAUI_BUTTON_STATE_HOVER);
}

void wxAuiTabCtrl::OnLeftUp(wxMouseEvent& evt)
{
    if (GetCapture() == this)
        ReleaseMouse();

    if (m_isDragging)
    {
        const auto clickTab = m_clickTab;

        DoEndDragging();

        wxAuiTabEventSource::TabEndDrag(this, GetIdxFromWindow(clickTab));

        return;
    }

    if (auto* const pressedButton = FindPressedButton())
    {
        ClearButtonState(*pressedButton, wxAUI_BUTTON_STATE_PRESSED);

        // make sure we're still clicking the button
        const wxAuiTabContainerButton* const
            button = ButtonHitTest(evt.GetPosition());
        if (!button || button->curState & wxAUI_BUTTON_STATE_DISABLED)
            return;

        if (button != pressedButton)
            return;

        if (!(pressedButton->curState & wxAUI_BUTTON_STATE_DISABLED))
        {
            OnButton(GetIdxFromWindow(m_clickTab), pressedButton->id);
        }
    }

    DoEndDragging();
}

void wxAuiTabCtrl::OnMiddleUp(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiTabEventSource::TabMiddleUp(this, tabInfo.pos);
}

void wxAuiTabCtrl::OnMiddleDown(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiTabEventSource::TabMiddleDown(this, tabInfo.pos);
}

void wxAuiTabCtrl::OnRightUp(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiTabEventSource::TabRightUp(this, tabInfo.pos);
}

void wxAuiTabCtrl::OnRightDown(wxMouseEvent& evt)
{
    auto const tabInfo = TabHitTest(evt.GetPosition());
    if (!tabInfo)
        return;

    wxAuiTabEventSource::TabRightDown(this, tabInfo.pos);
}

void wxAuiTabCtrl::OnLeftDClick(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();

    if (!TabHitTest(pos) && !ButtonHitTest(pos))
    {
        wxAuiTabEventSource::TabBgDClick(this);
    }
}

void wxAuiTabCtrl::OnMotion(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();

    // Don't highlight any buttons while dragging the tab itself.
    if ( !m_isDragging )
    {
        // check if the mouse is hovering above a button and, if so, if it's the
        // same one as before or a different one
        auto* const hoverButton = FindHoverButton();
        wxAuiTabContainerButton* const button = ButtonHitTest(pos);
        if ( button != hoverButton )
        {
            if ( hoverButton )
                ClearButtonState(*hoverButton, wxAUI_BUTTON_STATE_HOVER);

            if ( button && !(button->curState & wxAUI_BUTTON_STATE_DISABLED) )
                SetButtonState(*button, wxAUI_BUTTON_STATE_HOVER);
        }

        // Don't do anything else if we're hovering over a button, even a
        // disabled one.
        if ( button )
            return;

        // Also skip the rest if we're moving the mouse while a button is
        // pressed.
        if ( FindPressedButton() )
            return;
    }

    bool hovering = false;
    if (evt.Moving())
    {
        if ( auto const tabInfo = TabHitTest(pos) )
        {
            hovering = true;

            SetHoverTab(tabInfo.window);

#if wxUSE_TOOLTIPS
            wxString tooltip(m_pages[tabInfo.pos].tooltip);

            // If the text changes, set it else, keep old, to avoid
            // 'moving tooltip' effect
            if (GetToolTipText() != tooltip)
                SetToolTip(tooltip);
#endif // wxUSE_TOOLTIPS
        }
    }

    if (!hovering)
    {
        SetHoverTab(nullptr);

#if wxUSE_TOOLTIPS
        UnsetToolTip();
#endif // wxUSE_TOOLTIPS
    }

    if (!evt.LeftIsDown() || m_clickPt == wxDefaultPosition)
        return;

    if (m_isDragging)
    {
        wxAuiTabEventSource::TabDragMotion(this, GetIdxFromWindow(m_clickTab));
        return;
    }


    int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X, this);
    int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y, this);

    if (abs(pos.x - m_clickPt.x) > drag_x_threshold ||
        abs(pos.y - m_clickPt.y) > drag_y_threshold)
    {
        const int idx = GetIdxFromWindow(m_clickTab);
        if ( idx != wxNOT_FOUND )
        {
            switch ( GetPage(idx).kind )
            {
                case wxAuiTabKind::Normal:
                case wxAuiTabKind::Pinned:
                    break;

                case wxAuiTabKind::Locked:
                    // Don't allow dragging locked tabs.
                    return;
            }
        }

        wxAuiTabEventSource::TabBeginDrag(this, idx);

        m_isDragging = true;
    }
}

void wxAuiTabCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
    auto* const hoverButton = FindHoverButton();
    if (hoverButton)
    {
        ClearButtonState(*hoverButton, wxAUI_BUTTON_STATE_HOVER);
    }

    SetHoverTab(nullptr);
}

wxAuiTabContainerButton*
wxAuiTabCtrl::FindButtonIn(wxAuiPaneButtonState state) const
{
    for ( const auto& button : m_buttons )
    {
        if ( button.curState & state )
            return const_cast<wxAuiTabContainerButton*>(&button);
    }

    for ( const auto& page : m_pages )
    {
        for ( const auto& button : page.buttons )
        {
            if ( button.curState & state )
                return const_cast<wxAuiTabContainerButton*>(&button);
        }
    }

    return nullptr;
}

bool
wxAuiTabCtrl::UpdateButtonStateAndRefresh(wxAuiTabContainerButton& button,
                                          wxAuiPaneButtonState state,
                                          bool on)
{
    int newState = button.curState;
    if ( on )
        newState |= state;
    else
        newState &= ~state;

    if ( newState == button.curState )
        return false;

    button.curState = newState;
    wxAuiBumpTabContainerRevision(this);

    Refresh();
    Update();

    return true;
}

void wxAuiTabCtrl::OnButton(int tabIdx, int button)
{
    if (button == wxAUI_BUTTON_LEFT || button == wxAUI_BUTTON_RIGHT)
    {
        if (button == wxAUI_BUTTON_LEFT)
        {
            if (GetTabOffset() > 0)
            {
                SetTabOffset(GetTabOffset()-1);
                Refresh();
                Update();
            }
        }
        else
        {
            SetTabOffset(GetTabOffset()+1);
            Refresh();
            Update();
        }
    }
    else if (button == wxAUI_BUTTON_WINDOWLIST)
    {
        const wxWeakRef<wxAuiTabCtrl> weakThis(this);
        wxAuiTabArt* const art = GetArtProvider();
        if ( !art )
            return;
        const wxAuiTabContainerSnapshot revision(
            this, m_art, wxAuiTabContainer::m_rect, m_tabOffset, m_flags,
            m_tabRowHeight,
            m_pages, m_buttons, this);
        wxAuiPageWindowProjection pageProjection;
        if ( !pageProjection.Capture(m_pages) )
            return;
        const wxAuiNotebookPageArray expectedPages = m_pages;
        wxAuiNotebookPageArray menuPages = expectedPages;
        const int activePage = GetActivePage();
        const int idx = wxAuiInvokeTabArt(
            art, [this, &menuPages, activePage](
                wxAuiTabArt* const leasedArt)
            {
                return leasedArt->ShowDropDown(
                    this, menuPages, activePage);
            });

        if ( !revision.IsAlive() || weakThis.get() != this ||
                wxWindowIsUnavailableForCallbacks(this) ||
                !pageProjection.MatchesAll(m_pages) ||
                !revision.Matches(
                    m_art, wxAuiTabContainer::m_rect, m_tabOffset, m_flags,
                    m_tabRowHeight,
                    m_pages, m_buttons) )
        {
            return;
        }

        if ( idx >= 0 &&
                static_cast<size_t>(idx) < expectedPages.GetCount() &&
                static_cast<size_t>(idx) < menuPages.GetCount() &&
                pageProjection.Matches(
                    m_pages, static_cast<size_t>(idx)) &&
                wxAuiHasSamePagePayload(
                    expectedPages[static_cast<size_t>(idx)],
                    menuPages[static_cast<size_t>(idx)]) &&
                m_pages[static_cast<size_t>(idx)].window ==
                    expectedPages[static_cast<size_t>(idx)].window )
        {
            wxAuiTabEventSource::TabClicked(this, idx);
        }
    }
    else
    {
        wxAuiTabEventSource::TabButton(this, tabIdx, button);
    }
}

void wxAuiTabCtrl::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    Refresh();
}

void wxAuiTabCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    Refresh();
}

void wxAuiTabCtrl::OnChar(wxKeyEvent& event)
{
    if (GetActivePage() == -1)
    {
        event.Skip();
        return;
    }

    // We can't leave tab processing to the system; on Windows, tabs and keys
    // get eaten by the system and not processed properly if we specify both
    // wxTAB_TRAVERSAL and wxWANTS_CHARS. And if we specify just wxTAB_TRAVERSAL,
    // we don't key arrow key events.

    int key = event.GetKeyCode();

    if (key == WXK_NUMPAD_PAGEUP)
        key = WXK_PAGEUP;
    if (key == WXK_NUMPAD_PAGEDOWN)
        key = WXK_PAGEDOWN;
    if (key == WXK_NUMPAD_HOME)
        key = WXK_HOME;
    if (key == WXK_NUMPAD_END)
        key = WXK_END;
    if (key == WXK_NUMPAD_LEFT)
        key = WXK_LEFT;
    if (key == WXK_NUMPAD_RIGHT)
        key = WXK_RIGHT;

    if (key == WXK_TAB || key == WXK_PAGEUP || key == WXK_PAGEDOWN)
    {
        bool bCtrlDown = event.ControlDown();
        bool bShiftDown = event.ShiftDown();

        bool bForward = (key == WXK_TAB && !bShiftDown) || (key == WXK_PAGEDOWN);
        bool bWindowChange = (key == WXK_PAGEUP) || (key == WXK_PAGEDOWN) || bCtrlDown;
        bool bFromTab = (key == WXK_TAB);

        if (bFromTab && !bWindowChange)
        {
            // Handle ordinary tabs via Navigate. This is needed at least for wxGTK to tab properly.
            Navigate(bForward ? wxNavigationKeyEvent::IsForward : wxNavigationKeyEvent::IsBackward);
            return;
        }

        wxAuiNotebook* nb = wxDynamicCast(GetParent(), wxAuiNotebook);
        if (!nb)
        {
            event.Skip();
            return;
        }

        wxNavigationKeyEvent keyEvent;
        keyEvent.SetDirection(bForward);
        keyEvent.SetWindowChange(bWindowChange);
        keyEvent.SetFromTab(bFromTab);
        keyEvent.SetEventObject(nb);

        if (!nb->ProcessWindowEvent(keyEvent))
        {
            // Not processed? Do an explicit tab into the page.
            wxWindow* win = GetWindowFromIdx(GetActivePage());
            if (win)
                win->SetFocus();
        }
        return;
    }

    if (m_pages.GetCount() < 2)
    {
        event.Skip();
        return;
    }

    int newPage = -1;

    int forwardKey, backwardKey;
    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {
        forwardKey = WXK_LEFT;
        backwardKey = WXK_RIGHT;
    }
    else
     {
        forwardKey = WXK_RIGHT;
        backwardKey = WXK_LEFT;
    }

    if (key == forwardKey)
    {
        if (m_pages.GetCount() > 1)
        {
            if (GetActivePage() == -1)
                newPage = 0;
            else if (GetActivePage() < (int) (m_pages.GetCount() - 1))
                newPage = GetActivePage() + 1;
        }
    }
    else if (key == backwardKey)
    {
        if (m_pages.GetCount() > 1)
        {
            if (GetActivePage() == -1)
                newPage = (int) (m_pages.GetCount() - 1);
            else if (GetActivePage() > 0)
                newPage = GetActivePage() - 1;
        }
    }
    else if (key == WXK_HOME)
    {
        newPage = 0;
    }
    else if (key == WXK_END)
    {
        newPage = (int) (m_pages.GetCount() - 1);
    }
    else
        event.Skip();

    if (newPage != -1)
    {
        wxAuiTabEventSource::TabClicked(this, newPage);
    }
    else
        event.Skip();
}

void wxAuiTabCtrl::OnDPIChanged(wxDPIChangedEvent& event)
{
    wxAuiTabArt* const art = m_art;
    wxAuiInvokeTabArt(art, [](wxAuiTabArt* const leasedArt)
    {
        leasedArt->UpdateDpi();
    });
    event.Skip();
}

// wxAuiTabFrame is an interesting case.  It's important that all child pages
// of the multi-notebook control are all actually children of that control
// (and not grandchildren).  wxAuiTabFrame facilitates this.  There is one
// instance of wxAuiTabFrame for each tab control inside the multi-notebook.
// It's important to know that wxAuiTabFrame is not a real window, but it merely
// used to capture the dimensions/positioning of the internal tab control and
// it's managed page windows

class wxAuiTabFrame : public wxWindow
{
public:

    wxAuiTabFrame(wxAuiTabCtrl* tabs, const wxSize& size, int tabCtrlHeight) :
        m_rect(size),
        m_tabs(tabs),
        m_tabsLifetime(tabs),
        m_tabsOwner(tabs ? tabs->GetParent() : nullptr),
        m_tabsOwnerLifetime(m_tabsOwner),
        m_tabCtrlHeight(tabCtrlHeight)
    {
    }

    ~wxAuiTabFrame()
    {
        // use pending delete because sometimes during
        // window closing, refreshes are pending
        wxAuiTabCtrl* const tabs = m_tabsLifetime.get();
        wxWindow* const owner = m_tabsOwnerLifetime.get();
        if (tabs == m_tabs && owner == m_tabsOwner && owner &&
                !wxWindowIsUnavailableForCallbacks(owner) &&
                tabs->GetParent() == owner &&
                !wxPendingDelete.Member(m_tabs))
            wxPendingDelete.Append(m_tabs);
    }

    void SetTabCtrlHeight(int h)
    {
        m_tabCtrlHeight = h;
    }

    // As we don't have a valid HWND, base class implementations of these
    // functions don't work for this window, so override them to forward to the
    // real window.
    wxSize GetDPI() const override
    {
        wxAuiTabCtrl* const tabs = m_tabsLifetime.get();
        wxWindow* const owner = m_tabsOwnerLifetime.get();
        return tabs == m_tabs && owner == m_tabsOwner && owner &&
                       !wxWindowIsUnavailableForCallbacks(owner) &&
                       tabs->GetParent() == owner
                    ? tabs->GetDPI()
                    : wxSize(96, 96);
    }

    wxLayoutDirection GetLayoutDirection() const override
    {
        wxAuiTabCtrl* const tabs = m_tabsLifetime.get();
        wxWindow* const owner = m_tabsOwnerLifetime.get();
        return tabs == m_tabs && owner == m_tabsOwner && owner &&
                       !wxWindowIsUnavailableForCallbacks(owner) &&
                       tabs->GetParent() == owner
                    ? tabs->GetLayoutDirection()
                    : wxLayout_Default;
    }

protected:
    void DoSetSize(int x, int y,
                   int width, int height,
                   int WXUNUSED(sizeFlags = wxSIZE_AUTO)) override
    {
        m_rect = wxRect(x, y, width, height);
        DoSizing();
    }

    void DoGetClientSize(int* x, int* y) const override
    {
        *x = m_rect.width;
        *y = m_rect.height;
    }

public:
    bool Show( bool WXUNUSED(show = true) ) override { return false; }

    void DoSizing()
    {
        if ( wxWindowIsUnavailableForCallbacks(this) )
            return;

        wxAuiTabCtrl* const tabs = m_tabsLifetime.get();
        if ( !tabs || tabs != m_tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) )
            return;

        wxWindow* const owner = m_tabsOwnerLifetime.get();
        if ( !owner || owner != m_tabsOwner ||
                wxWindowIsUnavailableForCallbacks(owner) ||
                tabs->GetParent() != owner )
        {
            return;
        }

        wxAuiNotebook* const book = wxDynamicCast(owner, wxAuiNotebook);
        if ( !book || wxWindowIsUnavailableForCallbacks(book) )
            return;

        const wxWeakRef<wxAuiTabFrame> weakThis(this);
        const wxWeakRef<wxAuiTabCtrl> weakTabs(tabs);
        const wxWeakRef<wxAuiNotebook> weakBook(book);
        const wxRect frameRect = m_rect;
        const int tabCtrlHeight = m_tabCtrlHeight;

        if (tabs->IsFrozen() || book->IsFrozen())
            return;

        struct PageSnapshot
        {
            explicit PageSnapshot(wxWindow* const page_)
                : page(page_), lifetime(page_)
            {
            }

            wxWindow* const page;
            const wxWeakRef<wxWindow> lifetime;
        };

        std::vector<PageSnapshot> pages;
        pages.reserve(tabs->GetPageCount());
        for ( const wxAuiNotebookPage& page : tabs->GetPages() )
            pages.emplace_back(page.window);

        const auto hasExactLayoutRevision = [&]() -> wxAuiTabFrame*
        {
            wxAuiTabFrame* const frame = weakThis.get();
            wxAuiNotebook* const currentBook = weakBook.get();
            if ( !frame || wxWindowIsUnavailableForCallbacks(frame) ||
                    weakTabs.get() != tabs || !currentBook ||
                    wxWindowIsUnavailableForCallbacks(tabs) ||
                    wxWindowIsUnavailableForCallbacks(currentBook) ||
                    tabs->GetParent() != currentBook ||
                    frame->m_tabs != tabs || frame->m_rect != frameRect ||
                    frame->m_tabCtrlHeight != tabCtrlHeight ||
                    tabs->GetPageCount() != pages.size() )
            {
                return nullptr;
            }

            for ( size_t i = 0; i < pages.size(); ++i )
            {
                const PageSnapshot& expected = pages[i];
                if ( expected.lifetime.get() != expected.page ||
                        wxWindowIsUnavailableForCallbacks(expected.page) ||
                        expected.page->GetParent() != currentBook ||
                        tabs->GetWindowFromIdx(i) != expected.page )
                {
                    return nullptr;
                }
            }
            return frame;
        };

        tabs->DoApplyRect(frameRect, tabCtrlHeight);
        wxAuiTabFrame* frame = hasExactLayoutRevision();
        if ( !frame )
            return;

        frame->m_tab_rect = tabs->GetRect();
        const wxRect tabRect = frame->m_tab_rect;
        wxAuiTabArt* const art = tabs->GetArtProvider();
        wxAuiTabArtLease artLease(art);

        for ( const PageSnapshot& page : pages )
        {
            const int borderSpace =
                art->GetAdditionalBorderSpace(page.page);
            frame = hasExactLayoutRevision();
            if ( !frame || tabs->GetArtProvider() != art ||
                    frame->m_tab_rect != tabRect )
            {
                return;
            }

            int height = frameRect.height - tabRect.height - borderSpace;
            if ( height < 0 )
            {
                // avoid passing negative height to wxWindow::SetSize(), this
                // results in assert failures/GTK+ warnings
                height = 0;
            }
            int width = frameRect.width - 2 * borderSpace;
            if (width < 0)
                width = 0;

            if (tabs->IsFlagSet(wxAUI_NB_BOTTOM))
            {
                page.page->SetSize(frameRect.x + borderSpace,
                                   frameRect.y + borderSpace,
                                   width, height);
            }
            else //TODO: if (IsFlagSet(wxAUI_NB_TOP))
            {
                page.page->SetSize(frameRect.x + borderSpace,
                                   frameRect.y + tabRect.height,
                                   width, height);
            }
            if ( !hasExactLayoutRevision() )
                return;
            // TODO: else if (IsFlagSet(wxAUI_NB_LEFT)){}
            // TODO: else if (IsFlagSet(wxAUI_NB_RIGHT)){}
        }
    }

protected:
    void DoGetSize(int* x, int* y) const override
    {
        if (x)
            *x = m_rect.GetWidth();
        if (y)
            *y = m_rect.GetHeight();
    }

public:
    void Update() override
    {
        // does nothing
    }

    wxRect m_rect;
    wxRect m_tab_rect;
    wxAuiTabCtrl* const m_tabs;
    const wxWeakRef<wxAuiTabCtrl> m_tabsLifetime;
    wxWindow* const m_tabsOwner;
    const wxWeakRef<wxWindow> m_tabsOwnerLifetime;
    int m_tabCtrlHeight = 0;
};


const int wxAuiBaseTabCtrlId = 5380;


// -- wxAuiNotebook class implementation --

// More convenient version of FindTab(): returns all the results instead of
// requiring output parameters for returning some of them.
//
// Note that TabInfo returned by FindTab() is normally always valid.
struct wxAuiNotebook::TabInfo : wxAuiNotebookPosition
{
    TabInfo() = default;

    TabInfo(wxAuiTabCtrl* tabCtrl_, int tabIdx_, wxAuiNotebookPage* info)
        : wxAuiNotebookPosition{tabCtrl_, tabIdx_}, pageInfo(info)
    {
    }

    // Information about the page or nullptr if not found.
    wxAuiNotebookPage* pageInfo = nullptr;
};

wxBEGIN_EVENT_TABLE(wxAuiNotebook, wxBookCtrlBase)
    EVT_SIZE(wxAuiNotebook::OnSize)
    EVT_CHILD_FOCUS(wxAuiNotebook::OnChildFocusNotebook)
    EVT_NAVIGATION_KEY(wxAuiNotebook::OnNavigationKeyNotebook)
    EVT_SYS_COLOUR_CHANGED(wxAuiNotebook::OnSysColourChanged)
    EVT_DPI_CHANGED(wxAuiNotebook::OnDPIChanged)
wxEND_EVENT_TABLE()

namespace
{

// wxAuiNotebook always adds a special dummy pane with this name.
constexpr const char* const DUMMY_PANE_NAME = "dummy";

// Check if this is a dummy pane.
bool IsDummyPane(const wxAuiPaneInfo& pane)
{
    return pane.name == DUMMY_PANE_NAME;
}

} // anonymous namespace

void wxAuiNotebook::OnSysColourChanged(wxSysColourChangedEvent &event)
{
    event.Skip(true);

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    wxAuiTabArt* const canonicalArt = m_tabs.GetArtProvider();
    if ( !canonicalArt )
        return;
    const wxAuiTabContainerSnapshot canonicalRevision(
        &m_tabs, m_tabs.m_art, m_tabs.m_rect, m_tabs.m_tabOffset,
        m_tabs.m_flags, m_tabs.m_tabRowHeight, m_tabs.m_pages,
        m_tabs.m_buttons, this);

    struct FrameColourRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiTabArt* art;
        std::unique_ptr<wxAuiTabContainerSnapshot> tabsRevision;
    };

    // Materialize the entire manager projection before entering the first
    // virtual provider. Never retain a live pane iterator across a callback.
    std::vector<FrameColourRevision> frames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        if ( !tabs || frame->m_tabs != tabs ||
                frame->m_tabsOwner != this ||
                frame->m_tabsOwnerLifetime.get() != this ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != this || !tabs->m_art )
        {
            return;
        }

        frames.push_back(
        {
            frame,
            wxWeakRef<wxAuiTabFrame>(frame),
            tabs,
            wxWeakRef<wxAuiTabCtrl>(tabs),
            tabs->m_art,
            std::unique_ptr<wxAuiTabContainerSnapshot>(
                new wxAuiTabContainerSnapshot(
                    tabs, tabs->m_art, tabs->m_rect, tabs->m_tabOffset,
                    tabs->m_flags, tabs->m_tabRowHeight, tabs->m_pages,
                    tabs->m_buttons, tabs))
        });
    }
    const size_t paneCount = m_mgr.GetAllPanes().GetCount();

    const auto hasExactRevision = [&]() -> wxAuiNotebook*
    {
        // The standalone canonical container token is checked before any
        // access through this potentially destroyed notebook.
        if ( !canonicalRevision.IsAlive() )
            return nullptr;

        wxAuiNotebook* const book = weakThis.get();
        if ( book != this || !book ||
                wxWindowIsUnavailableForCallbacks(book) )
        {
            return nullptr;
        }
        if ( !canonicalRevision.Matches(
                book->m_tabs.m_art, book->m_tabs.m_rect,
                book->m_tabs.m_tabOffset, book->m_tabs.m_flags,
                book->m_tabs.m_tabRowHeight, book->m_tabs.m_pages,
                book->m_tabs.m_buttons) ||
                book->m_mgr.GetAllPanes().GetCount() != paneCount )
        {
            return nullptr;
        }

        size_t frameIndex = 0;
        for ( const auto& pane : book->m_mgr.GetAllPanes() )
        {
            if ( IsDummyPane(pane) )
                continue;
            if ( frameIndex >= frames.size() ||
                    pane.window != frames[frameIndex].frame )
            {
                return nullptr;
            }
            ++frameIndex;
        }
        if ( frameIndex != frames.size() )
            return nullptr;

        for ( const FrameColourRevision& expected : frames )
        {
            if ( expected.frameLifetime.get() != expected.frame ||
                    expected.tabsLifetime.get() != expected.tabs ||
                    wxWindowIsUnavailableForCallbacks(expected.frame) ||
                    wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                    expected.frame->m_tabs != expected.tabs ||
                    expected.frame->m_tabsOwner != book ||
                    expected.frame->m_tabsOwnerLifetime.get() != book ||
                    expected.tabs->GetParent() != book ||
                    book->GetTabFrameFromTabCtrl(expected.tabs) !=
                        expected.frame ||
                    !expected.tabsRevision->IsAlive() )
            {
                return nullptr;
            }
            if ( !expected.tabsRevision->Matches(
                    expected.tabs->m_art, expected.tabs->m_rect,
                    expected.tabs->m_tabOffset, expected.tabs->m_flags,
                    expected.tabs->m_tabRowHeight, expected.tabs->m_pages,
                    expected.tabs->m_buttons) )
            {
                return nullptr;
            }
        }
        return book;
    };

    if ( !hasExactRevision() )
        return;
    wxAuiInvokeTabArt(canonicalArt, [](wxAuiTabArt* const leasedArt)
    {
        leasedArt->UpdateColoursFromSystem();
    });
    wxAuiNotebook* book = hasExactRevision();
    if ( !book )
        return;

    for ( const FrameColourRevision& expected : frames )
    {
        wxAuiInvokeTabArt(expected.art, [](wxAuiTabArt* const leasedArt)
        {
            leasedArt->UpdateColoursFromSystem();
        });
        book = hasExactRevision();
        if ( !book )
            return;
        expected.tabs->Refresh();
        if ( !hasExactRevision() )
            return;
    }
    book->Refresh();
}

void wxAuiNotebook::OnDPIChanged(wxDPIChangedEvent& event)
{
    UpdateTabCtrlHeight();
    event.Skip();
}

void wxAuiNotebook::Init()
{
    m_curPage = wxNOT_FOUND;
    m_tabIdCounter = wxAuiBaseTabCtrlId;
    m_dummyWnd = nullptr;
    m_requestedBmpSize = wxDefaultSize;
    m_requestedTabCtrlHeight = -1;
}

bool wxAuiNotebook::Create(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
{
    if (!wxControl::Create(parent, id, pos, size, style))
        return false;

    InitNotebook(style);

    return true;
}

// InitNotebook() contains common initialization
// code called by all constructors
void wxAuiNotebook::InitNotebook(long style)
{
    SetName(wxT("wxAuiNotebook"));
    m_curPage = wxNOT_FOUND;
    m_tabIdCounter = wxAuiBaseTabCtrlId;
    m_dummyWnd = nullptr;
    m_flags = (unsigned int)style;
    m_tabCtrlHeight = FromDIP(20);

    m_normalFont = *wxNORMAL_FONT;
    m_selectedFont = *wxNORMAL_FONT;
    m_selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    SetArtProvider(new wxAuiDefaultTabArt);

    m_dummyWnd = new wxWindow(this, wxID_ANY, wxPoint(0,0), wxSize(0,0));
    m_dummyWnd->SetSize(FromDIP(wxSize(200, 200)));
    m_dummyWnd->Show(false);

    m_mgr.SetManagedWindow(this);
    m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
    m_mgr.SetDockSizeConstraint(1.0, 1.0); // no dock size constraint

    m_mgr.AddPane(m_dummyWnd,
              wxAuiPaneInfo().Name(DUMMY_PANE_NAME).Bottom().CaptionVisible(false).Show(false));

    m_mgr.Update();
}

wxAuiTabFrame* wxAuiNotebook::CreateTabFrame(wxSize size)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    wxAuiTabArt* const sourceArt = m_tabs.GetArtProvider();
    // Bridge the allocation-to-weak-reference gap with ordinary ownership.
    // Once the weak witness exists, callback-aware cleanup below becomes the
    // sole arbiter because Create()/SetFlags()/Clone() may destroy or reparent
    // the control synchronously.
    std::unique_ptr<wxAuiTabCtrl> allocatedTabs(new wxAuiTabCtrl);
    auto* const tabs = allocatedTabs.get();
    const wxWeakRef<wxAuiTabCtrl> weakTabs(tabs);
    allocatedTabs.release();
    const auto discardUnclaimedTabs = [&]()
    {
        wxAuiTabCtrl* const liveTabs = weakTabs.get();
        if ( liveTabs != tabs || !liveTabs ||
                wxWindowIsUnavailableForCallbacks(liveTabs) )
        {
            return;
        }
        wxWindow* const parent = liveTabs->GetParent();
        wxAuiNotebook* const book = weakThis.get();
        if ( !parent ||
                (book && !wxWindowIsUnavailableForCallbacks(book) &&
                 parent == book) )
        {
            delete liveTabs;
        }
    };
    wxScopeGuard discardGuard = wxMakeGuard(discardUnclaimedTabs);
    const wxWindowID tabId = m_tabIdCounter++;
    if ( !tabs->Create(this, tabId) )
        return nullptr;

    wxAuiNotebook* book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakTabs.get() != tabs ||
            wxWindowIsUnavailableForCallbacks(tabs) ||
            tabs->GetParent() != book ||
            book->m_tabs.GetArtProvider() != sourceArt )
    {
        return nullptr;
    }

    tabs->SetFlags(m_flags);

    book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakTabs.get() != tabs ||
            wxWindowIsUnavailableForCallbacks(tabs) ||
            tabs->GetParent() != book ||
            book->m_tabs.GetArtProvider() != sourceArt )
    {
        return nullptr;
    }

    wxAuiTabArtStaging clonedArt(wxAuiInvokeTabArt(
        sourceArt, [](wxAuiTabArt* const leasedArt)
        {
            return leasedArt->Clone();
        }));
    book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakTabs.get() != tabs ||
            wxWindowIsUnavailableForCallbacks(tabs) ||
            tabs->GetParent() != book ||
            book->m_tabs.GetArtProvider() != sourceArt || !clonedArt.Get() ||
            !clonedArt.StillOwnsArt() )
    {
        return nullptr;
    }

    tabs->SetArtProvider(clonedArt.Get());
    book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakTabs.get() != tabs ||
            wxWindowIsUnavailableForCallbacks(tabs) ||
            tabs->GetParent() != book ||
            !wxAuiIsTabArtOwnedBy(clonedArt.Get(), tabs) )
    {
        return nullptr;
    }

    wxAuiTabFrame* const frame =
        new wxAuiTabFrame(tabs, size, book->m_tabCtrlHeight);
    discardGuard.Dismiss();
    return frame;
}

wxAuiNotebook::~wxAuiNotebook()
{
    wxScopeGuard forgetMutationEpochs = wxMakeGuard([this]()
    {
        wxAuiForgetNotebookMutationEpochs(this);
    });
    wxUnusedVar(forgetMutationEpochs);

    // Block every callback-reentrant topology writer for the entire derived
    // teardown, while the AUI projections and manager still exist.
    const wxAuiNotebookTopologyTransaction teardownTransaction(this);

    struct FrameSnapshot
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
    };

    // Snapshot the manager-owned pseudo frames before the destroy event. The
    // event is an application callback boundary, even though topology writers
    // are rejected as soon as destruction begins.
    std::vector<FrameSnapshot> frames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) || !pane.window )
            continue;

        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        // The tab control can already have been destroyed by a callback while
        // its pseudo frame is still present in the manager. Never create a new
        // weak reference from that potentially stale raw pointer: the frame's
        // lifetime token is the authoritative witness.
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs)});
    }

    wxWindow* const dummy = m_dummyWnd;
    const wxWeakRef<wxWindow> dummyLifetime(dummy);

    // wxAuiManager::OnDestroy() unbinds itself and then forwards the same
    // destroy event back to the managed window.  During an already active
    // dynamic-handler dispatch this can make later wxEVT_DESTROY handlers run
    // twice.  Unbind the manager first: its pane projection remains available
    // for the explicit detach loop below, while application observers receive
    // exactly one event with the notebook already unavailable to writers.
    m_mgr.UnInit();

    // Indicate we're deleting pages
    SendDestroyEvent();

    // Don't use the public deletion path after SendDestroyEvent(): at this
    // point the notebook is intentionally unavailable for callbacks and the
    // transactional DoRemovePage() must reject ordinary writers. Publish an
    // empty controller model without invoking art, layout or selection code;
    // DestroyChildren() below then synchronously destroys all child pages and
    // tab controls while the wxAuiNotebook members still exist.
    //
    // Clear the display projections before the canonical model so that no
    // live tab control ever retains a page identity absent from m_tabs, even
    // if manager teardown examines its panes.
    for ( const FrameSnapshot& expected : frames )
    {
        wxAuiTabFrame* const frame = expected.frameLifetime.get();
        wxAuiTabCtrl* const tabs = expected.tabsLifetime.get();
        if ( frame != expected.frame || tabs != expected.tabs ||
                !frame || !tabs || frame->m_tabs != tabs ||
                frame->m_tabsOwner != this ||
                frame->m_tabsOwnerLifetime.get() != this )
        {
            continue;
        }

        // The control can already have been transferred to another live
        // owner. It must survive in that case, but its page projection still
        // belongs to this notebook and would otherwise retain windows which
        // DestroyChildren() is about to delete.
        tabs->m_pages.Clear();
        wxAuiBumpTabContainerRevision(tabs);
    }

    m_tabs.m_pages.Clear();
    wxAuiBumpTabContainerRevision(&m_tabs);
    m_curPage = wxNOT_FOUND;

    // wxAuiManager::UnInit() only unbinds the managed window: it doesn't own
    // or destroy panes. Detach and delete every still-live pseudo frame while
    // the manager projection is still available. Because the original owner
    // is already being deleted, frame destruction deliberately does not queue
    // its tab control: DestroyChildren() below is the single ownership arbiter
    // and a child callback may still transfer the control to another owner.
    for ( const FrameSnapshot& expected : frames )
    {
        wxAuiTabFrame* const frame = expected.frameLifetime.get();
        if ( frame != expected.frame || !frame )
            continue;

        if ( m_mgr.GetPane(frame).IsOk() )
            m_mgr.DetachPane(frame);

        if ( expected.frameLifetime.get() == expected.frame )
            delete expected.frame;
    }

    if ( dummy && dummyLifetime.get() == dummy &&
            m_mgr.GetPane(dummy).IsOk() )
    {
        m_mgr.DetachPane(dummy);
    }
    m_dummyWnd = nullptr;

    // This is intentionally earlier than the wxWindow base destructor: page
    // destroy handlers may still query their parent book, whose derived model
    // must remain alive (and already be observably empty) until they finish.
    DestroyChildren();
}

void wxAuiNotebook::SetArtProvider(wxAuiTabArt* art)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    // Art ownership is independent from its mutable metrics. A font or
    // sizing callback may supersede those values, but only a newer
    // SetArtProvider() may stop this provider from reaching every live tab
    // control.
    const wxAuiNotebookArtEpoch artEpoch =
        wxAuiBumpNotebookArtEpoch(this);
    wxAuiBumpNotebookFontEpoch(this);
    const wxAuiNotebookFontEpoch normalFontEpoch =
        wxAuiBumpNotebookNormalFontEpoch(this);
    const wxAuiNotebookFontEpoch selectedFontEpoch =
        wxAuiBumpNotebookSelectedFontEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_tabs.SetArtProvider(art);

    const auto hasCurrentCanonicalArt = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && !wxWindowIsUnavailableForCallbacks(book) &&
                       wxAuiGetNotebookArtEpoch(book) == artEpoch &&
                       book->m_tabs.GetArtProvider() == art
                    ? book
                    : nullptr;
    };

    wxAuiNotebook* book = hasCurrentCanonicalArt();
    if ( !book )
        return;

    // If the art provider implements GetXXXFont() functions, use them.
    // Otherwise keep using our own fonts. Each component has its own token:
    // a nested SetNormalFont() suppresses only the stale normal-font result,
    // while a nested SetFont() supersedes both queried values.
    wxFont font = wxAuiInvokeTabArt(
        art, [](wxAuiTabArt* const leasedArt)
        {
            return leasedArt->GetNormalFont();
        });
    book = hasCurrentCanonicalArt();
    if ( !book )
        return;
    if ( wxAuiGetNotebookNormalFontEpoch(book) == normalFontEpoch &&
            font.IsOk() )
    {
        book->m_normalFont = font;
    }

    font = wxAuiInvokeTabArt(
        art, [](wxAuiTabArt* const leasedArt)
        {
            return leasedArt->GetSelectedFont();
        });
    book = hasCurrentCanonicalArt();
    if ( !book )
        return;
    if ( wxAuiGetNotebookSelectedFontEpoch(book) == selectedFontEpoch &&
            font.IsOk() )
    {
        book->m_selectedFont = font;
    }

    // Font callbacks during reconciliation are allowed to publish a newer
    // font. Restart from the latest component values in that case. The bound
    // prevents a malicious provider continuously writing from creating a
    // synchronous livelock; regardless of the bound, all installed art
    // providers are already the canonical kind after each completed art pass.
    constexpr unsigned int maxReconcileAttempts = 4;
    for ( unsigned int attempt = 0;
          attempt < maxReconcileAttempts;
          ++attempt )
    {
        book = hasCurrentCanonicalArt();
        if ( !book )
            return;

        const auto reconcileCanonicalNormal = [&]() -> bool
        {
            for ( unsigned int retry = 0;
                  retry < maxReconcileAttempts;
                  ++retry )
            {
                wxAuiNotebook* current = hasCurrentCanonicalArt();
                if ( !current )
                    return false;
                const wxAuiNotebookFontEpoch epoch =
                    wxAuiGetNotebookNormalFontEpoch(current);
                const wxFont latestFont = current->m_normalFont;
                wxAuiInvokeTabArt(
                    art, [&latestFont](wxAuiTabArt* const leasedArt)
                    {
                        leasedArt->SetNormalFont(latestFont);
                    });
                current = hasCurrentCanonicalArt();
                if ( !current )
                    return false;
                if ( wxAuiGetNotebookNormalFontEpoch(current) == epoch )
                    return true;
            }
            return false;
        };
        const auto reconcileCanonicalSelected = [&]() -> bool
        {
            for ( unsigned int retry = 0;
                  retry < maxReconcileAttempts;
                  ++retry )
            {
                wxAuiNotebook* current = hasCurrentCanonicalArt();
                if ( !current )
                    return false;
                const wxAuiNotebookFontEpoch epoch =
                    wxAuiGetNotebookSelectedFontEpoch(current);
                const wxFont latestFont = current->m_selectedFont;
                wxAuiInvokeTabArt(
                    art, [&latestFont](wxAuiTabArt* const leasedArt)
                    {
                        leasedArt->SetSelectedFont(latestFont);
                    });
                current = hasCurrentCanonicalArt();
                if ( !current )
                    return false;
                if ( wxAuiGetNotebookSelectedFontEpoch(current) == epoch )
                    return true;
            }
            return false;
        };
        (void)reconcileCanonicalNormal();
        if ( !hasCurrentCanonicalArt() )
            return;
        (void)reconcileCanonicalSelected();
        if ( !hasCurrentCanonicalArt() )
            return;

        // Calculate from the latest art/font/sizing inputs. If a callback
        // writes a newer input, UpdateTabCtrlHeight() aborts its own stale
        // publication and the epoch check at the end repeats this pass.
        const wxAuiNotebookHeightInputEpoch heightEpochBeforeUpdate =
            wxAuiGetNotebookHeightInputEpoch(book);
        book->UpdateTabCtrlHeight();
        book = hasCurrentCanonicalArt();
        if ( !book )
            return;
        const wxAuiNotebookHeightInputEpoch heightInputEpoch =
            wxAuiGetNotebookHeightInputEpoch(book);
        const bool heightSupplantedDuringUpdate =
            heightInputEpoch != heightEpochBeforeUpdate;

        wxAuiPageProjection canonicalPages;
        if ( !wxAuiCapturePageProjection(book->m_tabs, book,
                                         canonicalPages) )
        {
            return;
        }

        struct FrameRevision
        {
            wxAuiTabFrame* frame;
            wxWeakRef<wxAuiTabFrame> frameLifetime;
            wxAuiTabCtrl* tabs;
            wxWeakRef<wxAuiTabCtrl> tabsLifetime;
            wxAuiPageProjection pages;
            wxAuiTabArt* expectedArt;
        };

        std::vector<FrameRevision> frames;
        for ( const auto& pane : book->m_mgr.GetAllPanes() )
        {
            if ( IsDummyPane(pane) )
                continue;

            auto* const frame =
                static_cast<wxAuiTabFrame*>(pane.window);
            if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
                return;
            wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
            wxAuiPageProjection pages;
            if ( !tabs || frame->m_tabs != tabs ||
                    wxWindowIsUnavailableForCallbacks(tabs) ||
                    tabs->GetParent() != book ||
                    !wxAuiCapturePageProjection(*tabs, book, pages) )
            {
                return;
            }
            frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                              tabs, wxWeakRef<wxAuiTabCtrl>(tabs),
                              std::move(pages), tabs->GetArtProvider()});
        }

        const size_t paneCount = book->m_mgr.GetAllPanes().GetCount();
        const auto hasExactArtRevision = [&]() -> wxAuiNotebook*
        {
            wxAuiNotebook* const current = hasCurrentCanonicalArt();
            if ( !current ||
                    current->m_mgr.GetAllPanes().GetCount() != paneCount ||
                    !wxAuiMatchesPageProjection(current->m_tabs, current,
                                                canonicalPages) )
            {
                return nullptr;
            }

            for ( const FrameRevision& expected : frames )
            {
                if ( expected.frameLifetime.get() != expected.frame ||
                        expected.tabsLifetime.get() != expected.tabs ||
                        wxWindowIsUnavailableForCallbacks(expected.frame) ||
                        wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                        expected.frame->m_tabs != expected.tabs ||
                        expected.tabs->GetParent() != current ||
                        expected.tabs->GetArtProvider() !=
                            expected.expectedArt ||
                        current->GetTabFrameFromTabCtrl(expected.tabs) !=
                            expected.frame ||
                        !wxAuiMatchesPageProjection(
                            *expected.tabs, current, expected.pages) )
                {
                    return nullptr;
                }
            }
            return current;
        };

        // Prepare every replacement before publishing any of them. Clone()
        // and font setters are virtual; font callbacks are accepted, but an
        // art/topology callback invalidates this captured pass.
        std::vector<wxAuiTabArtStaging> clones;
        clones.reserve(frames.size());
        for ( size_t i = 0; i < frames.size(); ++i )
        {
            clones.emplace_back(wxAuiInvokeTabArt(
                art, [](wxAuiTabArt* const leasedArt)
                {
                    return leasedArt->Clone();
                }));
            book = hasExactArtRevision();
            if ( !clones.back().Get() || !clones.back().StillOwnsArt() ||
                    !book )
                return;

            const wxFont normalFont = book->m_normalFont;
            const wxFont selectedFont = book->m_selectedFont;
            wxAuiInvokeTabArt(
                clones.back().Get(),
                [&normalFont](wxAuiTabArt* const leasedArt)
                {
                    leasedArt->SetNormalFont(normalFont);
                });
            if ( !clones.back().StillOwnsArt() )
                return;
            book = hasExactArtRevision();
            if ( !book )
                return;
            wxAuiInvokeTabArt(
                clones.back().Get(),
                [&selectedFont](wxAuiTabArt* const leasedArt)
                {
                    leasedArt->SetSelectedFont(selectedFont);
                });
            if ( !clones.back().StillOwnsArt() )
                return;
            book = hasExactArtRevision();
            if ( !book )
                return;
            const wxSize tabSize = frames[i].tabs->m_rect.GetSize();
            const size_t pageCount = frames[i].tabs->m_pages.GetCount();
            wxAuiInvokeTabArt(
                clones.back().Get(),
                [&tabSize, pageCount, &frames, i](
                    wxAuiTabArt* const leasedArt)
                {
                    leasedArt->SetSizingInfo(
                        tabSize, pageCount, frames[i].tabs);
                });
            if ( !clones.back().StillOwnsArt() ||
                    !hasExactArtRevision() )
                return;
        }

        for ( size_t i = 0; i < frames.size(); ++i )
        {
            if ( !clones[i].StillOwnsArt() )
                return;
            wxAuiTabArt* const clone = clones[i].Get();
            frames[i].tabs->SetArtProvider(clone);
            frames[i].expectedArt = clone;
            if ( !wxAuiIsTabArtOwnedBy(clone, frames[i].tabs) ||
                    !hasExactArtRevision() )
                return;
        }

        const auto reconcileNormalFont = [&]() -> bool
        {
            for ( unsigned int retry = 0;
                  retry < maxReconcileAttempts;
                  ++retry )
            {
                wxAuiNotebook* current = hasExactArtRevision();
                if ( !current )
                    return false;
                const wxAuiNotebookFontEpoch epoch =
                    wxAuiGetNotebookNormalFontEpoch(current);
                const wxFont latestFont = current->m_normalFont;

                wxAuiInvokeTabArt(
                    art, [&latestFont](wxAuiTabArt* const leasedArt)
                    {
                        leasedArt->SetNormalFont(latestFont);
                    });
                current = hasExactArtRevision();
                if ( !current )
                    return false;
                if ( wxAuiGetNotebookNormalFontEpoch(current) != epoch )
                    continue;

                bool restart = false;
                for ( const FrameRevision& expected : frames )
                {
                    expected.tabs->SetNormalFont(latestFont);
                    current = hasExactArtRevision();
                    if ( !current )
                        return false;
                    if ( wxAuiGetNotebookNormalFontEpoch(current) != epoch )
                    {
                        restart = true;
                        break;
                    }
                }
                if ( !restart )
                    return true;
            }
            return false;
        };

        const auto reconcileSelectedFont = [&]() -> bool
        {
            for ( unsigned int retry = 0;
                  retry < maxReconcileAttempts;
                  ++retry )
            {
                wxAuiNotebook* current = hasExactArtRevision();
                if ( !current )
                    return false;
                const wxAuiNotebookFontEpoch epoch =
                    wxAuiGetNotebookSelectedFontEpoch(current);
                const wxFont latestFont = current->m_selectedFont;

                wxAuiInvokeTabArt(
                    art, [&latestFont](wxAuiTabArt* const leasedArt)
                    {
                        leasedArt->SetSelectedFont(latestFont);
                    });
                current = hasExactArtRevision();
                if ( !current )
                    return false;
                if ( wxAuiGetNotebookSelectedFontEpoch(current) != epoch )
                    continue;

                bool restart = false;
                for ( const FrameRevision& expected : frames )
                {
                    expected.tabs->SetSelectedFont(latestFont);
                    current = hasExactArtRevision();
                    if ( !current )
                        return false;
                    if ( wxAuiGetNotebookSelectedFontEpoch(current) != epoch )
                    {
                        restart = true;
                        break;
                    }
                }
                if ( !restart )
                    return true;
            }
            return false;
        };

        const bool normalReconciled = reconcileNormalFont();
        if ( !hasExactArtRevision() )
            return;
        const bool selectedReconciled = reconcileSelectedFont();
        if ( !normalReconciled || !selectedReconciled )
            continue;

        book = hasExactArtRevision();
        if ( !book )
            return;
        if ( !heightSupplantedDuringUpdate &&
                wxAuiGetNotebookHeightInputEpoch(book) ==
                    heightInputEpoch )
        {
            return;
        }
    }
}

// SetTabCtrlHeight() is the highest-level override of the
// tab height.  A call to this function effectively enforces a
// specified tab ctrl height, overriding all other considerations,
// such as text or bitmap height.  It overrides any call to
// SetUniformBitmapSize().  Specifying a height of -1 reverts
// any previous call and returns to the default behaviour

void wxAuiNotebook::SetTabCtrlHeight(int height)
{
    // Bump before publication so an outer sizing callback can never overwrite
    // this request, even when the nested calculation keeps the same height.
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_requestedTabCtrlHeight = height;

    // if window is already initialized, recalculate the tab height
    if (m_dummyWnd)
    {
        UpdateTabCtrlHeight();
    }
}


// SetUniformBitmapSize() ensures that all tabs will have
// the same height, even if some tabs don't have bitmaps
// Passing wxDefaultSize to this function will instruct
// the control to use dynamic tab height-- so when a tab
// with a large bitmap is added, the tab ctrl's height will
// automatically increase to accommodate the bitmap

void wxAuiNotebook::SetUniformBitmapSize(const wxSize& size)
{
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_requestedBmpSize = size;

    // if window is already initialized, recalculate the tab height
    if (m_dummyWnd)
    {
        UpdateTabCtrlHeight();
    }
}

// UpdateTabCtrlHeight() does the actual tab resizing. It's meant
// to be used internally
bool wxAuiNotebook::UpdateTabCtrlHeight()
{
    if ( wxWindowIsUnavailableForCallbacks(this) )
        return false;

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    wxAuiTabArt* const art = m_tabs.GetArtProvider();
    const wxAuiNotebookHeightInputEpoch heightInputEpoch =
        wxAuiGetNotebookHeightInputEpoch(this);
    const wxSize requestedBmpSize = m_requestedBmpSize;
    const int requestedTabCtrlHeight = m_requestedTabCtrlHeight;
    int expectedPublishedHeight = m_tabCtrlHeight;
    wxAuiPageProjection pages;
    if ( !wxAuiCapturePageProjection(m_tabs, this, pages) )
        return false;

    const auto hasExactCanonicalRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                book->m_tabs.GetArtProvider() != art ||
                wxAuiGetNotebookHeightInputEpoch(book) != heightInputEpoch ||
                book->m_requestedBmpSize != requestedBmpSize ||
                book->m_requestedTabCtrlHeight != requestedTabCtrlHeight ||
                book->m_tabCtrlHeight != expectedPublishedHeight ||
                !wxAuiMatchesPageProjection(book->m_tabs, book, pages) )
        {
            return nullptr;
        }
        return book;
    };

    wxAuiNotebook* book = this;

    struct FrameRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
    };
    std::vector<FrameRevision> frames;
    for ( const auto& pane : book->m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return false;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        if ( !tabs || wxWindowIsUnavailableForCallbacks(tabs) ||
                frame->m_tabs != tabs || tabs->GetParent() != book )
        {
            return false;
        }
        wxAuiPageProjection tabPages;
        if ( !wxAuiCapturePageProjection(*tabs, book, tabPages) )
            return false;
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs),
                          std::move(tabPages)});
    }

    const size_t paneCount = book->m_mgr.GetAllPanes().GetCount();
    const auto hasExactManagerRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = hasExactCanonicalRevision();
        if ( !current ||
                current->m_mgr.GetAllPanes().GetCount() != paneCount )
        {
            return nullptr;
        }
        for ( size_t i = 0; i < frames.size(); ++i )
        {
            const FrameRevision& expected = frames[i];
            if ( expected.frameLifetime.get() != expected.frame ||
                    expected.tabsLifetime.get() != expected.tabs ||
                    wxWindowIsUnavailableForCallbacks(expected.frame) ||
                    wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                    expected.frame->m_tabs != expected.tabs ||
                    expected.tabs->GetParent() != current ||
                    current->GetTabFrameFromTabCtrl(expected.tabs) !=
                        expected.frame ||
                    !wxAuiMatchesPageProjection(*expected.tabs, current,
                                                expected.pages) )
            {
                return nullptr;
            }
        }
        return current;
    };

    // CalculateTabCtrlHeight() enters custom art. The complete manager
    // revision must already be weakly captured before that first callback.
    const int height = CalculateTabCtrlHeight();
    book = hasExactManagerRevision();
    if ( !book )
        return false;

    // If the tab control height doesn't change no clone/layout publication is
    // needed, but only decide this after validating the callback boundary.
    if ( book->m_tabCtrlHeight == height )
        return false;

    // Clone everything before publishing the height. A custom Clone() can
    // destroy or rebuild the notebook, and a later allocation failure must not
    // leave only some frames updated.
    std::vector<wxAuiTabArtStaging> clonedArt;
    clonedArt.reserve(frames.size());
    for ( size_t i = 0; i < frames.size(); ++i )
    {
        clonedArt.emplace_back(wxAuiInvokeTabArt(
            art, [](wxAuiTabArt* const leasedArt)
            {
                return leasedArt->Clone();
            }));
        if ( !clonedArt.back().Get() ||
                !clonedArt.back().StillOwnsArt() ||
                !hasExactManagerRevision() )
            return false;
    }

    book = hasExactManagerRevision();
    if ( !book )
        return false;
    book->m_tabCtrlHeight = height;
    expectedPublishedHeight = height;

    for ( size_t i = 0; i < frames.size(); ++i )
    {
        if ( !clonedArt[i].StillOwnsArt() )
            return true;
        frames[i].frame->SetTabCtrlHeight(height);
        if ( !clonedArt[i].StillOwnsArt() )
            return true;
        frames[i].tabs->SetArtProvider(clonedArt[i].Get());
        if ( !wxAuiIsTabArtOwnedBy(clonedArt[i].Get(), frames[i].tabs) )
            return true;
        book = hasExactManagerRevision();
        if ( !book )
            return true;

        frames[i].frame->DoSizing();
        if ( !hasExactManagerRevision() )
            return true;
    }

    return true;
}

void wxAuiNotebook::UpdateHintWindowSize()
{
    wxSize size = CalculateNewSplitSize();

    // the placeholder hint window should be set to this size
    wxAuiPaneInfo& info = m_mgr.GetPane(DUMMY_PANE_NAME);
    if (info.IsOk())
    {
        info.MinSize(size);
        info.BestSize(size);
        m_dummyWnd->SetSize(size);
    }
}


// calculates the size of the new split
wxSize wxAuiNotebook::CalculateNewSplitSize()
{
    return m_mgr.CalculateNewSplitSize();
}

int wxAuiNotebook::CalculateTabCtrlHeight()
{
    // if a fixed tab ctrl height is specified,
    // just return that instead of calculating a
    // tab height
    if (m_requestedTabCtrlHeight != -1)
        return m_requestedTabCtrlHeight;

    // find out new best tab height
    wxAuiTabArt* art = m_tabs.GetArtProvider();

    return wxAuiInvokeTabArt(
        art, [this](wxAuiTabArt* const leasedArt)
        {
            return leasedArt->GetBestTabCtrlSize(
                this, m_tabs.GetPages(), m_requestedBmpSize);
        });
}


wxAuiTabArt* wxAuiNotebook::GetArtProvider() const
{
    return m_tabs.GetArtProvider();
}

void wxAuiNotebook::SetWindowStyleFlag(long style)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxAuiNotebookStyleEpoch styleEpoch =
        wxAuiBumpNotebookStyleEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);

    wxControl::SetWindowStyleFlag(style);

    wxAuiNotebook* book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            wxAuiGetNotebookStyleEpoch(book) != styleEpoch ||
            book->GetWindowStyleFlag() != style )
    {
        return;
    }

    const unsigned int flags = static_cast<unsigned int>(style);
    book->m_flags = flags;

    // if the control is already initialized
    if ( book->m_mgr.GetManagedWindow() != book )
        return;

    wxAuiPageProjection canonicalPages;
    if ( !wxAuiCapturePageProjection(book->m_tabs, book, canonicalPages) )
        return;

    struct FrameRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
        unsigned int originalFlags;
        bool flagsPublished;
    };

    std::vector<FrameRevision> frames;
    for ( const auto& pane : book->m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != book ||
                !wxAuiCapturePageProjection(*tabs, book, pages) )
        {
            return;
        }
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs),
                          std::move(pages), tabs->GetFlags(), false});
    }

    const size_t paneCount = book->m_mgr.GetAllPanes().GetCount();
    const auto hasExactRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = weakThis.get();
        if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                wxAuiGetNotebookStyleEpoch(current) != styleEpoch ||
                current->GetWindowStyleFlag() != style ||
                current->m_flags != flags ||
                current->m_mgr.GetManagedWindow() != current ||
                current->m_mgr.GetAllPanes().GetCount() != paneCount ||
                !wxAuiMatchesPageProjection(current->m_tabs, current,
                                            canonicalPages) )
        {
            return nullptr;
        }

        for ( const FrameRevision& expected : frames )
        {
            if ( expected.frameLifetime.get() != expected.frame ||
                    expected.tabsLifetime.get() != expected.tabs ||
                    wxWindowIsUnavailableForCallbacks(expected.frame) ||
                    wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                    expected.frame->m_tabs != expected.tabs ||
                    expected.tabs->GetParent() != current ||
                    current->GetTabFrameFromTabCtrl(expected.tabs) !=
                        expected.frame ||
                    expected.tabs->GetFlags() !=
                        (expected.flagsPublished
                             ? flags
                             : expected.originalFlags) ||
                    !wxAuiMatchesPageProjection(*expected.tabs, current,
                                                expected.pages) )
            {
                return nullptr;
            }
        }
        return current;
    };

    // Every step below can enter application code through a custom art
    // provider, layout, paint or update handler. Never carry a raw frame/tab
    // identity across one of these boundaries without checking the complete
    // captured projection and latest-writer token again.
    for ( FrameRevision& expected : frames )
    {
        book = hasExactRevision();
        if ( !book )
            return;

        expected.tabs->SetFlags(flags);
        expected.flagsPublished = true;
        if ( !hasExactRevision() )
            return;

        expected.frame->DoSizing();
        if ( !hasExactRevision() )
            return;

        expected.tabs->Refresh();
        if ( !hasExactRevision() )
            return;

        expected.tabs->Update();
        if ( !hasExactRevision() )
            return;
    }
}


bool wxAuiNotebook::AddPage(wxWindow* page,
                            const wxString& caption,
                            bool select,
                            const wxBitmapBundle& bitmap)
{
    return InsertPage(m_tabs.GetPageCount(), page, caption, select, bitmap);
}

bool wxAuiNotebook::InsertPage(size_t page_idx,
                               wxWindow* page,
                               const wxString& caption,
                               bool select,
                               const wxBitmapBundle& bitmap)
{
    if ( wxWindowIsUnavailableForCallbacks(this) ||
            IsDeletingAllPages() ||
            wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return false;

    // Note that index may be equal to the page count here to append the page.
    wxCHECK_MSG(page_idx <= m_tabs.GetPageCount(), false,
                wxT("invalid page index"));

    wxCHECK_MSG(page, false, wxT("page pointer must be non-null"));
    wxCHECK_MSG(!wxWindowIsUnavailableForCallbacks(page), false,
                wxT("can't insert a page being deleted"));
    wxCHECK_MSG(m_tabs.GetIdxFromWindow(page) == wxNOT_FOUND, false,
                wxT("can't insert the same page twice"));

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxWindow* const originalParent = page->GetParent();

    wxCHECK_MSG(page->GetParent() == this, false,
                wxT("page must be a child of the notebook"));

    wxAuiNotebookPage info;
    info.window = page;
    info.caption = caption;
    info.bitmap = bitmap;
    info.active = false;

    wxAuiTabCtrl* active_tabctrl = GetActiveTabCtrl();
    if ( !active_tabctrl ||
            wxWindowIsUnavailableForCallbacks(active_tabctrl) ||
            active_tabctrl->GetParent() != this )
        return false;

    // The usual case is appending a new page, when we can just add it page at
    // the end of the currently active tab control.
    int tab_page_idx = -1;

    // But when inserting, we may need to do it at a different place.
    if ( page_idx != m_tabs.GetPageCount() )
    {
        const auto tabInfo = FindTab(m_tabs.GetWindowFromIdx(page_idx));
        if ( tabInfo.tabCtrl == active_tabctrl )
        {
            // Use this index for insertion, as it's in the correct control.
            tab_page_idx = tabInfo.tabIdx;
        }
        //else: Do nothing, we'll append it to the active tab control.
    }

    InsertPageAt(info, page_idx, active_tabctrl, tab_page_idx, select);

    wxAuiNotebook* const book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakPage.get() != page ||
            wxWindowIsUnavailableForCallbacks(page) )
        return true;

    if ( book->m_tabs.GetIdxFromWindow(page) != wxNOT_FOUND &&
            page->GetParent() == book )
    {
        return true;
    }

    // A callback which deliberately transferred the candidate to another
    // owner consumed it even though this insertion was rolled back.
    wxWindow* const currentParent = page->GetParent();
    return currentParent != originalParent && currentParent != book;
}

void wxAuiNotebook::InsertPageAt(wxAuiNotebookPage& info,
                                 size_t page_idx,
                                 wxAuiTabCtrl* tabctrl,
                                 int tab_page_idx,
                                 bool select)
{
    if ( wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return;

    if ( wxWindowIsUnavailableForCallbacks(this) || !tabctrl ||
            wxWindowIsUnavailableForCallbacks(tabctrl) ||
            tabctrl->GetParent() != this || !info.window ||
            wxWindowIsUnavailableForCallbacks(info.window) )
    {
        return;
    }

    const wxAuiNotebookTopologyTransaction transaction(this);
    wxWindow* const page = info.window;
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    const wxWeakRef<wxAuiTabCtrl> weakTabCtrl(tabctrl);
    wxWindow* const originalParent = page->GetParent();
    const wxWeakRef<wxWindow> weakOriginalParent(originalParent);
    const bool pageShownBefore = page->IsShown();
    const bool needsReparent = originalParent != this;
    const size_t tabCountBefore = tabctrl->GetPageCount();
    const int selectionBefore = m_curPage;
    wxWindow* const selectedBefore =
        selectionBefore >= 0 &&
                static_cast<size_t>(selectionBefore) < m_tabs.GetPageCount()
            ? m_tabs.GetWindowFromIdx(static_cast<size_t>(selectionBefore))
            : nullptr;
    const wxWeakRef<wxWindow> selectedBeforeLifetime(selectedBefore);

    struct FrameRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
        wxWindow* activePage;
        wxWeakRef<wxWindow> activePageLifetime;
    };
    std::vector<FrameRevision> frames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != this ||
                !wxAuiCapturePageProjection(*tabs, this, pages) )
        {
            return;
        }
        const int activeIndex = tabs->GetActivePage();
        wxWindow* const activePage = activeIndex >= 0 &&
                                             static_cast<size_t>(activeIndex) <
                                                 tabs->GetPageCount()
            ? tabs->GetWindowFromIdx(static_cast<size_t>(activeIndex))
            : nullptr;
        if ( activePage && wxWindowIsUnavailableForCallbacks(activePage) )
            return;
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs),
                          std::move(pages), activePage,
                          wxWeakRef<wxWindow>(activePage)});
    }
    const size_t paneCount = m_mgr.GetAllPanes().GetCount();

    if ( tab_page_idx == -1 )
        tab_page_idx = static_cast<int>(tabCountBefore);
    if ( page_idx > m_tabs.GetPageCount() || tab_page_idx < 0 ||
            static_cast<size_t>(tab_page_idx) > tabCountBefore )
    {
        return;
    }

    wxAuiPageProjection mainBefore;
    wxAuiPageProjection tabBefore;
    if ( !wxAuiCapturePageProjection(m_tabs, this, mainBefore) ||
            !wxAuiCapturePageProjection(*tabctrl, this, tabBefore) )
    {
        return;
    }
    wxAuiPageProjection mainAfter = mainBefore;
    wxAuiPageProjection tabAfter = tabBefore;
    try
    {
        mainAfter.insert(mainAfter.begin() + page_idx,
                         wxAuiPageProjectionEntry(page));
        tabAfter.insert(tabAfter.begin() + tab_page_idx,
                        wxAuiPageProjectionEntry(page));
    }
    catch ( const std::exception& )
    {
        return;
    }

    const auto hasExactManagerRevision =
        [&](const bool pagePublished) -> wxAuiNotebook*
        {
            wxAuiNotebook* const book = weakThis.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    book->m_mgr.GetAllPanes().GetCount() != paneCount )
            {
                return nullptr;
            }

            for ( const FrameRevision& expected : frames )
            {
                if ( expected.frameLifetime.get() != expected.frame ||
                        expected.tabsLifetime.get() != expected.tabs ||
                        wxWindowIsUnavailableForCallbacks(expected.frame) ||
                        wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                        expected.frame->m_tabs != expected.tabs ||
                        expected.tabs->GetParent() != book ||
                        book->GetTabFrameFromTabCtrl(expected.tabs) !=
                            expected.frame )
                {
                    return nullptr;
                }

                const wxAuiPageProjection& expectedPages =
                    expected.tabs == tabctrl && pagePublished
                        ? tabAfter
                        : expected.pages;
                if ( !wxAuiMatchesPageProjection(*expected.tabs, book,
                                                 expectedPages) )
                {
                    return nullptr;
                }
            }
            return book;
        };

    bool mainPublished = false;
    bool tabPublished = false;
    const auto eraseEntryWithoutCallback =
        [](wxAuiTabContainer& container,
           size_t index,
           wxWindow* expected)
        {
            if ( index >= container.m_pages.GetCount() ||
                    container.m_pages[index].window != expected )
            {
                return false;
            }

            container.m_pages.RemoveAt(index);
            wxAuiBumpTabContainerRevision(&container);
            return true;
        };
    bool reparentedByUs = false;
    const auto rollbackPublication = [&]()
    {
        wxAuiNotebook* book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) )
            return;

        bool tabErased = false;
        if ( tabPublished && weakTabCtrl.get() == tabctrl &&
                !wxWindowIsUnavailableForCallbacks(tabctrl) )
        {
            tabErased = eraseEntryWithoutCallback(
                *tabctrl, static_cast<size_t>(tab_page_idx), page);
        }

        bool mainErased = false;
        if ( mainPublished )
            mainErased = eraseEntryWithoutCallback(
                book->m_tabs, page_idx, page);

        // Rollback is deliberately local. A callback may have destroyed or
        // rebuilt an unrelated pane, but that must not prevent the canonical
        // model, the insertion target and each independently surviving pane
        // from being repaired.
        const auto hasLocalCanonical = [&]() -> wxAuiNotebook*
        {
            wxAuiNotebook* const current = weakThis.get();
            return current && !wxWindowIsUnavailableForCallbacks(current) &&
                           wxAuiMatchesPageProjection(current->m_tabs,
                                                      current, mainBefore)
                        ? current
                        : nullptr;
        };
        const auto getLocalFrame =
            [&](const FrameRevision& expected) -> wxAuiTabCtrl*
            {
                wxAuiNotebook* const current = hasLocalCanonical();
                wxAuiTabFrame* const frame =
                    expected.frameLifetime.get();
                wxAuiTabCtrl* const tabs = expected.tabsLifetime.get();
                if ( !current || frame != expected.frame || !frame ||
                        tabs != expected.tabs || !tabs ||
                        wxWindowIsUnavailableForCallbacks(frame) ||
                        wxWindowIsUnavailableForCallbacks(tabs) ||
                        frame->m_tabs != tabs ||
                        tabs->GetParent() != current ||
                        !current->m_mgr.GetPane(frame).IsOk() )
                {
                    return nullptr;
                }

                const wxAuiPageProjection& expectedPages =
                    tabs == tabctrl ? tabBefore : expected.pages;
                return wxAuiMatchesPageProjection(*tabs, current,
                                                   expectedPages)
                            ? tabs
                            : nullptr;
            };

        // Restore ownership before any visibility, layout or art callback.
        // If application code already moved the candidate elsewhere, it has
        // explicitly consumed it and rollback must not steal it back.
        const bool absentFromTarget =
            weakTabCtrl.get() != tabctrl ||
            wxWindowIsUnavailableForCallbacks(tabctrl) ||
            tabctrl->GetIdxFromWindow(page) == wxNOT_FOUND;
        if ( reparentedByUs && hasLocalCanonical() == book &&
                weakPage.get() == page &&
                !wxWindowIsUnavailableForCallbacks(page) &&
                page->GetParent() == book && absentFromTarget &&
                book->m_tabs.GetIdxFromWindow(page) == wxNOT_FOUND )
        {
            wxWindow* const restoreParent = weakOriginalParent.get();
            if ( restoreParent == originalParent && restoreParent &&
                    !wxWindowIsUnavailableForCallbacks(restoreParent) )
            {
                (void)page->Reparent(restoreParent);
            }
        }

        // Reparenting and Show() are both application-code boundaries. Only
        // restore the candidate's original physical visibility while it is
        // still owned by the parent captured at transaction entry.
        book = hasLocalCanonical();
        wxWindow* const candidate = weakPage.get();
        wxWindow* const restoredParent = weakOriginalParent.get();
        if ( book && candidate == page &&
                !wxWindowIsUnavailableForCallbacks(candidate) &&
                restoredParent == originalParent &&
                candidate->GetParent() == originalParent &&
                candidate->IsShown() != pageShownBefore )
        {
            candidate->Show(pageShownBefore);
        }

        book = hasLocalCanonical();
        if ( !book )
            return;

        wxWindow* const selected = selectedBeforeLifetime.get();
        book->m_curPage = selected && selected == selectedBefore &&
                                  !wxWindowIsUnavailableForCallbacks(selected)
                              ? book->m_tabs.GetIdxFromWindow(selected)
                              : wxNOT_FOUND;

        // Selection callbacks may already have published different active
        // pages before invalidating the insertion. Restore every pane by its
        // original active-page identity, not just the globally selected pane.
        if ( book->m_curPage != wxNOT_FOUND )
            book->m_tabs.SetActivePage(selected);
        else
            book->m_tabs.SetNoneActive();
        for ( const FrameRevision& expected : frames )
        {
            wxAuiTabCtrl* const tabs = getLocalFrame(expected);
            if ( !tabs )
                continue;

            wxWindow* const active = expected.activePageLifetime.get();
            if ( active == expected.activePage && active &&
                    !wxWindowIsUnavailableForCallbacks(active) &&
                    active->GetParent() == book &&
                    tabs->GetIdxFromWindow(active) != wxNOT_FOUND )
            {
                tabs->SetActivePage(active);
            }
            else
                tabs->SetNoneActive();
        }

        // Active flags are model state; DoUpdateActive() publishes the actual
        // Show()/Hide() state. Validate each pane against its own snapshot,
        // independently of damaged siblings, before and after this callback.
        for ( const FrameRevision& expected : frames )
        {
            book = weakThis.get();
            wxAuiTabCtrl* const tabs = getLocalFrame(expected);
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    !tabs )
            {
                continue;
            }

            const int expectedActive =
                expected.activePage &&
                        expected.activePageLifetime.get() ==
                            expected.activePage
                    ? tabs->GetIdxFromWindow(expected.activePage)
                    : wxNOT_FOUND;
            if ( tabs->GetActivePage() != expectedActive )
                continue;

            tabs->DoUpdateActive();

            book = weakThis.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    getLocalFrame(expected) != tabs ||
                    tabs->GetActivePage() != expectedActive )
            {
                continue;
            }
        }

        // Art repair is local too: a dead sibling cannot suppress sizing for
        // the surviving insertion target or the canonical container.
        const FrameRevision* targetFrame = nullptr;
        for ( const FrameRevision& expected : frames )
        {
            if ( expected.tabs == tabctrl )
            {
                targetFrame = &expected;
                break;
            }
        }
        if ( tabErased && targetFrame &&
                getLocalFrame(*targetFrame) == tabctrl && tabctrl->m_art )
        {
            wxAuiSetTabArtSizingInfo(
                tabctrl->m_art, tabctrl->m_rect.GetSize(),
                tabctrl->m_pages.GetCount(), tabctrl);
        }

        book = hasLocalCanonical();
        if ( mainErased && book && book->m_tabs.m_art )
        {
            wxAuiSetTabArtSizingInfo(
                book->m_tabs.m_art, book->m_tabs.m_rect.GetSize(),
                book->m_tabs.m_pages.GetCount(), book);
        }
    };
    const auto isCurrent = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && hasExactManagerRevision(true) == book &&
                       weakPage.get() == page &&
                       !wxWindowIsUnavailableForCallbacks(page) &&
                       page->GetParent() == book &&
                       weakTabCtrl.get() == tabctrl &&
                       !wxWindowIsUnavailableForCallbacks(tabctrl) &&
                       tabctrl->GetParent() == book &&
                       mainPublished && tabPublished &&
                       wxAuiMatchesPageProjection(book->m_tabs, book,
                                                  mainAfter) &&
                       wxAuiMatchesPageProjection(*tabctrl, book, tabAfter)
                    ? book
                    : nullptr;
    };

    const auto hasOriginalProjection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && hasExactManagerRevision(false) == book &&
                       weakPage.get() == page &&
                       !wxWindowIsUnavailableForCallbacks(page) &&
                       weakTabCtrl.get() == tabctrl &&
                       !wxWindowIsUnavailableForCallbacks(tabctrl) &&
                       tabctrl->GetParent() == book &&
                       wxAuiMatchesPageProjection(book->m_tabs, book,
                                                  mainBefore) &&
                       wxAuiMatchesPageProjection(*tabctrl, book, tabBefore)
                    ? book
                    : nullptr;
    };

    if ( !hasOriginalProjection() )
        return;

    // Reparent() is allowed to report false when no native parent change was
    // needed. Pages are commonly constructed with the book as their parent,
    // so only make this a transaction boundary when an actual transfer is
    // required.
    if ( needsReparent )
    {
        if ( !page->Reparent(this) )
            return;
        reparentedByUs = true;
    }

    if ( weakThis.get() != this ||
            wxWindowIsUnavailableForCallbacks(this) ||
            hasExactManagerRevision(false) != this ||
            weakPage.get() != page ||
            wxWindowIsUnavailableForCallbacks(page) ||
            page->GetParent() != this || weakTabCtrl.get() != tabctrl ||
            wxWindowIsUnavailableForCallbacks(tabctrl) ||
            !wxAuiMatchesPageProjection(m_tabs, this, mainBefore) ||
            !wxAuiMatchesPageProjection(*tabctrl, this, tabBefore) )
    {
        rollbackPublication();
        return;
    }

    // if there are currently no tabs, the first added
    // tab must be selected, even if "select" is false
    if (m_tabs.GetPageCount() == 0)
    {
        select = true;
    }

    // Publish both projections without invoking either art provider between
    // them. wxAuiTabContainer::InsertPage() notifies SetSizingInfo()
    // immediately, which would expose an N+1 main model and N split model to
    // application-provided art code.
    try
    {
        auto mainButtons = MakePageButtons(m_tabs.m_flags);
        auto tabButtons = MakePageButtons(tabctrl->m_flags);

        const auto mainIt =
            m_tabs.m_pages.insert(m_tabs.m_pages.begin() + page_idx, info);
        mainPublished = true;
        wxAuiBumpTabContainerRevision(&m_tabs);
        mainIt->buttons = std::move(mainButtons);
        wxAuiBumpTabContainerRevision(&m_tabs);

        const auto tabIt = tabctrl->m_pages.insert(
            tabctrl->m_pages.begin() + tab_page_idx, info);
        tabPublished = true;
        wxAuiBumpTabContainerRevision(tabctrl);
        tabIt->buttons = std::move(tabButtons);
        wxAuiBumpTabContainerRevision(tabctrl);
    }
    catch ( const std::exception& )
    {
        rollbackPublication();
        return;
    }

    if ( !isCurrent() )
    {
        rollbackPublication();
        return;
    }

    if ( tabctrl->m_art )
    {
        wxAuiSetTabArtSizingInfo(
            tabctrl->m_art, tabctrl->m_rect.GetSize(),
            tabctrl->m_pages.GetCount(), tabctrl);
    }
    if ( !isCurrent() )
    {
        rollbackPublication();
        return;
    }

    wxAuiNotebook* bookAfterArt = weakThis.get();
    if ( bookAfterArt &&
            !wxWindowIsUnavailableForCallbacks(bookAfterArt) &&
            bookAfterArt->m_tabs.m_art )
    {
        wxAuiSetTabArtSizingInfo(
            bookAfterArt->m_tabs.m_art,
            bookAfterArt->m_tabs.m_rect.GetSize(),
            bookAfterArt->m_tabs.m_pages.GetCount(), bookAfterArt);
    }
    if ( !isCurrent() )
    {
        rollbackPublication();
        return;
    }

    // Note that we don't need to call DoSizing() if the height has changed, as
    // it's already called from UpdateTabCtrlHeight() itself in this case.
    const bool heightUpdated = UpdateTabCtrlHeight();
    if ( !isCurrent() )
    {
        rollbackPublication();
        return;
    }

    if ( !heightUpdated )
    {
        DoSizing();
        if ( !isCurrent() )
        {
            rollbackPublication();
            return;
        }
    }

    tabctrl->DoUpdateActive();

    if ( !isCurrent() )
    {
        rollbackPublication();
        return;
    }

    // adjust selected index
    if(m_curPage >= (int) page_idx)
    {
        m_curPage++;
    }

    if (select)
    {
        SetSelectionToWindow(page);
        if ( !isCurrent() )
            rollbackPublication();
    }
}

int wxAuiNotebook::GetNextPage(bool forward) const
{
    if ( m_curPage == wxNOT_FOUND )
        return wxNOT_FOUND;

    const auto tabInfo = FindTab(m_tabs.GetWindowFromIdx(m_curPage));
    if ( !tabInfo )
        return wxNOT_FOUND;

    // Find the next or previous position in the active tab control, with
    // wraparound.
    const int lastPos = tabInfo.tabCtrl->GetPageCount() - 1;
    const int nextPos = forward
        ? tabInfo.tabIdx < lastPos ? tabInfo.tabIdx + 1 : 0
        : tabInfo.tabIdx > 0 ? tabInfo.tabIdx - 1 : lastPos;

    // Now find the corresponding page index.
    return m_tabs.GetIdxFromWindow(tabInfo.tabCtrl->GetWindowFromIdx(nextPos));
}

wxWindow* wxAuiNotebook::DoRemovePage(size_t page_idx)
{
    if ( wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return nullptr;

    wxCHECK_MSG(page_idx < m_tabs.GetPageCount(), nullptr,
                "invalid page index");

    const wxAuiNotebookTopologyTransaction transaction(this);
    const wxWeakRef<wxAuiNotebook> weakThis(this);

    wxWindow* const wnd = m_tabs.GetWindowFromIdx(page_idx);
    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return nullptr;
    const wxWeakRef<wxWindow> weakPage(wnd);

    const auto tabInfo = FindTab(wnd);
    if ( !tabInfo || !tabInfo.tabCtrl ||
            wxWindowIsUnavailableForCallbacks(tabInfo.tabCtrl) )
    {
        return nullptr;
    }

    wxAuiTabCtrl* const ctrl = tabInfo.tabCtrl;
    const wxWeakRef<wxAuiTabCtrl> weakCtrl(ctrl);
    int ctrl_idx = tabInfo.tabIdx;
    if ( ctrl_idx < 0 ||
            static_cast<size_t>(ctrl_idx) >= ctrl->GetPageCount() )
    {
        return nullptr;
    }

    // Every raw page identity needed after the first callback is paired with
    // a weak reference up front. In particular, don't discover a replacement
    // page and only then construct a weak reference after wxEVT_SHOW.
    struct PageSnapshot
    {
        explicit PageSnapshot(wxWindow* const page_)
            : page(page_), lifetime(page_)
        {
        }

        wxWindow* page;
        wxWeakRef<wxWindow> lifetime;
    };

    std::vector<PageSnapshot> canonicalBefore;
    canonicalBefore.reserve(m_tabs.GetPageCount());
    for ( const wxAuiNotebookPage& page : m_tabs.GetPages() )
    {
        if ( !page.window ||
                wxWindowIsUnavailableForCallbacks(page.window) )
        {
            return nullptr;
        }
        canonicalBefore.emplace_back(page.window);
    }

    std::vector<PageSnapshot> ctrlBefore;
    ctrlBefore.reserve(ctrl->GetPageCount());
    for ( const wxAuiNotebookPage& page : ctrl->GetPages() )
    {
        if ( !page.window ||
                wxWindowIsUnavailableForCallbacks(page.window) )
        {
            return nullptr;
        }
        ctrlBefore.emplace_back(page.window);
    }

    if ( page_idx >= canonicalBefore.size() ||
            canonicalBefore[page_idx].page != wnd ||
            static_cast<size_t>(ctrl_idx) >= ctrlBefore.size() ||
            ctrlBefore[static_cast<size_t>(ctrl_idx)].page != wnd )
    {
        return nullptr;
    }

    const int oldSelection = m_curPage;
    const PageSnapshot* selectedBeforeCommit = nullptr;
    if ( m_curPage >= 0 &&
            static_cast<size_t>(m_curPage) < canonicalBefore.size() )
    {
        selectedBeforeCommit =
            &canonicalBefore[static_cast<size_t>(m_curPage)];
    }
    const bool wasCurrent = selectedBeforeCommit &&
                            selectedBeforeCommit->page == wnd;
    const bool wasActiveInSplit =
        ctrl->m_pages[static_cast<size_t>(ctrl_idx)].active;

    std::vector<PageSnapshot> canonicalSurvivors;
    canonicalSurvivors.reserve(canonicalBefore.size() - 1);
    for ( size_t i = 0; i < canonicalBefore.size(); ++i )
    {
        if ( i != page_idx )
            canonicalSurvivors.push_back(canonicalBefore[i]);
    }

    std::vector<PageSnapshot> ctrlSurvivors;
    ctrlSurvivors.reserve(ctrlBefore.size() - 1);
    for ( size_t i = 0; i < ctrlBefore.size(); ++i )
    {
        if ( i != static_cast<size_t>(ctrl_idx) )
            ctrlSurvivors.push_back(ctrlBefore[i]);
    }

    const auto matchesProjection = [](const wxAuiTabContainer& container,
                                      const std::vector<PageSnapshot>& pages,
                                      wxAuiNotebook* const book)
    {
        if ( container.GetPageCount() != pages.size() )
            return false;
        for ( size_t i = 0; i < pages.size(); ++i )
        {
            const PageSnapshot& expected = pages[i];
            if ( expected.lifetime.get() != expected.page ||
                    wxWindowIsUnavailableForCallbacks(expected.page) ||
                    expected.page->GetParent() != book ||
                    container.GetWindowFromIdx(i) != expected.page )
            {
                return false;
            }
        }
        return true;
    };

    const auto hasOriginalProjection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && !wxWindowIsUnavailableForCallbacks(book) &&
                       weakPage.get() == wnd &&
                       !wxWindowIsUnavailableForCallbacks(wnd) &&
                       wnd->GetParent() == book &&
                       weakCtrl.get() == ctrl &&
                       !wxWindowIsUnavailableForCallbacks(ctrl) &&
                       ctrl->GetParent() == book &&
                       matchesProjection(book->m_tabs, canonicalBefore, book) &&
                       matchesProjection(*ctrl, ctrlBefore, book)
                    ? book
                    : nullptr;
    };

    // Freeze itself is a callback boundary on some ports, so take all weak
    // snapshots before constructing the weak-aware locker and validate it.
    wxAuiNotebookUpdateLocker locker(this);
    wxAuiNotebook* book = hasOriginalProjection();
    if ( !book )
        return nullptr;

    ShowWnd(wnd, false);
    book = hasOriginalProjection();
    if ( !book )
        return nullptr;

    // Atomic commit: both models are changed with no callback in between.
    ctrl->m_pages.RemoveAt(static_cast<size_t>(ctrl_idx));
    wxAuiBumpTabContainerRevision(ctrl);
    book->m_tabs.m_pages.RemoveAt(page_idx);
    wxAuiBumpTabContainerRevision(&book->m_tabs);

    const auto hasCommittedProjection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = weakThis.get();
        if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                current->m_tabs.GetIdxFromWindow(wnd) != wxNOT_FOUND ||
                !matchesProjection(current->m_tabs,
                                   canonicalSurvivors, current) )
        {
            return nullptr;
        }

        wxAuiTabCtrl* const currentCtrl = weakCtrl.get();
        if ( ctrlSurvivors.empty() &&
                (!currentCtrl ||
                 wxWindowIsUnavailableForCallbacks(currentCtrl)) )
        {
            return current;
        }
        return currentCtrl == ctrl &&
                       !wxWindowIsUnavailableForCallbacks(ctrl) &&
                       ctrl->GetParent() == current &&
                       ctrl->GetIdxFromWindow(wnd) == wxNOT_FOUND &&
                       matchesProjection(*ctrl, ctrlSurvivors, current)
                    ? current
                    : nullptr;
    };

    const auto getTransferredPage = [&]() -> wxWindow*
    {
        wxAuiNotebook* const current = weakThis.get();
        if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                weakPage.get() != wnd ||
                wxWindowIsUnavailableForCallbacks(wnd) ||
                wnd->GetParent() != current ||
                current->m_tabs.GetIdxFromWindow(wnd) != wxNOT_FOUND )
        {
            return nullptr;
        }
        wxAuiTabCtrl* const currentCtrl = weakCtrl.get();
        if ( currentCtrl &&
                !wxWindowIsUnavailableForCallbacks(currentCtrl) &&
                currentCtrl->GetIdxFromWindow(wnd) != wxNOT_FOUND )
        {
            return nullptr;
        }
        return wnd;
    };

    if ( wasCurrent )
    {
        book->m_curPage = wxNOT_FOUND;
    }
    else if ( selectedBeforeCommit &&
            selectedBeforeCommit->lifetime.get() ==
                selectedBeforeCommit->page )
    {
        book->m_curPage = book->m_tabs.GetIdxFromWindow(
            selectedBeforeCommit->page);
    }
    else
    {
        book->m_curPage = wxNOT_FOUND;
    }

    if ( ctrl->m_art )
    {
        wxAuiSetTabArtSizingInfo(
            ctrl->m_art, ctrl->m_rect.GetSize(),
            ctrl->m_pages.GetCount(), ctrl);
    }
    book = hasCommittedProjection();
    if ( !book )
        return getTransferredPage();
    if ( book->m_tabs.m_art )
    {
        wxAuiSetTabArtSizingInfo(
            book->m_tabs.m_art, book->m_tabs.m_rect.GetSize(),
            book->m_tabs.m_pages.GetCount(), book);
    }
    book = hasCommittedProjection();
    if ( !book )
        return getTransferredPage();

    const PageSnapshot* newActive = nullptr;
    if ( wasActiveInSplit && !ctrlSurvivors.empty() )
    {
        if ( static_cast<size_t>(ctrl_idx) >= ctrlSurvivors.size() )
            ctrl_idx = static_cast<int>(ctrlSurvivors.size() - 1);

        const PageSnapshot& splitReplacement =
            ctrlSurvivors[static_cast<size_t>(ctrl_idx)];
        ctrl->SetActivePage(ctrl_idx);
        book = hasCommittedProjection();
        if ( !book )
            return getTransferredPage();

        if ( wasCurrent )
        {
            newActive = &splitReplacement;
        }
        else
        {
            if ( selectedBeforeCommit && !wasCurrent )
                newActive = selectedBeforeCommit;

            ctrl->DoShowTab(ctrl_idx);
            book = hasCommittedProjection();
            if ( !book )
                return getTransferredPage();
        }
    }
    else if ( selectedBeforeCommit && !wasCurrent )
    {
        newActive = selectedBeforeCommit;
    }

    if ( !newActive && !canonicalSurvivors.empty() )
    {
        newActive = page_idx < canonicalSurvivors.size()
            ? &canonicalSurvivors[page_idx]
            : &canonicalSurvivors[0];
    }

    RemoveEmptyTabFrames();
    book = hasCommittedProjection();
    if ( !book )
        return getTransferredPage();

    wxWindow* latestSelected = nullptr;
    if ( book->m_curPage >= 0 &&
            static_cast<size_t>(book->m_curPage) <
                book->m_tabs.GetPageCount() )
    {
        latestSelected = book->m_tabs.GetWindowFromIdx(book->m_curPage);
    }
    if ( !latestSelected && newActive &&
            newActive->lifetime.get() == newActive->page &&
            !wxWindowIsUnavailableForCallbacks(newActive->page) &&
            book->m_tabs.GetIdxFromWindow(newActive->page) != wxNOT_FOUND &&
            !book->m_isBeingDeleted )
    {
        book->SetSelectionToWindow(newActive->page);
    }

    book = hasCommittedProjection();
    if ( !book )
        return getTransferredPage();

    if ( canonicalSurvivors.empty() && oldSelection != wxNOT_FOUND &&
            !book->m_isBeingDeleted )
    {
        // Removing the last page changes selection without a replacement
        // window. Dispatch only after revalidating the committed topology;
        // the handler may destroy the book or the transferred page.
        wxAuiNotebookEvent evt(wxEVT_AUINOTEBOOK_PAGE_CHANGED, book->m_windowId);
        evt.SetSelection(wxNOT_FOUND);
        evt.SetOldSelection(oldSelection);
        evt.SetEventObject(book);
        (void)book->ProcessWindowEvent(evt);
    }

    return getTransferredPage();
}

int wxAuiNotebook::FindPage(const wxWindow* page) const
{
    return m_tabs.GetIdxFromWindow(page);
}


// SetPageText() changes the tab caption of the specified page
bool wxAuiNotebook::SetPageText(size_t page_idx, const wxString& text)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    wxAuiBumpNotebookHeightInputEpoch(this);
    page_info.caption = text;
    wxAuiBumpTabContainerRevision(&m_tabs);

    // update what's on screen
    if ( const auto tabInfo = FindTab(page_info.window) )
    {
        tabInfo.pageInfo->caption = text;
        wxAuiBumpTabContainerRevision(tabInfo.tabCtrl);
        tabInfo.tabCtrl->Refresh();
        tabInfo.tabCtrl->Update();
    }

    return true;
}

// returns the page caption
wxString wxAuiNotebook::GetPageText(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxString(), "invalid page index");

    // update our own tab catalog
    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.caption;
}

bool wxAuiNotebook::SetPageToolTip(size_t page_idx, const wxString& text)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    wxAuiBumpNotebookHeightInputEpoch(this);
    page_info.tooltip = text;
    wxAuiBumpTabContainerRevision(&m_tabs);

    const auto tabInfo = FindTab(page_info.window);
    if ( !tabInfo )
        return false;

    tabInfo.pageInfo->tooltip = text;
    wxAuiBumpTabContainerRevision(tabInfo.tabCtrl);

    // NB: we don't update the tooltip if it is already being displayed, it
    //     typically never happens, no need to code that
    return true;
}

wxString wxAuiNotebook::GetPageToolTip(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxString(), "invalid page index");

    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.tooltip;
}

bool wxAuiNotebook::SetPageBitmap(size_t page_idx, const wxBitmapBundle& bitmap)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxBitmapBundle requestedBitmap(bitmap);

    // update our own tab catalog
    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    wxWindow* const page = page_info.window;
    const wxWeakRef<wxWindow> weakPage(page);
    const auto originalTabInfo = FindTab(page);
    if ( !originalTabInfo || !originalTabInfo.tabCtrl ||
            originalTabInfo.tabIdx < 0 )
    {
        return false;
    }
    wxAuiTabCtrl* const tabCtrl = originalTabInfo.tabCtrl;
    const wxWeakRef<wxAuiTabCtrl> weakTabCtrl(tabCtrl);
    const size_t tabIndex =
        static_cast<size_t>(originalTabInfo.tabIdx);
    wxAuiPageProjection tabProjection;
    if ( !wxAuiCapturePageProjection(*tabCtrl, this, tabProjection) )
        return false;

    const wxAuiNotebookBitmapEpoch bitmapEpoch =
        wxAuiBumpNotebookBitmapEpoch(this, page);
    wxAuiBumpNotebookHeightInputEpoch(this);
    page_info.bitmap = requestedBitmap;
    wxAuiBumpTabContainerRevision(&m_tabs);

    // tab height might have changed
    UpdateTabCtrlHeight();

    bool visualPublished = false;
    const auto hasExactBitmapRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && !wxWindowIsUnavailableForCallbacks(book) &&
                       wxAuiGetNotebookBitmapEpoch(book, page) ==
                           bitmapEpoch &&
                       weakPage.get() == page &&
                       !wxWindowIsUnavailableForCallbacks(page) &&
                       page->GetParent() == book &&
                       page_idx < book->GetPageCount() &&
                       book->m_tabs.GetWindowFromIdx(page_idx) == page &&
                       book->m_tabs.GetPage(page_idx).bitmap.IsSameAs(
                           requestedBitmap) &&
                       weakTabCtrl.get() == tabCtrl &&
                       !wxWindowIsUnavailableForCallbacks(tabCtrl) &&
                       tabCtrl->GetParent() == book &&
                       tabIndex < tabCtrl->GetPageCount() &&
                       tabCtrl->GetWindowFromIdx(tabIndex) == page &&
                       (!visualPublished ||
                        tabCtrl->GetPage(tabIndex).bitmap.IsSameAs(
                            requestedBitmap)) &&
                       wxAuiMatchesPageProjection(*tabCtrl, book,
                                                  tabProjection)
                    ? book
                    : nullptr;
    };

    if ( !hasExactBitmapRevision() )
        return false;

    // update what's on screen
    tabCtrl->GetPage(tabIndex).bitmap = requestedBitmap;
    wxAuiBumpTabContainerRevision(tabCtrl);
    visualPublished = true;
    tabCtrl->Refresh();
    if ( !hasExactBitmapRevision() )
        return false;

    tabCtrl->Update();
    if ( !hasExactBitmapRevision() )
        return false;

    return true;
}

// returns the page bitmap
wxBitmap wxAuiNotebook::GetPageBitmap(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxBitmap(), "invalid page index");

    // update our own tab catalog
    const wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    return page_info.bitmap.GetBitmap(page_info.bitmap.GetDefaultSize());
}

wxAuiTabKind wxAuiNotebook::GetPageKind(size_t page_idx) const
{
    wxCHECK_MSG(page_idx < GetPageCount(), wxAuiTabKind::Normal, "invalid page index");

    return m_tabs.GetPage(page_idx).kind;
}

bool wxAuiNotebook::SetPageKind(size_t page_idx, wxAuiTabKind kind)
{
    wxCHECK_MSG(page_idx < GetPageCount(), false, "invalid page index");

    wxAuiNotebookPage& page_info = m_tabs.GetPage(page_idx);
    if ( page_info.kind == kind )
        return false;

    const auto tabInfo = FindTab(page_info.window);
    if ( !tabInfo )
        return false;

    wxAuiBumpNotebookHeightInputEpoch(this);

    auto& tabCtrl = *tabInfo.tabCtrl;

    // Update the tab position as changing its kind moves it: if it becomes
    // locked/pinned, it needs to be moved to the end of the group of the tabs
    // of this kind and if it becomes normal, it needs to be moved to the
    // beginning of the normal tabs (and not the end, to minimize the distance
    // by which it moves).
    size_t newIdx = 0;
    switch ( kind )
    {
        case wxAuiTabKind::Normal:
            newIdx = tabCtrl.GetFirstTabOfKind(wxAuiTabKind::Normal);

            // There must be at least the locked/pinned tab which is changing
            // its kind now before the first normal one.
            wxASSERT( newIdx > 0 );

            // Move it before the first normal tab or before the end.
            newIdx--;
            break;

        case wxAuiTabKind::Pinned:
            // When a normal tab becomes pinned, it should be moved to the end
            // of the pinned tabs, but when a locked tab becomes pinned, it
            // makes more sense to put it at the beginning of this group, for
            // symmetry with what happens when a pinned tab becomes normal.
            switch ( tabInfo.pageInfo->kind )
            {
                case wxAuiTabKind::Normal:
                    newIdx = tabCtrl.GetFirstTabOfKind(wxAuiTabKind::Normal);
                    break;

                case wxAuiTabKind::Pinned:
                    // This tab is switching to pinned state, so it couldn't
                    // have already been pinned before.
                    wxFAIL_MSG("unreachable");
                    break;

                case wxAuiTabKind::Locked:
                    newIdx = tabCtrl.GetFirstTabNotOfKind(wxAuiTabKind::Locked);

                    // There must be at least one locked tab, which is
                    // switching to pinned state right now.
                    wxASSERT( newIdx > 0 );

                    newIdx--;
                    break;
            }
            break;

        case wxAuiTabKind::Locked:
            newIdx = tabCtrl.GetFirstTabNotOfKind(wxAuiTabKind::Locked);
            break;
    }

    page_info.kind = kind;
    wxAuiBumpTabContainerRevision(&m_tabs);

    // And also update kind in this copy of the page info before moving it.
    tabInfo.pageInfo->kind = kind;
    wxAuiBumpTabContainerRevision(tabInfo.tabCtrl);

    tabCtrl.MovePage(tabInfo.tabIdx, newIdx);
    tabCtrl.Refresh();

    return true;
}

// GetSelection() returns the index of the currently active page
int wxAuiNotebook::GetSelection() const
{
    return m_curPage;
}

// SetSelection() sets the currently active page
int wxAuiNotebook::SetSelection(size_t new_page)
{
    return DoModifySelection(new_page, true);
}

void wxAuiNotebook::SetSelectionToWindow(wxWindow *win)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(win);
    const size_t pageCount = m_tabs.GetPageCount();
    const int idx = m_tabs.GetIdxFromWindow(win);
    wxCHECK_RET( idx != wxNOT_FOUND, wxT("invalid notebook page") );
    const wxAuiNotebookSelectionTransaction selectionTransaction(this);
    if ( !selectionTransaction.IsAccepted() )
        return;


    // since a tab was clicked, let the parent know that we received
    // the focus, even if we will assign that focus immediately
    // to the child tab in the SetSelection call below
    // (the child focus event will also let wxAuiManager, if any,
    // know that the notebook control has been activated)

    wxWindow* parent = GetParent();
    if (parent)
    {
        wxChildFocusEvent eventFocus(this);
        parent->ProcessWindowEvent(eventFocus);
    }

    // The parent focus handler can destroy/rebuild us or make a newer nested
    // selection. Never publish the stale outer request afterwards.
    wxAuiNotebook* const book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            !selectionTransaction.IsLatest() ||
            weakPage.get() != win ||
            wxWindowIsUnavailableForCallbacks(win) ||
            win->GetParent() != book ||
            book->m_tabs.GetPageCount() != pageCount || idx < 0 ||
            static_cast<size_t>(idx) >= pageCount ||
            book->m_tabs.GetWindowFromIdx(static_cast<size_t>(idx)) != win )
    {
        return;
    }

    book->SetSelection(static_cast<size_t>(idx));
}

// GetPageCount() returns the total number of
// pages managed by the multi-notebook
size_t wxAuiNotebook::GetPageCount() const
{
    return m_tabs.GetPageCount();
}

// GetPage() returns the wxWindow pointer of the
// specified page
wxWindow* wxAuiNotebook::GetPage(size_t page_idx) const
{
    wxASSERT(page_idx < m_tabs.GetPageCount());

    return m_tabs.GetWindowFromIdx(page_idx);
}

// DoSizing() performs all sizing operations in each tab control
void wxAuiNotebook::DoSizing()
{
    if ( wxWindowIsUnavailableForCallbacks(this) )
        return;

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    wxAuiPageProjection canonicalPages;
    if ( !wxAuiCapturePageProjection(m_tabs, this, canonicalPages) )
        return;

    struct FrameRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
    };
    std::vector<FrameRevision> frames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != this ||
                !wxAuiCapturePageProjection(*tabs, this, pages) )
        {
            return;
        }
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs),
                          std::move(pages)});
    }
    const size_t paneCount = m_mgr.GetAllPanes().GetCount();
    const auto hasExactRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                book->m_mgr.GetAllPanes().GetCount() != paneCount ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalPages) )
        {
            return nullptr;
        }
        for ( const FrameRevision& expected : frames )
        {
            if ( expected.frameLifetime.get() != expected.frame ||
                    wxWindowIsUnavailableForCallbacks(expected.frame) ||
                    expected.tabsLifetime.get() != expected.tabs ||
                    wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                    expected.frame->m_tabs != expected.tabs ||
                    expected.tabs->GetParent() != book ||
                    book->GetTabFrameFromTabCtrl(expected.tabs) !=
                        expected.frame ||
                    !wxAuiMatchesPageProjection(*expected.tabs, book,
                                                expected.pages) )
            {
                return nullptr;
            }
        }
        return book;
    };

    if ( !hasExactRevision() )
        return;
    for ( const FrameRevision& expected : frames )
    {
        expected.frame->DoSizing();
        if ( !hasExactRevision() )
            return;
    }
}

// GetActiveTabCtrl() returns the active tab control.  It is
// called to determine which control gets new windows being added
wxAuiTabCtrl* wxAuiNotebook::GetActiveTabCtrl()
{
    if (m_curPage >= 0 && m_curPage < (int)m_tabs.GetPageCount())
    {
        // find the tab ctrl with the current page
        return FindTab(m_tabs.GetPage(m_curPage).window).tabCtrl;
    }

    // no current page, just return the main tab control
    return GetMainTabCtrl();
}

wxAuiTabCtrl* wxAuiNotebook::GetMainTabCtrl()
{
    if ( wxWindowIsUnavailableForCallbacks(this) )
        return nullptr;

    wxAuiTabCtrl* tabMain = nullptr;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        if ( pane.dock_direction == wxAUI_DOCK_CENTER )
        {
            wxASSERT_MSG( !tabMain, "Multiple main tab controls?" );
            auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
            if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
                return nullptr;
            wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
            if ( !tabs || frame->m_tabs != tabs ||
                    wxWindowIsUnavailableForCallbacks(tabs) ||
                    tabs->GetParent() != this )
            {
                return nullptr;
            }
            tabMain = tabs;
        }
    }

    if ( !tabMain )
    {
        const wxWeakRef<wxAuiNotebook> weakThis(this);

        struct PaneRevision
        {
            wxAuiTabFrame* frame;
            wxWeakRef<wxAuiTabFrame> frameLifetime;
            wxAuiTabCtrl* tabs;
            wxWeakRef<wxAuiTabCtrl> tabsLifetime;
            wxAuiPageProjection pages;
        };
        std::vector<PaneRevision> panes;
        for ( const auto& pane : m_mgr.GetAllPanes() )
        {
            if ( IsDummyPane(pane) )
                continue;
            auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
            if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
                return nullptr;
            wxAuiTabCtrl* const paneTabs = frame->m_tabsLifetime.get();
            wxAuiPageProjection pages;
            if ( !paneTabs || frame->m_tabs != paneTabs ||
                    wxWindowIsUnavailableForCallbacks(paneTabs) ||
                    paneTabs->GetParent() != this ||
                    !wxAuiCapturePageProjection(*paneTabs, this, pages) )
            {
                return nullptr;
            }
            panes.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                             paneTabs, wxWeakRef<wxAuiTabCtrl>(paneTabs),
                             std::move(pages)});
        }
        const size_t paneCountBefore = m_mgr.GetAllPanes().GetCount();
        const auto hasExistingRevision =
            [&](const size_t expectedPaneCount) -> wxAuiNotebook*
            {
                wxAuiNotebook* const current = weakThis.get();
                if ( !current ||
                        wxWindowIsUnavailableForCallbacks(current) ||
                        current->m_mgr.GetAllPanes().GetCount() !=
                            expectedPaneCount )
                {
                    return nullptr;
                }
                for ( const PaneRevision& expected : panes )
                {
                    if ( expected.frameLifetime.get() != expected.frame ||
                            expected.tabsLifetime.get() != expected.tabs ||
                            wxWindowIsUnavailableForCallbacks(
                                expected.frame) ||
                            wxWindowIsUnavailableForCallbacks(
                                expected.tabs) ||
                            expected.frame->m_tabs != expected.tabs ||
                            expected.tabs->GetParent() != current ||
                            current->GetTabFrameFromTabCtrl(expected.tabs) !=
                                expected.frame ||
                            !wxAuiMatchesPageProjection(
                                *expected.tabs, current, expected.pages) )
                    {
                        return nullptr;
                    }
                }
                return current;
            };

        wxAuiTabFrame* tabframe = CreateTabFrame();
        if ( !tabframe )
            return nullptr;
        const wxWeakRef<wxAuiTabFrame> weakFrame(tabframe);
        wxAuiTabCtrl* const tabs = tabframe->m_tabs;
        const wxWeakRef<wxAuiTabCtrl> weakTabs(tabs);
        const auto rollbackFrame = [&]()
        {
            wxAuiNotebook* book = weakThis.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    weakFrame.get() != tabframe ||
                    wxWindowIsUnavailableForCallbacks(tabframe) ||
                    weakTabs.get() != tabs ||
                    wxWindowIsUnavailableForCallbacks(tabs) ||
                    tabs->GetParent() != book || tabs->GetPageCount() != 0 )
            {
                return;
            }
            if ( book->m_mgr.GetPane(tabframe).IsOk() )
                book->m_mgr.DetachPane(tabframe);
            book = weakThis.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) )
                return;
            if ( weakFrame.get() == tabframe &&
                    !wxWindowIsUnavailableForCallbacks(tabframe) &&
                    !book->m_mgr.GetPane(tabframe).IsOk() )
            {
                delete tabframe;
            }
        };

        wxAuiNotebook* book = hasExistingRevision(paneCountBefore);
        if ( !book || weakFrame.get() != tabframe ||
                weakTabs.get() != tabs ||
                wxWindowIsUnavailableForCallbacks(tabframe) ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != book || tabs->GetPageCount() != 0 )
        {
            rollbackFrame();
            return nullptr;
        }

        m_mgr.AddPane(tabframe,
                      wxAuiPaneInfo().Center().CaptionVisible(false));
        book = hasExistingRevision(paneCountBefore + 1);
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                weakFrame.get() != tabframe ||
                wxWindowIsUnavailableForCallbacks(tabframe) ||
                weakTabs.get() != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != book ||
                book->GetTabFrameFromTabCtrl(tabs) != tabframe )
        {
            rollbackFrame();
            return nullptr;
        }

        const auto hasExactRevision = [&]() -> wxAuiNotebook*
        {
            wxAuiNotebook* const current =
                hasExistingRevision(paneCountBefore + 1);
            return current && weakFrame.get() == tabframe &&
                           weakTabs.get() == tabs &&
                           !wxWindowIsUnavailableForCallbacks(tabframe) &&
                           !wxWindowIsUnavailableForCallbacks(tabs) &&
                           tabframe->m_tabs == tabs &&
                           tabs->GetParent() == current &&
                           tabs->GetPageCount() == 0 &&
                           current->GetTabFrameFromTabCtrl(tabs) == tabframe
                        ? current
                        : nullptr;
        };
        m_mgr.Update();

        book = hasExactRevision();
        if ( !book ||
                weakFrame.get() != tabframe || weakTabs.get() != tabs ||
                book->GetTabFrameFromTabCtrl(tabs) != tabframe )
        {
            rollbackFrame();
            return nullptr;
        }
        tabMain = tabs;
    }

    return tabMain;
}

std::vector<wxAuiTabCtrl*> wxAuiNotebook::GetAllTabCtrls()
{
    std::vector<wxAuiTabCtrl*> tabCtrls;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        tabCtrls.push_back(tabframe->m_tabs);
    }

    if ( tabCtrls.empty() )
    {
        if ( wxAuiTabCtrl* const mainTabCtrl = GetMainTabCtrl() )
            tabCtrls.push_back(mainTabCtrl);
    }

    return tabCtrls;
}

wxAuiNotebook::TabInfo wxAuiNotebook::FindTab(wxWindow* wnd) const
{
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        wxAuiTabCtrl* const tabCtrl = tabframe->m_tabs;

        const size_t page_count = tabCtrl->GetPageCount();
        for ( size_t i = 0; i < page_count; ++i )
        {
            wxAuiNotebookPage& page = tabCtrl->GetPage(i);
            if ( page.window == wnd )
                return { tabCtrl, static_cast<int>(i), &page };
        }
    }

    wxFAIL_MSG( "Window unexpectedly not found in any tab control" );

    return {};
}

// This overload is deprecated and kept only for compatibility.
bool wxAuiNotebook::FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx) const
{
    const auto tabInfo = FindTab(page);
    if ( !tabInfo )
        return false;

    *ctrl = tabInfo.tabCtrl;
    *idx = tabInfo.tabIdx;

    return true;
}

int
wxAuiNotebook::GetDropIndex(const wxAuiNotebookPage& page_info,
                            wxAuiTabCtrl* dest_tabs,
                            const wxPoint& mouse_screen_pt) const
{
    int insert_idx = -1;

    wxPoint pt = dest_tabs->ScreenToClient(mouse_screen_pt);
    if (auto const targetInfo = dest_tabs->TabHitTest(pt))
    {
        insert_idx = targetInfo.pos;

        // Check that we don't try to insert a tab between tabs of
        // inappropriate kind, kinds must always remain in a (non-strict)
        // decreasing order.
        if ( insert_idx > 0 )
        {
            if ( dest_tabs->GetPage(insert_idx - 1).kind < page_info.kind )
                return wxNOT_FOUND;
        }

        if ( page_info.kind < dest_tabs->GetPage(insert_idx).kind )
            return wxNOT_FOUND;
    }
    else // Append the tab if there is no existing tab under it.
    {
        insert_idx = dest_tabs->GetPageCount();

        // When appending we need just a single test.
        if ( dest_tabs->GetPage(insert_idx - 1).kind < page_info.kind )
            return wxNOT_FOUND;
    }

    return insert_idx;
}

wxAuiNotebookPosition wxAuiNotebook::GetPagePosition(size_t page) const
{
    return FindTab(GetPage(page));
}

std::vector<size_t>
wxAuiNotebook::GetPagesInDisplayOrder(wxAuiTabCtrl* tabCtrl) const
{
    wxCHECK_MSG( tabCtrl, std::vector<size_t>(), "invalid tab control" );

    const size_t count = tabCtrl->GetPageCount();
    std::vector<size_t> pages(count);
    for ( size_t i = 0; i < count; ++i )
    {
        pages[i] = m_tabs.GetIdxFromWindow(tabCtrl->GetWindowFromIdx(i));
    }

    return pages;
}

void wxAuiNotebook::Split(size_t page, int direction)
{
    if ( wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return;

    const wxAuiNotebookTopologyTransaction transaction(this);
    const wxWeakRef<wxAuiNotebook> weakThis(this);

    if ( page >= m_tabs.GetPageCount() )
        return;

    // get the page's window pointer
    wxWindow* const wnd = m_tabs.GetWindowFromIdx(page);
    if (!wnd)
        return;
    const wxWeakRef<wxWindow> weakPage(wnd);

    // notebooks with 1 or less pages can't be split
    if (m_tabs.GetPageCount() < 2)
        return;

    // find out which tab control the page currently belongs to
    const auto srcTabInfo = FindTab(wnd);
    if ( !srcTabInfo )
        return;

    wxAuiTabCtrl* const src_tabs = srcTabInfo.tabCtrl;
    const wxWeakRef<wxAuiTabCtrl> weakSourceTabs(src_tabs);
    const int srcIndex = srcTabInfo.tabIdx;
    const size_t mainCount = m_tabs.GetPageCount();
    const size_t sourceCount = src_tabs->GetPageCount();
    if ( srcIndex < 0 || static_cast<size_t>(srcIndex) >= sourceCount )
        return;

    std::vector<wxWindow*> canonicalPages;
    std::vector<wxWeakRef<wxWindow>> canonicalLifetimes;
    std::vector<wxWindow*> sourcePages;
    std::vector<wxWeakRef<wxWindow>> sourceLifetimes;
    for ( const wxAuiNotebookPage& info : m_tabs.GetPages() )
    {
        if ( !info.window ||
                wxWindowIsUnavailableForCallbacks(info.window) )
            return;
        canonicalPages.push_back(info.window);
        canonicalLifetimes.emplace_back(info.window);
    }
    for ( const wxAuiNotebookPage& info : src_tabs->GetPages() )
    {
        if ( !info.window ||
                wxWindowIsUnavailableForCallbacks(info.window) )
            return;
        sourcePages.push_back(info.window);
        sourceLifetimes.emplace_back(info.window);
    }

    // SplitPane() performs a synchronous manager layout, and CreateTabFrame()
    // invokes custom Clone(). Capture every pre-existing visual projection,
    // not just the source, before either callback boundary: an art provider
    // shared by panes can destroy a page or tab control in a different pane.
    struct ExistingTabRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
    };
    std::vector<ExistingTabRevision> existingTabs;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != this ||
                !wxAuiCapturePageProjection(*tabs, this, pages) )
        {
            return;
        }
        existingTabs.push_back(
            {frame, wxWeakRef<wxAuiTabFrame>(frame), tabs,
             wxWeakRef<wxAuiTabCtrl>(tabs), std::move(pages)});
    }

    std::vector<wxWindow*> movedSourcePages = sourcePages;
    std::vector<wxWeakRef<wxWindow>> movedSourceLifetimes = sourceLifetimes;
    movedSourcePages.erase(movedSourcePages.begin() + srcIndex);
    movedSourceLifetimes.erase(movedSourceLifetimes.begin() + srcIndex);

    wxWindow* const selectedBefore =
        m_curPage >= 0 && static_cast<size_t>(m_curPage) < mainCount
            ? canonicalPages[static_cast<size_t>(m_curPage)]
            : nullptr;
    const wxWeakRef<wxWindow> selectedBeforeLifetime(selectedBefore);

    const auto matchesPages = [](const wxAuiTabContainer& tabs,
                                 const std::vector<wxWindow*>& pages,
                                 const std::vector<wxWeakRef<wxWindow>>& weak,
                                 wxAuiNotebook* const book)
    {
        if ( tabs.GetPageCount() != pages.size() )
            return false;
        for ( size_t i = 0; i < pages.size(); ++i )
        {
            if ( weak[i].get() != pages[i] ||
                    wxWindowIsUnavailableForCallbacks(pages[i]) ||
                    pages[i]->GetParent() != book ||
                    tabs.GetWindowFromIdx(i) != pages[i] )
            {
                return false;
            }
        }
        return true;
    };

    const auto hasOriginalTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                weakPage.get() != wnd ||
                wxWindowIsUnavailableForCallbacks(wnd) ||
                wnd->GetParent() != book ||
                weakSourceTabs.get() != src_tabs ||
                wxWindowIsUnavailableForCallbacks(src_tabs) ||
                src_tabs->GetParent() != book ||
                book->m_tabs.GetPageCount() != mainCount ||
                page >= mainCount ||
                book->m_tabs.GetWindowFromIdx(page) != wnd ||
                srcIndex < 0 ||
                static_cast<size_t>(srcIndex) >=
                    src_tabs->GetPageCount() ||
                src_tabs->GetWindowFromIdx(
                    static_cast<size_t>(srcIndex)) != wnd ||
                !matchesPages(book->m_tabs, canonicalPages,
                              canonicalLifetimes, book) ||
                !matchesPages(*src_tabs, sourcePages,
                              sourceLifetimes, book) )
        {
            return nullptr;
        }

        for ( const ExistingTabRevision& expected : existingTabs )
        {
            if ( expected.frameLifetime.get() != expected.frame ||
                    expected.tabsLifetime.get() != expected.tabs ||
                    wxWindowIsUnavailableForCallbacks(expected.frame) ||
                    wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                    expected.frame->m_tabs != expected.tabs ||
                    expected.tabs->GetParent() != book ||
                    book->GetTabFrameFromTabCtrl(expected.tabs) !=
                        expected.frame ||
                    !wxAuiMatchesPageProjection(*expected.tabs, book,
                                                expected.pages) )
            {
                return nullptr;
            }
        }
        return book;
    };

    // Direct destruction of a child page doesn't enter a notebook topology
    // writer. If custom art does this while wxAuiManager is laying out the
    // provisional split, remove each expired identity from its one exact
    // visual projection and from the canonical projection. Every live frame,
    // tab control, page and order must still match the pre-callback snapshot;
    // any other mutation is authoritative and is not guessed away.
    const auto repairDestroyedPages = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                book->m_tabs.GetPageCount() != canonicalPages.size() )
        {
            return nullptr;
        }

        std::vector<size_t> deadCanonical;
        for ( size_t i = 0; i < canonicalPages.size(); ++i )
        {
            if ( book->m_tabs.GetWindowFromIdx(i) != canonicalPages[i] )
                return nullptr;

            wxWindow* const livePage = canonicalLifetimes[i].get();
            if ( !livePage )
            {
                deadCanonical.push_back(i);
            }
            else if ( livePage != canonicalPages[i] ||
                    wxWindowIsUnavailableForCallbacks(livePage) ||
                    livePage->GetParent() != book )
            {
                return nullptr;
            }
        }

        if ( deadCanonical.empty() )
            return nullptr;

        std::vector<std::vector<size_t>> deadVisual(existingTabs.size());
        size_t deadVisualCount = 0;
        for ( size_t tabIndex = 0; tabIndex < existingTabs.size();
              ++tabIndex )
        {
            const ExistingTabRevision& expected = existingTabs[tabIndex];
            wxAuiTabFrame* const frame = expected.frameLifetime.get();
            wxAuiTabCtrl* const tabs = expected.tabsLifetime.get();
            if ( frame != expected.frame || tabs != expected.tabs ||
                    !frame || !tabs ||
                    wxWindowIsUnavailableForCallbacks(frame) ||
                    wxWindowIsUnavailableForCallbacks(tabs) ||
                    frame->m_tabs != tabs || tabs->GetParent() != book ||
                    book->GetTabFrameFromTabCtrl(tabs) != frame ||
                    tabs->GetPageCount() != expected.pages.size() )
            {
                return nullptr;
            }

            for ( size_t i = 0; i < expected.pages.size(); ++i )
            {
                const wxAuiPageProjectionEntry& page = expected.pages[i];
                if ( tabs->GetWindowFromIdx(i) != page.page )
                    return nullptr;

                wxWindow* const livePage = page.lifetime.get();
                if ( !livePage )
                {
                    deadVisual[tabIndex].push_back(i);
                    ++deadVisualCount;
                }
                else if ( livePage != page.page ||
                        wxWindowIsUnavailableForCallbacks(livePage) ||
                        livePage->GetParent() != book )
                {
                    return nullptr;
                }
            }
        }

        // Each canonical page occurs in exactly one visual projection. This
        // equality plus identity checks below rejects missing, duplicated or
        // cross-pane repair guesses.
        if ( deadVisualCount != deadCanonical.size() )
            return nullptr;
        for ( const size_t canonicalIndex : deadCanonical )
        {
            size_t occurrences = 0;
            for ( size_t tabIndex = 0; tabIndex < existingTabs.size();
                  ++tabIndex )
            {
                for ( const size_t pageIndex : deadVisual[tabIndex] )
                {
                    if ( existingTabs[tabIndex].pages[pageIndex].page ==
                            canonicalPages[canonicalIndex] )
                    {
                        ++occurrences;
                    }
                }
            }
            if ( occurrences != 1 )
                return nullptr;
        }

        for ( size_t tabIndex = 0; tabIndex < existingTabs.size();
              ++tabIndex )
        {
            wxAuiTabCtrl* const tabs =
                existingTabs[tabIndex].tabsLifetime.get();
            for ( auto i = deadVisual[tabIndex].rbegin();
                  i != deadVisual[tabIndex].rend(); ++i )
            {
                tabs->m_pages.RemoveAt(*i);
                wxAuiBumpTabContainerRevision(tabs);
            }
        }
        for ( auto i = deadCanonical.rbegin();
              i != deadCanonical.rend(); ++i )
        {
            book->m_tabs.m_pages.RemoveAt(*i);
            wxAuiBumpTabContainerRevision(&book->m_tabs);
        }

        wxWindow* const selected = selectedBeforeLifetime.get();
        book->m_curPage = selected && selected == selectedBefore &&
                                  !wxWindowIsUnavailableForCallbacks(selected)
                              ? book->m_tabs.GetIdxFromWindow(selected)
                              : wxNOT_FOUND;
        return book;
    };

    wxAuiNotebookPage page_info = *srcTabInfo.pageInfo;
    page_info.active = false;

    const wxSize splitSize = CalculateNewSplitSize();
    if ( !hasOriginalTopology() )
        return;

    // create a new tab frame
    wxAuiTabFrame* new_tabs = CreateTabFrame(splitSize);
    if ( !new_tabs )
        return;
    wxAuiTabCtrl* const dest_tabs = new_tabs->m_tabs;
    const wxWeakRef<wxAuiTabFrame> weakNewFrame(new_tabs);
    const wxWeakRef<wxAuiTabCtrl> weakDestinationTabs(dest_tabs);

    // Until the source projection is erased, the new pane is wholly
    // provisional. Remove and destroy it on every pre-commit exit, but only
    // while it is still our exact empty frame: callbacks are allowed to take
    // it over and must not have their work undone.
    const auto rollbackUncommittedFrame = [&]()
    {
        wxAuiNotebook* rollbackBook = weakThis.get();
        if ( !rollbackBook ||
                wxWindowIsUnavailableForCallbacks(rollbackBook) ||
                weakNewFrame.get() != new_tabs ||
                wxWindowIsUnavailableForCallbacks(new_tabs) ||
                weakDestinationTabs.get() != dest_tabs ||
                wxWindowIsUnavailableForCallbacks(dest_tabs) ||
                dest_tabs->GetParent() != rollbackBook ||
                dest_tabs->GetPageCount() != 0 )
        {
            return;
        }

        wxWindow* const managedFrame =
            rollbackBook->GetTabFrameFromTabCtrl(dest_tabs);
        if ( managedFrame && managedFrame != new_tabs )
            return;
        if ( managedFrame == new_tabs )
            rollbackBook->m_mgr.DetachPane(new_tabs);

        rollbackBook = weakThis.get();
        if ( rollbackBook &&
                !wxWindowIsUnavailableForCallbacks(rollbackBook) &&
                weakNewFrame.get() == new_tabs &&
                !wxWindowIsUnavailableForCallbacks(new_tabs) &&
                weakDestinationTabs.get() == dest_tabs &&
                !wxWindowIsUnavailableForCallbacks(dest_tabs) &&
                dest_tabs->GetPageCount() == 0 &&
                rollbackBook->GetTabFrameFromTabCtrl(dest_tabs) == nullptr )
        {
            delete new_tabs;
        }
    };
    wxScopeGuard uncommittedFrameGuard =
        wxMakeGuard(rollbackUncommittedFrame);

    wxAuiNotebook* book = hasOriginalTopology();
    if ( !book || weakNewFrame.get() != new_tabs ||
            weakDestinationTabs.get() != dest_tabs ||
            dest_tabs->GetParent() != book )
    {
        rollbackUncommittedFrame();
        return;
    }

    wxWindow* const sourceFrame = book->GetTabFrameFromTabCtrl(src_tabs);
    if ( !sourceFrame )
    {
        rollbackUncommittedFrame();
        return;
    }

    book->m_mgr.SplitPane(sourceFrame, new_tabs, direction);

    book = hasOriginalTopology();
    if ( !book || weakNewFrame.get() != new_tabs ||
            weakDestinationTabs.get() != dest_tabs ||
            dest_tabs->GetParent() != book ||
            book->GetTabFrameFromTabCtrl(dest_tabs) != new_tabs )
    {
        wxAuiNotebook* const repairedBook = repairDestroyedPages();
        rollbackUncommittedFrame();
        if ( repairedBook && weakThis.get() == repairedBook &&
                !wxWindowIsUnavailableForCallbacks(repairedBook) )
        {
            repairedBook->RemoveEmptyTabFrames();
            wxAuiNotebook* const liveBook = weakThis.get();
            if ( liveBook &&
                    !wxWindowIsUnavailableForCallbacks(liveBook) )
            {
                liveBook->DoSizing();
            }
        }
        return;
    }

    // Publish the move without invoking an art provider between the source
    // and destination projections. Insert first so allocation failure leaves
    // the original projection untouched; erasing the exact source then cannot
    // throw.
    try
    {
        auto destinationButtons = MakePageButtons(dest_tabs->m_flags);
        const auto inserted = dest_tabs->m_pages.insert(
            dest_tabs->m_pages.begin(), page_info);
        wxAuiBumpTabContainerRevision(dest_tabs);
        inserted->buttons = std::move(destinationButtons);
        wxAuiBumpTabContainerRevision(dest_tabs);
    }
    catch ( const std::exception& )
    {
        if ( weakDestinationTabs.get() == dest_tabs &&
                !wxWindowIsUnavailableForCallbacks(dest_tabs) &&
                dest_tabs->m_pages.GetCount() == 1 &&
                dest_tabs->m_pages[0].window == wnd )
        {
            dest_tabs->m_pages.RemoveAt(0);
            wxAuiBumpTabContainerRevision(dest_tabs);
        }
        rollbackUncommittedFrame();
        return;
    }

    if ( !hasOriginalTopology() )
    {
        if ( weakDestinationTabs.get() == dest_tabs &&
                dest_tabs->m_pages.GetCount() != 0 &&
                dest_tabs->m_pages[0].window == wnd )
        {
            dest_tabs->m_pages.RemoveAt(0);
            wxAuiBumpTabContainerRevision(dest_tabs);
        }
        rollbackUncommittedFrame();
        return;
    }
    src_tabs->m_pages.RemoveAt(static_cast<size_t>(srcIndex));
    wxAuiBumpTabContainerRevision(src_tabs);
    uncommittedFrameGuard.Dismiss();

    const auto hasMovedTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = weakThis.get();
        return current && !wxWindowIsUnavailableForCallbacks(current) &&
                       weakPage.get() == wnd &&
                       !wxWindowIsUnavailableForCallbacks(wnd) &&
                       wnd->GetParent() == current &&
                       weakSourceTabs.get() == src_tabs &&
                       weakDestinationTabs.get() == dest_tabs &&
                       !wxWindowIsUnavailableForCallbacks(src_tabs) &&
                       !wxWindowIsUnavailableForCallbacks(dest_tabs) &&
                       src_tabs->GetParent() == current &&
                       dest_tabs->GetParent() == current &&
                       current->m_tabs.GetPageCount() == mainCount &&
                       page < mainCount &&
                       current->m_tabs.GetWindowFromIdx(page) == wnd &&
                       matchesPages(current->m_tabs, canonicalPages,
                                    canonicalLifetimes, current) &&
                       matchesPages(*src_tabs, movedSourcePages,
                                    movedSourceLifetimes, current) &&
                       src_tabs->GetIdxFromWindow(wnd) == wxNOT_FOUND &&
                       dest_tabs->GetPageCount() == 1 &&
                       dest_tabs->GetWindowFromIdx(0) == wnd
                    ? current
                    : nullptr;
    };

    const auto hasFinalMovedTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = weakThis.get();
        if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                weakPage.get() != wnd ||
                wxWindowIsUnavailableForCallbacks(wnd) ||
                wnd->GetParent() != current ||
                weakDestinationTabs.get() != dest_tabs ||
                wxWindowIsUnavailableForCallbacks(dest_tabs) ||
                dest_tabs->GetParent() != current ||
                dest_tabs->GetPageCount() != 1 ||
                dest_tabs->GetWindowFromIdx(0) != wnd ||
                !matchesPages(current->m_tabs, canonicalPages,
                              canonicalLifetimes, current) )
        {
            return nullptr;
        }
        wxAuiTabCtrl* const currentSource = weakSourceTabs.get();
        if ( movedSourcePages.empty() &&
                (!currentSource ||
                 wxWindowIsUnavailableForCallbacks(currentSource)) )
        {
            return current;
        }
        return currentSource == src_tabs &&
                       !wxWindowIsUnavailableForCallbacks(src_tabs) &&
                       src_tabs->GetParent() == current &&
                       matchesPages(*src_tabs, movedSourcePages,
                                    movedSourceLifetimes, current)
                    ? current
                    : nullptr;
    };

    // Only now may custom art see the new page counts: both projections
    // already describe the same topology at either callback boundary.
    if ( src_tabs->m_art )
    {
        wxAuiSetTabArtSizingInfo(
            src_tabs->m_art, src_tabs->m_rect.GetSize(),
            src_tabs->m_pages.GetCount(), src_tabs);
    }
    book = hasMovedTopology();
    if ( !book )
        return;
    if ( dest_tabs->m_art )
    {
        wxAuiSetTabArtSizingInfo(
            dest_tabs->m_art, dest_tabs->m_rect.GetSize(),
            dest_tabs->m_pages.GetCount(), dest_tabs);
    }
    book = hasMovedTopology();
    if ( !book )
        return;

    if (src_tabs->GetPageCount() > 0)
    {
        src_tabs->SetActivePage((size_t)0);
        src_tabs->DoUpdateActive();
        book = hasMovedTopology();
        if ( !book )
            return;
    }

    if (src_tabs->GetPageCount() == 0)
    {
        book->RemoveEmptyTabFrames();
        book = hasFinalMovedTopology();
        if ( !book )
            return;
    }

    book->DoSizing();
    book = hasFinalMovedTopology();
    if ( !book )
        return;
    dest_tabs->DoUpdateActive();
    book = hasFinalMovedTopology();
    if ( !book )
        return;

    // force the set selection function reset the selection
    book->m_curPage = wxNOT_FOUND;

    // set the active page to the one we just split off
    book->SetSelectionToPage(page_info);

    // SetSelectionToPage() enters the public selection event pipeline.
    book = hasFinalMovedTopology();
    if ( !book )
        return;
    book->UpdateHintWindowSize();
}

void wxAuiNotebook::UnsplitAll()
{
    if ( wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return;

    const wxAuiNotebookTopologyTransaction transaction(this);
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const size_t canonicalCount = m_tabs.GetPageCount();

    auto* const tabMain = GetMainTabCtrl();
    if ( !tabMain )
        return;
    const wxWeakRef<wxAuiTabCtrl> weakMain(tabMain);
    wxAuiNotebook* book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
            weakMain.get() != tabMain ||
            wxWindowIsUnavailableForCallbacks(tabMain) ||
            tabMain->GetParent() != book )
    {
        return;
    }

    wxAuiPageProjection canonicalProjection;
    if ( !wxAuiCapturePageProjection(book->m_tabs, book,
                                     canonicalProjection) )
    {
        return;
    }

    // Snapshot the complete manager revision before the first custom-art
    // boundary. A shared provider in one pane may destroy a different tab
    // control which hasn't been visited yet.
    struct TabRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
    };
    std::vector<TabRevision> managerTabs;
    std::vector<wxWeakRef<wxAuiTabCtrl>> tabs;
    for ( const auto& pane : book->m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        auto* const tab = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tab || frame->m_tabs != tab ||
                wxWindowIsUnavailableForCallbacks(tab) ||
                tab->GetParent() != book ||
                !wxAuiCapturePageProjection(*tab, book, pages) )
        {
            return;
        }
        managerTabs.push_back(
            {frame, wxWeakRef<wxAuiTabFrame>(frame), tab,
             wxWeakRef<wxAuiTabCtrl>(tab), std::move(pages)});
        if ( tab != tabMain )
            tabs.emplace_back(tab);
    }
    const size_t paneCount = book->m_mgr.GetAllPanes().GetCount();
    const auto hasManagerRevision =
        [&](wxAuiTabCtrl* const firstOverride,
            const wxAuiPageProjection* const firstPages,
            wxAuiTabCtrl* const secondOverride,
            const wxAuiPageProjection* const secondPages) -> wxAuiNotebook*
        {
            wxAuiNotebook* const current = weakThis.get();
            if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                    current->m_mgr.GetAllPanes().GetCount() != paneCount ||
                    !wxAuiMatchesPageProjection(current->m_tabs, current,
                                                canonicalProjection) )
            {
                return nullptr;
            }

            for ( const TabRevision& expected : managerTabs )
            {
                if ( expected.frameLifetime.get() != expected.frame ||
                        expected.tabsLifetime.get() != expected.tabs ||
                        wxWindowIsUnavailableForCallbacks(expected.frame) ||
                        wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                        expected.frame->m_tabs != expected.tabs ||
                        expected.tabs->GetParent() != current ||
                        current->GetTabFrameFromTabCtrl(expected.tabs) !=
                            expected.frame )
                {
                    return nullptr;
                }

                const wxAuiPageProjection* pages = &expected.pages;
                if ( expected.tabs == firstOverride )
                    pages = firstPages;
                else if ( expected.tabs == secondOverride )
                    pages = secondPages;
                if ( !pages || !wxAuiMatchesPageProjection(
                        *expected.tabs, current, *pages) )
                {
                    return nullptr;
                }
            }
            return current;
        };

    if ( !hasManagerRevision(nullptr, nullptr, nullptr, nullptr) )
        return;

    bool changed = false;
    for ( const wxWeakRef<wxAuiTabCtrl>& weakTab : tabs )
    {
        wxAuiTabCtrl* const tab = weakTab.get();
        book = hasManagerRevision(nullptr, nullptr, nullptr, nullptr);
        if ( !book ||
                !tab || wxWindowIsUnavailableForCallbacks(tab) ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                tabMain->GetParent() != book || tab->GetParent() != book ||
                !book->GetTabFrameFromTabCtrl(tab) )
        {
            return;
        }

        while ( tab->GetPageCount() )
        {
            wxAuiNotebookPage info = tab->GetPage(0);
            info.active = false;
            wxWindow* const page = info.window;
            const wxWeakRef<wxWindow> weakPage(page);
            const size_t sourceCountBefore = tab->GetPageCount();
            const size_t mainCountBefore = tabMain->GetPageCount();

            wxAuiPageProjection mainBefore;
            wxAuiPageProjection sourceBefore;
            if ( !wxAuiCapturePageProjection(*tabMain, book, mainBefore) ||
                    !wxAuiCapturePageProjection(*tab, book, sourceBefore) )
            {
                return;
            }
            wxAuiPageProjection mainAfter = mainBefore;
            wxAuiPageProjection sourceAfter = sourceBefore;
            try
            {
                mainAfter.push_back(sourceBefore[0]);
            }
            catch ( const std::exception& )
            {
                return;
            }
            sourceAfter.erase(sourceAfter.begin());

            if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
                    page->GetParent() != book ||
                    book->m_tabs.GetIdxFromWindow(page) == wxNOT_FOUND )
            {
                return;
            }

            // As in Split(), don't expose an intermediate missing/duplicated
            // projection to custom art. Allocation happens before the exact
            // source entry is erased.
            try
            {
                auto destinationButtons = MakePageButtons(tabMain->m_flags);
                const auto inserted = tabMain->m_pages.insert(
                    tabMain->m_pages.end(), info);
                wxAuiBumpTabContainerRevision(tabMain);
                inserted->buttons = std::move(destinationButtons);
                wxAuiBumpTabContainerRevision(tabMain);
            }
            catch ( const std::exception& )
            {
                return;
            }

            book = weakThis.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    weakPage.get() != page ||
                    wxWindowIsUnavailableForCallbacks(page) ||
                    weakMain.get() != tabMain || weakTab.get() != tab ||
                    wxWindowIsUnavailableForCallbacks(tabMain) ||
                    wxWindowIsUnavailableForCallbacks(tab) ||
                    tabMain->GetPageCount() != mainCountBefore + 1 ||
                    tabMain->GetWindowFromIdx(mainCountBefore) != page ||
                    tab->GetPageCount() != sourceCountBefore ||
                    tab->GetWindowFromIdx(0) != page )
            {
                if ( weakMain.get() == tabMain &&
                        !wxWindowIsUnavailableForCallbacks(tabMain) &&
                        tabMain->GetPageCount() == mainCountBefore + 1 &&
                        tabMain->GetWindowFromIdx(mainCountBefore) == page )
                {
                    tabMain->m_pages.RemoveAt(mainCountBefore);
                    wxAuiBumpTabContainerRevision(tabMain);
                }
                return;
            }

            tab->m_pages.RemoveAt(0);
            wxAuiBumpTabContainerRevision(tab);

            const auto hasCommittedMove = [&]() -> wxAuiNotebook*
            {
                wxAuiNotebook* const current = weakThis.get();
                return current &&
                               !wxWindowIsUnavailableForCallbacks(current) &&
                               hasManagerRevision(tabMain, &mainAfter,
                                                  tab, &sourceAfter) ==
                                   current &&
                               weakPage.get() == page &&
                               !wxWindowIsUnavailableForCallbacks(page) &&
                               page->GetParent() == current &&
                               weakMain.get() == tabMain &&
                               weakTab.get() == tab &&
                               !wxWindowIsUnavailableForCallbacks(tabMain) &&
                               !wxWindowIsUnavailableForCallbacks(tab) &&
                               tabMain->GetParent() == current &&
                               tab->GetParent() == current &&
                               current->m_tabs.GetPageCount() ==
                                   canonicalCount &&
                               current->m_tabs.GetIdxFromWindow(page) !=
                                   wxNOT_FOUND &&
                               tabMain->GetPageCount() ==
                                   mainCountBefore + 1 &&
                               tabMain->GetWindowFromIdx(mainCountBefore) ==
                                   page &&
                               tab->GetPageCount() == sourceCountBefore - 1 &&
                               tab->GetIdxFromWindow(page) == wxNOT_FOUND &&
                               wxAuiMatchesPageProjection(current->m_tabs,
                                                          current,
                                                          canonicalProjection) &&
                               wxAuiMatchesPageProjection(*tabMain, current,
                                                          mainAfter) &&
                               wxAuiMatchesPageProjection(*tab, current,
                                                          sourceAfter)
                            ? current
                            : nullptr;
            };

            if ( tab->m_art )
            {
                wxAuiSetTabArtSizingInfo(
                    tab->m_art, tab->m_rect.GetSize(),
                    tab->m_pages.GetCount(), tab);
            }
            book = hasCommittedMove();
            if ( !book )
                return;
            if ( tabMain->m_art )
            {
                wxAuiSetTabArtSizingInfo(
                    tabMain->m_art, tabMain->m_rect.GetSize(),
                    tabMain->m_pages.GetCount(), tabMain);
            }
            book = hasCommittedMove();
            if ( !book )
                return;

            for ( TabRevision& revision : managerTabs )
            {
                if ( revision.tabs == tabMain )
                    revision.pages = std::move(mainAfter);
                else if ( revision.tabs == tab )
                    revision.pages = std::move(sourceAfter);
            }

            changed = true;
        }
    }

    if ( changed )
    {
        wxAuiPageProjection finalMainProjection;
        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                hasManagerRevision(nullptr, nullptr, nullptr, nullptr) !=
                    book ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiCapturePageProjection(*tabMain, book,
                                             finalMainProjection) )
        {
            return;
        }

        // We need to update the selection if the current page was in another
        // tab control before, so force a selection change to ensure that the
        // right page is shown.
        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                hasManagerRevision(nullptr, nullptr, nullptr, nullptr) !=
                    book ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                tabMain->GetParent() != book ||
                tabMain->GetPageCount() != canonicalCount ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiMatchesPageProjection(*tabMain, book,
                                            finalMainProjection) )
        {
            return;
        }

        book->m_curPage = wxNOT_FOUND;
        int projectedSelection = tabMain->GetActivePage();
        if ( projectedSelection == wxNOT_FOUND )
        {
            // Not sure if this can actually happen, but fall back to the first
            // page if it does (note that we know that the main tab control is
            // not empty as we must have added a page to it above for "changed"
            // to be true).
            projectedSelection = 0;
        }

        wxWindow* const selectedPage = tabMain->GetWindowFromIdx(
            static_cast<size_t>(projectedSelection));
        const wxWeakRef<wxWindow> weakSelectedPage(selectedPage);
        const int canonicalSelection =
            book->m_tabs.GetIdxFromWindow(selectedPage);
        if ( canonicalSelection == wxNOT_FOUND )
            return;

        // The active index belongs to the visual projection. Selection APIs
        // consume the canonical m_tabs index, which can differ after split and
        // display-order moves.
        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                hasManagerRevision(nullptr, nullptr, nullptr, nullptr) !=
                    book ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                weakSelectedPage.get() != selectedPage ||
                tabMain->GetParent() != book ||
                tabMain->GetPageCount() != canonicalCount ||
                static_cast<size_t>(projectedSelection) >=
                    tabMain->GetPageCount() ||
                tabMain->GetWindowFromIdx(
                    static_cast<size_t>(projectedSelection)) != selectedPage ||
                static_cast<size_t>(canonicalSelection) >= canonicalCount ||
                book->m_tabs.GetWindowFromIdx(
                    static_cast<size_t>(canonicalSelection)) != selectedPage ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiMatchesPageProjection(*tabMain, book,
                                            finalMainProjection) )
        {
            return;
        }

        book->ChangeSelection(static_cast<size_t>(canonicalSelection));

        // ChangeSelection() enters the focus/layout pipeline. The remaining
        // frame cleanup and layout are only valid for the exact fully-unsplit
        // projection committed above.
        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                hasManagerRevision(nullptr, nullptr, nullptr, nullptr) !=
                    book ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                tabMain->GetParent() != book ||
                book->m_tabs.GetPageCount() != canonicalCount ||
                tabMain->GetPageCount() != canonicalCount ||
                weakSelectedPage.get() != selectedPage ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiMatchesPageProjection(*tabMain, book,
                                            finalMainProjection) )
        {
            return;
        }

        book->RemoveEmptyTabFrames();

        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                tabMain->GetParent() != book ||
                book->m_tabs.GetPageCount() != canonicalCount ||
                tabMain->GetPageCount() != canonicalCount ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiMatchesPageProjection(*tabMain, book,
                                            finalMainProjection) )
        {
            return;
        }
        book->DoSizing();

        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                weakMain.get() != tabMain ||
                wxWindowIsUnavailableForCallbacks(tabMain) ||
                tabMain->GetParent() != book ||
                book->m_tabs.GetPageCount() != canonicalCount ||
                tabMain->GetPageCount() != canonicalCount ||
                !wxAuiMatchesPageProjection(book->m_tabs, book,
                                            canonicalProjection) ||
                !wxAuiMatchesPageProjection(*tabMain, book,
                                            finalMainProjection) )
        {
            return;
        }
        book->UpdateHintWindowSize();
    }
}

void wxAuiNotebook::OnSize(wxSizeEvent& evt)
{
    UpdateHintWindowSize();

    evt.Skip();
}

void wxAuiNotebook::OnTabClicked(wxAuiTabCtrl* ctrl, int tabIdx)
{
    wxWindow* const wnd = ctrl->GetWindowFromIdx(tabIdx);
    wxASSERT(wnd != nullptr);

    // Note that this will generate wxEVT_AUINOTEBOOK_PAGE_CHANG{ING,ED} events.
    SetSelectionToWindow(wnd);
}

void wxAuiNotebook::OnTabBgDClick(wxAuiTabCtrl* ctrl)
{
    // select the tab ctrl which received the db click
    const int selection = ctrl->GetActivePage();
    if ( selection != wxNOT_FOUND )
    {
        if ( wxWindow* const wnd = ctrl->GetWindowFromIdx(selection) )
            SetSelectionToWindow(wnd);
    }

    // notify owner that the tabbar background has been double-clicked
    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BG_DCLICK, m_windowId);
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabBeginDrag(wxAuiTabCtrl* ctrl, int tabIdx)
{
    m_lastDropMovePos = -1;

    wxWindow* const wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_BEGIN_DRAG, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetOldSelection(e.GetSelection());
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabDragMotion(wxAuiTabCtrl* src_tabs, int src_idx)
{
    if ( !src_tabs || wxWindowIsUnavailableForCallbacks(src_tabs) ||
            src_idx < 0 ||
            static_cast<size_t>(src_idx) >= src_tabs->GetPageCount() )
    {
        return;
    }

    wxWindow* const wnd = src_tabs->GetWindowFromIdx(src_idx);
    if ( !wnd || wxWindowIsUnavailableForCallbacks(wnd) )
        return;

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxAuiTabCtrl> weakSourceTabs(src_tabs);
    const wxWeakRef<wxWindow> weakPage(wnd);
    const size_t canonicalCount = m_tabs.GetPageCount();
    const size_t sourceCount = src_tabs->GetPageCount();
    const int canonicalIndex = m_tabs.GetIdxFromWindow(wnd);
    if ( canonicalIndex == wxNOT_FOUND )
        return;

    std::vector<wxWindow*> canonicalPages;
    std::vector<wxWeakRef<wxWindow>> canonicalLifetimes;
    canonicalPages.reserve(canonicalCount);
    canonicalLifetimes.reserve(canonicalCount);
    for ( const wxAuiNotebookPage& page : m_tabs.GetPages() )
    {
        if ( !page.window ||
                wxWindowIsUnavailableForCallbacks(page.window) )
        {
            return;
        }
        canonicalPages.push_back(page.window);
        canonicalLifetimes.emplace_back(page.window);
    }

    std::vector<wxWindow*> sourcePages;
    std::vector<wxWeakRef<wxWindow>> sourceLifetimes;
    sourcePages.reserve(sourceCount);
    sourceLifetimes.reserve(sourceCount);
    for ( const wxAuiNotebookPage& page : src_tabs->GetPages() )
    {
        if ( !page.window ||
                wxWindowIsUnavailableForCallbacks(page.window) )
        {
            return;
        }
        sourcePages.push_back(page.window);
        sourceLifetimes.emplace_back(page.window);
    }

    const auto hasOriginalTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                weakSourceTabs.get() != src_tabs ||
                wxWindowIsUnavailableForCallbacks(src_tabs) ||
                src_tabs->GetParent() != book ||
                weakPage.get() != wnd ||
                wxWindowIsUnavailableForCallbacks(wnd) ||
                wnd->GetParent() != book ||
                book->m_tabs.GetPageCount() != canonicalCount ||
                book->m_tabs.GetIdxFromWindow(wnd) != canonicalIndex ||
                src_tabs->GetPageCount() != sourceCount ||
                static_cast<size_t>(src_idx) >= sourceCount ||
                src_tabs->GetWindowFromIdx(
                    static_cast<size_t>(src_idx)) != wnd )
        {
            return nullptr;
        }
        for ( size_t i = 0; i < canonicalCount; ++i )
        {
            if ( canonicalLifetimes[i].get() != canonicalPages[i] ||
                    wxWindowIsUnavailableForCallbacks(canonicalPages[i]) ||
                    canonicalPages[i]->GetParent() != book ||
                    book->m_tabs.GetWindowFromIdx(i) != canonicalPages[i] )
            {
                return nullptr;
            }
        }
        for ( size_t i = 0; i < sourceCount; ++i )
        {
            if ( sourceLifetimes[i].get() != sourcePages[i] ||
                    wxWindowIsUnavailableForCallbacks(sourcePages[i]) ||
                    sourcePages[i]->GetParent() != book ||
                    src_tabs->GetWindowFromIdx(i) != sourcePages[i] )
            {
                return nullptr;
            }
        }
        return book;
    };

    if ( !hasOriginalTopology() )
        return;

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_DRAG_MOTION, m_windowId);
    e.SetSelection(canonicalIndex);
    e.SetOldSelection(e.GetSelection());
    e.SetEventObject(this);
    ProcessWindowEvent(e);

    if ( !hasOriginalTopology() )
        return;

    wxPoint screen_pt = ::wxGetMousePosition();
    wxPoint client_pt = ScreenToClient(screen_pt);

    wxAuiTabCtrl* const dest_tabs = GetTabCtrlFromPoint(client_pt);

    if (dest_tabs == src_tabs)
    {
        src_tabs->SetCursor(wxCursor(wxCURSOR_ARROW));

        // always hide the hint for inner-tabctrl drag
        m_mgr.HideHint();

        // if tab moving is not allowed, leave
        if (!(m_flags & wxAUI_NB_TAB_MOVE))
        {
            return;
        }

        wxPoint pt = dest_tabs->ScreenToClient(screen_pt);

        // When using multiple rows, allow dragging the tab to the space after
        // the last tab of the row too.
        int flags = wxAuiTabContainer::HitTest_Default;
        if (dest_tabs->IsFlagSet(wxAUI_NB_MULTILINE))
            flags |= wxAuiTabContainer::HitTest_AllowAfterTab;

        // this is an inner-tab drag/reposition
        if (auto const destTabInfo = dest_tabs->TabHitTest(pt, flags))
        {
            wxCHECK_RET( src_idx != -1, "Invalid source tab?" );

            int dest_idx = destTabInfo.pos;

            if ( src_idx == dest_idx )
            {
                // Reset the last drop position, the mouse has moved away from
                // the other tab, so now the user should be able to drag this
                // one there again.
                m_lastDropMovePos = -1;
                return;
            }

            const auto& src_page = src_tabs->GetPage(src_idx);

            // A tab can only be moved inside the group of tabs of the same
            // kind, as otherwise the tabs of the same kind wouldn't be grouped
            // together any longer.
            if ( dest_tabs->GetPage(dest_idx).kind != src_page.kind )
                return;

            // When dragging a smaller tab over the larger one, after moving
            // the tab into the new position, the same point can be now over a
            // different tab, which would result in another move of this tab as
            // soon as the mouse moves even by a single pixel. And then, of
            // course, it would move again, and so on. To avoid this, we need
            // to check if the mouse has moved from the tab over which it was
            // when the last drop occurred.
            if ( dest_idx == m_lastDropMovePos )
                return;

            if (dest_tabs->MovePage(src_page.window, dest_idx))
            {
                std::vector<wxWindow*> movedPages = sourcePages;
                std::vector<wxWeakRef<wxWindow>> movedLifetimes =
                    sourceLifetimes;
                const auto moveExpectedPages =
                    [src_idx, dest_idx](std::vector<wxWindow*>& pages)
                {
                    const auto begin = pages.begin();
                    if ( src_idx < dest_idx )
                    {
                        std::rotate(begin + src_idx, begin + src_idx + 1,
                                    begin + dest_idx + 1);
                    }
                    else
                    {
                        std::rotate(begin + dest_idx, begin + src_idx,
                                    begin + src_idx + 1);
                    }
                };
                const auto moveExpectedLifetimes =
                    [src_idx, dest_idx](
                        std::vector<wxWeakRef<wxWindow>>& pages)
                {
                    const auto begin = pages.begin();
                    if ( src_idx < dest_idx )
                    {
                        std::rotate(begin + src_idx, begin + src_idx + 1,
                                    begin + dest_idx + 1);
                    }
                    else
                    {
                        std::rotate(begin + dest_idx, begin + src_idx,
                                    begin + src_idx + 1);
                    }
                };
                moveExpectedPages(movedPages);
                moveExpectedLifetimes(movedLifetimes);

                const auto hasMovedTopology = [&]() -> wxAuiNotebook*
                {
                    wxAuiNotebook* const book = weakThis.get();
                    if ( !book ||
                            wxWindowIsUnavailableForCallbacks(book) ||
                            weakSourceTabs.get() != src_tabs ||
                            wxWindowIsUnavailableForCallbacks(src_tabs) ||
                            src_tabs->GetParent() != book ||
                            weakPage.get() != wnd ||
                            wxWindowIsUnavailableForCallbacks(wnd) ||
                            wnd->GetParent() != book ||
                            book->m_tabs.GetPageCount() != canonicalCount ||
                            book->m_tabs.GetIdxFromWindow(wnd) !=
                                canonicalIndex ||
                            src_tabs->GetPageCount() != sourceCount ||
                            dest_idx < 0 ||
                            static_cast<size_t>(dest_idx) >= sourceCount ||
                            src_tabs->GetWindowFromIdx(
                                static_cast<size_t>(dest_idx)) != wnd )
                    {
                        return nullptr;
                    }
                    for ( size_t i = 0; i < canonicalCount; ++i )
                    {
                        if ( canonicalLifetimes[i].get() !=
                                canonicalPages[i] ||
                                wxWindowIsUnavailableForCallbacks(
                                    canonicalPages[i]) ||
                                canonicalPages[i]->GetParent() != book ||
                                book->m_tabs.GetWindowFromIdx(i) !=
                                    canonicalPages[i] )
                        {
                            return nullptr;
                        }
                    }
                    for ( size_t i = 0; i < sourceCount; ++i )
                    {
                        if ( movedLifetimes[i].get() != movedPages[i] ||
                                wxWindowIsUnavailableForCallbacks(
                                    movedPages[i]) ||
                                movedPages[i]->GetParent() != book ||
                                src_tabs->GetWindowFromIdx(i) != movedPages[i] )
                        {
                            return nullptr;
                        }
                    }
                    return book;
                };

                if ( !hasMovedTopology() )
                    return;

                // Update the layout when using multiline tabs as it can change
                // depending on the tab order.
                if (dest_tabs->IsFlagSet(wxAUI_NB_MULTILINE))
                {
                    dest_tabs->LayoutMultiLineTabs(dest_tabs);
                    if ( !hasMovedTopology() )
                        return;
                }

                dest_tabs->SetActivePage((size_t)dest_idx);
                if ( !hasMovedTopology() )
                    return;
                dest_tabs->DoUpdateActive();
                if ( !hasMovedTopology() )
                    return;
            }

            m_lastDropMovePos = dest_tabs->TabHitTest(pt).pos;
        }

        return;
    }


    // if external drag is allowed, check if the tab is being dragged
    // over a different wxAuiNotebook control
    if (m_flags & wxAUI_NB_TAB_EXTERNAL_MOVE)
    {
        wxWindow* tab_ctrl = ::wxFindWindowAtPoint(screen_pt);

        // if we aren't over any window, stop here
        if (!tab_ctrl)
            return;

        // make sure we are not over the hint window
        if (!wxDynamicCast(tab_ctrl, wxFrame))
        {
            wxAuiTabCtrl* other_tabs = nullptr;
            while (tab_ctrl)
            {
                other_tabs = wxDynamicCast(tab_ctrl, wxAuiTabCtrl);
                if (other_tabs)
                    break;
                tab_ctrl = tab_ctrl->GetParent();
            }

            if (other_tabs)
            {
                wxAuiNotebook* nb = (wxAuiNotebook*)tab_ctrl->GetParent();

                if (nb != this)
                {
                    m_mgr.UpdateHint(other_tabs->GetHintScreenRect());
                    return;
                }
            }
        }
        else
        {
            if (!dest_tabs)
            {
                // we are either over a hint window, or not over a tab
                // window, and there is no where to drag to, so exit
                return;
            }
        }
    }


    // if there are less than two panes, split can't happen, so leave
    if (m_tabs.GetPageCount() < 2)
        return;

    // if tab moving is not allowed, leave
    if (!(m_flags & wxAUI_NB_TAB_SPLIT))
        return;


    if (src_tabs)
    {
        src_tabs->SetCursor(wxCursor(wxCURSOR_SIZING));
    }


    wxRect hintRect;
    if (dest_tabs)
    {
        if (src_tabs)
        {
            const auto& src_page = src_tabs->GetPage(src_idx);
            if (GetDropIndex(src_page, dest_tabs, screen_pt) == wxNOT_FOUND)
            {
                m_mgr.HideHint();
                return;
            }
        }

        hintRect = dest_tabs->GetHintScreenRect();
    }
    else
    {
        hintRect = m_mgr.CalculateHintRect(m_dummyWnd, client_pt);
    }

    m_mgr.UpdateHint(hintRect);
}
void wxAuiNotebook::OnTabEndDragForTesting(wxAuiTabCtrl* const srcTabs,
                                           const int srcIndex,
                                           wxAuiTabCtrl* const destination,
                                           const int destinationIndex,
                                           const bool createNewPane)
{
    wxCHECK_RET(!gs_auiNotebookEndDragOverride,
                "nested deterministic AUI drag override");
    wxCHECK_RET(!createNewPane || !destination,
                "a new-pane drag cannot have an existing destination");

    const wxAuiNotebookEndDragOverride override =
        {this, srcTabs, destination, destinationIndex, createNewPane};
    gs_auiNotebookEndDragOverride = &override;
    wxScopeGuard resetOverride = wxMakeGuard([]()
    {
        gs_auiNotebookEndDragOverride = nullptr;
    });
    wxUnusedVar(resetOverride);

    OnTabEndDrag(srcTabs, srcIndex);
}

void wxAuiNotebook::OnTabEndDrag(wxAuiTabCtrl* src_tabs, int src_idx)
{
    if ( wxWindowIsUnavailableForCallbacks(this) ||
            wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
        return;

    wxAuiNotebookTopologyTransaction sourceTransaction(this);
    const wxWeakRef<wxAuiNotebook> weakSource(this);

    if ( !src_tabs || wxWindowIsUnavailableForCallbacks(src_tabs) ||
            src_tabs->GetParent() != this || src_idx < 0 ||
            static_cast<size_t>(src_idx) >= src_tabs->GetPageCount() )
    {
        return;
    }

    wxWindow* const src_page = src_tabs->GetWindowFromIdx(src_idx);
    if ( !src_page || wxWindowIsUnavailableForCallbacks(src_page) ||
            src_page->GetParent() != this )
        return;

    const wxWeakRef<wxAuiTabCtrl> weakSourceTabs(src_tabs);
    const wxWeakRef<wxWindow> weakSourcePage(src_page);
    const size_t sourceMainCount = m_tabs.GetPageCount();
    const size_t sourceTabCount = src_tabs->GetPageCount();
    const int sourceMainIndex = m_tabs.GetIdxFromWindow(src_page);
    if ( sourceMainIndex == wxNOT_FOUND )
        return;

    wxAuiPageProjection sourceCanonicalBefore;
    wxAuiPageProjection sourceTabsBefore;
    if ( !wxAuiCapturePageProjection(m_tabs, this,
                                     sourceCanonicalBefore) ||
            !wxAuiCapturePageProjection(*src_tabs, this,
                                        sourceTabsBefore) )
    {
        return;
    }
    wxAuiPageProjection sourceCanonicalAfter = sourceCanonicalBefore;
    wxAuiPageProjection sourceTabsAfter = sourceTabsBefore;
    sourceCanonicalAfter.erase(
        sourceCanonicalAfter.begin() + sourceMainIndex);
    sourceTabsAfter.erase(sourceTabsAfter.begin() + src_idx);

    struct DragFrameRevision
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
        wxAuiPageProjection pages;
    };
    std::vector<DragFrameRevision> sourceFrames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        wxAuiPageProjection pages;
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                tabs->GetParent() != this ||
                !wxAuiCapturePageProjection(*tabs, this, pages) )
        {
            return;
        }
        sourceFrames.push_back(
            {frame, wxWeakRef<wxAuiTabFrame>(frame), tabs,
             wxWeakRef<wxAuiTabCtrl>(tabs), std::move(pages)});
    }
    wxAuiTabFrame* dragSourceFrame = nullptr;
    for ( const DragFrameRevision& frame : sourceFrames )
    {
        if ( frame.tabs == src_tabs )
        {
            dragSourceFrame = frame.frame;
            break;
        }
    }
    if ( !dragSourceFrame )
        return;
    const wxWeakRef<wxAuiTabFrame> weakDragSourceFrame(dragSourceFrame);

    const size_t sourcePaneCount = m_mgr.GetAllPanes().GetCount();
    const auto hasSourceManagerRevision =
        [&](const size_t expectedPaneCount,
            const wxAuiPageProjection& canonical,
            wxAuiTabCtrl* const firstOverride,
            const wxAuiPageProjection* const firstPages,
            wxAuiTabCtrl* const secondOverride,
            const wxAuiPageProjection* const secondPages) -> wxAuiNotebook*
        {
            wxAuiNotebook* const current = weakSource.get();
            if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                    current->m_mgr.GetAllPanes().GetCount() !=
                        expectedPaneCount ||
                    !wxAuiMatchesPageProjection(current->m_tabs, current,
                                                canonical) )
            {
                return nullptr;
            }
            for ( const DragFrameRevision& expected : sourceFrames )
            {
                if ( expected.frameLifetime.get() != expected.frame ||
                        expected.tabsLifetime.get() != expected.tabs ||
                        wxWindowIsUnavailableForCallbacks(expected.frame) ||
                        wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                        expected.frame->m_tabs != expected.tabs ||
                        expected.tabs->GetParent() != current ||
                        current->GetTabFrameFromTabCtrl(expected.tabs) !=
                            expected.frame )
                {
                    return nullptr;
                }
                const wxAuiPageProjection* pages = &expected.pages;
                if ( expected.tabs == firstOverride )
                    pages = firstPages;
                else if ( expected.tabs == secondOverride )
                    pages = secondPages;
                if ( !pages || !wxAuiMatchesPageProjection(
                        *expected.tabs, current, *pages) )
                {
                    return nullptr;
                }
            }
            return current;
        };

    const auto hasOriginalSourceTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const source = weakSource.get();
        return source && !wxWindowIsUnavailableForCallbacks(source) &&
                       hasSourceManagerRevision(
                           sourcePaneCount, sourceCanonicalBefore,
                           nullptr, nullptr, nullptr, nullptr) == source &&
                       weakSourceTabs.get() == src_tabs &&
                       !wxWindowIsUnavailableForCallbacks(src_tabs) &&
                       weakSourcePage.get() == src_page &&
                       !wxWindowIsUnavailableForCallbacks(src_page) &&
                       src_tabs->GetParent() == source &&
                       src_page->GetParent() == source &&
                       source->m_tabs.GetPageCount() == sourceMainCount &&
                       source->m_tabs.GetIdxFromWindow(src_page) !=
                           wxNOT_FOUND &&
                       src_tabs->GetPageCount() == sourceTabCount &&
                       static_cast<size_t>(src_idx) < sourceTabCount &&
                       src_tabs->GetWindowFromIdx(
                           static_cast<size_t>(src_idx)) == src_page &&
                       wxAuiMatchesPageProjection(source->m_tabs, source,
                                                  sourceCanonicalBefore) &&
                       wxAuiMatchesPageProjection(*src_tabs, source,
                                                  sourceTabsBefore)
                    ? source
                    : nullptr;
    };

    // The source rollback owns only the canonical projection, the exact
    // source tab control and the still-source-owned page. It must remain
    // possible when an unrelated source pane was invalidated by a callback.
    const auto hasLocalSourceProjectionAfter = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const source = weakSource.get();
        if ( !source || wxWindowIsUnavailableForCallbacks(source) ||
                !wxAuiMatchesPageProjection(source->m_tabs, source,
                                            sourceCanonicalAfter) ||
                weakSourcePage.get() != src_page ||
                wxWindowIsUnavailableForCallbacks(src_page) ||
                src_page->GetParent() != source )
        {
            return nullptr;
        }
        wxAuiTabCtrl* const tabs = weakSourceTabs.get();
        return tabs == src_tabs &&
                       !wxWindowIsUnavailableForCallbacks(src_tabs) &&
                       src_tabs->GetParent() == source &&
                       weakDragSourceFrame.get() == dragSourceFrame &&
                       !wxWindowIsUnavailableForCallbacks(dragSourceFrame) &&
                       dragSourceFrame->m_tabs == src_tabs &&
                       source->m_mgr.GetPane(dragSourceFrame).IsOk() &&
                       wxAuiMatchesPageProjection(*src_tabs, source,
                                                  sourceTabsAfter)
                    ? source
                    : nullptr;
    };

    const auto hasLocalOriginalSourceProjection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const source = weakSource.get();
        wxAuiTabCtrl* const tabs = weakSourceTabs.get();
        return source && !wxWindowIsUnavailableForCallbacks(source) &&
                       tabs == src_tabs &&
                       !wxWindowIsUnavailableForCallbacks(src_tabs) &&
                       src_tabs->GetParent() == source &&
                       weakDragSourceFrame.get() == dragSourceFrame &&
                       !wxWindowIsUnavailableForCallbacks(dragSourceFrame) &&
                       dragSourceFrame->m_tabs == src_tabs &&
                       source->m_mgr.GetPane(dragSourceFrame).IsOk() &&
                       weakSourcePage.get() == src_page &&
                       !wxWindowIsUnavailableForCallbacks(src_page) &&
                       src_page->GetParent() == source &&
                       wxAuiMatchesPageProjection(
                           source->m_tabs, source,
                           sourceCanonicalBefore) &&
                       wxAuiMatchesPageProjection(
                           *src_tabs, source, sourceTabsBefore)
                    ? source
                    : nullptr;
    };

    const auto hasSourceProjectionAfter = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const source = hasLocalSourceProjectionAfter();
        return source && hasSourceManagerRevision(
                            sourcePaneCount, sourceCanonicalAfter,
                            src_tabs, &sourceTabsAfter,
                            nullptr, nullptr) == source
                    ? source
                    : nullptr;
    };

    const auto hasDetachedSourceTopology = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const source = hasSourceProjectionAfter();
        return source && weakSourcePage.get() == src_page &&
                       !wxWindowIsUnavailableForCallbacks(src_page) &&
                       src_page->GetParent() == source
                    ? source
                    : nullptr;
    };

    wxAuiNotebook* source = hasOriginalSourceTopology();
    if ( !source )
        return;

    source->m_mgr.HideHint();
    source = hasOriginalSourceTopology();
    if ( !source )
        return;

    src_tabs->SetCursor(wxCursor(wxCURSOR_ARROW));
    source = hasOriginalSourceTopology();
    if ( !source )
        return;

    const wxAuiNotebookEndDragOverride* dragOverride =
        gs_auiNotebookEndDragOverride;
    if ( dragOverride )
    {
        if ( dragOverride->book != source ||
                dragOverride->bookLifetime.get() != source ||
                dragOverride->sourceTabs != src_tabs ||
                dragOverride->sourceTabsLifetime.get() != src_tabs )
        {
            dragOverride = nullptr;
        }
        else if ( dragOverride->destinationTabs &&
                  (dragOverride->destinationTabsLifetime.get() !=
                       dragOverride->destinationTabs ||
                   wxWindowIsUnavailableForCallbacks(
                       dragOverride->destinationTabs) ||
                   dragOverride->destinationParentLifetime.get() !=
                       dragOverride->destinationParent ||
                   (dragOverride->destinationParent &&
                    wxWindowIsUnavailableForCallbacks(
                        dragOverride->destinationParent)) ||
                   dragOverride->destinationTabs->GetParent() !=
                       dragOverride->destinationParent) )
        {
            // HideHint() and SetCursor() above are callback boundaries. A
            // deterministic destination destroyed or reparented by either
            // one cannot be replaced with ambient mouse hit-testing.
            return;
        }
    }

    // Get the mouse position used for a normal drag. The deterministic test
    // seam replaces only the hit target/index; wxDefaultPosition makes
    // SplitPane() use its normal direction-derived drop point.
    const wxPoint mouse_screen_pt = dragOverride
        ? source->ClientToScreen(wxPoint(0, 0))
        : ::wxGetMousePosition();
    const wxPoint mouse_client_pt = dragOverride &&
                                            dragOverride->createNewPane
        ? wxDefaultPosition
        : source->ScreenToClient(mouse_screen_pt);



    // check for an external move
    if (source->m_flags & wxAUI_NB_TAB_EXTERNAL_MOVE)
    {
        wxWindow* tab_ctrl = dragOverride &&
                                     !dragOverride->createNewPane
            ? dragOverride->destinationTabsLifetime.get()
            : ::wxFindWindowAtPoint(mouse_screen_pt);

        while (tab_ctrl)
        {
            if (wxDynamicCast(tab_ctrl, wxAuiTabCtrl))
                break;
            tab_ctrl = tab_ctrl->GetParent();
        }

        if (tab_ctrl)
        {
            auto* const destinationTabs =
                wxDynamicCast(tab_ctrl, wxAuiTabCtrl);
            auto* const destination = destinationTabs
                ? wxDynamicCast(destinationTabs->GetParent(), wxAuiNotebook)
                : nullptr;

            if (destination && destination != source)
            {
                if ( wxAuiNotebookTopologyTransaction::IsActiveFor(
                        destination) )
                {
                    return;
                }

                wxAuiNotebookTopologyTransaction
                    destinationTransaction(destination);
                const wxWeakRef<wxAuiNotebook> weakDestination(destination);
                const wxWeakRef<wxAuiTabCtrl> weakDestinationTabs(
                    destinationTabs);
                const size_t destinationMainCount =
                    destination->m_tabs.GetPageCount();
                const size_t destinationTabCount =
                    destinationTabs->GetPageCount();
                wxAuiPageProjection destinationCanonicalBefore;
                wxAuiPageProjection destinationTabsBefore;
                if ( !wxAuiCapturePageProjection(destination->m_tabs,
                                                  destination,
                                                  destinationCanonicalBefore) ||
                        !wxAuiCapturePageProjection(*destinationTabs,
                                                    destination,
                                                    destinationTabsBefore) )
                {
                    return;
                }

                std::vector<DragFrameRevision> destinationFrames;
                for ( const auto& pane :
                      destination->m_mgr.GetAllPanes() )
                {
                    if ( IsDummyPane(pane) )
                        continue;
                    auto* const frame =
                        static_cast<wxAuiTabFrame*>(pane.window);
                    if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
                        return;
                    wxAuiTabCtrl* const tabs =
                        frame->m_tabsLifetime.get();
                    wxAuiPageProjection pages;
                    if ( !tabs || frame->m_tabs != tabs ||
                            wxWindowIsUnavailableForCallbacks(tabs) ||
                            tabs->GetParent() != destination ||
                            !wxAuiCapturePageProjection(
                                *tabs, destination, pages) )
                    {
                        return;
                    }
                    destinationFrames.push_back(
                        {frame, wxWeakRef<wxAuiTabFrame>(frame), tabs,
                         wxWeakRef<wxAuiTabCtrl>(tabs), std::move(pages)});
                }
                const size_t destinationPaneCount =
                    destination->m_mgr.GetAllPanes().GetCount();
                const auto hasDestinationManagerRevision =
                    [&](const wxAuiPageProjection& canonical,
                        wxAuiTabCtrl* const overrideTabs,
                        const wxAuiPageProjection* const overridePages)
                        -> wxAuiNotebook*
                    {
                        wxAuiNotebook* const current = weakDestination.get();
                        if ( !current ||
                                wxWindowIsUnavailableForCallbacks(current) ||
                                current->m_mgr.GetAllPanes().GetCount() !=
                                    destinationPaneCount ||
                                !wxAuiMatchesPageProjection(
                                    current->m_tabs, current, canonical) )
                        {
                            return nullptr;
                        }
                        for ( const DragFrameRevision& expected :
                              destinationFrames )
                        {
                            if ( expected.frameLifetime.get() !=
                                    expected.frame ||
                                    expected.tabsLifetime.get() !=
                                        expected.tabs ||
                                    wxWindowIsUnavailableForCallbacks(
                                        expected.frame) ||
                                    wxWindowIsUnavailableForCallbacks(
                                        expected.tabs) ||
                                    expected.frame->m_tabs != expected.tabs ||
                                    expected.tabs->GetParent() != current ||
                                    current->GetTabFrameFromTabCtrl(
                                        expected.tabs) != expected.frame )
                            {
                                return nullptr;
                            }
                            const wxAuiPageProjection& pages =
                                expected.tabs == overrideTabs
                                    ? *overridePages
                                    : expected.pages;
                            if ( !wxAuiMatchesPageProjection(
                                    *expected.tabs, current, pages) )
                            {
                                return nullptr;
                            }
                        }
                        return current;
                    };

                const auto hasOriginalDestinationTopology =
                    [&]() -> wxAuiNotebook*
                    {
                        wxAuiNotebook* const current = weakDestination.get();
                        return current &&
                                       !wxWindowIsUnavailableForCallbacks(
                                           current) &&
                                       hasDestinationManagerRevision(
                                           destinationCanonicalBefore,
                                           nullptr, nullptr) == current &&
                                       weakDestinationTabs.get() ==
                                           destinationTabs &&
                                       !wxWindowIsUnavailableForCallbacks(
                                           destinationTabs) &&
                                       destinationTabs->GetParent() ==
                                           current &&
                                       current->GetTabFrameFromTabCtrl(
                                           destinationTabs) &&
                                       current->m_tabs.GetPageCount() ==
                                           destinationMainCount &&
                                       destinationTabs->GetPageCount() ==
                                           destinationTabCount &&
                                       current->m_tabs.GetIdxFromWindow(
                                           src_page) == wxNOT_FOUND &&
                                       destinationTabs->GetIdxFromWindow(
                                           src_page) == wxNOT_FOUND &&
                                       wxAuiMatchesPageProjection(
                                           current->m_tabs, current,
                                           destinationCanonicalBefore) &&
                                       wxAuiMatchesPageProjection(
                                           *destinationTabs, current,
                                           destinationTabsBefore)
                                    ? current
                                    : nullptr;
                    };

                if ( !hasOriginalSourceTopology() ||
                        !hasOriginalDestinationTopology() )
                {
                    return;
                }

                // find out from the destination control
                // if it's ok to drop this tab here
                wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_ALLOW_DND,
                                     source->m_windowId);
                e.SetSelection(src_idx);
                e.SetOldSelection(e.GetSelection());
                e.SetEventObject(source);
                e.SetDragSource(source);
                e.Veto(); // dropping must be explicitly approved by control owner

                destination->ProcessWindowEvent(e);

                source = hasOriginalSourceTopology();
                wxAuiNotebook* liveDestination =
                    hasOriginalDestinationTopology();
                if ( !source || !liveDestination )
                {
                    return;
                }

                if (!e.IsAllowed())
                {
                    // no answer or negative answer
                    source->m_mgr.HideHint();
                    return;
                }

                // drop was allowed

                // Check that it's not an impossible parent relationship
                wxWindow* p = liveDestination;
                while (p && !p->IsTopLevel())
                {
                    if (p == src_page)
                    {
                        return;
                    }
                    p = p->GetParent();
                }

                // get main index of the page
                const int main_idx =
                    source->m_tabs.GetIdxFromWindow(src_page);
                wxCHECK_RET( main_idx != wxNOT_FOUND, wxT("no source page?") );


                // Capture the exact source metadata and selection before the
                // destructive half of the transfer. The destination copy
                // clears hover, but rollback restores the untouched canonical
                // and per-tab records.
                const wxAuiNotebookPage sourceCanonicalPageInfo =
                    source->m_tabs.GetPage(main_idx);
                const wxAuiNotebookPage sourceTabPageInfo =
                    src_tabs->GetPage(static_cast<size_t>(src_idx));
                const int sourceSelectionBefore = source->m_curPage;
                wxWindow* const sourceSelectedBefore =
                    sourceSelectionBefore >= 0 &&
                            static_cast<size_t>(sourceSelectionBefore) <
                                source->m_tabs.GetPageCount()
                        ? source->m_tabs.GetWindowFromIdx(
                              static_cast<size_t>(sourceSelectionBefore))
                        : nullptr;
                const wxWeakRef<wxWindow> sourceSelectedBeforeLifetime(
                    sourceSelectedBefore);
                const int sourceActiveIndexBefore = src_tabs->GetActivePage();
                wxWindow* const sourceActiveBefore =
                    sourceActiveIndexBefore >= 0 &&
                            static_cast<size_t>(sourceActiveIndexBefore) <
                                src_tabs->GetPageCount()
                        ? src_tabs->GetWindowFromIdx(
                              static_cast<size_t>(sourceActiveIndexBefore))
                        : nullptr;
                const wxWeakRef<wxWindow> sourceActiveBeforeLifetime(
                    sourceActiveBefore);

                wxAuiNotebookPage page_info = sourceCanonicalPageInfo;
                page_info.hover = false;

                // found out the insert idx
                int insert_idx = -1;
                if ( dragOverride &&
                        dragOverride->destinationTabsLifetime.get() ==
                            destinationTabs )
                {
                    insert_idx = dragOverride->destinationIndex;
                }
                else
                {
                    const wxPoint pt =
                        destinationTabs->ScreenToClient(mouse_screen_pt);
                    if ( auto const targetInfo =
                            destinationTabs->TabHitTest(pt) )
                    {
                        insert_idx = targetInfo.pos;
                    }
                }

                const size_t expectedDestinationTabIndex = insert_idx == -1
                    ? destinationTabCount
                    : static_cast<size_t>(insert_idx);
                if ( expectedDestinationTabIndex > destinationTabCount )
                    return;
                wxAuiPageProjection destinationCanonicalAfter =
                    destinationCanonicalBefore;
                wxAuiPageProjection destinationTabsAfter =
                    destinationTabsBefore;
                try
                {
                    destinationCanonicalAfter.push_back(
                        sourceCanonicalBefore[
                            static_cast<size_t>(sourceMainIndex)]);
                    destinationTabsAfter.insert(
                        destinationTabsAfter.begin() +
                            expectedDestinationTabIndex,
                        sourceCanonicalBefore[
                            static_cast<size_t>(sourceMainIndex)]);
                }
                catch ( const std::exception& )
                {
                    return;
                }

                // Remove only after every identity needed by the transfer is
                // weakly captured. A removal callback can destroy either
                // notebook, the page, or either tab controller. Preserve the
                // exact source control if it becomes empty: destination
                // publication can still fail and rollback must restore the
                // original visual pane, tab index and complete page metadata.
                {
                    const wxAuiNotebookPreserveTabCtrl preserveSourceCtrl(
                        source, src_tabs);
                    const wxAuiNotebookTopologyTransaction::InternalCall
                        allowSourcePrimitive(sourceTransaction);
                    if ( !source->RemovePage(
                            static_cast<size_t>(main_idx)) )
                    {
                        return;
                    }
                }

                // From this point until an exact destination commit, the
                // page is deliberately absent from both source projections.
                // Arm rollback immediately: destination callbacks may destroy
                // or invalidate that notebook before InsertPageAt() is even
                // entered, while the still-source-owned page remains fully
                // recoverable.
                const auto restoreDetachedSource = [&]()
                {
                    wxAuiNotebook* rollbackSource =
                        hasLocalSourceProjectionAfter();
                    wxAuiTabCtrl* const rollbackTabs =
                        weakSourceTabs.get();
                    wxWindow* const rollbackPage = weakSourcePage.get();
                    if ( !rollbackSource || rollbackTabs != src_tabs ||
                            !rollbackTabs ||
                            wxWindowIsUnavailableForCallbacks(rollbackTabs) ||
                            rollbackTabs->GetParent() != rollbackSource ||
                            weakDragSourceFrame.get() != dragSourceFrame ||
                            wxWindowIsUnavailableForCallbacks(
                                dragSourceFrame) ||
                            dragSourceFrame->m_tabs != rollbackTabs ||
                            !rollbackSource->m_mgr.GetPane(
                                dragSourceFrame).IsOk() ||
                            rollbackPage != src_page || !rollbackPage ||
                            wxWindowIsUnavailableForCallbacks(rollbackPage) )
                    {
                        return;
                    }

                    wxWindow* currentParent = rollbackPage->GetParent();
                    if ( currentParent != rollbackSource )
                    {
                        wxAuiNotebook* const rollbackDestination =
                            hasOriginalDestinationTopology();
                        if ( !rollbackDestination ||
                                currentParent != rollbackDestination )
                        {
                            // A third owner (or successful but subsequently
                            // mutated destination) consumed the page.
                            return;
                        }

                        (void)rollbackPage->Reparent(rollbackSource);
                        rollbackSource = hasLocalSourceProjectionAfter();
                        if ( !rollbackSource ||
                                weakSourcePage.get() != rollbackPage ||
                                wxWindowIsUnavailableForCallbacks(
                                    rollbackPage) ||
                                rollbackPage->GetParent() != rollbackSource )
                        {
                            return;
                        }
                    }
                    else
                    {
                        // Don't create duplicate ownership if an invalid
                        // destination nevertheless retained a projection.
                        wxAuiNotebook* const currentDestination =
                            weakDestination.get();
                        wxAuiTabCtrl* const currentDestinationTabs =
                            weakDestinationTabs.get();
                        if ( currentDestination &&
                                !wxWindowIsUnavailableForCallbacks(
                                    currentDestination) &&
                                currentDestination->m_tabs.GetIdxFromWindow(
                                    rollbackPage) != wxNOT_FOUND )
                        {
                            return;
                        }
                        if ( currentDestinationTabs &&
                                !wxWindowIsUnavailableForCallbacks(
                                    currentDestinationTabs) &&
                                currentDestinationTabs->GetIdxFromWindow(
                                    rollbackPage) != wxNOT_FOUND )
                        {
                            return;
                        }
                    }

                    rollbackSource = hasLocalSourceProjectionAfter();
                    if ( !rollbackSource ||
                            rollbackSource->m_tabs.GetIdxFromWindow(
                                rollbackPage) != wxNOT_FOUND ||
                            rollbackTabs->GetIdxFromWindow(rollbackPage) !=
                                wxNOT_FOUND )
                    {
                        return;
                    }

                    if ( main_idx < 0 || src_idx < 0 ||
                            static_cast<size_t>(main_idx) >
                                rollbackSource->m_tabs.GetPageCount() ||
                            static_cast<size_t>(src_idx) >
                                rollbackTabs->GetPageCount() )
                    {
                        return;
                    }

                    // Restore just the two projections owned by this scope.
                    // Calling InsertPageAt() here would revalidate every
                    // manager pane and make rollback impossible precisely when
                    // an unrelated sibling was invalidated by the removal
                    // callback.
                    bool canonicalInserted = false;
                    try
                    {
                        rollbackSource->m_tabs.m_pages.insert(
                            rollbackSource->m_tabs.m_pages.begin() + main_idx,
                            sourceCanonicalPageInfo);
                        canonicalInserted = true;
                        wxAuiBumpTabContainerRevision(
                            &rollbackSource->m_tabs);
                        rollbackTabs->m_pages.insert(
                            rollbackTabs->m_pages.begin() + src_idx,
                            sourceTabPageInfo);
                        wxAuiBumpTabContainerRevision(rollbackTabs);
                    }
                    catch ( const std::exception& )
                    {
                        if ( canonicalInserted &&
                                static_cast<size_t>(main_idx) <
                                    rollbackSource->m_tabs.GetPageCount() &&
                                rollbackSource->m_tabs.GetWindowFromIdx(
                                    static_cast<size_t>(main_idx)) ==
                                    rollbackPage )
                        {
                            rollbackSource->m_tabs.m_pages.RemoveAt(
                                static_cast<size_t>(main_idx));
                            wxAuiBumpTabContainerRevision(
                                &rollbackSource->m_tabs);
                        }
                        return;
                    }

                    rollbackSource = hasLocalOriginalSourceProjection();
                    if ( !rollbackSource )
                        return;

                    wxWindow* const selected =
                        sourceSelectedBeforeLifetime.get();
                    rollbackSource->m_curPage =
                        selected == sourceSelectedBefore && selected &&
                                !wxWindowIsUnavailableForCallbacks(selected)
                            ? rollbackSource->m_tabs.GetIdxFromWindow(selected)
                            : wxNOT_FOUND;
                    if ( rollbackSource->m_curPage != wxNOT_FOUND )
                        rollbackSource->m_tabs.SetActivePage(selected);
                    else
                        rollbackSource->m_tabs.SetNoneActive();

                    wxWindow* const active =
                        sourceActiveBeforeLifetime.get();
                    if ( active == sourceActiveBefore && active &&
                            !wxWindowIsUnavailableForCallbacks(active) &&
                            rollbackTabs->GetIdxFromWindow(active) !=
                                wxNOT_FOUND )
                    {
                        rollbackTabs->SetActivePage(active);
                    }
                    else
                    {
                        rollbackTabs->SetNoneActive();
                    }

                    if ( rollbackTabs->m_art )
                    {
                        wxAuiSetTabArtSizingInfo(
                            rollbackTabs->m_art,
                            rollbackTabs->m_rect.GetSize(),
                            rollbackTabs->m_pages.GetCount(), rollbackTabs);
                        rollbackSource = hasLocalOriginalSourceProjection();
                        if ( !rollbackSource )
                            return;
                    }
                    if ( rollbackSource->m_tabs.m_art )
                    {
                        wxAuiSetTabArtSizingInfo(
                            rollbackSource->m_tabs.m_art,
                            rollbackSource->m_tabs.m_rect.GetSize(),
                            rollbackSource->m_tabs.m_pages.GetCount(),
                            rollbackSource);
                        rollbackSource = hasLocalOriginalSourceProjection();
                        if ( !rollbackSource )
                            return;
                    }

                    rollbackTabs->DoUpdateActive();
                    (void)hasLocalOriginalSourceProjection();
                };
                wxScopeGuard detachedSourceGuard =
                    wxMakeGuard(restoreDetachedSource);

                source = hasDetachedSourceTopology();
                liveDestination = hasOriginalDestinationTopology();
                if ( !source || !liveDestination ||
                        destinationTabs->GetParent() != liveDestination )
                {
                    return;
                }


                // add the page to the new notebook: note that we always append
                // it to the end in logical order, as it's not clear what
                // should its position be (the alternative would be to insert
                // it before the target page, but it's not really clear if this
                // is really what we want)
                {
                    const wxAuiNotebookTopologyTransaction::InternalCall
                        allowDestinationPrimitive(destinationTransaction);
                    liveDestination->InsertPageAt(
                        page_info, liveDestination->m_tabs.GetPageCount(),
                        destinationTabs, insert_idx, true /* select */);
                }

                source = weakSource.get();
                liveDestination = weakDestination.get();
                const bool transferred =
                    hasSourceProjectionAfter() && liveDestination &&
                    !wxWindowIsUnavailableForCallbacks(liveDestination) &&
                    hasDestinationManagerRevision(
                        destinationCanonicalAfter, destinationTabs,
                        &destinationTabsAfter) == liveDestination &&
                    weakSourcePage.get() == src_page &&
                    !wxWindowIsUnavailableForCallbacks(src_page) &&
                    weakDestinationTabs.get() == destinationTabs &&
                    !wxWindowIsUnavailableForCallbacks(destinationTabs) &&
                    src_page->GetParent() == liveDestination &&
                    source->m_tabs.GetPageCount() + 1 == sourceMainCount &&
                    source->m_tabs.GetIdxFromWindow(src_page) ==
                        wxNOT_FOUND &&
                    liveDestination->m_tabs.GetPageCount() ==
                        destinationMainCount + 1 &&
                    liveDestination->m_tabs.GetWindowFromIdx(
                        destinationMainCount) == src_page &&
                    destinationTabs->GetPageCount() ==
                        destinationTabCount + 1 &&
                    expectedDestinationTabIndex <
                        destinationTabs->GetPageCount() &&
                    destinationTabs->GetWindowFromIdx(
                        expectedDestinationTabIndex) == src_page &&
                    wxAuiMatchesPageProjection(
                        liveDestination->m_tabs, liveDestination,
                        destinationCanonicalAfter) &&
                    wxAuiMatchesPageProjection(
                        *destinationTabs, liveDestination,
                        destinationTabsAfter);
                if ( !transferred )
                {
                    return;
                }

                detachedSourceGuard.Dismiss();

                // The now-empty preserved source pane is no longer needed
                // after destination commit. Remove it only after the transfer
                // is proven exact; failure here cannot invalidate destination
                // ownership of the page.
                if ( sourceTabsAfter.empty() )
                {
                    source->RemoveEmptyTabFrames();
                    source = weakSource.get();
                    liveDestination = weakDestination.get();
                    if ( !source ||
                            wxWindowIsUnavailableForCallbacks(source) ||
                            !liveDestination ||
                            wxWindowIsUnavailableForCallbacks(
                                liveDestination) ||
                            weakSourcePage.get() != src_page ||
                            wxWindowIsUnavailableForCallbacks(src_page) ||
                            src_page->GetParent() != liveDestination )
                    {
                        return;
                    }
                }

                // notify owner that the tab has been dragged
                source = weakSource.get();
                if ( !source || wxWindowIsUnavailableForCallbacks(source) ||
                        weakSourcePage.get() != src_page ||
                        wxWindowIsUnavailableForCallbacks(src_page) ||
                        source->m_tabs.GetIdxFromWindow(src_page) !=
                            wxNOT_FOUND )
                    return;
                wxAuiNotebookEvent e2(
                    wxEVT_AUINOTEBOOK_DRAG_DONE, source->m_windowId);
                e2.SetSelection(src_idx);
                e2.SetOldSelection(src_idx);
                e2.SetEventObject(source);
                source->ProcessWindowEvent(e2);

                return;
            }
        }
    }




    // only perform a tab split if it's allowed
    wxAuiTabCtrl* dest_tabs = nullptr;

    source = hasOriginalSourceTopology();
    if ( !source )
        return;

    if ((source->m_flags & wxAUI_NB_TAB_SPLIT) &&
            source->m_tabs.GetPageCount() >= 2)
    {
        wxAuiNotebookPage page_info = src_tabs->GetPage(src_idx);

        // If the pointer is in an existing tab frame, do a tab insert.
        wxWindow* const hit_wnd = dragOverride
            ? (dragOverride->createNewPane
                   ? nullptr
                   : dragOverride->destinationTabsLifetime.get())
            : ::wxFindWindowAtPoint(mouse_screen_pt);
        wxAuiTabFrame* tab_frame = static_cast<wxAuiTabFrame*>(
            source->GetTabFrameFromTabCtrl(hit_wnd));
        int insert_idx = -1;
        wxWeakRef<wxAuiTabFrame> weakNewFrame;
        wxWeakRef<wxAuiTabCtrl> weakCreatedTabs;
        wxAuiTabFrame* provisionalFrame = nullptr;
        wxAuiTabCtrl* provisionalTabs = nullptr;
        bool rollbackProvisionalFrame = false;
        wxScopeGuard provisionalFrameGuard = wxMakeGuard([&]()
        {
            if ( !rollbackProvisionalFrame )
                return;
            wxAuiNotebook* book = weakSource.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                    weakNewFrame.get() != provisionalFrame ||
                    !provisionalFrame ||
                    wxWindowIsUnavailableForCallbacks(provisionalFrame) ||
                    weakCreatedTabs.get() != provisionalTabs ||
                    !provisionalTabs ||
                    wxWindowIsUnavailableForCallbacks(provisionalTabs) ||
                    provisionalTabs->GetParent() != book )
            {
                return;
            }
            if ( provisionalTabs->GetPageCount() == 1 &&
                    provisionalTabs->GetWindowFromIdx(0) == src_page )
            {
                provisionalTabs->m_pages.RemoveAt(0);
                wxAuiBumpTabContainerRevision(provisionalTabs);
            }
            if ( provisionalTabs->GetPageCount() != 0 )
                return;
            if ( book->m_mgr.GetPane(provisionalFrame).IsOk() )
                book->m_mgr.DetachPane(provisionalFrame);
            book = weakSource.get();
            if ( !book || wxWindowIsUnavailableForCallbacks(book) )
                return;
            if ( weakNewFrame.get() == provisionalFrame &&
                    !wxWindowIsUnavailableForCallbacks(provisionalFrame) &&
                    !book->m_mgr.GetPane(provisionalFrame).IsOk() )
            {
                delete provisionalFrame;
            }
        });

        // Validate the exact manager topology for each publication stage of
        // an internal move. The destination can be either one of the original
        // frames or a provisional frame added by SplitPane(); the source frame
        // can disappear only after its last tab has committed elsewhere.
        const auto hasInternalManagerRevision =
            [&](const wxAuiPageProjection& expectedSourcePages,
                wxAuiTabCtrl* const destination,
                const wxAuiPageProjection& expectedDestinationPages,
                wxAuiTabFrame* const addedFrame,
                wxAuiTabCtrl* const addedTabs,
                const bool sourceFrameRemoved) -> wxAuiNotebook*
            {
                wxAuiNotebook* const current = weakSource.get();
                const size_t expectedPaneCount = sourcePaneCount +
                    (addedFrame ? 1u : 0u) -
                    (sourceFrameRemoved ? 1u : 0u);
                if ( !current || wxWindowIsUnavailableForCallbacks(current) ||
                        current->m_mgr.GetAllPanes().GetCount() !=
                            expectedPaneCount ||
                        !wxAuiMatchesPageProjection(
                            current->m_tabs, current,
                            sourceCanonicalBefore) ||
                        weakSourcePage.get() != src_page ||
                        wxWindowIsUnavailableForCallbacks(src_page) ||
                        src_page->GetParent() != current )
                {
                    return nullptr;
                }

                bool destinationFound = false;
                for ( const DragFrameRevision& expected : sourceFrames )
                {
                    if ( expected.tabs == src_tabs && sourceFrameRemoved )
                    {
                        if ( expected.frameLifetime.get() ||
                                current->m_mgr.GetPane(expected.frame).IsOk() )
                        {
                            return nullptr;
                        }
                        continue;
                    }

                    if ( expected.frameLifetime.get() != expected.frame ||
                            expected.tabsLifetime.get() != expected.tabs ||
                            wxWindowIsUnavailableForCallbacks(expected.frame) ||
                            wxWindowIsUnavailableForCallbacks(expected.tabs) ||
                            expected.frame->m_tabs != expected.tabs ||
                            expected.tabs->GetParent() != current ||
                            current->GetTabFrameFromTabCtrl(expected.tabs) !=
                                expected.frame )
                    {
                        return nullptr;
                    }

                    const wxAuiPageProjection* pages = &expected.pages;
                    if ( expected.tabs == src_tabs )
                        pages = &expectedSourcePages;
                    else if ( expected.tabs == destination )
                    {
                        pages = &expectedDestinationPages;
                        destinationFound = true;
                    }
                    if ( !wxAuiMatchesPageProjection(
                            *expected.tabs, current, *pages) )
                    {
                        return nullptr;
                    }
                }

                if ( addedFrame )
                {
                    if ( weakNewFrame.get() != addedFrame ||
                            weakCreatedTabs.get() != addedTabs ||
                            !addedTabs || destination != addedTabs ||
                            wxWindowIsUnavailableForCallbacks(addedFrame) ||
                            wxWindowIsUnavailableForCallbacks(addedTabs) ||
                            addedFrame->m_tabs != addedTabs ||
                            addedTabs->GetParent() != current ||
                            current->GetTabFrameFromTabCtrl(addedTabs) !=
                                addedFrame ||
                            !wxAuiMatchesPageProjection(
                                *addedTabs, current,
                                expectedDestinationPages) )
                    {
                        return nullptr;
                    }
                    destinationFound = true;
                }

                return destinationFound ? current : nullptr;
            };

        const wxAuiPageProjection emptyDestination;

        if (tab_frame)
        {
            dest_tabs = tab_frame->m_tabs;

            if (dest_tabs == src_tabs)
                return;

            insert_idx = dragOverride &&
                                 dragOverride->destinationTabsLifetime.get() ==
                                     dest_tabs
                ? dragOverride->destinationIndex
                : source->GetDropIndex(
                      page_info, dest_tabs, mouse_screen_pt);
            if ( insert_idx == wxNOT_FOUND )
                return;
        }
        else
        {
            const wxRect rect = dragOverride &&
                                        dragOverride->createNewPane
                ? wxRect(wxPoint(0, 0), wxSize(1, 1))
                : source->m_mgr.CalculateHintRect(
                      source->m_dummyWnd, mouse_client_pt);
            source = hasOriginalSourceTopology();
            if ( !source )
                return;
            if (rect.IsEmpty())
            {
                // there is no suitable drop location here, exit out
                return;
            }

            // If there is no tabframe at all, create one.
            const wxSize splitSize = source->CalculateNewSplitSize();
            source = hasOriginalSourceTopology();
            if ( !source )
                return;

            wxAuiTabFrame* const new_tabs =
                source->CreateTabFrame(splitSize);
            if ( !new_tabs )
                return;
            weakNewFrame = new_tabs;
            dest_tabs = new_tabs->m_tabs;
            weakCreatedTabs = dest_tabs;
            provisionalFrame = new_tabs;
            provisionalTabs = dest_tabs;
            rollbackProvisionalFrame = true;

            source = hasOriginalSourceTopology();
            if ( !source || weakNewFrame.get() != new_tabs ||
                    weakCreatedTabs.get() != dest_tabs ||
                    dest_tabs->GetParent() != source )
            {
                return;
            }

            wxAuiTabFrame* const sourceFrame =
                weakDragSourceFrame.get();
            if ( sourceFrame != dragSourceFrame || !sourceFrame ||
                    wxWindowIsUnavailableForCallbacks(sourceFrame) ||
                    sourceFrame->m_tabs != src_tabs ||
                    !source->m_mgr.GetPane(sourceFrame).IsOk() )
                return;
            source->m_mgr.SplitPane(sourceFrame, new_tabs, wxBOTTOM,
                                    mouse_client_pt);

            source = hasInternalManagerRevision(
                sourceTabsBefore, dest_tabs, emptyDestination,
                new_tabs, dest_tabs, false);
            if ( !source )
            {
                return;
            }
            insert_idx = 0;
        }

        source = weakSource.get();
        if ( !source || wxWindowIsUnavailableForCallbacks(source) ||
                wxWindowIsUnavailableForCallbacks(dest_tabs) ||
                dest_tabs->GetParent() != source )
        {
            return;
        }

        source = provisionalFrame
            ? hasInternalManagerRevision(
                  sourceTabsBefore, dest_tabs, emptyDestination,
                  provisionalFrame, provisionalTabs, false)
            : hasOriginalSourceTopology();
        if ( !source )
            return;

        const wxWeakRef<wxAuiTabCtrl> weakDestinationTabs(dest_tabs);
        const size_t destinationCountBefore = dest_tabs->GetPageCount();
        if ( insert_idx < 0 ||
                static_cast<size_t>(insert_idx) > destinationCountBefore )
        {
            return;
        }

        wxAuiPageProjection destinationBefore;
        if ( !wxAuiCapturePageProjection(*dest_tabs, source,
                                         destinationBefore) )
        {
            return;
        }

        source = hasInternalManagerRevision(
            sourceTabsBefore, dest_tabs, destinationBefore,
            provisionalFrame, provisionalTabs, false);
        if ( !source )
            return;

        wxAuiPageProjection destinationAfter = destinationBefore;
        try
        {
            destinationAfter.insert(
                destinationAfter.begin() + insert_idx,
                sourceCanonicalBefore[
                    static_cast<size_t>(sourceMainIndex)]);
        }
        catch ( const std::exception& )
        {
            return;
        }

        // Move between the two visual projections atomically with respect to
        // custom art: allocate and publish the destination before erasing the
        // exact source, with no callback in between.
        page_info.active = false;
        try
        {
            auto destinationButtons = MakePageButtons(dest_tabs->m_flags);
            const auto inserted = dest_tabs->m_pages.insert(
                dest_tabs->m_pages.begin() + insert_idx, page_info);
            wxAuiBumpTabContainerRevision(dest_tabs);
            inserted->buttons = std::move(destinationButtons);
            wxAuiBumpTabContainerRevision(dest_tabs);
        }
        catch ( const std::exception& )
        {
            return;
        }

        source = hasInternalManagerRevision(
            sourceTabsBefore, dest_tabs, destinationAfter,
            provisionalFrame, provisionalTabs, false);
        if ( !source || weakDestinationTabs.get() != dest_tabs ||
                wxWindowIsUnavailableForCallbacks(dest_tabs) )
        {
            if ( weakDestinationTabs.get() == dest_tabs &&
                    static_cast<size_t>(insert_idx) <
                        dest_tabs->GetPageCount() &&
                    dest_tabs->GetWindowFromIdx(
                        static_cast<size_t>(insert_idx)) == src_page )
            {
                dest_tabs->m_pages.RemoveAt(
                    static_cast<size_t>(insert_idx));
                wxAuiBumpTabContainerRevision(dest_tabs);
            }
            return;
        }
        src_tabs->m_pages.RemoveAt(static_cast<size_t>(src_idx));
        wxAuiBumpTabContainerRevision(src_tabs);
        rollbackProvisionalFrame = false;
        provisionalFrameGuard.Dismiss();

        const auto hasMovedSourceTopology = [&]() -> wxAuiNotebook*
        {
            wxAuiNotebook* const current = hasInternalManagerRevision(
                sourceTabsAfter, dest_tabs, destinationAfter,
                provisionalFrame, provisionalTabs, false);
            return current && weakSourceTabs.get() == src_tabs &&
                           weakDestinationTabs.get() == dest_tabs &&
                           src_tabs->GetPageCount() + 1 == sourceTabCount &&
                           src_tabs->GetIdxFromWindow(src_page) == wxNOT_FOUND &&
                           dest_tabs->GetPageCount() ==
                               destinationCountBefore + 1 &&
                           static_cast<size_t>(insert_idx) <
                               dest_tabs->GetPageCount() &&
                           dest_tabs->GetWindowFromIdx(
                               static_cast<size_t>(insert_idx)) == src_page
                        ? current : nullptr;
        };

        const auto hasFinalMovedTopology = [&]() -> wxAuiNotebook*
        {
            return hasInternalManagerRevision(
                sourceTabsAfter, dest_tabs, destinationAfter,
                provisionalFrame, provisionalTabs,
                sourceTabsAfter.empty());
        };

        if ( src_tabs->m_art )
        {
            wxAuiSetTabArtSizingInfo(
                src_tabs->m_art, src_tabs->m_rect.GetSize(),
                src_tabs->m_pages.GetCount(), src_tabs);
        }
        source = hasMovedSourceTopology();
        if ( !source )
            return;
        if ( dest_tabs->m_art )
        {
            wxAuiSetTabArtSizingInfo(
                dest_tabs->m_art, dest_tabs->m_rect.GetSize(),
                dest_tabs->m_pages.GetCount(), dest_tabs);
        }
        source = hasMovedSourceTopology();
        if ( !source )
            return;

        if (src_tabs->GetPageCount() > 0)
        {
            src_tabs->SetActivePage((size_t)0);
            src_tabs->DoUpdateActive();
            source = hasMovedSourceTopology();
            if ( !source )
                return;
        }

        if (src_tabs->GetPageCount() == 0)
        {
            source->RemoveEmptyTabFrames();
            source = hasFinalMovedTopology();
            if ( !source )
                return;
        }

        source->DoSizing();
        source = hasFinalMovedTopology();
        if ( !source )
            return;
        dest_tabs->DoUpdateActive();
        source = hasFinalMovedTopology();
        if ( !source )
            return;

        // force the set selection function reset the selection
        source->m_curPage = wxNOT_FOUND;

        // set the active page to the one we just split off
        source->SetSelectionToPage(page_info);

        // Selection dispatch can destroy/rebuild the book. Revalidate the
        // exact committed projection before the final layout continuation.
        source = hasFinalMovedTopology();
        if ( !source )
            return;
        source->UpdateHintWindowSize();

        source = hasFinalMovedTopology();
        if ( !source )
            return;
    }

    // notify owner that the tab has been dragged
    source = weakSource.get();
    if ( !source || wxWindowIsUnavailableForCallbacks(source) ||
            weakSourcePage.get() != src_page ||
            wxWindowIsUnavailableForCallbacks(src_page) )
    {
        return;
    }
    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_DRAG_DONE, source->m_windowId);
    e.SetSelection(source->m_tabs.GetIdxFromWindow(src_page));
    e.SetOldSelection(e.GetSelection());
    e.SetEventObject(source);
    source->ProcessWindowEvent(e);
}



void wxAuiNotebook::OnTabCancelDrag(wxAuiTabCtrl* ctrl, int tabIdx)
{
    m_mgr.HideHint();

    ctrl->SetCursor(wxCursor(wxCURSOR_ARROW));

    wxWindow* const wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_CANCEL_DRAG, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

wxAuiTabCtrl* wxAuiNotebook::GetTabCtrlFromPoint(const wxPoint& pt)
{
    // if we've just removed the last tab from the source
    // tab set, the remove the tab control completely
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tab_rect.Contains(pt))
            return tabframe->m_tabs;
    }

    return nullptr;
}

wxWindow* wxAuiNotebook::GetTabFrameFromTabCtrl(wxWindow* tab_ctrl)
{
    // if we've just removed the last tab from the source
    // tab set, the remove the tab control completely
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tabs == tab_ctrl)
        {
            return tabframe;
        }
    }

    return nullptr;
}

void wxAuiNotebook::RemoveEmptyTabFrames()
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    struct FrameSnapshot
    {
        wxAuiTabFrame* frame;
        wxWeakRef<wxAuiTabFrame> frameLifetime;
        wxAuiTabCtrl* tabs;
        wxWeakRef<wxAuiTabCtrl> tabsLifetime;
    };

    // Snapshot identities before the first DetachPane()/destructor boundary.
    std::vector<FrameSnapshot> frames;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const frame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !frame || wxWindowIsUnavailableForCallbacks(frame) )
            return;
        wxAuiTabCtrl* const tabs = frame->m_tabsLifetime.get();
        if ( !tabs || frame->m_tabs != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) )
            return;
        frames.push_back({frame, wxWeakRef<wxAuiTabFrame>(frame),
                          tabs, wxWeakRef<wxAuiTabCtrl>(tabs)});
    }

    for ( const FrameSnapshot& expected : frames )
    {
        wxAuiNotebook* book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) )
            return;

        wxAuiTabFrame* const frame = expected.frameLifetime.get();
        wxAuiTabCtrl* const tabs = expected.tabsLifetime.get();
        if ( frame != expected.frame || !frame ||
                wxWindowIsUnavailableForCallbacks(frame) ||
                tabs != expected.tabs || !tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                frame->m_tabs != tabs || tabs->GetParent() != book ||
                book->GetTabFrameFromTabCtrl(tabs) != frame )
        {
            return;
        }
        if ( tabs->GetPageCount() != 0 )
            continue;
        if ( wxAuiNotebookPreserveTabCtrl::IsActiveFor(book, tabs) )
            continue;

        book->m_mgr.DetachPane(frame);
        book = weakThis.get();
        if ( !book || wxWindowIsUnavailableForCallbacks(book) )
            return;

        // Delete only the exact frame we detached. A callback which reattached
        // or repurposed it owns the new topology and wins.
        if ( expected.frameLifetime.get() != frame ||
                wxWindowIsUnavailableForCallbacks(frame) ||
                expected.tabsLifetime.get() != tabs ||
                wxWindowIsUnavailableForCallbacks(tabs) ||
                frame->m_tabs != tabs || tabs->GetPageCount() != 0 ||
                book->GetTabFrameFromTabCtrl(tabs) != nullptr )
        {
            return;
        }
        delete frame;
        if ( !weakThis.get() ||
                wxWindowIsUnavailableForCallbacks(weakThis.get()) )
        {
            return;
        }
    }

    wxAuiNotebook* book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) )
        return;

    wxWindow* firstGood = nullptr;
    wxWeakRef<wxWindow> weakFirstGood;
    bool centerFound = false;
    for ( const auto& pane : book->m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        if ( !pane.window ||
                wxWindowIsUnavailableForCallbacks(pane.window) )
        {
            return;
        }
        if ( pane.dock_direction == wxAUI_DOCK_CENTRE )
            centerFound = true;
        if ( !firstGood )
        {
            firstGood = pane.window;
            weakFirstGood = firstGood;
        }
    }

    if ( !centerFound && firstGood && weakFirstGood.get() == firstGood &&
            !wxWindowIsUnavailableForCallbacks(firstGood) )
    {
        book->m_mgr.GetPane(firstGood).Centre();
    }

    book = weakThis.get();
    if ( book && !wxWindowIsUnavailableForCallbacks(book) &&
            !book->m_isBeingDeleted )
    {
        book->m_mgr.Update();
    }
}

void wxAuiNotebook::OnChildFocusNotebook(wxChildFocusEvent& evt)
{
    evt.Skip();

    // if we're dragging a tab, don't change the current selection.
    // This code prevents a bug that used to happen when the hint window
    // was hidden.  In the bug, the focus would return to the notebook
    // child, which would then enter this handler and call
    // SetSelection, which is not desired turn tab dragging.

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        wxAuiTabFrame* tabframe = (wxAuiTabFrame*)pane.window;
        if (tabframe->m_tabs->IsDragging())
            return;
    }


    // find the page containing the focused child
    wxWindow* win = evt.GetWindow();
    while ( win )
    {
        // pages have the notebook as the parent, so stop when we reach one
        // (and also stop in the impossible case of no parent at all)
        wxWindow* const parent = win->GetParent();
        if ( !parent || parent == this )
            break;

        win = parent;
    }

    // change the tab selection to this page
    int idx = m_tabs.GetIdxFromWindow(win);
    if (idx != -1 && idx != m_curPage)
    {
        SetSelection(idx);
    }
}

void wxAuiNotebook::OnNavigationKeyNotebook(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        // FIXME: the problem with this is that if we have a split notebook,
        // we selection may go all over the place.
        AdvanceSelection(event.GetDirection());
    }
    else {
        // we get this event in 3 cases
        //
        // a) one of our pages might have generated it because the user TABbed
        // out from it in which case we should propagate the event upwards and
        // our parent will take care of setting the focus to prev/next sibling
        //
        // or
        //
        // b) the parent panel wants to give the focus to us so that we
        // forward it to our selected page. We can't deal with this in
        // OnSetFocus() because we don't know which direction the focus came
        // from in this case and so can't choose between setting the focus to
        // first or last panel child
        //
        // or
        //
        // c) we ourselves (see MSWTranslateMessage) generated the event
        //
        wxWindow * const parent = GetParent();

        // the wxObject* casts are required to avoid MinGW GCC 2.95.3 ICE
        const bool isFromParent = event.GetEventObject() == (wxObject*) parent;
        const bool isFromSelf = event.GetEventObject() == (wxObject*) this;

        if ( isFromParent || isFromSelf )
        {
            // no, it doesn't come from child, case (b) or (c): forward to a
            // page but only if direction is backwards (TAB) or from ourselves,
            if ( GetSelection() != wxNOT_FOUND &&
                    (!event.GetDirection() || isFromSelf) )
            {
                // so that the page knows that the event comes from it's parent
                // and is being propagated downwards
                event.SetEventObject(this);

                wxWindow *page = GetPage(GetSelection());
                if ( !page->ProcessWindowEvent(event) )
                {
                    page->SetFocus();
                }
                //else: page manages focus inside it itself
            }
            else // otherwise set the focus to the notebook itself
            {
                SetFocus();
            }
        }
        else
        {
            // it comes from our child, case (a), pass to the parent, but only
            // if the direction is forwards. Otherwise set the focus to the
            // notebook itself. The notebook is always the 'first' control of a
            // page.
            if ( !event.GetDirection() )
            {
                SetFocus();
            }
            else if ( parent )
            {
                event.SetCurrentFocus(this);
                parent->ProcessWindowEvent(event);
            }
        }
    }
}

void wxAuiNotebook::OnTabButton(wxAuiTabCtrl* tabs, int tabIdx, int button_id)
{
    if (button_id == wxAUI_BUTTON_CLOSE)
    {
        int selection = tabIdx;
        if (selection == -1)
        {
            // if the close button is to the right, use the active
            // page selection to determine which page to close
            selection = tabs->GetActivePage();
        }

        if (selection != -1)
        {
            wxWindow* close_wnd = tabs->GetWindowFromIdx(selection);

            // ask owner if it's ok to close the tab
            wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_PAGE_CLOSE, m_windowId);
            e.SetSelection(m_tabs.GetIdxFromWindow(close_wnd));
            const int idx = m_tabs.GetIdxFromWindow(close_wnd);
            e.SetSelection(idx);
            e.SetOldSelection(selection);
            e.SetEventObject(this);
            ProcessWindowEvent(e);
            if (!e.IsAllowed())
                return;


#if wxUSE_MDI
            if (wxDynamicCast(close_wnd, wxAuiMDIChildFrame))
            {
                close_wnd->Close();
            }
            else
#endif
            {
                int main_idx = m_tabs.GetIdxFromWindow(close_wnd);
                wxCHECK_RET( main_idx != wxNOT_FOUND, wxT("no page to delete?") );

                DeletePage(main_idx);
            }

            // notify owner that the tab has been closed
            wxAuiNotebookEvent e2(wxEVT_AUINOTEBOOK_PAGE_CLOSED, m_windowId);
            e2.SetSelection(idx);
            e2.SetEventObject(this);
            ProcessWindowEvent(e2);
        }
    }
    else if (button_id == wxAUI_BUTTON_PIN)
    {
        // For now we don't send any event, this can be always added later if
        // necessary.
        wxWindow* const wnd = tabs->GetWindowFromIdx(tabIdx);

        const auto idx = m_tabs.GetIdxFromWindow(wnd);

        wxAuiTabKind newKind = wxAuiTabKind::Locked;
        switch ( GetPageKind(idx) )
        {
            case wxAuiTabKind::Normal:
                newKind = wxAuiTabKind::Pinned;
                break;

            case wxAuiTabKind::Pinned:
                newKind = wxAuiTabKind::Normal;
                break;

            case wxAuiTabKind::Locked:
                // Locked tabs can't be pinned or unpinned.
                break;
        }

        wxCHECK_RET(newKind != wxAuiTabKind::Locked,
                    "locked pages shouldn't have pin button");

        SetPageKind(idx, newKind);
    }
}


void wxAuiNotebook::OnTabMiddleDown(wxAuiTabCtrl* ctrl, int tabIdx)
{
    wxWindow* const wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabMiddleUp(wxAuiTabCtrl* ctrl, int tabIdx)
{
    // if the wxAUI_NB_MIDDLE_CLICK_CLOSE is specified, middle
    // click should act like a tab close action.  However, first
    // give the owner an opportunity to handle the middle up event
    // for custom action

    wxWindow* const wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    if (ProcessWindowEvent(e))
        return;
    if (!e.IsAllowed())
        return;

    // check if we are supposed to close on middle-up
    if ((m_flags & wxAUI_NB_MIDDLE_CLICK_CLOSE) == 0)
        return;

    // simulate the user pressing the close button on the tab
    OnTabButton(ctrl, tabIdx, wxAUI_BUTTON_CLOSE);
}

void wxAuiNotebook::OnTabRightDown(wxAuiTabCtrl* ctrl, int tabIdx)
{
    // Generate corresponding wxEVT_AUI event.
    wxWindow* wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

void wxAuiNotebook::OnTabRightUp(wxAuiTabCtrl* ctrl, int tabIdx)
{
    // Generate corresponding wxEVT_AUI event.
    wxWindow* wnd = ctrl->GetWindowFromIdx(tabIdx);

    wxAuiNotebookEvent e(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, m_windowId);
    e.SetSelection(m_tabs.GetIdxFromWindow(wnd));
    e.SetEventObject(this);
    ProcessWindowEvent(e);
}

// Sets the normal font
void wxAuiNotebook::SetNormalFont(const wxFont& font)
{
    wxAuiBumpNotebookFontEpoch(this);
    wxAuiBumpNotebookNormalFontEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_normalFont = font;
    m_tabs.SetNormalFont(font);
}

// Sets the selected tab font
void wxAuiNotebook::SetSelectedFont(const wxFont& font)
{
    wxAuiBumpNotebookFontEpoch(this);
    wxAuiBumpNotebookSelectedFontEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_selectedFont = font;
    m_tabs.SetSelectedFont(font);
}

// Sets the measuring font
void wxAuiNotebook::SetMeasuringFont(const wxFont& font)
{
    wxAuiBumpNotebookFontEpoch(this);
    wxAuiBumpNotebookMeasuringFontEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);
    m_tabs.SetMeasuringFont(font);
}

// Sets the tab font
bool wxAuiNotebook::SetFont(const wxFont& font)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxAuiNotebookFontEpoch fontEpoch =
        wxAuiBumpNotebookFontEpoch(this);
    wxAuiBumpNotebookNormalFontEpoch(this);
    wxAuiBumpNotebookSelectedFontEpoch(this);
    wxAuiBumpNotebookMeasuringFontEpoch(this);
    wxAuiBumpNotebookHeightInputEpoch(this);

    wxControl::SetFont(font);

    wxFont normalFont(font);
    wxFont selectedFont(normalFont);
    selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    const auto hasCurrentFontTransaction =
        [&](wxAuiTabArt* const expectedArt = nullptr) -> wxAuiNotebook*
        {
            wxAuiNotebook* const book = weakThis.get();
            return book && !wxWindowIsUnavailableForCallbacks(book) &&
                           wxAuiGetNotebookFontEpoch(book) == fontEpoch &&
                           (!expectedArt ||
                            book->GetArtProvider() == expectedArt)
                        ? book
                        : nullptr;
        };

    wxAuiNotebook* book = hasCurrentFontTransaction();
    if ( !book )
        return false;

    wxAuiTabArt* const art = book->GetArtProvider();
    book->m_normalFont = normalFont;
    wxAuiInvokeTabArt(
        art, [&normalFont](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetNormalFont(normalFont);
        });
    book = hasCurrentFontTransaction(art);
    if ( !book )
        return false;

    book->m_selectedFont = selectedFont;
    wxAuiInvokeTabArt(
        art, [&selectedFont](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetSelectedFont(selectedFont);
        });
    book = hasCurrentFontTransaction(art);
    if ( !book )
        return false;

    wxAuiInvokeTabArt(
        art, [&selectedFont](wxAuiTabArt* const leasedArt)
        {
            leasedArt->SetMeasuringFont(selectedFont);
        });
    if ( !hasCurrentFontTransaction(art) )
        return false;

    return true;
}

// Gets the tab control height
int wxAuiNotebook::GetTabCtrlHeight() const
{
    return m_tabCtrlHeight;
}

// Gets the height of the notebook for a given page height
int wxAuiNotebook::GetHeightForPageHeight(int pageHeight)
{
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const int previousTabCtrlHeight = m_tabCtrlHeight;
    UpdateTabCtrlHeight();

    const wxAuiNotebook* const book = weakThis.get();
    const int tabCtrlHeight =
        book && !wxWindowIsUnavailableForCallbacks(book)
            ? book->m_tabCtrlHeight
            : previousTabCtrlHeight;
    return tabCtrlHeight + pageHeight + 2;
}

// Shows the window menu
bool wxAuiNotebook::ShowWindowMenu()
{
    wxAuiTabCtrl* tabCtrl = GetActiveTabCtrl();
    if ( !tabCtrl || wxWindowIsUnavailableForCallbacks(tabCtrl) )
        return false;

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxAuiTabCtrl> weakTabCtrl(tabCtrl);
    wxAuiTabArt* const art = tabCtrl->GetArtProvider();
    if ( !art )
        return false;
    const wxAuiTabContainerSnapshot revision(
        tabCtrl, tabCtrl->m_art, tabCtrl->m_rect, tabCtrl->m_tabOffset,
        tabCtrl->m_flags, tabCtrl->m_tabRowHeight, tabCtrl->m_pages,
        tabCtrl->m_buttons, tabCtrl);
    wxAuiPageWindowProjection pageProjection;
    if ( !pageProjection.Capture(tabCtrl->m_pages) )
        return false;
    const wxAuiNotebookPageArray expectedPages = tabCtrl->m_pages;
    wxAuiNotebookPageArray menuPages = expectedPages;
    const int activePage = tabCtrl->GetActivePage();
    const int idx = wxAuiInvokeTabArt(
        art, [tabCtrl, &menuPages, activePage](
            wxAuiTabArt* const leasedArt)
        {
            return leasedArt->ShowDropDown(
                tabCtrl, menuPages, activePage);
        });

    if ( !revision.IsAlive() || weakThis.get() != this ||
            wxWindowIsUnavailableForCallbacks(this) ||
            weakTabCtrl.get() != tabCtrl ||
            wxWindowIsUnavailableForCallbacks(tabCtrl) ||
            tabCtrl->GetParent() != this ||
            !pageProjection.MatchesAll(tabCtrl->m_pages) ||
            GetActiveTabCtrl() != tabCtrl ||
            !revision.Matches(
                tabCtrl->m_art, tabCtrl->m_rect, tabCtrl->m_tabOffset,
                tabCtrl->m_flags, tabCtrl->m_tabRowHeight,
                tabCtrl->m_pages, tabCtrl->m_buttons) )
    {
        return false;
    }

    if ( idx >= 0 &&
            static_cast<size_t>(idx) < expectedPages.GetCount() &&
            static_cast<size_t>(idx) < menuPages.GetCount() &&
            pageProjection.Matches(
                tabCtrl->m_pages, static_cast<size_t>(idx)) &&
            wxAuiHasSamePagePayload(
                expectedPages[static_cast<size_t>(idx)],
                menuPages[static_cast<size_t>(idx)]) &&
            tabCtrl->m_pages[static_cast<size_t>(idx)].window ==
                expectedPages[static_cast<size_t>(idx)].window )
    {
        OnTabClicked(tabCtrl, idx);

        return true;
    }
    else
        return false;
}

void wxAuiNotebook::DoThaw()
{
    DoSizing();

    wxBookCtrlBase::DoThaw();
}

void wxAuiNotebook::SetPageSize (const wxSize& WXUNUSED(size))
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
}

int wxAuiNotebook::HitTest (const wxPoint &pt, long *flags) const
{
    wxWindow *w = nullptr;
    long position = wxBK_HITTEST_NOWHERE;
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabFrame* tabframe = (wxAuiTabFrame*) pane.window;
        if (tabframe->m_tab_rect.Contains(pt))
        {
            wxPoint tabpos = tabframe->m_tabs->ScreenToClient(ClientToScreen(pt));
            w = tabframe->m_tabs->TabHitTest(tabpos).window;
            if (w)
                position = wxBK_HITTEST_ONITEM;
            break;
        }
        else if (tabframe->m_rect.Contains(pt))
        {
            w = tabframe->m_tabs->GetWindowFromIdx(tabframe->m_tabs->GetActivePage());
            if (w)
                position = wxBK_HITTEST_ONPAGE;
            break;
        }
    }

    if (flags)
        *flags = position;
    return w ? GetPageIndex(w) : wxNOT_FOUND;
}

int wxAuiNotebook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
    return -1;
}

bool wxAuiNotebook::SetPageImage(size_t n, int imageId)
{
    return SetPageBitmap(n, GetBitmapBundle(imageId));
}

int wxAuiNotebook::ChangeSelection(size_t n)
{
    return DoModifySelection(n, false);
}

bool wxAuiNotebook::AddPage(wxWindow *page, const wxString &text, bool select,
                            int imageId)
{
    return AddPage(page, text, select, GetBitmapBundle(imageId));
}

bool wxAuiNotebook::DeleteAllPages()
{
    // The base transaction operates through our polymorphic GetPage*() and
    // DoRemovePage() slots, so it supports the external m_tabs model while
    // also enforcing bounded reentrance and honest partial-failure reporting.
    return wxBookCtrlBase::DeleteAllPages();
}

bool wxAuiNotebook::InsertPage(size_t index, wxWindow *page,
                               const wxString &text, bool select,
                               int imageId)
{
    return InsertPage(index, page, text, select, GetBitmapBundle(imageId));
}

namespace
{

// Helper class to calculate the best size of a wxAuiNotebook
class wxAuiLayoutObject
{
public:
    enum DockDir
    {
        DockDir_Center,
        DockDir_Left,
        DockDir_Right,
        DockDir_Vertical,   // Merge elements from here vertically
        DockDir_Top,
        DockDir_Bottom,
        DockDir_None
    };

    wxAuiLayoutObject(const wxSize &size, const wxAuiPaneInfo &pInfo)
        : m_size(size)
    {
        m_pInfo = &pInfo;
        /*
            To speed up the sorting of the panes, the direction is mapped to a
            useful increasing value. This avoids complicated comparison of the
            enum values during the sort. The size calculation is done from the
            inner to the outermost direction. Therefore CENTER < LEFT/RIGHT <
            TOP/BOTTOM (It doesn't matter it LEFT or RIGHT is done first, as
            both extend the best size horizontally; the same applies for
            TOP/BOTTOM in vertical direction)
         */
        switch ( pInfo.dock_direction )
        {
            case wxAUI_DOCK_CENTER: m_dir = DockDir_Center; break;
            case wxAUI_DOCK_LEFT:   m_dir = DockDir_Left; break;
            case wxAUI_DOCK_RIGHT:  m_dir = DockDir_Right; break;
            case wxAUI_DOCK_TOP:    m_dir = DockDir_Top; break;
            case wxAUI_DOCK_BOTTOM: m_dir = DockDir_Bottom; break;
            default:                m_dir = DockDir_None;
        }
    }
    void MergeLayout(const wxAuiLayoutObject &lo2)
    {
        if ( this == &lo2 )
            return;

        bool mergeHorizontal;
        if ( m_pInfo->dock_layer != lo2.m_pInfo->dock_layer || m_dir != lo2.m_dir )
            mergeHorizontal = lo2.m_dir < DockDir_Vertical;
        else if ( m_pInfo->dock_row != lo2.m_pInfo->dock_row )
            mergeHorizontal = true;
        else
            mergeHorizontal = lo2.m_dir >= DockDir_Vertical;

        if ( mergeHorizontal )
        {
            m_size.x += lo2.m_size.x;
            if ( lo2.m_size.y > m_size.y )
                m_size.y = lo2.m_size.y;
        }
        else
        {
            if ( lo2.m_size.x > m_size.x )
                m_size.x = lo2.m_size.x;
            m_size.y += lo2.m_size.y;
        }
    }

    wxSize m_size;
    const wxAuiPaneInfo *m_pInfo;
    DockDir m_dir;

    /*
        As the calculation is done from the inner to the outermost pane, the
        panes are sorted in the following order: layer, direction, row,
        position.
     */
    bool operator<(const wxAuiLayoutObject& lo2) const
    {
        int diff = m_pInfo->dock_layer - lo2.m_pInfo->dock_layer;
        if ( diff )
            return diff < 0;
        diff = m_dir - lo2.m_dir;
        if ( diff )
            return diff < 0;
        diff = m_pInfo->dock_row - lo2.m_pInfo->dock_row;
        if ( diff )
            return diff < 0;
        return m_pInfo->dock_pos < lo2.m_pInfo->dock_pos;
    }
};

} // anonymous namespace

wxSize wxAuiNotebook::DoGetBestSize() const
{
    /*
        The best size of the wxAuiNotebook is a combination of all panes inside
        the object. To be able to efficiently  calculate the dimensions (i.e.
        without iterating over the panes multiple times) the panes need to be
        processed in a specific order. Therefore we need to collect them in the
        following variable which is sorted later on.
     */
    wxVector<wxAuiLayoutObject> layouts;
    const int tabHeight = GetTabCtrlHeight();
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) || pane.IsFloating() )
            continue;

        const wxAuiTabFrame* tabframe = (wxAuiTabFrame*) pane.window;
        const wxAuiNotebookPageArray &pages = tabframe->m_tabs->GetPages();

        wxSize bestPageSize;
        for ( size_t pIdx = 0; pIdx < pages.GetCount(); pIdx++ )
            bestPageSize.IncTo(pages[pIdx].window->GetBestSize());

        bestPageSize.y += tabHeight;
        // Store the current pane with its largest window dimensions
        layouts.push_back(wxAuiLayoutObject(bestPageSize, pane));
    }

    if ( layouts.empty() )
        return wxSize(0, 0);

    wxVectorSort(layouts);

    /*
        The sizes of the panes are merged here. As the center pane is always at
        position 0 all sizes are merged there. As panes can be stacked using
        the dock_pos property, different positions are merged at the first
        (i.e. dock_pos = 0) element before being merged with the center pane.
     */
    size_t pos = 0;
    for ( size_t n = 1; n < layouts.size(); n++ )
    {
        if ( layouts[n].m_pInfo->dock_layer == layouts[pos].m_pInfo->dock_layer &&
             layouts[n].m_dir == layouts[pos].m_dir &&
             layouts[n].m_pInfo->dock_row == layouts[pos].m_pInfo->dock_row )
        {
            layouts[pos].MergeLayout(layouts[n]);
        }
        else
        {
            layouts[0].MergeLayout(layouts[pos]);
            pos = n;
        }
    }
    layouts[0].MergeLayout(layouts[pos]);

    return layouts[0].m_size;
}

int wxAuiNotebook::DoModifySelection(size_t n, bool events)
{
    const int selectionBefore = m_curPage;
    const size_t pageCount = m_tabs.GetPageCount();
    if ( n >= pageCount )
        return selectionBefore;

    wxWindow* const wnd = m_tabs.GetWindowFromIdx(n);
    if (!wnd)
        return selectionBefore;

    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(wnd);
    const wxAuiNotebookSelectionTransaction selectionTransaction(this);
    if ( !selectionTransaction.IsAccepted() )
        return selectionBefore;

    const auto hasCurrentPageRevision = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const book = weakThis.get();
        return book && !wxWindowIsUnavailableForCallbacks(book) &&
                       weakPage.get() == wnd &&
                       !wxWindowIsUnavailableForCallbacks(wnd) &&
                       wnd->GetParent() == book &&
                       book->m_tabs.GetPageCount() == pageCount &&
                       n < pageCount &&
                       book->m_tabs.GetWindowFromIdx(n) == wnd
                    ? book
                    : nullptr;
    };

    wxAuiNotebook* book = hasCurrentPageRevision();
    if ( !book )
        return selectionBefore;

    // don't change the page unless necessary;
    // however, clicking again on a tab should give it the focus.
    if ( static_cast<int>(n) == book->m_curPage )
    {
        const auto tabInfo = book->FindTab(wnd);
        if ( tabInfo )
        {
            wxAuiTabCtrl* const ctrl = tabInfo.tabCtrl;
            if ( ctrl && FindFocus() != ctrl )
                ctrl->SetFocus();
        }
        // SetFocus() may synchronously destroy either object. There is no
        // continuation, so return the pre-callback value without touching
        // either raw identity again.
        return selectionBefore;
    }

    wxAuiNotebookEvent evt(wxEVT_AUINOTEBOOK_PAGE_CHANGING,
                           book->m_windowId);
    bool vetoed = false;
    if(events)
    {
        evt.SetSelection(n);
        evt.SetOldSelection(selectionBefore);
        evt.SetEventObject(book);
        book->ProcessWindowEvent(evt);
        vetoed = !evt.IsAllowed();

        book = hasCurrentPageRevision();
        if ( !book )
            return selectionBefore;
        if ( !selectionTransaction.IsLatest() )
            return book->m_curPage;
    }

    if (vetoed)
        return book->m_curPage;

    const int oldSelection = book->m_curPage;
    book->m_curPage = static_cast<int>(n);

    const auto tabInfo = book->FindTab(wnd);
    if ( !tabInfo )
        return book->m_curPage;

    wxAuiTabCtrl* const ctrl = tabInfo.tabCtrl;
    const wxWeakRef<wxAuiTabCtrl> weakCtrl(ctrl);
    const int tabIndex = tabInfo.tabIdx;
    const size_t tabCount = ctrl->GetPageCount();
    const size_t paneCount = book->m_mgr.GetAllPanes().GetCount();
    const wxAuiNotebookFontEpoch fontEpoch =
        wxAuiGetNotebookFontEpoch(book);

    const auto hasCommittedSelection = [&]() -> wxAuiNotebook*
    {
        wxAuiNotebook* const current = hasCurrentPageRevision();
        return current && selectionTransaction.IsLatest() &&
                       wxAuiGetNotebookFontEpoch(current) == fontEpoch &&
                       current->m_curPage == static_cast<int>(n) &&
                       weakCtrl.get() == ctrl &&
                       !wxWindowIsUnavailableForCallbacks(ctrl) &&
                       ctrl->GetParent() == current && tabIndex >= 0 &&
                       static_cast<size_t>(tabIndex) < tabCount &&
                       ctrl->GetPageCount() == tabCount &&
                       ctrl->GetWindowFromIdx(
                           static_cast<size_t>(tabIndex)) == wnd &&
                       current->m_mgr.GetAllPanes().GetCount() == paneCount
                    ? current
                    : nullptr;
    };

    ctrl->SetActivePage(tabIndex);
    book = hasCommittedSelection();
    if ( !book )
        return oldSelection;

    book->DoSizing();
    book = hasCommittedSelection();
    if ( !book )
        return oldSelection;

    ctrl->DoShowTab(tabIndex);
    book = hasCommittedSelection();
    if ( !book )
        return oldSelection;

    // Snapshot weak tab identities because a custom art provider invoked by
    // SetSelectedFont() may rebuild the manager pane array.
    std::vector<wxWeakRef<wxAuiTabCtrl>> tabCtrls;
    for ( const auto& pane : book->m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;
        auto* const tabFrame = static_cast<wxAuiTabFrame*>(pane.window);
        if ( !tabFrame || wxWindowIsUnavailableForCallbacks(tabFrame) )
            return oldSelection;
        wxAuiTabCtrl* const tabCtrl = tabFrame->m_tabs;
        if ( !tabCtrl || wxWindowIsUnavailableForCallbacks(tabCtrl) ||
                tabCtrl->GetParent() != book )
        {
            return oldSelection;
        }
        tabCtrls.emplace_back(tabCtrl);
    }
    const wxFont normalFont = book->m_normalFont;
    const wxFont selectedFont = book->m_selectedFont;

    for ( const wxWeakRef<wxAuiTabCtrl>& weakTabCtrl : tabCtrls )
    {
        wxAuiTabCtrl* const tabCtrl = weakTabCtrl.get();
        book = hasCommittedSelection();
        if ( !book || !tabCtrl ||
                wxWindowIsUnavailableForCallbacks(tabCtrl) ||
                tabCtrl->GetParent() != book ||
                !book->GetTabFrameFromTabCtrl(tabCtrl) )
        {
            return oldSelection;
        }

        tabCtrl->SetSelectedFont(
            tabCtrl == ctrl ? selectedFont : normalFont);
        book = hasCommittedSelection();
        if ( !book || weakTabCtrl.get() != tabCtrl ||
                wxWindowIsUnavailableForCallbacks(tabCtrl) )
            return oldSelection;

        tabCtrl->Refresh();
        book = hasCommittedSelection();
        if ( !book || weakTabCtrl.get() != tabCtrl ||
                wxWindowIsUnavailableForCallbacks(tabCtrl) )
            return oldSelection;
    }

    // Set the focus to the page if we're not currently focused on the tab.
    // This is Firefox-like behaviour.
    const bool isShownOnScreen = wnd->IsShownOnScreen();
    book = hasCommittedSelection();
    if ( !book )
        return oldSelection;
    if (isShownOnScreen && FindFocus() != ctrl)
    {
        wnd->SetFocus();
        book = hasCommittedSelection();
        if ( !book )
            return oldSelection;
    }

    // program allows the page change
    if(events)
    {
        evt.SetEventType(wxEVT_AUINOTEBOOK_PAGE_CHANGED);
        evt.SetEventObject(book);
        (void)book->ProcessWindowEvent(evt);

        // The event is the final callback, but still resolve the weak state so
        // no stale selection is observed by a future continuation added here.
        book = hasCommittedSelection();
        wxUnusedVar(book);
    }

    return oldSelection;
}

void wxAuiTabCtrl::SetHoverTab(wxWindow* wnd)
{
    bool hoverChanged = false;

    for ( auto& page : m_pages )
    {
        bool oldHover = page.hover;
        page.hover = (page.window == wnd);
        if ( oldHover != page.hover )
            hoverChanged = true;
    }

    if ( hoverChanged )
    {
        wxAuiBumpTabContainerRevision(this);
        Refresh();
        Update();
    }
}

// ----------------------------------------------------------------------------
// Layout serialization
// ----------------------------------------------------------------------------

void
wxAuiNotebook::SaveLayout(const wxString& name,
                          wxAuiBookSerializer& serializer) const
{
    serializer.BeforeSaveNotebook(name);

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( IsDummyPane(pane) )
            continue;

        wxAuiTabLayoutInfo tab;
        m_mgr.CopyDockLayoutFrom(tab, pane);

        const wxAuiTabCtrl* const
            tabCtrl = static_cast<wxAuiTabFrame*>(pane.window)->m_tabs;

        auto* const activePage =
            tabCtrl->GetWindowFromIdx(tabCtrl->GetActivePage());
        tab.active = m_tabs.GetIdxFromWindow(activePage);

        // As an optimization, don't bother with saving the pages order for the
        // main control if it hasn't been changed from the default.
        bool mustSavePages = false;
        if ( tab.dock_direction != wxAUI_DOCK_CENTER )
        {
            // The non-main tab controls can't possibly have all the pages, so
            // we always have to save the indices of the ones they contain.
            mustSavePages = true;
        }
        else if ( tabCtrl->GetPageCount() != GetPageCount() )
        {
            // If the main control doesn't have all the pages, we need to save
            // them for it as well.
            mustSavePages = true;
        }

        std::vector<int> pages;
        int n = 0;
        for ( const auto& page : tabCtrl->GetPages() )
        {
            // This is inefficient for many pages, we should consider using a
            // hash map indexed by the window pointer if this proves to be a
            // problem in practice.
            const int idx = m_tabs.GetIdxFromWindow(page.window);
            if ( idx != n++ )
            {
                // And if the pages are not in order, we must save them too.
                mustSavePages = true;
            }

            pages.push_back(idx);

            // Also remember if this page is pinned.
            switch ( page.kind )
            {
                case wxAuiTabKind::Normal:
                    // Nothing special to do.
                    break;

                case wxAuiTabKind::Pinned:
                    tab.pinned.push_back(idx);
                    break;

                case wxAuiTabKind::Locked:
                    // We don't store locked pages because their status can't
                    // be changed by the user, so it would be pointless to save
                    // and restore them.
                    break;
            }
        }

        // But if none of the conditions above is true, we can avoid saving
        // them, which also allows us not to remove and re-add them when
        // restoring later, see LoadLayout() below.
        if ( mustSavePages )
            tab.pages = std::move(pages);

        serializer.SaveNotebookTabControl(tab);
    }

    serializer.AfterSaveNotebook();
}

void
wxAuiNotebook::LoadLayout(const wxString& name,
                          wxAuiBookDeserializer& deserializer)
{
    if ( wxWindowIsUnavailableForCallbacks(this) ||
            wxAuiNotebookTopologyTransaction::IsActiveFor(this) )
    {
        return;
    }

    wxAuiNotebookTopologyTransaction transaction(this);
    const wxWeakRef<wxAuiNotebook> weakThis(this);
    const auto tabs = deserializer.LoadNotebookTabs(name);

    wxAuiNotebook* book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) )
        return;

    if ( tabs.empty() )
        return;

    // Remove any existing tabs before adding new ones.
    {
        const wxAuiNotebookTopologyTransaction::InternalCall
            allowUnsplit(transaction);
        book->UnsplitAll();
    }

    book = weakThis.get();
    if ( !book || wxWindowIsUnavailableForCallbacks(book) )
        return;

    // Get the only remaining tab control.
    wxAuiTabCtrl* const tabMain = book->GetMainTabCtrl();
    if ( !tabMain || wxWindowIsUnavailableForCallbacks(tabMain) ||
            tabMain->GetParent() != book )
        return;
    const wxWeakRef<wxAuiTabCtrl> weakMain(tabMain);

    // If we don't have anything saved and the pages are in the default order
    // we may not have to do anything at all.
    bool useExistingPages = false;

    // Reset the state of all pinned pages to normal, as this state will be
    // restored from the deserialized data below.
    //
    // Note that we do not do this for the locked pages, as their state can't
    // be changed by the user and so it's not necessarily to save nor restore
    // it.
    bool canonicalKindsChanged = false;
    for ( auto& page : m_tabs.GetPages() )
    {
        if ( page.kind == wxAuiTabKind::Pinned )
        {
            const_cast<wxAuiNotebookPage&>(page).kind = wxAuiTabKind::Normal;
            canonicalKindsChanged = true;
        }
    }
    if ( canonicalKindsChanged )
        wxAuiBumpTabContainerRevision(&m_tabs);

    // This set will contain all the pages that should be pinned.
    std::unordered_set<int> pinned;

    // Remember the active page in the main tab control if we change it.
    const wxWindow* activeInMainTab = nullptr;

    // Keep track of pages we've already added to some tab control: even if the
    // deserialized data is somehow incorrect and duplicates the page indices,
    // we don't want to try to have the same page in more than one tab control.
    std::unordered_set<int> addedPages;
    const int pageCount = m_tabs.GetPageCount();
    wxAuiPageProjection canonicalProjection;
    if ( !wxAuiCapturePageProjection(m_tabs, this, canonicalProjection) )
        return;

    for ( const auto& tab : tabs )
    {
        wxAuiTabCtrl* tabCtrl = nullptr;

        // We'll deal with the pinned pages outside of this loop below, after
        // all pages are in their correct places, for now just remember them.
        pinned.insert(tab.pinned.begin(), tab.pinned.end());

        // The pages pointer will be set to point to either tab.pages or
        // pageDefault in which case it will also be filled.
        std::vector<int> pagesDefault;
        const std::vector<int>* pages = nullptr;

        // We don't need to create a new pane for the main, central tab control,
        // but we still need to order its tabs correctly.
        if ( tab.dock_direction == wxAUI_DOCK_CENTER )
        {
            // Special case: if the pages vector is empty check if we're using
            // the default order, in which case we can avoid doing anything if
            // the pages are already in the same order (which is a common case).
            if ( tab.pages.empty() )
            {
                useExistingPages = true;

                for ( int i = 0; i < pageCount; ++i )
                {
                    if ( tabMain->GetWindowFromIdx(i) != m_tabs.GetWindowFromIdx(i) )
                    {
                        useExistingPages = false;
                        break;
                    }
                }

                if ( useExistingPages )
                {
                    // All pages are in the main tab in the default order
                    // already, so we don't have anything to do except
                    // restoring the active page -- which is in this case the
                    // same as selection (tab indices == notebook indices).
                    SetSelection(tab.active);
                    break;
                }

                // The pages had been reordered, restore the default order when
                // re-adding them below.
                for ( int i = 0; i < pageCount; ++i )
                    pagesDefault.push_back(i);

                pages = &pagesDefault;
            }
            else
            {
                // Just use the specified pages.
                pages = &tab.pages;
            }

            // In any case, we must remove all pages currently in this tab as
            // they will be re-added.
            tabMain->RemoveAll();

            tabCtrl = tabMain;
        }
        else // Non-central tab control.
        {
            // In this case, we must have some pages, but if we somehow don't,
            // just skip adding the tab control entirely.
            if ( tab.pages.empty() )
                continue;

            // Re-add using the saved layout.
            wxAuiPaneInfo pane;
            m_mgr.CopyDockLayoutTo(tab, pane);

            // Our panes never show caption.
            pane.CaptionVisible(false);

            wxAuiTabFrame* tabframe = CreateTabFrame();
            if ( !tabframe )
                return;
            m_mgr.AddPane(tabframe, pane);

            tabCtrl = tabframe->m_tabs;
            pages = &tab.pages;
        }

        // In any case, add the pages that this tab control had before to it.
        //
        // There is one extra complication: we must ensure that any locked
        // pages still come first, even if the deserializer positioned them
        // wrongly, because this is an invariant of wxAuiNotebook and has to be
        // respected. So instead of adding the pages directly, collect them in
        // this vector first.
        std::vector<int> pagesToAdd;
        pagesToAdd.reserve(pages->size());

        int lockedCount = 0;
        for ( auto page : *pages )
        {
            // We just silently ignore invalid or duplicate page indices
            // here, because it doesn't seem right for them to result in a
            // fatal error and any warnings would be just annoying.
            if ( page >= pageCount )
                continue;

            if ( !addedPages.insert(page).second )
                continue;

            if ( m_tabs.GetPage(page).kind == wxAuiTabKind::Locked )
            {
                // Locked pages must always come first, so insert them at the
                // beginning (but keep their original order).
                pagesToAdd.insert(pagesToAdd.begin() + lockedCount++, page);
            }
            else
            {
                pagesToAdd.push_back(page);
            }
        }

        // In the degenerate case when all pages are invalid, pagesToAdd may be
        // empty here, but we can still execute the rest of the code below, it
        // just won't do anything.

        const wxWindow* activeWindow = nullptr;
        for ( auto page : pagesToAdd )
        {
            const auto& info = m_tabs.GetPage(page);

            if ( page == tab.active )
                activeWindow = info.window;

            tabCtrl->AddPage(info);
        }

        if ( !activeWindow )
        {
            // We must have some active page, so make the first one active if
            // the deserialized data didn't define a valid active page.
            activeWindow = tabCtrl->GetWindowFromIdx(0);
        }

        tabCtrl->SetActivePage(activeWindow);

        if ( tabCtrl == tabMain )
        {
            // Remember it to set the selection to it below.
            activeInMainTab = activeWindow;
        }
    }

    // Check if there were any existing pages not added to any tab control.
    if ( !useExistingPages && wxSsize(addedPages) < pageCount )
    {
        // Use a stack here to remove the pages from the end below.
        std::stack<int> toRemove;

        for ( int i = 0; i < pageCount; ++i )
        {
            if ( addedPages.count(i) == 0 )
            {
                wxAuiTabCtrl* tabCtrl = tabMain;
                int tabIndex = wxNOT_FOUND;
                const bool keepPage = deserializer.HandleOrphanedPage(
                    *this, i, &tabCtrl, &tabIndex);

                book = weakThis.get();
                if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                        weakMain.get() != tabMain ||
                        wxWindowIsUnavailableForCallbacks(tabMain) ||
                        tabMain->GetParent() != book ||
                        !wxAuiMatchesPageProjection(book->m_tabs, book,
                                                    canonicalProjection) )
                {
                    return;
                }

                if ( keepPage )
                {
                    // Never trust a raw tabCtrl supplied by application code:
                    // resolve it against the current live manager topology
                    // before constructing a weak reference or dereferencing it.
                    wxAuiTabCtrl* resolvedTabCtrl = nullptr;
                    for ( const auto& pane : book->m_mgr.GetAllPanes() )
                    {
                        if ( IsDummyPane(pane) )
                            continue;
                        auto* const frame =
                            static_cast<wxAuiTabFrame*>(pane.window);
                        if ( !frame ||
                                wxWindowIsUnavailableForCallbacks(frame) )
                        {
                            return;
                        }
                        wxAuiTabCtrl* const candidate = frame->m_tabs;
                        if ( !candidate ||
                                wxWindowIsUnavailableForCallbacks(candidate) ||
                                candidate->GetParent() != book )
                        {
                            return;
                        }
                        if ( candidate == tabCtrl )
                            resolvedTabCtrl = candidate;
                    }

                    // Try not to crash even if the deserializer implements
                    // HandleOrphanedPage() incorrectly.
                    if ( !resolvedTabCtrl )
                    {
                        wxFAIL_MSG
                        (
                            "HandleOrphanedPage() can't return null tab control"
                        );
                        resolvedTabCtrl = tabMain;
                    }

                    tabCtrl = resolvedTabCtrl;
                    const wxWeakRef<wxAuiTabCtrl> weakTabCtrl(tabCtrl);

                    const int tabCount = tabCtrl->GetPageCount();
                    if ( tabIndex < wxNOT_FOUND ||
                            (tabIndex != wxNOT_FOUND &&
                             tabIndex >= tabCount) )
                    {
                        wxFAIL_MSG
                        (
                            "HandleOrphanedPage() must return valid tab index"
                        );

                        tabIndex = wxNOT_FOUND;
                    }

                    if ( tabIndex == wxNOT_FOUND )
                        tabIndex = tabCount;

                    wxAuiPageProjection tabBefore;
                    if ( !wxAuiCapturePageProjection(*tabCtrl, book,
                                                     tabBefore) )
                    {
                        return;
                    }
                    wxAuiPageProjection tabAfter = tabBefore;
                    try
                    {
                        tabAfter.insert(tabAfter.begin() + tabIndex,
                                        canonicalProjection[
                                            static_cast<size_t>(i)]);
                    }
                    catch ( const std::exception& )
                    {
                        return;
                    }
                    tabCtrl->InsertPage(book->m_tabs.GetPage(i), tabIndex);
                    book = weakThis.get();
                    if ( !book || wxWindowIsUnavailableForCallbacks(book) ||
                            weakTabCtrl.get() != tabCtrl ||
                            wxWindowIsUnavailableForCallbacks(tabCtrl) ||
                            tabCtrl->GetParent() != book ||
                            !wxAuiMatchesPageProjection(book->m_tabs, book,
                                                        canonicalProjection) ||
                            !wxAuiMatchesPageProjection(*tabCtrl, book,
                                                        tabAfter) )
                    {
                        return;
                    }
                }
                else // Remove this page.
                {
                    toRemove.push(i);
                }
            }
        }

        while ( !toRemove.empty() )
        {
            // Note that we shouldn't call DoRemovePage() because it supposes
            // that the page is in some tab control and does nothing if this is
            // not the case.
            m_tabs.RemovePageAt(toRemove.top());

            toRemove.pop();
        }

        RemoveEmptyTabFrames();
    }

    // Now deal with the pinned pages: we must ensure that their positions are
    // consistent, i.e. there are no non-pinned pages before a pinned one in
    // any tab control.
    if ( !pinned.empty() )
    {
        for ( auto tabCtrl : GetAllTabCtrls() )
        {
            bool stop = false;
            for ( auto n : GetPagesInDisplayOrder(tabCtrl) )
            {
                auto& page = m_tabs.GetPage(n);
                switch ( page.kind )
                {
                    case wxAuiTabKind::Normal:
                        if ( pinned.count(n) )
                        {
                            // Make this page pinned. We don't need to call
                            // SetPageKind() for this, as it's already in the
                            // right place, just update its kind directly.
                            page.kind = wxAuiTabKind::Pinned;
                            wxAuiBumpTabContainerRevision(&m_tabs);

                            int idx = tabCtrl->GetIdxFromWindow(page.window);
                            if ( idx == wxNOT_FOUND )
                            {
                                // This would be a logic error in this code.
                                wxFAIL_MSG("page not found in tab control");
                                break;
                            }

                            tabCtrl->GetPage(idx).kind = wxAuiTabKind::Pinned;
                            wxAuiBumpTabContainerRevision(tabCtrl);
                        }
                        else
                        {
                            // There can be no pinned pages after the first
                            // normal one, so there is no point in continuing:
                            // even if any other pages were marked as pinned in
                            // the deserialized data, we would just ignore them
                            // anyhow.
                            stop = true;
                        }
                        break;

                    case wxAuiTabKind::Pinned:
                        // This would indicate a logic error in this function,
                        // as we unpinned all the pages at the start of it.
                        wxFAIL_MSG("all pages should be unpinned");
                        break;

                    case wxAuiTabKind::Locked:
                        // We can't change the kind of a locked page, so either
                        // it is before the pinned pages or data is invalid,
                        // leave it as is in any case.
                        break;
                }

                if ( stop )
                    break;
            }
        }
    }

    // Update the active pages visibility in all tab controls after adding and
    // removing all the pages.
    for ( auto tabCtrl : GetAllTabCtrls() )
    {
        tabCtrl->DoUpdateActive();
    }

    // We don't save information about the currently focused tab control, so
    // always make the main one active after loading the layout by setting the
    // selected page to the page active in it (if there is no such page, it
    // means we're reusing the existing pages and so don't need to do anything).
    if ( activeInMainTab )
        m_curPage = m_tabs.GetIdxFromWindow(activeInMainTab);

    UpdateHintWindowSize();

    m_mgr.Update();
}

#endif // wxUSE_AUI
