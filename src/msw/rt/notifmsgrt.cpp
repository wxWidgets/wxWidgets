/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/notifmsgrt.cpp
// Purpose:     WinRT implementation of wxNotificationMessageImpl
// Author:      Tobias Taschner
// Created:     2015-09-13
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/module.h"
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/msw/rt/private/notifmsg.h"

#include "wx/notifmsg.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/winui/private/notification.h"

#include "wx/app.h"
#include "wx/module.h"
#include "wx/scopeguard.h"
#include "wx/stockitem.h"
#include "wx/weakref.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#else // legacy WinRT toast backend

#include "wx/msw/rt/utils.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/wrapshl.h"

#include "wx/filename.h"
#include "wx/stdpaths.h"

#include <roapi.h>
#include <windows.ui.notifications.h>
#include <functiondiscoverykeys.h>
#include <propvarutil.h>
#include <wrl/implements.h>

#endif // __WXWINUI__ && wxUSE_WINUI3

#if defined(__WXWINUI__) && wxUSE_WINUI3

namespace
{

constexpr int wxAPP_NOTIFICATION_MAX_TIMEOUT_SECONDS = 7 * 24 * 60 * 60;
constexpr std::size_t wxAPP_NOTIFICATION_MAX_ARGUMENT_LENGTH = 256;

struct wxWinUIAppNotificationInstanceState
{
    explicit wxWinUIAppNotificationInstanceState(
        wxNotificationMessageBase* const notification)
        : notification(notification)
    {
    }

    wxWeakRef<wxNotificationMessageBase> notification;
    std::map<std::uint64_t, std::uint32_t> activeNotifications;
    std::map<std::uint64_t, std::uint64_t> pendingDeliveries;
    struct DeferredActivation
    {
        wxWinUIAppNotificationActivationKind kind =
            wxWinUIAppNotificationActivationKind::Click;
        wxWindowID actionId = wxID_NONE;
        std::uint64_t closeEpoch = 0;
    };
    std::map<std::uint64_t, std::uint64_t> closeAuthorities;
    std::map<std::uint64_t, DeferredActivation> deferredActivations;
    std::uint64_t lastDeliveryToken = 0;
    std::uint64_t requestEpoch = 0;
    std::size_t inFlightShowCount = 0;
    bool alive = true;
};

// QueueEvent() holds a process-wide dispatch lease while using its raw wxApp
// target. Track ownership per thread as well, so Shutdown() can distinguish a
// normal waiter from a callback that would otherwise wait for its own lease.
thread_local std::size_t gs_appNotificationDispatchLeaseDepth = 0;
// Show() must retain the native registration until its exact native result has
// either committed or been compensated. In particular, a synchronous native
// callback may re-enter Shutdown()/Finalize() before NativeShow() returns.
thread_local std::size_t gs_appNotificationNativeShowLeaseDepth = 0;
// CompleteShutdown() invokes Remove()/Unregister(), both native callback
// boundaries. A same-thread Shutdown()/Finalize() from either boundary must
// join the active completion instead of waiting for itself to clear it.
thread_local std::size_t gs_appNotificationShutdownCompletionDepth = 0;

struct wxWinUIAppNotificationGeneration
{
    std::weak_ptr<wxWinUIAppNotificationInstanceState> state;
    std::vector<wxWindowID> actionIds;
    // A generation whose compensating Remove() failed remains tracked so a
    // later Close() can retry it, but it no longer owns user-visible events.
    bool acceptsActivation = true;
    std::uint64_t cleanupToken = 0;
};

struct wxWinUIAppNotificationCleanupOnly
{
    std::shared_ptr<wxWinUIAppNotificationNative> native;
    std::weak_ptr<wxWinUIAppNotificationInstanceState> state;
    std::uint64_t generation = 0;
    std::uint32_t notificationId = 0;
    bool retrying = false;
};

class wxWinUIAppNotificationNativeImpl final
    : public wxWinUIAppNotificationNative,
      public std::enable_shared_from_this<wxWinUIAppNotificationNativeImpl>
{
public:
    bool IsSupported() override
    {
        try
        {
            return winrt::Microsoft::Windows::AppNotifications::
                AppNotificationManager::IsSupported();
        }
        catch (...)
        {
            return false;
        }
    }

    bool Register(wxWinUIAppNotificationActivation activation) override
    {
        if ( m_registered )
            return true;

        try
        {
            m_manager = winrt::Microsoft::Windows::AppNotifications::
                AppNotificationManager::Default();
            {
                std::lock_guard<std::mutex> lock(m_activationMutex);
                m_activation = std::move(activation);
            }

            const std::weak_ptr<wxWinUIAppNotificationNativeImpl> weakSelf =
                weak_from_this();
            m_invokedToken = m_manager.NotificationInvoked(
                [weakSelf](
                    const winrt::Microsoft::Windows::AppNotifications::
                        AppNotificationManager&,
                    const winrt::Microsoft::Windows::AppNotifications::
                        AppNotificationActivatedEventArgs& args)
                {
                    if ( const auto self = weakSelf.lock() )
                        self->Dispatch(args.Argument());
                });
            m_hasInvokedToken = true;

            // Subscribe first: Register() can make a pending cold activation
            // observable before it returns.
            try
            {
                m_manager.Register();
                m_registered = true;
                return true;
            }
            catch (...)
            {
                {
                    std::lock_guard<std::mutex> lock(m_activationMutex);
                    m_activation = {};
                }
                try
                {
                    m_manager.NotificationInvoked(m_invokedToken);
                }
                catch (...)
                {
                }
                try
                {
                    m_manager.Unregister();
                }
                catch (...)
                {
                }
                m_hasInvokedToken = false;
                m_registered = false;
                m_manager = nullptr;
                return false;
            }
        }
        catch (...)
        {
            {
                std::lock_guard<std::mutex> lock(m_activationMutex);
                m_activation = {};
            }
            if ( m_hasInvokedToken && m_manager )
            {
                try
                {
                    m_manager.NotificationInvoked(m_invokedToken);
                }
                catch (...)
                {
                }
            }
            if ( m_manager )
            {
                try
                {
                    m_manager.Unregister();
                }
                catch (...)
                {
                }
            }
            m_hasInvokedToken = false;
            m_registered = false;
            m_manager = nullptr;
            return false;
        }
    }

    void Unregister() override
    {
        // Invalidate the callback before revoking its token: a native callback
        // may already be in flight on another thread.
        {
            std::lock_guard<std::mutex> lock(m_activationMutex);
            m_activation = {};
        }

        if ( m_hasInvokedToken && m_manager )
        {
            try
            {
                m_manager.NotificationInvoked(m_invokedToken);
            }
            catch (...)
            {
            }
        }
        // State cleanup is unconditional: a revoke failure must never make
        // the next Register() believe that this object is still subscribed.
        m_hasInvokedToken = false;

        if ( m_registered && m_manager )
        {
            try
            {
                m_manager.Unregister();
            }
            catch (...)
            {
            }
        }

        m_registered = false;
        m_manager = nullptr;
    }

    bool Show(const wxWinUIAppNotificationPayload& payload,
              std::uint32_t& notificationId) override
    {
        if ( !m_registered || !m_manager )
            return false;

        try
        {
            namespace appnotif =
                winrt::Microsoft::Windows::AppNotifications;
            namespace builder =
                winrt::Microsoft::Windows::AppNotifications::Builder;

            const winrt::hstring generation(
                std::to_wstring(payload.generation));

            auto notificationBuilder = builder::AppNotificationBuilder()
                .AddArgument(L"wx-generation", generation)
                .AddText(winrt::hstring(payload.title.wc_str()));

            if ( !payload.message.empty() )
            {
                notificationBuilder.AddText(
                    winrt::hstring(payload.message.wc_str()));
            }

            for ( const auto& action : payload.actions )
            {
                const winrt::hstring actionId(
                    std::to_wstring(static_cast<long long>(action.id)));
                auto button = builder::AppNotificationButton(
                    winrt::hstring(action.label.wc_str()))
                    .AddArgument(L"wx-generation", generation)
                    .AddArgument(L"wx-action", actionId);
                notificationBuilder.AddButton(button);
            }

            appnotif::AppNotification notification =
                notificationBuilder.BuildNotification();

            if ( payload.timeout > 0 )
            {
                const int boundedTimeout = std::min(
                    payload.timeout,
                    wxAPP_NOTIFICATION_MAX_TIMEOUT_SECONDS);
                notification.Expiration(
                    winrt::clock::now() +
                    std::chrono::seconds(boundedTimeout));
            }

            // Timeout_Auto uses the platform policy. Timeout_Never deliberately
            // leaves Expiration unset: AppNotification has no contract for a
            // permanently visible heads-up notification.
            notificationId = notification.Id();
            if ( notificationId == 0 )
                return false;

            m_manager.Show(notification);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool Remove(const std::uint32_t notificationId) override
    {
        if ( !m_registered || !m_manager || notificationId == 0 )
            return false;

        try
        {
            // Starting the operation is the strongest synchronous guarantee
            // exposed by AppNotificationManager. Completion is intentionally
            // not blocked on the GUI thread.
            m_manager.RemoveByIdAsync(notificationId);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

private:
    static bool ParseUnsigned(const std::wstring_view text,
                              std::uint64_t& value)
    {
        if ( text.empty() || text.size() > 20 )
            return false;

        std::uint64_t parsed = 0;
        for ( const wchar_t ch : text )
        {
            if ( ch < L'0' || ch > L'9' )
                return false;

            const unsigned digit = static_cast<unsigned>(ch - L'0');
            if ( parsed >
                    (std::numeric_limits<std::uint64_t>::max() - digit) / 10 )
            {
                return false;
            }
            parsed = parsed * 10 + digit;
        }

        value = parsed;
        return true;
    }

    static bool ParseSigned(const std::wstring_view text,
                            wxWindowID& value)
    {
        if ( text.empty() || text.size() > 12 )
            return false;

        bool negative = false;
        std::size_t pos = 0;
        if ( text.front() == L'-' )
        {
            negative = true;
            pos = 1;
        }
        if ( pos == text.size() )
            return false;

        std::int64_t parsed = 0;
        for ( ; pos < text.size(); ++pos )
        {
            const wchar_t ch = text[pos];
            if ( ch < L'0' || ch > L'9' )
                return false;
            parsed = parsed * 10 + static_cast<int>(ch - L'0');
            if ( parsed > std::numeric_limits<int>::max() )
                return false;
        }

        const std::int64_t signedValue = negative ? -parsed : parsed;
        if ( signedValue < std::numeric_limits<int>::min() ||
             signedValue > std::numeric_limits<int>::max() )
        {
            return false;
        }

        value = static_cast<wxWindowID>(signedValue);
        return true;
    }

    static bool FindArgument(const std::wstring_view arguments,
                             const std::wstring_view key,
                             std::wstring_view& value)
    {
        std::size_t begin = 0;
        while ( begin <= arguments.size() )
        {
            const std::size_t end = arguments.find(L'&', begin);
            const std::wstring_view item = arguments.substr(
                begin,
                end == std::wstring_view::npos
                    ? std::wstring_view::npos
                    : end - begin);
            const std::size_t equals = item.find(L'=');
            if ( equals != std::wstring_view::npos &&
                 item.substr(0, equals) == key )
            {
                value = item.substr(equals + 1);
                return true;
            }

            if ( end == std::wstring_view::npos )
                break;
            begin = end + 1;
        }

        return false;
    }

    void Dispatch(const winrt::hstring& nativeArguments) const
    {
        if ( nativeArguments.size() >
                 wxAPP_NOTIFICATION_MAX_ARGUMENT_LENGTH )
        {
            return;
        }

        const std::wstring_view arguments(nativeArguments.c_str(),
                                          nativeArguments.size());
        std::wstring_view generationText;
        std::uint64_t generation = 0;
        if ( !FindArgument(arguments, L"wx-generation", generationText) ||
             !ParseUnsigned(generationText, generation) )
        {
            return;
        }

        std::wstring_view actionText;
        if ( FindArgument(arguments, L"wx-action", actionText) )
        {
            wxWindowID actionId = wxID_NONE;
            if ( ParseSigned(actionText, actionId) )
            {
                wxWinUIAppNotificationActivation activation;
                {
                    std::lock_guard<std::mutex> lock(m_activationMutex);
                    activation = m_activation;
                }
                if ( activation )
                {
                    activation(
                        generation,
                        wxWinUIAppNotificationActivationKind::Action,
                        actionId);
                }
            }
            return;
        }

        wxWinUIAppNotificationActivation activation;
        {
            std::lock_guard<std::mutex> lock(m_activationMutex);
            activation = m_activation;
        }
        if ( activation )
        {
            activation(generation,
                       wxWinUIAppNotificationActivationKind::Click,
                       wxID_NONE);
        }
    }

    winrt::Microsoft::Windows::AppNotifications::AppNotificationManager
        m_manager{nullptr};
    winrt::event_token m_invokedToken{};
    mutable std::mutex m_activationMutex;
    wxWinUIAppNotificationActivation m_activation;
    bool m_hasInvokedToken = false;
    bool m_registered = false;
};

class wxWinUIAppNotificationBroker final
{
public:
    static wxWinUIAppNotificationBroker& Get()
    {
        static wxWinUIAppNotificationBroker broker;
        return broker;
    }

    bool Enable()
    {
        if ( IsFinalizationRequested() )
            return false;

        wxApp* const dispatchTarget = wxTheApp;
        if ( !dispatchTarget )
            return false;

        // AppNotificationManager is a single process-wide object. Keep every
        // operation on it serial, while permitting deterministic same-thread
        // reentrancy from the fake boundary used by the transaction tests.
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        if ( IsShutdownRequested() || IsFinalizationRequested() )
            return false;

        std::shared_ptr<wxWinUIAppNotificationNative> native;
        std::uint64_t enableShutdownSerial = 0;
        std::uint64_t enableEpoch = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( m_registered && m_enabled && !m_shuttingDown &&
                 !m_finalized && !IsShutdownRequested() &&
                 !IsFinalizationRequested() )
            {
                return true;
            }
            if ( m_shuttingDown || m_finalized || IsShutdownRequested() ||
                 IsFinalizationRequested() )
            {
                return false;
            }
            if ( m_enableInProgress )
            {
                // Register() is a user/native callback boundary. A recursive
                // Enable() must never overwrite its activation token by
                // starting a second registration transaction.
                return false;
            }

            if ( !m_native )
                m_native = std::make_shared<
                    wxWinUIAppNotificationNativeImpl>();
            native = m_native;
            // A same-thread Register() callback can call Shutdown() and let
            // its transient request flag disappear before Register() returns.
            // The monotone serial makes that revocation durable.
            enableShutdownSerial = m_shutdownSerial;
            enableEpoch = NextNonZero(m_enableEpoch);
            m_enableInProgress = true;
        }
        const wxScopeGuard finishEnable = wxMakeGuard(
            [this, enableEpoch]()
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if ( m_enableInProgress && m_enableEpoch == enableEpoch )
                    m_enableInProgress = false;
            });
        wxUnusedVar(finishEnable);

        RetryCleanupOnly();

        if ( !NativeIsSupported(native) || IsShutdownRequested() ||
             IsFinalizationRequested() )
        {
            return false;
        }

        const bool registered = NativeRegister(native,
            [](const std::uint64_t generation,
               const wxWinUIAppNotificationActivationKind kind,
               const wxWindowID actionId)
            {
                wxWinUIAppNotificationBroker::Get().OnActivation(
                    generation, kind, actionId);
            });

        // A retained cleanup entry may belong to this same native object from
        // a prior registration epoch. It could not be removed while the
        // manager was unregistered, so retry once registration exists again.
        if ( registered )
            RetryCleanupOnly();

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( !IsShutdownRequested() && !IsFinalizationRequested() &&
                 !m_shuttingDown && !m_finalized && native == m_native &&
                 enableShutdownSerial == m_shutdownSerial )
            {
                m_registered = registered;
                m_enabled = registered;
                m_dispatchTarget = registered ? dispatchTarget : nullptr;
                m_dispatchEnabled = registered;
                return registered;
            }
        }

        // Never call into the native boundary while holding the broker lock:
        // revocation can wait for an in-flight callback entering OnActivation.
        if ( registered )
            NativeUnregister(native);
        return false;
    }

    bool IsEnabled() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_enabled && m_registered && !m_shuttingDown && !m_finalized &&
               !IsShutdownRequested() && !IsFinalizationRequested();
    }

    std::shared_ptr<wxWinUIAppNotificationInstanceState> CreateState(
        wxNotificationMessageBase* const notification)
    {
        auto state = std::make_shared<
            wxWinUIAppNotificationInstanceState>(notification);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_states[state.get()] = state;
        ++m_liveInstanceCount;
        return state;
    }

    void DestroyState(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state)
    {
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            state->alive = false;
            state->notification = nullptr;
            state->pendingDeliveries.clear();
            for ( const auto& active : state->activeNotifications )
                m_generations.erase(active.first);
            state->activeNotifications.clear();
            state->closeAuthorities.clear();
            state->deferredActivations.clear();
            NextNonZero(state->requestEpoch);
            m_states.erase(state.get());
            if ( m_liveInstanceCount != 0 )
                --m_liveInstanceCount;
        }

        // Successful fire-and-forget notifications never enter this ledger.
        // Only failed compensations are retried when their wx wrapper dies.
        RetryCleanupOnly();
    }

    bool Show(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state,
        wxWinUIAppNotificationPayload payload)
    {
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);

        RetryCleanupOnly();

        std::shared_ptr<wxWinUIAppNotificationNative> native;
        std::map<std::uint64_t, std::uint32_t> oldNotifications;
        std::uint64_t operationShutdownSerial = 0;
        std::uint64_t requestEpoch = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( !state->alive || !m_enabled || !m_registered ||
                 m_shuttingDown || m_finalized || IsShutdownRequested() ||
                 IsFinalizationRequested() )
            {
                return false;
            }

            // A new request supersedes any event that was queued for the
            // previous generation, even if this Show() later fails.
            state->pendingDeliveries.clear();
            requestEpoch = NextNonZero(state->requestEpoch);
            payload.generation = ++m_lastGeneration;
            if ( payload.generation == 0 )
                payload.generation = ++m_lastGeneration;
            wxWinUIAppNotificationGeneration entry;
            entry.state = state;
            entry.actionIds.reserve(payload.actions.size());
            for ( const auto& action : payload.actions )
                entry.actionIds.push_back(action.id);
            m_generations.emplace(payload.generation, std::move(entry));
            oldNotifications = state->activeNotifications;
            native = m_native;
            operationShutdownSerial = m_shutdownSerial;
            ++state->inFlightShowCount;
            ++m_nativeShowLeaseCount;
        }

        ++gs_appNotificationNativeShowLeaseDepth;
        const wxScopeGuard releaseShowLease = wxMakeGuard(
            [this, state]()
            {
                ReleaseNativeShowLease(state);
            });
        wxUnusedVar(releaseShowLease);

        std::uint32_t newId = 0;
        const bool nativeShown = NativeShow(native, payload, newId);
        if ( !nativeShown || newId == 0 )
        {
            // The native boundary may have made a notification visible before
            // reporting failure (or throwing). A non-zero id is ownership and
            // must always be compensated.
            if ( newId != 0 )
            {
                RollbackShownGeneration(
                    state, payload.generation, newId, native, {});

                std::lock_guard<std::mutex> lock(m_mutex);
                if ( state->requestEpoch == requestEpoch )
                    state->pendingDeliveries.clear();
                else
                    InvalidatePendingGeneration(*state, payload.generation);
                return false;
            }

            std::lock_guard<std::mutex> lock(m_mutex);
            if ( state->requestEpoch == requestEpoch )
                state->pendingDeliveries.clear();
            else
                InvalidatePendingGeneration(*state, payload.generation);
            RetireGenerationLocked(*state, payload.generation);
            return false;
        }

        bool requestStillCurrent = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            requestStillCurrent = IsRequestCurrentLocked(
                *state, requestEpoch, native, operationShutdownSerial);
        }

        if ( !requestStillCurrent )
        {
            // A nested request or Shutdown won while native Show() was in
            // flight. Compensation leaves the winning request's delivery
            // tokens untouched.
            RollbackShownGeneration(
                state, payload.generation, newId, native, {});
            return false;
        }

        // Replacement is transactional. Every successful old removal is
        // committed by exact generation; an unsuccessful compensation keeps
        // the new native id tracked as cleanup-only for a later Close().
        std::vector<std::uint64_t> retiredOldGenerations;
        bool retiredAllOld = true;
        for ( const auto& old : oldNotifications )
        {
            if ( NativeRemove(native, old.second) )
                retiredOldGenerations.push_back(old.first);
            else
                retiredAllOld = false;

            std::lock_guard<std::mutex> lock(m_mutex);
            requestStillCurrent = IsRequestCurrentLocked(
                *state, requestEpoch, native, operationShutdownSerial);
            if ( !requestStillCurrent )
                break;
        }

        if ( !retiredAllOld || !requestStillCurrent )
        {
            RollbackShownGeneration(state, payload.generation, newId, native,
                                    retiredOldGenerations);
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( IsRequestCurrentLocked(
                     *state, requestEpoch, native, operationShutdownSerial) )
            {
                for ( const std::uint64_t retired : retiredOldGenerations )
                {
                    ForgetCleanupOnlyLocked(retired);
                    RetireGenerationLocked(*state, retired);
                }

                // Synchronous activation can legitimately consume the new
                // generation before Show() publishes its native id.
                if ( m_generations.find(payload.generation) !=
                     m_generations.end() )
                {
                    state->activeNotifications[payload.generation] = newId;
                }
                InvalidatePendingExcept(*state, payload.generation);
                return true;
            }
        }

        RollbackShownGeneration(state, payload.generation, newId, native,
                                retiredOldGenerations);
        return false;
    }

    bool Close(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state)
    {
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);

        RetryCleanupOnly();

        std::shared_ptr<wxWinUIAppNotificationNative> native;
        std::map<std::uint64_t, std::uint32_t> notifications;
        std::uint64_t operationShutdownSerial = 0;
        std::uint64_t requestEpoch = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( !state->alive || m_shuttingDown ||
                 m_finalized || IsShutdownRequested() ||
                 IsFinalizationRequested() )
            {
                return false;
            }

            notifications = state->activeNotifications;
            const bool hasInFlightShow = state->inFlightShowCount != 0;
            if ( notifications.empty() && !hasInFlightShow )
            {
                // A generation already consumed by OnActivation() owns any
                // delivery it committed. A true no-op Close() must not revoke
                // that delivery merely because the native surface is gone.
                return false;
            }

            // An otherwise empty Close() still supersedes a Show() which
            // re-entered the native boundary before publishing its id.
            state->pendingDeliveries.clear();
            requestEpoch = NextNonZero(state->requestEpoch);
            native = m_native;
            operationShutdownSerial = m_shutdownSerial;
            for ( const auto& notification : notifications )
                state->closeAuthorities[notification.first] = requestEpoch;
        }

        if ( notifications.empty() )
            return false;
        if ( !native )
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for ( const auto& notification : notifications )
            {
                const auto authority =
                    state->closeAuthorities.find(notification.first);
                if ( authority != state->closeAuthorities.end() &&
                     authority->second == requestEpoch )
                {
                    state->closeAuthorities.erase(authority);
                }
            }
            return false;
        }

        std::vector<std::uint64_t> removed;
        bool requestStillCurrent = true;
        for ( const auto& notification : notifications )
        {
            if ( NativeRemove(native, notification.second) )
                removed.push_back(notification.first);

            std::lock_guard<std::mutex> lock(m_mutex);
            requestStillCurrent = IsRequestCurrentLocked(
                *state, requestEpoch, native, operationShutdownSerial);
            if ( !requestStillCurrent )
                break;
        }

        std::vector<wxWinUIAppNotificationInstanceState::DeferredActivation>
            deferred;
        const bool allRemoved = removed.size() == notifications.size();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            requestStillCurrent = IsRequestCurrentLocked(
                *state, requestEpoch, native, operationShutdownSerial);

            for ( const auto& notification : notifications )
            {
                const std::uint64_t generation = notification.first;
                const bool wasRemoved =
                    std::find(removed.begin(), removed.end(), generation) !=
                        removed.end();

                const auto pendingActivation =
                    state->deferredActivations.find(generation);
                if ( pendingActivation != state->deferredActivations.end() &&
                     pendingActivation->second.closeEpoch == requestEpoch )
                {
                    // Remove is the authority if it succeeded. If it failed,
                    // the physical activation remains the sole event, but a
                    // newer request still supersedes this delayed delivery.
                    if ( !wasRemoved && requestStillCurrent )
                        deferred.push_back(pendingActivation->second);
                    state->deferredActivations.erase(pendingActivation);
                }

                const auto authority =
                    state->closeAuthorities.find(generation);
                if ( authority != state->closeAuthorities.end() &&
                     authority->second == requestEpoch )
                {
                    state->closeAuthorities.erase(authority);
                }

                if ( wasRemoved )
                {
                    ForgetCleanupOnlyLocked(generation);
                    RetireGenerationLocked(*state, generation);
                }
            }
        }

        for ( const auto& activation : deferred )
        {
            QueueEvent(state, requestEpoch, activation.kind,
                       activation.actionId, false);
        }

        if ( !allRemoved || !requestStillCurrent )
            return false;

        QueueEvent(state,
                   requestEpoch,
                   wxWinUIAppNotificationActivationKind::Click,
                   wxID_NONE,
                   true);
        return true;
    }

    void Shutdown()
    {
        ShutdownImpl(false);
    }

    void Finalize()
    {
        // Publish the permanent latch before waiting for a native transaction,
        // exactly as the transient shutdown request is published below.
        m_finalizationRequested.store(true, std::memory_order_release);
        ShutdownImpl(true);
    }

    void ResetFinalizationForTesting()
    {
        // Production has no path back from Finalize(). Tests reset it only
        // after forcing the broker into the same quiescent state as OnExit().
        // Keep the finalization intent published across the whole reset: a
        // concurrent Enable() must not publish a new registration between
        // ShutdownImpl() releasing its operation lock and the test seam
        // reacquiring it below.
        m_finalizationRequested.store(true, std::memory_order_release);
        ShutdownImpl(false);

        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        std::lock_guard<std::mutex> lock(m_mutex);
        wxASSERT(m_dispatchLeaseCount == 0);
        wxASSERT(m_nativeShowLeaseCount == 0);
        wxASSERT(!m_deferredShutdownCompletion);
        wxASSERT(!m_shuttingDown);
        m_finalized = false;
        m_finalizationRequested.store(false, std::memory_order_release);
    }

    bool ReplaceNativeForTesting(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native)
    {
        bool expected = false;
        if ( !m_nativeReplacementRequested.compare_exchange_strong(
                 expected, true,
                 std::memory_order_acq_rel,
                 std::memory_order_acquire) )
        {
            return false;
        }
        const wxScopeGuard finishReplacement = wxMakeGuard(
            [this]()
            {
                m_nativeReplacementRequested.store(
                    false, std::memory_order_release);
            });
        wxUnusedVar(finishReplacement);

        std::shared_ptr<wxWinUIAppNotificationNative> oldNative;
        {
            // Failed Show compensation belongs to the exact native manager
            // which produced its id. Retry it before unregistering or
            // replacing that manager: after either action the id may become
            // permanently unremovable.
            std::lock_guard<std::recursive_mutex> operationLock(
                m_nativeOperationMutex);
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                oldNative = m_native;
                // RetryCleanupOnly() crosses the native Remove() boundary.
                // A callback from it may re-enter Shutdown(), which must not
                // wait for a pre-existing dispatch/Show lease while this
                // replacement still owns an outer recursive operation-lock
                // level. The published replacement intent prevents any new
                // lease after this check.
                if ( !CompletionLeasesAreZeroLocked() || m_shuttingDown ||
                     m_deferredShutdownCompletion )
                {
                    return false;
                }
            }

            RetryCleanupOnly();

            std::lock_guard<std::mutex> lock(m_mutex);
            if ( !m_cleanupOnly.empty() || m_native != oldNative ||
                 m_finalized || IsFinalizationRequested() )
            {
                return false;
            }
        }

        Shutdown();

        // Shutdown() may need to drop the operation mutex while waiting for a
        // dispatch/Show lease. Never add an outer recursive ownership level
        // around that wait. The replacement intent remains visible while the
        // lock is dropped, so Enable/Show/Close cannot reopen the broker.
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        std::lock_guard<std::mutex> lock(m_mutex);
        if ( m_native != oldNative || m_shuttingDown || m_enabled ||
             m_registered || m_dispatchEnabled || m_finalized ||
             IsFinalizationRequested() ||
             !m_generations.empty() || !m_cleanupOnly.empty() ||
             !CompletionLeasesAreZeroLocked() ||
             m_deferredShutdownCompletion )
        {
            return false;
        }

        m_native = native;
        m_enabled = false;
        m_registered = false;
        return true;
    }

    void SetDispatchHookForTesting(
        const wxWinUIAppNotificationDispatchHookForTesting hook)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dispatchHookForTesting = hook;
    }

    wxWinUIAppNotificationSnapshot Snapshot() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        wxWinUIAppNotificationSnapshot snapshot;
        snapshot.enabled = m_enabled;
        snapshot.registered = m_registered;
        snapshot.shuttingDown = m_shuttingDown || IsShutdownRequested();
        snapshot.finalized = m_finalized || IsFinalizationRequested();
        snapshot.activeGenerationCount = m_generations.size();
        snapshot.liveInstanceCount = m_liveInstanceCount;
        snapshot.queuedCallbackCount = m_queuedCallbackCount;
        snapshot.dispatchLeaseCount = m_dispatchLeaseCount;
        snapshot.nativeShowLeaseCount = m_nativeShowLeaseCount;
        snapshot.cleanupOnlyCount = m_cleanupOnly.size();
        snapshot.dispatchEnabled = m_dispatchEnabled;
        snapshot.lastGeneration = m_lastGeneration;
        snapshot.shutdownSerial = m_shutdownSerial;
        return snapshot;
    }

private:
    void ShutdownImpl(const bool finalize)
    {
        // Publish intent before waiting for a blocked Register()/Show(). The
        // in-flight operation observes it and compensates instead of
        // publishing state after OnExit() has begun.
        class ShutdownRequestLease final
        {
        public:
            explicit ShutdownRequestLease(std::atomic<unsigned>& count)
                : m_count(count)
            {
                m_count.fetch_add(1, std::memory_order_acq_rel);
            }

            ~ShutdownRequestLease()
            {
                m_count.fetch_sub(1, std::memory_order_acq_rel);
            }

        private:
            std::atomic<unsigned>& m_count;
        };

        ShutdownRequestLease requestLease(m_shutdownRequestCount);
        std::unique_lock<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);

        std::shared_ptr<wxWinUIAppNotificationNative> native;
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // This serial is the durable revocation epoch. Advance it for
            // every request, including a no-op or a reentrant request while a
            // shutdown is already being completed.
            NextNonZero(m_shutdownSerial);
            if ( finalize )
                m_finalized = true;

            if ( m_shuttingDown )
            {
                if ( OwnsShutdownCompletionLease() )
                    return;

                // A prior self-owned dispatch lease deferred completion. A
                // normal caller still receives the synchronous Shutdown()
                // contract, but must release the native-operation lock so the
                // last lease can acquire it and finish the transaction.
                operationLock.unlock();
                m_completionLeaseChanged.wait(lock,
                    [this]() { return !m_shuttingDown; });
                return;
            }
            if ( !m_registered && !m_enabled && m_generations.empty() &&
                 m_cleanupOnly.empty() && !m_dispatchEnabled &&
                 CompletionLeasesAreZeroLocked() )
            {
                return;
            }
            m_shuttingDown = true;
            m_enabled = false;
            m_registered = false;
            m_dispatchEnabled = false;
            m_dispatchTarget = nullptr;
            for ( auto it = m_states.begin(); it != m_states.end(); )
            {
                if ( const auto state = it->second.lock() )
                {
                    state->pendingDeliveries.clear();
                    state->activeNotifications.clear();
                    state->closeAuthorities.clear();
                    state->deferredActivations.clear();
                    NextNonZero(state->requestEpoch);
                    ++it;
                }
                else
                    it = m_states.erase(it);
            }
            m_generations.clear();
            native = m_native;

            // A callback which acquired the raw wxApp target before the
            // authority was revoked must finish QueueEvent() before OnExit()
            // is allowed to return and wxApp destruction can begin.
            if ( !CompletionLeasesAreZeroLocked() &&
                 OwnsShutdownCompletionLease() )
            {
                // Waiting here would be an automatic same-thread deadlock.
                // Keep the broker revoked and let the last dispatch/Show
                // lease finish cleanup and native unregistration.
                m_deferredShutdownNative = native;
                m_deferredShutdownCompletion = true;
                return;
            }

            if ( !CompletionLeasesAreZeroLocked() )
            {
                // A leased callback is allowed to re-enter Enable/Show/Close.
                // It must be able to acquire the operation mutex, observe the
                // already-published revocation and return before releasing its
                // lease. Waiting while owning that mutex would deadlock it.
                operationLock.unlock();
                m_completionLeaseChanged.wait(lock,
                    [this]() { return CompletionLeasesAreZeroLocked(); });
            }
        }

        if ( !operationLock.owns_lock() )
            operationLock.lock();
        CompleteShutdown(native);
    }

    void CompleteShutdown(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native)
    {
        ++gs_appNotificationShutdownCompletionDepth;
        const wxScopeGuard leaveCompletion = wxMakeGuard(
            []()
            {
                wxASSERT(gs_appNotificationShutdownCompletionDepth != 0);
                if ( gs_appNotificationShutdownCompletionDepth != 0 )
                    --gs_appNotificationShutdownCompletionDepth;
            });
        wxUnusedVar(leaveCompletion);

        // Remove every failed compensation while its exact native manager is
        // still registered. A failed retry remains broker-owned for a later
        // operation; it is never silently confused with fire-and-forget state.
        RetryCleanupOnly();
        NativeUnregister(native);

        std::lock_guard<std::mutex> lock(m_mutex);
        m_shuttingDown = false;
        m_completionLeaseChanged.notify_all();
    }

    static bool NativeIsSupported(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native)
    {
        try
        {
            return native && native->IsSupported();
        }
        catch (...)
        {
            return false;
        }
    }

    static bool NativeRegister(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native,
        wxWinUIAppNotificationActivation activation)
    {
        try
        {
            return native && native->Register(std::move(activation));
        }
        catch (...)
        {
            return false;
        }
    }

    static void NativeUnregister(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native)
    {
        try
        {
            if ( native )
                native->Unregister();
        }
        catch (...)
        {
        }
    }

    static bool NativeShow(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native,
        const wxWinUIAppNotificationPayload& payload,
        std::uint32_t& notificationId)
    {
        try
        {
            return native && native->Show(payload, notificationId);
        }
        catch (...)
        {
            return false;
        }
    }

    static bool NativeRemove(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native,
        const std::uint32_t notificationId)
    {
        try
        {
            return native && native->Remove(notificationId);
        }
        catch (...)
        {
            return false;
        }
    }

    static std::uint64_t NextNonZero(std::uint64_t& value)
    {
        if ( ++value == 0 )
            ++value;
        return value;
    }

    bool IsShutdownRequested() const
    {
        return m_shutdownRequestCount.load(std::memory_order_acquire) != 0 ||
               m_nativeReplacementRequested.load(
                   std::memory_order_acquire);
    }

    bool IsFinalizationRequested() const
    {
        return m_finalizationRequested.load(std::memory_order_acquire);
    }

    static bool OwnsShutdownCompletionLease()
    {
        return gs_appNotificationDispatchLeaseDepth != 0 ||
               gs_appNotificationNativeShowLeaseDepth != 0 ||
               gs_appNotificationShutdownCompletionDepth != 0;
    }

    bool CompletionLeasesAreZeroLocked() const
    {
        return m_dispatchLeaseCount == 0 && m_nativeShowLeaseCount == 0;
    }

    bool IsRequestCurrentLocked(
        const wxWinUIAppNotificationInstanceState& state,
        const std::uint64_t requestEpoch,
        const std::shared_ptr<wxWinUIAppNotificationNative>& native,
        const std::uint64_t shutdownSerial) const
    {
        return state.alive && state.requestEpoch == requestEpoch &&
               m_enabled && m_registered && !m_shuttingDown && !m_finalized &&
               !IsShutdownRequested() && !IsFinalizationRequested() &&
               native == m_native && shutdownSerial == m_shutdownSerial;
    }

    static void InvalidatePendingGeneration(
        wxWinUIAppNotificationInstanceState& state,
        const std::uint64_t generation)
    {
        for ( auto it = state.pendingDeliveries.begin();
              it != state.pendingDeliveries.end(); )
        {
            if ( it->second == generation )
                it = state.pendingDeliveries.erase(it);
            else
                ++it;
        }
    }

    std::uint64_t AdoptCleanupOnlyLocked(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state,
        const std::uint64_t generation,
        const std::uint32_t notificationId,
        const std::shared_ptr<wxWinUIAppNotificationNative>& native)
    {
        std::uint64_t token = NextNonZero(m_lastCleanupToken);
        wxWinUIAppNotificationCleanupOnly cleanup;
        cleanup.native = native;
        cleanup.state = state;
        cleanup.generation = generation;
        cleanup.notificationId = notificationId;
        m_cleanupOnly.emplace(token, std::move(cleanup));
        return token;
    }

    void ForgetCleanupOnlyLocked(const std::uint64_t generation)
    {
        const auto entry = m_generations.find(generation);
        if ( entry == m_generations.end() || entry->second.cleanupToken == 0 )
            return;

        m_cleanupOnly.erase(entry->second.cleanupToken);
        entry->second.cleanupToken = 0;
    }

    void RetryCleanupOnly()
    {
        std::uint64_t cursor = 0;
        for ( ;; )
        {
            std::uint64_t token = 0;
            wxWinUIAppNotificationCleanupOnly cleanup;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto current = m_cleanupOnly.upper_bound(cursor);
                while ( current != m_cleanupOnly.end() &&
                        current->second.retrying )
                {
                    ++current;
                }
                if ( current == m_cleanupOnly.end() )
                    return;

                token = current->first;
                cursor = token;
                current->second.retrying = true;
                cleanup = current->second;
            }

            const bool removed =
                NativeRemove(cleanup.native, cleanup.notificationId);
            if ( !removed )
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                const auto current = m_cleanupOnly.find(token);
                if ( current != m_cleanupOnly.end() &&
                     current->second.native == cleanup.native &&
                     current->second.notificationId ==
                         cleanup.notificationId )
                {
                    current->second.retrying = false;
                }
                continue;
            }

            std::lock_guard<std::mutex> lock(m_mutex);
            const auto current = m_cleanupOnly.find(token);
            if ( current == m_cleanupOnly.end() ||
                 current->second.native != cleanup.native ||
                 current->second.notificationId != cleanup.notificationId )
            {
                continue;
            }

            const auto generation = m_generations.find(cleanup.generation);
            if ( generation != m_generations.end() &&
                 generation->second.cleanupToken == token )
            {
                generation->second.cleanupToken = 0;
                if ( const auto state = current->second.state.lock() )
                    RetireGenerationLocked(*state, cleanup.generation);
                else
                    m_generations.erase(generation);
            }
            m_cleanupOnly.erase(current);
        }
    }

    void RetireGenerationLocked(
        wxWinUIAppNotificationInstanceState& state,
        const std::uint64_t generation)
    {
        state.activeNotifications.erase(generation);
        state.closeAuthorities.erase(generation);
        state.deferredActivations.erase(generation);
        InvalidatePendingGeneration(state, generation);
        m_generations.erase(generation);
    }

    void RollbackShownGeneration(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state,
        const std::uint64_t generation,
        const std::uint32_t notificationId,
        const std::shared_ptr<wxWinUIAppNotificationNative>& native,
        const std::vector<std::uint64_t>& retiredOldGenerations)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            const auto entry = m_generations.find(generation);
            if ( entry != m_generations.end() )
                entry->second.acceptsActivation = false;
            InvalidatePendingGeneration(*state, generation);
        }

        const bool removed = NativeRemove(native, notificationId);

        std::lock_guard<std::mutex> lock(m_mutex);
        for ( const std::uint64_t retired : retiredOldGenerations )
        {
            ForgetCleanupOnlyLocked(retired);
            RetireGenerationLocked(*state, retired);
        }

        const auto entry = m_generations.find(generation);
        if ( removed )
        {
            ForgetCleanupOnlyLocked(generation);
            RetireGenerationLocked(*state, generation);
        }
        else
        {
            // Native ownership survives independently of the wx wrapper and
            // even of m_native replacement. Only a successful exact Remove()
            // may erase this cleanup-only ledger entry.
            const std::uint64_t cleanupToken = AdoptCleanupOnlyLocked(
                state, generation, notificationId, native);
            if ( state->alive && native == m_native &&
                 entry != m_generations.end() )
            {
                entry->second.acceptsActivation = false;
                entry->second.cleanupToken = cleanupToken;
                state->activeNotifications[generation] = notificationId;
            }
            else
            {
                RetireGenerationLocked(*state, generation);
            }
        }
    }

    void OnActivation(
        const std::uint64_t generation,
        const wxWinUIAppNotificationActivationKind kind,
        const wxWindowID actionId)
    {
        std::shared_ptr<wxWinUIAppNotificationInstanceState> state;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( m_shuttingDown || m_finalized || IsShutdownRequested() ||
                 IsFinalizationRequested() )
                return;
            const auto it = m_generations.find(generation);
            // A cold process activation has no live wxEvtHandler to own it.
            // The current wx API exposes per-object events only, so unknown
            // generations are deliberately ignored instead of inventing a
            // second process activation API here.
            if ( it == m_generations.end() )
                return;
            if ( kind == wxWinUIAppNotificationActivationKind::Action &&
                 std::find(it->second.actionIds.begin(),
                           it->second.actionIds.end(),
                           actionId) == it->second.actionIds.end() )
            {
                return;
            }
            state = it->second.state.lock();
            if ( !state || !state->alive )
            {
                m_generations.erase(it);
                return;
            }

            if ( !it->second.acceptsActivation )
            {
                RetireGenerationLocked(*state, generation);
                return;
            }

            const auto closeAuthority =
                state->closeAuthorities.find(generation);
            if ( closeAuthority != state->closeAuthorities.end() )
            {
                wxWinUIAppNotificationInstanceState::DeferredActivation
                    activation;
                activation.kind = kind;
                activation.actionId = actionId;
                activation.closeEpoch = closeAuthority->second;
                state->deferredActivations[generation] = activation;
                state->activeNotifications.erase(generation);
                m_generations.erase(it);
                return;
            }

            state->activeNotifications.erase(generation);
            m_generations.erase(it);
        }

        QueueEvent(state, generation, kind, actionId, false);
    }

    void QueueEvent(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state,
        const std::uint64_t generation,
        const wxWinUIAppNotificationActivationKind kind,
        const wxWindowID actionId,
        const bool dismissedByApp)
    {
        wxApp* app = nullptr;
        wxWinUIAppNotificationDispatchHookForTesting hook = nullptr;
        std::uint64_t deliveryToken = 0;
        std::uint64_t shutdownSerial = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( m_shuttingDown || m_finalized || !state->alive ||
                 IsShutdownRequested() || IsFinalizationRequested() ||
                 !m_dispatchEnabled || !m_dispatchTarget )
            {
                return;
            }
            deliveryToken = ++state->lastDeliveryToken;
            if ( deliveryToken == 0 )
                deliveryToken = ++state->lastDeliveryToken;
            state->pendingDeliveries[deliveryToken] = generation;
            shutdownSerial = m_shutdownSerial;
            ++m_queuedCallbackCount;
            ++m_dispatchLeaseCount;
            app = m_dispatchTarget;
            hook = m_dispatchHookForTesting;
        }

        ++gs_appNotificationDispatchLeaseDepth;
        const wxScopeGuard releaseDispatchLease = wxMakeGuard(
            [this]() { ReleaseDispatchLease(); });
        wxUnusedVar(releaseDispatchLease);

        try
        {
            if ( hook )
                hook();

            // The hook and wxApp dispatch implementations are user-controlled
            // boundaries and may re-enter Shutdown(). Revalidate before using
            // the raw target acquired by this lease.
            bool mayDispatch = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                const auto pending =
                    state->pendingDeliveries.find(deliveryToken);
                mayDispatch = !m_shuttingDown && !m_finalized &&
                    !IsShutdownRequested() && !IsFinalizationRequested() &&
                    state->alive && m_dispatchEnabled &&
                    m_dispatchTarget == app &&
                    m_shutdownSerial == shutdownSerial &&
                    pending != state->pendingDeliveries.end() &&
                    pending->second == generation;
                if ( !mayDispatch )
                {
                    if ( m_queuedCallbackCount != 0 )
                        --m_queuedCallbackCount;
                    if ( pending != state->pendingDeliveries.end() )
                        state->pendingDeliveries.erase(pending);
                }
            }

            if ( !mayDispatch )
                return;

            app->CallAfter(
                [state, generation, deliveryToken, shutdownSerial, kind,
                 actionId, dismissedByApp]()
                {
                    wxNotificationMessageBase* notification = nullptr;
                    {
                        auto& broker = wxWinUIAppNotificationBroker::Get();
                        std::lock_guard<std::mutex> lock(broker.m_mutex);
                        if ( broker.m_queuedCallbackCount != 0 )
                            --broker.m_queuedCallbackCount;
                        const auto pending =
                            state->pendingDeliveries.find(deliveryToken);
                        if ( broker.m_shuttingDown || broker.m_finalized ||
                             broker.IsFinalizationRequested() || !state->alive ||
                             broker.m_shutdownSerial != shutdownSerial ||
                             pending == state->pendingDeliveries.end() ||
                             pending->second != generation )
                        {
                            if ( pending != state->pendingDeliveries.end() )
                                state->pendingDeliveries.erase(pending);
                            return;
                        }
                        state->pendingDeliveries.erase(pending);
                        notification = state->notification.get();
                    }

                    if ( !notification )
                        return;

                    if ( dismissedByApp )
                    {
                        wxCommandEvent event(
                            wxEVT_NOTIFICATION_MESSAGE_DISMISSED);
                        event.SetInt(static_cast<int>(
                            wxNotificationMessage::DismissalReason::ByApp));
                        notification->ProcessEvent(event);
                    }
                    else if (
                        kind ==
                            wxWinUIAppNotificationActivationKind::Action )
                    {
                        wxCommandEvent event(
                            wxEVT_NOTIFICATION_MESSAGE_ACTION, actionId);
                        notification->ProcessEvent(event);
                    }
                    else
                    {
                        wxCommandEvent event(
                            wxEVT_NOTIFICATION_MESSAGE_CLICK);
                        notification->ProcessEvent(event);
                    }
                });
        }
        catch (...)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if ( m_queuedCallbackCount != 0 )
                --m_queuedCallbackCount;
            state->pendingDeliveries.erase(deliveryToken);
        }
    }

    void ReleaseDispatchLease()
    {
        wxASSERT(gs_appNotificationDispatchLeaseDepth != 0);
        if ( gs_appNotificationDispatchLeaseDepth != 0 )
            --gs_appNotificationDispatchLeaseDepth;

        std::shared_ptr<wxWinUIAppNotificationNative> deferredNative;
        bool completeDeferredShutdown = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            wxASSERT(m_dispatchLeaseCount != 0);
            if ( m_dispatchLeaseCount != 0 )
                --m_dispatchLeaseCount;
            m_completionLeaseChanged.notify_all();

            if ( CompletionLeasesAreZeroLocked() &&
                 m_deferredShutdownCompletion )
            {
                deferredNative = std::move(m_deferredShutdownNative);
                m_deferredShutdownCompletion = false;
                completeDeferredShutdown = true;
            }
        }

        if ( !completeDeferredShutdown )
            return;

        // ShutdownImpl() released this lock before deferring. Reacquiring it
        // serializes native unregistration with any outer native transaction;
        // the recursive case covers synchronous callbacks from that boundary.
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        CompleteShutdown(deferredNative);
    }

    void ReleaseNativeShowLease(
        const std::shared_ptr<wxWinUIAppNotificationInstanceState>& state)
    {
        wxASSERT(gs_appNotificationNativeShowLeaseDepth != 0);
        if ( gs_appNotificationNativeShowLeaseDepth != 0 )
            --gs_appNotificationNativeShowLeaseDepth;

        std::shared_ptr<wxWinUIAppNotificationNative> deferredNative;
        bool completeDeferredShutdown = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            wxASSERT(state->inFlightShowCount != 0);
            if ( state->inFlightShowCount != 0 )
                --state->inFlightShowCount;
            wxASSERT(m_nativeShowLeaseCount != 0);
            if ( m_nativeShowLeaseCount != 0 )
                --m_nativeShowLeaseCount;
            m_completionLeaseChanged.notify_all();

            if ( CompletionLeasesAreZeroLocked() &&
                 m_deferredShutdownCompletion )
            {
                deferredNative = std::move(m_deferredShutdownNative);
                m_deferredShutdownCompletion = false;
                completeDeferredShutdown = true;
            }
        }

        if ( !completeDeferredShutdown )
            return;

        // The Show transaction has now returned from the native boundary and
        // committed or compensated its exact id. Only now may Unregister run.
        std::lock_guard<std::recursive_mutex> operationLock(
            m_nativeOperationMutex);
        CompleteShutdown(deferredNative);
    }

    static void InvalidatePendingExcept(
        wxWinUIAppNotificationInstanceState& state,
        const std::uint64_t generation)
    {
        for ( auto it = state.pendingDeliveries.begin();
              it != state.pendingDeliveries.end(); )
        {
            if ( it->second == generation )
                ++it;
            else
                it = state.pendingDeliveries.erase(it);
        }
    }

    mutable std::mutex m_mutex;
    mutable std::recursive_mutex m_nativeOperationMutex;
    std::atomic<unsigned> m_shutdownRequestCount{0};
    std::atomic<bool> m_nativeReplacementRequested{false};
    std::atomic<bool> m_finalizationRequested{false};
    std::shared_ptr<wxWinUIAppNotificationNative> m_native;
    std::shared_ptr<wxWinUIAppNotificationNative> m_deferredShutdownNative;
    std::map<std::uint64_t, wxWinUIAppNotificationGeneration> m_generations;
    std::map<std::uint64_t, wxWinUIAppNotificationCleanupOnly> m_cleanupOnly;
    std::map<wxWinUIAppNotificationInstanceState*,
             std::weak_ptr<wxWinUIAppNotificationInstanceState>> m_states;
    std::condition_variable m_completionLeaseChanged;
    wxApp* m_dispatchTarget = nullptr;
    wxWinUIAppNotificationDispatchHookForTesting m_dispatchHookForTesting =
        nullptr;
    std::uint64_t m_lastGeneration = 0;
    std::uint64_t m_lastCleanupToken = 0;
    std::uint64_t m_shutdownSerial = 0;
    std::uint64_t m_enableEpoch = 0;
    std::size_t m_liveInstanceCount = 0;
    std::size_t m_queuedCallbackCount = 0;
    std::size_t m_dispatchLeaseCount = 0;
    std::size_t m_nativeShowLeaseCount = 0;
    bool m_dispatchEnabled = false;
    bool m_enabled = false;
    bool m_registered = false;
    bool m_enableInProgress = false;
    bool m_shuttingDown = false;
    bool m_deferredShutdownCompletion = false;
    bool m_finalized = false;
};

} // anonymous namespace

class wxToastNotifMsgImpl final : public wxNotificationMessageImpl
{
public:
    explicit wxToastNotifMsgImpl(wxNotificationMessageBase* notification)
        : wxNotificationMessageImpl(notification),
          m_state(wxWinUIAppNotificationBroker::Get().CreateState(notification))
    {
    }

    ~wxToastNotifMsgImpl() override
    {
        // Do not remove the native notification here: wx supports
        // fire-and-forget messages whose wrapper is destroyed after Show().
        // Erasing the generation is sufficient to make late activation safe.
        wxWinUIAppNotificationBroker::Get().DestroyState(m_state);
    }

    bool Show(const int timeout) override
    {
        wxWinUIAppNotificationPayload payload;
        payload.title = m_title;
        payload.message = m_message;
        payload.timeout = timeout > 0
            ? std::min(timeout, wxAPP_NOTIFICATION_MAX_TIMEOUT_SECONDS)
            : timeout;
        payload.actions = m_actions;
        return wxWinUIAppNotificationBroker::Get().Show(
            m_state, std::move(payload));
    }

    bool Close() override
    {
        return wxWinUIAppNotificationBroker::Get().Close(m_state);
    }

    void SetTitle(const wxString& title) override { m_title = title; }
    void SetMessage(const wxString& message) override { m_message = message; }
    void SetParent(wxWindow*) override { }
    void SetFlags(int) override { }
    void SetIcon(const wxIcon&) override { }

    bool AddAction(const wxWindowID actionId,
                   const wxString& requestedLabel) override
    {
        wxString label = requestedLabel;
        if ( label.empty() )
            label = wxGetStockLabel(actionId, wxSTOCK_NOFLAGS);
        if ( label.empty() )
            return false;

        m_actions.push_back({actionId, label});
        return true;
    }

    static bool UseToasts(const wxString&, const wxString&)
    {
        return wxWinUIAppNotificationBroker::Get().Enable();
    }

    static bool IsEnabled()
    {
        return wxWinUIAppNotificationBroker::Get().IsEnabled();
    }

    static void Finalize()
    {
        wxWinUIAppNotificationBroker::Get().Finalize();
    }

private:
    wxString m_title;
    wxString m_message;
    std::vector<wxWinUIAppNotificationAction> m_actions;
    std::shared_ptr<wxWinUIAppNotificationInstanceState> m_state;
};

bool wxWinUISetAppNotificationNativeForTesting(
    const std::shared_ptr<wxWinUIAppNotificationNative>& native)
{
    return wxWinUIAppNotificationBroker::Get().ReplaceNativeForTesting(
        native);
}

bool wxWinUIResetAppNotificationNativeForTesting()
{
    return wxWinUIAppNotificationBroker::Get().ReplaceNativeForTesting(
        nullptr);
}

void wxWinUIShutdownAppNotificationsForTesting()
{
    wxWinUIAppNotificationBroker::Get().Shutdown();
}

void wxWinUIFinalizeAppNotificationsForTesting()
{
    wxWinUIAppNotificationBroker::Get().Finalize();
}

void wxWinUIResetAppNotificationFinalizationForTesting()
{
    wxWinUIAppNotificationBroker::Get().ResetFinalizationForTesting();
}

void wxWinUISetAppNotificationDispatchHookForTesting(
    const wxWinUIAppNotificationDispatchHookForTesting hook)
{
    wxWinUIAppNotificationBroker::Get().SetDispatchHookForTesting(hook);
}

void wxWinUIResetAppNotificationDispatchHookForTesting()
{
    wxWinUIAppNotificationBroker::Get().SetDispatchHookForTesting(nullptr);
}

wxWinUIAppNotificationSnapshot
wxWinUIGetAppNotificationSnapshotForTesting()
{
    return wxWinUIAppNotificationBroker::Get().Snapshot();
}

class wxToastNotifMsgModule : public wxModule
{
public:
    wxToastNotifMsgModule()
    {
        // Registration owns Windows App SDK objects and must be revoked before
        // wxWinUI3Module tears down the runtime projections.
        AddDependency("wxWinUI3Module");
    }

    bool OnInit() override { return true; }

    void OnExit() override
    {
        wxToastNotifMsgImpl::Finalize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxToastNotifMsgModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxToastNotifMsgModule, wxModule);

#else // legacy WinRT toast backend

using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;

namespace rt = wxWinRT;

typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ::IInspectable *> DesktopToastActivatedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ToastDismissedEventArgs *> DesktopToastDismissedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ToastFailedEventArgs *> DesktopToastFailedEventHandler;

class wxToastNotifMsgImpl;

class wxToastEventHandler final :
    public Microsoft::WRL::Implements<DesktopToastActivatedEventHandler, DesktopToastDismissedEventHandler, DesktopToastFailedEventHandler>
{
public:
    explicit wxToastEventHandler(wxToastNotifMsgImpl* toastImpl) :
        m_impl(toastImpl)
    {

    }

    void Detach()
    {
        m_impl = nullptr;
    }

    // DesktopToastActivatedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification* sender, IInspectable* args) override;

    // DesktopToastDismissedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification* sender, IToastDismissedEventArgs* e) override;

    // DesktopToastFailedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification* sender, IToastFailedEventArgs* e) override;

    // IUnknown
    STDMETHODIMP_(ULONG) AddRef() override
    {
        return ++m_cRef;
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        if ( !--m_cRef )
        {
            delete this;
            return 0;
        }

        return m_cRef;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override
    {
        if ( riid == IID_IUnknown || riid == __uuidof(DesktopToastActivatedEventHandler) )
        {
            DesktopToastActivatedEventHandler* const p = this;
            *ppv = p;
        }
        else if ( riid == __uuidof(DesktopToastDismissedEventHandler) )
        {
            DesktopToastDismissedEventHandler* const p = this;
            *ppv = p;
        }
        else if ( riid == __uuidof(DesktopToastFailedEventHandler) )
        {
            DesktopToastFailedEventHandler* const p = this;
            *ppv = p;
        }
        else
        {
            *ppv = nullptr;

            return E_NOINTERFACE;
        }

        AddRef();

        return S_OK;
    }

private:
    ULONG m_cRef = 0;

    wxToastNotifMsgImpl* m_impl;
};

class wxToastNotifMsgImpl : public wxNotificationMessageImpl
{
public:
    wxToastNotifMsgImpl(wxNotificationMessageBase* notification) :
        wxNotificationMessageImpl(notification),
        m_toastEventHandler(nullptr)
    {

    }

    virtual ~wxToastNotifMsgImpl()
    {
        // Final destruction is the one path allowed to detach without Hide():
        // no wx event target remains, while the native toast may persist.
        ReleaseToast();
    }

    virtual bool Show(int WXUNUSED(timeout)) override
    {
        // Keep the old handler and all three native tokens attached until
        // Hide() succeeds. A failed replacement leaves a still-visible toast,
        // which must remain routable and closeable exactly as before.
        if ( m_notifier.get() && m_toast.get() &&
             FAILED(m_notifier->Hide(m_toast)) )
        {
            return false;
        }
        ReleaseToast();

        wxCOMPtr<IXmlDocument> toastXml;
        HRESULT hr = CreateToastXML(&toastXml);
        if ( SUCCEEDED(hr) )
        {
            hr = CreateToast(toastXml);
        }

        return SUCCEEDED(hr);
    }

    virtual bool Close() override
    {
        if ( m_notifier.get() && m_toast.get() )
        {
            // As in replacement, Hide() failure means the native toast can
            // still activate. Preserve its handler, tokens and references.
            if ( FAILED(m_notifier->Hide(m_toast)) )
                return false;
            ReleaseToast();
            return true;
        }
        else
            return false;
    }

    virtual void SetTitle(const wxString& title) override
    {
        m_title = title;
    }

    virtual void SetMessage(const wxString& message) override
    {
        m_message = message;
    }

    virtual void SetParent(wxWindow *WXUNUSED(parent)) override
    {

    }

    virtual void SetFlags(int WXUNUSED(flags)) override
    {

    }

    virtual void SetIcon(const wxIcon& WXUNUSED(icon)) override
    {
        // Icon would have to be saved to disk (temporarily?)
        // to be used as a file:// url in the notifications XML
    }

    virtual bool AddAction(wxWindowID WXUNUSED(actionid), const wxString &WXUNUSED(label)) override
    {
        return false;
    }

    void ReleaseToast()
    {
        if ( m_toastEventHandler )
            m_toastEventHandler->Detach();

        if ( m_toast.get() )
        {
            if ( m_hasActivatedToken )
                m_toast->remove_Activated(m_activatedToken);
            if ( m_hasDismissedToken )
                m_toast->remove_Dismissed(m_dismissedToken);
            if ( m_hasFailedToken )
                m_toast->remove_Failed(m_failedToken);
        }
        m_hasActivatedToken = false;
        m_hasDismissedToken = false;
        m_hasFailedToken = false;
        m_toastEventHandler = nullptr;
        m_notifier = nullptr;
        m_toast = nullptr;
    }

    HRESULT CreateToast(IXmlDocument *xml)
    {
        HRESULT hr = ms_toastMgr->CreateToastNotifierWithId(rt::TempStringRef(ms_appId), &m_notifier);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IToastNotificationFactory> factory;
            hr = rt::GetActivationFactory(RuntimeClass_Windows_UI_Notifications_ToastNotification,
                IID_IToastNotificationFactory, reinterpret_cast<void**>(&factory));
            if ( SUCCEEDED(hr) )
            {
                hr = factory->CreateToastNotification(xml, &m_toast);
                if ( SUCCEEDED(hr) )
                {
                    // Register the event handlers
                    m_toastEventHandler = new wxToastEventHandler(this);

                    hr = m_toast->add_Activated(m_toastEventHandler,
                                                &m_activatedToken);
                    if ( SUCCEEDED(hr) )
                    {
                        m_hasActivatedToken = true;
                        hr = m_toast->add_Dismissed(m_toastEventHandler,
                                                   &m_dismissedToken);
                        if ( SUCCEEDED(hr) )
                        {
                            m_hasDismissedToken = true;
                            hr = m_toast->add_Failed(m_toastEventHandler,
                                                    &m_failedToken);
                            if ( SUCCEEDED(hr) )
                            {
                                m_hasFailedToken = true;
                                hr = m_notifier->Show(m_toast);
                            }
                        }
                    }
                }
            }
        }

        if ( FAILED(hr) )
            ReleaseToast();

        return hr;
    }

    HRESULT CreateToastXML(IXmlDocument** toastXml) const
    {
        HRESULT hr = ms_toastMgr->GetTemplateContent(ToastTemplateType_ToastText02, toastXml);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IXmlNodeList> nodeList;
            hr = (*toastXml)->GetElementsByTagName(rt::TempStringRef("text"), &nodeList);
            if ( SUCCEEDED(hr) )
            {
                hr = SetNodeListValueString(0, m_title, nodeList, *toastXml);
                if ( SUCCEEDED(hr) )
                    hr = SetNodeListValueString(1, m_message, nodeList, *toastXml);
            }
        }

        return hr;
    }

    static HRESULT SetNodeListValueString(UINT32 index, const wxString& str, IXmlNodeList* nodeList, IXmlDocument *toastXml)
    {
        wxCOMPtr<IXmlNode> textNode;
        // Set title node
        HRESULT hr = nodeList->Item(index, &textNode);
        if ( SUCCEEDED(hr) )
        {
            hr = SetNodeValueString(str, textNode, toastXml);
        }

        return hr;
    }

    static HRESULT SetNodeValueString(const wxString& str, IXmlNode *node, IXmlDocument *xml)
    {
        wxCOMPtr<IXmlText> inputText;

        HRESULT hr = xml->CreateTextNode(rt::TempStringRef(str), &inputText);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IXmlNode> inputTextNode;

            hr = inputText->QueryInterface(IID_IXmlNode, reinterpret_cast<void**>(&inputTextNode));
            if ( SUCCEEDED(hr) )
            {
                wxCOMPtr<IXmlNode> pAppendedChild;
                hr = node->AppendChild(inputTextNode, &pAppendedChild);
            }
        }

        return hr;
    }

    static bool IsEnabled()
    {
        return ms_enabled;
    }

    static wxString BuildAppId()
    {
        // Build a Application User Model IDs based on app info
        wxString vendorId = wxTheApp->GetVendorName();
        if ( vendorId.empty() )
            vendorId = "wxWidgetsApp";
        wxString appId = vendorId + "." + wxTheApp->GetAppName();
        // Remove potential spaces
        appId.Replace(" ", "", true);
        return appId;
    }

    static bool CheckShortcut(const wxFileName& filename)
    {
        // Prepare interfaces
        wxCOMPtr<IShellLink> shellLink;
        if ( FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
            IID_IShellLinkW, reinterpret_cast<void**>(&shellLink))) )
            return false;
        wxCOMPtr<IPersistFile> persistFile;
        if ( FAILED(shellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persistFile))) )
            return false;
        wxCOMPtr<IPropertyStore> propertyStore;
        if ( FAILED(shellLink->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&propertyStore))) )
            return false;

        bool writeShortcut = false;

        if ( filename.Exists() )
        {
            // Check existing shortcut for application id
            if ( SUCCEEDED(persistFile->Load(filename.GetFullPath().wc_str(), 0)) )
            {
                PROPVARIANT appIdPropVar;
                if ( SUCCEEDED(propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar)) )
                {
                    wxString appId;
                    if ( appIdPropVar.vt == VT_LPWSTR )
                        appId = appIdPropVar.pwszVal;
                    if ( appId.empty() || (!ms_appId.empty() && ms_appId != appId) )
                    {
                        // Update shortcut if app id does not match or is empty
                        writeShortcut = true;
                    }
                    else if ( ms_appId.empty() )
                    {
                        // Use if no app id has been set
                        ms_appId = appId;
                    }
                }
            }
            else
                return false;
        }
        else
        {
            // Create new shortcut
            if ( FAILED(shellLink->SetPath(wxStandardPaths::Get().GetExecutablePath().t_str())) )
                return false;
            if ( FAILED(shellLink->SetArguments(wxT(""))) )
                return false;

            writeShortcut = true;
        }

        if ( writeShortcut )
        {
            if ( ms_appId.empty() )
                ms_appId = BuildAppId();

            // Set application id in shortcut
            PROPVARIANT appIdPropVar;
            if ( FAILED(InitPropVariantFromString(ms_appId.wc_str(), &appIdPropVar)) )
                return false;
            if ( FAILED(propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar)) )
                return false;
            if ( FAILED(propertyStore->Commit()) )
                return false;
            if ( FAILED(persistFile->Save(filename.GetFullPath().wc_str(), TRUE)) )
                return false;
        }

        return true;
    }

    static bool UseToasts(
        const wxString& shortcutPath,
        const wxString& appId)
    {
        ms_enabled = false;

        // WinRT runtime is required (available since Win8)
        if ( !rt::IsAvailable() )
            return false;

        // Toast notification manager has to be available
        if ( ms_toastStaticsInitialized == -1 )
        {
            if ( SUCCEEDED(rt::GetActivationFactory(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager,
                IID_IToastNotificationManagerStatics, reinterpret_cast<void**>(&ms_toastMgr))) )
            {
                ms_toastStaticsInitialized = 1;
            }
            else
                ms_toastStaticsInitialized = 0;
        }

        if ( ms_toastStaticsInitialized != 1 )
            return false;

        // Build/complete shortcut path
        wxFileName shortcutFilename(shortcutPath);
        if ( !shortcutFilename.HasName() )
            shortcutFilename.SetName(wxTheApp->GetAppDisplayName());
        if ( !shortcutFilename.HasExt() )
            shortcutFilename.SetExt("lnk");
        if ( shortcutFilename.IsRelative() )
            shortcutFilename.MakeAbsolute(wxStandardPaths::MSWGetShellDir(CSIDL_STARTMENU));

        ms_appId = appId;

        if ( CheckShortcut(shortcutFilename) )
            ms_enabled = true;

        return ms_enabled;
    }

    static void Uninitalize()
    {
        if (ms_toastStaticsInitialized == 1)
        {
            ms_toastMgr = nullptr;
            ms_toastStaticsInitialized = -1;
        }
    }

private:
    wxString m_title;
    wxString m_message;
    wxCOMPtr<IToastNotifier> m_notifier;
    wxCOMPtr<IToastNotification> m_toast;
    wxCOMPtr<wxToastEventHandler> m_toastEventHandler;
    EventRegistrationToken m_activatedToken{};
    EventRegistrationToken m_dismissedToken{};
    EventRegistrationToken m_failedToken{};
    bool m_hasActivatedToken = false;
    bool m_hasDismissedToken = false;
    bool m_hasFailedToken = false;

    static bool ms_enabled;
    static wxString ms_appId;
    static int ms_toastStaticsInitialized;
    static wxCOMPtr<IToastNotificationManagerStatics> ms_toastMgr;

    friend class wxToastEventHandler;
};

bool wxToastNotifMsgImpl::ms_enabled = false;
int wxToastNotifMsgImpl::ms_toastStaticsInitialized = -1;
wxString wxToastNotifMsgImpl::ms_appId;
wxCOMPtr<IToastNotificationManagerStatics> wxToastNotifMsgImpl::ms_toastMgr;

HRESULT wxToastEventHandler::Invoke(
    IToastNotification *WXUNUSED(sender),
    IInspectable *WXUNUSED(args))
{
    if ( m_impl )
    {
        wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_CLICK);
        m_impl->ProcessNotificationEvent(evt);
    }

    return S_OK;
}

HRESULT wxToastEventHandler::Invoke(
    IToastNotification *WXUNUSED(sender),
    IToastDismissedEventArgs *e)
{
    if ( m_impl )
    {
        wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_DISMISSED);
        ABI::Windows::UI::Notifications::ToastDismissalReason nativeReason;
        auto reason = wxNotificationMessage::DismissalReason::Unknown;
        if ( SUCCEEDED(e->get_Reason(&nativeReason)) )
        {
            switch ( nativeReason )
            {
                case ABI::Windows::UI::Notifications::ToastDismissalReason_UserCanceled:
                    reason = wxNotificationMessage::DismissalReason::ByUser;
                    break;
                case ABI::Windows::UI::Notifications::ToastDismissalReason_ApplicationHidden:
                    reason = wxNotificationMessage::DismissalReason::ByApp;
                    break;
                case ABI::Windows::UI::Notifications::ToastDismissalReason_TimedOut:
                    reason = wxNotificationMessage::DismissalReason::TimedOut;
                    break;
            }
        }
        evt.SetInt(static_cast<int>(reason));
        m_impl->ProcessNotificationEvent(evt);
    }

    return S_OK;
}

HRESULT wxToastEventHandler::Invoke(IToastNotification *WXUNUSED(sender),
    IToastFailedEventArgs *WXUNUSED(e))
{
    //TODO: Handle toast failed event
    return S_OK;
}

//
// wxToastNotifMsgModule
//

class wxToastNotifMsgModule : public wxModule
{
public:
    wxToastNotifMsgModule()
    {
        // Using RT API requires OLE and, importantly, we must ensure our
        // OnExit() runs before it is uninitialized.
        AddDependency("wxOleInitModule");
    }

    virtual bool OnInit() override
    {
        return true;
    }

    virtual void OnExit() override
    {
        wxToastNotifMsgImpl::Uninitalize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxToastNotifMsgModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxToastNotifMsgModule, wxModule);

#endif // __WXWINUI__ && wxUSE_WINUI3

//
// wxToastNotificationHelper
//

bool wxToastNotificationHelper::UseToasts(const wxString& shortcutPath,
    const wxString& appId)
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return wxToastNotifMsgImpl::UseToasts(shortcutPath, appId);
#else
    wxUnusedVar(shortcutPath);
    wxUnusedVar(appId);
    return false;
#endif
}

bool wxToastNotificationHelper::IsEnabled()
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return wxToastNotifMsgImpl::IsEnabled();
#else
    return false;
#endif
}

wxNotificationMessageImpl* wxToastNotificationHelper::CreateInstance(wxNotificationMessageBase* notification)
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return new wxToastNotifMsgImpl(notification);
#else
    wxUnusedVar(notification);
    return nullptr;
#endif
}

#endif // wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
