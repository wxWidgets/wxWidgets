/////////////////////////////////////////////////////////////////////////////
// Name:        wizard.cpp
// Purpose:     wxWidgets sample demonstrating wxWizard control
// Author:      Vadim Zeitlin
// Modified by: Robert Vazan (sizers)
// Created:     15.08.99
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


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/stattext.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/checkbox.h"
    #include "wx/checklst.h"
    #include "wx/msgdlg.h"
    #include "wx/radiobox.h"
    #include "wx/menu.h"
    #include "wx/sizer.h"
#endif

#include "wx/textctrl.h"
#include "wx/wizard.h"

#include "../sample.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ids for menu items
enum
{
    Wizard_About = wxID_ABOUT,
    Wizard_Quit = wxID_EXIT,
    Wizard_RunModal = wxID_HIGHEST,

    Wizard_RunNoSizer,
    Wizard_RunModeless,

    Wizard_LargeWizard,
    Wizard_ExpandBitmap
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    virtual bool OnInit() override;
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
    void OnRunWizardNoSizer(wxCommandEvent& event);
    void OnRunWizardModeless(wxCommandEvent& event);
    void OnWizardCancel(wxWizardEvent& event);
    void OnWizardFinished(wxWizardEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// our wizard
// ----------------------------------------------------------------------------

class MyWizard : public wxWizard
{
public:
    MyWizard(wxFrame *frame, bool useSizer = true);

    wxWizardPage *GetFirstPage() const { return m_page1; }

private:
    wxWizardPageSimple *m_page1;
};

// ----------------------------------------------------------------------------
// some pages for our wizard
// ----------------------------------------------------------------------------

// This shows how to simply control the validity of the user input by just
// overriding TransferDataFromWindow() - of course, in a real program, the
// check wouldn't be so trivial and the data will be probably saved somewhere
// too.
//
// It also shows how to use a different bitmap for one of the pages.
class wxValidationPage : public wxWizardPageSimple
{
public:
    wxValidationPage(wxWizard *parent) : wxWizardPageSimple(parent)
    {
        m_bitmap = wxBitmapBundle::FromSVGFile("wiztest2.svg", wxSize(116, 260));

        m_checkbox = new wxCheckBox(this, wxID_ANY, "&Check me");

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(
            new wxStaticText(this, wxID_ANY,
                             "You need to check the checkbox\n"
                             "below before going to the next page\n"),
            0,
            wxALL,
            5
        );

        mainSizer->Add(
            m_checkbox,
            0, // No stretching
            wxALL,
            5 // Border
        );
        SetSizerAndFit(mainSizer);
    }

    virtual bool TransferDataFromWindow() override
    {
        if ( !m_checkbox->GetValue() )
        {
            wxMessageBox("Check the checkbox first!", "No way",
                         wxICON_WARNING | wxOK, this);

            return false;
        }

        return true;
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

        m_radio = new wxRadioBox(this, wxID_ANY, "Allow to proceed:",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(choices), choices,
                                 1, wxRA_SPECIFY_COLS);
        m_radio->SetSelection(Both);

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(
            m_radio,
            0, // No stretching
            wxALL,
            5 // Border
        );

        SetSizerAndFit(mainSizer);
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

    wxDECLARE_EVENT_TABLE();
};

// This shows how to dynamically (i.e. during run-time) arrange the page order.
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

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(
            new wxStaticText(this, wxID_ANY, "Try checking the box below and\n"
                                       "then going back and clearing it"),
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );

        m_checkbox = new wxCheckBox(this, wxID_ANY, "&Skip the next page");
        mainSizer->Add(
            m_checkbox,
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );

#if wxUSE_CHECKLISTBOX
        static const wxString aszChoices[] =
        {
            "Zeroth",
            "First",
            "Second",
            "Third",
            "Fourth",
            "Fifth",
            "Sixth",
            "Seventh",
            "Eighth",
            "Nineth"
        };

        m_checklistbox = new wxCheckListBox
                             (
                                this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxSize(100,100),
                                wxArrayString(WXSIZEOF(aszChoices), aszChoices)
                             );

        mainSizer->Add(
            m_checklistbox,
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );
#endif // wxUSE_CHECKLISTBOX

        wxSize textSize = wxSize(150, 200);
        if (((wxFrame*) wxTheApp->GetTopWindow())->GetMenuBar()->IsChecked(Wizard_LargeWizard))
            textSize = wxSize(150, wxGetClientDisplayRect().GetHeight() - 200);


        wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, textSize, wxTE_MULTILINE);
        mainSizer->Add(textCtrl, 0, wxALL|wxEXPAND, 5);

        SetSizerAndFit(mainSizer);
    }

    // implement wxWizardPage functions
    virtual wxWizardPage *GetPrev() const override { return m_prev; }
    virtual wxWizardPage *GetNext() const override
    {
        return m_checkbox->GetValue() ? m_next->GetNext() : m_next;
    }

private:
    wxWizardPage *m_prev,
                 *m_next;

    wxCheckBox *m_checkbox;
#if wxUSE_CHECKLISTBOX
    wxCheckListBox *m_checklistbox;
#endif
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Wizard_Quit,         MyFrame::OnQuit)
    EVT_MENU(Wizard_About,        MyFrame::OnAbout)
    EVT_MENU(Wizard_RunModal,     MyFrame::OnRunWizard)
    EVT_MENU(Wizard_RunNoSizer,   MyFrame::OnRunWizardNoSizer)
    EVT_MENU(Wizard_RunModeless,  MyFrame::OnRunWizardModeless)

    EVT_WIZARD_CANCEL(wxID_ANY,   MyFrame::OnWizardCancel)
    EVT_WIZARD_FINISHED(wxID_ANY, MyFrame::OnWizardFinished)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(wxRadioboxPage, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, wxRadioboxPage::OnWizardPageChanging)
    EVT_WIZARD_CANCEL(wxID_ANY, wxRadioboxPage::OnWizardCancel)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame("wxWizard Sample");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // we're done
    return true;
}

// ----------------------------------------------------------------------------
// MyWizard
// ----------------------------------------------------------------------------

MyWizard::MyWizard(wxFrame *frame, bool useSizer)
{
    SetExtraStyle(wxWIZARD_EX_HELPBUTTON);

    Create(frame,wxID_ANY,"Absolutely Useless Wizard",
                   wxBitmapBundle::FromSVGFile("wiztest.svg", wxSize(116, 260)),
                   wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    SetIcon(wxICON(sample));

    // Allow the bitmap to be expanded to fit the page height
    if (frame->GetMenuBar()->IsChecked(Wizard_ExpandBitmap))
        SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);

    // Enable scrolling adaptation
    if (frame->GetMenuBar()->IsChecked(Wizard_LargeWizard))
        SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

    // a wizard page may be either an object of predefined class
    m_page1 = new wxWizardPageSimple(this);

    /* wxStaticText *text = */ new wxStaticText(m_page1, wxID_ANY,
             "This wizard doesn't help you\nto do anything at all.\n"
             "\n"
             "The next pages will present you\nwith more useless controls.",
             wxPoint(5,5)
        );

    // ... or a derived class
    wxRadioboxPage *page3 = new wxRadioboxPage(this);
    wxValidationPage *page4 = new wxValidationPage(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below, but Chain() is shorter, avoids the risk
    // of an error and can itself be chained, e.g. you could write
    // page3.Chain(page4).Chain(page5) and so on.
    page3->Chain(page4);

    // this page is not a wxWizardPageSimple, so we use SetNext/Prev to insert
    // it into the chain of pages
    wxCheckboxPage *page2 = new wxCheckboxPage(this, m_page1, page3);
    m_page1->SetNext(page2);
    page3->SetPrev(page2);

    if ( useSizer )
    {
        // allow the wizard to size itself around the pages
        GetPageAreaSizer()->Add(m_page1);
    }
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title)
        :wxFrame(nullptr, wxID_ANY, title,
                  wxDefaultPosition, wxSize(250, 150))  // small frame
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Wizard_RunModal, "&Run wizard modal...\tCtrl-R");
    menuFile->Append(Wizard_RunNoSizer, "Run wizard &without sizer...");
    menuFile->Append(Wizard_RunModeless, "Run wizard &modeless...");
    menuFile->AppendSeparator();
    menuFile->Append(Wizard_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuOptions = new wxMenu;
    menuOptions->AppendCheckItem(Wizard_LargeWizard, "&Scroll Wizard Pages");
    menuOptions->AppendCheckItem(Wizard_ExpandBitmap, "Si&ze Bitmap To Page");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Wizard_About, "&About\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuOptions, "&Options");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // also create status bar which we use in OnWizardCancel
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Demo of wxWizard class\n"
                 "(c) 1999, 2000 Vadim Zeitlin",
                 "About wxWizard sample", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnRunWizard(wxCommandEvent& WXUNUSED(event))
{
    MyWizard wizard(this);

    wizard.RunWizard(wizard.GetFirstPage());
}

void MyFrame::OnRunWizardNoSizer(wxCommandEvent& WXUNUSED(event))
{
    MyWizard wizard(this, false);

    wizard.RunWizard(wizard.GetFirstPage());
}

void MyFrame::OnRunWizardModeless(wxCommandEvent& WXUNUSED(event))
{
    MyWizard *wizard = new MyWizard(this);
    wizard->ShowPage(wizard->GetFirstPage());
    wizard->Show(true);
}

void MyFrame::OnWizardFinished(wxWizardEvent& WXUNUSED(event))
{
    wxMessageBox("The wizard finished successfully.", "Wizard notification");
}

void MyFrame::OnWizardCancel(wxWizardEvent& WXUNUSED(event))
{
    wxMessageBox("The wizard was cancelled.", "Wizard notification");
}
