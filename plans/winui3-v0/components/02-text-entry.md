# Fiche 02 — TextCtrl, SearchCtrl, ComboBox et contrat TextEntry

Baseline `20207bdbbb`, après fiches 00 et 01. Priorité globale : **P0 alpha**.
Le bon investissement est un adaptateur de peer texte commun, pas trois séries
de correctifs divergents.

## Gates observés

| Composant | Résultat Release |
|---|---:|
| `wxTextCtrl` | environ 26 assertions; crash timing-dépendant hors debugger |
| `wxSearchCtrl` | 3 échecs sélection/caret |
| `wxComboBox` | 8 échecs TextEntry/items/dropdown |
| `wxBitmapComboBox` | 6 échecs, dépend aussi de la fiche 03 |

## Adaptateur `TextPeerAdapter` proposé

Responsabilités : obtenir le TextBox template part par génération, texte,
selection/caret, readonly, clipboard, undo/redo, max length, distinction
utilisateur/programmatique, IME/dead keys, Enter/Tab et révocation des callbacks.
Ne jamais conserver un template part après re-template; chaque méthode retourne
un échec explicite si le part n’est pas encore réalisé et programme une seule
application différée generation-safe.

## wxTextCtrl — P0

### Lifetime d’abord

Plusieurs callbacks capturent `this` (`textctrl.cpp:78-88,179-316`) et le
destructeur est par défaut (`:145`). Avant tout cosmétique :

1. reproduire la suite 100 fois en Release ;
2. journaliser génération/destroy/callback ;
3. stocker et révoquer chaque token/property callback ;
4. lancer Application Verifier/ASan si le toolchain le permet ;
5. obtenir une pile si un AV survit.

Ne pas attribuer le crash intermittent à une ligne sans pile.

### Contrats alpha

- PasswordBox readonly; `SetEditable()` ne traite que TextBox (`:505-515`) ;
- `SetValue()` ne doit pas laisser `IsModified()` vrai (`:634-646`) ;
- `SetSelection()` et insertion point conformes (`:477-494`) ;
- `HitTest()` et `ShowPosition()` réels (`:611-620`) ;
- convertir rects XAML DIPs vers coordonnées wx (`:654-665`) ;
- max length robuste aux raccourcis, paste et IME (`:272-316`) ;
- enabled initial via slot ;
- un seul `wxEVT_TEXT` par mutation programmée selon le contrat.

### Beta

`wxTE_PROCESS_TAB`, `AUTO_URL`, `NOHIDESEL`, `NO_VSCROLL`, wrap modes, styles de
texte/rich. Les capacités rich non réalisables avec TextBox peuvent être
explicitement hors beta, mais les méthodes ne doivent pas prétendre réussir.

### Tests

`tests/controls/textctrltest.cpp` et `textentrytest.cpp` : dirty-state,
same-value, caret, max length, readonly password, hit-test/coords à 100/200 %,
paste/IME, multiline/scroll, destroy/recreate stress.

## wxSearchCtrl — P0/P1

### Alpha

- posséder/détruire correctement le `wxMenu`; `SetMenu()` remplace aujourd’hui
  un pointeur sans nettoyage (`srchctrl.cpp:146-149`, destructeur `:58`) ;
- implémenter clipboard/undo/redo et vraie selection sur le TextBox
  (`:231-300`) ;
- `SetEditable()` rend readonly, pas disabled (`:308-313`) ;
- honorer `wxTE_READONLY` à Create ;
- rendre visible/caché le bouton cancel; distinguer son clic d’un Backspace qui
  vide le texte (`:93-111,175-179`) ;
- afficher le menu et émettre Search/TextEnter avec ordre documenté ;
- appliquer les suggestions définies avant Loaded.

### Architecture

Choisir entre composite `TextBox + search + cancel/menu` possédé et accès robuste
aux template parts de AutoSuggestBox. Ne pas garder un modèle wx déconnecté du
vrai champ.

### Tests

Suite search, toutes opérations TextEntry, ownership/remplacement menu, clear
button vs Delete/Backspace, readonly, suggestions avant Loaded, Enter, UIA des
boutons et destruction menu ouvert.

## wxComboBox — P0/P1

### Alpha

- corriger d’abord Choice via fiche 03 ;
- connecter `DropDownOpened/Closed` aux événements wx; `Popup/Dismiss` ne le
  font pas (`combobox.cpp:213-240`) ;
- `SetEditable()` doit changer `ComboBox::IsEditable`, forcer/revalider le
  template et préserver valeur/sélection (`:306-320`) ;
- décider/implémenter `wxCB_SIMPLE` ou refuser ce style explicitement ;
- ne pas remettre le caret en fin à chaque changement (`:565-575`) ;
- transmettre client data dans événements et supprimer doublons TEXT/COMBOBOX ;
- révoquer `RegisterPropertyChangedCallback` au destroy (`:143-150`).

### Beta

autocomplete, process-tab, appearance/tooltip/UIA, editable↔readonly après
réalisation et destruction popup ouvert.

### Tests

`comboboxtest.cpp`, TextEntry commun, popup par souris/clavier/API, events exacts,
caret au milieu après WriteText/Remove, style simple, re-template et destruction.

## Ordre de travail

1. Tokens/lifetime TextCtrl et test loop sans crash.
2. Adaptateur texte et contrats selection/value/readonly/max.
3. Search ownership/buttons/menu.
4. Combo template/editable/dropdown.
5. IME/styles avancés/UIA/DPI.

## Sortie alpha

- 100 exécutions TextCtrl sans crash ni callback post-destroy ;
- suites TextCtrl/Search/Combo ciblées vertes pour capacités alpha ;
- AZERTY/AltGr/dead keys et accélérateurs restent verts ;
- aucun no-op silencieux dans clipboard/selection/readonly alpha.

