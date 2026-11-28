# Plan 007b — Stabiliser focus logique, Tab mixte et curseurs

> **Instructions** : préserver le pipeline clavier du plan 004 et ne produire
> qu’une transition focus wx finale par interaction. Aucun commit sans accord.
>
> **Drift check** : `$scope=@('include/wx/winui/private/tlwhost.h','src/winui/tlwhost.cpp','src/winui/ctrlhost.cpp','src/winui/winui.cpp','src/msw/evtloop.cpp','src/msw/window.cpp','tests/events/keyboard.cpp','tests/controls/windowtest.cpp','samples/winuispike'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P0 pour dialogues/alpha
- **Effort** : M/L
- **Risque** : HIGH
- **Dépend de** : 003, 004, 007a
- **Débloque** : 006 et gates composants focus

## État et scope

Propriétaires principaux : `FocusSlot()` (`tlwhost.cpp:714+`),
`OnSlotWindowFocus()` (`:1717+`), `OnTakeFocusRequested()` (`:1733+`) et
`ResolveFocusHwnd()` (`:1754+`). Les curseurs passent par `SetSlotCursor()`,
`MirrorNativeCursor()` et `ResetRootCursor()` (`:631-713`). RadioBox illustre un
root XAML non-Control sans cible préférée. Les slots animés peuvent réimposer la
flèche après resync.

Dans le scope : arbitre focus, chaîne Tab/Shift-Tab, preferred target et cursors
normal/busy/custom. Hors scope : accélérateurs/menu (004), pointer brut (007a),
dialog default/cancel (006) et API spécifique d’un composant.

## Étapes et vérifications

1. Ajouter dans `tests/events/keyboard.cpp` et `windowtest.cpp` une matrice
   Grid GDI ↔ Button/TextBox/Combo/RadioBox XAML, Tab/Shift/Ctrl+Tab, disable/
   hide/destroy/reparent pendant focus. **Verify :** une transition wx finale,
   jamais une transition par template part.
2. Introduire une identité de focus `(slot,generation,preferredElement)`; toute
   callback tardive devient no-op. `DoFindFocus()` retourne le contrôle wx, pas
   un child de template. **Verify :** 1 000 cycles re-template/reparent/destroy.
3. Faire de la navigation mixte un seul pipeline après accélérateurs du plan 004;
   XAML reçoit Tab uniquement si wx n’a pas avancé. **Verify :** aucune double
   avance, boucle ou caractère perdu dans TextCtrl/dialog.
4. Normaliser le curseur effectif par slot/ancestor; réassertion après resync,
   animation et capture. Gérer busy global imbriqué et custom fallback.
   **Verify :** restauration exacte après deux `BeginBusyCursor`/`EndBusyCursor`,
   slot détruit et passage XAML↔GDI.
5. Ajouter un scénario manuel reproductible RadioBox, dialog et control animé au
   spike; instrumentation désactivée par défaut.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets winuispike -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[KeyboardEvent]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[winui-focus]" --reporter compact
```

## Done

- [ ] focus logique et `DoFindFocus()` stables pour roots Control/non-Control ;
- [ ] Tab/Shift-Tab mixte sans boucle/double avance ;
- [ ] disable/hide/destroy/reparent restaurent une cible valide ;
- [ ] normal, busy imbriqué, custom et slot animé gardent le bon curseur ;
- [ ] aucun callback après génération ;
- [ ] tests keyboard/focus et harness verts.

## STOP

- Le fix du focus exige de changer l’ordre accélérateurs établi par 004.
- Une solution retient un template part au-delà d’un re-template.
- Le busy cursor global ne peut pas être concilié avec un cursor slot sans
  règle de priorité explicite et testée.
