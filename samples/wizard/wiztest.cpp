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
class wxCheckboxPage : public wxPanel
{
public:
    wxCheckboxPage(wxWizard *parent) : wxPanel(parent)
    {
        m_checkbox = new wxCheckBox(this, -1, "Check me", wxPoint(20, 20));
    }

    virtual bool TransferDataFromWindow()
    {
        if ( m_checkbox->GetValue() )
        {
            wxMessageBox("Clear the checkbox first", "No way",
                         wxICON_WARNING | wxOK, this);

            return FALSE;
        }

        return TRUE;
    }

private:
    wxCheckBox *m_checkbox;
};

// ============================================================================
// implementation
// ============================================================================

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

    wxPanel *panel = new wxPanel(wizard);
    (void)new wxStaticText(panel, -1,
                           "This wizard doesn't help you to do anything at "
                           "all.\n"
                           "\n"
                           "The next pages will present you with more useless "
                           "controls.");
    wizard->AddPage(panel);

    wizard->AddPage(new wxCheckboxPage(wizard));

    if ( wizard->RunWizard() )
    {
        wxMessageBox("The wizard successfully completed", "That's all",
                     wxICON_INFORMATION | wxOK);
    }

    wizard->Destroy();

    // we're done
    return FALSE;
}
