# wxWinUI — état des lieux au 21 novembre 2026

Document de reprise. Il remplace, comme point d'entrée, les 57 fichiers de
`plans/` produits entre juillet et octobre. Tout ce qui est écrit ici a été
relevé dans le dépôt, sur GitHub ou dans le fil Codex `019f85c0` ; ce qui n'a
pas été vérifié est marqué comme tel.

**Non vérifié dans cette passe :** aucun exécutable n'a été lancé. Le
comportement à l'usage (lag, crashs, rendu) vient de tes messages, des retours
de PBfordev sur la PR et des notes de Codex, pas d'un test fait aujourd'hui.

## 1. Où est le code

| Emplacement | Contenu | État |
|---|---|---|
| `C:\wxWidgets-master` (`feature/winui-port-prototype`, `33fdd2e558`, 24 août) | Ancienne copie de travail | **Périmée** : 703 commits de retard sur la branche de la PR, 27 fichiers modifiés non commités, un stash |
| `fork/feature/winui-port-prototype` | Tête de la PR 26890 | Dernière activité GitHub le 7 octobre |
| `F:\wxwinui-pr26890-feedback` (`codex/pr26890-feedback`, `07a5d9043f`) | **Dernier état réel du port** | 53 commits **non poussés**, plus 113 fichiers modifiés (+15 341 lignes) et 72 fichiers neufs **non commités** |
| `F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib` | Dernières démos compilées (4 octobre, 22 h 52) | showcase, widgets, dataview, grid, dialogs, ribbondemo, minimal |

Le travail d'octobre (DataView, PropertyGrid, Ribbon, AUI « natifs »,
projections de scrollbars) n'existe donc que sur le disque F:, sans sauvegarde
distante. Il y a par ailleurs 10 worktrees git, 15 dossiers `build*` dans
`C:\wxWidgets-master` et environ 150 dossiers `wrc*` / `wxwinui-*` sur F:.
C: n'a plus que 37 Go libres.

## 2. Les chiffres

Mesurés sur `F:\wxwinui-pr26890-feedback` (copie de travail) par rapport à son
point de divergence avec `master`.

| Mesure | Valeur |
|---|---|
| Diff total | 696 fichiers, +420 354 / −8 484 lignes |
| `src/winui` | 168 059 lignes (91 fichiers) |
| Pour comparaison | `src/msw` 152 222 · `src/gtk` 62 878 · `src/qt` 27 756 |
| Tests ajoutés | 154 000 lignes (179 fichiers) |
| Code partagé modifié | `src/common` + `src/generic` +25 259 · `src/msw` +23 019 · aui/ribbon/propgrid +23 369 |
| `#ifdef __WXWINUI__` hors `src/winui` | 782 occurrences dans 150 fichiers (75 dans `src/msw/window.cpp`, 42 dans `src/generic/grid.cpp`) |
| Plus gros fichiers | `textctrl.cpp` 14 784 · `tlwhost.cpp` 12 148 · `treectrl.cpp` 10 052 · `combobox.cpp` 9 916 · `notebook.cpp` 9 086 · `toolbar.cpp` 8 594 · `calctrl.cpp` 8 410 |
| Fil Codex | 21 juillet → 4 octobre, 323 messages de ta part, environ 65 millions de tokens, objectif laissé en pause en plein tour |

Un port complet de wxWidgets vers Qt tient en 27 756 lignes. Ici la seule
couche WinUI, posée par-dessus wxMSW qu'elle réutilise, en fait six fois plus.

## 3. Ce que l'amont demande

PR [wxWidgets/wxWidgets#26890](https://github.com/wxWidgets/wxWidgets/pull/26890),
brouillon, en conflit avec `master`.

- **vadz, 23 août :** rebaser ou fusionner `master` ; ajouter un job CI qui
  compile le port. Fait à l'époque, à refaire (conflits revenus).
- **vadz, 30 août :** découper la PR en PR beaucoup plus petites et relisibles,
  « right now I simply have no idea how to even start looking at this ».
  **Non fait.** La PR a grossi depuis.
- **vadz, issue #19352, juin :** souhaite pouvoir mélanger contrôles Win32 et
  WinUI dans une même appli pour une migration progressive (`wxWinUI::Button`
  et `wxMSW::Button`). **Non fait** : l'ADR 0004 a retenu deux toolkits
  séparés à la compilation.
- **vadz, 21 juillet :** demande si le port peut entrer dans wx 3.4 / 4.0,
  attendu fin 2026 ou début 2027.
- **PBfordev :** débordement de pile quand la souris sort d'un dialogue modal
  (corrigé le 25 août, revenu le 30) ; couleurs du mode clair fausses, donc
  DataView/Grid/PropGrid vides ; hauteur des `wxTextCtrl` une ligne trop
  faible ; fuites mémoire à la fermeture de `minimal` ; le dialogue ajoute une
  entrée dans la barre des tâches ; barre d'outils de debug XAML non
  désactivable ; question sur `LC_NUMERIC` forcé à `C`.

## 4. Tes signalements encore ouverts

Repris de tes messages du 25 août au 4 octobre. Codex a noté des correctifs
pour plusieurs d'entre eux, mais aucun n'a été confirmé par toi après le
dernier build ; je les considère tous ouverts.

- Poignée de redimensionnement en bas à droite : inactive, puis ne redimensionne
  qu'au relâchement, puis suit la souris mais revient à la taille initiale.
- Redimensionnement de fenêtre saccadé.
- Ancienne scrollbar qui réapparaît ou clignote pendant un déplacement ou un
  redimensionnement.
- Composants classiques qui clignotent au rafraîchissement ; aperçu de police
  invisible en HDR. Mica doit rester.
- `wxMessageDialog` et d'autres dialogues qui gèlent ou plantent ; crashs dans
  `dialogs.exe`.
- `wxComboCtrl` : crash au clic. `wxRadioBox` : boutons inertes dans Showcase.
- DataView : tout se redessine à chaque interaction, lag au redimensionnement
  des colonnes et au dépliage, pas d'adaptation en direct à l'espace
  disponible, réordonnancement des colonnes peu lisible.
- Listes : lignes trop basses, contenu coupé, composants manquants.
- PropertyGrid : pas un vrai port, éditeurs visibles seulement à la sélection.
- Ribbon : pas un vrai rendu WinUI ; absent de l'onglet Advanced à un moment.
- Onglets Notebook / AuiNotebook pas Fluent. FileCtrl inutilisable.
- CollapsiblePane et première page de Showcase : zone de défilement non
  recalculée sans redimensionner la fenêtre.
- Calendrier : avertissement « day-layout validation failed to converge » en
  défilement rapide.
- `widgets.exe` en mode clair : texte de la barre latérale illisible.
- Avertissement « WinUI TextBox value failed with HRESULT 0x80070005 ».

Ta règle, répétée plusieurs fois : un contrôle générique repeint avec des
coins arrondis et une couleur d'accent **n'est pas** un port WinUI.

## 5. Pourquoi ça lag, ça bugue, et pourquoi ça ne converge pas

### 5.1 Deux arbres de fenêtres maintenus en parallèle

Chaque contrôle wx garde un vrai HWND Win32 (la « coquille »), et en plus un
élément XAML placé dans une île unique qui recouvre toute la zone client. Les
panneaux et les contrôles génériques restent des HWND peints en GDI **sous**
cette île. Rien de tout cela n'est natif pour Windows ; tout est émulé :

- **Souris.** L'île reçoit toute l'entrée. Pour chaque mouvement,
  `OnRootPointer` (`src/winui/hostinput.cpp:3866`) reconstruit un échantillon,
  décide si XAML possède le point, sinon cherche le HWND dessous et lui
  re-synthétise des messages Win32. C'est une réimplémentation de USER32 :
  capture, survol, double-clic, curseur, `WM_NCHITTEST`. D'où le lag au
  pointeur, la poignée de redimensionnement, les récursions jusqu'au
  débordement de pile dans les dialogues.
- **Géométrie.** La position XAML suit celle du HWND en différé (`CallAfter`
  puis `FlushSync`, `src/winui/hostgeometry.cpp:543`), et chaque flush
  recalcule aussi la région de fenêtre de l'île (`RebuildBridgeRegion`). D'où
  le redimensionnement saccadé et le contenu qui traîne.
- **Ordre d'empilement.** Une seule bande XAML au-dessus de tous les HWND :
  l'ordre `XAML > HWND > XAML` est impossible (ADR 0003). D'où les trous, les
  contrôles classiques qui disparaissent ou clignotent, l'ancienne scrollbar
  visible sous la nouvelle, et les défauts HDR + Mica du contenu GDI.
- **Focus, clavier, accessibilité, drag-and-drop.** Deux autorités à
  réconcilier à chaque fois : hook `WH_GETMESSAGE`, double arbre UIA, broker
  OLE « double surface ».

Chaque réconciliation a été traitée par une machine d'état de plus :
générations, époques, tombstones, quarantaines, registres de « retirement ».
C'est ce qui remplit 168 000 lignes, et c'est pour cela qu'un correctif en
casse un autre.

### 5.2 Des tests verts sur un produit cassé

154 000 lignes de tests déterministes, des campagnes « RC1 à RC8 » avec
empreintes SHA-256, et pourtant la poignée de redimensionnement n'a jamais
marché : son test remplaçait le chemin de production au lieu de l'observer
(relevé par l'audit du 30 août, §9.3). Le temps est parti dans la preuve
plutôt que dans l'usage.

### 5.3 Des « projections » à la place de vrais contrôles

`listprojection`, `gridprojection`, `headerprojection`, `scrollbarprojection`
superposent des éléments XAML à des contrôles génériques qui continuent de
vivre dessous. C'est exactement ce que tu refuses, et c'est une troisième
couche à synchroniser.

### 5.4 Une dette de processus

57 fichiers de plans, des statuts « LOCAL PASS / PHYSICAL PENDING », une
documentation contredite par le code de la même révision, 150 dossiers de
build. Plus personne, agent compris, ne savait ce qui était fait : c'est
l'épisode du 29 juillet où les phases 008 ont été refaites.

## 6. Ce qui a de la valeur et se garde

- **Build et déploiement** : `build/cmake/winui.cmake`, paquets NuGet épinglés,
  génération C++/WinRT, `resources.pri`, test de fumée runtime. Jugé de bonne
  qualité par l'audit, réutilisable presque tel quel.
- **Amorçage et arrêt du runtime** XAML (`src/winui/winui.cpp`), à alléger.
- **Le savoir-faire par contrôle** : quel contrôle XAML pour quel contrôle wx,
  quels événements, quels pièges (ordre des événements du ComboBox, sous-menus
  `MenuFlyout`, `LC_NUMERIC`, réalisation des gabarits). Les fichiers sont cinq
  à dix fois trop gros, mais ce qu'ils savent est juste.
- **Le job CI**, les samples `showcase` et `winuispike`, et la liste du §4, qui
  devient la recette d'acceptation.
- **La dépendance native trouvée pour le Ribbon** (`XamlToolkit.Labs.WinUI`,
  port C++ du Community Toolkit).

## 7. Décision du 21 novembre

Trois directions avaient été envisagées : stabiliser l'existant tel quel,
refonder la couche fenêtre sans HWND par contrôle, ou revenir à une île par
contrôle. La deuxième, que je recommandais, **a été refusée** : elle
sacrifiait le HWND par contrôle.

Les exigences retenues sont :

1. un vrai HWND par contrôle, avec `GetHandle()` et les messages Win32
   fonctionnels, pour que changer de toolkit soit un simple drapeau de build ;
2. tous les composants en vrai WinUI 3, sans repli ;
3. pas de réécriture : audit, correction, simplification, achèvement.

L'architecture qui les satisfait est décrite dans `ARCHITECTURE-CIBLE.md` :
on garde les deux arbres (HWND et XAML) mais on sépare leurs rôles et on leur
donne la même forme. Elle s'atteint en sept étapes ; l'étape 0 a été lancée le
21 novembre.

## 8. État du dépôt après nettoyage (21 novembre)

- `C:\wxWidgets-master` est sur la branche locale `winui/reprise`, qui
  contient tout le travail d'octobre. Rien n'est poussé.
- L'ancien état de cette copie est dans la branche
  `backup/c-checkout-20261121` et dans un stash.
- 157 dossiers de build et d'instantanés ont été supprimés (58 Go).
- Les builds se font dans `F:\wxb` : `reprise-dll` pour le port,
  `msw-dll` pour wxMSW classique, qui sert de référence.
- Conservés : `F:\kicad-winui-test`, `F:\winui-tableview-demo`, le cache NuGet
  `F:\wxwinui-maintainer-integration-packages`, les deux anciens dossiers de
  démos `F:\wxwinui-pr26890-feedback-build*` et l'ancien worktree
  `F:\wxwinui-pr26890-feedback`, dont le contenu est dans la branche.

## 9. Mesures de référence

Banc : 300 redimensionnements de la fenêtre principale par `SetWindowPos`, sur
un bureau Windows invisible. Il mesure le calcul, pas l'affichage.

| Démo | wxMSW classique | Port WinUI |
|---|---|---|
| Showcase (178 contrôles WinUI) | 83 ms par pas | 69 ms par pas |
| Widgets | 32 ms | 24 ms |
| Minimal | 4,2 ms | 4,3 ms |

Le coût de mise en page n'est donc pas propre au port : il vient de wx, que
Showcase applique à toutes ses pages. La synchronisation de l'île coûte
environ 5 ms par pas dans Showcase. Ce que ce banc ne voit pas, et qui reste à
mesurer, c'est le décalage visuel entre les HWND et le XAML.
