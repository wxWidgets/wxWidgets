/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDIALOGH__
#define __GTKDIALOGH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/event.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// forward decls
//-----------------------------------------------------------------------------

class wxRadioBox;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDialog;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxDialogNameStr;

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

class wxDialog: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxDialog)

  public:

    wxDialog(void);
    wxDialog( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr );
    bool Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr );
    ~wxDialog(void);
    void SetTitle(const wxString& title);
    wxString GetTitle(void) const;
    bool OnClose(void);
    void OnApply( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnPaint(wxPaintEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
/*
    void OnCharHook(wxKeyEvent& event);
*/
    virtual bool Show( bool show );
    virtual int ShowModal(void);
    virtual void EndModal(int retCode);
    virtual bool IsModal(void) const { return ((GetWindowStyleFlag() & wxDIALOG_MODAL) == wxDIALOG_MODAL); }
    virtual void InitDialog(void);
/*
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
*/
    
  private:
  
    friend    wxWindow;
    friend    wxDC;
    friend    wxRadioBox;
    bool       m_modalShowing;
    wxString   m_title;
    
  DECLARE_EVENT_TABLE()
    
};

#endif // __GTKDIALOGH__
