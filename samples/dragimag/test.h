/////////////////////////////////////////////////////////////////////////////
// Name:        test.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAGIMAGSAMPLE_
#define _WX_DRAGIMAGSAMPLE_

// derived classes

class MyFrame;
class MyApp;
class MyCanvas;
class DragShape;

// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );

    MyCanvas* GetCanvas() const { return m_canvas; }
    void SetCanvas(MyCanvas* canvas) { m_canvas = canvas; }

private:
    MyCanvas*       m_canvas;

    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

// MyApp

class MyApp: public wxApp
{
public:
    MyApp();
    virtual bool OnInit();

//// Operations

    // Tile the bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);

//// Accessors
    wxBitmap& GetBackgroundBitmap() const { return (wxBitmap&) m_background; }

    bool GetUseScreen() const { return m_useScreen; }
    void SetUseScreen(bool useScreen) { m_useScreen = useScreen; }

    void OnUseScreen(wxCommandEvent& event);

protected:
    wxBitmap    m_background;
    bool        m_useScreen;

DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyApp)

#define TEST_USE_SCREEN   100

// MyCanvas

// Dragging modes
#define TEST_DRAG_NONE     0
#define TEST_DRAG_START    1
#define TEST_DRAG_DRAGGING 2

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();

    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

    void DrawShapes(wxDC& dc);
    void EraseShape(DragShape* shape, wxDC& dc);
    void ClearShapes();
    DragShape* FindShape(const wxPoint& pt) const;

    wxList& GetDisplayList() { return m_displayList; }

protected:

private:
    wxList          m_displayList; // A list of DragShapes
    int             m_dragMode;
    DragShape*      m_draggedShape;
    DragShape*      m_currentlyHighlighted; // The shape that's being highlighted
    wxPoint         m_dragStartPos;
    wxDragImage*    m_dragImage;

    DECLARE_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};


// Ways to drag a shape

#define SHAPE_DRAG_BITMAP       1
#define SHAPE_DRAG_TEXT         2
#define SHAPE_DRAG_ICON         3

// Shape

class DragShape: public wxObject
{
public:
    DragShape(const wxBitmap& bitmap);
    ~DragShape();

//// Operations

    bool HitTest(const wxPoint& pt) const;
    bool Draw(wxDC& dc, int op = wxCOPY);

//// Accessors

    wxPoint GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }

    wxRect GetRect() const { return wxRect(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight()); }

    wxBitmap& GetBitmap() const { return (wxBitmap&) m_bitmap; }
    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }

    int GetDragMethod() const { return m_dragMethod; }
    void SetDragMethod(int method) { m_dragMethod = method; }

    bool IsShown() const { return m_show; }
    void SetShow(bool show) { m_show = show; }

protected:
    wxPoint     m_pos;
    wxBitmap    m_bitmap;
    int         m_dragMethod;
    bool        m_show;
};

#endif
    // _WX_DRAGIMAGSAMPLE_
