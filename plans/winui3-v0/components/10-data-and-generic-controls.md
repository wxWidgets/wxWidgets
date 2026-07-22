# Fiche 10 — Data controls et fallbacks génériques

Baseline `20207bdbbb`. Décision V0 : **ne pas réécrire ces contrôles en XAML**.
Les qualifier sous le bridge partagé avec le renderer Fluent.

## Pourquoi conserver les fallbacks

CMake force `src/generic/listctrl.cpp` et retire les backends MSW ciblés
(`build/cmake/lib/core/CMakeLists.txt:28-77`). DataView/Grid sont déjà génériques.
Une migration ItemsView avant stabilisation input/D&D/focus multiplierait les
risques sans débloquer l’alpha.

## Risques communs

- GDI sous bridge : paint/invalidations/Z/clip ;
- scroll : scrollbar cutouts et slot sync sans cisaillement ;
- editor temporaire `wxTextCtrl` : slot geometry/focus/Tab ;
- D&D : impossible avant broker OLE ;
- capture/column drag/context menus ;
- UIA d’un contrôle GDI sous un bridge UIA ;
- hot theme et DPI renderer/métriques.

## wxHeaderCtrl

Generic forcé (`include/wx/headerctrl.h:253+`). Beta : resize/reorder/drag,
sort indicator, hide/show columns, capture out-of-window, context menu, DPI,
theme et UIA header roles. Vérifier que le bridge ne vole ni cursor resize ni
mouse capture.

## wxListCtrl / wxListView / virtual

Generic forcé (`include/wx/listctrl.h:29`, CMake line 77). Beta : exécuter toutes
les suites listctrl/listview/virt; modes report/icon/list, columns, selection,
label editor, images, sort, hit-test, scroll, drag, popup menu, D&D, 100k virtual
items, DPI/theme/UIA.

Gate spécifique : un editor TextCtrl slotté doit suivre scroll/reorder et rendre
le focus au list control après accept/cancel/destroy.

## wxDataViewCtrl/List/Tree

Generic sur Windows (`include/wx/dataview.h:36`). Beta : selection, expand,
columns/renderers/editors, row height, sort, drag source/drop target, keyboard,
scroll, model mutation et UIA. Lancer `dataview.exe` et les tests DVC après D&D.

Ne pas confondre le renderer Fluent du chrome avec une migration native.

## wxTreeListCtrl

Generic au-dessus de DataView. Qualifier après DataView : hierarchy/columns,
sort, selection, image, editor, drag, delete model, DPI/UIA. Aucun peer XAML V0.

## wxGrid

Generic `wxScrolledCanvas`. Beta : paint, selection/cursor, keyboard, editors,
column reorder/resize, frozen rows/cols si activés, scroll rapide, D&D, clipboard,
large model et UIA. Porter une attention spécifique au child editor WinUI et au
cursor resize sous le bridge.

## wxPropertyGrid

Generic/custom avec nombreux editors TextCtrl. Recommandation post-beta sauf
besoin produit. Avant promotion : suite propgrid complète, categories, editors,
popup choices, validation, scroll, splitter, help, theme/DPI/UIA et reparent.

## wxVListBox / wxHtmlListBox / wxSimpleHtmlListBox

Generic custom. Beta seulement si exposés : selection/events, variable heights,
scroll/hit-test, keyboard, links/context, large lists, theme/DPI/UIA. D&D si API
utilisée.

## wxCollapsiblePane

Generic composite, bon candidat beta : collapse events, min-size/layout parent,
focus des children, pages mixtes, animation si présente, DPI/theme et destroy
expanded/collapsed.

## wxGenericDirCtrl / wxFileCtrl

Composites Tree/List/dialogs. Post-beta recommandé. Ils héritent de tous les
risques Tree/List, filesystem refresh, D&D files et dialogs owner.

## Plan de qualification

1. Terminer host/input/cursor/tooltips et broker D&D.
2. Lancer les tests existants de chaque famille et enregistrer les échecs, sans
   les masquer par exclusions globales.
3. Créer un sample mixte : Grid, ListCtrl, DataView, Header avec editors WinUI,
   scrollbars et D&D.
4. Corriger uniquement les adaptations bridge/renderer communes lorsque
   plusieurs contrôles échouent; éviter les forks de code générique.
5. Ajouter UIA/DPI/theme/perf gates.

## Critères beta

- suites ListCtrl/DataView/Grid/Header/TreeList ciblées vertes ;
- scroll/capture/editor/context menu/D&D sans perte ;
- aucun cisaillement visible au budget défini ;
- UIA sans fragment bridge parasite ;
- aucune réécriture native nécessaire pour la V0.

