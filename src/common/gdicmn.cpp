/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gdicmn.cpp
// Purpose:     Common GDI classes
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/gdicmn.h"

#include "wx/display.h"
#include "wx/gdiobj.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/iconbndl.h"
    #include "wx/cursor.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
    #include "wx/colour.h"
    #include "wx/font.h"
    #include "wx/math.h"
#endif

#include <unordered_map>

wxIMPLEMENT_ABSTRACT_CLASS(wxGDIObject, wxObject);


WXDLLIMPEXP_DATA_CORE(wxBrushList*) wxTheBrushList;
WXDLLIMPEXP_DATA_CORE(wxFontList*)  wxTheFontList;
WXDLLIMPEXP_DATA_CORE(wxPenList*)   wxThePenList;

WXDLLIMPEXP_DATA_CORE(wxColourDatabase*) wxTheColourDatabase;

WXDLLIMPEXP_DATA_CORE(wxBitmap)  wxNullBitmap;
WXDLLIMPEXP_DATA_CORE(wxBrush)   wxNullBrush;
WXDLLIMPEXP_DATA_CORE(wxColour)  wxNullColour;
WXDLLIMPEXP_DATA_CORE(wxCursor)  wxNullCursor;
WXDLLIMPEXP_DATA_CORE(wxFont)    wxNullFont;
WXDLLIMPEXP_DATA_CORE(wxIcon)    wxNullIcon;
WXDLLIMPEXP_DATA_CORE(wxPen)     wxNullPen;
#if wxUSE_PALETTE
WXDLLIMPEXP_DATA_CORE(wxPalette) wxNullPalette;
#endif
WXDLLIMPEXP_DATA_CORE(wxIconBundle) wxNullIconBundle;

const wxSize wxDefaultSize(wxDefaultCoord, wxDefaultCoord);
const wxPoint wxDefaultPosition(wxDefaultCoord, wxDefaultCoord);

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxPointList)

// If wxIcon is really a different class (which is currently only the case in
// wxMSW), this is done in its implementation file instead.
#ifdef wxICON_IS_BITMAP
wxIMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap);
#endif

#if wxUSE_EXTENDED_RTTI

// wxPoint

template<> void wxStringReadValue(const wxString &s , wxPoint &data )
{
    wxSscanf(s, wxT("%d,%d"), &data.x , &data.y ) ;
}

template<> void wxStringWriteValue(wxString &s , const wxPoint &data )
{
    s = wxString::Format(wxT("%d,%d"), data.x , data.y ) ;
}

wxCUSTOM_TYPE_INFO(wxPoint, wxToStringConverter<wxPoint> , wxFromStringConverter<wxPoint>)

template<> void wxStringReadValue(const wxString &s , wxSize &data )
{
    wxSscanf(s, wxT("%d,%d"), &data.x , &data.y ) ;
}

template<> void wxStringWriteValue(wxString &s , const wxSize &data )
{
    s = wxString::Format(wxT("%d,%d"), data.x , data.y ) ;
}

wxCUSTOM_TYPE_INFO(wxSize, wxToStringConverter<wxSize> , wxFromStringConverter<wxSize>)

#endif

wxRect::wxRect(const wxPoint& point1, const wxPoint& point2)
{
    x = point1.x;
    y = point1.y;
    width = point2.x - point1.x;
    height = point2.y - point1.y;

    if (width < 0)
    {
        width = -width;
        x = point2.x;
    }
    width++;

    if (height < 0)
    {
        height = -height;
        y = point2.y;
    }
    height++;
}

wxRect& wxRect::Union(const wxRect& rect)
{
    // ignore empty rectangles: union with an empty rectangle shouldn't extend
    // this one to (0, 0)
    if ( !width || !height )
    {
        *this = rect;
    }
    else if ( rect.width && rect.height )
    {
        int x1 = wxMin(x, rect.x);
        int y1 = wxMin(y, rect.y);
        int y2 = wxMax(y + height, rect.height + rect.y);
        int x2 = wxMax(x + width, rect.width + rect.x);

        x = x1;
        y = y1;
        width = x2 - x1;
        height = y2 - y1;
    }
    //else: we're not empty and rect is empty

    return *this;
}

wxRect& wxRect::Inflate(wxCoord dx, wxCoord dy)
{
     if (-2*dx>width)
     {
         // Don't allow deflate to eat more width than we have,
         // a well-defined rectangle cannot have negative width.
         x+=width/2;
         width=0;
     }
     else
     {
         // The inflate is valid.
         x-=dx;
         width+=2*dx;
     }

     if (-2*dy>height)
     {
         // Don't allow deflate to eat more height than we have,
         // a well-defined rectangle cannot have negative height.
         y+=height/2;
         height=0;
     }
     else
     {
         // The inflate is valid.
         y-=dy;
         height+=2*dy;
     }

    return *this;
}

bool wxRect::Contains(int cx, int cy) const
{
    return ( (cx >= x) && (cy >= y)
          && ((cy - y) < height)
          && ((cx - x) < width)
          );
}

bool wxRect::Contains(const wxRect& rect) const
{
    return Contains(rect.GetTopLeft()) && Contains(rect.GetBottomRight());
}

wxRect& wxRect::Intersect(const wxRect& rect)
{
    int x2 = GetRight(),
        y2 = GetBottom();

    if ( x < rect.x )
        x = rect.x;
    if ( y < rect.y )
        y = rect.y;
    if ( x2 > rect.GetRight() )
        x2 = rect.GetRight();
    if ( y2 > rect.GetBottom() )
        y2 = rect.GetBottom();

    width = x2 - x + 1;
    height = y2 - y + 1;

    if ( width <= 0 || height <= 0 )
    {
        width =
        height = 0;
    }

    return *this;
}

bool wxRect::Intersects(const wxRect& rect) const
{
    wxRect r = Intersect(rect);

    // if there is no intersection, both width and height are 0
    return r.width != 0;
}

wxRect& wxRect::operator+=(const wxRect& rect)
{
    *this = *this + rect;
    return *this;
}


wxRect& wxRect::operator*=(const wxRect& rect)
{
    *this = *this * rect;
    return *this;
}


wxRect operator+(const wxRect& r1, const wxRect& r2)
{
    int x1 = wxMin(r1.x, r2.x);
    int y1 = wxMin(r1.y, r2.y);
    int y2 = wxMax(r1.y+r1.height, r2.height+r2.y);
    int x2 = wxMax(r1.x+r1.width, r2.width+r2.x);
    return wxRect(x1, y1, x2-x1, y2-y1);
}

wxRect operator*(const wxRect& r1, const wxRect& r2)
{
    int x1 = wxMax(r1.x, r2.x);
    int y1 = wxMax(r1.y, r2.y);
    int y2 = wxMin(r1.y+r1.height, r2.height+r2.y);
    int x2 = wxMin(r1.x+r1.width, r2.width+r2.x);
    return wxRect(x1, y1, x2-x1, y2-y1);
}

wxRealPoint::wxRealPoint(const wxPoint& pt)
 : x(pt.x), y(pt.y)
{
}

// ============================================================================
// wxColourDatabase
// ============================================================================

namespace
{

struct wxColourDesc
{
    const char *name;
    unsigned char r,g,b;
};

// Instead of just creating the colour and storing it in wxColourDatabase, we
// create a struct which contains a yet uninitialized wxColour and the RGB
// values that can be used to initialize it later if it's really needed. This
// makes initialization faster (~15% in my tests) as we don't need to create
// all the colour objects that we're never going to use.
class wxColourWithRGB
{
public:
    // Set the RGB values but leave wxColour uninitialized.
    wxColourWithRGB& operator=(const wxColourDesc& cc)
    {
        r = cc.r;
        g = cc.g;
        b = cc.b;

        return *this;
    }

    // Initialize the wxColour immediately.
    wxColourWithRGB& operator=(const wxColour& c)
    {
        m_col = c;

        // We don't really need to set the RGB values here as they are only
        // used if the colour is not initialized but it seems cleaner to do it
        // as it doesn't have any real performance implications.
        r = c.Red();
        g = c.Green();
        b = c.Blue();

        return *this;
    }

    // Get the colour, creating it on demand.
    const wxColour& GetColour()
    {
        if ( !m_col.IsOk() )
            m_col.Set(r, g, b);

        return m_col;
    }

    bool operator==(const wxColour& c) const
    {
        return m_col.IsOk() ? m_col == c
                            : (c.Red() == r && c.Green() == g && c.Blue() == b);
    }

private:
    wxColour m_col;
    unsigned char r,g,b;
};

using wxColourMap = std::unordered_map<wxString, wxColourWithRGB>;

void AddColours(wxColourMap& map, const wxColourDesc* table, size_t len)
{
    for ( size_t n = 0; n < len; n++ )
    {
        const wxColourDesc& cc = table[n];
        map[wxString::FromAscii(cc.name)] = cc;
    }
}

} // anonymous namespace

// Due to a bug mentioned in wx/hashmap.h we have to use aggregation here and
// define a simple accessor function below.
//
// FIXME-GCC-4.8: Remove this and just inherit from std::unordered_map<>.
class wxStringToColourHashMap
{
public:
    wxColourMap m_colours;
};

namespace
{

inline wxColourMap& GetColours(wxStringToColourHashMap* map)
{
    return map->m_colours;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxColourDatabase ctor/dtor
// ----------------------------------------------------------------------------

wxColourDatabase::wxColourDatabase ()
{
    // will be created on demand in Initialize()
    m_map = nullptr;

    m_scheme = CSS;
}

wxColourDatabase::~wxColourDatabase ()
{
    delete m_map;
}

// Colour database stuff
void wxColourDatabase::Initialize()
{
    if ( m_map )
    {
        // already initialized
        return;
    }

    m_map = new wxStringToColourHashMap;

    static const wxColourDesc legacyColours[] =
    {
        {"AQUAMARINE",112, 219, 147},
        {"BLACK",0, 0, 0},
        {"BLUE", 0, 0, 255},
        {"BLUE VIOLET", 159, 95, 159},
        {"BROWN", 165, 42, 42},
        {"CADET BLUE", 95, 159, 159},
        {"CORAL", 255, 127, 0},
        {"CORNFLOWER BLUE", 66, 66, 111},
        {"CYAN", 0, 255, 255},
        {"DARK GRAY", 47, 47, 47},
        {"DARK GREY", 47, 47, 47},   // ?
        {"DARK GREEN", 47, 79, 47},
        {"DARK OLIVE GREEN", 79, 79, 47},
        {"DARK ORCHID", 153, 50, 204},
        {"DARK SLATE BLUE", 107, 35, 142},
        {"DARK SLATE GRAY", 47, 79, 79},
        {"DARK SLATE GREY", 47, 79, 79},
        {"DARK TURQUOISE", 112, 147, 219},
        {"DIM GRAY", 84, 84, 84},
        {"DIM GREY", 84, 84, 84},
        {"FIREBRICK", 142, 35, 35},
        {"FOREST GREEN", 35, 142, 35},
        {"GOLD", 204, 127, 50},
        {"GOLDENROD", 219, 219, 112},
        {"GRAY", 128, 128, 128},
        {"GREY", 128, 128, 128},
        {"GREEN", 0, 255, 0},
        {"GREEN YELLOW", 147, 219, 112},
        {"INDIAN RED", 79, 47, 47},
        {"KHAKI", 159, 159, 95},
        {"LIGHT BLUE", 191, 216, 216},
        {"LIGHT GRAY", 192, 192, 192},
        {"LIGHT GREY", 192, 192, 192},
        {"LIGHT STEEL BLUE", 143, 143, 188},
        {"LIME GREEN", 50, 204, 50},
        {"LIGHT MAGENTA", 255, 119, 255},
        {"MAGENTA", 255, 0, 255},
        {"MAROON", 142, 35, 107},
        {"MEDIUM AQUAMARINE", 50, 204, 153},
        {"MEDIUM GRAY", 100, 100, 100},
        {"MEDIUM GREY", 100, 100, 100},
        {"MEDIUM BLUE", 50, 50, 204},
        {"MEDIUM FOREST GREEN", 107, 142, 35},
        {"MEDIUM GOLDENROD", 234, 234, 173},
        {"MEDIUM ORCHID", 147, 112, 219},
        {"MEDIUM SEA GREEN", 66, 111, 66},
        {"MEDIUM SLATE BLUE", 127, 0, 255},
        {"MEDIUM SPRING GREEN", 127, 255, 0},
        {"MEDIUM TURQUOISE", 112, 219, 219},
        {"MEDIUM VIOLET RED", 219, 112, 147},
        {"MIDNIGHT BLUE", 47, 47, 79},
        {"NAVY", 35, 35, 142},
        {"ORANGE", 204, 50, 50},
        {"ORANGE RED", 255, 0, 127},
        {"ORCHID", 219, 112, 219},
        {"PALE GREEN", 143, 188, 143},
        {"PINK", 255, 192, 203},
        {"PLUM", 234, 173, 234},
        {"PURPLE", 176, 0, 255},
        {"RED", 255, 0, 0},
        {"SALMON", 111, 66, 66},
        {"SEA GREEN", 35, 142, 107},
        {"SIENNA", 142, 107, 35},
        {"SKY BLUE", 50, 153, 204},
        {"SLATE BLUE", 0, 127, 255},
        {"SPRING GREEN", 0, 255, 127},
        {"STEEL BLUE", 35, 107, 142},
        {"TAN", 219, 147, 112},
        {"THISTLE", 216, 191, 216},
        {"TURQUOISE", 173, 234, 234},
        {"VIOLET", 79, 47, 79},
        {"VIOLET RED", 204, 50, 153},
        {"WHEAT", 216, 216, 191},
        {"WHITE", 255, 255, 255},
        {"YELLOW", 255, 255, 0},
        {"YELLOW GREEN", 153, 204, 50}
    };

    // See https://www.w3.org/TR/css-color-4/#named-colors
    static const wxColourDesc cssColours[] =
    {
        { "ALICEBLUE", 240, 248, 255 }, // #f0f8ff
        { "ANTIQUEWHITE", 250, 235, 215 }, // #faebd7
        { "AQUA", 0, 255, 255 }, // #00ffff
        { "AQUAMARINE", 127, 255, 212 }, // #7fffd4
        { "AZURE", 240, 255, 255 }, // #f0ffff
        { "BEIGE", 245, 245, 220 }, // #f5f5dc
        { "BISQUE", 255, 228, 196 }, // #ffe4c4
        { "BLACK", 0, 0, 0 }, // #000000
        { "BLANCHEDALMOND", 255, 235, 205 }, // #ffebcd
        { "BLUE", 0, 0, 255 }, // #0000ff
        { "BLUEVIOLET", 138, 43, 226 }, // #8a2be2
        { "BROWN", 165, 42, 42 }, // #a52a2a
        { "BURLYWOOD", 222, 184, 135 }, // #deb887
        { "CADETBLUE", 95, 158, 160 }, // #5f9ea0
        { "CHARTREUSE", 127, 255, 0 }, // #7fff00
        { "CHOCOLATE", 210, 105, 30 }, // #d2691e
        { "CORAL", 255, 127, 80 }, // #ff7f50
        { "CORNFLOWERBLUE", 100, 149, 237 }, // #6495ed
        { "CORNSILK", 255, 248, 220 }, // #fff8dc
        { "CRIMSON", 220, 20, 60 }, // #dc143c
        { "CYAN", 0, 255, 255 }, // #00ffff
        { "DARKBLUE", 0, 0, 139 }, // #00008b
        { "DARKCYAN", 0, 139, 139 }, // #008b8b
        { "DARKGOLDENROD", 184, 134, 11 }, // #b8860b
        { "DARKGRAY", 169, 169, 169 }, // #a9a9a9
        { "DARKGREEN", 0, 100, 0 }, // #006400
        { "DARKGREY", 169, 169, 169 }, // #a9a9a9
        { "DARKKHAKI", 189, 183, 107 }, // #bdb76b
        { "DARKMAGENTA", 139, 0, 139 }, // #8b008b
        { "DARKOLIVEGREEN", 85, 107, 47 }, // #556b2f
        { "DARKORANGE", 255, 140, 0 }, // #ff8c00
        { "DARKORCHID", 153, 50, 204 }, // #9932cc
        { "DARKRED", 139, 0, 0 }, // #8b0000
        { "DARKSALMON", 233, 150, 122 }, // #e9967a
        { "DARKSEAGREEN", 143, 188, 143 }, // #8fbc8f
        { "DARKSLATEBLUE", 72, 61, 139 }, // #483d8b
        { "DARKSLATEGRAY", 47, 79, 79 }, // #2f4f4f
        { "DARKSLATEGREY", 47, 79, 79 }, // #2f4f4f
        { "DARKTURQUOISE", 0, 206, 209 }, // #00ced1
        { "DARKVIOLET", 148, 0, 211 }, // #9400d3
        { "DEEPPINK", 255, 20, 147 }, // #ff1493
        { "DEEPSKYBLUE", 0, 191, 255 }, // #00bfff
        { "DIMGRAY", 105, 105, 105 }, // #696969
        { "DIMGREY", 105, 105, 105 }, // #696969
        { "DODGERBLUE", 30, 144, 255 }, // #1e90ff
        { "FIREBRICK", 178, 34, 34 }, // #b22222
        { "FLORALWHITE", 255, 250, 240 }, // #fffaf0
        { "FORESTGREEN", 34, 139, 34 }, // #228b22
        { "FUCHSIA", 255, 0, 255 }, // #ff00ff
        { "GAINSBORO", 220, 220, 220 }, // #dcdcdc
        { "GHOSTWHITE", 248, 248, 255 }, // #f8f8ff
        { "GOLD", 255, 215, 0 }, // #ffd700
        { "GOLDENROD", 218, 165, 32 }, // #daa520
        { "GRAY", 128, 128, 128 }, // #808080
        { "GREEN", 0, 128, 0 }, // #008000
        { "GREENYELLOW", 173, 255, 47 }, // #adff2f
        { "GREY", 128, 128, 128 }, // #808080
        { "HONEYDEW", 240, 255, 240 }, // #f0fff0
        { "HOTPINK", 255, 105, 180 }, // #ff69b4
        { "INDIANRED", 205, 92, 92 }, // #cd5c5c
        { "INDIGO", 75, 0, 130 }, // #4b0082
        { "IVORY", 255, 255, 240 }, // #fffff0
        { "KHAKI", 240, 230, 140 }, // #f0e68c
        { "LAVENDER", 230, 230, 250 }, // #e6e6fa
        { "LAVENDERBLUSH", 255, 240, 245 }, // #fff0f5
        { "LAWNGREEN", 124, 252, 0 }, // #7cfc00
        { "LEMONCHIFFON", 255, 250, 205 }, // #fffacd
        { "LIGHTBLUE", 173, 216, 230 }, // #add8e6
        { "LIGHTCORAL", 240, 128, 128 }, // #f08080
        { "LIGHTCYAN", 224, 255, 255 }, // #e0ffff
        { "LIGHTGOLDENRODYELLOW", 250, 250, 210 }, // #fafad2
        { "LIGHTGRAY", 211, 211, 211 }, // #d3d3d3
        { "LIGHTGREEN", 144, 238, 144 }, // #90ee90
        { "LIGHTGREY", 211, 211, 211 }, // #d3d3d3
        { "LIGHTPINK", 255, 182, 193 }, // #ffb6c1
        { "LIGHTSALMON", 255, 160, 122 }, // #ffa07a
        { "LIGHTSEAGREEN", 32, 178, 170 }, // #20b2aa
        { "LIGHTSKYBLUE", 135, 206, 250 }, // #87cefa
        { "LIGHTSLATEGRAY", 119, 136, 153 }, // #778899
        { "LIGHTSLATEGREY", 119, 136, 153 }, // #778899
        { "LIGHTSTEELBLUE", 176, 196, 222 }, // #b0c4de
        { "LIGHTYELLOW", 255, 255, 224 }, // #ffffe0
        { "LIME", 0, 255, 0 }, // #00ff00
        { "LIMEGREEN", 50, 205, 50 }, // #32cd32
        { "LINEN", 250, 240, 230 }, // #faf0e6
        { "MAGENTA", 255, 0, 255 }, // #ff00ff
        { "MAROON", 128, 0, 0 }, // #800000
        { "MEDIUMAQUAMARINE", 102, 205, 170 }, // #66cdaa
        { "MEDIUMBLUE", 0, 0, 205 }, // #0000cd
        { "MEDIUMORCHID", 186, 85, 211 }, // #ba55d3
        { "MEDIUMPURPLE", 147, 112, 219 }, // #9370db
        { "MEDIUMSEAGREEN", 60, 179, 113 }, // #3cb371
        { "MEDIUMSLATEBLUE", 123, 104, 238 }, // #7b68ee
        { "MEDIUMSPRINGGREEN", 0, 250, 154 }, // #00fa9a
        { "MEDIUMTURQUOISE", 72, 209, 204 }, // #48d1cc
        { "MEDIUMVIOLETRED", 199, 21, 133 }, // #c71585
        { "MIDNIGHTBLUE", 25, 25, 112 }, // #191970
        { "MINTCREAM", 245, 255, 250 }, // #f5fffa
        { "MISTYROSE", 255, 228, 225 }, // #ffe4e1
        { "MOCCASIN", 255, 228, 181 }, // #ffe4b5
        { "NAVAJOWHITE", 255, 222, 173 }, // #ffdead
        { "NAVY", 0, 0, 128 }, // #000080
        { "OLDLACE", 253, 245, 230 }, // #fdf5e6
        { "OLIVE", 128, 128, 0 }, // #808000
        { "OLIVEDRAB", 107, 142, 35 }, // #6b8e23
        { "ORANGE", 255, 165, 0 }, // #ffa500
        { "ORANGERED", 255, 69, 0 }, // #ff4500
        { "ORCHID", 218, 112, 214 }, // #da70d6
        { "PALEGOLDENROD", 238, 232, 170 }, // #eee8aa
        { "PALEGREEN", 152, 251, 152 }, // #98fb98
        { "PALETURQUOISE", 175, 238, 238 }, // #afeeee
        { "PALEVIOLETRED", 219, 112, 147 }, // #db7093
        { "PAPAYAWHIP", 255, 239, 213 }, // #ffefd5
        { "PEACHPUFF", 255, 218, 185 }, // #ffdab9
        { "PERU", 205, 133, 63 }, // #cd853f
        { "PINK", 255, 192, 203 }, // #ffc0cb
        { "PLUM", 221, 160, 221 }, // #dda0dd
        { "POWDERBLUE", 176, 224, 230 }, // #b0e0e6
        { "PURPLE", 128, 0, 128 }, // #800080
        { "REBECCAPURPLE", 102, 51, 153 }, // #663399
        { "RED", 255, 0, 0 }, // #ff0000
        { "ROSYBROWN", 188, 143, 143 }, // #bc8f8f
        { "ROYALBLUE", 65, 105, 225 }, // #4169e1
        { "SADDLEBROWN", 139, 69, 19 }, // #8b4513
        { "SALMON", 250, 128, 114 }, // #fa8072
        { "SANDYBROWN", 244, 164, 96 }, // #f4a460
        { "SEAGREEN", 46, 139, 87 }, // #2e8b57
        { "SEASHELL", 255, 245, 238 }, // #fff5ee
        { "SIENNA", 160, 82, 45 }, // #a0522d
        { "SILVER", 192, 192, 192 }, // #c0c0c0
        { "SKYBLUE", 135, 206, 235 }, // #87ceeb
        { "SLATEBLUE", 106, 90, 205 }, // #6a5acd
        { "SLATEGRAY", 112, 128, 144 }, // #708090
        { "SLATEGREY", 112, 128, 144 }, // #708090
        { "SNOW", 255, 250, 250 }, // #fffafa
        { "SPRINGGREEN", 0, 255, 127 }, // #00ff7f
        { "STEELBLUE", 70, 130, 180 }, // #4682b4
        { "TAN", 210, 180, 140 }, // #d2b48c
        { "TEAL", 0, 128, 128 }, // #008080
        { "THISTLE", 216, 191, 216 }, // #d8bfd8
        { "TOMATO", 255, 99, 71 }, // #ff6347
        { "TURQUOISE", 64, 224, 208 }, // #40e0d0
        { "VIOLET", 238, 130, 238 }, // #ee82ee
        { "WHEAT", 245, 222, 179 }, // #f5deb3
        { "WHITE", 255, 255, 255 }, // #ffffff
        { "WHITESMOKE", 245, 245, 245 }, // #f5f5f5
        { "YELLOW", 255, 255, 0 }, // #ffff00
        { "YELLOWGREEN", 154, 205, 50 }, // #9acd32
    };

    auto& map = GetColours(m_map);

    // We still use the legacy colour names in CSS scheme, but we add them
    // first so any conflicting values are overwritten with the correct values
    // in the CSS table. Similarly, we provide CSS colour names even in the
    // traditional scheme -- but legacy colour values take precedence for the
    // colours that used to be defined by wxWidgets.
    switch ( m_scheme )
    {
        case CSS:
            AddColours(map, legacyColours, WXSIZEOF(legacyColours));
            AddColours(map, cssColours, WXSIZEOF(cssColours));
            break;

        case Traditional:
            AddColours(map, cssColours, WXSIZEOF(cssColours));
            AddColours(map, legacyColours, WXSIZEOF(legacyColours));
            break;
    }
}

void wxColourDatabase::UseScheme(Scheme scheme)
{
    if ( scheme == m_scheme )
        return;

    m_scheme = scheme;

    // Reset the existing map for the different scheme, it will be re-filled on
    // next use.
    if ( m_map )
    {
        delete m_map;
        m_map = nullptr;
    }
}

// ----------------------------------------------------------------------------
// wxColourDatabase operations
// ----------------------------------------------------------------------------

void wxColourDatabase::AddColour(const wxString& name, const wxColour& colour)
{
    Initialize();

    // canonicalize the colour names before using them as keys: they should be
    // in upper case
    wxString colName = name;
    colName.MakeUpper();

    GetColours(m_map)[colName] = colour;
}

wxColour wxColourDatabase::Find(const wxString& colour) const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    // make the comparison case insensitive
    wxString colName = colour;
    colName.MakeUpper();

    auto& map = GetColours(m_map);
    auto it = map.find(colName);
    if ( it != map.end() )
        return it->second.GetColour();

    // we did not find any result in existing colours:
    // we won't use wxString -> wxColour conversion because the
    // wxColour::Set(const wxString &) function which does that conversion
    // internally uses this function (wxColourDatabase::Find) and we want
    // to avoid infinite recursion !
    return wxNullColour;
}

wxString wxColourDatabase::FindName(const wxColour& colour) const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    for ( const auto& kv : GetColours(m_map) )
    {
        if ( kv.second == colour )
            return kv.first;
    }

    return wxEmptyString;
}

wxVector<wxString> wxColourDatabase::GetAllNames() const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    const auto& map = GetColours(m_map);

    wxVector<wxString> names;
    names.reserve(map.size());

    for ( const auto& kv : map )
        names.push_back(kv.first);

    return names;
}

// ============================================================================
// stock objects
// ============================================================================

static wxStockGDI gs_wxStockGDI_instance;
wxStockGDI* wxStockGDI::ms_instance = &gs_wxStockGDI_instance;
wxObject* wxStockGDI::ms_stockObject[ITEMCOUNT];

wxStockGDI::wxStockGDI()
{
}

wxStockGDI::~wxStockGDI()
{
}

void wxStockGDI::DeleteAll()
{
    for (unsigned i = 0; i < ITEMCOUNT; i++)
    {
        wxDELETE(ms_stockObject[i]);
    }
}

const wxBrush* wxStockGDI::GetBrush(Item item)
{
    wxBrush* brush = static_cast<wxBrush*>(ms_stockObject[item]);
    if (brush == nullptr)
    {
        switch (item)
        {
        case BRUSH_BLACK:
            brush = new wxBrush(*GetColour(COLOUR_BLACK), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_BLUE:
            brush = new wxBrush(*GetColour(COLOUR_BLUE), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_CYAN:
            brush = new wxBrush(*GetColour(COLOUR_CYAN), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_GREEN:
            brush = new wxBrush(*GetColour(COLOUR_GREEN), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_YELLOW:
            brush = new wxBrush(*GetColour(COLOUR_YELLOW), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_GREY:
            brush = new wxBrush(*GetColour(COLOUR_GREY), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_LIGHTGREY:
            brush = new wxBrush(*GetColour(COLOUR_LIGHTGREY), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_MEDIUMGREY:
            brush = new wxBrush(*GetColour(COLOUR_MEDIUMGREY), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_RED:
            brush = new wxBrush(*GetColour(COLOUR_RED), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_TRANSPARENT:
            brush = new wxBrush(*GetColour(COLOUR_BLACK), wxBRUSHSTYLE_TRANSPARENT);
            break;
        case BRUSH_WHITE:
            brush = new wxBrush(*GetColour(COLOUR_WHITE), wxBRUSHSTYLE_SOLID);
            break;
        default:
            wxFAIL;
        }
        ms_stockObject[item] = brush;
    }
    return brush;
}

const wxColour* wxStockGDI::GetColour(Item item)
{
    wxColour* colour = static_cast<wxColour*>(ms_stockObject[item]);
    if (colour == nullptr)
    {
        switch (item)
        {
        case COLOUR_BLACK:
            colour = new wxColour(0, 0, 0);
            break;
        case COLOUR_BLUE:
            colour = new wxColour(0, 0, 255);
            break;
        case COLOUR_CYAN:
            colour = new wxColour(0, 255, 255);
            break;
        case COLOUR_GREEN:
            colour = new wxColour(0, 255, 0);
            break;
        case COLOUR_YELLOW:
            colour = new wxColour(255, 255, 0);
            break;
        case COLOUR_GREY:
            colour = new wxColour(128, 128, 128);
            break;
        case COLOUR_LIGHTGREY:
            colour = new wxColour(192, 192, 192);
            break;
        case COLOUR_MEDIUMGREY:
            colour = new wxColour(100, 100, 100);
            break;
        case COLOUR_RED:
            colour = new wxColour(255, 0, 0);
            break;
        case COLOUR_WHITE:
            colour = new wxColour(255, 255, 255);
            break;
        default:
            wxFAIL;
        }
        ms_stockObject[item] = colour;
    }
    return colour;
}

const wxCursor* wxStockGDI::GetCursor(Item item)
{
    wxCursor* cursor = static_cast<wxCursor*>(ms_stockObject[item]);
    if (cursor == nullptr)
    {
        switch (item)
        {
        case CURSOR_CROSS:
            cursor = new wxCursor(wxCURSOR_CROSS);
            break;
        case CURSOR_HOURGLASS:
            cursor = new wxCursor(wxCURSOR_WAIT);
            break;
        case CURSOR_STANDARD:
            cursor = new wxCursor(wxCURSOR_ARROW);
            break;
        default:
            wxFAIL;
        }
        ms_stockObject[item] = cursor;
    }
    return cursor;
}

const wxFont* wxStockGDI::GetFont(Item item)
{
    wxFont* font = static_cast<wxFont*>(ms_stockObject[item]);
    if (font == nullptr)
    {
        switch (item)
        {
        case FONT_ITALIC:
            font = new wxFont(GetFont(FONT_NORMAL)->GetPointSize(),
                              wxFONTFAMILY_ROMAN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL);
            break;
        case FONT_NORMAL:
            font = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
            break;
        case FONT_SMALL:
            font = new wxFont(GetFont(FONT_NORMAL)->GetPointSize()
                    // Using the font 2 points smaller than the normal one
                    // results in font so small as to be unreadable under MSW.
                    // We might want to actually use -1 under the other
                    // platforms too but for now be conservative and keep -2
                    // there for compatibility with the old behaviour as the
                    // small font seems to be readable enough there as it is.
#ifdef __WXMSW__
                    - 1,
#else
                    - 2,
#endif
                    wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
            break;
        case FONT_SWISS:
            font = new wxFont(GetFont(FONT_NORMAL)->GetPointSize(),
                              wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
            break;
        default:
            wxFAIL;
        }
        ms_stockObject[item] = font;
    }
    return font;
}

const wxPen* wxStockGDI::GetPen(Item item)
{
    wxPen* pen = static_cast<wxPen*>(ms_stockObject[item]);
    if (pen == nullptr)
    {
        switch (item)
        {
        case PEN_BLACK:
            pen = new wxPen(*GetColour(COLOUR_BLACK), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_BLACKDASHED:
            pen = new wxPen(*GetColour(COLOUR_BLACK), 1, wxPENSTYLE_SHORT_DASH);
            break;
        case PEN_BLUE:
            pen = new wxPen(*GetColour(COLOUR_BLUE), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_CYAN:
            pen = new wxPen(*GetColour(COLOUR_CYAN), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_GREEN:
            pen = new wxPen(*GetColour(COLOUR_GREEN), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_YELLOW:
            pen = new wxPen(*GetColour(COLOUR_YELLOW), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_GREY:
            pen = new wxPen(*GetColour(COLOUR_GREY), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_LIGHTGREY:
            pen = new wxPen(*GetColour(COLOUR_LIGHTGREY), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_MEDIUMGREY:
            pen = new wxPen(*GetColour(COLOUR_MEDIUMGREY), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_RED:
            pen = new wxPen(*GetColour(COLOUR_RED), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_TRANSPARENT:
            pen = new wxPen(*GetColour(COLOUR_BLACK), 1, wxPENSTYLE_TRANSPARENT);
            break;
        case PEN_WHITE:
            pen = new wxPen(*GetColour(COLOUR_WHITE), 1, wxPENSTYLE_SOLID);
            break;
        default:
            wxFAIL;
        }
        ms_stockObject[item] = pen;
    }
    return pen;
}

void wxInitializeStockLists()
{
    wxTheColourDatabase = new wxColourDatabase;

    wxTheBrushList = new wxBrushList;
    wxThePenList = new wxPenList;
    wxTheFontList = new wxFontList;
}

void wxDeleteStockLists()
{
    wxDELETE(wxTheBrushList);
    wxDELETE(wxThePenList);
    wxDELETE(wxTheFontList);

    wxDELETE(wxTheColourDatabase);
}

// ============================================================================
// wxTheXXXList stuff (semi-obsolete)
// ============================================================================

wxGDIObjListBase::wxGDIObjListBase()
{
}

wxGDIObjListBase::~wxGDIObjListBase()
{
    for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
    {
        delete static_cast<wxObject*>(node->GetData());
    }
}

wxPen *wxPenList::FindOrCreatePen (const wxColour& colour, int width, wxPenStyle style)
{
    for ( wxList::compatibility_iterator node = list.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxPen * const pen = (wxPen *) node->GetData();
        if ( pen->GetWidth () == width &&
                pen->GetStyle () == style &&
                    pen->GetColour() == colour )
            return pen;
    }

    wxPen* pen = nullptr;
    wxPen penTmp(colour, width, style);
    if (penTmp.IsOk())
    {
        pen = new wxPen(penTmp);
        list.Append(pen);
    }

    return pen;
}

wxBrush *wxBrushList::FindOrCreateBrush (const wxColour& colour, wxBrushStyle style)
{
    for ( wxList::compatibility_iterator node = list.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxBrush * const brush = (wxBrush *) node->GetData ();
        if ( brush->GetStyle() == style && brush->GetColour() == colour )
            return brush;
    }

    wxBrush* brush = nullptr;
    wxBrush brushTmp(colour, style);
    if (brushTmp.IsOk())
    {
        brush = new wxBrush(brushTmp);
        list.Append(brush);
    }

    return brush;
}

wxFont *wxFontList::FindOrCreateFont(const wxFontInfo& fontInfo)
{
    // info is fontInfo adjusted for platform oddities
    wxFontInfo info(fontInfo);

    // In all ports but wxOSX, the effective family of a font created using
    // wxFONTFAMILY_DEFAULT is wxFONTFAMILY_SWISS so this is what we need to
    // use for comparison.
    //
    // wxOSX is handled specifically below, see there.
    if ( info.GetFamily() == wxFONTFAMILY_DEFAULT )
        info.Family(wxFONTFAMILY_SWISS);

    // In wxMSW, creating a font with wxFONTSTYLE_SLANT creates the same font
    // as wxFONTSTYLE_ITALIC and its GetStyle() returns the latter, so we must
    // account for it here. Notice that wxOSX also uses the same native font
    // for these styles, but wxFont::GetStyle() in it still returns different
    // values depending on how the font was created, so there is inconsistency
    // between ports here which it would be nice to fix in one way or another
    // (wxGTK supports both as separate styles, so it doesn't suffer from it).
 #ifdef __WXMSW__
    if ( info.GetStyle() == wxFONTSTYLE_SLANT )
        info.Style(wxFONTSTYLE_ITALIC);
 #endif // __WXMSW__

    wxFont *font;
    wxList::compatibility_iterator node;
    for (node = list.GetFirst(); node; node = node->GetNext())
    {
        bool same;

        font = (wxFont *)node->GetData();

        if ( info.IsUsingSizeInPixels() )
        {
            // When the width is 0, it means that we don't care about it.
            if ( info.GetPixelSize().x == 0 )
                same = font->GetPixelSize().y == info.GetPixelSize().y;
            else
                same = font->GetPixelSize() == info.GetPixelSize();
        }
        else
        {
            same = font->GetFractionalPointSize() == info.GetFractionalPointSize();
        }

        if ( same &&
             font->GetStyle () == info.GetStyle() &&
             font->GetWeight () == info.GetWeight() &&
             font->GetUnderlined () == info.IsUnderlined() )
        {
            // empty facename matches anything at all: this is bad because
            // depending on which fonts are already created, we might get back
            // a different font if we create it with empty facename, but it is
            // still better than never matching anything in the cache at all
            // in this case
            const wxString fontFaceName(font->GetFaceName());

            if (info.GetFaceName().empty() || fontFaceName.empty())
            {
                same = font->GetFamily() == info.GetFamily();

                // In wxOSX fonts created using wxFONTFAMILY_DEFAULT can return
                // either it or wxFONTFAMILY_SWISS from GetFamily(), which is a
                // bug and needs to be fixed (see #23144), but for now work
                // around it here.
#ifdef __WXOSX__
                if ( !same &&
                     fontInfo.GetFamily() == wxFONTFAMILY_DEFAULT &&
                     font->GetFamily() == wxFONTFAMILY_DEFAULT )
                {
                    same = true;
                }
#endif // __WXOSX__
            }
            else
            {
                same = fontFaceName == info.GetFaceName();
            }

            if ( same && (info.GetEncoding() != wxFONTENCODING_DEFAULT) )
            {
                // have to match the encoding too
                same = font->GetEncoding() == info.GetEncoding();
            }

            if ( same )
            {
                return font;
            }
        }
    }

    // font not found, create the new one
    font = new wxFont(info);
    if (font->IsOk())
    {
        list.Append(font);
    }
    else
    {
        delete font;
        font = nullptr;
    }

    return font;
}

int wxDisplayDepth()
{
    return wxDisplay().GetDepth();
}

bool wxColourDisplay()
{
    // If GetDepth() returns 0, meaning unknown, we assume it's a colour
    // display, hence the use of "!=" rather than ">" here.
    return wxDisplay().GetDepth() != 1;
}

void wxDisplaySize(int *width, int *height)
{
    const wxSize size = wxGetDisplaySize();
    if ( width )
        *width = size.x;
    if ( height )
        *height = size.y;
}

wxSize wxGetDisplaySize()
{
    return wxDisplay().GetGeometry().GetSize();
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    const wxRect rect = wxGetClientDisplayRect();
    if ( x )
        *x = rect.x;
    if ( y )
        *y = rect.y;
    if ( width )
        *width = rect.width;
    if ( height )
        *height = rect.height;
}

wxRect wxGetClientDisplayRect()
{
    return wxDisplay().GetClientArea();
}

void wxDisplaySizeMM(int *width, int *height)
{
    const wxSize size = wxGetDisplaySizeMM();
    if ( width )
        *width = size.x;
    if ( height )
        *height = size.y;
}

wxSize wxGetDisplaySizeMM()
{
    const wxSize ppi = wxGetDisplayPPI();
    if ( !ppi.x || !ppi.y )
        return wxSize(0, 0);

    const wxSize pixels = wxGetDisplaySize();
    return wxSize(wxRound(pixels.x * inches2mm / ppi.x),
                  wxRound(pixels.y * inches2mm / ppi.y));
}

wxSize wxGetDisplayPPI()
{
    return wxDisplay().GetPPI();
}
