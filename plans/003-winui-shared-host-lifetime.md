# Plan 003 — Durcir l’hôte partagé, les slots et `wxWinUIXamlHost`

> **Instructions** : ajouter les tests de caractérisation avant de scinder le
> code. Aucun commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- include/wx/winui/private/tlwhost.h src/winui/tlwhost.cpp src/winui/ctrlhost.cpp src/winui/xamlhost.cpp src/msw/window.cpp src/winui/winui.cpp`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`.
> Empreintes worktree : `tlwhost.h`
> `09a18b18a7d48b5008f6713481bcb6c5377abfdd6cc87480c80875545850cda5`,
> `tlwhost.cpp`
> `a259eb75773e1c8bdd5d402680488898ce05d6b6dcd656d97e8346a1f6abfba5`,
> `ctrlhost.cpp`
> `83913e1c0170fc6e2deb5f28f55d81f1e6ab97f52831d5858034b7feb19a1fbb`,
> `xamlhost.cpp`
> `b17aadfe384df942f9beb85a3477649bf02507b572c2d9833b06248aafdfaccf`.
> Recalculer les SHA-256 et arrêter si non concordants.

## Statut

- **Priorité** : P0
- **Effort** : L
- **Risque** : HIGH
- **Dépend de** : 001, 002
- **Catégorie** : architecture, bug, memory, tests
- **Planifié à** : `20207bdbbb`, 2026-07-21
- **État d'exécution (2026-07-30)** : **DONE** — l’ancien blocage
  `CodexSandboxOffline` est supersédé. Runtime/self-tests 6/6, HostState,
  HostLifecycle, transactions shell, focus et reparent sont verts dans le
  runner desktop isolé.

## Pourquoi

Le host partagé est la fondation de tous les correctifs ultérieurs. Aujourd’hui,
les handlers routés ne sont pas révoqués, enabled/tooltips/UIA ne sont pas des
invariants et le host public crée encore sa propre île. Ajouter D&D/dialogues
sur cette base augmenterait les fuites et UAF potentielles.

## État courant essentiel

- `BindSlotEvents()` ajoute handlers capturant `this/window/container`
  (`tlwhost.cpp:878-1115`).
- `wxWinUISlot` ne stocke aucun delegate/token (`tlwhost.h:84-107`).
- `UnregisterSlot()` retire le Grid et delete le slot sans RemoveHandler
  (`tlwhost.cpp:581-608`).
- Flush copie géométrie/RTL/visibility mais pas enabled (`:2030-2069`).
- tooltip natif court-circuité pour hosts (`src/msw/window.cpp:1812-1828`).
- `wxWinUIXamlHost` crée une `DesktopWindowXamlSource` (`xamlhost.cpp:128-152`)
  et ClearContent avant Close (`:77-90`).
- le retour du hook n’est pas contrôlé (`src/winui/winui.cpp:474-485`).

## Scope

Headers/sources host/ctrlhost/xamlhost, points communs window/winui nécessaires,
tests lifecycle/focus/state. Hors scope : D&D, menu ordering, dialogs et contrats
spécifiques d’un composant.

## Étapes

1. **Tests lifecycle rouges.** Register/unregister/destroy/reparent en boucle,
   callback/enqueue en vol, weak finalization et compteurs de delegates.
2. **Transaction Disconnect.** Generation invalide → revoke tokens/delegates →
   détacher content → retirer maps/root → libérer. Toutes les sorties d’échec de
   Register suivent le même rollback.
3. **Captures faibles.** Aucun raw wxWindow/host dans callback pouvant survivre
   au slot. Utiliser sender pour coordonnées lorsque possible et weak/generation
   pour retourner vers wx.
4. **Slot state adapter.** Synchroniser effective enabled, hit-test, focusability,
   tooltip, cursor, automation name/state et preferred focus target. Bind parent
   state sans N callbacks redondants si possible.
5. **Convertir `wxWinUIXamlHost`.** API inchangée, content devient slot; test une
   seule source par TLW. Close respecte ordre invariant.
6. **API Win32 checked.** Contrôler et exposer/loguer l’état d’installation du
   hook, subclass, SetWindowPos et SetWindowRgn; détruire HRGN en cas d’échec.
   Ce plan ne route aucune touche et n’implémente pas le fallback sémantique du
   hook clavier : ce propriétaire unique est le plan 004.
7. **Freeze scheduling.** Une dirty generation en freeze, un seul schedule au
   Thaw, pas CallAfter infini.
8. **Extraction prudente.** Une fois tests verts, extraire Lifetime/SlotState de
   `tlwhost.cpp`; aucune modification fonctionnelle simultanée.
   (Décision utilisateur/reviewer du 2026-07-22 — étape satisfaite : Lifetime
   et SlotState résident déjà dans le header privé `tlwhost.h`; les points
   d'entrée MSW sans projection WinRT sont isolés dans `tlwhostmsw.h`. Une
   extraction supplémentaire n'apporterait pas de frontière architecturale et
   augmenterait le churn.)

## Vérification

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets minimal -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[window]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "Button::Click" --reporter compact
```

Exécuter `Button::Click` comme témoin de non-régression. Jusqu'au plan 007a,
le résultat attendu est l'échec connu `clicked.GetCount() == 0`, sans crash
et sans nouvelle assertion. Tout autre échec est une régression du plan 003.
(Décision utilisateur/reviewer du 2026-07-22 : la remise au vert appartient
au plan 007a, propriétaire du pipeline pointer/hit-test/SendInput.)

Ajouter `HostLifecycle` et exécuter 100 fois. Attendu : zéro crash, zéro weak
vivant, bounded flush et une source/TLW.

## Done

- [x] tous handlers révoqués avant slot free ;
- [x] no callback post-generation ;
- [x] enabled parent/child bloque pointer/keyboard/UIA ;
- [x] tooltips communs et GetToolTip cohérents ;
- [x] preferred focus target couvre RadioBox ;
- [x] `wxWinUIXamlHost` partage la source ;
- [x] hook/API failures déterministes ;
- [x] freeze n’occupe pas la boucle ;
- [ ] tests HostLifecycle, HostState et Window rejoués après la dernière
  passe ; signature connue de
  `Button::Click` inchangée (`clicked.GetCount() == 0`, sans crash) et
  propriété explicitement transférée au plan 007a. Les 41 cas
  HostLifecycle/HostState compilent, mais l'environnement Codex courant
  échoue au bootstrap WinAppSDK avant la création du premier host.

## STOP

- Conversion `wxWinUIXamlHost` requiert rupture ABI publique.
- Un callback WinRT ne peut pas être révoqué selon l’API utilisée : isoler le
  lifetime du state dans un objet ref-counted et faire valider avant de continuer.
- Le state adapter exige de toucher chaque composant : arrêter et revoir le seam
  commun, ne pas lancer 30 overrides.
