/////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     wxStatusBar sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.02.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_STATUSBAR
    #error "You need to set wxUSE_STATUSBAR to 1 to compile this sample"
#endif // wxUSE_STATUSBAR

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/statusbr.h"
    #include "wx/timer.h"
    #include "wx/checkbox.h"
    #include "wx/statbmp.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/textdlg.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/bmpbuttn.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/datetime.h"
#include "wx/numdlg.h"
#include "wx/fontdlg.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif


// define this for the platforms which don't support wxBitmapButton (such as
// Motif), else a wxBitmapButton will be used
#ifdef __WXMOTIF__
    #define USE_STATIC_BITMAP
#endif

//#define USE_MDI_PARENT_FRAME 1

#ifdef USE_MDI_PARENT_FRAME
    #include "wx/mdi.h"
#endif // USE_MDI_PARENT_FRAME


// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifdef USE_STATIC_BITMAP
    #include "green.xpm"
    #include "red.xpm"
#endif // USE_STATIC_BITMAP

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// A custom status bar which contains controls, icons &c
class MyStatusBar : public wxStatusBar
{
public:
    MyStatusBar(wxWindow *parent, long style = wxSTB_DEFAULT_STYLE);
    virtual ~MyStatusBar();

    void UpdateClock();

    // event handlers
#if wxUSE_TIMER
    void OnTimer(wxTimerEvent& WXUNUSED(event)) { UpdateClock(); }
#endif
    void OnSize(wxSizeEvent& event);
    void OnToggleClock(wxCommandEvent& event);
    void OnButton(wxCommandEvent& event);

private:
    // toggle the state of the status bar controls
    void DoToggle();

    wxBitmap CreateBitmapForButton(bool on = false);

    enum
    {
        Field_Text,
        Field_Checkbox,
        Field_Bitmap,
        Field_Clock,
        Field_Max
    };

#if wxUSE_TIMER
    wxTimer m_timer;
#endif

#if wxUSE_CHECKBOX
    wxCheckBox *m_checkbox;
#endif
#ifdef USE_STATIC_BITMAP
    wxStaticBitmap *m_statbmp;
#else
    wxBitmapButton *m_statbmp;
#endif

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
#ifdef USE_MDI_PARENT_FRAME
class MyFrame : public wxMDIParentFrame
#else
    virtual ~MyFrame();
#endif

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnResetFieldsWidth(wxCommandEvent& event);
    void OnSetStatusFields(wxCommandEvent& event);
    void OnSetStatusTexts(wxCommandEvent& event);
    void OnSetStatusFont(wxCommandEvent& event);
    void OnRecreateStatusBar(wxCommandEvent& event);

    void OnSetPaneStyle(wxCommandEvent& event);
    void OnSetStyle(wxCommandEvent& event);

private:
    enum StatusBarKind
    {
        StatBar_Default,
        StatBar_Custom,
        StatBar_Max
    } m_statbarKind;


    void OnUpdateForDefaultStatusbar(wxUpdateUIEvent& event);
    void OnUpdateStatusBarToggle(wxUpdateUIEvent& event);
    void OnUpdateSetPaneStyle(wxUpdateUIEvent& event);
    void OnUpdateSetStyle(wxUpdateUIEvent& event);
    void OnStatusBarToggle(wxCommandEvent& event);
    void DoCreateStatusBar(StatusBarKind kind, long style);
    void ApplyPaneStyle();

    int m_statbarPaneStyle;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// Our about dialog ith its status bar
class MyAboutDialog : public wxDialog
{
public:
    MyAboutDialog(wxWindow *parent);
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    StatusBar_Quit = wxID_EXIT,
    StatusBar_About = wxID_ABOUT,

    StatusBar_SetFields = wxID_HIGHEST+1,
    StatusBar_SetTexts,
    StatusBar_SetFont,
    StatusBar_ResetFieldsWidth,

    StatusBar_Recreate,
    StatusBar_Toggle,
    StatusBar_Checkbox,
    StatusBar_SetPaneStyle,
    StatusBar_SetPaneStyleNormal,
    StatusBar_SetPaneStyleFlat,
    StatusBar_SetPaneStyleRaised,

    StatusBar_SetStyleSizeGrip,
    StatusBar_SetStyleEllipsizeStart,
    StatusBar_SetStyleEllipsizeMiddle,
    StatusBar_SetStyleEllipsizeEnd,
    StatusBar_SetStyleShowTips
};

static const int BITMAP_SIZE_X = 32;
static const int BITMAP_SIZE_Y = 15;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
#ifdef USE_MDI_PARENT_FRAME
BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
#else
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
#endif
    EVT_MENU(StatusBar_Quit,  MyFrame::OnQuit)
    EVT_MENU(StatusBar_SetFields, MyFrame::OnSetStatusFields)
    EVT_MENU(StatusBar_SetTexts, MyFrame::OnSetStatusTexts)
    EVT_MENU(StatusBar_SetFont, MyFrame::OnSetStatusFont)
    EVT_MENU(StatusBar_ResetFieldsWidth, MyFrame::OnResetFieldsWidth)
    EVT_MENU(StatusBar_Recreate, MyFrame::OnRecreateStatusBar)
    EVT_MENU(StatusBar_About, MyFrame::OnAbout)
    EVT_MENU(StatusBar_Toggle, MyFrame::OnStatusBarToggle)
    EVT_MENU(StatusBar_SetPaneStyleNormal, MyFrame::OnSetPaneStyle)
    EVT_MENU(StatusBar_SetPaneStyleFlat, MyFrame::OnSetPaneStyle)
    EVT_MENU(StatusBar_SetPaneStyleRaised, MyFrame::OnSetPaneStyle)

    EVT_MENU(StatusBar_SetStyleSizeGrip, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeStart, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeMiddle, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeEnd, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleShowTips, MyFrame::OnSetStyle)

    EVT_UPDATE_UI_RANGE(StatusBar_SetFields, StatusBar_ResetFieldsWidth,
                        MyFrame::OnUpdateForDefaultStatusbar)
    EVT_UPDATE_UI(StatusBar_Toggle, MyFrame::OnUpdateStatusBarToggle)
    EVT_UPDATE_UI_RANGE(StatusBar_SetPaneStyleNormal, StatusBar_SetPaneStyleRaised,
                        MyFrame::OnUpdateSetPaneStyle)
    EVT_UPDATE_UI_RANGE(StatusBar_SetStyleSizeGrip, StatusBar_SetStyleShowTips,
                        MyFrame::OnUpdateSetStyle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
    EVT_SIZE(MyStatusBar::OnSize)
#if wxUSE_CHECKBOX
    EVT_CHECKBOX(StatusBar_Checkbox, MyStatusBar::OnToggleClock)
#endif
    EVT_BUTTON(wxID_ANY, MyStatusBar::OnButton)
#if wxUSE_TIMER
    EVT_TIMER(wxID_ANY, MyStatusBar::OnTimer)
#endif
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(wxT("wxStatusBar sample"),
                                wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned 'false' here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
#ifdef USE_MDI_PARENT_FRAME
    : wxMDIParentFrame((wxWindow *)NULL, wxID_ANY, title, pos, size)
#else
    : wxFrame((wxWindow *)NULL, wxID_ANY, title, pos, size)
#endif
{
    SetIcon(wxICON(sample));

    m_statbarPaneStyle = wxSB_NORMAL;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = StatusBar_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(StatusBar_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    wxMenu *statbarMenu = new wxMenu;

    wxMenu *statbarStyleMenu = new wxMenu;
    statbarStyleMenu->Append(StatusBar_SetStyleSizeGrip, wxT("wxSTB_SIZE_GRIP"), wxT("Toggles the wxSTB_SIZE_GRIP style"), true);
    statbarStyleMenu->Append(StatusBar_SetStyleShowTips, wxT("wxSTB_SHOW_TIPS"), wxT("Toggles the wxSTB_SHOW_TIPS style"), true);
    statbarStyleMenu->AppendSeparator();
    statbarStyleMenu->Append(StatusBar_SetStyleEllipsizeStart, wxT("wxSTB_ELLIPSIZE_START"), wxT("Toggles the wxSTB_ELLIPSIZE_START style"), true);
    statbarStyleMenu->Append(StatusBar_SetStyleEllipsizeMiddle, wxT("wxSTB_ELLIPSIZE_MIDDLE"), wxT("Toggles the wxSTB_ELLIPSIZE_MIDDLE style"), true);
    statbarStyleMenu->Append(StatusBar_SetStyleEllipsizeEnd, wxT("wxSTB_ELLIPSIZE_END"), wxT("Toggles the wxSTB_ELLIPSIZE_END style"), true);
    statbarMenu->Append(StatusBar_SetPaneStyle, wxT("Status bar style"), statbarStyleMenu);
    statbarMenu->AppendSeparator();

    statbarMenu->Append(StatusBar_SetFields, wxT("&Set field count\tCtrl-C"),
                        wxT("Set the number of status bar fields"));
    statbarMenu->Append(StatusBar_SetTexts, wxT("&Set field text\tCtrl-T"),
                        wxT("Set the text to display for each status bar field"));
    statbarMenu->Append(StatusBar_SetFont, wxT("&Set field font\tCtrl-F"),
                        wxT("Set the font to use for rendering status bar fields"));

    wxMenu *statbarPaneStyleMenu = new wxMenu;
    statbarPaneStyleMenu->Append(StatusBar_SetPaneStyleNormal, wxT("&Normal"), wxT("Sets the style of the first field to normal (sunken) look"), true);
    statbarPaneStyleMenu->Append(StatusBar_SetPaneStyleFlat, wxT("&Flat"), wxT("Sets the style of the first field to flat look"), true);
    statbarPaneStyleMenu->Append(StatusBar_SetPaneStyleRaised, wxT("&Raised"), wxT("Sets the style of the first field to raised look"), true);
    statbarMenu->Append(StatusBar_SetPaneStyle, wxT("Field style"), statbarPaneStyleMenu);

    statbarMenu->Append(StatusBar_ResetFieldsWidth, wxT("Reset field widths"),
                        wxT("Sets all fields to the same width"));
    statbarMenu->AppendSeparator();

    statbarMenu->Append(StatusBar_Toggle, wxT("&Toggle Status Bar"),
                        wxT("Toggle the status bar display"), true);
    statbarMenu->Append(StatusBar_Recreate, wxT("&Recreate\tCtrl-R"),
                        wxT("Toggle status bar format"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(StatusBar_About, wxT("&About...\tCtrl-A"), wxT("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(statbarMenu, wxT("&Status bar"));
    menuBar->Append(helpMenu, wxT("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create default status bar to start with
    DoCreateStatusBar(StatBar_Default, wxSTB_DEFAULT_STYLE);
    SetStatusText(wxT("Welcome to wxWidgets!"));
}

MyFrame::~MyFrame()
{
}

void MyFrame::DoCreateStatusBar(MyFrame::StatusBarKind kind, long style)
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        SetStatusBar(NULL);
        delete statbarOld;
    }

    wxStatusBar *statbarNew = NULL;
    switch ( kind )
    {
        case StatBar_Default:
            statbarNew = new wxStatusBar(this, wxID_ANY, style, "wxStatusBar");
            statbarNew->SetFieldsCount(2);
            break;

        case StatBar_Custom:
            statbarNew = new MyStatusBar(this, style);
            break;

        default:
            wxFAIL_MSG(wxT("unknown status bar kind"));
    }

    SetStatusBar(statbarNew);
    ApplyPaneStyle();
    PositionStatusBar();

    m_statbarKind = kind;
}


// ----------------------------------------------------------------------------
// main frame - event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnUpdateForDefaultStatusbar(wxUpdateUIEvent& event)
{
    // only allow this feature for the default status bar
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    event.Enable(sb->GetName() == "wxStatusBar");
}

void MyFrame::OnSetStatusTexts(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    wxString txt;
    for (int i=0; i<sb->GetFieldsCount(); i++)
    {
        txt =
            wxGetTextFromUser(wxString::Format("Enter the text for the %d-th field:", i+1),
                              "Input field text", "A dummy test string", this);

        sb->SetStatusText(txt, i);
    }
}

void MyFrame::OnSetStatusFont(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    wxFont fnt = wxGetFontFromUser(this, sb->GetFont(), "Choose statusbar font");
    if (fnt.IsOk())
    {
        sb->SetFont(fnt);
        sb->SetSize(sb->GetBestSize());
    }
}

void MyFrame::OnSetStatusFields(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    long nFields = wxGetNumberFromUser
                (
                    wxT("Select the number of fields in the status bar"),
                    wxT("Fields:"),
                    wxT("wxWidgets statusbar sample"),
                    sb->GetFieldsCount(),
                    1, 5,
                    this
                );

    // we don't check if the number changed at all on purpose: calling
    // SetFieldsCount() with the same number of fields should be ok
    if ( nFields != -1 )
    {
        static const int widthsFor2Fields[] = { 200, -1 };
        static const int widthsFor3Fields[] = { -1, -2, -1 };
        static const int widthsFor4Fields[] = { 100, -1, 100, -2, 100 };

        static const int *widthsAll[] =
        {
            NULL,               // 1 field: default
            widthsFor2Fields,   // 2 fields: 1 fixed, 1 var
            widthsFor3Fields,   // 3 fields: 3 var
            widthsFor4Fields,   // 4 fields: 3 fixed, 2 vars
            NULL                // 5 fields: default (all have same width)
        };

        const int * const widths = widthsAll[nFields - 1];
        sb->SetFieldsCount(nFields, widths);

        wxString s;
        for ( long n = 0; n < nFields; n++ )
        {
            if ( widths )
            {
                if ( widths[n] > 0 )
                    s.Printf(wxT("fixed (%d)"), widths[n]);
                else
                    s.Printf(wxT("variable (*%d)"), -widths[n]);
            }
            else
            {
                s = wxT("default");
            }

            SetStatusText(s, n);
        }
    }
    else
    {
        wxLogStatus(this, wxT("Cancelled"));
    }
}

void MyFrame::OnResetFieldsWidth(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *pStat = GetStatusBar();
    if (!pStat)
        return;

    int n = pStat->GetFieldsCount();
    pStat->SetStatusWidths(n, NULL);
    for (int i=0; i<n; i++)
        pStat->SetStatusText("same size", i);
}

void MyFrame::OnUpdateStatusBarToggle(wxUpdateUIEvent& event)
{
    event.Check(GetStatusBar() != NULL);
}

void MyFrame::OnStatusBarToggle(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        SetStatusBar(NULL);
        delete statbarOld;
    }
    else
    {
        DoCreateStatusBar(m_statbarKind, wxSTB_DEFAULT_STYLE);
    }
}

void MyFrame::OnRecreateStatusBar(wxCommandEvent& WXUNUSED(event))
{
    DoCreateStatusBar(m_statbarKind == StatBar_Custom ? StatBar_Default
                                                      : StatBar_Custom,
                      wxSTB_DEFAULT_STYLE);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    MyAboutDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnUpdateSetPaneStyle(wxUpdateUIEvent& event)
{
    switch (event.GetId())
    {
        case StatusBar_SetPaneStyleNormal:
            event.Check(m_statbarPaneStyle == wxSB_NORMAL);
            break;
        case StatusBar_SetPaneStyleFlat:
            event.Check(m_statbarPaneStyle == wxSB_FLAT);
            break;
        case StatusBar_SetPaneStyleRaised:
            event.Check(m_statbarPaneStyle == wxSB_RAISED);
            break;
    }
}

void MyFrame::OnSetPaneStyle(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case StatusBar_SetPaneStyleNormal:
            m_statbarPaneStyle = wxSB_NORMAL;
            break;
        case StatusBar_SetPaneStyleFlat:
            m_statbarPaneStyle = wxSB_FLAT;
            break;
        case StatusBar_SetPaneStyleRaised:
            m_statbarPaneStyle = wxSB_RAISED;
            break;
    }

    ApplyPaneStyle();
}

void MyFrame::ApplyPaneStyle()
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    int fields = sb->GetFieldsCount();
    int *styles = new int[fields];

    for (int i = 1; i < fields; i++)
        styles[i] = wxSB_NORMAL;

    styles[0] = m_statbarPaneStyle;

    sb->SetStatusStyles(fields, styles);

    delete [] styles;
}

void MyFrame::OnUpdateSetStyle(wxUpdateUIEvent& event)
{
    long currentStyle = wxSTB_DEFAULT_STYLE;
    if (GetStatusBar())
        currentStyle = GetStatusBar()->GetWindowStyle();

    switch (event.GetId())
    {
        case StatusBar_SetStyleSizeGrip:
            event.Check((currentStyle & wxSTB_SIZEGRIP) != 0);
            break;
        case StatusBar_SetStyleShowTips:
            event.Check((currentStyle & wxSTB_SHOW_TIPS) != 0);
            break;

        case StatusBar_SetStyleEllipsizeStart:
            event.Check((currentStyle & wxSTB_ELLIPSIZE_START) != 0);
            break;
        case StatusBar_SetStyleEllipsizeMiddle:
            event.Check((currentStyle & wxSTB_ELLIPSIZE_MIDDLE) != 0);
            break;
        case StatusBar_SetStyleEllipsizeEnd:
            event.Check((currentStyle & wxSTB_ELLIPSIZE_END) != 0);
            break;
    }
}

void MyFrame::OnSetStyle(wxCommandEvent& event)
{
    long oldStyle = wxSTB_DEFAULT_STYLE;
    if (GetStatusBar())
        oldStyle = GetStatusBar()->GetWindowStyle();

#define STB_ELLIPSIZE_MASK  (wxSTB_ELLIPSIZE_START|wxSTB_ELLIPSIZE_MIDDLE|wxSTB_ELLIPSIZE_END)

    long newStyle = oldStyle;
    long newStyleBit = 0;
    switch (event.GetId())
    {
        case StatusBar_SetStyleSizeGrip:
            newStyleBit = wxSTB_SIZEGRIP;
            break;
        case StatusBar_SetStyleShowTips:
            newStyleBit = wxSTB_SHOW_TIPS;
            break;

        case StatusBar_SetStyleEllipsizeStart:
            newStyleBit = wxSTB_ELLIPSIZE_START;
            newStyle &= ~STB_ELLIPSIZE_MASK;
            break;
        case StatusBar_SetStyleEllipsizeMiddle:
            newStyleBit = wxSTB_ELLIPSIZE_MIDDLE;
            newStyle &= ~STB_ELLIPSIZE_MASK;
            break;
        case StatusBar_SetStyleEllipsizeEnd:
            newStyleBit = wxSTB_ELLIPSIZE_END;
            newStyle &= ~STB_ELLIPSIZE_MASK;
            break;
    }

    newStyle = event.IsChecked() ? (newStyle | newStyleBit) :
                                   (newStyle & ~newStyleBit);
    if (newStyle != oldStyle)
    {
        DoCreateStatusBar(m_statbarKind, newStyle);
        SetStatusText("Status bar recreated with a new style");
    }
}

// ----------------------------------------------------------------------------
// MyAboutDialog
// ----------------------------------------------------------------------------

MyAboutDialog::MyAboutDialog(wxWindow *parent)
            : wxDialog(parent, wxID_ANY, wxString(wxT("About statbar")),
                        wxDefaultPosition, wxDefaultSize,
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxStaticText *text = new wxStaticText(this, wxID_ANY,
                                        wxT("wxStatusBar sample\n")
                                        wxT("(c) 2000 Vadim Zeitlin"));

    wxButton *btn = new wxButton(this, wxID_OK, wxT("&Close"));

    // create the top status bar without the size grip (default style),
    // otherwise it looks weird
    wxStatusBar *statbarTop = new wxStatusBar(this, wxID_ANY, 0);
    statbarTop->SetFieldsCount(3);
    statbarTop->SetStatusText(wxT("This is a top status bar"), 0);
    statbarTop->SetStatusText(wxT("in a dialog"), 1);
    statbarTop->SetStatusText(wxT("Great, isn't it?"), 2);

    wxStatusBar *statbarBottom = new wxStatusBar(this, wxID_ANY);
    statbarBottom->SetFieldsCount(2);
    statbarBottom->SetStatusText(wxT("This is a bottom status bar"), 0);
    statbarBottom->SetStatusText(wxT("in a dialog"), 1);

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(statbarTop, 0, wxGROW);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(text, 0, wxCENTRE | wxRIGHT | wxLEFT, 20);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(btn, 0, wxCENTRE | wxRIGHT | wxLEFT, 20);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(statbarBottom, 0, wxGROW);

    SetSizerAndFit(sizerTop);
}

// ----------------------------------------------------------------------------
// MyStatusBar
// ----------------------------------------------------------------------------

#ifdef __VISUALC__
    // 'this' : used in base member initializer list -- so what??
    #pragma warning(disable: 4355)
#endif

MyStatusBar::MyStatusBar(wxWindow *parent, long style)
        : wxStatusBar(parent, wxID_ANY, style, "MyStatusBar")
#if wxUSE_TIMER
            , m_timer(this)
#endif
#if wxUSE_CHECKBOX
            , m_checkbox(NULL)
#endif
{
    static const int widths[Field_Max] = { -1, 150, BITMAP_SIZE_X, 100 };

    SetFieldsCount(Field_Max);
    SetStatusWidths(Field_Max, widths);

#if wxUSE_CHECKBOX
    m_checkbox = new wxCheckBox(this, StatusBar_Checkbox, wxT("&Toggle clock"));
    m_checkbox->SetValue(true);
#endif

#ifdef USE_STATIC_BITMAP
    m_statbmp = new wxStaticBitmap(this, wxID_ANY, wxIcon(green_xpm));
#else
    m_statbmp = new wxBitmapButton(this, wxID_ANY, CreateBitmapForButton(),
                                wxDefaultPosition, wxDefaultSize,
                                wxBU_EXACTFIT);
#endif

#if wxUSE_TIMER
    m_timer.Start(1000);
#endif

    SetMinHeight(wxMax(m_statbmp->GetBestSize().GetHeight(),
                       m_checkbox->GetBestSize().GetHeight()));

    UpdateClock();
}

#ifdef __VISUALC__
    #pragma warning(default: 4355)
#endif

MyStatusBar::~MyStatusBar()
{
#if wxUSE_TIMER
    if ( m_timer.IsRunning() )
    {
        m_timer.Stop();
    }
#endif
}

#define BMP_BUTTON_SIZE_X  10
#define BMP_BUTTON_SIZE_Y  10

wxBitmap MyStatusBar::CreateBitmapForButton(bool on)
{
    wxBitmap bitmap(BMP_BUTTON_SIZE_X+1, BMP_BUTTON_SIZE_Y+1);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    dc.SetBrush(on ? *wxGREEN_BRUSH : *wxRED_BRUSH);
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.Clear();
    dc.DrawEllipse(0, 0, BMP_BUTTON_SIZE_X, BMP_BUTTON_SIZE_Y);
    dc.SelectObject(wxNullBitmap);

    return bitmap;
}

void MyStatusBar::OnSize(wxSizeEvent& event)
{
#if wxUSE_CHECKBOX
    if ( !m_checkbox )
        return;
#endif

    // TEMPORARY HACK: TODO find a more general solution
#ifdef wxStatusBarGeneric
    wxStatusBar::OnSize(event);
#endif

    wxRect rect;
    if (!GetFieldRect(Field_Checkbox, rect))
    {
        event.Skip();
        return;
    }

#if wxUSE_CHECKBOX
    m_checkbox->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);
#endif

    GetFieldRect(Field_Bitmap, rect);
    wxSize size = m_statbmp->GetSize();

    m_statbmp->Move(rect.x + (rect.width - size.x) / 2,
                    rect.y + (rect.height - size.y) / 2);

    event.Skip();
}

void MyStatusBar::OnButton(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_CHECKBOX
    m_checkbox->SetValue(!m_checkbox->GetValue());
#endif

    DoToggle();
}

void MyStatusBar::OnToggleClock(wxCommandEvent& WXUNUSED(event))
{
    DoToggle();
}

void MyStatusBar::DoToggle()
{
#if wxUSE_CHECKBOX
    if ( m_checkbox->GetValue() )
    {
#if wxUSE_TIMER
        m_timer.Start(1000);
#endif

#ifdef USE_STATIC_BITMAP
        m_statbmp->SetIcon(wxIcon(green_xpm));
#else
        m_statbmp->SetBitmapLabel(CreateBitmapForButton(false));
        m_statbmp->Refresh();
#endif

        UpdateClock();
    }
    else // don't show clock
    {
#if wxUSE_TIMER
        m_timer.Stop();
#endif

#ifdef USE_STATIC_BITMAP
        m_statbmp->SetIcon(wxIcon(red_xpm));
#else
        m_statbmp->SetBitmapLabel(CreateBitmapForButton(true));
        m_statbmp->Refresh();
#endif

        SetStatusText(wxEmptyString, Field_Clock);
    }
#endif
}

void MyStatusBar::UpdateClock()
{
    SetStatusText(wxDateTime::Now().FormatTime(), Field_Clock);
}
