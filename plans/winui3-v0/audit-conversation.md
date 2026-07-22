# Audit de la conversation Claude — tâches, bugs et décisions

## Source et découpage

Conversation auditée :
`C:\Users\ferre\.claude\projects\C--wxWidgets-master-build-master\75517e86-2263-422d-90a3-b8f7850d2373.jsonl`,
7 273 enregistrements, environ 40 Mo, couvrant les 19–21 juillet 2026.

Trois sous-agents ont relu les segments 1–2 600, 2 500–5 100 et 5 000–7 273,
avec chevauchement volontaire. Le présent ledger agrège les tâches techniques;
il ne compte pas comme « tâche » chaque commande de diagnostic ou chaque
reformulation conversationnelle.

États : **FAIT** = code ou diagnostic produit; **VALIDÉ** = utilisateur/test a
confirmé le comportement; **PARTIEL** = implémenté mais contrat incomplet;
**OUVERT** = non livré; **SUPERSEDED** = remplacé par une décision ultérieure.

## 1. Première passe du port et composants

| ID | Tâche ou bug remonté | État à la fin de la conversation | Commentaire actuel |
|---|---|---|---|
| C-001 | Auditer le vertical slice WinUI initial | FAIT | A révélé architecture N îles, gaps clavier/focus/DPI/UIA/D&D. |
| C-002 | Corriger l’entrée clavier/AZERTY/double frappe | FAIT | Commit `1be3c58d02`; a créé la dette accélérateurs. |
| C-003 | Appliquer Mica/backdrop aux îles/TLW | PARTIEL | Backdrop TLW fonctionne; priming et lifecycle restent. |
| C-004 | Migrer Gauge | PARTIEL | Tests de base verts; `wxGA_PROGRESS` manque. |
| C-005 | Migrer ToggleButton | PARTIEL | Rendu/valeur présents; clic test, bitmaps d’état et enabled restent. |
| C-006 | Migrer ListBox/CheckListBox | PARTIEL | 9 assertions rouges; ownership, tri, hit-test, multisélection. |
| C-007 | Migrer Notebook | PARTIEL | TabView top présent; orientations/API géométriques/test rouge. |
| C-008 | Migrer Hyperlink/ActivityIndicator | PARTIEL | Core présent; hover/style/UIA à finir. |
| C-009 | Migrer StaticBitmap | FAIT alpha | 506 assertions vertes; refresh DPI restant. |
| C-010 | Migrer SpinCtrl/SpinCtrlDouble | PARTIEL | 26 échecs actuels; contrat loin d’être complet. |
| C-011 | Migrer RadioBox | PARTIEL | Rendu/selection présents; focus/enable/show/item et UIA. |
| C-012 | Migrer DatePicker/TimePicker | PARTIEL | DatePicker a deux échecs; styles/secondes/locales à décider. |
| C-013 | Migrer SearchCtrl | PARTIEL | Nombreux stubs TextEntry/menu/cancel. |
| C-014 | Migrer SpinButton | PARTIEL puis régressé | Orientation verticale perdue dans un commit ultérieur. |
| C-015 | Migrer InfoBar | PARTIEL | API publique incomplète casse désormais build XRC/tests. |
| C-016 | Migrer Calendar | PARTIEL | Navigation/events/Mark/DoubleTap précis manquent. |
| C-017 | Migrer ColourPicker | PARTIEL | Event identity et label utilisateur incorrects. |
| C-018 | Migrer BitmapComboBox | PARTIEL | Stockage bitmap/string désaligné et tests rouges. |
| C-019 | Migrer ScrollBar | PARTIEL | Event sequence et état canonique incorrects. |
| C-020 | Migrer StaticBox | PARTIEL | Couleurs/masque/font/DPI fragiles. |
| C-021 | Migrer TreeCtrl | PARTIEL | Fonction riche mais 25 assertions rouges et gaps hit-test/lifetime/perf. |
| C-022 | Utiliser ListCtrl/Header génériques | FAIT comme stratégie | À qualifier, pas à réécrire avant beta. |
| C-023 | Exercer File/Dir/FontPicker | FAIT sample | Fallback générique, pas migration native. |
| C-024 | Créer MenuBar XAML | PARTIEL | Clics présents; accélérateurs/contrat menu ouverts. |
| C-025 | Réécrire Button proprement | PARTIEL | Base visuelle utile, mais `Button::Click` reste rouge : pas FAIT alpha. |
| C-026 | Ajouter curseurs/log/helpers dialogues | PARTIEL | Curseur statique ensuite corrigé; busy/custom/animé restent. |
| C-027 | Ajouter Message/TextDialog/StatusBar | PARTIEL | Presenter a été refondu; StatusBar reste partielle. |
| C-028 | Faire une passe API checkbox/radio/text/menu | PARTIEL | Plusieurs écarts précis restent malgré le titre du commit. |
| C-029 | Construire showcase complet | FAIT | Harnais manuel, pas suite de conformité. |
| C-030 | Rendre ComboBox réellement editable | PARTIEL | Template/caret/dropdown/styles restent. |
| C-031 | Compléter événements Slider | PARTIEL | Émet la famille mais avec sémantique fausse. |
| C-032 | Suivre caret/sélection TextCtrl | PARTIEL | Sélection existe; contrats et lifetime encore rouges. |
| C-033 | Ajouter icônes Notebook | PARTIEL | Refresh source/DPI absent. |
| C-034 | Rendre dialogues app-modal | PARTIEL | Owner a causé lag et nouvelles règles detach. |
| C-035 | Ajouter ColourDialog | FAIT alpha | Custom colours/ChooseFull manquent. |
| C-036 | Ajouter Popup MenuFlyout | PARTIEL | Routage/order/liveness à corriger. |
| C-037 | Ajouter TREE_KEY_DOWN | PARTIEL | Ne peut pas neutraliser le comportement XAML. |
| C-038 | Utiliser Font/Find génériques | FAIT comme stratégie | Re-fit initial manquant. |
| C-039 | Ajouter Tree drag gestures | FAIT partiel | Ce n’est pas le D&D OLE général. |
| C-040 | Implémenter ToolBar | PARTIEL | L’utilisateur a validé l’apparence nominale; suppression et API restent. |
| C-041 | Installer renderer Fluent générique | FAIT après fix | Crash ownership corrigé par `9a4951fee7`. |
| C-042 | Ajouter RichToolTip TeachingTip | PARTIEL | API/lifetime/transient manager incomplets. |
| C-043 | Corriger premier rendu/mesure | FAIT ciblé | N’agrandit pas une TLW croppée. |

## 2. Refonte « une île par top-level »

Le plan `iridescent-snacking-oasis.md` a été approuvé avec la règle absolue
« aucun commit sans validation explicite » (`plan:44-50,126-131`).

| ID | Tâche du plan | État conversation | Audit actuel |
|---|---|---|---|
| R-000 | Spike topologie composition/input | FAIT | A invalidé bridge-bottom et retenu bridge-top. |
| R-001 | Host unique par TLW | FAIT worktree | `tlwhost.cpp`, host/registre/root Canvas. |
| R-002 | Slots par contrôle | FAIT worktree | Grid/position/clip/Z; lifetime des handlers incomplet. |
| R-003 | Synchronisation coalescée | FAIT worktree | Move/size/show/destroy/ancestor/migration; freeze busy-loop restant. |
| R-004 | Clip ancestors/scrollbars | FAIT worktree | SetWindowRgn et clip TabView; failure cleanup à durcir. |
| R-005 | ZIndex pré-ordre | FAIT partiel | Ne couvre pas Raise/Lower/interleaving réel. |
| R-006 | Routeur island-first vers HWND | FAIT worktree | Pivot définitif après échec HTTRANSPARENT; parité hit-test/pointer à finir. |
| R-007 | Synthèse pointer par slot | FAIT partiel | Boutons gauche/droite/milieu; tokens/lifetime et tactile restent. |
| R-008 | Molette native à travers slots | FAIT | Injection native implémentée; soak scroll restant. |
| R-009 | Arbitre de focus/TLW | FAIT partiel | Root non-Control comme RadioBox et transitions finales restent. |
| R-010 | `DoFindFocus` bridge→slot | FAIT | À stresser multi-TLW/reparentage. |
| R-011 | Tab mixte wx/XAML | PARTIEL | Code existe; checklist complète jamais exécutée. |
| R-012 | Broker `IDropTarget` | **OUVERT** | Explicitement reporté phase 3 puis jamais livré. |
| R-013 | Cutover `wxWinUIControlHost` vers slots | FAIT worktree | Ancien chemin supprimé pour les contrôles internes. |
| R-014 | Migrer `wxWinUIXamlHost` public | **OUVERT** | Le plan le disait orthogonal; il viole maintenant l’invariant. |
| R-015 | Clip spécifique Notebook | FAIT | À valider sur templates/orientations. |
| R-016 | Gestion transitoires par host | PARTIEL | Menu/dialog/tooltip encore dispersés, pas de manager unique. |
| R-017 | ContentDialog overlay couvrant GDI | VALIDÉ visuellement | Default/stacking/lifetime et contrat public divergent. |
| R-018 | Supprimer DialogIsland | FAIT | Remplacé par presenter/host partagé. |
| R-019 | Supprimer ancienne machinerie N îles | FAIT en grande partie | `wxWinUIXamlHost` demeure une exception. |
| R-020 | Mettre à jour docs/invariants | PARTIEL | Header tlwhost décrit encore l’ancien HTTRANSPARENT. |
| R-021 | Retirer logs/switchs diagnostiques | PARTIEL | Spike 165 fps/histogramme toujours actif. |
| R-022 | Gate latence scroll/canvases miroirs | OUVERT | Aucune mesure/décision formelle. |
| R-023 | Audit chrome/Z/showcase/splitter | PARTIEL | Trait splitter présumé résolu, pas checklist probante. |
| R-024 | DPI complet | PARTIEL | Géométrie reflush; resources/bitmap/caches composants restent. |
| R-025 | RTL | PARTIEL | Correction coordonnées physiques faite; matrice complète absente. |
| R-026 | UIA names | PARTIEL | Nom initial + Tree item; refresh et shell duplicates restent. |
| R-027 | Narrator/Accessibility Insights | OUVERT manuel | À réaliser avec l’utilisateur. |
| R-028 | Stress multi-TLW/reparentage croisé | OUVERT | Migration existe sans stress. |
| R-029 | Réévaluer priming backdrop | PARTIEL | Toujours nécessaire après owner change; lifecycle dangereux. |

## 3. Forensics dialogues, lag, curseur et freeze

| ID | Incident / expérience | Résultat conversation | Reste actuel |
|---|---|---|---|
| F-001 | Dialogue overlay n’assombrit pas tout sous N îles | Résolu structurellement par host TLW | Qualifier transitoires empilés et UIA. |
| F-002 | Dialogues/freezes et AV TextDialog | Plusieurs hypothèses rejetées | Une famille freeze résolue; suite TextCtrl montre encore instabilité distincte. |
| F-003 | XAML ralenti dans boucles natives | **RÉSOLU** par `WH_GETMESSAGE` | Vérifier échec d’installation et ordre accélérateurs. |
| F-004 | Dialogues Win32-class lent pendant resize | Frame-class introduite | Entrée/Échap/default/focus pas validés. |
| F-005 | Toute fenêtre owner ralentit move/size | Matrice A/B a isolé owner | Fix detach actif validé nominalement. |
| F-006 | Détacher owner de la fenêtre active | **VALIDÉ** pour le lag | Minimize, Alt-Tab, stacking, destruction ouverts. |
| F-007 | Curseur revient à flèche au repos | **RÉSOLU** pour slots statiques | Slots animés en continu et busy/custom ouverts. |
| F-008 | Freeze après drag rapide hors fenêtre | Signature MOVE fixe trouvée | Breaker implémenté et utilisateur a confirmé disparition. |
| F-009 | Rafales MOVE identiques | **RÉSOLU nominalement** | Soak et variantes ENTER/EXIT/oscillation ouverts. |
| F-010 | `Ctrl-T` menu ne déclenche pas | **BUG CONFIRMÉ** | Direct `WM_COMMAND` marchait; corriger ordre de traduction. |
| F-011 | Crash retardé TextEntryDialog après tooltip | Cause `m_validator` non initialisé trouvée sous cdb | NSDMI dans worktree, utilisateur a confirmé. |
| F-012 | Autres scalaires headers | Audit demandé | Durcissement préventif, pas bugs démontrés. |
| F-013 | Backdrop cassé après owner-detach | **RÉSOLU** par re-prime owner change | Rendre priming officiel et génération-safe. |
| F-014 | Choose Font croppé | Cause supposée : template réalisé après hints | Re-fit grow-only à implémenter/mesurer. |
| F-015 | Dialogues empilés/modeless/minimize | Non exercé | Gate P0 dialogues. |
| F-016 | Chemin log collect-freeze/spike | Incohérent | Unifier et rendre instrumentation opt-in. |

## 4. Bugs résolus versus bugs encore ouverts

### Résolus ou superseded avec preuve

- ownership du renderer statique ;
- premier rendu de peers XAML ;
- double frappe clavier historique ;
- overlay plein client sous host partagé ;
- starvation XAML dans boucles natives ;
- lag owner nominal via detach actif ;
- freeze de signature MOVE identique ;
- curseur statique au repos ;
- crash du constructeur non-default TextEntryDialog ;
- phantom Tree icons/labels/toolbar visible du dernier commit.

### Déclarés « faits » dans la conversation mais reclassés partiels

- Phase 2/3 car D&D a été reporté puis oublié ;
- Tab/focus car la matrice mixte n’a pas été exécutée et RadioBox n’a pas de
  cible de focus ;
- UIA car seuls noms initiaux/Tree items sont présents ;
- DPI parce que bitmaps/caches et plusieurs doubles conversions restent ;
- Menus parce que souris nominale ne couvre ni accélérateurs ni contrat OPEN ;
- composants annoncés « full API pass » mais en échec dans `test_gui` ;
- dialogues « resolved » tant que frame-class/owner/stacking ne sont pas gates.

## 5. Contradictions du plan à nettoyer

- `plan:28,35` décrit bridge-bottom/HTTRANSPARENT; `plan:54-81` consigne le
  pivot bridge-top/island-first.
- `plan:48,124` dit Window par défaut; `plan:81` et le code courant disent
  Overlay.
- `plan:78` reporte `IDropTarget` à la phase 3; la phase 3 est ensuite marquée
  faite sans ce livrable.
- `plan:106` affirme D&D préservé/délégué alors qu’il n’existe pas.
- `plan:90,128-130` exige une passe widgets/checklist qui n’a pas été exécutée
  après les changements finaux.
