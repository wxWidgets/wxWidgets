///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitransientstate.cpp
// Purpose:     WinUI transient-owner and backdrop transaction tests
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/dialog.h"
#include "wx/settings.h"
#include "wx/weakref.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/shelltheme.h"
#include "wx/winui/private/transient.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"

#include <cstdint>
#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace
{

WXHWND FakeHandle(std::uintptr_t value)
{
    return reinterpret_cast<WXHWND>(value);
}

struct FakeOwnerWindow
{
    bool live = true;
    unsigned long long generation = 0;
    WXHWND owner = nullptr;
    WXHWND marker = nullptr;
};

struct FakeOwnerModel
{
    std::map<WXHWND, FakeOwnerWindow> windows;
    unsigned int setOwnerCalls = 0;
    unsigned int failSetOwnerBefore = 0;
    unsigned int failSetOwnerAfter = 0;
    unsigned int failSetMarker = 0;
    unsigned int failClearMarker = 0;
    std::function<void()> reenterSetOwner;

    static FakeOwnerModel& From(void *context)
    {
        return *static_cast<FakeOwnerModel *>(context);
    }

    static FakeOwnerWindow *Find(void *context, WXHWND hwnd)
    {
        FakeOwnerModel& model = From(context);
        const auto found = model.windows.find(hwnd);
        return found == model.windows.end() || !found->second.live
            ? nullptr
            : &found->second;
    }

    static unsigned long long GetGeneration(void *context, WXHWND hwnd)
    {
        const FakeOwnerWindow * const window = Find(context, hwnd);
        return window ? window->generation : 0;
    }

    static bool IsWindow(void *context, WXHWND hwnd)
    {
        return Find(context, hwnd) != nullptr;
    }

    static bool GetOwner(void *context, WXHWND hwnd, WXHWND *owner)
    {
        FakeOwnerWindow * const window = Find(context, hwnd);
        if ( !window || !owner )
            return false;

        *owner = window->owner;
        return true;
    }

    static bool SetOwner(void *context,
                         WXHWND hwnd,
                         WXHWND owner,
                         WXHWND *previous)
    {
        FakeOwnerModel& model = From(context);
        FakeOwnerWindow * const window = Find(context, hwnd);
        if ( !window || !previous )
            return false;

        ++model.setOwnerCalls;
        *previous = window->owner;
        if ( model.failSetOwnerBefore )
        {
            --model.failSetOwnerBefore;
            return false;
        }

        window->owner = owner;
        std::function<void()> reenter = std::move(model.reenterSetOwner);
        model.reenterSetOwner = std::function<void()>();
        if ( reenter )
            reenter();

        if ( model.failSetOwnerAfter )
        {
            --model.failSetOwnerAfter;
            return false;
        }
        return true;
    }

    static bool SetMarker(void *context, WXHWND hwnd, WXHWND owner)
    {
        FakeOwnerModel& model = From(context);
        FakeOwnerWindow * const window = Find(context, hwnd);
        if ( !window || !owner )
            return false;
        if ( model.failSetMarker )
        {
            --model.failSetMarker;
            return false;
        }

        window->marker = owner;
        return true;
    }

    static bool GetMarker(void *context, WXHWND hwnd, WXHWND *owner)
    {
        FakeOwnerWindow * const window = Find(context, hwnd);
        if ( !window || !owner )
            return false;

        *owner = window->marker;
        return true;
    }

    static bool ClearMarker(void *context, WXHWND hwnd)
    {
        FakeOwnerModel& model = From(context);
        FakeOwnerWindow * const window = Find(context, hwnd);
        if ( !window )
            return false;
        if ( model.failClearMarker )
        {
            --model.failClearMarker;
            return false;
        }

        window->marker = nullptr;
        return true;
    }

    wxWinUIOwnerNativeOps MakeOps()
    {
        wxWinUIOwnerNativeOps operations;
        operations.context = this;
        operations.getGeneration = GetGeneration;
        operations.isWindow = IsWindow;
        operations.getOwner = GetOwner;
        operations.setOwner = SetOwner;
        operations.setOwnerMarker = SetMarker;
        operations.getOwnerMarker = GetMarker;
        operations.clearOwnerMarker = ClearMarker;
        return operations;
    }
};

struct OwnerSeamGuard
{
    explicit OwnerSeamGuard(FakeOwnerModel& model)
    {
        wxWinUI3SetOwnerNativeOpsForTesting(model.MakeOps());
    }

    ~OwnerSeamGuard()
    {
        wxWinUI3ResetOwnerNativeOpsForTesting();
    }
};

struct FakeBackdropWindow
{
    bool live = true;
    unsigned long long generation = 0;
    wxWinUITransientRect rect;
    bool marker = false;
};

struct FakeBackdropModel
{
    struct Posted
    {
        void (*callback)(void *) = nullptr;
        void (*destroy)(void *) = nullptr;
        void *data = nullptr;
    };

    std::map<WXHWND, FakeBackdropWindow> windows;
    std::vector<Posted> posted;
    unsigned int setSizeCalls = 0;
    unsigned int failSetSizeBefore = 0;
    unsigned int failSetSizeAfter = 0;
    unsigned int failSetMarker = 0;
    unsigned int failClearMarker = 0;
    unsigned int failPost = 0;
    std::function<void()> reenterSetSize;

    static FakeBackdropModel& From(void *context)
    {
        return *static_cast<FakeBackdropModel *>(context);
    }

    static FakeBackdropWindow *Find(void *context, WXHWND hwnd)
    {
        FakeBackdropModel& model = From(context);
        const auto found = model.windows.find(hwnd);
        return found == model.windows.end() || !found->second.live
            ? nullptr
            : &found->second;
    }

    static unsigned long long GetGeneration(void *context, WXHWND hwnd)
    {
        const FakeBackdropWindow * const window = Find(context, hwnd);
        return window ? window->generation : 0;
    }

    static bool IsWindow(void *context, WXHWND hwnd)
    {
        return Find(context, hwnd) != nullptr;
    }

    static bool
    GetRect(void *context, WXHWND hwnd, wxWinUITransientRect *rect)
    {
        FakeBackdropWindow * const window = Find(context, hwnd);
        if ( !window || !rect )
            return false;

        *rect = window->rect;
        return true;
    }

    static bool
    SetSize(void *context, WXHWND hwnd, int width, int height)
    {
        FakeBackdropModel& model = From(context);
        FakeBackdropWindow * const window = Find(context, hwnd);
        if ( !window )
            return false;

        ++model.setSizeCalls;
        if ( model.failSetSizeBefore )
        {
            --model.failSetSizeBefore;
            return false;
        }

        window->rect.right = window->rect.left + width;
        window->rect.bottom = window->rect.top + height;
        std::function<void()> reenter = std::move(model.reenterSetSize);
        model.reenterSetSize = std::function<void()>();
        if ( reenter )
            reenter();

        if ( model.failSetSizeAfter )
        {
            --model.failSetSizeAfter;
            return false;
        }
        return true;
    }

    static bool SetMarker(void *context, WXHWND hwnd)
    {
        FakeBackdropModel& model = From(context);
        FakeBackdropWindow * const window = Find(context, hwnd);
        if ( !window )
            return false;
        if ( model.failSetMarker )
        {
            --model.failSetMarker;
            return false;
        }

        window->marker = true;
        return true;
    }

    static bool HasMarker(void *context, WXHWND hwnd)
    {
        const FakeBackdropWindow * const window = Find(context, hwnd);
        return window && window->marker;
    }

    static bool ClearMarker(void *context, WXHWND hwnd)
    {
        FakeBackdropModel& model = From(context);
        FakeBackdropWindow * const window = Find(context, hwnd);
        if ( !window )
            return false;
        if ( model.failClearMarker )
        {
            --model.failClearMarker;
            return false;
        }

        window->marker = false;
        return true;
    }

    static bool Post(void *context,
                     void (*callback)(void *),
                     void (*destroy)(void *),
                     void *data)
    {
        FakeBackdropModel& model = From(context);
        if ( model.failPost )
        {
            --model.failPost;
            return false;
        }

        model.posted.push_back({ callback, destroy, data });
        return true;
    }

    wxWinUIBackdropNativeOps MakeOps()
    {
        wxWinUIBackdropNativeOps operations;
        operations.context = this;
        operations.getGeneration = GetGeneration;
        operations.isWindow = IsWindow;
        operations.getRect = GetRect;
        operations.setSize = SetSize;
        operations.setPrimeMarker = SetMarker;
        operations.hasPrimeMarker = HasMarker;
        operations.clearPrimeMarker = ClearMarker;
        operations.post = Post;
        return operations;
    }

    void RunOne()
    {
        REQUIRE_FALSE(posted.empty());
        const Posted callback = posted.front();
        posted.erase(posted.begin());
        callback.callback(callback.data);
        callback.destroy(callback.data);
    }

    void RunAll()
    {
        while ( !posted.empty() )
        {
            const Posted callback = posted.front();
            posted.erase(posted.begin());
            callback.callback(callback.data);
            callback.destroy(callback.data);
        }
    }
};

struct BackdropSeamGuard
{
    explicit BackdropSeamGuard(FakeBackdropModel& model_)
        : model(model_)
    {
        wxWinUI3SetBackdropNativeOpsForTesting(model.MakeOps());
    }

    ~BackdropSeamGuard()
    {
        model.RunAll();
        wxWinUI3ResetBackdropNativeOpsForTesting();
    }

    FakeBackdropModel& model;
};

struct FakeShellThemeModel
{
    struct ThemeCall
    {
        WXHWND hwnd = nullptr;
        bool dark = false;
        bool highContrast = false;
    };

    bool backdropResult = false;
    bool extendFrameResult = true;
    WXHWND rejectMarkerHwnd = nullptr;
    bool rejectMarkerEnable = true;
    WXHWND rejectMarkerDisableHwnd = nullptr;
    WXHWND rejectControlThemeHwnd = nullptr;
    std::vector<std::pair<WXHWND, bool>> immersiveDarkCalls;
    std::vector<std::pair<WXHWND, bool>> backdropCalls;
    std::vector<std::pair<WXHWND, bool>> extendFrameCalls;
    std::vector<std::pair<WXHWND, bool>> markerCalls;
    std::vector<ThemeCall> controlThemeCalls;
    std::map<WXHWND, bool> backdropActive;
    std::map<WXHWND, bool> frameExtended;
    std::map<WXHWND, bool> markerActive;
    std::map<WXHWND, ThemeCall> controlThemeState;
    std::function<void()> reenterSetBackdrop;
    WXHWND reenterSetBackdropHwnd = nullptr;

    static FakeShellThemeModel& From(void *context)
    {
        return *static_cast<FakeShellThemeModel *>(context);
    }

    static bool SetImmersiveDark(void *context,
                                 WXHWND hwnd,
                                 bool dark)
    {
        From(context).immersiveDarkCalls.push_back({ hwnd, dark });
        return true;
    }

    static bool SetBackdrop(void *context,
                            WXHWND hwnd,
                            bool enabled)
    {
        FakeShellThemeModel& model = From(context);
        model.backdropCalls.push_back({ hwnd, enabled });
        if ( model.reenterSetBackdrop &&
             (!model.reenterSetBackdropHwnd ||
              model.reenterSetBackdropHwnd == hwnd) )
        {
            std::function<void()> reenter =
                std::move(model.reenterSetBackdrop);
            model.reenterSetBackdrop = std::function<void()>();
            reenter();
        }

        // Deliberately publish after re-entry. This models a synchronous
        // native call whose observable write completes after a nested theme
        // request and catches "epoch check only" latest-wins bugs.
        if ( !enabled || model.backdropResult )
            model.backdropActive[hwnd] = enabled;
        return enabled ? model.backdropResult : true;
    }

    static bool ExtendFrame(void *context,
                            WXHWND hwnd,
                            bool enabled)
    {
        FakeShellThemeModel& model = From(context);
        model.extendFrameCalls.push_back({ hwnd, enabled });
        if ( model.extendFrameResult )
            model.frameExtended[hwnd] = enabled;
        return model.extendFrameResult;
    }

    static bool SetMarker(void *context,
                          WXHWND hwnd,
                          bool enabled)
    {
        FakeShellThemeModel& model = From(context);
        model.markerCalls.push_back({ hwnd, enabled });
        if ( hwnd == model.rejectMarkerHwnd &&
             enabled == model.rejectMarkerEnable ||
             (!enabled && hwnd == model.rejectMarkerDisableHwnd) )
        {
            return false;
        }
        model.markerActive[hwnd] = enabled;
        return true;
    }

    static bool SetControlTheme(void *context,
                                WXHWND hwnd,
                                bool dark,
                                bool highContrast)
    {
        FakeShellThemeModel& model = From(context);
        const ThemeCall call{ hwnd, dark, highContrast };
        model.controlThemeCalls.push_back(call);
        if ( hwnd == model.rejectControlThemeHwnd )
            return false;
        model.controlThemeState[hwnd] = call;
        return true;
    }

    wxWinUIShellThemeNativeOps MakeOps()
    {
        wxWinUIShellThemeNativeOps operations;
        operations.context = this;
        operations.setImmersiveDarkMode = SetImmersiveDark;
        operations.setSystemBackdrop = SetBackdrop;
        operations.extendFrameIntoClientArea = ExtendFrame;
        operations.setBackdropMarker = SetMarker;
        operations.setControlTheme = SetControlTheme;
        return operations;
    }
};

struct ShellThemeSeamGuard
{
    explicit ShellThemeSeamGuard(FakeShellThemeModel& model)
    {
        wxWinUI3SetShellThemeNativeOpsForTesting(model.MakeOps());
    }

    ~ShellThemeSeamGuard()
    {
        wxWinUI3ResetShellThemeNativeOpsForTesting();
    }
};

struct HighContrastSeamGuard
{
    explicit HighContrastSeamGuard(
        wxWinUIHighContrastOverrideForTesting value)
    {
        wxWinUISetHighContrastOverrideForTesting(value);
    }

    ~HighContrastSeamGuard()
    {
        wxWinUISetHighContrastOverrideForTesting(
            wxWinUIHighContrastOverrideForTesting::System);
    }

    void Set(wxWinUIHighContrastOverrideForTesting value)
    {
        wxWinUISetHighContrastOverrideForTesting(value);
    }
};

wxWinUITransientRect TestRect(long left = 10,
                               long top = 20,
                               long width = 100,
                              long height = 100)
{
    wxWinUITransientRect rect;
    rect.left = left;
    rect.top = top;
    rect.right = left + width;
    rect.bottom = top + height;
    return rect;
}

} // anonymous namespace

TEST_CASE("WinUITransient::OwnerExactlyOnceAndNested",
          "[WinUITransient][owner][winui-beta-transients]")
{
    const WXHWND dialog = FakeHandle(0x100);
    const WXHWND owner = FakeHandle(0x200);
    FakeOwnerModel model;
    model.windows[dialog] = { true, 11, owner, nullptr };
    model.windows[owner] = { true, 21, nullptr, nullptr };
    OwnerSeamGuard guard(model);

    bool nestedActive = true;
    bool nestedInactive = true;
    model.reenterSetOwner =
        [&]()
        {
            nestedActive =
                wxWinUITransientOwnerActivation(dialog, true);
            nestedInactive =
                wxWinUITransientOwnerActivation(dialog, false);
        };

    CHECK(wxWinUITransientOwnerActivation(dialog, true));
    CHECK_FALSE(nestedActive);
    CHECK_FALSE(nestedInactive);
    CHECK(model.windows[dialog].owner == nullptr);
    CHECK(model.windows[dialog].marker == owner);
    CHECK(model.setOwnerCalls == 1);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 1);

    // Repeated activation coalesces into the published Detached epoch.
    CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, true));
    CHECK(model.setOwnerCalls == 1);

    CHECK(wxWinUITransientOwnerActivation(dialog, false));
    CHECK(model.windows[dialog].owner == owner);
    CHECK(model.windows[dialog].marker == nullptr);
    CHECK(model.setOwnerCalls == 2);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);

    CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
    CHECK(model.setOwnerCalls == 2);
}

TEST_CASE("WinUITransient::OwnerFaultAndExternalMutation",
          "[WinUITransient][owner]")
{
    const WXHWND dialog = FakeHandle(0x110);
    const WXHWND owner = FakeHandle(0x210);
    const WXHWND externalOwner = FakeHandle(0x310);
    FakeOwnerModel model;
    model.windows[dialog] = { true, 12, owner, nullptr };
    model.windows[owner] = { true, 22, nullptr, nullptr };
    model.windows[externalOwner] = { true, 32, nullptr, nullptr };
    OwnerSeamGuard guard(model);

    SECTION("SetProp failure leaves the owner untouched")
    {
        model.failSetMarker = 1;
        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, true));
        CHECK(model.windows[dialog].owner == owner);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 0);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }

    SECTION("SetWindowLongPtr failure is rolled back")
    {
        model.failSetOwnerAfter = 1;
        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, true));
        CHECK(model.windows[dialog].owner == owner);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 2);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }

    SECTION("A failed rollback preserves one truthful restore opportunity")
    {
        model.failSetOwnerAfter = 1;
        model.reenterSetOwner =
            [&model]()
            {
                model.failSetOwnerBefore = 1;
            };
        CHECK(wxWinUITransientOwnerActivation(dialog, true));
        CHECK(model.windows[dialog].owner == nullptr);
        CHECK(model.windows[dialog].marker == owner);
        CHECK(model.setOwnerCalls == 2);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 1);

        CHECK(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == owner);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 3);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }

    SECTION("A failed restore remains pending and retries exactly once")
    {
        REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
        model.failSetOwnerBefore = 1;

        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == nullptr);
        CHECK(model.windows[dialog].marker == owner);
        CHECK(model.setOwnerCalls == 2);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 1);

        CHECK(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == owner);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 3);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);

        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.setOwnerCalls == 3);
    }

    SECTION("An external owner installed while detached always wins")
    {
        REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
        model.windows[dialog].owner = externalOwner;
        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == externalOwner);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 1);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }

    SECTION("A recycled original owner is never restored")
    {
        REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
        ++model.windows[owner].generation;
        CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == nullptr);
        CHECK(model.setOwnerCalls == 1);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }

    SECTION("A failed RemoveProp does not permit a double restore")
    {
        REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
        model.failClearMarker = 1;
        CHECK(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.windows[dialog].owner == owner);
        CHECK(model.windows[dialog].marker == owner);
        CHECK(model.setOwnerCalls == 2);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);

        // The next detach first clears the stale diagnostic marker, but is a
        // new transaction and never repeats the already-consumed restore.
        CHECK(wxWinUITransientOwnerActivation(dialog, true));
        CHECK(model.setOwnerCalls == 3);
        CHECK(wxWinUITransientOwnerActivation(dialog, false));
        CHECK(model.setOwnerCalls == 4);
    }

    SECTION("Destroy consumes without restoring")
    {
        REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
        wxWinUITransientWindowDestroyed(dialog);
        CHECK(model.windows[dialog].owner == nullptr);
        CHECK(model.windows[dialog].marker == nullptr);
        CHECK(model.setOwnerCalls == 1);
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
    }
}

TEST_CASE("WinUITransient::OwnerHwndReuseIsFailClosed",
          "[WinUITransient][owner]")
{
    const WXHWND dialog = FakeHandle(0x120);
    const WXHWND owner = FakeHandle(0x220);
    const WXHWND replacementOwner = FakeHandle(0x320);
    FakeOwnerModel model;
    model.windows[dialog] = { true, 13, owner, nullptr };
    model.windows[owner] = { true, 23, nullptr, nullptr };
    model.windows[replacementOwner] = { true, 33, nullptr, nullptr };
    OwnerSeamGuard guard(model);

    REQUIRE(wxWinUITransientOwnerActivation(dialog, true));

    // Model USER32 destroying the old object and recycling its numeric HWND:
    // properties disappear and the new identity has its own owner.
    model.windows[dialog].generation = 14;
    model.windows[dialog].marker = nullptr;
    model.windows[dialog].owner = replacementOwner;

    CHECK_FALSE(wxWinUITransientOwnerActivation(dialog, false));
    CHECK(model.windows[dialog].owner == replacementOwner);
    CHECK(model.setOwnerCalls == 1);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
}

TEST_CASE("WinUITransient::OwnerMinimizeRestoresNestedTree",
          "[WinUITransient][owner][winui-beta-transients]")
{
    const WXHWND frame = FakeHandle(0x130);
    const WXHWND dialog = FakeHandle(0x230);
    const WXHWND nested = FakeHandle(0x330);
    FakeOwnerModel model;
    model.windows[frame] = { true, 15, nullptr, nullptr };
    model.windows[dialog] = { true, 25, frame, nullptr };
    model.windows[nested] = { true, 35, dialog, nullptr };
    OwnerSeamGuard guard(model);

    REQUIRE(wxWinUITransientOwnerActivation(dialog, true));
    REQUIRE(wxWinUITransientOwnerActivation(nested, true));
    REQUIRE(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 2);

    // This is the WM_SIZE/SIZE_MINIMIZED and WM_SHOWWINDOW(false) seam:
    // restoring the direct child recursively restores its nested transient.
    wxWinUITransientOwnerRestoreDependents(frame, false);
    CHECK(model.windows[dialog].owner == frame);
    CHECK(model.windows[nested].owner == dialog);
    CHECK(model.windows[dialog].marker == nullptr);
    CHECK(model.windows[nested].marker == nullptr);
    CHECK(model.setOwnerCalls == 4);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(dialog).transactionCount == 0);
}

TEST_CASE("WinUITransient::BackdropSuccessCoalesceAndNested",
          "[WinUITransient][backdrop]")
{
    const WXHWND dialog = FakeHandle(0x400);
    FakeBackdropModel model;
    model.windows[dialog] = { true, 41, TestRect(), false };
    BackdropSeamGuard guard(model);

    model.reenterSetSize =
        [dialog]()
        {
            wxWinUIPrimeBackdrop(dialog);
        };

    wxWinUIPrimeBackdrop(dialog);
    CHECK(model.windows[dialog].rect == TestRect(10, 20, 100, 99));
    CHECK(model.windows[dialog].marker);
    CHECK(model.setSizeCalls == 1);
    REQUIRE(model.posted.size() == 1);
    CHECK(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);

    wxWinUIPrimeBackdrop(dialog);
    CHECK(model.setSizeCalls == 1);
    CHECK(model.posted.size() == 1);

    model.RunOne();
    CHECK(model.windows[dialog].rect == TestRect());
    CHECK(model.windows[dialog].marker);
    CHECK(model.setSizeCalls == 2);
    CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);

    wxWinUIInvalidateBackdropPrime(dialog);
    CHECK_FALSE(model.windows[dialog].marker);
    wxWinUIPrimeBackdrop(dialog);
    CHECK(model.setSizeCalls == 3);
}

TEST_CASE("WinUITransient::BackdropFaultMatrix",
          "[WinUITransient][backdrop]")
{
    const WXHWND dialog = FakeHandle(0x410);
    FakeBackdropModel model;
    model.windows[dialog] = { true, 42, TestRect(), false };
    BackdropSeamGuard guard(model);

    SECTION("Shrink failure never publishes success")
    {
        model.failSetSizeBefore = 1;
        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Marker failure rolls the exact shrink back")
    {
        model.failSetMarker = 1;
        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Enqueue failure rolls the exact shrink back")
    {
        model.failPost = 1;
        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Restore gets one bounded deferred retry")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.failSetSizeBefore = 1;
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect(10, 20, 100, 99));
        REQUIRE(model.posted.size() == 1);
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Two restore failures preserve the exact rect for a later prime")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.failSetSizeBefore = 2;

        model.RunOne();
        REQUIRE(model.posted.size() == 1);
        model.RunOne();

        CHECK(model.windows[dialog].rect ==
              TestRect(10, 20, 100, 99));
        CHECK(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        const wxWinUITransientSnapshot pending =
            wxWinUI3GetBackdropSnapshotForTesting(dialog);
        CHECK_FALSE(pending.inFlight);
        CHECK(pending.restorePending);
        CHECK(pending.transactionCount == 1);

        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK(wxWinUI3GetBackdropSnapshotForTesting(dialog).
                  transactionCount == 0);
    }

    SECTION("Failure-after shrink cannot redefine the shortened rect")
    {
        model.failSetSizeAfter = 1;
        model.reenterSetSize =
            [&model]()
            {
                model.failSetSizeBefore = 1;
            };

        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.windows[dialog].rect ==
              TestRect(10, 20, 100, 99));
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK(model.posted.empty());
        CHECK(wxWinUI3GetBackdropSnapshotForTesting(dialog).
                  restorePending);

        wxWinUIInvalidateBackdropPrime(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK(wxWinUI3GetBackdropSnapshotForTesting(dialog).
                  transactionCount == 0);
    }

    SECTION("Failed marker removal remains explicitly retryable")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.RunOne();
        REQUIRE(model.windows[dialog].marker);
        REQUIRE(model.windows[dialog].rect == TestRect());

        model.failClearMarker = 1;
        wxWinUIInvalidateBackdropPrime(dialog);
        CHECK(model.windows[dialog].marker);

        const unsigned int callsBefore = model.setSizeCalls;
        wxWinUIPrimeBackdrop(dialog);
        CHECK(model.setSizeCalls == callsBefore + 1);
        CHECK(model.windows[dialog].rect == TestRect(10, 20, 100, 99));
        CHECK(model.windows[dialog].marker);
        CHECK(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }
}

TEST_CASE("WinUITransient::BackdropGenerationAndRectRevalidation",
          "[WinUITransient][backdrop]")
{
    const WXHWND dialog = FakeHandle(0x420);
    FakeBackdropModel model;
    model.windows[dialog] = { true, 43, TestRect(), false };
    BackdropSeamGuard guard(model);

    SECTION("Recycled HWND is never written by the stale callback")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.windows[dialog].generation = 44;
        model.windows[dialog].rect = TestRect(50, 60, 300, 200);
        model.windows[dialog].marker = false;
        const unsigned int callsBefore = model.setSizeCalls;
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect(50, 60, 300, 200));
        CHECK(model.setSizeCalls == callsBefore);
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("External resize supersedes instead of stale restore")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.windows[dialog].rect = TestRect(10, 20, 160, 140);
        const unsigned int callsBefore = model.setSizeCalls;
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect(10, 20, 160, 140));
        CHECK(model.setSizeCalls == callsBefore);
        CHECK(model.windows[dialog].marker);
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Move-only mutation is re-sampled before size restore")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        model.windows[dialog].rect = TestRect(70, 80, 100, 99);
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect(70, 80, 100, 99));
        REQUIRE(model.posted.size() == 1);
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect(70, 80, 100, 100));
        CHECK(model.windows[dialog].marker);
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
    }

    SECTION("Invalidation restores only the exact owned shrink")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        wxWinUIInvalidateBackdropPrime(dialog);
        CHECK(model.windows[dialog].rect == TestRect());
        CHECK_FALSE(model.windows[dialog].marker);
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
        model.RunOne();
        CHECK(model.windows[dialog].rect == TestRect());
    }

    SECTION("Destroy retires without resizing a dying HWND")
    {
        wxWinUIPrimeBackdrop(dialog);
        REQUIRE(model.posted.size() == 1);
        const unsigned int callsBefore = model.setSizeCalls;
        wxWinUIBackdropWindowDestroyed(dialog);
        CHECK(model.setSizeCalls == callsBefore);
        CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(dialog).inFlight);
        model.RunOne();
        CHECK(model.setSizeCalls == callsBefore);
    }
}

TEST_CASE("WinUITransient::ShellThemePolicyMatrix",
          "[WinUITransient][shell-theme][winui-009]")
{
    using Theme = wxWinUIAppTheme;

    wxWinUIShellThemePolicy policy =
        wxWinUI3ResolveShellThemePolicyForTesting(
            Theme::System, false, false);
    CHECK_FALSE(policy.highContrast);
    CHECK_FALSE(policy.dark);
    CHECK(policy.allowBackdrop);

    policy = wxWinUI3ResolveShellThemePolicyForTesting(
        Theme::System, true, false);
    CHECK_FALSE(policy.highContrast);
    CHECK(policy.dark);
    CHECK(policy.allowBackdrop);

    policy = wxWinUI3ResolveShellThemePolicyForTesting(
        Theme::Light, true, false);
    CHECK_FALSE(policy.highContrast);
    CHECK_FALSE(policy.dark);
    CHECK(policy.allowBackdrop);

    policy = wxWinUI3ResolveShellThemePolicyForTesting(
        Theme::Dark, false, false);
    CHECK_FALSE(policy.highContrast);
    CHECK(policy.dark);
    CHECK(policy.allowBackdrop);

    // High Contrast is authoritative even over an explicit Dark request.
    policy = wxWinUI3ResolveShellThemePolicyForTesting(
        Theme::Dark, true, true);
    CHECK(policy.highContrast);
    CHECK_FALSE(policy.dark);
    CHECK_FALSE(policy.allowBackdrop);
}

TEST_CASE("WinUITransient::ShellThemeHotMessages",
          "[WinUITransient][shell-theme][winui-009]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-test");
    REQUIRE(frame.GetHWND());
    const wxColour applicationBackground(17, 31, 47);
    REQUIRE(frame.SetBackgroundColour(applicationBackground));
    REQUIRE(frame.InheritsBackgroundColour());
    wxButton child(&frame, wxID_ANY, "native child");
    REQUIRE(child.GetHWND());
    const wxColour childBackground(91, 47, 23);
    REQUIRE(child.SetBackgroundColour(childBackground));

    FakeShellThemeModel model;
    // A failed material request keeps the test independent of the asynchronous
    // backdrop-prime machine while still recording the exact policy request.
    model.backdropResult = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOn);

    const WXHWND hwnd = frame.GetHWND();
    wxWinUIApplyWindowBackdrop(&frame);

    REQUIRE(model.immersiveDarkCalls.size() == 1);
    CHECK(model.immersiveDarkCalls.back() ==
          std::make_pair(hwnd, false));
    REQUIRE(model.backdropCalls.size() == 1);
    CHECK(model.backdropCalls.back() ==
          std::make_pair(hwnd, false));
    REQUIRE(model.extendFrameCalls.size() == 1);
    CHECK(model.extendFrameCalls.back() ==
          std::make_pair(hwnd, false));
    REQUIRE(model.markerCalls.size() == 2);
    CHECK(model.markerActive[hwnd] == false);
    CHECK(model.markerActive[child.GetHWND()] == false);
    REQUIRE(model.controlThemeCalls.size() == 2);
    CHECK_FALSE(model.controlThemeState[hwnd].dark);
    CHECK(model.controlThemeState[hwnd].highContrast);
    CHECK(model.controlThemeState[child.GetHWND()].highContrast);
    // The shell adapter must not turn application colours into a recursive
    // COLOR_WINDOW override. Native controls keep their role/palette and an
    // explicit wx colour remains application-owned.
    CHECK(frame.GetBackgroundColour() == applicationBackground);
    CHECK(frame.InheritsBackgroundColour());
    CHECK(child.GetBackgroundColour() == childBackground);
    CHECK(child.InheritsBackgroundColour());

    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    const wchar_t immersiveColorSet[] = L"ImmersiveColorSet";
    const std::size_t settingCallsBefore = model.backdropCalls.size();
    ::SendMessageW(
        nativeHwnd,
        WM_SETTINGCHANGE,
        0,
        reinterpret_cast<LPARAM>(immersiveColorSet));
    REQUIRE(model.backdropCalls.size() == settingCallsBefore + 1);
    CHECK_FALSE(model.backdropCalls.back().second);
    CHECK(model.controlThemeCalls.back().highContrast);

    const std::size_t colourCallsBefore = model.backdropCalls.size();
    ::SendMessageW(nativeHwnd, WM_SYSCOLORCHANGE, 0, 0);
    REQUIRE(model.backdropCalls.size() == colourCallsBefore + 1);
    CHECK_FALSE(model.backdropCalls.back().second);
    CHECK(model.controlThemeCalls.back().highContrast);

    // The next notification re-resolves the policy; no process restart or
    // actual desktop setting mutation is required.
    highContrast.Set(
        wxWinUIHighContrastOverrideForTesting::ForceOff);
    const std::size_t transitionCallsBefore = model.backdropCalls.size();
    ::SendMessageW(
        nativeHwnd,
        WM_SETTINGCHANGE,
        SPI_SETHIGHCONTRAST,
        reinterpret_cast<LPARAM>(immersiveColorSet));
    REQUIRE(model.backdropCalls.size() == transitionCallsBefore + 1);
    CHECK(model.backdropCalls.back().second);
    CHECK_FALSE(model.controlThemeCalls.back().highContrast);
    CHECK(frame.GetBackgroundColour() == applicationBackground);
    CHECK(frame.InheritsBackgroundColour());
    CHECK(child.GetBackgroundColour() == childBackground);
}

TEST_CASE("WinUITransient::ShellControlThemeFailureIsBestEffort",
          "[WinUITransient][shell-theme][winui-009][failure]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-best-effort");
    REQUIRE(frame.GetHWND());
    wxButton child(&frame, wxID_ANY, "native child");
    REQUIRE(child.GetHWND());

    FakeShellThemeModel model;
    model.rejectControlThemeHwnd = child.GetHWND();
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOn);

    wxWinUIApplyWindowBackdrop(&frame);

    // SetWindowTheme() is advisory for each native HWND. One control refusing
    // it must not roll back the top-level High Contrast/solid-surface policy
    // or prevent the remaining native controls from being updated.
    REQUIRE(model.controlThemeCalls.size() == 2);
    CHECK(model.controlThemeState.count(frame.GetHWND()) == 1);
    CHECK(model.controlThemeState.count(child.GetHWND()) == 0);
    CHECK_FALSE(model.backdropActive[frame.GetHWND()]);
    CHECK_FALSE(model.frameExtended[frame.GetHWND()]);
    CHECK_FALSE(model.markerActive[frame.GetHWND()]);
    CHECK_FALSE(model.markerActive[child.GetHWND()]);
}

TEST_CASE("WinUITransient::ShellThemeNestedLatestPolicyWins",
          "[WinUITransient][shell-theme][winui-009][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-nested-test");
    REQUIRE(frame.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    FakeShellThemeModel model;
    model.backdropResult = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    bool nested = false;
    model.reenterSetBackdrop =
        [&]()
        {
            nested = true;
            highContrast.Set(
                wxWinUIHighContrastOverrideForTesting::ForceOn);
            wxWinUIApplyWindowBackdrop(&frame);
        };

    wxWinUIApplyWindowBackdrop(&frame);

    CHECK(nested);
    REQUIRE(model.backdropCalls.size() == 2);
    CHECK(model.backdropCalls[0].second);
    CHECK_FALSE(model.backdropCalls[1].second);

    // The old native write completes after the nested request. The per-HWND
    // driver must therefore run the newer HC policy last, not merely abandon
    // the outer C++ pass after an epoch check.
    REQUIRE(model.extendFrameCalls.size() == 2);
    CHECK(model.extendFrameCalls[0].second);
    CHECK_FALSE(model.extendFrameCalls[1].second);
    REQUIRE(model.markerCalls.size() == 2);
    CHECK(model.markerCalls[0].second);
    CHECK_FALSE(model.markerCalls[1].second);
    REQUIRE(model.controlThemeCalls.size() == 2);
    CHECK(model.controlThemeCalls.back().highContrast);
    CHECK_FALSE(model.controlThemeCalls.back().dark);
    CHECK_FALSE(model.backdropActive[frame.GetHWND()]);
    CHECK_FALSE(model.frameExtended[frame.GetHWND()]);
    CHECK_FALSE(model.markerActive[frame.GetHWND()]);
    CHECK(model.controlThemeState[frame.GetHWND()].highContrast);
    CHECK_FALSE(frame.UseBackgroundColour());
}

TEST_CASE("WinUITransient::ShellThemeDriverRetiresDestroyedCurrentTLW",
          "[WinUITransient][shell-theme][winui-009][lifetime]")
{
    wxFrame * const doomed =
        new wxFrame(nullptr, wxID_ANY, "shell-theme-destroy-current");
    REQUIRE(doomed->GetHWND());
    const WXHWND doomedHwnd = doomed->GetHWND();
    wxWeakRef<wxWindow> lifetime(doomed);

    FakeBackdropModel backdropModel;
    backdropModel.windows[doomedHwnd] =
        { true, 1, TestRect(), false };
    BackdropSeamGuard backdropGuard(backdropModel);
    FakeShellThemeModel model;
    model.backdropResult = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    bool destroyed = false;
    model.reenterSetBackdropHwnd = doomedHwnd;
    model.reenterSetBackdrop =
        [doomed, &destroyed]()
        {
            destroyed = true;
            doomed->Destroy();
        };

    // Destruction occurs from the exact current driver's native callback. The
    // outer pass must not touch the dead wxWindow and its per-HWND epoch must
    // be retired before a future handle can reuse the numeric value.
    wxWinUIApplyWindowBackdrop(doomed);
    CHECK(destroyed);
    CHECK(wxPendingDelete.Member(doomed));
    CHECK(lifetime.get() == doomed);
    CHECK(model.extendFrameCalls.empty());
    CHECK(model.markerCalls.empty());
    CHECK(model.controlThemeCalls.empty());
    CHECK_FALSE(
        wxWinUI3GetBackdropSnapshotForTesting(doomedHwnd).inFlight);

    wxYield();
    wxYield();
    CHECK(lifetime.get() == nullptr);

    wxFrame replacement(
        nullptr, wxID_ANY, "shell-theme-after-destroy");
    REQUIRE(replacement.GetHWND());
    backdropModel.windows[replacement.GetHWND()] =
        { true, 2, TestRect(), false };
    wxWinUIApplyWindowBackdrop(&replacement);
    CHECK(model.controlThemeState.count(replacement.GetHWND()) == 1);
    CHECK(
        wxWinUI3GetBackdropSnapshotForTesting(
            replacement.GetHWND()).inFlight);
    backdropModel.RunAll();
    CHECK_FALSE(
        wxWinUI3GetBackdropSnapshotForTesting(
            replacement.GetHWND()).inFlight);
}

TEST_CASE("WinUITransient::ShellThemePartialMaterialRollsBack",
          "[WinUITransient][shell-theme][winui-009][failure]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-rollback-test");
    REQUIRE(frame.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    FakeShellThemeModel model;
    model.backdropResult = true;
    model.extendFrameResult = false;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    wxWinUIApplyWindowBackdrop(&frame);

    REQUIRE(model.backdropCalls.size() == 2);
    CHECK(model.backdropCalls[0].second);
    CHECK_FALSE(model.backdropCalls[1].second);
    REQUIRE(model.extendFrameCalls.size() == 2);
    CHECK(model.extendFrameCalls[0].second);
    CHECK_FALSE(model.extendFrameCalls[1].second);
    REQUIRE(model.markerCalls.size() == 1);
    CHECK_FALSE(model.markerCalls[0].second);
    CHECK_FALSE(model.backdropActive[frame.GetHWND()]);
    CHECK_FALSE(model.frameExtended[frame.GetHWND()]);
    CHECK_FALSE(model.markerActive[frame.GetHWND()]);
}

TEST_CASE("WinUITransient::FrameFailureCleanupFailureRetainsMaterial",
          "[WinUITransient][shell-theme][winui-009][failure]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-frame-cleanup-failure");
    wxButton child(&frame, wxID_ANY, "native-child");
    REQUIRE(frame.GetHWND());
    REQUIRE(child.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    const WXHWND root = frame.GetHWND();
    const WXHWND nativeChild = child.GetHWND();
    FakeBackdropModel backdropModel;
    backdropModel.windows[root] = { true, 1, TestRect(), false };
    BackdropSeamGuard backdropGuard(backdropModel);
    FakeShellThemeModel model;
    model.backdropResult = true;
    model.extendFrameResult = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    wxWinUIApplyWindowBackdrop(&frame);

    REQUIRE(model.backdropActive[root]);
    REQUIRE(model.frameExtended[root]);
    REQUIRE(model.markerActive[root]);
    REQUIRE(model.markerActive[nativeChild]);

    model.backdropCalls.clear();
    model.extendFrameCalls.clear();
    model.markerCalls.clear();
    model.controlThemeCalls.clear();
    model.extendFrameResult = false;
    model.rejectMarkerDisableHwnd = root;

    wxWinUIApplyWindowBackdrop(&frame);

    // The second activation cannot confirm its frame write. Its recovery must
    // clear every transparent marker before disabling DWM. Because the root
    // rejects that cleanup, the previously safe material/frame pair remains.
    REQUIRE(model.backdropCalls.size() == 1);
    CHECK(model.backdropCalls.front() == std::make_pair(root, true));
    REQUIRE(model.extendFrameCalls.size() == 1);
    CHECK(model.extendFrameCalls.front() == std::make_pair(root, true));
    REQUIRE(model.markerCalls.size() == 2);
    CHECK(model.markerCalls[0] == std::make_pair(root, false));
    CHECK(model.markerCalls[1] ==
          std::make_pair(nativeChild, false));
    CHECK(model.backdropActive[root]);
    CHECK(model.frameExtended[root]);
    CHECK(model.markerActive[root]);
    CHECK_FALSE(model.markerActive[nativeChild]);
    REQUIRE_FALSE(model.controlThemeCalls.empty());
    CHECK(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
    backdropModel.RunAll();
    CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
}

TEST_CASE("WinUITransient::ShellThemeMarkerFailureRollsBackWholeTree",
          "[WinUITransient][shell-theme][winui-009][failure]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-marker-rollback");
    wxButton child(&frame, wxID_ANY, "native-child");
    REQUIRE(frame.GetHWND());
    REQUIRE(child.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    FakeShellThemeModel model;
    model.backdropResult = true;
    model.extendFrameResult = true;
    model.rejectMarkerHwnd = child.GetHWND();
    model.rejectMarkerEnable = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    wxWinUIApplyWindowBackdrop(&frame);

    const WXHWND root = frame.GetHWND();
    const WXHWND nativeChild = child.GetHWND();
    REQUIRE(model.backdropCalls.size() == 2);
    CHECK(model.backdropCalls.front() ==
          std::make_pair(root, true));
    CHECK(model.backdropCalls.back() ==
          std::make_pair(root, false));
    REQUIRE(model.extendFrameCalls.size() == 2);
    CHECK(model.extendFrameCalls.front() ==
          std::make_pair(root, true));
    CHECK(model.extendFrameCalls.back() ==
          std::make_pair(root, false));

    // First phase tries to mark root+child; the recovery phase clears both,
    // including the child for which the enabling SetProp was rejected.
    REQUIRE(model.markerCalls.size() == 4);
    CHECK(model.markerCalls[0] == std::make_pair(root, true));
    CHECK(model.markerCalls[1] ==
          std::make_pair(nativeChild, true));
    CHECK(model.markerCalls[2] ==
          std::make_pair(root, false));
    CHECK(model.markerCalls[3] ==
          std::make_pair(nativeChild, false));
    CHECK_FALSE(model.backdropActive[root]);
    CHECK_FALSE(model.frameExtended[root]);
    CHECK_FALSE(model.markerActive[root]);
    CHECK_FALSE(model.markerActive[nativeChild]);
    REQUIRE_FALSE(model.controlThemeCalls.empty());
    const wxColour expectedFallback =
        model.controlThemeCalls.back().dark
            ? wxColour(32, 32, 32)
            : wxColour(243, 243, 243);
    CHECK(frame.GetBackgroundColour() == expectedFallback);
    CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
}

TEST_CASE("WinUITransient::ShellThemeMarkerCleanupFailureRetainsMaterial",
          "[WinUITransient][shell-theme][winui-009][failure]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-marker-cleanup-failure");
    wxButton child(&frame, wxID_ANY, "native-child");
    REQUIRE(frame.GetHWND());
    REQUIRE(child.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    const WXHWND root = frame.GetHWND();
    const WXHWND nativeChild = child.GetHWND();
    FakeBackdropModel backdropModel;
    backdropModel.windows[root] = { true, 1, TestRect(), false };
    BackdropSeamGuard backdropGuard(backdropModel);
    FakeShellThemeModel model;
    model.backdropResult = true;
    model.extendFrameResult = true;
    model.rejectMarkerHwnd = child.GetHWND();
    model.rejectMarkerEnable = true;
    model.rejectMarkerDisableHwnd = frame.GetHWND();
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    wxWinUIApplyWindowBackdrop(&frame);

    REQUIRE(model.backdropCalls.size() == 1);
    CHECK(model.backdropCalls.front() == std::make_pair(root, true));
    REQUIRE(model.extendFrameCalls.size() == 1);
    CHECK(model.extendFrameCalls.front() == std::make_pair(root, true));

    REQUIRE(model.markerCalls.size() == 4);
    CHECK(model.markerCalls[0] == std::make_pair(root, true));
    CHECK(model.markerCalls[1] ==
          std::make_pair(nativeChild, true));
    CHECK(model.markerCalls[2] == std::make_pair(root, false));
    CHECK(model.markerCalls[3] ==
          std::make_pair(nativeChild, false));

    // The root still carries its transparent marker because clearing it was
    // rejected. Keeping the material and extended frame active is the only
    // safe state: rolling DWM back here would expose a black client surface.
    CHECK(model.backdropActive[root]);
    CHECK(model.frameExtended[root]);
    CHECK(model.markerActive[root]);
    CHECK_FALSE(model.markerActive[nativeChild]);
    CHECK(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
    backdropModel.RunAll();
    CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
}

TEST_CASE("WinUITransient::HighContrastCleanupFailureRetainsMaterial",
          "[WinUITransient][shell-theme][winui-009][failure][high-contrast]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-theme-hc-cleanup-failure");
    wxButton child(&frame, wxID_ANY, "native-child");
    REQUIRE(frame.GetHWND());
    REQUIRE(child.GetHWND());
    frame.SetBackgroundColour(wxNullColour);

    const WXHWND root = frame.GetHWND();
    const WXHWND nativeChild = child.GetHWND();
    FakeBackdropModel backdropModel;
    backdropModel.windows[root] = { true, 1, TestRect(), false };
    BackdropSeamGuard backdropGuard(backdropModel);
    FakeShellThemeModel model;
    model.backdropResult = true;
    model.extendFrameResult = true;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    wxWinUIApplyWindowBackdrop(&frame);

    REQUIRE(model.backdropActive[root]);
    REQUIRE(model.frameExtended[root]);
    REQUIRE(model.markerActive[root]);
    REQUIRE(model.markerActive[nativeChild]);

    model.backdropCalls.clear();
    model.extendFrameCalls.clear();
    model.markerCalls.clear();
    model.controlThemeCalls.clear();
    model.rejectMarkerDisableHwnd = root;
    highContrast.Set(
        wxWinUIHighContrastOverrideForTesting::ForceOn);

    wxWinUIApplyWindowBackdrop(&frame);

    // Cleanup runs before the irreversible DWM/frame writes. The rejected root
    // marker therefore keeps its matching material instead of becoming a
    // transparent client over a disabled backdrop.
    CHECK(model.backdropCalls.empty());
    CHECK(model.extendFrameCalls.empty());
    REQUIRE(model.markerCalls.size() == 2);
    CHECK(model.markerCalls[0] == std::make_pair(root, false));
    CHECK(model.markerCalls[1] ==
          std::make_pair(nativeChild, false));
    CHECK(model.backdropActive[root]);
    CHECK(model.frameExtended[root]);
    CHECK(model.markerActive[root]);
    CHECK_FALSE(model.markerActive[nativeChild]);
    REQUIRE_FALSE(model.controlThemeCalls.empty());
    CHECK(model.controlThemeCalls.back().highContrast);
    CHECK(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
    backdropModel.RunAll();
    CHECK_FALSE(wxWinUI3GetBackdropSnapshotForTesting(root).inFlight);
}

TEST_CASE("WinUITransient::OwnedTopLevelsUseIndependentShellTransactions",
          "[WinUITransient][shell-theme][winui-009][multi-tlw]")
{
    wxFrame frame(nullptr, wxID_ANY, "shell-owner");
    wxButton child(&frame, wxID_ANY, "owner-child");
    wxDialog dialog(&frame, wxID_ANY, "owned-dialog");
    REQUIRE(frame.GetHWND());
    REQUIRE(child.GetHWND());
    REQUIRE(dialog.GetHWND());

    FakeShellThemeModel model;
    model.backdropResult = false;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);

    const WXHWND frameHwnd = frame.GetHWND();
    const WXHWND childHwnd = child.GetHWND();
    const WXHWND dialogHwnd = dialog.GetHWND();
    wxWinUIApplyWindowBackdrop(&frame);

    CHECK(model.controlThemeState.count(frameHwnd) == 1);
    CHECK(model.controlThemeState.count(childHwnd) == 1);
    CHECK(model.controlThemeState.count(dialogHwnd) == 0);
    CHECK(model.markerActive.count(dialogHwnd) == 0);
    for ( const auto& call : model.backdropCalls )
        CHECK(call.first != dialogHwnd);

    model.backdropCalls.clear();
    model.markerCalls.clear();
    model.controlThemeCalls.clear();
    wxWinUIApplyWindowBackdrop(&dialog);
    REQUIRE(model.backdropCalls.size() == 2);
    CHECK(model.backdropCalls[0] ==
          std::make_pair(dialogHwnd, true));
    CHECK(model.backdropCalls[1] ==
          std::make_pair(dialogHwnd, false));
    REQUIRE(model.markerCalls.size() == 1);
    CHECK(model.markerCalls[0].first == dialogHwnd);
    REQUIRE(model.controlThemeCalls.size() == 1);
    CHECK(model.controlThemeCalls[0].hwnd == dialogHwnd);
}

TEST_CASE("WinUITransient::AppThemeSnapshotSurvivesTLWDestruction",
          "[WinUITransient][shell-theme][winui-009][multi-tlw][lifetime]")
{
    wxFrame * const first =
        new wxFrame(nullptr, wxID_ANY, "theme-snapshot-first");
    wxFrame *second =
        new wxFrame(nullptr, wxID_ANY, "theme-snapshot-second");
    REQUIRE(first->GetHWND());
    REQUIRE(second->GetHWND());
    const WXHWND secondHwnd = second->GetHWND();

    FakeShellThemeModel model;
    model.backdropResult = false;
    ShellThemeSeamGuard nativeGuard(model);
    HighContrastSeamGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);
    std::size_t callsAtDestruction = 0;
    model.reenterSetBackdrop =
        [&]()
        {
            if ( second )
            {
                callsAtDestruction = model.backdropCalls.size();
                second->Destroy();
                second = nullptr;
            }
        };
    model.reenterSetBackdropHwnd = first->GetHWND();

    const wxWinUIAppTheme previousTheme = wxWinUIGetAppTheme();
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);

    REQUIRE(callsAtDestruction != 0);
    for ( std::size_t i = callsAtDestruction;
          i < model.backdropCalls.size();
          ++i )
    {
        CHECK(model.backdropCalls[i].first != secondHwnd);
    }
    wxWinUISetAppTheme(previousTheme);
    first->Destroy();
    wxYield();
}

#endif // __WXWINUI__ && wxUSE_WINUI3
