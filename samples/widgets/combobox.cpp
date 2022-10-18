/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        combobox.cpp
// Purpose:     Part of the widgets sample showing wxComboBox
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


#if wxUSE_COMBOBOX

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/combobox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "itemcontainer.h"
#include "widgets.h"
#include "icons/combobox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ComboPage_Reset = wxID_HIGHEST,
    ComboPage_Popup,
    ComboPage_Dismiss,
    ComboPage_SetCurrent,
    ComboPage_CurText,
    ComboPage_InsertionPointText,
    ComboPage_Insert,
    ComboPage_InsertText,
    ComboPage_Add,
    ComboPage_AddText,
    ComboPage_SetFirst,
    ComboPage_SetFirstText,
    ComboPage_AddSeveral,
    ComboPage_AddMany,
    ComboPage_Clear,
    ComboPage_Change,
    ComboPage_ChangeText,
    ComboPage_Delete,
    ComboPage_DeleteText,
    ComboPage_DeleteSel,
    ComboPage_SetValue,
    ComboPage_SetValueText,
    ComboPage_Combo,
    ComboPage_ContainerTests,
    ComboPage_Dynamic
};

// kinds of comboboxes
enum
{
    ComboKind_Default,
    ComboKind_Simple,
    ComboKind_DropDown
};

// ----------------------------------------------------------------------------
// ComboboxWidgetsPage
// ----------------------------------------------------------------------------

class ComboboxWidgetsPage : public ItemContainerWidgetsPage
{
public:
    ComboboxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const override { return m_combobox; }
    virtual wxTextEntryBase *GetTextEntry() const override { return m_combobox; }
    virtual wxItemContainer* GetContainer() const override { return m_combobox; }
    virtual void RecreateWidget() override { CreateCombo(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonPopup(wxCommandEvent&) { m_combobox->Popup(); }
    void OnButtonDismiss(wxCommandEvent&) { m_combobox->Dismiss(); }
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonInsert(wxCommandEvent &event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonSetFirst(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetCurrent(wxCommandEvent& event);

    void OnDropdown(wxCommandEvent& event);
    void OnCloseup(wxCommandEvent& event);
    void OnPopup(wxCommandEvent &event);
    void OnDismiss(wxCommandEvent &event);
    void OnComboBox(wxCommandEvent& event);
    void OnComboText(wxCommandEvent& event);
    void OnComboTextPasted(wxClipboardTextEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIInsertionPointText(wxUpdateUIEvent& event);

    void OnUpdateUIInsert(wxUpdateUIEvent& event);
    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);
    void OnUpdateUISetCurrent(wxUpdateUIEvent& event);

    // reset the combobox parameters
    void Reset();

    // (re)create the combobox
    void CreateCombo();

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioKind;

    // the checkboxes for styles
    wxCheckBox *m_chkSort,
               *m_chkReadonly,
               *m_chkProcessEnter;

    // the combobox itself and the sizer it is in
    wxComboBox *m_combobox, *m_combobox1;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textInsert,
               *m_textAdd,
               *m_textSetFirst,
               *m_textChange,
               *m_textSetValue,
               *m_textDelete,
               *m_textCur;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ComboboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ComboboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(ComboPage_Reset, ComboboxWidgetsPage::OnButtonReset)
    EVT_BUTTON(ComboPage_Popup, ComboboxWidgetsPage::OnButtonPopup)
    EVT_BUTTON(ComboPage_Dismiss, ComboboxWidgetsPage::OnButtonDismiss)
    EVT_BUTTON(ComboPage_Change, ComboboxWidgetsPage::OnButtonChange)
    EVT_BUTTON(ComboPage_Delete, ComboboxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ComboPage_DeleteSel, ComboboxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ComboPage_Clear, ComboboxWidgetsPage::OnButtonClear)
    EVT_BUTTON(ComboPage_Insert, ComboboxWidgetsPage::OnButtonInsert)
    EVT_BUTTON(ComboPage_Add, ComboboxWidgetsPage::OnButtonAdd)
    EVT_BUTTON(ComboPage_SetFirst, ComboboxWidgetsPage::OnButtonSetFirst)
    EVT_BUTTON(ComboPage_AddSeveral, ComboboxWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(ComboPage_AddMany, ComboboxWidgetsPage::OnButtonAddMany)
    EVT_BUTTON(ComboPage_SetValue, ComboboxWidgetsPage::OnButtonSetValue)
    EVT_BUTTON(ComboPage_SetCurrent, ComboboxWidgetsPage::OnButtonSetCurrent)
    EVT_BUTTON(ComboPage_ContainerTests, ItemContainerWidgetsPage::OnButtonTestItemContainer)

    EVT_TEXT_ENTER(ComboPage_InsertText, ComboboxWidgetsPage::OnButtonInsert)
    EVT_TEXT_ENTER(ComboPage_AddText, ComboboxWidgetsPage::OnButtonAdd)
    EVT_TEXT_ENTER(ComboPage_DeleteText, ComboboxWidgetsPage::OnButtonDelete)

    EVT_UPDATE_UI(ComboPage_InsertionPointText, ComboboxWidgetsPage::OnUpdateUIInsertionPointText)

    EVT_UPDATE_UI(ComboPage_Reset, ComboboxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(ComboPage_Insert, ComboboxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(ComboPage_AddSeveral, ComboboxWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(ComboPage_Clear, ComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ComboPage_DeleteText, ComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ComboPage_Delete, ComboboxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(ComboPage_Change, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ComboPage_ChangeText, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ComboPage_DeleteSel, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ComboPage_SetCurrent, ComboboxWidgetsPage::OnUpdateUISetCurrent)

    EVT_COMBOBOX(ComboPage_Combo, ComboboxWidgetsPage::OnComboBox)
    EVT_COMBOBOX_DROPDOWN(ComboPage_Combo, ComboboxWidgetsPage::OnDropdown)
    EVT_COMBOBOX_CLOSEUP(ComboPage_Combo, ComboboxWidgetsPage::OnCloseup)
    EVT_TEXT(ComboPage_Combo, ComboboxWidgetsPage::OnComboText)
    EVT_TEXT_ENTER(ComboPage_Combo, ComboboxWidgetsPage::OnComboText)
    EVT_TEXT_PASTE(ComboPage_Combo, ComboboxWidgetsPage::OnComboTextPasted)

    EVT_CHECKBOX(wxID_ANY, ComboboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ComboboxWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ComboboxWidgetsPage, "Combobox",
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS | COMBO_CTRLS
                       );

ComboboxWidgetsPage::ComboboxWidgetsPage(WidgetsBookCtrl *book,
                                         wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, combobox_xpm)
{
    // init everything
    m_chkSort =
    m_chkReadonly =
    m_chkProcessEnter = nullptr;

    m_combobox = nullptr;
    m_sizerCombo = nullptr;
}

void ComboboxWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous combobox operations and the pane containing the combobox
       itself to the right
    */
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // upper left pane

    // should be in sync with ComboKind_XXX values
    static const wxString kinds[] =
    {
        "default",
        "simple",
        "drop down",
    };

    m_radioKind = new wxRadioBox(this, wxID_ANY, "Combobox &kind:",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);

    wxSizer *sizerLeftTop = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerLeftTop, "&Sort items");
    m_chkReadonly = CreateCheckBoxAndAddToSizer(sizerLeftTop, "&Read only");
    m_chkProcessEnter = CreateCheckBoxAndAddToSizer(sizerLeftTop, "Process &Enter");

    sizerLeftTop->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeftTop->Add(m_radioKind, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, ComboPage_Reset, "&Reset");
    sizerLeftTop->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // lower left pane
    wxSizer *sizerLeftBottom = new wxStaticBoxSizer(wxVERTICAL, this, "&Popup");
    sizerLeftBottom->Add(new wxButton(this, ComboPage_Popup, "&Show"),
                         wxSizerFlags().Border().Centre());
    sizerLeftBottom->Add(new wxButton(this, ComboPage_Dismiss, "&Hide"),
                         wxSizerFlags().Border().Centre());


    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
    sizerLeft->Add(sizerLeftTop);
    sizerLeft->AddSpacer(10);
    sizerLeft->Add(sizerLeftBottom, wxSizerFlags().Expand());

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        "&Change combobox contents");
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow;

    sizerRow = CreateSizerWithTextAndButton(ComboPage_SetCurrent,
                                            "Current &selection",
                                            ComboPage_CurText,
                                            &m_textCur);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    wxTextCtrl *text;
    sizerRow = CreateSizerWithTextAndLabel("Insertion Point",
                                           ComboPage_InsertionPointText,
                                           &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Insert,
                                            "&Insert this string",
                                            ComboPage_InsertText,
                                            &m_textInsert);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Add,
                                            "&Add this string",
                                            ComboPage_AddText,
                                            &m_textAdd);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_SetFirst,
                                            "Change &1st string",
                                            ComboPage_SetFirstText,
                                            &m_textSetFirst);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddSeveral, "&Append a few strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddMany, "Append &many strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Change,
                                            "C&hange current",
                                            ComboPage_ChangeText,
                                            &m_textChange);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Delete,
                                            "&Delete this item",
                                            ComboPage_DeleteText,
                                            &m_textDelete);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_DeleteSel, "Delete &selection");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_Clear, "&Clear");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_SetValue,
                                            "SetValue",
                                            ComboPage_SetValueText,
                                            &m_textSetValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_ContainerTests, "Run &tests");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);



    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new wxComboBox(this, ComboPage_Combo, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                0, nullptr,
                                0);
    sizerRight->Add(m_combobox, 0, wxGROW | wxALL, 5);
    m_combobox1 = new wxComboBox( this, ComboPage_Dynamic );
    m_combobox1->Append( "Dynamic ComboBox Test - Click me!" );
    m_combobox1->SetSelection( 0 );
    sizerRight->Add( 20, 20, 0, wxEXPAND, 0 );
    sizerRight->Add( m_combobox1, 0, wxGROW | wxALL, 5 );
    m_combobox1->Bind( wxEVT_COMBOBOX_DROPDOWN, &ComboboxWidgetsPage::OnPopup, this );
    m_combobox1->Bind( wxEVT_COMBOBOX_CLOSEUP, &ComboboxWidgetsPage::OnDismiss, this );
    sizerRight->SetMinSize(150, 0);
    m_sizerCombo = sizerRight; // save it to modify it later

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

void ComboboxWidgetsPage::Reset()
{
    m_chkSort->SetValue(false);
    m_chkReadonly->SetValue(false);
    m_chkProcessEnter->SetValue(false);
}

void ComboboxWidgetsPage::CreateCombo()
{
    wxCHECK_RET( m_combobox, "No combo box exists" );

    int flags = GetAttrs().m_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;
    if ( m_chkProcessEnter->GetValue() )
        flags |= wxTE_PROCESS_ENTER;


    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unknown combo kind" );
            wxFALLTHROUGH;

        case ComboKind_Default:
            break;

        case ComboKind_Simple:
            flags |= wxCB_SIMPLE;
            break;

        case ComboKind_DropDown:
            flags |= wxCB_DROPDOWN;
            break;
    }

    wxArrayString items;
    unsigned int count = m_combobox->GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
         items.Add(m_combobox->GetString(n));
    }
    int selItem  = m_combobox->GetSelection();

    wxComboBox* newCb = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                items,
                                flags);

    if ( selItem != wxNOT_FOUND )
        newCb->SetSelection(selItem);

    m_sizerCombo->Replace(m_combobox, newCb);
    m_sizerCombo->Layout();

    delete m_combobox;
    m_combobox = newCb;
    m_combobox->SetId(ComboPage_Combo);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ComboboxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateCombo();
}

void ComboboxWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != wxNOT_FOUND )
    {
        m_combobox->SetString(sel, m_textChange->GetValue());
    }
}

void ComboboxWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= m_combobox->GetCount()) )
    {
        return;
    }

    m_combobox->Delete(n);
}

void ComboboxWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != wxNOT_FOUND )
    {
        m_combobox->Delete(sel);
    }
}

void ComboboxWidgetsPage::OnButtonSetValue(wxCommandEvent& WXUNUSED(event))
{
    wxString value = m_textSetValue->GetValue();
    m_combobox->SetValue( value );
}

void ComboboxWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Clear();
}

void ComboboxWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textInsert->GetValue();
    if ( !m_textInsert->IsModified() )
    {
        // update the default string
        m_textInsert->SetValue(wxString::Format("test item %u", ++s_item));
    }

    if (m_combobox->GetSelection() >= 0)
        m_combobox->Insert(s, m_combobox->GetSelection());
}

void ComboboxWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format("test item %u", ++s_item));
    }

    m_combobox->Append(s);
}

void ComboboxWidgetsPage::OnButtonSetFirst(wxCommandEvent& WXUNUSED(event))
{
    if ( m_combobox->IsListEmpty() )
    {
        wxLogWarning("No string to change.");
        return;
    }

    m_combobox->SetString(0, m_textSetFirst->GetValue());
}

void ComboboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format("item #%u", n));
    }
}

void ComboboxWidgetsPage::OnButtonSetCurrent(wxCommandEvent& WXUNUSED(event))
{
    long n;
    if ( !m_textCur->GetValue().ToLong(&n) )
        return;

    m_combobox->SetSelection(n);
}

void ComboboxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Append("First");
    m_combobox->Append("another one");
    m_combobox->Append("and the last (very very very very very very very very very very long) one");
}

void ComboboxWidgetsPage::OnUpdateUIInsertionPointText(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.SetText( wxString::Format("%ld", m_combobox->GetInsertionPoint()) );
}

void ComboboxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkSort->GetValue() ||
                    m_chkReadonly->GetValue() ||
                        m_chkProcessEnter->GetValue() );
}

void ComboboxWidgetsPage::OnUpdateUIInsert(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
        bool enable = !(m_combobox->GetWindowStyle() & wxCB_SORT) &&
                       (m_combobox->GetSelection() >= 0);

        event.Enable(enable);
    }
}

void ComboboxWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
      unsigned long n;
      event.Enable(m_textDelete->GetValue().ToULong(&n) &&
        (n < (unsigned)m_combobox->GetCount()));
    }
}

void ComboboxWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetSelection() != wxNOT_FOUND);
}

void ComboboxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetCount() != 0);
}

void ComboboxWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(!(m_combobox->GetWindowStyle() & wxCB_SORT));
}

void ComboboxWidgetsPage::OnUpdateUISetCurrent(wxUpdateUIEvent& event)
{
    long n;
    event.Enable( m_textCur->GetValue().ToLong(&n) &&
                    (n == wxNOT_FOUND ||
                        (n >= 0 && (unsigned)n < m_combobox->GetCount())) );
}

void ComboboxWidgetsPage::OnComboText(wxCommandEvent& event)
{
    if (!m_combobox)
        return;

    wxString s = event.GetString();

    wxASSERT_MSG( s == m_combobox->GetValue(),
                  "event and combobox values should be the same" );

    if (event.GetEventType() == wxEVT_TEXT_ENTER)
    {
        wxLogMessage("Combobox enter pressed (now '%s')", s);
    }
    else
    {
        wxLogMessage("Combobox text changed (now '%s')", s);
    }
}

void ComboboxWidgetsPage::OnComboTextPasted(wxClipboardTextEvent& event)
{
    wxLogMessage("Text pasted from clipboard.");
    event.Skip();
}

void ComboboxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    long sel = event.GetInt();
    const wxString selstr = wxString::Format("%ld", sel);
    m_textDelete->SetValue(selstr);
    m_textCur->SetValue(selstr);

    wxLogMessage("Combobox item %ld selected", sel);

    wxLogMessage("Combobox GetValue(): %s", m_combobox->GetValue() );

    if ( event.GetString() != m_combobox->GetValue() )
    {
        wxLogMessage("ERROR: Event has different string \"%s\"",
                     event.GetString());
    }
}

void ComboboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateCombo();
}

void ComboboxWidgetsPage::OnDropdown(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox dropped down");
}

void ComboboxWidgetsPage::OnCloseup(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox closed up");
}

void ComboboxWidgetsPage::OnPopup(wxCommandEvent &WXUNUSED(event))
{
    m_combobox1->Clear();
    m_combobox1->Append( "Selection 1" );
    m_combobox1->Append( "Selection 2" );
    m_combobox1->Append( "Selection 3" );
    wxLogMessage("The number of items is %d", m_combobox1->GetCount());
}

void ComboboxWidgetsPage::OnDismiss(wxCommandEvent &WXUNUSED(event))
{
    if ( m_combobox1->GetSelection() == wxNOT_FOUND )
    {
        m_combobox1->Clear();
        m_combobox1->Append( "Dynamic ComboBox Test - Click me!" );
        m_combobox1->SetSelection( 0 );
    }
    wxLogMessage("The number of items is %d", m_combobox1->GetCount());
}

#endif // wxUSE_COMBOBOX
