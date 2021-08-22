/////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     Drag and drop sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04/01/98
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dnd.h"
#include "wx/dataobj.h"
#include "wx/image.h"
#include "wx/clipbrd.h"
#include "wx/colordlg.h"
#include "wx/metafile.h"
#include "wx/dirctrl.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#if wxUSE_DRAG_AND_DROP
    #include "dnd_copy.xpm"
    #include "dnd_move.xpm"
    #include "dnd_none.xpm"
#endif
#endif

#if wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// Derive two simple classes which just put in the listbox the strings (text or
// file names) we drop on them
// ----------------------------------------------------------------------------

class DnDText : public wxTextDropTarget
{
public:
    DnDText(wxListBox *pOwner) { m_pOwner = pOwner; }

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text) wxOVERRIDE;

private:
    wxListBox *m_pOwner;
};

class DnDFile : public wxFileDropTarget
{
public:
    DnDFile(wxListBox *pOwner = NULL) { m_pOwner = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames) wxOVERRIDE;

private:
    wxListBox *m_pOwner;
};

// ----------------------------------------------------------------------------
// Define a custom dtop target accepting URLs
// ----------------------------------------------------------------------------

class URLDropTarget : public wxDropTarget
{
public:
    URLDropTarget() { SetDataObject(new wxURLDataObject); }

    void OnDropURL(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxString& text)
    {
        // of course, a real program would do something more useful here...
        wxMessageBox(text, "wxDnD sample: got URL",
                     wxICON_INFORMATION | wxOK);
    }

    // URLs can't be moved, only copied
    virtual wxDragResult OnDragOver(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                    wxDragResult WXUNUSED(def)) wxOVERRIDE
        {
            return wxDragLink;  // At least IE 5.x needs wxDragLink, the
                                // other browsers on MSW seem okay with it too.
        }

    // translate this to calls to OnDropURL() just for convenience
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE
    {
        if ( !GetData() )
            return wxDragNone;

        OnDropURL(x, y, ((wxURLDataObject *)m_dataObject)->GetURL());

        return def;
    }
};

#endif // wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// Define a new application type
// ----------------------------------------------------------------------------

class DnDApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

wxIMPLEMENT_APP(DnDApp);

#if wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD

// ----------------------------------------------------------------------------
// Define canvas class to show a bitmap
// ----------------------------------------------------------------------------

class DnDCanvasBitmap : public wxScrolledWindow
{
public:
    DnDCanvasBitmap(wxWindow *parent) : wxScrolledWindow(parent) { }

    void SetBitmap(const wxBitmap& bitmap)
    {
        m_bitmap = bitmap;

        SetScrollbars(10, 10,
                      m_bitmap.GetWidth() / 10, m_bitmap.GetHeight() / 10);

        Refresh();
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        if ( m_bitmap.IsOk() )
        {
            PrepareDC(dc);

            dc.DrawBitmap(m_bitmap, 0, 0);
        }
    }

private:
    wxBitmap m_bitmap;

    wxDECLARE_EVENT_TABLE();
};

#if wxUSE_METAFILE

// and the same thing fo metafiles
class DnDCanvasMetafile : public wxScrolledWindow
{
public:
    DnDCanvasMetafile(wxWindow *parent) : wxScrolledWindow(parent) { }

    void SetMetafile(const wxMetafile& metafile)
    {
        m_metafile = metafile;

        SetScrollbars(10, 10,
                      m_metafile.GetWidth() / 10, m_metafile.GetHeight() / 10);

        Refresh();
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        if ( m_metafile.IsOk() )
        {
            PrepareDC(dc);

            m_metafile.Play(&dc);
        }
    }

private:
    wxMetafile m_metafile;

    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_METAFILE

// ----------------------------------------------------------------------------
// Define a new frame type for the main frame
// ----------------------------------------------------------------------------

class DnDFrame : public wxFrame
{
public:
    DnDFrame();
    virtual ~DnDFrame();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDrag(wxCommandEvent& event);
    void OnDragMoveByDefault(wxCommandEvent& event);
    void OnDragMoveAllow(wxCommandEvent& event);
    void OnNewFrame(wxCommandEvent& event);
    void OnHelp (wxCommandEvent& event);
#if wxUSE_LOG
    void OnLogClear(wxCommandEvent& event);
#endif // wxUSE_LOG

    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);

    void OnCopyBitmap(wxCommandEvent& event);
    void OnPasteBitmap(wxCommandEvent& event);

#if wxUSE_METAFILE
    void OnPasteMetafile(wxCommandEvent& event);
#endif // wxUSE_METAFILE

    void OnCopyFiles(wxCommandEvent& event);
    void OnCopyURL(wxCommandEvent& event);

    void OnUsePrimary(wxCommandEvent& event);

    void OnLeftDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

#if wxUSE_DRAG_AND_DROP
    void OnBeginDrag(wxTreeEvent& event);
#endif // wxUSE_DRAG_AND_DROP

    void OnUpdateUIMoveByDefault(wxUpdateUIEvent& event);

    void OnUpdateUIPasteText(wxUpdateUIEvent& event);
    void OnUpdateUIPasteBitmap(wxUpdateUIEvent& event);

private:
#if wxUSE_DRAG_AND_DROP
    // show the result of a dnd operation in the status bar
    void LogDragResult(wxDragResult result);
#endif // wxUSE_DRAG_AND_DROP


    // GUI controls
    wxListBox  *m_ctrlFile,
               *m_ctrlText;
    wxGenericDirCtrl *m_ctrlDir;

#if wxUSE_LOG
    wxTextCtrl *m_ctrlLog;

    wxLog *m_pLog,
          *m_pLogPrev;
#endif // wxUSE_LOG

    // move the text by default (or copy)?
    bool m_moveByDefault;

    // allow moving the text at all?
    bool m_moveAllow;

    // the text we drag
    wxString m_strText;


    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// A shape is an example of application-specific data which may be transported
// via drag-and-drop or clipboard: in our case, we have different geometric
// shapes, each one with its own colour and position
// ----------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

class DnDShape
{
public:
    enum Kind
    {
        None,
        Triangle,
        Rectangle,
        Ellipse
    };

    DnDShape(const wxPoint& pos,
             const wxSize& size,
             const wxColour& col)
        : m_pos(pos), m_size(size), m_col(col)
    {
    }

    // this is for debugging - lets us see when exactly an object is freed
    // (this may be later than you think if it's on the clipboard, for example)
    virtual ~DnDShape() { }

    // the functions used for drag-and-drop: they dump and restore a shape into
    // some bitwise-copiable data (might use streams too...)
    // ------------------------------------------------------------------------

    // restore from buffer
    static DnDShape *New(const void *buf);

    virtual size_t GetDataSize() const
    {
        return sizeof(ShapeDump);
    }

    virtual void GetDataHere(void *buf) const
    {
        ShapeDump& dump = *(ShapeDump *)buf;
        dump.x = m_pos.x;
        dump.y = m_pos.y;
        dump.w = m_size.x;
        dump.h = m_size.y;
        dump.r = m_col.Red();
        dump.g = m_col.Green();
        dump.b = m_col.Blue();
        dump.k = GetKind();
    }

    // accessors
    const wxPoint& GetPosition() const { return m_pos; }
    const wxColour& GetColour() const { return m_col; }
    const wxSize& GetSize() const { return m_size; }

    void Move(const wxPoint& pos) { m_pos = pos; }

    // to implement in derived classes
    virtual Kind GetKind() const = 0;

    virtual void Draw(wxDC& dc)
    {
        dc.SetPen(wxPen(m_col));
    }

protected:
    //get a point 1 up and 1 left, otherwise the mid-point of a triangle is on the line
    wxPoint GetCentre() const
         { return wxPoint(m_pos.x + m_size.x / 2 - 1, m_pos.y + m_size.y / 2 - 1); }

    struct ShapeDump
    {
        wxCoord x, y,             // position
                w, h;             // size
        int k;                    // kind
        unsigned char r, g, b;    // colour
    };

    wxPoint  m_pos;
    wxSize   m_size;
    wxColour m_col;
};

class DnDTriangularShape : public DnDShape
{
public:
    DnDTriangularShape(const wxPoint& pos,
                       const wxSize& size,
                       const wxColour& col)
        : DnDShape(pos, size, col)
    {
        wxLogMessage("DnDTriangularShape is being created");
    }

    virtual ~DnDTriangularShape()
    {
        wxLogMessage("DnDTriangularShape is being deleted");
    }

    virtual Kind GetKind() const wxOVERRIDE { return Triangle; }
    virtual void Draw(wxDC& dc) wxOVERRIDE
    {
        DnDShape::Draw(dc);

        // well, it's a bit difficult to describe a triangle by position and
        // size, but we're not doing geometry here, do we? ;-)
        wxPoint p1(m_pos);
        wxPoint p2(m_pos.x + m_size.x, m_pos.y);
        wxPoint p3(m_pos.x, m_pos.y + m_size.y);

        dc.DrawLine(p1, p2);
        dc.DrawLine(p2, p3);
        dc.DrawLine(p3, p1);

        //works in multicolor modes; on GTK (at least) will fail in 16-bit color
        dc.SetBrush(wxBrush(m_col));
        dc.FloodFill(GetCentre(), m_col, wxFLOOD_BORDER);
    }
};

class DnDRectangularShape : public DnDShape
{
public:
    DnDRectangularShape(const wxPoint& pos,
                        const wxSize& size,
                        const wxColour& col)
        : DnDShape(pos, size, col)
    {
        wxLogMessage("DnDRectangularShape is being created");
    }

    virtual ~DnDRectangularShape()
    {
        wxLogMessage("DnDRectangularShape is being deleted");
    }

    virtual Kind GetKind() const wxOVERRIDE { return Rectangle; }
    virtual void Draw(wxDC& dc) wxOVERRIDE
    {
        DnDShape::Draw(dc);

        wxPoint p1(m_pos);
        wxPoint p2(p1.x + m_size.x, p1.y);
        wxPoint p3(p2.x, p2.y + m_size.y);
        wxPoint p4(p1.x, p3.y);

        dc.DrawLine(p1, p2);
        dc.DrawLine(p2, p3);
        dc.DrawLine(p3, p4);
        dc.DrawLine(p4, p1);

        dc.SetBrush(wxBrush(m_col));
        dc.FloodFill(GetCentre(), m_col, wxFLOOD_BORDER);
    }
};

class DnDEllipticShape : public DnDShape
{
public:
    DnDEllipticShape(const wxPoint& pos,
                     const wxSize& size,
                     const wxColour& col)
        : DnDShape(pos, size, col)
    {
        wxLogMessage("DnDEllipticShape is being created");
    }

    virtual ~DnDEllipticShape()
    {
        wxLogMessage("DnDEllipticShape is being deleted");
    }

    virtual Kind GetKind() const wxOVERRIDE { return Ellipse; }
    virtual void Draw(wxDC& dc) wxOVERRIDE
    {
        DnDShape::Draw(dc);

        dc.DrawEllipse(m_pos, m_size);

        dc.SetBrush(wxBrush(m_col));
        dc.FloodFill(GetCentre(), m_col, wxFLOOD_BORDER);
    }
};

// ----------------------------------------------------------------------------
// A wxDataObject specialisation for the application-specific data
// ----------------------------------------------------------------------------

static wxString ShapeFormatId()
    {
        return "wxShape";
    }

class DnDShapeDataObject : public wxDataObject
{
public:
    // ctor doesn't copy the pointer, so it shouldn't go away while this object
    // is alive
    DnDShapeDataObject(DnDShape *shape = (DnDShape *)NULL)
    {
        if ( shape )
        {
            // we need to copy the shape because the one we're handled may be
            // deleted while it's still on the clipboard (for example) - and we
            // reuse the serialisation methods here to copy it
            void *buf = malloc(shape->DnDShape::GetDataSize());
            shape->GetDataHere(buf);
            m_shape = DnDShape::New(buf);

            free(buf);
        }
        else
        {
            // nothing to copy
            m_shape = NULL;
        }

        // this string should uniquely identify our format, but is otherwise
        // arbitrary
        m_formatShape.SetId(ShapeFormatId());

        // we don't draw the shape to a bitmap until it's really needed (i.e.
        // we're asked to do so)
        m_hasBitmap = false;
#if wxUSE_METAFILE
        m_hasMetaFile = false;
#endif // wxUSE_METAFILE
    }

    virtual ~DnDShapeDataObject() { delete m_shape; }

    // after a call to this function, the shape is owned by the caller and it
    // is responsible for deleting it!
    //
    // NB: a better solution would be to make DnDShapes ref counted and this
    //     is what should probably be done in a real life program, otherwise
    //     the ownership problems become too complicated really fast
    DnDShape *GetShape()
    {
        DnDShape *shape = m_shape;

        m_shape = (DnDShape *)NULL;
        m_hasBitmap = false;
#if wxUSE_METAFILE
        m_hasMetaFile = false;
#endif // wxUSE_METAFILE

        return shape;
    }

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxDataFormat GetPreferredFormat(Direction WXUNUSED(dir)) const wxOVERRIDE
    {
        return m_formatShape;
    }

    virtual size_t GetFormatCount(Direction dir) const wxOVERRIDE
    {
        // our custom format is supported by both GetData() and SetData()
        size_t nFormats = 1;
        if ( dir == Get )
        {
            // but the bitmap format(s) are only supported for output
            nFormats += m_dobjBitmap.GetFormatCount(dir);

#if wxUSE_METAFILE
            nFormats += m_dobjMetaFile.GetFormatCount(dir);
#endif // wxUSE_METAFILE
        }

        return nFormats;
    }

    virtual void GetAllFormats(wxDataFormat *formats, Direction dir) const wxOVERRIDE
    {
        formats[0] = m_formatShape;
        if ( dir == Get )
        {
            // in Get direction we additionally support bitmaps and metafiles
            // under Windows
            m_dobjBitmap.GetAllFormats(&formats[1], dir);

#if wxUSE_METAFILE
            // don't assume that m_dobjBitmap has only 1 format
            m_dobjMetaFile.GetAllFormats(&formats[1 +
                    m_dobjBitmap.GetFormatCount(dir)], dir);
#endif // wxUSE_METAFILE
        }
    }

    virtual size_t GetDataSize(const wxDataFormat& format) const wxOVERRIDE
    {
        if ( format == m_formatShape )
        {
            return m_shape->GetDataSize();
        }
#if wxUSE_METAFILE
        else if ( m_dobjMetaFile.IsSupported(format) )
        {
            if ( !m_hasMetaFile )
                CreateMetaFile();

            return m_dobjMetaFile.GetDataSize(format);
        }
#endif // wxUSE_METAFILE
        else
        {
            wxASSERT_MSG( m_dobjBitmap.IsSupported(format),
                          "unexpected format" );

            if ( !m_hasBitmap )
                CreateBitmap();

            return m_dobjBitmap.GetDataSize();
        }
    }

    virtual bool GetDataHere(const wxDataFormat& format, void *pBuf) const wxOVERRIDE
    {
        if ( format == m_formatShape )
        {
            m_shape->GetDataHere(pBuf);

            return true;
        }
#if wxUSE_METAFILE
        else if ( m_dobjMetaFile.IsSupported(format) )
        {
            if ( !m_hasMetaFile )
                CreateMetaFile();

            return m_dobjMetaFile.GetDataHere(format, pBuf);
        }
#endif // wxUSE_METAFILE
        else
        {
            wxASSERT_MSG( m_dobjBitmap.IsSupported(format),
                          "unexpected format" );

            if ( !m_hasBitmap )
                CreateBitmap();

            return m_dobjBitmap.GetDataHere(pBuf);
        }
    }

    virtual bool SetData(const wxDataFormat& format,
                         size_t WXUNUSED(len), const void *buf) wxOVERRIDE
    {
        wxCHECK_MSG( format == m_formatShape, false,
                     "unsupported format");

        delete m_shape;
        m_shape = DnDShape::New(buf);

        // the shape has changed
        m_hasBitmap = false;

#if wxUSE_METAFILE
        m_hasMetaFile = false;
#endif // wxUSE_METAFILE

        return true;
    }

private:
    // creates a bitmap and assigns it to m_dobjBitmap (also sets m_hasBitmap)
    void CreateBitmap() const;
#if wxUSE_METAFILE
    void CreateMetaFile() const;
#endif // wxUSE_METAFILE

    wxDataFormat        m_formatShape;  // our custom format

    wxBitmapDataObject  m_dobjBitmap;   // it handles bitmaps
    bool                m_hasBitmap;    // true if m_dobjBitmap has valid bitmap

#if wxUSE_METAFILE
    wxMetaFileDataObject m_dobjMetaFile;// handles metafiles
    bool                 m_hasMetaFile; // true if we have valid metafile
#endif // wxUSE_METAFILE

    DnDShape           *m_shape;        // our data
};

// ----------------------------------------------------------------------------
// A dialog to edit shape properties
// ----------------------------------------------------------------------------

class DnDShapeDialog : public wxDialog
{
public:
    DnDShapeDialog(wxFrame *parent, DnDShape *shape);

    DnDShape *GetShape() const;

    virtual bool TransferDataToWindow() wxOVERRIDE;
    virtual bool TransferDataFromWindow() wxOVERRIDE;

    void OnColour(wxCommandEvent& event);

private:
    // input
    DnDShape *m_shape;

    // output
    DnDShape::Kind m_shapeKind;
    wxPoint  m_pos;
    wxSize   m_size;
    wxColour m_col;

    // controls
    wxRadioBox *m_radio;
    wxTextCtrl *m_textX,
               *m_textY,
               *m_textW,
               *m_textH;

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// A frame for the shapes which can be drag-and-dropped between frames
// ----------------------------------------------------------------------------

class DnDShapeFrame : public wxFrame
{
public:
    DnDShapeFrame(wxFrame *parent);
    ~DnDShapeFrame();

    void SetShape(DnDShape *shape);
    virtual bool SetShape(const wxRegion &region)
    {
        return wxFrame::SetShape( region );
    }

    // callbacks
    void OnNewShape(wxCommandEvent& event);
    void OnEditShape(wxCommandEvent& event);
    void OnClearShape(wxCommandEvent& event);

    void OnCopyShape(wxCommandEvent& event);
    void OnPasteShape(wxCommandEvent& event);

    void OnUpdateUICopy(wxUpdateUIEvent& event);
    void OnUpdateUIPaste(wxUpdateUIEvent& event);

    void OnDrag(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnDrop(wxCoord x, wxCoord y, DnDShape *shape);

private:
    DnDShape *m_shape;

    static DnDShapeFrame *ms_lastDropTarget;

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// wxDropTarget derivation for DnDShapes
// ----------------------------------------------------------------------------

class DnDShapeDropTarget : public wxDropTarget
{
public:
    DnDShapeDropTarget(DnDShapeFrame *frame)
        : wxDropTarget(new DnDShapeDataObject)
    {
        m_frame = frame;
    }

    // override base class (pure) virtuals
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText("Mouse entered the frame");
#endif // wxUSE_STATUSBAR
        return OnDragOver(x, y, def);
    }
    virtual void OnLeave() wxOVERRIDE
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText("Mouse left the frame");
#endif // wxUSE_STATUSBAR
    }
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE
    {
        if ( !GetData() )
        {
            wxLogError("Failed to get drag and drop data");

            return wxDragNone;
        }

        m_frame->OnDrop(x, y,
                        ((DnDShapeDataObject *)GetDataObject())->GetShape());

        return def;
    }

private:
    DnDShapeFrame *m_frame;
};

#endif // wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// functions prototypes
// ----------------------------------------------------------------------------

static void ShowBitmap(const wxBitmap& bitmap);

#if wxUSE_METAFILE
static void ShowMetaFile(const wxMetaFile& metafile);
#endif // wxUSE_METAFILE

// ----------------------------------------------------------------------------
// IDs for the menu commands
// ----------------------------------------------------------------------------

enum
{
    Menu_Quit = 1,
    Menu_Drag,
    Menu_DragMoveDef,
    Menu_DragMoveAllow,
    Menu_NewFrame,
    Menu_About = 101,
    Menu_Help,
    Menu_Clear,
    Menu_Copy,
    Menu_Paste,
    Menu_CopyBitmap,
    Menu_PasteBitmap,
    Menu_PasteMFile,
    Menu_CopyFiles,
    Menu_CopyURL,
    Menu_UsePrimary,
    Menu_Shape_New = 500,
    Menu_Shape_Edit,
    Menu_Shape_Clear,
    Menu_ShapeClipboard_Copy,
    Menu_ShapeClipboard_Paste,
    Button_Colour = 1001
};

wxBEGIN_EVENT_TABLE(DnDFrame, wxFrame)
    EVT_MENU(Menu_Quit,       DnDFrame::OnQuit)
    EVT_MENU(Menu_About,      DnDFrame::OnAbout)
    EVT_MENU(Menu_Drag,       DnDFrame::OnDrag)
    EVT_MENU(Menu_DragMoveDef,  DnDFrame::OnDragMoveByDefault)
    EVT_MENU(Menu_DragMoveAllow,DnDFrame::OnDragMoveAllow)
    EVT_MENU(Menu_NewFrame,   DnDFrame::OnNewFrame)
    EVT_MENU(Menu_Help,       DnDFrame::OnHelp)
#if wxUSE_LOG
    EVT_MENU(Menu_Clear,      DnDFrame::OnLogClear)
#endif // wxUSE_LOG
    EVT_MENU(Menu_Copy,       DnDFrame::OnCopy)
    EVT_MENU(Menu_Paste,      DnDFrame::OnPaste)
    EVT_MENU(Menu_CopyBitmap, DnDFrame::OnCopyBitmap)
    EVT_MENU(Menu_PasteBitmap,DnDFrame::OnPasteBitmap)
#if wxUSE_METAFILE
    EVT_MENU(Menu_PasteMFile, DnDFrame::OnPasteMetafile)
#endif // wxUSE_METAFILE
    EVT_MENU(Menu_CopyFiles,  DnDFrame::OnCopyFiles)
    EVT_MENU(Menu_CopyURL,    DnDFrame::OnCopyURL)
    EVT_MENU(Menu_UsePrimary, DnDFrame::OnUsePrimary)

    EVT_UPDATE_UI(Menu_DragMoveDef, DnDFrame::OnUpdateUIMoveByDefault)

    EVT_UPDATE_UI(Menu_Paste,       DnDFrame::OnUpdateUIPasteText)
    EVT_UPDATE_UI(Menu_PasteBitmap, DnDFrame::OnUpdateUIPasteBitmap)

    EVT_LEFT_DOWN(            DnDFrame::OnLeftDown)
    EVT_RIGHT_DOWN(           DnDFrame::OnRightDown)
    EVT_PAINT(                DnDFrame::OnPaint)
    EVT_SIZE(                 DnDFrame::OnSize)
wxEND_EVENT_TABLE()

#if wxUSE_DRAG_AND_DROP

wxBEGIN_EVENT_TABLE(DnDShapeFrame, wxFrame)
    EVT_MENU(Menu_Shape_New,    DnDShapeFrame::OnNewShape)
    EVT_MENU(Menu_Shape_Edit,   DnDShapeFrame::OnEditShape)
    EVT_MENU(Menu_Shape_Clear,  DnDShapeFrame::OnClearShape)

    EVT_MENU(Menu_ShapeClipboard_Copy,  DnDShapeFrame::OnCopyShape)
    EVT_MENU(Menu_ShapeClipboard_Paste, DnDShapeFrame::OnPasteShape)

    EVT_UPDATE_UI(Menu_ShapeClipboard_Copy,  DnDShapeFrame::OnUpdateUICopy)
    EVT_UPDATE_UI(Menu_ShapeClipboard_Paste, DnDShapeFrame::OnUpdateUIPaste)

    EVT_LEFT_DOWN(DnDShapeFrame::OnDrag)

    EVT_PAINT(DnDShapeFrame::OnPaint)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(DnDShapeDialog, wxDialog)
    EVT_BUTTON(Button_Colour, DnDShapeDialog::OnColour)
wxEND_EVENT_TABLE()

#endif // wxUSE_DRAG_AND_DROP

wxBEGIN_EVENT_TABLE(DnDCanvasBitmap, wxScrolledWindow)
    EVT_PAINT(DnDCanvasBitmap::OnPaint)
wxEND_EVENT_TABLE()

#if wxUSE_METAFILE
wxBEGIN_EVENT_TABLE(DnDCanvasMetafile, wxScrolledWindow)
    EVT_PAINT(DnDCanvasMetafile::OnPaint)
wxEND_EVENT_TABLE()
#endif // wxUSE_METAFILE

#endif // wxUSE_DRAG_AND_DROP

// ============================================================================
// implementation
// ============================================================================

// `Main program' equivalent, creating windows and returning main app frame
bool DnDApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD
    // switch on trace messages
#if wxUSE_LOG
#if defined(__WXGTK__)
    wxLog::AddTraceMask("clipboard");
#elif defined(__WXMSW__)
    wxLog::AddTraceMask(wxTRACE_OleCalls);
#endif
#endif // wxUSE_LOG

#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    // create the main frame window
    new DnDFrame();

    return true;
#else
    wxMessageBox( "This sample has to be compiled with wxUSE_DRAG_AND_DROP", "Building error", wxOK);
    return false;
#endif // wxUSE_DRAG_AND_DROP
}

#if wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD

DnDFrame::DnDFrame()
        : wxFrame(NULL, wxID_ANY, "Drag-and-Drop/Clipboard wxWidgets Sample",
                  wxPoint(10, 100)),
          m_strText("wxWidgets drag & drop works :-)")

{
    // frame icon and status bar
    SetIcon(wxICON(sample));

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // construct menu
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(Menu_Drag, "&Test drag...");
    file_menu->AppendCheckItem(Menu_DragMoveDef, "&Move by default");
    file_menu->AppendCheckItem(Menu_DragMoveAllow, "&Allow moving");
    file_menu->AppendSeparator();
    file_menu->Append(Menu_NewFrame, "&New frame\tCtrl-N");
    file_menu->AppendSeparator();
    file_menu->Append(Menu_Quit, "E&xit\tCtrl-Q");

#if wxUSE_LOG
    wxMenu *log_menu = new wxMenu;
    log_menu->Append(Menu_Clear, "Clear\tCtrl-L");
#endif // wxUSE_LOG

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(Menu_Help, "&Help...");
    help_menu->AppendSeparator();
    help_menu->Append(Menu_About, "&About");

    wxMenu *clip_menu = new wxMenu;
    clip_menu->Append(Menu_Copy, "&Copy text\tCtrl-C");
    clip_menu->Append(Menu_Paste, "&Paste text\tCtrl-V");
    clip_menu->AppendSeparator();
    clip_menu->Append(Menu_CopyBitmap, "Copy &bitmap\tCtrl-Shift-C");
    clip_menu->Append(Menu_PasteBitmap, "Paste b&itmap\tCtrl-Shift-V");
#if wxUSE_METAFILE
    clip_menu->AppendSeparator();
    clip_menu->Append(Menu_PasteMFile, "Paste &metafile\tCtrl-M");
#endif // wxUSE_METAFILE
    clip_menu->AppendSeparator();
    clip_menu->Append(Menu_CopyFiles, "Copy &files\tCtrl-F");
    clip_menu->Append(Menu_CopyURL, "Copy &URL\tCtrl-U");
    clip_menu->AppendSeparator();
    clip_menu->AppendCheckItem(Menu_UsePrimary, "Use &primary selection\tCtrl-P");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
#if wxUSE_LOG
    menu_bar->Append(log_menu,  "&Log");
#endif // wxUSE_LOG
    menu_bar->Append(clip_menu, "&Clipboard");
    menu_bar->Append(help_menu, "&Help");

    SetMenuBar(menu_bar);

    // create the child controls
    SetBackgroundColour(*wxWHITE); // labels read better on this background

    wxString strFile("Drop files here!"), strText("Drop text on me");

    m_ctrlFile  = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, &strFile,
                                wxLB_HSCROLL | wxLB_ALWAYS_SB );
    m_ctrlText  = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, &strText,
                                wxLB_HSCROLL | wxLB_ALWAYS_SB );
    m_ctrlDir   = new wxGenericDirCtrl(this);

#if wxUSE_LOG
    m_ctrlLog   = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxTE_READONLY |
                                 wxSUNKEN_BORDER );

    // redirect log messages to the text window
    m_pLog = new wxLogTextCtrl(m_ctrlLog);
    m_pLogPrev = wxLog::SetActiveTarget(m_pLog);
#endif // wxUSE_LOG

#if wxUSE_DRAG_AND_DROP
    // associate drop targets with the controls
    m_ctrlFile->SetDropTarget(new DnDFile(m_ctrlFile));
    m_ctrlText->SetDropTarget(new DnDText(m_ctrlText));

#if wxUSE_DRAG_AND_DROP
    m_ctrlDir->Bind(wxEVT_TREE_BEGIN_DRAG, &DnDFrame::OnBeginDrag, this);
#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_LOG
    m_ctrlLog->SetDropTarget(new URLDropTarget);
#endif // wxUSE_LOG
#endif // wxUSE_DRAG_AND_DROP

    wxBoxSizer *sizer_top = new wxBoxSizer( wxHORIZONTAL );
    sizer_top->Add(m_ctrlFile, 1, wxEXPAND );
    sizer_top->Add(m_ctrlText, 1, wxEXPAND );

    wxBoxSizer *sizerDirCtrl = new wxBoxSizer(wxVERTICAL);
    sizerDirCtrl->Add(new wxStaticText(this, wxID_ANY, "Drag files from here"),
                      wxSizerFlags().Centre().Border());
    sizerDirCtrl->Add(m_ctrlDir, wxSizerFlags(1).Expand());
    sizer_top->Add(sizerDirCtrl, 1, wxEXPAND );

    // make all columns of reasonable minimal size
    for ( unsigned n = 0; n < sizer_top->GetChildren().size(); n++ )
        sizer_top->SetItemMinSize(n, 200, 300);

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add(sizer_top, 1, wxEXPAND );
#if wxUSE_LOG
    sizer->Add(m_ctrlLog, 1, wxEXPAND);
    sizer->SetItemMinSize(m_ctrlLog, 450, 200);
#endif // wxUSE_LOG
    sizer->AddSpacer(50);

    // copy data by default but allow moving it as well
    m_moveByDefault = false;
    m_moveAllow = true;
    menu_bar->Check(Menu_DragMoveAllow, true);

    // set the correct size and show the frame
    SetSizerAndFit(sizer);
    Show();
}

void DnDFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void DnDFrame::OnSize(wxSizeEvent& event)
{
    Refresh();

    event.Skip();
}

void DnDFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int w = 0;
    int h = 0;
    GetClientSize( &w, &h );

    wxPaintDC dc(this);
    dc.SetFont( wxFontInfo(24).Family(wxFONTFAMILY_DECORATIVE).FaceName("charter") );
    dc.DrawText( "Drag text from here!", 100, h-50 );
}

void DnDFrame::OnUpdateUIMoveByDefault(wxUpdateUIEvent& event)
{
    // only can move by default if moving is allowed at all
    event.Enable(m_moveAllow);
}

void DnDFrame::OnUpdateUIPasteText(wxUpdateUIEvent& event)
{
#ifdef __WXDEBUG__
    // too many trace messages if we don't do it - this function is called
    // very often
    wxLogNull nolog;
#endif

    event.Enable( wxTheClipboard->IsSupported(wxDF_TEXT)
#if wxUSE_UNICODE
        || wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
#endif
    );
}

void DnDFrame::OnUpdateUIPasteBitmap(wxUpdateUIEvent& event)
{
#ifdef __WXDEBUG__
    // too many trace messages if we don't do it - this function is called
    // very often
    wxLogNull nolog;
#endif

    event.Enable( wxTheClipboard->IsSupported(wxDF_BITMAP) );
}

void DnDFrame::OnNewFrame(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_DRAG_AND_DROP
    (new DnDShapeFrame(this))->Show(true);

    wxLogStatus(this, "Double click the new frame to select a shape for it");
#endif // wxUSE_DRAG_AND_DROP
}

void DnDFrame::OnDrag(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_DRAG_AND_DROP
    wxString strText = wxGetTextFromUser
        (
            "After you enter text in this dialog, press any mouse\n"
            "button in the bottom (empty) part of the frame and \n"
            "drag it anywhere - you will be in fact dragging the\n"
         "text object containing this text",
         "Please enter some text", m_strText, this
        );

    m_strText = strText;
#endif // wxUSE_DRAG_AND_DROP
}

void DnDFrame::OnDragMoveByDefault(wxCommandEvent& event)
{
    m_moveByDefault = event.IsChecked();
}

void DnDFrame::OnDragMoveAllow(wxCommandEvent& event)
{
    m_moveAllow = event.IsChecked();
}

void DnDFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Drag-&-Drop Demo\n"
                 "Please see \"Help|Help...\" for details\n"
                 "Copyright (c) 1998 Vadim Zeitlin",
                 "About wxDnD",
                 wxICON_INFORMATION | wxOK,
                 this);
}

void DnDFrame::OnHelp(wxCommandEvent& /* event */)
{
    wxMessageDialog dialog(this,
                           "This small program demonstrates drag & drop support in wxWidgets. The program window\n"
                           "consists of 3 parts: the bottom pane is for debug messages, so that you can see what's\n"
                           "going on inside. The top part is split into 2 listboxes, the left one accepts files\n"
                           "and the right one accepts text.\n"
                           "\n"
                           "To test wxDropTarget: open wordpad (write.exe), select some text in it and drag it to\n"
                           "the right listbox (you'll notice the usual visual feedback, i.e. the cursor will change).\n"
                           "Also, try dragging some files (you can select several at once) from Windows Explorer (or \n"
                           "File Manager) to the left pane. Hold down Ctrl/Shift keys when you drop text (doesn't \n"
                           "work with files) and see what changes.\n"
                           "\n"
                           "To test wxDropSource: just press any mouse button on the empty zone of the window and drag\n"
                           "it to wordpad or any other droptarget accepting text (and of course you can just drag it\n"
                           "to the right pane). Due to a lot of trace messages, the cursor might take some time to \n"
                           "change, don't release the mouse button until it does. You can change the string being\n"
                           "dragged in \"File|Test drag...\" dialog.\n"
                           "\n"
                           "\n"
                           "Please send all questions/bug reports/suggestions &c to \n"
                           "Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>",
                           "wxDnD Help");

    dialog.ShowModal();
}

#if wxUSE_LOG
void DnDFrame::OnLogClear(wxCommandEvent& /* event */ )
{
    m_ctrlLog->Clear();
    m_ctrlText->Clear();
    m_ctrlFile->Clear();
}
#endif // wxUSE_LOG

#if wxUSE_DRAG_AND_DROP

void DnDFrame::LogDragResult(wxDragResult result)
{
#if wxUSE_STATUSBAR
    wxString msg;
    switch ( result )
    {
        case wxDragError:   msg = "Error!";    break;
        case wxDragNone:    msg = "Nothing";   break;
        case wxDragCopy:    msg = "Copied";    break;
        case wxDragMove:    msg = "Moved";     break;
        case wxDragCancel:  msg = "Cancelled"; break;
        default:            msg = "Huh?";      break;
    }

    SetStatusText(wxString("Drag result: ") + msg);
#else
    wxUnusedVar(result);
#endif // wxUSE_STATUSBAR
}

#endif // wxUSE_DRAG_AND_DROP

void DnDFrame::OnLeftDown(wxMouseEvent &WXUNUSED(event) )
{
#if wxUSE_DRAG_AND_DROP
    if ( !m_strText.empty() )
    {
        // start drag operation
        wxTextDataObject textData(m_strText);
        wxDropSource source(textData, this,
                            wxDROP_ICON(dnd_copy),
                            wxDROP_ICON(dnd_move),
                            wxDROP_ICON(dnd_none));

        int flags = 0;
        if ( m_moveByDefault )
            flags |= wxDrag_DefaultMove;
        else if ( m_moveAllow )
            flags |= wxDrag_AllowMove;

        LogDragResult(source.DoDragDrop(flags));
    }
#endif // wxUSE_DRAG_AND_DROP
}

void DnDFrame::OnRightDown(wxMouseEvent &event )
{
    wxMenu menu("Dnd sample menu");

    menu.Append(Menu_Drag, "&Test drag...");
    menu.AppendSeparator();
    menu.Append(Menu_About, "&About");

    PopupMenu( &menu, event.GetX(), event.GetY() );
}

DnDFrame::~DnDFrame()
{
#if wxUSE_LOG
    if ( m_pLog != NULL ) {
        if ( wxLog::SetActiveTarget(m_pLogPrev) == m_pLog )
            delete m_pLog;
    }
#endif // wxUSE_LOG
}

void DnDFrame::OnUsePrimary(wxCommandEvent& event)
{
    const bool usePrimary = event.IsChecked();
    wxTheClipboard->UsePrimarySelection(usePrimary);

    wxLogStatus("Now using %s selection", usePrimary ? "primary"
                                                         : "clipboard");
}

#if wxUSE_DRAG_AND_DROP

void DnDFrame::OnBeginDrag(wxTreeEvent& event)
{
    wxFileDataObject data;
    data.AddFile(m_ctrlDir->GetPath(event.GetItem()));

    wxDropSource dragSource(this);
    dragSource.SetData(data);

    LogDragResult(dragSource.DoDragDrop());
}

#endif // wxUSE_DRAG_AND_DROP

// ---------------------------------------------------------------------------
// bitmap clipboard
// ---------------------------------------------------------------------------

void DnDFrame::OnCopyBitmap(wxCommandEvent& WXUNUSED(event))
{
    // PNG support is not always compiled in under Windows, so use BMP there
#if wxUSE_LIBPNG
    wxFileDialog dialog(this, "Open a PNG file", wxEmptyString, wxEmptyString, "PNG files (*.png)|*.png", 0);
#else
    wxFileDialog dialog(this, "Open a BMP file", wxEmptyString, wxEmptyString, "BMP files (*.bmp)|*.bmp", 0);
#endif

    if (dialog.ShowModal() != wxID_OK)
    {
        wxLogMessage( "Aborted file open" );
        return;
    }

    if (dialog.GetPath().empty())
    {
        wxLogMessage( "Returned empty string." );
        return;
    }

    if (!wxFileExists(dialog.GetPath()))
    {
        wxLogMessage( "File doesn't exist." );
        return;
    }

    wxImage image;
    image.LoadFile( dialog.GetPath(),
#if wxUSE_LIBPNG
                    wxBITMAP_TYPE_PNG
#else
                    wxBITMAP_TYPE_BMP
#endif
                  );
    if (!image.IsOk())
    {
        wxLogError( "Invalid image file..." );
        return;
    }

    wxLogStatus( "Decoding image file..." );
    wxYield();

    wxBitmap bitmap( image );

    if ( !wxTheClipboard->Open() )
    {
        wxLogError("Can't open clipboard.");

        return;
    }

    wxLogMessage( "Creating wxBitmapDataObject..." );
    wxYield();

    if ( !wxTheClipboard->AddData(new wxBitmapDataObject(bitmap)) )
    {
        wxLogError("Can't copy image to the clipboard.");
    }
    else
    {
        wxLogMessage("Image has been put on the clipboard." );
        wxLogMessage("You can paste it now and look at it." );
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnPasteBitmap(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError("Can't open clipboard.");

        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_BITMAP) )
    {
        wxLogWarning("No bitmap on clipboard");

        wxTheClipboard->Close();
        return;
    }

    wxBitmapDataObject data;
    if ( !wxTheClipboard->GetData(data) )
    {
        wxLogError("Can't paste bitmap from the clipboard");
    }
    else
    {
        const wxBitmap& bmp = data.GetBitmap();

        wxLogMessage("Bitmap %dx%d pasted from the clipboard",
                     bmp.GetWidth(), bmp.GetHeight());
        ShowBitmap(bmp);
    }

    wxTheClipboard->Close();
}

#if wxUSE_METAFILE

void DnDFrame::OnPasteMetafile(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError("Can't open clipboard.");

        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_METAFILE) )
    {
        wxLogWarning("No metafile on clipboard");
    }
    else
    {
        wxMetaFileDataObject data;
        if ( !wxTheClipboard->GetData(data) )
        {
            wxLogError("Can't paste metafile from the clipboard");
        }
        else
        {
            const wxMetaFile& mf = data.GetMetafile();

            wxLogMessage("Metafile %dx%d pasted from the clipboard",
                         mf.GetWidth(), mf.GetHeight());

            ShowMetaFile(mf);
        }
    }

    wxTheClipboard->Close();
}

#endif // wxUSE_METAFILE

// ----------------------------------------------------------------------------
// file clipboard
// ----------------------------------------------------------------------------

void DnDFrame::OnCopyFiles(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxFileDialog dialog(this, "Select a file to copy", wxEmptyString, wxEmptyString,
                         "All files (*.*)|*.*", 0);

    wxArrayString filenames;
    while ( dialog.ShowModal() == wxID_OK )
    {
        filenames.Add(dialog.GetPath());
    }

    if ( !filenames.IsEmpty() )
    {
        wxFileDataObject *dobj = new wxFileDataObject;
        size_t count = filenames.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            dobj->AddFile(filenames[n]);
        }

        wxClipboardLocker locker;
        if ( !locker )
        {
            wxLogError("Can't open clipboard");
        }
        else
        {
            if ( !wxTheClipboard->AddData(dobj) )
            {
                wxLogError("Can't copy file(s) to the clipboard");
            }
            else
            {
                wxLogStatus(this, "%d file%s copied to the clipboard",
                            count, count == 1 ? wxEmptyString : wxEmptyString);
            }
        }
    }
    else
    {
        wxLogStatus(this, "Aborted");
    }
#else // !MSW
    wxLogError("Sorry, not implemented");
#endif // MSW/!MSW
}

void DnDFrame::OnCopyURL(wxCommandEvent& WXUNUSED(event))
{
    // Just hard code it for now, we could ask the user but the point here is
    // to test copying URLs, it doesn't really matter what it is.
    const wxString url("http://www.wxwidgets.org/");

    wxClipboardLocker locker;
    if ( !!locker && wxTheClipboard->AddData(new wxURLDataObject(url)) )
    {
        wxLogStatus(this, "Copied URL \"%s\" to %s.",
                    url,
                    GetMenuBar()->IsChecked(Menu_UsePrimary)
                        ? "primary selection"
                        : "clipboard");
    }
    else
    {
        wxLogError("Failed to copy URL.");
    }
}

// ---------------------------------------------------------------------------
// text clipboard
// ---------------------------------------------------------------------------

void DnDFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError("Can't open clipboard.");

        return;
    }

    if ( !wxTheClipboard->AddData(new wxTextDataObject(m_strText)) )
    {
        wxLogError("Can't copy data to the clipboard");
    }
    else
    {
        wxLogMessage("Text '%s' put on the clipboard", m_strText);
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError("Can't open clipboard.");

        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_TEXT)
#if wxUSE_UNICODE
        && !wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
#endif
    )
    {
        wxLogWarning("No text data on clipboard");

        wxTheClipboard->Close();
        return;
    }

    wxTextDataObject text;
    if ( !wxTheClipboard->GetData(text) )
    {
        wxLogError("Can't paste data from the clipboard");
    }
    else
    {
        wxLogMessage("Text '%s' pasted from the clipboard",
                     text.GetText());
    }

    wxTheClipboard->Close();
}

#if wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// Notifications called by the base class
// ----------------------------------------------------------------------------

bool DnDText::OnDropText(wxCoord, wxCoord, const wxString& text)
{
    m_pOwner->Append(text);

    return true;
}

bool DnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();
    wxString str;
    str.Printf( "%d files dropped", (int)nFiles);

    if (m_pOwner != NULL)
    {
        m_pOwner->Append(str);
        for ( size_t n = 0; n < nFiles; n++ )
            m_pOwner->Append(filenames[n]);
    }

    return true;
}

// ----------------------------------------------------------------------------
// DnDShapeDialog
// ----------------------------------------------------------------------------

DnDShapeDialog::DnDShapeDialog(wxFrame *parent, DnDShape *shape)
  :wxDialog( parent, 6001, "Choose Shape", wxPoint( 10, 10 ),
             wxSize( 40, 40 ),
             wxDEFAULT_DIALOG_STYLE | wxRAISED_BORDER | wxRESIZE_BORDER )
{
    m_shape = shape;
    wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );

    // radio box
    wxBoxSizer* shapesSizer = new wxBoxSizer( wxHORIZONTAL );
    const wxString choices[] = { "None", "Triangle",
                                 "Rectangle", "Ellipse" };

    m_radio = new wxRadioBox( this, wxID_ANY, "&Shape",
                              wxDefaultPosition, wxDefaultSize, 4, choices, 4,
                              wxRA_SPECIFY_COLS );
    shapesSizer->Add( m_radio, 0, wxGROW|wxALL, 5 );
    topSizer->Add( shapesSizer, 0, wxALL, 2 );

    // attributes
    wxStaticBox* box = new wxStaticBox( this, wxID_ANY, "&Attributes" );
    wxStaticBoxSizer* attrSizer = new wxStaticBoxSizer( box, wxHORIZONTAL );
    wxFlexGridSizer* xywhSizer = new wxFlexGridSizer( 2 );

    wxStaticText* st;

    st = new wxStaticText( this, wxID_ANY, "Position &X:" );
    m_textX = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize( 30, 20 ) );
    xywhSizer->Add( st, 1, wxGROW|wxALL, 2 );
    xywhSizer->Add( m_textX, 1, wxGROW|wxALL, 2 );

    st = new wxStaticText( this, wxID_ANY, "Size &width:" );
    m_textW = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize( 30, 20 ) );
    xywhSizer->Add( st, 1, wxGROW|wxALL, 2 );
    xywhSizer->Add( m_textW, 1, wxGROW|wxALL, 2 );

    st = new wxStaticText( this, wxID_ANY, "&Y:" );
    m_textY = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize( 30, 20 ) );
    xywhSizer->Add( st, 1, wxALL|wxALIGN_RIGHT, 2 );
    xywhSizer->Add( m_textY, 1, wxGROW|wxALL, 2 );

    st = new wxStaticText( this, wxID_ANY, "&height:" );
    m_textH = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize( 30, 20 ) );
    xywhSizer->Add( st, 1, wxALL|wxALIGN_RIGHT, 2 );
    xywhSizer->Add( m_textH, 1, wxGROW|wxALL, 2 );

    wxButton* col = new wxButton( this, Button_Colour, "&Colour..." );
    attrSizer->Add( xywhSizer, 1, wxGROW );
    attrSizer->Add( col, 0, wxALL|wxALIGN_CENTRE_VERTICAL, 2 );
    topSizer->Add( attrSizer, 0, wxGROW|wxALL, 5 );

    // buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton* bt;
    bt = new wxButton( this, wxID_OK, "Ok" );
    buttonSizer->Add( bt, 0, wxALL, 2 );
    bt = new wxButton( this, wxID_CANCEL, "Cancel" );
    buttonSizer->Add( bt, 0, wxALL, 2 );
    topSizer->Add( buttonSizer, 0, wxALL|wxALIGN_RIGHT, 2 );

    SetSizerAndFit( topSizer );
}

DnDShape *DnDShapeDialog::GetShape() const
{
    switch ( m_shapeKind )
    {
        default:
        case DnDShape::None:      return NULL;
        case DnDShape::Triangle:  return new DnDTriangularShape(m_pos, m_size, m_col);
        case DnDShape::Rectangle: return new DnDRectangularShape(m_pos, m_size, m_col);
        case DnDShape::Ellipse:   return new DnDEllipticShape(m_pos, m_size, m_col);
    }
}

bool DnDShapeDialog::TransferDataToWindow()
{

    if ( m_shape )
    {
        m_radio->SetSelection(m_shape->GetKind());
        m_pos = m_shape->GetPosition();
        m_size = m_shape->GetSize();
        m_col = m_shape->GetColour();
    }
    else
    {
        m_radio->SetSelection(DnDShape::None);
        m_pos = wxPoint(1, 1);
        m_size = wxSize(100, 100);
    }

    m_textX->SetValue(wxString() << m_pos.x);
    m_textY->SetValue(wxString() << m_pos.y);
    m_textW->SetValue(wxString() << m_size.x);
    m_textH->SetValue(wxString() << m_size.y);

    return true;
}

bool DnDShapeDialog::TransferDataFromWindow()
{
    m_shapeKind = (DnDShape::Kind)m_radio->GetSelection();

    m_pos.x = wxAtoi(m_textX->GetValue());
    m_pos.y = wxAtoi(m_textY->GetValue());
    m_size.x = wxAtoi(m_textW->GetValue());
    m_size.y = wxAtoi(m_textH->GetValue());

    if ( !m_pos.x || !m_pos.y || !m_size.x || !m_size.y )
    {
        wxMessageBox("All sizes and positions should be non null!",
                     "Invalid shape", wxICON_HAND | wxOK, this);

        return false;
    }

    return true;
}

void DnDShapeDialog::OnColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    if ( dialog.ShowModal() == wxID_OK )
    {
        m_col = dialog.GetColourData().GetColour();
    }
}

// ----------------------------------------------------------------------------
// DnDShapeFrame
// ----------------------------------------------------------------------------

DnDShapeFrame *DnDShapeFrame::ms_lastDropTarget = NULL;

DnDShapeFrame::DnDShapeFrame(wxFrame *parent)
             : wxFrame(parent, wxID_ANY, "Shape Frame")
{
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    wxMenu *menuShape = new wxMenu;
    menuShape->Append(Menu_Shape_New, "&New default shape\tCtrl-S");
    menuShape->Append(Menu_Shape_Edit, "&Edit shape\tCtrl-E");
    menuShape->AppendSeparator();
    menuShape->Append(Menu_Shape_Clear, "&Clear shape\tCtrl-L");

    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(Menu_ShapeClipboard_Copy, "&Copy\tCtrl-C");
    menuClipboard->Append(Menu_ShapeClipboard_Paste, "&Paste\tCtrl-V");

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuShape, "&Shape");
    menubar->Append(menuClipboard, "&Clipboard");

    SetMenuBar(menubar);

#if wxUSE_STATUSBAR
    SetStatusText("Press Ctrl-S to create a new shape");
#endif // wxUSE_STATUSBAR

    SetDropTarget(new DnDShapeDropTarget(this));

    m_shape = NULL;

    SetBackgroundColour(*wxWHITE);
}

DnDShapeFrame::~DnDShapeFrame()
{
    if (m_shape)
        delete m_shape;
}

void DnDShapeFrame::SetShape(DnDShape *shape)
{
    if (m_shape)
        delete m_shape;
    m_shape = shape;
    Refresh();
}

// callbacks
void DnDShapeFrame::OnDrag(wxMouseEvent& event)
{
    if ( !m_shape )
    {
        event.Skip();

        return;
    }

    // start drag operation
    DnDShapeDataObject shapeData(m_shape);
    wxDropSource source(shapeData, this);

    wxString msg;
    switch ( source.DoDragDrop(true) )
    {
        default:
        case wxDragError:
            wxLogError("An error occurred during drag and drop operation");
            break;

        case wxDragNone:
#if wxUSE_STATUSBAR
            SetStatusText("Nothing happened");
#endif // wxUSE_STATUSBAR
            break;

        case wxDragCopy:
            msg = "copied";
            break;

        case wxDragMove:
            msg = "moved";
            if ( ms_lastDropTarget != this )
            {
                // don't delete the shape if we dropped it on ourselves!
                SetShape(NULL);
            }
            break;

        case wxDragCancel:
#if wxUSE_STATUSBAR
            SetStatusText("Drag and drop operation cancelled");
#endif // wxUSE_STATUSBAR
            break;
    }

    if (msg.length() )
    {
#if wxUSE_STATUSBAR
        SetStatusText(wxString("Shape successfully ") + msg);
#endif // wxUSE_STATUSBAR
    }
    //else: status text already set
}

void DnDShapeFrame::OnDrop(wxCoord x, wxCoord y, DnDShape *shape)
{
    ms_lastDropTarget = this;

    wxPoint pt(x, y);

#if wxUSE_STATUSBAR
    wxString s;
    s.Printf("Shape dropped at (%d, %d)", pt.x, pt.y);
    SetStatusText(s);
#endif // wxUSE_STATUSBAR

    shape->Move(pt);
    SetShape(shape);
}

void DnDShapeFrame::OnEditShape(wxCommandEvent& WXUNUSED(event))
{
    DnDShapeDialog dlg(this, m_shape);
    if ( dlg.ShowModal() == wxID_OK )
    {
        SetShape(dlg.GetShape());

#if wxUSE_STATUSBAR
        if ( m_shape )
        {
            SetStatusText("You can now drag the shape to another frame");
        }
#endif // wxUSE_STATUSBAR
    }
}

void DnDShapeFrame::OnNewShape(wxCommandEvent& WXUNUSED(event))
{
    SetShape(new DnDEllipticShape(wxPoint(10, 10), wxSize(80, 60), *wxRED));

#if wxUSE_STATUSBAR
    SetStatusText("You can now drag the shape to another frame");
#endif // wxUSE_STATUSBAR
}

void DnDShapeFrame::OnClearShape(wxCommandEvent& WXUNUSED(event))
{
    SetShape(NULL);
}

void DnDShapeFrame::OnCopyShape(wxCommandEvent& WXUNUSED(event))
{
    if ( m_shape )
    {
        wxClipboardLocker clipLocker;
        if ( !clipLocker )
        {
            wxLogError("Can't open the clipboard");

            return;
        }

        wxTheClipboard->AddData(new DnDShapeDataObject(m_shape));
    }
}

void DnDShapeFrame::OnPasteShape(wxCommandEvent& WXUNUSED(event))
{
    wxClipboardLocker clipLocker;
    if ( !clipLocker )
    {
        wxLogError("Can't open the clipboard");

        return;
    }

    DnDShapeDataObject shapeDataObject(NULL);
    if ( wxTheClipboard->GetData(shapeDataObject) )
    {
        SetShape(shapeDataObject.GetShape());
    }
    else
    {
        wxLogStatus("No shape on the clipboard");
    }
}

void DnDShapeFrame::OnUpdateUICopy(wxUpdateUIEvent& event)
{
    event.Enable( m_shape != NULL );
}

void DnDShapeFrame::OnUpdateUIPaste(wxUpdateUIEvent& event)
{
    event.Enable( wxTheClipboard->IsSupported(wxDataFormat(ShapeFormatId())) );
}

void DnDShapeFrame::OnPaint(wxPaintEvent& event)
{
    if ( m_shape )
    {
        wxPaintDC dc(this);

        m_shape->Draw(dc);
    }
    else
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// DnDShape
// ----------------------------------------------------------------------------

DnDShape *DnDShape::New(const void *buf)
{
    const ShapeDump& dump = *(const ShapeDump *)buf;
    switch ( dump.k )
    {
        case Triangle:
            return new DnDTriangularShape(wxPoint(dump.x, dump.y),
                                          wxSize(dump.w, dump.h),
                                          wxColour(dump.r, dump.g, dump.b));

        case Rectangle:
            return new DnDRectangularShape(wxPoint(dump.x, dump.y),
                                           wxSize(dump.w, dump.h),
                                           wxColour(dump.r, dump.g, dump.b));

        case Ellipse:
            return new DnDEllipticShape(wxPoint(dump.x, dump.y),
                                        wxSize(dump.w, dump.h),
                                        wxColour(dump.r, dump.g, dump.b));

        default:
            wxFAIL_MSG("invalid shape!");
            return NULL;
    }
}

// ----------------------------------------------------------------------------
// DnDShapeDataObject
// ----------------------------------------------------------------------------

#if wxUSE_METAFILE

void DnDShapeDataObject::CreateMetaFile() const
{
    wxPoint pos = m_shape->GetPosition();
    wxSize size = m_shape->GetSize();

    wxMetaFileDC dcMF(wxEmptyString, pos.x + size.x, pos.y + size.y);

    m_shape->Draw(dcMF);

    wxMetafile *mf = dcMF.Close();

    DnDShapeDataObject* self = const_cast<DnDShapeDataObject*>(this);
    self->m_dobjMetaFile.SetMetafile(*mf);
    self->m_hasMetaFile = true;

    delete mf;
}

#endif // wxUSE_METAFILE

void DnDShapeDataObject::CreateBitmap() const
{
    wxPoint pos = m_shape->GetPosition();
    wxSize size = m_shape->GetSize();
    int x = pos.x + size.x,
        y = pos.y + size.y;
    wxBitmap bitmap(x, y);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.Clear();
    m_shape->Draw(dc);
    dc.SelectObject(wxNullBitmap);

    DnDShapeDataObject* self = const_cast<DnDShapeDataObject*>(this);
    self->m_dobjBitmap.SetBitmap(bitmap);
    self->m_hasBitmap = true;
}

#endif // wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static void ShowBitmap(const wxBitmap& bitmap)
{
    wxFrame *frame = new wxFrame(NULL, wxID_ANY, "Bitmap view");
#if wxUSE_STATUSBAR
    frame->CreateStatusBar();
#endif // wxUSE_STATUSBAR
    DnDCanvasBitmap *canvas = new DnDCanvasBitmap(frame);
    canvas->SetBitmap(bitmap);

    int w = bitmap.GetWidth(),
        h = bitmap.GetHeight();
#if wxUSE_STATUSBAR
    frame->SetStatusText(wxString::Format("%dx%d", w, h));
#endif // wxUSE_STATUSBAR

    frame->SetClientSize(w > 100 ? 100 : w, h > 100 ? 100 : h);
    frame->Show(true);
}

#if wxUSE_METAFILE

static void ShowMetaFile(const wxMetaFile& metafile)
{
    wxFrame *frame = new wxFrame(NULL, wxID_ANY, "Metafile view");
    frame->CreateStatusBar();
    DnDCanvasMetafile *canvas = new DnDCanvasMetafile(frame);
    canvas->SetMetafile(metafile);

    wxSize size = metafile.GetSize();
    frame->SetStatusText(wxString::Format("%dx%d", size.x, size.y));

    frame->SetClientSize(size.x > 100 ? 100 : size.x,
                         size.y > 100 ? 100 : size.y);
    frame->Show();
}

#endif // wxUSE_METAFILE

#endif // wxUSE_DRAG_AND_DROP || wxUSE_CLIPBOARD
