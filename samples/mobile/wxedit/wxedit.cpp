/////////////////////////////////////////////////////////////////////////////
// Name:        wxedit.cpp
// Author:      Robert Roebling
// Created:     04/07/02
// Copyright:
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/filename.h"
#include "wx/config.h"

// Include private headers
#include "wxedit.h"

//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------

#define HISTORY_ENTRIES 3

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(ID_ABOUT, MyFrame::OnAbout)

    EVT_MENU(ID_NEW, MyFrame::OnNew)
    EVT_MENU(ID_OPEN, MyFrame::OnOpen)
    EVT_MENU(ID_SAVE, MyFrame::OnSave)
    EVT_MENU(ID_SAVEAS, MyFrame::OnSaveAs)
    EVT_MENU(ID_QUIT, MyFrame::OnQuit)

    EVT_MENU(ID_COPY, MyFrame::OnCopy)
    EVT_MENU(ID_CUT, MyFrame::OnCut)
    EVT_MENU(ID_PASTE, MyFrame::OnPaste)
    EVT_MENU(ID_DELETE, MyFrame::OnDelete)

    EVT_MENU_RANGE(ID_LAST_1, ID_LAST_3, MyFrame::OnLastFiles)

    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_UPDATE_UI(wxID_ANY,MyFrame::OnUpdateUI)
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // Create menu and status bar.
    CreateMyMenuBar();
#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText( _T("Welcome to wxEdit!") );
#endif // wxUSE_STATUSBAR

    // Create edit control. Since it is the only
    // control in the frame, it will be resized
    // to file it out.
    m_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );

    // Read .ini file for file history etc.
    wxConfig *conf = (wxConfig*) wxConfig::Get();

    int entries = 0;
    conf->Read( _T("/History/Count"), &entries );

    for (int i = 0; i < entries; i++)
    {
        wxString tmp;
        tmp.Printf( _T("/History/File%d"), (int)i );

        wxString res;
        conf->Read( tmp, &res );

        if (!res.empty())
            AddToHistory( res );
    }
}

void MyFrame::MakeHistory()
{
    wxMenuBar *mb = GetMenuBar();

    wxASSERT( mb );

    int max = m_history.GetCount();
    if (max > HISTORY_ENTRIES)
        max = HISTORY_ENTRIES;

    for (int i = 0; i < max; i++)
    {
        wxMenu *menu = NULL;
        mb->FindItem( ID_LAST_1 + i, &menu );
        wxASSERT( menu );

        wxFileName fname( m_history[(size_t)i] );
        menu->SetLabel( ID_LAST_1 + i, fname.GetFullName() );
    }
}

void MyFrame::AddToHistory( const wxString &fname )
{
    // Fill menu with history index
    int index = m_history.Index( fname );

    if (index != wxNOT_FOUND)
        m_history.RemoveAt( (size_t) index );

    m_history.Insert( fname, 0 );

    // Update menu
    MakeHistory();
}

void MyFrame::CreateMyMenuBar()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append( ID_ABOUT, _T("About..."), _T("Program info") );
    file_menu->AppendSeparator();
    file_menu->Append( ID_NEW, _T("New..."), _T("New text") );
    file_menu->Append( ID_OPEN, _T("Open..."), _T("Open text") );
    file_menu->Append( ID_SAVE, _T("Save"), _T("Save text") );
    file_menu->Append( ID_SAVEAS, _T("Save as..."), _T("Save text as...") );
    file_menu->AppendSeparator();
    file_menu->Append( ID_QUIT, _T("Quit..."), _T("Quit program") );

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append( ID_COPY, _T("Copy") );
    edit_menu->Append( ID_CUT, _T("Cut") );
    edit_menu->Append( ID_PASTE, _T("Paste") );
    edit_menu->AppendSeparator();
    edit_menu->Append( ID_DELETE, _T("Delete") );

    wxMenu *history_menu = new wxMenu;
    history_menu->Append( ID_LAST_1, _T("No file.") );
    history_menu->Append( ID_LAST_2, _T("No file.") );
    history_menu->Append( ID_LAST_3, _T("No file.") );

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, _T("&File") );
    menu_bar->Append( edit_menu, _T("&Edit") );
    menu_bar->Append( history_menu, _T("&History") );

    SetMenuBar( menu_bar );
}

void MyFrame::OnCopy( wxCommandEvent& WXUNUSED(event) )
{
}

void MyFrame::OnCut( wxCommandEvent& WXUNUSED(event) )
{
}

void MyFrame::OnPaste( wxCommandEvent& WXUNUSED(event) )
{
}

void MyFrame::OnDelete( wxCommandEvent& WXUNUSED(event) )
{
}

void MyFrame::OnLastFiles( wxCommandEvent &event )
{
    if (!Discard()) return;

    if (!m_filename.empty())
        AddToHistory( m_filename );

    size_t index = event.GetId() - ID_LAST_1;

    if( index < m_history.GetCount() )
    {
        m_filename = m_history[index];

        m_text->Clear();
        m_text->LoadFile( m_filename );

#if wxUSE_STATUSBAR
        SetStatusText( m_filename );
#endif // wxUSE_STATUSBAR
    }
    else
    {
        wxMessageBox(
            _T("This entry is empty. It should be filled once you will start opening."),
            _T("Empty entry"),
            wxOK | wxICON_INFORMATION,
            this
        );
    }
}

void MyFrame::OnNew( wxCommandEvent& WXUNUSED(event) )
{
    if (!Discard()) return;

    m_text->Clear();

    if (!m_filename.empty())
        AddToHistory( m_filename );

    m_filename = wxEmptyString;

#if wxUSE_STATUSBAR
    SetStatusText( wxEmptyString );
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnOpen( wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    if (!Discard()) return;

    wxFileDialog dialog( this, _T("Open text"), wxEmptyString, wxEmptyString,
        _T("Text file (*.txt)|*.txt|Any file (*)|*"),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST );
    if (dialog.ShowModal() == wxID_OK)
    {
        m_text->Clear();

#ifdef __WXX11__
        // requires wxUSE_UNIV_TEXTCTRL to be set to 0
#if 0
        wxFileName fname( dialog.GetPath() );
        if ((fname.GetExt() == _T("cpp")) ||
            (fname.GetExt() == _T("c")) ||
            (fname.GetExt() == _T("h")) ||
            (fname.GetExt() == _T("cxx")) ||
            (fname.GetExt() == _T("hxx")))
        {
            m_text->SetLanguage( wxSOURCE_LANG_CPP );
        }
        else
        if (fname.GetExt() == _T("py"))
        {
            m_text->SetLanguage( wxSOURCE_LANG_PYTHON );
        }
        else
        if ((fname.GetExt() == _T("pl")) ||
            (fname.GetExt() == _T("pm")))
        {
            m_text->SetLanguage( wxSOURCE_LANG_PYTHON );
        }
        else
        {
            m_text->SetLanguage( wxSOURCE_LANG_NONE );
        }
#endif
#endif

        m_filename = dialog.GetPath();
        m_text->LoadFile( m_filename );

#if wxUSE_STATUSBAR
        SetStatusText( m_filename );
#endif // wxUSE_STATUSBAR
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnSave( wxCommandEvent& WXUNUSED(event) )
{
    Save();
}

void MyFrame::OnSaveAs( wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    wxFileDialog dialog( this, _T("Open text"), wxEmptyString, wxEmptyString,
        _T("Text file (*.txt)|*.txt|Any file (*)|*"),
        wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    if (dialog.ShowModal() == wxID_OK)
    {
        m_filename = dialog.GetPath();
        m_text->SaveFile( m_filename );

#if wxUSE_STATUSBAR
        SetStatusText( m_filename );
#endif // wxUSE_STATUSBAR
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog( this, _T("Welcome to wxEdit\n(C)opyright Robert Roebling"),
        _T("About wxEdit"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
     Close( true );
}

bool MyFrame::Save()
{
    wxCommandEvent event;

    if (m_filename.empty())
        OnSaveAs( event );
    else
        m_text->SaveFile( m_filename );

   return true;
}

bool MyFrame::Discard()
{
    if (m_text->IsModified())
    {
        wxMessageDialog dialog( this, _T("Text has been\nmodified! Save?"),
            _T("wxEdit"), wxYES_NO|wxCANCEL|wxICON_EXCLAMATION );

        int ret = dialog.ShowModal();

        if (ret == wxID_CANCEL)
            return false;

        if (ret == wxID_YES)
        {
            if (!Save())
                return false;
        }
    }

    return true;
}

void MyFrame::OnUpdateUI( wxUpdateUIEvent &event )
{
    switch (event.GetId())
    {
        case ID_COPY:
            event.Enable( false );
            break;
        case ID_CUT:
            event.Enable( false );
            break;
        case ID_PASTE:
            event.Enable( false );
            break;
        case ID_DELETE:
#ifdef __WXUNIVERSAL__
            event.Enable( m_text->HasSelection() );
#else
            {
              long selFrom, selTo;
              m_text->GetSelection(& selFrom, & selTo);
              event.Enable( selFrom != selTo );
            }
#endif
            break;
        default:
            break;
    }
}

void MyFrame::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
    // Save changes?
    if (!Discard()) return;

    // Add current to history
    if (!m_filename.empty())
        AddToHistory( m_filename );

    // Write .ini file
    wxConfig *conf = (wxConfig*) wxConfig::Get();

    int max = HISTORY_ENTRIES;
    if (m_history.GetCount() < (size_t)max)
        max = m_history.GetCount();

    conf->Write( _T("/History/Count"), max );

    for (int i = 0; i < max; i++)
    {
        wxString tmp;
        tmp.Printf( _T("/History/File%d"), (int)i );

        conf->Write( tmp, m_history[(size_t)i] );
    }

    // Flush and delete config
    delete wxConfig::Set( NULL );

    // Finally destroy window and quit
    Destroy();
}

//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    SetVendorName(_T("Free world"));
    SetAppName(_T("wxEdit"));

    MyFrame *frame = new MyFrame( NULL, wxID_ANY, _T("wxEdit"), wxPoint(20,20), wxSize(500,340) );
    frame->Show( true );

    return true;
}

int MyApp::OnExit()
{
    return 0;
}
