/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/dialog.h
// Purpose:
// Author:      Robert Roebling
// Created:
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDIALOGH__
#define __GTKDIALOGH__

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDialog: public wxDialogBase
{
public:
    wxDialog() { Init(); }
    wxDialog( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );
    bool Create( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );
    virtual ~wxDialog() {}

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
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxDialog);
};

#endif // __GTKDIALOGH__
