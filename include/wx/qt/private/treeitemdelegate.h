/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/private/treeitemdelegate.h
// Purpose:     Delegate to create text edit controls for the tree items
// Author:      Matthew Griffin
// Created:     2019-05-29
// Copyright:   Matthew Griffin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRIVATE_TREEITEM_DELEGATE_H
#define _WX_QT_PRIVATE_TREEITEM_DELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QToolTip>

#include "wx/app.h"
#include "wx/textctrl.h"

#include "treeitemfactory.h"

class wxQTTreeItemDelegate : public QStyledItemDelegate
{
public:
    explicit wxQTTreeItemDelegate(wxWindow* parent)
        : m_parent(parent),
        m_textCtrl(nullptr)
    {
    }

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &WXUNUSED(option), const QModelIndex &index) const override
    {
        if ( m_textCtrl != nullptr )
            destroyEditor(m_textCtrl->GetHandle(), m_currentModelIndex);

        m_currentModelIndex = index;
        m_textCtrl = new wxQtListTextCtrl(m_parent, parent);
        m_textCtrl->SetFocus();
        return m_textCtrl->GetHandle();
    }

    void destroyEditor(QWidget *WXUNUSED(editor), const QModelIndex &WXUNUSED(index)) const override
    {
        if ( m_textCtrl != nullptr )
        {
            m_currentModelIndex = QModelIndex(); // invalidate the index
            wxTheApp->ScheduleForDestruction(m_textCtrl);
            m_textCtrl = nullptr;
        }
    }

    void setModelData(QWidget *WXUNUSED(editor), QAbstractItemModel *WXUNUSED(model), const QModelIndex &WXUNUSED(index)) const override
    {
        // Don't set model data until wx has had a chance to send out events
    }

    wxTextCtrl* GetEditControl() const
    {
        return m_textCtrl;
    }

    QModelIndex GetCurrentModelIndex() const
    {
        return m_currentModelIndex;
    }

    void AcceptModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        if ( event->type() == QEvent::ToolTip )
        {
            const QRect &itemRect = view->visualRect(index);
            const QSize &bestSize = sizeHint(option, index);
            if ( itemRect.width() < bestSize.width() )
            {
                const QString &value = index.data(Qt::DisplayRole).toString();
                QToolTip::showText(event->globalPos(), value, view);
            }
            else
            {
                QToolTip::hideText();
            }

            return true;
        }

        return QStyledItemDelegate::helpEvent(event, view, option, index);
    }

private:
    wxWindow* m_parent;
    mutable wxTextCtrl* m_textCtrl;
    mutable QModelIndex m_currentModelIndex;
};

#endif // _WX_QT_PRIVATE_TREEITEM_DELEGATE_H
