/////////////////////////////////////////////////////////////////////////////
// Name:        _timer.i
// Purpose:     SWIG interface definitions wxTimer
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

enum {
    // generate notifications periodically until the timer is stopped (default)
    wxTIMER_CONTINUOUS,

    // only send the notification once and then stop the timer
    wxTIMER_ONE_SHOT
};
   

//---------------------------------------------------------------------------


%{
IMP_PYCALLBACK__(wxPyTimer, wxTimer, Notify);
%}



%name(Timer) class wxPyTimer : public wxEvtHandler
{
public:

    // if you don't call SetOwner() or provide an owner in the contstructor
    // then you must override Notify() inorder to receive the timer
    // notification.  If the owner is set then it will get the timer
    // notifications which can be handled with EVT_TIMER.
    wxPyTimer(wxEvtHandler *owner=NULL, int id = -1);
    virtual ~wxPyTimer();

    // Set the owner instance that will receive the EVT_TIMER events using the
    // given id.
    void SetOwner(wxEvtHandler *owner, int id = -1);


    // start the timer: if milliseconds == -1, use the same value as for the
    // last Start()
    //
    // it is now valid to call Start() multiple times: this just restarts the
    // timer if it is already running
    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE);

    // stop the timer
    virtual void Stop();

    // override this in your wxTimer-derived class if you want to process timer
    // messages in it, use non default ctor or SetOwner() otherwise
    virtual void Notify();

    // return TRUE if the timer is running
    virtual bool IsRunning() const;

    // get the (last) timer interval in the milliseconds
    int GetInterval() const;

    // return TRUE if the timer is one shot
    bool IsOneShot() const;
};


%pythoncode {    
%# For backwards compatibility with 2.4
class PyTimer(Timer):
    def __init__(self, notify):
        self.notify = self.notify

    def Notify(self):
        if self.notify:
            self.notify()
};



//---------------------------------------------------------------------------
