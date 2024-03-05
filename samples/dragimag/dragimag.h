/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Created:     28/2/2000
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

    wxDECLARE_DYNAMIC_CLASS(MyFrame);
    wxDECLARE_EVENT_TABLE();
};

// MyApp

class MyApp: public wxApp
{
public:
    MyApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;

//// Operations

    // Tile the bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, const wxBitmap& bitmap);

//// Accessors
    const wxBitmap& GetBackgroundBitmap() const { return m_background; }

    bool GetUseScreen() const { return m_useScreen; }
    void SetUseScreen(bool useScreen) { m_useScreen = useScreen; }

    void OnUseScreen(wxCommandEvent& event);

protected:
    wxBitmap    m_background;
    bool        m_useScreen;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(MyApp);

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

    wxDECLARE_ABSTRACT_CLASS(MyCanvas);
    wxDECLARE_EVENT_TABLE();
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
    ~DragShape(){}

//// Operations

    bool HitTest(const wxPoint& pt) const;
    bool Draw(wxDC& dc, bool highlight = false);

//// Accessors

    wxPoint GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }

    wxRect GetRect() const { return wxRect(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight()); }

    const wxBitmap& GetBitmap() const { return m_bitmap; }
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

// MyDragImage
// A derived class is required since we're overriding UpdateBackingFromWindow,
// for compatibility with Mac OS X (Core Graphics) which does not support blitting
// from a window.

class MyDragImage: public wxDragImage
{
public:
    MyDragImage(MyCanvas* canvas): m_canvas(canvas) {}

    MyDragImage(MyCanvas* canvas, const wxBitmap& image, const wxCursor& cursor = wxNullCursor):
        wxDragImage(image, cursor), m_canvas(canvas)
    {
    }

    MyDragImage(MyCanvas* canvas, const wxIcon& image, const wxCursor& cursor = wxNullCursor):
        wxDragImage(image, cursor), m_canvas(canvas)
    {
    }

    MyDragImage(MyCanvas* canvas, const wxString& str, const wxCursor& cursor = wxNullCursor):
        wxDragImage(str, cursor), m_canvas(canvas)
    {
    }

#if wxUSE_GENERIC_DRAGIMAGE
    // On some platforms, notably Mac OS X with Core Graphics, we can't blit from
    // a window, so we need to draw the background explicitly.
    virtual bool UpdateBackingFromWindow(wxDC& windowDC, wxMemoryDC& destDC, const wxRect& sourceRect,
                    const wxRect& destRect) const override;
#endif

protected:
    MyCanvas*   m_canvas;
};

#endif
    // _WX_DRAGIMAGSAMPLE_
