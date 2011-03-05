///////////////////////////////////////////////////////////////////////////////
// Name:        ownerdrw.cpp
// Purpose:     Owner-draw sample, for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
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
#include  "wx/checklst.h"

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
    OwnerDrawnFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h);
    ~OwnerDrawnFrame(){};

    // notifications
    void OnQuit             (wxCommandEvent& event);
    void OnMenuToggle       (wxCommandEvent& event);
    void OnAbout            (wxCommandEvent& event);
    void OnListboxSelect    (wxCommandEvent& event);
    void OnCheckboxToggle   (wxCommandEvent& event);
    void OnListboxDblClick  (wxCommandEvent& event);
    bool OnClose            ()                        { return true; }

    DECLARE_EVENT_TABLE()

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

BEGIN_EVENT_TABLE(OwnerDrawnFrame, wxFrame)
    EVT_MENU(Menu_Toggle, OwnerDrawnFrame::OnMenuToggle)
    EVT_MENU(Menu_About, OwnerDrawnFrame::OnAbout)
    EVT_MENU(Menu_Quit, OwnerDrawnFrame::OnQuit)
    EVT_LISTBOX(Control_Listbox, OwnerDrawnFrame::OnListboxSelect)
    EVT_CHECKLISTBOX(Control_Listbox, OwnerDrawnFrame::OnCheckboxToggle)
    EVT_COMMAND(Control_Listbox, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
                OwnerDrawnFrame::OnListboxDblClick)
END_EVENT_TABLE()

IMPLEMENT_APP(OwnerDrawnApp)

// init our app: create windows
bool OwnerDrawnApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;

    OwnerDrawnFrame *pFrame
        = new OwnerDrawnFrame(NULL, wxT("wxWidgets Ownerdraw Sample"),
                              50, 50, 450, 340);

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
    wxFont fontLarge(18, wxROMAN, wxNORMAL, wxBOLD, false),
           fontUlined(12, wxDEFAULT, wxNORMAL, wxNORMAL, true),
           fontItalic(12, wxMODERN, wxITALIC, wxBOLD, false),
           // should be at least of the size of bitmaps
           fontBmp(14, wxDEFAULT, wxNORMAL, wxNORMAL, false);

    // sorry for my artistic skills...
    wxBitmap bmpBell(wxT("bell")),
             bmpSound(wxT("sound")),
             bmpNoSound(wxT("nosound")),
             bmpInfo(wxT("info")),
             bmpInfo_mono(wxT("info_mono"));

    // construct submenu
    pItem = new wxMenuItem(sub_menu, Menu_Sub1, wxT("Submenu &first"), wxT("large"));

    pItem->SetFont(fontLarge);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub2, wxT("Submenu &second"), wxT("italic"),
                           wxITEM_CHECK);
    pItem->SetFont(fontItalic);
    sub_menu->Append(pItem);

    pItem = new wxMenuItem(sub_menu, Menu_Sub3, wxT("Submenu &third"), wxT("underlined"),
                           wxITEM_CHECK);
    pItem->SetFont(fontUlined);
    sub_menu->Append(pItem);

    // construct menu
    pItem = new wxMenuItem(file_menu, Menu_Test1, wxT("&Uncheckable"), wxT("red item"));
    pItem->SetFont(*wxITALIC_FONT);
    pItem->SetTextColour(wxColor(255, 0, 0));
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Test2, wxT("&Checkable"),
                           wxT("checkable item"), wxITEM_CHECK);
    pItem->SetFont(*wxSMALL_FONT);
    file_menu->Append(pItem);
    file_menu->Check(Menu_Test2, true);

    pItem = new wxMenuItem(file_menu, Menu_Test3, wxT("&Disabled"), wxT("disabled item"));
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);
    file_menu->Enable(Menu_Test3, false);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Bitmap, wxT("&Bell"),
                           wxT("check/uncheck me!"), wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpBell);
    file_menu->Append(pItem);

    pItem = new wxMenuItem(file_menu, Menu_Bitmap2, wxT("So&und"),
                           wxT("icon changes!"), wxITEM_CHECK);
    pItem->SetFont(fontBmp);
    pItem->SetBitmaps(bmpSound, bmpNoSound);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, Menu_Submenu, wxT("&Sub menu"), wxT(""),
                           wxITEM_CHECK, sub_menu);
    pItem->SetFont(*wxSWISS_FONT);
    file_menu->Append(pItem);

    file_menu->AppendSeparator();
    pItem = new wxMenuItem(file_menu, Menu_Toggle, wxT("&Disable/Enable\tCtrl+D"),
                          wxT("enables/disables the About-Item"), wxITEM_NORMAL);
    pItem->SetFont(*wxNORMAL_FONT);
    file_menu->Append(pItem);

    // Of course Ctrl+RatherLongAccel will not work in this example:
    pAboutItem = new wxMenuItem(file_menu, Menu_About, wxT("&About\tCtrl+RatherLongAccel"),
                                wxT("display program information"), wxITEM_NORMAL);
    pAboutItem->SetBitmap(bmpInfo);
    pAboutItem->SetDisabledBitmap(bmpInfo_mono);
    file_menu->Append(pAboutItem);

    file_menu->AppendSeparator();
#endif

    pItem = new wxMenuItem(file_menu, Menu_Quit, wxT("&Quit"), wxT("Normal item"),
                           wxITEM_NORMAL);
    file_menu->Append(pItem);

    wxMenu* drawn_menu = new wxMenu;
    pItem = new wxMenuItem(drawn_menu, Menu_Drawn1, wxT("&Menu item\tCtrl+K"));
    drawn_menu->Append(pItem);

    drawn_menu->AppendSeparator();

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn2, wxT("&Checked item"),
        wxT("check/uncheck me!"), wxITEM_CHECK);
    drawn_menu->Append(pItem);
    drawn_menu->Check(Menu_Drawn2, true);

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn3, wxT("&Radio item"),
        wxT("check/uncheck me!"), wxITEM_RADIO);
    drawn_menu->Append(pItem);

    drawn_menu->AppendSeparator();

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn4, wxT("&Disabled item\tCtrl+RatherLongAccel"),
        wxT("disabled item"));
    pItem->Enable(false);
    drawn_menu->Append(pItem);

    pItem = new wxMenuItem(drawn_menu, Menu_Drawn5, wxT("&Other\tCtrl+O"), wxT("other item"));
    pItem->SetTextColour(*wxRED);
    drawn_menu->Append(pItem);

    wxMenu* native_menu = new wxMenu;
    pItem = new wxMenuItem(native_menu, Menu_Native1, wxT("&Menu item\tCtrl+K"));
    native_menu->Append(pItem);

    native_menu->AppendSeparator();

    pItem = new wxMenuItem(native_menu, Menu_Native2, wxT("&Checked item"),
        wxT("check/uncheck me!"), wxITEM_CHECK);
    native_menu->Append(pItem);
    native_menu->Check(Menu_Native2, true);

    pItem = new wxMenuItem(native_menu, Menu_Native3, wxT("&Radio item"),
        wxT("check/uncheck me!"), wxITEM_RADIO);
    native_menu->Append(pItem);

    native_menu->AppendSeparator();

    pItem = new wxMenuItem(native_menu, Menu_Native4, wxT("&Disabled item\tCtrl+RatherLongAccel"),
        wxT("disabled item"));
    pItem->Enable(false);
    native_menu->Append(pItem);

    pItem = new wxMenuItem(native_menu, Menu_Native5, wxT("&Other\tCtrl+O"), wxT("other item"));
    native_menu->Append(pItem);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(drawn_menu, wxT("&Drawn"));
    menu_bar->Append(native_menu, wxT("&Native"));
    SetMenuBar(menu_bar);
}

// main frame constructor
OwnerDrawnFrame::OwnerDrawnFrame(wxFrame *frame, const wxChar *title,
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
    SetStatusText(wxT("no selection"), 0);
#endif // wxUSE_STATUSBAR

    // make a panel with some controls
    wxPanel *pPanel = new wxPanel(this);

    // check list box
    static const wxChar* aszChoices[] = { wxT("Hello"), wxT("world"), wxT("and"),
                                          wxT("goodbye"), wxT("cruel"), wxT("world"),
                                          wxT("-------"), wxT("owner-drawn"), wxT("listbox") };

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
    static const wxChar* aszColors[] = { wxT("Red"), wxT("Blue"), wxT("Pink"),
                                         wxT("Green"), wxT("Yellow"),
                                         wxT("Black"), wxT("Violet")  };

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
                           wxT("Demo of owner-drawn controls\n"),
                           wxT("About wxOwnerDrawn"), wxYES_NO | wxCANCEL);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnListboxSelect(wxCommandEvent& event)
{
#if wxUSE_STATUSBAR
    wxString strSelection;
    unsigned int nSel = event.GetSelection();
    strSelection.Printf(wxT("item %d selected (%schecked)"), nSel,
                        m_pListBox->IsChecked(nSel) ? wxT("") : wxT("not "));
    SetStatusText(strSelection);
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void OwnerDrawnFrame::OnListboxDblClick(wxCommandEvent& WXUNUSED(event))
{
    wxString strSelection;
    strSelection.Printf(wxT("item %d double clicked"),
                        m_pListBox->GetSelection());
    wxMessageDialog dialog(this, strSelection);
    dialog.ShowModal();
}

void OwnerDrawnFrame::OnCheckboxToggle(wxCommandEvent& event)
{
#if wxUSE_STATUSBAR
    wxString strSelection;
    unsigned int nItem = event.GetInt();
    strSelection.Printf(wxT("item %d was %schecked"), nItem,
                        m_pListBox->IsChecked(nItem) ? wxT("") : wxT("un"));
    SetStatusText(strSelection);
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}
