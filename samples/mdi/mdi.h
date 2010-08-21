/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/toolbar.h"

// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc);

    bool IsDirty() const { return m_dirty; }

    void SetText(const wxString& text) { m_text = text; Refresh(); }

private:
    void OnEvent(wxMouseEvent& event);

    wxString m_text;

    bool m_dirty;

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MyFrame : public wxMDIParentFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

    static wxMenuBar *CreateMainMenubar();

private:
    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnFullScreen(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnCloseAll(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);

    wxTextCtrl *m_textWindow;

    DECLARE_EVENT_TABLE()
};

class MyChild : public wxMDIChildFrame
{
public:
    MyChild(wxMDIParentFrame *parent);
    virtual ~MyChild();

    static unsigned GetChildrenCount() { return ms_numChildren; }

private:
    void OnActivate(wxActivateEvent& event);

    void OnRefresh(wxCommandEvent& event);
    void OnUpdateRefresh(wxUpdateUIEvent& event);
    void OnChangeTitle(wxCommandEvent& event);
    void OnChangePosition(wxCommandEvent& event);
    void OnChangeSize(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

#if wxUSE_CLIPBOARD
    void OnPaste(wxCommandEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
#endif // wxUSE_CLIPBOARD

    static unsigned ms_numChildren;

    MyCanvas *m_canvas;

    // simple test event handler class
    class EventHandler : public wxEvtHandler
    {
    public:
        EventHandler(unsigned numChild) : m_numChild(numChild) { }

    private:
        void OnRefresh(wxCommandEvent& event)
        {
            wxLogMessage("Child #%u refreshed.", m_numChild);
            event.Skip();
        }

        const unsigned m_numChild;

        DECLARE_EVENT_TABLE()

        wxDECLARE_NO_COPY_CLASS(EventHandler);
    };

    DECLARE_EVENT_TABLE()
};

// menu items ids
enum
{
    MDI_FULLSCREEN = 100,
    MDI_REFRESH,
    MDI_CHANGE_TITLE,
    MDI_CHANGE_POSITION,
    MDI_CHANGE_SIZE
};
