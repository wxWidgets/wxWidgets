///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/taskbarbutton.h
// Purpose:     Testable native boundary for MSW taskbar-button integration
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_TASKBARBUTTON_H_
#define _WX_MSW_PRIVATE_TASKBARBUTTON_H_

#include "wx/defs.h"

#if wxUSE_TASKBARBUTTON

#include "wx/gdicmn.h"
#include "wx/string.h"
#include "wx/taskbarbutton.h"

#include <cstddef>
#include <memory>

// This interface is deliberately expressed in wx/native-neutral types. The
// production implementation is the sole owner of ITaskbarList3; tests replace
// it with an in-process recorder and never depend on a running Explorer shell.
struct wxMSWTaskBarThumbButtonNative
{
    bool occupied { false };
    unsigned token { 0 };
    // Keep the HICON owner alive across the native call. The raw value below
    // is only a convenience for native-neutral recorder seams.
    wxIcon retainedIcon;
    void* icon { nullptr };
    wxString tooltip;
    unsigned flags { 0 };
};

class wxMSWTaskBarButtonNativeBackend
{
public:
    virtual ~wxMSWTaskBarButtonNativeBackend() = default;

    virtual long Initialize() = 0;
    virtual long AddTab(WXHWND hwnd) = 0;
    virtual long DeleteTab(WXHWND hwnd) = 0;
    virtual long SetProgressValue(WXHWND hwnd,
                                  unsigned long long value,
                                  unsigned long long range) = 0;
    virtual long SetProgressState(WXHWND hwnd, unsigned state) = 0;
    virtual long SetOverlayIcon(WXHWND hwnd,
                                void* icon,
                                const wxString& description) = 0;
    virtual long SetThumbnailTooltip(WXHWND hwnd,
                                     const wxString& tooltip) = 0;
    virtual long SetThumbnailClip(WXHWND hwnd,
                                  const wxRect* rect) = 0;
    virtual long ThumbBarAddButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) = 0;
    virtual long ThumbBarUpdateButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) = 0;
};

using wxMSWTaskBarButtonNativeFactoryForTesting =
    std::shared_ptr<wxMSWTaskBarButtonNativeBackend> (*)();

// GUI-thread-only replacement seam. Passing nullptr restores the production
// COM factory. Existing taskbar-button objects retain their exact backend.
WXDLLIMPEXP_CORE void wxMSWTaskBarButtonSetNativeFactoryForTesting(
    wxMSWTaskBarButtonNativeFactoryForTesting factory);

struct wxMSWTaskBarButtonSnapshot
{
    bool available { false };
    bool retired { false };
    WXHWND hwnd { nullptr };
    unsigned long long hwndGeneration { 0 };
    unsigned long long shellEpoch { 0 };

    int desiredProgressRange { 0 };
    int desiredProgressValue { 0 };
    bool desiredProgressValueKnown { false };
    wxTaskBarButtonState desiredProgressState {
        wxTASKBAR_BUTTON_NO_PROGRESS
    };

    bool progressValueAppliedKnown { false };
    int appliedProgressRange { 0 };
    int appliedProgressValue { 0 };
    bool progressStateAppliedKnown { false };
    wxTaskBarButtonState appliedProgressState {
        wxTASKBAR_BUTTON_NO_PROGRESS
    };

    bool thumbButtonsAppliedKnown { false };
    bool thumbButtonsInitialized { false };
    std::size_t desiredThumbButtonCount { 0 };
    std::size_t appliedThumbButtonCount { 0 };
};

WXDLLIMPEXP_CORE bool wxMSWTaskBarButtonGetSnapshotForTesting(
    wxTaskBarButton* button,
    wxMSWTaskBarButtonSnapshot* snapshot);

// Replay every wxAppProgressIndicator sidecar targeting this exact TLW after
// Explorer recreates its taskbar button. Unlike wxFrame::MSWGetTaskBarButton(),
// app-progress controllers are deliberately not stored in wxFrame.
WXDLLIMPEXP_CORE void wxMSWAppProgressNotifyTaskbarCreated(wxWindow* window);

#endif // wxUSE_TASKBARBUTTON

#endif // _WX_MSW_PRIVATE_TASKBARBUTTON_H_
