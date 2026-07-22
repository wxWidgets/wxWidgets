# Plan 008c — Stabiliser plages, spin, date, temps et calendrier

> **Instructions** : un setter programmatique reste silencieux; une action
> utilisateur produit une séquence wx exacte. Aucun commit sans validation.
>
> **Drift check** : `$scope=@('include/wx/winui/gauge.h','include/wx/winui/slider.h','include/wx/winui/scrolbar.h','include/wx/winui/spinbutt.h','include/wx/winui/spinctrl.h','include/wx/winui/datectrl.h','include/wx/winui/timectrl.h','include/wx/winui/calctrl.h','src/winui/gauge.cpp','src/winui/slider.cpp','src/winui/scrolbar.cpp','src/winui/spinbutt.cpp','src/winui/spinctrl.cpp','src/winui/datectrl.cpp','src/winui/timectrl.cpp','src/winui/calctrl.cpp','tests/controls'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha; Time/Calendar beta
- **Effort** : L
- **Risque** : MED/HIGH events
- **Dépend de** : 003, 007a, 007b

## Baseline et scope

Slider a 4 échecs, SpinCtrl/Double 26 sur 13 cas, DatePicker 2; Gauge conserve
10 assertions vertes. ScrollBar émet un état non clampé/CHANGED prématuré,
SpinButton perd l’orientation verticale et Calendar confond un double tap large
avec un double clic wx. Ce plan corrige ces modèles; aucune nouvelle migration.

## Étapes et vérifications

1. Définir pour Slider/ScrollBar une machine source→wx event (line/page/thumb,
   track/release/end), valeur canonique clampée et setter silencieux.
2. SpinCtrl/Double : texte↔valeur, min/max/wrap, base, digits/increment,
   selection/caret et événements; aucune double notification NumberBox.
3. SpinButton : orientation, step/clamp/wrap, keyboard et UIA.
4. Gauge : préserver baseline; décider/documenter `wxGA_PROGRESS`, pulse puis
   retour determinate, vertical si promis.
5. DatePicker : bornes/default/ALLOWNONE/focus/locale. TimePicker : décider si
   secondes sont Supported Beta ou explicitement limitées. Calendar : navigation,
   marks, exact double-click, hit-test et month-change seulement en beta.
6. Ajouter NSDMI/default+Create pour Slider/Spin et scalaires apparentés, sans
   modifier une valeur API par défaut. Ajouter XRC smoke par famille.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[SliderTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[spinctrl]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[GaugeTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[DatePickerCtrlTestCase]" --reporter compact
```

## Done alpha

- [ ] Slider/ScrollBar/Spin/Gauge/DatePicker capacités alpha vertes ;
- [ ] une séquence exacte par action et aucun event de setter ;
- [ ] orientation/valeur/texte/precision conformes ;
- [ ] NSDMI/default+Create et XRC verts ;
- [ ] TimePicker/Calendar classés explicitement beta/experimental.

## STOP

- La source d’un événement XAML est indiscernable sans fabriquer un contrat.
- Aucune décision produit sur secondes/Calendar mais le fix élargit l’alpha.
- Le correctif casse les 10 assertions Gauge vertes.
