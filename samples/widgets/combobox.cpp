/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        combobox.cpp
// Purpose:     Part of the widgets sample showing wxComboBox
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
    ComboPage_ContainerTests
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

    virtual wxControl *GetWidget() const { return m_combobox; }
    virtual wxTextEntryBase *GetTextEntry() const { return m_combobox; }
    virtual wxItemContainer* GetContainer() const { return m_combobox; }
    virtual void RecreateWidget() { CreateCombo(); }

    // lazy creation of the content
    virtual void CreateContent();

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
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetCurrent(wxCommandEvent& event);

    void OnDropdown(wxCommandEvent& event);
    void OnCloseup(wxCommandEvent& event);
    void OnComboBox(wxCommandEvent& event);
    void OnComboText(wxCommandEvent& event);

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
               *m_chkFilename;

    // the combobox itself and the sizer it is in
    wxComboBox *m_combobox;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textInsert,
               *m_textAdd,
               *m_textChange,
               *m_textSetValue,
               *m_textDelete,
               *m_textCur;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ComboboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ComboboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(ComboPage_Reset, ComboboxWidgetsPage::OnButtonReset)
    EVT_BUTTON(ComboPage_Popup, ComboboxWidgetsPage::OnButtonPopup)
    EVT_BUTTON(ComboPage_Dismiss, ComboboxWidgetsPage::OnButtonDismiss)
    EVT_BUTTON(ComboPage_Change, ComboboxWidgetsPage::OnButtonChange)
    EVT_BUTTON(ComboPage_Delete, ComboboxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ComboPage_DeleteSel, ComboboxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ComboPage_Clear, ComboboxWidgetsPage::OnButtonClear)
    EVT_BUTTON(ComboPage_Insert, ComboboxWidgetsPage::OnButtonInsert)
    EVT_BUTTON(ComboPage_Add, ComboboxWidgetsPage::OnButtonAdd)
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

    EVT_CHECKBOX(wxID_ANY, ComboboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ComboboxWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ComboboxWidgetsPage, wxT("Combobox"),
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS | COMBO_CTRLS
                       );

ComboboxWidgetsPage::ComboboxWidgetsPage(WidgetsBookCtrl *book,
                                         wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, combobox_xpm)
{
    // init everything
    m_chkSort =
    m_chkReadonly =
    m_chkFilename = (wxCheckBox *)NULL;

    m_combobox = (wxComboBox *)NULL;
    m_sizerCombo = (wxSizer *)NULL;
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
        wxT("default"),
        wxT("simple"),
        wxT("drop down"),
    };

    m_radioKind = new wxRadioBox(this, wxID_ANY, wxT("Combobox &kind:"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);

    wxSizer *sizerLeftTop = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerLeftTop, wxT("&Sort items"));
    m_chkReadonly = CreateCheckBoxAndAddToSizer(sizerLeftTop, wxT("&Read only"));
    m_chkFilename = CreateCheckBoxAndAddToSizer(sizerLeftTop, wxT("&File name"));
    m_chkFilename->Disable(); // not implemented yet

    sizerLeftTop->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeftTop->Add(m_radioKind, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, ComboPage_Reset, wxT("&Reset"));
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
    sizerLeft->Add(sizerLeftBottom);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        wxT("&Change combobox contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow;

    sizerRow = CreateSizerWithTextAndButton(ComboPage_SetCurrent,
                                            wxT("Current &selection"),
                                            ComboPage_CurText,
                                            &m_textCur);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    wxTextCtrl *text;
    sizerRow = CreateSizerWithTextAndLabel(wxT("Insertion Point"),
                                           ComboPage_InsertionPointText,
                                           &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Insert,
                                            wxT("&Insert this string"),
                                            ComboPage_InsertText,
                                            &m_textInsert);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Add,
                                            wxT("&Add this string"),
                                            ComboPage_AddText,
                                            &m_textAdd);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddSeveral, wxT("&Append a few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddMany, wxT("Append &many strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Change,
                                            wxT("C&hange current"),
                                            ComboPage_ChangeText,
                                            &m_textChange);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_Delete,
                                            wxT("&Delete this item"),
                                            ComboPage_DeleteText,
                                            &m_textDelete);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_DeleteSel, wxT("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_Clear, wxT("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ComboPage_SetValue,
                                            wxT("SetValue"),
                                            ComboPage_SetValueText,
                                            &m_textSetValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_ContainerTests, wxT("Run &tests"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);



    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new wxComboBox(this, ComboPage_Combo, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                0, NULL,
                                0);
    sizerRight->Add(m_combobox, 0, wxGROW | wxALL, 5);
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
    m_chkFilename->SetValue(false);
}

void ComboboxWidgetsPage::CreateCombo()
{
    int flags = ms_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;

    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unknown combo kind") );
            // fall through

        case ComboKind_Default:
            break;

        case ComboKind_Simple:
            flags |= wxCB_SIMPLE;
            break;

        case ComboKind_DropDown:
            flags = wxCB_DROPDOWN;
            break;
    }

    wxArrayString items;
    if ( m_combobox )
    {
        unsigned int count = m_combobox->GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            items.Add(m_combobox->GetString(n));
        }

        m_sizerCombo->Detach( m_combobox );
        delete m_combobox;
    }

    m_combobox = new wxComboBox(this, ComboPage_Combo, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                0, NULL,
                                flags);

#if 0
    if ( m_chkFilename->GetValue() )
        ;
#endif // TODO

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_combobox->Append(items[n]);
    }

    m_sizerCombo->Add(m_combobox, 0, wxGROW | wxALL, 5);
    m_sizerCombo->Layout();
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
#ifndef __WXGTK__
        m_combobox->SetString(sel, m_textChange->GetValue());
#else
        wxLogMessage(wxT("Not implemented in wxGTK"));
#endif
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
        m_textInsert->SetValue(wxString::Format(wxT("test item %u"), ++s_item));
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
        m_textAdd->SetValue(wxString::Format(wxT("test item %u"), ++s_item));
    }

    m_combobox->Append(s);
}

void ComboboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format(wxT("item #%u"), n));
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
    m_combobox->Append(wxT("First"));
    m_combobox->Append(wxT("another one"));
    m_combobox->Append(wxT("and the last (very very very very very very very very very very long) one"));
}

void ComboboxWidgetsPage::OnUpdateUIInsertionPointText(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.SetText( wxString::Format(wxT("%ld"), m_combobox->GetInsertionPoint()) );
}

void ComboboxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkSort->GetValue() ||
                    m_chkReadonly->GetValue() ||
                        m_chkFilename->GetValue() );
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
                  wxT("event and combobox values should be the same") );

    if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
    {
        wxLogMessage(wxT("Combobox enter pressed (now '%s')"), s.c_str());
    }
    else
    {
        wxLogMessage(wxT("Combobox text changed (now '%s')"), s.c_str());
    }
}

void ComboboxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    long sel = event.GetInt();
    const wxString selstr = wxString::Format(wxT("%ld"), sel);
    m_textDelete->SetValue(selstr);
    m_textCur->SetValue(selstr);

    wxLogMessage(wxT("Combobox item %ld selected"), sel);

    wxLogMessage(wxT("Combobox GetValue(): %s"), m_combobox->GetValue().c_str() );
}

void ComboboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateCombo();
}

void ComboboxWidgetsPage::OnDropdown(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(wxT("Combobox dropped down"));
}

void ComboboxWidgetsPage::OnCloseup(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(wxT("Combobox closed up"));
}

#endif // wxUSE_COMBOBOX
