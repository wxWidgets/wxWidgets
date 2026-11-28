# Fiche 07 — Notebook, books, TreeCtrl et TreeList

Baseline `20207bdbbb` avec changements Tree non commités. Dépend des fiches 00,
03 et 08 pour focus, items, D&D et toolbar.

## wxNotebook — alpha rouge

TabView est créé à `notebook.cpp:91+`. Le test upstream signale trois erreurs :
image sans source et `GetTabRect()` non implémenté. Les styles non-top sont
acceptés mais la page réserve toujours une bande supérieure; SetPadding et
SetTabSize sont no-op (`notebook.cpp:369-403`).

### Plan

1. Faire passer `NotebookTestCase`: `GetTabRect`, hit-test, page image sans
   imagelist et mutations Add/Insert/Delete.
2. Implémenter `OnImagesChanged()` afin de rafraîchir toutes les tabs après
   SetImages/SetImageList/clear/DPI.
3. Décider `wxBK_TOP/BOTTOM/LEFT/RIGHT`: implémenter orientation et page rect,
   ou rejeter/fallback explicitement; jamais accepter silencieusement.
4. Implémenter padding/tab size ou documenter une limitation beta.
5. Valider le clip de bande déjà présent, scroll des pages et Tab mixte.
6. Ajouter UIA tabs/pages et hot theme/DPI.

### Tests

`notebooktest.cpp`, `AddPageEvents`, quatre orientations, selection veto,
images avant/après pages, 100/200 %, native+XAML page, scroll, Tab/Shift-Tab,
remove selected et destroy page.

## Books composites

| Composant | Stratégie V0 | Dépendance/gate |
|---|---|---|
| Simplebook | conserver common | pages mixtes, focus et show/hide |
| Choicebook | conserver generic | Choice fiche 03, events/page lifecycle |
| Listbook | conserver generic | ListCtrl fiche 10, scroll/selection |
| Toolbook | conserver generic | ToolBar fiche 08 |
| Treebook | conserver generic | TreeCtrl ci-dessous |

Après leurs dépendances, lancer toutes les suites `*booktest.cpp`, vérifier
images, veto, dynamic pages, focus initial et DPI. Aucune migration XAML dédiée
n’est requise pour beta.

## wxTreeCtrl — alpha rouge

25 assertions échouent dans 8 cas : sélection, suppressions, activation/menu,
expand/collapse, events changing/changed et label edit.

### Défauts statiques prioritaires

- TREE_KEY_DOWN ignore le résultat wx et ne marque pas l’event XAML Handled
  (`treectrl.cpp:479-503`) ;
- drop highlight vide (`:654`) et drag interne n’est pas D&D OLE ;
- `GetBoundingRect(textOnly)` ignore textOnly (`:1311+`) ;
- hit-test basé surtout sur Y et largeur client (`:1430+`) ;
- callback correction sélection capture `this` dans TryEnqueue sans garde et
  ignore le booléen (`:1641-1663`) ;
- état « un item expansible existe » non mis à jour sur toutes mutations, et
  `UpdatePeerItem` rescane l’arbre, donnant O(N²) au refresh (`:637,908,937,
  1389,1712-1739`).

### Plan

1. Transformer chaque assertion actuelle en sous-backlog et faire passer les
   opérations modèle programmatiques avant l’interaction.
2. Sécuriser TryEnqueue par weak generation; remettre pending=false si refus.
3. Centraliser sélection/event veto et rendre le comportement XAML consommable.
4. Mesurer les éléments réalisés (expander/image/text) pour bounding rect et
   hit flags; définir le comportement des items virtualisés.
5. Maintenir un compteur/état expansible en O(1) ou calculer une fois par refresh.
6. Implémenter drop highlight après le broker OLE fiche 00/plan 005.
7. Qualifier image list/DPI, bold/font/colors, editor, sort, RTL et UIA.

### Tests

Toute suite `treectrltest.cpp`; key handled/nonhandled; first/last expandable,
SetItemHasChildren/Delete*/hide-root; expander/icon/label/right-space hit-test;
10k items budget de refresh; edit label; D&D; 100/200 %; RTL; UIA names.

## wxTreeListCtrl

Fallback générique basé sur DataView. Conserver pour V0. Après Tree/DataView et
D&D : lancer TreeList tests, colonnes/sort/selection, editor, scroll, DPI/theme,
UIA et drag entre lignes. Ne pas engager une TreeView XAML multi-colonnes avant
beta.

## wxAuiNotebook

Traité en fiche 11. Il n’utilise pas wxNotebook et exige son propre gate de
drag/capture/floating/reparentage.

## Sortie

Alpha : Notebook/Tree suites alpha vertes, callback lifetime sûr, Tab pages
mixte, aucune orientation silencieusement fausse. Beta : tous books génériques
qualifiés, Tree hit-test/UIA/DPI/D&D et performance 10k validés.
