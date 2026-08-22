///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/toolbkg.cpp
// Purpose:     generic implementation of wxToolbook
// Author:      Julian Smart
// Created:     2006-01-29
// Copyright:   (c) 2006 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TOOLBOOK

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/settings.h"
    #include "wx/toolbar.h"
#endif

#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/sysopt.h"
#include "wx/toolbook.h"
#include "wx/weakref.h"

#include <vector>

#if defined(__WXMAC__) && wxUSE_TOOLBAR && wxUSE_BMPBUTTON
#include "wx/generic/buttonbar.h"
#endif

namespace
{

// Rebuilding the WinUI toolbar crosses synchronous Loaded/layout callbacks.
// Keep each Toolbook topology mutation authoritative across that boundary:
// same-book nested writers fail normally and may be retried afterwards,
// instead of publishing only the page model or only its toolbar controller.
class wxToolbookTopologyTransaction
{
public:
    explicit wxToolbookTopologyTransaction(wxToolbook* const book)
        : m_book(book),
          m_lifetime(book),
          m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxToolbookTopologyTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(wxToolbook* const book)
    {
        return FindActive(book) != nullptr;
    }

    static bool IsControllerPublicationFor(wxToolbook* const book)
    {
        wxToolbookTopologyTransaction* const transaction =
            FindActive(book);
        return transaction && transaction->m_controllerPublication;
    }

    static void BeginControllerPublication(wxToolbook* const book)
    {
        wxToolbookTopologyTransaction* const transaction =
            FindActive(book);
        wxASSERT(transaction);
        if ( transaction )
        {
            wxASSERT(!transaction->m_controllerPublication);
            transaction->m_controllerPublication = true;
            transaction->m_hasDeferredSelection = false;
        }
    }

    static bool DeferSelection(wxToolbook* const book,
                               size_t selection,
                               int flags)
    {
        wxToolbookTopologyTransaction* const transaction =
            FindActive(book);
        if ( !transaction || !transaction->m_controllerPublication )
            return false;

        // The XAML candidate is already visible to Loaded callbacks here, but
        // its wrapper is not inserted into m_tools until DoInsertTool()
        // returns. Defer selection so PAGE_CHANGED can never expose the two
        // positional models in different states.
        transaction->m_hasDeferredSelection = true;
        transaction->m_deferredSelection = selection;
        transaction->m_deferredSelectionFlags = flags;
        return true;
    }

    static bool EndControllerPublication(wxToolbook* const book,
                                         bool commit,
                                         size_t *selection,
                                         int *flags)
    {
        wxToolbookTopologyTransaction* const transaction =
            FindActive(book);
        if ( !transaction || !transaction->m_controllerPublication )
            return false;

        transaction->m_controllerPublication = false;
        const bool hasDeferred =
            commit && transaction->m_hasDeferredSelection;
        if ( hasDeferred )
        {
            if ( selection )
                *selection = transaction->m_deferredSelection;
            if ( flags )
                *flags = transaction->m_deferredSelectionFlags;
        }
        transaction->m_hasDeferredSelection = false;
        return hasDeferred;
    }

private:
    static wxToolbookTopologyTransaction* FindActive(
        wxToolbook* const book)
    {
        for ( wxToolbookTopologyTransaction* transaction = GetActive();
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book )
            {
                return transaction;
            }
        }

        return nullptr;
    }

    static wxToolbookTopologyTransaction*& GetActive()
    {
        static thread_local wxToolbookTopologyTransaction* active = nullptr;
        return active;
    }

    wxToolbook* const m_book;
    const wxWeakRef<wxToolbook> m_lifetime;
    wxToolbookTopologyTransaction* const m_previous;
    bool m_controllerPublication = false;
    bool m_hasDeferredSelection = false;
    size_t m_deferredSelection = 0;
    int m_deferredSelectionFlags = 0;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxToolbook, wxBookCtrlBase);

wxDEFINE_EVENT( wxEVT_TOOLBOOK_PAGE_CHANGING, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_TOOLBOOK_PAGE_CHANGED,  wxBookCtrlEvent );

wxBEGIN_EVENT_TABLE(wxToolbook, wxBookCtrlBase)
    EVT_SIZE(wxToolbook::OnSize)
    EVT_TOOL(wxID_ANY, wxToolbook::OnToolSelected)
    EVT_IDLE(wxToolbook::OnIdle)
wxEND_EVENT_TABLE()

// ============================================================================
// wxToolbook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolbook creation
// ----------------------------------------------------------------------------

void wxToolbook::Init()
{
    m_needsRealizing = false;
}

bool wxToolbook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    // no border for this control
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    int tbFlags = wxTB_TEXT | wxTB_FLAT | wxBORDER_NONE;
    if ( (style & (wxBK_LEFT | wxBK_RIGHT)) != 0 )
        tbFlags |= wxTB_VERTICAL;
    else
        tbFlags |= wxTB_HORIZONTAL;

    if ( style & wxTBK_HORZ_LAYOUT )
        tbFlags |= wxTB_HORZ_LAYOUT;

    // TODO: make more configurable

#if defined(__WXMAC__) && wxUSE_TOOLBAR && wxUSE_BMPBUTTON
    if (style & wxTBK_BUTTONBAR)
    {
        m_bookctrl = new wxButtonToolBar
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    tbFlags
                 );
    }
    else
#endif
    {
        m_bookctrl = new wxToolBar
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    tbFlags | wxTB_NODIVIDER
                 );
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxToolbook geometry management
// ----------------------------------------------------------------------------

void wxToolbook::OnSize(wxSizeEvent& event)
{
    const wxWeakRef<wxToolbook> weakThis(this);
    if (m_needsRealizing)
        Realize();

    if ( !weakThis )
        return;

    wxBookCtrlBase::OnSize(event);
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxToolbook::SetPageText(size_t n, const wxString& strText)
{
    if ( n >= wxBookCtrlBase::GetPageCount() ||
         n >= GetToolBar()->GetToolsCount() ||
         wxToolbookTopologyTransaction::
             IsControllerPublicationFor(this) )
    {
        return false;
    }

    wxToolBarToolBase* const tool =
        GetToolBar()->GetToolByPos(static_cast<int>(n));
    if (tool)
    {
        tool->SetLabel(strText);
        return true;
    }
    else
        return false;
}

int wxToolbook::SetSelection(size_t n)
{
    wxCHECK_MSG( n < GetPageCount(), m_selection,
                 wxT("invalid page index in wxToolbook::SetSelection()") );
    if ( wxToolbookTopologyTransaction::DeferSelection(
             this, n, SetSelection_SendEvent) )
    {
        return m_selection;
    }
    return DoSetSelection(n, SetSelection_SendEvent);
}

int wxToolbook::ChangeSelection(size_t n)
{
    wxCHECK_MSG( n < GetPageCount(), m_selection,
                 wxT("invalid page index in wxToolbook::ChangeSelection()") );
    if ( wxToolbookTopologyTransaction::DeferSelection(this, n, 0) )
        return m_selection;
    return DoSetSelection(n);
}

wxString wxToolbook::GetPageText(size_t n) const
{
    wxToolBarBase* const toolbar = GetToolBar();
    if ( n >= wxBookCtrlBase::GetPageCount() ||
         n >= toolbar->GetToolsCount() )
    {
        return wxEmptyString;
    }

    wxToolBarToolBase* const tool =
        toolbar->GetToolByPos(static_cast<int>(n));
    if (tool)
        return tool->GetLabel();
    else
        return wxEmptyString;
}

int wxToolbook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG( wxT("wxToolbook::GetPageImage() not implemented") );

    return wxNOT_FOUND;
}

bool wxToolbook::SetPageImage(size_t n, int imageId)
{
    if ( n >= wxBookCtrlBase::GetPageCount() ||
         n >= GetToolBar()->GetToolsCount() )
    {
        return false;
    }

    wxBitmapBundle bmp = GetBitmapBundle(imageId);
    if ( !bmp.IsOk() )
        return false;

    GetToolBar()->DoSetToolNormalBitmapByPos(n, bmp);

    return true;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

wxBookCtrlEvent* wxToolbook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_TOOLBOOK_PAGE_CHANGING, m_windowId);
}

void wxToolbook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_TOOLBOOK_PAGE_CHANGED);
}

void wxToolbook::UpdateSelectedPage(size_t newsel)
{
    GetToolBar()->DoToggleToolByPos(newsel, true);
}

// Not part of the wxBookctrl API, but must be called in OnIdle or
// by application to realize the toolbar and select the initial page.
void wxToolbook::Realize()
{
    const wxWeakRef<wxToolbook> weakThis(this);
    wxToolBarBase* const toolbar = GetToolBar();
    const wxWeakRef<wxToolBarBase> weakToolbar(toolbar);
    const size_t pageCount = wxBookCtrlBase::GetPageCount();
    if ( weakThis.get() != this )
        return;

    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pages.reserve(pageCount);
    pageLifetimes.reserve(pageCount);
    for ( size_t i = 0; i < pageCount; ++i )
    {
        wxToolbook* book = weakThis.get();
        if ( !book ||
                book->wxBookCtrlBase::GetPageCount() != pageCount )
            return;

        wxWindow* const page = book->wxBookCtrlBase::GetPage(i);
        book = weakThis.get();
        if ( !book ||
                book->wxBookCtrlBase::GetPageCount() != pageCount )
            return;

        pages.push_back(page);
        pageLifetimes.emplace_back(page);
    }
    const auto isCurrent = [&]() -> bool
    {
        wxToolbook* book = weakThis.get();
        if ( !book ||
                weakToolbar.get() != toolbar ||
                book->GetToolBar() != toolbar ||
                book->wxBookCtrlBase::GetPageCount() != pages.size() )
        {
            return false;
        }

        for ( size_t i = 0; i < pages.size(); ++i )
        {
            if ( pageLifetimes[i].get() != pages[i] )
            {
                return false;
            }

            wxWindow* const currentPage =
                book->wxBookCtrlBase::GetPage(i);
            book = weakThis.get();
            if ( !book || currentPage != pages[i] )
                return false;
        }

        return true;
    };

    if (m_needsRealizing)
    {
        m_needsRealizing = false;

        if ( !toolbar->Realize() )
        {
            // Keep the retry armed: an idle pass may succeed after the
            // controller/backend recovers.
            if ( wxToolbook* const book = weakThis.get() )
            {
                if ( weakToolbar.get() == toolbar &&
                        book->GetToolBar() == toolbar )
                {
                    book->m_needsRealizing = true;
                }
            }
            return;
        }
        if ( !isCurrent() )
            return;
    }

    if (wxBookCtrlBase::GetPageCount() > 0)
    {
        if (m_selection == wxNOT_FOUND)
            m_selection = 0;

        int sel = m_selection;
        m_selection = wxNOT_FOUND;

        SetSelection(sel);
        if ( !isCurrent() )
            return;
    }

    if ( isCurrent() )
        DoSize();
}

int wxToolbook::HitTest(const wxPoint& pt, long *flags) const
{
    int pagePos = wxNOT_FOUND;

    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    // convert from wxToolbook coordinates to wxToolBar ones
    const wxToolBarBase * const tbar = GetToolBar();
    const wxPoint tbarPt = tbar->ScreenToClient(ClientToScreen(pt));

    // is the point over the toolbar?
    if ( wxRect(tbar->GetSize()).Contains(tbarPt) )
    {
        const wxToolBarToolBase * const
            tool = tbar->FindToolForPosition(tbarPt.x, tbarPt.y);

        if ( tool )
        {
            pagePos = tbar->GetToolPos(tool->GetId());
            if ( flags )
                *flags = wxBK_HITTEST_ONICON | wxBK_HITTEST_ONLABEL;
        }
    }
    else // not over the toolbar
    {
        if ( flags && GetPageRect().Contains(pt) )
            *flags |= wxBK_HITTEST_ONPAGE;
    }

    return pagePos;
}

void wxToolbook::OnIdle(wxIdleEvent& event)
{
    if (m_needsRealizing)
        Realize();
    event.Skip();
}

// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool wxToolbook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( IsDeletingAllPages() ||
         wxToolbookTopologyTransaction::IsActiveFor(this) )
        return false;

    const wxWeakRef<wxToolbook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxToolBarBase* const toolbar = GetToolBar();
    const wxWeakRef<wxToolBarBase> weakToolbar(toolbar);
    const auto preCommitResult = [&]() -> bool
    {
        wxToolbook* const book = weakThis.get();
        if ( !book || weakPage.get() != page ||
                page->GetParent() != book )
            return true;

        return book->wxBookCtrlBase::FindPage(page) != wxNOT_FOUND;
    };
    const size_t countBefore = wxBookCtrlBase::GetPageCount();
    if ( weakThis.get() != this )
        return preCommitResult();

    const int selectionBefore = m_selection;
    const int shiftedSelection =
        selectionBefore != wxNOT_FOUND &&
        static_cast<int>(n) <= selectionBefore
            ? selectionBefore + 1
            : selectionBefore;
    const bool needsRealizingBefore = m_needsRealizing;
    std::vector<wxWindow*> pagesBefore;
    std::vector<wxWeakRef<wxWindow>> pageLifetimesBefore;
    pagesBefore.reserve(countBefore);
    pageLifetimesBefore.reserve(countBefore);
    for ( size_t i = 0; i < countBefore; ++i )
    {
        wxToolbook* current = weakThis.get();
        if ( !current ||
                current->wxBookCtrlBase::GetPageCount() != countBefore )
            return preCommitResult();

        wxWindow* const existingPage =
            current->wxBookCtrlBase::GetPage(i);
        current = weakThis.get();
        if ( !current ||
                current->wxBookCtrlBase::GetPageCount() != countBefore )
            return preCommitResult();

        pagesBefore.push_back(existingPage);
        pageLifetimesBefore.emplace_back(existingPage);
    }

    // Image-list implementations are virtual and may run application code.
    // Resolve the bitmap before the common model publishes the page, otherwise
    // reentry here would expose a page for which the toolbar has no tool yet.
    const wxBitmapBundle bitmap = GetBitmapBundle(imageId);
    wxToolbook* book = weakThis.get();
    if ( !book || weakPage.get() != page ||
            page->GetParent() != book ||
            weakToolbar.get() != toolbar ||
            book->GetToolBar() != toolbar ||
            book->wxBookCtrlBase::GetPageCount() != countBefore ||
            book->m_selection != selectionBefore ||
            n > countBefore )
    {
        return preCommitResult();
    }

    for ( size_t i = 0; i < countBefore; ++i )
    {
        if ( pageLifetimesBefore[i].get() != pagesBefore[i] )
        {
            return preCommitResult();
        }

        wxWindow* const currentPage =
            book->wxBookCtrlBase::GetPage(i);
        book = weakThis.get();
        if ( !book || currentPage != pagesBefore[i] )
            return preCommitResult();
    }

    const InsertPageResult modelResult =
        DoInsertPageIntoModel(n, page, text, bSelect, imageId);
    if ( modelResult == InsertPageResult::Failed )
        return false;
    if ( modelResult == InsertPageResult::OwnershipConsumed )
        return true;

    const size_t expectedCount = wxBookCtrlBase::GetPageCount();
    const size_t controllerCountBefore = toolbar->GetToolsCount();
    const auto finishCommittedInsertion = [&]() -> bool
    {
        wxToolbook* current = weakThis.get();
        if ( current &&
                (weakPage.get() != page ||
                 page->GetParent() != current) &&
                n < current->wxBookCtrlBase::GetPageCount() &&
                current->wxBookCtrlBase::GetPage(n) == page )
        {
            current->DoErasePageRange(n, 1);
            if ( wxWeakWindowIsAvailableForCallbacks(
                     weakToolbar, toolbar) &&
                    current->GetToolBar() == toolbar &&
                    toolbar->GetToolsCount() == expectedCount )
            {
                (void)toolbar->DeleteToolByPos(n);
            }

            current = weakThis.get();
            if ( current )
            {
                current->m_needsRealizing = needsRealizingBefore;
                if ( current->m_selection != selectionBefore ||
                        shiftedSelection == selectionBefore )
                {
                    current->DoSetSelectionAfterRemoval(n);
                }
            }
        }

        current = weakThis.get();
        if ( current )
            (void)current->DoReconcilePageVisibility();

        return true;
    };
    std::vector<wxWindow*> pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    pages.reserve(expectedCount);
    pageLifetimes.reserve(expectedCount);
    for ( size_t i = 0; i < expectedCount; ++i )
    {
        book = weakThis.get();
        if ( !book ||
                book->wxBookCtrlBase::GetPageCount() != expectedCount )
            return finishCommittedInsertion();

        wxWindow* const expectedPage =
            book->wxBookCtrlBase::GetPage(i);
        book = weakThis.get();
        if ( !book ||
                book->wxBookCtrlBase::GetPageCount() != expectedCount )
            return finishCommittedInsertion();

        pages.push_back(expectedPage);
        pageLifetimes.emplace_back(expectedPage);
    }
    const auto getCurrent = [&]() -> wxToolbook*
    {
        wxToolbook* book = weakThis.get();
        if ( !book ||
                weakPage.get() != page ||
                page->GetParent() != book ||
                weakToolbar.get() != toolbar ||
                book->GetToolBar() != toolbar ||
                book->wxBookCtrlBase::GetPageCount() != expectedCount )
        {
            return nullptr;
        }

        for ( size_t i = 0; i < expectedCount; ++i )
        {
            if ( pageLifetimes[i].get() != pages[i] )
            {
                return nullptr;
            }

            wxWindow* const currentPage =
                book->wxBookCtrlBase::GetPage(i);
            book = weakThis.get();
            if ( !book || currentPage != pages[i] )
                return nullptr;
        }

        return book;
    };
    const auto rollbackCommonPage = [&]() -> bool
    {
        wxToolbook* const current = getCurrent();
        if ( current &&
                toolbar->GetToolsCount() == controllerCountBefore )
        {
            if ( current->m_selection == shiftedSelection )
                current->m_selection = selectionBefore;
            current->m_needsRealizing = needsRealizingBefore;
            wxWindow* const rolledBack =
                current->wxBookCtrlBase::DoRemovePage(n);
            if ( rolledBack == page && weakPage.get() == page )
                return false;
        }

        return finishCommittedInsertion();
    };

    m_needsRealizing = true;

    book = getCurrent();
    if ( !book )
        return finishCommittedInsertion();

    if ( controllerCountBefore != expectedCount - 1 )
        return rollbackCommonPage();

    const wxToolbookTopologyTransaction topologyTransaction(this);

    // Publish the shifted selection before the controller mutation. Any
    // synchronous nested selection then observes the new page indices and is
    // the authoritative writer if it changes this value. Topology writers for
    // this same book are rejected until both parallel models are coherent.
    book->m_selection = shiftedSelection;

    const int toolId = page->GetId();
    wxToolbookTopologyTransaction::BeginControllerPublication(book);
    if ( !toolbar->InsertTool(n, toolId, text, bitmap,
                              wxBitmapBundle(), wxITEM_RADIO) )
    {
        if ( wxToolbook* const current = getCurrent() )
        {
            (void)wxToolbookTopologyTransaction::
                EndControllerPublication(
                    current, false, nullptr, nullptr);
        }
        return rollbackCommonPage();
    }

    book = getCurrent();
    if ( !book || toolbar->GetToolsCount() != expectedCount )
    {
        if ( book )
        {
            (void)wxToolbookTopologyTransaction::
                EndControllerPublication(
                    book, false, nullptr, nullptr);
        }
        return finishCommittedInsertion();
    }

    size_t deferredSelection = 0;
    int deferredSelectionFlags = 0;
    if ( wxToolbookTopologyTransaction::EndControllerPublication(
             book, true, &deferredSelection,
             &deferredSelectionFlags) )
    {
        book->DoSetSelection(
            deferredSelection, deferredSelectionFlags);
        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();
    }

    const bool selectionOvertaken =
        book->m_selection != shiftedSelection;
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
    }
    else if ( selectionBefore == wxNOT_FOUND )
    {
        book->DoShowPage(page, true);
        book = getCurrent();
        if ( !book || book->m_selection != shiftedSelection )
            return finishCommittedInsertion();

        book->m_selection = n;
    }
    else
    {
        book->DoShowPage(page, false);
        book = getCurrent();
        if ( !book || book->m_selection != shiftedSelection )
            return finishCommittedInsertion();
    }

    if ( !selectionOvertaken &&
            bSelect && book->m_selection != static_cast<int>(n) )
    {
        book->SetSelection(n);
        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();
    }

    // InsertTool() crosses XAML Loaded/layout callbacks before publishing its
    // wrapper in the toolbar list. A nested ChangeSelection() is the
    // authoritative writer, but the candidate radio button did not yet exist
    // when that writer projected the selection. Re-assert the exact positional
    // item now that both parallel models are coherent.
    for ( unsigned attempt = 0; attempt < 3; ++attempt )
    {
        const int selection = book->m_selection;
        if ( selection == wxNOT_FOUND )
        {
            for ( size_t i = 0; i < expectedCount; ++i )
                toolbar->DoToggleToolByPos(i, false);
        }
        else if ( selection >= 0 &&
                  static_cast<size_t>(selection) < expectedCount )
        {
            toolbar->DoToggleToolByPos(
                static_cast<size_t>(selection), true);
        }

        book = getCurrent();
        if ( !book )
            return finishCommittedInsertion();
        if ( book->m_selection == selection )
            break;
    }

    book->InvalidateBestSize();
    return finishCommittedInsertion();
}

wxWindow *wxToolbook::DoRemovePage(size_t page)
{
    if ( (IsDeletingAllPages() &&
            !IsPerformingDeleteAllPageRemoval()) ||
         wxToolbookTopologyTransaction::IsActiveFor(this) )
        return nullptr;

    wxCHECK_MSG( page < wxBookCtrlBase::GetPageCount(), nullptr,
                 wxT("invalid toolbook page index") );

    const wxWeakRef<wxToolbook> weakThis(this);
    wxToolBarBase* const toolbar = GetToolBar();
    const wxWeakRef<wxToolBarBase> weakToolbar(toolbar);
    wxWindow* const expectedPage = wxBookCtrlBase::GetPage(page);
    const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);
    const size_t pageCount = wxBookCtrlBase::GetPageCount();
    const size_t controllerCount = toolbar->GetToolsCount();
    if ( controllerCount != pageCount )
        return nullptr;

    wxToolbookTopologyTransaction transaction(this);

    // Stage the exact controller position first. Even though normal Toolbook
    // event routing expects unique page IDs, deletion must not accidentally
    // remove another projection if malformed input contains a duplicate. A
    // failed transactional toolbar rebuild leaves both models unchanged.
    if ( !toolbar->DeleteToolByPos(page) )
        return nullptr;

    wxToolbook* book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakToolbar, toolbar) ||
            book->GetToolBar() != toolbar ||
            weakExpectedPage.get() != expectedPage ||
            book->wxBookCtrlBase::GetPageCount() != pageCount ||
            toolbar->GetToolsCount() != controllerCount - 1 ||
            book->wxBookCtrlBase::GetPage(page) != expectedPage )
    {
        if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                wxWeakWindowIsAvailableForCallbacks(weakToolbar, toolbar) &&
                book->GetToolBar() == toolbar &&
                book->wxBookCtrlBase::GetPageCount() == pageCount &&
                toolbar->GetToolsCount() == controllerCount - 1 &&
                book->wxBookCtrlBase::GetPage(page) == expectedPage &&
                weakExpectedPage.get() != expectedPage )
        {
            // The toolbar callback consumed the page before common-model
            // publication. Erase the dangling identity while the topology
            // transaction still excludes nested writers.
            book->DoErasePageRange(page, 1);
            book->DoSetSelectionAfterRemoval(page);
        }
        return nullptr;
    }

    wxWindow* const win =
        book->wxBookCtrlBase::DoRemovePage(page);
    const wxWeakRef<wxWindow> weakRemoved(win);
    const auto getTransferredPage = [&]() -> wxWindow*
    {
        wxToolbook* const book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !win || win != expectedPage ||
                weakExpectedPage.get() != win ||
                !wxWeakWindowIsAvailableForCallbacks(weakRemoved, win) )
        {
            return nullptr;
        }

        return book->wxBookCtrlBase::FindPage(win) == wxNOT_FOUND
                    ? win
                    : nullptr;
    };

    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            !wxWeakWindowIsAvailableForCallbacks(weakToolbar, toolbar) ||
            book->GetToolBar() != toolbar ||
            book->wxBookCtrlBase::GetPageCount() != pageCount - 1 )
    {
        return getTransferredPage();
    }

    // The common model can be committed even when invalidation consumed the
    // page and therefore returned no transferable pointer.
    book->DoSetSelectionAfterRemoval(page);
    return getTransferredPage();
}


bool wxToolbook::DeleteAllPages()
{
    if ( IsDeletingAllPages() ||
         wxToolbookTopologyTransaction::IsActiveFor(this) )
    {
        return false;
    }

    const wxWeakRef<wxToolbook> weakThis(this);
    wxToolBarBase* const toolbar = GetToolBar();
    const wxWeakRef<wxToolBarBase> weakToolbar(toolbar);
    const bool deleted = wxBookCtrlBase::DeleteAllPages();
    wxToolbook* const book = weakThis.get();
    if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
            wxWeakWindowIsAvailableForCallbacks(weakToolbar, toolbar) &&
            book->GetToolBar() == toolbar &&
            book->wxBookCtrlBase::GetPageCount() == 0 &&
            toolbar->GetToolsCount() == 0 )
    {
        book->m_needsRealizing = false;
    }

    return deleted;
}

bool wxToolbook::EnablePage(size_t page, bool enable)
{
    if ( page >= wxBookCtrlBase::GetPageCount() ||
         page >= GetToolBar()->GetToolsCount() )
    {
        return false;
    }

    GetToolBar()->DoEnableToolByPos(page, enable);
    if (!enable && GetSelection() == (int)page)
    {
        AdvanceSelection();
    }
    return true;
}

bool wxToolbook::EnablePage(wxWindow *page, bool enable)
{
    const int pageIndex = FindPage(page);
    if (pageIndex == wxNOT_FOUND)
    {
        return false;
    }
    return EnablePage(pageIndex, enable);
}

int wxToolbook::PageToToolId(size_t page) const
{
    wxCHECK_MSG(page < GetPageCount(), wxID_NONE, "Invalid page number");
    return GetPage(page)->GetId();
}

int wxToolbook::ToolIdToPage(int toolId) const
{
    for (size_t i = 0; i < m_pages.size(); i++)
    {
        if (m_pages[i]->GetId() == toolId)
        {
            return (int) i;
        }
    }
    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// wxToolbook events
// ----------------------------------------------------------------------------

void wxToolbook::OnToolSelected(wxCommandEvent& event)
{
    // find page for the tool
    int page = ToolIdToPage(event.GetId());
    if (page == wxNOT_FOUND)
    {
        // we may have gotten an event from something other than our tool, e.g.
        // a menu item from a context menu shown from the application code, so
        // take care to avoid consuming it in this case
        event.Skip();
        return;
    }

    if (page == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    const wxWeakRef<wxToolbook> weakThis(this);
    wxToolBarBase* const toolbar = GetToolBar();
    const wxWeakRef<wxToolBarBase> weakToolbar(toolbar);

    SetSelection(page);

    wxToolbook* const book = weakThis.get();
    if ( !book || weakToolbar.get() != toolbar ||
            book->GetToolBar() != toolbar )
    {
        return;
    }

    // change wasn't allowed, return to previous state
    if ( book->m_selection != page )
    {
        const int selection = book->m_selection;
        if ( selection != wxNOT_FOUND &&
                static_cast<size_t>(selection) < book->GetPageCount() )
        {
            toolbar->DoToggleToolByPos(
                static_cast<size_t>(selection), true);
        }
    }
}

#endif // wxUSE_TOOLBOOK
