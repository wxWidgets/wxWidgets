///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/canvas.cpp
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// Copyright:   (c) 1998-2005 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/mstream.h"

#include "canvas.h"

#include "smile.xbm"
#include "smile.xpm"
#include "cursor_png.c"


//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_DPI_CHANGED(MyCanvas::OnDPIChanged)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
    , m_bmpSmileXpm(smile_xpm)
    , m_iconSmileXpm(smile_xpm)
{
    my_horse_ani = nullptr;
    m_ani_images = 0;

    SetBackgroundColour(*wxWHITE);

    wxBitmap bitmap(100, 100);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    DrawSquares(dc, 0, 0);
    dc.SelectObject(wxNullBitmap);

    // try to find the directory with our images
    wxString dir;
    if ( wxFile::Exists("./horse.png") )
        dir = "./";
    else if ( wxFile::Exists("../horse.png") )
        dir = "../";
    else
        wxLogWarning("Can't find image files in either '.' or '..'!");

    wxImage image = bitmap.ConvertToImage();

#if wxUSE_LIBPNG
    if ( !image.SaveFile( dir + "test.png", wxBITMAP_TYPE_PNG ))
    {
        wxLogError("Can't save file");
    }

    image.Destroy();

    if ( image.LoadFile( dir + "test.png" ) )
        my_square = wxBitmap( image );

    image.Destroy();

    if ( !image.LoadFile( dir + "horse.png") )
    {
        wxLogError("Can't load PNG image");
    }
    else
    {
        my_horse_png = wxBitmap( image );
    }

    if ( !image.LoadFile( dir + "toucan.png") )
    {
        wxLogError("Can't load PNG image");
    }
    else
    {
        my_toucan = wxBitmap(image);
    }

    my_toucan_flipped_horiz = wxBitmap(image.Mirror(true));
    my_toucan_flipped_vert = wxBitmap(image.Mirror(false));
    my_toucan_flipped_both = wxBitmap(image.Mirror(true).Mirror(false));
    my_toucan_grey = wxBitmap(image.ConvertToGreyscale());
    my_toucan_head = wxBitmap(image.GetSubImage(wxRect(40, 7, 80, 60)));
    my_toucan_scaled_normal = wxBitmap(image.Scale(110,90,wxIMAGE_QUALITY_NORMAL));
    my_toucan_scaled_high = wxBitmap(image.Scale(110,90,wxIMAGE_QUALITY_HIGH));
    my_toucan_blur = wxBitmap(image.Blur(10));

#endif // wxUSE_LIBPNG

#if wxUSE_LIBJPEG
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.jpg") )
    {
        wxLogError("Can't load JPG image");
    }
    else
    {
        my_horse_jpeg = wxBitmap( image );

        // Colorize by rotating green hue to red
        wxImage::HSVValue greenHSV = wxImage::RGBtoHSV(wxImage::RGBValue(0, 255, 0));
        wxImage::HSVValue redHSV = wxImage::RGBtoHSV(wxImage::RGBValue(255, 0, 0));
        image.RotateHue(redHSV.hue - greenHSV.hue);
        colorized_horse_jpeg = wxBitmap( image );
    }

    if ( !image.LoadFile( dir + "cmyk.jpg") )
    {
        wxLogError("Can't load CMYK JPG image");
    }
    else
    {
        my_cmyk_jpeg = wxBitmap(image);
    }
#endif // wxUSE_LIBJPEG

#if wxUSE_GIF
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.gif" ) )
    {
        wxLogError("Can't load GIF image");
    }
    else
    {
        my_horse_gif = wxBitmap( image );
    }
#endif

#if wxUSE_PCX
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.pcx", wxBITMAP_TYPE_PCX ) )
    {
        wxLogError("Can't load PCX image");
    }
    else
    {
        my_horse_pcx = wxBitmap( image );
    }
#endif

    image.Destroy();

    if ( !image.LoadFile( dir + "horse.bmp", wxBITMAP_TYPE_BMP ) )
    {
        wxLogError("Can't load BMP image");
    }
    else
    {
        my_horse_bmp = wxBitmap( image );
    }

#if wxUSE_XPM
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.xpm", wxBITMAP_TYPE_XPM ) )
    {
        wxLogError("Can't load XPM image");
    }
    else
    {
        my_horse_xpm = wxBitmap( image );
    }

    if ( !image.SaveFile( dir + "test.xpm", wxBITMAP_TYPE_XPM ))
    {
        wxLogError("Can't save file");
    }
#endif

#if wxUSE_PNM
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.pnm", wxBITMAP_TYPE_PNM ) )
    {
        wxLogError("Can't load PNM image");
    }
    else
    {
        my_horse_pnm = wxBitmap( image );
    }

    image.Destroy();

    if ( !image.LoadFile( dir + "horse_ag.pnm", wxBITMAP_TYPE_PNM ) )
    {
        wxLogError("Can't load PNM image");
    }
    else
    {
        my_horse_asciigrey_pnm = wxBitmap( image );
    }

    image.Destroy();

    if ( !image.LoadFile( dir + "horse_rg.pnm", wxBITMAP_TYPE_PNM ) )
    {
        wxLogError("Can't load PNM image");
    }
    else
    {
        my_horse_rawgrey_pnm = wxBitmap( image );
    }
#endif

#if wxUSE_LIBTIFF
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.tif", wxBITMAP_TYPE_TIFF ) )
    {
        wxLogError("Can't load TIFF image");
    }
    else
    {
        my_horse_tiff = wxBitmap( image );
    }
#endif

#if wxUSE_TGA
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.tga", wxBITMAP_TYPE_TGA ) )
    {
        wxLogError("Can't load TGA image");
    }
    else
    {
        my_horse_tga = wxBitmap( image );
    }
#endif

#if wxUSE_LIBWEBP
    image.Destroy();

    if (!image.LoadFile(dir + "horse.webp", wxBITMAP_TYPE_WEBP))
    {
        wxLogError("Can't load WebP image");
    }
    else
    {
        my_horse_webp = wxBitmap(image);
    }
#endif

    CreateAntiAliasedBitmap();

    my_smile_xbm = wxBitmap( (const char*)smile_bits, smile_width,
                                smile_height, 1 );

    // demonstrates XPM automatically using the mask when saving
    if ( m_bmpSmileXpm.IsOk() )
        m_bmpSmileXpm.SaveFile("saved.xpm", wxBITMAP_TYPE_XPM);

#if wxUSE_ICO_CUR
    image.Destroy();

    if ( !image.LoadFile( dir + "horse.ico", wxBITMAP_TYPE_ICO, 0 ) )
    {
        wxLogError("Can't load first ICO image");
    }
    else
    {
        my_horse_ico32 = wxBitmap( image );
    }

    image.Destroy();

    if ( !image.LoadFile( dir + "horse.ico", wxBITMAP_TYPE_ICO, 1 ) )
    {
        wxLogError("Can't load second ICO image");
    }
    else
    {
        my_horse_ico16 = wxBitmap( image );
    }

    image.Destroy();

    if ( !image.LoadFile( dir + "horse.ico" ) )
    {
        wxLogError("Can't load best ICO image");
    }
    else
    {
        my_horse_ico = wxBitmap( image );
    }

    image.Destroy();

    if ( !image.LoadFile( dir + "horse.cur", wxBITMAP_TYPE_CUR ) )
    {
        wxLogError("Can't load best ICO image");
    }
    else
    {
        my_horse_cur = wxBitmap( image );
        cur_hotspot = wxPoint(
            image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X),
            image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y)
        );
    }

    m_ani_images = wxImage::GetImageCount ( dir + "horse3.ani", wxBITMAP_TYPE_ANI );
    if (m_ani_images==0)
    {
        wxLogError("No ANI-format images found");
    }
    else
    {
        my_horse_ani = new wxBitmap [m_ani_images];
    }

    int i;
    for (i=0; i < m_ani_images; i++)
    {
        image.Destroy();
        if (!image.LoadFile( dir + "horse3.ani", wxBITMAP_TYPE_ANI, i ))
        {
            wxString tmp = "Can't load image number ";
            tmp << i ;
            wxLogError(tmp);
        }
        else
            my_horse_ani [i] = wxBitmap( image );
    }
#endif // wxUSE_ICO_CUR


    image.Destroy();

    // test image loading from stream
    wxFile file(dir + "horse.bmp");
    if ( file.IsOpened() )
    {
        wxFileOffset len = file.Length();
        size_t dataSize = (size_t)len;
        void *data = malloc(dataSize);
        if ( file.Read(data, dataSize) != len )
        {
            wxLogError("Reading bitmap file failed");
        }
        else
        {
            wxMemoryInputStream mis(data, dataSize);
            if ( !image.LoadFile(mis) )
            {
                wxLogError("Can't load BMP image from stream");
            }
            else
            {
                my_horse_bmp2 = wxBitmap( image );
            }
        }

        free(data);
    }

    // This macro loads PNG from either resources on the platforms that support
    // this (Windows and OS X) or from in-memory data (coming from cursor_png.c
    // included above in our case).
    my_png_from_res = wxBITMAP_PNG(cursor);

    // This one always loads PNG from memory but exists for consistency with
    // the above one and also because it frees you from the need to specify the
    // length explicitly, without it you'd have to do it and also spell the
    // array name in full, like this:
    //
    // my_png_from_mem = wxBitmap::NewFromPNGData(cursor_png, WXSIZEOF(cursor_png));
    my_png_from_mem = wxBITMAP_PNG_FROM_DATA(cursor);

    // prevent -Wunused-const-variable when compiler fails to detect its usage
    wxUnusedVar(cursor_png);
}

MyCanvas::~MyCanvas()
{
    delete [] my_horse_ani;
}

void MyCanvas::DrawSquares(wxDC& dc, int xOffset, int yOffset)
{
    const int outerSquare = 100;
    const int innerSquare = 60;
    const int overlap = (outerSquare - innerSquare) / 2;

    dc.SetBrush(wxBrush("orange"));
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(xOffset, yOffset, outerSquare, outerSquare);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(xOffset + overlap, yOffset + overlap, innerSquare, innerSquare);
}

wxBitmap const& MyCanvas::GetPngBitmap() const
{
    return my_horse_png;
}

void MyCanvas::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();
    my_horse_png_scaled = wxNullBitmap;
}

wxSize MyCanvas::GetDrawingSize() const
{
    // Aproximate the size used in OnPaint
    // 4 columns, about 14 rows and 1 DPI dependent row
    const int imageSize = 200;
    const int width = 4 * FromDIP(250);
    const int height = 14 * (imageSize + 2 * GetCharHeight()) + FromDIP(imageSize);

    return wxSize(width, height);
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    const int colWidth = dc.FromDIP(250);
    const int colOffset = dc.FromDIP(20);
    const int col1 = colOffset + 0 * colWidth;
    const int col2 = colOffset + 1 * colWidth;
    const int col3 = colOffset + 2 * colWidth;
    const int col4 = colOffset + 3 * colWidth;

    const int ch = dc.GetCharHeight();
    const int imageSize = 200; // horse images are 200x200 px
    const int yOffset = imageSize + 2 * ch;
    int y = ch;

    dc.SetTextForeground(*wxBLACK);
    dc.DrawText("Loaded image", col1, y);
    if (my_square.IsOk())
        dc.DrawBitmap(my_square, col1, y + ch);

    dc.DrawText("Drawn directly", col1 + colWidth / 2, y);
    DrawSquares(dc, col1 + colWidth / 2, y + ch);

    if (my_anti.IsOk())
        dc.DrawBitmap(my_anti, col2, y + ch);

    y += yOffset;

    dc.DrawText("PNG handler", col1, y);
    if (my_horse_png.IsOk())
    {
        dc.DrawBitmap(my_horse_png, col1, y + ch);
        wxRect rect(0, 0, 100, 100);
        wxBitmap sub(my_horse_png.GetSubBitmap(rect));
        dc.DrawText("GetSubBitmap()", col2, y);
        dc.DrawBitmap(sub, col2, y + ch);

        const double scale = GetDPIScaleFactor() / GetContentScaleFactor();
        if (!my_horse_png_scaled.IsOk())
        {
            my_horse_png_scaled = my_horse_png;
            wxBitmap::Rescale(my_horse_png_scaled, scale * my_horse_png.GetSize());
        }
        dc.DrawText(wxString::Format("DPI Rescale(%.1f)", scale), col3, y);
        dc.DrawBitmap(my_horse_png_scaled, col3, y + ch);
        // extra offset
        y += (scale * imageSize - imageSize);
    }

    y += yOffset;

    dc.DrawText("JPEG handler", col1, y);
    if (my_horse_jpeg.IsOk())
        dc.DrawBitmap(my_horse_jpeg, col1, y + ch);

    dc.DrawText("Green rotated to red", col2, y);
    if (colorized_horse_jpeg.IsOk())
        dc.DrawBitmap(colorized_horse_jpeg, col2, y + ch);

    dc.DrawText("CMYK JPEG image", col3, y);
    if (my_cmyk_jpeg.IsOk())
        dc.DrawBitmap(my_cmyk_jpeg, col3, y + ch);

    y += yOffset;

    dc.DrawText("GIF handler", col1, y);
    if (my_horse_gif.IsOk())
        dc.DrawBitmap(my_horse_gif, col1, y + ch);

    y += yOffset;

    dc.DrawText("PCX handler", col1, y);
    if (my_horse_pcx.IsOk())
        dc.DrawBitmap(my_horse_pcx, col1, y + ch);

    y += yOffset;

    dc.DrawText("BMP handler", col1, y);
    if (my_horse_bmp.IsOk())
        dc.DrawBitmap(my_horse_bmp, col1, y + ch);

    dc.DrawText("BMP read from memory", col2, y);
    if (my_horse_bmp2.IsOk())
        dc.DrawBitmap(my_horse_bmp2, col2, y + ch);

    y += yOffset;

    dc.DrawText("PNM handler", col1, y);
    if (my_horse_pnm.IsOk())
        dc.DrawBitmap(my_horse_pnm, col1, y + ch);

    dc.DrawText("PNM handler (ascii grey)", col2, y);
    if (my_horse_asciigrey_pnm.IsOk())
        dc.DrawBitmap(my_horse_asciigrey_pnm, col2, y + ch);

    dc.DrawText("PNM handler (raw grey)", col3, y);
    if (my_horse_rawgrey_pnm.IsOk())
        dc.DrawBitmap(my_horse_rawgrey_pnm, col3, y + ch);

    y += yOffset;

    dc.DrawText("TIFF handler", col1, y);
    if (my_horse_tiff.IsOk())
        dc.DrawBitmap(my_horse_tiff, col1, y + ch);

    y += yOffset;

    dc.DrawText("TGA handler", col1, y);
    if (my_horse_tga.IsOk())
        dc.DrawBitmap(my_horse_tga, col1, y + ch);

    y += yOffset;

    dc.DrawText("WebP handler", col1, y);
    if (my_horse_webp.IsOk())
        dc.DrawBitmap(my_horse_webp, col1, y + ch);

    y += yOffset;

    dc.DrawText("XPM handler", col1, y);
    if (my_horse_xpm.IsOk())
        dc.DrawBitmap(my_horse_xpm, col1, y + ch);

    y += yOffset;

    // toucans
    {
        const int y2Offset = imageSize;
        int y2 = ch;

        dc.DrawText("Original toucan", col4, y2);
        dc.DrawBitmap(my_toucan, col4, y2 + ch, true);
        y2 += y2Offset;
        dc.DrawText("Flipped horizontally", col4, y2);
        dc.DrawBitmap(my_toucan_flipped_horiz, col4, y2 + ch, true);
        y2 += y2Offset;
        dc.DrawText("Flipped vertically", col4, y2);
        dc.DrawBitmap(my_toucan_flipped_vert, col4, y2 + ch, true);
        y2 += y2Offset;
        dc.DrawText("Flipped both h&v", col4, y2);
        dc.DrawBitmap(my_toucan_flipped_both, col4, y2 + ch, true);

        y2 += y2Offset;
        dc.DrawText("In greyscale", col4, y2);
        dc.DrawBitmap(my_toucan_grey, col4, y2 + ch, true);

        y2 += y2Offset;
        dc.DrawText("Toucan's head", col4, y2);
        dc.DrawBitmap(my_toucan_head, col4, y2 + ch, true);

        y2 += y2Offset;
        dc.DrawText("Scaled with normal quality", col4, y2);
        dc.DrawBitmap(my_toucan_scaled_normal, col4, y2 + ch, true);

        y2 += y2Offset;
        dc.DrawText("Scaled with high quality", col4, y2);
        dc.DrawBitmap(my_toucan_scaled_high, col4, y2 + 15, true);

        y2 += y2Offset;
        dc.DrawText("Blured", col4, y2);
        dc.DrawBitmap(my_toucan_blur, col4, y2 + ch, true);
    }

    if (my_smile_xbm.IsOk())
    {
        dc.DrawText("XBM bitmap", col2, y);
        dc.DrawText("(green on red)", col2, y + ch);
        dc.SetTextForeground("GREEN");
        dc.SetTextBackground("RED");
        dc.DrawBitmap(my_smile_xbm, col2, y + ch * 5 / 2);

        dc.SetTextForeground(*wxBLACK);
        dc.DrawText("After wxImage conversion", col2 + colWidth / 2, y);
        dc.DrawText("(red on white)", col2 + colWidth / 2, y + ch);
        dc.SetTextForeground("RED");
        wxImage i = my_smile_xbm.ConvertToImage();
        i.SetMaskColour(255, 255, 255);
        i.Replace(0, 0, 0,
            wxRED_PEN->GetColour().Red(),
            wxRED_PEN->GetColour().Green(),
            wxRED_PEN->GetColour().Blue());
        dc.DrawBitmap(wxBitmap(i), col2 + colWidth / 2, y + ch * 5 / 2, true);
        dc.SetTextForeground(*wxBLACK);

        y += yOffset / 2;
    }

    {
        wxBitmap mono(60, 50, 1);
        wxMemoryDC memdc;
        memdc.SelectObject(mono);
        memdc.SetPen(*wxBLACK_PEN);
        memdc.SetBrush(*wxWHITE_BRUSH);
        memdc.DrawRectangle(0, 0, 60, 50);
        memdc.SetTextForeground(*wxBLACK);
#ifndef __WXGTK__
        // I cannot convince GTK2 to draw into mono bitmaps
        memdc.DrawText("Hi!", 5, 5);
#endif
        memdc.SetBrush(*wxBLACK_BRUSH);
        memdc.DrawRectangle(33, 5, 20, 20);
        memdc.SetPen(*wxRED_PEN);
        memdc.DrawLine(5, 42, 50, 42);
        memdc.SelectObject(wxNullBitmap);

        if (mono.IsOk())
        {
            dc.DrawText("Mono bitmap", col2, y);
            dc.DrawText("(red on green)", col2, y + ch);
            dc.SetTextForeground("RED");
            dc.SetTextBackground("GREEN");
            dc.DrawBitmap(mono, col2, y + ch * 5 / 2);

            dc.SetTextForeground(*wxBLACK);
            dc.DrawText("After wxImage conversion", col2 + colWidth / 2, y);
            dc.DrawText("(red on white)", col2 + colWidth / 2, y + ch);
            dc.SetTextForeground("RED");
            wxImage i = mono.ConvertToImage();
            i.SetMaskColour(255, 255, 255);
            i.Replace(0, 0, 0,
                wxRED_PEN->GetColour().Red(),
                wxRED_PEN->GetColour().Green(),
                wxRED_PEN->GetColour().Blue());
            dc.DrawBitmap(wxBitmap(i), col2 + colWidth / 2, y + ch * 5 / 2, true);
            dc.SetTextForeground(*wxBLACK);
        }

        y += yOffset * 2 / 3;
    }

    // For testing transparency
    int xmpCol = col1;
    int xmpSep = dc.FromDIP(80);
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawRectangle(xmpCol, y, col3 - xmpCol, (ch + imageSize) / 2);

    xmpCol += xmpSep / 4;
    dc.DrawText("XPM bitmap", xmpCol, y + ch / 2);
    if (m_bmpSmileXpm.IsOk())
        dc.DrawBitmap(m_bmpSmileXpm, xmpCol, y + 2 * ch, true);

    xmpCol += xmpSep;
    dc.DrawText("XPM icon", xmpCol, y + ch / 2);
    if (m_iconSmileXpm.IsOk())
        dc.DrawIcon(m_iconSmileXpm, xmpCol, y + 2 * ch);

    // testing icon -> bitmap conversion
    wxBitmap to_blit(m_iconSmileXpm);
    if (to_blit.IsOk())
    {
        xmpCol += xmpSep;
        dc.DrawText("SubBitmap", xmpCol, y + ch / 2);
        wxBitmap sub = to_blit.GetSubBitmap(wxRect(0, 0, 15, 15));
        if (sub.IsOk())
            dc.DrawBitmap(sub, xmpCol, y + 2 * ch, true);

        xmpCol += xmpSep;
        dc.DrawText("Enlarged", xmpCol, y + ch / 2);
        dc.SetUserScale(1.5, 1.5);
        dc.DrawBitmap(to_blit, xmpCol * 2 / 3, (y + 2 * ch) * 2 / 3, true);
        xmpCol += xmpSep / 2;
        dc.SetUserScale(2, 2);
        dc.DrawBitmap(to_blit, xmpCol / 2, (y + 2 * ch) / 2, true);
        dc.SetUserScale(1.0, 1.0);

        xmpCol += xmpSep * 3 / 4;
        dc.DrawText("Blit", xmpCol, y + ch / 2);
        wxMemoryDC blit_dc;
        blit_dc.SelectObject(to_blit);
        dc.Blit(xmpCol, y + 2 * ch, to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true);
        xmpCol += xmpSep * 2 / 5;
        dc.SetUserScale(1.5, 1.5);
        dc.Blit(xmpCol * 2 / 3, (y + 2 * ch) * 2 / 3, to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true);
        xmpCol += xmpSep / 2;
        dc.SetUserScale(2, 2);
        dc.Blit(xmpCol / 2, (y + 2 * ch) / 2, to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true);
        dc.SetUserScale(1.0, 1.0);
    }

    y += yOffset * 2 / 3;

    dc.DrawText("ICO handler (1st image)", col1, y);
    if (my_horse_ico32.IsOk())
        dc.DrawBitmap(my_horse_ico32, col1, y + ch, true);

    dc.DrawText("ICO handler (2nd image)", col2, y);
    if (my_horse_ico16.IsOk())
        dc.DrawBitmap(my_horse_ico16, col2, y + ch, true);

    dc.DrawText("ICO handler (best image)", col3, y);
    if (my_horse_ico.IsOk())
        dc.DrawBitmap(my_horse_ico, col3, y + ch, true);

    y += yOffset / 2;

    dc.DrawText("CUR handler", col1, y);
    if (my_horse_cur.IsOk())
    {
        dc.DrawBitmap(my_horse_cur, col1, y + ch, true);
        dc.SetPen(*wxRED_PEN);
        const int cr = dc.FromDIP(10);
        const int cx = col1 + cur_hotspot.x;
        const int cy = y + ch + cur_hotspot.y;
        dc.DrawLine(cx - cr, cy, cx + cr, cy);
        dc.DrawLine(cx, cy - cr, cx, cy + cr);
    }

    dc.DrawText("ANI handler", col2, y);
    for (int i = 0; i < m_ani_images; i++)
    {
        if (my_horse_ani[i].IsOk())
        {
            dc.DrawBitmap(my_horse_ani[i], col2 + i * 2 * my_horse_ani[i].GetWidth(), y + ch, true);
        }
    }

    y += yOffset / 2;

    dc.DrawText("PNG from resources", col1, y);
    if (my_png_from_res.IsOk())
        dc.DrawBitmap(my_png_from_res, col1, y + ch, true);
    dc.DrawText("PNG from memory", col2, y);
    if (my_png_from_mem.IsOk())
        dc.DrawBitmap(my_png_from_mem, col2, y + ch, true);

    y += yOffset / 2;

    if (GetDrawingSize().y < y) {
        wxLogWarning("GetDrawingSize() size too small: %d<%d", GetDrawingSize().y, y);
    }
}

void MyCanvas::CreateAntiAliasedBitmap()
{
    wxBitmap bitmap( 300, 300 );

    {
        wxMemoryDC dc(bitmap);

        dc.Clear();

        dc.SetFont( wxFontInfo(24).Family(wxFONTFAMILY_DECORATIVE) );
        dc.SetTextForeground( "RED" );
        dc.DrawText( "This is anti-aliased Text.", 20, 5 );
        dc.DrawText( "And a Rectangle.", 20, 45 );

        dc.SetBrush( *wxRED_BRUSH );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRoundedRectangle( 20, 85, 200, 180, 20 );
    }

    wxImage original = bitmap.ConvertToImage();
    wxImage anti( 150, 150 );

    /* This is quite slow, but safe. Use wxImage::GetData() for speed instead. */

    for (int y = 1; y < 149; y++)
        for (int x = 1; x < 149; x++)
        {
            int red = original.GetRed( x*2, y*2 ) +
                        original.GetRed( x*2-1, y*2 ) +
                        original.GetRed( x*2, y*2+1 ) +
                        original.GetRed( x*2+1, y*2+1 );
            red = red/4;

            int green = original.GetGreen( x*2, y*2 ) +
                        original.GetGreen( x*2-1, y*2 ) +
                        original.GetGreen( x*2, y*2+1 ) +
                        original.GetGreen( x*2+1, y*2+1 );
            green = green/4;

            int blue = original.GetBlue( x*2, y*2 ) +
                        original.GetBlue( x*2-1, y*2 ) +
                        original.GetBlue( x*2, y*2+1 ) +
                        original.GetBlue( x*2+1, y*2+1 );
            blue = blue/4;
            anti.SetRGB( x, y, (unsigned char)red, (unsigned char)green, (unsigned char)blue );
        }

    my_anti = wxBitmap(anti);
}
