/////////////////////////////////////////////////////////////////////////////
// Name:        resource.h
// Purpose:     Dialog resource sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void) ;
    bool OnInit(void);
};

class MyPanel: public wxPanel
{
  public:
    MyPanel( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
             int style, const wxString &name );
    void OnClick(wxMouseEvent &event);

 DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
  public:
    wxWindow *panel;
    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);
    bool OnClose(void);
    void OnQuit(wxCommandEvent& event);
    void OnTest1(wxCommandEvent& event);

 DECLARE_EVENT_TABLE()
};

class MyDialog : public wxDialog
{
  public:
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

 DECLARE_EVENT_TABLE()
};

#define RESOURCE_QUIT       4
#define RESOURCE_TEST1      2

/*
#define RESOURCE_OK         1
#define RESOURCE_CANCEL     2
*/
