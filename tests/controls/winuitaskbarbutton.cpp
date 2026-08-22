///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitaskbarbutton.cpp
// Purpose:     Deterministic MSW/WinUI taskbar button lifecycle contracts
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXMSW__) && wxUSE_TASKBARBUTTON

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif

#include "wx/appprogress.h"
#include "wx/dcmemory.h"
#include "wx/log.h"
#include "wx/msw/taskbarbutton.h"
#include "wx/msw/private/taskbarbutton.h"
#include "wx/taskbarbutton.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace
{

constexpr long wxTASKBAR_TEST_OK = 0;
constexpr long wxTASKBAR_TEST_FAIL = static_cast<long>(0x80004005L);
constexpr unsigned wxTHBN_CLICKED_FOR_TEST = 0x1800;

struct TaskBarProgressCall
{
    WXHWND hwnd { nullptr };
    unsigned long long value { 0 };
    unsigned long long range { 0 };
};

struct TaskBarStateCall
{
    WXHWND hwnd { nullptr };
    unsigned state { 0 };
};

struct TaskBarThumbCall
{
    WXHWND hwnd { nullptr };
    bool add { false };
    std::vector<wxMSWTaskBarThumbButtonNative> buttons;
};

struct TaskBarOverlayCall
{
    WXHWND hwnd { nullptr };
    void* icon { nullptr };
    wxString description;
};

struct TaskBarNativeRecorder
{
    int factories { 0 };
    int initialized { 0 };
    int destroyed { 0 };
    int addTabs { 0 };
    int deleteTabs { 0 };
    int tooltipCalls { 0 };
    int overlayCalls { 0 };
    int clipCalls { 0 };
    bool failNextProgressValue { false };
    bool failNextProgressState { false };
    bool failNextThumbAdd { false };
    bool failNextThumbUpdate { false };
    std::function<void()> factoryAction;
    std::function<void()> releaseAction;
    std::function<void()> reentrantAction;
    std::vector<TaskBarProgressCall> progressCalls;
    std::vector<TaskBarStateCall> stateCalls;
    std::vector<TaskBarThumbCall> thumbCalls;
    std::vector<TaskBarOverlayCall> overlayCallsLog;
    std::map<WXHWND, unsigned> shellProgressStates;

    void InvokeReentrantAction()
    {
        std::function<void()> action = std::move(reentrantAction);
        reentrantAction = std::function<void()>();
        if ( action )
            action();
    }

    void InvokeFactoryAction()
    {
        std::function<void()> action = std::move(factoryAction);
        factoryAction = std::function<void()>();
        if ( action )
            action();
    }

    void InvokeReleaseAction()
    {
        std::function<void()> action = std::move(releaseAction);
        releaseAction = std::function<void()>();
        if ( action )
            action();
    }

    std::size_t ProgressCallCountFor(WXHWND hwnd) const
    {
        std::size_t count = 0;
        for ( const TaskBarProgressCall& call : progressCalls )
        {
            if ( call.hwnd == hwnd )
                ++count;
        }
        return count;
    }

    const TaskBarProgressCall* LastProgressCallFor(WXHWND hwnd) const
    {
        for ( auto it = progressCalls.rbegin();
              it != progressCalls.rend(); ++it )
        {
            if ( it->hwnd == hwnd )
                return &*it;
        }
        return nullptr;
    }

    unsigned ShellProgressStateFor(WXHWND hwnd) const
    {
        const auto found = shellProgressStates.find(hwnd);
        return found == shellProgressStates.end()
            ? static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS)
            : found->second;
    }
};

TaskBarNativeRecorder* gs_taskBarRecorder = nullptr;

class RecordingTaskBarBackend final
    : public wxMSWTaskBarButtonNativeBackend
{
public:
    explicit RecordingTaskBarBackend(TaskBarNativeRecorder* recorder)
        : m_recorder(recorder)
    {
    }

    ~RecordingTaskBarBackend() override
    {
        if ( m_recorder )
        {
            ++m_recorder->destroyed;
            m_recorder->InvokeReleaseAction();
        }
    }

    long Initialize() override
    {
        ++m_recorder->initialized;
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long AddTab(WXHWND WXUNUSED(hwnd)) override
    {
        ++m_recorder->addTabs;
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long DeleteTab(WXHWND WXUNUSED(hwnd)) override
    {
        ++m_recorder->deleteTabs;
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long SetProgressValue(WXHWND hwnd,
                          unsigned long long value,
                          unsigned long long range) override
    {
        m_recorder->progressCalls.push_back({ hwnd, value, range });
        const bool fail = m_recorder->failNextProgressValue;
        m_recorder->failNextProgressValue = false;
        if ( !fail )
        {
            unsigned& state = m_recorder->shellProgressStates[hwnd];
            if ( state == static_cast<unsigned>(
                             wxTASKBAR_BUTTON_NO_PROGRESS) ||
                 state == static_cast<unsigned>(
                             wxTASKBAR_BUTTON_INDETERMINATE) )
            {
                state = static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL);
            }
        }
        m_recorder->InvokeReentrantAction();
        if ( fail )
            return wxTASKBAR_TEST_FAIL;
        return wxTASKBAR_TEST_OK;
    }

    long SetProgressState(WXHWND hwnd, unsigned state) override
    {
        m_recorder->stateCalls.push_back({ hwnd, state });
        const bool fail = m_recorder->failNextProgressState;
        m_recorder->failNextProgressState = false;
        if ( !fail )
            m_recorder->shellProgressStates[hwnd] = state;
        m_recorder->InvokeReentrantAction();
        return fail ? wxTASKBAR_TEST_FAIL : wxTASKBAR_TEST_OK;
    }

    long SetOverlayIcon(WXHWND hwnd,
                        void* icon,
                        const wxString& description) override
    {
        ++m_recorder->overlayCalls;
        m_recorder->overlayCallsLog.push_back(
            { hwnd, icon, description });
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long SetThumbnailTooltip(
        WXHWND WXUNUSED(hwnd),
        const wxString& WXUNUSED(tooltip)) override
    {
        ++m_recorder->tooltipCalls;
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long SetThumbnailClip(WXHWND WXUNUSED(hwnd),
                          const wxRect* WXUNUSED(rect)) override
    {
        ++m_recorder->clipCalls;
        m_recorder->InvokeReentrantAction();
        return wxTASKBAR_TEST_OK;
    }

    long ThumbBarAddButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) override
    {
        RecordThumbCall(true, hwnd, count, buttons);
        m_recorder->InvokeReentrantAction();
        if ( m_recorder->failNextThumbAdd )
        {
            m_recorder->failNextThumbAdd = false;
            return wxTASKBAR_TEST_FAIL;
        }
        return wxTASKBAR_TEST_OK;
    }

    long ThumbBarUpdateButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) override
    {
        RecordThumbCall(false, hwnd, count, buttons);
        m_recorder->InvokeReentrantAction();
        if ( m_recorder->failNextThumbUpdate )
        {
            m_recorder->failNextThumbUpdate = false;
            return wxTASKBAR_TEST_FAIL;
        }
        return wxTASKBAR_TEST_OK;
    }

private:
    void RecordThumbCall(
        bool add,
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons)
    {
        TaskBarThumbCall call;
        call.hwnd = hwnd;
        call.add = add;
        call.buttons.assign(buttons, buttons + count);
        m_recorder->thumbCalls.push_back(std::move(call));
    }

    TaskBarNativeRecorder* m_recorder;
};

std::shared_ptr<wxMSWTaskBarButtonNativeBackend>
CreateRecordingTaskBarBackend()
{
    if ( !gs_taskBarRecorder )
        return {};
    ++gs_taskBarRecorder->factories;
    const std::shared_ptr<RecordingTaskBarBackend> backend =
        std::make_shared<RecordingTaskBarBackend>(gs_taskBarRecorder);
    gs_taskBarRecorder->InvokeFactoryAction();
    return backend;
}

class TaskBarNativeFactoryGuard final
{
public:
    explicit TaskBarNativeFactoryGuard(TaskBarNativeRecorder& recorder)
    {
        REQUIRE(gs_taskBarRecorder == nullptr);
        gs_taskBarRecorder = &recorder;
        wxMSWTaskBarButtonSetNativeFactoryForTesting(
            &CreateRecordingTaskBarBackend);
    }

    ~TaskBarNativeFactoryGuard()
    {
        wxMSWTaskBarButtonSetNativeFactoryForTesting(nullptr);
        gs_taskBarRecorder = nullptr;
    }
};

std::unique_ptr<wxFrame> CreateTaskBarFrame(const wxString& title)
{
    return std::unique_ptr<wxFrame>(
        new wxFrame(nullptr, wxID_ANY, title));
}

std::vector<unsigned> OccupiedThumbTokens(const TaskBarThumbCall& call)
{
    std::vector<unsigned> result;
    for ( const wxMSWTaskBarThumbButtonNative& button : call.buttons )
    {
        if ( button.occupied )
            result.push_back(button.token);
    }
    return result;
}

wxIcon CreateTaskBarTestIcon()
{
    wxBitmap bitmap(16, 16, 32);
    if ( !bitmap.IsOk() )
        return wxIcon();
    {
        wxMemoryDC dc(bitmap);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
    }
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    return icon;
}

} // anonymous namespace

TEST_CASE("WinUITaskBarButton::DefaultThumbAndSevenButtonLimit",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]")
{
    wxThumbBarButton defaultButton;
    CHECK(defaultButton.GetID() == wxID_ANY);
    CHECK(defaultButton.IsEnable());
    CHECK_FALSE(defaultButton.IsDismissOnClick());
    CHECK(defaultButton.HasBackground());
    CHECK(defaultButton.IsShown());
    CHECK(defaultButton.IsInteractive());
    CHECK(defaultButton.GetParent() == nullptr);

    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame = CreateTaskBarFrame("seven thumbs");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);

    wxString longTooltip;
    longTooltip.Pad(400, 'x');
    for ( int i = 0; i < 7; ++i )
    {
        INFO("thumb index " << i);
        CHECK(taskbar->AppendThumbBarButton(
            new wxThumbBarButton(1000 + i, wxNullIcon,
                                 i == 0 ? longTooltip : wxString())));
    }

    wxThumbBarButton* const rejected =
        new wxThumbBarButton(2000, wxNullIcon);
    WX_ASSERT_FAILS_WITH_ASSERT(
        taskbar->AppendThumbBarButton(rejected));
    CHECK(rejected->GetParent() == nullptr);
    delete rejected;

    wxMSWTaskBarButtonSnapshot snapshot;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &snapshot));
    CHECK(snapshot.desiredThumbButtonCount == 7);
    CHECK(snapshot.appliedThumbButtonCount == 7);
    CHECK(snapshot.thumbButtonsInitialized);
    CHECK(snapshot.thumbButtonsAppliedKnown);
    REQUIRE_FALSE(recorder.thumbCalls.empty());
    CHECK(OccupiedThumbTokens(recorder.thumbCalls.back()) ==
          (std::vector<unsigned>{ 0, 1, 2, 3, 4, 5, 6 }));
    CHECK(recorder.thumbCalls.back().buttons[0].tooltip.length() == 259);
}

TEST_CASE("WinUITaskBarButton::FixedSlotsExactRemoveAndCommandBounds",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame = CreateTaskBarFrame("stable thumbs");
    wxTaskBarButton* const taskbar = frame->MSWGetTaskBarButton();
    REQUIRE(taskbar);

    wxThumbBarButton* const first =
        new wxThumbBarButton(700, wxNullIcon);
    wxThumbBarButton* const secondSameId =
        new wxThumbBarButton(700, wxNullIcon);
    REQUIRE(taskbar->AppendThumbBarButton(first));
    REQUIRE(taskbar->AppendThumbBarButton(secondSameId));
    CHECK(taskbar->RemoveThumbBarButton(secondSameId) == secondSameId);
    CHECK(first->GetParent() == taskbar);
    CHECK(secondSameId->GetParent() == nullptr);
    delete secondSameId;

    wxThumbBarButton* const inserted =
        new wxThumbBarButton(701, wxNullIcon);
    REQUIRE(taskbar->InsertThumbBarButton(0, inserted));
    REQUIRE_FALSE(recorder.thumbCalls.empty());
    // Shell fixes the seven physical slots on the first Add. Reordering is
    // represented by updating the contents of slots 0 and 1, never by sending
    // an array in a fictitious new order.
    CHECK(OccupiedThumbTokens(recorder.thumbCalls.back()) ==
          (std::vector<unsigned>{ 0, 1 }));

    int firstClicks = 0;
    int insertedClicks = 0;
    frame->Bind(wxEVT_BUTTON,
                [&](wxCommandEvent& event)
                {
                    if ( event.GetId() == 700 )
                        ++firstClicks;
                    if ( event.GetId() == 701 )
                        ++insertedClicks;
                });

    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    ::SendMessage(hwnd, WM_COMMAND,
                  MAKEWPARAM(6, wxTHBN_CLICKED_FOR_TEST), 0);
    CHECK(firstClicks == 0);
    CHECK(insertedClicks == 0);
    ::SendMessage(hwnd, WM_COMMAND,
                  MAKEWPARAM(0, wxTHBN_CLICKED_FOR_TEST), 0);
    ::SendMessage(hwnd, WM_COMMAND,
                  MAKEWPARAM(1, wxTHBN_CLICKED_FOR_TEST), 0);
    CHECK(firstClicks == 1);
    CHECK(insertedClicks == 1);

    // A delayed token has no generation bits: after slot reuse/reorder it is
    // indistinguishable from a click on the new occupant. This is the exact
    // platform limitation, and the implementation deliberately makes no stale
    // command claim it cannot uphold.
    ::SendMessage(hwnd, WM_COMMAND,
                  MAKEWPARAM(0, wxTHBN_CLICKED_FOR_TEST), 0);
    CHECK(insertedClicks == 2);
}

TEST_CASE("WinUITaskBarButton::ThumbSnapshotsRetainAndClearContents",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame = CreateTaskBarFrame("thumb snapshots");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);

    wxBitmap bitmap(16, 16, 32);
    REQUIRE(bitmap.IsOk());
    {
        wxMemoryDC dc(bitmap);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
    }
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    REQUIRE(icon.IsOk());

    wxThumbBarButton* const original =
        new wxThumbBarButton(810, icon, "old tooltip");
    REQUIRE(taskbar->AppendThumbBarButton(original));
    REQUIRE_FALSE(recorder.thumbCalls.empty());
    const TaskBarThumbCall retainedCall = recorder.thumbCalls.back();
    REQUIRE(retainedCall.buttons[0].retainedIcon.IsOk());
    REQUIRE(retainedCall.buttons[0].icon != nullptr);

    CHECK(taskbar->RemoveThumbBarButton(original) == original);
    delete original;
    REQUIRE_FALSE(recorder.thumbCalls.back().buttons[0].occupied);
    CHECK(recorder.thumbCalls.back().buttons[0].icon == nullptr);
    CHECK(recorder.thumbCalls.back().buttons[0].tooltip.empty());
    // The in-flight/native recorder snapshot owns its HICON independently of
    // the removed public button.
    CHECK(retainedCall.buttons[0].retainedIcon.IsOk());

    REQUIRE(taskbar->AppendThumbBarButton(
        new wxThumbBarButton(811, wxNullIcon, wxString())));
    REQUIRE(recorder.thumbCalls.back().buttons[0].occupied);
    CHECK(recorder.thumbCalls.back().buttons[0].icon == nullptr);
    CHECK(recorder.thumbCalls.back().buttons[0].tooltip.empty());
}

TEST_CASE("WinUITaskBarButton::UnknownRetryAndExplorerReconcile",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[transaction]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame = CreateTaskBarFrame("taskbar retry");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);

    recorder.failNextThumbAdd = true;
    wxLogNull suppressExpectedFailure;
    wxThumbBarButton* const retained =
        new wxThumbBarButton(900, wxNullIcon, "retained");
    CHECK_FALSE(taskbar->AppendThumbBarButton(retained));
    CHECK(retained->GetParent() == taskbar.get());

    wxMSWTaskBarButtonSnapshot failedThumb;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &failedThumb));
    CHECK(failedThumb.desiredThumbButtonCount == 1);
    CHECK_FALSE(failedThumb.thumbButtonsInitialized);
    CHECK_FALSE(failedThumb.thumbButtonsAppliedKnown);

    REQUIRE(static_cast<wxTaskBarButtonImpl*>(taskbar.get())->
                InitOrUpdateThumbBarButtons());
    wxMSWTaskBarButtonSnapshot recoveredThumb;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &recoveredThumb));
    CHECK(recoveredThumb.thumbButtonsInitialized);
    CHECK(recoveredThumb.thumbButtonsAppliedKnown);
    CHECK(recoveredThumb.appliedThumbButtonCount == 1);
    REQUIRE(recorder.thumbCalls.size() >= 2);
    CHECK(recorder.thumbCalls[recorder.thumbCalls.size() - 2].add);
    CHECK(recorder.thumbCalls.back().add);

    taskbar->SetProgressRange(100);
    recorder.failNextProgressValue = true;
    taskbar->SetProgressValue(42);
    wxMSWTaskBarButtonSnapshot failedProgress;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &failedProgress));
    CHECK(failedProgress.desiredProgressValue == 42);
    CHECK(failedProgress.desiredProgressValueKnown);
    CHECK(failedProgress.desiredProgressState ==
          wxTASKBAR_BUTTON_NORMAL);
    CHECK_FALSE(failedProgress.progressValueAppliedKnown);
    CHECK_FALSE(failedProgress.progressStateAppliedKnown);
    const unsigned long long oldEpoch = failedProgress.shellEpoch;

    taskbar->Realize();
    wxMSWTaskBarButtonSnapshot reconciled;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &reconciled));
    CHECK(reconciled.shellEpoch > oldEpoch);
    CHECK(reconciled.progressValueAppliedKnown);
    CHECK(reconciled.appliedProgressRange == 100);
    CHECK(reconciled.appliedProgressValue == 42);
    CHECK(reconciled.progressStateAppliedKnown);
    CHECK(reconciled.appliedProgressState == wxTASKBAR_BUTTON_NORMAL);
    CHECK(recorder.ShellProgressStateFor(frame->GetHWND()) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL));
    CHECK(reconciled.thumbButtonsInitialized);
    CHECK(reconciled.thumbButtonsAppliedKnown);
    CHECK(recorder.thumbCalls.back().add);
}

TEST_CASE("WinUITaskBarButton::RealizeRejectsStaleThumbSnapshot",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("thumb realize generation");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);

    REQUIRE(taskbar->AppendThumbBarButton(
        new wxThumbBarButton(820, wxNullIcon, "snapshot A")));
    const std::size_t callsBeforeRealize = recorder.thumbCalls.size();

    bool nestedInsertSucceeded = false;
    recorder.reentrantAction = [&]()
    {
        wxThumbBarButton* const replacement =
            new wxThumbBarButton(821, wxNullIcon, "snapshot B");
        nestedInsertSucceeded =
            taskbar->InsertThumbBarButton(0, replacement);
        if ( !nestedInsertSucceeded && !replacement->GetParent() )
            delete replacement;
    };
    taskbar->Realize();

    REQUIRE(nestedInsertSucceeded);
    REQUIRE(recorder.thumbCalls.size() >= callsBeforeRealize + 2);
    for ( std::size_t i = callsBeforeRealize;
          i < recorder.thumbCalls.size(); ++i )
    {
        const TaskBarThumbCall& call = recorder.thumbCalls[i];
        REQUIRE(call.buttons.size() == 7);
        // Snapshot A was captured before the first Realize seam, but mutation
        // B won during that seam. No later native call may publish A alone.
        CHECK(call.buttons[0].tooltip == "snapshot B");
    }
    const TaskBarThumbCall& finalCall = recorder.thumbCalls.back();
    REQUIRE(finalCall.buttons.size() == 7);
    CHECK(finalCall.buttons[0].token == 0);
    CHECK(finalCall.buttons[0].tooltip == "snapshot B");
    CHECK(finalCall.buttons[1].token == 1);
    CHECK(finalCall.buttons[1].tooltip == "snapshot A");

    wxMSWTaskBarButtonSnapshot snapshot;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &snapshot));
    CHECK(snapshot.thumbButtonsInitialized);
    CHECK(snapshot.thumbButtonsAppliedKnown);
    CHECK(snapshot.desiredThumbButtonCount == 2);
    CHECK(snapshot.appliedThumbButtonCount == 2);
}

TEST_CASE("WinUITaskBarButton::ExactProgressValueStateMachine",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[transaction]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("taskbar exact progress state");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);
    const WXHWND hwnd = frame->GetHWND();

    wxMSWTaskBarButtonSnapshot initial;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &initial));
    CHECK_FALSE(initial.desiredProgressValueKnown);

    const std::size_t progressCallsBeforeRange =
        recorder.progressCalls.size();
    const std::size_t stateCallsBeforeRange = recorder.stateCalls.size();
    taskbar->SetProgressRange(100);
    CHECK(recorder.progressCalls.size() == progressCallsBeforeRange);
    CHECK(recorder.stateCalls.size() == stateCallsBeforeRange);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));

    // An Explorer replay after a range-only setup must not synthesize the
    // otherwise-valid zero value and make a normal indicator visible.
    taskbar->Realize();
    CHECK(recorder.progressCalls.size() == progressCallsBeforeRange);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));

    taskbar->SetProgressValue(0);
    REQUIRE_FALSE(recorder.progressCalls.empty());
    CHECK(recorder.progressCalls.back().value == 0);
    CHECK(recorder.progressCalls.back().range == 100);
    wxMSWTaskBarButtonSnapshot zero;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &zero));
    CHECK(zero.desiredProgressValueKnown);
    CHECK(zero.desiredProgressValue == 0);
    CHECK(zero.desiredProgressState == wxTASKBAR_BUTTON_NORMAL);
    CHECK(zero.progressValueAppliedKnown);
    CHECK(zero.progressStateAppliedKnown);
    CHECK(zero.appliedProgressState == wxTASKBAR_BUTTON_NORMAL);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL));

    // The public wx contract dismisses a determinate bar at completion even
    // though SetProgressValue() itself first forces the Shell to NORMAL.
    taskbar->SetProgressValue(100);
    wxMSWTaskBarButtonSnapshot completed;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &completed));
    CHECK(completed.desiredProgressValueKnown);
    CHECK(completed.desiredProgressValue == 100);
    CHECK(completed.desiredProgressState ==
          wxTASKBAR_BUTTON_NO_PROGRESS);
    CHECK(completed.progressValueAppliedKnown);
    CHECK(completed.progressStateAppliedKnown);
    CHECK(completed.appliedProgressState ==
          wxTASKBAR_BUTTON_NO_PROGRESS);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));

    taskbar->PulseProgress();
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_INDETERMINATE));
    const std::size_t statesBeforePulseValue = recorder.stateCalls.size();
    taskbar->SetProgressValue(10);
    CHECK(recorder.stateCalls.size() == statesBeforePulseValue);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL));
    wxMSWTaskBarButtonSnapshot afterPulseValue;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &afterPulseValue));
    CHECK(afterPulseValue.desiredProgressState ==
          wxTASKBAR_BUTTON_NORMAL);
    CHECK(afterPulseValue.appliedProgressState ==
          wxTASKBAR_BUTTON_NORMAL);
    CHECK(afterPulseValue.progressStateAppliedKnown);

    taskbar->SetProgressState(wxTASKBAR_BUTTON_ERROR);
    const std::size_t statesBeforeErrorValue = recorder.stateCalls.size();
    taskbar->SetProgressValue(20);
    CHECK(recorder.stateCalls.size() == statesBeforeErrorValue);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_ERROR));
    wxMSWTaskBarButtonSnapshot afterErrorValue;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &afterErrorValue));
    CHECK(afterErrorValue.desiredProgressState == wxTASKBAR_BUTTON_ERROR);
    CHECK(afterErrorValue.appliedProgressState == wxTASKBAR_BUTTON_ERROR);
    CHECK(afterErrorValue.progressStateAppliedKnown);

    taskbar->SetProgressState(wxTASKBAR_BUTTON_PAUSED);
    const std::size_t statesBeforePausedValue = recorder.stateCalls.size();
    taskbar->SetProgressValue(30);
    CHECK(recorder.stateCalls.size() == statesBeforePausedValue);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_PAUSED));
    wxMSWTaskBarButtonSnapshot afterPausedValue;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &afterPausedValue));
    CHECK(afterPausedValue.desiredProgressState == wxTASKBAR_BUTTON_PAUSED);
    CHECK(afterPausedValue.appliedProgressState == wxTASKBAR_BUTTON_PAUSED);
    CHECK(afterPausedValue.progressStateAppliedKnown);

    // Realize() must replay the value before the final state. The reverse order
    // would make both of these end up NORMAL due to SetProgressValue semantics.
    taskbar->PulseProgress();
    taskbar->Realize();
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_INDETERMINATE));
    taskbar->SetProgressState(wxTASKBAR_BUTTON_NO_PROGRESS);
    taskbar->Realize();
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));

    // A failed value write makes both its value and state side effects unknown.
    // A direct retry reconciles the ambiguous state only after the value works.
    recorder.failNextProgressValue = true;
    {
        wxLogNull suppressExpectedFailure;
        taskbar->SetProgressValue(40);
    }
    wxMSWTaskBarButtonSnapshot failed;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &failed));
    CHECK(failed.desiredProgressState == wxTASKBAR_BUTTON_NORMAL);
    CHECK_FALSE(failed.progressValueAppliedKnown);
    CHECK_FALSE(failed.progressStateAppliedKnown);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));

    const std::size_t statesBeforeRetry = recorder.stateCalls.size();
    taskbar->SetProgressValue(40);
    CHECK(recorder.stateCalls.size() == statesBeforeRetry + 1);
    wxMSWTaskBarButtonSnapshot retried;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &retried));
    CHECK(retried.progressValueAppliedKnown);
    CHECK(retried.progressStateAppliedKnown);
    CHECK(retried.appliedProgressState == wxTASKBAR_BUTTON_NORMAL);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL));
}

TEST_CASE("WinUITaskBarButton::ProgressStatesAndAuxiliaryShellState",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("taskbar state matrix");
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame.get()));
    REQUIRE(taskbar);

    taskbar->SetProgressRange(250);
    taskbar->SetProgressValue(125);
    const wxTaskBarButtonState states[] = {
        wxTASKBAR_BUTTON_NORMAL,
        wxTASKBAR_BUTTON_PAUSED,
        wxTASKBAR_BUTTON_ERROR,
        wxTASKBAR_BUTTON_INDETERMINATE,
        wxTASKBAR_BUTTON_NO_PROGRESS
    };
    for ( const wxTaskBarButtonState state : states )
    {
        taskbar->SetProgressState(state);
        wxMSWTaskBarButtonSnapshot snapshot;
        REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
            taskbar.get(), &snapshot));
        CHECK(snapshot.desiredProgressState == state);
        CHECK(snapshot.progressStateAppliedKnown);
        CHECK(snapshot.appliedProgressState == state);
    }

    taskbar->PulseProgress();
    wxMSWTaskBarButtonSnapshot pulse;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(taskbar.get(), &pulse));
    CHECK(pulse.desiredProgressState ==
          wxTASKBAR_BUTTON_INDETERMINATE);
    CHECK(pulse.progressStateAppliedKnown);

    taskbar->SetThumbnailTooltip("taskbar tooltip");
    taskbar->SetOverlayIcon(wxNullIcon, "cleared overlay");
    taskbar->SetThumbnailClip(wxRect(1, 2, 30, 40));
    taskbar->SetThumbnailClip(wxRect());
    taskbar->Hide();
    taskbar->Show();
    CHECK(recorder.tooltipCalls == 1);
    CHECK(recorder.overlayCalls == 1);
    CHECK(recorder.clipCalls == 2);
    CHECK(recorder.deleteTabs == 1);
    CHECK(recorder.addTabs == 1);
}

TEST_CASE("WinUITaskBarButton::DestroyDuringNativeCallCannotPublish",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    wxFrame* frame = CreateTaskBarFrame("destroy in taskbar seam").release();
    std::unique_ptr<wxTaskBarButton> taskbar(
        wxTaskBarButton::New(frame));
    REQUIRE(taskbar);
    taskbar->SetProgressRange(100);

    recorder.reentrantAction = [&]()
    {
        delete frame;
        frame = nullptr;
    };
    taskbar->SetProgressValue(37);

    wxMSWTaskBarButtonSnapshot snapshot;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar.get(), &snapshot));
    CHECK(snapshot.retired);
    CHECK_FALSE(snapshot.available);
    CHECK_FALSE(snapshot.progressValueAppliedKnown);
    CHECK(recorder.destroyed == 1);

    taskbar.reset();
    CHECK(recorder.destroyed == 1);
}

TEST_CASE("WinUITaskBarButton::FrameRebindPreservesPointerAndDesiredState",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("taskbar native generation");
    wxTaskBarButton* const first = frame->MSWGetTaskBarButton();
    REQUIRE(first);
    first->SetProgressRange(100);
    first->SetProgressValue(42);
    const wxIcon overlay = CreateTaskBarTestIcon();
    REQUIRE(overlay.IsOk());
    first->SetOverlayIcon(overlay, "generation overlay");
    REQUIRE(first->AppendThumbBarButton(
        new wxThumbBarButton(980, overlay, "generation thumb")));

    wxMSWTaskBarButtonSnapshot before;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(first, &before));
    REQUIRE(before.hwndGeneration != 0);
    const int factoriesBefore = recorder.factories;
    const int destroyedBefore = recorder.destroyed;
    const int overlayCallsBefore = recorder.overlayCalls;
    const std::size_t progressCallsBefore = recorder.progressCalls.size();
    const std::size_t thumbCallsBefore = recorder.thumbCalls.size();

    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    REQUIRE(::RemovePropW(
        hwnd, L"wxWidgets.TaskBarButton.HwndGeneration") != nullptr);

    const UINT message = ::RegisterWindowMessageW(L"TaskbarButtonCreated");
    REQUIRE(message != 0);
    ::SendMessage(hwnd, message, 0, 0);
    wxTaskBarButton* const rebound = frame->MSWGetTaskBarButton();
    REQUIRE(rebound);
    CHECK(rebound == first);
    wxMSWTaskBarButtonSnapshot after;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        rebound, &after));
    CHECK(after.available);
    CHECK(after.hwnd == before.hwnd);
    CHECK(after.hwndGeneration != before.hwndGeneration);
    CHECK(after.desiredProgressValueKnown);
    CHECK(after.desiredProgressValue == 42);
    CHECK(after.appliedProgressValue == 42);
    CHECK(after.desiredThumbButtonCount == 1);
    CHECK(after.appliedThumbButtonCount == 1);
    CHECK(recorder.factories == factoriesBefore);
    CHECK(recorder.destroyed == destroyedBefore);
    CHECK(recorder.progressCalls.size() == progressCallsBefore + 1);
    REQUIRE(recorder.overlayCalls == overlayCallsBefore + 1);
    REQUIRE_FALSE(recorder.overlayCallsLog.empty());
    CHECK(recorder.overlayCallsLog.back().description ==
          "generation overlay");
    CHECK(recorder.overlayCallsLog.back().icon != nullptr);
    REQUIRE(recorder.thumbCalls.size() == thumbCallsBefore + 1);
    CHECK(recorder.thumbCalls.back().add);
    CHECK(OccupiedThumbTokens(recorder.thumbCalls.back()) ==
          (std::vector<unsigned>{ 0 }));
    CHECK(recorder.thumbCalls.back().buttons[0].tooltip ==
          "generation thumb");
}

TEST_CASE("WinUITaskBarButton::FrameRebindsGenerationInsideNativeSeam",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("taskbar reentrant native generation");
    wxTaskBarButton* const first = frame->MSWGetTaskBarButton();
    REQUIRE(first);
    first->SetProgressRange(100);

    wxMSWTaskBarButtonSnapshot before;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(first, &before));
    const int factoriesBefore = recorder.factories;
    const int destroyedBefore = recorder.destroyed;
    wxTaskBarButton* rebound = nullptr;
    bool generationRemoved = false;

    recorder.reentrantAction = [&]()
    {
        const HWND hwnd = static_cast<HWND>(frame->GetHWND());
        generationRemoved = ::RemovePropW(
            hwnd, L"wxWidgets.TaskBarButton.HwndGeneration") != nullptr;
        // The same public controller is rebound while its old-generation
        // SetProgressValue() remains on the stack. Its operation lease must not
        // publish over the desired mutation made on the new exact identity.
        rebound = frame->MSWGetTaskBarButton();
        if ( rebound )
        {
            rebound->SetProgressRange(100);
            rebound->SetProgressValue(65);
        }
    };

    first->SetProgressValue(25);

    REQUIRE(generationRemoved);
    REQUIRE(rebound);
    CHECK(rebound == first);
    wxMSWTaskBarButtonSnapshot after;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        rebound, &after));
    CHECK(after.available);
    CHECK(after.hwndGeneration != before.hwndGeneration);
    CHECK(after.desiredProgressValueKnown);
    CHECK(after.desiredProgressValue == 65);
    CHECK(after.progressValueAppliedKnown);
    CHECK(after.appliedProgressValue == 65);
    CHECK(recorder.factories == factoriesBefore);
    CHECK(recorder.destroyed == destroyedBefore);
    const TaskBarProgressCall* const finalCall =
        recorder.LastProgressCallFor(frame->GetHWND());
    REQUIRE(finalCall);
    CHECK(finalCall->value == 65);
}

TEST_CASE("WinUITaskBarButton::RebindConvergenceIsBoundedAndRetryable",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance][stress]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("bounded taskbar rebind");
    wxTaskBarButton* const taskbar = frame->MSWGetTaskBarButton();
    REQUIRE(taskbar);
    taskbar->SetProgressRange(100);
    taskbar->SetProgressValue(55);

    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    REQUIRE(::RemovePropW(
        hwnd, L"wxWidgets.TaskBarButton.HwndGeneration") != nullptr);
    int invalidations = 0;
    std::function<void()> invalidateGeneration;
    invalidateGeneration = [&]()
    {
        ++invalidations;
        CHECK(::RemovePropW(
            hwnd, L"wxWidgets.TaskBarButton.HwndGeneration") != nullptr);
        if ( invalidations < 32 )
            recorder.reentrantAction = invalidateGeneration;
        CHECK(frame->MSWGetTaskBarButton() == taskbar);
    };
    recorder.reentrantAction = invalidateGeneration;

    CHECK(frame->MSWGetTaskBarButton() == taskbar);
    CHECK(invalidations == 32);
    wxMSWTaskBarButtonSnapshot exhausted;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar, &exhausted));
    CHECK_FALSE(exhausted.available);
    CHECK_FALSE(exhausted.progressValueAppliedKnown);
    CHECK(exhausted.desiredProgressValue == 55);

    // The next accessor is a fresh bounded transaction and must converge once
    // the hostile callback stops invalidating each newly published identity.
    CHECK(frame->MSWGetTaskBarButton() == taskbar);
    wxMSWTaskBarButtonSnapshot recovered;
    REQUIRE(wxMSWTaskBarButtonGetSnapshotForTesting(
        taskbar, &recovered));
    CHECK(recovered.available);
    CHECK(recovered.progressValueAppliedKnown);
    CHECK(recovered.appliedProgressValue == 55);
    CHECK(recorder.factories == 1);
}

TEST_CASE("WinUITaskBarButton::InitialFactoryAndInitializeAreSingleOwner",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("taskbar initial owner transaction");
    wxTaskBarButton* nested = reinterpret_cast<wxTaskBarButton*>(1);

    SECTION("factory")
    {
        recorder.factoryAction = [&]()
        {
            nested = frame->MSWGetTaskBarButton();
        };
    }
    SECTION("Initialize")
    {
        recorder.reentrantAction = [&]()
        {
            nested = frame->MSWGetTaskBarButton();
        };
    }

    wxTaskBarButton* const taskbar = frame->MSWGetTaskBarButton();
    REQUIRE(taskbar);
    CHECK(nested == nullptr);
    CHECK(frame->MSWGetTaskBarButton() == taskbar);
    CHECK(recorder.factories == 1);
    CHECK(recorder.initialized == 1);
    CHECK(recorder.destroyed == 0);
}

TEST_CASE("WinUITaskBarButton::InitialConstructionRejectsDestroyedFrame",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    wxFrame* frame =
        CreateTaskBarFrame("destroy initial taskbar transaction").release();

    const auto destroyFrame = [&]()
    {
        delete frame;
        frame = nullptr;
    };
    SECTION("factory")
    {
        recorder.factoryAction = destroyFrame;
    }
    SECTION("Initialize")
    {
        recorder.reentrantAction = destroyFrame;
    }

    wxFrame* const caller = frame;
    wxTaskBarButton* const taskbar = caller->MSWGetTaskBarButton();
    CHECK(frame == nullptr);
    CHECK(taskbar == nullptr);
    CHECK(recorder.factories == 1);
    CHECK(recorder.initialized == 1);
    CHECK(recorder.destroyed == 1);
}

TEST_CASE("WinUITaskBarButton::BackendReleaseCannotReenterPublishedOwner",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    wxFrame* frame = CreateTaskBarFrame("hostile taskbar Release").release();
    REQUIRE(frame->MSWGetTaskBarButton());

    int releaseCallbacks = 0;
    wxTaskBarButton* nested = reinterpret_cast<wxTaskBarButton*>(1);
    recorder.releaseAction = [&]()
    {
        ++releaseCallbacks;
        nested = frame->MSWGetTaskBarButton();
    };

    delete frame;
    frame = nullptr;
    CHECK(releaseCallbacks == 1);
    CHECK(nested == nullptr);
    CHECK(recorder.factories == 1);
    CHECK(recorder.destroyed == 1);
}

TEST_CASE("WinUITaskBarButton::TaskbarCreatedStopsAfterFrameDestruction",
          "[winui-beta-aux][winui-beta-taskbarbutton][taskbarbutton]"
          "[lifetime][transaction][reentrance][explorer]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    wxFrame* frame = CreateTaskBarFrame("destroy in Explorer replay").release();
    wxTaskBarButton* const taskbar = frame->MSWGetTaskBarButton();
    REQUIRE(taskbar);
    taskbar->SetProgressRange(100);
    taskbar->SetProgressValue(30);
    wxAppProgressIndicator progress(frame, 100);
    REQUIRE(progress.IsAvailable());
    progress.SetValue(40);

    bool destroyedInRealize = false;
    recorder.reentrantAction = [&]()
    {
        destroyedInRealize = true;
        delete frame;
        frame = nullptr;
    };
    const UINT message = ::RegisterWindowMessageW(L"TaskbarButtonCreated");
    REQUIRE(message != 0);
    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    ::SendMessage(hwnd, message, 0, 0);

    CHECK(destroyedInRealize);
    CHECK(frame == nullptr);
    CHECK_FALSE(progress.IsAvailable());
    CHECK(recorder.destroyed == recorder.factories);
}

TEST_CASE("WinUIAppProgress::NestedMutationAvailabilityAndSelfDestroy",
          "[winui-beta-aux][winui-beta-taskbarbutton][appprogress]"
          "[lifetime][transaction][reentrance]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("app progress reentrance");
    const WXHWND hwnd = frame->GetHWND();
    std::unique_ptr<wxAppProgressIndicator> progress(
        new wxAppProgressIndicator(frame.get(), 100));
    REQUIRE(progress->IsAvailable());

    progress->SetValue(10);
    bool nestedAvailable = false;
    recorder.reentrantAction = [&]()
    {
        nestedAvailable = progress && progress->IsAvailable();
        if ( progress )
            progress->SetValue(80);
    };
    progress->SetValue(20);
    CHECK(nestedAvailable);
    const TaskBarProgressCall* const nestedFinal =
        recorder.LastProgressCallFor(hwnd);
    REQUIRE(nestedFinal);
    CHECK(nestedFinal->value == 80);
    CHECK(nestedFinal->range == 100);

    recorder.reentrantAction = [&]() { progress.reset(); };
    wxAppProgressIndicator* const doomed = progress.get();
    doomed->SetValue(35);
    CHECK(progress == nullptr);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS));
}

TEST_CASE("WinUIAppProgress::TaskbarCreatedReplaysIndependentController",
          "[winui-beta-aux][winui-beta-taskbarbutton][appprogress]"
          "[transaction][explorer]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);
    std::unique_ptr<wxFrame> frame =
        CreateTaskBarFrame("app progress Explorer replay");
    const WXHWND hwnd = frame->GetHWND();
    wxAppProgressIndicator progress(frame.get(), 100);
    REQUIRE(progress.IsAvailable());
    progress.SetValue(45);
    const std::size_t callsBefore =
        recorder.ProgressCallCountFor(hwnd);
    recorder.shellProgressStates[hwnd] =
        static_cast<unsigned>(wxTASKBAR_BUTTON_NO_PROGRESS);

    wxMSWAppProgressNotifyTaskbarCreated(frame.get());

    CHECK(recorder.ProgressCallCountFor(hwnd) > callsBefore);
    const TaskBarProgressCall* const replay =
        recorder.LastProgressCallFor(hwnd);
    REQUIRE(replay);
    CHECK(replay->value == 45);
    CHECK(replay->range == 100);
    CHECK(recorder.ShellProgressStateFor(hwnd) ==
          static_cast<unsigned>(wxTASKBAR_BUTTON_NORMAL));
}

TEST_CASE("WinUIAppProgress::PerTLWWeakIdentityAndHundredCycles",
          "[winui-beta-aux][winui-beta-taskbarbutton][appprogress]"
          "[lifetime][stress]")
{
    TaskBarNativeRecorder recorder;
    TaskBarNativeFactoryGuard seam(recorder);

    wxFrame* first = CreateTaskBarFrame("app progress first").release();
    wxFrame* second = CreateTaskBarFrame("app progress second").release();
    const WXHWND firstHwnd = first->GetHWND();
    const WXHWND secondHwnd = second->GetHWND();
    {
        wxAppProgressIndicator progress(nullptr, 100);
        REQUIRE(progress.IsAvailable());
        progress.SetValue(40);

        const TaskBarProgressCall* const firstCall =
            recorder.LastProgressCallFor(firstHwnd);
        const TaskBarProgressCall* const secondCall =
            recorder.LastProgressCallFor(secondHwnd);
        REQUIRE(firstCall);
        REQUIRE(secondCall);
        CHECK(firstCall->value == 40);
        CHECK(firstCall->range == 100);
        CHECK(secondCall->value == 40);
        CHECK(secondCall->range == 100);

        const std::size_t firstCallsBeforeDestroy =
            recorder.ProgressCallCountFor(firstHwnd);
        delete first;
        first = nullptr;
        progress.SetRange(80);
        progress.SetValue(60);
        CHECK(recorder.ProgressCallCountFor(firstHwnd) ==
              firstCallsBeforeDestroy);
        const TaskBarProgressCall* const survivingCall =
            recorder.LastProgressCallFor(secondHwnd);
        REQUIRE(survivingCall);
        CHECK(survivingCall->value == 60);
        CHECK(survivingCall->range == 80);

        const std::size_t secondCallsBeforeDestroy =
            recorder.ProgressCallCountFor(secondHwnd);
        delete second;
        second = nullptr;
        // The test harness itself may own another permanent TLW, so an
        // all-top-level indicator can legitimately remain available here.
        // What matters is that reconciliation never calls either retired
        // native identity again.
        progress.SetRange(70);
        progress.SetValue(50);
        CHECK(recorder.ProgressCallCountFor(firstHwnd) ==
              firstCallsBeforeDestroy);
        CHECK(recorder.ProgressCallCountFor(secondHwnd) ==
              secondCallsBeforeDestroy);
    }

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        INFO("app progress cycle " << cycle);
        wxFrame* const frame =
            CreateTaskBarFrame(wxString::Format(
                "app progress cycle %d", cycle)).release();
        std::unique_ptr<wxAppProgressIndicator> progress(
            new wxAppProgressIndicator(frame, 10));
        REQUIRE(progress->IsAvailable());
        progress->SetValue(cycle % 11);
        delete frame;
        CHECK_FALSE(progress->IsAvailable());
        progress.reset();
    }

    CHECK(recorder.destroyed == recorder.factories);
    CHECK(recorder.initialized == recorder.factories);
}

#endif // __WXMSW__ && wxUSE_TASKBARBUTTON
