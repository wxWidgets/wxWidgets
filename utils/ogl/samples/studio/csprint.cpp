/////////////////////////////////////////////////////////////////////////////
// Name:        csprint.cpp
// Purpose:     Printing and clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
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

#include <wx/wxexpr.h>
#include <wx/clipbrd.h>

#ifdef __WXMSW__
#include <wx/metafile.h>
#endif

#include "studio.h"
#include "doc.h"
#include "shapes.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(wxDiagramClipboard, wxDiagram)

// Copy selection
bool wxDiagramClipboard::Copy(wxDiagram* diagram)
{
    DeleteAllShapes();

    return DoCopy(diagram, this, FALSE, NULL);
}

// Copy contents to the diagram, with new ids.

bool wxDiagramClipboard::Paste(wxDiagram* diagram, wxDC* dc, int offsetX, int offsetY)
{
    return DoCopy(this, diagram, TRUE, dc, offsetX, offsetY);
}

// Universal copy function (to or from clipboard).
// TODO:
// Note that this only works for non-composites so far (nested shapes
// don't have their old-to-new object mappings stored).
// Also, lines don't yet get their attachment points moved to the new offset position
// if they have more than 2 points.
bool wxDiagramClipboard::DoCopy(wxDiagram* diagramFrom, wxDiagram* diagramTo, bool newIds,
    wxDC* dc, int offsetX, int offsetY)
{
    OnStartCopy(diagramTo);

    wxHashTable mapping(wxKEY_INTEGER);

    // First copy all node shapes.
    wxList* shapeList = diagramFrom->GetShapeList();
    wxNode* node = shapeList->First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (((diagramFrom == this) || shape->Selected()) && !shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxShape* newShape = shape->CreateNewCopy();
            newShape->GetLines().Clear();
            if (newIds)
            {
                newShape->AssignNewIds();
            }
            mapping.Put((long) shape, (wxObject*) newShape);

            newShape->SetX(newShape->GetX() + offsetX);
            newShape->SetY(newShape->GetY() + offsetY);

            OnAddShape(diagramTo, newShape, dc);

        }
        node = node->Next();
    }

    node = shapeList->First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (((diagramFrom == this) || shape->Selected()) && shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxLineShape* lineShape = (wxLineShape*) shape;
            // Only copy a line if its ends are selected too.
            if ((diagramFrom == this) || (lineShape->GetTo()->Selected() && lineShape->GetFrom()->Selected()))
            {
                wxLineShape* newShape = (wxLineShape*) shape->CreateNewCopy();
                mapping.Put((long) shape, (wxObject*) newShape);

                if (newIds)
                    newShape->AssignNewIds();

                wxShape* fromShape = (wxShape*) mapping.Get((long) lineShape->GetFrom());
                wxShape* toShape = (wxShape*) mapping.Get((long) lineShape->GetTo());

                wxASSERT_MSG( (fromShape != NULL), "Could not find 'from' shape");
                wxASSERT_MSG( (toShape != NULL), "Could not find 'to' shape");

                fromShape->AddLine(newShape, toShape, newShape->GetAttachmentFrom(),
                  newShape->GetAttachmentTo());

                OnAddShape(diagramTo, newShape, dc);

            }
        }
        node = node->Next();
    }

    // Now make sure line ordering is correct
    node = shapeList->First();
    while (node)
    {
        wxShape* shape = (wxShape*) node->Data();
        if (((diagramFrom == this) || shape->Selected()) && !shape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            wxShape* newShape = (wxShape*) mapping.Get((long) shape);

            // Make a list of all the new lines, in the same order as the old lines.
            // Then apply the list of new lines to the shape.
            wxList newLines;
            wxNode* lineNode = shape->GetLines().First();
            while (lineNode)
            {
                wxLineShape* lineShape = (wxLineShape*) lineNode->Data();
                if ((diagramFrom == this) || (lineShape->GetTo()->Selected() && lineShape->GetFrom()->Selected()))
                {
                    wxLineShape* newLineShape = (wxLineShape*) mapping.Get((long) lineShape);

                    wxASSERT_MSG( (newLineShape != NULL), "Could not find new line shape");

                    newLines.Append(newLineShape);
                }

                lineNode = lineNode->Next();
            }

            if (newLines.Number() > 0)
                newShape->ApplyAttachmentOrdering(newLines);
        }
        node = node->Next();
    }

    OnEndCopy(diagramTo);

    return TRUE;
}

#ifdef __WXMSW__
// Draw contents to a Windows metafile device context and a bitmap, and copy
// these to the Windows clipboard
bool wxDiagramClipboard::CopyToClipboard(double scale)
{
  // Make a metafile DC
  wxMetaFileDC mfDC;
  if (mfDC.Ok())
  {
    mfDC.SetUserScale(scale, scale);

    // Draw on metafile DC
    Redraw(mfDC);

    int printWidth = mfDC.MaxX() - mfDC.MinX();
    int printHeight = mfDC.MaxY() - mfDC.MinY();
    int maxX = (int)mfDC.MaxX();
    int maxY = (int)mfDC.MaxY();
    wxMetaFile *mf = mfDC.Close();

    // Set to a bitmap memory DC
    wxBitmap *newBitmap = new wxBitmap((int)(maxX + 10), (int)(maxY + 10));
    if (!newBitmap->Ok())
    {
      delete newBitmap;
      
      char buf[200];
      sprintf(buf, "Sorry, could not allocate clipboard bitmap (%dx%d)", (maxX+10), (maxY+10));
      wxMessageBox(buf, "Clipboard copy problem");
      return FALSE;
    }

    wxMemoryDC memDC;
    memDC.SelectObject(*newBitmap);
    memDC.Clear();

    // Now draw on memory bitmap DC
    Redraw(memDC);

    memDC.SelectObject(wxNullBitmap);

    // Open clipboard and set the data
    if (wxOpenClipboard())
    {
        wxEmptyClipboard();

        // Copy the bitmap to the clipboard
        wxSetClipboardData(wxDF_BITMAP, newBitmap, 0, 0);

        if (mf)
        {
            // Copy the metafile to the clipboard
            // Allow a small margin
            bool success = mf->SetClipboard((int)(mfDC.MaxX() + 15), (int)(mfDC.MaxY() + 15));
        }

        // Close clipboard
        wxCloseClipboard();
    }
    
    delete newBitmap;
    delete mf;

  }
  return TRUE;
}
#endif
    // __WXMSW__

// Override this to e.g. have the shape added through a Do/Undo command system.
// By default, we'll just add it directly to the destination diagram.
bool wxDiagramClipboard::OnAddShape(wxDiagram* diagramTo, wxShape* newShape, wxDC* dc)
{
    diagramTo->AddShape(newShape);

    if (dc && (diagramTo != this))
    {
        newShape->Select(TRUE, dc);
    }

    return TRUE;
}

/*
 * csDiagramClipboard
 */

IMPLEMENT_DYNAMIC_CLASS(csDiagramClipboard, wxDiagramClipboard)

// Start/end copying
bool csDiagramClipboard::OnStartCopy(wxDiagram* diagramTo)
{
    // Do nothing if copying to the clipboard
    if (diagramTo == this)
        return TRUE;

    // Deselect all objects initially.

    csDiagram* diagram = (csDiagram*) diagramTo;
    csDiagramDocument* doc = diagram->GetDocument();
    ((csDiagramView*)doc->GetFirstView())->SelectAll(FALSE);

    m_currentCmd = new csDiagramCommand("Paste", doc);

    return TRUE;
}

bool csDiagramClipboard::OnEndCopy(wxDiagram* diagramTo)
{
    // Do nothing if copying to the clipboard
    if (diagramTo == this)
        return TRUE;

    csDiagram* diagram = (csDiagram*) diagramTo;
    csDiagramDocument* doc = diagram->GetDocument();

    if (m_currentCmd)
    {
        if (m_currentCmd->GetStates().Number() == 0)
        {
            delete m_currentCmd;
        }
        else
        {
            doc->GetCommandProcessor()->Submit(m_currentCmd);
            m_currentCmd = NULL;
        }
    }
    return TRUE;
}

// Use the command framework to add the shapes, if we're copying to a diagram and
// not the clipboard.
bool csDiagramClipboard::OnAddShape(wxDiagram* diagramTo, wxShape* newShape, wxDC* dc)
{
    if (diagramTo == this)
    {
        diagramTo->AddShape(newShape);
    }
    else
    {
        csDiagram* diagram = (csDiagram*) diagramTo;
        csDiagramDocument* doc = diagram->GetDocument();

        if (newShape->IsKindOf(CLASSINFO(wxLineShape)))
            m_currentCmd->AddState(new csCommandState(ID_CS_ADD_LINE_SELECT, newShape, NULL));
        else
            m_currentCmd->AddState(new csCommandState(ID_CS_ADD_SHAPE_SELECT, newShape, NULL));
    }

    return TRUE;
}


