# wxWinUI — audit technique des systèmes (21 novembre 2026)

Constats relevés par lecture du code de la branche `winui/reprise`
(`6a7bf38d6c`). Rien n'a été exécuté pour cet audit. « Lu » = vu dans le code,
« déduit » = conclusion tirée du code sans l'avoir observée à l'exécution.
Les chemins sont relatifs à la racine du dépôt.

Abréviations de fichiers : `HG` = `src/winui/hostgeometry.cpp`,
`TH` = `src/winui/tlwhost.cpp`, `CH` = `src/winui/ctrlhost.cpp`,
`HI` = `src/winui/hostinput.cpp`, `IR` = `src/winui/inputrouter.cpp`,
`IS` = `src/winui/inputstate.cpp`, `HR` = `src/winui/hostresize.cpp`,
`SB` = `src/winui/scrollbarprojection.cpp`, `MW` = `src/msw/window.cpp`,
`DC` = `src/msw/dcclient.cpp`, `WU` = `src/winui/winui.cpp`.

## 1. Géométrie, empilement, découpage

**Forme de l'arbre XAML : plate.** Un `Canvas` racine transparent, et un
« slot » par contrôle WinUI posé dessus en coordonnées de la fenêtre
principale (`TH:2977-3020`, `TH:5302`). Un slot = `Grid` conteneur →
`ContentControl` porteur → contrôle (`TH:5074-5103`). Un précédent
d'imbrication existe déjà pour les enfants de composites (`TH:5065-5071`).

**Rien n'atteint XAML de façon synchrone, et tout est recalculé à chaque fois.**

- Un enfant bouge : `WM_WINDOWPOSCHANGED` (`MW:5010-5023`) →
  `NotifyNativeLayoutMutation` → `InvalidateStructure` + **`MarkAllDirty`**
  (`HG:508-518`) → un `CallAfter(FlushSync)` (`HG:540`). Le drapeau « l'ordre
  a changé » est calculé puis jeté (`HG:509`).
- Tout `WM_WINDOWPOSCHANGED` de la fenêtre, tout réglage de scrollbar, tout
  `WM_ENABLE`, un changement de curseur ou de thème marquent **tous** les
  slots sales. Le commentaire `HG:512-517` dit qu'un marquage plus fin a été
  tenté puis annulé.
- Un `FlushSync` (`HG:543-735`) fait, même pour un seul tooltip modifié :
  reconstruction de la région de l'île avec parcours complet de l'arbre wx
  (`HG:2327-2344`), second parcours complet pour les scrollbars
  (`SB:538-615`), `SyncSlot` sur chaque slot, `RecomputeZOrder`, puis un
  hit-test XAML pour le curseur (`TH:9061-9085`).
- Un `SyncSlot` (`HG:838-1762`, 924 lignes dont environ 130 de géométrie)
  appelle `GetWindowRect`, `GetClientRect` + `MapWindowPoints` par ancêtre
  (`HG:1083-1107`), copie une dizaine de chaînes, relit le libellé, puis pose
  `Canvas.Left/Top/Width/Height` **sans comparer à la valeur précédente**
  (`HG:1261-1264`), recrée une géométrie de découpage (`HG:1282-1290`),
  repose la visibilité, le tooltip et les propriétés d'accessibilité.
- `RecomputeZOrder` (`HG:1764-2225`) : une région GDI par HWND natif, boucles
  de recouvrement imbriquées, pire cas quadratique (`HG:1875-1898`), puis un
  `ZIndex` reposé sur chaque slot (`HG:2196`).

**L'île.** Fenêtre enfant toujours au-dessus (`TH:2931-2950`). Le GDI se voit
à travers parce que le fond XAML est transparent, pas grâce à des trous. La
région de fenêtre ne sert qu'à découper les bandes des scrollbars natives ;
comme elle inclut le rectangle client (`HG:2294`), chaque pas de
redimensionnement réinstalle une région avec redessin complet dès qu'une
scrollbar native est visible (`HG:2421`).

**Empilement.** Tout le XAML est au-dessus de tout le natif. Un contrôle GDI
placé devant un contrôle XAML ne peut pas être montré ; il est seulement
compté (`HG:2013-2022`).

**Volume.** 23 100 lignes dans les huit fichiers de ce sous-système, dont
environ 2 000 de mécanisme réel, 4 000 à 5 000 de gardes de génération,
1 475 de support de test dans 209 blocs `#ifdef`, 2 850 de commentaires.
`tlwhost.h` porte 42 membres de génération ou d'époque.

## 2. Affichage du contenu non XAML, Mica, HDR, projections

**Mica** : purement DWM (`DWMWA_SYSTEMBACKDROP_TYPE` +
`DwmExtendFrameIntoClientArea(-1)`, `CH:258-288`). Aucun fond XAML.

**Le noir sert de clé de transparence.** Les panneaux effacent en noir
(alpha 0) pour laisser voir Mica (`MW:5662-5722`, `CH:3971-3973`). Tout ce que
GDI dessine a aussi alpha 0, sauf les fenêtres `wxBG_STYLE_PAINT`, dont le
`wxPaintDC` passe par un tampon où l'on force alpha à 255 pixel par pixel
(`DC:96-281`). C'était le « correctif HDR ».

Restent en alpha 0 (déduit de `CH:2873-2875`) : les fenêtres `EVT_PAINT` au
style de fond par défaut, tout dessin par `wxClientDC` / `wxWindowDC`, les
contrôles Win32 natifs. L'aperçu de police n'a pas été corrigé : il a été
remplacé par un `wxStaticText` (`src/generic/fontdlgg.cpp:50-66`).

**Mode clair illisible** (déduit) : sur un chemin alpha 0, un texte sombre
s'additionne au Mica clair et sature en blanc. Le noir exact est en plus
remappé en (27,27,27) parce qu'il sert de clé (`src/msw/settings.cpp:157-189`).

**Sources de clignotement** (déduit) : effacement en noir puis peinture
présentés séparément ; l'île efface puis fait réparer les pixels classiques
de façon asynchrone (`CH:3917-3919`) ; un redimensionnement d'un pixel
« d'amorçage » de la fenêtre à chaque fin de déplacement (`MW:5070-5082`) ;
surface GDI et composition XAML non synchronisées.

**Les quatre « projections » sont des superpositions.** Le contrôle wx reste
vivant et fait autorité ; des éléments XAML sont posés dessus.

- *Scrollbar* : la fenêtre garde sa scrollbar USER32 ; une `ScrollBar` XAML
  est posée par-dessus dans le flush asynchrone, et la peinture USER32 est
  étouffée en filtrant `WM_NCPAINT` et en remplissant en noir
  (`SB:901-985`, `MW:6081-6198`). L'ancienne barre apparaît dès que USER32
  dessine hors de ces interceptions ou que la géométrie XAML est en retard.
- *Liste* : `wxListCtrl` est le générique ; seules les lignes visibles sont
  publiées en `ListViewItem` aux rectangles calculés par wx
  (`src/winui/listprojection.cpp:891-1047`).
- *Grille* : chaque cellule visible est un `Button` ou une `CheckBox` sur un
  `Canvas` (`src/winui/gridprojection.cpp:390-412`).
- *En-tête* : boutons XAML sur l'en-tête générique.

**DataView** : vraie implémentation sur `ListView` XAML
(`include/wx/winui/dataview.h:61-155`), arbre aplati en lignes avec
profondeur. Manquent : glisser-déposer, menu contextuel d'élément, lignes de
grille, hauteur de ligne variable, événements clavier. Les listes virtuelles
ne le sont pas : les N éléments sont énumérés (`src/winui/dataview.cpp:610-618`).
Rafraîchissement complet à chaque ajout, suppression, modification ou tri
(`dataview.cpp:1449-1484`), et à chaque défilement ou changement de sélection
(`dataview.cpp:346-356`, `2241-2298`).

**PropertyGrid** : superposition sur la grille générique, éditeurs natifs
pour six classes seulement. **AuiNotebook** : vrai `TabView`, mais pour les
en-têtes seulement, avec retour au GDI en multiligne. **Ribbon** : le rendu
natif des conteneurs n'est compilé qu'avec une option expérimentale
désactivée par défaut ; le build par défaut garde le chrome GDI.

**Aucun mécanisme ne rend la peinture wx dans une surface XAML.** Ni
`SurfaceImageSource`, ni `SwapChainPanel`, ni surface de composition.

**Modifications du code partagé** : `src/aui/auibook.cpp` +7 899 lignes,
`src/propgrid/propgrid.cpp` +6 386, `src/msw/window.cpp` +3 653,
`src/generic/datavgen.cpp` +3 297, `src/generic/grid.cpp` +1 966. Avec si peu
de marqueurs `__WXWINUI__` (18 et 13 pour les deux premiers), la plupart de
ces modifications sont inconditionnelles et touchent tous les ports (déduit).

## 3. Souris, capture, curseur, poignée de redimensionnement

**Abonnements.** Huit événements routés sur le `Canvas` racine avec
`handledEventsToo=true` (`TH:9652-9771`) **et** les huit mêmes sur chaque
slot (`TH:9371-9470`). Un mouvement au-dessus d'un contrôle exécute les deux.

**Coût d'un mouvement.**

- Au-dessus d'un contrôle XAML : une dizaine d'appels WinRT, un
  `wxMouseEvent` créé directement, puis le gestionnaire racine, puis une
  remontée des parents visuels croisée avec tous les slots pour le curseur
  (`TH:8939-8979`).
- Au-dessus d'un HWND générique ou d'un fond de panneau : un hit-test XAML
  (`TH:9862`), une résolution native qui alloue une région par fenêtre testée
  (`IR:70-93`), un premier `WM_NCHITTEST` (`IR:851`), un `WM_SETCURSOR`, puis
  **toute la résolution refaite** avec un second `WM_NCHITTEST` (`HI:1100`),
  enfin `SendMessage(WM_MOUSEMOVE)` et un `RedrawWindow` forcé (`HI:842-865`).
- Aucun regroupement : chaque `PointerMoved` est traité de façon synchrone.

**Livraison.** Les slots reçoivent des événements wx créés directement
(`HI:3261-3293`), pas des messages Win32. Aucun `wxContextMenuEvent` n'existe
dans `src/winui` : un clic droit sur un contrôle WinUI ne produit donc pas
`wxEVT_CONTEXT_MENU` (déduit).

**Le débordement de pile des dialogues.** Un mouvement sur la bordure résout
vers la fenêtre comme zone non cliente et envoie `WM_NCMOUSEMOVE`
(`HI:1161`). Microsoft.UI.Input relève aussitôt un `PointerMoved`, que
`OnRootPointer` route de nouveau, sans borne (`HI:62-67`). Gardes actuelles :
`IsEcho` exige l'égalité exacte des coordonnées (`HI:85-119`), qu'un écart
d'un pixel d'arrondi met en défaut ; le chemin rapide envoie `WM_NCHITTEST`
sans garde (`IR:851`) et il est devenu le chemin par défaut fin août, ce qui
colle avec la régression signalée le 30 août (déduit).

**Capture.** `CaptureMouse` fait `SetCapture` sur la coquille
(`MW:1079-1100`) ; `CapturePointer` n'est jamais appelé. La réconciliation se
fait en interrogeant `GetCapture()` à chaque événement. `IsMouseInWindow()`
utilise encore `WindowFromPoint` (`MW:2545`), qui renvoie l'île.

**La poignée.** Le clic poste un message, puis
`SendMessageW(tlw, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT)` (`HR:738`). La fenêtre
ne se redimensionne qu'au relâchement parce que le hook `WH_GETMESSAGE` donne
les messages souris de la boucle de redimensionnement à XAML et les annule
(`WU:2408-2414`, `2483-2490`). Le contournement actuel repasse les
mouvements par un message privé (`HR:95-242`). Le retour à la taille initiale
vient de `WM_CANCELMODE`, envoyé pour tout relâchement non reconnu
(`HR:515-523`).

**Tactile et stylet** : contact principal seulement, converti en bouton
gauche.

**Volume.** Environ 10 600 lignes, dont 3 300 de mécanisme et 6 000 de gardes
d'époque, de génération et de « tombstones ».

## 4. Compatibilité Win32 des HWND de contrôle

**État : rien n'existe.**

- La coquille de chaque contrôle WinUI est une fenêtre de classe
  `wxWindowNR`, créée par `wxControl::Create` → `CreateUsingMSWClass(nullptr)`
  (`src/msw/control.cpp:59-67`, `src/msw/window.cpp:762-811`).
  `MSWCreateControl` n'est jamais appelé depuis `src/winui`.
- Styles : seulement les génériques (`WS_CHILD`, `WS_TABSTOP`, `WS_VISIBLE`…).
  Aucune classe WinUI ne surcharge `MSWGetStyle` : pas de `ES_*`, `BS_*`,
  `CBS_*`, `LBS_*`.
- Message inconnu : `MSWHandleMessage` → `MSWDefWindowProc` →
  `::DefWindowProc` (`window.cpp:4440-4446`). `EM_SETCUEBANNER`, `BM_CLICK` et
  les autres renvoient 0 sans effet. Un commentaire le dit même en toutes
  lettres : `src/winui/combobox.cpp:3822`.
- Aucune surcharge de `MSWWindowProc`, `MSWHandleMessage`, `MSWCommand` ou
  `MSWOnNotify` dans `src/winui`.
- **Notifications sortantes** : les gestionnaires XAML créent les événements
  wx directement (environ 99 sites dans 30 fichiers, par exemple
  `src/winui/button.cpp:862-880`). Aucun `WM_COMMAND` ni `WM_NOTIFY` n'est
  jamais émis. `docs/winui/toolkit-contract.md:29-46` présente cela comme
  voulu, ce qui contredit l'exigence de compatibilité.
- **Entrée** : souris livrée en événements wx directs, clavier envoyé au HWND
  de l'île (`src/winui/ctrlhost.cpp:3576-3610`). Une surcharge applicative de
  `MSWWindowProc` compile mais ne voit ni `WM_LBUTTON*`, ni `WM_KEY*`, ni
  `WM_CHAR`, ni `WM_COMMAND`.

**Autres faits observables sur un HWND de contrôle.**

| Fait Win32 | Aujourd'hui |
|---|---|
| `GetWindowText` | Le libellé pour bouton, case, radio, bascule, boîte, lien, choix ; le **nom wx** (« text ») pour `wxTextCtrl` et `wxStaticText` |
| `GWL_ID`, `GetDlgItem` | Corrects |
| `GWL_STYLE` | Réel mais sans bits de contrôle |
| `GetClassName` | `wxWindowNR` |
| `IsWindowEnabled` / `IsWindowVisible` | Justes si le changement vient de wx ; un `::EnableWindow` direct n'atteint pas XAML |
| `GetWindowRect`, `SetWindowPos` | Justes, et déplacent bien l'élément XAML |
| `::GetFocus() == hwnd` | Faux : le focus est sur le HWND de l'île |
| `WM_GETFONT` | Renvoie 0 |
| `WM_SETREDRAW` | Sans effet sur XAML |
| `WM_CTLCOLOR*` | Jamais envoyé |
| `WM_CONTEXTMENU` | Jamais généré, sauf texte et lien |
| `WM_GETDLGCODE` | Sans `DLGC_BUTTON` ni `DLGC_HASSETSEL` |

**Vocabulaire à couvrir** (messages que wxMSW utilise lui-même, relevés dans
`src/msw`) : environ 320 messages entrants et 70 codes de notification, dont
190 entrants hors liste, arbre, en-tête et barre d'outils. La méthode wx
correspondante existe déjà pour presque tous.

| Effort | Familles |
|---|---|
| Petit | Bouton, bascule, case, radio, statiques, jauge, curseur, scrollbar, bouton fléché, barre d'état, lien, date, heure, calendrier |
| Moyen | Choix et combo (pas de HWND d'édition enfant), listbox, notebook, spin, texte simple (une trentaine de `EM_`) |
| Grand | Texte riche, barre d'outils, arbre, en-tête, liste |
| Transversal | Aller-retour des notifications et reflet de l'entrée sur la coquille |

**Point d'insertion** : `wxWindowMSW::MSWDefWindowProc`. En wxMSW classique
c'est là que le comportement natif est atteint (`CallWindowProc` vers la
procédure du contrôle Win32) ; sous WinUI ce pointeur est nul. Y brancher une
procédure de contrôle émulée conserve l'ordre exact pour les surcharges
applicatives et les sous-classements. Il n'y a pas de classe de base commune
aux contrôles WinUI ; un précédent de méthode virtuelle réservée à WinUI
existe dans la base partagée (`include/wx/msw/window.h:638-645`).

## 5. Focus, clavier, dialogues, runtime

Fichiers : `HF` = `src/winui/hostfocus.cpp`, `DP` = `src/winui/dlgpresenter.cpp`,
`DLG` = `src/msw/dialog.cpp`, `TOP` = `src/msw/toplevel.cpp`.

**Focus : pas d'autorité unique.** Avec un contrôle XAML actif,
`::GetFocus()` est le HWND d'entrée interne de l'île, pas la coquille
(`HF:1380-1385`). `SetFocus()` passe par `FocusSlotImpl`, une fonction de
777 lignes (`HF:1026-1802`) : `::SetFocus(île)` puis `Control.Focus()`.
`FindFocus()` renvoie un `m_focusOwner` mis en cache
(`TH:9882-9902`). Win32 et XAML sont réconciliés par des générations, une
pile de demandes et une quinzaine de verrous ; environ 930 lignes ne servent
qu'à conserver le focus lors d'un changement de fenêtre parente
(`HF:80-188`, `237-858`).

**Clavier.**

- Un hook `WH_GETMESSAGE` traite chaque message retiré de la file : chaîne
  clavier wx, puis `ContentPreTranslateMessage` ; un message traité est
  réécrit en `WM_NULL`. Aucune garde de réentrance (`WU:2399-2494`).
- **Aucun `wxEVT_KEY_DOWN`, `KEY_UP` ni `CHAR` générique pour les contrôles
  XAML.** Les touches vont au HWND de l'île ; le seul `wxEVT_KEY_DOWN`
  fabriqué dans `src/winui` est dans `treectrl.cpp:9883`. Une appli qui lie
  `wxEVT_KEY_DOWN` sur un `wxTextCtrl` ne reçoit rien.
- Dans une boucle native imbriquée, les gestionnaires d'accélérateurs,
  d'Entrée et d'Échap peuvent s'exécuter depuis le `GetMessage` de cette
  boucle, ce que wxMSW ne fait jamais (déduit).
- Tab : wx décide avant XAML en parcourant les coquilles
  (`CH:3391-3565`). Les arrêts de tabulation internes à un contrôle composite
  sont sautés dès qu'un HWND suivant existe (déduit).
- Les accélérateurs sont évalués deux fois pour les HWND natifs.

**Dialogues.**

- `wxDialog` n'est plus une fenêtre de classe dialogue : Entrée, Échap et
  mnémoniques sont réimplémentés (`DLG:95-358`).
- `wxTextEntryDialog` et `wxColourDialog` montrent bien le HWND public.
- **`wxMessageDialog` utilise deux fenêtres** : une cachée créée par le
  constructeur (`include/wx/winui/msgdlg.h:27`), et une seconde, privée,
  montrée par `ShowModal` (`src/winui/msgdlg.cpp:214-215`, `DP:1384-1402`).
- Entrée de barre des tâches : cause non prouvée. Trois candidats : dialogue
  sans propriétaire, propriétaire effacé (`DLG:877`), `AppWindow` créé pour
  toute fenêtre à titre (`src/winui/titlebar.cpp:798-817`).

**Vecteurs de gel et de crash.**

1. Un `Click` XAML distribue l'événement wx de façon synchrone
   (`src/winui/button.cpp:862-880`) : un `ShowModal` appelé dans le
   gestionnaire imbrique une boucle de messages **à l'intérieur d'un délégué
   XAML**, pendant que le hook rappelle XAML. Suspect principal des gels et
   crashs intermittents (déduit).
2. Chaque `HandleWindowEvent` incrémente une profondeur globale
   (`src/common/wincmn.cpp:1537-1543`). Tant qu'elle n'est pas nulle, toutes
   les destructions de fenêtres sont mises en file (`TOP:872`,
   `TH:2348-2426`). Pendant un dialogue modal ouvert depuis un gestionnaire,
   rien n'est donc détruit : les coquilles et les îles s'accumulent.
3. Le verrou `m_dismissQueued` du présentateur n'est jamais remis à zéro si
   la fermeture trouve un ancêtre en cours de destruction (`DP:1115-1168`,
   `1217-1237`), et les boîtes Oui/Non refusent la croix (`DP:1079-1085`) :
   gel modal permanent (déduit).
4. **Drapeau empoisonné** : le premier échec d'une garde de popup pose un
   drapeau global au processus qui fait échouer pour toujours
   `Initialize` / `SetContent` de tous les contrôles (`CH:2366`,
   `2409-2422`, `2590`). Les boîtes de message renvoient alors `wxID_CANCEL`
   sans s'afficher (`DP:1402-1406`). Les 58 `catch(...)` de
   `popupretirement.cpp` alimentent ce drapeau.
5. Les échecs internes sont signalés par `wxLogWarning`, donc par une boîte
   modale ouverte depuis ces mêmes chemins. Le message
   « HRESULT 0x80070005 » vient de `src/winui/textctrl.cpp:10716`, un
   `catch` général autour des écritures du champ texte.

**Retrait des popups** (`popupretirement.cpp`, 3 000 lignes) : retarde la
suppression du contenu XAML tant que le popup du ComboBox, du Flyout ou du
calendrier n'est pas fermé. La contrainte est réelle ; fermer le popup,
révoquer les abonnements et libérer l'élément dans un seul appel différé la
couvrirait en quelques dizaines de lignes (déduit).

**Runtime.** Démarrage : STA, amorçage du Windows App SDK, file de
dispatch, application XAML, ressources, moteur de rendu, hook
(`WU:2806-3158`). Deux allocations volontairement jamais libérées à chaque
démarrage (`WU:118-119`, `764-765`, `2816`) expliquent les fuites signalées à
la fermeture de `minimal`. Effets globaux : `LC_NUMERIC` laissé à `C`
(`WU:2160`), hook de thread, moteur de rendu remplacé, Mica sur toute
fenêtre.

**Volume.** 13 600 lignes, dont environ 3 500 de mécanisme et 8 500 de gardes
(retrait des popups 3 300, quarantaine du framework 1 400, migration du focus
1 700, registre des transitoires 800, durée de vie des dialogues 1 300).

## 6. État de WinUI et des bibliothèques (recherche web du 21 novembre)

- **Le port est épinglé sur Windows App SDK 1.8, qui n'est plus maintenu**
  depuis le 24 septembre 2026. La version stable courante est la **2.7.0**
  (17 novembre 2026), toujours compatible Windows 10 1809. La migration C++
  est annoncée comme un simple retrait puis rajout des paquets NuGet.
- Nouveautés stables utiles : contrôle `TitleBar` (1.7), `SplitMenuFlyoutItem`
  (1.8.6), **`SystemBackdropElement`** (2.0.1), qui permet Mica ou Acrylic à
  n'importe quel endroit de l'arbre XAML.
- **`TableView` natif de Microsoft** (`Microsoft.UI.Xaml.Controls.Tabular`),
  avec tri, filtrage, groupes, redimensionnement des colonnes et, depuis
  2.6.6, hiérarchie de lignes : **canal expérimental seulement**, avec
  l'avertissement de ne pas l'utiliser en production. Le réordonnancement des
  colonnes n'y est pas encore. Aucune date de passage en stable.
- Pas de ruban, de docking ni de grille de propriétés chez Microsoft.
- **`WinUI.TableView` (w-ahmad) et le Community Toolkit sont en C#** : ils
  exigent le runtime .NET dans le processus, ce qu'aucune source ne documente
  comme fonctionnel pour une bibliothèque C++ redistribuable.
- **Seule source native** de Ribbon, DataTable, TokenView : le portage C++
  « XamlToolkit » de lgztx96 (licence MIT, version 0.8.0-alpha, un seul
  mainteneur, compatible 1.8). C'est celui que Codex a utilisé.
- **Dessin applicatif dans XAML** : `SwapChainPanel` ne gère pas la
  transparence et rien ne peut se composer derrière lui ;
  `SurfaceImageSource` et les surfaces de composition ne sont pas sur cette
  liste (déduit de la documentation, non testé). Le dessin GDI exige une
  texture que l'on crée soi-même avec le drapeau de compatibilité GDI, puis
  que l'on recopie. Le comportement HDR n'est documenté nulle part : à
  mesurer.
