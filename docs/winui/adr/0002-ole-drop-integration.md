# ADR 0002 — Broker OLE unique, enregistré sur le bridge et le TLW WinUI

- Statut : accepté et implémenté ; RC7 rejeté, correctif de présentation
  qualifié en shared/static, nouveau candidat physique à geler
- Date de décision : 2026-08-22 (révisée après les campagnes RC4 et RC7)
- Portée : backend wxWinUI, drag-and-drop OLE externe

## Contexte

Un top-level wxWinUI contient un unique `DesktopWindowXamlSource`. Son bridge
de composition couvre les surfaces XAML, mais sa région comporte des trous pour
laisser apparaître certains enfants Win32, notamment les scrollbars et les
fallbacks natifs. Le HWND retourné par `WindowFromPoint()` n'est donc pas le
même sur toute la surface logique du top-level.

Le chemin wxMSW historique enregistre chaque `wxDropTarget` sur le shell HWND
de son contrôle. Conserver ces inscriptions sous un host WinUI créerait
plusieurs récepteurs physiques concurrents et ne fournirait pas une autorité
unique pour les transitions entre contrôles XAML et enfants natifs.

## Options évaluées

1. Enregistrer le broker sur `InputSiteWindowClass`.
   Cette fenêtre ne reste pas physiquement exposée dans la topologie observée.
2. Enregistrer le broker sur le bridge.
   Le bridge reçoit la surface XAML normale mais pas les points situés dans ses
   trous de région.
3. Enregistrer deux adaptateurs physiques d'un broker unique sur le bridge et
   le TLW. Cette option couvre les deux surfaces, exige deux clés d'ownership,
   deux identités COM et deux verrous externes, mais conserve une seule session
   logique.
4. Enregistrer un broker unique sur le HWND du top-level (TLW).
   Le TLW enveloppe la surface normale et les trous natifs, tandis que le
   bridge et le registre des slots restent disponibles pour le routage logique.

## Décision

Chaque `wxWinUITopLevelHost` possède exactement un `wxWinUIDropBroker`, une
session logique et deux adaptateurs COM `IDropTarget` physiques distincts, un
par HWND. Le broker réserve d'abord les deux couples exacts
`(HWND, génération)`, puis appelle `RegisterDragDrop()` sur le bridge et sur le
TLW, dans cet ordre. L'activation exige les deux inscriptions encore courantes.
Le shutdown révoque et déverrouille en ordre inverse, TLW puis bridge. Chaque
adaptateur équilibre son propre verrou externe. Un échec du second verrou ou
enregistrement déroule exactement le premier ; aucune demi-inscription active
n'est publiée.

La première acquisition native n'a jamais lieu pendant la construction cachée.
Le broker existe alors inactif et les `wxDropTarget` logiques conservent leurs
inscriptions wxMSW normales. Le premier `FlushSync()` qui observe le TLW et le
bridge visibles, non iconifiés et dotés d'une géométrie non vide détache ces
cibles puis acquiert atomiquement la paire. Une fois acquise, elle reste en
place à travers `Hide()`/`Show()` et l'iconification. Le host ne fait donc ni
`RevokeDragDrop()` ni rebind spéculatif après une publication XAML dont il ne
peut pas inspecter l'identité OLE native.

Un registre global partagé avec le chemin wxMSW classique sérialise les
opérations OLE par couple exact `(HWND, génération)`. Il distingue une
registration fixe wxMSW, une acquisition externe par le broker et une
occupation devenue incertaine. Les identités COM et le verrou externe sont
retenus tant que leur libération n'est pas prouvée ; un échec de
`RevokeDragDrop()` ou de `CoLockObjectExternal(FALSE)` reste donc fail-closed et
ne rend jamais prématurément le HWND disponible.

Le bridge est le récepteur physique de la surface XAML normale et reste
l'autorité de composition, de hit-test et de conversion de coordonnées. Le TLW
reçoit les trous qui exposent des enfants natifs. Les deux inscriptions appellent
la même session logique : un `Drop` physique ne peut donc pas créer deux
livraisons wx. Chaque callback valide les deux identités HWND avant tout routage.

Avant d'acquérir la paire, le broker détache transactionnellement les cibles wx
déjà inscrites sous ce top-level, y compris une cible placée directement sur le
frame. Tout échec d'acquisition restaure les inscriptions détachées. Si une
étape ultérieure d'initialisation du host échoue, le shutdown révoque le broker
provisoire puis réconcilie les cibles logiques. Une inscription étrangère ou
une identité devenue incertaine provoque un échec fermé ; elle n'est jamais
révoquée par supposition.

Pendant l'initialisation provisoire, dès que le broker acquiert la paire, les
nouveaux appels `SetDropTarget()` sous ce host ne doivent plus créer
d'inscription shell concurrente. Le reparentage réconcilie chaque liaison avec
le broker du TLW de destination ou restaure le chemin wxMSW hors host.

Si une registration fixe est momentanément en cours, le broker publie un état
`PendingFixed` exact et reprend l'acquisition une seule fois après la sortie de
la frontière OLE externe. La continuation est liée à l'identité native, au
thread UI et à un identifiant 64 bits ; les doublons et messages périmés sont
ignorés. Un échec de `PostMessage()` arme un timer one-shot sur ce même thread,
sans boucle active. La publication de l'hôte XAML et de ses slots reste
indépendante de ce rendez-vous : un contrôle et son contenu ne sont ni perdus
ni détruits parce que la propriété OLE est encore différée ou échoue.

`DragAcceptFiles()` et `WM_DROPFILES` constituent un contrat séparé de la
session COM OLE. Le message reste reçu et routé par le bridge ; sa réussite ne
qualifie pas OLE, et réciproquement.

## Preuve de faisabilité et correction RC4

Le 2026-08-21, le probe physique opt-in
`wx_winui_runtime_smoke --ole-drop-delivery-probe` a exécuté de vrais
`DoDragDrop()` bornés sur la surface normale puis dans un trou exposant un
enfant natif. Avec une garde d'entrée propre :

- le bridge et le TLW ont reçu séparément la surface normale ;
- le bridge a manqué proprement le trou natif ;
- le TLW a reçu la surface normale et le trou ;
- les armements combinés ont sélectionné le bridge en surface normale et le
  TLW dans le trou ;
- le résumé a produit `receiver-qualified` et un code de sortie nul.

La campagne RC4 du 2026-08-22 a ensuite passé latence, claviers shared/static et
la matrice OLE brute, mais son gate produit TLW-only a expiré sur la surface
normale avec zéro callback COM. Dans le même artefact, l'armement combiné brut a
livré exclusivement au bridge en surface normale et exclusivement au TLW dans
le trou natif. Cette preuve a invalidé la décision TLW-only et impose la paire
bridge+TLW, sans multiplier les brokers ni les sessions logiques.

RC7 a qualifié de nouveau latence, claviers et matrice OLE brute, puis a échoué
au premier drag produit normal : bouton injecté observé, 51
`QueryContinueDrag`, garde d'entrée propre, mais zéro callback `DragEnter` sur
le broker. Les deux entrées du ledger étaient possédées et « current ». La
différence structurante avec le probe brut était l'ordre : le produit
enregistrait pendant que le frame était caché, tandis que le probe enregistrait
après `Show()` et layout. RC7 est donc rejeté ; ce résultat n'est ni une
interruption utilisateur ni un échec de routage wx.

Le correctif supprime cette frontière non qualifiée : aucune acquisition dans
la construction, même si le TLW est déjà visible ; acquisition seulement après
un flush visible ayant publié la géométrie et l'epoch structurel ; deux
identités COM physiques distinctes partageant le même état et la même session ;
et source du gate produit bornée dans le bridge comme le probe brut déjà
qualifié. Les messages show/géométrie du bridge déclenchent les retries sans
polling. Un changement de présentation réentrant avant commit invalide l'epoch,
rollbacke la paire provisoire, restaure les cibles logiques puis attend un
nouveau flush stable.

Les gates déterministes postérieures couvrent le broker produit et le registre
global dans les deux linkages :

- `[winui-drop-broker]~[physical]` : **31 cas / 1 330 assertions** dans les deux
  linkages ;
- `[msw-drop-session]` : **11 cas / 218 assertions** en shared et **11 cas /
  202 assertions** en static/no-exceptions ;
- initialisation différée du host : **6 cas / 59 assertions** ;
- Supported V0 : **50 cas / 927 assertions** dans chaque linkage ;
- HostLifecycle/HostState/broker : **190 cas / 25 675 assertions** dans chaque
  linkage.

Ces résultats sont les preuves pre-freeze du correctif post-RC7 et non la
signature d'un nouveau candidat physique.

## Conséquences et gates de qualification

Les tests automatiques doivent couvrir l'identité exacte Register/Revoke,
l'acquisition d'une cible TLW préexistante, les rollbacks d'échec et de faute
tardive, la réentrance, le reparentage et le multi-TLW. Les profils partagé et
statique sans exceptions doivent être verts.

Avant promotion en bêta production-ready, la gate physique opt-in du prochain
candidat gelé doit livrer données, effet et coordonnées au vrai `wxDropTarget` via
le broker produit, sur une surface XAML normale et dans un trou natif, avec
garde d'entrée propre. Le candidat doit aussi passer les drags
inter-processus manuels texte/Explorer, l'accessibilité, le DPI per-monitor,
les thèmes/High Contrast/RTL et le soak signé de 60 minutes. Ces gates restent
ouvertes ; la décision ne promet ni les profils exclus ni la parité wxMSW
globale.
