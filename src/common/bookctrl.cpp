///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bookctrl.cpp
// Purpose:     wxBookCtrlBase implementation
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


#if wxUSE_BOOKCTRL

#include "wx/compositebookctrl.h"

#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

namespace
{

// Page destructors are application-code boundaries and may synchronously call
// back into the same book. Keep the outer bulk deletion authoritative: nested
// topology writers fail before touching either the common model or a derived
// controller model.
class wxBookDeleteAllTransaction
{
public:
    explicit wxBookDeleteAllTransaction(wxBookCtrlBase* const book)
        : m_book(book),
          m_lifetime(book),
          m_previous(GetActive())
    {
        GetActive() = this;
    }

    ~wxBookDeleteAllTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    static bool IsActiveFor(const wxBookCtrlBase* const book)
    {
        for ( wxBookDeleteAllTransaction* transaction = GetActive();
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

    static bool IsRemovingPageFor(const wxBookCtrlBase* const book)
    {
        for ( wxBookDeleteAllTransaction* transaction = GetActive();
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_lifetime.get() == book )
            {
                return transaction->m_isRemovingPage;
            }
        }

        return false;
    }

    void BeginPageRemoval()
    {
        wxASSERT(!m_isRemovingPage);
        m_isRemovingPage = true;
    }

    void EndPageRemoval()
    {
        wxASSERT(m_isRemovingPage);
        m_isRemovingPage = false;
    }

private:
    static wxBookDeleteAllTransaction*& GetActive()
    {
        static thread_local wxBookDeleteAllTransaction* active = nullptr;
        return active;
    }

    wxBookCtrlBase* const m_book;
    const wxWeakRef<wxBookCtrlBase> m_lifetime;
    wxBookDeleteAllTransaction* const m_previous;
    bool m_isRemovingPage{false};
};

class wxBookPagesSnapshot
{
public:
    enum class Model
    {
        Common,
        Public
    };

    explicit wxBookPagesSnapshot(wxBookCtrlBase* const book,
                                 Model model = Model::Common)
        : m_book(book),
          m_bookLifetime(book),
          m_previous(GetActive()),
          m_model(model)
    {
        // Activate the snapshot before invoking a public virtual accessor.
        // A nested page operation then invalidates this transaction even if
        // it happens to restore an indistinguishable topology (ABA).
        GetActive() = this;
        m_active = true;
        m_epoch = m_revision;

        const size_t count = ReadPageCount(book);
        if ( !IsCaptureCurrent(book) )
            return;

        m_pages.reserve(count);
        m_trackers.reserve(count);

        for ( size_t n = 0; n < count; ++n )
        {
            wxWindow* const pageAddress = ReadPage(book, n);
            if ( !IsCaptureCurrent(book) )
                return;

            wxWindow* const page = ResolveLivePage(book, pageAddress);
            if ( pageAddress && !page )
                return;

            m_pages.push_back(page);
            m_trackers.emplace_back(
                new PageLifetimeTracker(page, &m_revision));
        }

        m_valid = true;
    }

    ~wxBookPagesSnapshot()
    {
        if ( m_active )
        {
            wxASSERT(GetActive() == this);
            GetActive() = m_previous;
        }
    }

    bool IsCurrent(wxBookCtrlBase* const book) const
    {
        if ( !m_active || !m_valid || book != m_book ||
                m_bookLifetime.get() != book ||
                m_epoch != m_revision )
        {
            return false;
        }

        const size_t count = ReadPageCount(book);
        if ( !IsCaptureCurrent(book) || count != m_pages.size() )
            return false;

        if ( m_model == Model::Public )
        {
            for ( size_t n = 0; n < count; ++n )
            {
                if ( m_trackers[n]->Get() != m_pages[n] )
                    return false;

                wxWindow* const pageAddress = ReadPage(book, n);
                if ( !IsCaptureCurrent(book) ||
                        ResolveLivePage(book, pageAddress) != m_pages[n] ||
                        !IsCaptureCurrent(book) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool IsPageCurrent(wxBookCtrlBase* const book, size_t n) const
    {
        if ( !IsCurrent(book) ||
                n >= m_pages.size() ||
                m_trackers[n]->Get() != m_pages[n] )
        {
            return false;
        }

        wxWindow* const pageAddress = ReadPage(book, n);
        if ( !IsCaptureCurrent(book) )
            return false;
        wxWindow* const page = ResolveLivePage(book, pageAddress);
        return IsCaptureCurrent(book) &&
               m_epoch == m_revision &&
               m_trackers[n]->Get() == m_pages[n] &&
               page == m_pages[n];
    }

    wxWindow* GetPage(size_t n) const
    {
        return m_pages[n];
    }

    size_t GetPageCount() const
    {
        return m_pages.size();
    }

    bool Contains(const wxWindow* const page) const
    {
        return std::find(m_pages.begin(), m_pages.end(), page) !=
               m_pages.end();
    }

    static void NotifyTopologyChange(wxBookCtrlBase* const book)
    {
        for ( wxBookPagesSnapshot* snapshot = GetActive();
              snapshot;
              snapshot = snapshot->m_previous )
        {
            if ( snapshot->m_book == book )
                ++snapshot->m_revision;
        }
    }

    static void NotifyPublicOperation(wxBookCtrlBase* const book)
    {
        // This is deliberately separate from NotifyTopologyChange(): an
        // external-model book such as wxAuiNotebook never mutates m_pages.
        // Entering any nested public writer still supersedes an outer
        // snapshot before the virtual controller operation begins.
        NotifyTopologyChange(book);
    }

private:
    size_t ReadPageCount(wxBookCtrlBase* const book) const
    {
        return m_model == Model::Common
                    ? book->wxBookCtrlBase::GetPageCount()
                    : book->GetPageCount();
    }

    wxWindow* ReadPage(wxBookCtrlBase* const book, size_t n) const
    {
        return m_model == Model::Common
                    ? book->wxBookCtrlBase::GetPage(n)
                    : book->GetPage(n);
    }

    wxWindow* ResolveLivePage(wxBookCtrlBase* const book,
                              wxWindow* const address) const
    {
        if ( !address || !IsCaptureCurrent(book) )
            return nullptr;

        // Never create a weak reference/tracker from a virtual accessor's raw
        // result: a malicious or merely reentrant override can destroy the
        // page and still return its old address. Resolve identity from the
        // owner's live direct-child list first, comparing addresses only.
        for ( wxWindowList::compatibility_iterator node =
                  book->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow* const child = node->GetData();
            if ( child == address &&
                    !wxWindowIsUnavailableForCallbacks(child) &&
                    child->GetParent() == book )
            {
                return child;
            }
        }

        return nullptr;
    }

    bool IsCaptureCurrent(wxBookCtrlBase* const book) const
    {
        return m_bookLifetime.get() == book &&
               m_epoch == m_revision;
    }

    class PageLifetimeTracker final : public wxTrackerNode
    {
    public:
        PageLifetimeTracker(wxWindow* const page,
                            std::uint64_t* const revision)
            : m_page(page),
              m_revision(revision)
        {
            if ( m_page )
                m_page->AddNode(this);
        }

        ~PageLifetimeTracker()
        {
            if ( m_page )
                m_page->RemoveNode(this);
        }

        void OnObjectDestroy() override
        {
            m_page = nullptr;
            ++*m_revision;
        }

        wxWindow* Get() const
        {
            return m_page;
        }

    private:
        wxWindow* m_page;
        std::uint64_t* const m_revision;
    };

    static wxBookPagesSnapshot*& GetActive()
    {
        static thread_local wxBookPagesSnapshot* active = nullptr;
        return active;
    }

    wxBookCtrlBase* const m_book;
    const wxWeakRef<wxBookCtrlBase> m_bookLifetime;
    wxBookPagesSnapshot* const m_previous;
    const Model m_model;
    std::uint64_t m_revision{0};
    std::uint64_t m_epoch{0};
    bool m_active{false};
    bool m_valid{false};
    std::vector<wxWindow*> m_pages;
    std::vector<std::unique_ptr<PageLifetimeTracker>> m_trackers;
};

// A selection callback can make nested selection requests and eventually
// return to the same visible state as the outer request started from.
// Comparing m_selection alone can't distinguish this ABA case. Keep a
// monotonically increasing epoch for each synchronous chain of transactions:
// each accepted nested request for the same book becomes the latest writer,
// even if it ultimately selects the original page again.
class wxBookSelectionTransaction
{
public:
    explicit wxBookSelectionTransaction(wxBookCtrlBase* const book)
        : m_book(book),
          m_bookLifetime(book),
          m_previous(GetActive()),
          m_state(&m_ownState),
          m_epoch(0),
          m_accepted(false)
    {
        for ( wxBookSelectionTransaction* transaction = m_previous;
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_bookLifetime.get() == book )
            {
                m_state = transaction->m_state;
                break;
            }
        }

        // A Show()/Hide() callback can synchronously request another
        // selection, whose Show()/Hide() callback can do the same again.
        // Without a chain-wide circuit breaker this recursion has no finite
        // fixed point. Accept a generous number of legitimate nested writers,
        // then freeze the last accepted writer for the rest of this
        // synchronous chain. A later top-level call gets a fresh State and is
        // unaffected.
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

    ~wxBookSelectionTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    bool IsLatest() const
    {
        return m_bookLifetime.get() == m_book &&
               m_accepted && m_epoch == m_state->latest;
    }

    bool IsAccepted() const
    {
        return m_accepted;
    }

    std::uint64_t GetLatestEpoch() const
    {
        return m_state->latest;
    }

    void SealFurtherRequests() const
    {
        m_state->sealed = true;
    }

private:
    static constexpr unsigned MaxAcceptedRequests = 64;

    struct State
    {
        std::uint64_t latest{0};
        unsigned acceptedRequests{0};
        bool sealed{false};
    };

    static wxBookSelectionTransaction*& GetActive()
    {
        static thread_local wxBookSelectionTransaction* active = nullptr;
        return active;
    }

    wxBookCtrlBase* const m_book;
    const wxWeakRef<wxBookCtrlBase> m_bookLifetime;
    wxBookSelectionTransaction* const m_previous;
    State m_ownState;
    State* m_state;
    std::uint64_t m_epoch;
    bool m_accepted;
};

class wxBookLayoutTransaction
{
public:
    explicit wxBookLayoutTransaction(wxBookCtrlBase* const book)
        : m_book(book),
          m_bookLifetime(book),
          m_previous(GetActive()),
          m_state(&m_ownState),
          m_epoch(0)
    {
        for ( wxBookLayoutTransaction* transaction = m_previous;
              transaction;
              transaction = transaction->m_previous )
        {
            if ( transaction->m_book == book &&
                    transaction->m_bookLifetime.get() == book )
            {
                m_state = transaction->m_state;
                break;
            }
        }

        m_epoch = ++m_state->latest;
        GetActive() = this;
    }

    ~wxBookLayoutTransaction()
    {
        wxASSERT(GetActive() == this);
        GetActive() = m_previous;
    }

    bool IsLatest() const
    {
        return m_bookLifetime.get() == m_book &&
               m_epoch == m_state->latest;
    }

private:
    struct State
    {
        std::uint64_t latest{0};
    };

    static wxBookLayoutTransaction*& GetActive()
    {
        static thread_local wxBookLayoutTransaction* active = nullptr;
        return active;
    }

    wxBookCtrlBase* const m_book;
    const wxWeakRef<wxBookCtrlBase> m_bookLifetime;
    wxBookLayoutTransaction* const m_previous;
    State m_ownState;
    State* m_state;
    std::uint64_t m_epoch;
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxBookCtrlBase, wxControl);

wxBEGIN_EVENT_TABLE(wxBookCtrlBase, wxControl)
    EVT_SIZE(wxBookCtrlBase::OnSize)
#if wxUSE_HELP
    EVT_HELP(wxID_ANY, wxBookCtrlBase::OnHelp)
#endif // wxUSE_HELP
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constructors and destructors
// ----------------------------------------------------------------------------

void wxBookCtrlBase::Init()
{
    m_selection = wxNOT_FOUND;
    m_bookctrl = nullptr;
    m_fitToCurrentPage = false;

    m_internalBorder = 5;

    m_controlMargin = 0;
    m_controlSizer = nullptr;

    Bind(wxEVT_DPI_CHANGED, &wxBookCtrlBase::WXHandleDPIChanged, this);
}

bool
wxBookCtrlBase::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    return wxControl::Create
                     (
                        parent,
                        id,
                        pos,
                        size,
                        style,
                        wxDefaultValidator,
                        name
                     );
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

void wxBookCtrlBase::DoInvalidateBestSize()
{
    // notice that it is not necessary to invalidate our own best size
    // explicitly if we have m_bookctrl as it will already invalidate the best
    // size of its parent when its own size is invalidated and its parent is
    // this control
    if ( m_bookctrl )
        m_bookctrl->InvalidateBestSize();
    else
        wxControl::InvalidateBestSize();
}

wxSize wxBookCtrlBase::CalcSizeFromPage(const wxSize& sizePage) const
{
    // Add the size of the controller and the border between if it's shown.
    if ( !m_bookctrl || !m_bookctrl->IsShown() )
        return sizePage;

    // Notice that the controller size is its current size while we really want
    // to have its best size. So we only take into account its size in the
    // direction in which we should add it but not in the other one, where the
    // controller size is determined by the size of wxBookCtrl itself.
    const wxSize sizeController = GetControllerSize();

    wxSize size = sizePage;
    if ( IsVertical() )
        size.y += sizeController.y + GetInternalBorder();
    else // left/right aligned
        size.x += sizeController.x + GetInternalBorder();

    return size;
}

void wxBookCtrlBase::SetPageSize(const wxSize& size)
{
    SetClientSize(CalcSizeFromPage(size));
}

wxSize wxBookCtrlBase::DoGetBestSize() const
{
    wxSize bestSize;

    if (m_fitToCurrentPage && GetCurrentPage())
    {
        bestSize = GetCurrentPage()->GetBestSize();
    }
    else
    {
        // iterate over all pages, get the largest width and height
        const size_t nCount = m_pages.size();
        for ( size_t nPage = 0; nPage < nCount; nPage++ )
        {
            const wxWindow * const pPage = m_pages[nPage];
            if ( pPage )
                bestSize.IncTo(pPage->GetBestSize());
        }
    }

    // convert display area to window area, adding the size necessary for the
    // tabs
    return CalcSizeFromPage(bestSize);
}

wxRect wxBookCtrlBase::GetPageRect() const
{
    const wxSize size = GetControllerSize();

    wxPoint pt;
    wxRect rectPage(pt, GetClientSize());

    switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( wxT("unexpected alignment") );
            wxFALLTHROUGH;

        case wxBK_TOP:
            rectPage.y = size.y + GetInternalBorder();
            wxFALLTHROUGH;

        case wxBK_BOTTOM:
            rectPage.height -= size.y + GetInternalBorder();
            if (rectPage.height < 0)
                rectPage.height = 0;
            break;

        case wxBK_LEFT:
            rectPage.x = size.x + GetInternalBorder();
            wxFALLTHROUGH;

        case wxBK_RIGHT:
            rectPage.width -= size.x + GetInternalBorder();
            if (rectPage.width < 0)
                rectPage.width = 0;
            break;
    }

    return rectPage;
}

// Lay out controls
void wxBookCtrlBase::DoSize()
{
    if ( !m_bookctrl )
    {
        // we're not fully created yet or OnSize() should be hidden by derived class
        return;
    }

    const wxBookLayoutTransaction layoutTransaction(this);
    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    wxWindow* const controller = m_bookctrl;
    const wxWeakRef<wxWindow> weakController(controller);
    const auto isControllerCurrent = [&]() -> bool
    {
        wxBookCtrlBase* book = weakThis.get();
        return layoutTransaction.IsLatest() &&
               book && weakController.get() == controller &&
               book->m_bookctrl == controller;
    };

    if (GetSizer())
    {
        Layout();
        if ( !isControllerCurrent() )
            return;
    }
    else
    {
        // resize controller and the page area to fit inside our new size
        const wxSize sizeClient( GetClientSize() ),
                    sizeBorder( m_bookctrl->GetSize() - m_bookctrl->GetClientSize() ),
                    sizeCtrl( GetControllerSize() );

        m_bookctrl->SetClientSize( sizeCtrl.x - sizeBorder.x, sizeCtrl.y - sizeBorder.y );
        if ( !isControllerCurrent() )
            return;

        // if this changes the visibility of the scrollbars the best size changes, relayout in this case
        wxSize sizeCtrl2 = GetControllerSize();
        if ( !isControllerCurrent() )
            return;

        if ( sizeCtrl != sizeCtrl2 )
        {
            wxSize sizeBorder2 = m_bookctrl->GetSize() - m_bookctrl->GetClientSize();
            m_bookctrl->SetClientSize( sizeCtrl2.x - sizeBorder2.x, sizeCtrl2.y - sizeBorder2.y );
            if ( !isControllerCurrent() )
                return;
        }

        const wxSize sizeNew = m_bookctrl->GetSize();
        wxPoint posCtrl;
        switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
        {
            default:
                wxFAIL_MSG( wxT("unexpected alignment") );
                wxFALLTHROUGH;

            case wxBK_TOP:
            case wxBK_LEFT:
                // posCtrl is already ok
                break;

            case wxBK_BOTTOM:
                posCtrl.y = sizeClient.y - sizeNew.y;
                break;

            case wxBK_RIGHT:
                posCtrl.x = sizeClient.x - sizeNew.x;
                break;
        }

        if ( m_bookctrl->GetPosition() != posCtrl )
        {
            m_bookctrl->Move(posCtrl);
            if ( !isControllerCurrent() )
                return;
        }
    }

    // resize all pages to fit the new control size
    const wxRect pageRect = GetPageRect();
    if ( !isControllerCurrent() )
        return;

    const wxBookPagesSnapshot pages(this);
    const size_t pagesCount = pages.GetPageCount();
    for ( size_t i = 0; i < pagesCount; ++i )
    {
        if ( !isControllerCurrent() || !pages.IsPageCurrent(this, i) )
            return;

        wxWindow * const page = pages.GetPage(i);
        if ( !page )
        {
            wxASSERT_MSG( AllowNullPage(),
                wxT("Null page in a control that does not allow null pages?") );
            continue;
        }

        page->SetSize(pageRect);
        if ( !isControllerCurrent() || !pages.IsPageCurrent(this, i) )
            return;
    }
}

void wxBookCtrlBase::OnSize(wxSizeEvent& event)
{
    event.Skip();

    DoSize();
}

wxSize wxBookCtrlBase::GetControllerSize() const
{
    // For at least some book controls (e.g. wxChoicebook) it may make sense to
    // (temporarily?) hide the controller and we shouldn't leave extra space
    // for the hidden control in this case.
    if ( !m_bookctrl || !m_bookctrl->IsShown() )
        return wxSize(0, 0);

    const wxSize sizeClient = GetClientSize();

    wxSize size;

    // Ask for the best width/height considering the other direction.
    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = m_bookctrl->GetBestHeight(sizeClient.x);
    }
    else // left/right aligned
    {
        size.x = m_bookctrl->GetBestWidth(sizeClient.y);
        size.y = sizeClient.y;
    }

    return size;
}

// ----------------------------------------------------------------------------
// miscellaneous stuff
// ----------------------------------------------------------------------------

#if wxUSE_HELP

void wxBookCtrlBase::OnHelp(wxHelpEvent& event)
{
    // determine where does this even originate from to avoid redirecting it
    // back to the page which generated it (resulting in an infinite loop)

    wxWindow *source = wxStaticCast(event.GetEventObject(), wxWindow);

    // In all ports but wxUniv it's sufficient to compare the event object with
    // the book control itself to check if the event came to it directly, but
    // in wxUniv we may have other controls inside it (e.g. wxSpinButton inside
    // wxNotebook), so we need more involved checks there.
#ifdef __WXUNIVERSAL__
    while ( source )
    {
        wxWindow* const parent = source->GetParent();
        if ( parent == this )
        {
            if ( FindPage(source) != wxNOT_FOUND )
            {
                // The event comes from our own page, don't send it back to it.
                source = nullptr;
            }
            else
            {
                // Must be one of internal sub-controls such as the
                // wxSpinButton mentioned above, consider the book control
                // itself as the source of this event.
                source = this;
            }

            break;
        }

        source = parent;
    }
#endif // __WXUNIVERSAL__

    if ( source == this )
    {
        // this event is for the book control itself, redirect it to the
        // corresponding page
        wxWindow *page = nullptr;

        if ( event.GetOrigin() == wxHelpEvent::Origin_HelpButton )
        {
            // show help for the page under the mouse
            const int pagePos = HitTest(ScreenToClient(event.GetPosition()));

            if ( pagePos != wxNOT_FOUND)
            {
                page = GetPage((size_t)pagePos);
            }
        }
        else // event from keyboard or unknown source
        {
            // otherwise show the current page help
            page = GetCurrentPage();
        }

        if ( page )
        {
            // change event object to the page to avoid infinite recursion if
            // we get this event ourselves if the page doesn't handle it
            event.SetEventObject(page);

            if ( page->GetEventHandler()->ProcessEvent(event) )
            {
                // don't call event.Skip()
                return;
            }
        }
    }
    //else: event coming from one of our pages already

    event.Skip();
}

#endif // wxUSE_HELP

// ----------------------------------------------------------------------------
// pages management
// ----------------------------------------------------------------------------

bool wxBookCtrlBase::AddPage(wxWindow* page,
                             const wxString& text,
                             bool bSelect,
                             int imageId)
{
    wxBookPagesSnapshot::NotifyPublicOperation(this);

    if ( IsDeletingAllPages() )
        return false;

    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);

    // A virtual count/page query can itself enter application code. Retry a
    // bounded number of times when a nested writer supersedes the snapshot;
    // this preserves AddPage()'s append semantics without risking an
    // unbounded callback loop.
    for ( unsigned int attempt = 0; attempt < 8; ++attempt )
    {
        const wxBookPagesSnapshot pages(
            this, wxBookPagesSnapshot::Model::Public);
        wxBookCtrlBase* const book = weakThis.get();
        if ( !book || (page && weakPage.get() != page) )
            return true;

        if ( !pages.IsCurrent(book) )
            continue;

        if ( page && pages.Contains(page) )
        {
            // A nested writer from the virtual lookup already consumed the
            // candidate. Report success so the caller doesn't delete it.
            return true;
        }

        // InsertPage() owns best-size invalidation and treats it as a
        // synchronous callback boundary. Most importantly, the append index
        // comes from the public model, not wxBookCtrlBase::m_pages: AUI books
        // intentionally keep their canonical pages in m_tabs.
        return book->InsertPage(pages.GetPageCount(),
                                page, text, bSelect, imageId);
    }

    return false;
}

bool
wxBookCtrlBase::InsertPage(size_t nPage,
                           wxWindow *page,
                           const wxString& text,
                           bool bSelect,
                           int imageId)
{
    return DoInsertPageIntoModel(nPage, page, text, bSelect, imageId) !=
           InsertPageResult::Failed;
}

wxBookCtrlBase::InsertPageResult
wxBookCtrlBase::DoInsertPageIntoModel(
    size_t nPage,
    wxWindow *page,
    const wxString& WXUNUSED(text),
    bool WXUNUSED(bSelect),
    int WXUNUSED(imageId))
{
    if ( IsDeletingAllPages() )
        return InsertPageResult::Failed;

    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(page);
    const auto interruptedResult = [&]() -> InsertPageResult
    {
        wxBookCtrlBase* const book = weakThis.get();
        if ( !book || (page && weakPage.get() != page) )
            return InsertPageResult::OwnershipConsumed;

        if ( page &&
                std::find(book->m_pages.begin(),
                          book->m_pages.end(),
                          page) != book->m_pages.end() )
        {
            return InsertPageResult::OwnershipConsumed;
        }

        return InsertPageResult::Failed;
    };

    const bool allowsNull = page || AllowNullPage();
    if ( weakThis.get() != this )
        return interruptedResult();

    wxCHECK_MSG( allowsNull, InsertPageResult::Failed,
                 wxT("null page in wxBookCtrlBase::InsertPage()") );
    // A page destructor can synchronously re-enter user code. Reject trying
    // to publish that dying object as an ordinary transactional failure: it
    // is a reachable lifetime boundary, not a caller programming error.
    if ( page && page->IsBeingDeleted() )
        return InsertPageResult::Failed;
    wxCHECK_MSG( nPage <= m_pages.size(), InsertPageResult::Failed,
                 wxT("invalid page index in wxBookCtrlBase::InsertPage()") );
    wxCHECK_MSG( !page ||
                    std::find(m_pages.begin(), m_pages.end(), page) ==
                        m_pages.end(),
                 InsertPageResult::Failed,
                 wxT("can't insert the same page twice") );

    const wxBookPagesSnapshot pages(this);
    wxBookCtrlBase* snapshotBook = weakThis.get();
    if ( !pages.IsCurrent(snapshotBook) )
        return interruptedResult();

    wxWindow* const controller = snapshotBook->m_bookctrl;
    const wxWeakRef<wxWindow> weakController(controller);
    const auto isControllerCurrent =
        [&](wxBookCtrlBase* const book) -> bool
        {
            return book &&
                   weakController.get() == controller &&
                   book->m_bookctrl == controller;
        };

    if ( page )
    {
        // GetPageRect() and SetSize() are application-code boundaries (the
        // latter sends wxEVT_SIZE synchronously). Do all of this before
        // publishing the page in m_pages. This makes failure atomic for
        // derived books: if either callback destroys or mutates the book,
        // their controller is not left missing an entry for a page that the
        // common model already contains.
        const wxRect pageRect = GetPageRect();

        wxBookCtrlBase* book = weakThis.get();
        if ( !isControllerCurrent(book) ||
                weakPage.get() != page ||
                !pages.IsCurrent(book) ||
                nPage > book->m_pages.size() )
        {
            return interruptedResult();
        }

        page->SetSize(pageRect);

        book = weakThis.get();
        if ( !isControllerCurrent(book) ||
                weakPage.get() != page ||
                !pages.IsCurrent(book) ||
                nPage > book->m_pages.size() )
        {
            return interruptedResult();
        }
    }

    // Invalidation doesn't depend on the new page already being present.
    // Keep it on the pre-publication side too, and conservatively revalidate
    // it like any other call into the window hierarchy.
    wxBookCtrlBase* book = weakThis.get();
    if ( !isControllerCurrent(book) ||
            weakPage.get() != page ||
            !pages.IsCurrent(book) )
    {
        return interruptedResult();
    }

    book->DoInvalidateBestSize();

    book = weakThis.get();
    if ( !isControllerCurrent(book) ||
            weakPage.get() != page ||
            !pages.IsCurrent(book) ||
            nPage > book->m_pages.size() )
    {
        return interruptedResult();
    }

    // No callback-capable operation may be introduced between publication in
    // the common model and returning to the derived controller insertion.
    wxBookPagesSnapshot::NotifyTopologyChange(book);
    book->m_pages.insert(book->m_pages.begin() + nPage, page);

    return InsertPageResult::Inserted;
}

bool wxBookCtrlBase::IsDeletingAllPages() const
{
    return wxBookDeleteAllTransaction::IsActiveFor(this);
}

bool wxBookCtrlBase::IsPerformingDeleteAllPageRemoval() const
{
    return wxBookDeleteAllTransaction::IsRemovingPageFor(this);
}

bool wxBookCtrlBase::RemovePage(size_t nPage)
{
    wxBookPagesSnapshot::NotifyPublicOperation(this);

    if ( IsDeletingAllPages() )
        return false;

    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxBookPagesSnapshot pages(
        this, wxBookPagesSnapshot::Model::Public);
    wxBookCtrlBase* book = weakThis.get();
    if ( !pages.IsCurrent(book) )
        return false;

    wxCHECK_MSG( nPage < pages.GetPageCount(), false,
                 wxT("invalid page index in wxBookCtrlBase::RemovePage()") );

    wxWindow* const expectedPage = pages.GetPage(nPage);
    const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);

    wxWindow* const page = book->DoRemovePage(nPage);
    book = weakThis.get();
    if ( !book )
        return false;

    const wxBookPagesSnapshot remaining(
        book, wxBookPagesSnapshot::Model::Public);
    if ( !remaining.IsCurrent(book) ||
            remaining.GetPageCount() >= pages.GetPageCount() ||
            remaining.Contains(expectedPage) )
    {
        return false;
    }

    if ( !expectedPage )
        return book->AllowNullPage() && !page;

    if ( !page || page != expectedPage ||
            weakExpectedPage.get() != page ||
            page->GetParent() != book )
    {
        return false;
    }

    // RemovePage() transfers ownership but its public contract also requires
    // the detached window to be hidden. Hide() is an application-code
    // boundary, so don't claim transfer if it destroys, reparents or
    // republishes it.
    const bool wasShown = page->IsShown();
    book = weakThis.get();
    if ( !book || weakExpectedPage.get() != page ||
            !remaining.IsCurrent(book) ||
            remaining.Contains(page) ||
            page->GetParent() != book )
    {
        return false;
    }

    if ( wasShown )
        page->Hide();

    book = weakThis.get();
    return book && weakExpectedPage.get() == page &&
           remaining.IsCurrent(book) &&
           !remaining.Contains(page) &&
           page->GetParent() == book;
}

bool wxBookCtrlBase::DeletePage(size_t nPage)
{
    wxBookPagesSnapshot::NotifyPublicOperation(this);

    if ( IsDeletingAllPages() )
        return false;

    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxBookPagesSnapshot pages(
        this, wxBookPagesSnapshot::Model::Public);
    wxBookCtrlBase* book = weakThis.get();
    if ( !pages.IsCurrent(book) )
        return false;

    wxCHECK_MSG( nPage < pages.GetPageCount(), false,
                 wxT("invalid page index in wxBookCtrlBase::DeletePage()") );

    wxWindow* const expectedPage = pages.GetPage(nPage);
    const wxWeakRef<wxWindow> weakExpectedPage(expectedPage);

    wxWindow* const page = book->DoRemovePage(nPage);
    book = weakThis.get();

    // Destruction of the book during the removal consumes all of its child
    // pages, so returning false here would incorrectly invite the caller to
    // attempt a second ownership action.
    if ( !book )
        return true;

    const wxBookPagesSnapshot remaining(
        book, wxBookPagesSnapshot::Model::Public);
    if ( !remaining.IsCurrent(book) ||
            remaining.GetPageCount() >= pages.GetPageCount() ||
            remaining.Contains(expectedPage) )
    {
        return false;
    }

    if ( !expectedPage )
    {
        return book->AllowNullPage() && !page;
    }

    // A callback after the committed erase may already have destroyed the
    // page. This satisfies DeletePage() and, crucially, leaves no stale raw
    // pointer in either the common or an external controller model.
    if ( weakExpectedPage.get() != expectedPage )
        return true;

    if ( page && page != expectedPage )
        return false;

    // A callback is allowed to transfer the removed page to another owner.
    // Never destroy that owner's child and never report ownership transfer to
    // the stale outer caller.
    if ( expectedPage->GetParent() != book )
    {
        return false;
    }

    expectedPage->Destroy();

    return true;
}

bool wxBookCtrlBase::DeleteAllPages()
{
    wxBookPagesSnapshot::NotifyPublicOperation(this);

    if ( IsDeletingAllPages() ||
            wxWindowIsUnavailableForCallbacks(this) )
        return false;

    wxBookDeleteAllTransaction transaction(this);
    const wxWeakRef<wxBookCtrlBase> weakThis(this);

    struct PageIdentity
    {
        wxWindow* page;
        wxWeakRef<wxWindow> lifetime;
    };

    std::vector<PageIdentity> identities;
    std::vector<wxWindow*> currentPages;
    {
        wxBookCtrlBase* const initialBook = weakThis.get();
        const wxBookPagesSnapshot initialPages(
            initialBook, wxBookPagesSnapshot::Model::Public);
        if ( !initialPages.IsCurrent(initialBook) )
            return false;

        identities.reserve(initialPages.GetPageCount());
        currentPages.reserve(initialPages.GetPageCount());
        for ( size_t i = 0; i < initialPages.GetPageCount(); ++i )
        {
            wxWindow* const page = initialPages.GetPage(i);
            identities.push_back({ page, wxWeakRef<wxWindow>(page) });
            currentPages.push_back(page);
        }
    }

    const auto findIdentity = [&](wxWindow* const page)
        -> const PageIdentity*
    {
        for ( const PageIdentity& identity : identities )
        {
            if ( identity.page == page )
                return &identity;
        }
        return nullptr;
    };

    // Read the public model without creating lifetime trackers from any raw
    // pointer first. If a callback destroyed a still-published sibling, its
    // original weak identity catches it before the next snapshot/AddNode can
    // touch the dangling address.
    const auto readCurrentPagesSafely = [&]()
        -> std::pair<bool, std::vector<wxWindow*>>
    {
        std::vector<wxWindow*> pages;
        wxBookCtrlBase* const currentBook = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return { false, pages };

        const size_t count = currentBook->GetPageCount();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return { false, pages };
        pages.reserve(count);
        for ( size_t i = 0; i < count; ++i )
        {
            wxWindow* const page = currentBook->GetPage(i);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return { false, {} };

            if ( page )
            {
                const PageIdentity* const identity = findIdentity(page);
                if ( !identity || identity->lifetime.get() != page )
                    return { false, {} };
            }
            pages.push_back(page);
        }
        return { true, std::move(pages) };
    };

    // Remove and destroy one coherent public-model batch at a time. In
    // particular, if a later controller removal fails, pages successfully
    // removed before it must already have been destroyed: this is the
    // historical, observable partial-success contract of DeleteAllPages().
    // Same-book writers remain excluded by the transaction for the whole
    // operation. Directly deleting a different page still owned by the book
    // from a page destructor is outside the public book-control contract.
    while ( !currentPages.empty() )
    {
        wxBookCtrlBase* book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return false;

        const size_t countBefore = currentPages.size();
        wxWindow* const expectedPage = currentPages.front();
        const PageIdentity* const expectedIdentity =
            findIdentity(expectedPage);
        if ( expectedPage &&
                (!expectedIdentity ||
                 expectedIdentity->lifetime.get() != expectedPage) )
        {
            return false;
        }

        transaction.BeginPageRemoval();
        wxScopeGuard endRemoval = wxMakeGuard(
            [&transaction]()
            {
                transaction.EndPageRemoval();
            });

        wxWindow* const removedPage = book->DoRemovePage(0);
        endRemoval.Dismiss();
        transaction.EndPageRemoval();

        // A non-null return from DoRemovePage() is the ownership hand-off for
        // the exact page requested above. Arm its deletion immediately: all
        // of the public-model reads below are callback boundaries and may
        // fail after the derived controller has already committed the erase.
        // On the normal path removedBatch deletes the page first and the weak
        // identity makes this guard a no-op. A callback may also transfer the
        // detached page to another parent, in which case that new owner wins.
        wxScopeGuard deleteCommittedRoot = wxMakeGuard(
            [&, removedPage]()
            {
                wxBookCtrlBase* const currentBook = weakThis.get();
                if ( removedPage != expectedPage || !expectedPage ||
                        !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                        expectedIdentity->lifetime.get() != expectedPage ||
                        wxWindowIsUnavailableForCallbacks(expectedPage) ||
                        expectedPage->GetParent() != currentBook )
                {
                    return;
                }

                delete expectedPage;
            });
        wxUnusedVar(deleteCommittedRoot);

        book = weakThis.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
            return false;

        auto current = readCurrentPagesSafely();
        if ( !current.first )
            return false;

        const size_t countAfter = current.second.size();
        if ( countAfter >= countBefore )
            return false;

        if ( expectedPage )
        {
            // A treebook removal can consume an entire subtree, but the first
            // page is always the ownership-bearing root returned by its
            // existing DoRemovePage() slot. Never manufacture ownership for a
            // different pointer after a callback boundary.
            if ( removedPage && removedPage != expectedPage )
                return false;

            if ( std::find(current.second.begin(), current.second.end(),
                           expectedPage) != current.second.end() )
            {
                return false;
            }

            if ( expectedIdentity->lifetime.get() == expectedPage &&
                    expectedPage->GetParent() != book )
                return false;
        }

        // Treebook can remove an entire subtree in one virtual call. Collect
        // every identity removed by this successful controller transaction,
        // not just the ownership-bearing root returned by DoRemovePage().
        std::vector<PageIdentity> removedBatch;
        for ( wxWindow* const removed : currentPages )
        {
            if ( std::find(current.second.begin(), current.second.end(),
                           removed) != current.second.end() )
            {
                continue;
            }

            const PageIdentity* const identity = findIdentity(removed);
            if ( removed && identity && identity->lifetime.get() == removed )
            {
                if ( removed->GetParent() != book )
                    return false;
                removedBatch.push_back(*identity);
            }
        }

        currentPages = std::move(current.second);

        // Delete in reverse public order. This keeps a removed tree subtree's
        // descendants from observing a live model parent and preserves the
        // old per-removal ownership hand-off for ordinary books.
        for ( auto i = removedBatch.rbegin(); i != removedBatch.rend(); ++i )
        {
            const PageIdentity& removed = *i;
            book = weakThis.get();
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
                return false;

            if ( removed.lifetime.get() != removed.page )
                continue;
            if ( wxWindowIsUnavailableForCallbacks(removed.page) ||
                    removed.page->GetParent() != book )
            {
                return false;
            }

            delete removed.page;
        }
    }

    wxBookCtrlBase* const book = weakThis.get();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return false;
    book->m_selection = wxNOT_FOUND;

    return true;
}

wxWindow *wxBookCtrlBase::DoRemovePage(size_t nPage)
{
    if ( IsDeletingAllPages() &&
            !IsPerformingDeleteAllPageRemoval() )
        return nullptr;

    wxCHECK_MSG( nPage < m_pages.size(), nullptr,
                 wxT("invalid page index in wxBookCtrlBase::DoRemovePage()") );

    wxWindow* const pageRemoved = m_pages[nPage];
    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxWeakRef<wxWindow> weakPage(pageRemoved);
    DoErasePageRange(nPage, 1);
    DoInvalidateBestSize();

    wxBookCtrlBase* const book = weakThis.get();
    if ( !book || weakPage.get() != pageRemoved ||
            std::find(book->m_pages.begin(),
                      book->m_pages.end(),
                      pageRemoved) != book->m_pages.end() )
    {
        return nullptr;
    }

    return pageRemoved;
}

void wxBookCtrlBase::DoErasePageRange(size_t first, size_t count)
{
    wxCHECK_RET( first <= m_pages.size() &&
                    count <= m_pages.size() - first,
                 wxT("invalid page range in wxBookCtrlBase") );

    if ( count == 0 )
        return;

    wxBookPagesSnapshot::NotifyTopologyChange(this);
    m_pages.erase(m_pages.begin() + first,
                  m_pages.begin() + first + count);
}

bool wxBookCtrlBase::DoReconcilePageVisibility()
{
    const wxWeakRef<wxBookCtrlBase> weakThis(this);
    const wxBookPagesSnapshot pages(this);
    wxBookCtrlBase* const book = weakThis.get();
    if ( !pages.IsCurrent(book) )
        return false;

    const int selection = book->m_selection;
    const auto isSelectionCurrent = [&]() -> bool
    {
        wxBookCtrlBase* const currentBook = weakThis.get();
        return currentBook &&
               pages.IsCurrent(currentBook) &&
               currentBook->m_selection == selection;
    };

    wxWindow* selectedPage = nullptr;
    if ( selection != wxNOT_FOUND &&
            static_cast<size_t>(selection) < pages.GetPageCount() )
    {
        selectedPage = TryGetNonNullPage(selection);
        if ( !isSelectionCurrent() )
            return false;
    }

    const auto setShown =
        [&](wxWindow* const page, bool show) -> bool
        {
            const bool isShown = page->IsShown();
            if ( !isSelectionCurrent() )
                return false;

            if ( isShown != show )
                DoShowPage(page, show);

            return isSelectionCurrent();
        };

    // Show the committed page first so reconciliation never creates a
    // transient state in which no page is visible.
    if ( selectedPage && !setShown(selectedPage, true) )
        return false;

    for ( size_t i = 0; i < pages.GetPageCount(); ++i )
    {
        if ( !isSelectionCurrent() || !pages.IsPageCurrent(this, i) )
            return false;

        wxWindow* const page = pages.GetPage(i);
        if ( page && page != selectedPage && !setShown(page, false) )
            return false;
    }

    return true;
}

int wxBookCtrlBase::GetNextPage(bool forward) const
{
    int nPage;

    int nMax = GetPageCount();
    if ( nMax-- ) // decrement it to get the last valid index
    {
        int nSel = GetSelection();

        // change selection wrapping if it becomes invalid
        nPage = forward ? nSel == nMax ? 0
                                       : nSel + 1
                        : nSel == 0 ? nMax
                                    : nSel - 1;
    }
    else // notebook is empty, no next page
    {
        nPage = wxNOT_FOUND;
    }

    return nPage;
}

int wxBookCtrlBase::FindPage(const wxWindow* page) const
{
    const size_t nCount = m_pages.size();
    for ( size_t nPage = 0; nPage < nCount; nPage++ )
    {
        if ( m_pages[nPage] == page )
            return (int)nPage;
    }

    return wxNOT_FOUND;
}

bool wxBookCtrlBase::DoSetSelectionAfterInsertion(size_t n, bool bSelect)
{
    const wxWeakRef<wxBookCtrlBase> weakThis(this);

    if ( bSelect )
    {
        SetSelection(n);
        wxBookCtrlBase* const book = weakThis.get();
        return book && book->GetSelection() == static_cast<int>(n);
    }
    else if ( m_selection == wxNOT_FOUND )
    {
        ChangeSelection(0);
        wxBookCtrlBase* const book = weakThis.get();
        return book && book->GetSelection() == static_cast<int>(n);
    }
    else // We're not going to select this page.
        return false;
}

void wxBookCtrlBase::DoSetSelectionAfterRemoval(size_t n)
{
    if ( m_selection >= (int)n )
    {
        // ensure that the selection is valid
        int sel;
        if ( m_pages.empty() )
            sel = wxNOT_FOUND;
        else
            sel = m_selection ? m_selection - 1 : 0;

        // if deleting current page we shouldn't try to hide it
        m_selection = m_selection == (int)n ? wxNOT_FOUND
                                            : m_selection - 1;

        if ( sel != wxNOT_FOUND && sel != m_selection )
            SetSelection(sel);
    }
}

int wxBookCtrlBase::DoSetSelection(size_t n, int flags)
{
    wxCHECK_MSG( n < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxBookCtrlBase::DoSetSelection()") );

    const int oldSel = m_selection;

    if ( n != (size_t)oldSel )
    {
        const wxBookSelectionTransaction transaction(this);
        if ( !transaction.IsAccepted() )
            return oldSel;

        const wxWeakRef<wxBookCtrlBase> weakThis(this);
        const wxBookPagesSnapshot pages(this);
        wxBookCtrlBase* const snapshotBook = weakThis.get();
        if ( !pages.IsCurrent(snapshotBook) )
            return oldSel;

        wxWindow* const controller = snapshotBook->m_bookctrl;
        const wxWeakRef<wxWindow> weakController(controller);
        const auto isControllerCurrent =
            [&](wxBookCtrlBase* const book) -> bool
            {
                return book &&
                       weakController.get() == controller &&
                       book->m_bookctrl == controller;
            };
        const auto isRequestCurrent = [&]() -> bool
        {
            wxBookCtrlBase* const book = weakThis.get();
            return transaction.IsLatest() &&
                   isControllerCurrent(book) &&
                   book->m_selection == oldSel &&
                   n < book->m_pages.size() &&
                   pages.IsCurrent(book);
        };
        const auto restoreLatestCommittedSelection = [&]()
        {
            // Reconciliation itself calls Show()/Hide(), so allowing another
            // selection writer from those callbacks could create an endless
            // repair loop. Seal this synchronous transaction chain first:
            // nested requests made by the repair callbacks become observable
            // no-ops (the committed selection is unchanged), while a later
            // top-level request remains unrestricted.
            transaction.SealFurtherRequests();

            wxBookCtrlBase* book = weakThis.get();
            if ( !book )
                return;

            const std::uint64_t selectionEpoch =
                transaction.GetLatestEpoch();
            const wxBookPagesSnapshot currentPages(book);
            if ( weakThis.get() != book ||
                    transaction.GetLatestEpoch() != selectionEpoch ||
                    !currentPages.IsCurrent(book) )
            {
                return;
            }
            const int selection = book->m_selection;

            wxWindow* selectedPage = nullptr;
            if ( selection != wxNOT_FOUND &&
                    static_cast<size_t>(selection) <
                        currentPages.GetPageCount() )
            {
                selectedPage = book->TryGetNonNullPage(selection);
                if ( weakThis.get() != book ||
                        transaction.GetLatestEpoch() != selectionEpoch ||
                        !currentPages.IsCurrent(book) )
                {
                    // A topology mutation is a newer authoritative operation.
                    // Do not retry over it: its own selection repair owns the
                    // resulting model.
                    return;
                }
            }

            const auto setPageShown =
                [&](wxWindow* const page, bool show) -> bool
                {
                    if ( page->IsShown() != show )
                        book->DoShowPage(page, show);

                    return weakThis.get() == book &&
                           transaction.GetLatestEpoch() == selectionEpoch &&
                           currentPages.IsCurrent(book);
                };

            // Make the selected page visible first so a vetoed nested
            // selection never leaves the book with no visible page.
            if ( selectedPage && !setPageShown(selectedPage, true) )
                return;

            for ( size_t i = 0;
                  i < currentPages.GetPageCount();
                  ++i )
            {
                if ( !currentPages.IsPageCurrent(book, i) )
                    return;

                wxWindow* const page = currentPages.GetPage(i);
                if ( page && page != selectedPage &&
                        !setPageShown(page, false) )
                {
                    return;
                }
            }
        };

        wxBookCtrlEvent *event = CreatePageChangingEvent();
        if ( !isRequestCurrent() )
        {
            delete event;
            return oldSel;
        }

        wxCHECK_MSG( event, oldSel,
                     wxT("CreatePageChangingEvent() returned null") );

        bool allowed = true;

        if ( flags & SetSelection_SendEvent )
        {
            event->SetSelection(n);
            event->SetOldSelection(oldSel);
            event->SetEventObject(this);

            allowed = !GetEventHandler()->ProcessEvent(*event) || event->IsAllowed();

            // Event handlers are application code: they can destroy this
            // control, mutate its pages or perform a nested selection. The
            // nested/newer transaction is authoritative in all these cases.
            if ( !isRequestCurrent() )
            {
                delete event;
                return oldSel;
            }
        }

        if ( allowed )
        {
            if ( oldSel != wxNOT_FOUND )
            {
                wxWindow* const oldPage = TryGetNonNullPage(oldSel);
                if ( !isRequestCurrent() )
                {
                    delete event;
                    return oldSel;
                }

                if ( oldPage )
                {
                    DoShowPage(oldPage, false);
                    if ( !isRequestCurrent() )
                    {
                        restoreLatestCommittedSelection();
                        delete event;
                        return oldSel;
                    }
                }
            }

            wxWindow* const page = TryGetNonNullPage(n);
            if ( !isRequestCurrent() )
            {
                // The old page may already have been hidden above. A virtual
                // page lookup is an application-code boundary too, so a
                // nested writer here must get the same visibility
                // reconciliation as reentry from SetSize()/Show().
                restoreLatestCommittedSelection();
                delete event;
                return oldSel;
            }

            if ( page )
            {
                const wxRect pageRect = GetPageRect();
                if ( !isRequestCurrent() )
                {
                    restoreLatestCommittedSelection();
                    delete event;
                    return oldSel;
                }

                page->SetSize(pageRect);
                if ( !isRequestCurrent() )
                {
                    restoreLatestCommittedSelection();
                    delete event;
                    return oldSel;
                }

                DoShowPage(page, true);
                if ( !isRequestCurrent() )
                {
                    restoreLatestCommittedSelection();
                    delete event;
                    return oldSel;
                }
            }

            // change selection now to ignore the selection change event
            m_selection = n;
            UpdateSelectedPage(n);
            wxBookCtrlBase* const currentBook = weakThis.get();
            if ( !transaction.IsLatest() ||
                    !isControllerCurrent(currentBook) ||
                    currentBook->m_selection != static_cast<int>(n) ||
                    !pages.IsCurrent(currentBook) )
            {
                restoreLatestCommittedSelection();
                delete event;
                return oldSel;
            }

            if ( flags & SetSelection_SendEvent )
            {
                wxBookCtrlBase* const book = weakThis.get();
                if ( !transaction.IsLatest() ||
                        !isControllerCurrent(book) ||
                        book->m_selection != static_cast<int>(n) ||
                        !pages.IsCurrent(book) )
                {
                    delete event;
                    return oldSel;
                }

                // program allows the page change
                book->MakeChangedEvent(*event);

                if ( wxBookCtrlBase* const current = weakThis.get() )
                {
                    if ( transaction.IsLatest() &&
                            isControllerCurrent(current) &&
                            current->m_selection == static_cast<int>(n) &&
                            pages.IsCurrent(current) )
                    {
                        (void)current->GetEventHandler()->ProcessEvent(*event);
                    }
                }
            }
        }
        else
        {
            // Selection in the control might have already had changed.
            if ( oldSel != wxNOT_FOUND && isRequestCurrent() )
            {
                m_selection = oldSel;
                UpdateSelectedPage(oldSel);
            }
        }

        delete event;
    }

    return oldSel;
}

wxIMPLEMENT_DYNAMIC_CLASS(wxBookCtrlEvent, wxNotifyEvent);

// Implement the trivial ctor here to ensure it's emitted here and exported
// from the DLL instead of having an inline version of it which may result in
// link errors if it happens to be instantiated both inside and outside of the
// DLL, see #22805.
wxCompositeBookCtrlBase::wxCompositeBookCtrlBase() = default;

#endif // wxUSE_BOOKCTRL
