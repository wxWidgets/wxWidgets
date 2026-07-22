# Matrice de migration des composants wxWinUI 3

## Comment lire la matrice

Cette matrice couvre tous les peers dédiés de `src/winui`, les contrôles publics
core/adv utilisés par la V0 et les grandes familles GUI susceptibles d’interagir
avec l’île. Les helpers très spécialisés non nommés restent hors promesse; leur
absence ne vaut jamais support implicite. Elle distingue :

- **Native WinUI** : pair XAML dédié dans `src/winui` ;
- **Generic fallback** : implémentation wx dessinée/composée, conservée pour V0 ;
- **MSW hérité** : implémentation Win32 historique compilée sous `__WXWINUI__` ;
- **Hybride** : shell MSW + intégration/children WinUI ;
- **Non migré** : fonctionne éventuellement, mais aucune qualification WinUI ;
- **Hors V0** : exclusion recommandée de la promesse alpha/beta.

« Alpha » signifie à corriger/qualifier avant la V0 alpha; « Beta » avant la
V0 beta; « Post » après. Les détails d’exécution sont dans les fiches liées.

## Infrastructure et top-level

| Composant | Implémentation actuelle | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxWindow`/contrôle custom | MSW/GDI sous bridge | Alpha | entrée, Z, D&D, enable/tooltips/UIA communs | [00](components/00-foundation-host.md) |
| `wxWinUIControlHost` | Proxy vers slot TLW | Alpha | lifetime/revokers/state adapter | [00](components/00-foundation-host.md) |
| `wxWinUITopLevelHost` | Native/hybride WIP | Alpha | découpage, stress, teardown, reparent | [00](components/00-foundation-host.md) |
| `wxWinUIXamlHost` | Ancienne île autonome | **P0 incompatible** | migrer vers slot unique | [00](components/00-foundation-host.md) |
| `wxTopLevelWindow` | MSW + host WinUI | Alpha | multi-TLW, owner, DPI/theme | [12](components/12-toplevel-platform.md) |
| `wxFrame` | MSW + Menu/Tool/Status WinUI | Alpha | accélérateurs/chrome/Z | [08](components/08-menus-chrome-tooltips.md) |
| `wxDialog` | MSW frame-class + presenter | Alpha | Enter/Escape/default/focus/owner | [09](components/09-dialogs-pickers-transients.md) |
| `wxPopupWindow`/`wxTransientPopupWindow` | MSW hérité | Beta | coexistence bridge/transitoires | [09](components/09-dialogs-pickers-transients.md) |
| `wxMiniFrame` | MSW hérité | Beta | host TLW et owner/reparent smoke | [12](components/12-toplevel-platform.md) |
| `wxMDIParent/ChildFrame` | MSW hérité, non migré | Hors V0 | îles imbriquées/child frames | [12](components/12-toplevel-platform.md) |
| `wxTaskBarIcon`/`wxTaskBarButton` | MSW hérité | Post | menus/dialogues smoke | [12](components/12-toplevel-platform.md) |

## Conteneurs, scrolling et layout génériques

| Composant | Implémentation actuelle | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxPanel` | Common + shell MSW | Alpha fondation | navigation mixte, enabled, paint/reparent | [13](components/13-generic-foundation-aux-ui.md) |
| `wxScrolledWindow`/`wxScrolledCanvas` | Generic | Alpha fondation | latence/clip/hit-test des slots pendant scroll | [13](components/13-generic-foundation-aux-ui.md) |
| `wxSplitterWindow` | Generic | Alpha gate | trait blanc, sash capture/cursor, clip/Z | [13](components/13-generic-foundation-aux-ui.md) |
| `wxSashWindow`/`wxSashLayoutWindow` | Generic | Beta/Post | tracker, capture, overlap/DPI/theme | [13](components/13-generic-foundation-aux-ui.md) |
| `wxNativeWindow` | MSW hérité | Hors V0 | HWND externe, topologie Z non représentable | [13](components/13-generic-foundation-aux-ui.md) |

## Boutons et sélection

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxButton` | Native WinUI Button | Alpha rouge | clic E2E, default/dialog, mnemonic, DPI/UIA | [01](components/01-buttons-selection.md) |
| `wxBitmapButton` | `wxButton` WinUI + bitmap states | Alpha | mêmes gates, bitmap-only UIA | [01](components/01-buttons-selection.md) |
| `wxCommandLinkButton` | Generic au-dessus de wxButton | Beta | note/layout/bitmap/default/focus | [01](components/01-buttons-selection.md) |
| `wxToggleButton` | Native WinUI ToggleButton | Alpha rouge | clic, enabled, apparence | [01](components/01-buttons-selection.md) |
| `wxBitmapToggleButton` | Native partiel | Alpha | tous bitmaps d’état/getters/marges/DPI | [01](components/01-buttons-selection.md) |
| `wxCheckBox` | Native WinUI CheckBox | Alpha partiel | événement 3-state, Command, enabled init | [01](components/01-buttons-selection.md) |
| `wxRadioButton` | Native WinUI RadioButton | Alpha rouge | groupes, navigation, focus, UIA | [01](components/01-buttons-selection.md) |
| `wxRadioBox` | Native composite Grid/RadioButtons | Alpha rouge | preferred focus, item show/enable, DPI | [01](components/01-buttons-selection.md) |

## Texte, recherche et combos

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxTextCtrl` | Native TextBox/PasswordBox | **P0 rouge** | lifetime/crash, dirty/caret/hit-test/max/readonly | [02](components/02-text-entry.md) |
| `wxSearchCtrl` | Native AutoSuggestBox | Alpha rouge | vrai TextEntry, menu/cancel/ownership | [02](components/02-text-entry.md) |
| `wxChoice` | Native ComboBox readonly | Alpha rouge | tri, best size, events/client data, DPI | [03](components/03-item-controls.md) |
| `wxComboBox` | Native ComboBox editable | Alpha rouge | template/caret, popup events, styles | [02](components/02-text-entry.md) |
| `wxBitmapComboBox` | Native partiel sur ComboBox | Alpha rouge | modèle bitmap/item atomique, taille/DPI | [03](components/03-item-controls.md) |
| `wxComboCtrl` | Generic fallback | Beta | TextCtrl child, popup/focus/Z | [03](components/03-item-controls.md) |
| `wxOwnerDrawnComboBox` | Generic fallback | Beta | GDI popup, D&D/capture/theme | [03](components/03-item-controls.md) |

## Listes et composites d’items

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxListBox` | Native WinUI ListView | Alpha rouge | ownership, tri, hit-test, multisélection | [03](components/03-item-controls.md) |
| `wxCheckListBox` | Native sur ListBox | Alpha rouge | rebuild destructif, clavier/UIA | [03](components/03-item-controls.md) |
| `wxRearrangeList/Ctrl/Dialog` | Composite commun | Beta | dépend CheckList/dialogues/D&D | [03](components/03-item-controls.md) |
| `wxEditableListBox` | Generic composite ListCtrl | Beta | dépend ListCtrl/editor/focus | [03](components/03-item-controls.md) |
| `wxVListBox`/`wxHtmlListBox` | Generic custom | Beta | GDI/scroll/selection/UIA | [10](components/10-data-and-generic-controls.md) |

## Plages, progression et spin

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxGauge` | Native ProgressBar | Alpha proche | `wxGA_PROGRESS`, Pulse→determinate | [04](components/04-range-spin.md) |
| `wxSlider` | Native Slider | Alpha rouge | séquence complète wxEVT_SCROLL, styles/ticks | [04](components/04-range-spin.md) |
| `wxScrollBar` | Native ScrollBar | Alpha | clamp et événements end-scroll | [04](components/04-range-spin.md) |
| `wxSpinButton` | Native RepeatButtons | Alpha | orientation, clavier, UIA | [04](components/04-range-spin.md) |
| `wxSpinCtrl` | Native NumberBox | Alpha rouge | valeur/wrap/base/selection/events | [04](components/04-range-spin.md) |
| `wxSpinCtrlDouble` | Native NumberBox | Alpha rouge | digits/increment/wrap/texte/events | [04](components/04-range-spin.md) |

## Date, temps et pickers inline

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxDatePickerCtrl` | Native DatePicker | Alpha rouge | clamp range/default bounds/focus/styles | [05](components/05-date-time-calendar.md) |
| `wxTimePickerCtrl` | Native TimePicker | Beta | secondes, invalid values, locale | [05](components/05-date-time-calendar.md) |
| `wxCalendarCtrl` | Native CalendarView | Beta/Alpha vitrine | navigation/events/Mark/hit-test | [05](components/05-date-time-calendar.md) |
| `wxColourPickerCtrl` | Native ColorPicker composite | Beta | event parent, label, theme/enabled | [09](components/09-dialogs-pickers-transients.md) |
| `wxFilePickerCtrl`/`wxDirPickerCtrl` | Generic fallback | Beta | modal/focus/re-fit/owner | [09](components/09-dialogs-pickers-transients.md) |
| `wxFontPickerCtrl` | Generic fallback | Beta | FontDialog/re-fit | [09](components/09-dialogs-pickers-transients.md) |

## Affichage et feedback

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxStaticText` | Native TextBlock | Alpha | ellipsize/font/colours/UIA | [06](components/06-display-feedback.md) |
| `wxStaticBitmap` | Native Image | Alpha proche | refresh BitmapBundle DPI | [06](components/06-display-feedback.md) |
| `wxStaticBox` | Native composite | Alpha | masque/theme/font/double scaling/UIA group | [06](components/06-display-feedback.md) |
| `wxStaticLine` | Native Border | Post | décoration UIA/DPI smoke | [06](components/06-display-feedback.md) |
| `wxActivityIndicator` | Native ProgressRing | Beta | UIA/destroy while active | [06](components/06-display-feedback.md) |
| `wxHyperlinkCtrl` | Native HyperlinkButton | Beta | hover/context/alignment/appearance/UIA | [06](components/06-display-feedback.md) |
| `wxInfoBar` | Native InfoBar | **P0 build**, puis Beta | API checkbox/effects, measure/layout | [06](components/06-display-feedback.md) |
| `wxBannerWindow` | Generic custom | Post | GDI/theme/composition smoke | [11](components/11-advanced-rich-surfaces.md) |
| `wxAnimationCtrl` | Generic fallback | Post | timer/GDI/theme/lifetime | [11](components/11-advanced-rich-surfaces.md) |

## Books et arbres

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxNotebook` | Native TabView | Alpha rouge | GetTabRect/orientations/icons/DPI/Tab | [07](components/07-books-trees.md) |
| `wxSimplebook` | Common/generic | Beta | pages mixtes/focus | [07](components/07-books-trees.md) |
| `wxChoicebook` | Generic composite + Choice | Beta | dépend Choice/focus/layout | [07](components/07-books-trees.md) |
| `wxListbook` | Generic composite + ListCtrl | Beta | GDI/scroll/focus | [07](components/07-books-trees.md) |
| `wxToolbook` | Generic composite + ToolBar | Beta | dépend toolbar | [07](components/07-books-trees.md) |
| `wxTreebook` | Generic composite + TreeCtrl | Beta | dépend TreeCtrl | [07](components/07-books-trees.md) |
| `wxAuiNotebook` | Generic/custom non migré | Beta si AUI | drag/capture/reparent | [11](components/11-advanced-rich-surfaces.md) |
| `wxTreeCtrl` | Native TreeView | Alpha rouge | 25 assertions, hit-test/key/lifetime/perf | [07](components/07-books-trees.md) |
| `wxTreeListCtrl` | Generic DataView fallback | Beta | dépend DataView/D&D/editor | [10](components/10-data-and-generic-controls.md) |

## Menus, toolbar, status et tooltips

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxMenuBar` | Native XAML MenuBar | **P0** | accélérateurs, OPEN/routage, enabled, DPI | [08](components/08-menus-chrome-tooltips.md) |
| `wxMenu` popup | Native MenuFlyout | Alpha | event order, drain queue, lifetime | [08](components/08-menus-chrome-tooltips.md) |
| `wxToolBar` | Native AppBar/StackPanel | Alpha | delete/toggle/control/stretch/hit/DPI | [08](components/08-menus-chrome-tooltips.md) |
| `wxStatusBar` | Native Grid/TextBlock | Beta | styles/tips/ellipsis/DPI/UIA | [08](components/08-menus-chrome-tooltips.md) |
| `wxToolTip` | Adaptation ponctuelle | **P0 infra** | adapter slot commun/ownership/settings | [08](components/08-menus-chrome-tooltips.md) |
| `wxRichToolTip` | Native TeachingTip partiel | Beta | rect/delay/style/RAII/transient manager | [08](components/08-menus-chrome-tooltips.md) |

## Dialogues

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxMessageDialog` | Native presenter + fallback MessageBox | Alpha | mode/default/owner/focus/button matrix | [09](components/09-dialogs-pickers-transients.md) |
| `wxTextEntryDialog`/Password | Native presenter | Alpha | frame-class, validators, UIA shell/lifetime | [09](components/09-dialogs-pickers-transients.md) |
| `wxColourDialog` | Native ColorPicker/presenter | Beta | custom colours/ChooseFull | [09](components/09-dialogs-pickers-transients.md) |
| `wxFontDialog` | Generic fallback | Alpha/Beta | grow-only re-fit | [09](components/09-dialogs-pickers-transients.md) |
| `wxFindReplaceDialog` | Generic fallback | Beta | re-fit/focus/frame-class | [09](components/09-dialogs-pickers-transients.md) |
| `wxFileDialog`/`wxDirDialog` | MSW hérité | Beta | nested native loops/owner/Alt-Tab | [09](components/09-dialogs-pickers-transients.md) |
| `wxRichMessageDialog`/`wxGenericProgressDialog` | Generic/MSW selon classe | Beta | checkbox/details/modal/owner | [09](components/09-dialogs-pickers-transients.md) |
| `wxWizard` | Generic/MSW composite non qualifié | Post | pages/dialog semantics/theme | [11](components/11-advanced-rich-surfaces.md) |

## Contrôles de données génériques

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxHeaderCtrl` | Generic forcé | Beta | drag/resize/scroll/UIA/theme | [10](components/10-data-and-generic-controls.md) |
| `wxListCtrl`/`wxListView`/virtual | Generic forcé | Beta | GDI input/editor/D&D/UIA/perf | [10](components/10-data-and-generic-controls.md) |
| `wxDataViewCtrl/List/Tree` | Generic | Beta | editor/focus/D&D/scroll/UIA | [10](components/10-data-and-generic-controls.md) |
| `wxGrid` | Generic custom | Beta | editor WinUI, scroll sync, capture/D&D | [10](components/10-data-and-generic-controls.md) |
| `wxPropertyGrid` | Generic/custom | Post | nombreux editors/focus/scroll | [10](components/10-data-and-generic-controls.md) |
| `wxCollapsiblePane` | Generic fallback | Beta | layout/focus/pages mixtes | [10](components/10-data-and-generic-controls.md) |
| `wxGenericDirCtrl`/`wxFileCtrl` | Generic/MSW composites | Post | list/tree/D&D/dialogs | [10](components/10-data-and-generic-controls.md) |

## AUI, contenu riche et surfaces natives

| Composant | Implémentation | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxAuiManager`/floating panes | Generic + art MSW | Beta si promis | reparent multi-TLW, drag/capture | [11](components/11-advanced-rich-surfaces.md) |
| `wxRibbon*` | Generic/custom non migré | Post | GDI/reparent/theme/DPI | [11](components/11-advanced-rich-surfaces.md) |
| `wxRichTextCtrl` | Generic custom | Post | D&D OLE, scroll, editor, UIA | [11](components/11-advanced-rich-surfaces.md) |
| `wxStyledTextCtrl` | Scintilla/MSW integration | Post | IME, D&D, focus, native surfaces | [11](components/11-advanced-rich-surfaces.md) |
| `wxHtmlWindow` | Generic custom | Post | GDI/scroll/links/printing | [11](components/11-advanced-rich-surfaces.md) |
| `wxWebView` Edge/IE | MSW/WebView2 hérité | Hors V0 recommandé | composition/focus/IME/D&D | [11](components/11-advanced-rich-surfaces.md) |
| `wxMediaCtrl` | MSW hérité | Hors V0 recommandé | HWND/video surface sous bridge | [11](components/11-advanced-rich-surfaces.md) |
| `wxGLCanvas` | MSW/WGL hérité | Hors V0 recommandé | own-DC/swap surface/Z/input | [12](components/12-toplevel-platform.md) |
| wxDC/GDI+/Direct2D/printing | MSW conservé | Alpha fondation | composition/invalidations, pas migration | [12](components/12-toplevel-platform.md) |

## UI auxiliaire et intégrations spécialisées

| Composant | Implémentation actuelle | Maturité | Reste principal | Fiche |
|---|---|---:|---|---|
| `wxProgressDialog` | MSW hérité | Beta | owner, nested loop, cancel/skip, shutdown | [13](components/13-generic-foundation-aux-ui.md) |
| `wxBusyInfo` | Generic TLW | Beta | owner, busy cursor, update/destroy | [13](components/13-generic-foundation-aux-ui.md) |
| `wxNotificationMessage` | MSW/generic selon config | Beta/Post | actions/timeout/Explorer restart/lifetime | [13](components/13-generic-foundation-aux-ui.md) |
| `wxSplashScreen` | Generic Frame | Post | startup timer, bitmap DPI, early teardown | [13](components/13-generic-foundation-aux-ui.md) |
| `wxTipWindow` | Generic popup | Beta/Post | activation/capture/dismiss/multi-monitor | [13](components/13-generic-foundation-aux-ui.md) |
| `wxAuiToolBar` | Generic AUI | Post | art/overflow/floating/capture | [13](components/13-generic-foundation-aux-ui.md) |
| AUI MDI | Generic AUI | Post | pages/menus/focus/reparent/host | [13](components/13-generic-foundation-aux-ui.md) |
| `wxActiveXContainer` | MSW OLE | Hors V0 | in-place activation, Z/input/UIA/COM | [13](components/13-generic-foundation-aux-ui.md) |

## Classification ferme de la cible V0

Cette classification est la promesse recommandée. « Supported » ne décrit pas
l’état actuel : il devient vrai uniquement après le gate de la fiche. Les rows
détaillées de la matrice restent l’autorité nominale pour chaque classe; le
tableau ci-dessous regroupe ce profil sans remplacer ces rows. Seules les classes
absentes de toute la matrice sont Excluded jusqu’à décision explicite.

| Famille | Cible alpha | Cible beta | Exclu jusqu’après beta |
|---|---|---|---|
| Fondation | `wxWindow`, host/slots, TLW/Frame/Dialog, Panel, Scrolled, Splitter | + Popup/MiniFrame/Sash selon tests | MDI, NativeWindow, GL |
| Contrôles natifs principaux | Button/BitmapButton/Toggle/Check/Radio, TextCtrl/SearchCtrl, Choice/Combo/List/CheckList/BitmapCombo, Gauge/Slider/Scroll/Spin, StaticText/Box/Bitmap, Notebook/Tree, DatePicker | + CommandLinkButton, TimePicker, Calendar, Hyperlink, Activity et ColourPicker | StaticLine et aucun nouveau peer natif |
| Chrome et feedback | MenuBar/Menu/ToolBar/ToolTip; InfoBar compile + message/buttons/checkbox | + StatusBar, RichToolTip, InfoBar effets/apparence complets | — |
| Dialogues | Message/TextEntry/Password et Font fallback sans crop | + Find/Colour/File/Dir/Rich/Progress selon fiche | Wizard avancé |
| Data/generic | smoke alpha de ListCtrl/DataView/Grid/Header dans les samples, classé `Fallback` | `Supported Fallback` après contrats fiche 10; ComboCtrl/OwnerDrawn, Rearrange/Editable et picker ctrls selon leurs dépendances | PropertyGrid et filesystem composites sauf besoin |
| Books/composites | aucun composite promis; smoke seulement si une page alpha l’utilise | Simple/Choice/List/Tool/TreeBook après leurs dépendances | AuiNotebook avancé sauf besoin |
| UI auxiliaire | Busy/Progress uniquement smoke si rencontrés | promotion nominale uniquement pour les rows marquées Beta après fiche 13 | Splash/Notification/Tip/AuiToolBar/AUI MDI/ActiveX par défaut |
| Rich/surfaces | aucune | AUI seulement si explicitement promis et stress vert | Ribbon, RichText/STC avancés, HTML avancé, WebView, Media |

Pour l’alpha développeur interne, le seuil minimal est 001–004 → 007a → 005 →
007b → 006, puis la stabilisation lifetime/crash de TextCtrl dans 008a. Pour
l’alpha publique, toutes les
cellules « cible alpha » doivent être soit vertes, soit reclassées explicitement
Experimental/Excluded avant release.

## Synthèse de portée V0 recommandée

### Promesse alpha

- Infrastructure/host, top-level et dialogues de base.
- Contrôles natifs déjà présents, mais uniquement leur sous-ensemble explicitement
  vert dans les fiches.
- Fallbacks ListCtrl/DataView/Grid/books visibles dans samples, sans promesse de
  migration native.
- Exclusion documentée de MDI, Media, WebView, GL, Ribbon, STC/RichText avancé.

### Promesse beta

- Tous les composants marqués Alpha/Beta dans cette matrice passent leurs gates.
- Fallbacks génériques qualifiés pour input/focus/D&D/scroll/UIA/theme/DPI.
- AUI seulement si le stress reparent/floating est vert.

### Post-beta

- Migration ItemsView native des data controls, si décidée.
- Rich content/surfaces, MDI, Ribbon, Media, WebView, GL.
- Coexistence de backends/namespaces après ADR.
