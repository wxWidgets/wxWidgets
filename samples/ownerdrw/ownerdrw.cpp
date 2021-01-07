///////////////////////////////////////////////////////////////////////////////
// Name:        ownerdrw.cpp
// Purpose:     Owner-draw sample, for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include  "wx/ownerdrw.h"
#include  "wx/menuitem.h"
#include  "wx/checklst.h"

// Define a new application type
class OwnerDrawnApp: public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

// Define a new frame type
class OwnerDrawnFrame : public wxFrame
{
public:
    // ctor & dtor
    OwnerDrawnFrame(wxFrame *frame, const wxString& title, int x, int y, int w, int h);
    ~OwnerDrawnFrame(){}

    // notifications
    void OnQuit             (wxCommandEvent& event);
    void OnMenuToggle       (wxCommandEvent& event);
    void OnAbout            (wxCommandEvent& event);
    void OnListboxSelect    (wxCommandEvent& event);
    void OnCheckboxToggle   (wxCommandEvent& event);
    void OnListboxDblClick  (wxCommandEvent& event);
    bool OnClose            ()                        { return true; }

    wxDECLARE_EVENT_TABLE();

private:
    void InitMenu();

    wxCheckListBox *m_pListBox;
    wxMenuItem *pAboutItem;
};

enum
{
    Menu_Quit = 1,
    Menu_First = 100,
    Menu_Test1, Menu_Test2, Menu_Test3,
    Menu_Bitmap, Menu_Bitmap2,
    Menu_Submenu, Menu_Sub1, Menu_Sub2, Menu_Sub3,
    Menu_Toggle, Menu_About,
    Menu_Drawn1, Menu_Drawn2, Menu_Drawn3, Menu_Drawn4, Menu_Drawn5,
    Menu_Native1, Menu_Native2, Menu_Native3, Menu_Native4, Menu_Native5,
    Control_First = 1000,
    Control_Listbox, Control_Listbox2
};

wxBEGIN_EVENT_TABLE(OwnerDrawnFrame, wxFrame)
    EVT_MENU(Menu_Toggle, OwnerDrawnFrame::OnMenuToggle)
    EVT_MENU(Menu_About, OwnerDrawnFrame::OnAbout)
    EVT_MENU(Menu_Quit, OwnerDrawnFrame::OnQuit)
    EVT_LISTBOX(Control_Listbox, OwnerDrawnFrame::OnListboxSelect)
    EVT_CHECKLISTBOX(Control_Listbox, OwnerDrawnFrame::OnCheckboxToggle)
    EVT_COMMAND(Control_Listbox, wxEVT_LISTBOX_DCLICK,
                OwnerDrawnFrame::OnListboxDblClick)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(OwnerDrawnApp);

// init our app: create windows
bool OwnerDrawnApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;

    new OwnerDrawnFrame(NULL, "wxWidgets Ownerdraw Sample", 50, 50, 450, 340);

    return true;
}

// create the menu bar for the main frame
void OwnerDrawnFrame::InitMenu()
{
    // Make a menubar
    wxMenuItem *pItem;
    wxMenu *file_menu = new wxMenu;

#ifndef __WXUNIVERSAL__
    wxMenu *sub_menu  = new wxMenu;

    // vars used for menu construction
    wxFont fontLarge(wxFontInfo(18).Family(wxFONTFAMILY_ROMAN).Bold()),
           fontUlined(wxFontInfo(12).Underlined()),
           fontItalic(wxFontInfo(12).Italic().Bold()),
           // should be at least of the size of bitmaps
           fontBmp(wxFontInfo(14));

    // sorry for my artistic skills...
    wxBitmap bmpBell("bell"),
             bmpSound("sound"),
             bmpNoSound("nosound"),
             bmpInfo("info"),
             bmpInfo_mono("info_mono");

    // construct submenu
    pItem = new wxMenuItem(sub_menu, Menu_Sub1, "Submenu &first", "large");

    pItem->SetFont(fontLarge);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub2, "Submenu &second", "italic",
                           wxITEM_CHECK);
    pItem->SetFont(fontItalic);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub3, "Submenu &third", "underlined",
                           wxITEM_CHECK);
    pItem->SetFont(fontUlined);
    sub_menu->Append(pItem);

    // construct menu
    pItem = new wxMenuItem(file_menu, Menu_Test1, "&Uncheckable", "red item");
    pItem->SetFont(*wxITALIC_FONT);
    pItem->SetTextColour(wxColor(255, 0, 0));
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Test2, "&Checkable",
                           "checkable item", wxITEM_CHECK);
    pItem->SetFont(*wxSMALL_FONT);
    file_menu->Append(pItem);
    file_menu->Check(Menu_Test2, true);

    pItem = new wxMenuItem(file_menu, Menu_Test3, "&Disabled", "disabled item");
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);
    file_menu->Enable(Menu_Test3, false);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Bitmap, "&Bell",
                           "check/uncheck me!", wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpBell);
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Bitmap2, "So&und",
                           "icon changes!", wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpSound, bmpNoSound);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Submenu, "&Sub menu", "",
                           wxITEM_CHECK, sub_menu);
    pItem->SetFont(*wxSWISS_FONT);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();
    pItem = new wxMenuItem(file_menu, Menu_Toggle, "&Disable/Enable\tCtrl+D",
                          "enables/disables the About-Item", wxITEM_NORMAL);
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);

    // Of course Ctrl+RatherLongAccel will not work in this example:
    pAboutItem = new wxMenuItem(file_menu, Menu_About, "&About\tCtrl+RatherLongAccel",
                                "display program information", wxITEM_NORMAL);
    pAboutItem->SetBitmap(bmpInfo);
    pAboutItem->SetDisabledBitmap(bmpInfo_mono);
    file_menu->Append(pAboutItem);

    file_menu->AppendSeparator();
#endif

    pItem = new wxMenuItem(file_menu, Menu_Quit, "&Quit", "Normal item",
                           wxITEM_NORMAL);
    file_menu->Append(pItem);

    wxMenu* drawn_menu = new wxMenu;
    pItem = new wxMenuItem(drawn_menu, Menu_Drawn1, "&Menu item\tCtrl+K");
    drawn_menu->Append(pItem);

    drawn_menu->AppendSeparator();

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn2, "&Checked item",
        "check/uncheck me!", wxITEM_CHECK);
    drawn_menu->Append(pItem);
    drawn_menu->Check(Menu_Drawn2, true);

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn3, "&Radio item",
        "check/uncheck me!", wxITEM_RADIO);
    drawn_menu->Append(pItem);

    drawn_menu->AppendSeparator();

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn4, "&Disabled item\tCtrl+RatherLongAccel",
        "disabled item");
    pItem->Enable(false);
    drawn_menu->Append(pItem);

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn5, "&Other\tCtrl+O", "other item");
    pItem->SetTextColour(*wxRED);
    drawn_menu->Append(pItem);

    wxMenu* native_menu = new wxMenu;
    pItem = new wxMenuItem(native_menu, Menu_Native1, "&Menu item\tCtrl+K");
    native_menu->Append(pItem);

    native_menu->AppendSeparator();

    pItem = new wxMenuItem(native_menu, Menu_Native2, "&Checked item",
        "check/uncheck me!", wxITEM_CHECK);
    native_menu->Append(pItem);
    native_menu->Check(Menu_Native2, true);

    pItem = new wxMenuItem(native_menu, Menu_Native3, "&Radio item",
        "check/uncheck me!", wxITEM_RADIO);
    native_menu->Append(pItem);

    native_menu->AppendSeparator();

    pItem = new wxMenuItem(native_menu, Menu_Native4, "&Disabled item\tCtrl+RatherLongAccel",
        "disabled item");
    pItem->Enable(false);
    native_menu->Append(pItem);

    pItem = new wxMenuItem(native_menu, Menu_Native5, "&Other\tCtrl+O", "other item");
    native_menu->Append(pItem);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(drawn_menu, "&Drawn");
    menu_bar->Append(native_menu, "&Native");
    SetMenuBar(menu_bar);
}

// main frame constructor
OwnerDrawnFrame::OwnerDrawnFrame(wxFrame *frame, const wxString& title,
                                 int x, int y, int w, int h)
         : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    // set the icon
    SetIcon(wxICON(sample));

    // create the menu
    InitMenu();

#if wxUSE_STATUSBAR
    // create the status line
    const int widths[] = { -1, 60 };
    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    SetStatusText("no selection", 0);
#endif // wxUSE_STATUSBAR

    // make a panel with some controls
    wxPanel *pPanel = new wxPanel(this);

    // check list box
    static const wxString aszChoices[] = { "Hello", "world", "and",
                                          "goodbye", "cruel", "world",
                                          "-------", "owner-drawn", "listbox" };

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
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
        m_pListBox->GetItem(ui)->SetBackgroundColour(wxColor(200, 200, 200));
#endif
    }

    m_pListBox->Check(2);

    // normal (but owner-drawn) listbox
    static const wxString aszColors[] = { "Red", "Blue", "Pink",
                                         "Green", "Yellow",
                                         "Black", "Violet"  };

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
            wxSize(200, 200),    // listbox size
            WXSIZEOF(aszColors), // number of strings
            astrChoices,         // array of strings
            wxLB_OWNERDRAW       // owner-drawn
        );

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

    struct { unsigned char r, g, b; } aColors[] =
        {
            {255,0,0}, {0,0,255}, {255,128,192},
            {0,255,0}, {255,255,128},
            {0,0,0}, {128,0,255}
        };

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

#else
    wxUnusedVar( pListBox );
#endif

    delete[] astrChoices;

    Show(true);
}

void OwnerDrawnFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void OwnerDrawnFrame::OnMenuToggle(wxCommandEvent& WXUNUSED(event))
{
    // This example shows the use of bitmaps in ownerdrawn menuitems and is not a good
    // example on how to enable and disable menuitems - this should be done with the help of
    // EVT_UPDATE_UI and EVT_UPDATE_UI_RANGE !
    pAboutItem->Enable( pAboutItem->IsEnabled() ? false : true );
}

void OwnerDrawnFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this,
                           "Demo of owner-drawn controls\n",
                           "About wxOwnerDrawn", wxYES_NO | wxCANCEL);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnListboxSelect(wxCommandEvent& event)
{
#if wxUSE_STATUSBAR
    wxString strSelection;
    unsigned int nSel = event.GetSelection();
    strSelection.Printf("item %d selected (%schecked)", nSel,
                        m_pListBox->IsChecked(nSel) ? "" : "not ");
    SetStatusText(strSelection);
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void OwnerDrawnFrame::OnListboxDblClick(wxCommandEvent& WXUNUSED(event))
{
    wxString strSelection;
    strSelection.Printf("item %d double clicked",
                        m_pListBox->GetSelection());
    wxMessageDialog dialog(this, strSelection);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnCheckboxToggle(wxCommandEvent& event)
{
#if wxUSE_STATUSBAR
    wxString strSelection;
    unsigned int nItem = event.GetInt();
    strSelection.Printf("item %d was %schecked", nItem,
                        m_pListBox->IsChecked(nItem) ? "" : "un");
    SetStatusText(strSelection);
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}
