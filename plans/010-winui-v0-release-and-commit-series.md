# Plan 010 — Qualifier et livrer la V0 alpha puis beta

> **Instructions** : ce plan autorise commits/push uniquement après une nouvelle
> validation explicite de l’utilisateur. Sans elle, produire le rapport de
> qualification et s’arrêter avant `git add`.
>
> **Drift check** : `git rev-parse HEAD; git status --short; git diff --stat; git diff --cached --stat`.
> Baseline auditée : HEAD `20207bdbbb`, 18 fichiers suivis modifiés, 6 fichiers
> indexés et nouveaux fichiers host/spike/docs. Toute différence doit être
> réinventoriée avant découpe.

## Statut

- **Priorité** : P1
- **Effort** : M
- **Risque** : HIGH livraison
- **Dépend de** : 001–006, 007a–007c, 008a–008f selon la matrice, puis 009
- **Catégorie** : release, tests, docs, git
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

La V0 ne peut pas être déclarée par ressenti sur quelques samples. Il faut une
matrice de support, un build clean-room, des gates automatisées et manuelles,
puis des commits révisables qui conservent la branche compilable.

## Scope

Qualification, documentation, instrumentation opt-in, découpe des changements
déjà validés. Hors scope : nouveau fix fonctionnel; si un gate échoue, revenir au
plan propriétaire et ne pas « réparer pendant la release ».

## Étapes

### 1. Geler la matrice de promesse

À partir de `plans/winui3-v0/component-matrix.md`, marquer chaque composant :
Supported Alpha, Supported Beta, Experimental, Generic/MSW fallback ou Excluded.
Lister chaque no-op/style/API non supporté.

Geler aussi la plateforme. Proposition conservatrice tant qu’aucun gate ne
prouve davantage : **Windows 11 x64, MSVC, Release, DLL shared, unpackaged** est
Supported Alpha; Windows 10 1809+, x86, ARM64, Debug, static et packaged restent
Experimental. Pour chaque dimension promue en beta, ajouter un build/runtime
gate propre; ne pas transformer « devrait marcher » en promesse.

**Vérifier** : aucun composant n’a deux classifications contradictoires entre
README, progress et docs; aucune page n’annonce Windows 10/x86/ARM64/static ou
packaged comme Supported sans ligne de matrice et preuve.

### 2. Build clean-room

```powershell
cmake -S . -B build-winui-release -G "Visual Studio 17 2022" -A x64 `
  -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=ALL `
  -DwxBUILD_SHARED=ON
cmake --build build-winui-release --config Release -- /m:4 /nr:false
```

Attendu : exit 0, XRC/InfoBar ON, aucune copie manuelle, aucun warning resource.
Ne pas utiliser `build-winui` comme preuve clean-room.

### 3. Suites automatisées

Exécuter tous filtres des composants promis et infrastructure. Produire un
tableau command/exit/failures/duration. Une exclusion doit correspondre à un
composant Excluded/Experimental et être documentée.

**Gate alpha** : zéro crash/freeze et filtres core alpha verts.
**Gate beta** : suite GUI ciblée intégrale verte pour les composants supportés.

Ajouter une ressource XRC mixte couvrant chaque famille `Supported` : création,
styles/propriétés principales, IDs/bindings puis destruction. Le gate exige non
seulement la compilation de `src/xrc`, mais le chargement réussi de cette
ressource dans `test_gui` sans île supplémentaire ni callback tardif.

### 4. Checklist manuelle reproductible

Sur widgets, showcase, minimal, dataview et dialogs, appliquer le niveau lié à
la classification figée : `Supported` = contrat complet ci-dessous,
`Experimental`/`Fallback` = smoke/no-crash + limites documentées, `Excluded` =
non-gate consigné sans prétendre au support. Pour les pages concernées :

- chaque page `Supported` : rendu, best size, enable/show,
  mouse/double/wheel/capture ;
- focus, Tab/Shift-Tab, mnemonics, accelerators ;
- tooltips/cursors/busy/custom ;
- scroll, splitter, notebook, z overlaps ;
- D&D texte/fichier ;
- dialogs Window/Overlay selon promesse, owner/nested/minimize/Alt-Tab ;
- Light/Dark/High Contrast à chaud ;
- DPI inter-écrans 100/150/200 et RTL ;
- UIA/Narrator ;
- multi-TLW reparent ;
- soak input/freeze au moins une heure.

Enregistrer OS/runtime/DPI et résultat par case; « semble OK » sans case n’est
pas une preuve.

### 5. Nettoyer l’outillage

Rendre ring 165 fps/histogrammes/logs opt-in, unifier collect-freeze, décider si
spike est supprimé ou promu en harness. Aucun probe permanent actif par défaut.

### 6. Mettre les docs à la vérité

Mettre à jour `WINUI_PORT_PROGRESS.md`, docs WinUI et matrice : ne plus annoncer
D&D/UIA/composant complet avant gate. Documenter fallbacks/exclusions et
packaging runtime.

### 7. Proposer la découpe à l’utilisateur

Sans staging, montrer `git diff --stat` et patch par slice :

1. build/runtime/packaging ;
2. InfoBar/API compile et tests ;
3. host TLW/slots/cutover ;
4. keyboard/menus ;
5. pointer/hit-test/freeze et instrumentation opt-in (007a) ;
6. focus/Tab/cursors (007b) ;
7. dialogs/frame-class/owner/refit ;
8. Z-order/scroll/splitter (007c) ;
9. refactor de session OLE sans changement wxMSW ;
10. broker D&D du bridge + tests `DragAcceptFiles` ;
11. component fixes, une famille verte par commit ;
12. adapters/gate final UIA/DPI/RTL ;
13. tests/docs/release matrix ;
14. suppression des probes jetables restants.

Chaque slice compile/teste seul. L’utilisateur approuve/ajuste avant `git add`.

### 8. Commits seulement après approbation

Créer une branche `codex/winui-v0-release` seulement si demandé. Stager par
paths/hunks vérifiés, afficher cached diff, compiler/tester, puis commit avec
message style historique `wxWinUI: ...`. Ne pas push/PR sans instruction.

## Done alpha

- [ ] matrice support/excluded publiée ;
- [ ] matrice OS/architecture/config/linkage/identity publiée ;
- [ ] build clean complet ;
- [ ] tests alpha et checklist manuelle verts ;
- [ ] aucun crash/freeze/ressource manquante ;
- [ ] lifetime/crash TextCtrl stable sur 100 runs ;
- [ ] D&D/menus/dialogues core verts ;
- [ ] instrumentation off par défaut ;
- [ ] docs reflètent limites ;
- [ ] découpe approuvée avant commits.

## Done beta

- [ ] suites composants supportés vertes ;
- [ ] XRC mixte charge tous les composants supportés ;
- [ ] UIA/Narrator, DPI/RTL/theme, multi-TLW ;
- [ ] perf/soak budgets ;
- [ ] fallbacks génériques qualifiés ;
- [ ] release notes et artefacts clean-room reproductibles.

## STOP

- Un gate fonctionnel échoue : retourner au plan correspondant.
- Un processus utilisateur verrouille un binaire : demander fermeture ou
  construire dans un nouveau dossier; ne pas tuer sans accord.
- Le worktree contient une modification non attribuable : préserver et demander.
- L’utilisateur n’a pas approuvé les commits/staging/push.
