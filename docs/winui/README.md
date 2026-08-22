# wxWinUI: the WinUI 3 port of wxWidgets

Toolkit rendering the standard wx controls with WinUI 3, while every wx
window keeps its real Win32 `HWND`.  The port defines both
`__WXWINUI__` and `__WXMSW__`: it is the MSW port with a different rendering
backend, not a separate platform.

> **Current qualification status (2026-08-22).** The narrow Windows 11 x64,
> MSVC, Release, unpackaged **Supported V0** profile is a beta candidate, not
> yet a production-ready beta. The shared and static/no-exceptions automated
> gates are green, including the Supported V0 inventory (**50 cases / 927
> assertions** in each linkage) and HostLifecycle/HostState/broker (**190
> cases / 25,675 assertions** in each linkage). RC4 proved that a TLW-only
> product OLE registration misses the normal XAML surface. RC7 then showed
> that acquiring bridge+TLW while the frame is hidden can still yield zero
> native callback after presentation. The broker now waits for the first
> visible geometry flush and uses two physical COM adapters sharing one
> session; its focused shared/static gates pass (**31 cases / 1,330
> assertions** in each linkage). The next frozen product OLE/physical campaign,
> manual accessibility,
> DPI/theme/High Contrast/RTL checks and the signed 60-minute soak remain open.
> This status does not claim full wxMSW parity.

The architecture is defined by
[plan 003](../../plans/003-winui-shared-host-lifetime.md) and the
[scroll/Z-order ADR](adr/0003-scroll-sync-and-zorder.md).
`WINUI_PORT_PROGRESS.md` is a historical engineering journal, not a support
contract. Each top-level window hosts a single XAML island composited over its
client area; WinUI-backed controls contribute one XAML element each ("slot")
positioned, clipped and ordered by the shared host, which also routes input.
The current public support promise and its exact limitations are in the
[V0 component matrix](../../plans/winui3-v0/component-matrix.md).

## Building

Requirements:

- Windows 11 for the Supported V0 candidate profile (Windows 10 1809+ remains
  Experimental);
- Visual Studio 2022 (MSVC); the toolkit is MSVC-only for now (C++/WinRT);
- CMake 3.21+;
- one complete Windows 10/11 SDK installation (10.0.19041.0 or newer)
  containing `Windows.winmd`, the C++/WinRT headers and `cppwinrt.exe`; the
  default PRI deployment also requires `makepri.exe` from that same SDK
  version;
- an internet connection for the first configure: the Windows App SDK
  (stable 1.8 packages pinned in `build/cmake/winui.cmake`, currently
  runtime 1.8.260710003) and its
  dependencies are fetched as NuGet packages automatically, and the
  C++/WinRT projection headers are generated during the build.

Configure and build (clean-room, no pre-existing artefacts needed):

```
cmake -S . -B build-winui -G "Visual Studio 17 2022" -A x64 ^
      -DwxBUILD_TOOLKIT=winui -DwxBUILD_SAMPLES=SOME
cmake --build build-winui --config Release
```

`wxUSE_WINUI3` defaults to `ON` for this toolkit.  The samples and the DLLs
end up in `build-winui\lib\vc_x64_dll\`; run `widgets.exe` (the per-control
exerciser), `showcase.exe` (broad smoke test) or `minimal.exe` from there.
The unpackaged runtime payload described below is deployed next to them
automatically.

To verify a build's runtime deployment without starting a full sample, build
and run the standalone probe (requires configuring with a non-OFF
`wxBUILD_TESTS`, e.g. `-DwxBUILD_TESTS=ALL`; it builds into its own isolated
directory):

```
cmake --build build-winui --config Release --target wx_winui_runtime_smoke
build-winui\winui\runtime-smoke\wx_winui_runtime_smoke.exe
```

It bootstraps the Windows App SDK, loads `XamlControlsResources` and realizes
Button, TreeView and TabView in an off-screen island; it exits 0 on success
and prints a precise diagnostic (see the exit-code table in
`tests/winui/runtime-smoke.cpp`) when part of the payload is missing.

## Supported V0 test gate

WinUI test builds register the serialized `wx_winui_supported_beta` CTest on
the isolated desktop runner. It invokes the existing mixed `test_gui` binary
with the exact Catch2 filter
`[winui-v0-supported]~[.]~[physical]`. The positive tag is the explicit V0
support inventory; the two negative terms only exclude hidden and physical
cases and cannot broaden the selection to unrelated tests.

Run it from a configured build tree with:

```
ctest --test-dir build-winui -C Release \
      -R "^wx_winui_supported_beta$" --output-on-failure
```

Tests carrying other WinUI tags remain Experimental or excluded according to
the component matrix; absence from this positive tag is not a promotion.
The current pre-freeze shared and static/no-exceptions runs of this exact
inventory are green at **50 cases / 927 assertions** each. That result is
necessary but does not by itself qualify installed-package relocation or the
final release profile without fresh frozen-candidate build, runtime and human
evidence.

Runtime switches:

- `WX_WINUI_DIALOG_OVERLAY=1` — opt in to showing WinUI-drawn common dialogs
  as in-window `ContentDialog` overlays. The alpha default is a separate
  top-level `Window`; an explicit `wxWinUISetDialogPresentation()` call takes
  precedence over this environment switch. This selector applies to the
  WinUI presenters used by common dialogs (`wxMessageDialog`,
  `wxTextEntryDialog`, `wxColourDialog`, etc.); it does not change the
  topology of an ordinary `wxDialog`.

## Component mapping notes

- **wxDialog** remains a regular frame-class Win32 `HWND`. Its WinUI-backed
  children occupy slots in the shared XAML strip over the dialog client area;
  `wxDialog` itself is not a WinUI `Window` or `ContentDialog`.
- **wxInfoBar** maps to the WinUI `InfoBar` control.  The full common API is
  available, with these mappings:
  - the native close (X) button emits `wxEVT_BUTTON` with `wxID_CLOSE`
    through the wx handler chain (the XAML-side closing is cancelled so wx
    stays in charge) and the bar is dismissed only when the application
    does not handle that event — same contract as the generic version, so
    handlers reading `IsCheckBoxChecked()` on close work;
  - the checkbox (`wxINFOBAR_CHECKBOX`/`ShowCheckBox()`) and the custom
    buttons are composed together in `InfoBar.Content`, so both coexist like
    in the generic version (custom buttons still replace the built-in close
    button);
  - the show/hide effects (`SetShowHideEffects()`/`SetEffectDuration()`)
    are implemented with `AnimateWindow()` on the bar's HWND — the standard
    wxMSW `ShowWithEffect()` mapping of every `wxShowEffect` — because the
    XAML `InfoBar` has no equivalent parametrable open/close transition.
    The composited island band may only appear at the final frame of the
    animation.  If `AnimateWindow()` fails, the bar falls back to a plain
    show/hide (never a silent no-op: the bar is always shown/hidden);
    the requested duration is clamped to `[0, 10000]` ms, and a duration of
    0 means the platform default (200 ms).

## Runtime deployment

The executables produced by this build are **framework-dependent and
unpackaged** in the Windows App SDK sense: they require the Windows App
Runtime framework package to be installed on the machine, and they carry a
small file payload next to the executable.  They are *not* self-contained.
The target machine also needs the architecture-matching Microsoft Visual C++
Redistributable required by the MSVC-built wxWidgets binaries.

- `Microsoft.WindowsAppRuntime.Bootstrap.dll` is deployed next to every
  executable; at startup `wxWinUI3Initialize()` uses it to bind the process
  to the installed **Windows App Runtime framework package** (major version
  matching the pinned SDK, currently 1.8).  The framework provides all the
  WinUI/WinAppSDK DLLs; they are *not* copied next to the application.
  On a machine without the framework, install the matching runtime: the
  MSIX packages ship inside the pinned `Microsoft.WindowsAppSDK.Runtime`
  NuGet package under `tools/MSIX/win10-<arch>/`, or use the corresponding
  Windows App SDK runtime installer. See Microsoft's
  [framework-dependent unpackaged deployment guide](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/deploy-unpackaged-apps).
- `Microsoft.Web.WebView2.Core.dll` and the matching
  `WebView2Loader.dll` come from the single hash-pinned WebView2 NuGet package
  used both by the C++/WinRT projection and by the classic Edge backend. The
  Core DLL is the unpackaged activation server for the XAML WebView2 control;
  it exports its WinRT activation factory and needs neither `regsvr32` nor an
  additional PRI. The package's unmodified `LICENSE.txt` and `NOTICE.txt` are
  deployed under `licenses/Microsoft.Web.WebView2/`, satisfying the binary
  redistribution notice requirement. The Evergreen WebView2 Runtime must
  still be installed on the target machine.
- `resources.pri` is an **application resource index generated by MakePri**.
  At configure time, the build merges the three architecture-specific
  component indexes binplaced by the stable NuGet packages:
  `Microsoft.UI.Xaml.Controls.pri`, `Microsoft.UI.pri` and
  `Microsoft.WindowsAppRuntime.pri`.  It also copies the two
  `Microsoft.UI.Xaml/Assets/` files directly from the WinUI NuGet component.
  No file is extracted from a framework MSIX.  Unpackaged processes resolve
  their `ms-appx:///` URIs against the application PRI next to the
  executable; without the merged index, `XamlControlsResources` fails with
  `0x80004005: Cannot locate resource from
  'ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xaml'`. This follows
  Microsoft's [unpackaged MakePri guidance](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/mrtcore/localize-strings):
  omit the packaging section, enable the PRI indexer, omit `/IndexName` so
  the root map is `Application`, then copy the generated PRI beside the
   executable.
- 25 Windows App SDK metadata files (`*.winmd`) are deployed beside each
  executable: the two WinUI metadata files plus all pinned Foundation and
  InteractiveExperiences metadata files used by the projection. They are
  recommended for unpackaged deployment because WinRT apartment marshaling
  can need metadata even when ordinary in-process activation succeeds.
- `Microsoft.UI.Xaml/Assets/NoiseAsset_256x256_PNG.png` and
  `Microsoft.UI.Xaml/Assets/map.html` are copied with their framework-relative
  layout from the pinned stable WinUI package.

### Coexistence with an application's own resources.pri

`resources.pri` is a per-application file, so the deployment never clobbers
an index it does not own: next to the deployed file it writes a
`resources.pri.wxwinui` tag recording the hash it deployed.  On later
builds, a `resources.pri` matching the tag is updated in place; a different
one (the application's own) is left untouched with a build warning — such
an application must merge the same three Windows App SDK component PRIs into
its own `Application` map. When configuring the **wxWidgets package build**,
set `wxWINUI3_DEPLOY_RESOURCES_PRI=OFF` before building/installing wxWidgets
to omit the default PRI entirely. This is a package-producer option baked into
the generated deployment script; setting it in an external consuming project
does not alter an already installed package. A package built with the default
`ON` still detects and preserves an application-owned PRI instead of
overwriting it, but emits the warning described above.
`build/cmake/winui-priconfig.xml` is intentionally the minimal configuration
for merging these component PRI files only; it is not a complete template for
an application that also owns `.resw`, `.resjson` or other resources. Such an
application must extend its own MakePri configuration with the required
indexers and inputs.

### Automatic application

The deployment is implemented by `wx_winui3_deploy_runtime(<target>)` in
`build/cmake/winui.cmake` (executables only) and applied automatically to
every GUI executable created through the standard wx CMake functions
(samples, demos, tests including `test_gui`), so no per-target calls are
needed.  Copies use `copy_if_different` and each run takes a per-directory
lock (the `.wx-winui-deploy.lock` file left in the output directory is its
normal residue). The deployment dependency runs even when the executable did
not need relinking, so missing or modified non-PRI payload files and a missing
owned PRI are restored by an ordinary build. A PRI whose bytes no longer
match its ownership tag is conservatively treated as application-owned: it is
left in place, the stale tag is removed and the build warns; delete it
explicitly to redeploy the generated default. The step is idempotent and safe
for many targets sharing one output directory even under parallel builds. It
normally only
adds/updates files; the one deletion is an owned `resources.pri` (proved by
its matching sidecar hash) when reconfiguring with
`wxWINUI3_DEPLOY_RESOURCES_PRI=OFF`. It never deletes an application-owned
PRI or arbitrary legacy files. The exact per-architecture payload is printed
by the configure log (`wxWinUI: unpackaged runtime payload (win-<arch>)`) for
the architecture selected with `-A` (x64/Win32/ARM64).

### Installed-consumer beta candidate

The wxWinUI install/export machinery implements a relocatable consumer flow
from a different source/build directory. Until fresh relocated shared and
static consumers and the Supported V0 gate pass without a new WER dump, the
candidate beta flow is:

```cmake
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core)

add_executable(my_app WIN32 main.cpp)
target_link_libraries(my_app PRIVATE wx::core)
wx_winui3_deploy_runtime(my_app)
```

The explicit `wx_winui3_deploy_runtime(<executable>)` call is required for
external targets. It installs an always-run build dependency which copies the
installed, architecture-specific unpackaged payload beside the executable and
uses the same locking and `resources.pri` ownership rules as in-tree targets.
It does not copy the wxWidgets shared DLLs themselves: deploy those using the
application's normal DLL packaging step, or put their installed directory on
`PATH` while running from a build tree. The fixture's CTest runner uses the
latter approach without modifying the executable directory.
The helper and payload are located relative to `wxWidgetsConfig.cmake`; they do
not retain a path to the wx build tree or its NuGet cache. Shared wxWidgets
targets do not expose the private Windows App SDK import libraries in their
link interface. Static installs retain those transitive dependencies as two
imported targets whose `.lib` files are part of the install tree.

The repository contains a minimal external fixture in
`tests/winui/install-consumer`. For example, using distinct install and
consumer directories:

```powershell
cmake --install C:/build/wx-winui --config Release --prefix C:/stage/wx-winui
cmake -S tests/winui/install-consumer -B C:/build/wx-winui-consumer -A x64 `
  -DwxWidgets_DIR=C:/stage/wx-winui/lib/cmake/wxWidgets-3.3
cmake --build C:/build/wx-winui-consumer --config Release
ctest --test-dir C:/build/wx-winui-consumer -C Release --output-on-failure
```

Moving `C:/stage/wx-winui` before configuring the fixture is an additional
relocation check; only `wxWidgets_DIR` needs to change. The target machine
requirements at the start of this section still apply: the install payload is
framework-dependent, not self-contained.

### Licensing and redistribution

The pinned Windows App SDK 1.8 component packages carry the stable
`MICROSOFT WINDOWS APP SDK` terms.  Their distributable-code section permits
redistribution of files binplaced with an application by the
WindowsAppSDK NuGet package, subject to the distribution requirements in
that licence.  The deployed bootstrap and theme assets are such binplaced
files, as are the deployed Windows App SDK metadata files; `resources.pri` is
the application's own MakePri output produced from the binplaced component
indexes using Microsoft's documented unpackaged PRI indexing mechanism.

CMake verifies each pinned WindowsAppSDK package's `license.txt` before
assembling the payload.  It rejects an `ENGINEERING PREVIEW` licence, a
live-environment prohibition, or the absence of the expected NuGet
binplacement redistribution grant.  This guard prevents an accidental
version bump back to the non-redistributable 2.1 engineering packages that
previously blocked plan 001.  It is an engineering safeguard, not a
substitute for checking the licence's distribution requirements for a
particular product.

### Determinism

Acquisition of the toolchain inputs is deterministic: the NuGet package
versions *and their SHA-256 hashes* are pinned in `build/cmake/winui.cmake`
(downloads and cached archives are validated before extraction, extraction
is atomic, and same-version packages with different hashes coexist in
content-addressed archive/extraction paths). Shared package caches are locked,
and changing the configured cache root cannot silently reuse an automatically
selected extraction from the old root. Every cached archive is hash-validated
before extraction; every file in a managed extraction is covered by an
archive-bound SHA-256/size manifest revalidated on each configure, and a
modified extraction is rebuilt from the pinned archive. A corrupted archive
is never extracted or used. The
exception is an explicitly supplied
`-DwxWINUI3_<COMPONENT>_PACKAGE_DIR=...` or a matching source-tree vendored
package: these are intentional trusted inputs, accepted by marker path and
not hash-validated or deleted by CMake.

The generated C++/WinRT projection directory is content-addressed by all
metadata, `Windows.winmd`, `cppwinrt.exe` and a versioned generation contract.
Every generated header is covered by a SHA-256/size manifest verified on each
build; regeneration is locked and transactional. The merged PRI stage is
similarly content-addressed by every component PRI, the MakePri configuration,
`makepri.exe`, the selected SDK and a versioned invocation contract. One
single Windows 10 SDK version supplies `cppwinrt.exe`, `Windows.winmd` and the
C++/WinRT headers (and, when the default PRI deployment is enabled,
`makepri.exe`), selected by numeric version comparison among complete
installations only.
These behaviours are covered by CTest self-tests (`wx_winui_selftest_*`, in
`tests/winui/selftest/`) alongside the `wx_winui_runtime_smoke` runtime
probe.

Limitations of the demonstrated profile: Windows 11 x64, MSVC, Release and
framework-dependent unpackaged. Shared and static/no-exceptions focused gates
are green, but the post-RC7 corrected candidate has not yet been frozen and
signed through install/deployment plus physical qualification.
The port currently requires `wxUSE_UNICODE_UTF8=OFF`: XAML text offsets are
UTF-16 code-unit indices and the alternative UTF-8 `wxString` representation
is rejected at configure time until that mapping is qualified.
Packaged deployment (MSIX identity), x86, ARM64, Debug and monolithic builds
are not qualified (known pre-existing gap: a monolithic build creates
`wxmono` without applying the C++/WinRT compile configuration). Static linkage
is covered by the current narrow automated gates, not yet by every release
profile named above.

## Win32 / WinUI coexistence and progressive migration

A design goal of the re-architecture is that classic Win32/MSW and
WinUI-backed controls coexist on the same top-level client plane:

- every control — WinUI-backed or not — is a real `HWND` child in the usual
  wx parent/child tree, so `GetHandle()`, native subclassing and sizers retain
  their usual foundation. The host contains explicit adapters for z-order,
  focus and mixed Tab traversal; physical parity with wxMSW remains
  Experimental until the corresponding V0 qualification rows are complete;
- windows that draw themselves with wx (`wxDC`) — the generic
  `wxDataViewCtrl`, `wxGrid`, custom `EVT_PAINT` windows, `wxBannerWindow` —
  paint natively under the island band. The island-first root router forwards
  native messages when its XAML hit test finds no peer, and native scrollbar
  bands are cut out of the island region for their tracking loops. The
  deterministic routes are implemented, but real pointer/capture, scrolling
  and mixed-Z behaviour remain Experimental pending the physical campaign;
- a WinUI-backed control is simply one that registered a slot; nothing in
  the host requires *all* the controls of a window to be WinUI-backed.

The cross-cutting maturity table in the
[V0 component matrix](../../plans/winui3-v0/component-matrix.md) is part of
the support contract. A green component-specific suite does not by itself
promote physical input, D&D, accessibility, DPI/theme/RTL or multi-window
behaviour.

### OLE drag-and-drop ownership

Each WinUI top-level host owns one product OLE broker and one logical drag
session behind two physical COM target identities. The adapters are registered
atomically on the exact bridge and TLW `HWND` generations. The bridge receives
the normal XAML plane; the TLW receives native children exposed through holes
in the bridge region. Both callbacks feed the same logical hit-test,
coordinate-routing and drag-session authority.

Native acquisition is always deferred out of host construction, including when
the first WinUI control is added to an already-visible top-level. Existing
wxMSW targets remain registered until a visible `FlushSync()` has published
non-empty bridge geometry and the current structural epoch; that pass adopts
the logical targets and acquires the pair. Bridge show/geometry messages drive
bounded retries without polling. A reentrant presentation change before commit
rolls the provisional pair back exactly, restores the logical targets and
retries on a later stable flush. Once committed, the pair is retained across
later Hide/Show cycles, so no speculative post-presentation revoke is performed.

Classic per-window wxMSW registrations and the WinUI broker share a global,
generation-aware ledger. It serializes fixed and external ownership, reserves
both broker identities before either native registration, rolls the bridge back
if TLW acquisition fails or becomes pending, retains uncertain lock/revoke
failures closed, and retires TLW then bridge. XAML host and slot publication do
not wait for OLE ownership, so a temporarily pending or failed OLE acquisition
cannot discard successfully created control content. The design and failure
rules are recorded in [ADR 0002](adr/0002-ole-drop-integration.md).

The 2026-08-21 raw `DoDragDrop()` probe established the split topology: bridge
for the normal surface, TLW for a real native bridge hole. The frozen RC4
product gate then showed that TLW-only delivery times out on the normal surface
without any COM callback. RC7 further showed that a dual pair acquired during
hidden construction can remain absent from native delivery after presentation.
The visible-flush/two-adapter implementation is now green in both automated
linkages; the next frozen product-broker physical gate remains a distinct
release requirement.

This coexistence has a structural boundary: the single shared island cannot
represent the alternating nesting `XAML A > HWND > XAML B`. Components that
require such a topology remain Experimental or Excluded until a dedicated
architecture decision proves another composition model.

Public wx class names remain the standard toolkit-neutral names (`wxButton`,
`wxTextCtrl`, and so on), selected per build by the usual
`wx/foo.h` → `wx/winui/foo.h` dispatch. The port does not promise parallel
public `wxWinUI::Button` and `wxMSW::Button` class families in one
translation unit. Native/generic and WinUI-backed controls still coexist in a
single window through the slot contract described above; namespace use is
reserved for private backend implementation helpers where it improves
ownership boundaries without changing wxWidgets' public source API. The
decision and rejected alternatives are recorded in
[`ADR 0004`](adr/0004-backend-namespaces.md).
