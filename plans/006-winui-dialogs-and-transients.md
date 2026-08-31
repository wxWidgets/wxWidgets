# Plan 006 — Stabiliser dialogues, surfaces transitoires et re-fit initial

> **Instructions** : choisir le default Window/Overlay explicitement avant de
> coder. Aucun commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/winui/dlgpresenter.cpp src/winui/msgdlg.cpp src/winui/textdlg.cpp src/winui/colordlg.cpp src/winui/richtooltip.cpp src/winui/menubar.cpp src/msw/toplevel.cpp src/msw/window.cpp include/wx/winui/winui.h`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`.
> Empreintes : dlgpresenter
> `74a384cfb66049dc1ea873671531ae5783f0c1db0611bb6006c8bb46cb3d812e`,
> toplevel
> `8c5820e7cd3847d9119db55f9e54aa6ed84f7db4c1a52a12db20b23fbab11016`,
> window
> `dc1dc97986cdbce45be6c557a0d6ec1b0301981ed22d48283dea1f6ba9ca7d5c`.
> Réconcilier toute différence avant édition.

## Statut

- **Priorité** : P0
- **Effort** : L
- **Risque** : HIGH
- **Dépend de** : 003, 004, 007b
- **Catégorie** : bug, architecture, layout, accessibility
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

La conversation a résolu des symptômes majeurs (starvation, lag owner, freeze,
crash TextDialog), mais le contrat global n’est pas qualifié : defaults
contradictoires, frame-class sans dialog manager, owner detach, stacking et
re-fit. Tous les dialogues partagent ces risques.

## État courant

- header public Window default, code Overlay (`include/wx/winui/winui.h:68`,
  `dlgpresenter.cpp:45-68`) ;
- frame-class contourne CreateDialog (`src/msw/toplevel.cpp:470-502`) ;
- owner detach à `src/msw/window.cpp:3205-3263` ;
- presenter crée directement ContentDialog et nested loop (`dlgpresenter.cpp`) ;
- Font/Find hints avant Loaded (`fontdlgg.cpp:511`, `fdrepdlg.cpp:206`) ;
- relayout différé ne grow pas TLW (`ctrlhost.cpp:692-713`) ;
- RichToolTip raw new/API partielle (`richtooltip.cpp:80-198`).

## Scope

Presenter/transient manager, TLW dialog integration, owner/priming, dialogs
WinUI/generic concernés, tests. Hors scope : refaire FileDialog MSW, component
TextCtrl complet, D&D.

## Étapes

1. **ADR default.** Créer
   `docs/winui/adr/0001-dialog-presentation.md`, choisir Window/Overlay et
   consigner contexte, options, décision, conséquences et rollback; aligner
   code/header/env/docs. Pour alpha, préférer Window tant que Overlay
   stacking/lifetime n’est pas vert.
2. **Tests frame-class.** Enter/Escape/default, Tab/Shift-Tab, mnemonic, initial
   focus, validation veto, restore focus, modal/modeless/nested.
3. **TransientManager par TLW.** State machine/generation/weak owner; un modal;
   popup/dialog/teaching tip enregistrés; shutdown/cancel centralisés; aucun loop
   si enqueue échoue.
4. **Owner transaction.** Tests minimize/Alt-Tab/grouping/nested/destroy; detach
   et restore exactement une fois. Priming vérifie IsWindow + generation + rect
   inchangé et ne pose le flag qu’après premier resize réussi.
5. **Presenter lifetime.** Aucun callback capture presenter/dialog brut après
   Destroy; détacher content et tokens selon ordre host. Corriger Accent style par
   Lookup try/catch, pas HasKey sur merged dictionaries.
6. **Grow-only refit.** Après Loaded/Measure stable, recalcul min/best et grandir
   une fois par dimension/generation; no shrink/no loop.
7. **Dialogs spécifiques.** Text validator NSDMI/tests, message button matrix,
   colour custom/ChooseFull, Font/Find generic.
8. **UIA.** Neutraliser shell invisible, vérifier roles/names/default/cancel et
   Narrator.

## Vérification

```powershell
cmake --build build-winui-clean --config Release --target test_gui widgets -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[modal]" --reporter compact
```

Exécuter widgets/dialogs en Window puis Overlay, 100 cycles, nested et owner
matrix. Attendu : aucune freeze/crash/lag/crop.

## Done

- [ ] default unique et documenté ;
- [ ] frame-class keyboard/focus parity ;
- [ ] un modal/TLW, shutdown/enqueue safe ;
- [ ] owner detach/minimize/Alt-Tab/nested/destroy verts ;
- [ ] Choose Font/Find jamais croppés, aucune boucle de layout ;
- [ ] message/text/colour tests verts ;
- [ ] UIA sans shells dupliqués ;
- [ ] Window et mode optionnel Overlay qualifiés selon promesse.

## STOP

- Les deux modes ne peuvent pas partager le même contrat public : documenter une
  limitation et demander décision produit.
- Re-fit oscille après deux générations.
- Une correction owner modifie le grouping d’autres TLW non-dialogues.
- La solution exige une attente bloquante DispatcherQueue ou `.get()`.
