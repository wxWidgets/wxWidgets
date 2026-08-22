///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/treebkg.cpp
// Purpose:     generic implementation of wxTreebook
// Author:      Evgeniy Tarassov, Vadim Zeitlin
// Created:     2005-09-15
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
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


#if wxUSE_TREEBOOK

#include "wx/treebook.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif

#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/treectrl.h"
#include "wx/weakref.h"

#include <vector>

// ----------------------------------------------------------------------------
// various wxWidgets macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < DoInternalGetPageCount())

namespace
{

// The tree widget, m_treeIds and the common page array form one topology.
// Callbacks from Hide(), tree deletion and page destructors must never publish
// a nested half-mutation while an outer operation owns that transaction.
class wxTreebookTopologyTransaction
{
public:
    explicit wxTreebookTopologyTransaction(wxTreebook* const book)
        : m_book(book),
          m_lifetime(book),
          m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxTreebookTopologyTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(const wxTreebook* const book)
    {
        for ( wxTreebookTopologyTransaction* transaction = GetActive();
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

private:
    static wxTreebookTopologyTransaction*& GetActive()
    {
        static thread_local wxTreebookTopologyTransaction* active = nullptr;
        return active;
    }

    wxTreebook* const m_book;
    const wxWeakRef<wxTreebook> m_lifetime;
    wxTreebookTopologyTransaction* const m_previous;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTreebook, wxBookCtrlBase);

wxDEFINE_EVENT( wxEVT_TREEBOOK_PAGE_CHANGING,  wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_TREEBOOK_PAGE_CHANGED,   wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_TREEBOOK_NODE_COLLAPSED, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_TREEBOOK_NODE_EXPANDED,  wxBookCtrlEvent );

wxBEGIN_EVENT_TABLE(wxTreebook, wxBookCtrlBase)
    EVT_TREE_SEL_CHANGED   (wxID_ANY, wxTreebook::OnTreeSelectionChange)
    EVT_TREE_ITEM_EXPANDED (wxID_ANY, wxTreebook::OnTreeNodeExpandedCollapsed)
    EVT_TREE_ITEM_COLLAPSED(wxID_ANY, wxTreebook::OnTreeNodeExpandedCollapsed)
wxEND_EVENT_TABLE()

// ============================================================================
// wxTreebook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTreebook creation
// ----------------------------------------------------------------------------

bool
wxTreebook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    // Check the style flag to have either wxTBK_RIGHT or wxTBK_LEFT
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
        style |= wxBK_LEFT;
    }
    style |= wxTAB_TRAVERSAL;

    // no border for this control, it doesn't look nice together with the tree
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size,
                            style, wxDefaultValidator, name) )
        return false;

    m_bookctrl = new wxTreeCtrl
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxBORDER_THEME |
                    wxTR_DEFAULT_STYLE |
                    wxTR_HIDE_ROOT |
                    wxTR_SINGLE
                 );
    GetTreeCtrl()->SetQuickBestSize(false); // do full size calculation
    GetTreeCtrl()->AddRoot(wxEmptyString); // label doesn't matter, it's hidden

#ifdef __WXMSW__
    // We need to add dummy size event to force possible scrollbar hiding
    wxSizeEvent evt;
    GetEventHandler()->AddPendingEvent(evt);
#endif

    return true;
}


// insert a new page just before the pagePos
bool wxTreebook::InsertPage(size_t pagePos,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect,
                            int imageId)
{
    return DoInsertPage(pagePos, page, text, bSelect, imageId);
}

bool wxTreebook::InsertSubPage(size_t pagePos,
                               wxWindow *page,
                               const wxString& text,
                               bool bSelect,
                               int imageId)
{
    return DoInsertSubPage(pagePos, page, text, bSelect, imageId);
}

bool wxTreebook::AddPage(wxWindow *page, const wxString& text, bool bSelect,
                         int imageId)
{
    return DoInsertPage(m_treeIds.size(), page, text, bSelect, imageId);
}

// insertion time is linear to the number of top-pages
bool wxTreebook::AddSubPage(wxWindow *page, const wxString& text, bool bSelect, int imageId)
{
    return DoAddSubPage(page, text, bSelect, imageId);
}


bool wxTreebook::DoInsertPage(size_t pagePos,
                              wxWindow *page,
                              const wxString& text,
                              bool bSelect,
                              int imageId)
{
    if ( wxTreebookTopologyTransaction::IsActiveFor(this) ||
            IsDeletingAllPages() )
    {
        return false;
    }

    const wxTreebookTopologyTransaction transaction(this);
    wxCHECK_MSG( pagePos <= DoInternalGetPageCount(), false,
                        wxT("Invalid treebook page position") );

    const wxWeakRef<wxTreebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxTreeCtrl* const tree = GetTreeCtrl();
    const wxWeakRef<wxTreeCtrl> weakTree(tree);
    const size_t controllerCountBefore = m_treeIds.size();
    const int selectionBefore = m_selection;
    wxTreeItemId insertedTreeId;

    const InsertPageResult modelResult =
        DoInsertPageIntoModel(pagePos, page, text, bSelect, imageId);
    if ( modelResult == InsertPageResult::Failed )
        return false;
    if ( modelResult == InsertPageResult::OwnershipConsumed )
        return true;

    const auto finishCommittedInsertion = [&]() -> bool
    {
        wxTreebook* book = weakThis.get();
        if ( book && page &&
                (weakPage.get() != page || page->GetParent() != book) &&
                pagePos < book->wxBookCtrlBase::GetPageCount() &&
                book->wxBookCtrlBase::GetPage(pagePos) == page )
        {
            book->DoErasePageRange(pagePos, 1);
            if ( pagePos < book->m_treeIds.size() &&
                    book->m_treeIds.size() == controllerCountBefore + 1 &&
                    (!insertedTreeId.IsOk() ||
                     book->m_treeIds[pagePos] == insertedTreeId) )
            {
                book->m_treeIds.erase(book->m_treeIds.begin() + pagePos);
            }

            if ( insertedTreeId.IsOk() &&
                    wxWeakWindowIsAvailableForCallbacks(weakTree, tree) &&
                    book->GetTreeCtrl() == tree )
            {
                tree->Delete(insertedTreeId);
            }

            book = weakThis.get();
            if ( book &&
                    (book->m_selection != selectionBefore ||
                     static_cast<int>(pagePos) > selectionBefore) )
            {
                book->DoSetSelectionAfterRemoval(pagePos);
            }
        }

        book = weakThis.get();
        if ( book )
            (void)book->DoReconcilePageVisibility();

        return true;
    };
    const auto getBeforeMappingPublication = [&]() -> wxTreebook*
    {
        wxTreebook* const book = weakThis.get();
        return book && weakPage.get() == page &&
               (!page || page->GetParent() == book) &&
               weakTree.get() == tree &&
               book->GetTreeCtrl() == tree &&
               book->m_treeIds.size() == controllerCountBefore &&
               book->wxBookCtrlBase::GetPageCount() ==
                    controllerCountBefore + 1 &&
               pagePos < book->wxBookCtrlBase::GetPageCount() &&
               book->wxBookCtrlBase::GetPage(pagePos) == page
                    ? book
                    : nullptr;
    };

    wxTreebook* book = getBeforeMappingPublication();
    if ( !book )
        return finishCommittedInsertion();

    wxTreeItemId newId;
    if ( pagePos == controllerCountBefore )
    {
        // append the page to the end
        wxTreeItemId rootId = tree->GetRootItem();

        newId = tree->AppendItem(rootId, text, imageId);
    }
    else // insert the new page before the given one
    {
        wxTreeItemId nodeId = m_treeIds[pagePos];

        wxTreeItemId previousId = tree->GetPrevSibling(nodeId);
        wxTreeItemId parentId = tree->GetItemParent(nodeId);

        if ( previousId.IsOk() )
        {
            // insert before the sibling - previousId
            newId = tree->InsertItem(parentId, previousId, text, imageId);
        }
        else // no prev siblings -- insert as a first child
        {
            wxASSERT_MSG( parentId.IsOk(), wxT( "Tree has no root node?" ) );

            newId = tree->PrependItem(parentId, text, imageId);
        }
    }
    insertedTreeId = newId;

    if ( !newId.IsOk() )
    {
        //something wrong -> cleaning and returning with false
        book = getBeforeMappingPublication();
        bool rolledBack = false;
        if ( book )
        {
            (void)book->wxBookCtrlBase::DoRemovePage(pagePos);
            book = weakThis.get();
            rolledBack = book &&
                         book->wxBookCtrlBase::GetPageCount() ==
                            controllerCountBefore &&
                         book->m_treeIds.size() == controllerCountBefore;
        }

        wxFAIL_MSG( wxT("Failed to insert treebook page") );
        return !rolledBack;
    }

    book = getBeforeMappingPublication();
    if ( !book )
        return finishCommittedInsertion();

    book->DoInternalAddPage(pagePos, page, newId);

    book = weakThis.get();
    if ( !book || weakPage.get() != page ||
            (page && page->GetParent() != book) ||
            weakTree.get() != tree ||
            book->GetTreeCtrl() != tree ||
            book->m_treeIds.size() != controllerCountBefore + 1 ||
            book->wxBookCtrlBase::GetPageCount() !=
                controllerCountBefore + 1 ||
            book->wxBookCtrlBase::GetPage(pagePos) != page ||
            book->m_treeIds[pagePos] != newId )
    {
        return finishCommittedInsertion();
    }

    // Hide only after all three topology models are published. Its EVT_SHOW
    // callback can destroy the book, but same-book topology reentry is
    // rejected by the transaction above.
    if ( page )
    {
        page->Hide();
        book = weakThis.get();
        if ( !book || weakPage.get() != page ||
                (page && page->GetParent() != book) ||
                book->wxBookCtrlBase::FindPage(page) == wxNOT_FOUND )
        {
            return finishCommittedInsertion();
        }
    }

    book->DoUpdateSelection(bSelect, pagePos);

    return finishCommittedInsertion();
}

bool wxTreebook::DoAddSubPage(wxWindow *page, const wxString& text, bool bSelect, int imageId)
{
    wxTreeCtrl *tree = GetTreeCtrl();

    wxTreeItemId rootId = tree->GetRootItem();

    wxTreeItemId lastNodeId = tree->GetLastChild(rootId);

    wxCHECK_MSG( lastNodeId.IsOk(), false,
                        wxT("Can't insert sub page when there are no pages") );

    // now calculate its position (should we save/update it too?)
    size_t newPos = tree->GetCount() -
                        (tree->GetChildrenCount(lastNodeId, true) + 1);

    return DoInsertSubPage(newPos, page, text, bSelect, imageId);
}

bool wxTreebook::DoInsertSubPage(size_t pagePos,
                                 wxTreebookPage *page,
                                 const wxString& text,
                                 bool bSelect,
                                 int imageId)
{
    if ( wxTreebookTopologyTransaction::IsActiveFor(this) ||
            IsDeletingAllPages() )
    {
        return false;
    }

    const wxTreebookTopologyTransaction transaction(this);
    wxTreeItemId parentId = DoInternalGetPage(pagePos);
    wxCHECK_MSG( parentId.IsOk(), false, wxT("invalid tree item") );

    const wxWeakRef<wxTreebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxTreeCtrl* const tree = GetTreeCtrl();
    const wxWeakRef<wxTreeCtrl> weakTree(tree);
    const size_t controllerCountBefore = m_treeIds.size();
    const int selectionBefore = m_selection;
    wxTreeItemId insertedTreeId;

    size_t newPos = pagePos + tree->GetChildrenCount(parentId, true) + 1;
    wxASSERT_MSG( newPos <= DoInternalGetPageCount(),
                    wxT("Internal error in tree insert point calculation") );

    const InsertPageResult modelResult =
        DoInsertPageIntoModel(newPos, page, text, bSelect, imageId);
    if ( modelResult == InsertPageResult::Failed )
        return false;
    if ( modelResult == InsertPageResult::OwnershipConsumed )
        return true;

    const auto finishCommittedInsertion = [&]() -> bool
    {
        wxTreebook* book = weakThis.get();
        if ( book && page &&
                (weakPage.get() != page || page->GetParent() != book) &&
                newPos < book->wxBookCtrlBase::GetPageCount() &&
                book->wxBookCtrlBase::GetPage(newPos) == page )
        {
            book->DoErasePageRange(newPos, 1);
            if ( newPos < book->m_treeIds.size() &&
                    book->m_treeIds.size() == controllerCountBefore + 1 &&
                    (!insertedTreeId.IsOk() ||
                     book->m_treeIds[newPos] == insertedTreeId) )
            {
                book->m_treeIds.erase(book->m_treeIds.begin() + newPos);
            }

            if ( insertedTreeId.IsOk() &&
                    wxWeakWindowIsAvailableForCallbacks(weakTree, tree) &&
                    book->GetTreeCtrl() == tree )
            {
                tree->Delete(insertedTreeId);
            }

            book = weakThis.get();
            if ( book &&
                    (book->m_selection != selectionBefore ||
                     static_cast<int>(newPos) > selectionBefore) )
            {
                book->DoSetSelectionAfterRemoval(newPos);
            }
        }

        book = weakThis.get();
        if ( book )
            (void)book->DoReconcilePageVisibility();

        return true;
    };
    const auto getBeforeMappingPublication = [&]() -> wxTreebook*
    {
        wxTreebook* const book = weakThis.get();
        return book && weakPage.get() == page &&
               (!page || page->GetParent() == book) &&
               weakTree.get() == tree &&
               book->GetTreeCtrl() == tree &&
               book->m_treeIds.size() == controllerCountBefore &&
               book->wxBookCtrlBase::GetPageCount() ==
                    controllerCountBefore + 1 &&
               newPos < book->wxBookCtrlBase::GetPageCount() &&
               book->wxBookCtrlBase::GetPage(newPos) == page
                    ? book
                    : nullptr;
    };

    wxTreebook* book = getBeforeMappingPublication();
    if ( !book )
        return finishCommittedInsertion();

    wxTreeItemId newId = tree->AppendItem(parentId, text, imageId);
    insertedTreeId = newId;

    if ( !newId.IsOk() )
    {
        book = getBeforeMappingPublication();
        bool rolledBack = false;
        if ( book )
        {
            (void)book->wxBookCtrlBase::DoRemovePage(newPos);
            book = weakThis.get();
            rolledBack = book &&
                         book->wxBookCtrlBase::GetPageCount() ==
                            controllerCountBefore &&
                         book->m_treeIds.size() == controllerCountBefore;
        }

        wxFAIL_MSG( wxT("Failed to insert treebook page") );
        return !rolledBack;
    }

    book = getBeforeMappingPublication();
    if ( !book )
        return finishCommittedInsertion();

    book->DoInternalAddPage(newPos, page, newId);

    book = weakThis.get();
    if ( !book || weakPage.get() != page ||
            (page && page->GetParent() != book) ||
            weakTree.get() != tree ||
            book->GetTreeCtrl() != tree ||
            book->m_treeIds.size() != controllerCountBefore + 1 ||
            book->wxBookCtrlBase::GetPageCount() !=
                controllerCountBefore + 1 ||
            book->wxBookCtrlBase::GetPage(newPos) != page ||
            book->m_treeIds[newPos] != newId )
    {
        return finishCommittedInsertion();
    }

    if ( page )
    {
        page->Hide();
        book = weakThis.get();
        if ( !book || weakPage.get() != page ||
                (page && page->GetParent() != book) ||
                book->wxBookCtrlBase::FindPage(page) == wxNOT_FOUND )
        {
            return finishCommittedInsertion();
        }
    }

    book->DoUpdateSelection(bSelect, newPos);

    return finishCommittedInsertion();
}

bool wxTreebook::DeletePage(size_t pagePos)
{
    wxCHECK_MSG( IS_VALID_PAGE(pagePos), false,
                 wxT("Invalid tree index") );
    const wxWeakRef<wxTreebook> weakThis(this);
    wxTreebookPage* const expectedPage =
        wxBookCtrlBase::GetPage(pagePos);
    const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);
    const size_t countBefore = wxBookCtrlBase::GetPageCount();

    wxTreebookPage* oldPage = nullptr;
    if ( !DoRemovePageAndReport(pagePos, &oldPage) )
    {
        wxTreebook* const book = weakThis.get();
        return book &&
               book->wxBookCtrlBase::GetPageCount() < countBefore &&
               expectedPage && weakExpectedPage.get() != expectedPage;
    }

    if ( oldPage )
    {
        if ( weakExpectedPage.get() != oldPage ||
                oldPage->GetParent() != this ||
                wxBookCtrlBase::FindPage(oldPage) != wxNOT_FOUND )
        {
            return false;
        }
        delete oldPage;
    }

    return true;
}

bool wxTreebook::RemovePage(size_t pagePos)
{
    const wxWeakRef<wxTreebook> weakThis(this);
    wxTreebookPage* oldPage = nullptr;
    if ( !DoRemovePageAndReport(pagePos, &oldPage) )
        return false;

    if ( !oldPage )
        return weakThis.get() == this;

    const wxWeakRef<wxWindow> weakPage(oldPage);
    const bool wasShown = oldPage->IsShown();
    wxTreebook* book = weakThis.get();
    if ( !book || weakPage.get() != oldPage ||
            book->wxBookCtrlBase::FindPage(oldPage) != wxNOT_FOUND ||
            oldPage->GetParent() != book )
    {
        return false;
    }

    if ( wasShown )
        oldPage->Hide();

    book = weakThis.get();
    return book && weakPage.get() == oldPage &&
           book->wxBookCtrlBase::FindPage(oldPage) == wxNOT_FOUND &&
           oldPage->GetParent() == book;
}

bool wxTreebook::DoRemovePageAndReport(
    size_t pagePos,
    wxTreebookPage** const removedPage)
{
    wxCHECK_MSG( removedPage, false, wxT("null removed-page output") );
    *removedPage = nullptr;
    wxCHECK_MSG( IS_VALID_PAGE(pagePos), false, wxT("Invalid tree index") );

    wxTreeCtrl* const tree = GetTreeCtrl();
    const wxTreeItemId pageId = DoInternalGetPage(pagePos);
    wxCHECK_MSG( pageId.IsOk(), false, wxT("Invalid tree item") );

    const size_t countBefore = wxBookCtrlBase::GetPageCount();
    const size_t removedCount = tree->GetChildrenCount(pageId, true) + 1;
    const wxWeakRef<wxTreebook> weakThis(this);

    std::vector<wxTreebookPage*> pagesBefore;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pagesBefore.reserve(countBefore);
    pageLifetimes.reserve(countBefore);
    for ( size_t i = 0; i < countBefore; ++i )
    {
        wxTreebookPage* const page =
            wxBookCtrlBase::GetPage(i);
        pagesBefore.push_back(page);
        pageLifetimes.emplace_back(page);
    }
    const wxVector<wxTreeItemId> idsBefore = m_treeIds;
    wxTreebookPage* const expectedPage = pagesBefore[pagePos];

    wxTreebookPage* const returnedPage = DoRemovePage(pagePos);
    wxTreebook* const book = weakThis.get();
    if ( !book ||
            returnedPage != expectedPage ||
            (expectedPage &&
                pageLifetimes[pagePos].get() != expectedPage) ||
            book->wxBookCtrlBase::GetPageCount() !=
                countBefore - removedCount ||
            book->m_treeIds.size() != countBefore - removedCount )
    {
        return false;
    }

    size_t current = 0;
    for ( size_t old = 0; old < countBefore; ++old )
    {
        if ( old >= pagePos && old < pagePos + removedCount )
            continue;

        if ( pageLifetimes[old].get() != pagesBefore[old] ||
                book->wxBookCtrlBase::GetPage(current) != pagesBefore[old] ||
                book->m_treeIds[current] != idsBefore[old] )
        {
            return false;
        }
        ++current;
    }

    if ( expectedPage &&
            book->wxBookCtrlBase::FindPage(expectedPage) != wxNOT_FOUND )
    {
        return false;
    }

    *removedPage = expectedPage;
    return true;
}

wxTreebookPage *wxTreebook::DoRemovePage(size_t pagePos)
{
    if ( wxTreebookTopologyTransaction::IsActiveFor(this) ||
         (IsDeletingAllPages() &&
            !IsPerformingDeleteAllPageRemoval()) )
    {
        return nullptr;
    }

    const wxTreebookTopologyTransaction transaction(this);
    wxTreeItemId pageId = DoInternalGetPage(pagePos);
    wxCHECK_MSG( pageId.IsOk(), nullptr, wxT("Invalid tree index") );

    const wxWeakRef<wxTreebook> weakThis(this);
    wxTreebookPage* const oldPage =
        wxBookCtrlBase::GetPage(pagePos);
    wxTreeCtrl* const tree = GetTreeCtrl();
    const wxWeakRef<wxTreeCtrl> weakTree(tree);

    const size_t subCount = tree->GetChildrenCount(pageId, true);
    const size_t removedCount = subCount + 1;
    wxASSERT_MSG ( IS_VALID_PAGE(pagePos + subCount),
                        wxT("Internal error in wxTreebook::DoRemovePage") );

    std::vector<wxTreebookPage*> removedPages;
    std::vector<wxWeakRef<wxWindow>> removedLifetimes;
    removedPages.reserve(removedCount);
    removedLifetimes.reserve(removedCount);
    for ( size_t i = 0; i < removedCount; ++i )
    {
        wxTreebookPage* const page =
            wxBookCtrlBase::GetPage(pagePos + i);
        removedPages.push_back(page);
        removedLifetimes.emplace_back(page);
    }
    const auto getTransferredRoot = [&]() -> wxTreebookPage*
    {
        wxTreebook* const current = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !oldPage ||
                removedLifetimes[0].get() != oldPage ||
                wxWindowIsUnavailableForCallbacks(oldPage) ||
                oldPage->GetParent() != current )
        {
            return nullptr;
        }

        return current->wxBookCtrlBase::FindPage(oldPage) == wxNOT_FOUND
                    ? oldPage
                    : nullptr;
    };
    size_t nextDescendantToDestroy = 1;
    const auto destroyRemainingDescendants = [&]() -> bool
    {
        while ( nextDescendantToDestroy < removedCount )
        {
            const size_t i = nextDescendantToDestroy++;
            wxTreebookPage* const page = removedPages[i];
            if ( page &&
                    wxWeakWindowIsAvailableForCallbacks(removedLifetimes[i],
                                                        page) )
            {
                wxTreebook* const current = weakThis.get();
                if ( !current )
                    return false;

                if ( page->GetParent() == current &&
                        current->wxBookCtrlBase::FindPage(page) ==
                            wxNOT_FOUND )
                {
                    delete page;
                }
            }

            // Destroying any descendant may destroy the entire book, in which
            // case its child hierarchy owns the remaining cleanup.
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;
        }

        return true;
    };
    const auto finishCommittedRemoval = [&]() -> wxTreebookPage*
    {
        // Descendants are implementation-owned by this subtree transaction,
        // including during DeleteAllPages(). Destroy them immediately after
        // the atomic model erase so a later hostile public accessor in the
        // common bulk loop cannot strand them outside every projection.
        (void)destroyRemainingDescendants();
        return getTransferredRoot();
    };

    const int selectionBefore = m_selection;
    const bool selectionRemoved =
        selectionBefore != wxNOT_FOUND &&
        static_cast<size_t>(selectionBefore) >= pagePos &&
        static_cast<size_t>(selectionBefore) < pagePos + removedCount;

    const wxTreeItemId nextId = tree->GetNextSibling(pageId);
    const wxTreeItemId parentId = tree->GetItemParent(pageId);
    const wxTreeItemId rootId = tree->GetRootItem();

    // Publish both index models without crossing a layout/event boundary.
    // Calling the common single-page remover repeatedly would invalidate best
    // size after every erase and expose a partially removed subtree.
    DoErasePageRange(pagePos, removedCount);

    wxVector<wxTreeItemId>::iterator const itPos =
        m_treeIds.begin() + pagePos;
    m_treeIds.erase(itPos, itPos + removedCount);

    if ( selectionBefore != wxNOT_FOUND )
    {
        if ( static_cast<size_t>(selectionBefore) >=
                pagePos + removedCount )
        {
            m_selection -= static_cast<int>(removedCount);
        }
        else if ( selectionRemoved )
        {
            m_selection = wxNOT_FOUND;
        }
    }

    DoInvalidateBestSize();
    wxTreebook* book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakTree, tree) ||
            book->GetTreeCtrl() != tree )
    {
        return finishCommittedRemoval();
    }

    tree->DeleteChildren(pageId);
    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakTree, tree) ||
            book->GetTreeCtrl() != tree )
    {
        return finishCommittedRemoval();
    }

    tree->Delete(pageId);
    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakTree, tree) ||
            book->GetTreeCtrl() != tree )
    {
        return finishCommittedRemoval();
    }

    // Ownership of the root page is transferred to the caller. Descendant
    // pages are an implementation detail of the subtree removal and are
    // destroyed here, with weak guards after every destructor callback.
    if ( !destroyRemainingDescendants() )
        return getTransferredRoot();

    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakTree, tree) ||
            book->GetTreeCtrl() != tree )
    {
        return getTransferredRoot();
    }

    // Tree deletion can select a surviving item itself. Honour that newer
    // selection; otherwise preserve the documented next-sibling/parent
    // replacement policy.
    if ( selectionRemoved && book->m_selection == wxNOT_FOUND )
    {
        wxTreeItemId replacement;
        if ( nextId.IsOk() &&
                book->DoInternalFindPageById(nextId) != wxNOT_FOUND )
        {
            replacement = nextId;
        }
        else if ( parentId.IsOk() && parentId != rootId &&
                    book->DoInternalFindPageById(parentId) != wxNOT_FOUND )
        {
            replacement = parentId;
        }

        if ( replacement.IsOk() )
            tree->SelectItem(replacement);
        else
            book->DoUpdateSelection(false, wxNOT_FOUND);

        book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return getTransferredRoot();
    }

    return getTransferredRoot();
}

bool wxTreebook::DeleteAllPages()
{
    if ( wxTreebookTopologyTransaction::IsActiveFor(this) ||
            IsDeletingAllPages() )
    {
        return false;
    }

    return wxBookCtrlBase::DeleteAllPages();
}

void wxTreebook::DoInternalAddPage(size_t newPos,
                                   wxTreebookPage *WXUNUSED(page),
                                   wxTreeItemId pageId)
{
    wxASSERT_MSG( newPos <= m_treeIds.size(),
                  wxT("Invalid index passed to wxTreebook::DoInternalAddPage") );

    if ( newPos == m_treeIds.size() )
    {
        // append
        m_treeIds.push_back(pageId);
    }
    else // insert
    {
        m_treeIds.insert(m_treeIds.begin() + newPos, pageId);

        if ( m_selection != wxNOT_FOUND && newPos <= (size_t)m_selection )
        {
            // selection has been moved one unit toward the end
            ++m_selection;
        }
    }
}

void wxTreebook::DoInternalRemovePageRange(size_t pagePos, size_t subCount)
{
    // Attention: this function is only for a situation when we delete a node
    // with all its children so pagePos is the node's index and subCount is the
    // node children count
    wxASSERT_MSG( pagePos + subCount < m_treeIds.size(),
                    wxT("Invalid page index") );

    wxTreeItemId pageId = m_treeIds[pagePos];

    wxVector<wxTreeItemId>::iterator itPos = m_treeIds.begin() + pagePos;
    m_treeIds.erase(itPos, itPos + subCount + 1);

    if ( m_selection != wxNOT_FOUND )
    {
        if ( (size_t)m_selection > pagePos + subCount)
        {
            // selection is far after the deleted page, so just update the index and move on
            m_selection -= 1 + subCount;
        }
        else if ( (size_t)m_selection >= pagePos )
        {
            wxTreeCtrl *tree = GetTreeCtrl();

            // as selected page is going to be deleted, try to select the next
            // sibling if exists, if not then the parent
            wxTreeItemId nodeId = tree->GetNextSibling(pageId);

            m_selection = wxNOT_FOUND;

            if ( nodeId.IsOk() )
            {
                // selecting next siblings
                tree->SelectItem(nodeId);
            }
            else // no next sibling, select the parent
            {
                wxTreeItemId parentId = tree->GetItemParent(pageId);

                if ( parentId.IsOk() && parentId != tree->GetRootItem() )
                {
                    tree->SelectItem(parentId);
                }
                else // parent is root
                {
                    // we can't select it as it's hidden
                    DoUpdateSelection(false, wxNOT_FOUND);
                }
            }
        }
        //else: nothing to do -- selection is before the deleted node
    }
    else
    {
        DoUpdateSelection(false, wxNOT_FOUND);
    }
}


void wxTreebook::DoUpdateSelection(bool bSelect, int newPos)
{
    int newSelPos;
    if ( bSelect )
    {
        newSelPos = newPos;
    }
    else if ( m_selection == wxNOT_FOUND && DoInternalGetPageCount() > 0 )
    {
        newSelPos = 0;
    }
    else
    {
        newSelPos = wxNOT_FOUND;
    }

    if ( newSelPos != wxNOT_FOUND )
    {
        SetSelection((size_t)newSelPos);
    }
}

wxTreeItemId wxTreebook::DoInternalGetPage(size_t pagePos) const
{
    if ( pagePos >= m_treeIds.size() )
    {
        // invalid position but ok here, in this internal function, don't assert
        // (the caller will do it)
        return wxTreeItemId();
    }

    return m_treeIds[pagePos];
}

int wxTreebook::DoInternalFindPageById(wxTreeItemId pageId) const
{
    const size_t count = m_treeIds.size();
    for ( size_t i = 0; i < count; ++i )
    {
        if ( m_treeIds[i] == pageId )
            return i;
    }

    return wxNOT_FOUND;
}

bool wxTreebook::IsNodeExpanded(size_t pagePos) const
{
    wxTreeItemId pageId = DoInternalGetPage(pagePos);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    return GetTreeCtrl()->IsExpanded(pageId);
}

bool wxTreebook::ExpandNode(size_t pagePos, bool expand)
{
    wxTreeItemId pageId = DoInternalGetPage(pagePos);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    if ( expand )
    {
        GetTreeCtrl()->Expand( pageId );
    }
    else // collapse
    {
        GetTreeCtrl()->Collapse( pageId );

        // rely on the events generated by wxTreeCtrl to update selection
    }

    return true;
}

int wxTreebook::GetPageParent(size_t pagePos) const
{
    wxTreeItemId nodeId = DoInternalGetPage( pagePos );
    wxCHECK_MSG( nodeId.IsOk(), wxNOT_FOUND, wxT("Invalid page index spacified!") );

    const wxTreeItemId parent = GetTreeCtrl()->GetItemParent( nodeId );

    return parent.IsOk() ? DoInternalFindPageById(parent) : wxNOT_FOUND;
}

bool wxTreebook::SetPageText(size_t n, const wxString& strText)
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    GetTreeCtrl()->SetItemText(pageId, strText);

    return true;
}

wxString wxTreebook::GetPageText(size_t n) const
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), wxString(), wxT("invalid tree item") );

    return GetTreeCtrl()->GetItemText(pageId);
}

int wxTreebook::GetPageImage(size_t n) const
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), wxNOT_FOUND, wxT("invalid tree item") );

    return GetTreeCtrl()->GetItemImage(pageId);
}

bool wxTreebook::SetPageImage(size_t n, int imageId)
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    GetTreeCtrl()->SetItemImage(pageId, imageId);

    return true;
}

void wxTreebook::UpdateSelectedPage(size_t newsel)
{
    GetTreeCtrl()->SelectItem(DoInternalGetPage(newsel));
}

wxBookCtrlEvent* wxTreebook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_TREEBOOK_PAGE_CHANGING, m_windowId);
}

void wxTreebook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_TREEBOOK_PAGE_CHANGED);
}

wxWindow *wxTreebook::TryGetNonNullPage(size_t n)
{
    wxWindow* page = wxBookCtrlBase::GetPage(n);

    if ( !page )
    {
        // Find the next suitable page, i.e. the first (grand)child
        // of this one with a non-null associated page
        wxTreeCtrl* const tree = GetTreeCtrl();
        for ( wxTreeItemId childId = m_treeIds[n]; childId.IsOk(); )
        {
            wxTreeItemIdValue cookie;
            childId = tree->GetFirstChild( childId, cookie );
            if ( childId.IsOk() )
            {
                page = wxBookCtrlBase::GetPage(++n);
                if ( page )
                    break;
            }
        }
    }

    return page;
}

void wxTreebook::OnImagesChanged()
{
    // Propagate the images to the tree control which will actually use them.
    wxTreeCtrl* const tree = GetTreeCtrl();
    const Images& images = GetImages();
    if ( !images.empty() )
        tree->SetImages(images);
    else
        tree->SetImageList(GetImageList());
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxTreebook::OnTreeSelectionChange(wxTreeEvent& event)
{
    if ( event.GetEventObject() != m_bookctrl )
    {
        event.Skip();
        return;
    }

    wxTreeItemId newId = event.GetItem();

    if ( (m_selection == wxNOT_FOUND &&
                (!newId.IsOk() || newId == GetTreeCtrl()->GetRootItem())) ||
            (m_selection != wxNOT_FOUND && newId == m_treeIds[m_selection]) )
    {
        // this event can only come when we modify the tree selection ourselves
        // so we should simply ignore it
        return;
    }

    int newPos = DoInternalFindPageById(newId);

    if ( newPos != wxNOT_FOUND )
    {
        const wxWeakRef<wxTreebook> weakThis(this);
        wxTreeCtrl* const tree = GetTreeCtrl();
        const wxWeakRef<wxTreeCtrl> weakTree(tree);

        SetSelection(newPos);

        wxTreebook* const book = weakThis.get();
        if ( !book || weakTree.get() != tree ||
                book->GetTreeCtrl() != tree ||
                book->m_selection == newPos )
        {
            return;
        }

        // The controller changes selection before this notification. If the
        // wx page-changing event vetoes (or a newer nested writer wins), put
        // the tree peer back on the actually committed model selection.
        if ( book->m_selection != wxNOT_FOUND &&
                static_cast<size_t>(book->m_selection) <
                    book->m_treeIds.size() )
        {
            tree->SelectItem(book->m_treeIds[book->m_selection]);
        }
        else
        {
            tree->UnselectAll();
        }
    }
}

void wxTreebook::OnTreeNodeExpandedCollapsed(wxTreeEvent & event)
{
    if ( event.GetEventObject() != m_bookctrl )
    {
        event.Skip();
        return;
    }

    wxTreeItemId nodeId = event.GetItem();
    if ( !nodeId.IsOk() || nodeId == GetTreeCtrl()->GetRootItem() )
        return;
    int pagePos = DoInternalFindPageById(nodeId);
    wxCHECK_RET( pagePos != wxNOT_FOUND, wxT("Internal problem in wxTreebook!..") );

    wxBookCtrlEvent ev(GetTreeCtrl()->IsExpanded(nodeId)
            ? wxEVT_TREEBOOK_NODE_EXPANDED
            : wxEVT_TREEBOOK_NODE_COLLAPSED,
        m_windowId);

    ev.SetSelection(pagePos);
    ev.SetOldSelection(pagePos);
    ev.SetEventObject(this);

    GetEventHandler()->ProcessEvent(ev);
}

// ----------------------------------------------------------------------------
// wxTreebook geometry management
// ----------------------------------------------------------------------------

int wxTreebook::HitTest(wxPoint const & pt, long * flags) const
{
    int pagePos = wxNOT_FOUND;

    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    // convert from wxTreebook coordinates to wxTreeCtrl ones
    const wxTreeCtrl * const tree = GetTreeCtrl();
    const wxPoint treePt = tree->ScreenToClient(ClientToScreen(pt));

    // is it over the tree?
    if ( wxRect(tree->GetSize()).Contains(treePt) )
    {
        int flagsTree;
        wxTreeItemId id = tree->HitTest(treePt, flagsTree);

        if ( id.IsOk() && (flagsTree & wxTREE_HITTEST_ONITEM) )
        {
            pagePos = DoInternalFindPageById(id);
        }

        if ( flags )
        {
            if ( pagePos != wxNOT_FOUND )
                *flags = 0;

            if ( flagsTree & (wxTREE_HITTEST_ONITEMBUTTON |
                              wxTREE_HITTEST_ONITEMICON |
                              wxTREE_HITTEST_ONITEMSTATEICON) )
                *flags |= wxBK_HITTEST_ONICON;

            if ( flagsTree & wxTREE_HITTEST_ONITEMLABEL )
                *flags |= wxBK_HITTEST_ONLABEL;
        }
    }
    else // not over the tree
    {
        if ( flags && GetPageRect().Contains( pt ) )
            *flags |= wxBK_HITTEST_ONPAGE;
    }

    return pagePos;
}

#endif // wxUSE_TREEBOOK
