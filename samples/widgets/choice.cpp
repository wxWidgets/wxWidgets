/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        choice.cpp
// Purpose:     Part of the widgets sample showing wxChoice
// Created:     23.07.07
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

#if wxUSE_CHOICE

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/choice.h"
    #include "wx/combobox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "wx/checklst.h"

#include "itemcontainer.h"
#include "widgets.h"

#include "icons/choice.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ChoicePage_Reset = wxID_HIGHEST,
    ChoicePage_Add,
    ChoicePage_AddText,
    ChoicePage_AddSeveral,
    ChoicePage_AddMany,
    ChoicePage_Clear,
    ChoicePage_Change,
    ChoicePage_ChangeText,
    ChoicePage_Delete,
    ChoicePage_DeleteText,
    ChoicePage_DeleteSel,
    ChoicePage_Choice,
    ChoicePage_ContainerTests
};

// ----------------------------------------------------------------------------
// ChoiceWidgetsPage
// ----------------------------------------------------------------------------

class ChoiceWidgetsPage : public ItemContainerWidgetsPage
{
public:
    ChoiceWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_choice; }
    virtual wxItemContainer* GetContainer() const wxOVERRIDE { return m_choice; }
    virtual void RecreateWidget() wxOVERRIDE { CreateChoice(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);

    void OnChoice(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the choice parameters
    void Reset();

    // (re)create the choice
    void CreateChoice();

    // should it be sorted?
    bool m_sorted;

    // the controls
    // ------------

    // the checkboxes
    wxCheckBox *m_chkSort;

    // the choice itself and the sizer it is in
    wxChoice *m_choice;

    wxSizer *m_sizerChoice;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textAdd,
               *m_textChange,
               *m_textDelete;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ChoiceWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ChoiceWidgetsPage, WidgetsPage)
    EVT_BUTTON(ChoicePage_Reset, ChoiceWidgetsPage::OnButtonReset)
    EVT_BUTTON(ChoicePage_Change, ChoiceWidgetsPage::OnButtonChange)
    EVT_BUTTON(ChoicePage_Delete, ChoiceWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ChoicePage_DeleteSel, ChoiceWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ChoicePage_Clear, ChoiceWidgetsPage::OnButtonClear)
    EVT_BUTTON(ChoicePage_Add, ChoiceWidgetsPage::OnButtonAdd)
    EVT_BUTTON(ChoicePage_AddSeveral, ChoiceWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(ChoicePage_AddMany, ChoiceWidgetsPage::OnButtonAddMany)
    EVT_BUTTON(ChoicePage_ContainerTests, ItemContainerWidgetsPage::OnButtonTestItemContainer)

    EVT_TEXT_ENTER(ChoicePage_AddText, ChoiceWidgetsPage::OnButtonAdd)
    EVT_TEXT_ENTER(ChoicePage_DeleteText, ChoiceWidgetsPage::OnButtonDelete)

    EVT_UPDATE_UI(ChoicePage_Reset, ChoiceWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(ChoicePage_AddSeveral, ChoiceWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(ChoicePage_Clear, ChoiceWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ChoicePage_DeleteText, ChoiceWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ChoicePage_Delete, ChoiceWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(ChoicePage_Change, ChoiceWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ChoicePage_ChangeText, ChoiceWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ChoicePage_DeleteSel, ChoiceWidgetsPage::OnUpdateUIDeleteSelButton)

    EVT_CHOICE(ChoicePage_Choice, ChoiceWidgetsPage::OnChoice)

    EVT_CHECKBOX(wxID_ANY, ChoiceWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ChoiceWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ChoiceWidgetsPage, "Choice",
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS
                       );

ChoiceWidgetsPage::ChoiceWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, choice_xpm)
{
    // init everything

    m_chkSort = (wxCheckBox *)NULL;

    m_choice = NULL;
    m_sizerChoice = (wxSizer *)NULL;

}

void ChoiceWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous choice operations and the pane containing the choice
       itself to the right
    */
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY,
        "&Set choice parameters");
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerLeft, "&Sort items");

    wxButton *btn = new wxButton(this, ChoicePage_Reset, "&Reset");
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        "&Change choice contents");
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ChoicePage_Add, "&Add this string");
    m_textAdd = new wxTextCtrl(this, ChoicePage_AddText, "test item 0");
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textAdd, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ChoicePage_AddSeveral, "&Insert a few strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ChoicePage_AddMany, "Add &many strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ChoicePage_Change, "C&hange current");
    m_textChange = new wxTextCtrl(this, ChoicePage_ChangeText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textChange, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ChoicePage_Delete, "&Delete this item");
    m_textDelete = new wxTextCtrl(this, ChoicePage_DeleteText, wxEmptyString);
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ChoicePage_DeleteSel, "Delete &selection");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ChoicePage_Clear, "&Clear");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ChoicePage_ContainerTests, "Run &tests");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_choice = new wxChoice(this, ChoicePage_Choice);
    sizerRight->Add(m_choice, 0, wxALL | wxGROW, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerChoice = sizerRight; // save it to modify it later

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

void ChoiceWidgetsPage::Reset()
{
    m_chkSort->SetValue(false);
}

void ChoiceWidgetsPage::CreateChoice()
{
    int flags = GetAttrs().m_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;

    wxArrayString items;
    if ( m_choice )
    {
        int count = m_choice->GetCount();
        for ( int n = 0; n < count; n++ )
        {
            items.Add(m_choice->GetString(n));
        }

        m_sizerChoice->Detach( m_choice );
        delete m_choice;
    }

    m_choice = new wxChoice(this, ChoicePage_Choice,
                            wxDefaultPosition, wxDefaultSize,
                            0, NULL,
                            flags);

    m_choice->Set(items);
    m_sizerChoice->Add(m_choice, 0, wxGROW | wxALL, 5);
    m_sizerChoice->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ChoiceWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateChoice();
}

void ChoiceWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    int selection = m_choice->GetSelection();
    if(selection != wxNOT_FOUND)
    {
        m_choice->SetString(selection, m_textChange->GetValue());
    }
}

void ChoiceWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= (unsigned)m_choice->GetCount()) )
    {
        return;
    }

    m_choice->Delete(n);
}

void ChoiceWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    int selection = m_choice->GetSelection();
    if(selection != wxNOT_FOUND)
    {
        m_choice->Delete(selection);
    }
}

void ChoiceWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_choice->Clear();
}

void ChoiceWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format("test item %u", ++s_item));
    }

    m_choice->Append(s);
}

void ChoiceWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    wxArrayString strings;
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        strings.Add(wxString::Format("item #%u", n));
    }
    m_choice->Append(strings);
}

void ChoiceWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString items;
    items.Add("First");
    items.Add("another one");
    items.Add("and the last (very very very very very very very very very very long) one");
    m_choice->Insert(items, 0);
}

void ChoiceWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkSort->GetValue() );
}

void ChoiceWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable(m_textDelete->GetValue().ToULong(&n) &&
                    (n < (unsigned)m_choice->GetCount()));
}

void ChoiceWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    wxArrayInt selections;
    event.Enable(m_choice->GetSelection() != wxNOT_FOUND);
}

void ChoiceWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(m_choice->GetCount() != 0);
}

void ChoiceWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    event.Enable(!m_choice->HasFlag(wxCB_SORT));
}

void ChoiceWidgetsPage::OnChoice(wxCommandEvent& event)
{
    long sel = event.GetSelection();
    m_textDelete->SetValue(wxString::Format("%ld", sel));

    wxLogMessage("Choice item %ld selected", sel);
}

void ChoiceWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateChoice();
}

#endif // wxUSE_CHOICE
