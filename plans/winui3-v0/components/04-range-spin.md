# Fiche 04 — Gauge, Slider, ScrollBar et Spin

Baseline `20207bdbbb`; dépend de l’état enabled/tooltips/DPI de la fiche 00.

## Gates observés

| Composant | Résultat |
|---|---:|
| Gauge | 10 assertions OK |
| Slider | 4 échecs |
| SpinCtrl/Double | 26 échecs dans 13 cas |
| ScrollBar/SpinButton | pas de suite dédiée suffisante |

## wxGauge — alpha proche

Le rendu horizontal/vertical, valeur, range et Pulse existent. `Create()` ne
passe toutefois pas par `wxGaugeBase::Create()` et n’appelle pas
`InitProgressIndicatorIfNeeded()` (`gauge.cpp:48-65`; contrat
`include/wx/gauge.h:101-108`). `wxGA_PROGRESS` est donc inopérant.

**Plan :** restaurer le chemin base; synchroniser range/value avec le progress
indicator; faire sortir Pulse du mode indéterminé via SetRange et SetValue;
décider couleurs custom en beta.

**Tests :** maintenir les 10 assertions, Pulse→SetRange/SetValue, range nul,
vertical, `wxGA_PROGRESS`, disable/theme/DPI.

## wxSlider — alpha bloquant

Toute variation devient THUMBTRACK (`slider.cpp:106-120`), tout KeyUp termine
l’interaction (`:135-143`) et SetValue/Range ne synchronisent pas
`m_lastEndValue` (`:166-180`), d’où CHANGED parasite (`:274-282`).

**Plan :**

1. introduire état source `PointerDrag`, `KeyboardLine`, `KeyboardPage`,
   `HomeEnd`, `Wheel`, `Programmatic` ;
2. mapper LINE/PAGE/TOP/BOTTOM/THUMBTRACK/THUMBRELEASE/CHANGED ;
3. émettre CHANGED une fois à la fin d’une interaction, aucun event pour setter ;
4. implémenter ou déclarer hors V0 `LEFT/TOP/RIGHT/BOTTOM/BOTH`, LABELS,
   SELRANGE, SetTick, thumb length ;
5. appliquer inverse/orientation et enabled.

**Tests :** remettre `slidertest.cpp:94-240` au vert; ajouter drag annulé,
molette, Page/Home/End, SetValue puis KeyUp sans event, inverse et disable.

## wxScrollBar — alpha

Le peer bride sa valeur mais le modèle conserve l’entrée brute
(`scrolbar.cpp:91-124`). Les valeurs numériques magiques de ScrollEventType
(`:140-154`) et l’ajout systématique de CHANGED (`:160-167`) ne reproduisent pas
MSW (`src/msw/scrolbar.cpp:70-129`).

**Plan :** enum nommé, clamp canonique stocké, suppression des mouvements nuls,
un événement par notification et CHANGED uniquement sur EndScroll.

**Tests à créer :** line/page/top/bottom/thumbtrack/thumbrelease/end, positions
hors plage, range/thumb/page mutations, horizontal/vertical, disabled et drag
avec capture.

## wxSpinButton — alpha

La grille est toujours horizontale (`spinbutt.cpp:90-126`) et best-size toujours
large (`:164-168`), malgré le défaut vertical. `wxSP_ARROW_KEYS` n’est pas câblé
et les RepeatButtons n’ont pas de noms UIA.

**Plan :** grille lignes/colonnes selon style, glyph/order cohérents, Up/Down,
wrap/bornes/veto, noms « augmenter/diminuer » et preferred focus target.

**Tests :** vertical/horizontal, press/hold, clavier, wrap, bornes, veto,
disabled, DPI et Narrator.

## wxSpinCtrl / wxSpinCtrlDouble — alpha bloquant

### Causes à traiter

- valeur initiale non bornée (`spinctrl.cpp:106-113,279-286`) ;
- wrap absent ;
- SetSelection vide (`:185-188,368-370`) ;
- entier refuse base 16 (`:176-182`) ;
- snap-to-ticks seulement mémorisé dans le header ;
- events Enter/Text non adaptés; seul ValueChanged ;
- double applique `m_digits=0` sans dériver de l’incrément (`:279-295`) ;
- SetIncrement ne recalcule pas digits (`:339-343`) ;
- SetValue("") ignoré.

### Plan

1. Modèle canonique value/min/max/increment/digits/wrap, clamp initial compris.
2. Faire passer chaîne vide, wrap et digits automatiques.
3. Réutiliser `TextPeerAdapter` pour sélection/Enter/validation.
4. Appliquer snap lors de validation et des flèches.
5. Fournir parseur/formatter base 16, ou exclure explicitement cette capacité de
   l’alpha avec fallback clair; elle doit être présente pour beta.
6. Dédoubler `wxEVT_TEXT`, `TEXT_ENTER`, `SPIN`, `SPINCTRL` selon source.

### Tests

Ramener les 26 échecs à zéro : init hors plage, wrap, flèches, empty, base 16,
digits/increment, snap, selection, Enter, events exacts, locales décimales,
disabled, DPI et destroy pendant édition.

## Ordre et critères

1. Slider/ScrollBar event contract.
2. SpinCtrl modèle et tests actuels.
3. SpinButton orientation/clavier.
4. Gauge progress integration.
5. UIA/tooltips/styles beta.

Alpha : aucune suite rouge; setters programmatiques silencieux; un event wx par
action logique; disabled effectif; valeurs modèle et peer toujours identiques.

