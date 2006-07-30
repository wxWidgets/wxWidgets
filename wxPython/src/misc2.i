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
#include <wx/resource.h>
#include <wx/tooltip.h>
#include <wx/caret.h>
#include <wx/fontenum.h>
#include <wx/tipdlg.h>
#include <wx/process.h>

#if wxUSE_JOYSTICK || defined(__WXMSW__)
#include <wx/joystick.h>
#endif

#if wxUSE_WAVE || defined(__WXMSW__)
#include <wx/wave.h>
#endif

#include <wx/mimetype.h>
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

%{
    static wxString wxPyEmptyStr("");
%}

//---------------------------------------------------------------------------
// Dialog Functions

wxString wxFileSelector(char* message,
                        char* default_path = NULL,
                        char* default_filename = NULL,
                        char* default_extension = NULL,
                        char* wildcard = "*.*",
                        int flags = 0,
                        wxWindow *parent = NULL,
                        int x = -1, int y = -1);

wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxPyEmptyStr,
                           const wxString& default_value = wxPyEmptyStr,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE);

wxString wxGetPasswordFromUser(const wxString& message,
                               const wxString& caption = wxPyEmptyStr,
                               const wxString& default_value = wxPyEmptyStr,
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
                 const wxString& caption = wxPyEmptyStr,
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

wxWindow * wxFindWindowByLabel(const wxString& label, wxWindow *parent=NULL);
wxWindow * wxFindWindowByName(const wxString& name, wxWindow *parent=NULL);

void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
wxWindow * wxGetActiveWindow();

wxWindow* wxGenericFindWindowAtPoint(const wxPoint& pt);
wxWindow* wxFindWindowAtPoint(const wxPoint& pt);

#ifdef __WXMSW__
bool wxCheckForInterrupt(wxWindow *wnd);
void wxFlushEvents();
#endif

//---------------------------------------------------------------------------
// Resource System

bool wxResourceAddIdentifier(char *name, int value);
void wxResourceClear(void);
wxBitmap  wxResourceCreateBitmap(char *resource);
wxIcon  wxResourceCreateIcon(char *resource);
wxMenuBar * wxResourceCreateMenuBar(char *resource);
int wxResourceGetIdentifier(char *name);
bool wxResourceParseData(char *resource, wxResourceTable *table = NULL);
bool wxResourceParseFile(char *filename, wxResourceTable *table = NULL);
bool wxResourceParseString(char *resource, wxResourceTable *table = NULL);

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
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_BTNTEXT,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_LISTBOX,

    wxSYS_COLOUR_DESKTOP = wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_3DFACE = wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_3DSHADOW = wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_3DHIGHLIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_3DHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNHILIGHT = wxSYS_COLOUR_BTNHIGHLIGHT
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


    // the backwards compatible versions, don't use these methods in the new
    // code!
    static wxColour GetSystemColour(int index);
    static wxFont GetSystemFont(int index);
    static int GetSystemMetric(int index);

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

%{
class wxPyFontEnumerator : public wxFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}

    DEC_PYCALLBACK_BOOL_STRING(OnFacename);
    DEC_PYCALLBACK_BOOL_STRINGSTRING(OnFontEncoding);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_STRING(wxPyFontEnumerator, wxFontEnumerator, OnFacename);
IMP_PYCALLBACK_BOOL_STRINGSTRING(wxPyFontEnumerator, wxFontEnumerator, OnFontEncoding);

%}

%name(wxFontEnumerator) class wxPyFontEnumerator {
public:
    wxPyFontEnumerator();
    ~wxPyFontEnumerator();
    void _setCallbackInfo(PyObject* self, PyObject* _class, bool incref);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxFontEnumerator, 0)"

    bool EnumerateFacenames(
        wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
        bool fixedWidthOnly = FALSE);
    bool EnumerateEncodings(const char* facename = "");

    //wxArrayString* GetEncodings();
    //wxArrayString* GetFacenames();
    %addmethods {
        PyObject* GetEncodings() {
            wxArrayString* arr = self->GetEncodings();
            return wxArrayString2PyList_helper(*arr);
        }

        PyObject* GetFacenames() {
            wxArrayString* arr = self->GetFacenames();
            return wxArrayString2PyList_helper(*arr);
        }
    }
};

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

bool wxSafeYield(wxWindow* win=NULL);
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

};


// The C++ version of wxPyTipProvider
%{
class wxPyTipProvider : public wxTipProvider {
public:
    wxPyTipProvider(size_t currentTip)
        : wxTipProvider(currentTip) {}

    DEC_PYCALLBACK_STRING__pure(GetTip);

    PYPRIVATE;
};

IMP_PYCALLBACK_STRING__pure( wxPyTipProvider, wxTipProvider, GetTip);

%}


// Now let SWIG know about it
class wxPyTipProvider : public wxTipProvider {
public:
    wxPyTipProvider(size_t currentTip);
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

    %name(BeginDrag2) bool BeginDrag(const wxPoint& hotspot, wxWindow* window,
                                     wxWindow* fullScreenRect);

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
    void Start(int milliseconds=-1, int oneShot=FALSE);
    void Stop();
};


class wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch();
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


class wxLog
{
public:
    wxLog();

    static bool IsEnabled();
    static bool EnableLogging(bool doIt = TRUE);
    static void OnLog(wxLogLevel level, const char *szString, int t=0);

    virtual void Flush();
    bool HasPendingMessages() const;

    static void FlushActive();
    static wxLog *GetActiveTarget();
    static wxLog *SetActiveTarget(wxLog *pLogger);

    static void Suspend();
    static void Resume();

    void SetVerbose(bool bVerbose = TRUE);

    static void DontCreateOnDemand();
    static void SetTraceMask(wxTraceMask ulMask);
    static void AddTraceMask(const wxString& str);
    static void RemoveTraceMask(const wxString& str);
    static void ClearTraceMasks();

    static void SetTimestamp(const wxChar *ts);
    static const wxChar *GetTimestamp();

    bool GetVerbose() const { return m_bVerbose; }

    static wxTraceMask GetTraceMask();
    static bool IsAllowedTraceMask(const char *mask);

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
            const char *szTitle,          // the title of the frame
            bool bShow = TRUE,            // show window immediately?
            bool bPassToOld = TRUE);      // pass log messages to the old target?

    void Show(bool bShow = TRUE);
    wxFrame *GetFrame() const;
    wxLog *GetOldLog() const;
    bool IsPassingMessages() const;
    void PassMessages(bool bDoPass) { m_bPassMessages = bDoPass; }
};


class wxLogNull
{
public:
    wxLogNull();
    ~wxLogNull();
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
const char* wxSysErrorMsg(unsigned long nErrCode = 0);
void wxLogFatalError(const char *szFormat);
void wxLogError(const char *szFormat);
void wxLogWarning(const char *szFormat);
void wxLogMessage(const char *szFormat);
void wxLogInfo(const char *szFormat);
void wxLogVerbose(const char *szFormat);
void wxLogStatus(const char *szFormat);
%name(wxLogStatusFrame)void wxLogStatus(wxFrame *pFrame, const char *szFormat);
void wxLogSysError(const char *szFormat);


%{
// A Log class that can be derived from in wxPython
class wxPyLog : public wxLog {
public:
    wxPyLog() : wxLog() {}

    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t) {
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLog")))
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(isi)", level, szString, t));
        wxPyEndBlockThreads();
        if (! found)
            wxLog::DoLog(level, szString, t);
    }

    virtual void DoLogString(const wxChar *szString, time_t t) {
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoLogString")))
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(si)", szString, t));
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
};



long wxExecute(const wxString& command,
               int sync = FALSE,
               wxPyProcess *process = NULL);

//----------------------------------------------------------------------

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
    wxFileTypeInfo(const char *mimeType,
                   const char *openCmd,
                   const char *printCmd,
                   const char *desc);


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
        MessageParameters(const wxString& filename=wxPyEmptyStr,
                          const wxString& mimetype=wxPyEmptyStr);

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
            if (self->GetMimeType(&str))
                return PyString_FromString(str.c_str());
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
                                                              "wxIcon", TRUE));
                PyTuple_SetItem(tuple, 1, PyString_FromString(iconFile.c_str()));
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
            if (self->GetDescription(&str))
                return PyString_FromString(str.c_str());
            else
                RETURN_NONE();
        }
    }


    // get the command to open/execute the file of given type
    %addmethods {
        PyObject* GetOpenCommand(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyStr) {
            wxString str;
            if (self->GetOpenCommand(&str, wxFileType::MessageParameters(filename, mimetype)))
                return PyString_FromString(str.c_str());
            else
                RETURN_NONE();
        }
    }


    // get the command to print the file of given type
    %addmethods {
        PyObject* GetPrintCommand(const wxString& filename,
                                  const wxString& mimetype=wxPyEmptyStr) {
            wxString str;
            if (self->GetPrintCommand(&str, wxFileType::MessageParameters(filename, mimetype)))
                return PyString_FromString(str.c_str());
            else
                RETURN_NONE();
        }
    }


    // Get all commands defined for this file type
    %addmethods {
        PyObject* GetAllCommands(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyStr) {
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

    bool SetDefaultIcon(const wxString& cmd = wxEmptyString, int index = 0);


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
    void Initialize(int mailcapStyle = wxMAILCAP_STANDARD,
                    const wxString& extraDir = wxEmptyString);

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
    %name(AddFilesToSingleMenu)void AddFilesToMenu(wxMenu* menu);

    // Accessors
    wxString GetHistoryFile(int i) const;

    // A synonym for GetNoHistoryFiles
    int GetCount() const;
    int GetNoHistoryFiles() const;

};

//----------------------------------------------------------------------
//----------------------------------------------------------------------


%init %{
    wxPyPtrTypeMap_Add("wxFontEnumerator", "wxPyFontEnumerator");
    wxPyPtrTypeMap_Add("wxDragImage", "wxGenericDragImage");
    wxPyPtrTypeMap_Add("wxProcess", "wxPyProcess");
%}

//----------------------------------------------------------------------

