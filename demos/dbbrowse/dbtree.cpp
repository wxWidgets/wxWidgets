//---------------------------------------------------------------------------
// Name:        DBTree.h
// Purpose:     Programm Control with a Tree
// Author:      Mark Johnson
// Modified by:
// Created:     19991129
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
BEGIN_EVENT_TABLE(DBTree, wxTreeCtrl)
  EVT_MOTION (DBTree::OnMouseMove)
  EVT_TREE_SEL_CHANGED(TREE_CTRL_DB, DBTree::OnSelChanged)
  EVT_TREE_ITEM_RIGHT_CLICK(TREE_CTRL_DB,DBTree::OnRightSelect)
  EVT_MENU(DATA_SHOW,DBTree::OnDBGrid)
  EVT_MENU(DATA_DB,DBTree::OnDBClass)
  EVT_MENU(DATA_TABLE,DBTree::OnTableClass)
  EVT_MENU(DATA_TABLE_ALL,DBTree::OnTableClassAll)
  END_EVENT_TABLE()
  //---------------------------------------------------------------------------
  // DBTree implementation
  //---------------------------------------------------------------------------
  IMPLEMENT_DYNAMIC_CLASS(DBTree, wxTreeCtrl)
  //---------------------------------------------------------------------------
  DBTree::DBTree(wxWindow *parent)  : wxTreeCtrl(parent)
{
}
DBTree::DBTree(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size, long style)
  : wxTreeCtrl(parent, id, pos, size, style)
{
  //wxFont* ft_Temp = new wxFont(10,wxSWISS,wxNORMAL,wxBOLD,FALSE,"Comic Sans MS");
  wxFont* ft_Temp = new wxFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
  SetFont(* ft_Temp);

  // Make an image list containing small icons
  p_imageListNormal = new wxImageList(16, 16, TRUE);

  // should correspond to TreeIc_xxx enum

#if !defined(__WXMSW__)
#include "bitmaps/logo.xpm"
#include "bitmaps/dsnclose.xpm"
#include "bitmaps/dsnopen.xpm"
#include "bitmaps/tab.xpm"
#include "bitmaps/key.xpm"
#include "bitmaps/keyf.xpm"
#include "bitmaps/d_open.xpm"
#include "bitmaps/d_closed.xpm"
#include "bitmaps/col.xpm"
#endif

   p_imageListNormal->Add(wxICON(Logo));
   p_imageListNormal->Add(wxICON(DsnClosed));
   p_imageListNormal->Add(wxICON(DsnOpen));
   p_imageListNormal->Add(wxICON(TAB));
   p_imageListNormal->Add(wxICON(COL));
   p_imageListNormal->Add(wxICON(KEY));
   p_imageListNormal->Add(wxICON(KEYF));
   p_imageListNormal->Add(wxICON(DocOpen));
   p_imageListNormal->Add(wxICON(DocOpen));





  SetImageList(p_imageListNormal);
  ct_BrowserDB = NULL;
}
//---------------------------------------------------------------------------
DBTree::~DBTree()
{
  // wxLogMessage("DBTree::~DBTree() - Vor  OnCloseDB()");
  (pDoc->db_Br+i_Which)->OnCloseDB(FALSE);
  // wxLogMessage("DBTree::~DBTree() - Nach OnCloseDB()");
  (pDoc->db_Br+i_Which)->db_BrowserDB = NULL;
  (pDoc->db_Br+i_Which)->ct_BrowserDB = NULL;
  (pDoc->db_Br+i_Which)->cl_BrowserDB = NULL;
  delete ct_BrowserDB;
  delete p_imageListNormal;
}
//---------------------------------------------------------------------------
#undef TREE_EVENT_HANDLER
//---------------------------------------------------------------------------
int  DBTree::OnPopulate()
{
 wxTreeItemId Root, Folder, Docu, Funkt;
 int i,x,y, TableType;
 wxString SQL_TYPE, DB_TYPE;
 //----------------------------------------------------------------------------------------------------------------------------
 if ((pDoc->db_Br+i_Which)->Initialize(FALSE))
 {
  wxBeginBusyCursor();
  ct_BrowserDB = (pDoc->db_Br+i_Which)->OnGetCatalog(FALSE);
  if (ct_BrowserDB)
  { // Use the wxDatabase Information
   Temp0.Printf("%s - (%s) (%s)", s_DSN.c_str(),ct_BrowserDB->catalog, ct_BrowserDB->schema);
   Root = AddRoot(Temp0,TreeIc_DsnOpen,TreeIc_DsnOpen,new DBTreeData("Root"));
   for (x=0;x<ct_BrowserDB->numTables;x++)
   {
    wxYield();
    TableType = 0; // TABLE = 1 ; VIEW = 2 ; 0 We are not interested in
    if (!wxStrcmp((ct_BrowserDB->pTableInf+x)->tableType,"TABLE"))    // only TABLES
     TableType = 1;
    if (!wxStrcmp((ct_BrowserDB->pTableInf+x)->tableType,"VIEW"))     // and  VIEWS
     TableType = 2;
    if (TableType)    // only TABLES or Views
    {
     Temp1.Printf("TN(%s",(ct_BrowserDB->pTableInf+x)->tableName);
     //----
     (ct_BrowserDB->pTableInf+x)->pColInf = (pDoc->db_Br+i_Which)->OnGetColumns((ct_BrowserDB->pTableInf+x)->tableName,(ct_BrowserDB->pTableInf+x)->numCols,FALSE);
     //----
     if ((ct_BrowserDB->pTableInf+x)->pColInf)
     {
      if (TableType == 1)  // Table
      {
       Temp0.Printf(_("Table-Name(%s) with (%d)Columns ; Remarks(%s)"),  (ct_BrowserDB->pTableInf+x)->tableName,
                              (ct_BrowserDB->pTableInf+x)->numCols,(ct_BrowserDB->pTableInf+x)->tableRemarks);
       Folder = AppendItem(Root,Temp0,TreeIc_TAB,TreeIc_TAB, new DBTreeData(Temp1));
      }
      if (TableType == 2)  // View
      {
       Temp0.Printf(_("View-Name(%s) with (%d)Columns ; Remarks(%s)"),  (ct_BrowserDB->pTableInf+x)->tableName,
                              (ct_BrowserDB->pTableInf+x)->numCols,(ct_BrowserDB->pTableInf+x)->tableRemarks);
       Folder = AppendItem(Root,Temp0,TreeIc_VIEW,TreeIc_VIEW, new DBTreeData(Temp1));
      }
      for (y=0;y<(ct_BrowserDB->pTableInf+x)->numCols;y++)
      {
       Temp1.Printf("FN(%s",((ct_BrowserDB->pTableInf+x)->pColInf+y)->colName);
       // Here is where we find out if the Column is a Primary / Foreign Key
       if (((ct_BrowserDB->pTableInf+x)->pColInf+y)->PkCol != 0)  // Primary Key
       {
        Docu = AppendItem(Folder,((ct_BrowserDB->pTableInf+x)->pColInf+y)->colName,TreeIc_KEY,TreeIc_KEY,new DBTreeData(Temp1));
        Temp2.Printf(_("This Key is used in the following Tables : %s"),((ct_BrowserDB->pTableInf+x)->pColInf+y)->PkTableName);
        Funkt = AppendItem(Docu,Temp2,TreeIc_DocClosed,TreeIc_DocOpen,new DBTreeData("KEY"));
       }
       else
       {
        if (((ct_BrowserDB->pTableInf+x)->pColInf+y)->FkCol != 0) // Foreign Key
        {
         Docu = AppendItem(Folder,((ct_BrowserDB->pTableInf+x)->pColInf+y)->colName,TreeIc_KEYF,TreeIc_KEYF,new DBTreeData(Temp1));
         Temp2.Printf(_("This Foreign Key comes from the following Table : %s"),((ct_BrowserDB->pTableInf+x)->pColInf+y)->FkTableName);
         Funkt = AppendItem(Docu,Temp2,TreeIc_DocClosed,TreeIc_DocOpen,new DBTreeData("KEYF"));
        }
        else
         Docu = AppendItem(Folder,((ct_BrowserDB->pTableInf+x)->pColInf+y)->colName,TreeIc_COL,TreeIc_COL,new DBTreeData(Temp1));
       }
       SQL_TYPE.Printf(_("SQL_C_???? (%d)"),((ct_BrowserDB->pTableInf+x)->pColInf+y)->sqlDataType);
       DB_TYPE.Printf(_("DB_DATA_TYPE_???? (%d)"),((ct_BrowserDB->pTableInf+x)->pColInf+y)->dbDataType);
       for (i=1;i<=(pDoc->db_Br+i_Which)->i_SqlTyp[0];i++)
       {
        if (((ct_BrowserDB->pTableInf+x)->pColInf+y)->sqlDataType == (pDoc->db_Br+i_Which)->i_SqlTyp[i])
        {
         SQL_TYPE.Printf("%s(%d) ; ",(pDoc->db_Br+i_Which)->s_SqlTyp[i].c_str(),(pDoc->db_Br+i_Which)->i_SqlTyp[i]);
        }
       } // for (i=1;i<=i_SqlTyp[0];i++)
       for (i=1;i<=(pDoc->db_Br+i_Which)->i_dbTyp[0];i++)
       {
        if (((ct_BrowserDB->pTableInf+x)->pColInf+y)->dbDataType == (pDoc->db_Br+i_Which)->i_dbTyp[i])
        {
         DB_TYPE.Printf("%s(%d)",(pDoc->db_Br+i_Which)->s_dbTyp[i].c_str(),(pDoc->db_Br+i_Which)->i_dbTyp[i]);
        }
       } // for (i=1;i<=i_dbTyp[0];i++)
       SQL_TYPE += DB_TYPE;
       Funkt = AppendItem(Docu,SQL_TYPE,TreeIc_DocClosed,TreeIc_DocOpen,new DBTreeData(SQL_TYPE));
       SQL_TYPE.Printf("%10s %d,%d",((ct_BrowserDB->pTableInf+x)->pColInf+y)->typeName,
        ((ct_BrowserDB->pTableInf+x)->pColInf+y)->columnSize,((ct_BrowserDB->pTableInf+x)->pColInf+y)->decimalDigits);
       Funkt = AppendItem(Docu,SQL_TYPE,TreeIc_DocClosed,TreeIc_DocOpen,new DBTreeData(SQL_TYPE));
      }  // for (y=0;y<(ct_BrowserDB->pTableInf+x)->numCols;y++)
     }   // if ((ct_BrowserDB->pTableInf+x)->pColInf)
     else
      Folder = AppendItem(Root,Temp0,TreeIc_FolderClosed,TreeIc_FolderOpen, new DBTreeData(Temp1));
    }    // if ((ct_BrowserDB->pTableInf+x)->tableType == "TABLE" or VIEW)
    // else
    //  wxLogMessage(_("\n-I-> if ! TABLE or VIEW  >%s<"),(ct_BrowserDB->pTableInf+x)->tableType);
   }     // for (x=0;x<ct_BrowserDB->numTables;x++)
  }      // if (ct_BrowserDB)
  else
   wxLogMessage(_("\n-E-> DBTree::OnPopulate() : Invalid Catalog Pointer : Failed"));
  wxEndBusyCursor();
 }       // if((pDoc->db_Br+i_Which)->Initialize(FALSE))
 else
 {
  wxLogMessage(_("\n-E-> DBTree::OnPopulate() : A valid Pointer could not be created : Failed"));
  return 0;
 }
 //----------------------------------------------------------------------------------------------------------------------------
 Expand(Root);
 //----------------------------------------------------------------------------------------------------------------------------
 popupMenu1 = NULL;
 popupMenu1 = new wxMenu("");
 popupMenu1->Append(DATA_DB, _("Make wxDB.cpp/h "));
 popupMenu1->AppendSeparator();
 popupMenu1->Append(DATA_TABLE_ALL, _("Make all wxTable.cpp/h classes"));
 popupMenu2 = NULL;
 popupMenu2 = new wxMenu("");
 popupMenu2->Append(DATA_SHOW, _("Show Data"));
 popupMenu2->AppendSeparator();
 popupMenu2->Append(DATA_TABLE, _("Make wxTable.cpp/h "));
 //----------------------------------------------------------------------------------------------------------------------------

 return 0;
}
//---------------------------------------------------------------------------
void DBTree::OnSelChanged(wxTreeEvent& WXUNUSED(event))
{
  int i;
  Temp0.Empty();
  pDoc->p_MainFrame->SetStatusText(Temp0,0);
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
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
		  // pDoc->OnChosenDSN(i);
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
//---------------------------------------------------------------------------
void DBTree::OnRightSelect(wxTreeEvent& WXUNUSED(event))
{
  int i;
  Temp0.Empty();
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      int Treffer = 0;
      Temp1.Printf("%s",item->m_desc.c_str());
      //-----------------------------------------------------------------------------------------
      if (!wxStrcmp("Root",Temp1))
	{
	  PopupMenu(popupMenu1,TreePos.x,TreePos.y);
	  Treffer++;
	}
      for (i=0;i<ct_BrowserDB->numTables;i++)
	{
	  Temp2.Printf("TN(%s",(ct_BrowserDB->pTableInf+i)->tableName);
	  if (!wxStrcmp(Temp2,Temp1))
	    {
	      PopupMenu(popupMenu2,TreePos.x,TreePos.y);
	      Treffer++;
	    }
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
//---------------------------------------------------------------------------
void DBTree::OnDBGrid(wxMenu& , wxCommandEvent& event)
{
  int i;
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      Temp1.Printf("%s",item->m_desc.c_str());
      for (i=0;i<ct_BrowserDB->numTables;i++)
	{
	  Temp2.Printf("TN(%s",(ct_BrowserDB->pTableInf+i)->tableName);
	  if (!wxStrcmp(Temp2,Temp1))
	    {
	      // Temp0.Printf("(%d) Here is where a GridCtrl for >%s< will be called! ",i,(ct_BrowserDB->pTableInf+i)->tableName);
	      pDoc->OnChosenTbl(1,(ct_BrowserDB->pTableInf+i)->tableName);
	      // wxMessageBox(Temp0);
	    }
	}
    }
}
//---------------------------------------------------------------------------
void DBTree::OnDBClass(wxMenu& , wxCommandEvent& event)
{
  // int i;
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      Temp0.Printf(_("Here is where a wxDB Class for >%s< will be made! "),s_DSN.c_str());
      wxMessageBox(Temp0);
    }
}
//---------------------------------------------------------------------------
void DBTree::OnTableClass(wxMenu& , wxCommandEvent& event)
{
  int i;
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      Temp1.Printf("%s",item->m_desc.c_str());
      for (i=0;i<ct_BrowserDB->numTables;i++)
	{
	  Temp2.Printf("TN(%s",(ct_BrowserDB->pTableInf+i)->tableName);
	  if (!wxStrcmp(Temp2,Temp1))
	    {
	      Temp0.Printf(_("(%d) Here is where a wxTable Class for >%s< will be made! "),i,(ct_BrowserDB->pTableInf+i)->tableName);
	      wxMessageBox(Temp0);
	    }
	}
    }
}
//---------------------------------------------------------------------------
void DBTree::OnTableClassAll(wxMenu& , wxCommandEvent& event)
{
  // int i;
  // Get the Information that we need
  wxTreeItemId itemId = GetSelection();
  DBTreeData *item = (DBTreeData *)GetItemData(itemId);
  if ( item != NULL )
    {
      Temp0.Printf(_("Here is where all wxTable Classes in >%s< will be made! "),s_DSN.c_str());
      wxMessageBox(Temp0);
    }
}
//------------------------------------------------------------------------------
void DBTree::OnMouseMove(wxMouseEvent &event)
{
  TreePos = event.GetPosition();
}
//------------------------------------------------------------------------------


