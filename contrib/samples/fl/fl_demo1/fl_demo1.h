/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: Sebastian Haase (June 21, 2001)
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __NEW_TEST_G__
#define __NEW_TEST_G__

#ifdef __GNUG__
#pragma interface "fl_demo1.h"
#endif

#define NEW_TEST_SAVE 1101
#define NEW_TEST_LOAD 1102
#define NEW_TEST_EXIT 1103

#include "wx/panel.h"

// Define a new application type
class MyApp: public wxApp
{ 
public:
    bool OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ 
public:
    wxFrameLayout*  mpLayout;
    wxTextCtrl*     mpClientWnd;
    
    wxTextCtrl* CreateTextCtrl( const wxString& value );
    
public:
    MyFrame(wxFrame *frame);
    virtual ~MyFrame();
    
    bool OnClose(void) { Show(FALSE); return TRUE; }
    
    void OnLoad( wxCommandEvent& event );
    void OnSave( wxCommandEvent& event );
    void OnExit( wxCommandEvent& event );
    
    DECLARE_EVENT_TABLE()
};

#endif

