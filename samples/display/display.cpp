/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     wxWidgets sample showing the features of wxDisplay class
// Author:      Vadim Zeitlin
// Modified by: Ryan Norton & Brian Victor
// Created:     23.02.03
// Copyright:   (c) Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/bookctrl.h"
#include "wx/sysopt.h"

#include "wx/display.h"


// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

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
    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnFromPoint(wxCommandEvent& event);
    void OnFullScreen(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

#if wxUSE_DISPLAY
    void OnChangeMode(wxCommandEvent& event);
    void OnResetMode(wxCommandEvent& event);

    void OnDisplayChanged(wxDisplayChangedEvent& event);
#endif // wxUSE_DISPLAY

    void OnLeftClick(wxMouseEvent& event);

private:
    // Fill m_book with the information about all the displays.
    void PopuplateWithDisplayInfo();

#if wxUSE_DISPLAY
    // convert video mode to textual description
    wxString VideoModeToText(const wxVideoMode& mode);
#endif // wxUSE_DISPLAY

    // GUI controls
    wxBookCtrl *m_book;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

#if wxUSE_DISPLAY
// Client data class for the choice control containing the video modes
class MyVideoModeClientData : public wxClientData
{
public:
    MyVideoModeClientData(const wxVideoMode& m) : mode(m) { }

    const wxVideoMode mode;
};
#endif // wxUSE_DISPLAY

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Display_FromPoint = wxID_HIGHEST + 1,
    Display_FullScreen,

    // controls
    Display_ChangeMode,
    Display_ResetMode,
    Display_CurrentMode,


    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Display_Quit = wxID_EXIT,
    Display_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Display_Quit,  MyFrame::OnQuit)
    EVT_MENU(Display_FromPoint,  MyFrame::OnFromPoint)
    EVT_MENU(Display_FullScreen, MyFrame::OnFullScreen)
    EVT_MENU(Display_About, MyFrame::OnAbout)

#if wxUSE_DISPLAY
    EVT_CHOICE(Display_ChangeMode, MyFrame::OnChangeMode)
    EVT_BUTTON(Display_ResetMode, MyFrame::OnResetMode)

    EVT_DISPLAY_CHANGED(MyFrame::OnDisplayChanged)
#endif // wxUSE_DISPLAY

    EVT_LEFT_UP(MyFrame::OnLeftClick)
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

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_("Display wxWidgets Sample"),
                                 wxDefaultPosition, wxDefaultSize);

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show();

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuDisplay = new wxMenu;
    menuDisplay->Append(Display_FromPoint, _("Find from &point..."));
    menuDisplay->AppendSeparator();
    wxMenuItem* const
        itemFullScreen = new wxMenuItem(menuDisplay,
                                        Display_FullScreen,
                                        _("Full &screen\tF12"));
    itemFullScreen->SetBitmap(
            wxArtProvider::GetBitmap(wxART_FULL_SCREEN, wxART_MENU)
        );
    menuDisplay->Append(itemFullScreen);
    menuDisplay->AppendSeparator();
    menuDisplay->Append(Display_Quit, _("E&xit\tAlt-X"), _("Quit this program"));

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Display_About, _("&About\tF1"), _("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuDisplay, _("&Display"));
    menuBar->Append(helpMenu, _("&Help"));

    EnableFullScreenView();

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create status bar
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // create child controls
    m_book = new wxBookCtrl(this, wxID_ANY);
    PopuplateWithDisplayInfo();
}

void MyFrame::PopuplateWithDisplayInfo()
{
    const size_t countDpy = wxDisplay::GetCount();
    for ( size_t nDpy = 0; nDpy < countDpy; nDpy++ )
    {
        wxDisplay display(nDpy);

        wxWindow *page = new wxPanel(m_book, wxID_ANY);

        // create 2 column flex grid sizer with growable 2nd column
        wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 10, 20);
        sizer->AddGrowableCol(1);

        const wxRect r(display.GetGeometry());
        sizer->Add(new wxStaticText(page, wxID_ANY, "Origin: "));
        sizer->Add(new wxStaticText
                       (
                        page,
                        wxID_ANY,
                        wxString::Format("(%d, %d)",
                                         r.x, r.y)
                       ));

        sizer->Add(new wxStaticText(page, wxID_ANY, "Size: "));
        sizer->Add(new wxStaticText
                       (
                        page,
                        wxID_ANY,
                        wxString::Format("(%d, %d)",
                                         r.width, r.height)
                       ));

        const wxRect rc(display.GetClientArea());
        sizer->Add(new wxStaticText(page, wxID_ANY, "Client area: "));
        sizer->Add(new wxStaticText
                       (
                        page,
                        wxID_ANY,
                        wxString::Format("(%d, %d)-(%d, %d)",
                                         rc.x, rc.y, rc.width, rc.height)
                       ));

        sizer->Add(new wxStaticText(page, wxID_ANY, "Resolution: "));
        const wxSize ppi = display.GetPPI();
        sizer->Add(new wxStaticText(page, wxID_ANY,
                                    wxString::Format("%d*%d", ppi.x, ppi.y)));

        sizer->Add(new wxStaticText(page, wxID_ANY, "Depth: "));
        sizer->Add(new wxStaticText(page, wxID_ANY,
                                    wxString::Format("%d", display.GetDepth())));

        sizer->Add(new wxStaticText(page, wxID_ANY, "Name: "));
        sizer->Add(new wxStaticText(page, wxID_ANY, display.GetName()));

        sizer->Add(new wxStaticText(page, wxID_ANY, "Primary: "));
        sizer->Add(new wxStaticText(page, wxID_ANY,
                                    display.IsPrimary() ? "yes" : "no"));

        // add it to another sizer to have borders around it and button below
        wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
        sizerTop->Add(sizer, wxSizerFlags(1).Expand().DoubleBorder());

#if wxUSE_DISPLAY
        wxChoice *choiceModes = new wxChoice(page, Display_ChangeMode);
        const wxArrayVideoModes modes = display.GetModes();
        const size_t countModes = modes.GetCount();
        for ( size_t nMode = 0; nMode < countModes; nMode++ )
        {
            const wxVideoMode& mode = modes[nMode];

            choiceModes->Append(VideoModeToText(mode),
                                new MyVideoModeClientData(mode));
        }
        const wxString currentMode = VideoModeToText(display.GetCurrentMode());
        choiceModes->SetStringSelection(currentMode);

        sizer->Add(new wxStaticText(page, wxID_ANY, "&Modes: "),
                   wxSizerFlags().CentreVertical());
        sizer->Add(choiceModes, wxSizerFlags().Expand());

        sizer->Add(new wxStaticText(page, wxID_ANY, "Current: "));
        sizer->Add(new wxStaticText(page, Display_CurrentMode, currentMode));

        sizerTop->Add(new wxButton(page, Display_ResetMode, "&Reset mode"),
                      wxSizerFlags().Centre().Border());
#endif // wxUSE_DISPLAY

        page->SetSizer(sizerTop);
        page->Layout();

        m_book->AddPage(page, wxString::Format("Display %zu", nDpy + 1));
    }

    SetClientSize(m_book->GetBestSize());
    SetMinSize(GetSize());
}

#if wxUSE_DISPLAY

wxString MyFrame::VideoModeToText(const wxVideoMode& mode)
{
    wxString s;
    s.Printf("%dx%d", mode.w, mode.h);

    if ( mode.bpp )
    {
        s += wxString::Format(", %dbpp", mode.bpp);
    }

    if ( mode.refresh )
    {
        s += wxString::Format(", %dHz", mode.refresh);
    }

    return s;
}

#endif // wxUSE_DISPLAY

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Demo program for wxDisplay class.\n\n(c) 2003-2006 Vadim Zeitlin",
                 "About Display Sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnFromPoint(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_STATUSBAR
    SetStatusText("Press the mouse anywhere...");
#endif // wxUSE_STATUSBAR

    CaptureMouse();
}

void MyFrame::OnFullScreen(wxCommandEvent& WXUNUSED(event))
{
    ShowFullScreen(!IsFullScreen());
}

#if wxUSE_DISPLAY

void MyFrame::OnChangeMode(wxCommandEvent& event)
{
    wxDisplay dpy(m_book->GetSelection());

    // you wouldn't write this in real code, would you?
    if ( !dpy.ChangeMode(((MyVideoModeClientData *)
                wxDynamicCast(event.GetEventObject(), wxChoice)->
                    GetClientObject(event.GetInt()))->mode) )
    {
        wxLogError("Changing video mode failed!");
    }
}

void MyFrame::OnResetMode(wxCommandEvent& WXUNUSED(event))
{
    wxDisplay dpy(m_book->GetSelection());

    dpy.ResetMode();
}

#endif // wxUSE_DISPLAY

void MyFrame::OnLeftClick(wxMouseEvent& event)
{
    if ( HasCapture() )
    {
        // mouse events are in client coords, wxDisplay works in screen ones
        const wxPoint ptScreen = ClientToScreen(event.GetPosition());
        int dpy = wxDisplay::GetFromPoint(ptScreen);
        if ( dpy == wxNOT_FOUND )
        {
            wxLogError("Mouse clicked outside of display!?");
        }

        wxLogStatus(this, "Mouse clicked in display %d (at (%d, %d))",
                    dpy, ptScreen.x, ptScreen.y);

        ReleaseMouse();
    }
}

#if wxUSE_DISPLAY

void MyFrame::OnDisplayChanged(wxDisplayChangedEvent& event)
{
    m_book->DeleteAllPages();
    PopuplateWithDisplayInfo();

    wxLogStatus(this, "Display resolution was changed.");

    event.Skip();
}

#endif // wxUSE_DISPLAY
