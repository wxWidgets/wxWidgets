/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/propgrid_minimal.cpp
// Purpose:     Minimal portion of wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-23
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/propgrid/propgrid.h"

class MyFrame : public wxFrame 
{ 
public: 
    MyFrame(wxWindow* parent); 
 
    void OnAction(wxCommandEvent& event); 
    void OnPropertyGridChange(wxPropertyGridEvent& event); 

private:
    wxPropertyGrid* m_pg;
    DECLARE_EVENT_TABLE()
};

//
// Called from propgridsample.cpp
//
void DisplayMinimalFrame(wxWindow* parent)
{
    MyFrame *frame = new MyFrame(parent);
    frame->Show(true);
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_HIGHEST+1, MyFrame::OnAction)
    EVT_PG_CHANGED( -1, MyFrame::OnPropertyGridChange )
END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxT("PropertyGrid Test"))
{
    wxMenu *Menu = new wxMenu; 
    Menu->Append(wxID_HIGHEST+1, wxT("Action")); 
    wxMenuBar *MenuBar = new wxMenuBar(); 
    MenuBar->Append(Menu, wxT("Action")); 
    SetMenuBar(MenuBar); 

    wxPropertyGrid *pg = new wxPropertyGrid(this,-1,wxDefaultPosition,wxSize(400,400),
                        wxPG_SPLITTER_AUTO_CENTER |
                        wxPG_BOLD_MODIFIED );
    m_pg = pg;

    for ( int i=0; i< 20; i++ )
        pg->Append(new wxStringProperty(wxString::Format(wxT("Item %i"),i), wxPG_LABEL));

    wxPGProperty* topId;
    wxPGProperty* medId;
    wxPGProperty* botId;

    topId = pg->Append( new wxStringProperty(wxT("Top Item"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(topId, true);
    medId = pg->AppendIn( topId, new wxStringProperty(wxT("Medium Level Item A"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(medId, true);
    botId = pg->AppendIn( medId, new wxStringProperty(wxT("Position"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(botId, true);
    pg->AppendIn( botId, new wxFloatProperty(wxT("x"), wxPG_LABEL, 1.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("y"), wxPG_LABEL, 2.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("z"), wxPG_LABEL, 3.0) );
    pg->AppendIn( medId, new wxStringProperty(wxT("Name"), wxPG_LABEL, wxT("name")) );
    medId = pg->AppendIn( topId, new wxStringProperty(wxT("Medium Level Item B"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(medId, true);
    botId = pg->AppendIn( medId, new wxStringProperty(wxT("Position"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(botId, true);
    pg->AppendIn( botId, new wxFloatProperty(wxT("x"), wxPG_LABEL, 1.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("y"), wxPG_LABEL, 2.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("z"), wxPG_LABEL, 3.0) );
    pg->AppendIn( medId, new wxStringProperty(wxT("Name"), wxPG_LABEL, wxT("name")) );
    medId = pg->AppendIn( topId, new wxStringProperty(wxT("Medium Level Item C"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(medId, true);
    botId = pg->AppendIn( medId, new wxStringProperty(wxT("Position"), wxPG_LABEL, wxT("<composed>")) );
    pg->LimitPropertyEditing(botId, true);
    pg->AppendIn( botId, new wxFloatProperty(wxT("x"), wxPG_LABEL, 1.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("y"), wxPG_LABEL, 2.0) );
    pg->AppendIn( botId, new wxFloatProperty(wxT("z"), wxPG_LABEL, 3.0) );
    pg->AppendIn( medId, new wxStringProperty(wxT("Name"), wxPG_LABEL, wxT("name")) );

    SetSize(400, 600);
}

void MyFrame::OnPropertyGridChange(wxPropertyGridEvent &event)
{
    wxPGProperty* p = event.GetProperty();

    wxLogDebug(wxT("OnPropertyGridChange(%s)"), p->GetName().c_str());

    if ( p->GetBaseName() == wxT("x") )
    {
        wxLogDebug(wxT("double values=%.2f)"), m_pg->GetPropertyValueAsDouble(p));
    }
}

void MyFrame::OnAction(wxCommandEvent &) 
{
}
