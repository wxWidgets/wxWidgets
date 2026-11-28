# Audit du code courant — HEAD + index + worktree

## Portée

Audit statique du commit `20207bdbbb81055da1f3e980b3271fc24566990e`, des
changements indexés, non indexés et des nouveaux fichiers WinUI. Les preuves
d’exécution figurent dans [verification.md](verification.md). Aucun fichier
source n’a été modifié.

Échelle : priorité P0/P1/P2/P3; effort S/M/L; risque de mise en œuvre
LOW/MED/HIGH; confiance LOW/MED/HIGH.

## Inventaire exact du code en attente

État capturé avant création de `plans/` : 18 fichiers suivis modifiés, dont 6
présents dans l’index.

| État Git | Fichiers | Verdict de découpe |
|---|---|---|
| Index seul | `include/wx/winui/treectrl.h` | Ne pas committer seul : vérifier sa cohérence avec les deux couches de `treectrl.cpp`. |
| Index + worktree (`MM`) | `ctrlhost.cpp`, `dlgpresenter.cpp`, `renderer.cpp`, `toolbar.cpp`, `treectrl.cpp` | Comparer cached/unstaged hunk par hunk; chaque fichier mélange au moins deux états et ne doit jamais être ajouté en bloc. |
| Worktree seul | `WINUI_PORT_PROGRESS.md`, deux CMakeLists, `textdlg.h`, `evtloop.cpp`, `toplevel.cpp`, `window.cpp`, `menubar.cpp`, `private.h`, `richtooltip.cpp`, `textdlg.cpp`, `winui.cpp` | Répartir par build, clavier, dialogues, transitoires et documentation; docs seulement après comportement validé. |
| Non suivis WinUI | `docs/winui/README.md`, `tlwhost.h`, `tlwhost.cpp`, `winuispike.cpp` | Host h/cpp indispensables à la refonte; spike à rendre opt-in; doc à aligner en dernier. |
| Non suivi hors chantier | `choice-popup-border.patch` | Patch wxMSW classique non appliqué : série/issue séparée, jamais dans un commit WinUI. |

Le patch `choice-popup-border.patch` sous-classe le popup de `src/msw/choice.cpp`.
L’approche NC-paint est plausible, mais elle ne contrôle pas les retours de
`SetWindowSubclass()`, `CreatePen()` ni `SelectObject()`. Avant usage : tests
dark toggle/DPI, destruction et recréation du popup, puis gestion sûre des
échecs GDI/subclass. Il est hors scope de la V0 WinUI.

Dans le diff final de `src/winui/renderer.cpp`, `#include "wx/utils.h"` reste
alors que l’unstaged a supprimé son seul usage (`WX_WINUI_NO_RENDERER`) : retirer
cet include avant commit. Le nouveau splitter « draw nothing » dépend du fond
fourni par `WM_ERASEBKGND`; la fiche 13 exige donc une validation Light/Dark/High
Contrast au lieu de considérer le trait blanc résolu par le seul diff.

## P0 — bloqueurs alpha

### BUILD-01 — Le build frais ne déploie pas les ressources WinUI

- **Preuves :** `build/cmake/winui.cmake:249-263` ne copie que
  `Microsoft.WindowsAppRuntime.Bootstrap.dll`; le fallback charge des XAML/PRI
  à `src/winui/winui.cpp:134-178,250-279`. Le build frais n’avait ni PRI ni
  arborescences et a retourné `E_FAIL`.
- **Impact :** contrôles sans templates ou initialisation WinUI refusée sur une
  machine/build propre; résultat local non reproductible.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH.
- **Direction :** produire une fonction de déploiement runtime commune à tout
  exécutable WinUI, copier les PRI, ressources et dépendances d’architecture,
  puis tester depuis un répertoire propre sans artefact historique.

### BUILD-02 — `wxInfoBar` casse l’API publique et la suite

- **Preuves :** `include/wx/winui/infobar.h:19-56` n’expose ni
  `wxINFOBAR_CHECKBOX`, ni setters/getters d’effets, ni `ShowCheckBox()`/
  `IsCheckBoxChecked()`, ni les overrides d’apparence de la version générique;
  les premiers usages bloquants sont dans `src/xrc/xh_infobar.cpp:40-90` et
  `tests/controls/infobar.cpp:28`.
- **Impact :** XRC et `test_gui` ne compilent pas avec la configuration standard.
- **Effort :** M. **Risque :** LOW. **Confiance :** HIGH.
- **Direction :** comparer toute l’API publique à la version générique, ajouter
  un TU de compatibilité, implémenter checkbox/checked state/apparence et
  documenter la dégradation éventuelle des effets sans supprimer l’API.

### LIFE-01 — Les handlers routés des slots ne sont jamais révoqués

- **Preuves :** `src/winui/tlwhost.cpp:878-1115` utilise `AddHandler()` avec des
  delegates capturant `this`, `window` et parfois `container`; `wxWinUISlot`
  ne stocke ni delegate ni token (`include/wx/winui/private/tlwhost.h:84-107`);
  `UnregisterSlot()` retire le Grid puis le détruit sans `RemoveHandler()`
  (`src/winui/tlwhost.cpp:581-608`).
- **Impact :** cycles COM, contenu non libéré, callback tardif vers un host ou
  `wxWindow*` détruit; candidat crédible aux crashs dépendants du timing.
- **Effort :** M. **Risque :** HIGH. **Confiance :** HIGH sur l’absence de
  révocation, MED-HIGH sur l’UAF observable.
- **Direction :** stocker tous les delegates routés dans le slot et les retirer
  avant le contenu; préférer captures faibles/générations invalidables.

### ARCH-01 — `wxWinUIXamlHost` viole « une île par TLW »

- **Preuves :** `src/winui/xamlhost.cpp:54-59,128-152` crée un
  `DesktopWindowXamlSource` autonome. Son destructeur appelle `ClearContent()`
  avant `Close()` (`:77-90`), contrairement à la règle de `tlwhost.h:129-132`.
- **Impact :** réintroduit z-order, focus, entrée, UIA et teardown multi-îles dès
  qu’une application utilise le host public.
- **Effort :** L. **Risque :** HIGH. **Confiance :** HIGH.
- **Direction :** transformer la classe en slot public du host TLW; si un host
  autonome doit survivre pour compatibilité, le déclarer explicitement hors
  invariant et refuser sa coexistence avec un host TLW dans la même fenêtre.

### STATE-01 — `Enable(false)` n’est pas un invariant de slot

- **Preuves :** `src/winui/tlwhost.cpp:2030-2069` synchronise position, taille,
  clip, RTL et visibilité, pas `IsEnabled`. Seuls button, checkbox,
  radiobutton/radiobox et textctrl ont un `DoEnable()` WinUI; les autres pairs
  XAML vivent dans le bridge séparé du HWND désactivé.
- **Impact :** contrôle visuellement ou fonctionnellement actif via souris,
  clavier ou UIA malgré un shell wx désactivé.
- **Effort :** M. **Risque :** HIGH. **Confiance :** HIGH.
- **Direction :** synchroniser `IsEnabled`, hit-test, focusability et état UIA
  dans le slot commun; les overrides composants ne servent qu’aux nuances.

### INPUT-01 — Le hook global peut échouer silencieusement

- **Preuves :** le retour de `SetWindowsHookExW()` est assigné sans test dans
  `src/winui/winui.cpp:474-485`; le chemin `src/msw/evtloop.cpp:126-144` suppose
  que les non-Tab ont déjà été proposés à XAML.
- **Impact :** saisie XAML cassée ou comportement dépendant de la boucle si le
  hook n’est pas installé.
- **Effort :** S. **Risque :** MED. **Confiance :** HIGH.
- **Direction :** échouer proprement à l’initialisation ou activer un fallback
  explicite dans la boucle wx; journaliser `GetLastError()` une seule fois.

### INPUT-02 — Les accélérateurs sont consommés avant wx

- **Preuves :** la table existe dans `src/msw/menu.cpp:1272-1311` et est traduite
  dans `src/msw/frame.cpp:771-780`; le hook appelle d’abord XAML et remplace le
  message par `WM_NULL` à `src/winui/winui.cpp:336-361`.
- **Impact :** Ctrl/Alt/F-key des menus inopérants avec focus XAML; `Ctrl-T`
  reproduit dans la conversation.
- **Effort :** M. **Risque :** HIGH. **Confiance :** HIGH.
- **Direction :** séparer traduction accélérateur/navigation de la partie
  dialog preprocessing qui cassait AZERTY; garantir une consommation unique
  dans boucles wx et natives.

### DND-01 — Aucun broker OLE sur le bridge

- **Preuves :** `src/msw/window.cpp:1778-1790` enregistre la cible sur le HWND
  du contrôle, mais le bridge est au sommet (`src/winui/tlwhost.cpp:2135-2166`)
  et reçoit l’entrée. Zéro occurrence de `IDropTarget`/`RegisterDragDrop` dans
  `src/winui`.
- **Impact :** `wxDropTarget` inutilisable sous l’île partagée.
- **Effort :** L. **Risque :** HIGH. **Confiance :** HIGH.
- **Direction :** une cible COM par bridge, transitions correctes entre slots,
  conversion de coordonnées, révocation atomique au teardown/reparentage.

### DIALOG-01 — Le contrat de présentation par défaut diverge

- **Preuves :** le header public documente `Window` par défaut dans
  `include/wx/winui/winui.h:68`, tandis que
  `src/winui/dlgpresenter.cpp:45-68` choisit `Overlay`.
- **Impact :** comportement non déterministe pour l’utilisateur et plan de
  validation ambigu.
- **Effort :** S décision, L selon le chemin retenu. **Risque :** HIGH produit.
  **Confiance :** HIGH.
- **Direction :** choisir par ADR. Pour l’alpha, `Window` est le chemin prudent
  tant que stacking/lifetime overlay ne sont pas qualifiés.

### DIALOG-02 — Le remplacement du dialog manager n’est pas qualifié

- **Preuves :** `src/msw/toplevel.cpp:470-502` contourne `CreateDialog()` pour
  tous les dialogues WinUI et affirme que navigation/default/Enter/Escape sont
  remplacés; aucun test dédié ne le prouve.
- **Impact :** régression transversale clavier, focus initial, validation et
  accessibilité dans tous les dialogues.
- **Effort :** M validation, L correction. **Risque :** HIGH. **Confiance :**
  HIGH sur le besoin de gate.

## P1 — robustesse et bloqueurs beta

### MENU-01 — Routage et ordre des événements menu non conformes

- **Preuves :** `src/winui/menubar.cpp:531-560` construit le popup avant
  `wxEVT_MENU_OPEN`; `:109-129,606-610` envoie directement au frame; le chemin
  canonique est `src/common/menucmn.cpp:679+`. Les sous-menus de la barre ne
  testent pas toujours `IsEnabled()` (`menubar.cpp:219+`).
- **Impact :** handlers menu/menubar sautés, mutations dans OPEN invisibles,
  sous-menu disabled encore activable.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH.

### LIVE-01 — Le drain popup peut entrer dans une boucle sans sortie

- **Preuves :** `src/winui/menubar.cpp:578-595` ignore le booléen de
  `DispatcherQueue::TryEnqueue()` puis lance une boucle imbriquée dont la sortie
  dépend du callback.
- **Impact :** freeze au shutdown/destruction lorsque la queue refuse le job.
- **Effort :** S. **Risque :** MED. **Confiance :** HIGH.

### LIFE-02 — Priming DWM différé sur HWND brut

- **Preuves :** `src/winui/ctrlhost.cpp:947-991` pose la propriété avant de
  vérifier `SetWindowPos`, puis capture HWND/taille bruts dans `CallAfter()`.
- **Impact :** restauration d’une taille obsolète ou action sur un HWND détruit
  puis réutilisé; échec initial empêchant un nouveau prime.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH.

### PERF-01 — Flush auto-réarmé tant que la TLW est frozen

- **Preuves :** `src/winui/tlwhost.cpp:1862-1892` republie un `CallAfter()` à
  chaque tentative si `IsFrozen()`.
- **Impact :** churn de boucle et CPU pendant un freeze long.
- **Effort :** S. **Risque :** LOW. **Confiance :** HIGH.

### GDI-01 — Région GDI perdue si `SetWindowRgn()` échoue

- **Preuves :** `src/winui/tlwhost.cpp:2234-2250` transfère supposément la
  propriété au système mais ne détruit pas `region` lorsque l’API retourne 0.
- **Impact :** fuite GDI sous échecs répétés de mise à jour région.
- **Effort :** S. **Risque :** LOW. **Confiance :** HIGH.

### ZORDER-01 — L’ordre des slots ne représente pas l’ordre réel

- **Preuves :** `src/winui/tlwhost.cpp:2076-2102` assigne les ZIndex par parcours
  pré-ordre de création; `:2135-2166` garde un bridge unique au sommet.
- **Impact :** `Raise()`/`Lower()`, overlaps et intercalation XAML/Win32 peuvent
  rendre et hit-tester dans un ordre différent du contrat wx.
- **Effort :** L. **Risque :** HIGH. **Confiance :** HIGH.

### INPUT-03 — Parité hit-test/hover/pointer incomplète

- **Preuves :** `FindNativeTargetAt()` prend le premier sibling couvrant le
  point (`tlwhost.cpp:1668-1715`); un `HTTRANSPARENT` remonte aux parents au lieu
  de poursuivre vers le sibling inférieur (`:1373-1388`). Le bookkeeping poste
  uniquement `WM_MOUSELEAVE`, jamais `WM_NCMOUSELEAVE` (`:1355-1396`). Les
  handlers de slot ne mappent que gauche/droite/milieu (`:974-1036`).
- **Impact :** overlays natifs, non-client hover, XBUTTON, tactile/stylet et
  capture peuvent diverger de Win32.
- **Effort :** L. **Risque :** HIGH. **Confiance :** MED-HIGH.

### FREEZE-01 — Le breaker couvre une seule signature

- **Preuves :** `src/winui/tlwhost.cpp:1298-1335` ne casse que les MOVE de même
  pixel après un seuil.
- **Impact :** variantes ENTER/EXIT ou alternance de positions susceptibles de
  saturer encore la boucle; absence de test reproductible.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH sur la couverture.

### UIA-01 — Noms générés obsolètes et shells dupliqués

- **Preuves :** le nom est appliqué à l’enregistrement
  (`src/winui/tlwhost.cpp:513-573`) mais pas rafraîchi après `SetLabel`; le plan
  de référence demande encore la neutralisation des HWND coquilles.
- **Impact :** Narrator lit des noms périmés et peut exposer deux fragments pour
  un contrôle.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH pour le nom dynamique.

### TOOLTIP-01 — Infrastructure commune absente

- **Preuves :** `src/msw/window.cpp:1812-1828` court-circuite le tooltip natif
  pour tout host; seules quelques classes appellent `wxWinUISetToolTip`.
- **Impact :** majorité des contrôles migrés sans tooltip ou tooltip non mis à
  jour.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH.

### TOOLTIP-02 — RichToolTip ignore l’API et a une fenêtre de fuite

- **Preuves :** `src/winui/richtooltip.cpp:80-120` ignore show delay/rect et
  alloue le host avec `new`; les opérations suivantes peuvent lever avant que
  le callback Closed n’en prenne la propriété (`:120-198`).
- **Impact :** options publiques sans effet, fuite/host orphelin à la fermeture.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH pour l’API, MED pour la
  fuite observable.

### TRANSIENT-01 — Pas de manager modal unique par TLW

- **Preuves :** les chemins menu, tooltip et ContentDialog créent directement
  leurs objets; `src/winui/dlgpresenter.cpp:287-485` n’impose pas « un modal par
  host » malgré l’invariant du plan.
- **Impact :** dialogues empilés, popup résiduel, destruction de TLW et boucles
  imbriquées non sérialisés.
- **Effort :** L. **Risque :** HIGH. **Confiance :** HIGH.

### DIALOG-03 — Re-fit grow-only absent

- **Preuves :** les dialogs génériques figent leurs hints avant réalisation
  (`src/generic/fontdlgg.cpp:511`, `src/generic/fdrepdlg.cpp:206`); le relayout
  `src/winui/ctrlhost.cpp:692-713` ne grandit pas la TLW.
- **Impact :** contenu croppé au premier affichage, notamment Choose Font.
- **Effort :** M. **Risque :** MED. **Confiance :** HIGH sur le manque, MED sur
  l’unicité de la cause.

## P1/P2 — défauts composants confirmés

| ID | Composant et preuve | Impact | E/R/C |
|---|---|---|---|
| COMP-01 | ListBox `listbox.cpp:71-73,105-114,463-539` | fuite client object, multisélection perdue/événement faux | M/M/H |
| COMP-02 | BitmapCombo `bmpcbox.cpp:83-108` | insertion/sort désassocie bitmap et texte | M/M/H |
| COMP-03 | ColourPicker `clrpicker.cpp:193-196` | événement émis par le child/ID interne | S/L/H |
| COMP-04 | ScrollBar `scrolbar.cpp:91-124,133-167` | valeur canonique non clampée, CHANGED dupliqué/prématuré | M/M/H |
| COMP-05 | Slider `slider.cpp:106-180,274-282` | sources scroll confondues et CHANGED parasite | M/M/H |
| COMP-06 | SpinButton `spinbutt.cpp:90-126,164-168` | `wxSP_VERTICAL` rendu horizontal | S/L/H |
| COMP-07 | TextCtrl `textctrl.cpp:349-364,505-646,692-715` | caret/dirty/max length/hit-test; suite instable | L/H/H |
| COMP-08 | SearchCtrl `srchctrl.cpp:81-180,231-313` | faux cancel, menu/caret/édition non implémentés | L/M/H |
| COMP-09 | ComboBox `combobox.cpp:306-355,497-556` | toggle editable et caret/template divergents | M/M/H |
| COMP-10 | CheckBox `checkbox.cpp:181-185,270-275` | état indéterminé réduit à false | S/L/H |
| COMP-11 | RadioButton `radiobut.cpp:334-357` | frontières `wxRB_GROUP` cassées | S/L/H |
| COMP-12 | Calendar `calctrl.cpp:93-111,160-170` | faux double-clic, navigation/mark no-op | L/M/H |
| COMP-13 | Notebook `notebook.cpp:76-95,324-403` | placements/taille/refresh images incomplets | L/H/H |
| COMP-14 | TreeCtrl `treectrl.cpp:479-503,1311-1430,1641-1739` | key non consommable, hit-test faux, callback tardif, O(N²) | L/H/H |
| COMP-15 | Toolbar `toolbar.cpp:187-267` | outil supprimé recréé; toggle/control tools incomplets | M/M/H |
| COMP-16 | StaticBox `statbox.cpp:104-132,216-252` | couleurs/masque/police fixes | M/L/H |
| COMP-17 | StaticText `stattext.cpp:215-258,329-359` | ellipsize start/middle et font incorrects | M/L/H |
| COMP-18 | StatusBar `statbar.cpp:97-183` | styles de champs, ellipses, tips ignorés | M/L/H |

`E/R/C` = effort / risque / confiance, avec S/M/L, LOW/MED/HIGH abrégés.
Les plans détaillés et les tests par composant sont dans
`plans/winui3-v0/components/`.

## P2/P3 — dette de fond et documentation

### BUILD-03 — Téléchargements NuGet non vérifiés et SDK choisi lexicalement

- `build/cmake/winui.cmake:33-42` trie les chemins comme chaînes et peut choisir
  une version numériquement plus ancienne.
- `build/cmake/winui.cmake:65-100` télécharge et extrait les nupkg sans hash.
- **Impact :** build non déterministe et risque supply-chain/cache corrompu.
- **Effort :** S/M. **Risque :** LOW. **Confiance :** HIGH.

### DOC-01 — Les invariants documentés sont périmés

- `include/wx/winui/private/tlwhost.h:115-125` décrit encore
  `WM_NCHITTEST→HTTRANSPARENT`; l’implémentation finale est island-first
  (`src/winui/tlwhost.cpp:2253-2267`).
- `src/winui/private.h` et le plan historique indiquent encore des defaults ou
  surfaces supprimées. `WINUI_PORT_PROGRESS.md` contient plusieurs annonces de
  complétude contredites par les tests.
- **Direction :** mettre à jour après décision, jamais avant que le comportement
  et les tests soient stables.

### ARCH-02 — `tlwhost.cpp` concentre trop de responsabilités

Le fichier fait environ 2 458 lignes et mélange lifetime, géométrie, input,
focus, hit-test, régions, UIA, cursors et freeze. Après ajout des tests de
caractérisation — pas avant — extraire des objets internes `HostLifetime`,
`GeometrySync`, `InputRouter`, `FocusArbiter`, `DropBroker`,
`TransientManager` et `BackdropPolicy`. Garder une façade unique afin de ne pas
exposer WinRT dans les headers publics.
