/////////////////////////////////////////////////////////////////////////////
// Name:        treetest.cpp
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "wx/colordlg.h"
#include "wx/numdlg.h"

#include "wx/artprov.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/math.h"
#include "wx/renderer.h"

#ifdef __WIN32__
    // this is not supported by native control
    #define NO_VARIABLE_HEIGHT
#endif

#include "treetest.h"

#include "icon1.xpm"
#include "icon2.xpm"
#include "icon3.xpm"
#include "icon4.xpm"
#include "icon5.xpm"

#include "state1.xpm"
#include "state2.xpm"
#include "state3.xpm"
#include "state4.xpm"
#include "state5.xpm"

#include "unchecked.xpm"
#include "checked.xpm"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

static const int NUM_CHILDREN_PER_LEVEL = 5;
static const int NUM_LEVELS = 2;

// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM( item ) if ( !item.IsOk() ) {                                 \
                             wxMessageBox(wxT("Please select some item first!"), \
                                          wxT("Tree sample error"),              \
                                          wxOK | wxICON_EXCLAMATION,             \
                                          this);                                 \
                             return;                                             \
                           }

#define MENU_LINK(name) EVT_MENU(TreeTest_##name, MyFrame::On##name)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_IDLE(MyFrame::OnIdle)
    EVT_SIZE(MyFrame::OnSize)

    MENU_LINK(Quit)
    MENU_LINK(About)
    MENU_LINK(ClearLog)

    MENU_LINK(TogButtons)
    MENU_LINK(TogTwist)
    MENU_LINK(TogLines)
    MENU_LINK(TogEdit)
    MENU_LINK(TogHideRoot)
    MENU_LINK(TogRootLines)
    MENU_LINK(TogBorder)
    MENU_LINK(TogFullHighlight)
    MENU_LINK(SetFgColour)
    MENU_LINK(SetBgColour)
    MENU_LINK(ResetStyle)

    MENU_LINK(Highlight)
    MENU_LINK(Dump)
#ifndef NO_MULTIPLE_SELECTION
    MENU_LINK(DumpSelected)
    MENU_LINK(Select)
    MENU_LINK(Unselect)
    MENU_LINK(ToggleSel)
    MENU_LINK(SelectChildren)
#endif // NO_MULTIPLE_SELECTION
    MENU_LINK(Rename)
    MENU_LINK(Count)
    MENU_LINK(CountRec)
    MENU_LINK(Sort)
    MENU_LINK(SortRev)
    MENU_LINK(SetBold)
    MENU_LINK(ClearBold)
    MENU_LINK(Delete)
    MENU_LINK(DeleteChildren)
    MENU_LINK(DeleteAll)
    MENU_LINK(Recreate)
    MENU_LINK(ToggleImages)
    MENU_LINK(ToggleStates)
    MENU_LINK(ToggleAlternateImages)
    MENU_LINK(ToggleAlternateStates)
    MENU_LINK(ToggleButtons)
    MENU_LINK(SetImageSize)
    MENU_LINK(CollapseAndReset)
    MENU_LINK(EnsureVisible)
    MENU_LINK(SetFocus)
    MENU_LINK(AddItem)
    MENU_LINK(InsertItem)
    MENU_LINK(IncIndent)
    MENU_LINK(DecIndent)
    MENU_LINK(IncSpacing)
    MENU_LINK(DecSpacing)
    MENU_LINK(ToggleIcon)
    MENU_LINK(ToggleState)
    MENU_LINK(SelectRoot)
    MENU_LINK(SetFocusedRoot)
    MENU_LINK(ClearFocused)

    MENU_LINK(ShowFirstVisible)
#ifdef wxHAS_LAST_VISIBLE
    MENU_LINK(ShowLastVisible)
#endif // wxHAS_LAST_VISIBLE
    MENU_LINK(ShowNextVisible)
    MENU_LINK(ShowPrevVisible)
    MENU_LINK(ShowParent)
    MENU_LINK(ShowPrevSibling)
    MENU_LINK(ShowNextSibling)
    MENU_LINK(ScrollTo)
    MENU_LINK(SelectLast)
#undef MENU_LINK

END_EVENT_TABLE()

#if USE_GENERIC_TREECTRL
BEGIN_EVENT_TABLE(MyTreeCtrl, wxGenericTreeCtrl)
#else
BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
#endif
    EVT_TREE_BEGIN_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginRDrag)
    EVT_TREE_END_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnEndDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(TreeTest_Ctrl, MyTreeCtrl::OnDeleteItem)
#if 0       // there are so many of those that logging them causes flicker
    EVT_TREE_GET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnGetInfo)
#endif
    EVT_TREE_SET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnSetInfo)
    EVT_TREE_ITEM_EXPANDED(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanded)
    EVT_TREE_ITEM_EXPANDING(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanding)
    EVT_TREE_ITEM_COLLAPSED(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsed)
    EVT_TREE_ITEM_COLLAPSING(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsing)

    EVT_TREE_SEL_CHANGED(TreeTest_Ctrl, MyTreeCtrl::OnSelChanged)
    EVT_TREE_SEL_CHANGING(TreeTest_Ctrl, MyTreeCtrl::OnSelChanging)
    EVT_TREE_KEY_DOWN(TreeTest_Ctrl, MyTreeCtrl::OnTreeKeyDown)
    EVT_TREE_ITEM_ACTIVATED(TreeTest_Ctrl, MyTreeCtrl::OnItemActivated)
    EVT_TREE_STATE_IMAGE_CLICK(TreeTest_Ctrl, MyTreeCtrl::OnItemStateClick)

    // so many different ways to handle right mouse button clicks...
    EVT_CONTEXT_MENU(MyTreeCtrl::OnContextMenu)
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(TreeTest_Ctrl, MyTreeCtrl::OnItemMenu)
    EVT_TREE_ITEM_RIGHT_CLICK(TreeTest_Ctrl, MyTreeCtrl::OnItemRClick)

    EVT_RIGHT_DOWN(MyTreeCtrl::OnRMouseDown)
    EVT_RIGHT_UP(MyTreeCtrl::OnRMouseUp)
    EVT_RIGHT_DCLICK(MyTreeCtrl::OnRMouseDClick)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame(wxT("wxTreeCtrl Test"), 50, 50, 450, 600);

    // Show the frame
    frame->Show(true);

    return true;
}


// My frame constructor
MyFrame::MyFrame(const wxString& title, int x, int y, int w, int h)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, wxPoint(x, y), wxSize(w, h)),
         m_treeCtrl(NULL)
#if wxUSE_LOG
         , m_textCtrl(NULL)
#endif // wxUSE_LOG
{
    // This reduces flicker effects - even better would be to define
    // OnEraseBackground to do nothing. When the tree control's scrollbars are
    // show or hidden, the frame is sent a background erase event.
    SetBackgroundColour(wxColour(255, 255, 255));

    // Give it an icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // Make a menubar
    wxMenu *file_menu = new wxMenu,
           *style_menu = new wxMenu,
           *tree_menu = new wxMenu,
           *item_menu = new wxMenu;

    file_menu->Append(TreeTest_ClearLog, wxT("&Clear log\tCtrl-L"));
    file_menu->AppendSeparator();
    file_menu->Append(TreeTest_About, wxT("&About..."));
    file_menu->AppendSeparator();
    file_menu->Append(TreeTest_Quit, wxT("E&xit\tAlt-X"));

    style_menu->AppendCheckItem(TreeTest_TogButtons, wxT("Toggle &normal buttons"));
    style_menu->AppendCheckItem(TreeTest_TogTwist, wxT("Toggle &twister buttons"));
    style_menu->AppendCheckItem(TreeTest_ToggleButtons, wxT("Toggle image &buttons"));
    style_menu->AppendSeparator();
    style_menu->AppendCheckItem(TreeTest_TogLines, wxT("Toggle &connecting lines"));
    style_menu->AppendCheckItem(TreeTest_TogRootLines, wxT("Toggle &lines at root"));
    style_menu->AppendCheckItem(TreeTest_TogHideRoot, wxT("Toggle &hidden root"));
    style_menu->AppendCheckItem(TreeTest_TogBorder, wxT("Toggle &item border"));
    style_menu->AppendCheckItem(TreeTest_TogFullHighlight, wxT("Toggle &full row highlight"));
    style_menu->AppendCheckItem(TreeTest_TogEdit, wxT("Toggle &edit mode"));
#ifndef NO_MULTIPLE_SELECTION
    style_menu->AppendCheckItem(TreeTest_ToggleSel, wxT("Toggle &selection mode\tCtrl-S"));
#endif // NO_MULTIPLE_SELECTION
    style_menu->AppendCheckItem(TreeTest_ToggleImages, wxT("Toggle show ima&ges"));
    style_menu->AppendCheckItem(TreeTest_ToggleStates, wxT("Toggle show st&ates"));
    style_menu->AppendCheckItem(TreeTest_ToggleAlternateImages, wxT("Toggle alternate images"));
    style_menu->AppendCheckItem(TreeTest_ToggleAlternateStates, wxT("Toggle alternate state images"));
    style_menu->Append(TreeTest_SetImageSize, wxT("Set image si&ze..."));
    style_menu->AppendSeparator();
    style_menu->Append(TreeTest_SetFgColour, wxT("Set &foreground colour..."));
    style_menu->Append(TreeTest_SetBgColour, wxT("Set &background colour..."));
    style_menu->AppendSeparator();
    style_menu->Append(TreeTest_ResetStyle, wxT("&Reset to default\tF10"));

    tree_menu->Append(TreeTest_Recreate, wxT("&Recreate the tree"));
    tree_menu->Append(TreeTest_CollapseAndReset, wxT("C&ollapse and reset"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_AddItem, wxT("Append a &new item"));
    tree_menu->Append(TreeTest_InsertItem, wxT("&Insert a new item"));
    tree_menu->Append(TreeTest_Delete, wxT("&Delete this item"));
    tree_menu->Append(TreeTest_DeleteChildren, wxT("Delete &children"));
    tree_menu->Append(TreeTest_DeleteAll, wxT("Delete &all items"));
    tree_menu->Append(TreeTest_SelectRoot, wxT("Select root item"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_SetFocusedRoot, wxT("Set focus to root item"));
    tree_menu->Append(TreeTest_ClearFocused, wxT("Reset focus"));

    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_Count, wxT("Count children of current item"));
    tree_menu->Append(TreeTest_CountRec, wxT("Recursively count children of current item"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_Sort, wxT("Sort children of current item"));
    tree_menu->Append(TreeTest_SortRev, wxT("Sort in reversed order"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_EnsureVisible, wxT("Make the last item &visible"));
    tree_menu->Append(TreeTest_SetFocus, wxT("Set &focus to the tree"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_IncIndent, wxT("Add 5 points to indentation\tAlt-I"));
    tree_menu->Append(TreeTest_DecIndent, wxT("Reduce indentation by 5 points\tAlt-R"));
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_IncSpacing, wxT("Add 5 points to spacing\tCtrl-I"));
    tree_menu->Append(TreeTest_DecSpacing, wxT("Reduce spacing by 5 points\tCtrl-R"));

    item_menu->Append(TreeTest_Dump, wxT("&Dump item children"));
    item_menu->Append(TreeTest_Rename, wxT("&Rename item..."));

    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_SetBold, wxT("Make item &bold"));
    item_menu->Append(TreeTest_ClearBold, wxT("Make item &not bold"));
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ToggleIcon, wxT("Toggle the item's &icon"));
    item_menu->Append(TreeTest_ToggleState, wxT("Toggle the item's &state"));
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ShowFirstVisible, wxT("Show &first visible"));
#ifdef wxHAS_LAST_VISIBLE
    item_menu->Append(TreeTest_ShowLastVisible, wxT("Show &last visible"));
#endif // wxHAS_LAST_VISIBLE
    item_menu->Append(TreeTest_ShowNextVisible, wxT("Show &next visible"));
    item_menu->Append(TreeTest_ShowPrevVisible, wxT("Show &previous visible"));
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ShowParent, "Show pa&rent");
    item_menu->Append(TreeTest_ShowPrevSibling, "Show &previous sibling");
    item_menu->Append(TreeTest_ShowNextSibling, "Show &next sibling");
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ScrollTo, "Scroll &to item",
                      "Scroll to the last by one top level child");
    item_menu->Append(TreeTest_SelectLast, "Select &last item",
                      "Select the last item");

#ifndef NO_MULTIPLE_SELECTION
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_DumpSelected, wxT("Dump selected items\tAlt-D"));
    item_menu->Append(TreeTest_Select, wxT("Select current item\tAlt-S"));
    item_menu->Append(TreeTest_Unselect, wxT("Unselect everything\tAlt-U"));
    item_menu->Append(TreeTest_SelectChildren, wxT("Select all children\tCtrl-A"));
#endif // NO_MULTIPLE_SELECTION

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(style_menu, wxT("&Style"));
    menu_bar->Append(tree_menu, wxT("&Tree"));
    menu_bar->Append(item_menu, wxT("&Item"));
    SetMenuBar(menu_bar);
#endif // wxUSE_MENUS

    m_panel = new wxPanel(this);

#if wxUSE_LOG
    // create the controls
    m_textCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxT(""),
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxSUNKEN_BORDER);
#endif // wxUSE_LOG

    CreateTreeWithDefStyle();

    menu_bar->Check(TreeTest_ToggleImages, true);
    menu_bar->Check(TreeTest_ToggleStates, true);
    menu_bar->Check(TreeTest_ToggleAlternateImages, false);
    menu_bar->Check(TreeTest_ToggleAlternateStates, false);

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

#if wxUSE_LOG
#ifdef __WXMOTIF__
    // For some reason, we get a memcpy crash in wxLogStream::DoLogStream
    // on gcc/wxMotif, if we use wxLogTextCtl. Maybe it's just gcc?
    delete wxLog::SetActiveTarget(new wxLogStderr);
#else
    // set our text control as the log target
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(m_textCtrl);
    delete wxLog::SetActiveTarget(logWindow);
#endif
#endif // wxUSE_LOG
}

MyFrame::~MyFrame()
{
#if wxUSE_LOG
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
}

void MyFrame::CreateTreeWithDefStyle()
{
    long style = wxTR_DEFAULT_STYLE |
#ifndef NO_VARIABLE_HEIGHT
                 wxTR_HAS_VARIABLE_ROW_HEIGHT |
#endif
                 wxTR_EDIT_LABELS;

    CreateTree(style | wxSUNKEN_BORDER);

    // as we don't know what wxTR_DEFAULT_STYLE could contain, test for
    // everything
    wxMenuBar *mbar = GetMenuBar();
    mbar->Check(TreeTest_TogButtons, (style & wxTR_HAS_BUTTONS) != 0);
    mbar->Check(TreeTest_TogButtons, (style & wxTR_TWIST_BUTTONS) != 0);
    mbar->Check(TreeTest_TogLines, (style & wxTR_NO_LINES) == 0);
    mbar->Check(TreeTest_TogRootLines, (style & wxTR_LINES_AT_ROOT) != 0);
    mbar->Check(TreeTest_TogHideRoot, (style & wxTR_HIDE_ROOT) != 0);
    mbar->Check(TreeTest_TogEdit, (style & wxTR_EDIT_LABELS) != 0);
    mbar->Check(TreeTest_TogBorder, (style & wxTR_ROW_LINES) != 0);
    mbar->Check(TreeTest_TogFullHighlight, (style & wxTR_FULL_ROW_HIGHLIGHT) != 0);
}

void MyFrame::CreateTree(long style)
{
    m_treeCtrl = new MyTreeCtrl(m_panel, TreeTest_Ctrl,
                                wxDefaultPosition, wxDefaultSize,
                                style);

    GetMenuBar()->Enable(TreeTest_SelectRoot, !(style & wxTR_HIDE_ROOT));

    Resize();
}

void MyFrame::TogStyle(int id, long flag)
{
    long style = m_treeCtrl->GetWindowStyle() ^ flag;

    // most treectrl styles can't be changed on the fly using the native
    // control and the tree must be recreated
#ifndef __WXMSW__
    m_treeCtrl->SetWindowStyle(style);
#else // MSW
    delete m_treeCtrl;
    CreateTree(style);
#endif // !MSW/MSW

    GetMenuBar()->Check(id, (style & flag) != 0);
}

void MyFrame::OnIdle(wxIdleEvent& event)
{
#if wxUSE_STATUSBAR
    if ( m_treeCtrl )
    {
        wxTreeItemId idRoot = m_treeCtrl->GetRootItem();
        wxString status;
        if (idRoot.IsOk())
        {
            wxTreeItemId idLast = m_treeCtrl->GetLastChild(idRoot);
            status = wxString::Format(
                wxT("Root/last item is %svisible/%svisible"),
                m_treeCtrl->IsVisible(idRoot) ? wxT("") : wxT("not "),
                idLast.IsOk() && m_treeCtrl->IsVisible(idLast)
                    ? wxT("") : wxT("not "));
        }
        else
            status = wxT("No root item");

        SetStatusText(status, 1);
    }
#endif // wxUSE_STATUSBAR

    event.Skip();
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    if ( m_treeCtrl
#if wxUSE_LOG
                    && m_textCtrl
#endif // wxUSE_LOG
                                  )
    {
        Resize();
    }

    event.Skip();
}

void MyFrame::Resize()
{
    wxSize size = GetClientSize();
    m_treeCtrl->SetSize(0, 0, size.x, size.y
#if wxUSE_LOG
                                            *2/3);
    m_textCtrl->SetSize(0, 2*size.y/3, size.x, size.y/3
#endif
    );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Tree test sample\n")
                 wxT("(c) Julian Smart 1997, Vadim Zeitlin 1998"),
                 wxT("About tree test"),
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_textCtrl->Clear();
}

void MyFrame::OnRename(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    // old code - now we edit in place
#if 0
    static wxString s_text;
    s_text = wxGetTextFromUser(wxT("New name: "), wxT("Tree sample question"),
            s_text, this);
    if ( !s_text.empty() )
    {
        m_treeCtrl->SetItemText(item, s_text);
    }
#endif // 0

    // TODO demonstrate creating a custom edit control...
    (void)m_treeCtrl->EditLabel(item);
}

void MyFrame::OnCount(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    int i = m_treeCtrl->GetChildrenCount( item, false );

    wxLogMessage(wxT("%d children"), i);
}

void MyFrame::OnCountRec(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    int i = m_treeCtrl->GetChildrenCount( item );

    wxLogMessage(wxT("%d children"), i);
}

void MyFrame::DoSort(bool reverse)
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->DoSortChildren(item, reverse);
}

void MyFrame::OnHighlight(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId id = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( id );

    wxRect r;
    if ( !m_treeCtrl->GetBoundingRect(id, r, true /* text, not full row */) )
    {
        wxLogMessage(wxT("Failed to get bounding item rect"));
        return;
    }

    wxClientDC dc(m_treeCtrl);
    dc.SetBrush(*wxRED);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(r);
    m_treeCtrl->Update();
}

void MyFrame::OnDump(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId root = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( root );

    m_treeCtrl->GetItemsRecursively(root);
}

#ifndef NO_MULTIPLE_SELECTION

void MyFrame::OnToggleSel(wxCommandEvent& event)
{
    TogStyle(event.GetId(), wxTR_MULTIPLE);
}

void MyFrame::OnDumpSelected(wxCommandEvent& WXUNUSED(event))
{
    wxArrayTreeItemIds array;

    size_t count = m_treeCtrl->GetSelections(array);
    wxLogMessage(wxT("%u items selected"), unsigned(count));

    for ( size_t n = 0; n < count; n++ )
    {
        wxLogMessage(wxT("\t%s"), m_treeCtrl->GetItemText(array.Item(n)).c_str());
    }
}

void MyFrame::OnSelect(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->SelectItem(m_treeCtrl->GetFocusedItem());
}

void MyFrame::OnSelectRoot(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_treeCtrl->HasFlag(wxTR_HIDE_ROOT) )
        m_treeCtrl->SelectItem(m_treeCtrl->GetRootItem());
}

void MyFrame::OnSetFocusedRoot(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_treeCtrl->HasFlag(wxTR_HIDE_ROOT) )
        m_treeCtrl->SetFocusedItem(m_treeCtrl->GetRootItem());
}

void MyFrame::OnClearFocused(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->ClearFocusedItem();
}

void MyFrame::OnUnselect(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->UnselectAll();
}

void MyFrame::OnSelectChildren(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();
    if ( !item.IsOk() )
        item = m_treeCtrl->GetRootItem();

    m_treeCtrl->SelectChildren(item);
}

#endif // NO_MULTIPLE_SELECTION

void MyFrame::DoSetBold(bool bold)
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->SetItemBold(item, bold);
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->Delete(item);
}

void MyFrame::OnDeleteChildren(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->DeleteChildren(item);
}

void MyFrame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->DeleteAllItems();
}

void MyFrame::OnRecreate(wxCommandEvent& event)
{
    OnDeleteAll(event);
    m_treeCtrl->AddTestItemsToTree(NUM_CHILDREN_PER_LEVEL, NUM_LEVELS);
}

void MyFrame::OnSetImageSize(wxCommandEvent& WXUNUSED(event))
{
    int size = wxGetNumberFromUser(wxT("Enter the size for the images to use"),
                                    wxT("Size: "),
                                    wxT("TreeCtrl sample"),
                                    m_treeCtrl->ImageSize());
    if ( size == -1 )
        return;

    m_treeCtrl->CreateImageList(size);
    wxGetApp().SetShowImages(true);
}

void MyFrame::OnToggleImages(wxCommandEvent& WXUNUSED(event))
{
    if ( wxGetApp().ShowImages() )
    {
        m_treeCtrl->CreateImageList(-1);
        wxGetApp().SetShowImages(false);
    }
    else
    {
        m_treeCtrl->CreateImageList(0);
        wxGetApp().SetShowImages(true);
    }
}

void MyFrame::OnToggleStates(wxCommandEvent& WXUNUSED(event))
{
    if ( wxGetApp().ShowStates() )
    {
        m_treeCtrl->CreateStateImageList(true);
        wxGetApp().SetShowStates(false);
    }
    else
    {
        m_treeCtrl->CreateStateImageList(false);
        wxGetApp().SetShowStates(true);
    }
}

void MyFrame::OnToggleAlternateImages(wxCommandEvent& WXUNUSED(event))
{
    bool alternateImages = m_treeCtrl->AlternateImages();

    m_treeCtrl->SetAlternateImages(!alternateImages);
    m_treeCtrl->CreateImageList(0);
}

void MyFrame::OnToggleAlternateStates(wxCommandEvent& WXUNUSED(event))
{
    bool alternateStates = m_treeCtrl->AlternateStates();

    m_treeCtrl->SetAlternateStates(!alternateStates);
    m_treeCtrl->CreateStateImageList();

    // normal states < alternate states
    // so we must reset broken states
    if ( alternateStates )
        m_treeCtrl->ResetBrokenStateImages();
}

void MyFrame::OnToggleButtons(wxCommandEvent& WXUNUSED(event))
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if ( wxGetApp().ShowButtons() )
    {
        m_treeCtrl->CreateButtonsImageList(-1);
        wxGetApp().SetShowButtons(false);
    }
    else
    {
        m_treeCtrl->CreateButtonsImageList(15);
        wxGetApp().SetShowButtons(true);
    }
#endif
}

void MyFrame::OnCollapseAndReset(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->CollapseAndReset(m_treeCtrl->GetRootItem());
}

void MyFrame::OnEnsureVisible(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->DoEnsureVisible();
}

void MyFrame::OnSetFocus(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->SetFocus();
}

void MyFrame::OnInsertItem(wxCommandEvent& WXUNUSED(event))
{
    int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_File : -1;
    m_treeCtrl->InsertItem(m_treeCtrl->GetRootItem(), image, wxT("2nd item"));
}

void MyFrame::OnAddItem(wxCommandEvent& WXUNUSED(event))
{
    static int s_num = 0;

    wxString text;
    text.Printf(wxT("Item #%d"), ++s_num);

    m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(),
                           text /*,
                           MyTreeCtrl::TreeCtrlIcon_File */ );
}

void MyFrame::OnIncIndent(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetIndent();
    if (indent < 100)
        m_treeCtrl->SetIndent( indent+5 );
}

void MyFrame::OnDecIndent(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetIndent();
    if (indent > 10)
        m_treeCtrl->SetIndent( indent-5 );
}

void MyFrame::OnIncSpacing(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetSpacing();
    if (indent < 100)
    {
        m_treeCtrl->SetSpacing( indent+5 );
        m_treeCtrl->Refresh();
    }
}

void MyFrame::OnDecSpacing(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetSpacing();
    if (indent > 10)
    {
        m_treeCtrl->SetSpacing( indent-5 );
        m_treeCtrl->Refresh();
    }
}

void MyFrame::OnToggleIcon(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->DoToggleIcon(item);
}

void MyFrame::OnToggleState(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    m_treeCtrl->DoToggleState(item);
}

void MyFrame::DoShowFirstOrLast(TreeFunc0_t pfn, const wxString& label)
{
    const wxTreeItemId item = (m_treeCtrl->*pfn)();

    if ( !item.IsOk() )
    {
        wxLogMessage("There is no %s item", label);
    }
    else
    {
        wxLogMessage("The %s item is \"%s\"",
                     label, m_treeCtrl->GetItemText(item));
    }
}

void MyFrame::DoShowRelativeItem(TreeFunc1_t pfn, const wxString& label)
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();

    CHECK_ITEM( item );

    if ((pfn == (TreeFunc1_t) &wxTreeCtrl::GetPrevVisible
         || pfn == (TreeFunc1_t) &wxTreeCtrl::GetNextVisible)
        && !m_treeCtrl->IsVisible(item))
    {
        wxLogMessage("The selected item must be visible.");
        return;
    }

    wxTreeItemId new_item = (m_treeCtrl->*pfn)(item);

    if ( !new_item.IsOk() )
    {
        wxLogMessage("There is no %s item", label);
    }
    else
    {
        wxLogMessage("The %s item is \"%s\"",
                     label, m_treeCtrl->GetItemText(new_item));
    }
}

void MyFrame::OnScrollTo(wxCommandEvent& WXUNUSED(event))
{
    // scroll to the last but one top level child
    wxTreeItemId item = m_treeCtrl->GetPrevSibling(
                            m_treeCtrl->GetLastChild(
                                m_treeCtrl->GetRootItem()));
    CHECK_ITEM( item );

    m_treeCtrl->ScrollTo(item);
}

void MyFrame::OnSelectLast(wxCommandEvent& WXUNUSED(event))
{
    // select the very last item of the tree
    wxTreeItemId item = m_treeCtrl->GetRootItem();
    for ( ;; )
    {
        wxTreeItemId itemChild = m_treeCtrl->GetLastChild(item);
        if ( !itemChild.IsOk() )
            break;

        item = itemChild;
    }

    CHECK_ITEM( item );

    m_treeCtrl->SelectItem(item);
}

void MyFrame::OnSetFgColour(wxCommandEvent& WXUNUSED(event))
{
    wxColour col = wxGetColourFromUser(this, m_treeCtrl->GetForegroundColour());
    if ( col.IsOk() )
        m_treeCtrl->SetForegroundColour(col);
}

void MyFrame::OnSetBgColour(wxCommandEvent& WXUNUSED(event))
{
    wxColour col = wxGetColourFromUser(this, m_treeCtrl->GetBackgroundColour());
    if ( col.IsOk() )
        m_treeCtrl->SetBackgroundColour(col);
}

// MyTreeCtrl implementation
#if USE_GENERIC_TREECTRL
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxGenericTreeCtrl)
#else
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxTreeCtrl)
#endif

MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style)
          : wxTreeCtrl(parent, id, pos, size, style),
            m_alternateImages(false),
            m_alternateStates(false)
{
    m_reverseSort = false;

    CreateImageList();
    CreateStateImageList();

    // Add some items to the tree
    AddTestItemsToTree(NUM_CHILDREN_PER_LEVEL, NUM_LEVELS);
}

void MyTreeCtrl::CreateImageList(int size)
{
    if ( size == -1 )
    {
        SetImageList(NULL);
        return;
    }
    if ( size == 0 )
        size = m_imageSize;
    else
        m_imageSize = size;

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, true);

    // should correspond to TreeCtrlIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[5];

    if (m_alternateImages)
    {
        icons[TreeCtrlIcon_File] = wxIcon(icon1_xpm);
        icons[TreeCtrlIcon_FileSelected] = wxIcon(icon2_xpm);
        icons[TreeCtrlIcon_Folder] = wxIcon(icon3_xpm);
        icons[TreeCtrlIcon_FolderSelected] = wxIcon(icon4_xpm);
        icons[TreeCtrlIcon_FolderOpened] = wxIcon(icon5_xpm);
    }
    else
    {
        wxSize iconSize(size, size);

        icons[TreeCtrlIcon_File] =
        icons[TreeCtrlIcon_FileSelected] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
        icons[TreeCtrlIcon_Folder] =
        icons[TreeCtrlIcon_FolderSelected] =
        icons[TreeCtrlIcon_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    }

    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        int sizeOrig = icons[0].GetWidth();
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignImageList(images);
}

void MyTreeCtrl::CreateStateImageList(bool del)
{
    if ( del )
    {
        SetStateImageList(NULL);
        return;
    }

    wxImageList *states;
    wxBusyCursor wait;

    if (m_alternateStates)
    {
        wxIcon icons[5];
        icons[0] = wxIcon(state1_xpm);  // yellow
        icons[1] = wxIcon(state2_xpm);  // green
        icons[2] = wxIcon(state3_xpm);  // red
        icons[3] = wxIcon(state4_xpm);  // blue
        icons[4] = wxIcon(state5_xpm);  // black

        int width  = icons[0].GetWidth(),
            height = icons[0].GetHeight();

        // Make an state image list containing small icons
        states = new wxImageList(width, height, true);

        for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
            states->Add(icons[i]);
    }
    else
    {
        wxIcon icons[2];
        icons[0] = wxIcon(unchecked_xpm);
        icons[1] = wxIcon(checked_xpm);

        int width  = icons[0].GetWidth(),
            height = icons[0].GetHeight();

        // Make an state image list containing small icons
        states = new wxImageList(width, height, true);

        for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
            states->Add(icons[i]);
    }

    AssignStateImageList(states);
}

#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
void MyTreeCtrl::CreateButtonsImageList(int size)
{
    if ( size == -1 )
    {
        SetButtonsImageList(NULL);
        return;
    }

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, true);

    // should correspond to TreeCtrlIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[4];

    if (m_alternateImages)
    {
        icons[0] = wxIcon(icon3_xpm);   // closed
        icons[1] = wxIcon(icon3_xpm);   // closed, selected
        icons[2] = wxIcon(icon5_xpm);   // open
        icons[3] = wxIcon(icon5_xpm);   // open, selected
    }
    else
    {
        wxSize iconSize(size, size);

        icons[0] =                                                                 // closed
        icons[1] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);     // closed, selected
        icons[2] =                                                                 // open
        icons[3] = wxArtProvider::GetIcon(wxART_FOLDER_OPEN, wxART_LIST, iconSize);// open, selected
    }

   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        int sizeOrig = icons[i].GetWidth();
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignButtonsImageList(images);
#else
void MyTreeCtrl::CreateButtonsImageList(int WXUNUSED(size))
{
#endif
}

int MyTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    if ( m_reverseSort )
    {
        // just exchange 1st and 2nd items
        return wxTreeCtrl::OnCompareItems(item2, item1);
    }
    else
    {
        return wxTreeCtrl::OnCompareItems(item1, item2);
    }
}

void MyTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent,
                                     size_t numChildren,
                                     size_t depth,
                                     size_t folder)
{
    if ( depth > 0 )
    {
        bool hasChildren = depth > 1;

        wxString str;
        for ( size_t n = 0; n < numChildren; n++ )
        {
            // at depth 1 elements won't have any more children
            if ( hasChildren )
                str.Printf(wxT("%s child %u"), wxT("Folder"), unsigned(n + 1));
            else
                str.Printf(wxT("%s child %u.%u"), wxT("File"), unsigned(folder), unsigned(n + 1));

            // here we pass to AppendItem() normal and selected item images (we
            // suppose that selected image follows the normal one in the enum)
            int image, imageSel;
            if ( wxGetApp().ShowImages() )
            {
                image = depth == 1 ? TreeCtrlIcon_File : TreeCtrlIcon_Folder;
                imageSel = image + 1;
            }
            else
            {
                image = imageSel = -1;
            }
            wxTreeItemId id = AppendItem(idParent, str, image, imageSel,
                                         new MyTreeItemData(str));

            if ( wxGetApp().ShowStates() )
                SetItemState(id, 0);

            // and now we also set the expanded one (only for the folders)
            if ( hasChildren && wxGetApp().ShowImages() )
            {
                SetItemImage(id, TreeCtrlIcon_FolderOpened,
                             wxTreeItemIcon_Expanded);
            }

            // remember the last child for OnEnsureVisible()
            if ( !hasChildren && n == numChildren - 1 )
            {
                m_lastItem = id;
            }

            AddItemsRecursively(id, numChildren, depth - 1, n + 1);
        }
    }
    //else: done!
}

void MyTreeCtrl::AddTestItemsToTree(size_t numChildren,
                                    size_t depth)
{
    int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_Folder : -1;
    wxTreeItemId rootId = AddRoot(wxT("Root"),
                                  image, image,
                                  new MyTreeItemData(wxT("Root item")));
    if ( !HasFlag(wxTR_HIDE_ROOT) && image != -1 )
    {
        SetItemImage(rootId, TreeCtrlIcon_FolderOpened, wxTreeItemIcon_Expanded);
    }

    AddItemsRecursively(rootId, numChildren, depth, 0);

    // set some colours/fonts for testing
    if ( !HasFlag(wxTR_HIDE_ROOT) )
        SetItemFont(rootId, *wxITALIC_FONT);

    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetFirstChild(rootId, cookie);
    SetItemTextColour(id, *wxBLUE);

    id = GetNextChild(rootId, cookie);
    if ( id )
        id = GetNextChild(rootId, cookie);
    if ( id )
    {
        SetItemTextColour(id, *wxRED);
        SetItemBackgroundColour(id, *wxLIGHT_GREY);
    }
}

void MyTreeCtrl::GetItemsRecursively(const wxTreeItemId& idParent,
                                     wxTreeItemIdValue cookie)
{
    wxTreeItemId id;

    if ( !cookie )
        id = GetFirstChild(idParent, cookie);
    else
        id = GetNextChild(idParent, cookie);

    if ( !id.IsOk() )
        return;

    wxString text = GetItemText(id);
    wxLogMessage(text);

    if (ItemHasChildren(id))
        GetItemsRecursively(id);

    GetItemsRecursively(idParent, cookie);
}

void MyTreeCtrl::DoToggleIcon(const wxTreeItemId& item)
{
    int image = GetItemImage(item) == TreeCtrlIcon_Folder
                    ? TreeCtrlIcon_File
                    : TreeCtrlIcon_Folder;
    SetItemImage(item, image, wxTreeItemIcon_Normal);

    image = GetItemImage(item, wxTreeItemIcon_Selected) == TreeCtrlIcon_FolderSelected
                    ? TreeCtrlIcon_FileSelected
                    : TreeCtrlIcon_FolderSelected;
    SetItemImage(item, image, wxTreeItemIcon_Selected);
}

void MyTreeCtrl::DoToggleState(const wxTreeItemId& item)
{
    if ( m_alternateStates )
    {
        // sets random state unlike current
        int state = GetItemState(item);
        int nState;

        srand (time(NULL));
        do {
            nState = rand() % GetStateImageList()->GetImageCount();
        } while (nState == state);

        SetItemState(item, nState);
    }
    else
    {
        // we have only 2 checkbox states, so next state will be reversed
        SetItemState(item, wxTREE_ITEMSTATE_NEXT);
    }
}

void MyTreeCtrl::DoResetBrokenStateImages(const wxTreeItemId& idParent,
                                          wxTreeItemIdValue cookie, int state)
{
    wxTreeItemId id;

    if ( !cookie )
        id = GetFirstChild(idParent, cookie);
    else
        id = GetNextChild(idParent, cookie);

    if ( !id.IsOk() )
        return;

    int curState = GetItemState(id);
    if ( curState != wxTREE_ITEMSTATE_NONE && curState > state )
        SetItemState(id, state);

    if (ItemHasChildren(id))
        DoResetBrokenStateImages(id, 0, state);

    DoResetBrokenStateImages(idParent, cookie, state);
}

void MyTreeCtrl::LogEvent(const wxChar *name, const wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    wxString text;
    if ( item.IsOk() )
        text << wxT('"') << GetItemText(item).c_str() << wxT('"');
    else
        text = wxT("invalid item");
    wxLogMessage(wxT("%s(%s)"), name, text.c_str());
}

// avoid repetition
#define TREE_EVENT_HANDLER(name)                                 \
void MyTreeCtrl::name(wxTreeEvent& event)                        \
{                                                                \
    LogEvent(wxT(#name), event);                                  \
    SetLastItem(wxTreeItemId());                                 \
    event.Skip();                                                \
}

TREE_EVENT_HANDLER(OnBeginRDrag)
TREE_EVENT_HANDLER(OnDeleteItem)
TREE_EVENT_HANDLER(OnGetInfo)
TREE_EVENT_HANDLER(OnSetInfo)
TREE_EVENT_HANDLER(OnItemExpanded)
TREE_EVENT_HANDLER(OnItemExpanding)
TREE_EVENT_HANDLER(OnItemCollapsed)
TREE_EVENT_HANDLER(OnSelChanged)
TREE_EVENT_HANDLER(OnSelChanging)

#undef TREE_EVENT_HANDLER

void LogKeyEvent(const wxChar *name, const wxKeyEvent& event)
{
    wxString key;
    long keycode = event.GetKeyCode();
    {
        switch ( keycode )
        {
            case WXK_BACK: key = wxT("BACK"); break;
            case WXK_TAB: key = wxT("TAB"); break;
            case WXK_RETURN: key = wxT("RETURN"); break;
            case WXK_ESCAPE: key = wxT("ESCAPE"); break;
            case WXK_SPACE: key = wxT("SPACE"); break;
            case WXK_DELETE: key = wxT("DELETE"); break;
            case WXK_START: key = wxT("START"); break;
            case WXK_LBUTTON: key = wxT("LBUTTON"); break;
            case WXK_RBUTTON: key = wxT("RBUTTON"); break;
            case WXK_CANCEL: key = wxT("CANCEL"); break;
            case WXK_MBUTTON: key = wxT("MBUTTON"); break;
            case WXK_CLEAR: key = wxT("CLEAR"); break;
            case WXK_SHIFT: key = wxT("SHIFT"); break;
            case WXK_ALT: key = wxT("ALT"); break;
            case WXK_CONTROL: key = wxT("CONTROL"); break;
            case WXK_MENU: key = wxT("MENU"); break;
            case WXK_PAUSE: key = wxT("PAUSE"); break;
            case WXK_CAPITAL: key = wxT("CAPITAL"); break;
            case WXK_END: key = wxT("END"); break;
            case WXK_HOME: key = wxT("HOME"); break;
            case WXK_LEFT: key = wxT("LEFT"); break;
            case WXK_UP: key = wxT("UP"); break;
            case WXK_RIGHT: key = wxT("RIGHT"); break;
            case WXK_DOWN: key = wxT("DOWN"); break;
            case WXK_SELECT: key = wxT("SELECT"); break;
            case WXK_PRINT: key = wxT("PRINT"); break;
            case WXK_EXECUTE: key = wxT("EXECUTE"); break;
            case WXK_SNAPSHOT: key = wxT("SNAPSHOT"); break;
            case WXK_INSERT: key = wxT("INSERT"); break;
            case WXK_HELP: key = wxT("HELP"); break;
            case WXK_NUMPAD0: key = wxT("NUMPAD0"); break;
            case WXK_NUMPAD1: key = wxT("NUMPAD1"); break;
            case WXK_NUMPAD2: key = wxT("NUMPAD2"); break;
            case WXK_NUMPAD3: key = wxT("NUMPAD3"); break;
            case WXK_NUMPAD4: key = wxT("NUMPAD4"); break;
            case WXK_NUMPAD5: key = wxT("NUMPAD5"); break;
            case WXK_NUMPAD6: key = wxT("NUMPAD6"); break;
            case WXK_NUMPAD7: key = wxT("NUMPAD7"); break;
            case WXK_NUMPAD8: key = wxT("NUMPAD8"); break;
            case WXK_NUMPAD9: key = wxT("NUMPAD9"); break;
            case WXK_MULTIPLY: key = wxT("MULTIPLY"); break;
            case WXK_ADD: key = wxT("ADD"); break;
            case WXK_SEPARATOR: key = wxT("SEPARATOR"); break;
            case WXK_SUBTRACT: key = wxT("SUBTRACT"); break;
            case WXK_DECIMAL: key = wxT("DECIMAL"); break;
            case WXK_DIVIDE: key = wxT("DIVIDE"); break;
            case WXK_F1: key = wxT("F1"); break;
            case WXK_F2: key = wxT("F2"); break;
            case WXK_F3: key = wxT("F3"); break;
            case WXK_F4: key = wxT("F4"); break;
            case WXK_F5: key = wxT("F5"); break;
            case WXK_F6: key = wxT("F6"); break;
            case WXK_F7: key = wxT("F7"); break;
            case WXK_F8: key = wxT("F8"); break;
            case WXK_F9: key = wxT("F9"); break;
            case WXK_F10: key = wxT("F10"); break;
            case WXK_F11: key = wxT("F11"); break;
            case WXK_F12: key = wxT("F12"); break;
            case WXK_F13: key = wxT("F13"); break;
            case WXK_F14: key = wxT("F14"); break;
            case WXK_F15: key = wxT("F15"); break;
            case WXK_F16: key = wxT("F16"); break;
            case WXK_F17: key = wxT("F17"); break;
            case WXK_F18: key = wxT("F18"); break;
            case WXK_F19: key = wxT("F19"); break;
            case WXK_F20: key = wxT("F20"); break;
            case WXK_F21: key = wxT("F21"); break;
            case WXK_F22: key = wxT("F22"); break;
            case WXK_F23: key = wxT("F23"); break;
            case WXK_F24: key = wxT("F24"); break;
            case WXK_NUMLOCK: key = wxT("NUMLOCK"); break;
            case WXK_SCROLL: key = wxT("SCROLL"); break;
            case WXK_PAGEUP: key = wxT("PAGEUP"); break;
            case WXK_PAGEDOWN: key = wxT("PAGEDOWN"); break;
            case WXK_NUMPAD_SPACE: key = wxT("NUMPAD_SPACE"); break;
            case WXK_NUMPAD_TAB: key = wxT("NUMPAD_TAB"); break;
            case WXK_NUMPAD_ENTER: key = wxT("NUMPAD_ENTER"); break;
            case WXK_NUMPAD_F1: key = wxT("NUMPAD_F1"); break;
            case WXK_NUMPAD_F2: key = wxT("NUMPAD_F2"); break;
            case WXK_NUMPAD_F3: key = wxT("NUMPAD_F3"); break;
            case WXK_NUMPAD_F4: key = wxT("NUMPAD_F4"); break;
            case WXK_NUMPAD_HOME: key = wxT("NUMPAD_HOME"); break;
            case WXK_NUMPAD_LEFT: key = wxT("NUMPAD_LEFT"); break;
            case WXK_NUMPAD_UP: key = wxT("NUMPAD_UP"); break;
            case WXK_NUMPAD_RIGHT: key = wxT("NUMPAD_RIGHT"); break;
            case WXK_NUMPAD_DOWN: key = wxT("NUMPAD_DOWN"); break;
            case WXK_NUMPAD_PAGEUP: key = wxT("NUMPAD_PAGEUP"); break;
            case WXK_NUMPAD_PAGEDOWN: key = wxT("NUMPAD_PAGEDOWN"); break;
            case WXK_NUMPAD_END: key = wxT("NUMPAD_END"); break;
            case WXK_NUMPAD_BEGIN: key = wxT("NUMPAD_BEGIN"); break;
            case WXK_NUMPAD_INSERT: key = wxT("NUMPAD_INSERT"); break;
            case WXK_NUMPAD_DELETE: key = wxT("NUMPAD_DELETE"); break;
            case WXK_NUMPAD_EQUAL: key = wxT("NUMPAD_EQUAL"); break;
            case WXK_NUMPAD_MULTIPLY: key = wxT("NUMPAD_MULTIPLY"); break;
            case WXK_NUMPAD_ADD: key = wxT("NUMPAD_ADD"); break;
            case WXK_NUMPAD_SEPARATOR: key = wxT("NUMPAD_SEPARATOR"); break;
            case WXK_NUMPAD_SUBTRACT: key = wxT("NUMPAD_SUBTRACT"); break;
            case WXK_NUMPAD_DECIMAL: key = wxT("NUMPAD_DECIMAL"); break;

            default:
            {
               if ( keycode < 128 && wxIsprint((int)keycode) )
                   key.Printf(wxT("'%c'"), (char)keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), wxT('A') + keycode - 1);
               else
                   key.Printf(wxT("unknown (%ld)"), keycode);
            }
        }
    }

    wxLogMessage( wxT("%s event: %s (flags = %c%c%c%c)"),
                  name,
                  key.c_str(),
                  event.ControlDown() ? wxT('C') : wxT('-'),
                  event.AltDown() ? wxT('A') : wxT('-'),
                  event.ShiftDown() ? wxT('S') : wxT('-'),
                  event.MetaDown() ? wxT('M') : wxT('-'));
}

void MyTreeCtrl::OnTreeKeyDown(wxTreeEvent& event)
{
    LogKeyEvent(wxT("Tree key down "), event.GetKeyEvent());

    event.Skip();
}

void MyTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
    // need to explicitly allow drag
    if ( event.GetItem() != GetRootItem() )
    {
        m_draggedItem = event.GetItem();

        wxPoint clientpt = event.GetPoint();
        wxPoint screenpt = ClientToScreen(clientpt);

        wxLogMessage(wxT("OnBeginDrag: started dragging %s at screen coords (%i,%i)"),
                     GetItemText(m_draggedItem).c_str(),
                     screenpt.x, screenpt.y);

        event.Allow();
    }
    else
    {
        wxLogMessage(wxT("OnBeginDrag: this item can't be dragged."));
    }
}

void MyTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemSrc = m_draggedItem,
                 itemDst = event.GetItem();
    m_draggedItem = (wxTreeItemId)0l;

    // where to copy the item?
    if ( itemDst.IsOk() && !ItemHasChildren(itemDst) )
    {
        // copy to the parent then
        itemDst = GetItemParent(itemDst);
    }

    if ( !itemDst.IsOk() )
    {
        wxLogMessage(wxT("OnEndDrag: can't drop here."));

        return;
    }

    wxString text = GetItemText(itemSrc);
    wxLogMessage(wxT("OnEndDrag: '%s' copied to '%s'."),
                 text.c_str(), GetItemText(itemDst).c_str());

    // just do append here - we could also insert it just before/after the item
    // on which it was dropped, but this requires slightly more work... we also
    // completely ignore the client data and icon of the old item but could
    // copy them as well.
    //
    // Finally, we only copy one item here but we might copy the entire tree if
    // we were dragging a folder.
    int image = wxGetApp().ShowImages() ? TreeCtrlIcon_File : -1;
    wxTreeItemId id = AppendItem(itemDst, text, image);

    if ( wxGetApp().ShowStates() )
        SetItemState(id, GetItemState(itemSrc));
}

void MyTreeCtrl::OnBeginLabelEdit(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnBeginLabelEdit"));

    // for testing, prevent this item's label editing
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox(wxT("You can't edit this item."));

        event.Veto();
    }
    else if ( itemId == GetRootItem() )
    {
        // test that it is possible to change the text of the item being edited
        SetItemText(itemId, wxT("Editing root item"));
    }
}

void MyTreeCtrl::OnEndLabelEdit(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnEndLabelEdit"));

    // don't allow anything except letters in the labels
    if ( !event.GetLabel().IsWord() )
    {
        wxMessageBox(wxT("The new label should be a single word."));

        event.Veto();
    }
}

void MyTreeCtrl::OnItemCollapsing(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnItemCollapsing"));

    // for testing, prevent the user from collapsing the first child folder
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox(wxT("You can't collapse this item."));

        event.Veto();
    }
}

void MyTreeCtrl::OnItemActivated(wxTreeEvent& event)
{
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
    MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);

    if ( item != NULL )
    {
        item->ShowInfo(this);
    }

    wxLogMessage(wxT("OnItemActivated"));
}

void MyTreeCtrl::OnItemStateClick(wxTreeEvent& event)
{
    // toggle item state
    wxTreeItemId itemId = event.GetItem();
    DoToggleState(itemId);

    wxLogMessage(wxT("Item \"%s\" state changed to %d"),
                 GetItemText(itemId), GetItemState(itemId));
}

void MyTreeCtrl::OnItemMenu(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    wxCHECK_RET( itemId.IsOk(), "should have a valid item" );

    MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);
    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);

    wxLogMessage(wxT("OnItemMenu for item \"%s\" at screen coords (%i, %i)"),
                 item->GetDesc(), screenpt.x, screenpt.y);

    ShowMenu(itemId, clientpt);
    event.Skip();
}

void MyTreeCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint pt = event.GetPosition();

    wxLogMessage(wxT("OnContextMenu at screen coords (%i, %i)"), pt.x, pt.y);

    event.Skip();
}

void MyTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    wxString title;
    if ( id.IsOk() )
    {
        title << wxT("Menu for ") << GetItemText(id);
    }
    else
    {
        title = wxT("Menu for no particular item");
    }

#if wxUSE_MENUS
    wxMenu menu(title);
    menu.Append(TreeTest_About, wxT("&About..."));
    menu.AppendSeparator();
    menu.Append(TreeTest_Highlight, wxT("&Highlight item"));
    menu.Append(TreeTest_Dump, wxT("&Dump"));

    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}

void MyTreeCtrl::OnItemRClick(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    wxCHECK_RET( itemId.IsOk(), "should have a valid item" );

    MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);

    wxLogMessage(wxT("Item \"%s\" right clicked"), item->GetDesc());

    event.Skip();
}

void MyTreeCtrl::OnRMouseDown(wxMouseEvent& event)
{
    wxLogMessage(wxT("Right mouse button down"));

    event.Skip();
}

void MyTreeCtrl::OnRMouseUp(wxMouseEvent& event)
{
    wxLogMessage(wxT("Right mouse button up"));

    event.Skip();
}

void MyTreeCtrl::OnRMouseDClick(wxMouseEvent& event)
{
    wxTreeItemId id = HitTest(event.GetPosition());
    if ( !id )
    {
        wxLogMessage(wxT("No item under mouse"));
    }
    else
    {
        MyTreeItemData *item = (MyTreeItemData *)GetItemData(id);
        if ( item )
        {
            wxLogMessage(wxT("Item '%s' under mouse"), item->GetDesc());
        }
    }

    event.Skip();
}

static inline const wxChar *Bool2String(bool b)
{
    return b ? wxT("") : wxT("not ");
}

void MyTreeItemData::ShowInfo(wxTreeCtrl *tree)
{
    wxLogMessage(wxT("Item '%s': %sselected, %sexpanded, %sbold,\n")
                 wxT("%u children (%u immediately under this item)."),
                 m_desc.c_str(),
                 Bool2String(tree->IsSelected(GetId())),
                 Bool2String(tree->IsExpanded(GetId())),
                 Bool2String(tree->IsBold(GetId())),
                 unsigned(tree->GetChildrenCount(GetId())),
                 unsigned(tree->GetChildrenCount(GetId(), false)));
}
