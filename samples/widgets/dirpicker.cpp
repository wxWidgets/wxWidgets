/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        dirpicker.cpp
// Purpose:     Shows wxDirPickerCtrl
// Author:      Francesco Montorsi
// Created:     20/6/2006
// Copyright:   (c) 2006 Francesco Montorsi
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


#if wxUSE_DIRPICKERCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/radiobox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"
#include "wx/imaglist.h"

#include "wx/filepicker.h"
#include "widgets.h"

#include "icons/dirpicker.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    PickerPage_Reset = wxID_HIGHEST,
    PickerPage_Dir,
    PickerPage_SetDir
};


// ----------------------------------------------------------------------------
// DirPickerWidgetsPage
// ----------------------------------------------------------------------------

class DirPickerWidgetsPage : public WidgetsPage
{
public:
    DirPickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_dirPicker; }
    virtual void RecreateWidget() wxOVERRIDE { RecreatePicker(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:

    // called only once at first construction
    void CreatePicker();

    // called to recreate an existing control
    void RecreatePicker();

    // restore the checkboxes state to the initial values
    void Reset();


    void OnDirChange(wxFileDirPickerEvent &ev);
    void OnCheckBox(wxCommandEvent &ev);
    void OnButtonReset(wxCommandEvent &ev);
    void OnButtonSetDir(wxCommandEvent &ev);


    // the picker
    wxDirPickerCtrl *m_dirPicker;


    // other controls
    // --------------

    wxCheckBox *m_chkDirTextCtrl,
               *m_chkDirChangeDir,
               *m_chkDirMustExist,
               *m_chkSmall;
    wxTextCtrl *m_textInitialDir;

    wxBoxSizer *m_sizer;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(DirPickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DirPickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(PickerPage_Reset, DirPickerWidgetsPage::OnButtonReset)
    EVT_BUTTON(PickerPage_SetDir, DirPickerWidgetsPage::OnButtonSetDir)

    EVT_DIRPICKER_CHANGED(PickerPage_Dir, DirPickerWidgetsPage::OnDirChange)

    EVT_CHECKBOX(wxID_ANY, DirPickerWidgetsPage::OnCheckBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK20__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(DirPickerWidgetsPage, "DirPicker",
                       PICKER_CTRLS | FAMILY_CTRLS);

DirPickerWidgetsPage::DirPickerWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, dirpicker_xpm)
{
}

void DirPickerWidgetsPage::CreateContent()
{
    // left pane
    wxSizer *boxleft = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *dirbox = new wxStaticBoxSizer(wxVERTICAL, this, "&DirPicker style");
    m_chkDirTextCtrl = CreateCheckBoxAndAddToSizer(dirbox, "With textctrl");
    m_chkDirMustExist = CreateCheckBoxAndAddToSizer(dirbox, "Dir must exist");
    m_chkDirChangeDir = CreateCheckBoxAndAddToSizer(dirbox, "Change working dir");
    m_chkSmall = CreateCheckBoxAndAddToSizer(dirbox, "&Small version");
    boxleft->Add(dirbox, 0, wxALL|wxGROW, 5);

    boxleft->Add(CreateSizerWithTextAndButton
                 (
                    PickerPage_SetDir,
                    "&Initial directory",
                    wxID_ANY,
                    &m_textInitialDir
                 ), wxSizerFlags().Expand().Border());

    boxleft->AddSpacer(10);

    boxleft->Add(new wxButton(this, PickerPage_Reset, "&Reset"),
                 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    Reset();    // set checkboxes state

    // create pickers
    m_dirPicker = NULL;
    CreatePicker();

    // right pane
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
    m_sizer->Add(m_dirPicker, 0, wxEXPAND|wxALL, 5);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer

    // global pane
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(boxleft, 0, wxGROW|wxALL, 5);
    sz->Add(m_sizer, 1, wxGROW|wxALL, 5);

    SetSizer(sz);
}

void DirPickerWidgetsPage::CreatePicker()
{
    delete m_dirPicker;

    long style = GetAttrs().m_defaultFlags;

    if ( m_chkDirTextCtrl->GetValue() )
        style |= wxDIRP_USE_TEXTCTRL;

    if ( m_chkDirMustExist->GetValue() )
        style |= wxDIRP_DIR_MUST_EXIST;

    if ( m_chkDirChangeDir->GetValue() )
        style |= wxDIRP_CHANGE_DIR;

    if ( m_chkSmall->GetValue() )
        style |= wxDIRP_SMALL;

    m_dirPicker = new wxDirPickerCtrl(this, PickerPage_Dir,
                                      wxGetHomeDir(), "Hello!",
                                      wxDefaultPosition, wxDefaultSize,
                                      style);
}

void DirPickerWidgetsPage::RecreatePicker()
{
    m_sizer->Remove(1);
    CreatePicker();
    m_sizer->Insert(1, m_dirPicker, 0, wxEXPAND|wxALL, 5);

    m_sizer->Layout();
}

void DirPickerWidgetsPage::Reset()
{
    m_chkDirTextCtrl->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_USE_TEXTCTRL) != 0);
    m_chkDirMustExist->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_DIR_MUST_EXIST) != 0);
    m_chkDirChangeDir->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_CHANGE_DIR) != 0);
    m_chkSmall->SetValue((wxFLP_DEFAULT_STYLE & wxDIRP_SMALL) != 0);
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void DirPickerWidgetsPage::OnButtonSetDir(wxCommandEvent& WXUNUSED(event))
{
    m_dirPicker->SetInitialDirectory(m_textInitialDir->GetValue());
}

void DirPickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();
    RecreatePicker();
}

void DirPickerWidgetsPage::OnDirChange(wxFileDirPickerEvent& event)
{
    wxLogMessage("The directory changed to '%s' ! The current working directory is '%s'",
                 event.GetPath(), wxGetCwd());
}

void DirPickerWidgetsPage::OnCheckBox(wxCommandEvent &event)
{
    if (event.GetEventObject() == m_chkDirTextCtrl ||
        event.GetEventObject() == m_chkDirChangeDir ||
        event.GetEventObject() == m_chkDirMustExist ||
        event.GetEventObject() == m_chkSmall)
        RecreatePicker();
}

#endif  //  wxUSE_DIRPICKERCTRL
