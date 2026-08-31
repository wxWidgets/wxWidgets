# Plans d’implémentation — wxWinUI 3 V0

Générés le 2026-07-21 à partir du commit `20207bdbbb81055da1f3e980b3271fc24566990e`
et d’un arbre de travail volontairement sale. Exécuter dans l’ordre ci-dessous,
sauf dépendances indiquées. Chaque exécutant doit lire son plan en entier,
respecter les conditions `STOP` et mettre à jour sa ligne.

> Règle de la session : ne créer aucun commit, push ou PR sans validation
> explicite de l’utilisateur. Les modifications présentes avant cet audit
> appartiennent à l’utilisateur et ne doivent pas être écrasées.

> **État de promotion au 2026-08-21.** Le périmètre étroit Supported V0 est un
> candidat bêta : ses gates automatisées shared et static/no-exceptions sont
> vertes (Supported V0 **50/927** et HostLifecycle/HostState **161/24 850**
> dans chaque linkage). Le probe OLE physique brut est également vert
> (`receiver-qualified`, recommandation `tlw`, exit 0). La promotion
> production-ready attend encore la campagne physique produit du RC7 gelé, les
> validations manuelles accessibilité/DPI/thèmes/HC/RTL et le soak signé de 60
> minutes. Ce statut ne couvre pas la parité wxMSW globale.

## Ordre d’exécution et statut

| Plan | Titre | Priorité | Effort | Dépend de | Statut |
|---|---|---:|---:|---|---|
| [001](001-winui-build-runtime-baseline.md) | Rendre le build frais et le runtime reproductibles | P0 | M | — | DONE — WinAppSDK stable 1.8 + PRI MakePri validés |
| [002](002-winui-public-api-and-test-gate.md) | Restaurer la compilation API publique et la porte de tests | P0 | M | 001 | DONE |
| [003](003-winui-shared-host-lifetime.md) | Durcir l’hôte partagé, ses slots et `wxWinUIXamlHost` | P0 | L | 001–002 | DONE |
| [004](004-winui-keyboard-and-menus.md) | Restaurer accélérateurs et contrats des menus | P0 | M | 003 | IN REVIEW — code et gates automatisées verts ; blocage sandbox historique levé, rejeu physique final sur RC3 requis |
| [007a](007a-winui-pointer-hit-test-freeze.md) | Pointer, hit-test, capture et storms | P0 | L | 003 | IN PROGRESS — audit terminé, instrumentation opt-in en validation |
| [005](005-winui-ole-drop-broker.md) | Implémenter le broker OLE drag-and-drop du TLW | P0 | L | 003, 007a | IN REVIEW — paire bridge+TLW atomique et gates shared/static vertes ; RC4 TLW-only invalidé, gate produit RC7 gelée restante |
| [007b](007b-winui-focus-tab-cursors.md) | Focus logique, Tab mixte et curseurs | P0 | M/L | 003, 004, 007a | DONE |
| [006](006-winui-dialogs-and-transients.md) | Stabiliser dialogues, transitoires et re-fit initial | P0 | L | 003, 004, 007b | DONE |
| [007c](007c-winui-zorder-scroll-splitter.md) | Z-order, clips, scroll et splitter | P1 | M/L | 003, 007a | IN REVIEW — code/tests/revues/165 Hz verts, gates physiques thème/DPI restantes |
| [007](007-winui-input-focus-freeze-zorder.md) | Epic input/focus/freeze/Z-order | P1 | XL | 007a–007c | EPIC |
| [008a](008a-winui-text-peer.md) | TextCtrl et SearchCtrl | P0 | L | 003, 004, 007b | IN REVIEW — implémentation et gates déterministes vertes ; client UIA externe/IME physique dans 009 |
| [008b](008b-winui-item-model.md) | Choice/Combo/List/CheckList/BitmapCombo | P1 | L | 007a, 008a | DONE |
| [008c](008c-winui-range-spin-date.md) | Range/Spin/Date/Time/Calendar | P1 | L | 003, 007a, 007b | IN REVIEW — implémentation et gates déterministes vertes ; qualification physique 009 restante |
| [008d](008d-winui-actions-appearance.md) | Actions, sélection et apparence | P1 | L | 002, 003, 006, 007b | IN REVIEW — implémentation, réentrance et apparence déterministes vertes ; qualification physique 009 restante |
| [008e](008e-winui-notebook-tree-books.md) | Notebook, TreeCtrl et books | P1 | L | 005, 007a–007c, 008a–008b | DONE |
| [008f](008f-winui-chrome-feedback.md) | Chrome, feedback, tooltips et NSDMI | P1 | L | 002–004, 006, 007a–007c, 008d–008e | IN REVIEW — implémentation/revues/gates déterministes vertes ; validation physique 009 restante |
| [008](008-winui-component-contracts.md) | Epic contrats composants V0 | P1 | XL | 008a–008f | EPIC |
| [009](009-winui-uia-dpi-rtl-multitlw.md) | Valider UIA, DPI, RTL et multi-TLW | P1 | L | sous-plans 003–008 promis | IN REVIEW — implémentation/revues/gates déterministes vertes ; Accessibility Insights/Narrator/UIA externe, multi-écrans et validations physiques restantes |
| [010](010-winui-v0-release-and-commit-series.md) | Qualifier et livrer la V0 alpha puis beta | P1 | M | 001–006, 007a–c, 008a–f, 009 | IN PROGRESS — candidat bêta Supported V0 automatisé vert ; gate physique produit RC7, validations humaines 009 et soak 60 min restants |
| [011](011-winui-namespace-adr.md) | Décider la stratégie `wxWinUI::` / `wxMSW::` | P3 | M | 010 | DONE — ADR 0004 : API wx globale, toolkits séparés |

Valeurs de statut : `TODO`, `IN PROGRESS`, `DONE`, `BLOCKED: raison`,
`REJECTED: raison`; `EPIC` désigne un index non exécutable directement.

## Dépendances et chemin critique

- 001 et 002 transforment un build local historique en baseline reproductible.
  Sans eux, un succès dans `build-winui` peut dépendre de ressources PRI copiées
  manuellement et masque la rupture publique de `wxInfoBar`.
- 003 fixe les invariants communs avant d’ajouter D&D, menus ou dialogues : ces
  fonctions dépendent toutes du même bridge et des mêmes règles de durée de vie.
- 007a stabilise la résolution d’entrée avant le broker D&D 005; 004 puis 007b
  stabilisent clavier/focus avant les dialogues 006. 007c ferme Z/scroll/splitter.
- 007 et 008 sont seulement des epics. Exécuter directement 008a–008f, qui
  s’appuient sur les fiches détaillées de `plans/winui3-v0/components/`.
- 009 est la porte transversale UIA/DPI/RTL/multi-TLW avant release.
- 010 ne doit pas être commencé tant que les critères alpha de la matrice ne sont
  pas verts. 011 est une décision d’architecture post-V0, pas un renommage de fin
  de chantier.

## Dossier d’audit et fiches composants

- [Synthèse et verdict](winui3-v0/README.md)
- [Audit de la conversation Claude](winui3-v0/audit-conversation.md)
- [Audit des 61 commits](winui3-v0/audit-commits.md)
- [Audit du code courant](winui3-v0/audit-current-code.md)
- [Validation de la liste des 14 restes](winui3-v0/validation-liste-restes.md)
- [Architecture proposée](winui3-v0/architecture.md)
- [Matrice des 43 sources WinUI et des composants/fallbacks](winui3-v0/component-matrix.md)
- [Index des fiches composants](winui3-v0/components/README.md)
- [Preuves de build et tests](winui3-v0/verification.md)

## Constatations examinées et reclassées

- « Construire une table d’accélérateurs dans `menubar.cpp` » : rejeté comme
  diagnostic. La table existe déjà dans `src/msw/menu.cpp`; l’ordre de
  prétraduction face au hook XAML est fautif. Voir plan 004.
- « Tous les scalaires des headers WinUI sont des bugs actifs » : reclassé en
  durcissement préventif. Le crash confirmé de `wxTextEntryDialog` est corrigé
  dans le worktree; les constructeurs complets des autres classes initialisent
  normalement leurs membres.
- « Tout le chantier est non commité » : faux. Il existe 61 commits depuis le
  point de divergence avec `origin/master`; seule la grande refonte postérieure
  à `20207bdbbb` est actuellement mélangée entre index, worktree et fichiers non
  suivis.
- « Les namespaces sont un reste du plan approuvé » : non trouvé dans le plan
  fourni. La coexistence de deux backends sous deux namespaces exige une ADR et
  une analyse ABI; elle est donc reportée au plan 011.
