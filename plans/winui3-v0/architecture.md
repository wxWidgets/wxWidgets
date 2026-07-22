# Architecture recommandée pour une base WinUI 3 solide

## Position de départ

Le choix « une île XAML par top-level window » est le bon axe pour ce port : il
réduit les fragments de composition, permet un arbre UIA cohérent et rend les
surfaces transitoires capables de couvrir tout le client. En revanche, la V0 ne
doit pas prétendre que tous les widgets sont natifs. CMake compose déjà un port
hybride (`build/cmake/lib/core/CMakeLists.txt:21-121`) : MSW commun, contrôles
WinUI ciblés, fallbacks génériques et renderer Fluent.

La recommandation est de formaliser ce modèle hybride au lieu de le masquer.

## Invariants non négociables

1. **Une source XAML par TLW.** Aucun contrôle, tooltip, popup ou helper public
   ne crée un `DesktopWindowXamlSource` supplémentaire dans la même TLW.
2. **Le HWND wx reste l’identité publique.** `GetHandle`, sizers, sous-classement,
   events et compatibilité MSW continuent de s’appuyer sur le shell.
3. **Le slot est l’unité de synchronisation.** Géométrie, visibilité, enabled,
   RTL, tooltip, curseur, UIA, focus target et génération lifetime sont gérés au
   même endroit.
4. **Un seul propriétaire de chaque callback.** Chaque delegate/event token est
   stocké dans un revoker RAII et révoqué avant retrait du contenu.
5. **Win32 est l’autorité du focus logique; XAML suit.** L’arbitre publie un état
   final par slot, pas une paire Got/Lost transitoire pour chaque template part.
6. **Un message clavier n’est consommé qu’une fois.** Ordre explicite : filtres
   application → accélérateurs/mnémoniques wx → navigation mixte → XAML text
   input → dispatch Win32.
7. **Un seul transitoire modal par TLW.** Menus, ContentDialog et TeachingTip
   non modal partagent un registre; aucun nested loop ne dépend d’un callback
   dont l’enqueue peut échouer.
8. **DIPs et pixels portent un type/nom explicite.** Une valeur passée à XAML est
   en DIPs; `FromDIP()` ne doit pas être réappliqué avant Width/Margin/Padding.
9. **Les fallbacks sont des citoyens supportés.** Un contrôle générique GDI sous
   le bridge doit conserver entrée, capture, scroll, D&D, focus, UIA et thème.
10. **Aucune annonce de complétude sans gate.** Un composant est Native-Stable
    seulement si son contrat ciblé et ses tests alpha/beta sont verts.

## Décomposition interne proposée

Garder `wxWinUITopLevelHost` comme façade privée, mais séparer les
responsabilités après ajout des tests de caractérisation :

```text
wxWinUITopLevelHost (façade, registre TLW)
├── HostLifetime       création, close, generations, reparentage
├── SlotRegistry       wxWindow ↔ Grid/content, state adapter, revokers
├── GeometrySync       rects, clips, scrollbars, DPI, Z signature
├── InputRouter        hit-test XAML/native, pointer, capture, hover
├── FocusArbiter       focus target, Tab, logical wx focus
├── DropBroker         IDropTarget bridge et transitions de cible
├── TransientManager   popup/dialog/tooltip et modal loops
├── BackdropPolicy     Mica, priming, owner transitions
└── AccessibilityMap   Name/Role/state et neutralisation des shells
```

Ne pas scinder immédiatement `tlwhost.cpp` uniquement pour réduire ses 2 458
lignes : commencer par les tests, puis extraire un sous-objet à la fois sans
changer le comportement. `InputRouter` et `DropBroker` doivent être séparés
conceptuellement même s’ils partagent le hit-test.

## Adaptateurs communs à créer

### `SlotStateAdapter`

Entrées : `wxWindow*`, slot et peer racine/focus cible. Sorties synchronisées :

- `Visibility` et `IsEnabled` effectifs, y compris parent désactivé ;
- `IsHitTestVisible`, `IsTabStop` et focusability ;
- `FlowDirection`, thème et scale generation ;
- tooltip, curseur et nom/rôle UIA ;
- invalidation best-size et refresh bitmap sur DPI.

Cela remplace les dizaines de `DoEnable`, tooltip et font handlers divergents.
Les contrôles gardent un override seulement lorsque la valeur wx ne correspond
pas directement à la propriété du root.

### `TextPeerAdapter`

À partager par `wxTextCtrl`, `wxSearchCtrl` et `wxComboBox` :

- obtention/revalidation du TextBox template part ;
- texte, selection/caret, readonly, clipboard, undo/redo, max length ;
- distinction mutation utilisateur/programmatique et suppression de doublons ;
- IME/AltGr/dead keys, Enter/Tab ;
- invalidation du template part à chaque re-template.

Ce module ne doit jamais retenir un template part au-delà de sa génération.

### `ItemModelAdapter`

À partager par Choice/Combo/List/CheckList/BitmapCombo :

- identité stable d’item, texte, client data, bitmap, checked, selected ;
- comparateur wx unique pour `wx*_SORT` ;
- deltas insert/delete/update au lieu de `Items().Clear()` ;
- mapping sélection simple/multiple et événements Added/Removed ;
- ownership `wxClientData` exactement une fois.

Ce modèle élimine plusieurs bugs aujourd’hui indépendants mais de même cause.

### `WinUIAppearanceAdapter`

Centraliser conversion font family/weight/style/size, foreground/background,
ThemeResource, label/mnemonic/AccessKey, bitmap bundle et reset des propriétés
custom. Ne pas dupliquer des conversions légèrement différentes dans Button,
CheckBox, RadioButton, StaticText, etc.

## Input et Z-order

Le bridge-top/island-first est une contrainte physique constatée, mais il faut
documenter ses limites : un seul HWND de bridge ne peut pas représenter une
intercalation arbitraire XAML↔HWND. Pour la V0 :

- déclarer que les slots XAML constituent un plan unique au-dessus du GDI ;
- reproduire l’ordre relatif entre slots depuis le vrai sibling order, pas la
  seule création ;
- interdire ou détecter les overlaps qui exigent un HWND natif au-dessus d’un
  slot XAML ;
- créer un test explicite `Raise/Lower` et un diagnostic debug lorsqu’une
  topologie non représentable apparaît.

Ne pas multiplier les bridges pour contourner ce problème avant beta : cela
réintroduirait exactement les problèmes que la refonte supprime.

## Dialogues et surfaces transitoires

Créer un `TransientManager` par TLW avec une petite machine d’état :

```text
Idle → PopupOpen → Closing → Idle
Idle → ModalOpening → ModalOpen → ModalClosing → Idle
Idle → TeachingTipOpen → Closing → Idle
```

- Toute transition porte une generation et un `wxWeakRef`/identité TLW.
- `TryEnqueue(false)` annule proprement; aucune boucle imbriquée n’est démarrée.
- Destruction TLW force `Closing` sans callback vers wx détruit.
- Les dialogues empilés sont refusés, sérialisés ou explicitement mis en file.
- `Window` doit rester le défaut alpha si le chemin Overlay n’a pas passé les
  gates stacking/focus/lifetime; changer ce défaut seulement par ADR.
- Le re-fit grow-only s’exécute une fois après Loaded/Measure stable, par
  génération, sans shrink ni boucle.

## Build, packaging et supply chain

La fonction actuelle configure surtout une bibliothèque. Il faut distinguer :

- `wx_winui3_configure_library(target)` : includes, C++/WinRT, link flags ;
- `wx_winui3_deploy_runtime(target)` : bootstrap DLL, runtime/PRI/XBF/assets,
  architecture/configuration et manifest/identity éventuel ;
- `wx_winui3_add_smoke_test(target)` : lancement depuis output propre, création
  `XamlControlsResources`, Button, TreeView, TabView, puis fermeture.

Ajouter SHA-256 des nupkg, téléchargement atomique et choix numérique d’un SDK
unique. Le runtime test doit échouer si une ressource manque, pas continuer avec
des contrôles sans template.

## Tests comme contrat de support

Pour chaque composant, définir trois niveaux :

| Niveau | Exigence |
|---|---|
| Native-Experimental | compile et apparaît dans spike; aucune promesse API |
| Native-Alpha | zéro crash/freeze; opérations principales et tests alpha verts; limites listées |
| Native-Beta | suite upstream ciblée verte, UIA/DPI/RTL/theme/lifetime et interactions mixtes qualifiées |

Une exclusion de test n’est acceptable que si la capacité est explicitement
hors V0 dans la matrice et si l’API échoue clairement ou utilise un fallback;
pas si elle renvoie silencieusement une valeur fausse.

Ajouter des tests d’infrastructure indépendants des composants :

- register/unregister 10 000 slots avec weak-ref finalisé ;
- destroy/reparent pendant callback/enqueue ;
- disable parent/child et UIA Invoke ;
- Tab et accélérateurs dans boucles wx/native ;
- drag OLE entre slots et HWND générique ;
- move/resize/freeze/owner detach sous message storm ;
- DPI 100/150/200, RTL et hot theme ;
- UIA tree sans shells dupliqués.

## Stratégie de migration des composants

### Alpha

Ne migrer aucun nouveau gros contrôle. Stabiliser les composants déjà natifs,
et conserver ListCtrl/Header/DataView/Grid/Font/Find comme fallbacks. Exclure MDI,
Media, GL, WebView, Ribbon et STC de la promesse alpha sauf smoke documenté.

### Beta

Qualifier la coexistence des fallbacks génériques, AUI floating/reparenting,
books composites, pickers et rich content prioritaire. Une migration native ne
doit commencer que si elle apporte un bénéfice produit mesurable et possède une
suite de contrat.

### Post-beta

ItemsView natif pour data controls, surfaces WebView/Media/GL, Ribbon, MDI et
namespaces side-by-side. Chacun requiert son propre spike/ADR.

## Namespaces `wxWinUI::` / `wxMSW::`

Ce n’est pas un simple packaging. Les deux backends définissent aujourd’hui les
mêmes classes globales, RTTI wx et symboles ABI; CMake retire les sources MSW
quand les sources WinUI équivalentes sont activées. Avant tout changement :

1. préciser le cas d’usage : coexistence dans un même binaire, API interne de
   backend, ou simples helpers ;
2. inventorier symboles/RTTI/macros de sélection et ABI DLL ;
3. tester une classe pilote sans dupliquer `wxClassInfo` ni événements ;
4. décider compatibilité source/binaire et politique upstream.

La recommandation V0 est de garder les classes wx publiques inchangées et de
placer uniquement les helpers privés nouveaux dans `wxWinUI`/`wxMSW` si une ADR
le justifie.

