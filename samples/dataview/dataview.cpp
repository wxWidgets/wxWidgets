/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/dataview.h"
#include "wx/datetime.h"
#include "wx/splitter.h"
#include "wx/aboutdlg.h"
#include "wx/colordlg.h"
#include "wx/choicdlg.h"
#include "wx/numdlg.h"
#include "wx/spinctrl.h"
#include "wx/imaglist.h"
#include "wx/itemattr.h"
#include "wx/notebook.h"

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    #include "wx/headerctrl.h"
#endif // wxHAS_GENERIC_DATAVIEWCTRL

#include "mymodels.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "wx_small.xpm"

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h);
    ~MyFrame();

    void BuildDataViewCtrl(wxPanel* parent,
                           unsigned int nPanel,
                           unsigned long style = 0);

private:
    // event handlers
    void OnStyleChange(wxCommandEvent& event);
    void OnSetBackgroundColour(wxCommandEvent& event);
    void OnCustomHeaderAttr(wxCommandEvent& event);
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    void OnCustomHeaderHeight(wxCommandEvent& event);
#endif // wxHAS_GENERIC_DATAVIEWCTRL
    void OnGetPageInfo(wxCommandEvent& event);
    void OnDisable(wxCommandEvent& event);
    void OnSetForegroundColour(wxCommandEvent& event);
    void OnIncIndent(wxCommandEvent& event);
    void OnDecIndent(wxCommandEvent& event);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClearLog(wxCommandEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);

    void OnAddMozart(wxCommandEvent& event);
    void OnDeleteSelected(wxCommandEvent& event);
    void OnDeleteYear(wxCommandEvent& event);
    void OnSelectNinth(wxCommandEvent& event);
    void OnCollapse(wxCommandEvent& event);
    void OnExpand(wxCommandEvent& event);
    void OnShowCurrent(wxCommandEvent& event);
    void OnSetNinthCurrent(wxCommandEvent& event);
    void OnChangeNinthTitle(wxCommandEvent& event);

    void OnPrependList(wxCommandEvent& event);
    void OnDeleteList(wxCommandEvent& event);

    // Third (wxDataViewListCtrl) page.
    void OnListValueChanged(wxDataViewEvent& event);

    // Fourth page.
    void OnDeleteTreeItem(wxCommandEvent& event);
    void OnDeleteAllTreeItems(wxCommandEvent& event);
    void OnAddTreeItem(wxCommandEvent& event);
    void OnAddTreeContainerItem(wxCommandEvent& event);

    void OnValueChanged( wxDataViewEvent &event );

    void OnActivated( wxDataViewEvent &event );
    void OnExpanding( wxDataViewEvent &event );
    void OnExpanded( wxDataViewEvent &event );
    void OnCollapsing( wxDataViewEvent &event );
    void OnCollapsed( wxDataViewEvent &event );
    void OnSelectionChanged( wxDataViewEvent &event );

    void OnStartEditing( wxDataViewEvent &event );
    void OnEditingStarted( wxDataViewEvent &event );
    void OnEditingDone( wxDataViewEvent &event );

    void OnHeaderClick( wxDataViewEvent &event );
    void OnAttrHeaderClick( wxDataViewEvent &event );
    void OnHeaderRightClick( wxDataViewEvent &event );
    void OnHeaderClickList( wxDataViewEvent &event );
    void OnSorted( wxDataViewEvent &event );
    void OnSortedList( wxDataViewEvent &event );
    void OnColumnReordered( wxDataViewEvent &event);

    void OnContextMenu( wxDataViewEvent &event );

    void OnGoto( wxCommandEvent &event);
    void OnAddMany( wxCommandEvent &event);
    void OnHideAttributes( wxCommandEvent &event);
    void OnShowAttributes( wxCommandEvent &event);

    void OnMultipleSort( wxCommandEvent &event);
    void OnSortByFirstColumn( wxCommandEvent &event);

#if wxUSE_DRAG_AND_DROP
    void OnBeginDrag( wxDataViewEvent &event );
    void OnDropPossible( wxDataViewEvent &event );
    void OnDrop( wxDataViewEvent &event );
#endif // wxUSE_DRAG_AND_DROP

    void OnDataViewChar(wxKeyEvent& event);

    // helper used by both OnDeleteSelected() and OnDataViewChar()
    void DeleteSelectedItems();


    wxNotebook* m_notebook;

    // the controls stored in the various tabs of the main notebook:

    wxDataViewCtrl* m_ctrl[4];

    // the models associated with the first two DVC:

    wxObjectDataPtr<MyMusicTreeModel> m_music_model;
    wxObjectDataPtr<MyListModel> m_list_model;

    // other data:

    wxDataViewColumn* m_col;
    wxDataViewColumn* m_attributes;

    wxTextCtrl* m_log;
    wxLog *m_logOld;

private:
    // Flag used by OnListValueChanged(), see there.
    bool m_eventFromProgram;

    wxDECLARE_EVENT_TABLE();
};


// ----------------------------------------------------------------------------
// MyCustomRenderer
// ----------------------------------------------------------------------------

class MyCustomRenderer: public wxDataViewCustomRenderer
{
public:
    // This renderer can be either activatable or editable, for demonstration
    // purposes. In real programs, you should select whether the user should be
    // able to activate or edit the cell and it doesn't make sense to switch
    // between the two -- but this is just an example, so it doesn't stop us.
    explicit MyCustomRenderer(wxDataViewCellMode mode)
        : wxDataViewCustomRenderer("string", mode, wxALIGN_CENTER)
       { }

    virtual bool Render( wxRect rect, wxDC *dc, int state ) wxOVERRIDE
    {
        dc->SetBrush( *wxLIGHT_GREY_BRUSH );
        dc->SetPen( *wxTRANSPARENT_PEN );

        rect.Deflate(2);
        dc->DrawRoundedRectangle( rect, 5 );

        RenderText(m_value,
                   0, // no offset
                   wxRect(dc->GetTextExtent(m_value)).CentreIn(rect),
                   dc,
                   state);
        return true;
    }

    virtual bool ActivateCell(const wxRect& WXUNUSED(cell),
                              wxDataViewModel *WXUNUSED(model),
                              const wxDataViewItem &WXUNUSED(item),
                              unsigned int WXUNUSED(col),
                              const wxMouseEvent *mouseEvent) wxOVERRIDE
    {
        wxString position;
        if ( mouseEvent )
            position = wxString::Format("via mouse at %d, %d", mouseEvent->m_x, mouseEvent->m_y);
        else
            position = "from keyboard";
        wxLogMessage("MyCustomRenderer ActivateCell() %s", position);
        return false;
    }

    virtual wxSize GetSize() const wxOVERRIDE
    {
        return wxSize(60,20);
    }

    virtual bool SetValue( const wxVariant &value ) wxOVERRIDE
    {
        m_value = value.GetString();
        return true;
    }

    virtual bool GetValue( wxVariant &WXUNUSED(value) ) const wxOVERRIDE { return true; }

#if wxUSE_ACCESSIBILITY
    virtual wxString GetAccessibleDescription() const wxOVERRIDE
    {
        return m_value;
    }
#endif // wxUSE_ACCESSIBILITY

    virtual bool HasEditorCtrl() const wxOVERRIDE { return true; }

    virtual wxWindow*
    CreateEditorCtrl(wxWindow* parent,
                     wxRect labelRect,
                     const wxVariant& value) wxOVERRIDE
    {
        wxTextCtrl* text = new wxTextCtrl(parent, wxID_ANY, value,
                                          labelRect.GetPosition(),
                                          labelRect.GetSize(),
                                          wxTE_PROCESS_ENTER);
        text->SetInsertionPointEnd();

        return text;
    }

    virtual bool
    GetValueFromEditorCtrl(wxWindow* ctrl, wxVariant& value) wxOVERRIDE
    {
        wxTextCtrl* text = wxDynamicCast(ctrl, wxTextCtrl);
        if ( !text )
            return false;

        value = text->GetValue();

        return true;
    }

private:
    wxString m_value;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame =
        new MyFrame(NULL, "wxDataViewCtrl sample", 40, 40, 1000, 540);

    frame->Show(true);
    return true;
}


// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

enum
{
    ID_CLEARLOG = wxID_HIGHEST+1,
    ID_GET_PAGE_INFO,
    ID_DISABLE,
    ID_BACKGROUND_COLOUR,
    ID_FOREGROUND_COLOUR,
    ID_CUSTOM_HEADER_ATTR,
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    ID_CUSTOM_HEADER_HEIGHT,
#endif // wxHAS_GENERIC_DATAVIEWCTRL
    ID_STYLE_MENU,
    ID_INC_INDENT,
    ID_DEC_INDENT,

    // file menu
    //ID_SINGLE,        wxDV_SINGLE==0 so it's always present
    ID_MULTIPLE,
    ID_ROW_LINES,
    ID_HORIZ_RULES,
    ID_VERT_RULES,

    ID_EXIT = wxID_EXIT,

    // about menu
    ID_ABOUT = wxID_ABOUT,


    // control IDs

    ID_MUSIC_CTRL       = 50,
    ID_ATTR_CTRL        = 51,

    ID_ADD_MOZART       = 100,
    ID_DELETE_SEL       = 101,
    ID_DELETE_YEAR      = 102,
    ID_SELECT_NINTH     = 103,
    ID_COLLAPSE         = 104,
    ID_EXPAND           = 105,
    ID_SHOW_CURRENT,
    ID_SET_NINTH_CURRENT,
    ID_CHANGE_NINTH_TITLE,

    ID_PREPEND_LIST     = 200,
    ID_DELETE_LIST      = 201,
    ID_GOTO             = 202,
    ID_ADD_MANY         = 203,
    ID_HIDE_ATTRIBUTES  = 204,
    ID_SHOW_ATTRIBUTES  = 205,
    ID_MULTIPLE_SORT    = 206,
    ID_SORT_BY_FIRST_COLUMN,

    // Fourth page.
    ID_DELETE_TREE_ITEM = 400,
    ID_DELETE_ALL_TREE_ITEMS = 401,
    ID_ADD_TREE_ITEM = 402,
    ID_ADD_TREE_CONTAINER_ITEM = 403
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU_RANGE( ID_MULTIPLE, ID_VERT_RULES, MyFrame::OnStyleChange )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_CLEARLOG, MyFrame::OnClearLog )

    EVT_MENU( ID_GET_PAGE_INFO, MyFrame::OnGetPageInfo )
    EVT_MENU( ID_DISABLE, MyFrame::OnDisable )
    EVT_MENU( ID_FOREGROUND_COLOUR, MyFrame::OnSetForegroundColour )
    EVT_MENU( ID_BACKGROUND_COLOUR, MyFrame::OnSetBackgroundColour )
    EVT_MENU( ID_CUSTOM_HEADER_ATTR, MyFrame::OnCustomHeaderAttr )
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    EVT_MENU( ID_CUSTOM_HEADER_HEIGHT, MyFrame::OnCustomHeaderHeight )
#endif // wxHAS_GENERIC_DATAVIEWCTRL
    EVT_MENU( ID_INC_INDENT, MyFrame::OnIncIndent )
    EVT_MENU( ID_DEC_INDENT, MyFrame::OnDecIndent )

    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, MyFrame::OnPageChanged )

    EVT_BUTTON( ID_ADD_MOZART, MyFrame::OnAddMozart )
    EVT_BUTTON( ID_DELETE_SEL, MyFrame::OnDeleteSelected )
    EVT_BUTTON( ID_DELETE_YEAR, MyFrame::OnDeleteYear )
    EVT_BUTTON( ID_SELECT_NINTH, MyFrame::OnSelectNinth )
    EVT_BUTTON( ID_COLLAPSE, MyFrame::OnCollapse )
    EVT_BUTTON( ID_EXPAND, MyFrame::OnExpand )
    EVT_BUTTON( ID_SHOW_CURRENT, MyFrame::OnShowCurrent )
    EVT_BUTTON( ID_SET_NINTH_CURRENT, MyFrame::OnSetNinthCurrent )
    EVT_BUTTON( ID_CHANGE_NINTH_TITLE, MyFrame::OnChangeNinthTitle )

    EVT_BUTTON( ID_PREPEND_LIST, MyFrame::OnPrependList )
    EVT_BUTTON( ID_DELETE_LIST, MyFrame::OnDeleteList )
    EVT_BUTTON( ID_GOTO, MyFrame::OnGoto)
    EVT_BUTTON( ID_ADD_MANY, MyFrame::OnAddMany)
    EVT_BUTTON( ID_HIDE_ATTRIBUTES, MyFrame::OnHideAttributes)
    EVT_BUTTON( ID_SHOW_ATTRIBUTES, MyFrame::OnShowAttributes)
    EVT_CHECKBOX( ID_MULTIPLE_SORT, MyFrame::OnMultipleSort)
    EVT_CHECKBOX( ID_SORT_BY_FIRST_COLUMN, MyFrame::OnSortByFirstColumn)
    
    // Fourth page.
    EVT_BUTTON( ID_DELETE_TREE_ITEM, MyFrame::OnDeleteTreeItem )
    EVT_BUTTON( ID_DELETE_ALL_TREE_ITEMS, MyFrame::OnDeleteAllTreeItems )
    EVT_BUTTON( ID_ADD_TREE_ITEM, MyFrame::OnAddTreeItem )
    EVT_BUTTON( ID_ADD_TREE_CONTAINER_ITEM, MyFrame::OnAddTreeContainerItem )

    EVT_DATAVIEW_ITEM_VALUE_CHANGED( ID_MUSIC_CTRL, MyFrame::OnValueChanged )

    EVT_DATAVIEW_ITEM_ACTIVATED(ID_MUSIC_CTRL, MyFrame::OnActivated )
    EVT_DATAVIEW_ITEM_EXPANDING(ID_MUSIC_CTRL, MyFrame::OnExpanding)
    EVT_DATAVIEW_ITEM_EXPANDED(ID_MUSIC_CTRL, MyFrame::OnExpanded)
    EVT_DATAVIEW_ITEM_COLLAPSING(ID_MUSIC_CTRL, MyFrame::OnCollapsing)
    EVT_DATAVIEW_ITEM_COLLAPSED(ID_MUSIC_CTRL, MyFrame::OnCollapsed)
    EVT_DATAVIEW_SELECTION_CHANGED(ID_MUSIC_CTRL, MyFrame::OnSelectionChanged)

    EVT_DATAVIEW_ITEM_START_EDITING(ID_MUSIC_CTRL, MyFrame::OnStartEditing)
    EVT_DATAVIEW_ITEM_EDITING_STARTED(wxID_ANY, MyFrame::OnEditingStarted)
    EVT_DATAVIEW_ITEM_EDITING_DONE(wxID_ANY, MyFrame::OnEditingDone)

    EVT_DATAVIEW_COLUMN_HEADER_CLICK(ID_MUSIC_CTRL, MyFrame::OnHeaderClick)
    EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK(ID_MUSIC_CTRL, MyFrame::OnHeaderRightClick)
    EVT_DATAVIEW_COLUMN_SORTED(ID_MUSIC_CTRL, MyFrame::OnSorted)
    EVT_DATAVIEW_COLUMN_SORTED(ID_ATTR_CTRL, MyFrame::OnSortedList)
    EVT_DATAVIEW_COLUMN_REORDERED(wxID_ANY, MyFrame::OnColumnReordered)
    EVT_DATAVIEW_COLUMN_HEADER_CLICK(ID_ATTR_CTRL, MyFrame::OnHeaderClickList)

    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_MUSIC_CTRL, MyFrame::OnContextMenu)

#if wxUSE_DRAG_AND_DROP
    EVT_DATAVIEW_ITEM_BEGIN_DRAG( ID_MUSIC_CTRL, MyFrame::OnBeginDrag )
    EVT_DATAVIEW_ITEM_DROP_POSSIBLE( ID_MUSIC_CTRL, MyFrame::OnDropPossible )
    EVT_DATAVIEW_ITEM_DROP( ID_MUSIC_CTRL, MyFrame::OnDrop )
#endif // wxUSE_DRAG_AND_DROP

    EVT_DATAVIEW_COLUMN_HEADER_CLICK(ID_ATTR_CTRL, MyFrame::OnAttrHeaderClick)

wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_log = NULL;
    m_col = NULL;

    m_ctrl[0] = NULL;
    m_ctrl[1] = NULL;
    m_ctrl[2] = NULL;
    m_ctrl[3] = NULL;

    m_eventFromProgram = false;

    SetIcon(wxICON(sample));


    // build the menus
    // ----------------

    wxMenu *style_menu = new wxMenu;
    //style_menu->AppendCheckItem(ID_SINGLE, "Single selection"));
    style_menu->AppendCheckItem(ID_MULTIPLE, "Multiple selection");
    style_menu->AppendCheckItem(ID_ROW_LINES, "Alternating colours");
    style_menu->AppendCheckItem(ID_HORIZ_RULES, "Display horizontal rules");
    style_menu->AppendCheckItem(ID_VERT_RULES, "Display vertical rules");

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(ID_CLEARLOG, "&Clear log\tCtrl-L");
    file_menu->Append(ID_GET_PAGE_INFO, "Show current &page info");
    file_menu->AppendCheckItem(ID_DISABLE, "&Disable\tCtrl-D");
    file_menu->Append(ID_FOREGROUND_COLOUR, "Set &foreground colour...\tCtrl-S");
    file_menu->Append(ID_BACKGROUND_COLOUR, "Set &background colour...\tCtrl-B");
    file_menu->AppendCheckItem(ID_CUSTOM_HEADER_ATTR, "C&ustom header attributes");
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    file_menu->AppendCheckItem(ID_CUSTOM_HEADER_HEIGHT, "Custom header &height");
#endif // wxHAS_GENERIC_DATAVIEWCTRL
    file_menu->Append(ID_STYLE_MENU, "&Style", style_menu);
    file_menu->Append(ID_INC_INDENT, "&Increase indent\tCtrl-I");
    file_menu->Append(ID_DEC_INDENT, "&Decrease indent\tShift-Ctrl-I");
    file_menu->AppendSeparator();
    file_menu->Append(ID_EXIT, "E&xit");

    wxMenu *about_menu = new wxMenu;
    about_menu->Append(ID_ABOUT, "&About");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(about_menu, "&About");

    SetMenuBar(menu_bar);
    CreateStatusBar();


    // redirect logs from our event handlers to text control
    m_log = new wxTextCtrl( this, wxID_ANY, wxString(), wxDefaultPosition,
                            wxDefaultSize, wxTE_MULTILINE );
    m_log->SetMinSize(wxSize(-1, 100));
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
    wxLogMessage( "This is the log window" );


    // first page of the notebook
    // --------------------------

    m_notebook = new wxNotebook( this, wxID_ANY );

    wxPanel *firstPanel = new wxPanel( m_notebook, wxID_ANY );

    BuildDataViewCtrl(firstPanel, 0);    // sets m_ctrl[0]

    const wxSizerFlags border = wxSizerFlags().DoubleBorder();

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add( new wxButton( firstPanel, ID_ADD_MOZART,  "Add Mozart"),             border );
    button_sizer->Add( new wxButton( firstPanel, ID_DELETE_SEL,  "Delete selected"),        border );
    button_sizer->Add( new wxButton( firstPanel, ID_DELETE_YEAR, "Delete \"Year\" column"), border );
    button_sizer->Add( new wxButton( firstPanel, ID_SELECT_NINTH,"Select ninth symphony"),  border );
    button_sizer->Add( new wxButton( firstPanel, ID_COLLAPSE,    "Collapse"),               border );
    button_sizer->Add( new wxButton( firstPanel, ID_EXPAND,      "Expand"),                 border );

    wxBoxSizer *sizerCurrent = new wxBoxSizer(wxHORIZONTAL);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SHOW_CURRENT,
                                   "&Show current"), border);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SET_NINTH_CURRENT,
                                   "Make &ninth symphony current"), border);
    sizerCurrent->Add(new wxButton(firstPanel, ID_CHANGE_NINTH_TITLE,
                                   "Change ninth &title"), border);

    wxSizer *firstPanelSz = new wxBoxSizer( wxVERTICAL );
    m_ctrl[0]->SetMinSize(wxSize(-1, 200));
    firstPanelSz->Add(m_ctrl[0], 1, wxGROW|wxALL, 5);
    firstPanelSz->Add(
        new wxStaticText(firstPanel, wxID_ANY, "Most of the cells above are editable!"),
        0, wxGROW|wxALL, 5);
    firstPanelSz->Add(button_sizer);
    firstPanelSz->Add(sizerCurrent);
    firstPanel->SetSizerAndFit(firstPanelSz);


    // second page of the notebook
    // ---------------------------

    wxPanel *secondPanel = new wxPanel( m_notebook, wxID_ANY );

    BuildDataViewCtrl(secondPanel, 1);    // sets m_ctrl[1]

    wxBoxSizer *button_sizer2 = new wxBoxSizer( wxHORIZONTAL );
    button_sizer2->Add( new wxButton( secondPanel, ID_PREPEND_LIST,"Prepend"),                0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_DELETE_LIST, "Delete selected"),        0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_GOTO,        "Goto 50"),                0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_ADD_MANY,    "Add 1000"),               0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_HIDE_ATTRIBUTES,    "Hide attributes"), 0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_SHOW_ATTRIBUTES,    "Show attributes"), 0, wxALL, 10 );

    wxBoxSizer *sortSizer = new wxBoxSizer(wxHORIZONTAL);
    sortSizer->Add(new wxCheckBox(secondPanel, ID_SORT_BY_FIRST_COLUMN, "Sort by first column"),
                   wxSizerFlags().Centre().DoubleBorder());
    sortSizer->Add(new wxCheckBox(secondPanel, ID_MULTIPLE_SORT, "Allow multisort"),
                   wxSizerFlags().Centre().DoubleBorder());

    wxSizer *secondPanelSz = new wxBoxSizer( wxVERTICAL );
    secondPanelSz->Add(m_ctrl[1], 1, wxGROW|wxALL, 5);
    secondPanelSz->Add(button_sizer2);
    secondPanelSz->Add(sortSizer);
    secondPanel->SetSizerAndFit(secondPanelSz);


    // third page of the notebook
    // ---------------------------

    wxPanel *thirdPanel = new wxPanel( m_notebook, wxID_ANY );

    BuildDataViewCtrl(thirdPanel, 2);    // sets m_ctrl[2]

    wxSizer *thirdPanelSz = new wxBoxSizer( wxVERTICAL );
    thirdPanelSz->Add(m_ctrl[2], 1, wxGROW|wxALL, 5);
    thirdPanel->SetSizerAndFit(thirdPanelSz);


    // fourth page of the notebook
    // ---------------------------

    wxPanel *fourthPanel = new wxPanel( m_notebook, wxID_ANY );

    BuildDataViewCtrl(fourthPanel, 3);    // sets m_ctrl[3]
    // Buttons
    wxBoxSizer *button_sizer4 = new wxBoxSizer( wxHORIZONTAL );
    button_sizer4->Add( new wxButton( fourthPanel, ID_DELETE_TREE_ITEM, "Delete Selected"), 0, wxALL, 10 );
    button_sizer4->Add( new wxButton( fourthPanel, ID_DELETE_ALL_TREE_ITEMS, "Delete All"), 0, wxALL, 10 );
    button_sizer4->Add( new wxButton( fourthPanel, ID_ADD_TREE_ITEM, "Add Item"), 0, wxALL, 10 );
    button_sizer4->Add( new wxButton( fourthPanel, ID_ADD_TREE_CONTAINER_ITEM, "Add Container"), 0, wxALL, 10 );

    wxSizer *fourthPanelSz = new wxBoxSizer( wxVERTICAL );
    fourthPanelSz->Add(m_ctrl[3], 1, wxGROW|wxALL, 5);
    fourthPanelSz->Add(button_sizer4);
    fourthPanel->SetSizerAndFit(fourthPanelSz);



    // complete GUI
    // ------------

    m_notebook->AddPage(firstPanel, "MyMusicTreeModel");
    m_notebook->AddPage(secondPanel, "MyListModel");
    m_notebook->AddPage(thirdPanel, "wxDataViewListCtrl");
    m_notebook->AddPage(fourthPanel, "wxDataViewTreeCtrl");

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add( m_notebook, 1, wxGROW );
    mainSizer->Add( m_log, 0, wxGROW );

    SetSizerAndFit(mainSizer);
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

void MyFrame::BuildDataViewCtrl(wxPanel* parent, unsigned int nPanel, unsigned long style)
{
    switch (nPanel)
    {
    case 0:
        {
            wxASSERT(!m_ctrl[0] && !m_music_model);
            m_ctrl[0] =
                new wxDataViewCtrl( parent, ID_MUSIC_CTRL, wxDefaultPosition,
                                    wxDefaultSize, style );
            m_ctrl[0]->Bind(wxEVT_CHAR, &MyFrame::OnDataViewChar, this);

            m_music_model = new MyMusicTreeModel;
            m_ctrl[0]->AssociateModel( m_music_model.get() );

#if wxUSE_DRAG_AND_DROP && wxUSE_UNICODE
            m_ctrl[0]->EnableDragSource( wxDF_UNICODETEXT );
            m_ctrl[0]->EnableDropTarget( wxDF_UNICODETEXT );
#endif // wxUSE_DRAG_AND_DROP && wxUSE_UNICODE

            // column 0 of the view control:

            wxDataViewTextRenderer *tr =
                new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
            wxDataViewColumn *column0 =
                new wxDataViewColumn( "title", tr, 0, 200, wxALIGN_LEFT,
                                      wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
            m_ctrl[0]->AppendColumn( column0 );
#if 0
            // Call this and sorting is enabled
            // immediately upon start up.
            column0->SetAsSortKey();
#endif

            // column 1 of the view control:

            tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_EDITABLE );
            wxDataViewColumn *column1 =
                new wxDataViewColumn( "artist", tr, 1, 150, wxALIGN_LEFT,
                                      wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE |
                                      wxDATAVIEW_COL_RESIZABLE );
            column1->SetMinWidth(150); // this column can't be resized to be smaller
            m_ctrl[0]->AppendColumn( column1 );

            // column 2 of the view control:

            wxDataViewSpinRenderer *sr =
                new wxDataViewSpinRenderer( 0, 2010, wxDATAVIEW_CELL_EDITABLE,
                                            wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
            wxDataViewColumn *column2 =
                new wxDataViewColumn( "year", sr, 2, 60, wxALIGN_LEFT,
                                      wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE );
            m_ctrl[0]->AppendColumn( column2 );

            // column 3 of the view control:

            wxArrayString choices;
            choices.Add( "good" );
            choices.Add( "bad" );
            choices.Add( "lousy" );
            wxDataViewChoiceRenderer *c =
                new wxDataViewChoiceRenderer( choices, wxDATAVIEW_CELL_EDITABLE,
                                              wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
            wxDataViewColumn *column3 =
                new wxDataViewColumn( "rating", c, 3, 100, wxALIGN_LEFT,
                                      wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );
            m_ctrl[0]->AppendColumn( column3 );

            // column 4 of the view control:

            m_ctrl[0]->AppendProgressColumn( "popularity", 4, wxDATAVIEW_CELL_INERT, 80 );

            // column 5 of the view control:

            MyCustomRenderer *cr = new MyCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE);
            wxDataViewColumn *column5 =
                new wxDataViewColumn( "custom", cr, 5, -1, wxALIGN_LEFT,
                                      wxDATAVIEW_COL_RESIZABLE );
            column5->SetBitmap(wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MENU));
            m_ctrl[0]->AppendColumn( column5 );


            // select initially the ninth symphony:
            m_ctrl[0]->Select(m_music_model->GetNinthItem());
        }
        break;

    case 1:
        {
            wxASSERT(!m_ctrl[1] && !m_list_model);
            m_ctrl[1] = new wxDataViewCtrl( parent, ID_ATTR_CTRL, wxDefaultPosition,
                                            wxDefaultSize, style );

            m_list_model = new MyListModel;
            m_ctrl[1]->AssociateModel( m_list_model.get() );

            m_ctrl[1]->AppendToggleColumn(L"\u2714",
                                          MyListModel::Col_Toggle,
                                          wxDATAVIEW_CELL_ACTIVATABLE,
                                          wxCOL_WIDTH_AUTOSIZE);

            // the various columns
            m_ctrl[1]->AppendTextColumn("editable string",
                                        MyListModel::Col_EditableText,
                                        wxDATAVIEW_CELL_EDITABLE,
                                        wxCOL_WIDTH_AUTOSIZE,
                                        wxALIGN_NOT,
                                        wxDATAVIEW_COL_SORTABLE);
            m_ctrl[1]->AppendIconTextColumn("icon",
                                            MyListModel::Col_IconText,
                                            wxDATAVIEW_CELL_EDITABLE,
                                            wxCOL_WIDTH_AUTOSIZE,
                                            wxALIGN_NOT,
                                            wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_SORTABLE);

            m_ctrl[1]->AppendDateColumn("date",
                                        MyListModel::Col_Date);

            wxDataViewTextRenderer* const markupRenderer = new wxDataViewTextRenderer();
#if wxUSE_MARKUP
            markupRenderer->EnableMarkup();
#endif // wxUSE_MARKUP
            m_attributes =
                new wxDataViewColumn("attributes",
                                     markupRenderer,
                                     MyListModel::Col_TextWithAttr,
                                     wxCOL_WIDTH_AUTOSIZE,
                                     wxALIGN_RIGHT,
                                     wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
            m_ctrl[1]->AppendColumn( m_attributes );

            m_ctrl[1]->AppendColumn(
                new wxDataViewColumn("custom renderer",
                                     new MyCustomRenderer(wxDATAVIEW_CELL_EDITABLE),
                                     MyListModel::Col_Custom)
            );
        }
        break;

    case 2:
        {
            wxASSERT(!m_ctrl[2]);
            wxDataViewListCtrl* lc =
                new wxDataViewListCtrl( parent, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, style );
            m_ctrl[2] = lc;

            MyListStoreDerivedModel* page2_model = new MyListStoreDerivedModel();
            lc->AssociateModel(page2_model);
            page2_model->DecRef();

            lc->AppendToggleColumn( "Toggle" );

            // We're not limited to convenience column-appending functions, it
            // can also be done fully manually, which allows us to customize
            // the renderer being used.
            wxDataViewToggleRenderer* const rendererRadio =
                new wxDataViewToggleRenderer("bool", wxDATAVIEW_CELL_ACTIVATABLE);
            rendererRadio->ShowAsRadio();
            wxDataViewColumn* const colRadio =
                new wxDataViewColumn("Radio", rendererRadio, 1);
            lc->AppendColumn(colRadio, "bool");

            lc->AppendTextColumn( "Text" );
            lc->AppendProgressColumn( "Progress" );

            wxVector<wxVariant> data;
            for (unsigned int i=0; i<10; i++)
            {
                data.clear();
                data.push_back( (i%3) == 0 );
                data.push_back( i == 7 ); // select a single (random) radio item
                data.push_back( wxString::Format("row %d", i) );
                data.push_back( long(5*i) );

                lc->AppendItem( data );
            }

            lc->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &MyFrame::OnListValueChanged, this);
        }
        break;

    case 3:
        {
            wxASSERT(!m_ctrl[3]);
            wxDataViewTreeCtrl* tc =
                new wxDataViewTreeCtrl( parent, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, style | wxDV_NO_HEADER );
            m_ctrl[3] = tc;

            wxImageList *ilist = new wxImageList( 16, 16 );
            ilist->Add( wxIcon(wx_small_xpm) );
            tc->AssignImageList( ilist );

            const wxDataViewItem root =
                tc->AppendContainer( wxDataViewItem(0), "The Root", 0 );
            tc->AppendItem( root, "Child 1", 0 );
            tc->AppendItem( root, "Child 2", 0 );
            tc->AppendItem( root, "Child 3, very long, long, long, long", 0 );

            wxDataViewItem cont =
                tc->AppendContainer( root, "Container child", 0 );
            tc->AppendItem( cont, "Child 4", 0 );
            tc->AppendItem( cont, "Child 5", 0 );

            tc->Expand(cont);
        }
        break;
    }
}


// ----------------------------------------------------------------------------
// MyFrame - generic event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnClearLog( wxCommandEvent& WXUNUSED(event) )
{
    m_log->Clear();
}

void MyFrame::OnGetPageInfo(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewCtrl* const dvc = m_ctrl[m_notebook->GetSelection()];

    const wxDataViewItem top = dvc->GetTopItem();
    wxString topDesc;
    if ( top.IsOk() )
    {
        wxVariant value;
        dvc->GetModel()->GetValue(value, top, 0);
        topDesc.Printf("Top item is \"%s\"", value.GetString());
    }
    else
    {
        topDesc = "There is no top item";
    }

    wxLogMessage("%s and there are %d items per page",
                 topDesc,
                 dvc->GetCountPerPage());
}

void MyFrame::OnDisable(wxCommandEvent& event)
{
    m_ctrl[m_notebook->GetSelection()]->Enable(!event.IsChecked());
}

void MyFrame::OnSetForegroundColour(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    wxColour col = wxGetColourFromUser(this, dvc->GetForegroundColour());
    if ( col.IsOk() )
    {
        dvc->SetForegroundColour(col);
        Refresh();
    }
}

void MyFrame::OnSetBackgroundColour(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    wxColour col = wxGetColourFromUser(this, dvc->GetBackgroundColour());
    if ( col.IsOk() )
    {
        dvc->SetBackgroundColour(col);
        Refresh();
    }
}

void MyFrame::OnCustomHeaderAttr(wxCommandEvent& event)
{
    wxItemAttr attr;
    if ( event.IsChecked() )
    {
        attr.SetTextColour(*wxRED);
        attr.SetFont(wxFontInfo(20).Bold());
    }
    //else: leave it as default to disable custom header attributes

    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    if ( !dvc->SetHeaderAttr(attr) )
        wxLogMessage("Sorry, header attributes not supported on this platform");
}

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
void MyFrame::OnCustomHeaderHeight(wxCommandEvent& event)
{
    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    wxHeaderCtrl* const header = dvc->GenericGetHeader();
    if ( !header )
    {
        wxLogMessage("No header");
        return;
    }

    // Use a big height to show that this works.
    wxSize size = event.IsChecked() ? FromDIP(wxSize(0, 80)) : wxDefaultSize;
    header->SetMinSize(size);
    header->Refresh();

    dvc->Layout();
}
#endif // wxHAS_GENERIC_DATAVIEWCTRL

void MyFrame::OnIncIndent(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    dvc->SetIndent(dvc->GetIndent() + 5);
    wxLogMessage("Indent is now %d", dvc->GetIndent());
}

void MyFrame::OnDecIndent(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewCtrl * const dvc = m_ctrl[m_notebook->GetSelection()];
    dvc->SetIndent(wxMax(dvc->GetIndent() - 5, 0));
    wxLogMessage("Indent is now %d", dvc->GetIndent());
}

void MyFrame::OnPageChanged( wxBookCtrlEvent& WXUNUSED(event) )
{
    unsigned int nPanel = m_notebook->GetSelection();

    GetMenuBar()->FindItem(ID_STYLE_MENU)->SetItemLabel(
                wxString::Format("Style of panel #%d", nPanel+1));

    for (unsigned int id = ID_MULTIPLE; id <= ID_VERT_RULES; id++)
    {
        unsigned long style = 0;
        switch (id)
        {
        /*case ID_SINGLE:
            style = wxDV_SINGLE;
            break;*/
        case ID_MULTIPLE:
            style = wxDV_MULTIPLE;
            break;
        case ID_ROW_LINES:
            style = wxDV_ROW_LINES;
            break;
        case ID_HORIZ_RULES:
            style = wxDV_HORIZ_RULES;
            break;
        case ID_VERT_RULES:
            style = wxDV_VERT_RULES;
            break;
        default:
            wxFAIL;
        }

        GetMenuBar()->FindItem(id)->Check( m_ctrl[nPanel]->HasFlag(style) );
    }

    GetMenuBar()->FindItem(ID_DISABLE)->Check(!m_ctrl[nPanel]->IsEnabled());
}

void MyFrame::OnStyleChange( wxCommandEvent& WXUNUSED(event) )
{
    unsigned int nPanel = m_notebook->GetSelection();

    // build the style
    unsigned long style = 0;
    /*if (GetMenuBar()->FindItem(ID_SINGLE)->IsChecked())
        style |= wxDV_SINGLE;*/
    if (GetMenuBar()->FindItem(ID_MULTIPLE)->IsChecked())
        style |= wxDV_MULTIPLE;
    if (GetMenuBar()->FindItem(ID_ROW_LINES)->IsChecked())
        style |= wxDV_ROW_LINES;
    if (GetMenuBar()->FindItem(ID_HORIZ_RULES)->IsChecked())
        style |= wxDV_HORIZ_RULES;
    if (GetMenuBar()->FindItem(ID_VERT_RULES)->IsChecked())
        style |= wxDV_VERT_RULES;

    wxSizer* sz = m_ctrl[nPanel]->GetContainingSizer();
    wxASSERT(sz);

    sz->Detach(m_ctrl[nPanel]);
    wxDELETE(m_ctrl[nPanel]);
    m_ctrl[nPanel] = NULL;

    if (nPanel == 0)
        m_music_model.reset(NULL);
    else if (nPanel == 1)
        m_list_model.reset(NULL);

    // rebuild the DVC for the selected panel:
    BuildDataViewCtrl((wxPanel*)m_notebook->GetPage(nPanel), nPanel, style);

    sz->Prepend(m_ctrl[nPanel], 1, wxGROW|wxALL, 5);
    sz->Layout();
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates wxDataViewCtrl"));
    info.SetCopyright("(C) 2007-2009 Robert Roebling");
    info.AddDeveloper("Robert Roebling");
    info.AddDeveloper("Francesco Montorsi");

    wxAboutBox(info, this);
}


// ----------------------------------------------------------------------------
// MyFrame - event handlers for the first page
// ----------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

void MyFrame::OnBeginDrag( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drags for item, not containers
    if (m_music_model->IsContainer( item ) )
    {
        event.Veto();
        return;
    }

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    wxTextDataObject *obj = new wxTextDataObject;
    obj->SetText( node->m_title );
    event.SetDataObject( obj );
    event.SetDragFlags(wxDrag_AllowMove); // allows both copy and move
}

void MyFrame::OnDropPossible( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drags for item or background, not containers
    if ( item.IsOk() && m_music_model->IsContainer( item ) )
        event.Veto();

    if (event.GetDataFormat() != wxDF_UNICODETEXT)
        event.Veto();
}

void MyFrame::OnDrop( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drops for item, not containers
    if ( item.IsOk() && m_music_model->IsContainer( item ) )
    {
        event.Veto();
        return;
    }

    if (event.GetDataFormat() != wxDF_UNICODETEXT)
    {
        event.Veto();
        return;
    }

    wxTextDataObject obj;
    obj.SetData( wxDF_UNICODETEXT, event.GetDataSize(), event.GetDataBuffer() );

    if ( item.IsOk() )
        wxLogMessage( "Text dropped on item %s: %s", m_music_model->GetTitle( item ), obj.GetText() );
    else
        wxLogMessage( "Text dropped on background: %s", obj.GetText() );
}

#endif // wxUSE_DRAG_AND_DROP

void MyFrame::OnAddMozart( wxCommandEvent& WXUNUSED(event) )
{
    m_music_model->AddToClassical( "Eine kleine Nachtmusik", "Wolfgang Mozart", 1787 );
}

void MyFrame::DeleteSelectedItems()
{
    wxDataViewItemArray items;
    int len = m_ctrl[0]->GetSelections( items );
    for( int i = 0; i < len; i ++ )
        if (items[i].IsOk())
            m_music_model->Delete( items[i] );
}

void MyFrame::OnDeleteSelected( wxCommandEvent& WXUNUSED(event) )
{
    DeleteSelectedItems();
}

void MyFrame::OnDeleteYear( wxCommandEvent& WXUNUSED(event) )
{
    m_ctrl[0]->DeleteColumn( m_ctrl[0]->GetColumn( 2 ) );
    FindWindow( ID_DELETE_YEAR )->Disable();
}

void MyFrame::OnSelectNinth( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_music_model->GetNinthItem().IsOk())
    {
        wxLogError( "Cannot select the ninth symphony: it was removed!" );
        return;
    }

    m_ctrl[0]->Select( m_music_model->GetNinthItem() );
}

void MyFrame::OnCollapse( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItem item = m_ctrl[0]->GetSelection();
    if (item.IsOk())
        m_ctrl[0]->Collapse( item );
}

void MyFrame::OnExpand( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItem item = m_ctrl[0]->GetSelection();
    if (item.IsOk())
        m_ctrl[0]->Expand( item );
}

void MyFrame::OnShowCurrent(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item = m_ctrl[0]->GetCurrentItem();
    if ( item.IsOk() )
    {
        wxLogMessage("Current item: \"%s\" by %s",
                     m_music_model->GetTitle(item),
                     m_music_model->GetArtist(item));
    }
    else
    {
        wxLogMessage("There is no current item.");
    }

    wxDataViewColumn *col = m_ctrl[0]->GetCurrentColumn();
    if ( col )
    {
        wxLogMessage("Current column: %d",
                     m_ctrl[0]->GetColumnPosition(col));
    }
    else
    {
        wxLogMessage("There is no current column.");
    }
}

void MyFrame::OnSetNinthCurrent(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item(m_music_model->GetNinthItem());
    if ( !item.IsOk() )
    {
        wxLogError( "Cannot make the ninth symphony current: it was removed!" );
        return;
    }

    m_ctrl[0]->SetCurrentItem(item);
}

void MyFrame::OnChangeNinthTitle(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item(m_music_model->GetNinthItem());
    if ( !item.IsOk() )
    {
        wxLogError( "Cannot change the ninth symphony title: it was removed!" );
        return;
    }

    m_music_model->SetValue("Symphony No. 9", item, 0);
    m_music_model->ItemChanged(item);
}

void MyFrame::OnValueChanged( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, Item Id: %s;  Column: %d",
                  title, event.GetColumn() );
}

void MyFrame::OnActivated( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_ACTIVATED, Item: %s", title );

    if (m_ctrl[0]->IsExpanded( event.GetItem() ))
    {
        wxLogMessage( "Item: %s is expanded", title );
    }
}

void MyFrame::OnSelectionChanged( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    if (title.empty())
        title = "None";

    wxLogMessage( "wxEVT_DATAVIEW_SELECTION_CHANGED, First selected Item: %s", title );
}

void MyFrame::OnExpanding( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_EXPANDING, Item: %s", title );
}


void MyFrame::OnStartEditing( wxDataViewEvent &event )
{
    wxString artist = m_music_model->GetArtist( event.GetItem() );
    if (artist == "Ludwig van Beethoven")
    {
        event.Veto();

        wxLogMessage( "wxEVT_DATAVIEW_ITEM_START_EDITING vetoed. Artist: %s", artist );
    }
    else
        wxLogMessage( "wxEVT_DATAVIEW_ITEM_START_EDITING not vetoed. Artist: %s", artist );

}

void MyFrame::OnEditingStarted( wxDataViewEvent &event )
{
    // This event doesn't, currently, carry the value, so get it ourselves.
    wxDataViewModel* const model = event.GetModel();
    wxVariant value;
    model->GetValue(value, event.GetItem(), event.GetColumn());
    wxLogMessage("wxEVT_DATAVIEW_ITEM_EDITING_STARTED, current value %s",
                 value.GetString());
}

void MyFrame::OnEditingDone( wxDataViewEvent &event )
{
    wxLogMessage("wxEVT_DATAVIEW_ITEM_EDITING_DONE, new value %s",
                 event.IsEditCancelled()
                    ? wxString("unavailable because editing was cancelled")
                    : event.GetValue().GetString());
}

void MyFrame::OnExpanded( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_EXPANDED, Item: %s", title );
}

void MyFrame::OnCollapsing( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_COLLAPSING, Item: %s", title );
}

void MyFrame::OnCollapsed( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_COLLAPSED, Item: %s", title );
}

void MyFrame::OnContextMenu( wxDataViewEvent &event )
{
    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage( "wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, Item: %s", title );

    wxMenu menu;
    menu.Append( 1, "menuitem 1" );
    menu.Append( 2, "menuitem 2" );
    menu.Append( 3, "menuitem 3" );

    m_ctrl[0]->PopupMenu(&menu);
}

void MyFrame::OnAttrHeaderClick( wxDataViewEvent &event )
{
    // we need to skip the event to let the default behaviour of sorting by
    // this column when it is clicked to take place
    event.Skip();

    int pos = m_ctrl[1]->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage( "wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, Column position: %d", pos );
    wxLogMessage( "Column title: %s  Column width: %d", event.GetDataViewColumn()->GetTitle(), event.GetDataViewColumn()->GetWidth() );
}

void MyFrame::OnHeaderClick( wxDataViewEvent &event )
{
    // we need to skip the event to let the default behaviour of sorting by
    // this column when it is clicked to take place
    event.Skip();

    int pos = m_ctrl[0]->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage( "wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, Column position: %d", pos );
    wxLogMessage( "Column width: %d", event.GetDataViewColumn()->GetWidth() );
}

void MyFrame::OnHeaderRightClick( wxDataViewEvent &event )
{
    int pos = m_ctrl[0]->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage( "wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, Column position: %d", pos );
}

void MyFrame::OnColumnReordered(wxDataViewEvent& event)
{
    wxDataViewColumn* const col = event.GetDataViewColumn();
    if ( !col )
    {
        wxLogError("Unknown column reordered?");
        return;
    }

    wxLogMessage("wxEVT_DATAVIEW_COLUMN_REORDERED: \"%s\" is now at position %d",
                 col->GetTitle(), event.GetColumn());
}

void MyFrame::OnSortedList( wxDataViewEvent &/*event*/)
{
    wxVector<wxDataViewColumn *> const columns = m_ctrl[1]->GetSortingColumns();
    wxLogMessage( "wxEVT_DATAVIEW_COLUMN_SORTED using the following columns");

    for ( wxVector<wxDataViewColumn *>::const_iterator it = columns.begin(),
                                                      end = columns.end();
          it != end;
          ++it )
    {
        wxDataViewColumn* const col = *it;

        wxLogMessage("\t%d. %s %s",
                     col->GetModelColumn(),
                     col->GetTitle(),
                     col->IsSortOrderAscending() ? "ascending" : "descending");
    }
}

void MyFrame::OnHeaderClickList( wxDataViewEvent &event )
{
    // Use control+click to toggle sorting by this column.
    if ( wxGetKeyState(WXK_CONTROL) )
        m_ctrl[1]->ToggleSortByColumn(event.GetColumn());
    else
        event.Skip();
}

void MyFrame::OnSorted( wxDataViewEvent &event )
{
    int pos = m_ctrl[0]->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage( "wxEVT_DATAVIEW_COLUMN_SORTED, Column position: %d", pos );
}

void MyFrame::OnDataViewChar(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_DELETE )
        DeleteSelectedItems();
    else
        event.Skip();
}

// ----------------------------------------------------------------------------
// MyFrame - event handlers for the second page
// ----------------------------------------------------------------------------

void MyFrame::OnPrependList( wxCommandEvent& WXUNUSED(event) )
{
    m_list_model->Prepend("Test");
}

void MyFrame::OnDeleteList( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_ctrl[1]->GetSelections( items );
    if (len > 0)
        m_list_model->DeleteItems( items );
}

void MyFrame::OnGoto(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item = m_list_model->GetItem( 50 );
    m_ctrl[1]->EnsureVisible(item,m_col);
}

void MyFrame::OnAddMany(wxCommandEvent& WXUNUSED(event))
{
    m_list_model->AddMany();
}

void MyFrame::OnHideAttributes(wxCommandEvent& WXUNUSED(event))
{
    m_attributes->SetHidden(true);
}

void MyFrame::OnShowAttributes(wxCommandEvent& WXUNUSED(event))
{
    m_attributes->SetHidden(false);
}

// ----------------------------------------------------------------------------
// MyFrame - event handlers for the third (wxDataViewListCtrl) page
// ----------------------------------------------------------------------------

void MyFrame::OnListValueChanged(wxDataViewEvent& event)
{
    // Ignore changes coming from our own SetToggleValue() calls below.
    if ( m_eventFromProgram )
    {
        m_eventFromProgram = false;
        return;
    }

    wxDataViewListCtrl* const lc = static_cast<wxDataViewListCtrl*>(m_ctrl[2]);

    const int columnToggle = 1;

    // Handle selecting a radio button by unselecting all the other ones.
    if ( event.GetColumn() == columnToggle )
    {
        const int rowChanged = lc->ItemToRow(event.GetItem());
        if ( lc->GetToggleValue(rowChanged, columnToggle) )
        {
            for ( int row = 0; row < lc->GetItemCount(); ++row )
            {
                if ( row != rowChanged )
                {
                    m_eventFromProgram = true;
                    lc->SetToggleValue(false, row, columnToggle);
                }
            }
        }
        else // The item was cleared.
        {
            // Explicitly check it back, we want to always have exactly one
            // checked radio item in this column.
            m_eventFromProgram = true;
            lc->SetToggleValue(true, rowChanged, columnToggle);
        }
    }
}

// ----------------------------------------------------------------------------
// MyFrame - event handlers for the fourth page
// ----------------------------------------------------------------------------

void MyFrame::OnDeleteTreeItem(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewTreeCtrl* ctrl = (wxDataViewTreeCtrl*) m_ctrl[3];
    wxDataViewItem selected = ctrl->GetSelection();
    if (!selected.IsOk())
        return;

    ctrl->DeleteItem(selected);
}

void MyFrame::OnDeleteAllTreeItems(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewTreeCtrl* ctrl = (wxDataViewTreeCtrl*) m_ctrl[3];
    ctrl->DeleteAllItems();
}

void MyFrame::OnAddTreeItem(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewTreeCtrl* ctrl = (wxDataViewTreeCtrl*) m_ctrl[3];
    wxDataViewItem selected = ctrl->GetSelection();
    if (ctrl->IsContainer(selected)) {
        wxDataViewItem newitem = ctrl->AppendItem( selected, "Item", 0 );
        ctrl->Select(newitem);
        ctrl->EditItem(newitem, ctrl->GetColumn(0));
    }
}

void MyFrame::OnAddTreeContainerItem(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewTreeCtrl* ctrl = (wxDataViewTreeCtrl*) m_ctrl[3];
    wxDataViewItem selected = ctrl->GetSelection();
    if (ctrl->IsContainer(selected))
        ctrl->AppendContainer(selected, "Container", 0 );
}

void MyFrame::OnMultipleSort( wxCommandEvent &event )
{
    if ( !m_ctrl[1]->AllowMultiColumnSort(event.IsChecked()) )
        wxLogMessage("Sorting by multiple columns not supported");
}

void MyFrame::OnSortByFirstColumn(wxCommandEvent& event)
{
    wxDataViewColumn* const col = m_ctrl[1]->GetColumn(0);
    if ( event.IsChecked() )
        col->SetSortOrder(true /* ascending */);
    else
        col->UnsetAsSortKey();
}
