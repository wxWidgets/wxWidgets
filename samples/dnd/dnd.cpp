/////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     Drag and drop sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DRAG_AND_DROP
    #error This sample requires drag and drop support in the library
#endif

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/dnd.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "wx/image.h"
#include "wx/clipbrd.h"
#include "wx/colordlg.h"
#include "wx/resource.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// Derive two simple classes which just put in the listbox the strings (text or
// file names) we drop on them
// ----------------------------------------------------------------------------

typedef long wxDropPointCoord;

class DnDText : public wxTextDropTarget
{
public:
    DnDText(wxListBox *pOwner) { m_pOwner = pOwner; }

    virtual bool OnDropText(wxDropPointCoord x, wxDropPointCoord y,
                            const wxChar* psz);

private:
    wxListBox *m_pOwner;
};

class DnDFile : public wxFileDropTarget
{
public:
    DnDFile(wxListBox *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropFiles(wxDropPointCoord x, wxDropPointCoord y,
                           size_t nFiles, const wxChar* const aszFiles[] );

private:
    wxListBox *m_pOwner;
};

// ----------------------------------------------------------------------------
// Define a new application type
// ----------------------------------------------------------------------------

class DnDApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(DnDApp);

// ----------------------------------------------------------------------------
// Define a new frame type for the main frame
// ----------------------------------------------------------------------------

class DnDFrame : public wxFrame
{
public:
    DnDFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    ~DnDFrame();

    void OnPaint(wxPaintEvent& event);
    void OnQuit (wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDrag (wxCommandEvent& event);
    void OnNewFrame(wxCommandEvent& event);
    void OnHelp (wxCommandEvent& event);
    void OnLogClear(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnCopyBitmap(wxCommandEvent& event);
    void OnPasteBitmap(wxCommandEvent& event);
    void OnClipboardHasText(wxCommandEvent& event);
    void OnClipboardHasBitmap(wxCommandEvent& event);

    void OnLeftDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()

private:
    wxListBox  *m_ctrlFile,
    *m_ctrlText;
    wxTextCtrl *m_ctrlLog;

    wxLog *m_pLog, *m_pLogPrev;

    wxString  m_strText;
    wxBitmap  m_bitmap;
};

// ----------------------------------------------------------------------------
// A shape is an example of application-specific data which may be transported
// via drag-and-drop or clipboard: in our case, we have different geometric
// shapes, each one with its own colour and position
// ----------------------------------------------------------------------------

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

    // the functions used for drag-and-drop: they dump and restore a shape into
    // some bitwise-copiable data
    //
    // NB: here we profit from the fact that wxPoint, wxSize and wxColour are
    //     POD (plain old data) and so can be copied directly - but it wouldn't
    //     work for other types!
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

    // to implement in derived classes
    virtual Kind GetKind() const = 0;

    virtual void Draw(wxDC& dc) = 0
    {
        dc.SetPen(wxPen(m_col, 1, wxSOLID));
    }

protected:
    wxPoint GetCentre() const
        { return wxPoint(m_pos.x + m_size.x / 2, m_pos.y + m_size.y / 2); }

    struct ShapeDump
    {
        int x, y,       // position
            w, h,       // size
            r, g, b,    // colour
            k;          // kind
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
    }

    virtual Kind GetKind() const { return Triangle; }
    virtual void Draw(wxDC& dc)
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
    }

    virtual Kind GetKind() const { return Rectangle; }
    virtual void Draw(wxDC& dc)
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
    }

    virtual Kind GetKind() const { return Ellipse; }
    virtual void Draw(wxDC& dc)
    {
        DnDShape::Draw(dc);

        dc.DrawEllipse(m_pos, m_size);

        dc.FloodFill(GetCentre(), m_col, wxFLOOD_BORDER);
    }
};

// ----------------------------------------------------------------------------
// A wxDataObject specialisation for the application-specific data
// ----------------------------------------------------------------------------

static const char *shapeFormatId = "wxShape";

class DnDShapeDataObject : public wxDataObject
{
public:
    // ctor doesn't copy the pointer, so it shouldn't go away while this object
    // is alive
    DnDShapeDataObject(DnDShape *shape)
    {
        m_shape = shape;

        // this string should uniquely identify our format, but is otherwise
        // arbitrary
        m_formatShape.SetId(shapeFormatId);

        // we don't draw the shape to a bitmap until it's really needed (i.e.
        // we're asked to do so)
        m_hasBitmap = FALSE;
    }

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxDataFormat GetPreferredFormat() const
    {
        return m_formatShape;
    }

    virtual size_t GetFormatCount() const
    {
        // +1 for our custom format
        return m_dataobj.GetFormatCount() + 1;
    }

    virtual void GetAllFormats(wxDataFormat *formats) const
    {
        formats[0] = m_formatShape;
        m_dataobj.GetAllFormats(&formats[1]);
    }

    virtual size_t GetDataSize(const wxDataFormat& format) const
    {
        if ( format == m_formatShape )
        {
            return m_shape->GetDataSize();
        }
        else
        {
            wxASSERT_MSG( format == wxDF_BITMAP, "unsupported format" );

            if ( !m_hasBitmap )
                CreateBitmap();

            return m_dataobj.GetDataSize(format);
        }
    }

    virtual void GetDataHere(const wxDataFormat& format, void *pBuf) const
    {
        if ( format == m_formatShape )
        {
            m_shape->GetDataHere(pBuf);
        }
        else
        {
            wxASSERT_MSG( format == wxDF_BITMAP, "unsupported format" );

            if ( !m_hasBitmap )
                CreateBitmap();

            m_dataobj.GetDataHere(format, pBuf);
        }
    }

private:
    // creates a bitmap and assigns it to m_dataobj (also sets m_hasBitmap)
    void CreateBitmap() const;

    wxDataFormat        m_formatShape;  // our custom format

    wxBitmapDataObject  m_dataobj;      // it handles bitmaps
    bool                m_hasBitmap;    // true if m_dataobj has valid bitmap

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

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

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

    DECLARE_EVENT_TABLE()
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

    // callbacks
    void OnDrag(wxMouseEvent& event);
    void OnEdit(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnDrop(long x, long y, DnDShape *shape);

private:
    DnDShape *m_shape;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxDropTarget derivation for DnDShapes
// ----------------------------------------------------------------------------

class DnDShapeDropTarget : public wxDropTarget
{
public:
    DnDShapeDropTarget(DnDShapeFrame *frame)
    {
        m_frame = frame;

        // the same as used by DnDShapeDataObject
        m_formatShape.SetId(shapeFormatId);
    }

    // override base class (pure) virtuals
    virtual void OnEnter()
        { m_frame->SetStatusText("Mouse entered the frame"); }
    virtual void OnLeave()
        { m_frame->SetStatusText("Mouse left the frame"); }
    virtual bool OnDrop(long x, long y, const void *pData)
    {
        m_frame->OnDrop(x, y, DnDShape::New(pData));

        return TRUE;
    }

protected:
    virtual size_t GetFormatCount() const { return 1; }
    virtual wxDataFormat GetFormat(size_t WXUNUSED(n)) const
        { return m_formatShape; }

private:
    DnDShapeFrame *m_frame;
    wxDataFormat m_formatShape;
};

// ----------------------------------------------------------------------------
// IDs for the menu commands
// ----------------------------------------------------------------------------

enum
{
    Menu_Quit = 1,
    Menu_Drag,
    Menu_NewFrame,
    Menu_About = 101,
    Menu_Help,
    Menu_Clear,
    Menu_Copy,
    Menu_Paste,
    Menu_CopyBitmap,
    Menu_PasteBitmap,
    Menu_HasText,
    Menu_HasBitmap,
    Menu_ToBeGreyed,   /* for testing */
    Menu_ToBeDeleted,  /* for testing */
    Button_Colour = 1001
};

BEGIN_EVENT_TABLE(DnDFrame, wxFrame)
    EVT_MENU(Menu_Quit,       DnDFrame::OnQuit)
    EVT_MENU(Menu_About,      DnDFrame::OnAbout)
    EVT_MENU(Menu_Drag,       DnDFrame::OnDrag)
    EVT_MENU(Menu_NewFrame,   DnDFrame::OnNewFrame)
    EVT_MENU(Menu_Help,       DnDFrame::OnHelp)
    EVT_MENU(Menu_Clear,      DnDFrame::OnLogClear)
    EVT_MENU(Menu_Copy,       DnDFrame::OnCopy)
    EVT_MENU(Menu_Paste,      DnDFrame::OnPaste)
    EVT_MENU(Menu_CopyBitmap, DnDFrame::OnCopyBitmap)
    EVT_MENU(Menu_PasteBitmap,DnDFrame::OnPasteBitmap)
    EVT_MENU(Menu_HasText,    DnDFrame::OnClipboardHasText)
    EVT_MENU(Menu_HasBitmap,  DnDFrame::OnClipboardHasBitmap)

    EVT_LEFT_DOWN(            DnDFrame::OnLeftDown)
    EVT_RIGHT_DOWN(           DnDFrame::OnRightDown)
    EVT_PAINT(                DnDFrame::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(DnDShapeFrame, wxFrame)
    EVT_RIGHT_DOWN(DnDShapeFrame::OnDrag)
    EVT_LEFT_DCLICK(DnDShapeFrame::OnEdit)
    EVT_PAINT(DnDShapeFrame::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(DnDShapeDialog, wxDialog)
    EVT_BUTTON(Button_Colour, OnColour)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// `Main program' equivalent, creating windows and returning main app frame
bool DnDApp::OnInit()
{
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    // create the main frame window
    DnDFrame *frame = new DnDFrame((wxFrame  *) NULL,
                                   "Drag-and-Drop/Clipboard wxWindows Sample",
                                   50, 50, 450, 340);

    // activate it
    frame->Show(TRUE);

    SetTopWindow(frame);

    wxDefaultResourceTable->ParseResourceFile("dnd.wxr");

    return TRUE;
}

DnDFrame::DnDFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
        : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h)),
          m_strText("wxWindows drag & drop works :-)")

{
    // frame icon and status bar
    SetIcon(wxICON(mondrian));

    CreateStatusBar();

    // construct menu
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(Menu_Drag, "&Test drag...");
    file_menu->AppendSeparator();
    file_menu->Append(Menu_NewFrame, "&New frame\tCtrl-N");
    file_menu->AppendSeparator();
    file_menu->Append(Menu_Quit, "E&xit");

    wxMenu *log_menu = new wxMenu;
    log_menu->Append(Menu_Clear, "Clear");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(Menu_Help, "&Help...");
    help_menu->AppendSeparator();
    help_menu->Append(Menu_About, "&About");

    wxMenu *clip_menu = new wxMenu;
    clip_menu->Append(Menu_Copy, "&Copy text\tCtrl+C");
    clip_menu->Append(Menu_Paste, "&Paste text\tCtrl+V");
    clip_menu->AppendSeparator();
    clip_menu->Append(Menu_CopyBitmap, "&Copy bitmap");
    clip_menu->Append(Menu_PasteBitmap, "&Paste bitmap");
    clip_menu->AppendSeparator();
    clip_menu->Append(Menu_HasText, "Clipboard has &text\tCtrl+T");
    clip_menu->Append(Menu_HasBitmap, "Clipboard has a &bitmap\tCtrl+B");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(log_menu,  "&Log");
    menu_bar->Append(clip_menu, "&Clipboard");
    menu_bar->Append(help_menu, "&Help");

    SetMenuBar(menu_bar);

    // make a panel with 3 subwindows
    wxPoint pos(0, 0);
    wxSize  size(400, 200);

    wxString strFile("Drop files here!"), strText("Drop text on me");

    m_ctrlFile  = new wxListBox(this, -1, pos, size, 1, &strFile, wxLB_HSCROLL | wxLB_ALWAYS_SB );
    m_ctrlText  = new wxListBox(this, -1, pos, size, 1, &strText, wxLB_HSCROLL | wxLB_ALWAYS_SB );

    m_ctrlLog   = new wxTextCtrl(this, -1, "", pos, size,
                               wxTE_MULTILINE | wxTE_READONLY |
                               wxSUNKEN_BORDER );

    // redirect log messages to the text window and switch on OLE messages
    // logging
    wxLog::AddTraceMask(wxTRACE_OleCalls);
    m_pLog = new wxLogTextCtrl(m_ctrlLog);
    m_pLogPrev = wxLog::SetActiveTarget(m_pLog);

    // associate drop targets with 2 text controls
    m_ctrlFile->SetDropTarget(new DnDFile(m_ctrlFile));
    m_ctrlText->SetDropTarget(new DnDText(m_ctrlText));

    wxLayoutConstraints *c;

    // Top-left listbox
    c = new wxLayoutConstraints;
    c->left.SameAs(this, wxLeft);
    c->top.SameAs(this, wxTop);
    c->right.PercentOf(this, wxRight, 50);
    c->height.PercentOf(this, wxHeight, 30);
    m_ctrlFile->SetConstraints(c);

    // Top-right listbox
    c = new wxLayoutConstraints;
    c->left.SameAs    (m_ctrlFile, wxRight);
    c->top.SameAs     (this, wxTop);
    c->right.SameAs   (this, wxRight);
    c->height.PercentOf(this, wxHeight, 30);
    m_ctrlText->SetConstraints(c);

    // Lower text control
    c = new wxLayoutConstraints;
    c->left.SameAs    (this, wxLeft);
    c->right.SameAs   (this, wxRight);
    c->height.PercentOf(this, wxHeight, 30);
    c->top.SameAs(m_ctrlText, wxBottom);
    m_ctrlLog->SetConstraints(c);

    SetAutoLayout(TRUE);
}

void DnDFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void DnDFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int w = 0;
    int h = 0;
    GetClientSize( &w, &h );

    wxPaintDC dc(this);
    dc.SetFont( wxFont( 24, wxDECORATIVE, wxNORMAL, wxNORMAL, FALSE, "charter" ) );
    dc.DrawText( "Drag text from here!", 20, h-50 );

    if (m_bitmap.Ok())
        dc.DrawBitmap( m_bitmap, 280, h-120, TRUE );
}

void DnDFrame::OnClipboardHasText(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError( _T("Can't open clipboard.") );

        return;
    }

    if ( !wxTheClipboard->IsSupported( wxDF_TEXT ) )
    {
        wxLogMessage( _T("No text on the clipboard") );
    }
    else
    {
        wxLogMessage( _T("There is text on the clipboard") );
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnClipboardHasBitmap(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError( _T("Can't open clipboard.") );

        return;
    }

    if ( !wxTheClipboard->IsSupported( wxDF_BITMAP ) )
    {
        wxLogMessage( _T("No bitmap on the clipboard") );
    }
    else
    {
        wxLogMessage( _T("There is a bitmap on the clipboard") );
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnNewFrame(wxCommandEvent& WXUNUSED(event))
{
    (new DnDShapeFrame(this))->Show(TRUE);

    wxLogStatus(this, "Double click the new frame to select a shape for it");
}

void DnDFrame::OnDrag(wxCommandEvent& WXUNUSED(event))
{
    wxString strText = wxGetTextFromUser
        (
         "After you enter text in this dialog, press any mouse\n"
         "button in the bottom (empty) part of the frame and \n"
         "drag it anywhere - you will be in fact dragging the\n"
         "text object containing this text",
         "Please enter some text", m_strText, this
        );

    m_strText = strText;
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
            "This small program demonstrates drag & drop support in wxWindows. The program window\n"
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
            "dragged in in \"File|Test drag...\" dialog.\n"
            "\n"
            "\n"
            "Please send all questions/bug reports/suggestions &c to \n"
            "Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>",
            "wxDnD Help");

    dialog.ShowModal();
}

void DnDFrame::OnLogClear(wxCommandEvent& /* event */ )
{
    m_ctrlLog->Clear();
}

void DnDFrame::OnLeftDown(wxMouseEvent &WXUNUSED(event) )
{
    if ( !m_strText.IsEmpty() )
    {
        // start drag operation
        wxTextDataObject textData(m_strText);
        wxDropSource source(textData, this, wxICON(mondrian));

        const char *pc;

        switch ( source.DoDragDrop(TRUE) )
        {
            case wxDragError:   pc = "Error!";    break;
            case wxDragNone:    pc = "Nothing";   break;
            case wxDragCopy:    pc = "Copied";    break;
            case wxDragMove:    pc = "Moved";     break;
            case wxDragCancel:  pc = "Cancelled"; break;
            default:            pc = "Huh?";      break;
        }

        SetStatusText(wxString("Drag result: ") + pc);
    }
}

void DnDFrame::OnRightDown(wxMouseEvent &event )
{
    wxMenu *menu = new wxMenu;

    menu->Append(Menu_Drag, "&Test drag...");
    menu->Append(Menu_About, "&About");
    menu->Append(Menu_Quit, "E&xit");
    menu->Append(Menu_ToBeDeleted, "To be deleted");
    menu->Append(Menu_ToBeGreyed, "To be greyed");

    menu->Delete( Menu_ToBeDeleted );
    menu->Enable( Menu_ToBeGreyed, FALSE );

    PopupMenu( menu, event.GetX(), event.GetY() );
}

DnDFrame::~DnDFrame()
{
    if ( m_pLog != NULL ) {
        if ( wxLog::SetActiveTarget(m_pLogPrev) == m_pLog )
            delete m_pLog;
    }
}

// ---------------------------------------------------------------------------
// bitmap clipboard
// ---------------------------------------------------------------------------

void DnDFrame::OnCopyBitmap(wxCommandEvent& WXUNUSED(event))
{
    // PNG support is not always compiled in under Windows, so use BMP there
#ifdef __WXMSW__
    wxFileDialog dialog(this, "Open a BMP file", "", "", "BMP files (*.bmp)|*.bmp", 0);
#else
    wxFileDialog dialog(this, "Open a PNG file", "", "", "PNG files (*.png)|*.png", 0);
#endif

    if (dialog.ShowModal() != wxID_OK)
    {
        wxLogMessage( _T("Aborted file open") );
        return;
    }

    if (dialog.GetPath().IsEmpty())
    {
        wxLogMessage( _T("Returned empty string.") );
        return;
    }

    if (!wxFileExists(dialog.GetPath()))
    {
        wxLogMessage( _T("File doesn't exist.") );
        return;
    }

    wxImage image;
    image.LoadFile( dialog.GetPath(),
#ifdef __WXMSW__
                    wxBITMAP_TYPE_BMP
#else
                    wxBITMAP_TYPE_PNG
#endif
                  );
    if (!image.Ok())
    {
        wxLogError( _T("Invalid image file...") );
        return;
    }

    wxLogStatus( _T("Decoding image file...") );
    wxYield();

    wxBitmap bitmap( image.ConvertToBitmap() );

    if ( !wxTheClipboard->Open() )
    {
        wxLogError(_T("Can't open clipboard."));

        return;
    }

    wxLogMessage( _T("Creating wxBitmapDataObject...") );
    wxYield();

    if ( !wxTheClipboard->AddData(new wxBitmapDataObject(bitmap)) )
    {
        wxLogError(_T("Can't copy image to the clipboard."));
    }
    else
    {
        wxLogMessage(_T("Image has been put on the clipboard.") );
        wxLogMessage(_T("You can paste it now and look at it.") );
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnPasteBitmap(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError(_T("Can't open clipboard."));

        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_BITMAP) )
    {
        wxLogWarning(_T("No bitmap on clipboard"));

        wxTheClipboard->Close();
        return;
    }

    wxBitmapDataObject data;
    if ( !wxTheClipboard->GetData(&data) )
    {
        wxLogError(_T("Can't paste bitmap from the clipboard"));
    }
    else
    {
        wxLogMessage(_T("Bitmap pasted from the clipboard") );
        m_bitmap = data.GetBitmap();
        Refresh();
    }

    wxTheClipboard->Close();
}

// ---------------------------------------------------------------------------
// text clipboard
// ---------------------------------------------------------------------------

void DnDFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError(_T("Can't open clipboard."));

        return;
    }

    if ( !wxTheClipboard->AddData(new wxTextDataObject(m_strText)) )
    {
        wxLogError(_T("Can't copy data to the clipboard"));
    }
    else
    {
        wxLogMessage(_T("Text '%s' put on the clipboard"), m_strText.c_str());
    }

    wxTheClipboard->Close();
}

void DnDFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxTheClipboard->Open() )
    {
        wxLogError(_T("Can't open clipboard."));

        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_TEXT) )
    {
        wxLogWarning(_T("No text data on clipboard"));

        wxTheClipboard->Close();
        return;
    }

    wxTextDataObject text;
    if ( !wxTheClipboard->GetData(&text) )
    {
        wxLogError(_T("Can't paste data from the clipboard"));
    }
    else
    {
        wxLogMessage(_T("Text '%s' pasted from the clipboard"),
                     text.GetText().c_str());
    }

    wxTheClipboard->Close();
}

// ----------------------------------------------------------------------------
// Notifications called by the base class
// ----------------------------------------------------------------------------

bool DnDText::OnDropText( wxDropPointCoord, wxDropPointCoord, const wxChar *psz )
{
    m_pOwner->Append(psz);

    return TRUE;
}

bool DnDFile::OnDropFiles( wxDropPointCoord, wxDropPointCoord, size_t nFiles,
                           const wxChar* const aszFiles[])
{
    wxString str;
    str.Printf( _T("%d files dropped"), nFiles);
    m_pOwner->Append(str);
    for ( size_t n = 0; n < nFiles; n++ ) {
        m_pOwner->Append(aszFiles[n]);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// DnDShapeDialog
// ----------------------------------------------------------------------------

DnDShapeDialog::DnDShapeDialog(wxFrame *parent, DnDShape *shape)
{
    m_shape = shape;

    LoadFromResource(parent, "dialogShape");

    m_textX = (wxTextCtrl *)wxFindWindowByName("textX", this);
    m_textY = (wxTextCtrl *)wxFindWindowByName("textY", this);
    m_textW = (wxTextCtrl *)wxFindWindowByName("textW", this);
    m_textH = (wxTextCtrl *)wxFindWindowByName("textH", this);

    m_radio = (wxRadioBox *)wxFindWindowByName("radio", this);
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

    return TRUE;
}

bool DnDShapeDialog::TransferDataFromWindow()
{
    m_shapeKind = (DnDShape::Kind)m_radio->GetSelection();

    m_pos.x = atoi(m_textX->GetValue());
    m_pos.y = atoi(m_textY->GetValue());
    m_size.x = atoi(m_textW->GetValue());
    m_size.y = atoi(m_textH->GetValue());

    if ( !m_pos.x || !m_pos.y || !m_size.x || !m_size.y )
    {
        wxMessageBox("All sizes and positions should be non null!",
                     "Invalid shape", wxICON_HAND | wxOK, this);

        return FALSE;
    }

    return TRUE;
}

void DnDShapeDialog::OnColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(TRUE);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour(i*16, i*16, i*16);
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

DnDShapeFrame::DnDShapeFrame(wxFrame *parent)
             : wxFrame(parent, -1, "Shape Frame",
                       wxDefaultPosition, wxSize(250, 150))
{
    SetBackgroundColour(*wxWHITE);

    CreateStatusBar();

    SetStatusText("Double click the frame to create a shape");

    SetDropTarget(new DnDShapeDropTarget(this));

    m_shape = NULL;
}

DnDShapeFrame::~DnDShapeFrame()
{
    delete m_shape;
}

void DnDShapeFrame::SetShape(DnDShape *shape)
{
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
    wxDropSource source(shapeData, this, wxICON(mondrian));

    const char *pc = NULL;
    switch ( source.DoDragDrop(TRUE) )
    {
        default:
        case wxDragError:
            wxLogError("An error occured during drag and drop operation");
            break;

        case wxDragNone:
            SetStatusText("Nothing happened");
            break;

        case wxDragCopy:
            pc = "copied";
            break;

        case wxDragMove:
            pc = "moved";
            SetShape(NULL);
            break;

        case wxDragCancel:
            SetStatusText("Drag and drop operation cancelled");
            break;
    }

    if ( pc )
    {
        SetStatusText(wxString("Shape successfully ") + pc);
    }
    //else: status text already set
}

void DnDShapeFrame::OnEdit(wxMouseEvent& event)
{
    DnDShapeDialog dlg(this, m_shape);
    if ( dlg.ShowModal() == wxID_OK )
    {
        SetShape(dlg.GetShape());

        if ( m_shape )
        {
            SetStatusText("Right click now drag the shape to another frame");
        }
    }
}

void DnDShapeFrame::OnPaint(wxPaintEvent& event)
{
    if ( m_shape )
        m_shape->Draw(wxPaintDC(this));
    else
        event.Skip();
}

void DnDShapeFrame::OnDrop(long x, long y, DnDShape *shape)
{
    wxString s;
    s.Printf("Drop occured at (%ld, %ld)", x, y);
    SetStatusText(s);

    SetShape(shape);
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

void DnDShapeDataObject::CreateBitmap() const
{
    wxBitmap bitmap;
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    m_shape->Draw(dc);
    dc.SelectObject(wxNullBitmap);

    DnDShapeDataObject *self = (DnDShapeDataObject *)this; // const_cast
    self->m_dataobj.SetBitmap(bitmap);
    self->m_hasBitmap = TRUE;
}

