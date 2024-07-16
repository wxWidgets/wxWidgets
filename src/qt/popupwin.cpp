/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/popupwin.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/popupwin.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QWidget>


class wxQtPopupWindow : public wxQtEventSignalHandler<QWidget, wxPopupWindow>
{
public:
    wxQtPopupWindow(wxWindow* parent, wxPopupWindow* handler);

protected:

    virtual void closeEvent(QCloseEvent *event) override
    {
        QWidget::closeEvent(event);
    }
};

wxQtPopupWindow::wxQtPopupWindow(wxWindow* parent, wxPopupWindow* handler)
    : wxQtEventSignalHandler<QWidget, wxPopupWindow>(parent, handler)
{
}

wxPopupWindow::wxPopupWindow(wxWindow *parent, int flags)
{
   Create(parent, flags);
}

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    m_qtWindow = new wxQtPopupWindow(parent, this);

    m_qtWindow->setWindowFlag(Qt::Popup);
    m_qtWindow->setFocusPolicy(Qt::NoFocus);

    // all dialogs should have tab traversal enabled
    style |= wxTAB_TRAVERSAL;

    // Unlike windows, top level windows are created hidden by default.
    m_isShown = false;

    return wxPopupWindowBase::Create(parent, style) &&
            wxWindow::Create( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style );
}
