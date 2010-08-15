/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/richmsgdlgg.h
// Purpose:     wxGenericRichMessageDialog
// Author:      Rickard Westerlund
// Created:     2010-07-04
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_RICHMSGDLGG_H_
#define _WX_GENERIC_RICHMSGDLGG_H_

class WXDLLIMPEXP_FWD_CORE wxCollapsiblePane;
class WXDLLIMPEXP_FWD_CORE wxCollapsiblePaneEvent;

class WXDLLIMPEXP_CORE wxGenericRichMessageDialog
                       : public wxRichMessageDialogBase
{
public:
    wxGenericRichMessageDialog( wxWindow *parent,
                                const wxString& message,
                                const wxString& caption,
                                long style )
        : wxRichMessageDialogBase( parent, message, caption, style ),
          m_checkBox( NULL ),
          m_detailsExpander( NULL )
    { }

    void OnPaneChanged(wxCollapsiblePaneEvent& event);

    // overrides methods in base class
    virtual int ShowModal() { return wxGenericMessageDialog::ShowModal(); }

    // overrides the method in wxGenericMessageDialog so the dialog is properly
    // shown.
    virtual bool Show(bool show) { return wxDialog::Show( show ); }

    virtual bool IsCheckBoxChecked() const
    {
        // dialog has not been shown yet
        if ( m_checkBox == NULL )
            return m_checkBoxValue;

        return m_checkBox->IsChecked();
    }

protected:
    wxCheckBox *m_checkBox;
    wxCollapsiblePane *m_detailsExpander;

    // overrides methods in the base class
    virtual void AddMessageDialogCheckBox(wxSizer *sizer);
    virtual void AddMessageDialogDetails(wxSizer *sizer);

private:
    DECLARE_EVENT_TABLE()

    wxDECLARE_NO_COPY_CLASS(wxGenericRichMessageDialog);
};

#endif // _WX_GENERIC_RICHMSGDLGG_H_
