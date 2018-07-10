/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        filepicker.cpp
// Purpose:     Part of the widgets sample showing wx*PickerCtrl
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILEPICKERCTRL

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

#include "icons/filepicker.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    FilePickerMode_Open = 0,
    FilePickerMode_Save
};

// control ids
enum
{
    PickerPage_Reset = wxID_HIGHEST,
    PickerPage_File,
    PickerPage_SetDir,
    PickerPage_CurrentPath
};


// ----------------------------------------------------------------------------
// FilePickerWidgetsPage
// ----------------------------------------------------------------------------

class FilePickerWidgetsPage : public WidgetsPage
{
public:
    FilePickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_filePicker; }
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

    // update filepicker radiobox
    void UpdateFilePickerMode();

    // the pickers and the relative event handlers
    void OnFileChange(wxFileDirPickerEvent &ev);
    void OnCheckBox(wxCommandEvent &ev);
    void OnButtonReset(wxCommandEvent &ev);
    void OnButtonSetDir(wxCommandEvent &ev);
    void OnUpdatePath(wxUpdateUIEvent &ev);


    // the picker
    wxFilePickerCtrl *m_filePicker;


    // other controls
    // --------------

    wxCheckBox *m_chkFileTextCtrl,
               *m_chkFileOverwritePrompt,
               *m_chkFileMustExist,
               *m_chkFileChangeDir,
               *m_chkSmall;
    wxRadioBox *m_radioFilePickerMode;
    wxStaticText *m_labelPath;
    wxTextCtrl *m_textInitialDir;

    wxBoxSizer *m_sizer;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(FilePickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(FilePickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(PickerPage_Reset, FilePickerWidgetsPage::OnButtonReset)
    EVT_BUTTON(PickerPage_SetDir, FilePickerWidgetsPage::OnButtonSetDir)

    EVT_FILEPICKER_CHANGED(PickerPage_File, FilePickerWidgetsPage::OnFileChange)

    EVT_CHECKBOX(wxID_ANY, FilePickerWidgetsPage::OnCheckBox)
    EVT_RADIOBOX(wxID_ANY, FilePickerWidgetsPage::OnCheckBox)

    EVT_UPDATE_UI(PickerPage_CurrentPath, FilePickerWidgetsPage::OnUpdatePath)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK20__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(FilePickerWidgetsPage, wxT("FilePicker"),
                       PICKER_CTRLS | FAMILY_CTRLS);

FilePickerWidgetsPage::FilePickerWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, filepicker_xpm)
{
}

void FilePickerWidgetsPage::CreateContent()
{
    // left pane
    wxSizer *boxleft = new wxBoxSizer(wxVERTICAL);

    static const wxString mode[] = { wxT("open"), wxT("save") };
    m_radioFilePickerMode = new wxRadioBox(this, wxID_ANY, wxT("wxFilePicker mode"),
                                           wxDefaultPosition, wxDefaultSize,
                                           WXSIZEOF(mode), mode);
    boxleft->Add(m_radioFilePickerMode, 0, wxALL|wxGROW, 5);

    wxStaticBoxSizer *filebox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("&FilePicker style"));
    m_chkFileTextCtrl = CreateCheckBoxAndAddToSizer(filebox, wxT("With textctrl"));
    m_chkFileOverwritePrompt = CreateCheckBoxAndAddToSizer(filebox, wxT("Overwrite prompt"));
    m_chkFileMustExist = CreateCheckBoxAndAddToSizer(filebox, wxT("File must exist"));
    m_chkFileChangeDir = CreateCheckBoxAndAddToSizer(filebox, wxT("Change working dir"));
    m_chkSmall = CreateCheckBoxAndAddToSizer(filebox, "&Small version");

    boxleft->Add(filebox, 0, wxALL|wxGROW, 5);

    boxleft->Add(CreateSizerWithTextAndButton
                 (
                    PickerPage_SetDir,
                    "&Initial directory",
                    wxID_ANY,
                    &m_textInitialDir
                 ), wxSizerFlags().Expand().Border());

    boxleft->AddSpacer(10);

    boxleft->Add(new wxButton(this, PickerPage_Reset, wxT("&Reset")),
                 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    Reset();    // set checkboxes state

    // create the picker and the static text displaying its current value
    m_labelPath = new wxStaticText(this, PickerPage_CurrentPath, "");

    m_filePicker = NULL;
    CreatePicker();

    // right pane
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->AddStretchSpacer();
    m_sizer->Add(m_filePicker, wxSizerFlags().Expand().Border());
    m_sizer->AddStretchSpacer();
    m_sizer->Add(m_labelPath, wxSizerFlags().Expand().Border());
    m_sizer->AddStretchSpacer();

    // global pane
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(boxleft, 0, wxGROW|wxALL, 5);
    sz->Add(m_sizer, 1, wxGROW|wxALL, 5);

    SetSizer(sz);
}

void FilePickerWidgetsPage::CreatePicker()
{
    delete m_filePicker;

    long style = GetAttrs().m_defaultFlags;

    if ( m_chkFileTextCtrl->GetValue() )
        style |= wxFLP_USE_TEXTCTRL;

    if ( m_chkFileOverwritePrompt->GetValue() )
        style |= wxFLP_OVERWRITE_PROMPT;

    if ( m_chkFileMustExist->GetValue() )
        style |= wxFLP_FILE_MUST_EXIST;

    if ( m_chkFileChangeDir->GetValue() )
        style |= wxFLP_CHANGE_DIR;

    if ( m_chkSmall->GetValue() )
        style |= wxFLP_SMALL;

    if (m_radioFilePickerMode->GetSelection() == FilePickerMode_Open)
        style |= wxFLP_OPEN;
    else
        style |= wxFLP_SAVE;

    // pass an empty string as initial file
    m_filePicker = new wxFilePickerCtrl(this, PickerPage_File,
                                        wxEmptyString,
                                        wxT("Hello!"), wxT("*"),
                                        wxDefaultPosition, wxDefaultSize,
                                        style);
}

void FilePickerWidgetsPage::RecreatePicker()
{
    m_sizer->Remove(1);
    CreatePicker();
    m_sizer->Insert(1, m_filePicker, 0, wxEXPAND|wxALL, 5);

    m_sizer->Layout();
}

void FilePickerWidgetsPage::Reset()
{
    m_radioFilePickerMode->SetSelection((wxFLP_DEFAULT_STYLE & wxFLP_OPEN) ?
                                            FilePickerMode_Open : FilePickerMode_Save);
    m_chkFileTextCtrl->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_USE_TEXTCTRL) != 0);
    m_chkFileOverwritePrompt->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_OVERWRITE_PROMPT) != 0);
    m_chkFileMustExist->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_FILE_MUST_EXIST) != 0);
    m_chkFileChangeDir->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_CHANGE_DIR) != 0);
    m_chkSmall->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_SMALL) != 0);

    UpdateFilePickerMode();
}

void FilePickerWidgetsPage::UpdateFilePickerMode()
{
    switch (m_radioFilePickerMode->GetSelection())
    {
    case FilePickerMode_Open:
        m_chkFileOverwritePrompt->SetValue(false);
        m_chkFileOverwritePrompt->Disable();
        m_chkFileMustExist->Enable();
        break;
    case FilePickerMode_Save:
        m_chkFileMustExist->SetValue(false);
        m_chkFileMustExist->Disable();
        m_chkFileOverwritePrompt->Enable();
        break;
    }
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void FilePickerWidgetsPage::OnButtonSetDir(wxCommandEvent& WXUNUSED(event))
{
    const wxString& dir = m_textInitialDir->GetValue();
    m_filePicker->SetInitialDirectory(dir);
    wxLogMessage("Initial directory set to \"%s\"", dir);
}

void FilePickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    RecreatePicker();
}

void FilePickerWidgetsPage::OnFileChange(wxFileDirPickerEvent& event)
{
    wxLogMessage(wxT("The file changed to '%s' ! The current working directory is '%s'"),
                 event.GetPath().c_str(), wxGetCwd().c_str());
}

void FilePickerWidgetsPage::OnCheckBox(wxCommandEvent &event)
{
    if (event.GetEventObject() == m_chkFileTextCtrl ||
        event.GetEventObject() == m_chkFileOverwritePrompt ||
        event.GetEventObject() == m_chkFileMustExist ||
        event.GetEventObject() == m_chkFileChangeDir ||
        event.GetEventObject() == m_chkSmall)
        RecreatePicker();

    if (event.GetEventObject() == m_radioFilePickerMode)
    {
        UpdateFilePickerMode();
        RecreatePicker();
    }
}

void FilePickerWidgetsPage::OnUpdatePath(wxUpdateUIEvent& ev)
{
    ev.SetText( "Current path: " + m_filePicker->GetPath() );
}

#endif  // wxUSE_FILEPICKERCTRL
