/////////////////////////////////////////////////////////////////////////////
// Name:        validate.cpp
// Purpose:     wxWidgets validator sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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
#include "wx/valnum.h"

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
    {wxT("yes"), wxT("no (doesn't validate)"), wxT("maybe (doesn't validate)")};

wxString g_radiobox_choices[] =
    {wxT("green"), wxT("yellow"), wxT("red")};

// ----------------------------------------------------------------------------
// MyData
// ----------------------------------------------------------------------------

MyData::MyData()
{
    // This string will be passed to an alpha-only validator, which
    // will complain because spaces aren't alpha. Note that validation
    // is performed only when 'OK' is pressed.
    m_string = wxT("Spaces are invalid here");
    m_string2 = "Valid text";
    m_listbox_choices.Add(0);
    m_intValue = 0;
    m_doubleValue = 12354.31;
}

// ----------------------------------------------------------------------------
// MyComboBoxValidator
// ----------------------------------------------------------------------------

bool MyComboBoxValidator::Validate(wxWindow *WXUNUSED(parent))
{
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxComboBox)));

    wxComboBox* cb = (wxComboBox*)GetWindow();
    if (cb->GetValue() == g_combobox_choices[1] ||
        cb->GetValue() == g_combobox_choices[2])
    {
        // we accept any string != g_combobox_choices[1|2] !

        wxLogError("Invalid combo box text!");
        return false;
    }

    if (m_var)
        *m_var = cb->GetValue();

    return true;
}

bool MyComboBoxValidator::TransferToWindow()
{
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxComboBox)));

    if ( m_var )
    {
        wxComboBox* cb = (wxComboBox*)GetWindow();
        if ( !cb )
            return false;

        cb->SetValue(*m_var);
    }

    return true;
}

bool MyComboBoxValidator::TransferFromWindow()
{
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxComboBox)));

    if ( m_var )
    {
        wxComboBox* cb = (wxComboBox*)GetWindow();
        if ( !cb )
            return false;

        *m_var = cb->GetValue();
    }

    return true;
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
    m_listbox->Append(wxString(wxT("Try 'File|Test' to see how validators work.")));

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(VALIDATE_TEST_DIALOG, wxT("&Test dialog..."), wxT("Demonstrate validators"));
    file_menu->AppendCheckItem(VALIDATE_TOGGLE_BELL, wxT("&Bell on error"), wxT("Toggle bell on error"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, wxT("E&xit"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));
    SetMenuBar(menu_bar);

    // All validators share a common (static) flag that controls
    // whether they beep on error. Here we turn it off:
    wxValidator::SuppressBellOnError(m_silent);
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
        m_listbox->Append(wxString(wxT("string: ")) + g_data.m_string);
        m_listbox->Append(wxString(wxT("string #2: ")) + g_data.m_string2);

        for(unsigned int i = 0; i < g_data.m_listbox_choices.GetCount(); ++i)
        {
            int j = g_data.m_listbox_choices[i];
            m_listbox->Append(wxString(wxT("listbox choice(s): ")) + g_listbox_choices[j]);
        }

        wxString checkbox_state(g_data.m_checkbox_state ? wxT("checked") : wxT("unchecked"));
        m_listbox->Append(wxString(wxT("checkbox: ")) + checkbox_state);
        m_listbox->Append(wxString(wxT("combobox: ")) + g_data.m_combobox_choice);
        m_listbox->Append(wxString(wxT("radiobox: ")) + g_radiobox_choices[g_data.m_radiobox_choice]);

        m_listbox->Append(wxString::Format("integer value: %d", g_data.m_intValue));
        m_listbox->Append(wxString::Format("double value: %.3f", g_data.m_doubleValue));
    }
}

void MyFrame::OnToggleBell(wxCommandEvent& event)
{
    m_silent = !m_silent;
    wxValidator::SuppressBellOnError(m_silent);
    event.Skip();
}

// ----------------------------------------------------------------------------
// MyDialog
// ----------------------------------------------------------------------------

MyDialog::MyDialog( wxWindow *parent, const wxString& title,
                    const wxPoint& pos, const wxSize& size, const long WXUNUSED(style) ) :
    wxDialog(parent, VALIDATE_DIALOG_ID, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    // setup the flex grid sizer
    // -------------------------

    wxFlexGridSizer *flexgridsizer = new wxFlexGridSizer(3, 2, 5, 5);

    // Create and add controls to sizers. Note that a member variable
    // of g_data is bound to each control upon construction. There is
    // currently no easy way to substitute a different validator or a
    // different transfer variable after a control has been constructed.

    // Pointers to some of these controls are saved in member variables
    // so that we can use them elsewhere, like this one.
    m_text = new wxTextCtrl(this, VALIDATE_TEXT, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize, 0,
                            wxTextValidator(wxFILTER_ALPHA, &g_data.m_string));
    m_text->SetToolTip("uses wxTextValidator with wxFILTER_ALPHA");
    flexgridsizer->Add(m_text, 1, wxGROW);


    // Now set a wxTextValidator with an explicit list of characters NOT allowed:
    wxTextValidator textVal(wxFILTER_EMPTY|wxFILTER_EXCLUDE_LIST, &g_data.m_string2);
    textVal.SetCharExcludes("bcwyz");
    wxTextCtrl* txt2 =
             new wxTextCtrl(this, VALIDATE_TEXT2, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize, 0, textVal);
    txt2->SetToolTip("uses wxTextValidator with wxFILTER_EMPTY|wxFILTER_EXCLUDE_LIST (to exclude 'bcwyz')");
    flexgridsizer->Add(txt2, 1, wxGROW);

    flexgridsizer->Add(new wxListBox((wxWindow*)this, VALIDATE_LIST,
                        wxDefaultPosition, wxDefaultSize,
                        3, g_listbox_choices, wxLB_MULTIPLE,
                        wxGenericValidator(&g_data.m_listbox_choices)),
                       1, wxGROW);

    m_combobox = new wxComboBox(this, VALIDATE_COMBO, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                3, g_combobox_choices, 0L,
                                MyComboBoxValidator(&g_data.m_combobox_choice));
    m_combobox->SetToolTip("uses a custom validator (MyComboBoxValidator)");
    flexgridsizer->Add(m_combobox, 1, wxALIGN_CENTER);

    // This wxCheckBox* doesn't need to be assigned to any pointer
    // because we don't use it elsewhere--it can be anonymous.
    // We don't need any such pointer to query its state, which
    // can be gotten directly from g_data.
    flexgridsizer->Add(new wxCheckBox(this, VALIDATE_CHECK, wxT("Sample checkbox"),
                        wxDefaultPosition, wxDefaultSize, 0,
                        wxGenericValidator(&g_data.m_checkbox_state)),
                       1, wxALIGN_CENTER|wxALL, 15);

    flexgridsizer->AddGrowableCol(0);
    flexgridsizer->AddGrowableCol(1);
    flexgridsizer->AddGrowableRow(1);


    // setup the button sizer
    // ----------------------

    wxStdDialogButtonSizer *btn = new wxStdDialogButtonSizer();
    btn->AddButton(new wxButton(this, wxID_OK));
    btn->AddButton(new wxButton(this, wxID_CANCEL));
    btn->Realize();

    // setup a sizer with the controls for numeric validators
    // ------------------------------------------------------

    wxIntegerValidator<int> valInt(&g_data.m_intValue,
                                   wxNUM_VAL_THOUSANDS_SEPARATOR |
                                   wxNUM_VAL_ZERO_AS_BLANK);
    valInt.SetMin(0); // Only allow positive numbers

    m_numericTextInt = new wxTextCtrl
                           (
                                this,
                                wxID_ANY,
                                "",
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxTE_RIGHT,
                                valInt
                            );
    m_numericTextInt->SetToolTip("uses wxIntegerValidator to accept positive "
                                 "integers only");

    m_numericTextDouble = new wxTextCtrl
                              (
                                this,
                                wxID_ANY,
                                "",
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxTE_RIGHT,
                                wxMakeFloatingPointValidator
                                (
                                    3,
                                    &g_data.m_doubleValue,
                                    wxNUM_VAL_THOUSANDS_SEPARATOR |
                                    wxNUM_VAL_NO_TRAILING_ZEROES
                                )
                              );
    m_numericTextDouble->SetToolTip("uses wxFloatingPointValidator with 3 decimals");
    wxBoxSizer *numSizer = new wxBoxSizer( wxHORIZONTAL );
    numSizer->Add( m_numericTextInt, 1, wxALL, 10 );
    numSizer->Add( m_numericTextDouble, 1, wxALL, 10 );



    // setup the main sizer
    // --------------------

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add(flexgridsizer, 1, wxGROW | wxALL, 10);

    mainsizer->Add(new wxRadioBox((wxWindow*)this, VALIDATE_RADIO, wxT("Pick a color"),
                                    wxDefaultPosition, wxDefaultSize,
                                    3, g_radiobox_choices, 1, wxRA_SPECIFY_ROWS,
                                    wxGenericValidator(&g_data.m_radiobox_choice)),
                   0, wxGROW | wxLEFT|wxBOTTOM|wxRIGHT, 10);

    mainsizer->Add( numSizer, 0, wxGROW | wxALL );

    mainsizer->Add(btn, 0, wxGROW | wxALL, 10);

    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);

    // make the dialog a bit bigger than its minimal size:
    SetSize(GetBestSize()*1.5);
}

bool MyDialog::TransferDataToWindow()
{
    bool r = wxDialog::TransferDataToWindow();

    // These function calls have to be made here, after the
    // dialog has been created.
    m_text->SetFocus();
    m_combobox->SetSelection(0);

    return r;
}

