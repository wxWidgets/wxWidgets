# Validation de la liste proposée des restes

## Verdict global

La liste est une bonne photographie des travaux de la dernière session, mais ce
n’est pas encore une backlog complète de V0. Les points 1, 2, 6–10 et 12 sont
réels; les points 3–5 sont des portes de validation; 11 est préventif; 13 exige
une décision ABI; 14 décrit seulement le worktree post-HEAD, pas tout le port.

## Revue point par point

| # | Verdict | Correction ou précision | Priorité révisée |
|---:|---|---|---:|
| 1 | **Confirmé** | Aucune occurrence de `IDropTarget` ou `RegisterDragDrop` dans `src/winui`. Le drag TreeCtrl de `e69a11f706` est un geste interne, pas le contrat OLE `wxDropTarget`. | P0 alpha |
| 2 | **Bug confirmé, diagnostic à corriger** | La table est déjà créée par `wxMenu::UpdateAccel()` et traduite par `wxFrame::MSWDoTranslateMessage()`. Le hook XAML neutralise le message avant ce chemin; ne pas dupliquer la table dans `menubar.cpp`. | P0 alpha |
| 3 | **Partiellement confirmé** | La limite de bande TabView est déjà codée (`ctrlhost.cpp:623-631`, `tlwhost.cpp:2046-2061`). Il reste à la valider. Latence de scroll, Z mixte, showcase et splitter restent ouverts. | P1 |
| 4 | **Confirmé avec nuance** | Reparentage croisé, doublons UIA, Narrator et stress multi-TLW restent. Le priming a été réévalué mais est redevenu central après chaque changement d’owner; il faut le rendre sûr ou l’éliminer. | P1 |
| 5 | **Confirmé** | La checklist finale n’a pas été déroulée après les derniers changements. Les échecs `test_gui` renforcent cette nécessité. | P1 gate |
| 6 | **Confirmé** | Minimize, Alt-Tab, propriétaires empilés et destruction pendant detach ne sont pas couverts. Le code actif est dans `src/msw/window.cpp:3205-3263`. | P0 dialogues |
| 7 | **Confirmé** | Le chemin frame-class contourne `CreateDialog()` dans `src/msw/toplevel.cpp:470-502`; l’équivalence Enter/Escape/default/focus n’est pas prouvée. | P0 dialogues |
| 8 | **Tâche valide, cause encore hypothétique** | Le relayout différé relance les sizers mais n’agrandit pas la TLW. Faire un re-fit grow-only après mesure stabilisée, avec garde anti-boucle. | P0/P1 |
| 9 | **Confirmé** | Le breaker couvre uniquement une longue série de `WM_MOUSEMOVE` à la même position (`tlwhost.cpp:1298-1335`). ENTER/EXIT ou positions oscillantes restent à stresser. | P1 |
| 10 | **Confirmé** | Les slots animés en continu peuvent encore réimposer la flèche. Prévoir réassertion post-resync ou limite documentée. | P2 alpha, P1 beta |
| 11 | **Préventif** | Le crash `wxTextEntryDialog` a une correction NSDMI dans le worktree. Pour choice/combo/slider/radio/button, aucun crash spécifique n’est démontré et les constructeurs complets délèguent; ajouter les NSDMI reste souhaitable. | P2 |
| 12 | **Confirmé** | Le probe 165 fps/histogramme du spike est toujours actif; chemins de log non unifiés. Tout instrumentation doit devenir opt-in avant commit. | P1 livraison |
| 13 | **Pas un reste du plan fourni** | Aucun livrable namespace n’apparaît dans `iridescent-snacking-oasis.md`. Faire coexister `wxWinUI::` et `wxMSW::` touche ABI, RTTI et sélection des sources; écrire une ADR avant tout renommage. | P3 post-V0 |
| 14 | **À reformuler** | Le port compte 61 commits. Seule la refonte depuis `20207bdbbb` est non commitée. La découpe doit inclure infra/build, host, input, dialogues, freeze, curseur, composants et spike, avec validation utilisateur avant chaque commit. | P1 livraison |

## Trous supplémentaires découverts par l’audit

### P0 — bloquants avant alpha

1. **Ressources runtime absentes d’un build frais.**
   `build/cmake/winui.cmake:249-263` ne déploie que le bootstrap DLL; les PRI et
   arbres XAML présents dans l’ancien build ne sont pas reproduits.
2. **Rupture API `wxInfoBar`.** `include/wx/winui/infobar.h` omet le style
   checkbox, setters/getters d’effets, état checked et overrides d’apparence;
   seule une partie de ces absences apparaît dans les premières erreurs XRC/tests.
3. **Durée de vie des handlers de slot.** `BindSlotEvents()` ajoute des handlers
   capturant `this`, `window` et parfois le `container`, sans stocker/revoquer
   leurs delegates lors de `UnregisterSlot()`.
4. **`wxWinUIXamlHost` contourne l’architecture.** Il crée son propre
   `DesktopWindowXamlSource` (`src/winui/xamlhost.cpp:54-59,128-152`) et vide le
   contenu avant `Close()` (`:77-90`), contrairement aux invariants du host.
5. **État enabled non centralisé.** La synchro de slot copie géométrie,
   direction et visibilité, mais pas `IsEnabled`; seuls quelques contrôles ont
   un `DoEnable()` spécifique.
6. **Échec silencieux du hook clavier.** Le retour de `SetWindowsHookExW()`
   n’est pas contrôlé (`src/winui/winui.cpp:474-485`).

### P1 — bloquants beta ou robustesse élevée

7. **Contrat menu incomplet.** OPEN est émis après construction du flyout,
   commandes et open/close ne suivent pas le chemin menu→menubar→frame, et les
   sous-menus désactivés de la barre peuvent rester actifs.
8. **Freeze possible au drain de popup.** `TryEnqueue()` est ignoré avant une
   boucle imbriquée (`src/winui/menubar.cpp:578-595`).
9. **Fuites/risques transitoires.** `wxRichToolTip` alloue son host à nu et
   ignore une partie de l’API; aucun manager par TLW ne sérialise tous les
   transitoires.
10. **Z-order dynamique non représenté.** Les slots suivent un parcours de
    création, pas `Raise()`/`Lower()` ni une vraie intercalation HWND/XAML.
11. **Tests composants déjà rouges.** Voir
    [verification.md](verification.md) : TextCtrl, ListBox, ComboBox, Slider,
    SpinCtrl, TreeCtrl, Notebook et d’autres échouent avant même la checklist
    manuelle.
12. **Tooltips incomplets.** Le chemin natif est court-circuité pour les slots,
    mais l’infrastructure commune ne reporte pas le tooltip sur tous les
    éléments XAML.

## Ordre recommandé corrigé

1. Build frais + ressources + InfoBar + test gate (plans 001–002).
2. Durée de vie/invariants du host et `wxWinUIXamlHost` (plan 003).
3. Accélérateurs et menus (plan 004).
4. Pointer/hit-test/freeze (007a), puis D&D OLE (005).
5. Focus/Tab/cursors (007b), puis dialogues/transitoires/re-fit (006).
6. Z-order/scroll/splitter (007c).
7. Contrats composants P0/P1 par sous-plans 008a–008f.
8. UIA/DPI/RTL/multi-TLW puis checklist finale (plans 009–010).
