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

#define wxDIALOG_EDITOR_VERSION 1.6

#ifdef __GNUG__
#pragma interface "reseditr.h"
#endif

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/layout.h"
#include "wx/resource.h"
#include "wx/tbarsmpl.h"

#include "proplist.h"

#if defined(__WINDOWS__) && defined(__WIN95__)
#include "wx/tbar95.h"
#elif defined(__WINDOWS__)
#include "wx/tbarmsw.h"
#endif

#define RESED_DELETE            1
#define RESED_TOGGLE_TEST_MODE  2
#define RESED_RECREATE          3
#define RESED_CLEAR             4
#define RESED_NEW_DIALOG        5
#define RESED_NEW_PANEL         6

#define RESED_CONTENTS          20

/*
 * Controls loading, saving, user interface of resource editor(s).
 */

class wxResourceEditorFrame;
class EditorToolPalette;
class EditorToolBar;
class wxWindowPropertyInfo;

#ifdef __WINDOWS__
#define wxHelpController wxWinHelpController
#else
#define wxHelpController wxXLPHelpController;
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
  wxResourceTableWithSaving(void):wxResourceTable()
  {
  }
  virtual bool Save(const wxString& filename);
  virtual bool SaveResource(ostream& stream, wxItemResource *item);

  void GenerateWindowStyleString(long windowStyle, char *buf);
  void GeneratePanelStyleString(long windowStyle, char *buf);
  void GenerateDialogStyleString(long windowStyle, char *buf);

  void GenerateRadioBoxStyleString(long windowStyle, char *buf);
  void GenerateMessageStyleString(long windowStyle, char *buf);
  void GenerateTextStyleString(long windowStyle, char *buf);
  void GenerateButtonStyleString(long windowStyle, char *buf);
  void GenerateCheckBoxStyleString(long windowStyle, char *buf);
  void GenerateListBoxStyleString(long windowStyle, char *buf);
  void GenerateSliderStyleString(long windowStyle, char *buf);
  void GenerateGroupBoxStyleString(long windowStyle, char *buf);
  void GenerateGaugeStyleString(long windowStyle, char *buf);
  void GenerateChoiceStyleString(long windowStyle, char *buf);
  void GenerateScrollBarStyleString(long windowStyle, char *buf);
  void GenerateItemStyleString(long windowStyle, char *buf);
  
  bool GenerateStyle(char *buf, long windowStyle, long flag, char *strStyle);

  void OutputFont(ostream& stream, wxFont *font);
  wxControl *CreateItem(wxPanel *panel, wxItemResource *childResource);
};
 
class wxResourceManager: public wxObject
{
 protected:
   wxHelpController *helpInstance;
   wxResourceTableWithSaving resourceTable;
   wxFrame *editorFrame;
   wxPanel *editorPanel;
   wxMenu *popupMenu;
   wxListBox *editorResourceList;
   EditorToolPalette *editorPalette;
   EditorToolBar *editorToolBar;
   int nameCounter;
   bool modified;
   bool editMode;
   wxHashTable resourceAssociations;
   wxList selections;
   wxString currentFilename;
   
 public:
 
   // Options to be saved/restored
   wxString optionsResourceFilename; // e.g. dialoged.ini, .dialogrc
   wxRectangle propertyWindowSize;
   wxRectangle resourceEditorWindowSize;

 public:
   static wxResourceManager *currentResourceManager;

   wxResourceManager(void);
   ~wxResourceManager(void);

   bool Initialize(void);

   bool LoadOptions(void);
   bool SaveOptions(void);

   // Show or hide the resource editor frame, which displays a list
   // of resources with ability to edit them.
   virtual bool ShowResourceEditor(bool show, wxWindow *parent = NULL, const char *title = "wxWindows Dialog Editor");

   virtual bool Save(void);
   virtual bool SaveAs(void);
   virtual bool Save(const wxString& filename);
   virtual bool Load(const wxString& filename);
   virtual bool Clear(bool deleteWindows = TRUE, bool force = TRUE);
   virtual void SetFrameTitle(const wxString& filename);
   virtual bool DisassociateWindows(bool deleteWindows = TRUE);
   virtual bool New(bool loadFromFile = TRUE, const wxString& filename = "");
   virtual bool SaveIfModified(void);
   virtual void AlignItems(int flag);
   virtual void CopySize(void);
   virtual void ToBackOrFront(bool toBack);
   virtual wxWindow *FindParentOfSelection(void);

   virtual wxFrame *OnCreateEditorFrame(const char *title);
   virtual wxMenuBar *OnCreateEditorMenuBar(wxFrame *parent);
   virtual wxPanel *OnCreateEditorPanel(wxFrame *parent);
   virtual wxToolBarBase *OnCreateToolBar(wxFrame *parent);
   virtual EditorToolPalette *OnCreatePalette(wxFrame *parent);
//   virtual bool DeletePalette(void);
   virtual bool InitializeTools(void);

   virtual void UpdateResourceList(void);
   virtual void AddItemsRecursively(int level, wxItemResource *resource);
   virtual bool EditSelectedResource(void);
   virtual bool Edit(wxItemResource *res);
   virtual bool CreateNewDialog(void);
   virtual bool CreateNewPanel(void);
   virtual bool CreatePanelItem(wxItemResource *panelResource, wxPanel *panel, char *itemType, int x = 10, int y = 10, bool isBitmap = FALSE);

   virtual bool DeleteSelection(bool deleteWindow = TRUE);

   virtual bool DeleteResource(wxItemResource *res);
   virtual bool DeleteResource(wxWindow *win, bool deleteWindow = TRUE);

   // Add bitmap resource if there isn't already one with this filename.
   virtual char *AddBitmapResource(char *filename);

   // Delete the bitmap resource if it isn't being used by another resource.
   virtual void PossiblyDeleteBitmapResource(char *resourceName);

   // Helper function for above
   virtual bool IsBitmapResourceUsed(char *resourceName);

   wxItemResource *FindBitmapResourceByFilename(char *filename);

   char *FindBitmapFilenameForResource(wxItemResource *resource);

   // Deletes 'win' and creates a new window from the resource that
   // was associated with it. E.g. if you can't change properties on the
   // fly, you'll need to delete the window and create it again.
   virtual wxWindow *RecreateWindowFromResource(wxWindow *win, wxWindowPropertyInfo *info = NULL);

   virtual bool RecreateSelection(void);

   // Need to search through resource table removing this from
   // any resource which has this as a parent.
   virtual bool RemoveResourceFromParent(wxItemResource *res);

   virtual bool EditDialog(wxDialog *dialog, wxWindow *parent);

   inline void SetEditorFrame(wxFrame *fr) { editorFrame = fr; }
   inline void SetEditorToolBar(EditorToolBar *tb) { editorToolBar = tb; }
   inline void SetEditorPalette(EditorToolPalette *pal) { editorPalette = pal; }
   inline wxFrame *GetEditorFrame(void) { return editorFrame; }
   inline wxListBox *GetEditorResourceList(void) { return editorResourceList; }
   inline EditorToolPalette *GetEditorPalette(void) { return editorPalette; }
   inline wxList& GetSelections(void) { return selections; }
   
   void AddSelection(wxWindow *win);
   void RemoveSelection(wxWindow *win);

//   inline void SetCurrentResource(wxItemResource *item) { currentResource = item; }
//   inline void SetCurrentResourceWindow(wxWindow *win) { currentResourceWindow = win; }
//   inline wxItemResource *GetCurrentResource(void) { return currentResource; }
//   inline wxWindow *GetCurrentResourceWindow(void) { return currentResourceWindow; }
   inline wxMenu *GetPopupMenu(void) { return popupMenu; }
   
   inline wxHelpController *GetHelpInstance(void) { return helpInstance; }

   virtual void MakeUniqueName(char *prefix, char *buf);

   // (Dis)associate resource<->physical window
   virtual void AssociateResource(wxItemResource *resource, wxWindow *win);
   virtual bool DisassociateResource(wxItemResource *resource, bool deleteWindow = TRUE);
   virtual bool DisassociateResource(wxWindow *win, bool deleteWindow = TRUE);
   virtual wxItemResource *FindResourceForWindow(wxWindow *win);
   virtual wxWindow *FindWindowForResource(wxItemResource *resource);
   
   virtual bool InstantiateAllResourcesFromWindows(void);
   virtual bool InstantiateResourceFromWindow(wxItemResource *resource, wxWindow *window, bool recurse = FALSE);

   virtual void Modify(bool mod = TRUE) { modified = mod; }
   virtual bool Modified(void) { return modified; }

   inline bool GetEditMode(void) { return editMode; }
   void SetEditMode(bool flag, bool changeCurrentResource = TRUE);

   inline wxResourceTable& GetResourceTable(void) { return resourceTable; }
   inline wxHashTable& GetResourceAssociations(void) { return resourceAssociations; }
   
   inline wxString& GetCurrentFilename(void) { return currentFilename; }

//   void UsePosition(bool usePos, wxItemResource *resource = NULL, int x = 0, int y = 0);
};

class wxResourceEditorFrame: public wxFrame
{
 public:
  wxResourceManager *manager;
  wxResourceEditorFrame(wxResourceManager *resMan, wxFrame *parent, char *title, int x = -1, int y = -1, int width = 600, int height = 400,
    long style = 0, char *name = "frame");
  ~wxResourceEditorFrame(void);
  void OldOnMenuCommand(int cmd);
  bool OnClose(void);
};

class wxResourceEditorPanel: public wxPanel
{
 public:
  wxResourceEditorPanel(wxWindow *parent, int x = -1, int y = -1, int width = 600, int height = 400,
    long style = 0, char *name = "panel");
  ~wxResourceEditorPanel(void);
  void OnDefaultAction(wxControl *item);
};

class DialogEditorPanelFrame: public wxFrame
{
 public:
  DialogEditorPanelFrame(wxFrame *parent, char *title, int x, int y, int w, int h, long style, char *name):
    wxFrame(parent, -1, title, wxPoint(x, y), wxSize(w, h), style, name)
  {
  }
  bool OnClose(void);
};

// Edit menu
#define OBJECT_EDITOR_NEW_FRAME       220
#define OBJECT_EDITOR_NEW_DIALOG      221
#define OBJECT_EDITOR_NEW_PANEL       222
#define OBJECT_EDITOR_NEW_CANVAS      223
#define OBJECT_EDITOR_NEW_TEXT_WINDOW 224
#define OBJECT_EDITOR_NEW_BUTTON      225
#define OBJECT_EDITOR_NEW_CHECKBOX    226
#define OBJECT_EDITOR_NEW_MESSAGE     227
#define OBJECT_EDITOR_NEW_CHOICE      228
#define OBJECT_EDITOR_NEW_LISTBOX     229
#define OBJECT_EDITOR_NEW_RADIOBOX    230
#define OBJECT_EDITOR_NEW_SLIDER      231
#define OBJECT_EDITOR_NEW_TEXT        232
#define OBJECT_EDITOR_NEW_MULTITEXT   233
#define OBJECT_EDITOR_NEW_GAUGE       234
#define OBJECT_EDITOR_NEW_GROUPBOX    235

#define OBJECT_EDITOR_NEW_ITEM        240
#define OBJECT_EDITOR_NEW_SUBWINDOW   241

#define OBJECT_EDITOR_EDIT_MENU       250
#define OBJECT_EDITOR_EDIT_ATTRIBUTES 251
#define OBJECT_EDITOR_CLOSE_OBJECT    252
#define OBJECT_EDITOR_DELETE_OBJECT   253
#define OBJECT_EDITOR_EDIT_TOOLBAR    254

#define OBJECT_EDITOR_TOGGLE_TEST_MODE 255

#define OBJECT_EDITOR_RC_CONVERT      260
#define OBJECT_EDITOR_RC_CONVERT_MENU 261
#define OBJECT_EDITOR_RC_CONVERT_DIALOG 262

#define OBJECT_EDITOR_GRID            263

#define OBJECT_MENU_EDIT      1
#define OBJECT_MENU_DELETE    2

/*
 * Main toolbar
 *
 */
 
#if defined(__WINDOWS__) && defined(__WIN95__)
class EditorToolBar: public wxToolBar95
#elif defined(__WINDOWS__)
class EditorToolBar: public wxToolBarMSW
#else
class EditorToolBar: public wxToolBarSimple
#endif
{
  public:
  EditorToolBar(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(wxPaintEvent& event);

DECLARE_EVENT_TABLE()
};

// Toolbar ids
#define TOOLBAR_LOAD_FILE       1
#define TOOLBAR_SAVE_FILE       2
#define TOOLBAR_NEW             3
// #define TOOLBAR_GEN_CLIPS       4
#define TOOLBAR_TREE            5
#define TOOLBAR_HELP            6

// Formatting tools
#define TOOLBAR_FORMAT_HORIZ    10
#define TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN    11
#define TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN    12
#define TOOLBAR_FORMAT_VERT     13
#define TOOLBAR_FORMAT_VERT_TOP_ALIGN     14
#define TOOLBAR_FORMAT_VERT_BOT_ALIGN     15

#define TOOLBAR_TO_FRONT        16
#define TOOLBAR_TO_BACK         17
#define TOOLBAR_COPY_SIZE       18

#endif

