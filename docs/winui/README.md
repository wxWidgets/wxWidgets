# wxWinUI: the WinUI 3 port of wxWidgets

Experimental toolkit rendering the standard wx controls with WinUI 3, while
every wx window keeps its real Win32 `HWND`.  The port defines both
`__WXWINUI__` and `__WXMSW__`: it is the MSW port with a different rendering
backend, not a separate platform.

The architecture reference lives in the top-level
[`WINUI_PORT_PROGRESS.md`](../../WINUI_PORT_PROGRESS.md) (see the update
titled *"RE-ARCHITECTURE — one island per top-level window"*).  In two
sentences: each top-level window hosts a single XAML island composited over
its client area; the WinUI-backed controls contribute one XAML element each
("slot") positioned/clipped/z-ordered by the shared host, which also routes
the input — synthesizing wx events over XAML content and forwarding real
native messages to the wx windows everywhere else.

## Building

Requirements:

- Windows 11 (Windows 10 1809+ should work, less tested);
- Visual Studio 2022 (MSVC); the toolkit is MSVC-only for now (C++/WinRT);
- CMake 3.21+;
- an internet connection for the first configure: the Windows App SDK
  (pinned in `build/cmake/winui.cmake`, currently 2.1.3) and its
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

Runtime switches:

- `WX_WINUI_DIALOG_WINDOW=1` — show the common dialogs in classic separate
  top-level windows instead of the default in-window `ContentDialog`
  presentation.

## Component mapping notes

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
    a duration of 0 means the platform default (200 ms).

## Runtime deployment (development profile)

The executables produced by this build are **framework-dependent and
unpackaged** in the Windows App SDK sense: they require the Windows App
Runtime framework package to be installed on the machine, and they carry a
small file payload next to the executable.  They are *not* self-contained.

- `Microsoft.WindowsAppRuntime.Bootstrap.dll` is deployed next to every
  executable; at startup `wxWinUI3Initialize()` uses it to bind the process
  to the installed **Windows App Runtime framework package** (major version
  matching the pinned SDK, currently 2.x).  The framework provides all the
  WinUI/WinAppSDK DLLs; they are *not* copied next to the application.
  On a machine without the framework, install the matching runtime: the
  MSIX packages ship inside the pinned `Microsoft.WindowsAppSDK.Runtime`
  NuGet package under `tools/MSIX/win10-<arch>/` (installable with
  `Add-AppxPackage`; experimental/preview runtimes additionally require
  sideloading/Developer mode to be enabled), or use the corresponding
  Windows App SDK runtime installer.
- `resources.pri` — the framework package's resource index, extracted at
  configure time from that same MSIX — is deployed next to every executable,
  together with the `Microsoft.UI.Xaml/Assets/` files it references, laid
  out like in the framework package.  Unpackaged processes resolve their
  `ms-appx:///` URIs against the PRI found next to the executable; without
  it `XamlControlsResources` fails with `0x80004005: Cannot locate resource
  from 'ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xaml'` and every
  control renders without its template.  This app-local copy is a
  **development-profile workaround** needed by the current experimental SDK
  series; it is not part of the documented Windows App SDK deployment
  contract.

### Coexistence with an application's own resources.pri

`resources.pri` is a per-application file, so the deployment never clobbers
an index it does not own: next to the deployed file it writes a
`resources.pri.wxwinui` tag recording the hash it deployed.  On later
builds, a `resources.pri` matching the tag is updated in place; a different
one (the application's own) is left untouched with a build warning — such
an application must merge the Windows App Runtime resources into its own
index, or set `wxWINUI3_DEPLOY_RESOURCES_PRI=OFF` to take over entirely.

### Automatic application

The deployment is implemented by `wx_winui3_deploy_runtime(<target>)` in
`build/cmake/winui.cmake` (executables only) and applied automatically to
every GUI executable created through the standard wx CMake functions
(samples, demos, tests including `test_gui`), so no per-target calls are
needed.  Copies use `copy_if_different` and each run takes a per-directory
lock (the `.wx-winui-deploy.lock` file left in the output directory is its
normal residue): the step is idempotent and safe for many targets sharing
one output directory even under parallel builds, but it only adds/updates
files — it does not delete files deployed by older configurations.  The
exact per-architecture payload is printed by the configure log
(`wxWinUI: unpackaged runtime payload (win-<arch>)`), sourced exclusively
from the pinned NuGet packages for the architecture selected with `-A`
(x64/Win32/ARM64).

Consuming an *installed* wxWidgets via `find_package(wxWidgets)` from an
external project is **not yet a supported contract** for this toolkit: the
deployment helper and payload are not installed/exported, and the exported
targets currently reference the WinAppSDK import libraries by absolute path
inside the build's package cache (pre-existing limitation), so the install
tree is not relocatable.  This is deliberately deferred until the licensing
status below is resolved.

### Licensing status — development only, do not distribute

The current pinned SDK series is only suitable for **development and
testing**; do not redistribute build outputs beyond that:

- `Microsoft.WindowsAppSDK.WinUI 2.1.0` is licensed as a **Microsoft
  Windows App SDK Engineering Preview**: no production use ("live operating
  environment") and no distribution of anything in the package, with no
  distributable-code exception.  This build therefore uses that package
  only at build time (WinMD metadata and headers for the C++/WinRT
  projection), which its development/test grant covers; nothing from it is
  deployed.
- `Microsoft.WindowsAppSDK.Foundation`/`.Runtime`/`.Base`/
  `.InteractiveExperiences` carry the standard Windows App SDK license
  terms, whose distributable-code clause covers "files that are binplaced
  with your application by the WindowsAppSDK NuGet package" — that is the
  bootstrap DLL — and redistributing the runtime MSIX packages to end users
  is the documented deployment mechanism.  The app-local `resources.pri` /
  assets extracted from the framework MSIX are **not** covered by that
  clause and are not part of the documented contract, so they must be
  treated as development-only until the port moves to an SDK series whose
  documented unpackaged deployment works without them.
- Microsoft's deployment guide also recommends deploying the Windows App
  SDK `.winmd` files next to unpackaged apps (cross-apartment marshaling
  metadata).  This build does not deploy them; no scenario needing them has
  been hit yet, and they are available in the framework MSIX if one comes
  up.

Moving to a stable (non-preview) Windows App SDK series is the path to a
distributable baseline; that is a pinned-version change with its own
validation, tracked as a blocking decision in `plans/README.md` (plan 001).

### Determinism

Acquisition of the toolchain inputs is deterministic: the NuGet package
versions *and their SHA-256 hashes* are pinned in `build/cmake/winui.cmake`
(downloads and cached archives are validated before extraction, extraction
and staging are atomic, extractions are stamped with their archive hash and
re-made when the pinned hash or version changes, and a corrupted cache
fails the configure).  One single Windows 10 SDK version supplies
`cppwinrt.exe`, `Windows.winmd` and the C++/WinRT headers, selected by
numeric version comparison among complete installations only.  These
behaviours are covered by CTest self-tests (`wx_winui_selftest_*`, in
`tests/winui/selftest/`) alongside the `wx_winui_runtime_smoke` runtime
probe.

Limitations of the demonstrated profile: Windows 11 x64, MSVC, Release,
shared (DLL) build, framework-dependent unpackaged, development use only.
Packaged deployment (MSIX identity), x86, ARM64, Debug, static and
monolithic builds are not qualified (known pre-existing gap: a monolithic
build creates `wxmono` without applying the C++/WinRT compile
configuration).

## Win32 / WinUI coexistence and progressive migration

A design goal of the re-architecture is that **classic Win32/MSW and
WinUI-backed controls coexist freely in the same window**:

- every control — WinUI-backed or not — is a real `HWND` child in the usual
  wx parent/child tree: `GetHandle()`, native subclassing, sizers, z-order
  and tab order behave as on wxMSW;
- windows that draw themselves with wx (`wxDC`) — the generic
  `wxDataViewCtrl`, `wxGrid`, custom `EVT_PAINT` windows, `wxBannerWindow` —
  paint natively under the island band and receive genuine native input
  (the island forwards the real messages wherever it has no XAML content;
  native scrollbar bands are even cut out of the island's window region so
  their tracking loops run untouched);
- a WinUI-backed control is simply one that registered a slot; nothing in
  the host requires *all* the controls of a window to be WinUI-backed.

The public API packaging for explicit mix-and-match — `wxWinUI::Button`
*and* `wxMSW::Button` classes selectable per scope with a using-directive,
`wxButton` deriving from the toolkit default — is planned but not
implemented yet: today the selection is per-build via the usual
`wx/foo.h` → `wx/winui/foo.h` dispatch.  The re-architecture keeps that
refactor mechanical: the WinUI implementation of each control lives
entirely in its `src/winui/*.cpp` behind a `unique_ptr` impl (no WinRT
types in public headers), and its only contact with the hosting layer is a
7-method private contract, so wrapping the same implementations under a
`wxWinUI::` namespace does not touch the hosting infrastructure at all.
