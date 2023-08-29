/////////////////////////////////////////////////////////////////////////////
// Name:        splash.cpp
// Purpose:     wxSplashScreen sample
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     04/08/2004
// Copyright:   (c) Wlodzimierz Skiba
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


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/splash.h"
#include "wx/mediactrl.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// for smartphone, pda and other small screens use resized embedded image
// instead of full colour png dedicated to desktops
#include "mobile.xpm"

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

    void DecorateSplashScreen(wxBitmap& bmp);
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    bool m_isPda;

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    Minimal_Run = wxID_HIGHEST
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
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

    wxImage::AddHandler(new wxPNGHandler);

    // create the main application window
    MyFrame *frame = new MyFrame("wxSplashScreen sample application");

    wxBitmap bitmap;

    if (frame->m_isPda)
        bitmap = wxBitmap(mobile_xpm);

    bool ok = frame->m_isPda
            ? bitmap.IsOk()
            : bitmap.LoadFile("splash.png", wxBITMAP_TYPE_PNG);

    if (ok)
    {
        // we can even draw dynamic artwork onto our splashscreen
        DecorateSplashScreen(bitmap);

        // show the splashscreen
        new wxSplashScreen(bitmap,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
            6000, frame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }

#if !defined(__WXGTK__)
    // we don't need it at least on wxGTK with GTK+ 2.12.9
    wxYield();
#endif

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// Draws artwork onto our splashscreen at runtime
void MyApp::DecorateSplashScreen(wxBitmap& bmp)
{
    // use a memory DC to draw directly onto the bitmap
    wxMemoryDC memDc(bmp);

    // draw an orange box (with black outline) at the bottom of the splashscreen.
    // this box will be 10% of the height of the bitmap, and be at the bottom.
    const wxRect bannerRect(wxPoint(0, (bmp.GetHeight() / 10)*9),
                            wxPoint(bmp.GetWidth(), bmp.GetHeight()));
    wxDCBrushChanger bc(memDc, wxBrush(wxColour(255, 102, 0)));
    memDc.DrawRectangle(bannerRect);
    memDc.DrawLine(bannerRect.GetTopLeft(), bannerRect.GetTopRight());

    // dynamically get the wxWidgets version to display
    wxString description = wxString::Format("wxWidgets %s", wxVERSION_NUM_DOT_STRING);
    // create a copyright notice that uses the year that this file was compiled
    wxString year(__DATE__);
    wxString copyrightLabel = wxString::Format("%s%s wxWidgets. %s",
        wxString::FromUTF8("\xc2\xa9"), year.Mid(year.length() - 4),
        "All rights reserved.");

    // draw the (white) labels inside of our orange box (at the bottom of the splashscreen)
    memDc.SetTextForeground(*wxWHITE);
    // draw the "wxWidget" label on the left side, vertically centered.
    // note that we deflate the banner rect a little bit horizontally
    // so that the text has some padding to its left.
    memDc.DrawLabel(description, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL|wxALIGN_LEFT);

    // draw the copyright label on the right side
    memDc.SetFont(wxFontInfo(8));
    memDc.DrawLabel(copyrightLabel, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT);
}


// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title)
{
    m_isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about frame");

    menuFile->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxBitmap bitmap;

    if (m_isPda) bitmap = wxBitmap(mobile_xpm);

    bool ok = m_isPda
            ? bitmap.IsOk()
            : bitmap.LoadFile("splash.png", wxBITMAP_TYPE_PNG);

    if (ok)
    {
        wxImage image = bitmap.ConvertToImage();

        // do not scale on already small screens
        if (!m_isPda)
            image.Rescale( bitmap.GetWidth()/2, bitmap.GetHeight()/2 );

        bitmap = wxBitmap(image);
        wxSplashScreen *splash = new wxSplashScreen(bitmap,
            wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_NO_TIMEOUT,
            0, this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);

        wxWindow *win = splash->GetSplashWindow();
#if wxUSE_MEDIACTRL
        wxMediaCtrl *media = new wxMediaCtrl( win, wxID_EXIT, "press.mpg", wxPoint(2,2));
        media->Play();
#else
        wxStaticText *text = new wxStaticText( win,
                                               wxID_EXIT,
                                               "click somewhere\non this image",
                                               wxPoint(m_isPda ? 0 : 13,
                                                       m_isPda ? 0 : 11)
                                             );
        text->SetBackgroundColour(*wxWHITE);
        text->SetForegroundColour(*wxBLACK);
        wxFont font = text->GetFont();
        font.SetFractionalPointSize(2.0*font.GetFractionalPointSize()/3.0);
        text->SetFont(font);
#endif
    }
}
