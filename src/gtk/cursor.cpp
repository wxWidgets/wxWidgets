/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/cursor.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/image.h"
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:

    wxCursorRefData();
    virtual ~wxCursorRefData();

    GdkCursor *m_cursor;
};

wxCursorRefData::wxCursorRefData()
{
    m_cursor = (GdkCursor *) NULL;
}

wxCursorRefData::~wxCursorRefData()
{
    if (m_cursor) gdk_cursor_unref( m_cursor );
}

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor,wxObject)

wxCursor::wxCursor()
{

}

wxCursor::wxCursor( int cursorId )
{
    m_refData = new wxCursorRefData();

    GdkCursorType gdk_cur = GDK_LEFT_PTR;
    switch (cursorId)
    {
        case wxCURSOR_BLANK:
            {
                static const gchar bits[] = { 0 };
                static /* const -- not in GTK1 */ GdkColor color = { 0, 0, 0, 0 };

                GdkPixmap *pixmap = gdk_bitmap_create_from_data(NULL, bits, 1, 1);
                M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixmap(pixmap,
                                                                    pixmap,
                                                                    &color,
                                                                    &color,
                                                                    0, 0);
            }
            return;

        case wxCURSOR_ARROW:            // fall through to default
        case wxCURSOR_DEFAULT:          gdk_cur = GDK_LEFT_PTR; break;
        case wxCURSOR_RIGHT_ARROW:      gdk_cur = GDK_RIGHT_PTR; break;
        case wxCURSOR_HAND:             gdk_cur = GDK_HAND1; break;
        case wxCURSOR_CROSS:            gdk_cur = GDK_CROSSHAIR; break;
        case wxCURSOR_SIZEWE:           gdk_cur = GDK_SB_H_DOUBLE_ARROW; break;
        case wxCURSOR_SIZENS:           gdk_cur = GDK_SB_V_DOUBLE_ARROW; break;
        case wxCURSOR_ARROWWAIT:
        case wxCURSOR_WAIT:
        case wxCURSOR_WATCH:            gdk_cur = GDK_WATCH; break;
        case wxCURSOR_SIZING:           gdk_cur = GDK_FLEUR; break;
        case wxCURSOR_SPRAYCAN:         gdk_cur = GDK_SPRAYCAN; break;
        case wxCURSOR_IBEAM:            gdk_cur = GDK_XTERM; break;
        case wxCURSOR_PENCIL:           gdk_cur = GDK_PENCIL; break;
        case wxCURSOR_NO_ENTRY:         gdk_cur = GDK_PIRATE; break;
        case wxCURSOR_SIZENWSE:
        case wxCURSOR_SIZENESW:         gdk_cur = GDK_FLEUR; break;
        case wxCURSOR_QUESTION_ARROW:   gdk_cur = GDK_QUESTION_ARROW; break;
        case wxCURSOR_PAINT_BRUSH:      gdk_cur = GDK_SPRAYCAN; break;
        case wxCURSOR_MAGNIFIER:        gdk_cur = GDK_PLUS; break;
        case wxCURSOR_CHAR:             gdk_cur = GDK_XTERM; break;
        case wxCURSOR_LEFT_BUTTON:      gdk_cur = GDK_LEFTBUTTON; break;
        case wxCURSOR_MIDDLE_BUTTON:    gdk_cur = GDK_MIDDLEBUTTON; break;
        case wxCURSOR_RIGHT_BUTTON:     gdk_cur = GDK_RIGHTBUTTON; break;
        case wxCURSOR_BULLSEYE:         gdk_cur = GDK_TARGET; break;

        case wxCURSOR_POINT_LEFT:       gdk_cur = GDK_SB_LEFT_ARROW; break;
        case wxCURSOR_POINT_RIGHT:      gdk_cur = GDK_SB_RIGHT_ARROW; break;
/*
        case wxCURSOR_DOUBLE_ARROW:     gdk_cur = GDK_DOUBLE_ARROW; break;
        case wxCURSOR_CROSS_REVERSE:    gdk_cur = GDK_CROSS_REVERSE; break;
        case wxCURSOR_BASED_ARROW_UP:   gdk_cur = GDK_BASED_ARROW_UP; break;
        case wxCURSOR_BASED_ARROW_DOWN: gdk_cur = GDK_BASED_ARROW_DOWN; break;
*/

        default:
            wxFAIL_MSG(wxT("unsupported cursor type"));
            // will use the standard one
            break;
    }

    M_CURSORDATA->m_cursor = gdk_cursor_new( gdk_cur );
}

extern GtkWidget *wxGetRootWindow();

wxCursor::wxCursor(const char bits[], int width, int  height,
                   int hotSpotX, int hotSpotY,
                   const char maskBits[], const wxColour *fg, const wxColour *bg)
{
    if (!maskBits)
        maskBits = bits;
    if (!fg)
        fg = wxBLACK;
    if (!bg)
        bg = wxWHITE;
    if (hotSpotX < 0 || hotSpotX >= width)
        hotSpotX = 0;
    if (hotSpotY < 0 || hotSpotY >= height)
        hotSpotY = 0;

    GdkBitmap *data = gdk_bitmap_create_from_data( wxGetRootWindow()->window, (gchar *) bits, width, height );
    GdkBitmap *mask = gdk_bitmap_create_from_data( wxGetRootWindow()->window, (gchar *) maskBits, width, height);

    m_refData = new wxCursorRefData;
    M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixmap(
                 data, mask, fg->GetColor(), bg->GetColor(),
                 hotSpotX, hotSpotY );

    g_object_unref (data);
    g_object_unref (mask);
}

#if wxUSE_IMAGE

static void GetHotSpot(const wxImage& image, int& x, int& y)
{
    if (image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X))
        x = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X);
    else
        x = 0;

    if (image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y))
        y = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y);
    else
        y = 0;

    if (x < 0 || x >= image.GetWidth())
        x = 0;
    if (y < 0 || y >= image.GetHeight())
        y = 0;
}

wxCursor::wxCursor( const wxImage & image )
{
    unsigned char * rgbBits = image.GetData();
    int w = image.GetWidth() ;
    int h = image.GetHeight();
    bool bHasMask = image.HasMask();
    int imagebitcount = (w*h)/8;

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        if ( gdk_display_supports_cursor_color(gdk_display_get_default()) )
        {
            unsigned char rMask = 0,
                          gMask = 0,
                          bMask = 0;
            if (bHasMask)
            {
                rMask = image.GetMaskRed();
                gMask = image.GetMaskGreen();
                bMask = image.GetMaskBlue();
            }

            GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, w, h);
            unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : NULL;
            unsigned char *out = gdk_pixbuf_get_pixels(pixbuf);
            int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - 4 * w;
            for ( int y = 0; y < h; y++, out += rowpad )
            {
                for ( int x = 0; x < w; x++, out += 4, rgbBits += 3 )
                {
                    out[0] = rgbBits[0];
                    out[1] = rgbBits[1];
                    out[2] = rgbBits[2];
                    if (bHasMask &&
                        out[0] == rMask && out[1] == gMask && out[2] == bMask)
                        out[3] = 0;
                    else
                        out[3] = alpha ? *alpha : 255;
                    if ( alpha )
                        ++alpha;
                }
            }

            int hotSpotX, hotSpotY;
            GetHotSpot(image, hotSpotX, hotSpotY);

            m_refData = new wxCursorRefData;
            M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixbuf
                                 (
                                  gdk_display_get_default(),
                                  pixbuf,
                                  hotSpotX, hotSpotY
                                 );
            g_object_unref (pixbuf);
            return;
        }
    }
#endif // GTK+ 2.4+

    unsigned char * bits = new unsigned char [imagebitcount];
    unsigned char * maskBits = new unsigned char [imagebitcount];

    int i, j, i8; unsigned char c, cMask;
    for (i=0; i<imagebitcount; i++)
    {
        bits[i] = 0;
        i8 = i * 8;

        cMask = 1;
        for (j=0; j<8; j++)
        {
            // possible overflow if we do the summation first ?
            c = rgbBits[(i8+j)*3]/3 + rgbBits[(i8+j)*3+1]/3 + rgbBits[(i8+j)*3+2]/3;
            //if average value is > mid grey
            if (c>127)
                bits[i] = bits[i] | cMask;
            cMask = cMask * 2;
        }
    }

    unsigned long keyMaskColor;
    if (bHasMask)
    {
        unsigned char
            r = image.GetMaskRed(),
            g = image.GetMaskGreen(),
            b = image.GetMaskBlue();

        for (i=0; i<imagebitcount; i++)
        {
            maskBits[i] = 0x0;
            i8 = i * 8;

            cMask = 1;
            for (j=0; j<8; j++)
            {
                if (rgbBits[(i8+j)*3] != r || rgbBits[(i8+j)*3+1] != g || rgbBits[(i8+j)*3+2] != b)
                    maskBits[i] = maskBits[i] | cMask;
                cMask = cMask * 2;
            }
        }

        keyMaskColor = (r << 16) | (g << 8) | b;
    }
    else // no mask
    {
        for (i=0; i<imagebitcount; i++)
            maskBits[i] = 0xFF;

        // init it to avoid compiler warnings
        keyMaskColor = 0;
    }

    // find the most frequent color(s)
    wxImageHistogram histogram;
    image.ComputeHistogram(histogram);

    // colors as rrggbb
    unsigned long key;
    unsigned long value;

    long colMostFreq = 0;
    unsigned long nMost = 0;
    long colNextMostFreq = 0;
    unsigned long nNext = 0;
    for ( wxImageHistogram::iterator entry = histogram.begin();
          entry != histogram.end();
          ++entry )
    {
        value = entry->second.value;
        key = entry->first;
        if ( !bHasMask || (key != keyMaskColor) )
        {
            if (value > nMost)
            {
                nMost = value;
                colMostFreq = key;
            }
            else if (value > nNext)
            {
                nNext = value;
                colNextMostFreq = key;
            }
        }
    }

    wxColour fg = wxColour ( (unsigned char)(colMostFreq >> 16),
                             (unsigned char)(colMostFreq >> 8),
                             (unsigned char)(colMostFreq) );

    wxColour bg = wxColour ( (unsigned char)(colNextMostFreq >> 16),
                             (unsigned char)(colNextMostFreq >> 8),
                             (unsigned char)(colNextMostFreq) );

    int fg_intensity = fg.Red() + fg.Green() + fg.Blue();
    int bg_intensity = bg.Red() + bg.Green() + bg.Blue();

    if (bg_intensity > fg_intensity)
    {
        //swap fg and bg
        wxColour tmp = fg;
        fg = bg;
        bg = tmp;
    }

    int hotSpotX, hotSpotY;
    GetHotSpot(image, hotSpotX, hotSpotY);

    GdkBitmap *data = gdk_bitmap_create_from_data(wxGetRootWindow()->window,
                                                  (gchar *) bits, w, h);
    GdkBitmap *mask = gdk_bitmap_create_from_data(wxGetRootWindow()->window,
                                                  (gchar *) maskBits, w, h);

    m_refData = new wxCursorRefData;
    M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixmap
                             (
                                data,
                                mask,
                                fg.GetColor(), bg.GetColor(),
                                hotSpotX, hotSpotY
                             );

    g_object_unref (data);
    g_object_unref (mask);
    delete [] bits;
    delete [] maskBits;
}

#endif // wxUSE_IMAGE

wxCursor::~wxCursor()
{
}

bool wxCursor::IsOk() const
{
    return (m_refData != NULL);
}

GdkCursor *wxCursor::GetCursor() const
{
    return M_CURSORDATA->m_cursor;
}

//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

/* Current cursor, in order to hang on to
 * cursor handle when setting the cursor globally */
wxCursor g_globalCursor;

static wxCursor  gs_savedCursor;
static int       gs_busyCount = 0;

const wxCursor &wxBusyCursor::GetStoredCursor()
{
    return gs_savedCursor;
}

const wxCursor wxBusyCursor::GetBusyCursor()
{
    return wxCursor(wxCURSOR_WATCH);
}

void wxEndBusyCursor()
{
    if (--gs_busyCount > 0)
        return;

    wxSetCursor( gs_savedCursor );
    gs_savedCursor = wxNullCursor;

    if (wxTheApp)
        wxTheApp->ProcessIdle();
}

void wxBeginBusyCursor( const wxCursor *WXUNUSED(cursor) )
{
    if (gs_busyCount++ > 0)
        return;

    wxASSERT_MSG( !gs_savedCursor.Ok(),
                  wxT("forgot to call wxEndBusyCursor, will leak memory") );

    gs_savedCursor = g_globalCursor;

    wxSetCursor( wxCursor(wxCURSOR_WATCH) );

    if (wxTheApp)
        wxTheApp->ProcessIdle();

    gdk_flush();
}

bool wxIsBusy()
{
    return gs_busyCount > 0;
}

void wxSetCursor( const wxCursor& cursor )
{
    g_globalCursor = cursor;
    wxTheApp->WakeUpIdle();
}
