/////////////////////////////////////////////////////////////////////////////
// Name:        gdicmn.cpp
// Purpose:     Common GDI classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "gdicmn.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/brush.h"
#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/font.h"
#include "wx/palette.h"
#include "wx/app.h"
#include "wx/dc.h"
#include "wx/utils.h"
#include "wx/settings.h"
#include "wx/hashmap.h"

#include "wx/log.h"
#include <string.h>

#if defined(__WXMSW__) && !defined(__PALMOS__)
#include "wx/msw/wrapwin.h"
#endif

#ifdef __WXMOTIF__
#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif
#endif

#ifdef __WXX11__
#include "X11/Xlib.h"
#endif

#ifdef __WXMAC__
#include "wx/mac/private.h"
#include "wx/mac/uma.h"
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

IMPLEMENT_ABSTRACT_CLASS(wxDCBase, wxObject)

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

bool wxRect::operator==(const wxRect& rect) const
{
    return ((x == rect.x) &&
            (y == rect.y) &&
            (width == rect.width) &&
            (height == rect.height));
}

wxRect wxRect::operator+(const wxRect& rect) const
{
    int x1 = wxMin(this->x, rect.x);
    int y1 = wxMin(this->y, rect.y);
    int y2 = wxMax(y+height, rect.height+rect.y);
    int x2 = wxMax(x+width, rect.width+rect.x);
    return wxRect(x1, y1, x2-x1, y2-y1);
}

wxRect& wxRect::Union(const wxRect& rect)
{
    // ignore empty rectangles
    if ( rect.width && rect.height )
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

    return *this;
}

wxRect& wxRect::Inflate(wxCoord dx, wxCoord dy)
{
    x -= dx;
    y -= dy;
    width += 2*dx;
    height += 2*dy;

    // check that we didn't make the rectangle invalid by accident (you almost
    // never want to have negative coords and never want negative size)
    if ( x < 0 )
        x = 0;
    if ( y < 0 )
        y = 0;

    // what else can we do?
    if ( width < 0 )
        width = 0;
    if ( height < 0 )
        height = 0;

    return *this;
}

bool wxRect::Inside(int cx, int cy) const
{
    return ( (cx >= x) && (cy >= y)
          && ((cy - y) < height)
          && ((cx - x) < width)
          );
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

// ============================================================================
// wxColourDatabase
// ============================================================================

// ----------------------------------------------------------------------------
// wxColourDatabase ctor/dtor
// ----------------------------------------------------------------------------

wxColourDatabase::wxColourDatabase ()
{
    // will be created on demand in Initialize()
    m_map = NULL;
}

wxColourDatabase::~wxColourDatabase ()
{
    if ( m_map )
    {
        WX_CLEAR_HASH_MAP(wxStringToColourHashMap, *m_map);

        delete m_map;
    }

#ifdef __WXPM__
    delete [] m_palTable;
#endif
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
        {wxT("LIGHT MAGENTA"), 255, 0, 255},
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
        {wxT("PINK"), 188, 143, 234},
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
        (*m_map)[cc.name] = new wxColour(cc.r, cc.g, cc.b);
    }

#ifdef __WXPM__
    m_palTable = new long[n];
    for ( n = 0; n < WXSIZEOF(wxColourTable); n++ )
    {
        const wxColourDesc& cc = wxColourTable[n];
        m_palTable[n] = OS2RGB(cc.r,cc.g,cc.b);
    }
    m_nSize = n;
#endif
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
    if ( !colNameAlt.Replace(_T("GRAY"), _T("GREY")) )
    {
        // but in this case it is not necessary so avoid extra search below
        colNameAlt.clear();
    }

    wxStringToColourHashMap::iterator it = m_map->find(colName);
    if ( it == m_map->end() && !colNameAlt.empty() )
        it = m_map->find(colNameAlt);
    if ( it != m_map->end() )
    {
        *(it->second) = colour;
    }
    else // new colour
    {
        (*m_map)[colName] = new wxColour(colour);
    }
}

wxColour wxColourDatabase::Find(const wxString& colour) const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    // first look among the existing colours

    // make the comparaison case insensitive and also match both grey and gray
    wxString colName = colour;
    colName.MakeUpper();
    wxString colNameAlt = colName;
    if ( !colNameAlt.Replace(_T("GRAY"), _T("GREY")) )
        colNameAlt.clear();

    wxStringToColourHashMap::iterator it = m_map->find(colName);
    if ( it == m_map->end() && !colNameAlt.empty() )
        it = m_map->find(colNameAlt);
    if ( it != m_map->end() )
        return *(it->second);

    // if we didn't find it, query the system, maybe it knows about it
#if defined(__WXGTK__) || defined(__X__)
    wxColour col = wxColour::CreateByName(colour);

    if ( col.Ok() )
    {
        // cache it
        self->AddColour(colour, col);
    }

    return col;
#elif defined(__X__)
    // TODO: move this to wxColour::CreateByName()
    XColor xcolour;

#ifdef __WXMOTIF__
    Display *display = XtDisplay((Widget) wxTheApp->GetTopLevelWidget()) ;
#endif
#ifdef __WXX11__
    Display* display = (Display*) wxGetDisplay();
#endif
    /* MATTHEW: [4] Use wxGetMainColormap */
    if (!XParseColor(display, (Colormap) wxTheApp->GetMainColormap((WXDisplay*) display), colour.ToAscii() ,&xcolour))
        return NULL;

#if wxUSE_NANOX
    unsigned char r = (unsigned char)(xcolour.red);
    unsigned char g = (unsigned char)(xcolour.green);
    unsigned char b = (unsigned char)(xcolour.blue);
#else
    unsigned char r = (unsigned char)(xcolour.red >> 8);
    unsigned char g = (unsigned char)(xcolour.green >> 8);
    unsigned char b = (unsigned char)(xcolour.blue >> 8);
#endif

    wxColour col(r, g, b);
    AddColour(colour, col);

    return col;
#else // other platform
    return wxNullColour;
#endif // platforms
}

wxString wxColourDatabase::FindName(const wxColour& colour) const
{
    wxColourDatabase * const self = wxConstCast(this, wxColourDatabase);
    self->Initialize();

    typedef wxStringToColourHashMap::iterator iterator;

    for ( iterator it = m_map->begin(), en = m_map->end(); it != en; ++it )
    {
        if ( *(it->second) == colour )
            return it->first;
    }

    return wxEmptyString;
}

// ----------------------------------------------------------------------------
// deprecated wxColourDatabase methods
// ----------------------------------------------------------------------------

wxColour *wxColourDatabase::FindColour(const wxString& name)
{
    // This function is deprecated, use Find() instead.
    // Formerly this function sometimes would return a deletable pointer and
    // sometimes a non-deletable one (when returning a colour from the database).
    // Trying to delete the latter anyway results in problems, so probably
    // nobody ever freed the pointers. Currently it always returns a new
    // instance, which means there will be memory leaks.
    wxLogDebug(wxT("wxColourDataBase::FindColour():")
        wxT(" Please use wxColourDataBase::Find() instead"));

    // using a static variable here is not the most elegant solution but unless
    // we want to make wxStringToColourHashMap public (i.e. move it to the
    // header) so that we could have a member function returning
    // wxStringToColourHashMap::iterator, there is really no good way to do it
    // otherwise
    //
    // and knowing that this function is going to disappear in the next release
    // anyhow I don't want to waste time on this

    static wxColour s_col;

    s_col = Find(name);
    if ( !s_col.Ok() )
        return NULL;

    return new wxColour(s_col);
}

// ============================================================================
// stock objects
// ============================================================================

void wxInitializeStockLists()
{
    wxTheColourDatabase = new wxColourDatabase;

    wxTheBrushList = new wxBrushList;
    wxThePenList = new wxPenList;
    wxTheFontList = new wxFontList;
    wxTheBitmapList = new wxBitmapList;
}

void wxInitializeStockObjects ()
{
#ifdef __WXMOTIF__
#endif
#ifdef __X__
  // TODO
  //  wxFontPool = new XFontPool;
#endif

  // why under MSW fonts shouldn't have the standard system size?
/*
#ifdef __WXMSW__
  static const int sizeFont = 10;
#else
#endif
*/
#if defined(__WXMAC__)
    // retrieve size of system font for all stock fonts
    int sizeFont = 12;

    Str255 fontName ;
    SInt16 fontSize ;
    Style fontStyle ;

    GetThemeFont(kThemeSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
    sizeFont = fontSize ;
#ifdef __WXMAC_CLASSIC__
    wxNORMAL_FONT = new wxFont (fontSize, wxMODERN, wxNORMAL, wxNORMAL , false , wxMacMakeStringFromPascal(fontName) );
#else
    wxNORMAL_FONT = new wxFont () ;
    wxNORMAL_FONT->MacCreateThemeFont( kThemeSystemFont );
#endif
#elif defined(__WXPM__)
    static const int sizeFont = 12;
#else
    wxNORMAL_FONT = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    static const int sizeFont = wxNORMAL_FONT->GetPointSize();
#endif

#if defined(__WXPM__)
    /*
    // Basic OS/2 has a fairly limited number of fonts and these are as good
    // as I can do to get something that looks halfway "wx" normal
    */
    wxNORMAL_FONT = new wxFont (sizeFont, wxMODERN, wxNORMAL, wxBOLD);
    wxSMALL_FONT = new wxFont (sizeFont - 4, wxSWISS, wxNORMAL, wxNORMAL); /* Helv */
    wxITALIC_FONT = new wxFont (sizeFont, wxROMAN, wxITALIC, wxNORMAL);
    wxSWISS_FONT = new wxFont (sizeFont, wxSWISS, wxNORMAL, wxNORMAL); /* Helv */
#elif defined(__WXMAC__)
    wxSWISS_FONT = new wxFont (sizeFont, wxSWISS, wxNORMAL, wxNORMAL); /* Helv */
    wxITALIC_FONT = new wxFont (sizeFont, wxROMAN, wxITALIC, wxNORMAL);
#ifdef __WXMAC_CLASSIC__
  GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
    wxSMALL_FONT = new wxFont (fontSize, wxSWISS, wxNORMAL, wxNORMAL , false , wxMacMakeStringFromPascal( fontName ) );
#else
    wxSMALL_FONT = new wxFont () ;
    wxSMALL_FONT->MacCreateThemeFont( kThemeSmallSystemFont );
#endif
#else
    wxSMALL_FONT = new wxFont (sizeFont - 2, wxSWISS, wxNORMAL, wxNORMAL);
    wxITALIC_FONT = new wxFont (sizeFont, wxROMAN, wxITALIC, wxNORMAL);
    wxSWISS_FONT = new wxFont (sizeFont, wxSWISS, wxNORMAL, wxNORMAL);
#endif

    wxRED_PEN = new wxPen (wxT("RED"), 1, wxSOLID);
    wxCYAN_PEN = new wxPen (wxT("CYAN"), 1, wxSOLID);
    wxGREEN_PEN = new wxPen (wxT("GREEN"), 1, wxSOLID);
    wxBLACK_PEN = new wxPen (wxT("BLACK"), 1, wxSOLID);
    wxWHITE_PEN = new wxPen (wxT("WHITE"), 1, wxSOLID);
    wxTRANSPARENT_PEN = new wxPen (wxT("BLACK"), 1, wxTRANSPARENT);
    wxBLACK_DASHED_PEN = new wxPen (wxT("BLACK"), 1, wxSHORT_DASH);
    wxGREY_PEN = new wxPen (wxT("GREY"), 1, wxSOLID);
    wxMEDIUM_GREY_PEN = new wxPen (wxT("MEDIUM GREY"), 1, wxSOLID);
    wxLIGHT_GREY_PEN = new wxPen (wxT("LIGHT GREY"), 1, wxSOLID);

    wxBLUE_BRUSH = new wxBrush (wxT("BLUE"), wxSOLID);
    wxGREEN_BRUSH = new wxBrush (wxT("GREEN"), wxSOLID);
    wxWHITE_BRUSH = new wxBrush (wxT("WHITE"), wxSOLID);
    wxBLACK_BRUSH = new wxBrush (wxT("BLACK"), wxSOLID);
    wxTRANSPARENT_BRUSH = new wxBrush (wxT("BLACK"), wxTRANSPARENT);
    wxCYAN_BRUSH = new wxBrush (wxT("CYAN"), wxSOLID);
    wxRED_BRUSH = new wxBrush (wxT("RED"), wxSOLID);
    wxGREY_BRUSH = new wxBrush (wxT("GREY"), wxSOLID);
    wxMEDIUM_GREY_BRUSH = new wxBrush (wxT("MEDIUM GREY"), wxSOLID);
    wxLIGHT_GREY_BRUSH = new wxBrush (wxT("LIGHT GREY"), wxSOLID);

    wxBLACK = new wxColour (wxT("BLACK"));
    wxWHITE = new wxColour (wxT("WHITE"));
    wxRED = new wxColour (wxT("RED"));
    wxBLUE = new wxColour (wxT("BLUE"));
    wxGREEN = new wxColour (wxT("GREEN"));
    wxCYAN = new wxColour (wxT("CYAN"));
    wxLIGHT_GREY = new wxColour (wxT("LIGHT GREY"));

    wxSTANDARD_CURSOR = new wxCursor (wxCURSOR_ARROW);
    wxHOURGLASS_CURSOR = new wxCursor (wxCURSOR_WAIT);
    wxCROSS_CURSOR = new wxCursor (wxCURSOR_CROSS);
}

void wxDeleteStockObjects ()
{
    wxDELETE(wxNORMAL_FONT);
    wxDELETE(wxSMALL_FONT);
    wxDELETE(wxITALIC_FONT);
    wxDELETE(wxSWISS_FONT);

    wxDELETE(wxRED_PEN);
    wxDELETE(wxCYAN_PEN);
    wxDELETE(wxGREEN_PEN);
    wxDELETE(wxBLACK_PEN);
    wxDELETE(wxWHITE_PEN);
    wxDELETE(wxTRANSPARENT_PEN);
    wxDELETE(wxBLACK_DASHED_PEN);
    wxDELETE(wxGREY_PEN);
    wxDELETE(wxMEDIUM_GREY_PEN);
    wxDELETE(wxLIGHT_GREY_PEN);

    wxDELETE(wxBLUE_BRUSH);
    wxDELETE(wxGREEN_BRUSH);
    wxDELETE(wxWHITE_BRUSH);
    wxDELETE(wxBLACK_BRUSH);
    wxDELETE(wxTRANSPARENT_BRUSH);
    wxDELETE(wxCYAN_BRUSH);
    wxDELETE(wxRED_BRUSH);
    wxDELETE(wxGREY_BRUSH);
    wxDELETE(wxMEDIUM_GREY_BRUSH);
    wxDELETE(wxLIGHT_GREY_BRUSH);

    wxDELETE(wxBLACK);
    wxDELETE(wxWHITE);
    wxDELETE(wxRED);
    wxDELETE(wxBLUE);
    wxDELETE(wxGREEN);
    wxDELETE(wxCYAN);
    wxDELETE(wxLIGHT_GREY);

    wxDELETE(wxSTANDARD_CURSOR);
    wxDELETE(wxHOURGLASS_CURSOR);
    wxDELETE(wxCROSS_CURSOR);
}

void wxDeleteStockLists()
{
    wxDELETE(wxTheBrushList);
    wxDELETE(wxThePenList);
    wxDELETE(wxTheFontList);
    wxDELETE(wxTheBitmapList);
}

// ============================================================================
// wxTheXXXList stuff (semi-obsolete)
// ============================================================================

wxBitmapList::~wxBitmapList ()
{
    wxList::compatibility_iterator node = GetFirst ();
    while (node)
    {
        wxBitmap *bitmap = (wxBitmap *) node->GetData ();
        wxList::compatibility_iterator next = node->GetNext ();
        if (bitmap->GetVisible())
            delete bitmap;
        node = next;
    }
}

// Pen and Brush lists
wxPenList::~wxPenList ()
{
    wxList::compatibility_iterator node = GetFirst ();
    while (node)
    {
        wxPen *pen = (wxPen *) node->GetData ();
        wxList::compatibility_iterator next = node->GetNext ();
        if (pen->GetVisible())
            delete pen;
        node = next;
    }
}

void wxPenList::AddPen (wxPen * pen)
{
    Append (pen);
}

void wxPenList::RemovePen (wxPen * pen)
{
    DeleteObject (pen);
}

wxPen *wxPenList::FindOrCreatePen (const wxColour& colour, int width, int style)
{
    for (wxList::compatibility_iterator node = GetFirst (); node; node = node->GetNext ())
    {
        wxPen *each_pen = (wxPen *) node->GetData ();
        if (each_pen &&
                each_pen->GetVisible() &&
                each_pen->GetWidth () == width &&
                each_pen->GetStyle () == style &&
                each_pen->GetColour ().Red () == colour.Red () &&
                each_pen->GetColour ().Green () == colour.Green () &&
                each_pen->GetColour ().Blue () == colour.Blue ())
            return each_pen;
    }

    wxPen *pen = new wxPen (colour, width, style);
    if ( !pen->Ok() )
    {
        // don't save the invalid pens in the list
        delete pen;

        return NULL;
    }

    AddPen(pen);

    // we'll delete it ourselves later
    pen->SetVisible(true);

    return pen;
}

wxBrushList::~wxBrushList ()
{
    wxList::compatibility_iterator node = GetFirst ();
    while (node)
    {
        wxBrush *brush = (wxBrush *) node->GetData ();
        wxList::compatibility_iterator next = node->GetNext ();
        if (brush && brush->GetVisible())
            delete brush;
        node = next;
    }
}

void wxBrushList::AddBrush (wxBrush * brush)
{
    Append (brush);
}

wxBrush *wxBrushList::FindOrCreateBrush (const wxColour& colour, int style)
{
    for (wxList::compatibility_iterator node = GetFirst (); node; node = node->GetNext ())
    {
        wxBrush *each_brush = (wxBrush *) node->GetData ();
        if (each_brush &&
                each_brush->GetVisible() &&
                each_brush->GetStyle () == style &&
                each_brush->GetColour ().Red () == colour.Red () &&
                each_brush->GetColour ().Green () == colour.Green () &&
                each_brush->GetColour ().Blue () == colour.Blue ())
            return each_brush;
    }

    wxBrush *brush = new wxBrush (colour, style);

    if ( !brush->Ok() )
    {
        // don't put the brushes we failed to create into the list
        delete brush;

        return NULL;
    }

    AddBrush(brush);

    // we'll delete it ourselves later
    brush->SetVisible(true);

    return brush;
}

void wxBrushList::RemoveBrush (wxBrush * brush)
{
    DeleteObject (brush);
}

wxFontList::~wxFontList ()
{
    wxList::compatibility_iterator node = GetFirst ();
    while (node)
    {
        // Only delete objects that are 'visible', i.e.
        // that have been created using FindOrCreate...,
        // where the pointers are expected to be shared
        // (and therefore not deleted by any one part of an app).
        wxFont *font = (wxFont *) node->GetData ();
        wxList::compatibility_iterator next = node->GetNext ();
        if (font->GetVisible())
            delete font;
        node = next;
    }
}

void wxFontList::AddFont (wxFont * font)
{
    Append (font);
}

void wxFontList::RemoveFont (wxFont * font)
{
    DeleteObject (font);
}

wxFont *wxFontList::FindOrCreateFont(int pointSize,
                                     int family,
                                     int style,
                                     int weight,
                                     bool underline,
                                     const wxString& facename,
                                     wxFontEncoding encoding)
{
    wxFont *font = (wxFont *)NULL;
    wxList::compatibility_iterator node;
    for ( node = GetFirst(); node; node = node->GetNext() )
    {
        font = (wxFont *)node->GetData();
        if ( font->GetVisible() &&
             font->Ok() &&
             font->GetPointSize () == pointSize &&
             font->GetStyle () == style &&
             font->GetWeight () == weight &&
             font->GetUnderlined () == underline )
        {
            int fontFamily = font->GetFamily();

#if defined(__WXGTK__)
            // under GTK the default family is wxSWISS, so looking for a font
            // with wxDEFAULT family should return a wxSWISS one instead of
            // creating a new one
            bool same = (fontFamily == family) ||
                        (fontFamily == wxSWISS && family == wxDEFAULT);
#else // !GTK
            // VZ: but why elsewhere do we require an exact match? mystery...
            bool same = fontFamily == family;
#endif // GTK/!GTK

            // empty facename matches anything at all: this is bad because
            // depending on which fonts are already created, we might get back
            // a different font if we create it with empty facename, but it is
            // still better than never matching anything in the cache at all
            // in this case
            if ( same && !facename.empty() )
            {
                const wxString& fontFace = font->GetFaceName();

                // empty facename matches everything
                same = !fontFace || fontFace == facename;
            }

            if ( same && (encoding != wxFONTENCODING_DEFAULT) )
            {
                // have to match the encoding too
                same = font->GetEncoding() == encoding;
            }

            if ( same )
            {
                return font;
            }
        }
    }

    if ( !node )
    {
        // font not found, create the new one
        font = new wxFont(pointSize, family, style, weight,
                          underline, facename, encoding);

        AddFont(font);

        // and mark it as being cacheable
        font->SetVisible(true);
    }

    return font;
}

void wxBitmapList::AddBitmap(wxBitmap *bitmap)
{
    Append(bitmap);
}

void wxBitmapList::RemoveBitmap(wxBitmap *bitmap)
{
    DeleteObject(bitmap);
}

wxSize wxGetDisplaySize()
{
    int x, y;
    wxDisplaySize(& x, & y);
    return wxSize(x, y);
}

wxRect wxGetClientDisplayRect()
{
    int x, y, width, height;
    wxClientDisplayRect(&x, &y, &width, &height);  // call plat-specific version
    return wxRect(x, y, width, height);
}

wxSize wxGetDisplaySizeMM()
{
    int x, y;
    wxDisplaySizeMM(& x, & y);
    return wxSize(x, y);
}

wxResourceCache::~wxResourceCache ()
{
    wxList::compatibility_iterator node = GetFirst ();
    while (node) {
        wxObject *item = (wxObject *)node->GetData();
        delete item;

        node = node->GetNext ();
    }
}

