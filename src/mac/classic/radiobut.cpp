/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/radiobut.cpp
// Purpose:     wxRadioButton
// Author:      AUTHOR
// Modified by: JS Lair (99/11/15) adding the cyclic groupe notion for radiobox
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/radiobut.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

#include "wx/mac/uma.h"

bool wxRadioButton::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1,
          kControlRadioButtonProc , (long) this ) ;

    MacPostControlCreate() ;

    m_cycle = this ;

    if (HasFlag(wxRB_GROUP))
    {
        AddInCycle( NULL ) ;
    }
    else
    {
        /* search backward for last group start */
        wxRadioButton *chief = (wxRadioButton*) NULL;
        wxWindowList::Node *node = parent->GetChildren().GetLast();
        while (node)
        {
            wxWindow *child = node->GetData();
            if (child->IsKindOf( CLASSINFO( wxRadioButton ) ) )
            {
                chief = (wxRadioButton*) child;
                if (child->HasFlag(wxRB_GROUP)) break;
            }
            node = node->GetPrevious();
        }
        AddInCycle( chief ) ;
    }
    return true;
}

void wxRadioButton::SetValue(bool val)
{
    wxRadioButton *cycle;
      if ( GetControl32BitValue( (ControlHandle) m_macControl ) == val )
        return ;

   ::SetControl32BitValue( (ControlHandle) m_macControl , val ) ;
   if (val)
   {
           cycle=this->NextInCycle();
          if (cycle!=NULL) {
               while (cycle!=this) {
                   cycle->SetValue(false);
                   cycle=cycle->NextInCycle();
                   }
               }
           }
   MacRedrawControl() ;
}

bool wxRadioButton::GetValue() const
{
    return ::GetControl32BitValue( (ControlHandle) m_macControl ) ;
}

void wxRadioButton::Command (wxCommandEvent & event)
{
  SetValue ( (event.GetInt() != 0) );
  ProcessCommand (event);
}

void wxRadioButton::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED(mouseStillDown))
{
    if ( GetValue() )
        return ;

    wxRadioButton *cycle, *old = NULL ;
    cycle=this->NextInCycle();
    if (cycle!=NULL) {
          while (cycle!=this) {
            if ( cycle->GetValue() ) {
              old = cycle ;
                cycle->SetValue(false);
              }
              cycle=cycle->NextInCycle();
            }
    }

      SetValue(true) ;

    if ( old ) {
      wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, old->m_windowId );
      event.SetEventObject(old);
      event.SetInt( false );
      old->ProcessCommand(event);
    }
    wxCommandEvent event2(wxEVT_COMMAND_RADIOBUTTON_SELECTED, m_windowId );
    event2.SetEventObject(this);
    event2.SetInt( true );
    ProcessCommand(event2);
}

wxRadioButton *wxRadioButton::AddInCycle(wxRadioButton *cycle)
{
    wxRadioButton *next,*current;

    if (cycle==NULL) {
        m_cycle=this;
        return(this);
        }
    else {
        current=cycle;
          while ((next=current->m_cycle)!=cycle)
            current=current->m_cycle;
          m_cycle=cycle;
          current->m_cycle=this;
          return(cycle);
      }
}
