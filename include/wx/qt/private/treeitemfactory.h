/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/private/treeitemfactory.h
// Purpose:     Factory to create text edit controls for the tree items
// Author:      Graham Dawes
// Created:     2019-02-07
// Copyright:   Graham Dawes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRIVATE_TREEITEM_FACTORY_H_
#define _WX_QT_PRIVATE_TREEITEM_FACTORY_H_

#include <QtWidgets/QItemEditorFactory>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QItemDelegate>

#include "wx/recguard.h"
#include "wx/textctrl.h"

// wxQT Doesn't have a mechanism for "adopting" external widgets so we have to
// create an instance of wxTextCtrl rather than adopting the control QT would
// create.
//
// Unfortunately the factory is given an internal widget as the parent for
// editor.
//
// To work around these issues we create a wxTextCtl parented by the wxListCtrl
// then recalculate its position relative to the internal widget.

class wxQtListTextCtrl : public wxTextCtrl
{
public:
    wxQtListTextCtrl(wxWindow* parent, QWidget* actualParent)
        : wxTextCtrl(parent, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize,
            wxNO_BORDER),
        m_actualParent(actualParent),
        m_moving(0)
    {
        Bind(wxEVT_MOVE, &wxQtListTextCtrl::OnMove, this);
    }

    void OnMove(wxMoveEvent &event)
    {
        // QWidget::move generates a QMoveEvent so we need to guard against
        // reentrant calls.
        wxRecursionGuard guard(m_moving);

        if ( guard.IsInside() )
        {
            event.Skip();
            return;
        }

        const QPoint eventPos = wxQtConvertPoint(event.GetPosition());
        const QPoint globalPos = m_actualParent->mapToGlobal(eventPos);

        // For some reason this always gives us the offset from the header info
        // the internal control. So we need to treat this as an offset rather
        // than a position.
        QWidget* widget = GetHandle();
        const QPoint offset = widget->mapFromGlobal(globalPos);

        widget->move(eventPos + offset);
    }

private:
    QWidget* m_actualParent;
    wxRecursionGuardFlag m_moving;

    wxDECLARE_NO_COPY_CLASS(wxQtListTextCtrl);
};

// QT doesn't give us direct access to the editor within the QTreeWidget.
// Instead, we'll supply a factory to create the widget for QT and keep track
// of it ourselves.

class wxQtTreeItemEditorFactory : public QItemEditorFactory
{
public:
    explicit wxQtTreeItemEditorFactory(wxWindow* parent)
        : m_parent(parent),
          m_textCtrl(nullptr)
    {
    }

    void AttachTo(QTreeWidget *tree)
    {
        QAbstractItemDelegate* delegate = tree->itemDelegate();
        QItemDelegate *qItemDelegate = static_cast<QItemDelegate*>(delegate);
        qItemDelegate->setItemEditorFactory(this);
    }

    QWidget* createEditor(int WXUNUSED(userType), QWidget* parent) const override
    {
        if (m_textCtrl != nullptr)
            ClearEditor();

        m_textCtrl = new wxQtListTextCtrl(m_parent, parent);
        m_textCtrl->SetFocus();
        return m_textCtrl->GetHandle();
    }

    wxTextCtrl* GetEditControl() const
    {
        return m_textCtrl;
    }

    void ClearEditor() const
    {
        delete m_textCtrl;
        m_textCtrl = nullptr;
    }

private:
    wxWindow* m_parent;
    mutable wxTextCtrl* m_textCtrl;

    wxDECLARE_NO_COPY_CLASS(wxQtTreeItemEditorFactory);
};

#endif //_WX_QT_PRIVATE_TREEITEM_FACTORY_H_
