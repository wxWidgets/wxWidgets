# Fiche 12 — Top-level, plateforme, dessin et GL

Baseline `20207bdbbb` + worktree owner/frame-class. Cette fiche complète la
fondation et définit ce qui n’est pas une migration WinUI.

## wxTopLevelWindow / wxFrame

MSW reste le shell; le host unique et le chrome XAML s’ajoutent. Alpha :

- transaction create/show/destroy du host ;
- minimize/maximize/restore, move/resize, Alt-Tab ;
- owner detach/restoration et priming ;
- deux frames + reparent A↔B ;
- menu/toolbar/status reservations et client rect ;
- DPI inter-écrans, RTL/hot theme ;
- UIA root/children sans shell duplicate.

Tester aussi création sans aucun contrôle WinUI puis ajout tardif.

## wxDialog shell

MSW + frame-class/presenter. Les contrats sont détaillés fiche 09. Le point
plateforme est de garantir styles non-client, default/cancel keyboard, owner,
taskbar grouping, modality et destruction comme un vrai dialogue même sans
dialog manager Win32.

## wxPopupWindow / MiniFrame

MSW hérités. Beta smoke : host lazy, focus/capture, show without activate,
owner, close, DPI/theme et child slots. Les intégrer au TransientManager si leur
usage le requiert, sans créer une île autonome.

## MDI

MSW hérité et non migré. Hors V0 recommandé : parent/child frame, client HWND,
activation, menus fusionnés et child reparent peuvent créer une topologie de
hosts imbriqués non prévue. Spike dédié requis avant support.

## TaskbarIcon / TaskBarButton

MSW hérité; peu lié à l’île. Post-beta smoke : icon lifecycle, balloon,
progress/overlay, popup menu WinUI, dialog launch, Explorer restart et shutdown.

## wxDC, GDI+, Direct2D, printing et overlays

À conserver, pas à migrer : la stratégie hybride suppose que le GDI reste
visible sous le bridge. Alpha/Beta gates :

- PaintDC/ClientDC/MemoryDC, GDI+/D2D renderers ;
- invalidations partielles, Freeze/Thaw, overlay/rubber band ;
- scroll rapide avec slots ;
- printing/metafile/bitmap hors écran, indépendant de XAML ;
- high contrast/theme/DPI.

Les tests graphics existants constituent une bonne baseline; ajouter les cas de
composition sous slots.

## wxGLCanvas

MSW/WGL avec own DC (`src/msw/glcanvas.cpp:709`), surface à haut risque. Hors V0
recommandé. Spike avant promotion : création/context sharing, swap/resize,
bridge au-dessus, mouse/keyboard/capture, ContentDialog overlay, DPI, fullscreen,
device/context loss et destroy.

Si le bridge masque ou perturbe le swapchain, ne pas créer plusieurs îles sans
ADR; déclarer GL incompatible avec slots superposés pour la V0.

## Critères alpha plateforme

- minimal/widgets/showcase/dataview démarrent depuis build frais ;
- multi-TLW/reparent/minimize/Alt-Tab/owner sans lag, freeze ou orphan ;
- DPI/theme/RTL et client geometry stables ;
- drawing générique visible et interactif sous le bridge ;
- MDI/GL/Media/WebView clairement exclus s’ils ne sont pas qualifiés.

