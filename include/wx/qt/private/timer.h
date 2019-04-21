/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/timer.h
// Author:      Javier Torres
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TIMER_H_
#define _WX_QT_TIMER_H_

#if wxUSE_TIMER

#include <QtCore/QObject>
#include "wx/private/timer.h"

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

class QTimerEvent;
class WXDLLIMPEXP_CORE wxQtTimerImpl : public wxTimerImpl, QObject
{
public:
    wxQtTimerImpl( wxTimer* timer );

    virtual bool Start( int millisecs = -1, bool oneShot = false ) wxOVERRIDE;
    virtual void Stop() wxOVERRIDE;
    virtual bool IsRunning() const wxOVERRIDE;

protected:
    virtual void timerEvent( QTimerEvent * event ) wxOVERRIDE;

private:
    int m_timerId;
};

#endif // wxUSE_TIMER

#endif // _WX_QT_TIMER_H_
