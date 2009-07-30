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
#include "wx/colordlg.h"

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
        ID_PRIMARY_COLOUR,
        ID_SECONDARY_COLOUR,
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
    void OnHoveredColourChange(wxRibbonGalleryEvent& evt);
    void OnPrimaryColourSelect(wxRibbonGalleryEvent& evt);
    void OnSecondaryColourSelect(wxRibbonGalleryEvent& evt);
    void OnColourGalleryButton(wxCommandEvent& evt);

protected:
    wxRibbonGallery* PopulateColoursPanel(wxRibbonPanel* panel, wxColour def,
        int gallery_id);
    void AddText(wxString msg);
    wxRibbonGalleryItem* AddColourToGallery(wxRibbonGallery *gallery,
        wxString name, wxMemoryDC& dc, wxColour* value = NULL);
    wxColour GetGalleryColour(wxRibbonGallery *gallery,
        wxRibbonGalleryItem* item, wxString* name);
    void ResetGalleryArtProviders();

    wxRibbonBar* m_ribbon;
    wxRibbonGallery* m_primary_gallery;
    wxRibbonGallery* m_secondary_gallery;
    wxTextCtrl* m_logwindow;
    wxColourData m_colour_data;
    wxColour m_default_primary;
    wxColour m_default_secondary;
    wxColour m_default_tertiary;
    wxMemoryDC m_bitmap_creation_dc;

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
EVT_RIBBONGALLERY_HOVER_CHANGED(ID_PRIMARY_COLOUR, MyFrame::OnHoveredColourChange)
EVT_RIBBONGALLERY_HOVER_CHANGED(ID_SECONDARY_COLOUR, MyFrame::OnHoveredColourChange)
EVT_RIBBONGALLERY_SELECTED(ID_PRIMARY_COLOUR, MyFrame::OnPrimaryColourSelect)
EVT_RIBBONGALLERY_SELECTED(ID_SECONDARY_COLOUR, MyFrame::OnSecondaryColourSelect)
EVT_BUTTON(ID_PRIMARY_COLOUR, MyFrame::OnColourGalleryButton)
EVT_BUTTON(ID_SECONDARY_COLOUR, MyFrame::OnColourGalleryButton)
END_EVENT_TABLE()

#include "auto_crop_selection.xpm"
#include "auto_crop_selection_small.xpm"
#include "circle.xpm"
#include "circle_small.xpm"
#include "colours.xpm"
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

    {
        wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Examples"));
        new wxRibbonPanel(home, wxID_ANY, wxT("Empty Panel"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON | wxRIBBON_PANEL_NO_AUTO_MINIMISE);

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

        new wxRibbonPanel(home, wxID_ANY, wxT("Another Panel"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_EXT_BUTTON);
    }
    {
        wxFont label_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
        m_bitmap_creation_dc.SetFont(label_font);

        wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Appearance"));
        m_ribbon->GetArtProvider()->GetColourScheme(&m_default_primary,
            &m_default_secondary, &m_default_tertiary);
        wxRibbonPanel *primary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            wxT("Primary Colour"), colours_xpm);
        m_primary_gallery = PopulateColoursPanel(primary_panel,
            m_default_primary, ID_PRIMARY_COLOUR);
        wxRibbonPanel *secondary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            wxT("Secondary Colour"), colours_xpm);
        m_secondary_gallery = PopulateColoursPanel(secondary_panel,
            m_default_secondary, ID_SECONDARY_COLOUR);
    }
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Empty Page"));
    new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Another Page"));

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

class ColourClientData : public wxClientData
{
public:
    ColourClientData(const wxString& name, const wxColour& colour)
        : m_name(name), m_colour(colour) {}

    const wxString& GetName()   const {return m_name;}
    const wxColour& GetColour() const {return m_colour;}

private:
    wxString m_name;
    wxColour m_colour;
};

wxRibbonGallery* MyFrame::PopulateColoursPanel(wxRibbonPanel* panel,
                                               wxColour def, int gallery_id)
{
    wxRibbonGallery *gallery = new wxRibbonGallery(panel, gallery_id);
    wxMemoryDC& dc = m_bitmap_creation_dc;
    wxRibbonGalleryItem *def_item = 
        AddColourToGallery(gallery, wxT("Default"), dc, &def);
    gallery->SetSelection(def_item);
    AddColourToGallery(gallery, wxT("BLUE"), dc);
    AddColourToGallery(gallery, wxT("BLUE VIOLET"), dc);
    AddColourToGallery(gallery, wxT("BROWN"), dc);
    AddColourToGallery(gallery, wxT("CADET BLUE"), dc);
    AddColourToGallery(gallery, wxT("CORAL"), dc);
    AddColourToGallery(gallery, wxT("CYAN"), dc);
    AddColourToGallery(gallery, wxT("DARK GREEN"), dc);
    AddColourToGallery(gallery, wxT("DARK ORCHID"), dc);
    AddColourToGallery(gallery, wxT("FIREBRICK"), dc);
    AddColourToGallery(gallery, wxT("GOLD"), dc);
    AddColourToGallery(gallery, wxT("GOLDENROD"), dc);
    AddColourToGallery(gallery, wxT("GREEN"), dc);
    AddColourToGallery(gallery, wxT("INDIAN RED"), dc);
    AddColourToGallery(gallery, wxT("KHAKI"), dc);
    AddColourToGallery(gallery, wxT("LIGHT BLUE"), dc);
    AddColourToGallery(gallery, wxT("LIME GREEN"), dc);
    AddColourToGallery(gallery, wxT("MAGENTA"), dc);
    AddColourToGallery(gallery, wxT("MAROON"), dc);
    AddColourToGallery(gallery, wxT("NAVY"), dc);
    AddColourToGallery(gallery, wxT("ORANGE"), dc);
    AddColourToGallery(gallery, wxT("ORCHID"), dc);
    AddColourToGallery(gallery, wxT("PINK"), dc);
    AddColourToGallery(gallery, wxT("PLUM"), dc);
    AddColourToGallery(gallery, wxT("PURPLE"), dc);
    AddColourToGallery(gallery, wxT("RED"), dc);
    AddColourToGallery(gallery, wxT("SALMON"), dc);
    AddColourToGallery(gallery, wxT("SEA GREEN"), dc);
    AddColourToGallery(gallery, wxT("SIENNA"), dc);
    AddColourToGallery(gallery, wxT("SKY BLUE"), dc);
    AddColourToGallery(gallery, wxT("TAN"), dc);
    AddColourToGallery(gallery, wxT("THISTLE"), dc);
    AddColourToGallery(gallery, wxT("TURQUOISE"), dc);
    AddColourToGallery(gallery, wxT("VIOLET"), dc);
    AddColourToGallery(gallery, wxT("VIOLET RED"), dc);
    AddColourToGallery(gallery, wxT("WHEAT"), dc);
    AddColourToGallery(gallery, wxT("WHITE"), dc);
    AddColourToGallery(gallery, wxT("YELLOW"), dc);

    return gallery;
}

wxColour MyFrame::GetGalleryColour(wxRibbonGallery *gallery,
                                   wxRibbonGalleryItem* item, wxString* name)
{
    ColourClientData *data = (ColourClientData*)gallery->GetItemClientObject(item);
    if(name != NULL)
        *name = data->GetName();
    return data->GetColour();
}

void MyFrame::OnHoveredColourChange(wxRibbonGalleryEvent& evt)
{
    // Set the background of the gallery to the hovered colour, or back to the
    // default if there is no longer a hovered item.

    wxRibbonGallery *gallery = evt.GetGallery();
    wxRibbonArtProvider *provider = gallery->GetArtProvider();

    if(evt.GetGalleryItem() != NULL)
    {
        if(provider == m_ribbon->GetArtProvider())
        {
            provider = provider->Clone();
            gallery->SetArtProvider(provider);
        }
        provider->SetColour(wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR,
            GetGalleryColour(evt.GetGallery(), evt.GetGalleryItem(), NULL));
    }
    else
    {
        if(provider != m_ribbon->GetArtProvider())
        {
            gallery->SetArtProvider(m_ribbon->GetArtProvider());
            delete provider;
        }
    }
}

void MyFrame::OnPrimaryColourSelect(wxRibbonGalleryEvent& evt)
{
    wxString name;
    wxColour colour = GetGalleryColour(evt.GetGallery(), evt.GetGalleryItem(), &name);
    AddText(wxT("Colour \"") + name + wxT("\" selected as primary."));
    wxColour secondary, tertiary;
    m_ribbon->GetArtProvider()->GetColourScheme(NULL, &secondary, &tertiary);
    m_ribbon->GetArtProvider()->SetColourScheme(colour, secondary, tertiary);
    ResetGalleryArtProviders();
    m_ribbon->Refresh();
}

void MyFrame::OnSecondaryColourSelect(wxRibbonGalleryEvent& evt)
{
    wxString name;
    wxColour colour = GetGalleryColour(evt.GetGallery(), evt.GetGalleryItem(), &name);
    AddText(wxT("Colour \"") + name + wxT("\" selected as secondary."));
    wxColour primary, tertiary;
    m_ribbon->GetArtProvider()->GetColourScheme(&primary, NULL, &tertiary);
    m_ribbon->GetArtProvider()->SetColourScheme(primary, colour, tertiary);
    ResetGalleryArtProviders();
    m_ribbon->Refresh();
}

void MyFrame::ResetGalleryArtProviders()
{
    if(m_primary_gallery->GetArtProvider() != m_ribbon->GetArtProvider())
    {
        delete m_primary_gallery->GetArtProvider();
        m_primary_gallery->SetArtProvider(m_ribbon->GetArtProvider());
    }
    if(m_secondary_gallery->GetArtProvider() != m_ribbon->GetArtProvider())
    {
        delete m_secondary_gallery->GetArtProvider();
        m_secondary_gallery->SetArtProvider(m_ribbon->GetArtProvider());
    }
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
    menu.Append(wxID_ANY, wxT("Equilateral"));
    menu.Append(wxID_ANY, wxT("Isosceles"));
    menu.Append(wxID_ANY, wxT("Scalene"));

    evt.PopupMenu(&menu);
}

void MyFrame::OnSquareButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText(wxT("Square button clicked."));
}

void MyFrame::OnPolygonDropdown(wxRibbonButtonBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, wxT("Pentagon (5 sided)"));
    menu.Append(wxID_ANY, wxT("Hexagon (6 sided)"));
    menu.Append(wxID_ANY, wxT("Heptagon (7 sided)"));
    menu.Append(wxID_ANY, wxT("Octogon (8 sided)"));
    menu.Append(wxID_ANY, wxT("Nonagon (9 sided)"));
    menu.Append(wxID_ANY, wxT("Decagon (10 sided)"));

    evt.PopupMenu(&menu);
}

void MyFrame::AddText(wxString msg)
{
    m_logwindow->AppendText(msg);
    m_logwindow->AppendText(wxT("\n"));
    m_ribbon->DismissExpandedPanel();
}

wxRibbonGalleryItem* MyFrame::AddColourToGallery(wxRibbonGallery *gallery,
                                 wxString colour, wxMemoryDC& dc,
                                 wxColour* value)
{
    wxRibbonGalleryItem* item = NULL;
    wxColour c(colour);
    if(value != NULL)
        c = *value;
    if(c.IsOk())
    {
        const int iWidth = 64;
        const int iHeight = 40;

        wxBitmap bitmap(iWidth, iHeight);
        dc.SelectObject(bitmap);
        wxBrush b(c);
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(b);
        dc.DrawRectangle(0, 0, iWidth, iHeight);

        colour = colour.Mid(0, 1) + colour.Mid(1).Lower();
        wxSize size = dc.GetTextExtent(colour);
        wxColour foreground = wxColour(~c.Red(), ~c.Green(), ~c.Blue());
        if(abs(foreground.Red() - c.Red()) + abs(foreground.Blue() - c.Blue())
            + abs(foreground.Green() - c.Green()) < 64)
        {
            // Foreground too similar to background - use a different
            // strategy to find a contrasting colour
            foreground = wxColour((c.Red() + 64) % 256, 255 - c.Green(),
                (c.Blue() + 192) % 256);
        }
        dc.SetTextForeground(foreground);
        dc.DrawText(colour, (iWidth - size.GetWidth() + 1) / 2,
            (iHeight - size.GetHeight()) / 2);
        dc.SelectObjectAsSource(wxNullBitmap);

        item = gallery->Append(bitmap, wxID_ANY);
        gallery->SetItemClientObject(item, new ColourClientData(colour, c));
    }
    return item;
}

void MyFrame::OnColourGalleryButton(wxCommandEvent& evt)
{
    wxRibbonGallery *gallery = wxDynamicCast(evt.GetEventObject(), wxRibbonGallery);
    if(gallery == NULL)
        return;

    m_ribbon->DismissExpandedPanel();
    if(gallery->GetSelection())
        m_colour_data.SetColour(GetGalleryColour(gallery, gallery->GetSelection(), NULL));
    wxColourDialog dlg(this, &m_colour_data);
    if(dlg.ShowModal() == wxID_OK)
    {
        m_colour_data = dlg.GetColourData();
        wxColour clr = m_colour_data.GetColour();

        // Try to find colour in gallery
        wxRibbonGalleryItem *item = NULL;
        for(unsigned int i = 0; i < gallery->GetCount(); ++i)
        {
            item = gallery->GetItem(i);
            if(GetGalleryColour(gallery, item, NULL) == clr)
                break;
            else
                item = NULL;
        }

        // Colour not in gallery - add it
        if(item == NULL)
        {
            item = AddColourToGallery(gallery,
                clr.GetAsString(wxC2S_HTML_SYNTAX), m_bitmap_creation_dc,
                &clr);
            gallery->Realise();
        }

        // Set selection
        gallery->EnsureVisible(item);
        gallery->SetSelection(item);

        // Send an event to respond to the selection change
        wxRibbonGalleryEvent dummy(wxEVT_COMMAND_RIBBONGALLERY_SELECTED, gallery->GetId());
        dummy.SetEventObject(gallery);
        dummy.SetGallery(gallery);
        dummy.SetGalleryItem(item);
        ProcessWindowEvent(dummy);
    }
}
