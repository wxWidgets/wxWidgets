///////////////////////////////////////////////////////////////////////////////
// Name:        ownerdrw.cpp
// Purpose:     Owner-draw sample, for Windows
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include  "wx/ownerdrw.h"
#include  "wx/menuitem.h"
#include  "wx/image.h"
#include  "wx/msw/checklst.h"

// Define a new application type
class OwnerDrawnApp: public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class OwnerDrawnFrame : public wxFrame
{
public:
    // ctor & dtor
    OwnerDrawnFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);
    ~OwnerDrawnFrame();

    // notifications
    void OnQuit             (wxCommandEvent& event);
    void OnAbout            (wxCommandEvent& event);
    void OnListboxSelect    (wxCommandEvent& event);
    void OnCheckboxToggle   (wxCommandEvent& event);
    void OnListboxDblClick  (wxCommandEvent& event);
    bool OnClose            ()                        { return TRUE; }

    DECLARE_EVENT_TABLE()

private:
    void InitMenu();

    wxCheckListBox *m_pListBox;
};

enum 
{
    Menu_Quit = 1, 
    Menu_First = 100,
    Menu_Test1, Menu_Test2, Menu_Test3, 
    Menu_Bitmap, Menu_Bitmap2, 
    Menu_Submenu, Menu_Sub1, Menu_Sub2, Menu_Sub3,
    Control_First = 1000,
    Control_Listbox, Control_Listbox2,
};

BEGIN_EVENT_TABLE(OwnerDrawnFrame, wxFrame)
    EVT_MENU(Menu_Quit, OwnerDrawnFrame::OnQuit)
    EVT_LISTBOX(Control_Listbox, OwnerDrawnFrame::OnListboxSelect)
    EVT_CHECKLISTBOX(Control_Listbox, OwnerDrawnFrame::OnCheckboxToggle)
    EVT_COMMAND(Control_Listbox, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, 
                OwnerDrawnFrame::OnListboxDblClick)
END_EVENT_TABLE()

IMPLEMENT_APP(OwnerDrawnApp);

// init our app: create windows
bool OwnerDrawnApp::OnInit(void)
{
    wxInitAllImageHandlers();
    OwnerDrawnFrame *pFrame
        = new OwnerDrawnFrame(NULL, _T("wxWindows Ownerdraw Sample"),
                              50, 50, 450, 340);

    SetTopWindow(pFrame);

    return TRUE;
}

// create the menu bar for the main frame
void OwnerDrawnFrame::InitMenu()
{
    // Make a menubar
    wxMenu *file_menu = new wxMenu,
           *sub_menu  = new wxMenu;

    // vars used for menu construction
    wxMenuItem *pItem;
    wxFont fontLarge(18, wxROMAN, wxNORMAL, wxBOLD, FALSE),
           fontUlined(12, wxDEFAULT, wxNORMAL, wxNORMAL, TRUE),
           fontItalic(12, wxMODERN, wxITALIC, wxBOLD, FALSE),
           // should be at least of the size of bitmaps
           fontBmp(14, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);

    // sorry for my artistic skills...
    wxBitmap bmpBell(_T("bell"));
    wxBitmap bmpSound(_T("sound.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bmpNoSound(_T("nosound.png"), wxBITMAP_TYPE_PNG);

    // construct submenu
    pItem = new wxMenuItem(sub_menu, Menu_Sub1, _T("Submenu &first"), _T("large"));

    pItem->SetFont(fontLarge);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub2, _T("Submenu &second"), _T("italic"),
                           wxITEM_CHECK);
    pItem->SetFont(fontItalic);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub3, _T("Submenu &third"), _T("underlined"),
                           wxITEM_CHECK);
    pItem->SetFont(fontUlined);
    sub_menu->Append(pItem);

    // construct menu
    pItem = new wxMenuItem(file_menu, Menu_Test1, _T("&Uncheckable"), _T("red item"));
    pItem->SetFont(*wxITALIC_FONT);
    pItem->SetTextColour(wxColor(255, 0, 0));
    pItem->SetMarginWidth(23);
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Test2, _T("&Checkable"),
                           _T("checkable item"), wxITEM_CHECK);
    pItem->SetFont(*wxSMALL_FONT);
    file_menu->Append(pItem);
    file_menu->Check(Menu_Test2, TRUE);

    pItem = new wxMenuItem(file_menu, Menu_Test3, _T("&Disabled"), _T("disabled item"));
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);
    file_menu->Enable(Menu_Test3, FALSE);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Bitmap, _T("&Bell"),
                           _T("check/uncheck me!"), wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpBell);
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Bitmap2, _T("So&und"),
                           _T("icon changes!"), wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpSound, bmpNoSound);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Submenu, _T("&Sub menu"), _T(""),
                           wxITEM_CHECK, sub_menu);
    pItem->SetFont(*wxSWISS_FONT);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();
    pItem = new wxMenuItem(file_menu, Menu_Quit, _T("&Quit"), _T("Normal item"),
                           wxITEM_NORMAL);
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    SetMenuBar(menu_bar);
}

// main frame constructor
OwnerDrawnFrame::OwnerDrawnFrame(wxFrame *frame, wxChar *title,
                                 int x, int y, int w, int h)
         : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    // set the icon
    SetIcon(wxIcon(_T("mondrian")));

    // create the menu
    InitMenu();

    // create the status line
    const int widths[] = { -1, 60 };
    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    SetStatusText(_T("no selection"), 0);

    // make a panel with some controls
    wxPanel *pPanel = new wxPanel(this, -1, wxPoint(0, 0), 
                                  wxSize(400, 200), wxTAB_TRAVERSAL);

    // check list box
    static const wxChar* aszChoices[] = { _T("Hello"), _T("world"), _T("and"), 
                                          _T("goodbye"), _T("cruel"), _T("world"),
                                          _T("-------"), _T("owner-drawn"), _T("listbox") };

    wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
    unsigned int ui;
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
        astrChoices[ui] = aszChoices[ui];

    m_pListBox = new wxCheckListBox
        (
            pPanel,             // parent
            Control_Listbox,    // control id
            wxPoint(10, 10),    // listbox position
            wxSize(200, 200),   // listbox size
            WXSIZEOF(aszChoices), // number of strings
            astrChoices         // array of strings
        );

    delete [] astrChoices;

    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 )
    {
        m_pListBox->GetItem(ui)->SetBackgroundColour(wxColor(200, 200, 200));
    }

    m_pListBox->Check(2);

    // normal (but owner-drawn) listbox
    static const wxChar* aszColors[] = { _T("Red"), _T("Blue"), _T("Pink"),
                                         _T("Green"), _T("Yellow"), 
                                         _T("Black"), _T("Violet")  };
    struct { unsigned int r, g, b; } aColors[] =
        {
            {255,0,0}, {0,0,255}, {255,128,192},
            {0,255,0}, {255,255,128}, 
            {0,0,0}, {128,0,255}
        };

    astrChoices = new wxString[WXSIZEOF(aszColors)];

    for ( ui = 0; ui < WXSIZEOF(aszColors); ui++ )
    {
        astrChoices[ui] = aszColors[ui];
    }

    wxListBox *pListBox = new wxListBox
        (
            pPanel,              // parent
            Control_Listbox2,    // control id
            wxPoint(220, 10),    // listbox position
            wxDefaultSize,       // listbox size
            WXSIZEOF(aszColors),   // number of strings
            astrChoices,         // array of strings
            wxLB_OWNERDRAW,      // owner-drawn
            wxDefaultValidator,  //
            wxListBoxNameStr
        );

    for ( ui = 0; ui < WXSIZEOF(aszColors); ui++ )
    {
        pListBox->GetItem(ui)->SetTextColour(wxColor(aColors[ui].r,
                                                     aColors[ui].g, 
                                                     aColors[ui].b));
        // yellow on white is horrible...
        if ( ui == 4 )
        {
            pListBox->GetItem(ui)->SetBackgroundColour(wxColor(0, 0, 0));
        }

    }

    delete[] astrChoices;

    Show(TRUE);
}

OwnerDrawnFrame::~OwnerDrawnFrame()
{
}

void OwnerDrawnFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void OwnerDrawnFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageDialog dialog(this,
                           _T("Demo of owner-drawn controls\n"),
                           _T("About wxOwnerDrawn"), wxYES_NO | wxCANCEL);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnListboxSelect(wxCommandEvent& event)
{
    wxString strSelection;
    unsigned int nSel = event.GetSelection();
    strSelection.Printf(wxT("item %d selected (%schecked)"), nSel,
                        m_pListBox->IsChecked(nSel) ? wxT("") : wxT("not "));
    SetStatusText(strSelection);
}

void OwnerDrawnFrame::OnListboxDblClick(wxCommandEvent& event)
{
    wxString strSelection;
    strSelection.Printf(wxT("item %d double clicked"),
                        m_pListBox->GetSelection());
    wxMessageDialog dialog(this, strSelection);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnCheckboxToggle(wxCommandEvent& event)
{
    wxString strSelection;
    unsigned int nItem = event.GetInt();
    strSelection.Printf(wxT("item %d was %schecked"), nItem,
                        m_pListBox->IsChecked(nItem) ? wxT("") : wxT("un"));
    SetStatusText(strSelection);
}
