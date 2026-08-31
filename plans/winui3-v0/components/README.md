# Fiches de migration par famille

Chaque fiche est conçue pour être relue et appliquée ultérieurement. Elle donne
le statut de chaque composant, les contrats manquants, l’ordre de travail, les
tests et la frontière alpha/beta/post-beta.

Ensemble, la matrice et ces 14 fiches couvrent les 43 fichiers `.cpp` dédiés de
`src/winui` ainsi que les principaux composants common/generic/MSW conservés ou
manquants pour la V0. Une classe absente de la matrice reste hors promesse; son
absence ne signifie pas qu’elle est implicitement supportée.

| Fiche | Famille |
|---|---|
| [00-foundation-host.md](00-foundation-host.md) | host TLW, slots, `wxWindow`, `wxWinUIXamlHost` |
| [01-buttons-selection.md](01-buttons-selection.md) | Button, Toggle, CheckBox, RadioButton/Box, CommandLink |
| [02-text-entry.md](02-text-entry.md) | TextCtrl, SearchCtrl, ComboBox et adaptateur texte |
| [03-item-controls.md](03-item-controls.md) | Choice, BitmapCombo, ListBox, CheckList, ComboCtrl/composites |
| [04-range-spin.md](04-range-spin.md) | Gauge, Slider, ScrollBar, SpinButton, SpinCtrl/Double |
| [05-date-time-calendar.md](05-date-time-calendar.md) | DatePicker, TimePicker, Calendar |
| [06-display-feedback.md](06-display-feedback.md) | statiques, activity, hyperlink, InfoBar |
| [07-books-trees.md](07-books-trees.md) | Notebook/books, TreeCtrl/TreeList |
| [08-menus-chrome-tooltips.md](08-menus-chrome-tooltips.md) | menus, toolbar, statusbar, tooltip/richtooltip |
| [09-dialogs-pickers-transients.md](09-dialogs-pickers-transients.md) | dialogs, pickers et surfaces transitoires |
| [10-data-and-generic-controls.md](10-data-and-generic-controls.md) | ListCtrl, Header, DataView, Grid, PropertyGrid et fallbacks |
| [11-advanced-rich-surfaces.md](11-advanced-rich-surfaces.md) | AUI, Ribbon, richtext, STC, HTML, WebView, Media |
| [12-toplevel-platform.md](12-toplevel-platform.md) | TLW/frame/dialog shell, MDI, taskbar, drawing, GL |
| [13-generic-foundation-aux-ui.md](13-generic-foundation-aux-ui.md) | Panel, scrolling, splitter/sash, NativeWindow et UI auxiliaire |

Ordre recommandé : 00 → 08/09 → 13 (fondation alpha) → 01–07 → 10 → 11/12.
Les plans exécutables
numérotés à la racine `plans/` restent l’autorité pour les dépendances et les
conditions STOP.
