# Fiche 13 — Conteneurs génériques et UI auxiliaire

Baseline `20207bdbbb`. Ces classes n’ont pas de peer dédié sous `src/winui`,
mais elles restent dans le binaire via les sources common/generic/MSW. Elles ne
sont donc pas « gratuites » : le bridge top, l’hôte partagé et les changements
de dialogues modifient leurs contrats d’entrée, de dessin et de durée de vie.

## wxPanel — alpha fondation

Implémentation commune (`src/common/panelcmn.cpp`) sur le shell MSW. Elle porte
la navigation de nombreux composites et doit rester l’oracle des children wx.

- À faire : vérifier `wxTAB_TRAVERSAL`, focus initial, propagation enabled/show,
  background erase/paint sous le bridge, sizers et destruction/reparentage.
- Tests : panel vide, panel avec pairs XAML et GDI alternés, nested panels,
  disable parent, Tab/Shift-Tab, theme/DPI et 1 000 cycles create/destroy.
- Done alpha : aucune surface blanche, ordre focus stable, état effectif commun
  et aucune île additionnelle.

## wxScrolledWindow / wxScrolledCanvas — alpha fondation

Fallback générique (`src/generic/scrlwing.cpp`). Il déplace la vue logique et
alimente les cutouts/flush des slots.

- À faire : mesurer latence scroll; décider flush synchrone ou miroir seulement
  sur données; vérifier nested scrolling, `ScrollWindow`, wheel, keyboard,
  autoscroll/capture et recalcul des clips descendants.
- Tests : pairs XAML et custom windows partiellement visibles, scroll rapide
  horizontal/vertical/diagonal, nested scrolled windows, Freeze/Thaw, DPI/RTL,
  drag OLE avec autoscroll.
- Done alpha : aucun cisaillement au budget accepté, hit-test et focus suivent
  le contenu, pas de slot visible hors viewport.

## wxSplitterWindow — alpha gate visuelle

Fallback générique (`src/generic/splitter.cpp`). Le « trait blanc » rapporté doit
être validé par construction, pas présumé résolu.

- À faire : caractériser paint du sash et tracker face au bridge, vérifier que
  clip/cutout n’expose pas le fond du bridge, préserver capture et cursor resize.
- Tests : split vertical/horizontal, live update et tracker, double-click/
  unsplit, pane min/gravity, slot XAML de chaque côté, Light/Dark/High Contrast,
  DPI 100/150/200 et move inter-écrans.
- Done alpha : aucun trait parasite, sash interactif, aucun slot ne traverse la
  séparation pendant resize.

## wxSashWindow / wxSashLayoutWindow — beta

Fallbacks `src/generic/sashwin.cpp` et `src/generic/laywin.cpp`.

- À faire : capture hors fenêtre, hit-test/cursors sur quatre bords, tracker GDI,
  layout events, overlap de slots et DPI/theme.
- Tests : chaque edge, drag cancel/capture lost, nested sash, slot adjacent et
  reparentage. Garder post-beta si aucun sample V0 ne les promet.

## wxNativeWindow — exclu V0 par défaut

Wrapper MSW (`src/msw/nativewin.cpp`) autour d’un HWND externe. Son Z-order ne
peut pas être arbitrairement intercalé avec le plan XAML unique.

- Spike requis : attach/detach/subclass, focus/capture, destroy externe, HWND
  enfant vidéo/éditeur, overlap avec slot et UIA.
- STOP : si le HWND exige d’être au-dessus et au-dessous de slots différents,
  documenter la topologie non représentable; ne pas recréer plusieurs îles.

## wxProgressDialog / wxBusyInfo — beta

`wxProgressDialog` reste MSW (`src/msw/progdlg.cpp`); `wxBusyInfo` est générique
(`src/generic/busyinfo.cpp`). Ils exercent modalité, owner et fenêtres temporaires.

- À faire : intégrer au contrat owner/TransientManager sans changer leur API;
  vérifier pulse/update/cancel/skip, modeless, nested native loop, focus restore,
  busy cursor et destruction depuis callback.
- Tests : parent actif/inactif, minimize/Alt-Tab, deux TLW, cancel/veto, update
  rapide, shutdown et hot theme/DPI.
- Done beta : aucun lag owner, orphan ou loop bloquée; événements et valeurs
  suivent le backend MSW de référence.

## wxNotificationMessage / wxSplashScreen / wxTipWindow — beta ou post

Backends MSW/génériques (`src/msw/notifmsg.cpp`, `src/generic/splash.cpp`,
`src/generic/tipwin.cpp`). Ce sont des TLW/popups, pas des slots ordinaires.

- Notification : actions, timeout, close reason, Explorer restart, owner absent
  ou détruit; vérifier le backend réellement sélectionné par CMake.
- Splash : timer/close/click, bitmap DPI, startup/shutdown avant/après init XAML.
- TipWindow : show-without-activate, bounding rect, capture/dismiss, contenu long,
  multi-monitor et coexistence MenuFlyout/TeachingTip.
- Politique : beta seulement si l’API figure dans la promesse; sinon smoke et
  exclusion documentée post-beta.

## wxAuiToolBar / AUI MDI — post-beta sauf besoin produit

Implémentations génériques `src/aui/auibar.cpp` et `src/aui/tabmdi.cpp`.

- AuiToolBar : art provider, overflow, tool controls, drag/capture, floating,
  DPI/theme et interactions avec les slots.
- AUI MDI : activation pages, menus, focus, close/reparent et host unique.
- Gate : ne pas les confondre avec `wxToolBar` XAML ni le MDI Win32; exécuter un
  spike séparé après AUI floating/multi-TLW du plan 009.

## wxActiveXContainer — exclu V0

Le chemin OLE/ActiveX (`src/msw/ole/activex.cpp`) héberge une surface HWND/COM
étrangère. Avant promotion : activation in-place, focus/Tab/accelerators, D&D,
UIA, Z/composition, resize/DPI et teardown COM. Recommandation : exclure alpha et
beta tant qu’un cas produit n’impose pas son support.

## Plan d’exécution

1. Ajouter Panel/Scrolled/Splitter au harness infrastructure du plan 007c et à la
   checklist manuelle du plan 010.
2. Fermer Panel/Scrolled/Splitter avant alpha publique; enregistrer mesure scroll
   et captures visuelles du splitter.
3. Qualifier ProgressDialog/BusyInfo et UI temporaire seulement après le plan
   dialogues/transitoires.
4. Classer Sash, NativeWindow, AUI MDI/ToolBar et ActiveX selon besoin produit;
   aucun « support par héritage » sans smoke documenté.

## Critères de sortie

- Panel/Scrolled/Splitter ont tests automatiques ou harness reproductible et
  checklist alpha verte ;
- chaque classe auxiliaire est `Supported Beta`, `Experimental` ou `Excluded`,
  jamais implicitement supportée ;
- aucun fallback ne crée d’île supplémentaire ni ne contourne le broker D&D ;
- exclusions et topologies HWND non représentables sont publiées dans les docs.
