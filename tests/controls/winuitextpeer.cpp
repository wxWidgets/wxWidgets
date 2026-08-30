///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitextpeer.cpp
// Purpose:     deterministic tests for WinUI text peer contracts
// Author:      wxWidgets development team
// Created:     2026-07-25
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "testableframe.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TEXTCTRL

#include "wx/app.h"
#include "wx/font.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "textctrl-test-access.h"
#include "searchctrl-test-access.h"
#include "wx/textcompleter.h"
#include "wx/uiaction.h"
#include "wx/weakref.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/textpeer.h"

#if wxUSE_SEARCHCTRL
    #include "wx/srchctrl.h"
#endif

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif

#if wxUSE_CLIPBOARD
    #include <ole2.h>
#endif

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif // wxUSE_MENUS

#if wxUSE_VALIDATORS
    #include "wx/valtext.h"
#endif // wxUSE_VALIDATORS

#include <memory>
#include <cstdlib>
#include <vector>

namespace
{

#if wxUSE_CLIPBOARD
class ClipboardContentsRestorer final
{
public:
    ClipboardContentsRestorer()
    {
        const HRESULT result = ::OleGetClipboard(&m_previous);
        m_captured = SUCCEEDED(result) || ::CountClipboardFormats() == 0;
    }

    ~ClipboardContentsRestorer()
    {
        if ( m_captured )
            ::OleSetClipboard(m_previous);
        if ( m_previous )
            m_previous->Release();
    }

    bool IsCaptured() const { return m_captured; }

private:
    IDataObject *m_previous = nullptr;
    bool m_captured = false;
};
#endif // wxUSE_CLIPBOARD

struct PasswordTextChangingUIAProbe
{
    bool invoked = false;
    bool querySucceeded = false;
    bool isPassword = false;
    bool hasValuePattern = true;
    bool hasTextPattern = true;
};

void QueryPasswordUIADuringTextChanging(wxTextCtrl *owner, void *context)
{
    PasswordTextChangingUIAProbe * const probe =
        static_cast<PasswordTextChangingUIAProbe *>(context);
    probe->invoked = true;

    bool predictionEnabled = true;
    probe->querySucceeded = wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(*owner,
        &probe->isPassword,
        &probe->hasValuePattern,
        &probe->hasTextPattern,
        nullptr,
        &predictionEnabled);
}

class DeleteTextCtrlOnStartCompleter final : public wxTextCompleter
{
public:
    explicit DeleteTextCtrlOnStartCompleter(wxTextCtrl **owner)
        : m_owner(owner)
    {
    }

    bool Start(const wxString&) override
    {
        wxTextCtrl * const doomed = *m_owner;
        *m_owner = nullptr;
        delete doomed;
        return false;
    }

    wxString GetNext() override { return wxString(); }

private:
    wxTextCtrl **m_owner;
};

class DeleteTextCtrlOnDestroyCompleter final : public wxTextCompleter
{
public:
    DeleteTextCtrlOnDestroyCompleter(wxTextCtrl **owner, bool *armed)
        : m_owner(owner), m_armed(armed)
    {
    }

    ~DeleteTextCtrlOnDestroyCompleter() override
    {
        if ( *m_armed && *m_owner )
        {
            wxTextCtrl * const doomed = *m_owner;
            *m_owner = nullptr;
            delete doomed;
        }
    }

    bool Start(const wxString&) override { return false; }
    wxString GetNext() override { return wxString(); }

private:
    wxTextCtrl **m_owner;
    bool *m_armed;
};

bool MakeTestWindowTransparent(HWND hwnd)
{
    // Keep the XAML island on a real monitor so that WinUI realizes wrapped
    // TextBox lines without visibly displaying or activating the test frame.
    // DWM cloaking and alpha zero both make WinUI treat the XAML island as
    // non-visible and return sentinel rectangles for virtualized text. Alpha
    // one keeps the compositor/layout path real while remaining imperceptible;
    // WS_EX_TRANSPARENT also makes the frame click-through.
    const LONG_PTR exStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    ::SetLastError(ERROR_SUCCESS);
    if ( !::SetWindowLongPtr(
             hwnd,
             GWL_EXSTYLE,
             exStyle |
                 WS_EX_LAYERED |
                 WS_EX_NOACTIVATE |
                 WS_EX_TRANSPARENT) &&
         ::GetLastError() != ERROR_SUCCESS )
    {
        return false;
    }

    return ::SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA) != FALSE;
}

template <typename Control>
struct CreateLoadedDestructionProbe
{
    Control **ownerSlot = nullptr;
    bool invoked = false;
    bool ownerMatched = false;
};

void DestroyTextCtrlDuringCreateLoaded(
    wxTextCtrl *owner,
    void *context)
{
    CreateLoadedDestructionProbe<wxTextCtrl> * const probe =
        static_cast<CreateLoadedDestructionProbe<wxTextCtrl> *>(context);
    probe->invoked = true;
    probe->ownerMatched =
        probe->ownerSlot && *probe->ownerSlot == owner;
    if ( probe->ownerSlot )
        *probe->ownerSlot = nullptr;

    // Final operation: Close() invalidates the generation-bound callback
    // state before the host is detached.
    delete owner;
}

#if wxUSE_SEARCHCTRL
void DestroySearchCtrlDuringCreateLoaded(
    wxSearchCtrl *owner,
    void *context)
{
    CreateLoadedDestructionProbe<wxSearchCtrl> * const probe =
        static_cast<CreateLoadedDestructionProbe<wxSearchCtrl> *>(context);
    probe->invoked = true;
    probe->ownerMatched =
        probe->ownerSlot && *probe->ownerSlot == owner;
    if ( probe->ownerSlot )
        *probe->ownerSlot = nullptr;

    delete owner;
}
#endif // wxUSE_SEARCHCTRL

template <typename Control>
struct InitialSizeDestructionProbe
{
    Control **ownerSlot = nullptr;
    bool loaded = false;
    bool destroyed = false;
};

template <typename Control>
class DestroyOnInitialSizeControl final : public Control
{
public:
    void PrepareForInitialSize(
        InitialSizeDestructionProbe<Control> *probe)
    {
        // Make the explicit Create() size differ from the current one without
        // consuming the armed virtual call.
        Control::DoSetSize(
            wxDefaultCoord, wxDefaultCoord, 1, 1,
            wxSIZE_USE_EXISTING);
        m_probe = probe;
        m_armed = true;
    }

protected:
    void DoSetSize(int x,
                   int y,
                   int width,
                   int height,
                   int sizeFlags = wxSIZE_AUTO) override
    {
        if ( !m_armed )
        {
            Control::DoSetSize(x, y, width, height, sizeFlags);
            return;
        }

        m_armed = false;
        InitialSizeDestructionProbe<Control> * const probe = m_probe;
        probe->destroyed = true;
        if ( probe->ownerSlot && *probe->ownerSlot == this )
            *probe->ownerSlot = nullptr;

        // SetInitialSize() and its SetSize() wrapper don't touch the object
        // after this virtual call. Create() must nevertheless reacquire its
        // exact callback state before continuing to ForceRender().
        delete this;
    }

private:
    InitialSizeDestructionProbe<Control> *m_probe = nullptr;
    bool m_armed = false;
};

void ArmTextCtrlInitialSizeDestruction(
    wxTextCtrl *owner,
    void *context)
{
    InitialSizeDestructionProbe<wxTextCtrl> * const probe =
        static_cast<InitialSizeDestructionProbe<wxTextCtrl> *>(context);
    probe->loaded = true;
    static_cast<DestroyOnInitialSizeControl<wxTextCtrl> *>(owner)->
        PrepareForInitialSize(probe);
}

#if wxUSE_SEARCHCTRL
void ArmSearchCtrlInitialSizeDestruction(
    wxSearchCtrl *owner,
    void *context)
{
    InitialSizeDestructionProbe<wxSearchCtrl> * const probe =
        static_cast<InitialSizeDestructionProbe<wxSearchCtrl> *>(context);
    probe->loaded = true;
    static_cast<DestroyOnInitialSizeControl<wxSearchCtrl> *>(owner)->
        PrepareForInitialSize(probe);
}
#endif // wxUSE_SEARCHCTRL

struct TemporarySelectionSupersessionProbe
{
    bool invoked = false;
    bool peerSelectionRead = false;
    long publicFrom = -1;
    long publicTo = -1;
    long peerFrom = -1;
    long peerTo = -1;
    bool supersessionPending = false;
    unsigned supersessionRetries = 0;
};

void SupersedeDuringTemporarySelection(wxTextCtrl *owner, void *context)
{
    TemporarySelectionSupersessionProbe * const probe =
        static_cast<TemporarySelectionSupersessionProbe *>(context);
    probe->invoked = true;
    owner->GetSelection(&probe->publicFrom, &probe->publicTo);
    probe->peerSelectionRead = wxWinUITextCtrlTestAccess::GetPeerSelection(*owner,
        &probe->peerFrom, &probe->peerTo);

    // The newer generation is deliberately queued so the older lexical
    // selection guard must restore the peer before its continuation runs.
    wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(*owner);
    owner->ShowPosition(0);
    wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*owner,
        &probe->supersessionPending,
        &probe->supersessionRetries);
}

} // namespace

TEST_CASE("wxWinUI text position mapping", "[winui-textpeer]")
{
    SECTION("CRLF public coordinates")
    {
        const wxWinUITextPositionMap map(wxS("a\nb"), true, true);
        CHECK(map.GetLastPosition() == 4);

        const struct
        {
            long position;
            long x;
            long y;
        } positions[] =
        {
            { 0, 0, 0 },
            { 1, 1, 0 },
            { 2, 1, 0 },
            { 3, 0, 1 },
            { 4, 1, 1 }
        };

        for ( const auto& expected : positions )
        {
            long x = -1;
            long y = -1;
            REQUIRE(map.PositionToXY(expected.position, &x, &y));
            CHECK(x == expected.x);
            CHECK(y == expected.y);
        }

        CHECK(map.XYToPosition(0, 0) == 0);
        CHECK(map.XYToPosition(1, 0) == 1);
        CHECK(map.XYToPosition(0, 1) == 3);
        CHECK(map.XYToPosition(1, 1) == 4);
        CHECK(map.XYToPosition(2, 1) == -1);
    }

    SECTION("CRLF range operations preserve half-newlines")
    {
        const wxWinUITextPositionMap map(wxS("a\nb"), true, true);
        CHECK(map.GetRange(1, 2) == "\n");
        CHECK(map.GetRange(2, 3) == "\n");
        CHECK(map.GetRange(1, 3) == "\n");

        wxString value;
        long caret = -1;
        REQUIRE(map.ReplaceRange(1, 2, wxString(), &value, &caret));
        CHECK(value == "a\nb");

        REQUIRE(map.ReplaceRange(2, 3, wxString(), &value, &caret));
        CHECK(value == "a\nb");

        REQUIRE(map.ReplaceRange(1, 3, wxString(), &value, &caret));
        CHECK(value == "ab");

        REQUIRE(map.ReplaceRange(2, 2, "X", &value, &caret));
        CHECK(value == "a\nX\nb");
        CHECK(caret >= 0);
    }

    SECTION("normalization and invalid positions")
    {
        CHECK(wxWinUITextPositionMap::NormalizeNewlines(
                  "a\r\nb\rc") == "a\nb\nc");

        const wxWinUITextPositionMap map(wxS("abc"), false, false);
        CHECK(map.GetLastPosition() == 3);
        CHECK_FALSE(map.PublicToLogical(-1, nullptr));
        CHECK_FALSE(map.PublicToLogical(4, nullptr));
        CHECK_FALSE(map.PositionToXY(4, nullptr, nullptr));
        CHECK(map.XYToPosition(4, 0) == -1);
        CHECK(map.XYToPosition(0, 1) == -1);
    }
}

TEST_CASE("wxWinUI text callback state", "[winui-textpeer][lifetime]")
{
    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();
    int owner = 17;
    {
        const auto state =
            std::make_shared<wxWinUITextCallbackState>(&owner);
        CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() ==
              baseline + 1);
        CHECK(state->GetOwner<int>() == &owner);
        state->Invalidate();
        CHECK(state->GetOwner<int>() == nullptr);
    }
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() == baseline);
}

TEST_CASE("wxWinUI text controls revoke callbacks",
          "[winui-textpeer][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    // The public two-phase construction contract permits querying the
    // model-only state before Create(). Keep this oracle beside the lifetime
    // loop so every scalar added to the WinUI header remains initialized.
    {
        wxTextCtrl text;
        long from = -1;
        long to = -1;
        text.GetSelection(&from, &to);
        CHECK(text.GetInsertionPoint() == 0);
        CHECK(from == 0);
        CHECK(to == 0);
        CHECK(text.IsEditable());
        CHECK_FALSE(text.IsModified());
        CHECK(text.GetValue().empty());
    }

    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();

    for ( int i = 0; i < 100; ++i )
    {
        wxTextCtrl * const text = new wxTextCtrl;
        REQUIRE(text->Create(parent, wxID_ANY));
        delete text;

#if wxUSE_SEARCHCTRL
        wxSearchCtrl * const search = new wxSearchCtrl;
        REQUIRE(search->Create(parent, wxID_ANY));
        delete search;
#endif // wxUSE_SEARCHCTRL
    }

    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() == baseline);
}

TEST_CASE("wxWinUI text Create survives destruction from Loaded",
          "[winui-textpeer][lifetime][reentrancy][create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();

    SECTION("TextCtrl")
    {
        wxTextCtrl *text = new wxTextCtrl;
        wxWeakRef<wxWindow> lifetime(text);
        CreateLoadedDestructionProbe<wxTextCtrl> probe;
        probe.ownerSlot = &text;
        wxWinUITextCtrlTestAccess::SetNextCreateLoadedHook(*text,
            &DestroyTextCtrlDuringCreateLoaded, &probe);

        wxTextCtrl * const invoking = text;
        CHECK_FALSE(invoking->Create(parent, wxID_ANY));
        CHECK(probe.invoked);
        CHECK(probe.ownerMatched);
        CHECK(text == nullptr);
        CHECK(lifetime.get() == nullptr);

        if ( text )
        {
            delete text;
            text = nullptr;
        }
    }

#if wxUSE_SEARCHCTRL
    SECTION("SearchCtrl")
    {
        wxSearchCtrl *search = new wxSearchCtrl;
        wxWeakRef<wxWindow> lifetime(search);
        CreateLoadedDestructionProbe<wxSearchCtrl> probe;
        probe.ownerSlot = &search;
        wxWinUISearchCtrlTestAccess::SetNextCreateLoadedHook(*search,
            &DestroySearchCtrlDuringCreateLoaded, &probe);

        wxSearchCtrl * const invoking = search;
        CHECK_FALSE(invoking->Create(parent, wxID_ANY));
        CHECK(probe.invoked);
        CHECK(probe.ownerMatched);
        CHECK(search == nullptr);
        CHECK(lifetime.get() == nullptr);

        if ( search )
        {
            delete search;
            search = nullptr;
        }
    }
#endif // wxUSE_SEARCHCTRL

    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() == baseline);
}

TEST_CASE("wxWinUI text Create reacquires after SetInitialSize",
          "[winui-textpeer][lifetime][reentrancy][create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();

    SECTION("TextCtrl")
    {
        using TestControl =
            DestroyOnInitialSizeControl<wxTextCtrl>;
        TestControl * const allocated = new TestControl;
        wxTextCtrl *text = allocated;
        wxWeakRef<wxWindow> lifetime(text);
        InitialSizeDestructionProbe<wxTextCtrl> probe;
        probe.ownerSlot = &text;
        wxWinUITextCtrlTestAccess::SetNextCreateLoadedHook(*text,
            &ArmTextCtrlInitialSizeDestruction, &probe);

        CHECK_FALSE(allocated->Create(
            parent, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxSize(247, 83),
            wxTE_MULTILINE));
        CHECK(probe.loaded);
        CHECK(probe.destroyed);
        CHECK(text == nullptr);
        CHECK(lifetime.get() == nullptr);

        if ( text )
        {
            delete text;
            text = nullptr;
        }
    }

#if wxUSE_SEARCHCTRL
    SECTION("SearchCtrl")
    {
        using TestControl =
            DestroyOnInitialSizeControl<wxSearchCtrl>;
        TestControl * const allocated = new TestControl;
        wxSearchCtrl *search = allocated;
        wxWeakRef<wxWindow> lifetime(search);
        InitialSizeDestructionProbe<wxSearchCtrl> probe;
        probe.ownerSlot = &search;
        wxWinUISearchCtrlTestAccess::SetNextCreateLoadedHook(*search,
            &ArmSearchCtrlInitialSizeDestruction, &probe);

        CHECK_FALSE(allocated->Create(
            parent, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxSize(247, 41)));
        CHECK(probe.loaded);
        CHECK(probe.destroyed);
        CHECK(search == nullptr);
        CHECK(lifetime.get() == nullptr);

        if ( search )
        {
            delete search;
            search = nullptr;
        }
    }
#endif // wxUSE_SEARCHCTRL

    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() == baseline);
}

TEST_CASE("wxWinUI TextCtrl applies wxTE_NO_VSCROLL to ContentElement",
          "[winui-textpeer][textctrl][scroll]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl noVerticalScroll(
        parent, wxID_ANY, "one\ntwo\nthree",
        wxDefaultPosition, wxSize(240, 90),
        wxTE_MULTILINE | wxTE_NO_VSCROLL);
    using Visibility =
        wxWinUITextCtrlTestAccess::ScrollBarVisibility;
    Visibility visibility = Visibility::Disabled;
    REQUIRE(WaitFor("wxTE_NO_VSCROLL ContentElement policy", [&]()
    {
        return wxWinUITextCtrlTestAccess::GetVerticalScrollBarVisibility(noVerticalScroll,
                       &visibility) &&
               visibility == Visibility::Hidden;
    }));
    CHECK(visibility == Visibility::Hidden);

    wxTextCtrl defaultScroll(
        parent, wxID_ANY, "one\ntwo\nthree",
        wxDefaultPosition, wxSize(240, 90),
        wxTE_MULTILINE);
    visibility = Visibility::Disabled;
    REQUIRE(WaitFor("default TextBox ContentElement policy", [&]()
    {
        return wxWinUITextCtrlTestAccess::GetVerticalScrollBarVisibility(defaultScroll, &visibility);
    }));
    CHECK(visibility == Visibility::Auto);
}

TEST_CASE("wxWinUI TextCtrl TOM visibility is bounded and lifetime-bound",
          "[winui-textpeer][textctrl][scroll][lifetime]")
{
    const std::size_t statesBefore =
        wxWinUITextCallbackState::GetLiveCountForTesting();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "text visibility lifetime",
                    wxPoint(48, 48), wxSize(280, 160),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxTextCtrl *text =
        new wxTextCtrl(frame, wxID_ANY, wxString(),
                       wxPoint(10, 10), wxSize(150, 80),
                       wxTE_MULTILINE | wxTE_RICH2);
    REQUIRE(MakeTestWindowTransparent(
        static_cast<HWND>(frame->GetHWND())));

    frame->ShowWithoutActivating();

    // Commit a substantially taller narrow-layout generation first. Resize
    // and replace the document without yielding so ContentElement can publish
    // the new smaller extent while TOM still owns the previous final line (or
    // vice versa). This is the real positive-extent generation contradiction
    // handled by the bounded reflow path; an empty document never creates it.
    text->SetSize(wxSize(72, 80));
    text->ChangeValue(wxString(2400, 'W'));
    double primedViewportHeight = 0.0;
    double primedScrollableHeight = 0.0;
    REQUIRE(WaitFor("narrow RichEditBox wrapped extent", [&]()
    {
        return wxWinUITextCtrlTestAccess::GetScrollState(*text,
                   nullptr,
                   nullptr,
                   nullptr,
                   &primedViewportHeight,
                   nullptr,
                   nullptr,
                   nullptr,
                   &primedScrollableHeight) &&
               primedViewportHeight > 0.0 &&
               primedScrollableHeight > primedViewportHeight * 4.0;
    }));

    text->SetSize(wxSize(150, 80));
    text->ChangeValue(wxString(1200, 'W'));
    text->SetSelection(7, 11);
    constexpr unsigned RichReflow = 1u << 0;
    constexpr unsigned RichExactRequested = 1u << 2;
    constexpr unsigned RichExactAccepted = 1u << 3;
    constexpr unsigned RichVisible = 1u << 6;
    constexpr unsigned RichMeasured = 1u << 7;
    text->ShowPosition(text->GetLastPosition());
    bool pending = false;
    unsigned retries = 0;
    bool retainedPeerReferences = false;
    unsigned richTrace = 0;
    unsigned richPassCount = 0;
    wxPoint end = wxDefaultPosition;
    double horizontalOffset = 0.0;
    double verticalOffset = 0.0;
    double viewportWidth = 0.0;
    double viewportHeight = 0.0;
    double rasterizationScale = 0.0;
    double scrollableWidth = 0.0;
    double scrollableHeight = 0.0;
    double viewOriginX = 0.0;
    double viewOriginY = 0.0;
    double viewUnitXScale = 0.0;
    double viewUnitYScale = 0.0;
    int scrollStage = 0;
    bool hasScrollState = false;
    hasScrollState = wxWinUITextCtrlTestAccess::GetScrollState(*text,
        &horizontalOffset,
        &verticalOffset,
        &viewportWidth,
        &viewportHeight,
        &rasterizationScale,
        &scrollStage,
        &scrollableWidth,
        &scrollableHeight,
        &viewOriginX,
        &viewOriginY,
        &viewUnitXScale,
        &viewUnitYScale);
    end = text->PositionToCoords(text->GetLastPosition());
    REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
        &pending,
        &retries,
        &retainedPeerReferences,
        &richTrace,
        &richPassCount));
    const bool initiallyNeededDeferredCompletion =
        scrollableHeight <= 0.5 &&
        (richTrace & RichVisible) == 0;
    const bool initiallyRetainedPeerReferences = retainedPeerReferences;
    if ( initiallyNeededDeferredCompletion )
    {
        // A zero wrapped extent cannot accept a final ChangeView. Production
        // must retain only its generation-bound callback state and finish the
        // exact XAML request on a bounded dispatcher turn.
        REQUIRE(pending);
        REQUIRE(retries == 1);
        REQUIRE_FALSE(initiallyRetainedPeerReferences);
    }
    const bool richCompletionVisible =
        WaitFor("deferred RichEditBox wrapped visibility", [&]()
    {
        hasScrollState = wxWinUITextCtrlTestAccess::GetScrollState(*text,
            &horizontalOffset,
            &verticalOffset,
            &viewportWidth,
            &viewportHeight,
            &rasterizationScale,
            &scrollStage,
            &scrollableWidth,
            &scrollableHeight,
            &viewOriginX,
            &viewOriginY,
            &viewUnitXScale,
            &viewUnitYScale);
        end = text->PositionToCoords(text->GetLastPosition());
        return wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
                   &pending,
                   &retries,
                   &retainedPeerReferences,
                   &richTrace,
                   &richPassCount) &&
               !pending &&
               (richTrace & RichVisible) != 0 &&
               end != wxDefaultPosition &&
               end.y >= 0 && end.y <= text->GetClientSize().y;
    }, 1000);
    CAPTURE(hasScrollState,
            initiallyNeededDeferredCompletion,
            primedViewportHeight,
            primedScrollableHeight,
            horizontalOffset,
            verticalOffset,
            viewportWidth,
            viewportHeight,
            rasterizationScale,
            scrollableWidth,
            scrollableHeight,
            viewOriginX,
            viewOriginY,
            viewUnitXScale,
            viewUnitYScale,
            scrollStage,
            end.x,
            end.y,
            text->GetClientSize().x,
            text->GetClientSize().y,
            pending,
            retries,
            retainedPeerReferences,
            richTrace,
            richPassCount);
    REQUIRE(richCompletionVisible);
    REQUIRE(hasScrollState);
    REQUIRE((richTrace & RichReflow) != 0);
    REQUIRE((richTrace & RichExactRequested) != 0);
    REQUIRE((richTrace & RichExactAccepted) != 0);
    REQUIRE((richTrace & RichVisible) != 0);
    REQUIRE((richTrace & RichMeasured) != 0);
    REQUIRE(richPassCount >= 2);
    REQUIRE(richPassCount <= 8);
    REQUIRE(end != wxDefaultPosition);
    REQUIRE(end.y >= 0);
    REQUIRE(end.y <= text->GetClientSize().y);
    CHECK_FALSE(pending);
    CHECK_FALSE(retainedPeerReferences);
    CHECK(retries <= 8);
    if ( initiallyNeededDeferredCompletion )
        CHECK(retries >= 1);
    long from = -1;
    long to = -1;
    text->GetSelection(&from, &to);
    CHECK(from == 7);
    CHECK(to == 11);

    // Force the real synchronous equivalent of TOM GetPoint(S_FALSE). The
    // production XAML coarse move must realize the range without queuing a
    // native TOM ScrollIntoView request, and a bounded pass must prove exact
    // visibility before the generation-bound transaction completes.
    text->ShowPosition(0);
    wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(*text);
    text->ShowPosition(text->GetLastPosition());
    richTrace = 0;
    richPassCount = 0;
    constexpr unsigned RichUnavailable = 1u << 1;
    constexpr unsigned RichCoarseRequested = 1u << 4;
    constexpr unsigned RichCoarseAccepted = 1u << 5;
    const bool coarseCompletionVisible =
        WaitFor("deferred unavailable RichEditBox visibility", [&]()
    {
        end = text->PositionToCoords(text->GetLastPosition());
        return wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
                   &pending,
                   &retries,
                   nullptr,
                   &richTrace,
                   &richPassCount) &&
               !pending &&
               (richTrace & RichVisible) != 0 &&
               end != wxDefaultPosition &&
               end.y >= 0 && end.y <= text->GetClientSize().y;
    }, 1000);
    CAPTURE(richTrace, richPassCount, end.x, end.y,
            text->GetClientSize().x, text->GetClientSize().y,
            pending, retries);
    REQUIRE(coarseCompletionVisible);
    REQUIRE((richTrace & RichUnavailable) != 0);
    REQUIRE((richTrace & RichCoarseRequested) != 0);
    REQUIRE((richTrace & RichCoarseAccepted) != 0);
    REQUIRE((richTrace & RichVisible) != 0);
    REQUIRE(richPassCount >= 2);
    REQUIRE(richPassCount <= 8);
    REQUIRE(end != wxDefaultPosition);
    REQUIRE(end.y >= 0);
    REQUIRE(end.y <= text->GetClientSize().y);
    text->GetSelection(&from, &to);
    CHECK(from == 7);
    CHECK(to == 11);

    // Destruction after either synchronous or bounded deferred completion must
    // release the exact shared callback state and all peer geometry.
    delete text;
    text = nullptr;
    REQUIRE(WaitFor("destroyed visibility callback quiescence", [&]()
    {
        return wxWinUITextCallbackState::GetLiveCountForTesting() ==
               statesBefore;
    }));

    frame->Destroy();
    wxYield();
}

TEST_CASE("wxWinUI TextBox visibility continuations are lifetime-bound",
          "[winui-textpeer][textctrl][scroll][lifetime][textbox-fallback]")
{
    wxFrame frame(nullptr, wxID_ANY, "TextBox continuation owner",
                  wxPoint(52, 52), wxSize(300, 180),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    REQUIRE(MakeTestWindowTransparent(
        static_cast<HWND>(frame.GetHWND())));
    frame.ShowWithoutActivating();

    SECTION("queued retry is inert after destruction")
    {
        const std::size_t statesBefore =
            wxWinUITextCallbackState::GetLiveCountForTesting();
        wxTextCtrl *text = new wxTextCtrl;
        wxWinUITextCtrlTestAccess::UseTextBoxPeer(*text);
        REQUIRE(text->Create(
            &frame, wxID_ANY, wxString(600, 'W'),
            wxPoint(10, 10), wxSize(160, 90), wxTE_MULTILINE));

        double viewportHeight = 0.0;
        REQUIRE(WaitFor("TextBox retry peer realization", [&]()
        {
            return wxWinUITextCtrlTestAccess::GetScrollState(*text,
                       nullptr, nullptr, nullptr, &viewportHeight,
                       nullptr) &&
                   viewportHeight > 0.0;
        }));

        wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(*text);
        text->ShowPosition(text->GetLastPosition());
        bool pending = false;
        unsigned retries = 0;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
            &pending, &retries));
        CHECK(pending);
        CHECK(retries == 1);

        delete text;
        text = nullptr;
        REQUIRE(WaitFor("destroyed TextBox retry quiescence", [&]()
        {
            return wxWinUITextCallbackState::GetLiveCountForTesting() ==
                   statesBefore;
        }));
    }

    SECTION("temporary selection restores across supersession")
    {
        const wxString value(600, 'W');
        wxTextCtrl text;
        wxWinUITextCtrlTestAccess::UseTextBoxPeer(text);
        REQUIRE(text.Create(
            &frame, wxID_ANY, value,
            wxPoint(10, 10), wxSize(160, 90), wxTE_MULTILINE));

        double viewportHeight = 0.0;
        REQUIRE(WaitFor("TextBox selection peer realization", [&]()
        {
            return wxWinUITextCtrlTestAccess::GetScrollState(text,
                       nullptr, nullptr, nullptr, &viewportHeight,
                       nullptr) &&
                   viewportHeight > 0.0;
        }));

        text.SetSelection(7, 11);
        TemporarySelectionSupersessionProbe probe;
        wxWinUITextCtrlTestAccess::SetNextTemporarySelectionHook(text,
            &SupersedeDuringTemporarySelection, &probe);
        text.ShowPosition(text.GetLastPosition());

        REQUIRE(probe.invoked);
        REQUIRE(probe.peerSelectionRead);
        CHECK(probe.publicFrom == 7);
        CHECK(probe.publicTo == 11);
        CHECK(probe.peerFrom == static_cast<long>(value.length() - 1));
        CHECK(probe.peerTo == static_cast<long>(value.length()));
        CHECK(probe.supersessionPending);
        CHECK(probe.supersessionRetries == 1);

        long from = -1;
        long to = -1;
        text.GetSelection(&from, &to);
        CHECK(from == 7);
        CHECK(to == 11);
        REQUIRE(wxWinUITextCtrlTestAccess::GetPeerSelection(text, &from, &to));
        CHECK(from == 7);
        CHECK(to == 11);

        bool pending = true;
        unsigned retries = 0;
        wxPoint start = wxDefaultPosition;
        REQUIRE(WaitFor("superseding TextBox continuation", [&]()
        {
            start = text.PositionToCoords(0);
            return wxWinUITextCtrlTestAccess::GetPositionVisibilityState(text,
                       &pending, &retries) &&
                   !pending && retries >= 1 && retries <= 3 &&
                   start != wxDefaultPosition &&
                   start.y >= 0 && start.y <= text.GetClientSize().y;
        }));
        CHECK_FALSE(pending);
        CHECK(retries >= 1);
        CHECK(retries <= 3);

        text.GetSelection(&from, &to);
        CHECK(from == 7);
        CHECK(to == 11);
        REQUIRE(wxWinUITextCtrlTestAccess::GetPeerSelection(text, &from, &to));
        CHECK(from == 7);
        CHECK(to == 11);
    }

    SECTION("Replace cancels a queued request before destruction")
    {
        const std::size_t statesBefore =
            wxWinUITextCallbackState::GetLiveCountForTesting();
        const wxString value(600, 'W');
        wxTextCtrl *text = new wxTextCtrl;
        wxWinUITextCtrlTestAccess::UseTextBoxPeer(*text);
        REQUIRE(text->Create(
            &frame, wxID_ANY, value,
            wxPoint(10, 10), wxSize(160, 90), wxTE_MULTILINE));

        double viewportHeight = 0.0;
        REQUIRE(WaitFor("TextBox Replace peer realization", [&]()
        {
            return wxWinUITextCtrlTestAccess::GetScrollState(*text,
                       nullptr, nullptr, nullptr, &viewportHeight,
                       nullptr) &&
                   viewportHeight > 0.0;
        }));

        wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(*text);
        text->ShowPosition(text->GetLastPosition());
        bool pending = false;
        unsigned retries = 0;
        bool retainedPeerReferences = false;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
            &pending, &retries, &retainedPeerReferences));
        REQUIRE(pending);
        REQUIRE(retries == 1);
        REQUIRE(retainedPeerReferences);

        text->Replace(0, 1, "R");
        CHECK(text->GetValue() == wxString("R") + value.Mid(1));
        pending = true;
        retries = 99;
        retainedPeerReferences = true;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
            &pending, &retries, &retainedPeerReferences));
        CHECK_FALSE(pending);
        CHECK(retries == 0);
        CHECK_FALSE(retainedPeerReferences);

        // The stale Low-priority closure still owns only callbackState. Delete
        // before it runs; its generation check must make it inert and release
        // that final shared state when the dispatcher drains.
        delete text;
        text = nullptr;
        REQUIRE(WaitFor("destroyed TextBox Replace continuation", [&]()
        {
            return wxWinUITextCallbackState::GetLiveCountForTesting() ==
                   statesBefore;
        }));
    }

    SECTION("Remove cancels a queued request before destruction")
    {
        const std::size_t statesBefore =
            wxWinUITextCallbackState::GetLiveCountForTesting();
        const wxString value(600, 'W');
        wxTextCtrl *text = new wxTextCtrl;
        wxWinUITextCtrlTestAccess::UseTextBoxPeer(*text);
        REQUIRE(text->Create(
            &frame, wxID_ANY, value,
            wxPoint(10, 10), wxSize(160, 90), wxTE_MULTILINE));

        double viewportHeight = 0.0;
        REQUIRE(WaitFor("TextBox Remove peer realization", [&]()
        {
            return wxWinUITextCtrlTestAccess::GetScrollState(*text,
                       nullptr, nullptr, nullptr, &viewportHeight,
                       nullptr) &&
                   viewportHeight > 0.0;
        }));

        wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(*text);
        text->ShowPosition(text->GetLastPosition());
        bool pending = false;
        unsigned retries = 0;
        bool retainedPeerReferences = false;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
            &pending, &retries, &retainedPeerReferences));
        REQUIRE(pending);
        REQUIRE(retries == 1);
        REQUIRE(retainedPeerReferences);

        text->Remove(0, 1);
        CHECK(text->GetValue() == value.Mid(1));
        pending = true;
        retries = 99;
        retainedPeerReferences = true;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(*text,
            &pending, &retries, &retainedPeerReferences));
        CHECK_FALSE(pending);
        CHECK(retries == 0);
        CHECK_FALSE(retainedPeerReferences);

        delete text;
        text = nullptr;
        REQUIRE(WaitFor("destroyed TextBox Remove continuation", [&]()
        {
            return wxWinUITextCallbackState::GetLiveCountForTesting() ==
                   statesBefore;
        }));
    }

    frame.Hide();
}

TEST_CASE("wxWinUI text events may destroy their control",
          "[winui-textpeer][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("TextCtrl WriteText")
    {
        wxTextCtrl *text = new wxTextCtrl(parent, wxID_ANY);
        text->Bind(wxEVT_TEXT, [&text](wxCommandEvent&)
        {
            wxTextCtrl * const doomed = text;
            text = nullptr;
            delete doomed;
        });
        text->WriteText("destroy");
        CHECK(text == nullptr);
    }

    SECTION("TextCtrl SetValue")
    {
        wxTextCtrl *text = new wxTextCtrl(parent, wxID_ANY);
        text->Bind(wxEVT_TEXT, [&text](wxCommandEvent&)
        {
            wxTextCtrl * const doomed = text;
            text = nullptr;
            delete doomed;
        });
        text->SetValue("destroy");
        CHECK(text == nullptr);
    }

    SECTION("PasswordBox injected content change")
    {
        wxTextCtrl *password = new wxTextCtrl(
            parent, wxID_ANY, "old",
            wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
        password->Bind(wxEVT_TEXT, [&password](wxCommandEvent&)
        {
            wxTextCtrl * const doomed = password;
            password = nullptr;
            delete doomed;
        });
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(*password, "new"));
        CHECK(password == nullptr);
    }

    SECTION("PasswordBox max-length event")
    {
        wxTextCtrl *password = new wxTextCtrl(
            parent, wxID_ANY, "ab",
            wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
        password->SetMaxLength(3);
        password->Bind(wxEVT_TEXT_MAXLEN, [&password](wxCommandEvent&)
        {
            wxTextCtrl * const doomed = password;
            password = nullptr;
            delete doomed;
        });
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(*password, "abXY"));
        CHECK(password == nullptr);
    }

#if wxUSE_SEARCHCTRL
    SECTION("SearchCtrl WriteText")
    {
        wxSearchCtrl *search = new wxSearchCtrl(parent, wxID_ANY);
        search->Bind(wxEVT_TEXT, [&search](wxCommandEvent&)
        {
            wxSearchCtrl * const doomed = search;
            search = nullptr;
            delete doomed;
        });
        search->WriteText("destroy");
        CHECK(search == nullptr);
    }
#endif // wxUSE_SEARCHCTRL

    wxYield();
}

TEST_CASE("wxWinUI TextCtrl value policies",
          "[winui-textpeer][textctrl]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("max length applies to user paste only")
    {
        wxTextCtrl text(parent, wxID_ANY);
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(3);
        wxWinUITextCtrlTestAccess::PasteText(text, "abcdef");
        CHECK(text.GetValue() == "abc");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 1);

        text.ChangeValue("");
        textEvents.Clear();
        maxEvents.Clear();
        text.WriteText("abcdef");
        CHECK(text.GetValue() == "abcdef");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 0);
    }

    SECTION("PasswordBox complete replacement proposal")
    {
        wxTextCtrl password(parent, wxID_ANY, "ab",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PASSWORD);
        std::vector<int> eventOrder;
        unsigned textEvents = 0;
        unsigned maxEvents = 0;
        password.Bind(wxEVT_TEXT, [&](wxCommandEvent&)
        {
            ++textEvents;
            eventOrder.push_back(1);
        });
        password.Bind(wxEVT_TEXT_MAXLEN, [&](wxCommandEvent&)
        {
            ++maxEvents;
            eventOrder.push_back(2);
        });

        password.SetMaxLength(3);
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "WXYZ"));
        CHECK(password.GetValue() == "WXY");
        CHECK(textEvents == 1);
        CHECK(maxEvents == 1);
        REQUIRE(eventOrder.size() == 2);
        CHECK(eventOrder[0] == 1);
        CHECK(eventOrder[1] == 2);

        textEvents = 0;
        maxEvents = 0;
        eventOrder.clear();
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "ok"));
        CHECK(password.GetValue() == "ok");
        CHECK(textEvents == 1);
        CHECK(maxEvents == 0);
        REQUIRE(eventOrder.size() == 1);
        CHECK(eventOrder[0] == 1);
    }

    SECTION("PasswordBox complete composed edits report max length")
    {
        wxTextCtrl password(parent, wxID_ANY, "ab",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PASSWORD);
        EventCounter textEvents(&password, wxEVT_TEXT);
        EventCounter maxEvents(&password, wxEVT_TEXT_MAXLEN);

        password.SetMaxLength(3);

        // This deterministic seam explicitly injects the same complete
        // proposal the real PasswordChanged callback consumes. It does not
        // pretend that assigning Password raises PasswordChanging.
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "abXY"));
        CHECK(password.GetValue() == "abX");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 1);

        password.ChangeValue("ab");
        textEvents.Clear();
        maxEvents.Clear();
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password,
                wxS("ab\u00e9")));
        CHECK(password.GetValue() == wxS("ab\u00e9"));
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 0);

        // A composed/dead-key character attempted at the limit is rejected
        // through the content-change path, not guessed from virtual keys.
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password,
                wxS("ab\u00e9\u00df")));
        CHECK(password.GetValue() == wxS("ab\u00e9"));
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 1);

        password.SetMaxLength(5);
        password.ChangeValue("abcd");
        textEvents.Clear();
        maxEvents.Clear();
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "abXYcd"));
        CHECK(password.GetValue() == "abXcd");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 1);

        password.ChangeValue("abcdef");
        password.SetMaxLength(3);
        textEvents.Clear();
        maxEvents.Clear();
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "abcde"));
        CHECK(password.GetValue() == "abcde");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 0);
    }

    SECTION("Rich text peer accepts the fitting overflow prefix")
    {
        wxTextCtrl text(parent, wxID_ANY, "ab");
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(3);
        REQUIRE(wxWinUITextCtrlTestAccess::SetTextBoxPeerText(text, "abcd"));
        REQUIRE(WaitFor("RichEditBox fitting overflow prefix", [&]()
        {
            return text.GetValue() == "abc" &&
                   textEvents.GetCount() == 1 &&
                   maxEvents.GetCount() == 1;
        }));
    }

    SECTION("plain multiline peer counts public CRLF positions")
    {
        wxTextCtrl text(parent, wxID_ANY, wxString(),
                        wxDefaultPosition, wxDefaultSize,
                        wxTE_MULTILINE);
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(3);
        REQUIRE(wxWinUITextCtrlTestAccess::SetTextBoxPeerText(text, "a\nX"));
        wxString peerValue;
        REQUIRE(WaitFor("RichEditBox public CRLF max length", [&]()
        {
            return text.GetValue() == "a\n" &&
                   text.GetLastPosition() == 3 &&
                   wxWinUITextCtrlTestAccess::GetPeerText(text, &peerValue) &&
                   peerValue == "a\n" &&
                   textEvents.GetCount() == 1 &&
                   maxEvents.GetCount() == 1;
        }));
        CHECK(text.GetValue() == "a\n");
        CHECK(text.GetLastPosition() == 3);
        CHECK(peerValue == "a\n");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 1);
    }

    SECTION("peer max length never splits a UTF-16 surrogate pair")
    {
        wxTextCtrl text(parent, wxID_ANY);
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(1);
        REQUIRE(wxWinUITextCtrlTestAccess::SetTextBoxPeerText(text,
            wxS("\U0001f600")));
        wxString peerValue = "not yet corrected";
        REQUIRE(WaitFor("RichEditBox surrogate-safe max length", [&]()
        {
            return text.GetValue().empty() &&
                   wxWinUITextCtrlTestAccess::GetPeerText(text, &peerValue) &&
                   peerValue.empty() &&
                   textEvents.GetCount() == 0 &&
                   maxEvents.GetCount() == 1;
        }));
        CHECK(text.GetValue().empty());
        CHECK(peerValue.empty());
        CHECK(textEvents.GetCount() == 0);
        CHECK(maxEvents.GetCount() == 1);
    }

    SECTION("overflow replacement remaps the TOM selection before its suffix")
    {
        wxTextCtrl text(parent, wxID_ANY, "a\nXYcd",
                        wxDefaultPosition, wxDefaultSize,
                        wxTE_MULTILINE);
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        // These are public CRLF coordinates: [3, 5) selects XY.
        text.SetMaxLength(8);
        text.SetSelection(3, 5);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(text, "123456"));

        wxString peerValue;
        long selectionFrom = -1;
        long selectionTo = -1;
        long peerSelectionFrom = -1;
        long peerSelectionTo = -1;
        REQUIRE(WaitFor("TOM overflow selection remap before suffix", [&]()
        {
            text.GetSelection(&selectionFrom, &selectionTo);
            return text.GetValue() == "a\n123cd" &&
                   text.GetInsertionPoint() == 3 &&
                   selectionFrom == 3 && selectionTo == 6 &&
                   wxWinUITextCtrlTestAccess::GetPeerText(text, &peerValue) &&
                   peerValue == "a\n123cd" &&
                   wxWinUITextCtrlTestAccess::GetPeerSelection(text,
                       &peerSelectionFrom, &peerSelectionTo) &&
                   peerSelectionFrom == 3 && peerSelectionTo == 6 &&
                   textEvents.GetCount() == 1 &&
                   maxEvents.GetCount() == 1;
        }));
    }

    SECTION("overflow replacement does not share a low surrogate")
    {
        wxTextCtrl text(parent, wxID_ANY, wxS("A\U00010000Z"));
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(5);
        text.SetSelection(1, 3);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(text,
            wxS("XX\U00010400")));

        wxString peerValue;
        long selectionFrom = -1;
        long selectionTo = -1;
        long peerSelectionFrom = -1;
        long peerSelectionTo = -1;
        REQUIRE(WaitFor("TOM shared-low-surrogate boundary", [&]()
        {
            text.GetSelection(&selectionFrom, &selectionTo);
            return text.GetValue() == "AXXZ" &&
                   text.GetInsertionPoint() == 1 &&
                   selectionFrom == 1 && selectionTo == 3 &&
                   wxWinUITextCtrlTestAccess::GetPeerText(text, &peerValue) &&
                   peerValue == "AXXZ" &&
                   wxWinUITextCtrlTestAccess::GetPeerSelection(text,
                       &peerSelectionFrom, &peerSelectionTo) &&
                   peerSelectionFrom == 1 && peerSelectionTo == 3 &&
                   textEvents.GetCount() == 1 &&
                   maxEvents.GetCount() == 1;
        }));
    }

    SECTION("overflow replacement does not migrate a combining mark")
    {
        wxTextCtrl text(parent, wxID_ANY, wxS("Aa\u0301Z"));
        EventCounter textEvents(&text, wxEVT_TEXT);
        EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);

        text.SetMaxLength(5);
        text.SetSelection(1, 3);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(text,
            wxS("XXb\u0301")));

        wxString peerValue;
        long selectionFrom = -1;
        long selectionTo = -1;
        long peerSelectionFrom = -1;
        long peerSelectionTo = -1;
        REQUIRE(WaitFor("TOM combining-cluster boundary", [&]()
        {
            text.GetSelection(&selectionFrom, &selectionTo);
            return text.GetValue() == "AXXZ" &&
                   text.GetInsertionPoint() == 1 &&
                   selectionFrom == 1 && selectionTo == 3 &&
                   wxWinUITextCtrlTestAccess::GetPeerText(text, &peerValue) &&
                   peerValue == "AXXZ" &&
                   wxWinUITextCtrlTestAccess::GetPeerSelection(text,
                       &peerSelectionFrom, &peerSelectionTo) &&
                   peerSelectionFrom == 1 && peerSelectionTo == 3 &&
                   textEvents.GetCount() == 1 &&
                   maxEvents.GetCount() == 1;
        }));
    }

    SECTION("programmatic PasswordBox writes may exceed user limit")
    {
        wxTextCtrl password(parent, wxID_ANY, wxString(),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PASSWORD);
        EventCounter textEvents(&password, wxEVT_TEXT);
        EventCounter maxEvents(&password, wxEVT_TEXT_MAXLEN);

        password.SetMaxLength(3);
        password.WriteText("abcdef");
        CHECK(password.GetValue() == "abcdef");
        CHECK(textEvents.GetCount() == 1);
        CHECK(maxEvents.GetCount() == 0);
    }

    SECTION("programmatic value resets dirty and changed caret only")
    {
        wxTextCtrl text(parent, wxID_ANY);
        text.MarkDirty();
        text.SetInsertionPoint(0);
        text.SetValue("abc");
        CHECK_FALSE(text.IsModified());
        CHECK(text.GetInsertionPoint() == 0);

        text.SetInsertionPoint(2);
        text.SetValue("abc");
        CHECK(text.GetInsertionPoint() == 2);
        CHECK_FALSE(text.IsModified());
    }

    SECTION("readonly peers reject external text mutation")
    {
        wxTextCtrl plain(parent, wxID_ANY, "model",
                         wxDefaultPosition, wxDefaultSize,
                         wxTE_READONLY);

        wxString plainPeerValue;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPeerText(plain, &plainPeerValue));
        CHECK(plainPeerValue == "model");

        // wxTE_READONLY protects against user/native peer edits, not public
        // programmatic value assignment. RichEditBox's TOM is temporarily
        // unlocked by the production projection and immediately restored.
        plain.ChangeValue("programmatic");
        CHECK(plain.GetValue() == "programmatic");
        REQUIRE(wxWinUITextCtrlTestAccess::GetPeerText(plain, &plainPeerValue));
        CHECK(plainPeerValue == "programmatic");

        REQUIRE(wxWinUITextCtrlTestAccess::SetTextBoxPeerText(plain, "peer"));
        CHECK(plain.GetValue() == "programmatic");
        REQUIRE(wxWinUITextCtrlTestAccess::GetPeerText(plain, &plainPeerValue));
        CHECK(plainPeerValue == "programmatic");

        wxTextCtrl password(parent, wxID_ANY, "secret",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PASSWORD | wxTE_READONLY);
        EventCounter passwordTextEvents(&password, wxEVT_TEXT);
        EventCounter passwordMaxEvents(&password, wxEVT_TEXT_MAXLEN);
        password.SetMaxLength(3);
        REQUIRE(
            wxWinUITextCtrlTestAccess::InjectPasswordContentChange(password, "peer"));
        CHECK(password.GetValue() == "secret");
        CHECK(passwordTextEvents.GetCount() == 0);
        CHECK(passwordMaxEvents.GetCount() == 0);

        // Temporarily unlocking only the test projection forces a real TOM
        // content transaction while the wx control remains logically
        // read-only. The production branch must restore and read back the
        // complete mask through the shared scrubber before returning.
        password.SelectAll();
        PasswordTextChangingUIAProbe readOnlyUIA;
        wxWinUITextCtrlTestAccess::SetNextPasswordTextChangingHook(password,
            QueryPasswordUIADuringTextChanging, &readOnlyUIA);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password,
            "transient-read-only-clear"));
        CHECK(readOnlyUIA.invoked);
        REQUIRE(readOnlyUIA.querySucceeded);
        CHECK(readOnlyUIA.isPassword);
        CHECK_FALSE(readOnlyUIA.hasValuePattern);
        CHECK_FALSE(readOnlyUIA.hasTextPattern);
        CHECK(password.GetValue() == "secret");
        CHECK(passwordTextEvents.GetCount() == 0);
        CHECK(passwordMaxEvents.GetCount() == 0);

        bool isPassword = false;
        bool hasValuePattern = true;
        bool hasTextPattern = true;
        bool predictionEnabled = true;
        wxString peerDocument;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(password,
            &isPassword,
            &hasValuePattern,
            &hasTextPattern,
            &peerDocument,
            &predictionEnabled));
        wxString expectedMask;
        for ( std::size_t n = 0; n != password.GetValue().length(); ++n )
            expectedMask += static_cast<wxChar>(0x25cf);
        CHECK(peerDocument == expectedMask);
        CHECK_FALSE(wxWinUITextCtrlTestAccess::GetPasswordFailClosed(password,
            nullptr, nullptr));

        // Also model a successful but partial SetText implementation. Exact
        // read-back must detect it, clear the document and retire the peer.
        wxTextCtrl partial(parent, wxID_ANY, "read-only-secret",
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_PASSWORD | wxTE_READONLY);
        EventCounter partialTextEvents(&partial, wxEVT_TEXT);
        partial.SelectAll();
        wxWinUITextCtrlTestAccess::ForceNextPasswordScrubPartialWrite(partial);
        CHECK_FALSE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(partial,
            "transient-partial-clear"));
        long scrubFailure = 0;
        bool documentWasEmpty = false;
        REQUIRE(wxWinUITextCtrlTestAccess::GetPasswordFailClosed(partial,
            &scrubFailure, &documentWasEmpty));
        CHECK(scrubFailure == static_cast<long>(E_UNEXPECTED));
        CHECK(documentWasEmpty);
        CHECK(partialTextEvents.GetCount() == 0);
        CHECK(partial.GetValue() == "read-only-secret");
        CHECK_FALSE(wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(partial,
            &isPassword,
            &hasValuePattern,
            &hasTextPattern,
            nullptr,
            &predictionEnabled));

#if wxUSE_SEARCHCTRL
        wxSearchCtrl search(parent, wxID_ANY, "query",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_READONLY);
        REQUIRE(wxWinUISearchCtrlTestAccess::SetPeerText(search, "peer"));
        CHECK(search.GetValue() == "query");
#endif // wxUSE_SEARCHCTRL
    }

    SECTION("native TextBox hint and PasswordBox rejection")
    {
        wxTextCtrl plain(parent, wxID_ANY);
        REQUIRE(plain.SetHint("native placeholder"));
        CHECK(plain.GetHint() == "native placeholder");
        REQUIRE(plain.SetHint(wxString()));
        CHECK(plain.GetHint().empty());

        wxTextCtrl password(parent, wxID_ANY, wxString(),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PASSWORD);
        CHECK_FALSE(password.SetHint("must not be shown"));
        CHECK(password.GetHint().empty());
    }

    SECTION("null font clears every native font override")
    {
        wxTextCtrl text(parent, wxID_ANY);
        CHECK_FALSE(wxWinUITextCtrlTestAccess::HasLocalFontOverrides(text));

        wxFont custom = text.GetFont();
        custom.SetFaceName("Courier New");
        custom.SetPointSize(wxMax(10, custom.GetPointSize() + 2));
        custom.SetWeight(wxFONTWEIGHT_BOLD);
        custom.SetStyle(wxFONTSTYLE_ITALIC);
        REQUIRE(custom.IsOk());
        text.SetFont(custom);
        CHECK(wxWinUITextCtrlTestAccess::HasLocalFontOverrides(text));

        text.SetFont(wxNullFont);
        CHECK_FALSE(wxWinUITextCtrlTestAccess::HasLocalFontOverrides(text));
    }
}

TEST_CASE("wxWinUI TextCtrl projects inherited text-entry contracts",
          "[winui-textpeer][textctrl][textentry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl text(parent, wxID_ANY, "mixedCase");

#if wxUSE_RICHEDIT
    CHECK(text.GetRichVersion() == 0);
#endif
    CHECK_FALSE(text.IsInkEdit());

    REQUIRE(text.SetMargins(11, -1));
    const wxPoint margins = text.GetMargins();
    CHECK(std::abs(margins.x - 11) <= 1);
    CHECK(margins.y == -1);

    text.SetSelection(1, 4);
    text.ForceUpper();
    CHECK(text.GetValue() == "MIXEDCASE");
    long from = -1;
    long to = -1;
    text.GetSelection(&from, &to);
    CHECK(from == 1);
    CHECK(to == 4);
    text.WriteText("xy");
    CHECK(text.GetValue() == "MXYDCASE");

    EventCounter textEvents(&text, wxEVT_TEXT);
    text.ForwardEnableTextChangedEvents(false);
    text.SetValue("suppressed");
    CHECK(text.GetValue() == "SUPPRESSED");
    CHECK(textEvents.GetCount() == 0);
    text.ForwardEnableTextChangedEvents(true);
    text.SetValue("reported");
    CHECK(text.GetValue() == "REPORTED");
    CHECK(textEvents.GetCount() == 1);

    bool caretShown = false;
    REQUIRE(wxWinUITextCtrlTestAccess::GetNativeCaretShown(text, &caretShown));
    CHECK(caretShown);
    REQUIRE(text.HideNativeCaret());
    REQUIRE(wxWinUITextCtrlTestAccess::GetNativeCaretShown(text, &caretShown));
    CHECK_FALSE(caretShown);
    REQUIRE(text.ShowNativeCaret());
    REQUIRE(wxWinUITextCtrlTestAccess::GetNativeCaretShown(text, &caretShown));
    CHECK(caretShown);

    CHECK_FALSE(wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(text));
    text.SetWindowStyleFlag(
        text.GetWindowStyleFlag() | wxTE_NOHIDESEL);
    CHECK(wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(text));
    text.SetWindowStyleFlag(
        text.GetWindowStyleFlag() & ~wxTE_NOHIDESEL);
    CHECK_FALSE(wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(text));
}

TEST_CASE("wxWinUI password peer is masked, selectable and UIA-protected",
          "[winui-textpeer][textctrl][password][security]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl password(parent, wxID_ANY, "s3cret",
                        wxDefaultPosition, wxDefaultSize,
                        wxTE_PASSWORD);
    password.SetSelection(1, 4);
    long from = -1;
    long to = -1;
    password.GetSelection(&from, &to);
    CHECK(from == 1);
    CHECK(to == 4);
    REQUIRE(wxWinUITextCtrlTestAccess::GetPeerSelection(password, &from, &to));
    CHECK(from == 1);
    CHECK(to == 4);

    CHECK_FALSE(password.CanCopy());
    CHECK_FALSE(password.CanCut());

    bool caretShown = false;
    REQUIRE(wxWinUITextCtrlTestAccess::GetNativeCaretShown(password, &caretShown));
    CHECK(caretShown);
    REQUIRE(password.HideNativeCaret());
    REQUIRE(wxWinUITextCtrlTestAccess::GetNativeCaretShown(password, &caretShown));
    CHECK_FALSE(caretShown);
    REQUIRE(password.ShowNativeCaret());

    CHECK_FALSE(wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(password));
    password.SetWindowStyleFlag(
        password.GetWindowStyleFlag() | wxTE_NOHIDESEL);
    CHECK(wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(password));

    bool isPassword = false;
    bool hasValuePattern = true;
    bool hasTextPattern = true;
    bool predictionEnabled = true;
    wxString peerDocument;
    REQUIRE(wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(password,
        &isPassword,
        &hasValuePattern,
        &hasTextPattern,
        &peerDocument,
        &predictionEnabled));
    CHECK(isPassword);
    CHECK_FALSE(hasValuePattern);
    CHECK_FALSE(hasTextPattern);
    CHECK_FALSE(predictionEnabled);
    wxString expectedMask;
    for ( std::size_t n = 0; n != password.GetValue().length(); ++n )
        expectedMask += static_cast<wxChar>(0x25cf);
    CHECK(peerDocument == expectedMask);
    CHECK(peerDocument.Find("s3cret") == wxNOT_FOUND);

    unsigned copyEvents = 0;
    unsigned cutEvents = 0;
    unsigned pasteEvents = 0;
    password.Bind(wxEVT_TEXT_COPY,
                  [&](wxClipboardTextEvent& event)
                  {
                      ++copyEvents;
                      event.Skip();
                  });
    password.Bind(wxEVT_TEXT_CUT,
                  [&](wxClipboardTextEvent& event)
                  {
                      ++cutEvents;
                      event.Skip();
                  });
    password.Bind(wxEVT_TEXT_PASTE,
                  [&](wxClipboardTextEvent&) { ++pasteEvents; });
    password.Copy();
    password.Cut();
    password.Paste();
    CHECK(copyEvents == 1);
    CHECK(cutEvents == 1);
    CHECK(pasteEvents == 1);
    CHECK(password.GetValue() == "s3cret");
    CHECK(wxWinUITextCtrlTestAccess::GetPasswordClipboardExportAttemptCount(password) == 0);

    // Exercise the real TOM edit transaction: clear text exists only during
    // synchronous TextChanging and is replaced with mask glyphs pre-render.
    // The provider-facing peer is queried from inside that callback, which is
    // also the security window used by every IME composition update.
    PasswordTextChangingUIAProbe transientUIA;
    wxWinUITextCtrlTestAccess::SetNextPasswordTextChangingHook(password,
        QueryPasswordUIADuringTextChanging, &transientUIA);
    REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password, "XY"));
    CHECK(transientUIA.invoked);
    REQUIRE(transientUIA.querySucceeded);
    CHECK(transientUIA.isPassword);
    CHECK_FALSE(transientUIA.hasValuePattern);
    CHECK_FALSE(transientUIA.hasTextPattern);
    CHECK(password.GetValue() == "sXYet");
    CHECK(password.CanUndo());
    password.Undo();
    CHECK(password.GetValue() == "s3cret");
    CHECK(password.CanRedo());
    password.Redo();
    CHECK(password.GetValue() == "sXYet");

    // A literal password character equal to the visual mask is still data,
    // not a formatting-only notification. The content-changing transaction
    // and pre-edit selection disambiguate it from the backing bullets.
    wxString literalMask;
    literalMask += static_cast<wxChar>(0x25cf);
    password.SetSelection(1, 2);
    PasswordTextChangingUIAProbe literalMaskUIA;
    wxWinUITextCtrlTestAccess::SetNextPasswordTextChangingHook(password,
        QueryPasswordUIADuringTextChanging, &literalMaskUIA);
    REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password, literalMask));
    CHECK(literalMaskUIA.invoked);
    REQUIRE(literalMaskUIA.querySucceeded);
    CHECK(literalMaskUIA.isPassword);
    CHECK_FALSE(literalMaskUIA.hasValuePattern);
    CHECK_FALSE(literalMaskUIA.hasTextPattern);
    wxString expectedValue = "s";
    expectedValue += literalMask;
    expectedValue += "Yet";
    CHECK(password.GetValue() == expectedValue);

    // Never split a UTF-16 surrogate pair at the native max-length boundary.
    EventCounter maxLengthEvents(&password, wxEVT_TEXT_MAXLEN);
    password.SetMaxLength(6);
    password.SetInsertionPointEnd();
    REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password,
        wxS("\U0001f600")));
    CHECK(password.GetValue() == expectedValue);
    CHECK(maxLengthEvents.GetCount() == 1);

    REQUIRE(wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(password,
        &isPassword,
        &hasValuePattern,
        &hasTextPattern,
        &peerDocument,
        &predictionEnabled));
    CHECK(peerDocument.Find(expectedValue) == wxNOT_FOUND);
    CHECK(peerDocument.length() == password.GetValue().length());
}

TEST_CASE("wxWinUI password scrub failure is terminal and leak-free",
          "[winui-textpeer][textctrl][password][security][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl password(parent, wxID_ANY, "retained-secret",
                        wxDefaultPosition, wxDefaultSize,
                        wxTE_PASSWORD);
    password.SelectAll();
    EventCounter textEvents(&password, wxEVT_TEXT);

    PasswordTextChangingUIAProbe transientUIA;
    wxWinUITextCtrlTestAccess::SetNextPasswordTextChangingHook(password,
        QueryPasswordUIADuringTextChanging, &transientUIA);
    wxWinUITextCtrlTestAccess::ForceNextPasswordScrubFailure(password,
        static_cast<long>(E_FAIL));

    // The test seam fails both ways of writing the mask while clear text is
    // present in the real TOM edit transaction. The independent empty-story
    // fallback must be proved before the peer is retired.
    CHECK_FALSE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password,
        "transient-clear-text"));
    CHECK(transientUIA.invoked);
    REQUIRE(transientUIA.querySucceeded);
    CHECK(transientUIA.isPassword);
    CHECK_FALSE(transientUIA.hasValuePattern);
    CHECK_FALSE(transientUIA.hasTextPattern);

    long failure = 0;
    bool documentWasEmpty = false;
    REQUIRE(wxWinUITextCtrlTestAccess::GetPasswordFailClosed(password,
        &failure, &documentWasEmpty));
    CHECK(failure == static_cast<long>(E_FAIL));
    CHECK(documentWasEmpty);
    CHECK(textEvents.GetCount() == 0);
    CHECK(password.GetValue() == "retained-secret");

    bool isPassword = false;
    bool hasValuePattern = true;
    bool hasTextPattern = true;
    bool predictionEnabled = true;
    CHECK_FALSE(wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(password,
        &isPassword,
        &hasValuePattern,
        &hasTextPattern,
        nullptr,
        &predictionEnabled));
    CHECK_FALSE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(password, "again"));
    CHECK(textEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI TextCtrl AUTO_URL is RichEdit-compatible and hot",
          "[winui-textpeer][textctrl][url][style]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString value =
        "www.wxwidgets.org, mailto:user@example.com and "
        "(https://example.test/path).";
    wxTextCtrl text(parent, wxID_ANY, value);
    CHECK_FALSE(text.IsRich());
    CHECK(wxWinUITextCtrlTestAccess::GetAutoUrlRangeCount(text) == 0);

    text.SetWindowStyleFlag(
        text.GetWindowStyleFlag() | wxTE_AUTO_URL);
    CHECK(text.IsRich());
    CHECK(text.HasFlag(wxTE_RICH));
    CHECK(text.HasFlag(wxTE_RICH2));
#if wxUSE_RICHEDIT
    CHECK(text.GetRichVersion() == 4);
#endif
    REQUIRE(wxWinUITextCtrlTestAccess::GetAutoUrlRangeCount(text) == 3);

    const wxString expectedText[] =
    {
        "www.wxwidgets.org",
        "mailto:user@example.com",
        "https://example.test/path"
    };
    const wxString expectedTarget[] =
    {
        "http://www.wxwidgets.org",
        "mailto:user@example.com",
        "https://example.test/path"
    };
    for ( unsigned n = 0; n != WXSIZEOF(expectedText); ++n )
    {
        long from = -1;
        long to = -1;
        wxString target;
        REQUIRE(wxWinUITextCtrlTestAccess::GetAutoUrlRange(text,
            n, &from, &to, &target));
        CHECK(text.GetRange(from, to) == expectedText[n]);
        CHECK(target == expectedTarget[n]);
    }

    text.SetWindowStyleFlag(
        text.GetWindowStyleFlag() & ~wxTE_AUTO_URL);
    CHECK(wxWinUITextCtrlTestAccess::GetAutoUrlRangeCount(text) == 0);
    // Once promoted, this matches wxMSW's non-downgradable native peer.
    CHECK(text.IsRich());

    wxTextCtrl createdWithUrl(parent, wxID_ANY, "https://example.test",
                              wxDefaultPosition, wxDefaultSize,
                              wxTE_AUTO_URL);
    CHECK(createdWithUrl.IsRich());
    CHECK(createdWithUrl.HasFlag(wxTE_RICH));
    CHECK(createdWithUrl.HasFlag(wxTE_RICH2));
    CHECK(wxWinUITextCtrlTestAccess::GetAutoUrlRangeCount(createdWithUrl) == 1);
}

TEST_CASE("wxWinUI TextCtrl owns an actionable autocomplete flyout",
          "[winui-textpeer][textctrl][autocomplete]")
{
    wxFrame frame(nullptr, wxID_ANY, "text completion owner",
                  wxPoint(56, 56), wxSize(300, 130),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxTextCtrl text(&frame, wxID_ANY, wxString(),
                    wxPoint(10, 10), wxSize(180, 36));
    REQUIRE(MakeTestWindowTransparent(
        static_cast<HWND>(frame.GetHWND())));
    frame.ShowWithoutActivating();
    REQUIRE(WaitFor("TextCtrl completion XamlRoot", [&text]()
    {
        return wxWinUITextCtrlTestAccess::GetScrollState(text,
            nullptr, nullptr, nullptr, nullptr, nullptr);
    }, 1000));

    text.ChangeValue("al");
    text.SetInsertionPointEnd();
    wxArrayString completions;
    completions.Add("alpha");
    completions.Add("alpine");
    completions.Add("beta");
    REQUIRE(text.AutoComplete(completions));
    REQUIRE(WaitFor("TextCtrl completion flyout", [&text]()
    {
        return wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestionCount(text) == 2;
    }, 1000));
    CHECK(wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestion(text, 0) == "alpha");
    CHECK(wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestion(text, 1) == "alpine");

    EventCounter textEvents(&text, wxEVT_TEXT);
    REQUIRE(wxWinUITextCtrlTestAccess::InvokeAutoCompleteSuggestion(text, 1));
    CHECK(text.GetValue() == "alpine");
    CHECK(text.GetInsertionPoint() == text.GetLastPosition());
    CHECK(textEvents.GetCount() == 1);
    CHECK(wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestionCount(text) == 0);

    frame.Hide();
}

TEST_CASE("wxWinUI TextCtrl completers may destroy their owner",
          "[winui-textpeer][textctrl][autocomplete][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "text completion lifetime");
    frame.Show();

    wxTextCtrl *text = new wxTextCtrl(&frame, wxID_ANY, "al");
    text->SetInsertionPointEnd();
    const bool startResult = text->AutoComplete(
        new DeleteTextCtrlOnStartCompleter(&text));
    CHECK(startResult);
    CHECK(text == nullptr);

    text = new wxTextCtrl(&frame, wxID_ANY, "al");
    bool armed = false;
    REQUIRE(text->AutoComplete(
        new DeleteTextCtrlOnDestroyCompleter(&text, &armed)));
    armed = true;
    wxArrayString replacement;
    replacement.Add("alpha");
    const bool replacementResult = text->AutoComplete(replacement);
    CHECK(replacementResult);
    CHECK(text == nullptr);
}

TEST_CASE("wxTextCtrl::EmptyUndoBuffer native peer contract",
          "[winui-textpeer][textctrl][undo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto checkClearPreservesState =
        [](wxTextCtrl& text, EventCounter& textEvents)
        {
            const wxString value = text.GetValue();
            long selectionFrom = -1;
            long selectionTo = -1;
            text.GetSelection(&selectionFrom, &selectionTo);
            const bool modified = text.IsModified();
            wxWindow * const focus = wxWindow::FindFocus();
            const int eventCount = textEvents.GetCount();

            text.EmptyUndoBuffer();

            CHECK_FALSE(text.CanUndo());
            CHECK_FALSE(text.CanRedo());
            CHECK(text.GetValue() == value);
            long selectionFromAfter = -1;
            long selectionToAfter = -1;
            text.GetSelection(&selectionFromAfter, &selectionToAfter);
            CHECK(selectionFromAfter == selectionFrom);
            CHECK(selectionToAfter == selectionTo);
            CHECK(text.IsModified() == modified);
            CHECK(wxWindow::FindFocus() == focus);
            CHECK(textEvents.GetCount() == eventCount);
        };

    SECTION("clear undo stack")
    {
        wxTextCtrl text(parent, wxID_ANY, "abcdef");
        EventCounter textEvents(&text, wxEVT_TEXT);
        text.SetSelection(1, 4);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(text, "XYZ"));
        REQUIRE(WaitFor("TextBox undo history", [&]()
        {
            return text.GetValue() == "aXYZef" &&
                   text.CanUndo();
        }, 1000));
        text.MarkDirty();

        checkClearPreservesState(text, textEvents);
    }

    SECTION("clear redo stack")
    {
        wxTextCtrl text(parent, wxID_ANY, "abcdef");
        EventCounter textEvents(&text, wxEVT_TEXT);
        text.SetSelection(1, 4);
        REQUIRE(wxWinUITextCtrlTestAccess::ReplacePeerSelection(text, "XYZ"));
        REQUIRE(WaitFor("TextBox undo history", [&]()
        {
            return text.GetValue() == "aXYZef" &&
                   text.CanUndo();
        }, 1000));

        text.Undo();
        REQUIRE(WaitFor("TextBox redo history", [&]()
        {
            return text.GetValue() == "abcdef" &&
                   text.CanRedo();
        }, 1000));
        text.DiscardEdits();

        checkClearPreservesState(text, textEvents);
    }
}

TEST_CASE("wxWinUI text controls synchronize layout contracts",
          "[winui-textpeer][layout]")
{
    // A real XamlRoot and template ScrollViewer are required, but the frame
    // must neither activate nor interfere with a user sharing the desktop.
    wxFrame frame(nullptr, wxID_ANY, "text geometry owner",
                  wxPoint(32, 32), wxSize(520, 320),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxTextCtrl text(&frame, wxID_ANY, wxString(),
                    wxPoint(10, 10), wxSize(300, 110),
                    wxTE_MULTILINE);
    REQUIRE(MakeTestWindowTransparent(
        static_cast<HWND>(frame.GetHWND())));
    frame.ShowWithoutActivating();

    double horizontal = 0.0;
    double vertical = 0.0;
    double viewportWidth = 0.0;
    double viewportHeight = 0.0;
    double scale = 0.0;
    REQUIRE(WaitFor("initial WinUI TextBox template", [&]()
    {
        return wxWinUITextCtrlTestAccess::GetScrollState(text,
                   &horizontal,
                   &vertical,
                   &viewportWidth,
                   &viewportHeight,
                   &scale) &&
               viewportWidth > 0.0 &&
               viewportHeight > 0.0 &&
               scale > 0.0;
    }, 1000));
    CHECK(viewportWidth > 0.0);
    CHECK(viewportHeight > 0.0);
    CHECK(scale > 0.0);

    wxString value;
    for ( int n = 0; n != 60; ++n )
        value += wxString::Format("line %02d\n", n);
    value += "last line";
    text.ChangeValue(value);
    text.SetSelection(2, 5);
    long selectionFrom = -1;
    long selectionTo = -1;
    text.GetSelection(&selectionFrom, &selectionTo);

    text.ShowPosition(text.GetLastPosition());

    double scrolledVertical = 0.0;
    wxPoint first = wxDefaultPosition;
    wxPoint last = wxDefaultPosition;
    const bool projected =
        WaitFor("vertical TextBox viewport projection", [&]()
    {
        if ( !wxWinUITextCtrlTestAccess::GetScrollState(text,
                  nullptr,
                  &scrolledVertical,
                  nullptr,
                  nullptr,
                  nullptr) )
        {
            return false;
        }

        first = text.PositionToCoords(0);
        last = text.PositionToCoords(text.GetLastPosition());
        return scrolledVertical > vertical &&
               first != wxDefaultPosition &&
               last != wxDefaultPosition &&
               first.y < 0 &&
               last.y >= 0 &&
               last.y <= text.GetClientSize().y;
    }, 1000);
    CAPTURE(vertical,
            scrolledVertical,
            first.x,
            first.y,
            last.x,
            last.y,
            text.GetClientSize().x,
            text.GetClientSize().y);
    REQUIRE(projected);
    CHECK(scrolledVertical > vertical);

    long selectionFromAfter = -1;
    long selectionToAfter = -1;
    text.GetSelection(&selectionFromAfter, &selectionToAfter);
    CHECK(selectionFromAfter == selectionFrom);
    CHECK(selectionToAfter == selectionTo);
    long peerSelectionFrom = -1;
    long peerSelectionTo = -1;
    REQUIRE(wxWinUITextCtrlTestAccess::GetPeerSelection(text,
        &peerSelectionFrom, &peerSelectionTo));
    CHECK(peerSelectionFrom == selectionFrom);
    CHECK(peerSelectionTo == selectionTo);

    CHECK(first.y < 0);
    CHECK(last.y >= 0);
    CHECK(last.y <= text.GetClientSize().y);

    // Public CRLF positions on a plain multiline MSW-compatible control map
    // to the same XAML LF edge.
    text.ChangeValue("a\nb");
    text.ShowPosition(0);
    wxPoint crlfFirstHalf = wxDefaultPosition;
    wxPoint crlfSecondHalf = wxDefaultPosition;
    REQUIRE(WaitFor("CRLF TextBox geometry", [&]()
    {
        crlfFirstHalf = text.PositionToCoords(1);
        crlfSecondHalf = text.PositionToCoords(2);
        return crlfFirstHalf != wxDefaultPosition &&
               crlfFirstHalf == crlfSecondHalf;
    }, 1000));
    CHECK(crlfFirstHalf == crlfSecondHalf);

    // The trailing edge of a final newline is the caret on the new blank
    // physical line, not the trailing X coordinate of the previous line.
    text.ChangeValue("Hello\n");
    text.ShowPosition(0);
    wxPoint firstLineStart = wxDefaultPosition;
    wxPoint blankLineStart = wxDefaultPosition;
    REQUIRE(WaitFor("final-newline TextBox geometry", [&]()
    {
        firstLineStart = text.PositionToCoords(0);
        blankLineStart =
            text.PositionToCoords(text.GetLastPosition());
        return firstLineStart != wxDefaultPosition &&
               blankLineStart != wxDefaultPosition &&
               blankLineStart.y > firstLineStart.y;
    }, 1000));
    CHECK(blankLineStart.x == firstLineStart.x);
    CHECK(blankLineStart.y > firstLineStart.y);

    // Wrapped physical lines come from XAML layout, not logical '\n' rows.
    text.SetSize(wxSize(145, 95));
    text.ChangeValue(wxString(120, 'M'));
    text.ShowPosition(0);

    long wrapBoundary = wxNOT_FOUND;
    REQUIRE(WaitFor("wrapped TextBox geometry", [&]()
    {
        wrapBoundary = wxNOT_FOUND;
        const wxPoint origin = text.PositionToCoords(0);
        if ( origin == wxDefaultPosition || origin.y < 0 )
            return false;

        for ( long n = 0; n < text.GetLastPosition(); ++n )
        {
            const wxPoint current = text.PositionToCoords(n);
            const wxPoint next = text.PositionToCoords(n + 1);
            if ( current == wxDefaultPosition ||
                 next == wxDefaultPosition )
            {
                // RichEdit virtualizes wrapped ranges. Geometry outside the
                // realized viewport must be reported as unavailable, never as
                // a fictitious unwrapped model coordinate.
                break;
            }

            if ( next.y > current.y + 2 )
            {
                if ( wrapBoundary == wxNOT_FOUND )
                    wrapBoundary = n;
            }
        }

        return wrapBoundary != wxNOT_FOUND;
    }, 1000));

    // This is the final character of a physical line: its next logical
    // leading edge is on another line. HitTest() must use this character's
    // own native range and leading points instead of joining it to the next
    // leading edge. At a fractional raster scale the previous character's
    // tolerant cell can overlap this first physical pixel, so logical scan
    // order is not an acceptable tie-breaker.
    const wxPoint boundaryLeading =
        text.PositionToCoords(wrapBoundary);
    const int halfLineHeight =
        wxMax(1, text.GetTextExtent("M").y / 2);
    const wxPoint boundaryInside(
        boundaryLeading.x + 1,
        boundaryLeading.y + halfLineHeight);
    long hitPosition = wxNOT_FOUND;
    CAPTURE(wrapBoundary,
            boundaryLeading.x,
            boundaryLeading.y,
            boundaryInside.x,
            boundaryInside.y);
    CHECK(text.HitTest(boundaryInside, &hitPosition) ==
          wxTE_HT_ON_TEXT);
    CHECK(hitPosition == wrapBoundary);

    // BEFORE and BEYOND are visual line classifications, independent of the
    // non-zero document index at which a wrapped line begins.
    text.ShowPosition(text.GetLastPosition());
    long lastWrappedLineStart = wxNOT_FOUND;
    wxPoint lastLineLeading = wxDefaultPosition;
    wxPoint wrappedEnd = wxDefaultPosition;
    double wrappedVerticalOffset = 0.0;
    double wrappedViewportHeight = 0.0;
    double wrappedScrollableWidth = 0.0;
    double wrappedScrollableHeight = 0.0;
    bool wrappedRequestPending = false;
    unsigned wrappedRequestRetries = 0;
    int wrappedScrollStage = 0;
    const bool wrappedEndVisible =
        WaitFor("last wrapped TextBox line", [&]()
    {
        wxWinUITextCtrlTestAccess::GetScrollState(text,
            nullptr,
            &wrappedVerticalOffset,
            nullptr,
            &wrappedViewportHeight,
            nullptr,
            &wrappedScrollStage,
            &wrappedScrollableWidth,
            &wrappedScrollableHeight);
        wxWinUITextCtrlTestAccess::GetPositionVisibilityState(text,
            &wrappedRequestPending, &wrappedRequestRetries);
        wrappedEnd =
            text.PositionToCoords(text.GetLastPosition());
        if ( wrappedEnd == wxDefaultPosition )
            return false;

        lastWrappedLineStart = 0;
        for ( long n = text.GetLastPosition() - 1; n >= 0; --n )
        {
            const wxPoint leading = text.PositionToCoords(n);
            if ( leading == wxDefaultPosition )
                return false;
            if ( leading.y + 2 < wrappedEnd.y )
            {
                lastWrappedLineStart = n + 1;
                break;
            }
        }
        lastLineLeading =
            text.PositionToCoords(lastWrappedLineStart);
        return lastLineLeading != wxDefaultPosition &&
               lastLineLeading.y >= 0 &&
               wrappedEnd.y >= 0 &&
               wrappedEnd.x >= 0 &&
               wrappedEnd.x <= text.GetClientSize().x &&
               wrappedEnd.y <= text.GetClientSize().y;
    }, 1000);
    CAPTURE(wrappedVerticalOffset,
            wrappedViewportHeight,
            wrappedScrollableWidth,
            wrappedScrollableHeight,
            wrappedRequestPending,
            wrappedRequestRetries,
            wrappedScrollStage,
            lastLineLeading.x,
            lastLineLeading.y,
            wrappedEnd.x,
            wrappedEnd.y,
            text.GetClientSize().x,
            text.GetClientSize().y);
    REQUIRE(wrappedEndVisible);

    hitPosition = wxNOT_FOUND;
    CHECK(text.HitTest(
              wxPoint(lastLineLeading.x - 3,
                      lastLineLeading.y + halfLineHeight),
              &hitPosition) == wxTE_HT_BEFORE);
    CHECK(hitPosition == lastWrappedLineStart);

    hitPosition = wxNOT_FOUND;
    CHECK(text.HitTest(
              wxPoint(wrappedEnd.x + 3,
                      wrappedEnd.y + halfLineHeight),
              &hitPosition) == wxTE_HT_BEYOND);
    CHECK(hitPosition == text.GetLastPosition());

    // Exercise an arbitrary virtualized range too: endpoint-only extent
    // estimates are insufficient for wxTextEntry's general contract.
    text.ShowPosition(0);
    wxPoint wrappedFirst = wxDefaultPosition;
    REQUIRE(WaitFor("first wrapped TextBox line", [&]()
    {
        wrappedFirst = text.PositionToCoords(0);
        return wrappedFirst != wxDefaultPosition &&
               wrappedFirst.y >= 0 &&
               wrappedFirst.y <= text.GetClientSize().y;
    }, 1000));

    const long arbitraryPosition = text.GetLastPosition() / 2;
    text.SetSelection(2, 5);
    long selectionBeforeArbitraryFrom = -1;
    long selectionBeforeArbitraryTo = -1;
    text.GetSelection(
        &selectionBeforeArbitraryFrom,
        &selectionBeforeArbitraryTo);

    text.ShowPosition(arbitraryPosition);
    wxPoint arbitraryEdge = wxDefaultPosition;
    REQUIRE(WaitFor("arbitrary wrapped TOM range", [&]()
    {
        arbitraryEdge = text.PositionToCoords(arbitraryPosition);
        return arbitraryEdge != wxDefaultPosition &&
               arbitraryEdge.y >= 0 &&
               arbitraryEdge.y <= text.GetClientSize().y;
    }, 1000));

    text.GetSelection(&selectionFromAfter, &selectionToAfter);
    CHECK(selectionFromAfter == selectionBeforeArbitraryFrom);
    CHECK(selectionToAfter == selectionBeforeArbitraryTo);
    REQUIRE(wxWinUITextCtrlTestAccess::GetPeerSelection(text,
        &peerSelectionFrom, &peerSelectionTo));
    CHECK(peerSelectionFrom == selectionBeforeArbitraryFrom);
    CHECK(peerSelectionTo == selectionBeforeArbitraryTo);

    // An empty value still has one caret edge. Its zero document index must
    // not make a point to the right classify as BEFORE.
    text.ChangeValue("");
    text.ShowPosition(0);
    wxPoint emptyCaret = wxDefaultPosition;
    REQUIRE(WaitFor("empty TextBox geometry", [&]()
    {
        emptyCaret = text.PositionToCoords(0);
        return emptyCaret != wxDefaultPosition &&
               emptyCaret.y >= 0;
    }, 1000));

    hitPosition = wxNOT_FOUND;
    CHECK(text.HitTest(
              wxPoint(emptyCaret.x - 3,
                      emptyCaret.y + halfLineHeight),
              &hitPosition) == wxTE_HT_BEFORE);
    CHECK(hitPosition == 0);
    hitPosition = wxNOT_FOUND;
    CHECK(text.HitTest(
              wxPoint(emptyCaret.x + 3,
                      emptyCaret.y + halfLineHeight),
              &hitPosition) == wxTE_HT_BEYOND);
    CHECK(hitPosition == 0);

    text.ChangeValue("M");
    text.ShowPosition(0);
    wxPoint firstNonEmptyCaret = wxDefaultPosition;
    REQUIRE(WaitFor("non-empty TextBox leading edge", [&]()
    {
        firstNonEmptyCaret = text.PositionToCoords(0);
        return firstNonEmptyCaret != wxDefaultPosition &&
               firstNonEmptyCaret.y >= 0;
    }, 1000));
    CHECK(std::abs(firstNonEmptyCaret.x - emptyCaret.x) <= 1);
    CHECK(std::abs(firstNonEmptyCaret.y - emptyCaret.y) <= 1);

    // Sequential TOM visibility requests are synchronous and must never
    // mutate the public/native selection or leave a TextBox-era retry queued.
    text.ChangeValue(wxString(120, 'M'));
    text.SetSelection(7, 9);
    text.ShowPosition(text.GetLastPosition());
    text.ShowPosition(0);
    bool visibilityPending = true;
    unsigned visibilityRetries = 99;
    REQUIRE(WaitFor("sequential TOM visibility requests", [&]()
    {
        const wxPoint start = text.PositionToCoords(0);
        return wxWinUITextCtrlTestAccess::GetPositionVisibilityState(text,
                   &visibilityPending, &visibilityRetries) &&
               !visibilityPending &&
               visibilityRetries == 0 &&
               start != wxDefaultPosition &&
               start.y >= 0 &&
               start.y <= text.GetClientSize().y;
    }, 1000));
    CHECK_FALSE(visibilityPending);
    CHECK(visibilityRetries == 0);
    text.GetSelection(&selectionFromAfter, &selectionToAfter);
    CHECK(selectionFromAfter == 7);
    CHECK(selectionToAfter == 9);

    text.ShowPosition(text.GetLastPosition());
    text.ChangeValue("short");
    REQUIRE(wxWinUITextCtrlTestAccess::GetPositionVisibilityState(text,
        &visibilityPending, &visibilityRetries));
    CHECK_FALSE(visibilityPending);

    // A single-line peer must expose its real horizontal viewport too.
    // It is deliberately added after the frame is already shown to exercise
    // synchronous slot publication and template realization.
    wxTextCtrl single(&frame, wxID_ANY, wxString(),
                      wxPoint(10, 150), wxSize(170, 36));
    single.ChangeValue(wxString(120, 'W'));
    single.SetSelection(1, 3);
    single.ShowPosition(single.GetLastPosition());

    double singleHorizontal = 0.0;
    REQUIRE(WaitFor("late single-line TextBox viewport", [&]()
    {
        return wxWinUITextCtrlTestAccess::GetScrollState(single,
                   &singleHorizontal,
                   nullptr,
                   nullptr,
                   nullptr,
                   nullptr) &&
               singleHorizontal > 0.0;
    }, 1000));
    CHECK(singleHorizontal > 0.0);
    long singleFrom = -1;
    long singleTo = -1;
    single.GetSelection(&singleFrom, &singleTo);
    CHECK(singleFrom == 1);
    CHECK(singleTo == 3);
    wxPoint singleEnd = wxDefaultPosition;
    REQUIRE(WaitFor("single-line TextBox projection", [&]()
    {
        singleEnd =
            single.PositionToCoords(single.GetLastPosition());
        return singleEnd != wxDefaultPosition &&
               singleEnd.x >= 0 &&
               singleEnd.x <= single.GetClientSize().x;
    }, 1000));
    CHECK(singleEnd.x >= 0);
    CHECK(singleEnd.x <= single.GetClientSize().x);

    frame.Hide();

#if wxUSE_SEARCHCTRL
    // SearchCtrl focus remains synchronous; this uses the ordinary hidden
    // test parent and does not activate the off-screen geometry frame.
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxSearchCtrl search(parent, wxID_ANY);
    search.SetFocus();
    CHECK(wxWindow::FindFocus() == &search);
#endif // wxUSE_SEARCHCTRL
}

#if wxUSE_SEARCHCTRL
TEST_CASE("wxWinUI SearchCtrl template actions",
          "[winui-textpeer][searchctrl]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxSearchCtrl search(parent, wxID_ANY);

    unsigned searchEvents = 0;
    unsigned cancelEvents = 0;
    wxString submitted;
    search.Bind(wxEVT_SEARCH, [&](wxCommandEvent& event)
    {
        ++searchEvents;
        submitted = event.GetString();
    });
    search.Bind(wxEVT_SEARCH_CANCEL, [&](wxCommandEvent&)
    {
        ++cancelEvents;
    });

    // Install two detached template generations. This does not depend on a
    // visible/activated XamlRoot: both generations go through the exact
    // transactional token installation used for production template parts.
    REQUIRE(wxWinUISearchCtrlTestAccess::Retemplate(search));
    search.ChangeValue("needle");
    REQUIRE(wxWinUISearchCtrlTestAccess::InvokeSearchButton(search));
    CHECK(searchEvents == 1);
    CHECK(submitted == "needle");

    REQUIRE(wxWinUISearchCtrlTestAccess::Retemplate(search));
    INFO("template state = 0x" << std::hex
         << wxWinUISearchCtrlTestAccess::GetTemplateState(search));
    CHECK((wxWinUISearchCtrlTestAccess::GetTemplateState(search) & 0xe) == 0xe);

    // The retired part remains alive, but its token has been revoked and its
    // generation invalidated. Only the replacement may route the event.
    REQUIRE(wxWinUISearchCtrlTestAccess::InvokeRetiredSearchButton(search));
    CHECK(searchEvents == 1);
    REQUIRE(wxWinUISearchCtrlTestAccess::InvokeSearchButton(search));
    CHECK(searchEvents == 2);

    search.ShowCancelButton(true);
    REQUIRE(wxWinUISearchCtrlTestAccess::InvokeCancelButton(search));
    CHECK(cancelEvents == 1);

    wxArrayString choices;
    choices.Add("alpha");
    choices.Add("beta");
    choices.Add("alpine");
    search.ChangeValue("al");
    REQUIRE(search.AutoComplete(choices));
    CHECK(wxWinUISearchCtrlTestAccess::GetSuggestionCount(search) == 2);

    // CanUndo/CanRedo must resolve and validate the current edit generation,
    // not query a stale TextBox retained from the previous template.
    CHECK_FALSE(search.CanUndo());
    CHECK_FALSE(search.CanRedo());
}

TEST_CASE("wxWinUI SearchCtrl survives destruction from query Click",
          "[winui-textpeer][searchctrl][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const std::size_t statesBefore =
        wxWinUITextCallbackState::GetLiveCountForTesting();

    wxSearchCtrl *search =
        new wxSearchCtrl(parent, wxID_ANY, "destroy");
    REQUIRE(wxWinUISearchCtrlTestAccess::Retemplate(*search));

    unsigned searchEvents = 0;
    search->Bind(wxEVT_SEARCH, [&](wxCommandEvent&)
    {
        ++searchEvents;
        wxSearchCtrl * const doomed = search;
        search = nullptr;
        delete doomed;
    });

    wxSearchCtrl * const invoking = search;
    REQUIRE(wxWinUISearchCtrlTestAccess::InvokeSearchButton(*invoking));
    CHECK(search == nullptr);
    CHECK(searchEvents == 1);
    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() ==
          statesBefore);
}

#if wxUSE_MENUS
TEST_CASE("wxWinUI SearchCtrl owns its menu",
          "[winui-textpeer][searchctrl]")
{
    class CountingMenu final : public wxMenu
    {
    public:
        explicit CountingMenu(int& destructions)
            : m_destructions(destructions)
        {
        }

        ~CountingMenu() override
        {
            ++m_destructions;
        }

    private:
        int& m_destructions;
    };

    int destructions = 0;
    wxSearchCtrl * const search =
        new wxSearchCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
    search->SetMenu(new CountingMenu(destructions));
    CHECK(destructions == 0);
    search->SetMenu(new CountingMenu(destructions));
    CHECK(destructions == 1);
    delete search;
    CHECK(destructions == 2);
}
#endif // wxUSE_MENUS
#endif // wxUSE_SEARCHCTRL

TEST_CASE("wxWinUI TextCtrl owns wxTE_PROCESS_TAB",
          "[winui-textpeer][textctrl][keyboard][tab]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl text(parent,
                    wxID_ANY,
                    "a",
                    wxDefaultPosition,
                    wxSize(180, 70),
                    wxTE_MULTILINE | wxTE_PROCESS_TAB);
    text.SetInsertionPointEnd();
    REQUIRE(wxWinUITextCtrlTestAccess::ProcessTab(text));
    CHECK(text.GetValue() == "a\t");

    // The mixed HWND/XAML Tab arbiter runs before the XAML KeyDown event. It
    // must leave an editable PROCESS_TAB owner in place so the peer above can
    // consume the key rather than moving to another slot.
    text.SetFocus();
    wxYield();
    MSG tab{};
    tab.hwnd = ::GetFocus();
    tab.message = WM_KEYDOWN;
    tab.wParam = VK_TAB;
    REQUIRE(tab.hwnd != nullptr);
    CHECK_FALSE(wxWinUI3ProcessTabNavigationWithModifiers(
        &tab, false, false, false));
    CHECK(wxWindow::FindFocus() == &text);

    text.SetMaxLength(2);
    EventCounter maxEvents(&text, wxEVT_TEXT_MAXLEN);
    text.SetInsertionPointEnd();
    REQUIRE(wxWinUITextCtrlTestAccess::ProcessTab(text));
    CHECK(text.GetValue() == "a\t");
    CHECK(maxEvents.GetCount() == 1);

    wxTextCtrl readOnly(parent,
                        wxID_ANY,
                        "locked",
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxTE_PROCESS_TAB | wxTE_READONLY);
    CHECK_FALSE(wxWinUITextCtrlTestAccess::ProcessTab(readOnly));
    CHECK(readOnly.GetValue() == "locked");

    wxTextCtrl rich(parent,
                    wxID_ANY,
                    "rich",
                    wxDefaultPosition,
                    wxSize(180, 70),
                    wxTE_MULTILINE | wxTE_RICH2 | wxTE_PROCESS_TAB);
    rich.SetInsertionPointEnd();
    REQUIRE(wxWinUITextCtrlTestAccess::ProcessTab(rich));
    CHECK(rich.GetValue() == "rich\t");
}

#if wxUSE_CLIPBOARD
TEST_CASE("wxWinUI TextCtrl clipboard commands honour wx veto events",
          "[winui-textpeer][textctrl][clipboard][event]")
{
    long style = wxTE_MULTILINE;
    SECTION("plain")
    {
    }
    SECTION("rich AllFormats")
    {
        style |= wxTE_RICH2;
    }

    wxTextCtrl text(wxTheApp->GetTopWindow(),
                    wxID_ANY,
                    "abcdef",
                    wxDefaultPosition,
                    wxDefaultSize,
                    style);
    if ( text.IsRich() )
        REQUIRE(wxWinUITextCtrlTestAccess::RichClipboardUsesAllFormats(text));

    int copyEvents = 0;
    int cutEvents = 0;
    int pasteEvents = 0;
    text.Bind(wxEVT_TEXT_COPY,
              [&](wxClipboardTextEvent&) { ++copyEvents; });
    text.Bind(wxEVT_TEXT_CUT,
              [&](wxClipboardTextEvent&) { ++cutEvents; });
    text.Bind(wxEVT_TEXT_PASTE,
              [&](wxClipboardTextEvent&) { ++pasteEvents; });

    // All handlers intentionally keep the event handled. No command below is
    // therefore allowed to touch the process clipboard or mutate the control.
    text.SetSelection(1, 4);
    text.Copy();
    CHECK(copyEvents == 1);
    CHECK(text.GetValue() == "abcdef");

    text.Cut();
    CHECK(cutEvents == 1);
    CHECK(text.GetValue() == "abcdef");

    text.Paste();
    CHECK(pasteEvents == 1);
    CHECK(text.GetValue() == "abcdef");

}

#if wxUSE_UIACTIONSIMULATOR
TEST_CASE("wxWinUI TextCtrl physical clipboard shortcuts (manual)",
          "[.][winui-text-clipboard-manual][clipboard][physical]")
{
    // Hidden/manual by design: wxUIActionSimulator uses SendInput and must
    // only run on an isolated desktop. Restore the complete OLE data object
    // even if a regression lets one of the vetoed commands reach Windows.
    ClipboardContentsRestorer clipboardRestorer;
    REQUIRE(clipboardRestorer.IsCaptured());

    long style = wxTE_MULTILINE;
    SECTION("plain")
    {
    }
    SECTION("rich AllFormats")
    {
        style |= wxTE_RICH2;
    }

    wxTextCtrl text(wxTheApp->GetTopWindow(), wxID_ANY, "abcdef",
                    wxDefaultPosition, wxDefaultSize, style);
    int copyEvents = 0;
    int cutEvents = 0;
    int pasteEvents = 0;
    text.Bind(wxEVT_TEXT_COPY,
              [&](wxClipboardTextEvent&) { ++copyEvents; });
    text.Bind(wxEVT_TEXT_CUT,
              [&](wxClipboardTextEvent&) { ++cutEvents; });
    text.Bind(wxEVT_TEXT_PASTE,
              [&](wxClipboardTextEvent&) { ++pasteEvents; });
    text.SetSelection(1, 4);
    text.SetFocus();
    REQUIRE(WaitFor("text clipboard shortcut focus", [&text]()
    {
        return wxWindow::FindFocus() == &text;
    }, 1000));

    wxUIActionSimulator simulator;
    REQUIRE(simulator.Char('c', wxMOD_CONTROL));
    REQUIRE(WaitFor("native text copy event", [&copyEvents]()
    {
        return copyEvents == 1;
    }, 1000));
    CHECK(text.GetValue() == "abcdef");

    REQUIRE(simulator.Char('x', wxMOD_CONTROL));
    REQUIRE(WaitFor("native text cut event", [&cutEvents]()
    {
        return cutEvents == 1;
    }, 1000));
    CHECK(text.GetValue() == "abcdef");

    REQUIRE(simulator.Char('v', wxMOD_CONTROL));
    REQUIRE(WaitFor("native text paste event", [&pasteEvents]()
    {
        return pasteEvents == 1;
    }, 1000));
    CHECK(text.GetValue() == "abcdef");
}
#endif // wxUSE_UIACTIONSIMULATOR
#endif // wxUSE_CLIPBOARD

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
TEST_CASE("wxWinUI RichEditBox AllFormats clipboard round trip (manual)",
          "[.][winui-text-clipboard-manual][clipboard]")
{
    // Hidden/manual by design: this verifies the real Windows clipboard and
    // must only run on an isolated desktop.
    ClipboardContentsRestorer clipboardRestorer;
    REQUIRE(clipboardRestorer.IsCaptured());

    wxWindow * const parent = wxTheApp->GetTopWindow();
    wxTextCtrl source(parent,
                      wxID_ANY,
                      "bold plain",
                      wxDefaultPosition,
                      wxDefaultSize,
                      wxTE_MULTILINE | wxTE_RICH2);
    wxTextAttr bold;
    bold.SetFontWeight(wxFONTWEIGHT_BOLD);
    REQUIRE(source.SetStyle(0, 4, bold));
    REQUIRE(wxWinUITextCtrlTestAccess::RichClipboardUsesAllFormats(source));
    source.SelectAll();
    source.Copy();

    wxTextCtrl destination(parent,
                           wxID_ANY,
                           wxEmptyString,
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxTE_MULTILINE | wxTE_RICH2);
    destination.Paste();
    REQUIRE(WaitFor("RichEditBox formatted clipboard paste", [&destination]()
    {
        return destination.GetValue() == "bold plain";
    }, 1000));

    wxTextAttr actual;
    REQUIRE(destination.GetStyle(1, actual));
    CHECK(actual.GetFontWeight() == wxFONTWEIGHT_BOLD);
}

#if wxUSE_VALIDATORS
TEST_CASE("wxWinUI TextCtrl validator filters native paste (manual)",
          "[.][winui-text-clipboard-manual][clipboard][validator]")
{
    ClipboardContentsRestorer clipboardRestorer;
    REQUIRE(clipboardRestorer.IsCaptured());

    long style = wxTE_MULTILINE;
    SECTION("plain")
    {
    }
    SECTION("rich")
    {
        style |= wxTE_RICH2;
    }

    {
        wxClipboardLocker clipboard;
        REQUIRE_FALSE(!clipboard);
        REQUIRE(wxTheClipboard->SetData(
            new wxTextDataObject("alpha-123-beta")));
    }

    wxTextValidator validator(wxFILTER_ALPHA);
    wxTextCtrl text(wxTheApp->GetTopWindow(),
                    wxID_ANY,
                    wxEmptyString,
                    wxDefaultPosition,
                    wxDefaultSize,
                    style,
                    validator);
    text.Paste();
    REQUIRE(WaitFor("validated text paste", [&text]()
    {
        return text.GetValue() == "alphabeta";
    }, 1000));
}
#endif // wxUSE_VALIDATORS
#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ

TEST_CASE("wxWinUI TextCtrl exposes RichEditBox parity",
          "[winui-textpeer][textctrl][rich]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTextCtrl text(parent,
                    wxID_ANY,
                    wxEmptyString,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxTE_MULTILINE | wxTE_RICH2);
    REQUIRE(text.IsRich());
    REQUIRE(text.IsRTFSupported());

    wxTextAttr redOnWhite(*wxRED, *wxWHITE);
    redOnWhite.SetFont(wxFontInfo(14).FaceName("Segoe UI"));
    REQUIRE(text.SetDefaultStyle(redOnWhite));
    text.AppendText("alpha beta");

    wxTextAttr actual;
    REQUIRE(text.GetStyle(2, actual));
    CHECK(actual.GetTextColour() == *wxRED);
    CHECK(actual.GetBackgroundColour() == *wxWHITE);
    CHECK(actual.GetFontSize() == 14);

    wxTextAttr blue;
    blue.SetTextColour(*wxBLUE);
    blue.SetFontWeight(wxFONTWEIGHT_BOLD);
    REQUIRE(text.SetStyle(6, 10, blue));
    REQUIRE(text.GetStyle(7, actual));
    CHECK(actual.GetTextColour() == *wxBLUE);
    CHECK(actual.GetFontWeight() == wxFONTWEIGHT_BOLD);

    const wxTextSearchResult found = text.SearchText(
        wxTextSearch("BETA").MatchCase(false).MatchWholeWord());
    REQUIRE(found);
    CHECK(found.m_start == 6);
    CHECK(found.m_end == 10);

    const wxString rtf = text.GetRTFValue();
    CHECK(rtf.Contains("alpha"));
    CHECK(rtf.Contains("beta"));

#if wxUSE_SPELLCHECK
    REQUIRE(text.EnableProofCheck(wxTextProofOptions::Default()));
    CHECK(text.GetProofCheckOptions().IsSpellCheckEnabled());
    REQUIRE(text.EnableProofCheck(wxTextProofOptions::Disable()));
    CHECK_FALSE(text.GetProofCheckOptions().AnyChecksEnabled());
#endif // wxUSE_SPELLCHECK
}

TEST_CASE("wxWinUI TextCtrl imports RTF through RichEditBox",
          "[winui-textpeer][textctrl][rich][rtf]")
{
    wxTextCtrl text(wxTheApp->GetTopWindow(),
                    wxID_ANY,
                    wxEmptyString,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxTE_MULTILINE | wxTE_RICH2);

    text.SetRTFValue(
        R"({\rtf1\ansi plain \b bold\b0  tail\par second})");
    CHECK(text.GetValue().Contains("plain"));
    CHECK(text.GetValue().Contains("bold"));
    CHECK(text.GetValue().Contains("second"));

    wxTextAttr bold;
    REQUIRE(text.GetStyle(
        static_cast<long>(text.GetValue().find("bold")), bold));
    CHECK(bold.GetFontWeight() == wxFONTWEIGHT_BOLD);

    const wxString exported = text.GetRTFValue();
    CHECK(exported.Contains("bold"));
    CHECK(exported.Contains("second"));
}

TEST_CASE("wxWinUI RichEditBox exposes TOM geometry",
          "[winui-textpeer][textctrl][rich][geometry]")
{
    wxFrame frame(nullptr,
                  wxID_ANY,
                  "rich text geometry owner",
                  wxPoint(40, 40),
                  wxSize(360, 230),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxTextCtrl rich(&frame,
                    wxID_ANY,
                    wxEmptyString,
                    wxPoint(10, 10),
                    wxSize(190, 105),
                    wxTE_MULTILINE | wxTE_RICH2);
    REQUIRE(MakeTestWindowTransparent(
        static_cast<HWND>(frame.GetHWND())));
    frame.ShowWithoutActivating();

    const wxString value =
        "first\n" + wxString(120, 'M') + "\nlast";
    rich.ChangeValue(value);
    rich.ShowPosition(0);

    wxPoint first = wxDefaultPosition;
    wxPoint last = wxDefaultPosition;
    REQUIRE(WaitFor("RichEditBox TOM endpoints", [&]()
    {
        first = rich.PositionToCoords(0);
        last = rich.PositionToCoords(rich.GetLastPosition());
        return first != wxDefaultPosition &&
               last != wxDefaultPosition &&
               first.y >= 0 &&
               rich.GetNumberOfLines() > 3;
    }, 1000));
    const int physicalLineCount = rich.GetNumberOfLines();
    CHECK(physicalLineCount > 3);
    CHECK(rich.GetLineText(0) == "first");
    CHECK(rich.GetLineText(physicalLineCount - 1) == "last");

    std::size_t physicalTextUnits = 0;
    for ( int line = 0; line < physicalLineCount; ++line )
    {
        const wxString lineText = rich.GetLineText(line);
        CHECK(rich.GetLineLength(line) ==
              static_cast<int>(lineText.length()));
        physicalTextUnits += lineText.length();
    }
    // Physical line splitting must neither duplicate nor omit wrapped text;
    // only the two explicit paragraph delimiters are excluded by the API.
    CHECK(physicalTextUnits == value.length() - 2);

    long wrappedPosition = wxNOT_FOUND;
    wxPoint wrappedLeading = wxDefaultPosition;
    REQUIRE(WaitFor("RichEditBox wrapped TOM line", [&]()
    {
        wxPoint previous = rich.PositionToCoords(0);
        if ( previous == wxDefaultPosition )
            return false;
        for ( long n = 1; n < rich.GetLastPosition(); ++n )
        {
            const wxPoint current = rich.PositionToCoords(n);
            if ( current == wxDefaultPosition )
                return false;
            if ( current.y > previous.y + 2 &&
                 value[static_cast<std::size_t>(n - 1)] != '\n' )
            {
                wrappedPosition = n;
                wrappedLeading = current;
                return true;
            }
            previous = current;
        }
        return false;
    }, 1000));

    long wrappedColumn = wxNOT_FOUND;
    long wrappedLine = wxNOT_FOUND;
    REQUIRE(rich.PositionToXY(
        wrappedPosition, &wrappedColumn, &wrappedLine));
    CHECK(wrappedColumn == 0);
    CHECK(wrappedLine > 0);
    CHECK(rich.XYToPosition(wrappedColumn, wrappedLine) ==
          wrappedPosition);

    const int halfLine = wxMax(1, rich.GetTextExtent("M").y / 2);
    long hit = wxNOT_FOUND;
    CHECK(rich.HitTest(
              wxPoint(wrappedLeading.x + 1,
                      wrappedLeading.y + halfLine),
              &hit) == wxTE_HT_ON_TEXT);
    CHECK(hit == wrappedPosition);

    hit = wxNOT_FOUND;
    CHECK(rich.HitTest(
              wxPoint(wrappedLeading.x - 4,
                      wrappedLeading.y + halfLine),
              &hit) == wxTE_HT_BEFORE);
    CHECK(hit == wrappedPosition);

    rich.ShowPosition(rich.GetLastPosition());
    const bool scrolledCoordinatesReady =
        WaitFor("RichEditBox TOM scrolled coordinates", [&]()
    {
        first = rich.PositionToCoords(0);
        last = rich.PositionToCoords(rich.GetLastPosition());
        const int clientHeight = rich.GetClientSize().y;
        const bool endVisible =
            last.y >= 0 && last.y <= clientHeight;
        const bool wholeDocumentVisible =
            first.y >= 0 && first.y <= clientHeight &&
            last.y >= first.y;
        return first != wxDefaultPosition &&
               last != wxDefaultPosition &&
               endVisible &&
               (first.y < 0 || wholeDocumentVisible);
    }, 1000);
    double horizontalOffset = 0.0;
    double verticalOffset = 0.0;
    double viewportWidth = 0.0;
    double viewportHeight = 0.0;
    double scrollableWidth = 0.0;
    double scrollableHeight = 0.0;
    double viewOriginX = 0.0;
    double viewOriginY = 0.0;
    double viewUnitXScale = 0.0;
    double viewUnitYScale = 0.0;
    int scrollStage = 0;
    const bool hasScrollState = wxWinUITextCtrlTestAccess::GetScrollState(rich,
        &horizontalOffset,
        &verticalOffset,
        &viewportWidth,
        &viewportHeight,
        nullptr,
        &scrollStage,
        &scrollableWidth,
        &scrollableHeight,
        &viewOriginX,
        &viewOriginY,
        &viewUnitXScale,
        &viewUnitYScale);
    CAPTURE(hasScrollState,
            horizontalOffset,
            verticalOffset,
            viewportWidth,
            viewportHeight,
            scrollableWidth,
            scrollableHeight,
            viewOriginX,
            viewOriginY,
            viewUnitXScale,
            viewUnitYScale,
            scrollStage,
            first.x,
            first.y,
            last.x,
            last.y,
            rich.GetClientSize().x,
            rich.GetClientSize().y);
    REQUIRE(scrolledCoordinatesReady);
    const bool wholeDocumentVisible =
        first.y >= 0 && first.y <= rich.GetClientSize().y &&
        last.y >= first.y;
    CHECK((first.y < 0 || wholeDocumentVisible));
    CHECK(last.y >= 0);
    CHECK(last.y <= rich.GetClientSize().y);

    frame.Hide();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TEXTCTRL
