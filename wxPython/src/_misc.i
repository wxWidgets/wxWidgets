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
    %typemap(out) wxToolTip*;    // turn off this typemap
    wxToolTip(const wxString &tip);
    // Turn it back on again
    %typemap(out) wxToolTip* { $result = wxPyMake_wxObject($1, $owner); }

    ~wxToolTip();
    
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

    %extend {
        %pythonAppend Destroy "args[0].thisown = 0"
        DocStr(Destroy,
               "Deletes the C++ object this Python object is a proxy for.", "");
        void Destroy() {
            delete self;
        }
    }
    
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

    static int GetBlinkTime();
    static void SetBlinkTime(int milliseconds);

    %property(Position, GetPosition, doc="See `GetPosition`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
    %property(Window, GetWindow, doc="See `GetWindow`");
    
};


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

    %pythoncode { def Destroy(self): pass }
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

    %property(Count, GetCount, doc="See `GetCount`");
    %property(HistoryFile, GetHistoryFile, doc="See `GetHistoryFile`");
    %property(MaxFiles, GetMaxFiles, doc="See `GetMaxFiles`");
    %property(NoHistoryFiles, GetNoHistoryFiles, doc="See `GetNoHistoryFiles`");
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
%newgroup

// families & sub-families of operating systems
enum wxOperatingSystemId
{
    wxOS_UNKNOWN = 0,                 // returned on error

    wxOS_MAC_OS         = 1 << 0,     // Apple Mac OS 8/9/X with Mac paths
    wxOS_MAC_OSX_DARWIN = 1 << 1,     // Apple Mac OS X with Unix paths
    wxOS_MAC = wxOS_MAC_OS|wxOS_MAC_OSX_DARWIN,

    wxOS_WINDOWS_9X     = 1 << 2,     // Windows 9x family (95/98/ME)
    wxOS_WINDOWS_NT     = 1 << 3,     // Windows NT family (NT/2000/XP)
    wxOS_WINDOWS_MICRO  = 1 << 4,     // MicroWindows
    wxOS_WINDOWS_CE     = 1 << 5,     // Windows CE (Window Mobile)
    wxOS_WINDOWS = wxOS_WINDOWS_9X      |
                   wxOS_WINDOWS_NT      |
                   wxOS_WINDOWS_MICRO   |
                   wxOS_WINDOWS_CE,

    wxOS_UNIX_LINUX     = 1 << 6,       // Linux
    wxOS_UNIX_FREEBSD   = 1 << 7,       // FreeBSD
    wxOS_UNIX_OPENBSD   = 1 << 8,       // OpenBSD
    wxOS_UNIX_NETBSD    = 1 << 9,       // NetBSD
    wxOS_UNIX_SOLARIS   = 1 << 10,      // SunOS
    wxOS_UNIX_AIX       = 1 << 11,      // AIX
    wxOS_UNIX_HPUX      = 1 << 12,      // HP/UX
    wxOS_UNIX = wxOS_UNIX_LINUX     |
                wxOS_UNIX_FREEBSD   |
                wxOS_UNIX_OPENBSD   |
                wxOS_UNIX_NETBSD    |
                wxOS_UNIX_SOLARIS   |
                wxOS_UNIX_AIX       |
                wxOS_UNIX_HPUX,

    // 1<<13 and 1<<14 available for other Unix flavours

    wxOS_DOS            = 1 << 15,      // Microsoft DOS
    wxOS_OS2            = 1 << 16       // OS/2
};

// list of wxWidgets ports - some of them can be used with more than
// a single toolkit.
enum wxPortId
{
    wxPORT_UNKNOWN  = 0,            // returned on error

    wxPORT_BASE     = 1 << 0,       // wxBase, no native toolkit used

    wxPORT_MSW      = 1 << 1,       // wxMSW, native toolkit is Windows API
    wxPORT_MOTIF    = 1 << 2,       // wxMotif, using [Open]Motif or Lesstif
    wxPORT_GTK      = 1 << 3,       // wxGTK, using GTK+ 1.x, 2.x, GPE or Maemo
    wxPORT_MGL      = 1 << 4,       // wxMGL, using wxUniversal
    wxPORT_X11      = 1 << 5,       // wxX11, using wxUniversal
    wxPORT_PM       = 1 << 6,       // wxOS2, using OS/2 Presentation Manager
    wxPORT_OS2      = wxPORT_PM,    // wxOS2, using OS/2 Presentation Manager
    wxPORT_MAC      = 1 << 7,       // wxMac, using Carbon or Classic Mac API
    wxPORT_COCOA    = 1 << 8,       // wxCocoa, using Cocoa NextStep/Mac API
    wxPORT_WINCE    = 1 << 9,       // wxWinCE, toolkit is WinCE SDK API
    wxPORT_PALMOS   = 1 << 10,       // wxPalmOS, toolkit is PalmOS API
    wxPORT_DFB      = 1 << 11       // wxDFB, using wxUniversal
};

// architecture of the operating system
// (regardless of the build environment of wxWidgets library - see
// wxIsPlatform64bit documentation for more info)
enum wxArchitecture
{
    wxARCH_INVALID = -1,        // returned on error

    wxARCH_32,                  // 32 bit
    wxARCH_64,

    wxARCH_MAX
};


// endian-ness of the machine
enum wxEndianness
{
    wxENDIAN_INVALID = -1,      // returned on error

    wxENDIAN_BIG,               // 4321
    wxENDIAN_LITTLE,            // 1234
    wxENDIAN_PDP,               // 3412

    wxENDIAN_MAX
};

// Information about the toolkit that the app is running under and some basic
// platform and architecture info

%rename(PlatformInformation) wxPlatformInfo; // wxPython already has a wx.PlatformInfo

class  wxPlatformInfo
{
public:
    wxPlatformInfo();
//     wxPlatformInfo(wxPortId pid,
//                    int tkMajor = -1, int tkMinor = -1,
//                    wxOperatingSystemId id = wxOS_UNKNOWN,
//                    int osMajor = -1, int osMinor = -1,
//                    wxArchitecture arch = wxARCH_INVALID,
//                    wxEndianness endian = wxENDIAN_INVALID,
//                    bool usingUniversal = false);

    // default copy ctor, assignment operator and dtor are ok

    bool operator==(const wxPlatformInfo &t) const;

    bool operator!=(const wxPlatformInfo &t) const;


//     // string -> enum conversions
//     // ---------------------------------

//     static wxOperatingSystemId GetOperatingSystemId(const wxString &name);
//     static wxPortId GetPortId(const wxString &portname);

//     static wxArchitecture GetArch(const wxString &arch);
//     static wxEndianness GetEndianness(const wxString &end);

//     // enum -> string conversions
//     // ---------------------------------

//     static wxString GetOperatingSystemFamilyName(wxOperatingSystemId os);
//     static wxString GetOperatingSystemIdName(wxOperatingSystemId os);
//     static wxString GetPortIdName(wxPortId port, bool usingUniversal);
//     static wxString GetPortIdShortName(wxPortId port, bool usingUniversal);

//     static wxString GetArchName(wxArchitecture arch);
//     static wxString GetEndiannessName(wxEndianness end);

    // getters
    // -----------------

    int GetOSMajorVersion() const;
    int GetOSMinorVersion() const;

    int GetToolkitMajorVersion() const;
    int GetToolkitMinorVersion() const;

    bool IsUsingUniversalWidgets() const;

    wxOperatingSystemId GetOperatingSystemId() const;
    wxPortId GetPortId() const;
    wxArchitecture GetArchitecture() const;
    wxEndianness GetEndianness() const;


    // string getters
    // -----------------

    wxString GetOperatingSystemFamilyName() const;
    wxString GetOperatingSystemIdName() const;
    wxString GetPortIdName() const;
    wxString GetPortIdShortName() const;
    wxString GetArchName() const;
    wxString GetEndiannessName() const;

    // setters
    // -----------------

    void SetOSVersion(int major, int minor);
    void SetToolkitVersion(int major, int minor);

    void SetOperatingSystemId(wxOperatingSystemId n);
    void SetPortId(wxPortId n);
    void SetArchitecture(wxArchitecture n);
    void SetEndianness(wxEndianness n);

    // miscellaneous
    // -----------------

    bool IsOk() const;
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Experimental...

%{
#ifdef __WXMSW__
#include <wx/msw/private.h>
#include <wx/dynload.h>
#endif
%}


%inline %{

bool wxDrawWindowOnDC(wxWindow* window, const wxDC& dc
#if 0
                      , int method
#endif
    )
{
#ifdef __WXMSW__
#if 0
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
#endif
            // This one works much better, nearly all widgets and their
            // children are captured correctly[**].  Prior to the big
            // background erase changes that Vadim did in 2004-2005 this
            // method failed badly on XP with Themes activated, most native
            // widgets draw only partially, if at all.  Without themes it
            // worked just like on Win2k.  After those changes this method
            // works very well.
            //
            // ** For example the radio buttons in a wxRadioBox are not its
            // children by default, but you can capture it via the panel
            // instead, or change RADIOBTN_PARENT_IS_RADIOBOX in radiobox.cpp.
            ::SendMessage(GetHwndOf(window), WM_PRINT, (long)GetHdcOf(dc),
                          PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN |
                          PRF_ERASEBKGND | PRF_OWNED );
            return true;
#if 0
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
#endif  // 0
#else
    return false;
#endif  // __WXMSW__    
}

%}



#if 0
%{
    void t_output_tester1(int* a, int* b, int* c, int* d)
    {
        *a = 1234;
        *b = 2345;
        *c = 3456;
        *d = 4567;
    }
    PyObject* t_output_tester2(int* a, int* b, int* c, int* d)
    {
        *a = 1234;
        *b = 2345;
        *c = 3456;
        *d = 4567;
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject* t_output_tester3(int* a, int* b, int* c, int* d)
    {
        *a = 1234;
        *b = 2345;
        *c = 3456;
        *d = 4567;
        PyObject* res = PyTuple_New(2);
        PyTuple_SetItem(res, 0, PyInt_FromLong(1));
        PyTuple_SetItem(res, 1, PyInt_FromLong(2));
        return res;
    }
    PyObject* t_output_tester4()
    {
        PyObject* res = PyTuple_New(2);
        PyTuple_SetItem(res, 0, PyInt_FromLong(132));
        PyTuple_SetItem(res, 1, PyInt_FromLong(244));
        return res;
    }
%}    

%newobject t_output_tester2;
%newobject t_output_tester3;
%newobject t_output_tester4;

void      t_output_tester1(int* OUTPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
PyObject* t_output_tester2(int* OUTPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
PyObject* t_output_tester3(int* OUTPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
PyObject* t_output_tester4();

#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
