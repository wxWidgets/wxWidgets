/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        odcombobox.cpp
// Purpose:     Part of the widgets sample showing wxOwnerDrawnComboBox
// Author:      Jaakko Salli (based on combobox page by Vadim Zeitlin)
// Created:     Jul-28-2006
// Copyright:   (c) 2006 Jaakko Salli
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


#if wxUSE_ODCOMBOBOX

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

#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/sizer.h"
#include "wx/odcombo.h"


#include "itemcontainer.h"
#include "widgets.h"

#include "icons/odcombobox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ODComboPage_Reset = wxID_HIGHEST,
    ODComboPage_PopupMinWidth,
    ODComboPage_PopupHeight,
    ODComboPage_ButtonWidth,
    ODComboPage_ButtonHeight,
    ODComboPage_ButtonSpacing,
    ODComboPage_CurText,
    ODComboPage_InsertionPointText,
    ODComboPage_Insert,
    ODComboPage_InsertText,
    ODComboPage_Add,
    ODComboPage_AddText,
    ODComboPage_AddSeveral,
    ODComboPage_AddMany,
    ODComboPage_Clear,
    ODComboPage_Change,
    ODComboPage_ChangeText,
    ODComboPage_Delete,
    ODComboPage_DeleteText,
    ODComboPage_DeleteSel,
    ODComboPage_Combo,
    ODComboPage_ContainerTests
};


// ----------------------------------------------------------------------------
// ODComboboxWidgetsPage
// ----------------------------------------------------------------------------

class ODComboboxWidgetsPage : public ItemContainerWidgetsPage
{
public:
    ODComboboxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_combobox; }
    virtual wxTextEntryBase *GetTextEntry() const wxOVERRIDE
        { return m_combobox ? m_combobox->GetTextCtrl() : NULL; }
    virtual wxItemContainer* GetContainer() const wxOVERRIDE { return m_combobox; }
    virtual void RecreateWidget() wxOVERRIDE { CreateCombo(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonInsert(wxCommandEvent &event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);

    void OnComboBox(wxCommandEvent& event);
    void OnDropDown(wxCommandEvent& event);
    void OnCloseUp(wxCommandEvent& event);
    void OnComboText(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnTextPopupWidth(wxCommandEvent& event);
    void OnTextPopupHeight(wxCommandEvent& event);
    void OnTextButtonAll(wxCommandEvent& event);

    void OnUpdateUICurText(wxUpdateUIEvent& event);
    void OnUpdateUIInsertionPointText(wxUpdateUIEvent& event);

    void OnUpdateUIInsert(wxUpdateUIEvent& event);
    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the odcombobox parameters
    void Reset();

    // (re)create the odcombobox
    void CreateCombo();

    // helper that gets all button values from controls and calls SetButtonPosition
    void GetButtonPosition();

    // helper to create the button bitmap
    wxBitmap CreateBitmap(const wxColour& colour);

    // the controls
    // ------------

    // the checkboxes for styles
    wxCheckBox *m_chkSort,
               *m_chkReadonly,
               *m_chkDclickcycles,
               *m_chkBitmapbutton,
               *m_chkStdbutton;

    // the text entries for popup and button adjustment
    wxTextCtrl *m_textPopupMinWidth,
               *m_textPopupHeight,
               *m_textButtonWidth,
               *m_textButtonHeight,
               *m_textButtonSpacing;

    // the checkboxes for same
    wxCheckBox *m_chkAlignpopupright,
               *m_chkAlignbutleft;

    // the combobox itself and the sizer it is in
    wxOwnerDrawnComboBox *m_combobox;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textInsert,
               *m_textAdd,
               *m_textChange,
               *m_textDelete;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ODComboboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ODComboboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(ODComboPage_Reset, ODComboboxWidgetsPage::OnButtonReset)
    EVT_BUTTON(ODComboPage_Change, ODComboboxWidgetsPage::OnButtonChange)
    EVT_BUTTON(ODComboPage_Delete, ODComboboxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ODComboPage_DeleteSel, ODComboboxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ODComboPage_Clear, ODComboboxWidgetsPage::OnButtonClear)
    EVT_BUTTON(ODComboPage_Insert, ODComboboxWidgetsPage::OnButtonInsert)
    EVT_BUTTON(ODComboPage_Add, ODComboboxWidgetsPage::OnButtonAdd)
    EVT_BUTTON(ODComboPage_AddSeveral, ODComboboxWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(ODComboPage_AddMany, ODComboboxWidgetsPage::OnButtonAddMany)
    EVT_BUTTON(ODComboPage_ContainerTests, ItemContainerWidgetsPage::OnButtonTestItemContainer)

    EVT_TEXT_ENTER(ODComboPage_InsertText, ODComboboxWidgetsPage::OnButtonInsert)
    EVT_TEXT_ENTER(ODComboPage_AddText, ODComboboxWidgetsPage::OnButtonAdd)
    EVT_TEXT_ENTER(ODComboPage_DeleteText, ODComboboxWidgetsPage::OnButtonDelete)

    EVT_TEXT(ODComboPage_PopupMinWidth, ODComboboxWidgetsPage::OnTextPopupWidth)
    EVT_TEXT(ODComboPage_PopupHeight, ODComboboxWidgetsPage::OnTextPopupHeight)
    EVT_TEXT(ODComboPage_ButtonWidth, ODComboboxWidgetsPage::OnTextButtonAll)
    EVT_TEXT(ODComboPage_ButtonHeight, ODComboboxWidgetsPage::OnTextButtonAll)
    EVT_TEXT(ODComboPage_ButtonSpacing, ODComboboxWidgetsPage::OnTextButtonAll)

    EVT_UPDATE_UI(ODComboPage_CurText, ODComboboxWidgetsPage::OnUpdateUICurText)
    EVT_UPDATE_UI(ODComboPage_InsertionPointText, ODComboboxWidgetsPage::OnUpdateUIInsertionPointText)

    EVT_UPDATE_UI(ODComboPage_Reset, ODComboboxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(ODComboPage_Insert, ODComboboxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(ODComboPage_AddSeveral, ODComboboxWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(ODComboPage_Clear, ODComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ODComboPage_DeleteText, ODComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ODComboPage_Delete, ODComboboxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(ODComboPage_Change, ODComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ODComboPage_ChangeText, ODComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ODComboPage_DeleteSel, ODComboboxWidgetsPage::OnUpdateUIDeleteSelButton)

    EVT_COMBOBOX_DROPDOWN(ODComboPage_Combo, ODComboboxWidgetsPage::OnDropDown)
    EVT_COMBOBOX_CLOSEUP(ODComboPage_Combo, ODComboboxWidgetsPage::OnCloseUp)
    EVT_COMBOBOX(ODComboPage_Combo, ODComboboxWidgetsPage::OnComboBox)
    EVT_TEXT(ODComboPage_Combo, ODComboboxWidgetsPage::OnComboText)
    EVT_TEXT_ENTER(ODComboPage_Combo, ODComboboxWidgetsPage::OnComboText)

    EVT_CHECKBOX(wxID_ANY, ODComboboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ODComboboxWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

//
// wxOwnerDrawnComboBox needs to subclassed so that owner-drawing
// callbacks can be implemented.
class DemoODComboBox : public wxOwnerDrawnComboBox
{
public:
    virtual void OnDrawItem(wxDC& dc,
                            const wxRect& rect,
                            int item,
                            int WXUNUSED(flags)) const wxOVERRIDE
    {
        if ( item == wxNOT_FOUND )
            return;

        wxColour txtCol;
        int mod = item % 4;

        if ( mod == 0 )
            txtCol = *wxBLACK;
        else if ( mod == 1 )
            txtCol = *wxRED;
        else if ( mod == 2 )
            txtCol = *wxGREEN;
        else
            txtCol = *wxBLUE;

        dc.SetTextForeground(txtCol);

        dc.DrawText(GetString(item),
                    rect.x + 3,
                    rect.y + ((rect.height - dc.GetCharHeight())/2)
                   );
    }

    virtual void OnDrawBackground(wxDC& dc, const wxRect& rect,
                                  int item, int flags ) const wxOVERRIDE
    {

        // If item is selected or even, or we are painting the
        // combo control itself, use the default rendering.
        if ( (flags & (wxODCB_PAINTING_CONTROL|wxODCB_PAINTING_SELECTED)) ||
             (item & 1) == 0 )
        {
            wxOwnerDrawnComboBox::OnDrawBackground(dc,rect,item,flags);
            return;
        }

        // Otherwise, draw every other background with different colour.
        wxColour bgCol(240,240,250);
        dc.SetBrush(wxBrush(bgCol));
        dc.SetPen(wxPen(bgCol));
        dc.DrawRectangle(rect);
    }

    virtual wxCoord OnMeasureItem(size_t WXUNUSED(item)) const wxOVERRIDE
    {
        return 48;
    }

    virtual wxCoord OnMeasureItemWidth(size_t WXUNUSED(item)) const wxOVERRIDE
    {
        return -1; // default - will be measured from text width
    }
};


IMPLEMENT_WIDGETS_PAGE(ODComboboxWidgetsPage, "OwnerDrawnCombobox",
                       GENERIC_CTRLS | WITH_ITEMS_CTRLS | COMBO_CTRLS
                       );

ODComboboxWidgetsPage::ODComboboxWidgetsPage(WidgetsBookCtrl *book,
                                             wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, odcombobox_xpm)
{
    // init everything
    m_chkSort =
    m_chkReadonly =
    m_chkDclickcycles = (wxCheckBox *)NULL;

    m_combobox = (wxOwnerDrawnComboBox *)NULL;
    m_sizerCombo = (wxSizer *)NULL;
}

void ODComboboxWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous combobox operations and the pane containing the combobox
       itself to the right
    */
    wxTextCtrl *text;
    wxSizer *sizerRow;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);

    // left pane - style box
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Set style");

    wxSizer *sizerStyle = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerStyle, "&Sort items");
    m_chkReadonly = CreateCheckBoxAndAddToSizer(sizerStyle, "&Read only");
    m_chkDclickcycles = CreateCheckBoxAndAddToSizer(sizerStyle, "&Double-click Cycles");

    sizerStyle->AddSpacer(4);

    m_chkBitmapbutton = CreateCheckBoxAndAddToSizer(sizerStyle, "&Bitmap button");
    m_chkStdbutton = CreateCheckBoxAndAddToSizer(sizerStyle, "B&lank button background");

    wxButton *btn = new wxButton(this, ODComboPage_Reset, "&Reset");
    sizerStyle->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 3);

    sizerLeft->Add(sizerStyle, wxSizerFlags().Expand());

    // left pane - popup adjustment box
    box = new wxStaticBox(this, wxID_ANY, "Adjust &popup");

    wxSizer *sizerPopupPos = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerRow = CreateSizerWithTextAndLabel("Min. Width:",
                                           ODComboPage_PopupMinWidth,
                                           &m_textPopupMinWidth);
    m_textPopupMinWidth->SetValue("-1");
    sizerPopupPos->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("Max. Height:",
                                           ODComboPage_PopupHeight,
                                           &m_textPopupHeight);
    m_textPopupHeight->SetValue("-1");
    sizerPopupPos->Add(sizerRow, 0, wxALL | wxGROW, 5);

    m_chkAlignpopupright = CreateCheckBoxAndAddToSizer(sizerPopupPos, "Align Right");

    sizerLeft->Add(sizerPopupPos, wxSizerFlags().Expand().Border(wxTOP, 2));

    // left pane - button adjustment box
    box = new wxStaticBox(this, wxID_ANY, "Adjust &button");

    wxSizer *sizerButtonPos = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerRow = CreateSizerWithTextAndLabel("Width:",
                                           ODComboPage_ButtonWidth,
                                           &m_textButtonWidth);
    m_textButtonWidth->SetValue("-1");
    sizerButtonPos->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("VSpacing:",
                                           ODComboPage_ButtonSpacing,
                                           &m_textButtonSpacing);
    m_textButtonSpacing->SetValue("0");
    sizerButtonPos->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("Height:",
                                           ODComboPage_ButtonHeight,
                                           &m_textButtonHeight);
    m_textButtonHeight->SetValue("-1");
    sizerButtonPos->Add(sizerRow, 0, wxALL | wxGROW, 5);

    m_chkAlignbutleft = CreateCheckBoxAndAddToSizer(sizerButtonPos, "Align Left");

    sizerLeft->Add(sizerButtonPos, wxSizerFlags().Expand().Border(wxTOP, 2));

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        "&Change combobox contents");
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    btn = new wxButton(this, ODComboPage_ContainerTests, "Run &tests");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("Current selection",
                                           ODComboPage_CurText,
                                           &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("Insertion Point",
                                           ODComboPage_InsertionPointText,
                                           &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ODComboPage_Insert,
                                            "&Insert this string",
                                            ODComboPage_InsertText,
                                            &m_textInsert);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ODComboPage_Add,
                                            "&Add this string",
                                            ODComboPage_AddText,
                                            &m_textAdd);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ODComboPage_AddSeveral, "&Append a few strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ODComboPage_AddMany, "Append &many strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ODComboPage_Change,
                                            "C&hange current",
                                            ODComboPage_ChangeText,
                                            &m_textChange);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(ODComboPage_Delete,
                                            "&Delete this item",
                                            ODComboPage_DeleteText,
                                            &m_textDelete);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ODComboPage_DeleteSel, "Delete &selection");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ODComboPage_Clear, "&Clear");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new DemoODComboBox();
    m_combobox->Create(this, ODComboPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, NULL,
                       0);
    sizerRight->Add(m_combobox, 0, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerCombo = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 4, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 5, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 4, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void ODComboboxWidgetsPage::Reset()
{
    m_chkSort->SetValue(false);
    m_chkReadonly->SetValue(false);
    m_chkDclickcycles->SetValue(false);
    m_chkDclickcycles->Enable(false);
    m_chkBitmapbutton->SetValue(false);
    m_chkStdbutton->SetValue(false);
    m_chkStdbutton->Enable(false);
}

void ODComboboxWidgetsPage::CreateCombo()
{
    int flags = GetAttrs().m_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;
    if ( m_chkDclickcycles->GetValue() )
        flags |= wxODCB_DCLICK_CYCLES;

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

    m_combobox = new DemoODComboBox();
    m_combobox->Create(this, ODComboPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, NULL,
                       flags);

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_combobox->Append(items[n]);
    }

    // Update from controls that edit popup position etc.

    wxUpdateUIEvent tempEvt;
    OnTextPopupWidth(tempEvt);
    OnTextPopupHeight(tempEvt);
    GetButtonPosition();

    m_combobox->SetPopupAnchor( m_chkAlignpopupright->GetValue() ? wxRIGHT : wxLEFT );

    if ( m_chkBitmapbutton->GetValue() )
    {
        wxBitmap bmpNormal = CreateBitmap(*wxBLUE);
        wxBitmap bmpPressed = CreateBitmap(wxColour(0,0,128));
        wxBitmap bmpHover = CreateBitmap(wxColour(128,128,255));
        m_combobox->SetButtonBitmaps(bmpNormal,m_chkStdbutton->GetValue(),bmpPressed,bmpHover);
    }

    m_sizerCombo->Add(m_combobox, 0, wxGROW | wxALL, 5);
    m_sizerCombo->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ODComboboxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateCombo();
}

void ODComboboxWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != wxNOT_FOUND )
    {
#ifndef __WXGTK__
        m_combobox->SetString(sel, m_textChange->GetValue());
#else
        wxLogMessage("Not implemented in wxGTK");
#endif
    }
}

void ODComboboxWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= m_combobox->GetCount()) )
    {
        return;
    }

    m_combobox->Delete(n);
}

void ODComboboxWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != wxNOT_FOUND )
    {
        m_combobox->Delete(sel);
    }
}

void ODComboboxWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Clear();
}

void ODComboboxWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
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

void ODComboboxWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
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

void ODComboboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format("item #%u", n));
    }
}

void ODComboboxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Append("First");
    m_combobox->Append("another one");
    m_combobox->Append("and the last (very very very very very very very very very very long) one");
}

void ODComboboxWidgetsPage::OnTextPopupWidth(wxCommandEvent& WXUNUSED(event))
{
    long l = 0;

    m_textPopupMinWidth->GetValue().ToLong(&l);

    if (m_combobox && l > 0)
    {
        m_combobox->SetPopupMinWidth(l);
    }
}

void ODComboboxWidgetsPage::OnTextPopupHeight(wxCommandEvent& WXUNUSED(event))
{
    long l = 0;

    m_textPopupHeight->GetValue().ToLong(&l);

    if (m_combobox && l > 0)
    {
        m_combobox->SetPopupMaxHeight(l);
    }
}

void ODComboboxWidgetsPage::GetButtonPosition()
{
    long w = -1;
    long h = -1;
    long spacing = 0;

    m_textButtonWidth->GetValue().ToLong(&w);
    m_textButtonSpacing->GetValue().ToLong(&spacing);
    m_textButtonHeight->GetValue().ToLong(&h);
    int align = m_chkAlignbutleft->GetValue() ?
                wxLEFT : wxRIGHT;

    m_combobox->SetButtonPosition(w,h,align,spacing);
}

void ODComboboxWidgetsPage::OnTextButtonAll(wxCommandEvent& WXUNUSED(event))
{
    if (m_combobox)
    {
        if ( m_chkBitmapbutton->GetValue() )
            CreateCombo();
        else
            GetButtonPosition();
    }
}

void ODComboboxWidgetsPage::OnUpdateUICurText(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.SetText( wxString::Format("%d", m_combobox->GetSelection()) );
}

void ODComboboxWidgetsPage::OnUpdateUIInsertionPointText(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.SetText( wxString::Format("%ld", m_combobox->GetInsertionPoint()) );
}

void ODComboboxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable( m_chkSort->GetValue() || m_chkReadonly->GetValue() ||
                      m_chkBitmapbutton->GetValue() );
}

void ODComboboxWidgetsPage::OnUpdateUIInsert(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
        bool enable = !(m_combobox->GetWindowStyle() & wxCB_SORT) &&
                       (m_combobox->GetSelection() >= 0);

        event.Enable(enable);
    }
}

void ODComboboxWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
      unsigned long n;
      event.Enable(m_textDelete->GetValue().ToULong(&n) &&
        (n < (unsigned)m_combobox->GetCount()));
    }
}

void ODComboboxWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetSelection() != wxNOT_FOUND);
}

void ODComboboxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetCount() != 0);
}

void ODComboboxWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(!(m_combobox->GetWindowStyle() & wxCB_SORT));
}

void ODComboboxWidgetsPage::OnComboText(wxCommandEvent& event)
{
    if (!m_combobox)
        return;

    wxString s = event.GetString();

    wxASSERT_MSG( s == m_combobox->GetValue(),
                  "event and combobox values should be the same" );

    if (event.GetEventType() == wxEVT_TEXT_ENTER)
    {
        wxLogMessage("OwnerDrawnCombobox enter pressed (now '%s')", s);
    }
    else
    {
        wxLogMessage("OwnerDrawnCombobox text changed (now '%s')", s);
    }
}

void ODComboboxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    long sel = event.GetInt();
    m_textDelete->SetValue(wxString::Format("%ld", sel));

    wxLogMessage("OwnerDrawnCombobox item %ld selected", sel);

    wxLogMessage("OwnerDrawnCombobox GetValue(): %s", m_combobox->GetValue() );
}

void ODComboboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    wxObject* ctrl = event.GetEventObject();

    // Double-click cycles only applies to read-only combobox
    if ( ctrl == (wxObject*) m_chkReadonly )
    {
        m_chkDclickcycles->Enable( m_chkReadonly->GetValue() );
    }
    else if ( ctrl == (wxObject*) m_chkBitmapbutton )
    {
        m_chkStdbutton->Enable( m_chkBitmapbutton->GetValue() );
    }
    else if ( ctrl == (wxObject*) m_chkAlignbutleft )
    {
        wxUpdateUIEvent tempEvt;
        OnTextButtonAll(tempEvt);
    }

    CreateCombo();
}

wxBitmap ODComboboxWidgetsPage::CreateBitmap(const wxColour& colour)
{
    int ch = m_combobox->GetClientSize().y - 1;
    int h0 = ch - 5;

    long w = -1;
    long h = -1;

    m_textButtonWidth->GetValue().ToLong(&w);
    m_textButtonHeight->GetValue().ToLong(&h);

    if ( w <= 0 )
        w = h0 - 1;
    if ( h <= 0 )
        h = h0;
    if ( h > ch )
        h = ch;

    wxMemoryDC dc;
    wxBitmap bmp(w,h);
    dc.SelectObject(bmp);

    // Draw transparent background
    wxColour magic(255,0,255);
    wxBrush magicBrush(magic);
    dc.SetBrush(magicBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0,0,bmp.GetWidth(),bmp.GetHeight());

    // Draw image content
    dc.SetBrush(wxBrush(colour));
    dc.DrawCircle(h/2,h/2+1,(h/2));

    dc.SelectObject(wxNullBitmap);

    // Finalize transparency with a mask
    wxMask *mask = new wxMask(bmp, magic);
    bmp.SetMask(mask);

    return bmp;
}

void ODComboboxWidgetsPage::OnDropDown(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox dropped down");
}

void ODComboboxWidgetsPage::OnCloseUp(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox closed up");
}

#endif //wxUSE_ODCOMBOBOX
