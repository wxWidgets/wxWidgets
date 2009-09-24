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
    #include "wx/window.h"
    #include "wx/image.h"
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/gtk/private.h" //for idle stuff

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
                g_object_unref(pixmap);
            }
            return;

        case wxCURSOR_ARROW:            // fall through to default
        case wxCURSOR_DEFAULT:          gdk_cur = GDK_LEFT_PTR; break;
        case wxCURSOR_RIGHT_ARROW:      gdk_cur = GDK_RIGHT_PTR; break;
        case wxCURSOR_HAND:             gdk_cur = GDK_HAND2; break;
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
    int w = image.GetWidth() ;
    int h = image.GetHeight();
    bool bHasMask = image.HasMask();
    int hotSpotX, hotSpotY;
    GetHotSpot(image, hotSpotX, hotSpotY);
    m_refData = new wxCursorRefData;
    wxImage image_copy(image);

    GdkDisplay* display = gdk_drawable_get_display(wxGetRootWindow()->window);
    if (gdk_display_supports_cursor_color(display))
    {
        if (!image.HasAlpha())
        {
            // add alpha, so wxBitmap will convert to pixbuf format
            image_copy.InitAlpha();
        }
        wxBitmap bitmap(image_copy);
        wxASSERT(bitmap.HasPixbuf());
        M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixbuf
                             (
                              display,
                              bitmap.GetPixbuf(),
                              hotSpotX, hotSpotY
                             );
        return;
    }

    unsigned long keyMaskColor = 0;
    GdkPixmap* mask;
    if (bHasMask)
    {
        keyMaskColor = wxImageHistogram::MakeKey(
            image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        // get mask before image is modified
        wxBitmap bitmap(image, 1);
        mask = bitmap.GetMask()->GetBitmap();
        g_object_ref(mask);
    }
    else
    {
        const int size = ((w + 7) / 8) * h;
        char* bits = new char[size];
        memset(bits, 0xff, size);
        mask = gdk_bitmap_create_from_data(
            wxGetRootWindow()->window, bits, w, h);
        delete[] bits;
    }

    // modify image so wxBitmap can be used to convert to pixmap
    image_copy.SetMask(false);
    int i, j;
    wxByte* data = image_copy.GetData();
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++, data += 3)
        {
            //if average value is > mid grey
            if (int(data[0]) + data[1] + data[2] >= 3 * 128)
            {
                // wxBitmap only converts (255,255,255) to white
                data[0] = 255;
                data[1] = 255;
                data[2] = 255;
            }
        }
    }
    wxBitmap bitmap(image_copy, 1);

    // find the most frequent color(s)
    wxImageHistogram histogram;
    image.ComputeHistogram(histogram);

    long colMostFreq = 0;
    unsigned long nMost = 0;
    long colNextMostFreq = 0;
    unsigned long nNext = 0;
    for ( wxImageHistogram::iterator entry = histogram.begin();
          entry != histogram.end();
          ++entry )
    {
        unsigned long key = entry->first;
        if ( !bHasMask || (key != keyMaskColor) )
        {
            unsigned long value = entry->second.value;
            if (value > nMost)
            {
                nNext = nMost;
                colNextMostFreq = colMostFreq;
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

    M_CURSORDATA->m_cursor = gdk_cursor_new_from_pixmap
                             (
                                bitmap.GetPixmap(),
                                mask,
                                fg.GetColor(), bg.GetColor(),
                                hotSpotX, hotSpotY
                             );

    g_object_unref (mask);
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

extern wxCursor g_globalCursor;

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

static void InternalIdle(const wxWindowList& list)
{
    wxWindowList::const_iterator i = list.begin();
    for (size_t n = list.size(); n--; ++i)
    {
        wxWindow* win = *i;
        win->OnInternalIdle();
        InternalIdle(win->GetChildren());
    }
}

void wxEndBusyCursor()
{
    if (--gs_busyCount > 0)
        return;

    g_globalCursor = gs_savedCursor;
    gs_savedCursor = wxNullCursor;
    InternalIdle(wxTopLevelWindows);
}

void wxBeginBusyCursor(const wxCursor* cursor)
{
    if (gs_busyCount++ > 0)
        return;

    wxASSERT_MSG( !gs_savedCursor.Ok(),
                  wxT("forgot to call wxEndBusyCursor, will leak memory") );

    gs_savedCursor = g_globalCursor;
    g_globalCursor = *cursor;
    InternalIdle(wxTopLevelWindows);
    gdk_flush();
}

bool wxIsBusy()
{
    return gs_busyCount > 0;
}

void wxSetCursor( const wxCursor& cursor )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    g_globalCursor = cursor;
}
