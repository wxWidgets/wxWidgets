/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        filectrl.cpp
// Purpose:     Part of the widgets sample showing wxFileCtrl
// Author:      Diaa M. Sami
// Created:     28 Jul 2007
// Copyright:   (c) 2007 Diaa M. Sami
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

#if wxUSE_FILECTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filectrl.h"

#include "wx/wupdlock.h"
#include "wx/filename.h"

#include "widgets.h"

// TODO change this
#include "icons/dirctrl.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    FileCtrlPage_Reset = wxID_HIGHEST,
    FileCtrlPage_SetDirectory,
    FileCtrlPage_SetPath,
    FileCtrlPage_SetFilename,
    FileCtrlPage_Ctrl
};

enum
{
    FileCtrlMode_Open = 0,
    FileCtrlMode_Save
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class FileCtrlWidgetsPage : public WidgetsPage
{
public:
    FileCtrlWidgetsPage( WidgetsBookCtrl *book, wxImageList *imaglist );
    virtual ~FileCtrlWidgetsPage() {}

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_fileCtrl; }
    virtual void RecreateWidget() wxOVERRIDE { CreateFileCtrl(); }

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // event handlers
    void OnButtonSetDirectory( wxCommandEvent& event );
    void OnButtonSetPath( wxCommandEvent& event );
    void OnButtonSetFilename( wxCommandEvent& event );
    void OnButtonReset( wxCommandEvent& event );
    void OnCheckBox( wxCommandEvent& event );
    void OnRadioBox( wxCommandEvent& event );
    void OnFileCtrl( wxFileCtrlEvent& event );

    // reset the control parameters
    void Reset();

    // (re)create the m_fileCtrl
    void CreateFileCtrl();

    // the controls
    // ------------

    // the control itself and the sizer it is in
    wxFileCtrl *m_fileCtrl;

    // the text entries for command parameters
    wxTextCtrl *m_dir;
    wxTextCtrl *m_path;
    wxTextCtrl *m_filename;

    // flags
    wxCheckBox *m_chkMultiple,
               *m_chkNoShowHidden;

    wxRadioBox *m_radioFileCtrlMode;

    // filters
    wxCheckBox *m_fltr[3];

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE( FileCtrlWidgetsPage )
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( FileCtrlWidgetsPage, WidgetsPage )
    EVT_BUTTON( FileCtrlPage_Reset, FileCtrlWidgetsPage::OnButtonReset )
    EVT_BUTTON( FileCtrlPage_SetDirectory, FileCtrlWidgetsPage::OnButtonSetDirectory )
    EVT_BUTTON( FileCtrlPage_SetPath, FileCtrlWidgetsPage::OnButtonSetPath )
    EVT_BUTTON( FileCtrlPage_SetFilename, FileCtrlWidgetsPage::OnButtonSetFilename )
    EVT_CHECKBOX( wxID_ANY, FileCtrlWidgetsPage::OnCheckBox )
    EVT_RADIOBOX( wxID_ANY, FileCtrlWidgetsPage::OnRadioBox )

    EVT_FILECTRL_FILTERCHANGED( wxID_ANY, FileCtrlWidgetsPage::OnFileCtrl )
    EVT_FILECTRL_FOLDERCHANGED( wxID_ANY, FileCtrlWidgetsPage::OnFileCtrl )
    EVT_FILECTRL_SELECTIONCHANGED( wxID_ANY, FileCtrlWidgetsPage::OnFileCtrl )
    EVT_FILECTRL_FILEACTIVATED( wxID_ANY, FileCtrlWidgetsPage::OnFileCtrl )
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE( FileCtrlWidgetsPage, wxT( "FileCtrl" ),
                        FAMILY_CTRLS );

FileCtrlWidgetsPage::FileCtrlWidgetsPage( WidgetsBookCtrl *book,
        wxImageList *imaglist )
        : WidgetsPage( book, imaglist, dirctrl_xpm )
{
}

void FileCtrlWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer( wxHORIZONTAL );

    // left pane
    wxSizer *sizerLeft = new wxBoxSizer( wxVERTICAL );

    static const wxString mode[] = { wxT( "open" ), wxT( "save" ) };
    m_radioFileCtrlMode = new wxRadioBox( this, wxID_ANY, wxT( "wxFileCtrl mode" ),
                                          wxDefaultPosition, wxDefaultSize,
                                          WXSIZEOF( mode ), mode );

    sizerLeft->Add( m_radioFileCtrlMode,
                    0, wxALL | wxEXPAND , 5 );

    sizerLeft->Add( CreateSizerWithTextAndButton( FileCtrlPage_SetDirectory , wxT( "Set &directory" ), wxID_ANY, &m_dir ),
                    0, wxALL | wxEXPAND , 5 );
    sizerLeft->Add( CreateSizerWithTextAndButton( FileCtrlPage_SetPath , wxT( "Set &path" ), wxID_ANY, &m_path ),
                    0, wxALL | wxEXPAND , 5 );
    sizerLeft->Add( CreateSizerWithTextAndButton( FileCtrlPage_SetFilename , wxT( "Set &filename" ), wxID_ANY, &m_filename ),
                    0, wxALL | wxEXPAND , 5 );

    wxSizer *sizerUseFlags =
        new wxStaticBoxSizer( wxVERTICAL, this, wxT( "&Flags" ) );

    m_chkMultiple   = CreateCheckBoxAndAddToSizer( sizerUseFlags, wxT( "wxFC_MULTIPLE" ) );
    m_chkNoShowHidden   = CreateCheckBoxAndAddToSizer( sizerUseFlags, wxT( "wxFC_NOSHOWHIDDEN" ) );
    sizerLeft->Add( sizerUseFlags, wxSizerFlags().Expand().Border() );

    wxSizer *sizerFilters =
        new wxStaticBoxSizer( wxVERTICAL, this, wxT( "&Filters" ) );
    m_fltr[0] = CreateCheckBoxAndAddToSizer( sizerFilters, wxString::Format( wxT( "all files (%s)|%s" ),
                wxFileSelectorDefaultWildcardStr, wxFileSelectorDefaultWildcardStr ) );
    m_fltr[1] = CreateCheckBoxAndAddToSizer( sizerFilters, wxT( "C++ files (*.cpp; *.h)|*.cpp;*.h" ) );
    m_fltr[2] = CreateCheckBoxAndAddToSizer( sizerFilters, wxT( "PNG images (*.png)|*.png" ) );
    sizerLeft->Add( sizerFilters, wxSizerFlags().Expand().Border() );

    wxButton *btn = new wxButton( this, FileCtrlPage_Reset, wxT( "&Reset" ) );
    sizerLeft->Add( btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15 );

    // right pane
    m_fileCtrl = new wxFileCtrl(
                     this,
                     FileCtrlPage_Ctrl,
                     wxEmptyString,
                     wxEmptyString,
                     wxEmptyString,
                     wxFC_OPEN,
                     wxDefaultPosition,
                     wxDefaultSize
                 );

    // the 3 panes panes compose the window
    sizerTop->Add( sizerLeft, 0, ( wxALL & ~wxLEFT ), 10 );
    sizerTop->Add( m_fileCtrl, 1, wxGROW | ( wxALL & ~wxRIGHT ), 10 );

    // final initializations
    Reset();

    SetSizer( sizerTop );
}

void FileCtrlWidgetsPage::Reset()
{
    m_dir->SetValue( m_fileCtrl->GetDirectory() );
    m_radioFileCtrlMode->SetSelection( ( wxFC_DEFAULT_STYLE & wxFC_OPEN ) ?
                                       FileCtrlMode_Open : FileCtrlMode_Save );
}

void FileCtrlWidgetsPage::CreateFileCtrl()
{
    wxWindowUpdateLocker noUpdates( this );

    long style = GetAttrs().m_defaultFlags;
    style |= m_radioFileCtrlMode->GetSelection() == FileCtrlMode_Open
                ? wxFC_OPEN
                : wxFC_SAVE;
    if ( m_chkMultiple->IsChecked() )
        style |= wxFC_MULTIPLE;
    if ( m_chkNoShowHidden->IsChecked() )
        style |= wxFC_NOSHOWHIDDEN;

    wxFileCtrl *fileCtrl = new wxFileCtrl(
                               this,
                               FileCtrlPage_Ctrl,
                               wxEmptyString,
                               wxEmptyString,
                               wxEmptyString,
                               style,
                               wxDefaultPosition,
                               wxDefaultSize
                           );

    wxString wildcard;
    for ( unsigned int i = 0; i < WXSIZEOF( m_fltr ); ++i )
    {
        if ( m_fltr[i]->IsChecked() )
        {
            if ( !wildcard.IsEmpty() )
                wildcard += wxT( "|" );
            wildcard += m_fltr[i]->GetLabel();
        }
    }
    fileCtrl->SetWildcard( wildcard );

    // update sizer's child window
    GetSizer()->Replace( m_fileCtrl, fileCtrl, true );

    // update our pointer
    delete m_fileCtrl;
    m_fileCtrl = fileCtrl;

    // relayout the sizer
    GetSizer()->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void FileCtrlWidgetsPage::OnButtonSetDirectory( wxCommandEvent& WXUNUSED( event ) )
{
    m_fileCtrl->SetDirectory( m_dir->GetValue() );
}

void FileCtrlWidgetsPage::OnButtonSetPath( wxCommandEvent& WXUNUSED( event ) )
{
    m_fileCtrl->SetPath( m_path->GetValue() );
}

void FileCtrlWidgetsPage::OnButtonSetFilename( wxCommandEvent& WXUNUSED( event ) )
{
    m_fileCtrl->SetFilename( m_filename->GetValue() );
}

void FileCtrlWidgetsPage::OnButtonReset( wxCommandEvent& WXUNUSED( event ) )
{
    Reset();

    CreateFileCtrl();
}

void FileCtrlWidgetsPage::OnCheckBox( wxCommandEvent& WXUNUSED( event ) )
{
    CreateFileCtrl();
}

void FileCtrlWidgetsPage::OnRadioBox( wxCommandEvent& WXUNUSED( event ) )
{
    CreateFileCtrl();
}

void FileCtrlWidgetsPage::OnFileCtrl( wxFileCtrlEvent& event )
{
    if ( event.GetEventType() == wxEVT_FILECTRL_FOLDERCHANGED )
    {
        wxLogMessage("Folder changed event, new folder: %s", event.GetDirectory());
    }
    else if ( event.GetEventType() == wxEVT_FILECTRL_FILEACTIVATED )
    {
        wxLogMessage("File activated event: %s", wxJoin(event.GetFiles(), ' '));
    }
    else if ( event.GetEventType() == wxEVT_FILECTRL_SELECTIONCHANGED )
    {
        wxLogMessage("Selection changed event: %s", wxJoin(event.GetFiles(), ' '));
    }
    else if ( event.GetEventType() == wxEVT_FILECTRL_FILTERCHANGED )
    {
        wxLogMessage("Filter changed event: filter %d selected",
                     event.GetFilterIndex() + 1);
    }
}

#endif // wxUSE_FILECTRL
