/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/msgdlg.h
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MSGDLG_H_
#define _WX_QT_MSGDLG_H_

#include "wx/msgdlg.h"

#include "wx/qt/winevent_qt.h"
#include <QtGui/QMessageBox>

class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog(wxWindow *parent, const wxString& message,
                    const wxString& caption = wxMessageBoxCaptionStr,
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);

    // Reimplemented to translate return codes from Qt to wx
    virtual int ShowModal();
    
    virtual QMessageBox *GetHandle() const;

private:
    wxQtPointer< QMessageBox > m_qtMessageBox;
};

class WXDLLIMPEXP_CORE wxQtMessageDialog : public wxQtEventSignalHandler< QMessageBox, wxMessageDialog >
{
    Q_OBJECT
    
    public:
        wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler );
};

#endif // _WX_QT_MSGDLG_H_
