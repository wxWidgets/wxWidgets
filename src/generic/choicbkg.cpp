///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/choicbkg.cpp
// Purpose:     generic implementation of wxChoicebook
// Author:      Vadim Zeitlin
// Modified by: Wlodzimierz ABX Skiba from generic/listbkg.cpp
// Created:     15.09.04
// Copyright:   (c) Vadim Zeitlin, Wlodzimierz Skiba
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


#if wxUSE_CHOICEBOOK

#include "wx/choicebk.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/choice.h"
    #include "wx/sizer.h"
#endif

#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/weakref.h"

#include <vector>

namespace
{

// Choice insertion/deletion calls into a controller which can synchronously
// dispatch application code. Keep one topology writer authoritative until
// both the controller and common page vector have committed.
class wxChoicebookTopologyTransaction
{
public:
    explicit wxChoicebookTopologyTransaction(wxChoicebook* const book)
        : m_book(book), m_lifetime(book), m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxChoicebookTopologyTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(const wxChoicebook* const book)
    {
        for ( wxChoicebookTopologyTransaction* current = GetActive();
              current;
              current = current->m_previous )
        {
            if ( current->m_book == book &&
                    current->m_lifetime.get() == book )
            {
                return true;
            }
        }

        return false;
    }

private:
    static wxChoicebookTopologyTransaction*& GetActive()
    {
        static thread_local wxChoicebookTopologyTransaction* active = nullptr;
        return active;
    }

    wxChoicebook* const m_book;
    const wxWeakRef<wxChoicebook> m_lifetime;
    wxChoicebookTopologyTransaction* const m_previous;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxChoicebook, wxBookCtrlBase);

wxDEFINE_EVENT( wxEVT_CHOICEBOOK_PAGE_CHANGING, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_CHOICEBOOK_PAGE_CHANGED,  wxBookCtrlEvent );

wxBEGIN_EVENT_TABLE(wxChoicebook, wxBookCtrlBase)
    EVT_CHOICE(wxID_ANY, wxChoicebook::OnChoiceSelected)
wxEND_EVENT_TABLE()

// ============================================================================
// wxChoicebook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxChoicebook creation
// ----------------------------------------------------------------------------

bool
wxChoicebook::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
        style |= wxBK_TOP;
    }

    // no border for this control, it doesn't look nice together with
    // wxChoice border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_bookctrl = new wxChoice
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize
                 );

    wxSizer* mainSizer = new wxBoxSizer(IsVertical() ? wxVERTICAL : wxHORIZONTAL);

    if (style & wxBK_RIGHT || style & wxBK_BOTTOM)
        mainSizer->Add(0, 0, 1, wxEXPAND, 0);

    m_controlSizer = new wxBoxSizer(IsVertical() ? wxHORIZONTAL : wxVERTICAL);
    m_controlSizer->Add(m_bookctrl, wxSizerFlags(1).Expand());
    wxSizerFlags flags;
    if ( IsVertical() )
        flags.Expand();
    else
        flags.CentreVertical();
    mainSizer->Add(m_controlSizer, flags.Border(wxALL, m_controlMargin));
    SetSizer(mainSizer);
    return true;
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxChoicebook::SetPageText(size_t n, const wxString& strText)
{
    GetChoiceCtrl()->SetString(n, RemoveMnemonics(strText));

    return true;
}

wxString wxChoicebook::GetPageText(size_t n) const
{
    return GetChoiceCtrl()->GetString(n);
}

int wxChoicebook::GetPageImage(size_t WXUNUSED(n)) const
{
    return wxNOT_FOUND;
}

bool wxChoicebook::SetPageImage(size_t WXUNUSED(n), int WXUNUSED(imageId))
{
    // fail silently, the code may be written to use one of several book
    // classes and call SetPageImage() unconditionally, it's better to just
    // ignore it (which is the best we can do short of rewriting this class to
    // use wxBitmapComboBox anyhow) than complain loudly about a rather
    // harmless problem

    return false;
}

// ----------------------------------------------------------------------------
// miscellaneous other stuff
// ----------------------------------------------------------------------------

void wxChoicebook::DoSetWindowVariant(wxWindowVariant variant)
{
    wxBookCtrlBase::DoSetWindowVariant(variant);
    if (m_bookctrl)
        m_bookctrl->SetWindowVariant(variant);
}

void wxChoicebook::SetImageList(wxImageList *imageList)
{
    // TODO: can be implemented in form of static bitmap near choice control

    wxBookCtrlBase::SetImageList(imageList);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

wxBookCtrlEvent* wxChoicebook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_CHOICEBOOK_PAGE_CHANGING, m_windowId);
}

void wxChoicebook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_CHOICEBOOK_PAGE_CHANGED);
}

// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxChoicebook::InsertPage(size_t n,
                         wxWindow *page,
                         const wxString& text,
                         bool bSelect,
                         int imageId)
{
    if ( wxChoicebookTopologyTransaction::IsActiveFor(this) ||
            IsDeletingAllPages() ||
            wxWindowIsUnavailableForCallbacks(this) )
    {
        return false;
    }

    const wxChoicebookTopologyTransaction transaction(this);
    const InsertPageResult modelResult =
        DoInsertPageIntoModel(n, page, text, bSelect, imageId);
    if ( modelResult == InsertPageResult::Failed )
        return false;
    if ( modelResult == InsertPageResult::OwnershipConsumed )
        return true;

    const wxWeakRef<wxChoicebook> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    wxChoice* const choice = GetChoiceCtrl();
    const wxWeakRef<wxChoice> weakChoice(choice);
    const size_t expectedCount = wxBookCtrlBase::GetPageCount();
    const unsigned int controllerCountBefore = choice->GetCount();
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
    const auto getCurrent = [&]() -> wxChoicebook*
    {
        wxChoicebook* const book = weakThis.get();
        if ( !book || weakPage.get() != page ||
                page->GetParent() != book ||
                weakChoice.get() != choice ||
                book->GetChoiceCtrl() != choice ||
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
        wxChoicebook* book = weakThis.get();
        if ( book &&
                (weakPage.get() != page || page->GetParent() != book) &&
                n < book->wxBookCtrlBase::GetPageCount() &&
                book->wxBookCtrlBase::GetPage(n) == page )
        {
            // A controller/selection callback consumed the candidate after
            // common publication. Remove the dangling identity first, then
            // converge the choice projection if it had already published.
            book->DoErasePageRange(n, 1);
            if ( wxWeakWindowIsAvailableForCallbacks(weakChoice, choice) &&
                    book->GetChoiceCtrl() == choice &&
                    choice->GetCount() == expectedCount )
            {
                choice->Delete(n);
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
        wxChoicebook* const book = getCurrent();
        if ( book && choice->GetCount() == controllerCountBefore )
        {
            if ( book->m_selection == shiftedSelection )
                book->m_selection = selectionBefore;
            wxWindow* const rolledBack =
                book->wxBookCtrlBase::DoRemovePage(n);
            if ( rolledBack == page && weakPage.get() == page )
                return false;
        }

        return finishCommittedInsertion();
    };

    if ( controllerCountBefore != expectedCount - 1 )
        return rollbackCommonPage();

    wxChoicebook* book = getCurrent();
    if ( !book )
        return finishCommittedInsertion();

    book->m_selection = shiftedSelection;
    const int inserted = choice->Insert(RemoveMnemonics(text), n);
    book = getCurrent();
    if ( !book )
        return finishCommittedInsertion();

    if ( inserted != static_cast<int>(n) )
    {
        if ( inserted != wxNOT_FOUND &&
                static_cast<unsigned int>(inserted) < choice->GetCount() )
        {
            choice->Delete(inserted);
            book = getCurrent();
            if ( !book )
                return finishCommittedInsertion();
        }

        return rollbackCommonPage();
    }

    if ( choice->GetCount() != expectedCount )
        return finishCommittedInsertion();

    const bool selectionOvertaken =
        book->m_selection != shiftedSelection;

    if ( !selectionOvertaken &&
            shiftedSelection != selectionBefore )
    {
        choice->Select(shiftedSelection);
        book = getCurrent();
        if ( !book || book->m_selection != shiftedSelection )
            return finishCommittedInsertion();
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

    return finishCommittedInsertion();
}

wxWindow *wxChoicebook::DoRemovePage(size_t page)
{
    if ( wxChoicebookTopologyTransaction::IsActiveFor(this) ||
         (IsDeletingAllPages() &&
            !IsPerformingDeleteAllPageRemoval()) ||
         wxWindowIsUnavailableForCallbacks(this) )
    {
        return nullptr;
    }

    const wxChoicebookTopologyTransaction transaction(this);
    const wxWeakRef<wxChoicebook> weakThis(this);
    wxChoice* const choice = GetChoiceCtrl();
    const wxWeakRef<wxChoice> weakChoice(choice);
    const size_t pageCount = wxBookCtrlBase::GetPageCount();
    if ( page >= pageCount || choice->GetCount() != pageCount )
        return nullptr;

    wxWindow* const expectedPage = wxBookCtrlBase::GetPage(page);
    const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);
    const int selectionBefore = m_selection;

    std::vector<wxString> strings;
    strings.reserve(pageCount);
    for ( size_t i = 0; i < pageCount; ++i )
        strings.push_back(choice->GetString(i));

    const auto restoreController = [&]() -> bool
    {
        wxChoicebook* const book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return false;

        wxChoice* const currentChoice =
            wxDynamicCast(book->m_bookctrl, wxChoice);
        if ( !currentChoice ||
                wxWindowIsUnavailableForCallbacks(currentChoice) )
        {
            return false;
        }

        const wxWeakRef<wxChoice> weakCurrentChoice(currentChoice);
        currentChoice->Clear();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             !wxWeakWindowIsAvailableForCallbacks(weakCurrentChoice,
                                                   currentChoice) ||
             book->m_bookctrl != currentChoice )
        {
            return false;
        }

        for ( const wxString& string : strings )
        {
            if ( currentChoice->Append(string) == wxNOT_FOUND ||
                 !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 !wxWeakWindowIsAvailableForCallbacks(weakCurrentChoice,
                                                       currentChoice) ||
                 book->m_bookctrl != currentChoice )
            {
                return false;
            }
        }

        if ( selectionBefore != wxNOT_FOUND )
            currentChoice->Select(selectionBefore);

        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               wxWeakWindowIsAvailableForCallbacks(weakCurrentChoice,
                                                     currentChoice) &&
               book->m_bookctrl == currentChoice &&
               currentChoice->GetCount() == pageCount;
    };

    // Mutate the controller first. Until this call returns the common model is
    // unchanged, and the active topology transaction rejects any nested
    // writer. If the controller is replaced or only partially changes, rebuild
    // the current controller from the exact pre-call snapshot.
    choice->Delete(page);
    wxChoicebook* book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return nullptr;

    if ( !wxWeakWindowIsAvailableForCallbacks(weakChoice, choice) ||
         book->GetChoiceCtrl() != choice ||
         choice->GetCount() != pageCount - 1 )
    {
        (void)restoreController();
        return nullptr;
    }

    // Delete() is an application-code boundary on some controller ports. If
    // it destroyed the target, erase its raw identity from the common model
    // before any helper can construct a tracker/weak reference from it.
    if ( expectedPage && weakExpectedPage.get() != expectedPage )
    {
        if ( page >= book->wxBookCtrlBase::GetPageCount() ||
                book->wxBookCtrlBase::GetPage(page) != expectedPage )
        {
            (void)restoreController();
            return nullptr;
        }

        book->DoErasePageRange(page, 1);
        book->DoSetSelectionAfterRemoval(page);
        return nullptr;
    }

    wxWindow* const win = book->wxBookCtrlBase::DoRemovePage(page);
    const wxWeakRef<wxWindow> weakRemoved(win);
    book = weakThis.get();
    if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
            (book->wxBookCtrlBase::GetPageCount() != pageCount - 1 ||
             book->wxBookCtrlBase::FindPage(expectedPage) != wxNOT_FOUND) )
    {
        // The controller mutation committed but the common removal did not.
        // Restore the old controller projection before reporting failure.
        (void)restoreController();
        return nullptr;
    }

    // If invalidation destroyed expectedPage after the common erase, win is
    // necessarily null. The removal nevertheless committed in both models;
    // never rebuild the controller back to a page that no longer exists.

    const auto getTransferredPage = [&]() -> wxWindow*
    {
        wxChoicebook* const current = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                !win || weakRemoved.get() != win )
            return nullptr;

        return current->wxBookCtrlBase::FindPage(win) == wxNOT_FOUND
                    ? win
                    : nullptr;
    };
    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            weakChoice.get() != choice ||
            book->GetChoiceCtrl() != choice )
    {
        return getTransferredPage();
    }

    book->DoSetSelectionAfterRemoval(page);
    book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
            weakRemoved.get() != win )
        return getTransferredPage();

    return getTransferredPage();
}


bool wxChoicebook::DeleteAllPages()
{
    return wxBookCtrlBase::DeleteAllPages();
}

// ----------------------------------------------------------------------------
// wxChoicebook events
// ----------------------------------------------------------------------------

void wxChoicebook::OnChoiceSelected(wxCommandEvent& eventChoice)
{
    if ( eventChoice.GetEventObject() != m_bookctrl )
    {
        eventChoice.Skip();
        return;
    }

    const int selNew = eventChoice.GetSelection();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    const wxWeakRef<wxChoicebook> weakThis(this);
    wxChoice* const choice = GetChoiceCtrl();
    const wxWeakRef<wxChoice> weakChoice(choice);

    SetSelection(selNew);

    wxChoicebook* const book = weakThis.get();
    if ( !book || weakChoice.get() != choice ||
            book->GetChoiceCtrl() != choice )
    {
        return;
    }

    // change wasn't allowed, return to previous state
    if ( book->m_selection != selNew )
        choice->Select(book->m_selection);
}

#endif // wxUSE_CHOICEBOOK
