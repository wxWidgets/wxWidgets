/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     Implements view functionality
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/colordlg.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "studio.h"
#include "doc.h"
#include "view.h"
#include "cspalette.h"
#include "symbols.h"
#include "dialogs.h"
#include "basicp.h"
#include "linesp.h"

IMPLEMENT_DYNAMIC_CLASS(csDiagramView, wxView)

BEGIN_EVENT_TABLE(csDiagramView, wxView)
    EVT_MENU(wxID_CUT, csDiagramView::OnCut)
    EVT_MENU(wxID_COPY, csDiagramView::OnCopy)
    EVT_MENU(wxID_CLEAR, csDiagramView::OnClear)
    EVT_MENU(wxID_PASTE, csDiagramView::OnPaste)
    EVT_MENU(wxID_DUPLICATE, csDiagramView::OnDuplicate)
    EVT_MENU(ID_CS_CHANGE_BACKGROUND_COLOUR, csDiagramView::OnChangeBackgroundColour)
    EVT_MENU(ID_CS_EDIT_PROPERTIES, csDiagramView::OnEditProperties)
    EVT_MENU(ID_CS_SELECT_ALL, csDiagramView::OnSelectAll)
    EVT_TOOL(DIAGRAM_TOOLBAR_LINE_ARROW, csDiagramView::OnToggleArrowTool)
    EVT_COMBOBOX(ID_WINDOW_POINT_SIZE_COMBOBOX, csDiagramView::OnPointSizeComboSel)
    EVT_COMBOBOX(ID_WINDOW_ZOOM_COMBOBOX, csDiagramView::OnZoomSel)
    EVT_TEXT(ID_WINDOW_POINT_SIZE_COMBOBOX, csDiagramView::OnPointSizeComboText)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGNL, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGNR, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGNB, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGNT, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGN_HORIZ, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_ALIGN_VERT, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_COPY_SIZE, csDiagramView::OnAlign)
    EVT_TOOL(DIAGRAM_TOOLBAR_NEW_POINT, csDiagramView::OnNewLinePoint)
    EVT_TOOL(DIAGRAM_TOOLBAR_CUT_POINT, csDiagramView::OnCutLinePoint)
    EVT_TOOL(DIAGRAM_TOOLBAR_STRAIGHTEN, csDiagramView::OnStraightenLines)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGNL, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGNR, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGNB, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGNT, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGN_HORIZ, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_ALIGN_VERT, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_COPY_SIZE, csDiagramView::OnAlignUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_NEW_POINT, csDiagramView::OnNewLinePointUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_CUT_POINT, csDiagramView::OnCutLinePointUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_STRAIGHTEN, csDiagramView::OnStraightenLinesUpdate)
    EVT_UPDATE_UI(DIAGRAM_TOOLBAR_LINE_ARROW, csDiagramView::OnToggleArrowToolUpdate)
    EVT_UPDATE_UI(wxID_CUT, csDiagramView::OnCutUpdate)
    EVT_UPDATE_UI(wxID_COPY, csDiagramView::OnCopyUpdate)
    EVT_UPDATE_UI(wxID_CLEAR, csDiagramView::OnClearUpdate)
    EVT_UPDATE_UI(wxID_PASTE, csDiagramView::OnPasteUpdate)
    EVT_UPDATE_UI(wxID_DUPLICATE, csDiagramView::OnDuplicateUpdate)
    EVT_UPDATE_UI(ID_CS_EDIT_PROPERTIES, csDiagramView::OnEditPropertiesUpdate)
    EVT_UPDATE_UI(wxID_UNDO, csDiagramView::OnUndoUpdate)
    EVT_UPDATE_UI(wxID_REDO, csDiagramView::OnRedoUpdate)
END_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool csDiagramView::OnCreate(wxDocument *doc, long flags)
{
  wxMenu* editMenu;
  frame = wxGetApp().CreateChildFrame(doc, this, &editMenu);
  canvas = wxGetApp().CreateCanvas(this, frame);
  canvas->SetView(this);

  SetFrame(frame);
  Activate(TRUE);

  // Initialize the edit menu Undo and Redo items
  doc->GetCommandProcessor()->SetEditMenu(editMenu);
  doc->GetCommandProcessor()->Initialize();

  wxShapeCanvas *shapeCanvas = (wxShapeCanvas *)canvas;
  csDiagramDocument *diagramDoc = (csDiagramDocument *)doc;
  shapeCanvas->SetDiagram(diagramDoc->GetDiagram());
  diagramDoc->GetDiagram()->SetCanvas(shapeCanvas);

  diagramDoc->GetDiagram()->SetGridSpacing((double) wxGetApp().GetGridSpacing());

    switch (wxGetApp().GetGridStyle())
    {
        case csGRID_STYLE_NONE:
        {
            diagramDoc->GetDiagram()->SetSnapToGrid(FALSE);
            break;
        }
        case csGRID_STYLE_INVISIBLE:
        {
            diagramDoc->GetDiagram()->SetSnapToGrid(TRUE);
            break;
        }
        case csGRID_STYLE_DOTTED:
        {
            // TODO (not implemented in OGL)
            break;
        }
    }


  return TRUE;
}

csDiagramView::~csDiagramView(void)
{
    if (frame)
    {
        ((wxDocMDIChildFrame*)frame)->SetView(NULL);
    }
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void csDiagramView::OnDraw(wxDC *dc)
{
}

void csDiagramView::OnUpdate(wxView *sender, wxObject *hint)
{
  if (canvas)
    canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool csDiagramView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;

  csDiagramDocument *diagramDoc = (csDiagramDocument *)GetDocument();
  diagramDoc->GetDiagram()->SetCanvas(NULL);

  canvas->Clear();
  canvas->SetDiagram(NULL);
  canvas->SetView(NULL);
  canvas = NULL;

  wxMenu* fileMenu = frame->GetMenuBar()->GetMenu(0);

  // Remove file menu from those managed by the command history
  wxGetApp().GetDocManager()->FileHistoryRemoveMenu(fileMenu);

  Activate(FALSE);
  frame->Show(FALSE);

  if (deleteWindow)
  {
    frame->Destroy();
  }
  
  return TRUE;
}

// Adds or removes shape from m_selections
void csDiagramView::SelectShape(wxShape* shape, bool select)
{
    if (select && !m_selections.Member(shape))
        m_selections.Append(shape);
    else if (!select)
        m_selections.DeleteObject(shape);
}

void csDiagramView::OnSelectAll(wxCommandEvent& event)
{
    SelectAll(TRUE);
}

wxShape *csDiagramView::FindFirstSelectedShape(void)
{
  csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
  wxShape *theShape = NULL;
  wxNode *node = doc->GetDiagram()->GetShapeList()->First();
  while (node)
  {
    wxShape *eachShape = (wxShape *)node->Data();
    if ((eachShape->GetParent() == NULL) && !eachShape->IsKindOf(CLASSINFO(wxLabelShape)) && eachShape->Selected())
    {
      theShape = eachShape;
      node = NULL;
    }
    else node = node->Next();
  }
  return theShape;
}

void csDiagramView::FindSelectedShapes(wxList& selections, wxClassInfo* toFind)
{
  csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
  wxNode *node = doc->GetDiagram()->GetShapeList()->First();
  while (node)
  {
    wxShape *eachShape = (wxShape *)node->Data();
    if ((eachShape->GetParent() == NULL) && !eachShape->IsKindOf(CLASSINFO(wxLabelShape)) && eachShape->Selected() && ((toFind == NULL) || (eachShape->IsKindOf(toFind))))
    {
      selections.Append(eachShape);
    }
    node = node->Next();
  }
}

void csDiagramView::OnUndoUpdate(wxUpdateUIEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    event.Enable(doc->GetCommandProcessor()->CanUndo());
}

void csDiagramView::OnRedoUpdate(wxUpdateUIEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    event.Enable(doc->GetCommandProcessor()->CanRedo());
}

void csDiagramView::OnCut(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    // Copy the shapes to the clipboard
    wxGetApp().GetDiagramClipboard().Copy(doc->GetDiagram());

    wxList selections;
    FindSelectedShapes(selections);

    DoCut(selections);
}

void csDiagramView::OnClear(wxCommandEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections);

    DoCut(selections);
}

void csDiagramView::OnCopy(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    // Copy the shapes to the clipboard
    if (wxGetApp().GetDiagramClipboard().Copy(doc->GetDiagram()))
    {
#ifdef __WXMSW__
        // Copy to the Windows clipboard
        wxGetApp().GetDiagramClipboard().CopyToClipboard(1.0);
#endif
    }
}

void csDiagramView::OnPaste(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    wxGetApp().GetDiagramClipboard().Paste(doc->GetDiagram());
}

void csDiagramView::OnDuplicate(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    // Do a copy, then a paste
    wxGetApp().GetDiagramClipboard().Copy(doc->GetDiagram());

    // Apply an offset. Really, this offset should keep being incremented,
    // but where do we reset it again?
    wxGetApp().GetDiagramClipboard().Paste(doc->GetDiagram(), NULL, 20, 20);
}

void csDiagramView::OnCutUpdate(wxUpdateUIEvent& event)
{
    event.Enable( (m_selections.Number() > 0) );
}

void csDiagramView::OnClearUpdate(wxUpdateUIEvent& event)
{
    event.Enable( (m_selections.Number() > 0) );
}

void csDiagramView::OnCopyUpdate(wxUpdateUIEvent& event)
{
    event.Enable( (m_selections.Number() > 0) );
}

void csDiagramView::OnPasteUpdate(wxUpdateUIEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    int n = wxGetApp().GetDiagramClipboard().GetCount();

    event.Enable( (n > 0) );
}

void csDiagramView::OnDuplicateUpdate(wxUpdateUIEvent& event)
{
    event.Enable( (m_selections.Number() > 0) );
}

void csDiagramView::DoCut(wxList& shapes)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    if (shapes.Number() > 0)
    {
        csDiagramCommand* cmd = new csDiagramCommand("Cut", doc);

        wxNode* node = shapes.First();
        while (node)
        {
            wxShape *theShape = (wxShape*) node->Data();
            csCommandState* state = new csCommandState(ID_CS_CUT, NULL, theShape);

            // Insert lines at the front, so they are cut first.
            // Otherwise we may try to remove a shape with a line still
            // attached.
            if (theShape->IsKindOf(CLASSINFO(wxLineShape)))
                cmd->InsertState(state);
            else
                cmd->AddState(state);

            node = node->Next();
        }
        cmd->RemoveLines(); // Schedule any connected lines, not already mentioned,
                            // to be removed first

        doc->GetCommandProcessor()->Submit(cmd);
    }
}

// Generalised command
void csDiagramView::DoCmd(wxList& shapes, wxList& oldShapes, int cmd, const wxString& op)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    if (shapes.Number() > 0)
    {
        csDiagramCommand* command = new csDiagramCommand(op, doc);

        wxNode* node = shapes.First();
        wxNode* node1 = oldShapes.First();
        while (node && node1)
        {
            wxShape *theShape = (wxShape*) node->Data();
            wxShape *oldShape = (wxShape*) node1->Data();
            csCommandState* state = new csCommandState(cmd, theShape, oldShape);
            command->AddState(state);

            node = node->Next();
            node1 = node1->Next();
        }
        doc->GetCommandProcessor()->Submit(command);
    }
}

void csDiagramView::OnChangeBackgroundColour(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    wxList selections;
    FindSelectedShapes(selections);

    if (selections.Number() > 0)
    {
        wxColourData data;
        data.SetChooseFull(TRUE);
        if (selections.Number() == 1)
        {
            wxShape* firstShape = (wxShape*) selections.First()->Data();
            data.SetColour(firstShape->GetBrush()->GetColour());
        }

        wxColourDialog *dialog = new wxColourDialog(frame, &data);
        wxBrush *theBrush = NULL;
        if (dialog->ShowModal() == wxID_OK)
        {
          wxColourData retData = dialog->GetColourData();
          wxColour col = retData.GetColour();
          theBrush = wxTheBrushList->FindOrCreateBrush(col, wxSOLID);
        }
        dialog->Close(TRUE);
        if (!theBrush)
            return;

        csDiagramCommand* cmd = new csDiagramCommand("Change colour", doc);

        wxNode* node = selections.First();
        while (node)
        {
            wxShape *theShape = (wxShape*) node->Data();
            wxShape* newShape = theShape->CreateNewCopy();
            newShape->SetBrush(theBrush);

            csCommandState* state = new csCommandState(ID_CS_CHANGE_BACKGROUND_COLOUR, newShape, theShape);
            cmd->AddState(state);

            node = node->Next();
        }
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

void csDiagramView::OnEditProperties(wxCommandEvent& event)
{
      wxShape *theShape = FindFirstSelectedShape();
      if (theShape)
        ((csEvtHandler *)theShape->GetEventHandler())->EditProperties();
}

void csDiagramView::OnEditPropertiesUpdate(wxUpdateUIEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections);
    event.Enable( (selections.Number() > 0) );
}

void csDiagramView::OnPointSizeComboSel(wxCommandEvent& event)
{
    wxComboBox* combo = (wxComboBox*) event.GetEventObject();
    wxASSERT( combo != NULL );

    int newPointSize = (combo->GetSelection() + 1);

    ApplyPointSize(newPointSize);

}

// TODO: must find out how to intercept the Return key, rather than
// every key stroke. But for now, do every key stroke.
void csDiagramView::OnPointSizeComboText(wxCommandEvent& event)
{
    wxComboBox* combo = (wxComboBox*) event.GetEventObject();
    wxASSERT( combo != NULL );

    wxString str(combo->GetValue());
    int newPointSize = atoi((const char*) str);

    if (newPointSize < 2)
        return;

    ApplyPointSize(newPointSize);
}

void csDiagramView::ApplyPointSize(int pointSize)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    wxList selections;
    FindSelectedShapes(selections);

    if (selections.Number() > 0)
    {
        csDiagramCommand* cmd = new csDiagramCommand("Point size", doc);

        wxNode* node = selections.First();
        while (node)
        {
            wxShape *theShape = (wxShape*) node->Data();
            wxShape *newShape = theShape->CreateNewCopy();

            wxFont* newFont = wxTheFontList->FindOrCreateFont(pointSize,
                theShape->GetFont()->GetFamily(),
                theShape->GetFont()->GetStyle(),
                theShape->GetFont()->GetWeight(),
                theShape->GetFont()->GetUnderlined(),
                theShape->GetFont()->GetFaceName());

            newShape->SetFont(newFont);

            csCommandState* state = new csCommandState(ID_CS_FONT_CHANGE, newShape, theShape);

            cmd->AddState(state);

            node = node->Next();
        }
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

void csDiagramView::OnZoomSel(wxCommandEvent& event)
{
    int maxZoom = 200;
    int minZoom = 5;
    int inc = 5;
    int noStrings = (maxZoom - minZoom)/inc ;

    wxComboBox* combo = (wxComboBox*) event.GetEventObject();
    wxASSERT( combo != NULL );

    int scale = (int) ((noStrings - combo->GetSelection() - 1)*inc + minZoom);

    canvas->SetScale((double) (scale/100.0), (double) (scale/100.0));
    canvas->Refresh();
}

// Select or deselect all
void csDiagramView::SelectAll(bool select)
{
    wxClientDC dc(canvas);
    canvas->PrepareDC(dc);

    if (!select)
    {
        wxList selections;
        FindSelectedShapes(selections);

        wxNode* node = selections.First();
        while (node)
        {
            wxShape *theShape = (wxShape*) node->Data();
            theShape->Select(FALSE, &dc);
            SelectShape(theShape, FALSE);

            node = node->Next();
        }
    }
    else
    {
        csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
        wxNode *node = doc->GetDiagram()->GetShapeList()->First();
        while (node)
        {
            wxShape *eachShape = (wxShape *)node->Data();
            if (eachShape->GetParent() == NULL &&
                !eachShape->IsKindOf(CLASSINFO(wxControlPoint)) &&
                !eachShape->IsKindOf(CLASSINFO(wxLabelShape)))
            {
                eachShape->Select(TRUE, &dc);
                SelectShape(eachShape, TRUE);
            }
            node = node->Next();
        }
    }
}


void csDiagramView::OnToggleArrowTool(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();

    bool state = wxGetApp().GetDiagramToolBar()->GetToolState(DIAGRAM_TOOLBAR_LINE_ARROW);
    wxString stateName;
    if (state)
        stateName = "Arrow on";
    else
        stateName = "Arrow off";

    wxList selections;
    FindSelectedShapes(selections, CLASSINFO(wxLineShape));

    if (selections.Number() > 0)
    {
        csDiagramCommand* cmd = new csDiagramCommand(stateName, doc);

        wxNode* node = selections.First();
        while (node)
        {
            wxLineShape *theShape = (wxLineShape*) node->Data();
            wxLineShape *newShape = NULL;

            if (state)
            {
                // Add arrow
                if (theShape->GetArrows().Number() == 0)
                {
                    newShape = (wxLineShape*) theShape->CreateNewCopy();
                    newShape->AddArrow(ARROW_ARROW, ARROW_POSITION_MIDDLE, 10.0, 0.0, "Normal arrowhead");
                }
            }
            else
            {
                if (theShape->GetArrows().Number() > 0)
                {
                    newShape = (wxLineShape*) theShape->CreateNewCopy();
                    newShape->ClearArrowsAtPosition();
                }
            }

            // If the new state is the same as the old, don't bother adding it to the command state.
            if (newShape)
            {
                csCommandState* state = new csCommandState(ID_CS_ARROW_CHANGE, newShape, theShape);
                cmd->AddState(state);
            }

            node = node->Next();
        }
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

void csDiagramView::OnToggleArrowToolUpdate(wxUpdateUIEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections, CLASSINFO(wxLineShape));
    event.Enable( (selections.Number() > 0) );
}

// Make the point size combobox reflect this
void csDiagramView::ReflectPointSize(int pointSize)
{
    wxComboBox* comboBox = wxGetApp().GetPointSizeComboBox();
    comboBox->SetSelection(pointSize -1);
}

// Make the arrow toggle button reflect the state of the line
void csDiagramView::ReflectArrowState(wxLineShape* lineShape)
{
    bool haveArrow = FALSE;
    wxNode *node = lineShape->GetArrows().First();
    while (node)
    {
      wxArrowHead *arrow = (wxArrowHead *)node->Data();
      if (ARROW_POSITION_MIDDLE == arrow->GetArrowEnd())
        haveArrow = TRUE;
      node = node->Next();
    }

    wxGetApp().GetDiagramToolBar()->ToggleTool(DIAGRAM_TOOLBAR_LINE_ARROW, haveArrow);
}

void csDiagramView::OnAlign(wxCommandEvent& event)
{
    // Make a copy of the selections, keeping only those shapes
    // that are top-level non-line shapes.
    wxList selections;
    wxNode* node = GetSelectionList().First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if ((shape->GetParent() == NULL) && (!shape->IsKindOf(CLASSINFO(wxLineShape))))
        {
            selections.Append(shape);
        }
        node = node->Next();
    }

    if (selections.Number() == 0)
        return;

    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    csDiagramCommand* cmd = new csDiagramCommand("Align", doc);

    node = selections.First();
    wxShape* firstShape = (wxShape*) node->Data();

    double x = firstShape->GetX();
    double y = firstShape->GetY();
    double width, height;
    firstShape->GetBoundingBoxMax(&width, &height);

    node = selections.First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (shape != firstShape)
        {
            double x1 = shape->GetX();
            double y1 = shape->GetY();
            double width1, height1;
            shape->GetBoundingBoxMax(& width1, & height1);

            wxShape* newShape = shape->CreateNewCopy();

            switch (event.GetId())
            {
                case DIAGRAM_TOOLBAR_ALIGNL:
                {
                    double x2 = (double)(x - (width/2.0) + (width1/2.0));
                    newShape->SetX(x2);
                    break;
                }
                case DIAGRAM_TOOLBAR_ALIGNR:
                {
                    double x2 = (double)(x + (width/2.0) - (width1/2.0));
                    newShape->SetX(x2);
                    break;
                }
                case DIAGRAM_TOOLBAR_ALIGNB:
                {
                    double y2 = (double)(y + (height/2.0) - (height1/2.0));
                    newShape->SetY(y2);
                    break;
                }
                case DIAGRAM_TOOLBAR_ALIGNT:
                {
                    double y2 = (double)(y - (height/2.0) + (height1/2.0));
                    newShape->SetY(y2);
                    break;
                }
                case DIAGRAM_TOOLBAR_ALIGN_HORIZ:
                {
                    newShape->SetX(x);
                    break;
                }
                case DIAGRAM_TOOLBAR_ALIGN_VERT:
                {
                    newShape->SetY(y);
                    break;
                }
                case DIAGRAM_TOOLBAR_COPY_SIZE:
                {
                    newShape->SetSize(width, height);
                    break;
                }
            }
            csCommandState* state = new csCommandState(ID_CS_ALIGN, newShape, shape);
            cmd->AddState(state);
        }
        node = node->Next();
    }
    doc->GetCommandProcessor()->Submit(cmd);
}

void csDiagramView::OnAlignUpdate(wxUpdateUIEvent& event)
{
    // This is an approximation, since there may be lines
    // amongst the selections.
    event.Enable( (m_selections.Number() > 1) ) ;
}

void csDiagramView::OnNewLinePoint(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    csDiagramCommand* cmd = new csDiagramCommand("New line point", doc);

    wxNode* node = m_selections.First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxShape* newShape = shape->CreateNewCopy();
            ((wxLineShape*)newShape)->InsertLineControlPoint(NULL);
            csCommandState* state = new csCommandState(ID_CS_NEW_POINT, newShape, shape);
            cmd->AddState(state);
        }
        node = node->Next();
    }
    doc->GetCommandProcessor()->Submit(cmd);
}

void csDiagramView::OnCutLinePoint(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    csDiagramCommand* cmd = new csDiagramCommand("Cut line point", doc);

    wxNode* node = m_selections.First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxShape* newShape = shape->CreateNewCopy();
            ((wxLineShape*)newShape)->DeleteLineControlPoint();
            csCommandState* state = new csCommandState(ID_CS_CUT_POINT, newShape, shape);
            cmd->AddState(state);
        }
        node = node->Next();
    }
    doc->GetCommandProcessor()->Submit(cmd);
}

void csDiagramView::OnStraightenLines(wxCommandEvent& event)
{
    csDiagramDocument *doc = (csDiagramDocument *)GetDocument();
    csDiagramCommand* cmd = new csDiagramCommand("Straighten lines", doc);

    wxNode* node = m_selections.First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxShape* newShape = shape->CreateNewCopy();
            ((wxLineShape*)newShape)->Straighten();
            csCommandState* state = new csCommandState(ID_CS_STRAIGHTEN, newShape, shape);
            cmd->AddState(state);
        }
        node = node->Next();
    }
    doc->GetCommandProcessor()->Submit(cmd);
}

void csDiagramView::OnNewLinePointUpdate(wxUpdateUIEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections, CLASSINFO(wxLineShape));
    event.Enable( (selections.Number() > 0) );
}

void csDiagramView::OnCutLinePointUpdate(wxUpdateUIEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections, CLASSINFO(wxLineShape));
    event.Enable( (selections.Number() > 0) );
}

void csDiagramView::OnStraightenLinesUpdate(wxUpdateUIEvent& event)
{
    wxList selections;
    FindSelectedShapes(selections, CLASSINFO(wxLineShape));
    event.Enable( (selections.Number() > 0) );
}

/*
 * Window implementations
 */

IMPLEMENT_CLASS(csCanvas, wxShapeCanvas)

BEGIN_EVENT_TABLE(csCanvas, wxShapeCanvas)
    EVT_MOUSE_EVENTS(csCanvas::OnMouseEvent)
    EVT_PAINT(csCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
csCanvas::csCanvas(csDiagramView *v, wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style):
 wxShapeCanvas(parent, id, pos, size, style)
{
  m_view = v;
}

csCanvas::~csCanvas(void)
{
}

void csCanvas::DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2)
{
    wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
    dc.SetPen(dottedPen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    dc.DrawRectangle((long) x1, (long) y1, (long) (x2 - x1), (long) (y2 - y1));
}

void csCanvas::OnLeftClick(double x, double y, int keys)
{
    csEditorToolPalette *palette = wxGetApp().GetDiagramPalette();

    if (palette->GetSelection() == PALETTE_ARROW)
    {
        GetView()->SelectAll(FALSE);

        wxClientDC dc(this);
        PrepareDC(dc);

        Redraw(dc);
        return;
    }

    if (palette->GetSelection() == PALETTE_TEXT_TOOL)
    {
        // Ask for a label and create a new free-floating text region
        csLabelEditingDialog* dialog = new csLabelEditingDialog(GetParent());

        dialog->SetShapeLabel("");
        dialog->SetTitle("New text box");
        if (dialog->ShowModal() == wxID_CANCEL)
        {
            dialog->Destroy();
            return;
        }

        wxString newLabel = dialog->GetShapeLabel();
        dialog->Destroy();

        wxShape* shape = new csTextBoxShape;
        shape->AssignNewIds();
        shape->SetEventHandler(new csEvtHandler(shape, shape, newLabel));

        wxComboBox* comboBox = wxGetApp().GetPointSizeComboBox();
        wxString str(comboBox->GetValue());
        int pointSize = atoi((const char*) str);

        wxFont* newFont = wxTheFontList->FindOrCreateFont(pointSize,
                shape->GetFont()->GetFamily(),
                shape->GetFont()->GetStyle(),
                shape->GetFont()->GetWeight(),
                shape->GetFont()->GetUnderlined(),
                shape->GetFont()->GetFaceName());

        shape->SetFont(newFont);

        shape->SetX(x);
        shape->SetY(y);

        csDiagramCommand* cmd = new csDiagramCommand("Text box",
            (csDiagramDocument *)GetView()->GetDocument(),
            new csCommandState(ID_CS_ADD_SHAPE, shape, NULL));
        GetView()->GetDocument()->GetCommandProcessor()->Submit(cmd);

        palette->SetSelection(PALETTE_ARROW);

        return;
    }

    csSymbol* symbol = wxGetApp().GetSymbolDatabase()->FindSymbol(palette->GetSelection());
    if (symbol)
    {
        wxShape* theShape = symbol->GetShape()->CreateNewCopy();

        wxComboBox* comboBox = wxGetApp().GetPointSizeComboBox();
        wxString str(comboBox->GetValue());
        int pointSize = atoi((const char*) str);

        wxFont* newFont = wxTheFontList->FindOrCreateFont(pointSize,
                symbol->GetShape()->GetFont()->GetFamily(),
                symbol->GetShape()->GetFont()->GetStyle(),
                symbol->GetShape()->GetFont()->GetWeight(),
                symbol->GetShape()->GetFont()->GetUnderlined(),
                symbol->GetShape()->GetFont()->GetFaceName());

        theShape->SetFont(newFont);

        theShape->AssignNewIds();
        theShape->SetX(x);
        theShape->SetY(y);

        csDiagramCommand* cmd = new csDiagramCommand(symbol->GetName(),
            (csDiagramDocument *)GetView()->GetDocument(),
            new csCommandState(ID_CS_ADD_SHAPE, theShape, NULL));
        GetView()->GetDocument()->GetCommandProcessor()->Submit(cmd);

        palette->SetSelection(PALETTE_ARROW);
    }
}

void csCanvas::OnRightClick(double x, double y, int keys)
{
}

// Initial point
static double sg_initialX, sg_initialY;

void csCanvas::OnDragLeft(bool draw, double x, double y, int keys)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    dc.SetLogicalFunction(OGLRBLF);
    DrawOutline(dc, sg_initialX, sg_initialY, x, y);
}

void csCanvas::OnBeginDragLeft(double x, double y, int keys)
{
    sg_initialX = x;
    sg_initialY = y;

    wxClientDC dc(this);
    PrepareDC(dc);

    dc.SetLogicalFunction(OGLRBLF);
    DrawOutline(dc, sg_initialX, sg_initialY, x, y);
    CaptureMouse();
}

void csCanvas::OnEndDragLeft(double x, double y, int keys)
{
    ReleaseMouse();

    wxClientDC dc(this);
    PrepareDC(dc);

    // Select all images within the rectangle
    float min_x, max_x, min_y, max_y;
    min_x = wxMin(x, sg_initialX);
    max_x = wxMax(x, sg_initialX);
    min_y = wxMin(y, sg_initialY);
    max_y = wxMax(y, sg_initialY);

    wxNode *node = GetDiagram()->GetShapeList()->First();
    while (node)
    {
        wxShape *shape = (wxShape *)node->Data();
        if (shape->GetParent() == NULL && !shape->IsKindOf(CLASSINFO(wxControlPoint)))
        {
            float image_x = shape->GetX();
            float image_y = shape->GetY();
            if (image_x >= min_x && image_x <= max_x &&
                image_y >= min_y && image_y <= max_y)
            {
                shape->Select(TRUE, &dc);
                GetView()->SelectShape(shape, TRUE);
            }
        }
        node = node->Next();
    }
}

void csCanvas::OnDragRight(bool draw, double x, double y, int keys)
{
}

void csCanvas::OnBeginDragRight(double x, double y, int keys)
{
}

void csCanvas::OnEndDragRight(double x, double y, int keys)
{
}

void csCanvas::OnMouseEvent(wxMouseEvent& event)
{
    wxShapeCanvas::OnMouseEvent(event);
}

void csCanvas::OnPaint(wxPaintEvent& event)
{
//  if (GetDiagram())
    wxShapeCanvas::OnPaint(event);
}
