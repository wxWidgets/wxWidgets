/////////////////////////////////////////////////////////////////////////////
// Name:        doc_mainpage.h
// Purpose:     wxFoldPanel
// Author:      Jorgen Bodde
// Modified by:
// Created:     22/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/** \author Jorgen Bodde
    \mainpage

    This is the wxFoldPanel documentation guide. This control is written for wxWidgets (http://www.wxwidgets.com), and 
    can be used as a side bar with managed panels which can collapse and expand. 

    The wxFoldPanelBar is very easy in it's use and allows great flexibility in manipulating it even after creation. It can 
    be created in two modes:

    - In place folding: This means that the panels which are collapsed, stay where they are. 
    - To bottom folding: This means that the panels are collapsed to the bottom

    The caption bars are managed by another stand alone control (which I will fix up to be released soon), called the 
    wxCaptionBar. You as user don't have to do much with this control right now, all styles are rerouted through the
    wxFoldPanelBar. The only class of interest is wxCaptionBarStyle which takes care of the visual style of the
    bar. The caption bar can be drawn in the following modes:

    - Gradient horizontal fill
    - Gradient vertical fill
    - Single flat colour
    - Single colour with border
    - Single border with background fill

    wxFoldPanelBar is freeware and distributed under the wxWidgets license. wxWidgets is THE toolkit for 
    (cross platform) C++ / Python programming!

    \section things_done Things to be done

    I would like to add the following features when time allows me. When you really like one of these features to be in the 
    next release, you can either make that clear to me, or help me with it. They aren't that hard.

    - Single fold which means all other panels are collapsed automatically
    - Smart size of panels when only one is open, take the whole space, or use sizers to proportionally size
    - Small icons next to the caption for clarity and make it a bit more visually nice
    - A bottom bar with visual icon when the bar cannot be completely shown to aid the user that there is more then can be seen
    - Panels can be hidden. This feature will also need the bottom bar, so that the icons of hidden panels will apear there
    - Resizable panels, the user can manually size the panels bigger or smaller

    \section how_work How does it work?

    The internals of the wxFoldPanelBar is a list of wxFoldPanelItem classes. Through the reference of wxFoldPanel these
    panels can be controlled by adding new controls to a wxFoldPanel or adding new wxFoldPanels to the wxFoldPanelBar. The
    wxCaptionBar fires events to the parent (container of all panel items) when a sub-panel needs resizing (either folding
    or expanding). The fold or expand process is simply a resize of the panel so it looks like all controls on it are gone.
    All controls are still child of the wxFoldPanel they are located on. If they don't handle the event (and they won't) 
    then the owner of the wxFoldPanelBar gets the events. This is what you need to handle the controls. There isn't much to it
    just a lot of calculations to see what panel belongs where. There are no sizers involved in the panels, everything is
    purely xy positioning.

    \section what_dp What can it do and what not?

    What it can do:

    - Run-time addition of panels (no deletion just yet)
    - Run time addition of controls to the panel (it will be resized accordingly)
    - Creating panels in collapsed mode or expanded mode
    - Various modes of caption behaviour and filling to make it more appealing
    - Panels can be folded and collapsed (or all of them) to allow more space
    
    What it cannot do:

    - Selection of a panel like in a list ctrl
    - Dragging and dropping the panels
    - Re-ordering the panels (not yet)

    Special thanks to Julian Smart et al. for making this great toolkit!
*/

