/////////////////////////////////////////////////////////////////////////////
// Name:        validate.h
// Purpose:     wxWidgets validation sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/combobox.h"
#include "wx/dialog.h"
#include "wx/dynarray.h"
#include "wx/frame.h"
#include "wx/hashset.h"
#include "wx/listbox.h"
#include "wx/string.h"

// Define a new application type
class MyApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

// Define a new frame type
class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString&title, int x, int y, int w, int h);

    void OnQuit(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnToggleBell(wxCommandEvent& event);
    void OnToggleInteractive(wxCommandEvent& event);

private:
    wxListBox *m_listbox;
    bool m_silent;

    wxDECLARE_EVENT_TABLE();
};

WX_DECLARE_HASH_SET(wxWindow*, wxPointerHash, wxPointerEqual,
                    InvalidWindowsSet);

class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow *parent, const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxDEFAULT_DIALOG_STYLE);

private:
    void OnChangeValidator(wxCommandEvent& event);
    void OnValidation(wxValidationStatusEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

private:
    wxTextCtrl *m_text;
    wxComboBox *m_combobox;

    wxTextCtrl *m_numericTextInt;
    wxTextCtrl *m_numericTextDouble;

    InvalidWindowsSet m_invalidWins;
};

// ----------------------------------------------------------------------------
// TextValidatorDialog
// ----------------------------------------------------------------------------
class TextValidatorDialog : public wxDialog
{
public:
    TextValidatorDialog(wxWindow *parent, wxTextCtrl* txtCtrl);

    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnChecked(wxCommandEvent& event);
    void OnKillFocus( wxFocusEvent &event );

    void ApplyValidator();

private:
    // Special validator for our checkboxes
    class StyleValidator : public wxValidator
    {
    public:
        StyleValidator(long* style) { m_style = style; }

        virtual bool Validate(wxWindow *WXUNUSED(parent)) wxOVERRIDE { return true; }
        virtual wxObject* Clone() const wxOVERRIDE { return new StyleValidator(*this); }

        // Called to transfer data to the window
        virtual bool TransferToWindow() wxOVERRIDE;

        // Called to transfer data from the window
        virtual bool TransferFromWindow() wxOVERRIDE;

    private:
        long* m_style;
    };

private:
    bool HasFlag(wxTextValidatorStyle style) const
    {
        return (m_validatorStyle & style) != 0;
    }

    enum
    {
        // CheckBoxes Ids (should be in sync with wxTextValidatorStyle)
        Id_None = wxID_HIGHEST,
        Id_Empty,
        Id_Ascii,
        Id_Alpha,
        Id_Alphanumeric,
        Id_Digits,
        Id_Numeric,
        Id_IncludeList,
        Id_IncludeCharList,
        Id_ExcludeList,
        Id_ExcludeCharList,
        Id_Xdigits,
        Id_Space,

        // TextCtrls Ids
        Id_IncludeListTxt,
        Id_IncludeCharListTxt,
        Id_ExcludeListTxt,
        Id_ExcludeCharListTxt,
    };

    wxTextCtrl* const m_txtCtrl;

    bool m_noValidation;
    long m_validatorStyle;

    wxString        m_charIncludes;
    wxString        m_charExcludes;
    wxArrayString   m_includes;
    wxArrayString   m_excludes;
};

class MyData
{
public:
    MyData();

    // These data members are designed for transfer to and from
    // controls, via validators. For instance, a text control's
    // transferred value is a string:
    wxString m_string, m_string2;

    // Listboxes may permit multiple selections, so their state
    // is transferred to an integer-array class.
    wxArrayInt m_listbox_choices;

    // Comboboxes differ from listboxes--validators transfer
    // the string entered in the combobox's text-edit field.
    wxString m_combobox_choice;

    // variables handled by wxNumericTextValidator
    int m_intValue;
    unsigned short m_smallIntValue;
    double m_doubleValue;
    float m_percentValue;

    bool m_checkbox_state;
    int m_radiobox_choice;
};

class MyComboBoxValidator : public wxValidator
{
public:
    MyComboBoxValidator(wxString* var) { m_var=var; }

    virtual bool Validate(wxWindow* parent) wxOVERRIDE;
    virtual wxObject* Clone() const wxOVERRIDE { return new MyComboBoxValidator(*this); }

    virtual void SetWindow(wxWindow *win) wxOVERRIDE;

    // Called to transfer data to the window
    virtual bool TransferToWindow() wxOVERRIDE;

    // Called to transfer data from the window
    virtual bool TransferFromWindow() wxOVERRIDE;

private:
    // Validate the control each time the value has changed.
    void OnValueChanged(wxCommandEvent& event);

    wxString* m_var;
};

enum
{
    VALIDATE_DIALOG_ID = wxID_HIGHEST,

    VALIDATE_TEST_DIALOG,
    VALIDATE_TOGGLE_BELL,
    VALIDATE_TOGGLE_INTERACTIVE,

    VALIDATE_TEXT,
    VALIDATE_LIST,
    VALIDATE_CHECK,
    VALIDATE_COMBO,
    VALIDATE_RADIO
};
