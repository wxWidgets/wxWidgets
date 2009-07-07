///////////////////////////////////////////////////////////////////////////////
// Name:        ribbondemo.cpp
// Purpose:     wxRibbon: Ribbon user interface - sample/test program
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2009, Peter Cawley
// Licence:     wxWindows Library Licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/sizer.h"

// -- application --

class MyApp : public wxApp
{
public:
    bool OnInit();
};

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

// -- frame --

class MyFrame : public wxFrame
{
public:
    MyFrame();
    ~MyFrame();

protected:
    wxRibbonBar* m_ribbon;
    wxTextCtrl* m_logwindow;

    DECLARE_EVENT_TABLE()
};

// -- implementations --

bool MyApp::OnInit()
{
    if(!wxApp::OnInit())
        return false;

    wxFrame* frame = new MyFrame;
    SetTopWindow(frame);
    frame->Show();

    return true;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
END_EVENT_TABLE()

#include "auto_crop_selection.xpm"
#include "circle.xpm"
#include "cross.xpm"
#include "expand_selection_v.xpm"
#include "expand_selection_h.xpm"
#include "square.xpm"
#include "triangle.xpm"

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, wxT("wxRibbon Sample Application"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
{
    m_ribbon = new wxRibbonBar(this);

    // Page names are from MS Word 2007, will be changed later
    {
        wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Home"));
        new wxRibbonPanel(home, wxID_ANY, wxT("Clipboard"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON | wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        new wxRibbonPanel(home, wxID_ANY, wxT("Font"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON);
        new wxRibbonPanel(home, wxID_ANY, wxT("Paragraph"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON);
        new wxRibbonPanel(home, wxID_ANY, wxT("Styles"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON);

        wxRibbonPanel *selection_panel = new wxRibbonPanel(home, wxID_ANY, wxT("Selection"));
        wxRibbonButtonBar *selection = new wxRibbonButtonBar(selection_panel);
        selection->AddButton(wxID_ANY, wxT("Expand Vertically"), wxBitmap(expand_selection_v_xpm), wxEmptyString);
        selection->AddButton(wxID_ANY, wxT("Expand Horizontally"), wxBitmap(expand_selection_h_xpm), wxEmptyString);
        selection->AddButton(wxID_ANY, wxT("Contract"), wxBitmap(auto_crop_selection_xpm), wxEmptyString);

        wxRibbonPanel *shapes_panel = new wxRibbonPanel(home, wxID_ANY, wxT("Shapes"));
        wxRibbonButtonBar *shapes = new wxRibbonButtonBar(shapes_panel);
        shapes->AddButton(wxID_ANY, wxT("Circle"), wxBitmap(circle_xpm), wxEmptyString);
        shapes->AddButton(wxID_ANY, wxT("Cross"), wxBitmap(cross_xpm), wxEmptyString);
        shapes->AddButton(wxID_ANY, wxT("Triangle"), wxBitmap(triangle_xpm), wxEmptyString);
        shapes->AddButton(wxID_ANY, wxT("Square"), wxBitmap(square_xpm), wxEmptyString);
    }
    {
        wxRibbonPage* insert = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Insert"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Pages"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Tables"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        new wxRibbonPanel(insert, wxID_ANY, wxT("Illustrations"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Links"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Header & Footer"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Text"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Symbols"));
    }
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Page Layout"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("References"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Mailings"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Review"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("View"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Developer"));

    m_ribbon->Realize();

    m_logwindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_LEFT | wxTE_BESTWRAP);

    wxSizer *s = new wxBoxSizer(wxVERTICAL);

    s->Add(m_ribbon, 0, wxEXPAND);
    s->Add(m_logwindow, 1, wxEXPAND);

    SetSizer(s);
}

MyFrame::~MyFrame()
{
}
