/////////////////////////////////////////////////////////////////////////////
// Name:        treetest.h
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);

    wxImageList *m_imageListNormal;
};

class MyTreeCtrl: public wxTreeCtrl
{
public:
   MyTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style):
        wxTreeCtrl(parent, id, pos, size, style)
   {
   }

	void OnBeginDrag(wxTreeEvent& event);
	void OnBeginRDrag(wxTreeEvent& event);
	void OnBeginLabelEdit(wxTreeEvent& event);
	void OnEndLabelEdit(wxTreeEvent& event);
	void OnDeleteItem(wxTreeEvent& event);
	void OnGetInfo(wxTreeEvent& event);
	void OnSetInfo(wxTreeEvent& event);
	void OnItemExpanded(wxTreeEvent& event);
	void OnItemExpanding(wxTreeEvent& event);
	void OnSelChanged(wxTreeEvent& event);
	void OnSelChanging(wxTreeEvent& event);
	void OnKeyDown(wxTreeEvent& event);

   DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyTreeCtrl *m_treeCtrl;
    wxTextCtrl *m_logWindow;

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    ~MyFrame(void);
    
 public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	bool OnClose(void) { return TRUE; }

   DECLARE_EVENT_TABLE()
};


// ID for the menu quit command
#define TREE_QUIT 	1
#define TREE_ABOUT 	102

#define TREE_CTRL   1000


