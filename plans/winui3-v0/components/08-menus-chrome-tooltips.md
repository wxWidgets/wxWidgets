# Fiche 08 — Menus, toolbar, statusbar et tooltips

Baseline `20207bdbbb` + worktree. Cette fiche contient un bloqueur clavier P0 et
un bloqueur infrastructure tooltip.

## wxMenuBar et wxMenu popup — P0/P1

### Accélérateurs

Ne pas construire une deuxième table. `src/msw/menu.cpp:1272-1311` fusionne déjà
les accélérateurs et `src/msw/frame.cpp:771-780` les traduit. Corriger l’ordre
avant que `src/winui/winui.cpp:336-361` transforme le message en WM_NULL.

Ordre cible : filter app → accelerator/mnemonic wx → mixed Tab → XAML text →
dispatch. Tester une seule consommation dans loop wx, move/resize, MessageBox,
menu natif et OLE.

### Contrat menu

- émettre OPEN avant construction/refresh du flyout (`menubar.cpp:531-560`) ;
- router via menu→parents→menubar→frame (`src/common/menucmn.cpp:679+`) ;
- appliquer enabled/check/radio/label/bitmap à chaque ouverture ;
- sous-menu disabled non activable ;
- afficher mnemonics/access keys et accélérateur ;
- recalculer hauteur au DPI (`menubar.cpp:46-106`) ;
- si TryEnqueue retourne false, ne pas lancer la boucle drain
  (`menubar.cpp:578-595`).

**Tests :** Ctrl/Alt/F-key depuis TextBox/Button/Grid, disabled accelerator,
OPEN mutation add/delete/enable, handlers menu/menubar/frame, radio/check,
Escape, queue shutdown, destroy TLW et DPI.

## wxToolBar — alpha si chrome V0

### Défauts

- `DoDeleteTool()` rebuild avant retrait de `m_tools`, recréant un ghost
  (`toolbar.cpp:187-253`; ordre base `src/common/tbarbase.cpp:319`) ;
- control tools ignorés (`:267-275`) ; stretch traité comme separator ;
- DoSetToggle no-op (`:227-231`) ;
- whole-toolbar disable non propagé ;
- NOICONS/HORZ_LAYOUT/disabled bitmap/dropdown incomplets ;
- hit-test suppose 48 DIPs (`:403-415`) ;
- bitmap non rechargé au DPI (`:281-290`).

### Plan

1. Corriger Delete/Remove/Clear sans rebuild sur un item encore possédé.
2. Modèle visuel stable par tool et bounds réels pour hit-test.
3. Toggle/dropdown/radio/disabled et short-help incremental updates.
4. Stretch star column/row; stratégie slot pour AddControl.
5. Styles text/icons/layout, bitmap states et DPI.

**Tests :** Remove/Delete/Clear immédiatement après Realize, clic ghost absent,
normal/check/radio/dropdown, AddControl, stretch resize, vertical/horizontal,
NOICONS/text, disabled, hit-test, tooltips et DPI.

## wxStatusBar — beta

Les modes ellipse sont réduits à end (`statbar.cpp:97-130`), SetStatusStyles
rebuild sans lire les styles (`:179-183`), SHOW_TIPS et size grip manquent, et
pixels/DIPs sont mélangés (`:109-148`).

**Plan :** cellule par field avec style border, ellipse/tooltip du texte complet,
conversion px→DIP unique, font, controls de field, role UIA status et grip si
requis.

**Tests :** fixed/weighted widths, resize, flat/raised/sunken, trois ellipses,
SHOW_TIPS, AddFieldControl, font, menu help, DPI/theme/UIA.

## wxToolTip — alpha infrastructure

Le chemin Win32 est sauté pour les slots (`src/msw/window.cpp:1812-1828`) et la
majorité des composants n’appelle pas le helper WinUI. Certains overrides
détruisent l’objet transmis alors que l’API commune conserve `m_tooltip`.

**Plan :** adapter au slot qui conserve `wxToolTip`, texte/mutation/clear,
enable global, initial/autopop/reshow delays, max width/multiline lorsque possible,
fallback documenté sinon, cleanup au replace/reparent/destroy.

**Tests :** tous composants interactifs, SetTip/GetToolTip, replace/delete,
global enable/settings, multiline/width, delayed show, disable, reparent/destroy.

## wxRichToolTip — beta/lifetime alpha

Rect, background, icon, kind, title font et show delay ne sont pas appliqués
(`richtooltip.cpp:80-120`). Le host est `new` puis plusieurs opérations peuvent
lever avant le callback Closed (`:120-198`).

**Plan :** unique_ptr jusqu’à ouverture sûre, enregistrement TransientManager,
generation TLW, timer de show delay, anchoring rect, mapping des propriétés et
fallback generic explicite si non représentable.

**Tests :** tous setters, rect, delay/timeout 0, plusieurs tips, target/TLW
destroy, exception path et shutdown.

## Sortie

Alpha : accélérateurs et event order verts, aucun popup freeze, toolbar core et
tooltips communs stables. Beta : styles/bitmaps/DPI/UIA complets pour chrome et
RichToolTip public honoré ou fallback explicite.
