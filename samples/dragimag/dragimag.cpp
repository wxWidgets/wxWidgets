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

#define wxUSE_GENERIC_DRAGIMAGE 1

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "dragimag.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
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

        m_draggedShape->SetPosition(m_draggedShape->GetPosition()
                                    + event.GetPosition() - m_dragStartPos);

        m_dragImage->Hide();
        m_dragImage->EndDrag();
        delete m_dragImage;
        m_dragImage = NULL;

        m_draggedShape->SetShow(true);

        m_currentlyHighlighted = (DragShape*) NULL;

        m_draggedShape = (DragShape*) NULL;
        
        Refresh(true);
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

            // Start the drag.
            m_dragMode = TEST_DRAG_DRAGGING;

            if (m_dragImage)
                delete m_dragImage;

            // Erase the dragged shape from the canvas
            m_draggedShape->SetShow(false);

            // redraw immediately
            Refresh(true);
            Update();

            switch (m_draggedShape->GetDragMethod())
            {
                case SHAPE_DRAG_BITMAP:
                {
                    m_dragImage = new MyDragImage(this, m_draggedShape->GetBitmap(), wxCursor(wxCURSOR_HAND));
                    break;
                }
                case SHAPE_DRAG_TEXT:
                {
                    m_dragImage = new MyDragImage(this, wxString(_T("Dragging some test text")), wxCursor(wxCURSOR_HAND));
                    break;
                }
                case SHAPE_DRAG_ICON:
                {
                    m_dragImage = new MyDragImage(this, wxICON(dragicon), wxCursor(wxCURSOR_HAND));
                    break;
                }
            }

            bool fullScreen = wxGetApp().GetUseScreen();

            // The offset between the top-left of the shape image and the current shape position
            wxPoint beginDragHotSpot = m_dragStartPos - m_draggedShape->GetPosition();

            // Now we do this inside the implementation: always assume
            // coordinates relative to the capture window (client coordinates)

            //if (fullScreen)
            //    beginDragHotSpot -= ClientToScreen(wxPoint(0, 0));

            if (!m_dragImage->BeginDrag(beginDragHotSpot, this, fullScreen))
            {
                delete m_dragImage;
                m_dragImage = (wxDragImage*) NULL;
                m_dragMode = TEST_DRAG_NONE;

            } else
            {
                m_dragImage->Move(event.GetPosition());
                m_dragImage->Show();
            }
        }
        else if (m_dragMode == TEST_DRAG_DRAGGING)
        {
            // We're currently dragging. See if we're over another shape.
            DragShape* onShape = FindShape(event.GetPosition());

            bool mustUnhighlightOld = false;
            bool mustHighlightNew = false;

            if (m_currentlyHighlighted)
            {
                if ((onShape == (DragShape*) NULL) || (m_currentlyHighlighted != onShape))
                    mustUnhighlightOld = true;
            }

            if (onShape && (onShape != m_currentlyHighlighted) && onShape->IsShown())
                mustHighlightNew = true;

            if (mustUnhighlightOld || mustHighlightNew)
                m_dragImage->Hide();
            
            // Now with the drag image switched off, we can change the window contents.
            if (mustUnhighlightOld)
                m_currentlyHighlighted = (DragShape*) NULL;

            if (mustHighlightNew)
                m_currentlyHighlighted = onShape;

            if (mustUnhighlightOld || mustHighlightNew)
            {
                Refresh(mustUnhighlightOld);
                Update();
            }

            // Move and show the image again
            m_dragImage->Move(event.GetPosition());

            if (mustUnhighlightOld || mustHighlightNew)
                 m_dragImage->Show();
        }
    }
}

void MyCanvas::DrawShapes(wxDC& dc)
{
    wxList::compatibility_iterator node = m_displayList.GetFirst();
    while (node)
    {
        DragShape* shape = (DragShape*) node->GetData();
        if (shape->IsShown() && m_draggedShape != shape)
        {
            shape->Draw(dc, (m_currentlyHighlighted == shape));
        }
        node = node->GetNext();
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
    wxList::compatibility_iterator node = m_displayList.GetFirst();
    while (node)
    {
        DragShape* shape = (DragShape*) node->GetData();
        delete shape;
        node = node->GetNext();
    }
    m_displayList.Clear();
}

DragShape* MyCanvas::FindShape(const wxPoint& pt) const
{
    wxList::compatibility_iterator node = m_displayList.GetFirst();
    while (node)
    {
        DragShape* shape = (DragShape*) node->GetData();
        if (shape->HitTest(pt))
            return shape;
        node = node->GetNext();
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
: wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxDragImage sample"),
          wxPoint(20,20), wxSize(470,360) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( wxID_ABOUT, _T("&About..."));
    file_menu->AppendCheckItem( TEST_USE_SCREEN, _T("&Use whole screen for dragging"), _T("Use whole screen"));
    file_menu->Append( wxID_EXIT, _T("E&xit"));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, _T("&File"));

    SetIcon(wxICON(mondrian));
    SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR

    m_canvas = new MyCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(10,10) );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( true );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    (void)wxMessageBox( _T("wxDragImage demo\n")
        _T("Julian Smart (c) 2000"),
        _T("About wxDragImage Demo"),
        wxICON_INFORMATION | wxOK );
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
    m_useScreen = false;
}

bool MyApp::OnInit()
{
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    wxImage image;
    if (image.LoadFile(_T("backgrnd.png"), wxBITMAP_TYPE_PNG))
    {
        m_background = wxBitmap(image);
    }

    MyFrame *frame = new MyFrame();

    wxString rootName(_T("shape0"));

    int i;
    for (i = 1; i < 4; i++)
    {
        wxString filename;
        filename.Printf(wxT("%s%d.png"), (const wxChar*)rootName, i);
    /* For some reason under wxX11, the 2nd LoadFile in this loop fails, with
       a BadMatch inside CreateFromImage (inside ConvertToBitmap). This happens even if you copy
       the first file over the second file. */
        if (image.LoadFile(filename, wxBITMAP_TYPE_PNG))
        {
            DragShape* newShape = new DragShape(wxBitmap(image));
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

    frame->Show( true );

    return true;
}

int MyApp::OnExit()
{
    return 0;
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
    return true;
}

void MyApp::OnUseScreen(wxCommandEvent& WXUNUSED(event))
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
    m_show = true;
}

bool DragShape::HitTest(const wxPoint& pt) const
{
    wxRect rect(GetRect());
    return rect.Contains(pt.x, pt.y);
}

bool DragShape::Draw(wxDC& dc, bool highlight)
{
    if (m_bitmap.Ok())
    {
        wxMemoryDC memDC;
        memDC.SelectObject(m_bitmap);

        dc.Blit(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight(),
            & memDC, 0, 0, wxCOPY, true);
            
        if (highlight)
        {
            dc.SetPen(*wxWHITE_PEN);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight());
        }

        return true;
    }
    else
        return false;
}

// MyDragImage

// On some platforms, notably Mac OS X with Core Graphics, we can't blit from
// a window, so we need to draw the background explicitly.
bool MyDragImage::UpdateBackingFromWindow(wxDC& WXUNUSED(windowDC), wxMemoryDC& destDC, const wxRect& WXUNUSED(sourceRect),
                    const wxRect& destRect) const
{
    destDC.SetClippingRegion(destRect);

    if (wxGetApp().GetBackgroundBitmap().Ok())
        wxGetApp().TileBitmap(destRect, destDC, wxGetApp().GetBackgroundBitmap());

    m_canvas->DrawShapes(destDC);
    return true;
}

