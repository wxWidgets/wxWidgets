# Plan 005 — Implémenter le broker OLE drag-and-drop du bridge

> **Instructions** : le drag interne TreeCtrl n’est pas ce plan. Respecter la
> séquence COM Enter/Over/Leave/Drop, même lors d’un changement de cible. Aucun
> commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/winui/tlwhost.cpp include/wx/winui/private/tlwhost.h src/msw/window.cpp src/msw/ole/droptgt.cpp include/wx/msw/ole/droptgt.h include/wx/window.h`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`. Réconcilier toute dérive avant édition.

## Statut

- **Priorité** : P0 alpha
- **Effort** : L
- **Risque** : HIGH
- **Dépend de** : 003, 007a
- **Catégorie** : feature, compatibility, input
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

`wxWindowMSW::SetDropTarget()` enregistre la cible sur le shell HWND
(`src/msw/window.cpp:1778-1790`), mais le bridge top couvre le client. Aucun
`IDropTarget` n’est enregistré sur ce bridge : tous les drop targets sous l’île
sont fonctionnellement morts.

## Contraintes du code existant

- Le wrapper COM privé `wxIDropTarget` vit dans `src/msw/ole/droptgt.cpp:72-129`.
- `wxDropTarget::Register/Revoke` font CoLock et RegisterDragDrop
  (`:475-513`).
- DragEnter garde l’IDataObject; DragOver appelle `OnDragOver`; Drop configure la
  source puis `OnDrop/OnData` (`:207-445`).
- `wxWindow::GetDropTarget()` est public (`include/wx/window.h:1515`).
- ListCtrl/Grid peuvent rediriger leur drop target vers une subwindow; respecter
  leurs overrides.

## Design attendu

Une cible COM enregistrée par bridge, possédée par HostLifetime. Elle hit-teste
le `wxWindow` logique le plus profond à chaque mouvement. Quand la cible change :

```text
old.OnLeave / drag-image leave
resolve new target + coordinates
new DragEnter equivalent avec le même IDataObject/key/effects
new OnDragOver...
```

Le broker doit réutiliser/factoriser la logique de format/data source du wrapper
MSW; ne pas appeler seulement `OnDragOver()` puis inventer `GetData()`.

## Scope

Host/bridge, SetDropTarget integration et implémentation OLE MSW partagée, tests
D&D. Hors scope : redesign public wxDND, drag gesture TreeCtrl, shell file-drop
`DragAcceptFiles` sauf non-régression.

## Étapes

1. **Gate de faisabilité COM.** Sur un bridge réellement attaché à la source
   XAML, appeler `RegisterDragDrop(m_bridgeHwnd, probe)` et enregistrer le HRESULT,
   notamment `DRAGDROP_E_ALREADYREGISTERED`. Vérifier si le site XAML possède
   déjà la cible et si une révocation wx affecte l’île. Aucun design définitif
   avant ce résultat; le broker doit coopérer avec une cible existante plutôt
   que la remplacer aveuglément.
2. **Tests/probe rouge.** Deux targets (texte/fichier), une zone sans target,
   XAML slot et generic Grid/List; log Enter/Over/Leave/Drop/Data exact.
   Ajouter un child utilisant `DragAcceptFiles(true)` et consigner si le bridge
   détourne `WM_DROPFILES`.
3. **Refactor sans comportement.** Extraire de `wxIDropTarget` une session qui
   peut cibler un `wxDropTarget*` logique et un HWND/convertisseur de coordonnées,
   tout en conservant IDataObject, format negotiation et drag helper. Les tests
   wxMSW existants restent verts.
4. **Broker bridge.** RegisterDragDrop une fois à création du bridge; Revoke avant
   source.Close/subclass removal. Échec d’enregistrement fait échouer ou dégrade
   explicitement le host, jamais silencieusement.
5. **Intégration SetDropTarget.** Sous TLW hébergée, ne pas enregistrer chaque
   shell couvert; notifier/laisser le broker interroger `GetDropTarget()`. Hors
   host, conserver exactement le chemin MSW.
6. **`DragAcceptFiles`.** Si le probe démontre que `WM_DROPFILES` n’atteint plus
   le child, router/factoriser aussi ce contrat via le bridge, ou déclarer un
   bloqueur V0 explicite. Une simple mention de non-régression ne suffit pas.
7. **Hit-test/coordonnées.** Inclure slots XAML, subwindows génériques, scroll,
   RTL/DPI et fenêtres disabled/hidden; convertir écran→client de la cible
   logique.
8. **Transitions/lifetime.** Target détruit/reparenté pendant drag → Leave sûr,
   effect none ou nouvelle cible; migration host atomique; IDataObject libéré une
   fois à Leave/Drop/cancel.
9. **Drop images/effects.** Copy/move/link/none, Ctrl/Shift/default action et
   helper image utilisent la cible logique/bridge correctement.

## Tests

- texte et fichiers ; copy/move/link/none ;
- deux targets adjacentes, traversée répétée, sortie/rentrée ;
- child target override ListCtrl/Grid ;
- scroll pendant drag, DPI/RTL ;
- target disabled/hidden/destroyed/reparented ;
- deux TLW et passage A→B ;
- cancel Escape/source abort/app shutdown ;
- Tree internal drag ne compte pas comme succès de ce plan ;
- `DragAcceptFiles` reste fonctionnel; sinon le fix/routage fait partie du gate
  alpha ou la promesse V0 doit être explicitement renégociée.

## Done

- [ ] un RegisterDragDrop par bridge, un Revoke exact ;
- [ ] coexistence avec la cible XAML caractérisée, sans révocation étrangère ;
- [ ] séquences callback exactes et coordonnées client correctes ;
- [ ] data formats/OnData fonctionnent ;
- [ ] changement/destruction/reparent sans UAF ;
- [ ] generic et XAML targets couverts ;
- [ ] MSW hors host inchangé ;
- [ ] `DragAcceptFiles` child atteint la bonne fenêtre ;
- [ ] tests multi-TLW/DPI/RTL/cancel verts.

## STOP

- Le refactor de `wxIDropTarget` change l’ABI publique de wxDropTarget.
- Une cible doit être enregistrée simultanément sur shell et bridge pour un cas
  non représentable : arrêter et documenter le cas, ne pas double-enregistrer.
- Le bridge est déjà enregistré par XAML et aucune composition/coexistence sûre
  n’est démontrée : arrêter au spike et produire
  `docs/winui/adr/0002-ole-drop-integration.md` avec options, HRESULTs,
  ownership/revoke et conséquences.
- Les callbacks target sont exécutés hors thread UI.
