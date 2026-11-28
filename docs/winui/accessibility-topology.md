# wxWinUI accessibility topology

This document defines which accessibility tree owns each logical wx control.
It is an implementation invariant, not just a testing convention: a control
must never appear once through its covered HWND and a second time through its
XAML peer.

## Authority for a hosted slot

A WinUI-hosted wx window has three physical layers:

1. its covered native HWND shell;
2. a host-owned XAML carrier and slot container;
3. the component-owned XAML content.

The carrier and slot container are visual infrastructure. They are excluded
from the Control view, but a plain XAML layout element does not necessarily
create an automation peer: a Raw-view client may therefore see some of these
layers collapsed rather than as explicit nodes.
The component content is authoritative when the wx window has no
`wxAccessible`:

- its XAML peer is visible in the Control view;
- the covered HWND answers `UiaRootObjectId` with a server-side Raw-only
  provider;
- that shell provider reports no control/content/focusable/name state;
- `OBJID_CLIENT` is suppressed so MSAA cannot duplicate the XAML control.

When an application installs `wxAccessible`, authority changes synchronously:

- the native HWND/MSAA provider becomes authoritative;
- the component content is forced to `AccessibilityView::Raw`;
- its exact previous local `AccessibilityView` value is retained;
- removing `wxAccessible`, replacing content, or rolling a transaction back
  restores that exact authored value.

Every shell provider carries the slot lifetime, HWND generation, content
generation, and authority generation. A provider retained across teardown,
handle reuse, content replacement, reparenting, or an authority change becomes
inert instead of calling a stale wx object.

The visual root and semantic target are deliberately distinct. Layout,
clipping, tooltip policy and focus traversal belong to the visual root; UIA
Name, HelpText, relations and role belong to the semantic target. A semantic
target must remain inside its visual root for the entire published content
generation. If application code detaches it during a transaction, the adapter
falls back to the visual root rather than retaining an invalid relation.

## Expected trees

Control view:

```text
top-level window
└── logical wx control (one semantic peer)
    └── semantic child peers, for a composite control only
```

Raw-view visual/infrastructure topology (nodes without a peer may be
collapsed):

```text
top-level window
├── DesktopWindowXamlSource bridge (infrastructure)
│   └── carrier / slot grids (infrastructure)
│       └── component XAML content
└── covered HWND shell (Raw-only, no duplicated semantics)
```

Native GDI-only controls remain native and appear once. A composite may expose
real business children (for example RadioButtons in a RadioBox or toolbar
commands), but visual Borders, Grids, title TextBlocks, placeholders, and
separators must not create extra Control-view elements.

## Composite roots

`Grid`, `Border`, `StackPanel`, and `ContentControl` do not create automation
peers merely because `AutomationProperties` are attached to them. Composite
controls use `wxWinUICreateAccessibleGrid()` as their semantic root:

| wx control | UIA control type | class name | semantic children |
|---|---|---|---|
| `wxStaticBox` | Group | `wxStaticBox` | none owned yet; hosted controls remain sibling slots |
| `wxRadioBox` | Group | `wxRadioBox` | RadioButton peers |
| `wxToolBar` | ToolBar | `wxToolBar` | command/button peers |
| `wxStatusBar` | StatusBar | `wxStatusBar` | status text |

The root peer reads `AutomationProperties.Name`, `HelpText`, `AccessKey`, and
`LocalizedControlType` normally. The shared slot adapter remains the sole
writer of a normal hosted control's fallback Name.

## Attached-property ownership

The adapter never treats an effective empty value as proof that a property is
unowned. It reads local dependency-property state and follows these rules:

- an initially authored value, including an explicit empty Name, is preserved;
- once wx supplies a missing Name, direct XAML mutation or `ClearValue()`
  permanently relinquishes that Name for the current content generation;
- an authored `AutomationProperties.LabeledBy` relation suppresses the wx
  fallback Name while it is present; removing the relation restores the exact
  previous Name ownership decision instead of blindly claiming the property;
- content replacement, failed replacement rollback and cross-TLW migration
  transfer relation/Name provenance transactionally, including a direct
  `SetName()` followed by `ClearValue()` while old content is detached;
- context help is the primary UIA HelpText and the wx tooltip is its fallback;
- a wx help/tooltip override saves and later restores the exact local HelpText
  value, including the distinction between unset and an authored empty value;
- a direct XAML HelpText mutation while wx owns it relinquishes ownership until
  the wx-side value changes again;
- ToolTipService ownership is tracked independently because its value may be a
  string, a `ToolTip` object, or null.

Enabled state is applied to the host-owned carrier, not to component content,
so effective ancestor disabling reaches pointer, keyboard, and UIA without
overwriting a component's intrinsic disabled state. Layout direction is
applied to the content while slot geometry stays physical-LTR.

Two dependency-property limitations are explicit:

- assigning exactly the same string already supplied by wx may produce no
  property-change notification, so application intent is intrinsically
  indistinguishable until another observable mutation occurs;
- a custom semantic target which is a `UIElement` but not a
  `FrameworkElement` cannot currently install the Style/Name observer. Its
  authored state is inspected on every ordinary slot sync, but a standalone
  dynamic mutation may converge only when another dirty signal occurs.

## Patterns

Built-in WinUI peers provide the normal action patterns (`Invoke`, `Toggle`,
`SelectionItem`, `Selection`, and `RangeValue`). Tests must query the peer that
is actually present in the UIA tree. In particular, a ListView's item data peer
owns `SelectionItem`; the `ListViewItem` FrameworkElement peer is a separate
visual peer and is not the correct oracle.

The native WinUI `TextBoxAutomationPeer` surface exposes Text and Value
semantics to UIA clients. On the currently pinned WinAppSDK runtime, however,
the in-process C++/WinRT `AutomationPeer::GetPattern()` projection may return
null for both even though an external UIA client can obtain the native
providers. Automated tests therefore validate the Edit identity and wx/native
text model, and exercise Value only when that in-process provider is
projected. Text document ranges and the externally visible Text/Value patterns
remain an explicit Accessibility Insights/client gate; the port must not
invent a replacement pattern merely to satisfy an in-process test.

## Qualification

Automated coverage verifies:

- singular shell/XAML authority and exact authority restoration;
- root role, class, and dynamic name for every composite root above;
- UIA action patterns, disabled rejection, and retained-provider teardown
  where the runtime projects those patterns in-process;
- local Name, HelpText, tooltip, enabled, focus, RTL, content-generation, and
  cross-top-level migration state;
- selection through the ListView item data peer.

Release qualification still requires manual Accessibility Insights snapshots
of both Control and Raw views, including the actual StaticBox/hosted-control
sibling relationship and external Text/Value providers, plus Narrator checks
for reading order, dynamic labels/help, localized control types, focus
announcements, multi-top-level reparenting, and Light/Dark/High Contrast
transitions. Store those results in the V0 verification log; automated
topology tests do not replace that manual gate.
