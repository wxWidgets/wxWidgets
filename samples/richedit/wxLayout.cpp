/*
 * Program: wxLayout
 *
 * Author: Karsten Ballüder
 *
 * Copyright: (C) 1998, Karsten Ballüder <Ballueder@usa.net>
 *
 */

#ifdef __GNUG__
#pragma implementation "wxLayout.h"
#endif

#include <wx/wxprec.h>
#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#include "wxLayout.h"
#include <wx/textfile.h>

#include <iostream.h>

#include   "Micon.xpm"


//-----------------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

enum ids
{
    ID_ADD_SAMPLE = 1, ID_CLEAR, ID_PRINT,
    ID_PRINT_SETUP, ID_PAGE_SETUP, ID_PREVIEW, ID_PRINT_PS,
    ID_PRINT_SETUP_PS, ID_PAGE_SETUP_PS,ID_PREVIEW_PS,
    ID_WRAP, ID_NOWRAP, ID_PASTE, ID_COPY, ID_CUT,
    ID_PASTE_PRIMARY,
    ID_FIND,
    ID_WXLAYOUT_DEBUG, ID_QUIT, ID_CLICK, ID_HTML, ID_TEXT,
    ID_TEST, ID_LINEBREAKS_TEST, ID_LONG_TEST, ID_URL_TEST
};


IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
   EVT_MENU(ID_PRINT, MyFrame::OnPrint)
   EVT_MENU(ID_PREVIEW, MyFrame::OnPrintPreview)
   EVT_MENU(ID_PRINT_SETUP, MyFrame::OnPrintSetup)
   EVT_MENU(ID_PAGE_SETUP, MyFrame::OnPageSetup)
   EVT_MENU(ID_PRINT_PS, MyFrame::OnPrintPS)
   EVT_MENU(ID_PREVIEW_PS, MyFrame::OnPrintPreviewPS)
   EVT_MENU(ID_PRINT_SETUP_PS, MyFrame::OnPrintSetupPS)
   EVT_MENU(ID_PAGE_SETUP_PS, MyFrame::OnPageSetupPS)
   EVT_MENU    (-1,       MyFrame::OnCommand)
   EVT_COMMAND (-1,-1,    MyFrame::OnCommand)
   EVT_CHAR    (  wxLayoutWindow::OnChar  )
END_EVENT_TABLE()


MyFrame::MyFrame(void) :
   wxFrame( (wxFrame *) NULL, -1, "wxLayout",
             wxPoint(880,100), wxSize(256,256) )
{
   CreateStatusBar( 2 );

   SetStatusText( "wxLayout by Karsten Ballüder." );

   wxMenuBar *menu_bar = new wxMenuBar();

   wxMenu *file_menu = new wxMenu;
   file_menu->Append(ID_PRINT, "&Print...", "Print");
   file_menu->Append(ID_PRINT_SETUP, "Print &Setup...","Setup printer properties");
   file_menu->Append(ID_PAGE_SETUP, "Page Set&up...", "Page setup");
   file_menu->Append(ID_PREVIEW, "Print Pre&view", "Preview");
#ifdef __WXMSW__
   file_menu->AppendSeparator();
   file_menu->Append(ID_PRINT_PS, "Print PostScript...", "Print (PostScript)");
   file_menu->Append(ID_PRINT_SETUP_PS, "Print Setup PostScript...", "Setup printer properties (PostScript)");
   file_menu->Append(ID_PAGE_SETUP_PS, "Page Setup PostScript...", "Page setup (PostScript)");
   file_menu->Append(ID_PREVIEW_PS, "Print Preview PostScript", "Preview (PostScript)");
#endif
   file_menu->AppendSeparator();
   file_menu->Append( ID_TEXT, "Export &Text");
   file_menu->Append( ID_HTML, "Export &HTML");
   file_menu->Append( ID_QUIT, "E&xit");
   menu_bar->Append(file_menu, "&File" );

   wxMenu *edit_menu = new wxMenu;
   edit_menu->Append( ID_CLEAR, "C&lear");
   edit_menu->Append( ID_ADD_SAMPLE, "&Example");
   edit_menu->Append( ID_LONG_TEST, "Add &many lines");
   edit_menu->AppendSeparator();
   edit_menu->Append( ID_LINEBREAKS_TEST, "Add &several lines");
   edit_menu->Append( ID_URL_TEST, "Insert an &URL");
   edit_menu->AppendSeparator();
   edit_menu->Append(ID_WRAP, "&Wrap mode", "Activate wrapping at pixel 200.");
   edit_menu->Append(ID_NOWRAP, "&No-wrap mode", "Deactivate wrapping.");
   edit_menu->AppendSeparator();
   edit_menu->Append(ID_COPY, "&Copy", "Copy text to clipboard.");
   edit_menu->Append(ID_CUT, "Cu&t", "Cut text to clipboard.");
#ifdef __WXGTK__
   edit_menu->Append(ID_PASTE,"&Paste", "Paste text from clipboard.");
#endif
   edit_menu->Append(ID_PASTE_PRIMARY,"&Paste primary", "Paste text from primary selection.");
   edit_menu->Append(ID_FIND, "&Find", "Find text.");
   menu_bar->Append(edit_menu, "&Edit" );

#ifndef __WXMSW__
   menu_bar->Show( TRUE );
#endif // MSW

   SetMenuBar( menu_bar );

   m_lwin = new wxLayoutWindow(this);
   m_lwin->SetStatusBar(GetStatusBar(), 0, 1);
   m_lwin->SetMouseTracking(true);
   m_lwin->SetEditable(true);
   m_lwin->SetWrapMargin(40);
   m_lwin->SetFocus();

   // JACS: under MSW, the window doesn't show the caret initially,
   // and the following line I added doesn't help either.
   // going to another window and then back again fixes it.
   // m_lwin->OnSetFocus(wxFocusEvent());

   Clear();

#if 0
   // create and set the background bitmap (this will result in a lattice)
   static const int sizeBmp = 10;
   wxBitmap *bitmap = new wxBitmap(sizeBmp, sizeBmp);
   wxMemoryDC dcMem;
   dcMem.SelectObject( *bitmap );
   dcMem.SetBackground( *wxWHITE_BRUSH );
   dcMem.Clear();

   dcMem.SetPen( *wxGREEN_PEN );
   dcMem.DrawLine(sizeBmp/2, 0, sizeBmp/2, sizeBmp);
   dcMem.DrawLine(0, sizeBmp/2, sizeBmp, sizeBmp/2);

   dcMem.SelectObject( wxNullBitmap );

   m_lwin->SetBackgroundBitmap(bitmap);
#endif // 0
};

void
MyFrame::AddSampleText(wxLayoutList *llist)
{
   llist->Clear(wxSWISS,16,wxNORMAL,wxNORMAL, false);
   llist->SetFont(-1,-1,-1,-1,-1,"blue");
   llist->Insert("blue");
   llist->SetFont(-1,-1,-1,-1,-1,"black");
   llist->Insert("The quick brown fox jumps over the lazy dog.");
   llist->LineBreak();

   llist->SetFont(wxROMAN,16,wxNORMAL,wxNORMAL, false);
   llist->Insert("--");
   llist->LineBreak();

   llist->SetFont(wxROMAN);
   llist->Insert("The quick brown fox jumps over the lazy dog.");
   llist->LineBreak();
   llist->Insert("Hello ");
   llist->Insert(new wxLayoutObjectIcon(new wxICON(Micon)));
   llist->SetFontWeight(wxBOLD);
   llist->Insert("World! ");
   llist->SetFontWeight(wxNORMAL);
   llist->Insert("The quick brown fox jumps...");
   llist->LineBreak();
   llist->Insert("over the lazy dog.");
   llist->SetFont(-1,-1,-1,-1,true);
   llist->Insert("underlined");
   llist->SetFont(-1,-1,-1,-1,false);
   llist->SetFont(wxROMAN);
   llist->Insert("This is ");
   llist->SetFont(-1,-1,-1,wxBOLD);  llist->Insert("BOLD ");  llist->SetFont(-1,-1,-1,wxNORMAL);
   llist->Insert("and ");
   llist->SetFont(-1,-1,wxITALIC);
   llist->Insert("italics ");
   llist->SetFont(-1,-1,wxNORMAL);
   llist->LineBreak();
   llist->Insert("and ");
   llist->SetFont(-1,-1,wxSLANT);
   llist->Insert("slanted");
   llist->SetFont(-1,-1,wxNORMAL);
   llist->Insert(" text.");
   llist->LineBreak();
   llist->Insert("and ");
   llist->SetFont(-1,-1,-1,-1,-1,"blue");
   llist->Insert("blue");
   llist->SetFont(-1,-1,-1,-1,-1,"black");
   llist->Insert(" and ");
   llist->SetFont(-1,-1,-1,-1,-1,"green","black");
   llist->Insert("green on black");
   llist->SetFont(-1,-1,-1,-1,-1,"black","white");
   llist->Insert(" text.");
   llist->LineBreak();

   llist->SetFont(-1,-1,wxSLANT);
   llist->Insert("Slanted");
   llist->SetFont(-1,-1,wxNORMAL);
   llist->Insert(" and normal text and ");
   llist->SetFont(-1,-1,wxSLANT);
   llist->Insert("slanted");
   llist->SetFont(-1,-1,wxNORMAL);
   llist->Insert(" again.");
   llist->LineBreak();

   // add some more text for testing:
   llist->Insert("And here the source for the test program:");
   llist->LineBreak();
   llist->SetFont(wxTELETYPE,16);
   llist->Insert("And here the source for the test program:");
   llist->LineBreak();
   llist->Insert("And here the source for the test program:");
   llist->LineBreak();
   llist->Insert("And here the source for the test program:");

   char buffer[1024];
   FILE *in = fopen("wxLayout.cpp","r");
   if(in)
   {
      for(;;)
      {
         fgets(buffer,1024,in);
         if(feof(in))
            break;
         wxLayoutImportText(llist, buffer);
      }
   }

   llist->MoveCursorTo(wxPoint(0,0));
   m_lwin->SetDirty();
   m_lwin->Refresh();
}

void
MyFrame::Clear(void)
{
   wxColour colBg(0, 0, 0);

   m_lwin->Clear(wxROMAN,16,wxNORMAL,wxNORMAL, false, wxRED, &colBg);
}


void MyFrame::OnCommand( wxCommandEvent &event )
{
   switch (event.GetId())
   {
   case ID_QUIT:
      Close( TRUE );
      break;
   case ID_PRINT:
   {
      wxPrinter printer;
      wxLayoutPrintout printout(m_lwin->GetLayoutList(),_("M: Printout"));
      if (! printer.Print(this, &printout, TRUE))
         wxMessageBox(
            _("There was a problem with printing the message:\n"
              "perhaps your current printer is not set up correctly?"),
            _("Printing"), wxOK);
   }
   break;
   case ID_NOWRAP:
   case ID_WRAP:
      m_lwin->SetWrapMargin(event.GetId() == ID_NOWRAP ? 0 : 40);
      break;
   case ID_ADD_SAMPLE:
      AddSampleText(m_lwin->GetLayoutList());
      break;
   case ID_CLEAR:
      Clear();
      break;
   case ID_CLICK:
      cerr << "Received click event." << endl;
      break;
   case ID_PASTE:
      m_lwin->Paste();
      m_lwin->Refresh(FALSE);
      break;
#ifdef __WXGTK__
   case ID_PASTE_PRIMARY:
      m_lwin->Paste(TRUE);
      m_lwin->Refresh(FALSE);
      break;
#endif
   case ID_COPY:
      m_lwin->Copy();
      m_lwin->Refresh(FALSE);
      break;
   case ID_CUT:
      m_lwin->Cut();
      m_lwin->Refresh(FALSE);
      break;
#ifdef M_BASEDIR
   case ID_FIND:
      m_lwin->Find("void");
      m_lwin->Refresh(FALSE);
      break;
#endif
   case ID_HTML:
   {
      wxLayoutExportObject *export0;
      wxLayoutExportStatus status(m_lwin->GetLayoutList());

      while((export0 = wxLayoutExport( &status,
                                      WXLO_EXPORT_AS_HTML)) != NULL)
      {
         if(export0->type == WXLO_EXPORT_HTML)
            cout << *(export0->content.text);
         else
            cout << "<!--UNKNOWN OBJECT>";
         delete export0;
      }
   }
   break;
   case ID_TEXT:
   {
      wxLayoutExportObject *export0;
      wxLayoutExportStatus status(m_lwin->GetLayoutList());

      while((export0 = wxLayoutExport( &status, WXLO_EXPORT_AS_TEXT)) != NULL)
      {
         if(export0->type == WXLO_EXPORT_TEXT)
            cout << *(export0->content.text);
         else
            cout << "<!--UNKNOWN OBJECT>";
         delete export0;
      }
   }
   break;
   case ID_LONG_TEST:
   {
      wxString line;
      wxLayoutList *llist = m_lwin->GetLayoutList();
      for(int i = 1; i < 300; i++)
      {
         line.Printf("This is line number %d.", i);
         llist->Insert(line);
         llist->LineBreak();
      }
      llist->MoveCursorTo(wxPoint(0,0));
      m_lwin->SetDirty();
      m_lwin->Refresh();
      break;
   }

   case ID_LINEBREAKS_TEST:
      wxLayoutImportText(m_lwin->GetLayoutList(),
                         "This is a text\n"
                         "with embedded line\n"
                         "breaks.\n");
      m_lwin->SetDirty();
      m_lwin->Refresh();
      break;

   case ID_URL_TEST:
      // VZ: this doesn't work, of course, but I think it should -
      //     wxLayoutWindow should have a flag m_highlightUrls and do it itself
      //     (instead of doing it manually like M does now)
      m_lwin->GetLayoutList()->Insert("http://www.wxwindows.org/");
   }
};

void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
   wxGetApp().SetPrintMode(wxPRINT_WINDOWS);
#else
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
   wxPrinter printer;
   wxLayoutPrintout printout( m_lwin->GetLayoutList(),"Printout from wxLayout");
   if (! printer.Print(this, &printout, TRUE))
      wxMessageBox(
         "There was a problem printing.\nPerhaps your current printer is not set correctly?",
         "Printing", wxOK);
}

void MyFrame::OnPrintPS(wxCommandEvent& WXUNUSED(event))
{
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);

#ifdef OS_UNIX
   wxPostScriptPrinter printer;
   wxLayoutPrintout printout( m_lwin->GetLayoutList(),"My printout");
   printer.Print(this, &printout, TRUE);
#endif
}

void MyFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
   wxGetApp().SetPrintMode(wxPRINT_WINDOWS);
#else
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
   wxPrintData printData;

   // Pass two printout objects: for preview, and possible printing.
   wxPrintPreview *preview = new wxPrintPreview(new
                                                wxLayoutPrintout(
                                                   m_lwin->GetLayoutList()), new wxLayoutPrintout( m_lwin->GetLayoutList()), & printData);
   if (!preview->Ok())
   {
      delete preview;
      wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
      return;
   }

   wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
   frame->Centre(wxBOTH);
   frame->Initialize();
   frame->Show(TRUE);
}

void MyFrame::OnPrintPreviewPS(wxCommandEvent& WXUNUSED(event))
{
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);

   wxPrintData printData;

   // Pass two printout objects: for preview, and possible printing.
   wxPrintPreview *preview = new wxPrintPreview(new wxLayoutPrintout( m_lwin->GetLayoutList()), new wxLayoutPrintout( m_lwin->GetLayoutList()), & printData);
   wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
   frame->Centre(wxBOTH);
   frame->Initialize();
   frame->Show(TRUE);
}

void MyFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
#ifdef OS_WIN
   wxGetApp().SetPrintMode(wxPRINT_WINDOWS);
#else
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
   wxPrintDialog printerDialog(this, & m_PrintData);
   printerDialog.ShowModal();
}

void MyFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
   wxGetApp().SetPrintMode(wxPRINT_WINDOWS);
#else
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
   wxPageSetupData data;

#ifdef __WXMSW__
   wxPageSetupDialog pageSetupDialog(this, & data);
#else
   wxGenericPageSetupDialog pageSetupDialog(this, & data);
#endif
   pageSetupDialog.ShowModal();

   data = pageSetupDialog.GetPageSetupData();
}

void MyFrame::OnPrintSetupPS(wxCommandEvent& WXUNUSED(event))
{
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);

   wxPrintData data;

#ifdef __WXMSW__
   wxPrintDialog printerDialog(this, & data);
#else
   wxGenericPrintDialog printerDialog(this, & data);
#endif
   printerDialog.ShowModal();
}

void MyFrame::OnPageSetupPS(wxCommandEvent& WXUNUSED(event))
{
   wxGetApp().SetPrintMode(wxPRINT_POSTSCRIPT);

   wxPageSetupData data;
#ifdef __WXMSW__
   wxPageSetupDialog pageSetupDialog(this, & data);
#else
   wxGenericPageSetupDialog pageSetupDialog(this, & data);
#endif
   pageSetupDialog.ShowModal();
}


//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

MyApp::MyApp(void) :
   wxApp( )
{
};

bool MyApp::OnInit(void)
{
   wxFrame *frame = new MyFrame();
   frame->Show( TRUE );
//   wxSetAFMPath("/usr/local/src/wxWindows/misc/afm/");
   return TRUE;
};





