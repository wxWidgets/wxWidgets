///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/image.cpp
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2005 Robert Roebling
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"
#include "wx/quantize.h"
#include "wx/stopwatch.h"

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif // wxUSE_CLIPBOARD

#if defined(__WXMSW__)
    #ifdef wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
    #endif
#endif

#if defined(__WXMAC__) || defined(__WXGTK__)
    #define wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
#endif

#include "canvas.h"


// ============================================================================
// declarations
// ============================================================================

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnNewFrame( wxCommandEvent &event );
    void OnImageInfo( wxCommandEvent &event );
    void OnThumbnail( wxCommandEvent &event );

#ifdef wxHAVE_RAW_BITMAP
    void OnTestRawBitmap( wxCommandEvent &event );
#endif // wxHAVE_RAW_BITMAP
    void OnQuit( wxCommandEvent &event );

#if wxUSE_CLIPBOARD
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
#endif // wxUSE_CLIPBOARD

    MyCanvas         *m_canvas;

private:
    // ask user for the file name and try to load an image from it
    //
    // return the file path on success, empty string if we failed to load the
    // image or were cancelled by user
    static wxString LoadUserImage(wxImage& image);


    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// Frame used for showing a standalone image
// ----------------------------------------------------------------------------

enum
{
    ID_ROTATE_LEFT = wxID_HIGHEST+1,
    ID_ROTATE_RIGHT,
    ID_RESIZE,
    ID_PAINT_BG
};

class MyImageFrame : public wxFrame
{
public:
    MyImageFrame(wxFrame *parent, const wxString& desc, const wxBitmap& bitmap)
        : wxFrame(parent, wxID_ANY,
                wxString::Format(_T("Image from %s"), desc.c_str()),
                wxDefaultPosition, wxDefaultSize,
                wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE),
                m_bitmap(bitmap)
    {
        wxMenu *menu = new wxMenu;
        menu->Append(wxID_SAVE);
        menu->AppendSeparator();
        m_pClearBgMenu = menu->AppendCheckItem(ID_PAINT_BG, _T("&Paint background"));
        menu->AppendSeparator();
        menu->Append(ID_RESIZE, _T("&Fit to window\tCtrl-F"));
        menu->AppendSeparator();
        menu->Append(ID_ROTATE_LEFT, _T("Rotate &left\tCtrl-L"));
        menu->Append(ID_ROTATE_RIGHT, _T("Rotate &right\tCtrl-R"));

        wxMenuBar *mbar = new wxMenuBar;
        mbar->Append(menu, _T("&Image"));
        SetMenuBar(mbar);

        CreateStatusBar();

        SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());

        UpdateStatusBar();

//        SetBackgroundColour(*wxWHITE);
    }

    void OnEraseBackground(wxEraseEvent& WXUNUSED(event))
    {
        // do nothing here to be able to see how transparent images are shown
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        if (m_pClearBgMenu->IsChecked())
            ClearBackground();

        const wxSize size = GetClientSize();
        dc.DrawBitmap(m_bitmap,
                    (size.x - m_bitmap.GetWidth())/2,
                    (size.y - m_bitmap.GetHeight())/2,
                    true /* use mask */);
    }

    void OnSave(wxCommandEvent& WXUNUSED(event))
    {
#if wxUSE_FILEDLG
        wxImage image = m_bitmap.ConvertToImage();

        wxString savefilename = wxFileSelector( wxT("Save Image"),
                                                wxEmptyString,
                                                wxEmptyString,
                                                (const wxChar *)NULL,
                                                wxT("BMP files (*.bmp)|*.bmp|")
                                                wxT("PNG files (*.png)|*.png|")
                                                wxT("JPEG files (*.jpg)|*.jpg|")
                                                wxT("GIF files (*.gif)|*.gif|")
                                                wxT("TIFF files (*.tif)|*.tif|")
                                                wxT("PCX files (*.pcx)|*.pcx|")
                                                wxT("ICO files (*.ico)|*.ico|")
                                                wxT("CUR files (*.cur)|*.cur"),
                                                wxFD_SAVE,
                                                this);

        if ( savefilename.empty() )
            return;

        wxString extension;
        wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

        bool saved = false;
        if ( extension == _T("bmp") )
        {
            static const int bppvalues[] =
            {
                wxBMP_1BPP,
                wxBMP_1BPP_BW,
                wxBMP_4BPP,
                wxBMP_8BPP,
                wxBMP_8BPP_GREY,
                wxBMP_8BPP_RED,
                wxBMP_8BPP_PALETTE,
                wxBMP_24BPP
            };

            const wxString bppchoices[] =
            {
                _T("1 bpp color"),
                _T("1 bpp B&W"),
                _T("4 bpp color"),
                _T("8 bpp color"),
                _T("8 bpp greyscale"),
                _T("8 bpp red"),
                _T("8 bpp own palette"),
                _T("24 bpp")
            };

            int bppselection = wxGetSingleChoiceIndex(_T("Set BMP BPP"),
                                                    _T("Image sample: save file"),
                                                    WXSIZEOF(bppchoices),
                                                    bppchoices,
                                                    this);
            if ( bppselection != -1 )
            {
                int format = bppvalues[bppselection];
                image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, format);

                if ( format == wxBMP_8BPP_PALETTE )
                {
                    unsigned char *cmap = new unsigned char [256];
                    for ( int i = 0; i < 256; i++ )
                        cmap[i] = (unsigned char)i;
                    image.SetPalette(wxPalette(256, cmap, cmap, cmap));

                    delete[] cmap;
                }
            }
        }
        else if ( extension == _T("png") )
        {
            static const int pngvalues[] =
            {
                wxPNG_TYPE_COLOUR,
                wxPNG_TYPE_COLOUR,
                wxPNG_TYPE_GREY,
                wxPNG_TYPE_GREY,
                wxPNG_TYPE_GREY_RED,
                wxPNG_TYPE_GREY_RED,
            };

            const wxString pngchoices[] =
            {
                _T("Colour 8bpp"),
                _T("Colour 16bpp"),
                _T("Grey 8bpp"),
                _T("Grey 16bpp"),
                _T("Grey red 8bpp"),
                _T("Grey red 16bpp"),
            };

            int sel = wxGetSingleChoiceIndex(_T("Set PNG format"),
                                            _T("Image sample: save file"),
                                            WXSIZEOF(pngchoices),
                                            pngchoices,
                                            this);
            if ( sel != -1 )
            {
                image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, pngvalues[sel]);
                image.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, sel % 2 ? 16 : 8);
            }
        }
        else if ( extension == _T("cur") )
        {
            image.Rescale(32,32);
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
            // This shows how you can save an image with explicitly
            // specified image format:
            saved = image.SaveFile(savefilename, wxBITMAP_TYPE_CUR);
        }

        if ( !saved )
        {
            // This one guesses image format from filename extension
            // (it may fail if the extension is not recognized):
            image.SaveFile(savefilename);
        }
#endif // wxUSE_FILEDLG
    }

    void OnResize(wxCommandEvent& WXUNUSED(event))
    {
        wxImage img(m_bitmap.ConvertToImage());

        const wxSize size = GetClientSize();
        img.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
        m_bitmap = wxBitmap(img);

        UpdateStatusBar();
        Refresh();
    }

    void OnRotate(wxCommandEvent& event)
    {
        double angle = 5;
        if ( event.GetId() == ID_ROTATE_LEFT )
            angle = -angle;

        wxImage img(m_bitmap.ConvertToImage());
        img = img.Rotate(angle, wxPoint(img.GetWidth() / 2, img.GetHeight() / 2));
        if ( !img.Ok() )
        {
            wxLogWarning(_T("Rotation failed"));
            return;
        }

        m_bitmap = wxBitmap(img);

        UpdateStatusBar();
        Refresh();
    }

private:
    void UpdateStatusBar()
    {
        wxLogStatus(this, _T("Image size: (%d, %d)"),
                    m_bitmap.GetWidth(),
                    m_bitmap.GetHeight());
    }

    wxBitmap m_bitmap;
    wxMenuItem* m_pClearBgMenu;

    DECLARE_EVENT_TABLE()
};

#ifdef wxHAVE_RAW_BITMAP

#include "wx/rawbmp.h"

class MyRawBitmapFrame : public wxFrame
{
public:
    enum
    {
        BORDER = 15,
        SIZE = 150,
        REAL_SIZE = SIZE - 2*BORDER
    };

    MyRawBitmapFrame(wxFrame *parent)
        : wxFrame(parent, wxID_ANY, _T("Raw bitmaps (how exciting)")),
        m_bitmap(SIZE, SIZE, 24),
        m_alphaBitmap(SIZE, SIZE, 32)
    {
        SetClientSize(SIZE, SIZE*2+25);

        InitAlphaBitmap();
        InitBitmap();

    }

    void InitAlphaBitmap()
    {
        // First, clear the whole bitmap by making it alpha
        {
            wxAlphaPixelData data( m_alphaBitmap, wxPoint(0,0), wxSize(SIZE, SIZE) );
            if ( !data )
            {
                wxLogError(_T("Failed to gain raw access to bitmap data"));
                return;
            }
            wxAlphaPixelData::Iterator p(data);
            for ( int y = 0; y < SIZE; ++y )
            {
                wxAlphaPixelData::Iterator rowStart = p;
                for ( int x = 0; x < SIZE; ++x )
                {
                    p.Alpha() = 0;
                    ++p; // same as p.OffsetX(1)
                }
                p = rowStart;
                p.OffsetY(data, 1);
            }
        }

        // Then, draw colourful alpha-blended stripes
        wxAlphaPixelData data(m_alphaBitmap, wxPoint(BORDER, BORDER),
                            wxSize(REAL_SIZE, REAL_SIZE));
        if ( !data )
        {
            wxLogError(_T("Failed to gain raw access to bitmap data"));
            return;
        }

        wxAlphaPixelData::Iterator p(data);

        for ( int y = 0; y < REAL_SIZE; ++y )
        {
            wxAlphaPixelData::Iterator rowStart = p;

            int r = y < REAL_SIZE/3 ? 255 : 0,
                g = (REAL_SIZE/3 <= y) && (y < 2*(REAL_SIZE/3)) ? 255 : 0,
                b = 2*(REAL_SIZE/3) <= y ? 255 : 0;

            for ( int x = 0; x < REAL_SIZE; ++x )
            {
                // note that RGB must be premultiplied by alpha
                unsigned a = (wxAlphaPixelData::Iterator::ChannelType)((x*255.)/REAL_SIZE);
                p.Red() = r * a / 256;
                p.Green() = g * a / 256;
                p.Blue() = b * a / 256;
                p.Alpha() = a;

                ++p; // same as p.OffsetX(1)
            }

            p = rowStart;
            p.OffsetY(data, 1);
        }
    }

    void InitBitmap()
    {
        // draw some colourful stripes without alpha
        wxNativePixelData data(m_bitmap);
        if ( !data )
        {
            wxLogError(_T("Failed to gain raw access to bitmap data"));
            return;
        }

        wxNativePixelData::Iterator p(data);
        for ( int y = 0; y < SIZE; ++y )
        {
            wxNativePixelData::Iterator rowStart = p;

            int r = y < SIZE/3 ? 255 : 0,
                g = (SIZE/3 <= y) && (y < 2*(SIZE/3)) ? 255 : 0,
                b = 2*(SIZE/3) <= y ? 255 : 0;

            for ( int x = 0; x < SIZE; ++x )
            {
                p.Red() = r;
                p.Green() = g;
                p.Blue() = b;
                ++p; // same as p.OffsetX(1)
            }

            p = rowStart;
            p.OffsetY(data, 1);
        }
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc( this );
        dc.DrawText(_T("This is alpha and raw bitmap test"), 0, BORDER);
        dc.DrawText(_T("This is alpha and raw bitmap test"), 0, SIZE/2 - BORDER);
        dc.DrawText(_T("This is alpha and raw bitmap test"), 0, SIZE - 2*BORDER);
        dc.DrawBitmap( m_alphaBitmap, 0, 0, true /* use mask */ );

        dc.DrawText(_T("Raw bitmap access without alpha"), 0, SIZE+5);
        dc.DrawBitmap( m_bitmap, 0, SIZE+5+dc.GetCharHeight());
    }

private:
    wxBitmap m_bitmap;
    wxBitmap m_alphaBitmap;

    DECLARE_EVENT_TABLE()
};

#endif // wxHAVE_RAW_BITMAP


// ============================================================================
// implementations
// ============================================================================

//-----------------------------------------------------------------------------
// MyImageFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyImageFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyImageFrame::OnEraseBackground)
    EVT_PAINT(MyImageFrame::OnPaint)

    EVT_MENU(wxID_SAVE, MyImageFrame::OnSave)
    EVT_MENU_RANGE(ID_ROTATE_LEFT, ID_ROTATE_RIGHT, MyImageFrame::OnRotate)
    EVT_MENU(ID_RESIZE, MyImageFrame::OnResize)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
// MyRawBitmapFrame
//-----------------------------------------------------------------------------

#ifdef wxHAVE_RAW_BITMAP

BEGIN_EVENT_TABLE(MyRawBitmapFrame, wxFrame)
    EVT_PAINT(MyRawBitmapFrame::OnPaint)
END_EVENT_TABLE()

#endif // wxHAVE_RAW_BITMAP

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

enum
{
    ID_QUIT  = wxID_EXIT,
    ID_ABOUT = wxID_ABOUT,
    ID_NEW = 100,
    ID_INFO,
    ID_SHOWRAW,
    ID_SHOWTHUMBNAIL
};

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
    EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
    EVT_MENU    (ID_NEW,   MyFrame::OnNewFrame)
    EVT_MENU    (ID_INFO,  MyFrame::OnImageInfo)
    EVT_MENU    (ID_SHOWTHUMBNAIL, MyFrame::OnThumbnail)
#ifdef wxHAVE_RAW_BITMAP
    EVT_MENU    (ID_SHOWRAW, MyFrame::OnTestRawBitmap)
#endif
#if wxUSE_CLIPBOARD
    EVT_MENU(wxID_COPY, MyFrame::OnCopy)
    EVT_MENU(wxID_PASTE, MyFrame::OnPaste)
#endif // wxUSE_CLIPBOARD
END_EVENT_TABLE()

MyFrame::MyFrame()
    : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxImage sample"),
                wxPoint(20, 20), wxSize(950, 700) )
{
    wxMenuBar *menu_bar = new wxMenuBar();

    wxMenu *menuImage = new wxMenu;
    menuImage->Append( ID_NEW, _T("&Show any image...\tCtrl-O"));
    menuImage->Append( ID_INFO, _T("Show image &information...\tCtrl-I"));
#ifdef wxHAVE_RAW_BITMAP
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWRAW, _T("Test &raw bitmap...\tCtrl-R"));
#endif
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWTHUMBNAIL, _T("Test &thumbnail...\tCtrl-T"),
                        "Test scaling the image during load (try with JPEG)");
    menuImage->AppendSeparator();
    menuImage->Append( ID_ABOUT, _T("&About..."));
    menuImage->AppendSeparator();
    menuImage->Append( ID_QUIT, _T("E&xit\tCtrl-Q"));
    menu_bar->Append(menuImage, _T("&Image"));

#if wxUSE_CLIPBOARD
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(wxID_COPY, _T("&Copy test image\tCtrl-C"));
    menuClipboard->Append(wxID_PASTE, _T("&Paste image\tCtrl-V"));
    menu_bar->Append(menuClipboard, _T("&Clipboard"));
#endif // wxUSE_CLIPBOARD

    SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR

    m_canvas = new MyCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(10,10) );

    // 500 width * 2750 height
    m_canvas->SetScrollbars( 10, 10, 50, 275 );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( true );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    (void)wxMessageBox( _T("wxImage demo\n")
                        _T("Robert Roebling (c) 1998,2000"),
                        _T("About wxImage Demo"), wxICON_INFORMATION | wxOK );
}

wxString MyFrame::LoadUserImage(wxImage& image)
{
    wxString filename;

#if wxUSE_FILEDLG
    filename = wxFileSelector(_T("Select image file"));
    if ( !filename.empty() )
    {
        if ( !image.LoadFile(filename) )
        {
            wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());

            return wxEmptyString;
        }
    }
#endif // wxUSE_FILEDLG

    return filename;
}

void MyFrame::OnNewFrame( wxCommandEvent &WXUNUSED(event) )
{
    wxImage image;
    wxString filename = LoadUserImage(image);
    if ( !filename.empty() )
        (new MyImageFrame(this, filename, wxBitmap(image)))->Show();
}

void MyFrame::OnImageInfo( wxCommandEvent &WXUNUSED(event) )
{
    wxImage image;
    if ( !LoadUserImage(image).empty() )
    {
        // TODO: show more information about the file
        wxString info = wxString::Format("Image size: %dx%d",
                                        image.GetWidth(),
                                        image.GetHeight());

        int xres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONX),
            yres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONY);
        if ( xres || yres )
        {
            info += wxString::Format("\nResolution: %dx%d", xres, yres);
            switch ( image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONUNIT) )
            {
                default:
                    wxFAIL_MSG( "unknown image resolution units" );
                    // fall through

                case wxIMAGE_RESOLUTION_NONE:
                    info += " in default units";
                    break;

                case wxIMAGE_RESOLUTION_INCHES:
                    info += " in";
                    break;

                case wxIMAGE_RESOLUTION_CM:
                    info += " cm";
                    break;
            }
        }

        wxLogMessage("%s", info);
    }
}

#ifdef wxHAVE_RAW_BITMAP

void MyFrame::OnTestRawBitmap( wxCommandEvent &WXUNUSED(event) )
{
    (new MyRawBitmapFrame(this))->Show();
}

#endif // wxHAVE_RAW_BITMAP

#if wxUSE_CLIPBOARD

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    wxBitmapDataObject *dobjBmp = new wxBitmapDataObject;
    dobjBmp->SetBitmap(m_canvas->my_horse_png);

    wxTheClipboard->Open();

    if ( !wxTheClipboard->SetData(dobjBmp) )
    {
        wxLogError(_T("Failed to copy bitmap to clipboard"));
    }

    wxTheClipboard->Close();
}

void MyFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    wxBitmapDataObject dobjBmp;

    wxTheClipboard->Open();
    if ( !wxTheClipboard->GetData(dobjBmp) )
    {
        wxLogMessage(_T("No bitmap data in the clipboard"));
    }
    else
    {
        (new MyImageFrame(this, _T("Clipboard"), dobjBmp.GetBitmap()))->Show();
    }
    wxTheClipboard->Close();
}

#endif // wxUSE_CLIPBOARD

void MyFrame::OnThumbnail( wxCommandEvent &WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    wxString filename = wxFileSelector(_T("Select image file"));
    if ( filename.empty() )
        return;

    static const int THUMBNAIL_WIDTH = 320;
    static const int THUMBNAIL_HEIGHT = 240;

    wxImage image;
    image.SetOption(wxIMAGE_OPTION_MAX_WIDTH, THUMBNAIL_WIDTH);
    image.SetOption(wxIMAGE_OPTION_MAX_HEIGHT, THUMBNAIL_HEIGHT);

    wxStopWatch sw;
    if ( !image.LoadFile(filename) )
    {
        wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());
        return;
    }

    const long loadTime = sw.Time();

    MyImageFrame * const
        frame = new MyImageFrame(this, filename, wxBitmap(image));
    frame->Show();
    wxLogStatus(frame, "Loaded \"%s\" in %ldms", filename, loadTime);
#else
    wxLogError( _T("Couldn't create file selector dialog") );
    return;
#endif // wxUSE_FILEDLG
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();

    wxFrame *frame = new MyFrame();
    frame->Show( true );

    return true;
}
