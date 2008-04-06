/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "docview.h"
#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView)

// For drawing lines in a canvas
static float xpos = -1;
static float ypos = -1;

BEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(DOODLE_CUT, DrawingView::OnCut)
END_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    if (!singleWindowMode)
    {
        // Multiple windows
        m_frame = wxGetApp().CreateChildFrame(doc, this, true);
        m_frame->SetTitle(wxT("DrawingView"));

        m_canvas = GetMainFrame()->CreateCanvas(this, m_frame);
#ifdef __X__
        // X seems to require a forced resize
        int x, y;
        m_frame->GetSize(&x, &y);
        m_frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
        m_frame->Show(true);
    }
    else
    {
        // Single-window mode
        m_frame = GetMainFrame();
        m_canvas = GetMainFrame()->m_canvas;
        m_canvas->m_view = this;

        // Associate the appropriate frame with this view.
        SetFrame(m_frame);

        // Make sure the document manager knows that this is the
        // current view.
        Activate(true);

        // Initialize the edit menu Undo and Redo items
        doc->GetCommandProcessor()->SetEditMenu(((MyFrame*)m_frame)->m_editMenu);
        doc->GetCommandProcessor()->Initialize();
    }

    return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void DrawingView::OnDraw(wxDC *dc)
{
    dc->SetFont(*wxNORMAL_FONT);
    dc->SetPen(*wxBLACK_PEN);

    wxList::compatibility_iterator node = GetDocument()->GetDoodleSegments().GetFirst();
    while (node)
    {
        DoodleSegment *seg = (DoodleSegment *)node->GetData();
        seg->Draw(dc);
        node = node->GetNext();
    }
}

DrawingDocument* DrawingView::GetDocument()
{
    return wxStaticCast(wxView::GetDocument(), DrawingDocument);
}

void DrawingView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    if (m_canvas)
        m_canvas->Refresh();

/* Is the following necessary?
#ifdef __WXMSW__
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
        return false;

    // Clear the canvas in  case we're in single-window mode,
    // and the canvas stays.
    m_canvas->ClearBackground();
    m_canvas->m_view = NULL;
    m_canvas = NULL;

    wxString s(wxTheApp->GetAppDisplayName());
    if (m_frame)
        m_frame->SetTitle(s);

    SetFrame(NULL);

    Activate(false);

    if (deleteWindow && !singleWindowMode)
    {
        delete m_frame;
        return true;
    }
    return true;
}

void DrawingView::OnCut(wxCommandEvent& WXUNUSED(event) )
{
    DrawingDocument* doc = GetDocument();
    doc->GetCommandProcessor()->Submit(new DrawingCommand(wxT("Cut Last Segment"), DOODLE_CUT, doc, NULL));
}

IMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView)

BEGIN_EVENT_TABLE(TextEditView, wxView)
    EVT_MENU(wxID_COPY, TextEditView::OnCopy)
    EVT_MENU(wxID_PASTE, TextEditView::OnPaste)
    EVT_MENU(wxID_SELECTALL, TextEditView::OnSelectAll)
END_EVENT_TABLE()

bool TextEditView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    m_frame = wxGetApp().CreateChildFrame(doc, this, false);

    wxSize size = m_frame->GetClientSize();
    m_textsw = new MyTextWindow(this, m_frame, wxPoint(0, 0), size, wxTE_MULTILINE);
    m_frame->SetTitle(wxT("TextEditView"));

#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif

    m_frame->Show(true);
    Activate(true);

    return true;
}

// Handled by wxTextWindow
void TextEditView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

void TextEditView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
{
}

bool TextEditView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close())
        return false;

    Activate(false);

    if (deleteWindow)
    {
        wxDELETE(m_frame)
        return true;
    }
    return true;
}

/*
* Window implementations
*/

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(DrawingView* view, wxFrame* frame, const wxPoint& pos, const wxSize& size, const long style):
    wxScrolledWindow(frame, wxID_ANY, pos, size, style)
{
    m_view = view;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    if (m_view)
        m_view->OnDraw(& dc);
}

// This implements a tiny doodling program. Drag the mouse using
// the left button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if (!m_view)
        return;

    static DoodleSegment *currentSegment = NULL;

    wxClientDC dc(this);
    PrepareDC(dc);

    dc.SetPen(*wxBLACK_PEN);

    wxPoint pt(event.GetLogicalPosition(dc));

    if (currentSegment && event.LeftUp())
    {
        if (currentSegment->m_lines.GetCount() == 0)
        {
            delete currentSegment;
            currentSegment = NULL;
        }
        else
        {
            // We've got a valid segment on mouse left up, so store it.
            DrawingDocument* doc = m_view->GetDocument();

            doc->GetCommandProcessor()->Submit(new DrawingCommand(wxT("Add Segment"), DOODLE_ADD, doc, currentSegment));

            m_view->GetDocument()->Modify(true);
            currentSegment = NULL;
        }
    }

    if ( (xpos > -1) && (ypos > -1) && event.Dragging())
    {
        if (!currentSegment)
            currentSegment = new DoodleSegment;

        DoodleLine *newLine = new DoodleLine;
        newLine->x1 = (long)xpos;
        newLine->y1 = (long)ypos;
        newLine->x2 = pt.x;
        newLine->y2 = pt.y;
        currentSegment->m_lines.Append(newLine);

        dc.DrawLine( (long)xpos, (long)ypos, pt.x, pt.y);
    }
    xpos = pt.x;
    ypos = pt.y;
}

// Define a constructor for my text subwindow
MyTextWindow::MyTextWindow(wxView* view, wxFrame* frame, const wxPoint& pos, const wxSize& size, const long style):
    wxTextCtrl(frame, wxID_ANY, wxEmptyString, pos, size, style)
{
    m_view = view;
}


