/////////////////////////////////////////////////////////////////////////////
// Name:        _log.i
// Purpose:     SWIG interface stuff for wxLog
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


typedef unsigned long wxTraceMask;
typedef unsigned long wxLogLevel;


enum
{
    wxLOG_FatalError, // program can't continue, abort immediately
    wxLOG_Error,      // a serious error, user must be informed about it
    wxLOG_Warning,    // user is normally informed about it but may be ignored
    wxLOG_Message,    // normal message (i.e. normal output of a non GUI app)
    wxLOG_Status,     // informational: might go to the status line of GUI app
    wxLOG_Info,       // informational message (a.k.a. 'Verbose')
    wxLOG_Debug,      // never shown to the user, disabled in release mode
    wxLOG_Trace,      // trace messages are also only enabled in debug mode
    wxLOG_Progress,   // used for progress indicator (not yet)
    wxLOG_User = 100, // user defined levels start here
    wxLOG_Max = 10000
};

#define wxTRACE_MemAlloc "memalloc" // trace memory allocation (new/delete)
#define wxTRACE_Messages "messages" // trace window messages/X callbacks
#define wxTRACE_ResAlloc "resalloc" // trace GDI resource allocation
#define wxTRACE_RefCount "refcount" // trace various ref counting operations
#define wxTRACE_OleCalls "ole"      // OLE interface calls

#define wxTraceMemAlloc 0x0001  // trace memory allocation (new/delete)
#define wxTraceMessages 0x0002  // trace window messages/X callbacks
#define wxTraceResAlloc 0x0004  // trace GDI resource allocation
#define wxTraceRefCount 0x0008  // trace various ref counting operations
#define wxTraceOleCalls 0x0100  // OLE interface calls

//---------------------------------------------------------------------------

class wxLog
{
public:
    wxLog();
    ~wxLog();

    // these functions allow to completely disable all log messages
    // is logging disabled now?
    static bool IsEnabled();

    // change the flag state, return the previous one
    static bool EnableLogging(bool doIt = true);

    // static sink function
    static void OnLog(wxLogLevel level, const wxChar *szString, time_t t);    

    // message buffering
    // flush shows all messages if they're not logged immediately (FILE
    // and iostream logs don't need it, but wxGuiLog does to avoid showing
    // 17 modal dialogs one after another)
    virtual void Flush();

    // flush the active target if any
    static void FlushActive();

    // only one sink is active at each moment
    // get current log target, will call wxApp::CreateLogTarget() to
    // create one if none exists
    static wxLog *GetActiveTarget();

    %disownarg( wxLog* pLogger );
    %newobject SetActiveTarget;
    // change log target, pLogger may be NULL
    static wxLog *SetActiveTarget(wxLog *pLogger);
    %cleardisown( wxLog* pLogger );
    
    // suspend the message flushing of the main target until the next call
    // to Resume() - this is mainly for internal use (to prevent wxYield()
    // from flashing the messages)
    static void Suspend();

    // must be called for each Suspend()!
    static void Resume();


    // verbose mode is activated by standard command-line '-verbose'
    // option
    static void SetVerbose(bool bVerbose = true);

    // Set log level.  Log messages with level > logLevel will not be logged.
    static void SetLogLevel(wxLogLevel logLevel);

    // should GetActiveTarget() try to create a new log object if the
    // current is NULL?
    static void DontCreateOnDemand();

    // log the count of repeating messages instead of logging the messages
    // multiple times
    static void SetRepetitionCounting(bool bRepetCounting = true);

    // gets duplicate counting status
    static bool GetRepetitionCounting();

    // trace mask (see wxTraceXXX constants for details)
    static void SetTraceMask(wxTraceMask ulMask);

    // add string trace mask
    static void AddTraceMask(const wxString& str);
    
    // remove string trace mask
    static void RemoveTraceMask(const wxString& str);

    // remove all string trace masks
    static void ClearTraceMasks();

    // get string trace masks
    static const wxArrayString &GetTraceMasks();

    // sets the timestamp string: this is used as strftime() format string
    // for the log targets which add time stamps to the messages - set it
    // to NULL to disable time stamping completely.
    static void SetTimestamp(const wxChar *ts);


    // gets the verbose status
    static bool GetVerbose();
    
    // get trace mask
    static wxTraceMask GetTraceMask();

    // is this trace mask in the list?
    static bool IsAllowedTraceMask(const wxChar *mask);

    // return the current loglevel limit
    static wxLogLevel GetLogLevel();


    // get the current timestamp format string (may be NULL)
    static const wxChar *GetTimestamp();


    %extend {
        static wxString TimeStamp() {
            wxString msg;
            wxLog::TimeStamp(&msg);
            return msg;
        }
    }

    %pythonPrepend Destroy "args[0].this.own(False)";
    %extend { void Destroy() { delete self; } }
};


//---------------------------------------------------------------------------


class wxLogStderr : public wxLog
{
public:
    wxLogStderr(/* TODO: FILE *fp = (FILE *) NULL*/);
};


class wxLogTextCtrl : public wxLog
{
public:
    wxLogTextCtrl(wxTextCtrl *pTextCtrl);
};


class wxLogGui : public wxLog
{
public:
    wxLogGui();
};

class wxLogWindow : public wxLog
{
public:
    wxLogWindow(wxFrame *pParent,         // the parent frame (can be NULL)
            const wxString& szTitle,      // the title of the frame
            bool bShow = true,            // show window immediately?
            bool bPassToOld = true);      // pass log messages to the old target?

    void Show(bool bShow = true);
    wxFrame *GetFrame() const;
    wxLog *GetOldLog() const;
    bool IsPassingMessages() const;
    void PassMessages(bool bDoPass);
    
    %property(Frame, GetFrame, doc="See `GetFrame`");
    %property(OldLog, GetOldLog, doc="See `GetOldLog`");
};


class wxLogChain : public wxLog
{
public:
    wxLogChain(wxLog *logger);
    void SetLog(wxLog *logger);
    void PassMessages(bool bDoPass);
    bool IsPassingMessages();
    wxLog *GetOldLog();

    %property(OldLog, GetOldLog, doc="See `GetOldLog`");    
};

// log everything to a buffer
class wxLogBuffer : public wxLog
{
public:
    wxLogBuffer();

    // get the string contents with all messages logged
    const wxString& GetBuffer() const;

    // show the buffer contents to the user in the best possible way (this uses
    // wxMessageOutputMessageBox) and clear it
    virtual void Flush();

    %property(Buffer, GetBuffer, doc="See `GetBuffer`");    
};


//---------------------------------------------------------------------------

unsigned long wxSysErrorCode();
const wxString wxSysErrorMsg(unsigned long nErrCode = 0);

%{// Make some wrappers that double any % signs so they are 'escaped'
    void wxPyLogFatalError(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogFatalError(m);
    }
    
    void wxPyLogError(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogError(m);
    }

    void wxPyLogWarning(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogWarning(m);
    }

    void wxPyLogMessage(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogMessage(m);
    }

    void wxPyLogInfo(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogInfo(m);
    }

    void wxPyLogDebug(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogDebug(m);
    }

    void wxPyLogVerbose(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogVerbose(m);
    }

    void wxPyLogStatus(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogStatus(m);
    }

    void wxPyLogStatusFrame(wxFrame *pFrame, const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogStatus(pFrame, m);
    }

    void wxPyLogSysError(const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogSysError(m);
    }

    void wxPyLogGeneric(unsigned long level, const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogGeneric(level, m);
    }

    void wxPyLogTrace(unsigned long mask, const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogTrace(mask, m);
    }
        
    void wxPyLogTrace(const wxString& mask, const wxString& msg)
    {
        wxString m(msg);
        m.Replace(wxT("%"), wxT("%%"));
        wxLogTrace(mask, m);
    }
    
%}

%Rename(LogFatalError,  void, wxPyLogFatalError(const wxString& msg));
%Rename(LogError,  void, wxPyLogError(const wxString& msg));
%Rename(LogWarning,  void, wxPyLogWarning(const wxString& msg));
%Rename(LogMessage,  void, wxPyLogMessage(const wxString& msg));
%Rename(LogInfo,  void, wxPyLogInfo(const wxString& msg));
%Rename(LogDebug,  void, wxPyLogDebug(const wxString& msg));
%Rename(LogVerbose,  void, wxPyLogVerbose(const wxString& msg));
%Rename(LogStatus,  void, wxPyLogStatus(const wxString& msg));
%Rename(LogStatusFrame,  void, wxPyLogStatusFrame(wxFrame *pFrame, const wxString& msg));
%Rename(LogSysError,  void, wxPyLogSysError(const wxString& msg));

%Rename(LogGeneric,  void, wxPyLogGeneric(unsigned long level, const wxString& msg));

%nokwargs wxPyLogTrace;
%Rename(LogTrace,  void, wxPyLogTrace(unsigned long mask, const wxString& msg));
%Rename(LogTrace,  void, wxPyLogTrace(const wxString& mask, const wxString& msg));


// wxLogFatalError helper: show the (fatal) error to the user in a safe way,
// i.e. without using wxMessageBox() for example because it could crash
void wxSafeShowMessage(const wxString& title, const wxString& text);



// Suspress logging while an instance of this class exists
class wxLogNull
{
public:
    wxLogNull();
    ~wxLogNull();
};



//---------------------------------------------------------------------------

%{
// A wxLog class that can be derived from in wxPython
class wxPyLog : public wxLog {
public:
    wxPyLog() : wxLog() {}

    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t) {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLog"))) {
            PyObject* s = wx2PyString(szString);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iOi)", level, s, t));
            Py_DECREF(s);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxLog::DoLog(level, szString, t);
    }

    virtual void DoLogString(const wxChar *szString, time_t t) {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLogString"))) {
            PyObject* s = wx2PyString(szString);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", s, t));
            Py_DECREF(s);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxLog::DoLogString(szString, t);
    }

    DEC_PYCALLBACK_VOID_(Flush);
    PYPRIVATE;
};
IMP_PYCALLBACK_VOID_(wxPyLog, wxLog, Flush);
%}

// Now tell SWIG about it
class wxPyLog : public wxLog {
public:
    %pythonAppend wxPyLog  setCallbackInfo(PyLog)

    wxPyLog();
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};

//---------------------------------------------------------------------------
