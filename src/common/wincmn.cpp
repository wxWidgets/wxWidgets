/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     common (to all ports) wxWindow functions
// Author:      Julian Smart, Vadim Zeitlin
// Modified by:
// Created:     13/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/frame.h"
#include "wx/defs.h"
#include "wx/window.h"

// Do Update UI processing for child controls

// TODO: should this be implemented for the child window rather
// than the parent? Then you can override it e.g. for wxCheckBox
// to do the Right Thing rather than having to assume a fixed number
// of control classes.
#include "wx/checkbox.h"
#include "wx/radiobut.h"

void wxWindow::UpdateWindowUI()
{
  wxWindowID id = GetId();
  if (id > 0)
  {
    wxUpdateUIEvent event(id);
    event.m_eventObject = this;

    if (this->GetEventHandler()->ProcessEvent(event))
    {
      if (event.GetSetEnabled())
        this->Enable(event.GetEnabled());

      if (event.GetSetText() && this->IsKindOf(CLASSINFO(wxControl)))
        ((wxControl*)this)->SetLabel(event.GetText());

      if (this->IsKindOf(CLASSINFO(wxCheckBox)))
      {
        if (event.GetSetChecked())
          ((wxCheckBox *) this)->SetValue(event.GetChecked());
      }
      // @@@ No radio buttons in wxGTK yet
#ifndef __WXGTK__
      else if (this->IsKindOf(CLASSINFO(wxRadioButton)))
      {
        if (event.GetSetChecked())
          ((wxRadioButton *) this)->SetValue(event.GetChecked());
      }
#endif
    }
  }
}
