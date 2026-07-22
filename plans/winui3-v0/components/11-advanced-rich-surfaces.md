# Fiche 11 — AUI, Ribbon, rich content et surfaces embarquées

Baseline `20207bdbbb`. Ces familles sont principalement post-beta. La feuille
de route vise à éviter qu’elles bloquent la fondation tout en laissant des gates
précis pour une promotion future.

## wxAUI Manager / floating panes / AuiNotebook

Implémentation generic/custom avec art MSW. AUI reparent et crée des TLW
flottantes (`src/aui/floatpane.cpp:80`, `framemanager.cpp:1352`) et son notebook
gère drag/capture (`auibook.cpp:1411,2372`).

**Si AUI est dans la beta :**

1. deux frames, docking/undocking d’un pane contenant TextCtrl/Tree/Grid ;
2. migration slot vers host flottant puis retour ;
3. drag cancel/outside, capture et freeze breaker ;
4. fermeture frame flottante pendant animation/drag ;
5. DPI inter-écrans, RTL, hot theme, UIA et D&D ;
6. AuiNotebook tabs/pages/focus indépendamment de wxNotebook.

Sinon, annoncer AUI experimental et le garder hors promesse V0.

## wxRibbon

Generic/custom, aucune branche WinUI. Post-beta. Avant promotion : galerie,
buttons/dropdown, minimized state, reparent children, keyboard tips/mnemonics,
GDI sous bridge, DPI/theme/UIA et popup lifetime. Ne pas migrer en XAML sans ADR
et suite de tests dédiée.

## wxBannerWindow / wxAnimationCtrl

Fallbacks génériques. Post-beta polish : GDI/theme/DPI, timer/destroy, alpha,
resize/scroll et UIA decorative. Aucun peer natif prioritaire.

## wxRichTextCtrl

Generic custom et installe un `wxDropTarget` (`richtextctrl.cpp:340`). Bloqué par
le broker OLE. Post-beta : editing, styles, images, clipboard, D&D, scroll,
printing, context menus, IME, large document, UIA et hot theme.

## wxStyledTextCtrl / Scintilla

Generic Scintilla avec IME/Win32 et drop target. Post-beta : IME CJK, caret,
autocomplete/calltip child windows, clipboard, D&D, context menus, scrollbars,
focus/Tab, DPI/theme, large file et shutdown. Une intégration superficielle peut
casser l’IME; garder hors beta tant que ces gates ne sont pas exécutés.

## wxHtmlWindow

Generic scrolled/painted. Post-beta : layout/links/selection, scroll, context,
images, print, keyboard/focus, D&D URLs, DPI/theme/UIA. Conserver GDI.

## wxWebView (WebView2/IE)

MSW hérité avec controller attaché au HWND (`src/msw/webview_edge.cpp:499`).
Surface à haut risque sous le bridge : composition, focus/IME, context menus,
D&D, popups, permission dialogs, fullscreen et teardown async.

Recommandation : hors V0. Faire un spike distinct bridge + WebView2 + XAML
overlap + ContentDialog avant engagement. Ne pas déduire la compatibilité du
simple fait que la bibliothèque compile.

## wxMediaCtrl

MSW hérité, surface vidéo HWND/COM. Hors V0 recommandé. Spike : playback,
resize, z-order avec slot, overlay dialog, focus/keys, fullscreen, destroy while
playing et device loss.

## Ordre de promotion

1. AUI uniquement si besoin beta et stress reparent vert.
2. RichText/HTML après D&D/input/scroll.
3. STC après IME/lifetime.
4. WebView/Media après spike de composition.
5. Ribbon et finitions visuelles post-beta.

## Condition STOP

Si une surface possède sa propre composition/swapchain qui ne peut pas être
intercalée sous le bridge unique, ne pas ajouter un second bridge ad hoc. Arrêter
et rédiger une ADR de surface/Z-order avec options et limitation V0.

