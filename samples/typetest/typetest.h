/////////////////////////////////////////////////////////////////////////////
// Name:        typetest.h
// Purpose:     Types wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "typetest.h"
#endif

#ifndef _WX_TYPETEST_H_
#define _WX_TYPETEST_H_

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit(void);

    void DoDateDemo(wxCommandEvent& event);
    void DoTimeDemo(wxCommandEvent& event);
    void DoVariantDemo(wxCommandEvent& event);

    wxTextCtrl*     GetTextCtrl() const { return m_textCtrl; }

private:
    wxTextCtrl*     m_textCtrl;

DECLARE_DYNAMIC_CLASS(MyApp)
DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyApp)

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent, const wxString& title,
            const wxPoint& pos, const wxSize& size);
    
public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	bool OnClose(void) { return TRUE; }

   DECLARE_EVENT_TABLE()
    
};

// ID for the menu commands
#define TYPES_QUIT 	    wxID_EXIT
#define TYPES_TEXT 	    101
#define TYPES_ABOUT     102

#define TYPES_DATE      103
#define TYPES_TIME      104
#define TYPES_VARIANT   105

#endif
    // _WX_TYPETEST_H_

