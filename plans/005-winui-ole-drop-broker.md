# Plan 005 — Implémenter le broker OLE drag-and-drop du TLW

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
- **Exécution** : IN REVIEW depuis le 2026-07-24 — RC4 a invalidé le broker
  TLW-only. RC7 a ensuite invalidé l'acquisition pendant la construction
  cachée : malgré un ledger bridge+TLW « current », le drag produit n'a reçu
  aucun `DragEnter`. La première acquisition est désormais différée au premier
  flush visible et utilise deux adaptateurs COM physiques partageant une seule
  session. Les gates déterministes shared/static sont vertes ; un nouveau
  candidat physique doit être gelé.

### Gates de faisabilité et de topologie exécutées

Le probe isolé initial `wx_winui_runtime_smoke --ole-drop-probe` crée une vraie
`DesktopWindowXamlSource`, réalise les templates avec `AllowDrop=true`,
initialise OLE en STA puis caractérise séparément le
`DesktopChildSiteBridge` et son descendant `InputSite`. Il prouve la capacité
d'enregistrement de ces HWND, pas le choix du récepteur couvrant tout le TLW.

Résultat initial local (Windows App SDK runtime 2.1.3, x64) : les HWND sondées ont retourné
`S_OK` à `CoLockObjectExternal`, `RegisterDragDrop`, `RevokeDragDrop` et au
déverrouillage; aucune inscription étrangère n'a donc été révoquée, et les
contrôles XAML sont restés chargés/mesurés. Le premier STOP
`DRAGDROP_E_ALREADYREGISTERED` n'est pas rencontré sur ce profil.

Le micro-probe physique `--ole-drop-delivery-probe`, opt-in, borné et
fail-closed, a ensuite passé sa matrice brute le 2026-08-21 avec garde d'entrée
propre et `EXITCODE=0`. Sur la surface XAML normale, bridge et TLW reçoivent
séparément; lorsque les deux sont armés, le bridge reçoit. Dans un trou du
bridge exposant une vraie HWND enfant, le bridge manque proprement et seul le
TLW reçoit; l'armement combiné sélectionne exactement le TLW. La synthèse
`receiver-qualified` a d'abord recommandé `tlw`. La campagne produit RC4 a
ensuite montré que cette recommandation TLW-only ne se transpose pas au broker
produit sur cette machine : l'armement combiné du même artefact livre au bridge
en surface normale et au TLW dans le trou.

Cette preuve sélectionne le **HWND d'enregistrement physique**; elle ne qualifie
pas à elle seule le `wxWinUIDropBroker` produit, son routage vers les
`wxDropTarget` ni un artefact RC7. Les deux linkages et le routage produit sont
désormais couverts par les gates déterministes détaillées plus bas. Tout
`DRAGDROP_E_ALREADYREGISTERED` futur est traité comme
`BLOCKED/INCONCLUSIVE`, jamais comme un succès et jamais suivi d'un
`RevokeDragDrop` d'une registration non possédée.

## Pourquoi

`wxWindowMSW::SetDropTarget()` enregistre normalement chaque cible sur son shell
HWND. Dans un host WinUI, le bridge top reçoit la surface XAML normale, tandis
que les trous de région nécessaires aux enfants Win32 exposent d'autres HWND.
Une registration sur le bridge seul perd donc les drops dans ces trous, tandis
que RC4 a prouvé qu'une registration produit TLW-only peut manquer la surface
normale. Des registrations par contrôle créeraient plusieurs sessions physiques
concurrentes. La solution retenue est une seule session logique derrière deux
adaptateurs COM physiques distincts, chacun inscrit sur sa surface du host.

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

Deux adaptateurs COM possédés par `HostLifetime` et partageant le même état sont
enregistrés sur le bridge puis sur le TLW. Le bridge reçoit la surface XAML
normale et fournit le routage logique/hit-test vers le `wxWindow` le plus
profond ; le TLW couvre les trous natifs. Les deux callbacks alimentent la même
session logique.
Quand la cible logique change :

```text
old.OnLeave / drag-image leave
resolve new target + coordinates
new DragEnter equivalent avec le même IDataObject/key/effects
new OnDragOver...
```

Le broker doit réutiliser/factoriser la logique de format/data source du wrapper
MSW; ne pas appeler seulement `OnDragOver()` puis inventer `GetData()`.

### Contrat produit implémenté

- Le broker réserve les deux couples exacts `(HWND bridge, génération)` et
  `(HWND TLW, génération)`, puis enregistre l'adaptateur physique de chaque
  rôle bridge→TLW. `Ready` exige les deux ; rollback et shutdown nettoient
  TLW→bridge.
- Aucune acquisition n'est faite sur un TLW caché. Les cibles wxMSW restent
  inscrites jusqu'au premier `FlushSync()` visible, qui les adopte puis
  acquiert la paire. Aucun rebind/revoke spéculatif n'est effectué lors des
  `Hide()`/`Show()` suivants.
- Un registre global commun au chemin fixe wxMSW et au broker externe
  sérialise l'ownership. Une réservation ou un verrou dont la libération n'est
  pas prouvée reste retenu fail-closed avec son identité COM.
- Une acquisition bloquée par une opération fixe devient `PendingFixed`. Sa
  reprise est one-shot, liée à l'identité native et au thread UI ; un timer
  one-shot exact assure la progression lorsque `PostMessage()` échoue.
- La publication XAML du host et l'enregistrement de ses slots sont
  indépendants du résultat OLE. Un contenu accepté n'est pas perdu pendant une
  acquisition différée et le host peut continuer à fonctionner si OLE échoue
  fermé.

## Scope

Host/TLW/bridge, intégration `SetDropTarget`, implémentation OLE MSW partagée et
tests D&D. Hors scope : redesign public wxDND et drag gesture TreeCtrl. Le shell
file-drop `DragAcceptFiles`/`WM_DROPFILES` reste un contrat distinct reçu et
routé par le bridge; sa non-régression ne vaut jamais preuve de livraison OLE.

## Étapes

1. **Gate de faisabilité COM et de topologie.** Caractériser bridge, `InputSite`
   et TLW sans révoquer de cible étrangère, puis exécuter la matrice physique sur
   la surface normale et dans un trou natif. RC4 impose finalement le bridge
   pour la surface normale et le TLW pour le trou, via un broker unique.
2. **Tests/probe rouge.** Deux targets (texte/fichier), une zone sans target,
   XAML slot et generic Grid/List; log Enter/Over/Leave/Drop/Data exact.
   Ajouter un child utilisant `DragAcceptFiles(true)` et vérifier séparément que
   le bridge lui route `WM_DROPFILES`.
3. **Refactor sans comportement.** Extraire de `wxIDropTarget` une session qui
   peut cibler un `wxDropTarget*` logique et un HWND/convertisseur de coordonnées,
   tout en conservant IDataObject, format negotiation et drag helper. Les tests
   wxMSW existants restent verts.
4. **Broker bridge+TLW.** Après la première présentation/geometry flush,
   acquérir transactionnellement l'éventuelle cible wx du TLW, réserver les
   deux identités exactes, appeler `RegisterDragDrop()` avec deux adaptateurs
   distincts sur le bridge puis le TLW et nettoyer dans l'ordre inverse. Tout
   échec restaure les registrations détachées et fait échouer ou dégrade
   explicitement le host, jamais silencieusement.
5. **Intégration SetDropTarget.** Sous TLW hébergée, ne pas enregistrer chaque
   shell couvert; notifier/laisser le broker interroger `GetDropTarget()`. Hors
   host, conserver exactement le chemin MSW.
6. **`DragAcceptFiles`.** Conserver `WM_DROPFILES` séparément sur le bridge et le
   router vers le child logique. Tester ce chemin indépendamment : il ne passe
   ni par `RegisterDragDrop()` ni par la session COM OLE.
7. **Hit-test/coordonnées.** Inclure slots XAML, subwindows génériques, scroll,
   RTL/DPI et fenêtres disabled/hidden; convertir écran→client de la cible
   logique.
8. **Transitions/lifetime.** Target détruit/reparenté pendant drag → Leave sûr,
   effect none ou nouvelle cible; migration host atomique; IDataObject libéré une
   fois à Leave/Drop/cancel.
9. **Drop images/effects.** Copy/move/link/none, Ctrl/Shift/default action et
   helper image utilisent la cible logique et le TLW enregistré correctement.

## Tests

- texte et fichiers ; copy/move/link/none ;
- deux targets adjacentes, traversée répétée, sortie/rentrée ;
- child target override ListCtrl/Grid ;
- scroll pendant drag, DPI/RTL ;
- target disabled/hidden/destroyed/reparented ;
- deux TLW et passage A→B ;
- surface XAML normale et trou exposant une HWND native, avec un seul récepteur ;
- cancel Escape/source abort/app shutdown ;
- Tree internal drag ne compte pas comme succès de ce plan ;
- `DragAcceptFiles` reste fonctionnel; sinon le fix/routage fait partie du gate
  alpha ou la promesse V0 doit être explicitement renégociée.

## Done

- [x] diagnostic physique brut : surface normale + trou natif, recommandation
  `tlw`, garde d'entrée propre et exit 0 ;
- [x] broker produit : une session logique, deux identités COM physiques et
  deux `RegisterDragDrop` exacts bridge+TLW, avec rollback/revoke inverse par
  identité/génération ;
- [x] acquisition interdite pendant toute construction, TLW caché ou déjà
  visible ; première acquisition après flush/epoch de présentation stable,
  retry événementiel et rollback exact d'un Hide/Show réentrant avant commit,
  puis stabilité sans register/revoke supplémentaire à travers Hide/Show ;
- [x] coexistence avec la cible XAML caractérisée, sans révocation étrangère ;
- [x] séquences callback exactes et coordonnées client correctes en tests COM
  déterministes ;
- [x] data formats/OnData fonctionnent ;
- [x] changement/destruction/reparent sans UAF ;
- [x] generic et XAML targets couverts ;
- [x] MSW hors host inchangé, y compris compilation `wxUSE_DRAG_AND_DROP=OFF` ;
- [x] `DragAcceptFiles` child atteint la bonne fenêtre ;
- [x] tests multi-TLW/RTL/cancel verts ;
- [x] gates déterministes du broker produit :
  `[winui-drop-broker]~[physical]` **31 cas / 1 330 assertions** dans chaque
  linkage,
  `[msw-drop-session]` **11/218 shared** et **11/202 static/no-exceptions**,
  initialisation différée du host **6/59** ;
- [x] non-régressions transversales shared et static/no-exceptions : Supported
  V0 **50/927** et HostLifecycle/HostState/broker **190/25 675** ;
- [ ] livraison physique du broker produit sur le prochain candidat gelé, puis DPI
  per-monitor/accessibilité/thèmes/High Contrast/RTL et soak signé de 60
  minutes ; le PASS du diagnostic brut ne coche pas ces gates.

## STOP

- Le refactor de `wxIDropTarget` change l’ABI publique de wxDropTarget.
- Une cible wx par contrôle devrait rester enregistrée simultanément sous la
  paire du broker : arrêter et documenter le cas. La double inscription
  bridge+TLW de l'unique broker est, elle, le contrat décidé par l'ADR 0002.
- Le TLW possède une registration étrangère dont l'ownership ne peut pas être
  prouvé : arrêter au spike sans la révoquer et produire
  `docs/winui/adr/0002-ole-drop-integration.md` avec options, HRESULTs,
  ownership/revoke et conséquences.
- Les callbacks target sont exécutés hors thread UI.
