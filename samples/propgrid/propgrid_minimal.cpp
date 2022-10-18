/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/propgrid_minimal.cpp
// Purpose:     Minimal portion of wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-23
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"

class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow* parent);

    void OnAction(wxCommandEvent& event);
    void OnPropertyGridChange(wxPropertyGridEvent& event);
    void OnPropertyGridChanging(wxPropertyGridEvent& event);

private:
    wxPropertyGrid* m_pg;
    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_ACTION = wxID_HIGHEST+1
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_ACTION, MyFrame::OnAction)
    EVT_PG_CHANGED( -1, MyFrame::OnPropertyGridChange )
    EVT_PG_CHANGING( -1, MyFrame::OnPropertyGridChanging )
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "PropertyGrid Test")
{
    wxMenu *Menu = new wxMenu;
    Menu->Append(ID_ACTION, "Action");
    wxMenuBar *MenuBar = new wxMenuBar();
    MenuBar->Append(Menu, "Action");
    SetMenuBar(MenuBar);

    wxPropertyGrid *pg = new wxPropertyGrid(this,wxID_ANY,wxDefaultPosition,wxSize(400,400),
                        wxPG_SPLITTER_AUTO_CENTER |
                        wxPG_BOLD_MODIFIED );
    m_pg = pg;

    pg->Append( new wxStringProperty("String Property", wxPG_LABEL) );
    pg->Append( new wxIntProperty("Int Property", wxPG_LABEL) );
    pg->Append( new wxBoolProperty("Bool Property", wxPG_LABEL) );

    SetSize(400, 600);
}

void MyFrame::OnPropertyGridChange(wxPropertyGridEvent &event)
{
    wxPGProperty* p = event.GetProperty();

    if ( p )
    {
        wxLogVerbose("OnPropertyGridChange(%s, value=%s)",
                   p->GetName(), p->GetValueAsString());
    }
    else
    {
        wxLogVerbose("OnPropertyGridChange(nullptr)");
    }
}

void MyFrame::OnPropertyGridChanging(wxPropertyGridEvent &event)
{
    wxPGProperty* p = event.GetProperty();

    wxLogVerbose("OnPropertyGridChanging(%s)", p->GetName());
}

void MyFrame::OnAction(wxCommandEvent &)
{
}

// Called from propgridsample.cpp
//
void DisplayMinimalFrame(wxWindow* parent)
{
    MyFrame *frame = new MyFrame(parent);
    frame->Show(true);
}
