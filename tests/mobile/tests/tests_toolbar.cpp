/*
 *  tests_toolbar.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_toolbar.h"
#include "tests_toolbar_bitmap.h"

BEGIN_EVENT_TABLE(MobileTestsWxToolBarPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxToolBarPanel::CreateWithControls(wxWindow* parent,
                                                    wxWindowID id,
                                                    const wxPoint& pos,
                                                    const wxSize& size,
                                                    long style,
                                                    const wxString& name)
{
    if ( !wxPanel::Create(parent, id, pos, size, style, name)) {
        wxFAIL_MSG("Unable to create parent wxPanel");
        return false;
    }
    
    if ( !CreateControls()) {
        wxFAIL_MSG("Unable to create controls for this test panel");
        return false;
    }
    
    return true;
}

bool MobileTestsWxToolBarPanel::CreateControls()
{
    wxBoxSizer* checkBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Toolbar
    // 
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    
    // Toolbar
    
    m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(300, 44), wxTB_BLACK_OPAQUE_BG);
    sizer->Add(m_toolbar, 0, wxEXPAND, 0);
    
    // Events
    m_toolbar->Connect(wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MobileTestsWxToolBarPanel::OnToolBarToolClicked), NULL, this);
    
    m_toolbar->AddTool(100, _("First tool"), wxBitmap(tests_toolbar_bitmap));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_REFRESH, wxEmptyString, wxNullBitmap);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_CAMERA, wxEmptyString, wxNullBitmap);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_ADD, wxEmptyString, wxNullBitmap);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_COMPOSE, wxEmptyString, wxNullBitmap);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_TRASH, wxEmptyString, wxNullBitmap);
    m_toolbar->AddSeparator();
    
    m_toolbar->Realize();
    
    return true;
}

void MobileTestsWxToolBarPanel::OnToolBarToolClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Event wxEVT_COMMAND_TOOL_CLICKED");
}
