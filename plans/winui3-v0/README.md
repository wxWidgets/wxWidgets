# Audit de reprise wxWinUI 3 — verdict V0

> **Archive de baseline (22 juillet 2026).** Ce document conserve le verdict
> initial et ne décrit plus l’état courant du chantier : plusieurs bloqueurs
> et niveaux ci-dessous ont depuis été fermés. La source de vérité actuelle
> est [le tableau des plans](../README.md), avec les preuves chronologiques
> dans [verification.md](verification.md). Ne pas utiliser les statuts de cet
> audit pour décider de réimplémenter une phase.

> **Mise à jour de clôture (22 août 2026).** Le bloqueur historique « D&D OLE
> absent » ci-dessous est fermé au niveau déterministe : le produit possède un
> broker et une identité COM uniques, enregistrés atomiquement sur le bridge et
> le TLW exact et coordonnés avec wxMSW par un registre global fail-closed. RC4
> a invalidé le broker TLW-only sur la surface normale; les gates duales
> shared/static sont vertes. Le statut courant reste **candidat bêta Supported
> V0**, et non bêta production-ready, tant que la gate physique produit du RC7
> gelé, l'accessibilité/DPI/thèmes/HC/RTL
> manuels et le soak signé de 60 minutes ne sont pas terminés. Cela ne vaut pas
> parité wxMSW globale.

## Verdict court

Le port possède une base architecturale prometteuse et de nombreux contrôles
natifs, mais il n’est pas encore qualifiable comme beta. Il peut devenir une
**V0 alpha de développeur** après fermeture de sept bloqueurs : déploiement des
ressources WinUI dans un build frais, compilation publique `wxInfoBar`, durée de
vie de l’hôte partagé, accélérateurs/menus, D&D OLE, contrat des dialogues et
stabilisation lifetime/crash de `wxTextCtrl`.

La liste de 14 restes proposée est globalement pertinente, mais incomplète. Elle
omet notamment :

1. le build frais qui ne déploie pas les PRI/arborescences XAML nécessaires ;
2. la rupture d’API publique `wxInfoBar`, qui empêche XRC et `test_gui` de
   compiler ;
3. les handlers routés des slots non révoqués et leurs captures fortes ;
4. `wxWinUIXamlHost`, qui crée encore une île par contrôle et viole l’invariant
   central ;
5. la synchronisation commune de `Enable`, tooltips et noms UIA ;
6. les échecs déjà observables dans les tests upstream de plusieurs composants.

## Baseline auditée

- Dépôt : `C:\wxWidgets-master`
- Branche : `feature/winui-port-prototype`
- HEAD : `20207bdbbb81055da1f3e980b3271fc24566990e`
- Base `origin/master` : `5bd892f7ac21eb007e1ebb0e5e00c492c17580f8`
- Historique WinUI audité : 61 commits, dont 35 en avance sur
  `fork/feature/winui-port-prototype`
- Conversation :
  `C:\Users\ferre\.claude\projects\C--wxWidgets-master-build-master\75517e86-2263-422d-90a3-b8f7850d2373.jsonl`
  (7 273 enregistrements, environ 40 Mo)
- Plan de référence :
  `C:\Users\ferre\.claude\plans\iridescent-snacking-oasis.md`
- Worktree : sale avant l’audit, avec 18 fichiers suivis modifiés, 6 fichiers
  indexés et plusieurs fichiers non suivis liés à la refonte.

## Niveau réel par jalon

| Jalon | Verdict | Conditions minimales |
|---|---|---|
| Prototype | Atteint | Samples principaux compilables et architecture une île/TLW démontrée |
| V0 alpha développeur | Pas atteint | 001–004, 007a, 005, 007b, 006 et gate lifetime de 008a terminés; aucun crash/freeze reproductible; samples/tests ciblés depuis build frais |
| V0 alpha publique | Pas atteint | En plus : composants P0/P1 de la matrice conformes, checklist interactive complète, documentation des limites |
| V0 beta | Pas atteint | Suite GUI qualifiée, UIA/Narrator, DPI/RTL/multi-TLW, thèmes à chaud, D&D, menus et dialogues sans limitation critique |
| Parité wxMSW | Très loin | Nombreuses API/styles/events manquants; contrôles complexes encore génériques ou MSW |

## Bloqueurs ordonnés

| Rang | Bloqueur | Impact | Preuve principale |
|---:|---|---|---|
| 1 | Déploiement runtime WinUI incomplet dans un build frais | Les contrôles n’obtiennent pas leurs templates | `build/cmake/winui.cmake:249-263`, test d’audit |
| 2 | API `wxInfoBar` incomplète | XRC et `test_gui` ne compilent pas | `include/wx/winui/infobar.h:19-56` |
| 3 | Durée de vie et invariants des slots | Fuites/UAF, contrôles désactivés encore actifs | `src/winui/tlwhost.cpp:878-1115`, `2030-2069` |
| 4 | Accélérateurs neutralisés par XAML | Raccourcis de menu cassés | `src/winui/winui.cpp:336-361` |
| 5 | D&D OLE absent — **constat historique fermé déterministement le 2026-08-22** | Le broker produit acquiert atomiquement le bridge et le TLW ; sa gate physique RC7 gelée reste ouverte | [ADR 0002](../../docs/winui/adr/0002-ole-drop-integration.md) et [preuves chronologiques](verification.md) |
| 6 | Dialogues/transitoires non contractualisés | Focus, Enter/Escape, owner, stacking et cropping | `src/msw/toplevel.cpp:470-502`, `src/winui/dlgpresenter.cpp` |
| 7 | Parité API de composants | Tests upstream en échec, dont TextCtrl instable | `plans/winui3-v0/verification.md` |

## Décision d’architecture recommandée

Construire la V0 comme un **port hybride explicite** : composants WinUI natifs
seulement lorsqu’ils respectent le contrat wx; fallback générique ou MSW annoncé
pour le reste. Ne pas faire de « migration native de tout » un prérequis beta.
L’invariant central doit devenir vérifiable : un seul
`DesktopWindowXamlSource` par TLW, y compris pour `wxWinUIXamlHost` et les
surfaces transitoires, avec un registre de durée de vie par hôte.

Le détail est dans [architecture.md](architecture.md) et la feuille de route par
composant dans [component-matrix.md](component-matrix.md).

## Limites de cet audit

- Aucun fichier source n’a été modifié.
- Aucun commit n’a été créé.
- La validation manuelle Narrator, multi-écrans et thème à chaud reste à faire
  avec l’utilisateur.
- `showcase` n’a pas pu être relinké car le `showcase.exe` de l’utilisateur
  verrouillait `WebView2Loader.dll`; ce n’est pas une erreur de compilation
  constatée.
