//------------------------------------------------------------------------------
// Name:        dbbrowse.cpp
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// BJO        : Bart A.M. JOURQUIN
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
//------------------------------------------------------------------------------
//-- all #ifdefs that the whole Project needs. ------------------------------
//---------------------------------------------------------------------------
#ifdef __GNUG__
 #pragma implementation
 #pragma interface
#endif
//---------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
 #pragma hdrstop
#endif
//---------------------------------------------------------------------------
#ifndef WX_PRECOMP
 #include "wx/wx.h"
#endif
//---------------------------------------------------------------------------
#ifndef __WXMSW__
#endif
 #include "bitmaps/logo.xpm"
//---------------------------------------------------------------------------
//-- all #includes that every .cpp needs             --- 19990807.mj10777 ---
//---------------------------------------------------------------------------
#include "std.h"    // sorgsam Pflegen !
#include <iostream>
//---------------------------------------------------------------------------
//-- Some Global Vars for this file ----------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MainFrame *frame = NULL;      // The one and only MainFrame
// wxLogNull logNo;              // No Log messages
//---------------------------------------------------------------------------
// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM(item)                                                \
 if (!item.IsOk())                                                      \
 {                                                                      \
  wxMessageBox(_("Please select some item first!"),_("Tree sample error"),      \
                 wxOK | wxICON_EXCLAMATION,this);                       \
  return;                                                               \
 }                                                                      \
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
 EVT_MENU(QUIT, MainFrame::OnQuit)
 EVT_MENU(ABOUT, MainFrame::OnAbout)
 EVT_MENU(HELP, MainFrame::OnHelp)
 EVT_SIZE(MainFrame::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)
//---------------------------------------------------------------------------
// `Main program' equivalent, creating windows and returning main app frame
//---------------------------------------------------------------------------
bool MyApp::OnInit(void)
{
 //---------------------------------------------------------------------------------------
 // set the language to use
 const char *langhelp = NULL;
 const char *language = NULL;
 const char *langid   = NULL;
 //---------------------------------------------------------------------------------------
#if wxUSE_LIBPNG
 wxImage::AddHandler( new wxPNGHandler );   // use only when needed, otherwise big .exe's
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
 // wxBitmap::AddHandler( new wxXPMFileHandler );   // use only when needed, otherwise big .exe's
 // wxBitmap::AddHandler( new wxXPMDataHandler );   // use only when needed, otherwise big .exe's
#endif
 //---------------------------------------------------------------------------------------
 langid = "std"; // BJO20000125 instead of ""
 switch ( argc )
 {
  default:
      // ignore the other args, fall through
  case 3:
      language = argv[2];
      langid   = argv[1];
      break;
  case 2:
      langid = argv[1];
      break;
  case 1:
      break;
 };
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
 SetVendorName("mj10777");
 SetAppName("DBBrowser"); // not needed, it's the default value
 p_ProgramCfg = wxConfigBase::Get();
 // p_ProgramCfg->DeleteAll();
 p_ProgramCfg->SetPath("/");
 wxString Temp0, Temp1;
 Temp0.Empty();

 //---------------------------------------------------------------------------------------
 //-- Set the Language and remember it for the next time. --------------------------------
 //---------------------------------------------------------------------------------------
 langhelp = "help.std/dbbrowse.hhp";
 if (langid == "std")
 {
  p_ProgramCfg->Read("/Local/langid",&Temp0); // >const char *langid< can't be used here
  if (Temp0 == "")
   langid = "std";
  else
   langid = Temp0;
 }
 Temp0 = langid;
 p_ProgramCfg->Write("/Local/langid",Temp0); // >const char *langid< can't be used here
 //---------------------------------------------------------------------------------------
 // Support the following languages
 if (Temp0 != "std")
 {
  if (Temp0 == "cz")
  {
   language = "czech";  // csy or czech
   langhelp = "help.cz/dbbrowse.hhp";
  }
  if ((Temp0 == "de") || (Temp0 == "a"))
  {
   language = "german";  // deu or german
   langhelp = "help.de/dbbrowse.hhp";
  }
  if (Temp0 == "fr")
  {
   language = "french";  // fra or french
   langhelp = "help.fr/dbbrowse.hhp";
  }
  if (Temp0 == "pl")
  {
   language = "polish";  // plk or polish
   langhelp = "help.pl/dbbrowse.hhp";
  }
  if (!m_locale.Init(language, langid, language))      // setlocale(LC_ALL,""); does not work
   wxMessageBox("SetLocale error");
  m_locale.AddCatalog("PgmText");
  m_locale.AddCatalog("Help");
  Temp0 = language;
  p_ProgramCfg->Write("/Local/language",Temp0);
 }
 else
 {
  Temp0 = "english";
  p_ProgramCfg->Write("/Local/language",Temp0);
  Temp0 = "std";    // allways english if not german or french (at the moment austrian)
 }

 Temp0 = "NONE";
 p_ProgramCfg->Write("/NONE",Temp0);
 p_ProgramCfg->Write("/Paths/NONE",Temp0);
 p_ProgramCfg->Write("/MainFrame/NONE",Temp0);
 //-----------------------------------------------------------------------------
 p_ProgramCfg->Write("/Paths/Work",wxGetCwd());
 p_ProgramCfg->SetPath("/");
 //-----------------------------------------------------------------------------
 // or you could also write something like this:
 //  wxFileConfig *p_ProgramCfg = new wxFileConfig("conftest");
 //  wxConfigBase::Set(p_ProgramCfg);
 // where you can also specify the file names explicitly if you wish.
 // Of course, calling Set() is optional and you only must do it if
 // you want to later retrieve this pointer with Get().
 //----------------------------------------------------------------------------
 // SetPath() understands ".."
 // restore frame position and size
 int x = p_ProgramCfg->Read("/MainFrame/x", 1), y = p_ProgramCfg->Read("/MainFrame/y", 1),
     w = p_ProgramCfg->Read("/MainFrame/w", 600), h = p_ProgramCfg->Read("/MainFrame/h", 450);
 //---------------------------------------------------------------------------------------
 // Create the main frame window
 frame = new MainFrame((wxFrame *) NULL, (char *) _("DBBrowser - mj10777"),x,y,w,h);
 //---------------------------------------------------------------------------------------
 // This reduces flicker effects - even better would be to define OnEraseBackground
 // to do Temp0. When the list control's scrollbars are show or hidden, the
 // frame is sent a background erase event.
 //---------------------------------------------------------------------------------------
 frame->SetBackgroundColour( *wxWHITE );
 // frame->SetBackgroundColour(wxColour(255, 255, 255));

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

 frame->p_Splitter = new DocSplitterWindow(frame, SPLITTER_WINDOW);

 // p_Splitter->SetCursor(wxCursor(wxCURSOR_PENCIL));
 frame->pDoc                       = new mjDoc();
 frame->pDoc->p_MainFrame          = frame;
 frame->pDoc->p_Splitter           = frame->p_Splitter;
 frame->pDoc->p_Splitter->pDoc     = frame->pDoc;       // ControlBase: saving the Sash
 if (!frame->pDoc->OnNewDocument())
  frame->Close(TRUE);
 // frame->Maximize();
 // frame->SetSize(-1,-1);               // the wxSplitter does not show correctly without this !
 frame->SetClientSize(width, height);    // the wxSplitter does not show correctly without this !
 //---------------------------------------------------------------------------------------
 //-- Problem : GetClientSize(Width,Hight) are not the same as the values given in the ---
 //--            construction of the Frame.                                            ---
 //-- Solved  : GetClientSize is called here and the difference is noted. When the     ---
 //--           Window is closed the diff. is added to the result of GetClientSize.    ---
 //---------------------------------------------------------------------------------------
 frame->GetClientSize(&frame->DiffW, &frame->DiffH); frame->DiffW-=w; frame->DiffH-=h;
 //----------------------------------------------------------------------------
 //-- Help    :                                                                        ---
 //----------------------------------------------------------------------------

 frame->help.UseConfig(p_ProgramCfg);
 frame->help.AddBook(langhelp);
 /*
 if (!frame->help.AddBook("helpfiles/dbbrowse.hhp"))
  wxMessageBox("Failed adding book helpfiles/dbbrowse.hhp");
 if (!frame->help.AddBook("helpfiles/testing.hhp"))
  wxMessageBox("Failed adding book helpfiles/testing.hhp");
 if (!frame->help.AddBook("helpfiles/another.hhp"))
   wxMessageBox("Failed adding book helpfiles/another.hhp");
 */


 //---------------------------------------------------------------------------------------
 p_ProgramCfg->Flush(TRUE);        // sicher Objekt

 //---------------------------------------------------------------------------------------
 // Show the frame
 frame->Show(TRUE);
 SetTopWindow(frame);



 return TRUE;
}
//---------------------------------------------------------------------------
// My frame constructor
//---------------------------------------------------------------------------
MainFrame::MainFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, SPLITTER_FRAME, title, wxPoint(x, y), wxSize(w, h))
{
 InitializeMenu();
 // This reduces flicker effects - even better would be to define OnEraseBackground
 // to do Temp0. When the list control's scrollbars are show or hidden, the
 // frame is sent a background erase event.
 SetBackgroundColour( *wxWHITE );
 // frame->SetBackgroundColour(wxColour(255, 255, 255));
 //---------------------------------------------------------------------------------------
 // Give it an icon
 SetIcon( wxICON(Logo) );
 //---------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
MainFrame::~MainFrame(void)
{
 // save the control's values to the config
 if ( p_ProgramCfg == NULL )
   return;
 // save the frame position
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
 p_ProgramCfg->Flush(TRUE);        // sichert Objekt
 delete frame->pDoc;
}
//--------------------------------------------------------------------------------------
void MainFrame::InitializeMenu()
{
 //---------------------------------------------------------------------------------------
 // Make a menubar
 //---------------------------------------------------------------------------------------
 wxMenu *file_menu = new wxMenu;

 file_menu->Append(ABOUT, _("&About"));
 file_menu->AppendSeparator();
 file_menu->Append(QUIT, _("E&xit"));


 wxMenuBar *menu_bar = new wxMenuBar;
 menu_bar->Append(file_menu, _("&File"));
 SetMenuBar(menu_bar);
}
//---------------------------------------------------------------------------
void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
 // Close the help frame; this will cause the config data to
 // get written.
 if (help.GetFrame()) // returns NULL if no help frame active
  help.GetFrame()->Close(TRUE);
 Close(TRUE);
}
//---------------------------------------------------------------------------
void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
 wxMessageDialog dialog(this, _("DBBrowser\nMark Johnson\nBerlin, Germany\nmj10777@gmx.net\n (c) 1999"),
      _("About DBBrowser"), wxOK|wxCANCEL);

 dialog.ShowModal();
}
//---------------------------------------------------------------------------
void MainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
 help.Display("Main page");
}
//---------------------------------------------------------------------------
