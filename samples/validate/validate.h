/////////////////////////////////////////////////////////////////////////////
// Name:        validate.h
// Purpose:     wxWidgets validation sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/combobox.h"
#include "wx/dialog.h"
#include "wx/dynarray.h"
#include "wx/frame.h"
#include "wx/listbox.h"
#include "wx/string.h"

// Define a new application type
class MyApp : public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString&title, int x, int y, int w, int h);

    void OnQuit(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnToggleBell(wxCommandEvent& event);

private:
    wxListBox *m_listbox;
    bool m_silent;

    DECLARE_EVENT_TABLE()
};

class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow *parent, const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxDEFAULT_DIALOG_STYLE);

    bool TransferDataToWindow();
    wxTextCtrl *m_text;
    wxComboBox *m_combobox;

    wxTextCtrl *m_numericTextInt;
    wxTextCtrl *m_numericTextDouble;
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
    double m_doubleValue;

    bool m_checkbox_state;
    int m_radiobox_choice;
};

class MyComboBoxValidator : public wxValidator
{
public:
    MyComboBoxValidator(const MyComboBoxValidator& tocopy) { m_var=tocopy.m_var; }
    MyComboBoxValidator(wxString* var) { m_var=var; }

    virtual bool Validate(wxWindow* parent);
    virtual wxObject* Clone() const { return new MyComboBoxValidator(*this); }

    // Called to transfer data to the window
    virtual bool TransferToWindow();

    // Called to transfer data from the window
    virtual bool TransferFromWindow();

protected:
    wxString* m_var;
};

enum
{
    VALIDATE_DIALOG_ID = wxID_HIGHEST,

    VALIDATE_TEST_DIALOG,
    VALIDATE_TOGGLE_BELL,

    VALIDATE_TEXT,
    VALIDATE_TEXT2,
    VALIDATE_LIST,
    VALIDATE_CHECK,
    VALIDATE_COMBO,
    VALIDATE_RADIO
};
