# Fiche 06 — Statiques, activité, hyperlink et InfoBar

Baseline `20207bdbbb`. Cette famille partage apparence, DPI et UIA; créer ou
réutiliser `WinUIAppearanceAdapter` avant de multiplier les setters locaux.

## wxStaticText — alpha

- START/MIDDLE/END ellipsis deviennent tous end ellipsis
  (`stattext.cpp:215-224`) ;
- SetFont reconstruit seulement les inlines, sans propriétés font
  (`:253-258,329-359`) ;
- foreground/background non adaptés ;
- markup strike/big/small/teletype incomplet ;
- UIA Name non rafraîchi après SetLabel.

**Plan :** appliquer font/brush/reset, ellipsisation wx pour start/middle si XAML
ne la fournit pas, refresh UIA, puis parser markup beta.

**Tests :** contraintes largeur pour 3 ellipses, font/colors, label chaud,
markup/mnemonic, `NO_AUTORESIZE`, theme/DPI/UIA.

## wxStaticBitmap — alpha proche

506 assertions passent. Le bundle est sélectionné uniquement Create/Set
(`statbmp.cpp:125-145,191-218`). Ajouter DPI listener, reselect source,
InvalidateBestSize et layout, puis tester 100→150→200 %, alpha, icon et scale
modes. Ne pas refondre le composant avant ce gate.

## wxStaticBox — alpha

- brushes/masque codés en dur (`statbox.cpp:216-252`) ;
- setters appearance reconstruisent mais ne transmettent pas réellement toutes
  les propriétés (`:104-132`) ;
- `FromDIP()` utilisé avant Margin/Padding XAML (`:221-248`) = double scaling ;
- role UIA group/label relation absents.

**Plan :** ThemeResource/clip au lieu de masque opaque, DIPs bruts, font/colour,
hot-theme rebuild et UIA group. Tester Mica/fallback/fond custom, label-window,
nested layout, 100/200 % et Narrator.

## wxStaticLine — post-beta

Orientation/brush dynamique sont simples (`statline.cpp:65-83`). Marquer la
ligne comme décorative dans UIA, tester 1 DIP, theme/DPI. Pas de migration
supplémentaire prioritaire.

## wxActivityIndicator — beta accessibilité

Start/Stop et état natif sont présents (`activityindicator.cpp:61-104`). Filtrer
le nom technique automatique ou fournir un nom accessible selon usage; tester
start/stop répété, show/hide, destroy while active, Tab order, theme/DPI.

## wxHyperlinkCtrl — beta

La couleur hover est seulement stockée; le rendu choisit normal/visited
(`hyperlink.cpp:134-150`). `wxHL_CONTEXTMENU`, alignements et appearance wx sont
incomplets; UIA Name devient stale après SetLabel.

**Plan :** visual states hover/visited, context menu copy URL, alignement,
appearance adapter et UIA refresh.

**Tests :** étendre hyperlink tests : hover, visited reset, contexte, alignment,
disable, keyboard, SetLabel+UIA et hot theme.

## wxInfoBar — P0 build puis beta

### P0 API

Le header WinUI omet `wxINFOBAR_CHECKBOX`, `SetShowHideEffects`,
`SetEffectDuration`, `ShowCheckBox`, les getters d’effets/durée,
`IsCheckBoxChecked()` et les overrides font/foreground/border de la version
générique. Une partie seulement bloque aujourd’hui XRC/tests. Aligner toute
l’interface publique, avec checkbox réelle, état checked, apparence propagée et
effets supportés ou dégradation documentée.

### Beta comportement

- best size fixe (`infobar.cpp:177-180`) ;
- mutations boutons reconstruisent sans InvalidateBestSize/Layout parent
  (`:132-154,182-230`) ;
- texte tronqué sans tooltip, contrairement au generic ;
- enabled/tooltips/UIA via slot.

**Tests :** TU de parité de toutes les signatures, build XRC/test_gui, suite
InfoBar, checkbox checked/unchecked, getters/setters effects, font/foreground/
border, long localized text, multiple buttons, hot mutation, click/dismiss,
disabled, narrow resize et DPI.

## Ordre

1. InfoBar API compile.
2. StaticBox/StaticText appearance et DPI correctness.
3. StaticBitmap DPI refresh.
4. Hyperlink/InfoBar behavioral beta.
5. Activity/StaticLine UIA polish.
