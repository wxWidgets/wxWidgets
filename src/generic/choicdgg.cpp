/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "choicdgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/listbox.h"
    #include "wx/stattext.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h" 
#endif

#include "wx/generic/choicdgg.h"

#define wxID_LISTBOX 3000

wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n, 
                            const wxString *choices, wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                            int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetStringSelection();
    else
        return _T("");
}

// Overloaded for backward compatibility
wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n, 
                            char *choices[], wxWindow *parent,
                            int x, int y, bool centre, 
                            int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    wxString ans(wxGetSingleChoice(message, caption, n, (const wxString *)strings, parent,
        x, y, centre, width, height));
    delete[] strings;
    return ans;
}

int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n, 
                            const wxString *choices, wxWindow *parent,
                int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetSelection();
    else
        return -1;
}

// Overloaded for backward compatibility
int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n, 
                            wxChar *choices[], wxWindow *parent,
                int x, int y, bool centre, 
                int width, int height )
{
    wxString *strings = new wxString[n];
    for ( int i = 0; i < n; i++)
        strings[i] = choices[i];
    int ans = wxGetSingleChoiceIndex(message, caption, n, (const wxString *)strings, parent,
        x, y, centre, width, height);
    delete[] strings;
    return ans;
}

wxChar *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n,
                   const wxString *choices, wxChar **client_data, wxWindow *parent,
                   int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                   int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices, client_data);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetSelectionClientData();
    else
        return NULL;
}

// Overloaded for backward compatibility
wxChar *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n, 
                   wxChar *choices[], wxChar **client_data, wxWindow *parent,
                   int x, int y, bool centre, 
                   int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    wxChar *data = wxGetSingleChoiceData(message, caption, n, (const wxString *)strings, client_data, parent,
        x, y, centre, width, height);
    delete[] strings;
    return data;
}


/* Multiple choice dialog contributed by Robert Cowell
 *

The new data passed are in the "int nsel" and "int * selection"

The idea is to make a multiple selection from list of strings.
The returned value is the total number selected. initialily there
are nsel selected, with indices stored in
selection[0],...,selection[nsel-1] which appear highlighted to
begin with. On exit with value i
selection[0..i-1] contains the indices of the selected items.
(Some prior selectecions might be deselected.)
Thus selection must be as big as choices, in case all items are
selected.

*/
/*
int wxGetMultipleChoice(const wxString& message, const wxString& caption,
              int n, const wxString *choices,
              int nsel, int * selection,
              wxWindow *parent , int x , int y, bool centre,
              int width, int height)
{
    return -1;
}
*/

// wxSingleChoiceDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
    EVT_LISTBOX_DCLICK(wxID_LISTBOX, wxSingleChoiceDialog::OnListBoxDClick)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxSingleChoiceDialog, wxDialog)
#endif

#define wxCHOICEDLG_DIALOG_STYLE (wxDEFAULT_DIALOG_STYLE | \
                                  wxDIALOG_MODAL |         \
                                  wxTAB_TRAVERSAL)

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           int n, 
                                           const wxString *choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& pos)
                    : wxDialog(parent, -1, caption, pos, wxDefaultSize,
                               wxCHOICEDLG_DIALOG_STYLE)
{
    Create(parent, message, caption, n, choices, clientData, style);
}

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           const wxStringList& choices, 
                                           wxChar **clientData, 
                                           long style, 
                                           const wxPoint& pos)
                    : wxDialog(parent, -1, caption, pos, wxDefaultSize,
                               wxCHOICEDLG_DIALOG_STYLE)
{
    Create(parent, message, caption, choices, clientData, style);
}

bool wxSingleChoiceDialog::Create(wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  const wxStringList& choices,
                                  char **clientData,
                                  long style,
                                  const wxPoint& pos)
{
    wxString *strings = new wxString[choices.Number()];
    int i;
    for ( i = 0; i < choices.Number(); i++)
    {
        strings[i] = (char *)choices.Nth(i)->Data();
    }
    bool ans = Create(parent, message, caption, choices.Number(), strings, clientData, style, pos);
    delete[] strings;
    return ans;
}

bool wxSingleChoiceDialog::Create( wxWindow *WXUNUSED(parent),
                                   const wxString& message, 
                                   const wxString& WXUNUSED(caption),
                                   int n, 
                                   const wxString *choices,
                                   char **clientData,
                                   long style,
                                   const wxPoint& WXUNUSED(pos) )
{
    m_dialogStyle = style;
    m_selection = 0;
    m_clientData = NULL;

    // dialog layout constants
    static const int LAYOUT_X_MARGIN = 5;
    static const int LAYOUT_Y_MARGIN = 5;
    static const int MARGIN_BETWEEN_BUTTONS = 3*LAYOUT_X_MARGIN;

    // calc the message size
    // ---------------------

    // TODO this should be factored out to a common function (also used in
    //      msgdlgg.cpp)
    wxClientDC dc(this);
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    wxArrayString lines;
    wxString curLine;
    long height, width, heightTextMax = 0, widthTextMax = 0;
    for ( const char *pc = message; ; pc++ ) {
        if ( *pc == '\n' || *pc == '\0' ) {
            dc.GetTextExtent(curLine, &width, &height);
            if ( width > widthTextMax )
                widthTextMax = width;
            if ( height > heightTextMax )
                heightTextMax = height;

            lines.Add(curLine);

            if ( *pc == '\n' ) {
               curLine.Empty();
            }
            else {
               // the end of string
               break;
            }
        }
        else {
            curLine += *pc;
        }
    }

    size_t nLineCount = lines.Count();
    long hTotalMsg = heightTextMax*nLineCount;

    // calc the button size
    // --------------------

    bool hasCancel = FALSE;

    // always create the OK button - the code below supposes we do have buttons
    // and besides the user should have some way to close this dialog
    wxASSERT_MSG( style & wxOK, _T("this dialog should have OK button") );

    wxString labelOk(_("OK"));
    long wButton = 0;
    dc.GetTextExtent(labelOk, &width, NULL);
    if ( width > wButton )
        wButton = width;

    wxString labelCancel;
    if ( style & wxCANCEL )
    {
        labelCancel = _("Cancel");
        dc.GetTextExtent(labelCancel, &width, NULL);
        if ( width > wButton )
            wButton = width;

        hasCancel = TRUE;
    }

    if ( wButton < 75 )
        wButton = 75;
    else
        wButton += 10;

    long hButton = wButton*23/75;
    long wTotalButtons = wButton;
    if ( hasCancel )
    {
        wTotalButtons *= 2;                         // second button
        wTotalButtons += MARGIN_BETWEEN_BUTTONS;    // margin between the 2
    }

    // listbox and stat line
    // ---------------------

    // make the listbox at least as tall as the message - otherwise it looks
    // ugly (the lower limit of 300 for the width is arbitrary OTOH)
    //
    // NB: we write "n + 2" because the horiz. scrollbar also takes some place
    long hListbox = wxMax((n + 2) * heightTextMax, hTotalMsg),
         wListbox = wxMax(300, wxMax(wTotalButtons, widthTextMax));

#if wxUSE_STATLINE
    // arbitrary...
    long hStatLine = 5;
#endif

    // now the complete dialog size
    // ----------------------------

    long hDialog = 2*LAYOUT_Y_MARGIN +  // top margin
                   hTotalMsg +          // message
                   2*LAYOUT_Y_MARGIN +  // margin between text and listbox
                   hListbox +           // listbox
#if wxUSE_STATLINE
                   LAYOUT_Y_MARGIN +    // margin
                   hStatLine +          // separator line
#endif
                   2*LAYOUT_Y_MARGIN +  // margin between listbox and buttons
                   hButton +            // button(s)
                   LAYOUT_Y_MARGIN;     // bottom margin

    long wDialog = wxMax(wListbox, wxMax(wTotalButtons, widthTextMax)) +
                   4*LAYOUT_X_MARGIN;   // 2 from each side

    // create the controls
    // -------------------

    // message
    wxStaticText *text;
    int y = 2*LAYOUT_Y_MARGIN;
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
    {
        text = new wxStaticText(this, -1, lines[nLine],
                                wxPoint(2*LAYOUT_X_MARGIN, y),
                                wxSize(widthTextMax, heightTextMax));
        y += heightTextMax;
    }

    y += 2*LAYOUT_X_MARGIN;

    // listbox
    m_listbox = new wxListBox( this, wxID_LISTBOX,
                               wxPoint(2*LAYOUT_X_MARGIN, y),
                               wxSize(wListbox, hListbox), 
                               n, choices,
                               wxLB_HSCROLL);
    y += hListbox;

    if ( clientData )
    {
        for (int i = 0; i < n; i++)
            m_listbox->SetClientData(i, clientData[i]);
    }

    // separator line
#if wxUSE_STATLINE
    (void) new wxStaticLine( this, -1,
                             wxPoint(0, y + LAYOUT_Y_MARGIN),
                             wxSize(wDialog, hStatLine) );

    y += LAYOUT_Y_MARGIN + hStatLine;
#endif
    
    // buttons

    y += 2*LAYOUT_X_MARGIN;

    // NB: create [Ok] first to get the right tab order

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    
    long x = wDialog / 2;
    if ( hasCancel )
        x -= MARGIN_BETWEEN_BUTTONS / 2 + wButton;
    else
        x -= wButton / 2;

    ok = new wxButton( this, wxID_OK, labelOk,
                       wxPoint(x, y),
                       wxSize(wButton, hButton) );

    if ( hasCancel )
    {
        x += MARGIN_BETWEEN_BUTTONS + wButton;
        cancel = new wxButton( this, wxID_CANCEL, labelCancel,
                               wxPoint(x, y),
                               wxSize(wButton, hButton) );
    }

    ok->SetDefault();
    ok->SetFocus();

    SetClientSize( wDialog, hDialog );

    Centre( wxBOTH );

    return TRUE;
}

// Set the selection
void wxSingleChoiceDialog::SetSelection(int sel)
{
    m_listbox->SetSelection(sel);
    m_selection = sel;
}

void wxSingleChoiceDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    m_selection = m_listbox->GetSelection();
    m_stringSelection = m_listbox->GetStringSelection();
    m_clientData = m_listbox->GetClientData(m_selection);

    EndModal(wxID_OK);
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    m_selection = m_listbox->GetSelection();
    m_stringSelection = m_listbox->GetStringSelection();
    m_clientData = m_listbox->GetClientData(m_selection);

    EndModal(wxID_OK);
}

