/////////////////////////////////////////////////////////////////////////////
// Name:        wxedit.cpp
// Author:      Robert Roebling
// Created:     04/07/02
// Copyright:   
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxedit.cpp"
#endif

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
    EVT_UPDATE_UI(-1,MyFrame::OnUpdateUI)
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // Create menu and status bar.
    CreateMyMenuBar();
    CreateStatusBar(1);
    SetStatusText( "Welcome to wxEdit!" );
    
    // Create edit control. Since it is the only
    // control in the frame, it will be resized
    // to file it out.
    m_text = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    
    // Read .ini file for file history etc.
    wxConfig *conf = (wxConfig*) wxConfig::Get();

    int entries = 0;    
    conf->Read( "/History/Count", &entries );
    
    for (int i = 0; i < entries; i++)
    {
        wxString tmp;
        tmp.Printf( "/History/File%d", (int)i );
        
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
        m_history.Remove( (size_t) index );
    
    m_history.Insert( fname, 0 );
    
    // Update menu
    MakeHistory();
}

void MyFrame::CreateMyMenuBar()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append( ID_ABOUT, "About...", "Program info" );
    file_menu->AppendSeparator();
    file_menu->Append( ID_NEW, "New...", "New text" );
    file_menu->Append( ID_OPEN, "Open...", "Open text" );
    file_menu->Append( ID_SAVE, "Save", "Save text" );
    file_menu->Append( ID_SAVEAS, "Save as...", "Save text as..." );
    file_menu->AppendSeparator();
    file_menu->Append( ID_QUIT, "Quit...", "Quit program" );

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append( ID_COPY, "Copy" );
    edit_menu->Append( ID_CUT, "Cut" );
    edit_menu->Append( ID_PASTE, "Paste" );
    edit_menu->AppendSeparator();
    edit_menu->Append( ID_DELETE, "Delete" );
    
    wxMenu *history_menu = new wxMenu;
    history_menu->Append( ID_LAST_1, "No file." );
    history_menu->Append( ID_LAST_2, "No file." );
    history_menu->Append( ID_LAST_3, "No file." );
    
    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, "&File" );
    menu_bar->Append( edit_menu, "&Edit" );
    menu_bar->Append( history_menu, "&History" );
    
    SetMenuBar( menu_bar );
}

void MyFrame::OnCopy( wxCommandEvent &event )
{
}

void MyFrame::OnCut( wxCommandEvent &event )
{
}

void MyFrame::OnPaste( wxCommandEvent &event )
{
}

void MyFrame::OnDelete( wxCommandEvent &event )
{
}

void MyFrame::OnLastFiles( wxCommandEvent &event )
{
    if (!Discard()) return;

    if (!m_filename.empty())
        AddToHistory( m_filename );
        
    size_t index = event.GetId() - ID_LAST_1;
    
    wxASSERT( index < m_history.GetCount() );
    
    m_filename = m_history[index];
    
    m_text->Clear();
    m_text->LoadFile( m_filename );
    
    SetStatusText( m_filename );
}

void MyFrame::OnNew( wxCommandEvent &event )
{
    if (!Discard()) return;

    m_text->Clear();
    
    if (!m_filename.empty())
        AddToHistory( m_filename );
        
    m_filename = wxEmptyString;
    
    SetStatusText( "" );
}

void MyFrame::OnOpen( wxCommandEvent &event )
{
    if (!Discard()) return;

    wxFileDialog dialog( this, "Open text", "", "",
        "Text file (*.txt)|*.txt|"
        "Any file (*)|*",
        wxOPEN|wxFILE_MUST_EXIST );
    if (dialog.ShowModal() == wxID_OK)
    {
        m_text->Clear();

#ifdef __WXX11__
        wxFileName fname( dialog.GetPath() );
        if ((fname.GetExt() == "cpp") ||
            (fname.GetExt() == "c") ||
            (fname.GetExt() == "h") ||
            (fname.GetExt() == "cxx") ||
            (fname.GetExt() == "hxx"))
        {
            m_text->SetLanguage( wxSOURCE_LANG_CPP );
        }
        else
        if (fname.GetExt() == "py")
        {
            m_text->SetLanguage( wxSOURCE_LANG_PYTHON );
        }
        else
        if ((fname.GetExt() == "pl") ||
            (fname.GetExt() == "pm"))
        {
            m_text->SetLanguage( wxSOURCE_LANG_PYTHON );
        }
        else
        {
            m_text->SetLanguage( wxSOURCE_LANG_NONE );
        }
#endif

        m_filename = dialog.GetPath();
        m_text->LoadFile( m_filename );
    
        SetStatusText( m_filename );
    }
}

void MyFrame::OnSave( wxCommandEvent &event )
{
    Save();
}

void MyFrame::OnSaveAs( wxCommandEvent &event )
{
    wxFileDialog dialog( this, "Open text", "", "",
        "Text file (*.txt)|*.txt|"
        "Any file (*)|*",
        wxSAVE|wxOVERWRITE_PROMPT );
    if (dialog.ShowModal() == wxID_OK)
    {
        m_filename = dialog.GetPath();
        m_text->SaveFile( m_filename );
    
        SetStatusText( m_filename );
    }
}

void MyFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, "Welcome to wxEdit\n(C)opyright Robert Roebling",
        "About wxEdit", wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void MyFrame::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

bool MyFrame::Save()
{
    wxCommandEvent event;
    
    if (m_filename.empty())
        OnSaveAs( event );
    else
        m_text->SaveFile( m_filename );
   
   return TRUE;
}

bool MyFrame::Discard()
{
    if (m_text->IsModified())
    {
        wxMessageDialog dialog( this, "Text has been\nmodified! Save?",
            "wxEdit", wxYES_NO|wxCANCEL|wxICON_EXCLAMATION );
            
        int ret = dialog.ShowModal();
        
        if (ret == wxID_CANCEL)
            return FALSE;
            
        if (ret == wxID_YES)
        {
            if (!Save())
                return FALSE;
        }
    }
    
    return TRUE;
}

void MyFrame::OnUpdateUI( wxUpdateUIEvent &event )
{
    switch (event.GetId())
    {
        case ID_COPY:
            event.Enable( FALSE );
            break;
        case ID_CUT:
            event.Enable( FALSE );
            break;
        case ID_PASTE:
            event.Enable( FALSE );
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

void MyFrame::OnCloseWindow( wxCloseEvent &event )
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
        
    conf->Write( "/History/Count", max );
    
    for (int i = 0; i < max; i++)
    {
        wxString tmp;
        tmp.Printf( "/History/File%d", (int)i );
        
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

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
    SetVendorName("Free world");
    SetAppName("wxEdit");
    
    MyFrame *frame = new MyFrame( NULL, -1, "wxEdit", wxPoint(20,20), wxSize(500,340) );
    frame->Show( TRUE );
    
    return TRUE;
}

int MyApp::OnExit()
{
    return 0;
}

