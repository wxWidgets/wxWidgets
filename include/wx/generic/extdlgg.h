/////////////////////////////////////////////////////////////////////////////
// Name:        extdlgg.h
// Purpose:     Extended Generic dialogs
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __EXTDLGH_G__
#define __EXTDLGH_G__

#ifdef __GNUG__
    #pragma interface "extdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"
#include "wx/button.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxExtDialog;
class WXDLLEXPORT wxStaticLine;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar *wxDialogNameStr;

//-----------------------------------------------------------------------------
// wxExtDialog
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxExtDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxExtDialog)

public:
    wxExtDialog() {}
    wxExtDialog( wxWindow *parent, wxWindowID id,
                 const wxString& title,
		 long extraStyle = (wxOK|wxCANCEL|wxCENTRE),
                 const wxPoint& pos = wxDefaultPosition, 
	         const wxSize& size = wxDefaultSize,
                 long style = wxEXT_DIALOG_STYLE,
                 const wxString &name = wxDialogNameStr );
    bool Create( wxWindow *parent, wxWindowID id,
                 const wxString& title,
		 long extraStyle = (wxOK|wxCANCEL|wxCENTRE),
                 const wxPoint& pos = wxDefaultPosition, 
	         const wxSize& size = wxDefaultSize,
                 long style = wxEXT_DIALOG_STYLE,
                 const wxString &name = wxDialogNameStr );

    void SetClientWindow( wxWindow *clientWindow ) 
        { m_clientWindow = clientWindow; }
    wxWindow *GetClientWindow() 
        { return m_clientWindow; }
    void SetClientWindowMargin( int margin )
        { m_clientWindowMargin = margin; }
    int GetClientWindowMargin()
        { return m_clientWindowMargin; }
    void SetSpacePerButton( wxSize space )
        { m_spacePerButton = space; }
    wxSize GetSpacePerButton()
        { return m_spacePerButton; }

    void AddButton( wxButton *button );

    void SetDefaultButton( wxWindowID button );

    void EnableButton( wxWindowID button, bool enable=TRUE );
    bool ButtonIsEnabled( wxWindowID button );
    
    wxSize GetButtonAreaSize();
    
    void OnSize( wxSizeEvent &event );
    
    void OnYes(wxCommandEvent& event);
    void OnNo(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    
protected:
    wxWindow      *m_clientWindow;
    long           m_extraStyle;
    wxList         m_buttons;
#if wxUSE_STATLINE
    wxStaticLine  *m_statLine;
#endif
    int            m_clientWindowMargin;    
    wxSize         m_spacePerButton;

    wxSize LayoutButtons();

private:
    DECLARE_EVENT_TABLE()
};


#endif
