/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/textdlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TEXTDLG_H_
#define _WX_QT_TEXTDLG_H_

// Use the generic version for now:
#include "wx/generic/textdlgg.h"

//#if wxUSE_TEXTDLG
//
//#if wxUSE_VALIDATORS
//#include "wx/valtext.h"
//#include "wx/textctrl.h"
//#endif
//
//#include "wx/dialog.h"
//#include "wx/qt/pointer_qt.h"
//#include <QtGui/QInputDialog>
//
//class WXDLLIMPEXP_CORE wxTextEntryDialog : public wxDialog
//{
//public:
//    wxTextEntryDialog(wxWindow *parent,
//                      const wxString& message,
//                      const wxString& caption = wxGetTextFromUserPromptStr,
//                      const wxString& value = wxEmptyString,
//                      long style = wxTextEntryDialogStyle,
//                      const wxPoint& pos = wxDefaultPosition);
//
//    void SetValue(const wxString& val);
//    const wxString &GetValue() const;
//
//#if wxUSE_VALIDATORS
//    void SetTextValidator( const wxTextValidator& validator );
//    void SetTextValidator( wxTextValidatorStyle style = wxFILTER_NONE );
//    wxTextValidator* GetTextValidator();
//#endif
//
//    virtual QInputDialog *GetHandle() const;
//
//private:
//    wxQtPointer< QInputDialog > m_qtInputDialog;
//};
//
//
//
//class WXDLLIMPEXP_CORE wxPasswordEntryDialog : public wxTextEntryDialog
//{
//public:
//    wxPasswordEntryDialog(wxWindow *parent,
//                      const wxString& message,
//                      const wxString& caption = wxGetPasswordFromUserPromptStr,
//                      const wxString& value = wxEmptyString,
//                      long style = wxTextEntryDialogStyle,
//                      const wxPoint& pos = wxDefaultPosition);
//};
//
//#endif // wxUSE_TEXTDLG

#endif // _WX_QT_TEXTDLG_H_
