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
#include "wx/ribbon/gallery.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/dcbuffer.h"

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

    enum
    {
        ID_CIRCLE = wxID_HIGHEST + 1,
        ID_CROSS,
        ID_TRIANGLE,
        ID_SQUARE,
        ID_POLYGON,
        ID_SELECTION_EXPAND_H,
        ID_SELECTION_EXPAND_V,
        ID_SELECTION_CONTRACT,
    };

    void OnCircleButton(wxRibbonButtonBarEvent& evt);
    void OnCrossButton(wxRibbonButtonBarEvent& evt);
    void OnTriangleButton(wxRibbonButtonBarEvent& evt);
    void OnTriangleDropdown(wxRibbonButtonBarEvent& evt);
    void OnSquareButton(wxRibbonButtonBarEvent& evt);
    void OnPolygonDropdown(wxRibbonButtonBarEvent& evt);
    void OnSelectionExpandVButton(wxRibbonButtonBarEvent& evt);
    void OnSelectionExpandHButton(wxRibbonButtonBarEvent& evt);
    void OnSelectionContractButton(wxRibbonButtonBarEvent& evt);

protected:
    void AddText(wxString msg);
    void AddColourToGallery(wxRibbonGallery *gallery, wxString colour,
        wxMemoryDC& dc);

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
EVT_RIBBONBUTTONBAR_CLICKED(ID_SELECTION_EXPAND_H, MyFrame::OnSelectionExpandHButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_SELECTION_EXPAND_V, MyFrame::OnSelectionExpandVButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_SELECTION_CONTRACT, MyFrame::OnSelectionContractButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CIRCLE, MyFrame::OnCircleButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CROSS, MyFrame::OnCrossButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_TRIANGLE, MyFrame::OnTriangleButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_SQUARE, MyFrame::OnSquareButton)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_TRIANGLE, MyFrame::OnTriangleDropdown)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_POLYGON, MyFrame::OnPolygonDropdown)
END_EVENT_TABLE()

#include "auto_crop_selection.xpm"
#include "auto_crop_selection_small.xpm"
#include "circle.xpm"
#include "circle_small.xpm"
#include "cross.xpm"
#include "expand_selection_v.xpm"
#include "expand_selection_h.xpm"
#include "hexagon.xpm"
#include "selection_panel.xpm"
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

        wxRibbonPanel *selection_panel = new wxRibbonPanel(home, wxID_ANY, wxT("Selection"), wxBitmap(selection_panel_xpm));
        wxRibbonButtonBar *selection = new wxRibbonButtonBar(selection_panel);
        selection->AddButton(ID_SELECTION_EXPAND_V, wxT("Expand Vertically"), wxBitmap(expand_selection_v_xpm), wxEmptyString);
        selection->AddButton(ID_SELECTION_EXPAND_H, wxT("Expand Horizontally"), wxBitmap(expand_selection_h_xpm), wxEmptyString);
        selection->AddButton(ID_SELECTION_CONTRACT, wxT("Contract"), wxBitmap(auto_crop_selection_xpm), wxBitmap(auto_crop_selection_small_xpm));

        wxRibbonPanel *shapes_panel = new wxRibbonPanel(home, wxID_ANY, wxT("Shapes"), wxBitmap(circle_small_xpm));
        wxRibbonButtonBar *shapes = new wxRibbonButtonBar(shapes_panel);
        shapes->AddButton(ID_CIRCLE, wxT("Circle"), wxBitmap(circle_xpm), wxBitmap(circle_small_xpm));
        shapes->AddButton(ID_CROSS, wxT("Cross"), wxBitmap(cross_xpm), wxEmptyString);
        shapes->AddHybridButton(ID_TRIANGLE, wxT("Triangle"), wxBitmap(triangle_xpm));
        shapes->AddButton(ID_SQUARE, wxT("Square"), wxBitmap(square_xpm), wxEmptyString);
        shapes->AddDropdownButton(ID_POLYGON, wxT("Other Polygon"), wxBitmap(hexagon_xpm), wxEmptyString);

        new wxRibbonPanel(home, wxID_ANY, wxT("Font"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON);
    }
    {
        wxRibbonPage* insert = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Insert"));
        new wxRibbonPanel(insert, wxID_ANY, wxT("Tables"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonPanel *colours_panel = new wxRibbonPanel(insert, wxID_ANY, wxT("Colours"));
        wxRibbonGallery *colours_gallery = new wxRibbonGallery(colours_panel, wxID_ANY);
        wxMemoryDC tmp_dc;
        wxFont label_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
        tmp_dc.SetFont(label_font);
        AddColourToGallery(colours_gallery, "BLUE", tmp_dc);
        AddColourToGallery(colours_gallery, "GOLDENROD", tmp_dc);
        AddColourToGallery(colours_gallery, "MAROON", tmp_dc);
        AddColourToGallery(colours_gallery, "TURQUOISE", tmp_dc);
        AddColourToGallery(colours_gallery, "WHEAT", tmp_dc);
        new wxRibbonPanel(insert, wxID_ANY, wxT("Links"));
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

void MyFrame::OnSelectionExpandHButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Expand selection horizontally button clicked."));
}

void MyFrame::OnSelectionExpandVButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Expand selection vertically button clicked."));
}

void MyFrame::OnSelectionContractButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Contract selection button clicked."));
}

void MyFrame::OnCircleButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Circle button clicked."));
}

void MyFrame::OnCrossButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Cross button clicked."));
}

void MyFrame::OnTriangleButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Triangle button clicked."));
}

void MyFrame::OnTriangleDropdown(wxRibbonButtonBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, wxT("Item 1"));
    menu.Append(wxID_ANY, wxT("Item 2"));
    menu.Append(wxID_ANY, wxT("Item 3"));

    evt.PopupMenu(&menu);
}

void MyFrame::OnSquareButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Square button clicked."));
}

void MyFrame::OnPolygonDropdown(wxRibbonButtonBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, wxT("Item 1"));
    menu.Append(wxID_ANY, wxT("Item 2"));
    menu.Append(wxID_ANY, wxT("Item 3"));

    evt.PopupMenu(&menu);
}

void MyFrame::AddText(wxString msg)
{
    m_logwindow->AppendText(msg);
    m_logwindow->AppendText(wxT("\n"));
    m_logwindow->SetFocus();
}

void MyFrame::AddColourToGallery(wxRibbonGallery *gallery, wxString colour,
                                 wxMemoryDC& dc)
{
    wxColour c(colour);
    if(c.IsOk())
    {
        const int iWidth = 64;
        const int iHeight = 48;

        wxBitmap bitmap(iWidth, iHeight);
        dc.SelectObject(bitmap);
        wxBrush b(c);
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(b);
        dc.DrawRectangle(0, 0, iWidth, iHeight);

        colour = colour.Mid(0, 1) + colour.Mid(1).Lower();
        wxSize size = dc.GetTextExtent(colour);
        dc.SetTextForeground(wxColour(~c.Red(), ~c.Green(), ~c.Blue()));
        dc.DrawText(colour, (iWidth - size.GetWidth() + 1) / 2,
            (iHeight - size.GetHeight()) / 2);

        gallery->Append(bitmap, wxID_ANY);
    }
}