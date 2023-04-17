/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gdicmn.cpp
// Purpose:     Common GDI classes
// Author:      Julian Smart
// Modified by:
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

// Due to a bug mentioned in wx/hashmap.h we have to use aggregation here and
// define a simple accessor function below.
//
// FIXME-GCC-4.8: Remove this and just inherit from std::unordered_map<>.
class wxStringToColourHashMap
{
public:
    std::unordered_map<wxString, wxColour> m_colours;
};

namespace
{

inline std::unordered_map<wxString, wxColour>&
GetColours(wxStringToColourHashMap* map)
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

    static const struct wxColourDesc
    {
        const wxChar *name;
        unsigned char r,g,b;
    }
    wxColourTable[] =
    {
        {wxT("AQUAMARINE"),112, 219, 147},
        {wxT("BLACK"),0, 0, 0},
        {wxT("BLUE"), 0, 0, 255},
        {wxT("BLUE VIOLET"), 159, 95, 159},
        {wxT("BROWN"), 165, 42, 42},
        {wxT("CADET BLUE"), 95, 159, 159},
        {wxT("CORAL"), 255, 127, 0},
        {wxT("CORNFLOWER BLUE"), 66, 66, 111},
        {wxT("CYAN"), 0, 255, 255},
        {wxT("DARK GREY"), 47, 47, 47},   // ?

        {wxT("DARK GREEN"), 47, 79, 47},
        {wxT("DARK OLIVE GREEN"), 79, 79, 47},
        {wxT("DARK ORCHID"), 153, 50, 204},
        {wxT("DARK SLATE BLUE"), 107, 35, 142},
        {wxT("DARK SLATE GREY"), 47, 79, 79},
        {wxT("DARK TURQUOISE"), 112, 147, 219},
        {wxT("DIM GREY"), 84, 84, 84},
        {wxT("FIREBRICK"), 142, 35, 35},
        {wxT("FOREST GREEN"), 35, 142, 35},
        {wxT("GOLD"), 204, 127, 50},
        {wxT("GOLDENROD"), 219, 219, 112},
        {wxT("GREY"), 128, 128, 128},
        {wxT("GREEN"), 0, 255, 0},
        {wxT("GREEN YELLOW"), 147, 219, 112},
        {wxT("INDIAN RED"), 79, 47, 47},
        {wxT("KHAKI"), 159, 159, 95},
        {wxT("LIGHT BLUE"), 191, 216, 216},
        {wxT("LIGHT GREY"), 192, 192, 192},
        {wxT("LIGHT STEEL BLUE"), 143, 143, 188},
        {wxT("LIME GREEN"), 50, 204, 50},
        {wxT("LIGHT MAGENTA"), 255, 119, 255},
        {wxT("MAGENTA"), 255, 0, 255},
        {wxT("MAROON"), 142, 35, 107},
        {wxT("MEDIUM AQUAMARINE"), 50, 204, 153},
        {wxT("MEDIUM GREY"), 100, 100, 100},
        {wxT("MEDIUM BLUE"), 50, 50, 204},
        {wxT("MEDIUM FOREST GREEN"), 107, 142, 35},
        {wxT("MEDIUM GOLDENROD"), 234, 234, 173},
        {wxT("MEDIUM ORCHID"), 147, 112, 219},
        {wxT("MEDIUM SEA GREEN"), 66, 111, 66},
        {wxT("MEDIUM SLATE BLUE"), 127, 0, 255},
        {wxT("MEDIUM SPRING GREEN"), 127, 255, 0},
        {wxT("MEDIUM TURQUOISE"), 112, 219, 219},
        {wxT("MEDIUM VIOLET RED"), 219, 112, 147},
        {wxT("MIDNIGHT BLUE"), 47, 47, 79},
        {wxT("NAVY"), 35, 35, 142},
        {wxT("ORANGE"), 204, 50, 50},
        {wxT("ORANGE RED"), 255, 0, 127},
        {wxT("ORCHID"), 219, 112, 219},
        {wxT("PALE GREEN"), 143, 188, 143},
        {wxT("PINK"), 255, 192, 203},
        {wxT("PLUM"), 234, 173, 234},
        {wxT("PURPLE"), 176, 0, 255},
        {wxT("RED"), 255, 0, 0},
        {wxT("SALMON"), 111, 66, 66},
        {wxT("SEA GREEN"), 35, 142, 107},
        {wxT("SIENNA"), 142, 107, 35},
        {wxT("SKY BLUE"), 50, 153, 204},
        {wxT("SLATE BLUE"), 0, 127, 255},
        {wxT("SPRING GREEN"), 0, 255, 127},
        {wxT("STEEL BLUE"), 35, 107, 142},
        {wxT("TAN"), 219, 147, 112},
        {wxT("THISTLE"), 216, 191, 216},
        {wxT("TURQUOISE"), 173, 234, 234},
        {wxT("VIOLET"), 79, 47, 79},
        {wxT("VIOLET RED"), 204, 50, 153},
        {wxT("WHEAT"), 216, 216, 191},
        {wxT("WHITE"), 255, 255, 255},
        {wxT("YELLOW"), 255, 255, 0},
        {wxT("YELLOW GREEN"), 153, 204, 50}
    };

    size_t n;

    for ( n = 0; n < WXSIZEOF(wxColourTable); n++ )
    {
        const wxColourDesc& cc = wxColourTable[n];
        GetColours(m_map)[cc.name] = wxColour(cc.r, cc.g, cc.b);
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

    // ... and we also allow both grey/gray
    wxString colNameAlt = colName;
    if ( !colNameAlt.Replace(wxT("GRAY"), wxT("GREY")) )
    {
        // but in this case it is not necessary so avoid extra search below
        colNameAlt.clear();
    }

    auto& map = GetColours(m_map);
    auto it = map.find(colName);
    if ( it == map.end() && !colNameAlt.empty() )
        it = map.find(colNameAlt);
    if ( it != map.end() )
    {
        it->second = colour;
    }
    else // new colour
    {
        map[colName] = wxColour(colour);
    }
}

wxColour wxColourDatabase::Find(const wxString& colour) const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    // make the comparison case insensitive and also match both grey and gray
    wxString colName = colour;
    colName.MakeUpper();
    wxString colNameAlt = colName;
    if ( !colNameAlt.Replace(wxT("GRAY"), wxT("GREY")) )
        colNameAlt.clear();

    const auto& map = GetColours(m_map);
    auto it = map.find(colName);
    if ( it == map.end() && !colNameAlt.empty() )
        it = map.find(colNameAlt);
    if ( it != map.end() )
        return it->second;

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
            brush = new wxBrush(wxColour(wxT("GREY")), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_LIGHTGREY:
            brush = new wxBrush(*GetColour(COLOUR_LIGHTGREY), wxBRUSHSTYLE_SOLID);
            break;
        case BRUSH_MEDIUMGREY:
            brush = new wxBrush(wxColour(wxT("MEDIUM GREY")), wxBRUSHSTYLE_SOLID);
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
        case COLOUR_LIGHTGREY:
            colour = new wxColour(192, 192, 192);
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
            pen = new wxPen(wxColour(wxT("GREY")), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_LIGHTGREY:
            pen = new wxPen(*GetColour(COLOUR_LIGHTGREY), 1, wxPENSTYLE_SOLID);
            break;
        case PEN_MEDIUMGREY:
            pen = new wxPen(wxColour(wxT("MEDIUM GREY")), 1, wxPENSTYLE_SOLID);
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

    // wxTheColourDatabase is cleaned up by wxAppBase::CleanUp()
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
