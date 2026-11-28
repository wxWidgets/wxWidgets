# Audit des 61 commits WinUI

## Méthode

Trois sous-agents ont relu en parallèle les plages 1–20, 21–40 et 41–61,
puis le résultat a été recoupé contre le code courant et le worktree. La plage
est `5bd892f7ac21..20207bdbbb81`. Les 61 diffs passent `git diff-tree --check`;
cela ne valide ni contrats API ni exécution.

Légende : **OK** = objectif du commit cohérent; **Partiel** = base utile mais API
ou validation incomplète; **À corriger** = défaut survivant; **Superseded** =
remplacé par une architecture ultérieure; **Docs/sample** = pas une preuve de
conformité.

## Commits 1–20 — fondation et première vague de contrôles

| # | SHA | Objet | Verdict courant |
|---:|---|---|---|
| 1 | `4e34052138` | vertical slice expérimental | **Partiel.** Fondation utile; D&D, contrats Text/Combo et déploiement SDK/NuGet restent. L’hébergement par contrôle est superseded. |
| 2 | `1b92ff9cf2` | amélioration contrôles | **Partiel.** États/apparence restent propagés classe par classe. |
| 3 | `1be3c58d02` | clavier vers île focalisée | **À corriger.** A réglé AZERTY/double frappe, mais le direct-dispatch contourne maintenant les accélérateurs wx. |
| 4 | `4351bd896d` | Mica | **Superseded en partie.** Le backdrop TLW subsiste; le priming différé est fragile. |
| 5 | `0fc1e86bb2` | Gauge/Toggle | **Partiel.** Gauge de base solide; Toggle et disabled-state partagé à revalider. |
| 6 | `36049a6922` | merge origin/master | **OK.** Aucun défaut WinUI isolé attribuable. |
| 7 | `dbf4543ff1` | ListBox/CheckListBox | **À corriger.** Ownership client data, multisélection, rebuild et événements. |
| 8 | `bc6a28e2ae` | Notebook/TabView | **Partiel.** Top tabs utilisables; autres orientations et API de géométrie manquent. |
| 9 | `d4421dff8d` | Hyperlink/Activity | **Partiel.** Activity proche; hover/state du lien incomplets. |
| 10 | `0d092b4932` | StaticBitmap | **OK alpha.** Tests actuels verts; refresh bundle DPI reste. |
| 11 | `71f86150f1` | SpinCtrl/Double | **À corriger.** 26 assertions upstream échouent : valeur, events, base, précision, sélection. |
| 12 | `63b422abd1` | RadioBox | **Partiel.** Core présent; Show/Enable item et apparence/UIA incomplets. |
| 13 | `6ae8a0aea1` | Date/TimePicker | **Partiel.** Range/focus/style/format à compléter; deux échecs DatePicker observés. |
| 14 | `e417bf9bed` | SearchCtrl | **À corriger.** Le titre surestime le contrat TextEntry/menu/cancel. |
| 15 | `0b38b459fd` | SpinButton | **Régressé plus tard.** L’orientation du commit a été perdue par `db2232ea02`. |
| 16 | `558e08e037` | InfoBar | **À corriger P0.** Header incompatible avec API commune XRC/tests. |
| 17 | `a27d18ac4f` | Calendar | **Partiel.** navigation events, Mark, EnableMonthChange et hit-test double-clic manquent. |
| 18 | `2f9801fc7b` | ColourPicker | **À corriger.** Event child/ID faux, malgré tests API statiques verts. |
| 19 | `aa38183531` | BitmapComboBox | **À corriger.** association bitmap/string et taille DPI; 6 assertions échouent. |
| 20 | `0f0c3fe4fd` | ScrollBar | **À corriger.** état non clampé et `CHANGED` prématuré/dupliqué. |

## Commits 21–40 — chrome, API pass et consolidation

| # | SHA | Objet | Verdict courant |
|---:|---|---|---|
| 21 | `4ba4807431` | StaticBox/Tree + generic header/list | **Partiel.** Tree enrichi ensuite; StaticBox couleurs/masque fixes; data controls restent génériques. |
| 22 | `184e396401` | pickers dans sample | **Docs/sample.** Exerce des composites génériques, sans migration native ni tests. |
| 23 | `74ce30922b` | backdrop TLW automatique | **Partiel.** Bon axe; fallback/priming/multi-TLW non qualifiés. |
| 24 | `78eef5a085` | masque StaticBox | **À corriger.** Couleurs codées en dur, conflit Mica/fonds custom. |
| 25 | `7674b67413` | MenuBar XAML | **À corriger.** Souris utile; accélérateurs et contrats événements incomplets. |
| 26 | `db2232ea02` | SpinButton horizontal + StaticText | **À corriger.** Ignore `wxSP_VERTICAL`; ellipsize/font partiels. |
| 27 | `744a702e6d` | checkpoint | **Superseded en partie.** Bouton remplacé; callback Tree/tooltip encore pertinents. |
| 28 | `f10c931a2d` | rewrite Button | **Partiel, base alpha utile.** `Button::Click` est rouge; focus/default/clavier dialogue restent. |
| 29 | `1dc22e0db2` | cursors/dialog helpers/log | **Partiel.** Curseurs busy/custom et slots animés non couverts. |
| 30 | `a71db60f92` | dialogs + StatusBar | **Superseded/partiel.** Presenter refondu; crash TextDialog corrigé seulement dans worktree; StatusBar partielle. |
| 31 | `cda2b63980` | checkbox/radio/text/menu pass | **À corriger.** 3-state, frontières radio, dirty/caret/max-length. |
| 32 | `5953d0f5c1` | showcase | **Docs/sample.** Bonne galerie manuelle, pas un test de non-régression. |
| 33 | `2af83862fb` | setup `wxUSE_WINUI3` | **OK.** Plumbing cohérent. |
| 34 | `1c838179a9` | popup choice dark MSW | **Hors backend WinUI.** Correctif wxMSW classique. |
| 35 | `3280a44ed3` | progress notes | **Docs à actualiser.** Plusieurs annonces contredites par les tests/cutover. |
| 36 | `e3f70efa90` | ComboBox editable | **Partiel.** caret/template, readonly transition, `wxCB_SIMPLE`. |
| 37 | `7f5218c80b` | événements Slider | **À corriger.** Toutes les sources deviennent THUMBTRACK; fins parasites. |
| 38 | `30fa89ac97` | caret/selection TextCtrl | **Partiel.** vraie sélection ajoutée; SetValue/max-length/hit-test faux. |
| 39 | `291ca426b4` | API fixes transverses | **Partiel.** Search, Toggle bitmap, Hyperlink hover restent. |
| 40 | `a40a7b4676` | icônes Notebook | **À corriger.** Pas d’`OnImagesChanged()`/refresh DPI/source. |

## Commits 41–61 — dialogues, Tree, menus, renderer et derniers fixes

| # | SHA | Objet | Verdict courant |
|---:|---|---|---|
| 41 | `d8c765417b` | dialogues app-modal | **Superseded en partie.** Owner/modeless relève du presenter courant. |
| 42 | `852086f3a6` | Tree interactivité | **Partiel.** `textOnly` et hit-test X incorrects. |
| 43 | `cc44caeb59` | petits gaps API | **Partiel.** Date ALLOWNONE/gauge vertical/listbox utiles; Calendar double-click trop large. |
| 44 | `676b93f4cb` | label ColourPicker | **À corriger.** Le label ne suit pas un choix utilisateur. |
| 45 | `96dd51fcdc` | ColourDialog | **OK alpha.** Palette custom/ChooseFull ignorés. |
| 46 | `60018bc43a` | popup MenuFlyout | **À corriger.** Ordre/routage wx et `TryEnqueue` liveness. |
| 47 | `c9ce9f5092` | TREE_KEY_DOWN | **À corriger.** Le handler ne peut pas marquer la touche XAML handled. |
| 48 | `73265906e7` | progress | **Docs historiques.** Validation insuffisante déclarée honnêtement, maintenant périmée. |
| 49 | `006f61bf05` | compilation | **OK.** Correctifs ciblés cohérents. |
| 50 | `ab7d4a6914` | DialogIsland factorisé | **Superseded.** Remplacé par host partagé/TLW. |
| 51 | `87d19c50b9` | font/find génériques | **Partiel.** Choix raisonnable; re-fit initial absent. |
| 52 | `e69a11f706` | Tree drag gestures | **Titre trompeur.** BEGIN/END_DRAG interne, pas D&D OLE. |
| 53 | `1539be711b` | progress phases 1–2 | **Docs périmées.** Surestime D&D et ancien DialogIsland. |
| 54 | `9368e31ab8` | ToolBar CommandBar | **À corriger.** suppression recrée l’outil; toggle/control/dropdown/overflow partiels. |
| 55 | `bbc2098c70` | renderer Fluent | **Incorrect isolément.** Adresse de renderer statique confiée à un owner. |
| 56 | `b26f08747e` | RichToolTip TeachingTip | **Partiel.** API rect/delay/style et lifetime incomplets. |
| 57 | `2b736e6091` | progress phases 3–4 | **Docs historiques.** Transitoires désormais différents. |
| 58 | `9a4951fee7` | renderer heap | **OK.** Corrige l’ownership de `bbc2098c70`. |
| 59 | `e4dccfdea4` | taille premier rendu | **OK ciblé.** Ne fait pas grandir une TLW devenue trop petite. |
| 60 | `6b20668e67` | dialogues en Window par défaut | **Architecture utile, divergence actuelle.** Le worktree a remis Overlay sans aligner le contrat public. |
| 61 | `20207bdbbb` | labels/tree icons/toolbar | **OK ciblé.** Les changements Tree postérieurs ajoutent cache/perf à corriger. |

## Bugs explicitement résolus dans l’historique/session

- double frappe/AZERTY par routage clavier (`1be3c58d02`), mais avec dette
  accélérateurs à réparer sans régression ;
- ownership du renderer statique (`9a4951fee7`) ;
- taille initiale de nombreux peers (`e4dccfdea4`) ;
- toolbar cachée/labels coupés/icônes Tree fantômes (`20207bdbbb`) ;
- starvation XAML dans les boucles natives via hook `WH_GETMESSAGE` (worktree) ;
- lag owner pendant move/size via detach de la fenêtre active (worktree) ;
- curseur statique qui revenait à la flèche (worktree) ;
- freeze à rafale MOVE identique via breaker (worktree, soak incomplet) ;
- crash retardé du constructeur non-default `wxTextEntryDialog` via NSDMI
  (`include/wx/winui/textdlg.h`, worktree).

## Conclusion sur la stratégie de commits

Ne pas squasher la refonte courante en un commit. Après validation utilisateur,
la découpe minimale est :

1. build/runtime + API compile;
2. host TLW/slots/cutover;
3. prétraduction clavier et fallback hook;
4. dialogues frame-class/presenter/owner;
5. routeur input + freeze breaker;
6. cursors/tooltips/UIA;
7. correctifs Tree/toolbar/renderer;
8. text dialog NSDMI;
9. spike/logs opt-in ou suppression.

Chaque commit doit compiler seul et indiquer quels tests ciblés passent. Aucun
commit ne doit être créé avant l’accord explicite de l’utilisateur.
