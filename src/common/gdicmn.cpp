/////////////////////////////////////////////////////////////////////////////
// Name:        gdicmn.cpp
// Purpose:     Common GDI classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gdicmn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/gdicmn.h"
#include "wx/brush.h"
#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/font.h"
#include "wx/palette.h"

#include <string.h>

#ifdef __WXMSW__
#include <windows.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxColourDatabase, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxFontList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxPenList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxBrushList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxResourceCache, wxList)
/*
IMPLEMENT_DYNAMIC_CLASS(wxRect, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPoint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxRealPoint, wxObject)
*/
#endif

wxRect::wxRect()
{
    x = 0; y = 0; width = 0; height = 0;
}

wxRect::wxRect(long xx, long yy, long w, long h)
{
    x = xx; y = yy; width = w; height = h;
}

wxRect::wxRect(const wxPoint& topLeft, const wxPoint& bottomRight)
{
  x = topLeft.x;
  y = topLeft.y;
  width = bottomRight.x - topLeft.x;
  height = bottomRight.y - topLeft.y;
  
  if (width < 0)
  {
    width = -width;
    x -= width;
  }
  
  if (height < 0)
  {
    height = -height;
    x -= height;
  }
}

wxRect::wxRect(const wxPoint& point, const wxSize& size)
{
    x = point.x; y = point.y;
    width = size.x; height = size.y;
}

wxRect::wxRect(const wxRect& rect)
{
    x = rect.x; 
    y = rect.y; 
    width = rect.width; 
    height = rect.height;
}

wxRect& wxRect::operator = (const wxRect& rect)
{
    x = rect.x; y = rect.y; width = rect.width; height = rect.height;
    return *this;
}

bool wxRect::operator == (const wxRect& rect)
{
  return ((x == rect.x) &&
          (y == rect.y) &&
          (width == rect.width) &&
          (height == rect.height));
}

bool wxRect::operator != (const wxRect& rect)
{
  return ((x != rect.x) ||
          (y != rect.y) ||
          (width != rect.width) ||
          (height != rect.height));
}

wxColourDatabase::wxColourDatabase (int type):
wxList (type)
{
}

wxColourDatabase::~wxColourDatabase ()
{
  // Cleanup Colour allocated in Initialize()
  wxNode *node = First ();
  while (node)
    {
      wxColour *col = (wxColour *) node->Data ();
      wxNode *next = node->Next ();
      delete col;
      node = next;
    }
}

// Colour database stuff
void wxColourDatabase::Initialize ()
{
  // Don't initialize for X: colours are found
  // in FindColour below.
  // Added: Not all
  
  struct cdef {
   char *name;
   int r,g,b;
  };
  cdef cc;
  static cdef table[]={
  
// #ifdef __WXMSW__
   {"AQUAMARINE",112, 219, 147},
   {"BLACK",0, 0, 0},
   {"BLUE", 0, 0, 255},
   {"BLUE VIOLET", 159, 95, 159},
   {"BROWN", 165, 42, 42},
   {"CADET BLUE", 95, 159, 159},
   {"CORAL", 255, 127, 0},
   {"CORNFLOWER BLUE", 66, 66, 111},
   {"CYAN", 0, 255, 255},
   {"DARK GREY", 47, 47, 47},   // ?

   {"DARK GREEN", 47, 79, 47},
   {"DARK OLIVE GREEN", 79, 79, 47},
   {"DARK ORCHID", 153, 50, 204},
   {"DARK SLATE BLUE", 107, 35, 142},
   {"DARK SLATE GREY", 47, 79, 79},
   {"DARK TURQUOISE", 112, 147, 219},
   {"DIM GREY", 84, 84, 84},
   {"FIREBRICK", 142, 35, 35},
   {"FOREST GREEN", 35, 142, 35},
   {"GOLD", 204, 127, 50},
   {"GOLDENROD", 219, 219, 112},
   {"GREY", 128, 128, 128},
   {"GREEN", 0, 255, 0},
   {"GREEN YELLOW", 147, 219, 112},
   {"INDIAN RED", 79, 47, 47},
   {"KHAKI", 159, 159, 95},
   {"LIGHT BLUE", 191, 216, 216},
   {"LIGHT GREY", 192, 192, 192},
   {"LIGHT STEEL BLUE", 143, 143, 188},
   {"LIME GREEN", 50, 204, 50},
   {"LIGHT MAGENTA", 255, 0, 255},
   {"MAGENTA", 255, 0, 255},
   {"MAROON", 142, 35, 107},
   {"MEDIUM AQUAMARINE", 50, 204, 153},
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
   {"PINK", 188, 143, 234},
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
   {"YELLOW GREEN", 153, 204, 50},
// #endif

#if defined(__WXGTK__) || defined(__X__)
   {"MEDIUM GOLDENROD", 234, 234, 173},
   {"MEDIUM FOREST GREEN", 107, 142, 35},
   {"LIGHT MAGENTA", 255, 0, 255},
   {"MEDIUM GREY", 100, 100, 100},
#endif
   
   {0,0,0,0}
  };
  int i;
  for (i=0;cc=table[i],cc.name!=0;i++)
  {
    Append(cc.name,new wxColour(cc.r,cc.g,cc.b));
  }

}

/*
 * Changed by Ian Brown, July 1994.
 *
 * When running under X, the Colour Database starts off empty. The X server
 * is queried for the colour first time after which it is entered into the
 * database. This allows our client to use the server colour database which
 * is hopefully gamma corrected for the display being used.
 */

wxColour *wxColourDatabase::FindColour(const wxString& colour)
{
  wxNode *node = Find((char *) (const char *)colour);
  if (node)
    return (wxColour *)node->Data();
    
#ifdef __WXMSW__
  else return NULL;
#endif

// TODO for other implementations. This should really go into
// platform-specific directories.
#ifdef __WXSTUBS__
  else return NULL;
#endif

#ifdef __WXGTK__
  else {
    wxColour *col = new wxColour( colour );
    
    if (!(col->Ok())) {
      delete col;
      return NULL;
    }
    Append( colour, col );
    return col;
  }
#endif

#ifdef __X__
  else {
    XColor xcolour;

#ifdef __WXMOTIF__
    Display *display = XtDisplay(wxTheApp->topLevel) ;
#endif
#ifdef __XVIEW__
    Xv_Screen screen = xv_get(xview_server, SERVER_NTH_SCREEN, 0);
    Xv_opaque root_window = xv_get(screen, XV_ROOT);
    Display *display = (Display *)xv_get(root_window, XV_DISPLAY);
#endif

    /* MATTHEW: [4] Use wxGetMainColormap */
    if (!XParseColor(display, wxGetMainColormap(display), colour,&xcolour))
      return NULL;

    unsigned char r = (unsigned char)(xcolour.red >> 8);
    unsigned char g = (unsigned char)(xcolour.green >> 8);
    unsigned char b = (unsigned char)(xcolour.blue >> 8);

    wxColour *col = new wxColour(r, g, b);
    Append(colour, col);

    return col;
  }
#endif
}

wxString wxColourDatabase::FindName (const wxColour& colour) const
{
  unsigned char red = colour.Red ();
  unsigned char green = colour.Green ();
  unsigned char blue = colour.Blue ();
  
  for (wxNode * node = First (); node; node = node->Next ())
  {
      wxColour *col = (wxColour *) node->Data ();
      
      if (col->Red () == red && col->Green () == green && col->Blue () == blue)
	{
	  char *found = node->key.string;
	  if (found)
	    return wxString(found);
	}
  }
  return wxString("");			// Not Found

}

void wxInitializeStockLists () {
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
  wxFontPool = new XFontPool;
#endif

  wxNORMAL_FONT = new wxFont (12, wxMODERN, wxNORMAL, wxNORMAL);
  wxSMALL_FONT = new wxFont (10, wxSWISS, wxNORMAL, wxNORMAL);
  wxITALIC_FONT = new wxFont (12, wxROMAN, wxITALIC, wxNORMAL);
  wxSWISS_FONT = new wxFont (12, wxSWISS, wxNORMAL, wxNORMAL);

  wxRED_PEN = new wxPen ("RED", 1, wxSOLID);
  wxCYAN_PEN = new wxPen ("CYAN", 1, wxSOLID);
  wxGREEN_PEN = new wxPen ("GREEN", 1, wxSOLID);
  wxBLACK_PEN = new wxPen ("BLACK", 1, wxSOLID);
  wxWHITE_PEN = new wxPen ("WHITE", 1, wxSOLID);
  wxTRANSPARENT_PEN = new wxPen ("BLACK", 1, wxTRANSPARENT);
  wxBLACK_DASHED_PEN = new wxPen ("BLACK", 1, wxSHORT_DASH);
  wxGREY_PEN = new wxPen ("GREY", 1, wxSOLID);
  wxMEDIUM_GREY_PEN = new wxPen ("MEDIUM GREY", 1, wxSOLID);
  wxLIGHT_GREY_PEN = new wxPen ("LIGHT GREY", 1, wxSOLID);

  wxBLUE_BRUSH = new wxBrush ("BLUE", wxSOLID);
  wxGREEN_BRUSH = new wxBrush ("GREEN", wxSOLID);
  wxWHITE_BRUSH = new wxBrush ("WHITE", wxSOLID);
  wxBLACK_BRUSH = new wxBrush ("BLACK", wxSOLID);
  wxTRANSPARENT_BRUSH = new wxBrush ("BLACK", wxTRANSPARENT);
  wxCYAN_BRUSH = new wxBrush ("CYAN", wxSOLID);
  wxRED_BRUSH = new wxBrush ("RED", wxSOLID);
  wxGREY_BRUSH = new wxBrush ("GREY", wxSOLID);
  wxMEDIUM_GREY_BRUSH = new wxBrush ("MEDIUM GREY", wxSOLID);
  wxLIGHT_GREY_BRUSH = new wxBrush ("LIGHT GREY", wxSOLID);

  wxBLACK = new wxColour ("BLACK");
  wxWHITE = new wxColour ("WHITE");
  wxRED = new wxColour ("RED");
  wxBLUE = new wxColour ("BLUE");
  wxGREEN = new wxColour ("GREEN");
  wxCYAN = new wxColour ("CYAN");
  wxLIGHT_GREY = new wxColour ("LIGHT GREY");

  wxSTANDARD_CURSOR = new wxCursor (wxCURSOR_ARROW);
  wxHOURGLASS_CURSOR = new wxCursor (wxCURSOR_WAIT);
  wxCROSS_CURSOR = new wxCursor (wxCURSOR_CROSS);
}

void 
wxDeleteStockObjects ()
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

void wxDeleteStockLists() {
  wxDELETE(wxTheBrushList);
  wxDELETE(wxThePenList);
  wxDELETE(wxTheFontList);
  wxDELETE(wxTheBitmapList);
}

wxBitmapList::wxBitmapList ()
{
}

wxBitmapList::~wxBitmapList ()
{
  printf( "Count: %d.\n", Number() );

  wxNode *node = First ();
  while (node)
    {
      wxBitmap *bitmap = (wxBitmap *) node->Data ();
      wxNode *next = node->Next ();
      delete bitmap;
//      bitmap->FreeResource(TRUE);
      node = next;
    }
}

// Pen and Brush lists
wxPenList::~wxPenList ()
{
  wxNode *node = First ();
  while (node)
    {
      wxPen *pen = (wxPen *) node->Data ();
      wxNode *next = node->Next ();
      delete pen;
//      pen->FreeResource(TRUE);
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
  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxPen *each_pen = (wxPen *) node->Data ();
      if (each_pen && each_pen->GetVisible() &&
	  each_pen->GetWidth () == width &&
	  each_pen->GetStyle () == style &&
	  each_pen->GetColour ().Red () == colour.Red () &&
	  each_pen->GetColour ().Green () == colour.Green () &&
	  each_pen->GetColour ().Blue () == colour.Blue ())
	return each_pen;
    }
  wxPen *pen = new wxPen (colour, width, style);

  // Yes, we can return a pointer to this in a later FindOrCreatePen call,
  // because we created it within FindOrCreatePen. Safeguards against
  // returning a pointer to an automatic variable and hanging on to it
  // (dangling pointer).
  pen->SetVisible(TRUE);
  return pen;
}

wxPen *wxPenList::FindOrCreatePen (const wxString& colour, int width, int style)
{
  wxColour *the_colour = wxTheColourDatabase->FindColour (colour);
  if (the_colour)
    return FindOrCreatePen (*the_colour, width, style);
  else
    return NULL;
}

wxBrushList::~wxBrushList ()
{
  wxNode *node = First ();
  while (node)
    {
      wxBrush *brush = (wxBrush *) node->Data ();
      wxNode *next = node->Next ();
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
  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxBrush *each_brush = (wxBrush *) node->Data ();
      if (each_brush && each_brush->GetVisible() &&
	  each_brush->GetStyle () == style &&
	  each_brush->GetColour ().Red () == colour.Red () &&
	  each_brush->GetColour ().Green () == colour.Green () &&
	  each_brush->GetColour ().Blue () == colour.Blue ())
	return each_brush;
    }
  // Yes, we can return a pointer to this in a later FindOrCreateBrush call,
  // because we created it within FindOrCreateBrush. Safeguards against
  // returning a pointer to an automatic variable and hanging on to it
  // (dangling pointer).
  wxBrush *brush = new wxBrush (colour, style);
  brush->SetVisible(TRUE);
  return brush;
}

wxBrush *wxBrushList::FindOrCreateBrush (const wxString& colour, int style)
{
  wxColour *the_colour = wxTheColourDatabase->FindColour (colour);
  if (the_colour)
    return FindOrCreateBrush (*the_colour, style);
  else
    return NULL;
}

void wxBrushList::RemoveBrush (wxBrush * brush)
{
  DeleteObject (brush);
}

wxFontList::~wxFontList ()
{
#ifdef __WXMSW__
  wxNode *node = First ();
  while (node)
    {
/*
      wxFont *font = (wxFont *) node->Data ();
      wxNode *next = node->Next ();
      delete font;
      node = next;
*/
	  // New for 2.0: don't delete the font (it may be a member
	  // of a wxDC, for example)
      wxFont *font = (wxFont *) node->Data ();
      wxNode *next = node->Next ();

	  // Force the font to be deleted
      font->FreeResource(TRUE);
      node = next;
    }
#endif
}

void wxFontList::AddFont (wxFont * font)
{
  Append (font);
}

void wxFontList::RemoveFont (wxFont * font)
{
  DeleteObject (font);
}

wxFont *wxFontList::
	FindOrCreateFont (int PointSize, int FamilyOrFontId, int Style, int Weight, bool underline, const wxString& Face)
{
  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxFont *each_font = (wxFont *) node->Data ();
      if (each_font && each_font->GetVisible() && each_font->Ok() &&
	  each_font->GetPointSize () == PointSize &&
	  each_font->GetStyle () == Style &&
	  each_font->GetWeight () == Weight &&
	  each_font->GetUnderlined () == underline &&
#if defined(__X__) || (defined(__WXMSW__) && USE_PORTABLE_FONTS_IN_MSW)
	  each_font->GetFontId () == FamilyOrFontId) /* New font system */
#else
	  each_font->GetFamily () == FamilyOrFontId &&
          (!each_font->GetFaceName() || each_font->GetFaceName() == Face))
#endif
	return each_font;
    }
  wxFont *font = new wxFont (PointSize, FamilyOrFontId, Style, Weight, underline, Face);
  font->SetVisible(TRUE);
  return font;
}

void wxBitmapList::AddBitmap(wxBitmap *bitmap)
{ Append(bitmap); }
void wxBitmapList::RemoveBitmap(wxBitmap *bitmap)
{ DeleteObject(bitmap); }

wxSize wxGetDisplaySize()
{
    int x, y;
    wxDisplaySize(& x, & y);
    return wxSize(x, y);
}

wxResourceCache::wxResourceCache () : wxList() {
}

wxResourceCache::wxResourceCache (const unsigned int the_key_type) : wxList(the_key_type) {
}

wxResourceCache::~wxResourceCache () {
  wxNode *node = First ();
  while (node) {
    wxGDIObject *item = (wxGDIObject *)node->Data();
    if (item->IsKindOf(CLASSINFO(wxBrush))) {
      wxBrush *brush = (wxBrush *)item;
      delete brush;
    }

    if (item->IsKindOf(CLASSINFO(wxFont))) {
      wxFont *font = (wxFont *)item;
      delete font;
    }

    if (item->IsKindOf(CLASSINFO(wxBitmap))) {
      wxBitmap *bitmap = (wxBitmap *)item;
      delete bitmap;
    }

    if (item->IsKindOf(CLASSINFO(wxColour))) {
      wxColour *colour = (wxColour *)item;
      delete colour;
    }

    wxNode *next = node->Next ();
    node = next;
  }
}

