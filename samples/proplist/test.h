/////////////////////////////////////////////////////////////////////////////
// Name:        test.h
// Purpose:     Property sheet sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "test.h"
#endif

#ifndef _PROPTEST_H_
#define _PROPTEST_H_

#include "wx/proplist.h"
#include "wx/propform.h"

class MyChild;

// Define a new application
class MyFrame;
class MyApp: public wxApp
{
public:
    MyApp(void);
    bool OnInit(void);

    void RegisterValidators(void);
    void PropertyListTest(bool useDialog);
    void PropertyFormTest(bool useDialog);

    MyFrame*    m_mainFrame;
    wxWindow*   m_childWindow;
};

DECLARE_APP(MyApp)

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long type);

    void OnCloseWindow(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnDialogList(wxCommandEvent& event);
    void OnFrameList(wxCommandEvent& event);
    void OnDialogForm(wxCommandEvent& event);
    void OnFrameForm(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

class PropListFrame: public wxPropertyListFrame
{
public:
  PropListFrame(wxPropertyListView *v, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame"):
        wxPropertyListFrame(v, parent, title, pos, size, style, name)
    {
    }

    void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

class PropListDialog: public wxPropertyListDialog
{
public:
  PropListDialog(wxPropertyListView *v, wxWindow *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox"):
        wxPropertyListDialog(v, parent, title, pos, size, style, name)
   {
   }

    void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

class PropFormFrame: public wxPropertyFormFrame
{
public:
  PropFormFrame(wxPropertyFormView *v, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame"):
    wxPropertyFormFrame(v, parent, title, pos, size, style, name)
    {
    }

    void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

class PropFormDialog: public wxPropertyFormDialog
{
public:
  PropFormDialog(wxPropertyFormView *v, wxWindow *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox"):
      wxPropertyFormDialog(v, parent, title, pos, size, style, name)
    {
    }

    void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

#define PROPERTY_QUIT                   1
#define PROPERTY_ABOUT                  2
#define PROPERTY_TEST_DIALOG_LIST       3
#define PROPERTY_TEST_FRAME_LIST        4
#define PROPERTY_TEST_DIALOG_FORM       5
#define PROPERTY_TEST_FRAME_FORM        6

#endif

