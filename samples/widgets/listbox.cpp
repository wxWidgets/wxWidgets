/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        listbox.cpp
// Purpose:     Part of the widgets sample showing wxListbox
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
    ListboxPage_ContainerTests
};

// ----------------------------------------------------------------------------
// ListboxWidgetsPage
// ----------------------------------------------------------------------------

class ListboxWidgetsPage : public ItemContainerWidgetsPage
{
public:
    ListboxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxControl *GetWidget() const { return m_lbox; }
    virtual wxItemContainer* GetContainer() const { return m_lbox; }
    virtual void RecreateWidget() { CreateLbox(); }

    // lazy creation of the content
    virtual void CreateContent();

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

    // reset the listbox parameters
    void Reset();

    // (re)create the listbox
    void CreateLbox();

    // read the value of a listbox item index from the given control, return
    // false if it's invalid
    bool GetValidIndexFromText(const wxTextCtrl *text, int *n = NULL) const;


    // listbox parameters
    // ------------------

    // the selection mode
    enum LboxSelection
    {
        LboxSel_Single,
        LboxSel_Extended,
        LboxSel_Multiple
    } m_lboxSelMode;

    // should it be sorted?
    bool m_sorted;

    // should it have horz scroll/vert scrollbar permanently shown?
    bool m_horzScroll,
         m_vertScrollAlways;

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioSelMode;

    // the checkboxes
    wxCheckBox *m_chkVScroll,
               *m_chkHScroll,
               *m_chkCheck,
               *m_chkSort,
               *m_chkOwnerDraw;

    // the listbox itself and the sizer it is in
#ifdef __WXWINCE__
    wxListBoxBase
#else
    wxListBox
#endif
                  *m_lbox;

    wxSizer *m_sizerLbox;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textAdd,
               *m_textChange,
               *m_textEnsureVisible,
               *m_textDelete;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ListboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ListboxWidgetsPage, WidgetsPage)
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

    EVT_LISTBOX(ListboxPage_Listbox, ListboxWidgetsPage::OnListbox)
    EVT_LISTBOX_DCLICK(ListboxPage_Listbox, ListboxWidgetsPage::OnListboxDClick)
    EVT_CHECKLISTBOX(ListboxPage_Listbox, ListboxWidgetsPage::OnCheckListbox)

    EVT_CHECKBOX(wxID_ANY, ListboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ListboxWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ListboxWidgetsPage, wxT("Listbox"),
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS
                       );

ListboxWidgetsPage::ListboxWidgetsPage(WidgetsBookCtrl *book,
                                       wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, listbox_xpm)
{
    // init everything
    m_radioSelMode = (wxRadioBox *)NULL;

    m_chkVScroll =
    m_chkHScroll =
    m_chkCheck =
    m_chkSort =
    m_chkOwnerDraw = (wxCheckBox *)NULL;

    m_lbox = NULL;
    m_sizerLbox = (wxSizer *)NULL;

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
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY,
        wxT("&Set listbox parameters"));
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    static const wxString modes[] =
    {
        wxT("single"),
        wxT("extended"),
        wxT("multiple"),
    };

    m_radioSelMode = new wxRadioBox(this, wxID_ANY, wxT("Selection &mode:"),
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(modes), modes,
                                    1, wxRA_SPECIFY_COLS);

    m_chkVScroll = CreateCheckBoxAndAddToSizer
                   (
                    sizerLeft,
                    wxT("Always show &vertical scrollbar")
                   );
    m_chkHScroll = CreateCheckBoxAndAddToSizer
                   (
                    sizerLeft,
                    wxT("Show &horizontal scrollbar")
                   );
    m_chkCheck = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("&Check list box"));
    m_chkSort = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("&Sort items"));
    m_chkOwnerDraw = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("&Owner drawn"));

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioSelMode, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, ListboxPage_Reset, wxT("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        wxT("&Change listbox contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ListboxPage_Add, wxT("&Add this string"));
    m_textAdd = new wxTextCtrl(this, ListboxPage_AddText, wxT("test item 0"));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textAdd, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ListboxPage_AddSeveral, wxT("&Insert a few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ListboxPage_AddMany, wxT("Add &many strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ListboxPage_Change, wxT("C&hange current"));
    m_textChange = new wxTextCtrl(this, ListboxPage_ChangeText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textChange, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ListboxPage_EnsureVisible, wxT("Make item &visible"));
    m_textEnsureVisible = new wxTextCtrl(this, ListboxPage_EnsureVisibleText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textEnsureVisible, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ListboxPage_Delete, wxT("&Delete this item"));
    m_textDelete = new wxTextCtrl(this, ListboxPage_DeleteText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ListboxPage_DeleteSel, wxT("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ListboxPage_Clear, wxT("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ListboxPage_ContainerTests, wxT("Run &tests"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_lbox = new wxListBox(this, ListboxPage_Listbox,
                           wxDefaultPosition, wxDefaultSize,
                           0, NULL,
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
    m_chkVScroll->SetValue(false);
    m_chkHScroll->SetValue(true);
    m_chkCheck->SetValue(false);
    m_chkSort->SetValue(false);
    m_chkOwnerDraw->SetValue(false);
}

void ListboxWidgetsPage::CreateLbox()
{
    int flags = ms_defaultFlags;
    switch ( m_radioSelMode->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unexpected radio box selection") );

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
    if ( m_lbox )
    {
        int count = m_lbox->GetCount();
        for ( int n = 0; n < count; n++ )
        {
            items.Add(m_lbox->GetString(n));
        }

        m_sizerLbox->Detach( m_lbox );
        delete m_lbox;
    }

#if wxUSE_CHECKLISTBOX
    if ( m_chkCheck->GetValue() )
    {
        m_lbox = new wxCheckListBox(this, ListboxPage_Listbox,
                                    wxDefaultPosition, wxDefaultSize,
                                    0, NULL,
                                    flags);
    }
    else // just a listbox
#endif
    {
        m_lbox = new wxListBox(this, ListboxPage_Listbox,
                               wxDefaultPosition, wxDefaultSize,
                               0, NULL,
                               flags);
    }

    m_lbox->Set(items);
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

void ListboxWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format(wxT("test item %u"), ++s_item));
    }

    m_lbox->Append(s);
}

void ListboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_lbox->Append(wxString::Format(wxT("item #%u"), n));
    }
}

void ListboxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString items;
    items.Add(wxT("First"));
    items.Add(wxT("another one"));
    items.Add(wxT("and the last (very very very very very very very very very very long) one"));
    m_lbox->InsertItems(items, 0);
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

void ListboxWidgetsPage::OnListbox(wxCommandEvent& event)
{
    long sel = event.GetSelection();
    m_textDelete->SetValue(wxString::Format(wxT("%ld"), sel));

    if (event.IsSelection())
    {
        wxLogMessage(wxT("Listbox item %ld selected"), sel);
    }
    else
    {
        wxLogMessage(wxT("Listbox item %ld deselected"), sel);
    }
}

void ListboxWidgetsPage::OnListboxDClick(wxCommandEvent& event)
{
    wxLogMessage( wxT("Listbox item %d double clicked"), event.GetInt() );
}

void ListboxWidgetsPage::OnCheckListbox(wxCommandEvent& event)
{
    wxLogMessage( wxT("Listbox item %d toggled"), event.GetInt() );
}

void ListboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateLbox();
}

#endif // wxUSE_LISTBOX
