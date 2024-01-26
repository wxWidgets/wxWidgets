/////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     wxStatusBar sample
// Author:      Vadim Zeitlin
// Created:     04.02.00
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


#if !wxUSE_STATUSBAR
    #error "You need to set wxUSE_STATUSBAR to 1 to compile this sample"
#endif // wxUSE_STATUSBAR

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
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

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

//#define USE_MDI_PARENT_FRAME 1
#ifdef USE_MDI_PARENT_FRAME
    #include "wx/mdi.h"
#endif // USE_MDI_PARENT_FRAME

static const char *SAMPLE_DIALOGS_TITLE = "wxWidgets statbar sample";

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "green.xpm"
#include "red.xpm"

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
    virtual bool OnInit() override;
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
    void OnToggleClock(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);

private:
    // toggle the state of the status bar controls
    void DoToggle();

    enum
    {
        Field_Text,
        Field_Checkbox,
        Field_Bitmap,
        Field_NumLockIndicator,
        Field_Clock,
        Field_CapsLockIndicator,
        Field_Max
    };

#if wxUSE_TIMER
    wxTimer m_timer;
#endif

#if wxUSE_CHECKBOX
    wxCheckBox *m_checkbox;
#endif
    wxStaticBitmap *m_statbmp;

    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type: this is going to be our main frame
#ifdef USE_MDI_PARENT_FRAME
class MyFrame : public wxMDIParentFrame
#else
class MyFrame : public wxFrame
#endif
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSetStatusField(wxCommandEvent& event);
    void OnSetStatusText(wxCommandEvent& event);
    void OnPushStatusText(wxCommandEvent& event);
    void OnPopStatusText(wxCommandEvent& event);

    void OnResetFieldsWidth(wxCommandEvent& event);
    void OnShowFieldsRect(wxCommandEvent& event);
    void OnSetStatusFields(wxCommandEvent& event);
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

    // the index of the field used by some commands
    int m_field;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
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

    StatusBar_SetFields = wxID_HIGHEST,
    StatusBar_SetField,
    StatusBar_SetText,
    StatusBar_PushText,
    StatusBar_PopText,
    StatusBar_SetFont,
    StatusBar_ResetFieldsWidth,
    StatusBar_ShowFieldsRect,

    StatusBar_Recreate,
    StatusBar_Toggle,
    StatusBar_Checkbox,
    StatusBar_SetPaneStyle,
    StatusBar_SetPaneStyleNormal,
    StatusBar_SetPaneStyleFlat,
    StatusBar_SetPaneStyleRaised,
    StatusBar_SetPaneStyleSunken,

    StatusBar_SetStyleSizeGrip,
    StatusBar_SetStyleEllipsizeStart,
    StatusBar_SetStyleEllipsizeMiddle,
    StatusBar_SetStyleEllipsizeEnd,
    StatusBar_SetStyleShowTips
};


// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
#ifdef USE_MDI_PARENT_FRAME
wxBEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
#else
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
#endif
    EVT_MENU(StatusBar_Quit,  MyFrame::OnQuit)
    EVT_MENU(StatusBar_SetFields, MyFrame::OnSetStatusFields)
    EVT_MENU(StatusBar_SetField, MyFrame::OnSetStatusField)
    EVT_MENU(StatusBar_SetText, MyFrame::OnSetStatusText)
    EVT_MENU(StatusBar_PushText, MyFrame::OnPushStatusText)
    EVT_MENU(StatusBar_PopText, MyFrame::OnPopStatusText)
    EVT_MENU(StatusBar_SetFont, MyFrame::OnSetStatusFont)
    EVT_MENU(StatusBar_ResetFieldsWidth, MyFrame::OnResetFieldsWidth)
    EVT_MENU(StatusBar_ShowFieldsRect, MyFrame::OnShowFieldsRect)
    EVT_MENU(StatusBar_Recreate, MyFrame::OnRecreateStatusBar)
    EVT_MENU(StatusBar_About, MyFrame::OnAbout)
    EVT_MENU(StatusBar_Toggle, MyFrame::OnStatusBarToggle)
    EVT_MENU(StatusBar_SetPaneStyleNormal, MyFrame::OnSetPaneStyle)
    EVT_MENU(StatusBar_SetPaneStyleFlat, MyFrame::OnSetPaneStyle)
    EVT_MENU(StatusBar_SetPaneStyleRaised, MyFrame::OnSetPaneStyle)
    EVT_MENU(StatusBar_SetPaneStyleSunken, MyFrame::OnSetPaneStyle)

    EVT_MENU(StatusBar_SetStyleSizeGrip, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeStart, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeMiddle, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleEllipsizeEnd, MyFrame::OnSetStyle)
    EVT_MENU(StatusBar_SetStyleShowTips, MyFrame::OnSetStyle)

    EVT_UPDATE_UI_RANGE(StatusBar_SetFields, StatusBar_ResetFieldsWidth,
                        MyFrame::OnUpdateForDefaultStatusbar)
    EVT_UPDATE_UI(StatusBar_Toggle, MyFrame::OnUpdateStatusBarToggle)
    EVT_UPDATE_UI_RANGE(StatusBar_SetPaneStyleNormal,
                        StatusBar_SetPaneStyleSunken,
                        MyFrame::OnUpdateSetPaneStyle)
    EVT_UPDATE_UI_RANGE(StatusBar_SetStyleSizeGrip, StatusBar_SetStyleShowTips,
                        MyFrame::OnUpdateSetStyle)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
#if wxUSE_CHECKBOX
    EVT_CHECKBOX(StatusBar_Checkbox, MyStatusBar::OnToggleClock)
#endif
#if wxUSE_TIMER
    EVT_TIMER(wxID_ANY, MyStatusBar::OnTimer)
#endif
    EVT_IDLE(MyStatusBar::OnIdle)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

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
    MyFrame *frame = new MyFrame("wxStatusBar sample",
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
    : wxMDIParentFrame(nullptr, wxID_ANY, title, pos, size)
#else
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
#endif
{
    SetIcon(wxICON(sample));

    m_statbarPaneStyle = wxSB_NORMAL;
    m_field = 1;

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(StatusBar_Quit, "E&xit\tAlt-X",
                     "Quit this program");

    wxMenu *statbarMenu = new wxMenu;

    wxMenu *statbarStyleMenu = new wxMenu;
    statbarStyleMenu->Append(StatusBar_SetStyleSizeGrip, "wxSTB_SIZE_GRIP",
                             "Toggles the wxSTB_SIZE_GRIP style", true);
    statbarStyleMenu->Append(StatusBar_SetStyleShowTips, "wxSTB_SHOW_TIPS",
                             "Toggles the wxSTB_SHOW_TIPS style", true);
    statbarStyleMenu->AppendSeparator();
    statbarStyleMenu->AppendCheckItem(StatusBar_SetStyleEllipsizeStart,
                                      "wxSTB_ELLIPSIZE_START",
                                      "Toggle wxSTB_ELLIPSIZE_START style");
    statbarStyleMenu->AppendCheckItem(StatusBar_SetStyleEllipsizeMiddle,
                                      "wxSTB_ELLIPSIZE_MIDDLE",
                                      "Toggle wxSTB_ELLIPSIZE_MIDDLE style");
    statbarStyleMenu->AppendCheckItem(StatusBar_SetStyleEllipsizeEnd,
                                      "wxSTB_ELLIPSIZE_END",
                                      "Toggle wxSTB_ELLIPSIZE_END style");
    statbarMenu->Append(StatusBar_SetPaneStyle, "Status bar style",
                        statbarStyleMenu);
    statbarMenu->AppendSeparator();

    statbarMenu->Append(StatusBar_SetField, "Set active field &number\tCtrl-N",
                        "Set the number of field used by the next commands.");
    statbarMenu->Append(StatusBar_SetText, "Set field &text\tCtrl-T",
                        "Set the text of the selected field.");
    statbarMenu->Append(StatusBar_PushText, "P&ush field text\tCtrl-P",
                        "Push a message on top the selected field.");
    statbarMenu->Append(StatusBar_PopText, "&Pop field text\tShift-Ctrl-P",
                        "Restore the previous contents of the selected field.");
    statbarMenu->AppendSeparator();

    statbarMenu->Append(StatusBar_SetFields, "&Set field count\tCtrl-C",
                        "Set the number of status bar fields");
    statbarMenu->Append(StatusBar_SetFont, "&Set field font\tCtrl-F",
                        "Set the font to use for status bar fields");

    wxMenu *statbarPaneStyleMenu = new wxMenu;
    statbarPaneStyleMenu->AppendCheckItem
        (
            StatusBar_SetPaneStyleNormal,
            "&Normal",
            "Sets the style of the first field to normal (sunken) look"
        );
    statbarPaneStyleMenu->AppendCheckItem
        (
            StatusBar_SetPaneStyleFlat,
            "&Flat",
            "Sets the style of the first field to flat look"
        );
    statbarPaneStyleMenu->AppendCheckItem
        (
            StatusBar_SetPaneStyleRaised,
            "&Raised",
            "Sets the style of the first field to raised look"
        );
    statbarPaneStyleMenu->AppendCheckItem
        (
            StatusBar_SetPaneStyleSunken,
            "&Sunken",
            "Sets the style of the first field to sunken look"
        );
    statbarMenu->Append(StatusBar_SetPaneStyle, "Field style",
                        statbarPaneStyleMenu);

    statbarMenu->Append(StatusBar_ResetFieldsWidth, "Reset field widths",
                        "Sets all fields to the same width");
    statbarMenu->Append(StatusBar_ShowFieldsRect,
                        "Sho&w field rectangles\tCtrl-W",
                        "Visually show field rectangles");
    statbarMenu->AppendSeparator();

    statbarMenu->AppendCheckItem(StatusBar_Toggle, "&Toggle Status Bar",
                                 "Toggle the status bar display");
    statbarMenu->Append(StatusBar_Recreate, "&Recreate\tCtrl-R",
                        "Toggle status bar format");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(StatusBar_About, "&About\tCtrl-A",
                     "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(statbarMenu, "&Status bar");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create default status bar to start with
    DoCreateStatusBar(StatBar_Default, wxSTB_DEFAULT_STYLE);
    SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::DoCreateStatusBar(MyFrame::StatusBarKind kind, long style)
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        SetStatusBar(nullptr);
        delete statbarOld;
    }

    wxStatusBar *statbarNew = nullptr;
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
            wxFAIL_MSG("unknown status bar kind");
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

void MyFrame::OnSetStatusField(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    long rc = wxGetNumberFromUser
              (
                "Configure the field index to be used by the set, push "
                "and pop text commands in the menu.\n"
                "\n"
                "0 corresponds to the first field, 1 to the second one "
                "and so on.",
                "Field &index:",
                SAMPLE_DIALOGS_TITLE,
                m_field,
                0,
                sb->GetFieldsCount() - 1,
                nullptr
              );

    if ( rc == -1 )
        return;

    m_field = rc;

    wxLogStatus("Status bar text will be set for field #%d", m_field);
}

void MyFrame::OnSetStatusText(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    wxString txt = wxGetTextFromUser
                   (
                        wxString::Format
                        (
                            "Enter the text from for the field #%d",
                            m_field
                        ),
                        SAMPLE_DIALOGS_TITLE,
                        sb->GetStatusText(m_field),
                        this
                   );

    if ( txt.empty() )
        return;

    sb->SetStatusText(txt, m_field);
}

// the current depth of the stack used by Push/PopStatusText()
static int gs_depth = 0;

void MyFrame::OnPushStatusText(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    static int s_countPush = 0;
    sb->PushStatusText(wxString::Format
                       (
                            "Pushed message #%d (depth = %d)",
                            ++s_countPush, ++gs_depth
                       ), m_field);
}

void MyFrame::OnPopStatusText(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    if ( !gs_depth )
    {
        wxLogStatus("No message to pop.");
        return;
    }

    gs_depth--;
    sb->PopStatusText(m_field);
}

void MyFrame::OnSetStatusFont(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();
    if (!sb)
        return;

    wxFont
        fnt = wxGetFontFromUser(this, sb->GetFont(), "Choose status bar font");
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
                    "Select the number of fields in the status bar",
                    "Fields:",
                    SAMPLE_DIALOGS_TITLE,
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
            nullptr,            // 1 field: default
            widthsFor2Fields,   // 2 fields: 1 fixed, 1 var
            widthsFor3Fields,   // 3 fields: 3 var
            widthsFor4Fields,   // 4 fields: 3 fixed, 2 vars
            nullptr             // 5 fields: default (all have same width)
        };

        const int * const widths = widthsAll[nFields - 1];
        sb->SetFieldsCount(nFields, widths);

        wxString s;
        for ( long n = 0; n < nFields; n++ )
        {
            if ( widths )
            {
                if ( widths[n] > 0 )
                    s.Printf("fixed (%d)", widths[n]);
                else
                    s.Printf("variable (*%d)", -widths[n]);
            }
            else
            {
                s = "default";
            }

            SetStatusText(s, n);
        }

        if ( m_field >= nFields )
            m_field = nFields - 1;
    }
    else
    {
        wxLogStatus(this, "Cancelled");
    }
}

void MyFrame::OnResetFieldsWidth(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *pStat = GetStatusBar();
    if ( !pStat )
        return;

    const int n = pStat->GetFieldsCount();
    pStat->SetStatusWidths(n, nullptr);
    for ( int i = 0; i < n; i++ )
        pStat->SetStatusText("same size", i);
}

void MyFrame::OnShowFieldsRect(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *pStat = GetStatusBar();
    if ( !pStat )
        return;

    wxClientDC dc(pStat);
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    // Not all systems support drawing using wxClientDC, so also show the
    // coordinates in a message box.
    wxString msg;
    const wxSize size = pStat->GetClientSize();
    msg.Printf("Status bar client size is (%d,%d)\n\n", size.x, size.y);

    const int n = pStat->GetFieldsCount();
    for ( int i = 0; i < n; i++ )
    {
        wxRect r;
        if ( pStat->GetFieldRect(i, r) )
        {
            msg += wxString::Format("Field %d rectangle is (%d,%d)-(%d,%d)\n",
                                    i,
                                    r.x, r.y,
                                    r.x + r.width,
                                    r.y + r.height);
            dc.DrawRectangle(r);
        }
    }

    wxLogMessage("%s", msg);
}

void MyFrame::OnUpdateStatusBarToggle(wxUpdateUIEvent& event)
{
    event.Check(GetStatusBar() != nullptr);
}

void MyFrame::OnStatusBarToggle(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        SetStatusBar(nullptr);
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
        case StatusBar_SetPaneStyleSunken:
            event.Check(m_statbarPaneStyle == wxSB_SUNKEN);
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
        case StatusBar_SetPaneStyleSunken:
            m_statbarPaneStyle = wxSB_SUNKEN;
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

    #define STB_ELLIPSIZE_MASK \
        (wxSTB_ELLIPSIZE_START|wxSTB_ELLIPSIZE_MIDDLE|wxSTB_ELLIPSIZE_END)

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
            : wxDialog(parent, wxID_ANY, wxString("About statbar"),
                        wxDefaultPosition, wxDefaultSize,
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxStaticText *text = new wxStaticText(this, wxID_ANY,
                                        "wxStatusBar sample\n"
                                        "(c) 2000 Vadim Zeitlin");

    wxButton *btn = new wxButton(this, wxID_OK, "&Close");

    // create the top status bar without the size grip (default style),
    // otherwise it looks weird
    wxStatusBar *statbarTop = new wxStatusBar(this, wxID_ANY, 0);
    statbarTop->SetFieldsCount(3);
    statbarTop->SetStatusText("This is a top status bar", 0);
    statbarTop->SetStatusText("in a dialog", 1);
    statbarTop->SetStatusText("Great, isn't it?", 2);

    wxStatusBar *statbarBottom = new wxStatusBar(this, wxID_ANY);
    statbarBottom->SetFieldsCount(2);
    statbarBottom->SetStatusText("This is a bottom status bar", 0);
    statbarBottom->SetStatusText("in a dialog", 1);

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

static const char *numlockIndicators[] = { "OFF", "NUM" };
static const char *capslockIndicators[] = { "", "CAPS" };

MyStatusBar::MyStatusBar(wxWindow *parent, long style)
        : wxStatusBar(parent, wxID_ANY, style, "MyStatusBar")
#if wxUSE_TIMER
            , m_timer(this)
#endif
#if wxUSE_CHECKBOX
            , m_checkbox(nullptr)
#endif
{
    // compute the size needed for num lock indicator pane
    wxClientDC dc(this);
    wxSize sizeNumLock = dc.GetTextExtent(numlockIndicators[0]);
    sizeNumLock.IncTo(dc.GetTextExtent(numlockIndicators[1]));

    int widths[Field_Max];
    widths[Field_Text] = -1; // growable
    widths[Field_Checkbox] = 150;
    widths[Field_Bitmap] = -1; // growable
    widths[Field_NumLockIndicator] = sizeNumLock.x;
    widths[Field_Clock] = 100;
    widths[Field_CapsLockIndicator] = dc.GetTextExtent(capslockIndicators[1]).x;

    SetFieldsCount(Field_Max);
    SetStatusWidths(Field_Max, widths);

#if wxUSE_CHECKBOX
    m_checkbox = new wxCheckBox(this, StatusBar_Checkbox, "&Toggle clock");
    m_checkbox->SetValue(true);
    AddFieldControl(Field_Checkbox, m_checkbox);
#endif

    m_statbmp = new wxStaticBitmap(this, wxID_ANY, wxIcon(green_xpm));
    AddFieldControl(Field_Bitmap, m_statbmp);

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

void MyStatusBar::OnToggleClock(wxCommandEvent& WXUNUSED(event))
{
    DoToggle();
}

void MyStatusBar::OnIdle(wxIdleEvent& event)
{
    SetStatusText(numlockIndicators[wxGetKeyState(WXK_NUMLOCK)],
                  Field_NumLockIndicator);
    SetStatusText(capslockIndicators[wxGetKeyState(WXK_CAPITAL)],
                  Field_CapsLockIndicator);

    event.Skip();
}

void MyStatusBar::DoToggle()
{
#if wxUSE_CHECKBOX
    if ( m_checkbox->GetValue() )
    {
#if wxUSE_TIMER
        m_timer.Start(1000);
#endif

        m_statbmp->SetIcon(wxIcon(green_xpm));

        UpdateClock();
    }
    else // don't show clock
    {
#if wxUSE_TIMER
        m_timer.Stop();
#endif

        m_statbmp->SetIcon(wxIcon(red_xpm));

        SetStatusText(wxEmptyString, Field_Clock);
    }
#endif // wxUSE_CHECKBOX
}

void MyStatusBar::UpdateClock()
{
    SetStatusText(wxDateTime::Now().FormatTime(), Field_Clock);
}
