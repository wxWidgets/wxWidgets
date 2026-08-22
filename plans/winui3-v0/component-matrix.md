# Matrice de support wxWinUI 3 — V0

Cette matrice est l’autorité de la phase 010. Elle décrit la promesse V0, pas
seulement la présence d’un fichier dans `src/winui`. Les preuves détaillées et
les validations physiques encore ouvertes sont consignées dans
[verification.md](verification.md).

## Légende

| Classe | Signification |
|---|---|
| **Supported Alpha** | inclus dans la promesse alpha après fermeture des gates globales 010 |
| **Supported Beta** | inclus dans la promesse beta, avec les limites publiées ci-dessous |
| **Experimental** | construit et partiellement testé, sans garantie de compatibilité complète |
| **Generic/MSW fallback** | backend hérité disponible, mais pas encore qualifié comme supporté sous le bridge |
| **Excluded** | hors promesse V0 ; l’utiliser exige une qualification séparée |

Une classe `Supported` reste soumise au verrou de livraison global : build
clean-room, absence de crash/freeze et checklist physique correspondant à son
niveau. Une gate physique non réalisée empêche de déclarer la release, sans
transformer rétroactivement une preuve automatisée verte en échec de code.
Les contrats transversaux possèdent en plus leur propre maturité ci-dessous :
une suite locale verte ne permet pas de les déduire implicitement.

## Fondation, fenêtres et layout

| Composant | Backend réel | Classe V0 | Preuve ou limite principale |
|---|---|---|---|
| `wxWindow`/contrôle custom | HWND/GDI sous bridge | Experimental | entrée, UIA et composition physique restent à fermer |
| `wxWinUIControlHost` | proxy de slot partagé | Experimental | lifecycle, state adapter et teardown déterministes ; qualification transversale ouverte |
| `wxWinUITopLevelHost` | une île par TLW | Experimental | host/slots/reparent automatisés ; campagne physique globale ouverte |
| `wxWinUIXamlHost` | slot du host partagé | Experimental | aucune île autonome par contrôle ; input/focus/Z physiques non qualifiés |
| `wxTopLevelWindow`/`wxFrame` | shell MSW + host/chrome WinUI | Experimental | focus, DPI, thème, UIA et multi-TLW physiques |
| `wxDialog` | HWND frame-class + bande de slots XAML partagée | Experimental | contrats default/cancel/owner/refit automatisés ; qualification physique encore ouverte |
| `wxPopupWindow`/transient/`wxMiniFrame` | shell MSW | Generic/MSW fallback | smoke seulement, pas de contrat V0 complet |
| `wxPanel` | common + shell MSW | Generic/MSW fallback | fondation utilisée, qualification étendue au plan 012 |
| `wxScrolledWindow`/Canvas | generic | Generic/MSW fallback | scroll/clip automatisés, campagne physique et qualification 012 |
| `wxSplitterWindow` | generic | Generic/MSW fallback | modèle automatisé ; rendu/capture physique restant |
| `wxSashWindow`/`wxSashLayoutWindow` | generic | Generic/MSW fallback | plan 012 |
| `wxNativeWindow`, MDI | HWND imbriqué | Excluded | spikes de topologie du plan 015 |

Le host représente un plan XAML unique au-dessus des HWND du client. Il ne peut
pas représenter l’imbrication `XAML A > HWND > XAML B`; un composant nécessitant
cette alternance n’est jamais supporté implicitement.

## Contrats transversaux du host

Ces lignes s’appliquent à tous les composants. Elles empêchent notamment de
lire les preuves déterministes du host comme une qualification physique déjà
acquise.

| Contrat | Maturité actuelle | Preuve automatisée | Gate restant avant support |
|---|---|---|---|
| Pointer, hit-test et capture | Experimental | routeur island-first, états de geste et lifetime testés | clic/double/wheel/capture physiques sur pairs XAML et HWND génériques |
| Clavier, mnemonics et accélérateurs | Experimental | ordre de prétraduction et commandes menu testés | saisie réelle, menus ouverts, AltGr/dead keys/IME |
| OLE drag-and-drop | Experimental | broker, résolution de cible et sessions testés | texte/fichiers depuis une application externe, reparent/destroy pendant drag |
| Focus, Tab/Shift-Tab et curseurs | Experimental | arbitre de focus et traversée mixte déterministes | parcours réel XAML/Win32, curseurs/busy et sortie d’île |
| Z-order, scroll, clip et splitter | Experimental | modèles, régions et synchro bornée testés | overlaps, tracking scrollbar/splitter et latence sur écran réel |
| UIA et Narrator | Experimental | topologie/automation properties internes testées | Accessibility Insights, clients UIA externes et Narrator |
| DPI multi-écrans 100/150/200 | Experimental | conversions DIPs/pixels et transitions simulées testées | déplacement physique entre écrans et inspection du rendu |
| Light/Dark/High Contrast à chaud | Experimental | propagation de thème déterministe testée | inspection visuelle après changements système à chaud |
| RTL à chaud | Experimental | géométrie et direction sémantique testées | rendu, hit-test et navigation physiques |
| Multi-TLW, reparent, minimize et Alt-Tab | Experimental | lifetime/reparent inter-host automatisés | activation, regroupement, propriétaires et destruction sur bureau réel |

## Contrôles natifs principaux

| Composant | Backend réel | Classe V0 | Preuve ou limite principale |
|---|---|---|---|
| `wxButton`/`wxBitmapButton` | WinUI `Button` | Experimental | suites internes vertes ; clic physique et thèmes/DPI globaux |
| `wxToggleButton`/bitmap | WinUI `ToggleButton` | Experimental | suites internes vertes ; entrée physique globale |
| `wxCheckBox` | WinUI `CheckBox` | Experimental | état/events couverts ; campagne physique |
| `wxRadioButton`/`wxRadioBox` | WinUI natif/composite | Experimental | modèle et apparence couverts ; focus/RTL physique |
| `wxCommandLinkButton` | composite WinUI | Generic/MSW fallback | pas de qualification publique complète |
| `wxTextCtrl` | WinUI `TextBox`/`PasswordBox` | Experimental | suite publique encore rouge sur les cas d’injection ; limites rich text |
| `wxSearchCtrl` | WinUI `AutoSuggestBox` | Experimental | suite ciblée verte ; IME/UIA/menu physique |
| `wxChoice` | WinUI `ComboBox` readonly | Experimental | modèle interne vert ; suite publique/entrée à fermer |
| `wxComboBox` | WinUI `ComboBox` editable | Experimental | `wxCB_SIMPLE` non représenté distinctement |
| `wxBitmapComboBox` | WinUI `ComboBox` + contenu bitmap | Experimental | modèle interne vert ; suite publique à fermer |
| `wxListBox`/`wxCheckListBox` | WinUI `ListView`/`CheckBox` | Experimental | modèle stable vert ; réalisation XAML et entrée physique |
| `wxGauge` | WinUI `ProgressBar` | Experimental | modèle interne vert ; apparence globale |
| `wxSlider` | WinUI `Slider` | Experimental | API de ticks/labels partielles |
| `wxScrollBar` | WinUI `ScrollBar` | Experimental | séquences internes vertes ; entrée physique |
| `wxSpinButton`/`wxSpinCtrl`/double | RepeatButtons/`NumberBox` | Experimental | modèle interne vert ; clavier/locale physique |
| `wxDatePickerCtrl` | WinUI `DatePicker` | Experimental | `wxDP_SPIN` utilise le dropdown |
| `wxTimePickerCtrl` | `TimePicker` transactionnel masqué + champs `NumberBox` H/M/S ordonnés selon la locale | Experimental | précision seconde, 12/24 h et ordre locale automatisés ; clavier, UIA et retrait physique du popup de période ouverts |
| `wxCalendarCtrl` | WinUI `CalendarView` | Experimental | numéros de semaine non supportés |

`wxLB_OWNERDRAW` et la famille d’extensions wxMSW
`GetItem()`/`CreateLboxItem()`/`GetItemIndex()`/`GetItemRect()`/
`RefreshItem()`, ainsi que le dessin personnalisé `OnDrawItem()`, ne sont pas
implémentés par le backend WinUI. Ils sont explicitement hors promesse V0 : le
sample n’active pas ses branches de personnalisation wxMSW sous
`__WXWINUI__`, et aucun faux objet/no-op n’est exposé.

## Affichage, books et arbres

| Composant | Backend réel | Classe V0 | Preuve ou limite principale |
|---|---|---|---|
| `wxStaticText`/Bitmap/Box/Line | peers WinUI | Experimental | apparence/UIA automatisées ; raster thème/DPI physique |
| `wxActivityIndicator` | WinUI `ProgressRing` | Experimental | fonctionnel, gate physique globale |
| `wxHyperlinkCtrl` | WinUI `HyperlinkButton` | Experimental | événements/apparence couverts ; interaction physique |
| `wxInfoBar` | WinUI `InfoBar` | Supported Beta | API complète, croix réelle automatisée et sample validé manuellement |
| `wxBannerWindow`/`wxAnimationCtrl` | generic | Generic/MSW fallback | non qualifiés pour la V0 |
| `wxNotebook` (`wxNB_TOP`) | WinUI `TabView` | Supported Alpha | contrats pages/lifetime dans l’inventaire Supported |
| `wxNotebook` (autres orientations, `wxNB_MULTILINE`, `wxNB_NOPAGETHEME`) | surface de pairs étendue dans le même slot | Experimental | styles, géométrie et lifetime automatisés ; absents du gate Supported |
| `wxSimplebook` | common | Supported Beta | contrats pages/lifetime verts |
| `wxChoicebook`/`wxListbook`/`wxTreebook` | composites | Supported Beta | suites dédiées vertes, dépendances natives conservées |
| `wxToolbook` | generic + toolbar | Experimental | dépend de la gate physique toolbar |
| `wxTreeCtrl` | WinUI `TreeView` | Supported Alpha | `wxTR_LINES_AT_ROOT`, `wxTR_ROW_LINES`, `wxTR_FULL_ROW_HIGHLIGHT` et `wxTR_TWIST_BUTTONS` non supportés |
| `wxAuiNotebook` | custom/generic | Excluded | plan 014 |
| `wxTreeListCtrl` | DataView generic | Generic/MSW fallback | plan 012 |

## Menus, chrome et dialogues

| Composant | Backend réel | Classe V0 | Preuve ou limite principale |
|---|---|---|---|
| `wxMenuBar`/`wxMenu` | WinUI `MenuBar`/`MenuFlyout` | Experimental | `[WinUIMenu]` vert ; ouverture/rendu physique global |
| `wxToolBar` | WinUI AppBar/StackPanel | Experimental | suites déterministes vertes ; overflow physique restant |
| `wxStatusBar` | WinUI Grid/TextBlock | Experimental | contrats automatisés ; campagne physique |
| `wxToolTip` | adapter du slot | Experimental | délais globaux non appliqués aux tooltips XAML |
| `wxRichToolTip` | popup générique géré (`TeachingTip` natif désactivé) | Experimental | candidat Supported Beta après gates binaires frais ; API publique conservée |
| `wxMessageDialog` | presenter WinUI ; HWND Window par défaut, Overlay opt-in | Supported Alpha | matrice boutons/default/owner automatisée |
| `wxTextEntryDialog`/Password | presenter WinUI ; HWND Window par défaut, Overlay opt-in | Supported Alpha | valeur/validation/owner/refit automatisés |
| `wxColourDialog` | presenter `ColorPicker` | Experimental | couleur retournée par OK, couleurs personnalisées et `ChooseFull` non qualifiées |
| Mode commun `ContentDialog` Overlay | presenter WinUI opt-in | Experimental | ne modifie pas `wxDialog` ; stacking/interaction physiques à qualifier |
| `wxFontDialog`/`wxFindReplaceDialog` | generic | Generic/MSW fallback | 013/2A : données/événements/refit/lifetime/100 cycles verts ; clavier/rendu/UIA physiques ouverts |
| `wxFileDialog`/`wxDirDialog` | MSW `IFileDialog` hybride | Generic/MSW fallback | 013/2A : owner/nested/résultats/100 cycles verts au seam COM ; shell et fallback legacy physiques ouverts |
| `wxFilePickerCtrl`/`wxDirPickerCtrl`/`wxFontPickerCtrl`/`wxColourPickerCtrl` | composites generic/MSW hybrides; Colour common + pair WinUI | Experimental | 013/3 : API, événements, XRC, focus, reparent, lifetime et 100 cycles verts; shells/flyout, thème, DPI, RTL et UIA physiques ouverts |
| `wxRichMessageDialog`/`wxGenericProgressDialog`/`wxProgressDialog`/`wxBusyInfo` | generic + MSW `TaskDialogIndirect` hybride | Generic/MSW fallback | 013/2B : API, fallback, réentrance/lifetime et 100 cycles par famille verts ; rendu, shell, DPI/theme/HC/UIA physiques ouverts |
| `wxWizard`, property sheets/preferences | generic/MSW | Excluded | plan 013 |

## Données, contenu riche et surfaces natives

| Famille | Backend réel | Classe V0 | Suite prévue |
|---|---|---|---|
| Header/ListCtrl/ListView/virtual | generic GDI | Generic/MSW fallback | plan 012 |
| DataView/List/Tree, TreeList | generic GDI | Generic/MSW fallback | plan 012 |
| Grid, VListBox, HtmlListBox | generic/custom | Generic/MSW fallback | plan 012 |
| ComboCtrl/OwnerDrawnCombo, Rearrange, EditableList | generic composites | Generic/MSW fallback | plan 012 |
| `wxCollapsiblePane` | generic composite | Excluded | layout/focus/pages mixtes non qualifiés |
| `wxGenericDirCtrl`/`wxFileCtrl` | generic filesystem composite | Excluded | arbres/listes, D&D et dialogues non qualifiés |
| PropertyGrid, filesystem composites | generic/custom | Excluded | plan 012 avant promotion |
| AUI, Ribbon, RichText, STC, HTML | generic/custom | Excluded | plan 014 |
| WebView, Media, GLCanvas, ActiveX | HWND/COM/swapchain | Excluded | spikes du plan 015 |
| wxDC/GDI+/D2D/printing | MSW conservé | Generic/MSW fallback | backend de dessin, pas un pair XAML |
| Taskbar/notifications/splash/tip | MSW/generic | Excluded | plan 013 |

Toute classe publique absente de cette matrice est **Excluded** pour la V0.
L’existence d’un backend common/generic ou MSW ne vaut jamais qualification
implicite sous le bridge.

## Limites API publiées

- `wxTextCtrl` ne fournit pas de rich text réel pour `wxTE_RICH/RICH2`.
  `SetStyle()`/`GetStyle()` ne sont pas implémentés, et le style passé à
  `SetDefaultStyle()` n’est pas appliqué au peer.
- `wxComboBox` conserve `wxCB_SIMPLE` dans les flags mais ne possède pas de
  rendu simple distinct.
- `wxListBox`/`wxCheckListBox` ne supportent pas `wxLB_OWNERDRAW`,
  `GetItem()` ni l’apparence personnalisée par item.
- `wxSlider::SetTick()` et `SetThumbLength()` sont des no-op ; labels,
  sélection de plage et taille de thumb ne sont pas supportés.
- `wxDatePickerCtrl` représente `wxDP_SPIN` par le sélecteur dropdown.
- `wxCalendarCtrl` n’expose ni colonne de semaines ni
  `wxCAL_HITTEST_WEEK`.
- `wxNotebook` implémente `wxNB_TOP` avec le `TabView` natif et
  `wxNB_BOTTOM`/`wxNB_LEFT`/`wxNB_RIGHT`, `wxNB_MULTILINE` et
  `wxNB_NOPAGETHEME` avec une surface étendue dans le même slot. Seul
  `wxNB_TOP` appartient actuellement à l’inventaire Supported.
- `wxTreeCtrl` n’implémente pas `wxTR_LINES_AT_ROOT`, `wxTR_ROW_LINES`,
  `wxTR_FULL_ROW_HIGHLIGHT` ni `wxTR_TWIST_BUTTONS`.
- Les délais `wxToolTip::SetDelay()`, `SetAutoPop()` et `SetReshow()` ne
  modifient pas les tooltips XAML.
- `wxRichToolTip` utilise toujours le popup générique géré dans le profil
  candidat Supported Beta. Le backend `TeachingTip` reste désactivé tant que
  `Close`/`Closed` ne constitue pas une frontière de retrait physique prouvée;
  la promotion reste conditionnée aux gates partagés/statiques frais et à
  l'absence de nouveau dump WER.
- `wxColourDialog` ne promet pas encore la couleur retournée par validation,
  la palette personnalisée ni la sémantique `ChooseFull`.
- L’ellipsize START/MIDDLE de `wxStaticText` avec markup imbriqué peut perdre
  la mise en forme.
- L’inspection UIA externe Text/Value, Narrator et les mutations sémantiques
  non-`FrameworkElement` restent des gates physiques de livraison.

## Matrice plateforme

| Dimension | Profil | Classe |
|---|---|---|
| OS | Windows 11 | Supported Alpha |
| Architecture | x64 | Supported Alpha |
| Compilateur | Visual Studio 2022 / MSVC | Supported Alpha |
| Configuration | Release | Supported Alpha |
| Linkage | shared DLL | Supported Alpha |
| Identité | framework-dependent, unpackaged | Supported Alpha |
| OS | Windows 10 1809+ | Experimental |
| Architecture | x86, ARM64 | Experimental |
| Configuration | Debug | Experimental |
| Linkage | bibliothèques statiques | Experimental |
| Linkage | monolithic | Experimental |
| Identité | packaged/MSIX | Experimental |
| Installation relocatable/`find_package` | implémentation shared/static candidate | Experimental |
| Déploiement self-contained WinAppSDK | non fourni | Excluded V0 |

Le profil démontré exige aussi `wxUSE_UNICODE_UTF8=OFF`. Le runtime Windows App
SDK correspondant et le redistribuable MSVC adapté à l’architecture restent des
prérequis machine ; voir [la documentation de build et de
redistribution](../../docs/winui/README.md).

## Extensions après la V0

Les fallbacks et exclusions ne sont pas abandonnés. Leur qualification
composant par composant est planifiée dans la
[roadmap bêta étendue](../winui3-beta-extended/README.md), en privilégiant un
fallback common/generic complet avant la création d’un nouveau pair XAML.
