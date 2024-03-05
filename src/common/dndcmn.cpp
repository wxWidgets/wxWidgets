///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dndcmn.cpp
// Author:      Robert Roebling
// Created:     19.10.99
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#include "wx/dnd.h"

#if wxUSE_DRAG_AND_DROP

bool wxIsDragResultOk(wxDragResult res)
{
    return res == wxDragCopy || res == wxDragMove || res == wxDragLink;
}

#endif

