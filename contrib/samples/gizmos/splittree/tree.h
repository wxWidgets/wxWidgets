/////////////////////////////////////////////////////////////////////////////
// Name:        tree.h
// Purpose:     Testing tree functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _TREE_H_
#define _TREE_H_

#include "wx/wx.h"
#include "wx/gizmos/splittree.h"

class TestValueWindow;

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

protected:
    wxRemotelyScrolledTreeCtrl*        m_tree;
    wxThinSplitterWindow*   m_splitter;
	wxSplitterScrolledWindow* m_scrolledWindow;
    TestValueWindow*		m_valueWindow;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 100,
    Minimal_About
};

#define idTREE_CTRL         2000
#define idSPLITTER_WINDOW   2001
#define idVALUE_WINDOW		2002
#define idMAIN_FRAME		2003
#define idSCROLLED_WINDOW   2004

class TestTree: public wxRemotelyScrolledTreeCtrl
{
	DECLARE_CLASS(TestTree)
public:
    TestTree(wxWindow* parent, wxWindowID id, const wxPoint& pt = wxDefaultPosition,
        const wxSize& sz = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
	~TestTree();
    DECLARE_EVENT_TABLE()
protected:
	wxImageList*	m_imageList;
};

class TestValueWindow: public wxWindow
{
public:
    TestValueWindow(wxWindow* parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& sz = wxDefaultSize,
      long style = 0);

//// Overrides

//// Events
	void OnSize(wxSizeEvent& event);
    
//// Data members
protected:

    DECLARE_EVENT_TABLE()
};



#endif
        // _TREE_H_
