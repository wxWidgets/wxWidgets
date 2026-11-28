# Plan 001 — Rendre le build WinUI frais et son runtime reproductibles

> **Instructions exécutant** : suivre chaque étape, exécuter chaque gate et
> arrêter aux conditions STOP. Aucun commit/push/PR sans validation explicite de
> l’utilisateur. Mettre à jour cette ligne dans `plans/README.md` à la fin.
>
> **Drift check** :
> `git diff --stat 20207bdbbb81055da1f3e980b3271fc24566990e..HEAD -- build/cmake/winui.cmake build/cmake/tests/gui/CMakeLists.txt build/cmake/samples/CMakeLists.txt docs/winui/README.md`
> puis `git diff --stat -- build/cmake/winui.cmake build/cmake/tests/gui build/cmake/samples docs/winui`,
> `git diff --cached --stat -- build/cmake/winui.cmake build/cmake/tests/gui build/cmake/samples docs/winui`,
> `git status --short -- build/cmake/winui.cmake build/cmake/tests/gui build/cmake/samples docs/winui`
> et `Get-FileHash -Algorithm SHA256 build/cmake/winui.cmake`.
> Empreinte auditée :
> `caf66a6affb9573074097cd0e9e0d7f01886dbb4b8639281727380d2143aceda`.
> Si elle diffère, réconcilier ce plan avant de modifier.

## Statut

- **Priorité** : P0
- **Effort** : M
- **Risque** : MED
- **Dépend de** : aucun
- **Catégorie** : build, packaging, security, tests
- **Planifié à** : `20207bdbbb`, 2026-07-21
- **État au 2026-07-22** : `BLOCKED: licence/redist WinUI 2.1.0` — la
  condition STOP « payload non redistribuable sous la licence du package »
  est déclenchée (constat détaillé en fin de section STOP). L'ingénierie des
  étapes 1 à 6 est implémentée et vérifiée pour un **profil développement**;
  la clôture DONE exige une décision de version Windows App SDK (elle-même
  condition STOP), à trancher par ADR.

## Pourquoi

Un build vierge produit les DLL et `test_gui.exe`, mais ne déploie pas les PRI
et ressources XAML nécessaires. `XamlControlsResources` retourne `E_FAIL`; un
ancien build marche uniquement parce qu’il contient des artefacts copiés hors du
chemin CMake courant. La V0 est invérifiable et non distribuable tant que cette
baseline dépend de l’historique local.

## État courant

- `build/cmake/winui.cmake:249-263` ne copie que
  `Microsoft.WindowsAppRuntime.Bootstrap.dll`.
- `build/cmake/tests/gui/CMakeLists.txt:232+` crée `test_gui` sans helper de
  déploiement WinUI.
- `build/cmake/samples/CMakeLists.txt:98` configure seulement un cas spécifique.
- `src/winui/winui.cpp:134-178,250-279` essaie d’abord `ms-appx:///`, puis un
  loose `Microsoft.UI/Themes/generic.xaml`; le loose file seul échoue aussi.
- L’ancien output contient `Microsoft.UI.pri`,
  `Microsoft.UI.Xaml.Controls.pri`, `resources.pri`, `Microsoft.UI/` et
  `Microsoft.UI.Xaml/`; le build frais non.
- `build/cmake/winui.cmake:33-42` choisit le « dernier » SDK lexicalement.
- `build/cmake/winui.cmake:65-100` télécharge/extrait les nupkg sans hash.

## Commandes

| But | Commande | Attendu |
|---|---|---|
| Configurer | `cmake -S . -B build-winui-clean -G "Visual Studio 17 2022" -A x64 -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=ALL -DwxBUILD_SHARED=ON` | exit 0 |
| Compiler probe | `cmake --build build-winui-clean --config Release --target wx_winui_runtime_smoke minimal widgets -- /m:4 /nr:false` | exit 0, sans dépendre d’InfoBar |
| Intégration différée | `cmake --build build-winui-clean --config Release --target test_gui -- /m:4 /nr:false` | à exécuter dans le plan 002, après réparation d’InfoBar |
| Contrôler Git | `git status --short` | uniquement fichiers préexistants + scope du plan |

## Scope

**Dans le scope :**

- `build/cmake/winui.cmake`
- helpers CMake tests/samples/exécutables nécessaires
- `docs/winui/README.md`
- un petit probe/test CMake/runtime si nécessaire

**Hors scope :** contrôles, host TLW, API InfoBar, dialogues, changement de
version Windows App SDK.

## Étapes

### 1. Inventorier le payload unpackaged exact

À partir des packages x64 configurés, lister DLL, PRI, XBF, assets et dossiers
locale nécessaires par le runtime 2.1. Ne pas copier aveuglément l’intégralité
de toutes architectures. Comparer au payload de `build-winui` et vérifier les
licences/redist.

**Vérifier** : produire dans le log CMake une liste déterministe de sources
existantes; chaque path appartient à l’architecture/configuration demandée.

### 2. Séparer configuration de bibliothèque et déploiement d’exécutable

Créer un helper idempotent `wx_winui3_deploy_runtime(target)` appliqué aux
exécutables WinUI. Il copie bootstrap, PRI/XBF/assets/arborescences nécessaires
dans `$<TARGET_FILE_DIR:target>` et gère multi-config. Ne pas dépendre du fait que
wxcore et l’exécutable partagent fortuitement le même output.

**Vérifier** : `cmake --build ... --target wx_winui_runtime_smoke minimal
widgets`; les outputs de chaque cible contiennent le manifeste de fichiers
attendu. `test_gui` est l’intégration ultérieure du plan 002, pas un prérequis à
la clôture de 001.

### 3. Appliquer automatiquement le helper

Brancher le helper dans le mécanisme commun de création des exécutables/tests et
dans les samples WinUI, sans appels ponctuels oubliables. Éviter les copies
concurrentes conflictuelles; utiliser `copy_if_different`.

**Vérifier** : smoke runtime, minimal et widgets produits depuis un dossier
vide, sans copie manuelle; inspecter les propriétés CMake pour confirmer que le
helper est aussi attaché à `test_gui`. Sa compilation/exécution finale reste la
gate du plan 002.

### 4. Ajouter un smoke runtime

Le probe démarre l’app, crée `XamlControlsResources`, puis au moins Button,
TreeView et TabView, pompe un tour et ferme. Une ressource absente donne exit
non-zéro et message précis.

**Vérifier** : probe exit 0; renommer temporairement un PRI dans le build doit le
faire échouer proprement, puis restaurer le fichier.

### 5. Rendre acquisition SDK/NuGet déterministe

Choisir une seule version SDK via comparaison `VERSION_GREATER`, dériver outil
et winmd de ce root. Ajouter hashes SHA-256 versionnés aux nupkg, téléchargement
dans un fichier temporaire, validation avant extraction et cache atomique.

**Vérifier** : corrompre une copie cache de test doit faire échouer configure
avant extraction; un faux arbre SDK 9600/19041/26100 choisit 26100.

### 6. Documenter le contrat de distribution

Mettre à jour `docs/winui/README.md` : runtime/framework requis, fichiers
déployés, packaged/unpackaged, architectures, commande clean-room et limites.

## Tests et critères Done

Preuves détaillées : section du 2026-07-22 de
[winui3-v0/verification.md](winui3-v0/verification.md). Rejeu clean-room
dans `build-winui-clean2` (dossier vierge, code final).

- [x] configuration et build dans `build-winui-clean` sans artefact préalable ;
- [x] aucun warning `Cannot locate resource` ;
- [x] probe Button/TreeView/TabView exit 0 ;
- [x] le smoke runtime et minimal/widgets fonctionnent sans copie manuelle ;
- [x] le helper est branché sur `test_gui`, dont l’exécution finale appartient
  au plan 002 ;
- [x] sample minimal/widgets démarrent depuis leur output ;
- [x] cache corrompu rejeté par hash ;
- [x] choix SDK numérique testé ;
- [x] aucun fichier hors scope modifié ;
- [x] documentation et row README mises à jour.

Les critères techniques ci-dessus sont verts pour le **profil
développement**, mais le plan ne peut pas passer `DONE` : la condition STOP
licence (ci-dessous) prime, et sa résolution passe par un changement de
version Windows App SDK — lui-même condition STOP exigeant une ADR.

## STOP

- Le payload requis ne peut pas être redistribué sous la licence du package.
- Le runtime exige une identité package au lieu d’un déploiement unpackaged :
  arrêter et proposer une ADR packaged/unpackaged.
- Le helper commun imposerait les ressources WinUI à des exécutables wxMSW.
- Un changement de version Windows App SDK paraît nécessaire.

### Constat STOP licence (2026-07-22)

Lecture des licences embarquées des packages épinglés
(`build-winui-clean*/packages/*/license.txt`) :

- `Microsoft.WindowsAppSDK.WinUI 2.1.0` : « MICROSOFT WINDOWS APP SDK
  ENGINEERING PREVIEW » — usage développement/test uniquement (« You may not
  use the software in a live operating environment »), et l’interdiction de
  distribution (clause 3.e) n’a **aucune** exception de code distribuable.
  Rien de ce package n’est redistribuable. Il ne contribue plus qu’au build
  (winmd/headers), ce que son droit dev/test couvre.
- `Foundation 2.0.21`, `Runtime 2.1.3`, `Base 2.0.4`,
  `InteractiveExperiences 2.0.13` : licence Windows App SDK standard; la
  clause « DISTRIBUTABLE CODE » (3.a.i) couvre « any files that are
  binplaced with your application by the WindowsAppSDK NuGet package » —
  soit le bootstrap — et la redistribution des MSIX du runtime vers les
  machines cibles est le mécanisme documenté
  (learn.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).
- L’extraction app-locale de `resources.pri`/assets depuis le MSIX du
  framework n’est ni un « binplace » du package NuGet ni un élément du
  contrat de déploiement documenté : c’est un contournement **profil
  développement** exigé par la série expérimentale actuelle (sans lui,
  `XamlControlsResources` échoue), non déclarable redistribuable.

Chemin de sortie : passer à une série Windows App SDK dont le composant
WinUI est sous licence standard et dont le déploiement unpackaged documenté
fonctionne sans montage app-local — c’est un changement de version épinglée
(condition STOP ci-dessus), qui exige une ADR et sa propre validation.
