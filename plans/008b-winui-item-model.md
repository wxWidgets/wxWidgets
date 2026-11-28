# Plan 008b — Unifier le modèle Choice/Combo/List/CheckList/BitmapCombo

> **Instructions** : `wxItemContainer` reste propriétaire du client data; aucun
> adapter ne devient un second owner. Aucun commit sans validation.
>
> **Drift check** : `$scope=@('include/wx/winui/choice.h','include/wx/winui/combobox.h','include/wx/winui/listbox.h','include/wx/winui/checklst.h','include/wx/winui/bmpcbox.h','src/winui/choice.cpp','src/winui/combobox.cpp','src/winui/listbox.cpp','src/winui/checklst.cpp','src/winui/bmpcbox.cpp','src/common/ctrlsub.cpp','src/common/bmpcboxcmn.cpp','tests/controls'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha
- **Effort** : L
- **Risque** : HIGH ownership/events
- **Dépend de** : 007a, 008a

## Baseline

Choice 4 échecs, Combo 8, ListBox 9, BitmapCombo 6. Le code reconstruit parfois
`Items()` et perd identité, multisélection ou association bitmap/texte;
`listbox.cpp:71-73,105-114,463-539` et `bmpcbox.cpp:83-108` sont les premiers
points à caractériser.

## Scope

Les cinq peers et, si justifié, `include/wx/winui/private/itemmodel.h`,
`src/winui/itemmodel.cpp` et son entrée CMake. Inclure tri,
sélection, hit-test, events et client data. Hors scope : DataView/ListCtrl
génériques, virtualisation nouvelle et migration ItemsView.

## Étapes et vérifications

1. Écrire l’oracle : identité stable, comparateur wx unique, ownership exact,
   mapping simple/multiple et ordre d’événements. Ajouter compteurs client data.
2. Choice : insertion/delete/set string delta, tri/casse/best size, selection
   programmée silencieuse. **Verify :** suite Choice verte.
3. ListBox : multi-selection Added/Removed, hit-test, scroll/top item, clear/
   delete et double-click. **Verify :** suite ListBox + ownership verte.
4. CheckList : conserver checked/selection/client data lors d’un delta; clavier
   et UIA toggle. **Verify :** aucune reconstruction destructive.
5. BitmapCombo : mutation texte+bitmap atomique, sort et DPI size. **Verify :**
   index n’associe jamais bitmap d’un autre item.
6. Combo : réutiliser TextPeer 008a, editable/readonly, selection/caret,
   dropdown/open/close et template revalidation.
7. Ajouter NSDMI/default+Create pour Choice/Combo et états du modèle, puis XRC
   smoke de chaque classe/style principal.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[ChoiceTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[ListBoxTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[CheckListBoxTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[BitmapComboBoxTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[ComboBoxTestCase]" --reporter compact
```

## Done

- [ ] cinq suites vertes pour la capacité alpha ;
- [ ] chaque client object détruit exactement une fois ;
- [ ] mutation unitaire sans `Items().Clear()` ;
- [ ] sélection/events/hit-test conformes en simple et multiple ;
- [ ] default+Create, DPI et XRC smoke verts.

## STOP

- Deux couches revendiquent ownership du client data.
- Le tri WinUI ne peut pas reproduire le comparateur wx : trier dans le modèle,
  ne pas accepter un ordre silencieusement différent.
- Le fix requiert une migration ItemsView/virtualisation hors V0.
