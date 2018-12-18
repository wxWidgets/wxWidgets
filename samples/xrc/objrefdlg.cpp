//-----------------------------------------------------------------------------
// Name:        objref.cpp
// Purpose:     XML resources sample: Object references and ID ranges dialog
// Author:      David Hart, Vaclav Slavik
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "objrefdlg.h"

//-----------------------------------------------------------------------------
// Needed wx headers,
//-----------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resouces



//-----------------------------------------------------------------------------
// Public members
//-----------------------------------------------------------------------------
ObjrefDialog::ObjrefDialog(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "objref_dialog");

    nb = XRCCTRL(*this, "objref_notebook", wxNotebook);
    wxCHECK_RET(nb, "failed to find objref_notebook");

    // Connect different event handlers.
    nb->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &ObjrefDialog::OnNotebookPageChanged, this);

    // We want to direct UpdateUI events for the ID range 'first_row' to
    // OnUpdateUIFirst(). We could achieve this using first_row[0] and
    // first_row[2], but what if a fourth column were added? It's safer to use
    // the 'typedefs' for the two ends of the range:
    wxNotebookPage *page = nb->GetPage(icons_page);
    page->Bind(wxEVT_UPDATE_UI, &ObjrefDialog::OnUpdateUIFirst, this,
               XRCID("first_row[start]"), XRCID("first_row[end]"));
    page->Bind(wxEVT_UPDATE_UI, &ObjrefDialog::OnUpdateUISecond, this,
               XRCID("second_row[start]"), XRCID("second_row[end]"));
    page->Bind(wxEVT_UPDATE_UI, &ObjrefDialog::OnUpdateUIThird, this,
               XRCID("third_row[start]"), XRCID("third_row[end]"));

    // Connect the id ranges, using the [start] and [end] 'typedefs'
    page = nb->GetPage(calc_page);
    page->Bind(wxEVT_BUTTON, &ObjrefDialog::OnNumeralClick, this,
               XRCID("digits[start]"), XRCID("digits[end]"));
    page->Bind(wxEVT_BUTTON, &ObjrefDialog::OnOperatorClick, this,
               XRCID("operators[start]"), XRCID("operators[end]"));

}

ObjrefDialog::~ObjrefDialog()
{
    // Select page 0. Otherwise if the Calc page were selected, when it's
    // removed the Icons page is selected and sets the log target again in idle
    // time, *after* myframe restores the old one!
    nb->ChangeSelection(0);
}

//-----------------------------------------------------------------------------
// Private members (including the event handlers)
//-----------------------------------------------------------------------------
void ObjrefDialog::OnNotebookPageChanged( wxNotebookEvent &event )
{
    switch(event.GetSelection())
    {
        case copy_page:
            {
                // This is a straight object reference to the first page
                // so change the text programmatically
                nb->SetPageText(copy_page, "Page 1 copy");

                wxNotebookPage *page = nb->GetPage(copy_page);
                wxTextCtrl *
                    text = XRCCTRL(*page, "description_text", wxTextCtrl);
                text->ChangeValue(
                    "This is a duplicate of page 1, using an object reference. "
                    "It was created by this very simple xml:\n\n"
                    "<object class=\"notebookpage\">\n\t<object_ref ref=\"page1\"/>\n"
                    "\t<label>Page 1 copy</label>\n</object>"
                    "\n\n(Then I'm cheating by inserting this text programmatically.)"
                                );
                break;
            }

        case icons_page:
            {
                wxNotebookPage *page = nb->GetPage(icons_page);
                wxTextCtrl* const text = XRCCTRL(*page, "log_text", wxTextCtrl);
                if (text)
                    delete wxLog::SetActiveTarget(new wxLogTextCtrl(text));
                break;
            }

        case calc_page:
            {
                wxNotebookPage *page = nb->GetPage(calc_page);
                result_txt = XRCCTRL(*page, "result", wxTextCtrl);
                wxTextCtrl* const text = XRCCTRL(*page, "log_text", wxTextCtrl);
                if (text)
                    delete wxLog::SetActiveTarget(new wxLogTextCtrl(text));

                ClearCalculator();
                break;
            }
    }
}

// There are undoubtedly simpler ways of doing all this, but we're
// demonstrating the use of ID ranges
void ObjrefDialog::OnUpdateUIFirst(wxUpdateUIEvent& event)
{
    // The checkbox with the XRCID 'check[0]' controls this row of icons
    wxCheckBox *
        chk = XRCCTRL(*(nb->GetPage(icons_page)), "check[0]", wxCheckBox);
    if (chk)
        event.Enable(chk->IsChecked());

    // Let's create a log-window entry
    static bool checked = true;
    if (chk->IsChecked() != checked)
    {
        checked = chk->IsChecked();
        wxLogMessage("Row one has been %s by check[0], XRCID = %i",
                     checked ? "enabled" : "disabled", XRCID("check[0]"));
        wxLogMessage("XRCIDs: first_row[start] = %i, first_row[0] = %i, "
                     "first_row[1] = %i, first_row[2] = %i, "
                     "first_row[end] = %i",
                     XRCID("first_row[start]"), XRCID("first_row[0]"),
                     XRCID("first_row[1]"), XRCID("first_row[2]"),
                     XRCID("first_row[end]"));
    }
}

void ObjrefDialog::OnUpdateUISecond(wxUpdateUIEvent& event)
{
    // The checkbox with the XRCID 'check[1]' controls this row of icons
    wxCheckBox *
        chk = XRCCTRL(*(nb->GetPage(icons_page)), "check[1]", wxCheckBox);
    if (chk)
        event.Enable(chk->IsChecked());

    // Let's create a log-window entry
    static bool checked = true;
    if (chk->IsChecked() != checked)
    {
        checked = chk->IsChecked();
        wxLogMessage("Row two has been %s by check[1], XRCID = %i",
                     checked ? "enabled" : "disabled", XRCID("check[1]"));
        wxLogMessage("XRCIDs: second_row[start] = %i, second_row[0] = %i, "
                     "second_row[1] = %i, second_row[2] = %i, "
                     "second_row[end] = %i",
                     XRCID("second_row[start]"), XRCID("second_row[0]"),
                     XRCID("second_row[1]"), XRCID("second_row[2]"),
                     XRCID("second_row[end]"));
    }
}

void ObjrefDialog::OnUpdateUIThird(wxUpdateUIEvent& event)
{
    // The checkbox with the XRCID 'check[2]' controls this row of icons
    wxCheckBox *
        chk = XRCCTRL(*(nb->GetPage(icons_page)), "check[2]", wxCheckBox);
    if (chk)
        event.Enable(chk->IsChecked());

    // Let's create a log-window entry
    static bool checked = true;
    if (chk->IsChecked() != checked)
    {
        checked = chk->IsChecked();
        wxLogMessage("Row three has been %s by check[2], XRCID = %i",
                     checked ? "enabled" : "disabled", XRCID("check[2]"));
        wxLogMessage("XRCIDs: third_row[start] = %i, third_row[0] = %i, "
                     "third_row[1] = %i, third_row[2] = %i, "
                     "third_row[end] = %i",
                     XRCID("third_row[start]"), XRCID("third_row[0]"),
                     XRCID("third_row[1]"), XRCID("third_row[2]"),
                     XRCID("third_row[end]"));
    }
}

void ObjrefDialog::OnNumeralClick(wxCommandEvent& event)
{
    // See how the id range simplifies determining which numeral was clicked
    int digit = event.GetId() - XRCID("digits[start]");

    char c = '0' + digit;
    if (current==0 && previous==0)
    {
        // We're just starting a calculation, so get rid of the placeholder '0'
        result_txt->Clear();
    }
    else if (operator_expected == true)
    {
        // If we've just finished one calculation, and now a digit is entered,
        // clear
        ClearCalculator();
        result_txt->Clear();
    }
    (*result_txt) << c;


    current = current*10 + digit;

    wxLogMessage("You clicked digits[%c], XRCID %i", c, event.GetId());
}

void ObjrefDialog::OnOperatorClick(wxCommandEvent& event)
{
    static const char symbols[] = "+-*/=";

    operator_expected = false;
    int ID = event.GetId() - XRCID("operators[start]");

    // We carefully used "operators[end]" as the name of the Clear button
    if (event.GetId() == XRCID("operators[end]"))
    {
        wxLogMessage("You clicked operators[%i], XRCID %d, 'Clear'",
                     ID, event.GetId());
        ClearCalculator();
        return;
    }

    switch(ID)
    {
        case operator_plus:
        case operator_minus:
        case operator_multiply:
        case operator_divide:
            if (current!=0 || previous!=0)
            {
                // We're in the middle of a complex calculation, so do the
                // first bit
                Calculate();
            }
            curr_operator = (CalcOperator)ID;
            break;

        case operator_equals:
            Calculate();
            wxLogMessage("You clicked operators[%i], XRCID %i, giving a '%c'",
                         ID, event.GetId(), symbols[ID]);
            curr_operator = operator_equals;
            // Flag that the next entry should be an operator, not a digit
            operator_expected = true;
            return;
    }

    (*result_txt) << ' ' << symbols[ID] << ' ';

    wxLogMessage("You clicked operators[%i], XRCID %i, giving a '%c'",
                 ID, event.GetId(), symbols[ID]);
}

void ObjrefDialog::Calculate()
{
    switch(curr_operator)
    {
        case operator_plus:
            previous += current; break;
        case operator_minus:
            previous -= current; break;
        case operator_multiply:
            previous *= current; break;
        case operator_divide:
            if (current!=0)
                previous /= current;
            break;
         default: return;
    }

    curr_operator = operator_plus;
    current = 0;
    result_txt->Clear();

    (*result_txt) << previous;
}

void ObjrefDialog::ClearCalculator()
{
    current = previous = 0;
    curr_operator = operator_plus;
    operator_expected = false;
    result_txt->ChangeValue("0");
}
