/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        bmpcombobox.cpp
// Purpose:     Part of the widgets sample showing wxBitmapComboBox
// Author:      Jaakko Salli
// Created:     Sep-01-2006
// Id:          $Id:
// Copyright:   (c) 2006 Jaakko Salli
// License:     wxWindows license
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
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/bmpcbox.h"


#include "widgets.h"

#include "icons/odcombobox.xpm"

// Images loaded from file are reduced this width and height, if larger
#define IMG_SIZE_TRUNC  150


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
    BitmapComboBoxPage_Combo
};


// ----------------------------------------------------------------------------
// BitmapComboBoxWidgetsPage
// ----------------------------------------------------------------------------

class BitmapComboBoxWidgetsPage : public WidgetsPage
{
public:
    BitmapComboBoxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxControl *GetWidget() const { return m_combobox; }
    virtual void RecreateWidget() { CreateCombo(); }

    // lazy creation of the content
    virtual void CreateContent();

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
    void OnComboText(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnTextPopupWidth(wxCommandEvent& event);
    void OnTextPopupHeight(wxCommandEvent& event);
    void OnTextButtonAll(wxCommandEvent& event);

    void OnUpdateUIInsert(wxUpdateUIEvent& event);
    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIAddSeveralWithImages(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the bmpcombobox parameters
    void Reset();

    // (re)create the bmpcombobox
    void CreateCombo();

    // helpers for creating bitmaps
    wxBitmap CreateBitmap(const wxColour& colour);
    wxBitmap CreateRandomBitmap(wxString* pStr);
    wxBitmap LoadBitmap(const wxString& filepath);
    wxBitmap QueryBitmap(wxString* pStr);

    void LoadWidgetImages( wxArrayString* strings, wxImageList* images );

    wxSizer *CreateSizerWithSmallTextAndLabel(const wxString& label,
                                              wxWindowID id,
                                              wxTextCtrl **ppText);

    // the controls
    // ------------

    // the checkboxes for styles
    wxCheckBox *m_chkSort,
               *m_chkReadonly,
               *m_chkScaleimages;

    // the combobox itself and the sizer it is in
    wxBitmapComboBox *m_combobox;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textInsert,
               *m_textChangeHeight,
               *m_textChange,
               *m_textDelete;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(BitmapComboBoxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(BitmapComboBoxWidgetsPage, WidgetsPage)
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

    EVT_TEXT_ENTER(BitmapComboBoxPage_InsertText, BitmapComboBoxWidgetsPage::OnButtonInsert)
    EVT_TEXT(BitmapComboBoxPage_ChangeHeight, BitmapComboBoxWidgetsPage::OnTextChangeHeight)
    EVT_TEXT_ENTER(BitmapComboBoxPage_DeleteText, BitmapComboBoxWidgetsPage::OnButtonDelete)

    EVT_UPDATE_UI(BitmapComboBoxPage_Reset, BitmapComboBoxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Insert, BitmapComboBoxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(BitmapComboBoxPage_LoadFromFile, BitmapComboBoxWidgetsPage::OnUpdateUIInsert)
    EVT_UPDATE_UI(BitmapComboBoxPage_AddSeveral, BitmapComboBoxWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(BitmapComboBoxPage_AddSeveralWithImages, BitmapComboBoxWidgetsPage::OnUpdateUIAddSeveralWithImages)
    EVT_UPDATE_UI(BitmapComboBoxPage_Clear, BitmapComboBoxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_DeleteText, BitmapComboBoxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Delete, BitmapComboBoxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_Change, BitmapComboBoxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(BitmapComboBoxPage_DeleteSel, BitmapComboBoxWidgetsPage::OnUpdateUIDeleteSelButton)

    EVT_COMBOBOX(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboBox)
    EVT_TEXT(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboText)
    EVT_TEXT_ENTER(BitmapComboBoxPage_Combo, BitmapComboBoxWidgetsPage::OnComboText)

    EVT_CHECKBOX(wxID_ANY, BitmapComboBoxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, BitmapComboBoxWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================



IMPLEMENT_WIDGETS_PAGE(BitmapComboBoxWidgetsPage, _T("BitmapCombobox"),
                       GENERIC_CTRLS | WITH_ITEMS_CTRLS | COMBO_CTRLS
                       );


BitmapComboBoxWidgetsPage::BitmapComboBoxWidgetsPage(WidgetsBookCtrl *book,
                                             wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, odcombobox_xpm)
{
    // init everything
    m_chkSort =
    m_chkReadonly =
    m_chkScaleimages = (wxCheckBox *)NULL;

    m_combobox = (wxBitmapComboBox *)NULL;
    m_sizerCombo = (wxSizer *)NULL;
}

// create a sizer containing a label and a small text ctrl
wxSizer *BitmapComboBoxWidgetsPage::CreateSizerWithSmallTextAndLabel(const wxString& label,
                                                                    wxWindowID id,
                                                                    wxTextCtrl **ppText)
{
    wxControl* control = new wxStaticText(this, wxID_ANY, label);
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    wxTextCtrl *text = new wxTextCtrl(this, id, wxEmptyString,
        wxDefaultPosition, wxSize(50,-1), wxTE_PROCESS_ENTER);

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
    //wxTextCtrl *text;
    wxSizer *sizerRow;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);

    // left pane - style box
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    wxSizer *sizerStyle = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkSort = CreateCheckBoxAndAddToSizer(sizerStyle, _T("&Sort items"));
    m_chkReadonly = CreateCheckBoxAndAddToSizer(sizerStyle, _T("&Read only"));

    wxButton *btn = new wxButton(this, BitmapComboBoxPage_Reset, _T("&Reset"));
    sizerStyle->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 3);

    sizerLeft->Add(sizerStyle, 0, wxGROW | wxALIGN_CENTRE_HORIZONTAL);

    // left pane - other options box
    box = new wxStaticBox(this, wxID_ANY, _T("Demo options"));

    wxSizer *sizerOptions = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkScaleimages = CreateCheckBoxAndAddToSizer(sizerOptions, _T("&Scale loaded images to fit"));

    sizerRow = CreateSizerWithSmallTextAndLabel(_T("Control &height:"),
                                                BitmapComboBoxPage_ChangeHeight,
                                                &m_textChangeHeight);
    m_textChangeHeight->SetSize(20, -1);
    sizerOptions->Add(sizerRow, 0, wxALL | wxFIXED_MINSIZE /*| wxGROW*/, 5);

    sizerLeft->Add(sizerOptions, 0, wxGROW | wxALIGN_CENTRE_HORIZONTAL | wxTOP, 2);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY,
        _T("&Change wxBitmapComboBox contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

#if wxUSE_IMAGE
    btn = new wxButton(this, BitmapComboBoxPage_AddWidgetIcons, _T("Add &widget icons"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_LoadFromFile, _T("Insert image from &file"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_SetFromFile, _T("&Set image from file"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);
#endif

    btn = new wxButton(this, BitmapComboBoxPage_AddSeveralWithImages, _T("A&ppend a few strings with images"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_AddSeveral, _T("Append a &few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_AddMany, _T("Append &many strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(BitmapComboBoxPage_Delete,
                                            _T("&Delete this item"),
                                            BitmapComboBoxPage_DeleteText,
                                            &m_textDelete);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_DeleteSel, _T("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BitmapComboBoxPage_Clear, _T("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

#if wxUSE_IMAGE
    wxInitAllImageHandlers();
#endif

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new wxBitmapComboBox();
    m_combobox->Create(this, BitmapComboBoxPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, NULL,
                       wxCB_READONLY);

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

    sizerTop->Fit(this);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void BitmapComboBoxWidgetsPage::Reset()
{
    m_chkSort->SetValue(false);
    m_chkReadonly->SetValue(true);
    m_chkScaleimages->SetValue(true);
}

void BitmapComboBoxWidgetsPage::CreateCombo()
{
    int flags = ms_defaultFlags;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;

    wxArrayString items;
    wxArrayPtrVoid bitmaps;
    if ( m_combobox )
    {
        unsigned int count = m_combobox->GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            items.Add(m_combobox->GetString(n));
            bitmaps.Add(new wxBitmap(m_combobox->GetItemBitmap(n)));
        }

        m_sizerCombo->Detach( m_combobox );
        delete m_combobox;
    }

    m_combobox = new wxBitmapComboBox();
    m_combobox->Create(this, BitmapComboBoxPage_Combo, wxEmptyString,
                       wxDefaultPosition, wxDefaultSize,
                       0, NULL,
                       flags);

#if defined(wxGENERIC_BITMAPCOMBOBOX)
    // This will sure make the list look nicer when larger images are used.
    m_combobox->SetPopupMaxHeight(600);
#endif

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        wxBitmap* bmp = (wxBitmap*) bitmaps[n];
        m_combobox->Append(items[n], *bmp);
        delete bmp;
    }

    m_sizerCombo->Add(m_combobox, 0, wxGROW | wxALL, 5);
    m_sizerCombo->Layout();

    // Allow changing height inorder to demonstrate flexible
    // size of image "thumbnail" painted in the control itself.
    long h = 0;
    m_textChangeHeight->GetValue().ToLong(&h);
    if ( h >= 5 )
        m_combobox->SetSize(-1, h);
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
        wxLogMessage(_T("Not implemented in wxGTK"));
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
        m_textInsert->SetValue(wxString::Format(_T("test item %u"), ++s_item));
    }

    if (m_combobox->GetSelection() >= 0)
        m_combobox->Insert(s, wxNullBitmap, m_combobox->GetSelection());
}

void BitmapComboBoxWidgetsPage::OnTextChangeHeight(wxCommandEvent& WXUNUSED(event))
{
    long h = 0;
    m_textChangeHeight->GetValue().ToLong(&h);
    if ( h < 5 )
        return;
    m_combobox->SetSize(-1, h);
}

void BitmapComboBoxWidgetsPage::OnButtonLoadFromFile(wxCommandEvent& WXUNUSED(event))
{
    wxString s;
    m_combobox->Insert(s, QueryBitmap(&s), m_combobox->GetSelection());
}

void BitmapComboBoxWidgetsPage::OnButtonSetFromFile(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->SetItemBitmap(m_combobox->GetSelection(), QueryBitmap(NULL));
}

void BitmapComboBoxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( unsigned int n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format(_T("item #%u"), n));
    }
}

void BitmapComboBoxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& WXUNUSED(event))
{
    m_combobox->Append(_T("First"));
    m_combobox->Append(_T("another one"));
    m_combobox->Append(_T("and the last (very very very very very very very very very very long) one"));
}

void BitmapComboBoxWidgetsPage::OnButtonAddSeveralWithImages(wxCommandEvent& WXUNUSED(event))
{
    int i;

    for ( i=0; i<4; i++ )
    {
        wxString s;
        wxBitmap bmp = CreateRandomBitmap(&s);
        m_combobox->Append(s, bmp);
    }
}

void BitmapComboBoxWidgetsPage::LoadWidgetImages( wxArrayString* strings, wxImageList* images )
{
    wxFileName fn;
    fn.AssignCwd();
    fn.AppendDir(wxT("icons"));
    
    wxSetCursor(*wxHOURGLASS_CURSOR);

    if ( !wxDir::Exists(fn.GetFullPath()) ||
         !wxDir::GetAllFiles(fn.GetFullPath(),strings,wxT("*.xpm")) )
    {
        fn.RemoveLastDir();
        fn.RemoveLastDir();
        fn.AppendDir(wxT("icons"));
        if ( !wxDir::Exists(fn.GetFullPath()) ||
             !wxDir::GetAllFiles(fn.GetFullPath(),strings,wxT("*.xpm")) )
        {
            // Try ../../../samples/widgets/icons
            fn.AssignCwd();
            fn.RemoveLastDir();
            fn.RemoveLastDir();
            fn.RemoveLastDir();
            fn.AppendDir(wxT("samples"));
            fn.AppendDir(wxT("widgets"));
            fn.AppendDir(wxT("icons"));
            if ( !wxDir::Exists(fn.GetFullPath()) ||
                 !wxDir::GetAllFiles(fn.GetFullPath(),strings,wxT("*.xpm")) )
            {
                wxLogWarning(wxT("Could not load widget icons."));
                wxSetCursor(*wxSTANDARD_CURSOR);
                return;
            }
        }
    }

    unsigned int i;

    // Get size of existing images in list
    wxSize foundSize = m_combobox->GetBitmapSize();

    for ( i=0; i<strings->size(); i++ )
    {
        fn.SetFullName((*strings)[i]);
        wxString name =fn.GetName();

        // Handle few exceptions
        if ( name == wxT("bmpbtn") )
        {
            strings->RemoveAt(i);
            i--;
        }
        else
        {
#if wxUSE_IMAGE
            wxASSERT(fn.FileExists());
            wxImage image(fn.GetFullPath());
            wxASSERT(image.Ok());
            if ( m_chkScaleimages->GetValue() && foundSize.x > 0 )
                image.Rescale(foundSize.x, foundSize.y);
            wxBitmap bmp(image);
            wxASSERT( bmp.Ok() );
#else
            wxBitmap bmp(wxNullBitmap);
#endif
            images->Add(bmp);
            (*strings)[i] = name;
        }
    }

    wxSetCursor(*wxSTANDARD_CURSOR);
}

void BitmapComboBoxWidgetsPage::OnButtonAddWidgetIcons(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString strings;

    int sz = 32;
    //if ( m_chkScaleimages->GetValue() )
    //    sz = 16;

    wxImageList images(sz, sz);

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
        event.Enable( m_chkSort->GetValue() || m_chkReadonly->GetValue() );
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

void BitmapComboBoxWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetSelection() != wxNOT_FOUND);
}

void BitmapComboBoxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(m_combobox->GetCount() != 0);
}

void BitmapComboBoxWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(!(m_combobox->GetWindowStyle() & wxCB_SORT));
}

void BitmapComboBoxWidgetsPage::OnUpdateUIAddSeveralWithImages(wxUpdateUIEvent& event)
{
    if (m_combobox)
        event.Enable(!(m_combobox->GetWindowStyle() & wxCB_SORT));
}

void BitmapComboBoxWidgetsPage::OnComboText(wxCommandEvent& event)
{
    if (!m_combobox)
        return;

    wxString s = event.GetString();

    wxASSERT_MSG( s == m_combobox->GetValue(),
                  _T("event and combobox values should be the same") );

    if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
        wxLogMessage(_T("BitmapCombobox enter pressed (now '%s')"), s.c_str());
    else
        wxLogMessage(_T("BitmapCombobox text changed (now '%s')"), s.c_str());
}

void BitmapComboBoxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    long sel = event.GetInt();
    m_textDelete->SetValue(wxString::Format(_T("%ld"), sel));

    wxLogMessage(_T("BitmapCombobox item %ld selected"), sel);

    wxLogMessage(_T("BitmapCombobox GetValue(): %s"), m_combobox->GetValue().c_str() );
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

    wxImage image(filepath);
    if ( image.Ok() )
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

            image.Rescale(w, h);
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
    wxString filepath = wxFileSelector(wxT("Choose image file"),
                                       wxT(""),
                                       wxT(""),
                                       wxT(""),
                                       wxT("*.*"),
                                       wxOPEN | wxFILE_MUST_EXIST,
                                       this);

    wxBitmap bitmap;

    ::wxSetCursor( *wxHOURGLASS_CURSOR );

    if ( filepath.length() )
    {
        if ( pStr )
        {
            *pStr = wxFileName(filepath).GetName();
        }

        bitmap = LoadBitmap(filepath);
    }

    ::wxSetCursor( *wxSTANDARD_CURSOR );

    return bitmap;
}

wxBitmap BitmapComboBoxWidgetsPage::CreateBitmap(const wxColour& colour)
{
    int ch = m_combobox->GetBitmapSize().y;
    int h0 = ch - 5;

    long w = ch;
    long h = ch;

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

wxBitmap BitmapComboBoxWidgetsPage::CreateRandomBitmap( wxString* pStr )
{
    int i = rand() % 6;
    const wxChar* str = wxT("");
    wxBitmap bmp;

    if ( i == 0 )
    {
        str = wxT("Red Circle");
        bmp = CreateBitmap( *wxRED );
    }
    else if ( i == 1 )
    {
        str = wxT("Green Circle");
        bmp = CreateBitmap( *wxGREEN );
    }
    else if ( i == 2 )
    {
        str = wxT("Blue Circle");
        bmp = CreateBitmap( *wxBLUE );
    }
    else if ( i == 3 )
    {
        str = wxT("Black Circle");
        bmp = CreateBitmap( *wxBLACK );
    }
    else if ( i == 4 )
    {
        str = wxT("Cyan Circle");
        bmp = CreateBitmap( *wxCYAN );
    }
    else if ( i == 5 )
    {
        str = wxT("Light Grey Circle");
        bmp = CreateBitmap( *wxLIGHT_GREY );
    }

    if ( pStr )
        *pStr = str;

    return bmp;
}

#endif //wxUSE_BITMAPCOMBOBOX
