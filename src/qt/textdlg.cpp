/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textdlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/textdlg.h"
#include "../generic/textdlgg.cpp"

// For compilers that support precompilation, includes "wx.h".
//#include "wx/wxprec.h"
//
//#include "wx/textdlg.h"

//wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent,
//                  const wxString& message,
//                  const wxString& caption,
//                  const wxString& value,
//                  long style,
//                  const wxPoint& pos)
//{
//    m_qtInputDialog = new QInputDialog( parent->GetHandle() );
//    m_qtInputDialog->setInputMode( TextInput );
//}
//
//void wxTextEntryDialog::SetValue(const wxString& val)
//{
//    m_qtInputDialog->setTextValue( wxQtConvertString( val ));
//}
//
//const wxString &wxTextEntryDialog::GetValue() const
//{
//    return wxQtConvertString( m_qtInputDialog->textValue() );
//}
//
//#if wxUSE_VALIDATORS
//void wxTextEntryDialog::SetTextValidator( const wxTextValidator& validator )
//{
//}
//
//void wxTextEntryDialog::SetTextValidator( wxTextValidatorStyle style)
//{
//}
//
//wxTextValidator* wxTextEntryDialog::GetTextValidator()
//{
//    return NULL;
//}
//
//#endif
//
//QInputDialog *wxTextEntryDialog::GetHandle() const
//{
//    return m_qtInputDialog;
//}
//
////##############################################################################
//
//wxPasswordEntryDialog::wxPasswordEntryDialog(wxWindow *parent,
//                  const wxString& message,
//                  const wxString& caption,
//                  const wxString& value,
//                  long style,
//                  const wxPoint& pos)
//    : wxTextEntryDialog( parent, message, caption, value, style, pos )
//{
//}


