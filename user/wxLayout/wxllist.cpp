/*-*- c++ -*-********************************************************
 * wxllist: wxLayoutList, a layout engine for text and graphics     *
 *                                                                  *
 * (C) 1998-1999 by Karsten Ballüder (Ballueder@usa.net)            *
 *                                                                  *
 * $Id$
 *******************************************************************/

/*
  
 */
 
#ifdef __GNUG__
#pragma implementation "wxllist.h"
#endif

//#include "Mpch.h"


#include "wx/wxprec.h"
#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifdef M_BASEDIR
#   include "gui/wxllist.h"
#   include "gui/wxMDialogs.h"
#else
#   include "wxllist.h"
#endif

#ifndef USE_PCH
#   include   "iostream.h"
#   include   <wx/dc.h>
#   include   <wx/dcps.h>
#   include   <wx/print.h>
#   include   <wx/log.h>
#endif

#include <ctype.h>

/// This should never really get created
#define   WXLLIST_TEMPFILE   "__wxllist.tmp"

#ifdef WXLAYOUT_DEBUG

#  define   TypewxString(t)        g_aTypewxStrings[t]
#  define   WXLO_DEBUG(x)      wxLogDebug x

   static const char *g_aTypewxStrings[] = 
   { 
      "invalid", "text", "cmd", "icon"
   };
   void
   wxLayoutObject::Debug(void)
   {
      WXLO_DEBUG(("%s",g_aTypewxStrings[GetType()])); 
   }
#else 
#  define   TypewxString(t)        ""
#  define   WXLO_DEBUG(x)      
#endif


/// Cursors smaller than this disappear in XOR drawing mode
#define WXLO_MINIMUM_CURSOR_WIDTH   4

/// Use this character to estimate a cursor size when none is available.
#define WXLO_CURSORCHAR   "E"
/** @name Helper functions */
//@{
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

/// allows me to compare to wxPoints
bool operator <=(wxPoint const &p1, wxPoint const &p2)
{
   return p1.y < p2.y || (p1.y == p2.y && p1.x <= p2.x);
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

/// returns true if the point is in the rectangle
static
bool Contains(const wxRect &r, const wxPoint &p)
{
   return r.x <= p.x && r.y <= p.y && (r.x+r.width) >= p.x && (r.y + r.height) >= p.y;
}
//@}

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

wxPoint
wxLayoutObjectText::GetSize(CoordType *top, CoordType *bottom) const
{

   *top = m_Top; *bottom = m_Bottom;
   return wxPoint(m_Width, m_Height);
}

void
wxLayoutObjectText::Draw(wxDC &dc, wxPoint const &coords)
{
   dc.DrawText(m_Text, coords.x, coords.y-m_Top);
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
wxLayoutObjectText::Layout(wxDC &dc)
{
   long descent = 0l;

   dc.GetTextExtent(m_Text,&m_Width, &m_Height, &descent);
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
   m_Icon = new wxBitmap(icon);
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
wxLayoutObjectIcon::Draw(wxDC &dc, wxPoint const &coords)
{
   dc.DrawBitmap(*m_Icon, coords.x, coords.y-m_Icon->GetHeight(),
                 (m_Icon->GetMask() == NULL) ? FALSE : TRUE);
}

void
wxLayoutObjectIcon::Layout(wxDC & /* dc */)
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

   wxLayoutObjectIcon

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutObjectCmd::wxLayoutObjectCmd(int size, int family, int style, int
                                     weight, bool underline,
                                     wxColour &fg, wxColour &bg)
   
{
   m_font = new wxFont(size,family,style,weight,underline);
   m_ColourFG = fg;
   m_ColourBG = bg;
}

wxLayoutObject *
wxLayoutObjectCmd::Copy(void)
{
   wxLayoutStyleInfo si;
   GetStyle(&si);
   
   wxLayoutObjectCmd *obj = new wxLayoutObjectCmd(
      si.size, si.family, si.style, si.weight, si.underline,
      m_ColourFG, m_ColourBG);
   obj->SetUserData(m_UserData);
   return obj;
}


wxLayoutObjectCmd::~wxLayoutObjectCmd()
{
   delete m_font;
}

void
wxLayoutObjectCmd::GetStyle(wxLayoutStyleInfo *si) const
{
   si->size = m_font->GetPointSize();
   si->family = m_font->GetFamily();
   si->style = m_font->GetStyle();
   si->underline = m_font->GetUnderlined();
   si->weight = m_font->GetWeight();

   si->fg_red = m_ColourFG.Red();
   si->fg_green = m_ColourFG.Green();
   si->fg_blue = m_ColourFG.Blue();
   si->bg_red = m_ColourBG.Red();
   si->bg_green = m_ColourBG.Green();
   si->bg_blue = m_ColourBG.Blue();
}

void
wxLayoutObjectCmd::Draw(wxDC &dc, wxPoint const & /* coords */)
{
   wxASSERT(m_font);
   dc.SetFont(*m_font);
   dc.SetTextForeground(m_ColourFG);
   dc.SetTextBackground(m_ColourBG);
}

void
wxLayoutObjectCmd::Layout(wxDC &dc)
{
   // this get called, so that recalculation uses right font sizes
   Draw(dc, wxPoint(0,0));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   The wxLayoutLine object

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutLine::wxLayoutLine(wxLayoutLine *prev, wxLayoutList *llist)
{
   m_LineNumber = 0;
   m_Width = m_Height = 0;
   m_Length = 0;
   m_Dirty = true;
   m_Previous = prev;
   m_Next = NULL;
   RecalculatePosition(llist);
   if(m_Previous)
   {
      m_LineNumber = m_Previous->GetLineNumber()+1;
      m_Next = m_Previous->GetNextLine();
      m_Previous->m_Next = this;
      m_Height = m_Previous->GetHeight();
   }
   if(m_Next)
   {
      m_Next->m_Previous = this;
      m_Next->MoveLines(+1);
      m_Next->RecalculatePositions(1,llist);
   }
}

wxLayoutLine::~wxLayoutLine()
{
   // kbList cleans itself
}

wxPoint
wxLayoutLine::RecalculatePosition(wxLayoutList *llist)
{
   if(m_Previous)
      m_Position = m_Previous->GetPosition() +
         wxPoint(0,m_Previous->GetHeight());
   else
      m_Position = wxPoint(0,0);
   llist->SetUpdateRect(m_Position);
   return m_Position;
}

void
wxLayoutLine::RecalculatePositions(int recurse, wxLayoutList *llist)
{
   wxASSERT(recurse >= 0);
   wxPoint pos = m_Position;
   CoordType height = m_Height;
   
//   WXLO_TRACE("RecalculatePositions()");
   RecalculatePosition(llist);
   if(m_Next)
   {
      if(recurse > 0)
         m_Next->RecalculatePositions(--recurse, llist);
      else if(pos != m_Position || m_Height != height)
         m_Next->RecalculatePositions(0, llist);         
   }
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
wxLayoutLine::FindObjectScreen(wxDC &dc,
                               CoordType xpos, CoordType *cxpos,
                               bool *found) const
{
   wxASSERT(cxpos);
   wxASSERT(cxpos);
   wxLayoutObjectList::iterator i;
   CoordType x = 0, cx = 0, width;
   
   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      (**i).Layout(dc);
      width = (**i).GetWidth();
      if( x <= xpos && xpos <= x + width )
      {
         *cxpos = cx + (**i).GetOffsetScreen(dc, xpos-x);
         wxLogDebug("wxLayoutLine::FindObjectScreen: cursor xpos = %ld", *cxpos);
         if(found) *found = true;
         return i;
      }
      x += (**i).GetWidth();
      cx += (**i).GetLength();
   }
   // behind last object:
   *cxpos = cx;
   if(found) *found = false;
   return m_ObjectList.tail();
}

bool
wxLayoutLine::Insert(CoordType xpos, wxLayoutObject *obj)
{
   wxASSERT(xpos >= 0);
   wxASSERT(obj != NULL);
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
wxLayoutLine::Insert(CoordType xpos, wxString text)
{
   wxASSERT(xpos >= 0);
   CoordType offset;
   wxLOiterator i = FindObject(xpos, &offset);
   if(i != NULLIT && (**i).GetType() == WXLO_TYPE_TEXT)
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
      tobj->GetText().insert(offset, text);
      m_Length += text.Length();

      return true;
   }
   else
      return Insert(xpos, new wxLayoutObjectText(text));
}

CoordType
wxLayoutLine::Delete(CoordType xpos, CoordType npos)
{
   CoordType offset, len;

   wxASSERT(xpos >= 0);
   wxASSERT(npos >= 0);
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
   wxASSERT(0); // we should never arrive here
}

wxLayoutLine *
wxLayoutLine::DeleteLine(bool update, wxLayoutList *llist)
{
   if(m_Next) m_Next->m_Previous = m_Previous;
   if(m_Previous) m_Previous->m_Next = m_Next;
   if(update)
   {
      m_Next->MoveLines(-1);
      m_Next->RecalculatePositions(1, llist);
   }
   wxLayoutLine *next = m_Next;
   delete this;
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
   
   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      (**i).Draw(dc, pos);
      pos.x += (**i).GetWidth();
   }
}

void
wxLayoutLine::Layout(wxDC &dc,
                     wxLayoutList *llist,
                     wxPoint *cursorPos,
                     wxPoint *cursorSize,
                     int cx) 
{
   wxLayoutObjectList::iterator i;

   CoordType
      oldHeight = m_Height;
   CoordType
      topHeight, bottomHeight;  // above and below baseline
   CoordType
      objHeight = 0,
      objTopHeight, objBottomHeight;
   CoordType
      len, count = 0;
   m_Height = 0; m_BaseLine = 0;
   m_Width = 0;
   topHeight = 0; bottomHeight = 0;
   wxPoint size;
   bool cursorFound = false;

   if(cursorPos)
   {
      *cursorPos = m_Position;
      if(cursorSize) *cursorSize = wxPoint(0,0);
   }
   
   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      (**i).Layout(dc);
      size = (**i).GetSize(&objTopHeight, &objBottomHeight);

      if(cursorPos && ! cursorFound)
      {  // we need to check whether the text cursor is here
         len = (**i).GetLength();
         if(count <= cx && count+len > cx)
         {
            if((**i).GetType() == WXLO_TYPE_TEXT)
            {
               len = cx - count; // pos in object
               CoordType width, height, descent;
               dc.GetTextExtent((*(wxLayoutObjectText*)*i).GetText().substr(0,len), 
                                &width, &height, &descent);
               cursorPos->x += width;
               cursorPos->y = m_Position.y;
               wxString str;
               if(len < (**i).GetLength())
                  str = (*(wxLayoutObjectText*)*i).GetText().substr(len,1);
               else
                  str = WXLO_CURSORCHAR;
               dc.GetTextExtent(str, &width, &height, &descent);
               wxASSERT(cursorSize);
               // Just in case some joker inserted an empty string object:
               if(width == 0) width = WXLO_MINIMUM_CURSOR_WIDTH;
               if(height == 0) height = objHeight;
               cursorSize->x = width;
               cursorSize->y = height;
               cursorFound = true; // no more checks
            }
            else 
            { // on some other object
               CoordType top, bottom; // unused
               *cursorSize = (**i).GetSize(&top,&bottom);
               cursorPos->y = m_Position.y;
               cursorFound = true; // no more checks
            }
         }
         else
         {
            count += len;
            cursorPos->x += (**i).GetWidth();
         }
      } // cursor finding
      objHeight = size.y;
      m_Width += size.x;
      if(objHeight > m_Height) m_Height = objHeight;
      if(objTopHeight > topHeight) topHeight = objTopHeight;
      if(objBottomHeight > bottomHeight) bottomHeight = objBottomHeight;
   }
   if(topHeight + bottomHeight > m_Height) m_Height =
                                              topHeight+bottomHeight;
   m_BaseLine = topHeight;

   if(m_Height == 0)
   {
      if(GetPreviousLine()) // empty line
      {
         m_Height = GetPreviousLine()->GetHeight();
         m_BaseLine = GetPreviousLine()->m_BaseLine;
      }
      else
      {
            CoordType width, height, descent;
            dc.GetTextExtent(WXLO_CURSORCHAR, &width, &height, &descent);
            m_Height = height;
            m_BaseLine = m_Height - descent;
      }
   }

   
   // tell next line about coordinate change
   if(m_Next && objHeight != oldHeight)
      m_Next->RecalculatePositions(0, llist);

   // We need to check whether we found a valid cursor size:
   if(cursorPos)
   {
      // this might be the case if the cursor is at the end of the
      // line or on a command object:
      if(cursorSize->y < WXLO_MINIMUM_CURSOR_WIDTH)
      {
         CoordType width, height, descent;
         dc.GetTextExtent(WXLO_CURSORCHAR, &width, &height, &descent);
         cursorSize->x = width;
         cursorSize->y = height;
      }
      if(m_BaseLine >= cursorSize->y) // the normal case anyway
         cursorPos->y += m_BaseLine-cursorSize->y;
   }
}


wxLayoutLine *
wxLayoutLine::Break(CoordType xpos, wxLayoutList *llist)
{
   wxASSERT(xpos >= 0);
   
   if(xpos == 0)
   { // insert an empty line before this one
      wxLayoutLine *prev = new wxLayoutLine(m_Previous, llist);
      if(m_Previous == NULL)
      {  // We were in first line, need to link in new empty line
         // before this.
         prev->m_Next = this;
         m_Previous = prev;
         m_Previous->m_Height = GetHeight(); // this is a wild guess
      }
      MoveLines(+1);
      if(m_Next)
         m_Next->RecalculatePositions(1, llist);
      return this;
   }
   
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
      if(offset > 0)
         i++; // move objects from here to new list

   while(i != m_ObjectList.end())
   {
      newLine->Append(*i);
      m_Length -= (**i).GetLength();
      m_ObjectList.remove(i); // remove without deleting it
   }
   if(m_Next)
      m_Next->RecalculatePositions(2, llist);
   return newLine;
}
   

void
wxLayoutLine::MergeNextLine(wxLayoutList *llist)
{
   wxASSERT(GetNextLine());
   wxLayoutObjectList &list = GetNextLine()->m_ObjectList;
   wxLOiterator i;
   
   for(i = list.begin(); i != list.end();)
   {
      Append(*i);
      list.remove(i); // remove without deleting it
   }
   wxASSERT(list.empty());
   wxLayoutLine *oldnext = GetNextLine();
   SetNext(GetNextLine()->GetNextLine());
   delete oldnext;
   RecalculatePositions(1, llist);
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
   tmp.Printf("Line %ld, Pos (%ld,%ld), Height %ld",
              (long int) GetLineNumber(),
              (long int) pos.x, (long int) pos.y,
              (long int) GetHeight());
              
   wxLogDebug(tmp);
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   
   The wxLayoutList object
   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutList::wxLayoutList()
{
   m_DefaultSetting = NULL;
   m_FirstLine = NULL;
   m_ColourFG = *wxBLACK;
   m_ColourBG = *wxWHITE;
   InvalidateUpdateRect();
   Clear();
}

wxLayoutList::~wxLayoutList()
{
   InternalClear();
   m_FirstLine->DeleteLine(false, this);
}

void
wxLayoutList::Empty(void)
{
   while(m_FirstLine)
      m_FirstLine = m_FirstLine->DeleteLine(false, this);

   m_CursorPos = wxPoint(0,0);
   m_CursorScreenPos = wxPoint(0,0);
   m_CursorSize = wxPoint(0,0);
   m_FirstLine = new wxLayoutLine(NULL, this); // empty first line
   m_CursorLine = m_FirstLine;
   InvalidateUpdateRect();
}


void
wxLayoutList::InternalClear(void)
{
   Empty();
   if(m_DefaultSetting)
   {
      delete m_DefaultSetting;
      m_DefaultSetting = NULL;
   }
}

void
wxLayoutList::SetFont(int family, int size, int style, int weight,
                      int underline, wxColour *fg,
                      wxColour *bg)
{
   if(family != -1)    m_FontFamily = family;
   if(size != -1)      m_FontPtSize = size;
   if(style != -1)     m_FontStyle = style;
   if(weight != -1)    m_FontWeight = weight;
   if(underline != -1) m_FontUnderline = underline != 0;

   if(fg != NULL)     m_ColourFG = *fg;
   if(bg != NULL)     m_ColourBG = *bg;
   
   Insert(
      new wxLayoutObjectCmd(m_FontPtSize,m_FontFamily,m_FontStyle,m_FontWeight,m_FontUnderline,
                            m_ColourFG, m_ColourBG));
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
                    int /* underline */, wxColour *fg, wxColour *bg)
{
   InternalClear();
   
   // set defaults
   m_FontPtSize = size;
   m_FontUnderline = false;
   m_FontFamily = family;
   m_FontStyle = style;
   m_FontWeight = weight;
   if(fg) m_ColourFG = *fg;
   if(bg) m_ColourBG = *bg;

   m_ColourFG = *wxBLACK;
   m_ColourBG = *wxWHITE;
   
   if(m_DefaultSetting)
      delete m_DefaultSetting;

   m_DefaultSetting = new
      wxLayoutObjectCmd(m_FontPtSize,m_FontFamily,m_FontStyle,
                        m_FontWeight,m_FontUnderline,
                        m_ColourFG, m_ColourBG);
}



bool
wxLayoutList::MoveCursorTo(wxPoint const &p)
{
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
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
         return true;
      }
      else
      {
         m_CursorPos.x = len;
         return false;
      }
   }
   return false;
}
   
bool
wxLayoutList::MoveCursorVertically(int n)
{
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
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
         m_CursorPos.y ++;
         rc = false;
      }
      else
      {
         if(m_CursorPos.x > m_CursorLine->GetLength())
            m_CursorPos.x = m_CursorLine->GetLength();
         rc = true;
      }
   }
   return rc;
}

bool
wxLayoutList::MoveCursorHorizontally(int n)
{
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
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
   return n == 0;
}

bool
wxLayoutList::Insert(wxString const &text)
{
   wxASSERT(m_CursorLine);
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
   m_CursorLine->Insert(m_CursorPos.x, text);
   m_CursorPos.x += text.Length();
   return true;
}

bool
wxLayoutList::Insert(wxLayoutObject *obj)
{
   wxASSERT(m_CursorLine);
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
   m_CursorLine->Insert(m_CursorPos.x, obj);
   m_CursorPos.x += obj->GetLength();
   return true;
}

bool
wxLayoutList::LineBreak(void)
{
   wxASSERT(m_CursorLine);
   bool setFirst = (m_CursorLine == m_FirstLine && m_CursorPos.x == 0);
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
   m_CursorLine = m_CursorLine->Break(m_CursorPos.x, this);
   if(setFirst) // we were at beginning of first line
      m_FirstLine = m_CursorLine->GetPreviousLine();
   m_CursorPos.y++;
   m_CursorPos.x = 0;
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
      SetUpdateRect(m_CursorScreenPos);
      SetUpdateRect(m_CursorScreenPos+m_CursorSize);
      LineBreak();
      Delete(1); // delete the space
      m_CursorPos.x = newpos;
      return true;
   }
}

bool
wxLayoutList::Delete(CoordType npos)
{
   wxASSERT(m_CursorLine);
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
   CoordType left;
   do
   {
      left = m_CursorLine->Delete(m_CursorPos.x, npos);
      if(left == 0)
         return true;
      // More to delete, continue on next line.
      // First, check if line is empty:
      if(m_CursorLine->GetLength() == 0)
      {  // in this case, updating could probably be optimised
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
            m_CursorLine->MergeNextLine(this);
            left--;
         }
      }
   }
   while(left);
   return left == 0;
}

int
wxLayoutList::DeleteLines(int n)
{
   wxASSERT(m_CursorLine);
   wxLayoutLine *line;
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
   while(n > 0)
   {
      if(!m_CursorLine->GetNextLine())
      {  // we cannot delete this line, but we can clear it
         MoveCursorToBeginOfLine();
         DeleteToEndOfLine();
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
   m_CursorLine->RecalculatePositions(2, this);
   return n;
}

void
wxLayoutList::Recalculate(wxDC &dc, CoordType bottom)
{
   wxLayoutLine *line = m_FirstLine;

   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   m_DefaultSetting->Layout(dc);
   while(line)
   {
      line->RecalculatePosition(this); // so we don't need to do it all the time
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
}

void
wxLayoutList::UpdateCursorScreenPos(wxDC &dc)
{
   wxASSERT(m_CursorLine);
   m_CursorLine->Layout(dc, this, (wxPoint *)&m_CursorScreenPos, (wxPoint *)&m_CursorSize, m_CursorPos.x);
}

wxPoint
wxLayoutList::GetCursorScreenPos(wxDC &dc)
{
   UpdateCursorScreenPos(dc);
   return m_CursorScreenPos;
}

void
wxLayoutList::Layout(wxDC &dc, CoordType bottom)
{
   wxLayoutLine *line = m_FirstLine;

   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   m_DefaultSetting->Layout(dc);
   while(line)
   {
      if(line == m_CursorLine)
         line->Layout(dc, this, (wxPoint *)&m_CursorScreenPos, (wxPoint *)&m_CursorSize, m_CursorPos.x);
      else
         line->Layout(dc, this);
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }

///FIXME: disabled for now
#if 0
   // can only be 0 if we are on the first line and have no next line
   wxASSERT(m_CursorSize.x != 0 || (m_CursorLine &&
                                    m_CursorLine->GetNextLine() == NULL &&
                                    m_CursorLine == m_FirstLine));
#endif
   SetUpdateRect(m_CursorScreenPos);
   SetUpdateRect(m_CursorScreenPos+m_CursorSize);
}

void
wxLayoutList::Draw(wxDC &dc,
                   wxPoint const &offset,
                   CoordType top,
                   CoordType bottom)
{
   wxLayoutLine *line = m_FirstLine;

   Layout(dc, bottom);
   m_DefaultSetting->Draw(dc, wxPoint(0,0));
   wxBrush brush(m_ColourBG, wxSOLID);
   dc.SetBrush(brush);
   
   while(line)
   {
      // only draw if between top and bottom:
      if((top == -1 || line->GetPosition().y + line->GetHeight() >= top))
         line->Draw(dc, this, offset);
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
   // can only be 0 if we are on the first line and have no next line
   wxASSERT(m_CursorSize.x != 0 || (m_CursorLine &&
                                    m_CursorLine->GetNextLine() == NULL &&
                                    m_CursorLine == m_FirstLine));
   InvalidateUpdateRect();
}

wxLayoutObject *
wxLayoutList::FindObjectScreen(wxDC &dc, wxPoint const pos,
                               wxPoint *cursorPos,
                               bool *found)
{
   // First, find the right line:
   wxLayoutLine *line = m_FirstLine;
   wxPoint p;
   
   // we need to run a layout here to get font sizes right :-(
   m_DefaultSetting->Layout(dc);
   while(line)
   {
      p = line->GetPosition();
      if(p.y <= pos.y && p.y+line->GetHeight() >= pos.y)
         break;
      line->Layout(dc, this);
      line = line->GetNextLine();
   }
   if(line == NULL)
   {
      if(found) *found = false;
      return NULL; // not found
   }
   if(cursorPos) cursorPos->y = line->GetLineNumber();
   // Now, find the object in the line:
   wxLOiterator i = line->FindObjectScreen(dc, pos.x,
                                           cursorPos ? & cursorPos->x : NULL ,
                                           found);
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
   return maxPoint;
}

void
wxLayoutList::DrawCursor(wxDC &dc, bool active, wxPoint const &translate)
{
   wxPoint coords;
   coords = m_CursorScreenPos;
   coords.x += translate.x;
   coords.y += translate.y;

#ifdef WXLAYOUT_DEBUG
   WXLO_DEBUG(("Drawing cursor (%ld,%ld) at %ld,%ld, size %ld,%ld, line: %ld, len %ld",
               (long)m_CursorPos.x, (long)m_CursorPos.y,
               (long)coords.x, (long)coords.y,
               (long)m_CursorSize.x, (long)m_CursorSize.y,
               (long)m_CursorLine->GetLineNumber(),
               (long)m_CursorLine->GetLength()));
#endif
   
   dc.SetBrush(*wxBLACK_BRUSH);
   dc.SetLogicalFunction(wxXOR);
   dc.SetPen(wxPen(*wxBLACK,1,wxSOLID));
   if(active)
      dc.DrawRectangle(coords.x, coords.y, m_CursorSize.x,
                       m_CursorSize.y);
   else
      dc.DrawLine(coords.x, coords.y+m_CursorSize.y-1,
                  coords.x+m_CursorSize.x, coords.y+m_CursorSize.y-1);
   dc.SetLogicalFunction(wxCOPY);
   //dc.SetBrush(wxNullBrush);
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
wxLayoutList::StartSelection(void)
{
   wxLogDebug("Starting selection at %ld/%ld", m_CursorPos.x, m_CursorPos.y);
   m_Selection.m_CursorA = m_CursorPos;
   m_Selection.m_selecting = true;
   m_Selection.m_valid = false;
}

void
wxLayoutList::EndSelection(void)
{
   wxLogDebug("Ending selection at %ld/%ld", m_CursorPos.x, m_CursorPos.y);
   m_Selection.m_CursorB = m_CursorPos;
   m_Selection.m_selecting = false;
   m_Selection.m_valid = true;
}

bool
wxLayoutList::IsSelecting(void)
{
   return m_Selection.m_selecting;
}

bool
wxLayoutList::IsSelected(const wxPoint &cursor)
{
   return m_Selection.m_CursorA <= cursor
      && cursor <= m_Selection.m_CursorB;
}

#ifdef WXLAYOUT_DEBUG

void
wxLayoutList::Debug(void)
{
   wxLayoutLine *line;


   for(line = m_FirstLine;
       line;
       line = line->GetNextLine())
      line->Debug();
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
#ifdef   M_BASEDIR
   m_ProgressDialog = NULL;
#endif
}

wxLayoutPrintout::~wxLayoutPrintout()
{
#ifdef   M_BASEDIR
   if(m_ProgressDialog) delete m_ProgressDialog;
#endif
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
#ifdef M_BASEDIR
   wxString msg;
   msg.Printf(_("Printing page %d..."), page);
   if(! m_ProgressDialog->Update(page, msg))
      return false;
#endif
   wxDC *dc = GetDC();

   ScaleDC(dc);
   
   if (dc)
   {
      int top, bottom;
      top = (page - 1)*m_PrintoutHeight;
      bottom = top + m_PrintoutHeight;
      // SetDeviceOrigin() doesn't work here, so we need to manually
      // translate all coordinates.
      wxPoint translate(m_Offset.x,m_Offset.y-top);
      m_llist->Draw(*dc, translate, top, bottom);
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
   wxPrinterDC psdc("","",WXLLIST_TEMPFILE,false);
#else
   wxPostScriptDC psdc(WXLLIST_TEMPFILE,false);
#endif

   float scale = ScaleDC(&psdc);

   psdc.GetSize(&m_PageWidth, &m_PageHeight);
   // This sets a left/top origin of 15% and 20%:
   m_Offset = wxPoint((15*m_PageWidth)/100, m_PageHeight/20);

   // This is the length of the printable area.
   m_PrintoutHeight = m_PageHeight - (int) (m_PageHeight * 0.15);
   m_PrintoutHeight = (int)( m_PrintoutHeight / scale); // we want to use the real paper height
   
   
   m_NumOfPages = 1 +
      (int)( m_llist->GetSize().y / (float)(m_PrintoutHeight));

   *minPage = 1;
   *maxPage = m_NumOfPages;

   *selPageFrom = 1;
   *selPageTo = m_NumOfPages;
   wxRemoveFile(WXLLIST_TEMPFILE);

#ifdef M_BASEDIR
   m_ProgressDialog = new MProgressDialog(
      title, _("Printing..."),m_NumOfPages, NULL, false, true);
#endif
   
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


