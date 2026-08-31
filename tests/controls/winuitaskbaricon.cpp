///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitaskbaricon.cpp
// Purpose:     WinUI wxTaskBarIcon shell and lifetime contract tests
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TASKBARICON

#include "wx/app.h"
#include "wx/bmpbndl.h"
#include "wx/dialog.h"
#include "wx/menu.h"
#include "wx/taskbar.h"
#include "wx/weakref.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapshl.h"
#include "wx/winui/private/taskbaricon.h"

#include <algorithm>
#include <deque>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{

struct NativeResult
{
    unsigned message;
    bool result;
};

struct NativePayload
{
    unsigned message = 0;
    unsigned flags = 0;
    wxString tooltip;
    wxString info;
    wxString infoTitle;
    bool iconAdvertised = false;
    bool iconValidBeforeReentrance = false;
    bool iconValidAfterReentrance = false;
    bool balloonIconAdvertised = false;
    bool balloonIconValidBeforeReentrance = false;
    bool balloonIconValidAfterReentrance = false;
};

struct TaskBarNativeScript
{
    std::vector<unsigned> calls;
    std::vector<NativePayload> payloads;
    std::deque<NativeResult> results;
    wxTaskBarIcon *removeOnAdd = nullptr;
    wxTaskBarIcon *destroyHelperOnAdd = nullptr;
    wxTaskBarIcon *destroyOwnerOnAdd = nullptr;
    wxTaskBarIcon *rewriteOnAdd = nullptr;
    wxTaskBarIcon *rewriteOnEveryAdd = nullptr;
    wxTaskBarIcon *rewriteOnModify = nullptr;
    wxTaskBarIcon *removeOnModify = nullptr;
    wxTaskBarIcon *recreateHelperOnModify = nullptr;
    wxTaskBarIcon *destroyOwnerOnModify = nullptr;
    bool dispatchRestartOnModify = false;
    unsigned rewriteCalls = 0;
    std::vector<bool> nestedRemoveResults;
    std::vector<bool> nestedSetResults;
    std::vector<bool> nestedHelperDestroyResults;
};

TaskBarNativeScript *gs_nativeScript = nullptr;

wxBitmapBundle MakeTaskBarTestIcon()
{
    return wxBitmapBundle::FromBitmap(wxBitmap(16, 16));
}

bool CanInspectAndCopyIcon(HICON icon)
{
    if ( !icon )
        return false;

    AutoIconInfo originalInfo;
    if ( !originalInfo.GetFrom(icon) )
        return false;

    const HICON copy = ::CopyIcon(icon);
    if ( !copy )
        return false;

    AutoIconInfo copiedInfo;
    const bool copiedIconIsValid = copiedInfo.GetFrom(copy);
    CHECK(::DestroyIcon(copy) != 0);
    return copiedIconIsValid;
}

bool TaskBarShellHook(unsigned message, const void *data)
{
    CHECK(gs_nativeScript != nullptr);
    if ( !gs_nativeScript )
        return false;

    CHECK(data != nullptr);
    if ( !data )
        return false;

    const NOTIFYICONDATA * const notifyData =
        static_cast<const NOTIFYICONDATA *>(data);
    CHECK(notifyData->hWnd != nullptr);
    CHECK(notifyData->uID == 99);
    if ( message == NIM_SETVERSION )
        CHECK(notifyData->uVersion == 3);
    gs_nativeScript->calls.push_back(message);

    NativePayload payload;
    payload.message = message;
    payload.flags = notifyData->uFlags;
    payload.tooltip = notifyData->szTip;
    payload.info = notifyData->szInfo;
    payload.infoTitle = notifyData->szInfoTitle;
    payload.iconAdvertised =
        (notifyData->uFlags & NIF_ICON) != 0 && notifyData->hIcon;
    payload.balloonIconAdvertised =
        (notifyData->uFlags & NIF_INFO) != 0 &&
        notifyData->hBalloonIcon;

    const HICON iconHandle = payload.iconAdvertised
                                 ? notifyData->hIcon
                                 : nullptr;
    const HICON balloonIconHandle = payload.balloonIconAdvertised
                                        ? notifyData->hBalloonIcon
                                        : nullptr;
    payload.iconValidBeforeReentrance =
        iconHandle && CanInspectAndCopyIcon(iconHandle);
    payload.balloonIconValidBeforeReentrance =
        balloonIconHandle && CanInspectAndCopyIcon(balloonIconHandle);
    const size_t payloadIndex = gs_nativeScript->payloads.size();
    gs_nativeScript->payloads.push_back(payload);

    // Consume the result for this exact native invocation before executing a
    // reentrant action. A nested call with the same message must not steal the
    // outer call's scripted return value.
    bool result = true;
    if ( !gs_nativeScript->results.empty() &&
            gs_nativeScript->results.front().message == message )
    {
        result = gs_nativeScript->results.front().result;
        gs_nativeScript->results.pop_front();
    }

    if ( message == NIM_ADD && gs_nativeScript->removeOnAdd )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->removeOnAdd;
        gs_nativeScript->removeOnAdd = nullptr;
        gs_nativeScript->nestedRemoveResults.push_back(icon->RemoveIcon());
    }

    if ( message == NIM_ADD && gs_nativeScript->destroyHelperOnAdd )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->destroyHelperOnAdd;
        gs_nativeScript->destroyHelperOnAdd = nullptr;
        gs_nativeScript->nestedHelperDestroyResults.push_back(
            wxWinUITaskBarIconDestroyHelperForTesting(icon));
    }

    if ( message == NIM_ADD && gs_nativeScript->destroyOwnerOnAdd )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->destroyOwnerOnAdd;
        gs_nativeScript->destroyOwnerOnAdd = nullptr;
        delete icon;
    }

    if ( message == NIM_ADD && gs_nativeScript->rewriteOnAdd )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->rewriteOnAdd;
        gs_nativeScript->rewriteOnAdd = nullptr;
        gs_nativeScript->nestedSetResults.push_back(
            icon->SetIcon(MakeTaskBarTestIcon(), "nested replacement"));
    }

    if ( message == NIM_ADD && gs_nativeScript->rewriteOnEveryAdd )
    {
        ++gs_nativeScript->rewriteCalls;
        gs_nativeScript->nestedSetResults.push_back(
            gs_nativeScript->rewriteOnEveryAdd->SetIcon(
                MakeTaskBarTestIcon(),
                wxString::Format("reentrant-%u",
                                 gs_nativeScript->rewriteCalls)));
    }

    if ( message == NIM_MODIFY && gs_nativeScript->rewriteOnModify )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->rewriteOnModify;
        gs_nativeScript->rewriteOnModify = nullptr;
        gs_nativeScript->nestedSetResults.push_back(
            icon->SetIcon(MakeTaskBarTestIcon(), "nested rewrite"));
    }

    if ( message == NIM_MODIFY && gs_nativeScript->removeOnModify )
    {
        wxTaskBarIcon * const icon = gs_nativeScript->removeOnModify;
        gs_nativeScript->removeOnModify = nullptr;
        gs_nativeScript->nestedRemoveResults.push_back(icon->RemoveIcon());
    }

    if ( message == NIM_MODIFY &&
            gs_nativeScript->recreateHelperOnModify )
    {
        wxTaskBarIcon * const icon =
            gs_nativeScript->recreateHelperOnModify;
        gs_nativeScript->recreateHelperOnModify = nullptr;
        gs_nativeScript->nestedHelperDestroyResults.push_back(
            wxWinUITaskBarIconDestroyHelperForTesting(icon));
        gs_nativeScript->nestedSetResults.push_back(
            icon->SetIcon(MakeTaskBarTestIcon(), "replacement helper"));
    }

    if ( message == NIM_MODIFY &&
            gs_nativeScript->dispatchRestartOnModify )
    {
        gs_nativeScript->dispatchRestartOnModify = false;
        const UINT taskbarCreated =
            ::RegisterWindowMessage(wxT("TaskbarCreated"));
        CHECK(taskbarCreated != 0);
        if ( taskbarCreated )
            (void)::SendMessage(notifyData->hWnd, taskbarCreated, 0, 0);
    }

    if ( message == NIM_MODIFY &&
            gs_nativeScript->destroyOwnerOnModify )
    {
        wxTaskBarIcon * const icon =
            gs_nativeScript->destroyOwnerOnModify;
        gs_nativeScript->destroyOwnerOnModify = nullptr;
        delete icon;
    }

    // All native calls are synchronous. The stack-local wxIcon leases in the
    // production code must keep every advertised handle inspectable even if a
    // nested action destroyed its public owner or hidden helper above.
    gs_nativeScript->payloads[payloadIndex].iconValidAfterReentrance =
        iconHandle && CanInspectAndCopyIcon(iconHandle);
    gs_nativeScript->payloads[payloadIndex]
        .balloonIconValidAfterReentrance =
            balloonIconHandle && CanInspectAndCopyIcon(balloonIconHandle);

    return result;
}

class TaskBarShellHookScope final
{
public:
    explicit TaskBarShellHookScope(TaskBarNativeScript& script)
    {
        REQUIRE(gs_nativeScript == nullptr);
        gs_nativeScript = &script;
        wxWinUISetTaskBarIconShellNotifyHookForTesting(&TaskBarShellHook);
    }

    ~TaskBarShellHookScope()
    {
        wxWinUIResetTaskBarIconShellNotifyHookForTesting();
        gs_nativeScript = nullptr;
    }
};

void DrainTaskBarDeletes(unsigned rounds = 16)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( wxTheApp )
        {
            wxTheApp->ProcessPendingEvents();
            (void)wxTheApp->ProcessIdle();
        }
    }
}

wxWinUITaskBarIconSnapshotForTesting Snapshot(wxTaskBarIcon *icon)
{
    wxWinUITaskBarIconSnapshotForTesting snapshot;
    REQUIRE(wxWinUIGetTaskBarIconSnapshotForTesting(icon, &snapshot));
    return snapshot;
}

void CheckCalls(const std::vector<unsigned>& actual,
                 std::initializer_list<unsigned> expected)
{
    CHECK(actual == std::vector<unsigned>(expected));
}

void ClearNativeTrace(TaskBarNativeScript& script)
{
    script.calls.clear();
    script.payloads.clear();
}

constexpr int TaskBarCommandId = wxID_HIGHEST + 830;

class TaskBarMenuProbe final : public wxTaskBarIcon
{
public:
    TaskBarMenuProbe()
    {
        Bind(wxEVT_MENU, &TaskBarMenuProbe::OnCommand,
             this, TaskBarCommandId);
        Bind(wxEVT_UPDATE_UI, &TaskBarMenuProbe::OnUpdateUI,
             this, TaskBarCommandId);
    }

    unsigned commands = 0;
    unsigned updates = 0;
    bool nestedPopupRejected = false;
    bool launchDialog = false;
    bool dialogLaunched = false;
    bool destroyOnCommand = false;
    wxMenu *activeMenu = nullptr;

private:
    void OnCommand(wxCommandEvent&)
    {
        ++commands;
        if ( activeMenu )
            nestedPopupRejected = !PopupMenu(activeMenu);

        if ( launchDialog )
        {
            wxDialog * const dialog =
                new wxDialog(nullptr, wxID_ANY, "taskbar command dialog");
            dialogLaunched = dialog->GetHandle() != nullptr;
            dialog->Destroy();
        }

        if ( destroyOnCommand )
            Destroy();
    }

    void OnUpdateUI(wxUpdateUIEvent& event)
    {
        ++updates;
        event.Enable(true);
    }
};

struct TaskBarDispatchObservation
{
    unsigned events = 0;
    bool destructionAccepted = false;
};

class TaskBarDispatchProbe final : public wxTaskBarIcon
{
public:
    enum class Destruction
    {
        Owner,
        Helper
    };

    TaskBarDispatchProbe(Destruction destruction,
                         TaskBarDispatchObservation *observation)
        : m_destruction(destruction), m_observation(observation)
    {
        Bind(wxEVT_TASKBAR_LEFT_UP,
             &TaskBarDispatchProbe::OnLeftUp, this);
    }

private:
    void OnLeftUp(wxTaskBarIconEvent&)
    {
        ++m_observation->events;
        if ( m_destruction == Destruction::Owner )
        {
            // wxTaskBarIconBase::Destroy() is deliberately delayed: the
            // production callback and its hidden wxFrame must first unwind.
            m_observation->destructionAccepted = true;
            Destroy();
            return;
        }

        m_observation->destructionAccepted =
            wxWinUITaskBarIconDestroyHelperForTesting(this);
    }

    const Destruction m_destruction;
    TaskBarDispatchObservation * const m_observation;
};

bool DispatchTaskBarPopup(wxWindow *helper, wxMenu *menu)
{
    REQUIRE(helper != nullptr);
    REQUIRE(menu != nullptr);

    wxMenuInvokingWindowSetter invokingWindow(*menu, helper);
    menu->UpdateUI();

    wxCommandEvent command(wxEVT_MENU, TaskBarCommandId);
    command.SetEventObject(menu);
    return helper->ProcessWindowEvent(command);
}

#if wxUSE_EXCEPTIONS
bool ThrowFromTaskBarPopup(wxWindow *, wxMenu *)
{
    throw std::runtime_error("taskbar popup seam");
}
#endif

class TaskBarPopupHookScope final
{
public:
    explicit TaskBarPopupHookScope(
        wxWinUITaskBarIconPopupHookForTesting hook)
    {
        wxWinUISetTaskBarIconPopupHookForTesting(hook);
    }

    ~TaskBarPopupHookScope()
    {
        wxWinUIResetTaskBarIconPopupHookForTesting();
    }
};

} // anonymous namespace

TEST_CASE("WinUITaskBarIcon::DesiredAndAppliedLifecycle",
          "[winui-beta-aux][taskbaricon]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);

    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore + 1);
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "first"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    REQUIRE(script.payloads.size() == 2);
    const NativePayload& addPayload = script.payloads[0];
    CHECK((addPayload.flags & (NIF_MESSAGE | NIF_ICON | NIF_TIP)) ==
          (NIF_MESSAGE | NIF_ICON | NIF_TIP));
    CHECK((addPayload.flags & NIF_INFO) == 0);
    CHECK(addPayload.tooltip == "first");
    CHECK(addPayload.iconAdvertised);
    CHECK(addPayload.iconValidBeforeReentrance);
    CHECK(addPayload.iconValidAfterReentrance);
    CHECK(script.payloads[1].flags == 0);

    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK(snapshot.appliedExplorerGeneration == snapshot.explorerGeneration);
    CHECK(icon->IsIconInstalled());

    ClearNativeTrace(script);
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "second"));
    CheckCalls(script.calls, { NIM_MODIFY, NIM_SETVERSION });

    ClearNativeTrace(script);
    REQUIRE(icon->RemoveIcon());
    CheckCalls(script.calls, { NIM_DELETE });
    snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Absent);
    CHECK_FALSE(snapshot.versionApplied);
    CHECK_FALSE(icon->IsIconInstalled());

    ClearNativeTrace(script);
    CHECK_FALSE(icon->RemoveIcon());
    CHECK(script.calls.empty());
    delete icon;
    DrainTaskBarDeletes();
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::ExplorerRestartNeverResurrectsRemovedIntent",
          "[winui-beta-aux][taskbaricon][restart]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "restart"));
    REQUIRE(icon->RemoveIcon());
    const auto beforeRestart = Snapshot(icon);
    ClearNativeTrace(script);

    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CHECK(script.calls.empty());
    CHECK(std::find(script.calls.begin(), script.calls.end(),
                    static_cast<unsigned>(NIM_ADD)) ==
          script.calls.end());
    CHECK(std::find(script.calls.begin(), script.calls.end(),
                    static_cast<unsigned>(NIM_MODIFY)) ==
          script.calls.end());
    const auto removedRestart = Snapshot(icon);
    CHECK_FALSE(removedRestart.desiredInstalled);
    CHECK(removedRestart.applied ==
          wxWinUITaskBarIconAppliedState::Absent);
    CHECK(removedRestart.explorerGeneration !=
          beforeRestart.explorerGeneration);

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "present again"));
    ClearNativeTrace(script);
    script.results.push_back({ NIM_ADD, true });
    script.results.push_back({ NIM_SETVERSION, true });
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    auto presentRestart = Snapshot(icon);
    CHECK(presentRestart.desiredInstalled);
    CHECK(presentRestart.applied ==
          wxWinUITaskBarIconAppliedState::Present);
    CHECK(presentRestart.versionApplied);

    // Explorer can also broadcast TaskbarCreated as a refresh while the old
    // entry remains: ADD then legitimately fails and MODIFY must be attempted.
    ClearNativeTrace(script);
    script.results.push_back({ NIM_ADD, false });
    script.results.push_back({ NIM_MODIFY, true });
    script.results.push_back({ NIM_SETVERSION, true });
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CheckCalls(script.calls,
               { NIM_ADD, NIM_MODIFY, NIM_SETVERSION });
    presentRestart = Snapshot(icon);
    CHECK(presentRestart.desiredInstalled);
    CHECK(presentRestart.applied ==
          wxWinUITaskBarIconAppliedState::Present);
    CHECK(presentRestart.versionApplied);

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::ExplorerRestartHonoursReentrantIntent",
          "[winui-beta-aux][taskbaricon][restart][reentrancy]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon *icon = new wxTaskBarIcon;

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "before restart remove"));
    ClearNativeTrace(script);
    script.removeOnAdd = icon;
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CheckCalls(script.calls, { NIM_ADD, NIM_DELETE });
    REQUIRE(script.nestedRemoveResults.size() == 1);
    CHECK_FALSE(script.nestedRemoveResults[0]);
    auto snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Absent);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK_FALSE(icon->IsIconInstalled());

    ClearNativeTrace(script);
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CHECK(script.calls.empty());
    delete icon;
    DrainTaskBarDeletes();

    // Now destroy the owner from the ADD seam entered by the real helper
    // MSWWindowProc. The callback lease must keep the helper alive until
    // SendMessage() unwinds and must not republish the stale restart result.
    ClearNativeTrace(script);
    icon = new wxTaskBarIcon;
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "before restart destroy"));
    ClearNativeTrace(script);
    const wxWeakRef<wxEvtHandler> weakIcon(icon);
    script.destroyOwnerOnAdd = icon;
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CheckCalls(script.calls, { NIM_ADD, NIM_DELETE });
    CHECK_FALSE(weakIcon);
    REQUIRE(script.payloads.size() == 2);
    CHECK(script.payloads[0].iconValidBeforeReentrance);
    CHECK(script.payloads[0].iconValidAfterReentrance);

    DrainTaskBarDeletes(32);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::HelperCanDieBeforeItsOwnerAndBeRecreated",
          "[winui-beta-aux][taskbaricon][lifetime]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "first helper"));
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() ==
          helpersBefore + 1);

    // wxTaskBarIconWindow is itself a TLW. In a real shutdown
    // wxApp::DeleteAllTLWs() can delete it while the non-window owner remains
    // alive, so exercise that exact ordering synchronously here.
    ClearNativeTrace(script);
    REQUIRE(wxWinUITaskBarIconDestroyHelperForTesting(icon));
    CheckCalls(script.calls, { NIM_DELETE });
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore + 1);

    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(snapshot.versionApplied);
    CHECK_FALSE(icon->IsIconInstalled());
    wxMenu unavailableMenu;
    CHECK_FALSE(icon->PopupMenu(&unavailableMenu));

    ClearNativeTrace(script);
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "replacement helper"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() ==
          helpersBefore + 1);
    snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);

    ClearNativeTrace(script);
    delete icon;
    CheckCalls(script.calls, { NIM_DELETE });
    DrainTaskBarDeletes();
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::HelperWindowProcDefersCallbackDestruction",
          "[winui-beta-aux][taskbaricon][lifetime][native-message]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);

    TaskBarDispatchObservation ownerObservation;
    TaskBarDispatchProbe *ownerProbe = new TaskBarDispatchProbe(
        TaskBarDispatchProbe::Destruction::Owner, &ownerObservation);
    const wxWeakRef<wxEvtHandler> weakOwner(ownerProbe);
    REQUIRE(ownerProbe->SetIcon(MakeTaskBarTestIcon(), "owner callback"));
    ClearNativeTrace(script);

    REQUIRE(wxWinUITaskBarIconDispatchEventForTesting(
        ownerProbe, WM_LBUTTONUP));
    CHECK(ownerObservation.events == 1);
    CHECK(ownerObservation.destructionAccepted);
    DrainTaskBarDeletes(32);
    CHECK_FALSE(weakOwner);
    CheckCalls(script.calls, { NIM_DELETE });
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);

    ClearNativeTrace(script);
    TaskBarDispatchObservation helperObservation;
    TaskBarDispatchProbe * const helperProbe = new TaskBarDispatchProbe(
        TaskBarDispatchProbe::Destruction::Helper, &helperObservation);
    REQUIRE(helperProbe->SetIcon(MakeTaskBarTestIcon(), "helper callback"));
    ClearNativeTrace(script);

    REQUIRE(wxWinUITaskBarIconDispatchEventForTesting(
        helperProbe, WM_LBUTTONUP));
    CHECK(helperObservation.events == 1);
    CHECK(helperObservation.destructionAccepted);
    // The helper C++ object is deliberately still alive until its member
    // callback returns; draining then performs the requested destruction.
    DrainTaskBarDeletes(32);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
    CheckCalls(script.calls, { NIM_DELETE });
    auto snapshot = Snapshot(helperProbe);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(helperProbe->IsIconInstalled());

    ClearNativeTrace(script);
    REQUIRE(helperProbe->SetIcon(
        MakeTaskBarTestIcon(), "helper recreated after callback"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    CHECK(helperProbe->IsIconInstalled());

    delete helperProbe;
    DrainTaskBarDeletes(32);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::ReconciliationIsStrictlyBounded",
          "[winui-beta-aux][taskbaricon][reentrancy][bounded]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    script.rewriteOnEveryAdd = icon;

    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "outer"));
    CHECK(script.rewriteCalls == 16);
    CHECK(script.calls.size() == 16);
    CHECK(script.nestedSetResults.size() == 16);
    CHECK(std::all_of(script.nestedSetResults.begin(),
                      script.nestedSetResults.end(),
                      [](bool result) { return !result; }));
    CHECK(std::all_of(script.calls.begin(), script.calls.end(),
                      [](unsigned message)
                      {
                          return message == static_cast<unsigned>(NIM_ADD);
                      }));
    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(snapshot.reconciling);

    script.rewriteOnEveryAdd = nullptr;
    script.calls.clear();
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "stable"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    snapshot = Snapshot(icon);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::HelperLossInvalidatesAnInFlightNativeCall",
          "[winui-beta-aux][taskbaricon][lifetime][reentrancy]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    script.destroyHelperOnAdd = icon;

    // The shell seam returns success, but the exact helper generation ceased
    // to exist during the call. It must therefore never be published as the
    // applied state and the bounded reconcile must stop without touching the
    // freed frame.
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "lost in add"));
    CheckCalls(script.calls, { NIM_ADD, NIM_DELETE });
    REQUIRE(script.nestedHelperDestroyResults.size() == 1);
    CHECK(script.nestedHelperDestroyResults[0]);
    REQUIRE(script.payloads.size() == 2);
    CHECK(script.payloads[0].iconAdvertised);
    CHECK(script.payloads[0].iconValidBeforeReentrance);
    CHECK(script.payloads[0].iconValidAfterReentrance);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(snapshot.versionApplied);
    CHECK_FALSE(snapshot.reconciling);

    script.calls.clear();
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "retry"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() ==
          helpersBefore + 1);

    script.calls.clear();
    delete icon;
    CheckCalls(script.calls, { NIM_DELETE });
    DrainTaskBarDeletes();
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::InitialFailureIsNotPubliclyInstalled",
          "[winui-beta-aux][taskbaricon][failure][generation]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    script.results.push_back({ NIM_ADD, false });
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "failed initial"));
    CheckCalls(script.calls, { NIM_ADD });
    CHECK_FALSE(icon->IsIconInstalled());
    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);

#if wxUSE_TASKBARICON_BALLOONS
    ClearNativeTrace(script);
    CHECK_FALSE(icon->ShowBalloon("must not", "install"));
    CHECK(script.calls.empty());
#endif

    // The failed desired intent still exists and must be cancellable even
    // though the compatibility flag correctly stayed false.
    ClearNativeTrace(script);
    REQUIRE(icon->RemoveIcon());
    CheckCalls(script.calls, { NIM_DELETE });
    snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Absent);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK_FALSE(icon->IsIconInstalled());

    // Drive the production TaskbarCreated message path: a cancelled failed
    // intent is never resurrected.
    ClearNativeTrace(script);
    REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(icon));
    CHECK(script.calls.empty());

    delete icon;
    DrainTaskBarDeletes();
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::FailedModifyKeepsSuccessfulHistory",
          "[winui-beta-aux][taskbaricon][failure][generation]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "installed"));
    CHECK(icon->IsIconInstalled());

    ClearNativeTrace(script);
    script.results.push_back({ NIM_MODIFY, false });
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "modify fails"));
    CheckCalls(script.calls, { NIM_MODIFY });
    CHECK(icon->IsIconInstalled());
    CHECK(Snapshot(icon).applied ==
          wxWinUITaskBarIconAppliedState::Unknown);

    ClearNativeTrace(script);
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "explicit retry"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
    CHECK(icon->IsIconInstalled());

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::FailuresRemainUnknownUntilAProvenRetry",
          "[winui-beta-aux][taskbaricon][failure]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    script.results.push_back({ NIM_ADD, false });
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "failed add"));
    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(snapshot.versionApplied);

    script.calls.clear();
    script.results.push_back({ NIM_ADD, false });
    script.results.push_back({ NIM_MODIFY, true });
    script.results.push_back({ NIM_SETVERSION, true });
    REQUIRE(wxWinUITaskBarIconRestartExplorerForTesting(icon));
    CheckCalls(script.calls,
               { NIM_ADD, NIM_MODIFY, NIM_SETVERSION });
    CHECK(Snapshot(icon).applied ==
          wxWinUITaskBarIconAppliedState::Present);

    script.calls.clear();
    script.results.push_back({ NIM_DELETE, false });
    CHECK_FALSE(icon->RemoveIcon());
    snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);

    script.calls.clear();
    script.results.push_back({ NIM_DELETE, true });
    REQUIRE(wxWinUITaskBarIconRestartExplorerForTesting(icon));
    CheckCalls(script.calls, { NIM_DELETE });
    CHECK(Snapshot(icon).applied ==
          wxWinUITaskBarIconAppliedState::Absent);

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::SetVersionFailureIsRetried",
          "[winui-beta-aux][taskbaricon][failure][version]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    script.results.push_back({ NIM_ADD, true });
    script.results.push_back({ NIM_SETVERSION, false });
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "version fails"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });

    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK_FALSE(snapshot.versionApplied);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK_FALSE(icon->IsIconInstalled());

#if wxUSE_TASKBARICON_BALLOONS
    ClearNativeTrace(script);
    CHECK_FALSE(icon->ShowBalloon("version", "not applied"));
    CHECK(script.calls.empty());
#endif

    ClearNativeTrace(script);
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "version retry"));
    CheckCalls(script.calls, { NIM_MODIFY, NIM_SETVERSION });
    snapshot = Snapshot(icon);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK(icon->IsIconInstalled());

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::NativeIconLeaseOutlivesOwnerReentrance",
          "[winui-beta-aux][taskbaricon][lifetime][payload]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    const wxWeakRef<wxEvtHandler> weakIcon(icon);
    script.destroyOwnerOnAdd = icon;

    CHECK_FALSE(icon->SetIcon(
        MakeTaskBarTestIcon(), "leased icon tooltip"));
    CheckCalls(script.calls, { NIM_ADD, NIM_DELETE });
    CHECK_FALSE(weakIcon);
    REQUIRE(script.payloads.size() == 2);
    const NativePayload& payload = script.payloads[0];
    CHECK(payload.message == static_cast<unsigned>(NIM_ADD));
    CHECK((payload.flags & (NIF_MESSAGE | NIF_ICON | NIF_TIP)) ==
          (NIF_MESSAGE | NIF_ICON | NIF_TIP));
    CHECK(payload.tooltip == "leased icon tooltip");
    CHECK(payload.iconAdvertised);
    CHECK(payload.iconValidBeforeReentrance);
    CHECK(payload.iconValidAfterReentrance);

    DrainTaskBarDeletes(32);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

#if wxUSE_TASKBARICON_BALLOONS

TEST_CASE("WinUITaskBarIcon::BalloonResultCannotClobberNewerState",
          "[winui-beta-aux][taskbaricon][balloon][reentrancy]")
{
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "initial"));

    const auto beforeRewrite = Snapshot(icon);
    script.calls.clear();
    script.results.push_back({ NIM_MODIFY, false });
    script.rewriteOnModify = icon;
    CHECK_FALSE(icon->ShowBalloon("title", "rewritten"));
    CheckCalls(script.calls,
               { NIM_MODIFY, NIM_MODIFY, NIM_SETVERSION });
    auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredGeneration != beforeRewrite.desiredGeneration);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);

    script.calls.clear();
    script.results.push_back({ NIM_MODIFY, false });
    script.removeOnModify = icon;
    CHECK_FALSE(icon->ShowBalloon("title", "removed"));
    CheckCalls(script.calls, { NIM_MODIFY, NIM_DELETE });
    snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Absent);
    CHECK_FALSE(snapshot.versionApplied);

    script.calls.clear();
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "before recreation"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });

    script.calls.clear();
    script.results.push_back({ NIM_MODIFY, false });
    script.recreateHelperOnModify = icon;
    CHECK_FALSE(icon->ShowBalloon("title", "recreated"));
    CheckCalls(script.calls,
               { NIM_MODIFY, NIM_DELETE, NIM_ADD, NIM_SETVERSION });
    snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() ==
          helpersBefore + 1);

    const auto beforeRestart = snapshot;
    script.calls.clear();
    script.results.push_back({ NIM_MODIFY, false });
    script.dispatchRestartOnModify = true;
    CHECK_FALSE(icon->ShowBalloon("title", "explorer restart"));
    CheckCalls(script.calls, { NIM_MODIFY, NIM_ADD, NIM_SETVERSION });
    snapshot = Snapshot(icon);
    CHECK(snapshot.explorerGeneration != beforeRestart.explorerGeneration);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);
    CHECK(snapshot.appliedExplorerGeneration ==
          snapshot.explorerGeneration);

    // A failure that still belongs to the exact presentation invalidates our
    // native-state claim and is retried before the next balloon is sent.
    script.calls.clear();
    script.results.push_back({ NIM_MODIFY, false });
    CHECK_FALSE(icon->ShowBalloon("title", "current failure"));
    CheckCalls(script.calls, { NIM_MODIFY });
    snapshot = Snapshot(icon);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Unknown);
    CHECK_FALSE(snapshot.versionApplied);

    script.calls.clear();
    REQUIRE(icon->ShowBalloon("title", "retry"));
    CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION, NIM_MODIFY });
    snapshot = Snapshot(icon);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.versionApplied);

    delete icon;
    DrainTaskBarDeletes();
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::BalloonAllowsOwnerDestructionInNativeSeam",
          "[winui-beta-aux][taskbaricon][balloon][lifetime]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    const wxWeakRef<wxEvtHandler> weakIcon(icon);

    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "destroy in balloon"));
    ClearNativeTrace(script);
    script.destroyOwnerOnModify = icon;
    CHECK_FALSE(icon->ShowBalloon(
        "title", "destroy owner", 0, 0, MakeTaskBarTestIcon()));
    CheckCalls(script.calls, { NIM_MODIFY, NIM_DELETE });
    REQUIRE(script.payloads.size() == 2);
    const NativePayload& balloonPayload = script.payloads[0];
    CHECK(balloonPayload.message == static_cast<unsigned>(NIM_MODIFY));
    CHECK((balloonPayload.flags & NIF_INFO) != 0);
    CHECK(balloonPayload.infoTitle == "title");
    CHECK(balloonPayload.info == "destroy owner");
    CHECK(balloonPayload.balloonIconAdvertised);
    CHECK(balloonPayload.balloonIconValidBeforeReentrance);
    CHECK(balloonPayload.balloonIconValidAfterReentrance);
    DrainTaskBarDeletes();
    CHECK_FALSE(weakIcon);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

#endif // wxUSE_TASKBARICON_BALLOONS

TEST_CASE("WinUITaskBarIcon::ReentrantRemoveWinsTheExactGeneration",
          "[winui-beta-aux][taskbaricon][reentrancy]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    script.removeOnAdd = icon;

    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "superseded"));
    CheckCalls(script.calls, { NIM_ADD, NIM_DELETE });
    REQUIRE(script.nestedRemoveResults.size() == 1);
    // The nested call publishes the winning intent, but returns before the
    // outer transaction can apply that exact absence generation.
    CHECK_FALSE(script.nestedRemoveResults[0]);
    const auto snapshot = Snapshot(icon);
    CHECK_FALSE(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Absent);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK_FALSE(icon->IsIconInstalled());

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::ReentrantSetSupersedesOnlyItsOwnGeneration",
          "[winui-beta-aux][taskbaricon][reentrancy][generation]")
{
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);
    wxTaskBarIcon * const icon = new wxTaskBarIcon;
    script.rewriteOnAdd = icon;
    script.results.push_back({ NIM_ADD, true });
    script.results.push_back({ NIM_ADD, false });
    script.results.push_back({ NIM_MODIFY, true });
    script.results.push_back({ NIM_SETVERSION, true });

    // The outer ADD result belongs to generation A. The seam publishes B;
    // reconciliation converges B, but SetIcon(A) must still report false.
    CHECK_FALSE(icon->SetIcon(MakeTaskBarTestIcon(), "generation A"));
    CheckCalls(script.calls,
               { NIM_ADD, NIM_ADD, NIM_MODIFY, NIM_SETVERSION });
    REQUIRE(script.nestedSetResults.size() == 1);
    CHECK_FALSE(script.nestedSetResults[0]);
    const auto snapshot = Snapshot(icon);
    CHECK(snapshot.desiredInstalled);
    CHECK(snapshot.applied == wxWinUITaskBarIconAppliedState::Present);
    CHECK(snapshot.appliedGeneration == snapshot.desiredGeneration);
    CHECK(snapshot.versionApplied);
    CHECK(icon->IsIconInstalled());

    delete icon;
    DrainTaskBarDeletes();
}

TEST_CASE("WinUITaskBarIcon::LifecycleStressReturnsEveryBaseline",
          "[winui-beta-aux][taskbaricon][lifetime][stress]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope hookScope(script);

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        std::unique_ptr<wxTaskBarIcon> icon(new wxTaskBarIcon);
        REQUIRE(icon->SetIcon(
            MakeTaskBarTestIcon(), wxString::Format("cycle-%u", cycle)));

        if ( cycle % 2 == 0 )
        {
            ClearNativeTrace(script);
            REQUIRE(wxWinUITaskBarIconDestroyHelperForTesting(icon.get()));
            CheckCalls(script.calls, { NIM_DELETE });
            CHECK_FALSE(icon->IsIconInstalled());

            ClearNativeTrace(script);
            REQUIRE(icon->SetIcon(
                MakeTaskBarTestIcon(),
                wxString::Format("cycle-%u-recreated", cycle)));
            CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
        }

        if ( cycle % 3 == 0 )
        {
            ClearNativeTrace(script);
            REQUIRE(wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(
                icon.get()));
            CheckCalls(script.calls, { NIM_ADD, NIM_SETVERSION });
        }

        ClearNativeTrace(script);
        REQUIRE(icon->RemoveIcon());
        CheckCalls(script.calls, { NIM_DELETE });
        icon.reset();
        DrainTaskBarDeletes(2);
        CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
        CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
    }

    DrainTaskBarDeletes(32);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

TEST_CASE("WinUITaskBarIcon::PopupRoutingIsWeakAndScopeBound",
          "[winui-beta-aux][taskbaricon][menu][lifetime]")
{
    const unsigned statesBefore =
        wxWinUIGetTaskBarIconStateCountForTesting();
    const unsigned helpersBefore =
        wxWinUIGetTaskBarIconHelperCountForTesting();
    TaskBarNativeScript script;
    TaskBarShellHookScope shellHookScope(script);
    TaskBarPopupHookScope popupHookScope(&DispatchTaskBarPopup);

    TaskBarMenuProbe *icon = new TaskBarMenuProbe;
    REQUIRE(icon->SetIcon(MakeTaskBarTestIcon(), "menu"));
    wxMenu menu;
    menu.Append(TaskBarCommandId, "command");
    icon->activeMenu = &menu;
    icon->launchDialog = true;

    REQUIRE(icon->PopupMenu(&menu));
    CHECK(icon->updates == 1);
    CHECK(icon->commands == 1);
    CHECK(icon->nestedPopupRejected);
    CHECK(icon->dialogLaunched);
    DrainTaskBarDeletes();

#if wxUSE_EXCEPTIONS
    wxWinUISetTaskBarIconPopupHookForTesting(&ThrowFromTaskBarPopup);
    CHECK_THROWS_AS(icon->PopupMenu(&menu), std::runtime_error);
    wxWinUISetTaskBarIconPopupHookForTesting(&DispatchTaskBarPopup);
    REQUIRE(icon->PopupMenu(&menu));
    CHECK(icon->updates == 2);
    CHECK(icon->commands == 2);
#endif

    icon->destroyOnCommand = true;
    const wxWeakRef<wxEvtHandler> weakIcon(icon);
    REQUIRE(icon->PopupMenu(&menu));
    DrainTaskBarDeletes(32);
    CHECK_FALSE(weakIcon);
    CHECK(wxWinUIGetTaskBarIconStateCountForTesting() == statesBefore);
    CHECK(wxWinUIGetTaskBarIconHelperCountForTesting() == helpersBefore);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TASKBARICON
