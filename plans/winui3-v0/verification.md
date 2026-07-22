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

## Exécution du plan 001 — 2026-07-22

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
