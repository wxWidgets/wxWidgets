# wxWinUI — matrice des composants (21 novembre 2026)

État relevé dans le code de `winui/reprise` (répartition des en-têtes publics,
listes de sources CMake, recherches ciblées). Une case « reste à faire » vide
signifie qu'aucun manque n'a été repéré à la lecture, pas que le composant est
complet. L'objectif est que toutes les lignes passent à **WinUI**.

Légende : **WinUI** = implémentation dédiée sur des contrôles XAML ;
**Superposition** = éléments XAML posés sur un contrôle générique qui reste
maître ; **Générique** = dessiné par wx en GDI ; **Win32** = contrôle ou
dialogue natif de wxMSW ; **Composite** = assemblage générique dont les
pièces visibles sont déjà des contrôles WinUI.

Total : 140 familles. WinUI 40, Superposition 11, Générique 24, Win32 26,
Mixte 38 (dont 29 composites).

**Aspect à l'écran** : la revue visuelle du 22 novembre (`REVUE-VISUELLE.md`,
34 constats classés par gravité, avec la liste des composants qui ont le bon
aspect) complète cette matrice, qui dit sur quoi chaque composant s'appuie.

## Déjà en WinUI (40)

| Classes | Contrôle XAML | Fichier (lignes) | Reste à faire |
|---|---|---|---|
| wxButton, wxBitmapButton, wxCommandLinkButton | `Button` | `button.cpp` (2 967) | Pas de classe dédiée pour le lien de commande |
| wxToggleButton, wxBitmapToggleButton | `ToggleButton` | `tglbtn.cpp` (2 228) | |
| wxCheckBox | `CheckBox` | `checkbox.cpp` (554) | |
| wxRadioButton | `RadioButton` | `radiobut.cpp` (724) | |
| wxRadioBox | Grille de `RadioButton` | `radiobox.cpp` (2 165) | Boutons inertes signalés dans Showcase |
| wxStaticBox | `Border` + `TextBlock` | `statbox.cpp` (613) | |
| wxStaticText | `TextBlock` | `stattext.cpp` (754) | |
| wxStaticBitmap | `Image` | `statbmp.cpp` (475) | |
| wxStaticLine | `Border` | `statline.cpp` (92) | |
| wxCollapsiblePane | `Expander` (depuis le 22 novembre) | `collpane.cpp` | Le panneau reste une fenêtre wx posée sur le contenu de l'`Expander` : son apparition n'est pas animée avec lui |
| wxTextCtrl | `TextBox` / `PasswordBox` / `RichEditBox` | `textctrl.cpp` (14 784) | `wxTE_CHARWRAP` / `wxTE_WORDWRAP` jamais référencés ; hauteur une ligne trop faible (PBfordev) |
| wxSearchCtrl | `AutoSuggestBox` | `srchctrl.cpp` (2 046) | |
| wxChoice | `ComboBox` | `choice.cpp` (1 735) | |
| wxComboBox | `ComboBox` éditable | `combobox.cpp` (9 916) | `Popup()` / `Dismiss()` en échec pour `wxCB_SIMPLE`. La liste de `wxCB_SIMPLE` est encore l'ancien `ListBox` XAML, mais avec les éléments d'un `ListView` depuis le 23 novembre ; reste à y passer `wxFontDialog` |
| wxBitmapComboBox | `ComboBox` + images | `bmpcbox.cpp` (274) | |
| wxListBox, wxCheckListBox, wxRearrangeList | `ListView` | `listbox.cpp` (5 934) | Hauteur par élément en dessin personnalisé ; lignes trop basses signalées |
| wxTreeCtrl | `TreeView` | `treectrl.cpp` (10 052) | Styles de lignes, `FULL_ROW_HIGHLIGHT`, `TWIST_BUTTONS`, hauteur variable non gérés |
| wxDataViewCtrl, ListCtrl, TreeCtrl | `ListView` + en-têtes et éditeurs XAML | `dataview.cpp` (3 357) | Glisser-déposer, menu contextuel, lignes de grille, hauteur variable, clavier ; listes virtuelles non virtualisées ; rafraîchissement complet à chaque interaction |
| wxNotebook | `TabView` | `notebook.cpp` (9 086) | `TabView` seulement pour `wxNB_TOP` ; autres orientations sur une surface XAML faite main |
| wxGauge | `ProgressBar` | `gauge.cpp` (356) | |
| wxSlider | `Slider` | `slider.cpp` (4 001) | |
| wxScrollBar | `ScrollBar` | `scrolbar.cpp` (526) | |
| wxSpinButton | deux `RepeatButton` | `spinbutt.cpp` (681) | |
| wxSpinCtrl, wxSpinCtrlDouble | `NumberBox` | `spinctrl.cpp` (2 392) | |
| wxDatePickerCtrl | `CalendarDatePicker` | `datectrl.cpp` (2 077) | |
| wxTimePickerCtrl | `TimePicker` + champs | `timectrl.cpp` (2 245) | |
| wxCalendarCtrl | `CalendarView` | `calctrl.cpp` (8 410) | Avertissement de non-convergence en défilement rapide |
| wxActivityIndicator | `ProgressRing` | `activityindicator.cpp` (716) | |
| wxHyperlinkCtrl | `HyperlinkButton` | `hyperlink.cpp` (1 415) | |
| wxInfoBar | `InfoBar` | `infobar.cpp` (1 501) | |
| wxColourPickerCtrl | `DropDownButton` + `ColorPicker` | `clrpicker.cpp` (424) | |
| wxStatusBar | `Grid` + `TextBlock` | `statbar.cpp` (2 592) | Poignée de redimensionnement |
| wxToolBar | `CommandBar` (`AppBarButton`, `AppBarToggleButton`, `AppBarSeparator`) ; une colonne des mêmes boutons quand la barre est verticale | `toolbar.cpp` (1 577) | **Refait le 23 novembre** sur `CommandBar` : débordement dans le menu « ... », libellés à droite des icônes (sous les icônes en vertical), chevron pour les outils à menu, contrôles posés sur une place réservée. Pas de rangées (`SetRows`). Restent `wxRibbonToolBar` (déjà en XAML) et `wxAuiToolBar` (GDI) : voir ARCHITECTURE-CIBLE, « Étape 5, quatrième lot » |
| wxMessageDialog | Fenêtre privée à contenu XAML | `msgdlg.cpp` + `dlgpresenter.cpp` (2 640) | Deux fenêtres, gels et crashs |
| wxTextEntryDialog, wxPasswordEntryDialog | `TextBox` / `PasswordBox` | `textdlg.cpp` (553) | |
| wxColourDialog | `ColorPicker` | `colordlg.cpp` (557) | |
| wxWinUIXamlHost | XAML libre | `xamlhost.cpp` (199) | |

## Superpositions à remplacer par de vrais contrôles (11)

| Classes | Aujourd'hui | Cible |
|---|---|---|
| wxListCtrl, wxListView (y compris virtuel) | Générique + `ListView` sur les lignes visibles, sans les transitions d'éléments (23 novembre) | Moteur de table commun |
| wxGrid | Générique + `Canvas` de cellules | Table à sélection par cellule |
| wxHeaderCtrl | Générique + boutons XAML | En-tête du moteur de table |
| wxPropertyGrid, wxPropertyGridManager | Générique + cellules XAML, éditeurs wx | Moteur de table + éditeurs XAML |
| wxAuiNotebook, wxAuiTabCtrl, wxAuiMDI* | `TabView` pour les en-têtes, retour au GDI en multiligne | `TabView` complet |
| wxRibbonButtonBar, ToolBar, Gallery | `Button` / `ToggleButton` / `DropDownButton` / `SplitButton` discrets, `GridView` (depuis le 22 novembre) | Ruban XAML complet |
| wxMenuBar, wxMenu, menus contextuels | Modèle wxMSW affiché en `MenuBar` / `MenuFlyout` | À conserver, à fiabiliser ; MDI garde le menu Win32 |

## Génériques à porter (24)

| Classes | Cible WinUI envisagée |
|---|---|
| wxSplitterWindow, wxSashWindow | Séparateur XAML (poignée dessinée par XAML). wxSashWindow : filets du thème, plus de relief (23 novembre) |
| wxSimplebook | Rien à dessiner |
| wxComboCtrl, wxOwnerDrawnComboBox | `ComboBox` / `DropDownButton` + `Flyout` |
| wxVListBox, wxHtmlListBox, wxSimpleHtmlListBox | `ItemsRepeater` |
| wxAnimationCtrl | `Image` animée / `AnimatedVisualPlayer` |
| wxBannerWindow | `Border` + `TextBlock` + `Image` |
| wxRichToolTip | `TeachingTip` (le chemin existe mais n'est actif qu'en test) |
| wxSplashScreen, wxTipWindow | Fenêtre à contenu XAML |
| wxAuiManager (panneaux, sashes, indices), wxAuiToolBar | Chrome XAML ; la barre est aujourd'hui dessinée en GDI au style Fluent. Depuis le 23 novembre les titres de panneaux le sont aussi (`dockart.cpp`) : plats, texte du thème, hauteur 28, boutons à survol discret, barre d'accent sur le panneau actif ; flèches des barres en chevron et en « ... » |
| wxRibbonBar, wxRibbonPage, wxRibbonPanel, arts | Fait le 22 novembre par une implémentation propre : `SelectorBar`, contenu XAML par fenêtre aux métriques du Toolkit (voir ARCHITECTURE-CIBLE, « Ruban »). Reste : flyout XAML des groupes repliés |
| wxRichTextCtrl | `RichEditBox` ne couvre pas son modèle : reste une surface peinte |
| wxStyledTextCtrl | Scintilla : reste une surface peinte |
| wxHtmlWindow | Surface peinte, ou `WebView2` |
| wxRendererNative | Devient inutile quand plus rien n'est dessiné en GDI |
| Doublons génériques encore compilés (wxGenericStaticText, wxGenericTreeCtrl…) | À retirer du build |

## Encore Win32 (26)

| Classes | Cible |
|---|---|
| wxMDIParentFrame, wxMDIChildFrame | **Fait le 23 novembre** : l'implémentation générique à onglets de wx (`src/generic/mdig.cpp`), dont le carnet est un `TabView` dans ce port ; onglets à bouton de fermeture, barre de menus WinUI du cadre. Plus de MDI Win32 (`src/msw/mdi.cpp` n'est plus compilé) : pas de cascade ni de mosaïque |
| wxMiniFrame, wxPopupWindow, wxPopupTransientWindow | Fenêtres à contenu XAML ; restent des HWND de premier niveau |
| wxPanel, wxWindow peint, wxScrolledWindow | Nœud XAML + surface de peinture (voir l'architecture) |
| wxRichMessageDialog, wxProgressDialog | wxProgressDialog : **fait le 23 novembre**, c'est le dialogue générique, fait des contrôles WinUI du port (`ProgressBar`, boutons). wxRichMessageDialog : `TaskDialog` natif aujourd'hui ; dialogue XAML |
| wxFileDialog, wxDirDialog | Dialogues système : à garder, ce sont ceux de Windows 11 |
| wxPrintDialog, wxPageSetupDialog | Dialogues système |
| wxFontDialog | **Fait le 23 novembre** (`fontdlg.cpp`) : la disposition du dialogue de Windows avec des contrôles WinUI, polices lues par DirectWrite. Ses colonnes sont à passer sur `wxComboBox(wxCB_SIMPLE)` une fois celui-ci refait |
| wxWebView | Contrôle XAML `WebView2` |
| wxMediaCtrl | `MediaPlayerElement` |
| wxGLCanvas, wxActiveXContainer, wxNativeWindow, wxNativeContainerWindow | Restent de vrais HWND, par nature |
| wxTaskBarIcon, wxTaskBarButton, wxAppProgressIndicator, wxNotificationMessage | API du shell, sans interface XAML |
| wxCaret, wxOverlay, wxDragImage | Liés au dessin GDI ; suivent la surface de peinture |
| wxDC, wxGraphicsContext, wxClipboard, wxDataObject, glisser-déposer OLE | Infrastructure, pas des composants visuels |
| wxHelpController | Aide HTML de Windows |

## Composites (29)

Assemblages génériques de contrôles déjà WinUI : ils deviennent corrects dès
que leurs pièces le sont. À vérifier un par un dans Showcase, sans portage
propre : wxChoicebook, wxListbook, wxTreebook, wxToolbook, wxWizard,
wxPropertySheetDialog, wxPreferencesEditor, wxRearrangeCtrl,
wxEditableListBox, wxTreeListCtrl, wxFontPickerCtrl, wxFilePickerCtrl,
wxDirPickerCtrl, wxFileCtrl, wxGenericDirCtrl, wxAddRemoveCtrl, wxBusyInfo,
wxGenericMessageDialog, wxNumberEntryDialog, wxSingleChoiceDialog,
wxMultiChoiceDialog, wxFindReplaceDialog, wxPrintPreview,
wxGenericProgressDialog, wxAboutBox, astuce du jour, wxLogWindow,
wxCredentialEntryDialog, dialogues de wxRichText.

wxFileCtrl : ses défauts du 22 novembre venaient de la projection de
`wxListCtrl` (transitions d'éléments) et de la hauteur que Showcase lui
donnait ; corrigés le 23, à confirmer à l'écran avant de décider d'un
portage dédié.
