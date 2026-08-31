# Plan 011 — Décider la stratégie `wxWinUI::` / `wxMSW::`

> **Instructions** : produire d’abord une ADR, aucun renommage massif. Ce sujet
> n’est pas un reste explicite du plan historique et ne bloque pas la V0.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- include/wx src/winui src/msw build/cmake`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`. Réconcilier toute dérive avant l’ADR.

## Statut

- **Priorité** : P3
- **Effort** : M analyse, XL si coexistence choisie
- **Risque** : HIGH ABI
- **Dépend de** : 010
- **Catégorie** : direction, ABI, architecture
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

Les deux toolkits définissent actuellement les mêmes classes globales, symboles
DLL, RTTI wx et events. CMake retire les sources MSW quand un peer WinUI les
remplace. Ajouter des namespaces n’est donc ni une finition ni un alias trivial;
cela peut casser ABI, XRC, dynamic class info et code utilisateur.

## Scope

**Dans le scope :** ADR, inventaire d’exports/RTTI/XRC/CMake, prototypes jetables
dans un répertoire ignoré et planification de migration. **Hors scope :** tout
renommage de source, changement d’API publique, ABI ou sélection de backend avant
approbation explicite de l’ADR.

## Questions que l’ADR doit trancher

1. Veut-on deux backends dans le même binaire, ou seulement nommer les helpers
   privés ?
2. L’API publique reste-t-elle `wxButton`, sélectionnée au build, ou expose-t-on
   de nouvelles classes opt-in ?
3. Quelle compatibilité source/binaire et quel impact DLL/export ?
4. Comment XRC, RTTI (`wxClassInfo`), event tables et factories choisissent-ils
   une implémentation ?
5. Comment les fallbacks génériques et classes composites voient-ils les deux ?
6. Ce design est-il acceptable upstream wxWidgets ?

## Options à comparer

### A — Namespace uniquement pour helpers privés (recommandé V0)

Classes wx publiques inchangées; internes nouveaux sous `wxWinUI`/`wxMSW` ou
namespace detail. Faible risque ABI, pas de coexistence de widgets publics.

### B — Classes publiques parallèles

Ex. `wxWinUI::Button` et `wxMSW::Button`. Nécessite API/RTTI/XRC nouvelles et
adaptation des composites. Coût très élevé, compatibilité upstream incertaine.

### C — Backend runtime derrière pimpl/factory

Une classe wx publique choisit un peer à runtime. Permet coexistence mais exige
un seam portable massif, et les deux implémentations doivent être linkées sans
collision. Risque maximal.

### D — Deux bibliothèques/ABI distinctes (état actuel formalisé)

Sélection compile-time, namespaces internes seulement. Simple et cohérent avec
les toolkits wx historiques.

## Étapes

1. Inventorier exports/symboles/RTTI de Button comme pilote dans wxMSW et WinUI.
2. Faire un prototype minimal non livré pour chaque option viable; vérifier link,
   dynamic cast, XRC et event dispatch.
3. Mesurer impact source/ABI/CMake/package et maintenance par composant.
4. Consulter contraintes/conventions upstream.
5. Rédiger `docs/winui/adr/0004-backend-namespaces.md` avec décision,
   alternatives rejetées, conséquences, migration et non-goals.
6. Faire valider avant tout changement de namespace.

## Vérification

- dumpbin/export et link d’un app pilote ;
- deux Buttons si coexistence visée ;
- RTTI `wxDynamicCast`, XRC creation, event binding ;
- build shared/static MSW et WinUI ;
- aucun changement ABI involontaire démontré par comparaison exports.

## Done

- [ ] cas d’usage explicite ;
- [ ] quatre options comparées ;
- [ ] prototype pilote et preuves ABI/RTTI/XRC ;
- [ ] ADR approuvée ;
- [ ] migration séparée planifiée si nécessaire.

## STOP

- L’objectif réel n’est pas la coexistence mais seulement la lisibilité interne :
  choisir A et ne pas exposer d’API.
- Le prototype duplique `wxClassInfo` ou symboles globaux.
- Une rupture ABI est nécessaire sans accord upstream/utilisateur.
