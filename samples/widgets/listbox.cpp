/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        listbox.cpp
// Purpose:     Part of the widgets sample showing wxListbox
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_LISTBOX

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/combobox.h"
    #include "wx/listbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "wx/checklst.h"
#include "wx/rearrangectrl.h"

#include "itemcontainer.h"
#include "widgets.h"

#include "icons/listbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ListboxPage_Reset = wxID_HIGHEST,
    ListboxPage_Add,
    ListboxPage_AddText,
    ListboxPage_AddSeveral,
    ListboxPage_AddMany,
    ListboxPage_Clear,
    ListboxPage_Change,
    ListboxPage_ChangeText,
    ListboxPage_Delete,
    ListboxPage_DeleteText,
    ListboxPage_DeleteSel,
    ListboxPage_Listbox,
    ListboxPage_EnsureVisible,
    ListboxPage_EnsureVisibleText,
    ListboxPage_ContainerTests,
    ListboxPage_GetTopItem,
    ListboxPage_GetCountPerPage,
    ListboxPage_MoveUp,
    ListboxPage_MoveDown,
};

// ----------------------------------------------------------------------------
// ListboxWidgetsPage
// ----------------------------------------------------------------------------

class ListboxWidgetsPage : public ItemContainerWidgetsPage
{
public:
    ListboxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const override { return m_lbox; }
    virtual wxItemContainer* GetContainer() const override { return m_lbox; }
    virtual void RecreateWidget() override { CreateLbox(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonEnsureVisible(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);
    void OnButtonTopItem(wxCommandEvent& event);
    void OnButtonPageCount(wxCommandEvent& event);
    void OnButtonMoveUp(wxCommandEvent& evt);
    void OnButtonMoveDown(wxCommandEvent& evt);

    void OnListbox(wxCommandEvent& event);
    void OnListboxDClick(wxCommandEvent& event);
    void OnCheckListbox(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIEnsureVisibleButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);
    void OnUpdateUIMoveButtons(wxUpdateUIEvent& evt);

    // reset the listbox parameters
    void Reset();

    // (re)create the listbox
    void CreateLbox();

    // read the value of a listbox item index from the given control, return
    // false if it's invalid
    bool GetValidIndexFromText(const wxTextCtrl *text, int *n = nullptr) const;


    // listbox parameters
    // ------------------

    // the selection mode
    enum LboxSelection
    {
        LboxSel_Single,
        LboxSel_Extended,
        LboxSel_Multiple
    } m_lboxSelMode;

    // the list type
    enum LboxType
    {
        LboxType_ListBox,
        LboxType_CheckListBox,
        LboxType_RearrangeList
    };

    // should it be sorted?
    bool m_sorted;

    // should it have horz scroll/vert scrollbar permanently shown?
    bool m_horzScroll,
         m_vertScrollAlways;

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioSelMode;

    // List type selection radiobox
    wxRadioBox *m_radioListType;

    // the checkboxes
    wxCheckBox *m_chkVScroll,
               *m_chkHScroll,
               *m_chkSort,
               *m_chkOwnerDraw;

    // the listbox itself and the sizer it is in
    wxListBox *m_lbox;

    wxSizer *m_sizerLbox;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textAdd,
               *m_textChange,
               *m_textEnsureVisible,
               *m_textDelete;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ListboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ListboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(ListboxPage_Reset, ListboxWidgetsPage::OnButtonReset)
    EVT_BUTTON(ListboxPage_Change, ListboxWidgetsPage::OnButtonChange)
    EVT_BUTTON(ListboxPage_Delete, ListboxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ListboxPage_DeleteSel, ListboxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ListboxPage_EnsureVisible, ListboxWidgetsPage::OnButtonEnsureVisible)
    EVT_BUTTON(ListboxPage_Clear, ListboxWidgetsPage::OnButtonClear)
    EVT_BUTTON(ListboxPage_Add, ListboxWidgetsPage::OnButtonAdd)
    EVT_BUTTON(ListboxPage_AddSeveral, ListboxWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(ListboxPage_AddMany, ListboxWidgetsPage::OnButtonAddMany)
    EVT_BUTTON(ListboxPage_ContainerTests, ItemContainerWidgetsPage::OnButtonTestItemContainer)
    EVT_BUTTON(ListboxPage_GetTopItem, ListboxWidgetsPage::OnButtonTopItem)
    EVT_BUTTON(ListboxPage_GetCountPerPage, ListboxWidgetsPage::OnButtonPageCount)
    EVT_BUTTON(ListboxPage_MoveUp, ListboxWidgetsPage::OnButtonMoveUp)
    EVT_BUTTON(ListboxPage_MoveDown, ListboxWidgetsPage::OnButtonMoveDown)

    EVT_TEXT_ENTER(ListboxPage_AddText, ListboxWidgetsPage::OnButtonAdd)
    EVT_TEXT_ENTER(ListboxPage_DeleteText, ListboxWidgetsPage::OnButtonDelete)
    EVT_TEXT_ENTER(ListboxPage_EnsureVisibleText, ListboxWidgetsPage::OnButtonEnsureVisible)

    EVT_UPDATE_UI(ListboxPage_Reset, ListboxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(ListboxPage_AddSeveral, ListboxWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(ListboxPage_Clear, ListboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ListboxPage_DeleteText, ListboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ListboxPage_Delete, ListboxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(ListboxPage_Change, ListboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ListboxPage_ChangeText, ListboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ListboxPage_DeleteSel, ListboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ListboxPage_EnsureVisible, ListboxWidgetsPage::OnUpdateUIEnsureVisibleButton)
    EVT_UPDATE_UI_RANGE(ListboxPage_MoveUp, ListboxPage_MoveDown, ListboxWidgetsPage::OnUpdateUIMoveButtons)

    EVT_LISTBOX(ListboxPage_Listbox, ListboxWidgetsPage::OnListbox)
    EVT_LISTBOX_DCLICK(ListboxPage_Listbox, ListboxWidgetsPage::OnListboxDClick)
    EVT_CHECKLISTBOX(ListboxPage_Listbox, ListboxWidgetsPage::OnCheckListbox)

    EVT_CHECKBOX(wxID_ANY, ListboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ListboxWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ListboxWidgetsPage, "Listbox",
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS
                       );

ListboxWidgetsPage::ListboxWidgetsPage(WidgetsBookCtrl *book,
                                       wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, listbox_xpm)
{
    // init everything
    m_radioSelMode = nullptr;
    m_radioListType = nullptr;

    m_chkVScroll =
    m_chkHScroll =
    m_chkSort =
    m_chkOwnerDraw = nullptr;

    m_lbox = nullptr;
    m_sizerLbox = nullptr;

}

void ListboxWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous listbox operations and the pane containing the listbox
       itself to the right
    */
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBoxSizer *sizerLeft = new wxStaticBoxSizer(wxVERTICAL, this, "&Set listbox parameters");
    wxStaticBox* const sizerLeftBox = sizerLeft->GetStaticBox();

    m_chkVScroll = CreateCheckBoxAndAddToSizer
                   (
                    sizerLeft,
                    "Always show &vertical scrollbar",
                    wxID_ANY, sizerLeftBox
                   );
    m_chkHScroll = CreateCheckBoxAndAddToSizer
                   (
                    sizerLeft,
                    "Show &horizontal scrollbar",
                    wxID_ANY, sizerLeftBox
                   );
    m_chkSort = CreateCheckBoxAndAddToSizer(sizerLeft, "&Sort items", wxID_ANY, sizerLeftBox);
    m_chkOwnerDraw = CreateCheckBoxAndAddToSizer(sizerLeft, "&Owner drawn", wxID_ANY, sizerLeftBox);

    static const wxString modes[] =
    {
        "single",
        "extended",
        "multiple",
    };

    m_radioSelMode = new wxRadioBox(sizerLeftBox, wxID_ANY, "Selection &mode:",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(modes), modes,
                                    1, wxRA_SPECIFY_COLS);

    static const wxString listTypes[] =
    {
        "list box"
#if wxUSE_CHECKLISTBOX
      , "check list box"
#endif // wxUSE_CHECKLISTBOX
#if wxUSE_REARRANGECTRL
      , "rearrange list"
#endif // wxUSE_REARRANGECTRL
    };
    m_radioListType = new wxRadioBox(sizerLeftBox, wxID_ANY, "&List type:",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(listTypes), listTypes,
                                    1, wxRA_SPECIFY_COLS);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioSelMode, 0, wxGROW | wxALL, 5);

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioListType, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(sizerLeftBox, ListboxPage_Reset, "&Reset");
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBoxSizer *sizerMiddle = new wxStaticBoxSizer(wxVERTICAL, this, "&Change listbox contents");
    wxStaticBox* const sizerMiddleBox = sizerMiddle->GetStaticBox();

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(sizerMiddleBox, ListboxPage_Add, "&Add this string");
    m_textAdd = new wxTextCtrl(sizerMiddleBox, ListboxPage_AddText, "test item \t0");
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textAdd, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_AddSeveral, "&Insert a few strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_AddMany, "Add &many strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(sizerMiddleBox, ListboxPage_Change, "C&hange current");
    m_textChange = new wxTextCtrl(sizerMiddleBox, ListboxPage_ChangeText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textChange, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(sizerMiddleBox, ListboxPage_EnsureVisible, "Make item &visible");
    m_textEnsureVisible = new wxTextCtrl(sizerMiddleBox, ListboxPage_EnsureVisibleText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textEnsureVisible, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(sizerMiddleBox, ListboxPage_Delete, "&Delete this item");
    m_textDelete = new wxTextCtrl(sizerMiddleBox, ListboxPage_DeleteText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_DeleteSel, "Delete &selection");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_Clear, "&Clear");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_MoveUp, "Move item &up");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_MoveDown, "Move item &down");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_GetTopItem, "Get top item");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_GetCountPerPage, "Get count per page");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, ListboxPage_ContainerTests, "Run &tests");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_lbox = new wxListBox(this, ListboxPage_Listbox,
                           wxDefaultPosition, wxDefaultSize,
                           0, nullptr,
                           wxLB_HSCROLL);
    sizerRight->Add(m_lbox, 1, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerLbox = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void ListboxWidgetsPage::Reset()
{
    m_radioSelMode->SetSelection(LboxSel_Single);
    m_radioListType->SetSelection(LboxType_ListBox);
    m_chkVScroll->SetValue(false);
    m_chkHScroll->SetValue(true);
    m_chkSort->SetValue(false);
    m_chkOwnerDraw->SetValue(false);
}

void ListboxWidgetsPage::CreateLbox()
{
    int flags = GetAttrs().m_defaultFlags;
    switch ( m_radioSelMode->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unexpected radio box selection" );
            wxFALLTHROUGH;

        case LboxSel_Single:    flags |= wxLB_SINGLE; break;
        case LboxSel_Extended:  flags |= wxLB_EXTENDED; break;
        case LboxSel_Multiple:  flags |= wxLB_MULTIPLE; break;
    }

    if ( m_chkVScroll->GetValue() )
        flags |= wxLB_ALWAYS_SB;
    if ( m_chkHScroll->GetValue() )
        flags |= wxLB_HSCROLL;
    if ( m_chkSort->GetValue() )
        flags |= wxLB_SORT;
    if ( m_chkOwnerDraw->GetValue() )
        flags |= wxLB_OWNERDRAW;

    wxArrayString items;
    wxArrayInt order;
    if ( m_lbox )
    {
        int count = m_lbox->GetCount();
        for ( int n = 0; n < count; n++ )
        {
            items.Add(m_lbox->GetString(n));
        }

        order.reserve(count);
#if wxUSE_CHECKLISTBOX
        wxCheckListBox* cblist = wxDynamicCast(m_lbox, wxCheckListBox);
        if ( cblist )
        {
            for ( int n = 0; n < count; n++ )
                order.Add(cblist->IsChecked(n) ? n : ~n);
        }
        else
#endif // wxUSE_CHECKLISTBOX
        {
            for( int n = 0; n < count; n++ )
                order.Add(~n);
        }

        m_sizerLbox->Detach( m_lbox );
        delete m_lbox;
    }

    switch( m_radioListType->GetSelection() )
    {
#if wxUSE_CHECKLISTBOX
        case LboxType_CheckListBox:
            m_lbox = new wxCheckListBox(this, ListboxPage_Listbox,
                                    wxDefaultPosition, wxDefaultSize,
                                    items,
                                    flags);
            {
                wxCheckListBox* cblist = wxDynamicCast(m_lbox, wxCheckListBox);
                for ( size_t n = 0; n < order.size(); n++ )
                    cblist->Check(n, order[n] >= 0);
            }
            break;
#endif // wxUSE_CHECKLISTBOX
#if wxUSE_REARRANGECTRL
        case LboxType_RearrangeList:
            m_lbox = new wxRearrangeList(this, ListboxPage_Listbox,
                                    wxDefaultPosition, wxDefaultSize,
                                    order, items,
                                    flags);
            break;
#endif // wxUSE_REARRANGECTRL
        case LboxType_ListBox:
            wxFALLTHROUGH;
        default:
            m_lbox = new wxListBox(this, ListboxPage_Listbox,
                               wxDefaultPosition, wxDefaultSize,
                               items,
                               flags);
    }

    NotifyWidgetRecreation(m_lbox);

    m_sizerLbox->Add(m_lbox, 1, wxGROW | wxALL, 5);
    m_sizerLbox->Layout();
}

// ----------------------------------------------------------------------------
// miscellaneous helpers
// ----------------------------------------------------------------------------

bool
ListboxWidgetsPage::GetValidIndexFromText(const wxTextCtrl *text, int *n) const
{
    unsigned long idx;
    if ( !text->GetValue().ToULong(&idx) || (idx >= m_lbox->GetCount()) )
    {
        // don't give the warning if we're just testing but do give it if we
        // want to retrieve the value as this is only done in answer to a user
        // action
        if ( n )
        {
            wxLogWarning("Invalid index \"%s\"", text->GetValue());
        }

        return false;
    }

    if ( n )
        *n = idx;

    return true;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ListboxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateLbox();
}

void ListboxWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt selections;
    int count = m_lbox->GetSelections(selections);
    wxString s = m_textChange->GetValue();
    for ( int n = 0; n < count; n++ )
    {
        m_lbox->SetString(selections[n], s);
    }
}

void ListboxWidgetsPage::OnButtonEnsureVisible(wxCommandEvent& WXUNUSED(event))
{
    int n;
    if ( !GetValidIndexFromText(m_textEnsureVisible, &n) )
    {
        return;
    }

    m_lbox->EnsureVisible(n);
}

void ListboxWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    int n;
    if ( !GetValidIndexFromText(m_textDelete, &n) )
    {
        return;
    }

    m_lbox->Delete(n);
}

void ListboxWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt selections;
    int n = m_lbox->GetSelections(selections);
    while ( n > 0 )
    {
        m_lbox->Delete(selections[--n]);
    }
}

void ListboxWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_lbox->Clear();
}

void ListboxWidgetsPage::OnButtonTopItem(wxCommandEvent& WXUNUSED(event))
{
    int item = m_lbox->GetTopItem();
    wxLogMessage("Topmost visible item is: %d", item);
}

void ListboxWidgetsPage::OnButtonPageCount(wxCommandEvent& WXUNUSED(event))
{
    int count = m_lbox->GetCountPerPage();
    wxLogMessage("%d items fit into this listbox.", count);
}

void ListboxWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format("test item \t%u", ++s_item));
    }

    m_lbox->Append(s);
}

void ListboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_lbox->Append(wxString::Format("item #%u", n));
    }
}

void ListboxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString items;
    items.Add("First");
    items.Add("another one");
    items.Add("and the last (very very very very very very very very very very long) one");
    m_lbox->InsertItems(items, 0);
}

void ListboxWidgetsPage::OnButtonMoveUp(wxCommandEvent& WXUNUSED(evt))
{
#if wxUSE_REARRANGECTRL
    wxRearrangeList* list = wxDynamicCast(m_lbox, wxRearrangeList);
    list->MoveCurrentUp();
#endif // wxUSE_REARRANGECTRL
}

void ListboxWidgetsPage::OnButtonMoveDown(wxCommandEvent& WXUNUSED(evt))
{
#if wxUSE_REARRANGECTRL
    wxRearrangeList* list = wxDynamicCast(m_lbox, wxRearrangeList);
    list->MoveCurrentDown();
#endif // wxUSE_REARRANGECTRL
}

void ListboxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( (m_radioSelMode->GetSelection() != LboxSel_Single) ||
                  m_chkSort->GetValue() ||
                  m_chkOwnerDraw->GetValue() ||
                  !m_chkHScroll->GetValue() ||
                  m_chkVScroll->GetValue() );
}

void ListboxWidgetsPage::OnUpdateUIEnsureVisibleButton(wxUpdateUIEvent& event)
{
    event.Enable(GetValidIndexFromText(m_textEnsureVisible));
}

void ListboxWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    event.Enable(GetValidIndexFromText(m_textDelete));
}

void ListboxWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    wxArrayInt selections;
    event.Enable(m_lbox->GetSelections(selections) != 0);
}

void ListboxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(m_lbox->GetCount() != 0);
}

void ListboxWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    event.Enable(!(m_lbox->GetWindowStyle() & wxLB_SORT));
}

void ListboxWidgetsPage::OnUpdateUIMoveButtons(wxUpdateUIEvent& evt)
{
    evt.Enable(m_radioListType->GetSelection() == LboxType_RearrangeList);
}

void ListboxWidgetsPage::OnListbox(wxCommandEvent& event)
{
    long sel = event.GetSelection();
    m_textDelete->SetValue(wxString::Format("%ld", sel));

    if (event.IsSelection())
    {
        wxLogMessage("Listbox item %ld selected", sel);
    }
    else
    {
        wxLogMessage("Listbox item %ld deselected", sel);
    }
}

void ListboxWidgetsPage::OnListboxDClick(wxCommandEvent& event)
{
    wxLogMessage( "Listbox item %d double clicked", event.GetInt() );
}

void ListboxWidgetsPage::OnCheckListbox(wxCommandEvent& event)
{
    wxLogMessage( "Listbox item %d toggled", event.GetInt() );
}

void ListboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateLbox();
}

#endif // wxUSE_LISTBOX
