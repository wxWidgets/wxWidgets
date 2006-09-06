/////////////////////////////////////////////////////////////////////////////
// Name:        wizard.cpp
// Purpose:     wxWidgets sample demonstrating wxWizard control
// Author:      Vadim Zeitlin
// Modified by: Robert Vazan (sizers)
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

#include "wx/wizard.h"

#include "wiztest.xpm"
#include "wiztest2.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ids for menu items
enum
{
    Wizard_Quit = wxID_EXIT,
    Wizard_RunModal = wxID_HIGHEST,
    Wizard_RunNoSizer,
    Wizard_RunModeless,
    Wizard_About = wxID_ABOUT
};

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
    void OnRunWizardNoSizer(wxCommandEvent& event);
    void OnRunWizardModeless(wxCommandEvent& event);
    void OnWizardCancel(wxWizardEvent& event);
    void OnWizardFinished(wxWizardEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
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
        m_bitmap = wxBitmap(wiztest2_xpm);

        m_checkbox = new wxCheckBox(this, wxID_ANY, _T("&Check me"));

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(
            new wxStaticText(this, wxID_ANY,
                             _T("You need to check the checkbox\n")
                             _T("below before going to the next page\n")),
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
        SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    virtual bool TransferDataFromWindow()
    {
        if ( !m_checkbox->GetValue() )
        {
            wxMessageBox(_T("Check the checkbox first!"), _T("No way"),
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
        choices[0] = _T("forward");
        choices[1] = _T("backward");
        choices[2] = _T("both");
        choices[3] = _T("neither");

        m_radio = new wxRadioBox(this, wxID_ANY, _T("Allow to proceed:"),
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

        SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    // wizard event handlers
    void OnWizardCancel(wxWizardEvent& event)
    {
        if ( wxMessageBox(_T("Do you really want to cancel?"), _T("Question"),
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

        wxMessageBox(_T("You can't go there"), _T("Not allowed"),
                     wxICON_WARNING | wxOK, this);

        event.Veto();
    }

private:
    wxRadioBox *m_radio;

    DECLARE_EVENT_TABLE()
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
            new wxStaticText(this, wxID_ANY, _T("Try checking the box below and\n")
                                       _T("then going back and clearing it")),
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );

        m_checkbox = new wxCheckBox(this, wxID_ANY, _T("&Skip the next page"));
        mainSizer->Add(
            m_checkbox,
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );

#if wxUSE_CHECKLISTBOX
        static const wxChar *aszChoices[] =
        {
            _T("Zeroth"),
            _T("First"),
            _T("Second"),
            _T("Third"),
            _T("Fourth"),
            _T("Fifth"),
            _T("Sixth"),
            _T("Seventh"),
            _T("Eighth"),
            _T("Nineth")
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

        SetSizer(mainSizer);
        mainSizer->Fit(this);
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

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Wizard_Quit,         MyFrame::OnQuit)
    EVT_MENU(Wizard_About,        MyFrame::OnAbout)
    EVT_MENU(Wizard_RunModal,     MyFrame::OnRunWizard)
    EVT_MENU(Wizard_RunNoSizer,   MyFrame::OnRunWizardNoSizer)
    EVT_MENU(Wizard_RunModeless,  MyFrame::OnRunWizardModeless)

    EVT_WIZARD_CANCEL(wxID_ANY,   MyFrame::OnWizardCancel)
    EVT_WIZARD_FINISHED(wxID_ANY, MyFrame::OnWizardFinished)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxRadioboxPage, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, wxRadioboxPage::OnWizardPageChanging)
    EVT_WIZARD_CANCEL(wxID_ANY, wxRadioboxPage::OnWizardCancel)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("wxWizard Sample"));

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
        : wxWizard(frame,wxID_ANY,_T("Absolutely Useless Wizard"),
                   wxBitmap(wiztest_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    // a wizard page may be either an object of predefined class
    m_page1 = new wxWizardPageSimple(this);

    /* wxStaticText *text = */ new wxStaticText(m_page1, wxID_ANY,
             _T("This wizard doesn't help you\nto do anything at all.\n")
             _T("\n")
             _T("The next pages will present you\nwith more useless controls."),
             wxPoint(5,5)
        );

    // ... or a derived class
    wxRadioboxPage *page3 = new wxRadioboxPage(this);
    wxValidationPage *page4 = new wxValidationPage(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page3, page4);

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
        :wxFrame((wxFrame *)NULL, wxID_ANY, title,
                  wxDefaultPosition, wxSize(250, 150))  // small frame
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Wizard_RunModal, _T("&Run wizard modal...\tCtrl-R"));
    menuFile->Append(Wizard_RunNoSizer, _T("Run wizard &without sizer..."));
    menuFile->Append(Wizard_RunModeless, _T("Run wizard &modeless..."));
    menuFile->AppendSeparator();
    menuFile->Append(Wizard_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Wizard_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

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
    wxMessageBox(_T("Demo of wxWizard class\n")
                 _T("(c) 1999, 2000 Vadim Zeitlin"),
                 _T("About wxWizard sample"), wxOK | wxICON_INFORMATION, this);
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
    wxMessageBox(wxT("The wizard finished successfully."), wxT("Wizard notification"));
}

void MyFrame::OnWizardCancel(wxWizardEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("The wizard was cancelled."), wxT("Wizard notification"));
}
