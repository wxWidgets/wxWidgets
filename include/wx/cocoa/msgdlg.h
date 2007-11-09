/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/dirdlg.h
// Purpose:     wxMessageDialog class
// Author:      Gareth Simpson
// Created:     2007-10-29
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_MSGDLG_H_
#define _WX_COCOA_MSGDLG_H_

#include "wx/msgdlg.h"

DECLARE_WXCOCOA_OBJC_CLASS(NSAlert);

#ifndef wxUSE_COCOA_NATIVE_MSGDLG
// trunk: Always use Cocoa dialog
// 2.8: Only use Cocoa dialog if ABI incompatible features is on
// Build both on both branches (there was no wxCocoaMessageDialog class so it's not an ABI issue)
    #if wxUSE_ABI_INCOMPATIBLE_FEATURES
        #define wxUSE_COCOA_NATIVE_MSGDLG 1
    #else
        #define wxUSE_COCOA_NATIVE_MSGDLG 0
    #endif
#endif

#include "wx/generic/msgdlgg.h"

#if wxUSE_COCOA_NATIVE_MSGDLG
    #define wxMessageDialog wxCocoaMessageDialog
#else
    #define wxMessageDialog wxGenericMessageDialog
#endif

//-------------------------------------------------------------------------
// wxMsgDialog
//-------------------------------------------------------------------------



class WXDLLEXPORT wxCocoaMessageDialog: public wxDialog, public wxMessageDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxCocoaMessageDialog)
    DECLARE_NO_COPY_CLASS(wxCocoaMessageDialog)


public:
    wxCocoaMessageDialog(wxWindow *parent,
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

    // 2.8: These are in the base class in trunk:
    wxString m_message,
             m_extendedMessage,
             m_caption;
private:
    wxString m_yes,
             m_no,
             m_ok,
             m_cancel;

};

#endif // _WX_MSGDLG_H_

