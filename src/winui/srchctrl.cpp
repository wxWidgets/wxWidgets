/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/srchctrl.cpp
// Purpose:     wxWinUI wxSearchCtrl implementation (WinUI AutoSuggestBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"
#include "wx/app.h"

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif // wxUSE_MENUS

#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/winui/private/textpeer.h"

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

long wxWinUIClampPos(long pos, long len)
{
    if ( pos < 0 || pos > len )
        return len;
    return pos;
}

MUXC::TextBox wxWinUIFindSearchTextBox(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::TextBox textBox = root.try_as<MUXC::TextBox>() )
        return textBox;

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const MUXC::TextBox textBox =
                 wxWinUIFindSearchTextBox(
                     MUXM::VisualTreeHelper::GetChild(root, i)) )
        {
            return textBox;
        }
    }
    return nullptr;
}

MUXC::Button wxWinUIFindSearchDeleteButton(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::Button button = root.try_as<MUXC::Button>() )
    {
        if ( const MUX::FrameworkElement element =
                 button.try_as<MUX::FrameworkElement>() )
        {
            const wxString name =
                wxWinUIFromHString(element.Name()).Lower();
            if ( name.Contains(wxS("delete")) ||
                 name.Contains(wxS("clear")) )
            {
                return button;
            }
        }
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const MUXC::Button button =
                 wxWinUIFindSearchDeleteButton(
                     MUXM::VisualTreeHelper::GetChild(root, i)) )
        {
            return button;
        }
    }
    return nullptr;
}

MUXC::Button wxWinUIFindSearchQueryButton(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::Button button = root.try_as<MUXC::Button>() )
    {
        if ( const MUX::FrameworkElement element =
                 button.try_as<MUX::FrameworkElement>() )
        {
            const wxString name =
                wxWinUIFromHString(element.Name()).Lower();
            if ( name.Contains(wxS("query")) ||
                 name.Contains(wxS("search")) )
            {
                return button;
            }
        }
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const MUXC::Button button =
                 wxWinUIFindSearchQueryButton(
                     MUXM::VisualTreeHelper::GetChild(root, i)) )
        {
            return button;
        }
    }
    return nullptr;
}

} // namespace

class wxWinUISearchPartRefreshState final
{
public:
    bool active = true;
    bool resolving = false;
    bool pending = false;
};

class wxWinUISearchCtrlImpl
{
public:
    ~wxWinUISearchCtrlImpl()
    {
        Close();
    }

    void Close()
    {
        if ( partRefreshState )
        {
            partRefreshState->active = false;
            partRefreshState->pending = false;
        }

        if ( callbackState )
            callbackState->Invalidate();

        if ( editBox && selectionChangedToken.value )
        {
            try
            {
                editBox.SelectionChanged(selectionChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SearchCtrl selection removal", e);
            }
        }
        selectionChangedToken = {};

        if ( deleteButton && deleteClickToken.value )
        {
            try
            {
                deleteButton.Click(deleteClickToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SearchCtrl cancel removal", e);
            }
        }
        deleteClickToken = {};

        if ( queryButton && queryClickToken.value )
        {
            try
            {
                queryButton.Click(queryClickToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SearchCtrl query-button removal", e);
            }
        }
        queryClickToken = {};

        if ( box )
        {
            if ( textChangedToken.value )
            {
                try
                {
                    box.TextChanged(textChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl TextChanged removal", e);
                }
            }
            if ( querySubmittedToken.value )
            {
                try
                {
                    box.QuerySubmitted(querySubmittedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl QuerySubmitted removal", e);
                }
            }
            if ( loadedToken.value )
            {
                try
                {
                    box.Loaded(loadedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl Loaded removal", e);
                }
            }
            if ( layoutUpdatedToken.value )
            {
                try
                {
                    box.LayoutUpdated(layoutUpdatedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl LayoutUpdated removal", e);
                }
            }
        }
        textChangedToken = {};
        querySubmittedToken = {};
        loadedToken = {};
        layoutUpdatedToken = {};

        host.Close();
        retiredQueryButtonForTesting = nullptr;
        injectedQueryButtonForTesting = nullptr;
        injectedDeleteButtonForTesting = nullptr;
        injectedEditBoxForTesting = nullptr;
        useInjectedPartsForTesting = false;
        queryButton = nullptr;
        deleteButton = nullptr;
        editBox = nullptr;
        box = nullptr;
        callbackState.reset();
        partRefreshState.reset();
        hasPendingPeerValue = false;
        pendingPeerValue.clear();
        suggestionCount = 0;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUITextCallbackState> callbackState;
    std::shared_ptr<wxWinUISearchPartRefreshState> partRefreshState =
        std::make_shared<wxWinUISearchPartRefreshState>();
    MUXC::AutoSuggestBox box{ nullptr };
    MUXC::TextBox editBox{ nullptr };
    MUXC::Button deleteButton{ nullptr };
    MUXC::Button queryButton{ nullptr };
    winrt::event_token textChangedToken{};
    winrt::event_token querySubmittedToken{};
    winrt::event_token loadedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token selectionChangedToken{};
    winrt::event_token deleteClickToken{};
    winrt::event_token queryClickToken{};
    wxString pendingPeerValue;
    bool hasPendingPeerValue = false;
    std::uint64_t editGeneration = 0;
    std::uint64_t deleteGeneration = 0;
    std::uint64_t queryGeneration = 0;
    unsigned suggestionCount = 0;
    MUXC::TextBox injectedEditBoxForTesting{ nullptr };
    MUXC::Button injectedDeleteButtonForTesting{ nullptr };
    MUXC::Button injectedQueryButtonForTesting{ nullptr };
    MUXC::Button retiredQueryButtonForTesting{ nullptr };
    bool useInjectedPartsForTesting = false;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase);

wxSearchCtrl::wxSearchCtrl()
{
}

wxSearchCtrl::wxSearchCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
                           const wxPoint& pos, const wxSize& size, long style,
                           const wxValidator& validator, const wxString& name)
{
    Create(parent, id, value, pos, size, style, validator, name);
}

wxSearchCtrl::~wxSearchCtrl()
{
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }
#if wxUSE_MENUS
    delete m_menu;
    m_menu = nullptr;
#endif // wxUSE_MENUS
}

void wxSearchCtrl::SetFocus()
{
    const wxWeakRef<wxWindow> self(this);
    wxWinUISearchCtrlImpl * const impl = m_winui.get();

    // Record/supersede the logical intent before any template operation:
    // ApplyTemplate()/UpdateLayout() may synchronously raise Loaded, which
    // must never replay an older slot's deferred request.
    wxControl::SetFocus();

    if ( !self || !m_winui || m_winui.get() != impl )
        return;

    // Resolve only this control's preferred edit part after its intent is
    // authoritative, then consume the single host retry before returning.
    ResolvePeerParts();
    if ( !self || !m_winui || m_winui.get() != impl )
        return;

    m_winui->host.SynchronizeForFocus();
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id, const wxString& value,
                          const wxPoint& pos, const wxSize& size, long style,
                          const wxValidator& validator, const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = wxWinUITextPositionMap::NormalizeNewlines(value);
    m_insertionPoint = m_selectionStart = m_selectionEnd = m_value.length();
    m_editable = (style & wxTE_READONLY) == 0;

    m_winui.reset(new wxWinUISearchCtrlImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUITextCallbackState>(this);
    wxWinUISearchCtrlImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> createState =
        createImpl->callbackState;
    const std::uint64_t createGeneration =
        createState->GetGeneration();
    const auto getLiveOwner =
        [createState, createGeneration,
         createImpl]() -> wxSearchCtrl *
        {
            if ( createState->GetGeneration() != createGeneration )
                return nullptr;

            wxSearchCtrl * const owner =
                createState->GetOwner<wxSearchCtrl>();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != createImpl ||
                 owner->m_winui->callbackState != createState )
            {
                return nullptr;
            }
            return owner;
        };
    const auto closeLiveOwner =
        [&getLiveOwner]()
        {
            if ( wxSearchCtrl * const owner = getLiveOwner() )
            {
                owner->m_winui->Close();
                owner->m_winui.reset();
            }
        };

    if ( !createImpl->host.Initialize(this) )
    {
        closeLiveOwner();
        return false;
    }

    wxSearchCtrl *liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

    const WinUICreateLoadedHookForTesting loadedHook =
        liveOwner->m_nextCreateLoadedHookForTesting;
    void * const loadedHookContext =
        liveOwner->m_nextCreateLoadedContextForTesting;
    liveOwner->m_nextCreateLoadedHookForTesting = nullptr;
    liveOwner->m_nextCreateLoadedContextForTesting = nullptr;
    if ( loadedHook )
    {
        createImpl->host.SetNextContentLoadedHookForTesting(
            [createState, createGeneration, createImpl,
             loadedHook, loadedHookContext]()
            {
                if ( createState->GetGeneration() != createGeneration )
                    return;

                wxSearchCtrl * const owner =
                    createState->GetOwner<wxSearchCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != createState )
                {
                    return;
                }
                loadedHook(owner, loadedHookContext);
            });
    }

    try
    {
        createImpl->box = MUXC::AutoSuggestBox();
        createImpl->box.QueryIcon(MUXC::SymbolIcon(winrt::Microsoft::UI::Xaml::Controls::Symbol::Find));

        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            createImpl->callbackState;
        createImpl->textChangedToken = createImpl->box.TextChanged(
            [callbackState](
                MUXC::AutoSuggestBox const& sender,
                MUXC::AutoSuggestBoxTextChangedEventArgs const& args)
            {
                wxSearchCtrl * const owner =
                    callbackState->GetOwner<wxSearchCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }

                wxWinUISearchCtrlImpl * const impl =
                    owner->m_winui.get();
                const wxString peerValue =
                    wxWinUITextPositionMap::NormalizeNewlines(
                        wxWinUIFromHString(sender.Text()));
                if ( impl->hasPendingPeerValue &&
                     impl->pendingPeerValue == peerValue )
                {
                    impl->hasPendingPeerValue = false;
                    impl->pendingPeerValue.clear();
                    return;
                }
                impl->hasPendingPeerValue = false;
                impl->pendingPeerValue.clear();

                if ( owner->m_updatingPeer )
                    return;

                if ( !owner->m_editable )
                {
                    owner->ApplyValueToPeer();
                    return;
                }

                owner->m_value = peerValue;
                owner->ReadSelectionFromPeer();
                owner->ApplySuggestions(owner->m_suggestions);

                if ( args.Reason() !=
                     MUXC::AutoSuggestionBoxTextChangeReason::
                         ProgrammaticChange )
                {
                    wxCommandEvent event(wxEVT_TEXT, owner->GetId());
                    event.SetEventObject(owner);
                    event.SetString(owner->m_value);
                    owner->ProcessCommand(event);
                }
            });

        createImpl->querySubmittedToken = createImpl->box.QuerySubmitted(
            [callbackState](
                MUXC::AutoSuggestBox const&,
                MUXC::AutoSuggestBoxQuerySubmittedEventArgs const& args)
            {
                wxSearchCtrl * const owner =
                    callbackState->GetOwner<wxSearchCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                owner->m_value =
                    wxWinUITextPositionMap::NormalizeNewlines(
                        wxWinUIFromHString(args.QueryText()));
                owner->m_insertionPoint =
                    owner->m_selectionStart =
                    owner->m_selectionEnd =
                        static_cast<long>(owner->m_value.length());

                wxCommandEvent event(wxEVT_SEARCH, owner->GetId());
                event.SetEventObject(owner);
                event.SetString(owner->m_value);
                owner->ProcessCommand(event);
            });

        createImpl->loadedToken = createImpl->box.Loaded(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxSearchCtrl * const owner =
                    callbackState->GetOwner<wxSearchCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }

                owner->ResolvePeerParts();
                wxSearchCtrl * const liveOwner =
                    callbackState->GetOwner<wxSearchCtrl>();
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                liveOwner->ApplySelectionToPeer();
                if ( callbackState->GetOwner<wxSearchCtrl>() == liveOwner &&
                     liveOwner->m_winui &&
                     liveOwner->m_winui->callbackState == callbackState )
                {
                    liveOwner->ApplySuggestions(liveOwner->m_suggestions);
                }
            });

        // A theme/template replacement can swap the inner TextBox and its
        // buttons without re-raising Loaded on the AutoSuggestBox. Observe
        // layout edges and compare part identities there. The resolver has a
        // two-pass reentrancy budget and never schedules itself with
        // CallAfter(), so this cannot become an unbounded refresh loop.
        createImpl->layoutUpdatedToken = createImpl->box.LayoutUpdated(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                wxSearchCtrl * const owner =
                    callbackState->GetOwner<wxSearchCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }

                owner->ResolvePeerParts(false);
            });

        ApplyValueToPeer();
        const MUXC::AutoSuggestBox box = createImpl->box;
        if ( !createImpl->host.SetContent(box) )
        {
            closeLiveOwner();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI AutoSuggestBox creation", e);
        closeLiveOwner();
        return false;
    }

    liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

    if ( loadedHook )
    {
        // Force only the implementation-only seam through the real Loaded
        // path so Create() lifetime tests never depend on compositor timing.
        createImpl->host.ForceRender();
        liveOwner = getLiveOwner();
        if ( !liveOwner )
            return false;

        // The one-shot hook is allowed to delete this control and host.
        // Reacquire through callback state before any later implementation use.
        createImpl->host.DispatchPendingContentLoadedHookForTesting();
        liveOwner = getLiveOwner();
        if ( !liveOwner )
            return false;
    }

    liveOwner->SetInitialSize(size);

    liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

    // ForceRender() may run application code and destroy the owner. Keep it
    // as the final implementation access and validate only through the
    // independently-owned callback state afterwards.
    createImpl->host.ForceRender();
    return getLiveOwner() != nullptr;
}

// ----------------------------------------------------------------------------
// wxSearchCtrlBase
// ----------------------------------------------------------------------------

#if wxUSE_MENUS
void wxSearchCtrl::SetMenu(wxMenu *menu)
{
    if ( menu == m_menu )
        return;

    delete m_menu;
    m_menu = menu;
    ShowSearchButton(m_searchButtonVisible);
}

wxMenu *wxSearchCtrl::GetMenu()
{
    return m_menu;
}
#endif // wxUSE_MENUS

void wxSearchCtrl::ShowSearchButton(bool show)
{
    m_searchButtonVisible = show;
    if ( m_winui && m_winui->box )
    {
        try
        {
            if ( show
#if wxUSE_MENUS
                 || m_menu
#endif // wxUSE_MENUS
               )
            {
                m_winui->box.QueryIcon(MUXC::SymbolIcon(
                    winrt::Microsoft::UI::Xaml::Controls::Symbol::Find));
            }
            else
                m_winui->box.QueryIcon(nullptr);
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl search-button visibility", e);
        }
    }
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    return m_searchButtonVisible
#if wxUSE_MENUS
        || m_menu
#endif // wxUSE_MENUS
        ;
}

void wxSearchCtrl::ShowCancelButton(bool show)
{
    m_cancelButtonVisible = show;
    ResolvePeerParts();
    if ( m_winui && m_winui->deleteButton )
    {
        try
        {
            m_winui->deleteButton.Visibility(
                show ? MUX::Visibility::Visible
                     : MUX::Visibility::Collapsed);
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl cancel-button visibility", e);
        }
    }
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
    return m_cancelButtonVisible;
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    m_descriptiveText = text;
    if ( m_winui && m_winui->box )
    {
        try
        {
            m_winui->box.PlaceholderText(wxWinUIToHString(text));
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl descriptive text", e);
        }
    }
}

wxString wxSearchCtrl::GetDescriptiveText() const
{
    return m_descriptiveText;
}

// ----------------------------------------------------------------------------
// wxTextEntry
// ----------------------------------------------------------------------------

void wxSearchCtrl::WriteText(const wxString& text)
{
    long from = wxMin(m_selectionStart, m_selectionEnd);
    long to = wxMax(m_selectionStart, m_selectionEnd);
    if ( from == to )
        from = to = m_insertionPoint;

    Replace(from, to, text);
}

void wxSearchCtrl::Replace(long from,
                           long to,
                           const wxString& value)
{
    const long len = static_cast<long>(m_value.length());
    from = wxWinUIClampPos(from, len);
    to = to < 0 ? len : wxWinUIClampPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    const wxString inserted =
        wxWinUITextPositionMap::NormalizeNewlines(value);
    m_value = m_value.Left(from) + inserted + m_value.Mid(to);
    m_insertionPoint =
        from + static_cast<long>(inserted.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyValueToPeer();

    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxSearchCtrl::Remove(long from, long to)
{
    const long len = m_value.length();
    from = wxWinUIClampPos(from, len);
    to = to < 0 ? len : wxWinUIClampPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    m_value = m_value.Left(from) + m_value.Mid(to);
    m_insertionPoint = m_selectionStart = m_selectionEnd = from;
    ApplyValueToPeer();

    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxSearchCtrl::Copy()
{
    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.CopySelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl copy", e);
        }
    }

#if wxUSE_CLIPBOARD
    if ( !HasSelection() || !wxTheClipboard )
        return;
    if ( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData(
            new wxTextDataObject(GetStringSelection()));
        wxTheClipboard->Close();
    }
#endif // wxUSE_CLIPBOARD
}

void wxSearchCtrl::Cut()
{
    if ( !m_editable )
        return;

    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.CutSelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl cut", e);
        }
    }

    Copy();
    Remove(m_selectionStart, m_selectionEnd);
}

void wxSearchCtrl::Paste()
{
    if ( !m_editable )
        return;

    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.PasteFromClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl paste", e);
        }
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard || !wxTheClipboard->Open() )
        return;

    if ( wxTheClipboard->IsSupported(wxDF_TEXT) )
    {
        wxTextDataObject data;
        if ( wxTheClipboard->GetData(data) )
            WriteText(data.GetText());
    }
    wxTheClipboard->Close();
#endif // wxUSE_CLIPBOARD
}

void wxSearchCtrl::Undo()
{
    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.Undo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI SearchCtrl undo", e);
        }
    }
}

void wxSearchCtrl::Redo()
{
    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.Redo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI SearchCtrl redo", e);
        }
    }
}

bool wxSearchCtrl::CanUndo() const
{
    const wxWeakRef<wxWindow> self(
        const_cast<wxSearchCtrl *>(this));
    const_cast<wxSearchCtrl *>(this)->ResolvePeerParts();
    if ( !self || !m_winui || !m_winui->editBox )
        return false;

    const MUXC::TextBox editBox = m_winui->editBox;
    const std::uint64_t generation = m_winui->editGeneration;
    try
    {
        const bool canUndo = editBox.CanUndo();
        return self && m_winui &&
               m_winui->editGeneration == generation &&
               m_winui->editBox == editBox &&
               canUndo;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxSearchCtrl::CanRedo() const
{
    const wxWeakRef<wxWindow> self(
        const_cast<wxSearchCtrl *>(this));
    const_cast<wxSearchCtrl *>(this)->ResolvePeerParts();
    if ( !self || !m_winui || !m_winui->editBox )
        return false;

    const MUXC::TextBox editBox = m_winui->editBox;
    const std::uint64_t generation = m_winui->editGeneration;
    try
    {
        const bool canRedo = editBox.CanRedo();
        return self && m_winui &&
               m_winui->editGeneration == generation &&
               m_winui->editBox == editBox &&
               canRedo;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxSearchCtrl::SetInsertionPoint(long pos)
{
    m_insertionPoint = wxWinUIClampPos(pos, m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplySelectionToPeer();
}

long wxSearchCtrl::GetInsertionPoint() const
{
    return m_insertionPoint;
}

long wxSearchCtrl::GetLastPosition() const
{
    return m_value.length();
}

void wxSearchCtrl::SetSelection(long from, long to)
{
    const long len = m_value.length();
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = len;
    }
    else
    {
        from = wxWinUIClampPos(from, len);
        to = wxWinUIClampPos(to, len);
    }
    m_selectionStart = from;
    m_selectionEnd = to;
    m_insertionPoint = from;
    ApplySelectionToPeer();
}

void wxSearchCtrl::GetSelection(long *from, long *to) const
{
    if ( from )
        *from = m_selectionStart;
    if ( to )
        *to = m_selectionEnd;
}

bool wxSearchCtrl::IsEditable() const
{
    return m_editable;
}

void wxSearchCtrl::SetEditable(bool editable)
{
    m_editable = editable;
    ResolvePeerParts();
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.IsReadOnly(!editable);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI SearchCtrl editable state", e);
        }
    }
}

bool wxSearchCtrl::WinUIInvokeSearchButtonForTesting()
{
    if ( !m_winui )
        return false;

    const wxWeakRef<wxWindow> self(this);
    m_winui->host.SynchronizeForFocus();
    if ( !self )
        return false;

    m_winui->host.ForceRender();
    if ( !self )
        return false;

    ResolvePeerParts();
    if ( !self || !m_winui || !m_winui->queryButton )
        return false;

    try
    {
        winrt::Microsoft::UI::Xaml::Automation::Peers::
            ButtonAutomationPeer peer(m_winui->queryButton);
        peer.Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl query-button invoke", e);
        return false;
    }
}

bool wxSearchCtrl::WinUIInvokeCancelButtonForTesting()
{
    if ( !m_winui )
        return false;

    const wxWeakRef<wxWindow> self(this);
    m_winui->host.SynchronizeForFocus();
    if ( !self )
        return false;

    m_winui->host.ForceRender();
    if ( !self )
        return false;

    ResolvePeerParts();
    if ( !self || !m_winui || !m_winui->deleteButton )
        return false;

    try
    {
        winrt::Microsoft::UI::Xaml::Automation::Peers::
            ButtonAutomationPeer peer(m_winui->deleteButton);
        peer.Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl cancel-button invoke", e);
        return false;
    }
}

bool wxSearchCtrl::WinUISetPeerTextForTesting(const wxString& text)
{
    if ( !m_winui || !m_winui->box )
        return false;

    m_winui->hasPendingPeerValue = false;
    m_winui->pendingPeerValue.clear();
    try
    {
        m_winui->box.Text(wxWinUIToHString(
            wxWinUITextPositionMap::NormalizeNewlines(text)));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl peer test mutation", e);
        return false;
    }
}

bool wxSearchCtrl::WinUIRetemplateForTesting()
{
    if ( !m_winui )
        return false;

    const wxWeakRef<wxWindow> self(this);
    try
    {
        // Keep the previous query button alive after the transactional swap:
        // invoking it is the deterministic proof that its old Click token was
        // revoked (and, independently, invalidated by its generation).
        m_winui->retiredQueryButtonForTesting = m_winui->queryButton;
        m_winui->injectedEditBoxForTesting = MUXC::TextBox();
        m_winui->injectedDeleteButtonForTesting = MUXC::Button();
        m_winui->injectedQueryButtonForTesting = MUXC::Button();
        m_winui->useInjectedPartsForTesting = true;
        ResolvePeerParts(false);

        return self && m_winui &&
               m_winui->editBox ==
                   m_winui->injectedEditBoxForTesting &&
               m_winui->deleteButton ==
                   m_winui->injectedDeleteButtonForTesting &&
               m_winui->queryButton ==
                   m_winui->injectedQueryButtonForTesting;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl test re-template", e);
        return false;
    }
}

bool wxSearchCtrl::WinUIInvokeRetiredSearchButtonForTesting()
{
    if ( !m_winui || !m_winui->retiredQueryButtonForTesting )
        return false;

    try
    {
        winrt::Microsoft::UI::Xaml::Automation::Peers::
            ButtonAutomationPeer peer(
                m_winui->retiredQueryButtonForTesting);
        peer.Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl retired query-button invoke", e);
        return false;
    }
}

unsigned wxSearchCtrl::WinUIGetSuggestionCountForTesting() const
{
    return m_winui ? m_winui->suggestionCount : 0;
}

unsigned wxSearchCtrl::WinUIGetTemplateStateForTesting() const
{
    if ( !m_winui )
        return 0;

    unsigned state = 0;
    try
    {
        if ( m_winui->box && m_winui->box.XamlRoot() )
            state |= 0x1;
    }
    catch ( const winrt::hresult_error& )
    {
    }
    if ( m_winui->editBox )
        state |= 0x2;
    if ( m_winui->deleteButton )
        state |= 0x4;
    if ( m_winui->queryButton )
        state |= 0x8;
    return state;
}

void wxSearchCtrl::WinUISetNextCreateLoadedHookForTesting(
    WinUICreateLoadedHookForTesting hook,
    void *context)
{
    m_nextCreateLoadedHookForTesting = hook;
    m_nextCreateLoadedContextForTesting = hook ? context : nullptr;
}

// ----------------------------------------------------------------------------
// internals
// ----------------------------------------------------------------------------

wxSize wxSearchCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(180, 36), const_cast<wxSearchCtrl*>(this));
}

bool wxSearchCtrl::MSWShouldPreProcessMessage(WXMSG* msg)
{
    return wxTextEntry::MSWShouldPreProcessMessage(msg) &&
           wxControl::MSWShouldPreProcessMessage(msg);
}

void wxSearchCtrl::DoSetValue(const wxString& value, int flags)
{
    const wxString normalized =
        wxWinUITextPositionMap::NormalizeNewlines(value);
    if ( normalized != m_value )
        m_insertionPoint = 0;
    else
        m_insertionPoint =
            wxWinUIClampPos(m_insertionPoint, m_value.length());

    m_value = normalized;
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyValueToPeer();

    if ( flags & SetValue_SendEvent )
    {
        wxCommandEvent event(wxEVT_TEXT, GetId());
        event.SetEventObject(this);
        event.SetString(m_value);
        ProcessCommand(event);
    }
}

wxString wxSearchCtrl::DoGetValue() const
{
    return m_value;
}

WXHWND wxSearchCtrl::GetEditHWND() const
{
    return GetHWND();
}

void wxSearchCtrl::ApplyValueToPeer()
{
    if ( !m_winui || !m_winui->box )
        return;

    wxWinUISearchCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner =
        [self, impl, callbackState]() -> wxSearchCtrl *
        {
            wxSearchCtrl * const owner =
                wxDynamicCast(self.get(), wxSearchCtrl);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                     ? owner
                     : nullptr;
        };
    const bool wasUpdatingPeer = m_updatingPeer;
    m_updatingPeer = true;
    wxScopeGuard updatingGuard = wxMakeGuard(
        [getLiveOwner, wasUpdatingPeer]()
    {
        if ( wxSearchCtrl * const owner = getLiveOwner() )
            owner->m_updatingPeer = wasUpdatingPeer;
    });
    wxUnusedVar(updatingGuard);

    bool setTextAttempted = false;
    bool setTextCompleted = false;
    try
    {
        if ( wxWinUITextPositionMap::NormalizeNewlines(
                 wxWinUIFromHString(impl->box.Text())) != m_value )
        {
            impl->pendingPeerValue = m_value;
            impl->hasPendingPeerValue = true;
            setTextAttempted = true;
            impl->box.Text(wxWinUIToHString(m_value));
            setTextCompleted = true;
            if ( !getLiveOwner() )
                return;
        }
        wxSearchCtrl *live = getLiveOwner();
        if ( !live )
            return;
        live->ResolvePeerParts();
        live = getLiveOwner();
        if ( !live )
            return;
        live->ApplySelectionToPeer();
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setTextAttempted && !setTextCompleted )
        {
            if ( wxSearchCtrl * const live = getLiveOwner() )
            {
                wxUnusedVar(live);
                impl->hasPendingPeerValue = false;
                impl->pendingPeerValue.clear();
            }
        }
        wxWinUILogException("WinUI SearchCtrl value", e);
    }
}

void wxSearchCtrl::ApplySelectionToPeer()
{
    if ( !m_winui )
        return;

    const wxWeakRef<wxWindow> self(this);
    wxWinUISearchCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const auto getLiveOwner =
        [self, impl, callbackState]() -> wxSearchCtrl *
        {
            wxSearchCtrl * const owner =
                wxDynamicCast(self.get(), wxSearchCtrl);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState
                     ? owner
                     : nullptr;
        };

    ResolvePeerParts();
    wxSearchCtrl *live = getLiveOwner();
    if ( !live || !impl->editBox )
        return;

    const MUXC::TextBox editBox = impl->editBox;
    const long from = wxMin(live->m_selectionStart, live->m_selectionEnd);
    const long to = wxMax(live->m_selectionStart, live->m_selectionEnd);
    try
    {
        editBox.SelectionStart(static_cast<int32_t>(from));
        live = getLiveOwner();
        if ( !live || impl->editBox != editBox )
            return;
        editBox.SelectionLength(
            static_cast<int32_t>(to - from));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI SearchCtrl selection", e);
    }
}

void wxSearchCtrl::ReadSelectionFromPeer()
{
    ResolvePeerParts();
    if ( !m_winui || !m_winui->editBox )
    {
        m_insertionPoint =
            m_selectionStart =
            m_selectionEnd =
                static_cast<long>(m_value.length());
        return;
    }

    try
    {
        m_selectionStart = m_winui->editBox.SelectionStart();
        m_selectionEnd =
            m_selectionStart + m_winui->editBox.SelectionLength();
        m_insertionPoint = m_selectionStart;
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxSearchCtrl::ResolvePeerParts(bool updateLayout)
{
    if ( !m_winui || !m_winui->box || !m_winui->partRefreshState )
        return;

    const std::shared_ptr<wxWinUISearchPartRefreshState> refreshState =
        m_winui->partRefreshState;
    if ( !refreshState->active )
        return;

    if ( refreshState->resolving )
    {
        refreshState->pending = true;
        return;
    }

    refreshState->resolving = true;
    wxON_BLOCK_EXIT_SET(refreshState->resolving, false);

    const wxWeakRef<wxWindow> self(this);
    for ( unsigned pass = 0; pass != 2; ++pass )
    {
        refreshState->pending = false;
        ResolvePeerPartsOnce(updateLayout && pass == 0);
        if ( !self || !refreshState->active ||
             !refreshState->pending )
        {
            break;
        }
    }
}

void wxSearchCtrl::ResolvePeerPartsOnce(bool updateLayout)
{
    if ( !m_winui || !m_winui->box )
        return;

    wxWinUISearchCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const MUXC::AutoSuggestBox box = impl->box;
    const bool useInjectedParts =
        impl->useInjectedPartsForTesting;
    const auto getLiveOwner =
        [callbackState, impl, box]() -> wxSearchCtrl *
        {
            wxSearchCtrl * const owner =
                callbackState->GetOwner<wxSearchCtrl>();
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           owner->m_winui->box == box
                     ? owner
                     : nullptr;
        };

    try
    {
        if ( !useInjectedParts && !box.XamlRoot() )
            return;

        // Query/Delete/TextBox live in the control template. Applying it here
        // makes explicit API calls deterministic even before the next render
        // tick (notably SetFocus() and the automation test seams).
        if ( updateLayout && !useInjectedParts )
        {
            box.ApplyTemplate();
            if ( !getLiveOwner() )
                return;
            box.UpdateLayout();
        }
        if ( !getLiveOwner() )
            return;

        const MUXC::TextBox editBox =
            useInjectedParts
                ? impl->injectedEditBoxForTesting
                : wxWinUIFindSearchTextBox(box);
        if ( editBox != impl->editBox )
        {
            // Invalidate and detach the old generation before attempting its
            // revocation. A throwing removal can then leave only an inert
            // delegate, never a callback that still matches the live part.
            const MUXC::TextBox oldEditBox = impl->editBox;
            const winrt::event_token oldSelectionToken =
                impl->selectionChangedToken;
            impl->editBox = nullptr;
            impl->selectionChangedToken = {};
            if ( ++impl->editGeneration == 0 )
                ++impl->editGeneration;

            if ( oldEditBox && oldSelectionToken.value )
            {
                try
                {
                    oldEditBox.SelectionChanged(oldSelectionToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl selection removal", e);
                }
            }

            const std::uint64_t generation =
                impl->editGeneration;
            winrt::event_token selectionToken{};
            if ( editBox )
            {
                try
                {
                    editBox.IsReadOnly(!m_editable);
                    selectionToken = editBox.SelectionChanged(
                    [callbackState, generation](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUX::RoutedEventArgs const&)
                    {
                        wxSearchCtrl * const owner =
                            callbackState->GetOwner<wxSearchCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                callbackState ||
                             owner->m_winui->editGeneration != generation ||
                             owner->m_updatingPeer )
                        {
                            return;
                        }
                        owner->ReadSelectionFromPeer();
                    });

                    if ( callbackState->GetOwner<wxSearchCtrl>() != this ||
                         !m_winui || m_winui.get() != impl ||
                         impl->editGeneration != generation )
                    {
                        editBox.SelectionChanged(selectionToken);
                        return;
                    }

                    // Commit only after the candidate is completely
                    // configured and subscribed. A failed setup leaves the
                    // field null, so the next layout edge retries it.
                    impl->editBox = editBox;
                    impl->selectionChangedToken = selectionToken;
                }
                catch ( const winrt::hresult_error& e )
                {
                    if ( selectionToken.value )
                    {
                        try
                        {
                            editBox.SelectionChanged(selectionToken);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    }
                    wxWinUILogException(
                        "WinUI SearchCtrl edit-part setup", e);
                }
            }
        }

        // Delete/Query are template children of the inner TextBox, not of the
        // AutoSuggestBox namescope itself. Realize that nested template before
        // walking its visual tree.
        if ( impl->editBox && updateLayout && !useInjectedParts )
        {
            const MUXC::TextBox editBoxForLayout = impl->editBox;
            editBoxForLayout.ApplyTemplate();
            if ( !getLiveOwner() ||
                 impl->editBox != editBoxForLayout )
            {
                return;
            }
            editBoxForLayout.UpdateLayout();
            if ( !getLiveOwner() ||
                 impl->editBox != editBoxForLayout )
            {
                return;
            }
        }
        const MUX::DependencyObject buttonRoot =
            impl->editBox
                ? impl->editBox.try_as<MUX::DependencyObject>()
                : impl->box.try_as<MUX::DependencyObject>();

        MUXC::Button deleteButton{ nullptr };
        MUXC::IControlProtected editTemplate{ nullptr };
        if ( useInjectedParts )
        {
            deleteButton = impl->injectedDeleteButtonForTesting;
        }
        else if ( impl->editBox )
        {
            editTemplate =
                impl->editBox.try_as<MUXC::IControlProtected>();
            if ( editTemplate )
            {
                deleteButton = editTemplate.
                    GetTemplateChild(L"DeleteButton").
                    try_as<MUXC::Button>();
            }
        }
        if ( !deleteButton && !useInjectedParts )
            deleteButton = wxWinUIFindSearchDeleteButton(buttonRoot);
        if ( !deleteButton && impl->editBox && !useInjectedParts )
        {
            deleteButton = impl->editBox.
                FindName(L"DeleteButton").
                try_as<MUXC::Button>();
        }
        if ( deleteButton != impl->deleteButton )
        {
            const MUXC::Button oldDeleteButton = impl->deleteButton;
            const winrt::event_token oldDeleteToken =
                impl->deleteClickToken;
            impl->deleteButton = nullptr;
            impl->deleteClickToken = {};
            if ( ++impl->deleteGeneration == 0 )
                ++impl->deleteGeneration;

            if ( oldDeleteButton && oldDeleteToken.value )
            {
                try
                {
                    oldDeleteButton.Click(oldDeleteToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl cancel removal", e);
                }
            }

            const std::uint64_t generation =
                impl->deleteGeneration;
            winrt::event_token deleteToken{};
            if ( deleteButton )
            {
                try
                {
                    deleteButton.Visibility(
                        m_cancelButtonVisible
                            ? MUX::Visibility::Visible
                            : MUX::Visibility::Collapsed);
                    deleteToken = deleteButton.Click(
                [callbackState, generation](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxSearchCtrl * const owner =
                        callbackState->GetOwner<wxSearchCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_winui->deleteGeneration != generation ||
                         !owner->m_cancelButtonVisible )
                    {
                        return;
                    }

                    wxCommandEvent event(
                        wxEVT_SEARCH_CANCEL, owner->GetId());
                    event.SetEventObject(owner);
                    owner->ProcessCommand(event);
                });

                    if ( callbackState->GetOwner<wxSearchCtrl>() != this ||
                         !m_winui || m_winui.get() != impl ||
                         impl->deleteGeneration != generation )
                    {
                        deleteButton.Click(deleteToken);
                        return;
                    }

                    impl->deleteButton = deleteButton;
                    impl->deleteClickToken = deleteToken;
                }
                catch ( const winrt::hresult_error& e )
                {
                    if ( deleteToken.value )
                    {
                        try
                        {
                            deleteButton.Click(deleteToken);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    }
                    wxWinUILogException(
                        "WinUI SearchCtrl delete-part setup", e);
                }
            }
        }

        MUXC::Button queryButton{ nullptr };
        if ( useInjectedParts )
        {
            queryButton = impl->injectedQueryButtonForTesting;
        }
        else if ( editTemplate )
        {
            queryButton = editTemplate.
                GetTemplateChild(L"QueryButton").
                try_as<MUXC::Button>();
        }
        if ( !queryButton && !useInjectedParts )
            queryButton = wxWinUIFindSearchQueryButton(buttonRoot);
        if ( !queryButton && impl->editBox && !useInjectedParts )
        {
            queryButton = impl->editBox.
                FindName(L"QueryButton").
                try_as<MUXC::Button>();
        }
        if ( queryButton != impl->queryButton )
        {
            const MUXC::Button oldQueryButton = impl->queryButton;
            const winrt::event_token oldQueryToken = impl->queryClickToken;
            impl->queryButton = nullptr;
            impl->queryClickToken = {};
            if ( ++impl->queryGeneration == 0 )
                ++impl->queryGeneration;

            if ( oldQueryButton && oldQueryToken.value )
            {
                try
                {
                    oldQueryButton.Click(oldQueryToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI SearchCtrl query-button removal", e);
                }
            }

            const std::uint64_t generation =
                impl->queryGeneration;
            const bool submitFromClickForTesting =
                useInjectedParts;
            winrt::event_token queryToken{};
            if ( queryButton )
            {
                try
                {
                    queryToken = queryButton.Click(
                        [callbackState, generation,
                         submitFromClickForTesting](
                            winrt::Windows::Foundation::IInspectable const&,
                            MUX::RoutedEventArgs const&)
                        {
                            wxSearchCtrl * owner =
                                callbackState->GetOwner<wxSearchCtrl>();
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->callbackState !=
                                     callbackState ||
                                 owner->m_winui->queryGeneration !=
                                     generation )
                            {
                                return;
                            }

                            // A detached synthetic part has no
                            // AutoSuggestBox machinery to raise
                            // QuerySubmitted. The implementation-only seam
                            // therefore routes the same wx event from this
                            // real Button::Click delegate.
                            if ( submitFromClickForTesting )
                            {
                                wxCommandEvent event(
                                    wxEVT_SEARCH, owner->GetId());
                                event.SetEventObject(owner);
                                event.SetString(owner->m_value);
                                owner->ProcessCommand(event);

                                owner = callbackState->
                                    GetOwner<wxSearchCtrl>();
                                if ( !owner || !owner->m_winui ||
                                     owner->m_winui->callbackState !=
                                         callbackState ||
                                     owner->m_winui->queryGeneration !=
                                         generation )
                                {
                                    return;
                                }
                            }

#if wxUSE_MENUS
                            if ( !owner->m_menu || !wxTheApp )
                                return;

                            // AutoSuggestBox raises QuerySubmitted from the
                            // same click. Defer only the menu so the wx search
                            // event is delivered first, while Enter remains a
                            // plain search action and never opens the menu.
                            wxMenu * const expectedMenu = owner->m_menu;
                            wxTheApp->CallAfter(
                                [callbackState, expectedMenu]()
                                {
                                    wxSearchCtrl * const liveOwner =
                                        callbackState->
                                            GetOwner<wxSearchCtrl>();
                                    if ( !liveOwner ||
                                         !liveOwner->m_winui ||
                                         liveOwner->m_winui->
                                             callbackState != callbackState ||
                                         liveOwner->m_menu != expectedMenu )
                                    {
                                        return;
                                    }

                                    const int y =
                                        liveOwner->GetSize().y;
                                    liveOwner->PopupMenu(
                                        expectedMenu, 0, y);
                                });
#endif // wxUSE_MENUS
                        });

                    if ( callbackState->GetOwner<wxSearchCtrl>() != this ||
                         !m_winui || m_winui.get() != impl ||
                         impl->queryGeneration != generation )
                    {
                        queryButton.Click(queryToken);
                        return;
                    }

                    impl->queryButton = queryButton;
                    impl->queryClickToken = queryToken;
                }
                catch ( const winrt::hresult_error& e )
                {
                    if ( queryToken.value )
                    {
                        try
                        {
                            queryButton.Click(queryToken);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    }
                    wxWinUILogException(
                        "WinUI SearchCtrl query-part setup", e);
                }
            }
        }

        // Preferred focus is intentionally retried even when the template
        // part identity did not change: a transient detached-tree failure
        // must not permanently strand focus on the outer AutoSuggestBox.
        if ( impl->editBox )
            impl->host.SetPreferredFocus(impl->editBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI SearchCtrl template-part resolution", e);
    }
}

bool wxSearchCtrl::DoAutoCompleteStrings(const wxArrayString& choices)
{
    m_suggestions = choices;
    ApplySuggestions(choices);
    return true;
}

void wxSearchCtrl::ApplySuggestions(const wxArrayString& choices)
{
    if ( !m_winui || !m_winui->box )
        return;

    // The AutoSuggestBox can only accept an item source once it is part of a
    // XamlRoot (i.e. after it has been realized in the island); setting it
    // earlier throws and leaves the control in a state that crashes on the next
    // layout pass.
    try
    {
        if ( !m_winui->box.XamlRoot() )
        {
            if ( m_winui->useInjectedPartsForTesting )
            {
                unsigned count = 0;
                for ( size_t i = 0; i < choices.GetCount(); ++i )
                {
                    if ( m_value.empty() ||
                         choices[i].Lower().StartsWith(m_value.Lower()) )
                    {
                        ++count;
                    }
                }
                m_winui->suggestionCount = count;
            }
            return;
        }

        auto items = winrt::single_threaded_vector<
            winrt::Windows::Foundation::IInspectable>();
        for ( size_t i = 0; i < choices.GetCount(); ++i )
        {
            // Show only the suggestions matching the current text.
            if ( m_value.empty() ||
                 choices[i].Lower().StartsWith(m_value.Lower()) )
            {
                items.Append(winrt::box_value(wxWinUIToHString(choices[i])));
            }
        }
        m_winui->box.ItemsSource(items);
        m_winui->suggestionCount = items.Size();
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

#endif // wxUSE_SEARCHCTRL
