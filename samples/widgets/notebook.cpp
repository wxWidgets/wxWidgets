/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        notebook.cpp
// Purpose:     Part of the widgets sample showing wxNotebook
// Author:      Vadim Zeitlin
// Created:     06.04.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
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

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/combobox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"

    #include "wx/dynarray.h"
#endif

#include "wx/sizer.h"
#include "wx/notebook.h"
#include "wx/artprov.h"

#include "widgets.h"
#if 1
#include "icons/notebook.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    NotebookPage_Reset = 100,
    NotebookPage_SelectPage,
    NotebookPage_AddPage,
    NotebookPage_InsertPage,
    NotebookPage_RemovePage,
    NotebookPage_DeleteAll,
    NotebookPage_InsertText,
    NotebookPage_RemoveText,
    NotebookPage_SelectText,
    NotebookPage_NumPagesText,
    NotebookPage_CurSelectText,
    NotebookPage_Notebook
};

// notebook orientations
enum Orient
{
    Orient_Top,
    Orient_Bottom,
    Orient_Left,
    Orient_Right,
    Orient_Max
};

// old versions of wxWidgets don't define this style
#ifndef wxNB_TOP
    #define wxNB_TOP (0)
#endif

// ----------------------------------------------------------------------------
// NotebookWidgetsPage
// ----------------------------------------------------------------------------

class NotebookWidgetsPage : public WidgetsPage
{
public:
    NotebookWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~NotebookWidgetsPage();

    virtual wxControl *GetWidget() const { return m_notebook; }

protected:
    // event handlers
    void OnPageChanging(wxNotebookEvent& event);
    void OnPageChanged(wxNotebookEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonDeleteAll(wxCommandEvent& event);
    void OnButtonSelectPage(wxCommandEvent& event);
    void OnButtonAddPage(wxCommandEvent& event);
    void OnButtonInsertPage(wxCommandEvent& event);
    void OnButtonRemovePage(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUINumPagesText(wxUpdateUIEvent& event);
    void OnUpdateUICurSelectText(wxUpdateUIEvent& event);

    void OnUpdateUISelectButton(wxUpdateUIEvent& event);
    void OnUpdateUIInsertButton(wxUpdateUIEvent& event);
    void OnUpdateUIRemoveButton(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the wxNotebook parameters
    void Reset();

    // (re)create the wxNotebook
    void CreateNotebook();

    // create or destroy the image list
    void CreateImageList();

    // create a new page
    wxWindow *CreateNewPage();

    // get the image index for the new page
    int GetIconIndex() const;

    // get the numeric value of text ctrl
    int GetTextValue(wxTextCtrl *text) const;

    // is the value in range?
    bool IsValidValue(int val) const
        { return (val >= 0) && (val < (int) m_notebook->GetPageCount()); }

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkImages;
    wxRadioBox *m_radioOrient;

    // the text controls containing input for various commands
    wxTextCtrl *m_textInsert,
               *m_textRemove,
               *m_textSelect;

    // the notebook itself and the sizer it is in
    wxNotebook *m_notebook;
    wxSizer *m_sizerNotebook;

    // thei mage list for our notebook
    wxImageList *m_imageList;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(NotebookWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NotebookWidgetsPage, WidgetsPage)
    EVT_BUTTON(NotebookPage_Reset, NotebookWidgetsPage::OnButtonReset)
    EVT_BUTTON(NotebookPage_SelectPage, NotebookWidgetsPage::OnButtonSelectPage)
    EVT_BUTTON(NotebookPage_AddPage, NotebookWidgetsPage::OnButtonAddPage)
    EVT_BUTTON(NotebookPage_InsertPage, NotebookWidgetsPage::OnButtonInsertPage)
    EVT_BUTTON(NotebookPage_RemovePage, NotebookWidgetsPage::OnButtonRemovePage)
    EVT_BUTTON(NotebookPage_DeleteAll, NotebookWidgetsPage::OnButtonDeleteAll)

    EVT_UPDATE_UI(NotebookPage_NumPagesText, NotebookWidgetsPage::OnUpdateUINumPagesText)
    EVT_UPDATE_UI(NotebookPage_CurSelectText, NotebookWidgetsPage::OnUpdateUICurSelectText)

    EVT_UPDATE_UI(NotebookPage_SelectPage, NotebookWidgetsPage::OnUpdateUISelectButton)
    EVT_UPDATE_UI(NotebookPage_InsertPage, NotebookWidgetsPage::OnUpdateUIInsertButton)
    EVT_UPDATE_UI(NotebookPage_RemovePage, NotebookWidgetsPage::OnUpdateUIRemoveButton)

    EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, NotebookWidgetsPage::OnPageChanging)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, NotebookWidgetsPage::OnPageChanged)

    EVT_CHECKBOX(wxID_ANY, NotebookWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, NotebookWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(NotebookWidgetsPage, _T("Notebook"));

NotebookWidgetsPage::NotebookWidgetsPage(wxNotebook *notebook,
                                         wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(notebook_xpm));

    // init everything
    m_chkImages = NULL;
    m_imageList = NULL;

    m_notebook = (wxNotebook *)NULL;
    m_sizerNotebook = (wxSizer *)NULL;

    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    // must be in sync with Orient enum
    wxString orientations[] =
    {
        _T("&top"),
        _T("&bottom"),
        _T("&left"),
        _T("&right"),
    };

    wxASSERT_MSG( WXSIZEOF(orientations) == Orient_Max,
                  _T("forgot to update something") );

    m_chkImages = new wxCheckBox(this, wxID_ANY, _T("Show &images"));
    m_radioOrient = new wxRadioBox(this, wxID_ANY, _T("&Tab orientation"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(orientations), orientations,
                                   1, wxRA_SPECIFY_COLS);

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_chkImages, 0, wxALL, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioOrient, 0, wxALL, 5);

    wxButton *btn = new wxButton(this, NotebookPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel(_T("Number of pages: "),
                                                    NotebookPage_NumPagesText,
                                                    &text);
    text->SetEditable(false);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel(_T("Current selection: "),
                                           NotebookPage_CurSelectText,
                                           &text);
    text->SetEditable(false);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(NotebookPage_SelectPage,
                                            _T("&Select page"),
                                            NotebookPage_SelectText,
                                            &m_textSelect);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, NotebookPage_AddPage, _T("&Add page"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(NotebookPage_InsertPage,
                                            _T("&Insert page at"),
                                            NotebookPage_InsertText,
                                            &m_textInsert);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(NotebookPage_RemovePage,
                                            _T("&Remove page"),
                                            NotebookPage_RemoveText,
                                            &m_textRemove);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, NotebookPage_DeleteAll, _T("&Delete All"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    m_notebook = new wxNotebook(this, NotebookPage_Notebook);
    sizerRight->Add(m_notebook, 1, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerNotebook = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();
    CreateImageList();

    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

NotebookWidgetsPage::~NotebookWidgetsPage()
{
    delete m_imageList;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void NotebookWidgetsPage::Reset()
{
    m_chkImages->SetValue(true);
    m_radioOrient->SetSelection(Orient_Top);
}

void NotebookWidgetsPage::CreateImageList()
{
    if ( m_chkImages->GetValue() )
    {
        if ( !m_imageList )
        {
            // create a dummy image list with a few icons
            m_imageList = new wxImageList(32, 32);
            wxSize size(32, 32);
            m_imageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, size));
            m_imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));
            m_imageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, size));
            m_imageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, size));
        }

        m_notebook->SetImageList(m_imageList);
    }
    else // no images
    {
        if ( m_imageList )
        {
            delete m_imageList;
            m_imageList = NULL;
        }
    }

    // because of the bug in wxMSW we can't use SetImageList(NULL) - although
    // it would be logical if this removed the image list from notebook, under
    // MSW it crashes instead
}

void NotebookWidgetsPage::CreateNotebook()
{
    int flags;
    switch ( m_radioOrient->GetSelection() )
    {
        default:
            wxFAIL_MSG( _T("unknown notebook orientation") );
            // fall through

        case Orient_Top:
            flags = wxNB_TOP;
            break;

        case Orient_Bottom:
            flags = wxNB_BOTTOM;
            break;

        case Orient_Left:
            flags = wxNB_LEFT;
            break;

        case Orient_Right:
            flags = wxNB_RIGHT;
            break;
    }

    wxNotebook *old_note = m_notebook;

    m_notebook = new wxNotebook(this, NotebookPage_Notebook,
                                wxDefaultPosition, wxDefaultSize,
                                flags);

    CreateImageList();

    if ( old_note )
    {
        const int sel = old_note->GetSelection();

        const int count = old_note->GetPageCount();

        // recreate the pages
        for ( int n = 0; n < count; n++ )
        {
            m_notebook->AddPage(CreateNewPage(),
                                old_note->GetPageText(n),
                                false,
																m_chkImages->GetValue() ?
                                GetIconIndex() : -1);
        }

        m_sizerNotebook->Detach( old_note );
        delete old_note;

        // restore selection
        if ( sel != -1 )
        {
            m_notebook->SetSelection(sel);
        }
    }

    m_sizerNotebook->Add(m_notebook, 1, wxGROW | wxALL, 5);
    m_sizerNotebook->Layout();
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

int NotebookWidgetsPage::GetTextValue(wxTextCtrl *text) const
{
    long pos;
    if ( !text->GetValue().ToLong(&pos) )
        pos = -1;

    return (int)pos;
}

int NotebookWidgetsPage::GetIconIndex() const
{
    if ( m_imageList )
    {
       int nImages = m_imageList->GetImageCount();
       if ( nImages > 0 )
       {
           return m_notebook->GetPageCount() % nImages;
       }
    }

    return -1;
}

wxWindow *NotebookWidgetsPage::CreateNewPage()
{
    return new wxTextCtrl(m_notebook, wxID_ANY, _T("I'm a notebook page"));
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void NotebookWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateNotebook();
}

void NotebookWidgetsPage::OnButtonDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    m_notebook->DeleteAllPages();
}

void NotebookWidgetsPage::OnButtonSelectPage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textSelect);
    wxCHECK_RET( IsValidValue(pos), _T("button should be disabled") );

    m_notebook->SetSelection(pos);
}

void NotebookWidgetsPage::OnButtonAddPage(wxCommandEvent& WXUNUSED(event))
{
    m_notebook->AddPage(CreateNewPage(), _T("Added page"), false,
                        GetIconIndex());
}

void NotebookWidgetsPage::OnButtonInsertPage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textInsert);
    wxCHECK_RET( IsValidValue(pos), _T("button should be disabled") );

    m_notebook->InsertPage(pos, CreateNewPage(), _T("Inserted page"), false,
                           GetIconIndex());
}

void NotebookWidgetsPage::OnButtonRemovePage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textRemove);
    wxCHECK_RET( IsValidValue(pos), _T("button should be disabled") );

    m_notebook->DeletePage(pos);
}

void NotebookWidgetsPage::OnUpdateUISelectButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textSelect)) );
}

void NotebookWidgetsPage::OnUpdateUIInsertButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textInsert)) );
}

void NotebookWidgetsPage::OnUpdateUIRemoveButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textRemove)) );
}

void NotebookWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( !m_chkImages->GetValue() ||
                  m_radioOrient->GetSelection() != wxNB_TOP );
}

void NotebookWidgetsPage::OnUpdateUINumPagesText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format(_T("%d"), m_notebook->GetPageCount()) );
}

void NotebookWidgetsPage::OnUpdateUICurSelectText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format(_T("%d"), m_notebook->GetSelection()) );
}

void NotebookWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateNotebook();
}

void NotebookWidgetsPage::OnPageChanging(wxNotebookEvent& event)
{
    wxLogMessage(_T("Notebook page changing from %d to %d (currently %d)."),
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_notebook->GetSelection());

    event.Skip();
}

void NotebookWidgetsPage::OnPageChanged(wxNotebookEvent& event)
{
    wxLogMessage(_T("Notebook page changed from %d to %d (currently %d)."),
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_notebook->GetSelection());

    event.Skip();
}

#endif
