/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !USE_DOC_VIEW_ARCHITECTURE
#error You must set USE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "docview.h"
#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView)

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;

BEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(DOODLE_CUT, DrawingView::OnCut)
END_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
  if (!singleWindowMode)
  {
    // Multiple windows
    frame = wxGetApp().CreateChildFrame(doc, this, TRUE);
    frame->SetTitle("DrawingView");

    canvas = GetMainFrame()->CreateCanvas(this, frame);
#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(x, y);
#endif
    frame->Show(TRUE);
  }
  else
  {
    // Single-window mode
    frame = GetMainFrame();
    canvas = GetMainFrame()->canvas;
    canvas->view = this;

    // Associate the appropriate frame with this view.
    SetFrame(frame);

    // Make sure the document manager knows that this is the
    // current view.
    Activate(TRUE);

    // Initialize the edit menu Undo and Redo items
    doc->GetCommandProcessor()->SetEditMenu(((MyFrame *)frame)->editMenu);
    doc->GetCommandProcessor()->Initialize();
  }

  return TRUE;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void DrawingView::OnDraw(wxDC *dc)
{
  dc->SetFont(*wxNORMAL_FONT);
  dc->SetPen(*wxBLACK_PEN);

  wxNode *node = ((DrawingDocument *)GetDocument())->GetDoodleSegments().First();
  while (node)
  {
    DoodleSegment *seg = (DoodleSegment *)node->Data();
    seg->Draw(dc);
    node = node->Next();
  }
}

void DrawingView::OnUpdate(wxView *sender, wxObject *hint)
{
  if (canvas)
    canvas->Refresh();

/* Is the following necessary?
#ifdef __WINDOWS__
  if (canvas)
    canvas->Refresh();
#else
  if (canvas)
    {
      wxClientDC dc(canvas);
      dc.Clear();
      OnDraw(& dc);
    }
#endif
*/
}

// Clean up windows used for displaying the view.
bool DrawingView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;

  // Clear the canvas in  case we're in single-window mode,
  // and the canvas stays.
  canvas->Clear();
  canvas->view = NULL;
  canvas = NULL;

  wxString s(wxTheApp->GetAppName());
  if (frame)
    frame->SetTitle(s);

  SetFrame(NULL);

  Activate(FALSE);
  
  if (deleteWindow && !singleWindowMode)
  {
    delete frame;
    return TRUE;
  }
  return TRUE;
}

void DrawingView::OnCut(wxCommandEvent& event)
{
    DrawingDocument *doc = (DrawingDocument *)GetDocument();
    doc->GetCommandProcessor()->Submit(new DrawingCommand("Cut Last Segment", DOODLE_CUT, doc, NULL));
}

IMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView)

bool TextEditView::OnCreate(wxDocument *doc, long flags)
{
  frame = wxGetApp().CreateChildFrame(doc, this, FALSE);

  int width, height;
  frame->GetClientSize(&width, &height);
  textsw = new MyTextWindow(this, frame, wxPoint(0, 0), wxSize(width, height), wxTE_MULTILINE);
  frame->SetTitle("TextEditView");

#ifdef __X__
  // X seems to require a forced resize
  int x, y;
  frame->GetSize(&x, &y);
  frame->SetSize(x, y);
#endif

  frame->Show(TRUE);
  Activate(TRUE);
  
  return TRUE;
}

// Handled by wxTextWindow
void TextEditView::OnDraw(wxDC *dc)
{
}

void TextEditView::OnUpdate(wxView *sender, wxObject *hint)
{
}

bool TextEditView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;
    
  Activate(FALSE);

  if (deleteWindow)
  {
    delete frame;
    return TRUE;
  }
  return TRUE;
}

/*
 * Window implementations
 */

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, const long style):
 wxScrolledWindow(frame, -1, pos, size, style)
{
  view = v;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
  if (view)
    view->OnDraw(& dc);
}

// This implements a tiny doodling program. Drag the mouse using
// the left button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
  if (!view)
    return;
    
  static DoodleSegment *currentSegment = NULL;

  wxClientDC dc(this);
  PrepareDC(dc);

  dc.SetPen(*wxBLACK_PEN);

  wxPoint pt(event.GetLogicalPosition(dc));

  if (currentSegment && event.LeftUp())
  {
    if (currentSegment->lines.Number() == 0)
    {
      delete currentSegment;
      currentSegment = NULL;
    }
    else
    {
      // We've got a valid segment on mouse left up, so store it.
      DrawingDocument *doc = (DrawingDocument *)view->GetDocument();

      doc->GetCommandProcessor()->Submit(new DrawingCommand("Add Segment", DOODLE_ADD, doc, currentSegment));

      view->GetDocument()->Modify(TRUE);
      currentSegment = NULL;
    }
  }
  
  if (xpos > -1 && ypos > -1 && event.Dragging())
  {
    if (!currentSegment)
      currentSegment = new DoodleSegment;

    DoodleLine *newLine = new DoodleLine;
    newLine->x1 = xpos; newLine->y1 = ypos;
    newLine->x2 = pt.x; newLine->y2 = pt.y;
    currentSegment->lines.Append(newLine);
    
    dc.DrawLine(xpos, ypos, pt.x, pt.y);
  }
  xpos = pt.x;
  ypos = pt.y;
}

// Define a constructor for my text subwindow
MyTextWindow::MyTextWindow(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, const long style):
 wxTextCtrl(frame, -1, "", pos, size, style)
{
  view = v;
}


