/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_TIMER_H__
#define __WX_TIMER_H__

#ifdef __GNUG__
    #pragma interface "timer.h"
#endif

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

//FIXME_MGL
class WXDLLEXPORT wxTimer : public wxTimerBase
{
public:
    wxTimer() { Init(); }
    wxTimer(wxEvtHandler *owner, int id = -1) : wxTimerBase(owner, id)
        { Init(); }
    ~wxTimer() {}

    virtual bool Start( int millisecs = -1, bool oneShot = FALSE ) {}
    virtual void Stop() {}

    virtual bool IsRunning() const { return m_tag != -1; }

protected:
    void Init() {}

    int  m_tag;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif // __GTKTIMERH__
