/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/evtloop_qt.h
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <QtCore/QTimer>

class wxQtEventLoopBase;

class wxQtIdleTimer : public QTimer
{
Q_OBJECT

public:
    wxQtIdleTimer( wxQtEventLoopBase *eventLoop );
    virtual bool eventFilter( QObject * watched, QEvent * event  );

private Q_SLOTS:
    void idle();

private:
    wxQtEventLoopBase *m_eventLoop;
};
