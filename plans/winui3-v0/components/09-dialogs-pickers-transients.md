# Fiche 09 — Dialogues, pickers et surfaces transitoires

Baseline `20207bdbbb` + worktree presenter/owner/frame-class. Priorité : P0
alpha. Dépend de host lifetime, clavier/menu et state adapter.

## Décision préalable : Window ou Overlay

Le contrat public dit Window (`include/wx/winui/winui.h:68`) mais le code choisit
Overlay (`dlgpresenter.cpp:45-68`). Rédiger une décision et aligner header, code,
variables d’environnement, docs et tests. Recommandation alpha : Window par
défaut jusqu’à qualification du stacking/lifetime Overlay; garder l’autre mode
comme opt-in testé.

## TransientManager par TLW

Centraliser popup, ContentDialog et TeachingTip : une generation, un weak owner,
un état Idle/Open/Closing, un seul modal, cancellation au destroy et aucun nested
loop si TryEnqueue échoue. Les callbacks n’accèdent jamais à un presenter wx
détruit.

## Frame-class dialogs

`src/msw/toplevel.cpp:470-502` contourne le dialog manager. Gates obligatoires :

- focus initial et restauration ;
- Tab/Shift-Tab/mnemonics ;
- Enter default, Escape cancel, validation veto ;
- button default accent style (`dlgpresenter.cpp:129-137` ne doit pas utiliser
  HasKey avant Lookup d’un merged dictionary) ;
- modeless, modal, nested dialog, destroy pendant open ;
- Narrator/UIA et shell invisible.

Maintenir une bascule de comparaison tant que ces gates ne sont pas verts.

## Owner detach et priming

Tester la transaction `src/msw/window.cpp:3205-3263` : active child detach,
minimize parent, Alt-Tab grouping, two-level dialogs, owner destroyed, modeless
switch, app shutdown. Re-prime generation-safe après owner change; restaurer
l’owner exactement une fois.

## Re-fit grow-only

Font/Find fixent leurs hints avant réalisation (`fontdlgg.cpp:511`,
`fdrepdlg.cpp:206`); le relayout actuel ne grandit pas la TLW. Après Loaded et
Measure stabilisés : recalcul min/best, grandir chaque dimension insuffisante,
jamais shrink, une fois par generation. Stop si deux passes continuent de
changer la taille : diagnostiquer la boucle au lieu d’ajouter un délai.

## Composants

### MessageDialog — alpha

Presenter WinUI avec fallback MessageBox pour certaines combinaisons. Tester la
matrice boutons/icons/styles dans les deux chemins, mêmes IDs/events/default,
modalité/owner/focus/Enter/Escape. Éviter un comportement différent silencieux
selon quatre boutons.

### TextEntryDialog / PasswordEntryDialog — alpha

Le crash `m_validator` non initialisé est corrigé par NSDMI dans le worktree;
ajouter un test constructeur default/non-default, validator transfer/refusal,
password, multiline, max length, OK/Cancel et delayed destroy. Neutraliser le
HWND shell invisible en UIA et vérifier callback lifetime.

### ColourDialog — beta

ColorPicker/presenter nominal. Mapper custom colours et `ChooseFull` au lieu de
forcer MoreButton (`colordlg.cpp:72-90`); cancel ne modifie pas data; alpha et
theme/DPI.

### FontDialog / FindReplaceDialog — beta

Conserver generic. Ajouter re-fit, focus/default, localization/DPI/theme et
modeless Find close/reopen. Ne pas créer une migration native avant beta.

### FileDialog / DirDialog — beta

Conserver MSW. Tester native nested loop avec hook XAML, owner detach, Alt-Tab,
cancel, multi-select, validation et parent destruction.

### File/Dir/FontPickerCtrl — beta

Conserver composites génériques; vérifier bouton/field WinUI, modal owner,
value events, focus/Tab, disable, long paths/font names, re-fit et DPI.

### ColourPickerCtrl — beta

Émettre l’événement depuis le parent public et son ID, pas le child
(`clrpicker.cpp:193-196`); mettre à jour label lors de ColorChanged; enabled,
tooltip, hot theme et UIA.

## Tests de sortie

- `tests/controls/dialogtest.cpp`, picker/date/colour tests ;
- samples dialogs/widgets en Window et Overlay ;
- 100 cycles open/accept/cancel/destroy ;
- nested modal/modeless, owner/minimize/Alt-Tab ;
- Enter/Escape/default/Tab/mnemonic/validator veto ;
- 100/150/200 %, long localized text, hot theme ;
- Narrator sans shell duplicate ;
- aucun freeze, callback tardif ou re-prime sur HWND réutilisé.

