/////////////////////////////////////////////////////////////////////////////
// Name:        Studio.h
// Purpose:     Studio application class
// Author:      Julian Smart
// Modified by:
// Created:     27/7/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_STUDIO_H_
#define _STUDIO_STUDIO_H_

#include <wx/docmdi.h>
#include <wx/help.h>

#include <ogl.h>
#include <canvas.h>
#include "shapes.h"

class csEditorToolPalette;
class csProjectTreeCtrl;
class csCanvas;
class csSymbolDatabase;
class wxSashLayoutWindow;
class csFrame;

// Grid style
#define csGRID_STYLE_NONE       0
#define csGRID_STYLE_INVISIBLE  1
#define csGRID_STYLE_DOTTED     2

// Define a new application
class csApp: public wxApp
{
    friend csFrame;
public:
    csApp();
    ~csApp();

// Operations
    bool OnInit(void);
    int OnExit(void);

    // Read/write configuration information
    bool ReadOptions();
    bool WriteOptions();

    // Create the diagram tool palette
    bool CreatePalette(wxFrame *parent);

    // Create the project window
    bool CreateProjectWindow(wxFrame *parent);

    // Initialise the general toolbar
    void InitToolBar(wxToolBar* toolBar);

    // Create and initialise the diagram toolbar
    void CreateDiagramToolBar(wxFrame* parent);

    wxMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view, wxMenu** editMenu);
    csCanvas *CreateCanvas(wxView *view, wxFrame *parent);

    // Fill out the project tree control
    void FillProjectTreeCtrl();

    // Add symbols to database
    void InitSymbols();

// Accessors
    csEditorToolPalette* GetDiagramPalette() const { return m_diagramPalette; }
    wxToolBar* GetDiagramToolBar() const { return m_diagramToolBar; }
    csProjectTreeCtrl* GetProjectTreeCtrl() const { return m_projectTreeCtrl; }
    wxSashLayoutWindow* GetDiagramPaletteSashWindow() const { return m_diagramPaletteSashWindow; }
    wxSashLayoutWindow* GetProjectSashWindow() const { return m_projectSashWindow; }
    wxSashLayoutWindow* GetDiagramToolBarSashWindow() const { return m_diagramToolBarSashWindow; }
    csSymbolDatabase* GetSymbolDatabase() const { return m_symbolDatabase; }
    wxComboBox* GetPointSizeComboBox() const { return m_pointSizeComboBox; }
    wxComboBox* GetZoomComboBox() const { return m_zoomComboBox; }
    wxMenu* GetShapeEditMenu() const { return m_shapeEditMenu; }
    wxDiagramClipboard& GetDiagramClipboard() const { return (wxDiagramClipboard&) m_diagramClipboard; }
    wxDocManager* GetDocManager() const { return m_docManager; }
    wxHelpController& GetHelpController() const { return (wxHelpController&) m_helpController; }

    int GetGridStyle() const { return m_gridStyle; }
    void SetGridStyle(int style) { m_gridStyle = style; }

    int GetGridSpacing() const { return m_gridSpacing; }
    void SetGridSpacing(int spacing) { m_gridSpacing = spacing; }

protected:
    wxDocManager*           m_docManager;
    wxSashLayoutWindow*     m_diagramPaletteSashWindow;
    wxSashLayoutWindow*     m_diagramToolBarSashWindow;
    wxSashLayoutWindow*     m_projectSashWindow;
    csEditorToolPalette*    m_diagramPalette;
    csProjectTreeCtrl*      m_projectTreeCtrl;
    csSymbolDatabase*       m_symbolDatabase;
    wxToolBar*              m_diagramToolBar;
    wxComboBox*             m_pointSizeComboBox;
    wxComboBox*             m_zoomComboBox;
    wxMenu*                 m_shapeEditMenu;

    // Configuration
    wxPoint                 m_mainFramePos;
    wxSize                  m_mainFrameSize;
    int                     m_gridStyle;
    int                     m_gridSpacing;

    // Diagram clipboard
    csDiagramClipboard      m_diagramClipboard;

    // Help instance
    wxHelpController        m_helpController;
};

DECLARE_APP(csApp)

#define ID_CS_CUT                         wxID_CUT
#define ID_CS_ADD_SHAPE                   2
#define ID_CS_ADD_LINE                    3
// #define ID_CS_EDIT_LABEL                  4
#define ID_CS_EDIT_PROPERTIES             4
#define ID_CS_CHANGE_BACKGROUND_COLOUR    5
#define ID_CS_MOVE                        6
#define ID_CS_SIZE                        7
#define ID_CS_FONT_CHANGE                 8
#define ID_CS_ARROW_CHANGE                9
#define ID_CS_ROTATE_CLOCKWISE            11
#define ID_CS_ROTATE_ANTICLOCKWISE        12
#define ID_CS_CHANGE_LINE_ORDERING        13  // Change the list of lines for a wxShape
#define ID_CS_CHANGE_LINE_ATTACHMENT      14  // Change the attachment point for one end of a line
#define ID_CS_ALIGN                       15
#define ID_CS_NEW_POINT                   16
#define ID_CS_CUT_POINT                   17
#define ID_CS_STRAIGHTEN                  18
#define ID_CS_MOVE_LINE_POINT             19
#define ID_CS_MOVE_LABEL                  20
#define ID_CS_ADD_SHAPE_SELECT            21
#define ID_CS_ADD_LINE_SELECT             22

#define ID_CS_ABOUT                       100
#define ID_CS_SELECT_ALL                  102
#define ID_CS_SETTINGS                    103

#define ID_LAYOUT_WINDOW_PALETTE          200
#define ID_LAYOUT_WINDOW_DIAGRAM_TOOLBAR  201
#define ID_LAYOUT_WINDOW_PROJECT          202

#define ID_DIAGRAM_PALETTE                250

#define ID_WINDOW_PROJECT_TREE            300
#define ID_WINDOW_POINT_SIZE_COMBOBOX     301
#define ID_WINDOW_ZOOM_COMBOBOX           302

#define DIAGRAM_TOOLBAR_ALIGNL            500
#define DIAGRAM_TOOLBAR_ALIGNR            501
#define DIAGRAM_TOOLBAR_ALIGNB            502
#define DIAGRAM_TOOLBAR_ALIGNT            503
#define DIAGRAM_TOOLBAR_ALIGN_HORIZ       504
#define DIAGRAM_TOOLBAR_ALIGN_VERT        505
#define DIAGRAM_TOOLBAR_COPY_SIZE         506
#define DIAGRAM_TOOLBAR_LINE_ARROW        507
#define DIAGRAM_TOOLBAR_NEW_POINT         508
#define DIAGRAM_TOOLBAR_CUT_POINT         509
#define DIAGRAM_TOOLBAR_STRAIGHTEN        510

#endif
  // _STUDIO_STUDIO_H_

