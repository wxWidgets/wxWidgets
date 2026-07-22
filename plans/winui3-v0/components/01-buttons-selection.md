# Fiche 01 — Boutons, checkbox et radio

Baseline `20207bdbbb`, après stabilisation de la fiche 00. Fichiers :
`src/winui/button.cpp`, `tglbtn.cpp`, `checkbox.cpp`, `radiobut.cpp`,
`radiobox.cpp` et headers correspondants.

## Gates observés

| Composant | Test Release | Niveau actuel |
|---|---:|---|
| Button | `Button::Click` : 1 échec | Alpha rouge |
| ToggleButton | 1 échec | Alpha rouge |
| CheckBox | 20 assertions OK | Alpha partiel |
| RadioButton | 5 échecs | Alpha rouge |
| RadioBox | 6 échecs | Alpha rouge |
| CommandLinkButton | non ciblé | Generic non qualifié |

## Button / BitmapButton

**Présent :** clic, default style, alignement/RTL, font/couleurs, tooltip,
markup, auth shield, positions/marges et bitmaps par état
(`button.cpp:539-769,889-1223`). Le nettoyage explicite des handlers
(`:490-536`) est le modèle lifetime à réutiliser.

**Alpha :** diagnostiquer le gate clic via readiness/position du slot et le
routeur; ne pas dupliquer `SendClickEvent()` déjà correct (`:567-576`). Valider
Space/Enter, bouton default et focus avec frame-class dialogs.

**Beta :** créer `AccessKey` à partir du `&`, refresh `wxBitmapBundle` au DPI,
retirer les doubles conversions `FromDIP()` vers propriétés XAML
(`:389-401,1070,1177-1180`), nommer les boutons bitmap-only.

**Tests :** `buttontest.cpp:54-110`, Space/Enter/Alt+mnemonic, every bitmap state,
default/cancel dialogs, 100/150/200 %, hot theme, UIA Name après SetLabel.

## ToggleButton / BitmapToggleButton

**Présent :** valeur et événement simples (`tglbtn.cpp:73-110,166-182`).

**Alpha :** gate clic; enabled via slot; appliquer disabled-before-create;
révoquer le callback capturant `this` avant destructeur (`:21-58`).

**Beta :** bitmap normal/pressed/current/focused/disabled, getters, marges,
position, font/couleurs/tooltip/RTL/mnemonic et DPI. Actuellement
`DoSetBitmap()` ignore les états non normaux (`:206-213`).

**Tests :** suite toggle, toutes transitions valeur/Command, enabled avant/après
Create, bitmaps par état et UIA bitmap-only.

## CheckBox

**Présent :** 2/3 états et test API programmatique vert.

**Alpha :**

- préserver `wxCHK_UNDETERMINED` dans event `GetInt()`; le code le réduit à 0
  (`checkbox.cpp:270-275`) ;
- `Command()` doit utiliser la valeur entière, pas `event.IsChecked()`
  (`:181-185`) ;
- aligner `GetValue()` sur les autres ports pour l’état indéterminé ;
- appliquer disabled-before-create via l’adapter commun.

**Beta :** AccessKey, RTL dynamique, reset complet font family/weight/style,
Narrator sur les trois états.

**Tests :** conserver les 20 assertions; ajouter round-trip 0/1/2 via Command et
event, disabled initial, mnemonic, RTL chaud et UIA ToggleState.

## RadioButton

**Alpha :**

- partager un GroupName XAML stable pour chaque groupe wx ;
- respecter les frontières `wxRB_GROUP`; `ClearRadioGroup()` parcourt trop loin
  (`radiobut.cpp:334-357`) ;
- reproduire flèches, tab-stop unique et focus programmatique ;
- enabled initial via slot ;
- ramener les 5 échecs de `radiobuttontest.cpp` à zéro.

**Beta :** access keys, RTL/font reset, groupe UIA et Narrator.

**Tests :** deux groupes adjacents, contrôle non-radio intercalé, `wxRB_SINGLE`,
flèches, Tab/Shift-Tab, sélection sans focus et disable.

## RadioBox

Le root est un Grid/StackPanel (`radiobox.cpp:400-459`), donc
`FocusSlot()` ne sait pas le focaliser. Ajouter une `preferredFocusTarget` au
slot, pointant sur le bouton sélectionné ou premier enabled.

**Alpha :**

- `SetSelection(wxNOT_FOUND)` doit décocher le peer (`:164-176`) ;
- un `Enable(item,true)` ne peut contourner le disable global (`:185-194`) ;
- rebuild doit appliquer show/enabled globaux ;
- `SetLabel()` met à jour le titre et UIA ;
- `SetString()` invalide best size ;
- supprimer le double scaling `FromDIP()` dans Margin/Padding (`:366-454`).

**Beta :** item hit-test, help/tooltip par item, font/couleurs, hot theme, access
keys et sémantique UIA de groupe.

**Tests :** suite existante, clear selection, disable global puis enable item,
SetLabel/SetString à chaud, focus/flèches, hit-test, 200 %, Narrator.

## CommandLinkButton

Fallback générique au-dessus de wxButton (`include/wx/commandlinkbutton.h:134-165`).
Pour la V0, ne pas créer un peer dédié : qualifier label+note, best size,
default/focus et thème. Le bitmap par défaut est volontairement omis sous
WinUI (`src/generic/commandlinkbuttong.cpp:107-111`); décider si le glyph XAML
est requis en beta.

## Ordre de travail

1. Fix infrastructure clic/enabled/focus target (fiche 00).
2. CheckBox 3-state et Radio group/focus.
3. Button/Toggle default keyboard et dialogs.
4. Apparence commune, mnemonic, DPI et UIA.
5. Qualifier CommandLink generic.

## Sortie alpha

- tous les gates dynamiques de la table verts ;
- aucun événement sur contrôle disabled ;
- navigation clavier et groupes conformes ;
- Enter/Escape/default dans dialogues validés ;
- limitations beta documentées, jamais des no-op silencieux non listés.

