/////////////////////////////////////////////////////////////////////////////
// Name:        artbrows.cpp
// Purpose:     wxArtProvider demo - art browser dialog
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/04/05
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/choice.h"
#endif

#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"

#include "artbrows.h"

#define ART_CLIENT(id) \
    choice->Append(#id, new wxStringClientData(id));
#define ART_ICON(id) \
    { \
        int ind; \
        wxIcon icon = wxArtProvider::GetIcon(id, client, size); \
        if ( icon.IsOk() ) \
            ind = images->Add(icon); \
        else \
            ind = 0; \
        list->InsertItem(index, #id, ind); \
        list->SetItemPtrData(index, wxPtrToUInt(new wxString(id))); \
        index++; \
    }

// ----------------------------------------------------------------------------
// Functions to fill-in all supported art IDs
// ----------------------------------------------------------------------------

static void FillClients(wxChoice *choice)
{
    ART_CLIENT(wxART_OTHER)
    ART_CLIENT(wxART_BUTTON)
    ART_CLIENT(wxART_TOOLBAR)
    ART_CLIENT(wxART_MENU)
    ART_CLIENT(wxART_FRAME_ICON)
    ART_CLIENT(wxART_CMN_DIALOG)
    ART_CLIENT(wxART_HELP_BROWSER)
    ART_CLIENT(wxART_MESSAGE_BOX) // Keep this last, it's the initial shown one
}

static void FillBitmaps(wxImageList *images, wxListCtrl *list,
                        int& index,
                        const wxArtClient& client, const wxSize& size)
{
    ART_ICON(wxART_ERROR)
    ART_ICON(wxART_QUESTION)
    ART_ICON(wxART_WARNING)
    ART_ICON(wxART_INFORMATION)
    ART_ICON(wxART_ADD_BOOKMARK)
    ART_ICON(wxART_DEL_BOOKMARK)
    ART_ICON(wxART_HELP_SIDE_PANEL)
    ART_ICON(wxART_HELP_SETTINGS)
    ART_ICON(wxART_HELP_BOOK)
    ART_ICON(wxART_HELP_FOLDER)
    ART_ICON(wxART_HELP_PAGE)
    ART_ICON(wxART_GO_BACK)
    ART_ICON(wxART_GO_FORWARD)
    ART_ICON(wxART_GO_UP)
    ART_ICON(wxART_GO_DOWN)
    ART_ICON(wxART_GO_TO_PARENT)
    ART_ICON(wxART_GO_HOME)
    ART_ICON(wxART_GOTO_FIRST)
    ART_ICON(wxART_GOTO_LAST)
    ART_ICON(wxART_PRINT)
    ART_ICON(wxART_HELP)
    ART_ICON(wxART_TIP)
    ART_ICON(wxART_REPORT_VIEW)
    ART_ICON(wxART_LIST_VIEW)
    ART_ICON(wxART_NEW_DIR)
    ART_ICON(wxART_FOLDER)
    ART_ICON(wxART_FOLDER_OPEN);
    ART_ICON(wxART_GO_DIR_UP)
    ART_ICON(wxART_EXECUTABLE_FILE)
    ART_ICON(wxART_NORMAL_FILE)
    ART_ICON(wxART_TICK_MARK)
    ART_ICON(wxART_CROSS_MARK)
    ART_ICON(wxART_MISSING_IMAGE)
    ART_ICON(wxART_NEW);
    ART_ICON(wxART_FILE_OPEN)
    ART_ICON(wxART_FILE_SAVE)
    ART_ICON(wxART_FILE_SAVE_AS)
    ART_ICON(wxART_DELETE);
    ART_ICON(wxART_COPY)
    ART_ICON(wxART_CUT)
    ART_ICON(wxART_PASTE)
    ART_ICON(wxART_UNDO)
    ART_ICON(wxART_REDO)
    ART_ICON(wxART_PLUS)
    ART_ICON(wxART_MINUS)
    ART_ICON(wxART_CLOSE)
    ART_ICON(wxART_QUIT)
    ART_ICON(wxART_FIND)
    ART_ICON(wxART_FIND_AND_REPLACE)
    ART_ICON(wxART_EDIT)
    ART_ICON(wxART_FULL_SCREEN)
    ART_ICON(wxART_HARDDISK)
    ART_ICON(wxART_FLOPPY)
    ART_ICON(wxART_CDROM)
    ART_ICON(wxART_REMOVABLE)
    ART_ICON(wxART_REFRESH)
    ART_ICON(wxART_STOP)
}


// ----------------------------------------------------------------------------
// Browser implementation
// ----------------------------------------------------------------------------

#include "null.xpm"

const int SIZE_CHOICE_ID = ::wxNewId();

// Bitmap sizes that can be chosen in the size selection wxChoice.
static const int bitmapSizes[] = { -1, 16, 32, 64, 128, 256, 0 };

wxBEGIN_EVENT_TABLE(wxArtBrowserDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, wxArtBrowserDialog::OnSelectItem)
    EVT_CHOICE(SIZE_CHOICE_ID, wxArtBrowserDialog::OnChangeSize)
    EVT_CHOICE(wxID_ANY, wxArtBrowserDialog::OnChooseClient)
wxEND_EVENT_TABLE()

wxArtBrowserDialog::wxArtBrowserDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Art resources browser",
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    m_currentArtId = wxART_ERROR;

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer *subsizer;

    wxChoice *choice = new wxChoice(this, wxID_ANY);
    FillClients(choice);

    subsizer = new wxBoxSizer(wxHORIZONTAL);
    subsizer->Add(new wxStaticText(this, wxID_ANY, "Client:"), 0, wxALIGN_CENTER_VERTICAL);
    subsizer->Add(choice, 1, wxLEFT, 5);
    sizer->Add(subsizer, 0, wxALL | wxEXPAND, 10);

    subsizer = new wxBoxSizer(wxHORIZONTAL);

    m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(250, 300),
                            wxLC_REPORT | wxSUNKEN_BORDER);
    m_list->AppendColumn("wxArtID");
    subsizer->Add(m_list, 0, wxEXPAND | wxRIGHT, 10);

    wxSizer *subsub = new wxBoxSizer(wxVERTICAL);

    m_sizes = new wxChoice( this, SIZE_CHOICE_ID );
    for ( const int* p = bitmapSizes; *p; ++p )
    {
      if ( *p == -1 )
        m_sizes->Append( "Default" );
      else
        m_sizes->Append( wxString::Format("%d x %d", *p, *p ) );
    }
    m_sizes->SetSelection(0);
    subsub->Add(m_sizes, 0, wxALL, 4);

    m_text = new wxStaticText(this, wxID_ANY, "Size: 333x333");
    subsub->Add(m_text, 0, wxALL, 4);

    m_canvas = new wxStaticBitmap(this, wxID_ANY, wxBitmap(null_xpm));
    subsub->Add(m_canvas);
    subsub->Add(256, 256);
    subsizer->Add(subsub, 1, wxLEFT, 4 );

    sizer->Add(subsizer, 1, wxEXPAND | wxLEFT|wxRIGHT, 10);

    wxButton *ok = new wxButton(this, wxID_OK, "Close");
    ok->SetDefault();
    sizer->Add(ok, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizerAndFit(sizer);

    choice->SetSelection(choice->GetCount() - 1);
    SetArtClient(wxART_MESSAGE_BOX);
}

wxArtBrowserDialog::~wxArtBrowserDialog()
{
    DeleteListItemData();
}

wxSize wxArtBrowserDialog::GetSelectedBitmapSize() const
{
  const int size = bitmapSizes[m_sizes->GetSelection()];
  return wxSize(size, size);
}

void wxArtBrowserDialog::DeleteListItemData()
{
    const int itemCount = m_list->GetItemCount();

    // item data are set by the ART_ICON macro
    for ( int i = 0; i < itemCount; ++i )
        delete reinterpret_cast<wxString*>(m_list->GetItemData(i));
}

void wxArtBrowserDialog::SetArtClient(const wxArtClient& client)
{
    wxBusyCursor bcur;

    wxImageList *img = new wxImageList(16, 16);
    img->Add(wxIcon(null_xpm));
    int index = 0;

    long sel = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    if (sel < 0) sel = 0;

    DeleteListItemData();
    m_list->DeleteAllItems();
    FillBitmaps(img, m_list, index, client, wxSize(16, 16));
    m_list->AssignImageList(img, wxIMAGE_LIST_SMALL);
    m_list->SetColumnWidth(0, wxLIST_AUTOSIZE);

    m_list->SetItemState(sel, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);

    m_client = client;

    const wxString *data = (const wxString*)m_list->GetItemData(sel);
    SetArtBitmap(*data, m_client);
}

void wxArtBrowserDialog::OnSelectItem(wxListEvent &event)
{
    const wxString *data = (const wxString*)event.GetData();
    m_currentArtId = *data;
    SetArtBitmap(*data, m_client, GetSelectedBitmapSize());
}

void wxArtBrowserDialog::OnChangeSize(wxCommandEvent& WXUNUSED(event))
{
    SetArtBitmap(m_currentArtId, m_client, GetSelectedBitmapSize() );
}

void wxArtBrowserDialog::OnChooseClient(wxCommandEvent &event)
{
    wxStringClientData *data = (wxStringClientData *)event.GetClientObject();
    SetArtClient(data->GetData());
}

void wxArtBrowserDialog::SetArtBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
    wxBitmap bmp = wxArtProvider::GetBitmap(id, client, size);
    m_canvas->SetSize(bmp.GetWidth(), bmp.GetHeight());
    m_canvas->SetBitmap(bmp);
    m_text->SetLabel(wxString::Format("Size: %d x %d", bmp.GetWidth(), bmp.GetHeight()));
    Refresh();
}
