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

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/panel.h"
#include "wx/icon.h"

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
    DECLARE_DYNAMIC_CLASS(wxDialog)

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
    ~wxDialog();

    void SetTitle(const wxString& title);
    wxString GetTitle() const;

    void OnApply( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );
    void OnOK( wxCommandEvent &event );
    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent &event );
    void OnCloseWindow( wxCloseEvent& event );
    /*
       void OnCharHook( wxKeyEvent& event );
     */

    bool Destroy();

    virtual bool Show( bool show );
    virtual int ShowModal();
    virtual void EndModal( int retCode );
    virtual bool IsModal() const;
    void SetModal( bool modal );

    virtual void InitDialog(void);

    virtual void SetIcon( const wxIcon &icon );
    virtual void Iconize( bool WXUNUSED(iconize)) { }
    virtual bool IsIconized() const { return FALSE; }
    bool Iconized() const { return IsIconized(); }
    virtual void Maximize() { }
    virtual void Restore() { }

    // implementation

    virtual void GtkOnSize( int x, int y, int width, int height );
    virtual void OnInternalIdle();

    bool       m_modalShowing;
    wxString   m_title;
    wxIcon     m_icon;

protected:
    // common part of all ctors
    void Init();

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

private:
    DECLARE_EVENT_TABLE()
};

#endif // __GTKDIALOGH__
