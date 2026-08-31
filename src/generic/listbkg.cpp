///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/listbkg.cpp
// Purpose:     generic implementation of wxListbook
// Author:      Vadim Zeitlin
// Created:     19.08.03
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_LISTBOOK

#include "wx/listbook.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif

#include "wx/listctrl.h"
#include "wx/private/windowlifetime.h"
#include "wx/statline.h"
#include "wx/imaglist.h"
#include "wx/weakref.h"

#include <vector>

namespace
{

// wxListCtrl sends its item notifications from inside a controller mutation.
// A removal must remain authoritative because a nested topology writer could
// otherwise erase a controller item without the corresponding page (or vice
// versa). An insertion is different: it publishes the common model before the
// controller item and its revision checks can safely observe a nested removal.
class wxListbookTopologyTransaction
{
public:
    enum class Kind
    {
        Inserting,
        Removing
    };

    wxListbookTopologyTransaction(wxListbook* const book, Kind kind)
        : m_book(book),
          m_lifetime(book),
          m_kind(kind),
          m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxListbookTopologyTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(wxListbook* const book)
    {
        for ( wxListbookTopologyTransaction* transaction = GetActive();
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book )
            {
                return true;
            }
        }

        return false;
    }

    static bool IsRemovingFor(wxListbook* const book)
    {
        for ( wxListbookTopologyTransaction* transaction = GetActive();
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book &&
                    transaction->m_kind == Kind::Removing )
            {
                return true;
            }
        }

        return false;
    }

private:
    static wxListbookTopologyTransaction*& GetActive()
    {
        static thread_local wxListbookTopologyTransaction* active = nullptr;
        return active;
    }

    wxListbook* const m_book;
    const wxWeakRef<wxListbook> m_lifetime;
    const Kind m_kind;
    wxListbookTopologyTransaction* const m_previous;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxListbook, wxBookCtrlBase);

wxDEFINE_EVENT( wxEVT_LISTBOOK_PAGE_CHANGING, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_LISTBOOK_PAGE_CHANGED,  wxBookCtrlEvent );

wxBEGIN_EVENT_TABLE(wxListbook, wxBookCtrlBase)
    EVT_SIZE(wxListbook::OnSize)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, wxListbook::OnListSelected)
wxEND_EVENT_TABLE()

// ============================================================================
// wxListbook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListbook creation
// ----------------------------------------------------------------------------

bool
wxListbook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
#ifdef __WXMAC__
        style |= wxBK_TOP;
#else // !__WXMAC__
        style |= wxBK_LEFT;
#endif // __WXMAC__/!__WXMAC__
    }

    // no border for this control, it doesn't look nice together with
    // wxListCtrl border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_bookctrl = new wxListView
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    GetListCtrlFlags(HasImages())
                 );

    if ( GetListView()->InReportView() )
        GetListView()->InsertColumn(0, wxS("Pages"));

    // Ensure that we rearrange the items in our list view after all the pages
    // are added.
    PostSizeEvent();

    return true;
}

// ----------------------------------------------------------------------------
// wxListCtrl flags
// ----------------------------------------------------------------------------

long wxListbook::GetListCtrlFlags(bool hasImages) const
{
    // We'd like to always use wxLC_ICON mode but it doesn't work with the
    // native wxListCtrl under MSW unless we do have icons for all the items,
    // so we can't use it if we have no image list. In this case we'd like to
    // use wxLC_LIST mode because it works correctly for both horizontally and
    // vertically laid out controls, but MSW native wxListCtrl insists on
    // creating multiple columns if there are too many items and there doesn't
    // seem anything to do about it, so we have to use wxLC_REPORT mode in this
    // case there.

    long flags = IsVertical() ? wxLC_ALIGN_LEFT : wxLC_ALIGN_TOP;
    if ( hasImages )
    {
        flags |= wxLC_ICON;
    }
    else // No images.
    {
#ifdef __WXMSW__
        if ( !IsVertical() )
        {
            // Notice that we intentionally overwrite the alignment flags here
            // by not using "|=", alignment isn't used for report view.
            flags = wxLC_REPORT | wxLC_NO_HEADER;
        }
        else
#endif // __WXMSW__
        {
            flags |= wxLC_LIST;
        }

#ifdef __WXQT__
        flags |= wxLC_NO_HEADER;
#endif
    }

    // Use single selection in any case.
    return flags | wxLC_SINGLE_SEL;
}

// ----------------------------------------------------------------------------
// wxListbook geometry management
// ----------------------------------------------------------------------------

void wxListbook::OnSize(wxSizeEvent& event)
{
    // arrange the icons before calling SetClientSize(), otherwise it wouldn't
    // account for the scrollbars the list control might need and, at least
    // under MSW, we'd finish with an ugly looking list control with both
    // vertical and horizontal scrollbar (with one of them being added because
    // the other one is not accounted for in client size computations)
    wxListView * const list = GetListView();
    if ( list )
    {
        list->Arrange();

        const int sel = GetSelection();
        if ( sel != wxNOT_FOUND )
            list->EnsureVisible(sel);
    }

    event.Skip();
}

int wxListbook::HitTest(const wxPoint& pt, long *flags) const
{
    int pagePos = wxNOT_FOUND;

    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    // convert from listbook control coordinates to list control coordinates
    const wxListView * const list = GetListView();
    const wxPoint listPt = list->ScreenToClient(ClientToScreen(pt));

    // is the point inside list control?
    if ( wxRect(list->GetSize()).Contains(listPt) )
    {
        int flagsList;
        pagePos = list->HitTest(listPt, flagsList);

        if ( flags )
        {
            if ( pagePos != wxNOT_FOUND )
                *flags = 0;

            if ( flagsList & (wxLIST_HITTEST_ONITEMICON |
                              wxLIST_HITTEST_ONITEMSTATEICON ) )
                *flags |= wxBK_HITTEST_ONICON;

            if ( flagsList & wxLIST_HITTEST_ONITEMLABEL )
                *flags |= wxBK_HITTEST_ONLABEL;
        }
    }
    else // not over list control at all
    {
        if ( flags && GetPageRect().Contains(pt) )
            *flags |= wxBK_HITTEST_ONPAGE;
    }

    return pagePos;
}

void wxListbook::UpdateSize()
{
    // we should find a more elegant way to force a layout than generating this
    // dummy event
    wxSizeEvent sz(GetSize(), GetId());
    GetEventHandler()->ProcessEvent(sz);
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxListbook::SetPageText(size_t n, const wxString& strText)
{
    GetListView()->SetItemText(n, RemoveMnemonics(strText));

    return true;
}

wxString wxListbook::GetPageText(size_t n) const
{
    return GetListView()->GetItemText(n);
}

int wxListbook::GetPageImage(size_t n) const
{
    wxListItem item;
    item.SetId(n);
    item.SetMask(wxLIST_MASK_IMAGE);

    if (GetListView()->GetItem(item))
    {
       return item.GetImage();
    }
    else
    {
       return wxNOT_FOUND;
    }
}

bool wxListbook::SetPageImage(size_t n, int imageId)
{
    return GetListView()->SetItemImage(n, imageId);
}

// ----------------------------------------------------------------------------
// images support
// ----------------------------------------------------------------------------

void wxListbook::OnImagesChanged()
{
    wxListView * const list = GetListView();

    // We may need to change the list control mode if the image list presence
    // has changed.
    const bool hasImages = HasImages();
    if ( list->HasFlag(wxLC_ICON) != hasImages )
    {
        // Preserve the selection which is lost when changing the mode
        const int oldSel = GetSelection();

        // Note that we can't just ToggleWindowStyle(wxLC_ICON) here because we
        // may also need to turn off wxLC_REPORT under MSW.
        list->SetWindowStyleFlag(GetListCtrlFlags(hasImages));
        if ( list->InReportView() )
            list->InsertColumn(0, wxS("Pages"));

        // Restore selection
        if ( oldSel != wxNOT_FOUND )
            SetSelection(oldSel);
    }

    // We also need to propagate the actual images to use to the list control.
    const Images& images = GetImages();
    if ( !images.empty() )
        list->SetNormalImages(images);
    else
        list->SetImageList(GetImageList(), wxIMAGE_LIST_NORMAL);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxListbook::UpdateSelectedPage(size_t newsel)
{
    const wxWeakRef<wxListbook> weakThis(this);
    wxListView* const list = GetListView();
    const wxWeakRef<wxListView> weakList(list);
    const size_t pageCount = wxBookCtrlBase::GetPageCount();
    wxWindow* const selectedPage =
        newsel < pageCount
            ? wxBookCtrlBase::GetPage(newsel)
            : nullptr;
    const wxWeakRef<wxWindow> weakSelectedPage(selectedPage);
    const int selection = m_selection;
    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pages.reserve(pageCount);
    pageLifetimes.reserve(pageCount);
    for ( size_t i = 0; i < pageCount; ++i )
    {
        wxWindow* const page = wxBookCtrlBase::GetPage(i);
        pages.push_back(page);
        pageLifetimes.emplace_back(page);
    }

    list->Select(newsel);

    wxListbook* const book = weakThis.get();
    if ( !book || weakList.get() != list ||
            book->GetListView() != list ||
            book->wxBookCtrlBase::GetPageCount() != pageCount ||
            list->GetItemCount() != static_cast<long>(pageCount) ||
            newsel >= pageCount ||
            weakSelectedPage.get() != selectedPage ||
            book->wxBookCtrlBase::GetPage(newsel) != selectedPage ||
            book->m_selection != selection )
    {
        return;
    }

    for ( size_t i = 0; i < pageCount; ++i )
    {
        if ( pageLifetimes[i].get() != pages[i] ||
                book->wxBookCtrlBase::GetPage(i) != pages[i] )
        {
            return;
        }
    }

    list->Focus(newsel);
}

wxBookCtrlEvent* wxListbook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_LISTBOOK_PAGE_CHANGING, m_windowId);
}

void wxListbook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_LISTBOOK_PAGE_CHANGED);
}


// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxListbook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( wxListbookTopologyTransaction::IsActiveFor(this) )
        return false;

    const wxListbookTopologyTransaction transaction(
        this, wxListbookTopologyTransaction::Kind::Inserting);
    const InsertPageResult modelResult =
        DoInsertPageIntoModel(n, page, text, bSelect, imageId);
    if ( modelResult == InsertPageResult::Failed )
        return false;
    if ( modelResult == InsertPageResult::OwnershipConsumed )
        return true;

    const wxWeakRef<wxListbook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxListView* const list = GetListView();
    const wxWeakRef<wxListView> weakList(list);
    const size_t expectedCount = wxBookCtrlBase::GetPageCount();
    const long controllerCountBefore = list->GetItemCount();
    const int selectionBefore = m_selection;
    const int shiftedSelection =
        selectionBefore != wxNOT_FOUND &&
        static_cast<int>(n) <= selectionBefore
            ? selectionBefore + 1
            : selectionBefore;
    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pages.reserve(expectedCount);
    pageLifetimes.reserve(expectedCount);
    for ( size_t i = 0; i < expectedCount; ++i )
    {
        wxWindow* const expectedPage = wxBookCtrlBase::GetPage(i);
        pages.push_back(expectedPage);
        pageLifetimes.emplace_back(expectedPage);
    }
    const auto getCurrent = [&]() -> wxListbook*
    {
        wxListbook* const book = weakThis.get();
        if ( !book ||
                weakPage.get() != page ||
                page->GetParent() != book ||
                weakList.get() != list ||
                book->GetListView() != list ||
                book->wxBookCtrlBase::GetPageCount() != expectedCount )
        {
            return nullptr;
        }

        for ( size_t i = 0; i < expectedCount; ++i )
        {
            if ( pageLifetimes[i].get() != pages[i] ||
                    book->wxBookCtrlBase::GetPage(i) != pages[i] )
            {
                return nullptr;
            }
        }

        return book;
    };
    const auto finishCommittedInsertion = [&]() -> bool
    {
        wxListbook* book = weakThis.get();
        if ( book &&
                (weakPage.get() != page || page->GetParent() != book) &&
                n < book->wxBookCtrlBase::GetPageCount() &&
                book->wxBookCtrlBase::GetPage(n) == page )
        {
            book->DoErasePageRange(n, 1);
            if ( wxWeakWindowIsAvailableForCallbacks(weakList, list) &&
                    book->GetListView() == list &&
                    list->GetItemCount() ==
                        static_cast<long>(expectedCount) )
            {
                (void)list->DeleteItem(n);
            }

            book = weakThis.get();
            if ( book &&
                    (book->m_selection != selectionBefore ||
                     shiftedSelection == selectionBefore) )
            {
                book->DoSetSelectionAfterRemoval(n);
            }
        }

        book = weakThis.get();
        if ( book )
            (void)book->DoReconcilePageVisibility();

        return true;
    };
    const auto rollbackCommonPage = [&]() -> bool
    {
        wxListbook* const book = getCurrent();
        if ( book && list->GetItemCount() == controllerCountBefore )
        {
            if ( book->m_selection == shiftedSelection )
                book->m_selection = selectionBefore;
            wxWindow* const rolledBack =
                book->wxBookCtrlBase::DoRemovePage(n);
            if ( rolledBack == page && weakPage.get() == page )
                return false;
        }

        // A callback from invalidation can supersede the rollback and
        // republish the candidate. Returning false in that case would tell
        // the caller to delete a page that the book still owns.
        return finishCommittedInsertion();
    };

    if ( controllerCountBefore != static_cast<long>(expectedCount - 1) )
    {
        return rollbackCommonPage();
    }

    // Publish the shifted model selection before InsertItem(): its
    // wxEVT_LIST_INSERT_ITEM callback must observe indices in the already
    // published page topology. If it selects another page, that newer writer
    // is preserved below.
    wxListbook* book = getCurrent();
    if ( !book )
        return finishCommittedInsertion();
    book->m_selection = shiftedSelection;

    const long inserted =
        list->InsertItem(n, RemoveMnemonics(text), imageId);
    book = getCurrent();
    if ( !book )
        return finishCommittedInsertion();

    if ( inserted != static_cast<long>(n) )
    {
        if ( inserted != wxNOT_FOUND )
        {
            list->DeleteItem(inserted);
            book = getCurrent();
            if ( !book )
                return finishCommittedInsertion();
        }

        return rollbackCommonPage();
    }

    if ( list->GetItemCount() != static_cast<long>(expectedCount) )
        return finishCommittedInsertion();

    const bool selectionOvertaken = book->m_selection != shiftedSelection;

    // If the insertion shifted the selected page and no callback superseded
    // it, synchronize the controller selection to the already shifted model.
    if ( !selectionOvertaken && shiftedSelection != selectionBefore )
    {
        list->Select(shiftedSelection);

        book = getCurrent();
        if ( !book ||
                list->GetItemCount() != static_cast<long>(expectedCount) ||
                book->m_selection != shiftedSelection )
        {
            return finishCommittedInsertion();
        }

        list->Focus(shiftedSelection);
        book = getCurrent();
        if ( !book ||
                list->GetItemCount() != static_cast<long>(expectedCount) ||
                book->m_selection != shiftedSelection )
        {
            return finishCommittedInsertion();
        }
    }

    if ( selectionOvertaken )
    {
        const int overtakenSelection = book->m_selection;
        wxWindow* const selectedPage =
            overtakenSelection != wxNOT_FOUND &&
            static_cast<size_t>(overtakenSelection) < expectedCount
                ? book->wxBookCtrlBase::GetPage(overtakenSelection)
                : nullptr;
        const bool pageShown = page->IsShown();
        book = getCurrent();
        if ( !book || book->m_selection != overtakenSelection )
            return finishCommittedInsertion();

        if ( selectedPage != page && pageShown )
        {
            page->Hide();
            book = getCurrent();
            if ( !book )
                return finishCommittedInsertion();
        }

        book->UpdateSize();
        return finishCommittedInsertion();
    }

    if ( !book->DoSetSelectionAfterInsertion(n, bSelect) )
    {
        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();

        page->Hide();
        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();
    }
    else
    {
        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();
    }

    book->UpdateSize();

    return finishCommittedInsertion();
}

wxWindow *wxListbook::DoRemovePage(size_t page)
{
    if ( wxListbookTopologyTransaction::IsRemovingFor(this) )
        return nullptr;

    const size_t pageCount = wxBookCtrlBase::GetPageCount();
    if ( page >= pageCount )
        return wxBookCtrlBase::DoRemovePage(page);

    const wxWeakRef<wxListbook> weakThis(this);
    wxListView* const list = GetListView();
    const wxWeakRef<wxListView> weakList(list);
    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pages.reserve(pageCount);
    pageLifetimes.reserve(pageCount);
    for ( size_t i = 0; i < pageCount; ++i )
    {
        wxWindow* const candidate = wxBookCtrlBase::GetPage(i);
        pages.push_back(candidate);
        pageLifetimes.emplace_back(candidate);
    }

    const auto getOriginal =
        [&](long expectedControllerCount) -> wxListbook*
        {
            wxListbook* const book = weakThis.get();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                    !wxWeakWindowIsAvailableForCallbacks(weakList, list) ||
                    book->GetListView() != list ||
                    book->wxBookCtrlBase::GetPageCount() != pageCount ||
                    list->GetItemCount() != expectedControllerCount )
            {
                return nullptr;
            }

            for ( size_t i = 0; i < pageCount; ++i )
            {
                if ( pageLifetimes[i].get() != pages[i] ||
                        book->wxBookCtrlBase::GetPage(i) != pages[i] )
                {
                    return nullptr;
                }
            }

            return book;
        };
    wxListbook* book = getOriginal(static_cast<long>(pageCount));
    if ( !book )
        return nullptr;

    // DELETE_ITEM is a synchronous application-code boundary. Keep the
    // controller and common model under one transaction so a nested writer
    // can't publish only one half of their shared topology.
    const wxListbookTopologyTransaction transaction(
        this, wxListbookTopologyTransaction::Kind::Removing);

    // Stage the controller mutation first. If it fails, the common model and
    // ownership are untouched and RemovePage()/DeletePage() correctly report
    // failure. The delete event is an application-code boundary, so only
    // publish the model removal if the complete original snapshot survived.
    if ( !list->DeleteItem(page) )
        return nullptr;

    book = getOriginal(static_cast<long>(pageCount - 1));
    if ( !book )
    {
        book = weakThis.get();
        wxWindow* const expectedPage = pages[page];
        if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                wxWeakWindowIsAvailableForCallbacks(weakList, list) &&
                book->GetListView() == list &&
                list->GetItemCount() ==
                    static_cast<long>(pageCount - 1) &&
                book->wxBookCtrlBase::GetPageCount() == pageCount &&
                book->wxBookCtrlBase::GetPage(page) == expectedPage &&
                pageLifetimes[page].get() != expectedPage )
        {
            // DELETE_ITEM consumed the page before the common publication.
            // Remove its now-dangling identity without invoking another
            // callback-capable controller operation.
            book->DoErasePageRange(page, 1);
            book->DoSetSelectionAfterRemoval(page);
        }
        return nullptr;
    }

    wxWindow* const removed = book->wxBookCtrlBase::DoRemovePage(page);
    const wxWeakRef<wxWindow> weakRemoved(removed);
    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return nullptr;

    if ( !removed )
    {
        // DoRemovePage() can commit the common erase and then lose ownership
        // during best-size invalidation. The list projection already
        // committed above, so only the selection/layout tail remains.
        if ( book->wxBookCtrlBase::GetPageCount() == pageCount - 1 )
        {
            book->DoSetSelectionAfterRemoval(page);
            book = weakThis.get();
            if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                    wxWeakWindowIsAvailableForCallbacks(weakList, list) &&
                    book->GetListView() == list )
            {
                list->Arrange();
            }
        }
        return nullptr;
    }

    const auto getCurrent = [&]() -> wxListbook*
    {
        wxListbook* const currentBook = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !wxWeakWindowIsAvailableForCallbacks(weakList, list) ||
                currentBook->GetListView() != list ||
                weakRemoved.get() != removed ||
                currentBook->wxBookCtrlBase::GetPageCount() != pageCount - 1 ||
                list->GetItemCount() != static_cast<long>(pageCount - 1) )
        {
            return nullptr;
        }

        size_t current = 0;
        for ( size_t i = 0; i < pageCount; ++i )
        {
            if ( i == page )
                continue;

            if ( pageLifetimes[i].get() != pages[i] ||
                    currentBook->wxBookCtrlBase::GetPage(current++) != pages[i] )
            {
                return nullptr;
            }
        }

        return currentBook;
    };
    const auto getTransferredPage = [&]() -> wxWindow*
    {
        wxListbook* const book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !wxWeakWindowIsAvailableForCallbacks(weakRemoved, removed) )
            return nullptr;

        // A nested writer may have put the page back into the book. In that
        // case ownership was not transferred to RemovePage()/DeletePage().
        return book->wxBookCtrlBase::FindPage(removed) == wxNOT_FOUND
                    ? removed
                    : nullptr;
    };

    book = getCurrent();
    if ( !book )
        return getTransferredPage();

    book->DoSetSelectionAfterRemoval(page);
    book = getCurrent();
    if ( !book )
        return getTransferredPage();

    list->Arrange();
    book = getCurrent();
    if ( !book )
        return getTransferredPage();

    book->UpdateSize();
    return getTransferredPage();
}


bool wxListbook::DeleteAllPages()
{
    if ( wxListbookTopologyTransaction::IsActiveFor(this) ||
            IsDeletingAllPages() )
    {
        return false;
    }

    const wxWeakRef<wxListbook> weakThis(this);
    if ( !wxBookCtrlBase::DeleteAllPages() )
        return false;

    wxListbook* const book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return false;
    book->UpdateSize();
    return wxWeakWindowIsAvailableForCallbacks(weakThis, this);
}

// ----------------------------------------------------------------------------
// wxListbook events
// ----------------------------------------------------------------------------

void wxListbook::OnListSelected(wxListEvent& eventList)
{
    if ( eventList.GetEventObject() != m_bookctrl )
    {
        eventList.Skip();
        return;
    }

    const int selNew = eventList.GetIndex();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    const wxWeakRef<wxListbook> weakThis(this);
    wxListView* const list = GetListView();
    const wxWeakRef<wxListView> weakList(list);

    SetSelection(selNew);

    wxListbook* book = weakThis.get();
    if ( !book || weakList.get() != list || book->GetListView() != list )
        return;

    // change wasn't allowed, return to previous state
    if ( book->m_selection != selNew )
    {
        list->Select(book->m_selection);

        book = weakThis.get();
        if ( !book || weakList.get() != list ||
                book->GetListView() != list )
        {
            return;
        }

        list->Focus(book->m_selection);
    }
}

#endif // wxUSE_LISTBOOK
