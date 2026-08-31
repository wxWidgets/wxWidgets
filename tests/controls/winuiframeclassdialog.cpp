///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiframeclassdialog.cpp
// Purpose:     Deterministic WinUI frame-class wxDialog contract tests
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_BUTTON && wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/msw/wrapwin.h"
#include "textctrl-test-access.h"
#include "wx/timer.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/winui.h"

namespace
{

const wxPoint OffscreenPosition(-30000, -30000);
const wxSize DialogSize(320, 180);

struct XamlStageTrace
{
    unsigned calls = 0;
    bool handles = false;
};

bool TraceXamlStage(WXMSG *, void *context)
{
    XamlStageTrace& trace = *static_cast<XamlStageTrace *>(context);
    ++trace.calls;
    return trace.handles;
}

wxWinUIKeyboardModifiers Modifiers(bool shift = false)
{
    wxWinUIKeyboardModifiers modifiers = {};
    modifiers.shiftDown = shift;
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
RunDialogPipeline(wxWindow *logicalOwner,
                  MSG& msg,
                  XamlStageTrace& trace)
{
    return wxWinUI3RunKeyboardPipelineForTesting(
        &msg, logicalOwner, Modifiers(), TraceXamlStage, &trace);
}

class ContractDialog : public wxDialog
{
public:
    explicit ContractDialog(wxWindow *parent = wxTheApp->GetTopWindow(),
                            const wxString& title = "Frame-class dialog")
        : wxDialog(parent, wxID_ANY, title,
                   OffscreenPosition, DialogSize)
    {
    }

    bool Validate() override
    {
        ++validateCalls;
        return validationAllowsClose;
    }

    bool TransferDataFromWindow() override
    {
        ++transferCalls;
        return transferAllowsClose;
    }

    bool validationAllowsClose = true;
    bool transferAllowsClose = true;
    unsigned validateCalls = 0;
    unsigned transferCalls = 0;
};

void ShowAndFlush(wxDialog& dialog)
{
    REQUIRE(dialog.Show());
    wxYield();
    REQUIRE(dialog.IsShown());
}

void FocusAndFlush(wxWindow *window)
{
    REQUIRE(window != nullptr);
    window->SetFocus();
    wxYield();
    REQUIRE(wxWindow::FindFocus() == window);
}

} // anonymous namespace

TEST_CASE("WinUIFrameDialog::DefaultAndValidation",
          "[winui-frame-dialog][modal][keyboard]")
{
    ContractDialog dialog;
    wxTextCtrl * const editor =
        new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                       wxPoint(16, 16), wxSize(180, 28));
    wxButton * const ok =
        new wxButton(&dialog, wxID_OK, "&OK",
                     wxPoint(16, 64), wxSize(90, 32));
    ok->SetDefault();

    ShowAndFlush(dialog);
    FocusAndFlush(editor);

    wchar_t className[64] = {};
    REQUIRE(::GetClassNameW(static_cast<HWND>(dialog.GetHWND()),
                            className, WXSIZEOF(className)) != 0);
    CHECK(wxString(className) != "#32770");

    dialog.validationAllowsClose = false;
    XamlStageTrace vetoTrace;
    MSG vetoed = KeyMessage(WM_KEYDOWN, VK_RETURN);
    CHECK(RunDialogPipeline(editor, vetoed, vetoTrace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(vetoTrace.calls == 0);
    CHECK(dialog.validateCalls == 1);
    CHECK(dialog.transferCalls == 0);
    CHECK(dialog.IsShown());

    dialog.validationAllowsClose = true;
    XamlStageTrace acceptedTrace;
    MSG accepted = KeyMessage(WM_KEYDOWN, VK_RETURN);
    CHECK(RunDialogPipeline(editor, accepted, acceptedTrace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(acceptedTrace.calls == 0);
    CHECK(dialog.validateCalls == 2);
    CHECK(dialog.transferCalls == 1);
    CHECK_FALSE(dialog.IsShown());
}

TEST_CASE("WinUIFrameDialog::FocusedButtonBeatsPermanentDefault",
          "[winui-frame-dialog][modal][keyboard]")
{
    ContractDialog dialog;
    wxButton * const defaultButton =
        new wxButton(&dialog, wxID_OK, "&OK",
                     wxPoint(16, 16), wxSize(90, 32));
    wxButton * const focusedButton =
        new wxButton(&dialog, wxID_APPLY, "&Apply",
                     wxPoint(120, 16), wxSize(90, 32));
    defaultButton->SetDefault();

    unsigned defaultClicks = 0;
    unsigned focusedClicks = 0;
    defaultButton->Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++defaultClicks; });
    focusedButton->Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++focusedClicks; });

    ShowAndFlush(dialog);
    FocusAndFlush(focusedButton);

    XamlStageTrace trace;
    MSG enter = KeyMessage(WM_KEYDOWN, VK_RETURN);
    CHECK(RunDialogPipeline(focusedButton, enter, trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(trace.calls == 0);
    CHECK(focusedClicks == 1);
    CHECK(defaultClicks == 0);
    CHECK(dialog.IsShown());
}

TEST_CASE("WinUIFrameDialog::TextEntryOwnsEnter",
          "[winui-frame-dialog][modal][keyboard]")
{
    ContractDialog dialog;
    wxTextCtrl * const multiline =
        new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                       wxPoint(16, 16), wxSize(180, 60),
                       wxTE_MULTILINE);
    wxButton * const ok =
        new wxButton(&dialog, wxID_OK, "&OK",
                     wxPoint(16, 92), wxSize(90, 32));
    ok->SetDefault();

    unsigned okClicks = 0;
    ok->Bind(wxEVT_BUTTON,
             [&](wxCommandEvent&) { ++okClicks; });

    ShowAndFlush(dialog);
    FocusAndFlush(multiline);

    XamlStageTrace trace;
    trace.handles = true;
    MSG enter = KeyMessage(WM_KEYDOWN, VK_RETURN);
    CHECK(RunDialogPipeline(multiline, enter, trace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(trace.calls == 1);
    CHECK(okClicks == 0);
    CHECK(dialog.IsShown());
}

TEST_CASE("WinUIFrameDialog::ProcessEnterFallback",
          "[winui-frame-dialog][modal][keyboard]")
{
    SECTION("single-line skipped event activates default")
    {
        ContractDialog dialog;
        wxTextCtrl * const editor =
            new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                           wxPoint(16, 16), wxSize(180, 28),
                           wxTE_PROCESS_ENTER);
        wxButton * const ok =
            new wxButton(&dialog, wxID_OK, "&OK",
                         wxPoint(16, 64), wxSize(90, 32));
        ok->SetDefault();

        unsigned textEvents = 0;
        editor->Bind(
            wxEVT_TEXT_ENTER,
            [&](wxCommandEvent& event)
            {
                ++textEvents;
                event.Skip();
            });

        ShowAndFlush(dialog);
        CHECK(wxWinUITextCtrlTestAccess::ProcessEnter(*editor));
        CHECK(textEvents == 1);
        CHECK_FALSE(dialog.IsShown());
    }

    SECTION("handled event does not activate default")
    {
        ContractDialog dialog;
        wxTextCtrl * const editor =
            new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                           wxPoint(16, 16), wxSize(180, 28),
                           wxTE_PROCESS_ENTER);
        wxButton * const ok =
            new wxButton(&dialog, wxID_OK, "&OK",
                         wxPoint(16, 64), wxSize(90, 32));
        ok->SetDefault();
        editor->Bind(wxEVT_TEXT_ENTER, [](wxCommandEvent&) {});

        ShowAndFlush(dialog);
        CHECK(wxWinUITextCtrlTestAccess::ProcessEnter(*editor));
        CHECK(dialog.IsShown());
    }

    SECTION("multiline skipped event keeps Return")
    {
        ContractDialog dialog;
        wxTextCtrl * const editor =
            new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                           wxPoint(16, 16), wxSize(180, 60),
                           wxTE_PROCESS_ENTER | wxTE_MULTILINE);
        wxButton * const ok =
            new wxButton(&dialog, wxID_OK, "&OK",
                         wxPoint(16, 92), wxSize(90, 32));
        ok->SetDefault();
        editor->Bind(
            wxEVT_TEXT_ENTER,
            [](wxCommandEvent& event) { event.Skip(); });

        ShowAndFlush(dialog);
        CHECK_FALSE(wxWinUITextCtrlTestAccess::ProcessEnter(*editor));
        CHECK(dialog.IsShown());
    }
}

TEST_CASE("WinUIFrameDialog::EscapeContract",
          "[winui-frame-dialog][modal][keyboard]")
{
    ContractDialog dialog;
    wxTextCtrl * const editor =
        new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                       wxPoint(16, 16), wxSize(180, 28));
    new wxButton(&dialog, wxID_CANCEL, "&Cancel",
                 wxPoint(16, 64), wxSize(90, 32));

    ShowAndFlush(dialog);
    FocusAndFlush(editor);

    unsigned hookEvents = 0;
    dialog.Bind(
        wxEVT_CHAR_HOOK,
        [&](wxKeyEvent& event)
        {
            ++hookEvents;
            event.Skip();
        });
    dialog.SetEscapeId(wxID_NONE);
    wxKeyEvent hook(wxEVT_CHAR_HOOK);
    hook.SetEventObject(&dialog);
    hook.m_keyCode = WXK_ESCAPE;
    hook.m_uniChar = WXK_ESCAPE;
    dialog.ProcessWindowEvent(hook);
    REQUIRE(hookEvents == 1);

    XamlStageTrace ignoredTrace;
    ignoredTrace.handles = true;
    MSG ignored = KeyMessage(WM_KEYDOWN, VK_ESCAPE);
    CHECK(RunDialogPipeline(editor, ignored, ignoredTrace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(ignoredTrace.calls == 1);
    CHECK(hookEvents == 1);
    CHECK(dialog.IsShown());

    dialog.SetEscapeId(wxID_ANY);
    XamlStageTrace closeTrace;
    MSG close = KeyMessage(WM_KEYDOWN, VK_ESCAPE);
    CHECK(RunDialogPipeline(editor, close, closeTrace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(closeTrace.calls == 0);
    CHECK(hookEvents == 1);
    CHECK_FALSE(dialog.IsShown());
    CHECK(dialog.validateCalls == 0);
    CHECK(dialog.transferCalls == 0);
}

TEST_CASE("WinUIFrameDialog::Mnemonic",
          "[winui-frame-dialog][modal][keyboard]")
{
    ContractDialog dialog;
    wxTextCtrl * const editor =
        new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                       wxPoint(16, 16), wxSize(180, 28));
    wxButton * const apply =
        new wxButton(&dialog, wxID_APPLY, "&Apply",
                     wxPoint(16, 64), wxSize(90, 32));
    new wxButton(&dialog, wxID_ANY, "Save && Exit",
                 wxPoint(120, 64), wxSize(120, 32));

    unsigned applyClicks = 0;
    apply->Bind(wxEVT_BUTTON,
                [&](wxCommandEvent&) { ++applyClicks; });

    ShowAndFlush(dialog);
    FocusAndFlush(editor);

    XamlStageTrace trace;
    MSG mnemonic = KeyMessage(WM_SYSCHAR, 'a');
    CHECK(RunDialogPipeline(editor, mnemonic, trace) ==
          wxWinUIKeyboardPipelineResult::WxHandled);
    CHECK(trace.calls == 0);
    CHECK(applyClicks == 1);

    trace.handles = true;
    MSG escapedAmpersand = KeyMessage(WM_SYSCHAR, 'e');
    CHECK(RunDialogPipeline(editor, escapedAmpersand, trace) ==
          wxWinUIKeyboardPipelineResult::XamlHandled);
    CHECK(trace.calls == 1);
    CHECK(applyClicks == 1);
}

TEST_CASE("WinUIFrameDialog::TabAndShiftTab",
          "[winui-frame-dialog][modal][keyboard][focus]")
{
    ContractDialog dialog;
    wxButton * const first =
        new wxButton(&dialog, wxID_ANY, "&First",
                     wxPoint(16, 16), wxSize(90, 32));
    wxButton * const second =
        new wxButton(&dialog, wxID_ANY, "&Second",
                     wxPoint(120, 16), wxSize(90, 32));

    ShowAndFlush(dialog);
    FocusAndFlush(first);

    MSG forward = KeyMessage(WM_KEYDOWN, VK_TAB);
    forward.hwnd = ::GetFocus();
    REQUIRE(forward.hwnd != nullptr);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &forward, false, false, false));
    wxYield();
    CHECK(wxWindow::FindFocus() == second);

    MSG backward = KeyMessage(WM_KEYDOWN, VK_TAB);
    backward.hwnd = ::GetFocus();
    REQUIRE(backward.hwnd != nullptr);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &backward, true, false, false));
    wxYield();
    CHECK(wxWindow::FindFocus() == first);
}

TEST_CASE("WinUIFrameDialog::InitialAndRestoredFocus",
          "[winui-frame-dialog][modal][keyboard][focus]"
          "[winui-beta-dialogs][winui-beta-transients]")
{
    wxFrame * const owner =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "Frame-class dialog owner",
                    OffscreenPosition, wxSize(360, 220));
    wxButton * const ownerFocus =
        new wxButton(owner, wxID_ANY, "Owner focus",
                     wxPoint(16, 16), wxSize(110, 32));
    owner->Show();
    wxYield();
    FocusAndFlush(ownerFocus);

    {
        ContractDialog dialog(owner);
        wxTextCtrl * const initial =
            new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                           wxPoint(16, 16), wxSize(180, 28));
        new wxButton(&dialog, wxID_OK, "&OK",
                     wxPoint(16, 64), wxSize(90, 32));
        dialog.Bind(
            wxEVT_INIT_DIALOG,
            [initial](wxInitDialogEvent& event)
            {
                initial->SetFocus();
                event.Skip();
            });

        bool sawInitialFocus = false;
        unsigned focusPolls = 0;
        wxEvtHandler focusPoller;
        wxTimer focusTimer(&focusPoller);
        focusPoller.Bind(
            wxEVT_TIMER,
            [&](wxTimerEvent&)
            {
                sawInitialFocus = wxWindow::FindFocus() == initial;
                if ( sawInitialFocus || ++focusPolls == 50 )
                {
                    focusTimer.Stop();
                    dialog.EndModal(wxID_OK);
                }
            });
        REQUIRE(focusTimer.Start(10));

        CHECK(dialog.ShowModal() == wxID_OK);
        focusTimer.Stop();
        wxYield();
        CHECK(sawInitialFocus);
        CHECK(wxWindow::FindFocus() == ownerFocus);
    }

    owner->Destroy();
    wxYield();
}

TEST_CASE("WinUIFrameDialog::NestedModalFocus",
          "[winui-frame-dialog][modal][keyboard][focus]"
          "[winui-beta-dialogs][winui-beta-transients]")
{
    wxFrame * const owner =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "Nested frame-class dialog owner",
                    OffscreenPosition, wxSize(360, 220));
    wxButton * const ownerFocus =
        new wxButton(owner, wxID_ANY, "Owner focus",
                     wxPoint(16, 16), wxSize(110, 32));
    owner->Show();
    wxYield();
    FocusAndFlush(ownerFocus);

    {
        ContractDialog outer(owner, "Outer frame-class dialog");
        wxButton * const outerFocus =
            new wxButton(&outer, wxID_ANY, "Outer focus",
                         wxPoint(16, 16), wxSize(110, 32));
        outer.Bind(
            wxEVT_INIT_DIALOG,
            [outerFocus](wxInitDialogEvent& event)
            {
                outerFocus->SetFocus();
                event.Skip();
            });

        int innerResult = wxID_NONE;
        bool outerHadInitialFocus = false;
        bool innerHadInitialFocus = false;
        bool outerFocusWasRestored = false;
        unsigned outerFocusPolls = 0;
        wxEvtHandler outerFocusPoller;
        wxTimer outerFocusTimer(&outerFocusPoller);
        outerFocusPoller.Bind(
            wxEVT_TIMER,
            [&](wxTimerEvent&)
            {
                outerHadInitialFocus =
                    wxWindow::FindFocus() == outerFocus;
                if ( !outerHadInitialFocus &&
                     ++outerFocusPolls < 50 )
                {
                    return;
                }
                outerFocusTimer.Stop();

                ContractDialog inner(&outer, "Inner frame-class dialog");
                wxButton * const innerFocus =
                    new wxButton(&inner, wxID_ANY, "Inner focus",
                                 wxPoint(16, 16), wxSize(110, 32));
                inner.Bind(
                    wxEVT_INIT_DIALOG,
                    [innerFocus](wxInitDialogEvent& event)
                    {
                        innerFocus->SetFocus();
                        event.Skip();
                    });

                unsigned innerFocusPolls = 0;
                wxEvtHandler innerFocusPoller;
                wxTimer innerFocusTimer(&innerFocusPoller);
                innerFocusPoller.Bind(
                    wxEVT_TIMER,
                    [&](wxTimerEvent&)
                    {
                        innerHadInitialFocus =
                            wxWindow::FindFocus() == innerFocus;
                        if ( innerHadInitialFocus ||
                             ++innerFocusPolls == 50 )
                        {
                            innerFocusTimer.Stop();
                            inner.EndModal(wxID_OK);
                        }
                    });
                REQUIRE(innerFocusTimer.Start(10));

                innerResult = inner.ShowModal();
                innerFocusTimer.Stop();
                wxYield();
                outerFocusWasRestored =
                    wxWindow::FindFocus() == outerFocus;
                outer.EndModal(wxID_OK);
            });
        REQUIRE(outerFocusTimer.Start(10));

        CHECK(outer.ShowModal() == wxID_OK);
        outerFocusTimer.Stop();
        wxYield();
        CHECK(innerResult == wxID_OK);
        CHECK(outerHadInitialFocus);
        CHECK(innerHadInitialFocus);
        CHECK(outerFocusWasRestored);
        CHECK(wxWindow::FindFocus() == ownerFocus);
    }

    owner->Destroy();
    wxYield();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_BUTTON && wxUSE_TEXTCTRL
