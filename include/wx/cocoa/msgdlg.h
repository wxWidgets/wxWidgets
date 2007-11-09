/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/dirdlg.h
// Purpose:     wxMessageDialog class
// Author:      Gareth Simpson
// Created:     2007-10-29
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_MSGDLG_H_
#define _WX_COCOA_MSGDLG_H_

#include "wx/msgdlg.h"

DECLARE_WXCOCOA_OBJC_CLASS(NSAlert);



#define wxMessageDialog wxCocoaMessageDialog
//-------------------------------------------------------------------------
// wxMsgDialog
//-------------------------------------------------------------------------



class WXDLLEXPORT wxMessageDialog: public wxMessageDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxMessageDialog)
    DECLARE_NO_COPY_CLASS(wxMessageDialog)


public:
    wxMessageDialog(wxWindow *parent,
                    const wxString& message,
                    const wxString& caption = wxMessageBoxCaptionStr,
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);
						
	virtual ~wxCocoaMessageDialog();

	virtual int ShowModal();
	
    // customization of the message box
    virtual bool SetYesNoLabels(const wxString& yes,const wxString& no);
    virtual bool SetYesNoCancelLabels(const wxString& yes, const wxString& no, const wxString& cancel);
    virtual bool SetOKLabel(const wxString& ok);
    virtual bool SetOKCancelLabels(const wxString& ok, const wxString& cancel);

protected:
    // not supported for message dialog
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) {}
	
private:
	wxString m_yes,
			 m_no,
			 m_ok,
			 m_cancel;

};

#endif // _WX_MSGDLG_H_

