/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dialog_qt.h
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DIALOG_QT_H_
#define _WX_QT_DIALOG_QT_H_

#include "wx/dialog.h"
#include "wx/qt/winevent_qt.h"
#include <QtGui/QDialog>

class WXDLLIMPEXP_CORE wxQtDialog : public wxQtEventForwarder< QDialog >
{
    Q_OBJECT

    public:
        wxQtDialog( wxDialog *dialog, QWidget *parent );

    protected:
        virtual wxWindow *GetEventReceiver();

    private:
        wxDialog *m_dialog;

};

#endif // _WX_QT_DIALOG_QT_H_
