# Toolkit identity and source compatibility

WinUI and MSW are separate, compile-time wxWidgets toolkits. They expose the
same portable wx class names, but select different class implementations and
library ABIs. Recompile the application and all wx-dependent libraries against
one consistent toolkit package; do not mix the two core DLLs or their headers.
See [ADR 0004](adr/0004-backend-namespaces.md).

## Compile-time conditions

| Condition | Supported meaning |
|---|---|
| `__WINDOWS__` | Windows operating-system services; also true for other Windows-hosted toolkits. It does not select a control implementation. |
| `__WXMSW__` | The MSW window substrate, currently shared by ordinary MSW and WinUI. `wxWindow`/`wxControl` and common HWND infrastructure remain MSW implementations. |
| `__WXWINUI__` | WinUI toolkit selected. Its control dispatch branches must precede the alternative MSW branches. |
| `wxUSE_WINUI3` | WinUI implementation enabled. Required when the WinUI toolkit is selected; disabling it is not an MSW fallback configuration. |
| `defined(__WXMSW__) && !defined(__WXWINUI__)` | Ordinary MSW-specific control implementation details, such as spin-control buddy windows. |
| `wxUSE_OLE`, other `wxUSE_*` switches | Availability of that particular subsystem; neither a toolkit nor an OS macro substitutes for its feature switch. |

Keep the shared MSW macro for this ABI. Removing it globally would also remove
required HWND/window-loop implementations and change public declarations.
No new global capability flag is needed: use the existing OS, toolkit and
feature conditions separately. Native fallback availability is per component,
not a property inferred for every control from `__WXMSW__`.

## Native-control differences

Portable operations such as values, selection, labels and wx event binding
remain the application-facing contract. Inheriting from an MSW implementation
class or using its internal/protected helpers is not portable source code.
In particular, the following differences are intentional, not missing Win32
buddy objects to synthesize behind a XAML control:

| Class | Ordinary MSW | WinUI |
|---|---|---|
| `wxRadioBox` | `wxCompositeWindow<wxStaticBox>` plus `wxRadioBoxBase`; has static-box sizing/painting and native child enumeration helpers. | `wxControl` plus `wxRadioBoxBase`; no conversion to `wxStaticBox`, native static-box paint contract, or native radio-button child collection. |
| `wxSpinCtrl` | Derives from `wxSpinButton`, owns a native buddy edit window; provides `GetBuddyWndProc`, `GetSpinForTextCtrl`, `ProcessTextCommand` and protected native normalization. | Derives from `wxSpinCtrlBase`; NumberBox has no Win32 buddy. Portable numeric/text operations and snap-to-ticks use the XAML peer. |
| `wxSpinCtrlDouble` | Generic composite implementation deriving from `wxSpinCtrlBase`. | NumberBox implementation deriving directly from `wxSpinCtrlBase`; `wxHAS_NATIVE_SPINCTRLDOUBLE` is defined. |
| `wxSpinButton` | Derives from `wxSpinButtonBase`, with native up-down normalization/notification handling. | Same portable base; no protected native `NormalizeValue` implementation. |

`ContainsHWND`, `MSWGetFocusHWND`, `MSWOnNotify`, `MSWOnScroll`, message
preprocessing, DPI-font and background-brush hooks still exist through the
shared MSW window hierarchy. Their presence does **not** mean a NumberBox has
a buddy HWND, a RadioBox is a native static box, or XAML controls emit native
control notifications. Similarly, a callable theme/focus hook is not proof of
matching visual or keyboard behaviour. Those are component integration tests,
not compile-contract assertions.

## Regression checks

`tests/winui/toolkit-contract` is a standalone CMake consumer using only the
installed package (`find_package(wxWidgets CONFIG REQUIRED COMPONENTS core)`).
Configure it with `CMAKE_PREFIX_PATH` pointing to that installation and
`WX_EXPECT_TOOLKIT=msw` or `winui`. Its CTest builds four separate translation
units: all 35 directly dispatched public headers and the RadioBox, SpinCtrl,
and SpinButton portable/native contracts. These are compile-only checks: they
do not create windows, run a resize loop, or claim control behaviour parity.

`tests/winui/selftest/header-dispatch.cmake` checks the actual public dispatch
headers and rejects deliberately reversed/incorrect guard fixtures. The
compile consumers independently verify the selected types, including the
case where both Windows toolkit macros are defined.

WinUI currently supports the documented MSVC toolchain. A compiler or platform
not executed in the evidence is not considered qualified by these probes.
