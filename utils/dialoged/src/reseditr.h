/////////////////////////////////////////////////////////////////////////////
// Name:        reseditr.h
// Purpose:     Resource editor class
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _RESEDITR_H_
#define _RESEDITR_H_

#define wxDIALOG_EDITOR_VERSION 1.7

#ifdef __GNUG__
#pragma interface "reseditr.h"
#endif

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/layout.h"
#include "wx/resource.h"
#include "wx/toolbar.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/proplist.h"
#include "wx/txtstrm.h"
#include "symbtabl.h"
#include "winstyle.h"

#define RESED_DELETE            1
#define RESED_RECREATE          3
#define RESED_CLEAR             4
#define RESED_NEW_DIALOG        5
#define RESED_NEW_PANEL         6
#define RESED_TEST              10

#define RESED_CONTENTS          20

#define IDC_TREECTRL            100
#define IDC_LISTCTRL            101

// For control list ('palette')
#define RESED_POINTER           0
#define RESED_BUTTON            1
#define RESED_BMPBUTTON         2
#define RESED_STATICTEXT        3
#define RESED_STATICBMP         4
#define RESED_STATICBOX         5
#define RESED_TEXTCTRL_SINGLE   6
#define RESED_TEXTCTRL_MULTIPLE 7
#define RESED_LISTBOX           8
#define RESED_CHOICE            9
#define RESED_COMBOBOX          10
#define RESED_CHECKBOX          11
#define RESED_SLIDER            12
#define RESED_GAUGE             13
#define RESED_RADIOBOX          14
#define RESED_RADIOBUTTON       15
#define RESED_SCROLLBAR         16
#define RESED_TREECTRL          17
#define RESED_LISTCTRL          18
#define RESED_SPINBUTTON        19

/*
 * Controls loading, saving, user interface of resource editor(s).
 */

class wxResourceEditorFrame;
class EditorToolBar;
class wxWindowPropertyInfo;
class wxResourceEditorProjectTree;
class wxResourceEditorControlList;

#ifdef __WXMSW__
#define wxHelpController wxWinHelpController
#else
#define wxHelpController wxHTMLHelpController;
#endif

class wxHelpController;

/*
 * The resourceTable contains a list of wxItemResources (which each may
 * have further children, defining e.g. a dialog box with controls).
 *
 * We need to associate actual windows with each wxItemResource,
 * instead of the current 'one current window' scheme.
 *
 *  - We create a new dialog, create a wxItemResource,
 *    associate the dialog with wxItemResource via a hash table.
 *    Must be a hash table in case dialog is deleted without
 *    telling the resource manager.
 *  - When we save the resource after editing/closing the dialog,
 *    we check the wxItemResource/wxDialog and children for
 *    consistency (throw away items no longer in the wxDialog,
 *    create any new wxItemResources).
 *  - We save the wxItemResources via the wxPropertyInfo classes,
 *    so devolve the code to the appropriate class.
 *    This involves creating a new temporary wxPropertyInfo for
 *    the purpose.
 *
 * We currently assume we only create one instance of a window for
 * each wxItemResource. We will need to relax this when we're editing
 * in situ.
 *
 *
 */
 
class wxResourceTableWithSaving: public wxResourceTable
{
 public:
  wxResourceTableWithSaving():wxResourceTable()
  {
    // Add all known window styles
    m_styleTable.Init();
  }
  virtual bool Save(const wxString& filename);
  virtual bool SaveResource(wxTextOutputStream& stream, wxItemResource* item, wxItemResource* parentItem);

  void GeneratePanelStyleString(long windowStyle, char *buf);
  void GenerateDialogStyleString(long windowStyle, char *buf);

/*
  void GenerateRadioBoxStyleString(long windowStyle, char *buf);
  void GenerateMessageStyleString(long windowStyle, char *buf);
  void GenerateTextStyleString(long windowStyle, char *buf);
  void GenerateButtonStyleString(long windowStyle, char *buf);
  void GenerateCheckBoxStyleString(long windowStyle, char *buf);
  void GenerateRadioButtonStyleString(long windowStyle, char *buf);
  void GenerateListBoxStyleString(long windowStyle, char *buf);
  void GenerateSliderStyleString(long windowStyle, char *buf);
  void GenerateGroupBoxStyleString(long windowStyle, char *buf);
  void GenerateGaugeStyleString(long windowStyle, char *buf);
  void GenerateChoiceStyleString(long windowStyle, char *buf);
  void GenerateComboBoxStyleString(long windowStyle, char *buf);
  void GenerateScrollBarStyleString(long windowStyle, char *buf);
*/

  void GenerateControlStyleString(const wxString& windowClass, long windowStyle, char *buf);

/*
  bool GenerateStyle(char *buf, long windowStyle, long flag, char *strStyle);
*/

  void OutputFont(wxTextOutputStream& stream, const wxFont& font);
  wxControl *CreateItem(wxPanel *panel, const wxItemResource *childResource, const wxItemResource* parentResource);

protected:
  wxWindowStyleTable    m_styleTable;
};
 
class wxResourceEditorScrolledWindow;

class wxResourceManager: public wxObject
{
   friend class wxResourceEditorFrame;

public:
   wxResourceManager();
   ~wxResourceManager();

// Operations

   // Initializes the resource manager
   bool Initialize();

   // Load/save window size etc.
   bool LoadOptions();
   bool SaveOptions();

   // Show or hide the resource editor frame, which displays a list
   // of resources with ability to edit them.
   virtual bool ShowResourceEditor(bool show, wxWindow *parent = NULL, const char *title = "wxWindows Dialog Editor");

   virtual bool Save();
   virtual bool SaveAs();
   virtual bool Save(const wxString& filename);
   virtual bool Load(const wxString& filename);
   virtual bool Clear(bool deleteWindows = TRUE, bool force = TRUE);
   virtual void SetFrameTitle(const wxString& filename);
   virtual void ClearCurrentDialog();
   virtual bool New(bool loadFromFile = TRUE, const wxString& filename = "");
   virtual bool SaveIfModified();
   virtual void AlignItems(int flag);
   virtual void CopySize();
   virtual void ToBackOrFront(bool toBack);
   virtual wxWindow *FindParentOfSelection();

   virtual wxFrame *OnCreateEditorFrame(const char *title);
   virtual wxMenuBar *OnCreateEditorMenuBar(wxFrame *parent);
   virtual wxResourceEditorScrolledWindow *OnCreateEditorPanel(wxFrame *parent);
   virtual wxToolBar *OnCreateToolBar(wxFrame *parent);

   // Create a window information object for the give window
   wxWindowPropertyInfo* CreatePropertyInfoForWindow(wxWindow *win);
   // Edit the given window
   void EditWindow(wxWindow *win);

   virtual void UpdateResourceList();
   virtual void AddItemsRecursively(long parent, wxItemResource *resource);
   virtual bool EditSelectedResource();
   virtual bool Edit(wxItemResource *res);
   virtual bool CreateNewPanel();
   virtual bool CreatePanelItem(wxItemResource *panelResource, wxPanel *panel, char *itemType, int x = 10, int y = 10, bool isBitmap = FALSE);
   virtual bool DeleteSelection();
   virtual bool TestCurrentDialog(wxWindow* parent);

   // Saves the window info into the resource, and deletes the
   // handler. Doesn't actually disassociate the window from
   // the resources. Replaces OnClose.
   virtual bool SaveInfoAndDeleteHandler(wxWindow* win);

   // Destroys the window. If this is the 'current' panel, NULLs the
   // variable.
   virtual bool DeleteWindow(wxWindow* win);
   virtual bool DeleteResource(wxItemResource *res);
   virtual bool DeleteResource(wxWindow *win);

   // Add bitmap resource if there isn't already one with this filename.
   virtual wxString AddBitmapResource(const wxString& filename);

   // Delete the bitmap resource if it isn't being used by another resource.
   virtual void PossiblyDeleteBitmapResource(const wxString& resourceName);

   // Helper function for above
   virtual bool IsBitmapResourceUsed(const wxString& resourceName);

   wxItemResource *FindBitmapResourceByFilename(const wxString& filename);

   wxString FindBitmapFilenameForResource(wxItemResource *resource);

   // Is this window identifier in use?
   bool IsSymbolUsed(wxItemResource* thisResource, wxWindowID id) ;

   // Is this window identifier compatible with the given name? (i.e.
   // does it already exist under a different name)
   bool IsIdentifierOK(const wxString& name, wxWindowID id);

   // Change all integer ids that match oldId, to newId.
   // This is necessary if an id is changed for one resource - all resources
   // must be changed.
   void ChangeIds(int oldId, int newId);

   // If any resource ids were missing (or their symbol was missing),
   // repair them i.e. give them new ids. Returns TRUE if any resource
   // needed repairing.
   bool RepairResourceIds();

   // Deletes 'win' and creates a new window from the resource that
   // was associated with it. E.g. if you can't change properties on the
   // fly, you'll need to delete the window and create it again.
   virtual wxWindow *RecreateWindowFromResource(wxWindow *win, wxWindowPropertyInfo *info = NULL);

   virtual bool RecreateSelection();

   // Need to search through resource table removing this from
   // any resource which has this as a parent.
   virtual bool RemoveResourceFromParent(wxItemResource *res);

   virtual bool EditDialog(wxDialog *dialog, wxWindow *parent);

   void AddSelection(wxWindow *win);
   void RemoveSelection(wxWindow *win);

   virtual void MakeUniqueName(char *prefix, char *buf);

   // (Dis)associate resource<->physical window
   // Doesn't delete any windows.
   virtual void AssociateResource(wxItemResource *resource, wxWindow *win);
   virtual bool DisassociateResource(wxItemResource *resource);
   virtual bool DisassociateResource(wxWindow *win);
   virtual bool DisassociateWindows();
   virtual wxItemResource *FindResourceForWindow(wxWindow *win);
   virtual wxWindow *FindWindowForResource(wxItemResource *resource);
   
   virtual bool InstantiateAllResourcesFromWindows();
   virtual bool InstantiateResourceFromWindow(wxItemResource *resource, wxWindow *window, bool recurse = FALSE);

// Accessors
   inline void SetEditorFrame(wxFrame *fr) { m_editorFrame = fr; }
   inline void SetEditorToolBar(EditorToolBar *tb) { m_editorToolBar = tb; }
   inline wxFrame *GetEditorFrame() const { return m_editorFrame; }
   inline wxResourceEditorProjectTree *GetEditorResourceTree() const { return m_editorResourceTree; }
   inline wxResourceEditorControlList *GetEditorControlList() const { return m_editorControlList; }
   inline wxList& GetSelections() { return m_selections; }
   inline wxMenu *GetPopupMenu() const { return m_popupMenu; }
#ifdef __WXMSW__
   inline wxHelpController *GetHelpController() const { return m_helpController; }
#endif

   inline void Modify(bool mod = TRUE) { m_modified = mod; }
   inline bool Modified() const { return m_modified; }

   inline wxResourceTable& GetResourceTable() { return m_resourceTable; }
   inline wxHashTable& GetResourceAssociations() { return m_resourceAssociations; }

   inline wxString GetCurrentFilename() const { return m_currentFilename; }
   static wxResourceManager* GetCurrentResourceManager() { return sm_currentResourceManager; }

   inline void SetSymbolFilename(const wxString& s) { m_symbolFilename = s; }
   inline wxString GetSymbolFilename() const { return m_symbolFilename; }

   inline wxRect& GetPropertyWindowSize() { return m_propertyWindowSize; }
   inline wxRect& GetResourceEditorWindowSize() { return m_resourceEditorWindowSize; }

   wxResourceSymbolTable& GetSymbolTable() { return m_symbolTable; }

   // Generate a window id and a first stab at a name
   int GenerateWindowId(const wxString& prefix, wxString& idName) ;

// Member variables
 protected:
#ifdef __WXMSW__
   wxHelpController*                m_helpController;
#endif
   wxResourceTableWithSaving        m_resourceTable;
   wxFrame*                         m_editorFrame;
   wxResourceEditorScrolledWindow*  m_editorPanel;
   wxMenu*                          m_popupMenu;
   wxResourceEditorProjectTree*     m_editorResourceTree;
   wxResourceEditorControlList*     m_editorControlList;
   EditorToolBar*                   m_editorToolBar;
   int                              m_nameCounter;
   int                              m_symbolIdCounter; // For generating window ids
   bool                             m_modified;
   wxHashTable                      m_resourceAssociations;
   wxList                           m_selections;
   wxString                         m_currentFilename;
   wxBitmap*                        m_bitmapImage; // Default for static bitmaps/buttons

   wxImageList                      m_imageList;
   long                             m_rootDialogItem; // Root of dialog hierarchy in tree (unused)

   // Options to be saved/restored
   wxString                         m_optionsResourceFilename; // e.g. dialoged.ini, .dialogrc
   wxRect                           m_propertyWindowSize;
   wxRect                           m_resourceEditorWindowSize;
   static wxResourceManager*        sm_currentResourceManager;

   // Symbol table with identifiers for controls
   wxResourceSymbolTable            m_symbolTable;
   // Filename for include file, e.g. resource.h
   wxString                         m_symbolFilename;
};


class wxResourceEditorFrame: public wxFrame
{
 public:
  DECLARE_CLASS(wxResourceEditorFrame)

  wxResourceManager *manager;
  wxResourceEditorFrame(wxResourceManager *resMan, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 400),
    long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame");
  ~wxResourceEditorFrame();

  void OnCloseWindow(wxCloseEvent& event);

  void OnNew(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnNewDialog(wxCommandEvent& event);
  void OnClear(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnContents(wxCommandEvent& event);
  void OnDeleteSelection(wxCommandEvent& event);
  void OnRecreateSelection(wxCommandEvent& event);
  void OnTest(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

class wxResourceEditorScrolledWindow: public wxScrolledWindow
{
 public:
  wxResourceEditorScrolledWindow(wxWindow *parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = 0);
  ~wxResourceEditorScrolledWindow();

  void OnPaint(wxPaintEvent& event);

  void DrawTitle(wxDC& dc);

// Accessors
  inline int GetMarginX() { return m_marginX; }
  inline int GetMarginY() { return m_marginY; }

 public:
  wxWindow* m_childWindow;
 private:
  int m_marginX, m_marginY;

 DECLARE_EVENT_TABLE()
};

#define OBJECT_MENU_EDIT      1
#define OBJECT_MENU_DELETE    2

/*
 * Main toolbar
 *
 */
 
class EditorToolBar: public wxToolBar
{
public:
  EditorToolBar(wxFrame *frame, const wxPoint& pos = wxPoint(0, 0), const wxSize& size = wxSize(0, 0),
            long style = wxTB_HORIZONTAL);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);

DECLARE_EVENT_TABLE()
};

// Toolbar ids
#define TOOLBAR_LOAD_FILE                   1
#define TOOLBAR_SAVE_FILE                   2
#define TOOLBAR_NEW                         3
#define TOOLBAR_TREE                        5
#define TOOLBAR_HELP                        6

// Formatting tools
#define TOOLBAR_FORMAT_HORIZ                10
#define TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN     11
#define TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN    12
#define TOOLBAR_FORMAT_VERT                 13
#define TOOLBAR_FORMAT_VERT_TOP_ALIGN       14
#define TOOLBAR_FORMAT_VERT_BOT_ALIGN       15

#define TOOLBAR_TO_FRONT                    16
#define TOOLBAR_TO_BACK                     17
#define TOOLBAR_COPY_SIZE                   18

/*
 * this class is used to store data associated with a tree item
 */
class wxResourceTreeData : public wxTreeItemData
{
public:
    wxResourceTreeData(wxItemResource *resource) { m_resource = resource; }

    wxItemResource *GetResource() const { return m_resource; }

private:
    wxItemResource *m_resource;
};

#endif

