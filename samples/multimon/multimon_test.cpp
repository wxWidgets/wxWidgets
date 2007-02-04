/////////////////////////////////////////////////////////////////////////////
// Name:        multimon_test.cpp
// Purpose:     tests wxDisplay class
// Author:      Royce Mitchell III
// Modified by:
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/display.h"

class TestApp : public wxApp
{
    bool OnInit();
};

DECLARE_APP(TestApp)
IMPLEMENT_APP(TestApp)

bool TestApp::OnInit()
{
  if ( !wxApp::OnInit() )
      return false;

  bool is_use_display =
                        #if wxUSE_DISPLAY
                        true
                        #else
                        false
                        #endif
                        ;
  if( !is_use_display )
  {
    wxMessageBox( _T("This sample has to be compiled with wxUSE_DISPLAY"), _T("Building error"), wxOK);
  }
#if wxUSE_DISPLAY
  else
  {
    unsigned count = wxDisplay::GetCount();
    wxLogDebug ( _T("I detected %u display(s) on your system"), count );
    for (unsigned i = 0; i < count; i++)
    {
        wxDisplay display ( i );
        wxRect r = display.GetGeometry();
        wxLogDebug ( _T("Display #%u \"%s\" = ( %i, %i, %i, %i ) @ %i bits"),
            i, display.GetName().c_str(), r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
            display.GetCurrentMode().GetDepth() );
    }
  }
#endif
  return false;
}
