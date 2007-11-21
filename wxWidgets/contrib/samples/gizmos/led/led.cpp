/////////////////////////////////////////////////////////////////////////////
// Name:        led.cpp
// Purpose:     LED sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/gizmos/ledctrl.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/numdlg.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyPanel : public wxPanel
{
public:
    MyPanel(wxFrame *frame);

    void OnIncrement();
    void OnDecrement();
    void OnSmallIncrement();
    void OnSmallDecrement();
    void OnSetValue();
    void OnAlignLeft();
    void OnAlignCenter();
    void OnAlignRight();
    void OnDrawFaded();

private:
    wxLEDNumberCtrl    *m_led;
    wxBoxSizer         *m_sizer;
};

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

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnIncrement(wxCommandEvent& event);
    void OnDecrement(wxCommandEvent& event);
    void OnSmallIncrement(wxCommandEvent& event);
    void OnSmallDecrement(wxCommandEvent& event);
    void OnSetValue(wxCommandEvent& event);
    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignCenter(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);
    void OnDrawFaded(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    MyPanel  *m_panel;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    LED_Quit = wxID_EXIT,

    LED_Edit_Increment = wxID_HIGHEST + 1,
    LED_Edit_Decrement,
    LED_Edit_Small_Increment,
    LED_Edit_Small_Decrement,
    LED_Edit_SetValue,
    LED_Edit_AlignLeft,
    LED_Edit_AlignCenter,
    LED_Edit_AlignRight,
    LED_Edit_DrawFaded,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    LED_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(LED_Quit,  MyFrame::OnQuit)
    EVT_MENU(LED_Edit_Increment, MyFrame::OnIncrement)
    EVT_MENU(LED_Edit_Decrement, MyFrame::OnDecrement)
    EVT_MENU(LED_Edit_Small_Increment, MyFrame::OnSmallIncrement)
    EVT_MENU(LED_Edit_Small_Decrement, MyFrame::OnSmallDecrement)
    EVT_MENU(LED_Edit_SetValue, MyFrame::OnSetValue)
    EVT_MENU(LED_Edit_AlignLeft, MyFrame::OnAlignLeft)
    EVT_MENU(LED_Edit_AlignCenter, MyFrame::OnAlignCenter)
    EVT_MENU(LED_Edit_AlignRight, MyFrame::OnAlignRight)
    EVT_MENU(LED_Edit_DrawFaded, MyFrame::OnDrawFaded)
    EVT_MENU(LED_About, MyFrame::OnAbout)
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

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("LED App"),
                                 wxDefaultPosition, wxSize(450, 120));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

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
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(LED_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(LED_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *editMenu = new wxMenu;
    editMenu->Append(LED_Edit_Increment, _T("&Increment LED (1)\tCtrl-I"));
    editMenu->Append(LED_Edit_Small_Increment, _T("&Increment LED (0.01)\tAlt-I"));
    editMenu->Append(LED_Edit_Decrement, _T("&Decrement LED (1)\tCtrl-D"));
    editMenu->Append(LED_Edit_Small_Decrement, _T("&Decrement LED (0.01)\tAlt-D"));
    editMenu->Append(LED_Edit_SetValue, _T("&Set LED Value...\tCtrl-S"));
    editMenu->AppendSeparator();
    editMenu->AppendRadioItem(LED_Edit_AlignLeft, _T("Align &Left"));
    editMenu->AppendRadioItem(LED_Edit_AlignCenter, _T("Align &Center"));
    editMenu->AppendRadioItem(LED_Edit_AlignRight, _T("Align &Right"));
    editMenu->AppendSeparator();
    editMenu->AppendCheckItem(LED_Edit_DrawFaded, _T("Draw &Faded\tCtrl-F"));

    editMenu->Check(LED_Edit_DrawFaded, true);

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(editMenu, _T("&Edit"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    m_panel = new MyPanel(this);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnIncrement(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnIncrement();
}

void MyFrame::OnDecrement(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnDecrement();
}

void MyFrame::OnSmallIncrement(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnSmallIncrement();
}

void MyFrame::OnSmallDecrement(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnSmallDecrement();
}

void MyFrame::OnSetValue(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnSetValue();
}

void MyFrame::OnAlignLeft(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnAlignLeft();
}

void MyFrame::OnAlignCenter(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnAlignCenter();
}

void MyFrame::OnAlignRight(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnAlignRight();
}

void MyFrame::OnDrawFaded(wxCommandEvent& WXUNUSED(event))
{
    m_panel->OnDrawFaded();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the LED sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About LED"), wxOK | wxICON_INFORMATION, this);
}

// --------------------------------------------------------------------------
// MyPanel
// --------------------------------------------------------------------------

MyPanel::MyPanel(wxFrame *frame)
                : wxPanel(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
    m_led = new wxLEDNumberCtrl(this, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize,
                                wxLED_ALIGN_LEFT|wxLED_DRAW_FADED|wxFULL_REPAINT_ON_RESIZE);

    m_led->SetValue(_T("01.23 7-8-9"));

    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(m_led, 1, wxEXPAND|wxALL, 10);
    m_sizer->Fit(this);

    SetSizer(m_sizer);
    SetAutoLayout(true);
}

void MyPanel::OnIncrement()
{
    wxString strValue = m_led->GetValue();

    double dValue;
    strValue.ToDouble(&dValue);
    dValue += 1.0;
    m_led->SetValue(wxString::Format(_T("%.2f"), dValue));
}

void MyPanel::OnDecrement()
{
    wxString strValue = m_led->GetValue();

    double dValue;
    strValue.ToDouble(&dValue);
    dValue -= 1.0;
    m_led->SetValue(wxString::Format(_T("%.2f"), dValue));
}

void MyPanel::OnSmallIncrement()
{
    wxString strValue = m_led->GetValue();

    double dValue;
    strValue.ToDouble(&dValue);
    dValue += 0.01;
    m_led->SetValue(wxString::Format(_T("%.2f"), dValue));
}

void MyPanel::OnSmallDecrement()
{
    wxString strValue = m_led->GetValue();

    double dValue;
    strValue.ToDouble(&dValue);
    dValue -= 0.01;
    m_led->SetValue(wxString::Format(_T("%.2f"), dValue));
}

void MyPanel::OnSetValue()
{
    wxString strValue = m_led->GetValue();

    strValue = ::wxGetTextFromUser(_T("Please enter a number for LED display"), _T("Please enter a number"), strValue, this);

    if (strValue != _T(""))
        m_led->SetValue(strValue);
}

void MyPanel::OnAlignLeft()
{
    m_led->SetAlignment(wxLED_ALIGN_LEFT);
}

void MyPanel::OnAlignCenter()
{
    m_led->SetAlignment(wxLED_ALIGN_CENTER);
}

void MyPanel::OnAlignRight()
{
    m_led->SetAlignment(wxLED_ALIGN_RIGHT);
}

void MyPanel::OnDrawFaded()
{
    m_led->SetDrawFaded(!(m_led->GetDrawFaded()));
}
