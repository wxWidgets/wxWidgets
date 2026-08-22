/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_simplebook.cpp
// Purpose:     XRC resource handler for wxSimplebook
// Author:      Vaclav Slavik
// Created:     2014-08-05
// Copyright:   (c) 2014 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_BOOKCTRL

#include "wx/xrc/xh_simplebook.h"

#ifndef WX_PRECOMP
#endif

#include "wx/simplebook.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "xmlrespriv.h"

#include <vector>

namespace
{

struct SimplebookPageCreationContext
{
    wxSimplebookXmlHandler* const handler;
    wxSimplebook* const book;
    const wxWeakRef<wxWindow> bookLifetime;
    bool failed{false};
    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
};

thread_local std::vector<SimplebookPageCreationContext*>
    gs_simplebookCreationContexts;

SimplebookPageCreationContext* FindSimplebookContext(
    wxSimplebookXmlHandler* const handler,
    wxSimplebook* const book)
{
    for ( auto i = gs_simplebookCreationContexts.rbegin();
          i != gs_simplebookCreationContexts.rend(); ++i )
    {
        SimplebookPageCreationContext* const context = *i;
        if ( context->handler == handler && context->book == book &&
                context->bookLifetime.get() == book )
        {
            return context;
        }
    }

    return nullptr;
}

bool IsLiveSimplebook(const wxWeakRef<wxWindow>& lifetime,
                      wxSimplebook* const book)
{
    return wxWeakWindowIsAvailableForCallbacks(lifetime, book);
}

wxWindow* FindLiveDirectSimplebookChildByAddress(
    wxSimplebook* const book,
    const void* const address,
    const wxWeakRef<wxWindow>& bookLifetime)
{
    if ( !IsLiveSimplebook(bookLifetime, book) )
        return nullptr;

    for ( wxWindowList::compatibility_iterator node =
              book->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const child = node->GetData();
        if ( static_cast<const void*>(child) == address &&
                !wxWindowIsUnavailableForCallbacks(child) &&
                child->GetParent() == book )
        {
            return child;
        }
    }

    return nullptr;
}

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxSimplebookXmlHandler, wxXmlResourceHandler);

wxSimplebookXmlHandler::wxSimplebookXmlHandler()
                      : wxXmlResourceHandler(),
                        m_isInside(false),
                        m_simplebook(nullptr)
{
    AddWindowStyles();
}

wxObject *wxSimplebookXmlHandler::DoCreateResource()
{
    if (m_class == wxS("simplebookpage"))
    {
        wxSimplebook* const book = m_simplebook;
        SimplebookPageCreationContext* const context =
            FindSimplebookContext(this, book);
        if ( !context ||
                !IsLiveSimplebook(context->bookLifetime, book) )
        {
            return nullptr;
        }

        bool committed = false;
        const wxScopeGuard markFailed = wxMakeGuard(
            [context, &committed]()
            {
                if ( !committed )
                    context->failed = true;
            });
        wxUnusedVar(markFailed);

        wxXmlNode *n = GetParamNode(wxS("object"));
        if ( !IsLiveSimplebook(context->bookLifetime, book) )
            return nullptr;

        if ( !n )
        {
            n = GetParamNode(wxS("object_ref"));
            if ( !IsLiveSimplebook(context->bookLifetime, book) )
                return nullptr;
        }

        if (n)
        {
            const bool old_ins = m_isInside;
            m_isInside = false;
            const wxScopeGuard restoreInside = wxMakeGuard(
                [this, old_ins]() { m_isInside = old_ins; });
            wxUnusedVar(restoreInside);

            wxObject * const item = CreateResFromNode(n, book, nullptr);
            m_isInside = old_ins;
            if ( !IsLiveSimplebook(context->bookLifetime, book) )
                return nullptr;

            // The returned pointer can already be dangling. Recover the
            // object by identity from the book's live child list before RTTI
            // or any virtual access.
            wxWindow* const wnd = FindLiveDirectSimplebookChildByAddress(
                book, item, context->bookLifetime);

            if (wnd)
            {
                const wxWeakRef<wxWindow> weakPage(wnd);
                context->pages.push_back(wnd);
                context->pageLifetimes.emplace_back(wnd);

                const wxString label = GetText(wxS("label"));
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book )
                {
                    return nullptr;
                }

                const bool selected = GetBool(wxS("selected"));
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book )
                {
                    return nullptr;
                }

                const size_t oldCount = book->GetPageCount();
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book )
                    return nullptr;

                const bool added = book->AddPage(wnd, label, selected);
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book || !added )
                {
                    return nullptr;
                }

                const int pageIndex = book->FindPage(wnd);
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book ||
                        pageIndex == wxNOT_FOUND )
                {
                    return nullptr;
                }

                const size_t newCount = book->GetPageCount();
                if ( !IsLiveSimplebook(context->bookLifetime, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) ||
                        wnd->GetParent() != book ||
                        newCount != oldCount + 1 )
                {
                    return nullptr;
                }

                committed = true;
            }
            else
            {
                ReportError(n, "simplebookpage child must be a window");
            }
            return wnd;
        }
        else
        {
            ReportError("simplebookpage must have a window child");
            return nullptr;
        }
    }

    else
    {
        const bool factoryOwned = wxXRCIsCurrentInstanceFactoryOwned(this);
        const bool ownsBook = m_instance == nullptr || factoryOwned;
        wxSimplebook* const sb =
            m_instance ? wxDynamicCast(m_instance, wxSimplebook)
                       : new wxSimplebook;
        if ( !sb )
        {
            if ( factoryOwned )
            {
                wxObject* const wrongInstance = m_instance;
                m_instance = nullptr;
                delete wrongInstance;
            }
            ReportError("provided instance is not a wxSimplebook");
            return nullptr;
        }

        const wxWeakRef<wxWindow> weakBook(sb);
        wxWindow* const parent = m_parentAsWindow;
        const wxWeakRef<wxWindow> weakParent(parent);
        const auto contextIsLive = [&]()
        {
            return IsLiveSimplebook(weakBook, sb) &&
                   (!parent ||
                    wxWeakWindowIsAvailableForCallbacks(weakParent, parent));
        };
        const auto createdBookIsLive = [&]()
        {
            return contextIsLive() && sb->GetParent() == parent;
        };
        const auto discardOwnedBook = [&]()
        {
            if ( ownsBook && weakBook.get() == sb &&
                    !wxWindowItselfIsUnavailableForCallbacks(sb) )
            {
                delete sb;
            }
        };

        const wxWindowID id = GetID();
        const wxPoint position = GetPosition();
        const wxSize size = GetSize();
        const long style = GetStyle(wxS("style"));
        const wxString name = GetName();
        if ( !contextIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        const bool created =
            sb->Create(parent, id, position, size, style, name);
        if ( !created || !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        SetupWindow(sb);
        if ( !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        const size_t initialCount = sb->GetPageCount();
        if ( !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        std::vector<wxWindow*> initialPages;
        std::vector<wxWeakRef<wxWindow>> initialPageLifetimes;
        initialPages.reserve(initialCount);
        initialPageLifetimes.reserve(initialCount);
        for ( size_t i = 0; i < initialCount; ++i )
        {
            wxWindow* const pageAddress = sb->GetPage(i);
            if ( !createdBookIsLive() )
            {
                discardOwnedBook();
                return nullptr;
            }
            wxWindow* const page = pageAddress
                ? FindLiveDirectSimplebookChildByAddress(
                      sb, pageAddress, weakBook)
                : nullptr;
            if ( pageAddress && !page )
            {
                discardOwnedBook();
                return nullptr;
            }
            initialPages.push_back(page);
            initialPageLifetimes.emplace_back(page);
        }

        const int initialSelection = sb->GetSelection();
        if ( !createdBookIsLive() )
        {
            discardOwnedBook();
            return nullptr;
        }

        wxSimplebook * const old_par = m_simplebook;
        const wxWeakRef<wxWindow> weakOld(old_par);
        m_simplebook = sb;
        const bool old_ins = m_isInside;
        m_isInside = true;

        const wxScopeGuard restoreHandlerState = wxMakeGuard(
            [this, old_par, weakOld, old_ins]()
            {
                m_simplebook = nullptr;
                if ( old_par &&
                        wxWeakWindowIsAvailableForCallbacks(
                            weakOld, old_par) )
                {
                    m_simplebook = old_par;
                }
                m_isInside = old_ins;
            });
        wxUnusedVar(restoreHandlerState);

        SimplebookPageCreationContext creationContext
            { this, sb, weakBook };
        gs_simplebookCreationContexts.push_back(&creationContext);
        const wxScopeGuard restoreCreationContext = wxMakeGuard(
            [&creationContext]()
            {
                wxASSERT( !gs_simplebookCreationContexts.empty() &&
                          gs_simplebookCreationContexts.back() ==
                              &creationContext );
                gs_simplebookCreationContexts.pop_back();
            });
        wxUnusedVar(restoreCreationContext);

        CreateChildren(sb, true /* only use this handler */);
        if ( !createdBookIsLive() )
        {
            // A factory-owned instance remains resource-owned even if a
            // callback reparented it while parsing. Do not leak that failed
            // object graph merely because its parent identity changed.
            discardOwnedBook();
            return nullptr;
        }

        const auto initialTopologyIsCurrent = [&]()
        {
            if ( !createdBookIsLive() )
                return false;

            const size_t count = sb->GetPageCount();
            if ( !createdBookIsLive() || count < initialCount )
                return false;

            for ( size_t i = 0; i < initialCount; ++i )
            {
                wxWindow* const page = initialPages[i];
                if ( (page &&
                        !wxWeakWindowIsAvailableForCallbacks(
                            initialPageLifetimes[i], page)) )
                {
                    return false;
                }

                wxWindow* const currentPage = sb->GetPage(i);
                if ( !createdBookIsLive() ||
                        (page && !wxWeakWindowIsAvailableForCallbacks(
                                     initialPageLifetimes[i], page)) ||
                        (page && page->GetParent() != sb) ||
                        currentPage != page )
                    return false;
            }
            return true;
        };

        const auto initialTopologyIsDirectlyCurrent = [&]()
        {
            if ( !createdBookIsLive() ||
                    sb->wxBookCtrlBase::GetPageCount() < initialCount )
            {
                return false;
            }

            for ( size_t i = 0; i < initialCount; ++i )
            {
                wxWindow* const page = initialPages[i];
                if ( (page &&
                      (!wxWeakWindowIsAvailableForCallbacks(
                           initialPageLifetimes[i], page) ||
                       page->GetParent() != sb)) ||
                        sb->wxBookCtrlBase::GetPage(i) != page )
                {
                    return false;
                }
            }
            return true;
        };

        bool creationSucceeded =
            !creationContext.failed && initialTopologyIsCurrent();
        if ( creationSucceeded )
        {
            const size_t count = sb->GetPageCount();
            if ( !createdBookIsLive() )
                return nullptr;
            creationSucceeded =
                count == initialCount + creationContext.pages.size();
        }
        for ( size_t i = 0;
              creationSucceeded && i < creationContext.pages.size(); ++i )
        {
            wxWindow* const page = creationContext.pages[i];
            if ( creationContext.pageLifetimes[i].get() != page ||
                    wxWindowIsUnavailableForCallbacks(page) ||
                    page->GetParent() != sb )
            {
                creationSucceeded = false;
                break;
            }

            const int pageIndex = sb->FindPage(page);
            if ( !createdBookIsLive() ||
                    creationContext.pageLifetimes[i].get() != page ||
                    wxWindowIsUnavailableForCallbacks(page) ||
                    page->GetParent() != sb ||
                    pageIndex == wxNOT_FOUND )
            {
                creationSucceeded = false;
            }
        }

        const auto rollback = [&]()
        {
            if ( !createdBookIsLive() )
                return false;

            for ( size_t i = creationContext.pages.size(); i-- > 0; )
            {
                wxWindow* const page = creationContext.pages[i];
                if ( !page ||
                        creationContext.pageLifetimes[i].get() != page )
                {
                    continue;
                }

                const int pageIndex = sb->FindPage(page);
                if ( !createdBookIsLive() ||
                        creationContext.pageLifetimes[i].get() != page ||
                        wxWindowIsUnavailableForCallbacks(page) ||
                        page->GetParent() != sb )
                    return false;
                if ( pageIndex != wxNOT_FOUND )
                {
                    const bool removed =
                        sb->RemovePage(static_cast<size_t>(pageIndex));
                    if ( !createdBookIsLive() ||
                            creationContext.pageLifetimes[i].get() != page ||
                            wxWindowIsUnavailableForCallbacks(page) ||
                            page->GetParent() != sb )
                        return false;
                    if ( !removed )
                    {
                        const int stillPresent = sb->FindPage(page);
                        if ( !createdBookIsLive() ||
                                creationContext.pageLifetimes[i].get() != page ||
                                wxWindowIsUnavailableForCallbacks(page) ||
                                page->GetParent() != sb ||
                                stillPresent != wxNOT_FOUND )
                        {
                            return false;
                        }
                    }
                }

                if ( creationContext.pageLifetimes[i].get() == page &&
                        !wxWindowIsUnavailableForCallbacks(page) )
                {
                    if ( page->GetParent() != sb )
                        return false;
                    delete page;
                    if ( !createdBookIsLive() )
                        return false;
                }
            }

            if ( !initialTopologyIsCurrent() )
                return false;
            const size_t count = sb->GetPageCount();
            if ( !createdBookIsLive() || count != initialCount )
                return false;

            const int selection = sb->GetSelection();
            if ( !createdBookIsLive() )
                return false;
            if ( selection != initialSelection )
            {
                if ( initialSelection == wxNOT_FOUND )
                    return false;
                sb->ChangeSelection(static_cast<size_t>(initialSelection));
                if ( !createdBookIsLive() )
                    return false;
            }

            const int restoredSelection = sb->GetSelection();
            return createdBookIsLive() &&
                   restoredSelection == initialSelection &&
                   initialTopologyIsDirectlyCurrent() &&
                   sb->wxBookCtrlBase::GetPageCount() == initialCount &&
                   sb->wxBookCtrlBase::GetSelection() == initialSelection;
        };

        if ( !creationSucceeded )
        {
            // Preserve the historical XRC compatibility contract: malformed
            // pages yield a valid empty/pre-populated book if and only if the
            // transaction can prove a complete rollback.
            if ( !rollback() )
            {
                discardOwnedBook();
                return nullptr;
            }
            return sb;
        }

        if ( !initialTopologyIsCurrent() ||
                !initialTopologyIsDirectlyCurrent() ||
                sb->wxBookCtrlBase::GetPageCount() !=
                    initialCount + creationContext.pages.size() )
        {
            discardOwnedBook();
            return nullptr;
        }
        for ( size_t i = 0; i < creationContext.pages.size(); ++i )
        {
            wxWindow* const page = creationContext.pages[i];
            if ( creationContext.pageLifetimes[i].get() != page ||
                    wxWindowIsUnavailableForCallbacks(page) ||
                    page->GetParent() != sb ||
                    sb->wxBookCtrlBase::GetPage(initialCount + i) != page )
            {
                discardOwnedBook();
                return nullptr;
            }
        }

        return sb;
    }
}

bool wxSimplebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, wxS("wxSimplebook"))) ||
            (m_isInside && IsOfClass(node, wxS("simplebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_BOOKCTRL
