/////////////////////////////////////////////////////////////////////////////
// Name:        validate.cpp
// Purpose:     wxWidgets validator sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// See online help for an overview of validators. In general, a
// validator transfers data between a control and a variable.
// It may also test for validity of a string transferred to or
// from a text control. All validators transfer data, but not
// all test validity, so don't be confused by the name.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif // __BORLANDC__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "validate.h"

#include "wx/sizer.h"
#include "wx/valgen.h"
#include "wx/valtext.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// Global data
// ----------------------------------------------------------------------------

MyData g_data;

wxString g_listbox_choices[] =
    {wxT("one"),  wxT("two"),  wxT("three")};

wxString g_combobox_choices[] =
    {wxT("yes"), wxT("no"), wxT("maybe")};

wxString g_radiobox_choices[] =
    {wxT("green"), wxT("yellow"), wxT("red")};

// ----------------------------------------------------------------------------
// MyData
// ----------------------------------------------------------------------------

MyData::MyData()
{
    // This string will be passed to an alpha-only validator, which
    // will complain because spaces aren't alpha. Note that validation
    // is performed only when 'OK' is pressed. It would be nice to
    // enhance this so that validation would occur when the text
    // control loses focus.
    m_string = wxT("Spaces are invalid here");
    m_listbox_choices.Add(0);
}

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create and display the main frame window.
    MyFrame *frame = new MyFrame((wxFrame *) NULL, wxT("Validator Test"),
                                 50, 50, 300, 250);
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(VALIDATE_TEST_DIALOG, MyFrame::OnTestDialog)
    EVT_MENU(VALIDATE_TOGGLE_BELL, MyFrame::OnToggleBell)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString&title, int x, int y, int w, int h)
       : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h)),
         m_silent(true)
{
    SetIcon(wxICON(sample));

    // Create a listbox to display the validated data.
    m_listbox = new wxListBox(this, wxID_ANY);
    m_listbox->Append(wxString(_T("Try 'File|Test' to see how validators work.")));

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(VALIDATE_TEST_DIALOG, wxT("&Test"), wxT("Demonstrate validators"));
    file_menu->AppendCheckItem(VALIDATE_TOGGLE_BELL, wxT("&Bell on error"), wxT("Toggle bell on error"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, wxT("E&xit"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("File"));
    SetMenuBar(menu_bar);

    // All validators share a common (static) flag that controls
    // whether they beep on error. Here we turn it off:
    wxValidator::SetBellOnError(m_silent);
    file_menu->Check(VALIDATE_TOGGLE_BELL, !wxValidator::IsSilent());

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event))
{
    // The validators defined in the dialog implementation bind controls
    // and variables together. Values are transferred between them behind
    // the scenes, so here we don't have to query the controls for their
    // values.
    MyDialog dialog(this, wxT("Validator demonstration"));

    // When the dialog is displayed, validators automatically transfer
    // data from variables to their corresponding controls.
    if ( dialog.ShowModal() == wxID_OK )
    {
        // 'OK' was pressed, so controls that have validators are
        // automatically transferred to the variables we specified
        // when we created the validators.
        m_listbox->Clear();
        m_listbox->Append(wxString(_T("string: ")) + g_data.m_string);
        for(unsigned int i = 0; i < g_data.m_listbox_choices.GetCount(); ++i)
        {
            int j = g_data.m_listbox_choices[i];
            m_listbox->Append(wxString(_T("listbox choice(s): ")) + g_listbox_choices[j]);
        }

        wxString checkbox_state(g_data.m_checkbox_state ? _T("checked") : _T("unchecked"));
        m_listbox->Append(wxString(_T("checkbox: ")) + checkbox_state);
        m_listbox->Append(wxString(_T("combobox: ")) + g_data.m_combobox_choice);
        m_listbox->Append(wxString(_T("radiobox: ")) + g_radiobox_choices[g_data.m_radiobox_choice]);
    }
}

void MyFrame::OnToggleBell(wxCommandEvent& event)
{
    m_silent = !m_silent;
    wxValidator::SetBellOnError(m_silent);
    event.Skip();
}

// ----------------------------------------------------------------------------
// MyDialog
// ----------------------------------------------------------------------------

MyDialog::MyDialog( wxWindow *parent, const wxString& title,
                    const wxPoint& pos, const wxSize& size, const long WXUNUSED(style) ) :
    wxDialog(parent, VALIDATE_DIALOG_ID, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    // Sizers automatically ensure a workable layout.
    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *flexgridsizer = new wxFlexGridSizer(2, 2, 5, 5);

    // Create and add controls to sizers. Note that a member variable
    // of g_data is bound to each control upon construction. There is
    // currently no easy way to substitute a different validator or a
    // different transfer variable after a control has been constructed.

    // Pointers to some of these controls are saved in member variables
    // so that we can use them elsewhere, like this one.
    text = new wxTextCtrl(this, VALIDATE_TEXT, wxEmptyString,
        wxPoint(10, 10), wxSize(120, wxDefaultCoord), 0,
        wxTextValidator(wxFILTER_ALPHA, &g_data.m_string));
    flexgridsizer->Add(text);

    // This wxCheckBox* doesn't need to be assigned to any pointer
    // because we don't use it elsewhere--it can be anonymous.
    // We don't need any such pointer to query its state, which
    // can be gotten directly from g_data.
    flexgridsizer->Add(new wxCheckBox(this, VALIDATE_CHECK, wxT("Sample checkbox"),
        wxPoint(130, 10), wxSize(120, wxDefaultCoord), 0,
        wxGenericValidator(&g_data.m_checkbox_state)));

    flexgridsizer->Add(new wxListBox((wxWindow*)this, VALIDATE_LIST,
        wxPoint(10, 30), wxSize(120, wxDefaultCoord),
        3, g_listbox_choices, wxLB_MULTIPLE,
        wxGenericValidator(&g_data.m_listbox_choices)));

    combobox = new wxComboBox((wxWindow*)this, VALIDATE_COMBO, wxEmptyString,
        wxPoint(130, 30), wxSize(120, wxDefaultCoord),
        3, g_combobox_choices, 0L,
        wxGenericValidator(&g_data.m_combobox_choice));
    flexgridsizer->Add(combobox);

    mainsizer->Add(flexgridsizer, 1, wxGROW | wxALL, 10);

    mainsizer->Add(new wxRadioBox((wxWindow*)this, VALIDATE_RADIO, wxT("Pick a color"),
        wxPoint(10, 100), wxDefaultSize,
        3, g_radiobox_choices, 1, wxRA_SPECIFY_ROWS,
        wxGenericValidator(&g_data.m_radiobox_choice)),
        0, wxGROW | wxALL, 10);

    wxGridSizer *gridsizer = new wxGridSizer(2, 2, 5, 5);

    wxButton *ok_button = new wxButton(this, wxID_OK, wxT("OK"), wxPoint(250, 70), wxSize(80, 30));
    ok_button->SetDefault();
    gridsizer->Add(ok_button);
    gridsizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxPoint(250, 100), wxSize(80, 30)));

    mainsizer->Add(gridsizer, 0, wxGROW | wxALL, 10);

    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);
}

bool MyDialog::TransferDataToWindow()
{
    bool r = wxDialog::TransferDataToWindow();
    // These function calls have to be made here, after the
    // dialog has been created.
    text->SetFocus();
    combobox->SetSelection(0);
    return r;
}

