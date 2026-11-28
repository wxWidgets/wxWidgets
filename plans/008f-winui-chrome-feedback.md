# Plan 008f — Fermer chrome, feedback, tooltips et audit NSDMI

> **Instructions** : tout le contrat InfoBar appartient à 002, les menus à 004 et le
> lifetime transitoire à 006; ce plan n’en duplique pas les propriétaires.
> Aucun commit sans validation.
>
> **Drift check** : `$scope=@('include/wx/winui/toolbar.h','include/wx/winui/statusbar.h','include/wx/winui/infobar.h','src/winui/toolbar.cpp','src/winui/statbar.cpp','src/winui/infobar.cpp','src/winui/hyperlink.cpp','src/winui/activityindicator.cpp','src/winui/richtooltip.cpp','src/winui/ctrlhost.cpp','src/msw/window.cpp','tests/controls','samples/widgets'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P1 alpha/beta
- **Effort** : L
- **Risque** : MED/HIGH transient lifetime
- **Dépend de** : 002–004, 006, 007a–007c, 008d, 008e

## État et scope

Toolbar peut recréer un outil supprimé et ses toggle/control/stretch/dropdown
sont incomplets (`toolbar.cpp:187-267`). StatusBar ignore styles/tips/ellipsis;
RichToolTip utilise un host raw et une API partielle. API, checkbox, effets,
apparence et layout InfoBar appartiennent entièrement à 002; ce plan exécute
seulement leur non-régression d’intégration. Hyperlink et Activity restent à
qualifier UIA/lifetime.

## Étapes et vérifications

1. ToolTip commun : via SlotStateAdapter 003, replacement/removal/settings,
   disabled/hidden/destroy et tous roots XAML. Aucun host par tooltip.
2. Toolbar : modèle delta, delete exact, toggle, radio/dropdown, AddControl,
   stretch/overflow, hit-test, bitmap DPI et UIA. **Verify :** aucun ghost tool.
3. Toolbook : exécuter après Toolbar et dépendances book du 008e.
4. InfoBar : exécuter uniquement la suite 002 après changements chrome/tooltip;
   toute régression retourne à 002 au lieu d’être corrigée dans ce lot.
5. StatusBar : widths/styles/ellipsis/tooltips, GetFieldRect, theme/DPI/UIA.
6. Hyperlink/Activity : hover/visited/context/alignment et destroy while active.
7. RichToolTip : RAII et API rect/delay/style via TransientManager 006;
   enqueue failure/destroy/shutdown bornés.
8. Audit préventif NSDMI final des headers WinUI, notamment choice, combobox,
   slider, textctrl, radiobut et button; pour chaque scalar, prouver valeur API,
   ajouter default-constructor+Create test et ne pas inventer un bug actif.
9. Étendre XRC smoke pour chrome/feedback supportés.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets showcase -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "Window::ToolTip" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[wxInfoBar]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[hyperlinkctrl]" --reporter compact
```

## Done alpha

- [ ] Toolbar/ToolTip alpha sans ghost/leak/callback tardif ;
- [ ] Toolbook promis vert ;
- [ ] instrumentation/probes off par défaut ;
- [ ] audit NSDMI tracé, default+Create et XRC verts.

## Done beta

- [ ] non-régression InfoBar 002, StatusBar, Hyperlink/Activity et RichToolTip verts ;
- [ ] styles/layout/DPI/theme/UIA et transient lifetime qualifiés.

## STOP

- AddControl ou tooltip requiert une île supplémentaire.
- RichToolTip contourne le TransientManager ou démarre une loop après enqueue
  failure.
- Un NSDMI change une valeur publique sans oracle/tests.
