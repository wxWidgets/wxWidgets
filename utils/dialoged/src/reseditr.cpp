/////////////////////////////////////////////////////////////////////////////
// Name:        reseditr.cpp
// Purpose:     Resource editor class
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "reseditr.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/choice.h"
#include "wx/listbox.h"
#include "wx/radiobox.h"
#include "wx/statbox.h"
#include "wx/gauge.h"
#include "wx/slider.h"
#include "wx/textctrl.h"
#endif

#include "wx/scrolbar.h"

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WINDOWS__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#ifdef __WINDOWS__
#include <windows.h>
#endif

#include "wx/help.h"

#include "reseditr.h"
#include "winprop.h"
#include "editrpal.h"
#include "dlghndlr.h"

static void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event);
void wxResourceEditWindow(wxWindow *win);
wxWindowPropertyInfo *wxCreatePropertyInfoForWindow(wxWindow *win);
wxResourceManager *wxResourceManager::currentResourceManager = NULL;

// Bitmaps for toolbar
wxBitmap *ToolbarLoadBitmap = NULL;
wxBitmap *ToolbarSaveBitmap = NULL;
wxBitmap *ToolbarNewBitmap = NULL;
wxBitmap *ToolbarVertBitmap = NULL;
wxBitmap *ToolbarAlignTBitmap = NULL;
wxBitmap *ToolbarAlignBBitmap = NULL;
wxBitmap *ToolbarHorizBitmap = NULL;
wxBitmap *ToolbarAlignLBitmap = NULL;
wxBitmap *ToolbarAlignRBitmap = NULL;
wxBitmap *ToolbarCopySizeBitmap = NULL;
wxBitmap *ToolbarToFrontBitmap = NULL;
wxBitmap *ToolbarToBackBitmap = NULL;
wxBitmap *ToolbarHelpBitmap = NULL;

wxBitmap *wxWinBitmap = NULL;

#ifdef __X__
#include "bitmaps/load.xbm"
#include "bitmaps/save.xbm"
#include "bitmaps/new.xbm"
#include "bitmaps/vert.xbm"
#include "bitmaps/alignt.xbm"
#include "bitmaps/alignb.xbm"
#include "bitmaps/horiz.xbm"
#include "bitmaps/alignl.xbm"
#include "bitmaps/alignr.xbm"
#include "bitmaps/copysize.xbm"
#include "bitmaps/tofront.xbm"
#include "bitmaps/toback.xbm"
#include "bitmaps/help.xbm"
#include "bitmaps/wxwin.xbm"
#endif

/*
 * Resource manager
 */


wxResourceManager::wxResourceManager(void)
{
  currentResourceManager = this;
  editorFrame = NULL;
  editorPanel = NULL;
  popupMenu = NULL;
  editorResourceList = NULL;
  editorPalette = NULL;
  nameCounter = 1;
  modified = FALSE;
  currentFilename = "";
  editMode = TRUE;
  editorToolBar = NULL;
  
  // Default window positions
  resourceEditorWindowSize.width = 470;
  resourceEditorWindowSize.height = 300;

  resourceEditorWindowSize.x = 0;
  resourceEditorWindowSize.y = 0;
  
  propertyWindowSize.width = 300;
  propertyWindowSize.height = 300;

  helpInstance = NULL;
}

wxResourceManager::~wxResourceManager(void)
{
  currentResourceManager = NULL;
  SaveOptions();

  helpInstance->Quit();
  delete helpInstance;
  helpInstance = NULL;
}

bool wxResourceManager::Initialize(void)
{
  // Set up the resource filename for each platform.
#ifdef __WINDOWS__
  // dialoged.ini in the Windows directory
  char buf[256];
  GetWindowsDirectory(buf, 256);
  strcat(buf, "\\dialoged.ini");
  optionsResourceFilename = buf;
#elif defined(__X__)
  char buf[500];
  (void)wxGetHomeDir(buf);
  strcat(buf, "/.hardyrc");
  optionsResourceFilename = buf;
#else
#error "Unsupported platform."
#endif

  LoadOptions();

  helpInstance = new wxHelpController;
  helpInstance->Initialize("dialoged");

  InitializeTools();
  popupMenu = new wxMenu("", (wxFunction)ObjectMenuProc);
  popupMenu->Append(OBJECT_MENU_EDIT, "Edit properties");
  popupMenu->Append(OBJECT_MENU_DELETE, "Delete object");
  
  if (!wxWinBitmap)
  {
#ifdef __WINDOWS__
    wxWinBitmap = new wxBitmap("WXWINBMP", wxBITMAP_TYPE_BMP_RESOURCE);
#endif
#ifdef __X__
    wxWinBitmap = new wxBitmap(wxwin_bits, wxwin_width, wxwin_height);
#endif
  }
  return TRUE;
}

bool wxResourceManager::LoadOptions(void)
{
  wxGetResource("DialogEd", "editorWindowX", &resourceEditorWindowSize.x, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowY", &resourceEditorWindowSize.y, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowWidth", &resourceEditorWindowSize.width, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowHeight", &resourceEditorWindowSize.height, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowX", &propertyWindowSize.x, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowY", &propertyWindowSize.y, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowWidth", &propertyWindowSize.width, optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowHeight", &propertyWindowSize.height, optionsResourceFilename.GetData());
  return TRUE;
}

bool wxResourceManager::SaveOptions(void)
{
  wxWriteResource("DialogEd", "editorWindowX", resourceEditorWindowSize.x, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowY", resourceEditorWindowSize.y, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowWidth", resourceEditorWindowSize.width, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowHeight", resourceEditorWindowSize.height, optionsResourceFilename.GetData());

  wxWriteResource("DialogEd", "propertyWindowX", propertyWindowSize.x, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowY", propertyWindowSize.y, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowWidth", propertyWindowSize.width, optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowHeight", propertyWindowSize.height, optionsResourceFilename.GetData());
  
  return TRUE;
}

// Show or hide the resource editor frame, which displays a list
// of resources with ability to edit them.
bool wxResourceManager::ShowResourceEditor(bool show, wxWindow *parent, const char *title)
{
  if (show)
  {
    if (editorFrame)
    {
      editorFrame->Iconize(FALSE);
      editorFrame->Show(TRUE);
      return TRUE;
    }
    editorFrame = OnCreateEditorFrame(title);
    SetFrameTitle("");
    wxMenuBar *menuBar = OnCreateEditorMenuBar(editorFrame);
    editorFrame->SetMenuBar(menuBar);
    editorPanel = OnCreateEditorPanel(editorFrame);
    editorToolBar = (EditorToolBar *)OnCreateToolBar(editorFrame);
    editorPalette = OnCreatePalette(editorFrame);

    // Constraints for toolbar
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorFrame, wxTop, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.Unconstrained();    
    c->width.Unconstrained();
    c->height.Absolute(28);
    editorToolBar->SetConstraints(c);

    // Constraints for palette
    c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorToolBar, wxBottom, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.Unconstrained();    
    c->width.Unconstrained();
    c->height.Absolute(34);
    editorPalette->SetConstraints(c);

    // Constraints for panel
    c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorPalette, wxBottom, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.SameAs     (editorFrame, wxBottom, 0);    
    c->width.Unconstrained();
    c->height.Unconstrained();
    editorPanel->SetConstraints(c);

    editorFrame->SetAutoLayout(TRUE);

    UpdateResourceList();
    editorFrame->Show(TRUE);
    return TRUE;
  }
  else
  {
    wxFrame *fr = editorFrame;
    if (editorFrame->OnClose())
    {
      fr->Show(FALSE);
      delete fr;
      editorFrame = NULL;
      editorPanel = NULL;
    }
  }
  return TRUE;
}

void wxResourceManager::SetFrameTitle(const wxString& filename)
{
  if (editorFrame)
  {
    if (filename == wxString(""))
      editorFrame->SetTitle("wxWindows Dialog Editor - untitled");
    else
    {
      wxString str("wxWindows Dialog Editor - ");
      wxString str2(wxFileNameFromPath(WXSTRINGCAST filename));
      str += str2;
      editorFrame->SetTitle(str);
    }
  }
}

bool wxResourceManager::Save(void)
{
  if (currentFilename == wxString(""))
    return SaveAs();
  else
    return Save(currentFilename);
}

bool wxResourceManager::Save(const wxString& filename)
{
  // Ensure all visible windows are saved to their resources
  currentFilename = filename;
  SetFrameTitle(currentFilename);
  InstantiateAllResourcesFromWindows();
  if (resourceTable.Save(filename))
  {
    Modify(FALSE);
    return TRUE;
  }
  else
    return FALSE;
}

bool wxResourceManager::SaveAs(void)
{
  wxString s(wxFileSelector("Save resource file", wxPathOnly(WXSTRINGCAST currentFilename), wxFileNameFromPath(WXSTRINGCAST currentFilename),
    "wxr", "*.wxr", wxSAVE | wxOVERWRITE_PROMPT));
    
  if (s.IsNull() || s == "")
    return FALSE;
    
  currentFilename = s;
  Save(currentFilename);
  return TRUE;
}

bool wxResourceManager::SaveIfModified(void)
{
  if (Modified())
    return Save();
  else return TRUE;
}

bool wxResourceManager::Load(const wxString& filename)
{
  return New(TRUE, filename);
}

bool wxResourceManager::New(bool loadFromFile, const wxString& filename)
{
  if (!Clear(TRUE, FALSE))
    return FALSE;
    
  if (loadFromFile)
  {
    wxString str = filename;
    if (str == wxString(""))
    {
      wxString f(wxFileSelector("Open resource file", NULL, NULL, "wxr", "*.wxr", 0, NULL));
      if (!f.IsNull() && f != "")
        str = f;
      else
        return FALSE;
    }
    
    if (!resourceTable.ParseResourceFile(WXSTRINGCAST str))
    {
      wxMessageBox("Could not read file.", "Resource file load error", wxOK | wxICON_EXCLAMATION);
      return FALSE;
    }
    currentFilename = str;
    
    SetFrameTitle(currentFilename);
    
    UpdateResourceList();
  }
  else
  {
    SetFrameTitle("");
    currentFilename = "";
  }
  Modify(FALSE);
  
  return TRUE;
}

bool wxResourceManager::Clear(bool deleteWindows, bool force)
{
  if (!force && Modified())
  {
    int ans = wxMessageBox("Save modified resource file?", "Dialog Editor", wxYES_NO | wxCANCEL);
    if (ans == wxCANCEL)
      return FALSE;
    if (ans == wxYES)
      if (!SaveIfModified())
        return FALSE;
    if (ans == wxNO)
        Modify(FALSE);
  }
  
  DisassociateWindows(deleteWindows);

  resourceTable.ClearTable();
  UpdateResourceList();

  return TRUE;
}

bool wxResourceManager::DisassociateWindows(bool deleteWindows)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    DisassociateResource(res, deleteWindows);
  }
  
  return TRUE;
}

void wxResourceManager::AssociateResource(wxItemResource *resource, wxWindow *win)
{
  if (!resourceAssociations.Get((long)resource))
    resourceAssociations.Put((long)resource, win);
    
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    wxWindow *childWindow = (wxWindow *)resourceAssociations.Get((long)child);
    if (!childWindow)
      childWindow = win->FindWindow(child->GetName());
    if (childWindow)
      AssociateResource(child, childWindow);
    else
    {
      char buf[200];
      sprintf(buf, "AssociateResource: cannot find child window %s", child->GetName() ? child->GetName() : "(unnamed)");
      wxMessageBox(buf, "Dialog Editor problem", wxOK);
    }

    node = node->Next();
  }
}

bool wxResourceManager::DisassociateResource(wxItemResource *resource, bool deleteWindow)
{
  wxWindow *win = FindWindowForResource(resource);
  if (!win)
    return FALSE;

  // Disassociate children of window without deleting windows
  // since they'll be deleted by parent.
  if (win->GetChildren())
  {
    wxNode *node = win->GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      if (child->IsKindOf(CLASSINFO(wxControl)))
      	DisassociateResource(child, FALSE);
      node = node->Next();
    }
  }
  
  if (deleteWindow)
  {
    if (win->IsKindOf(CLASSINFO(wxPanel)) && !win->IsKindOf(CLASSINFO(wxDialog)))
      delete win->GetParent(); // Delete frame
    else if ( win->IsKindOf(CLASSINFO(wxControl)) )
	{
	  wxEvtHandler *childHandler = win->GetEventHandler();
	  if ( childHandler != win )
	  {
		win->PopEventHandler();
		delete childHandler;
	  }
      delete win;
	}
	else
	  // Is this enough? What about event handler? TODO
	  delete win;
  }
  RemoveSelection(win);
  resourceAssociations.Delete((long)resource);
  return TRUE;
}

bool wxResourceManager::DisassociateResource(wxWindow *win, bool deleteWindow)
{
  wxItemResource *res = FindResourceForWindow(win);
  if (res)
    return DisassociateResource(res, deleteWindow);
  return FALSE;
}

wxItemResource *wxResourceManager::FindResourceForWindow(wxWindow *win)
{
  resourceAssociations.BeginFind();
  wxNode *node;
  while (node = resourceAssociations.Next())
  {
    wxWindow *w = (wxWindow *)node->Data();
    if (w == win)
    {
      return (wxItemResource *)node->key.integer;
    }
  }
  return NULL;
}

wxWindow *wxResourceManager::FindWindowForResource(wxItemResource *resource)
{
  return (wxWindow *)resourceAssociations.Get((long)resource);
}


void wxResourceManager::MakeUniqueName(char *prefix, char *buf)
{
  while (TRUE)
  {
    sprintf(buf, "%s%d", prefix, nameCounter);
    nameCounter ++;
    
    if (!resourceTable.FindResource(buf))
      return;
  }
}

wxFrame *wxResourceManager::OnCreateEditorFrame(const char *title)
{
  int frameWidth = 420;
  int frameHeight = 300;
  
  wxResourceEditorFrame *frame = new wxResourceEditorFrame(this, NULL, (char *)title,

    resourceEditorWindowSize.x, resourceEditorWindowSize.y,
    resourceEditorWindowSize.width, resourceEditorWindowSize.height,

    wxDEFAULT_FRAME);

  wxFrame::UseNativeStatusBar(FALSE);

  frame->CreateStatusBar(2);

  wxFrame::UseNativeStatusBar(TRUE);

  frame->SetStatusText(editMode ? "Edit mode" : "Test mode", 1);
  frame->SetAutoLayout(TRUE);
#ifdef __WINDOWS__
  wxIcon *icon = new wxIcon("DIALOGEDICON");
  frame->SetIcon(icon);
#endif
  return frame;
}

wxMenuBar *wxResourceManager::OnCreateEditorMenuBar(wxFrame *parent)
{
  wxMenuBar *menuBar = new wxMenuBar;

  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(RESED_NEW_DIALOG, "New &dialog", "Create a new dialog");
  fileMenu->Append(RESED_NEW_PANEL, "New &panel", "Create a new panel");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_NEW, "&New project",           "Clear the current project");
  fileMenu->Append(wxID_OPEN, "&Open...",         "Load a resource file");
  fileMenu->Append(wxID_SAVE, "&Save",            "Save a resource file");
  fileMenu->Append(wxID_SAVEAS, "Save &As...",   "Save a resource file as...");
  fileMenu->Append(RESED_CLEAR, "&Clear",   "Clear current resources");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "E&xit",            "Exit resource editor");

  wxMenu *editMenu = new wxMenu;
  editMenu->Append(RESED_RECREATE, "&Recreate",  "Recreate the selected resource(s)");
  editMenu->Append(RESED_DELETE, "&Delete",  "Delete the selected resource(s)");
  editMenu->AppendSeparator();
  editMenu->Append(RESED_TOGGLE_TEST_MODE, "&Toggle edit/test mode",  "Toggle edit/test mode");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(RESED_CONTENTS, "&Help topics",          "Invokes the on-line help");
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT, "&About",          "About wxWindows Dialog Editor");

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(editMenu, "&Edit");
  menuBar->Append(helpMenu, "&Help");

  return menuBar;
}

wxPanel *wxResourceManager::OnCreateEditorPanel(wxFrame *parent)
{
  wxResourceEditorPanel *panel = new wxResourceEditorPanel(parent);

  editorResourceList = new wxListBox(panel, -1, wxPoint(0, 0), wxSize(-1, -1));

  wxLayoutConstraints *c = new wxLayoutConstraints;
  c->left.SameAs       (panel, wxLeft, 5);
  c->top.SameAs        (panel, wxTop, 5);
  c->right.SameAs      (panel, wxRight, 5);
  c->bottom.SameAs     (panel, wxBottom, 5);
  c->width.Unconstrained();
  c->height.Unconstrained();
  editorResourceList->SetConstraints(c);

  return panel;
}

wxToolBarBase *wxResourceManager::OnCreateToolBar(wxFrame *parent)
{
  // Load palette bitmaps
#ifdef __WINDOWS__
  ToolbarLoadBitmap = new wxBitmap("LOADTOOL");
  ToolbarSaveBitmap = new wxBitmap("SAVETOOL");
  ToolbarNewBitmap = new wxBitmap("NEWTOOL");
  ToolbarVertBitmap = new wxBitmap("VERTTOOL");
  ToolbarAlignTBitmap = new wxBitmap("ALIGNTTOOL");
  ToolbarAlignBBitmap = new wxBitmap("ALIGNBTOOL");
  ToolbarHorizBitmap = new wxBitmap("HORIZTOOL");
  ToolbarAlignLBitmap = new wxBitmap("ALIGNLTOOL");
  ToolbarAlignRBitmap = new wxBitmap("ALIGNRTOOL");
  ToolbarCopySizeBitmap = new wxBitmap("COPYSIZETOOL");
  ToolbarToBackBitmap = new wxBitmap("TOBACKTOOL");
  ToolbarToFrontBitmap = new wxBitmap("TOFRONTTOOL");
  ToolbarHelpBitmap = new wxBitmap("HELPTOOL");
#endif
#ifdef __X__
  ToolbarLoadBitmap = new wxBitmap(load_bits, load_width, load_height);
  ToolbarSaveBitmap = new wxBitmap(save_bits, save_width, save_height);
  ToolbarNewBitmap = new wxBitmap(new_bits, save_width, save_height);
  ToolbarVertBitmap = new wxBitmap(vert_bits, vert_width, vert_height);
  ToolbarAlignTBitmap = new wxBitmap(alignt_bits, alignt_width, alignt_height);
  ToolbarAlignBBitmap = new wxBitmap(alignb_bits, alignb_width, alignb_height);
  ToolbarHorizBitmap = new wxBitmap(horiz_bits, horiz_width, horiz_height);
  ToolbarAlignLBitmap = new wxBitmap(alignl_bits, alignl_width, alignl_height);
  ToolbarAlignRBitmap = new wxBitmap(alignr_bits, alignr_width, alignr_height);
  ToolbarCopySizeBitmap = new wxBitmap(copysize_bits, copysize_width, copysize_height);
  ToolbarToBackBitmap = new wxBitmap(toback_bits, toback_width, toback_height);
  ToolbarToFrontBitmap = new wxBitmap(tofront_bits, tofront_width, tofront_height);
//  ToolbarCPPBitmap = new wxBitmap(cpp_bits, cpp_width, cpp_height);
//  ToolbarTreeBitmap = new wxBitmap(tree_bits, tree_width, tree_height);
  ToolbarHelpBitmap = new wxBitmap(help_bits, help_width, help_height);
#endif

  // Create the toolbar
  EditorToolBar *toolbar = new EditorToolBar(parent, 0, 0, -1, -1, wxNO_BORDER,
                                        wxVERTICAL, 1);
  toolbar->SetMargins(2, 2);
//  toolbar->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);

#ifdef __WINDOWS__
  int width = 24;
  int dx = 2;
  int gap = 6;
#else
  int width = ToolbarLoadBitmap->GetWidth();
  int dx = 2;
  int gap = 6;
#endif
  int currentX = gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_NEW, ToolbarNewBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "New dialog");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_LOAD_FILE, ToolbarLoadBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Load");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_SAVE_FILE, ToolbarSaveBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Save");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ, ToolbarVertBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Horizontal align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_TOP_ALIGN, ToolbarAlignTBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Top align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_BOT_ALIGN, ToolbarAlignBBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Bottom align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT, ToolbarHorizBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Vertical align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN, ToolbarAlignLBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Left align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN, ToolbarAlignRBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Right align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_COPY_SIZE, ToolbarCopySizeBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Copy size");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_TO_FRONT, ToolbarToFrontBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "To front");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_TO_BACK, ToolbarToBackBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "To back");
  currentX += width + dx + gap;
/*
  toolbar->AddTool(TOOLBAR_GEN_CPP, ToolbarCPPBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL);
  currentX += width + dx;

  toolbar->AddTool(TOOLBAR_TREE, ToolbarTreeBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL);
  currentX += width + dx;
*/
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_HELP, ToolbarHelpBitmap, (wxBitmap *)NULL,
                   FALSE, (float)currentX, -1, NULL, "Help");
  currentX += width + dx;
  
  toolbar->CreateTools();

  return toolbar;

//  parent->OnSize(-1, -1);
}

void wxResourceManager::UpdateResourceList(void)
{
  editorResourceList->Clear();
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
//    if (res->GetType() == wxTYPE_DIALOG_BOX || res->GetType() == wxTYPE_FRAME || res->GetType() == wxTYPE_BITMAP)
    if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel" || resType == "wxBitmap")
    {
      AddItemsRecursively(0, res);
    }
  }
}

void wxResourceManager::AddItemsRecursively(int level, wxItemResource *resource)
{
  int padWidth = level*4;
  
  wxString theString("");
  theString.Append(' ', padWidth);
  theString += resource->GetName();
  
  editorResourceList->Append(theString.GetData(), (char *)resource);

  if (strcmp(resource->GetType(), "wxBitmap") != 0)
  {
    wxNode *node = resource->GetChildren().First();
    while (node)
    {
      wxItemResource *res = (wxItemResource *)node->Data();
      AddItemsRecursively(level+1, res);
      node = node->Next();
    }
  }
}

bool wxResourceManager::EditSelectedResource(void)
{
  int sel = editorResourceList->GetSelection();
  if (sel > -1)
  {
    wxItemResource *res = (wxItemResource *)editorResourceList->wxListBox::GetClientData(sel);
    return Edit(res);
  }
  return FALSE;
}

bool wxResourceManager::Edit(wxItemResource *res)
{
  wxString resType(res->GetType());
  if (resType == "wxDialog" || resType == "wxDialogBox")
    {
      wxDialog *dialog = (wxDialog *)FindWindowForResource(res);
      if (dialog)
        dialog->Show(TRUE);
      else
      {
        dialog = new wxDialog;
        wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(dialog, res, dialog->GetEventHandler(),
           this);
        dialog->PushEventHandler(handler);

//        dialog->SetUserEditMode(TRUE);
          
        dialog->LoadFromResource(GetEditorFrame(), res->GetName(), &resourceTable);
		handler->AddChildHandlers(); // Add event handlers for all controls
        dialog->SetModal(FALSE);
        AssociateResource(res, dialog);
        dialog->Show(TRUE);
      }
    }
  else if (resType == "wxPanel")
    {
      wxPanel *panel = (wxPanel *)FindWindowForResource(res);
      if (panel)
        panel->GetParent()->Show(TRUE);
      else
      {
        DialogEditorPanelFrame *frame = new DialogEditorPanelFrame(GetEditorFrame(), res->GetName(), 10, 10, 400, 300, wxDEFAULT_FRAME_STYLE, res->GetName());
        panel = new wxPanel;
        wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, res, panel->GetEventHandler(),
           this);
        panel->PushEventHandler(handler);
//        panel->SetUserEditMode(TRUE);
          
        panel->LoadFromResource(frame, res->GetName(), &resourceTable);
		handler->AddChildHandlers(); // Add event handlers for all controls
        AssociateResource(res, panel);
        frame->SetClientSize(res->GetWidth(), res->GetHeight());
        frame->Show(TRUE);
      }
    }
  return FALSE;
}

bool wxResourceManager::CreateNewDialog(void)
{
  char buf[256];
  MakeUniqueName("dialog", buf);
  
  wxItemResource *resource = new wxItemResource;
//  resource->SetType(wxTYPE_DIALOG_BOX);
  resource->SetType("wxDialog");
  resource->SetName(buf);
  resource->SetTitle(buf);
  resourceTable.AddResource(resource);

#ifdef __MOTIF__
  wxDialog *dialog = new wxDialog(GetEditorFrame(), -1, buf, wxPoint(10, 10), wxSize(400, 300), wxDEFAULT_DIALOG_STYLE, buf);
/*
  dialog->SetBackgroundColour(*wxLIGHT_GREY);
  dialog->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  dialog->SetButtonColour(*wxBLACK);
  dialog->SetLabelColour(*wxBLACK);
*/
#else
  wxDialog *dialog = new wxDialog(GetEditorFrame(), -1, buf, wxPoint(10, 10), wxSize(400, 300), wxDEFAULT_DIALOG_STYLE, buf);
#endif
  
  resource->SetValue1(FALSE); // Modeless to start with
  resource->SetStyle(dialog->GetWindowStyleFlag());

  // For editing in situ we will need to use the hash table to ensure
  // we don't dereference invalid pointers.
//  resourceWindowTable.Put((long)resource, dialog);

  wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(dialog, resource, dialog->GetEventHandler(),
   this);

  dialog->PushEventHandler(handler);
  
  dialog->Centre(wxBOTH);
//  dialog->SetUserEditMode(TRUE);
  dialog->Show(TRUE);

  SetEditMode(TRUE, FALSE);

  AssociateResource(resource, dialog);
//  SetCurrentResource(resource);
//  SetCurrentResourceWindow(dialog);
  UpdateResourceList();
  
  Modify(TRUE);
  
  return TRUE;
}

bool wxResourceManager::CreateNewPanel(void)
{
  char buf[256];
  MakeUniqueName("panel", buf);
  
  wxItemResource *resource = new wxItemResource;
//  resource->SetType(wxTYPE_PANEL);
  resource->SetType("wxPanel");
  resource->SetName(buf);
  resource->SetTitle(buf);
  resourceTable.AddResource(resource);
  
  DialogEditorPanelFrame *frame = new DialogEditorPanelFrame(GetEditorFrame(), buf, 10, 10, 400, 300, wxDEFAULT_FRAME_STYLE, buf);

#ifdef __MOTIF__
  wxPanel *panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 300), 0, buf);
/*
  panel->SetBackgroundColour(*wxLIGHT_GREY);
  panel->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  panel->SetButtonColour(*wxBLACK);
  panel->SetLabelColour(*wxBLACK);
*/

#else
  wxPanel *panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 300), 0, buf);
#endif
  
  resource->SetStyle(panel->GetWindowStyleFlag());

  // For editing in situ we will need to use the hash table to ensure
  // we don't dereference invalid pointers.
//  resourceWindowTable.Put((long)resource, panel);

  wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, resource, panel->GetEventHandler(),
   this);
  panel->PushEventHandler(handler);
  
  panel->Centre(wxBOTH);
//  panel->SetUserEditMode(TRUE);
  frame->Show(TRUE);

  SetEditMode(TRUE, FALSE);

  AssociateResource(resource, panel);
//  SetCurrentResource(resource);
//  SetCurrentResourceWindow(panel);
  UpdateResourceList();
  
  Modify(TRUE);
  
  return TRUE;
}

bool wxResourceManager::CreatePanelItem(wxItemResource *panelResource, wxPanel *panel, char *iType, int x, int y, bool isBitmap)
{
  char buf[256];
  if (!panel->IsKindOf(CLASSINFO(wxPanel)) && !panel->IsKindOf(CLASSINFO(wxDialog)))
    return FALSE;

  Modify(TRUE);
  
  wxItemResource *res = new wxItemResource;
  wxControl *newItem = NULL;
  res->SetSize(x, y, -1, -1);
  res->SetType(iType);
  
  wxString itemType(iType);

  if (itemType == "wxButton")
    {
      MakeUniqueName("button", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxBitmapButton(panel, -1, wxWinBitmap, wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
      else
        newItem = new wxButton(panel, -1, "Button", wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  if (itemType == "wxBitmapButton")
    {
      MakeUniqueName("button", buf);
      res->SetName(buf);
      newItem = new wxBitmapButton(panel, -1, wxWinBitmap, wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxMessage" || itemType == "wxStaticText")
    {
      MakeUniqueName("message", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxStaticBitmap(panel, -1, wxWinBitmap, wxPoint(x, y), wxSize(0, 0), 0, buf);
      else
        newItem = new wxStaticText(panel, -1, "Message", wxPoint(x, y), wxSize(-1, -1), 0, buf);
    }
  else if (itemType == "wxStaticBitmap")
    {
      MakeUniqueName("message", buf);
      res->SetName(buf);
      newItem = new wxStaticBitmap(panel, -1, wxWinBitmap, wxPoint(x, y), wxSize(-1, -1), 0, buf);
    }
  else if (itemType == "wxCheckBox")
    {
      MakeUniqueName("checkbox", buf);
      res->SetName(buf);
      newItem = new wxCheckBox(panel, -1, "Checkbox", wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxListBox")
    {
      MakeUniqueName("listbox", buf);
      res->SetName(buf);
      newItem = new wxListBox(panel, -1, wxPoint(x, y), wxSize(-1, -1), 0, NULL, 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxRadioBox")
    {
      MakeUniqueName("radiobox", buf);
      res->SetName(buf);
      wxString names[] = { "One", "Two" };
      newItem = new wxRadioBox(panel, -1, "Radiobox", wxPoint(x, y), wxSize(-1, -1), 2, names, 2,
	     wxHORIZONTAL, wxDefaultValidator, buf);
      res->SetStringValues(new wxStringList("One", "Two", NULL));
    }
  else if (itemType == "wxChoice")
    {
      MakeUniqueName("choice", buf);
      res->SetName(buf);
      newItem = new wxChoice(panel, -1, wxPoint(x, y), wxSize(-1, -1), 0, NULL, 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxGroupBox" || itemType == "wxStaticBox")
    {
      MakeUniqueName("group", buf);
      res->SetName(buf);
      newItem = new wxStaticBox(panel, -1, "Groupbox", wxPoint(x, y), wxSize(200, 200), 0, buf);
    }
  else if (itemType == "wxGauge")
    {
      MakeUniqueName("gauge", buf);
      res->SetName(buf);
      newItem = new wxGauge(panel, -1, 10, wxPoint(x, y), wxSize(80, 30), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  else if (itemType == "wxSlider")
    {
      MakeUniqueName("slider", buf);
      res->SetName(buf);
      newItem = new wxSlider(panel, -1, 1, 1, 10, wxPoint(x, y), wxSize(120, -1), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  else if (itemType == "wxText" || itemType == "wxTextCtrl")
    {
      MakeUniqueName("text", buf);
      res->SetName(buf);
      newItem = new wxTextCtrl(panel, -1, "", wxPoint(x, y), wxSize(120, -1), 0, wxDefaultValidator, buf);
    }
/*
  else if (itemType == "wxMultiText")
    {
      MakeUniqueName("multitext", buf);
      res->SetName(buf);
      newItem = new wxMultiText(panel, (wxFunction)NULL, "Multitext", "", x, y, -1, -1, 0, wxDefaultValidator, buf);
    }
*/
  else if (itemType == "wxScrollBar")
    {
      MakeUniqueName("scrollbar", buf);
      res->SetName(buf);
      newItem = new wxScrollBar(panel, -1, wxPoint(x, y), wxSize(140, -1), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  if (!newItem)
    return FALSE;

  newItem->PushEventHandler(new wxResourceEditorControlHandler(newItem, newItem));

  res->SetStyle(newItem->GetWindowStyleFlag());
  AssociateResource(res, newItem);
  panelResource->GetChildren().Append(res);

  UpdateResourceList();
  
  return TRUE;
}

// Find the first dialog or panel for which
// there is a selected panel item.
wxWindow *wxResourceManager::FindParentOfSelection(void)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxWindow *win = FindWindowForResource(res);
    if (win)
    {
      wxNode *node1 = win->GetChildren()->First();
      while (node1)
      {
        wxControl *item = (wxControl *)node1->Data();
	    wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
        if (item->IsKindOf(CLASSINFO(wxControl)) && childHandler->IsSelected())
          return win;
        node1 = node1->Next();
      }
    }
  }
  return NULL;
}

// Format the panel items according to 'flag'
void wxResourceManager::AlignItems(int flag)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;

  wxNode *node = GetSelections().First();
  if (!node)
    return;
    
  wxControl *firstSelection = (wxControl *)node->Data();
  if (firstSelection->GetParent() != win)
    return;
    
  int firstX, firstY;
  int firstW, firstH;
  firstSelection->GetPosition(&firstX, &firstY);
  firstSelection->GetSize(&firstW, &firstH);
  int centreX = (int)(firstX + (firstW / 2));
  int centreY = (int)(firstY + (firstH / 2));

  while (node = node->Next())
  {
    wxControl *item = (wxControl *)node->Data();
    if (item->GetParent() == win)
    {
      int x, y, w, h;
      item->GetPosition(&x, &y);
      item->GetSize(&w, &h);

      int newX, newY;

      switch (flag)
      {
        case TOOLBAR_FORMAT_HORIZ:
        {
          newX = x;
          newY = (int)(centreY - (h/2.0));
          break;
        }
        case TOOLBAR_FORMAT_VERT:
        {
          newX = (int)(centreX - (w/2.0));
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
        {
          newX = firstX;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
        {
          newX = x;
          newY = firstY;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
        {
          newX = firstX + firstW - w;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
        {
          newX = x;
          newY = firstY + firstH - h;
          break;
        }
        default:
          newX = x; newY = y;
          break;
      }

      item->SetSize(newX, newY, w, h);
    }
  }
  win->Refresh();
}

// Copy the first image's size to subsequent images
void wxResourceManager::CopySize(void)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;

  wxNode *node = GetSelections().First();
  if (!node)
    return;
    
  wxControl *firstSelection = (wxControl *)node->Data();
  if (firstSelection->GetParent() != win)
    return;
    
  int firstX, firstY;
  int firstW, firstH;
  firstSelection->GetPosition(&firstX, &firstY);
  firstSelection->GetSize(&firstW, &firstH);
  int centreX = (int)(firstX + (firstW / 2));
  int centreY = (int)(firstY + (firstH / 2));

  while (node = node->Next())
  {
    wxControl *item = (wxControl *)node->Data();
    if (item->GetParent() == win)
      item->SetSize(-1, -1, firstW, firstH);
  }
  win->Refresh();
}

void wxResourceManager::ToBackOrFront(bool toBack)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;
  wxItemResource *winResource = FindResourceForWindow(win);

  wxNode *node = GetSelections().First();
  while (node)
  {
    wxControl *item = (wxControl *)node->Data();
    wxItemResource *itemResource = FindResourceForWindow(item);
    if (item->GetParent() == win)
    {
      win->GetChildren()->DeleteObject(item);
      if (winResource)
        winResource->GetChildren().DeleteObject(itemResource);
      if (toBack)
      {
        win->GetChildren()->Insert(item);
        if (winResource)
          winResource->GetChildren().Insert(itemResource);
      }
      else
      {
        win->GetChildren()->Append(item);
        if (winResource)
          winResource->GetChildren().Append(itemResource);
      }
    }
    node = node->Next();
  }
//  win->Refresh();
}

void wxResourceManager::AddSelection(wxWindow *win)
{
  if (!selections.Member(win))
    selections.Append(win);
}

void wxResourceManager::RemoveSelection(wxWindow *win)
{
  selections.DeleteObject(win);
}

// Need to search through resource table removing this from
// any resource which has this as a parent.
bool wxResourceManager::RemoveResourceFromParent(wxItemResource *res)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *thisRes = (wxItemResource *)node->Data();
    if (thisRes->GetChildren().Member(res))
    {
      thisRes->GetChildren().DeleteObject(res);
      return TRUE;
    }
  }
  return FALSE;
}

bool wxResourceManager::DeleteResource(wxItemResource *res)
{
  if (!res)
    return FALSE;

  RemoveResourceFromParent(res);

  wxNode *node = res->GetChildren().First();
  while (node)
  {
    wxNode *next = node->Next();
    wxItemResource *child = (wxItemResource *)node->Data();
    DeleteResource(child);
    node = next;
  }
  
  // If this is a button or message resource, delete the
  // associate bitmap resource if not being used.
  wxString resType(res->GetType());
  
  if ((resType == "wxMessage" || resType == "wxButton") && res->GetValue4())
  {
    PossiblyDeleteBitmapResource(res->GetValue4());
  }

  resourceTable.Delete(res->GetName());
  delete res;
  Modify(TRUE);
  return TRUE;
}

bool wxResourceManager::DeleteResource(wxWindow *win, bool deleteWindow)
{
  if (win->IsKindOf(CLASSINFO(wxControl)))
  {
    // Deselect and refresh window in case we leave selection
    // handles behind
    wxControl *item = (wxControl *)win;
    wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
    if (childHandler->IsSelected())
    {
      RemoveSelection(item);
      childHandler->SelectItem(FALSE);
      item->GetParent()->Refresh();
    }
  }
  
  wxItemResource *res = FindResourceForWindow(win);
  
  DisassociateResource(res, deleteWindow);
  DeleteResource(res);
  UpdateResourceList();

  // What about associated event handler? Must clean up! BUGBUG
//  if (win && deleteWindow)
//    delete win;
  return TRUE;
}

// Will eventually have bitmap type information, for different
// kinds of bitmap.
char *wxResourceManager::AddBitmapResource(char *filename)
{
  wxItemResource *resource = FindBitmapResourceByFilename(filename);
  if (!resource)
  {
    char buf[256];
    MakeUniqueName("bitmap", buf);
    resource = new wxItemResource;
    resource->SetType("wxBitmap");
    resource->SetName(buf);
    
    // A bitmap resource has one or more children, specifying
    // alternative bitmaps.
    wxItemResource *child = new wxItemResource;
    child->SetType("wxBitmap");
    child->SetName(filename);
    child->SetValue1(wxBITMAP_TYPE_BMP);
    child->SetValue2(RESOURCE_PLATFORM_ANY);
    child->SetValue3(0); // Depth
    child->SetSize(0,0,0,0);
    resource->GetChildren().Append(child);

    resourceTable.AddResource(resource);

    UpdateResourceList();
  }
  if (resource)
    return resource->GetName();
  else
    return NULL;
}

 // Delete the bitmap resource if it isn't being used by another resource.
void wxResourceManager::PossiblyDeleteBitmapResource(char *resourceName)
{
  if (!IsBitmapResourceUsed(resourceName))
  {
    wxItemResource *res = resourceTable.FindResource(resourceName);
    DeleteResource(res);
    UpdateResourceList();
  }
}

bool wxResourceManager::IsBitmapResourceUsed(char *resourceName)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxDialog")
    {
      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        wxString childResType(child->GetType());
        
        if ((childResType == "wxMessage" || childResType == "wxButton") &&
             child->GetValue4() &&
            (strcmp(child->GetValue4(), resourceName) == 0))
          return TRUE;
        node1 = node1->Next();
      }
    }
  }
  return FALSE;
}

// Given a wxButton or wxMessage, find the corresponding bitmap filename.
char *wxResourceManager::FindBitmapFilenameForResource(wxItemResource *resource)
{
  if (!resource || !resource->GetValue4())
    return NULL;
  wxItemResource *bitmapResource = resourceTable.FindResource(resource->GetValue4());
  if (!bitmapResource)
    return NULL;

  wxNode *node = bitmapResource->GetChildren().First();
  while (node)
  {
    // Eventually augment this to return a bitmap of the right kind or something...
    // Maybe the root of the filename remains the same, so it doesn't matter which we
    // pick up. Otherwise how do we specify multiple filenames... too boring...
    wxItemResource *child = (wxItemResource *)node->Data();
    return child->GetName();
    
    node = node->Next();
  }
  return NULL;
}

wxItemResource *wxResourceManager::FindBitmapResourceByFilename(char *filename)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxBitmap")
    {
      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        if (child->GetName() && (strcmp(child->GetName(), filename) == 0))
          return res;
        node1 = node1->Next();
      }
    }
  }
  return NULL;
}

 // Deletes 'win' and creates a new window from the resource that
 // was associated with it. E.g. if you can't change properties on the
 // fly, you'll need to delete the window and create it again.
wxWindow *wxResourceManager::RecreateWindowFromResource(wxWindow *win, wxWindowPropertyInfo *info)
{
  wxItemResource *resource = FindResourceForWindow(win);

  // Put the current window properties into the wxItemResource object
  
  wxWindowPropertyInfo *newInfo = NULL;
  if (!info)
  {
    newInfo = wxCreatePropertyInfoForWindow(win);
    info = newInfo;
  }

  info->InstantiateResource(resource);

  wxWindow *newWin = NULL;
  wxWindow *parent = win->GetParent();
  
  if (win->IsKindOf(CLASSINFO(wxPanel)))
  {
    bool isDialog = win->IsKindOf(CLASSINFO(wxDialog));
    wxWindow *parent = win->GetParent();
    
    win->GetEventHandler()->OnClose();
    
    if (!isDialog && parent)
    {
      // Delete frame parent of panel if this is not a dialog box
      parent->Close(TRUE);
    }

    Edit(resource);
    newWin = FindWindowForResource(resource);
  }
  else
  {
    DisassociateResource(resource, FALSE);
    delete win;
    newWin = resourceTable.CreateItem((wxPanel *)parent, resource);
    AssociateResource(resource, newWin);
    UpdateResourceList();
  }

  if (info)
    info->SetPropertyWindow(newWin);
    
  if (newInfo)
    delete newInfo;
  
  return newWin;
}

// Delete resource highlighted in the listbox
bool wxResourceManager::DeleteSelection(bool deleteWindow)
{
  int sel = editorResourceList->GetSelection();
  if (sel > -1)
  {
    wxItemResource *res = (wxItemResource *)editorResourceList->wxListBox::GetClientData(sel);
    wxWindow *win = FindWindowForResource(res);
/*
    if (res == currentResource)
    {
      currentResource = NULL;
      currentResourceWindow = NULL;
    }
*/

    DisassociateResource(res, deleteWindow);
    DeleteResource(res);
    UpdateResourceList();

/*
    // What about associated event handler? Must clean up! BUGBUG
    if (win && deleteWindow)
      delete win;
*/

    Modify(TRUE);
  }

  return FALSE;
}

// Delete resource highlighted in the listbox
bool wxResourceManager::RecreateSelection(void)
{
  wxNode *node = GetSelections().First();
  while (node)
  {
    wxControl *item = (wxControl *)node->Data();
    wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
    wxNode *next = node->Next();
    childHandler->SelectItem(FALSE);

    RemoveSelection(item);
    
    RecreateWindowFromResource(item);
    
    node = next;
  }
  return TRUE;
}

bool wxResourceManager::EditDialog(wxDialog *dialog, wxWindow *parent)
{
  return FALSE;
}

void wxResourceManager::SetEditMode(bool flag, bool changeCurrentResource)
{
  editMode = flag;
  if (editorFrame)
    editorFrame->SetStatusText(editMode ? "Edit mode" : "Test mode", 1);

  // Switch mode for each dialog in the resource list
  resourceTable.BeginFind();
  wxNode *node = resourceTable.Next();
  while (node)
  {
    wxItemResource *resource = (wxItemResource *)node->Data();
    wxWindow *currentResourceWindow = FindWindowForResource(resource);
  
    if (changeCurrentResource && currentResourceWindow && (currentResourceWindow->IsKindOf(CLASSINFO(wxPanel))))
    {
      wxPanel *panel = (wxPanel *)currentResourceWindow;
      if (editMode)
      {
        // If we have already installed our own handler, don't bother editing.
        // This test will need to be changed eventually because for in-situ editing,
        // the user might have installed a different one anyway.
        wxEvtHandler *handler = panel->GetEventHandler();
  	    handler->SetEvtHandlerEnabled(TRUE);
        // Enable all children
        wxNode *node = panel->GetChildren()->First();
        while (node)
        {
          wxControl *item = (wxControl *)node->Data();
		  if ( item->IsKindOf(CLASSINFO(wxControl)) )
		  {
          	wxEvtHandler *childHandler = item->GetEventHandler();
		  	childHandler->SetEvtHandlerEnabled(TRUE);
		  }
          node = node->Next();
        }
      }
      else
      {
        wxEvtHandler *handler = panel->GetEventHandler();
		handler->SetEvtHandlerEnabled(FALSE);
        // Deselect all items on the dialog and refresh.
        wxNode *node = panel->GetChildren()->First();
        while (node)
        {
          wxControl *item = (wxControl *)node->Data();
          if (item->IsKindOf(CLASSINFO(wxControl)))
	      {
		     wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
		     childHandler->SetEvtHandlerEnabled(FALSE);
             childHandler->SelectItem(FALSE);
 		  }
          node = node->Next();
        }
        panel->Refresh();
      }
    }
    node = resourceTable.Next();
  }
}

// Ensures that all currently shown windows are saved to resources,
// e.g. just before writing to a .wxr file.
bool wxResourceManager::InstantiateAllResourcesFromWindows(void)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    
    if (resType == "wxDialog")
    {
      wxWindow *win = (wxWindow *)FindWindowForResource(res);
      if (win)
        InstantiateResourceFromWindow(res, win, TRUE);
    }
    else if (resType == "wxPanel")
    {
      wxWindow *win = (wxWindow *)FindWindowForResource(res);
      if (win)
        InstantiateResourceFromWindow(res, win, TRUE);
    }
  }
  return TRUE;  
}

bool wxResourceManager::InstantiateResourceFromWindow(wxItemResource *resource, wxWindow *window, bool recurse)
{
  wxWindowPropertyInfo *info = wxCreatePropertyInfoForWindow(window);
  info->SetResource(resource);
  info->InstantiateResource(resource);
  delete info;
  
  if (recurse)
  {
    wxNode *node = resource->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      wxWindow *childWindow = FindWindowForResource(child);
      
      if (!childWindow)
      {
        char buf[200];
        sprintf(buf, "Could not find window %s", child->GetName());
        wxMessageBox(buf, "Dialog Editor problem", wxOK);
      }
      else
        InstantiateResourceFromWindow(child, childWindow, recurse);
      node = node->Next();
    }
  }
  
  return TRUE;
}


/*
 * Resource editor frame
 */
 
wxResourceEditorFrame::wxResourceEditorFrame(wxResourceManager *resMan, wxFrame *parent, char *title,
    int x, int y, int width, int height, long style, char *name):
  wxFrame(parent, -1, title, wxPoint(x, y), wxSize(width, height), style, name)
{
  manager = resMan;
}

wxResourceEditorFrame::~wxResourceEditorFrame(void)
{
}

void wxResourceEditorFrame::OldOnMenuCommand(int cmd)
{
  switch (cmd)
  {
    case wxID_NEW:
    {
      manager->New(FALSE);
      break;
    }
    case RESED_NEW_DIALOG:
    {
      manager->CreateNewDialog();
      break;
    }
    case RESED_NEW_PANEL:
    {
      manager->CreateNewPanel();
      break;
    }
    case wxID_OPEN:
    {
      manager->New(TRUE);
      break;
    }
    case RESED_CLEAR:
    {
      manager->Clear(TRUE, FALSE);
      break;
    }
    case wxID_SAVE:
    {
      manager->Save();
      break;
    }
    case wxID_SAVEAS:
    {
      manager->SaveAs();
      break;
    }
    case wxID_EXIT:
    {
	  manager->Clear(TRUE, FALSE) ;
      this->Close();
      break;
    }
    case wxID_ABOUT:
    {
      char buf[300];
      sprintf(buf, "wxWindows Dialog Editor %.1f\nAuthor: Julian Smart J.Smart@ed.ac.uk\nJulian Smart (c) 1996", wxDIALOG_EDITOR_VERSION);
      (void)wxMessageBox(buf, "About Dialog Editor", wxOK|wxCENTRE);
      break;
    }
    case RESED_CONTENTS:
    {
      wxBeginBusyCursor();
      manager->GetHelpInstance()->LoadFile();
      manager->GetHelpInstance()->DisplayContents();
      wxEndBusyCursor();
      break;
    }
    case RESED_DELETE:
    {
      manager->DeleteSelection();
      break;
    }
    case RESED_RECREATE:
    {
      manager->RecreateSelection();
      break;
    }
    case RESED_TOGGLE_TEST_MODE:
    {
      manager->SetEditMode(!manager->GetEditMode());
      break;
    }
    default:
      break;
  }
}

bool wxResourceEditorFrame::OnClose(void)
{
  if (manager->Modified())
  {
/*
    int ans = wxMessageBox("Save modified resource file?", "Dialog Editor", wxYES_NO | wxCANCEL);
    if (ans == wxCANCEL)
      return FALSE;
    if (ans == wxYES)
      if (!manager->SaveIfModified())
        return FALSE;
*/
  if (!manager->Clear(TRUE, FALSE))
     return FALSE;
  }
    
  if (!Iconized())
  {
    int w, h;
    GetSize(&w, &h);
    manager->resourceEditorWindowSize.width = w;
    manager->resourceEditorWindowSize.height = h;

    int x, y;
    GetPosition(&x, &y);

    manager->resourceEditorWindowSize.x = x;
    manager->resourceEditorWindowSize.y = y;
  }
  manager->SetEditorFrame(NULL);
  manager->SetEditorToolBar(NULL);
  manager->SetEditorPalette(NULL);

  return TRUE;
}

/*
 * Resource editor panel
 */
 
wxResourceEditorPanel::wxResourceEditorPanel(wxWindow *parent, int x, int y, int width, int height,
    long style, char *name):
  wxPanel(parent, -1, wxPoint(x, y), wxSize(width, height), style, name)
{
}

wxResourceEditorPanel::~wxResourceEditorPanel(void)
{
}

void wxResourceEditorPanel::OnDefaultAction(wxControl *item)
{
  wxResourceEditorFrame *frame = (wxResourceEditorFrame *)GetParent();
  wxResourceManager *manager = frame->manager;

  if (item == manager->GetEditorResourceList())
  {
    manager->EditSelectedResource();
  }
}

// Popup menu callback
void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event)
{
  wxWindow *data = (wxWindow *)menu.GetClientData();
  if (!data)
    return;

  switch (event.GetInt())
  {
    case OBJECT_MENU_EDIT:
    {
      wxResourceEditWindow(data);
      break;
    }
    case OBJECT_MENU_DELETE:
    {
      wxResourceManager::currentResourceManager->DeleteResource(data);
      break;
    }
    default:
      break;
  }
}

wxWindowPropertyInfo *wxCreatePropertyInfoForWindow(wxWindow *win)
{
  wxWindowPropertyInfo *info = NULL;
  if (win->IsKindOf(CLASSINFO(wxScrollBar)))
        {
          info = new wxScrollBarPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxStaticBox)))
        {
          info = new wxGroupBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxCheckBox)))
        {
          info = new wxCheckBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxSlider)))
        {
          info = new wxSliderPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxGauge)))
        {
          info = new wxGaugePropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxListBox)))
        {
          info = new wxListBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxRadioBox)))
        {
          info = new wxRadioBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxChoice)))
        {
          info = new wxChoicePropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxButton)))
        {
          info = new wxButtonPropertyInfo(win);
/*
          if (((wxButton *)win)->IsBitmap())
            ((wxButtonPropertyInfo *)info)->isBitmapButton = TRUE;
*/
        }
  else if (win->IsKindOf(CLASSINFO(wxStaticText)))
        {
          info = new wxStaticTextPropertyInfo(win);
/*
          if (((wxMessage *)win)->IsBitmap())
            ((wxMessagePropertyInfo *)info)->isBitmapMessage = TRUE;
*/
        }
  else if (win->IsKindOf(CLASSINFO(wxTextCtrl)))
        {
          info = new wxTextPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxDialog)))
        {
          info = new wxDialogPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxPanel)))
        {
          info = new wxPanelPropertyInfo(win);
        }
  else
        {
          info = new wxWindowPropertyInfo(win);
        }
  return info;
}

// Popup menu callback
void wxResourceEditWindow(wxWindow *win)
{
  wxWindowPropertyInfo *info = wxCreatePropertyInfoForWindow(win);
  if (info)
  {
    info->SetResource(wxResourceManager::currentResourceManager->FindResourceForWindow(win));
    wxString str("Editing ");
    str += win->GetClassInfo()->GetClassName();
    str += ": ";
    if (win->GetName() != "")
        str += win->GetName();
    else
        str += "properties";
    info->Edit(NULL, WXSTRINGCAST str);
    delete info;
  }
}

/*
 * Main toolbar
 *
 */

#if defined(__WINDOWS__) && defined(__WIN95__)
BEGIN_EVENT_TABLE(EditorToolBar, wxToolBar95)
#elif defined(__WINDOWS__)
BEGIN_EVENT_TABLE(EditorToolBar, wxToolBarMSW)
#else
BEGIN_EVENT_TABLE(EditorToolBar, wxToolBarSimple)
#endif
	EVT_PAINT(EditorToolBar::OnPaint)
END_EVENT_TABLE()

EditorToolBar::EditorToolBar(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
#if defined(__WINDOWS__) && defined(__WIN95__)
  wxToolBar95(frame, -1, wxPoint(x, y), wxSize(w, h), style, direction, RowsOrColumns)
#elif defined(__WINDOWS__)
  wxToolBarMSW(frame, -1, wxPoint(x, y), wxSize(w, h), style, direction, RowsOrColumns)
#else
  wxToolBarSimple(frame, -1, wxPoint(x, y), wxSize(w, h), style, direction, RowsOrColumns)
#endif
{
}

bool EditorToolBar::OnLeftClick(int toolIndex, bool toggled)
{
  wxResourceManager *manager = wxResourceManager::currentResourceManager;
  
  switch (toolIndex)
  {
    case TOOLBAR_LOAD_FILE:
    {
      manager->New(TRUE);
      break;
    }
    case TOOLBAR_NEW:
    {
      manager->New(FALSE);
      break;
    }
    case TOOLBAR_SAVE_FILE:
    {
      manager->Save();
      break;
    }
    case TOOLBAR_HELP:
    {
      wxBeginBusyCursor();
      manager->GetHelpInstance()->LoadFile();
      manager->GetHelpInstance()->DisplayContents();
      wxEndBusyCursor();
      break;
    }
    case TOOLBAR_FORMAT_HORIZ:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT);
      break;
    }
    case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT_TOP_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT_BOT_ALIGN);
      break;
    }
    case TOOLBAR_COPY_SIZE:
    {
      manager->CopySize();
      break;
    }
    case TOOLBAR_TO_BACK:
    {
      manager->ToBackOrFront(TRUE);
      break;
    }
    case TOOLBAR_TO_FRONT:
    {
      manager->ToBackOrFront(FALSE);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

void EditorToolBar::OnMouseEnter(int toolIndex)
{
  wxFrame *frame = (wxFrame *)GetParent();
  
  if (!frame) return;
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case TOOLBAR_LOAD_FILE:
          frame->SetStatusText("Load project file");
          break;
        case TOOLBAR_SAVE_FILE:
          frame->SetStatusText("Save project file");
          break;
        case TOOLBAR_NEW:
          frame->SetStatusText("Create a new resource");
          break;
        case TOOLBAR_FORMAT_HORIZ:
          frame->SetStatusText("Align items horizontally");
          break;
        case TOOLBAR_FORMAT_VERT:
          frame->SetStatusText("Align items vertically");
          break;
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
          frame->SetStatusText("Left-align items");
          break;
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
          frame->SetStatusText("Right-align items");
          break;
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
          frame->SetStatusText("Top-align items");
          break;
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
          frame->SetStatusText("Bottom-align items");
          break;
        case TOOLBAR_COPY_SIZE:
          frame->SetStatusText("Copy size from first selection");
          break;
        case TOOLBAR_TO_FRONT:
          frame->SetStatusText("Put image to front");
          break;
        case TOOLBAR_TO_BACK:
          frame->SetStatusText("Put image to back");
          break;
        case TOOLBAR_HELP:
          frame->SetStatusText("Display help contents");
          break;
        default:
          break;
      }
  }
  else frame->SetStatusText("");
}

void EditorToolBar::OnPaint(wxPaintEvent& event)
{
#if defined(__WINDOWS__) && defined(__WIN95__)
  wxToolBar95::OnPaint(event);
#elif defined(__WINDOWS__)
  wxToolBarMSW::OnPaint(event);
#else
  wxToolBarSimple::OnPaint(event);
#endif

  wxPaintDC dc(this);
  int w, h;
  GetSize(&w, &h);
  dc.SetPen(wxBLACK_PEN);
  dc.SetBrush(wxTRANSPARENT_BRUSH);
  dc.DrawLine(0, h-1, w, h-1);
}

/*
 * Frame for editing a panel in
 */
 
bool DialogEditorPanelFrame::OnClose(void)
{
  wxWindow *child = (wxWindow *)GetChildren()->First()->Data();
  wxEvtHandler *handler = child->GetEventHandler();
  return handler->OnClose();
}
