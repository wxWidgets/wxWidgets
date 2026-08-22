///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuinotification.cpp
// Purpose:     WinUI AppNotification broker contract tests
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && \
        wxUSE_WINRT && wxUSE_TASKBARICON && wxUSE_TASKBARICON_BALLOONS

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/notifmsg.h"
#include "wx/thread.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/winui/private/notification.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{

struct DispatchBarrier
{
    std::mutex mutex;
    std::condition_variable changed;
    bool entered = false;
    bool release = false;
};

DispatchBarrier* gs_dispatchBarrier = nullptr;
std::atomic<bool>* gs_reentrantShutdownReturned = nullptr;
wxNotificationMessage* gs_dispatchCloseNotification = nullptr;
std::atomic<bool>* gs_dispatchCloseReturned = nullptr;
std::atomic<bool>* gs_dispatchCloseResult = nullptr;

void BlockNotificationDispatchForTesting()
{
    DispatchBarrier* const barrier = gs_dispatchBarrier;
    if ( !barrier )
        return;

    std::unique_lock<std::mutex> lock(barrier->mutex);
    barrier->entered = true;
    barrier->changed.notify_all();
    barrier->changed.wait(lock, [barrier]() { return barrier->release; });
}

void ShutdownDuringNotificationDispatchForTesting()
{
    wxWinUIShutdownAppNotificationsForTesting();
    if ( gs_reentrantShutdownReturned )
        gs_reentrantShutdownReturned->store(true);
}

void BlockThenCloseDuringNotificationDispatchForTesting()
{
    BlockNotificationDispatchForTesting();
    wxNotificationMessage* const notification =
        gs_dispatchCloseNotification;
    const bool result = notification && notification->Close();
    if ( gs_dispatchCloseResult )
        gs_dispatchCloseResult->store(result);
    if ( gs_dispatchCloseReturned )
        gs_dispatchCloseReturned->store(true);
}

class NativeCallBarrier final
{
public:
    void Arm()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_armed = true;
        m_entered = false;
        m_released = false;
    }

    void WaitInsideIfArmed()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if ( !m_armed )
            return;

        m_entered = true;
        m_changed.notify_all();
        m_changed.wait(lock, [this]() { return m_released; });
        m_armed = false;
    }

    bool WaitUntilEntered()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_changed.wait_for(
            lock, std::chrono::seconds(2),
            [this]() { return m_entered; });
    }

    void Release()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_released = true;
        m_changed.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_changed;
    bool m_armed = false;
    bool m_entered = false;
    bool m_released = false;
};

void DrainNotifications(unsigned rounds = 24)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainNotificationsUntil(Predicate predicate, unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainNotifications(1);
    }
    return predicate();
}

bool WaitForShutdownIntent()
{
    for ( unsigned i = 0; i < 200; ++i )
    {
        if ( wxWinUIGetAppNotificationSnapshotForTesting().shuttingDown )
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return wxWinUIGetAppNotificationSnapshotForTesting().shuttingDown;
}

bool WaitForShutdownRevocation()
{
    for ( unsigned i = 0; i < 200; ++i )
    {
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        if ( snapshot.shuttingDown && !snapshot.enabled &&
             !snapshot.registered && !snapshot.dispatchEnabled )
        {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    const auto snapshot = wxWinUIGetAppNotificationSnapshotForTesting();
    return snapshot.shuttingDown && !snapshot.enabled &&
           !snapshot.registered && !snapshot.dispatchEnabled;
}

class FakeAppNotificationNative final
    : public wxWinUIAppNotificationNative
{
public:
    bool IsSupported() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ++supportCalls;
        return supported;
    }

    bool Register(wxWinUIAppNotificationActivation activation) override
    {
        std::function<void ()> hook;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            ++registerCalls;
            operationLog.push_back("Register");
            activationPresentDuringRegister = static_cast<bool>(activation);
            hook = std::move(m_registerHook);
        }

        m_registerBarrier.WaitInsideIfArmed();
        if ( hook )
            hook();

        std::lock_guard<std::mutex> lock(m_mutex);
        if ( failRegister )
            return false;
        m_activation = std::move(activation);
        registered = true;
        return true;
    }

    void Unregister() override
    {
        bool mustThrow = false;
        std::function<void ()> hook;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            ++unregisterCalls;
            operationLog.push_back("Unregister");
            registered = false;
            m_activation = {};
            mustThrow = throwNextUnregister;
            throwNextUnregister = false;
            hook = std::move(m_unregisterHook);
        }
        if ( hook )
            hook();
        if ( mustThrow )
            throw std::runtime_error("synthetic Unregister failure");
    }

    bool Show(const wxWinUIAppNotificationPayload& payload,
              std::uint32_t& notificationId) override
    {
        wxWinUIAppNotificationActivation immediate;
        std::function<void ()> hook;
        bool failAfterShow = false;
        bool throwAfterShow = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            ++showCalls;
            if ( failNextShow )
            {
                failNextShow = false;
                return false;
            }
            shown.push_back(payload);
            notificationId = nextId++;
            shownIds.push_back(notificationId);
            operationLog.push_back(
                "Show " + std::to_string(notificationId));
            m_generationsById[notificationId] = payload.generation;
            hook = std::move(m_showHook);
            failAfterShow = failAfterNextShow;
            failAfterNextShow = false;
            throwAfterShow = throwAfterNextShow;
            throwAfterNextShow = false;
            if ( activateInsideShow )
            {
                activateInsideShow = false;
                immediate = m_activation;
            }
        }

        m_showBarrier.WaitInsideIfArmed();
        if ( hook )
            hook();
        if ( immediate )
        {
            immediate(payload.generation,
                      wxWinUIAppNotificationActivationKind::Click,
                      wxID_NONE);
        }
        if ( throwAfterShow )
            throw std::runtime_error("synthetic post-Show failure");
        return !failAfterShow;
    }

    bool Remove(const std::uint32_t notificationId) override
    {
        wxWinUIAppNotificationActivation immediate;
        std::function<void ()> hook;
        std::uint64_t generation = 0;
        bool success = true;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            ++removeCalls;
            if ( failNextRemove )
            {
                failNextRemove = false;
                success = false;
            }
            if ( rejectRemoveWhenUnregistered && !registered )
                success = false;
            operationLog.push_back(
                std::string(success ? "Remove " : "RemoveFail ") +
                std::to_string(notificationId));
            hook = std::move(m_removeHook);
            const auto found = m_generationsById.find(notificationId);
            if ( activateInsideRemove && found != m_generationsById.end() )
            {
                activateInsideRemove = false;
                generation = found->second;
                immediate = m_activation;
            }
            if ( success )
            {
                removedIds.push_back(notificationId);
                m_generationsById.erase(notificationId);
            }
        }

        if ( immediate )
        {
            immediate(generation,
                      wxWinUIAppNotificationActivationKind::Click,
                      wxID_NONE);
        }
        if ( hook )
            hook();
        return success;
    }

    void BlockRegister() { m_registerBarrier.Arm(); }
    bool WaitForBlockedRegister()
    {
        return m_registerBarrier.WaitUntilEntered();
    }
    void ReleaseRegister() { m_registerBarrier.Release(); }

    void BlockShow() { m_showBarrier.Arm(); }
    bool WaitForBlockedShow() { return m_showBarrier.WaitUntilEntered(); }
    void ReleaseShow() { m_showBarrier.Release(); }

    void SetShowHook(std::function<void ()> hook)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_showHook = std::move(hook);
    }

    void SetRegisterHook(std::function<void ()> hook)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registerHook = std::move(hook);
    }

    void SetRemoveHook(std::function<void ()> hook)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_removeHook = std::move(hook);
    }

    void SetUnregisterHook(std::function<void ()> hook)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_unregisterHook = std::move(hook);
    }

    wxWinUIAppNotificationActivation CopyActivation() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_activation;
    }

    void Activate(const std::uint64_t generation,
                  const wxWinUIAppNotificationActivationKind kind,
                  const wxWindowID actionId = wxID_NONE) const
    {
        const auto activation = CopyActivation();
        if ( activation )
            activation(generation, kind, actionId);
    }

    wxWinUIAppNotificationPayload LastPayload() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return shown.empty()
            ? wxWinUIAppNotificationPayload()
            : shown.back();
    }

    std::vector<std::string> OperationLog() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return operationLog;
    }

    bool supported = true;
    bool failRegister = false;
    bool failNextShow = false;
    bool failAfterNextShow = false;
    bool throwAfterNextShow = false;
    bool failNextRemove = false;
    bool activateInsideShow = false;
    bool activateInsideRemove = false;
    bool rejectRemoveWhenUnregistered = false;
    bool throwNextUnregister = false;
    bool activationPresentDuringRegister = false;
    bool registered = false;
    unsigned supportCalls = 0;
    unsigned registerCalls = 0;
    unsigned unregisterCalls = 0;
    unsigned showCalls = 0;
    unsigned removeCalls = 0;
    std::vector<wxWinUIAppNotificationPayload> shown;
    std::vector<std::uint32_t> shownIds;
    std::vector<std::uint32_t> removedIds;

private:
    mutable std::mutex m_mutex;
    NativeCallBarrier m_registerBarrier;
    NativeCallBarrier m_showBarrier;
    wxWinUIAppNotificationActivation m_activation;
    std::function<void ()> m_registerHook;
    std::function<void ()> m_showHook;
    std::function<void ()> m_removeHook;
    std::function<void ()> m_unregisterHook;
    std::map<std::uint32_t, std::uint64_t> m_generationsById;
    std::vector<std::string> operationLog;
    std::uint32_t nextId = 1;
};

class NotificationNativeScope final
{
public:
    NotificationNativeScope()
        : native(std::make_shared<FakeAppNotificationNative>())
    {
        wxWinUIResetAppNotificationDispatchHookForTesting();
        wxWinUIResetAppNotificationFinalizationForTesting();
        REQUIRE(wxWinUISetAppNotificationNativeForTesting(native));
    }

    ~NotificationNativeScope()
    {
        wxWinUIResetAppNotificationDispatchHookForTesting();
        DrainNotifications();
        wxWinUIResetAppNotificationFinalizationForTesting();
        const bool resetNative =
            wxWinUIResetAppNotificationNativeForTesting();
        CHECK(resetNative);
        DrainNotifications();
    }

    std::shared_ptr<FakeAppNotificationNative> native;
};

void RequireEnabled(const NotificationNativeScope& scope)
{
    REQUIRE(wxNotificationMessage::MSWUseToasts());
    REQUIRE(wxNotificationMessage::MSWUseToasts());
    CHECK(scope.native->registerCalls == 1);
    CHECK(scope.native->activationPresentDuringRegister);
    const auto snapshot =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(snapshot.enabled);
    CHECK(snapshot.registered);
}

} // anonymous namespace

TEST_CASE("WinUIAppNotification::Registration",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    SECTION("unsupported_and_registration_failure_leave_backend_disabled")
    {
        NotificationNativeScope scope;
        scope.native->supported = false;
        CHECK_FALSE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 0);

        scope.native->supported = true;
        scope.native->failRegister = true;
        CHECK_FALSE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 1);
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK_FALSE(snapshot.enabled);
        CHECK_FALSE(snapshot.registered);
    }

    SECTION("register_and_unregister_are_idempotent")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        const std::uint64_t serialBefore =
            wxWinUIGetAppNotificationSnapshotForTesting().shutdownSerial;
        wxWinUIShutdownAppNotificationsForTesting();
        const std::uint64_t serialAfterFirst =
            wxWinUIGetAppNotificationSnapshotForTesting().shutdownSerial;
        wxWinUIShutdownAppNotificationsForTesting();
        const std::uint64_t serialAfterNoOp =
            wxWinUIGetAppNotificationSnapshotForTesting().shutdownSerial;
        CHECK(scope.native->unregisterCalls == 1);
        CHECK(serialAfterFirst > serialBefore);
        CHECK(serialAfterNoOp > serialAfterFirst);
        CHECK_FALSE(
            wxWinUIGetAppNotificationSnapshotForTesting().registered);

        // Models Explorer/native-manager restart without using a system
        // notification surface.
        REQUIRE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 2);
        CHECK(wxWinUIGetAppNotificationSnapshotForTesting().dispatchEnabled);
    }

    SECTION("recursive_enable_does_not_start_a_second_register")
    {
        NotificationNativeScope scope;
        bool nestedEnabled = true;
        scope.native->SetRegisterHook(
            [&]()
            {
                nestedEnabled = wxNotificationMessage::MSWUseToasts();
            });

        REQUIRE(wxNotificationMessage::MSWUseToasts());
        CHECK_FALSE(nestedEnabled);
        CHECK(scope.native->registerCalls == 1);
        CHECK(scope.native->activationPresentDuringRegister);
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK(snapshot.enabled);
        CHECK(snapshot.registered);

        REQUIRE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 1);
    }

    SECTION("same_thread_shutdown_during_register_revokes_publish_authority")
    {
        NotificationNativeScope scope;
        const std::uint64_t serialBefore =
            wxWinUIGetAppNotificationSnapshotForTesting().shutdownSerial;
        scope.native->SetRegisterHook(
            []() { wxWinUIShutdownAppNotificationsForTesting(); });

        CHECK_FALSE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 1);
        CHECK(scope.native->unregisterCalls == 1);
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK(snapshot.shutdownSerial > serialBefore);
        CHECK_FALSE(snapshot.enabled);
        CHECK_FALSE(snapshot.registered);
        CHECK_FALSE(snapshot.dispatchEnabled);
    }

    SECTION("shutdown_wins_a_register_already_inside_the_native_boundary")
    {
        NotificationNativeScope scope;
        scope.native->BlockRegister();
        std::atomic<bool> registerEntered{false};
        std::atomic<bool> shutdownObserved{false};

        std::thread coordinator(
            [&]()
            {
                registerEntered.store(scope.native->WaitForBlockedRegister());
                if ( !registerEntered.load() )
                {
                    scope.native->ReleaseRegister();
                    return;
                }

                std::thread shutdown(
                    []() { wxWinUIShutdownAppNotificationsForTesting(); });
                shutdownObserved.store(WaitForShutdownIntent());
                scope.native->ReleaseRegister();
                shutdown.join();
            });

        const bool enabled = wxNotificationMessage::MSWUseToasts();
        coordinator.join();

        REQUIRE(registerEntered.load());
        REQUIRE(shutdownObserved.load());
        CHECK_FALSE(enabled);
        CHECK(scope.native->registerCalls == 1);
        CHECK(scope.native->unregisterCalls == 1);
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK_FALSE(snapshot.enabled);
        CHECK_FALSE(snapshot.registered);
        CHECK_FALSE(snapshot.dispatchEnabled);
    }

    SECTION("throwing_revoke_still_allows_a_clean_reenable")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        scope.native->throwNextUnregister = true;
        wxWinUIShutdownAppNotificationsForTesting();

        const auto stopped =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK_FALSE(stopped.enabled);
        CHECK_FALSE(stopped.registered);
        CHECK_FALSE(stopped.dispatchEnabled);
        CHECK(scope.native->unregisterCalls == 1);

        REQUIRE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == 2);
        CHECK(wxWinUIGetAppNotificationSnapshotForTesting().registered);
    }

    SECTION("finalize_is_a_permanent_production_latch")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        const unsigned registerCalls = scope.native->registerCalls;

        wxWinUIFinalizeAppNotificationsForTesting();
        const auto finalized =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK(finalized.finalized);
        CHECK_FALSE(finalized.enabled);
        CHECK_FALSE(finalized.registered);
        CHECK_FALSE(finalized.dispatchEnabled);
        CHECK(scope.native->unregisterCalls == 1);

        CHECK_FALSE(wxNotificationMessage::MSWUseToasts());
        CHECK(scope.native->registerCalls == registerCalls);
        CHECK(wxWinUIGetAppNotificationSnapshotForTesting().finalized);
    }
}

TEST_CASE("WinUIAppNotification::ShowReplaceClose",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);

    wxNotificationMessage notification("title", "message");
    REQUIRE(notification.AddAction(wxID_OK));
    int dismissals = 0;
    int dismissalReason = -1;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_DISMISSED,
        [&](wxCommandEvent& event)
        {
            ++dismissals;
            dismissalReason = event.GetInt();
        });

    REQUIRE(notification.Show(1000000000));
    const auto first = scope.native->LastPayload();
    CHECK(first.timeout == 7 * 24 * 60 * 60);
    REQUIRE(first.actions.size() == 1);
    CHECK(first.actions.front().id == wxID_OK);
    CHECK_FALSE(first.actions.front().label.empty());

    REQUIRE(notification.Show(wxNotificationMessage::Timeout_Never));
    const auto second = scope.native->LastPayload();
    CHECK(second.generation > first.generation);
    REQUIRE(scope.native->removedIds.size() == 1);
    CHECK(scope.native->removedIds.front() == 1);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 1);

    scope.native->failNextRemove = true;
    CHECK_FALSE(notification.Close());
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 1);
    REQUIRE(notification.Close());
    // The first Close() already committed its dismissal delivery. A second,
    // physically empty Close() must not revoke that queued event.
    CHECK_FALSE(notification.Close());
    REQUIRE(DrainNotificationsUntil([&]() { return dismissals == 1; }));
    CHECK(dismissalReason == static_cast<int>(
        wxNotificationMessage::DismissalReason::ByApp));
    CHECK(scope.native->removedIds.back() == 2);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);
    CHECK_FALSE(notification.Close());
}

TEST_CASE("WinUIAppNotification::SerializedTransactions",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    SECTION("shutdown_wins_a_show_already_inside_the_native_boundary")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        wxNotificationMessage notification("blocked show");
        scope.native->BlockShow();
        std::atomic<bool> showEntered{false};
        std::atomic<bool> shutdownObserved{false};

        std::thread coordinator(
            [&]()
            {
                showEntered.store(scope.native->WaitForBlockedShow());
                if ( !showEntered.load() )
                {
                    scope.native->ReleaseShow();
                    return;
                }

                std::thread shutdown(
                    []() { wxWinUIShutdownAppNotificationsForTesting(); });
                shutdownObserved.store(WaitForShutdownIntent());
                scope.native->ReleaseShow();
                shutdown.join();
            });

        const bool shown = notification.Show();
        coordinator.join();

        REQUIRE(showEntered.load());
        REQUIRE(shutdownObserved.load());
        CHECK_FALSE(shown);
        REQUIRE(scope.native->shownIds.size() == 1);
        REQUIRE(scope.native->removedIds.size() == 1);
        CHECK(scope.native->removedIds.front() ==
              scope.native->shownIds.front());
        const auto snapshot =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK_FALSE(snapshot.enabled);
        CHECK_FALSE(snapshot.registered);
        CHECK(snapshot.activeGenerationCount == 0);
    }

    SECTION("nested_show_is_the_only_committed_writer")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        wxNotificationMessage notification("nested show");
        bool nestedResult = false;
        scope.native->SetShowHook(
            [&]() { nestedResult = notification.Show(); });

        CHECK_FALSE(notification.Show());
        REQUIRE(nestedResult);
        REQUIRE(scope.native->shown.size() == 2);
        REQUIRE(scope.native->shownIds.size() == 2);
        REQUIRE(scope.native->removedIds.size() == 1);
        CHECK(scope.native->removedIds.front() ==
              scope.native->shownIds.front());
        CHECK(scope.native->shown[1].generation >
              scope.native->shown[0].generation);
        CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
                  .activeGenerationCount == 1);

        int clicks = 0;
        notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
            [&](wxCommandEvent&) { ++clicks; });
        scope.native->Activate(
            scope.native->shown[1].generation,
            wxWinUIAppNotificationActivationKind::Click);
        REQUIRE(DrainNotificationsUntil([&]() { return clicks == 1; }));
    }

    SECTION("nested_close_cancels_an_unpublished_show")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        wxNotificationMessage notification("show versus close");
        bool closeResult = true;
        scope.native->SetShowHook(
            [&]() { closeResult = notification.Close(); });

        CHECK_FALSE(notification.Show());
        CHECK_FALSE(closeResult);
        REQUIRE(scope.native->shownIds.size() == 1);
        REQUIRE(scope.native->removedIds.size() == 1);
        CHECK(scope.native->removedIds.front() ==
              scope.native->shownIds.front());
        CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
                  .activeGenerationCount == 0);
    }
}

TEST_CASE("WinUIAppNotification::CloseOwnsSynchronousActivation",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("close authority");
    int clicks = 0;
    int dismissals = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_DISMISSED,
        [&](wxCommandEvent&) { ++dismissals; });

    // If removal fails, the physical activation is the sole authority.
    REQUIRE(notification.Show());
    scope.native->activateInsideRemove = true;
    scope.native->failNextRemove = true;
    CHECK_FALSE(notification.Close());
    REQUIRE(DrainNotificationsUntil([&]() { return clicks == 1; }));
    DrainNotifications();
    CHECK(dismissals == 0);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    // If removal succeeds, the app dismissal wins and the same synchronous
    // native activation must not leak a second CLICK event.
    REQUIRE(notification.Show());
    scope.native->activateInsideRemove = true;
    REQUIRE(notification.Close());
    REQUIRE(DrainNotificationsUntil([&]() { return dismissals == 1; }));
    DrainNotifications();
    CHECK(clicks == 1);
    CHECK(dismissals == 1);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);
}

TEST_CASE("WinUIAppNotification::ActivationAndGeneration",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);

    wxNotificationMessage notification("activation");
    REQUIRE(notification.AddAction(wxID_APPLY, "Apply"));
    std::atomic<int> clicks{0};
    std::atomic<int> actions{0};
    std::atomic<int> actionId{wxID_NONE};
    std::atomic<bool> callbackOnGui{false};
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&)
        {
            callbackOnGui.store(wxIsMainThread());
            ++clicks;
        });
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_ACTION,
        [&](wxCommandEvent& event)
        {
            callbackOnGui.store(wxIsMainThread());
            actionId.store(event.GetId());
            ++actions;
        });

    REQUIRE(notification.Show());
    const std::uint64_t stale = scope.native->LastPayload().generation;
    REQUIRE(notification.Show());
    const std::uint64_t current = scope.native->LastPayload().generation;
    scope.native->Activate(
        stale, wxWinUIAppNotificationActivationKind::Click);
    DrainNotifications();
    CHECK(clicks.load() == 0);

    std::thread worker(
        [native = scope.native, current]()
        {
            native->Activate(
                current, wxWinUIAppNotificationActivationKind::Click);
        });
    worker.join();
    // Activation has committed a delivery and retired its native generation.
    // Close() is now a true no-op and must leave that delivery authoritative.
    CHECK_FALSE(notification.Close());
    REQUIRE(DrainNotificationsUntil([&]() { return clicks.load() == 1; }));
    CHECK(callbackOnGui.load());
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    REQUIRE(notification.Show());
    const std::uint64_t actionGeneration =
        scope.native->LastPayload().generation;
    scope.native->Activate(actionGeneration,
                           wxWinUIAppNotificationActivationKind::Action,
                           wxID_APPLY);
    REQUIRE(DrainNotificationsUntil([&]() { return actions.load() == 1; }));
    CHECK(actionId.load() == wxID_APPLY);
    CHECK(callbackOnGui.load());
}

TEST_CASE("WinUIAppNotification::FailureAndSynchronousActivation",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("failure");
    int clicks = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });

    REQUIRE(notification.Show());
    const std::uint64_t first = scope.native->LastPayload().generation;
    // The replacement cannot retire id 1. It must roll id 2 back, preserve
    // generation 1 as the only authority and report failure.
    scope.native->failNextRemove = true;
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->removedIds.size() == 1);
    CHECK(scope.native->removedIds.front() == 2);
    scope.native->Activate(
        first, wxWinUIAppNotificationActivationKind::Click);
    REQUIRE(DrainNotificationsUntil([&]() { return clicks == 1; }));

    scope.native->failNextShow = true;
    CHECK_FALSE(notification.Show());
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    // A native boundary can make a surface visible, assign its id, and only
    // then report failure (or throw). Both paths must remove that exact id.
    scope.native->failAfterNextShow = true;
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->shownIds.size() >= 3);
    CHECK(scope.native->removedIds.back() == scope.native->shownIds.back());
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    scope.native->throwAfterNextShow = true;
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->shownIds.size() >= 4);
    CHECK(scope.native->removedIds.back() == scope.native->shownIds.back());
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    // The new generation remains authoritative even if activation arrives
    // synchronously before Show() publishes its native id.
    scope.native->activateInsideShow = true;
    REQUIRE(notification.Show());
    REQUIRE(DrainNotificationsUntil([&]() { return clicks == 2; }));
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);
}

TEST_CASE("WinUIAppNotification::QueuedDeliveryAndActionAuthority",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("delivery tokens");
    REQUIRE(notification.AddAction(wxID_APPLY, "Apply"));
    int clicks = 0;
    int actions = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_ACTION,
        [&](wxCommandEvent&) { ++actions; });

    REQUIRE(notification.Show());
    const auto first = scope.native->LastPayload();
    scope.native->Activate(
        first.generation, wxWinUIAppNotificationActivationKind::Click);

    // Do not pump: Show() must invalidate this already-marshalled callback.
    REQUIRE(notification.Show());
    const auto second = scope.native->LastPayload();
    DrainNotifications();
    CHECK(clicks == 0);

    // An action not published by this exact generation is ignored without
    // consuming the legitimate generation.
    scope.native->Activate(second.generation,
                           wxWinUIAppNotificationActivationKind::Action,
                           wxID_CANCEL);
    DrainNotifications();
    CHECK(actions == 0);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 1);
    scope.native->Activate(second.generation,
                           wxWinUIAppNotificationActivationKind::Action,
                           wxID_APPLY);
    REQUIRE(DrainNotificationsUntil([&]() { return actions == 1; }));
}

TEST_CASE("WinUIAppNotification::DestructionAndShutdown",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    int callbacks = 0;
    std::uint64_t generation = 0;
    {
        wxNotificationMessage notification("fire and forget");
        notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
            [&](wxCommandEvent&) { ++callbacks; });
        REQUIRE(notification.Show());
        generation = scope.native->LastPayload().generation;
    }

    CHECK(scope.native->removedIds.empty());
    scope.native->Activate(
        generation, wxWinUIAppNotificationActivationKind::Click);
    DrainNotifications();
    CHECK(callbacks == 0);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .activeGenerationCount == 0);

    wxNotificationMessage notification("shutdown");
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++callbacks; });
    REQUIRE(notification.Show());
    generation = scope.native->LastPayload().generation;
    const auto lateActivation = scope.native->CopyActivation();
    REQUIRE(static_cast<bool>(lateActivation));
    wxWinUIShutdownAppNotificationsForTesting();
    lateActivation(generation,
                   wxWinUIAppNotificationActivationKind::Click,
                   wxID_NONE);
    DrainNotifications();
    CHECK(callbacks == 0);
    CHECK(scope.native->unregisterCalls == 1);
}

TEST_CASE("WinUIAppNotification::CleanupOnlyLedger",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    std::uint32_t shownId = 0;
    {
        wxNotificationMessage notification("failed compensation");
        scope.native->failAfterNextShow = true;
        scope.native->failNextRemove = true;

        CHECK_FALSE(notification.Show());
        REQUIRE(scope.native->shownIds.size() == 1);
        shownId = scope.native->shownIds.front();
        const auto retained =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK(retained.cleanupOnlyCount == 1);
        CHECK(retained.activeGenerationCount == 1);
        CHECK(scope.native->removedIds.empty());
    }

    // Wrapper destruction retries only the failed compensation. A successful
    // fire-and-forget Show follows the separate DestructionAndShutdown
    // contract and is deliberately never removed by DestroyState().
    const auto cleaned = wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(cleaned.cleanupOnlyCount == 0);
    CHECK(cleaned.activeGenerationCount == 0);
    REQUIRE(scope.native->removedIds.size() == 1);
    CHECK(scope.native->removedIds.front() == shownId);

    wxWinUIShutdownAppNotificationsForTesting();
    const std::vector<std::string> expected = {
        "Register",
        "Show " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "Remove " + std::to_string(shownId),
        "Unregister"
    };
    CHECK(scope.native->OperationLog() == expected);
    const auto stopped = wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(stopped.cleanupOnlyCount == 0);
    CHECK(stopped.nativeShowLeaseCount == 0);
}

TEST_CASE("WinUIAppNotification::CleanupOnlySurvivesWrapper",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    int callbacks = 0;
    std::uint64_t generation = 0;
    std::uint32_t shownId = 0;
    {
        wxNotificationMessage notification("persistent cleanup ownership");
        notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
            [&](wxCommandEvent&) { ++callbacks; });
        scope.native->failAfterNextShow = true;
        scope.native->failNextRemove = true;
        CHECK_FALSE(notification.Show());
        REQUIRE(scope.native->shownIds.size() == 1);
        shownId = scope.native->shownIds.front();
        generation = scope.native->LastPayload().generation;
        REQUIRE(wxWinUIGetAppNotificationSnapshotForTesting()
                    .cleanupOnlyCount == 1);

        // DestroyState performs an immediate exact retry; fail it as well so
        // the proof crosses the wx wrapper's lifetime boundary.
        scope.native->failNextRemove = true;
    }

    const auto orphanOwned =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(orphanOwned.cleanupOnlyCount == 1);
    CHECK(orphanOwned.activeGenerationCount == 0);
    CHECK(scope.native->removedIds.empty());

    scope.native->Activate(
        generation, wxWinUIAppNotificationActivationKind::Click);
    DrainNotifications();
    CHECK(callbacks == 0);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .cleanupOnlyCount == 1);

    scope.native->rejectRemoveWhenUnregistered = true;
    wxWinUIShutdownAppNotificationsForTesting();
    REQUIRE(scope.native->removedIds.size() == 1);
    CHECK(scope.native->removedIds.front() == shownId);
    const std::vector<std::string> expected = {
        "Register",
        "Show " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "Remove " + std::to_string(shownId),
        "Unregister"
    };
    CHECK(scope.native->OperationLog() == expected);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .cleanupOnlyCount == 0);
}

TEST_CASE("WinUIAppNotification::NativeReplacementRetainsCleanupAuthority",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("replacement cleanup authority");
    scope.native->failAfterNextShow = true;
    scope.native->failNextRemove = true;
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->shownIds.size() == 1);
    const std::uint32_t shownId = scope.native->shownIds.front();
    REQUIRE(wxWinUIGetAppNotificationSnapshotForTesting()
                .cleanupOnlyCount == 1);

    const auto replacement =
        std::make_shared<FakeAppNotificationNative>();

    // The preflight retry must fail closed. In particular, it must not
    // unregister or swap out the only manager which can still remove the id.
    scope.native->failNextRemove = true;
    CHECK_FALSE(wxWinUISetAppNotificationNativeForTesting(replacement));
    const auto retained =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(retained.enabled);
    CHECK(retained.registered);
    CHECK(retained.cleanupOnlyCount == 1);
    CHECK(scope.native->registered);
    CHECK(scope.native->unregisterCalls == 0);
    CHECK(replacement->registerCalls == 0);

    // Once the exact old id can be removed, replacement is ordered as
    // Remove(old) -> Unregister(old) -> publication of the new boundary.
    REQUIRE(wxWinUISetAppNotificationNativeForTesting(replacement));
    CHECK_FALSE(scope.native->registered);
    CHECK(scope.native->unregisterCalls == 1);
    REQUIRE(scope.native->removedIds.size() == 1);
    CHECK(scope.native->removedIds.front() == shownId);
    const std::vector<std::string> expectedOld = {
        "Register",
        "Show " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "Remove " + std::to_string(shownId),
        "Unregister"
    };
    CHECK(scope.native->OperationLog() == expectedOld);

    const auto replaced =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK_FALSE(replaced.enabled);
    CHECK_FALSE(replaced.registered);
    CHECK(replaced.cleanupOnlyCount == 0);
    REQUIRE(wxNotificationMessage::MSWUseToasts());
    CHECK(replacement->registerCalls == 1);
    CHECK(replacement->registered);
}

TEST_CASE("WinUIAppNotification::NativeReplacementLosesToFinalization",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("replacement versus finalization");
    scope.native->failAfterNextShow = true;
    scope.native->failNextRemove = true;
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->shownIds.size() == 1);
    const std::uint32_t shownId = scope.native->shownIds.front();

    std::atomic<bool> finalizeReturned{false};
    scope.native->SetRemoveHook(
        [&]()
        {
            wxWinUIFinalizeAppNotificationsForTesting();
            finalizeReturned.store(true);
        });
    const auto replacement =
        std::make_shared<FakeAppNotificationNative>();

    // Finalize() re-enters from the exact successful preflight Remove(). Its
    // permanent latch wins before replacement publication: the old boundary
    // is retained even though finalization has already unregistered it.
    CHECK_FALSE(wxWinUISetAppNotificationNativeForTesting(replacement));
    REQUIRE(finalizeReturned.load());
    const auto finalized =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(finalized.finalized);
    CHECK_FALSE(finalized.shuttingDown);
    CHECK_FALSE(finalized.enabled);
    CHECK_FALSE(finalized.registered);
    CHECK(finalized.cleanupOnlyCount == 0);
    CHECK_FALSE(wxNotificationMessage::MSWUseToasts());
    CHECK(replacement->registerCalls == 0);
    const std::vector<std::string> expectedOld = {
        "Register",
        "Show " + std::to_string(shownId),
        "RemoveFail " + std::to_string(shownId),
        "Remove " + std::to_string(shownId),
        "Unregister"
    };
    CHECK(scope.native->OperationLog() == expectedOld);

    // Reset only the test latch, then prove identity by re-enabling: Register
    // must target the retained old boundary, never the rejected replacement.
    wxWinUIResetAppNotificationFinalizationForTesting();
    REQUIRE(wxNotificationMessage::MSWUseToasts());
    CHECK(scope.native->registerCalls == 2);
    CHECK(scope.native->registered);
    CHECK(replacement->registerCalls == 0);
}

TEST_CASE("WinUIAppNotification::NativeReplacementRefusesDispatchLease",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);

    wxNotificationMessage leasedNotification("leased dispatch");
    REQUIRE(leasedNotification.Show());
    const std::uint64_t leasedGeneration =
        scope.native->LastPayload().generation;

    wxNotificationMessage cleanupNotification("cleanup during dispatch");
    scope.native->failAfterNextShow = true;
    scope.native->failNextRemove = true;
    CHECK_FALSE(cleanupNotification.Show());
    REQUIRE(wxWinUIGetAppNotificationSnapshotForTesting()
                .cleanupOnlyCount == 1);

    DispatchBarrier barrier;
    std::atomic<bool> closeReturned{false};
    std::atomic<bool> closeResult{true};
    gs_dispatchBarrier = &barrier;
    gs_dispatchCloseNotification = &leasedNotification;
    gs_dispatchCloseReturned = &closeReturned;
    gs_dispatchCloseResult = &closeResult;
    wxWinUISetAppNotificationDispatchHookForTesting(
        &BlockThenCloseDuringNotificationDispatchForTesting);

    std::thread callback(
        [native = scope.native, leasedGeneration]()
        {
            native->Activate(
                leasedGeneration,
                wxWinUIAppNotificationActivationKind::Click);
        });

    bool entered = false;
    {
        std::unique_lock<std::mutex> lock(barrier.mutex);
        entered = barrier.changed.wait_for(
            lock, std::chrono::seconds(2),
            [&barrier]() { return barrier.entered; });
        if ( !entered )
        {
            barrier.release = true;
            barrier.changed.notify_all();
        }
    }
    if ( !entered )
    {
        callback.join();
        wxWinUIResetAppNotificationDispatchHookForTesting();
        gs_dispatchBarrier = nullptr;
        gs_dispatchCloseNotification = nullptr;
        gs_dispatchCloseReturned = nullptr;
        gs_dispatchCloseResult = nullptr;
    }
    REQUIRE(entered);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .dispatchLeaseCount == 1);

    std::atomic<bool> removeHookEntered{false};
    std::atomic<bool> nestedShutdownReturned{false};
    scope.native->SetRemoveHook(
        [&]()
        {
            removeHookEntered.store(true);
            wxWinUIShutdownAppNotificationsForTesting();
            nestedShutdownReturned.store(true);
        });
    const unsigned removeCallsBefore = scope.native->removeCalls;
    const auto replacement =
        std::make_shared<FakeAppNotificationNative>();

    // Calling Remove() while retaining an outer operation-lock level would
    // deadlock if its hook waited for this lease and the leased callback then
    // entered Close(). Replacement must therefore fail before native Remove.
    CHECK_FALSE(wxWinUISetAppNotificationNativeForTesting(replacement));
    CHECK_FALSE(removeHookEntered.load());
    CHECK(scope.native->removeCalls == removeCallsBefore);
    CHECK(replacement->registerCalls == 0);

    {
        std::lock_guard<std::mutex> lock(barrier.mutex);
        barrier.release = true;
        barrier.changed.notify_all();
    }
    callback.join();
    wxWinUIResetAppNotificationDispatchHookForTesting();
    gs_dispatchBarrier = nullptr;
    gs_dispatchCloseNotification = nullptr;
    gs_dispatchCloseReturned = nullptr;
    gs_dispatchCloseResult = nullptr;
    DrainNotifications();

    // Close() now owns the lease itself, so the reentrant Shutdown() defers
    // native completion until that lease is released instead of waiting.
    REQUIRE(closeReturned.load());
    CHECK_FALSE(closeResult.load());
    REQUIRE(removeHookEntered.load());
    REQUIRE(nestedShutdownReturned.load());
    const auto stopped =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK_FALSE(stopped.shuttingDown);
    CHECK_FALSE(stopped.enabled);
    CHECK_FALSE(stopped.registered);
    CHECK(stopped.cleanupOnlyCount == 0);
    CHECK(stopped.dispatchLeaseCount == 0);
    CHECK(scope.native->unregisterCalls == 1);
    CHECK(replacement->registerCalls == 0);
}

TEST_CASE("WinUIAppNotification::DestroyedOwner",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxFrame* const owner = new wxFrame(
        nullptr, wxID_ANY, "notification owner",
        wxPoint(-30000, -30000), wxSize(160, 100));
    const wxWeakRef<wxWindow> weakOwner(owner);
    wxNotificationMessage notification("owner", wxString(), owner);
    int clicks = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });
    REQUIRE(notification.Show());
    const std::uint64_t generation =
        scope.native->LastPayload().generation;

    owner->Destroy();
    REQUIRE(DrainNotificationsUntil([weakOwner]() { return !weakOwner; }));
    scope.native->Activate(
        generation, wxWinUIAppNotificationActivationKind::Click);
    REQUIRE(DrainNotificationsUntil([&]() { return clicks == 1; }));
}

TEST_CASE("WinUIAppNotification::ShutdownWaitsForDispatchLease",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("dispatch authority");
    int clicks = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });
    REQUIRE(notification.Show());
    const std::uint64_t generation =
        scope.native->LastPayload().generation;

    DispatchBarrier barrier;
    std::atomic<bool> closeReturned{false};
    std::atomic<bool> closeResult{true};
    gs_dispatchBarrier = &barrier;
    gs_dispatchCloseNotification = &notification;
    gs_dispatchCloseReturned = &closeReturned;
    gs_dispatchCloseResult = &closeResult;
    wxWinUISetAppNotificationDispatchHookForTesting(
        &BlockThenCloseDuringNotificationDispatchForTesting);

    std::thread callback(
        [native = scope.native, generation]()
        {
            native->Activate(
                generation,
                wxWinUIAppNotificationActivationKind::Click);
        });

    bool entered = false;
    {
        std::unique_lock<std::mutex> lock(barrier.mutex);
        entered = barrier.changed.wait_for(
            lock, std::chrono::seconds(2),
            [&barrier]() { return barrier.entered; });
        if ( !entered )
        {
            barrier.release = true;
            barrier.changed.notify_all();
        }
    }
    if ( !entered )
    {
        callback.join();
        wxWinUIResetAppNotificationDispatchHookForTesting();
        gs_dispatchBarrier = nullptr;
        gs_dispatchCloseNotification = nullptr;
        gs_dispatchCloseReturned = nullptr;
        gs_dispatchCloseResult = nullptr;
    }
    REQUIRE(entered);
    CHECK(wxWinUIGetAppNotificationSnapshotForTesting()
              .dispatchLeaseCount == 1);

    std::atomic<bool> shutdownObserved{false};
    std::thread release(
        [&barrier, &shutdownObserved]()
        {
            shutdownObserved.store(WaitForShutdownRevocation());
            std::lock_guard<std::mutex> lock(barrier.mutex);
            barrier.release = true;
            barrier.changed.notify_all();
        });

    // Shutdown runs on the GUI thread, revokes dispatch authority first and
    // releases the native-operation mutex before waiting. The leased worker
    // must therefore be able to enter Close(), observe revocation and return;
    // holding that mutex across the wait would deadlock this exact sequence.
    wxWinUIShutdownAppNotificationsForTesting();
    release.join();
    callback.join();
    wxWinUIResetAppNotificationDispatchHookForTesting();
    gs_dispatchBarrier = nullptr;
    gs_dispatchCloseNotification = nullptr;
    gs_dispatchCloseReturned = nullptr;
    gs_dispatchCloseResult = nullptr;
    DrainNotifications();

    const auto after = wxWinUIGetAppNotificationSnapshotForTesting();
    REQUIRE(shutdownObserved.load());
    REQUIRE(closeReturned.load());
    CHECK_FALSE(closeResult.load());
    CHECK_FALSE(after.dispatchEnabled);
    CHECK(after.dispatchLeaseCount == 0);
    CHECK(clicks == 0);
    CHECK(scope.native->unregisterCalls == 1);
}

TEST_CASE("WinUIAppNotification::ReentrantShutdownDefersItsOwnLease",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    wxNotificationMessage notification("reentrant dispatch shutdown");
    int clicks = 0;
    notification.Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK,
        [&](wxCommandEvent&) { ++clicks; });
    REQUIRE(notification.Show());
    const std::uint64_t generation =
        scope.native->LastPayload().generation;

    std::atomic<bool> shutdownReturned{false};
    gs_reentrantShutdownReturned = &shutdownReturned;
    wxWinUISetAppNotificationDispatchHookForTesting(
        &ShutdownDuringNotificationDispatchForTesting);

    // This runs on the same thread that owns QueueEvent()'s dispatch lease.
    // Shutdown must revoke synchronously but defer its native completion until
    // QueueEvent() releases the lease, instead of waiting on itself.
    scope.native->Activate(
        generation, wxWinUIAppNotificationActivationKind::Click);

    wxWinUIResetAppNotificationDispatchHookForTesting();
    gs_reentrantShutdownReturned = nullptr;
    REQUIRE(shutdownReturned.load());
    DrainNotifications();

    const auto stopped = wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK_FALSE(stopped.shuttingDown);
    CHECK_FALSE(stopped.enabled);
    CHECK_FALSE(stopped.registered);
    CHECK_FALSE(stopped.dispatchEnabled);
    CHECK(stopped.dispatchLeaseCount == 0);
    CHECK(stopped.queuedCallbackCount == 0);
    CHECK(clicks == 0);
    CHECK(scope.native->unregisterCalls == 1);

    // Ordinary Shutdown() remains restartable after the deferred transaction.
    REQUIRE(wxNotificationMessage::MSWUseToasts());
    CHECK(scope.native->registerCalls == 2);
}

TEST_CASE("WinUIAppNotification::ShowLeaseDefersFinalization",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    scope.native->rejectRemoveWhenUnregistered = true;
    scope.native->SetShowHook(
        []()
        {
            wxWinUIShutdownAppNotificationsForTesting();
            wxWinUIFinalizeAppNotificationsForTesting();
        });

    wxNotificationMessage notification("reentrant Show finalization");
    CHECK_FALSE(notification.Show());
    REQUIRE(scope.native->shownIds.size() == 1);
    const std::uint32_t shownId = scope.native->shownIds.front();

    const std::vector<std::string> expected = {
        "Register",
        "Show " + std::to_string(shownId),
        "Remove " + std::to_string(shownId),
        "Unregister"
    };
    CHECK(scope.native->OperationLog() == expected);
    const auto finalized =
        wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(finalized.finalized);
    CHECK_FALSE(finalized.shuttingDown);
    CHECK_FALSE(finalized.enabled);
    CHECK_FALSE(finalized.registered);
    CHECK(finalized.nativeShowLeaseCount == 0);
    CHECK(finalized.dispatchLeaseCount == 0);
    CHECK(finalized.cleanupOnlyCount == 0);
}

TEST_CASE("WinUIAppNotification::ShutdownCompletionReentrancy",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    SECTION("cleanup_remove_can_reenter_shutdown")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        wxNotificationMessage notification("reentrant cleanup");
        scope.native->failAfterNextShow = true;
        scope.native->failNextRemove = true;
        CHECK_FALSE(notification.Show());
        REQUIRE(scope.native->shownIds.size() == 1);
        const std::uint32_t shownId = scope.native->shownIds.front();
        REQUIRE(wxWinUIGetAppNotificationSnapshotForTesting()
                    .cleanupOnlyCount == 1);

        std::atomic<bool> nestedReturned{false};
        scope.native->rejectRemoveWhenUnregistered = true;
        scope.native->SetRemoveHook(
            [&]()
            {
                wxWinUIShutdownAppNotificationsForTesting();
                nestedReturned.store(true);
            });

        wxWinUIShutdownAppNotificationsForTesting();
        REQUIRE(nestedReturned.load());
        const std::vector<std::string> expected = {
            "Register",
            "Show " + std::to_string(shownId),
            "RemoveFail " + std::to_string(shownId),
            "Remove " + std::to_string(shownId),
            "Unregister"
        };
        CHECK(scope.native->OperationLog() == expected);
        const auto stopped =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK_FALSE(stopped.shuttingDown);
        CHECK_FALSE(stopped.registered);
        CHECK(stopped.cleanupOnlyCount == 0);
        CHECK(scope.native->unregisterCalls == 1);
    }

    SECTION("unregister_can_reenter_shutdown_and_finalize")
    {
        NotificationNativeScope scope;
        RequireEnabled(scope);
        std::atomic<bool> nestedReturned{false};
        scope.native->SetUnregisterHook(
            [&]()
            {
                wxWinUIShutdownAppNotificationsForTesting();
                wxWinUIFinalizeAppNotificationsForTesting();
                nestedReturned.store(true);
            });

        wxWinUIShutdownAppNotificationsForTesting();
        REQUIRE(nestedReturned.load());
        const std::vector<std::string> expected = {
            "Register",
            "Unregister"
        };
        CHECK(scope.native->OperationLog() == expected);
        const auto finalized =
            wxWinUIGetAppNotificationSnapshotForTesting();
        CHECK(finalized.finalized);
        CHECK_FALSE(finalized.shuttingDown);
        CHECK_FALSE(finalized.registered);
        CHECK(finalized.cleanupOnlyCount == 0);
        CHECK(scope.native->unregisterCalls == 1);
    }
}

TEST_CASE("WinUIAppNotification::HundredCycles",
          "[winui-beta-aux][winui-beta-notifications][notification]")
{
    NotificationNativeScope scope;
    RequireEnabled(scope);
    const auto before = wxWinUIGetAppNotificationSnapshotForTesting();

    for ( int i = 0; i < 100; ++i )
    {
        wxNotificationMessage notification(
            wxString::Format("cycle %d", i));
        REQUIRE(notification.Show());
        const auto payload = scope.native->LastPayload();
        if ( i % 2 )
        {
            REQUIRE(notification.Close());
        }
        else
        {
            scope.native->Activate(
                payload.generation,
                wxWinUIAppNotificationActivationKind::Click);
        }
        DrainNotifications(1);
    }
    DrainNotifications();

    const auto after = wxWinUIGetAppNotificationSnapshotForTesting();
    CHECK(after.liveInstanceCount == before.liveInstanceCount);
    CHECK(after.activeGenerationCount == before.activeGenerationCount);
    CHECK(after.queuedCallbackCount == before.queuedCallbackCount);
}

#endif // WinUI AppNotification backend
