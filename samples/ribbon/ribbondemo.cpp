///////////////////////////////////////////////////////////////////////////////
// Name:        ribbondemo.cpp
// Purpose:     wxRibbon: Ribbon user interface - sample/test program
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// Copyright:   (C) Copyright 2009, Peter Cawley
// Licence:     wxWindows Library Licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#include "wx/app.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/dcbuffer.h"
#include "wx/colordlg.h"
#include "wx/artprov.h"
#include "wx/combobox.h"
#include "wx/tglbtn.h"
#include "wx/wrapsizer.h"

// -- application --

class MyApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

wxDECLARE_APP(MyApp);
wxIMPLEMENT_APP(MyApp);

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
        ID_BUTTON_XX,
        ID_BUTTON_XY,
        ID_PRIMARY_COLOUR,
        ID_SECONDARY_COLOUR,
        ID_DEFAULT_PROVIDER,
        ID_AUI_PROVIDER,
        ID_MSW_PROVIDER,
        ID_MAIN_TOOLBAR,
        ID_POSITION_TOP,
        ID_POSITION_TOP_ICONS,
        ID_POSITION_TOP_BOTH,
        ID_POSITION_LEFT,
        ID_POSITION_LEFT_LABELS,
        ID_POSITION_LEFT_BOTH,
        ID_TOGGLE_PANELS,
        ID_ENABLE,
        ID_DISABLE,
        ID_DISABLED,
        ID_UI_ENABLE_UPDATED,
        ID_CHECK,
        ID_UI_CHECK_UPDATED,
        ID_CHANGE_TEXT1,
        ID_CHANGE_TEXT2,
        ID_UI_CHANGE_TEXT_UPDATED,
        ID_REMOVE_PAGE,
        ID_HIDE_PAGES,
        ID_SHOW_PAGES,
        ID_PLUS_MINUS,
        ID_CHANGE_LABEL,
        ID_SMALL_BUTTON_1,
        ID_SMALL_BUTTON_2,
        ID_SMALL_BUTTON_3,
        ID_SMALL_BUTTON_4,
        ID_SMALL_BUTTON_5,
        ID_SMALL_BUTTON_6
    };

    void OnEnableUpdateUI(wxUpdateUIEvent& evt);
    void OnCheckUpdateUI(wxUpdateUIEvent& evt);
    void OnChangeTextUpdateUI(wxUpdateUIEvent& evt);
    void OnCheck(wxRibbonButtonBarEvent& evt);
    void OnEnable(wxRibbonButtonBarEvent& evt);
    void OnDisable(wxRibbonButtonBarEvent& evt);
    void OnDisabled(wxRibbonButtonBarEvent& evt);
    void OnEnableUpdated(wxRibbonButtonBarEvent& evt);
    void OnChangeText1(wxRibbonButtonBarEvent& evt);
    void OnChangeText2(wxRibbonButtonBarEvent& evt);
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
    void OnDefaultProvider(wxRibbonButtonBarEvent& evt);
    void OnAUIProvider(wxRibbonButtonBarEvent& evt);
    void OnMSWProvider(wxRibbonButtonBarEvent& evt);
    void OnJustify(wxRibbonToolBarEvent& evt);
    void OnJustifyUpdateUI(wxUpdateUIEvent& evt);
    void OnNew(wxRibbonToolBarEvent& evt);
    void OnNewDropdown(wxRibbonToolBarEvent& evt);
    void OnPrint(wxRibbonToolBarEvent& evt);
    void OnPrintDropdown(wxRibbonToolBarEvent& evt);
    void OnRedoDropdown(wxRibbonToolBarEvent& evt);
    void OnUndoDropdown(wxRibbonToolBarEvent& evt);
    void OnPositionTop(wxRibbonToolBarEvent& evt);
    void OnPositionTopLabels(wxCommandEvent& evt);
    void OnPositionTopIcons(wxCommandEvent& evt);
    void OnPositionTopBoth(wxCommandEvent& evt);
    void OnPositionTopDropdown(wxRibbonToolBarEvent& evt);
    void OnPositionLeft(wxRibbonToolBarEvent& evt);
    void OnPositionLeftLabels(wxCommandEvent& evt);
    void OnPositionLeftIcons(wxCommandEvent& evt);
    void OnPositionLeftBoth(wxCommandEvent& evt);
    void OnPositionLeftDropdown(wxRibbonToolBarEvent& evt);
    void OnRemovePage(wxRibbonButtonBarEvent& evt);
    void OnHidePages(wxRibbonButtonBarEvent& evt);
    void OnShowPages(wxRibbonButtonBarEvent& evt);
    void OnPlusMinus(wxRibbonButtonBarEvent& evt);
    void OnChangeLabel(wxRibbonButtonBarEvent& evt);
    void OnTogglePanels(wxCommandEvent& evt);
    void OnRibbonBarToggled(wxRibbonBarEvent& evt);
    void OnRibbonBarHelpClicked(wxRibbonBarEvent& evt);

    void OnSizeEvent(wxSizeEvent& evt);

    void OnExtButton(wxRibbonPanelEvent& evt);

protected:
    wxRibbonGallery* PopulateColoursPanel(wxWindow* panel, wxColour def,
        int gallery_id);
    void AddText(wxString msg);
    wxRibbonGalleryItem* AddColourToGallery(wxRibbonGallery *gallery,
        wxString name, wxMemoryDC& dc, wxColour* value = NULL);
    wxColour GetGalleryColour(wxRibbonGallery *gallery,
        wxRibbonGalleryItem* item, wxString* name);
    void ResetGalleryArtProviders();
    void SetArtProvider(wxRibbonArtProvider* prov);
    void SetBarStyle(long style);

    wxRibbonBar* m_ribbon;
    wxRibbonGallery* m_primary_gallery;
    wxRibbonGallery* m_secondary_gallery;
    wxTextCtrl* m_logwindow;
    wxToggleButton* m_togglePanels;

    wxColourData m_colour_data;
    wxColour m_default_primary;
    wxColour m_default_secondary;
    wxColour m_default_tertiary;
    wxMemoryDC m_bitmap_creation_dc;
    bool m_bEnabled;
    bool m_bChecked;
    wxString m_new_text;

    bool m_plus_minus_state;
    bool m_change_label_state;

    wxDECLARE_EVENT_TABLE();
};

// -- implementations --

bool MyApp::OnInit()
{
    if(!wxApp::OnInit())
        return false;

    wxFrame* frame = new MyFrame;
    frame->Show();

    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_RIBBONBUTTONBAR_CLICKED(ID_ENABLE, MyFrame::OnEnable)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DISABLE, MyFrame::OnDisable)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DISABLED, MyFrame::OnDisabled)
EVT_RIBBONBUTTONBAR_CLICKED(ID_UI_ENABLE_UPDATED, MyFrame::OnEnableUpdated)
EVT_UPDATE_UI(ID_UI_ENABLE_UPDATED, MyFrame::OnEnableUpdateUI)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CHECK, MyFrame::OnCheck)
EVT_UPDATE_UI(ID_UI_CHECK_UPDATED, MyFrame::OnCheckUpdateUI)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CHANGE_TEXT1, MyFrame::OnChangeText1)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CHANGE_TEXT2, MyFrame::OnChangeText2)
EVT_UPDATE_UI(ID_UI_CHANGE_TEXT_UPDATED, MyFrame::OnChangeTextUpdateUI)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DEFAULT_PROVIDER, MyFrame::OnDefaultProvider)
EVT_RIBBONBUTTONBAR_CLICKED(ID_AUI_PROVIDER, MyFrame::OnAUIProvider)
EVT_RIBBONBUTTONBAR_CLICKED(ID_MSW_PROVIDER, MyFrame::OnMSWProvider)
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
EVT_RIBBONTOOLBAR_CLICKED(wxID_JUSTIFY_LEFT, MyFrame::OnJustify)
EVT_RIBBONTOOLBAR_CLICKED(wxID_JUSTIFY_CENTER, MyFrame::OnJustify)
EVT_RIBBONTOOLBAR_CLICKED(wxID_JUSTIFY_RIGHT, MyFrame::OnJustify)
EVT_UPDATE_UI(wxID_JUSTIFY_LEFT, MyFrame::OnJustifyUpdateUI)
EVT_UPDATE_UI(wxID_JUSTIFY_CENTER, MyFrame::OnJustifyUpdateUI)
EVT_UPDATE_UI(wxID_JUSTIFY_RIGHT, MyFrame::OnJustifyUpdateUI)
EVT_RIBBONTOOLBAR_CLICKED(wxID_NEW, MyFrame::OnNew)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(wxID_NEW, MyFrame::OnNewDropdown)
EVT_RIBBONTOOLBAR_CLICKED(wxID_PRINT, MyFrame::OnPrint)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(wxID_PRINT, MyFrame::OnPrintDropdown)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(wxID_REDO, MyFrame::OnRedoDropdown)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(wxID_UNDO, MyFrame::OnUndoDropdown)
EVT_RIBBONTOOLBAR_CLICKED(ID_POSITION_LEFT, MyFrame::OnPositionLeft)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(ID_POSITION_LEFT, MyFrame::OnPositionLeftDropdown)
EVT_RIBBONTOOLBAR_CLICKED(ID_POSITION_TOP, MyFrame::OnPositionTop)
EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(ID_POSITION_TOP, MyFrame::OnPositionTopDropdown)
EVT_BUTTON(ID_PRIMARY_COLOUR, MyFrame::OnColourGalleryButton)
EVT_BUTTON(ID_SECONDARY_COLOUR, MyFrame::OnColourGalleryButton)
EVT_MENU(ID_POSITION_LEFT, MyFrame::OnPositionLeftIcons)
EVT_MENU(ID_POSITION_LEFT_LABELS, MyFrame::OnPositionLeftLabels)
EVT_MENU(ID_POSITION_LEFT_BOTH, MyFrame::OnPositionLeftBoth)
EVT_MENU(ID_POSITION_TOP, MyFrame::OnPositionTopLabels)
EVT_MENU(ID_POSITION_TOP_ICONS, MyFrame::OnPositionTopIcons)
EVT_MENU(ID_POSITION_TOP_BOTH, MyFrame::OnPositionTopBoth)
EVT_TOGGLEBUTTON(ID_TOGGLE_PANELS, MyFrame::OnTogglePanels)
EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(wxID_ANY, MyFrame::OnExtButton)
EVT_RIBBONBUTTONBAR_CLICKED(ID_REMOVE_PAGE, MyFrame::OnRemovePage)
EVT_RIBBONBUTTONBAR_CLICKED(ID_HIDE_PAGES, MyFrame::OnHidePages)
EVT_RIBBONBUTTONBAR_CLICKED(ID_SHOW_PAGES, MyFrame::OnShowPages)
EVT_RIBBONBUTTONBAR_CLICKED(ID_PLUS_MINUS, MyFrame::OnPlusMinus)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CHANGE_LABEL, MyFrame::OnChangeLabel)
EVT_RIBBONBAR_TOGGLED(wxID_ANY, MyFrame::OnRibbonBarToggled)
EVT_RIBBONBAR_HELP_CLICK(wxID_ANY, MyFrame::OnRibbonBarHelpClicked)
EVT_SIZE(MyFrame::OnSizeEvent)
wxEND_EVENT_TABLE()

#include "align_center.xpm"
#include "align_left.xpm"
#include "align_right.xpm"
#include "aui_style.xpm"
#include "auto_crop_selection.xpm"
#include "auto_crop_selection_small.xpm"
#include "circle.xpm"
#include "circle_small.xpm"
#include "colours.xpm"
#include "cross.xpm"
#include "empty.xpm"
#include "expand_selection_v.xpm"
#include "expand_selection_h.xpm"
#include "eye.xpm"
#include "hexagon.xpm"
#include "msw_style.xpm"
#include "position_left_small.xpm"
#include "position_top_small.xpm"
#include "ribbon.xpm"
#include "selection_panel.xpm"
#include "square.xpm"
#include "triangle.xpm"

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "wxRibbon Sample Application", wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
{
    m_ribbon = new wxRibbonBar(this,-1,wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_FLOW_HORIZONTAL
                                | wxRIBBON_BAR_SHOW_PAGE_LABELS
                                | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
                                | wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                                | wxRIBBON_BAR_SHOW_HELP_BUTTON
                                );

    {
        wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, "Examples", ribbon_xpm);
        wxRibbonPanel *toolbar_panel = new wxRibbonPanel(home, wxID_ANY, "Toolbar",
                                            wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                                            wxRIBBON_PANEL_NO_AUTO_MINIMISE |
                                            wxRIBBON_PANEL_EXT_BUTTON);
        wxRibbonToolBar *toolbar = new wxRibbonToolBar(toolbar_panel, ID_MAIN_TOOLBAR);
        toolbar->AddToggleTool(wxID_JUSTIFY_LEFT, align_left_xpm);
        toolbar->AddToggleTool(wxID_JUSTIFY_CENTER , align_center_xpm);
        toolbar->AddToggleTool(wxID_JUSTIFY_RIGHT, align_right_xpm);
        toolbar->AddSeparator();
        toolbar->AddHybridTool(wxID_NEW, wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 15)));
        toolbar->AddTool(wxID_OPEN, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 15)), "Open something");
        toolbar->AddTool(wxID_SAVE, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize(16, 15)), "Save something");
        toolbar->AddTool(wxID_SAVEAS, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, wxSize(16, 15)), "Save something as ...");
        toolbar->EnableTool(wxID_OPEN, false);
        toolbar->EnableTool(wxID_SAVE, false);
        toolbar->EnableTool(wxID_SAVEAS, false);
        toolbar->AddSeparator();
        toolbar->AddDropdownTool(wxID_UNDO, wxArtProvider::GetBitmap(wxART_UNDO, wxART_OTHER, wxSize(16, 15)));
        toolbar->AddDropdownTool(wxID_REDO, wxArtProvider::GetBitmap(wxART_REDO, wxART_OTHER, wxSize(16, 15)));
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_OTHER, wxSize(16, 15)));
        toolbar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_OTHER, wxSize(16, 15)));
        toolbar->AddSeparator();
        toolbar->AddHybridTool(ID_POSITION_LEFT, position_left_xpm,
                                "Align ribbonbar vertically\non the left\nfor demonstration purposes");
        toolbar->AddHybridTool(ID_POSITION_TOP, position_top_xpm,
                                "Align the ribbonbar horizontally\nat the top\nfor demonstration purposes");
        toolbar->AddSeparator();
        toolbar->AddHybridTool(wxID_PRINT, wxArtProvider::GetBitmap(wxART_PRINT, wxART_OTHER, wxSize(16, 15)),
                                "This is the Print button tooltip\ndemonstrating a tooltip");
        toolbar->SetRows(2, 3);

        wxRibbonPanel *selection_panel = new wxRibbonPanel(home, wxID_ANY, "Selection", wxBitmap(selection_panel_xpm));
        wxRibbonButtonBar *selection = new wxRibbonButtonBar(selection_panel);
        selection->AddButton(ID_SELECTION_EXPAND_V, "Expand Vertically", wxBitmap(expand_selection_v_xpm),
                                "This is a tooltip for Expand Vertically\ndemonstrating a tooltip");
        selection->AddButton(ID_SELECTION_EXPAND_H, "Expand Horizontally", wxBitmap(expand_selection_h_xpm), wxEmptyString);
        selection->AddButton(ID_SELECTION_CONTRACT, "Contract", wxBitmap(auto_crop_selection_xpm), wxBitmap(auto_crop_selection_small_xpm));

        wxRibbonPanel *shapes_panel = new wxRibbonPanel(home, wxID_ANY, "Shapes", wxBitmap(circle_small_xpm));
        wxRibbonButtonBar *shapes = new wxRibbonButtonBar(shapes_panel);
        shapes->AddButton(ID_CIRCLE, "Circle", wxBitmap(circle_xpm), wxBitmap(circle_small_xpm),
                            wxNullBitmap, wxNullBitmap, wxRIBBON_BUTTON_NORMAL,
                            "This is a tooltip for the circle button\ndemonstrating another tooltip");
        shapes->AddButton(ID_CROSS, "Cross", wxBitmap(cross_xpm), wxEmptyString);
        shapes->AddHybridButton(ID_TRIANGLE, "Triangle", wxBitmap(triangle_xpm));
        shapes->AddButton(ID_SQUARE, "Square", wxBitmap(square_xpm), wxEmptyString);
        shapes->AddDropdownButton(ID_POLYGON, "Other Polygon", wxBitmap(hexagon_xpm), wxEmptyString);

        wxRibbonPanel *sizer_panel = new wxRibbonPanel(home, wxID_ANY, "Panel with Sizer",
                                                    wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                                                    wxRIBBON_PANEL_DEFAULT_STYLE);

        wxArrayString as;
        as.Add("Item 1 using a box sizer now");
        as.Add("Item 2 using a box sizer now");
        wxComboBox* sizer_panelcombo = new wxComboBox(sizer_panel, wxID_ANY,
                                                    wxEmptyString,
                                                    wxDefaultPosition, wxDefaultSize,
                                                    as, wxCB_READONLY);

        wxComboBox* sizer_panelcombo2 = new wxComboBox(sizer_panel, wxID_ANY,
                                                    wxEmptyString,
                                                    wxDefaultPosition, wxDefaultSize,
                                                    as, wxCB_READONLY);

        sizer_panelcombo->Select(0);
        sizer_panelcombo2->Select(1);
        sizer_panelcombo->SetMinSize(wxSize(150, -1));
        sizer_panelcombo2->SetMinSize(wxSize(150, -1));

        wxRibbonButtonBar* bar = new wxRibbonButtonBar(sizer_panel, wxID_ANY);
        bar->AddButton(ID_BUTTON_XX, "xx", ribbon_xpm);
        bar->AddButton(ID_BUTTON_XY, "xy", ribbon_xpm);
        // This prevents ribbon buttons in panels with sizer from collapsing.
        bar->SetButtonMinSizeClass(ID_BUTTON_XX, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
        bar->SetButtonMinSizeClass(ID_BUTTON_XY, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

        wxSizer* sizer_panelsizer_h = new wxBoxSizer(wxHORIZONTAL);
        wxSizer* sizer_panelsizer_v = new wxBoxSizer(wxVERTICAL);
        sizer_panelsizer_v->AddStretchSpacer(1);
        sizer_panelsizer_v->Add(sizer_panelcombo, 0, wxALL|wxEXPAND, 2);
        sizer_panelsizer_v->Add(sizer_panelcombo2, 0, wxALL|wxEXPAND, 2);
        sizer_panelsizer_v->AddStretchSpacer(1);
        sizer_panelsizer_h->Add(bar, 0, wxEXPAND);
        sizer_panelsizer_h->Add(sizer_panelsizer_v, 0);
        sizer_panel->SetSizer(sizer_panelsizer_h);

        wxFont label_font(wxFontInfo(8).Light());
        m_bitmap_creation_dc.SetFont(label_font);

        wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "Appearance", eye_xpm);
        m_ribbon->GetArtProvider()->GetColourScheme(&m_default_primary,
            &m_default_secondary, &m_default_tertiary);
        wxRibbonPanel *provider_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Art", wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar *provider_bar = new wxRibbonButtonBar(provider_panel, wxID_ANY);
        provider_bar->AddButton(ID_DEFAULT_PROVIDER, "Default Provider",
            wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(32, 32)));
        provider_bar->AddButton(ID_AUI_PROVIDER, "AUI Provider", aui_style_xpm);
        provider_bar->AddButton(ID_MSW_PROVIDER, "MSW Provider", msw_style_xpm);
        wxRibbonPanel *primary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Primary Colour", colours_xpm);
        m_primary_gallery = PopulateColoursPanel(primary_panel,
            m_default_primary, ID_PRIMARY_COLOUR);
        wxRibbonPanel *secondary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Secondary Colour", colours_xpm);
        m_secondary_gallery = PopulateColoursPanel(secondary_panel,
            m_default_secondary, ID_SECONDARY_COLOUR);
    }
    {
        wxRibbonPage* page = new wxRibbonPage(m_ribbon, wxID_ANY, "UI Updated", ribbon_xpm);
        wxRibbonPanel *panel = new wxRibbonPanel(page, wxID_ANY, "Enable/Disable", ribbon_xpm);
        wxRibbonButtonBar *bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_DISABLED, "Disabled", ribbon_xpm);
        bar->AddButton(ID_ENABLE,   "Enable", ribbon_xpm);
        bar->AddButton(ID_DISABLE,  "Disable", ribbon_xpm);
        bar->AddButton(ID_UI_ENABLE_UPDATED, "Enable UI updated", ribbon_xpm);
        bar->EnableButton(ID_DISABLED, false);
        m_bEnabled = true;

        panel = new wxRibbonPanel(page, wxID_ANY, "Toggle", ribbon_xpm);
        bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_CHECK, "Toggle", ribbon_xpm);
        bar->AddToggleButton(ID_UI_CHECK_UPDATED, "Toggled UI updated", ribbon_xpm);
        m_bChecked = true;

        panel = new wxRibbonPanel(page, wxID_ANY, "Change text", ribbon_xpm);
        bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_CHANGE_TEXT1, "One", ribbon_xpm);
        bar->AddButton(ID_CHANGE_TEXT2, "Two", ribbon_xpm);
        bar->AddButton(ID_UI_CHANGE_TEXT_UPDATED, "Zero", ribbon_xpm);

        //Also set the general disabled text colour:
        wxRibbonArtProvider* artProvider = m_ribbon->GetArtProvider();
        wxColour tColour = artProvider->GetColor(wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR);
        artProvider->SetColor(wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR, tColour.MakeDisabled());
    }
    new wxRibbonPage(m_ribbon, wxID_ANY, "Empty Page", empty_xpm);
    {
        wxRibbonPage* page = new wxRibbonPage(m_ribbon, wxID_ANY, "Another Page", empty_xpm);
        wxRibbonPanel *panel = new wxRibbonPanel(page, wxID_ANY, "Page manipulation", ribbon_xpm);
        wxRibbonButtonBar *bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_REMOVE_PAGE, "Remove", wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, wxSize(24, 24)));
        bar->AddButton(ID_HIDE_PAGES, "Hide Pages", ribbon_xpm);
        bar->AddButton(ID_SHOW_PAGES, "Show Pages", ribbon_xpm);

        panel = new wxRibbonPanel(page, wxID_ANY, "Button bar manipulation", ribbon_xpm);
        wxRibbonButtonBar* button_bar = new wxRibbonButtonBar(panel, wxID_ANY);
        button_bar->AddButton(ID_PLUS_MINUS, "+/-",
            wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(24, 24)));
        m_plus_minus_state = false;
        button_bar->AddButton(ID_CHANGE_LABEL, "short", ribbon_xpm);
        button_bar->SetButtonTextMinWidth(ID_CHANGE_LABEL, "some long text");
        m_change_label_state = false;

        panel = new wxRibbonPanel(page, wxID_ANY, "Always medium buttons", ribbon_xpm);
        bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_SMALL_BUTTON_1, "Button 1", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_1, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_2, "Button 2", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_2, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_3, "Button 3", ribbon_xpm);
        bar->AddButton(ID_SMALL_BUTTON_4, "Button 4", ribbon_xpm);
        bar->AddButton(ID_SMALL_BUTTON_5, "Button 5", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_5, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_6, "Button 6", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_6, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
    }
    new wxRibbonPage(m_ribbon, wxID_ANY, "Highlight Page", empty_xpm);
    m_ribbon->AddPageHighlight(m_ribbon->GetPageCount()-1);

    {
        wxRibbonPage* page = new wxRibbonPage(m_ribbon, wxID_ANY, "Advanced", empty_xpm);
        wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Button bar manipulation", ribbon_xpm);
        wxRibbonButtonBar* button_bar = new wxRibbonButtonBar(panel, wxID_ANY);
        button_bar->AddButton(ID_PLUS_MINUS, "+/-",
            wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(24, 24)));
        m_plus_minus_state = false;
        button_bar->AddButton(ID_CHANGE_LABEL, "short", ribbon_xpm);
        button_bar->SetButtonTextMinWidth(ID_CHANGE_LABEL, "some long text");
        m_change_label_state = false;

        panel = new wxRibbonPanel(page, wxID_ANY, "Always medium buttons", ribbon_xpm);
        wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
        bar->AddButton(ID_SMALL_BUTTON_1, "Button 1", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_1, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_2, "Button 2", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_2, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_3, "Button 3", ribbon_xpm);
        bar->AddButton(ID_SMALL_BUTTON_4, "Button 4", ribbon_xpm);
        bar->AddButton(ID_SMALL_BUTTON_5, "Button 5", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_5, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        bar->AddButton(ID_SMALL_BUTTON_6, "Button 6", ribbon_xpm);
        bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_6, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
    }

    m_ribbon->Realize();

    m_logwindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY |
        wxTE_LEFT | wxTE_BESTWRAP | wxBORDER_NONE);

    m_togglePanels = new wxToggleButton(this, ID_TOGGLE_PANELS, "&Toggle panels");
    m_togglePanels->SetValue(true);

    wxSizer *s = new wxBoxSizer(wxVERTICAL);

    s->Add(m_ribbon, 0, wxEXPAND);
    s->Add(m_logwindow, 1, wxEXPAND);
    s->Add(m_togglePanels, wxSizerFlags().Border());

    SetSizer(s);
}

void MyFrame::SetBarStyle(long style)
{
    m_ribbon->Freeze();
    m_ribbon->SetWindowStyleFlag(style);
    wxBoxSizer *pTopSize = reinterpret_cast<wxBoxSizer*>(GetSizer());
    wxRibbonToolBar *pToolbar = wxDynamicCast(FindWindow(ID_MAIN_TOOLBAR), wxRibbonToolBar);
    if(style & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        m_ribbon->SetTabCtrlMargins(10, 10);
        pTopSize->SetOrientation(wxHORIZONTAL);
        if(pToolbar)
            pToolbar->SetRows(3, 5);
    }
    else
    {
        m_ribbon->SetTabCtrlMargins(50, 20);
        pTopSize->SetOrientation(wxVERTICAL);
        if(pToolbar)
            pToolbar->SetRows(2, 3);
    }
    m_ribbon->Realise();
    Layout();
    m_ribbon->Thaw();
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

wxRibbonGallery* MyFrame::PopulateColoursPanel(wxWindow* panel,
                                               wxColour def, int gallery_id)
{
    wxRibbonGallery *gallery = wxDynamicCast(panel->FindWindow(gallery_id), wxRibbonGallery);
    if(gallery)
        gallery->Clear();
    else
        gallery = new wxRibbonGallery(panel, gallery_id);
    wxMemoryDC& dc = m_bitmap_creation_dc;
    wxRibbonGalleryItem *def_item =
        AddColourToGallery(gallery, "Default", dc, &def);
    gallery->SetSelection(def_item);
    AddColourToGallery(gallery, "BLUE", dc);
    AddColourToGallery(gallery, "BLUE VIOLET", dc);
    AddColourToGallery(gallery, "BROWN", dc);
    AddColourToGallery(gallery, "CADET BLUE", dc);
    AddColourToGallery(gallery, "CORAL", dc);
    AddColourToGallery(gallery, "CYAN", dc);
    AddColourToGallery(gallery, "DARK GREEN", dc);
    AddColourToGallery(gallery, "DARK ORCHID", dc);
    AddColourToGallery(gallery, "FIREBRICK", dc);
    AddColourToGallery(gallery, "GOLD", dc);
    AddColourToGallery(gallery, "GOLDENROD", dc);
    AddColourToGallery(gallery, "GREEN", dc);
    AddColourToGallery(gallery, "INDIAN RED", dc);
    AddColourToGallery(gallery, "KHAKI", dc);
    AddColourToGallery(gallery, "LIGHT BLUE", dc);
    AddColourToGallery(gallery, "LIME GREEN", dc);
    AddColourToGallery(gallery, "MAGENTA", dc);
    AddColourToGallery(gallery, "MAROON", dc);
    AddColourToGallery(gallery, "NAVY", dc);
    AddColourToGallery(gallery, "ORANGE", dc);
    AddColourToGallery(gallery, "ORCHID", dc);
    AddColourToGallery(gallery, "PINK", dc);
    AddColourToGallery(gallery, "PLUM", dc);
    AddColourToGallery(gallery, "PURPLE", dc);
    AddColourToGallery(gallery, "RED", dc);
    AddColourToGallery(gallery, "SALMON", dc);
    AddColourToGallery(gallery, "SEA GREEN", dc);
    AddColourToGallery(gallery, "SIENNA", dc);
    AddColourToGallery(gallery, "SKY BLUE", dc);
    AddColourToGallery(gallery, "TAN", dc);
    AddColourToGallery(gallery, "THISTLE", dc);
    AddColourToGallery(gallery, "TURQUOISE", dc);
    AddColourToGallery(gallery, "VIOLET", dc);
    AddColourToGallery(gallery, "VIOLET RED", dc);
    AddColourToGallery(gallery, "WHEAT", dc);
    AddColourToGallery(gallery, "WHITE", dc);
    AddColourToGallery(gallery, "YELLOW", dc);

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
    AddText("Colour \"" + name + "\" selected as primary.");
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
    AddText("Colour \"" + name + "\" selected as secondary.");
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

void MyFrame::OnChangeText1(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_new_text = "One";
}

void MyFrame::OnChangeText2(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_new_text = "Two";
}

void MyFrame::OnEnable(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_bEnabled = true;
}

void MyFrame::OnDisable(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_bEnabled = false;
}

void MyFrame::OnDisabled(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("ERROR: Disabled button activated (not supposed to happen)");
}

void MyFrame::OnEnableUpdated(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Button activated");
}

void MyFrame::OnCheck(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_bChecked = !m_bChecked;
}

void MyFrame::OnEnableUpdateUI(wxUpdateUIEvent& evt)
{
    evt.Enable(m_bEnabled);
}

void MyFrame::OnCheckUpdateUI(wxUpdateUIEvent& evt)
{
    evt.Check(m_bChecked);
}

void MyFrame::OnChangeTextUpdateUI(wxUpdateUIEvent& evt)
{
    if ( !m_new_text.IsEmpty() )
    {
        evt.SetText(m_new_text);
        m_new_text = "";
    }
}

void MyFrame::OnSelectionExpandHButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Expand selection horizontally button clicked.");
}

void MyFrame::OnSelectionExpandVButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Expand selection vertically button clicked.");
}

void MyFrame::OnSelectionContractButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Contract selection button clicked.");
}

void MyFrame::OnCircleButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Circle button clicked.");
}

void MyFrame::OnCrossButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Cross button clicked.");
}

void MyFrame::OnTriangleButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Triangle button clicked.");
}

void MyFrame::OnTriangleDropdown(wxRibbonButtonBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "Equilateral");
    menu.Append(wxID_ANY, "Isosceles");
    menu.Append(wxID_ANY, "Scalene");

    evt.PopupMenu(&menu);
}

void MyFrame::OnSquareButton(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    AddText("Square button clicked.");
}

void MyFrame::OnPolygonDropdown(wxRibbonButtonBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "Pentagon (5 sided)");
    menu.Append(wxID_ANY, "Hexagon (6 sided)");
    menu.Append(wxID_ANY, "Heptagon (7 sided)");
    menu.Append(wxID_ANY, "Octogon (8 sided)");
    menu.Append(wxID_ANY, "Nonagon (9 sided)");
    menu.Append(wxID_ANY, "Decagon (10 sided)");

    evt.PopupMenu(&menu);
}

void MyFrame::OnJustify(wxRibbonToolBarEvent& evt)
{
    long style = m_logwindow->GetWindowStyle() &
        ~(wxTE_LEFT | wxTE_CENTER | wxTE_RIGHT);
    switch(evt.GetId())
    {
        case wxID_JUSTIFY_LEFT:
            m_logwindow->SetWindowStyle(style | wxTE_LEFT);
            break;
        case wxID_JUSTIFY_CENTER:
            m_logwindow->SetWindowStyle(style | wxTE_CENTER);
            break;
        case wxID_JUSTIFY_RIGHT:
            m_logwindow->SetWindowStyle(style | wxTE_RIGHT);
            break;
    }
}

void MyFrame::OnJustifyUpdateUI(wxUpdateUIEvent& evt)
{
    switch(evt.GetId())
    {
        case wxID_JUSTIFY_LEFT:
            evt.Check(!m_logwindow->HasFlag(wxTE_CENTER | wxTE_RIGHT));
            break;
        case wxID_JUSTIFY_CENTER:
            evt.Check(m_logwindow->HasFlag(wxTE_CENTER));
            break;
        case wxID_JUSTIFY_RIGHT:
            evt.Check(m_logwindow->HasFlag(wxTE_RIGHT));
            break;
    }
}

void MyFrame::OnNew(wxRibbonToolBarEvent& WXUNUSED(evt))
{
    AddText("New button clicked.");
}

void MyFrame::OnNewDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "New Document");
    menu.Append(wxID_ANY, "New Template");
    menu.Append(wxID_ANY, "New Mail");

    evt.PopupMenu(&menu);
}

void MyFrame::OnPrint(wxRibbonToolBarEvent& WXUNUSED(evt))
{
    AddText("Print button clicked.");
}

void MyFrame::OnPrintDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "Print");
    menu.Append(wxID_ANY, "Preview");
    menu.Append(wxID_ANY, "Options");

    evt.PopupMenu(&menu);
}

void MyFrame::OnRedoDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "Redo E");
    menu.Append(wxID_ANY, "Redo F");
    menu.Append(wxID_ANY, "Redo G");

    evt.PopupMenu(&menu);
}

void MyFrame::OnUndoDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(wxID_ANY, "Undo C");
    menu.Append(wxID_ANY, "Undo B");
    menu.Append(wxID_ANY, "Undo A");

    evt.PopupMenu(&menu);
}

void MyFrame::OnPositionTopLabels(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle(wxRIBBON_BAR_DEFAULT_STYLE);
}

void MyFrame::OnPositionTopIcons(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle((wxRIBBON_BAR_DEFAULT_STYLE &~wxRIBBON_BAR_SHOW_PAGE_LABELS)
        | wxRIBBON_BAR_SHOW_PAGE_ICONS);
}

void MyFrame::OnPositionTopBoth(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle(wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_SHOW_PAGE_ICONS);
}

void MyFrame::OnPositionLeftLabels(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle(wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_FLOW_VERTICAL);
}

void MyFrame::OnPositionLeftIcons(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle((wxRIBBON_BAR_DEFAULT_STYLE &~wxRIBBON_BAR_SHOW_PAGE_LABELS) |
        wxRIBBON_BAR_SHOW_PAGE_ICONS | wxRIBBON_BAR_FLOW_VERTICAL);
}

void MyFrame::OnPositionLeftBoth(wxCommandEvent& WXUNUSED(evt))
{
    SetBarStyle(wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_SHOW_PAGE_ICONS |
        wxRIBBON_BAR_FLOW_VERTICAL);
}

void MyFrame::OnPositionTop(wxRibbonToolBarEvent& evt)
{
    OnPositionTopLabels(evt);
}

void MyFrame::OnPositionTopDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(ID_POSITION_TOP, "Top with Labels");
    menu.Append(ID_POSITION_TOP_ICONS, "Top with Icons");
    menu.Append(ID_POSITION_TOP_BOTH, "Top with Both");
    evt.PopupMenu(&menu);
}

void MyFrame::OnPositionLeft(wxRibbonToolBarEvent& evt)
{
    OnPositionLeftIcons(evt);
}

void MyFrame::OnPositionLeftDropdown(wxRibbonToolBarEvent& evt)
{
    wxMenu menu;
    menu.Append(ID_POSITION_LEFT, "Left with Icons");
    menu.Append(ID_POSITION_LEFT_LABELS, "Left with Labels");
    menu.Append(ID_POSITION_LEFT_BOTH, "Left with Both");
    evt.PopupMenu(&menu);
}

void MyFrame::OnTogglePanels(wxCommandEvent& WXUNUSED(evt))
{
    m_ribbon->ShowPanels(m_togglePanels->GetValue());
}

void MyFrame::OnExtButton(wxRibbonPanelEvent& WXUNUSED(evt))
{
    wxMessageBox("Extension button clicked");
}

void MyFrame::AddText(wxString msg)
{
    m_logwindow->AppendText(msg);
    m_logwindow->AppendText("\n");
    m_ribbon->DismissExpandedPanel();
}

wxRibbonGalleryItem* MyFrame::AddColourToGallery(wxRibbonGallery *gallery,
                                 wxString colour, wxMemoryDC& dc,
                                 wxColour* value)
{
    wxRibbonGalleryItem* item = NULL;
    wxColour c;
    if (colour != "Default")
        c = wxColour(colour);
    if (!c.IsOk())
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
        wxRibbonGalleryEvent dummy(wxEVT_RIBBONGALLERY_SELECTED, gallery->GetId());
        dummy.SetEventObject(gallery);
        dummy.SetGallery(gallery);
        dummy.SetGalleryItem(item);
        ProcessWindowEvent(dummy);
    }
}

void MyFrame::OnDefaultProvider(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_ribbon->DismissExpandedPanel();
    SetArtProvider(new wxRibbonDefaultArtProvider);
}

void MyFrame::OnAUIProvider(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_ribbon->DismissExpandedPanel();
    SetArtProvider(new wxRibbonAUIArtProvider);
}

void MyFrame::OnMSWProvider(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_ribbon->DismissExpandedPanel();
    SetArtProvider(new wxRibbonMSWArtProvider);
}

void MyFrame::SetArtProvider(wxRibbonArtProvider *prov)
{
    m_ribbon->Freeze();
    m_ribbon->SetArtProvider(prov);

    prov->GetColourScheme(&m_default_primary, &m_default_secondary,
        &m_default_tertiary);
    PopulateColoursPanel(m_primary_gallery->GetParent(), m_default_primary,
        ID_PRIMARY_COLOUR);
    PopulateColoursPanel(m_secondary_gallery->GetParent(), m_default_secondary,
        ID_SECONDARY_COLOUR);

    m_ribbon->Realize();
    m_ribbon->Thaw();
    GetSizer()->Layout();
}

void MyFrame::OnRemovePage(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    size_t n = m_ribbon->GetPageCount();
    if(n > 0)
    {
        m_ribbon->DeletePage(n-1);
        m_ribbon->Realize();
    }
}

void MyFrame::OnHidePages(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_ribbon->HidePage(1);
    m_ribbon->HidePage(2);
    m_ribbon->HidePage(3);
    m_ribbon->Realize();
}

void MyFrame::OnShowPages(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
    m_ribbon->ShowPage(1);
    m_ribbon->ShowPage(2);
    m_ribbon->ShowPage(3);
    m_ribbon->Realize();
}

void MyFrame::OnPlusMinus(wxRibbonButtonBarEvent& evt)
{
    if(m_plus_minus_state)
    {
        evt.GetBar()->SetButtonIcon(ID_PLUS_MINUS,
            wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(24, 24)));
        m_plus_minus_state = false;
    }
    else
    {
        evt.GetBar()->SetButtonIcon(ID_PLUS_MINUS,
            wxArtProvider::GetBitmap(wxART_MINUS, wxART_OTHER, wxSize(24, 24)));
        m_plus_minus_state = true;
    }
}

void MyFrame::OnChangeLabel(wxRibbonButtonBarEvent& evt)
{
    if(m_change_label_state)
    {
        evt.GetBar()->SetButtonText(ID_CHANGE_LABEL, "short");
        m_change_label_state = false;
    }
    else
    {
        evt.GetBar()->SetButtonText(ID_CHANGE_LABEL, "some long text");
        m_change_label_state = true;
    }
}

void MyFrame::OnRibbonBarToggled(wxRibbonBarEvent& WXUNUSED(evt))
{
    AddText(wxString::Format("Ribbon bar %s.",
                             m_ribbon->ArePanelsShown()
                                ? "expanded"
                                : "collapsed"));
}

void MyFrame::OnRibbonBarHelpClicked(wxRibbonBarEvent& WXUNUSED(evt))
{
    AddText("Ribbon bar help clicked");
}

// This shows how to hide ribbon dynamically if there is not enough space.
void MyFrame::OnSizeEvent(wxSizeEvent& evt)
{
    if ( evt.GetSize().GetWidth() < 200 )
        m_ribbon->Hide();
    else
        m_ribbon->Show();

    evt.Skip();
}
