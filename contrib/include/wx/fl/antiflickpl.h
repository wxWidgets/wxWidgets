/////////////////////////////////////////////////////////////////////////////
// Name:        antiflickpl.h
// Purpose:     Double-buffering plugin class for reducing flicker
// Author:      Aleksandras Gluchovas (@Lithuania)
// Modified by:
// Created:     23/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ANTIFLICKPL_G__
#define __ANTIFLICKPL_G__

#include "wx/fl/controlbar.h"

/*
Implements double-buffering to reduce flicker.
Bitmap and memory DC buffers are shared 'resources' among all instances of
antiflicker plugins within the application.

Locking for multithreaded applications is not yet implemented.
*/

class WXDLLIMPEXP_FL cbAntiflickerPlugin : public cbPluginBase
{
    DECLARE_DYNAMIC_CLASS( cbAntiflickerPlugin )
protected:

    static wxBitmap*   mpVertBuf;
    static wxBitmap*   mpHorizBuf;
    static wxMemoryDC* mpVertBufDc;
    static wxMemoryDC* mpHorizBufDc;

    static int mRefCount;

    wxDC*  mpLRUBufDc; // last-recently-used buffer
    wxRect mLRUArea;   // last-recently-used area

protected:
        // Finds a suitable buffer. Returns NULL if a suitable buffer is not present.

    wxDC* FindSuitableBuffer( const wxRect& forArea );

        // Allocates a suitable buffer.

    wxDC* AllocNewBuffer( const wxRect& forArea );

        // Gets the window device context.

    wxDC& GetWindowDC();

        // Gets the client device context.

    wxDC& GetClientDC();
public:

        // Default constructor.

    cbAntiflickerPlugin(void);

        // Constructor taking frame layout panel, and pane mask.

    cbAntiflickerPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Destructor.

    virtual ~cbAntiflickerPlugin();

        // Handler for plugin event.

    void OnStartDrawInArea ( cbStartDrawInAreaEvent& event );

        // Handler for plugin event.

    void OnFinishDrawInArea( cbFinishDrawInAreaEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __ANTIFLICKPL_G__ */

