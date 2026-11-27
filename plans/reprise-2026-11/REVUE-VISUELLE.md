# wxWinUI : revue visuelle des composants (22 novembre 2026)

## Suites données le jour même

Ce relevé décrit les binaires **d'avant** les corrections ci-dessous
(copie figée `F:\wxb\revue-snap`). Les captures et les scripts joués sont
gardés dans `F:\wxb\revue-2026-11-22`.

| Ligne | Suite |
|---|---|
| Cadre blanc à angles vifs (28, et 8, 9, 12, 13, 14, 21 pour leur cadre) | **Corrigé** : la bordure des fenêtres est dessinée par XAML, un pixel, coins de 4, pinceau du thème (ARCHITECTURE-CIBLE, « Étape 2, troisième lot ») |
| 2, plantage Listbook puis Treebook | **Corrigé.** Cause : `wxScrollHelperEvtHandler::ProcessEvent` (`src/generic/scrlwing.cpp`) n'acheminait plus aucun événement à une fenêtre en cours de destruction, donc pas son `wxEVT_DESTROY` : l'hôte gardait le canevas d'une fenêtre libérée. Valait pour toute fenêtre défilante, et pour les gestionnaires des applications |
| 4, aperçu de wxFontDialog | **Corrigé** : l'échantillon garde la couleur choisie et se pose sur le fond de l'autre thème quand il serait illisible. Le 23, le dialogue générique a été remplacé par celui fait d'après Windows (`fontdlg.cpp`), qui applique la même règle |
| 6, wxBannerWindow | **Corrigé** : le dégradé par défaut va du fond du thème à sa couleur d'accent ; sur un dégradé choisi par le programme le texte prend la couleur qui s'y lit |
| 5, petites icônes de wxListCtrl | Non corrigé : c'est la disposition du contrôle générique, que la projection suit. À reprendre avec le moteur de table (étape 5) |
| 1, onglets de wxNotebook à gauche et en bas | Non corrigé : des `TabViewItem` isolés, hors de tout `TabView`, dont le contenu disparaît à l'état sélectionné. À refaire sur de vrais contrôles (étape 5) |
| 3, fille MDI vide | À confirmer à l'écran |
| 7, wxGrid | **Corrigé en capture** le 23 : une table WinUI (cellules plates, filets, sélection teintée d'accent) |
| 9, en-têtes et colonnes de wxListCtrl | **Corrigé en capture** le 23 : en-tête de table WinUI, largeur par défaut en DIP |
| 11, wxComboCtrl et wxOwnerDrawnComboBox | **Refait** le 23 : face d'un ComboBox WinUI, champ de texte sans cadre |
| 12, wxHtmlListBox et wxVListBox | **Refait** le 23 : sélection d'un élément de liste WinUI |
| 22, wxDataViewCtrl et wxTreeListCtrl | **Corrigé en capture** le 23 : en-têtes, chevron, largeurs ; glisser des colonnes refait, non essayé |
| 24, wxHeaderCtrl | **Corrigé en capture** le 23 |
| 26, wxSpinCtrl et wxSpinButton | **Corrigé en capture** le 23 : hauteur d'un champ WinUI, paire de boutons compacte |
| 8, wxPropertyGrid | **Refait** le 23 d'après WinUI.TableView : lignes de 40, éditeurs de 32 centrés, pastille de couleur arrondie au repos et en édition, flèches de repli à la place de celles d'un arbre, cadre XAML. Vu en capture (propgriddemo au repos et en édition, Showcase) à 100 % ; à confirmer à l'écran et à 150 % |
| Tables (9, 22, 24, 7 et wxFileCtrl) | **Repris** le 23 sur la même référence : bande d'en-tête, filets clairs, lignes de 40, cadre à coins de 4 (ARCHITECTURE-CIBLE, « sixième lot »). Vu en capture ; thème clair non vu |
| Rectangle vide dans la grille de propriétés de propgriddemo | **Corrigé** le 23 : l'île du cadre était trouée pour la barre de défilement d'une fenêtre de journal enfant |
| 18, sélecteurs de date et d'heure | **Date** : parties compactes le 23. **Heure** : inchangée, sa retenue est câblée sur les flèches en ligne |
| 21, wxListbook | **Corrigé en capture** le 23 : fond en carte WinUI, page sélectionnée marquée |
| 1, onglets de wxNotebook à gauche et en bas | **En partie** le 23 : l'onglet sélectionné est montré, au style d'une liste de navigation WinUI ; à gauche la disposition icône / libellé et la flèche de défilement restent à reprendre |
| 25, wxStatusBar | **Corrigé en capture** le 23 : plus de cases ni de poignée dessinée |
| wxGauge invisible (signalé le 22 au soir) | **Corrigé** : hauteur minimale du `ProgressBar` donnée en valeur locale |
| Liste de wxFileCtrl : une ligne, retard et chevauchements (signalé le 22 au soir) | **Corrigé en capture** : transitions d'éléments du `ListView` retirées de la projection, hauteur de 280 dans Showcase |
| 15, wxCollapsiblePane | **Corrigé** : porté sur l'`Expander` de WinUI |
| 23, wxToolBar | **Refait** le 23 sur `CommandBar` : débordement dans le menu « ... », chevron discret pour les outils à menu, boutons sans cadre. Vu en capture (Showcase, toolbar.exe) ; menu « ... » ouvert et clics non vus |
| 16, wxMDIParentFrame | **Refait** le 23 : documents en onglets `TabView` à bouton de fermeture, barre de menus WinUI. Vu en capture (mdi.exe, sashtest.exe) ; clics non vus |
| 17, wxSashWindow | **Corrigé en capture** le 23 : filets du thème, plus de relief |
| 19, wxProgressDialog | **Corrigé en capture** le 23 : dialogue générique en contrôles WinUI au lieu du `TaskDialog` |
| 20, wxMiniFrame | Revu le 23 : c'est la petite barre de titre système des fenêtres d'outils, comme celle des autres cadres. La coupure du contenu vient de la démo (taille en pixels) |
| 29, wxBusyInfo | **Corrigé**, à voir à l'écran : sans cadre, coins arrondis demandés à Windows (non visibles en capture) |
| 30, wxGenericProgressDialog | **Corrigé en capture** : jauge visible, plus de texte coupé |
| 32, astuce du jour | **Corrigé en capture** le 23 : police des autres contrôles |
| 10, wxAuiManager et wxAuiToolBar | **Corrigé en capture** le 23 : titres de panneaux plats aux couleurs et mesures WinUI, boutons en glyphes fins, poignées en points, flèches en chevron et « ... ». Reste : le libellé d'un contrôle dans une barre garde un fond gris ; la barre de titre classique d'auidemo, à confirmer à l'écran ; panneaux flottants et indices de glissement non vus |
| 13, 27, 31, 33, 34 | Étape 5, par composant : restent à faire |

Revue en lecture seule. Binaires : `F:\wxb\revue-snap`. Toutes les démos ont tourné sur le bureau invisible, par `drive.ps1` et `revue.ps1`. Thème sombre partout : le thème clair n'a pas été vu. 96 captures gardées.

Dossier des captures, noté `R\` dans les tableaux :
`F:\wxb\revue-2026-11-22\`

- `R\showcase\` : les dix pages de showcase.exe et ses dialogues.
- `R\pass1\` : fenêtre de départ de chaque démo.
- `R\pass2\` : états atteints par commande de menu.
- `R\scripts\` : les scripts joués.

Colonne « Appui » : ce qui dessine le composant. « XAML » = vrai contrôle WinUI. « GDI » = dessiné par wx sur une surface de peinture. « Mixte » = générique avec des éléments XAML posés dessus. Cette colonne vient de COMPOSANTS.md et d'un grep rapide des sources, pas des captures.

## 1. Tableau classé par gravité

### Cassé

| # | Composant | Démo et accès | Capture | Ce qui ne va pas | Appui | Gravité |
|---|---|---|---|---|---|---|
| 1 | wxNotebook, onglets à gauche et en bas | notebook.exe, commandes 6011 (`ID_ORIENT_LEFT`) puis 6010 (`ID_ORIENT_BOTTOM`) | `R\pass2\notebook-left.png`, `R\pass2\notebook-bottom.png` | L'onglet sélectionné est un rectangle sombre vide, sans icône ni texte. À gauche, icônes et libellés sont décalés, l'onglet « Text » manque, les flèches de défilement sont des triangles classiques. | XAML fait main, pas `TabView` | cassé |
| 2 | wxListbook puis wxTreebook : plantage | notebook.exe, commande 6001, une capture, commande 6003, une capture | pas d'image ; journal de drive.ps1 : `PROGRAM EXITED with code 0xC0000005` | Violation d'accès juste après la première capture du Treebook. Reproduit 3 fois sur 3. Pas de plantage sans capture entre les deux, ni vers Choicebook, ni avec Treebook seul. | Mixte (la liste du Listbook est un wxListCtrl générique) ; cause non cherchée | cassé |
| 3 | wxMDIChildFrame | mdi.exe, commande 5002 (`wxID_NEW`) | `R\pass2\mdi-children.png`, `R\pass2\mdi-client.png`, `R\pass2\mdi-child.png` | La fenêtre fille existe (202x185 dans l'arbre des fenêtres) mais elle sort toute sombre : ni titre, ni cadre, ni dessin du canevas. Peut être une limite de la capture pour une fille MDI Win32. | Win32 | cassé, à confirmer à l'écran |
| 4 | wxFontDialog | showcase.exe `--page=8 --action="wxFontDialog"` | `R\showcase\dlg-font-1-2.png` | Aperçu en texte noir sur fond sombre : illisible. Cadre blanc à angles vifs. Familles « Swiss », couleur « BLACK » : c'est le dialogue générique. | Composite générique, aperçu GDI | cassé |
| 5 | wxListCtrl, petites icônes | listctrl.exe, commande 6004 (`LIST_SMALL_ICON_TEXT_VIEW`) | `R\pass2\listctrl-smallicontext.png` | L'icône est au-dessus du libellé, comme en grandes icônes. Elle devrait être à gauche. | Mixte | cassé |
| 6 | wxBannerWindow | showcase.exe, page 0 | `R\showcase\page0-overview.png` | Dégradé blanc vers bleu fixé dans le code générique. Le texte prend le blanc du thème sombre : le début du titre est invisible. | GDI | cassé (illisible) |

### Pas WinUI (fonctionne, mais aspect générique)

| # | Composant | Démo et accès | Capture | Ce qui ne va pas | Appui | Gravité |
|---|---|---|---|---|---|---|
| 7 | wxGrid | grid.exe ; commande 170 (sélection d'une ligne), 136 (en-tête « natif »), 165 (« Bugs table ») | `R\pass1\grid-0.png`, `R\pass2\grid-selrow.png`, `R\pass2\grid-nativeheader.png`, `R\pass2\grid-bugstable.png` | Chaque cellule est une boîte arrondie isolée : une mosaïque, pas une table. La sélection est faite de pastilles bleu système plein. Étiquettes de lignes et de colonnes sans séparateurs. Cellules à fond gris clair avec texte blanc : illisibles. | Mixte (`gridprojection.cpp`) | pas WinUI |
| 8 | wxPropertyGrid, wxPropertyGridManager | propgriddemo.exe ; clic posté sur « Cell Colour » | `R\pass1\propgriddemo-0.png`, `R\pass2\propgrid-click.png` | Cadre blanc 1 px à angles vifs autour de la grille et de la zone de description. La ligne sélectionnée montre l'éditeur générique (combo à bouton encadré), différent des lignes au repos. Barre d'outils du manager : petites icônes anciennes. | Mixte | pas WinUI |
| 9 | wxListCtrl : rapport, icônes, liste, virtuel | listctrl.exe, commandes 6005, 6002, 6000, 6006 ; sélection par 6028 | `R\pass1\listctrl-2.png`, `R\pass2\listctrl-report-sel-lines.png`, `R\pass2\listctrl-icontext.png`, `R\pass2\listctrl-list.png`, `R\pass2\listctrl-virtual.png` | Cadre blanc 1 px à angles vifs, fond opaque plus sombre que la fenêtre. En-têtes en cases encadrées ; leur texte prend la couleur du texte de la liste (bleu dans la démo). Sélection : simple bande grise, sans la marque d'accent d'un `ListView`. | Mixte (`listprojection.cpp`) | pas WinUI |
| 10 | wxAuiManager, wxAuiToolBar | auidemo.exe ; perspective « All Panes », commande 7013 | `R\pass1\auidemo-0.png`, `R\pass2\aui-allpanes.png` | Titres de panneaux en GDI : bande grise plate, boutons en petits glyphes pixelisés. Barres d'outils : poignées en pointillés, flèche de débordement en triangle classique. Les onglets du wxAuiNotebook sont corrects, mais leurs flèches de défilement sont des triangles classiques. La fenêtre sort avec une barre de titre claire classique, seule démo dans ce cas (la démo crée son cadre avec `wxSUNKEN_BORDER`) : à confirmer à l'écran. | GDI (`dockart.cpp`, `barartwinui.cpp`) ; onglets mixtes | pas WinUI |
| 11 | wxComboCtrl, wxOwnerDrawnComboBox | combo.exe ; showcase page 3 ; widgets.exe commande 206 | `R\pass1\combo-2.png`, `R\showcase\page3-choices.png`, `R\pass2\widgets-odcombo.png` | Aspect classique : cadre blanc à angles vifs, bouton flèche encadré, flèches bitmap noires. Rien de commun avec le `ComboBox` WinUI placé juste à côté. | GDI | pas WinUI |
| 12 | wxHtmlListBox, wxVListBox | htlbox.exe | `R\pass1\htlbox-0.png` | Sélection en rectangle bleu système plein. Séparateurs en pointillés. Cadre blanc à angles vifs. | GDI | pas WinUI |
| 13 | wxRichTextCtrl, sa barre et ses listes de styles | richtextdemo.exe, taille de départ puis 1100x720 ; showcase page 7 | `R\pass1\richtextdemo-0.png`, `R\pass2\richtext-1100.png`, `R\showcase\page7-advanced.png` | Surface peinte dans un cadre blanc à angles vifs. Le combo de styles de la barre est le combo générique classique. La liste de styles a un double cadre blanc. Icônes gras, italique, souligné noires sur fond sombre : invisibles (images de la démo). À 1100 px le combo de styles sort de la barre, sans débordement. | GDI | pas WinUI |
| 14 | wxFileCtrl | widgets.exe, commande 196 | `R\pass2\widgets-filectrl.png` | Deux cadres blancs emboîtés. Noms en bleu et en rouge, à la façon du générique. Noms coupés entre colonnes. | Composite sur wxListCtrl mixte | pas WinUI |
| 15 | wxCollapsiblePane | collpane.exe, puis commande 101 ; dialogue « À propos » de dialogs.exe (6055) | `R\pass1\collpane-2.png`, `R\pass2\collpane-expanded.png`, `R\pass2\dlgs-about-full-2.png` | Un chevron et un libellé nus, avec un rectangle de focus. Pas la carte d'en-tête d'un `Expander`. Champ de texte coupé dans le volet ouvert. | Générique | pas WinUI |
| 16 | wxMDIParentFrame | mdi.exe, docview.exe, sashtest.exe | `R\pass1\mdi-2.png`, `R\pass1\docview-1.png`, `R\pass1\sashtest-2.png` | Barre de menus Win32 classique : bande grise, petite police. Dans docview, la zone cliente MDI a un cadre blanc. | Win32 | pas WinUI |
| 17 | wxSashWindow, wxSashLayoutWindow | sashtest.exe | `R\pass1\sashtest-2.png` | Doubles bordures en relief classiques autour de chaque zone. | GDI | pas WinUI |
| 18 | wxDatePickerCtrl, wxTimePickerCtrl, style par défaut | showcase page 5 | `R\showcase\page5-pickers.png` | Trois champs numériques à flèches, séparés par « / » ou « : ». Ce n'est l'aspect ni de `CalendarDatePicker` ni de `TimePicker`. Source : `datectrl.cpp` ligne 740 force le style « spin » sans `wxDP_DROPDOWN` ; `timectrl.cpp` (commentaire ligne 79) garde un `TimePicker` comme pair interne et montre trois `NumberBox`. | XAML (`NumberBox`), mais pas le contrôle fait pour ça | pas WinUI |
| 19 | wxProgressDialog natif | dialogs.exe, commande 6050 | `R\pass2\dlgs-progress-2.png` | `TaskDialog` Win32 : barre de progression blanche classique, boutons anciens. | Win32 | pas WinUI |
| 20 | wxMiniFrame | dialogs.exe, commande 6047 | `R\pass2\dlgs-miniframe-2.png` | Petite barre de titre maison avec un « x » en texte. Contenu coupé à droite. | Win32 | pas WinUI |
| 21 | wxListbook | notebook.exe, commande 6001 ; showcase page 4 | `R\pass2\book-listbook.png`, `R\showcase\page4-containers.png` | Liste d'icônes dans un cadre blanc à angles vifs. Dernier élément coupé. Aucune marque de sélection visible. | Mixte | pas WinUI |

### Finition

| # | Composant | Démo et accès | Capture | Ce qui ne va pas | Appui | Gravité |
|---|---|---|---|---|---|---|
| 22 | wxDataViewCtrl, wxTreeListCtrl | dataview.exe, première page ; treelist.exe ; showcase page 6 | `R\pass1\dataview-0.png`, `R\pass1\treelist-2.png`, `R\showcase\page6-data.png` | Flèches de dépliage : petits triangles dans des boutons carrés, pas le chevron de `TreeView`. En-têtes en boutons arrondis avec une poignée en pointillés ; « year » coupé. Colonne « custom » : pastille gris clair, texte blanc illisible. Dans showcase, première colonne du wxTreeListCtrl trop étroite, textes coupés. | XAML (`ListView`, en-têtes maison) | finition |
| 23 | wxToolBar | toolbar.exe, commandes 202, 208, 201 | `R\pass1\toolbar-0.png`, `R\pass2\toolbar-two.png`, `R\pass2\toolbar-left.png`, `R\pass2\toolbar-horztext.png` | La flèche d'un outil à menu est une boîte haute séparée, avec un petit triangle. Pas de débordement : les outils sont coupés au bord, en largeur comme en hauteur. En mode « texte à côté », les boutons deviennent bordés. | XAML (`AppBarButton` en grille maison, pas `CommandBar`) | finition |
| 24 | wxHeaderCtrl | showcase page 6 ; widgets.exe commande 200 | `R\showcase\page6-data.png`, `R\pass2\widgets-header.png` | Colonnes en boutons arrondis collés, pas un en-tête de table. | Mixte (`headerprojection.cpp`) | finition |
| 25 | wxStatusBar | statbar.exe et toutes les démos | `R\pass1\statbar-2.png` | Champs dessinés comme des cases encadrées. Poignée en points classiques. | XAML (`Grid` + `TextBlock`) | finition |
| 26 | wxSpinCtrl, wxSpinCtrlDouble, wxSpinButton | showcase page 2 | `R\showcase\page2-text.png` | `NumberBox` deux fois trop haut, texte collé en haut. wxSpinButton étiré en deux barres sur toute la largeur. | XAML | finition |
| 27 | wxCalendarCtrl, taille | calendar.exe, taille de départ puis 760x560 | `R\pass1\calendar-2.png`, `R\pass2\calendar-760.png` | À la taille de départ le calendrier est coupé : colonne « di » et flèche suivante absentes. Correct une fois la fenêtre agrandie. La taille préférée semble trop petite. | XAML (`CalendarView`) | finition |
| 28 | Cadre blanc des contrôles à bordure | widgets.exe commande 193 (wxGenericDirCtrl) ; stctest.exe (wxStyledTextCtrl) ; docview.exe | `R\pass2\widgets-dirctrl.png`, `R\pass1\stctest-0.png`, `R\pass1\docview-1.png` | Contenu correct, mais cadre blanc 1 px à angles vifs. Le même cadre entoure wxListCtrl, wxPropertyGrid, wxRichTextCtrl, wxHtmlListBox, wxListbook, wxFileCtrl. Les contrôles XAML voisins ont un cadre discret à coins arrondis. | Bordure wx, pas XAML | finition |
| 29 | wxBusyInfo | dialogs.exe, commande 6057 | `R\pass2\dlgs-busyinfo-0.png` | Fenêtre à angles vifs avec un liseré clair. | Générique | finition |
| 30 | wxGenericProgressDialog | dialogs.exe, commande 6051 | `R\pass2\dlgs-progress-generic-2.png` | La jauge n'apparaît pas dans la capture (mode indéterminé : peut-être l'animation, non capturée). « Unknown » coupé à droite. | Composite | finition, à confirmer |
| 31 | wxTextCtrl avec `wxHSCROLL` | text.exe | `R\pass1\text-2.png` | Bande noire opaque avec un petit curseur en bas du champ. | XAML | finition |
| 32 | Astuce du jour | dialogs.exe, commande 6038 | `R\pass2\dlgs-tip-2.png` | Texte en Arial, pas en Segoe. | Composite | finition |
| 33 | wxRichToolTip | showcase.exe `--page=8 --action="Tooltip windows"` | `R\showcase\dlg-tooltip-1-0.png` | Rectangle à angles vifs avec le texte. Le fond n'est pas jugeable (Mica non capturé). | Générique d'après COMPOSANTS.md | finition, à confirmer |
| 34 | widgets.exe, page Button | commande 186 | `R\pass2\widgets-button.png` | Le bouton testé est coupé par le bord droit : il commence à x=1199 dans une fenêtre de 1226. Les contrôles WinUI sont plus larges que ce que la mise en page de la démo prévoit. | XAML | finition |

## 2. Composants qui ont le bon aspect

Vus en capture, sans défaut relevé : wxButton, wxBitmapButton, wxCommandLinkButton, wxToggleButton, wxCheckBox, wxRadioButton, wxRadioBox, wxStaticBox, wxStaticText, wxHyperlinkCtrl, wxInfoBar, wxTextCtrl (simple, mot de passe, lecture seule, multiligne, riche), wxSearchCtrl, wxChoice, wxComboBox, wxBitmapComboBox, wxListBox, wxCheckListBox, wxRearrangeCtrl, wxEditableListBox, wxAddRemoveCtrl, wxSlider, wxTreeCtrl, wxNotebook à onglets en haut, wxChoicebook, wxTreebook, wxToolbook, wxAuiNotebook (dans notebook.exe et showcase), wxSplitterWindow, wxColourPickerCtrl, wxFontPickerCtrl, wxFilePickerCtrl, wxDirPickerCtrl, wxCalendarCtrl quand il a la place, wxRibbonBar (showcase), wxMenuBar hors MDI, wxMessageDialog, wxTextEntryDialog, wxNumberEntryDialog, wxSingleChoiceDialog, wxMultiChoiceDialog, wxRearrangeDialog, wxFindReplaceDialog, wxColourDialog, wxCredentialEntryDialog, wxWizard, wxPreferencesEditor, wxPropertySheetDialog, wxLogDialog, wxAboutBox (hors volets repliables), dialogue de modèles de docview.

Acceptables comme surfaces peintes, au cadre près : wxStyledTextCtrl, wxHtmlWindow.

## 3. Ce qui n'a pas pu être atteint

- **dataview.exe, six autres pages** : les onglets sont un `TabView`, les clics postés n'y arrivent pas, et la démo n'a pas de commande de menu pour changer de page. Une touche et `TCM_SETCURSEL` postés à la fenêtre du notebook n'ont rien changé.
- **Tout ce qui s'ouvre par un clic sur un contrôle XAML** : popups de popup.exe (`BM_CLICK` posté au bouton sans effet), listes déroulantes de wxComboCtrl, flyouts des sélecteurs, éditeurs déroulants de wxPropertyGrid, menus déroulants et contextuels (menu.exe : seule la barre est vue).
- **treectrl.exe** : l'arbre reste replié sur sa racine (touche postée sans effet). Un arbre déplié est vu dans showcase page 6 et dans la page DirCtrl de widgets.exe. Les styles de lignes et de boutons n'ont pas été vus.
- **dialogs.exe** : les commandes 6003 (couleur générique), 6027 (fichier générique) et 6037 (répertoire générique) n'ouvrent aucune fenêtre. Probablement absentes de ce build : non vérifié. Les dialogues système de fichier et de répertoire n'ont pas été lancés.
- **docview.exe** : seuls le dialogue de modèles et le cadre MDI vide sont vus.
- **wxAuiManager** : panneaux flottants et indices de glissement.
- **États** : survol, pression, focus clavier, désactivé (sauf quelques boutons). Barres de défilement : elles ne s'affichent qu'au passage du pointeur (`scrollbarprojection.cpp`), aucune n'est visible en capture. Fond Mica. Thème clair. Échelle au-dessus de 100 %.
- **Non lancés** : splash.exe, notifications, wxMediaCtrl, wxWebView, ribbondemo.exe (le ruban est vu dans showcase page 7).

## Vu ou déduit

- **Vu en capture** : toutes les colonnes « Ce qui ne va pas », sauf les phrases qui citent un fichier source.
- **Vu dans le journal de drive.ps1** : le plantage de la ligne 2 et la taille de la fille MDI de la ligne 3.
- **Déduit des sources ou de COMPOSANTS.md** : la colonne « Appui », la cause de la ligne 18 (lue dans `datectrl.cpp` et `timectrl.cpp`), la cause de la ligne 6 (couleurs par défaut lues dans `src\generic\bannerwindow.cpp`).
- **À confirmer à l'écran** : lignes 3, 30, 33, et la barre de titre classique de auidemo (ligne 10). Ces quatre points peuvent venir de la capture elle-même.
- Plusieurs couleurs criardes viennent des démos (texte bleu de listctrl, cellules rouges de grid, fond gris de text.exe) et ne sont pas comptées comme défauts, sauf quand le texte devient illisible.

Aucun processus de démo n'est resté en marche. Aucun fichier n'a été touché sous `C:\wxWidgets-master`.
