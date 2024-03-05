/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dialog.h
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DIALOG_H_
#define _WX_QT_DIALOG_H_

#include "wx/dialog.h"
class QDialog;

class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
public:
    wxDialog();
    wxDialog( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );

    virtual ~wxDialog();

    bool Create( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );

    virtual int ShowModal() override;
    virtual void EndModal(int retCode) override;
    virtual bool IsModal() const override;
    virtual bool Show(bool show = true) override;

    QDialog *GetDialogHandle() const;

private:

    wxDECLARE_DYNAMIC_CLASS( wxDialog );
};


#endif // _WX_QT_DIALOG_H_
