/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
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
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"

    #include "wx/dynarray.h"
#endif

#include "wx/sizer.h"
#include "wx/notebook.h"

#include "widgets.h"

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

// old versions of wxWindows don't define this style
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
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(NotebookWidgetsPage);
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

    EVT_NOTEBOOK_PAGE_CHANGING(-1, NotebookWidgetsPage::OnPageChanging)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, NotebookWidgetsPage::OnPageChanged)

    EVT_CHECKBOX(-1, NotebookWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, NotebookWidgetsPage::OnCheckOrRadioBox)
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
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

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

    m_chkImages = new wxCheckBox(this, -1, _T("Show &images"));
    m_radioOrient = new wxRadioBox(this, -1, _T("&Tab orientation"),
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
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel(_T("Number of pages: "),
                                                    NotebookPage_NumPagesText,
                                                    &text);
    text->SetEditable(FALSE);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel(_T("Current selection: "),
                                           NotebookPage_CurSelectText,
                                           &text);
    text->SetEditable(FALSE);
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
    sizerRight->SetMinSize(250, 0);
    m_sizerNotebook = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();
    CreateImageList();

    SetAutoLayout(TRUE);
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
    m_chkImages->SetValue(TRUE);
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
            m_imageList->Add(wxTheApp->GetStdIcon(wxICON_INFORMATION));
            m_imageList->Add(wxTheApp->GetStdIcon(wxICON_QUESTION));
            m_imageList->Add(wxTheApp->GetStdIcon(wxICON_WARNING));
            m_imageList->Add(wxTheApp->GetStdIcon(wxICON_ERROR));
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
            wxFAIL_MSG( _T("unknown notebok orientation") );
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

    wxNotebook *notebook = m_notebook;

    m_notebook = new wxNotebook(this, NotebookPage_Notebook,
                                wxDefaultPosition, wxDefaultSize,
                                flags);

    CreateImageList();

    if ( notebook )
    {
        int sel = notebook->GetSelection();

        int count = notebook->GetPageCount();
        for ( int n = 0; n < count; n++ )
        {
            wxNotebookPage *page = notebook->GetPage(0);
            page->Reparent(m_notebook);

            m_notebook->AddPage(page, notebook->GetPageText(0), FALSE,
                                notebook->GetPageImage(0));

            notebook->RemovePage(0);
        }

        m_sizerNotebook->Remove(notebook);
        delete notebook;

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
    return new wxTextCtrl(m_notebook, -1, _T("I'm a notebook page"));
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

void NotebookWidgetsPage::OnButtonSelectPage(wxCommandEvent& event)
{
    int pos = GetTextValue(m_textSelect);
    wxCHECK_RET( pos >= 0, _T("button should be disabled") );

    m_notebook->SetSelection(pos);
}

void NotebookWidgetsPage::OnButtonAddPage(wxCommandEvent& WXUNUSED(event))
{
    m_notebook->AddPage(CreateNewPage(), _T("Added page"), FALSE,
                        GetIconIndex());
}

void NotebookWidgetsPage::OnButtonInsertPage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textInsert);
    wxCHECK_RET( pos >= 0, _T("button should be disabled") );

    m_notebook->InsertPage(pos, CreateNewPage(), _T("Inserted page"), FALSE,
                           GetIconIndex());
}

void NotebookWidgetsPage::OnButtonRemovePage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textRemove);
    wxCHECK_RET( pos >= 0, _T("button should be disabled") );

    m_notebook->DeletePage(pos);
}

void NotebookWidgetsPage::OnUpdateUISelectButton(wxUpdateUIEvent& event)
{
    event.Enable( GetTextValue(m_textSelect) >= 0 );
}

void NotebookWidgetsPage::OnUpdateUIInsertButton(wxUpdateUIEvent& event)
{
    event.Enable( GetTextValue(m_textInsert) >= 0 );
}

void NotebookWidgetsPage::OnUpdateUIRemoveButton(wxUpdateUIEvent& event)
{
    event.Enable( GetTextValue(m_textRemove) >= 0 );
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

void NotebookWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
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

