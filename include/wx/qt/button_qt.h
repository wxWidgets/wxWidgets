/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/qt/button_qt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_QT_H_
#define _WX_QT_BUTTON_QT_H_

#include <QtGui/QPushButton>
#include "wx/dlimpexp.h"

class WXDLLIMPEXP_CORE wxQtButton : public QPushButton
{
    Q_OBJECT

    public:
        wxQtButton( const QString &text, QWidget *parent );
};

#endif
