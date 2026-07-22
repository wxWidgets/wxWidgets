# Plan 007a — Stabiliser pointer, hit-test, hover/capture et storms

> **Instructions** : caractériser avant de généraliser le breaker. Aucun commit
> sans validation explicite. Planifié sur
> `20207bdbbb81055da1f3e980b3271fc24566990e`.
>
> **Drift check** : comparer commit, index et worktree avec
> `git diff --stat 20207bdbbb..HEAD -- include/wx/winui/private/tlwhost.h src/winui/tlwhost.cpp src/msw/window.cpp tests/controls/windowtest.cpp samples/winuispike`, puis les mêmes paths via `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`.

## Statut

- **Priorité** : P0 infrastructure
- **Effort** : L
- **Risque** : HIGH
- **Dépend de** : 003
- **Débloque** : 005, 007b, 007c

## Pourquoi et état courant

`OnRootPointer()` (`tlwhost.cpp:1273+`) route island-first, mais
`FindNativeTargetAt()` (`:1668+`) ne reproduit pas tous les cas Win32;
HTTRANSPARENT remonte au parent (`:1373-1388`), le hover ne couvre pas
correctement non-client et XBUTTON/touch/pen ne sont pas contractualisés. Le
breaker `WM_MOUSEMOVE` (`:1298-1335`) ne traite qu’une position identique.

## Scope

`wxWinUITopLevelHost::OnRootPointer`, `FindNativeTargetAt`,
`BridgeSubclassProc`, `SendSlotMouseEvent`, `CancelIslandPointerState`, état de
capture/hover et instrumentation du spike. Hors scope : focus/Tab/cursor (007b),
Z/scroll (007c), OLE D&D (005), contrat interne d’un contrôle (008).

## Étapes et vérifications

1. **Harnais rouge déterministe.** Ajouter des sections WinUI conditionnelles à
   `tests/controls/windowtest.cpp` et des scénarios scriptables dans
   `samples/winuispike/winuispike.cpp` : MOVE même pixel, oscillation 2 px,
   ENTER/EXIT, client↔non-client, capture perdue, release hors fenêtre.
   **Verify :** chaque scénario a compteur/timeout borné et reproduit l’écart.
2. **Instrumentation opt-in et collecteur versionné.** La seule copie auditée du
   collecteur est actuellement ignorée dans
   `build-winui/collect-freeze.ps1`. Créer la version source
   `samples/winuispike/collect-freeze.ps1` avec paramètres `-BuildDir`,
   `-OutputDir` et `-Debugger`; défaut de sortie :
   `<BuildDir>/logs/winui-freeze/<timestamp>`. Supprimer les chemins absolus
   cdb/symboles et faire pointer ring/histogrammes/traces vers ce même run-id,
   sous flag/env documenté. **Verify :** deux runs identiques flag off/on, le
   premier silencieux; le script source fonctionne depuis deux build dirs.
3. **Hit-test Win32 fidèle.** Factoriser une résolution deepest-window qui
   respecte hidden/disabled, client/non-client et la sémantique sibling de
   HTTRANSPARENT; retourner une identité génération-safe réutilisable par 005.
   **Verify :** siblings superposés et child transparent ciblent le même HWND que
   le backend MSW de référence.
4. **Hover et capture.** Gérer TrackMouseEvent client/non-client,
   `WM_MOUSELEAVE`/`WM_NCMOUSELEAVE`, `WM_CAPTURECHANGED`/`WM_CANCELMODE`, double
   clic et coordonnées écran/client. **Verify :** une seule paire enter/leave et
   aucun pressed state après capture loss/destroy.
5. **Parité pointer.** Ajouter XBUTTON et horizontal wheel; choisir et documenter
   mapping touch/pen (wx pointer ou souris synthétique), jamais les deux.
   **Verify :** événements, boutons/modifiers et propagation égaux à wxMSW.
6. **Breaker généralisable seulement sur preuve.** Distinguer message réellement
   bloqué, répétition bénigne et mouvement réel. Toute suppression porte un
   compteur/raison. **Verify :** storms finissent, mais un trajet réel de même
   longueur livre tous les changements significatifs.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui winuispike -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[winui-input-router]" --reporter compact
```

Exécuter ensuite le scénario spike 100 fois et un soak manuel d’une heure; noter
compteurs, durée et hash du binaire dans le rapport.

## Done

- [ ] mêmes cibles/coordonnées que wxMSW pour la matrice hit-test ;
- [ ] enter/leave/capture/double-click sans doublon ni état bloqué ;
- [ ] XBUTTON/hwheel et politique touch/pen testés ;
- [ ] aucune variante de storm déterministe ne bloque ;
- [ ] breaker n’altère pas le mouvement normal ;
- [ ] API de résolution réutilisable par le DropBroker ;
- [ ] instrumentation opt-in et tests verts.

## STOP

- Une correction nécessite une seconde île ou un second bridge.
- Après trois reproductions instrumentées, les signatures diffèrent sans
  invariant : conserver le harness et rapporter, ne pas élargir l’heuristique.
- La résolution retient un `wxWindow*` au-delà de sa génération/destroy.
