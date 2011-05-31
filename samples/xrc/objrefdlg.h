//-----------------------------------------------------------------------------
// Name:        objref.h
// Purpose:     XML resources sample: Object references and ID ranges dialog
// Author:      David Hart, Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#ifndef _OBJREFDLG_H_
#define _OBJREFDLG_H_

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/notebook.h"

//-----------------------------------------------------------------------------
// Class definition: ObjrefDialog
//-----------------------------------------------------------------------------

class ObjrefDialog : public wxDialog
{

public:

    // Constructor.
    ObjrefDialog( wxWindow* parent );

    // Destructor.
    ~ObjrefDialog();

private:
    enum PageNumbers
    {
        first_page,
        copy_page,
        icons_page,
        calc_page
    };

    enum CalcOperator
    {
        operator_plus,
        operator_minus,
        operator_multiply,
        operator_divide,
        operator_equals
    };

    void OnNotebookPageChanged( wxNotebookEvent &event );
    void OnUpdateUIFirst(wxUpdateUIEvent& event);
    void OnUpdateUISecond(wxUpdateUIEvent& event);
    void OnUpdateUIThird(wxUpdateUIEvent& event);
    void OnNumeralClick(wxCommandEvent& event);
    void OnOperatorClick(wxCommandEvent& event);
    void Calculate();
    void ClearCalculator();

    wxNotebook *nb;
    wxTextCtrl *text;
    wxTextCtrl *result_txt;
    int current;
    int previous;
    bool operator_expected;
    CalcOperator curr_operator;

};

#endif  //_OBJREFDLG_H_
