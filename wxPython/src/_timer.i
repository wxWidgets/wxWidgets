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
    wxTIMER_ONE_SHOT,
};

// Timer event type
%constant wxEventType wxEVT_TIMER;


//---------------------------------------------------------------------------


%{
IMP_PYCALLBACK__(wxPyTimer, wxTimer, Notify);

IMPLEMENT_ABSTRACT_CLASS(wxPyTimer, wxTimer);

wxPyTimer::wxPyTimer(wxEvtHandler *owner, int id)
    : wxTimer(owner, id)
{
    if (owner == NULL)
        SetOwner(this);
}

wxPyTimer::~wxPyTimer()
{
//    printf("-=* ~wxPyTimer\n");
}

%}


MustHaveApp(wxPyTimer);

%rename(Timer) wxPyTimer;
class wxPyTimer : public wxEvtHandler
{
public:
    // Don't let the OOR or callback info hold references to the object so
    // there won't be a reference cycle and it can clean itself up via normal
    // Python refcounting
    %pythonAppend wxPyTimer
        "self._setOORInfo(self,0); self.this.own(True); " setCallbackInfo(Timer); 

    // if you don't call SetOwner() or provide an owner in the ctor
    // then you must override Notify() in order to receive the timer
    // notification.  If the owner is set then it will get the timer
    // notifications which can be handled with EVT_TIMER.
    wxPyTimer(wxEvtHandler *owner = NULL, int id = wxID_ANY);

    // Destructor.  
    virtual ~wxPyTimer();

    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref = 0);

    // Set the owner instance that will receive the EVT_TIMER events
    // using the given id.
    void SetOwner(wxEvtHandler *owner, int id = wxID_ANY);
    wxEvtHandler* GetOwner();

    // start the timer: if milliseconds == -1, use the same value as for the
    // last Start()
    //
    // it is now valid to call Start() multiple times: this just restarts the
    // timer if it is already running
    virtual bool Start(int milliseconds = -1, bool oneShot = false);

    // stop the timer
    virtual void Stop();

    // override this in your wxTimer-derived class if you need to process timer
    // messages in it; otherwise, use non-default ctor or call SetOwner()
    virtual void Notify();

    // return True if the timer is running
    virtual bool IsRunning() const;

    // get the (last) timer interval in milliseconds
    int GetInterval() const;

    // return the timer ID
    int GetId() const;

    // return True if the timer is one shot
    bool IsOneShot() const;

    %pythoncode {
        def Destroy(self):
            """NO-OP: Timers must be destroyed by normal reference counting"""
            pass
    }

    %property(Id, GetId, doc="See `GetId`");
    %property(Interval, GetInterval, doc="See `GetInterval`");
    %property(Owner, GetOwner, SetOwner, doc="See `GetOwner` and `SetOwner`");
};


%pythoncode {
%# For backwards compatibility with 2.4
class PyTimer(Timer):
    def __init__(self, notify):
        Timer.__init__(self)
        self.notify = notify

    def Notify(self):
        if self.notify:
            self.notify()


EVT_TIMER = wx.PyEventBinder( wxEVT_TIMER, 1 )

};



class wxTimerEvent : public wxEvent
{
public:
    wxTimerEvent(int timerid = 0, int interval = 0);
    int GetInterval() const;

    %property(Interval, GetInterval, doc="See `GetInterval`");
};



// wxTimerRunner: starts the timer in its ctor, stops in the dtor
MustHaveApp(wxTimerRunner);
class wxTimerRunner
{
public:
    %nokwargs wxTimerRunner;
    wxTimerRunner(wxTimer& timer);
    wxTimerRunner(wxTimer& timer, int milli, bool oneShot = false);
    ~wxTimerRunner();

    void Start(int milli, bool oneShot = false);
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxTimer", "wxPyTimer");
%}
//---------------------------------------------------------------------------
