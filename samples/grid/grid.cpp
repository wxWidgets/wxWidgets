/*
 * File:    grid.cpp
 * Purpose: wxGrid test

   PLEASE NOTE: this sample is deprecated. See
   newgrid for a sample based on the newer wxGrid API.

 * Author:  Julian Smart
 * Created: 1995
 * Updated:
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/grid.h"
#include "wx/colordlg.h"

// Define a new application type
class MyApp: public wxApp
{
public:
    virtual bool OnInit(void);
    virtual int OnExit();
};


// Define a new frame type
class MyFrame: public wxFrame
{ public:
    wxGrid *grid;
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void ToggleEditable(wxCommandEvent& event);
    void ToggleEditInPlace(wxCommandEvent& event);
    void ToggleRowLabel(wxCommandEvent& event);
    void ToggleColLabel(wxCommandEvent& event);
    void ToggleDividers(wxCommandEvent& event);
    void LeftCell(wxCommandEvent& event);
    void CentreCell(wxCommandEvent& event);
    void RightCell(wxCommandEvent& event);
    void ColourLabelBackground(wxCommandEvent& event);
    void ColourLabelText(wxCommandEvent& event);
    void NormalLabelColouring(wxCommandEvent& event);
    void ColourCellBackground(wxCommandEvent& event);
    void ColourCellText(wxCommandEvent& event);
    void NormalCellColouring(wxCommandEvent& event);
    void Quit(wxCommandEvent& event);

    void OnActivate(wxActivateEvent& event);

DECLARE_EVENT_TABLE()
};

wxBitmap *cellBitmap1 = (wxBitmap *) NULL;
wxBitmap *cellBitmap2 = (wxBitmap *) NULL;

// ID for the menu quit command
#define GRID_QUIT 1
#define GRID_TOGGLE_EDITABLE 2
#define GRID_TOGGLE_EDITINPLACE 22
#define GRID_LEFT_CELL       3
#define GRID_CENTRE_CELL     4
#define GRID_RIGHT_CELL      5
#define GRID_TOGGLE_ROW_LABEL 6
#define GRID_TOGGLE_COL_LABEL 7
#define GRID_COLOUR_LABEL_BACKGROUND 8
#define GRID_COLOUR_LABEL_TEXT       9
#define GRID_NORMAL_LABEL_COLOURING  10
#define GRID_COLOUR_CELL_BACKGROUND 11
#define GRID_COLOUR_CELL_TEXT       12
#define GRID_NORMAL_CELL_COLOURING  13
#define GRID_TOGGLE_DIVIDERS        14

// Main proc

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
#ifdef __WXMSW__
    cellBitmap1 = new wxBitmap(_T("bitmap1"));
    cellBitmap2 = new wxBitmap(_T("bitmap2"));
#endif

    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL,  _T("wxGrid Sample"), wxPoint(50, 50), wxSize(450, 300));

    // Give it an icon
#ifdef __WXMSW__
    frame->SetIcon(wxIcon(_T("mondrian")));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(GRID_QUIT, _T("E&xit"));

    wxMenu *settings_menu = new wxMenu;
    settings_menu->Append(GRID_TOGGLE_EDITABLE, _T("&Toggle editable"));
    settings_menu->Append(GRID_TOGGLE_EDITINPLACE, _T("&Toggle edit in place"));
    settings_menu->Append(GRID_TOGGLE_ROW_LABEL, _T("Toggle ro&w label"));
    settings_menu->Append(GRID_TOGGLE_COL_LABEL, _T("Toggle co&l label"));
    settings_menu->Append(GRID_TOGGLE_DIVIDERS, _T("Toggle &dividers"));
    settings_menu->AppendSeparator();
    settings_menu->Append(GRID_LEFT_CELL, _T("&Left cell alignment "));
    settings_menu->Append(GRID_CENTRE_CELL, _T("&Centre cell alignment "));
    settings_menu->Append(GRID_RIGHT_CELL, _T("&Right cell alignment "));
    settings_menu->AppendSeparator();
    settings_menu->Append(GRID_COLOUR_LABEL_BACKGROUND, _T("Choose a label &background colour"));
    settings_menu->Append(GRID_COLOUR_LABEL_TEXT, _T("Choose a label fore&ground colour"));
    settings_menu->Append(GRID_NORMAL_LABEL_COLOURING, _T("&Normal label colouring"));
    settings_menu->AppendSeparator();
    settings_menu->Append(GRID_COLOUR_CELL_BACKGROUND, _T("Choo&se a cell &background colour"));
    settings_menu->Append(GRID_COLOUR_CELL_TEXT, _T("Choose &a cell foreground colour"));
    settings_menu->Append(GRID_NORMAL_CELL_COLOURING, _T("N&ormal cell colouring"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(settings_menu, _T("&Settings"));
    frame->SetMenuBar(menu_bar);

    // Make a grid
    frame->grid = new wxGrid(frame, 0, 0, 400, 400);

    frame->grid->CreateGrid(10, 8);
    frame->grid->SetColumnWidth(3, 200);
    frame->grid->SetRowHeight(4, 45);
    frame->grid->SetCellValue(_T("First cell"), 0, 0);
    frame->grid->SetCellValue(_T("Another cell"), 1, 1);
    frame->grid->SetCellValue(_T("Yet another cell"), 2, 2);
    frame->grid->SetCellTextFont(wxFont(10, wxROMAN, wxITALIC, wxNORMAL), 0, 0);
    frame->grid->SetCellTextColour(*wxRED, 1, 1);
    frame->grid->SetCellBackgroundColour(*wxCYAN, 2, 2);
    if (cellBitmap1 && cellBitmap2)
    {
        frame->grid->SetCellAlignment(wxCENTRE, 5, 0);
        frame->grid->SetCellAlignment(wxCENTRE, 6, 0);
        frame->grid->SetCellBitmap(cellBitmap1, 5, 0);
        frame->grid->SetCellBitmap(cellBitmap2, 6, 0);
    }

    frame->grid->UpdateDimensions();

    // Show the frame
    frame->Show(TRUE);

    wxMessageBox(wxT("Please note: this is an obsolete sample using the old wxGrid API.\nPlease compile newgrid instead."), wxT("wxGrid"), wxICON_INFORMATION|wxOK, frame);

    SetTopWindow(frame);
    return TRUE;
}

int MyApp::OnExit()
{
    if (cellBitmap1)
    {
        delete cellBitmap1;
        cellBitmap1 = (wxBitmap *) NULL;
    }

    if (cellBitmap2)
    {
        delete cellBitmap2;
        cellBitmap1 = (wxBitmap *) NULL;
    }

    // exit code is 0, everything is ok
    return 0;
}


// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title,
                 const wxPoint& pos, const wxSize& size):
    wxFrame(frame, -1, title, pos, size)
{
    grid = (wxGrid*) NULL;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(GRID_TOGGLE_EDITABLE, MyFrame::ToggleEditable)
    EVT_MENU(GRID_TOGGLE_EDITINPLACE, MyFrame::ToggleEditInPlace)
    EVT_MENU(GRID_TOGGLE_ROW_LABEL, MyFrame::ToggleRowLabel)
    EVT_MENU(GRID_TOGGLE_COL_LABEL, MyFrame::ToggleColLabel)
    EVT_MENU(GRID_TOGGLE_DIVIDERS, MyFrame::ToggleDividers)
    EVT_MENU(GRID_LEFT_CELL, MyFrame::LeftCell)
    EVT_MENU(GRID_CENTRE_CELL, MyFrame::CentreCell)
    EVT_MENU(GRID_RIGHT_CELL, MyFrame::RightCell)
    EVT_MENU(GRID_COLOUR_LABEL_BACKGROUND, MyFrame::ColourLabelBackground)
    EVT_MENU(GRID_COLOUR_LABEL_TEXT, MyFrame::ColourLabelText)
    EVT_MENU(GRID_NORMAL_LABEL_COLOURING, MyFrame::NormalLabelColouring)
    EVT_MENU(GRID_COLOUR_CELL_BACKGROUND, MyFrame::ColourCellBackground)
    EVT_MENU(GRID_COLOUR_CELL_TEXT, MyFrame::ColourCellText)
    EVT_MENU(GRID_NORMAL_CELL_COLOURING, MyFrame::NormalCellColouring)
    EVT_MENU(GRID_QUIT, MyFrame::Quit)
END_EVENT_TABLE()

void MyFrame::ToggleEditable(wxCommandEvent& WXUNUSED(event))
{
    grid->SetEditable(!grid->GetEditable());
    grid->Refresh();
}

void MyFrame::ToggleEditInPlace(wxCommandEvent& WXUNUSED(event))
{
    grid->SetEditInPlace(!grid->GetEditInPlace());
    grid->Refresh();
}

void MyFrame::ToggleRowLabel(wxCommandEvent& WXUNUSED(event))
{
    if (grid->GetLabelSize(wxVERTICAL) > 0)
        grid->SetLabelSize(wxVERTICAL, 0);
    else
        grid->SetLabelSize(wxVERTICAL, 40);

    grid->Refresh();
}

void MyFrame::ToggleColLabel(wxCommandEvent& WXUNUSED(event))
{
    if (grid->GetLabelSize(wxHORIZONTAL) > 0)
        grid->SetLabelSize(wxHORIZONTAL, 0);
    else
        grid->SetLabelSize(wxHORIZONTAL, 20);

      grid->Refresh();
}

void MyFrame::ToggleDividers(wxCommandEvent& WXUNUSED(event))
{
    if (!grid->GetDividerPen().Ok())
        grid->SetDividerPen(wxPen(wxT("LIGHT GREY"), 1, wxSOLID));
    else
        grid->SetDividerPen(wxNullPen);

    grid->Refresh();
}

void MyFrame::LeftCell(wxCommandEvent& WXUNUSED(event))
{
    grid->SetCellAlignment(wxLEFT);
    grid->Refresh();
}

void MyFrame::CentreCell(wxCommandEvent& WXUNUSED(event))
{
    grid->SetCellAlignment(wxCENTRE);
    grid->Refresh();
}

void MyFrame::RightCell(wxCommandEvent& WXUNUSED(event))
{
    grid->SetCellAlignment(wxRIGHT);
    grid->Refresh();
}

void MyFrame::ColourLabelBackground(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(TRUE);
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() != wxID_CANCEL)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetLabelBackgroundColour(col);
        grid->Refresh();
    }
}

void MyFrame::ColourLabelText(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(TRUE);
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() != wxID_CANCEL)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetLabelTextColour(col);
        grid->Refresh();
    }
}

void MyFrame::NormalLabelColouring(wxCommandEvent& WXUNUSED(event))
{
    grid->SetLabelBackgroundColour(*wxLIGHT_GREY);
    grid->SetLabelTextColour(*wxBLACK);
    grid->Refresh();
}

void MyFrame::ColourCellBackground(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(TRUE);
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() != wxID_CANCEL)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetCellBackgroundColour(col);
        grid->Refresh();
    }
}

void MyFrame::ColourCellText(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(TRUE);
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() != wxID_CANCEL)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetCellTextColour(col);
        grid->Refresh();
    }
}

void MyFrame::NormalCellColouring(wxCommandEvent& WXUNUSED(event))
{
    grid->SetCellBackgroundColour(*wxWHITE);
    grid->SetCellTextColour(*wxBLACK);
    grid->Refresh();
}

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
    this->Close(TRUE);
}

// Ensure that the grid's edit control always has the focus.
void MyFrame::OnActivate(wxActivateEvent& event)
{
    if (grid) grid->OnActivate(event.GetActive());
}

