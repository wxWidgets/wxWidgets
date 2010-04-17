/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/textdlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TEXTDLG_H_
#define _WX_QT_TEXTDLG_H_

#if wxUSE_TEXTDLG

#if wxUSE_VALIDATORS
#include "wx/valtext.h"
#include "wx/textctrl.h"
#endif

#include "wx/dialog.h"
#include <QtGui/QInputDialog>

class WXDLLIMPEXP_CORE wxTextEntryDialog : public wxDialog
{
public:
    wxTextEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);

    void SetValue(const wxString& val);
    const wxString &GetValue() const;

#if wxUSE_VALIDATORS
    void SetTextValidator( const wxTextValidator& validator );
    void SetTextValidator( wxTextValidatorStyle style = wxFILTER_NONE );
    wxTextValidator* GetTextValidator();
#endif

protected:
    virtual QInputDialog *GetHandle() const;
    
private:
};



class WXDLLIMPEXP_CORE wxPasswordEntryDialog : public wxTextEntryDialog
{
public:
    wxPasswordEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxGetPasswordFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);
};

#endif // wxUSE_TEXTDLG

#endif // _WX_QT_TEXTDLG_H_
