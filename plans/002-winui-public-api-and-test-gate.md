# Plan 002 — Restaurer l’API publique InfoBar et la porte de tests WinUI

> **Instructions** : aucune exclusion globale pour faire « vert ». Corriger le
> contrat public, conserver les échecs composants comme backlog visible. Aucun
> commit sans validation utilisateur.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- include/wx/winui/infobar.h src/winui/infobar.cpp src/xrc/xh_infobar.cpp tests/controls/infobar.cpp build/cmake/tests/gui`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`.
> Empreintes : header
> `dd2e823363d646580c5085b6a9d72d15c45782e82d5df180e8c3eb92d860d274`,
> source `f63f6c77dc0d3d78cf649a785586fef29ff1e13eaae68a867c4a6a850035f6ab`.
> Réconcilier toute différence d’empreinte, d’index ou de worktree avant édition.

## Statut

- **Priorité** : P0
- **Effort** : M
- **Risque** : LOW
- **Dépend de** : plan 001
- **Catégorie** : bug, tests, API compatibility
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

Le build standard s’arrête dans XRC, et même sans XRC le test InfoBar ne compile
pas. Le port expose un header plus petit que le contrat public commun; ce n’est
pas une limitation cosmétique mais une rupture source.

## État courant

Manquent dans `include/wx/winui/infobar.h:19-56` :

- enum/style `wxINFOBAR_CHECKBOX` ;
- `SetShowHideEffects()` ;
- `SetEffectDuration()` ;
- `ShowCheckBox()` ;
- `GetShowEffect()`, `GetHideEffect()` et `GetEffectDuration()` ;
- `IsCheckBoxChecked()` ;
- le comportement spécialisé de `SetFont()`, `SetForegroundColour()` et du
  border par défaut exposé par la version générique.

Usages obligatoires : `src/xrc/xh_infobar.cpp:40-90` et
`tests/controls/infobar.cpp:25-28`. La version générique sert d’exemple :
`include/wx/generic/infobar.h:24-91`, `src/generic/infobar.cpp:495+`.

## Scope

**Dans le scope :** header/source InfoBar WinUI, tests InfoBar, adaptation XRC si
nécessaire sans réduire l’API, test manifest/gates WinUI.

**Hors scope :** tous les autres contrats composants; ressources runtime (001);
refonte générale appearance/tooltip (003/008).

## Étapes

1. **Aligner tout le contrat public.** Comparer symboles/signatures de
   `wxInfoBar`, `wxInfoBarGeneric`, GTK et documentation, puis exposer style,
   setters, getters, état checkbox et overrides d’apparence compatibles. Ne pas
   limiter l’inventaire aux symboles qui bloquent actuellement la compilation.
   Stocker label/check state et effets/durée même si une propriété WinUI n’a pas
   d’équivalent direct. **Vérifier** : un TU de compatibilité compile chaque API
   publique, plus les TU XRC et le test InfoBar.
2. **Implémenter la checkbox.** Composer la checkbox dans Content sans perdre
   les boutons; `ShowCheckBox` met à jour label/state; les getters/événements du
   contrat commun restent cohérents. Tester création avec style absent/présent.
   **Vérifier** : test InfoBar ciblé exit 0.
3. **Mapper les effets et getters.** Utiliser transitions supportées; si un
   effet précis ne l’est pas, conserver l’API et documenter le mapping/fallback,
   sans no-op silencieux. Durée bornée; getters reflètent exactement le state
   demandé/effectif selon le contrat générique.
4. **Apparence et border.** `SetFont()`/`SetForegroundColour()` propagent au
   contenu XAML et le border par défaut reste `wxBORDER_NONE`; tester avant et
   après création et après re-template.
5. **Corriger mesure/layout.** Les mutations checkbox/boutons/message invalident
   best size et parent layout. Ajouter long text/wrap test.
6. **Réactiver config standard.** Reconfigurer avec XRC et InfoBar ON, compiler
   `test_gui` complet.
7. **Établir un dashboard de filtres.** Enregistrer les résultats composants de
   `plans/winui3-v0/verification.md`; ne pas marquer le pipeline vert en ignorant
   les filtres rouges. Les sous-plans 008a–008f les ferment.

## Commandes

```powershell
cmake -S . -B build-winui-clean -G "Visual Studio 17 2022" -A x64 `
  -DwxBUILD_TOOLKIT=winui -DwxBUILD_TESTS=ALL -DwxBUILD_SAMPLES=ALL `
  -DwxBUILD_SHARED=ON -DwxUSE_XRC=ON -DwxUSE_INFOBAR=ON
cmake --build build-winui-clean --config Release --target test_gui -- /m:4 /nr:false
build-winui-clean\lib\vc_x64_dll\test_gui.exe "[wxInfoBar]" --reporter compact
```

Attendu : build exit 0 et suite InfoBar verte, sans warning resources.

## Done

Exécuté le 2026-07-22; preuves dans la section du même jour de
[winui3-v0/verification.md](winui3-v0/verification.md).

- [x] XRC et tests compilent avec InfoBar ON ;
- [x] style, checkbox, setters, getters et overrides publics ont parité ;
- [x] checked state, effets/durée ont mapping ou fallback documenté et tests ;
- [x] font/foreground/border sont appliqués au peer XAML ;
- [x] long text/buttons/checkbox recalculent layout ;
- [x] build clean standard produit `test_gui` ;
- [x] dashboard conserve les échecs réels des autres composants ;
- [x] aucun fichier hors scope modifié.

## STOP

- L’API commune a changé upstream depuis le SHA planifié.
- WinUI InfoBar ne peut pas contenir checkbox et custom buttons sans changer un
  contrat public; proposer alors un composite interne, pas une suppression API.
- Le build reste bloqué par une erreur autre que celles inventoriées : rapporter
  la première erreur exacte avant d’élargir le scope.
