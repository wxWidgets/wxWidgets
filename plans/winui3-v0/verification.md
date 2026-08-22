# Preuves de build et de tests — 2026-07-21

## Environnement

- Windows 11 x64, Visual Studio 2022 Build Tools, MSVC 19.44.
- Configuration Release, générateur `Visual Studio 17 2022`, plateforme x64.
- HEAD `20207bdbbb81055da1f3e980b3271fc24566990e` avec le worktree sale décrit
  dans [README.md](README.md).
- Les répertoires `build-winui` et `build-winui-audit` sont ignorés par Git.

Cette preuve ne qualifie pas Windows 10, x86, ARM64, Debug, static ni un package
avec identité. Jusqu’aux gates du plan 010, le seul profil démontré est Windows
11 x64, MSVC, Release, DLL shared, unpackaged.

## Build existant

Commande :

```powershell
cmake --build build-winui --config Release --target wxcore widgets showcase minimal dataview winuispike -- /m
```

Résultat :

- `wxcore` et `widgets` compilent.
- La chaîne s’arrête au relink de `showcase` parce que le processus utilisateur
  `showcase.exe` verrouille `WebView2Loader.dll`. Ce n’est pas une erreur C++.
- Relancés séparément, `minimal`, `dataview` et `winuispike` compilent.
- Les processus utilisateur n’ont pas été arrêtés.

## Configuration fraîche et blocage `wxInfoBar`

Commande initiale :

```powershell
cmake -S . -B build-winui-audit -G "Visual Studio 17 2022" -A x64 `
  -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=OFF `
  -DwxBUILD_SHARED=ON
cmake --build build-winui-audit --config Release --target test_gui -- /m:4 /nr:false
```

Résultat : configuration réussie, puis compilation impossible :

- `src/xrc/xh_infobar.cpp:40,58,69,72,90` demande
  `wxINFOBAR_CHECKBOX`, `SetShowHideEffects`, `SetEffectDuration` et
  `ShowCheckBox`, absents de `include/wx/winui/infobar.h`.
- Avec `wxUSE_XRC=OFF`, `tests/controls/infobar.cpp:28` échoue encore sur
  `wxINFOBAR_CHECKBOX`.
- Avec `wxUSE_INFOBAR=OFF`, `test_gui.exe` est produit. Les bibliothèques AUI,
  richtext, STC, webview, propgrid, media et HTML ont également compilé.

Conclusion : le backend WinUI rompt une API publique commune; XRC n’est pas la
cause.

## Déploiement des ressources WinUI

Dans le build frais, `wx_winui3_configure_target()` ne copie que
`Microsoft.WindowsAppRuntime.Bootstrap.dll` (`build/cmake/winui.cmake:249-263`).
Le premier démarrage de `test_gui.exe --list-tests` a signalé :

```text
WinUI application resources initialization failed with HRESULT 0x80004005:
Cannot locate resource from
'ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xaml'.
```

Le build historique `build-winui` contenait, contrairement au build frais :

- `Microsoft.UI.pri` ;
- `Microsoft.UI.Xaml.Controls.pri` ;
- `resources.pri` ;
- les arborescences `Microsoft.UI/` et `Microsoft.UI.Xaml/`.

Copier seulement `generic.xaml` n’a pas suffi : le fallback `file:///` de
`src/winui/winui.cpp:134-178` a lui aussi retourné `E_FAIL`. Après copie
temporaire de l’ensemble des PRI/arborescences du build historique vers le
répertoire d’audit, les avertissements d’initialisation ont disparu. Ces copies
étaient uniquement dans un répertoire ignoré et ne constituent pas un fix.

Conclusion : un build frais ne produit pas un runtime autonome/reproductible.

## Inventaire des tests

```powershell
build-winui-audit\lib\vc_x64_dll\test_gui.exe --list-tests
```

Le binaire recense 487 cas. L’exit code de la commande de listing est 1, mais
la liste est complète; avant restauration temporaire des ressources, elle
émettait l’avertissement XAML ci-dessus.

## Échantillon ciblé après restauration temporaire des ressources

Tous les filtres ci-dessous ont été lancés séparément avec le reporter compact.
Les nombres sont des assertions échouées, sauf mention contraire.

| Filtre/cas | Résultat | Lecture |
|---|---:|---|
| `Button::Click` | 1 échec | le clic synthétisé n’atteint pas le bouton |
| `Button::Disabled` | 2 assertions OK | résultat peu probant tant que Click échoue |
| `[CheckBoxTestCase]` | 20 assertions OK | baseline API utile |
| `[ListBoxTestCase]` | 9 échecs | sélection, tri/casse, événements, hit-test |
| `[BitmapComboBoxTestCase]` | 6 échecs | TextEntry, sélection, taille bitmap |
| `[ChoiceTestCase]` | 4 échecs | sélection, casse, best size |
| `[ComboBoxTestCase]` | 8 échecs | caret, valeur, sélection/dropdown |
| `[SliderTestCase]` | 4 échecs | événements ligne/page et valeur |
| `[spinctrl]` | 26 échecs sur 13 cas | valeur, événements, texte, base, précision |
| `[treectrl]` | 25 échecs sur 8 cas | sélection, suppressions, événements, édition |
| `[DatePickerCtrlTestCase]` | 2 échecs | borne de date et focus |
| `[GaugeTestCase]` | 10 assertions OK | baseline API utile |
| `[RadioButton]` | 5 échecs sur 3 cas | clic, groupes et focus |
| `[radiobox]` | 6 échecs sur 2 cas | enable/show par item |
| `[wxSearchCtrl]` | 3 échecs | caret/sélection |
| `[TextCtrlTestCase]` | ~26 échecs; crash intermittent hors debugger | événements, caret, hit-test, lignes, style, coordonnées |
| `[ToggleButtonTestCase]` | 1 échec | clic synthétisé |
| `[NotebookTestCase]` | 3 erreurs | image sans source, `GetTabRect()` non implémenté |
| `[ColourPickerCtrlTestCase]` | 14 assertions OK | n’exerce pas l’identité d’événement trouvée statiquement |
| `[wxStaticBitmap]` | 506 assertions OK | baseline API solide |

Le `TextCtrlTestCase` a terminé sous `cdb` avec environ 26 assertions en échec,
mais a quitté prématurément lors de plusieurs exécutions normales. Le debugger a
modifié le timing; aucune pile AV fiable n’a été obtenue. Le comportement doit
être traité comme un signal de durée de vie/réentrance à reproduire avec ASan ou
Application Verifier, pas attribué arbitrairement à une ligne unique.

## Commandes de reproduction minimales

Après que les plans 001 et 002 auront produit un build frais fonctionnel :

```powershell
cmake --build build-winui-audit --config Release --target test_gui -- /m:4 /nr:false
build-winui-audit\lib\vc_x64_dll\test_gui.exe "Button::Click" --reporter compact
build-winui-audit\lib\vc_x64_dll\test_gui.exe "[ListBoxTestCase]" --reporter compact
build-winui-audit\lib\vc_x64_dll\test_gui.exe "[TextCtrlTestCase]" --reporter compact
```

Attendu pour une V0 beta : exit code 0 pour les composants déclarés supportés,
ou exclusions documentées et limitées à des capacités explicitement hors V0.

## Exécution initiale du plan 001 — 2026-07-22 (historique, superseded)

> **HISTORIQUE — NE DÉCRIT PLUS LE CODE ACTUEL.** Cette première exécution,
> son blocage Engineering Preview 2.1, son extraction depuis le MSIX, son
> payload de quatre fichiers et ses trois self-tests ont été remplacés le
> 2026-07-25. La source de vérité actuelle est la section
> « Plan 001 — résolution stable du runtime et de la redistribution » plus
> bas dans ce document.

Plan 001 exécuté puis repris après revue; statut final :
`BLOCKED: licence/redist WinUI 2.1.0` (constat licence détaillé dans
[001-winui-build-runtime-baseline.md](../001-winui-build-runtime-baseline.md),
section « Constat STOP licence »). Les gates techniques ci-dessous sont
toutes vertes pour un **profil développement** (framework-dependent
unpackaged, Windows 11 x64, MSVC, Release, DLL).

### Cause racine et payload

Un build vierge ne déployait que
`Microsoft.WindowsAppRuntime.Bootstrap.dll` : la résolution `ms-appx:///`
d'un processus unpackaged se fait contre un `resources.pri` à côté de
l'exécutable, et `XamlControlsResources` échouait
(`0x80004005: Cannot locate resource from
'ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xaml'`; reproduit par
la boîte « Widgets Warning » au lancement de `widgets.exe` et par le probe,
exit 3). Le payload correct est l'index de ressources du framework package
lui-même plus ses assets, extraits au configure du MSIX du package NuGet
`Microsoft.WindowsAppSDK.Runtime` épinglé — les artefacts copiés à la main
en mai dans `build-winui` (2 PRI composant, XBF, pri de 1 008 octets)
étaient une solution partielle; les 2 PRI composant sont prouvés superflus.
Plus rien du package `Microsoft.WindowsAppSDK.WinUI` (Engineering Preview)
n'est déployé.

### Gates du rejeu clean-room (build-winui-clean2, dossier vierge)

```powershell
cmake -S . -B build-winui-clean2 -G "Visual Studio 17 2022" -A x64 `
  -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=ALL `
  -DwxBUILD_SHARED=ON
cmake --build build-winui-clean2 --config Release `
  --target wx_winui_runtime_smoke minimal widgets -- /m:4 /nr:false
ctest -C Release -R wx_winui --output-on-failure
```

| Gate | Résultat |
|---|---|
| Configure clean-room (téléchargements NuGet compris) | exit 0, 106,7 s; « using Windows 10 SDK 10.0.26100.0 »; inventaire payload de 4 fichiers loggé, tous issus des packages épinglés |
| Build `wx_winui_runtime_smoke minimal widgets` | exit 0; `Deploying the WinUI runtime payload for` smoke/minimal/widgets (plus de déploiement par wxcore) |
| `ctest -R wx_winui` | 3/3 Passed : `wx_winui_runtime_smoke` (0,82 s), `wx_winui_selftest_sdk_selection`, `wx_winui_selftest_nupkg_cache` |
| Probe seul avec bootstrap sans PRI (état d'avant fix) | exit 3, message précis + rappel du payload attendu |
| `resources.pri` renommé puis restauré | exit 3 propre, puis exit 0 |
| Collision : `resources.pri` applicatif (contenu ≠, tag intact) | warning CMake, fichier applicatif intact, tag `resources.pri.wxwinui` retiré; re-run stable (toujours intact, toujours warné) |
| Owned-stale : tag = hash du pri en place | remplacé par le pri du staging, tag mis à jour; probe final exit 0 PASSED |
| `widgets.exe` depuis `lib/vc_x64_dll` | fenêtre « wxMSW widgets demo », aucune boîte de warning |
| `minimal.exe` | fenêtre « Minimal wxWidgets App », aucune boîte de warning |
| `test_gui.vcxproj` généré | 2 occurrences du POST_BUILD `wxWinUIDeployRuntime.cmake` (compilation/exécution = plan 002) |

Self-tests CMake versionnés (`tests/winui/selftest/`, enregistrés dans
CTest, hors ligne, archives synthétiques) : sélection SDK numérique
(9600/19041/26100 → 26100), version incomplète sautée, mode outil fourni
par l'utilisateur; archive intacte validée/extraite/stampée, cache corrompu
rejeté **avant** extraction, changement de version épinglée jamais résolu
vers l'ancien dossier, changement de hash épinglé invalidant l'extraction.
Les six SHA-256 épinglés correspondent aux archives téléchargées le
2026-07-21 **et** au cache de mai 2026 (deux instants indépendants).

### Défauts préexistants observés (non traités, hors scope 001)

- Le build monolithique crée `wxmono` sans `wx_winui3_configure_target`
  (pas de dépendance C++/WinRT ni `/bigobj`).
- L'export CMake (`wxWidgetsTargets.cmake`) propage les `.lib` WinAppSDK par
  chemins absolus vers le cache de packages du build : install non
  relocalisable; la consommation `find_package` hors arbre est documentée
  comme non supportée pour l'instant.

### Limites

- Le probe ne prouve pas la nécessité des fichiers `Microsoft.UI.Xaml/Assets`
  (il reste vert sans eux); ils sont inclus défensivement car référencés par
  les ressources de thème, avec la disposition du framework package.
- Les `.winmd` recommandés par le guide de déploiement Microsoft pour les
  apps unpackaged ne sont pas déployés (aucun scénario ne l'a exigé;
  disponibles dans le MSIX du framework au besoin).
- Premier build : plusieurs exécutables peuvent copier le même fichier vers
  `lib/vc_x64_dll` en parallèle (copy_if_different); une collision serait un
  échec de build bruyant, pas une corruption silencieuse.

### Corrections de seconde revue (2026-07-22)

1. **Cache utilisateur protégé** : l'invalidation de version et le
   stamp-check destructif ne s'appliquent plus qu'aux répertoires
   directement sous `wxWINUI3_PACKAGE_ROOT`; un répertoire pointé
   explicitement par `wxWINUI3_*_PACKAGE_DIR` (ou l'arbre vendored) est
   accepté sur la foi du marker et n'est jamais supprimé. Self-test
   `nupkg-cache.cmake` cas 5 : « user-provided directory trusted and
   untouched » — 5/5 cas verts.
2. **Déploiements parallèles sérialisés** : le script généré prend un
   `file(LOCK .../.wx-winui-deploy.lock GUARD PROCESS TIMEOUT 120)` par
   dossier de sortie avant toute copie (PRI + sidecar compris). Gate : 4
   exécutions simultanées de `wxWinUIDeployRuntime.cmake` vers un même
   dossier vierge → 4 × exit 0, `resources.pri` final identique au staging,
   sidecar cohérent. Le fichier `.wx-winui-deploy.lock` reste dans le
   dossier (résidu normal de `file(LOCK)`).
3. **Staging MSIX estampillé** : le stage
   `winui/runtime-payload/<ver>/win-<arch>/` porte
   `wxwinui-msix-source-sha256.txt` (hash épinglé du package Runtime dont
   provient le MSIX); un re-pin du hash sans changement de version jette et
   ré-extrait le stage. Gate : reconfiguration de `build-winui-clean2` dont
   le stage antérieur était sans stamp → ré-extraction observée, stamp
   `f3ed3c07…` posé.

Après ces corrections : reconfiguration exit 0, rebuild exit 0,
`ctest -R wx_winui` 3/3 Passed, probe exit 0 PASSED.

## Exécution du plan 002 — 2026-07-22

Plan 002 exécuté : parité du contrat public `wxInfoBar` restaurée et porte de
tests standard rouverte. Drift check conforme (HEAD `20207bdbbb`, empreintes
header/source identiques au plan, scope intact dans l'index et le worktree).

### Changements

- `include/wx/winui/infobar.h` : contrat public complet — `wxINFOBAR_CHECKBOX`
  (défini localement avec garde `_WX_GENERIC_INFOBAR_H_` : le port exclut
  délibérément `src/generic/infobar.cpp` du build, donc inclure le header
  générique — première tentative — casse le link, la classe dllexport
  n'ayant pas d'implémentation), `SetShowHideEffects()`,
  `GetShowEffect()`/`GetHideEffect()` (résolution automatique par placement
  dans le sizer, comme le générique), `SetEffectDuration()`/
  `GetEffectDuration()`, `ShowCheckBox()`, `IsCheckBoxChecked()`,
  `SetFont()`/`SetForegroundColour()` overrides, `GetDefaultBorder()` =
  `wxBORDER_NONE`.
- `src/winui/infobar.cpp` : checkbox XAML composée avec les boutons custom
  dans `InfoBar.Content` (les deux coexistent; les boutons custom remplacent
  toujours la croix de fermeture), état checked synchronisé par les
  événements Checked/Unchecked; effets show/hide implémentés par
  `ShowWithEffect`/`HideWithEffect` (AnimateWindow) avec fallback
  show/hide simple documenté (jamais de no-op); font/foreground propagés au
  contrôle XAML (y compris posés avant `Create()`); `DoGetBestSize()`
  mesure le contenu XAML réalisé (`MeasureContent`), fallback 200x48 DIP;
  `InvalidateBestSize()` + relayout parent sur message/boutons/checkbox.
- `tests/controls/infobar.cpp` : 4 nouveaux cas — `Checkbox` (état,
  coexistence avec boutons, label vide), `Effects` (défauts, hors-sizer →
  NONE, setters/getters, durée), `Appearance` (font/couleur avant et après
  création), `BestSizeInvalidation` (texte long, boutons+checkbox).
- `docs/winui/README.md` : note de mapping wxInfoBar (composition
  checkbox/boutons, effets via AnimateWindow et leur limite visuelle sur la
  bande composée, fallback).
- `src/xrc/xh_infobar.cpp` et `build/cmake/tests/gui` : non modifiés
  (l'API restaurée suffit).

### Gates

```powershell
cmake -S . -B build-winui-clean -G "Visual Studio 17 2022" -A x64 `
  -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=ALL `
  -DwxBUILD_SHARED=ON -DwxUSE_XRC=ON -DwxUSE_INFOBAR=ON        # exit 0
cmake --build build-winui-clean --config Release --target test_gui `
  -- /m:4 /nr:false                                             # exit 0
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[wxInfoBar]" `
  --reporter compact    # exit 0 — Passed all 5 test cases with 38 assertions
ctest --test-dir build-winui-clean -C Release `
  -R "wx_winui_(runtime_smoke|selftest_)" --output-on-failure   # 3/3 Passed
```

Aucun warning `Cannot locate resource` (payload du plan 001 déployé par le
build).

### Dashboard composants (test_gui frais du 2026-07-22)

Chaque filtre exécuté séparément (`--reporter compact`, timeout 120 s).
`exit` est le code de sortie du processus; les échecs listés sont le backlog
des sous-plans 008a–008f et ne sont pas masqués.

| Filtre | exit | Résultat |
|---|---:|---|
| `[wxInfoBar]` | 0 | **Passed all 5 test cases (38 assertions)** |
| `Button::Disabled` | 0 | Passed (2) |
| `[CheckBoxTestCase]` | 0 | Passed (20) |
| `[GaugeTestCase]` | 0 | Passed (10) |
| `[ColourPickerCtrlTestCase]` | 0 | Passed (14) |
| `[wxStaticBitmap]` | 0 | Passed both 2 test cases (506) |
| `Button::Click` | 1 | 1 échec (clic synthétisé non reçu) |
| `[ListBoxTestCase]` | 9 | 9 échecs (HitTest, sélection, tri) |
| `[BitmapComboBoxTestCase]` | 6 | 6 échecs (GetBitmapSize, TextEntry) |
| `[ChoiceTestCase]` | 4 | 4 échecs (best size, sélection) |
| `[ComboBoxTestCase]` | 8 | 8 échecs (valeur, dropdown) |
| `[SliderTestCase]` | 4 | 4 échecs (valeur, événements) |
| `[spinctrl]` | 26 | 26 échecs / 13 cas (valeur, événements) |
| `[treectrl]` | 27 | 27 échecs / 10 cas (sélection, menu, édition) |
| `[DatePickerCtrlTestCase]` | 2 | 2 échecs (borne, focus) |
| `[RadioButton]` | 5 | 5 échecs / 3 cas (focus, groupes) |
| `[radiobox]` | 6 | 6 échecs / 2 cas (show/enable par item) |
| `[wxSearchCtrl]` | 3 | 3 échecs (caret/sélection) |
| `[TextCtrlTestCase]` | 0xC0000005 | 26 échecs / 1 cas puis **access violation en sortie** (signal lifetime connu, plan 008a) |
| `[ToggleButtonTestCase]` | 1 | 1 échec (clic synthétisé) |
| `[NotebookTestCase]` | 0xC0000005 | 3 échecs (`GetTabRect()` non implémenté) puis **access violation** (plan 008e) |

Lecture : cohérent avec l'échantillon d'audit du 2026-07-21 (mêmes familles
et volumes d'échecs); `[wxInfoBar]` passe de « ne compile pas » à vert.
Note d'exécution : ces tests injectent de l'entrée réelle
(`wxUIActionSimulator`/SendInput) — ne pas utiliser la machine pendant un
run, fenêtres de test visibles requises.

### Passe corrective de revue (2026-07-22, plan 002)

Cinq défauts de contrat confirmés par la revue, corrigés :

1. **Croix native → `wxEVT_BUTTON`/`wxID_CLOSE`** : la fermeture XAML est
   annulée (`InfoBar.Closing`, `Reason == CloseButton`, `Cancel(true)`) et
   l'événement wx est routé par la chaîne de handlers; `Dismiss()` seulement
   s'il n'est pas traité — le pattern « Do not show this again » du sample
   `dialogs` (lecture de `IsCheckBoxChecked()` dans le handler) fonctionne.
2. **Fallback d'animation réellement effectif** : `MSWShowWithEffect()`
   bascule le flag wx avant qu'`AnimateWindow()` puisse échouer; le fallback
   réinitialise ce flag (`wxWindowBase::Show(!state)`) avant le
   `Show()`/`Hide()` de secours, qui n'est donc plus un no-op.
3. **Durée bornée** : `SetEffectDuration()` clampe dans `[0, 10000]` ms
   (négatif → défaut plateforme; jamais de -1 converti en ~4,3e9 ms vers le
   timeout unsigned d'`AnimateWindow()`).
4. **`RemoveButton()` retire le bouton ajouté le plus récemment** (itération
   inverse, parité générique).
5. **`SetFont(wxNullFont)` réinitialise le peer** (`ClearValue` des quatre
   propriétés de police; une face vide efface aussi `FontFamily` au lieu de
   garder celle d'une police précédente).

Tests renforcés (`tests/controls/infobar.cpp`) : `RemoveLastAdded` (ids
dupliqués → ordre restant observable), clamp de durée (assertions
`__WXWINUI__`), reset `wxNullFont`, et `EffectShowHide` — barre dans un
sizer d'un panel à largeur fixe, effets actifs (durée 1 ms), assertions sur
l'état **physique** de la fenêtre (`::IsWindowVisible`, `__WXMSW__`) qui
attrapent un fallback no-op, plus largeur contrainte sous message long.

Gates après correction : rebuild `test_gui` exit 0 (un premier run a subi un
arrêt transitoire de CL.exe sur `xh_infobar.cpp` — MSB6006 sans diagnostic,
machine en usage; la TU compile en isolation et le build suivant passe);
`[wxInfoBar]` : **7 cas, 51 assertions, exit 0**; `ctest -R
"wx_winui_(runtime_smoke|selftest_)"` : 3/3 Passed.

Limites de test restantes (documentées, non masquées) : le clic physique
sur la croix XAML (template WinUI) et l'apparence effective du peer ne sont
pas automatisables par l'API publique — couverts par revue de code et par le
sample `dialogs` en validation manuelle. Incohérence préexistante hors
scope, à trancher upstream : `interface/wx/infobar.h:18` documente
`wxINFOBAR_CHECKBOX = 0x0080` alors que les headers réels (générique et
winui) définissent `0x0010`.

### Seconde passe corrective (2026-07-22, plan 002) — croix native

La revue a confirmé un blocker dans le flux `Closing` : quand `Closing`
fire, WinUI a déjà mis `IsOpen` à `false` et, si `Cancel` est vrai après le
callback, il ré-ouvre lui-même la barre (`OnCloseButtonClick`/
`RaiseClosingEvent` dans microsoft-ui-xaml). Mon `Cancel(true)`
inconditionnel laissait donc le peer XAML ré-ouvert alors que le HWND wx
était caché dès que le handler applicatif appelait `Dismiss()`.

Correctif — machine d'état dans l'impl (`closeRequestPending`/
`dismissedDuringClose`) : le handler `Closing` (raison `CloseButton`
uniquement, garde de réentrance) route `wxEVT_BUTTON`/`wxID_CLOSE` pendant
le callback; `Dismiss()` appelé dans ce contexte enregistre le verdict au
lieu d'écrire un `IsOpen` déjà false; au retour, `Cancel(true)` n'est posé
que si la barre doit rester ouverte. Les quatre cas sont garantis :
non-traité → fermé; traité+`Dismiss()` → fermé; traité sans `Dismiss()` →
annulé, les deux restent ouverts; `Dismiss()` programmatique → fermé sans
émission `wxID_CLOSE` (raison `Programmatic`, aucune branche du handler).

Testabilité ajoutée (section « implementation only » du header, précédent
wxGTK) : `WinUIIsPeerOpen()` (état `IsOpen` réel du peer) et
`WinUIClickCloseButton()` (invoque la croix du template via son
`ButtonAutomationPeer` — le vrai chemin `CloseButtonClick → Closing`,
déterministe, sans souris). Nouveau test `wxInfoBar::WinUICloseButton`
(WinUI uniquement) : les quatre cas ci-dessus, un seul `wxEVT_BUTTON` par
clic, état du peer vérifié après chaque fermeture **et** réouverture
(`ShowMessage` ré-ouvre réellement le XAML), cycle final vivant.
Le commentaire d'`EffectShowHide` est corrigé : seule la branche normale
d'animation est automatisée, le fallback d'échec d'`AnimateWindow` n'a pas
de seam de test et reste couvert par revue de code.

Gates : rebuild `test_gui` + sample `dialogs` exit 0 (une erreur C3779 à la
première passe — include `Automation.Provider.h` manquant pour
`Invoke()` — corrigée); `[wxInfoBar]` : **8 cas, 72 assertions, exit 0**;
`ctest -R "wx_winui_(runtime_smoke|selftest_)"` : 3/3 Passed;
`git diff --check` : exit 0. Validation manuelle de la croix dans le sample
`dialogs` (croix, checkbox « Do not show this again », re-déclenchement) :
**confirmée par l'utilisateur le 2026-07-22** — plan 002 clos `DONE`.

## Exécution du plan 003 — en cours (2026-07-22)

Drift check conforme (diff HEAD vide sur le scope, les 4 empreintes worktree
exactes). **Passe A implémentée** (étapes 1-3 et 7 du plan) :

- `wxWinUISlotLifetime` (header privé) : état ref-counted par slot, seule
  voie des callbacks vers wx; compteur d'instances pour les tests.
  Introspection : `GetLiveHostCount()`/`GetLiveSlotCount()`/
  `GetLiveSlotLifetimeCount()`.
- Transaction `DisconnectSlot()` : invalide l'état, révoque tous les
  handlers routés (paires RoutedEvent+boxed stockées sur le slot) et les
  tokens GotFocus/LostFocus, détache le contenu, retire le container du
  root. Utilisée par `UnregisterSlot()` ET le rollback d'échec de
  `RegisterSlot()`.
- `BindSlotEvents()` réécrit : plus aucune capture crue de
  wxWindow/host/container — état partagé + container résolu depuis le
  sender (casse le cycle de rétention Grid→handler→Grid).
- Freeze borné : `FlushSync()` gelé note `m_frozenDirty` et rend la main
  (plus de boucle CallAfter); `wxWinUITLWHostNotifyThaw()` programme
  l'unique flush de rattrapage. (Point de notification et bornage corrigés
  par la passe corrective ci-dessous.)
- Tests `tests/controls/winuihostlifecycle.cpp` (`[HostLifecycle]`) :
  RegisterUnregisterLoop (×100), DestroyTLWWithSlots, ReparentAcrossTLW
  (×20), DestroyWithFlushInFlight; enregistrés dans le CMake des tests GUI
  (+ /bigobj, la TU tire la projection C++/WinRT). Note d'honnêteté : le
  seam de comptage n'existant pas avant la passe, les tests n'ont pas pu
  être exécutés « rouges » sur l'ancien code; ils caractérisent le contrat
  cible.

**Gates passe A (2026-07-22)** : build `test_gui widgets minimal` exit 0
(après fermeture d'un `dialogs.exe` résiduel de la validation manuelle qui
verrouillait la DLL core — LNK1104); **`[HostLifecycle]` : 4 cas, 28
assertions, exit 0** (aucun état de callback ne survit aux 100 cycles, à la
destruction de TLW peuplée, aux 20 reparentages croisés ni aux destroys
avec flush en vol); **`[window]` : 26 cas, 124 assertions, exit 0**;
`Button::Click` : **rouge persistant, identique à l'avant-passe** (le clic
SendInput n'atteint pas la cible; le routage Click XAML→wx fonctionne —
prouvé par l'invoke programmatique du test InfoBar et l'usage réel). Le
critère Done « Button vert » du plan 003 pointe en réalité sur le pipeline
pointer/hit-test du plan 007a, hors scope 003; à arbitrer à la clôture.

**Restent** : durcissement des captures du root router (extension étape 3),
étape 4 (slot state adapter : enabled/tooltip/UIA/focus préféré), étape 5
(`wxWinUIXamlHost` → slot du host partagé), étape 6 (API Win32 checked :
hook/subclass/SetWindowPos/SetWindowRgn + DeleteObject sur échec), étape 8
(extraction Lifetime/SlotState), gates finales complètes.

### Passe corrective de revue (2026-07-22, passe A) — 4 défauts P1

Verdict de revue : passe A non validée, 4 défauts structurels. Corrections
(row 003 passée à IN PROGRESS; la suite du plan — state adapter,
`wxWinUIXamlHost` — reste suspendue jusqu'à validation) :

1. **`Shutdown()` contournait `DisconnectSlot()`** (slots supprimés
   directement : lifetime non invalidé, handlers/tokens non révoqués;
   `m_source.Close()` pouvait rappeler une fenêtre en destruction).
   Correctif : la transaction est scindée — nouvelle moitié
   `RevokeSlotCallbacks()` (invalidation de l'état + révocation des
   handlers routés et des tokens focus, arbre visuel intact) appelée par
   `DisconnectSlot()` ET par `Shutdown()` **avant** `Close()`; la
   libération visuelle (delete des slots, `gs_slotGrids`, root) passe
   **après** `Close()`. L'invariant « fermer la source avec son arbre
   attaché » est préservé : seuls les callbacks sont coupés avant.
2. **Rollback de `RegisterSlot()` incomplet** (`m_slots[window]`,
   `BindSlotWindow()`, `BindSlotEvents()` hors du try). Correctif : les
   trois entrent dans le try transactionnel; le catch fait
   `m_slots.erase` + `m_dirty.erase` + `UnbindSlotWindow` +
   `DisconnectSlot` + delete (les handlers déjà ajoutés à mi-course sont
   dans `m_routedHandlers`, donc révoqués).
3. **Freeze non réellement borné** (le flush gelé remettait
   `m_flushScheduled` à false → chaque `MarkDirty()` suivant repostait un
   CallAfter; et la notification vivait dans `wxWindowMSW::DoThaw()` alors
   que les TLW ont un override vide — sans enfant notifiant, pas de
   rattrapage). Correctif : `ScheduleFlush()` ne poste **rien** pendant le
   gel (pose `m_frozenDirty`, l'early-return de `FlushSync()` ne couvre
   plus que la course schedulé-avant-gel); notification déplacée de
   `wxWindowMSW::DoThaw()` (retirée) vers l'override
   `wxTopLevelWindowMSW::DoThaw()` (`src/msw/toplevel.cpp`) —
   `wxWindowBase::Thaw()` décrémente le compteur puis appelle toujours ce
   `DoThaw` en fin de gel, avec ou sans enfants, et le gel est détecté au
   niveau TLW (`m_tlw->IsFrozen()`) : c'est le point unique et suffisant.
4. **Tests insuffisants**. Nouveaux seams cumulés
   `GetSlotHandlerAddCount()`/`GetSlotHandlerRevokeCount()` (chaque
   AddHandler/GotFocus/LostFocus compté à l'ajout, chaque révocation
   effective comptée dans `RevokeSlotCallbacks`) et
   `GetFlushScheduleCount()`/`GetFlushRunCount()` (CallAfter postés /
   flushes réellement exécutés). Tests renforcés :
   RegisterUnregisterLoop, DestroyTLWWithSlots (chemin `Shutdown()`!) et
   ReparentAcrossTLW vérifient `adds == revokes` sur leur delta;
   ReparentAcrossTLW vérifie à chaque migration que le host CIBLE possède
   le slot (`FindForTLW(target)->FindSlot(button)` non nul) et que
   l'autre ne l'a plus; nouveau cas **FreezeThawCoalescedFlush** : 50
   `Move()` sous `Freeze()` → delta de flushes postés ≤ 1 (borné), puis
   `Thaw()` → le flush de rattrapage s'exécute (`GetFlushRunCount()`
   croît) et `GetRectInTLW()` reflète la dernière position (rattrapage
   géométrique prouvé, y compris le chemin TLW-override sans dépendre des
   enfants).

Gates de la passe corrective : build `test_gui widgets minimal` Release
exit 0 sans warning; **`[HostLifecycle]` : 5 cas, 101 assertions, exit 0**.
Incident de méthode consigné : un run `[window]` lancé PENDANT le rejeu
×100 en arrière-plan a échoué (3 assertions) par interférence de focus
entre les deux instances de test_gui; le même run isolé est vert (26 cas,
124 assertions) — les gates s'exécutent désormais strictement en
séquentiel.

Gates séquentielles finales (2026-07-22) :

- **rejeu `[HostLifecycle]` ×100 : 100/100 green** (arrêt au premier
  échec; aucun);
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : rouge préexistant identique
  (`clicked.GetCount() == 0`), inchangé — pipeline pointer/hit-test du
  plan 007a, hors scope 003.

Verdict de revue sur cette passe : **passe A acceptée comme jalon**
(étapes 1-3 et 7), plan 003 maintenu IN PROGRESS, pas de passage à la
phase 4; deux renforcements de test exigés « sous réserve ».

### Passe B (2026-07-22) — réserves de revue + étapes 4, 5, 6, 8

Renforcements de test exigés par la revue :

- **Freeze strict** : `FreezeThawCoalescedFlush` draine jusqu'à
  quiescence des compteurs avant `Freeze()` (`DrainToQuiescence`), puis
  vérifie `schedule delta == 0` ET `run delta == 0` pendant le gel, et
  `schedule delta == 1` ET `run delta == 1` après `Thaw()` (plus les
  positions rattrapées) — au lieu des `<= 1` / `> 0` initiaux.
- **Injection de panne du rollback** : seam `TestFailHandlerAdd(nth)`
  (auto-désarmé après tir) + `wxWinUIMaybeInjectHandlerFault()` aux 9
  points d'abonnement de `BindSlotEvents` (6 pointer, DoubleTapped,
  GotFocus, LostFocus). Nouveau cas `RegisterRollbackOnFault` : sur une
  fenêtre nue sans peer, fait échouer chaque abonnement 1..9 et vérifie
  après chaque échec : `RegisterSlot` nul, pas de slot, pas de lifetime,
  balance adds==revokes, et l'élément réutilisable (un register normal
  réussit ensuite — un container non détaché ferait échouer l'append).
- Réserves secondaires : `DestroyWithFlushInFlight` vérifie aussi slots
  + balance; `/bigobj` scoped à la seule TU de test
  (`set_source_files_properties`); externs locaux → header (étape 8).

Étapes du plan implémentées :

- **Extension étape 3 — root router durci** : les 5 handlers XAML du
  root (4 pointer + DoubleTapped) ne capturent plus `this` cru mais un
  `m_hostLifetime` invalidable (invalidé en tête de `Shutdown()`), et
  leurs paires (RoutedEvent, delegate) sont stockées (`m_rootHandlers`)
  puis **révoquées avant `Close()`** — la révocation libère les copies
  du state de façon synchrone (leçon d'un rouge intermédiaire : la
  finalisation de l'arbre XAML mort est différée, un lifetime survivait
  au compteur). Comptés dans la même balance adds/revokes. Les tests du
  top window amorcent le host (`PrimeTopWindowHost`) avant leurs
  baselines : la création paresseuse du host (5 root handlers + 1
  lifetime, vivants jusqu'à la mort de la TLW) n'est pas une fuite.
- **Étape 4 — slot state adapter** dans le seam commun `SyncSlot` (pas
  d'overrides par composant), avec caches par slot (`m_lastEnabled`,
  `m_lastToolTip`, `m_lastUIAName`) pour un no-change à quelques
  comparaisons : enabled EFFECTIF (`IsEnabled()` replie les ancêtres) →
  `Control::IsEnabled` + `IsHitTestVisible` du container (un slot
  désactivé ne réagit ni ne capte le hit-test); tooltip depuis
  `GetToolTip()` (fonctionne pour toute fenêtre hébergée, pas seulement
  les contrôles qui la relaient); nom UIA suivant les changements de
  label sans écraser un nom posé par l'application. Propagation parent :
  hook `WM_ENABLE` dans `MSWWindowProc` (message non consommé) →
  `wxWinUITLWHostNotifyEnable` → `MarkAllDirty()` — un seul seam, pas de
  N callbacks. Focus préféré dans `FocusSlot` : si le contenu n'est pas
  un Control focusable (RadioBox = Grid de RadioButtons),
  `FocusManager::FindFirstFocusableElement` du container.
- **Étape 5 — `wxWinUIXamlHost` converti** : plus de
  `DesktopWindowXamlSource` propre; le contenu chargé par XamlReader
  devient un slot du host partagé (API publique et header inchangés —
  pas de rupture ABI, le STOP ne s'applique pas). `ClearContent` vide le
  slot sans le retirer; le dtor désenregistre explicitement. Nouveau cas
  `XamlHostSharesTheIsland` : un XamlHost + un bouton natif dans une
  frame → `GetLiveHostCount` delta == 1 (une source par TLW), le slot du
  XamlHost appartient au host partagé, clear + re-set utilisables,
  compteurs équilibrés à la destruction.
- **Étape 6 — API Win32 vérifiées** : retour de `SetWindowsHookExW`
  contrôlé et logué (dégradation déterminée : seules les boucles
  modales natives perdent la pré-traduction); `SetWindowSubclass` du
  bridge logué, celui de l'inner re-tenté au flush suivant si échec
  (`m_inner` laissé nul); les deux `SetWindowPos` d'épinglage logués;
  `SetWindowRgn` : sur échec, `DeleteObject(hrgn)` (le système ne prend
  l'ownership qu'en cas de succès) + signature `m_regionCuts` vidée pour
  re-tenter au prochain rebuild. Aucune touche routée (plan 004).
- **Étape 8 — extraction prudente** : `wxWinUISlotLifetime` et l'état de
  slot vivent déjà dans le header privé `tlwhost.h` (extraction de fait
  réalisée aux passes précédentes); le complément utile est le nouveau
  `include/wx/winui/private/tlwhostmsw.h` — les 3 points d'entrée MSW
  (`ResolveFocus`, `NotifyThaw`, `NotifyEnable`) déclarés sans tirer la
  projection C++/WinRT, remplaçant les 3 `extern` locaux de window.cpp
  et toplevel.cpp (réserve de revue). Déplacer davantage de code hors de
  tlwhost.cpp serait du churn qui compliquerait la revue du diff
  cumulatif : proposé tel quel à l'arbitrage.

Dette connue reconduite : `Button::Click` (SendInput) reste rouge —
enregistrée comme dette du plan 007a, la campagne d'entrée n'est PAS
déclarée verte.

Gates finales de la passe B (2026-07-22, séquentielles) :

- build `test_gui widgets minimal` Release : exit 0 (trois incréments
  buildés/testés séparément; un rouge intermédiaire — lifetime du host
  retenu par la finalisation différée de l'arbre — corrigé par la
  révocation explicite des root handlers, voir ci-dessus);
- **`[HostLifecycle]` : 7 cas, 163 assertions, exit 0**;
- **rejeu ×100 : 100/100 green** (arrêt au premier échec, aucun);
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : rouge préexistant identique (dette 007a).

Toutes les étapes 1-8 du plan sont implémentées. Diff cumulatif de
revue : `plans/winui3-v0/tlwhost-cumulative-diff.patch` (4314 lignes,
depuis `20207bdbbb` — tlwhost.h/.cpp/tlwhostmsw.h intégraux + deltas
xamlhost/window/toplevel/winui/tests/CMake). Row 003 maintenue
IN PROGRESS : la clôture DONE appartient à la revue du diff cumulatif.
Rien de cette passe n'est commité.

Verdict de revue sur la passe B : étapes 1-3, 6, 7 acceptées (2 sous
réserve de la revue du cumulatif réel); **étape 4 refusée**; étape 5
« direction acceptée, critères/tests incomplets »; étape 8 pas
clôturable; plan IN REVIEW, pas de plan 004, méthode test-first imposée.

### Passe C (2026-07-22) — état/focus test-first, carrier, resolver

**Phase rouge (constatée avant toute correction)** — nouveaux tests
`[HostState]` + renforcement `XamlHostSharesTheIsland` :

- `PreferredRadioBoxFocus` : ROUGE `"wxtest-rb0" == "wxtest-rb2"` — le
  fallback focusait le premier élément, pas l'item sélectionné.
- `TooltipState` (sur `wxWinUIXamlHost`, qui n'a aucune impl dédiée) :
  ROUGE — un `SetToolTip()` sans événement géométrique n'atteignait
  jamais le peer.
- `IntrinsicContentStatePreserved` : ROUGE ×2 — l'adapter passe B posait
  `IsEnabled` sur le CONTENU : un `<Button IsEnabled="False"/>` d'un
  XamlHost était force-enabled dès le premier flush, et après un
  disable/re-enable wx.
- `EffectiveDisableSimpleControl` : vert d'emblée (contenu Control +
  seam WM_ENABLE).
- `EffectiveDisableComposite` : vert d'emblée — enseignement consigné :
  wx propage `DoEnable` récursivement et l'impl wxRadioBox applique
  `enable && m_itemEnabled[i]` à chaque bouton; le trou réel du contrat
  était l'écrasement d'état intrinsèque (test Intrinsic ci-dessus).
- `AutomationState` : vert par effet de bord (le relayout d'un SetLabel
  déclenchait un flush) — rendu déterministe par le hook WM_SETTEXT.
- Incident d'infrastructure corrigé : les RadioButton ne sont pas
  enfants directs du root (label de groupe + grille interne) —
  collecte récursive dans le test.

**Corrections architecturales** :

- **Carrier** : chaque slot intercale un `ContentControl` host-owned
  (`m_carrier`) entre le container et le contenu — `IsTabStop(false)`,
  `AccessibilityView::Raw` (aucun niveau UIA ajouté). L'enabled effectif
  se pose sur le carrier, JAMAIS sur le contenu : hérité par tous les
  descendants (clavier + UIA compris, contenus composites inclus), état
  intrinsèque du contenu préservé.
- **`wxWinUISlot::SetContent` transactionnel** : détache l'ancien, RESET
  de tous les caches par-contenu (`m_lastEnabled`, tooltip, nom UIA),
  installe le nouveau, cohérent sur échec (contenu nul + caches reset),
  puis `MarkDirty` via le lifetime — le nouveau contenu reçoit l'état
  complet au flush suivant.
- **Cible de focus préférée** : `m_preferredFocus`
  (`std::function<UIElement()>`) sur le slot + API
  `SetSlotPreferredFocus`; `FocusSlot` l'essaie d'abord, puis le contenu
  Control, puis le premier focusable; transférée à la migration
  inter-TLW; libérée par `DisconnectSlot`. wxRadioBox enregistre un
  resolver qui relit `GetSelection()` à l'appel (survit aux rebuilds
  d'items).
- **Seam d'état sans géométrie** : `wxWinUITLWHostNotifySlotState`
  (tlwhostmsw.h) appelé de `DoSetToolTip` (chemin winui) et d'un case
  `WM_SETTEXT` non consommé de `MSWWindowProc` (le label alimente le nom
  UIA) → `MarkDirty` du seul slot concerné.
- **`SetContentFromXaml` commit-après-succès** : chargement dans une
  locale, `m_impl->content` remplacé seulement après le succès du
  register/swap.
- Cursor : chemin déjà relié (`SetCursor` → `wxWinUISetWindowCursor` →
  `SetSlotCursor`), désormais prouvé par compteur
  (`GetSlotCursorSetCount`) dans `AutomationState`.
- Fix de liaison DLL : la logique thaw passe par une méthode publique
  `NotifyThaw()` (plus de friend), définitions des points d'entrée
  préfixées `WXDLLIMPEXP_CORE` (C4273 éliminé).

**Phase verte** : build sans warning; **`[HostState]` : 6 cas, 61
assertions, exit 0**; **`[HostLifecycle]` : 7 cas, 184 assertions,
exit 0** (XamlHost renforcé : compteur DIRECT de bridges par
`EnumChildWindows`/classe `DesktopChildSiteBridge` == 1, slot
réellement vidé, nouveau type réellement installé, tooltip re-poussé
sur chaque nouveau contenu, ×3 swaps); `git diff --check` exit 0.

Gates finales de la passe C (2026-07-22, séquentielles) :

- **rejeu `[HostLifecycle],[HostState]` ×100 (13 cas/run) : 100/100
  green** (arrêt au premier échec, aucun);
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : rouge préexistant identique. Conformément au STOP de
  revue, la gate n'est PAS déplacée unilatéralement : le plan 003 exige
  « tests lifecycle/Window/Button verts », la réparer = implémenter le
  pipeline pointer/hit-test SendInput (périmètre du plan 007a entier) —
  décision demandée à l'utilisateur : modifier formellement le plan 003
  (gate transférée à 007a) ou exiger la réparation dans 003.
- `git diff --check` : exit 0.

Diff cumulatif de revue régénéré (passe C incluse, radiobox.cpp ajouté
au périmètre) : `plans/winui3-v0/tlwhost-cumulative-diff.patch`, 4902
lignes depuis `20207bdbbb`. Décisions restantes demandées à
l'utilisateur : (1) la gate Button::Click ci-dessus; (2) acter
formellement l'étape 8 « extraction supplémentaire N/A » (le
Lifetime/SlotState vit déjà dans le header privé + tlwhostmsw.h) ou
exiger une extraction de code supplémentaire. Row 003 : IN REVIEW.
Rien n'est commité.

Verdict de revue sur la passe C : les deux arbitrages ACTÉS —
Button::Click transféré formellement au plan 007a (dans 003 : témoin de
non-régression, échec connu `clicked.GetCount() == 0` attendu, tout
autre échec = régression; critère « vert avec SendInput réel » ajouté au
Done de 007a) et étape 8 « satisfaite par tlwhost.h privé +
tlwhostmsw.h » (textes des plans 003/007a mis à jour). Restaient 3
défauts de code + 1 défaut de preuve.

### Passe D (2026-07-22) — stretch, transaction réelle, focus sûr, focusability

**Phase rouge (constatée)** :

- `CarrierStretchesContent` : ROUGE ×4 — les alignements par défaut du
  ContentControl (Left/Top) laissaient un contenu sans taille collé au
  coin du slot (`ActualWidth/Height` ≠ taille du slot).
- `KeyboardFocusExclusion` : ROUGE — `GetNextDlgTabItem` n'honore que
  WS_TABSTOP : Tab focusait un contrôle `DisableFocusFromKeyboard()`.
- `FocusReentrancyUnregister` : scénario d'abord non déclenché (une TLW
  redélègue le focus reçu à un enfant — `DoRestoreLastFocus`) →
  porteur de focus corrigé en wxWindow nu; le cas caractérise le
  désenregistrement du slot PENDANT le `::SetFocus(bridge)` de
  `FocusSlot` (frontière réentrante).
- `EffectiveDisableArbitraryComposite` (StackPanel Button+CheckBox via
  XamlHost, rien ne propage l'état par lui-même) : VERT — prouve
  l'héritage du carrier indépendamment de wxRadioBox.
- `XamlAuthoredNamePreserved` (AutomationProperties.Name posé DANS le
  XAML avant le register) : VERT — jamais écrasé, y compris après un
  flush ultérieur.
- `ContentSwapFaultKeepsOldContent` : écrit contre la nouvelle API
  transactionnelle (l'ancienne retournait void — rouge non exécutable,
  documenté).

**Corrections** :

- **Stretch** : `HorizontalContentAlignment`/`VerticalContentAlignment`
  = Stretch posés sur le carrier à la création.
- **Transaction réelle** : `wxWinUISlot::SetContent()` retourne bool;
  l'ancien contenu et les caches SURVIVENT jusqu'au succès; sur échec
  d'installation, l'ancien contenu est RESTAURÉ (rien ne change); si
  même la restauration échoue, le slot se déclare vide (caches reset)
  au lieu de mentir. Helpers privés `ResetContentCaches`/`NudgeDirty`.
  Le re-register d'un slot existant propage l'échec (RegisterSlot →
  nullptr, slot inchangé); `ClearContent()` ne vide `m_impl->content`
  qu'après le succès du slot; `SetContentFromXaml` était déjà
  commit-après-succès.
- **Focus sûr** : `FocusSlot` tient le lifetime invalidable à travers
  `::SetFocus(bridge)` (frontière réentrante : WM_KILLFOCUS synchrone
  peut détruire/désenregistrer la cible), revalide
  (`IsValid`/même host/`FindSlot`) avant tout accès, et
  `m_forwardingFocus` est tenu par un garde RAII (reset sur toute
  sortie, exceptions comprises).
- **Cible préférée sans capture** : `m_preferredFocus` devient une
  `winrt::weak_ref<UIElement>` — plus AUCUN rappel vers wx; wxRadioBox
  POUSSE la cible (weak) à chaque changement : fin de rebuild,
  `SetSelection()`, handlers `Checked` (les clics utilisateur compris).
- **Focusability** : `wxWinUI3ProcessTabNavigation` boucle sur
  `GetNextDlgTabItem` en sautant les fenêtres dont
  `AcceptsFocusFromKeyboard()` est faux (borné par le retour au point de
  départ) — seam commun, zéro override par composant.
- Commentaire de `BindRootRouter` corrigé (les root handlers SONT
  révoqués par Shutdown).

**Phase verte** : build 3 cibles exit 0 sans warning; **`[HostState]` :
12 cas, 104 assertions, exit 0**; **`[HostLifecycle]` : 7 cas, 184
assertions, exit 0**.

Le patch cumulatif est régénéré en VRAI complet : `git add -N` (intent
to add) rend l'untracked `tlwhostmsw.h` diffable — **6452 lignes**,
création de `include/wx/winui/private/tlwhostmsw.h` incluse, périmètre
étendu à `ctrlhost.cpp` (fix Tab); l'index est restauré après coup
(header à nouveau untracked pur).

Gates finales de la passe D (2026-07-22, séquentielles) :

- build `test_gui widgets minimal` Release : exit 0, zéro warning;
- **`[HostLifecycle]` : 7 cas, 184 assertions, exit 0**;
- **`[HostState]` : 12 cas, 104 assertions, exit 0**;
- **rejeu des deux suites ×100 (19 cas/run) : 100/100 green**;
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : témoin de non-régression conforme au contrat acté —
  signature EXACTE de l'échec connu (`buttontest.cpp(73)`,
  `clicked.GetCount() == 1` avec count 0), sans crash, sans nouvelle
  assertion;
- `git diff --check` : exit 0.

Tous les critères Done du plan 003 (dans leur rédaction amendée par les
deux arbitrages actés) sont couverts et prouvés. Row 003 maintenue
IN REVIEW : la bascule DONE appartient à la revue du patch cumulatif
complet (6452 lignes). Rien n'est commité; plan 004 non commencé.

Verdict de revue sur la passe D : architecture et étapes validées, le
défaut de preuve du patch résolu (la copie complète a bien été reçue);
dernière passe corrective locale exigée : chemins de focusability
unifiés, transaction de contenu mécanique phase par phase, garde RAII
restaurant, nettoyages.

### Passe E (2026-07-22) — unification navigation, transaction 3 phases

- **Helper commun `wxWinUIFindNextKeyboardFocusable`** (private.h,
  ctrlhost.cpp) : `GetNextDlgTabItem` + saut des fenêtres refusant
  `AcceptsFocusFromKeyboard()`, borné par le retour au PREMIER candidat
  observé (pas seulement `hwndCurrent` — une énumération intégralement
  refusée dont le départ n'est pas membre ne peut plus cycler), deux
  directions, null si tout est refusé. Utilisé par
  `wxWinUI3ProcessTabNavigation` ET `OnTakeFocusRequested` (qui pouvait
  auparavant remettre le focus sur un contrôle exclu).
- **`wxWinUISlot::SetContent` en 3 phases mécaniques** : détachement
  (échec → RIEN de modifié, modèle et caches intacts), installation
  (échec → restauration de l'ancien), restauration (échec → le modèle
  est resynchronisé sur le contenu RÉEL du carrier via
  `SyncContentFromCarrier`/`GetCarrierContent` — jamais un nullptr
  arbitraire). Seam d'injection `TestFailContentSwap(mask)` (bits
  Detach/Install/Restore, auto-consommés). Test
  `ContentSwapFaultPhases` : les 3 pannes vérifient la cohérence
  modèle==carrier, puis le slot reste utilisable (swap propre + état
  re-poussé).
- **`ForwardingGuard` restaurant** : la valeur PRÉCÉDENTE de
  `m_forwardingFocus` est restaurée (un `FocusSlot` imbriqué ne remet
  plus le flag à false sous un appel extérieur actif).
- **Deux bugs produit débusqués par les tests de navigation** (les
  gates intermédiaires rouges et le black-box `WX_WINUI_INPUT_LOG` — le
  déroulé réel a été tracé, pas deviné) :
  1. `FocusSlot` faisait un `::SetFocus(bridge)` INCONDITIONNEL : quand
     l'île détient déjà le focus natif (porté par l'inner
     InputSiteWindow), cela le DÉGRADAIT inner→bridge, déclenchant des
     LostFocus parasites et fragilisant le focus programmatique. Le
     focus natif n'est plus pris que s'il est HORS de l'île.
  2. **Stale-guard de l'arbitre** : des GotFocus/LostFocus XAML arrivant
     une génération de dispatch en retard écrasaient `m_focusOwner`
     après qu'un `FocusSlot` plus récent avait désigné une autre cible
     (un refocus rapide corrompait l'arbitre en production). Les deux
     handlers ne prennent/lâchent l'ownership que si le focus XAML réel
     est/n'est plus dans le slot (`wxWinUIFocusIsInside`, marche
     d'ancêtres depuis `FocusManager::GetFocusedElement`).
- Tests de navigation réécrits sur les chemins RÉELS : Tab forward par
  `wxGUIEventLoop::ProcessMessage` (un message posté n'est pas pompé
  fiablement sous wxYield — leçon consignée), backward et double-refus
  par le seam `TestTakeFocusRequested` (un VK_SHIFT posté n'atteint
  jamais `GetKeyState`), tous-refusés borné, chemin
  `OnTakeFocusRequested` couvert.
- Nettoyages de revue : include `<functional>` retiré (weak_ref),
  bloc PowerShell vide du plan 003 retiré, commentaire du root router
  corrigé, logs focus ajoutés au black-box existant (coût nul désarmé).

Gates finales de la passe E (2026-07-22, séquentielles) :

- build `test_gui widgets minimal` Release : exit 0, zéro warning;
- **`[HostLifecycle]` : 7 cas, 184 assertions, exit 0**;
- **`[HostState]` : 14 cas, 129 assertions, exit 0**;
- **rejeu des deux suites ×100 (21 cas/run) : 100/100 green**;
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : signature EXACTE du témoin (`buttontest.cpp(73)`,
  `clicked.GetCount() == 1` échoue avec count 0), sans crash — conforme
  au contrat de non-régression;
- `git diff --check` : exit 0.

**Plan 003 : DONE** (2026-07-22) — tous les critères du texte du plan,
dans leur rédaction amendée par les arbitrages actés, sont cochés et
prouvés; row basculée dans plans/README.md. Le patch cumulatif final est
régénéré (même méthode add -N); il reste un artefact de revue et ne doit
PAS faire partie des commits de production. Aucun commit effectué; le
découpage est proposé dans le rapport de session. Le plan 004 peut
commencer sur instruction.

Verdict de revue sur la passe E : navigation, transaction du slot,
lifetime/réentrance, stale-guards acceptés; retour temporaire IN REVIEW
pour deux divergences de modèle : la transaction contournée par
`wxWinUIControlHost` (la couche par laquelle passent tous les composants
ordinaires) et la publication prématurée de `m_focusOwner`.

### Passe F (2026-07-22) — control host transactionnel, owner sur focus réel

- **`wxWinUIControlHost::SetContent` transactionnel au niveau appelant** :
  le hook Loaded du NOUVEAU contenu est posé dans un token local (avant
  le swap, pour ne rater aucun Loaded); le résultat de `RegisterSlot`
  est vérifié; sur échec, seul le nouveau hook est révoqué et l'ancien
  modèle (contenu + hook + état loaded) reste intégralement en place —
  cohérent avec le slot qui a conservé/restauré l'ancien élément; sur
  succès, l'ancien hook est retiré PUIS le nouveau modèle est commité.
  `ClearContent()` vérifie le bool de `slot->SetContent(nullptr)` et ne
  vide son modèle qu'après succès. (Note d'honnêteté :
  `wxWinUIControlHost::ClearContent` n'a actuellement AUCUN appelant —
  le contrat de la même séquence est testé par le chemin public
  `wxWinUIXamlHost::ClearContent`.)
- **Seam compteur `GetLiveLoadedHookCount`** (défini dans ctrlhost.cpp,
  exposé par le header d'introspection) : équilibre des hooks Loaded
  vivants — un swap échoué laisse le compte net inchangé (ancien
  conservé, nouveau retiré).
- **`TryGetCarrierContent(UIElement&)`** remplace `GetCarrierContent` :
  distingue « carrier lisiblement vide » (true + null) d'« illisible »
  (false). `SyncContentFromCarrier` (chemin restore-échec) : si la
  lecture échoue, le slot est NEUTRALISÉ (lifetime invalidé — plus
  aucun callback vers wx) au lieu d'être déclaré vide par supposition.
- **`m_focusOwner` publié seulement sur focus réel** : l'affectation
  d'entrée de `FocusSlot` est retirée; l'ownership n'est posé qu'après
  `Focus()==true` ET `wxWinUIFocusIsInside()==true` (les handlers
  GotFocus stale-guardés restant l'autre voie légitime). Sur échec
  complet, l'ancien propriétaire reste — son élément détient toujours
  le focus réel.
- Tests : `ControlHostContentTransaction` (échec d'install via
  `wxRadioBox::SetString` → slot == carrier == ancien root, host
  toujours propriétaire, balance des hooks fermée, puis un rebuild
  propre remplace réellement); `FocusOwnerOnlyOnRealFocus` (slot Border
  non focusable et slot désactivé : `GetFocusOwner` ET
  `ResolveFocusHwnd` restent sur le vrai propriétaire);
  `ContentSwapFaultPhases` adapté à `TryGetCarrierContent`.

Gates finales de la passe F (2026-07-22, séquentielles) :

- build `test_gui widgets minimal` Release : exit 0, zéro warning;
- **`[HostState]` : 16 cas, 151 assertions, exit 0**;
- **`[HostLifecycle]` : 7 cas, 184 assertions, exit 0**;
- **rejeu des deux suites ×100 (23 cas/run) : 100/100 green**;
- **`[window]` : 26 cas, 124 assertions, exit 0**;
- `Button::Click` : signature EXACTE du témoin (`buttontest.cpp(73)`,
  count 0, sans crash) — conforme;
- `git diff --check` : exit 0.

Verdict de revue sur la passe F : les deux divergences de modèle
acceptées; retour temporaire IN REVIEW pour deux défauts locaux —
`wxWinUIControlHost::ClearContent` fuit son hook `Loaded`, et le slot
« neutralisé » (carrier illisible) devient un zombie que la
réinscription ne reconstruit pas.

### Passe G (2026-07-22) — fuite du hook ClearContent, slot zombie

- **`wxWinUIControlHost::ClearContent` ne fuit plus le hook** : après un
  détachement RÉUSSI du slot, le hook `Loaded` de l'ancien contenu est
  révoqué (sa lambda capture `this`), `gs_liveLoadedHooks` décrémenté
  une fois, `m_loadedToken` remis à `{}`, `m_contentLoaded` à false.
  Un détachement échoué ne touche RIEN. Sans ce correctif, `Close()`
  n'aurait plus retiré le handler (il ne révoque que si `m_content` est
  non nul), laissant un delegate rappeler un control host détruit.
- **Slot zombie reconstruit** : nouvel état `m_poisoned` sur le slot;
  `SyncContentFromCarrier` le pose (et invalide le lifetime) quand
  `TryGetCarrierContent` échoue. `RegisterSlot`, sur un slot existant
  EMPOISONNÉ ou à lifetime invalide, le DÉTRUIT (unbind + DisconnectSlot
  + delete) et retombe sur la création d'un slot neuf (nouveau lifetime,
  nouveaux handlers) au lieu de faire un `SetContent` sur un cadavre dont
  tous les handlers pointent le lifetime mort. Le commentaire mensonger
  « next re-registration rebuilds it » est corrigé et rendu vrai.
- **Injection `TestContentFault_Read = 8`** dans `TryGetCarrierContent`,
  pour piloter l'échec de lecture du carrier.
- **Seam de test `wxWinUIControlHostProbe`** (exporté par le header
  d'introspection, implémenté dans ctrlhost.cpp) : `wxWinUIControlHost`
  étant interne et sans appelant de `ClearContent`, le probe crée un
  vrai control host sur une fenêtre, fait SetContent/ClearContent/Close,
  observés via `FindSlot`/`GetContent`/`GetLiveLoadedHookCount`.
- Tests : `ControlHostClearContent` (clear réussi → hook révoqué (retour
  baseline), slot + carrier vides; detach échoué → tout conservé;
  récupération; fermeture → dernier hook retiré);
  `PoisonedSlotRebuilt` (install+restore+read en échec → set retourne
  false; la réinscription suivante reconstruit un slot neuf, focus +
  tooltip fonctionnels, un seul lifetime vivant pour la fenêtre, balance
  adds==revokes). Note d'honnêteté : le scénario « ancien élément
  rechargé après destruction du host » n'est pas exécuté (un `Loaded`
  post-destruction n'est pas reproductible de façon déterministe en test
  unitaire); la preuve est le compteur de hooks revenu au niveau de
  base, qui atteste que la révocation a bien eu lieu.

### Passe H (2026-07-23) — reprise Codex, invariants finaux et revue croisée

Cette passe reprend la phase 003 après les passes A–G. Elle ne déclare pas
vert ce qui n'a pas pu être exécuté : l'environnement de processus fourni à
Codex s'identifie comme `CodexSandboxOffline` et
`MddBootstrapInitialize2()` échoue avec `0x80670016` avant la création du
premier host. Le code, les tests et les trois exécutables de gate compilent ;
le rejeu runtime reste donc le seul verrou externe.

#### Corrections d'architecture

- **Ownership transactionnel des slots** : registre autoritaire
  `window -> host`, bascule atomique pendant les migrations et résolution de
  l'owner réel après toute frontière réentrante. Une migration A→B peut être
  supersédée pendant `Loaded` par B→C, par un remplacement de contenu dans C
  ou par les deux : C reste l'unique owner, A/B sont nettoyés, et les caches
  liés à l'ancienne génération ne sont transférés que si le couple
  slot/contenu attendu est encore installé.
- **Reparentage multi-host** : la notification ne suppose plus que le TLW
  précédent possède tous les slots du sous-arbre. Elle photographie les
  lifetimes de tous les hosts, privilégie l'ancien host, puis revalide un
  `wxWeakRef` du sous-arbre avant chaque callback. Un échec A→B suivi
  immédiatement de B→C est reconcilié ; une destruction depuis `Loaded`
  interrompt proprement la tournée sans UAF.
- **Reparentage dans le même TLW** : seules les subscriptions d'ancêtres sont
  rafraîchies. Les handlers directs et `m_focusOwner` restent intacts tant que
  le focus XAML réel n'a pas changé.
- **Transaction de contenu** : lecture du carrier après échec, slot empoisonné
  reconstruit, modèle du control host commité seulement après le swap,
  révocation du hook `Loaded` équilibrée et opérations host/slot protégées
  contre une suppression depuis un callback imbriqué.
- **Focus** : arbitre origin-aware, ordre wx
  `KILL(old,new) -> CHILD(new) -> SET(new,old)`, stale guards XAML et
  revalidation après chaque callback applicatif. Le transfert natif
  shell-HWND→bridge consécutif à `wxWindow::SetFocus()` est marqué par un
  garde synchrone/nestable ; `HandleKillFocus()` ne publie plus un faux
  `KILL_FOCUS` pour la fenêtre qui vient de recevoir le focus.
- **État commun** : enabled effectif sur le carrier, tooltip avec restitution
  exacte du `IInspectable` XAML, nom UIA avec ownership explicite et
  relinquishment irréversible par génération (une chaîne vide posée par
  l'application reste vide), curseur hérité conservant le `wxCursor` et
  conversion des HCURSOR custom via l'interop WinAppSDK.
- **API Win32** : installation/subclass/hook contrôlés, hook GETMESSAGE
  quarantiné après un unhook incertain, retry borné du subclass inner,
  `SetWindowRgn` propriétaire du HRGN seulement après succès et cache RTL
  exact. Le routeur pointeur revalide host, HWND et appartenance au TLW après
  chaque `SendMessage` applicatif et avant chaque `PostMessage`; après
  `ReleaseCapture`, toute capture reprise supprime le forward, et une capture
  étrangère initiale n'est jamais relâchée.
- **Freeze/relayout** : un seul rattrapage au Thaw, aucune boucle CallAfter en
  freeze ; la traversée de relayout photographie des `wxWeakRef` et revalide
  parent/enfant après chaque frontière applicative.

#### Tests ajoutés ou renforcés

- 41 cas `[HostLifecycle]` + `[HostState]` compilés, dont :
  migrations A→B→C, échec puis reparent immédiat, migration + remplacement
  de contenu dans le même callback, destruction du sous-arbre pendant la
  tournée multi-host, swap fauté et slot empoisonné ;
- contrat de focus par le vrai `wxWindow::SetFocus()`, avec observation
  explicite de tout `kill-second` parasite ;
- noms UIA fournis par wx ou authored en XAML puis vidés par l'application ;
- tooltip authored/restauré par identité, mutation et unregister réentrants ;
- curseur hérité et HCURSOR bitmap custom ;
- `DrainToQuiescence()` échoue désormais explicitement après 50 tours au lieu
  de masquer une boucle de scheduling.

#### Revue contradictoire

Trois reviewers indépendants ont relu le diff final par périmètre :

1. transactions/lifetimes/migrations ;
2. état commun/focus/tests ;
3. hooks/subclasses/capture/régions et routage Win32.

Leurs premières passes ont remonté les scénarios réentrants ci-dessus ; chaque
défaut a reçu un correctif et un test. Après les rechecks, les trois verdicts
finaux sont **zéro P0/P1 restant dans le scope 003**. La dette de priorité
effective du curseur dans les descendants templated reste attribuée au plan
007a, pas masquée dans 003.

#### Gates de cette passe

- build Release `test_gui` : **exit 0**, payload runtime déployé ;
- build Release `widgets minimal` : **exit 0**, payloads déployés ;
- `ctest -R wx_winui_selftest_` : **2/2 Passed** ;
- `git diff --check` : **exit 0** (avertissements CRLF et global ignore
  inaccessible uniquement) ;
- tentative `[HostLifecycle],[HostState]` : le runner démarre, annonce
  `Windows 11 ... as CodexSandboxOffline`, puis les cas échouent tous avant le
  premier host ; diagnostic terminal unique :
  `MddBootstrapInitialize2 failed with HRESULT 0x80670016`.
- lancement via le pont graphique externe refusé (`minimal` non approuvé) et
  aucun device Desktop Commander connecté : aucun contournement de sécurité
  n'a été utilisé.

**Statut honnête** : implémentation 003 et revue terminées ; row maintenue
`IN REVIEW` et dernier checkbox runtime rouvert jusqu'au rejeu hors sandbox.
Le patch cumulatif non suivi est un artefact de revue ancien : ne pas le
committer comme source de production.

## Plan 004 — clavier, accélérateurs et contrats des menus (2026-07-23)

### Audit et architecture retenue

L'audit initial a confirmé que le hook `WH_GETMESSAGE` livrait les messages à
`ContentPreTranslateMessage()` avant les tables wx, puis remplaçait le message
par `WM_NULL`. Les accélérateurs existaient donc bien, mais n'avaient plus la
possibilité de s'exécuter lorsque le focus appartenait à l'île. Les menus XAML
avaient en plus divergé du contrat wx : projection avant OPEN, routage direct
au frame, états partiels et boucles imbriquées non bornées sur certains échecs.

La solution n'ajoute aucune deuxième table `KeyboardAccelerator` XAML. Un
pipeline unique traite désormais, dans cet ordre :

1. veto différé `wxEVT_CHAR_HOOK`/IME (`WM_KEYDOWN` et `WM_SYSKEYDOWN`) ;
2. frontière Tab minimale et fenêtre critique ;
3. filtres locaux du contrôle puis accélérateur/mnémonique wx ;
4. `ContentPreTranslateMessage()` exactement une fois.

La résolution distingue trois destinations : HWND d'île (résultat wx puis
résultat XAML honorés), HWND wx natif (wx honoré, XAML seulement pompé) et HWND
réellement étranger (XAML pompé sans pouvoir voler le message). Le fallback de
la boucle wx appelle le même corps lorsque le hook est absent. AltGr
(`Ctrl+RightAlt`), caractères, dead chars et `WM_SYSCHAR` conservent des voies
séparées ; TextCtrl, Choice et ComboBox gardent leurs raccourcis locaux.

### Contrat menu restauré

- projection après `wxEVT_MENU_OPEN`, avec mutations de labels, structure,
  enabled/check/radio, bitmap, access key et texte d'accélérateur visibles à
  chaque ouverture ;
- commandes routées par `wxMenu::MSWCommand()` avec le menu propriétaire exact,
  donc chaîne canonique menu → parent → menubar → frame et IDs dupliqués
  désambiguïsés ;
- CLOSE équilibré avec le bon `wxMenu*`, descendants fermés dans l'ordre et
  génération de projection bornée ;
- MenuBar remplacée transactionnellement : publication après `SetContent()`,
  génération côté frame, retrait différé et revalidation après chaque handler
  réentrant ;
- popup considéré consommé dès le début de OPEN : une exception post-OPEN ne
  peut pas déclencher une deuxième session native ;
- `TryEnqueue(false)`, échec des deux timers, destruction planifiée de
  l'invoker et arrêt du dispatcher ont tous une sortie bornée ;
- `EnableTop()`, items/submenus disabled et tables HACCEL filtrées restent
  cohérents.

### Tests ajoutés

`[WinUIKeyboard]` contient 15 cas : ordre des stages, AltGr, caractères/dead
chars, Tab/Alt-Tab, veto IME key/syskey, raccourcis TextEntry/Choice/ComboBox,
HWND île/wx/étranger, fenêtre critique, destruction depuis commande, hook
PM_REMOVE/PM_NOREMOVE, fallback, séquence `WM_KEYDOWN → WM_CHAR` sans double
livraison, et matrice TextBox/ComboBox/Button/Grid réel (`wxWinUIXamlHost`) avec
F6, Ctrl-T et Alt-M. L'état clavier injecté efface les modificateurs physiques
parasites. Un gate physique envoie Ctrl-T puis une unique touche `E`
QWERTY/AZERTY-stable et attend les résultats avec une deadline.

`[WinUIMenu]` contient 9 cas : OPEN avant projection, routage canonique,
snapshots check/radio/bitmap/accelerator, top et submenu disabled, cycle
AutomationPeer top+submenu, projection bornée, remplacements réentrants,
suppression pendant CLOSE descendant, échecs timer/drain, destruction pendant
OPEN et exception WinRT post-OPEN sans fallback natif.

Les tests MSW de table HACCEL couvrent séparément item, submenu et top disabled
avec activation/désactivation à chaud.

### Revue contradictoire

Trois reviewers ont relu indépendamment :

1. pipeline Win32, IME, HWND/Tab et tests clavier ;
2. projection/états/focus des menus et AutomationPeer ;
3. conformité intégrale de la matrice au plan 004.

Les premières passes ont notamment trouvé : veto IME absent sur SYSKEYDOWN,
perte des accélérateurs pour les HWND wx natifs dans les boucles natives,
parent-walk volant Tab aux HWND étrangers, réentrance après CLOSE descendant,
fallback natif après exception OPEN, faux slot Grid dans le test, état clavier
non normalisé et attente physique trop courte. Chaque point a été corrigé et
couvert. Verdict final des trois rechecks : **zéro P0/P1 restant dans le scope
004**. Le Tab mixte approfondi reste explicitement au plan 007b.

### Gates exécutés

- compilation ciblée de `winui.cpp`, `evtloop.cpp`, `ctrlhost.cpp`,
  `menubar.cpp`, `winuikeyboard.cpp`, `winuimenubar.cpp` : **exit 0, zéro
  warning final** ;
- build Release `test_gui widgets showcase` : **exit 0**, payload déployé ;
- `[accelentry]` : **4 cas, 195 assertions, exit 0** ;
- `[menu]` historique hors nouveaux cas WinUI : **2 cas, 176 assertions,
  exit 0** ;
- self-tests CMake SDK/cache NuGet : **2/2 Passed** ;
- `git diff --check` : **exit 0** (warnings CRLF/global ignore uniquement).

Gates runtime non déclarés verts :

- `[WinUIKeyboard]` recense 15 cas ; les 8 cas sans host passent, les 7 qui
  créent un contrôle/host échouent à l'initialisation avec le diagnostic unique
  `MddBootstrapInitialize2 failed with HRESULT 0x80670016` ;
- `[WinUIMenu]` : les 9 cas s'arrêtent avant la création de la MenuBar/popup
  pour le même diagnostic ;
- `[KeyboardEvent]` physique : aucun événement reçu car la fenêtre WinUI ne
  peut pas être initialisée dans le sandbox ;
- `wx_winui_runtime_smoke` échoue au bootstrap avec `0x80670016`, tandis que
  les deux self-tests non-runtime passent.

**Statut honnête** : implémentation 004 complète, build et revues verts ; row
maintenue `IN REVIEW` jusqu'au rejeu des gates physiques hors
`CodexSandboxOffline`. Aucun commit ni staging effectué.

## Plan 005 — broker OLE du bridge (archive du 2026-07-24)

> Cette section conserve la première implémentation et ses chiffres comme
> provenance. Ses affirmations « broker par bridge » et « probe physique non
> exécuté » sont closes par la révision TLW et les preuves du 2026-08-21
> consignées à la fin de ce document ; elles ne décrivent plus l'état courant.

- Session OLE factorisée et broker par bridge avec ownership exact
  `CoLockObjectExternal` / `RegisterDragDrop` / `RevokeDragDrop`.
- Routage déterministe texte/fichiers, effets copy/move/link/none,
  transitions de cibles, destruction depuis `OnEnter`/`OnData`, redirection
  composite, reparentage inter-TLW et `DragAcceptFiles` par le bridge.
- Build séparé `wxUSE_DRAG_AND_DROP=OFF` vert après correction des gardes
  `wxUSE_DATAOBJ` et du stub `HandleDropFiles`.
- Gate runtime de propriété bridge/InputSite verte. Le probe de livraison
  physique est armé explicitement, sans clic ni déplacement automatique,
  avec timeout coopératif 2,5 s puis grâce bornée 5 s et sortie dure 20 ; il
  n'a pas été exécuté afin de ne pas interférer avec l'utilisation de la souris.

Gates exécutées :

- `[winui-drop-broker],[msw-drop-session]` : **17 cas, 332 assertions,
  exit 0** ;
- reproducer cross-TLW/RTL après correction du retry focus : **exit 0** ;
- revue finale du probe et du broker : **0 P0 / 0 P1** ;
- `git diff --check` : **exit 0**.

Statut : **IN REVIEW**, uniquement pour la livraison OLE physique/DPI.

## Plan 007b — focus, Tab et curseurs (2026-07-24)

- Arbitre focus générationnel, preferred target faible, revalidation de
  l'autorité native après chaque frontière et révocation sûre des callbacks
  Choice/Combo.
- Le retry différé de focus ne peut plus réactiver un ancien TLW : le bug
  reproduit par le test D&D cross-TLW faisait alterner deux hosts à 100 % CPU.
- Curseurs : cache keyed par point/géométrie/topologie/policy, popup hit-test,
  priorité busy/modal explicite, source InputPointerSource truthful, replays
  réentrants callback-free et verdict natif revalidé après `WM_SETCURSOR`.
- Trois attentes `[HostState]` périmées ont été réalignées sur le contrat
  transactionnel : identité d'un vrai objet ToolTip, reconstruction du slot
  empoisonné, et masque de fault injection sans bit global résiduel.

Gates exécutées :

- build Release `test_gui` : **exit 0** ;
- `[winui-cursor]` : **6 cas, 83 assertions, exit 0** ;
- focus ciblé : **8 cas, 65 assertions, exit 0** ;
- `[WinUIKeyboard]~[physical]` : **15 cas, 413 assertions, exit 0** ;
- `[HostState]` : **33 cas, 521 assertions, exit 0** avant l'ajout du
  reproducer multi-TLW, puis reproducer dédié + D&D cross-TLW :
  **2 cas, 24 assertions, exit 0** ;
- stress focus reentrant + destruction Combo : **20/20 process runs verts**
  (en plus de la campagne 100/100 antérieure) ;
- revues finales focus/curseur/coupe-circuit multi-TLW :
  **0 P0 / 0 P1**.

Statut : **DONE**. Aucun test `SendInput` ou déplacement physique de souris
n'a été exécuté pendant l'utilisation de la machine.

## Plan 006 — dialogues, transitoires et re-fit (2026-07-25)

### Contrat livré

- `wxWinUIDialogPresentation::Window` est le default unique documenté par
  l’ADR 0001 ; l’API explicite précède l’opt-in environnement
  `WX_WINUI_DIALOG_OVERLAY=1`.
- Le `TransientManager` est autoritaire par TLW, générationnel, à ownership
  faible, avec une seule lease modale et annulation centralisée des sessions.
- Le detach/restore d’owner est une transaction native vérifiée : identité et
  génération HWND, marqueur, rollback, minimize/restore, activation,
  destruction et dialogues imbriqués.
- Le presenter Window utilise des callback states partagés/faibles et révoque
  tous ses tokens. Enter/Escape/default/veto et le focus initial suivent le
  contrat frame-class sans dépendre du dialog manager Windows.
- Le focus initial tardif est borné : tentative immédiate, un retry après le
  flush, puis au plus un `Loaded` de la même génération. Aucun polling ni
  boucle `CallAfter`.
- Le re-fit générique est grow-only et générationnel après réalisation des
  templates ; message/text/colour dialogs et RichToolTip ont leurs contrats
  spécifiques, et les HWND coquilles n’exposent plus un doublon UIA.

### Gates et revue

- gate groupée
  `[winui-frame-dialog],[winui-dialog-contract],[winui-dialog-lifetime],`
  `[winui-dialog-session],[WinUITransient],[winui-rich-tooltip],`
  `[winui-dialog-uia],[modal]` :
  **48 cas, 686 assertions, exit 0** ;
- build Release `test_gui` : exit 0 ;
- revue finale focus/Loaded : **0 P0 / 0 P1** ;
- aucun test physique, `SendInput` ou déplacement de souris.

Statut : **DONE**.

## Plan 007c — Z-order, clips, scroll et splitter (2026-07-25)

### Architecture et corrections

- L’ordre XAML est dérivé du vrai sibling order USER32. Une topologie
  alternée non représentable (`XAML A > HWND N > XAML B`) est diagnostiquée
  exactement une fois au lieu d’être rendue silencieusement avec un faux ordre.
- Les clips accumulent ancestors, régions HWND, scroll viewport, visibilité,
  minimisation et bande TabView. Le clip initial Notebook est appliqué avant
  le premier `LayoutUpdated`; le focus n’est transféré lors d’un changement de
  page que si le Notebook possédait déjà le focus avant que la base masque la
  page précédente.
- Les mutations de structure et de Z sont générationnelles/réentrance-safe.
  Le splitter ne dépend plus d’une bande peinte en blanc et ses géométries
  horizontal/vertical sont exercées par le harness.
- La décision ADR 0003 conserve l’île unique et le flush coalescé : les mesures
  qualifiées ne démontrent aucun besoin de miroir ou de flush synchrone.

### Harnais déterministe

- Le scénario Z/clip/splitter utilise un TLW off-screen
  `WS_EX_NOACTIVATE`, sans lecture/déplacement du pointeur. Le timer de layout
  est possédé, borné et arrêté sur chaque sortie.
- Campagne finale :
  `phase007c-zorder-final-20260724-074439`, exit 0, neuf checks, zéro
  activation, diagnostic impossible exactement une fois.
- Campagne de stabilité précédente : **20/20** processus verts, signature de
  bande Notebook stable `40/40/150`.

### Contrat latence v2

- Warm-up fixe : quatre callbacks `Rendering`, puis un batch scroll non mesuré.
  Aucun outlier n’est supprimé en fonction de sa valeur.
- Une seule évaluation produit `pass/0`, `unqualified/2` ou `fail`; une cadence
  différente ne peut masquer ni défaut structurel, ni budget slot, ni délai
  supérieur à `2 * max(période cible, médiane observée)`.
- Le JSON v2 contient les intervalles render bruts. Le runner impose des types
  JSON stricts et recalcule samples, relations QPC, p95, médiane, cadence,
  maxima, budgets, safety, gates et statut.
- `Process.Start` est couvert jusqu’à la sortie par `try/catch/finally` sur
  l’objet exact. PID, orphan PID, hashes binaire/script, watchdog et timeout
  interne sont archivés ; le timeout interne reste borné à 60 s même lorsque
  le watchdog runner vaut 120 s.
- Profil strict 165 Hz :
  `phase007c-latency-v2-strict-165-20260725-231627`,
  **pass/0**, 24/24, cadence 164.636 Hz, p95 slot 4.952 ms, p95 rendu
  6.239 ms.
- Branche négative :
  `phase007c-latency-v2-strict-unqualified-60-20260725-231639`,
  **unqualified/2**, toutes les gates structure/slot/safety vertes.
- Dix campagnes 165 Hz antérieures sur dix sont vertes
  (164.53–165.10 Hz, slot p95 1.374–2.913 ms, rendu p95
  5.744–5.979 ms).
- Un run ultérieur pendant une charge desktop interactive a correctement
  produit `fail/43` (slot p95 14.205 ms). Il est conservé comme preuve que la
  gate ne masque pas la contention, mais ne remplace pas le profil qualifié
  exécuté sans cette interférence.

### Gates et revues

- `[winui-focus]` : **2 cas, 13 assertions, exit 0** ;
- `[winui-zorder]` : **8 cas, 66 assertions, exit 0** ;
- `[HostLifecycle]` : **28 cas, 503 assertions, exit 0** ;
- build Release `winuispike` et parser PowerShell : exit 0 ;
- revues finales cœur Z/clip, Notebook focus/clip, latence C++ et runner :
  **0 P0 / 0 P1**.

Statut : **IN REVIEW** uniquement pour l’inspection physique splitter
Light/Dark/High Contrast, DPI multi-écran et minimisation. L’implémentation et
la qualification automatisée sont terminées.

## Plan 008b — modèle stable des contrôles à items (2026-07-26)

### Contrat livré

- Un modèle wx à identités monotones est l’autorité pour Choice, ComboBox,
  ListBox, CheckListBox et BitmapComboBox. Les insertions, suppressions,
  renommages triés et déplacements conservent sélection, état coché, bitmap
  et client data par identité plutôt que par index mutable.
- Les peers sont mis à jour par deltas. Une divergence XAML détectée provoque
  une reconstruction transactionnelle avant la mutation suivante, sans
  `Items.Clear()` sur le chemin nominal.
- ComboBox possède un modèle texte indépendant, une résolution générationnelle
  de son TextBox de template, une taille minimale extensible aux valeurs/items
  longs et l’ordre d’événements `COMBOBOX` puis `TEXT`. Une mutation réentrante
  du modèle dans le premier handler ne réutilise jamais un index/client data
  périmé dans le second.
- ListBox/CheckListBox partagent le même modèle, une sélection XAML réelle,
  un `SetFirstItem()` distinct d’`EnsureVisible()`, et une opération Toggle
  commune aux chemins checkbox, clavier et UIA. Tous les tokens sont révoqués
  après invalidation du callback state.
- Les chemins default-construction+`Create()` et le chargement XRC des cinq
  familles sont couverts.

### Gates et revue

- build Release `test_gui` : **exit 0** ;
- `[winui-itemmodel]` : **21 cas, 402 assertions, exit 0** ;
- `[xrc][winui-itemmodel]` : **1 cas, 25 assertions, exit 0** ;
- stress `[winui-itemmodel][lifetime]` : **100/100 processus verts** ;
- revue finale Choice/Combo/List/CheckList/BitmapCombo :
  **0 P0 / 0 P1** ;
- `git diff --check` ciblé : **exit 0**.

Les seams clavier/double-tap déterministes factorisent l’opération métier mais
ne fabriquent pas un routed event XAML ; le trajet physique, le DPI autre que
100 % et Narrator restent dans les gates transversales 009. Aucun `SendInput`
n’a été exécuté pendant l’utilisation de la machine. Statut : **DONE**.

## Plan 001 — résolution stable du runtime et de la redistribution (2026-07-25)

La condition STOP licence a été résolue en remplaçant l'ensemble
Windows App SDK 2.x Engineering Preview par les packages stables 1.8
suivants, tous épinglés par SHA-256 dans `build/cmake/winui.cmake` :

- Runtime `1.8.260710003` ;
- WinUI `1.8.260709004` ;
- Foundation `1.8.260709000` ;
- InteractiveExperiences `1.8.260708001` ;
- Base `1.8.251216001`.

Les cinq licences embarquées portent le grant stable de redistribution des
fichiers binplacés par les packages WindowsAppSDK NuGet. Le configure vérifie
désormais ce texte et refuse une licence `ENGINEERING PREVIEW`, une
interdiction d'environnement live ou l'absence du grant attendu.

Le payload n'extrait plus rien du MSIX framework. Le MakePri x64 du même SDK
Windows `10.0.26100.0` que `cppwinrt` fusionne
`Microsoft.UI.Xaml.Controls.pri`, `Microsoft.UI.pri` et
`Microsoft.WindowsAppRuntime.pri` en une map d'application unique. Les deux
assets XAML sont copiés depuis leur chemin `runtimes-framework/win-x64/native`
du package WinUI stable. Les 25 WinMD de runtime
WinUI/Foundation/InteractiveExperiences sont également déployés pour le
marshaling unpackaged. La génération PRI est atomique et content-addressée;
son empreinte dépend de chaque PRI, de la configuration, du binaire MakePri,
du SDK Windows et d'un contrat d'invocation versionné. Les assets et WinMD
proviennent des archives NuGet stables épinglées par SHA-256.

Les archives et extractions NuGet sont content-addressées par version+hash et
verrouillées; deux repins de même version coexistent. Chaque fichier d'une
extraction gérée est couvert par un manifeste SHA-256/taille lié au hash de
l'archive et revalidé à chaque configure; le self-test altère un fichier après
extraction et prouve sa reconstruction depuis l'archive épinglée. La projection
C++/WinRT possède elle aussi un répertoire par empreinte et un manifeste
SHA-256/taille exhaustif vérifié à chaque build. Une corruption ou disparition
de n'importe quel header provoque une régénération transactionnelle, sans
contaminer une autre empreinte.

Gates rejouées :

- configure stable isolé `build-winui-stable-probe` : **exit 0** ;
- PRI généré de façon indépendante et par CMake : SHA-256 identiques
  (`3BADA227F5AE4C0039A9E580DABB1703B2AF70F906CC2D3D8D2A7E78ABF07C91`) ;
- smoke isolé avec bootstrap + PRI généré seulement :
  **Button/TreeView/TabView, exit 0** ;
- build puis `wx_winui_runtime_smoke` dans `build-winui-clean` :
  **exit 0** ;
- `wx_winui_ole_drop_probe` : **exit 0** ;
- `wx_winui_selftest_sdk_selection`, `wx_winui_selftest_nupkg_cache`,
  `wx_winui_selftest_package_license`, `wx_winui_selftest_package_set` et
  `wx_winui_selftest_cppwinrt_cache` : **5/5 Passed** ;
- test projection synthétique A → B → A : coexistence préservée et réparation
  d'un header secondaire corrompu ;
- suppression temporaire du PRI et d'un WinMD déployés, puis rebuild sans
  relink : les deux fichiers sont restaurés byte-for-byte ;
- le PRI effectivement déployé possède le même SHA-256 que la sortie MakePri
  atomique.

Le plan 001 passe donc de `BLOCKED` à **DONE** pour le profil démontré
Windows 11 x64, MSVC, Release, shared, framework-dependent/unpackaged.

## Plan 008f — tranche RichToolTip (2026-07-26)

### Contrat livré

- `wxRichToolTip` possède un état auto-porté à captures faibles, enregistré
  dans le `TransientManager` 006. `Start()` est une transaction RAII : toute
  sortie autre que `Shown`, y compris exception WinRT, `std::exception` ou
  exception inconnue après publication des handlers, ferme le peer et révoque
  timer, shutdown, événement XAML, binding wx et enregistrement central.
- Un échec de l'ouverture différée exécute toujours `Close(true)`. Le cleanup
  n'appelle jamais le getter `IsOpen()` : il écrit directement `false`, puis
  retire TeachingTip et ancre par deux opérations indépendantes avant de
  vider systématiquement l'état interne.
- Un reparent du target ou d'un de ses ancêtres est notifié de manière
  synchrone et bornée depuis le host partagé. Un changement de TLW annule la
  session avant migration de l'ancienne racine XAML ; un reparent dans le
  même TLW recalcule l'ancre. Le registre global ne conserve que des
  `weak_ptr` et chaque parcours revalide aussi le subtree/TLW via `wxWeakRef`.
- Les fallbacks génériques nécessaires aux apparences non représentables et
  à une API TeachingTip indisponible ne contournent plus le gestionnaire :
  un seam protégé minimal retourne le popup générique créé, puis un état WinUI
  le rend annulable par TLW, target, shutdown et reparent. L'API publique reste
  inchangée.
- Les propriétés UIA `Name` et `HelpText` du TeachingTip reflètent
  respectivement titre et message. Les resets icône standard/custom, couleurs
  et police rétablissent bien le chemin TeachingTip natif.

### Gates

- compilations Release ciblées `richtooltipg.cpp`, `richtooltip.cpp`,
  `tlwhost.cpp` et `winuirichtooltip.cpp` : **exit 0** ;
- links Release `wxcore`, `wxxrc` et `test_gui` : **exit 0** ;
- `[winui-rich-tooltip]` : **19 cas, 225 assertions, exit 0** ;
- `[winui-dialog-session]` : **13 cas, 120 assertions, exit 0** ;
- `git diff --check` ciblé : **exit 0**.

Les tests couvrent les trois familles d'exception de démarrage, queue
indisponible, échec après ouverture différée, multi-tip, shutdown, fallbacks
centralisés, destruction target, reparent d'ancêtre cross-TLW et reparent
same-TLW, resets d'apparence, UIA et cleanup séparé tip/ancre. Les fallbacks
testés restent en délai long et les TLW sont off-screen : aucun `SendInput`,
déplacement de pointeur ou activation volontaire n'a été exécuté. Cette
tranche est **DONE** ; le plan 008f global reste ouvert pour ses autres
familles.

## Plan 008e — Notebook, TreeCtrl et books (2026-07-28)

### Contrat livré

- Notebook possède des transactions de pages/sélection réentrantes, une
  géométrie mesurée et déterministe pour les quatre orientations, le
  hit-test, les images DPI, le clip de la bande et le transfert de focus sans
  conserver de callback après destruction.
- TreeCtrl utilise un modèle à identités stables et des deltas bornés pour les
  insertions, suppressions, tris, sélections, expansions, éditions, images et
  parties réalisées. Les callbacks différés sont gardés par génération et les
  chaînes profondes sont libérées itérativement.
- Le benchmark de 10 000 nœuds impose explicitement : insertion sous 120 s,
  parcours des 10 000 siblings sous 5 s, suppression du subtree sous 30 s,
  zéro lookup global sur les chemins delta et un nombre linéaire de mises à
  jour du peer. Le processus de qualification complet du cas a terminé en
  2,3 s sur la machine de référence.
- Le drag interne Tree émet exactement les événements BEGIN/END attendus,
  traite cancel/drop et reparent cross-TLW, tandis qu'un drop OLE réel piloté
  directement via le broker 005 ne déclenche aucun doublon d'événement Tree.
- Simplebook, Choicebook, Listbook et Treebook partagent désormais des
  transactions topology/ownership tri-state. Les mutations et destructions
  réentrantes ne publient plus un index, une page ou une visibilité périmés.
  Le ListCtrl générique sous-jacent possède le même garde lifetime/révision,
  y compris pendant les comparateurs publics de `SortItems()`.

### Gates finales

- build Release `test_gui` sérialisé : **exit 0** ;
- `[winui-listctrl-lifetime]`, Listbook/Simplebook/Choicebook/Treebook,
  Toolbook de non-régression et `[bookctrl]` :
  **26 cas, 746 assertions, exit 0** ;
- XRC des books hors Toolbook :
  **1 cas, 78 assertions, exit 0** ;
- `[winui-notebook]`, `[NotebookTestCase]` et `[wxNotebook]` :
  **16 cas, 624 assertions, exit 0** ;
- suite complète `[treectrl]` :
  **40 cas, 391 670 assertions, exit 0** ;
- benchmark Tree 10k isolé :
  **1 cas, 10 018 assertions, exit 0** ;
- revue indépendante finale Books/ListCtrl :
  **0 P0 / 0 P1** ; seconde revue ciblée Sort/lifetime :
  **0 P0 / 0 P1 / 0 P2** ;
- `git diff --check` ciblé : **exit 0**.

### Limites P2 non bloquantes

- une classe book tierce dérivée qui appelle encore le booléen historique
  `InsertPage()` ne peut pas distinguer `OwnershipConsumed` ; les dérivées du
  dépôt sont migrées au helper tri-state, les dérivées tierces devront faire
  de même si elles publient leur propre contrôleur ;
- supprimer directement une page encore possédée par un book viole le contrat
  public documenté et n'est pas auto-assaini ;
- les getters virtuels de géométrie de `wxSimplebook::DoSize()` restent
  théoriquement réentrants pour une sous-classe hostile, sans callback API
  standard sur ce chemin.

Ces limites ne masquent aucune fonctionnalité promise du plan. Le plan 008e
est **DONE** ; Toolbook et le XRC chrome/feedback restent dans 008f.

## Plan 008f — clôture déterministe chrome/feedback (2026-07-29)

### Contrat livré

- ToolBar transactionnelle : suppression sans ghost, contrôles embarqués,
  toggle/radio/dropdown, stretch, overflow, RTL/vertical, bitmap/DPI, enabled,
  short-help et UIA, avec callbacks générationnels et convergence bornée.
- StatusBar : séparation pixels/DIP, widths et champs logiques, styles,
  ellipses, `SHOW_TIPS`, contrôles de champ, grip, RTL, apparence, UIA et
  rebuild réentrant borné.
- ToolTip commun : ownership exact de la valeur locale XAML, remplacement et
  clear, masque wx vide, enable global, freeze/thaw, migration et retry
  d’observer sans callback tardif. Les timings non exposés par WinUI gardent
  les valeurs système, sans émulation incomplète.
- Hyperlink et ActivityIndicator : création réentrante, apparence, hit-test,
  visited/URL, UIA, projection last-writer-wins et destruction active.
- RichToolTip : tranche RAII/TransientManager déjà qualifiée, revalidée avec
  l’ensemble chrome.
- Toolbook, audit NSDMI/default+Create et XRC chrome/feedback intégrés.

La passe de revue UIA finale a aussi fermé les cas où le slot masquait une
valeur authored : binding `AutomationProperties.Name`, styles explicites et
implicites, `BasedOn`, dictionnaires de thème, rollback et migration. La
résolution inspecte le dictionnaire connu du slot avant les ressources
application et ignore les dictionnaires framework `XamlControlsResources`.
WinUI ne signale pas toujours un changement de source
`Style={x:Null}` → `ClearValue(StyleProperty)` lorsque la valeur effective de
`Style` reste `null`, bien qu’un style implicite commence à contribuer des
setters. Le host observe `Style`, `AutomationProperties.Name` et le thème,
conserve un snapshot léger opportuniste sur `LayoutUpdated`, puis réinspecte
conservativement les ressources au prochain dirty pass du seul
`wxWinUIXamlHost` tant que wx possède — ou va revendiquer — le Name. Il n’existe
aucun abonnement `CompositionTarget.Rendering`, timer ou polling global. Un
oracle `Tag` sans effet de layout couvre les chemins Name déjà possédé et
première revendication après label vide ; `wxWinUIXamlHost::SetLabel()` marque
désormais explicitement son slot dirty.

### Gates finales sur bureau isolé

- build Release `wxcore test_gui widgets showcase` : **exit 0** ;
- `[winui-chrome-feedback]` : **33 cas, 939 assertions** ;
- `[winui-toolbar]` : **23 cas, 1097 assertions** ;
- `[winui-rich-tooltip]` : **19 cas, 225 assertions** ;
- `[ToolbookTestCase]` : **1 cas, 131 assertions** ;
- `[wxInfoBar]` : **12 cas, 107 assertions** ;
- `[hyperlinkctrl]` : **3 cas, 9 assertions** ;
- `Window::ToolTip` : **1 cas, 7 assertions** ;
- `[xrc][winui-chrome-feedback]` : **1 cas, 55 assertions** ;
- `[default-create]` : **15 cas, 442 assertions** ;
- `[winui-008f]` : **8 cas, 217 assertions** ;
- `[HostState]` : **64 cas, 1348 assertions** ;
- `[HostLifecycle]` : **48 cas, 1361 assertions** ;
- UIA ciblé final : implicit style **1/73**, transactions/animation
  **1/69**, thèmes **1/26**, tous verts ;
- `ctest -R wx_winui_(runtime_smoke|selftest_)` : **6/6 Passed** ;
- `git diff --check` : **exit 0**.

Revues contradictoires tooltip/freeze et UIA/style : **APPROVED**, aucun
P0–P3 restant. Après les dernières corrections, `[winui-008f]` reste vert
à **8/217**, `[default-create]` à **15/442** et
`[winui-chrome-feedback]` à **33/939**.

**Statut honnête** : implémentation 008f et gates déterministes complètes ;
row `IN REVIEW` tant que sa dépendance 008d et les validations physiques du
plan 009 ne sont pas closes. Aucun test `SendInput` n’a été exécuté sur le
bureau actif et aucun commit n’a été créé.

## Réconciliation canonique des plans 008a–008f (2026-07-29)

Une compression de contexte a exposé une dérive documentaire : les
implémentations 008c et 008d existaient déjà, mais leurs fiches et le tableau
central étaient restés sur la baseline `TODO`. Elles ont alors été annoncées à
tort comme des phases à réexécuter. La chronologie, le code, les suites et deux
audits indépendants établissent l’état suivant :

- 008a–008f ont toutes reçu leur implémentation ;
- 008b et 008e sont `DONE` ;
- 008f a terminé ses gates déterministes et reste `IN REVIEW` pour le correctif
  ciblé 008d et les validations physiques 009 ;
- 008a est rouverte uniquement pour une passe corrective transversale
  Text/Loaded/focus du host partagé ;
- 008c et 008d sont `IN REVIEW`, jamais `TODO`, avec chacune un seul défaut
  ciblé frais.

Gates rejouées sur le bureau de test isolé, sans `SendInput` ni interaction
avec le bureau actif :

- build Release `test_gui` sérialisé : **exit 0** ;
- `[winui-textpeer]` : **16 cas, 522 assertions, exit 0** ;
- `[wxSearchCtrl]` : **4 cas, 25 assertions, exit 0** ;
- `[winui-range-events]` : **11 cas, 328 assertions, exit 0** ;
- `[winui-range-date]` : **18 cas, 555 assertions, exit 0** ;
- `[xrc][winui-range-date]` : **1 cas, 96 assertions, exit 0** ;
- `[winui-spin-model]` : **un cas/une assertion rouge** : une flèche de
  `wxSpinButton` reste invocable après `Enable(false)` ;
- `[winui-008d]` : **un cas/trois assertions rouges** : destruction réentrante
  pendant le nettoyage d’un groupe `wxRadioButton` ;
- `[HostState]` : **6 cas/15 assertions rouges** et `[HostLifecycle]` :
  **3 cas/5 assertions rouges**, tous localisés dans la nouvelle passe
  focus/cycle de vie.

Conséquence d’exécution : terminer la passe corrective du host, corriger les
deux rouges SpinButton/RadioButton, rejouer les non-régressions concernées et
faire relire. Ne pas réimplémenter 008b–008f. Les validations
IME/pointer/thème/contraste/Narrator/DPI restent la responsabilité du plan 009.

## Plan 009 — fermeture des gates déterministes (2026-07-30)

La section de réconciliation précédente est conservée comme historique : ses
rouges SpinButton, RadioButton, HostState et HostLifecycle ont depuis été
corrigés et ne décrivent plus l’état courant. La passe 009 a ensuite revu les
frontières UIA, lifetime, thème, DPI et images des composants promis, avec
revues indépendantes contradictoires.

### Correctifs et contrats finaux

- Notebook et TreeCtrl ne mélangent plus pixels et DIPs lors du choix d’un
  `wxBitmapBundle`. Le consensus reproduit le contrat upstream (vote sur la
  taille complète, égalité départagée par la plus grande hauteur), revalide
  owner/révision après chaque callback et retire l’image sur donnée invalide
  sans retry. Les tests couvrent destruction, révision devenue obsolète,
  tie-break et absence de `GetBitmap()` après une invalidation terminale.
- Le chevron overflow Toolbar est qualifié jusqu’au vrai peer chargé :
  identité, rôle, nom, état et pattern UIA Invoke. L’ouverture physique du
  flyout reste une gate manuelle, WinUI refusant les popups par `E_INVALIDARG`
  sur le desktop isolé non actif.
- `wxButton::SetDefault()` contient désormais l’échec d’application d’un
  `AccentButtonStyle` applicatif au `TargetType` incompatible. L’état logique
  reste valide et le peer récupère après restauration de la ressource.
- StaticBox et RadioBox n’imposent plus le littéral anglais `group` à
  `LocalizedControlType` : le type standard UIA `Group` est porté par leur peer
  et WinUI fournit son libellé localisé. Le snapshot distingue propriété locale
  absente et valeur effective du peer.
- La documentation UIA ne prétend plus que les slots visuels sans peer sont des
  nœuds Raw garantis, ni que les contrôles hébergés sont déjà enfants du
  StaticBox. Elle distingue aussi les patterns Text/Value natifs du TextBox de
  leur projection C++/WinRT in-process, qui reste optionnelle ; un client UIA
  externe demeure l’oracle release.
- `SetWindowTheme()` est explicitement best-effort par HWND. Un refus sur un
  contrôle natif ne rollbacke ni High Contrast ni le fallback solide du TLW ;
  ce chemin d’échec est couvert par le seam natif.
- Une hypothèse de revue selon laquelle un handler wx levant une exception
  traverserait le delegate Toggle WinRT a été testée puis rejetée :
  `SafelyProcessEvent()` la stocke déjà et wxWidgets la réémet au point sûr de
  sa boucle. Aucun changement qui aurait avalé cette exception publique n’a été
  conservé.

### Gates automatisées finales

Toutes les commandes GUI ci-dessous ont été exécutées via
`wx_winui_desktop_test_runner.exe`, sur un desktop isolé, sans `SendInput` ni
interaction avec le desktop actif :

- build Release sérialisé de `test_gui` : **exit 0** ;
- `[winui-009]` : **73 cas, 3 239 assertions, exit 0** ;
- `[winui-appearance]` : **32 cas, 1 307 assertions, exit 0** ;
- `[winui-toolbar]` : **23 cas, 1 101 assertions, exit 0** ;
- `[winui-notebook]` : **21 cas, 640 assertions, exit 0** ;
- `[winui-treemodel]` : **21 cas, 391 668 assertions, exit 0** ;
- `[treectrl]` complet : **46 cas, 391 802 assertions, exit 0** ;
- `[HostState],[HostLifecycle],[shell-theme]` :
  **146 cas, 3 675 assertions, exit 0** ;
- `[WinUIMenu]` : **17 cas, 312 assertions, exit 0** ;
- `[winui-spin-model]` : **13 cas, 223 assertions, exit 0** ;
- `[winui-008d]` : **18 cas, 293 assertions, exit 0** ;
- `[winui-textpeer]` : **16 cas, 522 assertions, exit 0** ;
- `[wxSearchCtrl]` : **4 cas, 25 assertions, exit 0** ;
- `ctest -R wx_winui_(runtime_smoke|selftest_)` :
  **6/6 Passed** ;
- `git diff --check` : **exit 0** (seuls avertissements de normalisation
  LF→CRLF du worktree historique).

### Statut honnête

La phase 009 est `IN REVIEW`, avec implémentation et gates déterministes
terminées. Restent exclusivement les validations physiques : snapshots
Accessibility Insights Control/Raw (dont relation StaticBox/siblings et absence
de doublons shell), Narrator et client UIA externe Text/Value, déplacement
100/150/200 % avec inspection du raster, Light/Dark/High Contrast et RTL à
chaud, scénarios multi-TLW réels focus/tooltip/D&D/owner/dialog, et ouverture
du flyout overflow Toolbar sur un desktop actif.

## Plan 010 — gate d’implémentation automatisée (2026-07-30)

La passe release a réinventorié l’arbre courant à partir du HEAD
`2c82ad3d302f`. Aucun fichier n’est indexé et aucune opération Git n’a été
effectuée. Le dossier `build-winui-release`, configuré sans artefact manuel en
Release x64/shared avec tests et samples complets, a été reconstruit sur l’état
final courant.

### Corrections découvertes par la qualification

- La géométrie chaude de `wxNotebook` conserve désormais des handles XAML forts
  autour de `ContainerFromIndex()` et re-résout l’owner/révision après chaque
  frontière virtuelle ou XAML. Aucun pointeur wx brut n’est réutilisé après un
  callback susceptible de détruire ou muter le contrôle.
- Le cache des onglets et de leurs sous-parties suit l’axe logique des
  coordonnées client wx en LTR comme en RTL. Il ne remiroite plus l’icône et le
  label après la transformation `WS_EX_LAYOUTRTL`.
- Une sélection distante est explicitement amenée dans la liste du template
  après une transition RTL/LTR chaude. L’échec de découverte ou de scroll est
  propagé au lieu d’être déclaré réussi.
- `HitTest()` inspecte tous les hits et leurs ancêtres avant de choisir un
  onglet. Pour un strip overflow, il mesure la vraie `ListView`, tous les
  `ButtonBase` actifs et échoue fermé si le template ou les bounds sont
  inconnus. Pour un strip sans overflow, le fallback logique reste disponible
  sur un desktop isolé non-input ; `Create()` y garantit add-button masqué et
  items non closables.
- Les seams de test distinguent les rectangles XAML réellement réalisés du
  cache logique. Les tests couvrent deux onglets réalisés, icône/label réels,
  overflow froid fail-closed et un cycle RTL → LTR → RTL avec sélection
  distante.
- La revue finale a encore trouvé un UAF réentrant possible dans le changement
  de page : `CreatePageChangingEvent()`, `TryGetNonNullPage()`,
  `GetPageRect()` et `MakeChangedEvent()` sont des frontières virtuelles.
  L'owner, l'opération et la page faible sont maintenant revalidés après
  chacune d'elles, y compris dans l'insertion. Six scénarios destructifs
  dédiés couvrent ces frontières; deux revues indépendantes concluent `PASS`.

### Outillage et documentation

- Les probes du spike sont opt-in et leur parsing refuse les valeurs
  ambiguës. Les collecteurs utilisent des verrous `CreateNew`, des temporaires
  GUID, un cleanup `finally`, l’identité chemin+start-time du producteur et des
  chemins relatifs au build.
- Les self-tests PowerShell 5/7, course de producteurs, absence de cible,
  nettoyage des locks/processus et mode headless ont été rejoués. Le scénario
  Z-order est `PASS`; le scénario scroll sans mesure physique retourne
  volontairement `UNQUALIFIED`/exit 2 au lieu de fabriquer un succès.
- La matrice publie les contrats transversaux, limites et exclusions ; la
  documentation distingue `wxDialog` frame-class des présentations Window/
  Overlay des dialogues communs. Le host reste `Experimental` jusqu’aux gates
  physiques.

### Gates finales courantes

Toutes les suites GUI ont été exécutées par
`wx_winui_desktop_test_runner.exe` sur un desktop isolé, sans `SendInput` :

- `ALL_BUILD` Release x64/shared, tests et samples : **exit 0** ;
- `[winui-notebook]` : **22 cas, 732 assertions, exit 0** ;
- `[wxInfoBar],[winui-notebook],[NotebookTestCase],[wxNotebook]` :
  **39 cas, 1 043 assertions, exit 0** ;
- `[HostState],[HostLifecycle],[shell-theme]` :
  **146 cas, 3 675 assertions, exit 0** ;
- dialogues/frame-class/owner/refit ciblés :
  **57 cas, 930 assertions, exit 0** ;
- `[WinUIMenu],[winui-toolbar],[winui-009]` :
  **112 cas, 4 613 assertions, exit 0** ;
- `[winui-treemodel],[treectrl]` :
  **46 cas, 391 802 assertions, exit 0** ;
- books/composites : **21 cas, 879 assertions, exit 0** ;
- `[xrc][winui-v0-mixed]` : **1 cas, 305 assertions, exit 0** ;
- runtime smoke, OLE drop probe et six self-tests :
  **8/8 Passed** ;
- `git diff --check` : **exit 0** (avertissements historiques LF→CRLF
  uniquement).

### Statut et passage à la roadmap étendue

La gate d’implémentation automatisée de 010 est verte. Le plan reste néanmoins
`IN PROGRESS` et aucune alpha/bêta n’est promue : la checklist physique 009/010
(UIA externe/Narrator, DPI multi-écrans, thème/HC/RTL visuels, multi-TLW/D&D,
overflow Toolbar et soak) reste ouverte.

La roadmap 012–014 sépare désormais explicitement implémentation et promotion :
012 peut être ouvert sur cette baseline automatisée sans fermer 009/010 ni
classer un composant `Supported`. Le plan 015 reste conditionné par la
stabilisation de 012–014. Toute mutation Git demeure soumise à l’accord
explicite de l’utilisateur.

## Pré-gel RC3 — clôture OLE et double linkage (2026-08-21)

Cette section remplace, pour l'état courant, les conclusions historiques du
plan 005 « broker par bridge ». Elle ne réécrit pas les résultats antérieurs.
Le candidat visé est uniquement le profil **Supported V0** Windows 11 x64,
MSVC, Release, framework-dependent unpackaged ; il ne constitue pas une
revendication de parité wxMSW générale.

### Topologie et ownership OLE retenus

- Le `wxWinUIDropBroker` produit possède une seule registration OLE sur le
  couple exact `(HWND TLW, génération)`. Le bridge reste l'autorité logique de
  composition, hit-test, routage et coordonnées, mais n'est plus le HWND
  d'enregistrement.
- Le registre global commun aux registrations fixes wxMSW et externes WinUI
  sérialise les changements de propriétaire. Une erreur de revoke ou de
  déverrouillage conserve l'identité COM et l'occupation fail-closed jusqu'à
  une libération prouvée.
- Une acquisition `PendingFixed` reprend par une continuation one-shot exacte,
  liée à l'identité native, à un identifiant 64 bits et au thread UI. En cas
  d'échec de `PostMessage()`, un timer one-shot sur le même thread garantit une
  nouvelle frontière de progression sans polling ni boucle active.
- La publication du host XAML et de ses slots est indépendante de l'acquisition
  OLE. Un contrôle et son contenu déjà acceptés ne sont pas abandonnés pendant
  une attente `PendingFixed` ou un échec OLE fail-closed.

### Preuves courantes

Le probe physique brut opt-in `wx_winui_runtime_smoke
--ole-drop-delivery-probe` a exécuté de vrais `DoDragDrop()` sur une surface
XAML normale puis dans un trou exposant une vraie HWND native. La garde d'entrée
est restée propre ; le résumé est `receiver-qualified`, recommande `tlw` et se
termine avec `EXITCODE=0`. Il établit la topologie USER32/OLE, pas la gate
physique du broker produit du futur artefact gelé.

Les gates déterministes pre-freeze sont vertes dans les deux linkages :

- `[winui-drop-broker]~[physical]` : **23 cas, 875 assertions** ;
- `[msw-drop-session]` : **11 cas, 218 assertions** en shared et **11 cas,
  202 assertions** en static/no-exceptions ;
- initialisation différée du host : **6 cas, 59 assertions** ;
- `[winui-v0-supported]~[.]~[physical]` : **50 cas, 927 assertions** en shared
  et en static/no-exceptions ;
- `[HostLifecycle],[HostState]` : **161 cas, 24 850 assertions** en shared et
  en static/no-exceptions.

### Statut honnête du candidat

Le code et les preuves automatiques placent le périmètre Supported V0 au stade
de **candidat bêta**, pas encore de bêta production-ready. Restent à signer sur
le candidat RC3 effectivement gelé :

- la campagne physique produit shared/static, notamment la livraison OLE via
  le vrai `wxDropTarget` sur surface normale et trou natif ;
- les scénarios manuels d'accessibilité (Accessibility Insights/Narrator/client
  UIA externe), DPI per-monitor, Light/Dark/High Contrast et RTL à chaud ;
- le soak de 60 minutes sans nouveau dump WER ni freeze.

Le PASS physique brut ne ferme aucune de ces gates humaines et les profils
Windows 10, x86, ARM64, Debug, MSIX/packaged, monolithic ainsi que la parité
wxMSW complète restent hors de cette promotion étroite.

## Rejet RC7 et correction post-présentation (2026-08-22)

La section RC3 ci-dessus reste historique. RC4 a ensuite prouvé que la cible
produit TLW-only ne reçoit aucun callback sur la surface XAML normale. RC7 a
qualifié latence, claviers et matrice OLE brute, puis a échoué au premier drag
produit normal avec garde d'entrée propre, bouton injecté observé, 51
`QueryContinueDrag`, deux registrations bridge+TLW possédées/courantes et zéro
`DragEnter`. L'utilisateur n'a ni interrompu ni causé cet échec. La différence
avec le probe brut qualifié était l'acquisition pendant la construction cachée,
avant la première présentation XAML. RC7 est rejeté et reste immuable.

Le correctif courant interdit toute acquisition dans la construction, y compris
pour un contrôle ajouté à une frame déjà visible. Les cibles wxMSW logiques
restent inscrites jusqu'à un `FlushSync()` visible ayant publié géométrie,
z-order et epoch structurel. Le broker utilise deux adaptateurs COM physiques
distincts, un par HWND, derrière une seule session logique. Les messages
`WM_SHOWWINDOW`, `WM_WINDOWPOSCHANGED` et `WM_SIZE` du bridge déclenchent des
retries coalescés sans polling. Une continuation `PendingFixed` réveillée alors
que la présentation est cachée conserve son token exact et revient par le même
gate. Un changement `Hide/Show` réentrant avant commit invalide l'epoch,
rollbacke exactement la paire provisoire, restaure les cibles logiques et crée
une nouvelle paire seulement si aucune registration ni lock incertain ne reste.

Deux revues indépendantes concluent `PASS`, sans bloqueur P0/P1. Les gates
post-correctif, exécutées sur desktop isolé sans `SendInput`, sont vertes dans
les deux linkages :

- `[winui-drop-broker]~[physical]` : **31 cas, 1 330 assertions** ;
- `[HostLifecycle],[HostState],[winui-drop-broker]~[physical]` : **190 cas,
  25 675 assertions** ;
- `[winui-v0-supported]~[.]~[physical]` : **50 cas, 927 assertions**.

Ces résultats qualifient le worktree post-RC7 pour un nouveau gel. Ils ne sont
pas encore la preuve d'un artefact reconstruit et ne remplacent ni la campagne
physique produit ni les validations manuelles/accessibilité/DPI/thème/RTL et le
soak signé de 60 minutes.
