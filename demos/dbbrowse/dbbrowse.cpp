//----------------------------------------------------------------------------------------
// Name:        dbbrowse.cpp
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// BJO        : Bart A.M. JOURQUIN
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
//-- all #ifdefs that the whole Project needs. -------------------------------------------
//----------------------------------------------------------------------------------------
#ifdef __GNUG__
 #pragma implementation
 #pragma interface
#endif
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
 EVT_MENU(QUIT, MainFrame::OnQuit)                  // Program End
 EVT_MENU(ABOUT, MainFrame::OnAbout)                // Program Discription
 EVT_MENU(HELP, MainFrame::OnHelp)                  // Program Help
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
 const char *language = NULL; // czech, german, french, polish
 const char *langid   = NULL; // std = english , cz, de = german, fr = french, pl = polish
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
      break;
 };
 //---------------------------------------------------------------------------------------
 // Win-Registry : Workplace\HKEY_CURRENT_USERS\Software\%GetVendorName()\%GetAppName()
 //---------------------------------------------------------------------------------------
 SetVendorName("mj10777");           // Needed to get Configuration Information
 SetAppName("DBBrowse");            // "" , also needed for s_LangHelp
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
 p_ProgramCfg->SetPath("/");          // Start at root
 //---------------------------------------------------------------------------------------
 //-- Set the Language and remember it for the next time. --------------------------------
 //---------------------------------------------------------------------------------------
 if (langid == NULL) // No Parameter was given
 {
  Temp0.Empty();
  p_ProgramCfg->Read("/Local/langid",&Temp0); // >const char *langid< can't be used here
  if (Temp0 == "")
   langid = "std";  // Standard language is "std" = english
  else
   langid = Temp0;
 }
 Temp0.Printf("%s",langid);
 //---------------------------------------------------------------------------------------
 // Support the following languages  (std = english)
 if ((Temp0 == "a")  || (Temp0 == "cz") || (Temp0 == "de") ||
     (Temp0 == "fr") || (Temp0 == "pl"))
 { // The three-letter language-string codes are only valid in Windows NT and Windows 95.
  if (Temp0 == "cz")
   language = "czech";  // csy or czech
  if ((Temp0 == "de") || (Temp0 == "a"))
  {
   language = "german";  // deu or german
   if (Temp0 == "a")
   { langid = Temp0 = "de"; }  // Austrian = german
  } // german / austrian
  if (Temp0 == "fr")
   language = "french";  // fra or french
  if (Temp0 == "pl")
   language = "polish";  // plk or polish
  if (!m_locale.Init(language, langid, language)) // Don't do this for english (std)
  { // You should recieve errors here for cz and pl since there is no cz/ and pl/ directory
   wxLogMessage("-E-> %s : SetLocale error : langid(%s) ; language(%s)",GetAppName().c_str(),langid,language);
   langid = "std";
   language = "C";  // english, english-aus , -can , -nz , -uk , -usa
  }
  else
  { // Read in Foreign language's text for GetAppName() and Help
   Temp0 = GetAppName();
   Temp0 = Temp0.Lower();
   m_locale.AddCatalog(Temp0.c_str());
   m_locale.AddCatalog("help");
  }
 } // Support the following languages  (std = english)
 else
 {
  langid = "std";
  language = "C";  // english, english-aus , -can , -nz , -uk , -usa
 }
 s_Language.Printf("%s",language);                       // language is a pointer
 s_LangId.Printf("%s",langid);                           // langid   is a pointer
 p_ProgramCfg->Write("/Local/language",s_Language);
 p_ProgramCfg->Write("/Local/langid",s_LangId);
 s_LangHelp.Printf("help.%s/%s.hhp",s_LangId.c_str(),GetAppName().c_str()); // "help.std/Garantie.hhp";
 s_LangHelp = s_LangHelp.Lower();                       // A must for Linux
 //---------------------------------------------------------------------------------------
 Temp0 = "NONE";                               // I don't remember why I did this
 p_ProgramCfg->Write("/NONE",Temp0);           // I don't remember why I did this
 p_ProgramCfg->Write("/Paths/NONE",Temp0);     // I don't remember why I did this
 p_ProgramCfg->Write("/MainFrame/NONE",Temp0); // I don't remember why I did this
 //---------------------------------------------------------------------------------------
 p_ProgramCfg->Write("/Paths/Work",wxGetCwd()); // Get current Working Path
 p_ProgramCfg->SetPath("/");
 //---------------------------------------------------------------------------------------
 // restore frame position and size, if empty start Values (1,1) and (750,600)
 int x = p_ProgramCfg->Read("/MainFrame/x", 1), y = p_ProgramCfg->Read("/MainFrame/y", 1),
     w = p_ProgramCfg->Read("/MainFrame/w", 750), h = p_ProgramCfg->Read("/MainFrame/h", 600);
 //---------------------------------------------------------------------------------------
 // Create the main frame window
 Temp0.Printf("%s - %s",GetAppName().c_str(),GetVendorName().c_str());
 frame = new MainFrame((wxFrame *) NULL,(char *) Temp0.c_str(),wxPoint(x,y),wxSize(w,h));
 //---------------------------------------------------------------------------------------
 // Set the Backgroundcolour (only need if your are NOT using wxSYS_COLOUR_BACKGROUND)
 frame->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_BACKGROUND));
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

 help_menu->Append(HELP, _("&Help"));
 help_menu->AppendSeparator();
 help_menu->Append(ABOUT, _("&About"));
 file_menu->Append(QUIT, _("E&xit"));

 wxMenuBar *menu_bar = new wxMenuBar;
 menu_bar->Append(file_menu, _("&File"));
 menu_bar->Append(help_menu, _("&Help"));
 frame->SetMenuBar(menu_bar);
 frame->CreateStatusBar(1);
 Temp0.Printf(_("%s has started !"),p_ProgramCfg->GetAppName().c_str());
 frame->SetStatusText(Temp0, 0);
 //---------------------------------------------------------------------------------------
 int width, height;
 frame->GetClientSize(&width, &height);
 //---------------------------------------------------------------------------------------
 frame->p_Splitter = new DocSplitterWindow(frame,-1);
 // p_Splitter->SetCursor(wxCursor(wxCURSOR_PENCIL));
 frame->pDoc                       = new MainDoc();
 frame->pDoc->p_MainFrame          = frame;
 frame->pDoc->p_Splitter           = frame->p_Splitter;
 frame->pDoc->p_Splitter->pDoc     = frame->pDoc;       // ControlBase: saving the Sash
 //---------------------------------------------------------------------------------------
 //-- Problem : GetClientSize(Width,Hight) are not the same as the values given in the ---
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
  wxLogMessage("-E-> %s : AddBook error : s_LangHelp(%s)",GetAppName().c_str(),s_LangHelp.c_str());
 }
 frame->pDoc->p_Help = frame->p_Help;          // Save the information to the document
 //---------------------------------------------------------------------------------------
 frame->Show(TRUE);                            // Show the frame
 SetTopWindow(frame);                          // At this point the frame can be seen
 //---------------------------------------------------------------------------------------
 // If you need a "Splash Screen" because of a long OnNewDocument, do it here
 if (!frame->pDoc->OnNewDocument())
  frame->Close(TRUE);
 // Kill a "Splash Screen" because OnNewDocument, if you have one
 //---------------------------------------------------------------------------------------
 p_ProgramCfg->Flush(TRUE);        // save the configuration
 return TRUE;
} // bool MainApp::OnInit(void)
//----------------------------------------------------------------------------------------
// My frame constructor
//----------------------------------------------------------------------------------------
MainFrame::MainFrame(wxFrame *frame, char *title,  const wxPoint& pos, const wxSize& size):
  wxFrame(frame, -1, title,  pos, size)
{
 p_Splitter = NULL; pDoc = NULL; p_Help = NULL;    // Keep the Pointers clean !
 //--- Everything else is done in MainApp::OnInit() --------------------------------------
}
//----------------------------------------------------------------------------------------
MainFrame::~MainFrame(void)
{
 // save the control's values to the config
 if (p_ProgramCfg == NULL)
   return;
 // save the frame position before it is destroyed
 int x, y, w, h;
 GetPosition(&x, &y);
 GetClientSize(&w, &h); w -= DiffW; h -= DiffH;
 p_ProgramCfg->Write("/MainFrame/x", (long) x);
 p_ProgramCfg->Write("/MainFrame/y", (long) y);
 p_ProgramCfg->Write("/MainFrame/w", (long) w);
 p_ProgramCfg->Write("/MainFrame/h", (long) h);
 p_ProgramCfg->Write("/MainFrame/Sash", (long) pDoc->Sash);
 // clean up: Set() returns the active config object as Get() does, but unlike
 // Get() it doesn't try to create one if there is none (definitely not what
 // we want here!)
 // delete wxConfigBase::Set((wxConfigBase *) NULL);
 p_ProgramCfg->Flush(TRUE);        // saves   Objekt
 if (pDoc)                         // If we have a Valid Document
  delete pDoc;                     // Cleanup (MainDoc::~MainDoc)
} // MainFrame::~MainFrame(void)
//----------------------------------------------------------------------------------------
void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
 // Close the help frame; this will cause the config data to get written.
 if (p_Help->GetFrame()) // returns NULL if no help frame active
  p_Help->GetFrame()->Close(TRUE);
 Close(TRUE);
}
//----------------------------------------------------------------------------------------
void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
 wxString Temp0, Temp1;
 Temp0.Printf(_("%s\nMark Johnson\nBerlin, Germany\nmj10777@gmx.net\n (c) 2000"),p_ProgramCfg->GetAppName().c_str());
 Temp1.Printf(_("About %s"),p_ProgramCfg->GetAppName().c_str());
 wxMessageDialog dialog(this, Temp0,Temp1,wxOK|wxCANCEL);
 dialog.ShowModal();
}
//----------------------------------------------------------------------------------------
void MainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
 p_Help->Display("Main page");
}
//----------------------------------------------------------------------------------------
