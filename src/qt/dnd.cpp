/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dnd.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dnd.h"

wxDropTarget::wxDropTarget(wxDataObject *dataObject)
{
}

bool wxDropTarget::OnDrop(wxCoord x, wxCoord y)
{
    return false;
}

wxDragResult wxDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    return wxDragResult();
}

bool wxDropTarget::GetData()
{
    return false;
}

//##############################################################################


wxDropSource::wxDropSource( wxWindow *win,
              const wxIcon &copy,
              const wxIcon &move,
              const wxIcon &none)
{
}

wxDropSource::wxDropSource( wxDataObject& data,
              wxWindow *win,
              const wxIcon &copy,
              const wxIcon &move,
              const wxIcon &none)
{
}

wxDragResult wxDropSource::DoDragDrop(int flags)
{
    return wxDragResult();
}

