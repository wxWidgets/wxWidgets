/////////////////////////////////////////////////////////////////////////////
// Name:        wiztest.cpp
// Purpose:     wxWindows sample demonstrating wxWizard control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.99
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

#ifdef __GNUG__
    #pragma implementation "wiztest.cpp"
    #pragma interface "wiztest.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wizard.h"

#ifndef __WXMSW__
    #include "wiztest.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ids for menu items
enum
{
    Wizard_Quit = 100,
    Wizard_Run,
    Wizard_About = 1000
};

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------

#ifdef __WXMSW__
    #define BMP_WIZARD_1 wxBitmap("wiztest.bmp", wxBITMAP_TYPE_BMP)
    #define BMP_WIZARD_2 wxBitmap("wiztest2.bmp", wxBITMAP_TYPE_BMP)
#else
    #define BMP_WIZARD_1 wxBitmap(wizimage)
    #define BMP_WIZARD_2 wxBitmap(wizimage)
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnRunWizard(wxCommandEvent& event);
    void OnWizardCancel(wxWizardEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// some pages for our wizard
// ----------------------------------------------------------------------------

// this shows how to simply control the validity of the user input by just
// overriding TransferDataFromWindow() - of course, in a real program, the
// check wouldn't be so trivial and the data will be probably saved somewhere
// too
//
// it also shows how to use a different bitmap for one of the pages
class wxValidationPage : public wxWizardPageSimple
{
public:
    wxValidationPage(wxWizard *parent) : wxWizardPageSimple(parent)
    {
        m_bitmap = BMP_WIZARD_2;

        m_checkbox = new wxCheckBox(this, -1, "&Check me");
    }

    virtual bool TransferDataFromWindow()
    {
        if ( !m_checkbox->GetValue() )
        {
            wxMessageBox("Check the checkbox first!", "No way",
                         wxICON_WARNING | wxOK, this);

            return FALSE;
        }

        return TRUE;
    }

private:
    wxCheckBox *m_checkbox;
};

// This is a more complicated example of validity checking: using events we may
// allow to return to the previous page, but not to proceed. It also
// demonstrates how to intercept [Cancel] button press.
class wxRadioboxPage : public wxWizardPageSimple
{
public:
    // directions in which we allow the user to proceed from this page
    enum
    {
        Forward, Backward, Both, Neither
    };

    wxRadioboxPage(wxWizard *parent) : wxWizardPageSimple(parent)
    {
        // should correspond to the enum above
        //        static wxString choices[] = { "forward", "backward", "both", "neither" };
        // The above syntax can cause an internal compiler error with gcc.
        wxString choices[4];
        choices[0] = "forward";
        choices[1] = "backward";
        choices[2] = "both";
        choices[3] = "neither";

        m_radio = new wxRadioBox(this, -1, "Allow to proceed:",
                                 wxPoint(5, 5), wxDefaultSize,
                                 WXSIZEOF(choices), choices,
                                 1, wxRA_SPECIFY_COLS);
        m_radio->SetSelection(Both);
    }

    // wizard event handlers
    void OnWizardCancel(wxWizardEvent& event)
    {
        if ( wxMessageBox("Do you really want to cancel?", "Question",
                          wxICON_QUESTION | wxYES_NO, this) != wxYES )
        {
            // not confirmed
            event.Veto();
        }
    }

    void OnWizardPageChanging(wxWizardEvent& event)
    {
        int sel = m_radio->GetSelection();

        if ( sel == Both )
            return;

        if ( event.GetDirection() && sel == Forward )
            return;

        if ( !event.GetDirection() && sel == Backward )
            return;

        wxMessageBox("You can't go there", "Not allowed",
                     wxICON_WARNING | wxOK, this);

        event.Veto();
    }

private:
    wxRadioBox *m_radio;

    DECLARE_EVENT_TABLE()
};

// this shows how to dynamically (i.e. during run-time) arrange the page order
class wxCheckboxPage : public wxWizardPage
{
public:
    wxCheckboxPage(wxWizard *parent,
                   wxWizardPage *prev,
                   wxWizardPage *next)
        : wxWizardPage(parent)
    {
        m_prev = prev;
        m_next = next;

        (void)new wxStaticText(this, -1, "Try checking the box below and\n"
                                         "then going back and clearing it");

        m_checkbox = new wxCheckBox(this, -1, "&Skip the next page",
                                    wxPoint(5, 30));
    }

    // implement wxWizardPage functions
    virtual wxWizardPage *GetPrev() const { return m_prev; }
    virtual wxWizardPage *GetNext() const
    {
        return m_checkbox->GetValue() ? m_next->GetNext() : m_next;
    }

private:
    wxWizardPage *m_prev,
                 *m_next;

    wxCheckBox *m_checkbox;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Wizard_Quit,  MyFrame::OnQuit)
    EVT_MENU(Wizard_About, MyFrame::OnAbout)
    EVT_MENU(Wizard_Run,   MyFrame::OnRunWizard)

    EVT_WIZARD_CANCEL(-1, MyFrame::OnWizardCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxRadioboxPage, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(-1, wxRadioboxPage::OnWizardPageChanging)
    EVT_WIZARD_CANCEL(-1, wxRadioboxPage::OnWizardCancel)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("wxWizard Sample");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // we're done
    return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title)
       : wxFrame((wxFrame *)NULL, -1, title,
                  wxDefaultPosition, wxSize(250, 150))  // small frame
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Wizard_Run, "&Run wizard...\tCtrl-R");
    menuFile->AppendSeparator();
    menuFile->Append(Wizard_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Wizard_About, "&About...\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // also create status bar which we use in OnWizardCancel
    CreateStatusBar();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Demo of wxWizard class\n"
                 "© 1999, 2000 Vadim Zeitlin",
                 "About wxWizard sample", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnRunWizard(wxCommandEvent& WXUNUSED(event))
{
    wxWizard *wizard = wxWizard::Create(this, -1,
                                        "Absolutely Useless Wizard",
                                        BMP_WIZARD_1);

    // a wizard page may be either an object of predefined class
    wxWizardPageSimple *page1 = new wxWizardPageSimple(wizard);
    (void)new wxStaticText(page1, -1,
                           "This wizard doesn't help you to do anything at "
                           "all.\n"
                           "\n"
                           "The next pages will present you with more useless "
                           "controls.");

    // ... or a derived class
    wxRadioboxPage *page3 = new wxRadioboxPage(wizard);
    wxValidationPage *page4 = new wxValidationPage(wizard);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page3, page4);

    // this page is not a wxWizardPageSimple, so we use SetNext/Prev to insert
    // it into the chain of pages
    wxCheckboxPage *page2 = new wxCheckboxPage(wizard, page1, page3);
    page1->SetNext(page2);
    page3->SetPrev(page2);

    if ( wizard->RunWizard(page1) )
    {
        wxMessageBox("The wizard successfully completed", "That's all",
                     wxICON_INFORMATION | wxOK);
    }

    wizard->Destroy();
}

void MyFrame::OnWizardCancel(wxWizardEvent& WXUNUSED(event))
{
    wxLogStatus(this, "The wizard was cancelled.");
}
