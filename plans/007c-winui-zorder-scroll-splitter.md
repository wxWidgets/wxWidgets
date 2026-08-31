# Plan 007c — Qualifier Z-order, clips, latence scroll et splitter

> **Instructions** : mesurer avant de choisir sync flush ou miroirs. Aucun
> nouveau bridge/île sans ADR et validation utilisateur.
>
> **Drift check** : `$scope=@('include/wx/winui/private/tlwhost.h','src/winui/tlwhost.cpp','src/winui/ctrlhost.cpp','src/winui/renderer.cpp','src/generic/splitter.cpp','tests/controls/windowtest.cpp','samples/winuispike','docs/winui'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha gate
- **Effort** : M/L
- **Risque** : HIGH composition
- **Dépend de** : 003, 007a

## État et scope

`SyncSlot()` (`tlwhost.cpp:1956+`) positionne/clippe les slots,
`RecomputeZOrder()` (`:2076+`) utilise encore un ordre de parcours,
`RebuildBridgeRegion()` (`:2169+`) découpe le bridge, et `ScheduleFlush()`/
`FlushSync()` (`:1862+`) gouvernent la latence. Le renderer laisse désormais le
sash du splitter non peint et suppose un bon fond `WM_ERASEBKGND`.

Scope : ordre XAML/XAML, détection des overlaps HWND/XAML non représentables,
clip ancestors/TabView/scrollbars, mesure scroll et splitter. Hors scope : input
events (007a), D&D (005), refonte d’un composant Notebook (008f).

## Étapes et vérifications

1. Ajouter un scénario stable au spike : trois siblings XAML/GDI avec
   `Raise()`/`Lower()`, overlap, scroll et splitter horizontal/vertical.
   **Verify :** captures + signature d’ordre avant toute modification.
2. Recalculer Z depuis le vrai sibling order à chaque changement pertinent;
   garder un plan XAML unique. Détecter en debug une intercalation physiquement
   impossible. **Verify :** XAML/XAML suit Raise/Lower; topologie impossible
   produit diagnostic/limite, pas faux rendu silencieux.
3. Tester toutes les sources de clip : ancestors, scroll viewport, TabView band,
   hidden/minimized et DPI. Nettoyer HRGN sur chaque échec `SetWindowRgn`.
   **Verify :** aucune entrée/peinture hors clip et aucun leak GDI.
4. Instrumenter timestamp mutation HWND → slot présenté à 60/120/165 Hz, flag
   opt-in. Fixer un budget chiffré avec l’utilisateur.
5. Rédiger `docs/winui/adr/0003-scroll-sync-and-zorder.md` : mesures, option
   coalescée, flush synchrone ciblé, miroirs, décision et conséquences. N’adopter
   les miroirs que si les mesures l’exigent.
6. Valider splitter Light/Dark/High Contrast, live/tracker, double-click,
   min/gravity, cursor/capture et DPI. **Verify :** aucune bande blanche ni slot
   traversant le sash.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target widgets winuispike test_gui -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[winui-zorder]" --reporter compact
```

## Done

- [ ] ordre relatif XAML correct; topologies impossibles détectées/documentées ;
- [ ] clips scroll/TabView/ancestor corrects et aucun HRGN perdu ;
- [ ] budget scroll mesuré, accepté et ADR écrite ;
- [ ] splitter sans trait parasite dans trois thèmes/trois DPI ;
- [ ] instrumentation off par défaut et harness reproductible.

## STOP

- Le seul moyen de reproduire l’intercalation est plusieurs bridges/îles.
- Aucune mesure ne démontre le besoin de miroirs : conserver la solution simple.
- Le splitter correct dépend d’une couleur codée en dur ou d’un thème unique.
