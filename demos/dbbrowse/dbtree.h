//---------------------------------------------------------------------------
// Name:        DBTree.h
// Purpose:     Shows ODBC Catalog entries for Databases
// Author:      Mark Johnson
// Modified by: 19991129.mj10777
// Created:     19991129
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
//---------------------------------------------------------------------------
class mjDoc;               // Declared in Doc.h file
//---------------------------------------------------------------------------
class DBTreeData : public wxTreeItemData
{
public:
 DBTreeData(const wxString& desc) : m_desc(desc) { }

 void ShowInfo(wxTreeCtrl *tree);

  wxString m_desc;
};
//---------------------------------------------------------------------------
class DBTree : public wxTreeCtrl
{
 public:
  enum
  { // The order here must be the same as in m_imageListNormal !
   TreeIc_Logo,                     // logo.ico
   TreeIc_DsnClosed,                // dsnclose.ico
   TreeIc_DsnOpen,                  // dsnopen.ico
   TreeIc_TAB,                      // tab.ico
   TreeIc_COL,                      // col.ico
   TreeIc_KEY,                      // key.ico
   TreeIc_KEYF,                     // keyf.ico
   TreeIc_DocClosed,                // d_closed.ico
   TreeIc_DocOpen,                  // d_open.ico
   TreeIc_FolderClosed,             // f_closed.ico
   TreeIc_FolderOpen                // f_open.ico
  };
  wxString Temp0, Temp1, Temp2, Temp3, Temp4, Temp5;
//---------------------------------------------------------------------------
  DBTree() { }
  DBTree(wxWindow *parent);
  DBTree(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size,long style);
  virtual ~DBTree();
  mjDoc *pDoc;
  wxDbInf *ct_BrowserDB;
//---------------------------------------------------------------------------
  int       i_TabArt;   // Tab = 0 ; Page = 1;
  int       i_ViewNr;   // View Nummer in Tab / Page
  int       i_Which;    // Which View, Database is this/using
  wxString  s_DSN;      // Name of the Dataset
  wxMenu   *popupMenu1; // OnDBClass
  wxMenu   *popupMenu2; // OnDBGrid & OnTableclass
  wxPoint TreePos;
//---------------------------------------------------------------------------
  wxImageList *p_imageListNormal;
//---------------------------------------------------------------------------
 public:
  int  OnPopulate();
  void OnSelChanged(wxTreeEvent& event);
  void OnRightSelect(wxTreeEvent& event);
  void OnDBGrid(wxMenu& menu, wxCommandEvent& event);
  void OnDBClass(wxMenu& menu, wxCommandEvent& event);
  void OnTableClass(wxMenu& menu, wxCommandEvent& event);
  void OnTableClassAll(wxMenu& menu, wxCommandEvent& event);
  void OnMouseEvent(wxMouseEvent& event);
  void OnMouseMove(wxMouseEvent& event);
 public:
//---------------------------------------------------------------------------
  // NB: due to an ugly wxMSW hack you _must_ use DECLARE_DYNAMIC_CLASS()
  //     if you want your overloaded OnCompareItems() to be called.
  //     OTOH, if you don't want it you may omit the next line - this will
  //     make default (alphabetical) sorting much faster under wxMSW.
  DECLARE_DYNAMIC_CLASS(DBTree)
  DECLARE_EVENT_TABLE()
};
//------------------------------------------------------------------------------
#define POPUP_01_BEGIN     1100
#define DATA_SHOW          1101
#define DATA_DB            1102
#define DATA_TABLE         1103
#define DATA_TABLE_ALL     1104
#define POPUP_01_END       1117
//------------------------------------------------------------------------------
