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

#include "wxLayout.h"
#include "wx/textfile.h"


#include   "Micon.xpm"

// for testing only:
#include   <stdio.h>

//-----------------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

   enum ids{ ID_EDIT = 1, ID_ADD_SAMPLE, ID_CLEAR, ID_PRINT, ID_DPRINT,
             ID_WXLAYOUT_DEBUG, ID_QUIT, ID_CLICK, ID_HTML, ID_TEXT, ID_TEST };


IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

   BEGIN_EVENT_TABLE(MyFrame,wxFrame)
   EVT_MENU    (-1,       MyFrame::OnCommand)
   EVT_COMMAND (-1,-1,    MyFrame::OnCommand)
   EVT_CHAR    (  wxLayoutWindow::OnChar  )
   END_EVENT_TABLE()

   MyFrame::MyFrame(void) :
      wxFrame( NULL, -1, "wxLayout", wxPoint(20,20), wxSize(600,360) )
{
   CreateStatusBar( 1 );
  
   SetStatusText( "wxLayout by Karsten Ballüder." );

   wxMenu *file_menu = new wxMenu( "Menu 1" );
   file_menu->Append( ID_CLEAR, "Clear");
   file_menu->Append( ID_ADD_SAMPLE, "Example");
   file_menu->Append( ID_EDIT, "Edit");
   file_menu->Append( ID_WXLAYOUT_DEBUG, "Debug");
   file_menu->Append( ID_PRINT, "Print");
   file_menu->Append( ID_DPRINT, "Direct Print");
   file_menu->Append( ID_TEXT, "Export Text");
   file_menu->Append( ID_HTML, "Export HTML");
   file_menu->Append( ID_TEST, "Test");
   file_menu->Append( ID_QUIT, "Exit");
  
   wxMenuBar *menu_bar = new wxMenuBar();
   menu_bar->Append(file_menu, "File" );
   menu_bar->Show( TRUE );
  
   SetMenuBar( menu_bar );

   m_lwin = new wxLayoutWindow(this);
   m_lwin->SetEventId(ID_CLICK);
   m_lwin->GetLayoutList().SetEditable(true);
   m_lwin->Clear(wxROMAN,16,wxNORMAL,wxNORMAL, false);
   m_lwin->SetFocus();
};

void
MyFrame::AddSampleText(wxLayoutList &llist)
{
   llist.Insert("--");
   llist.LineBreak();

   llist.Insert("The quick brown fox jumps over the lazy dog.");
   llist.LineBreak();
   llist.Insert("Hello ");
   llist.Insert(new wxLayoutObjectIcon(new wxIcon(Micon_xpm,-1,-1)));
   llist.LineBreak();
   llist.SetFontWeight(wxBOLD);
   llist.Insert("World! ");
   llist.SetFontWeight(wxNORMAL);
   llist.Insert("The quick brown fox jumps...");
   llist.LineBreak();

   llist.Insert("over the lazy dog.");
   llist.SetFont(-1,-1,-1,-1,true);
   llist.Insert("underlined");
   llist.SetFont(-1,-1,-1,-1,false);
   llist.SetFont(wxROMAN);
   llist.Insert("This is ");
   llist.SetFont(-1,-1,-1,wxBOLD);  llist.Insert("BOLD ");  llist.SetFont(-1,-1,-1,wxNORMAL);
   llist.Insert("and ");
   llist.SetFont(-1,-1,wxITALIC);
   llist.Insert("italics ");
   llist.SetFont(-1,-1,wxNORMAL);
   llist.LineBreak();
  
   llist.Insert("and ");
   llist.SetFont(-1,-1,wxSLANT);
   llist.Insert("slanted");
   llist.SetFont(-1,-1,wxNORMAL);
   llist.Insert(" text.");
   llist.LineBreak();

   llist.Insert("and ");
   llist.SetFont(-1,-1,-1,-1,-1,"blue");
   llist.Insert("blue");
   llist.SetFont(-1,-1,-1,-1,-1,"black");
   llist.Insert(" and ");
   llist.SetFont(-1,-1,-1,-1,-1,"green","black");
   llist.Insert("green on black");
   llist.SetFont(-1,-1,-1,-1,-1,"black","white");
   llist.Insert(" text.");
   llist.LineBreak();

   llist.SetFont(-1,-1,wxSLANT);
   llist.Insert("Slanted");
   llist.SetFont(-1,-1,wxNORMAL);
   llist.Insert(" and normal text and ");
   llist.SetFont(-1,-1,wxSLANT);
   llist.Insert("slanted");
   llist.SetFont(-1,-1,wxNORMAL);
   llist.Insert(" again.");
   llist.LineBreak();

   // add some more text for testing:
   llist.Insert("And here the source for the test program:");
   llist.LineBreak();
   llist.SetFont(wxTELETYPE,16);
   char buffer[1024];
   FILE *in = fopen("wxLayout.cpp","r");
   if(in)
   {
      for(;;)
      {
         fgets(buffer,1024,in);
         if(feof(in))
            break;
         llist.Insert(buffer);
         llist.LineBreak();
      }
   }
   
   m_lwin->Refresh();
   m_lwin->UpdateScrollbars();
   llist.SetEditable();
   llist.SetCursor(wxPoint(0,0));
}

void
MyFrame::Clear(void)
{
   m_lwin->Erase();
   m_lwin->UpdateScrollbars();
}

/* test the editing */
void MyFrame::Edit(void)
{
   wxLayoutList & llist = m_lwin->GetLayoutList();
   m_lwin->SetEventId(ID_CLICK);
  
   llist.MoveCursor(0);
   llist.MoveCursor(5);
   llist.MoveCursor(0,2);
   llist.Delete(2);
   llist.MoveCursor(2);
   llist.Insert("not");
   llist.LineBreak();
   m_lwin->Refresh();
}

void MyFrame::OnCommand( wxCommandEvent &event )
{
   cerr << "id:" << event.GetId() << endl;
   switch (event.GetId())
   {
   case ID_QUIT:
      Close( TRUE );
      break;
   case ID_PRINT:
      m_lwin->Print();
      break;
   case ID_DPRINT:
   {
      wxLayoutList llist;
      AddSampleText(llist);
      wxPostScriptDC   dc("layout.ps",true,this);
      if (dc.Ok() && dc.StartDoc((char *)_("Printing message...")))
      {
         //dc.SetUserScale(1.0, 1.0);
         llist.Draw(dc); //,false,wxPoint(0,0),true);
         dc.EndDoc();
      }
   }
   break;
   case ID_EDIT:
      Edit();
      break;
   case ID_ADD_SAMPLE:
      AddSampleText(m_lwin->GetLayoutList());
      break;
   case ID_CLEAR:
      Clear();
      break;
   case ID_WXLAYOUT_DEBUG:
      m_lwin->GetLayoutList().Debug();
      break;
   case ID_CLICK:
      cerr << "Received click event." << endl;
      break;
   case ID_TEST:
   {
      Clear();
      m_lwin->GetLayoutList().LineBreak();
      m_lwin->GetLayoutList().Insert("abc");
      m_lwin->GetLayoutList().LineBreak();
      m_lwin->GetLayoutList().Insert("def");
      break;
   }
   case ID_HTML:
   {
      wxLayoutExportObject *export;
      wxLayoutList::iterator i = m_lwin->GetLayoutList().begin();

      while((export = wxLayoutExport(m_lwin->GetLayoutList(),
                                     i,WXLO_EXPORT_AS_HTML)) != NULL)
      {
         if(export->type == WXLO_EXPORT_HTML)
            cout << *(export->content.text);
         else
            cout << "<!--UNKNOWN OBJECT>";
         delete export;
      }
   }
   break;
   case ID_TEXT:
   {
      wxLayoutExportObject *export;
      wxLayoutList::iterator i = m_lwin->GetLayoutList().begin();

      while((export = wxLayoutExport(m_lwin->GetLayoutList(),
                                     i,WXLO_EXPORT_AS_TEXT)) != NULL)
      {
         if(export->type == WXLO_EXPORT_TEXT)
            cout << *(export->content.text);
         else
            cout << "<!--UNKNOWN OBJECT>";
         delete export;
      }
   }
   break;
   }
};


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
   wxSetAFMPath("/usr/local/src/wxWindows/misc/afm/");
   return TRUE;
};





