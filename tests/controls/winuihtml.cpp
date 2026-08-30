///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuihtml.cpp
// Purpose:     WinUI wxHtmlWindow fallback and lifetime qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_HTML && wxUSE_BUTTON

#include "button-test-access.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/filesys.h"
#include "wx/fs_mem.h"
#include "wx/html/htmlwin.h"
#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"

#if wxUSE_GIF && wxUSE_TIMER
    #include "wx/timer.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/dcmemory.h"
    #include "wx/html/htmprint.h"
#endif

#if wxUSE_WXHTML_HELP
    #include "wx/html/helpctrl.h"
#endif

#include <array>
#include <memory>
#include <vector>

namespace
{

constexpr char PageAName[] = "winui-html-014-page-a.htm";
constexpr char PageBName[] = "winui-html-014-page-b.htm";
constexpr char AnimatedGifName[] = "winui-html-014-animated.gif";
constexpr char HelpProjectName[] = "winui-html-014-help.hhp";
constexpr char HelpTopicName[] = "winui-html-014-topic.htm";

wxString MemoryURL(const char *name)
{
    return "memory:" + wxString::FromUTF8(name);
}

wxString MakeHistoryPageA()
{
    wxString html =
        "<html><head><title>History A</title></head><body>"
        "<h1>history-a</h1>"
        "<table border=\"1\"><tr><td>cell-a</td><td>cell-b</td></tr>"
        "</table>";

    for ( unsigned row = 0; row < 48; ++row )
    {
        html += wxString::Format(
            "<p>history row %u: deterministic local content</p>", row);
    }

    html += "<a name=\"tail\"></a><p>history-tail</p><a href=\"";
    html += MemoryURL(PageBName);
    html += "\">next-page</a></body></html>";
    return html;
}

wxString MakeLargePage()
{
    wxString html =
        "<html><head><title>Large local document</title></head><body>";
    for ( unsigned row = 0; row < 400; ++row )
    {
        html += wxString::Format(
            "<p>large row %u: alpha beta gamma delta epsilon</p>", row);
    }
    html += "<a name=\"large-tail\"></a><p>large-tail</p></body></html>";
    return html;
}

class MemoryHtmlResources final
{
public:
    MemoryHtmlResources()
        : m_handler(std::make_unique<wxMemoryFSHandler>())
    {
        wxFileSystem::AddHandler(m_handler.get());

        AddText(PageAName, MakeHistoryPageA(), "text/html");
        AddText(
            PageBName,
            "<html><head><title>History B</title></head><body>"
            "<p>history-b-original</p></body></html>",
            "text/html");
        AddText(
            HelpProjectName,
            "[OPTIONS]\n"
            "Title=WinUI local help\n"
            "Default topic=winui-html-014-topic.htm\n",
            "text/plain");
        AddText(
            HelpTopicName,
            "<html><head><title>Local help topic</title></head><body>"
            "local-help-body</body></html>",
            "text/html");

#if wxUSE_GIF && wxUSE_TIMER
        // Two 2x2 frames, 20 ms each. Keeping the bytes in the TU makes the
        // animation gate independent of the source tree and current directory.
        static const unsigned char animatedGif[] =
        {
            0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x02, 0x00,
            0x02, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x21, 0xff, 0x0b, 0x4e, 0x45, 0x54, 0x53,
            0x43, 0x41, 0x50, 0x45, 0x32, 0x2e, 0x30, 0x03,
            0x01, 0x00, 0x00, 0x00, 0x21, 0xf9, 0x04, 0x08,
            0x02, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00,
            0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x08, 0x06,
            0x00, 0x01, 0x08, 0x04, 0x10, 0x10, 0x00, 0x21,
            0xf9, 0x04, 0x08, 0x02, 0x00, 0x00, 0x00, 0x2c,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00,
            0x81, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x06, 0x00,
            0x01, 0x08, 0x04, 0x10, 0x10, 0x00, 0x3b
        };
        AddBinary(AnimatedGifName, animatedGif, sizeof(animatedGif),
                  "image/gif");
#endif
    }

    ~MemoryHtmlResources()
    {
        for ( auto it = m_names.rbegin(); it != m_names.rend(); ++it )
            wxMemoryFSHandler::RemoveFile(*it);
        wxFileSystem::RemoveHandler(m_handler.get());
    }

    void ReplacePageB(const wxString& html)
    {
        wxMemoryFSHandler::RemoveFile(PageBName);
        wxMemoryFSHandler::AddFileWithMimeType(
            PageBName, html, "text/html");
    }

private:
    void AddText(const wxString& name,
                 const wxString& text,
                 const wxString& mimeType)
    {
        wxMemoryFSHandler::AddFileWithMimeType(name, text, mimeType);
        m_names.push_back(name);
    }

    void AddBinary(const wxString& name,
                   const void *data,
                   std::size_t size,
                   const wxString& mimeType)
    {
        wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mimeType);
        m_names.push_back(name);
    }

    std::unique_ptr<wxMemoryFSHandler> m_handler;
    std::vector<wxString> m_names;
};

void DrainDispatch(unsigned rounds = 1)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 120)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch();
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 50; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() )
        {
            return true;
        }
    }

    return false;
}

bool FlushCallbacksStayQuiescent()
{
    const unsigned callbacks =
        wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
    DrainDispatch(4);
    return wxWinUITopLevelHost::GetFlushCallbackAttemptCount() == callbacks;
}

bool ShowOffscreenWithoutActivation(wxFrame *frame, int ordinal)
{
    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    if ( !hwnd )
        return false;

    ::SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame->Move(wxPoint(-30000 + ordinal * 16, -30000));
    frame->ShowWithoutActivating();
    return true;
}

struct RuntimeSnapshot
{
    static RuntimeSnapshot Capture()
    {
        RuntimeSnapshot snapshot;
        snapshot.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        snapshot.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        snapshot.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        snapshot.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        snapshot.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        snapshot.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        snapshot.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        snapshot.buttonCallbacks =
            wxWinUIButtonTestAccess::LiveCallbackStateCount();
        snapshot.slotHandlerAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount();
        snapshot.slotHandlerRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        snapshot.rootHandlerAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount();
        snapshot.rootHandlerRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        return snapshot;
    }

    bool IsRestored() const
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hosts &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   hostLifetimes &&
               wxWinUITopLevelHost::GetLiveSlotCount() == slots &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   slotLifetimes &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclasses &&
               wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
                   buttonCallbacks &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() -
                       slotHandlerAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotHandlerRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() -
                       rootHandlerAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootHandlerRevokes;
    }

    void CheckRestored(bool expectHostActivity = true) const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);
        CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
              buttonCallbacks);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;

        if ( expectHostActivity )
        {
            CHECK(slotAdds > 0);
            CHECK(rootAdds > 0);
        }
        CHECK(slotAdds == slotRevokes);
        CHECK(rootAdds == rootRevokes);
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned buttonCallbacks = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
};

struct HtmlProbeState
{
    unsigned liveHtmlWindows = 0;
    unsigned activeCallbacks = 0;
    unsigned lateHtmlCallbacks = 0;
    unsigned openingCallbacks = 0;
    unsigned linkCallbacks = 0;
    unsigned refreshCallbacks = 0;
    unsigned blockedRequests = 0;
    std::array<unsigned, 3> openingByType{{0, 0, 0}};
#if wxUSE_GIF && wxUSE_TIMER
    unsigned liveTestTimers = 0;
    unsigned timerCallbacks = 0;
#endif
    bool htmlAlive = false;
    bool countRefreshCallbacks = false;
};

class CallbackScope final
{
public:
    CallbackScope(HtmlProbeState& state, bool htmlCallback)
        : m_state(state)
    {
        ++m_state.activeCallbacks;
        if ( htmlCallback && !m_state.htmlAlive )
            ++m_state.lateHtmlCallbacks;
    }

    ~CallbackScope()
    {
        --m_state.activeCallbacks;
    }

private:
    HtmlProbeState& m_state;
};

class ProbeHtmlWindow final : public wxHtmlWindow
{
public:
    ProbeHtmlWindow(wxWindow *parent,
                    const std::shared_ptr<HtmlProbeState>& state)
        : wxHtmlWindow(parent, wxID_ANY,
                       wxDefaultPosition, wxDefaultSize,
                       wxHW_DEFAULT_STYLE),
          m_state(state)
    {
        ++m_state->liveHtmlWindows;
        m_state->htmlAlive = true;
    }

    ~ProbeHtmlWindow() override
    {
        m_state->htmlAlive = false;
        --m_state->liveHtmlWindows;
    }

    void CountRefreshCallbacks(bool count = true)
    {
        m_state->countRefreshCallbacks = count;
    }

    void DestroyFrameOnNextLink(wxFrame *frame)
    {
        m_destroyOnLink = wxWeakRef<wxFrame>(frame);
    }

    void Refresh(bool eraseBackground = true,
                 const wxRect *rect = nullptr) override
    {
        if ( m_state->countRefreshCallbacks )
        {
            CallbackScope callback(*m_state, true);
            ++m_state->refreshCallbacks;
        }
        wxHtmlWindow::Refresh(eraseBackground, rect);
    }

    void OnLinkClicked(const wxHtmlLinkInfo& link) override
    {
        CallbackScope callback(*m_state, true);
        ++m_state->linkCallbacks;

        if ( wxFrame * const frame = m_destroyOnLink.get() )
        {
            m_destroyOnLink = wxWeakRef<wxFrame>();
            frame->Destroy();
            return;
        }

        wxHtmlWindow::OnLinkClicked(link);
    }

    wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type,
                                     const wxString& url,
                                     wxString *redirect) const override
    {
        wxUnusedVar(redirect);
        CallbackScope callback(*m_state, true);
        ++m_state->openingCallbacks;
        if ( type >= wxHTML_URL_PAGE && type <= wxHTML_URL_OTHER )
            ++m_state->openingByType[static_cast<std::size_t>(type)];

        if ( url.StartsWith("memory:winui-html-014-") )
            return wxHTML_OPEN;

        ++m_state->blockedRequests;
        return wxHTML_BLOCK;
    }

private:
    std::shared_ptr<HtmlProbeState> m_state;
    wxWeakRef<wxFrame> m_destroyOnLink;
};

class HtmlFallbackFixture final
{
public:
    explicit HtmlFallbackFixture(
        const std::shared_ptr<HtmlProbeState>& state)
        : m_state(state)
    {
    }

    ~HtmlFallbackFixture()
    {
        Cleanup();
        DrainUntil(
            [this]()
            {
                return !m_frameA && !m_frameB && !m_pane && !m_html &&
                       !m_sentinel;
            });
        DrainToQuiescence();
    }

    bool Create(int ordinal)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI HTML fallback A",
            wxDefaultPosition, wxSize(520, 360));
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI HTML fallback B",
            wxDefaultPosition, wxSize(520, 360));
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);

        if ( !frameA->GetHWND() || !frameB->GetHWND() )
            return false;

        wxPanel * const pane = new wxPanel(frameA, wxID_ANY);
        ProbeHtmlWindow * const html = new ProbeHtmlWindow(pane, m_state);
        wxButton * const sentinel = new wxButton(
            pane, wxID_ANY, "shared WinUI slot sentinel");

        wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(html, 1, wxEXPAND | wxALL, 4);
        sizer->Add(sentinel, 0,
                   wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
        pane->SetSizer(sizer);
        pane->SetSize(frameA->GetClientSize());

        m_pane = wxWeakRef<wxPanel>(pane);
        m_html = wxWeakRef<ProbeHtmlWindow>(html);
        m_sentinel = wxWeakRef<wxButton>(sentinel);

        if ( !ShowOffscreenWithoutActivation(frameA, ordinal) ||
             !ShowOffscreenWithoutActivation(frameB, ordinal + 1) )
        {
            return false;
        }

        return DrainUntil([this]() { return IsOwnedOnlyBy(GetFrameA()); });
    }

    bool MoveTo(wxFrame *destination)
    {
        wxPanel * const pane = GetPane();
        if ( !pane || !destination || !pane->Reparent(destination) )
            return false;

        pane->SetSize(destination->GetClientSize());
        pane->Layout();
        return DrainUntil(
            [this, destination]() { return IsOwnedOnlyBy(destination); }, 60);
    }

    bool IsOwnedOnlyBy(wxFrame *expectedFrame) const
    {
        wxPanel * const pane = GetPane();
        ProbeHtmlWindow * const html = GetHtml();
        wxButton * const sentinel = GetSentinel();
        if ( !pane || !html || !sentinel || !expectedFrame ||
             pane->GetParent() != expectedFrame )
        {
            return false;
        }

        wxWinUITopLevelHost * const expectedHost =
            wxWinUITopLevelHost::FindForTLW(expectedFrame);
        if ( !expectedHost ||
             wxWinUITopLevelHost::FindSlotOwner(sentinel) != expectedHost ||
             !expectedHost->FindSlot(sentinel) )
        {
            return false;
        }

        // wxHtmlWindow and its pane stay in the painted/HWND fallback. Only
        // the ordinary wxButton sentinel contributes a XAML slot.
        if ( wxWinUITopLevelHost::FindSlotOwner(html) ||
             wxWinUITopLevelHost::FindSlotOwner(pane) )
        {
            return false;
        }

        wxFrame * const frames[] = {GetFrameA(), GetFrameB()};
        for ( wxFrame * const frame : frames )
        {
            if ( !frame || frame == expectedFrame )
                continue;
            wxWinUITopLevelHost * const otherHost =
                wxWinUITopLevelHost::FindForTLW(frame);
            if ( otherHost && otherHost->FindSlot(sentinel) )
                return false;
        }

        return true;
    }

    bool DestroyAndWait(const RuntimeSnapshot& before)
    {
        Cleanup();
        if ( !DrainUntil(
                [this, &before]()
                {
                    return !m_frameA && !m_frameB && !m_pane && !m_html &&
                           !m_sentinel && before.IsRestored();
                }) )
        {
            return false;
        }

        return DrainToQuiescence() && before.IsRestored() &&
               FlushCallbacksStayQuiescent();
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    wxPanel *GetPane() const { return m_pane.get(); }
    ProbeHtmlWindow *GetHtml() const { return m_html.get(); }
    wxButton *GetSentinel() const { return m_sentinel.get(); }

private:
    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        if ( wxFrame * const frame = m_frameA.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        if ( wxFrame * const frame = m_frameB.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
    }

    bool m_cleaned = false;
    std::shared_ptr<HtmlProbeState> m_state;
    wxWeakRef<wxFrame> m_frameA;
    wxWeakRef<wxFrame> m_frameB;
    wxWeakRef<wxPanel> m_pane;
    wxWeakRef<ProbeHtmlWindow> m_html;
    wxWeakRef<wxButton> m_sentinel;
};

void CheckProbeRestored(const std::shared_ptr<HtmlProbeState>& state)
{
    CHECK(state->liveHtmlWindows == 0);
    CHECK(state->activeCallbacks == 0);
    CHECK(state->lateHtmlCallbacks == 0);
#if wxUSE_GIF && wxUSE_TIMER
    CHECK(state->liveTestTimers == 0);
#endif
}

#if wxUSE_GIF && wxUSE_TIMER

class DestroyFrameTimer final : public wxTimer
{
public:
    DestroyFrameTimer(wxFrame *frame,
                      const std::shared_ptr<HtmlProbeState>& state)
        : m_frame(frame),
          m_state(state)
    {
        ++m_state->liveTestTimers;
    }

    ~DestroyFrameTimer() override
    {
        if ( IsRunning() )
            Stop();
        --m_state->liveTestTimers;
    }

    void Notify() override
    {
        CallbackScope callback(*m_state, false);
        ++m_state->timerCallbacks;
        if ( wxFrame * const frame = m_frame.get() )
            frame->Destroy();
    }

private:
    wxWeakRef<wxFrame> m_frame;
    std::shared_ptr<HtmlProbeState> m_state;
};

#endif // wxUSE_GIF && wxUSE_TIMER

#if wxUSE_PRINTING_ARCHITECTURE

class ModelGateHtmlPrintout final : public wxHtmlPrintout
{
public:
    explicit ModelGateHtmlPrintout(bool allow)
        : wxHtmlPrintout("WinUI HTML offscreen print gate"),
          m_allow(allow)
    {
    }

    unsigned GetFitChecks() const { return m_fitChecks; }

protected:
    bool CheckFit(const wxSize& pageArea,
                  const wxSize& docArea) const override
    {
        wxUnusedVar(pageArea);
        wxUnusedVar(docArea);
        ++m_fitChecks;
        return m_allow;
    }

private:
    const bool m_allow;
    mutable unsigned m_fitChecks = 0;
};

#endif // wxUSE_PRINTING_ARCHITECTURE

} // anonymous namespace

TEST_CASE("WinUIHtml::SetAppendSelectionAndURLGate",
          "[winui-html][html]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const auto state = std::make_shared<HtmlProbeState>();

    HtmlFallbackFixture fixture(state);
    REQUIRE(fixture.Create(0));
    ProbeHtmlWindow * const html = fixture.GetHtml();
    REQUIRE(html);

    // Hosts are lazy: frame B has no XAML slot until the pane migrates there.
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 1);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 1);
    CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
          before.buttonCallbacks + 1);
    CHECK(fixture.IsOwnedOnlyBy(fixture.GetFrameA()));

    REQUIRE(html->SetPage(
        "<html><head><title>SetPage title</title></head><body>"
        "<p>alpha</p><table><tr><td>beta</td><td>gamma</td></tr>"
        "</table><a href=\"memory:winui-html-014-page-b.htm\">"
        "local-link</a></body></html>"));
    CHECK(html->GetOpenedPage().empty());
    CHECK(html->GetOpenedPageTitle() == "SetPage title");
    REQUIRE(html->GetInternalRepresentation());

    REQUIRE(html->AppendToPage("<p>appended-delta</p>"));
#if wxUSE_CLIPBOARD
    html->SelectAll();
    const wxString selection = html->SelectionToText();
    CHECK(selection.Contains("alpha"));
    CHECK(selection.Contains("beta"));
    CHECK(selection.Contains("gamma"));
    CHECK(selection.Contains("local-link"));
    CHECK(selection.Contains("appended-delta"));
#endif // wxUSE_CLIPBOARD

    wxString redirect;
    CHECK(html->OnHTMLOpeningURL(
              wxHTML_URL_PAGE, "https://example.invalid/never-opened",
              &redirect) == wxHTML_BLOCK);
    CHECK(state->blockedRequests == 1);
    CHECK(state->activeCallbacks == 0);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    CheckProbeRestored(state);
}

TEST_CASE("WinUIHtml::LocalHistoryReloadAndLink",
          "[winui-html][html][history]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const auto state = std::make_shared<HtmlProbeState>();
    MemoryHtmlResources resources;

    HtmlFallbackFixture fixture(state);
    REQUIRE(fixture.Create(4));
    ProbeHtmlWindow * const html = fixture.GetHtml();
    REQUIRE(html);

    REQUIRE(html->LoadPage(MemoryURL(PageAName) + "#tail"));
    CHECK(html->GetOpenedPage() == MemoryURL(PageAName));
    CHECK(html->GetOpenedAnchor() == "tail");
    CHECK(html->GetOpenedPageTitle() == "History A");
    CHECK_FALSE(html->HistoryCanBack());

    int viewX = 0;
    int viewY = 0;
    html->GetViewStart(&viewX, &viewY);
    CHECK(viewY > 0);

    html->OnLinkClicked(wxHtmlLinkInfo(MemoryURL(PageBName)));
    CHECK(state->linkCallbacks == 1);
    CHECK(html->GetOpenedPage() == MemoryURL(PageBName));
    CHECK(html->GetOpenedPageTitle() == "History B");
    CHECK(html->HistoryCanBack());
    CHECK_FALSE(html->HistoryCanForward());

    REQUIRE(html->HistoryBack());
    CHECK(html->GetOpenedPage() == MemoryURL(PageAName));
    CHECK(html->GetOpenedAnchor() == "tail");
    CHECK(html->HistoryCanForward());

    REQUIRE(html->HistoryForward());
    CHECK(html->GetOpenedPage() == MemoryURL(PageBName));
    CHECK(html->GetOpenedPageTitle() == "History B");

    resources.ReplacePageB(
        "<html><head><title>History B reloaded</title></head><body>"
        "<p>history-b-reloaded</p></body></html>");
    const unsigned openingsBeforeReload = state->openingCallbacks;
    REQUIRE(html->LoadPage(MemoryURL(PageBName)));
    CHECK(state->openingCallbacks > openingsBeforeReload);
    CHECK(html->GetOpenedPageTitle() == "History B reloaded");
#if wxUSE_CLIPBOARD
    CHECK(html->ToText().Contains("history-b-reloaded"));
#endif // wxUSE_CLIPBOARD

    html->HistoryClear();
    CHECK_FALSE(html->HistoryCanBack());
    CHECK_FALSE(html->HistoryCanForward());
    CHECK(state->openingByType[wxHTML_URL_PAGE] >= 5);
    CHECK(state->activeCallbacks == 0);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    CheckProbeRestored(state);
}

TEST_CASE("WinUIHtml::LargeScrollAndTwoTLWStress",
          "[winui-html][html][stress]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const auto state = std::make_shared<HtmlProbeState>();

    HtmlFallbackFixture fixture(state);
    REQUIRE(fixture.Create(8));
    ProbeHtmlWindow * const html = fixture.GetHtml();
    REQUIRE(html);

    REQUIRE(html->SetPage(MakeLargePage()));
    CHECK(html->GetVirtualSize().y > html->GetClientSize().y);

    int unitX = 0;
    int unitY = 0;
    html->GetScrollPixelsPerUnit(&unitX, &unitY);
    REQUIRE(unitY > 0);
    html->Scroll(0, html->GetVirtualSize().y / unitY);
    int viewX = 0;
    int viewY = 0;
    html->GetViewStart(&viewX, &viewY);
    CHECK(viewY > 0);

    // One cycle is a complete A -> B -> A host transaction. This deliberately
    // matches the reusable 100-cycle primitive in winuiadvancedharness.cpp.
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        REQUIRE(html->SetPage(wxString::Format(
            "<html><head><title>cycle %u</title></head><body>"
            "<p>cycle-body-%u</p></body></html>",
            cycle, cycle)));
        REQUIRE(html->AppendToPage(wxString::Format(
            "<p>cycle-append-%u</p>", cycle)));

        REQUIRE(fixture.MoveTo(fixture.GetFrameB()));
        CHECK(fixture.IsOwnedOnlyBy(fixture.GetFrameB()));
        REQUIRE(fixture.MoveTo(fixture.GetFrameA()));
        CHECK(fixture.IsOwnedOnlyBy(fixture.GetFrameA()));
    }

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 2);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 1);
    CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
          before.buttonCallbacks + 1);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    CheckProbeRestored(state);
}

TEST_CASE("WinUIHtml::DestroyDuringLinkCallback",
          "[winui-html][html][lifetime]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const auto state = std::make_shared<HtmlProbeState>();

    HtmlFallbackFixture fixture(state);
    REQUIRE(fixture.Create(12));
    ProbeHtmlWindow * const html = fixture.GetHtml();
    REQUIRE(html);

    const wxWeakRef<ProbeHtmlWindow> htmlLifetime(html);
    html->DestroyFrameOnNextLink(fixture.GetFrameA());
    html->OnLinkClicked(wxHtmlLinkInfo("memory:winui-html-014-unused.htm"));

    CHECK(state->linkCallbacks == 1);
    CHECK(state->activeCallbacks == 0);
    REQUIRE(DrainUntil([&htmlLifetime]() { return !htmlLifetime; }));
    CHECK(state->liveHtmlWindows == 0);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    CheckProbeRestored(state);
}

#if wxUSE_GIF && wxUSE_TIMER

TEST_CASE("WinUIHtml::AnimatedImageTimerDestruction",
          "[winui-html][html][animation][lifetime]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const auto state = std::make_shared<HtmlProbeState>();
    MemoryHtmlResources resources;

    HtmlFallbackFixture fixture(state);
    REQUIRE(fixture.Create(16));
    ProbeHtmlWindow * const html = fixture.GetHtml();
    REQUIRE(html);

    wxString animatedPage =
        "<html><head><title>Animated local image</title></head><body>"
        "<p>before-image</p><img src=\"";
    animatedPage += MemoryURL(AnimatedGifName);
    animatedPage +=
        "\" alt=\"animated-local\"><p>after-image</p>"
        "<img src=\"memory:winui-html-014-missing.png\" "
        "alt=\"missing-local\"></body></html>";

    REQUIRE(html->SetPage(animatedPage));
    CHECK(state->openingByType[wxHTML_URL_IMAGE] == 2);
#if wxUSE_CLIPBOARD
    CHECK(html->ToText().Contains("animated-local"));
    CHECK(html->ToText().Contains("missing-local"));
#endif // wxUSE_CLIPBOARD

    REQUIRE(DrainToQuiescence());
    html->CountRefreshCallbacks();
    const unsigned refreshesBeforeTimer = state->refreshCallbacks;
    REQUIRE(DrainUntil(
        [state, refreshesBeforeTimer]()
        {
            return state->refreshCallbacks > refreshesBeforeTimer;
        },
        80));

    const wxWeakRef<ProbeHtmlWindow> htmlLifetime(html);
    auto destroyTimer =
        std::make_unique<DestroyFrameTimer>(fixture.GetFrameA(), state);
    REQUIRE(destroyTimer->StartOnce(1));
    REQUIRE(DrainUntil(
        [state, &htmlLifetime]()
        {
            return state->timerCallbacks == 1 && !htmlLifetime;
        }));
    CHECK_FALSE(destroyTimer->IsRunning());
    destroyTimer.reset();

    const unsigned refreshesAfterDestroy = state->refreshCallbacks;
    DrainDispatch(10);
    CHECK(state->refreshCallbacks == refreshesAfterDestroy);
    CHECK(state->activeCallbacks == 0);
    CHECK(state->lateHtmlCallbacks == 0);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    CheckProbeRestored(state);
}

#endif // wxUSE_GIF && wxUSE_TIMER

TEST_CASE("WinUIHtml::PrintingAndHelpModelGates",
          "[winui-html][html][model]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    MemoryHtmlResources resources;

#if wxUSE_PRINTING_ARCHITECTURE
    wxBitmap bitmap(800, 800);
    wxMemoryDC dc(bitmap);
    REQUIRE(dc.IsOk());

    {
        ModelGateHtmlPrintout printout(true);
        printout.SetMargins(0, 0, 0, 0, 0);
        printout.SetHeader("local header @PAGENUM@/@PAGESCNT@");
        printout.SetFooter("local footer @PAGENUM@/@PAGESCNT@");
        REQUIRE(printout.SetUp(dc));
        printout.SetHtmlText(
            "<html><body><h1>offscreen print</h1>"
            "<p>deterministic local page</p></body></html>");
        REQUIRE_NOTHROW(printout.OnPreparePrinting());
        CHECK(printout.GetFitChecks() == 1);
        CHECK(printout.HasPage(1));
        CHECK(printout.OnPrintPage(1));
    }

    {
        ModelGateHtmlPrintout cancelled(false);
        cancelled.SetMargins(0, 0, 0, 0, 0);
        REQUIRE(cancelled.SetUp(dc));
        cancelled.SetHtmlText(
            "<html><body><p>cancelled before physical output</p>"
            "</body></html>");
        REQUIRE_NOTHROW(cancelled.OnPreparePrinting());
        CHECK(cancelled.GetFitChecks() == 1);
        CHECK_FALSE(cancelled.HasPage(1));
    }

    dc.SelectObject(wxNullBitmap);
#endif // wxUSE_PRINTING_ARCHITECTURE

#if wxUSE_WXHTML_HELP
    {
        wxHtmlHelpController help(wxHF_FRAME);
        CHECK_FALSE(help.GetHelpWindow());
        CHECK_FALSE(help.GetFrame());
        CHECK_FALSE(help.GetDialog());

        REQUIRE(help.AddBook(MemoryURL(HelpProjectName), false));
        REQUIRE(help.GetHelpData());
        CHECK(help.GetHelpData()->GetBookRecArray().GetCount() == 1);
        CHECK(help.GetHelpData()
                  ->FindPageByName(HelpTopicName)
                  .EndsWith(HelpTopicName));

        // Loading the local help model must not instantiate its frame/dialog.
        CHECK_FALSE(help.GetHelpWindow());
        CHECK_FALSE(help.GetFrame());
        CHECK_FALSE(help.GetDialog());
    }
#endif // wxUSE_WXHTML_HELP

    REQUIRE(DrainToQuiescence());
    CHECK(before.IsRestored());
    before.CheckRestored(false);
    CHECK(FlushCallbacksStayQuiescent());
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_HTML && wxUSE_BUTTON
