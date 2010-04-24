/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/action_qt.h
// Purpose:     wxQtAction class interface
// Author:      Peter Most
// Modified by:
// Created:     09.08.09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ACTION_QT_H_
#define _WX_QT_ACTION_QT_H_

#include <QtGui/QAction>
#include "wx/dlimpexp.h"


template < typename Receiver >
    class wxQtSignalReceiver
    {
    public:
        wxQtSignalReceiver( Receiver *receiver )
        {
            m_receiver = receiver;
        }

        Receiver *GetSignalReceiver() const
        {
            return m_receiver;
        }

    private:
        Receiver *m_receiver;
    };



class WXDLLIMPEXP_FWD_CORE wxMenuItem;

class WXDLLIMPEXP_CORE wxQtAction : public QAction, protected wxQtSignalReceiver< wxMenuItem >
{
    Q_OBJECT
    
public:
    wxQtAction( wxMenuItem *menuItem, const QString &text, QObject *parent );

private slots:
    void OnActionTriggered( bool checked );
};

#endif

