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
#include "wx/scopedarray.h"

#include "wx/qt/private/converter.h"

#include <QDrag>
#include <QWidget>
#include <QMimeData>

namespace
{
    wxDragResult DropActionToDragResult(Qt::DropAction action)
    {
        switch ( action )
        {
            case Qt::IgnoreAction:
                return wxDragCancel;
            case Qt::CopyAction:
                return wxDragCopy;
            case Qt::MoveAction:
                return wxDragMove;
            case Qt::LinkAction:
                return wxDragLink;
            default:
                return wxDragNone;
        }
    }

    void AddDataFormat(wxDataObject* dataObject, QMimeData* mimeData, const wxDataFormat& format)
    {
        const size_t data_size = dataObject->GetDataSize(format);

        QByteArray data(static_cast<int>(data_size), Qt::Initialization());
        dataObject->GetDataHere(format, data.data());

        mimeData->setData(wxQtConvertString(format.GetMimeType()), data);
    }

    QMimeData* CreateMimeData(wxDataObject* dataObject)
    {
        QMimeData* mimeData = new QMimeData();

        const size_t count = dataObject->GetFormatCount();

        wxScopedArray<wxDataFormat> array(dataObject->GetFormatCount());
        dataObject->GetAllFormats(array.get());

        for ( size_t i = 0; i < count; i++ )
        {
            AddDataFormat(dataObject, mimeData, array[i]);
        }

        return mimeData;
    }
}

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

wxDropSource::wxDropSource( wxWindow *win,
              const wxIcon &WXUNUSED(copy),
              const wxIcon &WXUNUSED(move),
              const wxIcon &WXUNUSED(none))
    : m_parentWindow(win)
{
}

wxDropSource::wxDropSource( wxDataObject& data,
              wxWindow *win,
              const wxIcon &WXUNUSED(copy),
              const wxIcon &WXUNUSED(move),
              const wxIcon &WXUNUSED(none))
    : m_parentWindow(win)
{
    SetData(data);
}

wxDragResult wxDropSource::DoDragDrop(int flags /*=wxDrag_CopyOnly*/)
{
    wxCHECK_MSG(m_data != NULL, wxDragNone, wxT("No data in wxDropSource!"));
    wxCHECK_MSG(m_parentWindow != NULL, wxDragNone, wxT("NULL parent window in wxDropSource!"));

    QDrag drag(m_parentWindow->GetHandle());
    drag.setMimeData(CreateMimeData(m_data));

    Qt::DropActions actions = Qt::CopyAction | Qt::MoveAction;
    Qt::DropAction defaultAction = Qt::CopyAction;
    switch ( flags )
    {
        case wxDrag_CopyOnly:
            actions = Qt::CopyAction;
            break;
        case wxDrag_DefaultMove:
            defaultAction = Qt::MoveAction;
            break;
        default:
            break;
    }

    return DropActionToDragResult(drag.exec(actions, defaultAction));
}

#endif // wxUSE_DRAG_AND_DROP
