/////////////////////////////////////////////////////////////////////////////
// Name:        Studio.cpp
// Purpose:     Studio application class
// Author:      Julian Smart
// Modified by:
// Created:     27/7/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/resource.h"
#include "wx/config.h"
#include "wx/laywin.h"

#include "studio.h"
#include "view.h"
#include "doc.h"
#include "mainfrm.h"
#include "cspalette.h"
#include "project.h"
#include "symbols.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/undo.xpm"
#include "bitmaps/redo.xpm"

#include "bitmaps/alignl.xpm"
#include "bitmaps/alignr.xpm"
#include "bitmaps/alignt.xpm"
#include "bitmaps/alignb.xpm"
#include "bitmaps/horiz.xpm"
#include "bitmaps/vert.xpm"
#include "bitmaps/copysize.xpm"
#include "bitmaps/linearrow.xpm"
#include "bitmaps/newpoint.xpm"
#include "bitmaps/cutpoint.xpm"
#include "bitmaps/straight.xpm"

#include "studio.xpm"
#endif

IMPLEMENT_APP(csApp)

csApp::csApp()
{
    m_docManager = NULL;
    m_diagramPalette = NULL;
    m_diagramToolBar = NULL;
    m_projectTreeCtrl = NULL;
    m_diagramPaletteSashWindow = NULL;
    m_projectSashWindow = NULL;
    m_symbolDatabase = NULL;
    m_pointSizeComboBox = NULL;
    m_zoomComboBox = NULL;
    m_shapeEditMenu = NULL;

    // Configuration
    m_mainFramePos.x = 20;
    m_mainFramePos.y = 20;
    m_mainFrameSize.x = 500;
    m_mainFrameSize.y = 400;
    m_gridStyle = csGRID_STYLE_INVISIBLE;
    m_gridSpacing = 5;
}

csApp::~csApp()
{
}

// Initialise this in OnInit, not statically
bool csApp::OnInit(void)
{
  if (!wxResourceParseFile("studio_resources.wxr"))
  {
    wxMessageBox("Could not find or parse resource file: studio_resources.wxr", "Studio");
    return FALSE;
  }

  m_helpController.Initialize("studio.hlp");

  ReadOptions();

  wxOGLInitialize();

  InitSymbols();

  //// Create a document manager
  m_docManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate(m_docManager, "Diagram", "*.dia", "", "dia", "Diagram Doc", "Diagram View",
          CLASSINFO(csDiagramDocument), CLASSINFO(csDiagramView));

  // Create the main frame window

  csFrame* frame = new csFrame(m_docManager, NULL, -1, "OGL Studio", m_mainFramePos, m_mainFrameSize,
   wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

  // Give it an icon
  frame->SetIcon(wxICON(studio));

  // Make a menubar
  wxMenu *fileMenu = new wxMenu;

  fileMenu->Append(wxID_NEW, "&New...\tCtrl+N");
  fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O");

  fileMenu->AppendSeparator();

  fileMenu->Append(wxID_PRINT, "&Print...\tCtrl+P");
  fileMenu->Append(wxID_PRINT_SETUP, "Print &Setup...");
  fileMenu->Append(wxID_PREVIEW, "Print Pre&view");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "E&xit");

  // A history of files visited. Use this menu.
  m_docManager->FileHistoryUseMenu(fileMenu);

  wxMenu *viewMenu = new wxMenu;
  viewMenu->Append(ID_CS_SETTINGS, "&Settings...");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, "&Help Contents\tF1");
  helpMenu->Append(ID_CS_ABOUT, "&About");

  wxMenuBar *menuBar = new wxMenuBar;

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(viewMenu, "&View");
  menuBar->Append(helpMenu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menuBar);

  // Load the file history
  wxConfig config("OGL Studio", "wxWindows");
  m_docManager->FileHistoryLoad(config);

  frame->CreateStatusBar();

  // The ordering of these is important for layout purposes
  CreateDiagramToolBar(frame);
  CreatePalette(frame);
  CreateProjectWindow(frame);

  FillProjectTreeCtrl();

  // Create the shape editing menu
  m_shapeEditMenu = new wxMenu("", (wxFunction)studioShapeEditProc);
  m_shapeEditMenu->Append(ID_CS_EDIT_PROPERTIES, "Edit properties");
  m_shapeEditMenu->AppendSeparator();
  m_shapeEditMenu->Append(ID_CS_ROTATE_CLOCKWISE, "Rotate clockwise");
  m_shapeEditMenu->Append(ID_CS_ROTATE_ANTICLOCKWISE, "Rotate anticlockwise");
  m_shapeEditMenu->AppendSeparator();
  m_shapeEditMenu->Append(ID_CS_CUT, "Cut");

  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

int csApp::OnExit(void)
{
    WriteOptions();

    delete m_symbolDatabase;
    m_symbolDatabase = NULL;

    delete m_docManager;
    m_docManager = NULL;

    delete m_shapeEditMenu;
    m_shapeEditMenu = NULL;

    wxOGLCleanUp();

    return 0;
}

/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created.
 */
 
wxMDIChildFrame *csApp::CreateChildFrame(wxDocument *doc, wxView *view, wxMenu** editMenuRet)
{
  //// Make a child frame
  csMDIChildFrame *subframe = new csMDIChildFrame(doc, view, ((wxDocMDIParentFrame*)GetTopWindow()), -1, "Child Frame",
        wxPoint(10, 10), wxSize(300, 300), wxDEFAULT_FRAME_STYLE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString("chart"));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon("doc.xbm"));
#endif

  //// Make a menubar
  wxMenu *fileMenu = new wxMenu;

  fileMenu->Append(wxID_NEW, "&New...\tCtrl+N");
  fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O");
  fileMenu->Append(wxID_CLOSE, "&Close\tCtrl+W");
  fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
  fileMenu->Append(wxID_SAVEAS, "Save &As...\tF12");

  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_PRINT, "&Print...\tCtrl+P");
  fileMenu->Append(wxID_PRINT_SETUP, "Print &Setup...");
  fileMenu->Append(wxID_PREVIEW, "Print Pre&view");

  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "E&xit");

  wxMenu *editMenu = NULL;

  editMenu = new wxMenu;
  editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z");
  editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y");
  editMenu->AppendSeparator();
  editMenu->Append(wxID_CUT, "Cu&t\tCtrl+X");
  editMenu->Append(wxID_COPY, "&Copy\tCtrl+C");
  editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V");
  editMenu->Append(wxID_DUPLICATE, "&Duplicate\tCtrl+D");
  editMenu->AppendSeparator();
  editMenu->Append(wxID_CLEAR, "Cle&ar\tDelete");
  editMenu->Append(ID_CS_SELECT_ALL, "Select A&ll\tCtrl+A");
  editMenu->AppendSeparator();
  editMenu->Append(ID_CS_EDIT_PROPERTIES, "Edit P&roperties...");

  *editMenuRet = editMenu;

  m_docManager->FileHistoryUseMenu(fileMenu);
  m_docManager->FileHistoryAddFilesToMenu(fileMenu);

  doc->GetCommandProcessor()->SetEditMenu(editMenu);

  wxMenu *viewMenu = new wxMenu;
  viewMenu->Append(ID_CS_SETTINGS, "&Settings...");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, "&Help Contents\tF1");
  helpMenu->Append(ID_CS_ABOUT, "&About");

  wxMenuBar *menuBar = new wxMenuBar;

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(editMenu, "&Edit");
  menuBar->Append(viewMenu, "&View");
  menuBar->Append(helpMenu, "&Help");

  //// Associate the menu bar with the frame
  subframe->SetMenuBar(menuBar);

  return subframe;
}

// Creates a canvas. Called by OnInit as a child of the main window
csCanvas *csApp::CreateCanvas(wxView *view, wxFrame *parent)
{
  int width, height;
  parent->GetClientSize(&width, &height);

  // Non-retained canvas
  csCanvas *canvas = new csCanvas((csDiagramView*) view, parent, 1000, wxPoint(0, 0), wxSize(width, height), 0);

  wxColour bgColour("WHITE");
  canvas->SetBackgroundColour(bgColour);

  wxCursor cursor(wxCURSOR_HAND);
  canvas->SetCursor(cursor);

  // Give it scrollbars
  canvas->SetScrollbars(20, 20, 100, 100);

  return canvas;
}

void csApp::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap* bitmaps[10];

#ifdef __WXMSW__
    bitmaps[0] = new wxBitmap("new", wxBITMAP_TYPE_RESOURCE);
    bitmaps[1] = new wxBitmap("open", wxBITMAP_TYPE_RESOURCE);
    bitmaps[2] = new wxBitmap("save", wxBITMAP_TYPE_RESOURCE);
    bitmaps[3] = new wxBitmap("copy", wxBITMAP_TYPE_RESOURCE);
    bitmaps[4] = new wxBitmap("cut", wxBITMAP_TYPE_RESOURCE);
    bitmaps[5] = new wxBitmap("paste", wxBITMAP_TYPE_RESOURCE);
    bitmaps[6] = new wxBitmap("print", wxBITMAP_TYPE_RESOURCE);
    bitmaps[7] = new wxBitmap("help", wxBITMAP_TYPE_RESOURCE);
    bitmaps[8] = new wxBitmap("undo", wxBITMAP_TYPE_RESOURCE);
    bitmaps[9] = new wxBitmap("redo", wxBITMAP_TYPE_RESOURCE);
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
    bitmaps[0] = new wxBitmap( new_xpm );
    bitmaps[1] = new wxBitmap( open_xpm );
    bitmaps[2] = new wxBitmap( save_xpm );
    bitmaps[3] = new wxBitmap( copy_xpm );
    bitmaps[4] = new wxBitmap( cut_xpm );
    bitmaps[5] = new wxBitmap( paste_xpm );
    bitmaps[6] = new wxBitmap( print_xpm );
    bitmaps[7] = new wxBitmap( help_xpm );
    bitmaps[8] = new wxBitmap( undo_xpm );
    bitmaps[9] = new wxBitmap( redo_xpm );
#else
#error "Not implemented for this platform."
#endif

  toolBar->AddTool(wxID_NEW, *bitmaps[0], wxNullBitmap, FALSE, -1, -1, NULL, "New file");
  toolBar->AddTool(wxID_OPEN, *bitmaps[1], wxNullBitmap, FALSE, -1, -1, NULL, "Open file");
  toolBar->AddTool(wxID_SAVE, *bitmaps[2], wxNullBitmap, FALSE, -1, -1, NULL, "Save file");
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_PRINT, *bitmaps[6], wxNullBitmap, FALSE, -1, -1, NULL, "Print");
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_COPY, *bitmaps[3], wxNullBitmap, FALSE, -1, -1, NULL, "Copy");
  toolBar->AddTool(wxID_CUT, *bitmaps[4], wxNullBitmap, FALSE, -1, -1, NULL, "Cut");
  toolBar->AddTool(wxID_PASTE, *bitmaps[5], wxNullBitmap, FALSE, -1, -1, NULL, "Paste");
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_UNDO, *bitmaps[8], wxNullBitmap, FALSE, -1, -1, NULL, "Undo");
  toolBar->AddTool(wxID_REDO, *bitmaps[9], wxNullBitmap, FALSE, -1, -1, NULL, "Redo");
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_HELP, *bitmaps[7], wxNullBitmap, FALSE, -1, -1, NULL, "Help");

  toolBar->Realize();

  toolBar->EnableTool(wxID_COPY, FALSE);
  toolBar->EnableTool(wxID_PASTE, FALSE);
  toolBar->EnableTool(wxID_PRINT, FALSE);
  toolBar->EnableTool(wxID_UNDO, FALSE);
  toolBar->EnableTool(wxID_REDO, FALSE);

  int i;
  for (i = 0; i < 10; i++)
    delete bitmaps[i];
}

// Create and initialise the diagram toolbar
void csApp::CreateDiagramToolBar(wxFrame* parent)
{
    // First create a layout window
    wxSashLayoutWindow* win = new wxSashLayoutWindow(parent, ID_LAYOUT_WINDOW_DIAGRAM_TOOLBAR, wxDefaultPosition, wxSize(200, 30), wxNO_BORDER|wxSW_3D|wxCLIP_CHILDREN);
    win->SetDefaultSize(wxSize(10000, 30));
    win->SetOrientation(wxLAYOUT_HORIZONTAL);
    win->SetAlignment(wxLAYOUT_TOP);
    win->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

    m_diagramToolBarSashWindow = win;
    m_diagramToolBarSashWindow->Show(FALSE);

    // Create the actual toolbar
    m_diagramToolBar = new wxToolBar(win, -1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER|wxTB_FLAT);

    wxBitmap* bitmaps[11];

#ifdef __WXMSW__
    bitmaps[0] = new wxBitmap("alignl", wxBITMAP_TYPE_RESOURCE);
    bitmaps[1] = new wxBitmap("alignr", wxBITMAP_TYPE_RESOURCE);
    bitmaps[2] = new wxBitmap("alignt", wxBITMAP_TYPE_RESOURCE);
    bitmaps[3] = new wxBitmap("alignb", wxBITMAP_TYPE_RESOURCE);
    bitmaps[4] = new wxBitmap("horiz", wxBITMAP_TYPE_RESOURCE);
    bitmaps[5] = new wxBitmap("vert", wxBITMAP_TYPE_RESOURCE);
    bitmaps[6] = new wxBitmap("copysize", wxBITMAP_TYPE_RESOURCE);
    bitmaps[7] = new wxBitmap("linearrow", wxBITMAP_TYPE_RESOURCE);
    bitmaps[8] = new wxBitmap("newpoint", wxBITMAP_TYPE_RESOURCE);
    bitmaps[9] = new wxBitmap("cutpoint", wxBITMAP_TYPE_RESOURCE);
    bitmaps[10] = new wxBitmap("straighten", wxBITMAP_TYPE_RESOURCE);
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
    bitmaps[0] = new wxBitmap( alignl_xpm );
    bitmaps[1] = new wxBitmap( alignr_xpm );
    bitmaps[2] = new wxBitmap( alignt_xpm );
    bitmaps[3] = new wxBitmap( alignb_xpm );
    bitmaps[4] = new wxBitmap( horiz_xpm );
    bitmaps[5] = new wxBitmap( vert_xpm );
    bitmaps[6] = new wxBitmap( copysize_xpm );
    bitmaps[7] = new wxBitmap( linearrow_xpm );
    bitmaps[8] = new wxBitmap( newpoint_xpm );
    bitmaps[9] = new wxBitmap( cutpoint_xpm );
    bitmaps[10] = new wxBitmap( straight_xpm );
#else
#error "Not implemented for this platform."
#endif

    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGNL, *bitmaps[0], wxNullBitmap, FALSE, -1, -1, NULL, "Align left");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGNR, *bitmaps[1], wxNullBitmap, FALSE, -1, -1, NULL, "Align right");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGNT, *bitmaps[2], wxNullBitmap, FALSE, -1, -1, NULL, "Align top");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGNB, *bitmaps[3], wxNullBitmap, FALSE, -1, -1, NULL, "Align bottom");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGN_HORIZ, *bitmaps[4], wxNullBitmap, FALSE, -1, -1, NULL, "Align horizontally");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_ALIGN_VERT, *bitmaps[5], wxNullBitmap, FALSE, -1, -1, NULL, "Align vertically");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_COPY_SIZE, *bitmaps[6], wxNullBitmap, FALSE, -1, -1, NULL, "Copy size");
    m_diagramToolBar->AddSeparator();
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_LINE_ARROW, *bitmaps[7], wxNullBitmap, TRUE, -1, -1, NULL, "Toggle arrow");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_NEW_POINT, *bitmaps[8], wxNullBitmap, FALSE, -1, -1, NULL, "New line point");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_CUT_POINT, *bitmaps[9], wxNullBitmap, FALSE, -1, -1, NULL, "Cut line point");
    m_diagramToolBar->AddTool(DIAGRAM_TOOLBAR_STRAIGHTEN, *bitmaps[10], wxNullBitmap, FALSE, -1, -1, NULL, "Straighten lines");

    m_diagramToolBar->Realize();

    int i;
    for (i = 0; i < 11; i++)
        delete bitmaps[i];

    // Create a combobox for point size
    int maxPointSize = 40;
    wxString *pointSizes = new wxString[maxPointSize];
    for (i = 1; i <= maxPointSize; i++)
    {
        pointSizes[i-1].Printf("%d", i);
    }

    int controlX = 260;
    int pointSizeW = 40;
    int pointSizeH = 18;
    int zoomW = 60;
    int zoomH = 18;
#ifdef __WXMOTIF__
    controlX += 70;
    pointSizeW = 60;
    pointSizeH = 22;
    zoomW = 60;
    zoomH = 22;
#endif

    m_pointSizeComboBox = new wxComboBox(m_diagramToolBar, ID_WINDOW_POINT_SIZE_COMBOBOX,
        "", wxPoint(controlX, 1), wxSize(pointSizeW, pointSizeH), maxPointSize, pointSizes);
    delete[] pointSizes;

    m_pointSizeComboBox->SetSelection(10 - 1);

    // Create a combobox for zooming
    int maxZoom = 200;
    int minZoom = 5;
    int increment = 5;
    int noStrings = (maxZoom - minZoom)/5 ;
    wxString *zoomStrings = new wxString[noStrings];
    for (i = 0; i < noStrings; i ++)
    {
        zoomStrings[noStrings - i - 1].Printf("%d%%", (i*increment + minZoom));
    }

    controlX += pointSizeW + 10;

    m_zoomComboBox = new wxComboBox(m_diagramToolBar, ID_WINDOW_ZOOM_COMBOBOX,
        "", wxPoint(controlX, 1), wxSize(zoomW, zoomH), noStrings, zoomStrings);
    delete[] zoomStrings;

    // i = (zoom - minZoom)/increment
    // index = noStrings - i - 1
    // 100%
    i = (100 - minZoom)/increment;
    m_zoomComboBox->SetSelection(noStrings - i - 1);
}

// Read/write configuration information
bool csApp::ReadOptions()
{
    wxConfig config("OGL Studio", "wxWindows");

    config.Read("mainX", & m_mainFramePos.x);
    config.Read("mainY", & m_mainFramePos.y);
    config.Read("mainWidth", & m_mainFrameSize.x);
    config.Read("mainHeight", & m_mainFrameSize.y);
    config.Read("gridStyle", & m_gridStyle);
    config.Read("gridSpacing", & m_gridSpacing);

    return TRUE;
}

bool csApp::WriteOptions()
{
    wxConfig config("OGL Studio", "wxWindows");

    config.Write("mainX", m_mainFramePos.x);
    config.Write("mainY", m_mainFramePos.y);
    config.Write("mainWidth", m_mainFrameSize.x);
    config.Write("mainHeight", m_mainFrameSize.y);
    config.Write("gridStyle", (long) m_gridStyle);
    config.Write("gridSpacing", (long) m_gridSpacing);

    m_docManager->FileHistorySave(config);

    return TRUE;
}

