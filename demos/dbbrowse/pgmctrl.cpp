//---------------------------------------------------------------------------
// Name:        pgmctrl.h
// Purpose:     Programm Control with a Tree
// Author:      Mark Johnson
// Modified by: 19990806.mj10777
// Created:     19991010
// RCS-ID:      $Id$
// Copyright:   (c) Mark Johnson, Berlin Germany, mj10777@gmx.net
// Licence:     wxWindows license
//---------------------------------------------------------------------------
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
#include "bitmaps/d_closed.xpm"
#include "bitmaps/d_open.xpm"
#include "bitmaps/f_closed.xpm"
#include "bitmaps/f_open.xpm"
#include "bitmaps/logo.xpm"
#include "bitmaps/dsnclose.xpm"
#include "bitmaps/dsnopen.xpm"
#endif
//---------------------------------------------------------------------------
//-- all #includes that every .cpp needs             --- 19990807.mj10777 ---
//---------------------------------------------------------------------------
#include "std.h"    // sorgsam Pflegen !
//---------------------------------------------------------------------------
//-- Global functions -------------------------------------------------------
//---------------------------------------------------------------------------
static inline const char *bool2String(bool b)
{
  return b ? "" : "not ";
}
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PgmCtrl, wxTreeCtrl)
  EVT_MOTION (PgmCtrl::OnMouseMove)
  EVT_LEFT_DCLICK(PgmCtrl::OnSelChanged)
  EVT_TREE_ITEM_RIGHT_CLICK(TREE_CTRL_PGM,PgmCtrl::OnRightSelect)
  EVT_MENU(PGMCTRL_ODBC_USER,PgmCtrl::OnUserPassword)
  END_EVENT_TABLE()
  //---------------------------------------------------------------------------
  // PgmCtrl implementation
  //---------------------------------------------------------------------------
  IMPLEMENT_DYNAMIC_CLASS(PgmCtrl, wxTreeCtrl)
  //---------------------------------------------------------------------------
  PgmCtrl::PgmCtrl(wxWindow *parent) : wxTreeCtrl(parent)
{
}
PgmCtrl::PgmCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size, long style)
  : wxTreeCtrl(parent, id, pos, size, style)
{
  //wxFont* ft_Temp = new wxFont(10,wxSWISS,wxNORMAL,wxBOLD,FALSE,"Comic Sans MS");
  //SetFont(* ft_Temp);
  
  // Make an image list containing small icons
  p_imageListNormal = new wxImageList(16, 16, TRUE);
  
  // should correspond to TreeIc_xxx enum
#if defined(__WXMSW__) && defined(__WIN16__)
  // This is required in 16-bit Windows mode only because we can't load a specific (16x16)
  // icon image, so it comes out stretched
  p_imageListNormal->Add(wxBitmap("bitmap1", wxBITMAP_TYPE_BMP_RESOURCE));
  p_imageListNormal->Add(wxBitmap("bitmap2", wxBITMAP_TYPE_BMP_RESOURCE));
#else
  p_imageListNormal->Add(wxICON(Logo));
  p_imageListNormal->Add(wxICON(DsnClosed));
  p_imageListNormal->Add(wxICON(DsnOpen));
  p_imageListNormal->Add(wxICON(DocClosed));
  p_imageListNormal->Add(wxICON(DocOpen));
  p_imageListNormal->Add(wxICON(FolderClosed));
  p_imageListNormal->Add(wxICON(FolderOpen));
#endif
  
  SetImageList(p_imageListNormal);
  
  // Add some items to the tree
  // AddTestItemsToTree(3, 2);
}
//---------------------------------------------------------------------------
PgmCtrl::~PgmCtrl()
{
  // wxMessageBox("PgmCtrl::~PgmCtrl() - vor  DeleteAllItems");
  // DeleteAllItems();
  // wxMessageBox("nach DeleteAllItems");
  delete p_imageListNormal;
}
//---------------------------------------------------------------------------
#undef TREE_EVENT_HANDLER
//---------------------------------------------------------------------------
int  PgmCtrl::OnPopulate()
{
  wxTreeItemId Root, Folder, Docu;
  //----------------------------------------------------------------------------------------------------------------------------
  int i;
  double dTmp = 1234567.89;
  Temp0.Printf(_("%s Functions"),p_ProgramCfg->GetAppName().c_str());
  Root   = AddRoot(Temp0,TreeIc_Logo,TreeIc_Logo, new TreeData("Root"));
  //----------------------------------------------------------------------------------------------------------------------------
  Folder = AppendItem(Root, _("Program settings")   ,TreeIc_FolderClosed, TreeIc_FolderOpen, new TreeData("Settings"));
  p_ProgramCfg->Read("/Local/langid",&Temp0); p_ProgramCfg->Read("/Local/language",&Temp2);
  Temp1.Printf(_("locale (%s) ; Language (%s) ; Number(%2.2f)"),Temp0.c_str(), Temp2.c_str(), dTmp); Temp0.Empty();  Temp2.Empty();
  Docu   = AppendItem(Folder, Temp1                 ,TreeIc_DocClosed,          TreeIc_DocOpen,          new TreeData("Setting Language"));
  p_ProgramCfg->Read("/Paths/Work",&Temp0); Temp1.Printf(_("Work Path : %s"),Temp0.c_str()); Temp0.Empty();
  Docu   = AppendItem(Folder, Temp1                 ,TreeIc_DocClosed,          TreeIc_DocOpen,          new TreeData("Path Work"));
  Docu   = AppendItem(Folder, _("Change the language to English") ,TreeIc_DocClosed,          TreeIc_DocOpen,          new TreeData("Language English"));
  Docu   = AppendItem(Folder, _("Change the language to German")  ,TreeIc_DocClosed,          TreeIc_DocOpen,          new TreeData("Language German"));
  Docu   = AppendItem(Folder, _("Delete all wxConfigBase Entry's"),TreeIc_DocClosed,          TreeIc_DocOpen,          new TreeData("wxConfigBase Delete"));
  Folder = AppendItem(Root, "ODBC DSN"  ,TreeIc_FolderClosed, TreeIc_FolderOpen, new TreeData("ODBC-DSN"));
  for (i=0;i<pDoc->i_DSN;i++)
    {
      Temp0.Printf("ODBC-%s",(pDoc->p_DSN+i)->Dsn.c_str());
      Docu   = AppendItem(Folder,(pDoc->p_DSN+i)->Dsn ,TreeIc_DsnClosed,TreeIc_DsnOpen, new TreeData(Temp0));
    }
  //----------------------------------------------------------------------------------------------------------------------------
  popupMenu1 = NULL;
  popupMenu1 = new wxMenu("");
  popupMenu1->Append(PGMCTRL_ODBC_USER, _("Set Username and Password"));
  // popupMenu1->AppendSeparator();
  //----------------------------------------------------------------------------------------------------------------------------
  Expand(Root);
  Expand(Folder);
  //----------------------------------------------------------------------------------------------------------------------------
  return 0;
}
//---------------------------------------------------------------------------
void PgmCtrl::OnSelChanged(wxTreeEvent& WXUNUSED(event))
{
  int i;
  Temp0.Empty();   Temp1.Empty();
  pDoc->p_MainFrame->SetStatusText(Temp0,0);
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  TreeData *item = (TreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      int Treffer = 0;
      Temp1.Printf("%s",item->m_desc.c_str());
      //-----------------------------------------------------------------------------------------
      if (Temp1 == "Language English")
	{
	  Temp0 = "std";
	  p_ProgramCfg->Write("/Local/langid",Temp0);
	  Temp0 = _("-I-> After a programm restart, the language will be changed to English.");
	  wxMessageBox(Temp0);
	}
      if (Temp1 == "Language German")
	{
	  Temp0 = "de";
	  p_ProgramCfg->Write("/Local/langid",Temp0);
	  Temp0 = _("-I-> After a programm restart, the language will be changed to German.");
	  wxMessageBox(Temp0);
	}
      //-----------------------------------------------------------------------------------------
      if (Temp1 == "wxConfigBase Delete")
	{
	  if (p_ProgramCfg->DeleteAll()) // Default Diretory for wxFileSelector
	    Temp0 = _("-I-> wxConfigBase.p_ProgramCfg->DeleteAll() was succesfull.");
	  else
	    Temp0 = _("-E-> wxConfigBase.p_ProgramCfg->DeleteAll() was not succesfull !");
	  wxBell();     // Ding_a_Ling
	  Treffer++;
	}
      //-----------------------------------------------------------------------------------------
      if (Temp1.Contains("ODBC-"))
	{
	  Temp1 = Temp1.Mid(5,wxSTRING_MAXLEN);
	  for (i=0;i<pDoc->i_DSN;i++)
	    {
	      if (Temp1 == (pDoc->p_DSN+i)->Dsn)
		{
		  pDoc->OnChosenDSN(i);
		}
	    }
	  Treffer++;
	}
      //-----------------------------------------------------------------------------------------
      if (Treffer == 0)
	{
	  //---------------------------------------------------
	  Temp0.Printf(_("Item '%s': %sselected, %sexpanded, %sbold,"
                "%u children (%u immediately under this item)."),
		       item->m_desc.c_str(),
		       bool2String(IsSelected(itemId)),
		       bool2String(IsExpanded(itemId)),
		       bool2String(IsBold(itemId)),
		       GetChildrenCount(itemId),
		       GetChildrenCount(itemId));
	  LogBuf.Printf("-I-> PgmCtrl::OnSelChanged - %s",Temp0.c_str());
	  wxLogMessage( "%s", LogBuf.c_str() );
	  //---------------------------------------------------
	}
    }
}
//---------------------------------------------------------------------------
void PgmCtrl::OnRightSelect(wxTreeEvent& WXUNUSED(event))
{
  int i;
  Temp0.Empty();
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  SaveDSN.Empty();
  if ( item != NULL )
    {
      int Treffer = 0;
      Temp1.Printf("%s",item->m_desc.c_str());
      //-----------------------------------------------------------------------------------------
      if (Temp1.Contains("ODBC-"))
	{
	  Temp1 = Temp1.Mid(5,wxSTRING_MAXLEN);
	  for (i=0;i<pDoc->i_DSN;i++)
	    {
	      if (Temp1 == (pDoc->p_DSN+i)->Dsn)
		{
		  SaveDSN = Temp1;
		  PopupMenu(popupMenu1,TreePos.x,TreePos.y);
		}
	    }
	  Treffer++;
	}
      //-----------------------------------------------------------------------------------------
      if (Treffer == 0)
	{
	  //---------------------------------------------------
	  /*
	    Temp0.Printf(_("Item '%s': %sselected, %sexpanded, %sbold,"
	    "%u children (%u immediately under this item)."),
	    item->m_desc.c_str(),
	    bool2String(IsSelected(itemId)),
	    bool2String(IsExpanded(itemId)),
                bool2String(IsBold(itemId)),
                GetChildrenCount(itemId),
                GetChildrenCount(itemId));
		LogBuf.Printf("-I-> DBTree::OnSelChanged - %s",Temp0.c_str());
		wxLogMessage( "%s", LogBuf.c_str() );
		*/
	  //---------------------------------------------------
	}
    }
}
//------------------------------------------------------------------------------
void PgmCtrl::OnMouseMove(wxMouseEvent &event)
{
  TreePos = event.GetPosition();
}
//------------------------------------------------------------------------------
void PgmCtrl::OnUserPassword()
{
  // wxMessageBox(SaveDSN);
  int i, rc=0;
  //-------------------------------------------
  DlgUser *p_Dlg = new DlgUser(this, "Username and Password");
  //-------------------------------------------
  for (i=0;i<pDoc->i_DSN;i++)
    {
      if (SaveDSN == (pDoc->p_DSN+i)->Dsn)
	{
	  p_Dlg->s_DSN      = (pDoc->p_DSN+i)->Dsn;
	  p_Dlg->s_User     = (pDoc->p_DSN+i)->Usr;
	  p_Dlg->s_Password = (pDoc->p_DSN+i)->Pas;
	  p_Dlg->OnInit();
	  p_Dlg->Fit();
	  //--------------------
	  // Temp0.Printf("i(%d) ; s_DSN(%s) ; s_User(%s) ; s_Password(%s)",i,p_Dlg.s_DSN,p_Dlg.s_User,p_Dlg.s_Password);
	  // wxMessageBox(Temp0);
	  bool OK = FALSE;
	  if (p_Dlg->ShowModal() == wxID_OK)
	    {
	      (pDoc->p_DSN+i)->Usr = p_Dlg->s_User;
	      (pDoc->p_DSN+i)->Pas = p_Dlg->s_Password;
	      (pDoc->db_Br+i)->UserName  = (pDoc->p_DSN+i)->Usr;
	      (pDoc->db_Br+i)->Password  = (pDoc->p_DSN+i)->Pas;
	      OK = TRUE;
	    }
	  delete p_Dlg;
	  if (!OK) return;
	  //--------------------
	  break; // We have what we want, leave
	}
    }
 
  //-------------------------------------------
  SaveDSN.Empty();
}
//---------------------------------------------------------------------------
