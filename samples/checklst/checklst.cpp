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
    CheckListBoxFrame(wxFrame *frame, const char *title,
            int x, int y, int w, int h);
    ~CheckListBoxFrame();

    // notifications
    void OnQuit           (wxCommandEvent& event);
    void OnAbout          (wxCommandEvent& event);
    void OnListboxSelect  (wxCommandEvent& event);
    void OnCheckboxToggle (wxCommandEvent& event);
    void OnListboxDblClick(wxCommandEvent& event);
    void OnButtonUp       (wxCommandEvent& event);
    void OnButtonDown     (wxCommandEvent& event);

private:
    void OnButtonMove(bool up);

    wxCheckListBox *m_pListBox;

    DECLARE_EVENT_TABLE()
};

enum
{
    Menu_Quit = 1,
    Control_First = 1000,
    Control_Listbox,
    Btn_Up,
    Btn_Down
};

BEGIN_EVENT_TABLE(CheckListBoxFrame, wxFrame)
    EVT_MENU(Menu_Quit, CheckListBoxFrame::OnQuit)

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
                                     "wxWindows Checklistbox Sample",
                                     50, 50, 480, 320
                                    );
    SetTopWindow(pFrame);

    return TRUE;
}

// main frame constructor
CheckListBoxFrame::CheckListBoxFrame(wxFrame *frame,
        const char *title,
        int x, int y, int w, int h)
: wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    // create the status line
    const int widths[] = { -1, 60 };
    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    wxLogStatus(this, _T("no selection"));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    // construct submenu
    file_menu->Append(Menu_Quit, "E&xit");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    SetMenuBar(menu_bar);

    // make a panel with some controls
    wxPanel *panel = new wxPanel(this, -1, wxPoint(0, 0),
                                 wxSize(400, 200), wxTAB_TRAVERSAL);

    // check list box
    static const char* aszChoices[] =
    {
        "Zeroth",
        "First", "Second", "Third",
        "Fourth", "Fifth", "Sixth",
        "Seventh", "Eighth", "Nineth"
    };

    wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
    unsigned int ui;
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
        astrChoices[ui] = aszChoices[ui];

    m_pListBox = new wxCheckListBox
        (
         panel,                 // parent
         Control_Listbox,       // control id
         wxPoint(10, 10),       // listbox poistion
         wxSize(400, 100),      // listbox size
         WXSIZEOF(aszChoices),  // number of strings
         astrChoices            // array of strings
        );

    //m_pListBox->SetBackgroundColour(*wxGREEN);

    delete [] astrChoices;

    // not implemented in other ports yet
#ifdef __WXMSW__
    // set grey background for every second entry
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 ) {
        m_pListBox->GetItem(ui)->SetBackgroundColour(wxColor(200, 200, 200));
    }
#endif // wxGTK

    m_pListBox->Check(2);

    // create buttons for moving the items around
    wxButton *button1 = new wxButton(panel, Btn_Up, "   &Up  ", wxPoint(420, 90));
    wxButton *button2 = new wxButton(panel, Btn_Down, "&Down", wxPoint(420, 120));


    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );
  
    mainsizer->Add( m_pListBox, 1, wxGROW|wxALL, 10 );
    
    wxBoxSizer *bottomsizer = new wxBoxSizer( wxHORIZONTAL );
    
    bottomsizer->Add( button1, 0, wxALL, 10 );
    bottomsizer->Add( button2, 0, wxALL, 10 );

    mainsizer->Add( bottomsizer, 0, wxCENTER );
  
    // tell frame to make use of sizer (or constraints, if any)
    SetAutoLayout( TRUE );

    // set frame to minimum size
    mainsizer->Fit( this );  
  
    // don't allow frame to get smaller than what the sizers tell ye
    mainsizer->SetSizeHints( this );  
  
    SetSizer( mainsizer );

  
    Show(TRUE);
}

CheckListBoxFrame::~CheckListBoxFrame()
{
}

void CheckListBoxFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void CheckListBoxFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Demo of wxCheckListBox control\n"
                    "© Vadim Zeitlin 1998-1999"),
                 _T("About wxCheckListBox"),
                 wxICON_INFORMATION, this);
}

void CheckListBoxFrame::OnListboxSelect(wxCommandEvent& event)
{
    int nSel = event.GetSelection();
    wxLogStatus(this, _T("item %d selected (%schecked)"), nSel,
                      m_pListBox->IsChecked(nSel) ? _T("") : _T("not "));
}

void CheckListBoxFrame::OnListboxDblClick(wxCommandEvent& WXUNUSED(event))
{
    wxString strSelection;
    strSelection.sprintf(_T("item %d double clicked"), m_pListBox->GetSelection());
    wxMessageDialog dialog(this, strSelection);
    dialog.ShowModal();
}

void CheckListBoxFrame::OnCheckboxToggle(wxCommandEvent& event)
{
    unsigned int nItem = event.GetInt();

    wxLogStatus(this, _T("item %d was %schecked"), nItem,
                      m_pListBox->IsChecked(nItem) ? _T("") : _T("un"));
}

void CheckListBoxFrame::OnButtonUp(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(TRUE);
}

void CheckListBoxFrame::OnButtonDown(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(FALSE);
}

void CheckListBoxFrame::OnButtonMove(bool up)
{
    int selection = m_pListBox->GetSelection();
    if ( selection != -1 )
    {
        wxString label = m_pListBox->GetString(selection);

        int positionNew = up ? selection - 1 : selection + 2;
        if ( positionNew < 0 || positionNew > m_pListBox->Number() )
        {
            wxLogStatus(this, _T("Can't move this item %s"), up ? _T("up") : _T("down"));
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

            wxLogStatus(this, _T("Item moved %s"), up ? _T("up") : _T("down"));
        }
    }
    else
    {
        wxLogStatus(this, _T("Please select an item"));
    }
}
