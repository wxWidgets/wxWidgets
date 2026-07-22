# Plan 009 — Qualifier UIA, DPI, RTL, thème et multi-TLW

> **Instructions** : ce plan est une gate transversale, pas une passe cosmétique.
> Exécuter les validations manuelles Narrator avec l’utilisateur. Aucun commit
> sans validation.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/winui src/msw/window.cpp tests samples`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`. Réconcilier toute dérive avant édition.

## Statut

- **Priorité** : P1
- **Effort** : L
- **Risque** : MED
- **Dépend de** : 003–006, 007a–007c et les sous-plans 008a–008f des
  composants classés Supported pour le jalon visé
- **Catégorie** : accessibility, compatibility, tests
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

La géométrie de base suit DPI/RTL, mais les bitmap bundles, caches, noms UIA et
shells n’y sont pas intégrés. Le reparent multi-TLW existe sans stress. Ces gates
séparent une démo visuelle d’une beta utilisable.

## État courant

- DPI event marque les slots dirty (`tlwhost.cpp:1811-1818`) ;
- noms UIA appliqués seulement à registration (`:152-203,513-573`) ;
- slots shell restent fragments potentiels ;
- plusieurs contrôles utilisent `FromDIP()` avant propriétés XAML DIPs ;
- StaticBitmap/Notebook/Toolbar/Menu caches non rechargés ;
- reparent migration au flush (`tlwhost.cpp:1899+`) non stressée ;
- Narrator jamais validé sur version finale.

## Scope

Adapters communs et composants promis alpha/beta, samples/tests accessibility,
DPI/RTL/theme/multi-TLW. Hors scope : composants hors V0.

## Étapes

1. **UIA topology probe.** Accessibility Insights snapshot d’une frame mixte;
   identifier bridge, shells, roots XAML et enfants GDI. Définir tree attendu.
2. **Shell neutralization.** Cacher les HWND coquilles sans supprimer l’identité
   utile des contrôles GDI; un contrôle logique = un fragment/action.
3. **Automation adapter.** Name/Role/State/HelpText/relations; marquer les noms
   auto pour refresh après SetLabel, ne jamais écraser nom explicitement fourni.
4. **Focus/UIA actions.** Invoke/Toggle/Selection/Value sur enabled/disabled,
   logique focus wx et reparent.
5. **DPI audit typé.** Inventorier toutes propriétés XAML recevant FromDIP ou
   pixels; corriger double scaling; recharger bundles/caches à generation DPI.
6. **RTL.** Positions physiques restent non mirroir; FlowDirection content;
   hit-test/coordinates/text align/menu/tree/calendar; dynamic switch.
7. **Hot theme/high contrast.** ThemeResource et custom colours; aucun brush
   figé; switch sans recréer/doubler handlers.
8. **Multi-TLW/reparent.** Deux frames sur DPI différents; A↔B contrôle/parent,
   active focus, tooltip, D&D, owner/dialog, destroy source/destination.
9. **Narrator manuel.** Avec l’utilisateur : nom/role/state/value, Tab order,
   menu/dialog/tree/list et annonces dynamiques.

## Tests/gates

- 100/150/200 % et déplacement inter-écrans ;
- RTL LTR→RTL→LTR à chaud ;
- Light/Dark/High Contrast à chaud ;
- Accessibility Insights : zéro shell duplicate ;
- Narrator checklist signée ;
- reparent A↔B 100 cycles, source/destination destroy ;
- bitmap source resolution inspectée ;
- UIA actions disabled refusées.

## Done

- [ ] UIA tree attendu documenté et snapshots comparés ;
- [ ] names dynamiques, roles/states/actions corrects ;
- [ ] Narrator validé avec utilisateur ;
- [ ] aucun double scaling/cache stale à trois DPI ;
- [ ] RTL et theme/hc à chaud ;
- [ ] multi-TLW/reparent sans leak/focus/input loss ;
- [ ] limitations hors V0 explicites.

## STOP

- Neutraliser le shell masque aussi un contrôle GDI nécessaire : arrêter et
  concevoir une relation proxy, ne pas supprimer l’accessibilité.
- Un composant promis ne possède aucun pattern UIA équivalent; demander décision
  fallback ou exclusion.
- Aucun environnement multi-DPI/Narrator n’est disponible : marquer BLOCKED, ne
  pas auto-valider.
