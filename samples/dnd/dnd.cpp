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

#ifdef __WXMOTIF__
    #error Sorry, drag and drop is not yet implemented on wxMotif.
#endif

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/dnd.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "wx/image.h"
#include "wx/clipbrd.h"

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
    bool OnInit();
};

IMPLEMENT_APP(DnDApp);

// ----------------------------------------------------------------------------
// Define a new frame type
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
// IDs for the menu commands
// ----------------------------------------------------------------------------

enum
{
    Menu_Quit = 1,
    Menu_Drag,
    Menu_About = 101,
    Menu_Help,
    Menu_Clear,
    Menu_Copy,
    Menu_Paste,
    Menu_CopyBitmap,
    Menu_PasteBitmap,
    Menu_HasText,
    Menu_HasBitmap
};

BEGIN_EVENT_TABLE(DnDFrame, wxFrame)
    EVT_MENU(Menu_Quit,       DnDFrame::OnQuit)
    EVT_MENU(Menu_About,      DnDFrame::OnAbout)
    EVT_MENU(Menu_Drag,       DnDFrame::OnDrag)
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
    // redirect log messages to the text window (don't forget to delete it!)
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
        wxLogMessage( _T("No text is on the clipboard") );
    }
    else
    {
        wxLogMessage( _T("There is text is on the clipboard") );
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
        wxLogMessage( _T("No bitmap is on the clipboard") );
    }
    else
    {
        wxLogMessage( _T("A bitmap is on the clipboard") );
    }

    wxTheClipboard->Close();
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
#ifdef __WXMSW__
        wxTextDataObject textData(m_strText);
        wxDropSource dragSource( textData, this );
#else
        wxDropSource dragSource( new wxTextDataObject (m_strText), this, wxIcon(mondrian_xpm) );
#endif
        const char *pc;

        switch ( dragSource.DoDragDrop(TRUE) ) 
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
    wxFileDialog dialog(this, "Open a PNG file", "", "", "PNG files (*.png)|*.png", 0);

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
    image.LoadFile( dialog.GetPath(), wxBITMAP_TYPE_PNG );
    if (!image.Ok())
    {
        wxLogMessage( _T("Invalid image file...") );
        return;
    }
    
    wxLogMessage( _T("Decoding image file...") );
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
