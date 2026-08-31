# Plan 008a — Stabiliser TextCtrl/SearchCtrl et le contrat TextPeer

> **Instructions** : le lifetime/crash TextCtrl est la première gate; aucune
> extension Search tant qu’elle n’est pas verte. Aucun commit sans validation.
>
> **Drift check** : `$scope=@('include/wx/winui/textctrl.h','include/wx/winui/srchctrl.h','src/winui/textctrl.cpp','src/winui/srchctrl.cpp','src/winui/private.h','tests/controls/textctrltest.cpp','tests/controls/textentrytest.h','tests/controls/searchctrltest.cpp'); git diff --stat 20207bdbbb..HEAD -- $scope; git diff --stat -- $scope; git diff --cached --stat -- $scope; git status --short -- $scope`. Réconcilier toute dérive.

## Statut

- **Priorité** : P0 alpha
- **Effort** : L
- **Risque** : HIGH lifetime/réentrance
- **Dépend de** : 003, 004, 007b
- **Débloque** : alpha développeur, 008b et editors génériques

## État et scope

La suite TextCtrl montre environ 26 échecs et des sorties prématurées hors
debugger. `textctrl.cpp:349-364,505-646,692-715` concentre caret, dirty,
max-length et hit-test. SearchCtrl garde des no-op TextEntry/menu/cancel
(`srchctrl.cpp:81-180,231-313`). Le template part ne doit jamais survivre à sa
génération.

Scope : TextCtrl, SearchCtrl et, si l’extraction est justifiée par les tests,
`include/wx/winui/private/textpeer.h`, `src/winui/textpeer.cpp` et leur entrée
dans `build/cmake/lib/core/CMakeLists.txt`. Hors scope : ComboBox (008b),
dialogs TextEntry (006) hors non-régression, RichText/STC.

## Étapes et vérifications

1. Reproduire TextCtrl 100 fois hors debugger puis sous cdb/Application Verifier;
   journaliser exit, exception, dernier test et stacks. **Verify :** repro stable
   ou absence de crash sur 100 runs avec instrumentation lifetime.
2. Auditer/revoquer tous tokens/callbacks et template parts au destroy/re-template;
   utiliser generation/weak state du plan 003. **Verify :** aucun callback après
   destroy et compteurs COM revenus à zéro.
3. Corriger valeur/dirty/selection/caret, readonly, max length, clipboard,
   undo/redo, lignes/position↔coordonnées et events user vs programmatique.
   **Verify :** chaque sous-section de `[TextCtrlTestCase]` devient verte sans
   changer le test oracle.
4. Valider IME/AltGr/dead keys et Enter/Tab via pipeline 004/007b.
5. SearchCtrl : ownership menu, vrai texte/selection/caret, suggestions, search/
   cancel seulement sur action valide, destruction/replacement sûrs.
6. Ajouter tests default constructor + `Create()` et NSDMI pour tout état lu
   avant Create; ne pas masquer un callback lifetime par seule initialisation.
7. Étendre le smoke XRC mixte pour TextCtrl/SearchCtrl et détruire la ressource.

## Commandes

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[TextCtrlTestCase]" --reporter compact
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[wxSearchCtrl]" --reporter compact
```

## Done

- [ ] 100 runs TextCtrl sans crash/sortie prématurée ;
- [ ] aucun callback/template part après génération ;
- [ ] capacités TextCtrl alpha et SearchCtrl promises vertes ;
- [ ] IME/AltGr/dead keys et Tab sans régression ;
- [ ] menu Search détruit exactement une fois ;
- [ ] default+Create et XRC smoke verts.

## STOP

- Crash non localisé après instrumentation cdb/Application Verifier : conserver
  le repro et demander décision ASan/diagnostic, ne pas corriger au hasard.
- L’adaptateur oblige à exposer WinRT dans un header public.
- Une capacité rich text élargit la promesse V0.
