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


//---------------------------------------------------------------------------

%{
    DECLARE_DEF_STRING(FileSelectorPromptStr);
    DECLARE_DEF_STRING(FileSelectorDefaultWildcardStr);
    DECLARE_DEF_STRING(DirSelectorPromptStr);
%}

//---------------------------------------------------------------------------
%newgroup;


long wxNewId();
void wxRegisterId(long id);
long wxGetCurrentId();

void wxBell();
void wxEndBusyCursor();

long wxGetElapsedTime(bool resetTimer = TRUE);
#ifdef __WXMSW__
long wxGetFreeMemory();
#endif
void wxGetMousePosition(int* OUTPUT, int* OUTPUT);
bool wxIsBusy();
wxString wxNow();
bool wxShell(const wxString& command = wxPyEmptyString);
void wxStartTimer();
int wxGetOsVersion(int *OUTPUT, int *OUTPUT);
wxString wxGetOsDescription();

enum wxShutdownFlags
{
    wxSHUTDOWN_POWEROFF,    // power off the computer
    wxSHUTDOWN_REBOOT       // shutdown and reboot
};

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags);


void wxSleep(int secs);
void wxUsleep(unsigned long milliseconds);
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
                           int choices, wxString* choices_array,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);

int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int choices, wxString* choices_array,
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



// Miscellaneous functions

void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
wxWindow * wxGetActiveWindow();

wxWindow* wxGenericFindWindowAtPoint(const wxPoint& pt);
wxWindow* wxFindWindowAtPoint(const wxPoint& pt);

#ifdef __WXMSW__
bool wxCheckForInterrupt(wxWindow *wnd);
// link error? void wxFlushEvents();
#endif

wxWindow* wxGetTopLevelParent(wxWindow *win);

//bool wxSpawnBrowser(wxWindow *parent, wxString href);



//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
