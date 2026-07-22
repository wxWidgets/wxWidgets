# Plan 008 — Fermer les contrats composants bloquant la V0

> **Instructions** : **ce fichier est un EPIC d’orchestration, pas un lot à
> implémenter d’un bloc.** Exécuter 008a–008f par familles, une suite verte avant
> la suivante. Ne pas ajouter de nouveau composant natif. Lire les fiches 01–08
> et 13 selon le lot. Aucun commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/winui include/wx/winui tests/controls`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`. Réconcilier toute dérive avant édition.

## Statut

- **Priorité** : P1 (P0 pour TextCtrl et InfoBar)
- **Effort** : XL cumulé; sous-plans M/L
- **Risque** : HIGH
- **Dépend de** : dépendances fines 008a–008f, pas de barrière globale 002–007
- **Catégorie** : EPIC, bugs, compatibility, migration, tests
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

Les titres de commits donnent une impression de complétude que les tests ne
confirment pas. Une V0 solide se définit par un sous-ensemble public explicite et
testé, pas par le nombre de fichiers `src/winui`.

## Baseline dynamique à éliminer

| Famille | Échecs connus |
|---|---:|
| TextCtrl | ~26 + crash intermittent |
| SpinCtrl/Double | 26 dans 13 cas |
| TreeCtrl | 25 dans 8 cas |
| ListBox | 9 |
| ComboBox | 8 |
| BitmapComboBox | 6 |
| RadioBox | 6 |
| RadioButton | 5 |
| Choice | 4 |
| Slider | 4 |
| SearchCtrl | 3 |
| Notebook | 3 erreurs |
| DatePicker | 2 |
| Button/Toggle | 1 chacun |

Gates vertes à préserver : CheckBox 20 assertions, Gauge 10, ColourPicker 14,
StaticBitmap 506.

## Scope

Implémentations natives existantes et leurs tests. Hors scope : nouvelle
migration Grid/DataView/AUI/Ribbon/etc.; infrastructure déjà couverte plans
003–007; migrations post-beta.

## Sous-plans faisant autorité

| Sous-plan | Famille | Dépend de |
|---|---|---|
| [008a](008a-winui-text-peer.md) | TextCtrl et SearchCtrl | 003, 004, 007b |
| [008b](008b-winui-item-model.md) | Choice, Combo, List, CheckList, BitmapCombo | 007a, 008a |
| [008c](008c-winui-range-spin-date.md) | Gauge, Slider, ScrollBar, Spin, Date/Time/Calendar | 003, 007a, 007b |
| [008d](008d-winui-actions-appearance.md) | Button, Toggle, Check, Radio et statiques | 002, 003, 006, 007b |
| [008e](008e-winui-notebook-tree-books.md) | Notebook, TreeCtrl et books | 005, 007a–007c, 008a–008b |
| [008f](008f-winui-chrome-feedback.md) | Toolbar, status, tooltips, InfoBar et polish | 002–004, 006, 007a–007c, 008d–008e |

La baseline et l’ordre ci-dessous résument l’epic; chaque implémentation suit le
scope, les Verify, Done et STOP de son sous-plan.

## Ordre d’exécution

### Lot A — crash et build

1. TextCtrl lifetime/crash, puis dirty/selection/readonly/max/hit-test
   ([fiche 02](winui3-v0/components/02-text-entry.md)).
2. InfoBar API/checkbox/measure déjà lancé par plan 002
   ([fiche 06](winui3-v0/components/06-display-feedback.md)).

**Gate** : 100 boucles TextCtrl sans crash; filtres TextCtrl et InfoBar alpha
verts avant lot B.

### Lot B — modèles partagés

3. ItemModel : Choice tri/measure/events, ListBox ownership/hit/selection,
   BitmapCombo atomicité, CheckList delta ([fiche 03](winui3-v0/components/03-item-controls.md)).
4. TextPeer : Search menu/cancel/selection, Combo editable/template/dropdown.

**Gate** : Choice/List/CheckList/Combo/BitmapCombo/Search filtres verts; client
data détruit exactement une fois.

### Lot C — plage et date

5. Slider/ScrollBar event state machine.
6. SpinCtrl/Double model, digits/base/wrap/selection/events; SpinButton orientation.
7. DatePicker range/focus; Calendar double hit/nav/marks si alpha; TimePicker beta.

**Gate** : Slider, Spin et Date tests verts; Gauge baseline conservée.

### Lot D — sélection/navigation

8. Button/Toggle clic E2E et bitmaps; CheckBox 3-state.
9. RadioButton groups/focus; RadioBox preferred target/item state.
10. Notebook geometry/orientations/images; TreeCtrl tests/lifetime/hit/perf.

**Gate** : tous filtres de la baseline verts pour le sous-ensemble alpha.

### Lot E — display/chrome polish requis alpha

11. StaticText/StaticBox appearance/DPI/UIA; StaticBitmap DPI.
12. Hyperlink/Activity/InfoBar beta selon promesse.
13. Toolbar/Status/Tooltips selon fiche 08.

## Commande standard par lot

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "<filtre>" --reporter compact
git diff --check
```

Chaque fix ajoute un cas de régression nommé; ne pas simplement adapter le test à
la sortie courante. Comparer aux backends MSW/generic comme oracle de contrat.
Pour chaque famille terminée, étendre une ressource de smoke XRC mixte et la
charger réellement dans `test_gui`; compiler le handler seul n’est pas un gate.

## Done alpha

- zéro crash/freeze et aucun callback après destroy ;
- tous tests des capacités alpha verts ;
- XRC crée/détruit chaque famille Supported avec ses styles principaux ;
- no-op public restant listé comme hors V0 ou remplacé par fallback ;
- disabled/focus/tooltip/accelerator communs appliqués ;
- samples minimal/widgets et pages `Supported Alpha` du showcase validés; les
  pages Experimental sont smoke/no-crash, les Excluded sont consignées sans
  bloquer le gate ;
- limites beta inscrites dans component matrix/docs.

## Done beta

- suites upstream ciblées intégrales vertes pour tous composants promis ;
- API/styles/events, client data et lifetime qualifiés ;
- DPI/RTL/theme/UIA/Narrator et interactions mixtes ;
- performance List/Tree et scroll budgets.

## STOP

- Un fix composant duplique une adaptation qui appartient au Slot/Text/Item
  adapter : revenir au seam commun.
- Le backend MSW et les tests expriment des contrats incompatibles : documenter
  les preuves et demander décision, ne pas choisir arbitrairement.
- Un lot nécessite une nouvelle migration complexe; utiliser fallback et
  reclasser post-beta.
