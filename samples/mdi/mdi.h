/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/toolbar.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc);

    bool IsDirty() const { return m_dirty; }

    void OnEvent(wxMouseEvent& event);

    void SetText(const wxString& text) { m_text = text; Refresh(); }

private:
    wxString m_text;

    bool m_dirty;

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MyFrame : public wxMDIParentFrame
{
public:
    wxTextCtrl *textWindow;

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

class MyChild: public wxMDIChildFrame
{
public:
    MyCanvas *canvas;
    MyChild(wxMDIParentFrame *parent, const wxString& title);
    ~MyChild();

    void OnActivate(wxActivateEvent& event);

    void OnRefresh(wxCommandEvent& event);
    void OnUpdateRefresh(wxUpdateUIEvent& event);
    void OnChangeTitle(wxCommandEvent& event);
    void OnChangePosition(wxCommandEvent& event);
    void OnChangeSize(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnClose(wxCloseEvent& event);

#if wxUSE_CLIPBOARD
    void OnPaste(wxCommandEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
#endif // wxUSE_CLIPBOARD

    DECLARE_EVENT_TABLE()
};

// menu items ids
enum
{
    MDI_QUIT = wxID_EXIT,
    MDI_NEW_WINDOW = 101,
    MDI_REFRESH,
    MDI_CHANGE_TITLE,
    MDI_CHANGE_POSITION,
    MDI_CHANGE_SIZE,
    MDI_CHILD_QUIT,
    MDI_ABOUT = wxID_ABOUT
};
