///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidialogcontracts.cpp
// Purpose:     Deterministic contract tests for WinUI common dialogs
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/winui/private/dialogcontracts.h"

#include "wx/app.h"
#include "wx/busyinfo.h"
#include "wx/busycursor.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/choice.h"
#include "wx/collpane.h"
#include "wx/dirdlg.h"
#include "wx/file.h"
#include "wx/filedlg.h"
#include "wx/filename.h"
#if wxUSE_COLOURDLG
    #include "wx/colordlg.h"
#endif
#include "wx/dialog.h"
#include "wx/fdrepdlg.h"
#include "wx/fontdlg.h"
#include "wx/frame.h"
#include "wx/msgdlg.h"
#include "wx/modalhook.h"
#include "wx/panel.h"
#include "wx/progdlg.h"
#include "wx/radiobox.h"
#include "wx/richmsgdlg.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/stockitem.h"
#include "wx/textdlg.h"
#include "wx/textctrl.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/msw/private/filedialog.h"
#include "wx/msw/private/gethwnd.h"
#include "wx/msw/private/msgdlg.h"
#include "wx/msw/private.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/transient.h"

#include "waitfor.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

#include <atomic>
#include <climits>
#include <initializer_list>
#include <limits>
#include <memory>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

class CountingModalHook final : public wxModalDialogHook
{
public:
    int Enter(wxDialog *dialog) override
    {
        ++enterCount;
        lastEnter = dialog;
        return wxID_NONE;
    }

    void Exit(wxDialog *dialog) override
    {
        ++exitCount;
        lastExit = dialog;
        if ( dialog )
        {
            exitHadLiveDialog =
                dialog->IsKindOf(wxCLASSINFO(wxDialog)) &&
                dialog->GetHandle() != nullptr;
            exitObservedDestroy = dialog->IsBeingDeleted();
            exitTitle = dialog->GetTitle();
        }
    }

    int enterCount = 0;
    int exitCount = 0;
    wxDialog *lastEnter = nullptr;
    wxDialog *lastExit = nullptr;
    wxString exitTitle;
    bool exitHadLiveDialog = false;
    bool exitObservedDestroy = false;
};

void CheckLayout(long style,
                 std::initializer_list<int> expectedIds,
                 std::size_t defaultIndex = 0)
{
    const wxWinUIMessageDialogLayout layout =
        wxWinUIBuildMessageDialogLayout(style);

    REQUIRE_FALSE(layout.requiresNativeFallback);
    REQUIRE(layout.buttonCount == expectedIds.size());
    CHECK(layout.defaultIndex == defaultIndex);

    std::size_t index = 0;
    for ( const int id : expectedIds )
        CHECK(layout.buttonIds[index++] == id);
}

std::size_t CountTopLevelEntries(const wxWindow *window)
{
    std::size_t count = 0;
    for ( wxWindow * const candidate : wxTopLevelWindows )
    {
        if ( candidate == window )
            ++count;
    }
    return count;
}

std::vector<wxWindow *> SnapshotTopLevelWindows()
{
    std::vector<wxWindow *> windows;
    for ( wxWindow * const window : wxTopLevelWindows )
        windows.push_back(window);
    return windows;
}

wxWindow *FindAddedTopLevel(const std::vector<wxWindow *>& before)
{
    for ( wxWindow * const candidate : wxTopLevelWindows )
    {
        bool existed = false;
        for ( wxWindow * const previous : before )
        {
            if ( previous == candidate )
            {
                existed = true;
                break;
            }
        }

        if ( !existed )
            return candidate;
    }

    return nullptr;
}

class ScopedDialogWindowPresentation
{
public:
    ScopedDialogWindowPresentation()
        : m_previous(wxWinUIGetDialogPresentation())
    {
        wxWinUISetDialogPresentation(wxWinUIDialogPresentation::Window);
    }

    ~ScopedDialogWindowPresentation()
    {
        wxWinUISetDialogPresentation(m_previous);
    }

private:
    const wxWinUIDialogPresentation m_previous;
};

wxRect NativeDialogRect(HWND hwnd)
{
    RECT rect{};
    if ( !::GetWindowRect(hwnd, &rect) )
        return wxRect();

    return wxRect(rect.left, rect.top,
                  rect.right - rect.left, rect.bottom - rect.top);
}

const wchar_t DialogOwnerMarker[] =
    L"wxWidgets.WinUI.StashedOwner."
    L"{32117897-FD59-4E46-88AD-C7B707EC7D2B}";

bool DialogHasNativeOwner(HWND hwnd, HWND owner)
{
    const HWND nativeOwner = ::GetWindow(hwnd, GW_OWNER);
    if ( nativeOwner )
        return nativeOwner == owner;

    // The WinUI activation protocol temporarily detaches the native owner
    // of an active backdrop window. Require its exact saved owner and HWND
    // generation, not merely a missing GW_OWNER. Hiding must restore the
    // native link and retire this transaction (checked after ShowModal).
    const auto snapshot = wxWinUI3GetOwnerSnapshotForTesting(hwnd);
    return ::GetPropW(hwnd, DialogOwnerMarker) == owner &&
           snapshot.epoch != 0 && !snapshot.inFlight &&
           snapshot.generation == wxWinUIMSWGetNativeHwndGeneration(hwnd);
}

struct DialogWindowIdentityObservation
{
    bool active = true;
    bool callbackRan = false;
    bool watchdogFired = false;
    int initEvents = 0;
    bool initFromPublicDialog = true;
    bool initWasModal = true;
    bool visible = false;
    bool modal = false;
    bool sameHandle = false;
    bool mappedToPublicDialog = false;
    bool sameParent = false;
    bool sameNativeOwner = false;
    bool ownerDisabled = false;
    bool noAddedTopLevel = false;
    bool publicContentSlot = false;
    bool titleMatches = false;
    wxRect initialRect;
    wxRect initialNativeRect;
    wxRect movedRect;
    wxRect movedNativeRect;
    bool observingGeometry = false;
    int sizeEvents = 0;
    int moveEvents = 0;
    bool sizeEventSourceMatches = true;
    bool moveEventSourceMatches = true;
    bool sizeEventGeometryMatches = true;
};

void ExercisePublicDialogWindow(wxDialog *dialog,
                                wxWindow& owner,
                                wxRect expectedRect)
{
    const wxWeakRef<wxDialog> weakDialog(dialog);
    const wxWeakRef<wxWindow> weakOwner(&owner);
    const HWND originalHwnd = wxGetHWND(dialog);
    const HWND ownerHwnd = wxGetHWND(&owner);
    REQUIRE(originalHwnd);
    REQUIRE(NativeDialogRect(originalHwnd) == expectedRect);
    REQUIRE(::GetWindow(originalHwnd, GW_OWNER) == ownerHwnd);

    CountingModalHook hook;
    hook.Register();
    const int results[] = { wxID_CANCEL, wxID_HIGHEST + 103, wxID_OK };
    for ( const int expectedResult : results )
    {
        INFO("EndModal result " << expectedResult);
        wxDialog *live = weakDialog.get();
        REQUIRE(live);
        REQUIRE_FALSE(live->IsBeingDeleted());
        const wxString title = wxString::Format("public identity %d", expectedResult);
        live->SetTitle(title);
        const auto before = SnapshotTopLevelWindows();
        const wxRect movedRect(expectedRect.GetPosition() + wxPoint(17, 23),
                               expectedRect.GetSize() + wxSize(31, 19));
        const auto observation =
            std::make_shared<DialogWindowIdentityObservation>();

        live->Bind(wxEVT_INIT_DIALOG,
            [weakDialog, originalHwnd, observation](wxInitDialogEvent& event)
            {
                event.Skip();
                if ( !observation->active )
                    return;
                wxDialog * const current = weakDialog.get();
                ++observation->initEvents;
                observation->initFromPublicDialog =
                    observation->initFromPublicDialog && current &&
                    event.GetEventObject() == current &&
                    wxGetHWND(current) == originalHwnd;
                observation->initWasModal =
                    observation->initWasModal && current && current->IsModal();
            });
        live->Bind(wxEVT_SIZE,
            [weakDialog, observation](wxSizeEvent& event)
            {
                event.Skip();
                if ( !observation->active || !observation->observingGeometry )
                    return;
                wxDialog * const current = weakDialog.get();
                ++observation->sizeEvents;
                observation->sizeEventSourceMatches =
                    observation->sizeEventSourceMatches && current &&
                    event.GetEventObject() == current;
                observation->sizeEventGeometryMatches =
                    observation->sizeEventGeometryMatches && current &&
                    event.GetSize() == current->GetSize();
            });
        live->Bind(wxEVT_MOVE,
            [weakDialog, observation](wxMoveEvent& event)
            {
                event.Skip();
                if ( !observation->active || !observation->observingGeometry )
                    return;
                ++observation->moveEvents;
                observation->moveEventSourceMatches =
                    observation->moveEventSourceMatches && weakDialog &&
                    event.GetEventObject() == weakDialog.get();
            });

        wxTimer watchdog;
        watchdog.Bind(wxEVT_TIMER,
            [weakDialog, observation](wxTimerEvent&)
            {
                if ( !observation->active )
                    return;
                observation->watchdogFired = true;
                if ( wxDialog * const current = weakDialog.get() )
                    current->Destroy();
            });
        REQUIRE(watchdog.StartOnce(5000));

        // Observe only after InitDialog and native showing have completed:
        // presenter setup itself may dispatch messages before modality starts.
        // Weak owners also make an already queued timer event harmless after
        // the session has returned or its watchdog has destroyed the dialog.
        wxTimer observeTimer;
        observeTimer.Bind(wxEVT_TIMER,
            [weakDialog, weakOwner, originalHwnd, ownerHwnd, observation,
             before, movedRect, title, expectedResult](wxTimerEvent&)
            {
                if ( !observation->active || observation->callbackRan )
                    return;
                wxDialog * const current = weakDialog.get();
                if ( !current || current->IsBeingDeleted() )
                    return;
                if ( !current->IsModal() || !current->IsShown() ||
                        !::IsWindowVisible(originalHwnd) )
                    return;
                observation->callbackRan = true;
                observation->visible = current->IsShown() &&
                    ::IsWindowVisible(originalHwnd) != FALSE;
                observation->modal = current->IsModal();
                observation->sameHandle = wxGetHWND(current) == originalHwnd;
                observation->mappedToPublicDialog =
                    wxFindWinFromHandle(originalHwnd) == current;
                observation->sameParent = weakOwner &&
                    current->GetParent() == weakOwner.get();
                observation->sameNativeOwner =
                    DialogHasNativeOwner(originalHwnd, ownerHwnd);
                observation->ownerDisabled =
                    ::IsWindowEnabled(ownerHwnd) == FALSE;
                observation->noAddedTopLevel =
                    FindAddedTopLevel(before) == nullptr &&
                    CountTopLevelEntries(current) == 1;
                wxWinUITopLevelHost * const host =
                    wxWinUITopLevelHost::FindForTLW(current);
                wxWinUISlot * const slot = host ? host->FindSlot(current) : nullptr;
                observation->publicContentSlot = slot && slot->GetContent();
                wchar_t nativeTitle[128]{};
                ::GetWindowTextW(originalHwnd, nativeTitle,
                                 static_cast<int>(WXSIZEOF(nativeTitle)));
                observation->titleMatches = current->GetTitle() == title &&
                    wxString(nativeTitle) == title;
                observation->initialRect = current->GetRect();
                observation->initialNativeRect = NativeDialogRect(originalHwnd);

                observation->observingGeometry = true;
                current->Move(movedRect.GetPosition());
                if ( weakDialog.get() != current || current->IsBeingDeleted() )
                    return;
                current->SetSize(movedRect.GetSize());
                if ( weakDialog.get() != current || current->IsBeingDeleted() )
                    return;
                observation->observingGeometry = false;
                observation->movedRect = current->GetRect();
                observation->movedNativeRect = NativeDialogRect(originalHwnd);
                current->EndModal(expectedResult);
            });
        REQUIRE(observeTimer.Start(10));

        const int entersBefore = hook.enterCount;
        const int exitsBefore = hook.exitCount;
        const int result = live->ShowModal();
        observeTimer.Stop();
        watchdog.Stop();
        observation->active = false;
        CHECK(result == expectedResult);
        CHECK_FALSE(observation->watchdogFired);
        CHECK(observation->callbackRan);
        CHECK(observation->initEvents == 1);
        CHECK(observation->initFromPublicDialog);
        CHECK(observation->initWasModal);
        CHECK(observation->visible);
        CHECK(observation->modal);
        CHECK(observation->sameHandle);
        CHECK(observation->mappedToPublicDialog);
        CHECK(observation->sameParent);
        CHECK(observation->sameNativeOwner);
        CHECK(observation->ownerDisabled);
        CHECK(observation->noAddedTopLevel);
        CHECK(observation->publicContentSlot);
        CHECK(observation->titleMatches);
        CHECK(observation->initialRect == expectedRect);
        CHECK(observation->initialNativeRect == expectedRect);
        CHECK(observation->movedRect == movedRect);
        CHECK(observation->movedNativeRect == movedRect);
        CHECK(observation->sizeEvents > 0);
        CHECK(observation->moveEvents > 0);
        CHECK(observation->sizeEventSourceMatches);
        CHECK(observation->moveEventSourceMatches);
        CHECK(observation->sizeEventGeometryMatches);
        CHECK(hook.enterCount == entersBefore + 1);
        CHECK(hook.exitCount == exitsBefore + 1);
        CHECK(hook.lastEnter == dialog);
        CHECK(hook.lastExit == dialog);
        CHECK(hook.exitHadLiveDialog);
        CHECK_FALSE(hook.exitObservedDestroy);
        CHECK(wxModalDialogHook::GetOpenCount() == 0);

        live = weakDialog.get();
        REQUIRE(live);
        REQUIRE_FALSE(live->IsBeingDeleted());
        CHECK(wxGetHWND(live) == originalHwnd);
        CHECK(::IsWindow(originalHwnd));
        CHECK_FALSE(live->IsModal());
        CHECK_FALSE(live->IsShown());
        CHECK_FALSE(::IsWindowVisible(originalHwnd));
        CHECK(live->GetReturnCode() == expectedResult);
        CHECK(::IsWindowEnabled(ownerHwnd));
        CHECK(::GetWindow(originalHwnd, GW_OWNER) == ownerHwnd);
        CHECK_FALSE(::GetPropW(originalHwnd, DialogOwnerMarker));
        CHECK(wxWinUI3GetOwnerSnapshotForTesting(originalHwnd).epoch == 0);
        CHECK(FindAddedTopLevel(before) == nullptr);
        expectedRect = movedRect;
    }
}

#if wxUSE_VALIDATORS

class DialogIdentityTextValidator final : public wxTextValidator
{
public:
    explicit DialogIdentityTextValidator(
        const std::shared_ptr<std::vector<wxString>>& values)
        : m_values(values)
    {
    }

    wxObject *Clone() const override
    {
        return new DialogIdentityTextValidator(*this);
    }

    wxString IsValid(const wxString& value) const override
    {
        m_values->push_back(value);
        return value == "accepted" ? wxString() : "validation veto";
    }

private:
    const std::shared_ptr<std::vector<wxString>> m_values;
};

MUXC::Button FindDialogButtonPeer(const MUX::UIElement& element,
                                const wxString& label)
{
    if ( !element )
        return nullptr;
    if ( const MUXC::Button button = element.try_as<MUXC::Button>() )
    {
        const auto text = winrt::unbox_value<winrt::hstring>(button.Content());
        return wxString(text.c_str()) == label ? button : nullptr;
    }
    if ( const MUXC::Border border = element.try_as<MUXC::Border>() )
        return FindDialogButtonPeer(border.Child(), label);
    if ( const MUXC::Panel panel = element.try_as<MUXC::Panel>() )
    {
        for ( const MUX::UIElement& child : panel.Children() )
        {
            if ( const MUXC::Button found = FindDialogButtonPeer(child, label) )
                return found;
        }
    }
    return nullptr;
}

#endif // wxUSE_VALIDATORS

template <typename T>
void CollectChildControls(wxWindow *parent, std::vector<T *>& controls)
{
    for ( wxWindow * const child : parent->GetChildren() )
    {
        if ( T * const control = wxDynamicCast(child, T) )
            controls.push_back(control);

        CollectChildControls(child, controls);
    }
}

void InvokeButtonPeer(wxWindow& target, int id)
{
    wxButton * const button =
        wxDynamicCast(target.FindWindow(id), wxButton);
    REQUIRE(button);

    const unsigned invokeAttempts =
        wxButton::WinUIGetPeerInvokeAttemptCountForTesting();
    REQUIRE(button->WinUIQueueClickForTesting());
    WaitFor(
        "WinUI button automation peer invocation",
        [invokeAttempts]()
        {
            return wxButton::WinUIGetPeerInvokeAttemptCountForTesting() >
                   invokeAttempts;
        });
}

struct TaskDialogHookContext
{
    HRESULT richResult = S_OK;
    int richButton = IDOK;
    BOOL richVerification = FALSE;
    bool destroyRichOwner = false;
    wxWindowRef richOwner;

    DWORD flags = 0;
    TASKDIALOG_COMMON_BUTTON_FLAGS commonButtons = TDCBF_OK_BUTTON;
    int defaultButton = 0;
    HWND parent = nullptr;
    wxString title;
    wxString mainInstruction;
    wxString content;
    wxString expandedInformation;
    wxString verificationText;
    wxString footer;
    PCWSTR mainIcon = nullptr;
    PCWSTR footerIcon = nullptr;
    std::vector<int> customButtonIds;

    std::atomic<bool> progressEntered{false};
    std::atomic<bool> progressExited{false};
    std::atomic<bool> requestSkip{false};
    std::atomic<bool> requestCancel{false};
    std::atomic<unsigned> skipDeliveries{0};
    std::atomic<unsigned> cancelDeliveries{0};
    std::atomic<unsigned> timerDeliveries{0};
};

wxString TaskDialogString(PCWSTR value)
{
    return value ? wxString(value) : wxString();
}

HRESULT WINAPI InvokeTaskDialogForTesting(
    void *rawContext,
    const TASKDIALOGCONFIG *config,
    int *button,
    int *WXUNUSED(radio),
    BOOL *verification)
{
    TaskDialogHookContext& context =
        *static_cast<TaskDialogHookContext *>(rawContext);

    context.flags = config->dwFlags;
    context.commonButtons = config->dwCommonButtons;
    context.defaultButton = config->nDefaultButton;
    context.parent = config->hwndParent;
    context.title = TaskDialogString(config->pszWindowTitle);
    context.mainInstruction =
        TaskDialogString(config->pszMainInstruction);
    context.content = TaskDialogString(config->pszContent);
    context.expandedInformation =
        TaskDialogString(config->pszExpandedInformation);
    context.verificationText =
        TaskDialogString(config->pszVerificationText);
    context.footer = TaskDialogString(config->pszFooter);
    context.mainIcon = config->pszMainIcon;
    context.footerIcon = config->pszFooterIcon;
    context.customButtonIds.clear();
    for ( UINT n = 0; n < config->cButtons; ++n )
        context.customButtonIds.push_back(config->pButtons[n].nButtonID);

    if ( !(config->dwFlags & TDF_SHOW_PROGRESS_BAR) )
    {
        if ( context.destroyRichOwner )
        {
            if ( wxWindow * const owner = context.richOwner.get() )
            {
                owner->Destroy();
                wxYield();
            }
        }

        if ( SUCCEEDED(context.richResult) )
        {
            if ( button )
                *button = context.richButton;
            if ( verification )
                *verification = context.richVerification;
        }
        return context.richResult;
    }

    if ( !config->pfCallback )
        return E_UNEXPECTED;
    context.progressEntered = true;
    config->pfCallback(
        nullptr, TDN_CREATED, 0, 0, config->lpCallbackData);

    int skipButton = 0;
    for ( const int id : context.customButtonIds )
    {
        if ( id != IDCANCEL )
        {
            skipButton = id;
            break;
        }
    }

    bool ended = false;
    for ( unsigned iteration = 0; iteration < 10000; ++iteration )
    {
        if ( context.requestSkip.exchange(false) && skipButton )
        {
            config->pfCallback(
                nullptr,
                TDN_BUTTON_CLICKED,
                static_cast<WPARAM>(skipButton),
                0,
                config->lpCallbackData);
            ++context.skipDeliveries;
        }

        if ( context.requestCancel.exchange(false) )
        {
            config->pfCallback(
                nullptr,
                TDN_BUTTON_CLICKED,
                IDCANCEL,
                0,
                config->lpCallbackData);
            ++context.cancelDeliveries;
        }

        const HRESULT timerResult =
            config->pfCallback(
                nullptr, TDN_TIMER, iteration, 0,
                config->lpCallbackData);
        ++context.timerDeliveries;
        if ( timerResult == S_OK )
        {
            ended = true;
            break;
        }

        wxMilliSleep(1);
    }

    config->pfCallback(
        nullptr, TDN_DESTROYED, 0, 0, config->lpCallbackData);
    context.progressExited = true;

    if ( button )
        *button = IDCANCEL;

    return ended ? S_OK : HRESULT_FROM_WIN32(ERROR_TIMEOUT);
}

class ScopedTaskDialogHook
{
public:
    explicit ScopedTaskDialogHook(TaskDialogHookContext& context)
    {
        wxMSWMessageDialog::wxTaskDialogIndirectHookForTesting hook;
        hook.context = &context;
        hook.invoke = InvokeTaskDialogForTesting;
        wxMSWMessageDialog::SetTaskDialogIndirectHookForTesting(hook);
    }

    ~ScopedTaskDialogHook()
    {
        wxMSWMessageDialog::ResetTaskDialogIndirectHookForTesting();
    }

private:
    wxDECLARE_NO_COPY_CLASS(ScopedTaskDialogHook);
};

#if wxUSE_FILEDLG && wxUSE_DIRDLG && wxUSE_IFILEOPENDIALOG

class ScopedPickerTempDir
{
public:
    ScopedPickerTempDir()
    {
        static unsigned long s_generation = 0;
        const wxString leaf = wxString::Format(
            "wx-winui-picker-%lu-%lu",
            static_cast<unsigned long>(wxGetProcessId()),
            ++s_generation);
        m_tempRoot = wxFileName::GetTempDir();
        m_path = m_tempRoot + wxFILE_SEP_PATH + leaf;
        m_created = wxFileName::Mkdir(
            m_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }

    ~ScopedPickerTempDir()
    {
        // Keep recursive cleanup mechanically constrained to the unique
        // directory created under the OS temporary directory.
        const wxString prefix =
            m_tempRoot + wxFILE_SEP_PATH + "wx-winui-picker-";
        if ( m_created && m_path.StartsWith(prefix) )
        {
            (void)wxFileName::Rmdir(
                m_path, wxPATH_RMDIR_RECURSIVE);
        }
    }

    bool IsOk() const { return m_created; }
    const wxString& GetPath() const { return m_path; }

    wxString MakeDirectory(const wxString& relative) const
    {
        const wxString path =
            m_path + wxFILE_SEP_PATH + relative;
        if ( !wxFileName::Mkdir(
                 path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
        {
            return wxString();
        }

        return path;
    }

    wxString MakeFile(const wxString& relative,
                      const wxString& contents = "test") const
    {
        const wxString path =
            m_path + wxFILE_SEP_PATH + relative;
        wxFile file;
        if ( !file.Create(path, true) ||
             !file.Write(contents) )
        {
            return wxString();
        }

        return path;
    }

private:
    wxString m_tempRoot;
    wxString m_path;
    bool m_created = false;
};

struct PickerHookResponse
{
    int rc = wxID_CANCEL;
    wxArrayString paths;
    wxString path;
};

struct PickerHookContext
{
    std::vector<PickerHookResponse> responses;
    std::vector<int> optionsSeen;
    std::vector<WXHWND> ownersSeen;
    std::size_t calls = 0;
    int maxModalDepth = 0;

    wxFrame *owner = nullptr;
    bool destroyOwner = false;
    bool minimizeOwner = false;
    int minimizeCycles = 0;

    bool invokeNestedDir = false;
    bool inNestedDir = false;
    wxString nestedInitialPath;
    int nestedResult = wxID_NONE;
};

int PickerShowHook(void *opaque,
                   WXHWND ownerHwnd,
                   int options,
                   wxArrayString *paths,
                   wxString *path)
{
    PickerHookContext& context =
        *static_cast<PickerHookContext *>(opaque);
    const std::size_t call = context.calls++;
    context.optionsSeen.push_back(options);
    context.ownersSeen.push_back(ownerHwnd);
    context.maxModalDepth = wxMax(
        context.maxModalDepth,
        wxModalDialogHook::GetOpenCount());

    if ( context.invokeNestedDir && !context.inNestedDir )
    {
        context.inNestedDir = true;
        wxDirDialog nested(
            context.owner,
            "nested directory",
            context.nestedInitialPath,
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        context.nestedResult = nested.ShowModal();
        context.inNestedDir = false;
    }

    if ( context.minimizeOwner && context.owner )
    {
        const HWND hwnd = static_cast<HWND>(wxGetHWND(context.owner));
        if ( hwnd )
        {
            (void)::SendMessage(hwnd, WM_SIZE, SIZE_MINIMIZED, 0);
            (void)::SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, 0);
            ++context.minimizeCycles;
        }
    }

    if ( context.destroyOwner && context.owner )
    {
        context.owner->Destroy();
        context.owner = nullptr;
        wxYield();
    }

    if ( call >= context.responses.size() )
        return wxID_CANCEL;

    const PickerHookResponse& response = context.responses[call];
    if ( paths )
        *paths = response.paths;
    if ( path )
        *path = response.path;
    return response.rc;
}

class ScopedPickerShowHook
{
public:
    explicit ScopedPickerShowHook(PickerHookContext& context)
    {
        wxMSWImpl::ResetIFileDialogShowHookForTesting();

        wxMSWImpl::wxIFileDialogShowHookForTesting hook;
        hook.context = &context;
        hook.show = &PickerShowHook;
        wxMSWImpl::SetIFileDialogShowHookForTesting(hook);
    }

    ~ScopedPickerShowHook()
    {
        wxMSWImpl::ResetIFileDialogShowHookForTesting();
    }

private:
    wxDECLARE_NO_COPY_CLASS(ScopedPickerShowHook);
};

#endif // wxUSE_FILEDLG && wxUSE_DIRDLG && wxUSE_IFILEOPENDIALOG

} // anonymous namespace

TEST_CASE("WinUIDialogContracts::MessageButtonMatrix",
          "[winui-dialog-contract][winui-v0-supported]")
{
    CheckLayout(wxOK, { wxID_OK });
    CheckLayout(wxOK | wxCANCEL,
                { wxID_OK, wxID_CANCEL });
    CheckLayout(wxOK | wxHELP,
                { wxID_OK, wxID_HELP });
    CheckLayout(wxOK | wxHELP | wxCANCEL,
                { wxID_OK, wxID_HELP, wxID_CANCEL });
    CheckLayout(wxOK | wxCANCEL | wxCANCEL_DEFAULT,
                { wxID_OK, wxID_CANCEL }, 1);

    CheckLayout(wxYES_NO,
                { wxID_YES, wxID_NO });
    CheckLayout(wxYES_NO | wxNO_DEFAULT,
                { wxID_YES, wxID_NO }, 1);
    CheckLayout(wxYES_NO | wxCANCEL,
                { wxID_YES, wxID_NO, wxID_CANCEL });
    CheckLayout(wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT,
                { wxID_YES, wxID_NO, wxID_CANCEL }, 2);
    CheckLayout(wxYES_NO | wxHELP,
                { wxID_YES, wxID_NO, wxID_HELP });

    CheckLayout(wxYES_NO | wxCANCEL | wxHELP,
                { wxID_YES, wxID_NO, wxID_CANCEL, wxID_HELP });

    CHECK_FALSE(
        wxWinUIBuildMessageDialogLayout(wxYES_NO).
            canDismissExternally);
    CHECK_FALSE(
        wxWinUIBuildMessageDialogLayout(wxYES_NO | wxHELP).
            canDismissExternally);
    CHECK(
        wxWinUIBuildMessageDialogLayout(wxYES_NO | wxCANCEL).
            canDismissExternally);
}

TEST_CASE("WinUIDialogContracts::MessageDismissalIsNotHelp",
          "[winui-dialog-contract][winui-v0-supported]")
{
    const wxWinUIMessageDialogLayout ok =
        wxWinUIBuildMessageDialogLayout(wxOK);
    CHECK(wxWinUIResolveMessageDialogDismissal(ok, wxID_CANCEL) ==
          wxID_OK);

    const wxWinUIMessageDialogLayout okHelp =
        wxWinUIBuildMessageDialogLayout(wxOK | wxHELP);
    CHECK(wxWinUIResolveMessageDialogDismissal(okHelp, wxID_CANCEL) ==
          wxID_CANCEL);
    CHECK(wxWinUIResolveMessageDialogDismissal(okHelp, wxID_HELP) ==
          wxID_HELP);

    const wxWinUIMessageDialogLayout yesNoHelp =
        wxWinUIBuildMessageDialogLayout(wxYES_NO | wxHELP);
    CHECK(wxWinUIResolveMessageDialogDismissal(
              yesNoHelp, wxID_CANCEL) == wxID_CANCEL);
    CHECK(wxWinUIResolveMessageDialogDismissal(
              yesNoHelp, wxID_HELP) == wxID_HELP);

    const wxWinUIMessageDialogLayout cancellable =
        wxWinUIBuildMessageDialogLayout(wxYES_NO | wxCANCEL);
    CHECK(wxWinUIResolveMessageDialogDismissal(
              cancellable, wxID_CANCEL) == wxID_CANCEL);
    CHECK(wxWinUIResolveMessageDialogDismissal(
              cancellable, wxID_YES) == wxID_YES);
}

TEST_CASE("WinUIDialogContracts::FourButtonFallbackPreservesLabels",
          "[winui-dialog-contract]")
{
    wxMessageDialog dialog(
        nullptr,
        "message",
        "caption",
        wxYES_NO | wxCANCEL | wxHELP | wxCANCEL_DEFAULT);
    REQUIRE(dialog.SetYesNoCancelLabels(
        "Absolutely", "Never", "Later"));
    REQUIRE(dialog.SetHelpLabel("Explain"));

    wxWinUIMessageDialogButtons model;
    REQUIRE(wxWinUIBuildMessageDialogButtons(dialog, model));
    REQUIRE(model.layout.buttonCount == 4);
    CHECK(model.values[0].id == wxID_YES);
    CHECK(model.values[0].label == "Absolutely");
    CHECK(model.values[1].id == wxID_NO);
    CHECK(model.values[1].label == "Never");
    CHECK(model.values[2].id == wxID_CANCEL);
    CHECK(model.values[2].label == "Later");
    CHECK(model.values[3].id == wxID_HELP);
    CHECK(model.values[3].label == "Explain");
    CHECK(model.layout.buttonIds[model.layout.defaultIndex] ==
          wxID_CANCEL);
}

TEST_CASE("WinUIDialogContracts::MessageLabelsPreserved",
          "[winui-dialog-contract][winui-v0-supported]")
{
    wxMessageDialog dialog(
        nullptr,
        "message",
        "caption",
        wxYES_NO | wxCANCEL | wxNO_DEFAULT);
    REQUIRE(dialog.SetYesNoCancelLabels(
        "Absolutely", "Never", "Later"));

    wxWinUIMessageDialogButtons model;
    REQUIRE(wxWinUIBuildMessageDialogButtons(dialog, model));
    REQUIRE(model.layout.buttonCount == 3);
    CHECK(model.values[0].id == wxID_YES);
    CHECK(model.values[0].label == "Absolutely");
    CHECK(model.values[1].id == wxID_NO);
    CHECK(model.values[1].label == "Never");
    CHECK(model.values[2].id == wxID_CANCEL);
    CHECK(model.values[2].label == "Later");
    CHECK(model.layout.buttonIds[model.layout.defaultIndex] == wxID_NO);
}

TEST_CASE("WinUIDialogContracts::WindowSurfaceOwnsThemeBackground",
          "[winui-dialog-contract][winui-dialog-window]"
          "[winui-v0-supported]")
{
    const wxWinUIDialogPresentation previousPresentation =
        wxWinUIGetDialogPresentation();
    const wxWinUIAppTheme previousTheme = wxWinUIGetAppTheme();
    wxScopeGuard restorePolicy = wxMakeGuard(
        [previousPresentation, previousTheme]()
        {
            wxWinUISetDialogPresentation(previousPresentation);
            wxWinUISetAppTheme(previousTheme);
        });
    wxUnusedVar(restorePolicy);

    wxWinUISetDialogPresentation(wxWinUIDialogPresentation::Window);
    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);

    wxFrame owner(
        nullptr, wxID_ANY, "dialog-surface-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();
    wxYield();

    wxMessageDialog * const dialog = new wxMessageDialog(
        &owner, "Visible message body", "themed window surface", wxOK);
    const wxWeakRef<wxWindow> weakDialog(dialog);
    const std::vector<wxWindow *> before = SnapshotTopLevelWindows();

    struct Observation
    {
        bool callbackRan = false;
        bool foundShell = false;
        bool foundSlot = false;
        bool foundSurface = false;
        bool foundBody = false;
        bool hasBackground = false;
        bool backgroundIsOpaque = false;
        bool themeMatches = false;
    } observation;

    wxTheApp->CallAfter(
        [&observation, before, weakDialog]()
        {
            observation.callbackRan = true;
            try
            {
                wxWindow * const shell = FindAddedTopLevel(before);
                observation.foundShell = shell != nullptr;
                wxWinUITopLevelHost * const host = shell
                    ? wxWinUITopLevelHost::FindForTLW(shell)
                    : nullptr;
                wxWinUISlot * const slot = host && shell
                    ? host->FindSlot(shell)
                    : nullptr;
                observation.foundSlot = slot != nullptr;

                const MUXC::Border surface = slot
                    ? slot->GetContent().try_as<MUXC::Border>()
                    : nullptr;
                observation.foundSurface = surface != nullptr;
                if ( surface )
                {
                    const MUXC::Grid body =
                        surface.Child().try_as<MUXC::Grid>();
                    observation.foundBody = body != nullptr;
                    observation.themeMatches =
                        body &&
                        surface.RequestedTheme() == MUX::ElementTheme::Dark &&
                        body.RequestedTheme() == MUX::ElementTheme::Dark;

                    const MUXM::SolidColorBrush background =
                        surface.Background()
                            .try_as<MUXM::SolidColorBrush>();
                    observation.hasBackground = background != nullptr;
                    observation.backgroundIsOpaque =
                        background && background.Color().A == 255;
                }
            }
            catch ( const winrt::hresult_error& )
            {
            }

            if ( wxWindow * const live = weakDialog.get() )
                live->Destroy();
        });

    // A wxOK-only message normalizes external dismissal to wxID_OK, matching
    // the public MessageDialog contract even though source destruction is
    // what ends this private presenter session.
    CHECK(dialog->ShowModal() == wxID_OK);
    CHECK(observation.callbackRan);
    CHECK(observation.foundShell);
    CHECK(observation.foundSlot);
    CHECK(observation.foundSurface);
    CHECK(observation.foundBody);
    CHECK(observation.hasBackground);
    CHECK(observation.backgroundIsOpaque);
    CHECK(observation.themeMatches);

    wxYield();
    owner.Hide();
}

TEST_CASE("WinUIDialogContracts::TextEntryInitialState",
          "[winui-dialog-contract][winui-v0-supported]")
{
    // This is intentionally a real default construction/destruction cycle:
    // the old WinUI header left scalar members (notably the validator pointer)
    // indeterminate and the destructor could delete garbage.
    wxTextEntryDialog dialog;
    CHECK(dialog.GetValue().empty());
#if wxUSE_VALIDATORS
    CHECK(dialog.GetTextValidator() == nullptr);
#endif

    CHECK(wxWinUITextEntryMaxLength(0) == 0);
    CHECK(wxWinUITextEntryMaxLength(1) == 1);
    CHECK(wxWinUITextEntryMaxLength(
              static_cast<unsigned long>(INT_MAX)) == INT_MAX);

    if ( (std::numeric_limits<unsigned long>::max)() >
            static_cast<unsigned long>(INT_MAX) )
    {
        CHECK(wxWinUITextEntryMaxLength(
                  (std::numeric_limits<unsigned long>::max)()) == INT_MAX);
    }

    dialog.SetValue("Mixed Case 42");
    dialog.ForceUpper();
    CHECK(dialog.GetValue() == "MIXED CASE 42");
}

TEST_CASE("WinUIDialogContracts::TextAndPasswordWindowPresenters",
          "[winui-dialog-contract][winui-dialog-window]"
          "[winui-v0-supported]")
{
    const wxWinUIDialogPresentation previous =
        wxWinUIGetDialogPresentation();
    wxScopeGuard restoreMode =
        wxMakeGuard(
            [previous]()
            {
                wxWinUISetDialogPresentation(previous);
            });
    wxUnusedVar(restoreMode);
    wxWinUISetDialogPresentation(wxWinUIDialogPresentation::Window);

    wxFrame owner(
        nullptr, wxID_ANY, "text-dialog-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    const auto exercise = [](wxTextEntryDialog *dialog)
    {
        const wxWeakRef<wxTextEntryDialog> weakDialog(dialog);
        bool callbackRan = false;
        bool valueSet = false;
        wxString observed;
        wxTheApp->CallAfter(
            [weakDialog, &callbackRan, &valueSet, &observed]()
            {
                if ( wxTextEntryDialog * const live = weakDialog.get() )
                {
                    callbackRan = true;
                    valueSet =
                        live->WinUISetPeerValueForTesting("mixed Value");
                    live->ForceUpper();
                    observed = live->WinUIGetPeerValueForTesting();
                    live->Destroy();
                }
            });

        CHECK(dialog->ShowModal() == wxID_CANCEL);
        CHECK(callbackRan);
        CHECK(valueSet);
        CHECK(observed == "MIXED VALUE");
        wxYield();
    };

    exercise(new wxTextEntryDialog(
        &owner, "text", "text window", "Initial"));
    exercise(new wxPasswordEntryDialog(
        &owner, "password", "password window", "Initial"));

    owner.Hide();
}

TEST_CASE("WinUIDialogContracts::PublicWindowIdentity",
          "[winui-dialog-contract][winui-dialog-window]"
          "[winui-dialog-identity][winui-v0-supported]")
{
    REQUIRE(wxModalDialogHook::GetOpenCount() == 0);
    const ScopedDialogWindowPresentation windowPresentation;
    wxUnusedVar(windowPresentation);
    wxFrame owner(nullptr, wxID_ANY, "public-dialog-owner",
                  wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();
    wxYield();

    // Explicit placement intentionally excludes wxCENTRE. Text entry also
    // has a constructor size; password/colour use their public pre-show API.
    const wxPoint position(-31000, -30960);
    const wxSize size(560, 460);
    wxDialog *dialog = nullptr;
    SECTION("Text")
    {
        dialog = new wxTextEntryDialog(&owner, "Text identity", "text",
                                       "Initial", wxOK | wxCANCEL,
                                       position, size);
        CHECK(dialog->GetRect() == wxRect(position, size));
    }
    SECTION("Password")
    {
        dialog = new wxPasswordEntryDialog(&owner, "Password identity",
                                           "password", "Initial",
                                           wxOK | wxCANCEL, position);
        CHECK(dialog->GetPosition() == position);
        dialog->SetSize(size);
    }
#if wxUSE_COLOURDLG
    SECTION("Colour")
    {
        dialog = new wxColourDialog(&owner);
        dialog->Move(position);
        dialog->SetSize(size);
    }
#endif
    REQUIRE(dialog);
    const wxWeakRef<wxDialog> weakDialog(dialog);
    const auto cleanup = wxMakeGuard([weakDialog]()
    {
        if ( wxDialog * const live = weakDialog.get() )
            live->Destroy();
        wxYield();
    });
    wxUnusedVar(cleanup);

    ExercisePublicDialogWindow(dialog, owner, wxRect(position, size));
}

#if wxUSE_VALIDATORS

TEST_CASE("WinUIDialogContracts::PublicWindowButtonValidation",
          "[winui-dialog-contract][winui-dialog-window]"
          "[winui-dialog-identity][winui-v0-supported]")
{
    const ScopedDialogWindowPresentation windowPresentation;
    wxUnusedVar(windowPresentation);
    wxFrame owner(nullptr, wxID_ANY, "validation-owner",
                  wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();
    wxTextEntryDialog *dialog = nullptr;
    SECTION("Text")
    {
        dialog = new wxTextEntryDialog(&owner, "Text validation", "text",
                                       "Initial");
    }
    SECTION("Password")
    {
        dialog = new wxPasswordEntryDialog(&owner, "Password validation",
                                           "password", "Initial");
    }
    REQUIRE(dialog);
    const wxWeakRef<wxTextEntryDialog> weakDialog(dialog);
    const auto cleanup = wxMakeGuard([weakDialog]()
    {
        if ( wxTextEntryDialog * const live = weakDialog.get() )
            live->Destroy();
        wxYield();
    });
    wxUnusedVar(cleanup);
    const HWND originalHwnd = wxGetHWND(dialog);
    const auto values = std::make_shared<std::vector<wxString>>();
    dialog->SetTextValidator(DialogIdentityTextValidator(values));

    struct Observation
    {
        bool active = true;
        bool watchdogFired = false;
        bool callbackFailed = false;
        unsigned invokes = 0;
        bool rejectedValueSet = false;
        bool acceptedValueSet = false;
        bool vetoKeptPublicWindowOpen = false;
        bool vetoDidNotCommit = false;
    };
    const auto observation = std::make_shared<Observation>();
    wxTimer watchdog;
    watchdog.Bind(wxEVT_TIMER,
        [weakDialog, observation](wxTimerEvent&)
        {
            if ( !observation->active )
                return;
            observation->watchdogFired = true;
            if ( wxTextEntryDialog * const live = weakDialog.get() )
                live->Destroy();
        });
    wxTimer invokeTimer;
    invokeTimer.Bind(wxEVT_TIMER,
        [weakDialog, originalHwnd, observation, values](wxTimerEvent&)
        {
            if ( !observation->active || observation->invokes == 2 )
                return;
            wxTextEntryDialog * const live = weakDialog.get();
            if ( !live || live->IsBeingDeleted() )
                return;
            if ( !live->IsModal() || !::IsWindowVisible(originalHwnd) )
                return;
            if ( observation->invokes == 1 && values->empty() )
                return;

            try
            {
                wxWinUITopLevelHost * const host =
                    wxWinUITopLevelHost::FindForTLW(live);
                wxWinUISlot * const slot = host ? host->FindSlot(live) : nullptr;
                const MUXC::Button button = slot
                    ? FindDialogButtonPeer(slot->GetContent(),
                        wxGetStockLabel(wxID_OK, wxSTOCK_WITHOUT_ELLIPSIS))
                    : nullptr;
                if ( !button )
                    return;

                if ( observation->invokes == 0 )
                {
                    observation->rejectedValueSet =
                        live->WinUISetPeerValueForTesting("rejected");
                }
                else
                {
                    observation->vetoKeptPublicWindowOpen = live->IsModal() &&
                        wxGetHWND(live) == originalHwnd &&
                        wxFindWinFromHandle(originalHwnd) == live &&
                        ::IsWindowVisible(originalHwnd) != FALSE;
                    observation->vetoDidNotCommit = live->GetValue() == "Initial";
                    observation->acceptedValueSet =
                        live->WinUISetPeerValueForTesting("accepted");
                }

                ++observation->invokes;
                // This invokes the actual XAML Button.Click delegate and the
                // presenter's validation/dismissal path, not wx button events
                // or an acceptance-result substitution.
                const winrt::Microsoft::UI::Xaml::Automation::Peers::
                    ButtonAutomationPeer peer(button);
                peer.Invoke();
            }
            catch ( const winrt::hresult_error& )
            {
                observation->callbackFailed = true;
                if ( wxTextEntryDialog * const current = weakDialog.get() )
                    current->Destroy();
            }
        });
    REQUIRE(watchdog.StartOnce(5000));
    REQUIRE(invokeTimer.Start(10));
    const int result = dialog->ShowModal();
    invokeTimer.Stop();
    watchdog.Stop();
    observation->active = false;

    CHECK(result == wxID_OK);
    CHECK_FALSE(observation->watchdogFired);
    CHECK_FALSE(observation->callbackFailed);
    CHECK(observation->invokes == 2);
    CHECK(observation->rejectedValueSet);
    CHECK(observation->acceptedValueSet);
    CHECK(observation->vetoKeptPublicWindowOpen);
    CHECK(observation->vetoDidNotCommit);
    REQUIRE(values->size() == 2);
    CHECK((*values)[0] == "rejected");
    CHECK((*values)[1] == "accepted");
    wxTextEntryDialog * const live = weakDialog.get();
    REQUIRE(live);
    REQUIRE_FALSE(live->IsBeingDeleted());
    CHECK(wxGetHWND(live) == originalHwnd);
    CHECK(live->GetValue() == "accepted");
    CHECK_FALSE(live->IsModal());
    CHECK_FALSE(live->IsShown());
}

#endif // wxUSE_VALIDATORS

TEST_CASE("WinUIDialogContracts::ColourBodyFitsWorkArea",
          "[winui-dialog-contract]")
{
    CHECK(wxWinUIPhysicalWorkAreaToDIP(
              wxSize(1920, 1080), 96) ==
          wxSize(1920, 1080));
    CHECK(wxWinUIPhysicalWorkAreaToDIP(
              wxSize(1920, 1080), 120) ==
          wxSize(1536, 864));
    CHECK(wxWinUIPhysicalWorkAreaToDIP(
              wxSize(1920, 1080), 144) ==
          wxSize(1280, 720));
    CHECK(wxWinUIPhysicalWorkAreaToDIP(
              wxSize(1920, 1080), 192) ==
          wxSize(960, 540));

    CHECK(wxWinUIComputeColourDialogBodySize(
              true, true, wxSize(1280, 720)) ==
          wxSize(380, 560));
    CHECK(wxWinUIComputeColourDialogBodySize(
              true, false, wxSize(1024, 768)) ==
          wxSize(380, 608));
    CHECK(wxWinUIComputeColourDialogBodySize(
              false, false, wxSize(800, 600)) ==
          wxSize(380, 440));
    CHECK(wxWinUIComputeColourDialogBodySize(
              true, true, wxSize(320, 300)) ==
          wxSize(240, 140));
}

TEST_CASE("WinUIDialogContracts::GrowOnlyRefit",
          "[winui-dialog-contract]")
{
    CHECK(wxWinUIComputeGrowOnlyClientSize(
              wxSize(100, 200), wxSize(300, 50)) ==
          wxSize(300, 200));

    wxDialog dialog(
        nullptr,
        wxID_ANY,
        "grow-only",
        wxPoint(-32000, -32000),
        wxSize(160, 100));
    wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(480, 320);
    dialog.SetSizer(sizer);
    dialog.SetClientSize(wxSize(120, 80));

    const wxSize before = dialog.GetClientSize();
    REQUIRE(wxWinUIRefitDialogGrowOnly(&dialog));
    const wxSize grown = dialog.GetClientSize();
    CHECK(grown.x >= 480);
    CHECK(grown.y >= 320);
    CHECK(grown.x >= before.x);
    CHECK(grown.y >= before.y);
    const wxSize grownMin = dialog.GetMinClientSize();
    CHECK(grownMin.x >= 480);
    CHECK(grownMin.y >= 320);

    // A second pass is stable, and reducing the desired size can never shrink
    // either the current size or the stable minimum grown by the first
    // generation.
    CHECK_FALSE(wxWinUIRefitDialogGrowOnly(&dialog));
    sizer->Clear();
    sizer->Add(40, 30);
    CHECK_FALSE(wxWinUIRefitDialogGrowOnly(&dialog));
    CHECK(dialog.GetClientSize() == grown);
    CHECK(dialog.GetMinClientSize() == grownMin);

    wxPanel notADialog(&dialog, wxID_ANY);
    CHECK_FALSE(wxWinUIRefitDialogGrowOnly(&notADialog));
}

#if wxUSE_FONTDLG && wxUSE_FINDREPLDLG
TEST_CASE("WinUIDialogContracts::GenericDialogsRefitAfterLoaded",
          "[winui-dialog-contract][winui-dialog-refit]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "generic-dialog owner",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    owner.ShowWithoutActivating();

    const auto waitUntilFitted = [](wxDialog& dialog)
    {
        REQUIRE(dialog.GetSizer());

        for ( int attempt = 0; attempt < 100; ++attempt )
        {
            wxYield();
            const wxSize desired =
                dialog.GetSizer()->ComputeFittingClientSize(&dialog);
            const wxSize current = dialog.GetClientSize();
            if ( current.x >= desired.x && current.y >= desired.y )
                return;
            wxMilliSleep(5);
        }

        const wxSize desired =
            dialog.GetSizer()->ComputeFittingClientSize(&dialog);
        const wxSize current = dialog.GetClientSize();
        INFO("current=" << current.x << "x" << current.y
             << ", desired=" << desired.x << "x" << desired.y);
        CHECK(current.x >= desired.x);
        CHECK(current.y >= desired.y);
    };

    {
        wxFontData data;
        wxFontDialog dialog(&owner, data);
        dialog.Move(-32000, -32000);
        dialog.SetClientSize(wxSize(80, 60));
        // Exercise the ordinary wxDialog::Show() route. The isolated-desktop
        // runner keeps this focus-safe while proving the production boundary.
        dialog.Show();
        waitUntilFitted(dialog);
        dialog.Hide();
    }

    {
        wxFindReplaceData data;
        wxFindReplaceDialog dialog(
            &owner,
            &data,
            "Find and replace",
            wxFR_REPLACEDIALOG);
        REQUIRE(dialog.GetHandle());
        dialog.Move(-32000, -32000);
        dialog.SetClientSize(wxSize(80, 60));
        dialog.ShowWithoutActivating();
        waitUntilFitted(dialog);
        dialog.Hide();
    }

    owner.Hide();
}
#endif // wxUSE_FONTDLG && wxUSE_FINDREPLDLG

#if wxUSE_FONTDLG
TEST_CASE("WinUIDialogContracts::FontDialogTransactionalContract",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-font-dialog]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "font owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    wxFontData data;
    const wxFont initial(
        72,
        wxFONTFAMILY_ROMAN,
        wxFONTSTYLE_ITALIC,
        wxFONTWEIGHT_BOLD,
        true);
    REQUIRE(initial.IsOk());
    data.SetInitialFont(initial);
    data.SetColour(*wxRED);
    data.EnableEffects(true);

    // The default constructor must remain a real two-phase constructor.
    wxFontDialog dialog;
    CHECK(dialog.GetHandle() == nullptr);
    REQUIRE(dialog.Create(&owner, data));
    REQUIRE(dialog.GetHandle() != nullptr);
    dialog.Move(-32000, -32000);

    wxChoice * const family =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_FAMILY), wxChoice);
    wxChoice * const style =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_STYLE), wxChoice);
    wxChoice * const weight =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_WEIGHT), wxChoice);
    wxChoice * const colour =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_COLOUR), wxChoice);
    wxChoice * const pointSize =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_SIZE), wxChoice);
    wxCheckBox * const underline =
        wxDynamicCast(dialog.FindWindow(wxID_FONT_UNDERLINE), wxCheckBox);
    REQUIRE(family);
    REQUIRE(style);
    REQUIRE(weight);
    REQUIRE(colour);
    REQUIRE(pointSize);
    REQUIRE(underline);
    CHECK(dialog.GetDefaultItem() == dialog.FindWindow(wxID_OK));
    CHECK(dialog.GetAffirmativeId() == wxID_OK);
    CHECK(dialog.GetEscapeId() == wxID_ANY);

    // Indices, rather than translated labels, carry the enum contract.
    CHECK(family->GetCount() == 6);
    CHECK(family->GetSelection() == 0);
    CHECK(style->GetSelection() == 1);
    CHECK(weight->GetSelection() == 2);
    CHECK(pointSize->GetStringSelection() == "72");
    CHECK(underline->GetValue());

    family->SetSelection(4);
    style->SetSelection(0);
    weight->SetSelection(1);
    pointSize->SetStringSelection("33");
    underline->SetValue(false);
    REQUIRE(colour->SetStringSelection("BLUE"));

    wxCommandEvent changed(wxEVT_CHOICE, wxID_FONT_FAMILY);
    changed.SetEventObject(family);
    REQUIRE(dialog.ProcessWindowEvent(changed));

    // Live preview changes are working state only.
    CHECK(dialog.GetFontData().GetColour() == *wxRED);
    CHECK_FALSE(dialog.GetFontData().GetChosenFont().IsOk());

    const wxWeakRef<wxWindow> weakDialog(&dialog);
    wxTheApp->CallAfter(
        [weakDialog]()
        {
            if ( wxDialog * const live =
                     wxDynamicCast(weakDialog.get(), wxDialog) )
            {
                live->EndModal(wxID_CANCEL);
            }
        });
    CHECK(dialog.ShowModal() == wxID_CANCEL);
    CHECK(dialog.GetFontData().GetColour() == *wxRED);
    CHECK_FALSE(dialog.GetFontData().GetChosenFont().IsOk());

    wxTheApp->CallAfter(
        [weakDialog]()
        {
            if ( wxDialog * const live =
                     wxDynamicCast(weakDialog.get(), wxDialog) )
            {
                live->EndModal(wxID_OK);
            }
        });
    CHECK(dialog.ShowModal() == wxID_OK);

    const wxFont chosen = dialog.GetFontData().GetChosenFont();
    REQUIRE(chosen.IsOk());
    CHECK(chosen.GetFamily() == wxFONTFAMILY_SWISS);
    CHECK(chosen.GetStyle() == wxFONTSTYLE_NORMAL);
    CHECK(chosen.GetWeight() == wxFONTWEIGHT_LIGHT);
    CHECK(chosen.GetPointSize() == 33);
    CHECK_FALSE(chosen.GetUnderlined());
    CHECK(dialog.GetFontData().GetColour() == *wxBLUE);
}

TEST_CASE("WinUIDialogContracts::FontDialogLayoutCyclesAndOwner",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-font-dialog][lifetime]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "font cycle owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    wxFontData data;
    data.SetInitialFont(
        wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
               wxFONTWEIGHT_NORMAL));

    {
        wxFontDialog dialog(&owner, data);
        dialog.Move(-32000, -32000);

        std::vector<wxStaticText *> labels;
        CollectChildControls(&dialog, labels);
        REQUIRE_FALSE(labels.empty());
        labels.front()->SetLabel(
            wxString(180, 'W') + wxString::FromUTF8(" — 日本語 — العربية"));
        labels.front()->InvalidateBestSize();
        dialog.SetClientSize(wxSize(80, 60));

        const wxSize before = dialog.GetClientSize();
        REQUIRE(wxWinUIRefitDialogGrowOnly(&dialog));
        const wxSize fitted = dialog.GetClientSize();
        CHECK(fitted.x >= before.x);
        CHECK(fitted.y >= before.y);
        CHECK(fitted.x >=
              dialog.GetSizer()->ComputeFittingClientSize(&dialog).x);
        CHECK(fitted.y >=
              dialog.GetSizer()->ComputeFittingClientSize(&dialog).y);
        CHECK_FALSE(wxWinUIRefitDialogGrowOnly(&dialog));
        CHECK(dialog.GetClientSize() == fitted);
    }

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxFontDialog dialog;
        REQUIRE(dialog.Create(&owner, data));
        REQUIRE(dialog.GetHandle());
        CHECK(dialog.FindWindow(wxID_OK));
        CHECK(dialog.FindWindow(wxID_CANCEL));
    }

    wxFrame * const doomedOwner =
        new wxFrame(
            nullptr,
            wxID_ANY,
            "font doomed owner",
            wxPoint(-32000, -32000),
            wxSize(420, 260));
    doomedOwner->ShowWithoutActivating();
    wxFontDialog * const doomedDialog =
        new wxFontDialog(doomedOwner, data);
    const wxWeakRef<wxWindow> weakDoomed(doomedDialog);
    wxTheApp->CallAfter(
        [doomedOwner]()
        {
            doomedOwner->Destroy();
        });
    CHECK(doomedDialog->ShowModal() == wxID_CANCEL);
    wxYield();
    CHECK_FALSE(weakDoomed.get());
}
#endif // wxUSE_FONTDLG

#if wxUSE_FINDREPLDLG
TEST_CASE("WinUIDialogContracts::FindReplaceEventsCloseAndReopen",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-find-dialog]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "find owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    wxFindReplaceData data(wxFR_DOWN | wxFR_MATCHCASE);
    data.SetFindString("initial");
    data.SetReplaceString("replacement");

    wxFindReplaceDialog dialog;
    CHECK(dialog.GetHandle() == nullptr);
    REQUIRE(dialog.Create(
        &owner,
        &data,
        "Find — 日本語",
        wxFR_REPLACEDIALOG));
    REQUIRE(dialog.GetHandle());
    dialog.Move(-32000, -32000);

    std::vector<wxTextCtrl *> texts;
    std::vector<wxCheckBox *> checks;
    std::vector<wxRadioBox *> directions;
    CollectChildControls(&dialog, texts);
    CollectChildControls(&dialog, checks);
    CollectChildControls(&dialog, directions);
    REQUIRE(texts.size() == 2);
    REQUIRE(checks.size() == 2);
    REQUIRE(directions.size() == 1);
    CHECK(dialog.GetDefaultItem() == dialog.FindWindow(wxID_FIND));
    CHECK(dialog.FindWindow(wxID_CANCEL));

    CHECK(texts[0]->GetValue() == "initial");
    CHECK(texts[1]->GetValue() == "replacement");
    CHECK_FALSE(checks[0]->GetValue());
    CHECK(checks[1]->GetValue());
    CHECK(directions[0]->GetSelection() == 1);

    struct EventRecord
    {
        wxEventType type;
        int id;
        wxString find;
        wxString replace;
        int flags;
        wxFindReplaceDialog *dialog;
    };
    std::vector<EventRecord> events;
    const auto record =
        [&events](wxFindDialogEvent& event)
        {
            events.push_back(
                {
                    event.GetEventType(),
                    event.GetId(),
                    event.GetFindString(),
                    event.GetReplaceString(),
                    event.GetFlags(),
                    event.GetDialog()
                });
        };
    owner.Bind(wxEVT_FIND, record);
    owner.Bind(wxEVT_FIND_NEXT, record);
    owner.Bind(wxEVT_FIND_REPLACE, record);
    owner.Bind(wxEVT_FIND_REPLACE_ALL, record);

    const wxString longUnicode =
        wxString(300, 'x') +
        wxString::FromUTF8(" — résumé — 日本語 — العربية");
    texts[0]->SetValue(longUnicode);
    texts[1]->SetValue(wxString::FromUTF8("替換 — β"));
    checks[0]->SetValue(true);
    checks[1]->SetValue(true);
    directions[0]->SetSelection(0);

    InvokeButtonPeer(dialog, wxID_FIND);
    InvokeButtonPeer(dialog, wxID_FIND);
    InvokeButtonPeer(dialog, wxID_REPLACE);
    InvokeButtonPeer(dialog, wxID_REPLACE_ALL);

    REQUIRE(events.size() == 4);
    CHECK(events[0].type == wxEVT_FIND);
    CHECK(events[1].type == wxEVT_FIND_NEXT);
    CHECK(events[2].type == wxEVT_FIND_REPLACE);
    CHECK(events[3].type == wxEVT_FIND_REPLACE_ALL);
    for ( const EventRecord& event : events )
    {
        CHECK(event.id == dialog.GetId());
        CHECK(event.dialog == &dialog);
        CHECK(event.find == longUnicode);
        CHECK(event.flags == (wxFR_WHOLEWORD | wxFR_MATCHCASE));
    }
    CHECK(events[2].replace == wxString::FromUTF8("替換 — β"));
    CHECK(events[3].replace == wxString::FromUTF8("替換 — β"));
    CHECK(data.GetFindString() == longUnicode);
    CHECK(data.GetReplaceString() == wxString::FromUTF8("替換 — β"));
    CHECK(data.GetFlags() == (wxFR_WHOLEWORD | wxFR_MATCHCASE));

    texts[0]->Clear();
    wxUpdateUIEvent updateFind(wxID_FIND);
    updateFind.SetEventObject(dialog.FindWindow(wxID_FIND));
    REQUIRE(dialog.ProcessWindowEvent(updateFind));
    CHECK_FALSE(updateFind.GetEnabled());
    texts[0]->SetValue(longUnicode);

    int closeEvents = 0;
    owner.Bind(
        wxEVT_FIND_CLOSE,
        [&closeEvents, &data](wxFindDialogEvent& event)
        {
            ++closeEvents;
            REQUIRE(event.GetDialog());
            // Client data was synchronized, copied and detached before
            // entering arbitrary close handlers.
            CHECK(event.GetDialog()->GetData() != &data);
            REQUIRE(event.GetDialog()->GetData());
            CHECK(event.GetDialog()->GetData()->GetFindString() ==
                  event.GetFindString());

            // Reentrant close must not emit a duplicate.
            (void)event.GetDialog()->Close();
        });

    dialog.ShowWithoutActivating();
    InvokeButtonPeer(dialog, wxID_CANCEL);
    CHECK_FALSE(dialog.IsShown());
    CHECK(closeEvents == 1);
    InvokeButtonPeer(dialog, wxID_CANCEL);
    CHECK(closeEvents == 1);

    dialog.ShowWithoutActivating();
    CHECK(dialog.IsShown());
    REQUIRE(dialog.Close());
    CHECK_FALSE(dialog.IsShown());
    CHECK(closeEvents == 2);
}

TEST_CASE("WinUIDialogContracts::FindReplaceCyclesAndOwnerDestroy",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-find-dialog][lifetime]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "find cycle owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxFindReplaceData data(
            cycle % 2 ? wxFR_DOWN : wxFR_WHOLEWORD);
        wxFindReplaceDialog dialog(
            &owner, &data, "find cycle", wxFR_REPLACEDIALOG);
        REQUIRE(dialog.GetHandle());
        dialog.ShowWithoutActivating();
        InvokeButtonPeer(dialog, wxID_CANCEL);
        CHECK_FALSE(dialog.IsShown());
    }

    class OwnerWithData final : public wxFrame
    {
    public:
        OwnerWithData()
            : wxFrame(
                  nullptr,
                  wxID_ANY,
                  "find doomed owner",
                  wxPoint(-32000, -32000),
                  wxSize(420, 260))
        {
        }

        wxFindReplaceData data;
    };

    OwnerWithData * const doomedOwner = new OwnerWithData;
    doomedOwner->ShowWithoutActivating();
    wxFindReplaceDialog * const doomedDialog =
        new wxFindReplaceDialog(
            doomedOwner,
            &doomedOwner->data,
            "find doomed",
            wxFR_REPLACEDIALOG);
    const wxWeakRef<wxWindow> weakDialog(doomedDialog);
    doomedDialog->ShowWithoutActivating();
    doomedOwner->Destroy();
    wxYield();
    CHECK_FALSE(weakDialog.get());
}
#endif // wxUSE_FINDREPLDLG

#if wxUSE_FILEDLG && wxUSE_DIRDLG && wxUSE_IFILEOPENDIALOG
TEST_CASE("WinUIDialogContracts::NativePickerResultTransactions",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-picker-shell-adapter]")
{
    ScopedPickerTempDir temp;
    REQUIRE(temp.IsOk());

    const wxString unicodeDir =
        temp.MakeDirectory(wxString::FromUTF8("sélection-日本語"));
    const wxString longDir = temp.MakeDirectory(
        wxString(55, 'a') + wxFILE_SEP_PATH +
        wxString(55, 'b') + wxFILE_SEP_PATH +
        wxString::FromUTF8("long-路径"));
    REQUIRE_FALSE(unicodeDir.empty());
    REQUIRE_FALSE(longDir.empty());

    const wxString initialFile = temp.MakeFile("initial.txt");
    const wxString unicodeFile =
        temp.MakeFile(wxString::FromUTF8("sélection-日本語.txt"));
    const wxString secondFile = temp.MakeFile("second.cpp");
    const wxString savedFile = temp.MakeFile("saved.txt");
    REQUIRE_FALSE(initialFile.empty());
    REQUIRE_FALSE(unicodeFile.empty());
    REQUIRE_FALSE(secondFile.empty());
    REQUIRE_FALSE(savedFile.empty());

    wxFrame owner(
        nullptr,
        wxID_ANY,
        "picker owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();
    REQUIRE(wxGetHWND(&owner));

    PickerHookContext context;
    context.owner = &owner;

    PickerHookResponse cancelFile;
    cancelFile.rc = wxID_CANCEL;
    cancelFile.path = unicodeFile;
    context.responses.push_back(cancelFile);

    PickerHookResponse singleFile;
    singleFile.rc = wxID_OK;
    singleFile.path = unicodeFile;
    context.responses.push_back(singleFile);

    PickerHookResponse multipleFiles;
    multipleFiles.rc = wxID_OK;
    multipleFiles.paths.Add(unicodeFile);
    multipleFiles.paths.Add(secondFile);
    context.responses.push_back(multipleFiles);

    PickerHookResponse cancelDir;
    cancelDir.rc = wxID_CANCEL;
    cancelDir.path = unicodeDir;
    context.responses.push_back(cancelDir);

    PickerHookResponse singleDir;
    singleDir.rc = wxID_OK;
    singleDir.path = longDir;
    context.responses.push_back(singleDir);

    PickerHookResponse multipleDirs;
    multipleDirs.rc = wxID_OK;
    multipleDirs.paths.Add(unicodeDir);
    multipleDirs.paths.Add(longDir);
    context.responses.push_back(multipleDirs);

    // An injected "OK" without a filesystem result must never become a fake
    // success at the public API boundary.
    PickerHookResponse emptySuccess;
    emptySuccess.rc = wxID_OK;
    context.responses.push_back(emptySuccess);

    PickerHookResponse saveFile;
    saveFile.rc = wxID_OK;
    saveFile.path = savedFile;
    context.responses.push_back(saveFile);

    ScopedPickerShowHook hook(context);

    {
        wxFileDialog dialog(
            &owner,
            "cancel file",
            temp.GetPath(),
            "initial.txt",
            "Text (*.txt)|*.txt|C++ (*.cpp)|*.cpp",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        dialog.SetPath(initialFile);
        CHECK(dialog.ShowModal() == wxID_CANCEL);
        CHECK(dialog.GetPath() == initialFile);
        CHECK(dialog.GetFilename() == "initial.txt");
    }

    {
        wxFileDialog dialog(
            &owner,
            "single file",
            temp.GetPath(),
            wxString(),
            "Text (*.txt)|*.txt|C++ (*.cpp)|*.cpp",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        dialog.SetFilterIndex(999);
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK(dialog.GetPath() == unicodeFile);
        CHECK(dialog.GetDirectory() == wxFileName(unicodeFile).GetPath());
        CHECK(dialog.GetFilename() ==
              wxFileName(unicodeFile).GetFullName());
        CHECK(dialog.GetFilterIndex() == 0);
    }

    {
        wxFileDialog dialog(
            &owner,
            "multiple files",
            temp.GetPath(),
            wxString(),
            "All (*.*)|*.*",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
        CHECK(dialog.ShowModal() == wxID_OK);

        wxArrayString paths;
        wxArrayString names;
        dialog.GetPaths(paths);
        dialog.GetFilenames(names);
        REQUIRE(paths.size() == 2);
        REQUIRE(names.size() == 2);
        CHECK(paths[0] == unicodeFile);
        CHECK(paths[1] == secondFile);
        CHECK(names[0] == wxFileName(unicodeFile).GetFullName());
        CHECK(names[1] == wxFileName(secondFile).GetFullName());
    }

    {
        wxDirDialog dialog(
            &owner,
            "cancel directory",
            temp.GetPath(),
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        CHECK(dialog.ShowModal() == wxID_CANCEL);
        CHECK(dialog.GetPath() == temp.GetPath());
    }

    {
        wxDirDialog dialog(
            &owner,
            "single directory",
            unicodeDir,
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK(dialog.GetPath() == longDir);
    }

    {
        wxDirDialog dialog(
            &owner,
            "multiple directories",
            unicodeDir,
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST |
                wxDD_MULTIPLE | wxDD_SHOW_HIDDEN);
        CHECK(dialog.ShowModal() == wxID_OK);

        wxArrayString paths;
        dialog.GetPaths(paths);
        REQUIRE(paths.size() == 2);
        CHECK(paths[0] == unicodeDir);
        CHECK(paths[1] == longDir);
    }

    {
        wxFileDialog dialog(
            &owner,
            "empty success",
            temp.GetPath(),
            wxString(),
            "All (*.*)|*.*",
            wxFD_OPEN);
        CHECK(dialog.ShowModal() == wxID_CANCEL);
    }

    {
        wxFileDialog dialog(
            &owner,
            "save file",
            temp.GetPath(),
            "saved.txt",
            "Text (*.txt)|*.txt",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK(dialog.GetPath() == savedFile);
    }

    REQUIRE(context.calls == 8);
    REQUIRE(context.ownersSeen.size() == 8);
    REQUIRE(context.optionsSeen.size() == 8);
    for ( WXHWND ownerSeen : context.ownersSeen )
        CHECK(ownerSeen == wxGetHWND(&owner));

    CHECK((context.optionsSeen[0] & FOS_FILEMUSTEXIST) != 0);
    CHECK((context.optionsSeen[2] & FOS_ALLOWMULTISELECT) != 0);
    CHECK((context.optionsSeen[3] & FOS_PICKFOLDERS) != 0);
    CHECK((context.optionsSeen[5] & FOS_PICKFOLDERS) != 0);
    CHECK((context.optionsSeen[5] & FOS_ALLOWMULTISELECT) != 0);
    CHECK((context.optionsSeen[5] & FOS_FORCESHOWHIDDEN) != 0);
    CHECK((context.optionsSeen[7] & FOS_OVERWRITEPROMPT) != 0);
}

TEST_CASE("WinUIDialogContracts::NativePickerNestedAndWindowModal",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-picker-shell-adapter][lifetime]")
{
    ScopedPickerTempDir temp;
    REQUIRE(temp.IsOk());
    const wxString selectedFile = temp.MakeFile("nested.txt");
    const wxString selectedDir =
        temp.MakeDirectory(wxString::FromUTF8("nested-日本語"));
    REQUIRE_FALSE(selectedFile.empty());
    REQUIRE_FALSE(selectedDir.empty());

    wxFrame owner(
        nullptr,
        wxID_ANY,
        "nested picker owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    PickerHookContext context;
    context.owner = &owner;
    context.invokeNestedDir = true;
    context.minimizeOwner = true;
    context.nestedInitialPath = temp.GetPath();

    PickerHookResponse outer;
    outer.rc = wxID_OK;
    outer.path = selectedFile;
    context.responses.push_back(outer);

    PickerHookResponse inner;
    inner.rc = wxID_OK;
    inner.path = selectedDir;
    context.responses.push_back(inner);

    // ShowWindowModal() is synchronous for this native backend, but its public
    // completion event must still be emitted exactly once with the same result.
    PickerHookResponse windowModal;
    windowModal.rc = wxID_CANCEL;
    context.responses.push_back(windowModal);

    ScopedPickerShowHook hook(context);

    wxFileDialog dialog(
        &owner,
        "outer nested picker",
        temp.GetPath(),
        wxString(),
        "Text (*.txt)|*.txt",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    CHECK(dialog.ShowModal() == wxID_OK);
    CHECK(dialog.GetPath() == selectedFile);
    CHECK(context.nestedResult == wxID_OK);
    CHECK(context.calls == 2);
    CHECK(context.maxModalDepth == 2);
    CHECK(context.minimizeCycles == 2);
    REQUIRE(context.optionsSeen.size() >= 2);
    CHECK((context.optionsSeen[0] & FOS_PICKFOLDERS) == 0);
    CHECK((context.optionsSeen[1] & FOS_PICKFOLDERS) != 0);

    context.invokeNestedDir = false;
    int completionEvents = 0;
    wxDirDialog windowModalDialog(
        &owner,
        "window-modal picker",
        temp.GetPath(),
        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    windowModalDialog.Bind(
        wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
        [&completionEvents](wxWindowModalDialogEvent& event)
        {
            ++completionEvents;
            CHECK(event.GetReturnCode() == wxID_CANCEL);
        });
    windowModalDialog.ShowWindowModal();
    CHECK(completionEvents == 1);
    CHECK(windowModalDialog.GetReturnCode() == wxID_CANCEL);
    CHECK(context.calls == 3);
    CHECK(wxModalDialogHook::GetOpenCount() == 0);
}

TEST_CASE("WinUIDialogContracts::NativePickerOwnerDestroyAndCycles",
          "[winui-dialog-contract][winui-standard-dialog]"
          "[winui-picker-shell-adapter][lifetime]")
{
    ScopedPickerTempDir temp;
    REQUIRE(temp.IsOk());
    const wxString selectedFile = temp.MakeFile("cycle.txt");
    REQUIRE_FALSE(selectedFile.empty());

    {
        wxFrame * const owner =
            new wxFrame(
                nullptr,
                wxID_ANY,
                "doomed picker owner",
                wxPoint(-32000, -32000),
                wxSize(420, 260));
        owner->ShowWithoutActivating();
        const wxWeakRef<wxWindow> weakOwner(owner);

        PickerHookContext context;
        context.owner = owner;
        context.destroyOwner = true;
        PickerHookResponse response;
        response.rc = wxID_CANCEL;
        context.responses.push_back(response);
        ScopedPickerShowHook hook(context);

        wxFileDialog dialog(
            owner,
            "destroy owner",
            temp.GetPath(),
            wxString(),
            "All (*.*)|*.*",
            wxFD_OPEN);
        CHECK(dialog.ShowModal() == wxID_CANCEL);
        CHECK(context.calls == 1);
        CHECK_FALSE(weakOwner.get());
        CHECK(
            wxMSWImpl::GetIFileDialogOwnerCloseCountForTesting() == 1);
    }

    {
        wxFrame * const owner =
            new wxFrame(
                nullptr,
                wxID_ANY,
                "doomed directory owner",
                wxPoint(-32000, -32000),
                wxSize(420, 260));
        owner->ShowWithoutActivating();
        const wxWeakRef<wxWindow> weakOwner(owner);

        PickerHookContext context;
        context.owner = owner;
        context.destroyOwner = true;
        PickerHookResponse response;
        response.rc = wxID_CANCEL;
        context.responses.push_back(response);
        ScopedPickerShowHook hook(context);

        wxDirDialog dialog(
            owner,
            "destroy directory owner",
            temp.GetPath(),
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        CHECK(dialog.ShowModal() == wxID_CANCEL);
        CHECK(context.calls == 1);
        CHECK_FALSE(weakOwner.get());
        CHECK(
            wxMSWImpl::GetIFileDialogOwnerCloseCountForTesting() == 1);
    }

    wxFrame owner(
        nullptr,
        wxID_ANY,
        "picker cycle owner",
        wxPoint(-32000, -32000),
        wxSize(420, 260));
    owner.ShowWithoutActivating();

    PickerHookContext context;
    context.owner = &owner;
    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        PickerHookResponse response;
        response.rc = cycle % 3 == 0 ? wxID_CANCEL : wxID_OK;
        if ( cycle % 2 )
            response.path = temp.GetPath();
        else
            response.path = selectedFile;
        context.responses.push_back(response);
    }
    ScopedPickerShowHook hook(context);

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        int rc;
        if ( cycle % 2 )
        {
            wxDirDialog dialog(
                &owner,
                "directory cycle",
                temp.GetPath(),
                wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
            rc = dialog.ShowModal();
            if ( rc == wxID_OK )
                CHECK(dialog.GetPath() == temp.GetPath());
        }
        else
        {
            wxFileDialog dialog(
                &owner,
                "file cycle",
                temp.GetPath(),
                wxString(),
                "Text (*.txt)|*.txt",
                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            rc = dialog.ShowModal();
            if ( rc == wxID_OK )
                CHECK(dialog.GetPath() == selectedFile);
        }

        CHECK(rc == (cycle % 3 == 0 ? wxID_CANCEL : wxID_OK));
        CHECK(wxModalDialogHook::GetOpenCount() == 0);
    }

    CHECK(context.calls == 100);
}
#endif // wxUSE_FILEDLG && wxUSE_DIRDLG && wxUSE_IFILEOPENDIALOG

TEST_CASE("WinUIDialogContracts::PostShowRefitIsBoundedAndLifetimeSafe",
          "[winui-dialog-contract][winui-dialog-refit][lifetime]")
{
    wxFrame owner(
        nullptr,
        wxID_ANY,
        "post-show refit owner",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    owner.ShowWithoutActivating();

    const auto drainDeferredPass = []()
    {
        for ( int attempt = 0; attempt < 20; ++attempt )
        {
            wxYield();
            wxMilliSleep(1);
        }
    };

    SECTION("ShowWithoutActivating and repeated show never shrink")
    {
        wxDialog dialog(
            &owner,
            wxID_ANY,
            "post-show refit",
            wxPoint(-32000, -32000),
            wxSize(80, 60));
        auto * const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(420, 260);
        dialog.SetSizer(sizer);
        dialog.SetClientSize(wxSize(80, 60));

        dialog.ShowWithoutActivating();
        drainDeferredPass();
        CHECK(dialog.GetClientSize().x >= 420);
        CHECK(dialog.GetClientSize().y >= 260);

        dialog.Hide();
        sizer->Clear();
        sizer->Add(40, 30);
        dialog.SetClientSize(wxSize(600, 420));
        dialog.ShowWithoutActivating();
        drainDeferredPass();
        CHECK(dialog.GetClientSize().x >= 600);
        CHECK(dialog.GetClientSize().y >= 420);
        dialog.Hide();
    }

    SECTION("the deferred pass observes the current sizer")
    {
        wxDialog dialog(
            &owner,
            wxID_ANY,
            "sizer replacement",
            wxPoint(-32000, -32000),
            wxSize(80, 60));
        auto * const staleSizer = new wxBoxSizer(wxVERTICAL);
        staleSizer->Add(700, 500);
        dialog.SetSizer(staleSizer);
        dialog.SetClientSize(wxSize(80, 60));
        dialog.ShowWithoutActivating();

        auto * const currentSizer = new wxBoxSizer(wxVERTICAL);
        currentSizer->Add(180, 120);
        dialog.SetSizer(currentSizer, true);
        drainDeferredPass();

        const wxSize current = dialog.GetClientSize();
        CHECK(current.x >= 180);
        CHECK(current.y >= 120);
        CHECK(current.x < 700);
        CHECK(current.y < 500);
        dialog.Hide();
    }

    SECTION("destroy before callback")
    {
        wxWeakRef<wxWindow> destroyed;
        {
            wxDialog dialog(
                &owner,
                wxID_ANY,
                "destroyed refit",
                wxPoint(-32000, -32000),
                wxSize(80, 60));
            auto * const sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(420, 260);
            dialog.SetSizer(sizer);
            dialog.SetClientSize(wxSize(80, 60));
            destroyed = &dialog;
            dialog.ShowWithoutActivating();
        }

        REQUIRE_FALSE(destroyed.get());
        drainDeferredPass();
        CHECK_FALSE(destroyed.get());
    }

    owner.Hide();
}

TEST_CASE("WinUIDialogContracts::DestroySourceCancelsPresenter",
          "[winui-dialog-contract][winui-dialog-lifetime]")
{
    REQUIRE(wxModalDialogHook::GetOpenCount() == 0);
    CountingModalHook hook;
    hook.Register();

    const wxWinUIDialogPresentation previous =
        wxWinUIGetDialogPresentation();
    wxScopeGuard restoreMode =
        wxMakeGuard(
            [previous]()
            {
                wxWinUISetDialogPresentation(previous);
            });
    wxUnusedVar(restoreMode);

    const wxWinUIDialogPresentation modes[] =
    {
        wxWinUIDialogPresentation::Window,
        wxWinUIDialogPresentation::Overlay
    };

    for ( const wxWinUIDialogPresentation mode : modes )
    {
        wxWinUISetDialogPresentation(mode);

        wxFrame * const owner =
            new wxFrame(nullptr, wxID_ANY, "dialog-lifetime-owner",
                        wxPoint(-32000, -32000), wxSize(360, 220));
        owner->ShowWithoutActivating();
        wxYield();

        wxTextEntryDialog * const dialog =
            new wxTextEntryDialog(
                owner, "destroy during nested loop", "lifetime");
        const wxWeakRef<wxWindow> weakDialog(dialog);
        wxDialog * const textIdentity = dialog;
        wxTheApp->CallAfter(
            [weakDialog]()
            {
                if ( wxWindow * const live = weakDialog.get() )
                    live->Destroy();
            });

        INFO("presentation mode " << static_cast<int>(mode));
        const int entersBeforeText = hook.enterCount;
        const int exitsBeforeText = hook.exitCount;
        CHECK(dialog->ShowModal() == wxID_CANCEL);
        CHECK(hook.enterCount == entersBeforeText + 1);
        CHECK(hook.exitCount == exitsBeforeText + 1);
        CHECK(hook.lastEnter == textIdentity);
        CHECK(hook.lastExit == textIdentity);
        CHECK(hook.exitHadLiveDialog);
        CHECK(hook.exitObservedDestroy);
        CHECK(hook.exitTitle == "lifetime");
        CHECK(wxModalDialogHook::GetOpenCount() == 0);

        wxMessageDialog * const message =
            new wxMessageDialog(
                owner,
                "destroy message source during nested loop",
                "message lifetime",
                wxYES_NO);
        REQUIRE(message->GetHandle());
        wxDialog * const messageIdentity = message;
        const wxWeakRef<wxWindow> weakMessage(message);
        wxTheApp->CallAfter(
            [weakMessage]()
            {
                if ( wxWindow * const live = weakMessage.get() )
                    live->Destroy();
            });
        const int entersBeforeMessage = hook.enterCount;
        const int exitsBeforeMessage = hook.exitCount;
        CHECK(message->ShowModal() == wxID_CANCEL);
        CHECK(hook.enterCount == entersBeforeMessage + 1);
        CHECK(hook.exitCount == exitsBeforeMessage + 1);
        CHECK(hook.lastEnter == messageIdentity);
        CHECK(hook.lastExit == messageIdentity);
        CHECK(hook.exitHadLiveDialog);
        CHECK(hook.exitObservedDestroy);
        CHECK(hook.exitTitle == "message lifetime");
        CHECK(wxModalDialogHook::GetOpenCount() == 0);

        owner->Destroy();
        wxYield();

        wxFrame * const cascadeOwner =
            new wxFrame(nullptr, wxID_ANY, "dialog-parent-destroy",
                        wxPoint(-32000, -32000), wxSize(360, 220));
        cascadeOwner->ShowWithoutActivating();

        wxTextEntryDialog * const cascadeText =
            new wxTextEntryDialog(
                cascadeOwner, "parent destroy", "cascade text");
        REQUIRE(CountTopLevelEntries(cascadeText) == 1);
        const int entersBeforeCascadeText = hook.enterCount;
        const int exitsBeforeCascadeText = hook.exitCount;
        wxTheApp->CallAfter(
            [cascadeOwner]()
            {
                cascadeOwner->Destroy();
            });
        CHECK(cascadeText->ShowModal() == wxID_CANCEL);
        CHECK(hook.enterCount == entersBeforeCascadeText + 1);
        CHECK(hook.exitCount == exitsBeforeCascadeText + 1);
        CHECK(hook.exitHadLiveDialog);
        CHECK(hook.exitObservedDestroy);
        CHECK(wxModalDialogHook::GetOpenCount() == 0);
        CHECK(CountTopLevelEntries(cascadeText) == 1);

        wxFrame * const messageOwner =
            new wxFrame(nullptr, wxID_ANY, "message-parent-destroy",
                        wxPoint(-32000, -32000), wxSize(360, 220));
        messageOwner->ShowWithoutActivating();
        wxMessageDialog * const cascadeMessage =
            new wxMessageDialog(
                messageOwner,
                "parent destroy",
                "cascade message",
                wxYES_NO);
        REQUIRE(CountTopLevelEntries(cascadeMessage) == 1);
        const int entersBeforeCascadeMessage = hook.enterCount;
        const int exitsBeforeCascadeMessage = hook.exitCount;
        wxTheApp->CallAfter(
            [messageOwner]()
            {
                messageOwner->Destroy();
            });
        CHECK(cascadeMessage->ShowModal() == wxID_CANCEL);
        CHECK(hook.enterCount == entersBeforeCascadeMessage + 1);
        CHECK(hook.exitCount == exitsBeforeCascadeMessage + 1);
        CHECK(hook.exitHadLiveDialog);
        CHECK(hook.exitObservedDestroy);
        CHECK(wxModalDialogHook::GetOpenCount() == 0);
        CHECK(CountTopLevelEntries(cascadeMessage) == 1);

#if wxUSE_COLOURDLG
        wxFrame * const colourOwner =
            new wxFrame(nullptr, wxID_ANY, "colour-parent-destroy",
                        wxPoint(-32000, -32000), wxSize(360, 220));
        colourOwner->ShowWithoutActivating();
        wxColourDialog * const cascadeColour =
            new wxColourDialog(colourOwner);
        REQUIRE(CountTopLevelEntries(cascadeColour) == 1);
        const int entersBeforeCascadeColour = hook.enterCount;
        const int exitsBeforeCascadeColour = hook.exitCount;
        wxTheApp->CallAfter(
            [colourOwner]()
            {
                colourOwner->Destroy();
            });
        CHECK(cascadeColour->ShowModal() == wxID_CANCEL);
        CHECK(hook.enterCount == entersBeforeCascadeColour + 1);
        CHECK(hook.exitCount == exitsBeforeCascadeColour + 1);
        CHECK(hook.exitHadLiveDialog);
        CHECK(hook.exitObservedDestroy);
        CHECK(wxModalDialogHook::GetOpenCount() == 0);
        CHECK(CountTopLevelEntries(cascadeColour) == 1);
#endif

        wxYield();
    }
}

TEST_CASE("WinUIDialogContracts::ForceUpperWhileOpen",
          "[winui-dialog-contract][winui-dialog-lifetime]")
{
    const wxWinUIDialogPresentation previous =
        wxWinUIGetDialogPresentation();
    wxScopeGuard restoreMode =
        wxMakeGuard(
            [previous]()
            {
                wxWinUISetDialogPresentation(previous);
            });
    wxUnusedVar(restoreMode);

    const wxWinUIDialogPresentation modes[] =
    {
        wxWinUIDialogPresentation::Window,
        wxWinUIDialogPresentation::Overlay
    };

    for ( const wxWinUIDialogPresentation mode : modes )
    {
        wxWinUISetDialogPresentation(mode);
        wxFrame * const owner =
            new wxFrame(nullptr, wxID_ANY, "force-upper-owner",
                        wxPoint(-32000, -32000), wxSize(360, 220));
        owner->ShowWithoutActivating();

        wxTextEntryDialog * const text =
            new wxTextEntryDialog(
                owner, "text", "dynamic upper", "Initial");
        const wxWeakRef<wxTextEntryDialog> weakText(text);
        bool textCallbackRan = false;
        bool textSetSucceeded = false;
        wxString textObserved;
        wxTheApp->CallAfter(
            [weakText, &textCallbackRan, &textSetSucceeded,
             &textObserved]()
            {
                if ( wxTextEntryDialog * const live = weakText.get() )
                {
                    textCallbackRan = true;
                    textSetSucceeded =
                        live->WinUISetPeerValueForTesting("mixed Text");
                    live->ForceUpper();
                    textObserved =
                        live->WinUIGetPeerValueForTesting();
                    live->Destroy();
                }
            });
        CHECK(text->ShowModal() == wxID_CANCEL);
        CHECK(textCallbackRan);
        CHECK(textSetSucceeded);
        CHECK(textObserved == "MIXED TEXT");

        wxPasswordEntryDialog * const password =
            new wxPasswordEntryDialog(
                owner, "password", "dynamic password", "Initial");
        const wxWeakRef<wxPasswordEntryDialog> weakPassword(password);
        bool passwordCallbackRan = false;
        bool passwordSetSucceeded = false;
        wxString passwordObserved;
        wxTheApp->CallAfter(
            [weakPassword, &passwordCallbackRan,
             &passwordSetSucceeded, &passwordObserved]()
            {
                if ( wxPasswordEntryDialog * const live =
                         weakPassword.get() )
                {
                    passwordCallbackRan = true;
                    passwordSetSucceeded =
                        live->WinUISetPeerValueForTesting("mixed Secret");
                    live->ForceUpper();
                    passwordObserved =
                        live->WinUIGetPeerValueForTesting();
                    live->Destroy();
                }
            });
        CHECK(password->ShowModal() == wxID_CANCEL);
        CHECK(passwordCallbackRan);
        CHECK(passwordSetSucceeded);
        CHECK(passwordObserved == "MIXED SECRET");

        owner->Destroy();
        wxYield();
    }
}

#if wxUSE_RICHMSGDLG

TEST_CASE("WinUIDialogContracts::RichMessageNativeBoundary",
          "[winui-beta-dialogs][winui-dialog-aux][winui-rich-message]")
{
    REQUIRE(wxModalDialogHook::GetOpenCount() == 0);
    CountingModalHook modalHook;
    modalHook.Register();

    wxFrame owner(
        nullptr, wxID_ANY, "rich-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    TaskDialogHookContext context;
    context.richButton = IDNO;
    context.richVerification = TRUE;
    ScopedTaskDialogHook hook(context);

    wxRichMessageDialog dialog(
        &owner,
        "Primary message",
        "Rich contract",
        wxYES_NO | wxCANCEL | wxHELP | wxNO_DEFAULT | wxICON_WARNING);
    dialog.ShowCheckBox("Remember this choice", false);
    dialog.ShowDetailedText("Deterministic expanded details");
    dialog.SetFooterText("Footer contract");
    dialog.SetFooterIcon(wxICON_INFORMATION);

    const int entersBefore = modalHook.enterCount;
    const int exitsBefore = modalHook.exitCount;
    CHECK(dialog.ShowModal() == wxID_NO);
    CHECK(modalHook.enterCount == entersBefore + 1);
    CHECK(modalHook.exitCount == exitsBefore + 1);
    CHECK(wxModalDialogHook::GetOpenCount() == 0);

    CHECK(dialog.IsCheckBoxChecked());
    CHECK(context.parent == static_cast<HWND>(wxGetHWND(&owner)));
    CHECK(context.title == "Rich contract");
    CHECK(context.content == "Primary message");
    CHECK(context.expandedInformation ==
          "Deterministic expanded details");
    CHECK(context.verificationText == "Remember this choice");
    CHECK(context.footer == "Footer contract");
    CHECK(context.defaultButton == IDNO);
    CHECK((context.commonButtons & TDCBF_YES_BUTTON) != 0);
    CHECK((context.commonButtons & TDCBF_NO_BUTTON) != 0);
    CHECK((context.commonButtons & TDCBF_CANCEL_BUTTON) != 0);
    CHECK(context.mainIcon != nullptr);
    CHECK(context.footerIcon != nullptr);

    bool hasHelp = false;
    for ( const int id : context.customButtonIds )
    {
        if ( id == IDHELP )
            hasHelp = true;
    }
    CHECK(hasHelp);

    struct RichMatrixCase
    {
        long style;
        int nativeResult;
        int publicResult;
        int defaultButton;
        PCWSTR icon;
    };

    const RichMatrixCase matrix[] =
    {
        { static_cast<long>(wxOK | wxICON_ERROR),
          IDCANCEL, wxID_OK, 0, TD_ERROR_ICON },
        { static_cast<long>(
              wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_INFORMATION),
          IDCANCEL, wxID_CANCEL, IDCANCEL, TD_INFORMATION_ICON },
        { static_cast<long>(wxYES_NO | wxICON_WARNING),
          IDYES, wxID_YES, 0, TD_WARNING_ICON },
        { static_cast<long>(
              wxYES_NO | wxNO_DEFAULT | wxICON_AUTH_NEEDED),
          IDNO, wxID_NO, IDNO, TD_SHIELD_ICON }
    };

    // Includes the OK-only Escape translation: the native adapter uses a
    // cancellable button internally but keeps wxID_OK as the public result.
    context.richVerification = FALSE;
    for ( const RichMatrixCase& entry : matrix )
    {
        context.richButton = entry.nativeResult;
        wxRichMessageDialog item(
            &owner, "matrix", "rich matrix", entry.style);
        CHECK(item.ShowModal() == entry.publicResult);
        CHECK(context.defaultButton == entry.defaultButton);
        CHECK(reinterpret_cast<ULONG_PTR>(context.mainIcon) ==
              reinterpret_cast<ULONG_PTR>(entry.icon));
    }

    // The seam stays at the final native boundary, so a hundred complete
    // configuration/result cycles exercise the real adapter without showing
    // anything on the desktop.
    context.richButton = IDOK;
    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxRichMessageDialog repeated(
            &owner,
            wxString::Format("cycle %d", cycle),
            "rich-cycle",
            wxOK | (cycle % 2 ? wxICON_INFORMATION : wxICON_WARNING));
        CHECK(repeated.ShowModal() == wxID_OK);
    }
}

TEST_CASE("WinUIDialogContracts::RichMessageFallbackAndOwnerLifetime",
          "[winui-beta-dialogs][winui-dialog-aux][winui-rich-message]"
          "[winui-dialog-lifetime]")
{
    SECTION("native failure preserves one modal operation and rich data")
    {
        REQUIRE(wxModalDialogHook::GetOpenCount() == 0);
        CountingModalHook modalHook;
        modalHook.Register();

        wxFrame owner(
            nullptr, wxID_ANY, "rich-fallback-owner",
            wxPoint(-32000, -32000), wxSize(360, 220));
        owner.ShowWithoutActivating();

        TaskDialogHookContext context;
        context.richResult = E_FAIL;
        ScopedTaskDialogHook hook(context);

        wxRichMessageDialog dialog(
            &owner,
            "Fallback primary",
            "Fallback rich",
            wxYES_NO | wxCANCEL | wxICON_INFORMATION);
        dialog.ShowCheckBox("Fallback checkbox");
        dialog.ShowDetailedText("Fallback details");
        dialog.SetFooterText("Fallback footer");

        bool queued = false;
        bool sawCheckBox = false;
        bool sawDetails = false;
        bool sawFooter = false;
        const wxWeakRef<wxWindow> weakDialog(&dialog);
        wxTheApp->CallAfter(
            [weakDialog, &queued, &sawCheckBox,
             &sawDetails, &sawFooter]()
            {
                wxWindow * const live = weakDialog.get();
                if ( !live )
                    return;

                std::vector<wxCheckBox *> checkBoxes;
                CollectChildControls(live, checkBoxes);
                if ( !checkBoxes.empty() )
                {
                    checkBoxes[0]->SetValue(true);
                    sawCheckBox = true;
                }

                std::vector<wxCollapsiblePane *> panes;
                CollectChildControls(live, panes);
                if ( !panes.empty() )
                {
                    panes[0]->Expand();
                    sawDetails = panes[0]->IsExpanded();
                }

                std::vector<wxStaticText *> labels;
                CollectChildControls(live, labels);
                for ( wxStaticText * const label : labels )
                {
                    if ( label->GetLabel() == "Fallback footer" )
                        sawFooter = true;
                }

                wxButton * const yes =
                    wxDynamicCast(live->FindWindow(wxID_YES), wxButton);
                if ( yes )
                    queued = yes->WinUIQueueClickForTesting();
            });

        const int entersBefore = modalHook.enterCount;
        const int exitsBefore = modalHook.exitCount;
        CHECK(dialog.ShowModal() == wxID_YES);
        CHECK(queued);
        CHECK(sawCheckBox);
        CHECK(sawDetails);
        CHECK(sawFooter);
        CHECK(dialog.IsCheckBoxChecked());
        CHECK(modalHook.enterCount == entersBefore + 1);
        CHECK(modalHook.exitCount == exitsBefore + 1);
        CHECK(wxModalDialogHook::GetOpenCount() == 0);
    }

    SECTION("owner can disappear while native boundary is active")
    {
        wxFrame * const owner =
            new wxFrame(
                nullptr, wxID_ANY, "rich-destroy-owner",
                wxPoint(-32000, -32000), wxSize(360, 220));
        owner->ShowWithoutActivating();
        const wxWeakRef<wxWindow> weakOwner(owner);

        TaskDialogHookContext context;
        context.richButton = IDCANCEL;
        context.destroyRichOwner = true;
        context.richOwner = owner;
        ScopedTaskDialogHook hook(context);

        wxRichMessageDialog dialog(
            owner, "Owner destruction", "Rich lifetime", wxOK);
        CHECK(dialog.ShowModal() == wxID_OK);
        CHECK_FALSE(weakOwner.get());
        CHECK(dialog.GetParent() == nullptr);
    }
}

#endif // wxUSE_RICHMSGDLG

#if wxUSE_PROGRESSDLG

TEST_CASE("WinUIDialogContracts::GenericProgressContractAndReentrance",
          "[winui-beta-dialogs][winui-dialog-aux][winui-progress-dialog]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "generic-progress-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    {
        wxGenericProgressDialog progress(
            "Generic progress",
            "Starting",
            100000,
            &owner,
            wxPD_CAN_ABORT | wxPD_CAN_SKIP |
            wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
            wxPD_REMAINING_TIME | wxPD_AUTO_HIDE);

        CHECK_FALSE(owner.IsEnabled());
        CHECK(progress.GetRange() == 100000);
        CHECK(progress.GetValue() == 0);
        CHECK(progress.Update(70000, "Determinate 70%"));
        CHECK(progress.GetValue() == 70000);
        CHECK(progress.GetMessage() == "Determinate 70%");
        CHECK(progress.Pulse("Indeterminate pulse"));
        CHECK(progress.GetValue() == 70000);
        CHECK(progress.GetMessage() == "Indeterminate pulse");

        std::vector<wxStaticText *> timeLabels;
        CollectChildControls(&progress, timeLabels);
        CHECK(timeLabels.size() >= 7);

        const wxSize beforeLongMessage = progress.GetClientSize();
        const wxString longMessage =
            "A long progress message used to prove the grow-only layout "
            "contract after the WinUI templates have been realized.";
        CHECK(progress.Update(70500, longMessage));
        const wxSize afterLongMessage = progress.GetClientSize();
        CHECK(afterLongMessage.x >= beforeLongMessage.x);
        CHECK(afterLongMessage.y >= beforeLongMessage.y);
        CHECK(progress.Update(70600, "short"));
        CHECK(progress.GetClientSize().x >= afterLongMessage.x);
        CHECK(progress.GetClientSize().y >= afterLongMessage.y);

        std::vector<wxButton *> buttons;
        CollectChildControls(&progress, buttons);
        wxButton *skipButton = nullptr;
        for ( wxButton * const button : buttons )
        {
            if ( button->GetId() != wxID_CANCEL )
            {
                skipButton = button;
                break;
            }
        }
        REQUIRE(skipButton);

        const unsigned skipAttempts =
            wxButton::WinUIGetPeerInvokeAttemptCountForTesting();
        REQUIRE(skipButton->WinUIQueueClickForTesting());
        WaitFor(
            "generic progress skip automation",
            [skipAttempts]()
            {
                return
                    wxButton::WinUIGetPeerInvokeAttemptCountForTesting() >
                    skipAttempts;
            });

        bool skipped = false;
        CHECK(progress.Update(71000, wxString(), &skipped));
        CHECK(skipped);

        InvokeButtonPeer(progress, wxID_CANCEL);
        CHECK_FALSE(progress.Update(72000));
        CHECK(progress.WasCancelled());
        progress.Resume();
        CHECK_FALSE(progress.WasCancelled());
        CHECK(progress.Update(73000, "Resumed"));

        progress.Hide();
        CHECK(owner.IsEnabled());
        progress.Show();
        CHECK_FALSE(owner.IsEnabled());
    }
    CHECK(owner.IsEnabled());

    SECTION("nested owner disabling is restored exactly once")
    {
        wxGenericProgressDialog outer(
            "outer", "outer", 10, &owner, wxPD_AUTO_HIDE);
        CHECK_FALSE(owner.IsEnabled());
        {
            wxGenericProgressDialog inner(
                "inner", "inner", 10, &owner, wxPD_AUTO_HIDE);
            CHECK_FALSE(owner.IsEnabled());
        }
        CHECK_FALSE(owner.IsEnabled());
    }
    CHECK(owner.IsEnabled());

    SECTION("default construction plus Create uses the same contract")
    {
        wxGenericProgressDialog progress;
        CHECK(progress.GetRange() == 0);
        REQUIRE(progress.Create(
            "two-stage", "created", 20,
            &owner, wxPD_CAN_ABORT | wxPD_AUTO_HIDE));
        CHECK(progress.GetRange() == 20);
        CHECK(progress.Update(20, "complete"));
        CHECK(progress.GetValue() == 20);
    }
    CHECK(owner.IsEnabled());

    SECTION("a pre-disabled owner stays disabled")
    {
        owner.Disable();
        {
            wxGenericProgressDialog progress(
                "pre-disabled", "pre-disabled", 10,
                &owner, wxPD_AUTO_HIDE);
            CHECK_FALSE(owner.IsEnabled());
        }
        CHECK_FALSE(owner.IsEnabled());
        owner.Enable();
    }

    SECTION("nested update supersedes the outer write")
    {
        wxGenericProgressDialog progress(
            "reentrant", "initial", 100,
            &owner, wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
        wxWeakRef<wxGenericProgressDialog> weakProgress(&progress);
        progress.WinUISetYieldHookForTesting(
            &weakProgress,
            [](void *rawProgress)
            {
                wxWeakRef<wxGenericProgressDialog>& weakProgress =
                    *static_cast<wxWeakRef<wxGenericProgressDialog> *>(
                        rawProgress);
                if ( wxGenericProgressDialog * const live =
                         weakProgress.get() )
                {
                    (void)live->Update(60, "nested update");
                }
            });

        CHECK(progress.Update(10, "stale outer update"));
        CHECK(progress.GetValue() == 60);
        CHECK(progress.GetMessage() == "nested update");
    }

    SECTION("uncancelable close is vetoed")
    {
        wxGenericProgressDialog progress(
            "veto", "cannot close", 10,
            &owner, wxPD_AUTO_HIDE);
        (void)progress.Close();
        CHECK(progress.IsShown());
        CHECK_FALSE(progress.WasCancelled());
    }

    SECTION("non-auto-hide completion exits through the real close button")
    {
        wxGenericProgressDialog progress(
            "modal finish", "finishing", 10,
            &owner, wxPD_CAN_ABORT);
        bool queued = false;
        const wxWeakRef<wxGenericProgressDialog> weakProgress(&progress);
        wxTimer closeTimer;
        closeTimer.Bind(
            wxEVT_TIMER,
            [weakProgress, &queued, &closeTimer](wxTimerEvent&)
            {
                wxGenericProgressDialog * const live =
                    weakProgress.get();
                if ( !live || live->GetValue() != 10 )
                    return;

                wxButton * const close =
                    wxDynamicCast(
                        live->FindWindow(wxID_CANCEL), wxButton);
                if ( close )
                {
                    queued = close->WinUIQueueClickForTesting();
                    if ( queued )
                        closeTimer.Stop();
                }
            });
        closeTimer.Start(1);

        CHECK(progress.Update(10, "Done"));
        closeTimer.Stop();
        CHECK(queued);
        CHECK(progress.GetValue() == 10);
    }

    SECTION("destruction during the inherited bounded yield is safe")
    {
        wxGenericProgressDialog * const progress =
            new wxGenericProgressDialog(
                "destroy in update", "initial", 10,
                &owner, wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
        wxWeakRef<wxGenericProgressDialog> weakProgress(progress);
        progress->WinUISetYieldHookForTesting(
            &weakProgress,
            [](void *rawProgress)
            {
                wxWeakRef<wxGenericProgressDialog>& weakProgress =
                    *static_cast<wxWeakRef<wxGenericProgressDialog> *>(
                        rawProgress);
                if ( wxGenericProgressDialog * const live =
                         weakProgress.get() )
                {
                    delete live;
                }
            });

        CHECK_FALSE(progress->Update(1, "must not write after destroy"));
        CHECK_FALSE(weakProgress.get());
        CHECK(owner.IsEnabled());
    }

    SECTION("destroying the owner during update cancels the child safely")
    {
        wxFrame * const transientOwner =
            new wxFrame(
                nullptr, wxID_ANY, "generic-progress-destroy-owner",
                wxPoint(-32000, -32000), wxSize(360, 220));
        transientOwner->ShowWithoutActivating();
        wxGenericProgressDialog * const progress =
            new wxGenericProgressDialog(
                "owner destroy", "initial", 10,
                transientOwner,
                wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
        wxWeakRef<wxWindow> weakOwner(transientOwner);
        const wxWeakRef<wxGenericProgressDialog> weakProgress(progress);
        progress->WinUISetYieldHookForTesting(
            &weakOwner,
            [](void *rawOwner)
            {
                wxWeakRef<wxWindow>& weakOwner =
                    *static_cast<wxWeakRef<wxWindow> *>(rawOwner);
                if ( wxWindow * const live = weakOwner.get() )
                    delete live;
            });

        CHECK_FALSE(progress->Pulse("must not touch destroyed children"));
        CHECK_FALSE(weakOwner.get());
        CHECK_FALSE(weakProgress.get());
    }
}

TEST_CASE("WinUIDialogContracts::NativeProgressBoundaryAndLifetime",
          "[winui-beta-dialogs][winui-dialog-aux][winui-progress-dialog]"
          "[winui-dialog-lifetime]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "native-progress-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    TaskDialogHookContext context;
    ScopedTaskDialogHook hook(context);

    {
        wxProgressDialog progress(
            "Native progress",
            "Starting",
            100000,
            &owner,
            wxPD_CAN_ABORT | wxPD_CAN_SKIP |
            wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
            wxPD_REMAINING_TIME | wxPD_AUTO_HIDE);

        REQUIRE(context.progressEntered.load());
        CHECK_FALSE(owner.IsEnabled());
        CHECK((context.flags & TDF_SHOW_PROGRESS_BAR) != 0);
        CHECK((context.flags & TDF_CALLBACK_TIMER) != 0);
        CHECK(context.parent == static_cast<HWND>(wxGetHWND(&owner)));
        CHECK(progress.GetRange() == 100000);
        CHECK(progress.GetValue() == 0);
        CHECK_FALSE(context.expandedInformation.empty());

        for ( int update = 1; update <= 50; ++update )
        {
            CHECK(progress.Update(
                update * 1400,
                wxString::Format("Native update %d", update)));
        }
        CHECK(progress.GetValue() == 70000);
        CHECK(progress.GetMessage() == "Native update 50");
        CHECK(progress.Pulse("Native pulse"));
        CHECK(progress.GetValue() == 70000);
        CHECK(progress.GetMessage() == "Native pulse");

        context.requestSkip = true;
        WaitFor(
            "native progress skip callback",
            [&context]()
            {
                return context.skipDeliveries.load() == 1;
            });
        bool skipped = false;
        CHECK(progress.Update(71000, wxString(), &skipped));
        CHECK(skipped);

        context.requestCancel = true;
        WaitFor(
            "native progress cancel callback",
            [&context]()
            {
                return context.cancelDeliveries.load() == 1;
            });
        CHECK_FALSE(progress.Update(72000));
        CHECK(progress.WasCancelled());
        progress.Resume();
        CHECK_FALSE(progress.WasCancelled());

        progress.SetRange(200000);
        CHECK(progress.GetRange() == 200000);
        CHECK(progress.Update(150000, "Resumed native"));
        CHECK(progress.GetValue() == 150000);
        progress.SetTitle("Updated native title");
        CHECK(progress.GetTitle() == "Updated native title");
        progress.Fit();
        CHECK(progress.Update(200000, "Native done"));
        CHECK(progress.GetValue() == 200000);
        WaitFor(
            "native progress auto-hide teardown",
            [&context]()
            {
                return context.progressExited.load();
            });
    }
    CHECK(owner.IsEnabled());

    SECTION("minimized owner is not used for relative positioning")
    {
        owner.Iconize(true);
        wxYield();
        context.progressEntered = false;
        context.progressExited = false;
        {
            wxProgressDialog progress(
                "minimized owner", "progress", 10,
                &owner, wxPD_AUTO_HIDE);
            REQUIRE(context.progressEntered.load());
            CHECK((context.flags & TDF_POSITION_RELATIVE_TO_WINDOW) == 0);
            CHECK(progress.Update(10));
        }
        owner.Iconize(false);
        wxYield();
    }

    SECTION("destroying the owner never exposes a wxWindow to the worker")
    {
        wxFrame * const transientOwner =
            new wxFrame(
                nullptr, wxID_ANY, "native-progress-destroy-owner",
                wxPoint(-32000, -32000), wxSize(360, 220));
        transientOwner->ShowWithoutActivating();
        const wxWeakRef<wxWindow> weakOwner(transientOwner);

        context.progressEntered = false;
        context.progressExited = false;
        wxProgressDialog * const progress =
            new wxProgressDialog(
                "owner lifetime", "progress", 10,
                transientOwner,
                wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
        REQUIRE(context.progressEntered.load());
        transientOwner->Destroy();
        wxYield();
        CHECK_FALSE(weakOwner.get());
        CHECK(progress->Update(10));
        delete progress;
        CHECK(context.progressExited.load());
    }

    SECTION("destroying the dialog from its update dispatch is safe")
    {
        context.progressEntered = false;
        context.progressExited = false;
        wxProgressDialog * const progress =
            new wxProgressDialog(
                "callback destroy", "progress", 10,
                 &owner,
                 wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
        REQUIRE(context.progressEntered.load());
        const wxWeakRef<wxProgressDialog> weakProgress(progress);
        struct DeleteDuringYieldContext
        {
            explicit DeleteDuringYieldContext(wxProgressDialog *dialog)
                : progress(dialog),
                  deleted(false)
            {
            }

            wxWeakRef<wxProgressDialog> progress;
            bool deleted;
        } deleteContext(progress);
        progress->WinUISetYieldHookForTesting(
            &deleteContext,
            [](void *rawContext)
            {
                DeleteDuringYieldContext& context =
                    *static_cast<DeleteDuringYieldContext *>(rawContext);
                if ( wxProgressDialog * const live = context.progress.get() )
                {
                    delete live;
                    context.deleted = true;
                }
            });

        const bool updateResult = progress->Update(1);
        wxProgressDialog * const remainingProgress = weakProgress.get();
        const bool exitedFromDispatch = context.progressExited.load();
        if ( remainingProgress )
            delete remainingProgress;

        CHECK_FALSE(updateResult);
        CHECK(deleteContext.deleted);
        CHECK_FALSE(remainingProgress);
        CHECK(exitedFromDispatch);
    }
}

TEST_CASE("WinUIDialogContracts::ProgressHundredCycleShutdown",
          "[winui-beta-dialogs][winui-dialog-aux][winui-progress-dialog]"
          "[winui-dialog-stress]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "progress-cycle-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    TaskDialogHookContext context;
    ScopedTaskDialogHook hook(context);

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        context.progressEntered = false;
        context.progressExited = false;
        context.timerDeliveries = 0;

        wxProgressDialog progress(
            "native cycle",
            wxString::Format("cycle %d", cycle),
            1000,
            &owner,
            wxPD_AUTO_HIDE | (cycle % 2 ? wxPD_CAN_ABORT : 0));
        REQUIRE(context.progressEntered.load());
        CHECK(progress.Update(500, "half"));
        CHECK(progress.GetValue() == 500);
        CHECK(progress.Update(1000, "done"));
        WaitFor(
            "native progress cycle exit",
            [&context]()
            {
                return context.progressExited.load();
            });
    }

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxGenericProgressDialog progress(
            "generic cycle",
            wxString::Format("cycle %d", cycle),
            10,
            &owner,
            wxPD_AUTO_HIDE);
        CHECK(progress.Update(10));
    }

    CHECK(owner.IsEnabled());
}

#endif // wxUSE_PROGRESSDLG

#if wxUSE_BUSYINFO

TEST_CASE("WinUIDialogContracts::BusyInfoRAIIAndLifetime",
          "[winui-beta-dialogs][winui-dialog-aux][winui-busy-info]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "busy-owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    const std::vector<wxWindow *> baseline = SnapshotTopLevelWindows();
    wxWeakRef<wxWindow> weakBusyFrame;
    wxSize grownSize;
    {
        wxBusyInfo busy(
            wxBusyInfoFlags()
                .Parent(&owner)
                .Title("<b>Busy contract</b>")
                .Label("Short"));

        wxWindow * const busyFrame = FindAddedTopLevel(baseline);
        REQUIRE(busyFrame);
        weakBusyFrame = busyFrame;
        CHECK(busyFrame->IsShown());

        const wxSize initialSize = busyFrame->GetClientSize();
        const wxString longLabel =
            "A deliberately long localized-style busy label that must grow "
            "the realized transient instead of being cropped by its first "
            "template measurement.";
        busy.UpdateLabel(longLabel);
        grownSize = busyFrame->GetClientSize();
        CHECK(grownSize.x >= initialSize.x);
        CHECK(grownSize.y >= initialSize.y);

        std::vector<wxControl *> labels;
        CollectChildControls(busyFrame, labels);
        bool foundLongLabel = false;
        for ( wxControl * const label : labels )
        {
            if ( label->GetLabel() == longLabel )
                foundLongLabel = true;
        }
        CHECK(foundLongLabel);

        busy.UpdateLabel("Short again");
        CHECK(busyFrame->GetClientSize().x >= grownSize.x);
        CHECK(busyFrame->GetClientSize().y >= grownSize.y);

        const std::vector<wxWindow *> oneBusy = SnapshotTopLevelWindows();
        {
            wxBusyInfo nested("Nested busy", &owner);
            wxWindow * const nestedFrame = FindAddedTopLevel(oneBusy);
            REQUIRE(nestedFrame);
            CHECK(nestedFrame != busyFrame);
            CHECK(nestedFrame->IsShown());
        }
        wxYield();
        CHECK(weakBusyFrame.get() == busyFrame);
    }
    wxYield();
    CHECK_FALSE(weakBusyFrame.get());

    SECTION("busy cursor nesting restores the previous global state")
    {
        CHECK_FALSE(wxIsBusy());
        {
            wxBusyCursor outer;
            CHECK(wxIsBusy());
            {
                wxBusyCursor inner;
                CHECK(wxIsBusy());
                wxBusyInfo busy("Cursor nesting", &owner);
                CHECK(wxIsBusy());
            }
            CHECK(wxIsBusy());
        }
        CHECK_FALSE(wxIsBusy());
    }

    SECTION("owner may be destroyed before the RAII guard")
    {
        wxFrame * const transientOwner =
            new wxFrame(
                nullptr, wxID_ANY, "busy-destroy-owner",
                wxPoint(-32000, -32000), wxSize(360, 220));
        transientOwner->ShowWithoutActivating();
        const std::vector<wxWindow *> before = SnapshotTopLevelWindows();
        wxBusyInfo busy("Owner destruction", transientOwner);
        wxWindow * const busyFrame = FindAddedTopLevel(before);
        REQUIRE(busyFrame);
        const wxWeakRef<wxWindow> weakFrame(busyFrame);

        transientOwner->Destroy();
        wxYield();
        CHECK_FALSE(weakFrame.get());

        // Late calls are intentional no-ops, not stale-control accesses.
        busy.UpdateLabel("late label");
        busy.UpdateText("<b>late markup</b>");
    }

    SECTION("RAII object itself may be deleted from a callback")
    {
        const std::vector<wxWindow *> before = SnapshotTopLevelWindows();
        wxBusyInfo * const busy =
            new wxBusyInfo("callback destruction", &owner);
        wxWindow * const busyFrame = FindAddedTopLevel(before);
        REQUIRE(busyFrame);
        const wxWeakRef<wxWindow> weakFrame(busyFrame);
        wxTheApp->CallAfter([busy]() { delete busy; });
        WaitFor(
            "busy info callback destruction",
            [weakFrame]()
            {
                return !weakFrame.get();
            });
        CHECK_FALSE(weakFrame.get());
    }
}

TEST_CASE("WinUIDialogContracts::BusyInfoHundredCycles",
          "[winui-beta-dialogs][winui-dialog-aux][winui-busy-info]"
          "[winui-dialog-stress]")
{
    const std::vector<wxWindow *> baseline = SnapshotTopLevelWindows();

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxBusyInfo busy(
            wxString::Format("busy cycle %d — Unicode ✓", cycle));
        busy.UpdateLabel(
            wxString::Format("updated busy cycle %d", cycle));
    }

    WaitFor(
        "busy info hundred-cycle teardown",
        [&baseline]()
        {
            return SnapshotTopLevelWindows().size() == baseline.size();
        });
    CHECK(SnapshotTopLevelWindows().size() == baseline.size());
}

#endif // wxUSE_BUSYINFO

#endif // __WXWINUI__ && wxUSE_WINUI3
