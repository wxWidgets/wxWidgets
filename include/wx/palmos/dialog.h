/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/dialog.h
// Purpose:     wxDialog class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "dialog.h"
#endif

#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxDialogNameStr;

class WXDLLEXPORT wxDialogModalData;

// Dialog boxes
class WXDLLEXPORT wxDialog : public wxDialogBase
{
public:
    wxDialog() { Init(); }

    // full ctor
    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxDialogNameStr);

    virtual ~wxDialog();

    // return true if we're showing the dialog modally
    virtual bool IsModal() const { return m_modalData != NULL; }

    // show the dialog modally and return the value passed to EndModal()
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    // implementation only from now on
    // -------------------------------

    // override some base class virtuals
    virtual bool Show(bool show = TRUE);

    virtual void Raise();

    // event handlers
    void OnCharHook(wxKeyEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    // Standard buttons
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Windows callbacks
    WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    // obsolete methods
    // ----------------

    // use the other ctor
    wxDEPRECATED( wxDialog(wxWindow *parent,
             const wxString& title, bool modal,
             int x = -1, int y= -1, int width = 500, int height = 500,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr) );

    // just call Show() or ShowModal()
    wxDEPRECATED( void SetModal(bool flag) );

    // use IsModal()
    wxDEPRECATED( bool IsModalShowing() const );

protected:
    // find the window to use as parent for this dialog if none has been
    // specified explicitly by the user
    //
    // may return NULL
    wxWindow *FindSuitableParent() const;

    // common part of all ctors
    void Init();

private:
    wxWindow*   m_oldFocus;
    bool        m_endModalCalled; // allow for closing within InitDialog

    // this pointer is non-NULL only while the modal event loop is running
    wxDialogModalData *m_modalData;


    DECLARE_DYNAMIC_CLASS(wxDialog)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxDialog)
};

#endif
    // _WX_DIALOG_H_
