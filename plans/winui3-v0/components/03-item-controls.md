# Fiche 03 — Choice, listes, BitmapCombo et composites d’items

Baseline `20207bdbbb`, dépend des fiches 00 et 02. Fichiers principaux :
`choice.cpp`, `bmpcbox.cpp`, `listbox.cpp`, `checklst.cpp`, `combobox.cpp` et
`src/generic/combog.cpp`/`odcombo.cpp`.

## Direction commune : modèle d’items stable

Créer un adaptateur interne qui possède pour chaque item une identité, texte,
client data, bitmap, check et sélection. Insérer/supprimer/mette à jour par delta
au lieu de `Items().Clear()`. Employer un seul comparateur wx pour les styles
triés et une seule politique d’ownership `wxClientData`.

## Gates observés

| Composant | Échecs |
|---|---:|
| Choice | 4 |
| ComboBox | 8, traité aussi fiche 02 |
| BitmapComboBox | 6 |
| ListBox | 9 |
| CheckListBox | dépend ListBox; pas de compte séparé |

## wxChoice

**Alpha :**

- remplacer `CmpNoCase()` (`choice.cpp:217-225`) par l’ordre attendu par wx ;
- remplacer best size fixe 180×32 (`:253-255`) par mesure contenu+flèche+padding ;
- utiliser l’event helper commun avec event object/client data (`:335-342`) ;
- sous sort, `SetString()` repositionne l’item ou met à jour le modèle de façon
  cohérente ;
- enabled via slot.

**Beta :** font/couleurs/tooltip/border, bitmap items et UIA. Corriger dimensions
image physiques utilisées comme DIPs (`:283-303`).

**Tests :** suite Choice, ordre casse/Unicode, hidden best size, SetString trié,
void/client object data, selection event, 100/200 % et UIA item.

## wxBitmapComboBox

`Insert()` insère d’abord la string puis écrit la bitmap à l’index retourné
(`bmpcbox.cpp:57-108`), alors que le stockage commun ne crée que des slots de
fin (`src/common/bmpcboxcmn.cpp:62-77`).

**Alpha :** surcharger le chemin interne d’insertion pour insérer bitmap et
string atomiquement à l’index final trié; rendre delete/clear symétriques; appeler
`OnAddBitmap()` afin que `GetBitmapSize()` soit valide.

**Beta :** champ fermé en editable, refresh bundle DPI, tailles DIPs, UIA texte.

**Tests :** A/B puis X à 0, sorted insert, client data, delete/clear, bitmaps de
tailles différentes, closed field, DPI 100/150/200.

## wxListBox

**P0/P1 alpha :**

- destructeur vide (`listbox.cpp:71-73`) : libérer les object client data une
  fois, en suivant `src/common/ctrlsub.cpp:91-102` ;
- comparateur `CmpNoCase()` (`:416-424`) ;
- implémenter `DoListHitTest`, actuellement stub commun ;
- consommer AddedItems/RemovedItems, pas seulement `SelectedIndex`
  (`:105-114`) ;
- conserver la multisélection et le scroll à travers insert/delete/SetString/check;
- clear multiple via SelectedItems ;
- double-click uniquement sur item hit-testé ;
- synchroniser `m_oldSelections` après changements programmatiques.

**Beta :** SetFirstItem réellement top-visible, styles ownerdraw/int-height,
font/couleurs/tooltip/enabled/UIA et virtualisation.

**Tests :** toute la suite ListBox, destructeurs client data comptés, Ctrl-select
plusieurs items puis chaque mutation, événements différentiels exacts, clic vide,
hit-test DPI, scroll/top item et disabled.

## wxCheckListBox

`Check()` reconstruit toute la liste via `WinUIRefreshItems()`
(`checklst.cpp:22-62`), perdant sélection/scroll/focus. Après modèle stable
ListBox, modifier uniquement l’état check de l’item.

**Tests alpha :** Check programmatique sans event; clic utilisateur = un seul
CHECKLISTBOX; sélection/scroll conservés; Space; tri/insertion/delete; client data.

**Beta :** disabled item, UIA ToggleState et Narrator.

## wxComboCtrl / wxOwnerDrawnComboBox

Fallback générique sélectionné sous WinUI (`include/wx/combo.h:867-874`) avec
adaptations dans `src/generic/combog.cpp:454+`. Le conserver pour V0.

**Beta gate :** popup GDI au-dessus/à côté du bridge, TextCtrl child slotté,
focus/Tab, capture, scroll, DPI/theme, owner-draw, D&D et destruction popup
ouvert. Ne pas engager une migration XAML avant stabilisation de ComboBox.

## Rearrange et EditableListBox

Composites communs dépendant respectivement de CheckListBox/dialogues et
ListCtrl/editor. Pour beta : exécuter leurs tests après les composants de base,
vérifier add/delete/reorder, focus, D&D et modal semantics. Aucun peer natif
spécifique n’est recommandé pour V0.

## Ordre de travail

1. Ownership et comparateur commun.
2. Identité stable et deltas items.
3. Sélection/hit-test/events ListBox.
4. BitmapCombo atomicité/mesure.
5. CheckList mutation locale.
6. Fallback ComboCtrl/Rearrange/Editable qualification.

## Sortie alpha

- Choice, ListBox, CheckList et BitmapCombo tests ciblés verts ;
- destructeurs client object exactement une fois ;
- insert/delete/sort ne désassocie aucune donnée ;
- sélection multiple, scroll et focus survivent aux mutations ;
- aucune reconstruction destructive pendant un simple Check/SetString.

