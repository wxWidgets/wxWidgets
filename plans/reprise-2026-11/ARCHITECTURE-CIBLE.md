# wxWinUI — architecture cible (proposition du 21 novembre 2026)

Cette proposition respecte les exigences posées le 21 novembre et s'atteint
par transformation de l'existant, étape par étape, sans réécriture. Les
constats qui la motivent sont dans `AUDIT-TECHNIQUE.md`.

## 1. Les exigences

1. **Compatibilité wxMSW.** Chaque contrôle garde son propre HWND.
   `GetHandle()` le renvoie et les appels Win32 dessus fonctionnent, par
   exemple `SendMessage(text->GetHandle(), EM_SETCUEBANNER, ...)`. Changer de
   toolkit se résume à un drapeau de build ; `__WXMSW__` reste défini.
2. **Tout en vrai WinUI 3.** Aucun repli sur un contrôle Win32 ou sur un
   contrôle générique repeint. Des bibliothèques WinUI externes sont admises.
3. **Pas de réécriture.** On audite, on corrige, on simplifie et on termine.
4. **Qualité d'usage.** Fluide au redimensionnement et à la souris, sans
   clignotement, correct en HDR, Mica conservé.
5. **Amont.** Une série de petites PR relisibles, un job CI.

## 2. Pourquoi la structure actuelle ne tient pas

Elle a bien deux arbres (les HWND de wxMSW et les éléments XAML), mais leurs
rôles se chevauchent :

- les **pixels** viennent des deux : XAML pour les contrôles portés, GDI vu
  par transparence pour le reste. D'où la clé de transparence noire, les
  défauts HDR, le mode clair illisible, le clignotement, l'ancienne scrollbar ;
- l'**entrée** arrive à XAML, puis est redistribuée à la main vers les HWND
  par une imitation de USER32. D'où le lag, la récursion des dialogues, la
  poignée ;
- la **géométrie** fait autorité côté HWND et rejoint XAML en différé, par un
  recalcul global. D'où le redimensionnement saccadé ;
- les HWND des contrôles portés **ne répondent à aucun message de contrôle**,
  donc l'exigence 1 n'est pas tenue alors qu'on en paie le prix.

La structure est donc la bonne dans son principe (un HWND par contrôle, une
île par fenêtre), mais pas dans la répartition des responsabilités.

## 3. Le principe : deux arbres, deux rôles, une structure commune

| | Arbre HWND (wxMSW) | Arbre XAML (île) |
|---|---|---|
| Rôle | Identité et **contrat Win32** | **Tous les pixels, toute l'entrée brute** |
| Répond à | `GetHandle`, `SendMessage`, `GetWindowRect`, styles, identifiants, parenté, notifications `WM_COMMAND` / `WM_NOTIFY` | Rendu, composition, hit-test, animations, accessibilité |
| Ne fait jamais | Peindre à l'écran, recevoir la souris du système | Décider de la géométrie ou de la parenté |

Les deux arbres ont **la même forme** : un nœud XAML par `wxWindow`, imbriqué
comme son HWND. C'est ce qui rend la synchronisation triviale : une
propriété d'un HWND correspond à une propriété d'un seul nœud.

Sens des flux, sans exception :

- **HWND → XAML**, synchrone, fenêtre par fenêtre : position, taille,
  visibilité, activation, ordre entre frères ;
- **XAML → HWND** : l'entrée (livrée comme messages Win32 au HWND du nœud
  touché) et les notifications des contrôles.

## 4. Les mécanismes

### 4.1 Arbre XAML imbriqué, géométrie synchrone

Chaque `wxWindow` d'une fenêtre WinUI reçoit un conteneur (`Canvas`), placé
dans le conteneur de son parent en coordonnées relatives au parent. Son
contenu est soit le contrôle XAML (les « slots » actuels), soit une surface
de peinture (4.2), soit rien (simple panneau).

- `WM_WINDOWPOSCHANGED` d'un HWND pose quatre propriétés sur son nœud, tout de
  suite, et seulement si elles ont changé. Les descendants suivent seuls.
- Découpage et visibilité sont hérités du conteneur : plus de calcul par
  ancêtre.
- L'ordre d'empilement se règle entre frères, quand il change.

Disparaissent : le recalcul global `FlushSync` / `MarkAllDirty`,
`RecomputeZOrder` et son analyse quadratique, le suivi des ancêtres, la
reconstruction de la région de l'île à chaque flush. Le précédent existe
déjà dans le code pour les composites (`tlwhost.cpp:5065`).

### 4.2 Surfaces de peinture : plus aucun pixel GDI présenté directement

Toute fenêtre qui se peint elle-même (`EVT_PAINT`, `wxClientDC`) dessine dans
une surface retenue, opaque là où elle a peint, affichée par son nœud XAML.
Le HWND n'est plus peint. Le crochet existe en germe : le `wxPaintDC` des
fenêtres `wxBG_STYLE_PAINT` passe déjà par un tampon
(`src/msw/dcclient.cpp:96-281`) ; il s'agit de présenter ce tampon par XAML
au lieu de le recopier sur le HWND.

- Un panneau sans peinture n'a pas de surface : Mica se voit à travers,
  nativement, sans clé noire.
- Tout est composé dans la même image que les contrôles XAML : plus de
  clignotement, HDR correct, mode clair correct.
- Un élément peint peut passer devant ou derrière un contrôle XAML : la
  limite « tout le XAML au-dessus de tout le natif » tombe.

Cette surface reste nécessaire même quand tous les composants wx seront en
WinUI : les applications ont leurs propres fenêtres peintes (le canevas d'un
éditeur, un contrôle maison). Elle sert aussi de filet pendant la migration.

Restent de vrais HWND visibles, posés au-dessus de l'île : `wxGLCanvas`,
`wxNativeWindow`, ActiveX. Rien de XAML ne peut se dessiner sur leur
rectangle, hormis menus et infobulles qui sont des fenêtres séparées.

Les scrollbars gérées par wx deviennent des `ScrollBar` XAML à part entière,
posées dans le nœud, et non plus des barres USER32 masquées.

### 4.3 Entrée : XAML seule source, livrée au bon HWND

Puisque chaque fenêtre a son nœud, le hit-test de XAML désigne directement la
`wxWindow` touchée. On lui envoie le message Win32 correspondant
(`WM_MOUSEMOVE`, `WM_LBUTTONDOWN`…) sur son HWND, et wxMSW fabrique les
événements wx comme d'habitude, y compris `wxEVT_CONTEXT_MENU`, aujourd'hui
absent sur les contrôles portés.

- Distribution **non récursive** : un événement qui arrive pendant une
  livraison est mis en attente, et la boucle externe le traite. Le
  débordement de pile devient impossible par construction.
- Les mouvements sont **regroupés** : un par tour de boucle.
- `CaptureMouse` s'appuie sur la capture XAML du nœud.
- Pendant une boucle de déplacement ou de redimensionnement de Windows, les
  messages souris ne passent plus par XAML : la poignée fonctionne
  normalement.

Disparaissent : le résolveur natif (`inputrouter.cpp`), le double
`WM_NCHITTEST`, le miroir de curseur, l'essentiel des 6 000 lignes de gardes.

### 4.4 Le contrat Win32 des HWND de contrôle

C'est l'exigence 1, absente aujourd'hui. Chaque famille de contrôle reçoit un
**adaptateur de messages** : une table qui traduit les messages Win32 du
contrôle d'origine vers l'API wx déjà implémentée sur le contrôle XAML.

- `EM_SETCUEBANNER` → `SetHint`, `EM_SETSEL` → `SetSelection`,
  `BM_GETCHECK` → `GetValue`, `CB_ADDSTRING` → `Append`, et ainsi de suite ;
- les messages communs pour tous : `WM_SETTEXT`, `WM_GETTEXT`,
  `WM_GETTEXTLENGTH`, `WM_SETFONT`, `WM_GETFONT`, `WM_ENABLE`,
  `WM_GETDLGCODE` ;
- les styles Win32 (`ES_PASSWORD`, `BS_DEFPUSHBUTTON`…) reflétés dans
  `GWL_STYLE` ;
- en sortie, les notifications (`EN_CHANGE`, `BN_CLICKED`, `LVN_*`…) envoyées
  au parent en `WM_COMMAND` / `WM_NOTIFY`, pour le code qui les intercepte.

**La preuve** : une suite de tests écrite uniquement en messages Win32, que
l'on fait tourner sur le build wxMSW (qui fait foi par définition) puis sur
le build wxWinUI. L'écart entre les deux est la mesure objective de la
compatibilité, contrôle par contrôle.

Limites physiques, à documenter plutôt qu'à maquiller : `GetClassName` ne
renverra pas `Edit` ; `::GetFocus()` et `WindowFromPoint()` appelés
directement désignent l'île, pas le contrôle.

### 4.5 Composants

- Chaque classe wx s'appuie sur un vrai contrôle WinUI.
- Les superpositions (liste, grille, en-tête, PropertyGrid) sont remplacées
  par des contrôles XAML virtualisés qui possèdent leur mise en page, leur
  défilement et leur sélection ; wx ne fournit que le modèle.
- Un **moteur de table commun** (lignes virtualisées, en-tête à colonnes
  redimensionnables et réordonnables, tri, arborescence, éditeurs en place)
  sert `wxDataViewCtrl`, `wxListCtrl`, `wxTreeListCtrl` et la base de
  `wxPropertyGrid`, au lieu de quatre implémentations.
- Une petite bibliothèque interne (durée de vie du pair XAML, abonnements
  révoqués automatiquement, pose de propriété avec comparaison) remplace les
  machines d'état recopiées dans chaque fichier.
- Les modifications inconditionnelles du code partagé (`auibook.cpp`,
  `propgrid.cpp`, `datavgen.cpp`) sont ramenées à de petits points d'accroche.
- **`wxToolBar` est à refaire** (demande du 22 novembre) : une
  implémentation propre, pas une retouche de l'actuelle. Même méthode que
  pour le ruban : lire ce que fait le contrôle WinUI prévu pour ce rôle
  (`CommandBar` et ses `AppBarButton` / `AppBarToggleButton` /
  `AppBarSeparator` / `AppBarElementContainer`, débordement natif vers le
  menu « … »), garder de wx le modèle des outils, les événements et le
  HWND, et faire porter à XAML la mise en page, le débordement et les
  états. Sont concernés avec elle : les barres d'outils du ruban
  (`wxRibbonToolBar`) et `wxAuiToolBar`. Défauts connus de l'actuelle :
  taille des images d'outils fausse à 150 % (`winuitoolbar.cpp:697`).

### 4.6 Focus, clavier, dialogues

- **Une seule autorité de focus : XAML.** `GotFocus` / `LostFocus` d'un nœud
  désignent la `wxWindow` active et produisent `wxEVT_SET_FOCUS` /
  `wxEVT_KILL_FOCUS`. `SetFocus()` se réduit à donner le focus à l'île puis à
  l'élément. La machinerie de tickets, de générations et de migration du
  focus disparaît.
- **Le clavier atteint enfin les contrôles.** Les événements `KeyDown`,
  `KeyUp` et `CharacterReceived` du nœud actif sont livrés à sa coquille en
  `WM_KEYDOWN`, `WM_KEYUP`, `WM_CHAR`. wxMSW en tire `wxEVT_KEY_DOWN` et
  `wxEVT_CHAR`, absents aujourd'hui sur tous les contrôles portés, et une
  surcharge de `MSWWindowProc` les voit.
- **Aucun événement wx distribué depuis l'intérieur d'un délégué XAML.** Les
  événements d'origine XAML passent par une file vidée par la boucle de
  messages. Un `ShowModal` appelé dans un gestionnaire de clic n'imbrique
  plus de boucle dans XAML : c'est la cause la plus probable des gels et
  crashs intermittents des dialogues. La file de destructions différées
  globale devient inutile.
- **`wxMessageDialog` redevient une seule fenêtre**, le dialogue natif de
  Windows (`TaskDialog`, déjà câblé dans `src/winui/msgdlg.cpp:39-144`) ou un
  dialogue XAML dans son propre HWND public. Le présentateur à deux fenêtres
  et le registre des transitoires sont supprimés.
- **Plus de drapeau global qui condamne tous les contrôles** après un échec
  isolé ; plus de `wxLogWarning` ouvrant une boîte modale depuis un chemin
  interne. Le retrait des popups est réduit à : fermer, révoquer, libérer en
  différé.
- Le hook de messages reçoit une garde de réentrance et cesse d'intercepter
  les messages souris des boucles natives de déplacement et de
  redimensionnement.

## 5. Le chemin, sans réécriture

Chaque étape laisse le port utilisable et se juge sur `showcase.exe`.

| Étape | Contenu | Ce que tu constates |
|---|---|---|
| 0 | Corrections à structure constante : comparer avant de poser, ne plus tout marquer sale, sortir région, scrollbars et empilement du flush ; distribution souris non récursive et regroupée ; poignée ; fin de l'amorçage d'un pixel | Redimensionnement et souris nettement plus fluides, plus de crash des dialogues, poignée fonctionnelle |
| 1 | Arbre XAML imbriqué et géométrie synchrone (4.1) | Contenu collé à la fenêtre pendant le redimensionnement et le défilement |
| 2 | Surfaces de peinture et scrollbars XAML (4.2) | Plus de clignotement ni d'ancienne scrollbar, HDR et mode clair corrects |
| 3 | Entrée livrée au nœud (4.3), suppression du résolveur natif | Curseurs, survol, capture et clic droit justes partout |
| 4 | Contrat Win32 et suite de tests double (4.4) | `SendMessage` fonctionne sur les contrôles |
| 5 | Composants : moteur de table, puis Ribbon, barre d'outils (`wxToolBar`, à refaire proprement sur `CommandBar` : voir 4.5), AUI, FileCtrl et le reste, avec amaigrissement de chaque fichier au passage | Chaque page de Showcase en vrai WinUI |
| 6 | Fusion de `master`, découpage en PR | PR relisibles pour l'amont |

L'étape 4 peut avancer en parallèle des étapes 1 à 3, contrôle par contrôle.

## 6. Avancement

### Étape 0, premier lot (21 novembre) : écrit, compilé, testé, pas encore vu à l'écran

| Correction | Fichiers | Vérifié par |
|---|---|---|
| Écho du routage souris reconnu par l'absence de lecture de la file de messages, et non plus par l'égalité exacte des coordonnées ; profondeur de réentrance bornée à 16 | `src/winui/hostinput.cpp`, `src/winui/winui.cpp` | `[winui-root-input]`, 9 cas dont un nouveau (écho décalé d'un pixel) |
| Poignée de la barre d'état : suivi direct du pointeur capturé, avec `WM_ENTERSIZEMOVE`, `WM_SIZING`, `WM_EXITSIZEMOVE` ; plus de passage par la boucle modale de Windows | `src/winui/statbar.cpp` | Compilation et démarrage seulement ; le glisser réel reste à constater |
| Géométrie XAML publiée dès la fin du `WM_SIZE` de la fenêtre, sans attendre le tour de boucle suivant | `src/msw/window.cpp`, `src/winui/hostgeometry.cpp` | `[HostState]` 104 cas, `[HostLifecycle]` 119 cas |
| Amorçage d'un pixel du fond Mica seulement après un changement d'écran, plus à chaque fin de déplacement | `src/msw/window.cpp` | Lecture du code ; effet visuel à constater |
| 111 `wxLogWarning` / `wxLogError` internes redirigés vers le journal de débogage : plus de boîte modale ouverte depuis un chemin interne | 22 fichiers de `src/winui` et `src/ribbon` | `[winui-chrome-feedback]` 35 cas, `[winui-v0-supported]` 94 cas, `[winui-appearance]` 34 cas |

L'ancien passage de relais de la poignée (`src/winui/hostresize.cpp`, environ
800 lignes, et ses tests `[winui-native-resize]`) n'est plus utilisé par la
poignée. À supprimer une fois le nouveau comportement confirmé.

### Étape 0, deuxième lot (21 novembre) : écrit, compilé, testé ; le rendu des scrollbars est vu en capture, les gestes à la souris ne le sont pas

| Correction | Fichiers | Vérifié par |
|---|---|---|
| Curseur qui disparaît après la sortie lente d'un dialogue : l'hôte ne pose plus de curseur nul sur l'île (ce qui le masque), il pose la flèche standard | `src/winui/tlwhost.cpp` | `[winui-cursor]` 6 cas. L'hypothèse (curseur nul = masqué) n'est pas documentée par Microsoft ; à confirmer à la souris |
| Scrollbars des fenêtres défilantes : plus aucune bande non cliente réservée aux barres Windows (`WM_NCCALCSIZE`), la barre XAML est posée en surimpression au bord de la zone cliente ; suppression du nettoyage des anciennes bandes | `src/msw/window.cpp`, `src/winui/scrollbarprojection.cpp`, `src/winui/hostgeometry.cpp` | Captures de Showcase (au repos, après défilement, après redimensionnement) ; `[winui-window-scrollbar]` 9 cas |
| Geste tenu par un contrôle XAML (curseur de scrollbar, de slider, poignée) : le routeur de l'hôte n'achemine plus ses mouvements et n'agit plus quand le pointeur sort de l'île | `src/winui/hostinput.cpp`, `include/wx/winui/private/tlwhost.h` | `[winui-root-input]` 9 cas. Le glisser réel hors de la fenêtre reste à constater |

Six tests de l'ancien mécanisme (alignement sur la bande Windows et propreté
de ses pixels) ont été retirés de `tests/controls/winuiwindowscrollbar.cpp` ;
le test de géométrie a été réécrit pour le nouveau contrat.

Limite connue : une fenêtre défilante qui a une vraie bordure Win32
(`WS_BORDER`, `WS_EX_CLIENTEDGE`) peut laisser paraître un ou deux pixels de
barre classique sur sa bordure. Les fenêtres sans bordure ou à bordure
thématique, cas général, ne sont pas concernées.

### Troisième lot (21 novembre) : revue visuelle de Showcase, de ses dialogues et de Widgets

Méthode : `outils/revue.ps1` photographie les dix pages et onze dialogues de
Showcase sur le bureau invisible ; les pages de Widgets sont ouvertes par
leur commande de menu (`command main 124+n` dans `drive.ps1`). Chaque
correction ci-dessous a été constatée sur ces images, avant et après. Rien
de ce lot n'a encore été vu sur un écran réel.

| Défaut constaté | Cause | Correction |
|---|---|---|
| Boutons OK / Annuler des dialogues génériques (choix, nombre, réordonner, assistant, rechercher) minuscules, de largeurs différentes, mal placés | Le style WinUI de `Button`, `ToggleButton`, `ComboBox`, `CheckBox`… est aligné à gauche et centré : le contrôle gardait sa taille naturelle dans un coin de sa fenêtre wx | `src/winui/ctrlhost.cpp` : le contenu d'un slot remplit toujours sa fenêtre wx (sauf alignement posé explicitement par le portage, et sauf lien hypertexte) |
| Texte plus petit dans les dialogues génériques et dans les contrôles encore dessinés en GDI (12 px) que dans les contrôles XAML (14 px) ; largeurs calculées trop justes | La police par défaut de wx restait la police de message de Windows (Segoe UI 9 pt) alors que XAML dessine en Segoe UI Variable 14 px | `src/msw/settings.cpp` : la police par défaut de wx devient celle de WinUI. `src/winui/appearance.cpp` : cette police laisse XAML choisir sa famille |
| Rectangle de focus clavier autour du premier contrôle à l'ouverture de chaque fenêtre | XAML affiche l'indice de focus pour un focus posé par programme tant qu'aucune entrée n'a eu lieu | `src/winui/hostfocus.cpp` (+ liste, arbre, vue de données) : focus posé « comme au pointeur » jusqu'à la première touche, comme Windows masque ses indices de focus. Les champs de saisie (dont le ComboBox éditable) gardent l'état programmatique : posé « au pointeur », un ComboBox éditable ne transmet pas le focus à son éditeur, ce qui cassait l'auto-complétion (régression vue par les tests, corrigée) |
| Dialogue sans position explicite ouvert en (0,0) de l'écran | Les dialogues sont de simples fenêtres (plus de modèle de dialogue Win32) : `CW_USEDEFAULT` sur une fenêtre popup donne l'origine de l'écran | `src/msw/toplevel.cpp` : même position qu'un modèle de dialogue wxMSW (décalage fixe depuis la zone cliente du parent, ramené dans l'écran) |
| Arbre : première lettre de l'élément sélectionné sous l'indicateur ; chevrons absents des nœuds dépliables jusqu'au premier clic | Le portage effaçait les réglages de chevron que WinUI venait d'écrire (leur valeur par défaut est « masqué ») | `src/winui/treectrl.cpp` : écriture explicite de l'état du chevron ; marge du libellé déduite de la largeur réelle de la colonne |
| Molette sans effet au-dessus de la barre de défilement XAML d'une fenêtre | `ScrollBar` ignore la molette et le routeur laisse à XAML ce qui lui appartient | `src/winui/scrollbarprojection.cpp` : la molette est remise à la fenêtre défilante (horizontale sur la barre horizontale) |
| `wxScrollBar` autonome invisible au repos | Une `ScrollBar` nue n'affiche rien sans mode d'indicateur | `src/winui/scrolbar.cpp` : mode souris permanent |
| Outils radio d'une barre d'outils (dont `wxToolbook`) dessinés avec un rond de bouton radio | Le pair est un `RadioButton` au gabarit standard | `src/winui/toolbar.cpp` : gabarit d'outil (survol, pastille d'accent sous l'outil coché) |
| Liste générique (`wxListCtrl`, `wxEditableListBox`) : pastille de sélection sur la première lettre | wx place le texte là où `ListViewItem` dessine sa pastille | `src/winui/listprojection.cpp` : pastille désactivée, le fond de sélection suffit |
| `wxChoice` : texte tronqué (« Norma… ») | Marge de chrome sous-estimée de 6 DIP | `src/winui/choice.cpp` |
| `wxSpinCtrl` : largeur fixe de 110 DIP quelle que soit la plage | — | `src/winui/spinctrl.cpp` : chiffres de la plage + chrome du `NumberBox` |
| `wxTreeListCtrl` / `wxDataViewCtrl` : colonne d'arbre en largeur automatique trop étroite | Indentation et bouton de dépliage non comptés | `src/winui/dataview.cpp` |
| `wxCalendarCtrl` : barres noires sous les week-ends en thème sombre | Couleur de marque prise sur le texte du jour courant (noir en sombre) | `src/winui/calctrl.cpp` : couleur d'accent |
| Molette sur un contrôle qui s'en sert (calendrier, liste) : la page défile aussi | Le slot renvoyait toujours `WM_MOUSEWHEEL` à la fenêtre wx, et `DefWindowProc` le remonte au parent | `src/winui/hostinput.cpp` : molette déjà consommée par XAML → événement wx seul, sans remontée au parent |
| Libellés de cases à cocher et de boutons radio rognés (Widgets), surtout juste après l'ouverture d'une page | Taille demandée à XAML avant que le gabarit du contrôle soit appliqué : mesure fausse, corrigée après coup par une seconde mise en page | `src/winui/checkbox.cpp`, `radiobut.cpp`, `radiobox.cpp` : taille calculée dès la création (libellé mesuré + métriques du gabarit), comme pour `wxButton` |
| Boîtes radio très larges, boutons radio et cases espacés de 120 px | Largeur minimale de 120 DIP du style WinUI | Largeur minimale annulée (voir ligne suivante) |
| Champ texte étroit coupé à droite (« Control height » dans Widgets) | Styles WinUI à dimensions minimales (64 × 32 DIP pour un champ texte) supérieures à la fenêtre wx ; le gabarit de `RichEditBox` les lit même en dur | `src/winui/ctrlhost.cpp` : un pair n'a jamais de minimum XAML supérieur à sa fenêtre wx ; ressource locale pour `RichEditBox` |
| Arbre de navigation de Widgets trop étroit (« OwnerDrawnComb… ») | `wxTreeCtrl::DoGetBestSize()` renvoyait une taille fixe | `src/winui/treectrl.cpp` : taille des éléments dépliés, comme wxMSW |
| `wxFileCtrl` : noms de dossiers en bleu pur, illisibles en thème sombre | Couleurs en dur du contrôle générique | `src/generic/filectrlg.cpp` : variantes claires en thème sombre |

Dans l'exemple Showcase : options `--page=N` et `--action=LIBELLÉ` (pour la
revue), arbre de navigation synchronisé avec la page, `StyleClearAll()` sur
le `wxStyledTextCtrl` (sans quoi Scintilla garde du texte noir sur fond blanc
dans un contrôle sombre, comme sous wxMSW en mode sombre).

Défauts relevés, non traités dans ce lot :

- `wxCalendarCtrl` : la ligne des jours de la semaine est vide sur les
  captures. À regarder sur un écran réel avant d'y toucher.
- `wxAuiNotebook` : blocs sombres parasites dans la bande d'onglets ;
  `wxRibbonBar`, `wxComboCtrl`, `wxOwnerDrawnComboBox`, `wxRichTextCtrl`,
  bordure blanche des listes génériques : rendu GDI classique, traité à
  l'étape 5 (composants).
- Dialogue de police générique : l'aperçu écrit dans la couleur choisie
  (noir par défaut) sur le fond du thème.
- `wxNumberEntryDialog` : champ numérique à l'étroit (largeur imposée par
  le dialogue générique).
- Pages défilantes (Widgets) : rien n'indique au repos qu'une page défile,
  la barre XAML n'apparaissant qu'au passage du pointeur. C'est le
  comportement de Windows 11, mais une page coupée sans barre visible se lit
  comme un défaut. À décider : indicateur fin toujours visible.
- `wxTextCtrl` simple ligne : le pair est un `RichEditBox` ; à vérifier
  (un `TextBox` suffirait et se comporte mieux aux petites tailles).

### Quatrième lot (21 novembre) : gels de dialogues

Écrit et compilé. Aucun de ces trois points ne peut être provoqué depuis le
bureau invisible (il faut de vrais clics) : à constater à l'écran.

| Vecteur | Cause | Correction |
|---|---|---|
| Dialogue modal ouvert depuis un gestionnaire de clic | L'événement wx était envoyé depuis l'intérieur du rappel XAML `Click` / `DoubleTapped` : la boucle modale s'imbriquait dans un délégué XAML et sous le routage de pointeur de l'hôte | `wxWinUIPostNotification()` (`src/winui/ctrlhost.cpp`) : la notification est mise en file et part de la boucle d'événements, avant le message suivant. Appliqué à `wxButton`, aux outils de `wxToolBar` et à leur flèche, à `wxHyperlinkCtrl`, aux boutons de `wxInfoBar`, au double-clic de `wxListBox`, `wxTreeCtrl` et `wxDataViewCtrl`. Les menus différaient déjà leur commande |
| Dialogue qu'aucun bouton ne ferme plus | Le présentateur refusait la fermeture dès qu'un ancêtre (le cadre propriétaire) était en cours de fermeture, puis gardait son verrou « fermeture demandée » posé pour toujours | `src/winui/dlgpresenter.cpp` : seul l'état du dialogue lui-même compte ; le verrou est relâché si la fermeture n'a pas eu lieu |
| Plus aucun contrôle créé ni aucune liste déroulante ouverte jusqu'à la fin du programme | Un drapeau global au processus, posé au premier popup dont le retrait n'avait pas pu être prouvé — fermer une fenêtre avec une liste déroulante ouverte suffisait | `src/winui/ctrlhost.cpp` : la dégradation reste locale au contrôle concerné ; le drapeau n'est plus posé que par les tests |

`wxWinUIFlushNotifications()` vide la file sur demande : les points d'accès
de test qui déclenchent un clic l'appellent avant de rendre la main.

Non couvert : les clics sur les contrôles encore dessinés par wx (grille,
listes génériques, AUI, canevas) arrivent par le routeur de pointeur, qui
envoie le message de façon synchrone depuis le rappel XAML. Un dialogue
ouvert depuis leur gestionnaire reste donc imbriqué. C'est l'objet de
l'étape 3 (messages souris postés, un seul mouvement par tour de boucle).

### Cinquième lot (21 novembre) : texte GDI en HDR, fenêtres jamais retirées

| Défaut | Cause | Correction |
|---|---|---|
| Sur un écran HDR, le libellé des volets repliables (dialogue « À propos ») s'affiche puis disparaît ; il est présent sur une capture et sur un écran SDR | GDI écrit ses pixels avec un alpha nul. Au-dessus de Mica, DWM les additionne au fond en SDR mais ne les montre pas en HDR. Seules les fenêtres `wxBG_STYLE_PAINT` passaient par un tampon rendu opaque | `src/msw/dcclient.cpp`, `src/winui/ctrlhost.cpp` : les autres fenêtres peintes en GDI au-dessus de Mica passent aussi par un tampon ; l'alpha y part de 255 et GDI le remet à zéro sur chaque pixel qu'il écrit, ce qui permet de rendre opaques exactement les pixels dessinés (noir compris) en laissant transparent le fond non touché |
| Deux fenêtres restaient enregistrées « en cours de destruction » pour toujours après certains dialogues (échec hérité `winuidialogsession.cpp:512`) | `Destroy()` appelé sur un dialogue déjà en cours de destruction (son propriétaire est détruit) créait une pierre tombale dans le registre des propriétaires alors que l'événement de destruction était déjà parti : plus rien ne pouvait la retirer | `src/winui/dlgpresenter.cpp` : pas de pierre tombale pour une fenêtre déjà en cours de destruction ; `Register()` la refuse explicitement |

Le premier point a été confirmé sur écran HDR. Il ne couvre pas ce qui est
dessiné hors `WM_PAINT` (`wxClientDC`) ni les contrôles Win32 natifs : ces
pixels gardent un alpha nul jusqu'aux surfaces de peinture de l'étape 2.

Conséquence, et règle retenue : un texte noir dessiné en GDI, jusque-là
invisible sur Mica sombre, est devenu visible (exemple Dialogs : texte noir
prévu pour un fond blanc que le portage ne peint pas).

- **Le fond reste Mica.** Une couleur de fond posée par le programme
  (`SetBackgroundColour`) n'est pas peinte : on ne force pas de fond opaque.
- **C'est la couleur du texte qui s'adapte.** `wxWinUIAdaptTextColourOverBackdrop()`
  (`src/msw/dcclient.cpp`, appelé par `wxMSWDCImpl::DrawAnyText`) regarde ce
  qu'il y a sous le texte au moment de le dessiner — le fond opaque du texte,
  ce que le gestionnaire a déjà peint, ou Mica si rien n'a été peint. Si le
  contraste est insuffisant (moins de 3:1), la couleur garde sa teinte et sa
  luminosité est inversée : noir devient blanc, bleu foncé devient bleu clair.
  Un texte dessiné sur un fond que le programme a peint lui-même n'est pas
  touché.

### Calendrier (21 novembre)

| Défaut | Cause | Correction |
|---|---|---|
| Ligne des jours de la semaine vide | `CalendarView` ne publie les noms que lorsqu'il mesure un changement de sa source de dates **et** que son panneau de mois existe déjà ; ici la première mesure a lieu avant | `src/winui/calctrl.cpp` : si les noms sont vides, le premier jour de la semaine est changé puis rétabli, ce qui force leur publication (trois essais au plus) |
| Semaine affichée à partir du lundi mais propriété `FirstDayOfWeek` lue « dimanche » | La propriété ne rapporte pas le premier jour régional que le contrôle utilise tant qu'elle n'est pas posée | Le premier jour est posé explicitement : celui du style wx, sinon celui de la région de l'utilisateur |
| « ma » et « me » coupés sur deux lignes | Le gabarit entoure chaque nom de 12 DIP de chaque côté | Ressource `CalendarViewWeekDayPadding` réduite sur le calendrier |
| Barres sous tous les samedis et dimanches | Avec le style par défaut chaque jour de week-end est un jour férié, et les fériés étaient marqués comme les jours marqués par le programme | Un jour marqué par `Mark()` garde sa barre, un férié aussi sauf s'il tombe un samedi ou un dimanche. Limite : un vrai férié tombant un week-end n'est pas marqué |
| Double-clic sur un nom de jour : événement « double-clic sur une date » fabriqué | Les semaines défilées hors du mois affiché restent réalisées, sous l'en-tête ; leurs cases comptaient comme touchées. Défaut ancien, masqué tant que la ligne des noms était vide | Seule la partie visible d'une case, celle publiée pour `HitTest()`, compte |

### Étape 1 (22 novembre) : arbre XAML imbriqué, géométrie synchrone

Écrit, compilé, testé, vu en capture ; pas encore vu à l'écran.

**Ce qui a changé.** Avant, chaque contrôle était un enfant direct d'un
unique canevas racine, placé en coordonnées de la fenêtre, découpé à la main
contre chacun de ses ancêtres, et tout était recalculé pour tous les
contrôles à chaque fenêtre qui bougeait. Maintenant :

- Chaque fenêtre wx qui a des contrôles XAML quelque part dessous a un
  **nœud** : un canevas qui représente sa zone cliente, enfant du canevas de
  son propre parent (le canevas racine représente la fenêtre de premier
  niveau). Le conteneur d'un contrôle est un enfant du canevas de son parent.
  L'arbre XAML a donc la forme de l'arbre des HWND.
- Les positions sont **relatives au parent**. Déplacer ou masquer une
  fenêtre déplace ou masque tout ce qu'elle contient en écrivant une seule
  valeur ; la découpe et la visibilité sont héritées.
- La géométrie est écrite **au moment où Win32 la signale**
  (`WM_WINDOWPOSCHANGED`), pour la seule fenêtre concernée
  (`ApplyWindowGeometry`). La synchronisation complète d'un contrôle
  (`SyncSlot` : état actif, info-bulle, nom d'accessibilité, curseur) ne
  tourne plus que lorsque quelque chose d'autre que sa place a changé.
- L'ordre d'empilement n'est recalculé que lorsqu'il a pu changer (ordre
  des fenêtres sœurs, contrôle ajouté, fenêtre native déplacée), plus à
  chaque passe.
- Les rectangles « dans la fenêtre » utilisés par les tests de pointage
  (`m_rectPx`, `m_visibleRectPx`, `m_visible`) ne sont plus tenus à jour en
  continu : ils sont recalculés à la demande (`RefreshSlotRects`) quand une
  fenêtre a bougé depuis.

Code : `include/wx/winui/private/tlwhost.h` (`WindowNode`, `m_nodes`),
`src/winui/tlwhost.cpp` (montage : `AddAncestorRef`, `MountNode`,
`MountSlotContainer`, `MoveToCanvas`), `src/winui/hostgeometry.cpp`
(géométrie : `SyncNode`, `SyncSlotPlacement`, `ApplyWindowGeometry`,
`ComputeSlotRects`, `RecomputeZOrder`).

**Choix faits en route.**

| Question | Choix | Raison |
|---|---|---|
| Où mettre le nœud d'une fenêtre qui est elle-même un contrôle XAML (un carnet d'onglets et ses pages) ? | À côté de son conteneur, juste au-dessus, jamais dedans | Dedans, les enfants hériteraient du sens d'écriture du contrôle et ses gestionnaires recevraient leurs événements |
| Quand un canevas découpe-t-il ? | À partir du moment où un enfant dépasse de la zone cliente, pas avant | XAML dessine hors des limites d'un contrôle (l'anneau de focus est à 3 pixels dehors) ; un contrôle collé au bord de son parent le perdrait |
| Fenêtre détruite depuis un rappel XAML | Son canevas reste dans l'arbre tant qu'il contient quelque chose | Le retrait physique de ces contrôles est différé jusqu'au retour du rappel ; les sortir de l'arbre plus tôt est justement ce que ce report évite |
| Fenêtre gelée (`Freeze`) | Rien n'est écrit, ni recalculé, avant `Thaw` | C'était déjà la règle de la passe différée |
| Sens d'écriture, visibilité, échelle | Un changement renvoie le contrôle à la synchronisation complète | Ils touchent plus que la place |
| Contrôles montés dans le XAML de leur parent (Ribbon) | Inchangés : c'est la mise en page XAML du parent qui les place | Ils n'étaient déjà pas sur le canevas racine |

**Mesures** (`resizebench.ps1`, 300 redimensionnements, bureau invisible).

| | Avant | Après | wxMSW classique |
|---|---|---|---|
| Showcase (178 contrôles), synchronisation par étape | 4,1 à 4,7 ms | 1,5 ms | — |
| Showcase, resynchronisations globales pour 50 étapes | 17 000 à 30 000 | 0 | — |
| Showcase, étape complète (médiane) | 105 à 109 ms | 99 à 104 ms | 71 ms |
| Widgets (34 contrôles), synchronisation par étape | 1,2 ms | 0,6 ms | — |
| Widgets, étape complète (médiane) | 22 à 25 ms | 22 à 25 ms | 30 ms |

> Correction du 22 novembre : l'échantillonnage cité dans le paragraphe qui
> suit était faux (l'outil relisait la même pile à chaque échantillon), et
> sa conclusion avec lui. Voir « Audit du redimensionnement, suite ».

Le gain sur le redimensionnement lui-même est donc faible, et la mesure dit
pourquoi : un échantillonnage du fil d'interface pendant le test
(`profile.ps1`) trouve 30 échantillons sur 31 dans le noyau, à l'intérieur de
`MoveWindow`, appelé par la mise en page de wx. Le coût dominant d'un
redimensionnement est le déplacement des quelque 250 HWND par Windows, que
wxMSW classique paie aussi (71 ms). Déplacer les fenêtres de contrôle sans
copie de bits ni redessin (`SWP_NOCOPYBITS`, `SWP_NOREDRAW`) a été essayé :
99 ms contre 93 au mieux, donc abandonné. Ce que l'étape 1 apporte est
ailleurs : plus aucune passe sur tous les contrôles quand une fenêtre bouge
ou que le pointeur passe, un parent et ses enfants qui bougent ensemble, et
la structure dont les étapes 2 et 3 ont besoin.

**Tests.** Groupes de l'hôte et des contrôles relancés ; ceux qui
supposaient l'ancienne disposition ou l'ancien minutage ont été adaptés :

- Redimensionner un contrôle pour « le rendre sale » ne provoque plus de
  synchronisation complète. Les tests qui s'en servaient la demandent
  explicitement (`wxWinUIHostGeometryTestAccess::RequestSlotSync`).
- `winuicompositedirect.cpp` cherchait un conteneur parmi les enfants du
  canevas racine ; il vérifie maintenant qu'il est dans l'arbre de l'hôte.
- `winuitextpeer.cpp:1071` exigeait qu'une réparation de mise en page de
  `RichEditBox` s'enclenche. Le redimensionnement arrivant désormais avant
  le remplacement du texte, il n'y a plus rien à réparer ; le test exige
  seulement le résultat.
- `winuiolebroker.cpp:3772` et `winuiribbon.cpp:3463` (ce dernier une fois
  sur trois seulement, et uniquement après `WinUIAui::
  NativeCallbacksMayDestroyNotebook`) échouent aussi quand une sonde
  rétablit l'ancienne disposition et l'ancienne synchronisation : ils ne
  viennent pas de l'étape 1. La référence les passe, donc ils datent d'un lot
  antérieur, que ces deux groupes n'avaient pas couvert ; le plus probable
  est le premier, depuis lequel redimensionner la fenêtre synchronise tout
  de suite. Le courtier de glisser-déposer est alors prêt dès `Show()`, et
  le test pose désormais son point d'observation avant. Pour le Ribbon, la
  commande se charge après 118 ou 149 tours d'attente au lieu de 7, contre
  un budget de 120 tours qui passent plus vite qu'avant ; le test en accorde
  600. Ce délai fixe après la destruction d'un contrôle depuis un de ses
  rappels reste à expliquer.

**Limites connues.**

- Un canevas qui a commencé à découper ne cesse de le faire qu'à la
  prochaine passe complète (changement d'échelle, de thème, dégel).
- Changer de parent dans la même fenêtre sort le conteneur de l'arbre le
  temps de le remonter ; le focus clavier est rendu à l'élément qui l'avait.

### Suite du 22 novembre : séparateur, échelle 150 %, premier pas de l'étape 2

Écrit, compilé, testé, vu en capture ; pas encore vu à l'écran.

**Curseur de redimensionnement du séparateur.** Il s'affichait une image
puis redevenait une flèche, même pointeur immobile. XAML choisit lui-même
un curseur pour l'élément qu'il a sous le pointeur, et le canevas racine,
qui tient lieu de tout l'espace natif, n'en a pas : dès que XAML réévalue
(un mouvement, ou un curseur d'élément modifié par la passe de
synchronisation que le changement de curseur du séparateur déclenche), il
remet la flèche sur l'île. L'hôte ne s'en apercevait pas : deux raccourcis
de `MirrorNativeCursor`, hérités du travail d'octobre, supposaient que
l'île montrait encore ce qu'il y avait posé.

- `MirrorNativeCursor` relit d'abord le curseur réel de l'île
  (`ObserveXamlPointerCursor`) ; s'il a été remplacé, le curseur de la
  fenêtre native est reposé.
- Après un appui, un relâchement ou un cran de molette au-dessus d'une
  fenêtre native, et un tour de boucle après chaque passe de
  synchronisation, l'hôte regarde encore une fois
  (`ReassertIslandPointerCursor`).
- Deux flèches système sont maintenant tenues pour le même curseur, pour ne
  pas reposer la flèche à chaque mouvement au-dessus d'un panneau ordinaire.
- Test `WinUIRootInput::SplitterSashKeepsItsResizeCursor` : survol du
  séparateur par des mouvements simulés, XAML remplaçant le curseur entre
  deux mouvements.

Le journal `WX_WINUI_INPUT_LOG=<fichier>` écrit une ligne « cursor: the
island no longer shows ours » chaque fois que ce remplacement est constaté.

**Séparateur visible.** Il était peint de la couleur de transparence, donc
invisible. Il est maintenant dessiné par XAML dans le canevas de sa
fenêtre : un trait fin (`DividerStrokeColorDefaultBrush`), une poignée
centrale (`ControlStrongFillColorDefaultBrush`) et un fond au survol
(`SubtleFillColorSecondaryBrush`, fondu de 83 ms). Il ne prend aucune
entrée : le pointeur le traverse et c'est toujours `wxSplitterWindow` qui
fait glisser. Code : `src/winui/renderer.cpp` (`wxWinUIShowSplitterSash`),
`src/generic/splitter.cpp` (un appel quand il n'y a plus de séparateur).
Limite : sans `wxSP_LIVE_UPDATE`, le trait fantôme pendant le glisser reste
celui de wx, dessiné en GDI.

**Parties de fenêtre.** Mécanisme général ajouté à l'hôte :
`SetWindowPart(fenêtre, genre, élément)` pose un élément XAML dans le
canevas d'une fenêtre, qui garde alors son nœud même sans contrôle XAML
dessous. Deux genres : dessiné sous les enfants et sans entrée (le
séparateur), ou contrôle à part entière au-dessus des enfants (les
scrollbars). La partie part avec sa fenêtre.

**Étape 2, premier pas : les scrollbars XAML dans le nœud.** Les barres des
fenêtres défilantes ne sont plus des enfants du canevas racine placés en
coordonnées de la fenêtre de premier niveau : ce sont des parties de leur
fenêtre. Elles se déplacent, sont découpées et sont masquées avec elle, sans
attendre la passe différée. Leur position dans la fenêtre (après un
redimensionnement de celle-ci) est encore recalculée à la passe.

**Échelle d'affichage forcée.** `WX_WINUI_FORCE_SCALE=150` (ou `1.5`) fait
tourner le portage à cette échelle sur n'importe quel écran : wx annonce le
DPI correspondant pour toutes les fenêtres et l'île reçoit la même échelle
(`OverrideScale`). Les quatre groupes qui échouaient sur un vrai écran à
150 % y échouent à l'identique, assertion pour assertion : c'est donc le
moyen de travailler cette échelle sans toucher aux réglages de l'écran, pour
les tests comme pour les captures.

**Défauts à 150 %.**

| Échec ou défaut | Nature | Traitement |
|---|---|---|
| `winuitoolbar.cpp:697` et suivantes : image d'outil de 24 pixels au lieu de 36 | Défaut du portage, et test faux. Le code prenait `GetToolBitmapSize()` pour des DIP alors que ce sont des pixels, arrondis par la classe de base à une échelle entière (celle de la barre Win32), puis affichait l'image à la taille par défaut de son lot, quelle que soit la taille demandée | `src/winui/toolbar.cpp` : la taille d'une image d'outil est connue en DIP (celle de `SetToolBitmapSize()`, sinon celle du lot), ses pixels sont pris à l'échelle réelle et elle est affichée à cette taille. `GetToolBitmapSize()` reste celui de wxMSW. Test réécrit sur le vrai contrat (`SetToolBitmapSize()` reçoit des pixels) |
| `winuiribbon.cpp:3637`, `3753` : groupe replié | Test : fenêtre dimensionnée en pixels, seuils de repli en DIP | Tailles du test en DIP |
| `winuidataview.cpp:677` : pas de nouveau rendu | Test : 48 pixels sont la hauteur de ligne par défaut à 150 % | Hauteur du test en DIP |
| Dialogue Rechercher : libellés « Search for » et « Replace with » tronqués | Largeur fixe de 80 pixels dans le dialogue générique (wxMSW utilise le dialogue natif, d'où le défaut jamais vu en amont) | `src/generic/fdrepdlg.cpp` : le libellé prend la largeur de son texte |
| `winuidataview.cpp:1499` : violation d'accès | Défaut du portage, **non corrigé**. Le test détruit le contrôle depuis la lecture d'une largeur de colonne. À 150 % cette lecture a lieu pendant `ListView.UpdateLayout()` (appelé par `EnsureVisible`), et XAML continue sa passe sur des éléments libérés. Garder une opération d'hôte ouverte pendant `UpdateLayout()` et reporter le démontage interne du contrôle n'a pas suffi : essai retiré | À reprendre avec le moteur de table (étape 5) : aucune lecture du modèle ne devrait avoir lieu dans une passe de mise en page XAML |
| `winuilistctrl.cpp:1382` : pointage sur le texte d'une ligne, en droite-à-gauche | Défaut du portage, **non corrigé**. En modes liste et icônes, à 150 % et en droite-à-gauche, les cases XAML sont décalées de 487 pixels vers la gauche par rapport au modèle wx (leur bord droit est à 664 pixels dans une zone cliente de 1151) | À traiter avec l'audit droite-à-gauche |

**Contour des sections.** `wxStaticBox` utilisait le trait des cartes
(`CardStrokeColorDefaultBrush`), presque invisible sur fond sombre, et
`wxRadioBox` celui des contrôles (`ControlStrokeColorDefaultBrush`). Les
deux utilisent maintenant le second, plus lisible.

Le bord haut de `wxStaticBox` avait aussi un petit trait droit à gauche du
titre, posé par-dessus un coin que la bordure, sans bord haut, ne pouvait
pas arrondir. Le bord haut est maintenant fait de quatre pièces : coin
arrondi, titre, trait, coin arrondi ; le titre commence là où le premier
coin finit. Vu sur capture agrandie, `[winui-appearance]` passe à 100 % et
à 150 %.

**Droite-à-gauche.** `WX_WINUI_FORCE_RTL=1` dispose toutes les fenêtres de
droite à gauche, comme sous une langue arabe ou hébraïque, quelle que soit
la langue du programme : c'est l'outil de l'audit demandé le 22 novembre.

Premier passage, sur trois pages de Showcase (Boutons, Choix et listes,
Vues de données) à 100 % : la disposition est bien inversée partout (menu,
onglets, navigation, séparateur, cases à cocher, listes, en-têtes, barres de
progression, barre d'état) et les textes se lisent normalement. Attention
aux captures : `drive.ps1` rend l'image d'une fenêtre droite-à-gauche en
miroir, il faut la retourner avant de la lire. Défauts relevés, **non
corrigés** :

- Barre d'outils : le groupe d'outils est bien à droite, mais dans l'ordre
  gauche-à-droite (« New » devrait être le plus à droite).
- `wxTreeListCtrl` : le texte de la première colonne est rogné à son début
  (« winui » affiché « nui », « samples » affiché « mples »).
- `wxListCtrl` en modes liste et icônes à 150 % : le décalage de 487 pixels
  décrit plus haut.

Restent à passer en revue : les autres pages, les dialogues, Widgets, et le
tout à 150 %.

### Étape 2, surfaces de peinture : premier lot, sur option (22 novembre)

Écrit, compilé, vu en capture ; pas encore vu à l'écran. Ce premier lot
n'était actif qu'avec `WX_WINUI_PAINT_SURFACES=1` ; depuis le deuxième lot,
le même jour, les surfaces sont le chemin par défaut.

**Principe.** Une fenêtre qui se peint elle-même ne peint plus son HWND :
elle peint un bitmap que l'hôte garde pour elle, et qu'une `Image` XAML
montre dans le canevas de la fenêtre (une « partie » de plus, sous les
enfants). Ce qu'elle dessine est alors composé par XAML dans la même image
que les contrôles, avec un vrai canal alpha (le fond Mica se voit partout où
rien n'a été peint, quel que soit l'écran), et à sa place dans l'ordre des
fenêtres. Une fenêtre qui ne peint jamais n'a pas de surface.

Code : `src/winui/paintsurface.cpp` (nouveau), branché dans
`src/msw/dcclient.cpp` (`wxPaintDC`, `wxClientDC`) et `src/msw/window.cpp`
(effacement, défilement, déplacement des enfants, destruction).

| Point | Choix |
|---|---|
| Savoir ce que GDI a peint | Le bitmap garde dans l'octet alpha l'état de chaque pixel : 254 sans couleur = jamais peint (transparent), 0 = GDI vient d'écrire (rendu opaque à la fin de la peinture), autre valeur = peint. Aucun parcours du bitmap avant de peindre |
| `wxPaintDC` | Le DC rendu est celui du bitmap, limité au rectangle de `BeginPaint` ; si la fenêtre a été priée d'effacer son fond, ce rectangle repart transparent |
| `wxClientDC` | Redirigé vers le bitmap seulement si la fenêtre a déjà une surface (un `wxClientDC` sert surtout à mesurer du texte) ; ce qui a été dessiné est retrouvé par les bornes que GDI accumule |
| Enfants | Une fenêtre `WS_CLIPCHILDREN` ne peint rien sous ses enfants, comme sur un HWND, et ce qu'elle avait peint là où un enfant arrive est effacé : sinon d'anciens pixels se verraient à travers les enfants, qui sont transparents là où ils ne peignent rien |
| Vrais HWND (OpenGL, fenêtres étrangères) | Toujours exclus : ils se voient à travers l'île |
| Défilement | `ScrollWindow` fait aussi défiler le bitmap |
| « Couleur de transparence » | Le séparateur peignait du noir pour laisser voir le fond ; sur une surface il rend le rectangle transparent (`wxWinUIPaintSurfaceClearRect`) |

Vérifié en capture : démo `splitter` (deux canevas dessinés, avant et après
redimensionnement) et trois pages de Showcase, identiques au rendu par HWND.
Les groupes de l'hôte sont relancés avec la variable.

**Restaient, avant d'en faire le chemin par défaut** (traités le même jour,
voir « Étape 2, deuxième lot ») : le renvoi du bitmap entier à XAML, le
curseur texte de Win32, les dessins de `EVT_ERASE_BACKGROUND`, et les
dessins « en couleur de transparence ».

### Listes (22 novembre)

Écrit, compilé, vu en capture ; pas encore vu à l'écran.

- **Cases à cocher de `wxCheckListBox`.** La case occupait toute la largeur
  de la ligne : un clic n'importe où la cochait. Elle n'occupe plus que sa
  case et son libellé ; un clic ailleurs dans la ligne sélectionne
  l'élément. `src/winui/listbox.cpp`. Le clic réel reste à constater.
- **`wxEditableListBox`.** Boutons à glyphes de la police d'icônes de
  Windows (crayon, plus, corbeille, flèches) au lieu des bitmaps d'origine,
  dans la couleur du texte du thème ; plus de bordures creuses ; l'éditeur
  d'un libellé prend exactement la hauteur de sa ligne au lieu de la
  déborder, et le libellé s'efface pendant l'édition au lieu de se voir en
  transparence derrière lui. `src/generic/editlbox.cpp`,
  `src/generic/listctrl.cpp`, `src/winui/listprojection.cpp`. Ce dernier
  point vaut pour toute édition de libellé d'un `wxListCtrl`.

Suite du même jour, après retour d'écran : les boutons sont élargis (les
glyphes étaient rognés), la seule colonne prend toute la largeur de la liste
et l'éditeur prend exactement la place de la cellule, bords et hauteur
compris, comme les éditeurs d'une grille de propriétés ; la ligne
sélectionnée ne dépasse plus derrière lui.

### Curseur du séparateur, suite (22 novembre)

Retour d'écran après le premier correctif : pointeur immobile sur le
séparateur, le curseur clignotait (double flèche une seconde, flèche une
demi-seconde). XAML remettait donc bien sa flèche de lui-même, pointeur au
repos, et l'hôte ne la corrigeait qu'à sa vérification suivante. Plutôt que
corriger après coup, le canevas racine reçoit maintenant le même curseur
(`ProtectedCursor`) que celui posé sur l'île : quand XAML choisit de nouveau
le curseur de l'élément sous le pointeur, il choisit le bon.
`src/winui/tlwhost.cpp` (`SetIslandPointerCursor`,
`RelinquishIslandPointerCursor`). À confirmer à l'écran.

### Audit du redimensionnement (22 novembre)

Demande : trouver et corriger ce qui rend le redimensionnement lent, y
compris dans wxWidgets lui-même. Mesures avec `outils/resizebench.ps1`
(bureau invisible, médiane par étape), sondes temporaires retirées depuis.
Les valeurs absolues varient avec la charge de la machine ; les rapports
sont stables.

| Cause | Où | Correction | Effet mesuré |
|---|---|---|---|
| Un carnet d'onglets redimensionne **toutes** ses pages à chaque étape, cachées comprises : dans Showcase, 220 fenêtres sur 251 étaient remises en page pour rien | wxWidgets : `wxBookCtrlBase::DoSize()` (`src/common/bookctrl.cpp`), et le carnet du portage (`src/winui/notebook.cpp`) | Seules les pages affichées sont redimensionnées ; une page cachée reçoit déjà sa taille quand elle est sélectionnée | Showcase : 100 ms → 27 ms par étape |
| Chaque fenêtre wx est créée avec `WS_CLIPCHILDREN` : à chaque déplacement, Windows recalcule les zones de découpe de toute la hiérarchie. Le temps est passé dans le noyau, dans `EndDeferWindowPos` (7 ms pour les seuls enfants directs du cadre de Widgets) | wxWidgets : `wxWindowMSW::MSWGetStyle()` | Avec les surfaces de peinture, plus aucune fenêtre ne peint son HWND et une surface s'arrête d'elle-même aux enfants : le style n'est plus posé dans ce mode. Sans les surfaces, rien ne change | Widgets : 17,5 ms → 11,9 ms par étape avec les surfaces (9 ms pour le seul retrait du style) |
| Les contrôles d'un `wxStaticBoxSizer` sont enfants de l'encadré : le regroupement des déplacements de la fenêtre mise en page ne les couvre pas, ils étaient déplacés un par un | wxWidgets : `wxStaticBoxSizer::RepositionChildren()` (`src/common/sizer.cpp`) | Regroupement propre à l'encadré | Mise en page de la page de Showcase : 7,7 → 5,7 ms par étape |
| `GetScale()` cherchait la racine XAML à chaque position écrite (un dixième des échantillons d'un profil) | Portage : `src/winui/hostgeometry.cpp` | Échelle mémorisée le temps d'un message | Dans le bruit de la mesure |

Les trois premières causes existent telles quelles dans wxMSW classique ;
la première et la troisième s'y corrigent de la même façon.

Essais sans effet, non retenus : `SWP_NOCOPYBITS` seul. `SWP_NOREDRAW`
réduit le temps noyau d'un tiers mais oblige à invalider soi-même ; à
reprendre quand les surfaces seront le chemin par défaut.

Ce qui suivait ici (« il reste 14 à 16 ms dans la mise en page de la page
visible, refaite deux à trois fois par étape ») s'appuyait sur un profil
faux et n'a pas été confirmé : voir la suite.

### Audit du redimensionnement, suite : le profil était faux (22 novembre)

`profile.ps1` gardait une seule session du débogueur ouverte et y relançait
puis suspendait les fils entre deux relevés. Dans ce mode le débogueur garde
le contexte des registres de son premier relevé : toutes les piles suivantes
étaient reconstruites à partir du même pointeur de pile, périmé, et
montraient la même pile profonde. La conclusion « le temps est dans le
noyau, dans le `EndDeferWindowPos` du cadre » venait de là. L'outil lance
maintenant le débogueur une fois par relevé (trois secondes chacun, mais
juste) ; les mesures de durée par étape, elles, n'étaient pas concernées.

Profil refait sur Showcase, surfaces actives (140 relevés, 116 hors
attente), puis après correction (130 relevés, 94 hors attente) :

| Coût | Part du temps actif | Correction |
|---|---|---|
| La surface de peinture de chaque fenêtre était recréée à chaque étape : nouveau bitmap GDI, nouveau `WriteableBitmap`, recopie, renvoi entier à XAML | 26 % | Bitmap sur-alloué et tuiles, voir « Étape 2, deuxième lot » |
| À chaque déplacement d'un enfant, Windows fait effacer l'île, qui parcourait alors **toutes** les fenêtres du cadre, deux fois, pour ménager celles qui peignent leur HWND | 9 % | Avec les surfaces, dans une fenêtre à matériau (Mica), plus aucune ne le fait : le parcours n'y a lieu que s'il existe une fenêtre qui peint encore son HWND. Sans matériau (dialogues à fond plein) rien ne change (`src/winui/ctrlhost.cpp`, `wxWinUIEraseIslandBackground`) |
| À chaque déplacement d'un enfant, la surface du parent renvoyait une image à XAML, même sans rien avoir à effacer dessous | 9 % | Rien n'est renvoyé si rien n'était peint ; puis, au second passage, plus aucun pixel n'est lu hors du rectangle où la fenêtre a déjà peint |
| Chaque synchronisation de l'hôte demandait une propriété Win32 à toutes les fenêtres du cadre, pages cachées comprises, pour trouver celles qui ont des barres de défilement gérées par wx | 10 % (second profil) | Registre des fenêtres concernées (`src/winui/scrollbarprojection.cpp`) |
| `GetCharHeight()` / `GetCharWidth()` prenaient le contexte graphique de la fenêtre, ce qui fait calculer sa région visible à Windows, plusieurs fois par fenêtre et par mise en page | 6 % | wxWidgets : mesure sur un contexte mémoire gardé pour cela (`wxGetTextMetrics`, `src/msw/window.cpp`). Vaut aussi pour wxMSW classique |

Durée d'une étape (médiane, mesures dos à dos, `outils/resizebench.ps1`) :

| Démo | Sans surfaces (`WX_WINUI_PAINT_SURFACES=0`) | Avec surfaces, le matin | Avec surfaces, après ce lot |
|---|---|---|---|
| Showcase | 21,5 à 24 ms | 21 ms | 10 ms |
| Widgets | 15 à 16,7 ms | 11,9 ms | 7,5 ms |

Showcase était à 100 ms le 21 novembre.

**Ce qui reste**, d'après le second profil : les déplacements de fenêtres
par Windows eux-mêmes (`EndDeferWindowPos`, `MoveWindow` : 40 relevés sur
94), que wxMSW classique paie aussi. La piste connue est `SWP_NOREDRAW`
(Windows n'invalide plus rien, au portage de demander lui-même les
peintures nécessaires) ; elle touche au contrat de repeinte de toutes les
fenêtres et n'a pas été engagée sans retour d'écran sur la fluidité
obtenue. La boucle d'événements semble aussi tourner à vide entre deux
étapes (`PeekMessage` et son crochet : 20 relevés d'attente sur 36) ; à
regarder.

### Étape 2, deuxième lot : surfaces de peinture par défaut (22 novembre)

Écrit, compilé, testé, vu en capture ; pas encore vu à l'écran. Les
surfaces sont maintenant le chemin normal ; `WX_WINUI_PAINT_SURFACES=0`
remet la peinture du HWND, pour comparer quand quelque chose paraît faux.

| Point | Ce qui est fait |
|---|---|
| Mises à jour partielles | Le bitmap d'une fenêtre est plus grand qu'elle (arrondi à 64 pixels, réduit seulement quand l'écart en vaut la peine) et XAML le montre par tuiles de 512 pixels, une image par tuile. Une peinture ne renvoie que les tuiles touchées ; une fenêtre qu'on redimensionne garde son bitmap et ses tuiles d'une étape à l'autre. Pour une fenêtre de droite à gauche le bitmap est calé sur son bord droit, celui où Windows garde son contenu |
| Passes sur la mémoire | Une seule par peinture (rendre opaque ce que GDI vient d'écrire et le copier vers la tuile se font ensemble) ; un effacement après lequel rien n'est dessiné n'en coûte aucune |
| Marquage des pixels | « Jamais peint » n'est plus noir avec alpha 254 mais une couleur clé presque noire avec alpha 0. Un pixel que GDI a parcouru sans le changer (partie masquée d'un bitmap à masque, rectangle englobant relu et réécrit par GDI+) reste transparent au lieu de devenir un carré noir, et ce que `AlphaBlend` y dépose garde l'alpha de sa source : le bord doux d'une icône reste doux |
| `wxCaret` | Le curseur texte de Win32 est dessiné sur le HWND, qui ne se voit plus : la fenêtre a un élément XAML à la place (une « partie » de plus), clignotant au rythme du système, sombre sur fond clair et clair sur fond sombre. Le curseur Win32 existe toujours, caché, et suit la position : les méthodes de saisie et les outils d'accessibilité la lisent (`src/msw/caret.cpp`) |
| `EVT_ERASE_BACKGROUND` | Sous Mica l'événement **n'était plus envoyé du tout** : ce qu'une application dessinait là n'apparaissait pas (démo `erase`). Il est envoyé de nouveau, avec un contexte sur la surface. Une fenêtre sans surface le reçoit d'abord sur son HWND et n'obtient une surface que si un gestionnaire l'a pris |
| Fenêtre transparente (`wxBG_STYLE_TRANSPARENT`) | Son parent peint dessous, comme sur un HWND, et elle repart de rien à chaque peinture |
| Vrais HWND qui se peignent | Les fenêtres n'ont plus `WS_CLIPCHILDREN` (c'était la moitié du coût d'un redimensionnement). Celles qui contiennent une fenêtre qui n'est pas à wx (navigateur, ActiveX) ou qui possède son contexte (canevas OpenGL) le reprennent à la création de cette fenêtre (`WM_PARENTNOTIFY`) ; les fenêtres de premier niveau le gardent toujours |

Vérifié en capture sur les démos `splitter`, `erase`, `caret` (texte tapé,
curseur visible), `drawingsample`, `scroll` et quatre pages de Showcase, en
mode normal, de droite à gauche et à 150 %, avant et après
redimensionnement dans les deux sens.

**Limites connues :**

- Le texte lissé par GDI est mélangé avec la couleur clé, presque noire,
  là où rien n'est peint dessous : un texte clair garde un liseré sombre
  s'il se retrouve sur un fond clair peint par une autre fenêtre (le
  contrôle transparent de la démo `erase`).
- Les enfants d'une fenêtre qui peint son fond (démo `erase`) montrent le
  fond de la fenêtre de premier niveau dans leur rectangle, pas le dessin
  du parent : c'est l'équivalent de la « boîte » de couleur de fond de
  wxMSW classique, où le parent ne peint pas non plus sous ses enfants.
- Ce qu'une application dessine sur le HWND par un autre chemin
  (`wxWindowDC`, `wxScreenDC`, GDI direct) n'est pas redirigé.

**Piège de capture** : sur le bureau invisible Windows ne livre `WM_PAINT`
que si on le demande, ce que fait la capture ; ce qu'une fenêtre peint
après un redimensionnement est peint pendant la capture et XAML l'affiche
une image plus tard. Il faut capturer deux fois et garder la seconde. Une
heure a été perdue à chercher là un défaut des surfaces.

### Ruban : chrome et commandes en WinUI, sur le modèle du Toolkit (22 novembre)

Demande : que le ruban ait par défaut le style WinUI 3, en prenant pour
modèle le Ribbon du Windows Community Toolkit (Labs), et en analysant
vraiment ce que le Toolkit fait et ce que les contrôles natifs permettent.
Écrit, compilé, vu en capture, en partie confirmé à l'écran au fil de
l'eau ; les points encore à confirmer sont listés à la fin.

**État de départ.** Dans le build par défaut, seules les commandes étaient
des contrôles WinUI ; tout le reste (fond, cadres et étiquettes des groupes,
onglets, flèches) était dessiné en GDI par l'ancien fournisseur d'art, avec
ses métriques. Le ruban « natif » de Codex (`ribboncontainerswinui.cpp`,
2 400 lignes) s'appuie sur le portage C++ du Toolkit et n'est compilé
qu'avec une option expérimentale, en build statique : il n'est pas le
chemin par défaut et ne l'est pas devenu.

**Ce que fait le Toolkit** (lu dans `Ribbon.cs`, `RibbonPanel.cs`,
`RibbonGroup.cs`, `RibbonCollapsibleGroup.cs`, `RibbonStyle.xaml`) :

| Élément | Toolkit |
|---|---|
| Bande | `LayerFillColorDefaultBrush` entre deux traits `CardStrokeColorDefaultSolidBrush`, 4 px de marge intérieure, défilement horizontal par deux boutons opaques aux extrémités |
| Groupe | 12 px de marge de chaque côté, 4 dessus et dessous ; contenu centré (72 px au moins) ; étiquette dessous, 12 px, couleur secondaire, coupée par des points de suspension |
| Séparation | un trait `DividerStrokeColorDefaultBrush` d'un pixel au bord du groupe, pas de cadre |
| Groupe replié | un bouton discret (icône + petit chevron) qui ouvre un `Flyout` contenant le groupe et son étiquette ; repli par ordre de priorité quand la place manque |
| Commandes | contrôles WinUI ordinaires : `AppBarButton` sans étiquette 48×48, `Button`, `DropDownButton`, `ToggleButton`, `GridView` |
| Onglets | aucun : le Ribbon du Toolkit est une bande, les onglets sont à l'application |

**Architecture retenue.** wxRibbon garde ce qu'il sait faire et que les
applications utilisent (modèle, mise en page adaptative, événements, HWND
par fenêtre) ; chaque fenêtre du ruban montre ce qui lui revient par un
vrai contenu XAML, sous les fenêtres de ses enfants, comme le fait
`wxStaticBox`. Les gestionnaires de peinture, qui dessinaient avec le
fournisseur d'art, mettent ce contenu à jour : tout ce qui provoquait un
redessin provoque maintenant la mise à jour. Le ruban du Toolkit lui-même
n'est pas repris comme contrôle : il imposerait sa propre mise en page
(repli par priorité) à la place de celle de wxRibbon, et n'existe en natif
que par une dépendance tierce en version alpha.

| Partie | Avant | Maintenant | Fichiers |
|---|---|---|---|
| Bande d'une page | dégradé GDI | `Border` aux pinceaux du Toolkit | `ribbonchromewinui.h`, `page.cpp` |
| Groupe | cadre et étiquette GDI | séparateur, étiquette et lanceur XAML ; séparateur horizontal quand les groupes sont empilés (ruban vertical) | `ribbonchromewinui.h`, `panel.cpp` |
| Groupe replié | dessin GDI, clic n'importe où | vrai `Button` discret (icône + chevron) : lui seul répond au pointeur | `panel.cpp` |
| Groupe déplié | fenêtre nue, fermée au moindre changement de focus | carte de flyout (coins arrondis, bordure), fermée quand sa fenêtre est désactivée | `panel.cpp` |
| Onglets | dessin GDI | `SelectorBar` (le contrôle que WinUI recommande à la place de `Pivot`), défilable à la molette quand il y en a trop ; ou `TabView` avec le nouveau style `wxRIBBON_BAR_TAB_VIEW` (bouton « Tabs as TabView » dans la démo) | `bar.cpp`, `bar.h` |
| Replier / aide | glyphes GDI | boutons XAML ; le bouton montre une punaise quand le ruban est ouvert provisoirement, et l'épingle | `bar.cpp` |
| Flèches de défilement d'une page | dessin GDI, qui laissait passer le pointeur vers les commandes dessous | vrais boutons opaques | `page.cpp` |
| Commande simple / bascule | `AppBarButton` / `AppBarToggleButton` | `Button` / `ToggleButton` discrets à contenu | `ribboncommandwinui.h` |
| Commande à menu | `AppBarButton` + bouton flèche séparé | `DropDownButton` | idem |
| Commande action + menu | `AppBarButton` + bouton flèche séparé | `SplitButton` | idem |
| Boutons d'une galerie | trois `Button` bordés collés aux éléments | trois boutons discrets dans un cadre fin, écartés | `gallerywinui.h` |
| Métriques | celles du dessin GDI | celles du Toolkit, dans un fournisseur d'art devenu celui par défaut | `art_winui.cpp`, `art.h` |

**Défauts trouvés en route**, tous dans le portage :

- Un `AppBarButton` hors d'une `CommandBar` se mesure à sa largeur minimale
  (32 px) sans compter son étiquette : les sections étaient trop étroites et
  les textes coupés. D'où les boutons « à contenu », qui se mesurent à ce
  qu'ils montrent ; une étiquette longue passe sur deux lignes sous une
  grande icône.
- Une galerie était reconstruite en entier dès qu'on lui donnait un autre
  fournisseur d'art, ce que la démo fait à chaque survol d'une couleur :
  l'animation d'apparition rejouait sans cesse. Elle ne l'est plus que si la
  direction du flux change, et sans animation d'entrée.
- Le `SelectorBar` fait 48 px de haut ; dans 40, sa marque de sélection,
  tout en bas, était rognée.
- La barre prenait le focus à chaque pression, ce qui retirait la pression
  au bouton XAML visé (replier, aide).
- Groupe à sizer : la taille de son flyout était demandée pendant que ses
  enfants étaient cachés (un sizer ignore les fenêtres cachées), le groupe
  déplié se repliait donc à son tour et cachait son contenu ; et sa taille
  minimale, mémorisée avant que les contrôles WinUI aient la leur, le
  laissait trop étroit après un rétrécissement suivi d'un agrandissement.
- Un ruban vertical ne se repliait pas : wxRibbon replie en réduisant la
  hauteur de la barre, que le sizer d'un ruban latéral impose.
- Plantage à l'ouverture d'un groupe replié : le groupe déplié était mis en
  page avant d'avoir son fournisseur d'art.
- Galerie : la largeur qui ne fait pas une colonne entière restait entre
  les éléments et les boutons (jusqu'à une colonne moins un pixel) ; les
  éléments prennent maintenant la place de colonnes entières et les boutons
  les suivent.
- Galerie : une position de défilement demandée et jamais atteinte (hors
  limites) restait « en attente ». La position où la vue s'arrête fait foi.
- Galerie dans le flyout d'un groupe replié : à la molette, la vue revenait
  au début tant qu'on n'avait pas cliqué une de ses flèches. **Corrigé,
  confirmé à l'écran.** Trois corrections faites sur des hypothèses n'ont
  rien changé ; la cause a été trouvée par une trace
  (`WX_WINUI_INPUT_LOG`), puis reproduite sur le bureau invisible avec un
  défilement animé déclenché par le programme (`ChangeView` sans
  `disableAnimation` passe par le même chemin que la molette) :
  - la flèche « haut » d'une galerie est désactivée quand la vue est au
    début et réactivée dès qu'elle l'a quitté d'un demi-pixel ;
  - **dans une île XAML où le focus n'est jamais entré, XAML répond à un
    arrêt de tabulation qui devient actif en ramenant en vue le premier
    élément focalisable**, ici le premier élément de la liste : la vue
    repartait à 0 à chaque réactivation de la flèche (34 fois pour un
    défilement de 100 pixels), que la vue soit en mouvement ou arrêtée ;
  - le flyout d'un groupe replié est exactement ce cas : le focus Win32
    reste sur la fenêtre du groupe, qui n'a rien de focalisable à elle,
    jusqu'au premier clic dans son contenu. Dans la fenêtre principale une
    commande a toujours eu le focus et le défaut ne se voit pas.

  Les deux flèches de défilement ne sont plus des arrêts de tabulation (le
  clavier fait défiler en se déplaçant parmi les éléments) ; le troisième
  bouton, jamais désactivé, le reste. **À retenir pour les autres
  composants** : activer ou désactiver un contrôle XAML qui est un arrêt de
  tabulation peut faire défiler une liste de la même fenêtre tant que le
  focus n'y est jamais entré.
- Galerie : la taille demandée à la mise en page et la place donnée aux
  éléments ne comptaient pas la même largeur pour les boutons, une colonne
  était perdue et sa place restait vide à côté d'eux.
- wxRibbon : la page ne suivait pas un changement de hauteur des onglets
  sans changement de taille de la barre (autre fournisseur d'art) ; elle
  gardait sa place jusqu'au redimensionnement suivant. Corrigé dans
  `wxRibbonBar::Realize()`, ce que le test de stabilité de mise en page a
  révélé puisque les onglets WinUI n'ont pas la hauteur des autres.

**Reste à faire, par ordre d'intérêt :**

- Le flyout d'un groupe replié est encore une fenêtre wx habillée en
  flyout, parce que ses enfants sont des fenêtres : un vrai `Flyout` XAML
  demande d'y héberger leur contenu.
- Les onglets d'un ruban vertical étroit débordent (ils défilent, sans
  plus) ; un `NavigationView` conviendrait mieux à cette disposition.
- Barres d'outils du ruban : rangées de petits boutons, à revoir contre
  `CommandBar` (débordement natif).
- Le chemin expérimental Toolkit n'a pas été recompilé avec ces
  changements.
- Le flyout d'un groupe replié ne prend pas le focus dans son contenu à
  l'ouverture : la demande de focus sur le groupe déplié n'aboutit pas
  (il n'a rien de focalisable à lui) et celle sur son premier enfant non
  plus, sur le bureau invisible du moins. À reprendre avec le focus
  (étape 3 et 4.6).
- À confirmer à l'écran après les derniers correctifs : l'option `TabView` (écrite et
  compilée, jamais vue affichée : le bouton de la démo ne se laisse pas
  actionner sur le bureau invisible), la largeur exacte des galeries, les
  boutons des groupes repliés en vertical.
- Style des trois boutons d'une galerie : jugé un peu étrange pour
  Windows 11 par l'utilisateur, « à voir ».

### Étape 2, troisième lot : inventaire et bordures (22 novembre)

**Inventaire.** `drive.ps1` a une commande de plus (`topbig`, la plus grande
fenêtre wx du programme) et la liste des fenêtres de 36 démos a été relevée
(pages de Showcase, Widgets, dialogues, DataView, Grid, PropertyGrid, AUI,
ListCtrl, TreeList, ruban, texte, HTML, MDI…). Il ne reste **aucun contrôle
USER32 ou comctl32** : tout est fenêtre wx montrée par XAML ou peinte sur
une surface. Les seules vraies fenêtres visibles sont étrangères par
nature : WebView2 (`Chrome_*`), `MDIClient` (MDI de Win32) et le
conteneur ATL du lecteur multimédia. `wxStyledTextCtrl` est une fenêtre wx
de classe « Scintilla », peinte par wx sur une surface.

**Bordures.** C'était le dernier dessin GDI présenté directement : la
bande non cliente d'une fenêtre à bordure (`wxBORDER_SIMPLE`, `SUNKEN`,
`THEME`, `STATIC`, `RAISED` : liste HTML, canevas, `wxVListBox`…) était
peinte sur le HWND à `WM_NCPAINT`, un trait blanc carré en mode sombre et le
relief classique de USER32 en mode clair. Elle est maintenant une « partie »
XAML de plus (`WindowPartKind::Frame`) : un `Border` d'un pixel, coins de
4, au pinceau `ControlStrokeColorDefaultBrush` qui suit le thème, comme le
contour des contrôles voisins. La géométrie Win32 ne change pas (la zone
cliente reste rentrée de l'épaisseur de la bordure). Cette partie est posée
dans le canevas du **parent**, puisque le canevas d'une fenêtre représente
sa zone cliente et peut y découper. Elle vaut aussi pour une fenêtre montrée
par un contrôle XAML qui a une bordure de wx autour d'elle
(`wxPropertyGrid`). Vérifié en capture sur `htlbox`, `scroll` et
`propgriddemo` ; les dix groupes de l'hôte passent.

**L'étape 2 est tenue pour finie.** Limites qui restent, toutes notées
plus haut : texte lissé par GDI sur une zone jamais peinte (liseré sombre
quand il se retrouve sur le fond clair d'une autre fenêtre), `wxWindowDC` /
`wxScreenDC` / GDI direct non redirigés, et les traits fantômes dessinés
sur l'écran (`wxSashWindow`, séparateur sans `wxSP_LIVE_UPDATE`).

### Étape 3, premier lot : le clic droit (22 novembre)

Écrit et compilé ; **à confirmer à l'écran** (aucune entrée ne peut être
simulée sur le bureau invisible).

`wxEVT_CONTEXT_MENU` n'arrivait jamais à un contrôle montré par XAML : son
« slot » fabrique ses événements souris lui-même et USER32 ne voit donc
aucun `WM_RBUTTONUP` dont tirer `WM_CONTEXTMENU`. L'hôte écoute maintenant
`ContextRequested` à la racine de l'île (clic droit relâché, Maj+F10,
touche Menu), seulement quand rien ne l'a traité plus bas, retrouve le
contrôle wx par son slot et lui envoie `wxEVT_CONTEXT_MENU` avec la position
à l'écran (`wxDefaultPosition` au clavier, comme wxMSW). L'événement remonte
aux parents comme toute commande. S'il est traité, la demande XAML est
marquée traitée : le menu propre à certains contrôles (couper / copier /
coller d'une zone de texte) ne s'ouvre pas en plus. Les fenêtres peintes
n'étaient pas concernées : le routeur leur transmet les boutons et USER32
fait le reste.

À surveiller : `wxTreeCtrl` et `wxToolBar` traduisent déjà `RightTapped`
en leurs événements propres (`wxEVT_TREE_ITEM_MENU`,
`wxEVT_TOOL_RCLICKED`) ; un programme qui traite à la fois celui-là et
`wxEVT_CONTEXT_MENU` pourrait voir les deux.

### Étape 3, deuxième lot : trois options (22 novembre)

Écrit et compilé. **Rien n'est actif par défaut** : chaque volet s'allume
par une variable d'environnement, pour être essayé à la main puis rendu
normal une fois vu à l'écran, comme les surfaces de l'étape 2. Aucune entrée
ne peut être simulée sur le bureau invisible ; les tests qui pilotent le
routeur par ses points d'accès de test tournent avec les options.

| Variable | Ce qu'elle change |
|---|---|
| `WX_WINUI_INPUT_QUEUE=1` | **Distribution en file.** Ce que le pointeur fait au-dessus d'une fenêtre qui n'est pas montrée par XAML (fenêtre peinte, panneau) n'est plus livré depuis le délégué XAML : l'échantillon attend dans une file que la boucle de messages vide, par un message posté (toute boucle le reçoit, celles de Windows comprises). Un gestionnaire de clic qui ouvre un dialogue ou fait tourner sa propre boucle ne le fait donc plus à l'intérieur de XAML, cause supposée des gels de dialogues. Un seul mouvement attend à la fois. Chaque échantillon est retiré avant d'être livré : une boucle imbriquée livre les suivants dans l'ordre, comme des messages postés. Ce que XAML possède reste traité sur place, après ce qui attendait |
| `WX_WINUI_INPUT_MESSAGES=1` | **Messages Win32 aux contrôles.** Le slot d'un contrôle XAML envoie `WM_MOUSEMOVE`, `WM_LBUTTONDOWN`, `WM_RBUTTONUP`, `WM_MOUSELEAVE`… à son HWND au lieu de fabriquer l'événement wx. wxMSW en tire les événements comme pour toute fenêtre, une surcharge de `MSWWindowProc` les voit, et USER32 tire `WM_CONTEXTMENU` du bouton droit (le traitement de `ContextRequested` du premier lot ne double pas l'événement et sert encore à écarter le menu propre au contrôle). Le focus d'un contrôle cliqué reste l'affaire de XAML : wxMSW ne le redonne pas à la fenêtre |
| `WX_WINUI_INPUT_NODES=1` | **Un canevas par fenêtre, et comparaison.** Chaque fenêtre wx reçoit un canevas, qui ne montre rien et **ne prend aucune entrée**. La fenêtre dont le canevas est le plus haut sous le pointeur (cherché parmi tous les éléments au point, ceux que le test de pointage ignore compris) est comparée à celle que trouve le résolveur natif ; rien n'est routé par elle. Chaque désaccord est écrit une fois dans le journal (`WX_WINUI_INPUT_LOG`), ligne « nodes DISAGREE » |

**Tests.** Options éteintes, les onze groupes de l'hôte et du ruban passent
(`[HostLifecycle]` 119 sur 119) et le smoke aussi : le comportement normal
n'a pas bougé. Les trois options allumées ensemble, huit groupes relancés
passent, à une assertion près :
`winuihostlifecycle.cpp:13723` attend qu'un enfant natif d'une fenêtre de
premier niveau montrée par XAML soit signalé comme « ordre d'empilement
impossible » ; avec un canevas par fenêtre cet enfant en a un, et le
diagnostic n'a plus lieu d'être. C'est l'effet voulu de l'option, pas un
défaut.

**Piège rencontré.** Ajouter un membre à `wxWinUITopLevelHost` sans
recompiler `test_gui.exe`, qui inclut l'en-tête privé et lit ses membres en
ligne, donne des échecs de focus absurdes (pointeurs nuls ou aberrants) et
une violation d'accès. Vingt minutes ont été perdues à les attribuer au
volet « messages Win32 ». Il faut recompiler la cible `test_gui` après tout
changement de cet en-tête.

**Premier essai à l'écran (22 novembre au soir), trois défauts, corrigés :**

- *Boutons OK des dialogues inertes* (About, message, texte et nombre,
  couleur). Dans la première version de l'option des canevas, ceux-ci
  avaient un fond transparent pour être touchés par le pointeur ; ils se
  retrouvaient **au-dessus** du contenu XAML d'un dialogue, que rien de
  natif ne recouvre d'ordinaire, et prenaient le clic. Le journal le
  montrait : des appuis mis en file comme « espace natif » alors que le
  point était sur du contenu XAML. Les canevas ne prennent plus rien ; la
  comparaison les retrouve par une requête qui inclut les éléments
  invisibles au pointage. **Confirmé à l'écran.**
- *Molette bloquée au-dessus des zones peintes* : même cause.
- *Séparateur qui ne suit plus le pointeur quand on réduit le panneau de
  gauche* (il sautait au relâchement). Cause dans le volet « messages
  Win32 » : pendant le glisser, le contrôle survolé (l'arbre de gauche)
  recevait un `WM_MOUSEMOVE` à la même position que celui transmis au
  séparateur, et wxMSW jette tout mouvement dont la position est celle du
  dernier message souris (`gs_lastMouseEvent`). Tant qu'un bouton est tenu
  sur une fenêtre qui n'est pas montrée par XAML, elle seule reçoit les
  messages, comme la fenêtre qui a la capture en Win32. À confirmer.

Le journal de cet essai compte 34 désaccords entre la fenêtre désignée par
XAML et celle du résolveur natif, à étudier avant de router par XAML : les
panneaux d'un `wxSplitterWindow` (bande du séparateur), `wxComboCtrl` et
`wxOwnerDrawnComboBox` dans une fenêtre défilante, la liste de
`wxFileCtrl`, et les fenêtres de premier niveau des dialogues, qui n'ont
pas de canevas.

### Étape 3, troisième lot : tout par défaut, routage par la fenêtre que XAML désigne (22 et 23 novembre)

Les trois options du deuxième lot, vues à l'écran le 22 au soir, sont
actives **sans variable d'environnement**. La valeur `0` coupe chacune
(`WX_WINUI_INPUT_QUEUE=0`, `WX_WINUI_INPUT_MESSAGES=0`,
`WX_WINUI_INPUT_NODES=0`) ; `WX_WINUI_INPUT_NODES=2` garde la comparaison
avec le parcours complet (lignes « nodes DISAGREE » du journal).

Le pointeur au-dessus d'une fenêtre qui n'est pas montrée par XAML est
routé **à partir de la fenêtre que XAML désigne**, celle dont le canevas
est le plus haut au point (`PointOverXamlContent`). Le résolveur natif ne
parcourt plus l'arbre des HWND depuis la fenêtre de premier niveau :
`wxWinUIResolveNativeHitFrom` (`CollectFrom`, `inputrouter.cpp`) part de
cette fenêtre, vérifie par Win32 qu'elle convient (le point est dans son
rectangle, aucune sœur au-dessus ne la couvre ; cachée ou désactivée, il
remonte à son parent), puis ne regarde que ses enfants au point, elle-même
et ses ancêtres. Quand XAML ne désigne rien ou que la vérification échoue,
le parcours complet reste le recours. `WM_NCHITTEST` est toujours envoyé
aux candidats : c'est lui qui sépare la bande d'une barre de défilement, un
cadre et la zone cliente, cause de la plupart des désaccords du journal du
22.

Les canevas d'entrée n'existent que pour les fenêtres sans contrôle XAML ;
les autres ont déjà leur slot.

Un test différait, `winuihostlifecycle.cpp:13723` : il comptait un ordre
d'empilement « impossible » pour une fenêtre sans canevas, qui en a un
désormais. Adapté. Le 23, tout par défaut : les dix groupes de l'hôte,
`[winui-ribbon]` et le démarrage des démos passent. **Le routage par XAML
n'est pas encore confirmé à l'écran.**

### Étape 3, quatrième lot : une seule file pour tout ce que fait le pointeur (23 novembre)

Jusque-là seul ce que le pointeur faisait au-dessus d'une fenêtre peinte
attendait la boucle de messages. Les événements souris d'un contrôle XAML
(son slot) étaient encore livrés à wx **depuis le délégué XAML**, et la
racine y traitait de même ce que XAML possède (quitter la fenêtre peinte
que le pointeur vient de laisser, demander son curseur au contrôle
survolé). Les trois attendent maintenant dans **une seule file, dans leur
ordre** (`wxWinUITopLevelHost::QueuedPointer` : `Native`, `Xaml`, `Slot`),
vidée par le même message posté.

Pour y arriver, la lecture de l'événement XAML est séparée de ce qu'on en
fait :

- `OnSlotPointer` ne fait plus que lire l'événement (échantillon,
  observation) ; `ProcessSlotPointerSample` fait le reste, inchangé :
  machine à états du slot, puis messages Win32 au HWND du contrôle. La
  machine à états tourne donc à la livraison, pas à la lecture : ses
  validations (`CommitBeforeDispatch`…) restent celles d'une suite
  d'événements traités un par un.
- Le curseur d'un contrôle XAML se décide en deux temps :
  `FindXamlPointerCursorSlot` trouve le contrôle d'où vient l'événement
  tant que celui-ci est là, `ApplyXamlPointerCursorForSlot` lui demande son
  curseur à la livraison.

Deux choses ne peuvent pas attendre et sont traitées sur place, après ce
qui attendait : la fin d'un geste (annulation, capture perdue), qui défait
son état, et **le bouton droit relâché**. XAML en tire son
`ContextRequested`, levé aussitôt après, qui doit savoir à ce moment si le
programme a répondu à `wxEVT_CONTEXT_MENU` pour écarter ou non le menu
propre au contrôle. Un menu ouvert par un gestionnaire de clic droit
tourne donc encore dans un délégué XAML ; c'est le seul cas restant.

De même, la racine traite encore sur place ce que XAML possède tant qu'un
bouton est tenu ou que la souris est capturée : le routage peut alors
demander de retirer l'événement à XAML, ce qui ne se fait que pendant
l'événement.

Un mouvement au plus attend pour chaque destinataire ; un mouvement
au-dessus d'un contrôle attend en deux entrées (slot, racine), que le
regroupement regarde ensemble.

`WX_WINUI_INPUT_QUEUE=1` revient au comportement du troisième lot (seules
les fenêtres peintes attendent), `=0` livre tout sur place.

**Vérifié le 23 novembre** : les dix groupes de l'hôte, `[winui-ribbon]`, `[winui-font-dialog]` et `[winui-picker-contract]` passent, les démos démarrent sans plantage. Ces tests injectent leurs échantillons après la lecture de l'événement XAML : ils ne passent **pas** par la file. Le bureau invisible ne permet pas d'envoyer de vraie souris à un contrôle XAML ; ce lot ne se vérifie donc qu'à l'écran. Lancés avec `WX_WINUI_INPUT_NODES=2` et le journal, ces tests n'ont écrit aucune ligne « nodes DISAGREE » ni « route WALK », ce qui ne dit rien d'une utilisation réelle.

**Décisions sur le reste de l'étape 3**

- *Capture.* Le plan prévoyait d'appuyer `CaptureMouse` sur la capture XAML
  du nœud. À l'examen, non : `CaptureMouse` fait un `SetCapture` sur le
  HWND de la fenêtre, et Windows lui envoie alors directement les messages
  souris, y compris hors de la fenêtre de premier niveau. C'est déjà
  « livré au bon HWND », et `::GetCapture()` doit continuer de nommer la
  fenêtre wx pour le code écrit pour wxMSW. Passer par la capture XAML
  ferait mentir `::GetCapture()` et demanderait de refaire
  `wxWindow::GetCapture()`, `WM_CAPTURECHANGED` et
  `wxEVT_MOUSE_CAPTURE_LOST`. Les gardes qui réconcilient les deux sources
  pendant une capture (`SyncNativeCapture`, `NotifyNativeCaptureMutation`)
  restent donc.
- *Parcours complet des fenêtres.* Il n'est plus que le recours quand aucun
  canevas ne désigne de fenêtre. Le journal (`WX_WINUI_INPUT_LOG`) écrit
  une ligne « route WALK found … » chaque fois que ce recours trouve une
  autre fenêtre que celle de premier niveau, c'est-à-dire chaque fois
  qu'un canevas manquait. Il sera retiré du routage quand un journal
  d'utilisation réelle n'en montrera plus ; il sert aussi à
  `wxFindWindowAtPoint`, au courtier de glisser-déposer et à
  `wxWinUIRefreshNativeHit`.

**Reste de l'étape 3 :**

1. Confirmation à l'écran des troisième et quatrième lots.
2. Un journal d'utilisation réelle, pour retirer le parcours complet.

### Dialogue de polices, liste de fichiers, barre de progression (23 novembre)

**`wxFontDialog`.** Le portage montrait le dialogue générique de wx, dont la
disposition n'a rien de celle de Windows. Le dialogue de Windows
(`ChooseFont`, dans `comdlg32.dll`) n'est pas du code wx : il n'y a rien à
recompiler, il faut l'écrire une fois dans le portage, comme les dialogues
de couleur et de message. `src/winui/fontdlg.cpp` (`wx/winui/fontdlg.h`) en
reprend la disposition et le comportement avec les contrôles wx du portage,
donc de vrais contrôles WinUI, chacun avec son HWND : trois colonnes
Police / Style / Taille (un champ au-dessus d'une liste), Effets (barré,
souligné, seize couleurs), Aperçu, Script, OK / Annuler. Le code d'une
application ne change pas (`wxFontDialog dialog(parent, data)`). Le
générique reste disponible sous son nom, `wxGenericFontDialog`.

- Les familles et leurs styles viennent de DirectWrite (`dwrite`, liée à
  `wxcore`) : « Segoe UI » avec Light, Semibold…, comme dans le dialogue de
  Windows, et non les familles de GDI (« Segoe UI Light »). La police rendue
  est décrite comme GDI la nomme (`ConvertFontToLOGFONT`), avec le jeu de
  caractères du script choisi et la famille (`lfPitchAndFamily`) lus par
  `EnumFontFamiliesEx` : c'est ce que rend `ChooseFont`. Les obliques que
  DirectWrite simule sont retirées quand la famille a un vrai italique. Les
  polices bitmap de GDI (« MS Sans Serif », « Courier »…) n'y sont pas :
  XAML ne sait pas les dessiner.
- Chaque nom est écrit dans sa police, chaque style dans son style, par une
  accroche de `wxListBox` (`wx/winui/private/listitemfont.h`) qui règle la
  police du vrai `ListViewItem` et donne à ces listes les lignes compactes
  de WinUI (32 au lieu de 40).
- Ce qui est tapé dans un champ fait suivre sa liste ; OK refuse un nom de
  police ou de style inconnu et une taille hors limites, avec les messages
  du dialogue de Windows. Rien n'est écrit dans `wxFontData` avant OK.
- Une liste ne défile qu'une fois affichée et mise en page : la police
  initiale est amenée en tête de sa liste au repos, jusqu'à ce qu'elle soit
  en vue.
- Le cadre des listes est un `wxPanel` à bordure, dessinée par XAML. Sa
  taille minimale est fixée : celle qu'un panneau à bordure calcule dépend
  de sa taille du moment (`GetSize() - GetClientSize()`), nulle tant qu'il
  n'est pas mis en page, ce qui faisait grandir le dialogue de 4 pixels au
  second ajustement (vu par `FontDialogLayoutCyclesAndOwner`).

Vu en capture (thème sombre, 100 % et 150 %) : police initiale en tête de
liste, styles, taille, aperçu rouge de 72 points coupé dans son cadre. Pas
encore vu à l'écran, ni en thème clair.

**À reprendre : `wxComboBox(wxCB_SIMPLE)`.** Les colonnes du dialogue de
Windows sont des combos « simples » (`CBS_SIMPLE`), que le portage
implémente déjà (`combobox.cpp` : un `TextBox` suivi d'une liste toujours
visible). Le dialogue ne s'en sert pas : il assemble un `wxTextCtrl` et un
`wxListBox`, ce qui double le contrôle. La comparaison côte à côte montre
pourquoi il n'a pas été repris tel quel : sa liste est l'ancien `ListBox`
XAML (lignes de 40, sélection en bloc plein, pas de cadre). Le remettre sur
`ListView`, avec la police par élément, puis y passer les trois colonnes du
dialogue, est le bon correctif : il sert à toutes les applications.

**Liste de `wxFileCtrl` (mode liste de `wxListCtrl`).** Deux causes
distinctes aux défauts signalés :

- *Retard et chevauchements au défilement.* La projection ne publie que les
  éléments visibles : chaque pas de défilement en retire et en ajoute. Avec
  les transitions du style par défaut du `ListView`
  (`ItemContainerTransitions`), les éléments retirés restaient à leur
  ancienne place pendant leur fondu (vu en capture 60, 120 et 220 ms après
  un pas) et les nouveaux arrivaient ensuite, par-dessus. Transitions
  retirées dans `listprojection.cpp`, pour tous les modes de `wxListCtrl` :
  60 ms après un pas, les nouveaux éléments sont en place.
- *Une seule ligne.* Dans Showcase le contrôle avait 180 de haut : le champ
  et la liste de choix de WinUI, plus hauts que ceux de Windows, n'en
  laissaient que 51 à la liste. 280 désormais. La disposition était déjà
  celle de Windows : colonnes remplies de haut en bas, petite icône à
  gauche du nom, défilement horizontal.

Une application qui fixe en pixels la hauteur d'un contrôle composé
retrouvera cet effet : les contrôles WinUI sont plus hauts que ceux de
Windows.

**`wxGauge`** était invisible : l'hôte met à zéro la hauteur minimale du
contenu d'un slot, et c'est elle que le gabarit du `ProgressBar` prend pour
épaisseur de l'indicateur. Elle est donnée en valeur locale (ressource
`ProgressBarMinHeight`).

**Test de `wxCollapsiblePane`.** `winuishowcaselayout.cpp` cherchait
l'en-tête du contrôle générique pour lui envoyer un caractère ; depuis le
passage à l'`Expander` il prend le chemin de sa notification
(`WinUIUserToggled`), qui met aussi l'`Expander` dans le même état.

**Tests du 23 novembre** (bureau invisible, écran à 150 %) : les dix groupes
de l'hôte, `[winui-ribbon]`, `[winui-font-dialog]`,
`[winui-dialog-contract]`, `[winui-picker-contract]`,
`[winui-file-browsers]`, `[winui-filectrl]`, `[winui-listmodel]` passent, et
les démos démarrent sans plantage. `[winui-listctrl]` : 26 sur 27 ; le test
qui échoue (`winuilistctrl.cpp:1388`, `HitTest` en droite-à-gauche) échoue
de la même façon sur les binaires figés du 22 (`F:\wxbevue-snap`) : c'est
le défaut « wxListCtrl de droite à gauche à 150 % » déjà connu.

### Étape 5, premier lot : tables et listes au style WinUI (23 novembre)

Demande : que le style, la mise en page et les marges de chaque composant
soient strictement ceux de WinUI 3, tables et listes en tête (« quand on
sélectionne ils sont tout bleu »), glisser-déposer des colonnes compris.

**Trois outils communs**, pour ne pas refaire le même réglage composant par
composant :

- *Les couleurs du thème pour ce que wx peint* (`wx/winui/private/themecolours.h`,
  sans type WinRT, utilisable depuis les sources génériques) :
  `wxWinUIGetThemeColour` rend la couleur du pinceau WinUI du même rôle
  (accent, texte de premier et de second rang, remplissage subtil, filet,
  trait de contrôle…), lue dans les ressources de l'application et gardée
  une seconde ; `wxWinUIBlendOver` la pose sur un fond opaque, la plupart
  étant translucides.
- *`wxSYS_COLOUR_HIGHLIGHT` est la couleur d'accent de WinUI*, et
  `wxSYS_COLOUR_HIGHLIGHTTEXT` la couleur que WinUI écrit dessus (noir en
  thème sombre) : le bleu de Windows classique n'apparaît plus à côté de
  l'accent des contrôles XAML. Conséquence à connaître : tout code peint
  qui écrit en `HIGHLIGHTTEXT` sur autre chose que `HIGHLIGHT` devient
  illisible en thème sombre ; les cas trouvés sont corrigés ci-dessous.
- *Styles XAML partagés* (`appearance.cpp`) : `wxWinUIApplyTableHeaderStyle`
  (cellule d'en-tête : plate, texte de second rang, filet dessous et à
  droite, remplissage subtil au survol), `wxWinUIApplyTableCellStyle`
  (cellule de table : plate, sans coin, bordure dessinée par-dessus le
  contenu pour qu'un cadre plus épais ne déplace pas le texte),
  `wxWinUIApplySubtleButtonStyle` (bouton discret), `wxWinUICreateStyledListBox`
  et `wxWinUIApplyChromelessTextBoxStyle`. Tous passent par des
  dictionnaires de thème : ils suivent un changement de thème.

**Par composant :**

| Composant | Avant | Maintenant |
|---|---|---|
| En-têtes de `wxListCtrl`, `wxDataViewCtrl`, `wxTreeListCtrl`, `wxHeaderCtrl` | Boutons arrondis encadrés, titres coupés | En-tête de table WinUI : plat, 32 de haut, marge de 12, filets fins |
| `wxGrid` | Chaque cellule une boîte arrondie, étiquettes en gras sur bande grise, sélection en bleu plein, cellule courante cernée de blanc | Une table : cellules plates, filets fins, lignes de 32, texte centré à 12 du bord, étiquettes de second rang sur le fond des cellules, sélection teintée d'accent avec le texte dans sa couleur, cellule courante cernée d'accent |
| Sélection peinte par le moteur de rendu (`wxVListBox`, `wxHtmlListBox`, liste de `wxOwnerDrawnComboBox`) | Rectangle bleu plein, texte blanc | Élément de liste WinUI : remplissage subtil arrondi, barre d'accent au début, texte dans sa couleur |
| `wxComboBox(wxCB_SIMPLE)` | Liste de l'ancien `ListBox` : bloc bleu plein | Éléments au gabarit de `ListView`, cadre fin (`wxWinUICreateStyledListBox`) ; un exemple est ajouté à Showcase |
| `wxComboCtrl`, `wxOwnerDrawnComboBox` | Cadre classique, bouton flèche encadré, champ de texte avec son propre cadre dedans | Face d'un `ComboBox` WinUI peinte par `wxWinUIDrawComboBoxFace` : champ arrondi, trait plus fort dessous, chevron de la police d'icônes sans boîte ; le champ de texte intérieur n'a plus de cadre |
| `wxTextCtrl` créé avec `wxBORDER_NONE` | Montrait quand même le cadre du `TextBox` | Sans fond ni bordure : c'est le champ posé dans un autre contrôle (combo, cellule de table en édition, grille de propriétés) |
| Flèche de dépliage de `wxDataViewCtrl` / `wxTreeListCtrl` | Triangle dans un bouton carré | Chevron de `TreeView`, bouton discret |
| Colonnes de `wxDataViewCtrl` laissées à leur largeur par défaut | 80 ou 30 pixels quelle que soit l'échelle : « Enabl » | En DIP, et jamais moins que le titre ; la meilleure largeur compte la marge WinUI et la vraie case à cocher |
| Colonnes de `wxListCtrl` sans largeur | 80 pixels quelle que soit l'échelle | 80 DIP, et jamais moins que le titre |

**Glisser-déposer des colonnes.** `wxDataViewCtrl` : il fallait viser une
poignée « ⋮⋮ » de 22 de large à gauche de l'en-tête. On tire maintenant
l'en-tête lui-même, comme dans les listes de Windows et le `DataGrid` du
toolkit : un appui relâché sur place reste un clic (tri), au-delà de 6 de
déplacement l'en-tête suit le pointeur en transparence et un filet de 2
dans la couleur d'accent marque la place que prendrait la colonne (il
avait la forme d'un « I » de 9 de large). `wxHeaderCtrl` : la colonne tirée
est un remplissage subtil arrondi au lieu d'un contour, le repère un filet
de 2 au lieu d'une barre de 4. **Aucun de ces gestes n'a pu être essayé** :
le bureau invisible ne permet pas d'envoyer de souris à XAML.

**Tests du 23 novembre**, sur le dernier build du lot : 21 groupes passent
(les dix de l'hôte, `[winui-ribbon]`, `[winui-headerctrl]`,
`[winui-appearance]`, `[winui-dataview-columns]`, `[winui-dataview-reorder]`,
`[winui-dataview-churn]`, `[winui-font-dialog]`, `[winui-dialog-contract]`,
`[winui-picker-contract]`, `[winui-listmodel]`, `[winui-file-browsers]`,
`[winui-filectrl]`) et les démos démarrent sans plantage. Le test du repère
d'insertion (`winuidataviewcolumns.cpp`) est adapté au nouveau repère.
Trois échecs existent à l'identique sur les binaires figés du 22
(`F:\wxb\revue-snap`), tous à l'échelle 150 % de l'écran du jour, et ne
viennent donc pas de ce lot :

- `winuilistctrl.cpp:1388`, `HitTest` de droite à gauche ;
- `winuigridautomation.cpp:388`, l'éditeur natif d'une cellule n'est pas
  trouvé à sa création (le test s'arrête là, le reste de `[winui-grid]` ne
  tourne pas) ;
- `winuidataview.cpp:1502`, violation d'accès : `BeginEdit` →
  `EnsureVisible` → `ListView.UpdateLayout()` → mesure XAML, dans un test
  qui modifie les colonnes depuis une lecture du modèle pendant la mise en
  page. C'est le « plantage de DataView à 150 % » déjà noté.

### Étape 5, deuxième lot : fonds, sélection des listes, saisie numérique (23 novembre)

| Composant | Avant | Maintenant |
|---|---|---|
| Fond des zones de contenu (`wxSYS_COLOUR_WINDOW`, `wxSYS_COLOUR_LISTBOX`) : `wxListCtrl`, `wxGrid`, `wxListbook`, ce que wx peint sur « la couleur de fenêtre » | Un puits plus sombre que la page | La carte de WinUI (`CardBackgroundFillColorDefault` sur le fond de base), un peu plus claire que la page en thème sombre |
| Sélection de `wxListCtrl`, modes rapport et liste | Fond seul, presque invisible (la barre d'accent avait été retirée parce qu'elle couvrait le texte) | Barre d'accent de `ListView` ; le texte des éléments commence 12 plus loin, et l'élément du mode liste est élargi d'autant |
| `wxSpinCtrl`, `wxSpinCtrlDouble` | Étirés en hauteur par un sizer, texte collé en haut | Le `NumberBox` garde la hauteur d'un champ WinUI, au milieu |
| `wxSpinButton` | Deux barres sur toute la largeur de sa fenêtre | Une paire compacte de 32 de large (72 à l'horizontale), au début de sa fenêtre |
| `wxPropertyGrid` | — | Revu : déjà une présentation WinUI (catégories en bandeau, champs `TextBox`, `CheckBox`), rien à reprendre dans Showcase |

Vu en capture (Showcase, `listctrl.exe`). Tests : 18 groupes passent sur
ce build et les démos démarrent sans plantage ; `[winui-listctrl]` garde son
seul échec connu (`HitTest` de droite à gauche à 150 %). Les dix pages de
Showcase ont été revues en capture après ce lot : aucune ne montre plus de
composant d'aspect classique, hors les sélecteurs de date et d'heure.

Constat sans correction :
`wxDatePickerCtrl` et `wxTimePickerCtrl` au style par défaut sont trois
`NumberBox` séparés par « / » ou « : ». Ce n'est pas un défaut de fidélité :
wxMSW donne aussi le style à flèches par défaut (`src/msw/datectrl.cpp`
ligne 58) et un test le vérifie. Mais ce n'est pas un contrôle WinUI : le
contrôle fait pour cela est `DatePicker` (et `TimePicker`), le
`CalendarDatePicker` restant celui de `wxDP_DROPDOWN`, déjà en place. À
porter : deux fichiers de 2 000 lignes et leurs tests.

Autre constat : la liste de `wxListbook` ne montre pas sa page
sélectionnée dans Showcase, alors que la sélection s'affiche bien dans
`listctrl.exe`. Élucidé et corrigé au troisième lot.

### Étape 5, troisième lot : barre d'état, livres, date (23 novembre)

| Composant | Avant | Maintenant |
|---|---|---|
| `wxStatusBar` | Chaque champ dans une case encadrée, poignée en pointillés | Pas de case : un court filet entre deux champs ; le coin redimensionne toujours la fenêtre, mais rien n'y est dessiné |
| `wxListbook` | La page sélectionnée n'était pas marquée | Marquée. Cause : un `Selector` XAML qui n'est pas encore dans l'arbre oublie ce qu'on y sélectionne ; la projection de `wxListCtrl` republie la sélection quand la liste est chargée. Valait pour toute liste sélectionnée avant d'être affichée |
| `wxNotebook`, onglets à gauche, à droite et en bas | L'onglet sélectionné était un rectangle vide | Gabarit propre (`wxWinUIApplySideTabStyle`) : remplissage subtil, barre d'accent du côté opposé aux pages, contenu toujours montré |
| `wxDatePickerCtrl`, style à flèches | Trois `NumberBox` avec chacun ses deux flèches en ligne | Trois parties compactes : les flèches viennent à côté de la partie qui a le focus |

`wxTimePickerCtrl` garde ses flèches en ligne : la retenue d'une partie
à l'autre (59 minutes vers l'heure suivante) est câblée sur les deux
boutons en ligne du gabarit, que le placement compact ne crée pas, et
trois tests le vérifient. À rebrancher sur les boutons de la fenêtre
surgissante du placement compact pour l'aligner sur la date.

Reste des onglets à gauche : le libellé est au-dessus de l'icône au lieu
d'être à sa droite, la flèche de défilement est un triangle classique, et
les largeurs varient d'un onglet à l'autre.

**Reste à voir, composant par composant** (relevé `REVUE-VISUELLE.md`) :
`wxPropertyGrid` (8), AUI (10), `wxRichTextCtrl` (13), MDI (16),
`wxSashWindow` (17), sélecteurs de date et d'heure (18), `wxProgressDialog`
natif (19), `wxMiniFrame` (20), `wxListbook` (21), barre d'outils (23),
barre d'état (25), `wxSpinCtrl` (26), `wxCalendarCtrl` (27), `wxBusyInfo`
(29), et les onglets de `wxNotebook` à gauche et en bas (1). Le thème clair
n'a toujours pas été vu.

### Revue visuelle des composants (22 novembre)

Relevé complet, 34 constats classés, dans `REVUE-VISUELLE.md` (96 captures
gardées dans `F:\wxb\revue-2026-11-22`). Corrigé le jour même : le cadre
blanc des contrôles à bordure (ci-dessus), l'aperçu illisible de
`wxFontDialog`, la bannière illisible en thème sombre, et un plantage.

**Le plantage, parce que sa cause dépasse le cas vu.** Passer d'un
`wxListbook` à un autre livre après une peinture faisait une violation
d'accès dans `SyncAllNodes`. Une trace de la vie des canevas a montré que
la liste du `wxListbook` n'envoyait jamais son `wxEVT_DESTROY` à l'hôte.
Cause : `wxScrollHelperEvtHandler::ProcessEvent` (`src/generic/scrlwing.cpp`),
modifié en octobre pour survivre à la destruction de sa fenêtre, rendait la
main **avant** d'acheminer l'événement dès que la fenêtre était « en cours de
destruction » ; or c'est exactement l'état d'une fenêtre qui envoie
`wxEVT_DESTROY`. Aucune fenêtre défilante (`wxScrolled`, liste générique,
HTML, grille…) ne recevait donc plus cet événement, ni rien de ce qui lui
est envoyé pendant qu'un de ses parents est détruit : ni l'hôte, ni les
gestionnaires des applications. Le test n'arrête plus que si la fenêtre ou
le gestionnaire n'existent plus. `vscroll.cpp` n'avait pas le défaut.

`wxCollapsiblePane` a été porté le même soir, à la demande, sur
l'`Expander` de WinUI (`src/winui/collpane.cpp`, `wx/winui/collpane.h`) :
l'en-tête et le cadre sont ceux du contrôle, le panneau reste une fenêtre
wx, enfant de la première, posée sur la zone de contenu de l'`Expander`,
dont la hauteur lui est donnée par un élément vide. Les dimensions de
l'en-tête sont lues sur la partie `ExpanderHeader` du gabarit une fois
chargé (un `Expander` hors de l'arbre ne se mesure pas : zéro) ; d'ici là,
celles du style par défaut. Le contrôle générique reste disponible sous
son nom, `wxGenericCollapsiblePane`. Vu en capture replié et déplié
(`collpane.exe`). Limites : le contenu de l'`Expander` apparaît avec son
animation alors que le panneau wx apparaît d'un coup, et un clic sur le fond
du panneau va à l'`Expander`, pas à la fenêtre du panneau.

Restent « cassés » : les onglets de `wxNotebook` à gauche et en bas (des
`TabViewItem` isolés dont le contenu disparaît à l'état sélectionné) et la
disposition des petites icônes de `wxListCtrl`. Tous deux relèvent d'un
portage sur le vrai contrôle (étape 5).

### Reste de l'étape 0

- Synchronisation : fait à l'étape 1 pour les contrôles et l'empilement ;
  restent la région du pont et les scrollbars, recalculées à chaque passe.
- Souris : un seul mouvement livré par tour de boucle ; un seul
  `WM_NCHITTEST` ; `IsMouseInWindow` corrigé pour l'île.
- Clignotement : effacement et réparation par l'île, redessin à chaque
  changement de région.
- Dialogues : traité au quatrième lot pour les contrôles XAML ; reste le
  chemin du routeur de pointeur (étape 3).

### Référence pour les tests

`F:\wxbase-src` est une copie du dépôt dans l'état du commit de reprise
(`6a7bf38d6c`), sans aucune de mes modifications, et `F:\wxbase-dll` son
`test_gui.exe`. Un groupe en échec se relance dessus
(`tests.ps1 -Bin F:\wxbase-dll\libc_x64_dll`) pour savoir si l'échec est
une régression ou s'il était déjà là. Les binaires d'octobre
(`F:\wxwinui-pr26890-feedback-build`) sont plus anciens que ce code et ne
sont pas une référence valable.

Échecs déjà présents dans la référence, donc hérités :

- `winuidialogsession.cpp:512` (groupe `[winui-beta-dialogs]`) : après
  `WinUIPreferencesEditorContracts`, deux fenêtres propriétaires restaient
  enregistrées « en cours de destruction » et n'étaient jamais retirées.
  Corrigé au cinquième lot.
- `winuicombomodel.cpp:529` et `winuiitemmodel.cpp:2284` : exigent un client
  d'accessibilité externe ou un vrai clavier, impossibles sur le bureau
  invisible.
- `[winui-grid]` : le programme de test s'arrête brutalement après
  `winuigridautomation.cpp:388` (éditeur natif de cellule introuvable).
- `[winui-propgrid]` : `winuipropgrid.cpp:740` et
  `winuipropgrideditors.cpp:170`.

- Groupe `[lifetime]` : `winuitextpeer.cpp:2898` et
  `winuinotebook.cpp:874`.

Échecs qui n'apparaissent qu'à 150 % d'échelle d'affichage, identiques sur
la référence (constaté le 22 novembre, quand la machine est passée d'un
écran 2560×1440 à 100 % à un écran virtuel 3200×2000 à 150 % au milieu
d'une série de tests) :

- `[winui-toolbar]` : `winuitoolbar.cpp:697` et suivantes (taille des
  images d'outils).
- `[winui-ribbon]` : `winuiribbon.cpp:3637` et `3753` (un groupe reste
  replié alors que la largeur suffit).
- `[winui]` : `winuidataview.cpp:677`, puis violation d'accès dans le test
  de la ligne `1499`.
- `[winui-listctrl]` : `winuilistctrl.cpp:1382` (test de pointage sur le
  texte d'une ligne).

Ce sont de vrais défauts du portage à 150 %, à traiter avec les composants
(étape 5). Les outils `drive.ps1` et `revue.ps1` ne tiennent pas compte de
l'échelle : à 150 % leurs images ne montrent que le coin supérieur gauche
de la fenêtre.

Non expliqué : un débordement de pile est survenu une fois, pendant le
groupe `[winui-itemmodel]`, dans le test du raccourci presse-papiers (qui
échoue déjà ici faute de vrai clavier). Cinq relances ne l'ont pas
reproduit.

### Outils (dans `plans/reprise-2026-11/outils`)

Tous lancent les programmes sur un bureau Windows invisible : rien ne
s'affiche et aucune entrée n'est injectée.

- `smoke.ps1` : démarre chaque démo et vérifie qu'elle reste en vie.
- `rundesk.ps1` : lance `test_gui.exe` avec un filtre.
- `resizebench.ps1` : 300 redimensionnements chronométrés.
- `profile.ps1` + `stacks.py` : piles d'appels échantillonnées par `cdb`,
  un lancement du débogueur par relevé (fiable depuis le 22 novembre ; les
  profils antérieurs sont faux, voir « Audit du redimensionnement, suite »).
- `capture.ps1` : image de la fenêtre principale d'une démo. Le rendu XAML
  est bien capturé ; Mica ne l'est pas (bureau non composé).
- `build.ps1` : compile ; si une démo ouverte verrouille une DLL, celle-ci
  est mise de côté (renommée) au lieu de faire échouer l'édition de liens.
- `tests.ps1` : lance une liste de groupes de `test_gui.exe` et donne une
  ligne de résultat par groupe. Les groupes hérités de wxWidgets qui pilotent
  la souris ou le clavier (`[button]`, `[spinctrl]`, `[focus]`, `[choice]`…,
  via `wxUIActionSimulator`) échouent sur le bureau invisible, où Windows
  refuse l'injection d'entrée : ne lancer que les groupes du portage.
- `revue.ps1` : images des dix pages de Showcase et de ses dialogues dans un
  dossier ; deux dossiers se comparent avant / après.
- `drive.ps1` : petit langage de script pour redimensionner, envoyer des
  messages (`WM_COMMAND`, `WM_VSCROLL`…), lire capture et focus, et prendre
  des images (`shot`, `shotall`). À lancer avec `powershell.exe` (5.1).
  Depuis le 23 novembre il tient compte de l'échelle de chaque écran : les
  tailles sont en pixels réels et l'image d'une fenêtre est entière sur un
  écran à 150 % (avant, elle n'en montrait que le coin haut gauche).
  `childt <nom> <parent> <texte>` désigne une fenêtre visible par son nom.
  Après un `size`, capturer deux fois et garder la seconde image. Les messages souris
  postés n'atteignent **pas** les contrôles XAML : l'île ne lit pas la souris
  par ce canal. Les gestes à la souris ne peuvent donc pas être simulés ici.

## 7. Risques

- **Surfaces de peinture** : coût du transfert des grandes zones repeintes à
  chaque image, et qualité du texte GDI sur une surface à canal alpha. À
  mesurer sur un prototype avant de généraliser.
- **Géométrie synchrone** : certains réglages XAML rappellent le code wx de
  façon synchrone (perte de focus à la disparition d'un élément). La
  position et la taille ne le font pas ; la visibilité demande de la prudence.
- **Étendue du contrat Win32** : `SysListView32` et `SysTreeView32` ont
  chacun plus d'une centaine de messages. On commence par ceux que wxMSW
  utilise lui-même, qui sont les plus plausibles dans le code des applis.
- **Délai** : c'est un travail de plusieurs semaines. L'étape 0 est la seule
  dont l'effet est immédiat.

## Étape 5, quatrième lot : la barre d'outils (2026-11-23)

`wxToolBar` est récrit sur le contrôle WinUI fait pour cela, `CommandBar`
(`src/winui/toolbar.cpp`, 1 577 lignes au lieu de 8 731 ;
`include/wx/winui/toolbar.h`). L'ancienne version, qui assemblait elle-même
des boutons et son propre menu de débordement, est conservée hors du dépôt
dans `F:\wxbnciens	oolbar-2026-11-23`.

- un outil = `AppBarButton`, un outil cochable ou radio = `AppBarToggleButton`,
  un séparateur = `AppBarSeparator` ;
- un outil à menu (`wxITEM_DROPDOWN`) est suivi d'un petit bouton à chevron ;
- un contrôle (`AddControl`) garde sa fenêtre, posée sur une place réservée,
  à la taille que le programme lui a donnée (comme wxMSW) ;
- `wxTB_TEXT` : libellés à droite des icônes dans une barre horizontale,
  sous les icônes dans une barre verticale ;
- le premier espace extensible envoie les outils suivants au bout de la
  barre (une seconde `CommandBar`) ;
- ce qui ne tient pas passe dans le menu « ... » de la `CommandBar` ;
  `SetRows` n'a plus d'effet ;
- un outil ajouté après `Realize()` apparaît sans second appel ;
- la taille idéale est calculée (XAML ne la connaît qu'après affichage) et
  réserve la largeur du bouton « ... ».

Tests récrits sur le comportement : `tests/controls/winuitoolbar.cpp`
(9 cas), `winuitoolbook.cpp` et `toolbooktest.cpp` adaptés (les trois cas qui
faisaient échouer exprès l'ancienne barre sont retirés).

Vu en capture : Showcase (barre de la fenêtre, page Advanced) et
`toolbar.exe`. Pas vu : le menu « ... » ouvert, le menu d'un outil à chevron,
le thème clair, une barre verticale plus haute que sa fenêtre (elle est
coupée, sans défilement). Pas fait : libellé d'un contrôle (`AddControl`
avec texte), libellé centré à côté d'une icône de plus de 40.

## Étape 5, cinquième lot : MDI, progression, petits composants (2026-11-23)

- **MDI** : `wxUSE_GENERIC_MDI_AS_NATIVE` vaut 1 pour wxWinUI
  (`include/wx/mdi.h`). `wxMDIParentFrame` est donc l'implémentation
  générique à onglets, dont le carnet (`wxNotebook`) est un `TabView` ; le
  cadre a la barre de menus WinUI de tout cadre, où le menu de l'enfant
  actif remplace le sien. `src/msw/mdi.cpp` n'est plus compilé,
  `src/generic/mdig.cpp` l'est. Deux corrections dans le générique : redonner
  le même menu « Fenêtre » ne le détruit plus (plantage de mdi.exe), et
  « Suivant » / « Précédent » ont les identifiants standard
  `wxID_MDI_WINDOW_NEXT` / `PREV`. Pas de cascade ni de mosaïque.
- **Onglets fermables** : `wxNotebook::WinUISetCloseHandler()` (propre à
  wxWinUI) donne aux onglets le bouton de fermeture du `TabView` ; le carnet
  ne retire rien lui-même, il appelle la fonction donnée. Le MDI s'en sert
  pour fermer le document (`Close()`, que l'enfant peut refuser).
- **wxProgressDialog** : le dialogue générique (contrôles WinUI) au lieu du
  `TaskDialog` de Win32 (`include/wx/progdlg.h`, `src/msw/progdlg.cpp`).
- **wxBusyInfo** : fenêtre sans cadre à coins arrondis
  (`wxWinUIRoundWindowCorners()`, attribut DWM ; sans effet avant
  Windows 11).
- **wxSashWindow** : plus de relief ; un filet du thème au milieu de la place
  de la poignée.
- **Astuce du jour** : plus d'Arial imposé.

Vu en capture : mdi.exe avec deux documents, sashtest.exe, dialogs.exe
(progression, astuce). Pas vu : les coins arrondis de wxBusyInfo (une capture
de fenêtre ne montre pas son contour), le clic sur le bouton de fermeture
d'un onglet. `[winui-v0-mixed]` échoue de la même façon sur les binaires de
référence (`F:\wxb\revue-snap`) : pas de défilement à 150 %, sans rapport.

### Suite du cinquième lot (2026-11-23, après-midi)

- **AUI** (`src/aui/dockart.cpp`, branches wxWinUI de
  `wxAuiDefaultDockArt`) : couleurs prises au thème (plus de dégradé ni de
  titre en couleur d'accent), titre dans la police des contrôles, bande de
  28, boutons de 24 dont le glyphe est au milieu d'un carré arrondi qui ne se
  montre qu'au survol, panneau actif marqué par un fond discret et une barre
  d'accent, poignée en deux rangées de points. Là où la fenêtre montre le
  fond du système, le fond est peint en noir, comme le fait déjà l'art des
  barres (`wxWinUIWindowUsesBackdrop`). `src/aui/barartwinui.cpp` : la flèche
  d'un outil à menu est un chevron fin et le bouton de débordement trois
  points, en SVG.
- **MDI, robustesse** : détruire le cadre sans l'avoir fermé détruit
  d'abord les documents, tant que leur carnet est entier (plantage sinon) ;
  un document sans barre de menus montre celle du cadre et non celle du
  document précédent.
- **Barre d'outils** : sa racine XAML a le rôle « barre d'outils » pour les
  lecteurs d'écran (`wxWinUICreateAccessibleGrid`), comme l'attendait
  `HostState::CompositeRootsHaveSemanticAutomationPeers`.
- **Tests** : `tests/controls/winuimdi.cpp` est récrit pour le MDI à onglets
  (menus, titre, fermeture par le bouton de l'onglet avec refus possible).
  L'ancien fichier testait le MDI Win32, qui n'est plus compilé ; il est
  gardé hors du dépôt dans `F:\wxb\anciens\mdi-2026-11-23`. Deux attentes
  de `winuipropgridpreview.cpp` suivent le passage des parties de date en
  flèches compactes (lot 3).

Groupes passés sur ce build : `[winui-toolbar]`, `[toolbook]`,
`[winui-mdi]`, `[winui-notebook]`, `[winui-aui]`, `[winui-accessibility]`,
`[winui-ribbon]`, `[winui-range-date]`, `[winui-appearance]`, `[HostState]`,
`[HostLifecycle]`, `[winui-root-input]`, `[winui-window-scrollbar]`,
`[winui-cursor]`, `[winui-chrome-feedback]`, `[winui-v0-supported]`,
`[winui-input-state]`, `[winui-zorder]`. Échecs identiques à la référence :
`[xrc]` (un cas, pas de défilement à 150 %) et `[winui-propgrid]` (deux cas).

## Étape 5, sixième lot : tables et grille de propriétés d'après WinUI.TableView (2026-11-23)

Référence de style donnée par l'utilisateur : le contrôle
[WinUI.TableView](https://github.com/w-ahmad/WinUI.TableView), dont
l'application d'exemple a été capturée (`captures/reference-tableview.png`)
et dont les thèmes sont gardés dans `F:\wxb\ref-tableview`. Mesures
relevées, en DIP : en-tête de 32 sur une bande `SubtleFillColorSecondary`,
texte en demi-gras ; lignes de 40 (`ListViewItemMinHeight`) ; filets de 1 en
`ControlStrokeColorDefault` (plus clairs que le fond en thème sombre) ;
cadre du même pinceau, coins de 4 ; texte des cellules à 12 du bord ;
éditeur de 32 dans sa ligne ; cellule courante entourée de
`FocusStrokeColorOuter`, coins de 4.

- **Aides communes** (`include/wx/winui/private/appearance.h`,
  `themecolours.h`) : `wxWinUICreateTableHeaderBand()`,
  `wxWinUIApplyTableHeaderStyle()` / `...HeaderFont()`,
  `wxWinUICreateTableCellBorder()`, `wxWinUIApplyTableCellValueStyle()`
  (un contrôle qui montre la valeur d'une cellule sans fond ni cadre),
  constantes `wxWINUI_TABLE_*`.
- **wxListCtrl (rapport), wxDataViewCtrl, wxTreeListCtrl, wxGrid,
  wxHeaderCtrl, wxFileCtrl** : en-tête, filets, hauteur de ligne et cadre
  ci-dessus. XAML ne rogne pas ce qui est dans un cadre arrondi, et un clip
  de composition posé sur un élément fait planter XAML (0xC000027B) : c'est
  la bande d'en-tête et le fond de la racine qui portent les coins.
  `wxFileCtrl` montre une table de détails par défaut.
- **Cadre sans bordure Win32** : `wxWinUIRequestWindowFrame(window)`
  (`tlwhostmsw.h`) fait dessiner par XAML le cadre d'une fenêtre qui n'a pas
  de bordure Win32. Règle : une fenêtre dont le contenu XAML remplit la
  fenêtre (`wxTreeCtrl`, `wxPropertyGrid`, `wxHeaderCtrl`) n'a **pas** de
  bordure Win32, sinon son contenu XAML part du bord de la fenêtre alors que
  ses coordonnées wx (enfants, souris) partent de l'intérieur de la
  bordure : 2 pixels d'écart. `wxWinUIWindowHasRequestedFrame()` dit si le
  cadre a été demandé.
- **wxPropertyGrid** (`src/propgrid/propgrid.cpp`, `editors.cpp`,
  `propgridwinui.cpp`, mesures dans `propgridwinui.h`) :
  - lignes de 40 ; l'éditeur d'une valeur fait 32 et est au milieu de sa
    ligne (`TryGetEditorWidgetRect`), le champ de texte a son cadre de
    `TextBox` ;
  - la cellule montre la valeur là où son éditeur vient : même retrait
    (2), même début de texte (13), même flèche pour les listes ; rien ne
    bouge quand on entre en édition (mesuré en capture sur une liste et un
    texte) ;
  - marge des flèches de repli : bouton de 24 avec 4 de part et d'autre,
    16 de plus par niveau, flèche à 20 du texte comme dans un `TreeView` ;
  - la couleur d'une propriété de couleur est une pastille de 16 à coins
    arrondis, au repos (image XAML) et dans la liste déroulante d'édition
    (`wxPGWinUIPaintSwatch`, dessinée dans `OnComboItemPaint`) ;
  - une ligne a un seul fond arrondi partagé par ses cellules (survol,
    sélection), les catégories une bande arrondie ;
  - zone de description du gestionnaire : cadre arrondi du thème, texte à
    12 du bord (`src/propgrid/manager.cpp`).
- **Listes déroulantes génériques** (`wxComboCtrl`,
  `wxOwnerDrawnComboBox`) : la place du chevron est de 40, son milieu à 20
  de la fin comme dans un `ComboBox`, glyphe de 12 DIP
  (`wxWinUIDrawIconGlyph` dans `src/winui/renderer.cpp`).
- **Flèches du moteur de rendu** : `DrawTreeItemButton` (arbre générique)
  et `DrawCollapseButton` (`wxCollapsibleHeaderCtrl`) dessinent le chevron
  de la police d'icônes au lieu de traits ; l'expandeur de
  `wxDataViewCtrl` est un bouton de 24 centré.
- **Trou dans l'île (bogue corrigé)** : `RebuildBridgeRegion`
  (`src/winui/hostgeometry.cpp`) découpe l'île là où une fenêtre a une
  barre de défilement native. Le parcours descendait dans les cadres et
  dialogues enfants : la zone de texte d'une fenêtre de journal
  (`wxLogWindow`) trouait le cadre principal, à l'endroit où l'autre
  fenêtre se trouvait à l'écran. Le parcours saute maintenant les fenêtres
  de premier niveau.

Diagnostic ajouté : `WX_WINUI_DUMP_AT=<x>,<y>,<fichier>` écrit, cinq
secondes après la création de l'île, les éléments XAML sous ce point de la
zone cliente (pixels), du premier plan vers le fond, avec place, taille et
fond. `WX_WINUI_INPUT_LOG` trace aussi chaque découpe (`bridge CUT`).
`drive.ps1` : `treeall` (fenêtres cachées comprises), `childx` (titre
exact), `hide` / `unhide`, `place x y l h`, `scrollbars`, `region`.

Piège des captures : ce qui dépasse de l'écran n'est jamais peint par GDI.
Une fenêtre de test plus large que l'écran montre donc ses parties peintes
(surfaces de peinture) coupées au bord de l'écran, alors que XAML est
entier. Placer la fenêtre avec `place` dans l'écran. Une session Windows
déconnectée ne rend pas XAML du tout (captures vides, échelle 100 %).

Deux régressions de ce lot trouvées par les tests et corrigées dans
`src/winui/dataview.cpp` : la bande d'en-tête avait pris la première place
dans la racine, où l'en-tête est attendu (elle est maintenant après lui,
sous lui par son indice Z) ; avec la marge de 12 des cellules, le contenu
d'une cellule étroite dépassait de 13 sa colonne (la largeur maximale du
contenu tient compte des deux marges et du filet).

Tests sur ce build (bureau invisible, 100 %) : `[winui-dataview]` 42 cas,
`[winui-listctrl]`, `[winui-headerctrl]`, `[winui-treemodel]`,
`[winui-file-browsers]`, `[winui-toolbar]`, `[winui-toolbook]`,
`[winui-mdi]`, `[winui-aui]`, `[winui-appearance]`, `[winui-zorder]`,
`[winui-window-scrollbar]` passent. `[winui-propgrid]` : 33 cas sur 35, les
deux échecs sont ceux de la référence
(`NativePaintClearsOnlyInvalidatedLegacyBacking`,
`EditableNativeTextUsesPropertyCommit`). `[winui-grid]` s'arrête toujours
sur `winuigridautomation.cpp:388`, comme avant ce lot. Session Windows
déconnectée pendant ces passes : les cas `WinUIMedia::*` de `[HostState]`
et `[HostLifecycle]` (lecture d'un média) échouent alors, sur les binaires
de référence aussi ; à rejouer session connectée.

Vu en capture (100 %, thème sombre) : propgriddemo au repos et en édition
d'une couleur, d'une liste et d'un texte ; page « Data views » de Showcase.
Pas vu : thème clair, 150 %, la case à cocher d'une propriété booléenne
après son alignement sur les textes (écrit, session déconnectée), la liste
déroulante ouverte d'une propriété de couleur.

## Étape 5, septième lot : retours sur Showcase du 23 au soir (2026-11-23)

Liste donnée par l'utilisateur après essai de Showcase à l'écran. Captures
de départ : `F:\wxb\revue-2026-11-23-soir\winui`.

- **wxCalendarCtrl, bande sombre** : le gabarit d'origine du `CalendarView`
  donne à la ligne des noms de jours (`WeekDayNames`) le fond du contrôle
  une seconde fois. Ce fond est translucide : empilé sur lui-même il fait
  une bande plus sombre, presque noire sur Mica, sous laquelle les semaines
  paraissent coupées pendant qu'elles défilent. Le fond de cette ligne est
  retiré quand le gabarit est lié (`BindMonthViewport`, `calctrl.cpp`).
- **wxInfoBar** : un bouton unique devient l'`ActionButton` de l'`InfoBar`,
  sur la ligne du message comme dans WinUI ; plusieurs boutons restent en
  rangée dans le contenu, sous le message (`infobar.cpp`).
- **wxCollapsiblePane** : `MinHeight(48)` en valeur locale sur l'`Expander`.
  Un contrôle confié à une fenêtre wx perd les minima de son style
  (`ctrlhost.cpp`), et l'en-tête lit sa hauteur là : il n'avait plus que la
  hauteur de son chevron, 32.
- **wxListbook** : la liste des pages est une liste en mode rapport sans
  en-tête. Sa largeur idéale était les 50 caractères arbitraires de
  `wxListCtrlBase` (400 px) ; elle est maintenant celle de ses colonnes, et
  le Listbook ajuste la colonne aux noms des pages. Une liste rapport sans
  en-tête n'a plus les filets d'un tableau (sauf `wxLC_HRULES` /
  `wxLC_VRULES`).
- **wxFileCtrl** : de nouveau une liste de noms (`wxLC_LIST`) comme dans
  wxMSW ; le tableau de détails par défaut du sixième lot n'était pas ce qui
  était demandé.
- **wxRibbonBar** : avec une seule page et sans
  `wxRIBBON_BAR_ALWAYS_SHOW_TABS`, pas de rangée d'onglets, donc ni bouton
  de repli ni bouton d'aide, comme dans wxMSW. L'art WinUI testait `<= 1`
  alors que la classe de base renvoie 2 dans ce cas.
- **wxComboCtrl, wxOwnerDrawnComboBox** : hauteur d'un `ComboBox` (32),
  fond calculé sur la couleur de base de la fenêtre (il était plus clair que
  celui du `ComboBox` voisin), texte du champ à la hauteur de celui d'un
  `ComboBox`. La fenêtre du menu a les coins d'un flyout
  (`wxWinUIRoundWindowCorners`), la liste des éléments de 36 avec leur
  texte à 12, sans cadre, sur le fond d'un flyout
  (`wxWinUIThemeColour::FlyoutFill`).
- **Showcase** : le `wxComboBox(wxCB_SIMPLE)` de la page des listes est
  retiré (jugé de trop).
- **Boutons sans bordure** : un `wxButton` ou `wxBitmapButton` créé avec
  `wxBORDER_NONE` est le bouton discret de WinUI (pas de fond avant le
  survol), l'équivalent du bouton plat de wxMSW (`button.cpp`).
- **wxEditableListBox** : ses commandes sont donc des boutons discrets ; sa
  liste a le cadre d'un contrôle.
- **wxSpinButton** : les deux flèches côte à côte dans un champ arrondi de
  64 x 32, comme celles d'un `NumberBox`, au lieu de deux lamelles de 14
  empilées.
- **Dialogues, deux tons** (comme `ContentDialog`) :
  - dialogues WinUI en fenêtre (`dlgpresenter.cpp` : message, saisie de
    texte, couleurs) : contenu sur `LayerFillColorAltBrush` avec un filet
    `CardStrokeColorDefaultBrush`, boutons sur le fond du dialogue ;
  - dialogues wx à sizer (`dlgcmn.cpp`, `wxWinUIUpdateDialogTones`) : quand
    le dernier élément du sizer vertical du dialogue contient les boutons
    standard, ce qui est au-dessus reçoit le ton clair, par une partie de
    fenêtre `Ground` (Z -3) de l'hôte ;
  - `CreateSeparatedSizer()` ne crée plus de `wxStaticLine` (certains
    dialogues en avaient une, d'autres non) mais garde la place du filet.
- **Dialogues en superposition** : l'option existait déjà,
  `wxWinUISetDialogPresentation(wxWinUIDialogPresentation::Overlay)` ou
  `WX_WINUI_DIALOG_OVERLAY=1` : un vrai `ContentDialog` dans la fenêtre
  parente. Vérifié en capture sur wxMessageDialog.
- **Pastilles de couleur** : `wxWinUICreateColourSwatch()` (coins de 4,
  contour fin), utilisée par le dialogue de polices.
- **Dialogue de couleurs** : l'`Expander` des couleurs personnalisées
  prend la largeur du sélecteur.

Outils : `drive.ps1` sait agir sur les contrôles XAML par UI Automation
depuis le bureau invisible (`uia`, `invoke`, `uiascroll`), ce que les
messages souris postés ne permettent pas.

Reste de cette liste, non fait :
- sélecteurs de date et d'heure sur les `DatePicker` / `TimePicker` natifs
  (`wxDatePickerCtrl` en mode roulette, `wxTimePickerCtrl`) ;
- délai entre l'ouverture d'une fenêtre de dialogue et l'affichage de son
  contenu ;
- dialogue de couleurs : rien d'autre que l'`Expander` ;
- la mise en évidence de l'élément choisi dans la liste d'un
  `wxOwnerDrawnComboBox` n'a pas été vue en capture.

Suite du septième lot, retours donnés pendant la passe :
- **wxInfoBar** : le bouton d'action est à droite (l'`InfoBar` donne à son
  dernier élément toute la largeur restante).
- **Dialogues, deux tons** : le fond est opaque comme celui d'un
  `ContentDialog` (base sous les boutons, calque plus clair sous le contenu)
  et la barre de titre prend le ton du contenu
  (`wxWinUISetDialogCaptionTone`, attribut DWM `DWMWA_CAPTION_COLOR`). La
  couleur de la barre de titre ne se voit pas dans les captures du bureau
  invisible : à confirmer à l'écran.
- **wxCalendarCtrl, noms des jours invisibles chez l'utilisateur** : non
  reproduit ici (ils s'affichent sur le bureau invisible). Une trace écrit
  ce que contient la ligne des noms dans le journal `WX_WINUI_INPUT_LOG`
  (`calendar WEEKDAYS`), à la liaison du gabarit puis à chaque fin de
  défilement.
- **wxRibbonBar** : le repositionnement de la page après un changement de
  hauteur des onglets vérifie que la page courante existe encore (échec de
  `PageModelAndDisplayModes` apparu avec la correction des onglets).

Encore à faire, demandé le même soir : défilement au pixel (et non par
ligne) des listes génériques, dont celle de wxEditableListBox.

Tests après ce lot : `[winui-ribbon]`, `[listbook]`, `[winui-listctrl]`,
`[winui-file-browsers]`, `[winui-dataview]`, `[winui-appearance]`,
`[winui-zorder]`, `[HostState]` (104 cas), `[HostLifecycle]` (119 cas)
passent ; `[winui-propgrid]` et `[winui-combo]` gardent leurs deux échecs
de référence chacun.

## Étape 5, huitième lot : sélecteurs natifs, combos génériques, dialogues intégrés (2026-11-23 et 24)

Suite des retours sur Showcase, puis de la liste de contrôle donnée par
l'utilisateur le 24. Captures : `plans/reprise-2026-11/captures/lot8-*.png`.

- **wxTimePickerCtrl** (`timectrl.cpp`, réécrit : 2 250 lignes devenues 600) :
  le `TimePicker` de WinUI pour les heures, les minutes et la demi-journée,
  et un `NumberBox` à côté pour les secondes, que le contrôle natif n'a pas.
  Largeur minimale du bouton ramenée de 242 à 120 (24 h) ou 180 (12 h),
  posée sur la partie `FlyoutButton` une fois le gabarit appliqué ; hauteur
  32 par la ressource `TimePickerHostPadding`.
- **wxDatePickerCtrl** sans `wxDP_DROPDOWN` : le `DatePicker` de WinUI à la
  place des trois `NumberBox`. Le `CalendarDatePicker` caché reste le
  porteur de la date et de la plage. Les listes du `DatePicker` vont, sans
  plage du programme, de cent ans avant à cent ans après aujourd'hui (toute
  l'étendue du calendrier ferait des listes de milliers de lignes) ; les
  jours de la plage sont appliqués au choix (`OnPeerDateChanged`). Avec
  `wxDP_ALLOWNONE`, un bouton « Clear » à côté.
- **wxGauge vertical** : la barre tournée est dans un `Canvas`, un `Grid`
  la rognait à sa propre largeur avant la rotation.
- **wxComboCtrl, wxOwnerDrawnComboBox** : leur face n'est plus peinte par
  wx. C'est un `ComboBox` de WinUI (`wxWinUISetComboBoxFace`,
  `renderer.cpp`), partie `Ground` sous ce que le contrôle peint et sous
  son champ de texte, qui ne prend aucune entrée et que le contrôle met
  dans son état (`Normal`, `PointerOver`, `Pressed`, `Focused`). Avec le
  focus dans le champ de texte, un `Border` aux ressources d'un champ de
  WinUI (`TextControlBackgroundFocused`, `TextControlBorderBrushFocused`)
  derrière le `ComboBox` rendu transparent. Le champ interne
  `EditableText` d'un `ComboBox` éditable n'est jamais mis en page tant
  que le contrôle n'a pas lui-même le focus : inutilisable ici.
- **Menu de ces combos** : marges du menu d'un `ComboBox` (4 au-dessus et
  en dessous, éléments de 36 avec 2 de part et d'autre, texte à 16),
  élément choisi sur un fond discret avec la barre d'accent, élément sous
  le pointeur sur le fond seul. Fermé par tout appui du pointeur dans un
  îlot (`wxWinUIDismissPopupOnOutsidePress`, appelé par `OnRootPointer`) :
  un clic sur un contrôle XAML n'atteint aucune fenêtre wx, donc pas le
  crochet de wxMSW qui ferme les popups.
- **wxVListBox, wxHtmlListBox** : élément sous le pointeur, sélection
  dessinée par le moteur de rendu (fond arrondi, barre d'accent), marges
  12 et 8 d'un élément de liste WinUI. `wxRichTextStyleListCtrl` n'a plus
  qu'un cadre, autour de la liste.
- **Dialogues, deux tons** : la ligne laisse au-dessus des boutons autant
  de place qu'il y en a dessous ; juste sous le contenu quand la mise en
  page n'en laisse pas autant. Une rangée de boutons vide (dialogue de
  progression sans annulation) : tout le dialogue est contenu.
- **Mode intégré** (`wxWinUIDialogPresentation::Overlay`) :
  - `wxNumberEntryDialog` : un `ContentDialog` avec un `NumberBox`
    (`wxWinUIShowNumberEntryOverlay`), comme le dialogue de texte ;
  - Tab reste dans le `ContentDialog` : l'îlot prend le focus Windows pour
    la durée du dialogue et l'arbitre de Tab s'efface
    (`wxWinUIHasDialogOverlay`) ;
  - tout autre `wxDialog` (police, progression, choix, réarrangement,
    assistant, rechercher) : il est fait de contrôles wx avec chacun son
    HWND, qu'un `ContentDialog` ne peut pas contenir. Il reste une fenêtre
    à lui, sans barre de titre ni cadre, aux coins et à l'ombre d'un
    `ContentDialog`, centrée sur son propriétaire et le suivant, son titre
    en grand au-dessus du contenu, un voile sur le propriétaire quand
    celui-ci est désactivé (`wxWinUIBeginDialogCard`, `ctrlhost.cpp`). Le
    titre et les marges sont hors de la zone cliente
    (`wxDialog::GetClientAreaOrigin`, `DoGetClientSize`) : la mise en page
    du dialogue n'est pas touchée.
- **Ouverture des dialogues** : le contenu XAML d'un dialogue recevait ses
  gabarits et sa mise en page après l'affichage de la fenêtre (environ
  170 ms mesurés à la trace). `wxWinUIBeginShowWithContent` fait cette
  première mise en page avant l'affichage et masque la fenêtre (DWM cloak)
  jusqu'à la deuxième image dessinée, ou 150 ms.
- **Liste générique en mode rapport** : défilement au pixel (unité
  verticale de 1), molette suivie au pixel pour un pavé tactile et animée
  pour un cran de molette, flèches de la barre par ligne, clavier par
  élément.

Outils : `drive.ps1` a `bindfocus` (la fenêtre qui a le focus, pour lui
poster des touches).

Tests au 24 novembre, écran à 150 % : `[winui-range-date]` (46),
`[HostState]` (104), `[HostLifecycle]`, `[listbook]`,
`[winui-file-browsers]`, `[winui-appearance]`, `[winui-zorder]`,
`[winui-ribbon]` passent. Échecs présents aussi avec les binaires de
référence à cette échelle, donc antérieurs : `[winui-listctrl]`
(`ListAndIconModesUseNativeItemContainersAndFitText`), `[winui-dataview]`
(arrêt à `winuidataview.cpp:1502`), les deux de `[winui-combo]`,
`winuipropgrid.cpp:741` et `winuipropgrideditors.cpp:170`.

Reste de ce lot :
- **Grille de propriétés** : l'aperçu passif d'une date sans
  `wxDP_DROPDOWN` (`PropertyDateSpinPreview`, `propgridwinui.cpp`) montre
  encore trois `NumberBox`, alors que l'éditeur actif est maintenant un
  `DatePicker`. Le test `DateSpinPreviewHasRealCalendarFieldsAndStableGeometry`
  échoue pour cette raison.
- Non vus à l'écran, seulement en capture hors écran ou pas du tout : les
  coins, l'ombre et le voile des dialogues intégrés, le survol des combos
  et des listes, la fluidité du défilement, l'effet du masquage à
  l'ouverture des dialogues.
- Marges de 24 des dialogues en fenêtre : non faites (elles viennent des
  sizers de chaque dialogue) ; seuls les dialogues intégrés les ont.
- Le temps total d'ouverture d'un dialogue n'est pas réduit, seulement
  l'apparition d'une fenêtre vide.

## Étape 5, neuvième lot : dialogues uniformes, liste native des combos, en-tête, défilement au pixel (2026-11-24)

**Dialogues.** `wxWinUINormalizeDialogCommands()` (`dlgcmn.cpp`) donne à
tous les dialogues, en fenêtre comme intégrés, la même rangée de boutons :
24 DIP entre le trait et les boutons, 24 sous eux, par deux espaceurs
insérés dans le sizer du dialogue (identifiants -31873 et -31874).
Les dialogues à présentateur (message, texte, nombre, couleur) sont des
`ContentDialog` ; les autres dialogues modaux deviennent des « cartes »
(`wxWinUIBeginDialogCard()`, `ctrlhost.cpp`) : fenêtre sans cadre, titre
dans la partie Ground, voile (`WindowPartKind::Smoke`) sur le propriétaire.
Un dialogue non modal reste une fenêtre. `wxProgressDialog` est le dialogue
générique : le TaskDialog de Win32 n'est plus compilé sous wxWinUI.

**Combo dessiné par le programme.** Un `wxOwnerDrawnComboBox` exact ouvre
la liste du vrai `ComboBox` XAML de sa face (`wxWinUIOpenComboFaceList()`,
`renderer.cpp`). Ses sous-classes et un `wxComboCtrl` à popup propre
gardent la fenêtre popup de wx.

**wxHeaderCtrl.** Trois défauts, dont un de logique :
- `wxHeaderCtrl::DoUpdate()` arrêtait tout glissement en cours. Or un
  programme qui reçoit `wxEVT_HEADER_RESIZING` donne sa largeur à la colonne
  et appelle `UpdateColumn()` (wxGrid, wxPropertyGridManager) : le
  redimensionnement s'arrêtait au premier mouvement. `DoUpdate()` ne touche
  plus à un redimensionnement en cours.
- Les boutons des colonnes sont ceux de XAML mais le pointeur ne les
  atteint plus (`IsHitTestVisible(false)`) : le contrôle reçoit la souris en
  entier et donne leur état aux boutons (`PointerOver`, `Pressed`) par
  `VisualStateManager::GoToState`, comme la face des combos. Un bouton qui
  capturait le pointeur à l'appui le disputait au contrôle qui glissait.
- La colonne déplacée est montrée : un double de son bouton (titre, image,
  flèche de tri), levé et translucide, suit le pointeur ; le bouton d'origine
  est estompé ; le trait d'accent marque l'endroit du dépôt. Quand le
  programme ne donne sa largeur à la colonne qu'au relâchement, un trait
  suit le bord (`ShowResizeLine()`).

**Capture de la souris par une fenêtre montrée par XAML.** C'était la
cause du glissement « collant » de l'en-tête : rien ne suivait bouton tenu,
un simple clic laissait la colonne suivre le pointeur, et il fallait cliquer
une seconde fois pour l'arrêter. La séquence réelle, rejouée dans les
tests :
1. l'appui arrive par la route du slot ; le contrôle appelle
   `CaptureMouse()` ;
2. le même évènement XAML remonte ensuite à la racine, dont l'échantillon
   attend dans la file : quand il est traité, la fenêtre a la capture, et la
   route racine livre à la fenêtre qui a la capture ce même appui, une
   seconde fois et comme le sien (`captureRoutes`) ;
3. tant que la racine tient cet appui, `SendSlotMouseEvent()` n'envoie plus
   rien pour le slot, et le relâchement, consommé pour le slot propriétaire
   (`DispatchSlotOwnedRelease()`), n'atteint jamais la fenêtre ;
4. le relâchement du clic suivant équilibre l'appui de la racine et arrête
   enfin le glissement.
Corrections dans `hostinput.cpp` :
- `RouteRootPointerSampleAtEpoch()` ne route pas un appui venu de XAML dont
  un slot est déjà propriétaire (`HasSlotPointerOwner()`) ;
- `OnRootPointer()` route les mouvements d'un geste commencé sur XAML
  (`m_xamlGestureActive`) quand une fenêtre wx tient la capture ;
- une interruption (`WM_CAPTURECHANGED` du pont, `PointerCaptureLost`,
  `PointerCanceled`, par le slot ou par la racine) n'annule pas l'appui du
  slot dont la fenêtre tient la capture
  (`wxWinUISlotWindowHoldsCapture()`) : c'est cette prise de capture que
  l'îlot rapporte.
Filet dans `wxHeaderCtrl::OnMouse()` : un mouvement sans bouton pendant un
glissement le termine. Cela vaut pour toute fenêtre à contenu XAML qui
capture pendant un appui (l'en-tête de wxGrid et de wxPropertyGridManager,
le glissement d'un onglet de wxAuiNotebook). Tests :
`HeldButtonDragReachesTheHeaderThroughTheHost` et
`PlainClickLeavesNoDragBehind` ; sans le premier correctif ils échouent
avec le symptôme exact. Pour rejouer une vraie souris dans un test, l'appui
doit passer par le slot **puis** par la racine : c'est ce second passage
qui manquait à la première version du test, qui passait à tort.

**wxListBox et `wxLB_HSCROLL`.** `wxLB_HSCROLL` est `wxHSCROLL`, qui donnait
à la fenêtre le style `WS_HSCROLL` : une barre horizontale de Win32 restait
sous la ListView, toujours là, sans rien à faire défiler. La fenêtre n'a
plus de barre (`wxListBox::MSWGetStyle()`), la ListView a les siennes.

**Défilement au pixel de wxPropertyGrid.** L'unité de défilement est 1
pixel sous wxWinUI (`wxPG_PIXELS_PER_UNIT`). La molette est traitée par
`wxPropertyGrid::WinUIOnWheel()` : un cran glisse de trois rangées
(minuteur de 10 ms, un tiers du reste à chaque pas), un pavé tactile est
suivi au pixel ; les flèches de la barre déplacent d'une rangée.

**wxSashWindow.** Le trait que Win32 dessine sur l'écran pendant le
glissement n'existe pas sous XAML : `wxEVT_SASH_DRAGGED` est envoyé à
chaque mouvement, les zones suivent le pointeur.

**wxSlider.** Les étiquettes et les graduations prenaient le `Foreground`
du `Slider`, qui est la couleur d'accent de la partie remplie de la piste :
elles sont dans la couleur du texte et dans celle du `TickBar` de WinUI.
La plage de `wxSL_SELRANGE`, que WinUI n'a pas, est une bande translucide
de 12 DIP autour de la piste, distincte de la partie remplie.

Divers : la molette fait défiler horizontalement une liste `wxLC_LIST` (la
liste d'un `wxFileCtrl`) ; la projection des listes ne redemande plus les
images des cellules quand la liste ne fait que défiler ; `wxListBox` ajoute
ses lignes sans tout remesurer ; un champ de texte multiligne n'a plus la
bande de défilement de Win32 sous son contenu.

Tests retirés, devenus sans objet : `NativeProgressBoundaryAndLifetime` (le
TaskDialog) et la section `Current_page_tracking_does_not_bind_application_pages`
(l'hôte suit chaque fenêtre d'une fenêtre de premier niveau par ses propres
gestionnaires d'évènements, une page d'assistant comme les autres).

Tests au 24 novembre au soir, écran à 100 % : `[winui-headerctrl]`,
`[headerctrl]`, `[winui-beta-dialogs]`, `[winui-dialog-lifetime]`,
`[winui-listctrl]`, `[HostState]` passent. Restent en échec, connus :
les deux de `[winui-combo]`, `winuipropgrid.cpp:741` et
`winuipropgrideditors.cpp:170` (antérieurs), et les trois de l'aperçu de
date de la grille de propriétés.

Reste de ce lot :
- **AUI** : non repris. Vu en capture hors écran : le glissement d'un sash
  redimensionne en direct et un volet se détache. Non vu : l'indice
  d'ancrage, qui dépend de la position réelle du curseur. Avec le
  flottement désactivé, le rejeu d'un déplacement de volet n'a montré ni
  indice ni ancrage : à examiner.
- Les évènements de XAML eux-mêmes ne se rejouent pas hors écran (le pont
  lit la position réelle du curseur) : le glissement de l'en-tête est
  vérifié par les routes de l'hôte dans les tests et par messages postés,
  pas avec une vraie souris.
- `Grid::GetNonDefaultAlignment` (`gridtest.cpp:2342`) échoue : l'alignement
  vertical par défaut des cellules est le centre depuis le lot des tables,
  et non le haut comme sous wxMSW. À trancher.
- L'aperçu de date de la grille de propriétés, comme au lot précédent.
