/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        searchctrl.cpp
// Purpose:     Shows wxSearchCtrl
// Author:      Robin Dunn
// Created:     9-Dec-2006
// Copyright:   (c) 2006
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SEARCHCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"
#include "wx/menu.h"

#include "wx/srchctrl.h"

#include "widgets.h"

#include "icons/text.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
     ID_SEARCH_CB = wxID_HIGHEST,
     ID_CANCEL_CB,
     ID_MENU_CB,

     ID_SEARCHMENU
};


// ----------------------------------------------------------------------------
// ColourPickerWidgetsPage
// ----------------------------------------------------------------------------

class SearchCtrlWidgetsPage : public WidgetsPage
{
public:
    SearchCtrlWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_srchCtrl; }
    virtual wxTextEntryBase *GetTextEntry() const wxOVERRIDE { return m_srchCtrl; }
    virtual void RecreateWidget() wxOVERRIDE;

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:

    void OnToggleSearchButton(wxCommandEvent&);
    void OnToggleCancelButton(wxCommandEvent&);
    void OnToggleSearchMenu(wxCommandEvent&);

    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);

    void OnSearch(wxCommandEvent& event);
    void OnSearchCancel(wxCommandEvent& event);

    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    wxMenu* CreateTestMenu();

    // (re)create the control
    void CreateControl();


    wxSearchCtrl*       m_srchCtrl;
    wxCheckBox*         m_searchBtnCheck;
    wxCheckBox*         m_cancelBtnCheck;
    wxCheckBox*         m_menuBtnCheck;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(SearchCtrlWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SearchCtrlWidgetsPage, WidgetsPage)
    EVT_CHECKBOX(ID_SEARCH_CB, SearchCtrlWidgetsPage::OnToggleSearchButton)
    EVT_CHECKBOX(ID_CANCEL_CB, SearchCtrlWidgetsPage::OnToggleCancelButton)
    EVT_CHECKBOX(ID_MENU_CB, SearchCtrlWidgetsPage::OnToggleSearchMenu)

    EVT_TEXT(wxID_ANY, SearchCtrlWidgetsPage::OnText)
    EVT_TEXT_ENTER(wxID_ANY, SearchCtrlWidgetsPage::OnTextEnter)

    EVT_SEARCH(wxID_ANY, SearchCtrlWidgetsPage::OnSearch)
    EVT_SEARCH_CANCEL(wxID_ANY, SearchCtrlWidgetsPage::OnSearchCancel)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMAC__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(SearchCtrlWidgetsPage, wxT("SearchCtrl"),
                       FAMILY_CTRLS | EDITABLE_CTRLS | ALL_CTRLS);

SearchCtrlWidgetsPage::SearchCtrlWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, text_xpm)
{
}

void SearchCtrlWidgetsPage::CreateContent()
{
    m_srchCtrl = NULL;

    CreateControl();


    wxSizer* box = new wxStaticBoxSizer(
        new wxStaticBox(this, -1, wxT("Options")),
        wxVERTICAL);

    m_searchBtnCheck = new wxCheckBox(this, ID_SEARCH_CB, wxT("Search button"));
    m_cancelBtnCheck = new wxCheckBox(this, ID_CANCEL_CB, wxT("Cancel button"));
    m_menuBtnCheck   = new wxCheckBox(this, ID_MENU_CB,   wxT("Search menu"));

    m_searchBtnCheck->SetValue(true);

    box->Add(m_searchBtnCheck, 0, wxALL, 5);
    box->Add(m_cancelBtnCheck, 0, wxALL, 5);
    box->Add(m_menuBtnCheck,   0, wxALL, 5);

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(box, 0,  wxALL|wxEXPAND, 15);
    sizer->Add(m_srchCtrl, 0, wxALL|wxALIGN_CENTER, 15);

    SetSizer(sizer);
}

void SearchCtrlWidgetsPage::CreateControl()
{
    if (m_srchCtrl)
        m_srchCtrl->Destroy();

    long style = GetAttrs().m_defaultFlags;

    m_srchCtrl = new wxSearchCtrl(this, -1, wxEmptyString, wxDefaultPosition,
                                  wxSize(150, -1), style);

    m_srchCtrl->Bind(wxEVT_SET_FOCUS, &SearchCtrlWidgetsPage::OnSetFocus, this);
    m_srchCtrl->Bind(wxEVT_KILL_FOCUS, &SearchCtrlWidgetsPage::OnKillFocus, this);
}

void SearchCtrlWidgetsPage::RecreateWidget()
{
    CreateControl();

    GetSizer()->Add(m_srchCtrl, wxSizerFlags().Centre().TripleBorder());

    Layout();
}

wxMenu* SearchCtrlWidgetsPage::CreateTestMenu()
{
    wxMenu* menu = new wxMenu;
    const int SEARCH_MENU_SIZE = 5;
    wxMenuItem* menuItem = menu->Append(wxID_ANY, wxT("Recent Searches"), wxT(""), wxITEM_NORMAL);
    menuItem->Enable(false);
    for ( int i = 0; i < SEARCH_MENU_SIZE; i++ )
    {
        wxString itemText = wxString::Format(wxT("item %i"),i);
        wxString tipText = wxString::Format(wxT("tip %i"),i);
        menu->Append(ID_SEARCHMENU+i, itemText, tipText, wxITEM_NORMAL);
    }
//     target->Connect(
//         ID_SEARCHMENU,
//         ID_SEARCHMENU+SEARCH_MENU_SIZE,
//         wxEVT_MENU,
//         wxCommandEventHandler(MySearchCtrl::OnSearchMenu)
//         );
    return menu;
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void SearchCtrlWidgetsPage::OnToggleSearchButton(wxCommandEvent&)
{
    m_srchCtrl->ShowSearchButton( m_searchBtnCheck->GetValue() );
}

void SearchCtrlWidgetsPage::OnToggleCancelButton(wxCommandEvent&)
{
    m_srchCtrl->ShowCancelButton( m_cancelBtnCheck->GetValue() );

}

void SearchCtrlWidgetsPage::OnToggleSearchMenu(wxCommandEvent&)
{
    if ( m_menuBtnCheck->GetValue() )
        m_srchCtrl->SetMenu( CreateTestMenu() );
    else
        m_srchCtrl->SetMenu(NULL);
}

void SearchCtrlWidgetsPage::OnText(wxCommandEvent& event)
{
    wxLogMessage("Search control: text changes, contents is \"%s\".",
                 event.GetString());
}

void SearchCtrlWidgetsPage::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage("Search control: enter pressed, contents is \"%s\".",
                 event.GetString());
}

void SearchCtrlWidgetsPage::OnSearch(wxCommandEvent& event)
{
    wxLogMessage("Search button: search for \"%s\".", event.GetString());
}

void SearchCtrlWidgetsPage::OnSearchCancel(wxCommandEvent& event)
{
    wxLogMessage("Cancel button pressed.");

    event.Skip();
}

void SearchCtrlWidgetsPage::OnSetFocus(wxFocusEvent& event)
{
    wxLogMessage("Search control got focus");

    event.Skip();
}

void SearchCtrlWidgetsPage::OnKillFocus(wxFocusEvent& event)
{
    wxLogMessage("Search control lost focus");

    event.Skip();
}

#endif  //  wxUSE_SEARCHCTRL
