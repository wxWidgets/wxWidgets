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
#include "wx/valpnl.h"
#include "wx/spinctrl.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// Global data
// ----------------------------------------------------------------------------

wxString g_listbox_choices[] =
    {"one",  "two",  "three"};

wxString g_combobox_choices[] =
    {"yes", "no (doesn't validate)", "maybe", "accepted"};

wxString g_radiobox_choices[] =
    {"green", "yellow", "red"};

// initialization order matters (g_combobox_choices used in MyData ctor)
MyData g_data;

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
    m_combobox_choice = g_combobox_choices[0];
    m_intValue = 0;
    m_smallIntValue = 3;
    m_doubleValue = 12354.31;
    m_percentValue = 0.25;
}

// define these overloads unconditionally to prevent compilation errors.
static inline wxString GetString(const wxScopedPtr<wxString>& ptr)
{
    if (ptr)
        return *ptr;

    return wxString();
}

static inline wxString GetString(const wxString& var){ return var; }

#if wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

// we don't have to derive a whole new class from wxValidator
// just to customize validation behaviour. All we have to do 
// is specialize wxDataTransfer<W>::DoValidate() for this matter.
template<>
bool wxDataTransfer<wxComboBox>::DoValidate(wxComboBox* cb, wxWindow* parent)
{
    wxUnusedVar(parent);

    if (cb->GetValue() == g_combobox_choices[1] ||
          (cb->GetValue() == g_combobox_choices[2] && cb->GetId() != VALIDATE_COMBO2))
    {
        // we accept any string != g_combobox_choices[1|2] !

        wxLogError("Invalid combo box text!");
        return false;
    }

    return true;
}

#if defined(HAVE_STD_VARIANT)

class MyPanel 
    : public wxMonoValidatorPanel<MyData::VariantType2, 
                                  wxTextCtrl, wxSpinCtrl, wxComboBox, wxTextCtrl>
{
    using Base = wxMonoValidatorPanel<MyData::VariantType2, 
                                      wxTextCtrl, wxSpinCtrl, wxComboBox, wxTextCtrl>;

public:
    MyPanel(wxWindow *parent)
        : Base(parent, g_data.m_int_or_str)
    {
        wxSizer* const sizer = new wxFlexGridSizer(2, wxSize(5, 5));
        sizer->Add(new wxStaticText(this, wxID_ANY, "Enter your &name:"),
                   wxSizerFlags().Center().Right());
        auto textName = new wxTextCtrl(this, wxID_ANY);
        textName->SetHint("First Last");
        sizer->Add(textName, wxSizerFlags().Expand().CenterVertical());

        sizer->Add(new wxStaticText(this, wxID_ANY, "And your &age:"),
                   wxSizerFlags().Center().Right());
        auto spinAge = new wxSpinCtrl(this, wxID_ANY);
        sizer->Add(spinAge, wxSizerFlags().Expand().CenterVertical());

        sizer->Add(new wxStaticText(this, wxID_ANY, "Select any:"),
                   wxSizerFlags().Center().Right());
        auto combobox = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                WXSIZEOF(g_combobox_choices), g_combobox_choices, 0L);
        sizer->Add(combobox, wxSizerFlags().Expand().CenterVertical());

        sizer->Add(new wxStaticText(this, wxID_ANY, "Enter your &job:"),
                   wxSizerFlags().Center().Right());
        auto textJob = new wxTextCtrl(this, wxID_ANY);
        textJob->SetHint("Your job");
        sizer->Add(textJob, wxSizerFlags().Expand().CenterVertical());

        wxStaticBoxSizer* const
            box = new wxStaticBoxSizer(wxVERTICAL, this, "wxWidgets box");
        box->Add(sizer, wxSizerFlags(1).Expand());
        SetSizer(box);

        // We won't be resized automatically, so set our size ourselves.
        SetSize(GetBestSize());

        //
        SetAlternatives(textName, spinAge, combobox, textJob);
    }
};


static auto GetString(const MyData::VariantType& var)
{
    return std::visit(wxVisitor{
                [](int i){ return wxString::Format("item(%d)", i); },
                [](const wxString& str){ return str; }
           }, var);
}

static auto GetString(const MyData::VariantType2& var)
{
    return std::visit(wxVisitor{
                [](int i){ return wxString::Format("item(%d)", i); },
                [](bool b){ return wxString::Format("%s", (b ? "Selected" : "Unselected")); },
                [](const wxString& str){ return str; }
           }, var);
}

#endif // defined(HAVE_STD_VARIANT)

#else // wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

// ----------------------------------------------------------------------------
// MyComboBoxValidator
// ----------------------------------------------------------------------------

class MyComboBoxValidator : public wxValidator
{
public:
    MyComboBoxValidator(wxString* var) { m_var=var; }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE;
    virtual wxObject* Clone() const wxOVERRIDE { return new MyComboBoxValidator(*this); }

    // Called to transfer data to the window
    virtual bool TransferToWindow() wxOVERRIDE;

    // Called to transfer data from the window
    virtual bool TransferFromWindow() wxOVERRIDE;

protected:
    wxString* m_var;
};

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

#endif // wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

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
        m_listbox->Append(wxString("combobox2: ") + GetString(g_data.m_combobox2_choice));
        m_listbox->Append(wxString("combobox3: ") + GetString(g_data.m_combobox3_choice));
        m_listbox->Append(wxString("Int or Str: ") + GetString(g_data.m_int_or_str));
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

    wxListBox* const lstbox = new wxListBox((wxWindow*)this, VALIDATE_LIST, wxDefaultPosition, wxDefaultSize,
                                            WXSIZEOF(g_listbox_choices), g_listbox_choices, wxLB_MULTIPLE);
    wxSetGenericValidator(lstbox, &g_data.m_listbox_choices);

    flexgridsizer->Add(lstbox, 1, wxGROW);

    wxBoxSizer *combosizer = new wxBoxSizer( wxVERTICAL );

    m_combobox = new wxComboBox(this, VALIDATE_COMBO, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                WXSIZEOF(g_combobox_choices), g_combobox_choices, 0L);

    m_combobox2 = new wxComboBox(this, VALIDATE_COMBO2, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(g_combobox_choices), g_combobox_choices, 0L);

    m_combobox3 = new wxComboBox(this, VALIDATE_COMBO3, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(g_combobox_choices), g_combobox_choices, 0L);

#if wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER
    wxSetGenericValidator(m_combobox, &g_data.m_combobox_choice);
    m_combobox->SetToolTip("uses generic validator (with validation)");

    wxSetGenericValidator(m_combobox2, g_data.m_combobox2_choice);
    m_combobox2->SetToolTip("uses generic validator (with validation, wxScopedPtr)");

    #if defined(HAVE_STD_VARIANT)
    wxSetGenericValidator(m_combobox3, g_data.m_combobox3_choice);
    m_combobox3->SetToolTip("uses generic validator (with validation, std::variant)");

    MyPanel* const panel = new MyPanel(this);
    combosizer->Add(panel, wxSizerFlags().Expand());
    combosizer->AddSpacer(10);

    #else
    m_combobox3->Disable();
    #endif // defined(HAVE_STD_VARIANT)

#else
    m_combobox->SetValidator(MyComboBoxValidator(&g_data.m_combobox_choice));
    m_combobox->SetToolTip("uses a custom validator (MyComboBoxValidator)");

    m_combobox2->Disable();
#endif // wxUSE_DATATRANSFER && wxCAN_USE_DATATRANSFER

    combosizer->Add(m_combobox, wxSizerFlags().CenterHorizontal());
    combosizer->AddSpacer(10);
    combosizer->Add(m_combobox2, wxSizerFlags().CenterHorizontal());
    combosizer->AddSpacer(10);
    combosizer->Add(m_combobox3, wxSizerFlags().CenterHorizontal());

    flexgridsizer->Add(combosizer, wxSizerFlags(1).Expand().Border(wxALL, 5));

    wxCheckBox* const checkbox = new wxCheckBox(this, VALIDATE_CHECK, "Sample checkbox");
    wxSetGenericValidator(checkbox, &g_data.m_checkbox_state);

    flexgridsizer->Add(checkbox, 1, wxALIGN_CENTER|wxALL, 15);

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
    smallIntVal.SetRange(1, 5);
    numSizer->Add(new wxStaticText(this, wxID_ANY, "Int between 1 and 5:"),
                  center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, "",
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 smallIntVal),
                  wxSizerFlags(center).Expand());

    numSizer->AddSpacer(FromDIP(10));

    wxFloatingPointValidator<float> percentVal(&g_data.m_percentValue);
    percentVal.SetPrecision(2);
    percentVal.SetFactor(100.0);

    numSizer->Add(new wxStaticText(this, wxID_ANY, "Value displayed in %:"),
                  center);
    numSizer->Add(new wxTextCtrl(this, wxID_ANY, "",
                                 wxDefaultPosition, wxDefaultSize, wxTE_RIGHT,
                                 percentVal),
                  wxSizerFlags(center).Expand());

    // setup the main sizer
    // --------------------

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add(flexgridsizer, 1, wxGROW | wxALL, 10);

    wxRadioBox* const radiobox = new wxRadioBox((wxWindow*)this, VALIDATE_RADIO, "Pick a color",
                                                wxDefaultPosition, wxDefaultSize,
                                                WXSIZEOF(g_radiobox_choices), g_radiobox_choices,
                                                1, wxRA_SPECIFY_ROWS);
    wxSetGenericValidator(radiobox, &g_data.m_radiobox_choice);

    mainsizer->Add(radiobox, 0, wxGROW | wxLEFT|wxBOTTOM|wxRIGHT, 10);

    mainsizer->Add( numSizer, wxSizerFlags().Expand().DoubleBorder() );

    mainsizer->Add(CreateButtonSizer(wxOK | wxCANCEL),
                   wxSizerFlags().Expand().DoubleBorder());

    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);

    // make the dialog a bit bigger than its minimal size:
    SetSize(GetBestSize()*1.5);

    m_text->SetFocus();
}
