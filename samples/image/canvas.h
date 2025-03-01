///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/canvas.h
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// Copyright:   (c) 1998-2005 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/scrolwin.h"

//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow* parent);
    ~MyCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnDPIChanged(wxDPIChangedEvent& event);
    wxBitmap const& GetPngBitmap() const;
    wxSize GetDrawingSize() const;

private:
    void DrawSquares(wxDC& dc, int xOffset, int yOffset);
    void CreateAntiAliasedBitmap();

    wxBitmap  my_horse_png,
              my_horse_png_scaled;
    wxBitmap  my_horse_jpeg;
    wxBitmap  my_horse_gif;
    wxBitmap  my_horse_bmp;
    wxBitmap  my_horse_bmp2;
    wxBitmap  my_horse_pcx;
    wxBitmap  my_horse_pnm;
    wxBitmap  my_horse_tiff;
    wxBitmap  my_horse_tga;
    wxBitmap  my_horse_webp;
    wxBitmap  my_horse_xpm;
    wxBitmap  my_horse_ico32;
    wxBitmap  my_horse_ico16;
    wxBitmap  my_horse_ico;
    wxBitmap  my_horse_cur;
    wxBitmap  my_png_from_res,
              my_png_from_mem;

    wxBitmap  my_smile_xbm;
    wxBitmap  my_square;
    wxBitmap  my_anti;

    wxBitmap  my_horse_asciigrey_pnm;
    wxBitmap  my_horse_rawgrey_pnm;

    wxBitmap  colorized_horse_jpeg;
    wxBitmap  my_cmyk_jpeg;

    wxBitmap my_toucan;
    wxBitmap my_toucan_flipped_horiz;
    wxBitmap my_toucan_flipped_vert;
    wxBitmap my_toucan_flipped_both;
    wxBitmap my_toucan_grey;
    wxBitmap my_toucan_head;
    wxBitmap my_toucan_scaled_normal;
    wxBitmap my_toucan_scaled_high;
    wxBitmap my_toucan_blur;

    wxPoint cur_hotspot;
    int m_ani_images;
    wxBitmap *my_horse_ani;

    wxBitmap m_bmpSmileXpm;
    wxIcon   m_iconSmileXpm;

    wxDECLARE_EVENT_TABLE();
};
