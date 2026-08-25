///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/wizard.cpp
// Purpose:     generic implementation of wxWizard class
// Author:      Vadim Zeitlin
// Modified by: Robert Cavanaugh
//              1) Added capability for wxWizardPage to accept resources
//              2) Added "Help" button handler stub
//              3) Fixed ShowPage() bug on displaying bitmaps
//              Robert Vazan (sizers)
// Created:     15.08.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
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


#if wxUSE_WIZARDDLG

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/intl.h"
    #include "wx/statbmp.h"
    #include "wx/button.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
#endif //WX_PRECOMP

#include "wx/statline.h"

#include "wx/scrolwin.h"
#include "wx/wizard.h"
#include "wx/dcmemory.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{

// A wizard event handler, validator or dynamically implemented page method is
// allowed to destroy either the wizard or the page synchronously.  Keep the
// checks in one place so every callback boundary below follows the same rule:
// never dereference an object after its weak identity disappeared or after it
// entered destruction.
bool IsWizardWindowAlive(const wxWeakRef<wxWindow>& weakWindow,
                         const wxWindow* expected)
{
    return wxWeakWindowIsAvailableForCallbacks(weakWindow, expected);
}

wxWizardPage* FindLiveWizardPageChildByAddress(
    wxWizard* wizard,
    const wxWizardPage* address)
{
    if ( !wizard || !address )
        return nullptr;

    const wxWeakRef<wxWindow> weakWizard(wizard);
    if ( !IsWizardWindowAlive(weakWizard, wizard) )
        return nullptr;

    // Compare the untrusted historical raw value as an address only. Create a
    // weak identity from the corresponding live child obtained from the
    // owner's list, never by dereferencing the raw value itself.
    for ( wxWindowList::compatibility_iterator node =
              wizard->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const child = node->GetData();
        const wxWeakRef<wxWindow> weakChild(child);
        if ( !IsWizardWindowAlive(weakWizard, wizard) ||
             !IsWizardWindowAlive(weakChild, child) ||
             child->GetParent() != wizard )
        {
            return nullptr;
        }

        wxWizardPage* const page = wxDynamicCast(child, wxWizardPage);
        if ( page == address )
            return page;
    }

    return nullptr;
}

// wxWizard, wxWizardPageSimple and wxWizardXmlHandler are exported classes.
// Their historical layouts are part of the binary interface, so the extra
// lifetime identities needed at reentrant callback boundaries must not become
// data members. Keep them in implementation-only sidecars keyed by the stable
// object address instead.
struct WizardRuntimeState
{
    wxWizardPage* currentPageAddress = nullptr;
    wxWeakRef<wxWizardPage> currentPageLifetime;
    wxWizardPage* firstPageAddress = nullptr;
    wxWeakRef<wxWizardPage> firstPageLifetime;
    unsigned long navigationGeneration = 0;
};

using WizardRuntimeStates =
    std::unordered_map<const wxWizard*, WizardRuntimeState>;

WizardRuntimeStates& GetWizardRuntimeStates()
{
    // Avoid static-destruction ordering with wxWeakRef's tracked objects.
    static WizardRuntimeStates* const states = new WizardRuntimeStates;
    return *states;
}

WizardRuntimeState& GetWizardRuntimeState(wxWizard* wizard)
{
    return GetWizardRuntimeStates()[wizard];
}

const WizardRuntimeState* FindWizardRuntimeState(const wxWizard* wizard)
{
    const WizardRuntimeStates& states = GetWizardRuntimeStates();
    const WizardRuntimeStates::const_iterator it = states.find(wizard);
    return it == states.end() ? nullptr : &it->second;
}

wxWeakRef<wxWizardPage>& WizardPageLifetime(wxWizard* wizard)
{
    return GetWizardRuntimeState(wizard).currentPageLifetime;
}

const wxWeakRef<wxWizardPage>& WizardPageLifetime(const wxWizard* wizard)
{
    const WizardRuntimeState* const state = FindWizardRuntimeState(wizard);
    if ( state )
        return state->currentPageLifetime;

    static const wxWeakRef<wxWizardPage> empty;
    return empty;
}

wxWeakRef<wxWizardPage>& WizardFirstPageLifetime(wxWizard* wizard)
{
    return GetWizardRuntimeState(wizard).firstPageLifetime;
}

void TrackWizardFirstPage(wxWizard* wizard, wxWizardPage* page)
{
    WizardRuntimeState& state = GetWizardRuntimeState(wizard);
    state.firstPageAddress = page;
    state.firstPageLifetime = wxWeakRef<wxWizardPage>(page);
}

void SynchronizeWizardFirstPage(wxWizard* wizard, wxWizardPage* rawPage)
{
    WizardRuntimeState& state = GetWizardRuntimeState(wizard);
    state.firstPageAddress = rawPage;
    state.firstPageLifetime = wxWeakRef<wxWizardPage>(
        FindLiveWizardPageChildByAddress(wizard, rawPage));
}

unsigned long& WizardNavigationGeneration(wxWizard* wizard)
{
    return GetWizardRuntimeState(wizard).navigationGeneration;
}

struct SimplePageLinks
{
    wxWeakRef<wxWindow> sourceLifetime;
    wxWizardPage* previous = nullptr;
    wxWeakRef<wxWizardPage> previousLifetime;
    wxWizardPage* next = nullptr;
    wxWeakRef<wxWizardPage> nextLifetime;
};

using SimplePageLinkStates =
    std::unordered_map<const wxWizardPageSimple*, SimplePageLinks>;

SimplePageLinkStates& GetSimplePageLinkStates()
{
    static SimplePageLinkStates* const states = new SimplePageLinkStates;
    return *states;
}

SimplePageLinks& GetSimplePageLinks(const wxWizardPageSimple* page)
{
    SimplePageLinkStates& states = GetSimplePageLinkStates();
    SimplePageLinks& links = states[page];
    wxWizardPageSimple* const mutablePage =
        const_cast<wxWizardPageSimple*>(page);

    // Init() and the destructor used to be inline.  In particular, a binary
    // compiled against an older wxWidgets can't erase this sidecar entry when
    // its page dies.  Associate it with the source weak identity as well as
    // its address so that address reuse lazily starts with a clean entry.
    if ( links.sourceLifetime.get() != mutablePage )
    {
        links = SimplePageLinks();
        links.sourceLifetime = wxWeakRef<wxWindow>(mutablePage);
    }

    // Keep the compatibility sidecar bounded even for old binaries which
    // never call the new out-of-line destructor.
    if ( states.size() > 64 )
    {
        for ( SimplePageLinkStates::iterator it = states.begin();
              it != states.end(); )
        {
            if ( it->first != page && !it->second.sourceLifetime.get() )
                it = states.erase(it);
            else
                ++it;
        }
    }

    return links;
}

void SetTrackedPreviousPage(wxWizardPageSimple* source,
                            wxWizardPage* previous)
{
    SimplePageLinks& links = GetSimplePageLinks(source);
    links.previous = previous;
    links.previousLifetime = wxWeakRef<wxWizardPage>(previous);
}

void SetTrackedNextPage(wxWizardPageSimple* source, wxWizardPage* next)
{
    SimplePageLinks& links = GetSimplePageLinks(source);
    links.next = next;
    links.nextLifetime = wxWeakRef<wxWizardPage>(next);
}

wxWizardPage* GetTrackedPreviousPage(const wxWizardPageSimple* source,
                                     wxWizardPage* rawPrevious)
{
    const wxWeakRef<wxWindow> weakSource(
        const_cast<wxWizardPageSimple*>(source));
    if ( !IsWizardWindowAlive(weakSource, source) )
        return nullptr;

    SimplePageLinks& links = GetSimplePageLinks(source);
    if ( links.previous != rawPrevious )
    {
        // Compatibility with callers compiled against the former inline
        // SetPrev(): synchronize at the first read, while the legacy contract
        // still requires the pointed-to page to be live.
        links.previous = rawPrevious;
        wxWizard* const wizard = wxDynamicCast(source->GetParent(), wxWizard);
        links.previousLifetime = wxWeakRef<wxWizardPage>(
            FindLiveWizardPageChildByAddress(wizard, rawPrevious));
    }

    return rawPrevious &&
                   wxWeakWindowIsAvailableForCallbacks(
                       links.previousLifetime, rawPrevious)
               ? rawPrevious
               : nullptr;
}

wxWizardPage* GetTrackedNextPage(const wxWizardPageSimple* source,
                                 wxWizardPage* rawNext)
{
    const wxWeakRef<wxWindow> weakSource(
        const_cast<wxWizardPageSimple*>(source));
    if ( !IsWizardWindowAlive(weakSource, source) )
        return nullptr;

    SimplePageLinks& links = GetSimplePageLinks(source);
    if ( links.next != rawNext )
    {
        // See the corresponding SetPrev() compatibility note above.
        links.next = rawNext;
        wxWizard* const wizard = wxDynamicCast(source->GetParent(), wxWizard);
        links.nextLifetime = wxWeakRef<wxWizardPage>(
            FindLiveWizardPageChildByAddress(wizard, rawNext));
    }

    return rawNext &&
                   wxWeakWindowIsAvailableForCallbacks(links.nextLifetime,
                                                       rawNext)
               ? rawNext
               : nullptr;
}

thread_local std::unordered_set<const wxWizard*> gs_wizardsEndingRun;
thread_local std::unordered_set<const wxWizard*> gs_wizardsAdaptingLayout;

class WizardRunTerminalGuard final
{
public:
    explicit WizardRunTerminalGuard(const wxWizard* wizard)
        : m_wizard(wizard),
          m_inserted(wizard &&
                     gs_wizardsEndingRun.insert(wizard).second)
    {
    }

    ~WizardRunTerminalGuard()
    {
        if ( m_inserted )
            gs_wizardsEndingRun.erase(m_wizard);
    }

private:
    const wxWizard* const m_wizard;
    const bool m_inserted;

    wxDECLARE_NO_COPY_CLASS(WizardRunTerminalGuard);
};

bool IsWizardRunEnding(const wxWizard* wizard)
{
    return gs_wizardsEndingRun.find(wizard) !=
           gs_wizardsEndingRun.end();
}

bool SizerWindowsBelongTo(wxSizer* sizer, wxWindow* ancestor)
{
    const wxWeakRef<wxWindow> weakAncestor(ancestor);
    if ( !sizer || !IsWizardWindowAlive(weakAncestor, ancestor) )
        return false;

    for ( wxSizerItemList::compatibility_iterator node =
              sizer->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxSizerItem* const item = node->GetData();
        if ( wxWindow* const window = item->GetWindow() )
        {
            const wxWeakRef<wxWindow> weakWindow(window);
            if ( !IsWizardWindowAlive(weakAncestor, ancestor) ||
             !IsWizardWindowAlive(weakWindow, window) ||
             window->GetParent() != ancestor )
            {
                return false;
            }
        }

        if ( wxSizer* const childSizer = item->GetSizer() )
        {
            if ( !SizerWindowsBelongTo(childSizer, ancestor) )
                return false;
        }
    }

    return true;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWizardSizer
// ----------------------------------------------------------------------------

class wxWizardSizer : public wxSizer
{
public:
    wxWizardSizer(wxWizard *owner);

    virtual wxSizerItem *Insert(size_t index, wxSizerItem *item) override;

    virtual void RecalcSizes() override;
    virtual wxSize CalcMin() override;

    // get the max size of all wizard pages
    wxSize GetMaxChildSize();

    // return the border which can be either set using wxWizard::SetBorder() or
    // have default value
    int GetBorder() const;

private:
    wxSize SiblingSize(wxSizerItem *child);

    wxWizard *m_owner;
    wxSize m_childSize;
};

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

wxDEFINE_EVENT( wxEVT_WIZARD_PAGE_CHANGED, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_PAGE_CHANGING, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_BEFORE_PAGE_CHANGED, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_CANCEL, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_FINISHED, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_HELP, wxWizardEvent );
wxDEFINE_EVENT( wxEVT_WIZARD_PAGE_SHOWN, wxWizardEvent );

wxBEGIN_EVENT_TABLE(wxWizard, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxWizard::OnCancel)
    EVT_BUTTON(wxID_BACKWARD, wxWizard::OnBackOrNext)
    EVT_BUTTON(wxID_FORWARD, wxWizard::OnBackOrNext)
    EVT_BUTTON(wxID_HELP, wxWizard::OnHelp)

    EVT_WIZARD_PAGE_CHANGED(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_CANCEL(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_FINISHED(wxID_ANY, wxWizard::OnWizEvent)
    EVT_WIZARD_HELP(wxID_ANY, wxWizard::OnWizEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxWizard, wxDialog);

/*
    TODO PROPERTIES :
    wxWizard
        extstyle
        title
*/

wxIMPLEMENT_ABSTRACT_CLASS(wxWizardPage, wxPanel);
wxIMPLEMENT_DYNAMIC_CLASS(wxWizardPageSimple, wxWizardPage);
wxIMPLEMENT_DYNAMIC_CLASS(wxWizardEvent, wxNotifyEvent);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWizardPage
// ----------------------------------------------------------------------------

void wxWizardPage::Init()
{
    m_bitmap = wxBitmapBundle();
}

wxWizardPage::wxWizardPage(wxWizard *parent,
                           const wxBitmapBundle& bitmap)
{
    Create(parent, bitmap);
}

bool wxWizardPage::Create(wxWizard *parent,
                          const wxBitmapBundle& bitmap)
{
    const wxWeakRef<wxWindow> weakThis(this);

    if ( !wxPanel::Create(parent, wxID_ANY) )
        return false;

    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    m_bitmap = bitmap;

    // initially the page is hidden, it's shown only when it becomes current
    Hide();

    return IsWizardWindowAlive(weakThis, this);
}

// ----------------------------------------------------------------------------
// wxWizardPageSimple
// ----------------------------------------------------------------------------

void wxWizardPageSimple::Init()
{
    m_prev = m_next = nullptr;
    SimplePageLinks& links = GetSimplePageLinks(this);
    links = SimplePageLinks();
    links.sourceLifetime = wxWeakRef<wxWindow>(this);
}

wxWizardPageSimple::~wxWizardPageSimple()
{
    GetSimplePageLinkStates().erase(this);
}

void wxWizardPageSimple::SetPrev(wxWizardPage* prev)
{
    m_prev = prev;
    SetTrackedPreviousPage(this, prev);
}

void wxWizardPageSimple::SetNext(wxWizardPage* next)
{
    m_next = next;
    SetTrackedNextPage(this, next);
}

wxWizardPage *wxWizardPageSimple::GetPrev() const
{
    return GetTrackedPreviousPage(this, m_prev);
}

wxWizardPage *wxWizardPageSimple::GetNext() const
{
    return GetTrackedNextPage(this, m_next);
}

// ----------------------------------------------------------------------------
// wxWizardSizer
// ----------------------------------------------------------------------------

wxWizardSizer::wxWizardSizer(wxWizard *owner)
             : m_owner(owner),
               m_childSize(wxDefaultSize)
{
}

wxSizerItem *wxWizardSizer::Insert(size_t index, wxSizerItem *item)
{
    m_owner->m_usingSizer = true;

    // We want to take account of the pages for the layout even when
    // they're hidden, so always add the corresponding flag.
    item->SetFlag(item->GetFlag() | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

    return wxSizer::Insert(index, item);
}

void wxWizardSizer::RecalcSizes()
{
    // Effect of this function depends on the current page and
    // it should be called whenever it changes (wxWizard::ShowPage)
    if ( wxWizardPage* const page =
             m_owner->wxWizard::GetCurrentPage() )
    {
        page->SetSize(wxRect(m_position, m_size));
    }
}

wxSize wxWizardSizer::CalcMin()
{
    return m_owner->GetPageSize();
}

wxSize wxWizardSizer::GetMaxChildSize()
{
    wxSize maxOfMin;
    wxWizard* const owner = m_owner;
    const wxWeakRef<wxWindow> weakOwner(owner);

    struct ChildSnapshot
    {
        ChildSnapshot(wxSizerItem* item_, wxWindow* window_)
            : item(item_),
              window(window_),
              weakWindow(window_)
        {
        }

        wxSizerItem* item;
        wxWindow* window;
        wxWeakRef<wxWindow> weakWindow;
    };

    std::vector<ChildSnapshot> children;
    children.reserve(m_children.GetCount());
    for ( wxSizerItemList::compatibility_iterator node = m_children.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxSizerItem* const item = node->GetData();
        children.emplace_back(item,
                              item->IsWindow() ? item->GetWindow() : nullptr);
    }

    const auto containsChild = [this](const wxSizerItem* expected)
    {
        for ( wxSizerItemList::compatibility_iterator node =
                  m_children.GetFirst();
              node;
              node = node->GetNext() )
        {
            if ( node->GetData() == expected )
                return true;
        }

        return false;
    };

    for ( const ChildSnapshot& snapshot : children )
    {
        if ( !IsWizardWindowAlive(weakOwner, owner) ||
             owner->m_sizerPage != this ||
             !containsChild(snapshot.item) )
        {
            return maxOfMin;
        }

        wxSizerItem* const child = snapshot.item;
        wxWindow* const childWindow = snapshot.window;
        if ( (childWindow &&
              (!IsWizardWindowAlive(snapshot.weakWindow, childWindow) ||
               !child->IsWindow() || child->GetWindow() != childWindow)) ||
             (!childWindow && child->IsWindow()) )
        {
            return maxOfMin;
        }

        const wxSize childMin = child->CalcMin();

        if ( !IsWizardWindowAlive(weakOwner, owner) ||
             owner->m_sizerPage != this ||
             (childWindow &&
              !IsWizardWindowAlive(snapshot.weakWindow, childWindow)) ||
             !containsChild(child) )
        {
            return maxOfMin;
        }

        maxOfMin.IncTo(childMin);

        const wxSize siblingMin = SiblingSize(child);
        if ( !IsWizardWindowAlive(weakOwner, owner) ||
             owner->m_sizerPage != this ||
             (childWindow &&
              !IsWizardWindowAlive(snapshot.weakWindow, childWindow)) ||
             !containsChild(child) )
        {
            return maxOfMin;
        }

        maxOfMin.IncTo(siblingMin);
    }

    if ( owner->m_started )
    {
        m_childSize = maxOfMin;
    }

    return maxOfMin;
}

int wxWizardSizer::GetBorder() const
{
    return m_owner->m_border;
}

wxSize wxWizardSizer::SiblingSize(wxSizerItem *child)
{
    wxSize maxSibling;
    wxWizard* const owner = m_owner;
    const wxWeakRef<wxWindow> weakOwner(owner);

    if ( child->IsWindow() )
    {
        wxWizardPage *page = wxDynamicCast(child->GetWindow(), wxWizardPage);
        if ( page )
        {
            std::unordered_set<wxWizardPage*> visited;
            visited.insert(page);

            const wxWeakRef<wxWindow> weakPage(page);
            wxWizardPage* const rawSibling = page->GetNext();
            if ( !IsWizardWindowAlive(weakOwner, owner) ||
                 !IsWizardWindowAlive(weakPage, page) )
            {
                return maxSibling;
            }

            wxWizardPage* sibling = rawSibling
                                        ? FindLiveWizardPageChildByAddress(
                                              owner, rawSibling)
                                        : nullptr;
            if ( rawSibling && !sibling )
                return maxSibling;

            while ( sibling && visited.insert(sibling).second )
            {
                const wxWeakRef<wxWindow> weakSibling(sibling);
                wxSizer* const sizer = sibling->GetSizer();
                if ( sizer )
                {
                    const wxSize siblingMin = sizer->CalcMin();
                    if ( !IsWizardWindowAlive(weakOwner, owner) ||
                         !IsWizardWindowAlive(weakSibling, sibling) )
                    {
                        return maxSibling;
                    }

                    maxSibling.IncTo(siblingMin);
                }

                wxWizardPage* const current = sibling;
                wxWizardPage* const rawNext = sibling->GetNext();
                if ( !IsWizardWindowAlive(weakOwner, owner) ||
                     !IsWizardWindowAlive(weakSibling, current) )
                {
                    return maxSibling;
                }

                sibling = rawNext
                              ? FindLiveWizardPageChildByAddress(owner, rawNext)
                              : nullptr;
                if ( rawNext && !sibling )
                    return maxSibling;
            }
        }
    }

    return maxSibling;
}

// ----------------------------------------------------------------------------
// generic wxWizard implementation
// ----------------------------------------------------------------------------

void wxWizard::Init()
{
    WizardRuntimeState& runtimeState = GetWizardRuntimeState(this);
    runtimeState.currentPageAddress = nullptr;
    WizardPageLifetime(this).Release();
    runtimeState.firstPageAddress = nullptr;
    WizardFirstPageLifetime(this).Release();
    m_posWizard = wxDefaultPosition;
    m_page = nullptr;
    m_firstpage = nullptr;
    WizardNavigationGeneration(this) = 0;
    m_btnPrev = m_btnNext = nullptr;
    m_statbmp = nullptr;
    m_sizerBmpAndPage = nullptr;
    m_sizerPage = nullptr;
    m_border = 5;
    m_started = false;
    m_wasModal = false;
    m_usingSizer = false;
    m_bitmapBackgroundColour = *wxWHITE;
    m_bitmapPlacement = 0;
    m_bitmapMinimumWidth = 115;

    Bind(wxEVT_DPI_CHANGED, &wxWizard::WXHandleDPIChanged, this);
}

bool wxWizard::SetCurrentPage(wxWizardPage* page)
{
    // Unpublish before checking the replacement so a failed transition can
    // never leave a stale raw pointer behind.
    m_page = nullptr;
    GetWizardRuntimeState(this).currentPageAddress = nullptr;
    WizardPageLifetime(this).Release();

    if ( !page )
        return true;

    const wxWeakRef<wxWindow> weakPage(page);
    if ( !IsWizardWindowAlive(weakPage, page) ||
         page->GetParent() != this )
    {
        return false;
    }

    WizardPageLifetime(this) = wxWeakRef<wxWizardPage>(page);
    GetWizardRuntimeState(this).currentPageAddress = page;
    m_page = page;
    return true;
}

bool wxWizard::Create(wxWindow *parent,
                      int id,
                      const wxString& title,
                      const wxBitmapBundle& bitmap,
                      const wxPoint& pos,
                      long style)
{
    const wxWeakRef<wxWindow> weakThis(this);

    if ( !wxDialog::Create(parent, id, title, pos, wxDefaultSize, style) )
        return false;

    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    m_posWizard = pos;
    m_bitmap = bitmap;

    DoCreateControls();

    return IsWizardWindowAlive(weakThis, this);
}

wxWizard::~wxWizard()
{
    SetCurrentPage(nullptr);
    TrackWizardFirstPage(this, nullptr);
    m_firstpage = nullptr;

    // normally we don't have to delete this sizer as it's deleted by the
    // associated window but if we never used it or didn't set it as the window
    // sizer yet, do delete it manually
    if ( !m_usingSizer || !m_started )
        delete m_sizerPage;

    GetWizardRuntimeStates().erase(this);
}

void wxWizard::AddBitmapRow(wxBoxSizer *mainColumn)
{
    m_sizerBmpAndPage = new wxBoxSizer(wxHORIZONTAL);
    mainColumn->Add(
        m_sizerBmpAndPage,
        1, // Vertically stretchable
        wxEXPAND // Horizontal stretching, no border
    );
    mainColumn->Add(0, FromDIP(5),
        0, // No vertical stretching
        wxEXPAND // No border, (mostly useless) horizontal stretching
    );

#if wxUSE_STATBMP
    if ( m_bitmap.IsOk() )
    {
        wxSize bitmapSize(wxDefaultSize);
        if (GetBitmapPlacement())
            bitmapSize.x = GetMinimumBitmapWidth();

        m_statbmp = new wxStaticBitmap(this, wxID_ANY, m_bitmap, wxDefaultPosition, bitmapSize);
        m_sizerBmpAndPage->Add(
            m_statbmp,
            0, // No horizontal stretching
            wxALL, // Border all around, top alignment
            FromDIP(5) // Border width
        );
        m_sizerBmpAndPage->Add(
            FromDIP(5), 0,
            0, // No horizontal stretching
            wxEXPAND // No border, (mostly useless) vertical stretching
        );
    }
#endif

    // Added to m_sizerBmpAndPage later
    m_sizerPage = new wxWizardSizer(this);
}

void wxWizard::AddStaticLine(wxBoxSizer *mainColumn)
{
#if wxUSE_STATLINE
    mainColumn->Add(
        new wxStaticLine(this, wxID_ANY),
        0, // Vertically unstretchable
        wxEXPAND | wxALL, // Border all around, horizontally stretchable
        FromDIP(5) // Border width
    );
    mainColumn->Add(0, FromDIP(5),
        0, // No vertical stretching
        wxEXPAND // No border, (mostly useless) horizontal stretching
    );
#else
    (void)mainColumn;
#endif // wxUSE_STATLINE
}

void wxWizard::AddBackNextPair(wxBoxSizer *buttonRow)
{
    wxASSERT_MSG( m_btnNext && m_btnPrev,
                  wxT("You must create the buttons before calling ")
                  wxT("wxWizard::AddBackNextPair") );

    wxBoxSizer *backNextPair = new wxBoxSizer(wxHORIZONTAL);
    buttonRow->Add(
        backNextPair,
        0, // No horizontal stretching
        wxALL, // Border all around
        FromDIP(5) // Border width
    );

    backNextPair->Add(m_btnPrev);
    backNextPair->Add(FromDIP(10), 0,
        0, // No horizontal stretching
        wxEXPAND // No border, (mostly useless) vertical stretching
    );
    backNextPair->Add(m_btnNext);
}

void wxWizard::AddButtonRow(wxBoxSizer *mainColumn)
{
    // the order in which the buttons are created determines the TAB order - at least under MSWindows...
    // although the 'back' button appears before the 'next' button, a more userfriendly tab order is
    // to activate the 'next' button first (create the next button before the back button).
    // The reason is: The user will repeatedly enter information in the wizard pages and then wants to
    // press 'next'. If a user uses mostly the keyboard, he would have to skip the 'back' button
    // every time. This is annoying. There is a second reason: RETURN acts as TAB. If the 'next'
    // button comes first in the TAB order, the user can enter information very fast using the RETURN
    // key to TAB to the next entry field and page. This would not be possible, if the 'back' button
    // was created before the 'next' button.

    bool isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
    int buttonStyle = isPda ? wxBU_EXACTFIT : 0;

    wxBoxSizer *buttonRow = new wxBoxSizer(wxHORIZONTAL);
#ifdef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        mainColumn->Add(
            buttonRow,
            0, // Vertically unstretchable
            wxEXPAND
            );
    else
#endif
    mainColumn->Add(
        buttonRow,
        0, // Vertically unstretchable
        wxALIGN_RIGHT // Right aligned, no border
    );

    // Desired TAB order is 'next', 'cancel', 'help', 'back'. This makes the 'back' button the last control on the page.
    // Create the buttons in the right order...
    wxButton *btnHelp=nullptr;
#ifdef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        btnHelp=new wxButton(this, wxID_HELP, wxEmptyString, wxDefaultPosition, wxDefaultSize, buttonStyle);
#endif

    m_nextLabel = _("&Next >");
    m_finishLabel = _("&Finish");

    m_btnNext = new wxButton(this, wxID_FORWARD, m_nextLabel);
    wxButton *btnCancel=new wxButton(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, buttonStyle);
#ifndef __WXMAC__
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
        btnHelp=new wxButton(this, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, buttonStyle);
#endif
    m_btnPrev = new wxButton(this, wxID_BACKWARD, _("< &Back"), wxDefaultPosition, wxDefaultSize, buttonStyle);

    // compute the maximum width of the buttons and use it for all of them
    // (except for the "Help" button under Mac which is special there)
    wxSize buttonSize;
    buttonSize.IncTo(m_btnPrev->GetBestSize());
    buttonSize.IncTo(m_btnNext->GetBestSize());
    buttonSize.IncTo(btnCancel->GetBestSize());

    // use the other possible label for the "Next" button temporarily, so that
    // we could make it big enough to fit it too if it's longer
    m_btnNext->SetLabel(m_finishLabel);
    buttonSize.IncTo(m_btnNext->GetBestSize());

#ifndef __WXMAC__
    if (btnHelp)
        buttonSize.IncTo(btnHelp->GetBestSize());
#endif

    // now do make all buttons of the same (and big enough) size
    m_btnPrev->SetMinSize(buttonSize);
    m_btnNext->SetMinSize(buttonSize);
    btnCancel->SetMinSize(buttonSize);

#ifndef __WXMAC__
    if (btnHelp)
        btnHelp->SetMinSize(buttonSize);
#endif

    // restore the initial label of the 'next' button after temporarily
    // changing it above
    m_btnNext->SetLabel(m_nextLabel);

    if (btnHelp)
    {
        buttonRow->Add(
            btnHelp,
            0, // Horizontally unstretchable
            wxALL, // Border all around, top aligned
            FromDIP(5) // Border width
            );
#ifdef __WXMAC__
        // Put stretchable space between help button and others
        buttonRow->Add(0, 0, 1, wxALIGN_CENTRE, 0);
#endif
    }

    AddBackNextPair(buttonRow);

    buttonRow->Add(
        btnCancel,
        0, // Horizontally unstretchable
        wxALL, // Border all around, top aligned
        FromDIP(5) // Border width
    );
}

void wxWizard::DoCreateControls()
{
    // do nothing if the controls were already created
    if ( WasCreated() )
        return;

    bool isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    // Horizontal stretching, and if not PDA, border all around
    int mainColumnSizerFlags = isPda ? wxEXPAND : wxALL|wxEXPAND ;

    // wxWindow::SetSizer will be called at end
    wxBoxSizer *windowSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *mainColumn = new wxBoxSizer(wxVERTICAL);
    windowSizer->Add(
        mainColumn,
        1, // Vertical stretching
        mainColumnSizerFlags,
        FromDIP(5) // Border width
    );

    AddBitmapRow(mainColumn);

    if (!isPda)
        AddStaticLine(mainColumn);

    AddButtonRow(mainColumn);

    SetSizer(windowSizer);
}

void wxWizard::SetPageSize(const wxSize& size)
{
    wxCHECK_RET(!m_started, wxT("wxWizard::SetPageSize after RunWizard"));
    m_sizePage = size;
}

void wxWizard::FitToPage(const wxWizardPage *page)
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) )
        return;

    wxCHECK_RET(!m_started, wxT("wxWizard::FitToPage after RunWizard"));

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);
    std::unordered_set<const wxWizardPage*> visited;

    while ( page )
    {
        // A malformed dynamic chain must not keep layout in an infinite loop.
        if ( !visited.insert(page).second )
            return;

        const wxWeakRef<wxWindow> weakPage(
            const_cast<wxWizardPage*>(page));
        if ( !IsWizardWindowAlive(weakThis, this) || m_started ||
             WizardNavigationGeneration(this) != navigationGeneration ||
             !IsWizardWindowAlive(weakPage, page) ||
             page->GetParent() != this )
        {
            return;
        }

        const wxSize size = page->GetBestSize();

        if ( !IsWizardWindowAlive(weakThis, this) || m_started ||
             WizardNavigationGeneration(this) != navigationGeneration ||
             !IsWizardWindowAlive(weakPage, page) ||
             page->GetParent() != this )
        {
            return;
        }

        m_sizePage.IncTo(size);

        const wxWizardPage* const current = page;
        wxWizardPage* const rawNext = page->GetNext();

        if ( !IsWizardWindowAlive(weakThis, this) || m_started ||
             WizardNavigationGeneration(this) != navigationGeneration ||
             !IsWizardWindowAlive(weakPage, current) ||
             current->GetParent() != this )
        {
            return;
        }

        page = rawNext
                   ? FindLiveWizardPageChildByAddress(this, rawNext)
                   : nullptr;
        if ( rawNext && !page )
            return;

        if ( page )
        {
            const wxWeakRef<wxWindow> weakNext(
                const_cast<wxWizardPage*>(page));
            if ( !IsWizardWindowAlive(weakNext, page) ||
                 page->GetParent() != this )
            {
                return;
            }
        }
    }
}

bool wxWizard::ShowPage(wxWizardPage *page, bool goingForward)
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    wxCHECK_MSG( WasCreated(), false,
                 wxT("can't show a page in an uncreated wizard") );
    if ( IsWizardRunEnding(this) )
        return false;

    const wxWeakRef<wxWindow> weakTarget(page);
    wxCHECK_MSG( !page ||
                     (IsWizardWindowAlive(weakTarget, page) &&
                      page->GetParent() == this),
                 false,
                 wxT("wizard page must belong to this wizard") );

    // A terminal transition becomes authoritative before hiding the current
    // page: wxEVT_SHOW is synchronous and may otherwise reopen another page
    // before the terminal branch below can publish its result.
    const WizardRunTerminalGuard terminalGuard(page ? nullptr : this);

    const unsigned long previousNavigationGeneration = WizardNavigationGeneration(this);
    const unsigned long navigationGeneration = ++WizardNavigationGeneration(this);
    bool transitionCommitted = false;
    wxScopeGuard restoreNavigationGeneration = wxMakeGuard(
        [this, &weakThis, previousNavigationGeneration,
         navigationGeneration, &transitionCommitted]()
        {
            // A failed nested attempt must not poison its caller's token. But
            // never overwrite the token of a deeper transition that actually
            // committed and therefore superseded this one.
            if ( !transitionCommitted &&
                 IsWizardWindowAlive(weakThis, this) &&
                 WizardNavigationGeneration(this) == navigationGeneration )
            {
                WizardNavigationGeneration(this) = previousNavigationGeneration;
            }
        });
    wxUnusedVar(restoreNavigationGeneration);

    wxWizardPage* const oldPage = wxWizard::GetCurrentPage();
    if ( !oldPage && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return false;
        }
    }

    wxCHECK_MSG( page != oldPage, false, wxT("this is useless") );
    const wxWeakRef<wxWindow> weakOldPage(oldPage);

    wxButton* const btnPrev = m_btnPrev;
    wxButton* const btnNext = m_btnNext;
    const wxWeakRef<wxWindow> weakBtnPrev(btnPrev);
    const wxWeakRef<wxWindow> weakBtnNext(btnNext);
#if wxUSE_STATBMP
    wxStaticBitmap* const initialStatbmp = m_statbmp;
    const wxWeakRef<wxWindow> weakInitialStatbmp(initialStatbmp);
#endif

    const auto wizardAndControlsAreAlive = [this, &weakThis,
                                             navigationGeneration,
                                             btnPrev, &weakBtnPrev,
                                             btnNext, &weakBtnNext
#if wxUSE_STATBMP
                                             , initialStatbmp,
                                             &weakInitialStatbmp
#endif
                                             ]()
    {
        if ( !IsWizardWindowAlive(weakThis, this) )
            return false;

        if ( WizardNavigationGeneration(this) != navigationGeneration )
            return false;

        if ( m_btnPrev != btnPrev || m_btnNext != btnNext )
            return false;

        if ( !IsWizardWindowAlive(weakBtnPrev, btnPrev) )
        {
            m_btnPrev = nullptr;
            return false;
        }

        if ( !IsWizardWindowAlive(weakBtnNext, btnNext) )
        {
            m_btnNext = nullptr;
            return false;
        }

#if wxUSE_STATBMP
        if ( m_statbmp != initialStatbmp )
            return false;

        if ( initialStatbmp &&
             !IsWizardWindowAlive(weakInitialStatbmp, initialStatbmp) )
        {
            m_statbmp = nullptr;
            return false;
        }
#endif
        return true;
    };

    const auto targetIsAlive = [this, page, &weakTarget]()
    {
        return !page ||
               (IsWizardWindowAlive(weakTarget, page) &&
                page->GetParent() == this);
    };

    const auto currentPageIsAlive =
        [this, &weakThis, &wizardAndControlsAreAlive](
            wxWizardPage* expected,
            const wxWeakRef<wxWindow>& weakExpected)
        {
            if ( !IsWizardWindowAlive(weakThis, this) ||
                 m_page != expected ||
                 WizardPageLifetime(this).get() != expected )
            {
                return false;
            }

            if ( !IsWizardWindowAlive(weakExpected, expected) ||
                 expected->GetParent() != this )
            {
                // Never leave GetCurrentPage() pointing at a page destroyed by
                // the callback we just returned from.
                SetCurrentPage(nullptr);
                return false;
            }

            return wizardAndControlsAreAlive();
        };

    if ( !wizardAndControlsAreAlive() || !targetIsAlive() )
        return false;

    wxSizerFlags flags(1);
    flags.Border(wxALL, m_border).Expand();

    wxBoxSizer* const sizerBmpAndPage = m_sizerBmpAndPage;
    wxBitmap bmpPrev;
    const bool usingSizerAtEntry = m_usingSizer;
    const bool oldPrevEnabled = btnPrev->IsEnabled();
    const wxString oldNextLabel = btnNext->GetLabel();
    const wxSize oldPageSize = oldPage ? oldPage->GetSize() : wxDefaultSize;
    bool oldPageWasHidden = false;

    wxScopeGuard rollbackTransition = wxMakeGuard(
        [this, page, oldPage, &weakOldPage, navigationGeneration,
         usingSizerAtEntry, oldPrevEnabled, oldNextLabel, oldPageSize,
         &oldPageWasHidden, &transitionCommitted, &bmpPrev,
         &weakThis, &wizardAndControlsAreAlive, &flags,
         btnPrev, btnNext, sizerBmpAndPage
#if wxUSE_STATBMP
         , initialStatbmp
#endif
         ]()
        {
            if ( transitionCommitted || !oldPageWasHidden || !oldPage ||
                 !IsWizardWindowAlive(weakThis, this) ||
                 WizardNavigationGeneration(this) != navigationGeneration ||
                 !IsWizardWindowAlive(weakOldPage, oldPage) ||
                 oldPage->GetParent() != this ||
                 m_sizerBmpAndPage != sizerBmpAndPage )
            {
                return;
            }

            wxWizardPage* current = wxWizard::GetCurrentPage();
            if ( current && current != page && current != oldPage )
            {
                // A nested navigation committed a different page.
                return;
            }

            if ( current == page )
            {
                if ( !SetCurrentPage(nullptr) ||
                     !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration )
                {
                    return;
                }
                current = wxWizard::GetCurrentPage();
            }

            if ( !current )
            {
                if ( !SetCurrentPage(oldPage) ||
                     !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration )
                {
                    return;
                }
                current = wxWizard::GetCurrentPage();
            }

            if ( current != oldPage )
                return;

            if ( !usingSizerAtEntry )
            {
                if ( m_usingSizer )
                    return;

                if ( !sizerBmpAndPage->GetItem(oldPage) )
                {
                    sizerBmpAndPage->Add(oldPage, flags);
                    if ( !wizardAndControlsAreAlive() ||
                         m_sizerBmpAndPage != sizerBmpAndPage )
                    {
                        return;
                    }
                }

                if ( oldPageSize != wxDefaultSize )
                {
                    sizerBmpAndPage->SetItemMinSize(oldPage, oldPageSize);
                    if ( !wizardAndControlsAreAlive() ||
                         m_sizerBmpAndPage != sizerBmpAndPage )
                    {
                        return;
                    }
                }
            }

            if ( !wizardAndControlsAreAlive() )
                return;

            btnPrev->Enable(oldPrevEnabled);
            if ( !wizardAndControlsAreAlive() )
                return;

            const wxString nextLabel = btnNext->GetLabel();
            if ( !wizardAndControlsAreAlive() )
                return;

            if ( nextLabel != oldNextLabel )
            {
                btnNext->SetLabel(oldNextLabel);
                if ( !wizardAndControlsAreAlive() )
                    return;
            }

            btnNext->SetDefault();
            if ( !wizardAndControlsAreAlive() )
                return;

#if wxUSE_STATBMP
            if ( initialStatbmp && bmpPrev.IsOk() )
            {
                initialStatbmp->SetBitmap(bmpPrev);
                if ( !wizardAndControlsAreAlive() )
                    return;
            }
#endif

            if ( wxWizard::GetCurrentPage() != oldPage ||
                 !IsWizardWindowAlive(weakOldPage, oldPage) ||
                 oldPage->GetParent() != this )
            {
                return;
            }

            oldPage->Show();
            if ( !wizardAndControlsAreAlive() ||
                 wxWizard::GetCurrentPage() != oldPage ||
                 !IsWizardWindowAlive(weakOldPage, oldPage) ||
                 oldPage->GetParent() != this )
            {
                return;
            }

            if ( !usingSizerAtEntry &&
                 m_sizerBmpAndPage == sizerBmpAndPage )
            {
                sizerBmpAndPage->Layout();
            }
        });
    wxUnusedVar(rollbackTransition);

    if ( !m_started )
    {
        if ( m_usingSizer )
        {
            if ( !m_sizerBmpAndPage->GetItem(m_sizerPage) )
                m_sizerBmpAndPage->Add(m_sizerPage, flags);
        }
    }

    // check for previous page
    if ( oldPage )
    {
        // send the event to the old page
        wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGING, GetId(),
                            goingForward, oldPage);
        const bool processed =
            oldPage->GetEventHandler()->ProcessEvent(event);

        if ( !currentPageIsAlive(oldPage, weakOldPage) ||
             !targetIsAlive() )
        {
            return false;
        }

        if ( processed && !event.IsAllowed() )
        {
            // vetoed by the page
            return false;
        }

        oldPage->Hide();
        oldPageWasHidden = true;
        if ( !currentPageIsAlive(oldPage, weakOldPage) ||
             !targetIsAlive() )
        {
            return false;
        }

        bmpPrev = oldPage->GetBitmap();
        if ( !currentPageIsAlive(oldPage, weakOldPage) ||
             !targetIsAlive() )
        {
            return false;
        }

        if ( !m_usingSizer )
            m_sizerBmpAndPage->Detach(oldPage);
    }

    // is this the end?
    if ( !page )
    {
        transitionCommitted = true;

        // terminate successfully
        if ( IsModal() )
        {
            EndModal(wxID_OK);
        }
        else
        {
            SetReturnCode(wxID_OK);
            Hide();
        }

        if ( !currentPageIsAlive(oldPage, weakOldPage) )
            return true;

        // and notify the user code (this is especially useful for modeless
        // wizards)
        wxWizardEvent event(wxEVT_WIZARD_FINISHED, GetId(), false, oldPage);
        (void)GetEventHandler()->ProcessEvent(event);

        if ( IsWizardWindowAlive(weakThis, this) && m_page == oldPage )
            SetCurrentPage(nullptr);

        return true;
    }

    // notice that we change m_page only here so that wxEVT_WIZARD_FINISHED
    // event above could still use the correct (i.e. old) value of m_page
    if ( !SetCurrentPage(page) )
        return false;

    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    // position and show the new page
    (void)page->TransferDataToWindow();
    if ( !currentPageIsAlive(page, weakTarget) )
    {
        if ( IsWizardWindowAlive(weakThis, this) && m_page == page &&
             !IsWizardWindowAlive(weakTarget, page) )
        {
            SetCurrentPage(nullptr);
        }
        return false;
    }

    if ( m_usingSizer )
    {
        // wxWizardSizer::RecalcSizes wants to be called when m_page changes
        m_sizerPage->RecalcSizes();

        if ( !currentPageIsAlive(page, weakTarget) )
            return false;
    }
    else // pages are not managed by the sizer
    {
        if ( !m_sizerBmpAndPage->GetItem(page) )
            m_sizerBmpAndPage->Add(page, flags);

        const wxSize pageSize = GetPageSize();
        if ( !currentPageIsAlive(page, weakTarget) )
            return false;

        m_sizerBmpAndPage->SetItemMinSize(page, pageSize);
    }

    wxBitmap bmp;
#if wxUSE_STATBMP
    // update the bitmap if:it changed
    if ( m_statbmp )
    {
        wxStaticBitmap* const statbmp = m_statbmp;
        const wxWeakRef<wxWindow> weakStatbmp(statbmp);

        bmp = page->GetBitmap();
        if ( !currentPageIsAlive(page, weakTarget) ||
             m_statbmp != statbmp ||
             !IsWizardWindowAlive(weakStatbmp, statbmp) )
        {
            return false;
        }

        if ( !bmp.IsOk() )
        {
            bmp = m_bitmap.GetBitmapFor(this);
            if ( !currentPageIsAlive(page, weakTarget) ||
                 m_statbmp != statbmp ||
                 !IsWizardWindowAlive(weakStatbmp, statbmp) )
            {
                return false;
            }
        }

        if ( !bmpPrev.IsOk() )
        {
            bmpPrev = m_bitmap.GetBitmapFor(this);
            if ( !currentPageIsAlive(page, weakTarget) ||
                 m_statbmp != statbmp ||
                 !IsWizardWindowAlive(weakStatbmp, statbmp) )
            {
                return false;
            }
        }

        if (!GetBitmapPlacement())
        {
            if ( !bmp.IsSameAs(bmpPrev) )
            {
                statbmp->SetBitmap(bmp);
                if ( !currentPageIsAlive(page, weakTarget) ||
                     m_statbmp != statbmp ||
                     !IsWizardWindowAlive(weakStatbmp, statbmp) )
                {
                    return false;
                }
            }
        }
    }
#endif // wxUSE_STATBMP


    // and update the buttons state
    WizardRuntimeState& runtimeState = GetWizardRuntimeState(this);
    if ( runtimeState.firstPageAddress != m_firstpage )
    {
        // Preserve the historical protected-member contract for derived
        // classes assigning a live page directly. Rebuilt code should prefer
        // RunWizard()/SetCurrentPage(), which publish the identity eagerly.
        SynchronizeWizardFirstPage(this, m_firstpage);
    }

    wxWizardPage* firstPage = WizardFirstPageLifetime(this).get();
    if ( firstPage != m_firstpage ||
         (firstPage &&
           (firstPage->IsBeingDeleted() || firstPage->GetParent() != this)) )
    {
        m_firstpage = nullptr;
        TrackWizardFirstPage(this, nullptr);
        firstPage = nullptr;
    }

    bool hasPrev = false;
    if ( page != firstPage )
    {
        hasPrev = HasPrevPage(page);
        if ( !currentPageIsAlive(page, weakTarget) )
            return false;
    }

    btnPrev->Enable(hasPrev);
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    const bool hasNext = HasNextPage(page);
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    const wxString label = hasNext ? m_nextLabel : m_finishLabel;
    const wxString currentLabel = btnNext->GetLabel();
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    if ( label != currentLabel )
    {
        btnNext->SetLabel(label);
        if ( !currentPageIsAlive(page, weakTarget) )
            return false;
    }

    btnNext->SetDefault();
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;


    // send the change event to the new page now
    wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), goingForward, page);
    (void)page->GetEventHandler()->ProcessEvent(event);
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    // and finally show it
    page->Show();
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    page->SetFocus();
    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    if ( !m_usingSizer )
    {
        m_sizerBmpAndPage->Layout();
        if ( !currentPageIsAlive(page, weakTarget) )
            return false;
    }

    if ( !m_started )
    {
        m_started = true;

        DoWizardLayout();
        if ( !currentPageIsAlive(page, weakTarget) )
            return false;
    }

    if (GetBitmapPlacement() && m_statbmp)
    {
        wxStaticBitmap* const statbmp = m_statbmp;
        const wxWeakRef<wxWindow> weakStatbmp(statbmp);

        (void)ResizeBitmap(bmp);
        if ( !currentPageIsAlive(page, weakTarget) ||
             m_statbmp != statbmp ||
             !IsWizardWindowAlive(weakStatbmp, statbmp) )
        {
            return false;
        }

        if ( !bmp.IsSameAs(bmpPrev) )
        {
            statbmp->SetBitmap(bmp);
            if ( !currentPageIsAlive(page, weakTarget) ||
                 m_statbmp != statbmp ||
                 !IsWizardWindowAlive(weakStatbmp, statbmp) )
            {
                return false;
            }
        }

        if (m_usingSizer)
        {
            m_sizerPage->RecalcSizes();
            if ( !currentPageIsAlive(page, weakTarget) )
                return false;
        }
    }

    wxWizardEvent pageShownEvent(wxEVT_WIZARD_PAGE_SHOWN, GetId(),
        goingForward, page);
    page->GetEventHandler()->ProcessEvent(pageShownEvent);

    if ( !currentPageIsAlive(page, weakTarget) )
        return false;

    transitionCommitted = true;
    return true;
}

/// Do fit, and adjust to screen size if necessary
void wxWizard::DoWizardLayout()
{
    const wxWeakRef<wxWindow> weakThis(this);

    if ( wxSystemSettings::GetScreenType() > wxSYS_SCREEN_PDA )
    {
        const bool canAdapt = CanDoLayoutAdaptation();
        if ( !IsWizardWindowAlive(weakThis, this) )
            return;

        if ( canAdapt )
        {
            if ( !DoLayoutAdaptation() )
                return;
        }
        else
        {
            wxSizer* const sizer = GetSizer();
            if ( sizer )
                sizer->SetSizeHints(this);
        }

        if ( !IsWizardWindowAlive(weakThis, this) )
            return;

        if ( m_posWizard == wxDefaultPosition )
        {
            CentreOnScreen();
            if ( !IsWizardWindowAlive(weakThis, this) )
                return;
        }
    }

    // A nested ShowPage() may run this layout while an outer adaptation is
    // still capable of rolling its topology back. Such a nested pass is not
    // an independent completed adaptation and must not suppress the next real
    // attempt by publishing the completion flag prematurely.
    if ( gs_wizardsAdaptingLayout.find(this) ==
            gs_wizardsAdaptingLayout.end() )
    {
        SetLayoutAdaptationDone(true);
    }
}

bool wxWizard::RunWizard(wxWizardPage *firstPage)
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) ||
         IsWizardRunEnding(this) )
        return false;

    wxCHECK_MSG( firstPage, false, wxT("can't run empty wizard") );
    const wxWeakRef<wxWindow> weakFirstPage(firstPage);
    wxCHECK_MSG( IsWizardWindowAlive(weakFirstPage, firstPage) &&
                     firstPage->GetParent() == this,
                 false,
                 wxT("wizard page must belong to this wizard") );

    // From this point on, FINISHED/CANCEL events belong to a modal run even
    // though ShowModal() hasn't started yet. Page callbacks are allowed to
    // finish or cancel during the initial ShowPage() transition; treating
    // those events as modeless would schedule destruction of this wizard.
    const bool wasModal = m_wasModal;
    m_wasModal = true;
    SetReturnCode(wxID_NONE);

    m_firstpage = firstPage;
    TrackWizardFirstPage(this, firstPage);

    const bool firstTransitionCompleted =
        ShowPage(firstPage, true /* forward */);

    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    const int returnCodeBeforeCleanup = GetReturnCode();
    const WizardRunTerminalGuard terminalCleanupGuard(
        returnCodeBeforeCleanup == wxID_OK ||
                returnCodeBeforeCleanup == wxID_CANCEL
            ? this
            : nullptr);

    wxWizardPage* currentPage = wxWizard::GetCurrentPage();
    if ( !firstTransitionCompleted && currentPage == firstPage )
    {
        // The initial transition failed after publishing the first page.
        // Leave no visible/current half-transition behind. Hide() is itself a
        // callback boundary: a nested navigation performed there wins unless
        // a terminal transition was already committed before this cleanup.
        currentPage->Hide();
        if ( !IsWizardWindowAlive(weakThis, this) )
            return false;

        currentPage = wxWizard::GetCurrentPage();
        if ( currentPage == firstPage )
        {
            (void)SetCurrentPage(nullptr);
            currentPage = wxWizard::GetCurrentPage();
        }
    }

    const int initialReturnCode = GetReturnCode();
    if ( initialReturnCode == wxID_OK ||
         initialReturnCode == wxID_CANCEL )
    {
        const WizardRunTerminalGuard terminalGuard(this);

        // A nested FINISH already clears the current page. A nested CANCEL
        // keeps it only because OnCancel() normally relies on modeless
        // destruction; clean it up here before returning without ShowModal().
        currentPage = wxWizard::GetCurrentPage();
        if ( currentPage )
        {
            currentPage->Hide();
            if ( !IsWizardWindowAlive(weakThis, this) )
                return false;

            if ( IsWizardWindowAlive(weakThis, this) )
                (void)SetCurrentPage(nullptr);
        }

        m_firstpage = nullptr;
        TrackWizardFirstPage(this, nullptr);
        return initialReturnCode == wxID_OK;
    }

    if ( !currentPage )
    {
        m_firstpage = nullptr;
        TrackWizardFirstPage(this, nullptr);
        m_wasModal = wasModal;
        return false;
    }

    if ( currentPage == firstPage &&
         !IsWizardWindowAlive(weakFirstPage, firstPage) )
    {
        return false;
    }

    if ( currentPage != firstPage &&
         !IsWizardWindowAlive(weakFirstPage, firstPage) )
    {
        // A nested transition legitimately superseded the initial page after
        // deleting it. It becomes the new beginning of this modal run.
        m_firstpage = currentPage;
        TrackWizardFirstPage(this, currentPage);
    }

    const int modalResult = ShowModal();
    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    const WizardRunTerminalGuard terminalGuard(this);

    // GetCurrentPage() is specified to return nullptr once RunWizard() has
    // ended. This cleanup is also essential when cancellation invalidated a
    // ShowPage() frame after it published, but before it showed, its target.
    currentPage = wxWizard::GetCurrentPage();
    if ( currentPage )
    {
        currentPage->Hide();
        if ( !IsWizardWindowAlive(weakThis, this) )
            return false;

        if ( IsWizardWindowAlive(weakThis, this) )
            (void)SetCurrentPage(nullptr);
    }

    m_firstpage = nullptr;
    TrackWizardFirstPage(this, nullptr);
    return modalResult == wxID_OK;
}

wxWizardPage *wxWizard::GetCurrentPage() const
{
    WizardRuntimeState& runtimeState =
        GetWizardRuntimeState(const_cast<wxWizard*>(this));
    if ( runtimeState.currentPageAddress != m_page )
    {
        // m_page is protected for historical reasons. Late synchronization
        // keeps a derived class assigning a live page directly source-
        // compatible, while an expired tracked identity with the same address
        // is never resurrected (the ABA-safe address remains recorded).
        runtimeState.currentPageAddress = m_page;
        runtimeState.currentPageLifetime = wxWeakRef<wxWizardPage>(
            FindLiveWizardPageChildByAddress(
                const_cast<wxWizard*>(this), m_page));
    }

    wxWizardPage* const page = WizardPageLifetime(this).get();
    return page == m_page &&
                   wxWeakWindowIsAvailableForCallbacks(WizardPageLifetime(this),
                                                       page) &&
                   page->GetParent() == this
               ? page
               : nullptr;
}

wxSize wxWizard::GetPageSize() const
{
    const wxWeakRef<wxWindow> weakThis(const_cast<wxWizard*>(this));

    // default width and height of the page
    int DEFAULT_PAGE_WIDTH,
        DEFAULT_PAGE_HEIGHT;
    if ( wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA )
    {
        // Make the default page size small enough to fit on screen
        DEFAULT_PAGE_WIDTH = wxSystemSettings::GetMetric(wxSYS_SCREEN_X, m_parent) / 2;
        DEFAULT_PAGE_HEIGHT = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y, m_parent) / 2;
    }
    else // !PDA
    {
        DEFAULT_PAGE_WIDTH =
        DEFAULT_PAGE_HEIGHT = 270;
    }

    // start with default minimal size
    wxSize pageSize(DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT);

    // make the page at least as big as specified by user
    pageSize.IncTo(m_sizePage);

    if ( m_statbmp )
    {
        // make the page at least as tall as the bitmap
        const wxSize bitmapSize = m_bitmap.GetPreferredLogicalSizeFor(this);
        if ( !IsWizardWindowAlive(weakThis, this) )
            return pageSize;

        pageSize.IncTo(wxSize(0, bitmapSize.GetHeight()));
    }

    if ( m_usingSizer )
    {
        // make it big enough to contain all pages added to the sizer
        wxWizardSizer* const pageSizer = m_sizerPage;
        const wxSize maxChildSize = pageSizer->GetMaxChildSize();
        if ( !IsWizardWindowAlive(weakThis, this) ||
             m_sizerPage != pageSizer )
        {
            return pageSize;
        }

        pageSize.IncTo(maxChildSize);
    }

    return pageSize;
}

wxSizer *wxWizard::GetPageAreaSizer() const
{
    return m_sizerPage;
}

void wxWizard::SetBorder(int border)
{
    wxCHECK_RET(!m_started, wxT("wxWizard::SetBorder after RunWizard"));

    m_border = border;
}

void wxWizard::OnCancel(wxCommandEvent& WXUNUSED(eventUnused))
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) ||
         IsWizardRunEnding(this) )
        return;

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);

    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    wxWizardPage* const page = wxWizard::GetCurrentPage();
    if ( !page && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return;
        }
    }

    wxWindow* const win = page ? static_cast<wxWindow*>(page)
                               : static_cast<wxWindow*>(this);
    const wxWeakRef<wxWindow> weakPage(page);

    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId(), false, page);
    const bool processed = win->GetEventHandler()->ProcessEvent(event);

    if ( !IsWizardWindowAlive(weakThis, this) )
        return;

    if ( WizardNavigationGeneration(this) != navigationGeneration )
        return;

    if ( page && !IsWizardWindowAlive(weakPage, page) )
    {
        if ( m_page == page )
            SetCurrentPage(nullptr);
        return;
    }

    // A nested handler may have moved to another page.  In that case its
    // navigation decision supersedes this cancel request.
    if ( m_page != page )
        return;

    if ( !processed || event.IsAllowed() )
    {
        const WizardRunTerminalGuard terminalGuard(this);

        // Cancellation is a committed terminal transition. Invalidate every
        // ShowPage() frame still on the stack before ending/hiding the dialog
        // so none of them can continue with layout, focus or page-shown work.
        ++WizardNavigationGeneration(this);

        // no objections - close the dialog
        if ( IsModal() )
        {
            EndModal(wxID_CANCEL);
        }
        else
        {
            SetReturnCode(wxID_CANCEL);
            Hide();
        }
    }
    //else: request to Cancel ignored
}

void wxWizard::OnBackOrNext(wxCommandEvent& event)
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) ||
         IsWizardRunEnding(this) )
        return;

    wxObject* const eventObject = event.GetEventObject();
    wxCHECK_RET( eventObject == m_btnNext || eventObject == m_btnPrev,
                 wxT("unknown button") );

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);
    wxWizardPage* const currentPage = wxWizard::GetCurrentPage();
    if ( !currentPage && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return;
        }
    }

    wxCHECK_RET( currentPage, wxT("should have a valid current page") );

    const bool forward = eventObject == m_btnNext;
    const wxWeakRef<wxWindow> weakCurrentPage(currentPage);
    wxButton* const btnPrev = m_btnPrev;
    wxButton* const btnNext = m_btnNext;
    const wxWeakRef<wxWindow> weakBtnPrev(btnPrev);
    const wxWeakRef<wxWindow> weakBtnNext(btnNext);
#if wxUSE_STATBMP
    wxStaticBitmap* const statbmp = m_statbmp;
    const wxWeakRef<wxWindow> weakStatbmp(statbmp);
#endif

    const auto navigationStateIsValid =
        [this, currentPage, &weakThis, &weakCurrentPage,
         navigationGeneration,
         btnPrev, &weakBtnPrev, btnNext, &weakBtnNext
#if wxUSE_STATBMP
         , statbmp, &weakStatbmp
#endif
         ]()
        {
            if ( !IsWizardWindowAlive(weakThis, this) )
                return false;

            if ( WizardNavigationGeneration(this) != navigationGeneration )
                return false;

            if ( m_page != currentPage ||
                 WizardPageLifetime(this).get() != currentPage )
                return false;

            if ( !IsWizardWindowAlive(weakCurrentPage, currentPage) ||
                 currentPage->GetParent() != this )
            {
                SetCurrentPage(nullptr);
                return false;
            }

            if ( m_btnPrev != btnPrev || m_btnNext != btnNext )
                return false;

            if ( !IsWizardWindowAlive(weakBtnPrev, btnPrev) )
            {
                m_btnPrev = nullptr;
                return false;
            }

            if ( !IsWizardWindowAlive(weakBtnNext, btnNext) )
            {
                m_btnNext = nullptr;
                return false;
            }

#if wxUSE_STATBMP
            if ( m_statbmp != statbmp )
                return false;

            if ( statbmp && !IsWizardWindowAlive(weakStatbmp, statbmp) )
            {
                m_statbmp = nullptr;
                return false;
            }
#endif
            return true;
        };

    if ( !navigationStateIsValid() )
        return;

    // ask the current page first: notice that we do it before calling
    // GetNext/Prev() because the data transferred from the controls of the page
    // may change the value returned by these methods
    const bool isValid = currentPage->Validate();
    if ( !navigationStateIsValid() || !isValid )
        return;

    const bool transferred = currentPage->TransferDataFromWindow();
    if ( !navigationStateIsValid() || !transferred )
    {
        // the page data is incorrect, don't do anything
        return;
    }

    // Give the application a chance to set state which may influence GetNext()/GetPrev()
    wxWizardEvent eventPreChanged(wxEVT_WIZARD_BEFORE_PAGE_CHANGED, GetId(),
                                  forward, currentPage);
    (void)currentPage->GetEventHandler()->ProcessEvent(eventPreChanged);

    if ( !navigationStateIsValid() || !eventPreChanged.IsAllowed() )
        return;

    wxWizardPage* rawPage;
    if ( forward )
    {
        rawPage = currentPage->GetNext();
    }
    else // back
    {
        rawPage = currentPage->GetPrev();
    }

    if ( !navigationStateIsValid() )
        return;

    if ( !forward && !rawPage )
    {
        // The dynamic chain may have changed since the button was enabled.
        return;
    }

    wxWizardPage* const page = rawPage
                                   ? FindLiveWizardPageChildByAddress(this,
                                                                      rawPage)
                                   : nullptr;
    if ( rawPage && !page )
        return;

    if ( page )
    {
        const wxWeakRef<wxWindow> weakPage(page);
        if ( !IsWizardWindowAlive(weakPage, page) ||
             page->GetParent() != this )
        {
            return;
        }
    }

    // just pass to the new page (or maybe not - but we don't care here)
    (void)ShowPage(page, forward);
}

void wxWizard::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) ||
         IsWizardRunEnding(this) )
        return;

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);

    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    wxWizardPage* const page = wxWizard::GetCurrentPage();
    if ( !page && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return;
        }
    }

    if ( page )
    {
        const wxWeakRef<wxWindow> weakPage(page);
        if ( !IsWizardWindowAlive(weakPage, page) ||
             page->GetParent() != this )
        {
            if ( m_page == page )
                SetCurrentPage(nullptr);
            return;
        }

        // Create and send the help event to the specific page handler
        // event data contains the active page so that context-sensitive
        // help is possible
        wxWizardEvent eventHelp(wxEVT_WIZARD_HELP, GetId(), true, page);
        (void)page->GetEventHandler()->ProcessEvent(eventHelp);

        if ( IsWizardWindowAlive(weakThis, this) &&
             m_page == page &&
             !IsWizardWindowAlive(weakPage, page) )
        {
            SetCurrentPage(nullptr);
        }
    }
}

void wxWizard::OnWizEvent(wxWizardEvent& event)
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) )
        return;

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);
    wxWizardPage* const pageAtEntry = wxWizard::GetCurrentPage();
    if ( !pageAtEntry && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return;
        }
    }

    const wxWeakRef<wxWindow> weakPageAtEntry(pageAtEntry);

    // the dialogs have wxWS_EX_BLOCK_EVENTS style on by default but we want to
    // propagate wxEVT_WIZARD_XXX to the parent (if any), so do it manually
    if ( !(GetExtraStyle() & wxWS_EX_BLOCK_EVENTS) )
    {
        // the event will be propagated anyhow
        event.Skip();
    }
    else
    {
        wxWindow *parent = GetParent();
        const wxWeakRef<wxWindow> weakParent(parent);

        if ( !parent ||
             !wxWeakWindowIsAvailableForCallbacks(weakParent, parent) ||
             !parent->GetEventHandler()->ProcessEvent(event) )
        {
            event.Skip();
        }
    }

    if ( !IsWizardWindowAlive(weakThis, this) )
        return;

    if ( WizardNavigationGeneration(this) != navigationGeneration )
        return;

    if ( pageAtEntry && m_page == pageAtEntry &&
         !IsWizardWindowAlive(weakPageAtEntry, pageAtEntry) )
    {
        SetCurrentPage(nullptr);
    }

    if ( ( !m_wasModal ) &&
         event.IsAllowed() &&
         ( event.GetEventType() == wxEVT_WIZARD_FINISHED ||
           event.GetEventType() == wxEVT_WIZARD_CANCEL
         )
       )
    {
        Destroy();
    }
}

void wxWizard::SetBitmap(const wxBitmapBundle& bitmap)
{
    const wxWeakRef<wxWindow> weakThis(this);
    const unsigned long navigationGeneration = WizardNavigationGeneration(this);
    wxWizardPage* const page = wxWizard::GetCurrentPage();
    if ( !page && m_page )
    {
        (void)SetCurrentPage(nullptr);
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration )
        {
            return;
        }
    }

    const wxWeakRef<wxWindow> weakPage(page);

    m_bitmap = bitmap;
    wxStaticBitmap* const statbmp = m_statbmp;
    const wxWeakRef<wxWindow> weakStatbmp(statbmp);
    if ( statbmp )
        statbmp->SetBitmap(m_bitmap);

    if ( !IsWizardWindowAlive(weakThis, this) )
        return;

    if ( statbmp && m_statbmp == statbmp &&
         !IsWizardWindowAlive(weakStatbmp, statbmp) )
    {
        m_statbmp = nullptr;
    }

    if ( page && m_page == page &&
         !IsWizardWindowAlive(weakPage, page) )
    {
        SetCurrentPage(nullptr);
    }
}

void wxWizard::WXHandleDPIChanged(wxDPIChangedEvent& event)
{
    const wxWeakRef<wxWindow> weakThis(this);
    const unsigned long navigationGeneration = WizardNavigationGeneration(this);

    if ( m_statbmp )
    {
        wxStaticBitmap* const statbmp = m_statbmp;
        const wxWeakRef<wxWindow> weakStatbmp(statbmp);
        wxWizardPage* const page = wxWizard::GetCurrentPage();
        if ( !page && m_page )
        {
            (void)SetCurrentPage(nullptr);
            if ( !IsWizardWindowAlive(weakThis, this) ||
                 WizardNavigationGeneration(this) != navigationGeneration )
            {
                event.Skip();
                return;
            }
        }

        const wxWeakRef<wxWindow> weakPage(page);
        const auto dpiStateIsValid =
            [this, page, &weakPage, statbmp, &weakStatbmp, &weakThis,
             navigationGeneration]()
            {
                if ( !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration )
                    return false;

                if ( page &&
                     (!IsWizardWindowAlive(weakPage, page) ||
                      WizardPageLifetime(this).get() != page ||
                      page->GetParent() != this) )
                {
                    if ( m_page == page )
                        SetCurrentPage(nullptr);
                    return false;
                }

                return m_page == page &&
                       (!page || WizardPageLifetime(this).get() == page) &&
                       m_statbmp == statbmp &&
                       IsWizardWindowAlive(weakStatbmp, statbmp);
            };

        wxBitmap bmp;
        if ( page )
        {
            bmp = page->GetBitmap();
            if ( !dpiStateIsValid() )
            {
                event.Skip();
                return;
            }
        }

        if ( !bmp.IsOk() )
        {
            bmp = m_bitmap.GetBitmapFor(this);
            if ( !dpiStateIsValid() )
            {
                event.Skip();
                return;
            }
        }

        (void)ResizeBitmap(bmp);
        if ( !dpiStateIsValid() )
        {
            event.Skip();
            return;
        }

        statbmp->SetBitmap(bmp);
        (void)dpiStateIsValid();
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxWizardEvent
// ----------------------------------------------------------------------------

wxWizardEvent::wxWizardEvent(wxEventType type, int id, bool direction, wxWizardPage* page)
             : wxNotifyEvent(type, id)
{
    // Modified 10-20-2001 Robert Cavanaugh
    // add the active page to the event data
    m_direction = direction;
    m_page = page;
}

/// Do the adaptation
bool wxWizard::DoLayoutAdaptation()
{
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !IsWizardWindowAlive(weakThis, this) )
        return false;

    // GetNext(), Reparent(), SetSizer() and Layout() below are all virtual or
    // dispatch native events.  A nested ShowPage() must not start a second
    // adaptation transaction and publish its completion flag while the outer
    // transaction is still capable of rolling back.
    if ( !gs_wizardsAdaptingLayout.insert(this).second )
        return false;

    wxScopeGuard leaveLayoutAdaptation = wxMakeGuard(
        [this]() { gs_wizardsAdaptingLayout.erase(this); });
    wxUnusedVar(leaveLayoutAdaptation);

    const unsigned long navigationGeneration = WizardNavigationGeneration(this);
    wxWizardSizer* const pageSizer = m_sizerPage;
    if ( !pageSizer )
        return false;

    // Snapshot the roots before invoking GetNext() or any layout operation:
    // callbacks are allowed to mutate the page-area sizer itself.
    std::vector<wxWeakRef<wxWindow>> roots;
    for ( wxSizerItemList::compatibility_iterator node =
              pageSizer->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxSizerItem* const item = node->GetData();
        if ( item->IsWindow() )
        {
            if ( wxWizardPage* const page =
                     wxDynamicCast(item->GetWindow(), wxWizardPage) )
            {
                roots.emplace_back(page);
            }
        }
    }

    wxWindowList windows;
    std::vector<wxWeakRef<wxWindow>> scrollingWindows;
    std::unordered_set<wxWizardPage*> pages;

    struct AdaptedPage
    {
        AdaptedPage(wxWizardPage* page_,
                    wxScrolledWindow* scrolledWindow_,
                    wxSizer* oldSizer_,
                    wxSizer* newSizer_)
            : page(page_),
              weakPage(page_),
              scrolledWindow(scrolledWindow_),
              weakScrolledWindow(scrolledWindow_),
              oldSizer(oldSizer_),
              newSizer(newSizer_)
        {
        }

        wxWizardPage* page;
        wxWeakRef<wxWindow> weakPage;
        wxScrolledWindow* scrolledWindow;
        wxWeakRef<wxWindow> weakScrolledWindow;
        wxSizer* oldSizer;
        wxSizer* newSizer;
    };

    std::vector<AdaptedPage> adaptedPages;
    bool adaptationCommitted = false;
    wxScopeGuard rollbackAdaptation = wxMakeGuard(
        [&adaptedPages, &adaptationCommitted]()
        {
            if ( adaptationCommitted )
                return;

            for ( std::vector<AdaptedPage>::reverse_iterator it =
                      adaptedPages.rbegin();
                  it != adaptedPages.rend();
                  ++it )
            {
                AdaptedPage& adapted = *it;
                if ( !IsWizardWindowAlive(adapted.weakPage, adapted.page) ||
                     !IsWizardWindowAlive(adapted.weakScrolledWindow,
                                          adapted.scrolledWindow) ||
                     adapted.scrolledWindow->GetParent() != adapted.page ||
                     adapted.page->GetSizer() != adapted.newSizer ||
                     adapted.scrolledWindow->GetSizer() != adapted.oldSizer )
                {
                    continue;
                }

                wxStandardDialogLayoutAdapter::DoReparentControls(
                    adapted.scrolledWindow, adapted.page);
                if ( !IsWizardWindowAlive(adapted.weakPage, adapted.page) ||
                     !IsWizardWindowAlive(adapted.weakScrolledWindow,
                                          adapted.scrolledWindow) ||
                     adapted.scrolledWindow->GetParent() != adapted.page ||
                     adapted.page->GetSizer() != adapted.newSizer ||
                     adapted.scrolledWindow->GetSizer() != adapted.oldSizer ||
                     !SizerWindowsBelongTo(adapted.oldSizer, adapted.page) )
                {
                    continue;
                }

                if ( adapted.scrolledWindow->GetSizer() == adapted.oldSizer )
                    adapted.scrolledWindow->SetSizer(nullptr, false);

                if ( !IsWizardWindowAlive(adapted.weakPage, adapted.page) ||
                     !IsWizardWindowAlive(adapted.weakScrolledWindow,
                                          adapted.scrolledWindow) )
                {
                    continue;
                }

                if ( adapted.page->GetSizer() == adapted.newSizer )
                    adapted.page->SetSizer(adapted.oldSizer, true);

                if ( IsWizardWindowAlive(adapted.weakScrolledWindow,
                                         adapted.scrolledWindow) &&
                     adapted.scrolledWindow->GetParent() == adapted.page )
                {
                    adapted.scrolledWindow->Destroy();
                }
            }

        });
    wxUnusedVar(rollbackAdaptation);

    // Make all the pages (that use sizers) scrollable
    for ( const wxWeakRef<wxWindow>& weakRoot : roots )
    {
        if ( !IsWizardWindowAlive(weakThis, this) ||
             WizardNavigationGeneration(this) != navigationGeneration ||
             m_sizerPage != pageSizer )
        {
            return false;
        }

        wxWindow* const root = weakRoot.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakRoot, root) )
            return false;

        wxWizardPage* page = wxDynamicCast(root, wxWizardPage);
        while ( page && pages.insert(page).second )
        {
            const wxWeakRef<wxWindow> weakPage(page);
            if ( !IsWizardWindowAlive(weakPage, page) ||
                 page->GetParent() != this )
            {
                return false;
            }

            wxSizer* const oldSizer = page->GetSizer();
            if ( oldSizer )
            {
                wxScrolledWindow* const scrolledWindow = new wxScrolledWindow;
                const wxWeakRef<wxWindow> weakScrolledWindow(scrolledWindow);
                const bool created = scrolledWindow->Create(
                    page,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxTAB_TRAVERSAL | wxVSCROLL | wxHSCROLL | wxBORDER_NONE);

                if ( !created )
                {
                    if ( IsWizardWindowAlive(weakScrolledWindow,
                                              scrolledWindow) )
                    {
                        delete scrolledWindow;
                    }
                    return false;
                }

                if ( !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration ||
                     m_sizerPage != pageSizer ||
                     !IsWizardWindowAlive(weakPage, page) ||
                     page->GetParent() != this ||
                     !IsWizardWindowAlive(weakScrolledWindow,
                                          scrolledWindow) ||
                     scrolledWindow->GetParent() != page )
                {
                    if ( IsWizardWindowAlive(weakScrolledWindow,
                                              scrolledWindow) )
                    {
                        if ( scrolledWindow->GetParent() == page )
                            scrolledWindow->Destroy();
                        else
                            delete scrolledWindow;
                    }
                    return false;
                }

                // Creating the child may itself run native callbacks.  Do not
                // move a sizer that was replaced while this happened.
                if ( page->GetSizer() != oldSizer )
                {
                    scrolledWindow->Destroy();
                    return false;
                }

                wxSizer* const newSizer = new wxBoxSizer(wxVERTICAL);
                newSizer->Add(scrolledWindow, 1, wxEXPAND, 0);

                bool controlsMayHaveMoved = false;
                bool newSizerNeedsDelete = true;
                bool pageAdaptationCommitted = false;
                wxScopeGuard rollbackPageAdaptation = wxMakeGuard(
                    [this, page, &weakPage, scrolledWindow,
                     &weakScrolledWindow, oldSizer, newSizer,
                     &controlsMayHaveMoved, &newSizerNeedsDelete,
                     &pageAdaptationCommitted,
                     &weakThis]()
                    {
                        if ( pageAdaptationCommitted )
                            return;

                        if ( !IsWizardWindowAlive(weakThis, this) ||
                             !IsWizardWindowAlive(weakPage, page) ||
                             !IsWizardWindowAlive(weakScrolledWindow,
                                                  scrolledWindow) )
                        {
                            // If publication never happened, no window owns
                            // newSizer. An ancestor entering deferred teardown
                            // makes its children unavailable for callbacks but
                            // does not transfer ownership of this allocation.
                            if ( newSizerNeedsDelete )
                            {
                                delete newSizer;
                                newSizerNeedsDelete = false;
                            }
                            return;
                        }

                        if ( controlsMayHaveMoved )
                        {
                            // This operation only walks the provisional
                            // window tree. Do it even if an override replaced
                            // and deleted oldSizer during the forward move: it
                            // gives application controls the best chance to
                            // return to their page without touching that
                            // potentially stale sizer address.
                            wxStandardDialogLayoutAdapter::DoReparentControls(
                                scrolledWindow, page);
                        }

                        if ( !IsWizardWindowAlive(weakThis, this) ||
                             !IsWizardWindowAlive(weakPage, page) ||
                             !IsWizardWindowAlive(weakScrolledWindow,
                                                  scrolledWindow) )
                        {
                            return;
                        }

                        if ( page->GetSizer() != oldSizer &&
                             scrolledWindow->GetSizer() != oldSizer )
                        {
                            // Pointer equality with a currently published
                            // sizer is the only ownership proof we have. Never
                            // traverse oldSizer otherwise. Also never destroy
                            // a provisional scroller which still owns an
                            // application child after a refused inverse move.
                            if ( newSizerNeedsDelete )
                            {
                                delete newSizer;
                                newSizerNeedsDelete = false;
                            }
                            if ( scrolledWindow->GetParent() == page &&
                                 scrolledWindow->GetChildren().empty() )
                            {
                                scrolledWindow->Destroy();
                            }
                            return;
                        }

                        if ( !SizerWindowsBelongTo(oldSizer, page) )
                        {
                            // A user override may refuse the inverse Reparent.
                            // The common reparent transaction restores all
                            // controls to the scroller in this case. Preserve
                            // that coherent adapted topology; publishing the
                            // old sizer back on page would make it reference
                            // children of a soon-to-be-destroyed scroller.
                            if ( SizerWindowsBelongTo(oldSizer,
                                                      scrolledWindow) )
                            {
                                if ( page->GetSizer() == oldSizer &&
                                     !scrolledWindow->GetSizer() )
                                {
                                    page->SetSizer(newSizer, false);
                                    newSizerNeedsDelete = false;
                                    if ( !IsWizardWindowAlive(weakPage, page) ||
                                         !IsWizardWindowAlive(
                                             weakScrolledWindow,
                                             scrolledWindow) ||
                                         page->GetSizer() != newSizer )
                                    {
                                        return;
                                    }

                                    scrolledWindow->SetSizer(oldSizer);
                                }

                                if ( page->GetSizer() == newSizer &&
                                     scrolledWindow->GetSizer() == oldSizer )
                                {
                                    newSizerNeedsDelete = false;
                                }
                            }
                            return;
                        }

                        if ( scrolledWindow->GetSizer() == oldSizer )
                            scrolledWindow->SetSizer(nullptr, false);

                        if ( page->GetSizer() == newSizer )
                        {
                            page->SetSizer(oldSizer, true);
                            newSizerNeedsDelete = false;
                        }
                        else if ( newSizerNeedsDelete )
                        {
                            delete newSizer;
                            newSizerNeedsDelete = false;
                        }

                        if ( IsWizardWindowAlive(weakScrolledWindow,
                                                 scrolledWindow) )
                        {
                            scrolledWindow->Destroy();
                        }
                    });
                wxUnusedVar(rollbackPageAdaptation);

                // Move the controls before publishing oldSizer on the new
                // containing window. wxSizer::SetContainingWindow() verifies
                // parentage immediately, so doing this in the opposite order
                // both asserts and makes a refused Reparent() impossible to
                // roll back transactionally.
                controlsMayHaveMoved = true;
                wxStandardDialogLayoutAdapter::DoReparentControls(
                    page, scrolledWindow);
                if ( !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration ||
                     m_sizerPage != pageSizer ||
                     !IsWizardWindowAlive(weakPage, page) ||
                     page->GetParent() != this ||
                     !IsWizardWindowAlive(weakScrolledWindow,
                                          scrolledWindow) ||
                     page->GetSizer() != oldSizer ||
                     !SizerWindowsBelongTo(oldSizer, scrolledWindow) )
                {
                    return false;
                }

                page->SetSizer(newSizer,
                               false /* don't delete the old sizer */);
                newSizerNeedsDelete = false;
                if ( !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration ||
                     m_sizerPage != pageSizer ||
                     !IsWizardWindowAlive(weakPage, page) ||
                     page->GetParent() != this ||
                     !IsWizardWindowAlive(weakScrolledWindow,
                                          scrolledWindow) ||
                     page->GetSizer() != newSizer )
                {
                    return false;
                }

                scrolledWindow->SetSizer(oldSizer);
                if ( !IsWizardWindowAlive(weakThis, this) ||
                     WizardNavigationGeneration(this) != navigationGeneration ||
                     m_sizerPage != pageSizer ||
                     !IsWizardWindowAlive(weakPage, page) ||
                     page->GetParent() != this ||
                     !IsWizardWindowAlive(weakScrolledWindow,
                                          scrolledWindow) ||
                     page->GetSizer() != newSizer ||
                     scrolledWindow->GetSizer() != oldSizer ||
                     !SizerWindowsBelongTo(oldSizer, scrolledWindow) )
                {
                    return false;
                }

                windows.Append(scrolledWindow);
                scrollingWindows.emplace_back(scrolledWindow);
                adaptedPages.emplace_back(page,
                                          scrolledWindow,
                                          oldSizer,
                                          newSizer);
                pageAdaptationCommitted = true;
            }

            wxWizardPage* const current = page;
            wxWizardPage* const rawNext = page->GetNext();
            if ( !IsWizardWindowAlive(weakThis, this) ||
                 WizardNavigationGeneration(this) != navigationGeneration ||
                 m_sizerPage != pageSizer ||
                 !IsWizardWindowAlive(weakPage, current) ||
                 current->GetParent() != this )
            {
                return false;
            }

            page = rawNext
                       ? FindLiveWizardPageChildByAddress(this, rawNext)
                       : nullptr;
            if ( rawNext && !page )
                return false;
        }
    }

    // Never pass a stale raw pointer from windows to the generic adapter: a
    // callback run while adapting a later page may have destroyed one of the
    // scrolled windows recorded above.
    for ( const wxWeakRef<wxWindow>& weakWindow : scrollingWindows )
    {
        wxWindow* const window = weakWindow.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakWindow, window) )
            return false;
    }

    const bool fitSucceeded =
        wxStandardDialogLayoutAdapter::DoFitWithScrolling(this, windows);
    if ( !IsWizardWindowAlive(weakThis, this) ||
         WizardNavigationGeneration(this) != navigationGeneration ||
         m_sizerPage != pageSizer ||
         !fitSucceeded )
    {
        return false;
    }

    for ( const wxWeakRef<wxWindow>& weakWindow : scrollingWindows )
    {
        wxWindow* const window = weakWindow.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakWindow, window) )
            return false;
    }

    // Size event doesn't get sent soon enough on wxGTK
    Layout();
    if ( !IsWizardWindowAlive(weakThis, this) ||
         WizardNavigationGeneration(this) != navigationGeneration ||
         m_sizerPage != pageSizer )
        return false;

    // Layout() dispatches size callbacks. Validate the complete committed
    // topology again before suppressing the rollback transaction.
    for ( const AdaptedPage& adapted : adaptedPages )
    {
        if ( !IsWizardWindowAlive(adapted.weakPage, adapted.page) ||
             adapted.page->GetParent() != this ||
             adapted.page->GetSizer() != adapted.newSizer ||
             !IsWizardWindowAlive(adapted.weakScrolledWindow,
                                  adapted.scrolledWindow) ||
              adapted.scrolledWindow->GetParent() != adapted.page ||
              adapted.scrolledWindow->GetSizer() != adapted.oldSizer ||
              !SizerWindowsBelongTo(adapted.oldSizer,
                                    adapted.scrolledWindow) )
        {
            return false;
        }
    }

    SetLayoutAdaptationDone(true);
    adaptationCommitted = true;

    return true;
}

bool wxWizard::ResizeBitmap(wxBitmap& bmp)
{
    if ( !GetBitmapPlacement() || !m_sizerPage || !m_statbmp )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxStaticBitmap* const statbmp = m_statbmp;
    const wxWeakRef<wxWindow> weakStatbmp(statbmp);

    if (bmp.IsOk())
    {
        wxSize pageSize = m_sizerPage->GetSize();
        if (pageSize == wxSize(0,0))
        {
            pageSize = GetPageSize();
            if ( !IsWizardWindowAlive(weakThis, this) ||
                 m_statbmp != statbmp ||
                 !IsWizardWindowAlive(weakStatbmp, statbmp) )
            {
                return false;
            }
        }
        int bitmapWidth = wxMax(bmp.GetLogicalWidth(), GetMinimumBitmapWidth());
        int bitmapHeight = pageSize.y;

        const wxBitmap currentBitmap = statbmp->GetBitmap();
        if ( !IsWizardWindowAlive(weakThis, this) ||
             m_statbmp != statbmp ||
             !IsWizardWindowAlive(weakStatbmp, statbmp) )
        {
            return false;
        }

        if (!currentBitmap.IsOk() ||
            currentBitmap.GetLogicalHeight() != bitmapHeight)
        {
            wxBitmap bitmap;
            bitmap.CreateWithLogicalSize(bitmapWidth, bitmapHeight, bmp.GetScaleFactor(), bmp.GetDepth());
            {
                wxMemoryDC dc;
                dc.SelectObject(bitmap);
                dc.SetBackground(wxBrush(m_bitmapBackgroundColour));
                dc.Clear();

                if (GetBitmapPlacement() & wxWIZARD_TILE)
                {
                    TileBitmap(wxRect(0, 0, bitmapWidth, bitmapHeight), dc, bmp);
                }
                else
                {
                    int x, y;

                    if (GetBitmapPlacement() & wxWIZARD_HALIGN_LEFT)
                        x = 0;
                    else if (GetBitmapPlacement() & wxWIZARD_HALIGN_RIGHT)
                        x = bitmapWidth - bmp.GetLogicalWidth();
                    else
                        x = (bitmapWidth - bmp.GetLogicalWidth())/2;

                    if (GetBitmapPlacement() & wxWIZARD_VALIGN_TOP)
                        y = 0;
                    else if (GetBitmapPlacement() & wxWIZARD_VALIGN_BOTTOM)
                        y = bitmapHeight - bmp.GetLogicalHeight();
                    else
                        y = (bitmapHeight - bmp.GetLogicalHeight())/2;

                    dc.DrawBitmap(bmp, x, y, true);
                    dc.SelectObject(wxNullBitmap);
                }
            }

            bmp = bitmap;
        }
    }

    return true;
}

bool wxWizard::TileBitmap(const wxRect& rect, wxDC& dc, const wxBitmap& bitmap)
{
    if ( !bitmap.IsOk() )
        return false;

    int w = bitmap.GetLogicalWidth();
    int h = bitmap.GetLogicalHeight();

    if ( w <= 0 || h <= 0 )
        return false;

    wxMemoryDC dcMem;

    dcMem.SelectObjectAsSource(bitmap);

    int i, j;
    for (i = rect.x; i < rect.x + rect.width; i += w)
    {
        for (j = rect.y; j < rect.y + rect.height; j+= h)
            dc.Blit(i, j, bitmap.GetLogicalWidth(), bitmap.GetLogicalHeight(), & dcMem, 0, 0);
    }
    dcMem.SelectObject(wxNullBitmap);

    return true;
}

#endif // wxUSE_WIZARDDLG
