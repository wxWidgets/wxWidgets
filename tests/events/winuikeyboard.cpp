///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/winuikeyboard.cpp
// Purpose:     Deterministic tests for the wxWinUI keyboard pipeline
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/choice.h"
    #include "wx/combobox.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/textctrl.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/msw/wrapwin.h"
#include "wx/msw/evtloop.h"
#include "wx/stopwatch.h"
#include "wx/uiaction.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/xamlhost.h"
#include "choice-test-access.h"
#include "combobox-test-access.h"

#include <array>
#include <vector>

namespace
{

enum class PipelineStage
{
    ShouldPreProcess,
    WxTranslate,
    WxProcess,
    XamlPreTranslate
};

struct PipelineTrace
{
    std::vector<PipelineStage> stages;
    unsigned xamlCalls = 0;
    unsigned characterCalls = 0;
    bool xamlHandles = false;
};

class PipelineOwner : public wxWindow
{
public:
    explicit PipelineOwner(PipelineTrace& trace)
        : m_trace(trace)
    {
    }

    bool shouldPreProcess = true;
    bool wxHandles = false;
    bool mnemonicHandles = false;
    unsigned shouldCalls = 0;
    unsigned translateCalls = 0;
    unsigned processCalls = 0;

    bool MSWShouldPreProcessMessage(WXMSG *) override
    {
        ++shouldCalls;
        m_trace.stages.push_back(PipelineStage::ShouldPreProcess);
        return shouldPreProcess;
    }

    bool MSWTranslateMessage(WXMSG *) override
    {
        ++translateCalls;
        m_trace.stages.push_back(PipelineStage::WxTranslate);
        return wxHandles;
    }

    bool MSWProcessMessage(WXMSG *) override
    {
        ++processCalls;
        m_trace.stages.push_back(PipelineStage::WxProcess);
        return mnemonicHandles;
    }

private:
    PipelineTrace& m_trace;
};

// Keep the real wxTextCtrl implementation of MSWShouldPreProcessMessage(), but
// make reaching the accelerator stage directly observable.
class TextCtrlPipelineProbe : public wxTextCtrl
{
public:
    bool MSWTranslateMessage(WXMSG *) override
    {
        ++translateCalls;
        return true;
    }

    unsigned translateCalls = 0;
};

class ChoicePipelineProbe : public wxChoice
{
public:
    using wxChoice::MSWShouldPreProcessMessage;
};

class ComboPipelineProbe : public wxComboBox
{
public:
    using wxComboBox::MSWShouldPreProcessMessage;
};

bool TraceXamlPreTranslate(WXMSG *msg, void *context)
{
    PipelineTrace& trace = *static_cast<PipelineTrace *>(context);
    ++trace.xamlCalls;
    if ( msg->message == WM_CHAR )
        ++trace.characterCalls;
    trace.stages.push_back(PipelineStage::XamlPreTranslate);
    return trace.xamlHandles;
}

wxWinUIKeyboardModifiers
Modifiers(bool control = false,
          bool leftAlt = false,
          bool rightAlt = false,
          bool shift = false)
{
    wxWinUIKeyboardModifiers modifiers = {};
    modifiers.shiftDown = shift;
    modifiers.controlDown = control;
    modifiers.leftAltDown = leftAlt;
    modifiers.rightAltDown = rightAlt;
    return modifiers;
}

MSG KeyMessage(UINT message, WPARAM key)
{
    MSG msg = {};
    msg.message = message;
    msg.wParam = key;
    return msg;
}

wxWinUIKeyboardPipelineResult
RunPipeline(MSG& msg,
            wxWindow *owner,
            const wxWinUIKeyboardModifiers& modifiers,
            PipelineTrace& trace)
{
    return wxWinUI3RunKeyboardPipelineForTesting(
        &msg, owner, modifiers, TraceXamlPreTranslate, &trace);
}

} // anonymous namespace

TEST_CASE("WinUIKeyboard::StageOrdering", "[WinUIKeyboard][keyboard]")
{
    PipelineTrace trace;
    PipelineOwner owner(trace);
    MSG msg = KeyMessage(WM_KEYDOWN, VK_F6);

    SECTION("wx consumes before XAML")
    {
        owner.wxHandles = true;
        trace.xamlHandles = true;

        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
        CHECK(owner.shouldCalls == 1);
        CHECK(owner.translateCalls == 1);
        CHECK(trace.xamlCalls == 0);
        REQUIRE(trace.stages.size() == 2);
        CHECK(trace.stages[0] == PipelineStage::ShouldPreProcess);
        CHECK(trace.stages[1] == PipelineStage::WxTranslate);
    }

    SECTION("XAML sees an unhandled message exactly once")
    {
        trace.xamlHandles = true;

        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::XamlHandled);
        CHECK(owner.shouldCalls == 1);
        CHECK(owner.translateCalls == 1);
        CHECK(trace.xamlCalls == 1);
        REQUIRE(trace.stages.size() == 3);
        CHECK(trace.stages[0] == PipelineStage::ShouldPreProcess);
        CHECK(trace.stages[1] == PipelineStage::WxTranslate);
        CHECK(trace.stages[2] == PipelineStage::XamlPreTranslate);
    }

    SECTION("neither stage handles")
    {
        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::NotHandled);
        CHECK(owner.translateCalls == 1);
        CHECK(trace.xamlCalls == 1);
    }
}

TEST_CASE("WinUIKeyboard::AltGrIsNotCtrlAlt",
          "[WinUIKeyboard][keyboard]")
{
    PipelineTrace trace;
    PipelineOwner owner(trace);
    owner.wxHandles = true;
    trace.xamlHandles = true;

    MSG msg = KeyMessage(WM_SYSKEYDOWN, 'Q');
    msg.lParam = static_cast<LPARAM>(1u << 29); // KF_ALTDOWN

    SECTION("Ctrl plus right Alt is AltGr")
    {
        CHECK(RunPipeline(msg, &owner,
                          Modifiers(true, false, true), trace) ==
              wxWinUIKeyboardPipelineResult::XamlHandled);
        CHECK(owner.translateCalls == 0);
        CHECK(trace.xamlCalls == 1);
    }

    SECTION("Ctrl plus left Alt remains an accelerator chord")
    {
        CHECK(RunPipeline(msg, &owner,
                          Modifiers(true, true, false), trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
        CHECK(owner.translateCalls == 1);
        CHECK(trace.xamlCalls == 0);
    }

    SECTION("Alt mnemonic uses the narrow wx process stage")
    {
        owner.wxHandles = false;
        owner.mnemonicHandles = true;
        msg = KeyMessage(WM_SYSCHAR, 'Q');

        CHECK(RunPipeline(msg, &owner,
                          Modifiers(false, true, false), trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
        CHECK(owner.translateCalls == 0);
        CHECK(owner.processCalls == 1);
        CHECK(trace.xamlCalls == 0);
    }

    SECTION("AltGr system character remains printable")
    {
        owner.mnemonicHandles = true;
        msg = KeyMessage(WM_SYSCHAR, '@');

        CHECK(RunPipeline(msg, &owner,
                          Modifiers(true, false, true), trace) ==
              wxWinUIKeyboardPipelineResult::XamlHandled);
        CHECK(owner.processCalls == 0);
        CHECK(trace.xamlCalls == 1);
    }
}

TEST_CASE("WinUIKeyboard::CharacterMessagesBypassAccelerators",
          "[WinUIKeyboard][keyboard]")
{
    const UINT messages[] =
    {
        WM_CHAR,
        WM_DEADCHAR,
        WM_SYSDEADCHAR
    };

    for ( const UINT message : messages )
    {
        INFO("Windows message: " << message);

        PipelineTrace trace;
        PipelineOwner owner(trace);
        owner.wxHandles = true;
        trace.xamlHandles = true;
        MSG msg = KeyMessage(message, '^');

        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::XamlHandled);
        CHECK(owner.translateCalls == 0);
        CHECK(trace.xamlCalls == 1);
    }
}

TEST_CASE("WinUIKeyboard::TabBypassesAcceleratorsAndReachesXaml",
          "[WinUIKeyboard][keyboard]")
{
    PipelineTrace trace;
    PipelineOwner owner(trace);
    owner.wxHandles = true;
    trace.xamlHandles = true;
    MSG msg = KeyMessage(WM_KEYDOWN, VK_TAB);

    CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(owner.shouldCalls == 0);
    CHECK(owner.translateCalls == 0);
    CHECK(trace.xamlCalls == 1);

    // Alt+Tab is a system key, not wx/XAML tab traversal.
    PipelineTrace systemTrace;
    PipelineOwner systemOwner(systemTrace);
    MSG systemTab = KeyMessage(WM_SYSKEYDOWN, VK_TAB);
    CHECK(RunPipeline(systemTab, &systemOwner,
                      Modifiers(false, true), systemTrace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    CHECK(systemOwner.shouldCalls == 1);
    CHECK(systemOwner.translateCalls == 1);
    CHECK(systemTrace.xamlCalls == 1);
}

TEST_CASE("WinUIKeyboard::DeferredCharHookVetoPrecedesAllStages",
          "[WinUIKeyboard][keyboard]")
{
    for ( const UINT message : { WM_KEYDOWN, WM_SYSKEYDOWN } )
    {
        INFO("message = " << message);

        PipelineTrace trace;
        PipelineOwner owner(trace);
        owner.wxHandles = true;
        trace.xamlHandles = true;
        MSG msg = KeyMessage(message, 'K');

        wxWinUI3SetBlockedKeyboardKeyForTesting('K');
        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
        CHECK(owner.shouldCalls == 0);
        CHECK(owner.translateCalls == 0);
        CHECK(trace.xamlCalls == 0);

        // The veto is one-shot: the following physical key message takes the
        // normal path instead of suppressing an unrelated future key.
        owner.wxHandles = false;
        CHECK(RunPipeline(msg, &owner, Modifiers(), trace) ==
              wxWinUIKeyboardPipelineResult::XamlHandled);
        CHECK(owner.translateCalls == 1);
        CHECK(trace.xamlCalls == 1);
    }
}

TEST_CASE("WinUIKeyboard::TextEntryOwnsCtrlA",
           "[WinUIKeyboard][keyboard]")
{
    TextCtrlPipelineProbe text;
    PipelineTrace trace;
    trace.xamlHandles = true;
    MSG msg = KeyMessage(WM_KEYDOWN, 'A');

    CHECK(RunPipeline(msg, &text, Modifiers(true), trace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(text.translateCalls == 0);
    CHECK(trace.xamlCalls == 1);
}

TEST_CASE("WinUIKeyboard::MultilineTextOwnsPlainReturn",
          "[WinUIKeyboard][keyboard][winui-textpeer]")
{
    wxFrame * const frame =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "WinUI multiline Return ownership test");
    TextCtrlPipelineProbe * const text = new TextCtrlPipelineProbe;
    REQUIRE(text->Create(frame, wxID_ANY, wxEmptyString,
                         wxDefaultPosition, wxDefaultSize,
                         wxTE_MULTILINE));

    PipelineTrace trace;
    trace.xamlHandles = true;
    MSG enter = KeyMessage(WM_KEYDOWN, VK_RETURN);

    CHECK(RunPipeline(enter, text, Modifiers(), trace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(text->translateCalls == 0);
    CHECK(trace.xamlCalls == 1);

    frame->Destroy();
    wxYield();
}

TEST_CASE("WinUIKeyboard::ChoiceAndComboOwnLocalKeys",
          "[WinUIKeyboard][keyboard]")
{
    wxFrame * const frame =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "WinUI choice key ownership test");
    ChoicePipelineProbe * const choice = new ChoicePipelineProbe;
    REQUIRE(choice->Create(frame, wxID_ANY));
    ComboPipelineProbe * const combo = new ComboPipelineProbe;
    REQUIRE(combo->Create(frame, wxID_ANY, wxEmptyString));
    frame->Show();
    wxYield();

    MSG f4 = KeyMessage(WM_KEYDOWN, VK_F4);
    CHECK(!choice->MSWShouldPreProcessMessage(&f4));
    CHECK(!combo->MSWShouldPreProcessMessage(&f4));

    combo->Popup();
    MSG escape = KeyMessage(WM_KEYDOWN, VK_ESCAPE);
    MSG enter = KeyMessage(WM_KEYDOWN, VK_RETURN);
    CHECK(!combo->MSWShouldPreProcessMessage(&escape));
    CHECK(!combo->MSWShouldPreProcessMessage(&enter));
    combo->Dismiss();

    frame->Destroy();
    wxYield();
}

#if wxUSE_MENUS && wxUSE_ACCEL

namespace
{

constexpr int WinUIKeyboard_AcceleratorId = wxID_HIGHEST + 417;
constexpr int WinUIKeyboard_CtrlTId = wxID_HIGHEST + 418;
constexpr int WinUIKeyboard_F4Id = wxID_HIGHEST + 419;
constexpr int WinUIKeyboard_EscapeId = wxID_HIGHEST + 420;
constexpr int WinUIKeyboard_EnterId = wxID_HIGHEST + 421;
constexpr int WinUIKeyboard_AltMId = wxID_HIGHEST + 422;

struct AcceleratorFrame
{
    AcceleratorFrame()
    {
        frame = new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                            "WinUI keyboard pipeline test");
        owner = new wxTextCtrl(frame, wxID_ANY);

        menu = new wxMenu;
        menu->Append(WinUIKeyboard_AcceleratorId, "Run test\tF6");
        menu->Append(WinUIKeyboard_CtrlTId, "New test\tCtrl+T");
        menu->Append(WinUIKeyboard_F4Id, "Parent F4\tF4");
        menu->Append(WinUIKeyboard_EscapeId, "Parent Escape\tEsc");
        menu->Append(WinUIKeyboard_EnterId, "Parent Enter\tEnter");
        menu->Append(WinUIKeyboard_AltMId, "Parent mnemonic\tAlt+M");

        wxMenuBar * const menuBar = new wxMenuBar;
        menuBar->Append(menu, "&Test");
        frame->SetMenuBar(menuBar);
        frame->Show();
        wxYield();

        host = wxWinUITopLevelHost::ForWindow(owner, false);
        if ( host )
            host->FocusSlot(owner);
    }

    ~AcceleratorFrame() { Destroy(); }

    void Destroy()
    {
        if ( frame )
        {
            frame->Destroy();
            frame = nullptr;
            owner = nullptr;
            menu = nullptr;
            host = nullptr;
            wxYield();
        }
    }

    wxFrame *frame = nullptr;
    wxWindow *owner = nullptr;
    wxMenu *menu = nullptr;
    wxWinUITopLevelHost *host = nullptr;
};

#if wxUSE_UIACTIONSIMULATOR

bool IsPhysicalInputSafeForTesting(const AcceleratorFrame& fixture,
                                   wxTextCtrl *text,
                                   wxString *diagnostic)
{
    static const wxString acknowledgement =
        "I_UNDERSTAND_THIS_TEST_INJECTS_SYSTEM_INPUT";

    wxString suppliedAcknowledgement;
    const bool acknowledged =
        wxGetEnv("WX_WINUI_PHYSICAL_INPUT_ACK", &suppliedAcknowledgement) &&
        suppliedAcknowledgement == acknowledgement;

    DWORD processSession = 0;
    const DWORD activeConsoleSession = ::WTSGetActiveConsoleSessionId();
    const bool consoleSession =
        ::ProcessIdToSessionId(::GetCurrentProcessId(), &processSession) &&
        processSession == activeConsoleSession;

    const bool modifiersReleased =
        !(::GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        !(::GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        !(::GetAsyncKeyState(VK_MENU) & 0x8000) &&
        !(::GetAsyncKeyState(VK_LWIN) & 0x8000) &&
        !(::GetAsyncKeyState(VK_RWIN) & 0x8000);

    const HDESK threadDesktop =
        ::GetThreadDesktop(::GetCurrentThreadId());
    BOOL receivesInput = FALSE;
    DWORD desktopInfoSize = 0;
    const bool activeInputDesktop =
        threadDesktop != nullptr &&
        ::GetUserObjectInformationW(threadDesktop, UOI_IO,
                                    &receivesInput, sizeof(receivesInput),
                                    &desktopInfoSize) &&
        receivesInput;

    const HWND foreground = ::GetForegroundWindow();
    DWORD foregroundProcess = 0;
    if ( foreground )
        ::GetWindowThreadProcessId(foreground, &foregroundProcess);

    const HWND frame = fixture.frame
        ? reinterpret_cast<HWND>(fixture.frame->GetHandle())
        : nullptr;
    const HWND foregroundRoot = foreground
        ? ::GetAncestor(foreground, GA_ROOT)
        : nullptr;
    const HWND frameRoot = frame ? ::GetAncestor(frame, GA_ROOT) : nullptr;

    const HWND nativeFocus = ::GetFocus();
    const bool exactFocus =
        fixture.host && text && nativeFocus &&
        fixture.host->GetFocusOwner() == text &&
        wxWinUITopLevelHost::ResolveFocusHwnd(
            reinterpret_cast<WXHWND>(nativeFocus)) == text;

    if ( diagnostic )
    {
        *diagnostic = wxString::Format(
            "physical input acknowledgement=%d consoleSession=%d "
            "processSession=%lu activeConsoleSession=%lu "
            "remoteSession=%d activeInputDesktop=%d modifiersReleased=%d "
            "foregroundProcess=%lu expectedProcess=%lu "
            "exactForeground=%d exactFocus=%d",
            acknowledged,
            consoleSession,
            processSession,
            activeConsoleSession,
            ::GetSystemMetrics(SM_REMOTESESSION),
            activeInputDesktop,
            modifiersReleased,
            foregroundProcess,
            ::GetCurrentProcessId(),
            foregroundRoot && foregroundRoot == frameRoot,
            exactFocus);
    }

    return acknowledged &&
           consoleSession &&
           !::GetSystemMetrics(SM_REMOTESESSION) &&
           activeInputDesktop &&
           modifiersReleased &&
           foregroundProcess == ::GetCurrentProcessId() &&
           foregroundRoot && foregroundRoot == frameRoot &&
           exactFocus;
}

#endif // wxUSE_UIACTIONSIMULATOR

class CriticalWindowScope final
{
public:
    explicit CriticalWindowScope(wxWindow *window)
    {
        wxGUIEventLoop::SetCriticalWindow(window);
    }

    ~CriticalWindowScope()
    {
        wxGUIEventLoop::SetCriticalWindow(nullptr);
    }

    CriticalWindowScope(const CriticalWindowScope&) = delete;
    CriticalWindowScope& operator=(const CriticalWindowScope&) = delete;
};

class KeyboardStateScope final
{
public:
    explicit KeyboardStateScope(int virtualKey = 0)
    {
        m_savedValid = ::GetKeyboardState(m_saved.data()) != FALSE;
        if ( !m_savedValid )
            return;

        auto current = m_saved;
        for ( const int modifier :
              { VK_SHIFT, VK_LSHIFT, VK_RSHIFT,
                VK_CONTROL, VK_LCONTROL, VK_RCONTROL,
                VK_MENU, VK_LMENU, VK_RMENU } )
        {
            current[modifier] &= 0x7f;
        }

        if ( virtualKey )
        {
            current[virtualKey] |= 0x80;
            if ( virtualKey == VK_CONTROL )
                current[VK_LCONTROL] |= 0x80;
            else if ( virtualKey == VK_MENU )
                current[VK_LMENU] |= 0x80;
            else if ( virtualKey == VK_SHIFT )
                current[VK_LSHIFT] |= 0x80;
        }

        m_applied = ::SetKeyboardState(current.data()) != FALSE;
    }

    ~KeyboardStateScope()
    {
        if ( m_savedValid )
            ::SetKeyboardState(m_saved.data());
    }

    bool IsApplied() const { return m_applied; }

    KeyboardStateScope(const KeyboardStateScope&) = delete;
    KeyboardStateScope& operator=(const KeyboardStateScope&) = delete;

private:
    std::array<BYTE, 256> m_saved = {};
    bool m_savedValid = false;
    bool m_applied = false;
};

} // anonymous namespace

TEST_CASE("WinUIKeyboard::MenuAcceleratorIsUniqueAndCanonical",
          "[WinUIKeyboard][keyboard][menu]")
{
    KeyboardStateScope noModifiers;
    REQUIRE(noModifiers.IsApplied());
    AcceleratorFrame fixture;
    unsigned commandCount = 0;
    wxObject *eventSource = nullptr;

    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event)
        {
            ++commandCount;
            eventSource = event.GetEventObject();
        },
        WinUIKeyboard_AcceleratorId);

    PipelineTrace trace;
    trace.xamlHandles = true;
    MSG msg = KeyMessage(WM_KEYDOWN, VK_F6);
    msg.hwnd = static_cast<HWND>(fixture.owner->GetHWND());

    CHECK(RunPipeline(msg, fixture.owner, Modifiers(), trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(commandCount == 1);
    CHECK(eventSource == fixture.menu);
    CHECK(trace.xamlCalls == 0);

    fixture.Destroy();
}

TEST_CASE("WinUIKeyboard::AcceleratorFocusMatrix",
          "[WinUIKeyboard][keyboard][menu]")
{
    AcceleratorFrame fixture;
    REQUIRE(fixture.host != nullptr);
    REQUIRE(fixture.host->GetBridgeHwnd() != nullptr);

    wxComboBox * const combo =
        new wxComboBox(fixture.frame, wxID_ANY, "combo",
                       wxPoint(0, 40), wxSize(140, 30));
    wxButton * const button =
        new wxButton(fixture.frame, wxID_ANY, "button",
                     wxPoint(0, 80), wxSize(100, 30));
    wxWinUIXamlHost * const genericGrid =
        new wxWinUIXamlHost(fixture.frame, wxID_ANY,
                            wxPoint(0, 120), wxSize(100, 30));
    REQUIRE(genericGrid->SetContentFromXaml(
        "<Grid xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><Button Content=\"Grid focus\"/></Grid>"));
    wxYield();

    const std::vector<std::pair<const char *, wxWindow *>> owners =
    {
        { "TextBox", fixture.owner },
        { "ComboBox", combo },
        { "Button", button },
        { "generic Grid", genericGrid }
    };

    unsigned f6Commands = 0;
    unsigned ctrlTCommands = 0;
    unsigned altMCommands = 0;
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++f6Commands; },
        WinUIKeyboard_AcceleratorId);
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++ctrlTCommands; },
        WinUIKeyboard_CtrlTId);
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++altMCommands; },
        WinUIKeyboard_AltMId);

    PipelineTrace trace;
    trace.xamlHandles = true;
    for ( size_t i = 0; i < owners.size(); ++i )
    {
        INFO("focus owner = " << owners[i].first);
        wxWindow * const owner = owners[i].second;
        REQUIRE(fixture.host->FindSlot(owner) != nullptr);
        fixture.host->FocusSlot(owner);
        REQUIRE(WaitFor("WinUI keyboard accelerator focus owner", [&]()
        {
            return fixture.host->GetFocusOwner() == owner &&
                   wxWinUITopLevelHost::ResolveFocusHwnd(
                       fixture.host->GetBridgeHwnd()) == owner;
        }, 1000));

        {
            KeyboardStateScope noModifiers;
            REQUIRE(noModifiers.IsApplied());
            MSG f6 = KeyMessage(WM_KEYDOWN, VK_F6);
            f6.hwnd = fixture.host->GetBridgeHwnd();
            REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
                HC_ACTION, PM_REMOVE, &f6, Modifiers(),
                TraceXamlPreTranslate, &trace));
            CHECK(f6.message == WM_NULL);
        }
        CHECK(f6Commands == i + 1);
        CHECK(ctrlTCommands == i);
        CHECK(altMCommands == i);
        CHECK(trace.xamlCalls == 0);

        {
            KeyboardStateScope controlDown(VK_CONTROL);
            REQUIRE(controlDown.IsApplied());
            MSG ctrlT = KeyMessage(WM_KEYDOWN, 'T');
            ctrlT.hwnd = fixture.host->GetBridgeHwnd();
            REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
                HC_ACTION, PM_REMOVE, &ctrlT, Modifiers(true),
                TraceXamlPreTranslate, &trace));
            CHECK(ctrlT.message == WM_NULL);
        }
        CHECK(f6Commands == i + 1);
        CHECK(ctrlTCommands == i + 1);
        CHECK(altMCommands == i);
        CHECK(trace.xamlCalls == 0);

        {
            KeyboardStateScope altDown(VK_MENU);
            REQUIRE(altDown.IsApplied());
            MSG altM = KeyMessage(WM_SYSKEYDOWN, 'M');
            altM.hwnd = fixture.host->GetBridgeHwnd();
            REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
                HC_ACTION, PM_REMOVE, &altM,
                Modifiers(false, true),
                TraceXamlPreTranslate, &trace));
            CHECK(altM.message == WM_NULL);
        }
        CHECK(f6Commands == i + 1);
        CHECK(ctrlTCommands == i + 1);
        CHECK(altMCommands == i + 1);
        CHECK(trace.xamlCalls == 0);
    }

    // Disabling the command rebuilds the native accelerator table. The same
    // Ctrl+T must now reach XAML once and produce no command.
    fixture.menu->Enable(WinUIKeyboard_CtrlTId, false);
    fixture.host->FocusSlot(fixture.owner);
    wxYield();
    {
        KeyboardStateScope controlDown(VK_CONTROL);
        REQUIRE(controlDown.IsApplied());
        MSG disabled = KeyMessage(WM_KEYDOWN, 'T');
        disabled.hwnd = fixture.host->GetBridgeHwnd();
        REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
            HC_ACTION, PM_REMOVE, &disabled, Modifiers(true),
            TraceXamlPreTranslate, &trace));
        CHECK(disabled.message == WM_NULL);
    }
    CHECK(f6Commands == owners.size());
    CHECK(ctrlTCommands == owners.size());
    CHECK(altMCommands == owners.size());
    CHECK(trace.xamlCalls == 1);

    fixture.Destroy();
}

TEST_CASE("WinUIKeyboard::ComboLocalKeysBeatParentAccelerators",
          "[WinUIKeyboard][keyboard][menu]")
{
    AcceleratorFrame fixture;
    REQUIRE(fixture.host != nullptr);

    wxComboBox * const combo =
        new wxComboBox(fixture.frame, wxID_ANY, "combo",
                       wxPoint(0, 40), wxSize(140, 30));
    wxYield();
    REQUIRE(fixture.host->FindSlot(combo) != nullptr);
    fixture.host->FocusSlot(combo);
    REQUIRE(WaitFor("WinUI keyboard ComboBox focus", [&]()
    {
        return fixture.host->GetFocusOwner() == combo &&
               wxWinUITopLevelHost::ResolveFocusHwnd(
                   fixture.host->GetBridgeHwnd()) == combo;
    }, 1000));

    // Popup() deliberately remains a no-op until the editable peer is fully
    // realized. Keep that production contract out of this keyboard-routing
    // test by waiting through the passive snapshot seam before opening the
    // dropdown. In particular, the read-only six-bit popup readiness mask is
    // insufficient here: this style also requires the current edit part and
    // its shared-root/visual-root topology.
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot popupPeer;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot popupPeerDiagnostic;
    bool popupPeerCaptured = false;
    bool popupPeerDiagnosticCaptured = false;
    const bool popupPeerReady =
        WaitFor("WinUI keyboard ComboBox popup peer readiness", [&]()
    {
        popupPeerCaptured =
            wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(combo, &popupPeer);
        popupPeerDiagnosticCaptured =
            wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(combo,
                &popupPeerDiagnostic);
        return popupPeerCaptured && popupPeerDiagnosticCaptured &&
               popupPeer.state == wxWinUIComboBoxTestAccess::WinUITemplate_Complete &&
               !popupPeerDiagnostic.templateReplayPending;
    }, 1000);
    INFO("popup peer captured=" << popupPeerCaptured << "/" <<
             popupPeerDiagnosticCaptured <<
         " state/phase=" << popupPeer.state << "/" << popupPeer.phase <<
         " replayPending=" << popupPeerDiagnostic.templateReplayPending <<
         " root/combo/edit=" << popupPeer.xamlRootIdentity << "/" <<
             popupPeer.comboIdentity << "/" << popupPeer.editIdentity <<
         " visualRoot/children=" << popupPeer.visualRootIdentity << "/" <<
             popupPeer.visualRootChildCount << "/" <<
             popupPeer.visualChildCount <<
         " actual/desired=" << popupPeer.actualWidth << "x" <<
             popupPeer.actualHeight << "/" << popupPeer.desiredWidth <<
             "x" << popupPeer.desiredHeight <<
         " focus=" << popupPeer.logicalFocus << "/" <<
             popupPeer.nativeFocusInHost << "/" << popupPeer.editFocused);
    REQUIRE(popupPeerCaptured);
    REQUIRE(popupPeerDiagnosticCaptured);
    REQUIRE(popupPeerReady);

    unsigned parentCommands = 0;
    for ( const int id :
          { WinUIKeyboard_F4Id,
            WinUIKeyboard_EscapeId,
            WinUIKeyboard_EnterId } )
    {
        fixture.frame->Bind(
            wxEVT_MENU,
            [&](wxCommandEvent&) { ++parentCommands; },
            id);
    }

    PipelineTrace trace;
    trace.xamlHandles = true;
    {
        KeyboardStateScope noModifiers;
        REQUIRE(noModifiers.IsApplied());

        MSG f4 = KeyMessage(WM_KEYDOWN, VK_F4);
        f4.hwnd = fixture.host->GetBridgeHwnd();
        REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
            HC_ACTION, PM_REMOVE, &f4, Modifiers(),
            TraceXamlPreTranslate, &trace));

        wxWinUIResetComboPopupResolutionDiagnosticForTesting();
        combo->Popup();
        const wxWinUIComboPopupResolutionDiagnosticForTesting popup =
            wxWinUIGetComboPopupResolutionDiagnosticForTesting();
        INFO("popup resolution origin/result=" <<
                 static_cast<unsigned>(popup.origin) << "/" <<
                 static_cast<unsigned>(popup.result) <<
             " failReason/count=" <<
                 static_cast<unsigned>(popup.failCloseReason) << "/" <<
                 popup.failCloseCount <<
             " property/root/queue=" << popup.propertyOpen << "/" <<
                 popup.rootAvailable << "/" << popup.queueAvailable <<
             " session/completion=" << popup.sessionActive << "/" <<
                 popup.completionArmed <<
             " rootMatch/control/popup/child=" << popup.rootMatches << "/" <<
                 popup.controlAvailable << "/" << popup.popupAvailable <<
                 "/" << popup.childAvailable <<
             " hooks/open/openList/duplicate=" << popup.hooksArmed << "/" <<
                 popup.popupIsOpen << "/" << popup.exactOpenList << "/" <<
                 popup.duplicateOpenList);
        REQUIRE(wxWinUIChoiceTestAccess::IsPeerDropDownOpen(combo));
        for ( const WPARAM key : { VK_ESCAPE, VK_RETURN } )
        {
            MSG local = KeyMessage(WM_KEYDOWN, key);
            local.hwnd = fixture.host->GetBridgeHwnd();
            REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
                HC_ACTION, PM_REMOVE, &local, Modifiers(),
                TraceXamlPreTranslate, &trace));
            CHECK(local.message == WM_NULL);
        }
        combo->Dismiss();
        CHECK(f4.message == WM_NULL);
    }

    CHECK(parentCommands == 0);
    CHECK(trace.xamlCalls == 3);
    fixture.Destroy();
}

TEST_CASE("WinUIKeyboard::ComboTeardownRevokesDeferredCallbacks",
          "[WinUIKeyboard][keyboard][lifecycle]")
{
    // Losing focus commits an editable WinUI ComboBox on a dispatcher turn
    // and can raise TextSubmitted. Repeatedly destroy the focused owner before
    // that turn: no callback may reach the already-destructed wxComboBox.
    for ( unsigned i = 0; i < 50; ++i )
    {
        INFO("teardown cycle = " << i);

        wxFrame * const frame =
            new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                        "WinUI ComboBox teardown test");
        wxComboBox * const combo =
            new wxComboBox(frame, wxID_ANY, "pending text",
                           wxPoint(0, 0), wxSize(160, 30),
                           0, nullptr, wxTE_PROCESS_ENTER);
        frame->Show();
        wxYield();

        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(combo, false);
        REQUIRE(host != nullptr);
        host->FocusSlot(combo);
        wxYield();
        REQUIRE(host->GetFocusOwner() == combo);

        wxWeakRef<wxWindow> comboRef(combo);
        frame->Destroy();
        wxYield();
        wxYield();
        CHECK(comboRef.get() == nullptr);
    }
}

TEST_CASE("WinUIKeyboard::PrintableSequenceIsDeliveredOnce",
          "[WinUIKeyboard][keyboard]")
{
    KeyboardStateScope noModifiers;
    REQUIRE(noModifiers.IsApplied());
    AcceleratorFrame fixture;
    REQUIRE(fixture.host != nullptr);

    const MSG messages[] =
    {
        KeyMessage(WM_KEYDOWN, 'A'),
        KeyMessage(WM_CHAR, 'a')
    };

    PipelineTrace hookTrace;
    hookTrace.xamlHandles = true;
    for ( MSG message : messages )
    {
        message.hwnd = fixture.host->GetBridgeHwnd();
        MSG peek = message;
        CHECK(!wxWinUI3ProcessGetMessageHookForTesting(
            HC_ACTION, PM_NOREMOVE, &peek, Modifiers(),
            TraceXamlPreTranslate, &hookTrace));
        CHECK(hookTrace.xamlCalls ==
              (message.message == WM_KEYDOWN ? 0u : 1u));

        REQUIRE(wxWinUI3ProcessGetMessageHookForTesting(
            HC_ACTION, PM_REMOVE, &message, Modifiers(),
            TraceXamlPreTranslate, &hookTrace));
        CHECK(message.message == WM_NULL);

        // The event loop receives the neutralized message, so no second XAML
        // delivery can occur.
        CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
                  &message, Modifiers(),
                  TraceXamlPreTranslate, &hookTrace) ==
              wxWinUIKeyboardPipelineResult::NotApplicable);
    }
    CHECK(hookTrace.xamlCalls == 2);
    CHECK(hookTrace.characterCalls == 1);

    PipelineTrace fallbackTrace;
    fallbackTrace.xamlHandles = true;
    for ( MSG message : messages )
    {
        message.hwnd = fixture.host->GetBridgeHwnd();
        CHECK(wxWinUI3ProcessEventLoopFallbackForTesting(
            &message, Modifiers(),
            TraceXamlPreTranslate, &fallbackTrace));
    }
    CHECK(fallbackTrace.xamlCalls == 2);
    CHECK(fallbackTrace.characterCalls == 1);

    fixture.Destroy();
}

#if wxUSE_UIACTIONSIMULATOR
TEST_CASE("WinUIKeyboard::PhysicalCtrlTAndSingleTextCharacter",
          "[.][WinUIKeyboard][keyboard][menu][physical]")
{
    // The frame owns a handler capturing this counter. Declare the counter
    // first so that it also outlives frame destruction on a fatal assertion.
    unsigned ctrlTCommands = 0;
    AcceleratorFrame fixture;
    wxTextCtrl * const text =
        wxDynamicCast(fixture.owner, wxTextCtrl);
    REQUIRE(text != nullptr);
    REQUIRE(fixture.host != nullptr);

    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++ctrlTCommands; },
        WinUIKeyboard_CtrlTId);

    text->ChangeValue(wxString());
    fixture.frame->Raise();
    fixture.host->FocusSlot(text);
    for ( int i = 0; i < 4; ++i )
        wxYield();
    wxString physicalInputDiagnostic;
    bool physicalInputSafe =
        IsPhysicalInputSafeForTesting(fixture, text,
                                      &physicalInputDiagnostic);
    INFO(physicalInputDiagnostic);
    REQUIRE(physicalInputSafe);

    wxUIActionSimulator simulator;
    REQUIRE(simulator.Char('t', wxMOD_CONTROL));
    wxStopWatch commandWait;
    while ( ctrlTCommands == 0 && commandWait.Time() < 1000 )
    {
        wxYield();
        wxMilliSleep(2);
    }
    wxYield();
    CHECK(ctrlTCommands == 1);
    CHECK(text->GetValue().empty());

    // E has the same virtual-key position on QWERTY and AZERTY, keeping this
    // physical gate independent of the active layout.
    physicalInputSafe =
        IsPhysicalInputSafeForTesting(fixture, text,
                                      &physicalInputDiagnostic);
    INFO(physicalInputDiagnostic);
    REQUIRE(physicalInputSafe);
    REQUIRE(simulator.Char('e'));
    wxStopWatch textWait;
    while ( text->GetValue().empty() && textWait.Time() < 1000 )
    {
        wxYield();
        wxMilliSleep(2);
    }
    wxYield();
    CHECK(ctrlTCommands == 1);
    REQUIRE(text->GetValue().length() == 1);
    CHECK(text->GetValue().CmpNoCase("e") == 0);

    fixture.Destroy();
}
#endif // wxUSE_UIACTIONSIMULATOR

TEST_CASE("WinUIKeyboard::RemovedMessageUsesHwndResolverExactlyOnce",
          "[WinUIKeyboard][keyboard][menu]")
{
    KeyboardStateScope noModifiers;
    REQUIRE(noModifiers.IsApplied());
    AcceleratorFrame fixture;
    REQUIRE(fixture.host != nullptr);
    REQUIRE(fixture.host->GetBridgeHwnd() != nullptr);

    unsigned commandCount = 0;
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++commandCount; },
        WinUIKeyboard_AcceleratorId);

    PipelineTrace trace;
    trace.xamlHandles = true;

    MSG peek = KeyMessage(WM_KEYDOWN, VK_F6);
    peek.hwnd = fixture.host->GetBridgeHwnd();
    CHECK(!wxWinUI3ProcessGetMessageHookForTesting(
        HC_ACTION, PM_NOREMOVE, &peek, Modifiers(),
        TraceXamlPreTranslate, &trace));
    CHECK(peek.message == WM_KEYDOWN);
    CHECK(commandCount == 0);
    CHECK(trace.xamlCalls == 0);

    MSG removed = KeyMessage(WM_KEYDOWN, VK_F6);
    removed.hwnd = fixture.host->GetBridgeHwnd();
    CHECK(wxWinUI3ProcessGetMessageHookForTesting(
        HC_ACTION, PM_REMOVE, &removed, Modifiers(),
        TraceXamlPreTranslate, &trace));
    CHECK(removed.message == WM_NULL);
    CHECK(commandCount == 1);
    CHECK(trace.xamlCalls == 0);

    const bool previousFallback =
        wxWinUI3ForceEventLoopFallbackForTesting(true);
    CHECK(!wxWinUI3IsGetMessageHookActive());
    MSG fallback = KeyMessage(WM_KEYDOWN, VK_F6);
    fallback.hwnd = fixture.host->GetBridgeHwnd();
    CHECK(wxWinUI3PreProcessMessage(&fallback));
    CHECK(commandCount == 2);
    wxWinUI3ForceEventLoopFallbackForTesting(previousFallback);

    // A native wx HWND still gets wx preprocessing in a native nested loop.
    MSG nativeWx = KeyMessage(WM_KEYDOWN, VK_F6);
    nativeWx.hwnd = static_cast<HWND>(fixture.frame->GetHWND());
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &nativeWx, Modifiers(), TraceXamlPreTranslate, &trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(commandCount == 3);
    CHECK(trace.xamlCalls == 0);

    // If wx doesn't handle a native HWND message, XAML is still pumped but
    // its process-global return value cannot prevent native dispatch.
    MSG nativeUnhandled = KeyMessage(WM_KEYDOWN, VK_F7);
    nativeUnhandled.hwnd =
        static_cast<HWND>(fixture.frame->GetHWND());
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &nativeUnhandled, Modifiers(),
              TraceXamlPreTranslate, &trace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    CHECK(trace.xamlCalls == 1);

    // GetParent() reports an owner for WS_POPUP. A truly foreign owned popup
    // must not inherit either the frame accelerator table or its Tab domain.
    HWND const foreignPopup = ::CreateWindowExW(
        WS_EX_TOOLWINDOW,
        L"STATIC",
        L"",
        WS_POPUP,
        0, 0, 1, 1,
        static_cast<HWND>(fixture.frame->GetHWND()),
        nullptr,
        nullptr,
        nullptr);
    REQUIRE(foreignPopup != nullptr);

    MSG foreign = KeyMessage(WM_KEYDOWN, VK_F6);
    foreign.hwnd = foreignPopup;
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &foreign, Modifiers(), TraceXamlPreTranslate, &trace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    CHECK(commandCount == 3);
    CHECK(trace.xamlCalls == 2);

    MSG foreignTab = KeyMessage(WM_KEYDOWN, VK_TAB);
    foreignTab.hwnd = foreignPopup;
    CHECK(!wxWinUI3ProcessTabNavigation(&foreignTab));

    HWND const foreignChild = ::CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_TABSTOP,
        0, 0, 1, 1,
        static_cast<HWND>(fixture.frame->GetHWND()),
        nullptr,
        nullptr,
        nullptr);
    REQUIRE(foreignChild != nullptr);
    foreignTab.hwnd = foreignChild;
    CHECK(!wxWinUI3ProcessTabNavigation(&foreignTab));

    MSG imeVeto = KeyMessage(WM_KEYDOWN, 'K');
    imeVeto.hwnd = static_cast<HWND>(fixture.frame->GetHWND());
    wxWinUI3SetBlockedKeyboardKeyForTesting('K');
    CHECK(wxWinUI3ProcessGetMessageHookForTesting(
        HC_ACTION, PM_REMOVE, &imeVeto, Modifiers(),
        TraceXamlPreTranslate, &trace));
    CHECK(imeVeto.message == WM_NULL);
    CHECK(commandCount == 3);
    CHECK(trace.xamlCalls == 2);

    MSG imeSystemVeto = KeyMessage(WM_SYSKEYDOWN, 'K');
    imeSystemVeto.hwnd = foreignPopup;
    wxWinUI3SetBlockedKeyboardKeyForTesting('K');
    CHECK(wxWinUI3ProcessGetMessageHookForTesting(
        HC_ACTION, PM_REMOVE, &imeSystemVeto, Modifiers(),
        TraceXamlPreTranslate, &trace));
    CHECK(imeSystemVeto.message == WM_NULL);
    CHECK(commandCount == 3);
    CHECK(trace.xamlCalls == 2);

    REQUIRE(::DestroyWindow(foreignChild) != FALSE);
    REQUIRE(::DestroyWindow(foreignPopup) != FALSE);

    fixture.Destroy();
}

TEST_CASE("WinUIKeyboard::CriticalWindowBlocksIslandAndNativeOwners",
          "[WinUIKeyboard][keyboard][menu]")
{
    KeyboardStateScope noModifiers;
    REQUIRE(noModifiers.IsApplied());
    AcceleratorFrame fixture;
    REQUIRE(fixture.host != nullptr);
    REQUIRE(fixture.host->GetBridgeHwnd() != nullptr);

    wxFrame * const critical =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "WinUI keyboard critical window");
    critical->Show();
    wxYield();

    unsigned commandCount = 0;
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++commandCount; },
        WinUIKeyboard_AcceleratorId);

    PipelineTrace trace;
    trace.xamlHandles = true;
    {
        CriticalWindowScope criticalScope(critical);

        MSG island = KeyMessage(WM_KEYDOWN, VK_F6);
        island.hwnd = fixture.host->GetBridgeHwnd();
        CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
                  &island, Modifiers(),
                  TraceXamlPreTranslate, &trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);

        MSG nativeWx = KeyMessage(WM_KEYDOWN, VK_F6);
        nativeWx.hwnd =
            static_cast<HWND>(fixture.frame->GetHWND());
        CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
                  &nativeWx, Modifiers(),
                  TraceXamlPreTranslate, &trace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
    }

    CHECK(commandCount == 0);
    CHECK(trace.xamlCalls == 0);

    // The RAII guard must restore normal processing even if an assertion
    // above fails while unwinding the section.
    MSG restored = KeyMessage(WM_KEYDOWN, VK_F6);
    restored.hwnd = fixture.host->GetBridgeHwnd();
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &restored, Modifiers(),
              TraceXamlPreTranslate, &trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(commandCount == 1);
    CHECK(trace.xamlCalls == 0);

    critical->Destroy();
    wxYield();
    fixture.Destroy();
}

TEST_CASE("WinUIKeyboard::DestroyFromAcceleratorHandler",
          "[WinUIKeyboard][keyboard][menu]")
{
    KeyboardStateScope noModifiers;
    REQUIRE(noModifiers.IsApplied());
    AcceleratorFrame fixture;
    wxWeakRef<wxWindow> ownerAlive(fixture.owner);
    unsigned commandCount = 0;

    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            ++commandCount;
            fixture.owner->Destroy();
            fixture.owner = nullptr;
        },
        WinUIKeyboard_AcceleratorId);

    PipelineTrace trace;
    trace.xamlHandles = true;
    MSG msg = KeyMessage(WM_KEYDOWN, VK_F6);
    msg.hwnd = static_cast<HWND>(fixture.owner->GetHWND());

    CHECK(RunPipeline(msg, fixture.owner, Modifiers(), trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(commandCount == 1);
    CHECK(trace.xamlCalls == 0);
    CHECK(!ownerAlive);

    // The child is deleted synchronously (unlike a TLW queued in
    // wxPendingDelete), proving that the successful translation returns
    // without touching the now-dead logical owner.
    fixture.Destroy();
}

#endif // wxUSE_MENUS && wxUSE_ACCEL

#endif // __WXWINUI__ && wxUSE_WINUI3
