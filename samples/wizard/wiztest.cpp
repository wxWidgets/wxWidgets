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
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// some pages for our wizard
// ----------------------------------------------------------------------------

// this shows how to simply control the validity of the user input by just
// overriding TransferDataFromWindow() - of course, in a real program, the
// check wouldn't be so trivial and the data will be probably saved somewhere
// too
class wxValidationPage : public wxWizardPageSimple
{
public:
    wxValidationPage(wxWizard *parent) : wxWizardPageSimple(parent)
    {
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
        static wxString choices[] = { "forward", "backward", "both", "neither" };

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

BEGIN_EVENT_TABLE(wxRadioboxPage, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(-1, wxRadioboxPage::OnWizardPageChanging)
    EVT_WIZARD_CANCEL(-1, wxRadioboxPage::OnWizardCancel)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
#ifdef __WXMSW__
    wxBitmap bmpWizard("wiztest.bmp", wxBITMAP_TYPE_BMP);
#else
    wxBitmap bmpWizard(wizimage);
#endif

    wxWizard *wizard = wxWizard::Create(NULL, -1,
                                        "Absolutely Useless Wizard",
                                        bmpWizard);

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

    // we're done
    return FALSE;
}
