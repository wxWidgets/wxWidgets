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
#include "wx/menu.h"
#include "wx/toolbar.h"
#endif

#include "wx/scrolbar.h"
#include "wx/config.h"

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if wxUSE_IOSTREAMH
#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#else
#include <strstream>
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

#ifdef __WXMSW__
#include "wx/help.h"
#endif

#include "reseditr.h"
#include "winprop.h"
#include "dlghndlr.h"
#include "edtree.h"
#include "edlist.h"

static void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event);
wxResourceManager *wxResourceManager::sm_currentResourceManager = NULL;

#ifdef __WXGTK__
#include "bitmaps/load.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/vert.xpm"
#include "bitmaps/alignt.xpm"
#include "bitmaps/alignb.xpm"
#include "bitmaps/horiz.xpm"
#include "bitmaps/alignl.xpm"
#include "bitmaps/alignr.xpm"
#include "bitmaps/copysize.xpm"
#include "bitmaps/tofront.xpm"
#include "bitmaps/toback.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/wxwin.xpm"
#endif

/*
 * Resource manager
 */

wxResourceManager::wxResourceManager():
  m_imageList(16, 16, TRUE)
{
  sm_currentResourceManager = this;
  m_editorFrame = NULL;
  m_editorPanel = NULL;
  m_popupMenu = NULL;
  m_editorResourceTree = NULL;
  m_editorControlList = NULL;
  m_nameCounter = 1;
  m_symbolIdCounter = 99;
  m_modified = FALSE;
  m_currentFilename = "";
  m_symbolFilename = "";
  m_editorToolBar = NULL;

  // Default window positions
  m_resourceEditorWindowSize.width = 500;
  m_resourceEditorWindowSize.height = 450;

  m_resourceEditorWindowSize.x = 0;
  m_resourceEditorWindowSize.y = 0;

  m_propertyWindowSize.width = 300;
  m_propertyWindowSize.height = 300;

#ifdef __WXMSW__
  m_helpController = NULL;
#endif

  m_bitmapImage = NULL;
  m_rootDialogItem = 0;
}

wxResourceManager::~wxResourceManager()
{
  sm_currentResourceManager = NULL;
  SaveOptions();

#ifdef __WXMSW__
  if (m_helpController)
  {
    m_helpController->Quit();
    delete m_helpController;
    m_helpController = NULL;
  }
#endif  
  
  delete m_bitmapImage;
  delete m_popupMenu;
}

bool wxResourceManager::Initialize()
{
  // Set up the resource filename for each platform.
#ifdef __WXMSW__
  // dialoged.ini in the Windows directory
  char buf[256];
  GetWindowsDirectory(buf, 256);
  strcat(buf, "\\dialoged.ini");
  m_optionsResourceFilename = buf;
#elif defined(__WXGTK__)
  wxGetHomeDir( &m_optionsResourceFilename );
  m_optionsResourceFilename += "/.dialogedrc";
#else
#error "Unsupported platform."
#endif

  LoadOptions();

#ifdef __WXMSW__
  m_helpController = new wxHelpController;
  m_helpController->Initialize("dialoged");
#endif

  m_popupMenu = new wxMenu("", (wxFunction)ObjectMenuProc);
  m_popupMenu->Append(OBJECT_MENU_EDIT, "Edit properties");
  m_popupMenu->Append(OBJECT_MENU_DELETE, "Delete object");

  if (!m_bitmapImage)
  {
#ifdef __WXMSW__
    m_bitmapImage = new wxBitmap("WXWINBMP", wxBITMAP_TYPE_BMP_RESOURCE);
#endif
#ifdef __WXGTK__
    m_bitmapImage = new wxBitmap( wxwin_xpm );
#endif
  }

  // Initialize the image list icons
#ifdef __WXMSW__
  wxIcon icon1("DIALOG_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
  wxIcon icon2("FOLDER1_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
  wxIcon icon3("FOLDER2_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
  wxIcon icon4("BUTTONSM_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
  m_imageList.Add(icon1);
  m_imageList.Add(icon2);
  m_imageList.Add(icon3);
  m_imageList.Add(icon4);
#endif

  m_symbolTable.AddStandardSymbols();

  return TRUE;
}

bool wxResourceManager::LoadOptions()
{
  wxConfig config("DialogEd", "wxWindows");

  config.Read("editorWindowX", &m_resourceEditorWindowSize.x);
  config.Read("editorWindowY", &m_resourceEditorWindowSize.y);
  config.Read("editorWindowWidth", &m_resourceEditorWindowSize.width);
  config.Read("editorWindowHeight", &m_resourceEditorWindowSize.height);
  config.Read("propertyWindowX", &m_propertyWindowSize.x);
  config.Read("propertyWindowY", &m_propertyWindowSize.y);
  config.Read("propertyWindowWidth", &m_propertyWindowSize.width);
  config.Read("propertyWindowHeight", &m_propertyWindowSize.height);

  /*
  wxGetResource("DialogEd", "editorWindowX", &m_resourceEditorWindowSize.x, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowY", &m_resourceEditorWindowSize.y, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowWidth", &m_resourceEditorWindowSize.width, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "editorWindowHeight", &m_resourceEditorWindowSize.height, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowX", &m_propertyWindowSize.x, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowY", &m_propertyWindowSize.y, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowWidth", &m_propertyWindowSize.width, m_optionsResourceFilename.GetData());
  wxGetResource("DialogEd", "propertyWindowHeight", &m_propertyWindowSize.height, m_optionsResourceFilename.GetData());
  */
  return TRUE;
}

bool wxResourceManager::SaveOptions()
{
  wxConfig config("DialogEd", "wxWindows");

  config.Write("editorWindowX", m_resourceEditorWindowSize.x);
  config.Write("editorWindowY", m_resourceEditorWindowSize.y);
  config.Write("editorWindowWidth", m_resourceEditorWindowSize.width);
  config.Write("editorWindowHeight", m_resourceEditorWindowSize.height);
  config.Write("propertyWindowX", m_propertyWindowSize.x);
  config.Write("propertyWindowY", m_propertyWindowSize.y);
  config.Write("propertyWindowWidth", m_propertyWindowSize.width);
  config.Write("propertyWindowHeight", m_propertyWindowSize.height);
  /*
  wxWriteResource("DialogEd", "editorWindowX", m_resourceEditorWindowSize.x, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowY", m_resourceEditorWindowSize.y, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowWidth", m_resourceEditorWindowSize.width, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "editorWindowHeight", m_resourceEditorWindowSize.height, m_optionsResourceFilename.GetData());

  wxWriteResource("DialogEd", "propertyWindowX", m_propertyWindowSize.x, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowY", m_propertyWindowSize.y, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowWidth", m_propertyWindowSize.width, m_optionsResourceFilename.GetData());
  wxWriteResource("DialogEd", "propertyWindowHeight", m_propertyWindowSize.height, m_optionsResourceFilename.GetData());
  */

  return TRUE;
}

// Show or hide the resource editor frame, which displays a list
// of resources with ability to edit them.
bool wxResourceManager::ShowResourceEditor(bool show, wxWindow *WXUNUSED(parent), const char *title)
{
  if (show)
  {
    if (m_editorFrame)
    {
      m_editorFrame->Iconize(FALSE);
      m_editorFrame->Show(TRUE);
      return TRUE;
    }
    m_editorFrame = OnCreateEditorFrame(title);
    SetFrameTitle("");
    wxMenuBar *menuBar = OnCreateEditorMenuBar(m_editorFrame);
    m_editorFrame->SetMenuBar(menuBar);

    m_editorToolBar = (EditorToolBar *)OnCreateToolBar(m_editorFrame);
    m_editorControlList = new wxResourceEditorControlList(m_editorFrame, IDC_LISTCTRL, wxPoint(0, 0), wxSize(-1, -1));
    m_editorResourceTree = new wxResourceEditorProjectTree(m_editorFrame, IDC_TREECTRL, wxPoint(0, 0), wxSize(-1, -1),
      wxTR_HAS_BUTTONS);
    m_editorPanel = OnCreateEditorPanel(m_editorFrame);

    m_editorResourceTree->SetImageList(& m_imageList);

    // Constraints for toolbar
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs       (m_editorFrame, wxLeft, 0);
    c->top.SameAs        (m_editorFrame, wxTop, 0);
    c->right.SameAs      (m_editorFrame, wxRight, 0);
    c->bottom.Unconstrained();
    c->width.Unconstrained();
    c->height.Absolute(28);
    m_editorToolBar->SetConstraints(c);

    // Constraints for listbox
    c = new wxLayoutConstraints;
    c->left.SameAs       (m_editorFrame, wxLeft, 0);
    c->top.SameAs        (m_editorToolBar, wxBottom, 0);
    c->right.Absolute    (150);
    c->bottom.SameAs     (m_editorControlList, wxTop, 0);
    c->width.Unconstrained();
    c->height.Unconstrained();
    m_editorResourceTree->SetConstraints(c);

    // Constraints for panel
    c = new wxLayoutConstraints;
    c->left.SameAs       (m_editorResourceTree, wxRight, 0);
    c->top.SameAs        (m_editorToolBar, wxBottom, 0);
    c->right.SameAs      (m_editorFrame, wxRight, 0);
    c->bottom.SameAs     (m_editorControlList, wxTop, 0);
    c->width.Unconstrained();
    c->height.Unconstrained();
    m_editorPanel->SetConstraints(c);

    // Constraints for control list (bottom window)
    c = new wxLayoutConstraints;
    c->left.SameAs       (m_editorFrame, wxLeft, 0);
    c->right.SameAs      (m_editorFrame, wxRight, 0);
    c->bottom.SameAs     (m_editorFrame, wxBottom, 0);
    c->width.Unconstrained();
#ifdef __WXGTK__
    c->height.Absolute(140);
#else
    c->height.Absolute(60);
#endif

    m_editorControlList->SetConstraints(c);

    m_editorFrame->SetAutoLayout(TRUE);

    UpdateResourceList();

    m_editorFrame->Show(TRUE);
    return TRUE;
  }
  else
  {
    wxFrame *fr = m_editorFrame;
    if (m_editorFrame->OnClose())
    {
      fr->Show(FALSE);
      delete fr;
      m_editorFrame = NULL;
      m_editorPanel = NULL;
    }
  }
  return TRUE;
}

void wxResourceManager::SetFrameTitle(const wxString& filename)
{
  if (m_editorFrame)
  {
    if (filename == wxString(""))
      m_editorFrame->SetTitle("wxWindows Dialog Editor - untitled");
    else
    {
      wxString str("wxWindows Dialog Editor - ");
      wxString str2(wxFileNameFromPath(WXSTRINGCAST filename));
      str += str2;
      m_editorFrame->SetTitle(str);
    }
  }
}

bool wxResourceManager::Save()
{
  if (m_currentFilename == wxString(""))
    return SaveAs();
  else
    return Save(m_currentFilename);
}

bool wxResourceManager::Save(const wxString& filename)
{
  // Ensure all visible windows are saved to their resources
  m_currentFilename = filename;
  SetFrameTitle(m_currentFilename);
  InstantiateAllResourcesFromWindows();
  if (m_resourceTable.Save(filename))
  {
    m_symbolTable.WriteIncludeFile(m_symbolFilename);
    Modify(FALSE);
    return TRUE;
  }
  else
    return FALSE;
}

bool wxResourceManager::SaveAs()
{
  wxString s(wxFileSelector("Save resource file", wxPathOnly(WXSTRINGCAST m_currentFilename), wxFileNameFromPath(WXSTRINGCAST m_currentFilename),
    "wxr", "*.wxr", wxSAVE | wxOVERWRITE_PROMPT));
    
  if (s.IsNull() || s == "")
    return FALSE;
    
  m_currentFilename = s;
  wxStripExtension(m_currentFilename);
  m_currentFilename += ".wxr";

  // Construct include filename from this file
  m_symbolFilename = m_currentFilename;

  wxStripExtension(m_symbolFilename);
  m_symbolFilename += ".h";

  Save(m_currentFilename);
  return TRUE;
}

bool wxResourceManager::SaveIfModified()
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
    
  m_symbolTable.AddStandardSymbols();

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
    
    if (!m_resourceTable.ParseResourceFile(WXSTRINGCAST str))
    {
      wxMessageBox("Could not read file.", "Resource file load error", wxOK | wxICON_EXCLAMATION);
      return FALSE;
    }
    m_currentFilename = str;

    SetFrameTitle(m_currentFilename);

    UpdateResourceList();

    // Construct include filename from this file
    m_symbolFilename = m_currentFilename;

    wxStripExtension(m_symbolFilename);
    m_symbolFilename += ".h";

    if (!m_symbolTable.ReadIncludeFile(m_symbolFilename))
    {
        wxString str("Could not find include file ");
        str += m_symbolFilename;
        str += ".\nDialog Editor maintains a header file containing id symbols to be used in the application.\n";
        str += "The next time this .wxr file is saved, a header file will be saved also.";
        wxMessageBox(str, "Dialog Editor Warning", wxOK );

        m_symbolIdCounter = 99;
    }
    else
    {
        // Set the id counter to the last known id
        m_symbolIdCounter = m_symbolTable.FindHighestId();
    }

    // Now check in case some (or all) resources don't have resource ids, or they
    // don't match the .h file, or something of that nature.
    bool altered = RepairResourceIds();
    if (altered)
    {
        wxMessageBox("Some resources have had new identifiers associated with them, since they were missing.", 
	             "Dialog Editor Warning", wxOK );
        Modify(TRUE);
    }
    else
        Modify(FALSE);

    return TRUE;
  }
  else
  {
    SetFrameTitle("");
    m_currentFilename = "";
  }
  Modify(FALSE);
  
  return TRUE;
}

bool wxResourceManager::Clear(bool WXUNUSED(deleteWindows), bool force)
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
  
  ClearCurrentDialog();
  DisassociateWindows();

  m_symbolTable.Clear();
  m_resourceTable.ClearTable();
  UpdateResourceList();

  return TRUE;
}

bool wxResourceManager::DisassociateWindows()
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    DisassociateResource(res);
  }
  
  return TRUE;
}

void wxResourceManager::AssociateResource(wxItemResource *resource, wxWindow *win)
{
  if (!m_resourceAssociations.Get((long)resource))
    m_resourceAssociations.Put((long)resource, win);

  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    wxWindow *childWindow = (wxWindow *)m_resourceAssociations.Get((long)child);
    if (!childWindow)
      childWindow = win->FindWindow(child->GetName());
    if (childWindow)
      AssociateResource(child, childWindow);
    else
    {
      char buf[200];
      sprintf(buf, "AssociateResource: cannot find child window %s", child->GetName() ? (const char*) child->GetName() : "(unnamed)");
      wxMessageBox(buf, "Dialog Editor problem", wxOK);
    }

    node = node->Next();
  }
}

bool wxResourceManager::DisassociateResource(wxItemResource *resource)
{
  wxWindow *win = FindWindowForResource(resource);
  if (!win)
    return FALSE;

  // Disassociate children of window
  wxNode *node = win->GetChildren().First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if (child->IsKindOf(CLASSINFO(wxControl)))
  	    DisassociateResource(child);
    node = node->Next();
  }

  RemoveSelection(win);
  m_resourceAssociations.Delete((long)resource);
  return TRUE;
}

bool wxResourceManager::DisassociateResource(wxWindow *win)
{
  wxItemResource *res = FindResourceForWindow(win);
  if (res)
    return DisassociateResource(res);
  else
    return FALSE;
}

// Saves the window info into the resource, and deletes the
// handler. Doesn't actually disassociate the window from
// the resources. Replaces OnClose.
bool wxResourceManager::SaveInfoAndDeleteHandler(wxWindow* win)
{
    wxItemResource *res = FindResourceForWindow(win);

    if (win->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxResourceEditorDialogHandler* handler = (wxResourceEditorDialogHandler*) win->GetEventHandler();
        win->PopEventHandler();

        // Now reset all child event handlers
        wxNode *node = win->GetChildren().First();
        while ( node )
        {
	        wxWindow *child = (wxWindow *)node->Data();
	        wxEvtHandler *childHandler = child->GetEventHandler();
	        if ( child->IsKindOf(CLASSINFO(wxControl)) && childHandler != child )
	        {
		        child->PopEventHandler(TRUE);
	        }
	        node = node->Next();
        }
        delete handler;
    }
    else
    {
        win->PopEventHandler(TRUE);
    }
  
    // Save the information
    InstantiateResourceFromWindow(res, win, TRUE);
  
//  DisassociateResource(win);

    return TRUE;
}

// Destroys the window. If this is the 'current' panel, NULLs the
// variable.
bool wxResourceManager::DeleteWindow(wxWindow* win)
{
    bool clearDisplay = FALSE;
    if (m_editorPanel->m_childWindow == win)
    {
        m_editorPanel->m_childWindow = NULL;
        clearDisplay = TRUE;
    }

    win->Destroy();

    if (clearDisplay)
        m_editorPanel->Clear();

    return TRUE;
}

wxItemResource *wxResourceManager::FindResourceForWindow(wxWindow *win)
{
  m_resourceAssociations.BeginFind();
  wxNode *node;
  while ((node = m_resourceAssociations.Next()))
  {
    wxWindow *w = (wxWindow *)node->Data();
    if (w == win)
    {
      return (wxItemResource *)node->GetKeyInteger();
    }
  }
  return NULL;
}

wxWindow *wxResourceManager::FindWindowForResource(wxItemResource *resource)
{
  return (wxWindow *)m_resourceAssociations.Get((long)resource);
}


void wxResourceManager::MakeUniqueName(char *prefix, char *buf)
{
  while (TRUE)
  {
    sprintf(buf, "%s%d", prefix, m_nameCounter);
    m_nameCounter ++;

    if (!m_resourceTable.FindResource(buf))
      return;
  }
}

wxFrame *wxResourceManager::OnCreateEditorFrame(const char *title)
{
  /*
  int frameWidth = 420;
  int frameHeight = 300;
  */
  
  wxResourceEditorFrame *frame = new wxResourceEditorFrame(this, NULL, title,
    wxPoint(m_resourceEditorWindowSize.x, m_resourceEditorWindowSize.y),
    wxSize(m_resourceEditorWindowSize.width, m_resourceEditorWindowSize.height),
    wxDEFAULT_FRAME_STYLE);

  frame->CreateStatusBar(1);

  frame->SetAutoLayout(TRUE);
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("DIALOGEDICON"));
#endif
  return frame;
}

wxMenuBar *wxResourceManager::OnCreateEditorMenuBar(wxFrame *WXUNUSED(parent))
{
  wxMenuBar *menuBar = new wxMenuBar;

  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(RESED_NEW_DIALOG, "New &dialog", "Create a new dialog");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_NEW, "&New project",           "Clear the current project");
  fileMenu->Append(wxID_OPEN, "&Open...",         "Load a resource file");
  fileMenu->Append(wxID_SAVE, "&Save",            "Save a resource file");
  fileMenu->Append(wxID_SAVEAS, "Save &As...",   "Save a resource file as...");
  fileMenu->Append(RESED_CLEAR, "&Clear",   "Clear current resources");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "E&xit",            "Exit resource editor");

  wxMenu *editMenu = new wxMenu;
  editMenu->Append(RESED_TEST, "&Test Dialog",  "Test dialog");
  editMenu->Append(RESED_RECREATE, "&Recreate",  "Recreate the selected resource(s)");
  editMenu->Append(RESED_DELETE, "&Delete",  "Delete the selected resource(s)");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(RESED_CONTENTS, "&Help topics",          "Invokes the on-line help");
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT, "&About",          "About wxWindows Dialog Editor");

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(editMenu, "&Edit");
  menuBar->Append(helpMenu, "&Help");

  return menuBar;
}

wxResourceEditorScrolledWindow *wxResourceManager::OnCreateEditorPanel(wxFrame *parent)
{
  wxResourceEditorScrolledWindow *panel = new wxResourceEditorScrolledWindow(parent, wxDefaultPosition, wxDefaultSize,
//    wxSUNKEN_BORDER|wxCLIP_CHILDREN);
    wxSUNKEN_BORDER);

  panel->SetScrollbars(10, 10, 100, 100);

  return panel;
}

wxToolBar *wxResourceManager::OnCreateToolBar(wxFrame *parent)
{
  // Load palette bitmaps
#ifdef __WXMSW__
  wxBitmap ToolbarLoadBitmap("LOADTOOL");
  wxBitmap ToolbarSaveBitmap("SAVETOOL");
  wxBitmap ToolbarNewBitmap("NEWTOOL");
  wxBitmap ToolbarVertBitmap("VERTTOOL");
  wxBitmap ToolbarAlignTBitmap("ALIGNTTOOL");
  wxBitmap ToolbarAlignBBitmap("ALIGNBTOOL");
  wxBitmap ToolbarHorizBitmap("HORIZTOOL");
  wxBitmap ToolbarAlignLBitmap("ALIGNLTOOL");
  wxBitmap ToolbarAlignRBitmap("ALIGNRTOOL");
  wxBitmap ToolbarCopySizeBitmap("COPYSIZETOOL");
  wxBitmap ToolbarToBackBitmap("TOBACKTOOL");
  wxBitmap ToolbarToFrontBitmap("TOFRONTTOOL");
  wxBitmap ToolbarHelpBitmap("HELPTOOL");
#endif
#ifdef __WXGTK__
  wxBitmap ToolbarLoadBitmap( load_xpm );
  wxBitmap ToolbarSaveBitmap( save_xpm);
  wxBitmap ToolbarNewBitmap( new_xpm );
  wxBitmap ToolbarVertBitmap( vert_xpm );
  wxBitmap ToolbarAlignTBitmap( alignt_xpm );
  wxBitmap ToolbarAlignBBitmap( alignb_xpm );
  wxBitmap ToolbarHorizBitmap( horiz_xpm );
  wxBitmap ToolbarAlignLBitmap( alignl_xpm );
  wxBitmap ToolbarAlignRBitmap( alignr_xpm );
  wxBitmap ToolbarCopySizeBitmap( copysize_xpm );
  wxBitmap ToolbarToBackBitmap( toback_xpm );
  wxBitmap ToolbarToFrontBitmap( tofront_xpm );
  wxBitmap ToolbarHelpBitmap( help_xpm );
#endif

  // Create the toolbar
  EditorToolBar *toolbar = new EditorToolBar(parent, wxPoint(0, 0), wxSize(-1, -1), wxNO_BORDER|wxTB_HORIZONTAL);
  toolbar->SetMargins(2, 2);

#ifdef __WXMSW__
  int width = 24;
  int dx = 2;
  int gap = 6;
#else
  int width = 24; // ToolbarLoadBitmap->GetWidth();  ???
  int dx = 2;
  int gap = 6;
#endif
  int currentX = gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_NEW, ToolbarNewBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "New dialog");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_LOAD_FILE, ToolbarLoadBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Load");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_SAVE_FILE, ToolbarSaveBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Save");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ, ToolbarVertBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Horizontal align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_TOP_ALIGN, ToolbarAlignTBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Top align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_BOT_ALIGN, ToolbarAlignBBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Bottom align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT, ToolbarHorizBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Vertical align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN, ToolbarAlignLBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Left align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN, ToolbarAlignRBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Right align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_COPY_SIZE, ToolbarCopySizeBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Copy size");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_TO_FRONT, ToolbarToFrontBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "To front");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_TO_BACK, ToolbarToBackBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "To back");
  currentX += width + dx + gap;

  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_HELP, ToolbarHelpBitmap, wxNullBitmap,
                   FALSE, (float)currentX, -1, NULL, "Help");
  currentX += width + dx;
  
  toolbar->Realize();

  return toolbar;
}

void wxResourceManager::UpdateResourceList()
{
  if (!m_editorResourceTree)
      return;

  m_editorResourceTree->SetInvalid(TRUE);
  m_editorResourceTree->DeleteAllItems();

  long id = m_editorResourceTree->AddRoot("Dialogs", 1, 2);

  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel" || resType == "wxBitmap")
    {
      AddItemsRecursively(id, res);
    }
  }
  m_editorResourceTree->Expand(id);
  m_editorResourceTree->SetInvalid(FALSE);
}

void wxResourceManager::AddItemsRecursively(long parent, wxItemResource *resource)
{
  wxString theString("");
  theString = resource->GetName();

  int imageId = 0;
  wxString resType(resource->GetType());
  if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel")
    imageId = 0;
  else
    imageId = 3;
  
  long id = m_editorResourceTree->AppendItem(parent, theString, imageId );

  m_editorResourceTree->SetItemData(id, new wxResourceTreeData(resource));

  if (strcmp(resource->GetType(), "wxBitmap") != 0)
  {
    wxNode *node = resource->GetChildren().First();
    while (node)
    {
      wxItemResource *res = (wxItemResource *)node->Data();
      AddItemsRecursively(id, res);
      node = node->Next();
    }
  }
//  m_editorResourceTree->ExpandItem(id, wxTREE_EXPAND_EXPAND);
}

bool wxResourceManager::EditSelectedResource()
{
  int sel = m_editorResourceTree->GetSelection();
  if (sel != 0)
  {
    wxResourceTreeData *data = (wxResourceTreeData *)m_editorResourceTree->GetItemData(sel);
    wxItemResource *res = data->GetResource();
    return Edit(res);
  }
  return FALSE;
}

bool wxResourceManager::Edit(wxItemResource *res)
{
  ClearCurrentDialog();

  wxString resType(res->GetType());
  wxPanel *panel = (wxPanel *)FindWindowForResource(res);

  if (panel)
  {
    wxMessageBox("Should not find panel in wxResourceManager::Edit");
    return FALSE;
  }
  else
  {
//        long style = res->GetStyle();
//        res->SetStyle(style|wxRAISED_BORDER);
        panel = new wxPanel;
        wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, res, panel->GetEventHandler(),
           this);

        panel->LoadFromResource(m_editorPanel, res->GetName(), &m_resourceTable);

        panel->PushEventHandler(handler);

//        res->SetStyle(style);
		handler->AddChildHandlers(); // Add event handlers for all controls
        AssociateResource(res, panel);

        m_editorPanel->m_childWindow = panel;
        panel->Move(m_editorPanel->GetMarginX(), m_editorPanel->GetMarginY());
        panel->Show(TRUE);
        panel->Refresh();

        wxClientDC dc(m_editorPanel);
        m_editorPanel->DrawTitle(dc);
  }
  return FALSE;
}

bool wxResourceManager::CreateNewPanel()
{
  ClearCurrentDialog();

  char buf[256];
  MakeUniqueName("dialog", buf);
  
  wxItemResource *resource = new wxItemResource;
  resource->SetType("wxDialog");
  resource->SetName(buf);
  resource->SetTitle(buf);
  resource->SetResourceStyle(wxRESOURCE_USE_DEFAULTS);
  resource->SetResourceStyle(wxRESOURCE_DIALOG_UNITS);

  wxString newIdName;
  int id = GenerateWindowId("ID_DIALOG", newIdName);
  resource->SetId(id);

  // This is now guaranteed to be unique, so just add to symbol table
  m_symbolTable.AddSymbol(newIdName, id);

  m_resourceTable.AddResource(resource);

  wxSize size(400, 300);

  wxPanel *panel = new wxPanel(m_editorPanel, -1,
     wxPoint(m_editorPanel->GetMarginX(), m_editorPanel->GetMarginY()),
     size, wxRAISED_BORDER|wxDEFAULT_DIALOG_STYLE, buf);
  m_editorPanel->m_childWindow = panel;

  resource->SetStyle(panel->GetWindowStyleFlag());

  // Store dialog units in resource
  size = panel->ConvertPixelsToDialog(size);

  resource->SetSize(10, 10, size.x, size.y);

  // For editing in situ we will need to use the hash table to ensure
  // we don't dereference invalid pointers.
//  resourceWindowTable.Put((long)resource, panel);

  wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, resource, panel->GetEventHandler(),
   this);
  panel->PushEventHandler(handler);
  
  AssociateResource(resource, panel);
  UpdateResourceList();
  
  Modify(TRUE);
  m_editorPanel->m_childWindow->Refresh();

//  panel->Refresh();

  wxClientDC dc(m_editorPanel);
  m_editorPanel->DrawTitle(dc);

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

  if ((panelResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
  {
    wxPoint pt = panel->ConvertPixelsToDialog(wxPoint(x, y));
    res->SetSize(pt.x, pt.y, -1, -1);
  }
  else res->SetSize(x, y, -1, -1);

  res->SetType(iType);

  wxString prefix;
  
  wxString itemType(iType);

  if (itemType == "wxButton")
    {
      prefix = "ID_BUTTON";
      MakeUniqueName("button", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxBitmapButton(panel, -1, * m_bitmapImage, wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
      else
        newItem = new wxButton(panel, -1, "Button", wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  if (itemType == "wxBitmapButton")
    {
      prefix = "ID_BITMAPBUTTON";
      MakeUniqueName("button", buf);
      res->SetName(buf);
      newItem = new wxBitmapButton(panel, -1, * m_bitmapImage, wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxMessage" || itemType == "wxStaticText")
    {
      prefix = "ID_STATIC";
      MakeUniqueName("statictext", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxStaticBitmap(panel, -1, * m_bitmapImage, wxPoint(x, y), wxSize(0, 0), 0, buf);
      else
        newItem = new wxStaticText(panel, -1, "Static", wxPoint(x, y), wxSize(-1, -1), 0, buf);
    }
  else if (itemType == "wxStaticBitmap")
    {
      prefix = "ID_STATICBITMAP";
      MakeUniqueName("static", buf);
      res->SetName(buf);
      newItem = new wxStaticBitmap(panel, -1, * m_bitmapImage, wxPoint(x, y), wxSize(-1, -1), 0, buf);
    }
  else if (itemType == "wxCheckBox")
    {
      prefix = "ID_CHECKBOX";
      MakeUniqueName("checkbox", buf);
      res->SetName(buf);
      newItem = new wxCheckBox(panel, -1, "Checkbox", wxPoint(x, y), wxSize(-1, -1), 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxListBox")
    {
      prefix = "ID_LISTBOX";
      MakeUniqueName("listbox", buf);
      res->SetName(buf);
      newItem = new wxListBox(panel, -1, wxPoint(x, y), wxSize(-1, -1), 0, NULL, 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxRadioBox")
    {
      prefix = "ID_RADIOBOX";
      MakeUniqueName("radiobox", buf);
      res->SetName(buf);
      wxString names[] = { "One", "Two" };
      newItem = new wxRadioBox(panel, -1, "Radiobox", wxPoint(x, y), wxSize(-1, -1), 2, names, 2,
	     wxHORIZONTAL, wxDefaultValidator, buf);
      res->SetStringValues(wxStringList("One", "Two", NULL));
    }
  else if (itemType == "wxRadioButton")
    {
      prefix = "ID_RADIOBUTTON";
      MakeUniqueName("radiobutton", buf);
      res->SetName(buf);
      wxString names[] = { "One", "Two" };
      newItem = new wxRadioButton(panel, -1, "Radiobutton", wxPoint(x, y), wxSize(-1, -1),
	     0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxChoice")
    {
      prefix = "ID_CHOICE";
      MakeUniqueName("choice", buf);
      res->SetName(buf);
      newItem = new wxChoice(panel, -1, wxPoint(x, y), wxSize(-1, -1), 0, NULL, 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxComboBox")
    {
      prefix = "ID_COMBOBOX";
      MakeUniqueName("combobox", buf);
      res->SetName(buf);
      newItem = new wxComboBox(panel, -1, "", wxPoint(x, y), wxSize(-1, -1), 0, NULL, wxCB_DROPDOWN, wxDefaultValidator, buf);
    }
  else if (itemType == "wxGroupBox" || itemType == "wxStaticBox")
    {
      prefix = "ID_STATICBOX";
      MakeUniqueName("staticbox", buf);
      res->SetName(buf);
      newItem = new wxStaticBox(panel, -1, "Static", wxPoint(x, y), wxSize(200, 200), 0, buf);
    }
  else if (itemType == "wxGauge")
    {
      prefix = "ID_GAUGE";
      MakeUniqueName("gauge", buf);
      res->SetName(buf);
      newItem = new wxGauge(panel, -1, 10, wxPoint(x, y), wxSize(80, 30), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  else if (itemType == "wxSlider")
    {
      prefix = "ID_SLIDER";
      MakeUniqueName("slider", buf);
      res->SetName(buf);
      newItem = new wxSlider(panel, -1, 1, 1, 10, wxPoint(x, y), wxSize(120, -1), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  else if (itemType == "wxText" || itemType == "wxTextCtrl (single-line)")
    {
      prefix = "ID_TEXTCTRL";
      MakeUniqueName("textctrl", buf);
      res->SetName(buf);
      res->SetType("wxTextCtrl");
      newItem = new wxTextCtrl(panel, -1, "", wxPoint(x, y), wxSize(120, -1), 0, wxDefaultValidator, buf);
    }
  else if (itemType == "wxMultiText" || itemType == "wxTextCtrl (multi-line)")
    {
      prefix = "ID_TEXTCTRL";
      MakeUniqueName("textctrl", buf);
      res->SetName(buf);
      res->SetType("wxTextCtrl");
      newItem = new wxTextCtrl(panel, -1, "", wxPoint(x, y), wxSize(120, 100), wxTE_MULTILINE, wxDefaultValidator, buf);
    }
  else if (itemType == "wxScrollBar")
    {
      prefix = "ID_SCROLLBAR";
      MakeUniqueName("scrollbar", buf);
      res->SetName(buf);
      newItem = new wxScrollBar(panel, -1, wxPoint(x, y), wxSize(140, -1), wxHORIZONTAL, wxDefaultValidator, buf);
    }
  if (!newItem)
    return FALSE;

  int actualW, actualH;
  newItem->GetSize(&actualW, &actualH);
  wxSize actualSize(actualW, actualH);

  if ((panelResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
  {
    actualSize = panel->ConvertPixelsToDialog(actualSize);
  }
  res->SetSize(res->GetX(), res->GetY(), actualSize.x, actualSize.y);

  wxString newIdName;
  int id = GenerateWindowId(prefix, newIdName);
  res->SetId(id);

  // This is now guaranteed to be unique, so just add to symbol table
  m_symbolTable.AddSymbol(newIdName, id);

  newItem->PushEventHandler(new wxResourceEditorControlHandler(newItem, newItem));

  res->SetStyle(newItem->GetWindowStyleFlag());
  AssociateResource(res, newItem);
  panelResource->GetChildren().Append(res);

  UpdateResourceList();
  
  return TRUE;
}

void wxResourceManager::ClearCurrentDialog()
{
  if (m_editorPanel->m_childWindow)
  {
    SaveInfoAndDeleteHandler(m_editorPanel->m_childWindow);
    DisassociateResource(m_editorPanel->m_childWindow);
    DeleteWindow(m_editorPanel->m_childWindow);
    m_editorPanel->m_childWindow = NULL;
    m_editorPanel->Clear();
  }
}

bool wxResourceManager::TestCurrentDialog(wxWindow* parent)
{
  if (m_editorPanel->m_childWindow)
  {
    wxItemResource* item = FindResourceForWindow(m_editorPanel->m_childWindow);
    if (!item)
        return FALSE;

    // Make sure the resources are up-to-date w.r.t. the window
    InstantiateResourceFromWindow(item, m_editorPanel->m_childWindow, TRUE);

    wxDialog* dialog = new wxDialog;
    bool success = FALSE;
    if (dialog->LoadFromResource(parent, item->GetName(), & m_resourceTable))
    {
        dialog->Centre();
        dialog->ShowModal();
        success = TRUE;
    }
    return success;
  }
  return FALSE;
}

// Find the first dialog or panel for which
// there is a selected panel item.
wxWindow *wxResourceManager::FindParentOfSelection()
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxWindow *win = FindWindowForResource(res);
    if (win)
    {
      wxNode *node1 = win->GetChildren().First();
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

  while ((node = node->Next()))
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
void wxResourceManager::CopySize()
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

  while ((node = node->Next()))
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
      win->GetChildren().DeleteObject(item);
      if (winResource)
        winResource->GetChildren().DeleteObject(itemResource);
      if (toBack)
      {
        win->GetChildren().Insert(item);
        if (winResource)
          winResource->GetChildren().Insert(itemResource);
      }
      else
      {
        win->GetChildren().Append(item);
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
  if (!m_selections.Member(win))
    m_selections.Append(win);
}

void wxResourceManager::RemoveSelection(wxWindow *win)
{
  m_selections.DeleteObject(win);
}

// Need to search through resource table removing this from
// any resource which has this as a parent.
bool wxResourceManager::RemoveResourceFromParent(wxItemResource *res)
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
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

/* shouldn't have to do this now bitmaps are ref-counted
  if ((resType == "wxMessage" || resType == "wxStaticBitmap" || resType == "wxButton" || resType == "wxBitmapButton") && res->GetValue4())
  {
    PossiblyDeleteBitmapResource(res->GetValue4());
  }
*/

  // Remove symbol from table if appropriate
  if (!IsSymbolUsed(res, res->GetId()))
  {
    m_symbolTable.RemoveSymbol(res->GetId());
  }

  m_resourceTable.Delete(res->GetName());
  delete res;
  Modify(TRUE);
  return TRUE;
}

bool wxResourceManager::DeleteResource(wxWindow *win)
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
#ifndef __WXGTK__
      item->GetParent()->Refresh();
#endif
    }
  }
  
  wxItemResource *res = FindResourceForWindow(win);
  
  DisassociateResource(res);
  DeleteResource(res);
  UpdateResourceList();

  return TRUE;
}

// Will eventually have bitmap type information, for different
// kinds of bitmap.
wxString wxResourceManager::AddBitmapResource(const wxString& filename)
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

    m_resourceTable.AddResource(resource);

    UpdateResourceList();
  }
  if (resource)
    return resource->GetName();
  else
    return wxEmptyString;
}

 // Delete the bitmap resource if it isn't being used by another resource.
void wxResourceManager::PossiblyDeleteBitmapResource(const wxString& resourceName)
{
  if (!IsBitmapResourceUsed(resourceName))
  {
    wxItemResource *res = m_resourceTable.FindResource(resourceName);
    DeleteResource(res);
    UpdateResourceList();
  }
}

bool wxResourceManager::IsBitmapResourceUsed(const wxString& resourceName)
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
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
wxString wxResourceManager::FindBitmapFilenameForResource(wxItemResource *resource)
{
  if (!resource || (resource->GetValue4() == ""))
    return wxEmptyString;
  wxItemResource *bitmapResource = m_resourceTable.FindResource(resource->GetValue4());
  if (!bitmapResource)
    return wxEmptyString;

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
  return wxEmptyString;
}

wxItemResource *wxResourceManager::FindBitmapResourceByFilename(const wxString& filename)
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
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

// Is this window identifier symbol in use?
// Let's assume that we can't have 2 names for the same integer id.
// Therefore we can tell by the integer id whether the symbol is
// in use.
bool wxResourceManager::IsSymbolUsed(wxItemResource* thisResource, wxWindowID id)
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();

    wxString resType(res->GetType());
    if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel")
    {
      if ((res != thisResource) && (res->GetId() == id))
          return TRUE;

      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        if ((child != thisResource) && (child->GetId() == id))
          return TRUE;
        node1 = node1->Next();
      }
    }
  }
  return FALSE;
}

// Is this window identifier compatible with the given name? (i.e.
// does it already exist under a different name)
bool wxResourceManager::IsIdentifierOK(const wxString& name, wxWindowID id)
{
    if (m_symbolTable.SymbolExists(name))
    {
        int foundId = m_symbolTable.GetIdForSymbol(name);
        if (foundId != id)
            return FALSE;
    }
    return TRUE;
}

// Change all integer ids that match oldId, to newId.
// This is necessary if an id is changed for one resource - all resources
// must be changed.
void wxResourceManager::ChangeIds(int oldId, int newId)
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();

    wxString resType(res->GetType());
    if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel")
    {
      if (res->GetId() == oldId)
          res->SetId(newId);

      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        if (child->GetId() == oldId)
            child->SetId(newId);

        node1 = node1->Next();
      }
    }
  }
}

// If any resource ids were missing (or their symbol was missing),
// repair them i.e. give them new ids. Returns TRUE if any resource
// needed repairing.
bool wxResourceManager::RepairResourceIds()
{
  bool repaired = FALSE;

  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxDialog" || resType == "wxDialogBox" || resType == "wxPanel")
    {

      if ( (res->GetId() == 0) || ((res->GetId() > 0) && !m_symbolTable.IdExists(res->GetId())) )
      {
          wxString newSymbolName;
          int newId = GenerateWindowId("ID_DIALOG", newSymbolName) ;

          if (res->GetId() == 0)
          {
              res->SetId(newId);
              m_symbolTable.AddSymbol(newSymbolName, newId);
          }
          else
          {
              m_symbolTable.AddSymbol(newSymbolName, res->GetId());
          }

          repaired = TRUE;
      }

      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();

        if ( (child->GetId() == 0) || ((child->GetId() > 0) && !m_symbolTable.IdExists(child->GetId())) )
        {
            wxString newSymbolName;
            int newId = GenerateWindowId("ID_CONTROL", newSymbolName) ;

            if (child->GetId() == 0)
            {
                child->SetId(newId);
                m_symbolTable.AddSymbol(newSymbolName, newId);
            }
            else
            {
                m_symbolTable.AddSymbol(newSymbolName, child->GetId());
            }

            repaired = TRUE;
        }

        node1 = node1->Next();
      }
    }
  }
  return repaired;
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
    newInfo = CreatePropertyInfoForWindow(win);
    info = newInfo;
  }

  info->InstantiateResource(resource);

  wxWindow *newWin = NULL;
  wxWindow *parent = win->GetParent();
  wxItemResource* parentResource = NULL;
  if (parent)
    parentResource = FindResourceForWindow(parent);
  
  if (win->IsKindOf(CLASSINFO(wxPanel)))
  {
    Edit(resource);
    newWin = FindWindowForResource(resource);
  }
  else
  {
    DisassociateResource(resource);
    if (win->GetEventHandler() != win)
        win->PopEventHandler(TRUE);

    DeleteWindow(win);
    newWin = m_resourceTable.CreateItem((wxPanel *)parent, resource, parentResource);
    newWin->PushEventHandler(new wxResourceEditorControlHandler((wxControl*) newWin, (wxControl*) newWin));
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
bool wxResourceManager::DeleteSelection()
{
  int sel = m_editorResourceTree->GetSelection();
  if (sel != 0)
  {
    wxResourceTreeData *data = (wxResourceTreeData *)m_editorResourceTree->GetItemData(sel);
    wxItemResource *res = data->GetResource();
    wxWindow *win = FindWindowForResource(res);
    if (win)
    {
        DeleteResource(win);
        DeleteWindow(win);
        UpdateResourceList();
        Modify(TRUE);
    }
    return TRUE;
  }

  return FALSE;
}

// Delete resource highlighted in the listbox
bool wxResourceManager::RecreateSelection()
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

bool wxResourceManager::EditDialog(wxDialog *WXUNUSED(dialog), wxWindow *WXUNUSED(parent))
{
  return FALSE;
}

// Ensures that all currently shown windows are saved to resources,
// e.g. just before writing to a .wxr file.
bool wxResourceManager::InstantiateAllResourcesFromWindows()
{
  m_resourceTable.BeginFind();
  wxNode *node;
  while ((node = m_resourceTable.Next()))
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
  wxWindowPropertyInfo *info = CreatePropertyInfoForWindow(window);
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
        sprintf(buf, "Could not find window %s", (const char*) child->GetName());
        wxMessageBox(buf, "Dialog Editor problem", wxOK);
      }
      else
        InstantiateResourceFromWindow(child, childWindow, recurse);
      node = node->Next();
    }
  }
  
  return TRUE;
}

// Create a window information object for the give window
wxWindowPropertyInfo *wxResourceManager::CreatePropertyInfoForWindow(wxWindow *win)
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
  else if (win->IsKindOf(CLASSINFO(wxRadioButton)))
        {
          info = new wxRadioButtonPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxChoice)))
        {
          info = new wxChoicePropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxComboBox)))
        {
          info = new wxComboBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxButton)))
        {
          info = new wxButtonPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxBitmapButton)))
        {
          info = new wxBitmapButtonPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxStaticText)))
        {
          info = new wxStaticTextPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxStaticBitmap)))
        {
          info = new wxStaticBitmapPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxTextCtrl)))
        {
          info = new wxTextPropertyInfo(win);
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

// Edit the given window
void wxResourceManager::EditWindow(wxWindow *win)
{
  wxWindowPropertyInfo *info = CreatePropertyInfoForWindow(win);
  if (info)
  {
    info->SetResource(FindResourceForWindow(win));
    wxString str("Editing ");
    str += win->GetClassInfo()->GetClassName();
    str += ": ";
    if (win->GetName() != "")
        str += win->GetName();
    else
        str += "properties";
    info->Edit(NULL, str);
  }
}

// Generate a window id and a first stab at a name
int wxResourceManager::GenerateWindowId(const wxString& prefix, wxString& idName)
{
    m_symbolIdCounter ++;
    while (m_symbolTable.IdExists(m_symbolIdCounter))
        m_symbolIdCounter ++;

    int nameId = m_symbolIdCounter;

    wxString str;
    str.Printf("%d", nameId);
    idName = prefix + str;

    while (m_symbolTable.SymbolExists(idName))
    {
        nameId ++;
        str.Printf("%d", nameId);
        idName = prefix + str;
    }

    return m_symbolIdCounter;
}


/*
 * Resource editor frame
 */

IMPLEMENT_CLASS(wxResourceEditorFrame, wxFrame)

BEGIN_EVENT_TABLE(wxResourceEditorFrame, wxFrame)
    EVT_MENU(wxID_NEW, wxResourceEditorFrame::OnNew)
    EVT_MENU(RESED_NEW_DIALOG, wxResourceEditorFrame::OnNewDialog)
    EVT_MENU(wxID_OPEN, wxResourceEditorFrame::OnOpen)
    EVT_MENU(RESED_CLEAR, wxResourceEditorFrame::OnClear)
    EVT_MENU(wxID_SAVE, wxResourceEditorFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, wxResourceEditorFrame::OnSaveAs)
    EVT_MENU(wxID_EXIT, wxResourceEditorFrame::OnExit)
    EVT_MENU(wxID_ABOUT, wxResourceEditorFrame::OnAbout)
    EVT_MENU(RESED_CONTENTS, wxResourceEditorFrame::OnContents)
    EVT_MENU(RESED_DELETE, wxResourceEditorFrame::OnDeleteSelection)
    EVT_MENU(RESED_RECREATE, wxResourceEditorFrame::OnRecreateSelection)
    EVT_MENU(RESED_TEST, wxResourceEditorFrame::OnTest)
END_EVENT_TABLE()

wxResourceEditorFrame::wxResourceEditorFrame(wxResourceManager *resMan, wxFrame *parent, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxFrame(parent, -1, title, pos, size, style, name)
{
  manager = resMan;
}

wxResourceEditorFrame::~wxResourceEditorFrame()
{
}

void wxResourceEditorFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
      manager->New(FALSE);
}

void wxResourceEditorFrame::OnNewDialog(wxCommandEvent& WXUNUSED(event))
{
      manager->CreateNewPanel();
}

void wxResourceEditorFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
      manager->New(TRUE);
}

void wxResourceEditorFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
      manager->Clear(TRUE, FALSE);
}

void wxResourceEditorFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
      manager->Save();
}

void wxResourceEditorFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
      manager->SaveAs();
}

void wxResourceEditorFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	  manager->Clear(TRUE, FALSE) ;
      this->Close();
}

void wxResourceEditorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
      char buf[300];
      sprintf(buf, "wxWindows Dialog Editor %.1f\nAuthor: Julian Smart J.Smart@ed.ac.uk\nJulian Smart (c) 1996", wxDIALOG_EDITOR_VERSION);
      wxMessageBox(buf, "About Dialog Editor", wxOK|wxCENTRE);
}

void wxResourceEditorFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
    manager->TestCurrentDialog(this);
}

void wxResourceEditorFrame::OnContents(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
      wxBeginBusyCursor();
      manager->GetHelpController()->LoadFile();
      manager->GetHelpController()->DisplayContents();
      wxEndBusyCursor();
#endif
}

void wxResourceEditorFrame::OnDeleteSelection(wxCommandEvent& WXUNUSED(event))
{
      manager->DeleteSelection();
}

void wxResourceEditorFrame::OnRecreateSelection(wxCommandEvent& WXUNUSED(event))
{
      manager->RecreateSelection();
}

bool wxResourceEditorFrame::OnClose()
{
  if (manager->Modified())
  {
  if (!manager->Clear(TRUE, FALSE))
     return FALSE;
  }
    
  if (!Iconized())
  {
    int w, h;
    GetSize(&w, &h);
    manager->m_resourceEditorWindowSize.width = w;
    manager->m_resourceEditorWindowSize.height = h;

    int x, y;
    GetPosition(&x, &y);

    manager->m_resourceEditorWindowSize.x = x;
    manager->m_resourceEditorWindowSize.y = y;
  }
  manager->SetEditorFrame(NULL);
  manager->SetEditorToolBar(NULL);

  return TRUE;
}

/*
 * Resource editor window that contains the dialog/panel being edited
 */

BEGIN_EVENT_TABLE(wxResourceEditorScrolledWindow, wxScrolledWindow)
    EVT_SCROLL(wxResourceEditorScrolledWindow::OnScroll)
    EVT_PAINT(wxResourceEditorScrolledWindow::OnPaint)
END_EVENT_TABLE()

wxResourceEditorScrolledWindow::wxResourceEditorScrolledWindow(wxWindow *parent, const wxPoint& pos, const wxSize& size,
    long style):
  wxScrolledWindow(parent, -1, pos, size, style)
{
    m_marginX = 10;
    m_marginY = 40;
    m_childWindow = NULL;
}

wxResourceEditorScrolledWindow::~wxResourceEditorScrolledWindow()
{
}

void wxResourceEditorScrolledWindow::OnScroll(wxScrollEvent& event)
{
    wxScrolledWindow::OnScroll(event);

    int x, y;
    ViewStart(& x, & y);

    if (m_childWindow)
        m_childWindow->Move(m_marginX + (- x * 10), m_marginY + (- y * 10));
}

void wxResourceEditorScrolledWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    DrawTitle(dc);
}

void wxResourceEditorScrolledWindow::DrawTitle(wxDC& dc)
{
    if (m_childWindow)
    {
        wxItemResource* res = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_childWindow);
        if (res)
        {
            wxString str(res->GetTitle());
            int x, y;
            ViewStart(& x, & y);

            wxFont font(10, wxSWISS, wxNORMAL, wxBOLD);
            dc.SetFont(font);
            dc.SetBackgroundMode(wxTRANSPARENT);
            dc.SetTextForeground(wxColour(0, 0, 0));

            long w, h;
            dc.GetTextExtent(str, & w, & h);

            dc.DrawText(str, m_marginX + (- x * 10), m_marginY + (- y * 10) - h - 5);
        }
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
      wxResourceManager::GetCurrentResourceManager()->EditWindow(data);
      break;
    }
    case OBJECT_MENU_DELETE:
    {
      wxResourceManager::GetCurrentResourceManager()->SaveInfoAndDeleteHandler(data);
      wxResourceManager::GetCurrentResourceManager()->DeleteResource(data);
      wxResourceManager::GetCurrentResourceManager()->DeleteWindow(data);
      break;
    }
    default:
      break;
  }
}

/*
 * Main toolbar
 *
 */

#ifdef __WXGTK__   // I don't dare to delete it...
 
BEGIN_EVENT_TABLE(EditorToolBar, wxToolBar)
END_EVENT_TABLE()

#else

BEGIN_EVENT_TABLE(EditorToolBar, wxToolBar)
	EVT_PAINT(EditorToolBar::OnPaint)
END_EVENT_TABLE()

#endif

EditorToolBar::EditorToolBar(wxFrame *frame, const wxPoint& pos, const wxSize& size,
            long style):
  wxToolBar(frame, -1, pos, size, style)
{
}

bool EditorToolBar::OnLeftClick(int toolIndex, bool WXUNUSED(toggled))
{
  wxResourceManager *manager = wxResourceManager::GetCurrentResourceManager();

  switch (toolIndex)
  {
    case TOOLBAR_LOAD_FILE:
    {
      manager->New(TRUE);
      break;
    }
    case TOOLBAR_NEW:
    {
      manager->CreateNewPanel();
      break;
    }
    case TOOLBAR_SAVE_FILE:
    {
      manager->Save();
      break;
    }
    case TOOLBAR_HELP:
    {
#ifdef __WXMSW__
      wxBeginBusyCursor();
      manager->GetHelpController()->LoadFile();
      manager->GetHelpController()->DisplayContents();
      wxEndBusyCursor();
#endif
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

