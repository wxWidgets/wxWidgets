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
#include "sergdi.h"

IMPLEMENT_SERIAL_CLASS(wxBitmap, wxObject)
IMPLEMENT_SERIAL_CLASS(wxGDIObject, wxObject)
IMPLEMENT_SERIAL_CLASS(wxColour, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxFont, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxPen, wxGDIObject)
IMPLEMENT_SERIAL_CLASS(wxBrush, wxGDIObject)

IMPLEMENT_ALIAS_SERIAL_CLASS(wxPenList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxBrushList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxFontList, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxColourDatabase, wxList)
IMPLEMENT_ALIAS_SERIAL_CLASS(wxBitmapList, wxList)

void WXSERIAL(wxBitmap)::StoreObject(wxObjectOutputStream& s)
{
  // TODO
}

void WXSERIAL(wxBitmap)::LoadObject(wxObjectInputStream& s)
{
  // TODO
}

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

void WXSERIAL(wxColour)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxGDIObject)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxColour *colour = (wxColour *)Object();

  data_s.Write8(colour->Red());
  data_s.Write8(colour->Green());
  data_s.Write8(colour->Blue());
}

void WXSERIAL(wxColour)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxGDIObject)::LoadObject(s);
  
  wxDataInputStream data_s(s);
  wxColour *colour = (wxColour *)Object();
  int r, g, b;

  r = data_s.Read8();
  g = data_s.Read8();
  b = data_s.Read8();

  colour->Set(r, g, b);
}

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
  wxColour *col = (wxColour *) s.GetChild(0);

  s.RemoveChildren(1);

  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);

  pen->SetColour(*col);
  pen->SetCap( data_s.Read8() );
  pen->SetJoin( data_s.Read8() );
  pen->SetStyle( data_s.Read8() );
  pen->SetWidth( data_s.Read8() );
}

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
  wxColour *col = (wxColour *)s.GetChild(0);
  wxBitmap *bmap = (wxBitmap *)s.GetChild(1);

  s.RemoveChildren(2);

  WXSERIAL(wxGDIObject)::LoadObject(s);

  wxDataInputStream data_s(s);
  if (bmap)
    *brush = wxBrush(*col, data_s.Read8());
  else
    *brush = wxBrush(bmap);
}

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
