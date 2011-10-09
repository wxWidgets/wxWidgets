///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/image.cpp
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2005 Robert Roebling
//              (c) 2005-2009 Vadim Zeitlin
// Licence:     wxWindows licence
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
#include "wx/graphics.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"
#include "wx/quantize.h"
#include "wx/scopedptr.h"
#include "wx/stopwatch.h"
#include "wx/versioninfo.h"

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

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

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
#if wxUSE_GRAPHICS_CONTEXT
    void OnTestGraphics(wxCommandEvent& event);
#endif // wxUSE_GRAPHICS_CONTEXT
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
    MyImageFrame(wxFrame *parent, const wxString& desc, const wxImage& image)
    {
        Create(parent, desc, wxBitmap(image), image.GetImageCount(desc));
    }

    MyImageFrame(wxFrame *parent, const wxString& desc, const wxBitmap& bitmap)
    {
        Create(parent, desc, bitmap);
    }

private:
    bool Create(wxFrame *parent,
                const wxString& desc,
                const wxBitmap& bitmap,
                int numImages = 1)
    {
        if ( !wxFrame::Create(parent, wxID_ANY,
                              wxString::Format(wxT("Image from %s"), desc),
                              wxDefaultPosition, wxDefaultSize,
                              wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE) )
            return false;

        m_bitmap = bitmap;
        m_zoom = 1.;

        wxMenu *menu = new wxMenu;
        menu->Append(wxID_SAVE);
        menu->AppendSeparator();
        menu->AppendCheckItem(ID_PAINT_BG, wxT("&Paint background"),
                              "Uncheck this for transparent images");
        menu->AppendSeparator();
        menu->Append(ID_RESIZE, wxT("&Fit to window\tCtrl-F"));
        menu->Append(wxID_ZOOM_IN, "Zoom &in\tCtrl-+");
        menu->Append(wxID_ZOOM_OUT, "Zoom &out\tCtrl--");
        menu->Append(wxID_ZOOM_100, "Reset zoom to &100%\tCtrl-1");
        menu->AppendSeparator();
        menu->Append(ID_ROTATE_LEFT, wxT("Rotate &left\tCtrl-L"));
        menu->Append(ID_ROTATE_RIGHT, wxT("Rotate &right\tCtrl-R"));

        wxMenuBar *mbar = new wxMenuBar;
        mbar->Append(menu, wxT("&Image"));
        SetMenuBar(mbar);

        mbar->Check(ID_PAINT_BG, true);

        CreateStatusBar(2);
        if ( numImages != 1 )
            SetStatusText(wxString::Format("%d images", numImages), 1);

        SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());

        UpdateStatusBar();

        Show();

        return true;
    }

    void OnEraseBackground(wxEraseEvent& WXUNUSED(event))
    {
        // do nothing here to be able to see how transparent images are shown
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        if ( GetMenuBar()->IsChecked(ID_PAINT_BG) )
            dc.Clear();

        dc.SetUserScale(m_zoom, m_zoom);

        const wxSize size = GetClientSize();
        dc.DrawBitmap
           (
                m_bitmap,
                dc.DeviceToLogicalX((size.x - m_zoom*m_bitmap.GetWidth())/2),
                dc.DeviceToLogicalY((size.y - m_zoom*m_bitmap.GetHeight())/2),
                true /* use mask */
           );
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
#if wxUSE_LIBPNG
                                                wxT("PNG files (*.png)|*.png|")
#endif
#if wxUSE_LIBJPEG
                                                wxT("JPEG files (*.jpg)|*.jpg|")
#endif
#if wxUSE_GIF
                                                wxT("GIF files (*.gif)|*.gif|")
#endif
#if wxUSE_LIBTIFF
                                                wxT("TIFF files (*.tif)|*.tif|")
#endif
#if wxUSE_PCX
                                                wxT("PCX files (*.pcx)|*.pcx|")
#endif
                                                wxT("ICO files (*.ico)|*.ico|")
                                                wxT("CUR files (*.cur)|*.cur"),
                                                wxFD_SAVE,
                                                this);

        if ( savefilename.empty() )
            return;

        wxString extension;
        wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

        bool saved = false;
        if ( extension == wxT("bmp") )
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
                wxT("1 bpp color"),
                wxT("1 bpp B&W"),
                wxT("4 bpp color"),
                wxT("8 bpp color"),
                wxT("8 bpp greyscale"),
                wxT("8 bpp red"),
                wxT("8 bpp own palette"),
                wxT("24 bpp")
            };

            int bppselection = wxGetSingleChoiceIndex(wxT("Set BMP BPP"),
                                                    wxT("Image sample: save file"),
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
#if wxUSE_LIBPNG
        else if ( extension == wxT("png") )
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
                wxT("Colour 8bpp"),
                wxT("Colour 16bpp"),
                wxT("Grey 8bpp"),
                wxT("Grey 16bpp"),
                wxT("Grey red 8bpp"),
                wxT("Grey red 16bpp"),
            };

            int sel = wxGetSingleChoiceIndex(wxT("Set PNG format"),
                                            wxT("Image sample: save file"),
                                            WXSIZEOF(pngchoices),
                                            pngchoices,
                                            this);
            if ( sel != -1 )
            {
                image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, pngvalues[sel]);
                image.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, sel % 2 ? 16 : 8);

                // these values are taken from OptiPNG with -o3 switch
                const wxString compressionChoices[] =
                {
                    wxT("compression = 9, memory = 8, strategy = 0, filter = 0"),
                    wxT("compression = 9, memory = 9, strategy = 0, filter = 0"),
                    wxT("compression = 9, memory = 8, strategy = 1, filter = 0"),
                    wxT("compression = 9, memory = 9, strategy = 1, filter = 0"),
                    wxT("compression = 1, memory = 8, strategy = 2, filter = 0"),
                    wxT("compression = 1, memory = 9, strategy = 2, filter = 0"),
                    wxT("compression = 9, memory = 8, strategy = 0, filter = 5"),
                    wxT("compression = 9, memory = 9, strategy = 0, filter = 5"),
                    wxT("compression = 9, memory = 8, strategy = 1, filter = 5"),
                    wxT("compression = 9, memory = 9, strategy = 1, filter = 5"),
                    wxT("compression = 1, memory = 8, strategy = 2, filter = 5"),
                    wxT("compression = 1, memory = 9, strategy = 2, filter = 5"),
                };

                int sel = wxGetSingleChoiceIndex(wxT("Select compression option (Cancel to use default)\n"),
                                                 wxT("PNG Compression Options"),
                                                 WXSIZEOF(compressionChoices),
                                                 compressionChoices,
                                                 this);
                if (sel != -1)
                {
                    const int zc[] = {9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 1, 1};
                    const int zm[] = {8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9};
                    const int zs[] = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2};
                    const int f[]  = {0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                                      0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8};

                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL      , zc[sel]);
                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL  , zm[sel]);
                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY   , zs[sel]);
                    image.SetOption(wxIMAGE_OPTION_PNG_FILTER                 , f[sel]);
                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE, 1048576); // 1 MB
                }
            }
        }
#endif // wxUSE_LIBPNG
        else if ( extension == wxT("cur") )
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
    }

    void OnZoom(wxCommandEvent& event)
    {
        if ( event.GetId() == wxID_ZOOM_IN )
            m_zoom *= 1.2;
        else if ( event.GetId() == wxID_ZOOM_OUT )
            m_zoom /= 1.2;
        else // wxID_ZOOM_100
            m_zoom = 1.;

        UpdateStatusBar();
    }

    void OnRotate(wxCommandEvent& event)
    {
        double angle = 5;
        if ( event.GetId() == ID_ROTATE_LEFT )
            angle = -angle;

        wxImage img(m_bitmap.ConvertToImage());
        img = img.Rotate(angle, wxPoint(img.GetWidth() / 2, img.GetHeight() / 2));
        if ( !img.IsOk() )
        {
            wxLogWarning(wxT("Rotation failed"));
            return;
        }

        m_bitmap = wxBitmap(img);

        UpdateStatusBar();
    }

    void UpdateStatusBar()
    {
        wxLogStatus(this, wxT("Image size: (%d, %d), zoom %.2f"),
                    m_bitmap.GetWidth(),
                    m_bitmap.GetHeight(),
                    m_zoom);
        Refresh();
    }

    wxBitmap m_bitmap;
    double m_zoom;

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
        : wxFrame(parent, wxID_ANY, wxT("Raw bitmaps (how exciting)")),
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
                wxLogError(wxT("Failed to gain raw access to bitmap data"));
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
            wxLogError(wxT("Failed to gain raw access to bitmap data"));
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
            wxLogError(wxT("Failed to gain raw access to bitmap data"));
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
        dc.DrawText(wxT("This is alpha and raw bitmap test"), 0, BORDER);
        dc.DrawText(wxT("This is alpha and raw bitmap test"), 0, SIZE/2 - BORDER);
        dc.DrawText(wxT("This is alpha and raw bitmap test"), 0, SIZE - 2*BORDER);
        dc.DrawBitmap( m_alphaBitmap, 0, 0, true /* use mask */ );

        dc.DrawText(wxT("Raw bitmap access without alpha"), 0, SIZE+5);
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

    EVT_MENU(wxID_ZOOM_IN, MyImageFrame::OnZoom)
    EVT_MENU(wxID_ZOOM_OUT, MyImageFrame::OnZoom)
    EVT_MENU(wxID_ZOOM_100, MyImageFrame::OnZoom)
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
    ID_GRAPHICS,
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
#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU    (ID_GRAPHICS, MyFrame::OnTestGraphics)
#endif // wxUSE_GRAPHICS_CONTEXT
#if wxUSE_CLIPBOARD
    EVT_MENU(wxID_COPY, MyFrame::OnCopy)
    EVT_MENU(wxID_PASTE, MyFrame::OnPaste)
#endif // wxUSE_CLIPBOARD
END_EVENT_TABLE()

MyFrame::MyFrame()
    : wxFrame( (wxFrame *)NULL, wxID_ANY, wxT("wxImage sample"),
                wxPoint(20, 20), wxSize(950, 700) )
{
    SetIcon(wxICON(sample));

    wxMenuBar *menu_bar = new wxMenuBar();

    wxMenu *menuImage = new wxMenu;
    menuImage->Append( ID_NEW, wxT("&Show any image...\tCtrl-O"));
    menuImage->Append( ID_INFO, wxT("Show image &information...\tCtrl-I"));
#ifdef wxHAVE_RAW_BITMAP
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWRAW, wxT("Test &raw bitmap...\tCtrl-R"));
#endif
#if wxUSE_GRAPHICS_CONTEXT
    menuImage->AppendSeparator();
    menuImage->Append(ID_GRAPHICS, "Test &graphics context...\tCtrl-G");
#endif // wxUSE_GRAPHICS_CONTEXT
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWTHUMBNAIL, wxT("Test &thumbnail...\tCtrl-T"),
                        "Test scaling the image during load (try with JPEG)");
    menuImage->AppendSeparator();
    menuImage->Append( ID_ABOUT, wxT("&About...\tF1"));
    menuImage->AppendSeparator();
    menuImage->Append( ID_QUIT, wxT("E&xit\tCtrl-Q"));
    menu_bar->Append(menuImage, wxT("&Image"));

#if wxUSE_CLIPBOARD
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(wxID_COPY, wxT("&Copy test image\tCtrl-C"));
    menuClipboard->Append(wxID_PASTE, wxT("&Paste image\tCtrl-V"));
    menu_bar->Append(menuClipboard, wxT("&Clipboard"));
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
    m_canvas->SetCursor(wxImage("cursor.png"));
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( true );
}

#if wxUSE_ZLIB && wxUSE_STREAMS
#include "wx/zstream.h"
#endif

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    wxArrayString array;

    array.Add("wxImage demo");
    array.Add("(c) Robert Roebling 1998-2005");
    array.Add("(c) Vadim Zeitlin 2005-2009");

    array.Add(wxEmptyString);
    array.Add("Version of the libraries used:");

#if wxUSE_LIBPNG
    array.Add(wxPNGHandler::GetLibraryVersionInfo().ToString());
#endif
#if wxUSE_LIBJPEG
    array.Add(wxJPEGHandler::GetLibraryVersionInfo().ToString());
#endif
#if wxUSE_LIBTIFF
    array.Add(wxTIFFHandler::GetLibraryVersionInfo().ToString());
#endif
#if wxUSE_ZLIB && wxUSE_STREAMS
    // zlib is used by libpng
    array.Add(wxGetZlibVersionInfo().ToString());
#endif
    (void)wxMessageBox( wxJoin(array, '\n'),
                        "About wxImage Demo",
                        wxICON_INFORMATION | wxOK );
}

wxString MyFrame::LoadUserImage(wxImage& image)
{
    wxString filename;

#if wxUSE_FILEDLG
    filename = wxLoadFileSelector(wxT("image"), wxEmptyString);
    if ( !filename.empty() )
    {
        if ( !image.LoadFile(filename) )
        {
            wxLogError(wxT("Couldn't load image from '%s'."), filename.c_str());

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
        new MyImageFrame(this, filename, image);
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

#if wxUSE_GRAPHICS_CONTEXT

class MyGraphicsFrame : public wxFrame
{
public:
    enum
    {
        WIDTH = 256,
        HEIGHT = 90
    };

    MyGraphicsFrame(wxWindow* parent) :
        wxFrame(parent, wxID_ANY, "Graphics context test"),
        m_image(WIDTH, HEIGHT, false)
    {
        // Create a test image: it has 3 horizontal primary colour bands with
        // alpha increasing from left to right.
        m_image.SetAlpha();
        unsigned char* alpha = m_image.GetAlpha();
        unsigned char* data = m_image.GetData();

        for ( int y = 0; y < HEIGHT; y++ )
        {
            unsigned char r = 0,
                          g = 0,
                          b = 0;
            if ( y < HEIGHT/3 )
                r = 0xff;
            else if ( y < (2*HEIGHT)/3 )
                g = 0xff;
            else
                b = 0xff;

            for ( int x = 0; x < WIDTH; x++ )
            {
                *alpha++ = x;
                *data++ = r;
                *data++ = g;
                *data++ = b;
            }
        }

        m_bitmap = wxBitmap(m_image);

        Connect(wxEVT_PAINT, wxPaintEventHandler(MyGraphicsFrame::OnPaint));

        Show();
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);
        wxScopedPtr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
        wxGraphicsBitmap gb(gc->CreateBitmapFromImage(m_image));

        gc->SetFont(*wxNORMAL_FONT, *wxBLACK);
        gc->DrawText("Bitmap", 0, HEIGHT/2);
        gc->DrawBitmap(m_bitmap, 0, 0, WIDTH, HEIGHT);

        wxGraphicsFont gf = gc->CreateFont(wxNORMAL_FONT->GetPixelSize().y, "");
        gc->SetFont(gf);
        gc->DrawText("Graphics bitmap", 0, (3*HEIGHT)/2);
        gc->DrawBitmap(gb, 0, HEIGHT, WIDTH, HEIGHT);
    }

    wxImage m_image;
    wxBitmap m_bitmap;

    wxDECLARE_NO_COPY_CLASS(MyGraphicsFrame);
};

void MyFrame::OnTestGraphics(wxCommandEvent& WXUNUSED(event))
{
    new MyGraphicsFrame(this);
}

#endif // wxUSE_GRAPHICS_CONTEXT

#if wxUSE_CLIPBOARD

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    wxBitmapDataObject *dobjBmp = new wxBitmapDataObject;
    dobjBmp->SetBitmap(m_canvas->my_horse_png);

    wxTheClipboard->Open();

    if ( !wxTheClipboard->SetData(dobjBmp) )
    {
        wxLogError(wxT("Failed to copy bitmap to clipboard"));
    }

    wxTheClipboard->Close();
}

void MyFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    wxBitmapDataObject dobjBmp;

    wxTheClipboard->Open();
    if ( !wxTheClipboard->GetData(dobjBmp) )
    {
        wxLogMessage(wxT("No bitmap data in the clipboard"));
    }
    else
    {
        new MyImageFrame(this, wxT("Clipboard"), dobjBmp.GetBitmap());
    }
    wxTheClipboard->Close();
}

#endif // wxUSE_CLIPBOARD

void MyFrame::OnThumbnail( wxCommandEvent &WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    wxString filename = wxLoadFileSelector(wxT("image"), wxEmptyString, wxEmptyString, this);
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
        wxLogError(wxT("Couldn't load image from '%s'."), filename.c_str());
        return;
    }

    const long loadTime = sw.Time();

    MyImageFrame * const frame = new MyImageFrame(this, filename, image);
    wxLogStatus(frame, "Loaded \"%s\" in %ldms", filename, loadTime);
#else
    wxLogError( wxT("Couldn't create file selector dialog") );
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
