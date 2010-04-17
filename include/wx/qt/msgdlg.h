/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/msgdlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MSGDLG_H_
#define _WX_QT_MSGDLG_H_

#include <QtGui/QMessageBox>

class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog();
    wxMessageDialog(wxWindow *parent, const wxString& message,
                    const wxString& caption = wxMessageBoxCaptionStr,
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);
protected:
    virtual QMessageBox *GetHandle() const;
};

#endif // _WX_QT_MSGDLG_H_
