/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlgg.cpp
// Purpose:     wxGenericMessageDialog
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Markus Holzem, Robert Roebling
// Licence:           wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/statbmp.h"
#include "wx/stattext.h"
#include "wx/layout.h"
#include "wx/intl.h"
#include "wx/bitmap.h"
#include "wx/app.h"
#endif

#include <stdio.h>
#include <string.h>

#include "wx/gtk/msgdlg.h"
#include "wx/statline.h"

///////////////////////////////////////////////////////////////////
// New dialog box implementations

// Split message, using constraints to position controls
wxSize wxSplitMessage2( const wxString &message, wxWindow *parent, int text_pos_x )
{
    int y = 15;
    int w = 50;
    wxString line( _T("") );
    for (uint pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( parent, -1, line, wxPoint(text_pos_x,y) );
                wxSize size1( s1->GetSize() );
                if (size1.x > w) w = size1.x;
                line = _T("");
            }
            y += 18;
        }
        else
        {
            line += message[pos];
        }
    }
    
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( parent, -1, line, wxPoint(text_pos_x,y) );
        wxSize size2( s2->GetSize() );
        if (size2.x > w) w = size2.x;
    }
        
    y += 18;
    
    return wxSize(w+15+text_pos_x,y);
}

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxGenericMessageDialog, wxDialog)
        EVT_BUTTON(wxID_YES, wxGenericMessageDialog::OnYes)
        EVT_BUTTON(wxID_NO, wxGenericMessageDialog::OnNo)
        EVT_BUTTON(wxID_CANCEL, wxGenericMessageDialog::OnCancel)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxGenericMessageDialog, wxDialog)
#endif

wxGenericMessageDialog::wxGenericMessageDialog( wxWindow *parent, const wxString& message, 
   const wxString& caption, long style, const wxPoint& pos) :
  wxDialog( parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
    m_dialogStyle = style;

    int text_pos_x = 15;
    
    if (m_dialogStyle & wxICON_MASK)
        text_pos_x += 80;

    wxSize message_size( wxSplitMessage2( message, this, text_pos_x ) );

    if (m_dialogStyle & wxICON_MASK)
    {
       if (message_size.y < 50) message_size.y = 50;
        (void) new wxStaticBitmap( this, -1,
                                   wxTheApp->GetStdIcon(m_dialogStyle
                                                        & wxICON_MASK),
                                   wxPoint(15,message_size.y/2-16) ); 
    }

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxButton *yes = (wxButton *) NULL;
    wxButton *no = (wxButton *) NULL;
    
    int y = message_size.y + 30;
    
    if (style & wxYES_NO) 
    {
        yes = new wxButton( this, wxID_YES, _("Yes"), wxPoint(-1,y), wxSize(80,-1) );
        m_buttons.Append( yes );
        no = new wxButton( this, wxID_NO, _("No"), wxPoint(-1,y), wxSize(80,-1) );
        m_buttons.Append( no );
    }

    if (style & wxOK) 
    {
        ok = new wxButton( this, wxID_OK, _("OK"), wxPoint(-1,y), wxSize(80,-1) );
        m_buttons.Append( ok );
    }

    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxPoint(-1,y), wxSize(80,-1) );
        m_buttons.Append( cancel );
    }

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
    
    int w = m_buttons.GetCount() * 100;
    if (message_size.x > w) w = message_size.x;
    int space = w / (m_buttons.GetCount()*2);
    
    int n = 0;
    wxNode *node = m_buttons.First();
    while (node)
    {
        wxWindow *win = (wxWindow*)node->Data();
        int x = (n*2+1)*space - 40 + 15;
        win->Move( x, -1 );
        node = node->Next();
        n++;
    }
    
#ifdef __WXGTK__
    (void) new wxStaticLine( this, -1, wxPoint(0,y-20), wxSize(w+30, 5) );
#endif
    
    SetSize( w+30, y+40 );

    Centre( wxBOTH );
}

void wxGenericMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_YES );
}

void wxGenericMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_NO );
}

void wxGenericMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    /* Allow cancellation via ESC/Close button except if
       only YES and NO are specified. */
    if ( (m_dialogStyle & wxYES_NO) != wxYES_NO || (m_dialogStyle & wxCANCEL) )
    {
        EndModal( wxID_CANCEL );
    }
}


