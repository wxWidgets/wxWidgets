# Fiche 00 — Fondation, host TLW, slots et contrôles custom

> Baseline : `20207bdbbb81055da1f3e980b3271fc24566990e`, 2026-07-21,
> avec refonte host non commitée. Avant exécution :
> `git diff --stat 20207bdbbb..HEAD -- src/winui/ctrlhost.cpp src/winui/tlwhost.cpp src/winui/xamlhost.cpp include/wx/winui/private/tlwhost.h src/msw/window.cpp src/msw/evtloop.cpp`.
> Si les symboles cités ont changé, arrêter et réconcilier cette fiche.

## Objectif

Obtenir une infrastructure où tout contrôle XAML d’une TLW partage la même île,
où les slots sont détruits sans callback tardif, et où un `wxWindow` générique
coexiste avec XAML sans perdre enabled, focus, tooltip, D&D, UIA ou Z-order.

## État actuel

| Élément | Statut | Preuve |
|---|---|---|
| host unique par TLW | présent, WIP | `src/winui/tlwhost.cpp`, registre/bridge/root |
| proxy `wxWinUIControlHost` | présent | `src/winui/ctrlhost.cpp:320+` |
| slot géométrie/clip/RTL/visible | présent | `tlwhost.cpp:1871-2069` |
| migration inter-TLW | présente, non stressée | `tlwhost.cpp:1899+` |
| input island-first | présent | `tlwhost.cpp:1260-1715` |
| focus arbiter | partiel | `tlwhost.cpp:714-730,1717+` |
| enabled commun | absent | aucun `IsEnabled` à `tlwhost.cpp:2030-2069` |
| tooltip commun | absent | `src/msw/window.cpp:1812-1828` |
| revocation handlers | absente | `tlwhost.cpp:878-1115,581-608` |
| D&D OLE | absent | aucune cible COM dans `src/winui` |
| `wxWinUIXamlHost` public | incompatible | crée sa source à `xamlhost.cpp:128` |

## Défauts à fermer

### Alpha P0

1. Stocker/revoquer tous les handlers `AddHandler`, focus et template; ne jamais
   capturer un `wxWindow*` brut sans generation/weak identity.
2. Ajouter au slot `enabled`, hit-test, focus target, tooltip, cursor, UIA et
   DPI generation. Synchroniser l’état effectif des parents.
3. Migrer `wxWinUIXamlHost` vers `RegisterSlot()`; fermer sans `ClearContent()`
   préalable, conformément à `tlwhost.h:129-132`.
4. Contrôler l’échec de `SetWindowsHookExW()` et fournir un fallback explicite.
5. Libérer `HRGN` si `SetWindowRgn()` échoue.
6. Rendre le priming backdrop generation-safe; ne pas capturer HWND/taille bruts.

### Alpha/Beta P1

7. Représenter l’ordre relatif réel des slots; détecter les topologies HWND/XAML
   impossibles plutôt que mentir sur `Raise/Lower`.
8. Ne pas reposter un flush à chaque tour pendant `Freeze()`; marquer dirty et
   armer une fois au Thaw.
9. Corriger hit-test `HTTRANSPARENT` vers siblings sous-jacents, non-client leave,
   XBUTTON et politique touch/pen.
10. Coalescer Got/Lost focus d’éléments internes vers un seul changement de slot.
11. Actualiser AutomationProperties après label/name/state change et masquer les
    shells HWND dupliqués.

## Plan détaillé

1. **Ajouter des tests de caractérisation avant extraction.** Créer une suite
   interne dans `tests/controls/` ou `tests/window/` pour register/unregister,
   destroy pendant callback, disable parent/child, reparent A↔B et focus target
   non-Control. Vérifier d’abord qu’elle reproduit au moins un défaut.
2. **Introduire `wxWinUISlot::Disconnect()`.** Le slot stocke delegates/tokens,
   met sa generation invalide, révoque, détache content/container, puis efface
   les pointeurs wx. `UnregisterSlot()` appelle cette transaction avant de
   modifier les maps.
3. **Introduire un state adapter.** Bind enable/tooltip/label/DPI ou déclencher
   une synchronisation depuis les points communs `wxWindowMSW`. Appliquer les
   propriétés au root et à une `preferredFocusTarget` optionnelle.
4. **Convertir `wxWinUIXamlHost`.** Conserver l’API publique, remplacer `source`
   par un proxy/slot. Ajouter un test qui mélange ce host, un Button natif et un
   Grid générique dans la même TLW et vérifie une seule source.
5. **Durcir les API Win32.** Vérifier retours du hook, `SetWindowPos`,
   `SetWindowRgn`, `SetWindowSubclass`; journaliser une erreur unique et annuler
   proprement la transaction.
6. **Séparer les sous-responsabilités.** Après tests verts seulement, extraire
   lifetime/geometry/input/focus dans des fichiers privés sans changer l’API.

## Tests requis

```powershell
cmake --build build-winui-audit --config Release --target test_gui -- /m:4 /nr:false
build-winui-audit\lib\vc_x64_dll\test_gui.exe "[window]" --reporter compact
build-winui-audit\lib\vc_x64_dll\test_gui.exe "Button::Click" --reporter compact
```

Ajouter un harnais `HostLifecycle` couvrant :

- 10 000 cycles create/register/unregister/destroy; compteur weak finalisé ;
- callback/enqueue en vol au destroy et au reparent ;
- parent disabled, child disabled, réactivation, UIA Invoke refusé ;
- focus target Grid+RadioButton ;
- deux frames, déplacement d’un slot A→B→A ;
- `Raise/Lower` avec overlap XAML/XAML et XAML/GDI ;
- Freeze une seconde en pompant les events, nombre de flush borné ;
- DPI/RTL/theme avant et après reparent.

## Critères alpha

- une seule source XAML par TLW, y compris `wxWinUIXamlHost` ;
- aucun delegate/token vivant après unregister ;
- aucun slot interactif quand lui ou un parent est disabled ;
- clic/focus/Tab de base verts dans `test_gui` ;
- aucune fuite GDI/COM détectée sur le stress lifecycle ;
- hook failure et dispatcher shutdown ont un chemin déterministe.

## Frontières

- Ne pas ajouter D&D dans cette fiche : le plan 005 lui est dédié.
- Ne pas multiplier les bridges pour résoudre Z-order.
- Ne pas refactorer les 2 458 lignes avant tests de caractérisation.
- Arrêter si la conversion de `wxWinUIXamlHost` exige de casser son ABI public;
  proposer alors une transition documentée séparée.
