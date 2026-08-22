# ADR 0004: Keep wx public classes toolkit-neutral

- Status: Accepted for the V0
- Date: 2026-07-26
- Scope: public `wxMSW`/`wxWinUI` class names, ABI, RTTI, XRC and backend
  selection

## Context

The WinUI port and the classic MSW port both implement the normal wxWidgets
public classes: `wxButton`, `wxTextCtrl`, `wxFrame`, and so on. The selected
toolkit header supplies one definition and CMake links one implementation.
This is the same compile-time toolkit model used by the other wxWidgets
ports.

An earlier delivery note proposed parallel public classes such as
`wxWinUI::Button` and `wxMSW::Button`, selectable in one program. That is not
a packaging cleanup. It changes the public source API and requires new
factories, RTTI identities, event-table types, XRC names, exports and
composite-control contracts.

The shared-island architecture does not require public backend coexistence.
A wxWinUI build already mixes WinUI-backed controls, classic native HWND
surfaces and generic wx controls through the slot/bridge boundary while the
application continues to use standard wx class names.

## Use case

For the V0, an application chooses `winui` or `msw` when configuring wxWidgets
and keeps portable source using `wxButton`, `wxDialog`, etc. The desired
namespace benefit is limited to clearer ownership of new **private**
implementation helpers. Loading two independently implemented public
`wxButton` types into one process is not a V0 use case.

## Evidence from the Button pilot

Button is representative because it participates in public construction,
events, RTTI, XRC and generic composites:

- `include/wx/msw/button.h` and `include/wx/winui/button.h` both declare the
  exported global class `wxButton : public wxButtonBase`;
- `src/common/btncmn.cpp` registers the single dynamic class name `wxButton`;
- `src/xrc/xh_bttn.cpp` creates `wxButton` directly for the XRC class
  `"wxButton"`; there is no backend discriminator in the resource;
- the WinUI CMake source selection removes `src/msw/button.cpp` before adding
  `src/winui/button.cpp`, proving that the implementations are alternatives,
  not linkable peers;
- `dumpbin /exports` on the existing x64 shared builds
  (`wxmsw333u_core_vc_x64_custom.dll` and
  `wxwinui333u_core_vc_x64_custom.dll`) shows the same decorated constructor,
  destructor, vtable and `wxButtonBase`-facing symbols, including
  `??0wxButton@@...`, `??1wxButton@@...` and `??_7wxButton@@6B@`.

The existing MSW and WinUI builds are the useful pilot for the selected
compile-time model: each links and exports one coherent `wxButton`, uses the
same wx RTTI/XRC name and dispatches the same public event type. Conversely,
the identical global symbols and single dynamic-class/XRC identity are direct
negative evidence for linking both implementations unchanged. A throwaway
parallel-class prototype would only reproduce this already demonstrated ODR
and factory collision.

## Options considered

### A. Namespace only private implementation helpers

Public wx classes remain unchanged. New ABI-hidden helpers may use an internal
namespace such as `wxWinUI::detail` when that improves ownership boundaries.
Existing exported helpers are not renamed merely for style.

This has low source/ABI risk and is compatible with upstream wxWidgets'
toolkit-neutral API. It does not provide two public Button types in one build.

### B. Add parallel public backend classes

Expose `wxWinUI::Button` and `wxMSW::Button`, then make `wxButton` an alias or
wrapper for a default.

This duplicates public class hierarchies and needs distinct DLL exports,
`wxClassInfo` names, XRC handlers, event-table declarations and composite
control types. Aliases cannot provide two distinct runtime types, while
wrappers change layout, RTTI and ABI. It is rejected for the V0.

### C. Select a peer at runtime behind one public class

Keep one `wxButton` and choose the backend through a pimpl/factory.

This is the only option that could preserve the public name while allowing
per-instance backend selection, but it requires both native implementations
to conform to a new common peer interface and be linkable without their
current global definitions. Window creation, subclassing, protected virtuals,
RTTI, focus and composite controls make this a cross-toolkit redesign rather
than a WinUI refactor. It is rejected for the V0.

### D. Formalize separate toolkit libraries/ABIs

Keep the current model: a build selects one toolkit library and its public
headers/implementation. Private backend code can still be namespaced.

This matches every existing wxWidgets port and the current CMake/header
dispatch. It precludes public per-control backend selection but preserves
portable wx source and coherent ABI.

## Decision

Adopt **D** as the external/public model and **A** as the internal naming
policy:

1. public controls keep their standard global wx names;
2. `wxBUILD_TOOLKIT=winui` and `wxBUILD_TOOLKIT=msw` remain distinct library
   builds/ABIs selected at configure/link time;
3. no public `wxWinUI::Button` or `wxMSW::Button` family is introduced;
4. new private, non-exported helpers may use backend/detail namespaces, but
   this ADR authorizes no mass rename and no ABI-visible namespace change;
5. XRC continues to instantiate the standard class name against the toolkit
   linked by the application.

## Consequences

- Existing application source, RTTI, XRC and event bindings remain compatible.
- The V0 documentation must not promise simultaneous public backend classes.
- Native/generic controls can still coexist with WinUI peers inside a WinUI
  window; that is an implementation mix, not two public toolkit APIs.
- A future request for per-control runtime backend selection requires a
  separate upstream-facing RFC/ADR, ABI versioning, factory/XRC design and
  prototype library. It cannot be implemented as aliases or a namespace-only
  migration.

## Verification and maintenance

- WinUI and MSW shared builds each export one global `wxButton` symbol family.
- Standard Button RTTI/event/XRC tests run against the toolkit-specific build;
  no new backend-qualified resource name is added.
- Release documentation consistently describes backend selection as
  compile-time.
- Any future change to public backend namespaces must supersede this ADR and
  include shared/static ABI and two-backend link proofs.

## Non-goals

- Renaming existing exported `wxWinUI*` helper functions.
- Shipping both core toolkit DLLs as one coherent public object model.
- Runtime switching of an existing control's rendering backend.
- Native migration of generic/MSW fallbacks solely to satisfy a naming scheme.
