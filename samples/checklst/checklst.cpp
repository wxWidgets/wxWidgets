///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     wxCheckListBox sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    //#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __WXMSW__
    #include  "wx/ownerdrw.h"
#endif

#include  "wx/log.h"

#include  "wx/sizer.h"
#include  "wx/menuitem.h"
#include  "wx/checklst.h"

// Define a new application type
class CheckListBoxApp: public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class CheckListBoxFrame : public wxFrame
{
public:
    // ctor & dtor
    CheckListBoxFrame(wxFrame *frame, const wxChar *title,
                      int x, int y, int w, int h);
    virtual ~CheckListBoxFrame(){};

    // notifications
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnCheckFirstItem(wxCommandEvent& event);
    void OnUncheckFirstItem(wxCommandEvent& event);
    void OnToggleFirstItem(wxCommandEvent& event);
    void OnToggleSelection(wxCommandEvent& event);
    void OnAddItems(wxCommandEvent& event);

    void OnListboxSelect(wxCommandEvent& event);
    void OnCheckboxToggle(wxCommandEvent& event);
    void OnListboxDblClick(wxCommandEvent& event);

    void OnButtonUp(wxCommandEvent& event);
    void OnButtonDown(wxCommandEvent& event);

private:
    void CreateCheckListbox(long flags = 0);

    void OnButtonMove(bool up);

    void AdjustColour(size_t index);

    wxPanel *m_panel;

    wxCheckListBox *m_pListBox;

    DECLARE_EVENT_TABLE()
};

enum
{
    Menu_About = 100,
    Menu_Quit,

    Menu_CheckFirst,
    Menu_UncheckFirst,
    Menu_ToggleFirst,
    Menu_Selection,
    Menu_AddItems,

    Control_First = 1000,
    Control_Listbox,
    Btn_Up,
    Btn_Down
};

BEGIN_EVENT_TABLE(CheckListBoxFrame, wxFrame)
    EVT_MENU(Menu_About, CheckListBoxFrame::OnAbout)
    EVT_MENU(Menu_Quit, CheckListBoxFrame::OnQuit)

    EVT_MENU(Menu_CheckFirst, CheckListBoxFrame::OnCheckFirstItem)
    EVT_MENU(Menu_UncheckFirst, CheckListBoxFrame::OnUncheckFirstItem)
    EVT_MENU(Menu_ToggleFirst, CheckListBoxFrame::OnToggleFirstItem)
    EVT_MENU(Menu_Selection, CheckListBoxFrame::OnToggleSelection)
    EVT_MENU(Menu_AddItems, CheckListBoxFrame::OnAddItems)

    EVT_LISTBOX(Control_Listbox, CheckListBoxFrame::OnListboxSelect)
    EVT_CHECKLISTBOX(Control_Listbox, CheckListBoxFrame::OnCheckboxToggle)
    EVT_LISTBOX_DCLICK(Control_Listbox, CheckListBoxFrame::OnListboxDblClick)

    EVT_BUTTON(Btn_Up, CheckListBoxFrame::OnButtonUp)
    EVT_BUTTON(Btn_Down, CheckListBoxFrame::OnButtonDown)
END_EVENT_TABLE()

IMPLEMENT_APP(CheckListBoxApp);

// init our app: create windows
bool CheckListBoxApp::OnInit(void)
{
    CheckListBoxFrame *pFrame = new CheckListBoxFrame
                                    (
                                     NULL,
                                     _T("wxWidgets Checklistbox Sample"),
                                     50, 50, 480, 320
                                    );
    SetTopWindow(pFrame);

    return true;
}

// main frame constructor
CheckListBoxFrame::CheckListBoxFrame(wxFrame *frame,
                                     const wxChar *title,
                                     int x, int y, int w, int h)
                 : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
#if wxUSE_STATUSBAR
    // create the status line
    const int widths[] = { -1, 60 };
    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    wxLogStatus(this, _T("no selection"));
#endif // wxUSE_STATUSBAR

    // Make a menubar
    // --------------

    // file submenu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Menu_About, _T("&About...\tF1"));
    menuFile->AppendSeparator();
    menuFile->Append(Menu_Quit, _T("E&xit\tAlt-X"));

    // listbox submenu
    wxMenu *menuList = new wxMenu;
    menuList->Append(Menu_CheckFirst, _T("Check the first item\tCtrl-C"));
    menuList->Append(Menu_UncheckFirst, _T("Uncheck the first item\tCtrl-U"));
    menuList->Append(Menu_ToggleFirst, _T("Toggle the first item\tCtrl-T"));
    menuList->AppendSeparator();
    menuList->AppendCheckItem(Menu_AddItems, _T("Add more items\tCtrl-A"));
    menuList->AppendSeparator();
    menuList->AppendCheckItem(Menu_Selection, _T("Multiple selection\tCtrl-M"));

    // put it all together
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menuFile, _T("&File"));
    menu_bar->Append(menuList, _T("&List"));
    SetMenuBar(menu_bar);

    // make a panel with some controls
    m_panel = new wxPanel(this, wxID_ANY, wxPoint(0, 0),
                          wxSize(400, 200), wxTAB_TRAVERSAL);

    CreateCheckListbox();

    // create buttons for moving the items around
    wxButton *button1 = new wxButton(m_panel, Btn_Up, _T("   &Up  "), wxPoint(420, 90));
    wxButton *button2 = new wxButton(m_panel, Btn_Down, _T("&Down"), wxPoint(420, 120));


    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add( m_pListBox, 1, wxGROW|wxALL, 10 );

    wxBoxSizer *bottomsizer = new wxBoxSizer( wxHORIZONTAL );

    bottomsizer->Add( button1, 0, wxALL, 10 );
    bottomsizer->Add( button2, 0, wxALL, 10 );

    mainsizer->Add( bottomsizer, 0, wxCENTER );

    // tell frame to make use of sizer (or constraints, if any)
    m_panel->SetAutoLayout( true );
    m_panel->SetSizer( mainsizer );

    // don't allow frame to get smaller than what the sizers tell ye
    mainsizer->SetSizeHints( this );

    Show(true);
}

void CheckListBoxFrame::CreateCheckListbox(long flags)
{
    // check list box
    static const wxChar *aszChoices[] =
    {
        _T("Zeroth"),
        _T("First"), _T("Second"), _T("Third"),
        _T("Fourth"), _T("Fifth"), _T("Sixth"),
        _T("Seventh"), _T("Eighth"), _T("Nineth")
    };

    wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
    unsigned int ui;
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
        astrChoices[ui] = aszChoices[ui];

    m_pListBox = new wxCheckListBox
        (
         m_panel,               // parent
         Control_Listbox,       // control id
         wxPoint(10, 10),       // listbox poistion
         wxSize(400, 100),      // listbox size
         WXSIZEOF(aszChoices),  // number of strings
         astrChoices,           // array of strings
         flags
        );

    //m_pListBox->SetBackgroundColour(*wxGREEN);

    delete [] astrChoices;

    // set grey background for every second entry
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 ) {
        AdjustColour(ui);
    }

    m_pListBox->Check(2);
    m_pListBox->Select(3);
}

void CheckListBoxFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void CheckListBoxFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Demo of wxCheckListBox control\n(c) Vadim Zeitlin 1998-2002"),
                 wxT("About wxCheckListBox"),
                 wxICON_INFORMATION, this);
}

void CheckListBoxFrame::OnCheckFirstItem(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_pListBox->IsEmpty() )
        m_pListBox->Check(0);
}

void CheckListBoxFrame::OnUncheckFirstItem(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_pListBox->IsEmpty() )
        m_pListBox->Check(0, false);
}

void CheckListBoxFrame::OnToggleFirstItem(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_pListBox->IsEmpty() )
        m_pListBox->Check(0, !m_pListBox->IsChecked(0));
}

void CheckListBoxFrame::OnAddItems(wxCommandEvent& WXUNUSED(event))
{
    static size_t s_nItem = 0;
    wxArrayString items;
    items.Add(wxString::Format(_T("New item %lu"), (unsigned long)++s_nItem));
    items.Add(wxString::Format(_T("New item %lu"), (unsigned long)++s_nItem));
    items.Add(wxString::Format(_T("New item %lu"), (unsigned long)++s_nItem));

    m_pListBox->InsertItems(items, 0);//m_pListBox->GetCount());
}

void CheckListBoxFrame::OnToggleSelection(wxCommandEvent& event)
{
    wxSizer *sizer = m_panel->GetSizer();

    sizer->Detach( m_pListBox );
    delete m_pListBox;

    CreateCheckListbox(event.IsChecked() ? wxLB_EXTENDED : 0);

    sizer->Insert(0, m_pListBox, 1, wxGROW | wxALL, 10);

    m_panel->Layout();
}

void CheckListBoxFrame::OnListboxSelect(wxCommandEvent& event)
{
    int nSel = event.GetSelection();
    wxLogStatus(this, wxT("Item %d selected (%schecked)"), nSel,
                      m_pListBox->IsChecked(nSel) ? wxT("") : wxT("not "));
}

void CheckListBoxFrame::OnListboxDblClick(wxCommandEvent& WXUNUSED(event))
{
    int selection = -1;
    if(m_pListBox->GetWindowStyle() & wxLB_EXTENDED)
    {
        wxArrayInt list;
        m_pListBox->GetSelections(list);
        if(list.Count()==1)
        {
            selection = list.Item(0);
        }
    }
    else
    {
        selection = m_pListBox->GetSelection();
    }

    wxString strSelection;
    if ( selection != -1 )
    {
        strSelection.Printf(wxT("Item %d double clicked"), selection);
    }
    else
    {
        strSelection = wxT("List double clicked in multiple selection mode");
    }
    wxMessageDialog dialog(this, strSelection, wxT("wxCheckListBox message"), wxICON_INFORMATION);
    dialog.ShowModal();
}

void CheckListBoxFrame::OnCheckboxToggle(wxCommandEvent& event)
{
    unsigned int nItem = event.GetInt();

    wxLogStatus(this, wxT("item %d was %schecked"), nItem,
                      m_pListBox->IsChecked(nItem) ? wxT("") : wxT("un"));
}

void CheckListBoxFrame::OnButtonUp(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(true);
}

void CheckListBoxFrame::OnButtonDown(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(false);
}

void CheckListBoxFrame::OnButtonMove(bool up)
{
    int selection = -1;
    if(m_pListBox->GetWindowStyle() & wxLB_EXTENDED)
    {
        wxArrayInt list;
        m_pListBox->GetSelections(list);
        if(list.Count()==1)
        {
            selection = list.Item(0);
        }
    }
    else
    {
        selection = m_pListBox->GetSelection();
    }
    if ( selection != -1 )
    {
        wxString label = m_pListBox->GetString(selection);

        int positionNew = up ? selection - 1 : selection + 2;
        if ( positionNew < 0 || positionNew > m_pListBox->GetCount() )
        {
            wxLogStatus(this, wxT("Can't move this item %s"), up ? wxT("up") : wxT("down"));
        }
        else
        {
            bool wasChecked = m_pListBox->IsChecked(selection);

            int positionOld = up ? selection + 1 : selection;

            // insert the item
            m_pListBox->InsertItems(1, &label, positionNew);

            // and delete the old one
            m_pListBox->Delete(positionOld);

            int selectionNew = up ? positionNew : positionNew - 1;
            m_pListBox->Check(selectionNew, wasChecked);
            m_pListBox->SetSelection(selectionNew);

            AdjustColour(selection);
            AdjustColour(selectionNew);

            wxLogStatus(this, wxT("Item moved %s"), up ? wxT("up") : wxT("down"));
        }
    }
    else
    {
        wxLogStatus(this, wxT("Please select single item"));
    }
}

// not implemented in ports other than (native) MSW yet
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
void CheckListBoxFrame::AdjustColour(size_t index)
{
    // even items have grey backround, odd ones - white
    unsigned char c = index % 2 ? 255 : 200;
    m_pListBox->GetItem(index)->SetBackgroundColour(wxColor(c, c, c));
}
#else
void CheckListBoxFrame::AdjustColour(size_t WXUNUSED(index))
{
}
#endif // wxMSW
