/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/msgdlg.h
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MSGDLG_H_
#define _WX_QT_MSGDLG_H_

#include "wx/msgdlg.h"

class QMessageBox;

class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog(wxWindow *parent, const wxString& message,
                    const wxString& caption = wxASCII_STR(wxMessageBoxCaptionStr),
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);
    virtual ~wxMessageDialog();

    // Reimplemented to translate return codes from Qt to wx
    virtual int ShowModal() override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMessageDialog);
};

#endif // _WX_QT_MSGDLG_H_
