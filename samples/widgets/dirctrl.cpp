/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        dirctrl.cpp
// Purpose:     Part of the widgets sample showing wxGenericDirCtrl
// Author:      Wlodzimierz 'ABX' Skiba
// Created:     4 Oct 2006
// Id:          $Id$
// Copyright:   (c) 2006 wxWindows team
// License:     wxWindows license
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

#if wxUSE_DIRDLG

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/sizer.h"
    #include "wx/statbox.h"
    #include "wx/radiobox.h"
    #include "wx/checkbox.h"
    #include "wx/button.h"
#endif

#include "wx/generic/dirctrlg.h"

#include "wx/wupdlock.h"
#include "wx/stdpaths.h"

#include "widgets.h"

#include "icons/dirctrl.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    DirCtrlPage_Reset = wxID_HIGHEST,
    DirCtrlPage_SetPath,
    DirCtrlPage_Ctrl
};

static const wxString stdPaths[] =
{
    _T("&none"),
    _T("&config"),
    _T("&data"),
    _T("&documents"),
    _T("&local data"),
    _T("&plugins"),
    _T("&resources"),
    _T("&user config"),
    _T("&user data"),
    _T("&user local data")
};

enum
{
    stdPathUnknown = 0,
    stdPathConfig,
    stdPathData,
    stdPathDocuments,
    stdPathLocalData,
    stdPathPlugins,
    stdPathResources,
    stdPathUserConfig,
    stdPathUserData,
    stdPathUserLocalData,
    stdPathMax
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class DirCtrlWidgetsPage : public WidgetsPage
{
public:
    DirCtrlWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~DirCtrlWidgetsPage() {}

    virtual wxControl *GetWidget() const { return m_dirCtrl; }
    virtual void RecreateWidget() { CreateDirCtrl(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonSetPath(wxCommandEvent& event);
    void OnButtonReset(wxCommandEvent& event);
    void OnStdPath(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
    void OnRadioBox(wxCommandEvent& event);

    // reset the control parameters
    void Reset();

    // (re)create the m_dirCtrl
    void CreateDirCtrl();

    // the controls
    // ------------

    // the control itself and the sizer it is in
    wxGenericDirCtrl *m_dirCtrl;

    // the text entries for command parameters
    wxTextCtrl *m_path;

    wxRadioBox *m_radioStdPath;

    // flags
    wxCheckBox *m_chkDirOnly,
               *m_chk3D,
               *m_chkFirst,
               *m_chkFilters,
               *m_chkLabels;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(DirCtrlWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DirCtrlWidgetsPage, WidgetsPage)
    EVT_BUTTON(DirCtrlPage_Reset, DirCtrlWidgetsPage::OnButtonReset)
    EVT_BUTTON(DirCtrlPage_SetPath, DirCtrlWidgetsPage::OnButtonSetPath)
    EVT_CHECKBOX(wxID_ANY, DirCtrlWidgetsPage::OnCheckBox)
    EVT_RADIOBOX(wxID_ANY, DirCtrlWidgetsPage::OnRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(DirCtrlWidgetsPage, wxT("DirCtrl"),
                       GENERIC_CTRLS
                       );

DirCtrlWidgetsPage::DirCtrlWidgetsPage(WidgetsBookCtrl *book,
                                       wxImageList *imaglist)
                   :WidgetsPage(book, imaglist, dirctrl_xpm)
{
}

void DirCtrlWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Dir control details"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add( CreateSizerWithTextAndButton( DirCtrlPage_SetPath , wxT("Set &path"), wxID_ANY, &m_path ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    wxSizer *sizerUseFlags =
        new wxStaticBoxSizer(wxVERTICAL, this, _T("&Flags"));
    m_chkDirOnly = CreateCheckBoxAndAddToSizer(sizerUseFlags, _T("wxDIRCTRL_DIR_ONLY"));
    m_chk3D      = CreateCheckBoxAndAddToSizer(sizerUseFlags, _T("wxDIRCTRL_3D_INTERNAL"));
    m_chkFirst   = CreateCheckBoxAndAddToSizer(sizerUseFlags, _T("wxDIRCTRL_SELECT_FIRST"));
    m_chkFilters = CreateCheckBoxAndAddToSizer(sizerUseFlags, _T("wxDIRCTRL_SHOW_FILTERS"));
    m_chkLabels  = CreateCheckBoxAndAddToSizer(sizerUseFlags, _T("wxDIRCTRL_EDIT_LABELS"));
    sizerLeft->Add(sizerUseFlags, wxSizerFlags().Expand().Border());

    wxButton *btn = new wxButton(this, DirCtrlPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // keep consistency between enum and labels of radiobox
    wxCOMPILE_TIME_ASSERT( stdPathMax == WXSIZEOF(stdPaths), EnumForRadioBoxMismatch);

    // middle pane
    m_radioStdPath = new wxRadioBox(this, wxID_ANY, _T("Standard path"),
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(stdPaths), stdPaths, 1);

    // right pane
    m_dirCtrl = new wxGenericDirCtrl(
        this,
        DirCtrlPage_Ctrl,
        wxDirDialogDefaultFolderStr,
        wxDefaultPosition,
        wxDefaultSize,
        0
    );

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(m_radioStdPath, 0, wxGROW | wxALL , 10);
    sizerTop->Add(m_dirCtrl, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

void DirCtrlWidgetsPage::Reset()
{
    m_path->SetValue(m_dirCtrl->GetPath());
}

void DirCtrlWidgetsPage::CreateDirCtrl()
{
    wxWindowUpdateLocker noUpdates(this);

    wxGenericDirCtrl *dirCtrl = new wxGenericDirCtrl(
        this,
        DirCtrlPage_Ctrl,
        wxDirDialogDefaultFolderStr,
        wxDefaultPosition,
        wxDefaultSize,
        ( m_chkDirOnly->IsChecked() ? wxDIRCTRL_DIR_ONLY : 0 ) |
        ( m_chk3D->IsChecked() ? wxDIRCTRL_3D_INTERNAL : 0 ) |
        ( m_chkFirst->IsChecked() ? wxDIRCTRL_SELECT_FIRST : 0 ) |
        ( m_chkFilters->IsChecked() ? wxDIRCTRL_SHOW_FILTERS : 0 ) |
        ( m_chkLabels->IsChecked() ? wxDIRCTRL_EDIT_LABELS : 0 )
    );

    // update sizer's child window
    GetSizer()->Replace(m_dirCtrl, dirCtrl, true);

    // update our pointer
    delete m_dirCtrl;
    m_dirCtrl = dirCtrl;

    // relayout the sizer
    GetSizer()->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void DirCtrlWidgetsPage::OnButtonSetPath(wxCommandEvent& WXUNUSED(event))
{
    m_dirCtrl->SetPath(m_path->GetValue());
}

void DirCtrlWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateDirCtrl();
}

void DirCtrlWidgetsPage::OnCheckBox(wxCommandEvent& WXUNUSED(event))
{
    CreateDirCtrl();
}

void DirCtrlWidgetsPage::OnRadioBox(wxCommandEvent& WXUNUSED(event))
{
    wxString path;

    wxTheApp->SetAppName(_T("widgets"));
    wxStandardPathsBase& stdp = wxStandardPaths::Get();

    switch ( m_radioStdPath->GetSelection() )
    {
        default:
        case stdPathUnknown:
        case stdPathMax:
            // leave path
            break;

        case stdPathConfig:
            path = stdp.GetConfigDir();
            break;

        case stdPathData:
            path = stdp.GetDataDir();
            break;

        case stdPathDocuments:
            path = stdp.GetDocumentsDir();
            break;

        case stdPathLocalData:
            path = stdp.GetLocalDataDir();
            break;

        case stdPathPlugins:
            path = stdp.GetPluginsDir();
            break;

        case stdPathResources:
            path = stdp.GetResourcesDir();
            break;

        case stdPathUserConfig:
            path = stdp.GetUserConfigDir();
            break;

        case stdPathUserData:
            path = stdp.GetUserDataDir();
            break;

        case stdPathUserLocalData:
            path = stdp.GetUserLocalDataDir();
            break;
    }

    m_dirCtrl->SetPath(path);
    if(!m_dirCtrl->GetPath().IsSameAs(path))
    {
        wxLogMessage(_T("Selected standard path and path from control do not match!"));
        m_radioStdPath->SetSelection(stdPathUnknown);
    }
}

#endif // wxUSE_DIRDLG
