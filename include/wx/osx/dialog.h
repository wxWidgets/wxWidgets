/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dialog.h
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#include "wx/panel.h"

class WXDLLIMPEXP_FWD_CORE wxMacToolTip ;
class WXDLLIMPEXP_FWD_CORE wxModalEventLoop ;

// Dialog boxes
class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
    wxDECLARE_DYNAMIC_CLASS(wxDialog);

public:
    wxDialog() { Init(); }

    // Constructor with no modal flag - the new convention.
    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Init();
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxDialogNameStr);

    virtual ~wxDialog();

//    virtual bool Destroy();
    virtual bool Show(bool show = true);

    // return true if we're showing the dialog modally
    virtual bool IsModal() const;

    // show the dialog modally and return the value passed to EndModal()
    virtual int ShowModal();

    virtual void ShowWindowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    static bool OSXHasModalDialogsOpen();
    void OSXBeginModalDialog();
    void OSXEndModalDialog();
    
#if wxOSX_USE_COCOA
    bool OSXGetWorksWhenModal();
    void OSXSetWorksWhenModal(bool worksWhenModal);
#endif

    // implementation
    // --------------

    wxDialogModality GetModality() const;

#if wxOSX_USE_COCOA
    virtual void ModalFinishedCallback(void* WXUNUSED(panel), int WXUNUSED(returnCode)) {}
#endif

protected:
    // show window modal dialog
    void DoShowWindowModal();

    // end window modal dialog.
    void EndWindowModal();

    // mac also takes command-period as cancel
    virtual bool IsEscapeKey(const wxKeyEvent& event);


    wxDialogModality m_modality;

    wxModalEventLoop* m_eventLoop;

private:
    void Init();
    
    static wxVector<wxDialog*> s_modalStack;
#if wxOSX_USE_COCOA
    static wxVector<bool> s_modalWorksStack;
#endif
};

#endif
    // _WX_DIALOG_H_
