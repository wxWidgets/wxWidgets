/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:
// Author:      Robert Roebling
// Created:
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDIALOGH__
#define __GTKDIALOGH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/defs.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDialog;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar *wxDialogNameStr;

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

class wxDialog: public wxDialogBase
{
public:
    wxDialog() { Init(); }
    wxDialog( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxDialogNameStr );
    bool Create( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxDialogNameStr );
    ~wxDialog() {}

    void OnApply( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );
    void OnOK( wxCommandEvent &event );
    void OnPaint( wxPaintEvent& event );
    void OnCloseWindow( wxCloseEvent& event );
    /*
       void OnCharHook( wxKeyEvent& event );
     */

    virtual bool Show( bool show = TRUE );
    virtual int ShowModal();
    virtual void EndModal( int retCode );
    virtual bool IsModal() const;
    void SetModal( bool modal );

    // implementation
    // --------------

    bool       m_modalShowing;

protected:
    // common part of all ctors
    void Init();

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxDialog)
};

#endif // __GTKDIALOGH__
