# Plan 008e — Stabiliser Notebook, TreeCtrl et les books

> **Instructions** : le drag Tree interne ne remplace jamais le broker 005.
> Aucun commit sans validation utilisateur.
>
> **Drift check** : `$scope=@('include/wx/winui/notebook.h','include/wx/winui/treectrl.h','src/winui/notebook.cpp','src/winui/treectrl.cpp','tests/controls','samples/widgets'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha
- **Effort** : L
- **Risque** : HIGH model/lifetime/perf
- **Dépend de** : 005, 007a–007c, 008a–008b

## Baseline et scope

Notebook retourne trois erreurs/not-implemented; TreeCtrl 25 échecs sur 8 cas.
Notebook suppose surtout top tabs et ne rafraîchit pas toutes les images;
TreeCtrl a gaps sélection/veto, key handled, bounding rect/hit-test, enqueue
tardif et refresh potentiellement O(N²). Books composites restent génériques.

## Étapes et vérifications

1. Notebook : modèle page/selection/event veto, `GetTabRect`/page rect, images
   et `OnImagesChanged`, clip bande, Tab focus. Pour chaque orientation, supporter
   réellement ou rejeter/documenter; aucun faux succès.
2. Tree lifetime : weak/generation pour correction/enqueue/edit; mutation/delete
   ne laisse aucune référence d’item/template.
3. Tree contrat : selection/veto, expand/collapse, edit, key consumed, exact
   bounding rect/hit-test/drop highlight et images.
4. Tree performance : cache/delta; benchmark 10k nodes et mutation subtree,
   interdire parcours O(N²) observé.
5. D&D : préserver le pipeline interne `DragItemsStarting/Completed` qui produit
   BEGIN/END_DRAG; tester sa coexistence sans double événement avec le broker
   OLE 005. Seuls `wxDropTarget`, formats externes et drops inter-fenêtres passent
   par le broker; tester cancel/reparent/multi-TLW.
6. Qualifier Simple/Choice/List/TreeBook après leurs dépendances. Toolbook est
   explicitement hors scope et appartient à 008f après Toolbar. Ajouter XRC
   smoke Notebook/Tree/books supportés de ce plan.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[NotebookTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[treectrl]" --reporter compact
```

## Done

- [ ] Notebook et Tree capacités alpha vertes ;
- [ ] orientations honnêtes, rects/images/clip corrects ;
- [ ] aucun callback/item après destroy ;
- [ ] benchmark 10k dans budget documenté ;
- [ ] drag Tree interne et broker OLE coexistent sans confusion/doublon ;
- [ ] books promis hors Toolbook et XRC smoke verts.

## STOP

- Une orientation nécessite une seconde île ou est simulée incorrectement.
- Hit-test virtualisé est approximé sans API/mesure fiable.
- Le drag interne et le broker OLE se déclenchent deux fois pour le même geste,
  ou une optimisation change l’ordre d’événements.
