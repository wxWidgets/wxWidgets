/////////////////////////////////////////////////////////////////////////////
// Name:        extdlgg.cpp
// Purpose:     extended generic dialog
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "extdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/intl.h"
#include "wx/dialog.h"
#include "wx/button.h"
#endif

#if wxUSE_STATLINE
#include "wx/statline.h"
#endif

#include "wx/generic/extdlgg.h"

//-----------------------------------------------------------------------------
// wxExtDialog
//-----------------------------------------------------------------------------

#define BUTTON_AREA_MARGIN   10

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxExtDialog, wxDialog)

BEGIN_EVENT_TABLE(wxExtDialog, wxDialog)
        EVT_SIZE(wxExtDialog::OnSize)
        EVT_BUTTON(wxID_YES, wxExtDialog::OnYes)
        EVT_BUTTON(wxID_NO, wxExtDialog::OnNo)
        EVT_BUTTON(wxID_CANCEL, wxExtDialog::OnCancel)
END_EVENT_TABLE()
#endif

wxExtDialog::wxExtDialog( wxWindow *parent, wxWindowID id,
        const wxString& title, long extraStyle,
        const wxPoint& pos,  const wxSize& size,
        long style, const wxString &name )
{
    Create( parent, id, title, extraStyle, pos, size, style, name );
}

bool wxExtDialog::Create( wxWindow *parent, wxWindowID id,
        const wxString& title, long extraStyle,
        const wxPoint& pos,  const wxSize& size,
        long style, const wxString &name )
{
    if (!wxDialog::Create( parent, id, title, pos, size, style, name ))
      return FALSE;
      
    m_extraStyle = extraStyle;
    
    m_clientWindowMargin = 10;
    
    if (m_windowStyle & wxED_BUTTONS_RIGHT)
    {
        m_spacePerButton.x = wxButton::GetDefaultSize().x + 18;
        m_spacePerButton.y = wxButton::GetDefaultSize().y + 8;
    }
    else
    {
        m_spacePerButton.x = wxButton::GetDefaultSize().x + 8;
        m_spacePerButton.y = wxButton::GetDefaultSize().y + 18;
    }

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    // Under Motif and GTK, the default button has a big frame around
    // it and to avoid overlapping buttons we make the margin bigger.
    // We could give other platforms a bigger margin as well, but this
    // wouldn't be standard L&F.
    m_spacePerButton.x += 10;
    m_spacePerButton.y += 10;
#endif
	
    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxButton *yes = (wxButton *) NULL;
    wxButton *no = (wxButton *) NULL;
    
    
    if (m_extraStyle & wxYES_NO) 
    {
        yes = new wxButton( this, wxID_YES, _("Yes") );
        m_buttons.Append( yes );
        no = new wxButton( this, wxID_NO, _("No") );
        m_buttons.Append( no );
    }

    if (m_extraStyle & wxYES) 
    {
        yes = new wxButton( this, wxID_YES, _("Yes") );
        m_buttons.Append( yes );
    }

    if (m_extraStyle & wxNO) 
    {
        no = new wxButton( this, wxID_NO, _("No") );
        m_buttons.Append( no );
    }

    if (m_extraStyle & wxOK) 
    {
        ok = new wxButton( this, wxID_OK, _("OK") );
        m_buttons.Append( ok );
    }

    if (m_extraStyle & wxFORWARD) 
        AddButton( new wxButton( this, wxID_FORWARD, _("Forward")  ) );

    if (m_extraStyle & wxBACKWARD) 
        AddButton( new wxButton( this, wxID_BACKWARD, _("Backward")  ) );

    if (m_extraStyle & wxSETUP) 
        AddButton( new wxButton( this, wxID_SETUP, _("Setup")  ) );

    if (m_extraStyle & wxMORE) 
        AddButton( new wxButton( this, wxID_MORE, _("More...")  ) );

    if (m_extraStyle & wxHELP)
        AddButton( new wxButton( this, wxID_HELP, _("Help")  ) );

    if (m_extraStyle & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
        m_buttons.Append( cancel );
    }

    if ((m_extraStyle & wxNO_DEFAULT) == 0)
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
    
#if wxUSE_STATLINE
    if (style & wxED_STATIC_LINE)
    {
        int line_style = wxLI_HORIZONTAL;
        if (style & wxED_BUTTONS_RIGHT) line_style = wxLI_VERTICAL;

        m_statLine = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, line_style );
    }
    else
        m_statLine = (wxStaticLine*) NULL;
#endif

    if (m_extraStyle & wxCENTRE)
        Centre( wxBOTH );
    
    return TRUE;
}

void wxExtDialog::AddButton( wxButton *button )
{
    m_buttons.Append( button );
}

void wxExtDialog::SetDefaultButton( wxWindowID button )
{
    wxNode *node = m_buttons.First();
    while (node)
    {
       wxButton *but = (wxButton*) node->Data();
       if (but->GetId() == button)
       {
           but->SetDefault();
           but->SetFocus();
	   return;
       }
    }
}

void wxExtDialog::EnableButton( wxWindowID button, bool enable )
{
    wxNode *node = m_buttons.First();
    while (node)
    {
       wxButton *but = (wxButton*) node->Data();
       if (but->GetId() == button)
       {
           but->Enable(enable);
	   return;
       }
    }
}

bool wxExtDialog::ButtonIsEnabled( wxWindowID button )
{
    wxNode *node = m_buttons.First();
    while (node)
    {
       wxButton *but = (wxButton*) node->Data();
       if (but->GetId() == button)
	   return but->IsEnabled();
    }
    return FALSE;
}

void wxExtDialog::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    wxSize client_size( GetClientSize() );
    wxSize button_area( LayoutButtons() );
    
    if (HasFlag(wxED_BUTTONS_RIGHT))
        client_size.x -= button_area.x;
    else
        client_size.y -= button_area.y;

    if (m_clientWindow)
    {
        if (m_windowStyle & wxED_CLIENT_MARGIN)
	  m_clientWindow->SetSize( m_clientWindowMargin,
	                           m_clientWindowMargin,
				   client_size.x - 2*m_clientWindowMargin,
				   client_size.y - 2*m_clientWindowMargin );
	else
          m_clientWindow->SetSize( 0, 0, client_size.x, client_size.y );
	
	if (m_clientWindow->GetAutoLayout())
	    m_clientWindow->Layout();
    }
}

void wxExtDialog::OnYes(wxCommandEvent& event)
{
    EndModal( wxID_YES );
}

void wxExtDialog::OnNo(wxCommandEvent& event)
{
    EndModal( wxID_NO );
}

void wxExtDialog::OnCancel(wxCommandEvent& event)
{
    /* allow cancellation via ESC/Close button except if
       only YES and NO are specified. */
    if ((m_extraStyle & wxYES_NO) != wxYES_NO || (m_extraStyle & wxCANCEL))
    {
        EndModal( wxID_CANCEL );
    }
}

wxSize wxExtDialog::GetButtonAreaSize()
{
    if (m_buttons.GetCount() == 0) return wxSize(0,0);
    
    wxSize ret(0,0);

    if (m_windowStyle & wxED_BUTTONS_RIGHT)
    {
	ret.x = m_spacePerButton.x;
        ret.y = m_buttons.GetCount()*m_spacePerButton.y + 2*BUTTON_AREA_MARGIN;
#if wxUSE_STATLINE
        if (m_statLine)
	    ret.x += wxStaticLine::GetDefaultSize();
#endif
    }
    else
    {
        ret.x = m_buttons.GetCount()*m_spacePerButton.x + 2*BUTTON_AREA_MARGIN;
	ret.y = m_spacePerButton.y;
#if wxUSE_STATLINE
        if (m_statLine)
	    ret.y += wxStaticLine::GetDefaultSize();
#endif
    }
    
    return ret;
}

wxSize wxExtDialog::LayoutButtons()
{
    if (m_buttons.GetCount() == 0) return wxSize(0,0);
    
    wxSize area_used( GetButtonAreaSize() );
    wxSize client_area( GetClientSize() );
    
    if (m_windowStyle & wxED_BUTTONS_RIGHT)
    {
        area_used.y = client_area.y;
	wxSize area_used_by_buttons( area_used );
#if wxUSE_STATLINE
        if (m_statLine)
	    area_used_by_buttons.x -= wxStaticLine::GetDefaultSize();
#endif

        int space_for_each_button = (client_area.y-2*BUTTON_AREA_MARGIN) / m_buttons.GetCount();
        int n = 0;
        wxNode *node = m_buttons.First();
        while (node)
        {
            wxButton *button = (wxButton*)node->Data();
	    
	    wxSize button_size( button->GetSize() );
	    if (button_size.x < wxButton::GetDefaultSize().x) button_size.x = wxButton::GetDefaultSize().x;
	    
	    int center_of_button_y = n*space_for_each_button + space_for_each_button/2;
	    int button_y = BUTTON_AREA_MARGIN + center_of_button_y - button_size.y/2;
	    
	    int center_of_button_x = client_area.x - area_used_by_buttons.x/2;
	    int button_x = center_of_button_x - button_size.x/2;
	    
            button->SetSize( button_x, button_y, button_size.x, button_size.y );
	    
            node = node->Next();
            n++;
       }
       
#if wxUSE_STATLINE
        if (m_statLine)
	    m_statLine->SetSize( client_area.x - area_used_by_buttons.x - wxStaticLine::GetDefaultSize(), 
	                         0, 
				 wxStaticLine::GetDefaultSize(), 
				 client_area.y );
#endif
    }
    else
    {
        area_used.x = client_area.x;
	wxSize area_used_by_buttons( area_used );
#if wxUSE_STATLINE
        if (m_statLine)
	    area_used_by_buttons.y -= wxStaticLine::GetDefaultSize();
#endif

        int space_for_each_button = (client_area.x-2*BUTTON_AREA_MARGIN) / m_buttons.GetCount();
        int n = 0;
        wxNode *node = m_buttons.First();
        while (node)
        {
            wxButton *button = (wxButton*)node->Data();
	    
	    wxSize button_size( button->GetSize() );
	    if (button_size.x < wxButton::GetDefaultSize().x) button_size.x = wxButton::GetDefaultSize().x;
	    
	    int center_of_button_x = n*space_for_each_button + space_for_each_button/2;
	    int button_x = BUTTON_AREA_MARGIN + center_of_button_x - button_size.x/2;
	    
	    int center_of_button_y = client_area.y - area_used_by_buttons.y/2;
	    int button_y = center_of_button_y - button_size.y/2;
	    
            button->SetSize( button_x, button_y, button_size.x, button_size.y );
	    
            node = node->Next();
            n++;
       }
       
#if wxUSE_STATLINE
        if (m_statLine)
	    m_statLine->SetSize( 0, 
	                         client_area.y - area_used_by_buttons.y - wxStaticLine::GetDefaultSize(),
				 client_area.x, 
				 wxStaticLine::GetDefaultSize() );
#endif
    }
    
    return area_used;
}


