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

// Include private headers
#include "wxedit.h"

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
    
    EVT_MENU(ID_LAST_1, MyFrame::OnLast1)
    EVT_MENU(ID_LAST_2, MyFrame::OnLast2)
    EVT_MENU(ID_LAST_3, MyFrame::OnLast3)
    
    EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMyMenuBar();
    
    CreateStatusBar(1);
    SetStatusText( "Welcome!" );
    
    m_text = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
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
    
    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, "File" );
    
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

void MyFrame::OnLast1( wxCommandEvent &event )
{
}

void MyFrame::OnLast2( wxCommandEvent &event )
{
}

void MyFrame::OnLast3( wxCommandEvent &event )
{
}

void MyFrame::OnNew( wxCommandEvent &event )
{
    if (!Discard()) return;

    m_text->Clear();
}

void MyFrame::OnOpen( wxCommandEvent &event )
{
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

        m_text->LoadFile( dialog.GetPath() );
    }
}

void MyFrame::OnSave( wxCommandEvent &event )
{
}

void MyFrame::OnSaveAs( wxCommandEvent &event )
{
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
   m_text->SaveFile();
   
   return TRUE;
}

bool MyFrame::Discard()
{
    if (m_text->IsModified())
    {
        wxMessageDialog dialog( this, "Text has been modified! Save?",
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

void MyFrame::OnCloseWindow( wxCloseEvent &event )
{
    if (!Discard()) return;    

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
    MyFrame *frame = new MyFrame( NULL, -1, "wxEdit", wxPoint(20,20), wxSize(500,340) );
    frame->Show( TRUE );
    
    return TRUE;
}

int MyApp::OnExit()
{
    return 0;
}

