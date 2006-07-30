/////////////////////////////////////////////////////////////////////////////
// Name:        sergdi.cpp
// Purpose:     Serialization: GDI classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "sergdi.h"
#endif
#include <wx/objstrm.h>
#include <wx/datstrm.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/serbase.h>
#include <wx/imaglist.h>
#include <wx/region.h>
#include <wx/colour.h>
#include <wx/palette.h>
#include <wx/dcmemory.h>
#include <wx/log.h>
#include "sergdi.h"

IMPLEMENT_SERIAL_CLASS(wxBitmap, wxObject)
IMPLEMENT_SERIAL_CLASS(wxGDIObject, wxObject)
IMPLEMENT_SERIAL_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxColour, wxObject)
IMPLEMENT_SERIAL_CLASS(wxFont, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxPen, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxBrush, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxImageList, wxObject)

IMPLEMENT_ALIAS_SERIAL_CLASS(wxPenList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxBrushList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxFontList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxColourDatabase, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxBitmapList, wxList)

// ----------------------------------------------------------------------------

void WXSERIAL(wxBitmap)::StoreObject(wxObjectOutputStream& s)
{
  // TODO
  // I implemented a basic image saving (maybe I'll need to improve wxWin API).

  int x, y, w, h;
  wxDataOutputStream data_s(s);
  wxBitmap *bitmap = (wxBitmap *)Object();
  wxColour col;
  wxMemoryDC dc;

  w = bitmap->GetWidth();
  h = bitmap->GetHeight();

  if (s.FirstStage()) {
    s.AddChild(bitmap->GetMask());
  }

  dc.SelectObject(*bitmap);

  data_s.Write16(w);
  data_s.Write16(h);
  for (y=0;y<h;y++)
    for (x=0;x<w;x++) {
      dc.GetPixel(x, y, &col);
      data_s.Write8( col.Red() );
      data_s.Write8( col.Green() );
      data_s.Write8( col.Blue() );
    }
}

void WXSERIAL(wxBitmap)::LoadObject(wxObjectInputStream& s)
{
  // TODO
  // I implemented a basic image loading (maybe I'll need to improve wxWin API).
  wxDataInputStream data_s(s);
  wxBitmap *bitmap = (wxBitmap *)Object();
  wxMemoryDC dc;
  wxPen pen;
  int x, y, w, h;
  int r, g, b;

  w = data_s.Read16();
  h = data_s.Read16();

  bitmap->SetWidth(w);
  bitmap->SetHeight(h);
  dc.SelectObject(*bitmap);

  for (y=0;y<h;y++)
    for (x=0;x<w;x++) {
      r = data_s.Read8();
      g = data_s.Read8();
      b = data_s.Read8();
      pen.SetColour(r, g, b);
      dc.SetPen(pen);
      dc.DrawPoint(x,y); 
    }
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxGDIObject)::StoreObject(wxObjectOutputStream& s)
{
  if (s.FirstStage())
    return;

  bool visible = ((wxGDIObject *)Object())->GetVisible();

  wxDataOutputStream data_s(s);
  data_s.Write8(visible);
}

void WXSERIAL(wxGDIObject)::LoadObject(wxObjectInputStream& s)
{
  wxDataInputStream data_s(s);

  ((wxGDIObject *)Object())->SetVisible( data_s.Read8() );
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxRegion)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxGDIObject)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxRect rect = ((wxRegion *)Object())->GetBox();

  data_s.Write16( rect.GetX() ); 
  data_s.Write16( rect.GetY() ); 
  data_s.Write16( rect.GetWidth() ); 
  data_s.Write16( rect.GetHeight() ); 
}

void WXSERIAL(wxRegion)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxRegion *region = (wxRegion *)Object();
  wxRect rect;

  rect.SetX( data_s.Read16() );
  rect.SetY( data_s.Read16() );
  rect.SetWidth( data_s.Read16() );
  rect.SetHeight( data_s.Read16() );

  *region = wxRegion(rect);
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxColour)::StoreObject(wxObjectOutputStream& s)
{
  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxColour *colour = (wxColour *)Object();

  if (!colour->Ok()) {
    data_s.Write8(0);
    data_s.Write8(0);
    data_s.Write8(0);
    wxLogDebug("wxColour (0x%x) isn't ready.\n", colour);
    return;
  }

  data_s.Write8(colour->Red());
  data_s.Write8(colour->Green());
  data_s.Write8(colour->Blue());
}

void WXSERIAL(wxColour)::LoadObject(wxObjectInputStream& s)
{
  wxDataInputStream data_s(s);
  wxColour *colour = (wxColour *)Object();
  int r, g, b;

  r = data_s.Read8();
  g = data_s.Read8();
  b = data_s.Read8();

  colour->Set(r, g, b);
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxPen)::StoreObject(wxObjectOutputStream& s)
{
  wxPen *pen = (wxPen *)Object();
  WXSERIAL(wxGDIObject)::StoreObject(s);

  if (s.FirstStage()) {
    s.AddChild(& (pen->GetColour()) );
    return;
  }

  wxDataOutputStream data_s(s);

  data_s.Write8( pen->GetCap() );
  data_s.Write8( pen->GetJoin() );
  data_s.Write8( pen->GetStyle() );
  data_s.Write8( pen->GetWidth() );
}

void WXSERIAL(wxPen)::LoadObject(wxObjectInputStream& s)
{
  wxPen *pen = (wxPen *)Object();
  wxColour *col = (wxColour *) s.GetChild();

  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);

  pen->SetColour(*col);
  pen->SetCap( data_s.Read8() );
  pen->SetJoin( data_s.Read8() );
  pen->SetStyle( data_s.Read8() );
  pen->SetWidth( data_s.Read8() );
}

// ----------------------------------------------------------------------------
void WXSERIAL(wxBrush)::StoreObject(wxObjectOutputStream& s)
{
  wxBrush *brush = (wxBrush *)Object();
  WXSERIAL(wxGDIObject)::StoreObject(s);

  if (s.FirstStage()) {
    s.AddChild( &(brush->GetColour()) );
    s.AddChild( brush->GetStipple() );
    return;
  }

  wxDataOutputStream data_s(s);
  data_s.Write8( brush->GetStyle() );
}

void WXSERIAL(wxBrush)::LoadObject(wxObjectInputStream& s)
{
  wxBrush *brush = (wxBrush *)Object();
  wxColour *col = (wxColour *)s.GetChild();
  wxBitmap *bmap = (wxBitmap *)s.GetChild();

  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);
  if (bmap)
    *brush = wxBrush(*col, data_s.Read8());
  else
    *brush = wxBrush(bmap);
}

// ----------------------------------------------------------------------------
void WXSERIAL(wxFont)::StoreObject(wxObjectOutputStream& s)
{
  wxFont *font = (wxFont *)Object();

  WXSERIAL(wxGDIObject)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);

  data_s.Write8( font->GetPointSize() );
  data_s.WriteString( font->GetFaceName() );
  data_s.Write8( font->GetFamily() );
  data_s.Write8( font->GetStyle() );
  data_s.Write8( font->GetWeight() );
  data_s.Write8( font->GetUnderlined() );
}

void WXSERIAL(wxFont)::LoadObject(wxObjectInputStream& s)
{
  wxFont *font = (wxFont *)Object();

  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);
  int psize, family, style, weight;
  bool underlined;
  wxString face_name;

  psize      = data_s.Read8();
  face_name  = data_s.ReadString();
  family     = data_s.Read8();
  style      = data_s.Read8();
  weight     = data_s.Read8();
  underlined = data_s.Read8();

  *font = wxFont(psize, face_name, family, style, weight, underlined);
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxImageList)::StoreObject(wxObjectOutputStream& s)
{
  wxImageList *list = (wxImageList *)Object();
  int i;

  if (s.FirstStage()) {
#ifdef __WXGTK__
    for (i=0;i<list->GetImageCount();i++)
      s.AddChild(list->GetBitmap(i));
#endif
  }

  wxDataOutputStream data_s(s);

  data_s.Write32(list->GetImageCount());
}

void WXSERIAL(wxImageList)::LoadObject(wxObjectInputStream& s)
{
  int i, count;
  wxImageList *list = (wxImageList *)Object();
  wxDataInputStream data_s(s);

  count = data_s.Read32();
  for (i=0;i<count;i++)
    list->Add(*((wxBitmap *)s.GetChild()));
}
