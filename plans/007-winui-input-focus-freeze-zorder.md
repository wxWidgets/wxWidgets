# Plan 007 — Durcir input, focus, freeze, curseurs et Z-order

> **Instructions** : conserver les correctifs freeze/curseur validés; étendre
> leur couverture avec instrumentation opt-in. **Ce fichier est un EPIC
> d’orchestration : ne pas l’implémenter en un seul diff.** Exécuter 007a–007c
> séparément. Aucun commit sans validation.
>
> **Drift check** : `git diff --stat 20207bdbbb..HEAD -- src/winui/tlwhost.cpp include/wx/winui/private/tlwhost.h src/winui/ctrlhost.cpp src/msw/window.cpp src/msw/evtloop.cpp samples/winuispike`, puis les mêmes paths avec `git diff --stat --`, `git diff --cached --stat --` et `git status --short --`. Réconcilier toute dérive avant édition.

## Statut

- **Priorité** : P1
- **Effort** : XL cumulé; sous-plans M/L
- **Risque** : HIGH
- **Dépend de** : 003 et 004; dépendances fines dans les sous-plans
- **Catégorie** : EPIC, bug, perf, input, tests
- **Planifié à** : `20207bdbbb`, 2026-07-21

## Pourquoi

Le routeur island-first fonctionne nominalement, mais plusieurs équivalences
Win32 manquent : sibling transparent, non-client leave, XBUTTON/touch/pen,
focus final, Z dynamique. Le breaker freeze ne couvre qu’une signature et les
curseurs animés/busy restent.

## État courant

- storm breaker MOVE même pixel : `tlwhost.cpp:1298-1335` ;
- native target premier sibling et disabled handling : `:1668-1715` ;
- HTTRANSPARENT remonte parent au lieu de sibling inférieur : `:1373-1388` ;
- hover poste seulement WM_MOUSELEAVE : `:1355-1396` ;
- slot pointer gauche/droite/milieu : `:974-1036` ;
- ZIndex par création pré-ordre : `:2076-2102` ;
- bridge top unique : `:2135-2166` ;
- cursor class workaround : `:2125-2132,2269+`.

## Scope

InputRouter/FocusArbiter/Geometry/Z/Cursor/Freeze et spike/tests. Hors scope :
D&D COM (005), menu keyboard (004), contrats internes des contrôles (008).

## Sous-plans faisant autorité

| Sous-plan | Scope borné | Dépend de |
|---|---|---|
| [007a](007a-winui-pointer-hit-test-freeze.md) | pointer, hit-test, hover/capture, breaker freeze | 003 |
| [007b](007b-winui-focus-tab-cursors.md) | focus logique, Tab mixte et curseurs | 003, 004, 007a |
| [007c](007c-winui-zorder-scroll-splitter.md) | Z-order, clip, scroll latency et splitter | 003, 007a |

Les étapes ci-dessous sont la checklist de l’epic; les scopes, commandes, Done
et STOP exécutables sont ceux des trois sous-plans.

## Étapes

1. **Instrumentation opt-in.** Mettre ring/histogrammes derrière flag/env; unifier
   chemin collect-freeze; définir counters MOVE/ENTER/EXIT/cancel/capture/flush.
2. **Harnais storm déterministe.** Injecter same-pixel MOVE, oscillation 2 pixels,
   ENTER/EXIT, capture-cancel et release outside. Vérifier responsiveness et
   aucun faux breaker sur mouvement réel.
3. **Hit-test natif.** Reproduire `WindowFromPoint`/ChildWindowFromPointEx et
   HTTRANSPARENT sibling semantics; disabled/hidden/transparent; client/nonclient.
4. **Hover/capture.** TrackMouseEvent client/nonclient, WM_MOUSELEAVE/
   WM_NCMOUSELEAVE, capture changes/cancelmode et double-click coordinates.
5. **Pointer parity.** XBUTTON, horizontal wheel, touch/pen policy; soit mapping
   wx explicite, soit forwarding natif documenté. Aucun event double.
6. **Focus coalescé.** Got/Lost template parts aboutissent à un seul logical slot
   transition; preferred target; destroy/reparent; Tab chain mixte.
7. **Curseurs.** Busy global imbriqué, custom cursor fallback et réassertion après
   resync/animation; restoration exacte.
8. **Z-order.** Mirror vrai sibling order pour XAML/XAML; tester Raise/Lower.
   Détecter/interdire topologies XAML/native non représentables.
9. **Scroll latency.** Mesurer delta temporel HWND→slot sur rafale; définir budget
   (par ex. <1 frame 60 Hz). Décider flush synchrone ou mirrored canvases sur
   mesure, pas intuition.

## Tests

- spike automatisé input/freeze ;
- Window mouse/focus/enter-leave/keyboard tests ;
- splitter/grid/list scrollbar drag ;
- capture outside/release, popup/dialog active ;
- 1h soak interactif + compteur storms ;
- busy/custom/animated cursor ;
- Raise/Lower overlaps ;
- scroll 60/120/165 Hz avec histogramme opt-in.

## Done

- [ ] aucune variante storm ne bloque l’UI ;
- [ ] breaker n’altère pas input normal ;
- [ ] client/nonclient/transparent sibling corrects ;
- [ ] touch/pen/XBUTTON politique testée ;
- [ ] focus/Tab stable lors reparent/destroy ;
- [ ] busy/custom/animated cursors stables ;
- [ ] Z limitation détectée/documentée ;
- [ ] budget scroll mesuré et décision consignée ;
- [ ] instrumentation off par défaut.

## STOP

- Le test montre que l’intercalation native/XAML demandée est physiquement
  impossible avec un bridge; demander une décision de limitation, ne pas créer
  une île supplémentaire.
- Une variante freeze ne peut être reproduite de façon déterministe après trois
  tentatives : conserver soak/log et rapporter, ne pas généraliser un heuristic.
