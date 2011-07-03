/////////////////////////////////////////////////////////////////////////////
// Name:        demoframe.cpp
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     12/05/2009 12:30:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"
#include "wx/settings.h"

////@begin includes
////@end includes

#include "wx/mobile/button.h"
#include "wx/mobile/stattext.h"
#include "wx/mobile/bmpbutton.h"
#include "wx/mobile/switch.h"
#include "wx/mobile/tabctrl.h"
#include "wx/mobile/toolbar.h"
#include "wx/mobile/navbar.h"
#include "wx/mobile/notebook.h"
#include "wx/mobile/textctrl.h"
#include "wx/mobile/searchctrl.h"
#include "wx/mobile/statbmp.h"
#include "wx/mobile/navctrl.h"
#include "wx/mobile/scrollwin.h"
#include "wx/mobile/listbox.h"
#include "wx/mobile/wheelsctrl.h"
#include "wx/mobile/pagectrl.h"
#include "wx/mobile/gauge.h"
#include "wx/mobile/segctrl.h"
#include "wx/mobile/slider.h"
#include "wx/mobile/tablectrl.h"
#include "wx/mobile/webctrl.h"

#include "wx/mstream.h"

#include "wxwidgets.inc"
#include "toucan.inc"
#include "largedot.inc"
#include "smalldot.inc"

#include "demoframe.h"

////@begin XPM images
#include "clock.xpm"
////@end XPM images

#include "app.xpm"

class DemoViewController: public wxMoViewController
{
public:
    DemoViewController(const wxString& title, wxMoNavigationCtrl* ctrl, int viewNumber): wxMoViewController(title)
    {
        m_navCtrl = ctrl;
        m_viewNumber = viewNumber;

        // Create off-screen to avoid flicker
        wxMoPanel* panel = new wxMoPanel(ctrl, wxID_ANY, wxPoint(-100, -100), wxSize(10, 10));
        panel->Show(false);
        panel->SetBackgroundColour(wxColour(wxT("#E3E4FF")));
        SetWindow(panel);

        wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(sizer);

        sizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("This is a navigation control test.\n\nPress the button below to create a new view,\nand the back button above to return to the previous view."),
            wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE),
            0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

        sizer->AddStretchSpacer();

        wxString label(wxString::Format(_("Create Demo View %d"), viewNumber+1));

        wxMoButton* button = new wxMoButton(panel, wxID_ANY, label);
        sizer->Add(button, 0, wxALIGN_CENTER_HORIZONTAL);

        sizer->AddStretchSpacer();

        panel->Connect(button->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DemoViewController::OnNewView), NULL, this);
    }

    void OnNewView(wxCommandEvent& WXUNUSED(event))
    {
        m_navCtrl->PushController(new DemoViewController(wxString::Format(_("Demo View %d"), m_viewNumber+1), m_navCtrl, m_viewNumber+1));
    }

protected:
    wxMoNavigationCtrl* m_navCtrl;
    int                 m_viewNumber;
};

class DemoCanvas: public wxMoScrolledWindow
{
    DECLARE_EVENT_TABLE()
public:
    DemoCanvas(wxWindow* parent, wxWindowID id = wxID_ANY):
      wxMoScrolledWindow(parent, id)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        SetScrollbars(1, 1, 1000, 1000);
        wxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        SetFont(font);
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
#ifdef __WXMSW__
        wxBufferedPaintDC dc(this);
#else
        wxPaintDC dc(this);
#endif
        PrepareDC(dc);
        dc.SetBackground(*wxWHITE);
        dc.Clear();

        dc.SetFont(GetFont());
        dc.SetTextForeground(*wxBLACK);

        int w, h;
        dc.GetTextExtent(wxT("X"), & w, & h);

        int margin = 5;

        size_t i;
        for (i = 0; i < 200; i++)
        {
            int x = 10;
            int y = i * (h + margin);

            wxString text(wxString::Format(wxT("This is scroll window test line %d"), i));
            dc.DrawText(text, x, y);
        }
    }
};

BEGIN_EVENT_TABLE(DemoCanvas, wxMoScrolledWindow)
    EVT_PAINT(DemoCanvas::OnPaint)
END_EVENT_TABLE()

// This data source demonstrates plain and grouped tables

class MultiSectionDataSource: public wxMoTableDataSource
{
public:
    MultiSectionDataSource(wxMoTableCell::wxMoTableCellStyle cellStyle = wxMoTableCell::CellStyleDefault) { m_cellStyle = cellStyle; }

    /// Returns a table cell for the give location.
    virtual wxMoTableCell* GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxMoTableCtrl* ctrl) const;

    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxMoTableCtrl* ctrl, int section) const;

    /// Returns the number of rows in a section.
    virtual wxArrayString GetSectionTitles(wxMoTableCtrl* ctrl) const;

    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const;

    wxMoTableCell::wxMoTableCellStyle m_cellStyle;
};

// This data source demonstrates plain and grouped tables with editing

class EditableDataSource: public wxMoTableDataSource
{
public:
    EditableDataSource(wxMoTableCell::wxMoTableCellStyle cellStyle = wxMoTableCell::CellStyleDefault);

    /// Returns a table cell for the give location.
    virtual wxMoTableCell* GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxMoTableCtrl* ctrl) const;

    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxMoTableCtrl* ctrl, int section) const;

    /// Returns the number of rows in a section.
    virtual wxArrayString GetSectionTitles(wxMoTableCtrl* ctrl) const;

    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const;

    /// Called by the table control to commit an insertion requested by the user. This function should
    /// then call InsertRows in response.
    virtual bool CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Called by the table control to commit a deletion requested by the user. This function should
    /// then call DeleteRows in response.
    virtual bool CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Returns true if this row can be edited.
    virtual bool CanEditRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return true; }

    /// Returns true if this row can be moved.
    virtual bool CanMoveRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return true; }

    /// Requests a move operation from 'from' to 'to'. The data should then reflect
    /// the new ordering. Return false if the reordering was not done.
    virtual bool MoveRow(wxMoTableCtrl* ctrl, const wxTablePath& pathFrom, const wxTablePath& pathTo);

    /// Get the edit style for a row
    virtual wxMoTableCtrl::CellEditStyle GetCellEditStyle(wxMoTableCtrl* ctrl, const wxTablePath& path);

    wxArrayString m_rowLabels;
    wxArrayInt    m_rowData;

    wxMoTableCell::wxMoTableCellStyle m_cellStyle;
};

// This data source populates the root table in the Demo tab

class TableDemoRootDataSource: public wxMoTableDataSource
{
public:
    TableDemoRootDataSource(wxMoNavigationCtrl* ctrl) { m_navCtrl = ctrl; }

    /// Returns a table cell for the given location.
    virtual wxMoTableCell* GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxMoTableCtrl* ctrl) const;

    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxMoTableCtrl* ctrl, int section) const;

    /// Returns the number of rows in a section.
    virtual wxArrayString GetSectionTitles(wxMoTableCtrl* ctrl) const;

    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const;

    void OnSelectRow(wxTableCtrlEvent& event);

    // Utility to push table onto the navigation controller
    void PushTable(wxMoTableCtrl* tableCtrl, wxMoTableDataSource* dataSource, const wxString& title);

    // Utility to push a window onto the navigation controller
    void PushWindow(wxWindow* win, const wxString& title);

    DECLARE_EVENT_TABLE()
protected:
    wxMoNavigationCtrl* m_navCtrl;
};

// A navigation controller for Table tab, starting off with the root data source

class TableDemoController: public wxMoViewController
{
public:
    TableDemoController(const wxString& title, wxMoNavigationCtrl* ctrl): wxMoViewController(title)
    {
        m_navCtrl = ctrl;

        // Create off-screen to avoid flicker
        wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(ctrl, wxID_ANY, wxPoint(-100, -100), wxSize(10, 10),
            wxTC_GROUPED);
        tableCtrl->SetDataSource(new TableDemoRootDataSource(ctrl), true);
        tableCtrl->ReloadData();
        SetWindow(tableCtrl);
    }

protected:
    wxMoNavigationCtrl* m_navCtrl;
};

/*
 * DemoFrame type definition
 */

IMPLEMENT_CLASS( DemoFrame, wxMoFrame )

/*
 * DemoFrame event table definition
 */

BEGIN_EVENT_TABLE( DemoFrame, wxMoFrame )

    EVT_SHEET_BUTTON(DemoFrame::OnActionSheet)
    EVT_BUTTON(wxID_PROPERTIES, DemoFrame::OnShowActionSheet)
    EVT_BUTTON(wxID_VIEW_DETAILS, DemoFrame::OnShowAlertSheet)

    EVT_TABLE_ROW_SELECTED(wxID_ANY, DemoFrame::OnSelectRow)
    EVT_TABLE_ACCESSORY_CLICKED(wxID_ANY, DemoFrame::OnAccessoryClick)

////@begin DemoFrame event table entries
////@end DemoFrame event table entries

END_EVENT_TABLE()

/*
 * DemoFrame constructors
 */

DemoFrame::DemoFrame()
{
    Init();
}

DemoFrame::DemoFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * DemoFrame creator
 */

bool DemoFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    style = 0;

    wxMoFrame::Create( parent, id, caption, pos, size, style );
    CreateControls();
    
    return true;
}


/*
 * DemoFrame destructor
 */

DemoFrame::~DemoFrame()
{
////@begin DemoFrame destruction
////@end DemoFrame destruction
}


/*
 * Member initialisation
 */

void DemoFrame::Init()
{
////@begin DemoFrame member initialisation
////@end DemoFrame member initialisation
}


/*
 * Control creation for DemoFrame
 */

void DemoFrame::CreateControls()
{
//// @begin DemoFrame content construction
//// @end DemoFrame content construction
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);
    
    wxMoNotebook* notebook = new wxMoNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxBK_BOTTOM|wxBK_TEXT|wxBK_BITMAP|wxNO_BORDER);
    topSizer->Add(notebook, 1, wxEXPAND);

    wxMoPanel* page1 = new wxMoPanel(notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    notebook->AddPage(page1, _("Demo"), true, wxID_TOPRATED);
    // Items for page 1
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        page1->SetSizer(sizer);
        
        wxMoNavigationCtrl* navCtrl = new wxMoNavigationCtrl(page1, wxID_ANY);
        sizer->Add(navCtrl, 1, wxEXPAND, 0);
        
        navCtrl->PushController(new TableDemoController(_("iPhone Demo"), navCtrl));
    }
    
    wxMoPanel* page2 = new wxMoPanel(notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    notebook->AddPage(page2, _("Navigation"), false, wxID_SEARCH);

    // Items for page 2
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        page2->SetSizer(sizer);

        wxMoNavigationCtrl* navCtrl = new wxMoNavigationCtrl(page2, wxID_ANY);
        sizer->Add(navCtrl, 1, wxEXPAND, 0);

        navCtrl->PushController(new DemoViewController(_("Demo View 1"), navCtrl, 1));
    }

    wxMoPanel* page3 = new wxMoPanel(notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    notebook->AddPage(page3, _("Scrolling"), false, wxID_CONTACTS);
    notebook->SetBadge(2, wxT("1"));

    // Items for page 3
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        page3->SetSizer(sizer);

        // Toolbar
        
        wxMoToolBar* toolbar = new wxMoToolBar(page3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_BLACK_OPAQUE_BG);
        sizer->Add(toolbar, 0, wxEXPAND, 0);
        
        toolbar->AddTool(100, _("First tool"), wxBitmap(app_xpm));
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_REFRESH, wxEmptyString, wxNullBitmap);
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_CAMERA, wxEmptyString, wxNullBitmap);
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_ADD, wxEmptyString, wxNullBitmap);
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_COMPOSE, wxEmptyString, wxNullBitmap);
        toolbar->AddSeparator();
        toolbar->AddTool(wxID_TRASH, wxEmptyString, wxNullBitmap);
        toolbar->AddSeparator();
        toolbar->Realize();

        DemoCanvas* canvas = new DemoCanvas(page3, wxID_ANY);
        sizer->Add(canvas, 1, wxEXPAND, 0);
    }
}


/*
 * Should we show tooltips?
 */

bool DemoFrame::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap DemoFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DemoFrame bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("clock.xpm"))
    {
        wxBitmap bitmap(clock_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end DemoFrame bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon DemoFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DemoFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DemoFrame icon retrieval
}

// Respond to action sheet events
void DemoFrame::OnActionSheet(wxSheetEvent& event)
{
    wxMoSheetBase* sheet = event.GetSheet();
    sheet->DismissSheet(event.GetId());
    sheet->Destroy();
}

/// Show an action sheet
void DemoFrame::OnShowActionSheet(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString otherButtons;
    otherButtons.Add(_("First other button"));
    otherButtons.Add(_("Another other button"));

    wxMoActionSheet* sheet = new wxMoActionSheet(this,
        _("Testing an action sheet, with a relatively long title on it."),
        _("Cancel"), _("OK"), otherButtons);

    // Redirect events to this window
    sheet->SetOwner(this);
    sheet->ShowSheet(NULL);
}

/// Show an alert sheet
void DemoFrame::OnShowAlertSheet(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString otherButtons;
    otherButtons.Add(_("First other button"));
    otherButtons.Add(_("Another other button"));

    wxMoAlertSheet* sheet = new wxMoAlertSheet(this,
        _("Testing an alert sheet"),
        _("This is the message for the alert sheet, and it can be quite long if necessary."),
        _("Cancel"), otherButtons);

    // Redirect events to this window
    sheet->SetOwner(this);
    sheet->ShowSheet(NULL);
}

void DemoFrame::OnAccessoryClick(wxTableCtrlEvent& event)
{
    wxString msg(wxString::Format(_("Clicked accessory at section %d, row %d"),
        int(event.GetPath().GetSection()+1), int(event.GetPath().GetRow()+1)));
    wxIBMessageBox(msg, _("Table Event"));
}

void DemoFrame::OnSelectRow(wxTableCtrlEvent& event)
{
    wxString msg(wxString::Format(_("Selected row at section %d, row %d"),
        int(event.GetPath().GetSection()+1), int(event.GetPath().GetRow()+1)));
    //wxIBMessageBox(msg, _("Table Event"));
}


// Returns a table cell for the give location.
wxMoTableCell* MultiSectionDataSource::GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    int section = path.GetSection();
    int row = path.GetRow();

    wxString rowText(wxString::Format(_("Section %d, row %d"), int(section+1), int(row+1)));

    int accessory = section;
    wxString reuseName;
    if (accessory == 3)
        reuseName = wxT("DemoCellWithSwitch");
    else
        reuseName = wxT("DemoCell");

    wxMoTableCell* cell = ctrl->GetReusableCell(reuseName);
        
    if (!cell)
        cell = new wxMoTableCell(ctrl, reuseName);
    cell->SetText(rowText);
    cell->SetDetailText(_("Detail text."));
    cell->SetCellStyle(m_cellStyle);

    if (ctrl->HasFlag(wxTC_GROUPED))
    {
        if (accessory == 0)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeDisclosureIndicator);
        else if (accessory == 1)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeDetailDisclosureButton);
        else if (accessory == 2)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeCheckmark);
        else if (accessory == 3)
        {
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeNone);
            cell->SetAccessoryWindow(new wxMoSwitchCtrl(ctrl, wxID_ANY));
        }
    }

    if (accessory == 4)
    {
        cell->SetIndentationWidth(20);
        cell->SetIndentationLevel(row);
    }
    else
        cell->SetIndentationLevel(0);

    {
        wxMemoryInputStream is(wxwidgets_png, sizeof(wxwidgets_png));
        wxBitmap bitmap = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);

        cell->SetBitmap(bitmap);
    }

    return cell;
}

// Returns the number of sections in the table.
int MultiSectionDataSource::GetSectionCount(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    return 10;
}

// Returns the number of rows in a section.
int MultiSectionDataSource::GetRowCount(wxMoTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const
{
    return 10;
}

// Returns the number of rows in a section.
wxArrayString MultiSectionDataSource::GetSectionTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString titles;
    size_t i;
    for (i = 0; i < 10; i++)
    {
        wxString sectionText(wxString::Format(_("Welcome to section %d"), int(i+1)));
        titles.Add(sectionText);
    }
    return titles;
}

/// Returns an array of strings, each one corresponding to an index title to be shown
/// on the right hand side of a plain list.
wxArrayString MultiSectionDataSource::GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString index;
    size_t i;
    for (i = 0; i < 10; i++)
    {
        index.Add(wxString::Format(wxT("%c"), (char) 'A' + i));
    }
    return index;
}


BEGIN_EVENT_TABLE(TableDemoRootDataSource, wxMoTableDataSource)
    EVT_TABLE_ROW_SELECTED(wxID_ANY, TableDemoRootDataSource::OnSelectRow)
END_EVENT_TABLE()

// Returns a table cell for the give location.
wxMoTableCell* TableDemoRootDataSource::GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    int section = path.GetSection();
    int row = path.GetRow();

    wxMoTableCell* cell = ctrl->GetReusableCell(wxT("DemoCell"));
    if (!cell) {
        cell = new wxMoTableCell(ctrl, wxT("DemoCell"));
    }

    if (section == 0)
    {
        wxArrayString titles;
        titles.Add(_("Grouped table - Default style"));
        titles.Add(_("Grouped table - Value1 style"));
        titles.Add(_("Grouped table - Value2 style"));
        titles.Add(_("Grouped table - Subtitle style"));
        titles.Add(_("Plain table demo"));
        titles.Add(_("Editable table demo"));
        
        cell->SetText(titles[row]);
    }
    else if (section == 1)
    {
        wxArrayString titles;
        titles.Add(_("Basic controls"));
        titles.Add(_("More controls"));
        titles.Add(_("Picker controls"));
        
        cell->SetText(titles[row]);
    }
    else if (section == 2)
    {
        wxArrayString titles;
        titles.Add(_("Text & Image Toolbar"));
        titles.Add(_("Text Button Toolbar"));
        
        cell->SetText(titles[row]);
    }

    cell->SetAccessoryType(wxMoTableCell::AccessoryTypeDisclosureIndicator);

    {
        wxMemoryInputStream is(wxwidgets_png, sizeof(wxwidgets_png));
        wxImage image = wxImage(is, wxBITMAP_TYPE_ANY, -1);
        wxBitmap bitmap = wxBitmap(image, -1);

        cell->SetBitmap(bitmap);
    }

    return cell;
}

// Returns the number of sections in the table.
int TableDemoRootDataSource::GetSectionCount(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    return 3;
}

// Returns the number of rows in a section.
int TableDemoRootDataSource::GetRowCount(wxMoTableCtrl* WXUNUSED(ctrl), int section) const
{
    if (section == 0)
        return 6;
    else if (section == 1)
        return 3;
    else if (section == 2)
        return 2;

    return 0;
}

// Returns the number of rows in a section.
wxArrayString TableDemoRootDataSource::GetSectionTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString titles;
    titles.Add(_("Tables"));
    titles.Add(_("Controls"));
    titles.Add(_("Bars"));
    return titles;
}

/// Returns an array of strings, each one corresponding to an index title to be shown
/// on the right hand side of a plain list.
wxArrayString TableDemoRootDataSource::GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString index;
    return index;
}

void TableDemoRootDataSource::OnSelectRow(wxTableCtrlEvent& event)
{
    wxTablePath path = event.GetPath();

    wxMoTableCtrl* tableCtrl = event.GetTableCtrl();
    if (tableCtrl)
        tableCtrl->Deselect(event.GetPath());

    if (path.GetSection() == 0)
    {
        // Tables
        if (path.GetRow() == 0)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            MultiSectionDataSource* dataSource = new MultiSectionDataSource(wxMoTableCell::CellStyleDefault);
            PushTable(tableCtrl, dataSource, _("Grouped Table"));
        }
        else if (path.GetRow() == 1)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            MultiSectionDataSource* dataSource = new MultiSectionDataSource(wxMoTableCell::CellStyleValue1);
            PushTable(tableCtrl, dataSource, _("Grouped Table"));
        }
        else if (path.GetRow() == 2)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            MultiSectionDataSource* dataSource = new MultiSectionDataSource(wxMoTableCell::CellStyleValue2);
            PushTable(tableCtrl, dataSource, _("Grouped Table"));
        }
        else if (path.GetRow() == 3)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            MultiSectionDataSource* dataSource = new MultiSectionDataSource(wxMoTableCell::CellStyleSubtitle);
            PushTable(tableCtrl, dataSource, _("Grouped Table"));
        }
        else if (path.GetRow() == 4)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_PLAIN);
            MultiSectionDataSource* dataSource = new MultiSectionDataSource;
            PushTable(tableCtrl, dataSource, _("Plain Table"));
        }
        else if (path.GetRow() == 5)
        {
            wxMoTableCtrl* tableCtrl = new wxMoTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            EditableDataSource* dataSource = new EditableDataSource;
            PushTable(tableCtrl, dataSource, _("Editable Table"));
            tableCtrl->SetEditingMode(true);
        }
    }
    else if (path.GetSection() == 1)
    {
        // Controls
        if (path.GetRow() == 0)
        {
            wxPanel* panel = new wxPanel(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(sizer);
            
            wxFlexGridSizer* innerSizer = new wxFlexGridSizer(2);
            sizer->Add(innerSizer, 1, wxEXPAND|wxALL, 5);
            
            //wxMoStaticText *switchControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Switch control:"));
            wxMoStaticText *switchControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Switch control:"), wxPoint(20, 10));
            innerSizer->Add(switchControlLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            //wxMoSwitchCtrl* switchCtrl = new wxMoSwitchCtrl( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
            wxMoSwitchCtrl* switchCtrl = new wxMoSwitchCtrl( panel, wxID_ANY, wxPoint(160, 10));
            innerSizer->Add(switchCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            //wxMoStaticText *textButtonsLabel = new wxMoStaticText(panel, wxID_STATIC, _("Text buttons:"));
            wxMoStaticText *textButtonsLabel = new wxMoStaticText(panel, wxID_STATIC, _("Text buttons:"), wxPoint(20, 55));
            innerSizer->Add(textButtonsLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
            //wxMoButton* button = new wxMoButton( panel, wxID_PROPERTIES, _("Action Sheet"), wxDefaultPosition, wxDefaultSize, 0 );
            wxMoButton* button = new wxMoButton( panel, wxID_PROPERTIES, _("Action Sheet"), wxPoint(160, 55));
            //wxMoButton* button2 = new wxMoButton( panel, wxID_VIEW_DETAILS, _("Alert Sheet"), wxDefaultPosition, wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
            wxMoButton* button2 = new wxMoButton( panel, wxID_VIEW_DETAILS, _("Alert Sheet"), wxPoint(200, 55), wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
            buttonSizer->Add(button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            buttonSizer->Add(button2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
            innerSizer->Add(buttonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
            
            wxBitmap bitmap;
            
            {
                wxMemoryInputStream is(wxwidgets_png, sizeof(wxwidgets_png));
                bitmap = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
            }
            
            //wxMoStaticText *bitmapButtonsLabel = new wxMoStaticText(panel, wxID_STATIC, _("Bitmap buttons:"));
            wxMoStaticText *bitmapButtonsLabel = new wxMoStaticText(panel, wxID_STATIC, _("Bitmap buttons:"), wxPoint(20, 100));
            innerSizer->Add(bitmapButtonsLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxBoxSizer* bmpButtonSizer = new wxBoxSizer(wxHORIZONTAL);
            //wxMoBitmapButton* bitmapButton = new wxMoBitmapButton( panel, wxID_ANY, bitmap, wxDefaultPosition, wxDefaultSize, 0 );
            wxMoBitmapButton* bitmapButton = new wxMoBitmapButton( panel, wxID_ANY, bitmap, wxPoint(160, 100));
            bitmapButton->SetBackgroundColour(wxColour(wxT("#C21212")));
            bmpButtonSizer->Add(bitmapButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            //wxMoBitmapButton* bitmapButton2 = new wxMoBitmapButton( panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_DISCLOSURE );
            wxMoBitmapButton* bitmapButton2 = new wxMoBitmapButton( panel, wxID_ANY, wxNullBitmap, wxPoint(200, 100), wxDefaultSize, wxBU_DISCLOSURE );
            bmpButtonSizer->Add(bitmapButton2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            innerSizer->Add(bmpButtonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
            
            wxBitmap staticBitmap;
            
            {
                wxMemoryInputStream is(toucan_png, sizeof(toucan_png));
                staticBitmap = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
            }

#if 0   // FIXME
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Static bitmap:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoStaticBitmap* staticBitmapItem = new wxMoStaticBitmap( panel, wxID_ANY, staticBitmap);
            innerSizer->Add(staticBitmapItem, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
#endif
            
            //wxMoStaticText *textControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Text control:"));
            wxMoStaticText *textControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Text control:"), wxPoint(20, 150));
            innerSizer->Add(textControlLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            //wxMoTextCtrl* textCtrl = new wxMoTextCtrl( panel, wxID_ANY, _("Text"), wxDefaultPosition, wxSize(120, -1), 0 );
            wxMoTextCtrl* textCtrl = new wxMoTextCtrl( panel, wxID_ANY, _("Text"), wxPoint(160, 150), wxSize(120, -1), 0 );
            innerSizer->Add(textCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            //wxMoStaticText *searchControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Search control:"));
            wxMoStaticText *searchControlLabel = new wxMoStaticText(panel, wxID_STATIC, _("Search control:"), wxPoint(20, 190));
            innerSizer->Add(searchControlLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            //wxMoSearchCtrl* searchCtrl = new wxMoSearchCtrl( panel, wxID_ANY, _("Search"), wxDefaultPosition, wxSize(120, -1), 0 );
            wxMoSearchCtrl* searchCtrl = new wxMoSearchCtrl( panel, wxID_ANY, _("Search"), wxPoint(160, 190), wxSize(120, -1), 0 );
            innerSizer->Add(searchCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

            PushWindow(panel, _("Basic Controls"));
        }
        else if (path.GetRow() == 1)
        {
            wxPanel* panel = new wxPanel(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(sizer);

            wxFlexGridSizer* innerSizer = new wxFlexGridSizer(2);
            innerSizer->AddGrowableCol(1);
            sizer->Add(innerSizer, 1, wxEXPAND|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Page:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoPageCtrl* pageCtrl = new wxMoPageCtrl( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
            pageCtrl->SetPageCount(6);
            pageCtrl->SetCurrentPage(2);
            innerSizer->Add(pageCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Gauge:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoGauge* gaugeCtrl = new wxMoGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 15), 0 );
            gaugeCtrl->SetValue(50);
            innerSizer->Add(gaugeCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Segmented:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoSegmentedCtrl* segCtrl = new wxMoSegmentedCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
            segCtrl->AddItem(_("Tiger"));
            segCtrl->AddItem(_("Cheetah"));
            segCtrl->AddItem(_("Pig"));
            segCtrl->SetSelection(0);
            innerSizer->Add(segCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Slider:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxBitmap smallDot;
            wxBitmap largeDot;
            
            {
                wxMemoryInputStream is(smalldot_png, sizeof(smalldot_png));
                smallDot = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
            }
            {
                wxMemoryInputStream is(largedot_png, sizeof(largedot_png));
                largeDot = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
            }
            
            wxMoSlider* sliderCtrl = new wxMoSlider(panel, wxID_ANY, 30, 0, 100, wxDefaultPosition, wxSize(200, 25), 0 );
            sliderCtrl->SetMinValueBitmap(smallDot);
            sliderCtrl->SetMaxValueBitmap(largeDot);
            
            innerSizer->Add(sliderCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
            
#if 0
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Web:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoWebCtrl* webCtrl = new wxMoWebCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(200, 200), 0 );
            webCtrl->LoadURL(wxT("http://www.google.com"));
            innerSizer->Add(webCtrl, 1, wxGROW|wxALL, 5);
#endif
            
            PushWindow(panel, _("More Controls"));
        }
        else if (path.GetRow() == 2)
        {
            wxPanel* panel = new wxPanel(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(sizer);

            wxFlexGridSizer* innerSizer = new wxFlexGridSizer(2);
            sizer->Add(innerSizer, 1, wxEXPAND|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Listbox control:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxArrayString listboxStrings;
            size_t i;
            for (i = 0; i < 20; i++)
            {
                listboxStrings.Add(wxString::Format(wxT("This is string %d"), (int) i));
            }
            wxMoListBox* listBox = new wxMoListBox( panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 60), listboxStrings, wxBORDER_SIMPLE );
            listBox->SetSelection(0);
            innerSizer->Add(listBox, 1, wxEXPAND|wxALL, 5);
            
            innerSizer->Add(new wxMoStaticText(panel, wxID_STATIC, _("Wheels control:")), 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
            
            wxMoWheelsTextDataSource* dataSource = new wxMoWheelsTextDataSource(2);
            wxArrayString textDataStrings;
            textDataStrings.Add(_("First string"));
            textDataStrings.Add(_("A second string"));
            textDataStrings.Add(_("And here's another string"));
            textDataStrings.Add(_("Fourth string at your service"));
            textDataStrings.Add(_("Five strings are not quite enough"));
            textDataStrings.Add(_("So here's a sixth string"));
            dataSource->SetStrings(textDataStrings, 0);
            
            wxArrayString textDataStrings2;
            for (i = 0; i < 50; i++)
            {
                textDataStrings2.Add(wxString::Format(wxT("%d"), i));
            }
            dataSource->SetStrings(textDataStrings2, 1);
            
            wxMoWheelsCtrl* wheelsCtrl = new wxMoWheelsCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 80),
                wxBORDER_SIMPLE);
            wheelsCtrl->SetDataSource(dataSource, true /* control owns the data source */);
            
            innerSizer->Add(wheelsCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

            PushWindow(panel, _("Picker Controls"));
        }
    }
    else if (path.GetSection() == 2)
    {
        // Bars
        if (path.GetRow() == 0)
        {
            // Text and image toolbar
            wxPanel* panel = new wxPanel(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(sizer);

            // Toolbar
            
            wxMoToolBar* toolbar = new wxMoToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_BLACK_OPAQUE_BG);
            sizer->Add(toolbar, 0, wxEXPAND, 0);
            
            toolbar->AddTool(100, _("First tool"), wxBitmap(app_xpm));
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_REFRESH, wxEmptyString, wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_CAMERA, wxEmptyString, wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_ADD, wxEmptyString, wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_COMPOSE, wxEmptyString, wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_TRASH, wxEmptyString, wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->Realize();
            
            DemoCanvas* canvas = new DemoCanvas(panel, wxID_ANY);
            sizer->Add(canvas, 1, wxEXPAND, 0);

            PushWindow(panel, _("Text & Image Toolbar"));
        }
        else if (path.GetRow() == 1)
        {
            // Plain text button toolbar
            wxPanel* panel = new wxPanel(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(sizer);

            // Toolbar
            
            wxMoToolBar* toolbar = new wxMoToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_TEXT|wxTB_NORMAL_BG);
            wxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
            font.SetPointSize(8);
            font.SetWeight(wxBOLD);
            toolbar->SetFont(font);
            sizer->Add(toolbar, 0, wxEXPAND, 0);
            
            toolbar->AddTool(wxID_EDIT, _("Edit"), wxNullBitmap);
            toolbar->AddSeparator();
            toolbar->AddTool(wxID_DONE, _("Done"), wxNullBitmap);
            toolbar->Realize();
            
            DemoCanvas* canvas = new DemoCanvas(panel, wxID_ANY);
            sizer->Add(canvas, 1, wxEXPAND, 0);

            PushWindow(panel, _("Text Button Toolbar"));
        }
    }       
}

// Utility to push a table onto the navigation controller
void TableDemoRootDataSource::PushTable(wxMoTableCtrl* tableCtrl, wxMoTableDataSource* dataSource, const wxString& title)
{
    wxMoViewController* controller = new wxMoViewController(title);
    tableCtrl->Show(false);
    controller->SetWindow(tableCtrl);

    tableCtrl->SetDataSource(dataSource, true /* own this data source */);    
    m_navCtrl->PushController(controller);
    tableCtrl->Show(true);

    tableCtrl->ReloadData();
}

// Utility to push a window onto the navigation controller
void TableDemoRootDataSource::PushWindow(wxWindow* win, const wxString& title)
{
    wxMoViewController* controller = new wxMoViewController(title);
    win->Show(false);
    controller->SetWindow(win);
    m_navCtrl->PushController(controller);
    win->Show(true);
}

EditableDataSource::EditableDataSource(wxMoTableCell::wxMoTableCellStyle cellStyle)
{
    m_cellStyle = cellStyle;

    size_t i;
    for (i = 0; i < 20; i++)
    {
        m_rowLabels.Add(wxString::Format(wxT("This is row %d"), (i+1)));
        m_rowData.Add(i);
    }
}

// Returns a table cell for the give location.
wxMoTableCell* EditableDataSource::GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    //int section = path.GetSection();
    int row = path.GetRow();

    wxString rowText(m_rowLabels[row]);

    int accessory = m_rowData[row] % 4;
    wxString reuseName;
    if (accessory == 3)
        reuseName = wxT("DemoCellWithSwitch");
    else
        reuseName = wxT("DemoCell");

    wxMoTableCell* cell = ctrl->GetReusableCell(reuseName);
        
    if (!cell)
        cell = new wxMoTableCell(ctrl, reuseName);
    cell->SetText(rowText);
    cell->SetDetailText(_("Detail text."));
    cell->SetCellStyle(m_cellStyle);
    cell->SetShowReorderingControl(true);

    if (ctrl->HasFlag(wxTC_GROUPED))
    {
        if (accessory == 0)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeDisclosureIndicator);
        else if (accessory == 1)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeDetailDisclosureButton);
        else if (accessory == 2)
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeCheckmark);
        else if (accessory == 3)
        {
            cell->SetAccessoryType(wxMoTableCell::AccessoryTypeNone);
        }
        cell->SetEditingAccessoryType(cell->GetAccessoryType());

        if (accessory == 3)
        {
            if (!cell->GetAccessoryWindow())
            {
                cell->SetAccessoryWindow(new wxMoSwitchCtrl(ctrl, wxID_ANY));
                cell->SetEditingAccessoryWindow(cell->GetAccessoryWindow());
            }
        }
        else
        {
            if (cell->GetAccessoryWindow())
            {
                cell->SetAccessoryWindow(NULL);
                cell->SetEditingAccessoryWindow(NULL);
            }
        }
    }

    {
        wxMemoryInputStream is(wxwidgets_png, sizeof(wxwidgets_png));
        wxBitmap bitmap = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);

        cell->SetBitmap(bitmap);
    }

    return cell;
}

// Returns the number of sections in the table.
int EditableDataSource::GetSectionCount(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    return 1;
}

// Returns the number of rows in a section.
int EditableDataSource::GetRowCount(wxMoTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const
{
    return m_rowLabels.GetCount();
}

// Returns the number of rows in a section.
wxArrayString EditableDataSource::GetSectionTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString arr;
    arr.Add(wxT("Section 1"));
    return arr;
}

// Returns an array of strings, each one corresponding to an index title to be shown
// on the right hand side of a plain list.
wxArrayString EditableDataSource::GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const
{
    return wxArrayString();
}


// Called by the table control to commit an insertion requested by the user. This function should
// then call InsertRows in response.
bool EditableDataSource::CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    m_rowLabels.Insert(wxString::Format(wxT("New row at %d"), (int)(path.GetRow()+1)), path.GetRow());
    m_rowData.Insert(0, path.GetRow());

    return wxMoTableDataSource::CommitInsertRow(ctrl, path);
}

// Called by the table control to commit a deletion requested by the user. This function should
// then call DeleteRows in response.
bool EditableDataSource::CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    m_rowLabels.RemoveAt(path.GetRow());
    m_rowData.RemoveAt(path.GetRow());

    return wxMoTableDataSource::CommitDeleteRow(ctrl, path);
}

// Requests a move operation from 'from' to 'to'. The data should then reflect
// the new ordering. Return false if the reordering was not done.
bool EditableDataSource::MoveRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& pathFrom, const wxTablePath& pathTo)
{
    wxString label = m_rowLabels[pathFrom.GetRow()];
    int data = m_rowData[pathFrom.GetRow()];
    m_rowLabels.RemoveAt(pathFrom.GetRow());
    m_rowData.RemoveAt(pathFrom.GetRow());
    if (pathTo.GetRow() >= (int) m_rowLabels.GetCount())
    {
        m_rowLabels.Add(label);
        m_rowData.Add(data);
    }
    else
    {
        m_rowLabels.Insert(label, pathTo.GetRow());
        m_rowData.Insert(data, pathTo.GetRow());
    }

    return true;
}

// Get the edit style for a row
wxMoTableCtrl::CellEditStyle EditableDataSource::GetCellEditStyle(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& path)
{
    if ((path.GetRow() % 2) == 0)
        return wxMoTableCtrl::EditStyleShowInsertButton;
    else
        return wxMoTableCtrl::EditStyleShowDeleteButton;
}
