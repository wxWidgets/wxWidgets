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
#include <QCloseEvent>

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

    Qt::DropAction DragResultToDropAction(wxDragResult result)
    {
        switch ( result )
        {
            case wxDragCopy:
                return Qt::CopyAction;
            case wxDragMove:
                return Qt::MoveAction;
            case wxDragLink:
                return Qt::LinkAction;
            default:
                return Qt::IgnoreAction;
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


class wxDropTarget::Impl
{
public:
    const QMimeData* m_pendingMimeData;
};

namespace
{
    class PendingMimeDataSetter
    {
    public:
        PendingMimeDataSetter(const QMimeData*& targetMimeData, const QMimeData* mimeData)
            : m_targetMimeData(targetMimeData)
        {
            m_targetMimeData = mimeData;
        }

        ~PendingMimeDataSetter()
        {
            m_targetMimeData = NULL;
        }

    private:
        const QMimeData*& m_targetMimeData;
    };
}

wxDropTarget::wxDropTarget(wxDataObject *dataObject)
    : wxDropTargetBase(dataObject),
      m_pImpl(new Impl)
{
}

wxDropTarget::~wxDropTarget()
{
    delete m_pImpl;
}

bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return !GetMatchingPair().GetMimeType().empty();
}

wxDragResult wxDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def)
{
    GetData();
    return def;
}

bool wxDropTarget::GetData()
{
    const wxDataFormat droppedFormat = GetMatchingPair();

    const wxString mimeType = droppedFormat.GetMimeType();
    if ( mimeType.empty() )
        return false;

    const QByteArray data = m_pImpl->m_pendingMimeData->data(wxQtConvertString(mimeType));
    
    return m_dataObject->SetData(droppedFormat, data.size(), data.data());
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    if ( m_pImpl->m_pendingMimeData == NULL || m_dataObject == NULL )
        return wxFormatInvalid;

    const QStringList formats = m_pImpl->m_pendingMimeData->formats();
    for ( int i = 0; i < formats.count(); ++i )
    {
        const wxDataFormat format(wxQtConvertString(formats[i]));

        if ( m_dataObject->IsSupportedFormat(format) )
        {
            return format;
        }
    }
    return wxFormatInvalid;
}

void wxDropTarget::OnQtEnter(QEvent* event)
{
    event->accept();
    
    QDragEnterEvent *e = static_cast<QDragEnterEvent*>(event);
    const QPoint where = e->pos();

    PendingMimeDataSetter setter(m_pImpl->m_pendingMimeData, e->mimeData());

    wxDragResult result = OnEnter(where.x(), where.y(), DropActionToDragResult(e->proposedAction()));

    e->setDropAction(DragResultToDropAction(result));
}

void wxDropTarget::OnQtLeave(QEvent* event)
{
    event->accept();
    OnLeave();
}

void wxDropTarget::OnQtMove(QEvent* event)
{
    event->accept();

    QDragMoveEvent *e = static_cast<QDragMoveEvent*>(event);
    const QPoint where = e->pos();

    PendingMimeDataSetter setter(m_pImpl->m_pendingMimeData, e->mimeData());

    wxDragResult result = OnDragOver(where.x(), where.y(), DropActionToDragResult(e->proposedAction()));

    e->setDropAction(DragResultToDropAction(result));
}

void wxDropTarget::OnQtDrop(QEvent* event)
{
    event->accept();

    const QDropEvent *e = static_cast<QDropEvent*>(event);
    const QPoint where = e->pos();

    PendingMimeDataSetter setter(m_pImpl->m_pendingMimeData, e->mimeData());

    if ( OnDrop(where.x(), where.y()) )
    {
        OnData(where.x(), where.y(), DropActionToDragResult(e->dropAction()));
    }
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
