//----------------------------------------------------------------------------------------
// Name:        dbbrowse.cpp
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson
// Modified by:
// BJO        : Bart A.M. JOURQUIN
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//----------------------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#endif
//----------------------------------------------------------------------------------------
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/stockitem.h"
//----------------------------------------------------------------------------------------
#ifndef __WXMSW__
#include "bitmaps/logo.xpm"
#endif
//----------------------------------------------------------------------------------------
//-- all #includes that every .cpp needs             --- 19990807.mj10777 ----------------
//----------------------------------------------------------------------------------------
#include "std.h"    // sorgsam Pflegen !
// #include <iostream>
//----------------------------------------------------------------------------------------
//-- Some Global Vars for this file ------------------------------------------------------
//----------------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)                  // Program End
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)                // Program Discription
    EVT_MENU(wxID_HELP, MainFrame::OnHelp)                  // Program Help
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
IMPLEMENT_APP(MainApp)      // This declares wxApp::MainApp as "the" Application

//----------------------------------------------------------------------------------------
// 'Main program' equivalent, creating windows and returning main app frame
//----------------------------------------------------------------------------------------
bool MainApp::OnInit(void)  // Does everything needed for a program start
{
    wxString Temp0;            // Use as needed
    //---------------------------------------------------------------------------------------
    // set the language to use   // Help.??  (.std = english, .de = german etc.)
    const wxChar *language = NULL; // czech, german, french, polish
    const wxChar *langid   = NULL; // std = english , cz, de = german, fr = french, pl = polish
    wxString   s_LangHelp;       // Directory/Filename.hhp of the Help-Project file
    wxString   s_LangId, s_Language;
    s_Language.Empty(); s_LangId.Empty(); s_LangHelp.Empty();
    //---------------------------------------------------------------------------------------
    //-- Graphic File suport - use only when needed, otherwise big .exe's
    //---------------------------------------------------------------------------------------
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );   // needed for help System
#endif
/*
    #if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler );   // use only when needed, otherwise big .exe's
    #endif
    wxImage::AddHandler( new wxGIFHandler );   // use only when needed, otherwise big .exe's
    wxImage::AddHandler( new wxPCXHandler );   // use only when needed, otherwise big .exe's
    wxImage::AddHandler( new wxPNMHandler );   // use only when needed, otherwise big .exe's
*/
#ifdef __WXMSW__
    // wxBitmap::AddHandler( new wxXPMFileHandler );   // Attempt to use XPS instead of ico
    // wxBitmap::AddHandler( new wxXPMDataHandler );   // - Attempt failed
#endif
    //---------------------------------------------------------------------------------------
    switch ( argc )
    {
    default:
        // ignore the other args, fall through
    case 3:
        language = argv[2];         // czech, english, french, german , polish
        langid   = argv[1];         // cz, std, fr, de , pl
        break;
    case 2:
        langid   = argv[1];         // cz, std, fr, de , pl
        break;
    case 1:
    case 0:
        break;
    };
    //---------------------------------------------------------------------------------------
    // Win-Registry : Workplace\HKEY_CURRENT_USERS\Software\%GetVendorName()\%GetAppName()
    //---------------------------------------------------------------------------------------
    SetVendorName(_T("mj10777"));           // Needed to get Configuration Information
    SetAppName(_T("DBBrowse"));             // "" , also needed for s_LangHelp
    //---------------------------------------------------------------------------------------
    // we're using wxConfig's "create-on-demand" feature: it will create the
    // config object when it's used for the first time. It has a number of
    // advantages compared with explicitly creating our wxConfig:
    //  1) we don't pay for it if we don't use it
    //  2) there is no danger to create it twice

    // application and vendor name are used by wxConfig to construct the name
    // of the config file/registry key and must be set before the first call
    // to Get() if you want to override the default values (the application
    // name is the name of the executable and the vendor name is the same)
    //---------------------------------------------------------------------------------------
    p_ProgramCfg = wxConfigBase::Get();  // Get Program Configuration from Registry
    // p_ProgramCfg->DeleteAll();           // This is how the Config can be erased
    p_ProgramCfg->SetPath(_T("/"));          // Start at root
    //---------------------------------------------------------------------------------------
    //-- Set the Language and remember it for the next time. --------------------------------
    //---------------------------------------------------------------------------------------
    if (langid == NULL) // No Parameter was given
    {
        Temp0.Empty();
        p_ProgramCfg->Read(_T("/Local/langid"),&Temp0); // >const char *langid< can't be used here
        if (Temp0.empty())
            langid = _T("std");  // Standard language is "std" = english
        else
            langid = Temp0;
    }
    Temp0.Printf(_T("%s"),langid);
    //---------------------------------------------------------------------------------------
    // Support the following languages  (std = english)
    if ((Temp0 == _T("a"))  || (Temp0 == _T("cz")) || (Temp0 == _T("de")) ||
        (Temp0 == _T("fr")) || (Temp0 == _T("pl")))
    { // The three-letter language-string codes are only valid in Windows NT and Windows 95.
        if (Temp0 == _T("cz"))
            language = _T("czech");  // csy or czech
        if ((Temp0 == _T("de")) || (Temp0 == _T("a")))
        {
            language = _T("german");  // deu or german
            if (Temp0 == _T("a"))
            { langid = Temp0 = _T("de"); }  // Austrian = german
        } // german / austrian
        if (Temp0 == _T("fr"))
            language = _T("french");  // fra or french
        if (Temp0 == _T("pl"))
            language = _T("polish");  // plk or polish
        if (!m_locale.Init(language, langid, language)) // Don't do this for english (std)
        { // You should recieve errors here for cz and pl since there is no cz/ and pl/ directory
            wxLogMessage(_T("-E-> %s : SetLocale error : langid(%s) ; language(%s)"),GetAppName().c_str(),langid,language);
            langid = _T("std");
            language = _T("C");  // english, english-aus , -can , -nz , -uk , -usa
        }
        else
        { // Read in Foreign language's text for GetAppName() and Help
            Temp0 = GetAppName();
            Temp0 = Temp0.Lower();
            m_locale.AddCatalog(Temp0.c_str());
            m_locale.AddCatalog(_T("help"));
        }
    } // Support the following languages  (std = english)
    else
    {
        langid = _T("std");
        language = _T("C");  // english, english-aus , -can , -nz , -uk , -usa
    }
    s_Language.Printf(_T("%s"),language);                       // language is a pointer
    s_LangId.Printf(_T("%s"),langid);                           // langid   is a pointer
    p_ProgramCfg->Write(_T("/Local/language"),s_Language);
    p_ProgramCfg->Write(_T("/Local/langid"),s_LangId);
    s_LangHelp.Printf(_T("help.%s/%s.hhp"),s_LangId.c_str(),GetAppName().c_str()); // "help.std/Garantie.hhp";
    s_LangHelp = s_LangHelp.Lower();                       // A must for Linux
    //---------------------------------------------------------------------------------------
    Temp0 = _T("NONE");                               // I don't remember why I did this
    p_ProgramCfg->Write(_T("/NONE"),Temp0);           // I don't remember why I did this
    p_ProgramCfg->Write(_T("/Paths/NONE"),Temp0);     // I don't remember why I did this
    p_ProgramCfg->Write(_T("/MainFrame/NONE"),Temp0); // I don't remember why I did this
    //---------------------------------------------------------------------------------------
    p_ProgramCfg->Write(_T("/Paths/Work"),wxGetCwd()); // Get current Working Path
    p_ProgramCfg->SetPath(_T("/"));
    //---------------------------------------------------------------------------------------
    // restore frame position and size, if empty start Values (1,1) and (750,600)
    int x = p_ProgramCfg->Read(_T("/MainFrame/x"), 1), y = p_ProgramCfg->Read(_T("/MainFrame/y"), 1),
        w = p_ProgramCfg->Read(_T("/MainFrame/w"), 750), h = p_ProgramCfg->Read(_T("/MainFrame/h"), 600);
    //---------------------------------------------------------------------------------------
    // Create the main frame window
    Temp0.Printf(_T("%s - %s"),GetAppName().c_str(),GetVendorName().c_str());
    frame = new MainFrame((wxFrame *) NULL,(wxChar *) Temp0.c_str(),wxPoint(x,y),wxSize(w,h));
    //---------------------------------------------------------------------------------------
    // Set the Backgroundcolour (only needed if you are NOT using wxSYS_COLOUR_BACKGROUND)
    frame->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
    // frame->SetBackgroundColour(wxColour(255, 255, 255));
    // frame->SetBackgroundColour(* wxWHITE);
    //---------------------------------------------------------------------------------------
    // Give it an icon
    //---------------------------------------------------------------------------------------
    // 12.02.2000 - Guillermo Rodriguez Garcia :
    //---------------------------------------------------------------------------------------
    // This is different for Win9x and WinNT; one of them takes the first ico
    // in the .rc file, while the other takes the icon with the lowest name,
    // so to be sure that it always work, put your icon the first *and* give
    // it a name such a 'appicon' or something.
    //---------------------------------------------------------------------------------------
    // mj10777 : any special rule in Linux ?
    //---------------------------------------------------------------------------------------
    frame->SetIcon(wxICON(aLogo));    // lowest name and first entry in RC File
    //---------------------------------------------------------------------------------------
    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *help_menu = new wxMenu;

    help_menu->Append(wxID_HELP, wxGetStockLabel(wxID_HELP));
    help_menu->AppendSeparator();
    help_menu->Append(wxID_ABOUT, _("&About"));
    file_menu->Append(wxID_EXIT, wxGetStockLabel(wxID_EXIT));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    menu_bar->Append(help_menu, _("&Help"));
    frame->SetMenuBar(menu_bar);
#if wxUSE_STATUSBAR
    frame->CreateStatusBar(1);
    Temp0.Printf(_("%s has started !"),p_ProgramCfg->GetAppName().c_str());
    frame->SetStatusText(Temp0, 0);
#endif // wxUSE_STATUSBAR
    //---------------------------------------------------------------------------------------
    int width, height;
    frame->GetClientSize(&width, &height);
    //---------------------------------------------------------------------------------------
    frame->p_Splitter = new DocSplitterWindow(frame,wxID_ANY);
    // p_Splitter->SetCursor(wxCursor(wxCURSOR_PENCIL));
    frame->pDoc                       = new MainDoc();
    frame->pDoc->p_MainFrame          = frame;
    frame->pDoc->p_Splitter           = frame->p_Splitter;
    frame->pDoc->p_Splitter->pDoc     = frame->pDoc;       // ControlBase: saving the Sash
    //---------------------------------------------------------------------------------------
    //-- Problem : GetClientSize(Width,Height) are not the same as the values given in the ---
    //--            construction of the Frame.                                            ---
    //-- Solved  : GetClientSize is called here and the difference is noted. When the     ---
    //--           Window is closed the diff. is added to the result of GetClientSize.    ---
    //---------------------------------------------------------------------------------------
    frame->GetClientSize(&frame->DiffW, &frame->DiffH); frame->DiffW-=w; frame->DiffH-=h;
    //----------------------------------------------------------------------------
    //-- Help    : Load the help.%langid/%GetAppName().hhp (help.std/dbbrowse.hhp) file                                                                       ---
    //----------------------------------------------------------------------------
    frame->p_Help = new wxHtmlHelpController();   // construct the Help System
    frame->p_Help->UseConfig(p_ProgramCfg);       // Don't rember what this was for
    // You should recieve errors here for fr since there is no help.fr/ directory
    if (!frame->p_Help->AddBook(s_LangHelp))      // Use the language set
    { // You should recieve errors here for fr since there is no help.fr/ but a fr/ directory
        wxLogMessage(_T("-E-> %s : AddBook error : s_LangHelp(%s)"),GetAppName().c_str(),s_LangHelp.c_str());
    }
    frame->pDoc->p_Help = frame->p_Help;          // Save the information to the document
    //---------------------------------------------------------------------------------------
    frame->Show(true);                            // Show the frame
    SetTopWindow(frame);                          // At this point the frame can be seen
    //---------------------------------------------------------------------------------------
    // If you need a "Splash Screen" because of a long OnNewDocument, do it here
    if (!frame->pDoc->OnNewDocument())
        frame->Close(true);
    // Kill a "Splash Screen" because OnNewDocument, if you have one
    //---------------------------------------------------------------------------------------
    p_ProgramCfg->Flush(true);        // save the configuration
    return true;
} // bool MainApp::OnInit(void)

//----------------------------------------------------------------------------------------
// My frame constructor
//----------------------------------------------------------------------------------------
MainFrame::MainFrame(wxFrame *frame, wxChar *title,  const wxPoint& pos, const wxSize& size):
wxFrame(frame, wxID_ANY, title,  pos, size)
{
    p_Splitter = NULL; pDoc = NULL; p_Help = NULL;    // Keep the Pointers clean !
    //--- Everything else is done in MainApp::OnInit() --------------------------------------
}

//----------------------------------------------------------------------------------------
MainFrame::~MainFrame(void)
{
    // Close the help frame; this will cause the config data to get written.
    if (p_Help->GetFrame()) // returns NULL if no help frame active
        p_Help->GetFrame()->Close(true);
    delete p_Help;  // Memory Leak
    p_Help = NULL;
    // save the control's values to the config
    if (p_ProgramCfg == NULL)
        return;
    // save the frame position before it is destroyed
    int x, y, w, h;
    GetPosition(&x, &y);
    GetClientSize(&w, &h); w -= DiffW; h -= DiffH;
    p_ProgramCfg->Write(_T("/MainFrame/x"), (long) x);
    p_ProgramCfg->Write(_T("/MainFrame/y"), (long) y);
    p_ProgramCfg->Write(_T("/MainFrame/w"), (long) w);
    p_ProgramCfg->Write(_T("/MainFrame/h"), (long) h);
    p_ProgramCfg->Write(_T("/MainFrame/Sash"), (long) pDoc->Sash);
    // clean up: Set() returns the active config object as Get() does, but unlike
    // Get() it doesn't try to create one if there is none (definitely not what
    // we want here!)
    // delete wxConfigBase::Set((wxConfigBase *) NULL);
    p_ProgramCfg->Flush(true);        // saves   Objekt
    if (pDoc)                         // If we have a Valid Document
        delete pDoc;                     // Cleanup (MainDoc::~MainDoc)
} // MainFrame::~MainFrame(void)

//----------------------------------------------------------------------------------------
void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//----------------------------------------------------------------------------------------
void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString Temp0, Temp1;
    Temp0.Printf(_("%s\nMark Johnson\nBerlin, Germany\nwxWindows@mj10777.de\n (c) 2000"),p_ProgramCfg->GetAppName().c_str());
    Temp1.Printf(_("About %s"),p_ProgramCfg->GetAppName().c_str());
    wxMessageDialog dialog(this, Temp0,Temp1,wxOK|wxCANCEL);
    dialog.ShowModal();
}

//----------------------------------------------------------------------------------------
void MainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    p_Help->Display(_T("Main page"));
}
//----------------------------------------------------------------------------------------
