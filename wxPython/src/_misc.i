/////////////////////////////////////////////////////////////////////////////
// Name:        _misc.i
// Purpose:     SWIG interface definitions for lots of little stuff that
//              don't deserve their own file.  ;-)
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


#ifndef __WXX11__

MustHaveApp(wxToolTip);

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
#endif

//---------------------------------------------------------------------------

MustHaveApp(wxCaret);

class wxCaret {
public:
    wxCaret(wxWindow* window, const wxSize& size);
    ~wxCaret();

    bool IsOk();
    bool IsVisible();

    wxPoint GetPosition();
    DocDeclAName(
        void, GetPosition(int *OUTPUT, int *OUTPUT),
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);

    wxSize GetSize();
    DocDeclAName(
        void, GetSize( int *OUTPUT, int *OUTPUT ),
        "GetSizeTuple() -> (width, height)",
        GetSizeTuple);
    

    wxWindow *GetWindow();
    %Rename(MoveXY, void, Move(int x, int y));
    void Move(const wxPoint& pt);
    %Rename(SetSizeWH,  void, SetSize(int width, int height));
    void SetSize(const wxSize& size);
    void Show(int show = true);
    void Hide();

    %pythoncode { def __nonzero__(self): return self.IsOk() }
};

%inline %{
    int wxCaret_GetBlinkTime() {
        return wxCaret::GetBlinkTime();
    }

    void wxCaret_SetBlinkTime(int milliseconds) {
        wxCaret::SetBlinkTime(milliseconds);
    }
%}

//---------------------------------------------------------------------------

MustHaveApp(wxBusyCursor);

class  wxBusyCursor {
public:
    wxBusyCursor(wxCursor* cursor = wxHOURGLASS_CURSOR);
    ~wxBusyCursor();
};

//---------------------------------------------------------------------------

MustHaveApp(wxWindowDisabler);

class wxWindowDisabler {
public:
    wxWindowDisabler(wxWindow *winToSkip = NULL);
    ~wxWindowDisabler();
};

//---------------------------------------------------------------------------

MustHaveApp(wxBusyInfo);

class wxBusyInfo : public wxObject {
public:
    wxBusyInfo(const wxString& message);
    ~wxBusyInfo();
};


//---------------------------------------------------------------------------


// wxStopWatch: measure time intervals with up to 1ms resolution
class  wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch();

    // start the stop watch at the moment t0
    void Start(long t0 = 0);

    // pause the stop watch
    void Pause();
    
    // resume it
    void Resume();

    // get elapsed time since the last Start() in milliseconds
    long Time() const;
};



//---------------------------------------------------------------------------

class wxFileHistory : public wxObject
{
public:
    wxFileHistory(int maxFiles = 9, wxWindowID idBase = wxID_FILE1);
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
    %Rename(AddFilesToThisMenu, void, AddFilesToMenu(wxMenu* menu));

    // Accessors
    wxString GetHistoryFile(int i) const;

    int GetCount() const;
    %pythoncode { GetNoHistoryFiles = GetCount }

};


//---------------------------------------------------------------------------

%{
#include <wx/snglinst.h>
%}

class wxSingleInstanceChecker
{
public:
    // like Create() but no error checking (dangerous!)
    wxSingleInstanceChecker(const wxString& name,
                            const wxString& path = wxPyEmptyString);

    // default ctor, use Create() after it
    %RenameCtor(PreSingleInstanceChecker,  wxSingleInstanceChecker());

    ~wxSingleInstanceChecker();


    // name must be given and be as unique as possible, it is used as the mutex
    // name under Win32 and the lock file name under Unix -
    // wxTheApp->GetAppName() may be a good value for this parameter
    //
    // path is optional and is ignored under Win32 and used as the directory to
    // create the lock file in under Unix (default is wxGetHomeDir())
    //
    // returns False if initialization failed, it doesn't mean that another
    // instance is running - use IsAnotherRunning() to check it
    bool Create(const wxString& name, const wxString& path = wxPyEmptyString);

    // is another copy of this program already running?
    bool IsAnotherRunning() const;
};

//---------------------------------------------------------------------------
// Experimental...



%{
#ifdef __WXMSW__
#include <wx/msw/private.h>
#include <wx/dynload.h>
#endif
%}


%inline %{

void wxDrawWindowOnDC(wxWindow* window, const wxDC& dc, int method)
{
#ifdef __WXMSW__

    switch (method)
    {
        case 1:
            // This one only partially works.  Appears to be an undocumented
            // "standard" convention that not all widgets adhear to.  For
            // example, for some widgets backgrounds or non-client areas may
            // not be painted.
            ::SendMessage(GetHwndOf(window), WM_PAINT, (long)GetHdcOf(dc), 0);
            break;

        case 2:
            // This one works much better, except for on XP.  On Win2k nearly
            // all widgets and their children are captured correctly[**].  On
            // XP with Themes activated most native widgets draw only
            // partially, if at all.  Without themes it works just like on
            // Win2k.
            //
            // ** For example the radio buttons in a wxRadioBox are not its
            // children by default, but you can capture it via the panel
            // instead, or change RADIOBTN_PARENT_IS_RADIOBOX in radiobox.cpp.
            ::SendMessage(GetHwndOf(window), WM_PRINT, (long)GetHdcOf(dc),
                          PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN |
                          PRF_ERASEBKGND | PRF_OWNED );
            break;

        case 3:
            // This one is only defined in the latest SDK and is only
            // available on XP.  MSDN says it is similar to sending WM_PRINT
            // so I expect that it will work similar to the above.  Since it
            // is avaialble only on XP, it can't be compiled like this and
            // will have to be loaded dynamically.
            // //::PrintWindow(GetHwndOf(window), GetHdcOf(dc), 0); //break;

            // fall through

        case 4:
            // Use PrintWindow if available, or fallback to WM_PRINT
            // otherwise.  Unfortunately using PrintWindow is even worse than
            // WM_PRINT.  For most native widgets nothing is drawn to the dc
            // at all, with or without Themes.
            typedef BOOL (WINAPI *PrintWindow_t)(HWND, HDC, UINT);
            static bool s_triedToLoad = false;
            static PrintWindow_t pfnPrintWindow = NULL;
            if ( !s_triedToLoad )
            {

                s_triedToLoad = true;
                wxDynamicLibrary dllUser32(_T("user32.dll"));
                if ( dllUser32.IsLoaded() )
                {
                    wxLogNull nolog;  // Don't report errors here
                    pfnPrintWindow = (PrintWindow_t)dllUser32.GetSymbol(_T("PrintWindow"));
                }
            }
            if (pfnPrintWindow)
            {
                //printf("Using PrintWindow\n");
                pfnPrintWindow(GetHwndOf(window), GetHdcOf(dc), 0);
            }
            else
            {
                //printf("Using WM_PRINT\n");
                ::SendMessage(GetHwndOf(window), WM_PRINT, (long)GetHdcOf(dc),
                              PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN |
                              PRF_ERASEBKGND | PRF_OWNED );
            }
    }
#endif
}

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
