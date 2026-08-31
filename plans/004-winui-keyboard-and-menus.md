# Plan 004 — Restaurer les accélérateurs et le contrat des menus

> **Instructions** : préserver la saisie AZERTY/AltGr/dead-key réparée en 2026;
> un fix qui rétablit Ctrl-T mais réintroduit double frappe est invalide. Aucun
> commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/msw/evtloop.cpp src/msw/frame.cpp src/msw/menu.cpp src/winui/winui.cpp src/winui/ctrlhost.cpp src/winui/menubar.cpp`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`.
> Empreintes : evtloop
> `3cbae3bfa6cbdbb7bb1a0f56c77e0684044754e8021353b700df86ec64505984`,
> winui
> `6b4cda54b41165bbbb45bf7686c90edc23cee8a434765e233b949b32af863018`,
> menubar
> `2034344960b6518ca129dcb50b18c29c8f2f0cc6414996673048f257c03e5895`.
> Réconcilier toute différence avant édition.

## Statut

- **Priorité** : P0
- **Effort** : M
- **Risque** : HIGH
- **Dépend de** : 003
- **Catégorie** : bug, input, compatibility
- **Planifié à** : `20207bdbbb`, 2026-07-21
- **État** : `IN REVIEW` — implémentation, build et revues terminés ; le
  blocage `CodexSandboxOffline`/`MddBootstrapInitialize2()` documenté en juillet
  est levé. La signature release reste liée au rejeu de la campagne physique
  clavier/menu sur le candidat RC3 gelé et aux gates humaines transversales.

## Pourquoi

Les tables d’accélérateurs existent, mais XAML consomme et neutralise le message
avant `wxFrame::MSWDoTranslateMessage()`. Les menus souris semblent fonctionner
alors que les raccourcis et plusieurs contrats OPEN/command routing sont cassés.

## État courant

- table construite : `src/msw/menu.cpp:1272-1311` ;
- translation wx : `src/msw/frame.cpp:771-780` ;
- hook XAML d’abord, WM_NULL ensuite : `src/winui/winui.cpp:336-361` ;
- ProcessMessage suppose le hook pour non-Tab : `src/msw/evtloop.cpp:126-144` ;
- popup construit avant OPEN : `src/winui/menubar.cpp:531-560` ;
- commandes/open/close envoyés directement au frame, au lieu du chemin canonique
  `src/common/menucmn.cpp:679+` ;
- `TryEnqueue()` ignoré avant nested drain : `menubar.cpp:578-595`.

## Scope

Evtloop/frame/menu/winui/ctrlhost/menubar et tests input/menu. Hors scope : D&D,
dialog manager complet, component TextEntry APIs autres que non-régression.

## Étapes

1. **Ajouter une matrice de tests clavier rouge.** Focus TextBox, ComboBox,
   Button XAML et Grid générique; Ctrl/Alt/F-key, disabled accelerator, conflict
   local, Tab, printable, AltGr et dead keys. Inclure boucle wx et boucle native.
2. **Définir un pipeline unique.** Extraire une fonction qui tente uniquement
   filtres/accelerator/mnemonic wx sans le dialog preprocessing qui avalait le
   texte. L’appeler avant `ContentPreTranslateMessage` dans le hook; marquer
   consommé une seule fois.
3. **Fallback hook.** Si SetWindowsHookEx échoue, ProcessMessage exécute le même
   pipeline et XAML pretranslate; aucune divergence d’ordre.
4. **Frontière Tab/navigation.** Préserver le comportement actuel et ajouter les
   non-régressions minimales; le propriétaire de l’arbitre focus et de la chaîne
   Tab mixte est exclusivement le plan 007b, exécuté après ce pipeline.
5. **Routage menu canonique.** OPEN avant build/refresh; command via menu helper;
   CLOSE avec bon `wxMenu*`; mutations effectuées dans OPEN visibles.
6. **États/présentation.** Enabled du top/submenu, check/radio, access keys,
   label/bitmap/accelerator à chaque ouverture; métriques DPI.
7. **Drain sûr.** N’entrer dans la boucle que si TryEnqueue réussit; weak TLW,
   fermeture/shutdown bounded.

## Vérification

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets showcase -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[accelentry]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[menu]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[KeyboardEvent]" --reporter compact
```

Ajouter un test WinUI ciblé Ctrl-T et une matrice AZERTY. Attendu : un seul
command event ou un seul caractère, jamais les deux/double.

## Done

- [ ] Ctrl-T et tous accélérateurs passent depuis chaque focus — matrice
  TextBox/ComboBox/Button/Grid compilée, gate physique finale à rejouer sur le
  candidat RC3 gelé ;
- [ ] printable/AltGr/dead-key sans double frappe — seams déterministes verts,
  saisie TextBox physique finale à rejouer sur le candidat RC3 gelé ;
- [ ] Tab mixte stable — frontière minimale couverte ici, parcours approfondi
  explicitement propriétaire du plan 007b et gate runtime à rejouer ;
- [ ] OPEN mutation visible et handlers menu→menubar→frame corrects — code et
  tests relus, gate AutomationPeer à signer sur le candidat RC3 gelé ;
- [ ] disabled menu/accelerator inactifs — HACCEL structurel vert, projection
  XAML à rejouer hors sandbox ;
- [ ] popup shutdown/destroy sans freeze — boucles bornées et tests compilés,
  exécution WinUI finale à rejouer sur le candidat RC3 gelé ;
- [x] boucle native et fallback hook couverts par les seams de production.

## STOP

- Le seul fix semble nécessiter KeyboardAccelerator XAML dupliquant toutes les
  tables wx : arrêter et comparer l’option pipeline wx avant duplication.
- Une touche génère deux commandes ou deux caractères.
- Le fix dépend de la disposition clavier locale du développeur; ajouter un test
  injectable avant de continuer.
