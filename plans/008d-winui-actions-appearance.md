# Plan 008d — Fermer actions, sélection et apparence commune

> **Instructions** : corriger le seam commun avant un nouvel override local.
> Aucun commit sans validation.
>
> **Drift check** : `$scope=@('include/wx/winui/button.h','include/wx/winui/tglbtn.h','include/wx/winui/checkbox.h','include/wx/winui/radiobut.h','include/wx/winui/radiobox.h','include/wx/winui/stattext.h','include/wx/winui/statbox.h','include/wx/winui/statbmp.h','src/winui/button.cpp','src/winui/tglbtn.cpp','src/winui/checkbox.cpp','src/winui/radiobut.cpp','src/winui/radiobox.cpp','src/winui/stattext.cpp','src/winui/statbox.cpp','src/winui/statbmp.cpp','tests/controls'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha
- **Effort** : L
- **Risque** : MED
- **Dépend de** : 002, 003, 006, 007b

## Baseline et scope

Button/Toggle ont chacun un clic E2E rouge; RadioButton 5 échecs, RadioBox 6;
CheckBox et StaticBitmap ont respectivement 20 et 506 assertions vertes à
préserver. Restent 3-state, groupes, preferred focus, bitmap states, apparence,
DPI et UIA. Le contrat default/cancel appartient au plan 006.

## Étapes et vérifications

1. Isoler pourquoi l’injection Click échoue sans rendre `Disabled` faussement
   vert; tester pointer et keyboard Invoke via le routeur commun.
2. Toggle/BitmapToggle : state/event, tous bitmap bundles, marge/position et
   enabled. CheckBox : valeur entière 0/1/2 et commande exacte.
3. RadioButton : limites `wxRB_GROUP`, arrow/tab, destroy/reparent. RadioBox :
   preferred target, item show/enable, rows/cols, focus et UIA.
4. Créer/étendre `include/wx/winui/private/appearance.h` et
   `src/winui/appearance.cpp` (avec entrée CMake) pour font, foreground/background,
   reset, mnemonic/access key et bitmap DPI; ne pas dupliquer conversion.
5. StaticText/Box/Bitmap : ellipsize, font/color/mask, group semantics, bitmap
   refresh DPI et theme/high contrast.
6. Ajouter NSDMI/default+Create pour Button/Radio et scalaires apparentés, puis
   XRC smoke actions/statiques.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "Button::Click" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[CheckBoxTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[RadioButton]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[radiobox]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[wxStaticBitmap]" --reporter compact
```

## Done

- [ ] clic pointer/keyboard et disabled réels ;
- [ ] CheckBox/StaticBitmap baselines conservées ;
- [ ] 3-state, radio groups/items/focus conformes ;
- [ ] appearance/reset/DPI/UIA communs sans double scaling ;
- [ ] NSDMI/default+Create et XRC smoke verts.

## STOP

- Un fix default/cancel contourne le plan 006.
- Un composant duplique SlotState/Appearance au lieu d’étendre le seam.
- Une propriété XAML exige une couleur codée en dur pour paraître correcte.
