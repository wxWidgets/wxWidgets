/////////////////////////////////////////////////////////////////////////////
// Name:        mgl/data.cpp
// Purpose:     wxMGL-specific global data
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"
#include "wx/cursor.h"

/* Current cursor, in order to hang on to
 * cursor handle when setting the cursor globally */
wxCursor g_globalCursor;

/* Don't allow event propagation during drag */
bool g_blockEventsOnDrag = FALSE;

/* Don't allow mouse event propagation during scroll */
bool g_blockEventsOnScroll = FALSE;

/* Don't allow window closing if there are open dialogs */
int g_openDialogs = 0;

/* TRUE when the message queue is empty. this gets set to
   FALSE by all event callbacks before anything else is done */
bool g_isIdle = FALSE;

