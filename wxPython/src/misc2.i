/////////////////////////////////////////////////////////////////////////////
// Name:        misc2.i
// Purpose:     Definitions of miscelaneous functions and classes that need
//              to know about wxWindow.  (So they can't be in misc.i or an
//              import loop will happen.)
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module misc2

%{
#include "helpers.h"
#include "pyistream.h"
#include <wx/resource.h>
#include <wx/tooltip.h>
#include <wx/caret.h>
#include <wx/tipdlg.h>
#include <wx/process.h>

#if wxUSE_JOYSTICK || defined(__WXMSW__)
#include <wx/joystick.h>
#endif

#if wxUSE_WAVE || defined(__WXMSW__)
#include <wx/wave.h>
#endif

#include <wx/mimetype.h>
#include <wx/snglinst.h>
#include <wx/effects.h>
//#include <wx/spawnbrowser.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import windows.i
%import misc.i
%import gdi.i
%import events.i
%import streams.i
%import utils.i


//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(FileSelectorPromptStr);
    DECLARE_DEF_STRING(FileSelectorDefaultWildcardStr);
    DECLARE_DEF_STRING(DirSelectorPromptStr);
    static const wxString wxPyEmptyString(wxT(""));
%}

//---------------------------------------------------------------------------
// Dialog Functions

wxString wxFileSelector(const wxString& message = wxPyFileSelectorPromptStr,
                        const wxString& default_path = wxPyEmptyString,
                        const wxString& default_filename = wxPyEmptyString,
                        const wxString& default_extension = wxPyEmptyString,
                        const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                        int flags = 0,
                        wxWindow *parent = NULL,
                        int x = -1, int y = -1);

// Ask for filename to load
wxString wxLoadFileSelector(const wxString& what,
                            const wxString& extension,
                            const wxString& default_name = wxPyEmptyString,
                            wxWindow *parent = NULL);

// Ask for filename to save
wxString wxSaveFileSelector(const wxString& what,
                            const wxString& extension,
                            const wxString& default_name = wxPyEmptyString,
                            wxWindow *parent = NULL);


wxString wxDirSelector(const wxString& message = wxPyDirSelectorPromptStr,
                       const wxString& defaultPath = wxPyEmptyString,
                       long style = wxDD_DEFAULT_STYLE,
                       const wxPoint& pos = wxDefaultPosition,
                       wxWindow *parent = NULL);

wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxPyEmptyString,
                           const wxString& default_value = wxPyEmptyString,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE);

wxString wxGetPasswordFromUser(const wxString& message,
                               const wxString& caption = wxPyEmptyString,
                               const wxString& default_value = wxPyEmptyString,
                               wxWindow *parent = NULL);


// TODO: Need to custom wrap this one...
// int wxGetMultipleChoice(char* message, char* caption,
//                         int LCOUNT, char** choices,
//                         int nsel, int *selection,
//                         wxWindow *parent = NULL, int x = -1, int y = -1,
//                         bool centre = TRUE, int width=150, int height=200);


wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* choices,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);

int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* choices,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);


int wxMessageBox(const wxString& message,
                 const wxString& caption = wxPyEmptyString,
                 int style = wxOK | wxCENTRE,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

long wxGetNumberFromUser(const wxString& message,
                         const wxString& prompt,
                         const wxString& caption,
                         long value,
                         long min = 0, long max = 100,
                         wxWindow *parent = NULL,
                         const wxPoint& pos = wxDefaultPosition);

//---------------------------------------------------------------------------
// GDI Functions

bool wxColourDisplay();

int wxDisplayDepth();
int wxGetDisplayDepth();

void   wxDisplaySize(int* OUTPUT, int* OUTPUT);
wxSize wxGetDisplaySize();

void   wxDisplaySizeMM(int* OUTPUT, int* OUTPUT);
wxSize wxGetDisplaySizeMM();

void   wxClientDisplayRect(int *OUTPUT, int *OUTPUT, int *OUTPUT, int *OUTPUT);
wxRect wxGetClientDisplayRect();

void wxSetCursor(wxCursor& cursor);

//----------------------------------------------------------------------
// Miscellaneous functions

void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
wxWindow * wxGetActiveWindow();

wxWindow* wxGenericFindWindowAtPoint(const wxPoint& pt);
wxWindow* wxFindWindowAtPoint(const wxPoint& pt);

#ifdef __WXMSW__
bool wxCheckForInterrupt(wxWindow *wnd);
void wxFlushEvents();
#endif

wxWindow* wxGetTopLevelParent(wxWindow *win);

//bool wxSpawnBrowser(wxWindow *parent, wxString href);

//---------------------------------------------------------------------------
// Resource System

#ifdef wxUSE_WX_RESOURCES
bool wxResourceAddIdentifier(char* name, int value);
void wxResourceClear(void);
wxBitmap  wxResourceCreateBitmap(char* resource);
wxIcon  wxResourceCreateIcon(char* resource);
wxMenuBar * wxResourceCreateMenuBar(char* resource);
int wxResourceGetIdentifier(char* name);
bool wxResourceParseData(char* resource, wxResourceTable *table = NULL);
bool wxResourceParseFile(char* filename, wxResourceTable *table = NULL);
bool wxResourceParseString(char* resource, wxResourceTable *table = NULL);
#endif

//---------------------------------------------------------------------------
// System Settings

// possible values for wxSystemSettings::GetFont() parameter
//
enum wxSystemFont
{
    wxSYS_OEM_FIXED_FONT = 10,
    wxSYS_ANSI_FIXED_FONT,
    wxSYS_ANSI_VAR_FONT,
    wxSYS_SYSTEM_FONT,
    wxSYS_DEVICE_DEFAULT_FONT,
    wxSYS_DEFAULT_PALETTE,
    wxSYS_SYSTEM_FIXED_FONT,
    wxSYS_DEFAULT_GUI_FONT
};

// possible values for wxSystemSettings::GetColour() parameter
//
enum wxSystemColour
{
    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_DESKTOP = wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_MENUTEXT,
    wxSYS_COLOUR_WINDOWTEXT,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_3DFACE = wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_3DSHADOW = wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_BTNTEXT,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DHIGHLIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_LISTBOX,
    wxSYS_COLOUR_HOTLIGHT,
    wxSYS_COLOUR_GRADIENTACTIVECAPTION,
    wxSYS_COLOUR_GRADIENTINACTIVECAPTION,
    wxSYS_COLOUR_MENUHILIGHT,
    wxSYS_COLOUR_MENUBAR,

    wxSYS_COLOUR_MAX
};

// possible values for wxSystemSettings::GetMetric() parameter
//
enum wxSystemMetric
{
    wxSYS_MOUSE_BUTTONS = 1,
    wxSYS_BORDER_X,
    wxSYS_BORDER_Y,
    wxSYS_CURSOR_X,
    wxSYS_CURSOR_Y,
    wxSYS_DCLICK_X,
    wxSYS_DCLICK_Y,
    wxSYS_DRAG_X,
    wxSYS_DRAG_Y,
    wxSYS_EDGE_X,
    wxSYS_EDGE_Y,
    wxSYS_HSCROLL_ARROW_X,
    wxSYS_HSCROLL_ARROW_Y,
    wxSYS_HTHUMB_X,
    wxSYS_ICON_X,
    wxSYS_ICON_Y,
    wxSYS_ICONSPACING_X,
    wxSYS_ICONSPACING_Y,
    wxSYS_WINDOWMIN_X,
    wxSYS_WINDOWMIN_Y,
    wxSYS_SCREEN_X,
    wxSYS_SCREEN_Y,
    wxSYS_FRAMESIZE_X,
    wxSYS_FRAMESIZE_Y,
    wxSYS_SMALLICON_X,
    wxSYS_SMALLICON_Y,
    wxSYS_HSCROLL_Y,
    wxSYS_VSCROLL_X,
    wxSYS_VSCROLL_ARROW_X,
    wxSYS_VSCROLL_ARROW_Y,
    wxSYS_VTHUMB_Y,
    wxSYS_CAPTION_Y,
    wxSYS_MENU_Y,
    wxSYS_NETWORK_PRESENT,
    wxSYS_PENWINDOWS_PRESENT,
    wxSYS_SHOW_SOUNDS,
    wxSYS_SWAP_BUTTONS
};

// possible values for wxSystemSettings::HasFeature() parameter
enum wxSystemFeature
{
    wxSYS_CAN_DRAW_FRAME_DECORATIONS = 1,
    wxSYS_CAN_ICONIZE_FRAME
};

enum wxSystemScreenType
{
    wxSYS_SCREEN_NONE = 0,  //   not yet defined

    wxSYS_SCREEN_TINY,      //   <
    wxSYS_SCREEN_PDA,       //   >= 320x240
    wxSYS_SCREEN_SMALL,     //   >= 640x480
    wxSYS_SCREEN_DESKTOP    //   >= 800x600
};


class wxSystemSettings {
public:
    // get a standard system colour
    static wxColour GetColour(wxSystemColour index);

    // get a standard system font
    static wxFont GetFont(wxSystemFont index);

    // get a system-dependent metric
    static int GetMetric(wxSystemMetric index);

    // return true if the port has certain feature
    static bool HasFeature(wxSystemFeature index);

   // Get system screen design (desktop, pda, ..) used for
   // laying out various dialogs.
   static wxSystemScreenType GetScreenType();

   // Override default.
   static void SetScreenType( wxSystemScreenType screen );

};



//---------------------------------------------------------------------------
// wxToolTip

class wxToolTip : public wxObject {
public:
    wxToolTip(const wxString &tip);

    void SetTip(const wxString& tip);
    wxString GetTip();
    // *** Not in the "public" interface void SetWindow(wxWindow *win);
    wxWindow *GetWindow();

    static void Enable(bool flag);
    static void SetDelay(long milliseconds);
};

//----------------------------------------------------------------------

class wxCaret {
public:
    wxCaret(wxWindow* window, const wxSize& size);
    ~wxCaret();

    bool IsOk();
    bool IsVisible();
    %name(GetPositionTuple)void GetPosition(int *OUTPUT, int *OUTPUT);
    wxPoint GetPosition();
    %name(GetSizeTuple)void GetSize(int *OUTPUT, int *OUTPUT);
    wxSize GetSize();
    wxWindow *GetWindow();
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& pt);
    %name(SetSizeWH) void SetSize(int width, int height);
    void SetSize(const wxSize& size);
    void Show(int show = TRUE);
    void Hide();

    %pragma(python) addtoclass = "def __nonzero__(self): return self.IsOk()"
};

%inline %{
    int wxCaret_GetBlinkTime() {
        return wxCaret::GetBlinkTime();
    }

    void wxCaret_SetBlinkTime(int milliseconds) {
        wxCaret::SetBlinkTime(milliseconds);
    }
%}

//----------------------------------------------------------------------

class  wxBusyCursor {
public:
    wxBusyCursor(wxCursor* cursor = wxHOURGLASS_CURSOR);
    ~wxBusyCursor();
};

//----------------------------------------------------------------------

class wxWindowDisabler {
public:
    wxWindowDisabler(wxWindow *winToSkip = NULL);
    ~wxWindowDisabler();
};

//----------------------------------------------------------------------

bool wxSafeYield(wxWindow* win=NULL, bool onlyIfNeeded=FALSE);
void wxPostEvent(wxEvtHandler *dest, wxEvent& event);
void wxWakeUpIdle();


#ifdef __WXMSW__
void wxWakeUpMainThread();
#endif

void wxMutexGuiEnter();
void wxMutexGuiLeave();


class wxMutexGuiLocker  {
public:
    wxMutexGuiLocker();
    ~wxMutexGuiLocker();
};


%inline %{
    bool wxThread_IsMain() {
#ifdef WXP_WITH_THREAD
        return wxThread::IsMain();
#else
        return TRUE;
#endif
    }
%}

//----------------------------------------------------------------------


class wxTipProvider
{
public:
    // wxTipProvider(size_t currentTip);  **** Abstract base class
    ~wxTipProvider();

    virtual wxString GetTip() = 0;
    size_t GetCurrentTip();
    virtual wxString PreprocessTip(const wxString& tip);
};


// The C++ version of wxPyTipProvider
%{
class wxPyTipProvider : public wxTipProvider {
public:
    wxPyTipProvider(size_t currentTip)
        : wxTipProvider(currentTip) {}

    DEC_PYCALLBACK_STRING__pure(GetTip);
    DEC_PYCALLBACK_STRING_STRING(PreprocessTip);
    PYPRIVATE;
};

IMP_PYCALLBACK_STRING__pure( wxPyTipProvider, wxTipProvider, GetTip);
IMP_PYCALLBACK_STRING_STRING(wxPyTipProvider, wxTipProvider, PreprocessTip);
%}


// Now let SWIG know about it
class wxPyTipProvider : public wxTipProvider {
public:
    wxPyTipProvider(size_t currentTip);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyTipProvider)"
};



bool wxShowTip(wxWindow *parent, wxTipProvider *tipProvider, bool showAtStartup = TRUE);
%new wxTipProvider * wxCreateFileTipProvider(const wxString& filename, size_t currentTip);


//----------------------------------------------------------------------

%{
#include <wx/generic/dragimgg.h>
%}

%name (wxDragImage) class wxGenericDragImage : public wxObject
{
public:

    wxGenericDragImage(const wxBitmap& image,
                       const wxCursor& cursor = wxNullCursor);
    %name(wxDragIcon)wxGenericDragImage(const wxIcon& image,
                                        const wxCursor& cursor = wxNullCursor);
    %name(wxDragString)wxGenericDragImage(const wxString& str,
                                          const wxCursor& cursor = wxNullCursor);
    %name(wxDragTreeItem)wxGenericDragImage(const wxTreeCtrl& treeCtrl, wxTreeItemId& id);
    %name(wxDragListItem)wxGenericDragImage(const wxListCtrl& listCtrl, long id);

    ~wxGenericDragImage();

    void SetBackingBitmap(wxBitmap* bitmap);
    bool BeginDrag(const wxPoint& hotspot, wxWindow* window,
                   bool fullScreen = FALSE, wxRect* rect = NULL);

    %name(BeginDragBounded) bool BeginDrag(const wxPoint& hotspot, wxWindow* window,
                                           wxWindow* boundingWindow);

    %pragma(python) addtoclass = "BeginDrag2 = BeginDragBounded"

    bool EndDrag();
    bool Move(const wxPoint& pt);
    bool Show();
    bool Hide();

    wxRect GetImageRect(const wxPoint& pos) const;
    bool RedrawImage(const wxPoint& oldPos, const wxPoint& newPos,
                     bool eraseOld, bool drawNew);
};


//----------------------------------------------------------------------

class wxPyTimer : public wxObject {
public:
    wxPyTimer(PyObject* notify);
    ~wxPyTimer();
    int GetInterval();
    bool IsOneShot();
    bool IsRunning();
    void SetOwner(wxEvtHandler *owner, int id = -1);
    bool Start(int milliseconds=-1, int oneShot=FALSE);
    void Stop();
};


class wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch();
    ~wxStopWatch();
    void Start(long t = 0);
    void Pause();
    void Resume();

    // get elapsed time since the last Start() or Pause() in milliseconds
    long Time() const;
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------


enum
{
    wxLOG_FatalError, // program can't continue, abort immediately
    wxLOG_Error,      // a serious error, user must be informed about it
    wxLOG_Warning,    // user is normally informed about it but may be ignored
    wxLOG_Message,    // normal message (i.e. normal output of a non GUI app)
    wxLOG_Info,       // informational message (a.k.a. 'Verbose')
    wxLOG_Status,     // informational: might go to the status line of GUI app
    wxLOG_Debug,      // never shown to the user, disabled in release mode
    wxLOG_Trace,      // trace messages are also only enabled in debug mode
    wxLOG_Progress,   // used for progress indicator (not yet)
    wxLOG_User = 100  // user defined levels start here
};

#define wxTRACE_MemAlloc "memalloc" // trace memory allocation (new/delete)
#define wxTRACE_Messages "messages" // trace window messages/X callbacks
#define wxTRACE_ResAlloc "resalloc" // trace GDI resource allocation
#define wxTRACE_RefCount "refcount" // trace various ref counting operations

#ifdef  __WXMSW__
#define wxTRACE_OleCalls "ole"      // OLE interface calls
#endif

enum {
    wxTraceMemAlloc,
    wxTraceMessages,
    wxTraceResAlloc,
    wxTraceRefCount,

#ifdef  __WXMSW__
    wxTraceOleCalls,
#endif
};



class wxLog
{
public:
    wxLog();

    static bool IsEnabled();
    static bool EnableLogging(bool doIt = TRUE);
    static void OnLog(unsigned long level, const wxString& szString, int t=0);

    virtual void Flush();
    static void FlushActive();
    static wxLog *GetActiveTarget();
    static wxLog *SetActiveTarget(wxLog *pLogger);

    static void Suspend();
    static void Resume();

    static void SetVerbose(bool bVerbose = TRUE);

    static void DontCreateOnDemand();
    static void SetTraceMask(long ulMask);
    static void AddTraceMask(const wxString& str);
    static void RemoveTraceMask(const wxString& str);
    static void ClearTraceMasks();
    static const wxArrayString &GetTraceMasks();

    static void SetTimestamp(const wxString& ts);
    static const wxString& GetTimestamp();

    bool GetVerbose() const;

    static unsigned long GetTraceMask();
    static bool IsAllowedTraceMask(const wxString& mask);

    static void SetLogLevel(unsigned long logLevel);
    static unsigned long GetLogLevel();


    // static void TimeStamp(wxString *str);
    %addmethods {
        wxString TimeStamp() {
            wxString msg;
            wxLog::TimeStamp(&msg);
            return msg;
        }
    }

};


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
            bool bShow = TRUE,            // show window immediately?
            bool bPassToOld = TRUE);      // pass log messages to the old target?

    void Show(bool bShow = TRUE);
    wxFrame *GetFrame() const;
    wxLog *GetOldLog() const;
    bool IsPassingMessages() const;
    void PassMessages(bool bDoPass);
};


class wxLogChain : public wxLog
{
public:
    wxLogChain(wxLog *logger);
    void SetLog(wxLog *logger);
    void PassMessages(bool bDoPass);
    bool IsPassingMessages();
    wxLog *GetOldLog();
};


unsigned long wxSysErrorCode();
const wxString wxSysErrorMsg(unsigned long nErrCode = 0);
void wxLogFatalError(const wxString& msg);
void wxLogError(const wxString& msg);
void wxLogWarning(const wxString& msg);
void wxLogMessage(const wxString& msg);
void wxLogInfo(const wxString& msg);
void wxLogDebug(const wxString& msg);
void wxLogVerbose(const wxString& msg);
void wxLogStatus(const wxString& msg);
%name(wxLogStatusFrame)void wxLogStatus(wxFrame *pFrame, const wxString& msg);
void wxLogSysError(const wxString& msg);

void wxLogTrace(const wxString& msg);
%name(wxLogTraceMask)void wxLogTrace(const wxString& mask, const wxString& msg);

void wxLogGeneric(unsigned long level, const wxString& msg);

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




%{
// A wxLog class that can be derived from in wxPython
class wxPyLog : public wxLog {
public:
    wxPyLog() : wxLog() {}

    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t) {
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLog"))) {
            PyObject* s = wx2PyString(szString);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iOi)", level, s, t));
            Py_DECREF(s);
        }
        wxPyEndBlockThreads();
        if (! found)
            wxLog::DoLog(level, szString, t);
    }

    virtual void DoLogString(const wxChar *szString, time_t t) {
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLogString"))) {
            PyObject* s = wx2PyString(szString);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", s, t));
            Py_DECREF(s);
        }
        wxPyEndBlockThreads();
        if (! found)
            wxLog::DoLogString(szString, t);
    }

    PYPRIVATE;
};
%}

// Now tell SWIG about it
class wxPyLog : public wxLog {
public:
    wxPyLog();
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyLog)"
    %addmethods { void Destroy() { delete self; } }

};


//----------------------------------------------------------------------


enum wxKillError
{
    wxKILL_OK,              // no error
    wxKILL_BAD_SIGNAL,      // no such signal
    wxKILL_ACCESS_DENIED,   // permission denied
    wxKILL_NO_PROCESS,      // no such process
    wxKILL_ERROR            // another, unspecified error
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



enum {
    /* event type */
    wxEVT_END_PROCESS
};



class wxProcessEvent : public wxEvent {
public:
    wxProcessEvent(int id = 0, int pid = 0, int exitcode = 0);
    int GetPid();
    int GetExitCode();
    int m_pid, m_exitcode;
};




%{ // C++ version of wxProcess derived class

class wxPyProcess : public wxProcess {
public:
    wxPyProcess(wxEvtHandler *parent = NULL, int id = -1)
        : wxProcess(parent, id)
        {}

    DEC_PYCALLBACK_VOID_INTINT(OnTerminate);

    PYPRIVATE;
};

IMP_PYCALLBACK_VOID_INTINT( wxPyProcess, wxProcess, OnTerminate);

%}


%name(wxProcess)class wxPyProcess : public wxEvtHandler {
public:
    // kill the process with the given PID
    static wxKillError Kill(int pid, wxSignal sig = wxSIGTERM);

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



    wxPyProcess(wxEvtHandler *parent = NULL, int id = -1);
    %addmethods { void Destroy() { delete self; } }

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxProcess)"

    void base_OnTerminate(int pid, int status);

    void Redirect();
    bool IsRedirected();
    void Detach();

    wxInputStream *GetInputStream();
    wxInputStream *GetErrorStream();
    wxOutputStream *GetOutputStream();

    void CloseOutput();

    // return TRUE if the child process stdout is not closed
    bool IsInputOpened() const;

    // return TRUE if any input is available on the child process stdout/err
    bool IsInputAvailable() const;
    bool IsErrorAvailable() const;
};


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
    wxEXEC_MAKE_GROUP_LEADER = 4
};


long wxExecute(const wxString& command,
               int flags = wxEXEC_ASYNC,
               wxPyProcess *process = NULL);

//----------------------------------------------------------------------


// Which joystick? Same as Windows ids so no conversion necessary.
enum
{
    wxJOYSTICK1,
    wxJOYSTICK2
};

// Which button is down?
enum
{
    wxJOY_BUTTON_ANY,
    wxJOY_BUTTON1,
    wxJOY_BUTTON2,
    wxJOY_BUTTON3,
    wxJOY_BUTTON4,
};


%{
#if !wxUSE_JOYSTICK && !defined(__WXMSW__)
// A C++ stub class for wxJoystick for platforms that don't have it.
class wxJoystick : public wxObject {
public:
    wxJoystick(int joystick = wxJOYSTICK1) {
        wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError, "wxJoystick is not available on this platform.");
        wxPyEndBlockThreads();
    }
    wxPoint GetPosition() { return wxPoint(-1,-1); }
    int GetZPosition() { return -1; }
    int GetButtonState() { return -1; }
    int GetPOVPosition() { return -1; }
    int GetPOVCTSPosition() { return -1; }
    int GetRudderPosition() { return -1; }
    int GetUPosition() { return -1; }
    int GetVPosition() { return -1; }
    int GetMovementThreshold() { return -1; }
    void SetMovementThreshold(int threshold) {}

    bool IsOk(void) { return FALSE; }
    int GetNumberJoysticks() { return -1; }
    int GetManufacturerId() { return -1; }
    int GetProductId() { return -1; }
    wxString GetProductName() { return ""; }
    int GetXMin() { return -1; }
    int GetYMin() { return -1; }
    int GetZMin() { return -1; }
    int GetXMax() { return -1; }
    int GetYMax() { return -1; }
    int GetZMax() { return -1; }
    int GetNumberButtons() { return -1; }
    int GetNumberAxes() { return -1; }
    int GetMaxButtons() { return -1; }
    int GetMaxAxes() { return -1; }
    int GetPollingMin() { return -1; }
    int GetPollingMax() { return -1; }
    int GetRudderMin() { return -1; }
    int GetRudderMax() { return -1; }
    int GetUMin() { return -1; }
    int GetUMax() { return -1; }
    int GetVMin() { return -1; }
    int GetVMax() { return -1; }

    bool HasRudder() { return FALSE; }
    bool HasZ() { return FALSE; }
    bool HasU() { return FALSE; }
    bool HasV() { return FALSE; }
    bool HasPOV() { return FALSE; }
    bool HasPOV4Dir() { return FALSE; }
    bool HasPOVCTS() { return FALSE; }

    bool SetCapture(wxWindow* win, int pollingFreq = 0) { return FALSE; }
    bool ReleaseCapture() { return FALSE; }
};
#endif
%}


class wxJoystick : public wxObject {
public:
    wxJoystick(int joystick = wxJOYSTICK1);
    ~wxJoystick();

    wxPoint GetPosition();
    int GetZPosition();
    int GetButtonState();
    int GetPOVPosition();
    int GetPOVCTSPosition();
    int GetRudderPosition();
    int GetUPosition();
    int GetVPosition();
    int GetMovementThreshold();
    void SetMovementThreshold(int threshold) ;

    bool IsOk(void);
    int GetNumberJoysticks();
    int GetManufacturerId();
    int GetProductId();
    wxString GetProductName();
    int GetXMin();
    int GetYMin();
    int GetZMin();
    int GetXMax();
    int GetYMax();
    int GetZMax();
    int GetNumberButtons();
    int GetNumberAxes();
    int GetMaxButtons();
    int GetMaxAxes();
    int GetPollingMin();
    int GetPollingMax();
    int GetRudderMin();
    int GetRudderMax();
    int GetUMin();
    int GetUMax();
    int GetVMin();
    int GetVMax();

    bool HasRudder();
    bool HasZ();
    bool HasU();
    bool HasV();
    bool HasPOV();
    bool HasPOV4Dir();
    bool HasPOVCTS();

    bool SetCapture(wxWindow* win, int pollingFreq = 0);
    bool ReleaseCapture();

    %pragma(python) addtoclass = "def __nonzero__(self): return self.IsOk()"
};

//----------------------------------------------------------------------

%{
#if !wxUSE_WAVE
// A C++ stub class for wxWave for platforms that don't have it.
class wxWave : public wxObject
{
public:
    wxWave(const wxString& fileName, bool isResource = FALSE) {
        wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError, "wxWave is not available on this platform.");
        wxPyEndBlockThreads();
    }
    wxWave(int size, const wxByte* data) {
        wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError, "wxWave is not available on this platform.");
        wxPyEndBlockThreads();
    }

    ~wxWave() {}

    bool  IsOk() const { return FALSE; }
    bool  Play(bool async = TRUE, bool looped = FALSE) const { return FALSE; }
};

#endif
%}

class wxWave : public wxObject
{
public:
    wxWave(const wxString& fileName, bool isResource = FALSE);
    ~wxWave();

    bool  IsOk() const;
    bool  Play(bool async = TRUE, bool looped = FALSE) const;

    %pragma(python) addtoclass = "def __nonzero__(self): return self.IsOk()"
};

%new wxWave* wxWaveData(const wxString& data);
%{ // Implementations of some alternate "constructors"
    wxWave* wxWaveData(const wxString& data) {
        return new wxWave(data.Len(), (wxByte*)data.c_str());
    }
%}


//----------------------------------------------------------------------

enum wxMailcapStyle
{
    wxMAILCAP_STANDARD = 1,
    wxMAILCAP_NETSCAPE = 2,
    wxMAILCAP_KDE = 4,
    wxMAILCAP_GNOME = 8,

    wxMAILCAP_ALL = 15
};



class wxFileTypeInfo
{
public:
    // ctors
        // a normal item
    wxFileTypeInfo(const wxString& mimeType,
                   const wxString& openCmd,
                   const wxString& printCmd,
                   const wxString& desc);


        // the array elements correspond to the parameters of the ctor above in
        // the same order
    %name(wxFileTypeInfoSequence)wxFileTypeInfo(const wxArrayString& sArray);

        // invalid item - use this to terminate the array passed to
        // wxMimeTypesManager::AddFallbacks
    %name(wxNullFileTypeInfo)wxFileTypeInfo();


    // test if this object can be used
    bool IsValid() const;

    // setters
        // set the icon info
    void SetIcon(const wxString& iconFile, int iconIndex = 0);

        // set the short desc
    void SetShortDesc(const wxString& shortDesc);

    // accessors
        // get the MIME type
    const wxString& GetMimeType() const;
        // get the open command
    const wxString& GetOpenCommand() const;
        // get the print command
    const wxString& GetPrintCommand() const;
        // get the short description (only used under Win32 so far)
    const wxString& GetShortDesc() const;
        // get the long, user visible description
    const wxString& GetDescription() const;


        // get the array of all extensions
    //const wxArrayString& GetExtensions() const;
    %addmethods {
        PyObject* GetExtensions() {
            wxArrayString& arr = (wxArrayString&)self->GetExtensions();
            return wxArrayString2PyList_helper(arr);
        }
    }

    int GetExtensionsCount() const;

    // get the icon info
    const wxString& GetIconFile() const;
    int GetIconIndex() const;
};




class wxFileType
{
public:

    // TODO: Make a wxPyMessageParameters with virtual GetParamValue...

    // An object of this class must be passed to Get{Open|Print}Command. The
    // default implementation is trivial and doesn't know anything at all about
    // parameters, only filename and MIME type are used (so it's probably ok for
    // Windows where %{param} is not used anyhow)
    class MessageParameters
    {
    public:
        // ctors
        MessageParameters(const wxString& filename=wxPyEmptyString,
                          const wxString& mimetype=wxPyEmptyString);

        // accessors (called by GetOpenCommand)
            // filename
        const wxString& GetFileName() const;
            // mime type
        const wxString& GetMimeType() const;;

        // override this function in derived class
        virtual wxString GetParamValue(const wxString& name) const;

        // virtual dtor as in any base class
        virtual ~MessageParameters();
    };


    // ctor from static data
    wxFileType(const wxFileTypeInfo& ftInfo);

    // return the MIME type for this file type
    %addmethods {
        PyObject* GetMimeType() {
            wxString str;
            if (self->GetMimeType(&str)) {
#if wxUSE_UNICODE
	      return PyUnicode_FromWideChar(str.c_str(), str.Len());
#else
	      return PyString_FromStringAndSize(str.c_str(), str.Len());
#endif
	    }
            else
                RETURN_NONE();
        }
        PyObject* GetMimeTypes() {
            wxArrayString arr;
            if (self->GetMimeTypes(arr))
                return wxArrayString2PyList_helper(arr);
            else
                RETURN_NONE();
        }
    }


    // Get all extensions associated with this file type
    %addmethods {
        PyObject* GetExtensions() {
            wxArrayString arr;
            if (self->GetExtensions(arr))
                return wxArrayString2PyList_helper(arr);
            else
                RETURN_NONE();
        }
    }


    %addmethods {
        // Get the icon corresponding to this file type
        %new wxIcon* GetIcon() {
            wxIcon icon;
            if (self->GetIcon(&icon))
                return new wxIcon(icon);
            else
                return NULL;
        }

        // Get the icon corresponding to this file type, the name of the file
        // where this icon resides, and its index in this file if applicable.
        PyObject* GetIconInfo() {
            wxIcon icon;
            wxString iconFile;
            int iconIndex;
            if (self->GetIcon(&icon, &iconFile, &iconIndex)) {
                wxPyBeginBlockThreads();
                PyObject* tuple = PyTuple_New(3);
                PyTuple_SetItem(tuple, 0, wxPyConstructObject(new wxIcon(icon),
                                                              wxT("wxIcon"), TRUE));
#if wxUSE_UNICODE
                PyTuple_SetItem(tuple, 1, PyUnicode_FromWideChar(iconFile.c_str(), iconFile.Len()));
#else
                PyTuple_SetItem(tuple, 1, PyString_FromStringAndSize(iconFile.c_str(), iconFile.Len()));
#endif
                PyTuple_SetItem(tuple, 2, PyInt_FromLong(iconIndex));
                wxPyEndBlockThreads();
                return tuple;
            }
            else
                RETURN_NONE();
        }
    }

    %addmethods {
        // get a brief file type description ("*.txt" => "text document")
        PyObject* GetDescription() {
            wxString str;
            if (self->GetDescription(&str)) {
#if  wxUSE_UNICODE
	      return PyUnicode_FromWideChar(str.c_str(), str.Len());
#else
	      return PyString_FromStringAndSize(str.c_str(), str.Len());
#endif
            } else
                RETURN_NONE();
        }
    }


    // get the command to open/execute the file of given type
    %addmethods {
        PyObject* GetOpenCommand(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyString) {
            wxString str;
            if (self->GetOpenCommand(&str, wxFileType::MessageParameters(filename, mimetype))) {
#if  wxUSE_UNICODE
                return PyUnicode_FromWideChar(str.c_str(), str.Len());
#else
                return PyString_FromStringAndSize(str.c_str(), str.Len());
#endif
            } else
                RETURN_NONE();
        }
    }


    // get the command to print the file of given type
    %addmethods {
        PyObject* GetPrintCommand(const wxString& filename,
                                  const wxString& mimetype=wxPyEmptyString) {
            wxString str;
            if (self->GetPrintCommand(&str, wxFileType::MessageParameters(filename, mimetype))) {
#if wxUSE_UNICODE
                return PyUnicode_FromWideChar(str.c_str(), str.Len());
#else
                return PyString_FromStringAndSize(str.c_str(), str.Len());
#endif
            } else
                RETURN_NONE();
        }
    }


    // Get all commands defined for this file type
    %addmethods {
        PyObject* GetAllCommands(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyString) {
            wxArrayString verbs;
            wxArrayString commands;
            if (self->GetAllCommands(&verbs, &commands,
                                     wxFileType::MessageParameters(filename, mimetype))) {
                wxPyBeginBlockThreads();
                PyObject* tuple = PyTuple_New(2);
                PyTuple_SetItem(tuple, 0, wxArrayString2PyList_helper(verbs));
                PyTuple_SetItem(tuple, 1, wxArrayString2PyList_helper(commands));
                wxPyEndBlockThreads();
                return tuple;
            }
            else
                RETURN_NONE();
        }
    }


    // set an arbitrary command, ask confirmation if it already exists and
    // overwriteprompt is TRUE
    bool SetCommand(const wxString& cmd, const wxString& verb,
                    bool overwriteprompt = TRUE);

    bool SetDefaultIcon(const wxString& cmd = wxPyEmptyString, int index = 0);


    // remove the association for this filetype from the system MIME database:
    // notice that it will only work if the association is defined in the user
    // file/registry part, we will never modify the system-wide settings
    bool Unassociate();

    // operations
        // expand a string in the format of GetOpenCommand (which may contain
        // '%s' and '%t' format specificators for the file name and mime type
        // and %{param} constructions).
    static wxString ExpandCommand(const wxString& command,
                                  const MessageParameters& params);

    // dtor (not virtual, shouldn't be derived from)
    ~wxFileType();

};




class wxMimeTypesManager
{
public:
    // static helper functions
    // -----------------------

        // check if the given MIME type is the same as the other one: the
        // second argument may contain wildcards ('*'), but not the first. If
        // the types are equal or if the mimeType matches wildcard the function
        // returns TRUE, otherwise it returns FALSE
    static bool IsOfType(const wxString& mimeType, const wxString& wildcard);

    // ctor
    wxMimeTypesManager();

    // loads data from standard files according to the mailcap styles
    // specified: this is a bitwise OR of wxMailcapStyle values
    //
    // use the extraDir parameter if you want to look for files in another
    // directory
    void Initialize(int mailcapStyle = wxMAILCAP_ALL,
                    const wxString& extraDir = wxPyEmptyString);

    // and this function clears all the data from the manager
    void ClearData();

    // Database lookup: all functions return a pointer to wxFileType object
    // whose methods may be used to query it for the information you're
    // interested in. If the return value is !NULL, caller is responsible for
    // deleting it.
    // get file type from file extension
    %new wxFileType *GetFileTypeFromExtension(const wxString& ext);

    // get file type from MIME type (in format <category>/<format>)
    %new wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    // other operations: return TRUE if there were no errors or FALSE if there
    // were some unreckognized entries (the good entries are always read anyhow)
    //

        // read in additional file (the standard ones are read automatically)
        // in mailcap format (see mimetype.cpp for description)
        //
        // 'fallback' parameter may be set to TRUE to avoid overriding the
        // settings from other, previously parsed, files by this one: normally,
        // the files read most recently would override the older files, but with
        // fallback == TRUE this won't happen
    bool ReadMailcap(const wxString& filename, bool fallback = FALSE);

    // read in additional file in mime.types format
    bool ReadMimeTypes(const wxString& filename);

    // enumerate all known MIME types
    %addmethods {
        PyObject* EnumAllFileTypes() {
            wxArrayString arr;
            self->EnumAllFileTypes(arr);
            return wxArrayString2PyList_helper(arr);
        }
    }

    // these functions can be used to provide default values for some of the
    // MIME types inside the program itself (you may also use
    // ReadMailcap(filenameWithDefaultTypes, TRUE /* use as fallback */) to
    // achieve the same goal, but this requires having this info in a file).
    //
    void AddFallback(const wxFileTypeInfo& ft);


    // create or remove associations

        // create a new association using the fields of wxFileTypeInfo (at least
        // the MIME type and the extension should be set)
        // if the other fields are empty, the existing values should be left alone
    %new wxFileType *Associate(const wxFileTypeInfo& ftInfo);

        // undo Associate()
    bool Unassociate(wxFileType *ft) ;

    // dtor (not virtual, shouldn't be derived from)
    ~wxMimeTypesManager();
};


%readonly
%{
#if 0
%}
extern wxMimeTypesManager* wxTheMimeTypesManager;
%{
#endif
%}
%readwrite

//----------------------------------------------------------------------

%{
#include <wx/artprov.h>

    DECLARE_DEF_STRING(ART_OTHER);

%}

%pragma(python) code = "
# Art clients
wxART_TOOLBAR              = 'wxART_TOOLBAR_C'
wxART_MENU                 = 'wxART_MENU_C'
wxART_FRAME_ICON           = 'wxART_FRAME_ICON_C'
wxART_CMN_DIALOG           = 'wxART_CMN_DIALOG_C'
wxART_HELP_BROWSER         = 'wxART_HELP_BROWSER_C'
wxART_MESSAGE_BOX          = 'wxART_MESSAGE_BOX_C'
wxART_OTHER                = 'wxART_OTHER_C'

# Art IDs
wxART_ADD_BOOKMARK         = 'wxART_ADD_BOOKMARK'
wxART_DEL_BOOKMARK         = 'wxART_DEL_BOOKMARK'
wxART_HELP_SIDE_PANEL      = 'wxART_HELP_SIDE_PANEL'
wxART_HELP_SETTINGS        = 'wxART_HELP_SETTINGS'
wxART_HELP_BOOK            = 'wxART_HELP_BOOK'
wxART_HELP_FOLDER          = 'wxART_HELP_FOLDER'
wxART_HELP_PAGE            = 'wxART_HELP_PAGE'
wxART_GO_BACK              = 'wxART_GO_BACK'
wxART_GO_FORWARD           = 'wxART_GO_FORWARD'
wxART_GO_UP                = 'wxART_GO_UP'
wxART_GO_DOWN              = 'wxART_GO_DOWN'
wxART_GO_TO_PARENT         = 'wxART_GO_TO_PARENT'
wxART_GO_HOME              = 'wxART_GO_HOME'
wxART_FILE_OPEN            = 'wxART_FILE_OPEN'
wxART_PRINT                = 'wxART_PRINT'
wxART_HELP                 = 'wxART_HELP'
wxART_TIP                  = 'wxART_TIP'
wxART_REPORT_VIEW          = 'wxART_REPORT_VIEW'
wxART_LIST_VIEW            = 'wxART_LIST_VIEW'
wxART_NEW_DIR              = 'wxART_NEW_DIR'
wxART_FOLDER               = 'wxART_FOLDER'
wxART_GO_DIR_UP            = 'wxART_GO_DIR_UP'
wxART_EXECUTABLE_FILE      = 'wxART_EXECUTABLE_FILE'
wxART_NORMAL_FILE          = 'wxART_NORMAL_FILE'
wxART_TICK_MARK            = 'wxART_TICK_MARK'
wxART_CROSS_MARK           = 'wxART_CROSS_MARK'
wxART_ERROR                = 'wxART_ERROR'
wxART_QUESTION             = 'wxART_QUESTION'
wxART_WARNING              = 'wxART_WARNING'
wxART_INFORMATION          = 'wxART_INFORMATION'
wxART_MISSING_IMAGE        = 'wxART_MISSING_IMAGE'
"

%{  // Python aware wxArtProvider
class wxPyArtProvider : public wxArtProvider  {
public:

    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size) {
        wxBitmap rval = wxNullBitmap;
        wxPyBeginBlockThreads();
        if ((wxPyCBH_findCallback(m_myInst, "CreateBitmap"))) {
            PyObject* so = wxPyConstructObject((void*)&size, wxT("wxSize"), 0);
            PyObject* ro;
            wxBitmap* ptr;
            PyObject* s1, *s2;
            s1 = wx2PyString(id);
            s2 = wx2PyString(client);
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OOO)", s1, s2, so));
            Py_DECREF(so);
            Py_DECREF(s1);
            Py_DECREF(s2);
            if (ro) {
                if (!SWIG_GetPtrObj(ro, (void**)&ptr, "_wxBitmap_p"))
                    rval = *ptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads();
        return rval;
    }

    PYPRIVATE;
};
%}

// The one for SWIG to see
%name(wxArtProvider) class wxPyArtProvider : public wxObject
{
public:
    wxPyArtProvider();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxArtProvider)"

    // Add new provider to the top of providers stack.
    static void PushProvider(wxPyArtProvider *provider);

    // Remove latest added provider and delete it.
    static bool PopProvider();

    // Remove provider. The provider must have been added previously!
    // The provider is _not_ deleted.
    static bool RemoveProvider(wxPyArtProvider *provider);

    // Query the providers for bitmap with given ID and return it. Return
    // wxNullBitmap if no provider provides it.
    static wxBitmap GetBitmap(const wxString& id,
                              const wxString& client = wxPyART_OTHER,
                              const wxSize& size = wxDefaultSize);

    // Query the providers for icon with given ID and return it. Return
    // wxNullIcon if no provider provides it.
    static wxIcon GetIcon(const wxString& id,
                          const wxString& client = wxPyART_OTHER,
                          const wxSize& size = wxDefaultSize);

};


//----------------------------------------------------------------------

%{
#include <wx/docview.h>
%}

class wxFileHistory : public wxObject
{
public:
    wxFileHistory(int maxFiles = 9);
    ~wxFileHistory();

    // Operations
    void AddFileToHistory(const wxString& file);
    void RemoveFileFromHistory(int i);
    int GetMaxFiles() const;
    void UseMenu(wxMenu *menu);

    // Remove menu from the list (MDI child may be closing)
    void RemoveMenu(wxMenu *menu);

    void Load(wxConfigBase& config);
    void Save(wxConfigBase& config);

    void AddFilesToMenu();
    %name(AddFilesToThisMenu)void AddFilesToMenu(wxMenu* menu);

    // Accessors
    wxString GetHistoryFile(int i) const;

    // A synonym for GetNoHistoryFiles
    int GetCount() const;
    int GetNoHistoryFiles() const;

};

//----------------------------------------------------------------------

class wxEffects: public wxObject
{
public:
    // Assume system colours
    wxEffects();

    wxColour GetHighlightColour() const;
    wxColour GetLightShadow() const;
    wxColour GetFaceColour() const;
    wxColour GetMediumShadow() const;
    wxColour GetDarkShadow() const;

    void SetHighlightColour(const wxColour& c);
    void SetLightShadow(const wxColour& c);
    void SetFaceColour(const wxColour& c);
    void SetMediumShadow(const wxColour& c);
    void SetDarkShadow(const wxColour& c);

    void Set(const wxColour& highlightColour, const wxColour& lightShadow,
             const wxColour& faceColour, const wxColour& mediumShadow,
             const wxColour& darkShadow);

    // Draw a sunken edge
    void DrawSunkenEdge(wxDC& dc, const wxRect& rect, int borderSize = 1);

    // Tile a bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);

};

//----------------------------------------------------------------------

class wxSingleInstanceChecker
{
public:
    // like Create() but no error checking (dangerous!)
    wxSingleInstanceChecker(const wxString& name,
                            const wxString& path = wxPyEmptyString);

    // default ctor, use Create() after it
    %name(wxPreSingleInstanceChecker) wxSingleInstanceChecker();

    ~wxSingleInstanceChecker();


    // name must be given and be as unique as possible, it is used as the mutex
    // name under Win32 and the lock file name under Unix -
    // wxTheApp->GetAppName() may be a good value for this parameter
    //
    // path is optional and is ignored under Win32 and used as the directory to
    // create the lock file in under Unix (default is wxGetHomeDir())
    //
    // returns FALSE if initialization failed, it doesn't mean that another
    // instance is running - use IsAnotherRunning() to check it
    bool Create(const wxString& name, const wxString& path = wxPyEmptyString);

    // is another copy of this program already running?
    bool IsAnotherRunning() const;
};

//----------------------------------------------------------------------


%init %{
    wxPyPtrTypeMap_Add("wxDragImage", "wxGenericDragImage");
    wxPyPtrTypeMap_Add("wxProcess", "wxPyProcess");
    wxPyPtrTypeMap_Add("wxArtProvider", "wxPyArtProvider");

%}

//----------------------------------------------------------------------

