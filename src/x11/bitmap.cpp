/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/control.h"
#include "wx/dcmemory.h"
#include "wx/image.h"
#include "wx/app.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/x11/private.h"

#if wxHAVE_LIB_XPM
    #include <X11/xpm.h>
#endif
#include <math.h>


IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_bitmapMask = NULL;

    m_pixmap = (WXPixmap) 0;
    m_display = (WXDisplay*) 0;

    m_freePixmap = TRUE; //TODO: necessary?
    m_freeColors = (unsigned long*) 0;
    m_freeColorsCount = 0;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap && m_freePixmap)
        XFreePixmap ((Display*) m_display, (Pixmap) m_pixmap);

    if (m_freeColors)
    {
        int screen = DefaultScreen((Display*) m_display);
        Colormap cmp = DefaultColormap((Display*) m_display,screen);
        long llp;
        for(llp = 0;llp < m_freeColorsCount;llp++)
            XFreeColors((Display*) m_display, cmp, &m_freeColors[llp], 1, 0L);
        delete m_freeColors;
    };

    if (m_bitmapMask)
        delete m_bitmapMask;
    m_bitmapMask = NULL;
}

wxList wxBitmap::sm_handlers;

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

wxBitmap::wxBitmap()
{
    m_refData = NULL;
}

wxBitmap::~wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    m_refData = new wxBitmapRefData;

    (void) Create((void*) bits, wxBITMAP_TYPE_XBM_DATA, width, height, depth);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);
}

wxBitmap::wxBitmap(void *data, long type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxString& filename, long type)
{
    LoadFile(filename, (int)type);
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    if (d < 1)
        d = wxDisplayDepth();

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = d;
    M_BITMAPDATA->m_freePixmap = TRUE;

    Display *dpy = (Display*) wxGetDisplay();

    M_BITMAPDATA->m_display = dpy; /* MATTHEW: [4] Remember the display */

    M_BITMAPDATA->m_pixmap = (WXPixmap) XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
        w, h, d);

    M_BITMAPDATA->m_ok = (M_BITMAPDATA->m_pixmap != (WXPixmap) 0) ;
    return M_BITMAPDATA->m_ok;
}

bool wxBitmap::LoadFile(const wxString& filename, long type)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxImage image;
        if (!image.LoadFile( filename, type )) return FALSE;
        if (image.Ok()) 
        {
            *this = image.ConvertToBitmap();
            return TRUE;
        }
        else return FALSE;
    }

    return handler->LoadFile(this, filename, type, -1, -1);
}

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no data bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename, int type, const wxPalette *palette)
{
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) { // try wxImage
        wxImage image( *this );
        if (image.Ok()) return image.SaveFile( filename, type );
        else return FALSE;
    }

    return handler->SaveFile(this, filename, type, palette);
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_height = h;
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_depth = d;
}

void wxBitmap::SetQuality(int q)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_quality = q;
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_ok = isOk;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapPalette = palette ;
}

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, 0 );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

   // The remaining still TODO
   return ret;
}

void wxBitmap::AddHandler(wxBitmapHandler *handler)
{
    sm_handlers.Append(handler);
}

void wxBitmap::InsertHandler(wxBitmapHandler *handler)
{
    sm_handlers.Insert(handler);
}

bool wxBitmap::RemoveHandler(const wxString& name)
{
    wxBitmapHandler *handler = FindHandler(name);
    if ( handler )
    {
        sm_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& name)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetName() == name )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& extension, long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetExtension() == extension &&
            (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if (handler->GetType() == bitmapType)
            return handler;
        node = node->Next();
    }
    return NULL;
}

/*
* wxMask
*/

wxMask::wxMask()
{
    m_pixmap = (WXPixmap) 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap);
}

wxMask::~wxMask()
{
    // TODO: this may be the wrong display
    if ( m_pixmap )
        XFreePixmap ((Display*) wxGetDisplay(), (Pixmap) m_pixmap);
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap))
{
    // TODO
    return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap), int WXUNUSED(paletteIndex))
{
    // TODO
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap), const wxColour& WXUNUSED(colour))
{
    // TODO
    return FALSE;
}

/*
* wxBitmapHandler
*/

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

bool wxBitmapHandler::Create(wxBitmap *WXUNUSED(bitmap), void *WXUNUSED(data), long WXUNUSED(type),
                             int WXUNUSED(width), int WXUNUSED(height), int WXUNUSED(depth))
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), long WXUNUSED(type),
                               int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), int WXUNUSED(type),
                               const wxPalette *WXUNUSED(palette))
{
    return FALSE;
}

/*
* Standard handlers
*/

class WXDLLEXPORT wxXBMFileHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXBMFileHandler)
public:
    inline wxXBMFileHandler()
    {
        m_name = "XBM file";
        m_extension = "xbm";
        m_type = wxBITMAP_TYPE_XBM;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
        int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxXBMFileHandler, wxBitmapHandler)

bool wxXBMFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long WXUNUSED(flags),
                                int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
    M_BITMAPHANDLERDATA->m_freePixmap = TRUE;

    int hotX, hotY;
    unsigned int w, h;
    Pixmap pixmap;

    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPDATA->m_display = (WXDisplay*) dpy;

    int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)),
        (char*) (const char*) name, &w, &h, &pixmap, &hotX, &hotY);
    M_BITMAPHANDLERDATA->m_width = w;
    M_BITMAPHANDLERDATA->m_height = h;
    M_BITMAPHANDLERDATA->m_depth = 1;
    M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) pixmap;

    if ((value == BitmapFileInvalid) ||
        (value == BitmapOpenFailed) ||
        (value == BitmapNoMemory))
    {
        M_BITMAPHANDLERDATA->m_ok = FALSE;
        M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) 0;
    }
    else
        M_BITMAPHANDLERDATA->m_ok = TRUE;

    return M_BITMAPHANDLERDATA->m_ok ;
}

class WXDLLEXPORT wxXBMDataHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXBMDataHandler)
public:
    inline wxXBMDataHandler()
    {
        m_name = "XBM data";
        m_extension = "xbm";
        m_type = wxBITMAP_TYPE_XBM_DATA;
    };

    virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
};
IMPLEMENT_DYNAMIC_CLASS(wxXBMDataHandler, wxBitmapHandler)

bool wxXBMDataHandler::Create( wxBitmap *bitmap, void *data, long WXUNUSED(flags),
                              int width, int height, int WXUNUSED(depth))
{
    M_BITMAPHANDLERDATA->m_width = width;
    M_BITMAPHANDLERDATA->m_height = height;
    M_BITMAPHANDLERDATA->m_depth = 1;
    M_BITMAPHANDLERDATA->m_freePixmap = TRUE;

    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPHANDLERDATA->m_display = (WXDisplay*) dpy;

    M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) XCreateBitmapFromData (dpy, RootWindow (dpy, DefaultScreen (dpy)), (char*) data, width, height);
    M_BITMAPHANDLERDATA->m_ok = (M_BITMAPHANDLERDATA->m_pixmap != (WXPixmap) 0) ;

    return TRUE;
}

void wxBitmap::CleanUpHandlers()
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

void wxBitmap::InitStandardHandlers()
{
    // Initialize all standard bitmap or derived class handlers here.
    AddHandler(new wxXBMFileHandler);
    AddHandler(new wxXBMDataHandler);

    // XPM will be handled by wxImage
}

// Creates a bitmap with transparent areas drawn in
// the given colour.
wxBitmap wxCreateMaskedBitmap(const wxBitmap& bitmap, wxColour& colour)
{
    wxBitmap newBitmap(bitmap.GetWidth(),
        bitmap.GetHeight(),
        bitmap.GetDepth());
    wxMemoryDC destDC;
    wxMemoryDC srcDC;
    srcDC.SelectObject(bitmap);
    destDC.SelectObject(newBitmap);

    wxBrush brush(colour, wxSOLID);
    destDC.SetOptimization(FALSE);
    destDC.SetBackground(brush);
    destDC.Clear();
    destDC.Blit(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), & srcDC, 0, 0, wxCOPY, TRUE);

    return newBitmap;
}

//-----------------------------------------------------------------------------
// wxImage conversion routines
//-----------------------------------------------------------------------------

/*

Date: Wed, 05 Jan 2000 11:45:40 +0100
From: Frits Boel <boel@niob.knaw.nl>
To: julian.smart@ukonline.co.uk
Subject: Patch for Motif ConvertToBitmap

Hi Julian,

I've been working on a wxWin application for image processing. From the
beginning, I was surprised by the (lack of) speed of ConvertToBitmap,
till I looked in the source code of image.cpp. I saw that converting a
wxImage to a bitmap with 8-bit pixels is done with comparing every pixel
to the 256 colors of the palet. A very time-consuming piece of code!

Because I wanted a faster application, I've made a 'patch' for this. In
short: every pixel of the image is compared to a sorted list with
colors. If the color is found in the list, the palette entry is
returned; if the color is not found, the color palette is searched and
then the palette entry is returned and the color added to the sorted
list.

Maybe there is another method for this, namely changing the palette
itself (if the colors are known, as is the case with tiffs with a
colormap). I did not look at this, maybe someone else did?

The code of the patch is attached, have a look on it, and maybe you will
ship it with the next release of wxMotif?

Regards,

Frits Boel
Software engineer at Hubrecht Laboratory, The Netherlands.

*/

class wxSearchColor
{
public:
  wxSearchColor( void );
  wxSearchColor( int size, XColor *colors );
  ~wxSearchColor( void );

  int SearchColor( int r, int g, int b );
private:
  int AddColor( unsigned int value, int pos );

  int          size;
  XColor       *colors;
  unsigned int *color;
  int          *entry;

  int bottom;
  int top;
};

wxSearchColor::wxSearchColor( void )
{
  size   = 0;
  colors = (XColor*) NULL;
  color  = (unsigned int *) NULL;
  entry  = (int*) NULL;

  bottom = 0;
  top    = 0;
}

wxSearchColor::wxSearchColor( int size_, XColor *colors_ )
{
    int i;
    size   = size_;
    colors = colors_;
    color  = new unsigned int[size];
    entry  = new int         [size];

    for (i = 0; i < size; i++ ) {
        entry[i] = -1;
    }

    bottom = top = ( size >> 1 );
}

wxSearchColor::~wxSearchColor( void )
{
  if ( color ) delete color;
  if ( entry ) delete entry;
}

int wxSearchColor::SearchColor( int r, int g, int b )
{
  unsigned int value = ( ( ( r * 256 ) + g ) * 256 ) + b;
  int          begin = bottom;
  int          end   = top;
  int          middle = 0;

  while ( begin <= end ) {

    middle = ( begin + end ) >> 1;

    if ( value == color[middle] ) {
      return( entry[middle] );
    } else if ( value < color[middle] ) {
      end = middle - 1;
    } else {
      begin = middle + 1;
    }

  }

  return AddColor( value, middle );
}

int wxSearchColor::AddColor( unsigned int value, int pos )
{
  int i;
  int pixel = -1;
  int max = 3 * (65536);
  for ( i = 0; i < 256; i++ ) {
    int rdiff = ((value >> 8) & 0xFF00 ) - colors[i].red;
    int gdiff = ((value     ) & 0xFF00 ) - colors[i].green;
    int bdiff = ((value << 8) & 0xFF00 ) - colors[i].blue;
    int sum = abs (rdiff) + abs (gdiff) + abs (bdiff);
    if (sum < max) { pixel = i; max = sum; }
  }

  if ( entry[pos] < 0 ) {
    color[pos] = value;
    entry[pos] = pixel;
  } else if ( value < color[pos] ) {

    if ( bottom > 0 ) {
      for ( i = bottom; i < pos; i++ ) {
        color[i-1] = color[i];
        entry[i-1] = entry[i];
      }
      bottom--;
      color[pos-1] = value;
      entry[pos-1] = pixel;
    } else if ( top < size-1 ) {
      for ( i = top; i >= pos; i-- ) {
        color[i+1] = color[i];
        entry[i+1] = entry[i];
      }
      top++;
      color[pos] = value;
      entry[pos] = pixel;
    }

  } else {

    if ( top < size-1 ) {
      for ( i = top; i > pos; i-- ) {
        color[i+1] = color[i];
        entry[i+1] = entry[i];
      }
      top++;
      color[pos+1] = value;
      entry[pos+1] = pixel;
    } else if ( bottom > 0 ) {
      for ( i = bottom; i < pos; i++ ) {
        color[i-1] = color[i];
        entry[i-1] = entry[i];
      }
      bottom--;
      color[pos] = value;
      entry[pos] = pixel;
    }

  }

  return( pixel );
}


bool wxBitmap::CreateFromImage( const wxImage& image, int depth )
{
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )
    wxCHECK_MSG( depth == -1, FALSE, wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();

    int width = image.GetWidth();
    int height = image.GetHeight();

    SetHeight( height );
    SetWidth( width );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    // Create image

    XImage *data_image = XCreateImage( dpy, vis, bpp, ZPixmap, 0, 0, width, height, 32, 0 );
    data_image->data = (char*) malloc( data_image->bytes_per_line * data_image->height );

    Create( width, height, bpp );

    // Create mask

    XImage *mask_image = (XImage*) NULL;
    if (image.HasMask())
    {
        mask_image = XCreateImage( dpy, vis, 1, ZPixmap, 0, 0, width, height, 32, 0 );
        mask_image->data = (char*) malloc( mask_image->bytes_per_line * mask_image->height );
    }

    // Retrieve depth info

    XVisualInfo vinfo_template;
    XVisualInfo *vi;

    vinfo_template.visual = vis;
    vinfo_template.visualid = XVisualIDFromVisual( vis );
    vinfo_template.depth = bpp;
    int nitem = 0;

    vi = XGetVisualInfo( dpy, VisualIDMask|VisualDepthMask, &vinfo_template, &nitem );

    wxCHECK_MSG( vi, FALSE, wxT("no visual") );

    XFree( vi );

    if ((bpp == 16) && (vi->red_mask != 0xf800)) bpp = 15;
    if (bpp < 8) bpp = 8;

    // Render

    enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
    byte_order b_o = RGB;

    if (bpp >= 24)
    {
        if ((vi->red_mask > vi->green_mask) && (vi->green_mask > vi->blue_mask))      b_o = RGB;
        else if ((vi->red_mask > vi->blue_mask) && (vi->blue_mask > vi->green_mask))  b_o = RGB;
        else if ((vi->blue_mask > vi->red_mask) && (vi->red_mask > vi->green_mask))   b_o = BRG;
        else if ((vi->blue_mask > vi->green_mask) && (vi->green_mask > vi->red_mask)) b_o = BGR;
        else if ((vi->green_mask > vi->red_mask) && (vi->red_mask > vi->blue_mask))   b_o = GRB;
        else if ((vi->green_mask > vi->blue_mask) && (vi->blue_mask > vi->red_mask))  b_o = GBR;
    }

    int r_mask = image.GetMaskRed();
    int g_mask = image.GetMaskGreen();
    int b_mask = image.GetMaskBlue();

    XColor colors[256];
    if (bpp == 8)
    {
        Colormap cmap = (Colormap) wxTheApp->GetMainColormap( dpy );

        for (int i = 0; i < 256; i++) colors[i].pixel = i;
        XQueryColors( dpy, cmap, colors, 256 );
    }

    wxSearchColor scolor( 256, colors );
    unsigned char* data = image.GetData();

    bool hasMask = image.HasMask();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            if (hasMask)
            {
              if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                XPutPixel( mask_image, x, y, 0 );
              else
                XPutPixel( mask_image, x, y, 1 );
            }

            switch (bpp)
            {
            case 8:
                {
#if 0 // Old, slower code
                    int pixel = -1;
                    /*
                    if (wxTheApp->m_colorCube)
                    {
                    pixel = wxTheApp->m_colorCube
                    [ ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + ((b & 0xf8) >> 3) ];
                    }
                    else
                    {
                    */
                    int max = 3 * (65536);
                    for (int i = 0; i < 256; i++)
                    {
                        int rdiff = (r << 8) - colors[i].red;
                        int gdiff = (g << 8) - colors[i].green;
                        int bdiff = (b << 8) - colors[i].blue;
                        int sum = abs (rdiff) + abs (gdiff) + abs (bdiff);
                        if (sum < max) { pixel = i; max = sum; }
                    }
                    /*
                    }
                    */
#endif

                    // And this is all to get the 'right' color...
                    int pixel = scolor.SearchColor( r, g, b );
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 15:
                {
                    int pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 16:
                {
                    int pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
                    XPutPixel( data_image, x, y, pixel );
                    break;
                }
            case 32:
            case 24:
                {
                    int pixel = 0;
                    switch (b_o)
                    {
                    case RGB: pixel = (r << 16) | (g << 8) | b; break;
                    case RBG: pixel = (r << 16) | (b << 8) | g; break;
                    case BRG: pixel = (b << 16) | (r << 8) | g; break;
                    case BGR: pixel = (b << 16) | (g << 8) | r; break;
                    case GRB: pixel = (g << 16) | (r << 8) | b; break;
                    case GBR: pixel = (g << 16) | (b << 8) | r; break;
                    }
                    XPutPixel( data_image, x, y, pixel );
                }
            default: break;
            }
        } // for
    }  // for

    // Blit picture

    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel( dpy, DefaultScreen( dpy ) );
    GC gc = XCreateGC( dpy, RootWindow ( dpy, DefaultScreen(dpy) ), GCForeground, &gcvalues );
    XPutImage( dpy, (Drawable)GetPixmap(), gc, data_image, 0, 0, 0, 0, width, height );

    XDestroyImage( data_image );
    XFreeGC( dpy, gc );

    // Blit mask
    if (image.HasMask())
    {
        wxBitmap maskBitmap(width, height, 1);

        GC gcMask = XCreateGC( dpy, (Pixmap) maskBitmap.GetPixmap(), 0, (XGCValues*)NULL );
        XPutImage( dpy, (Drawable)maskBitmap.GetPixmap(), gcMask, mask_image, 0, 0, 0, 0, width, height );

        XDestroyImage( mask_image );
        XFreeGC( dpy, gcMask );

        wxMask* mask = new wxMask;
        mask->SetPixmap(maskBitmap.GetPixmap());

        SetMask(mask);

        maskBitmap.SetPixmapNull();
    }


    return TRUE;
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;
    
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    Display *dpy = (Display*) wxGetDisplay();
    Visual* vis = DefaultVisual( dpy, DefaultScreen( dpy ) );
    int bpp = DefaultDepth( dpy, DefaultScreen( dpy ) );

    XImage *ximage = XGetImage( dpy,
        (Drawable)GetPixmap(),
        0, 0,
        GetWidth(), GetHeight(),
        AllPlanes, ZPixmap );

    wxCHECK_MSG( ximage, wxNullImage, wxT("couldn't create image") );

    image.Create( GetWidth(), GetHeight() );
    char unsigned *data = image.GetData();

    if (!data)
    {
        XDestroyImage( ximage );
        wxFAIL_MSG( wxT("couldn't create image") );
        return wxNullImage;
    }

    /*
    GdkImage *gdk_image_mask = (GdkImage*) NULL;
    if (GetMask())
    {
    gdk_image_mask = gdk_image_get( GetMask()->GetBitmap(),
    0, 0,
    GetWidth(), GetHeight() );

      image.SetMaskColour( 16, 16, 16 );  // anything unlikely and dividable
      }
    */

    // Retrieve depth info

    XVisualInfo vinfo_template;
    XVisualInfo *vi;

    vinfo_template.visual = vis;
    vinfo_template.visualid = XVisualIDFromVisual( vis );
    vinfo_template.depth = bpp;
    int nitem = 0;

    vi = XGetVisualInfo( dpy, VisualIDMask|VisualDepthMask, &vinfo_template, &nitem );

    wxCHECK_MSG( vi, wxNullImage, wxT("no visual") );

    if ((bpp == 16) && (vi->red_mask != 0xf800)) bpp = 15;

    XFree( vi );

    XColor colors[256];
    if (bpp == 8)
    {
        Colormap cmap = (Colormap)wxTheApp->GetMainColormap( dpy );

        for (int i = 0; i < 256; i++) colors[i].pixel = i;
        XQueryColors( dpy, cmap, colors, 256 );
    }

    long pos = 0;
    for (int j = 0; j < GetHeight(); j++)
    {
        for (int i = 0; i < GetWidth(); i++)
        {
            int pixel = XGetPixel( ximage, i, j );
            if (bpp <= 8)
            {
                data[pos] = colors[pixel].red >> 8;
                data[pos+1] = colors[pixel].green >> 8;
                data[pos+2] = colors[pixel].blue >> 8;
            } else if (bpp == 15)
            {
                data[pos] = (pixel >> 7) & 0xf8;
                data[pos+1] = (pixel >> 2) & 0xf8;
                data[pos+2] = (pixel << 3) & 0xf8;
            } else if (bpp == 16)
            {
                data[pos] = (pixel >> 8) & 0xf8;
                data[pos+1] = (pixel >> 3) & 0xfc;
                data[pos+2] = (pixel << 3) & 0xf8;
            } else
            {
                data[pos] = (pixel >> 16) & 0xff;
                data[pos+1] = (pixel >> 8) & 0xff;
                data[pos+2] = pixel & 0xff;
            }

            /*
            if (gdk_image_mask)
            {
            int mask_pixel = gdk_image_get_pixel( gdk_image_mask, i, j );
            if (mask_pixel == 0)
            {
            data[pos] = 16;
            data[pos+1] = 16;
            data[pos+2] = 16;
            }
            }
            */

            pos += 3;
        }
    }

    XDestroyImage( ximage );
    /*
    if (gdk_image_mask) gdk_image_destroy( gdk_image_mask );
    */

    return image;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    wxBitmap* bitmap = (wxBitmap*) & icon;

    *this = *bitmap;

    return TRUE;
}
