/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:     wxDialog class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/settings.h"

#include <wx/mac/uma.h>

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxList wxModalDialogs;
wxList wxModelessWindows;  // Frames and modeless dialogs
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxPanel)

BEGIN_EVENT_TABLE(wxDialog, wxPanel)
	EVT_SIZE(wxDialog::OnSize)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
  EVT_CHAR_HOOK(wxDialog::OnCharHook)
  EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

#endif

wxDialog::wxDialog()
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_windowStyle = style;

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetName(name);
  
  if (!parent)
    wxTopLevelWindows.Append(this);

  if (parent) parent->AddChild(this);

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

	Rect theBoundsRect;

  m_x = (int)pos.x;
  m_y = (int)pos.y;
  if ( m_y < 50 )
  	m_y = 50 ;
  if ( m_x < 20 )
  	m_x = 20 ;
  	
  m_width = size.x;
	if (m_width == -1) 
		m_width = 20;
  m_height = size.y;
	if (m_height == -1) 
		m_height = 20;

	::SetRect(&theBoundsRect, m_x, m_y, m_x + m_width, m_y + m_height);
	m_macWindowData = new MacWindowData() ;

	WindowClass wclass = kMovableModalWindowClass ;
	WindowAttributes attr = kWindowNoAttributes ;
	
	if ( ( m_windowStyle & wxMINIMIZE_BOX ) || ( m_windowStyle & wxMAXIMIZE_BOX ) )
	{
		attr |= kWindowFullZoomAttribute ;
		attr |= kWindowResizableAttribute ;
	}

	UMACreateNewWindow( wclass , attr , &theBoundsRect , &m_macWindowData->m_macWindow ) ;
	wxAssociateWinWithMacWindow( m_macWindowData->m_macWindow , this ) ;
	wxString label ;
	if( wxApp::s_macDefaultEncodingIsPC )
		label = wxMacMakeMacStringFromPC( title ) ;
	else
		label = title ;
	UMASetWTitleC( m_macWindowData->m_macWindow , label ) ;
	m_macWindowData->m_macWindowBackgroundTheme = kThemeBrushDialogBackgroundActive ;
	UMACreateRootControl( m_macWindowData->m_macWindow , &m_macWindowData->m_macRootControl ) ;
	m_macWindowData->m_macFocus = NULL ;
	m_macShown = false ;
  return TRUE;
}

void wxDialog::SetModal(bool flag)
{
	if ( flag )
		m_windowStyle |= wxDIALOG_MODAL ;
	else
		if ( m_windowStyle & wxDIALOG_MODAL )
  			m_windowStyle -= wxDIALOG_MODAL ;
  
  wxModelessWindows.DeleteObject(this);
  if (!flag)
    wxModelessWindows.Append(this);
}

wxDialog::~wxDialog()
{
    wxTopLevelWindows.DeleteObject(this);

    if ( (GetWindowStyleFlag() & wxDIALOG_MODAL) != wxDIALOG_MODAL )
      wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
      wxTheApp->SetTopWindow(NULL);

      if (wxTheApp->GetExitOnFrameDelete())
      {
       	wxTheApp->ExitMainLoop() ;
      }
    }
}

// By default, pressing escape cancels the dialog
void wxDialog::OnCharHook(wxKeyEvent& event)
{
  if (event.m_keyCode == WXK_ESCAPE)
  {
		// Behaviour changed in 2.0: we'll send a Cancel message
		// to the dialog instead of Close.
		wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
		cancelEvent.SetEventObject( this );
		GetEventHandler()->ProcessEvent(cancelEvent);

		return;
  }
  // We didn't process this event.
  event.Skip();
}

void wxDialog::Iconize(bool WXUNUSED(iconize))
{
    // TODO
}

bool wxDialog::IsIconized() const
{
    // TODO
    return FALSE;
}

extern bool s_macIsInModalLoop ;

bool wxDialog::Show(bool show)
{
	if ( m_macShown == show )
		return TRUE ;

	m_macShown = show ;

  bool modal =  ((GetWindowStyleFlag() & wxDIALOG_MODAL) == wxDIALOG_MODAL) ;

	if ( modal )
	{
		s_macIsInModalLoop = true ;
	  if (show)
	  {
      // if we don't do it, some window might be deleted while we have pointers
      // to them in our disabledWindows list and the program will crash when it
      // will try to reenable them after the modal dialog end
      wxTheApp->DeletePendingObjects();

    	UMAShowWindow( m_macWindowData->m_macWindow ) ;
    	UMASelectWindow( m_macWindowData->m_macWindow ) ;

      if (!wxModalDialogs.Member(this))
        wxModalDialogs.Append(this);

      while (wxModalDialogs.Member(this) )
      {
      	wxTheApp->MacDoOneEvent() ;
      }
	  }
	  else
	  {
      wxModalDialogs.DeleteObject(this);
    	UMAHideWindow( m_macWindowData->m_macWindow ) ;
	  }
	  s_macIsInModalLoop = false ;
	}
	else // !modal
	{
	  if (show)
	  {
    	UMAShowWindow( m_macWindowData->m_macWindow ) ;
    	UMASelectWindow( m_macWindowData->m_macWindow ) ;
	  }
	  else
	  {
    	UMAHideWindow( m_macWindowData->m_macWindow ) ;
	  }
	}
	return TRUE ;
}


// Replacement for Show(TRUE) for modal dialogs - returns return code
int wxDialog::ShowModal()
{
  m_windowStyle |= wxDIALOG_MODAL;
	Show(TRUE);
	return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
	SetReturnCode(retCode);
    // TODO modal un-showing
	Show(FALSE);
}

// Standard buttons
void wxDialog::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
		    SetReturnCode(wxID_OK);
		    this->Show(FALSE);
        }
	}
}

void wxDialog::OnApply(wxCommandEvent& event)
{
	if (Validate())
		TransferDataFromWindow();
	// TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& event)
{
    if ( IsModal() )
        EndModal(wxID_CANCEL);
    else
    {
        SetReturnCode(wxID_CANCEL);
		this->Show(FALSE);
    }
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.

    static wxList closing;
    
    if ( closing.Member(this) )
        return;
    
    closing.Append(this);
    
    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent); // This may close the dialog

    closing.DeleteObject(this);
}

// Destroy the window (delayed, if a managed window)
bool wxDialog::Destroy()
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);
  return TRUE;
}

void wxDialog::OnSize(wxSizeEvent& WXUNUSED(event))
{
  // if we're using constraints - do use them
  #if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() ) {
      Layout();
    }
  #endif
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& event)
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  Refresh();
}

void wxDialog::Fit()
{
  wxWindow::Fit();
}

wxSizer *wxDialog::CreateTextSizer( const wxString &message )
{
    wxBoxSizer *box = new wxBoxSizer( wxVERTICAL );
    
    // get line height for empty lines
    int y = 0;
    wxFont new_font( GetFont() );
    if (!new_font.Ok()) new_font = *wxSWISS_FONT;
    GetTextExtent( "H", (int*)NULL, &y, (int*)NULL, (int*)NULL, &new_font );
    
    wxString line;
    for (size_t pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( this, -1, line );
		box->Add( s1 );
                line = _T("");
            }
	    else
	    {
	        box->Add( 5, y );
	    }
        }
        else
        {
            line += message[pos];
        }
    }
    
    // remaining text behind last '\n'
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( this, -1, line );
	box->Add( s2 );
    }
    
    return box;
}
    
wxSizer *wxDialog::CreateButtonSizer( long flags )
{
    wxBoxSizer *box = new wxBoxSizer( wxHORIZONTAL );

#if defined(__WXMSW__) || defined(__WXMAC__)
    int margin = 6;
#else
    int margin = 10;
#endif

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxButton *yes = (wxButton *) NULL;
    wxButton *no = (wxButton *) NULL;

    // always show an OK button, unless only YES_NO is given
    if ((flags & wxYES_NO) == 0) flags = flags | wxOK;
    
    if (flags & wxYES_NO) 
    {
        yes = new wxButton( this, wxID_YES, _("Yes") );
        box->Add( yes, 0, wxLEFT|wxRIGHT, margin );
        no = new wxButton( this, wxID_NO, _("No") );
        box->Add( no, 0, wxLEFT|wxRIGHT, margin );
    } else 
    if (flags & wxYES) 
    {
        yes = new wxButton( this, wxID_YES, _("Yes") );
        box->Add( yes, 0, wxLEFT|wxRIGHT, margin );
    } else 
    if (flags & wxNO) 
    {
        no = new wxButton( this, wxID_NO, _("No") );
        box->Add( no, 0, wxLEFT|wxRIGHT, margin );
    }

    if (flags & wxOK) 
    {
        ok = new wxButton( this, wxID_OK, _("OK") );
        box->Add( ok, 0, wxLEFT|wxRIGHT, margin );
    }

    if (flags & wxFORWARD) 
        box->Add( new wxButton( this, wxID_FORWARD, _("Forward")  ), 0, wxLEFT|wxRIGHT, margin ); 

    if (flags & wxBACKWARD) 
        box->Add( new wxButton( this, wxID_BACKWARD, _("Backward")  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxSETUP) 
        box->Add( new wxButton( this, wxID_SETUP, _("Setup")  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxMORE) 
        box->Add( new wxButton( this, wxID_MORE, _("More...")  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxHELP)
        box->Add( new wxButton( this, wxID_HELP, _("Help")  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
        box->Add( cancel, 0, wxLEFT|wxRIGHT, margin );
    }

    if ((flags & wxNO_DEFAULT) == 0)
    {
        if (ok)
        {
            ok->SetDefault();
            ok->SetFocus();
        }
        else if (yes)
        {
            yes->SetDefault();
            yes->SetFocus();
        }
    }
    
    return box;
}

