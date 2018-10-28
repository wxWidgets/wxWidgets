/////////////////////////////////////////////////////////////////////////////
// Name:        validate.cpp
// Purpose:     wxWidgets validator sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
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

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// Global data
// ----------------------------------------------------------------------------

/*static*/
const float MyData::ms_factor = 100.0f;

MyData g_data;

wxString g_listbox_choices[] =
    {"one",  "two",  "three"};

wxString g_combobox_choices[] =
    {"yes", "no (doesn't validate)", "maybe (doesn't validate)"};

wxString g_radiobox_choices[] =
    {"green", "yellow", "red"};

// ----------------------------------------------------------------------------
// MyData
// ----------------------------------------------------------------------------

MyData::MyData()
{
    // This string will be passed to an alpha-only validator, which
    // will complain because spaces aren't alpha. Note that validation
    // is performed only when 'OK' is pressed.
    m_string = "Spaces are invalid here";
    m_string2 = "Valid text";
    m_listbox_choices.Add(0);
    m_intValue = 0;

    m_minSmallInt = 150;
    m_maxSmallInt = 777;
    m_smallIntValue = m_minSmallInt;

    m_doubleValue = 12354.31;

    m_minPercent = -0.035; // -3.5%
    m_maxPercent = 0.995;  // 99.5%
    m_percentValue = m_minPercent;
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

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create and display the main frame window.
    MyFrame *frame = new MyFrame((wxFrame *) NULL, "Validator Test",
                                 50, 50, 300, 250);
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(VALIDATE_RANGES_DIALOG, MyFrame::OnRangesDialog)
    EVT_MENU(VALIDATE_TEST_DIALOG, MyFrame::OnTestDialog)
    EVT_MENU(VALIDATE_TOGGLE_BELL, MyFrame::OnToggleBell)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString&title, int x, int y, int w, int h)
       : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h)),
         m_silent(true)
{
    SetIcon(wxICON(sample));

    // Create a listbox to display the validated data.
    m_listbox = new wxListBox(this, wxID_ANY);
    m_listbox->Append(wxString("Try 'File|Test' to see how validators work."));

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(VALIDATE_RANGES_DIALOG, "Set &ranges...\tCtrl-R", "Set ranges");
    file_menu->Append(VALIDATE_TEST_DIALOG, "&Test dialog...\tCtrl-T", "Demonstrate validators");
    file_menu->AppendCheckItem(VALIDATE_TOGGLE_BELL, "&Bell on error", "Toggle bell on error");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
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

void MyFrame::OnRangesDialog(wxCommandEvent& WXUNUSED(event))
{
    RangesDialog dialog(this);

    dialog.ShowModal();
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event))
{
    // The validators defined in the dialog implementation bind controls
    // and variables together. Values are transferred between them behind
    // the scenes, so here we don't have to query the controls for their
    // values.
    MyDialog dialog(this, "Validator demonstration");

    // When the dialog is displayed, validators automatically transfer
    // data from variables to their corresponding controls.
    if ( dialog.ShowModal() == wxID_OK )
    {
        // 'OK' was pressed, so controls that have validators are
        // automatically transferred to the variables we specified
        // when we created the validators.
        m_listbox->Clear();
        m_listbox->Append(wxString("string: ") + g_data.m_string);
        m_listbox->Append(wxString("string #2: ") + g_data.m_string2);

        for(unsigned int i = 0; i < g_data.m_listbox_choices.GetCount(); ++i)
        {
            int j = g_data.m_listbox_choices[i];
            m_listbox->Append(wxString("listbox choice(s): ") + g_listbox_choices[j]);
        }

        wxString checkbox_state(g_data.m_checkbox_state ? "checked" : "unchecked");
        m_listbox->Append(wxString("checkbox: ") + checkbox_state);
        m_listbox->Append(wxString("combobox: ") + g_data.m_combobox_choice);
        m_listbox->Append(wxString("radiobox: ") + g_radiobox_choices[g_data.m_radiobox_choice]);

        m_listbox->Append(wxString::Format("integer value: %d", g_data.m_intValue));
        m_listbox->Append(wxString::Format("small int value: %u", g_data.m_smallIntValue));
        m_listbox->Append(wxString::Format("double value: %.3f", g_data.m_doubleValue));
        m_listbox->Append(wxString::Format("percent value: %.4f", g_data.m_percentValue));
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
    wxTextValidator textVal(wxFILTER_EMPTY|wxFILTER_EXCLUDE_CHAR_LIST, &g_data.m_string2);
    textVal.SetCharExcludes("bcwyz");
    wxTextCtrl* txt2 =
             new wxTextCtrl(this, VALIDATE_TEXT2, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize, 0, textVal);
    txt2->SetToolTip("uses wxTextValidator with wxFILTER_EMPTY|wxFILTER_EXCLUDE_CHAR_LIST to exclude 'bcwyz'");
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
    flexgridsizer->Add(new wxCheckBox(this, VALIDATE_CHECK, "Sample checkbox",
                        wxDefaultPosition, wxDefaultSize, 0,
                        wxGenericValidator(&g_data.m_checkbox_state)),
                       1, wxALIGN_CENTER|wxALL, 15);

    flexgridsizer->AddGrowableCol(0);
    flexgridsizer->AddGrowableCol(1);
    flexgridsizer->AddGrowableRow(1);


    // setup a sizer with the controls for numeric validators
    // ------------------------------------------------------

    wxFlexGridSizer* const
        numSizer = new wxFlexGridSizer(5, FromDIP(wxSize(5, 5)));

    const wxSizerFlags center = wxSizerFlags().CenterVertical();

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
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Positive integer:"),
                  center);
    numSizer->Add(m_numericTextInt, wxSizerFlags(center).Expand());

    numSizer->AddSpacer(FromDIP(10));

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
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Up to 3 decimals:"),
                  center);
    numSizer->Add(m_numericTextDouble, wxSizerFlags(center).Expand());

    wxIntegerValidator<unsigned short> smallIntVal(&g_data.m_smallIntValue);
    smallIntVal.SetRange(g_data.m_minSmallInt, g_data.m_maxSmallInt);

    const wxString lable_1 =
        wxString::Format("Int between %d and %d:",
            g_data.m_minSmallInt, g_data.m_maxSmallInt);
    numSizer->Add(new wxStaticText(this, wxID_ANY, lable_1), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, "",
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 smallIntVal),
                  wxSizerFlags(center).Expand());

    numSizer->AddSpacer(FromDIP(10));

    wxFloatingPointValidator<float> percentVal(&g_data.m_percentValue);
    percentVal.SetPrecision(2);
    percentVal.SetFactor(MyData::ms_factor);
    // FIXME: i couldn't enter -3.5 in the control but -3.49!
    percentVal.SetRange(g_data.m_minPercent, g_data.m_maxPercent);

    const wxString lable_2 =
        wxString::Format("Value displayed in %% [%g%%..%g%%]",
            g_data.m_minPercent * MyData::ms_factor,
            g_data.m_maxPercent * MyData::ms_factor);
    numSizer->Add(new wxStaticText(this, wxID_ANY, lable_2), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, "",
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 percentVal),
                  wxSizerFlags(center).Expand());

    // setup the main sizer
    // --------------------

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add(flexgridsizer, 1, wxGROW | wxALL, 10);

    mainsizer->Add(new wxRadioBox((wxWindow*)this, VALIDATE_RADIO, "Pick a color",
                                    wxDefaultPosition, wxDefaultSize,
                                    3, g_radiobox_choices, 1, wxRA_SPECIFY_ROWS,
                                    wxGenericValidator(&g_data.m_radiobox_choice)),
                   0, wxGROW | wxLEFT|wxBOTTOM|wxRIGHT, 10);

    mainsizer->Add( numSizer, wxSizerFlags().Expand().DoubleBorder() );

    mainsizer->Add(CreateButtonSizer(wxOK | wxCANCEL),
                   wxSizerFlags().Expand().DoubleBorder());

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

// ----------------------------------------------------------------------------
// RangesDialog
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(RangesDialog, wxDialog)
    EVT_BUTTON(VALIDATE_BTN_USHORT, RangesDialog::OnButtonClick)
    EVT_BUTTON(VALIDATE_BTN_FLOAT, RangesDialog::OnButtonClick)
wxEND_EVENT_TABLE()

RangesDialog::RangesDialog(wxWindow *parent) :
    wxDialog(parent, wxID_ANY, "Ranges")
{
    wxFlexGridSizer* const
        numSizer = new wxFlexGridSizer(4, 3, FromDIP(wxSize(5, 5)));

    const wxSizerFlags center = wxSizerFlags().CenterVertical();

    wxIntegerValidator<unsigned short> minSmallIntVal(&g_data.m_minSmallInt);
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Min:"), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, wxString(),
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 minSmallIntVal),
                  wxSizerFlags(center).Expand());
    numSizer->Add(new wxStaticText(this, wxID_ANY, "(unsigned short)"), center);

    wxIntegerValidator<unsigned short> maxSmallIntVal(&g_data.m_maxSmallInt);
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Max:"), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, wxString(),
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 maxSmallIntVal),
                  wxSizerFlags(center).Expand());
    numSizer->Add(new wxStaticText(this, wxID_ANY, "(unsigned short)"), center);

//    numSizer->AddSpacer(FromDIP(10));

    wxFloatingPointValidator<float> minPercentVal(&g_data.m_minPercent);
    minPercentVal.SetPrecision(2);
    minPercentVal.SetFactor(MyData::ms_factor);
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Min:"), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, wxString(),
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 minPercentVal),
                  wxSizerFlags(center).Expand());
    numSizer->Add(new wxStaticText(this, wxID_ANY, "(float)"), center);

    wxFloatingPointValidator<float> maxPercentVal(&g_data.m_maxPercent);
    maxPercentVal.SetPrecision(2);
    maxPercentVal.SetFactor(MyData::ms_factor);
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Max:"), center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, wxString(),
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 maxPercentVal),
                  wxSizerFlags(center).Expand());
    numSizer->Add(new wxStaticText(this, wxID_ANY, "(float)"), center);

    wxBoxSizer *btnsizer = new wxBoxSizer( wxHORIZONTAL );

    btnsizer->Add(new wxButton(this, VALIDATE_BTN_USHORT, "Full range (unsigned short)"), center);
    btnsizer->Add(new wxButton(this, VALIDATE_BTN_FLOAT, "Full range (float)"), center);

    // setup the main sizer
    // --------------------

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add(btnsizer, wxSizerFlags().Expand().DoubleBorder());
    mainsizer->Add(numSizer, wxSizerFlags().Expand().DoubleBorder());

    mainsizer->Add(CreateButtonSizer(wxOK | wxCANCEL),
                   wxSizerFlags().Expand().DoubleBorder());

    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);
}

void RangesDialog::OnButtonClick(wxCommandEvent& event)
{
    const int id = event.GetId();

    if (id == VALIDATE_BTN_USHORT)
    {
        g_data.m_minSmallInt = std::numeric_limits<unsigned short>::min();
        g_data.m_maxSmallInt = std::numeric_limits<unsigned short>::max();
    }
    else if (id == VALIDATE_BTN_FLOAT)
    {
        g_data.m_minPercent = 0.0f; // 0%
        g_data.m_maxPercent = 1.0f; // 100%
    }

    TransferDataToWindow();
}
