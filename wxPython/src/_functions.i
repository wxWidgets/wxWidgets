/////////////////////////////////////////////////////////////////////////////
// Name:        _functions.i
// Purpose:     SWIG interface defs for various functions and such
//
// Author:      Robin Dunn
//
// Created:     3-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


%{
#include <wx/stockitem.h>
%}

//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(FileSelectorPromptStr);
MAKE_CONST_WXSTRING(FileSelectorDefaultWildcardStr);
MAKE_CONST_WXSTRING(DirSelectorPromptStr);

//---------------------------------------------------------------------------
%newgroup;


long wxNewId();
void wxRegisterId(long id);
long wxGetCurrentId();

// Returns true if the ID is in the list of recognized stock actions
bool wxIsStockID(wxWindowID id);
 
// Returns true of the label is empty or label of a stock button with
// given ID
bool wxIsStockLabel(wxWindowID id, const wxString& label);

// Returns label that should be used for given stock UI element (e.g. "&OK"
// for wxID_OK):
wxString wxGetStockLabel(wxWindowID id,
                         bool withCodes = true,
                         wxString accelerator = wxPyEmptyString);
 

MustHaveApp(wxBell);
void wxBell();

MustHaveApp(wxEndBusyCursor);
void wxEndBusyCursor();

long wxGetElapsedTime(bool resetTimer = true);

MustHaveApp(wxGetMousePosition);
DocDeclA(
    void, wxGetMousePosition(int* OUTPUT, int* OUTPUT),
    "GetMousePosition() -> (x,y)");

bool wxIsBusy();
wxString wxNow();
bool wxShell(const wxString& command = wxPyEmptyString);
void wxStartTimer();

DocDeclA(
    int, wxGetOsVersion(int *OUTPUT, int *OUTPUT),
    "GetOsVersion() -> (platform, major, minor)");

wxString wxGetOsDescription();

// TODO:
// // Parses the wildCard, returning the number of filters.
// // Returns 0 if none or if there's a problem,
// // The arrays will contain an equal number of items found before the error.
// // wildCard is in the form:
// // "All files (*)|*|Image Files (*.jpeg *.png)|*.jpg;*.png"
// int wxParseCommonDialogsFilter(const wxString& wildCard, wxArrayString& descriptions, wxArrayString& filters);

#if defined(__WXMSW__) || defined(__WXMAC__)
long wxGetFreeMemory();
#else
%inline %{
    long wxGetFreeMemory()
        { wxPyRaiseNotImplemented(); return 0; }
%}
#endif

enum wxShutdownFlags
{
    wxSHUTDOWN_POWEROFF,    // power off the computer
    wxSHUTDOWN_REBOOT       // shutdown and reboot
};

// Shutdown or reboot the PC
MustHaveApp(wxShutdown);
bool wxShutdown(wxShutdownFlags wFlags);


void wxSleep(int secs);
void wxMilliSleep(unsigned long milliseconds);
void wxMicroSleep(unsigned long microseconds);
%pythoncode { Usleep = MilliSleep }

void wxEnableTopLevelWindows(bool enable);

wxString wxStripMenuCodes(const wxString& in);


wxString wxGetEmailAddress();
wxString wxGetHostName();
wxString wxGetFullHostName();
wxString wxGetUserId();
wxString wxGetUserName();
wxString wxGetHomeDir();
wxString wxGetUserHome(const wxString& user = wxPyEmptyString);

unsigned long wxGetProcessId();

void wxTrap();


// Dialog Functions

MustHaveApp(wxFileSelector);
wxString wxFileSelector(const wxString& message = wxPyFileSelectorPromptStr,
                        const wxString& default_path = wxPyEmptyString,
                        const wxString& default_filename = wxPyEmptyString,
                        const wxString& default_extension = wxPyEmptyString,
                        const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                        int flags = 0,
                        wxWindow *parent = NULL,
                        int x = -1, int y = -1);

// TODO: wxFileSelectorEx


// Ask for filename to load
MustHaveApp(wxLoadFileSelector);
wxString wxLoadFileSelector(const wxString& what,
                            const wxString& extension,
                            const wxString& default_name = wxPyEmptyString,
                            wxWindow *parent = NULL);

// Ask for filename to save
MustHaveApp(wxSaveFileSelector);
wxString wxSaveFileSelector(const wxString& what,
                            const wxString& extension,
                            const wxString& default_name = wxPyEmptyString,
                            wxWindow *parent = NULL);


MustHaveApp(wxDirSelector);
wxString wxDirSelector(const wxString& message = wxPyDirSelectorPromptStr,
                       const wxString& defaultPath = wxPyEmptyString,
                       long style = wxDD_DEFAULT_STYLE,
                       const wxPoint& pos = wxDefaultPosition,
                       wxWindow *parent = NULL);

MustHaveApp(wxGetTextFromUser);
wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxPyEmptyString,
                           const wxString& default_value = wxPyEmptyString,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = true);

MustHaveApp(wxGetPasswordFromUser);
wxString wxGetPasswordFromUser(const wxString& message,
                               const wxString& caption = wxPyEmptyString,
                               const wxString& default_value = wxPyEmptyString,
                               wxWindow *parent = NULL);


// TODO: Need to custom wrap this one...
// int wxGetMultipleChoice(char* message, char* caption,
//                         int LCOUNT, char** choices,
//                         int nsel, int *selection,
//                         wxWindow *parent = NULL, int x = -1, int y = -1,
//                         bool centre = true, int width=150, int height=200);


MustHaveApp(wxGetSingleChoice);
wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                           int choices, wxString* choices_array,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = true,
                           int width=150, int height=200);

MustHaveApp(wxGetSingleChoiceIndex);
int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int choices, wxString* choices_array,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = true,
                           int width=150, int height=200);


MustHaveApp(wxMessageBox);
int wxMessageBox(const wxString& message,
                 const wxString& caption = wxPyEmptyString,
                 int style = wxOK | wxCENTRE,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

// WXWIN_COMPATIBILITY_2_4
MustHaveApp(wxGetNumberFromUser);
long wxGetNumberFromUser(const wxString& message,
                         const wxString& prompt,
                         const wxString& caption,
                         long value,
                         long min = 0, long max = 100,
                         wxWindow *parent = NULL,
                         const wxPoint& pos = wxDefaultPosition);

// GDI Functions

MustHaveApp(wxColourDisplay);
bool wxColourDisplay();

MustHaveApp(wxDisplayDepth);
int wxDisplayDepth();

MustHaveApp(wxGetDisplayDepth);
int wxGetDisplayDepth();

MustHaveApp(wxDisplaySize);
DocDeclA(
    void, wxDisplaySize(int* OUTPUT, int* OUTPUT),
    "DisplaySize() -> (width, height)");

MustHaveApp(wxGetDisplaySize);
wxSize wxGetDisplaySize();

MustHaveApp(wxDisplaySizeMM);
DocDeclA(
    void, wxDisplaySizeMM(int* OUTPUT, int* OUTPUT),
    "DisplaySizeMM() -> (width, height)");

MustHaveApp(wxGetDisplaySizeMM);
wxSize wxGetDisplaySizeMM();

MustHaveApp(wxClientDisplayRect);
DocDeclA(
    void, wxClientDisplayRect(int *OUTPUT, int *OUTPUT, int *OUTPUT, int *OUTPUT),
    "ClientDisplayRect() -> (x, y, width, height)");

MustHaveApp(wxGetClientDisplayRect);
wxRect wxGetClientDisplayRect();


MustHaveApp(wxSetCursor);
void wxSetCursor(wxCursor& cursor);



// Miscellaneous functions

MustHaveApp(wxBeginBusyCursor);
void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);

MustHaveApp(wxGetActiveWindow);
wxWindow * wxGetActiveWindow();

MustHaveApp(wxGenericFindWindowAtPoint);
wxWindow* wxGenericFindWindowAtPoint(const wxPoint& pt);

MustHaveApp(wxFindWindowAtPoint);
wxWindow* wxFindWindowAtPoint(const wxPoint& pt);

MustHaveApp(wxGetTopLevelParent);
wxWindow* wxGetTopLevelParent(wxWindow *win);

//bool wxSpawnBrowser(wxWindow *parent, wxString href);




MustHaveApp(wxGetKeyState);
DocDeclStr(
    bool , wxGetKeyState(wxKeyCode key),
    "Get the state of a key (true if pressed or toggled on, false if not.)
This is generally most useful getting the state of the modifier or
toggle keys.  On some platforms those may be the only keys that work.
", "");



//---------------------------------------------------------------------------

MustHaveApp(wxWakeUpMainThread);

#if defined(__WXMSW__) || defined(__WXMAC__)
void wxWakeUpMainThread();
#else
%inline %{
    void wxWakeUpMainThread() {}
%}
#endif


MustHaveApp(wxMutexGuiEnter);
void wxMutexGuiEnter();

MustHaveApp(wxMutexGuiLeave);
void wxMutexGuiLeave();


MustHaveApp(wxMutexGuiLocker);
class wxMutexGuiLocker  {
public:
    wxMutexGuiLocker();
    ~wxMutexGuiLocker();
};


MustHaveApp(wxThread);
%inline %{
    bool wxThread_IsMain() {
#ifdef WXP_WITH_THREAD
        return wxThread::IsMain();
#else
        return true;
#endif
    }
%}

//---------------------------------------------------------------------------

// enum wxPowerType
// {
//     wxPOWER_SOCKET,
//     wxPOWER_BATTERY,
//     wxPOWER_UNKNOWN
// };

// DocDeclStr(
//     wxPowerType , wxGetPowerType(),
//     "Returns the type of power source as one of wx.POWER_SOCKET,
// wx.POWER_BATTERY or wx.POWER_UNKNOWN.  wx.POWER_UNKNOWN is also the
// default on platforms where this feature is not implemented.", "");


// enum wxBatteryState
// {
//     wxBATTERY_NORMAL_STATE,    // system is fully usable
//     wxBATTERY_LOW_STATE,       // start to worry
//     wxBATTERY_CRITICAL_STATE,  // save quickly
//     wxBATTERY_SHUTDOWN_STATE,  // too late
//     wxBATTERY_UNKNOWN_STATE
// };

// DocDeclStr(
//     wxBatteryState , wxGetBatteryState(),
//     "Returns battery state as one of wx.BATTERY_NORMAL_STATE,
// wx.BATTERY_LOW_STATE}, wx.BATTERY_CRITICAL_STATE,
// wx.BATTERY_SHUTDOWN_STATE or wx.BATTERY_UNKNOWN_STATE.
// wx.BATTERY_UNKNOWN_STATE is also the default on platforms where this
// feature is not implemented.", "");



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
