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
#include "wx/window.h"

#include "wx/qt/private/converter.h"

#include <QtGui/QDrag>
#include <QtWidgets/QWidget>
#include <QtCore/QMimeData>
#include <QtGui/QCloseEvent>

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
        case Qt::TargetMoveAction:
            return wxDragMove;
        case Qt::LinkAction:
            return wxDragLink;
        default:
            break;
    }

    wxFAIL_MSG("Illegal drop action");
    return wxDragNone;
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
        case wxDragError:
        case wxDragNone:
        case wxDragCancel:
            return Qt::IgnoreAction;
    }

    wxFAIL_MSG("Illegal drag result");
    return Qt::IgnoreAction;
}

void AddDataFormat(wxDataObject* dataObject,
                   QMimeData* mimeData,
                   const wxDataFormat& format)
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

void SetDragCursor(QDrag& drag,
                   const wxCursor& cursor,
                   Qt::DropAction action)
{
    if ( cursor.IsOk() )
        drag.setDragCursor(cursor.GetHandle().pixmap(), action);
}

class PendingMimeDataSetter
{
public:
    PendingMimeDataSetter(const QMimeData*& targetMimeData,
                          const QMimeData* mimeData)
        : m_targetMimeData(targetMimeData)
    {
        m_targetMimeData = mimeData;
    }

    ~PendingMimeDataSetter()
    {
        m_targetMimeData = nullptr;
    }

private:
    const QMimeData*& m_targetMimeData;
};

} // anonymous namespace

class wxDropTarget::Impl : public QObject
{
public:
    explicit Impl(wxDropTarget* dropTarget)
        : m_dropTarget(dropTarget),
          m_widget(nullptr),
          m_pendingMimeData(nullptr)
    {
    }

    ~Impl()
    {
        Disconnect();
    }

    void ConnectTo(QWidget* widget)
    {
        Disconnect();

        m_widget = widget;

        if ( m_widget != nullptr )
        {
            m_widget->setAcceptDrops(true);
            m_widget->installEventFilter(this);
        }
    }

    void Disconnect()
    {
        if ( m_widget != nullptr )
        {
            m_widget->setAcceptDrops(false);
            m_widget->removeEventFilter(this);
            m_widget = nullptr;
        }
    }

    virtual bool eventFilter(QObject* watched, QEvent* event) override
    {
        if ( m_dropTarget != nullptr )
        {
            switch ( event->type() )
            {
                case QEvent::Drop:
                    OnDrop(event);
                    return true;

                case QEvent::DragEnter:
                    OnEnter(event);
                    return true;

                case QEvent::DragMove:
                    OnMove(event);
                    return true;

                case QEvent::DragLeave:
                    OnLeave(event);
                    return true;

                default:
                    break;
            }
        }

        return QObject::eventFilter(watched, event);
    }

    void OnEnter(QEvent* event)
    {
        QDragEnterEvent *e = static_cast<QDragEnterEvent*>(event);

        const PendingMimeDataSetter setter(m_pendingMimeData, e->mimeData());

        if ( !CanDropHere() )
        {
            e->setDropAction(Qt::IgnoreAction);
            return;
        }

        event->accept();

        const QPoint where = e->pos();
        const wxDragResult proposedResult =
            DropActionToDragResult(e->proposedAction());
        const wxDragResult result = m_dropTarget->OnEnter(where.x(),
                                                          where.y(),
                                                          proposedResult);

        e->setDropAction(DragResultToDropAction(result));
    }

    void OnLeave(QEvent* event)
    {
        event->accept();
        m_dropTarget->OnLeave();
    }

    void OnMove(QEvent* event)
    {
        event->accept();

        QDragMoveEvent *e = static_cast<QDragMoveEvent*>(event);

        const PendingMimeDataSetter setter(m_pendingMimeData, e->mimeData());

        const QPoint where = e->pos();
        const wxDragResult proposedResult =
            DropActionToDragResult(e->proposedAction());
        const wxDragResult result = m_dropTarget->OnDragOver(where.x(),
                                                             where.y(),
                                                             proposedResult);

        e->setDropAction(DragResultToDropAction(result));
    }

    void OnDrop(QEvent* event)
    {
        event->accept();

        const QDropEvent *e = static_cast<QDropEvent*>(event);

        const PendingMimeDataSetter setter(m_pendingMimeData, e->mimeData());

        const QPoint where = e->pos();
        if ( m_dropTarget->OnDrop(where.x(), where.y()) )
        {
            m_dropTarget->OnData(where.x(),
                                 where.y(),
                                 DropActionToDragResult(e->dropAction()));
        }
    }

    const QMimeData* GetMimeData() const
    {
        return m_pendingMimeData;
    }

private:

    bool CanDropHere() const
    {
        return !m_dropTarget->GetMatchingPair().GetMimeType().empty();
    }

    wxDropTarget* m_dropTarget;
    QWidget* m_widget;
    const QMimeData* m_pendingMimeData;
};

wxDropTarget::wxDropTarget(wxDataObject *dataObject)
    : wxDropTargetBase(dataObject),
      m_pImpl(new Impl(this))
{
}

wxDropTarget::~wxDropTarget()
{
    delete m_pImpl;
}

bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return true;
}

wxDragResult wxDropTarget::OnData(wxCoord WXUNUSED(x),
                                  wxCoord WXUNUSED(y),
                                  wxDragResult def)
{
    return GetData() ? def : wxDragNone;
}

bool wxDropTarget::GetData()
{
    const wxDataFormat droppedFormat = GetMatchingPair();

    const wxString& mimeType = droppedFormat.GetMimeType();
    if ( mimeType.empty() )
        return false;

    const QString qMimeType = wxQtConvertString(mimeType);
    const QByteArray data = m_pImpl->GetMimeData()->data(qMimeType);

    return m_dataObject->SetData(droppedFormat, data.size(), data.data());
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    const QMimeData* mimeData = m_pImpl->GetMimeData();
    if ( mimeData == nullptr || m_dataObject == nullptr )
        return wxFormatInvalid;

    const QStringList formats = mimeData->formats();
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

void wxDropTarget::ConnectTo(QWidget* widget)
{
    m_pImpl->ConnectTo(widget);
}

void wxDropTarget::Disconnect()
{
    m_pImpl->Disconnect();
}

//###########################################################################

wxDropSource::wxDropSource(wxWindow *win,
              const wxCursor &copy,
              const wxCursor &move,
              const wxCursor &none)
    : wxDropSourceBase(copy, move, none),
      m_parentWindow(win)
{
}

wxDropSource::wxDropSource(wxDataObject& data,
              wxWindow *win,
              const wxCursor &copy,
              const wxCursor &move,
              const wxCursor &none)
    : wxDropSourceBase(copy, move, none),
      m_parentWindow(win)
{
    SetData(data);
}

wxDragResult wxDropSource::DoDragDrop(int flags /*=wxDrag_CopyOnly*/)
{
    wxCHECK_MSG(m_data != nullptr, wxDragNone,
                wxT("No data in wxDropSource!"));

    wxCHECK_MSG(m_parentWindow != nullptr, wxDragNone,
                wxT("null parent window in wxDropSource!"));

    QDrag drag(m_parentWindow->GetHandle());
    drag.setMimeData(CreateMimeData(m_data));

    SetDragCursor(drag, m_cursorCopy, Qt::CopyAction);
    SetDragCursor(drag, m_cursorMove, Qt::MoveAction);
    SetDragCursor(drag, m_cursorStop, Qt::IgnoreAction);

    const Qt::DropActions actions =
        flags == wxDrag_CopyOnly
            ? Qt::CopyAction
            : Qt::CopyAction | Qt::MoveAction;

    const Qt::DropAction defaultAction =
        flags == wxDrag_DefaultMove
            ? Qt::MoveAction
            : Qt::CopyAction;

    return DropActionToDragResult(drag.exec(actions, defaultAction));
}

#endif // wxUSE_DRAG_AND_DROP
