/////////////////////////////////////////////////////////////////////////////
// Name:        gdicmn.h
// Purpose:     Common GDI classes, types and declarations
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDICMNH__
#define _WX_GDICMNH__

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "gdicmn.h"
#endif

#include "wx/object.h"
#include "wx/list.h"
#include "wx/hash.h"
#include "wx/string.h"
#include "wx/setup.h"
#include "wx/colour.h"

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBrush;
class WXDLLEXPORT wxColour;
class WXDLLEXPORT wxCursor;
class WXDLLEXPORT wxFont;
class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxPalette;
class WXDLLEXPORT wxPen;
class WXDLLEXPORT wxRegion;
class WXDLLEXPORT wxString;

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// Bitmap flags
enum
{
    wxBITMAP_TYPE_BMP = 1,
    wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_RESOURCE = wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_ICO,
    wxBITMAP_TYPE_ICO_RESOURCE,
    wxBITMAP_TYPE_CUR,
    wxBITMAP_TYPE_CUR_RESOURCE,
    wxBITMAP_TYPE_XBM,
    wxBITMAP_TYPE_XBM_DATA,
    wxBITMAP_TYPE_XPM,
    wxBITMAP_TYPE_XPM_DATA,
    wxBITMAP_TYPE_TIF,
    wxBITMAP_TYPE_TIF_RESOURCE,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_GIF_RESOURCE,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_PNG_RESOURCE,
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_JPEG_RESOURCE,
    wxBITMAP_TYPE_PNM,
    wxBITMAP_TYPE_PNM_RESOURCE,
    wxBITMAP_TYPE_PCX,
    wxBITMAP_TYPE_PCX_RESOURCE,
    wxBITMAP_TYPE_ANY = 50
};

// Standard cursors
enum wxStockCursor
{
    wxCURSOR_NONE,          // should be 0
    wxCURSOR_ARROW,
    wxCURSOR_BULLSEYE,
    wxCURSOR_CHAR,
    wxCURSOR_CROSS,
    wxCURSOR_HAND,
    wxCURSOR_IBEAM,
    wxCURSOR_LEFT_BUTTON,
    wxCURSOR_MAGNIFIER,
    wxCURSOR_MIDDLE_BUTTON,
    wxCURSOR_NO_ENTRY,
    wxCURSOR_PAINT_BRUSH,
    wxCURSOR_PENCIL,
    wxCURSOR_POINT_LEFT,
    wxCURSOR_POINT_RIGHT,
    wxCURSOR_QUESTION_ARROW,
    wxCURSOR_RIGHT_BUTTON,
    wxCURSOR_SIZENESW,
    wxCURSOR_SIZENS,
    wxCURSOR_SIZENWSE,
    wxCURSOR_SIZEWE,
    wxCURSOR_SIZING,
    wxCURSOR_SPRAYCAN,
    wxCURSOR_WAIT,
    wxCURSOR_WATCH,
    wxCURSOR_BLANK,
#ifdef __WXGTK__
    wxCURSOR_DEFAULT, // standard X11 cursor
#endif
#ifdef __X__
    // Not yet implemented for Windows
    wxCURSOR_CROSS_REVERSE,
    wxCURSOR_DOUBLE_ARROW,
    wxCURSOR_BASED_ARROW_UP,
    wxCURSOR_BASED_ARROW_DOWN,
#endif // X11

    wxCURSOR_MAX
};

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

/* Useful macro for creating icons portably, for example:

    wxIcon *icon = new wxICON(mondrian);

  expands into:

    wxIcon *icon = new wxIcon("mondrian");      // On wxMSW
    wxIcon *icon = new wxIcon(mondrian_xpm);    // On wxGTK
 */

#ifdef __WXMSW__
    // Load from a resource
    #define wxICON(X) wxIcon("" #X "")
#elif defined(__WXPM__)
    // Load from a resource
    #define wxICON(X) wxIcon("" #X "")
#elif defined(__WXGTK__)
    // Initialize from an included XPM
    #define wxICON(X) wxIcon( (const char**) X##_xpm )
#elif defined(__WXMOTIF__)
    // Initialize from an included XPM
    #define wxICON(X) wxIcon( X##_xpm )
#else
    // This will usually mean something on any platform
    #define wxICON(X) wxIcon("" #X "")
#endif // platform

/* Another macro: this one is for portable creation of bitmaps. We assume that
   under Unix bitmaps live in XPMs and under Windows they're in ressources.
 */

#if defined(__WXMSW__) || defined(__WXPM__)
    #define wxBITMAP(name) wxBitmap(#name, wxBITMAP_TYPE_RESOURCE)
#else // !(Windows || OS2)
    #define wxBITMAP(name) wxBitmap(name##_xpm, wxBITMAP_TYPE_XPM)
#endif // platform

// ===========================================================================
// classes
// ===========================================================================

// ---------------------------------------------------------------------------
// wxSize
// ---------------------------------------------------------------------------
class WXDLLEXPORT wxSize
{
public:
    // members are public for compatibility (don't use them directly,
    // especially that there names were chosen very unfortunately - they should
    // have been called width and height)
    long x;
    long y;

    // constructors
    wxSize() { x = y = 0; }
    wxSize(long xx, long yy) { Set(xx, yy); }

    // no copy ctor or assignment operator - the defaults are ok
    bool operator==(const wxSize& sz) const { return x == sz.x && y == sz.y; }

    // FIXME are these really useful? If they're, we should have += &c as well
    wxSize operator+(const wxSize& sz) { return wxSize(x + sz.x, y + sz.y); }
    wxSize operator-(const wxSize& sz) { return wxSize(x - sz.x, y - sz.y); }

    // accessors
    void Set(long xx, long yy) { x = xx; y = yy; }
    void SetWidth(long w) { x = w; }
    void SetHeight(long h) { y = h; }

    long GetWidth() const { return x; }
    long GetHeight() const { return y; }

    // compatibility
    long GetX() const { return x; }
    long GetY() const { return y; }
};

// ---------------------------------------------------------------------------
// Point classes: with real or integer coordinates
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxRealPoint
{
public:
    double x;
    double y;

    wxRealPoint() { x = y = 0.0; };
    wxRealPoint(double xx, double yy) { x = xx; y = yy; };

    wxRealPoint operator+(const wxRealPoint& pt) { return wxRealPoint(x + pt.x, y + pt.y); }
    wxRealPoint operator-(const wxRealPoint& pt) { return wxRealPoint(x - pt.x, y - pt.y); }

    bool operator==(const wxRealPoint& pt) const { return x == pt.x && y == pt.y; }
};

class WXDLLEXPORT wxPoint
{
public:
#if defined(__WXMSW__) && !defined(__WIN32__)
    int x;
    int y;
#else
    long x;
    long y;
#endif

    wxPoint() { x = y = 0; };
    wxPoint(long xx, long yy) { x = xx; y = yy; };

    // no copy ctor or assignment operator - the defaults are ok

    // comparison
    bool operator==(const wxPoint& p) const { return x == p.x && y == p.y; }
    bool operator!=(const wxPoint& p) const { return !(*this == p); }

    // arithmetic operations (component wise)
    wxPoint operator+(const wxPoint& p) { return wxPoint(x + p.x, y + p.y); }
    wxPoint operator-(const wxPoint& p) { return wxPoint(x - p.x, y - p.y); }

    wxPoint& operator+=(const wxPoint& p) { x += p.x; y += p.y; return *this; }
    wxPoint& operator-=(const wxPoint& p) { x -= p.x; y -= p.y; return *this; }
};

#if WXWIN_COMPATIBILITY
    #define wxIntPoint wxPoint
    #define wxRectangle wxRect
#endif // WXWIN_COMPATIBILITY

// ---------------------------------------------------------------------------
// wxRect
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxRect
{
public:
    wxRect() { x = y = width = height = 0; }
    wxRect(long xx, long yy, long ww, long hh)
        { x = xx; y = yy; width = ww; height = hh; }
    wxRect(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRect(const wxPoint& pos, const wxSize& size);

    // default copy ctor and assignment operators ok

    long GetX() const { return x; }
    void SetX(long xx) { x = xx; }

    long GetY() const { return y; }
    void SetY(long yy) { y = yy; }

    long GetWidth() const { return width; }
    void SetWidth(long w) { width = w; }

    long GetHeight() const { return height; }
    void SetHeight(long h) { height = h; }

    wxPoint GetPosition() const { return wxPoint(x, y); }
    wxSize GetSize() const { return wxSize(width, height); }

    // MFC-like functions

    long GetLeft()   const { return x; }
    long GetTop()    const { return y; }
    long GetBottom() const { return y + height - 1; }
    long GetRight()  const { return x + width - 1; }

    void SetLeft(long left) { x = left; }
    void SetRight(long right) { width = right - x + 1; }
    void SetTop(long top) { y = top; }
    void SetBottom(long bottom) { height = bottom - y + 1; }

    bool operator==(const wxRect& rect) const;
    bool operator!=(const wxRect& rect) const { return !(*this == rect); }

    bool Inside(int cx, int cy) const;
    wxRect operator + (const wxRect& rect) const;
    const wxRect& operator += (const wxRect& rect);
    
public:
    long x, y, width, height;
};

// ---------------------------------------------------------------------------
// Management of pens, brushes and fonts
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxPenList : public wxList
{
    DECLARE_DYNAMIC_CLASS(wxPenList)

public:
    wxPenList() { }
    ~wxPenList();

    void AddPen(wxPen *pen);
    void RemovePen(wxPen *pen);
    wxPen *FindOrCreatePen(const wxColour& colour, int width, int style);
};

class WXDLLEXPORT wxBrushList : public wxList
{
    DECLARE_DYNAMIC_CLASS(wxBrushList)

public:
    wxBrushList() { }
    ~wxBrushList();

    void AddBrush(wxBrush *brush);
    void RemoveBrush(wxBrush *brush);
    wxBrush *FindOrCreateBrush(const wxColour& colour, int style);
};

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

class WXDLLEXPORT wxFontList : public wxList
{
    DECLARE_DYNAMIC_CLASS(wxFontList)

public:
    wxFontList() { }
    ~wxFontList();

    void AddFont(wxFont *font);
    void RemoveFont(wxFont *font);
    wxFont *FindOrCreateFont(int pointSize, int family, int style, int weight,
                             bool underline = FALSE,
                             const wxString& face = wxEmptyString);
};

class WXDLLEXPORT wxColourDatabase : public wxList
{
    DECLARE_CLASS(wxColourDatabase)

public:
    wxColourDatabase(int type);
    ~wxColourDatabase() ;

    // Not const because it may add a name to the database
    wxColour *FindColour(const wxString& colour) ;
    wxString FindName(const wxColour& colour) const;
    void Initialize();
};

class WXDLLEXPORT wxBitmapList : public wxList
{
    DECLARE_DYNAMIC_CLASS(wxBitmapList)

public:
    wxBitmapList();
    ~wxBitmapList();

    void AddBitmap(wxBitmap *bitmap);
    void RemoveBitmap(wxBitmap *bitmap);
};

class WXDLLEXPORT wxResourceCache: public wxList
{
public:
    wxResourceCache() { }
    wxResourceCache(const unsigned int keyType) : wxList(keyType) { }
    ~wxResourceCache();

private:
    DECLARE_DYNAMIC_CLASS(wxResourceCache)
};

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// Lists of GDI objects
WXDLLEXPORT_DATA(extern wxPenList*)   wxThePenList;
WXDLLEXPORT_DATA(extern wxBrushList*)   wxTheBrushList;
WXDLLEXPORT_DATA(extern wxFontList*)    wxTheFontList;
WXDLLEXPORT_DATA(extern wxBitmapList*)  wxTheBitmapList;

// Stock objects
WXDLLEXPORT_DATA(extern wxFont*)      wxNORMAL_FONT;
WXDLLEXPORT_DATA(extern wxFont*)      wxSMALL_FONT;
WXDLLEXPORT_DATA(extern wxFont*)      wxITALIC_FONT;
WXDLLEXPORT_DATA(extern wxFont*)      wxSWISS_FONT;

WXDLLEXPORT_DATA(extern wxPen*)      wxRED_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxCYAN_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxGREEN_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxBLACK_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxWHITE_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxTRANSPARENT_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxBLACK_DASHED_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxGREY_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxMEDIUM_GREY_PEN;
WXDLLEXPORT_DATA(extern wxPen*)      wxLIGHT_GREY_PEN;

WXDLLEXPORT_DATA(extern wxBrush*)    wxBLUE_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxGREEN_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxWHITE_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxBLACK_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxGREY_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxMEDIUM_GREY_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxLIGHT_GREY_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxTRANSPARENT_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxCYAN_BRUSH;
WXDLLEXPORT_DATA(extern wxBrush*)    wxRED_BRUSH;

WXDLLEXPORT_DATA(extern wxColour*)    wxBLACK;
WXDLLEXPORT_DATA(extern wxColour*)    wxWHITE;
WXDLLEXPORT_DATA(extern wxColour*)    wxRED;
WXDLLEXPORT_DATA(extern wxColour*)    wxBLUE;
WXDLLEXPORT_DATA(extern wxColour*)    wxGREEN;
WXDLLEXPORT_DATA(extern wxColour*)    wxCYAN;
WXDLLEXPORT_DATA(extern wxColour*)    wxLIGHT_GREY;

// 'Null' objects
WXDLLEXPORT_DATA(extern wxBitmap)     wxNullBitmap;
WXDLLEXPORT_DATA(extern wxIcon)       wxNullIcon;
WXDLLEXPORT_DATA(extern wxCursor)     wxNullCursor;
WXDLLEXPORT_DATA(extern wxPen)        wxNullPen;
WXDLLEXPORT_DATA(extern wxBrush)      wxNullBrush;
WXDLLEXPORT_DATA(extern wxPalette)     wxNullPalette;
WXDLLEXPORT_DATA(extern wxFont)       wxNullFont;
WXDLLEXPORT_DATA(extern wxColour)     wxNullColour;

// Stock cursors types
WXDLLEXPORT_DATA(extern wxCursor*)    wxSTANDARD_CURSOR;
WXDLLEXPORT_DATA(extern wxCursor*)    wxHOURGLASS_CURSOR;
WXDLLEXPORT_DATA(extern wxCursor*)    wxCROSS_CURSOR;

WXDLLEXPORT_DATA(extern wxColourDatabase*)  wxTheColourDatabase;

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

// The list of objects which should be deleted
WXDLLEXPORT_DATA(extern wxList) wxPendingDelete;

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// resource management
extern void WXDLLEXPORT wxInitializeStockObjects();
extern void WXDLLEXPORT wxInitializeStockLists();
extern void WXDLLEXPORT wxDeleteStockObjects();
extern void WXDLLEXPORT wxDeleteStockLists();

// is the display colour (or monochrome)?
extern bool WXDLLEXPORT wxColourDisplay();

// Returns depth of screen
extern int WXDLLEXPORT wxDisplayDepth();
#define wxGetDisplayDepth wxDisplayDepth

// get the diaplay size
extern void WXDLLEXPORT wxDisplaySize(int *width, int *height);
extern wxSize WXDLLEXPORT wxGetDisplaySize();

// set global cursor
extern void WXDLLEXPORT wxSetCursor(const wxCursor& cursor);

#endif
    // _WX_GDICMNH__
