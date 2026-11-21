/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribbontoolkitruntime.h
// Purpose:     Private module-local activation of native Toolkit Ribbon peers
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_TOOLKIT_RUNTIME_H_
#define _WX_RIBBON_TOOLKIT_RUNTIME_H_

#include "wx/defs.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

// Use the same Windows-macro cleanup and base projections as the adapters
// consuming this context, before including the component's generated headers.
#include "../winui/private.h"
#include <winrt/XamlToolkit.Labs.WinUI.h>
#include <memory>
#include <utility>

struct wxRibbonToolkitRuntimeState;

struct wxRibbonToolkitFailure
{
    winrt::hresult code{S_OK};
    const char* stage = "none";
    unsigned loadedModules = 0;
    unsigned pinnedModules = 0;
    wxString detail;
};

// Source-private, UI-thread-only adapter context. No COM registration, global
// activation hook, or global strong COM reference is installed. Both providers
// are published atomically into the existing wxWinUI Application registry.
//
// The eight dependency DLLs are OS-pinned after path/export validation and
// before their first factory call. This is IRREVERSIBLE until process exit:
// metadata, controls and internal factory caches may outlive their providers.
// Hot unloading/replacing this payload within the process is not supported.
// Providers and adapter contexts additionally retain the module leases; COM
// objects retire with their Application/adapters, never a static destructor.
//
// An empty directory means the executable's directory, NOT the current working
// directory. An explicit directory must be absolute. All eight files must be
// in that one directory; no module-directory, PATH or CWD fallback is tried.
// Build/deployment own the pinned archive/PRI checks. This loader verifies
// exact file identity and exports, not archive provenance at runtime.
//
// Failure leaves output objects unchanged. The optional failure record gives
// the failing stage and the number of dependencies already loaded/pinned by
// that attempt; a failed initialization may therefore have irreversible pins.
class WXDLLIMPEXP_RIBBON wxRibbonToolkitRuntime final
{
public:
    static bool TryAcquire(std::shared_ptr<wxRibbonToolkitRuntime>& result,
                           wxRibbonToolkitFailure* failure = nullptr,
                           const wxString& payloadDirectory = wxString()) noexcept;

    bool TryCreateRibbon(winrt::XamlToolkit::Labs::WinUI::Ribbon& result,
                         wxRibbonToolkitFailure* failure = nullptr) const noexcept;
    bool TryCreateGroup(winrt::XamlToolkit::Labs::WinUI::RibbonGroup& result,
                        wxRibbonToolkitFailure* failure = nullptr) const noexcept;
    bool TryCreateCollapsibleGroup(
        winrt::XamlToolkit::Labs::WinUI::RibbonCollapsibleGroup& result,
        wxRibbonToolkitFailure* failure = nullptr) const noexcept;

private:
    explicit wxRibbonToolkitRuntime(
        std::shared_ptr<wxRibbonToolkitRuntimeState> state)
        : m_state(std::move(state)) {}

    std::shared_ptr<wxRibbonToolkitRuntimeState> m_state;
};

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RIBBON && WXWINUI_RIBBON_TOOLKIT

#endif // _WX_RIBBON_TOOLKIT_RUNTIME_H_
