/////////////////////////////////////////////////////////////////////////////
// Name:        treetest.h
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application type
class MyApp : public wxApp
{
public:
  bool OnInit();
};

class MyTreeItemData : public wxTreeItemData
{
public:
  MyTreeItemData(const wxString& desc) : m_desc(desc) { }

  void ShowInfo(wxTreeCtrl *tree);

private:
  wxString m_desc;
};

class MyTreeCtrl : public wxTreeCtrl
{
public:
  enum
  {
    TreeCtrlIcon_File,
    TreeCtrlIcon_Folder
  };

  MyTreeCtrl(wxWindow *parent, const wxWindowID id,
             const wxPoint& pos, const wxSize& size,
             long style);
  virtual ~MyTreeCtrl();

  void OnBeginDrag(wxTreeEvent& event);
  void OnBeginRDrag(wxTreeEvent& event);
  void OnBeginLabelEdit(wxTreeEvent& event);
  void OnEndLabelEdit(wxTreeEvent& event);
  void OnDeleteItem(wxTreeEvent& event);
  void OnGetInfo(wxTreeEvent& event);
  void OnSetInfo(wxTreeEvent& event);
  void OnItemExpanded(wxTreeEvent& event);
  void OnItemExpanding(wxTreeEvent& event);
  void OnItemCollapsed(wxTreeEvent& event);
  void OnItemCollapsing(wxTreeEvent& event);
  void OnSelChanged(wxTreeEvent& event);
  void OnSelChanging(wxTreeEvent& event);
  void OnKeyDown(wxTreeEvent& event);

  void GetItemsRecursively(const wxTreeItemId& idParent, long cookie);

  void AddTestItemsToTree(size_t numChildren,
                          size_t depth);

private:
  void AddItemsRecursively(const wxTreeItemId& idParent,
                           size_t nChildren,
                           size_t depth,
                           size_t folder);

  wxImageList *m_imageListNormal;

  DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
  // ctor and dtor
  MyFrame(const wxString& title, int x, int y, int w, int h);
  virtual ~MyFrame();

  // menu callbacks
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnDump(wxCommandEvent& event);
  void OnDelete(wxCommandEvent& event);
  void OnDeleteAll(wxCommandEvent& event);
  void OnRecreate(wxCommandEvent& event);

  void OnSetBold(wxCommandEvent& event) { DoSetBold(TRUE); }
  void OnClearBold(wxCommandEvent& event) { DoSetBold(FALSE); }

private:
  MyTreeCtrl *m_treeCtrl;

  void DoSetBold(bool bold = TRUE);

  DECLARE_EVENT_TABLE()
};

// menu and control ids
enum
{
  TreeTest_Quit,
  TreeTest_About,
  TreeTest_Dump,
  TreeTest_Bold,
  TreeTest_UnBold,
  TreeTest_Delete,
  TreeTest_DeleteAll,
  TreeTest_Recreate,
  TreeTest_Ctrl = 100
};

enum
{
  TreeCtrlIcon_File,
  TreeCtrlIcon_Folder
};
