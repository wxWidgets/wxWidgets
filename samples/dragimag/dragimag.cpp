/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.cpp
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"

// Under Windows, change this to 1
// to use wxGenericDragImage

#define wxUSE_GENERIC_DRAGIMAGE 0

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "dragimag.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#include "dragicon.xpm"
#endif

// main program

IMPLEMENT_APP(MyApp)

// MyCanvas

IMPLEMENT_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(MyCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(MyCanvas::OnEraseBackground)
  EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER )
{
    SetBackgroundColour(* wxWHITE);

    SetCursor(wxCursor(wxCURSOR_ARROW));

    m_dragMode = TEST_DRAG_NONE;
    m_draggedShape = (DragShape*) NULL;
    m_dragImage = (wxDragImage*) NULL;
    m_currentlyHighlighted = (DragShape*) NULL;
}

MyCanvas::~MyCanvas()
{
    ClearShapes();

    if (m_dragImage)
        delete m_dragImage;
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    DrawShapes(dc);

}

void MyCanvas::OnEraseBackground(wxEraseEvent& event)
{
    if (wxGetApp().GetBackgroundBitmap().Ok())
    {
        wxSize sz = GetClientSize();
        wxRect rect(0, 0, sz.x, sz.y);
        
        if (event.GetDC())
        {
            wxGetApp().TileBitmap(rect, *(event.GetDC()), wxGetApp().GetBackgroundBitmap());
        }
        else
        {
            wxClientDC dc(this);
            wxGetApp().TileBitmap(rect, dc, wxGetApp().GetBackgroundBitmap());
        }
    }
    else
        event.Skip(); // The official way of doing it
}

void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
        DragShape* shape = FindShape(event.GetPosition());
        if (shape)
        {
            // We tentatively start dragging, but wait for
            // mouse movement before dragging properly.

            m_dragMode = TEST_DRAG_START;
            m_dragStartPos = event.GetPosition();
            m_draggedShape = shape;
        }
    }
    else if (event.LeftUp() && m_dragMode != TEST_DRAG_NONE)
    {
        // Finish dragging

        m_dragMode = TEST_DRAG_NONE;

        if (!m_draggedShape || !m_dragImage)
            return;

        wxPoint newPos(m_draggedShape->GetPosition().x + (event.GetPosition().x - m_dragStartPos.x),
                           m_draggedShape->GetPosition().y + (event.GetPosition().y - m_dragStartPos.y));

        m_draggedShape->SetPosition(newPos);

        m_dragImage->Hide();
        m_dragImage->EndDrag();
        delete m_dragImage;
        m_dragImage = NULL;

        wxClientDC dc(this);
        if (m_currentlyHighlighted)
        {
            m_currentlyHighlighted->Draw(dc);
        }
        m_draggedShape->SetShow(TRUE);
        m_draggedShape->Draw(dc);

        m_currentlyHighlighted = (DragShape*) NULL;

        m_draggedShape = (DragShape*) NULL;
    }
    else if (event.Dragging() && m_dragMode != TEST_DRAG_NONE)
    {
        if (m_dragMode == TEST_DRAG_START)
        {
            // We will start dragging if we've moved beyond a couple of pixels

            int tolerance = 2;
            int dx = abs(event.GetPosition().x - m_dragStartPos.x);
            int dy = abs(event.GetPosition().y - m_dragStartPos.y);
            if (dx <= tolerance && dy <= tolerance)
                return;

            wxPoint newPos(m_draggedShape->GetPosition().x + (event.GetPosition().x - m_dragStartPos.x),
                           m_draggedShape->GetPosition().y + (event.GetPosition().y - m_dragStartPos.y));

            // Start the drag.
            m_dragMode = TEST_DRAG_DRAGGING;

            if (m_dragImage)
                delete m_dragImage;

            // Erase the dragged shape from the canvas
            m_draggedShape->SetShow(FALSE);
            wxClientDC dc(this);
            EraseShape(m_draggedShape, dc);
            DrawShapes(dc);

            switch (m_draggedShape->GetDragMethod())
            {
                case SHAPE_DRAG_BITMAP:
                {
                    wxPoint hotSpot(event.GetPosition().x - newPos.x, event.GetPosition().y - newPos.y);
                    m_dragImage = new wxDragImage(m_draggedShape->GetBitmap(), wxCursor(wxCURSOR_HAND), hotSpot);
                    break;
                }
                case SHAPE_DRAG_TEXT:
                {
                    wxPoint hotSpot(event.GetPosition().x - newPos.x, event.GetPosition().y - newPos.y);
                    m_dragImage = new wxDragImage("Dragging some test text", wxCursor(wxCURSOR_HAND), hotSpot);
                    break;
                }
                case SHAPE_DRAG_ICON:
                {
                    wxPoint hotSpot(event.GetPosition().x - newPos.x, event.GetPosition().y - newPos.y);

                    // Can anyone explain why this test is necessary,
                    // to prevent a gcc error?
#ifdef __WXMOTIF__
		    wxIcon icon(dragicon_xpm);
#else
		    wxIcon icon(wxICON(dragicon));
#endif

                    m_dragImage = new wxDragImage(icon, wxCursor(wxCURSOR_HAND), hotSpot);
                    break;
                }
            }

            bool fullScreen = FALSE;
            if (wxGetApp().GetUseScreen())
            {
                newPos = ClientToScreen(newPos);
                fullScreen = TRUE;
            }

            bool retValue;

            if (fullScreen)
                // This line uses the whole screen...
                retValue = m_dragImage->BeginDrag(wxPoint(0, 0), this, TRUE);
                // while this line restricts dragging to the parent frame.
                // retValue = m_dragImage->BeginDrag(wxPoint(0, 0), this, GetParent());
            else
                retValue = m_dragImage->BeginDrag(wxPoint(0, 0), this);

            if (!retValue)
            {
                delete m_dragImage;
                m_dragImage = (wxDragImage*) NULL;
                m_dragMode = TEST_DRAG_NONE;
            }
            m_dragImage->Move(newPos);
            m_dragImage->Show();
        }
        else if (m_dragMode == TEST_DRAG_DRAGGING)
        {
            // We're currently dragging. See if we're over another shape.
            DragShape* onShape = FindShape(event.GetPosition());

            bool mustUnhighlightOld = FALSE;
            bool mustHighlightNew = FALSE;

            if (m_currentlyHighlighted)
            {
                if ((onShape == (DragShape*) NULL) || (m_currentlyHighlighted != onShape))
                    mustUnhighlightOld = TRUE;
            }

            if (onShape && (onShape != m_currentlyHighlighted) && onShape->IsShown())
                mustHighlightNew = TRUE;

            if (mustUnhighlightOld || mustHighlightNew)
                m_dragImage->Hide();

            // Now with the drag image switched off, we can change the window contents.

            if (mustUnhighlightOld)
            {
                wxClientDC clientDC(this);
                m_currentlyHighlighted->Draw(clientDC);
                m_currentlyHighlighted = (DragShape*) NULL;
            }
            if (mustHighlightNew)
            {
                wxClientDC clientDC(this);
                m_currentlyHighlighted = onShape;
                m_currentlyHighlighted->Draw(clientDC, wxINVERT);
            }

            wxPoint newPos(m_draggedShape->GetPosition().x + (event.GetPosition().x - m_dragStartPos.x),
                           m_draggedShape->GetPosition().y + (event.GetPosition().y - m_dragStartPos.y));

            if (wxGetApp().GetUseScreen())
            {
                newPos = ClientToScreen(newPos);
            }

            // Move and show the image again
            m_dragImage->Move(newPos);

            if (mustUnhighlightOld || mustHighlightNew)
                 m_dragImage->Show();
        }
    }
}

void MyCanvas::DrawShapes(wxDC& dc)
{
    wxNode* node = m_displayList.First();
    while (node)
    {
        DragShape* shape = (DragShape*) node->Data();
        if (shape->IsShown())
          shape->Draw(dc);
        node = node->Next();
    }
}

void MyCanvas::EraseShape(DragShape* shape, wxDC& dc)
{
    wxSize sz = GetClientSize();
    wxRect rect(0, 0, sz.x, sz.y);

    wxRect rect2(shape->GetRect());
    dc.SetClippingRegion(rect2.x, rect2.y, rect2.width, rect2.height);
        
    wxGetApp().TileBitmap(rect, dc, wxGetApp().GetBackgroundBitmap());

    dc.DestroyClippingRegion();
}

void MyCanvas::ClearShapes()
{
    wxNode* node = m_displayList.First();
    while (node)
    {
        DragShape* shape = (DragShape*) node->Data();
        delete shape;
        node = node->Next();
    }
    m_displayList.Clear();
}

DragShape* MyCanvas::FindShape(const wxPoint& pt) const
{
    wxNode* node = m_displayList.First();
    while (node)
    {
        DragShape* shape = (DragShape*) node->Data();
        if (shape->HitTest(pt))
            return shape;
        node = node->Next();
    }
    return (DragShape*) NULL;
}

// MyFrame

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (wxID_EXIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, "wxDragImage sample",
                  wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu();
  file_menu->Append( wxID_ABOUT, "&About...");
  file_menu->Append( TEST_USE_SCREEN, "&Use whole screen for dragging", "Use whole screen", TRUE);
  file_menu->Append( wxID_EXIT, "E&xit");

  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "&File");

  SetMenuBar( menu_bar );

  CreateStatusBar(2);
  int widths[] = { -1, 100 };
  SetStatusWidths( 2, widths );

  m_canvas = new MyCanvas( this, -1, wxPoint(0,0), wxSize(10,10) );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( "wxDragImage demo\n"
                      "Julian Smart (c) 2000",
                      "About wxDragImage Demo", wxICON_INFORMATION | wxOK );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(TEST_USE_SCREEN, MyApp::OnUseScreen)
END_EVENT_TABLE()

MyApp::MyApp()
{
    // Drag across whole screen
    m_useScreen = FALSE;
}

bool MyApp::OnInit()
{
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    wxImage image;
    if (image.LoadFile("backgrnd.png", wxBITMAP_TYPE_PNG))
    {
        m_background = image.ConvertToBitmap();
    }


    MyFrame *frame = new MyFrame();

    wxString rootName("shape0");

    int i;
    for (i = 1; i < 4; i++)
    {
        wxString filename;
        filename.Printf("%s%d.png", (const char*) rootName, i);
        if (image.LoadFile(filename, wxBITMAP_TYPE_PNG))
        {
            DragShape* newShape = new DragShape(image.ConvertToBitmap());
            newShape->SetPosition(wxPoint(i*50, i*50));

            if (i == 2)
                newShape->SetDragMethod(SHAPE_DRAG_TEXT);
            else if (i == 3)
                newShape->SetDragMethod(SHAPE_DRAG_ICON);
            else
                newShape->SetDragMethod(SHAPE_DRAG_BITMAP);
            frame->GetCanvas()->GetDisplayList().Append(newShape);
        }
    }

#if 0
    // Under Motif or GTK, this demonstrates that
    // wxScreenDC only gets the root window content.
    // We need to be able to copy the overall content
    // for full-screen dragging to work.
    int w, h;
    wxDisplaySize(& w, & h);
    wxBitmap bitmap(w, h);

    wxScreenDC dc;
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, w, h, & dc, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    m_background = bitmap;
#endif

    frame->Show( TRUE );

    return TRUE;
}

bool MyApp::TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap)
{
    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();
    
    int i, j;
    for (i = rect.x; i < rect.x + rect.width; i += w)
    {
        for (j = rect.y; j < rect.y + rect.height; j+= h)
            dc.DrawBitmap(bitmap, i, j);
    }
    return TRUE;
}

void MyApp::OnUseScreen(wxCommandEvent& event)
{
    m_useScreen = !m_useScreen;
}

// DragShape

DragShape::DragShape(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    m_pos.x = 0;
    m_pos.y = 0;
    m_dragMethod = SHAPE_DRAG_BITMAP;
    m_show = TRUE;
}

DragShape::~DragShape()
{
}

bool DragShape::HitTest(const wxPoint& pt) const
{
    wxRect rect(GetRect());
    return rect.Inside(pt.x, pt.y);
}

bool DragShape::Draw(wxDC& dc, int op)
{
    if (m_bitmap.Ok())
    {
        wxMemoryDC memDC;
        memDC.SelectObject(m_bitmap);
    
        dc.Blit(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight(),
            & memDC, 0, 0, op, TRUE);

        return TRUE;
    }
    else
        return FALSE;
}

