/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.cpp
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by: Santiago Palacios
// Copyright:   (c) Michael Bedward, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/aboutdlg.h"

#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"

#include "griddemo.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// Custom renderer that renders column header cells without borders and in
// italic
class CustomColumnHeaderRenderer : public wxGridColumnHeaderRenderer
{
public:
    CustomColumnHeaderRenderer(const wxColour& colFg, const wxColour& colBg)
        : m_colFg(colFg),
          m_colBg(colBg)
    {
    }
    CustomColumnHeaderRenderer(const CustomColumnHeaderRenderer&) = delete;
    CustomColumnHeaderRenderer& operator=(const CustomColumnHeaderRenderer&) = delete;

    virtual void DrawLabel(const wxGrid& WXUNUSED(grid),
                           wxDC& dc,
                           const wxString& value,
                           const wxRect& rect,
                           int horizAlign,
                           int vertAlign,
                           int WXUNUSED(textOrientation)) const override
    {
        dc.SetTextForeground(m_colFg);
        dc.SetFont(wxITALIC_FONT->Bold());
        dc.DrawLabel(value, rect, horizAlign | vertAlign);
    }

    virtual void DrawBorder(const wxGrid& WXUNUSED(grid),
                            wxDC& dc,
                            wxRect& rect) const override
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_colBg));
        dc.DrawRectangle(rect);
    }

private:
    const wxColour m_colFg, m_colBg;
};

// And a custom attributes provider which uses custom column header renderer
// defined above
class CustomColumnHeadersProvider : public wxGridCellAttrProvider
{
public:
    // by default custom column renderer is not used, call
    // UseCustomColHeaders() to enable it
    CustomColumnHeadersProvider()
        : m_customOddRenderer(*wxYELLOW, *wxBLUE),
          m_customEvenRenderer(*wxWHITE, *wxBLACK),
          m_useCustom(false)
    {
    }
    CustomColumnHeadersProvider(const CustomColumnHeadersProvider&) = delete;
    CustomColumnHeadersProvider& operator=(const CustomColumnHeadersProvider&) = delete;

    // enable or disable the use of custom renderer for column headers
    void UseCustomColHeaders(bool use = true) { m_useCustom = use; }

protected:
    virtual const wxGridColumnHeaderRenderer& GetColumnHeaderRenderer(int col) override
    {
        // if enabled, use custom renderers
        if ( m_useCustom )
        {
            // and use different ones for odd and even columns -- just to show
            // that we can
            return col % 2 ? m_customOddRenderer : m_customEvenRenderer;
        }

        return wxGridCellAttrProvider::GetColumnHeaderRenderer(col);
    }

private:
    CustomColumnHeaderRenderer m_customOddRenderer,
                               m_customEvenRenderer;

    bool m_useCustom;
};

// ----------------------------------------------------------------------------
// Custom wxGrid renderer and editor for showing stars as used for rating
// ----------------------------------------------------------------------------

// Max number of stars shown by MyGridStarRenderer.
static const int MAX_STARS = 5;

// Helper function returning the number between 0 and MAX_STARS corresponding
// to the value of the cell.
static int GetStarValue(wxGrid& grid, int row, int col)
{
    unsigned long n = 0;
    if ( !grid.GetCellValue(row, col).ToULong(&n) || n > MAX_STARS )
        n = 0;

    return static_cast<int>(n);
}

// Another helper returning the string containing the appropriate number of
// black and white stars.
static wxString GetStarString(int numBlackStars)
{
    const wxUniChar BLACK_STAR = 0x2605;
    const wxUniChar WHITE_STAR = 0x2606;

    return wxString(BLACK_STAR, numBlackStars) +
            wxString(WHITE_STAR, MAX_STARS - numBlackStars);
}

// Renders the value of the cell, which is supposed to be a number between 1
// and 5, as a sequence of that number of black stars followed by the number of
// white stars needed to have 5 stars in total.
class MyGridStarRenderer : public wxGridCellRenderer
{
public:
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override
    {
        wxGridCellRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

        SetTextColoursAndFont(grid, attr, dc, isSelected);

        grid.DrawTextRectangle(dc, GetStarString(GetStarValue(grid, row, col)),
                               rect, attr);
    }

    virtual wxSize GetBestSize(wxGrid& WXUNUSED(grid),
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int WXUNUSED(row),
                               int WXUNUSED(col)) override
    {
        dc.SetFont(attr.GetFont());
        return dc.GetTextExtent(GetStarString(MAX_STARS));
    }

    virtual wxGridCellRenderer *Clone() const override
    {
        return new MyGridStarRenderer();
    }
};

// Activatable editor cycling the number of stars on each activation.
class MyGridStarEditor : public wxGridCellActivatableEditor
{
public:
    virtual wxGridActivationResult
    TryActivate(int row, int col, wxGrid* grid,
                const wxGridActivationSource& actSource) override
    {
        int numStars = -1;

        switch ( actSource.GetOrigin() )
        {
            case wxGridActivationSource::Program:
                // It isn't really possible to programmatically start editing a
                // cell using this editor.
                return wxGridActivationResult::DoNothing();

            case wxGridActivationSource::Key:
                switch ( actSource.GetKeyEvent().GetKeyCode() )
                {
                    case '0': numStars = 0; break;
                    case '1': numStars = 1; break;
                    case '2': numStars = 2; break;
                    case '3': numStars = 3; break;
                    case '4': numStars = 4; break;
                    case '5': numStars = 5; break;
                    case ' ':
                        // Use space key to cycle over the values.
                        break;

                    default:
                        return wxGridActivationResult::DoNothing();
                }

                break;

            case wxGridActivationSource::Mouse:
                // Ideally we should use the mouse event position to determine
                // on which star the user clicked, but for now keep it simple
                // and just cycle through the star value.
                break;
        }

        if ( numStars == -1 )
            numStars = (GetStarValue(*grid, row, col) + 1) % (MAX_STARS + 1);

        m_value.Printf("%d", numStars);

        return wxGridActivationResult::DoChange(m_value);
    }

    virtual void DoActivate(int row, int col, wxGrid* grid) override
    {
        grid->SetCellValue(row, col, m_value);
    }

    virtual wxGridCellEditor *Clone() const override
    {
        return new MyGridStarEditor();
    }

private:
    wxString m_value;
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(GridApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GridApp
// ----------------------------------------------------------------------------

bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// GridFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( GridFrame, wxFrame )
    EVT_MENU( ID_TOGGLEROWLABELS,  GridFrame::ToggleRowLabels )
    EVT_MENU( ID_TOGGLECOLLABELS,  GridFrame::ToggleColLabels )
    EVT_MENU( ID_TOGGLEEDIT, GridFrame::ToggleEditing )
    EVT_MENU( ID_TOGGLEROWSIZING, GridFrame::ToggleRowSizing )
    EVT_MENU( ID_TOGGLECOLSIZING, GridFrame::ToggleColSizing )
    EVT_MENU( ID_TOGGLEROWMOVING, GridFrame::ToggleRowMoving)
    EVT_MENU( ID_TOGGLECOLMOVING, GridFrame::ToggleColMoving )
    EVT_MENU( ID_TOGGLECOLHIDING, GridFrame::ToggleColHiding )
    EVT_MENU( ID_TOGGLEGRIDSIZING, GridFrame::ToggleGridSizing )
    EVT_MENU( ID_TOGGLEGRIDDRAGCELL, GridFrame::ToggleGridDragCell )
    EVT_MENU( ID_COLNATIVEHEADER, GridFrame::SetNativeColHeader )
    EVT_MENU( ID_COLNATIVELABELS, GridFrame::SetNativeColLabels )
    EVT_MENU( ID_COLDEFAULTHEADER, GridFrame::SetDefaultColHeader )
    EVT_MENU( ID_COLCUSTOMHEADER, GridFrame::SetCustomColHeader )
    EVT_MENU_RANGE( ID_TAB_STOP, ID_TAB_LEAVE, GridFrame::SetTabBehaviour )
    EVT_MENU( ID_TAB_CUSTOM, GridFrame::SetTabCustomHandler )
    EVT_MENU( ID_TOGGLEGRIDLINES, GridFrame::ToggleGridLines )
    EVT_MENU( ID_AUTOSIZECOLS, GridFrame::AutoSizeCols )
    EVT_MENU( ID_CELLOVERFLOW, GridFrame::CellOverflow )
    EVT_MENU( ID_RESIZECELL, GridFrame::ResizeCell )
    EVT_MENU( ID_TOGGLE_CHECKERED_CELLS, GridFrame::ToggleCheckeredCells )
    EVT_MENU( ID_TOGGLE_COLOURED_CELLS, GridFrame::ToggleColouredCells )
    EVT_MENU( ID_SETLABELCOLOUR, GridFrame::SetLabelColour )
    EVT_MENU( ID_SETLABELTEXTCOLOUR, GridFrame::SetLabelTextColour )
    EVT_MENU( ID_SETLABEL_FONT, GridFrame::SetLabelFont )
    EVT_MENU( ID_ROWLABELHORIZALIGN, GridFrame::SetRowLabelHorizAlignment )
    EVT_MENU( ID_ROWLABELVERTALIGN, GridFrame::SetRowLabelVertAlignment )
    EVT_MENU( ID_COLLABELHORIZALIGN, GridFrame::SetColLabelHorizAlignment )
    EVT_MENU( ID_COLLABELVERTALIGN, GridFrame::SetColLabelVertAlignment )
    EVT_MENU( ID_CORNERLABELHORIZALIGN, GridFrame::SetCornerLabelHorizAlignment )
    EVT_MENU( ID_CORNERLABELVERTALIGN, GridFrame::SetCornerLabelVertAlignment )
    EVT_MENU( ID_CORNERLABELORIENTATION, GridFrame::ToggleCornerLabelOrientation )
    EVT_MENU( ID_GRIDLINECOLOUR, GridFrame::SetGridLineColour )
    EVT_MENU( ID_INSERTROW, GridFrame::InsertRow )
    EVT_MENU( ID_INSERTCOL, GridFrame::InsertCol )
    EVT_MENU( ID_DELETEROW, GridFrame::DeleteSelectedRows )
    EVT_MENU( ID_DELETECOL, GridFrame::DeleteSelectedCols )
    EVT_MENU( ID_CLEARGRID, GridFrame::ClearGrid )
    EVT_MENU( ID_EDITCELL, GridFrame::EditCell )
    EVT_MENU( ID_SETCORNERLABEL, GridFrame::SetCornerLabelValue )
    EVT_MENU( ID_SHOWSEL,   GridFrame::ShowSelection )
    EVT_MENU( ID_SELCELLS,  GridFrame::SelectCells )
    EVT_MENU( ID_SELROWS,  GridFrame::SelectRows )
    EVT_MENU( ID_SELCOLS,  GridFrame::SelectCols )
    EVT_MENU( ID_SELROWSORCOLS,  GridFrame::SelectRowsOrCols )
    EVT_MENU( ID_SELNONE,  GridFrame::SelectNone )

    EVT_MENU( ID_FREEZE_OR_THAW,  GridFrame::FreezeOrThaw )

    EVT_MENU( ID_SET_CELL_FG_COLOUR, GridFrame::SetCellFgColour )
    EVT_MENU( ID_SET_CELL_BG_COLOUR, GridFrame::SetCellBgColour )

    EVT_MENU( wxID_ABOUT, GridFrame::OnAbout )
    EVT_MENU( wxID_CLEAR, GridFrame::OnClear )
    EVT_MENU( wxID_EXIT, GridFrame::OnQuit )
    EVT_MENU( ID_VTABLE, GridFrame::OnVTable)
    EVT_MENU( ID_BUGS_TABLE, GridFrame::OnBugsTable)
    EVT_MENU( ID_TABULAR_TABLE, GridFrame::OnTabularTable)

    EVT_MENU( ID_DESELECT_CELL, GridFrame::DeselectCell)
    EVT_MENU( ID_DESELECT_COL, GridFrame::DeselectCol)
    EVT_MENU( ID_DESELECT_ROW, GridFrame::DeselectRow)
    EVT_MENU( ID_DESELECT_ALL, GridFrame::DeselectAll)
    EVT_MENU( ID_SELECT_CELL, GridFrame::SelectCell)
    EVT_MENU( ID_SELECT_COL, GridFrame::SelectCol)
    EVT_MENU( ID_SELECT_ROW, GridFrame::SelectRow)
    EVT_MENU( ID_SELECT_ALL, GridFrame::SelectAll)
    EVT_MENU( ID_SELECT_UNSELECT, GridFrame::OnAddToSelectToggle)

    EVT_MENU( ID_SIZE_ROW, GridFrame::AutoSizeRow )
    EVT_MENU( ID_SIZE_COL, GridFrame::AutoSizeCol )
    EVT_MENU( ID_SIZE_ROW_LABEL, GridFrame::AutoSizeRowLabel )
    EVT_MENU( ID_SIZE_COL_LABEL, GridFrame::AutoSizeColLabel )
    EVT_MENU( ID_SIZE_LABELS_COL, GridFrame::AutoSizeLabelsCol )
    EVT_MENU( ID_SIZE_LABELS_ROW, GridFrame::AutoSizeLabelsRow )
    EVT_MENU( ID_SIZE_GRID, GridFrame::AutoSizeTable )

    EVT_MENU( ID_HIDECOL, GridFrame::HideCol )
    EVT_MENU( ID_SHOWCOL, GridFrame::ShowCol )
    EVT_MENU( ID_HIDEROW, GridFrame::HideRow )
    EVT_MENU( ID_SHOWROW, GridFrame::ShowRow )

    EVT_MENU( ID_SET_HIGHLIGHT_WIDTH, GridFrame::OnSetHighlightWidth)
    EVT_MENU( ID_SET_RO_HIGHLIGHT_WIDTH, GridFrame::OnSetROHighlightWidth)

    EVT_MENU( wxID_PRINT, GridFrame::OnGridRender )
    EVT_MENU( ID_RENDER_COORDS, GridFrame::OnGridRender )

    EVT_GRID_LABEL_LEFT_CLICK( GridFrame::OnLabelLeftClick )
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_ROW_SIZE( GridFrame::OnRowSize )
    EVT_GRID_COL_SIZE( GridFrame::OnColSize )
    EVT_GRID_COL_AUTO_SIZE( GridFrame::OnColAutoSize )
    EVT_GRID_ROW_MOVE( GridFrame::OnRowMove )
    EVT_GRID_COL_MOVE( GridFrame::OnColMove )

    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECTING( GridFrame::OnRangeSelecting )
    EVT_GRID_RANGE_SELECTED( GridFrame::OnRangeSelected )
    EVT_GRID_CELL_CHANGING( GridFrame::OnCellValueChanging )
    EVT_GRID_CELL_CHANGED( GridFrame::OnCellValueChanged )
    EVT_GRID_CELL_BEGIN_DRAG( GridFrame::OnCellBeginDrag )

    EVT_GRID_EDITOR_SHOWN( GridFrame::OnEditorShown )
    EVT_GRID_EDITOR_HIDDEN( GridFrame::OnEditorHidden )
wxEND_EVENT_TABLE()


GridFrame::GridFrame()
        : wxFrame( nullptr, wxID_ANY, "wxWidgets grid class demo",
                   wxDefaultPosition,
                   wxDefaultSize )
{
    SetIcon(wxICON(sample));

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append( ID_VTABLE, "&Virtual table test\tCtrl-V");
    fileMenu->Append( ID_BUGS_TABLE, "&Bugs table test\tCtrl-B");
    fileMenu->Append( ID_TABULAR_TABLE, "&Tabular table test\tCtrl-T");
    fileMenu->AppendSeparator();

    wxMenu* setupMenu = new wxMenu;
    wxMenuItem* item;
    item = setupMenu->AppendCheckItem( ID_RENDER_ROW_LABEL,
                                       "Render row labels" );
    item->Check();
    item = setupMenu->AppendCheckItem( ID_RENDER_COL_LABEL,
                                       "Render column labels" );
    item->Check();
    item = setupMenu->AppendCheckItem( ID_RENDER_GRID_LINES,
                                       "Render grid cell lines" );
    item->Check();
    item = setupMenu->AppendCheckItem( ID_RENDER_GRID_BORDER,
                                       "Render border" );
    item->Check();
    item = setupMenu->AppendCheckItem( ID_RENDER_SELECT_HLIGHT,
                                       "Render selection highlight" );
    setupMenu->AppendSeparator();
    setupMenu->AppendCheckItem( ID_RENDER_LOMETRIC,
                                       "Use LOMETRIC mapping mode" );
    setupMenu->AppendCheckItem( ID_RENDER_DEFAULT_SIZE,
                                       "Use wxDefaultSize" );
    setupMenu->AppendCheckItem( ID_RENDER_MARGIN,
                                "Logical 50 unit margin" );
    setupMenu->AppendCheckItem( ID_RENDER_ZOOM,
                                "Zoom 125%" );

    fileMenu->AppendSubMenu( setupMenu, "Render setup" );
    fileMenu->Append( wxID_PRINT, "Render" );
    fileMenu->Append( ID_RENDER_COORDS, "Render G5:P30" );

#if wxUSE_LOG
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_CLEAR, "Clear &log\tCtrl-L" );
#endif // wxUSE_LOG

    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT, "E&xit\tAlt-X" );

    wxMenu *viewMenu = new wxMenu;
    viewMenu->AppendCheckItem(ID_TOGGLEROWLABELS, "&Row labels");
    viewMenu->AppendCheckItem(ID_TOGGLECOLLABELS, "&Col labels");
    viewMenu->AppendCheckItem(ID_TOGGLEEDIT,"&Editable");
    viewMenu->AppendCheckItem(ID_TOGGLEROWSIZING, "Ro&w drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLECOLSIZING, "C&ol drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLEROWMOVING, "Row drag-move");
    viewMenu->AppendCheckItem(ID_TOGGLECOLMOVING, "Col drag-&move");
    viewMenu->AppendCheckItem(ID_TOGGLECOLHIDING, "Col hiding popup menu");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDSIZING, "&Grid drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDDRAGCELL, "&Grid drag-cell");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDLINES, "&Grid Lines");
    viewMenu->AppendCheckItem(ID_SET_HIGHLIGHT_WIDTH, "&Set Cell Highlight Width...");
    viewMenu->AppendCheckItem(ID_SET_RO_HIGHLIGHT_WIDTH, "&Set Cell RO Highlight Width...");
    viewMenu->AppendCheckItem(ID_AUTOSIZECOLS, "&Auto-size cols");
    viewMenu->AppendCheckItem(ID_CELLOVERFLOW, "&Overflow cells");
    viewMenu->AppendCheckItem(ID_RESIZECELL, "&Resize cell (7,1)");
    viewMenu->AppendCheckItem(ID_TOGGLE_CHECKERED_CELLS,
                              "Toggle Chec&kered Cells\tCtrl+Shift+K");
    viewMenu->AppendCheckItem(ID_TOGGLE_COLOURED_CELLS,
                              "Toggle Co&loured Cells\tCtrl+Shift+L");
    viewMenu->Append(ID_HIDECOL, "&Hide column A");
    viewMenu->Append(ID_SHOWCOL, "&Show column A");
    viewMenu->Append(ID_HIDEROW, "&Hide row 2");
    viewMenu->Append(ID_SHOWROW, "&Show row 2");
    wxMenu *rowLabelMenu = new wxMenu;

    viewMenu->Append( ID_ROWLABELALIGN, "R&ow label alignment",
                      rowLabelMenu,
                      "Change alignment of row labels" );

    rowLabelMenu->AppendRadioItem( ID_ROWLABELHORIZALIGN, "&Horizontal" );
    rowLabelMenu->AppendRadioItem( ID_ROWLABELVERTALIGN, "&Vertical" );

    wxMenu *colLabelMenu = new wxMenu;

    viewMenu->Append( ID_COLLABELALIGN, "Col l&abel alignment",
                      colLabelMenu,
                      "Change alignment of col labels" );

    colLabelMenu->AppendRadioItem( ID_COLLABELHORIZALIGN, "&Horizontal" );
    colLabelMenu->AppendRadioItem( ID_COLLABELVERTALIGN, "&Vertical" );

    wxMenu *cornerLabelMenu = new wxMenu;
    viewMenu->Append( ID_CORNERLABELALIGN, "Corner label alignment",
                      cornerLabelMenu,
                      "Change alignment of corner label" );

    cornerLabelMenu->AppendRadioItem( ID_CORNERLABELHORIZALIGN, "&Horizontal" );
    cornerLabelMenu->AppendRadioItem( ID_CORNERLABELVERTALIGN, "&Vertical" );

    viewMenu->Append( ID_CORNERLABELORIENTATION, "Toggle corner label orientation" );

    wxMenu *colHeaderMenu = new wxMenu;

    viewMenu->Append( ID_ROWLABELALIGN, "Col header style",
                      colHeaderMenu,
                      "Change style of col header" );

    colHeaderMenu->AppendRadioItem( ID_COLDEFAULTHEADER, "&Default" );
    colHeaderMenu->AppendRadioItem( ID_COLNATIVEHEADER, "&Native" );
    colHeaderMenu->AppendRadioItem( ID_COLNATIVELABELS, "Native-&like" );
    colHeaderMenu->AppendRadioItem( ID_COLCUSTOMHEADER, "&Custom" );

    wxMenu *tabBehaviourMenu = new wxMenu;
    tabBehaviourMenu->AppendRadioItem(ID_TAB_STOP, "&Stop at the boundary");
    tabBehaviourMenu->AppendRadioItem(ID_TAB_WRAP, "&Wrap at the boundary");
    tabBehaviourMenu->AppendRadioItem(ID_TAB_LEAVE, "&Leave the grid");
    tabBehaviourMenu->AppendRadioItem(ID_TAB_CUSTOM, "&Custom tab handler");
    viewMenu->AppendSubMenu(tabBehaviourMenu, "&Tab behaviour");

    wxMenu *colMenu = new wxMenu;
    colMenu->Append( ID_SETLABELCOLOUR, "Set &label colour..." );
    colMenu->Append( ID_SETLABELTEXTCOLOUR, "Set label &text colour..." );
    colMenu->Append( ID_SETLABEL_FONT, "Set label fo&nt..." );
    colMenu->Append( ID_GRIDLINECOLOUR, "&Grid line colour..." );
    colMenu->Append( ID_SET_CELL_FG_COLOUR, "Set cell &foreground colour..." );
    colMenu->Append( ID_SET_CELL_BG_COLOUR, "Set cell &background colour..." );

    wxMenu *editMenu = new wxMenu;
    editMenu->Append( ID_INSERTROW, "Insert &rows\tCtrl+I" );
    editMenu->Append( ID_INSERTCOL, "Insert &columns\tCtrl+Shift+I" );
    editMenu->Append( ID_DELETEROW, "Delete selected ro&ws\tCtrl+D" );
    editMenu->Append( ID_DELETECOL, "Delete selected co&ls\tCtrl+Shift+D" );
    editMenu->Append( ID_CLEARGRID, "Cl&ear grid cell contents" );
    editMenu->Append( ID_EDITCELL, "&Edit current cell" );
    editMenu->Append( ID_SETCORNERLABEL, "&Set corner label..." );

    editMenu->AppendCheckItem( ID_FREEZE_OR_THAW, "Freeze up to cursor\tCtrl-F" );

    wxMenu *selectMenu = new wxMenu;
    selectMenu->Append( ID_SELECT_UNSELECT, "Add new cells to the selection",
                        "When off, old selection is deselected before "
                        "selecting the new cells", wxITEM_CHECK );
    selectMenu->AppendSeparator();
    selectMenu->Append( ID_SELECT_ALL, "Select all");
    selectMenu->Append( ID_SELECT_ROW, "Select row 2");
    selectMenu->Append( ID_SELECT_COL, "Select col 2");
    selectMenu->Append( ID_SELECT_CELL, "Select cell (3, 1)");
    selectMenu->AppendSeparator();
    selectMenu->Append( ID_DESELECT_ALL, "Deselect all");
    selectMenu->Append( ID_DESELECT_ROW, "Deselect row 2");
    selectMenu->Append( ID_DESELECT_COL, "Deselect col 2");
    selectMenu->Append( ID_DESELECT_CELL, "Deselect cell (3, 1)");
    selectMenu->AppendSeparator();
    wxMenu *selectionMenu = new wxMenu;
    selectMenu->Append( ID_SHOWSEL, "&Show current selection\tCtrl-S" );
    selectMenu->Append( ID_CHANGESEL, "Change &selection mode",
                      selectionMenu,
                      "Change selection mode" );

    selectionMenu->Append( ID_SELCELLS, "Select &cells" );
    selectionMenu->Append( ID_SELROWS, "Select &rows" );
    selectionMenu->Append( ID_SELCOLS, "Select col&umns" );
    selectionMenu->Append( ID_SELROWSORCOLS, "Select rows &or columns" );
    selectionMenu->Append( ID_SELNONE, "&Disallow selection" );

    wxMenu *autosizeMenu = new wxMenu;
    autosizeMenu->Append( ID_SIZE_ROW, "Selected &row data" );
    autosizeMenu->Append( ID_SIZE_COL, "Selected &column data" );
    autosizeMenu->Append( ID_SIZE_ROW_LABEL, "Selected row la&bel" );
    autosizeMenu->Append( ID_SIZE_COL_LABEL, "Selected column &label" );
    autosizeMenu->Append( ID_SIZE_LABELS_COL, "Column la&bels" );
    autosizeMenu->Append( ID_SIZE_LABELS_ROW, "Row label&s" );
    autosizeMenu->Append( ID_SIZE_GRID, "Entire &grid" );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append( wxID_ABOUT, "&About wxGrid demo" );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( fileMenu, "&File" );
    menuBar->Append( viewMenu, "&Grid" );
    menuBar->Append( colMenu,  "&Colours" );
    menuBar->Append( editMenu, "&Edit" );
    menuBar->Append( selectMenu, "&Select" );
    menuBar->Append( autosizeMenu, "&Autosize" );
    menuBar->Append( helpMenu, "&Help" );

    SetMenuBar( menuBar );

    m_addToSel = false;

    grid = new wxGrid( this,
                       wxID_ANY,
                       wxPoint( 0, 0 ),
                       FromDIP(wxSize( 800, 450 )) );


#if wxUSE_LOG
    logWin = new wxTextCtrl( this,
                             wxID_ANY,
                             wxEmptyString,
                             wxDefaultPosition,
                             wxSize(-1, 8*GetCharHeight()),
                             wxTE_MULTILINE );

    logger = new wxLogTextCtrl( logWin );
    m_logOld = wxLog::SetActiveTarget( logger );
    wxLog::DisableTimestamp();
#endif // wxUSE_LOG

    // this will create a grid and, by default, an associated grid
    // table for strings
    grid->CreateGrid( 0, 0 );

    grid->GetTable()->SetAttrProvider(new CustomColumnHeadersProvider());

    grid->AppendRows(1000);
    grid->AppendCols(100);

    int ir = grid->GetNumberRows();
    grid->DeleteRows(0, ir);
    grid->AppendRows(ir);

    grid->SetRowSize( 0, 4*grid->GetDefaultRowSize() );
    grid->SetCellValue( 0, 0, "Ctrl+Home\nwill go to\nthis cell" );

    grid->SetCellValue( 0, 1, "A long piece of text to demonstrate wrapping." );
    grid->SetCellRenderer(0 , 1, new wxGridCellAutoWrapStringRenderer);
    grid->SetCellEditor( 0,  1 , new wxGridCellAutoWrapStringEditor);

    grid->SetCellValue( 0, 2, "Blah" );
    grid->SetCellValue( 0, 3, "Read only" );
    grid->SetReadOnly( 0, 3 );

    grid->SetCellValue( 0, 4, "Can veto edit this cell" );

    grid->SetColSize(10, FromDIP(150));
    wxString longtext = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\n";
    longtext += "With tabs :\n";
    longtext += "Home,\t\thome\t\t\tagain\n";
    longtext += "Long word at start :\n";
    longtext += "ILikeToBeHereWhen I can\n";
    longtext += "Long word in the middle :\n";
    longtext += "When IComeHome,ColdAnd tired\n";
    longtext += "Long last word :\n";
    longtext += "It's GoodToWarmMyBonesBesideTheFire";
    grid->SetCellValue( 0, 10, longtext );
    grid->SetCellRenderer(0 , 10, new wxGridCellAutoWrapStringRenderer);
    grid->SetCellEditor( 0,  10 , new wxGridCellAutoWrapStringEditor);
    grid->SetCellValue( 0, 11, "K1 cell editor blocker" );

    grid->SetCellValue( 0, 5, "Press\nCtrl+arrow\nto skip over\ncells" );

    const int endRow = grid->GetNumberRows() - 1,
              endCol = grid->GetNumberCols() - 1;

    grid->SetRowSize(endRow, 4 * grid->GetDefaultRowSize());
    grid->SetCellValue(endRow - 1, endCol - 1, "Test background colour setting");
    grid->SetCellBackgroundColour(endRow - 1, endCol, wxColour(255, 127, 127));
    grid->SetCellBackgroundColour(endRow, endCol - 1, wxColour(255, 127, 127));
    grid->SetCellValue(endRow, endCol, "Ctrl+End\nwill go to\nthis cell");
    grid->SetCellValue( 1, 0, "This default cell will overflow into neighboring cells, but not if you turn overflow off.");

    grid->SetCellValue(2, 0, "This one always overflows");
    grid->SetCellFitMode(2, 0, wxGridFitMode::Overflow());

    grid->SetCellTextColour(1, 2, *wxRED);
    grid->SetCellBackgroundColour(1, 2, *wxGREEN);

    grid->SetCellValue( 1, 4, "I'm in the middle");

    grid->SetCellValue(2, 2, "red");

    grid->SetCellTextColour(2, 2, *wxRED);
    grid->SetCellValue(3, 3, "green on grey");
    grid->SetCellTextColour(3, 3, *wxGREEN);
    grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);

    grid->SetCellValue(4, 4, "a weird looking cell");
    grid->SetCellAlignment(4, 4, wxALIGN_CENTRE, wxALIGN_CENTRE);
    grid->SetCellRenderer(4, 4, new MyGridCellRenderer);

    grid->SetCellValue(4, 5, "3");
    grid->SetCellRenderer(4, 5, new MyGridStarRenderer);
    grid->SetCellEditor(4, 5, new MyGridStarEditor);

    grid->SetCellRenderer(3, 0, new wxGridCellBoolRenderer);
    grid->SetCellEditor(3, 0, new wxGridCellBoolEditor);
    grid->SetCellBackgroundColour(3, 0, wxColour(255, 127, 127));

    grid->SetCellRenderer(3, 1, new wxGridCellBoolRenderer);
    grid->SetCellEditor(3, 1, new wxGridCellBoolEditor);
    grid->SetCellValue(3, 1, "1");

    wxGridCellAttr *attr;
    attr = new wxGridCellAttr;
    attr->SetTextColour(*wxBLUE);
    grid->SetColAttr(5, attr);
    attr = new wxGridCellAttr;
    attr->SetBackgroundColour(*wxRED);
    grid->SetRowAttr(5, attr);

    grid->SetCellValue(2, 4, "a wider column");
    grid->SetColSize(4, 3*grid->GetDefaultColSize()/2);
    grid->SetColMinimalWidth(4, grid->GetColSize(4));

    grid->SetCellTextColour(5, 8, *wxGREEN);
    grid->SetCellValue(5, 8, "Bg from row attr\nText col from cell attr");
    grid->SetCellValue(5, 5, "Bg from row attr Text col from col attr and this text is so long that it covers over many many empty cells but is broken by one that isn't");

    // Some numeric columns with different formatting.
    grid->SetColFormatFloat(6);
    grid->SetReadOnly(0, 6);
    grid->SetCellValue(0, 6, "Default\nfloat format");
    grid->SetCellValue(1, 6, wxString::Format("%g", 3.1415));
    grid->SetCellValue(2, 6, wxString::Format("%g", 1415.0));
    grid->SetCellValue(3, 6, wxString::Format("%g", 12345.67890));

    grid->SetColFormatFloat(7, 6, 2);
    grid->SetReadOnly(0, 7);
    grid->SetCellValue(0, 7, "Width 6\nprecision 2");
    grid->SetCellValue(1, 7, wxString::Format("%g", 3.1415));
    grid->SetCellValue(2, 7, wxString::Format("%g", 1415.0));
    grid->SetCellValue(3, 7, wxString::Format("%g", 12345.67890));

    grid->SetColFormatCustom(8,
            wxString::Format("%s:%i,%i,%s", wxGRID_VALUE_FLOAT, -1, 4, "g"));
    grid->SetReadOnly(0, 8);
    grid->SetCellValue(0, 8, "Compact\nformat");
    grid->SetCellValue(1, 8, "31415e-4");
    grid->SetCellValue(2, 8, "1415");
    grid->SetCellValue(3, 8, "123456789e-4");

    grid->SetColFormatNumber(9);
    grid->SetReadOnly(0, 9);
    grid->SetCellValue(0, 9, "Integer\ncolumn");
    grid->SetCellValue(1, 9, "17");
    grid->SetCellValue(2, 9, "0");
    grid->SetCellEditor(2, 9, new wxGridCellNumberEditor(0, 100));
    grid->SetCellValue(2, 10, "<- This cell uses [0, 100] range");
    grid->SetCellValue(3, 9, "-666");
    grid->SetCellAlignment(3, 9, wxALIGN_CENTRE, wxALIGN_TOP);
    grid->SetCellValue(3, 10, "<- This numeric cell should be centred");

    grid->SetReadOnly(0, 13);
    grid->SetCellValue(0, 13, "Localized date\ncolumn");
    grid->SetColFormatDate(13); // Localized by default.
    grid->SetCellValue(1, 13, "Today");
    grid->SetReadOnly(0, 14);
    grid->SetCellValue(0, 14, "ISO 8601 date\ncolumn");
    grid->SetColFormatDate(14, "%Y-%m-%d"); // ISO 8601 date format.
    grid->SetCellValue(1, 14, "Tomorrow");

    grid->SetCellValue(13, 0, "Date cell:");
    grid->SetCellValue(13, 1, "Today");
    grid->SetCellRenderer(13, 1, new wxGridCellDateRenderer);
    grid->SetCellEditor(13, 1, new wxGridCellDateEditor);
    grid->SetCellValue(14, 0, "ISO date cell:");
    grid->SetCellValue(14, 1, "Tomorrow");
    grid->SetCellRenderer(14, 1, new wxGridCellDateRenderer("%Y-%m-%d"));
    grid->SetCellEditor(14, 1, new wxGridCellDateEditor);

    grid->SetCellValue(13, 3, "String using default ellipsization");
    grid->SetCellFitMode(13, 3, wxGridFitMode::Ellipsize());

    grid->SetCellValue(13, 4, "String ellipsized in the middle");
    grid->SetCellFitMode(13, 4, wxGridFitMode::Ellipsize(wxELLIPSIZE_MIDDLE));

    const wxString choices[] =
    {
        "Please select a choice",
        "This takes two cells",
        "Another choice",
    };
    grid->SetCellEditor(4, 2, new wxGridCellChoiceEditor(WXSIZEOF(choices), choices));
    grid->SetCellSize(4, 2, 1, 2);
    grid->SetCellValue(4, 2, choices[0]);
    grid->SetCellOverflow(4, 2, false);

    grid->SetCellSize(7, 1, 3, 4);
    grid->SetCellAlignment(7, 1, wxALIGN_CENTRE, wxALIGN_CENTRE);
    grid->SetCellValue(7, 1, "Big box!");

    // create a separator-like row: it's grey and it's non-resizable
    grid->DisableRowResize(10);
    grid->SetRowSize(10, 3*grid->GetDefaultRowSize()/2);
    attr = new wxGridCellAttr;
    attr->SetBackgroundColour(*wxLIGHT_GREY);
    attr->SetAlignment(wxALIGN_INVALID, wxALIGN_CENTRE);
    grid->SetRowAttr(10, attr);
    grid->SetCellValue(10, 0, "You can't resize this row interactively -- try it");

    // this does exactly nothing except testing that SetAttr() handles null
    // attributes and does reference counting correctly
    grid->SetAttr(11, 11, nullptr);
    grid->SetAttr(11, 11, new wxGridCellAttr);
    grid->SetAttr(11, 11, nullptr);

    grid->Bind(wxEVT_CONTEXT_MENU, &GridFrame::OnGridContextMenu, this, grid->GetId());

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add(grid, wxSizerFlags(2).Expand());

#if wxUSE_LOG
    topSizer->Add(logWin, wxSizerFlags(1).Expand());
#endif // wxUSE_LOG

    SetSizerAndFit( topSizer );

    Centre();
    SetDefaults();
}


GridFrame::~GridFrame()
{
#if wxUSE_LOG
    delete wxLog::SetActiveTarget(m_logOld);
#endif // wxUSE_LOG
}


void GridFrame::SetDefaults()
{
    GetMenuBar()->Check( ID_TOGGLEROWLABELS, true );
    GetMenuBar()->Check( ID_TOGGLECOLLABELS, true );
    GetMenuBar()->Check( ID_TOGGLEEDIT, true );
    GetMenuBar()->Check( ID_TOGGLEROWSIZING, true );
    GetMenuBar()->Check( ID_TOGGLECOLSIZING, true );
    GetMenuBar()->Check( ID_TOGGLECOLMOVING, false );
    GetMenuBar()->Check( ID_TOGGLECOLHIDING, true );
    GetMenuBar()->Check( ID_TOGGLEGRIDSIZING, true );
    GetMenuBar()->Check( ID_TOGGLEGRIDDRAGCELL, false );
    GetMenuBar()->Check( ID_TOGGLEGRIDLINES, true );
    GetMenuBar()->Check( ID_CELLOVERFLOW, true );
}


void GridFrame::ToggleRowLabels( wxCommandEvent& WXUNUSED(ev) )
{
    if ( GetMenuBar()->IsChecked( ID_TOGGLEROWLABELS ) )
    {
        grid->SetRowLabelSize( grid->GetDefaultRowLabelSize() );
    }
    else
    {
        grid->SetRowLabelSize( 0 );
    }
}


void GridFrame::ToggleColLabels( wxCommandEvent& WXUNUSED(ev) )
{
    if ( GetMenuBar()->IsChecked( ID_TOGGLECOLLABELS ) )
    {
        grid->SetColLabelSize( grid->GetDefaultColLabelSize() );
    }
    else
    {
        grid->SetColLabelSize( 0 );
    }
}


void GridFrame::ToggleEditing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableEditing(
        GetMenuBar()->IsChecked( ID_TOGGLEEDIT ) );
}


void GridFrame::ToggleRowSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragRowSize(
        GetMenuBar()->IsChecked( ID_TOGGLEROWSIZING ) );
}


void GridFrame::ToggleColSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragColSize(
        GetMenuBar()->IsChecked( ID_TOGGLECOLSIZING ) );
}

void GridFrame::ToggleRowMoving( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragRowMove(
        GetMenuBar()->IsChecked( ID_TOGGLEROWMOVING ) );
}

void GridFrame::ToggleColMoving( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragColMove(
        GetMenuBar()->IsChecked( ID_TOGGLECOLMOVING ) );
}

void GridFrame::ToggleColHiding( wxCommandEvent& WXUNUSED(ev) )
{
    if ( !grid->EnableHidingColumns(
        GetMenuBar()->IsChecked( ID_TOGGLECOLHIDING ) ) )
    {
        GetMenuBar()->Check( ID_TOGGLECOLHIDING, grid->CanHideColumns() );
    }
}

void GridFrame::ToggleGridSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragGridSize(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDSIZING ) );
}

void GridFrame::ToggleGridDragCell( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragCell(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDDRAGCELL ) );
}

void GridFrame::SetNativeColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(false);
    grid->UseNativeColHeader(true);
}

void GridFrame::SetNativeColLabels( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(false);
    grid->UseNativeColHeader(false);
    grid->SetUseNativeColLabels(true);
}

void GridFrame::SetCustomColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(true);
    grid->UseNativeColHeader(false);
    grid->SetUseNativeColLabels(false);
}

void GridFrame::SetDefaultColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(false);
    grid->UseNativeColHeader(false);
    grid->SetUseNativeColLabels(false);
}


void GridFrame::OnGridCustomTab(wxGridEvent& event)
{
    // just for testing, make the cursor move up and down instead of the usual
    // left and right
    if ( event.ShiftDown() )
    {
        if ( grid->GetGridCursorRow() > 0 )
            grid->MoveCursorUp( false );
    }
    else
    {
        if ( grid->GetGridCursorRow() < grid->GetNumberRows() - 1 )
            grid->MoveCursorDown( false );
    }
}

void GridFrame::SetTabBehaviour(wxCommandEvent& event)
{
    // To make any built-in behaviour work, we need to disable the custom TAB
    // handler, otherwise it would be overriding them.
    grid->Unbind(wxEVT_GRID_TABBING, &GridFrame::OnGridCustomTab, this);

    grid->SetTabBehaviour(
            static_cast<wxGrid::TabBehaviour>(event.GetId() - ID_TAB_STOP)
        );
}

void GridFrame::SetTabCustomHandler(wxCommandEvent&)
{
    grid->Bind(wxEVT_GRID_TABBING, &GridFrame::OnGridCustomTab, this);
}

void GridFrame::OnGridContextMenu(wxContextMenuEvent& event)
{
    // This is not supposed to happen: even if the grid consists of different
    // subwindows internally, all context menu events should be seen as coming
    // from the grid itself.
    if ( event.GetEventObject() != grid )
    {
        wxLogError("Context menu unexpectedly sent from non-grid window.");
    }
    else
    {
        wxLogMessage("wxEVT_CONTEXT_MENU in the grid at at (%d, %d)",
                     event.GetPosition().x, event.GetPosition().y);
    }

    event.Skip();
}

void GridFrame::ToggleGridLines( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableGridLines(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDLINES ) );
}

void GridFrame::OnSetHighlightWidth( wxCommandEvent& WXUNUSED(ev) )
{
    wxString choices[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

    wxSingleChoiceDialog dlg(this, "Choose the thickness of the highlight pen:",
                             "Pen Width", 11, choices);

    int current = grid->GetCellHighlightPenWidth();
    dlg.SetSelection(current);
    if (dlg.ShowModal() == wxID_OK) {
        grid->SetCellHighlightPenWidth(dlg.GetSelection());
    }
}

void GridFrame::OnSetROHighlightWidth( wxCommandEvent& WXUNUSED(ev) )
{
    wxString choices[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

    wxSingleChoiceDialog dlg(this, "Choose the thickness of the highlight pen:",
                             "Pen Width", 11, choices);

    int current = grid->GetCellHighlightROPenWidth();
    dlg.SetSelection(current);
    if (dlg.ShowModal() == wxID_OK) {
        grid->SetCellHighlightROPenWidth(dlg.GetSelection());
    }
}



void GridFrame::AutoSizeCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->AutoSizeColumns();
    grid->Refresh();
}

void GridFrame::CellOverflow( wxCommandEvent& ev )
{
    grid->SetDefaultCellOverflow(ev.IsChecked());
    grid->Refresh();
}

void GridFrame::ResizeCell( wxCommandEvent& ev )
{
    if (ev.IsChecked())
        grid->SetCellSize( 7, 1, 5, 5 );
    else
        grid->SetCellSize( 7, 1, 1, 5 );
    grid->Refresh();
}

void GridFrame::SetLabelColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( nullptr );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetLabelBackgroundColour( colour );
    }
}


void GridFrame::SetLabelTextColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( nullptr );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetLabelTextColour( colour );
    }
}

void GridFrame::SetLabelFont( wxCommandEvent& WXUNUSED(ev) )
{
    wxFont font = wxGetFontFromUser(this);
    if ( font.IsOk() )
    {
        grid->SetLabelFont(font);
    }
}

void GridFrame::SetRowLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxALIGN_LEFT:
            horiz = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            horiz = wxALIGN_RIGHT;
            break;

        case wxALIGN_RIGHT:
            horiz = wxALIGN_LEFT;
            break;
    }

    grid->SetRowLabelAlignment( horiz, vert );
}

void GridFrame::SetRowLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxALIGN_TOP:
            vert = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            vert = wxALIGN_BOTTOM;
            break;

        case wxALIGN_BOTTOM:
            vert = wxALIGN_TOP;
            break;
    }

    grid->SetRowLabelAlignment( horiz, vert );
}


void GridFrame::SetColLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxALIGN_LEFT:
            horiz = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            horiz = wxALIGN_RIGHT;
            break;

        case wxALIGN_RIGHT:
            horiz = wxALIGN_LEFT;
            break;
    }

    grid->SetColLabelAlignment( horiz, vert );
}


void GridFrame::SetColLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxALIGN_TOP:
            vert = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            vert = wxALIGN_BOTTOM;
            break;

        case wxALIGN_BOTTOM:
            vert = wxALIGN_TOP;
            break;
    }

    grid->SetColLabelAlignment( horiz, vert );
}


void GridFrame::SetCornerLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetCornerLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxALIGN_LEFT:
            horiz = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            horiz = wxALIGN_RIGHT;
            break;

        case wxALIGN_RIGHT:
            horiz = wxALIGN_LEFT;
            break;
    }

    grid->SetCornerLabelAlignment( horiz, vert );
}


void GridFrame::SetCornerLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetCornerLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxALIGN_TOP:
            vert = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            vert = wxALIGN_BOTTOM;
            break;

        case wxALIGN_BOTTOM:
            vert = wxALIGN_TOP;
            break;
    }

    grid->SetCornerLabelAlignment( horiz, vert );
}


void GridFrame::ToggleCornerLabelOrientation( wxCommandEvent& WXUNUSED(ev) )
{
    int orientation = grid->GetCornerLabelTextOrientation();

    switch(orientation)
    {
        case wxHORIZONTAL:
            orientation = wxVERTICAL;
            break;

        case wxVERTICAL:
            orientation = wxHORIZONTAL;
            break;
    }

    grid->SetCornerLabelTextOrientation(orientation);
}


void GridFrame::SetGridLineColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( nullptr );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetGridLineColour( colour );
    }
}

namespace
{

// Helper used to insert/delete rows/columns. Allows changing by more than
// one row/column at once.
void HandleEdits(wxGrid* grid, wxGridDirection direction, bool isInsertion)
{
    const bool isRow = (direction == wxGRID_ROW);

    const wxGridBlocks selected = grid->GetSelectedBlocks();
    wxGridBlocks::iterator it = selected.begin();

    if ( isInsertion )
    {
        int pos, count;
        // Only do multiple insertions if we have a single consecutive
        // selection (mimicking LibreOffice), otherwise do a single insertion
        // at cursor.
        if ( it != selected.end() && ++it == selected.end() )
        {
            const wxGridBlockCoords& b = *selected.begin();
            pos = isRow ? b.GetTopRow() : b.GetLeftCol();
            count = (isRow ? b.GetBottomRow() : b.GetRightCol()) - pos + 1;
        }
        else
        {
            pos = isRow ? grid->GetGridCursorRow() : grid->GetGridCursorCol();
            count = 1;
        }

        if ( isRow )
            grid->InsertRows(pos, count);
        else
            grid->InsertCols(pos, count);

        return;
    }

    wxGridUpdateLocker locker(grid);

    wxVector<int> deletions;

    for (; it != selected.end(); ++it )
    {
        const wxGridBlockCoords& b = *it;
        const int begin = isRow ? b.GetTopRow() : b.GetLeftCol();
        const int end = isRow ? b.GetBottomRow() : b.GetRightCol();

        for ( int n = begin; n <= end; ++n )
        {
            if ( !wxVectorContains(deletions, n) )
                deletions.push_back(n);
        }
    }

    wxVectorSort(deletions);

    const size_t deletionCount = deletions.size();

    if ( !deletionCount )
        return;

    int seqEnd = 0, seqCount = 0;
    for ( size_t i = deletionCount; i > 0; --i )
    {
        const int n = deletions[i - 1];

        if ( n != seqEnd - seqCount )
        {
            if (i != deletionCount)
            {
                const int seqStart = seqEnd - seqCount + 1;
                if ( isRow )
                    grid->DeleteRows(seqStart, seqCount);
                else
                    grid->DeleteCols(seqStart, seqCount);
            }

            seqEnd = n;
            seqCount = 0;
        }

        seqCount++;
    }

    const int seqStart = seqEnd - seqCount + 1;
    if ( isRow )
        grid->DeleteRows(seqStart, seqCount);
    else
        grid->DeleteCols(seqStart, seqCount);
}

} // anoymous namespace

void GridFrame::InsertRow( wxCommandEvent& WXUNUSED(ev) )
{
    HandleEdits(grid, wxGRID_ROW, /* isInsertion = */ true);
}


void GridFrame::InsertCol( wxCommandEvent& WXUNUSED(ev) )
{
    HandleEdits(grid, wxGRID_COLUMN, /* isInsertion = */ true);
}


void GridFrame::DeleteSelectedRows( wxCommandEvent& WXUNUSED(ev) )
{
    HandleEdits(grid, wxGRID_ROW, /* isInsertion = */ false);
}


void GridFrame::AutoSizeRow(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedRows();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeRow( sels[n], false );
    }
}

void GridFrame::AutoSizeCol(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedCols();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeColumn( sels[n], false );
    }
}

void GridFrame::AutoSizeRowLabel(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedRows();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeRowLabelSize( sels[n] );
    }
}

void GridFrame::AutoSizeColLabel(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedCols();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeColLabelSize( sels[n] );
    }
}

void GridFrame::AutoSizeLabelsCol(wxCommandEvent& WXUNUSED(event))
{
    grid->SetColLabelSize( wxGRID_AUTOSIZE );
}

void GridFrame::AutoSizeLabelsRow(wxCommandEvent& WXUNUSED(event))
{
    grid->SetRowLabelSize( wxGRID_AUTOSIZE );
}

void GridFrame::AutoSizeTable(wxCommandEvent& WXUNUSED(event))
{
    grid->AutoSize();
    Layout();
}


void GridFrame::DeleteSelectedCols( wxCommandEvent& WXUNUSED(ev) )
{
    HandleEdits(grid, wxGRID_COLUMN, /* isInsertion = */ false);
}


void GridFrame::ClearGrid( wxCommandEvent& WXUNUSED(ev) )
{
    grid->ClearGrid();
}

void GridFrame::EditCell( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableCellEditControl();
}

void GridFrame::SetCornerLabelValue( wxCommandEvent& WXUNUSED(ev) )
{
    wxTextEntryDialog dialog(this,
                             "Please enter corner label:",
                             "Please enter a string",
                             grid->GetCornerLabelValue(),
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        grid->SetCornerLabelValue(dialog.GetValue());
    }
}

void GridFrame::ShowSelection( wxCommandEvent& WXUNUSED(ev) )
{
    int count = 0;
    wxString desc;
    const wxGridBlocks& sel = grid->GetSelectedBlocks();
    for ( wxGridBlocks::iterator it = sel.begin(); it != sel.end(); ++it )
    {
        const wxGridBlockCoords& b = *it;

        wxString blockDesc;
        if ( b.GetLeftCol() == 0 &&
                b.GetRightCol() == grid->GetNumberCols() - 1 )
        {
            if ( b.GetTopRow() == b.GetBottomRow() )
                blockDesc.Printf("row %d", b.GetTopRow() + 1);
            else
                blockDesc.Printf("rows %d..%d",
                                 b.GetTopRow() + 1, b.GetBottomRow() + 1);
        }
        else if ( b.GetTopRow() == 0 &&
                        b.GetBottomRow() == grid->GetNumberRows() - 1 )
        {
            if ( b.GetLeftCol() == b.GetRightCol() )
                blockDesc.Printf("column %d", b.GetLeftCol() + 1);
            else
                blockDesc.Printf("columns %d..%d",
                                 b.GetLeftCol() + 1,
                                 b.GetRightCol() + 1);
        }
        else if ( b.GetTopRow() == b.GetBottomRow() &&
                    b.GetLeftCol() == b.GetRightCol() )
        {
            blockDesc.Printf("cell R%dC%d",
                             b.GetTopRow() + 1, b.GetLeftCol() + 1);
        }
        else
        {
            blockDesc.Printf("block R%dC%d - R%dC%d",
                             b.GetTopRow() + 1,
                             b.GetLeftCol() + 1,
                             b.GetBottomRow() + 1,
                             b.GetRightCol() + 1);
        }

        if ( count++ )
            desc += "\n\t";
        desc += blockDesc;
    }

    switch ( count )
    {
        case 0:
            wxLogMessage("No selection");
            break;

        case 1:
            wxLogMessage("Selection: %s", desc);
            break;

        default:
            wxLogMessage("%d selected blocks:\n\t%s", count, desc);
    }
}

void GridFrame::SelectCells( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectCells );
}

void GridFrame::SelectRows( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectRows );
}

void GridFrame::SelectCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectColumns );
}

void GridFrame::SelectRowsOrCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectRowsOrColumns );
}

void GridFrame::SelectNone( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectNone );
}

void GridFrame::FreezeOrThaw(wxCommandEvent& ev)
{
    if ( ev.IsChecked() )
    {
        if ( !grid->FreezeTo(grid->GetGridCursorCoords()) )
        {
            wxLogMessage("Failed to freeze the grid.");
            GetMenuBar()->Check(ID_FREEZE_OR_THAW, false);
            return;
        }

        wxLogMessage("Grid is now frozen");
    }
    else
    {
        // This never fails.
        grid->FreezeTo(0, 0);

        wxLogMessage("Grid is now thawed");
    }

    GetMenuBar()->Enable( ID_TOGGLECOLMOVING, !grid->IsFrozen() );
    GetMenuBar()->Enable( ID_TOGGLEROWMOVING, !grid->IsFrozen() );
}

void GridFrame::SetCellFgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.IsOk() )
    {
        grid->SetDefaultCellTextColour(col);
        grid->Refresh();
    }
}

void GridFrame::SetCellBgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.IsOk() )
    {
        // Check the new Refresh function by passing it a rectangle
        // which exactly fits the grid.
        wxPoint pt(0, 0);
        wxRect r(pt, grid->GetSize());
        grid->SetDefaultCellBackgroundColour(col);
        grid->Refresh(true, &r);
    }
}

void GridFrame::DeselectCell(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectCell(3, 1);
}

void GridFrame::DeselectCol(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectCol(2);
}

void GridFrame::DeselectRow(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectRow(2);
}

void GridFrame::DeselectAll(wxCommandEvent& WXUNUSED(event))
{
      grid->ClearSelection();
}

void GridFrame::SelectCell(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectBlock(3, 1, 3, 1, m_addToSel);
}

void GridFrame::SelectCol(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectCol(2, m_addToSel);
}

void GridFrame::SelectRow(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectRow(2, m_addToSel);
}

void GridFrame::SelectAll(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectAll();
}

void GridFrame::OnAddToSelectToggle(wxCommandEvent& event)
{
    m_addToSel = event.IsChecked();
}

void GridFrame::OnLabelLeftClick( wxGridEvent& ev )
{
    wxString logBuf;
    if ( ev.GetRow() != -1 )
    {
        logBuf << "Left click on row label " << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << "Left click on col label " << ev.GetCol();
    }
    else
    {
        logBuf << "Left click on corner label";
    }

    if ( ev.ShiftDown() )
        logBuf << " (shift down)";
    if ( ev.ControlDown() )
        logBuf << " (control down)";
    wxLogMessage( "%s", logBuf );

    // you must call event skip if you want default grid processing
    //
    ev.Skip();
}


void GridFrame::OnCellLeftClick( wxGridEvent& ev )
{
    wxLogMessage("Left click at row %d, col %d", ev.GetRow(), ev.GetCol());

    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}


void GridFrame::OnRowSize( wxGridSizeEvent& ev )
{
    const int row = ev.GetRowOrCol();

    wxLogMessage("Resized row %d, new height = %d",
                 row, grid->GetRowSize(row));

    ev.Skip();
}


void GridFrame::OnColSize( wxGridSizeEvent& ev )
{
    const int col = ev.GetRowOrCol();

    wxLogMessage("Resized column %d, new width = %d",
                 col, grid->GetColSize(col));

    ev.Skip();
}

void GridFrame::OnColAutoSize( wxGridSizeEvent &event )
{
    // Fit even-numbered columns to their contents while using the default
    // behaviour for the odd-numbered ones to be able to see the difference.
    int col = event.GetRowOrCol();
    if ( col % 2 )
    {
        wxLogMessage("Auto-sizing column %d to fit its contents", col);
        grid->AutoSizeColumn(col);
    }
    else
    {
        event.Skip();
    }
}

void GridFrame::OnRowMove(wxGridEvent& event)
{
    wxLogMessage("Moving row %d to %d", event.GetRow(), event.GetNewRow());
    event.Skip();
}

void GridFrame::OnColMove(wxGridEvent& event)
{
    wxLogMessage("Moving column %d to %d", event.GetCol(), event.GetNewCol());
    event.Skip();
}

void GridFrame::OnSelectCell( wxGridEvent& ev )
{
    wxString logBuf;
    if ( ev.Selecting() )
        logBuf << "Selected ";
    else
        logBuf << "Deselected ";
    logBuf << "cell at row " << ev.GetRow()
           << " col " << ev.GetCol()
           << " ( ControlDown: "<< (ev.ControlDown() ? 'T':'F')
           << ", ShiftDown: "<< (ev.ShiftDown() ? 'T':'F')
           << ", AltDown: "<< (ev.AltDown() ? 'T':'F')
           << ", MetaDown: "<< (ev.MetaDown() ? 'T':'F') << " )";

    //Indicate whether this row was moved
    if ( grid->GetRowPos( ev.GetRow() ) != ev.GetRow() )
        logBuf << " *** Row moved, current position: " << grid->GetRowPos( ev.GetRow() );
    //Indicate whether this column was moved
    if ( grid->GetColPos( ev.GetCol() ) != ev.GetCol() )
        logBuf << " *** Column moved, current position: " << grid->GetColPos( ev.GetCol() );

    wxLogMessage( "%s", logBuf );

    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
}

namespace
{

void
LogRangeSelectEvent(wxGridRangeSelectEvent& ev, const char* suffix)
{
    wxString logBuf;
    if ( ev.Selecting() )
        logBuf << "Select";
    else
        logBuf << "Deselect";
    logBuf << suffix
           << " cells from row " << ev.GetTopRow()
           << " col " << ev.GetLeftCol()
           << " to row " << ev.GetBottomRow()
           << " col " << ev.GetRightCol()
           << " ( ControlDown: "<< (ev.ControlDown() ? 'T':'F')
           << ", ShiftDown: "<< (ev.ShiftDown() ? 'T':'F')
           << ", AltDown: "<< (ev.AltDown() ? 'T':'F')
           << ", MetaDown: "<< (ev.MetaDown() ? 'T':'F') << " )";
    wxLogMessage( "%s", logBuf );

    ev.Skip();
}

} // anonymous namespace

void GridFrame::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
    LogRangeSelectEvent(ev, "ed");
}

void GridFrame::OnRangeSelecting( wxGridRangeSelectEvent& ev )
{
    LogRangeSelectEvent(ev, "ing");
}

void GridFrame::OnCellValueChanging( wxGridEvent& ev )
{
    int row = ev.GetRow(),
        col = ev.GetCol();

    wxLogMessage("Value of cell at (%d, %d): about to change "
                 "from \"%s\" to \"%s\"",
                 row, col,
                 grid->GetCellValue(row, col), ev.GetString());

    // test how vetoing works
    if ( ev.GetString() == "42" )
    {
        wxLogMessage("Vetoing the change.");
        ev.Veto();
        return;
    }

    ev.Skip();
}

void GridFrame::OnCellValueChanged( wxGridEvent& ev )
{
    int row = ev.GetRow(),
        col = ev.GetCol();

    wxLogMessage("Value of cell at (%d, %d) changed and is now \"%s\" "
                 "(was \"%s\")",
                 row, col,
                 grid->GetCellValue(row, col), ev.GetString());

    ev.Skip();
}

void GridFrame::OnCellBeginDrag( wxGridEvent& ev )
{
    wxLogMessage("Got request to drag cell at row %d, col %d",
                 ev.GetRow(), ev.GetCol());

    ev.Skip();
}

void GridFrame::OnEditorShown( wxGridEvent& ev )
{

    if ( (ev.GetCol() == 4) &&
         (ev.GetRow() == 0) &&
     (wxMessageBox("Are you sure you wish to edit this cell",
                   "Checking",wxYES_NO) == wxNO ) ) {

     ev.Veto();
     return;
    }

    wxLogMessage( "Cell editor shown." );

    ev.Skip();
}

void GridFrame::OnEditorHidden( wxGridEvent& ev )
{

    if ( (ev.GetCol() == 4) &&
         (ev.GetRow() == 0) &&
     (wxMessageBox("Are you sure you wish to finish editing this cell",
                   "Checking",wxYES_NO) == wxNO ) ) {

        ev.Veto();
        return;
    }

    wxLogMessage( "Cell editor hidden." );

    ev.Skip();
}

void GridFrame::OnAbout(  wxCommandEvent& WXUNUSED(ev) )
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("wxGrid demo");
    aboutInfo.SetDescription(_("wxGrid sample program"));
    aboutInfo.AddDeveloper("Michael Bedward");
    aboutInfo.AddDeveloper("Julian Smart");
    aboutInfo.AddDeveloper("Vadim Zeitlin");

    // this is just to force the generic version of the about
    // dialog under wxMSW so that it's easy to test if the grid
    // repaints correctly when it has lost focus and a dialog
    // (different from the Windows standard message box -- it doesn't
    // work with it for some reason) is moved over it.
    aboutInfo.SetWebSite("http://www.wxwidgets.org");

    wxAboutBox(aboutInfo, this);
}


void GridFrame::OnClear( wxCommandEvent& WXUNUSED(ev) )
{
#if wxUSE_LOG
    logWin->Clear();
#endif // wxUSE_LOG
}

void GridFrame::OnQuit( wxCommandEvent& WXUNUSED(ev) )
{
    Close( true );
}

void GridFrame::OnBugsTable(wxCommandEvent& )
{
    BugsGridFrame *frame = new BugsGridFrame;
    frame->Show(true);
}

// ----------------------------------------------------------------------------
// MyGridCellAttrProvider
// ----------------------------------------------------------------------------

MyGridCellAttrProvider::MyGridCellAttrProvider()
    : m_attrForOddRows(new wxGridCellAttr)
{
    m_attrForOddRows->SetBackgroundColour(*wxLIGHT_GREY);
}

wxGridCellAttr *MyGridCellAttrProvider::GetAttr(int row, int col,
                           wxGridCellAttr::wxAttrKind  kind /* = wxGridCellAttr::Any */) const
{
    wxGridCellAttrPtr attr(wxGridCellAttrProvider::GetAttr(row, col, kind));

    if ( row % 2 )
    {
        if ( !attr )
        {
            attr = m_attrForOddRows;
        }
        else
        {
            if ( !attr->HasBackgroundColour() )
            {
                attr = attr->Clone();
                attr->SetBackgroundColour(*wxLIGHT_GREY);
            }
        }
    }

    return attr.release();
}

void GridFrame::OnVTable(wxCommandEvent& )
{
    static long s_sizeGrid = 10000;

    s_sizeGrid = wxGetNumberFromUser("Size of the table to create",
                                     "Size: ",
                                     "wxGridDemo question",
                                     s_sizeGrid,
                                     0, 10000000, this);

    if ( s_sizeGrid != -1 )
    {
        BigGridFrame* win = new BigGridFrame(s_sizeGrid);
        win->Show(true);
    }
}

// ----------------------------------------------------------------------------
// MyGridCellRenderer
// ----------------------------------------------------------------------------

// do something that the default renderer doesn't here just to show that it is
// possible to alter the appearance of the cell beyond what the attributes
// allow
void MyGridCellRenderer::Draw(wxGrid& grid,
                              wxGridCellAttr& attr,
                              wxDC& dc,
                              const wxRect& rect,
                              int row, int col,
                              bool isSelected)
{
    wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

    dc.SetPen(*wxGREEN_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawEllipse(rect);
}

// ============================================================================
// BigGridFrame and BigGridTable:  Sample of a non-standard table
// ============================================================================

BigGridFrame::BigGridFrame(long sizeGrid)
            : wxFrame(nullptr, wxID_ANY, "Plugin Virtual Table")
{
    m_grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_table = new BigGridTable(sizeGrid);

    // VZ: I don't understand why this slows down the display that much,
    //     must profile it...
    //m_table->SetAttrProvider(new MyGridCellAttrProvider);

    m_grid->AssignTable(m_table);

    SetClientSize(FromDIP(wxSize(500, 450)));
}

// ============================================================================
// BugsGridFrame: a "realistic" table
// ============================================================================

// ----------------------------------------------------------------------------
// bugs table data
// ----------------------------------------------------------------------------

enum Columns
{
    Col_Id,
    Col_Summary,
    Col_Severity,
    Col_Priority,
    Col_Platform,
    Col_Opened,
    Col_Max
};

enum Severity
{
    Sev_Wish,
    Sev_Minor,
    Sev_Normal,
    Sev_Major,
    Sev_Critical,
    Sev_Max
};

static const wxString severities[] =
{
    "wishlist",
    "minor",
    "normal",
    "major",
    "critical",
};

static struct BugsGridData
{
    int id;
    wxString summary;
    Severity severity;
    int prio;
    wxString platform;
    bool opened;
} gs_dataBugsGrid [] =
{
    { 18, "foo doesn't work", Sev_Major, 1, "wxMSW", true },
    { 27, "bar crashes", Sev_Critical, 1, "all", false },
    { 45, "printing is slow", Sev_Minor, 3, "wxMSW", true },
    { 68, "Rectangle() fails", Sev_Normal, 1, "wxMSW", false },
};

// ----------------------------------------------------------------------------
// BugsGridTable
// ----------------------------------------------------------------------------

wxString BugsGridTable::GetTypeName(int WXUNUSED(row), int col)
{
    switch ( col )
    {
        case Col_Id:
        case Col_Priority:
            return wxGRID_VALUE_NUMBER;

        case Col_Severity:
            wxFALLTHROUGH;

        case Col_Summary:
            return wxString::Format("%s:80", wxGRID_VALUE_STRING);

        case Col_Platform:
            return wxString::Format("%s:all,MSW,GTK,other", wxGRID_VALUE_CHOICE);

        case Col_Opened:
            return wxGRID_VALUE_BOOL;
    }

    wxFAIL_MSG("unknown column");

    return wxEmptyString;
}

int BugsGridTable::GetNumberRows()
{
    return WXSIZEOF(gs_dataBugsGrid);
}

int BugsGridTable::GetNumberCols()
{
    return Col_Max;
}

bool BugsGridTable::IsEmptyCell( int WXUNUSED(row), int WXUNUSED(col) )
{
    return false;
}

wxString BugsGridTable::GetValue( int row, int col )
{
    const BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
            return wxString::Format("%d", gd.id);

        case Col_Priority:
            return wxString::Format("%d", gd.prio);

        case Col_Opened:
            return gd.opened ? "1" : "0";

        case Col_Severity:
            return severities[gd.severity];

        case Col_Summary:
            return gd.summary;

        case Col_Platform:
            return gd.platform;
    }

    return wxEmptyString;
}

void BugsGridTable::SetValue( int row, int col, const wxString& value )
{
    BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
        case Col_Priority:
        case Col_Opened:
            wxFAIL_MSG("unexpected column");
            break;

        case Col_Severity:
            {
                size_t n;
                for ( n = 0; n < WXSIZEOF(severities); n++ )
                {
                    if ( severities[n] == value )
                    {
                        gd.severity = (Severity)n;
                        break;
                    }
                }

                if ( n == WXSIZEOF(severities) )
                {
                    wxLogWarning("Invalid severity value '%s'.",
                                 value);
                    gd.severity = Sev_Normal;
                }
            }
            break;

        case Col_Summary:
            gd.summary = value;
            break;

        case Col_Platform:
            gd.platform = value;
            break;
    }
}

bool
BugsGridTable::CanGetValueAs(int WXUNUSED(row),
                             int col,
                             const wxString& typeName)
{
    if ( typeName == wxGRID_VALUE_STRING )
    {
        return true;
    }
    else if ( typeName == wxGRID_VALUE_BOOL )
    {
        return col == Col_Opened;
    }
    else if ( typeName == wxGRID_VALUE_NUMBER )
    {
        return col == Col_Id || col == Col_Priority || col == Col_Severity;
    }
    else
    {
        return false;
    }
}

bool BugsGridTable::CanSetValueAs( int row, int col, const wxString& typeName )
{
    return CanGetValueAs(row, col, typeName);
}

long BugsGridTable::GetValueAsLong( int row, int col )
{
    const BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
            return gd.id;

        case Col_Priority:
            return gd.prio;

        case Col_Severity:
            return gd.severity;

        default:
            wxFAIL_MSG("unexpected column");
            return -1;
    }
}

bool BugsGridTable::GetValueAsBool( int row, int col )
{
    if ( col == Col_Opened )
    {
        return gs_dataBugsGrid[row].opened;
    }
    else
    {
        wxFAIL_MSG("unexpected column");

        return false;
    }
}

void BugsGridTable::SetValueAsLong( int row, int col, long value )
{
    BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Priority:
            gd.prio = value;
            break;

        default:
            wxFAIL_MSG("unexpected column");
    }
}

void BugsGridTable::SetValueAsBool( int row, int col, bool value )
{
    if ( col == Col_Opened )
    {
        gs_dataBugsGrid[row].opened = value;
    }
    else
    {
        wxFAIL_MSG("unexpected column");
    }
}

wxString BugsGridTable::GetColLabelValue( int col )
{
    static const wxString headers[] = { "Id",
        "Summary",
        "Severity",
        "Priority",
        "Platform",
        "Opened?" };
    return headers[col];
}

// ----------------------------------------------------------------------------
// BugsGridFrame
// ----------------------------------------------------------------------------

BugsGridFrame::BugsGridFrame()
             : wxFrame(nullptr, wxID_ANY, "Bugs table")
{
    wxGrid *grid = new wxGrid(this, wxID_ANY);
    wxGridTableBase *table = new BugsGridTable();
    table->SetAttrProvider(new MyGridCellAttrProvider);
    grid->AssignTable(table);

    wxGridCellAttr *attrRO = new wxGridCellAttr,
                   *attrRangeEditor = new wxGridCellAttr,
                   *attrCombo = new wxGridCellAttr;

    attrRO->SetReadOnly();
    attrRangeEditor->SetEditor(new wxGridCellNumberEditor(1, 5));
    attrCombo->SetEditor(new wxGridCellChoiceEditor(WXSIZEOF(severities),
                                                    severities));

    grid->SetColAttr(Col_Id, attrRO);
    grid->SetColAttr(Col_Priority, attrRangeEditor);
    grid->SetColAttr(Col_Severity, attrCombo);

    grid->Fit();
    SetClientSize(grid->GetSize());
}

// ============================================================================
// TabularGrid: grid used for display of tabular data
// ============================================================================

class TabularGridTable : public wxGridTableBase
{
public:
    enum
    {
        COL_NAME,
        COL_EXT,
        COL_SIZE,
        COL_DATE,
        COL_MAX
    };

    enum
    {
        ROW_MAX = 3
    };

    TabularGridTable() { m_sortOrder = nullptr; }

    virtual int GetNumberRows() override { return ROW_MAX; }
    virtual int GetNumberCols() override { return COL_MAX; }

    virtual wxString GetValue(int row, int col) override
    {
        if ( m_sortOrder )
            row = m_sortOrder[row];

        switch ( col )
        {
            case COL_NAME:
            case COL_EXT:
                return GetNameOrExt(row, col);

            case COL_SIZE:
                return wxString::Format("%lu", GetSize(row));

            case COL_DATE:
                return GetDate(row).FormatDate();

            case COL_MAX:
            default:
                wxFAIL_MSG( "unknown column" );
        }

        return wxString();
    }

    virtual void SetValue(int, int, const wxString&) override
    {
        wxFAIL_MSG( "shouldn't be called" );
    }

    virtual wxString GetColLabelValue(int col) override
    {
        // notice that column parameter here always refers to the internal
        // column index, independently of its position on the screen
        static const char *labels[] = { "Name", "Extension", "Size", "Date" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(labels) == COL_MAX, LabelsMismatch );

        return labels[col];
    }

    virtual void SetColLabelValue(int, const wxString&) override
    {
        wxFAIL_MSG( "shouldn't be called" );
    }

    void Sort(int col, bool ascending)
    {
        // we hardcode all sorting orders for simplicity here
        static int sortOrders[COL_MAX][2][ROW_MAX] =
        {
            // descending   ascending
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 1, 0, 2 }, { 2, 0, 1 } },
        };

        m_sortOrder = col == wxNOT_FOUND ? nullptr : sortOrders[col][ascending];
    }

private:
    wxString GetNameOrExt(int row, int col) const
    {
        static const char *
            names[] = { "autoexec.bat", "boot.ini", "io.sys" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(names) == ROW_MAX, NamesMismatch );

        const wxString s(names[row]);
        return col == COL_NAME ? s.BeforeFirst('.') : s.AfterLast('.');
    }

    unsigned long GetSize(int row) const
    {
        static const unsigned long
            sizes[] = { 412, 604, 40774 };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(sizes) == ROW_MAX, SizesMismatch );

        return sizes[row];
    }

    wxDateTime GetDate(int row) const
    {
        static const char *
            dates[] = { "2004-04-17", "2006-05-27", "1994-05-31" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(dates) == ROW_MAX, DatesMismatch );

        wxDateTime dt;
        dt.ParseISODate(dates[row]);
        return dt;
    }

    int *m_sortOrder;
};

// specialized text control for column indexes entry
class ColIndexEntry : public wxTextCtrl
{
public:
    ColIndexEntry(wxWindow *parent)
        : wxTextCtrl(parent, wxID_ANY, "")
    {
        SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    }

    int GetCol()
    {
        unsigned long col;
        if ( !GetValue().ToULong(&col) || col > TabularGridTable::COL_MAX )
        {
            SetFocus();
            return -1;
        }

        return col;
    }

protected:
    virtual wxSize DoGetBestSize() const override
    {
        wxSize size = wxTextCtrl::DoGetBestSize();
        size.x = 3*GetCharWidth();
        return size;
    }
};

class TabularGridFrame : public wxFrame
{
public:
    TabularGridFrame();
    TabularGridFrame(const TabularGridFrame&) = delete;
    TabularGridFrame& operator=(const TabularGridFrame&) = delete;

private:
    enum // control ids
    {
        Id_Check_UseNativeHeader,
        Id_Check_DrawNativeLabels,
        Id_Check_ShowRowLabels,
        Id_Check_EnableRowMove,
        Id_Check_EnableColMove
    };

    // event handlers

    void OnToggleUseNativeHeader(wxCommandEvent&)
    {
        m_grid->UseNativeColHeader(m_chkUseNative->IsChecked());
    }

    void OnUpdateDrawNativeLabelsUI(wxUpdateUIEvent& event)
    {
        // we don't draw labels at all, native or otherwise, if we use the
        // native header control
        event.Enable( !m_chkUseNative->GetValue() );
    }

    void OnToggleDrawNativeLabels(wxCommandEvent&)
    {
        m_grid->SetUseNativeColLabels(m_chkDrawNative->IsChecked());
    }

    void OnToggleShowRowLabels(wxCommandEvent&)
    {
        m_grid->SetRowLabelSize(m_chkShowRowLabels->IsChecked()
                                    ? wxGRID_AUTOSIZE
                                    : 0);
    }

    void OnToggleRowMove(wxCommandEvent&)
    {
        m_grid->EnableDragRowMove(m_chkEnableRowMove->IsChecked());
    }

    void OnToggleColMove(wxCommandEvent&)
    {
        m_grid->EnableDragColMove(m_chkEnableColMove->IsChecked());
    }

    void OnShowHideColumn(wxCommandEvent& event)
    {
        int col = m_txtColShowHide->GetCol();
        if ( col != -1 )
        {
            m_grid->SetColSize(col,
                               event.GetId() == wxID_ADD ? wxGRID_AUTOSIZE : 0);

            UpdateColOrderAndVisibility();
        }
    }

    void OnMoveColumn(wxCommandEvent&)
    {
        int col = m_txtColIndex->GetCol();
        int pos = m_txtColPos->GetCol();
        if ( col == -1 || pos == -1 )
            return;

        m_grid->SetColPos(col, pos);

        UpdateColOrderAndVisibility();
    }

    void OnResetColumnOrder(wxCommandEvent&)
    {
        m_grid->ResetColPos();

        UpdateColOrderAndVisibility();
    }

    void OnGridColSort(wxGridEvent& event)
    {
        const int col = event.GetCol();
        m_table->Sort(col, !(m_grid->IsSortingBy(col) &&
                             m_grid->IsSortOrderAscending()));
    }

    void OnGridRowMove(wxGridEvent& event)
    {
        // can't update it yet as the order hasn't been changed, so do it a bit
        // later
        m_shouldUpdateRowOrder = true;

        event.Skip();
    }

    void OnGridColMove(wxGridEvent& event)
    {
        // can't update it yet as the order hasn't been changed, so do it a bit
        // later
        m_shouldUpdateColOrder = true;

        event.Skip();
    }

    void OnGridColSize(wxGridSizeEvent& event)
    {
        // we only catch this event to react to the user showing or hiding this
        // column using the header control menu and not because we're
        // interested in column resizing
        UpdateColOrderAndVisibility();

        event.Skip();
    }

    void OnIdle(wxIdleEvent& event)
    {
        if ( m_shouldUpdateColOrder )
        {
            m_shouldUpdateColOrder = false;
            UpdateColOrderAndVisibility();
        }

        if ( m_shouldUpdateRowOrder )
        {
            m_shouldUpdateRowOrder = false;
            UpdateRowOrderAndVisibility();
        }

        event.Skip();
    }

    void UpdateColOrderAndVisibility()
    {
        wxString s;
        for ( int pos = 0; pos < TabularGridTable::COL_MAX; pos++ )
        {
            const int col = m_grid->GetColAt(pos);
            const bool isHidden = m_grid->GetColSize(col) == 0;

            if ( isHidden )
                s << '[';
            s << col;
            if ( isHidden )
                s << ']';

            s << ' ';
        }

        m_statOrder->SetLabel(s);
    }

    void UpdateRowOrderAndVisibility()
    {
        wxString s;
        for ( int pos = 0; pos < TabularGridTable::ROW_MAX; pos++ )
        {
            const int row = m_grid->GetRowAt(pos);
            const bool isHidden = m_grid->GetRowSize(row) == 0;

            if ( isHidden )
                s << '[';
            s << row;
            if ( isHidden )
                s << ']';

            s << ' ';
        }

        m_statOrder->SetLabel(s);
    }

    // controls
    wxGrid *m_grid;
    TabularGridTable *m_table;
    wxCheckBox *m_chkUseNative,
               *m_chkDrawNative,
               *m_chkShowRowLabels,
               *m_chkEnableRowMove,
               *m_chkEnableColMove;

    ColIndexEntry *m_txtColIndex,
                  *m_txtColPos,
                  *m_txtColShowHide;

    wxStaticText *m_statOrder;

    // fla for EVT_IDLE handler
    bool m_shouldUpdateRowOrder,
         m_shouldUpdateColOrder;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(TabularGridFrame, wxFrame)
    EVT_CHECKBOX(Id_Check_UseNativeHeader,
                 TabularGridFrame::OnToggleUseNativeHeader)
    EVT_CHECKBOX(Id_Check_DrawNativeLabels,
                 TabularGridFrame::OnToggleDrawNativeLabels)
    EVT_CHECKBOX(Id_Check_ShowRowLabels,
                 TabularGridFrame::OnToggleShowRowLabels)
    EVT_CHECKBOX(Id_Check_EnableColMove,
                 TabularGridFrame::OnToggleColMove)

    EVT_UPDATE_UI(Id_Check_DrawNativeLabels,
                  TabularGridFrame::OnUpdateDrawNativeLabelsUI)

    EVT_BUTTON(wxID_APPLY, TabularGridFrame::OnMoveColumn)
    EVT_BUTTON(wxID_RESET, TabularGridFrame::OnResetColumnOrder)
    EVT_BUTTON(wxID_ADD, TabularGridFrame::OnShowHideColumn)
    EVT_BUTTON(wxID_DELETE, TabularGridFrame::OnShowHideColumn)

    EVT_GRID_COL_SORT(TabularGridFrame::OnGridColSort)
    EVT_GRID_ROW_MOVE(TabularGridFrame::OnGridRowMove)
    EVT_GRID_COL_MOVE(TabularGridFrame::OnGridColMove)
    EVT_GRID_COL_SIZE(TabularGridFrame::OnGridColSize)

    EVT_IDLE(TabularGridFrame::OnIdle)
wxEND_EVENT_TABLE()

TabularGridFrame::TabularGridFrame()
                : wxFrame(nullptr, wxID_ANY, "Tabular table")
{
    m_shouldUpdateColOrder = false;

    wxPanel * const panel = new wxPanel(this);

    // create and initialize the grid with the specified data
    m_table = new TabularGridTable;
    m_grid = new wxGrid(panel, wxID_ANY,
                        wxDefaultPosition, wxDefaultSize,
                        wxBORDER_STATIC | wxWANTS_CHARS);
    m_grid->AssignTable(m_table, wxGrid::wxGridSelectRows);

    m_grid->EnableDragRowMove();
    m_grid->EnableDragColMove();
    m_grid->UseNativeColHeader();
    m_grid->HideRowLabels();

    // add it and the other controls to the frame
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(m_grid, wxSizerFlags(1).Expand().Border());

    wxSizer * const sizerControls = new wxBoxSizer(wxHORIZONTAL);

    wxSizer * const sizerStyles = new wxBoxSizer(wxVERTICAL);
    m_chkUseNative = new wxCheckBox(panel, Id_Check_UseNativeHeader,
                                    "&Use native header");
    m_chkUseNative->SetValue(true);
    sizerStyles->Add(m_chkUseNative, wxSizerFlags().Border());

    m_chkDrawNative = new wxCheckBox(panel, Id_Check_DrawNativeLabels,
                                    "&Draw native column labels");
    sizerStyles->Add(m_chkDrawNative, wxSizerFlags().Border());

    m_chkShowRowLabels = new wxCheckBox(panel, Id_Check_ShowRowLabels,
                                        "Show &row labels");
    sizerStyles->Add(m_chkShowRowLabels, wxSizerFlags().Border());

    m_chkEnableRowMove = new wxCheckBox(panel, Id_Check_EnableRowMove,
                                        "Allow row reordering");
    m_chkEnableRowMove->SetValue(true);
    sizerStyles->Add(m_chkEnableRowMove, wxSizerFlags().Border());

    m_chkEnableColMove = new wxCheckBox(panel, Id_Check_EnableColMove,
                                        "Allow column re&ordering");
    m_chkEnableColMove->SetValue(true);
    sizerStyles->Add(m_chkEnableColMove, wxSizerFlags().Border());

    sizerControls->Add(sizerStyles);

    sizerControls->AddSpacer(FromDIP(10));

    wxSizer * const sizerColumns = new wxBoxSizer(wxVERTICAL);
    wxSizer * const sizerMoveCols = new wxBoxSizer(wxHORIZONTAL);
    const wxSizerFlags
        flagsHorz(wxSizerFlags().Border(wxLEFT | wxRIGHT).Centre());
    sizerMoveCols->Add(new wxStaticText(panel, wxID_ANY, "&Move column"),
                       flagsHorz);
    m_txtColIndex = new ColIndexEntry(panel);
    sizerMoveCols->Add(m_txtColIndex, flagsHorz);
    sizerMoveCols->Add(new wxStaticText(panel, wxID_ANY, "&to"), flagsHorz);
    m_txtColPos = new ColIndexEntry(panel);
    sizerMoveCols->Add(m_txtColPos, flagsHorz);
    sizerMoveCols->Add(new wxButton(panel, wxID_APPLY), flagsHorz);

    sizerColumns->Add(sizerMoveCols, wxSizerFlags().Expand().Border(wxBOTTOM));

    wxSizer * const sizerShowCols = new wxBoxSizer(wxHORIZONTAL);
    sizerShowCols->Add(new wxStaticText(panel, wxID_ANY, "Current order:"),
                       flagsHorz);
    m_statOrder = new wxStaticText(panel, wxID_ANY, "<<< default >>>");
    sizerShowCols->Add(m_statOrder, flagsHorz);
    sizerShowCols->Add(new wxButton(panel, wxID_RESET, "&Reset order"));
    sizerColumns->Add(sizerShowCols, wxSizerFlags().Expand().Border(wxTOP));

    wxSizer * const sizerShowHide = new wxBoxSizer(wxHORIZONTAL);
    sizerShowHide->Add(new wxStaticText(panel, wxID_ANY, "Show/hide column:"),
                       flagsHorz);
    m_txtColShowHide = new ColIndexEntry(panel);
    sizerShowHide->Add(m_txtColShowHide, flagsHorz);
    sizerShowHide->Add(new wxButton(panel, wxID_ADD, "&Show"), flagsHorz);
    sizerShowHide->Add(new wxButton(panel, wxID_DELETE, "&Hide"), flagsHorz);
    sizerColumns->Add(sizerShowHide, wxSizerFlags().Expand().Border(wxTOP));

    sizerControls->Add(sizerColumns, wxSizerFlags(1).Expand().Border());

    sizerTop->Add(sizerControls, wxSizerFlags().Expand().Border());

    panel->SetSizer(sizerTop);

    SetClientSize(panel->GetBestSize());
    SetSizeHints(GetSize());

    Show();
}

void GridFrame::OnTabularTable(wxCommandEvent&)
{
    new TabularGridFrame;
}

// Example using wxGrid::Render
// Displays a preset selection or, if it exists, a selection block
// Draws the selection to a wxBitmap and displays the bitmap
void GridFrame::OnGridRender( wxCommandEvent& event )
{
    int styleRender = 0, i;
    bool useLometric = false, defSize = false;
    double zoom = 1;
    wxSize sizeOffset( 0, 0 );
    wxPoint pointOrigin( 0, 0 );

    wxMenu* menu = GetMenuBar()->GetMenu( 0 );
    wxMenuItem* menuItem = menu->FindItem( ID_RENDER_ROW_LABEL );
    menu = menuItem->GetMenu();

    if ( menu->FindItem( ID_RENDER_ROW_LABEL )->IsChecked() )
        styleRender |= wxGRID_DRAW_ROWS_HEADER;
    if ( menu->FindItem( ID_RENDER_COL_LABEL )->IsChecked() )
        styleRender |= wxGRID_DRAW_COLS_HEADER;
    if ( menu->FindItem( ID_RENDER_GRID_LINES )->IsChecked() )
        styleRender |= wxGRID_DRAW_CELL_LINES;
    if ( menu->FindItem( ID_RENDER_GRID_BORDER )->IsChecked() )
        styleRender |= wxGRID_DRAW_BOX_RECT;
    if ( menu->FindItem( ID_RENDER_SELECT_HLIGHT )->IsChecked() )
        styleRender |= wxGRID_DRAW_SELECTION;
    if ( menu->FindItem( ID_RENDER_LOMETRIC )->IsChecked() )
        useLometric = true;
    if ( menu->FindItem( ID_RENDER_MARGIN )->IsChecked() )
    {
        pointOrigin.x += 50;
        pointOrigin.y += 50;
        sizeOffset.IncBy( 50 );
    }
    if ( menu->FindItem( ID_RENDER_ZOOM )->IsChecked() )
        zoom = 1.25;
    if ( menu->FindItem( ID_RENDER_DEFAULT_SIZE )->IsChecked() )
        defSize = true;

    // init render area coords with a default row and col selection
    wxGridCellCoords topLeft( 0, 0 ), bottomRight( 8, 6 );
    // check whether we are printing a block selection
    // other selection types not catered for here
    if ( event.GetId() == ID_RENDER_COORDS )
    {
        topLeft.SetCol( 6 );
        topLeft.SetRow( 4 );
        bottomRight.SetCol( 15 );
        bottomRight.SetRow( 29 );
    }
    else if ( grid->IsSelection() && grid->GetSelectionBlockTopLeft().Count() )
    {
        wxGridCellCoordsVector cells = grid->GetSelectionBlockTopLeft();
        if ( grid->GetSelectionBlockBottomRight().Count() )
        {
            cells.push_back( grid->GetSelectionBlockBottomRight()[ 0 ] );
            topLeft.Set( cells[ 0 ].GetRow(),
                            cells[ 0 ].GetCol() );
            bottomRight.Set( cells[ 1 ].GetRow(),
                                cells[ 1 ].GetCol() );
        }
    }

    // sum col widths
    wxSize sizeRender( 0, 0 );
    wxGridSizesInfo sizeinfo = grid->GetColSizes();
    for ( i = topLeft.GetCol(); i <= bottomRight.GetCol(); i++ )
    {
        sizeRender.x += sizeinfo.GetSize( i );
    }

    // sum row heights
    sizeinfo = grid->GetRowSizes();
    for ( i = topLeft.GetRow(); i <= bottomRight.GetRow(); i++ )
    {
        sizeRender.y += sizeinfo.GetSize( i );
    }

    if ( styleRender & wxGRID_DRAW_ROWS_HEADER )
        sizeRender.x += grid->GetRowLabelSize();
    if ( styleRender & wxGRID_DRAW_COLS_HEADER )
        sizeRender.y += grid->GetColLabelSize();

    sizeRender *= zoom;

    // delete any existing render frame and create new one
    wxWindow* win = FindWindowByName( "frameRender" );
    if ( win )
        win->Destroy();

    // create a frame large enough for the rendered bitmap
    wxFrame* frame = new wxFrame( this, wxID_ANY, "Grid Render" );
    frame->SetClientSize( sizeRender + sizeOffset * 2 );
    frame->SetName( "frameRender" );

    wxPanel* canvas = new wxPanel( frame, wxID_ANY );

    // make a bitmap large enough for any top/left offset
    wxBitmap bmp( sizeRender + sizeOffset );
    wxMemoryDC memDc(bmp);

    // default row labels have no background colour so set background
    memDc.SetBackground( wxBrush( canvas->GetBackgroundColour() ) );
    memDc.Clear();

    // convert sizeRender to mapping mode units if necessary
    if ( useLometric )
    {
        memDc.SetMapMode( wxMM_LOMETRIC );
        sizeRender.x = memDc.DeviceToLogicalXRel( sizeRender.x );
        sizeRender.y = memDc.DeviceToLogicalYRel( sizeRender.y );
    }

    // pass wxDefaultSize if menu item is checked
    if ( defSize )
        sizeRender = wxDefaultSize;

    grid->Render( memDc,
                  pointOrigin,
                  sizeRender,
                  topLeft, bottomRight,
                  wxGridRenderStyle( styleRender ) );

    m_gridBitmap = bmp;

    canvas->Bind( wxEVT_PAINT, &GridFrame::OnRenderPaint, this );

    frame->Show();
}

void GridFrame::OnRenderPaint( wxPaintEvent& event )
{
    wxPanel* canvas = ( wxPanel* )event.GetEventObject();
    wxPaintDC dc( canvas );
    canvas->PrepareDC( dc );

    if ( !m_gridBitmap.IsOk() )
        return;

    wxMemoryDC memDc( m_gridBitmap );

    dc.Blit( 0, 0,
             m_gridBitmap.GetWidth(),
             m_gridBitmap.GetHeight(),
             &memDc, 0, 0 );
}

void GridFrame::HideCol( wxCommandEvent& WXUNUSED(event) )
{
    grid->HideCol(0);
}

void GridFrame::ShowCol( wxCommandEvent& WXUNUSED(event) )
{
    grid->ShowCol(0);
}

void GridFrame::HideRow( wxCommandEvent& WXUNUSED(event) )
{
    grid->HideRow(1);
}

void GridFrame::ShowRow( wxCommandEvent& WXUNUSED(event) )
{
    grid->ShowRow(1);
}

namespace
{

// Toggle status of either checkered or coloured cells in the grid.
// Note that, for shared attribute testing purposes, with checkered cells
// a cell's complete attribute is destructively overwritten (except merged
// cells which are skipped). While with coloured cells only the background
// colour changes.
void ToggleGridCells(wxGrid* grid, bool useCheckered)
{
    static bool s_checkeredOn, s_colouredOn;
    if ( useCheckered )
        s_checkeredOn = !s_checkeredOn;
    else
        s_colouredOn = !s_colouredOn;

    wxGridCellAttrPtr attr;
    if ( useCheckered && s_checkeredOn )
    {
        attr = wxGridCellAttrPtr(new wxGridCellAttr);
        attr->SetBackgroundColour(*wxLIGHT_GREY);
    }

    wxColour bgCol = grid->GetDefaultCellBackgroundColour();

    for ( int row = 0; row < grid->GetNumberRows(); ++row )
    {
        for ( int col = 0; col < grid->GetNumberCols(); ++col )
        {
            if ( useCheckered && (row ^ col) & 1 )
                continue;

            // Skip overwriting attributes of merged cells.
            int rows, cols;
            if ( useCheckered
                 && grid->GetCellSize(row, col, &rows, &cols)
                    != wxGrid::CellSpan_None )
            {
                continue;
            }

            if ( useCheckered )
            {
                grid->SetAttr(row, col, attr.get());

                if ( s_checkeredOn )
                    attr->IncRef();
            }
            else
            {
                if ( s_colouredOn )
                {
                    const int factor = 256 / 64;
                    unsigned char r, g, b;
                    r = (127 + row * factor + col / factor) & 0xff;
                    g = (col * factor + row / (factor * 2)) & 0xff;
                    b = ((row ^ col) * factor) & 0xff;

                    bgCol.Set(r < 128 ? r * 2 : 255 - r * 2,
                              g < 128 ? g * 2 : 255 - g * 2,
                              b < 128 ? b * 2 : 255 - b * 2);
                }

                grid->SetCellBackgroundColour(row, col, bgCol);
            }
        }
    }

    grid->Refresh();
}

} // anoymous namespace

void GridFrame::ToggleCheckeredCells( wxCommandEvent& WXUNUSED(event) )
{
    ToggleGridCells(grid, /* useCheckered = */ true);
}

void GridFrame::ToggleColouredCells( wxCommandEvent& WXUNUSED(event) )
{
    ToggleGridCells(grid, /* useCheckered = */ false);
}
