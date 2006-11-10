/////////////////////////////////////////////////////////////////////////////
// Name:        _process.i
// Purpose:     SWIG interface stuff for wxProcess and wxExecute
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

%{
%}

//---------------------------------------------------------------------------

enum
{
    // no redirection
    wxPROCESS_DEFAULT = 0,

    // redirect the IO of the child process
    wxPROCESS_REDIRECT = 1
};

enum wxKillError
{
    wxKILL_OK,              // no error
    wxKILL_BAD_SIGNAL,      // no such signal
    wxKILL_ACCESS_DENIED,   // permission denied
    wxKILL_NO_PROCESS,      // no such process
    wxKILL_ERROR            // another, unspecified error
};

enum wxKillFlags
{
    wxKILL_NOCHILDREN = 0,  // don't kill children
    wxKILL_CHILDREN = 1     // kill children
};


enum wxSignal
{
    wxSIGNONE = 0,  // verify if the process exists under Unix
    wxSIGHUP,
    wxSIGINT,
    wxSIGQUIT,
    wxSIGILL,
    wxSIGTRAP,
    wxSIGABRT,
    wxSIGIOT = wxSIGABRT,   // another name
    wxSIGEMT,
    wxSIGFPE,
    wxSIGKILL,
    wxSIGBUS,
    wxSIGSEGV,
    wxSIGSYS,
    wxSIGPIPE,
    wxSIGALRM,
    wxSIGTERM

    // further signals are different in meaning between different Unix systems
};


//---------------------------------------------------------------------------


%{ 
IMP_PYCALLBACK_VOID_INTINT( wxPyProcess, wxProcess, OnTerminate);
%}


%rename(Process) wxPyProcess;
class wxPyProcess : public wxEvtHandler {
public:
    // kill the process with the given PID
    static wxKillError Kill(int pid,
                            wxSignal sig = wxSIGTERM,
                            int flags = wxKILL_NOCHILDREN);

    // test if the given process exists
    static bool Exists(int pid);

    // this function replaces the standard popen() one: it launches a process
    // asynchronously and allows the caller to get the streams connected to its
    // std{in|out|err}
    //
    // on error NULL is returned, in any case the process object will be
    // deleted automatically when the process terminates and should *not* be
    // deleted by the caller
    static wxPyProcess *Open(const wxString& cmd, int flags = wxEXEC_ASYNC);


    %pythonAppend wxPyProcess  "self._setCallbackInfo(self, Process); self.this.own(False)"
    wxPyProcess(wxEvtHandler *parent = NULL, int id = -1);
    ~wxPyProcess();

    void _setCallbackInfo(PyObject* self, PyObject* _class);


    DocDeclStr(
        long , GetPid() const,
        "get the process ID of the process executed by Open()", "");


    void OnTerminate(int pid, int status);
    %MAKE_BASE_FUNC(Process, OnTerminate);

    // call Redirect before passing the object to wxExecute() to redirect the
    // launched process stdin/stdout, then use GetInputStream() and
    // GetOutputStream() to get access to them
    void Redirect();
    bool IsRedirected();


    // detach from the parent - should be called by the parent if it's deleted
    // before the process it started terminates
    void Detach();

    wxInputStream *GetInputStream();
    wxInputStream *GetErrorStream();
    wxOutputStream *GetOutputStream();

    void CloseOutput();

    // return True if the child process stdout is not closed
    bool IsInputOpened() const;

    // return True if any input is available on the child process stdout/err
    bool IsInputAvailable() const;
    bool IsErrorAvailable() const;

    %property(ErrorStream, GetErrorStream, doc="See `GetErrorStream`");
    %property(InputStream, GetInputStream, doc="See `GetInputStream`");
    %property(OutputStream, GetOutputStream, doc="See `GetOutputStream`");

    %property(InputOpened, IsInputOpened);
    %property(InputAvailable, IsInputAvailable);
    %property(ErrorAvailable, IsErrorAvailable);
};

//---------------------------------------------------------------------------


class wxProcessEvent : public wxEvent {
public:
    wxProcessEvent(int id = 0, int pid = 0, int exitcode = 0);
    int GetPid();
    int GetExitCode();
    int m_pid, m_exitcode;

    %property(ExitCode, GetExitCode, doc="See `GetExitCode`");
    %property(Pid, GetPid, doc="See `GetPid`");
};


%constant wxEventType wxEVT_END_PROCESS;

%pythoncode {
EVT_END_PROCESS = wx.PyEventBinder( wxEVT_END_PROCESS, 1 )
}

//---------------------------------------------------------------------------

enum
{
    // execute the process asynchronously
    wxEXEC_ASYNC    = 0,

    // execute it synchronously, i.e. wait until it finishes
    wxEXEC_SYNC     = 1,

    // under Windows, don't hide the child even if it's IO is redirected (this
    // is done by default)
    wxEXEC_NOHIDE   = 2,

    // under Unix, if the process is the group leader then killing -pid kills
    // all children as well as pid
    wxEXEC_MAKE_GROUP_LEADER = 4,

    // by default synchronous execution disables all program windows to avoid
    // that the user interacts with the program while the child process is
    // running, you can use this flag to prevent this from happening
    wxEXEC_NODISABLE = 8
};


MustHaveApp(wxExecute);

long wxExecute(const wxString& command,
               int flags = wxEXEC_ASYNC,
               wxPyProcess *process = NULL);



%typemap(in,numinputs=0) wxKillError* rc ( wxKillError temp ) { $1 = &temp; }
%typemap(argout) wxKillError* rc
{
    PyObject* o;
    o = PyInt_FromLong((long) (*$1));
#if SWIG_VERSION < 0x010328
    $result = t_output_helper($result, o);
#else
    $result = SWIG_Python_AppendOutput($result, o);
#endif
}

int wxKill(long pid, wxSignal sig = wxSIGTERM, wxKillError* rc, int flags = wxKILL_NOCHILDREN);


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxProcess", "wxPyProcess");
%}
//---------------------------------------------------------------------------
