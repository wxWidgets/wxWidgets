/////////////////////////////////////////////////////////////////////////////
// Name:        gcupdatesmgr.h
// Purpose:     Header for cbGCUpdatesMgr class.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     19/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GCUPDATESMGR_G__
#define __GCUPDATESMGR_G__

#include "wx/fl/controlbar.h"
#include "wx/fl/updatesmgr.h"

#include "wx/fl/garbagec.h"

/*
This class implements optimized logic for refreshing
the areas of frame layout that actually need to be updated.
It is used as the default updates manager by wxFrameLayout.

It is called 'Garbage Collecting' updates manager because
its implementation tries to find out dependencies between bars,
and to order them into a 'hierarchy'. This hierarchical sorting resembles
the implementation of heap-garbage collectors, which resolve
dependencies between references.

Example: there are situations where the order in which the user
moves windows does matter.

\begin{verbatim}
  case 1)
     ------            ---
     | A     |             |B|
        ------     --->      | |
            ---               --- ------
            |B|                   | A  |
            | |                   ------
            ---
                        (future)
       (past)
\end{verbatim}

Past/future positions of A and B windows completely overlap, i.e.
depend on each other, and there is no solution for
moving the windows without refreshing both of them
-- we have a cyclic dependency here. The garbage collection algorithm will
find this cyclic dependecy and will force refresh after movement.

\begin{verbatim}
  case 2)

     ------
     | A     |
        ------     --->
            ---
            |B|                              ------
            | |                              | A  |
            ---                              ------
                         ---
                         |B|
                         | |
                         ---

                        (future)
       (past)
\end{verbatim}

In this case past/future positions do not overlap, so
it is enough only to move windows without refreshing them.
Garbage collection will 'notice' this.

There is also a third case, when overlapping is partial.
In this case the refreshing can also be avoided by
moving windows in the order of 'most-dependant' towards the
'least-dependent'. GC handles this automatically, by
sorting windows by their dependency-level (or 'hierarchy').

See garbagec.h for more details of this method; garbagec.h/cpp
implement sorting of generic dependencies and does not deal
with graphical objects directly.

Summary: garbage collection improves performance when complex or large
windows are moved around, by reducing the number of repaints. It also helps
to avoid dirty non-client areas of moved windows
in some special cases of 'overlapping anomalies'.
*/

class WXDLLIMPEXP_FL cbGCUpdatesMgr : public cbSimpleUpdatesMgr
{
    DECLARE_DYNAMIC_CLASS( cbGCUpdatesMgr )
protected:

    GarbageCollector mGC;

        // Internal function for repositioning items.
    void DoRepositionItems( wxList& items );

        // Internal function for repositioning items.
    void AddItem( wxList&     itemList,
                  cbBarInfo*  pBar,
                  cbDockPane* pPane,
                  wxRect&     curBounds,
                  wxRect&     prevBounds );

public:

        // Default constructor.
    cbGCUpdatesMgr(void) {}

        // Constructor, taking a frame layout.
    cbGCUpdatesMgr( wxFrameLayout* pPanel );

        // Receives notifications from the frame layout.
    virtual void OnStartChanges();

        // Refreshes the parts of the frame layout which need an update.
    virtual void UpdateNow();
};

#endif /* __GCUPDATESMGR_G__ */

