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

#include "smile.xbm"
#include "smile.xpm"
#include "cursor_png.c"

#include "canvas.h"


//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
    : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER )
    , m_bmpSmileXpm(smile_xpm)
    , m_iconSmileXpm(smile_xpm)
{
    my_horse_ani = NULL;
    m_ani_images = 0 ;

    SetBackgroundColour(* wxWHITE);

    wxBitmap bitmap( 100, 100 );

    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetBrush( wxBrush( "orange" ) );
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawRectangle( 0, 0, 100, 100 );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 20, 20, 60, 60 );
    dc.SelectObject( wxNullBitmap );

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
        xH = 30 + image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X) ;
        yH = 2420 + image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y) ;
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
}

MyCanvas::~MyCanvas()
{
    delete [] my_horse_ani;
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.DrawText( "Loaded image", 30, 10 );
    if (my_square.IsOk())
        dc.DrawBitmap( my_square, 30, 30 );

    dc.DrawText( "Drawn directly", 150, 10 );
    dc.SetBrush( wxBrush( "orange" ) );
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawRectangle( 150, 30, 100, 100 );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 170, 50, 60, 60 );

    if (my_anti.IsOk())
        dc.DrawBitmap( my_anti, 280, 30 );

    dc.DrawText( "PNG handler", 30, 135 );
    if (my_horse_png.IsOk())
    {
        dc.DrawBitmap( my_horse_png, 30, 150 );
        wxRect rect(0,0,100,100);
        wxBitmap sub( my_horse_png.GetSubBitmap(rect) );
        dc.DrawText( "GetSubBitmap()", 280, 175 );
        dc.DrawBitmap( sub, 280, 195 );
    }

    dc.DrawText( "JPEG handler", 30, 365 );
    if (my_horse_jpeg.IsOk())
        dc.DrawBitmap( my_horse_jpeg, 30, 380 );

    dc.DrawText( "Green rotated to red", 280, 365 );
    if (colorized_horse_jpeg.IsOk())
        dc.DrawBitmap( colorized_horse_jpeg, 280, 380 );

    dc.DrawText( "CMYK JPEG image", 530, 365 );
    if (my_cmyk_jpeg.IsOk())
        dc.DrawBitmap( my_cmyk_jpeg, 530, 380 );

    dc.DrawText( "GIF handler", 30, 595 );
    if (my_horse_gif.IsOk())
        dc.DrawBitmap( my_horse_gif, 30, 610 );

    dc.DrawText( "PCX handler", 30, 825 );
    if (my_horse_pcx.IsOk())
        dc.DrawBitmap( my_horse_pcx, 30, 840 );

    dc.DrawText( "BMP handler", 30, 1055 );
    if (my_horse_bmp.IsOk())
        dc.DrawBitmap( my_horse_bmp, 30, 1070 );

    dc.DrawText( "BMP read from memory", 280, 1055 );
    if (my_horse_bmp2.IsOk())
        dc.DrawBitmap( my_horse_bmp2, 280, 1070 );

    dc.DrawText( "PNM handler", 30, 1285 );
    if (my_horse_pnm.IsOk())
        dc.DrawBitmap( my_horse_pnm, 30, 1300 );

    dc.DrawText( "PNM handler (ascii grey)", 280, 1285 );
    if (my_horse_asciigrey_pnm.IsOk())
        dc.DrawBitmap( my_horse_asciigrey_pnm, 280, 1300 );

    dc.DrawText( "PNM handler (raw grey)", 530, 1285 );
    if (my_horse_rawgrey_pnm.IsOk())
        dc.DrawBitmap( my_horse_rawgrey_pnm, 530, 1300 );

    dc.DrawText( "TIFF handler", 30, 1515 );
    if (my_horse_tiff.IsOk())
        dc.DrawBitmap( my_horse_tiff, 30, 1530 );

    dc.DrawText( "TGA handler", 30, 1745 );
    if (my_horse_tga.IsOk())
        dc.DrawBitmap( my_horse_tga, 30, 1760 );

    dc.DrawText( "XPM handler", 30, 1975 );
    if (my_horse_xpm.IsOk())
        dc.DrawBitmap( my_horse_xpm, 30, 2000 );

    // toucans
    {
        int x = 750, y = 10, yy = 170;

        dc.DrawText("Original toucan", x+50, y);
        dc.DrawBitmap(my_toucan, x, y+15, true);
        y += yy;
        dc.DrawText("Flipped horizontally", x+50, y);
        dc.DrawBitmap(my_toucan_flipped_horiz, x, y+15, true);
        y += yy;
        dc.DrawText("Flipped vertically", x+50, y);
        dc.DrawBitmap(my_toucan_flipped_vert, x, y+15, true);
        y += yy;
        dc.DrawText("Flipped both h&v", x+50, y);
        dc.DrawBitmap(my_toucan_flipped_both, x, y+15, true);

        y += yy;
        dc.DrawText("In greyscale", x+50, y);
        dc.DrawBitmap(my_toucan_grey, x, y+15, true);

        y += yy;
        dc.DrawText("Toucan's head", x+50, y);
        dc.DrawBitmap(my_toucan_head, x, y+15, true);

        y += yy;
        dc.DrawText("Scaled with normal quality", x+50, y);
        dc.DrawBitmap(my_toucan_scaled_normal, x, y+15, true);

        y += yy;
        dc.DrawText("Scaled with high quality", x+50, y);
        dc.DrawBitmap(my_toucan_scaled_high, x, y+15, true);

        y += yy;
        dc.DrawText("Blured", x+50, y);
        dc.DrawBitmap(my_toucan_blur, x, y+15, true);
    }

    if (my_smile_xbm.IsOk())
    {
        int x = 300, y = 1800;

        dc.DrawText( "XBM bitmap", x, y );
        dc.DrawText( "(green on red)", x, y + 15 );
        dc.SetTextForeground( "GREEN" );
        dc.SetTextBackground( "RED" );
        dc.DrawBitmap( my_smile_xbm, x, y + 30 );

        dc.SetTextForeground( *wxBLACK );
        dc.DrawText( "After wxImage conversion", x + 120, y );
        dc.DrawText( "(red on white)", x + 120, y + 15 );
        dc.SetTextForeground( "RED" );
        wxImage i = my_smile_xbm.ConvertToImage();
        i.SetMaskColour( 255, 255, 255 );
        i.Replace( 0, 0, 0,
            wxRED_PEN->GetColour().Red(),
            wxRED_PEN->GetColour().Green(),
            wxRED_PEN->GetColour().Blue() );
        dc.DrawBitmap( wxBitmap(i), x + 120, y + 30, true );
        dc.SetTextForeground( *wxBLACK );
    }


    wxBitmap mono( 60,50,1 );
    wxMemoryDC memdc;
    memdc.SelectObject( mono );
    memdc.SetPen( *wxBLACK_PEN );
    memdc.SetBrush( *wxWHITE_BRUSH );
    memdc.DrawRectangle( 0,0,60,50 );
    memdc.SetTextForeground( *wxBLACK );
#ifndef __WXGTK20__
    // I cannot convince GTK2 to draw into mono bitmaps
    memdc.DrawText( "Hi!", 5, 5 );
#endif
    memdc.SetBrush( *wxBLACK_BRUSH );
    memdc.DrawRectangle( 33,5,20,20 );
    memdc.SetPen( *wxRED_PEN );
    memdc.DrawLine( 5, 42, 50, 42 );
    memdc.SelectObject( wxNullBitmap );

    if (mono.IsOk())
    {
        int x = 300, y = 1900;

        dc.DrawText( "Mono bitmap", x, y );
        dc.DrawText( "(red on green)", x, y + 15 );
        dc.SetTextForeground( "RED" );
        dc.SetTextBackground( "GREEN" );
        dc.DrawBitmap( mono, x, y + 30 );

        dc.SetTextForeground( *wxBLACK );
        dc.DrawText( "After wxImage conversion", x + 120, y );
        dc.DrawText( "(red on white)", x + 120, y + 15 );
        dc.SetTextForeground( "RED" );
        wxImage i = mono.ConvertToImage();
        i.SetMaskColour( 255,255,255 );
        i.Replace( 0,0,0,
            wxRED_PEN->GetColour().Red(),
            wxRED_PEN->GetColour().Green(),
            wxRED_PEN->GetColour().Blue() );
        dc.DrawBitmap( wxBitmap(i), x + 120, y + 30, true );
        dc.SetTextForeground( *wxBLACK );
    }

    // For testing transparency
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( 20, 2220, 560, 68 );

    dc.DrawText("XPM bitmap", 30, 2230 );
    if ( m_bmpSmileXpm.IsOk() )
        dc.DrawBitmap(m_bmpSmileXpm, 30, 2250, true);

    dc.DrawText("XPM icon", 110, 2230 );
    if ( m_iconSmileXpm.IsOk() )
        dc.DrawIcon(m_iconSmileXpm, 110, 2250);

    // testing icon -> bitmap conversion
    wxBitmap to_blit( m_iconSmileXpm );
    if (to_blit.IsOk())
    {
        dc.DrawText( "SubBitmap", 170, 2230 );
        wxBitmap sub = to_blit.GetSubBitmap( wxRect(0,0,15,15) );
        if (sub.IsOk())
            dc.DrawBitmap( sub, 170, 2250, true );

        dc.DrawText( "Enlarged", 250, 2230 );
        dc.SetUserScale( 1.5, 1.5 );
        dc.DrawBitmap( to_blit, (int)(250/1.5), (int)(2250/1.5), true );
        dc.SetUserScale( 2, 2 );
        dc.DrawBitmap( to_blit, (int)(300/2), (int)(2250/2), true );
        dc.SetUserScale( 1.0, 1.0 );

        dc.DrawText( "Blit", 400, 2230);
        wxMemoryDC blit_dc;
        blit_dc.SelectObject( to_blit );
        dc.Blit( 400, 2250, to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true );
        dc.SetUserScale( 1.5, 1.5 );
        dc.Blit( (int)(450/1.5), (int)(2250/1.5), to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true );
        dc.SetUserScale( 2, 2 );
        dc.Blit( (int)(500/2), (int)(2250/2), to_blit.GetWidth(), to_blit.GetHeight(), &blit_dc, 0, 0, wxCOPY, true );
        dc.SetUserScale( 1.0, 1.0 );
    }

    dc.DrawText( "ICO handler (1st image)", 30, 2290 );
    if (my_horse_ico32.IsOk())
        dc.DrawBitmap( my_horse_ico32, 30, 2330, true );

    dc.DrawText( "ICO handler (2nd image)", 230, 2290 );
    if (my_horse_ico16.IsOk())
        dc.DrawBitmap( my_horse_ico16, 230, 2330, true );

    dc.DrawText( "ICO handler (best image)", 430, 2290 );
    if (my_horse_ico.IsOk())
        dc.DrawBitmap( my_horse_ico, 430, 2330, true );

    dc.DrawText( "CUR handler", 30, 2390 );
    if (my_horse_cur.IsOk())
    {
        dc.DrawBitmap( my_horse_cur, 30, 2420, true );
        dc.SetPen (*wxRED_PEN);
        dc.DrawLine (xH-10,yH,xH+10,yH);
        dc.DrawLine (xH,yH-10,xH,yH+10);
    }

    dc.DrawText( "ANI handler", 230, 2390 );
    for ( int i=0; i < m_ani_images; i++ )
    {
        if (my_horse_ani[i].IsOk())
        {
            dc.DrawBitmap( my_horse_ani[i], 230 + i * 2 * my_horse_ani[i].GetWidth() , 2420, true );
        }
    }

    dc.DrawText("PNG from resources", 30, 2460);
    if ( my_png_from_res.IsOk() )
        dc.DrawBitmap(my_png_from_res, 30, 2480, true);
    dc.DrawText("PNG from memory", 230, 2460);
    if ( my_png_from_mem.IsOk() )
        dc.DrawBitmap(my_png_from_mem, 230, 2480, true);
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
