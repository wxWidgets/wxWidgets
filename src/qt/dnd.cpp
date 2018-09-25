/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dnd.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"

wxDropTarget::wxDropTarget(wxDataObject *WXUNUSED(dataObject))
{
}

bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return false;
}

wxDragResult wxDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult WXUNUSED(def))
{
    return wxDragResult();
}

bool wxDropTarget::GetData()
{
    return false;
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    wxFAIL_MSG("wxDropTarget::GetMatchingPair() not implemented in src/qt/dnd.cpp");
    return wxDF_INVALID;
}

//##############################################################################


wxDropSource::wxDropSource( wxWindow *WXUNUSED(win),
              const wxIcon &WXUNUSED(copy),
              const wxIcon &WXUNUSED(move),
              const wxIcon &WXUNUSED(none))
{
}

wxDropSource::wxDropSource( wxDataObject& WXUNUSED(data),
              wxWindow *WXUNUSED(win),
              const wxIcon &WXUNUSED(copy),
              const wxIcon &WXUNUSED(move),
              const wxIcon &WXUNUSED(none))
{
}

wxDragResult wxDropSource::DoDragDrop(int WXUNUSED(flags))
{
    return wxDragResult();
}

#endif // wxUSE_DRAG_AND_DROP
