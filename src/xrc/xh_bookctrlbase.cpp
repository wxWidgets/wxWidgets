///////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_bookctrlbase.cpp
// Purpose:     wxBookCtrlXmlHandlerBase implementation
// Author:      Vadim Zeitlin
// Created:     2022-02-24
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_XRC && wxUSE_BOOKCTRL

#include "wx/xrc/xh_bookctrlbase.h"

#include "wx/bookctrl.h"
#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include <memory>
#include <vector>

namespace
{

struct BookPageCreationContext
{
    BookPageCreationContext(wxBookCtrlXmlHandlerBase* handler_,
                            wxBookCtrlBase* book_)
        : handler(handler_), book(book_), bookLifetime(book_)
    {
    }

    wxBookCtrlXmlHandlerBase* handler;
    wxBookCtrlBase* book;
    wxWeakRef<wxWindow> bookLifetime;
    wxImageList* incomingImageList { nullptr };
    bool failed { false };
    std::vector<wxWindow*> ownedPages;
    std::vector<wxWeakRef<wxWindow>> ownedPageLifetimes;
};

thread_local std::vector<BookPageCreationContext*>
    gs_bookPageCreationContexts;

void MarkCurrentBookPageCreationFailed(
    wxBookCtrlXmlHandlerBase* handler,
    wxBookCtrlBase* book)
{
    for ( std::vector<BookPageCreationContext*>::reverse_iterator it =
              gs_bookPageCreationContexts.rbegin();
          it != gs_bookPageCreationContexts.rend();
          ++it )
    {
        BookPageCreationContext* const context = *it;
        if ( context->handler == handler && context->book == book )
        {
            context->failed = true;
            return;
        }
    }
}

BookPageCreationContext* FindCurrentBookPageCreationContext(
    wxBookCtrlXmlHandlerBase* handler,
    const wxBookCtrlBase* address)
{
    for ( std::vector<BookPageCreationContext*>::reverse_iterator it =
              gs_bookPageCreationContexts.rbegin();
          it != gs_bookPageCreationContexts.rend();
          ++it )
    {
        BookPageCreationContext* const context = *it;
        if ( context->handler == handler && context->book == address )
            return context;
    }

    return nullptr;
}

bool IsLiveBook(const wxWeakRef<wxWindow>& weakBook,
                const wxBookCtrlBase* book)
{
    return wxWeakWindowIsAvailableForCallbacks(weakBook, book);
}

wxWindow* FindLiveDirectBookChildByAddress(
    wxBookCtrlBase* book,
    const wxObject* address,
    const wxWeakRef<wxWindow>& weakBook)
{
    if ( !address || !IsLiveBook(weakBook, book) )
        return nullptr;

    for ( wxWindowList::compatibility_iterator node =
              book->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const child = node->GetData();
        if ( static_cast<const wxObject*>(child) != address )
            continue;

        const wxWeakRef<wxWindow> weakChild(child);
        return IsLiveBook(weakBook, book) &&
                       wxWeakWindowIsAvailableForCallbacks(weakChild, child) &&
                       child->GetParent() == book
                   ? child
                   : nullptr;
    }

    return nullptr;
}

} // anonymous namespace

// ============================================================================
// wxBookCtrlXmlHandlerBase implementation
// ============================================================================

wxBookCtrlXmlHandlerBase::PageWithAttrs::PageWithAttrs()
{
    wnd = nullptr;
    selected = false;
    imgId =
    bmpId = wxWithImages::NO_IMAGE;
}

int wxBookCtrlXmlHandlerBase::PageWithAttrs::GetImageId() const
{
    return bmpId != wxWithImages::NO_IMAGE ? bmpId : imgId;
}

wxBookCtrlXmlHandlerBase::wxBookCtrlXmlHandlerBase()
                        : m_isInside(false)
{
}

wxBookCtrlXmlHandlerBase::~wxBookCtrlXmlHandlerBase()
{
    // Dtor is trivial but has to be defined here because it implicitly uses
    // PageWithAttrs which is not fully declared in the header.
}

void
wxBookCtrlXmlHandlerBase::DoAddPage(wxBookCtrlBase* book,
                                    size_t WXUNUSED(n),
                                    const PageWithAttrs& page)
{
    book->AddPage(page.wnd, page.label, page.selected, page.GetImageId());
}

void wxBookCtrlXmlHandlerBase::DoCreatePages(wxBookCtrlBase* book)
{
    // Keep the historical exported ABI exactly: existing derived handlers
    // compiled against the void function continue to link and retain their
    // best-effort semantics. In-tree handlers use the status-returning helper.
    (void)DoCreatePagesSafely(book);
}

void wxBookCtrlXmlHandlerBase::MarkCurrentPageCreationFailed(
    wxBookCtrlBase* book)
{
    MarkCurrentBookPageCreationFailed(this, book);
}

bool wxBookCtrlXmlHandlerBase::DoCreatePagesSafely(wxBookCtrlBase* book)
{
    const wxWeakRef<wxWindow> weakBook(book);
    if ( !IsLiveBook(weakBook, book) )
        return false;

    const bool old_ins = m_isInside;
    m_isInside = true;

    BookPageCreationContext creationContext(this, book);
    gs_bookPageCreationContexts.push_back(&creationContext);
    wxScopeGuard restoreCreationContext = wxMakeGuard(
        [&creationContext]()
        {
            wxUnusedVar(creationContext);
            wxASSERT( !gs_bookPageCreationContexts.empty() &&
                      gs_bookPageCreationContexts.back() ==
                          &creationContext );
            gs_bookPageCreationContexts.pop_back();
        });
    wxUnusedVar(restoreCreationContext);

    wxVector<PageWithAttrs> pagesSave;
    m_bookPages.swap(pagesSave);

    wxVector<wxBitmapBundle> imagesSave;
    m_bookImages.swap(imagesSave);

    wxScopeGuard restoreHandlerState = wxMakeGuard(
        [this, old_ins, &pagesSave, &imagesSave]()
        {
            m_bookImages.swap(imagesSave);
            m_bookPages.swap(pagesSave);
            m_isInside = old_ins;
        });
    wxUnusedVar(restoreHandlerState);

    const size_t initialPageCount = book->GetPageCount();
    if ( !IsLiveBook(weakBook, book) )
        return false;

    std::vector<wxWindow*> initialPages;
    std::vector<wxWeakRef<wxWindow>> initialPageLifetimes;
    initialPages.reserve(initialPageCount);
    initialPageLifetimes.reserve(initialPageCount);
    for ( size_t i = 0; i < initialPageCount; ++i )
    {
        wxWindow* const pageAddress = book->GetPage(i);
        if ( !IsLiveBook(weakBook, book) )
            return false;

        wxWindow* const page = pageAddress
            ? FindLiveDirectBookChildByAddress(book, pageAddress, weakBook)
            : nullptr;
        if ( pageAddress && !page )
            return false;

        initialPages.push_back(page);
        initialPageLifetimes.emplace_back(page);
    }

    const int initialSelection = book->GetSelection();
    if ( !IsLiveBook(weakBook, book) )
        return false;

    if ( initialSelection != wxNOT_FOUND &&
            (initialSelection < 0 ||
             static_cast<size_t>(initialSelection) >= initialPageCount) )
    {
        return false;
    }

    wxWindow* const initialSelectedPage =
        initialSelection == wxNOT_FOUND
            ? nullptr
            : initialPages.at(static_cast<size_t>(initialSelection));
    const bool initialHadImages = book->HasImages();
    if ( !IsLiveBook(weakBook, book) )
        return false;

    const auto initialTopologyIsCurrent = [&]() -> bool
    {
        if ( !IsLiveBook(weakBook, book) )
            return false;

        const size_t pageCount = book->GetPageCount();
        if ( !IsLiveBook(weakBook, book) ||
                pageCount < initialPageCount )
            return false;

        for ( size_t i = 0; i < initialPageCount; ++i )
        {
            wxWindow* const page = initialPages[i];
            if ( page &&
                    !wxWeakWindowIsAvailableForCallbacks(
                        initialPageLifetimes[i], page) )
            {
                return false;
            }

            wxWindow* const currentPage = book->GetPage(i);
            if ( !IsLiveBook(weakBook, book) ||
                    (page && !wxWeakWindowIsAvailableForCallbacks(
                                 initialPageLifetimes[i], page)) ||
                    (page && page->GetParent() != book) ||
                    currentPage != page )
                return false;
        }

        return IsLiveBook(weakBook, book);
    };

    const auto initialTopologyIsDirectlyCurrent = [&]() -> bool
    {
        if ( !IsLiveBook(weakBook, book) ||
                book->wxBookCtrlBase::GetPageCount() < initialPageCount )
        {
            return false;
        }

        for ( size_t i = 0; i < initialPageCount; ++i )
        {
            wxWindow* const page = initialPages[i];
            if ( (page &&
                  (!wxWeakWindowIsAvailableForCallbacks(
                       initialPageLifetimes[i], page) ||
                   page->GetParent() != book)) ||
                    book->wxBookCtrlBase::GetPage(i) != page )
            {
                return false;
            }
        }
        return true;
    };

    std::unique_ptr<wxImageList> incomingImageList(GetImageList());
    if ( !IsLiveBook(weakBook, book) )
        return false;

    creationContext.incomingImageList = incomingImageList.get();

    CreateChildren(book, true /* only use this handler */);
    if ( !IsLiveBook(weakBook, book) )
        return false;

    bool parseSucceeded =
        !creationContext.failed && initialTopologyIsCurrent();
    if ( parseSucceeded )
    {
        const size_t pageCount = book->GetPageCount();
        if ( !IsLiveBook(weakBook, book) )
            return false;
        parseSucceeded = pageCount == initialPageCount;
    }
    if ( parseSucceeded )
    {
        const int selection = book->GetSelection();
        if ( !IsLiveBook(weakBook, book) )
            return false;
        parseSucceeded = selection == initialSelection;
    }

    // Snapshot every new direct child, including a page whose XRC attributes
    // failed after its window was constructed and which consequently wasn't
    // published in m_bookPages. All of them belong to this transaction and
    // must be destroyed on rollback.
    // Only children actually returned by this handler's page creation path
    // belong to the transaction. An application callback may add an unrelated
    // direct child while parsing; rollback must never claim that child.
    const std::vector<wxWindow*> transactionPages(
        creationContext.ownedPages);
    const std::vector<wxWeakRef<wxWindow>> transactionPageLifetimes(
        creationContext.ownedPageLifetimes);

    // Replacing a pre-existing image source is not rollbackable through the
    // public wxWithImages contract (an owned image list may be freed by the
    // replacement). Caller-owned populated books therefore accept new pages
    // using their existing images, but reject an XRC image-source replacement
    // before publishing any page.
    // A nested resource load may temporarily use this same handler. Iterate a
    // stable snapshot, and recover every raw page through the live child list
    // before creating a weak identity from it.
    const wxVector<PageWithAttrs> pages(m_bookPages);
    std::vector<wxWindow*> createdPages;
    std::vector<wxWeakRef<wxWindow>> createdPageLifetimes;
    createdPages.reserve(pages.size());
    createdPageLifetimes.reserve(pages.size());
    for ( size_t i = 0; i < pages.size(); ++i )
    {
        if ( !initialTopologyIsCurrent() )
        {
            parseSucceeded = false;
            break;
        }

        const PageWithAttrs& page = pages[i];
        wxWindow* const livePage = FindLiveDirectBookChildByAddress(
            book, page.wnd, weakBook);
        if ( !livePage )
        {
            parseSucceeded = false;
            break;
        }

        createdPages.push_back(livePage);
        createdPageLifetimes.emplace_back(livePage);
    }

    const auto publishedTopologyIsDirectlyCurrent = [&]() -> bool
    {
        if ( !initialTopologyIsDirectlyCurrent() ||
                createdPages.size() != pages.size() ||
                book->wxBookCtrlBase::GetPageCount() !=
                    initialPageCount + createdPages.size() )
        {
            return false;
        }

        for ( size_t i = 0; i < createdPages.size(); ++i )
        {
            wxWindow* const page = createdPages[i];
            if ( !wxWeakWindowIsAvailableForCallbacks(
                     createdPageLifetimes[i], page) ||
                    page->GetParent() != book ||
                    book->wxBookCtrlBase::GetPage(
                        initialPageCount + i) != page )
            {
                return false;
            }
        }
        return IsLiveBook(weakBook, book);
    };

    bool imageStateInstalled = false;
    bool committed = false;
    const auto rollback = [&]() -> bool
    {
        if ( !IsLiveBook(weakBook, book) )
            return false;

        // Remove only pages created by this transaction. Reverse order is
        // essential for treebook subpages and also preserves the indices of
        // every caller-owned page until the rollback is complete.
        for ( size_t i = transactionPages.size(); i-- > 0; )
        {
            wxWindow* const page = transactionPages[i];
            if ( !page || transactionPageLifetimes[i].get() != page )
                continue;

            const int pageIndex = book->FindPage(page);
            if ( !IsLiveBook(weakBook, book) ||
                    !wxWeakWindowIsAvailableForCallbacks(
                        transactionPageLifetimes[i], page) ||
                    page->GetParent() != book )
                return false;

            bool removed = true;
            if ( pageIndex != wxNOT_FOUND )
            {
                removed = book->RemovePage(static_cast<size_t>(pageIndex));
                if ( !IsLiveBook(weakBook, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(
                            transactionPageLifetimes[i], page) ||
                        page->GetParent() != book )
                    return false;
            }

            if ( !removed )
            {
                const int currentPageIndex = book->FindPage(page);
                if ( !IsLiveBook(weakBook, book) ||
                        !wxWeakWindowIsAvailableForCallbacks(
                            transactionPageLifetimes[i], page) ||
                        page->GetParent() != book ||
                        currentPageIndex != wxNOT_FOUND )
                    return false;
            }

            if ( !IsLiveBook(weakBook, book) )
                return false;

            if ( transactionPageLifetimes[i].get() == page &&
                    !wxWindowIsUnavailableForCallbacks(page) )
            {
                if ( page->GetParent() != book )
                    return false;

                delete page;
                if ( !IsLiveBook(weakBook, book) )
                    return false;
            }
        }

        if ( imageStateInstalled )
        {
            wxWithImages::Images noImages;
            book->SetImages(noImages);
            if ( !IsLiveBook(weakBook, book) )
                return false;
        }

        if ( !initialTopologyIsCurrent() )
            return false;

        const size_t restoredPageCount = book->GetPageCount();
        if ( !IsLiveBook(weakBook, book) ||
                restoredPageCount != initialPageCount )
            return false;

        const int selectedIndex = initialSelectedPage
            ? book->FindPage(initialSelectedPage)
            : wxNOT_FOUND;
        if ( !IsLiveBook(weakBook, book) ||
                (initialSelectedPage &&
                 (!wxWeakWindowIsAvailableForCallbacks(
                      initialPageLifetimes[
                          static_cast<size_t>(initialSelection)],
                      initialSelectedPage) ||
                  initialSelectedPage->GetParent() != book)) )
            return false;

        const int currentSelection = book->GetSelection();
        if ( !IsLiveBook(weakBook, book) )
            return false;

        if ( selectedIndex != currentSelection )
        {
            if ( selectedIndex == wxNOT_FOUND )
            {
                // Removing every newly created page restores wxNOT_FOUND for
                // an initially empty book without crossing another callback.
                if ( initialSelection != wxNOT_FOUND )
                    return false;
            }
            else
            {
                book->ChangeSelection(static_cast<size_t>(selectedIndex));
                if ( !IsLiveBook(weakBook, book) )
                    return false;
            }
        }

        const int restoredSelection = book->GetSelection();
        return IsLiveBook(weakBook, book) &&
               restoredSelection == initialSelection &&
               initialTopologyIsDirectlyCurrent() &&
               book->wxBookCtrlBase::GetPageCount() == initialPageCount &&
               book->wxBookCtrlBase::GetSelection() == initialSelection;
    };

    wxScopeGuard rollbackOnFailure = wxMakeGuard(
        [&]()
        {
            if ( !committed )
                (void)rollback();
        });
    wxUnusedVar(rollbackOnFailure);

    if ( !parseSucceeded )
    {
        // Historically a malformed child page did not make creation of the
        // surrounding book fail: XRC reported the diagnostic and returned an
        // empty (or pre-populated) book. Preserve that compatibility contract
        // when, and only when, the transaction can prove a complete rollback.
        const bool rolledBack = rollback();
        committed = rolledBack;
        return rolledBack;
    }

    if ( initialHadImages &&
            (incomingImageList || !m_bookImages.empty()) )
    {
        return false;
    }

    // The historical implementation replaced an imagelist with the bitmap
    // vector at the end, making any page using an imagelist index ambiguous.
    // Reject this malformed mixed source transaction atomically.
    if ( incomingImageList && !m_bookImages.empty() )
        return false;

    // Install a new image source only after every page window has been parsed
    // and validated. Initially empty books can always roll this state back to
    // empty without losing caller-owned resources.
    if ( !m_bookImages.empty() )
    {
        incomingImageList.reset();
        creationContext.incomingImageList = nullptr;
        book->SetImages(m_bookImages);
        imageStateInstalled = true;
        if ( !initialTopologyIsCurrent() )
            return false;
    }
    else if ( incomingImageList )
    {
        book->AssignImageList(incomingImageList.release());
        creationContext.incomingImageList = nullptr;
        imageStateInstalled = true;
        if ( !initialTopologyIsCurrent() )
            return false;
    }

    int requestedSelectionPage = wxNOT_FOUND;
    for ( size_t i = 0; i < pages.size(); ++i )
    {
        const PageWithAttrs& page = pages[i];
        wxWindow* const livePage = createdPages[i];
        const wxWeakRef<wxWindow>& weakPage = createdPageLifetimes[i];
        if ( !initialTopologyIsCurrent() ||
             !wxWeakWindowIsAvailableForCallbacks(weakPage, livePage) )
        {
            return false;
        }

        const size_t oldPageCount = book->GetPageCount();
        if ( !IsLiveBook(weakBook, book) ||
                !wxWeakWindowIsAvailableForCallbacks(weakPage, livePage) ||
                livePage->GetParent() != book )
            return false;

        PageWithAttrs livePageAttrs(page);
        livePageAttrs.wnd = livePage;
        // Defer selection until every page/controller projection has committed
        // so rollback never has to undo a user-visible selection event.
        livePageAttrs.selected = false;
        DoAddPage(book, i, livePageAttrs);

        if ( !IsLiveBook(weakBook, book) ||
             !wxWeakWindowIsAvailableForCallbacks(weakPage, livePage) )
        {
            return false;
        }

        const int insertedAt = book->FindPage(livePage);
        if ( !IsLiveBook(weakBook, book) ||
             !wxWeakWindowIsAvailableForCallbacks(weakPage, livePage) ||
             livePage->GetParent() != book ||
             insertedAt == wxNOT_FOUND )
        {
            return false;
        }

        const size_t newPageCount = book->GetPageCount();
        if ( !IsLiveBook(weakBook, book) ||
                !wxWeakWindowIsAvailableForCallbacks(weakPage, livePage) ||
                livePage->GetParent() != book ||
                newPageCount != oldPageCount + 1 )
            return false;

        if ( page.selected )
            requestedSelectionPage = static_cast<int>(i);
    }

    if ( requestedSelectionPage != wxNOT_FOUND )
    {
        const size_t selectedPageIndex =
            static_cast<size_t>(requestedSelectionPage);
        wxWindow* const requestedSelection =
            createdPages[selectedPageIndex];
        if ( !wxWeakWindowIsAvailableForCallbacks(
                 createdPageLifetimes[selectedPageIndex],
                 requestedSelection) )
        {
            return false;
        }

        const int requestedIndex = book->FindPage(requestedSelection);
        if ( !IsLiveBook(weakBook, book) ||
                !wxWeakWindowIsAvailableForCallbacks(
                    createdPageLifetimes[selectedPageIndex],
                    requestedSelection) ||
                requestedSelection->GetParent() != book ||
                requestedIndex == wxNOT_FOUND )
        {
            return false;
        }

        book->SetSelection(static_cast<size_t>(requestedIndex));
        if ( !IsLiveBook(weakBook, book) ||
                !wxWeakWindowIsAvailableForCallbacks(
                    createdPageLifetimes[selectedPageIndex],
                    requestedSelection) ||
                requestedSelection->GetParent() != book )
            return false;

        const int finalSelection = book->GetSelection();
        if ( !IsLiveBook(weakBook, book) ||
                !wxWeakWindowIsAvailableForCallbacks(
                    createdPageLifetimes[selectedPageIndex],
                    requestedSelection) ||
                requestedSelection->GetParent() != book ||
                finalSelection != requestedIndex )
            return false;
    }

    if ( !initialTopologyIsCurrent() ||
            !publishedTopologyIsDirectlyCurrent() )
    {
        return false;
    }

    committed = true;
    return true;
}

wxObject*
wxBookCtrlXmlHandlerBase::DoCreatePage(wxBookCtrlBase* book)
{
    BookPageCreationContext* const creationContext =
        FindCurrentBookPageCreationContext(this, book);
    if ( !creationContext )
        return nullptr;

    const wxWeakRef<wxWindow> weakBook = creationContext->bookLifetime;
    book = creationContext->book;

    if ( !IsLiveBook(weakBook, book) )
        return nullptr;

    bool pagePublished = false;
    wxScopeGuard markFailed = wxMakeGuard(
        [this, book, &pagePublished]()
        {
            if ( !pagePublished )
                MarkCurrentBookPageCreationFailed(this, book);
        });
    wxUnusedVar(markFailed);

    wxXmlNode *n = GetParamNode(wxT("object"));

    if ( !n )
        n = GetParamNode(wxT("object_ref"));

    if (n)
    {
        const bool old_ins = m_isInside;
        m_isInside = false;
        wxScopeGuard restoreInside = wxMakeGuard(
            [this, old_ins]() { m_isInside = old_ins; });
        wxUnusedVar(restoreInside);

        wxObject *item = CreateResFromNode(n, book, nullptr);
        m_isInside = old_ins;
        if ( !IsLiveBook(weakBook, book) )
            return nullptr;

        // item may already have been destroyed by a child callback. Resolve
        // it from the live direct-child list before using RTTI or properties.
        wxWindow* const wnd =
            FindLiveDirectBookChildByAddress(book, item, weakBook);

        if (wnd)
        {
            const wxWeakRef<wxWindow> weakPage(wnd);
            creationContext->ownedPages.push_back(wnd);
            creationContext->ownedPageLifetimes.emplace_back(wnd);
            PageWithAttrs currentPage;

            if ( HasParam(wxT("bitmap")) )
            {
                const wxBitmapBundle bitmap =
                    GetBitmapBundle(wxT("bitmap"), wxART_OTHER);
                if ( !IsLiveBook(weakBook, book) ||
                     !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) )
                {
                    return nullptr;
                }

                m_bookImages.push_back(bitmap);
                currentPage.bmpId = m_bookImages.size() - 1;
            }
            else if ( HasParam(wxT("image")) )
            {
                wxImageList* const imageList =
                    creationContext->incomingImageList
                        ? creationContext->incomingImageList
                        : book->GetImageList();
                if ( !IsLiveBook(weakBook, book) ||
                     !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) )
                {
                    return nullptr;
                }

                if ( imageList )
                {
                    currentPage.imgId = (int)GetLong(wxT("image"));
                    if ( !IsLiveBook(weakBook, book) ||
                         !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) )
                    {
                        return nullptr;
                    }
                }
                else // image without image list?
                {
                    ReportError(n, "image can only be used in conjunction "
                                   "with imagelist");
                    return nullptr;
                }
            }

            currentPage.wnd = wnd;
            currentPage.label = GetText(wxT("label"));
            if ( !IsLiveBook(weakBook, book) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) )
            {
                return nullptr;
            }

            currentPage.selected = GetBool(wxT("selected"));
            if ( !IsLiveBook(weakBook, book) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakPage, wnd) )
            {
                return nullptr;
            }

            m_bookPages.push_back(currentPage);
            pagePublished = true;
        }
        else
        {
            ReportError(n, wxString::Format("%s child must be a window", m_class));
        }
        return wnd;
    }
    else
    {
        ReportError(wxString::Format("%s must have a window child", m_class));
        return nullptr;
    }
}

#endif // wxUSE_XRC && wxUSE_BOOKCTRL
