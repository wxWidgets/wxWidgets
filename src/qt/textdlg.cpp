/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textdlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textdlg.h"

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent,
                  const wxString& message,
                  const wxString& caption,
                  const wxString& value,
                  long style,
                  const wxPoint& pos)
{
}

void wxTextEntryDialog::SetValue(const wxString& val)
{
}

const wxString &wxTextEntryDialog::GetValue() const
{
    static const wxString s_string;

    return s_string;
}

#if wxUSE_VALIDATORS
void wxTextEntryDialog::SetTextValidator( const wxTextValidator& validator )
{
}

void wxTextEntryDialog::SetTextValidator( wxTextValidatorStyle style)
{
}

wxTextValidator* wxTextEntryDialog::GetTextValidator()
{
    return NULL;
}

#endif

QInputDialog *wxTextEntryDialog::GetHandle() const
{
    return NULL;
}

//##############################################################################

wxPasswordEntryDialog::wxPasswordEntryDialog(wxWindow *parent,
                  const wxString& message,
                  const wxString& caption,
                  const wxString& value,
                  long style,
                  const wxPoint& pos)
    : wxTextEntryDialog( parent, message, caption, value, style, pos )
{
}


