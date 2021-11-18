/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        notebook.cpp
// Purpose:     Part of the widgets sample showing book controls
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     06.04.01
// Copyright:   (c) 2001 Vadim Zeitlin, 2006 Wlodzimierz Skiba
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


#if wxUSE_BOOKCTRL

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
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"

#include "widgets.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    BookPage_Reset = wxID_HIGHEST,
    BookPage_SelectPage,
    BookPage_AddPage,
    BookPage_InsertPage,
    BookPage_RemovePage,
    BookPage_DeleteAll,
    BookPage_InsertText,
    BookPage_RemoveText,
    BookPage_SelectText,
    BookPage_NumPagesText,
    BookPage_CurSelectText,
    BookPage_Book
};

// book orientations
enum Orient
{
    Orient_Top,
    Orient_Bottom,
    Orient_Left,
    Orient_Right,
    Orient_Max
};

// ----------------------------------------------------------------------------
// BookWidgetsPage
// ----------------------------------------------------------------------------

class BookWidgetsPage : public WidgetsPage
{
public:
    BookWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist, const char *const icon[]);
    virtual ~BookWidgetsPage();

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_book; }
    virtual void RecreateWidget() wxOVERRIDE { RecreateBook(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
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

    // reset book parameters
    void Reset();

    // (re)create book
    void RecreateBook();
    virtual wxBookCtrlBase *CreateBook(long flags) = 0;

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
        { return (val >= 0) && (val < (int) m_book->GetPageCount()); }

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkImages;
    wxRadioBox *m_radioOrient;

    // the text controls containing input for various commands
    wxTextCtrl *m_textInsert,
               *m_textRemove,
               *m_textSelect;

    // the book itself and the sizer it is in
    wxBookCtrlBase *m_book;
    wxSizer *m_sizerBook;

    // the image list for our book
    wxImageList *m_imageList;

private:
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(BookWidgetsPage, WidgetsPage)
    EVT_BUTTON(BookPage_Reset, BookWidgetsPage::OnButtonReset)
    EVT_BUTTON(BookPage_SelectPage, BookWidgetsPage::OnButtonSelectPage)
    EVT_BUTTON(BookPage_AddPage, BookWidgetsPage::OnButtonAddPage)
    EVT_BUTTON(BookPage_InsertPage, BookWidgetsPage::OnButtonInsertPage)
    EVT_BUTTON(BookPage_RemovePage, BookWidgetsPage::OnButtonRemovePage)
    EVT_BUTTON(BookPage_DeleteAll, BookWidgetsPage::OnButtonDeleteAll)

    EVT_UPDATE_UI(BookPage_NumPagesText, BookWidgetsPage::OnUpdateUINumPagesText)
    EVT_UPDATE_UI(BookPage_CurSelectText, BookWidgetsPage::OnUpdateUICurSelectText)

    EVT_UPDATE_UI(BookPage_SelectPage, BookWidgetsPage::OnUpdateUISelectButton)
    EVT_UPDATE_UI(BookPage_InsertPage, BookWidgetsPage::OnUpdateUIInsertButton)
    EVT_UPDATE_UI(BookPage_RemovePage, BookWidgetsPage::OnUpdateUIRemoveButton)

    EVT_CHECKBOX(wxID_ANY, BookWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, BookWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

BookWidgetsPage::BookWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist, const char *const icon[])
                :WidgetsPage(book, imaglist, icon)
{
    // init everything
    m_chkImages = NULL;
    m_imageList = NULL;

    m_book = NULL;
    m_radioOrient = NULL;
    m_sizerBook = (wxSizer *)NULL;
}

void BookWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Set style");

    // must be in sync with Orient enum
    wxArrayString orientations;
    orientations.Add("&top");
    orientations.Add("&bottom");
    orientations.Add("&left");
    orientations.Add("&right");

    wxASSERT_MSG( orientations.GetCount() == Orient_Max,
                  "forgot to update something" );

    m_chkImages = new wxCheckBox(this, wxID_ANY, "Show &images");
    m_radioOrient = new wxRadioBox(this, wxID_ANY, "&Tab orientation",
                                   wxDefaultPosition, wxDefaultSize,
                                   orientations, 1, wxRA_SPECIFY_COLS);

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_chkImages, 0, wxALL, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioOrient, 0, wxALL, 5);

    wxButton *btn = new wxButton(this, BookPage_Reset, "&Reset");
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Contents");
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel("Number of pages: ",
                                                    BookPage_NumPagesText,
                                                    &text);
    text->SetEditable(false);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndLabel("Current selection: ",
                                           BookPage_CurSelectText,
                                           &text);
    text->SetEditable(false);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(BookPage_SelectPage,
                                            "&Select page",
                                            BookPage_SelectText,
                                            &m_textSelect);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BookPage_AddPage, "&Add page");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(BookPage_InsertPage,
                                            "&Insert page at",
                                            BookPage_InsertText,
                                            &m_textInsert);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(BookPage_RemovePage,
                                            "&Remove page",
                                            BookPage_RemoveText,
                                            &m_textRemove);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, BookPage_DeleteAll, "&Delete All");
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    m_sizerBook = new wxBoxSizer(wxHORIZONTAL);

    // the 3 panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerBook, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    RecreateBook();

    // final initializations
    Reset();
    CreateImageList();

    SetSizer(sizerTop);
}

BookWidgetsPage::~BookWidgetsPage()
{
    delete m_imageList;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void BookWidgetsPage::Reset()
{
    m_chkImages->SetValue(true);
    m_radioOrient->SetSelection(Orient_Top);
}

void BookWidgetsPage::CreateImageList()
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

        if ( m_book )
            m_book->SetImageList(m_imageList);
    }
    else // no images
    {
        wxDELETE(m_imageList);
    }

    // because of the bug in wxMSW we can't use SetImageList(NULL) - although
    // it would be logical if this removed the image list from book, under
    // MSW it crashes instead - FIXME
}

void BookWidgetsPage::RecreateBook()
{
    // do not recreate anything in case page content was not prepared yet
    if(!m_radioOrient)
        return;

    int flags = GetAttrs().m_defaultFlags;

    switch ( m_radioOrient->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unknown orientation" );
            wxFALLTHROUGH;

        case Orient_Top:
            flags |= wxBK_TOP;
            break;

        case Orient_Bottom:
            flags |= wxBK_BOTTOM;
            break;

        case Orient_Left:
            flags |= wxBK_LEFT;
            break;

        case Orient_Right:
            flags |= wxBK_RIGHT;
            break;
    }

    wxBookCtrlBase *oldBook = m_book;

    m_book = CreateBook(flags);

    CreateImageList();

    if ( oldBook )
    {
        const int sel = oldBook->GetSelection();

        const int count = oldBook->GetPageCount();

        // recreate the pages
        for ( int n = 0; n < count; n++ )
        {
            m_book->AddPage(CreateNewPage(),
                            oldBook->GetPageText(n),
                            false,
                            m_chkImages->GetValue() ?
                            GetIconIndex() : -1);
        }

        m_sizerBook->Detach( oldBook );
        delete oldBook;

        // restore selection
        if ( sel != -1 )
        {
            m_book->SetSelection(sel);
        }
    }

    m_sizerBook->Add(m_book, 1, wxGROW | wxALL, 5);
    m_sizerBook->SetMinSize(150, 0);
    m_sizerBook->Layout();
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

int BookWidgetsPage::GetTextValue(wxTextCtrl *text) const
{
    long pos = -1;

    if ( !text || !text->GetValue().ToLong(&pos) )
        pos = -1;

    return (int)pos;
}

int BookWidgetsPage::GetIconIndex() const
{
    if ( m_book )
    {
       const int nImages = m_book->GetImageCount();
       if ( nImages > 0 )
       {
           return m_book->GetPageCount() % nImages;
       }
    }

    return -1;
}

wxWindow *BookWidgetsPage::CreateNewPage()
{
    return new wxTextCtrl(m_book, wxID_ANY, "I'm a book page");
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void BookWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    RecreateBook();
}

void BookWidgetsPage::OnButtonDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    m_book->DeleteAllPages();
}

void BookWidgetsPage::OnButtonSelectPage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textSelect);
    wxCHECK_RET( IsValidValue(pos), "button should be disabled" );

    m_book->SetSelection(pos);
}

void BookWidgetsPage::OnButtonAddPage(wxCommandEvent& WXUNUSED(event))
{
    m_book->AddPage(CreateNewPage(), "Added page", false,
                    GetIconIndex());
}

void BookWidgetsPage::OnButtonInsertPage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textInsert);
    wxCHECK_RET( IsValidValue(pos), "button should be disabled" );

    m_book->InsertPage(pos, CreateNewPage(), "Inserted page", false,
                       GetIconIndex());
}

void BookWidgetsPage::OnButtonRemovePage(wxCommandEvent& WXUNUSED(event))
{
    int pos = GetTextValue(m_textRemove);
    wxCHECK_RET( IsValidValue(pos), "button should be disabled" );

    m_book->DeletePage(pos);
}

void BookWidgetsPage::OnUpdateUISelectButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textSelect)) );
}

void BookWidgetsPage::OnUpdateUIInsertButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textInsert)) );
}

void BookWidgetsPage::OnUpdateUIRemoveButton(wxUpdateUIEvent& event)
{
    event.Enable( IsValidValue(GetTextValue(m_textRemove)) );
}

void BookWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    if(m_chkImages && m_radioOrient)
        event.Enable( !m_chkImages->GetValue() ||
                      m_radioOrient->GetSelection() != wxBK_TOP );
}

void BookWidgetsPage::OnUpdateUINumPagesText(wxUpdateUIEvent& event)
{
    if(m_book)
        event.SetText( wxString::Format("%u", unsigned(m_book->GetPageCount())) );
}

void BookWidgetsPage::OnUpdateUICurSelectText(wxUpdateUIEvent& event)
{
    if(m_book)
        event.SetText( wxString::Format("%d", m_book->GetSelection()) );
}

void BookWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    RecreateBook();
}

#if wxUSE_NOTEBOOK

#include "icons/notebook.xpm"
#include "wx/notebook.h"

// ----------------------------------------------------------------------------
// NotebookWidgetsPage
// ----------------------------------------------------------------------------

class NotebookWidgetsPage : public BookWidgetsPage
{
public:
    NotebookWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : BookWidgetsPage(book, imaglist, notebook_xpm)
    {
        RecreateBook();
    }
    virtual ~NotebookWidgetsPage() {}

protected:

    // event handlers
    void OnPageChanging(wxNotebookEvent& event);
    void OnPageChanged(wxNotebookEvent& event);

    // (re)create book
    virtual wxBookCtrlBase *CreateBook(long flags) wxOVERRIDE
    {
        return new wxNotebook(this, BookPage_Book,
                              wxDefaultPosition, wxDefaultSize,
                              flags);
    }

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(NotebookWidgetsPage)
};

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(NotebookWidgetsPage, BookWidgetsPage)
    EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, NotebookWidgetsPage::OnPageChanging)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, NotebookWidgetsPage::OnPageChanged)
wxEND_EVENT_TABLE()

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#elif defined(__WXMOTIF__)
    #define FAMILY_CTRLS GENERIC_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(NotebookWidgetsPage, "Notebook",
                       FAMILY_CTRLS | BOOK_CTRLS
                       );

void NotebookWidgetsPage::OnPageChanging(wxNotebookEvent& event)
{
    wxLogMessage("Notebook page changing from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

void NotebookWidgetsPage::OnPageChanged(wxNotebookEvent& event)
{
    wxLogMessage("Notebook page changed from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

#endif // wxUSE_NOTEBOOK

#if wxUSE_LISTBOOK

#include "icons/listbook.xpm"
#include "wx/listbook.h"

// ----------------------------------------------------------------------------
// ListbookWidgetsPage
// ----------------------------------------------------------------------------

class ListbookWidgetsPage : public BookWidgetsPage
{
public:
    ListbookWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : BookWidgetsPage(book, imaglist, listbook_xpm)
    {
        RecreateBook();
    }
    virtual ~ListbookWidgetsPage() {}

protected:

    // event handlers
    void OnPageChanging(wxListbookEvent& event);
    void OnPageChanged(wxListbookEvent& event);

    // (re)create book
    virtual wxBookCtrlBase *CreateBook(long flags) wxOVERRIDE
    {
        return new wxListbook(this, BookPage_Book,
                              wxDefaultPosition, wxDefaultSize,
                              flags);
    }

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ListbookWidgetsPage)
};

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ListbookWidgetsPage, BookWidgetsPage)
    EVT_LISTBOOK_PAGE_CHANGING(wxID_ANY, ListbookWidgetsPage::OnPageChanging)
    EVT_LISTBOOK_PAGE_CHANGED(wxID_ANY, ListbookWidgetsPage::OnPageChanged)
wxEND_EVENT_TABLE()

IMPLEMENT_WIDGETS_PAGE(ListbookWidgetsPage, "Listbook",
                       GENERIC_CTRLS | BOOK_CTRLS
                       );

void ListbookWidgetsPage::OnPageChanging(wxListbookEvent& event)
{
    wxLogMessage("Listbook page changing from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

void ListbookWidgetsPage::OnPageChanged(wxListbookEvent& event)
{
    wxLogMessage("Listbook page changed from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

#endif // wxUSE_LISTBOOK

#if wxUSE_CHOICEBOOK

#include "icons/choicebk.xpm"
#include "wx/choicebk.h"

// ----------------------------------------------------------------------------
// ChoicebookWidgetsPage
// ----------------------------------------------------------------------------

class ChoicebookWidgetsPage : public BookWidgetsPage
{
public:
    ChoicebookWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : BookWidgetsPage(book, imaglist, choicebk_xpm)
    {
        RecreateBook();
    }
    virtual ~ChoicebookWidgetsPage() {}

protected:

    // event handlers
    void OnPageChanging(wxChoicebookEvent& event);
    void OnPageChanged(wxChoicebookEvent& event);

    // (re)create book
    virtual wxBookCtrlBase *CreateBook(long flags) wxOVERRIDE
    {
        return new wxChoicebook(this, BookPage_Book,
                                wxDefaultPosition, wxDefaultSize,
                                flags);
    }

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ChoicebookWidgetsPage)
};

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ChoicebookWidgetsPage, BookWidgetsPage)
    EVT_CHOICEBOOK_PAGE_CHANGING(wxID_ANY, ChoicebookWidgetsPage::OnPageChanging)
    EVT_CHOICEBOOK_PAGE_CHANGED(wxID_ANY, ChoicebookWidgetsPage::OnPageChanged)
wxEND_EVENT_TABLE()

IMPLEMENT_WIDGETS_PAGE(ChoicebookWidgetsPage, "Choicebook",
                       GENERIC_CTRLS | BOOK_CTRLS
                       );

void ChoicebookWidgetsPage::OnPageChanging(wxChoicebookEvent& event)
{
    wxLogMessage("Choicebook page changing from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

void ChoicebookWidgetsPage::OnPageChanged(wxChoicebookEvent& event)
{
    wxLogMessage("Choicebook page changed from %d to %d (currently %d).",
                 event.GetOldSelection(),
                 event.GetSelection(),
                 m_book->GetSelection());

    event.Skip();
}

#endif // wxUSE_CHOICEBOOK

#endif // wxUSE_BOOKCTRL
