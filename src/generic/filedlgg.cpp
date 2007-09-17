//////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/filedlgg.cpp
// Purpose:     wxGenericFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILEDLG && (defined(__WXUNIVERSAL__) || defined(__WXGTK__))

// NOTE : it probably also supports MAC, untested
#if !defined(__UNIX__) && !defined(__DOS__) && !defined(__WIN32__) && !defined(__OS2__)
#error wxGenericFileDialog currently only supports Unix, win32 and DOS
#endif

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include "wx/msw/wrapwin.h"
    #endif
    #include "wx/hash.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/bmpbuttn.h"
    #include "wx/checkbox.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/sizer.h"
    #include "wx/filedlg.h"     // wxFD_OPEN, wxFD_SAVE...
#endif

#include "wx/longlong.h"
#include "wx/config.h"
#include "wx/imaglist.h"
#include "wx/artprov.h"
#include "wx/filefn.h"
#include "wx/filectrl.h"
#include "wx/generic/filedlgg.h"
#include "wx/debug.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#ifndef __WXWINCE__
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#ifdef __UNIX__
    #include <dirent.h>
    #include <pwd.h>
    #ifndef __VMS
    # include <grp.h>
    #endif
#endif

#ifdef __WINDOWS__
    #include "wx/msw/mslu.h"
#endif

#ifdef __WATCOMC__
    #include <direct.h>
#endif

#ifndef __WXWINCE__
#include <time.h>
#endif

#if defined(__UNIX__) || defined(__DOS__)
#include <unistd.h>
#endif

#if defined(__WXWINCE__)
#define IsTopMostDir(dir) (dir == wxT("\\") || dir == wxT("/"))
#elif (defined(__DOS__) || defined(__WINDOWS__) || defined (__OS2__))
#define IsTopMostDir(dir)   (dir.empty())
#else
#define IsTopMostDir(dir)   (dir == wxT("/"))
#endif

//-----------------------------------------------------------------------------
// wxGenericFileDialog
//-----------------------------------------------------------------------------

#define  ID_LIST_MODE     (wxID_FILEDLGG    )
#define  ID_REPORT_MODE   (wxID_FILEDLGG + 1)
#define  ID_UP_DIR        (wxID_FILEDLGG + 2)
#define  ID_PARENT_DIR    (wxID_FILEDLGG + 3)
#define  ID_NEW_DIR       (wxID_FILEDLGG + 4)
#define  ID_FILE_CTRL     (wxID_FILEDLGG + 5)

IMPLEMENT_DYNAMIC_CLASS(wxGenericFileDialog, wxFileDialogBase)

BEGIN_EVENT_TABLE(wxGenericFileDialog,wxDialog)
    EVT_BUTTON(ID_LIST_MODE, wxGenericFileDialog::OnList)
    EVT_BUTTON(ID_REPORT_MODE, wxGenericFileDialog::OnReport)
    EVT_BUTTON(ID_UP_DIR, wxGenericFileDialog::OnUp)
    EVT_BUTTON(ID_PARENT_DIR, wxGenericFileDialog::OnHome)
    EVT_BUTTON(ID_NEW_DIR, wxGenericFileDialog::OnNew)
    EVT_BUTTON(wxID_OK, wxGenericFileDialog::OnOk)
    EVT_FILECTRL_FILEACTIVATED(ID_FILE_CTRL, wxGenericFileDialog::OnFileActivated)
END_EVENT_TABLE()

long wxGenericFileDialog::ms_lastViewStyle = wxLC_LIST;
bool wxGenericFileDialog::ms_lastShowHidden = false;

void wxGenericFileDialog::Init()
{
    m_bypassGenericImpl = false;

    m_filectrl   = NULL;
    m_upDirButton  = NULL;
    m_newDirButton = NULL;
}

wxGenericFileDialog::wxGenericFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long  style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name,
                           bool  bypassGenericImpl ) : wxFileDialogBase()
{
    Init();
    Create( parent, message, defaultDir, defaultFile, wildCard, style, pos, sz, name, bypassGenericImpl );
}

bool wxGenericFileDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& defaultDir,
                                  const wxString& defaultFile,
                                  const wxString& wildCard,
                                  long  style,
                                  const wxPoint& pos,
                                  const wxSize& sz,
                                  const wxString& name,
                                  bool  bypassGenericImpl )
{
    m_bypassGenericImpl = bypassGenericImpl;

    parent = GetParentForModalDialog(parent);

    if (!wxFileDialogBase::Create(parent, message, defaultDir, defaultFile,
                                  wildCard, style, pos, sz, name))
    {
        return false;
    }

    if (m_bypassGenericImpl)
        return true;

    if (!wxDialog::Create( parent, wxID_ANY, message, pos, sz,
                           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | style, name
                           ))
    {
        return false;
    }

#if wxUSE_CONFIG
    if (wxConfig::Get(false))
    {
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                              &ms_lastViewStyle);
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                              &ms_lastShowHidden);
    }
#endif

    if ((m_dir.empty()) || (m_dir == wxT(".")))
    {
        m_dir = wxGetCwd();
        if (m_dir.empty())
            m_dir = wxFILE_SEP_PATH;
    }

    const size_t len = m_dir.length();
    if ((len > 1) && (wxEndsWithPathSeparator(m_dir)))
        m_dir.Remove( len-1, 1 );

    m_path = m_dir;
    m_path += wxFILE_SEP_PATH;
    m_path += defaultFile;
    m_filterExtension = wxEmptyString;

    // layout

    const bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );

    wxBitmapButton *but;

    but = new wxBitmapButton(this, ID_LIST_MODE,
                             wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a list view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    but = new wxBitmapButton(this, ID_REPORT_MODE,
                             wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a detailed view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    buttonsizer->Add( 30, 5, 1 );

    m_upDirButton = new wxBitmapButton(this, ID_UP_DIR,
                           wxArtProvider::GetBitmap(wxART_GO_DIR_UP, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    m_upDirButton->SetToolTip( _("Go to parent directory") );
#endif
    buttonsizer->Add( m_upDirButton, 0, wxALL, 5 );

#ifndef __DOS__ // VS: Home directory is meaningless in MS-DOS...
    but = new wxBitmapButton(this, ID_PARENT_DIR,
                             wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("Go to home directory") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5);

    buttonsizer->Add( 20, 20 );
#endif //!__DOS__

    m_newDirButton = new wxBitmapButton(this, ID_NEW_DIR,
                           wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    m_newDirButton->SetToolTip( _("Create new directory") );
#endif
    buttonsizer->Add( m_newDirButton, 0, wxALL, 5 );

    if (is_pda)
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 0 );
    else
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 5 );

    long style2 = 0;
    if ( HasFdFlag(wxFD_MULTIPLE) )
        style2 |= wxFC_MULTIPLE;

    m_filectrl = new wxGenericFileCtrl( this, ID_FILE_CTRL,
                                        m_dir, defaultFile,
                                        wildCard,
                                        style2,
                                        wxDefaultPosition, wxSize(540,200)
                                        );

    m_filectrl->ShowHidden( ms_lastShowHidden );

    if (ms_lastViewStyle == wxLC_LIST)
    {
        m_filectrl->ChangeToListMode();
    }
    else if (ms_lastViewStyle == wxLC_REPORT)
    {
        m_filectrl->ChangeToReportMode();
    }

    if (is_pda)
    {
        // PDAs have a different screen layout
        mainsizer->Add(m_filectrl, wxSizerFlags(1).Expand().HorzBorder());

        wxSizer *bsizer = CreateButtonSizer(wxOK | wxCANCEL);
        if ( bsizer )
            mainsizer->Add(bsizer, wxSizerFlags().Expand().Border());
    }
    else // !is_pda
    {
        mainsizer->Add(m_filectrl, wxSizerFlags(1).Expand().DoubleHorzBorder());

        wxBoxSizer *okcancelsizer = new wxBoxSizer( wxHORIZONTAL );
        okcancelsizer->Add(new wxButton(this, wxID_OK), wxSizerFlags().DoubleBorder().Centre());
        okcancelsizer->Add(new wxButton(this, wxID_CANCEL), wxSizerFlags().DoubleBorder().Centre());
        mainsizer->Add(okcancelsizer, wxSizerFlags().Center());
        }

    SetAutoLayout( true );
    SetSizer( mainsizer );

    if (!is_pda)
    {
        mainsizer->Fit( this );
        mainsizer->SetSizeHints( this );

        Centre( wxBOTH );
    }

    return true;
}

wxGenericFileDialog::~wxGenericFileDialog()
{
    if (!m_bypassGenericImpl)
    {
#if wxUSE_CONFIG
        if (wxConfig::Get(false))
        {
            wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                                   ms_lastViewStyle);
            wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                                   ms_lastShowHidden);
        }
#endif
    }
}

int wxGenericFileDialog::ShowModal()
{
    m_filectrl->SetDirectory(m_dir);
    UpdateControls();

    return wxDialog::ShowModal();
}

bool wxGenericFileDialog::Show( bool show )
{
    // Called by ShowModal, so don't repeate the update
#ifndef __WIN32__
    if (show)
    {
        m_filectrl->SetDirectory(m_dir);
        UpdateControls();
    }
#endif

    return wxDialog::Show( show );
}

void wxGenericFileDialog::SetWildcard(const wxString& wildCard)
{
    m_filectrl->SetWildcard(wildCard);
}

void wxGenericFileDialog::SetFilterIndex( int filterindex )
{
    m_filectrl->SetFilterIndex(filterindex);
}

void wxGenericFileDialog::OnOk( wxCommandEvent &WXUNUSED(event) )
{
    wxArrayString selectedFiles;
    m_filectrl->GetFilenames(selectedFiles);

    if (selectedFiles.Count() == 0)
        return;

    if (selectedFiles.Count() == 1)
    {
        SetPath( selectedFiles[0] );
    }

    EndModal(wxID_OK);
}

void wxGenericFileDialog::OnList( wxCommandEvent &WXUNUSED(event) )
{
    m_filectrl->ChangeToListMode();
    ms_lastViewStyle = wxLC_LIST;
    m_filectrl->GetFileList()->SetFocus();
}

void wxGenericFileDialog::OnReport( wxCommandEvent &WXUNUSED(event) )
{
    m_filectrl->ChangeToReportMode();
    ms_lastViewStyle = wxLC_REPORT;
    m_filectrl->GetFileList()->SetFocus();
}

void wxGenericFileDialog::OnUp( wxCommandEvent &WXUNUSED(event) )
{
    m_filectrl->GoToParentDir();
    m_filectrl->GetFileList()->SetFocus();
    UpdateControls();
}

void wxGenericFileDialog::OnHome( wxCommandEvent &WXUNUSED(event) )
{
    m_filectrl->GoToHomeDir();
    m_filectrl->SetFocus();
    UpdateControls();
}

void wxGenericFileDialog::OnNew( wxCommandEvent &WXUNUSED(event) )
{
    m_filectrl->GetFileList()->MakeDir();
}

void wxGenericFileDialog::OnFileActivated( wxFileCtrlEvent &WXUNUSED(event) )
{
    wxCommandEvent dummy;
    OnOk( dummy );
}

void wxGenericFileDialog::SetPath( const wxString& path )
{
    // not only set the full path but also update filename and dir
    m_path = path;

    m_filectrl->SetPath(path);
}

void wxGenericFileDialog::GetPaths( wxArrayString& paths ) const
{
    m_filectrl->GetPaths(paths);
}

void wxGenericFileDialog::GetFilenames(wxArrayString& files) const
{
    m_filectrl->GetFilenames(files);
}

void wxGenericFileDialog::UpdateControls()
{
    const bool enable = !IsTopMostDir(m_filectrl->GetDirectory());
    m_upDirButton->Enable(enable);

#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
    m_newDirButton->Enable(enable);
#endif // defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
}

#ifdef wxHAS_GENERIC_FILEDIALOG

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxGenericFileDialog)

#endif // wxHAS_GENERIC_FILEDIALOG

#endif // wxUSE_FILEDLG
