/*-*- c++ -*-********************************************************
 * wxllist: wxLayoutList, a layout engine for text and graphics     *
 *                                                                  *
 * (C) 1998-1999 by Karsten Ballüder (Ballueder@usa.net)            *
 *                                                                  *
 * $Id$
 *******************************************************************/

/*

  Some docs:

  Layout() recalculates the objects, sizes, etc.
  Draw()   just draws them with the current settings, without
           re-layout()ing them again

  Each line has its own wxLayoutStyleInfo structure which gets updated
  from within Layout(). Thanks to this, we don't need to re-layout all
  lines if we want to draw one, but can just use its styleinfo to set
  the right font.

 */

#ifdef __GNUG__
#   pragma implementation "wxllist.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#include "Mpch.h"

#ifdef M_BASEDIR
#   include "Mcommon.h"
#   include "gui/wxllist.h"
#   include "gui/wxlparser.h"
#   define  SHOW_SELECTIONS 1
#else
#   include "wxllist.h"
#   include "wxlparser.h"
#   define SHOW_SELECTIONS 1
#endif

#ifndef USE_PCH
#   include <iostream.h>

#   include <wx/dc.h>
#   include <wx/dcps.h>
#   include <wx/print.h>
#   include <wx/log.h>
#   include <wx/filefn.h>
#endif

#ifdef WXLAYOUT_USE_CARET
#   include <wx/caret.h>
#endif // WXLAYOUT_USE_CARET

#include <ctype.h>

/// This should never really get created
#define   WXLLIST_TEMPFILE   "__wxllist.tmp"

#ifdef WXLAYOUT_DEBUG

#  define   TypeString(t)      g_aTypeStrings[t]
#  define   WXLO_DEBUG(x)      wxLogDebug x

   static const char *g_aTypeStrings[] =
   {
      "invalid", "text", "cmd", "icon"
   };
   void
   wxLayoutObject::Debug(void)
   {
      WXLO_DEBUG(("%s",g_aTypeStrings[GetType()]));
   }
#else
#  define   TypeString(t)        ""
#  define   WXLO_DEBUG(x)
#endif

// FIXME under MSW, this constant is needed to make the thing properly redraw
//       itself - I don't know where the size calculation error is and I can't
//       waste time looking for it right now. Search for occurences of
//       MSW_CORRECTION to find all the places where I did it.
#ifdef __WXMSW__
   static const int MSW_CORRECTION = 10;
#else
   static const int MSW_CORRECTION = 0;
#endif

/// Cursors smaller than this disappear in XOR drawing mode
#define WXLO_MINIMUM_CURSOR_WIDTH   4

/// Use this character to estimate a cursor size when none is available.
#define WXLO_CURSORCHAR   "E"
/** @name Helper functions */
//@{
/// allows me to compare to wxPoints
bool operator <=(wxPoint const &p1, wxPoint const &p2)
{
   return p1.y < p2.y || (p1.y == p2.y && p1.x <= p2.x);
}

/*
  The following STAY HERE until we have a working wxGTK again!!!
*/
#ifndef wxWANTS_CHARS
/// allows me to compare to wxPoints
bool operator ==(wxPoint const &p1, wxPoint const &p2)
{
   return p1.x == p2.x && p1.y == p2.y;
}

/// allows me to compare to wxPoints
bool operator !=(wxPoint const &p1, wxPoint const &p2)
{
   return p1.x != p2.x || p1.y != p2.y;
}

wxPoint & operator += (wxPoint &p1, wxPoint const &p2)
{
   p1.x += p2.x;
   p1.y += p2.y;
   return p1;
}
#endif // old wxGTK

/// allows me to compare to wxPoints
bool operator>(wxPoint const &p1, wxPoint const &p2)
{
   return !(p1 <= p2);
}

/// grows a wxRect so that it includes the given point

static
void GrowRect(wxRect &r, CoordType x, CoordType y)
{
   if(r.x > x)
      r.x = x;
   else if(r.x + r.width < x)
      r.width = x - r.x;

   if(r.y > y)
      r.y = y;
   else if(r.y + r.height < y)
      r.height = y - r.y;
}

#if 0
// unused
/// returns true if the point is in the rectangle
static
bool Contains(const wxRect &r, const wxPoint &p)
{
   return r.x <= p.x && r.y <= p.y && (r.x+r.width) >= p.x && (r.y + r.height) >= p.y;
}
#endif


//@}


void ReadString(wxString &to, wxString &from)
{
   to = "";
   const char *cptr = from.c_str();
   while(*cptr && *cptr != '\n')
      to += *cptr++;
   if(*cptr) cptr++;
   from = cptr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   wxLayoutObject

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* static */
wxLayoutObject *
wxLayoutObject::Read(wxString &istr)
{
   wxString tmp;
   ReadString(tmp, istr);
   int type = -1;
   sscanf(tmp.c_str(),"%d", &type);

   switch(type)
   {
   case WXLO_TYPE_TEXT:
      return wxLayoutObjectText::Read(istr);
   case WXLO_TYPE_CMD:
      return wxLayoutObjectCmd::Read(istr);
   case WXLO_TYPE_ICON:
      return wxLayoutObjectIcon::Read(istr);
   default:
      return NULL;
   }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   wxLayoutObjectText

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutObjectText::wxLayoutObjectText(const wxString &txt)
{
   m_Text = txt;
   m_Width = 0;
   m_Height = 0;
   m_Top = 0;
   m_Bottom = 0;
}

wxLayoutObject *
wxLayoutObjectText::Copy(void)
{
   wxLayoutObjectText *obj = new wxLayoutObjectText(m_Text);
   obj->m_Width = m_Width;
   obj->m_Height = m_Height;
   obj->m_Top = m_Top;
   obj->m_Bottom = m_Bottom;
   obj->SetUserData(m_UserData);
   return obj;
}


void
wxLayoutObjectText::Write(wxString &ostr)
{
   ostr << (int) WXLO_TYPE_TEXT << '\n'
        << m_Text << '\n';
}
/* static */
wxLayoutObjectText *
wxLayoutObjectText::Read(wxString &istr)
{
   wxString text;
   ReadString(text, istr);

   return new wxLayoutObjectText(text);
}

wxPoint
wxLayoutObjectText::GetSize(CoordType *top, CoordType *bottom) const
{

   *top = m_Top; *bottom = m_Bottom;
   return wxPoint(m_Width, m_Height);
}

void
wxLayoutObjectText::Draw(wxDC &dc, wxPoint const &coords,
                         wxLayoutList *wxllist,
                         CoordType begin, CoordType end)
{
   if( end <= 0 )
   {
      // draw the whole object normally
      dc.DrawText(m_Text, coords.x, coords.y-m_Top);
   }
   else
   {
      // highlight the bit between begin and len
      CoordType
         xpos = coords.x,
         ypos = coords.y-m_Top;
      long width, height, descent;

      if(begin < 0) begin = 0;
      if( end > (signed)m_Text.Length() )
         end = m_Text.Length();

      wxString str = m_Text.Mid(0, begin);
      dc.DrawText(str, xpos, ypos);
      dc.GetTextExtent(str, &width, &height, &descent);
      xpos += width;
      wxllist->StartHighlighting(dc);
      str = m_Text.Mid(begin, end-begin);
      dc.DrawText(str, xpos, ypos);
      dc.GetTextExtent(str, &width, &height, &descent);
      xpos += width;
      wxllist->EndHighlighting(dc);
      str = m_Text.Mid(end, m_Text.Length()-end);
      dc.DrawText(str, xpos, ypos);
   }
}

CoordType
wxLayoutObjectText::GetOffsetScreen(wxDC &dc, CoordType xpos) const
{
   CoordType
      offs = 1,
      maxlen = m_Text.Length();
   long
      width = 0,
      height, descent = 0l;

   if(xpos == 0) return 0; // easy

   while(width < xpos && offs < maxlen)
   {
      dc.GetTextExtent(m_Text.substr(0,offs),
                       &width, &height, &descent);
      offs++;
   }
   /* We have to substract 1 to compensate for the offs++, and another
      one because we don't want to position the cursor behind the
      object what we clicked on, but before - otherwise it looks
      funny. */
   return (xpos > 2) ? offs-2 : 0;
}

void
wxLayoutObjectText::Layout(wxDC &dc, class wxLayoutList *llist)
{
   long descent = 0l;

   // now this is done in wxLayoutLine::Layout(), but this code might be
   // reenabled later - in principle, it's more efficient
#if 0
   CoordType widthOld = m_Width,
             heightOld = m_Height;
#endif // 0

#ifdef __WXDEBUG__
   CoordType a,b,c,d,e,f;
   dc.GetTextExtent("test ", &a, &b, &c);
   dc.GetTextExtent("test", &d, &e, &f);
   wxASSERT(a != d);
   wxASSERT(b == e);
   wxASSERT(c == f);
   dc.GetTextExtent(" ", &d, &e, &f);
   wxASSERT(a > 0);
#endif
   dc.GetTextExtent(m_Text, &m_Width, &m_Height, &descent);

#if 0
   if ( widthOld != m_Width || heightOld != m_Height )
   {
      // as the text length changed, it must be refreshed
      wxLayoutLine *line = GetLine();

      wxCHECK_RET( line, "wxLayoutObjectText can't refresh itself" );

      // as our size changed, we need to repaint the part which was appended
      wxPoint position(line->GetPosition());

      // this is not the most efficient way (we repaint the whole line), but
      // it's not too slow and is *simple*
      if ( widthOld < m_Width )
         widthOld = m_Width;
      if ( heightOld < m_Height )
         heightOld = m_Height;

      llist->SetUpdateRect(position.x + widthOld + MSW_CORRECTION,
                           position.y + heightOld + MSW_CORRECTION);
   }
#endif // 0

   m_Bottom = descent;
   m_Top = m_Height - m_Bottom;
}


#ifdef WXLAYOUT_DEBUG
void
wxLayoutObjectText::Debug(void)
{
   wxLayoutObject::Debug();
   WXLO_DEBUG((" `%s`", m_Text.c_str()));
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   wxLayoutObjectIcon

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutObjectIcon::wxLayoutObjectIcon(wxBitmap const &icon)
{
   if ( !icon.Ok() )
   {
      wxFAIL_MSG("invalid icon");

      m_Icon = NULL;

      return;
   }

#ifdef __WXMSW__
   // FIXME ugly, ugly, ugly - but the only way to avoid slicing
   m_Icon = icon.GetHBITMAP() ? new wxBitmap(icon)
                              : new wxBitmap(wxBitmap((const wxBitmap &)icon));
#else // !MSW
   m_Icon = new wxBitmap(icon);
#endif // MSW/!MSW
}


void
wxLayoutObjectIcon::Write(wxString &ostr)
{
   /* Exports icon through a temporary file. */

   wxString file = wxGetTempFileName("wxloexport");

   ostr << WXLO_TYPE_ICON << '\n'
        << file << '\n';
   m_Icon->SaveFile(file, WXLO_BITMAP_FORMAT);
}
/* static */
wxLayoutObjectIcon *
wxLayoutObjectIcon::Read(wxString &istr)
{
   wxString file;
   ReadString(file, istr);

   if(! wxFileExists(file))
      return NULL;
   wxLayoutObjectIcon *obj = new wxLayoutObjectIcon;

   if(!obj->m_Icon->LoadFile(file, WXLO_BITMAP_FORMAT))
   {
      delete obj;
      return NULL;
   }
   else
      return obj;
}

wxLayoutObject *
wxLayoutObjectIcon::Copy(void)
{
   wxLayoutObjectIcon *obj = new wxLayoutObjectIcon(new
                                                    wxBitmap(*m_Icon));
   obj->SetUserData(m_UserData);
   return obj;
}

wxLayoutObjectIcon::wxLayoutObjectIcon(wxBitmap *icon)
{
   m_Icon = icon;
}

void
wxLayoutObjectIcon::Draw(wxDC &dc, wxPoint const &coords,
                         wxLayoutList *wxllist,
                         CoordType begin, CoordType /* len */)
{
   dc.DrawBitmap(*m_Icon, coords.x, coords.y-m_Icon->GetHeight(),
                 (m_Icon->GetMask() == NULL) ? FALSE : TRUE);
}

void
wxLayoutObjectIcon::Layout(wxDC & /* dc */, class wxLayoutList * )
{
}

wxPoint
wxLayoutObjectIcon::GetSize(CoordType *top, CoordType *bottom) const
{
   *top = m_Icon->GetHeight();
   *bottom = 0;
   return wxPoint(m_Icon->GetWidth(), m_Icon->GetHeight());
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   wxLayoutObjectCmd

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


wxLayoutStyleInfo::wxLayoutStyleInfo(int ifamily,
                                     int isize,
                                     int istyle,
                                     int iweight,
                                     int iul,
                                     wxColour *fg,
                                     wxColour *bg)
{
   family = ifamily;
   size = isize;
   style = istyle;
   weight = iweight;
   underline = iul != 0;

   m_fg_valid = fg != 0;
   m_bg_valid = bg != 0;
   m_fg = m_fg_valid ? *fg : *wxBLACK;
   m_bg = m_bg_valid ? *bg : *wxWHITE;
}

#define COPY_SI_(what) if(right.what != -1) what = right.what;

wxLayoutStyleInfo &
wxLayoutStyleInfo::operator=(const wxLayoutStyleInfo &right)
{
   COPY_SI_(family);
   COPY_SI_(style);
   COPY_SI_(size);
   COPY_SI_(weight);
   COPY_SI_(underline);
   if(right.m_fg_valid) m_fg = right.m_fg;
   if(right.m_bg_valid) m_bg = right.m_bg;
   return *this;
}

wxLayoutObjectCmd::wxLayoutObjectCmd(int family, int size, int style, int
                                     weight, int underline,
                                     wxColour *fg, wxColour *bg)

{
   m_StyleInfo = new wxLayoutStyleInfo(family, size,style,weight,underline,fg,bg);
}

wxLayoutObject *
wxLayoutObjectCmd::Copy(void)
{
   wxLayoutObjectCmd *obj = new wxLayoutObjectCmd(
      m_StyleInfo->size,
      m_StyleInfo->family,
      m_StyleInfo->style,
      m_StyleInfo->weight,
      m_StyleInfo->underline,
      m_StyleInfo->m_fg_valid ?
      &m_StyleInfo->m_fg : NULL,
      m_StyleInfo->m_bg_valid ?
      &m_StyleInfo->m_bg : NULL);
   obj->SetUserData(m_UserData);
   return obj;
}

void
wxLayoutObjectCmd::Write(wxString &ostr)
{
   ostr << WXLO_TYPE_CMD << '\n'
        << m_StyleInfo->size << '\n'
        << m_StyleInfo->family << '\n'
        << m_StyleInfo->style << '\n'
        << m_StyleInfo->weight << '\n'
        << m_StyleInfo->underline << '\n'
        << m_StyleInfo->m_fg_valid << '\n'
        << m_StyleInfo->m_bg_valid << '\n';
   if(m_StyleInfo->m_fg_valid)
   {
      ostr << m_StyleInfo->m_fg.Red() << '\n'
           << m_StyleInfo->m_fg.Green() << '\n'
           << m_StyleInfo->m_fg.Blue() << '\n';
   }
   if(m_StyleInfo->m_bg_valid)
   {
      ostr << m_StyleInfo->m_bg.Red() << '\n'
           << m_StyleInfo->m_bg.Green() << '\n'
           << m_StyleInfo->m_bg.Blue() << '\n';
   }
}
/* static */
wxLayoutObjectCmd *
wxLayoutObjectCmd::Read(wxString &istr)
{
   wxLayoutObjectCmd *obj = new wxLayoutObjectCmd;

   wxString tmp;
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->size);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->family);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->style);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->weight);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->underline);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->m_fg_valid);
   ReadString(tmp, istr);
   sscanf(tmp.c_str(),"%d", &obj->m_StyleInfo->m_bg_valid);
   if(obj->m_StyleInfo->m_fg_valid)
   {
      int red, green, blue;
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &red);
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &green);
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &blue);
      obj->m_StyleInfo->m_fg = wxColour(red, green, blue);
   }
   if(obj->m_StyleInfo->m_bg_valid)
   {
      int red, green, blue;
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &red);
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &green);
      ReadString(tmp, istr);
      sscanf(tmp.c_str(),"%d", &blue);
      obj->m_StyleInfo->m_bg = wxColour(red, green, blue);
   }
   return obj;
}


wxLayoutObjectCmd::~wxLayoutObjectCmd()
{
   delete m_StyleInfo;
}

wxLayoutStyleInfo *
wxLayoutObjectCmd::GetStyle(void) const
{
   return m_StyleInfo;
}

void
wxLayoutObjectCmd::Draw(wxDC &dc, wxPoint const & /* coords */,
                        wxLayoutList *wxllist,
                        CoordType begin, CoordType /* len */)
{
   wxASSERT(m_StyleInfo);
   wxllist->ApplyStyle(*m_StyleInfo, dc);
}

void
wxLayoutObjectCmd::Layout(wxDC &dc, class wxLayoutList * llist)
{
   // this get called, so that recalculation uses right font sizes
   Draw(dc, wxPoint(0,0), llist);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   The wxLayoutLine object

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutLine::wxLayoutLine(wxLayoutLine *prev, wxLayoutList *llist)
{
   m_Width = m_Height = 0;
   m_Length = 0;

   m_updateLeft = -1;
   MarkDirty(0);

   m_Previous = prev;
   m_Next = NULL;

   m_LineNumber = 0;
   RecalculatePosition(llist);

   MarkDirty();
   if(m_Previous)
   {
      m_LineNumber = m_Previous->GetLineNumber() + 1;
      m_Next = m_Previous->GetNextLine();
      m_Previous->m_Next = this;
   }

   if(m_Next)
   {
      m_Next->m_Previous = this;
      m_Next->ReNumber();
   }

   m_StyleInfo = llist->GetDefaultStyleInfo();

   llist->IncNumLines();
}

wxLayoutLine::~wxLayoutLine()
{
   // kbList cleans itself
}

wxPoint
wxLayoutLine::RecalculatePosition(wxLayoutList *llist)
{
   wxASSERT(m_Previous || GetLineNumber() == 0);

   wxPoint posOld(m_Position);

   if(m_Previous)
   {
      m_Position = m_Previous->GetPosition();
      m_Position.y += m_Previous->GetHeight();
   }
   else
      m_Position = wxPoint(0,0);

   if ( m_Position != posOld )
   {
       // the whole line moved and must be repainted
       llist->SetUpdateRect(m_Position);
       llist->SetUpdateRect(m_Position.x + GetWidth() + MSW_CORRECTION,
                            m_Position.y + GetHeight() + MSW_CORRECTION);
       llist->SetUpdateRect(posOld);
       llist->SetUpdateRect(posOld.x + GetWidth() + MSW_CORRECTION,
                            posOld.y + GetHeight() + MSW_CORRECTION);
   }

   return m_Position;
}


wxLayoutObjectList::iterator
wxLayoutLine::FindObject(CoordType xpos, CoordType *offset) const
{
   wxASSERT(xpos >= 0);
   wxASSERT(offset);
   wxLayoutObjectList::iterator
      i,
      found = NULLIT;
   CoordType x = 0, len;

   /* We search through the objects. As we don't like returning the
      object that the cursor is behind, we just remember such an
      object in "found" so we can return it if there is really no
      further object following it. */
   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      len = (**i).GetLength();
      if( x <= xpos && xpos <= x + len )
      {
         *offset = xpos-x;
         if(xpos == x + len) // is there another object behind?
            found = i;
         else  // we are really inside this object
            return i;
      }
      x += (**i).GetLength();
   }
   return found;  // ==NULL if really none found
}

wxLayoutObjectList::iterator
wxLayoutLine::FindObjectScreen(wxDC &dc, wxLayoutList *llist,
                               CoordType xpos, CoordType *cxpos,
                               bool *found) const
{
   wxASSERT(cxpos);

   llist->ApplyStyle(GetStyleInfo(), dc);

   wxLayoutObjectList::iterator i;
   CoordType x = 0, cx = 0, width;

   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      wxLayoutObject *obj = *i;
      if ( obj->GetType() == WXLO_TYPE_CMD )
      {
         // this will set the correct font for the objects which follow
         obj->Layout(dc, llist);
      }

      width = obj->GetWidth();
      if( x <= xpos && xpos <= x + width )
      {
         *cxpos = cx + obj->GetOffsetScreen(dc, xpos-x);

         if ( found )
             *found = true;
         return i;
      }

      x += obj->GetWidth();
      cx += obj->GetLength();
   }

   // behind last object:
   *cxpos = cx;

   if (found)
       *found = false;
   return m_ObjectList.tail();
}

/** Finds text in this line.
    @param needle the text to find
    @param xpos the position where to start the search
    @return the cursoor coord where it was found or -1
*/
CoordType
wxLayoutLine::FindText(const wxString &needle, CoordType xpos) const
{
   int
      cpos = 0,
      relpos = -1;
   wxString const *text;

   for(wxLOiterator i = m_ObjectList.begin(); i != m_ObjectList.end(); i++)
   {
      if(cpos >= xpos) // search from here!
      {
         if((**i).GetType() == WXLO_TYPE_TEXT)
         {
            text = & ((wxLayoutObjectText*)(*i))->GetText();
            relpos = text->Find(needle);
            if(relpos >= cpos-xpos) // -1 if not found
            {
               return cpos+relpos;
            }
         }
         cpos += (**i).GetLength();
      }
   }
   return -1; // not found
}

bool
wxLayoutLine::Insert(CoordType xpos, wxLayoutObject *obj)
{
   wxASSERT(xpos >= 0);
   wxASSERT(obj != NULL);

   MarkDirty(xpos);

   CoordType offset;
   wxLOiterator i = FindObject(xpos, &offset);
   if(i == NULLIT)
   {
      if(xpos == 0 ) // aha, empty line!
      {
         m_ObjectList.push_back(obj);
         m_Length += obj->GetLength();
         return true;
      }
      else
         return false;
   }

   CoordType len = (**i).GetLength();
   if(offset == 0 /*&& i != m_ObjectList.begin()*/) // why?
   {  // insert before this object
      m_ObjectList.insert(i,obj);
      m_Length += obj->GetLength();
      return true;
   }
   if(offset == len )
   {
      if( i == m_ObjectList.tail()) // last object?
         m_ObjectList.push_back(obj);
      else
      {  // insert after current object
         i++;
         m_ObjectList.insert(i,obj);
      }
         m_Length += obj->GetLength();
      return true;
   }
   /* Otherwise we need to split the current object.
      Fortunately this can only be a text object. */
   wxASSERT((**i).GetType() == WXLO_TYPE_TEXT);
   wxString left, right;
   wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
   left = tobj->GetText().substr(0,offset);
   right = tobj->GetText().substr(offset,len-offset);
   // current text object gets set to right half
   tobj->GetText() = right; // set new text
   // before it we insert the new object
   m_ObjectList.insert(i,obj);
   m_Length += obj->GetLength();
   // and before that we insert the left half
   m_ObjectList.insert(i,new wxLayoutObjectText(left));
   return true;
}

bool
wxLayoutLine::Insert(CoordType xpos, const wxString& text)
{
   wxASSERT(xpos >= 0);

   MarkDirty(xpos);

   CoordType offset;
   wxLOiterator i = FindObject(xpos, &offset);
   if(i != NULLIT && (**i).GetType() == WXLO_TYPE_TEXT)
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
      tobj->GetText().insert(offset, text);
      m_Length += text.Length();
   }
   else
   {
      if ( !Insert(xpos, new wxLayoutObjectText(text)) )
         return false;
   }

   return true;
}

CoordType
wxLayoutLine::Delete(CoordType xpos, CoordType npos)
{
   CoordType offset, len;

   wxASSERT(xpos >= 0);
   wxASSERT(npos >= 0);
   MarkDirty(xpos);
   wxLOiterator i = FindObject(xpos, &offset);
   while(npos > 0)
   {
      if(i == NULLIT)  return npos;
      // now delete from that object:
      if((**i).GetType() != WXLO_TYPE_TEXT)
      {
         if(offset != 0) // at end of line after a non-text object
            return npos;
         // always len == 1:
         len = (**i).GetLength();
         m_Length -= len;
         npos -= len;
         m_ObjectList.erase(i);
      }
      else
      {
         // tidy up: remove empty text objects
         if((**i).GetLength() == 0)
         {
            m_ObjectList.erase(i);
            continue;
         }
         // Text object:
         CoordType max = (**i).GetLength() - offset;
         if(npos < max) max = npos;
         if(max == 0)
         {
            if(xpos == GetLength())
               return npos;
            else
            {  // at    the end of an object
               // move to    begin of next object:
               i++; offset = 0;
               continue; // start over
            }
         }
         npos -= max;
         m_Length -= max;
         if(offset == 0 && max == (**i).GetLength())
            m_ObjectList.erase(i);  // remove the whole object
         else
            ((wxLayoutObjectText *)(*i))->GetText().Remove(offset,max);
      }
   }

   return npos;
}

bool
wxLayoutLine::DeleteWord(CoordType xpos)
{
   wxASSERT(xpos >= 0);
   CoordType offset;
   MarkDirty(xpos);

   wxLOiterator i = FindObject(xpos, &offset);

   for(;;)
   {
      if(i == NULLIT) return false;
      if((**i).GetType() != WXLO_TYPE_TEXT)
      {
         // This should only happen when at end of line, behind a non-text
         // object:
         if(offset == (**i).GetLength()) return false;
         m_Length -= (**i).GetLength(); // -1
         m_ObjectList.erase(i);
         return true; // we are done
      }
      else
      {  // text object:
         if(offset == (**i).GetLength()) // at end of object
         {
            i++; offset = 0;
            continue;
         }
         wxLayoutObjectText *tobj = (wxLayoutObjectText *)*i;
         size_t count = 0;
         wxString str = tobj->GetText();
         str = str.substr(offset,str.Length()-offset);
         // Find out how many positions we need to delete:
         // 1. eat leading space
         while(isspace(str.c_str()[count])) count++;
         // 2. eat the word itself:
         while(isalnum(str.c_str()[count])) count++;
         // now delete it:
         wxASSERT(count+offset <= (size_t) (**i).GetLength());
         ((wxLayoutObjectText *)*i)->GetText().erase(offset,count);
         m_Length -= count;
         return true;
      }
   }

   wxFAIL_MSG("unreachable");
}

wxLayoutLine *
wxLayoutLine::DeleteLine(bool update, wxLayoutList *llist)
{
   // maintain linked list integrity
   if(m_Next)
       m_Next->m_Previous = m_Previous;
   if(m_Previous)
       m_Previous->m_Next = m_Next;

   // get the line numbers right again
   if ( update && m_Next)
      m_Next->ReNumber();

   MarkDirty();

   // we can't use m_Next after "delete this", so we must save this pointer
   // first
   wxLayoutLine *next = m_Next;
   delete this;

   llist->DecNumLines();

   return next;
}

void
wxLayoutLine::Draw(wxDC &dc,
                   wxLayoutList *llist,
                   const wxPoint & offset) const
{
   wxLayoutObjectList::iterator i;
   wxPoint pos = offset;
   pos = pos + GetPosition();

   pos.y += m_BaseLine;

   CoordType xpos = 0; // cursorpos, lenght of line

   CoordType from, to, tempto;

   int highlight = llist->IsSelected(this, &from, &to);
//   WXLO_DEBUG(("highlight=%d",  highlight ));
   if(highlight == 1) // we need to draw the whole line inverted!
      llist->StartHighlighting(dc);
   else
      llist->EndHighlighting(dc);

   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      if(highlight == -1) // partially highlight line
      {
         // parts of the line need highlighting
         tempto = xpos+(**i).GetLength();
         (**i).Draw(dc, pos, llist, from-xpos, to-xpos);
      }
      else
         (**i).Draw(dc, pos, llist);
      pos.x += (**i).GetWidth();
      xpos += (**i).GetLength();
   }
}

/*
  This function does all the recalculation, that is, it should only be
  called from within wxLayoutList::Layout(), as it uses the current
  list's styleinfo and updates it.
*/
void
wxLayoutLine::Layout(wxDC &dc,
                     wxLayoutList *llist,
                     wxPoint *cursorPos,
                     wxPoint *cursorSize,
                     wxLayoutStyleInfo *cursorStyle,
                     int cx,
                     bool suppressSIupdate)
{
   wxLayoutObjectList::iterator i;

   // when a line becomes dirty, we redraw it from the place where it was
   // changed till the end of line (because the following wxLayoutObjects are
   // moved when the preceding one changes) - calculate the update rectangle.
   CoordType updateTop = m_Position.y,
             updateLeft = -1,
             updateWidth = m_Width,
             updateHeight = m_Height;

   CoordType
      topHeight = 0,
      bottomHeight = 0;  // above and below baseline
   CoordType
      objTopHeight, objBottomHeight; // above and below baseline
   CoordType
      len, count = 0;

   CoordType heightOld = m_Height;

   m_Height = 0;
   m_Width = 0;
   m_BaseLine = 0;

   bool cursorFound = false;

   RecalculatePosition(llist);
   
   if(cursorPos)
   {
      *cursorPos = m_Position;
      if(cursorSize) *cursorSize = wxPoint(0,0);
   }

   m_StyleInfo = llist->GetStyleInfo(); // save current style
   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      wxLayoutObject *obj = *i;
      obj->Layout(dc, llist);
      wxPoint sizeObj = obj->GetSize(&objTopHeight, &objBottomHeight);

      if(cursorPos && ! cursorFound)
      {
         // we need to check whether the text cursor is here
         len = obj->GetLength();
         if(count <= cx && count+len > cx)
         {
            if(obj->GetType() == WXLO_TYPE_TEXT)
            {
               len = cx - count; // pos in object
               CoordType width, height, descent;
               dc.GetTextExtent((*(wxLayoutObjectText*)*i).GetText().substr(0,len),
                                &width, &height, &descent);
               cursorPos->x += width;
               cursorPos->y = m_Position.y;
               wxString str;
               if(len < obj->GetLength())
                  str = (*(wxLayoutObjectText*)*i).GetText().substr(len,1);
               else
                  str = WXLO_CURSORCHAR;
               dc.GetTextExtent(str, &width, &height, &descent);

               if(cursorStyle) // set style info
                  *cursorStyle = llist->GetStyleInfo();
               if ( cursorSize )
               {
                  // Just in case some joker inserted an empty string object:
                  if(width == 0)
                     width = WXLO_MINIMUM_CURSOR_WIDTH;
                  if(height == 0)
                     height = sizeObj.y;
                  cursorSize->x = width;
                  cursorSize->y = height;
               }
               
               cursorFound = true; // no more checks
            }
            else
            {
               // on some other object
               CoordType top, bottom; // unused
               if(cursorSize)
                  *cursorSize = obj->GetSize(&top,&bottom);
               cursorPos->y = m_Position.y;
               cursorFound = true; // no more checks
            }
         }
         else
         {
            count += len;
            cursorPos->x += obj->GetWidth();
         }
      } // cursor finding

      m_Width += sizeObj.x;
      if(sizeObj.y > m_Height)
      {
         m_Height = sizeObj.y;
      }

      if(objTopHeight > topHeight)
         topHeight = objTopHeight;
      if(objBottomHeight > bottomHeight)
         bottomHeight = objBottomHeight;
   }

   if ( IsDirty() )
   {
      if ( updateHeight < m_Height )
         updateHeight = m_Height;
      if ( updateWidth < m_Width )
         updateWidth = m_Width;

      // update all line if we don't know where to start from
      if ( updateLeft == -1 )
          updateLeft = 0;

      llist->SetUpdateRect(updateLeft, updateTop);
      llist->SetUpdateRect(updateLeft + updateWidth + MSW_CORRECTION,
                           updateTop + updateHeight + MSW_CORRECTION);
   }

   if(topHeight + bottomHeight > m_Height)
   {
      m_Height = topHeight+bottomHeight;
   }

   m_BaseLine = topHeight;

   if(m_Height == 0)
   {
      CoordType width, height, descent;
      dc.GetTextExtent(WXLO_CURSORCHAR, &width, &height, &descent);
      m_Height = height;
      m_BaseLine = m_Height - descent;
   }

   // tell next line about coordinate change
   if(m_Next && m_Height != heightOld)
   {
      m_Next->MarkDirty();
   }
   
   // We need to check whether we found a valid cursor size:
   if(cursorPos && cursorSize)
   {
      // this might be the case if the cursor is at the end of the
      // line or on a command object:
      if(cursorSize->x < WXLO_MINIMUM_CURSOR_WIDTH)
      {
         CoordType width, height, descent;
         dc.GetTextExtent(WXLO_CURSORCHAR, &width, &height, &descent);
         cursorSize->x = width;
         cursorSize->y = height;
      }
      if(m_BaseLine >= cursorSize->y) // the normal case anyway
         cursorPos->y += m_BaseLine-cursorSize->y;
   }
   MarkClean();
}


wxLayoutLine *
wxLayoutLine::Break(CoordType xpos, wxLayoutList *llist)
{
   wxASSERT(xpos >= 0);

   MarkDirty(xpos);

   CoordType offset;
   wxLOiterator i = FindObject(xpos, &offset);
   if(i == NULLIT)
      // must be at the end of the line then
      return new wxLayoutLine(this, llist);
   // split this line:

   wxLayoutLine *newLine = new wxLayoutLine(this, llist);
   // split object at i:
   if((**i).GetType() == WXLO_TYPE_TEXT && offset != 0)
   {
      wxString left, right;
      wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
      left = tobj->GetText().substr(0,offset);
      right = tobj->GetText().substr(offset,tobj->GetLength()-offset);
      // current text object gets set to left half
      tobj->GetText() = left; // set new text
      newLine->Append(new wxLayoutObjectText(right));
      m_Length -= right.Length();
      i++; // don't move this object to the new list
   }
   else
   {
      if(offset > 0)
         i++; // move objects from here to new list
   }

   while(i != m_ObjectList.end())
   {
      wxLayoutObject *obj = *i;
      newLine->Append(obj);
      m_Length -= obj->GetLength();

      m_ObjectList.remove(i); // remove without deleting it
   }
   if(m_Next)
      m_Next->MarkDirty();
   return newLine;
}

void
wxLayoutLine::ReNumber(void)
{
   CoordType lineNo = m_Previous ? m_Previous->m_LineNumber+1 : 0;
   m_LineNumber = lineNo++;
   
   for(wxLayoutLine *next = GetNextLine();
       next; next = next->GetNextLine())
      next->m_LineNumber = lineNo++;
}

void
wxLayoutLine::MergeNextLine(wxLayoutList *llist)
{
   wxCHECK_RET(GetNextLine(),"wxLayout internal error: no next line to merge");
   wxLayoutObjectList &list = GetNextLine()->m_ObjectList;
   wxLOiterator i;

   MarkDirty(GetWidth());

   wxLayoutObject *last = NULL;
   for(i = list.begin(); i != list.end();)
   {
      wxLayoutObject *current = *i;

      // merge text objects together for efficiency
      if ( last && last->GetType() == WXLO_TYPE_TEXT &&
                   current->GetType() == WXLO_TYPE_TEXT )
      {
         wxLayoutObjectText *textObj = (wxLayoutObjectText *)last;
         wxString text(textObj->GetText());
         text += ((wxLayoutObjectText *)current)->GetText();
         textObj->SetText(text);

         list.erase(i); // remove and delete it
      }
      else
      {
         // just append the object "as was"
         Append(current);

         list.remove(i); // remove without deleting it
      }
   }
   wxASSERT(list.empty());

   wxLayoutLine *oldnext = GetNextLine();
   wxLayoutLine *nextLine = oldnext->GetNextLine();
   SetNext(nextLine);
   if ( nextLine )
   {
      nextLine->ReNumber();
   }
   else
   {
       // this is now done in Delete(), but if this function is ever called
       // from elsewhere, we might have to move refresh code back here (in
       // order not to duplicate it)
#if 0
       wxPoint pos(oldnext->GetPosition());
       llist->SetUpdateRect(pos);
       llist->SetUpdateRect(pos.x + oldnext->GetWidth() + MSW_CORRECTION,
                            pos.y + oldnext->GetHeight() + MSW_CORRECTION);
#endif // 0
   }

   llist->DecNumLines();

   delete oldnext;
}

CoordType
wxLayoutLine::GetWrapPosition(CoordType column)
{
   CoordType offset;
   wxLOiterator i = FindObject(column, &offset);
   if(i == NULLIT) return -1; // cannot wrap

   // go backwards through the list and look for space in text objects
   do
   {
      if((**i).GetType() == WXLO_TYPE_TEXT)
      {
         do
         {
            if( isspace(((wxLayoutObjectText*)*i)->GetText().c_str()[(size_t)offset]))
               return column;
            else
            {
               offset--;
               column--;
            }
         }while(offset != -1);
         i--;  // move on to previous object
      }
      else
      {
         column -= (**i).GetLength();
         i--;
      }
      if( i != NULLIT)
         offset = (**i).GetLength();
   }while(i != NULLIT);
   /* If we reached the begin of the list and have more than one
      object, that one is longer than the margin, so break behind
      it. */
   CoordType pos = 0;
   i = m_ObjectList.begin();
   while(i != NULLIT && (**i).GetType() != WXLO_TYPE_TEXT)
   {
      pos += (**i).GetLength();
      i++;
   }
   if(i == NULLIT) return -1;  //why should this happen?
   pos += (**i).GetLength();
   i++;
   while(i != NULLIT && (**i).GetType() != WXLO_TYPE_TEXT)
   {
            pos += (**i).GetLength();
            i++;
   }
   if(i == NULLIT) return -1;  //this is possible, if there is only one text object
   // now we are at the second text object:
   pos -= (**i).GetLength();
   return pos; // in front of it
}


#ifdef WXLAYOUT_DEBUG
void
wxLayoutLine::Debug(void)
{
   wxString tmp;
   wxPoint pos = GetPosition();
   WXLO_DEBUG(("Line %ld, Pos (%ld,%ld), Height %ld, BL %ld, Font: %d",
               (long int) GetLineNumber(),
               (long int) pos.x, (long int) pos.y,
               (long int) GetHeight(),
               (long int) m_BaseLine,
               (int) m_StyleInfo.family));
   if(m_ObjectList.begin() != NULLIT)
      (**m_ObjectList.begin()).Debug();

}
#endif

void
wxLayoutLine::Copy(wxLayoutList *llist,
                   CoordType from,
                   CoordType to)
{
   CoordType firstOffset, lastOffset;

   if(to == -1) to = GetLength();
   if(from == to) return;

   wxLOiterator first = FindObject(from, &firstOffset);
   wxLOiterator last  = FindObject(to, &lastOffset);

   // Common special case: only one object
   if( first != NULLIT && last != NULLIT && *first == *last )
   {
      if( (**first).GetType() == WXLO_TYPE_TEXT )
      {
         llist->Insert(new wxLayoutObjectText(
            ((wxLayoutObjectText
              *)*first)->GetText().substr(firstOffset,
                                          lastOffset-firstOffset))
            );
         return;
      }
      else // what can we do?
      {
         if(lastOffset > firstOffset) // i.e. +1 :-)
            llist->Insert( (**first).Copy() );
         return;
      }
   }

   // If we reach here, we can safely copy the whole first object from
   // the firstOffset position on:
   if((**first).GetType() == WXLO_TYPE_TEXT && firstOffset != 0)
   {
      llist->Insert(new wxLayoutObjectText(
         ((wxLayoutObjectText *)*first)->GetText().substr(firstOffset))
         );
   }
   else if(firstOffset == 0)
      llist->Insert( (**first).Copy() );
   // else nothing to copy :-(

   // Now we copy all objects before the last one:
   wxLOiterator i = first; i++;
   for( ; i != last; i++)
      llist->Insert( (**i).Copy() );

   // And now the last object:
   if(lastOffset != 0)
   {
      if( (**last).GetType() == WXLO_TYPE_TEXT )
      {
         llist->Insert(new wxLayoutObjectText(
            ((wxLayoutObjectText *)*last)->GetText().substr(0,lastOffset))
            );
      }
      else
         llist->Insert( (**last).Copy() );
   }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   The wxLayoutList object

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutList::wxLayoutList()
{
#ifdef WXLAYOUT_USE_CARET
   m_caret = NULL;
#endif // WXLAYOUT_USE_CARET

   m_numLines = 0;
   m_FirstLine = NULL;
   SetAutoFormatting(TRUE);
   ForceTotalLayout(TRUE);  // for the first time, do all
   InvalidateUpdateRect();
   Clear();
}

wxLayoutList::~wxLayoutList()
{
   SetAutoFormatting(FALSE);
   InternalClear();
   Empty();
   m_FirstLine->DeleteLine(false, this);

   wxASSERT_MSG( m_numLines == 0, "line count calculation broken" );
}

void
wxLayoutList::Empty(void)
{
   while(m_FirstLine)
      m_FirstLine = m_FirstLine->DeleteLine(false, this);

   m_CursorPos = wxPoint(0,0);
   m_CursorScreenPos = wxPoint(0,0);
   m_CursorSize = wxPoint(0,0);
   m_movedCursor = true;
   m_FirstLine = new wxLayoutLine(NULL, this); // empty first line
   m_CursorLine = m_FirstLine;
   InvalidateUpdateRect();
}


void
wxLayoutList::InternalClear(void)
{
   m_Selection.m_selecting = false;
   m_Selection.m_valid = false;

   m_DefaultStyleInfo.family = wxSWISS;
   m_DefaultStyleInfo.size = WXLO_DEFAULTFONTSIZE;
   m_DefaultStyleInfo.style = wxNORMAL;
   m_DefaultStyleInfo.weight = wxNORMAL;
   m_DefaultStyleInfo.underline = 0;
   m_DefaultStyleInfo.m_fg_valid = TRUE;
   m_DefaultStyleInfo.m_fg = *wxBLACK;
   m_DefaultStyleInfo.m_bg_valid = TRUE;
   m_DefaultStyleInfo.m_bg = *wxWHITE;

   m_CurrentStyleInfo = m_DefaultStyleInfo;
   m_CursorStyleInfo = m_DefaultStyleInfo;
}

void
wxLayoutList::SetFont(int family, int size, int style, int weight,
                      int underline, wxColour *fg,
                      wxColour *bg)
{
   if(family != -1)    m_CurrentStyleInfo.family = family;
   if(size != -1)      m_CurrentStyleInfo.size = size;
   if(style != -1)     m_CurrentStyleInfo.style = style;
   if(weight != -1)    m_CurrentStyleInfo.weight = weight;
   if(underline != -1) m_CurrentStyleInfo.underline = underline != 0;
   if(fg) m_CurrentStyleInfo.m_fg = *fg;
   if(bg) m_CurrentStyleInfo.m_bg = *bg;
   Insert(
      new wxLayoutObjectCmd(
         m_CurrentStyleInfo.family,
         m_CurrentStyleInfo.size,
         m_CurrentStyleInfo.style,
         m_CurrentStyleInfo.weight,
         m_CurrentStyleInfo.underline,
         fg, bg));
}

void
wxLayoutList::SetFont(int family, int size, int style, int weight,
                      int underline, char const *fg, char const *bg)

{
   wxColour
      *cfg = NULL,
      *cbg = NULL;

   if( fg )
      cfg = wxTheColourDatabase->FindColour(fg);
   if( bg )
      cbg = wxTheColourDatabase->FindColour(bg);

   SetFont(family,size,style,weight,underline,cfg,cbg);
}

void
wxLayoutList::Clear(int family, int size, int style, int weight,
                    int underline, wxColour *fg, wxColour *bg)
{
   InternalClear();
   m_DefaultStyleInfo = wxLayoutStyleInfo(family, size, style, weight,
                                        underline, fg, bg);
   m_CurrentStyleInfo = m_DefaultStyleInfo;

   // Empty() should be called after we set m_DefaultStyleInfo because
   // otherwise the style info for the first line (created in Empty()) would be
   // incorrect
   Empty();
}

wxPoint
wxLayoutList::FindText(const wxString &needle, const wxPoint &cpos) const
{
   int xpos;

   wxLayoutLine *line;
   for(line = m_FirstLine;
       line;
       line = line->GetNextLine())
   {
      if(line->GetLineNumber() >= cpos.y)
      {
         xpos = line->FindText(needle,
                               (line->GetLineNumber() == cpos.y) ?
                               cpos.x : 0);
         if(xpos != -1)
            return wxPoint(xpos, line->GetLineNumber());
      }
   }
   return wxPoint(-1,-1);
}


bool
wxLayoutList::MoveCursorTo(wxPoint const &p)
{
   AddCursorPosToUpdateRect();

   wxPoint cursorPosOld = m_CursorPos;

   wxLayoutLine *line = m_FirstLine;
   while(line && line->GetLineNumber() != p.y)
      line = line->GetNextLine();
   if(line && line->GetLineNumber() == p.y) // found it
   {
      m_CursorPos.y = p.y;
      m_CursorLine = line;
      CoordType len = line->GetLength();
      if(len >= p.x)
      {
         m_CursorPos.x = p.x;
      }
      else
      {
         m_CursorPos.x = len;
      }
   }

   m_movedCursor = m_CursorPos != cursorPosOld;

   return m_CursorPos == p;
}

bool
wxLayoutList::MoveCursorVertically(int n)
{
   AddCursorPosToUpdateRect();

   wxPoint cursorPosOld = m_CursorPos;

   bool rc;
   if(n  < 0) // move up
   {
      if(m_CursorLine == m_FirstLine) return false;
      while(n < 0 && m_CursorLine)
      {
         m_CursorLine = m_CursorLine->GetPreviousLine();
         m_CursorPos.y--;
         n++;
      }
      if(! m_CursorLine)
      {
         m_CursorLine = m_FirstLine;
         m_CursorPos.y = 0;
         rc = false;
      }
      else
      {
         if(m_CursorPos.x > m_CursorLine->GetLength())
            m_CursorPos.x = m_CursorLine->GetLength();
         rc = true;
      }
   }
   else // move down
   {
      wxLayoutLine *last = m_CursorLine;
      if(! m_CursorLine->GetNextLine()) return false;
      while(n > 0 && m_CursorLine)
      {
         n--;
         m_CursorPos.y ++;
         m_CursorLine = m_CursorLine->GetNextLine();
      }
      if(! m_CursorLine)
      {
         m_CursorLine = last;
         m_CursorPos.y --;
         rc = false;
      }
      else
      {
         if(m_CursorPos.x > m_CursorLine->GetLength())
            m_CursorPos.x = m_CursorLine->GetLength();
         rc = true;
      }
   }

   m_movedCursor = m_CursorPos != cursorPosOld;

   return rc;
}

bool
wxLayoutList::MoveCursorHorizontally(int n)
{
   AddCursorPosToUpdateRect();

   wxPoint cursorPosOld = m_CursorPos;

   int move;
   while(n < 0)
   {
      if(m_CursorPos.x == 0) // at begin of line
      {
         if(! MoveCursorVertically(-1))
            break;
         MoveCursorToEndOfLine();
         n++;
         continue;
      }
      move = -n;
      if(move > m_CursorPos.x) move = m_CursorPos.x;
      m_CursorPos.x -= move; n += move;
   }

   while(n > 0)
   {
      int len =  m_CursorLine->GetLength();
      if(m_CursorPos.x == len) // at end of line
      {
         if(! MoveCursorVertically(1))
            break;
         MoveCursorToBeginOfLine();
         n--;
         continue;
      }
      move = n;
      if( move >= len-m_CursorPos.x) move = len-m_CursorPos.x;
      m_CursorPos.x += move;
      n -= move;
   }

   m_movedCursor = m_CursorPos != cursorPosOld;

   return n == 0;
}

bool
wxLayoutList::MoveCursorWord(int n, bool untilNext)
{
   wxCHECK_MSG( m_CursorLine, false, "no current line" );
   wxCHECK_MSG( n == -1 || n == +1, false, "not implemented yet" );

   CoordType moveDistance = 0;
   CoordType offset;
   wxLayoutLine *lineCur = m_CursorLine;
   for ( wxLOiterator i = lineCur->FindObject(m_CursorPos.x, &offset);
         n != 0;
         n > 0 ? i++ : i-- )
   {
      if ( i == NULLIT )
      {
         if ( n > 0 )
         {
            // moving forward, pass to the first object of the next line
            moveDistance++;
            lineCur = lineCur->GetNextLine();
            if ( lineCur )
               i = lineCur->GetFirstObject();
         }
         else
         {
            // moving backwards, pass to the last object of the prev line
            moveDistance--;
            lineCur = lineCur->GetPreviousLine();
            if ( lineCur )
               i = lineCur->GetLastObject();
         }

         if ( i == NULLIT )
         {
            // moved to the end/beginning of text
            return false;
         }

         offset = -1;
      }

      wxLayoutObject *obj = *i;

      if ( offset == -1 )
      {
         // calculate offset: we are either at the very beginning or the very
         // end of the object, so it isn't very difficult (the only time when
         // offset is != -1 is for the very first iteration when its value is
         // returned by FindObject)
         if ( n > 0 )
            offset = 0;
         else
            offset = obj->GetLength();
      }

      if( obj->GetType() != WXLO_TYPE_TEXT )
      {
         // any visible non text objects count as one word
         if ( obj->IsVisibleObject() )
         {
            n > 0 ? n-- : n++;

            moveDistance += obj->GetLength();
         }
      }
      else // text object
      {
         wxLayoutObjectText *tobj = (wxLayoutObjectText *)obj;

         bool canAdvance = true;

         if ( offset == tobj->GetLength() )
         {
            // at end of object
            if ( n > 0 )
            {
               // can't move further in this text object
               canAdvance = false;

               // still should move over the object border
               moveDistance++;
               n--;
            }
            else if ( offset > 0 )
            {
               // offset is off by 1, make it a valid index
               offset--;
            }
         }

         if ( canAdvance )
         {
            const wxString& text = tobj->GetText();
            const char *start = text.c_str();
            const char *end = start + text.length();
            const char *p = start + offset;

            if ( n < 0 )
            {
               if ( offset > 0 )
                  p--;
            }

            // to the beginning/end of the next/prev word
            while ( p >= start && p < end && isspace(*p) )
            {
               n > 0 ? p++ : p--;
            }

            // go to the end/beginning of the word (in a broad sense...)
            while ( p >= start && p < end && !isspace(*p) )
            {
               n > 0 ? p++ : p--;
            }

            if ( n > 0 )
            {
               if ( untilNext )
               {
                  // now advance to the beginning of the next word
                  while ( isspace(*p) && p < end )
                     p++;
               }
            }
            else // backwards
            {
               // in these 2 cases we took 1 char too much
               if ( (p < start) || isspace(*p) )
               {
                  p++;
               }
            }

            CoordType moveDelta = p - start - offset;
            if ( (n < 0) && (offset == tobj->GetLength() - 1) )
            {
               // because we substracted 1 from offset in this case above, now
               // compensate for it
               moveDelta--;
            }

            if ( moveDelta != 0 )
            {
               moveDistance += moveDelta;

               n > 0 ? n-- : n++;
            }
         }
      }

      // except for the first iteration, offset is calculated in the beginning
      // of the loop
      offset = -1;
   }

   MoveCursorHorizontally(moveDistance);

   return true;
}

bool
wxLayoutList::Insert(wxString const &text)
{
   wxASSERT(m_CursorLine);
   wxASSERT_MSG( text.Find('\n') == wxNOT_FOUND, "use wxLayoutImportText!" );

   if ( !text )
       return true;

   AddCursorPosToUpdateRect();

   if ( !m_CursorLine->Insert(m_CursorPos.x, text) )
      return false;

   m_CursorPos.x += text.Length();

   m_movedCursor = true;

   if(m_AutoFormat)
      m_CursorLine->MarkDirty();

   return true;
}

bool
wxLayoutList::Insert(wxLayoutObject *obj)
{
   wxASSERT(m_CursorLine);

   if(! m_CursorLine)
      m_CursorLine = GetFirstLine();

   AddCursorPosToUpdateRect();

   m_CursorLine->Insert(m_CursorPos.x, obj);
   m_CursorPos.x += obj->GetLength();
   m_movedCursor = true;

   if(m_AutoFormat)
      m_CursorLine->MarkDirty();

   return true;
}

bool
wxLayoutList::Insert(wxLayoutList *llist)
{
   wxASSERT(llist);
   bool rc = TRUE;

   for(wxLayoutLine *line = llist->GetFirstLine();
       line;
       line = line->GetNextLine()
      )
   {
      for(wxLOiterator i = line->GetFirstObject();
          i != NULLIT;
          i++)
         rc |= Insert(*i);
      LineBreak();
   }
   return rc;
}

bool
wxLayoutList::LineBreak(void)
{
   wxASSERT(m_CursorLine);

   AddCursorPosToUpdateRect();

   wxPoint position(m_CursorLine->GetPosition());

   CoordType
      width = m_CursorLine->GetWidth(),
      height = m_CursorLine->GetHeight();

   m_CursorLine = m_CursorLine->Break(m_CursorPos.x, this);
   if(m_CursorLine->GetPreviousLine() == NULL)
      m_FirstLine = m_CursorLine;
   m_CursorPos.y++;
   m_CursorPos.x = 0;

   // The following code will produce a height which is guaranteed to
   // be too high: old lineheight + the height of both new lines.
   // We can probably drop the old line height and start with height = 
   // 0. FIXME
   wxLayoutLine *prev = m_CursorLine->GetPreviousLine();
   if(prev)
      height += prev->GetHeight();
   height += m_CursorLine->GetHeight();

   m_movedCursor = true;

   SetUpdateRect(position);
   SetUpdateRect(position.x + width + MSW_CORRECTION,
                 position.y + height + MSW_CORRECTION);

   return true;
}

bool
wxLayoutList::WrapLine(CoordType column)
{
   if(m_CursorPos.x <= column || column < 1)
      return false; // do nothing yet
   else
   {
      CoordType xpos = m_CursorLine->GetWrapPosition(column);
      if(xpos == -1)
         return false; // cannot break line
      //else:
      CoordType newpos = m_CursorPos.x - xpos - 1;
      m_CursorPos.x = xpos;

      AddCursorPosToUpdateRect();

      LineBreak();
      Delete(1); // delete the space
      m_CursorPos.x = newpos;

      m_CursorLine->MarkDirty();

      m_movedCursor = true;

      return true;
   }
}

bool
wxLayoutList::Delete(CoordType npos)
{
   wxCHECK_MSG(m_CursorLine, false, "can't delete in non existing line");

   if ( npos == 0 )
       return true;

   AddCursorPosToUpdateRect();

   // were other lines appended to this one (this is important to know because
   // this means that our width _increased_ as the result of deletion)
   bool wasMerged = false;

   // the size of the region to update
   CoordType totalHeight = m_CursorLine->GetHeight(),
             totalWidth = m_CursorLine->GetWidth();

   CoordType left;
   do
   {
      left = m_CursorLine->Delete(m_CursorPos.x, npos);

      if( left > 0 )
      {
         // More to delete, continue on next line.

         // First, check if line is empty:
         if(m_CursorLine->GetLength() == 0)
         {
            // in this case, updating could probably be optimised
#ifdef WXLO_DEBUG
            wxASSERT(DeleteLines(1) == 0);
#else
            DeleteLines(1);
#endif

            left--;
         }
         else
         {
            // Need to join next line
            if(! m_CursorLine->GetNextLine())
               break; // cannot
            else
            {
               wasMerged = true;
               wxLayoutLine *next = m_CursorLine->GetNextLine();
               if ( next )
               {
                  totalHeight += next->GetHeight();
                  totalWidth += next->GetWidth();

                  m_CursorLine->MergeNextLine(this);
                  left--;
               }
               else
               {
                  wxFAIL_MSG("can't delete all this");

                  return false;
               }
            }
         }
      }
   }
   while ( left> 0 );

   // we need to update the whole tail of the line and the lines which
   // disappeared
   if ( wasMerged )
   {
      wxPoint position(m_CursorLine->GetPosition());
      SetUpdateRect(position);
      SetUpdateRect(position.x + totalWidth + MSW_CORRECTION,
                    position.y + totalHeight + MSW_CORRECTION);
   }

   return left == 0;
}

int
wxLayoutList::DeleteLines(int n)
{
   wxASSERT(m_CursorLine);
   wxLayoutLine *line;

   AddCursorPosToUpdateRect();

   while(n > 0)
   {
      if(!m_CursorLine->GetNextLine())
      {  // we cannot delete this line, but we can clear it
         MoveCursorToBeginOfLine();
         DeleteToEndOfLine();
         if(m_AutoFormat)
            m_CursorLine->MarkDirty();
         return n-1;
      }
      //else:
      line = m_CursorLine;
      m_CursorLine = m_CursorLine->DeleteLine(true, this);
      n--;
      if(line == m_FirstLine) m_FirstLine = m_CursorLine;
      wxASSERT(m_FirstLine);
      wxASSERT(m_CursorLine);
   }
   if(m_AutoFormat)
      m_CursorLine->MarkDirty();
   return n;
}

void
wxLayoutList::Recalculate(wxDC &dc, CoordType bottom)
{
   if(! m_AutoFormat)
      return;
   wxLayoutLine *line = m_FirstLine;

   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   ApplyStyle(m_DefaultStyleInfo, dc);
   while(line)
   {
      line->RecalculatePosition(this); // so we don't need to do it all the time
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
}

wxPoint
wxLayoutList::GetCursorScreenPos(void) const
{
   return m_CursorScreenPos;
}

/*
  Is called before each Draw(). Now, it will re-layout all lines which
  have changed.
*/
void
wxLayoutList::Layout(wxDC &dc, CoordType bottom, bool forceAll,
                     wxPoint *cpos, wxPoint *csize)
{
   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   ApplyStyle(m_DefaultStyleInfo, dc);

   
   if(m_ReLayoutAll)
   {
      forceAll = TRUE;
      bottom = -1;
   }
   ForceTotalLayout(FALSE);
   

   // If one line was dirty, we need to re-calculate all
   // following lines, too.
   bool wasDirty = forceAll;
   wxLayoutLine *line = m_FirstLine;
   while(line)
   {
      if(! wasDirty)
         ApplyStyle(line->GetStyleInfo(), dc);
      if(
         // if any previous line was dirty, we need to layout all
         // following lines:   
         wasDirty
         // layout dirty lines:
         || line->IsDirty()
         // always layout the cursor line toupdate the cursor
         // position and size:
         || line == m_CursorLine
         // or if it's the line we are asked to look for:
         || (cpos && line->GetLineNumber() == cpos->y) 
         )
      {
         if(line->IsDirty())
            wasDirty = true;
         
         // The following Layout() calls will update our
         // m_CurrentStyleInfo if needed.
         if(line == m_CursorLine)
         {
            line->Layout(dc, this,
                         (wxPoint *)&m_CursorScreenPos,
                         (wxPoint *)&m_CursorSize,
                         &m_CursorStyleInfo,
                         m_CursorPos.x);
            // we cannot layout the line twice, so copy the coords:
            if(cpos && line ->GetLineNumber() == cpos->y)
            {
               *cpos = m_CursorScreenPos;
               if ( csize )
                  *csize = m_CursorSize;
            }
         } 
         else
            if(cpos && line->GetLineNumber() == cpos->y)
               line->Layout(dc, this,
                            cpos,
                            csize, NULL, cpos->x);
         else
            line->Layout(dc, this);
         // little condition to speed up redrawing:
         if(bottom != -1 && line->GetPosition().y > bottom)
            break;
      }
      line = line->GetNextLine();
   }
   
#ifndef WXLAYOUT_USE_CARET
   // can only be 0 if we are on the first line and have no next line
   wxASSERT(m_CursorSize.x != 0 || (m_CursorLine &&
                                    m_CursorLine->GetNextLine() == NULL &&
                                    m_CursorLine == m_FirstLine));
#endif // WXLAYOUT_USE_CARET
   AddCursorPosToUpdateRect();
}

wxPoint
wxLayoutList::GetScreenPos(wxDC &dc, const wxPoint &cpos, wxPoint *csize)
{
   wxPoint pos = cpos;
   Layout(dc, -1, false, &pos, csize);
   return pos;
}

void
wxLayoutList::Draw(wxDC &dc,
                   wxPoint const &offset,
                   CoordType top,
                   CoordType bottom,
                   bool clipStrictly)
{
   wxLayoutLine *line = m_FirstLine;

   if ( m_Selection.m_discarded )
   {
      // calculate them if we don't have them already
      if ( !m_Selection.HasValidScreenCoords() )
      {
         m_Selection.m_ScreenA = GetScreenPos(dc, m_Selection.m_CursorA);
         m_Selection.m_ScreenB = GetScreenPos(dc, m_Selection.m_CursorB);
      }

      // invalidate the area which was previousle selected - and which is not
      // selected any more
      SetUpdateRect(m_Selection.m_ScreenA);
      SetUpdateRect(m_Selection.m_ScreenB);

      m_Selection.m_discarded = false;
   }

   /* This call to Layout() will re-calculate and update all lines
      marked as dirty.
   */
   Layout(dc, bottom);
   
   ApplyStyle(m_DefaultStyleInfo, dc);
   wxBrush brush(m_CurrentStyleInfo.m_bg, wxSOLID);
   dc.SetBrush(brush);
   dc.SetBackgroundMode(wxTRANSPARENT);

   while(line)
   {
      // only draw if between top and bottom:
      if((top == -1 ||
          line->GetPosition().y + line->GetHeight() > top))
      {
         ApplyStyle(line->GetStyleInfo(), dc);
         // little condition to speed up redrawing:
         if( bottom != -1
             && line->GetPosition().y
                +(clipStrictly ? line->GetHeight() : 0) >= bottom)
            break;
         line->Draw(dc, this, offset);
      }
      line = line->GetNextLine();
   }
   InvalidateUpdateRect();

   WXLO_DEBUG(("Selection is %s : l%d,%ld/%ld,%ld",
               m_Selection.m_valid ? "valid" : "invalid",
               m_Selection.m_CursorA.x, m_Selection.m_CursorA.y,
               m_Selection.m_CursorB.x, m_Selection.m_CursorB.y));
}

wxLayoutObject *
wxLayoutList::FindObjectScreen(wxDC &dc, wxPoint const pos,
                               wxPoint *cursorPos,
                               bool *found)
{
   // First, find the right line:
   wxLayoutLine
      *line = m_FirstLine,
      *lastline = m_FirstLine;
   wxPoint p;

   ApplyStyle(m_DefaultStyleInfo, dc);
   while(line)
   {
      p = line->GetPosition();
      if(p.y <= pos.y && p.y+line->GetHeight() >= pos.y)
         break;
      lastline = line;
      line = line->GetNextLine();
   }

   bool didFind = line != NULL;

   if ( !line )
   {
      // use the last line:
      line = lastline;
   }

   if ( cursorPos )
       cursorPos->y = line->GetLineNumber();

   bool foundinline = true;
   long cx = 0;

   // Now, find the object in the line:
   wxLOiterator i;

   if (cursorPos)
   {
     i = line->FindObjectScreen(dc, this,
                                           pos.x,
                                           &cx,
                                           &foundinline);
     cursorPos->x = cx;
   }
   else
     i = line->FindObjectScreen(dc, this,
                                           pos.x,
                                           NULL,
                                           &foundinline);
   if ( found )
      *found = didFind && foundinline;

   return (i == NULLIT) ? NULL : *i;

}

wxPoint
wxLayoutList::GetSize(void) const
{
   wxLayoutLine
      *line = m_FirstLine,
      *last = line;
   if(! line)
      return wxPoint(0,0);

   wxPoint maxPoint(0,0);

   // find last line:
   while(line)
   {
      if(line->GetWidth() > maxPoint.x)
          maxPoint.x = line->GetWidth();
      last = line;
      line = line->GetNextLine();
   }

   maxPoint.y = last->GetPosition().y + last->GetHeight();

   // if the line was just added, its height would be 0 and we can't call
   // Layout() from here because we don't have a dc and we might be not drawing
   // at all, besides... So take the cursor height by default (taking 0 is bad
   // because then the scrollbars won't be resized and the new line won't be
   // shown at all)
   if ( last->IsDirty() )
   {
      if ( last->GetHeight() == 0 )
         maxPoint.y += m_CursorSize.y;
      if ( last->GetWidth() == 0 && maxPoint.x < m_CursorSize.x )
         maxPoint.x = m_CursorSize.x;
   }

   return maxPoint;
}


void
wxLayoutList::DrawCursor(wxDC &dc, bool active, wxPoint const &translate)
{
   if ( m_movedCursor )
      m_movedCursor = false;

   wxPoint coords(m_CursorScreenPos);
   coords += translate;

#ifdef WXLAYOUT_DEBUG
   WXLO_DEBUG(("Drawing cursor (%ld,%ld) at %ld,%ld, size %ld,%ld, line: %ld, len %ld",
               (long)m_CursorPos.x, (long)m_CursorPos.y,
               (long)coords.x, (long)coords.y,
               (long)m_CursorSize.x, (long)m_CursorSize.y,
               (long)m_CursorLine->GetLineNumber(),
               (long)m_CursorLine->GetLength()));

   wxLogStatus("Cursor is at (%d, %d)", m_CursorPos.x, m_CursorPos.y);
#endif

#ifdef WXLAYOUT_USE_CARET
   m_caret->Move(coords);
#else // !WXLAYOUT_USE_CARET

   wxASSERT(m_CursorSize.x >= WXLO_MINIMUM_CURSOR_WIDTH);
   dc.SetBrush(*wxWHITE_BRUSH);
   //FIXME: wxGTK XOR is borken at the moment!!!dc.SetLogicalFunction(wxXOR);
   dc.SetPen(wxPen(*wxBLACK,1,wxSOLID));
   if(active)
   {
      dc.SetLogicalFunction(wxXOR);
      dc.DrawRectangle(coords.x, coords.y,
                       m_CursorSize.x, m_CursorSize.y);
      SetUpdateRect(coords.x, coords.y);
      SetUpdateRect(coords.x+m_CursorSize.x, coords.y+m_CursorSize.y);
   }
   else
   {
      dc.SetLogicalFunction(wxCOPY);
      dc.DrawLine(coords.x, coords.y+m_CursorSize.y-1,
                  coords.x, coords.y);
      SetUpdateRect(coords.x, coords.y+m_CursorSize.y-1);
      SetUpdateRect(coords.x, coords.y);
   }
   dc.SetLogicalFunction(wxCOPY);
   //dc.SetBrush(wxNullBrush);
#endif // WXLAYOUT_USE_CARET/!WXLAYOUT_USE_CARET
}

void
wxLayoutList::SetUpdateRect(CoordType x, CoordType y)
{
   if(m_UpdateRectValid)
      GrowRect(m_UpdateRect, x, y);
   else
   {
      m_UpdateRect.x = x;
      m_UpdateRect.y = y;
      m_UpdateRect.width = 4; // large enough to avoid surprises from
      m_UpdateRect.height = 4;// wxGTK :-)
      m_UpdateRectValid = true;
   }
}

void
wxLayoutList::StartSelection(const wxPoint& cposOrig, const wxPoint& spos)
{
   wxPoint cpos(cposOrig);
   if ( cpos.x == -1 )
      cpos = m_CursorPos;
   WXLO_DEBUG(("Starting selection at %ld/%ld", cpos.x, cpos.y));
   m_Selection.m_CursorA = cpos;
   m_Selection.m_CursorB = cpos;
   m_Selection.m_ScreenA = spos;
   m_Selection.m_ScreenB = spos;
   m_Selection.m_selecting = true;
   m_Selection.m_valid = false;
}

void
wxLayoutList::ContinueSelection(const wxPoint& cposOrig, const wxPoint& spos)
{
   wxPoint cpos(cposOrig);
   if(cpos.x == -1)
      cpos = m_CursorPos;

   wxASSERT(m_Selection.m_selecting == true);
   wxASSERT(m_Selection.m_valid == false);
   WXLO_DEBUG(("Continuing selection at %ld/%ld", cpos.x, cpos.y));

   m_Selection.m_ScreenB = spos;
   m_Selection.m_CursorB = cpos;
}

void
wxLayoutList::EndSelection(const wxPoint& cposOrig, const wxPoint& spos)
{
   wxPoint cpos(cposOrig);
   if(cpos.x == -1) cpos = m_CursorPos;
   ContinueSelection(cpos, spos);
   WXLO_DEBUG(("Ending selection at %ld/%ld", cpos.x, cpos.y));
   // we always want m_CursorA <= m_CursorB!
   if( m_Selection.m_CursorA > m_Selection.m_CursorB )
   {
      // exchange the start/end points
      wxPoint help = m_Selection.m_CursorB;
      m_Selection.m_CursorB = m_Selection.m_CursorA;
      m_Selection.m_CursorA = help;

      help = m_Selection.m_ScreenB;
      m_Selection.m_ScreenB = m_Selection.m_ScreenA;
      m_Selection.m_ScreenA = help;
   }
   m_Selection.m_selecting = false;
   m_Selection.m_valid = true;
   /// In case we just clicked somewhere, the selection will have zero 
   /// size, so we discard it immediately.
   if(m_Selection.m_CursorA == m_Selection.m_CursorB)
      DiscardSelection();
}

void
wxLayoutList::DiscardSelection()
{
   if ( !HasSelection() )
      return;

   m_Selection.m_valid =
   m_Selection.m_selecting = false;
   m_Selection.m_discarded = true;
}

bool
wxLayoutList::IsSelecting(void) const
{
   return m_Selection.m_selecting;
}

bool
wxLayoutList::IsSelected(const wxPoint &cursor) const
{
   if ( !HasSelection() )
      return false;

   return (
      (m_Selection.m_CursorA <= cursor
       && cursor <= m_Selection.m_CursorB) 
      || (m_Selection.m_CursorB <= cursor
          && cursor <= m_Selection.m_CursorA)
      );
}


/** Tests whether this layout line is selected and needs
    highlighting.
    @param line to test for
    @return 0 = not selected, 1 = fully selected, -1 = partially
    selected
    */
int
wxLayoutList::IsSelected(const wxLayoutLine *line, CoordType *from,
                         CoordType *to)
{
   wxASSERT(line); wxASSERT(to); wxASSERT(from);

   if(! m_Selection.m_valid && ! m_Selection.m_selecting)
      return 0;

   CoordType y = line->GetLineNumber();
   if(
      (m_Selection.m_CursorA.y < y && m_Selection.m_CursorB.y > y)
      || (m_Selection.m_CursorB.y < y && m_Selection.m_CursorA.y > y)
      )
      return 1;
   else if(m_Selection.m_CursorA.y == y)
   {
      *from = m_Selection.m_CursorA.x;
      if(m_Selection.m_CursorB.y == y)
         *to = m_Selection.m_CursorB.x;
      else
      {
         if(m_Selection.m_CursorB > m_Selection.m_CursorA)
            *to = line->GetLength();
         else
            *to = 0;
      }
      if(*to < *from)
      {
         CoordType help = *to;
         *to = *from;
         *from = help;
      }
      return -1;
   }
   else if(m_Selection.m_CursorB.y == y)
   {
      *to = m_Selection.m_CursorB.x;
      if(m_Selection.m_CursorA.y == y)
         *from = m_Selection.m_CursorA.x;
      else
      {
         if(m_Selection.m_CursorB > m_Selection.m_CursorA)
            *from = 0;
         else
            *from = line->GetLength();
      }
      if(*to < *from)
      {
         CoordType help = *to;
         *to = *from;
         *from = help;
      }
      return -1;
   }
   else
      return 0;
}

void
wxLayoutList::DeleteSelection(void)
{
   if(! m_Selection.m_valid)
      return;

   m_Selection.m_valid = false;

   // Only delete part of the current line?
   if(m_Selection.m_CursorA.y == m_Selection.m_CursorB.y)
   {
      MoveCursorTo(m_Selection.m_CursorA);
      Delete(m_Selection.m_CursorB.x - m_Selection.m_CursorA.x);
      return;
   }

   // We now know that the two lines are different:

   wxLayoutLine
      * firstLine = GetLine(m_Selection.m_CursorA.y),
      * lastLine = GetLine(m_Selection.m_CursorB.y);
   // be a bit paranoid:
   if(! firstLine || ! lastLine)
      return;
   
   // First, delete what's left of this line:
   MoveCursorTo(m_Selection.m_CursorA);
   DeleteToEndOfLine();

   wxLayoutLine *prevLine = firstLine->GetPreviousLine(),
                *nextLine = firstLine->GetNextLine();
   while(nextLine && nextLine != lastLine)
      nextLine = nextLine->DeleteLine(false, this);

   // Now nextLine = lastLine;
   Delete(1); // This joins firstLine and nextLine
   Delete(m_Selection.m_CursorB.x); // This deletes the first x positions

   // Recalculate the line positions and numbers but notice that firstLine
   // might not exist any more - it could be deleted by Delete(1) above
   wxLayoutLine *firstLine2 = prevLine ? prevLine->GetNextLine() : m_FirstLine;
   firstLine2->MarkDirty();
}

/// Starts highlighting the selection
void
wxLayoutList::StartHighlighting(wxDC &dc)
{
#if SHOW_SELECTIONS
   dc.SetTextForeground(m_CurrentStyleInfo.m_bg);
   dc.SetTextBackground(m_CurrentStyleInfo.m_fg);
   dc.SetBackgroundMode(wxSOLID);
#endif
}

/// Ends highlighting the selection
void
wxLayoutList::EndHighlighting(wxDC &dc)
{
#if SHOW_SELECTIONS
   dc.SetTextForeground(m_CurrentStyleInfo.m_fg);
   dc.SetTextBackground(m_CurrentStyleInfo.m_bg);
   dc.SetBackgroundMode(wxTRANSPARENT);
#endif
}


wxLayoutLine *
wxLayoutList::GetLine(CoordType index) const
{
   wxASSERT_MSG( (0 <= index) && (index < (CoordType)m_numLines),
                 "invalid index" );

   wxLayoutLine *line;
   CoordType n = index;
#ifdef DEBUG
   CoordType lineNo = 0;
#endif
       
   for ( line = m_FirstLine; line && n-- > 0; line =
            line->GetNextLine() )
   {
#ifdef DEBUG
wxASSERT(line->GetLineNumber() == lineNo );
      lineNo++;
#endif
}

   if ( line )
   {
      // should be the right one
      wxASSERT( line->GetLineNumber() == index );
   }

   return line;
}


wxLayoutList *
wxLayoutList::Copy(const wxPoint &from,
                   const wxPoint &to)
{
   wxLayoutLine
      * firstLine = NULL,
      * lastLine = NULL;

   for(firstLine = m_FirstLine;
       firstLine && firstLine->GetLineNumber() < from.y;
       firstLine=firstLine->GetNextLine())
      ;
   if(!firstLine || firstLine->GetLineNumber() != from.y)
      return NULL;

   for(lastLine = m_FirstLine;
       lastLine && lastLine->GetLineNumber() < to.y;
       lastLine=lastLine->GetNextLine())
      ;
   if(!lastLine || lastLine->GetLineNumber() != to.y)
      return NULL;

   if(to <= from)
   {
      wxLayoutLine *tmp = firstLine;
      firstLine = lastLine;
      lastLine = tmp;
   }

   wxLayoutList *llist = new wxLayoutList();

   if(firstLine == lastLine)
   {
      firstLine->Copy(llist, from.x, to.x);
   }
   else
   {
      // Extract objects from first line
      firstLine->Copy(llist, from.x);
      llist->LineBreak();
      // Extract all lines between
      for(wxLayoutLine *line = firstLine->GetNextLine();
          line != lastLine;
          line = line->GetNextLine())
      {
         line->Copy(llist);
         llist->LineBreak();
      }
      // Extract objects from last line
      lastLine->Copy(llist, 0, to.x);
   }
   return llist;
}

wxLayoutList *
wxLayoutList::GetSelection(wxLayoutDataObject *wxlo, bool invalidate)
{
   if(! m_Selection.m_valid)
   {
      if(m_Selection.m_selecting)
         EndSelection();
      else
         return NULL;
   }

   if(invalidate) m_Selection.m_valid = false;

   wxLayoutList *llist = Copy( m_Selection.m_CursorA,
                               m_Selection.m_CursorB );

   if(llist && wxlo) // export as data object, too
   {
      wxString string;

      wxLayoutExportObject *exp;
      wxLayoutExportStatus status(llist);
      while((exp = wxLayoutExport( &status, WXLO_EXPORT_AS_OBJECTS)) != NULL)
      {
         if(exp->type == WXLO_EXPORT_EMPTYLINE)
            ; //FIXME missing support for linebreaks in string format
         else
            exp->content.object->Write(string);
         delete exp;
      }

      wxlo->SetLayoutData(string);
   }
   return llist;
}



#define COPY_SI(what) if(si.what != -1) { m_CurrentStyleInfo.what = si.what; fontChanged = TRUE; }

void
wxLayoutList::ApplyStyle(wxLayoutStyleInfo const &si, wxDC &dc)
{
   bool fontChanged = FALSE;
   COPY_SI(family);
   COPY_SI(size);
   COPY_SI(style);
   COPY_SI(weight);
   COPY_SI(underline);
   if(fontChanged)
      dc.SetFont( m_FontCache.GetFont(m_CurrentStyleInfo) );

   if(si.m_fg_valid)
   {
      m_CurrentStyleInfo.m_fg = si.m_fg;
      dc.SetTextForeground(m_CurrentStyleInfo.m_fg);
   }
   if(si.m_bg_valid)
   {
      m_CurrentStyleInfo.m_bg = si.m_bg;
      dc.SetTextBackground(m_CurrentStyleInfo.m_bg);
   }
}


#ifdef WXLAYOUT_DEBUG

void
wxLayoutList::Debug(void)
{
   WXLO_DEBUG(("Cursor is in line %d, screen pos = (%d, %d)",
               m_CursorLine->GetLineNumber(),
               m_CursorScreenPos.x, m_CursorScreenPos.y));

   wxLayoutLine *line;
   for(line = m_FirstLine; line; line = line->GetNextLine())
   {
      line->Debug();
   }
}

#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   wxLayoutPrintout

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutPrintout::wxLayoutPrintout(wxLayoutList *llist,
                                   wxString const & title)
:wxPrintout(title)
{
   m_llist = llist;
   m_title = title;
   // remove any highlighting which could interfere with printing:
   m_llist->StartSelection();
   m_llist->EndSelection();
   // force a full layout of the list:
   m_llist->ForceTotalLayout();
   // layout  is called in ScaleDC() when we have a DC
}

wxLayoutPrintout::~wxLayoutPrintout()
{
}

float
wxLayoutPrintout::ScaleDC(wxDC *dc)
{
   // The following bit is taken from the printing sample, let's see
   // whether it works for us.

   /* You might use THIS code to set the printer DC to ROUGHLY reflect
    * the screen text size. This page also draws lines of actual length 5cm
    * on the page.
    */
  // Get the logical pixels per inch of screen and printer
   int ppiScreenX, ppiScreenY;
   GetPPIScreen(&ppiScreenX, &ppiScreenY);
   int ppiPrinterX, ppiPrinterY;
   GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

   if(ppiScreenX == 0) // not yet set, need to guess
   {
      ppiScreenX = 100;
      ppiScreenY = 100;
   }
   if(ppiPrinterX == 0) // not yet set, need to guess
   {
      ppiPrinterX = 72;
      ppiPrinterY = 72;
   }

  // This scales the DC so that the printout roughly represents the
  // the screen scaling. The text point size _should_ be the right size
  // but in fact is too small for some reason. This is a detail that will
  // need to be addressed at some point but can be fudged for the
  // moment.
  float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

  // Now we have to check in case our real page size is reduced
  // (e.g. because we're drawing to a print preview memory DC)
  int pageWidth, pageHeight;
  int w, h;
  dc->GetSize(&w, &h);
  GetPageSizePixels(&pageWidth, &pageHeight);
  if(pageWidth != 0) // doesn't work always
  {
     // If printer pageWidth == current DC width, then this doesn't
     // change. But w might be the preview bitmap width, so scale down.
     scale = scale * (float)(w/(float)pageWidth);
  }
  dc->SetUserScale(scale, scale);
  return scale;
}

bool wxLayoutPrintout::OnPrintPage(int page)
{
   wxDC *dc = GetDC();

   ScaleDC(dc);

   if (dc)
   {
      int top, bottom;
      top = (page - 1)*m_PrintoutHeight;
      bottom = top + m_PrintoutHeight; 

      WXLO_DEBUG(("OnPrintPage(%d) printing from %d to %d", page, top, 
                  bottom));
      // SetDeviceOrigin() doesn't work here, so we need to manually
      // translate all coordinates.
      wxPoint translate(m_Offset.x,m_Offset.y-top);
      m_llist->Draw(*dc, translate, top, bottom, TRUE /* clip strictly 
                                                       */);
      return true;
   }
   else
      return false;
}

void wxLayoutPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
   /* We allocate a temporary wxDC for printing, so that we can
      determine the correct paper size and scaling. We don't actually
      print anything on it. */
#ifdef __WXMSW__
   wxPrinterDC *psdc = new wxPrinterDC("","",WXLLIST_TEMPFILE,false);
#else
   wxPostScriptDC *psdc = new wxPostScriptDC(WXLLIST_TEMPFILE,false);
#endif

   psdc->StartDoc(m_title);
   // before we draw anything, me must make sure the list is properly
   // laid out
   m_llist->Layout(*psdc);

   float scale = ScaleDC(psdc);

   psdc->GetSize(&m_PageWidth, &m_PageHeight);

   // This sets a left/top origin of 15% and 5%:
   m_Offset = wxPoint((15*m_PageWidth)/100, (5*m_PageHeight)/100);

   // This is the length of the printable area.
   m_PrintoutHeight = m_PageHeight - 2*m_Offset.y;
   m_PrintoutHeight = (int)( m_PrintoutHeight / scale); // we want to use the real paper height

   m_NumOfPages = 1 +
      (int)( m_llist->GetSize().y / (float)(m_PrintoutHeight));

   *minPage = 1;
   *maxPage = m_NumOfPages;

   *selPageFrom = 1;
   *selPageTo = m_NumOfPages;
   psdc->EndDoc();
   delete psdc;
   wxRemoveFile(WXLLIST_TEMPFILE);
}

bool wxLayoutPrintout::HasPage(int pageNum)
{
   return pageNum <= m_NumOfPages;
}

/*
  Stupid wxWindows doesn't draw proper ellipses, so we comment this
  out. It's a waste of paper anyway.
*/
#if 0
void
wxLayoutPrintout::DrawHeader(wxDC &dc,
                             wxPoint topleft, wxPoint bottomright,
                             int pageno)
{
   // make backups of all essential parameters
   const wxBrush& brush = dc.GetBrush();
   const wxPen&   pen = dc.GetPen();
   const wxFont&  font = dc.GetFont();

   dc.SetBrush(*wxWHITE_BRUSH);
   dc.SetPen(wxPen(*wxBLACK,0,wxSOLID));
   dc.DrawRoundedRectangle(topleft.x,
                           topleft.y,bottomright.x-topleft.x,
                           bottomright.y-topleft.y);
   dc.SetBrush(*wxBLACK_BRUSH);
   wxFont myfont = wxFont((WXLO_DEFAULTFONTSIZE*12)/10,
                          wxSWISS,wxNORMAL,wxBOLD,false,"Helvetica");
   dc.SetFont(myfont);

   wxString page;
   page = "9999/9999  ";  // many pages...
   long w,h;
   dc.GetTextExtent(page,&w,&h);
   page.Printf("%d/%d", pageno, (int) m_NumOfPages);
   dc.DrawText(page,bottomright.x-w,topleft.y+h/2);
   dc.GetTextExtent("XXXX", &w,&h);
   dc.DrawText(m_title, topleft.x+w,topleft.y+h/2);

   // restore settings
   dc.SetPen(pen);
   dc.SetBrush(brush);
   dc.SetFont(font);
}
#endif


wxFont &
wxFontCache::GetFont(int family, int size, int style, int weight,
                     bool underline)
{
   for(wxFCEList::iterator i = m_FontList.begin();
       i != m_FontList.end(); i++)
      if( (**i).Matches(family, size, style, weight, underline) )
         return (**i).GetFont();
   // not found:
   wxFontCacheEntry *fce = new wxFontCacheEntry(family, size, style,
                                                weight, underline);
   m_FontList.push_back(fce);
   return fce->GetFont();
}

