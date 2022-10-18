///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/image.cpp
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// Copyright:   (c) 1998-2005 Robert Roebling
//              (c) 2005-2009 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


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
#include "wx/artprov.h"
#include "wx/spinctrl.h"

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

#ifndef wxHAS_IMAGES_IN_RESOURCES
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
    virtual bool OnInit() override;
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
    void OnNewFrameHiDPI(wxCommandEvent&);
#ifdef wxHAS_SVG
    void OnNewSVGFrame(wxCommandEvent&);
#endif // wxHAS_SVG
    void OnImageInfo( wxCommandEvent &event );
    void OnThumbnail( wxCommandEvent &event );
    void OnFilters(wxCommandEvent& event);
    void OnUpdateNewFrameHiDPI(wxUpdateUIEvent&);

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
    void OnCopyImage(wxCommandEvent& evt);
    void OnPasteImage(wxCommandEvent& evt);
#endif // wxUSE_CLIPBOARD

    MyCanvas         *m_canvas;

private:
    // ask user for the file name and try to load an image from it
    //
    // return the file path on success, empty string if we failed to load the
    // image or were cancelled by user
    static wxString LoadUserImage(wxImage& image);

private:
    friend class MyFiltersFrame;

private:
    wxDECLARE_DYNAMIC_CLASS(MyFrame);
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// Frame used for showing a standalone image
// ----------------------------------------------------------------------------

enum
{
    ID_ROTATE_LEFT = wxID_HIGHEST+1,
    ID_ROTATE_RIGHT,
    ID_RESIZE,
    ID_ZOOM_x2,
    ID_ZOOM_DC,
    ID_ZOOM_NEAREST,
    ID_ZOOM_BILINEAR,
    ID_ZOOM_BICUBIC,
    ID_ZOOM_BOX_AVERAGE,
    ID_PAINT_BG
};

class MyImageFrame : public wxFrame
{
public:
    MyImageFrame(wxFrame *parent, const wxString& desc, const wxImage& image, double scale = 1.0)
    {
        // Retrieve image info
        wxString info;
        int xres, yres;
        switch ( GetResolutionFromOptions(image, &xres, &yres) )
        {
            case wxIMAGE_RESOLUTION_NONE:
                break;

            case wxIMAGE_RESOLUTION_CM:
                // convert to DPI
                xres = wxRound(xres / 10.0 * inches2mm);
                yres = wxRound(yres / 10.0 * inches2mm);
                wxFALLTHROUGH;

            case wxIMAGE_RESOLUTION_INCHES:
                info = wxString::Format("DPI %i x %i", xres, yres);
                break;

            default:
                wxFAIL_MSG("unexpected image resolution units");
                break;
        }

        int numImages = desc.StartsWith("Clipboard") ? 1 : image.GetImageCount(desc);
        if ( numImages > 1 )
        {
            if ( !info.empty() )
                info += ", ";

            info += wxString::Format("%d images", numImages);
        }

        Create(parent, desc, wxBitmap(image, wxBITMAP_SCREEN_DEPTH, scale), info);
    }

    MyImageFrame(wxFrame *parent, const wxString& desc, const wxBitmap& bitmap)
    {
        Create(parent, desc, bitmap);
    }

private:
    bool Create(wxFrame *parent,
                const wxString& desc,
                const wxBitmap& bitmap,
                wxString info = wxString())
    {
        if ( !wxFrame::Create(parent, wxID_ANY,
                              wxString::Format("Image from %s", desc),
                              wxDefaultPosition, wxDefaultSize,
                              wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE) )
            return false;

        m_bitmap = bitmap;
        m_zoom = 1.;
        m_useImageForZoom = false;

        wxMenu *menu = new wxMenu;
        menu->Append(wxID_SAVEAS);
        menu->AppendSeparator();
        menu->AppendCheckItem(ID_PAINT_BG, "&Paint background",
                              "Uncheck this for transparent images");
        menu->AppendSeparator();
        menu->Append(ID_RESIZE, "&Fit to window\tCtrl-F");
        menu->AppendSeparator();
        menu->Append(wxID_ZOOM_IN, "Zoom &in\tCtrl-+");
        menu->Append(wxID_ZOOM_OUT, "Zoom &out\tCtrl--");
        menu->Append(wxID_ZOOM_100, "Reset zoom to &100%\tCtrl-1");
        menu->Append(ID_ZOOM_x2, "Double zoom level\tCtrl-2");
        menu->AppendSeparator();
        menu->AppendRadioItem(ID_ZOOM_DC, "Use wx&DC for zoomin\tShift-Ctrl-D");
        menu->AppendRadioItem(ID_ZOOM_NEAREST, "Use rescale nearest\tShift-Ctrl-N");
        menu->AppendRadioItem(ID_ZOOM_BILINEAR, "Use rescale bilinear\tShift-Ctrl-L");
        menu->AppendRadioItem(ID_ZOOM_BICUBIC, "Use rescale bicubic\tShift-Ctrl-C");
        menu->AppendRadioItem(ID_ZOOM_BOX_AVERAGE, "Use rescale box average\tShift-Ctrl-B");
        menu->AppendSeparator();
        menu->Append(ID_ROTATE_LEFT, "Rotate &left\tCtrl-L");
        menu->Append(ID_ROTATE_RIGHT, "Rotate &right\tCtrl-R");

        wxMenuBar *mbar = new wxMenuBar;
        mbar->Append(menu, "&Image");
        SetMenuBar(mbar);

        mbar->Check(ID_PAINT_BG, true);

        CreateStatusBar(2);
        SetStatusText(info, 1);

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

        const int width = int(m_zoom * m_bitmap.GetWidth());
        const int height = int(m_zoom * m_bitmap.GetHeight());

        wxBitmap bitmap;
        if ( m_useImageForZoom )
        {
            bitmap = m_bitmap.ConvertToImage().Scale(width, height,
                                                     m_resizeQuality);
        }
        else
        {
            dc.SetUserScale(m_zoom, m_zoom);
            bitmap = m_bitmap;
        }

        const wxSize size = GetClientSize();
        dc.DrawBitmap
           (
                bitmap,
                dc.DeviceToLogicalX((size.x - width) / 2),
                dc.DeviceToLogicalY((size.y - height) / 2),
                true /* use mask */
           );
    }

    void OnSave(wxCommandEvent& WXUNUSED(event))
    {
#if wxUSE_FILEDLG
        wxImage image = m_bitmap.ConvertToImage();

        wxString savefilename = wxFileSelector( "Save Image",
                                                wxEmptyString,
                                                wxEmptyString,
                                                wxEmptyString,
                                                "BMP files (*.bmp)|*.bmp|"
#if wxUSE_LIBPNG
                                                "PNG files (*.png)|*.png|"
#endif
#if wxUSE_LIBJPEG
                                                "JPEG files (*.jpg)|*.jpg|"
#endif
#if wxUSE_GIF
                                                "GIF files (*.gif)|*.gif|"
#endif
#if wxUSE_LIBTIFF
                                                "TIFF files (*.tif)|*.tif|"
#endif
#if wxUSE_PCX
                                                "PCX files (*.pcx)|*.pcx|"
#endif
#if wxUSE_XPM
                                                "X PixMap files (*.xpm)|*.xpm|"
#endif
                                                "ICO files (*.ico)|*.ico|"
                                                "CUR files (*.cur)|*.cur",
                                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                                                this);

        if ( savefilename.empty() )
            return;

        wxString extension;
        wxFileName::SplitPath(savefilename, nullptr, nullptr, &extension);

        bool saved = false;
        if ( extension == "bmp" )
        {
            static const int bppvalues[] =
            {
                wxBMP_1BPP,
                wxBMP_1BPP_BW,
                wxBMP_4BPP,
                wxBMP_8BPP,
                wxBMP_8BPP_GREY,
                wxBMP_8BPP_RED,
#if wxUSE_PALETTE
                wxBMP_8BPP_PALETTE,
#endif // wxUSE_PALETTE
                wxBMP_24BPP
            };

            const wxString bppchoices[] =
            {
                "1 bpp color",
                "1 bpp B&W",
                "4 bpp color",
                "8 bpp color",
                "8 bpp greyscale",
                "8 bpp red",
#if wxUSE_PALETTE
                "8 bpp own palette",
#endif // wxUSE_PALETTE
                "24 bpp"
            };

            int bppselection = wxGetSingleChoiceIndex("Set BMP BPP",
                                                    "Image sample: save file",
                                                    WXSIZEOF(bppchoices),
                                                    bppchoices,
                                                    this);
            if ( bppselection != -1 )
            {
                int format = bppvalues[bppselection];
                image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, format);
#if wxUSE_PALETTE
                if ( format == wxBMP_8BPP_PALETTE )
                {
                    unsigned char *cmap = new unsigned char [256];
                    for ( int i = 0; i < 256; i++ )
                        cmap[i] = (unsigned char)i;
                    image.SetPalette(wxPalette(256, cmap, cmap, cmap));

                    delete[] cmap;
                }
#endif // wxUSE_PALETTE
            }
        }
#if wxUSE_LIBPNG
        else if ( extension == "png" )
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
                "Colour 8bpp",
                "Colour 16bpp",
                "Grey 8bpp",
                "Grey 16bpp",
                "Grey red 8bpp",
                "Grey red 16bpp",
            };

            int sel = wxGetSingleChoiceIndex("Set PNG format",
                                            "Image sample: save file",
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
                    "compression = 9, memory = 8, strategy = 0, filter = 0",
                    "compression = 9, memory = 9, strategy = 0, filter = 0",
                    "compression = 9, memory = 8, strategy = 1, filter = 0",
                    "compression = 9, memory = 9, strategy = 1, filter = 0",
                    "compression = 1, memory = 8, strategy = 2, filter = 0",
                    "compression = 1, memory = 9, strategy = 2, filter = 0",
                    "compression = 9, memory = 8, strategy = 0, filter = 5",
                    "compression = 9, memory = 9, strategy = 0, filter = 5",
                    "compression = 9, memory = 8, strategy = 1, filter = 5",
                    "compression = 9, memory = 9, strategy = 1, filter = 5",
                    "compression = 1, memory = 8, strategy = 2, filter = 5",
                    "compression = 1, memory = 9, strategy = 2, filter = 5",
                };

                sel = wxGetSingleChoiceIndex("Select compression option (Cancel to use default)\n",
                                             "PNG Compression Options",
                                             WXSIZEOF(compressionChoices),
                                             compressionChoices,
                                             this);
                if ( sel != -1 )
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
        else if ( extension == "cur" )
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
        switch ( event.GetId() )
        {
            case wxID_ZOOM_IN:
                m_zoom *= 1.2;
                break;

            case wxID_ZOOM_OUT:
                m_zoom /= 1.2;
                break;

            case wxID_ZOOM_100:
                m_zoom = 1.;
                break;

            case ID_ZOOM_x2:
                m_zoom *= 2.;
                break;

            default:
                wxFAIL_MSG("unknown zoom command");
                return;
        }

        UpdateStatusBar();
    }

    void OnUseZoom(wxCommandEvent& event)
    {
        bool useImageForZoom = true;

        switch ( event.GetId() )
        {
            case ID_ZOOM_DC:
                useImageForZoom = false;
                break;

            case ID_ZOOM_NEAREST:
                m_resizeQuality = wxIMAGE_QUALITY_NEAREST;
                break;

            case ID_ZOOM_BILINEAR:
                m_resizeQuality = wxIMAGE_QUALITY_BILINEAR;
                break;

            case ID_ZOOM_BICUBIC:
                m_resizeQuality = wxIMAGE_QUALITY_BICUBIC;
                break;

            case ID_ZOOM_BOX_AVERAGE:
                m_resizeQuality = wxIMAGE_QUALITY_BOX_AVERAGE;
                break;

            default:
                wxFAIL_MSG("unknown use for zoom command");
                return;
        }

        m_useImageForZoom = useImageForZoom;

        Refresh();
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
            wxLogWarning("Rotation failed");
            return;
        }

        m_bitmap = wxBitmap(img);

        UpdateStatusBar();
    }

    void UpdateStatusBar()
    {
        wxLogStatus(this, "Image size: (%d, %d), zoom %.2f",
                    m_bitmap.GetWidth(),
                    m_bitmap.GetHeight(),
                    m_zoom);
        Refresh();
    }

    // This is a copy of protected wxImageHandler::GetResolutionFromOptions()
    static wxImageResolution GetResolutionFromOptions(const wxImage& image, int* x, int* y)
    {
        wxCHECK_MSG(x && y, wxIMAGE_RESOLUTION_NONE, wxT("null pointer"));

        if ( image.HasOption(wxIMAGE_OPTION_RESOLUTIONX) &&
            image.HasOption(wxIMAGE_OPTION_RESOLUTIONY) )
        {
            *x = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONX);
            *y = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONY);
        }
        else if ( image.HasOption(wxIMAGE_OPTION_RESOLUTION) )
        {
            *x =
            *y = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTION);
        }
        else // no resolution options specified
        {
            *x =
            *y = 0;

            return wxIMAGE_RESOLUTION_NONE;
        }

        // get the resolution unit too
        int resUnit = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONUNIT);
        if ( !resUnit )
        {
            // this is the default
            resUnit = wxIMAGE_RESOLUTION_INCHES;
        }

        return (wxImageResolution)resUnit;
    }

    wxBitmap m_bitmap;
    double m_zoom;

    // If false, then wxDC is used for zooming. If true, then m_resizeQuality
    // is used with wxImage::Scale() for zooming.
    bool m_useImageForZoom;
    wxImageResizeQuality m_resizeQuality;

    wxDECLARE_EVENT_TABLE();
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
        : wxFrame(parent, wxID_ANY, "Raw bitmaps (how exciting)"),
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
                wxLogError("Failed to gain raw access to bitmap data");
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
            wxLogError("Failed to gain raw access to bitmap data");
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
            wxLogError("Failed to gain raw access to bitmap data");
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
        dc.DrawText("This is alpha and raw bitmap test", 0, BORDER);
        dc.DrawText("This is alpha and raw bitmap test", 0, SIZE/2 - BORDER);
        dc.DrawText("This is alpha and raw bitmap test", 0, SIZE - 2*BORDER);
        dc.DrawBitmap( m_alphaBitmap, 0, 0, true /* use mask */ );

        dc.DrawText("Raw bitmap access without alpha", 0, SIZE+5);
        dc.DrawBitmap( m_bitmap, 0, SIZE+5+dc.GetCharHeight());
    }

private:
    wxBitmap m_bitmap;
    wxBitmap m_alphaBitmap;

    wxDECLARE_EVENT_TABLE();
};

#endif // wxHAVE_RAW_BITMAP

class MyFiltersFrame : public wxFrame
{
public:
    MyFiltersFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "Image filters test")
    {
        wxMenu *menuImage = new wxMenu;

        menuImage->Append(wxID_OPEN, "&Open...\tCtrl-O", "Load a user defined image");
        menuImage->Append(wxID_RESET, "&Reset\tCtrl-R", "Reset all the image filters");
        menuImage->Append(wxID_CLOSE, "&Close\tCtrl-Q", "Close this frame");

        wxMenuBar *menuBar = new wxMenuBar();
        menuBar->Append(menuImage, "&Image");
        SetMenuBar(menuBar);

        wxSizerFlags sizerFlags1(1);
        sizerFlags1.Border().Expand();

        wxSizerFlags sizerFlags2;
        sizerFlags2.Border().Expand();

        wxSizerFlags sizerFlags3;
        sizerFlags3.Border().Center();

        wxSizerFlags sizerFlags4;
        sizerFlags4.Border();

        wxStaticBoxSizer *sizerHue = new wxStaticBoxSizer(new wxStaticBox(this,
                                     wxID_ANY, wxS("Hue (°)")), wxVERTICAL);
        m_sliderHue = new wxSlider(sizerHue->GetStaticBox(), wxID_ANY, 0, -360, 360,
                      wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
        sizerHue->Add(m_sliderHue, sizerFlags2);

        wxStaticBoxSizer *sizerSaturation = new wxStaticBoxSizer(new wxStaticBox(this,
                                            wxID_ANY, wxS("Saturation (%)")), wxVERTICAL);
        m_sliderSaturation = new wxSlider(sizerSaturation->GetStaticBox(), wxID_ANY,
                             0, -100, 100, wxDefaultPosition, wxDefaultSize,
                             wxSL_HORIZONTAL | wxSL_LABELS);
        sizerSaturation->Add(m_sliderSaturation, sizerFlags2);

        wxStaticBoxSizer *sizerBrightness = new wxStaticBoxSizer(new wxStaticBox(this,
                                            wxID_ANY, wxS("Brightness (value) (%)")),
                                            wxVERTICAL);
        m_sliderBrightness = new wxSlider(sizerBrightness->GetStaticBox(), wxID_ANY,
                             0, -100, 100, wxDefaultPosition, wxDefaultSize,
                             wxSL_HORIZONTAL | wxSL_LABELS);
        sizerBrightness->Add(m_sliderBrightness, sizerFlags2);

        wxStaticBoxSizer *sizerLightness = new wxStaticBoxSizer(new wxStaticBox(this,
                                           wxID_ANY, wxS("Lightness")),
                                           wxVERTICAL);
        m_sliderLightness = new wxSlider(sizerLightness->GetStaticBox(), wxID_ANY,
                            100, 0, 200, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS);
        sizerLightness->Add(m_sliderLightness, sizerFlags2);

        wxStaticBoxSizer *sizerDisabled = new wxStaticBoxSizer(new wxStaticBox(this,
                                          wxID_ANY, wxS("Disabled")), wxVERTICAL);
        m_checkDisabled = new wxCheckBox(sizerDisabled->GetStaticBox(), wxID_ANY,
                          wxS("Convert to disabled"));
        m_sliderDisabled = new wxSlider(sizerDisabled->GetStaticBox(), wxID_ANY,
                           255, 0, 255, wxDefaultPosition, wxDefaultSize,
                           wxSL_HORIZONTAL | wxSL_LABELS);
        sizerDisabled->Add(m_checkDisabled, sizerFlags4);
        sizerDisabled->Add(m_sliderDisabled, sizerFlags2);

        wxStaticBoxSizer *sizerGrey = new wxStaticBoxSizer(new wxStaticBox(this,
                                      wxID_ANY, wxS("Greyscale")), wxVERTICAL);
        m_checkGrey = new wxCheckBox(sizerGrey->GetStaticBox(), wxID_ANY,
                      wxS("Convert to greyscale"));
        wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);
        m_spinGreyWeightR = new wxSpinCtrlDouble(sizerGrey->GetStaticBox(),
                            wxID_ANY, wxEmptyString, wxDefaultPosition,
                            wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.299, 0.001);
        m_spinGreyWeightG = new wxSpinCtrlDouble(sizerGrey->GetStaticBox(),
                            wxID_ANY, wxEmptyString, wxDefaultPosition,
                            wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.587, 0.001);
        m_spinGreyWeightB = new wxSpinCtrlDouble(sizerGrey->GetStaticBox(),
                            wxID_ANY, wxEmptyString, wxDefaultPosition,
                            wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.114, 0.001);
        sizer1->AddStretchSpacer();
        sizer1->Add(new wxStaticText(sizerGrey->GetStaticBox(), wxID_ANY,
                                     wxS("Red weight:")), sizerFlags3);
        sizer1->Add(m_spinGreyWeightR, sizerFlags3);
        sizer1->Add(new wxStaticText(sizerGrey->GetStaticBox(), wxID_ANY,
                                     wxS("Green weight:")), sizerFlags3);
        sizer1->Add(m_spinGreyWeightG, sizerFlags3);
        sizer1->Add(new wxStaticText(sizerGrey->GetStaticBox(), wxID_ANY,
                                     wxS("Blue weight:")), sizerFlags3);
        sizer1->Add(m_spinGreyWeightB, sizerFlags3);
        sizer1->AddStretchSpacer();
        sizerGrey->Add(m_checkGrey, sizerFlags4);
        sizerGrey->Add(sizer1, sizerFlags2);

        wxStaticBoxSizer *sizerMono = new wxStaticBoxSizer(new wxStaticBox(this,
                                      wxID_ANY, wxS("Monochrome")), wxVERTICAL);
        m_checkMono = new wxCheckBox(sizerMono->GetStaticBox(), wxID_ANY,
                      wxS("Convert to monochrome"));
        wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
        m_spinMonoR = new wxSpinCtrl(sizerMono->GetStaticBox(), wxID_ANY,
                      wxEmptyString, wxDefaultPosition, wxDefaultSize,
                      wxSP_ARROW_KEYS, 0, 255, 0);
        m_spinMonoG = new wxSpinCtrl(sizerMono->GetStaticBox(), wxID_ANY,
                      wxEmptyString, wxDefaultPosition, wxDefaultSize,
                      wxSP_ARROW_KEYS, 0, 255, 0);
        m_spinMonoB = new wxSpinCtrl(sizerMono->GetStaticBox(), wxID_ANY,
                      wxEmptyString, wxDefaultPosition, wxDefaultSize,
                      wxSP_ARROW_KEYS, 0, 255, 0);
        sizer2->AddStretchSpacer();
        sizer2->Add(new wxStaticText(sizerMono->GetStaticBox(), wxID_ANY,
                                     wxS("Red:")), sizerFlags3);
        sizer2->Add(m_spinMonoR, sizerFlags3);
        sizer2->Add(new wxStaticText(sizerMono->GetStaticBox(), wxID_ANY,
                                     wxS("Green:")), sizerFlags3);
        sizer2->Add(m_spinMonoG, sizerFlags3);
        sizer2->Add(new wxStaticText(sizerMono->GetStaticBox(), wxID_ANY,
                                     wxS("Blue:")), sizerFlags3);
        sizer2->Add(m_spinMonoB, sizerFlags3);
        sizer2->AddStretchSpacer();
        sizerMono->Add(m_checkMono, sizerFlags4);
        sizerMono->Add(sizer2, sizerFlags2);

        wxBoxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
        sizerLeft->Add(sizerHue, sizerFlags2);
        sizerLeft->Add(sizerSaturation, sizerFlags2);
        sizerLeft->Add(sizerBrightness, sizerFlags2);
        sizerLeft->Add(sizerLightness, sizerFlags2);

        wxBoxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
        sizerRight->Add(sizerDisabled, sizerFlags2);
        sizerRight->Add(sizerGrey, sizerFlags2);
        sizerRight->Add(sizerMono, sizerFlags2);

        wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_BUTTON,
                          wxSize(256, 256));
        m_image = bitmap.ConvertToImage();

        m_stcBitmap = new wxStaticBitmap(this, wxID_ANY, bitmap);
        wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
        sizerTop->AddStretchSpacer();
        sizerTop->Add(m_stcBitmap, sizerFlags1);
        sizerTop->AddStretchSpacer();

        sizerFlags1.Border(0);

        wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);
        sizerBottom->Add(sizerLeft, sizerFlags1);
        sizerBottom->Add(sizerRight, sizerFlags1);

        wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
        sizerMain->Add(sizerTop, sizerFlags1);
        sizerMain->Add(sizerBottom, sizerFlags2);

        SetSizer(sizerMain);
        CreateStatusBar();

        // Bind Events
        Bind(wxEVT_MENU, &MyFiltersFrame::OnNewImage, this, wxID_OPEN);
        Bind(wxEVT_MENU, &MyFiltersFrame::OnReset, this, wxID_RESET);
        Bind(wxEVT_MENU, &MyFiltersFrame::OnClose, this, wxID_CLOSE);
        m_sliderHue->Bind(wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this);
        m_sliderSaturation->Bind(wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this);
        m_sliderBrightness->Bind(wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this);
        m_sliderLightness->Bind(wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this);
        m_checkDisabled->Bind(wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this);
        m_sliderDisabled->Bind(wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this);
        m_checkGrey->Bind(wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this);
        m_spinGreyWeightR->Bind(wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter, this);
        m_spinGreyWeightG->Bind(wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter, this);
        m_spinGreyWeightB->Bind(wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter, this);
        m_checkMono->Bind(wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this);
        m_spinMonoR->Bind(wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this);
        m_spinMonoG->Bind(wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this);
        m_spinMonoB->Bind(wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this);
    }

private:
    wxStaticBitmap *m_stcBitmap;
    wxSlider *m_sliderHue;
    wxSlider *m_sliderSaturation;
    wxSlider *m_sliderBrightness;
    wxSlider *m_sliderLightness;
    wxCheckBox *m_checkDisabled;
    wxSlider *m_sliderDisabled;
    wxCheckBox *m_checkGrey;
    wxSpinCtrlDouble *m_spinGreyWeightR;
    wxSpinCtrlDouble *m_spinGreyWeightG;
    wxSpinCtrlDouble *m_spinGreyWeightB;
    wxCheckBox *m_checkMono;
    wxSpinCtrl *m_spinMonoR;
    wxSpinCtrl *m_spinMonoG;
    wxSpinCtrl *m_spinMonoB;
    wxImage m_image;

protected:
    void OnNewImage(wxCommandEvent& WXUNUSED(event))
    {
        wxImage image;
        wxString filename = static_cast<MyFrame *>(GetParent())->LoadUserImage(image);

        if ( !filename.empty() )
        {
            m_image = image;
            DoFilter();
        }
    }

    void OnReset(wxCommandEvent& WXUNUSED(event))
    {
        m_stcBitmap->SetBitmap(m_image);
        m_sliderHue->SetValue(0);
        m_sliderSaturation->SetValue(0);
        m_sliderBrightness->SetValue(0);
        m_sliderLightness->SetValue(100);
        m_checkDisabled->SetValue(false);
        m_sliderDisabled->SetValue(255);
        m_checkGrey->SetValue(false);
        m_spinGreyWeightR->SetValue(0.299);
        m_spinGreyWeightG->SetValue(0.587);
        m_spinGreyWeightB->SetValue(0.114);
        m_checkMono->SetValue(false);
        m_spinMonoR->SetValue(0);
        m_spinMonoG->SetValue(0);
        m_spinMonoB->SetValue(0);
    }

    void OnClose(wxCommandEvent& WXUNUSED(event))
    {
        Close(true);
    }

    virtual void OnFilter(wxEvent& WXUNUSED(event))
    {
        DoFilter();
    }

    void DoFilter()
    {
        wxImage image = m_image;
        image.RotateHue(m_sliderHue->GetValue() / 360.);
        image.ChangeSaturation(m_sliderSaturation->GetValue() / 100.);
        image.ChangeBrightness(m_sliderBrightness->GetValue() / 100.);
        image = image.ChangeLightness(m_sliderLightness->GetValue());

        if ( m_checkDisabled->IsChecked() )
            image = image.ConvertToDisabled(m_sliderDisabled->GetValue());

        if ( m_checkGrey->IsChecked() )
        {
            image = image.ConvertToGreyscale(m_spinGreyWeightR->GetValue(),
                                             m_spinGreyWeightG->GetValue(),
                                             m_spinGreyWeightB->GetValue());
        }

        if ( m_checkMono->IsChecked() )
        {
            image = image.ConvertToMono(m_spinMonoR->GetValue(),
                                        m_spinMonoG->GetValue(),
                                        m_spinMonoB->GetValue());
        }

        m_stcBitmap->SetBitmap(image);
        Layout();
    }
};

// ============================================================================
// implementations
// ============================================================================

//-----------------------------------------------------------------------------
// MyImageFrame
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyImageFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyImageFrame::OnEraseBackground)
    EVT_PAINT(MyImageFrame::OnPaint)

    EVT_MENU(wxID_SAVEAS, MyImageFrame::OnSave)
    EVT_MENU_RANGE(ID_ROTATE_LEFT, ID_ROTATE_RIGHT, MyImageFrame::OnRotate)
    EVT_MENU(ID_RESIZE, MyImageFrame::OnResize)

    EVT_MENU(wxID_ZOOM_IN, MyImageFrame::OnZoom)
    EVT_MENU(wxID_ZOOM_OUT, MyImageFrame::OnZoom)
    EVT_MENU(wxID_ZOOM_100, MyImageFrame::OnZoom)
    EVT_MENU(ID_ZOOM_x2, MyImageFrame::OnZoom)

    EVT_MENU(ID_ZOOM_DC, MyImageFrame::OnUseZoom)
    EVT_MENU(ID_ZOOM_NEAREST, MyImageFrame::OnUseZoom)
    EVT_MENU(ID_ZOOM_BILINEAR, MyImageFrame::OnUseZoom)
    EVT_MENU(ID_ZOOM_BICUBIC, MyImageFrame::OnUseZoom)
    EVT_MENU(ID_ZOOM_BOX_AVERAGE, MyImageFrame::OnUseZoom)
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// MyRawBitmapFrame
//-----------------------------------------------------------------------------

#ifdef wxHAVE_RAW_BITMAP

wxBEGIN_EVENT_TABLE(MyRawBitmapFrame, wxFrame)
    EVT_PAINT(MyRawBitmapFrame::OnPaint)
wxEND_EVENT_TABLE()

#endif // wxHAVE_RAW_BITMAP

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

enum
{
    ID_QUIT  = wxID_EXIT,
    ID_ABOUT = wxID_ABOUT,
    ID_NEW = 100,
    ID_NEW_HIDPI,
    ID_NEW_SVG,
    ID_INFO,
    ID_SHOWRAW,
    ID_GRAPHICS,
    ID_SHOWTHUMBNAIL,
    ID_COPY_IMAGE,
    ID_PASTE_IMAGE,
    ID_FILTERS
};

wxIMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame );
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
    EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
    EVT_MENU    (ID_NEW,   MyFrame::OnNewFrame)
    EVT_MENU    (ID_NEW_HIDPI,   MyFrame::OnNewFrameHiDPI)
#ifdef wxHAS_SVG
    EVT_MENU    (ID_NEW_SVG, MyFrame::OnNewSVGFrame)
#endif // wxHAS_SVG
    EVT_MENU    (ID_INFO,  MyFrame::OnImageInfo)
    EVT_MENU    (ID_SHOWTHUMBNAIL, MyFrame::OnThumbnail)
    EVT_MENU    (ID_FILTERS, MyFrame::OnFilters)
#ifdef wxHAVE_RAW_BITMAP
    EVT_MENU    (ID_SHOWRAW, MyFrame::OnTestRawBitmap)
#endif
#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU    (ID_GRAPHICS, MyFrame::OnTestGraphics)
#endif // wxUSE_GRAPHICS_CONTEXT
#if wxUSE_CLIPBOARD
    EVT_MENU(wxID_COPY, MyFrame::OnCopy)
    EVT_MENU(wxID_PASTE, MyFrame::OnPaste)
    EVT_MENU(ID_COPY_IMAGE, MyFrame::OnCopyImage)
    EVT_MENU(ID_PASTE_IMAGE, MyFrame::OnPasteImage)
#endif // wxUSE_CLIPBOARD
    EVT_UPDATE_UI(ID_NEW_HIDPI, MyFrame::OnUpdateNewFrameHiDPI)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
    : wxFrame( nullptr, wxID_ANY, "wxImage sample",
                wxPoint(20, 20), wxSize(950, 700) )
{
    SetIcon(wxICON(sample));

    wxMenuBar *menu_bar = new wxMenuBar();

    wxMenu *menuImage = new wxMenu;
    menuImage->Append( ID_NEW, "&Show any image...\tCtrl-O");
    menuImage->Append(ID_NEW_HIDPI, "Show any image as &HiDPI...\tCtrl-H");
#ifdef wxHAS_SVG
    menuImage->Append( ID_NEW_SVG, "Show &SVG image...\tCtrl-S");
#endif // wxHAS_SVG
    menuImage->Append( ID_INFO, "Show image &information...\tCtrl-I");
#ifdef wxHAVE_RAW_BITMAP
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWRAW, "Test &raw bitmap...\tCtrl-R");
#endif
#if wxUSE_GRAPHICS_CONTEXT
    menuImage->AppendSeparator();
    menuImage->Append(ID_GRAPHICS, "Test &graphics context...\tCtrl-G");
#endif // wxUSE_GRAPHICS_CONTEXT
    menuImage->AppendSeparator();
    menuImage->Append( ID_SHOWTHUMBNAIL, "Test &thumbnail...\tCtrl-T",
                        "Test scaling the image during load (try with JPEG)");
    menuImage->AppendSeparator();
    menuImage->Append(ID_FILTERS, "Test image &filters...\tCtrl-F",
                      "Test applying different image filters");
    menuImage->AppendSeparator();
    menuImage->Append( ID_ABOUT, "&About\tF1");
    menuImage->AppendSeparator();
    menuImage->Append( ID_QUIT, "E&xit\tCtrl-Q");
    menu_bar->Append(menuImage, "&Image");

#if wxUSE_CLIPBOARD
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(wxID_COPY, "&Copy test image as wxBitmap\tCtrl-C");
    menuClipboard->Append(wxID_PASTE, "&Paste image as wxBitmap\tCtrl-V");
    menuClipboard->AppendSeparator();
    menuClipboard->Append(ID_COPY_IMAGE, "Copy image as wxImage");
    menuClipboard->Append(ID_PASTE_IMAGE, "Paste image as wxImage");
    menu_bar->Append(menuClipboard, "&Clipboard");
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
    filename = wxLoadFileSelector("image", wxEmptyString);
    if ( !filename.empty() )
    {
        if ( !image.LoadFile(filename) )
        {
            wxLogError("Couldn't load image from '%s'.", filename);

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

void MyFrame::OnNewFrameHiDPI(wxCommandEvent&)
{
    wxImage image;
    wxString filename = LoadUserImage(image);
    if (!filename.empty())
        new MyImageFrame(this, filename, image, GetContentScaleFactor());
}

#ifdef wxHAS_SVG

class MySVGFrame : public wxFrame
{
public:
    explicit MySVGFrame(wxFrame* parent,
                        const wxString& filename,
                        const wxBitmapBundle& bundle)
        : wxFrame(parent, wxID_ANY, wxString::Format("SVG image %s", filename),
                  wxDefaultPosition, wxDefaultSize,
                  wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE),
          m_bundle(bundle)
    {
        Bind(wxEVT_PAINT, &MySVGFrame::OnPaint, this);

        SetClientSize(bundle.GetDefaultSize());

        Show();
    }

private:
    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        // Check if the bitmap needs to be re-rendered at the new size. Note
        // that the bitmap size is in physical pixels, which can be different
        // from the logical pixels in which the window size is expressed.
        const wxSize sizeWin = GetClientSize();
        const wxSize sizeBmp = sizeWin*GetContentScaleFactor();
        if ( !m_bitmap.IsOk() || m_bitmap.GetSize() != sizeBmp )
        {
            m_bitmap = m_bundle.GetBitmap(sizeBmp);
        }

        // Use wxGraphicsContext if available for alpha support.
#if wxUSE_GRAPHICS_CONTEXT
        wxScopedPtr<wxGraphicsContext> const
            gc(wxGraphicsRenderer::GetDefaultRenderer()->CreateContext(dc));

        gc->DrawBitmap(m_bitmap, 0, 0, sizeWin.x, sizeWin.y);
#else
        dc.DrawBitmap(m_bitmap, wxPoint(0, 0), true);
#endif
    }

    const wxBitmapBundle m_bundle;
    wxBitmap m_bitmap;

    wxDECLARE_NO_COPY_CLASS(MySVGFrame);
};

void MyFrame::OnNewSVGFrame(wxCommandEvent&)
{
    const wxString
        filename = wxLoadFileSelector("SVG document", ".svg", "image", this);
    if ( filename.empty() )
        return;

    // The default size here is completely arbitrary, as we don't know anything
    // about the SVG being loaded.
    wxBitmapBundle bb = wxBitmapBundle::FromSVGFile(filename, wxSize(200, 200));
    if ( !bb.IsOk() )
        return;

    new MySVGFrame(this, filename, bb);
}

#endif // wxHAS_SVG

void MyFrame::OnUpdateNewFrameHiDPI(wxUpdateUIEvent& event)
{
    event.Enable(GetContentScaleFactor() > 1);
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
                    wxFALLTHROUGH;

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

        Bind(wxEVT_PAINT, &MyGraphicsFrame::OnPaint, this);

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
        wxLogError("Failed to copy bitmap to clipboard");
    }

    wxTheClipboard->Close();
}

void MyFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    wxBitmapDataObject dobjBmp;

    wxTheClipboard->Open();
    if ( !wxTheClipboard->GetData(dobjBmp) )
    {
        wxLogMessage("No bitmap data in the clipboard");
    }
    else
    {
        new MyImageFrame(this, "Clipboard", dobjBmp.GetBitmap());
    }
    wxTheClipboard->Close();
}

void MyFrame::OnCopyImage(wxCommandEvent& WXUNUSED(evt))
{
    wxImage img;
    wxString filename = LoadUserImage(img);
    if ( filename.empty() )
        return;

    wxImageDataObject* dobjImage = new wxImageDataObject;
    dobjImage->SetImage(img);

    wxClipboardLocker clipOpener;
    if ( !wxTheClipboard->SetData(dobjImage) )
    {
        wxLogError("Failed to copy wxImage to clipboard");
    }
}

void MyFrame::OnPasteImage(wxCommandEvent& WXUNUSED(evt))
{
    wxImageDataObject dobjImage;

    wxClipboardLocker clipOpener;
    if ( !wxTheClipboard->GetData(dobjImage) )
    {
        wxLogMessage("No wxImage data in the clipboard");
    }
    else
    {
        new MyImageFrame(this, "Clipboard (wxImage)", dobjImage.GetImage());
    }
}

#endif // wxUSE_CLIPBOARD

void MyFrame::OnThumbnail( wxCommandEvent &WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    wxString filename = wxLoadFileSelector("image", wxEmptyString, wxEmptyString, this);
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
        wxLogError("Couldn't load image from '%s'.", filename);
        return;
    }

    int origWidth = image.GetOptionInt( wxIMAGE_OPTION_ORIGINAL_WIDTH );
    int origHeight = image.GetOptionInt( wxIMAGE_OPTION_ORIGINAL_HEIGHT );

    const long loadTime = sw.Time();

    MyImageFrame * const frame = new MyImageFrame(this, filename, image);
    wxLogStatus(frame, "Loaded \"%s\" in %ldms; original size was (%d, %d)",
                filename, loadTime, origWidth, origHeight);
#else
    wxLogError( "Couldn't create file selector dialog" );
    return;
#endif // wxUSE_FILEDLG
}

void MyFrame::OnFilters(wxCommandEvent& WXUNUSED(event))
{
    (new MyFiltersFrame(this))->Show();
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();

    wxFrame *frame = new MyFrame();
    frame->Show( true );

    return true;
}
