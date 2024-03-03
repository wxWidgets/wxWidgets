/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        bmpcombobox.cpp
// Purpose:     Part of the widgets sample showing wxBitmapComboBox
// Author:      Jaakko Salli
// Created:     Sep-01-2006
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


#if wxUSE_BITMAPCOMBOBOX

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
    #include "wx/filedlg.h"
#endif

#include "wx/stattext.h"
#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/sizer.h"
#include "wx/icon.h"
#include "wx/dir.h"
#include "wx/msgdlg.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/bmpcbox.h"

#include "itemcontainer.h"
#include "widgets.h"

#include "icons/bmpcombobox.xpm"

// Images loaded from file are reduced this width and height, if larger
#define IMG_SIZE_TRUNC  256


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    BitmapComboBoxPage_Reset = wxID_HIGHEST,
    BitmapComboBoxPage_Insert,
    BitmapComboBoxPage_InsertText,
    BitmapComboBoxPage_ChangeHeight,
    BitmapComboBoxPage_LoadFromFile,
    BitmapComboBoxPage_SetFromFile,
    BitmapComboBoxPage_AddWidgetIcons,
    BitmapComboBoxPage_AddSeveralWithImages,
    BitmapComboBoxPage_AddSeveral,
    BitmapComboBoxPage_AddMany,
    BitmapComboBoxPage_Clear,
    BitmapComboBoxPage_Change,
    BitmapComboBoxPage_Delete,
    BitmapComboBoxPage_DeleteText,
    BitmapComboBoxPage_DeleteSel,
    BitmapComboBoxPage_Combo,
    BitmapComboBoxPage_ContainerTests
};

// kinds of comboboxes
enum
{
    ComboKind_Default,
    ComboKind_Simple,
    ComboKind_DropDown
};

// ----------------------------------------------------------------------------
// BitmapComboBoxWidgetsPage
// ----------------------------------------------------------------------------

class BitmapComboBoxWidgetsPage : public ItemContainerWidgetsPage
{
public:
    BitmapComboBoxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const override { return m_combobox; }
    virtual wxItemContainer* GetContainer() const override { return m_combobox; }
    virtual void RecreateWidget() override { CreateCombo(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonInsert(wxCommandEvent &event);
    void OnTextChangeHeight(wxCommandEvent& event);
    void OnButtonLoadFromFile(wxCommandEvent& event);
    void OnButtonSetFromFile(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddSeveralWithImages(wxCommandEvent& event);
    void OnButtonAddWidgetIcons(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);

    void OnComboBox(wxCommandEvent& event);
    void OnDropDown(wxCommandEvent& event);
    void OnCloseUp(wxCommandEvent& event);
    void OnComboText(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnTextPopupWidth(wxCommandEvent& event);
    void OnTextPopupHeight(wxCommandEvent& event);
    void OnTextButtonAll(wxCommandEvent& event);

    void OnUpdateUIInsert(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIItemManipulator(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the bmpcombobox parameters
    void Reset();

    // (re)create the bmpcombobox
    void CreateCombo();

    // helpers for creating bitmaps
    wxBitmap CreateBitmap(const wxColour& colour);
    wxBitmap LoadBitmap(const wxString& filepath);
    wxBitmap QueryBitmap(wxString* pStr);

    void LoadWidgetImages( wxArrayString* strings, wxImageList* images );

    wxSizer *CreateSizerWithSmallTextAndLabel(const wxString& label,
                                              wxWindowID id,
                                              wxTextCtrl **ppText,
                                              wxWindow* parent = nullptr);

#if wxUSE_IMAGE
    void RescaleImage(wxImage& image, int w, int h);
#endif

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioKind;

    // the checkboxes for styles
    wxCheckBox *m_chkSort,
               *m_chkProcessEnter,
               *m_chkReadonly;

    // the combobox itself and the sizer it is in
    wxBitmapComboBox *m_combobox;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textInsert,
               *m_textChangeHeight,
               *m_textChange,
               *m_textDelete;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(BitmapComboBoxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(BitmapComboBoxWidgetsPage, WidgetsPage)
    EVT_BUTTON(BitmapComboBoxPage_Reset, BitmapComboBoxWidgetsPage::OnButtonReset)
    EVT_BUTTON(BitmapComboBoxPage_Change, BitmapComboBoxWidgetsPage::OnButtonChange)
    EVT_BUTTON(BitmapComboBoxPage_Delete, BitmapComboBoxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(BitmapComboBoxPage_DeleteSel, BitmapComboBoxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(BitmapComboBoxPage_Clear, BitmapComboBoxWidgetsPage::OnButtonClear)
    EVT_BUTTON(BitmapComboBoxPage_Insert, BitmapComboBoxWidgetsPage::OnButtonInsert)
    EVT_BUTTON(BitmapComboBoxPage_AddSeveral, BitmapComboBoxWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(BitmapComboBoxPage_AddSeveralWithImages, BitmapComboBoxWidgetsPage::OnButtonAddSeveralWithImages)
    EVT_BUTTON(BitmapComboBoxPage_AddWidgetIcons, BitmapComboBoxWidgetsPage::OnButtonAddWidgetIcons)
    EVT_BUTTON(BitmapComboBoxPage_AddMany, BitmapComboBoxWidgetsPage::OnButtonAddMany)
    EVT_BUTTON(BitmapComboBoxPage_LoadFromFile, BitmapComboBoxWidgetsPage::OnButtonLoadFromFile)
    EVT_BUTTON(BitmapComboBoxPage_SetFromFile, BitmapComboBoxWidgetsPage::OnButtonSetFromFile)
    EVT_BUTTON(BitmapComboBoxPage_ContainerTests, ItemContainerWidgetsPage::OnButtonTestItemContainer)

    EVT_TEXT_ENTER(BitmapComboBoxPage_InsertText, BitmapComboBoxWidgetsPage::OnButtonInsert)
    EVT_TEXT(BitmapComboBoxPage_ChangeHeight, BitmapComboBoxWidgetsPage::OnTextChangeHeight)
    EVT_TEXT_ENTER(BitmapComboBoxPage_DeleteText, BitmapComboBoxWidgetsPage::OnButtonDelete)

    EVT_UPDATE_UI(BitmapComboBoxPage_Reset, BitmapComboBoxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Insert, BitmapComboBoxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(BitmapComboBoxPage_LoadFromFile, BitmapComboBoxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(BitmapComboBoxPage_Clear, BitmapComboBoxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_DeleteText, BitmapComboBoxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Delete, BitmapComboBoxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Change, BitmapComboBoxWidgetsPage::OnUpdateUIItemManipulator)
    EVT_UPDATE_UI(BitmapComboBoxPage_SetFromFile, BitmapComboBoxWidgetsPage::OnUpdateUIItemManipulator)
    EVT_UPDATE_UI(BitmapComboBoxPage_DeleteSel, BitmapComboBoxWidgetsPage::OnUpdateUIItemManipulator)

    EVT_COMBOBOX_DROPDOWN(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnDropDown)
    EVT_COMBOBOX_CLOSEUP(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnCloseUp)
    EVT_COMBOBOX(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboBox)
    EVT_TEXT(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboText)
    EVT_TEXT_ENTER(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboText)

    EVT_CHECKBOX(wxID_ANY, BitmapComboBoxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, BitmapComboBoxWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMSW__) || defined(__WXGTK__)
    #define NATIVE_OR_GENERIC_CTRLS     NATIVE_CTRLS
#else
    #define NATIVE_OR_GENERIC_CTRLS     GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(BitmapComboBoxWidgetsPage, "BitmapCombobox",
                       NATIVE_OR_GENERIC_CTRLS | WITH_ITEMS_CTRLS | COMBO_CTRLS
                       );


BitmapComboBoxWidgetsPage::BitmapComboBoxWidgetsPage(WidgetsBookCtrl *book,
                                             wxImageList *imaglist)
                  : ItemContainerWidgetsPage(book, imaglist, bmpcombobox_xpm)
{
    // init everything
    m_chkSort =
    m_chkProcessEnter =
    m_chkReadonly = nullptr;

    m_combobox = nullptr;
    m_sizerCombo = nullptr;

    m_textInsert =
    m_textChangeHeight =
    m_textChange =
    m_textDelete = nullptr;
}

// create a sizer containing a label and a small text ctrl
wxSizer *BitmapComboBoxWidgetsPage::CreateSizerWithSmallTextAndLabel(const wxString& label,
                                                                    wxWindowID id,
                                                                    wxTextCtrl **ppText,
                                                                    wxWindow* parent)
{
    wxControl* control = new wxStaticText(parent ? parent : this, wxID_ANY, label);
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    wxTextCtrl *text = new wxTextCtrl(parent ? parent : this, id, wxEmptyString,
        wxDefaultPosition, wxSize(50,wxDefaultCoord), wxTE_PROCESS_ENTER);

    sizerRow->Add(control, 0, wxRIGHT | wxALIGN_CENTRE_VERTICAL, 5);
    sizerRow->Add(text, 1, wxFIXED_MINSIZE | wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    if ( ppText )
        *ppText = text;

    return sizerRow;
}

void BitmapComboBoxWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous combobox operations and the pane containing the combobox
       itself to the right
    */
    wxSizer *sizerRow;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);

    // left pane - style
    wxStaticBoxSizer *sizerStyle = new wxStaticBoxSizer(wxVERTICAL, this, "&Set style");
    wxStaticBox* const sizerStyleBox = sizerStyle->GetStaticBox();


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

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerStyle, "&Sort items", wxID_ANY, sizerStyleBox);
    m_chkProcessEnter = CreateCheckBoxAndAddToSizer(sizerStyle, "Process &Enter",wxID_ANY, sizerStyleBox);
    m_chkReadonly = CreateCheckBoxAndAddToSizer(sizerStyle, "&Read only", wxID_ANY, sizerStyleBox);

    wxButton *btn = new wxButton(sizerStyleBox, BitmapComboBoxPage_Reset, "&Reset");
    sizerStyle->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 3);

    sizerLeft->Add(sizerStyle, wxSizerFlags().Expand());
    sizerLeft->Add(m_radioKind, 0, wxGROW | wxALL, 5);

    // left pane - other options
    wxStaticBoxSizer *sizerOptions = new wxStaticBoxSizer(wxVERTICAL, this, "Demo options");

    sizerRow = CreateSizerWithSmallTextAndLabel("Control &height:",
                                                BitmapComboBoxPage_ChangeHeight,
                                                &m_textChangeHeight,
                                                sizerOptions->GetStaticBox());
    m_textChangeHeight->SetSize(20, wxDefaultCoord);
    sizerOptions->Add(sizerRow, 0, wxALL | wxFIXED_MINSIZE /*| wxGROW*/, 5);

    sizerLeft->Add( sizerOptions, wxSizerFlags().Expand().Border(wxTOP, 2));

    // middle pane
    wxStaticBoxSizer *sizerMiddle = new wxStaticBoxSizer(wxVERTICAL, this, "&Change wxBitmapComboBox contents");
    wxStaticBox* const sizerMiddleBox = sizerMiddle->GetStaticBox();

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_ContainerTests, "Run &tests");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

#if wxUSE_IMAGE
    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_AddWidgetIcons, "Add &widget icons");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_LoadFromFile, "Insert image from &file");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_SetFromFile, "&Set image from file");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);
#endif

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_AddSeveralWithImages, "A&ppend a few strings with images");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_AddSeveral, "Append a &few strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_AddMany, "Append &many strings");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(BitmapComboBoxPage_Delete,
                                            "&Delete this item",
                                            BitmapComboBoxPage_DeleteText,
                                            &m_textDelete,
                                            sizerMiddleBox);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_DeleteSel, "Delete &selection");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(sizerMiddleBox, BitmapComboBoxPage_Clear, "&Clear");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

#if wxUSE_IMAGE
    wxInitAllImageHandlers();
#endif

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new wxBitmapComboBox();
    m_combobox->Create(this, BitmapComboBoxPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, nullptr,
                       // Flags correspond to the checkboxes state in Reset().
                       wxTE_PROCESS_ENTER);

#if defined(wxGENERIC_BITMAPCOMBOBOX)
    // This will sure make the list look nicer when larger images are used.
    m_combobox->SetPopupMaxHeight(600);
#endif

    sizerRight->Add(m_combobox, 0, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerCombo = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 5, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 4, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void BitmapComboBoxWidgetsPage::Reset()
{
    m_chkSort->SetValue(false);
    m_chkProcessEnter->SetValue(true);
    m_chkReadonly->SetValue(false);
}

void BitmapComboBoxWidgetsPage::CreateCombo()
{
    int flags = GetAttrs().m_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkProcessEnter->GetValue() )
        flags |= wxTE_PROCESS_ENTER;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;

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
            flags = wxCB_DROPDOWN;
            break;
    }

    wxArrayString items;
    wxArrayPtrVoid bitmaps;
    if ( m_combobox )
    {
        unsigned int count = m_combobox->GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            items.Add(m_combobox->GetString(n));
            wxBitmap bmp = m_combobox->GetItemBitmap(n);
            bitmaps.Add(new wxBitmap(bmp));
        }

        m_sizerCombo->Detach( m_combobox );
        delete m_combobox;
    }

    m_combobox = new wxBitmapComboBox();
    m_combobox->Create(this, BitmapComboBoxPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, nullptr,
                       flags);

#if defined(wxGENERIC_BITMAPCOMBOBOX)
    // This will sure make the list look nicer when larger images are used.
    m_combobox->SetPopupMaxHeight(600);
#endif

    NotifyWidgetRecreation(m_combobox);

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        wxBitmap* bmp = (wxBitmap*) bitmaps[n];
        m_combobox->Append(items[n], *bmp);
        delete bmp;
    }

    m_sizerCombo->Add(m_combobox, 0, wxGROW | wxALL, 5);
    m_sizerCombo->Layout();

    // Allow changing height in order to demonstrate flexible
    // size of image "thumbnail" painted in the control itself.
    long h = 0;
    m_textChangeHeight->GetValue().ToLong(&h);
    if ( h >= 5 )
        m_combobox->SetSize(wxDefaultCoord, h);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void BitmapComboBoxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateCombo();
}

void BitmapComboBoxWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
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

void BitmapComboBoxWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= m_combobox->GetCount()) )
    {
        return;
    }

    m_combobox->Delete(n);
}

void BitmapComboBoxWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != wxNOT_FOUND )
    {
        m_combobox->Delete(sel);
    }
}

void BitmapComboBoxWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Clear();
}

void BitmapComboBoxWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    static unsigned int s_item = 0;

    wxString s = m_textInsert->GetValue();
    if ( !m_textInsert->IsModified() )
    {
        // update the default string
        m_textInsert->SetValue(wxString::Format("test item %u", ++s_item));
    }

    m_combobox->Insert(s, wxNullBitmap, m_combobox->GetSelection());
}

void BitmapComboBoxWidgetsPage::OnTextChangeHeight(wxCommandEvent& WXUNUSED(event))
{
    long h = 0;
    if ( m_textChangeHeight )
        m_textChangeHeight->GetValue().ToLong(&h);
    if ( h < 5 )
        return;
    m_combobox->SetSize(wxDefaultCoord, h);
}

void BitmapComboBoxWidgetsPage::OnButtonLoadFromFile(wxCommandEvent& WXUNUSED(event))
{
    wxString s;
    int sel = m_combobox->GetSelection();
    if ( sel == wxNOT_FOUND )
        sel = m_combobox->GetCount();

    wxBitmap bmp = QueryBitmap(&s);
    if (bmp.IsOk())
        m_combobox->Insert(s, bmp, sel);
}

void BitmapComboBoxWidgetsPage::OnButtonSetFromFile(wxCommandEvent& WXUNUSED(event))
{
    wxBitmap bmp = QueryBitmap(nullptr);
    if (bmp.IsOk())
        m_combobox->SetItemBitmap(m_combobox->GetSelection(), bmp);
}

void BitmapComboBoxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format("item #%u", n));
    }
}

void BitmapComboBoxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Append("First");
    m_combobox->Append("another one");
    m_combobox->Append("and the last (very very very very very very very very very very long) one");
}

void BitmapComboBoxWidgetsPage::OnButtonAddSeveralWithImages(wxCommandEvent& WXUNUSED(event))
{
    static const struct TestEntry
    {
        const char *text;
        unsigned long rgb;
    } s_entries[] =
    {
        { "Red circle",     0x0000ff },
        { "Blue circle",    0xff0000 },
        { "Green circle",   0x00ff00 },
        { "Black circle",   0x000000 },
    };

    for ( unsigned i = 0; i < WXSIZEOF(s_entries); i++ )
    {
        const TestEntry& e = s_entries[i];
        m_combobox->Append(e.text, CreateBitmap(wxColour(e.rgb)));
    }
}

#if wxUSE_IMAGE
void BitmapComboBoxWidgetsPage::RescaleImage(wxImage& image, int w, int h)
{
    if ( image.GetWidth() == w && image.GetHeight() == h )
        return;

    if ( w <= 0 || h <= 0 )
        return;

    static bool isFirstScale = true;

    if ( isFirstScale && m_combobox->GetCount() > 0 )
    {
        wxMessageBox( "wxBitmapComboBox normally only supports images of one size. "
                      "However, for demonstration purposes, loaded bitmaps are scaled to fit "
                      "using wxImage::Rescale.",
                      "Notice",
                      wxOK,
                      this );

        isFirstScale = false;
    }

    image.Rescale(w, h);
}
#endif

void BitmapComboBoxWidgetsPage::LoadWidgetImages( wxArrayString* strings, wxImageList* images )
{
    wxFileName fn;
    fn.AssignCwd();
    fn.AppendDir("icons");

    wxSetCursor(*wxHOURGLASS_CURSOR);

    if ( !wxDir::Exists(fn.GetFullPath()) ||
         !wxDir::GetAllFiles(fn.GetFullPath(),strings,"*.xpm") )
    {
        // Try ../../samples/widgets/icons
        fn.RemoveLastDir();
        fn.RemoveLastDir();
        fn.AppendDir("icons");
        if ( !wxDir::Exists(fn.GetFullPath()) ||
             !wxDir::GetAllFiles(fn.GetFullPath(),strings,"*.xpm") )
        {
            // Try ../../../samples/widgets/icons
            fn.AssignCwd();
            fn.RemoveLastDir();
            fn.RemoveLastDir();
            fn.RemoveLastDir();
            fn.AppendDir("samples");
            fn.AppendDir("widgets");
            fn.AppendDir("icons");
            if ( !wxDir::Exists(fn.GetFullPath()) ||
                 !wxDir::GetAllFiles(fn.GetFullPath(),strings,"*.xpm") )
            {
                wxLogWarning("Could not load widget icons.");
                wxSetCursor(*wxSTANDARD_CURSOR);
                return;
            }
        }
    }

    unsigned int i;

    // Get size of existing images in list
    wxSize foundSize = m_combobox->GetBitmapSize();
    if ( !foundSize.IsFullySpecified() )
        foundSize = images->GetSize();

    for ( i=0; i<strings->size(); i++ )
    {
        fn.SetFullName((*strings)[i]);
        wxString name = fn.GetName();

        // Handle few exceptions
        if ( name == "bmpbtn" )
        {
            strings->RemoveAt(i);
            i--;
        }
        else
        {
#if wxUSE_IMAGE
            wxASSERT(fn.FileExists());
            wxImage image(fn.GetFullPath());
            wxASSERT(image.IsOk());
            RescaleImage(image, foundSize.x, foundSize.y);
            wxBitmap bmp(image);
            wxASSERT( bmp.IsOk() );
#else
            wxBitmap bmp;
#endif
            images->Add(bmp);
            (*strings)[i] = name;

            // if the combobox is empty, use as bitmap size of the image list
            // the size of the first valid image loaded
            if (foundSize == wxDefaultSize)
                foundSize = bmp.GetSize();
        }
    }

    wxSetCursor(*wxSTANDARD_CURSOR);
}

void BitmapComboBoxWidgetsPage::OnButtonAddWidgetIcons(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString strings;

    wxSize sz = m_combobox->GetBitmapSize();
    if ( sz.x <= 0 )
    {
        sz.x = 32;
        sz.y = 32;
    }

    wxImageList images(sz.x, sz.y);

    LoadWidgetImages(&strings, &images);

    unsigned int i;

    for ( i=0; i<strings.size(); i++ )
    {
        m_combobox->Append(strings[i], images.GetBitmap(i));
    }
}

void BitmapComboBoxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable( m_chkSort->GetValue()
                        || !m_chkProcessEnter->GetValue()
                            || m_chkReadonly->GetValue() );
}

void BitmapComboBoxWidgetsPage::OnUpdateUIInsert(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
        bool enable = !(m_combobox->GetWindowStyle() & wxCB_SORT);

        event.Enable(enable);
    }
}

void BitmapComboBoxWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
    {
      unsigned long n;
      event.Enable(m_textDelete->GetValue().ToULong(&n) &&
        (n < (unsigned)m_combobox->GetCount()));
    }
}

void BitmapComboBoxWidgetsPage::OnUpdateUIItemManipulator(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetSelection() != wxNOT_FOUND);
}

void BitmapComboBoxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetCount() != 0);
}

void BitmapComboBoxWidgetsPage::OnComboText(wxCommandEvent& event)
{
    if (!m_combobox)
        return;

    wxString s = event.GetString();

    wxASSERT_MSG( s == m_combobox->GetValue(),
                  "event and combobox values should be the same" );

    if (event.GetEventType() == wxEVT_TEXT_ENTER)
    {
        wxLogMessage("BitmapCombobox enter pressed (now '%s')", s);
    }
    else
    {
        wxLogMessage("BitmapCombobox text changed (now '%s')", s);
    }
}

void BitmapComboBoxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    long sel = event.GetInt();
    m_textDelete->SetValue(wxString::Format("%ld", sel));

    wxLogMessage("BitmapCombobox item %ld selected", sel);

    wxLogMessage("BitmapCombobox GetValue(): %s", m_combobox->GetValue() );
}

void BitmapComboBoxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateCombo();
}

#if wxUSE_IMAGE
wxBitmap BitmapComboBoxWidgetsPage::LoadBitmap(const wxString& filepath)
{
    // Get size of existing images in list
    wxSize foundSize = m_combobox->GetBitmapSize();

    // Have some reasonable maximum size
    if ( foundSize.x <= 0 )
    {
        foundSize.x = IMG_SIZE_TRUNC;
        foundSize.y = IMG_SIZE_TRUNC;
    }

    wxImage image(filepath);
    if ( image.IsOk() )
    {
        // Rescale very large images
        int ow = image.GetWidth();
        int oh = image.GetHeight();

        if ( foundSize.x > 0 &&
             (ow != foundSize.x || oh != foundSize.y) )
        {
            int w = ow;
            if ( w > foundSize.x )
                w = foundSize.x;
            int h = oh;
            if ( h > foundSize.y )
                h = foundSize.y;

            RescaleImage(image, w, h);
        }

        return wxBitmap(image);
    }

    return wxNullBitmap;
}
#else
wxBitmap BitmapComboBoxWidgetsPage::LoadBitmap(const wxString& WXUNUSED(filepath))
{
    return wxNullBitmap;
}
#endif

wxBitmap BitmapComboBoxWidgetsPage::QueryBitmap(wxString* pStr)
{
    wxString filepath = wxLoadFileSelector("image",
                                       wxEmptyString,
                                       wxEmptyString,
                                       this);

    wxBitmap bitmap;

    ::wxSetCursor( *wxHOURGLASS_CURSOR );

    if ( !filepath.empty() )
    {
        if ( pStr )
        {
            *pStr = wxFileName(filepath).GetName();
        }

        bitmap = LoadBitmap(filepath);
    }

    if (bitmap.IsOk())
    {
        wxLogDebug("%i, %i",bitmap.GetWidth(), bitmap.GetHeight());
    }

    ::wxSetCursor( *wxSTANDARD_CURSOR );

    return bitmap;
}

wxBitmap BitmapComboBoxWidgetsPage::CreateBitmap(const wxColour& colour)
{
    const int w = 10,
              h = 10;

    wxMemoryDC dc;
    wxBitmap bmp(w, h);
    dc.SelectObject(bmp);

    // Draw transparent background
    wxColour magic(255, 0, 255);
    wxBrush magicBrush(magic);
    dc.SetBrush(magicBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, w, h);

    // Draw image content
    dc.SetBrush(wxBrush(colour));
    dc.DrawCircle(h/2, h/2+1, h/2);

    dc.SelectObject(wxNullBitmap);

    // Finalize transparency with a mask
    wxMask *mask = new wxMask(bmp, magic);
    bmp.SetMask(mask);

    return bmp;
}

void BitmapComboBoxWidgetsPage::OnDropDown(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox dropped down");
}

void BitmapComboBoxWidgetsPage::OnCloseUp(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Combobox closed up");
}

#endif // wxUSE_BITMAPCOMBOBOX
