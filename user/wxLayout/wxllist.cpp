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
#ifdef M_PREFIX
#   include "gui/wxllist.h"
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

/// Helper function, allows me to compare to wxPoints
bool operator ==(wxPoint const &p1, wxPoint const &p2)
{
   return p1.x == p2.x && p1.y == p2.y;
}

/// Helper function, allows me to compare to wxPoints
bool operator !=(wxPoint const &p1, wxPoint const &p2)
{
   return p1.x != p2.x || p1.y != p2.y;
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

wxLayoutObjectIcon::wxLayoutObjectIcon(wxBitmap *icon)
{
   m_Icon = icon;
}

void
wxLayoutObjectIcon::Draw(wxDC &dc, wxPoint const &coords)
{
   dc.DrawBitmap(*m_Icon, coords.x, coords.y-m_Icon->GetHeight());
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
                                     wxColour const *fg, wxColour const *bg)
   
{
   m_font = new wxFont(size,family,style,weight,underline);
   m_ColourFG = fg;
   m_ColourBG = bg;
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

   si->fg_red = m_ColourFG->Red();
   si->fg_green = m_ColourFG->Green();
   si->fg_blue = m_ColourFG->Blue();
   si->bg_red = m_ColourBG->Red();
   si->bg_green = m_ColourBG->Green();
   si->bg_blue = m_ColourBG->Blue();
}

void
wxLayoutObjectCmd::Draw(wxDC &dc, wxPoint const & /* coords */)
{
   wxASSERT(m_font);
   dc.SetFont(*m_font);
   if(m_ColourFG)
      dc.SetTextForeground(*m_ColourFG);
   if(m_ColourBG)
      dc.SetTextBackground(*m_ColourBG);
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

wxLayoutLine::wxLayoutLine(wxLayoutLine *prev)
{
   m_LineNumber = 0;
   m_Height = 0;
   m_Length = 0;
   m_Dirty = true;
   m_Previous = prev;
   m_Next = NULL;
   RecalculatePosition();
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
      m_Next->RecalculatePositions(1);
   }
}

wxLayoutLine::~wxLayoutLine()
{
   // kbList cleans itself
}

wxPoint
wxLayoutLine::RecalculatePosition(void)
{
   if(m_Previous)
      m_Position = m_Previous->GetPosition() +
         wxPoint(0,m_Previous->GetHeight());
   else
      m_Position = wxPoint(0,0);
   return m_Position;
}

void
wxLayoutLine::RecalculatePositions(int recurse)
{
   wxASSERT(recurse >= 0);
   wxPoint pos = m_Position;
   CoordType height = m_Height;
   
//   WXLO_TRACE("RecalculatePositions()");
   RecalculatePosition();
   if(m_Next)
   {
      if(recurse > 0)
      {
         if(m_Next) m_Next->RecalculatePositions(--recurse);
      }
      else if(pos != m_Position || m_Height != height)
         if(m_Next) m_Next->RecalculatePositions();         
   }
}

wxLayoutObjectList::iterator
wxLayoutLine::FindObject(CoordType xpos, CoordType *offset) const
{
   wxASSERT(xpos >= 0);
   wxASSERT(offset);
   wxLayoutObjectList::iterator i;
   CoordType x = 0, len;
   

   for(i = m_ObjectList.begin(); i != NULLIT; i++)
   {
      len = (**i).GetLength();
      if( x <= xpos && xpos <= x + len )
      {
         *offset = xpos-x;
         return i;
      }
      x += (**i).GetLength();
   }
   return NULLIT;
}

wxLayoutObjectList::iterator
wxLayoutLine::FindObjectScreen(wxDC &dc, CoordType xpos, CoordType *cxpos) const
{
   wxASSERT(cxpos);
   wxASSERT(xpos);
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
         return i;
      }
      x += (**i).GetWidth();
      cx += (**i).GetLength();
   }
   // behind last object:
   *cxpos = cx;
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
         while(isspace(str[count])) count++;
         // 2. eat the word itself:
         while(isalnum(str[count])) count++;
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
wxLayoutLine::DeleteLine(bool update)
{
   if(m_Next) m_Next->m_Previous = m_Previous;
   if(m_Previous) m_Previous->m_Next = m_Next;
   if(update)
   {
      m_Next->MoveLines(-1);
      m_Next->RecalculatePositions(1);
   }
   wxLayoutLine *next = m_Next;
   delete this;
   return next;
}

void
wxLayoutLine::Draw(wxDC &dc, const wxPoint & offset) const
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
wxLayoutLine::Layout(wxDC &dc, wxPoint *cursorPos,
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
      m_Next->RecalculatePositions();

   if(cursorPos)
   {
      // this might be the case if the cursor is at the end of the
      // line or on a command object:
      if(cursorSize->y < WXLO_MINIMUM_CURSOR_WIDTH)
      {
         if(m_BaseLine > 0)
         {
            cursorSize->y = m_BaseLine;
            if(cursorSize->x < WXLO_MINIMUM_CURSOR_WIDTH) cursorSize->x = WXLO_MINIMUM_CURSOR_WIDTH;
         }
         else // empty line
         {
            CoordType width, height, descent;
            dc.GetTextExtent(WXLO_CURSORCHAR, &width, &height, &descent);
            cursorSize->x = width;
            cursorSize->y = height;
         }
      }
      if(m_BaseLine >= cursorSize->y) // the normal case anyway
         cursorPos->y += m_BaseLine-cursorSize->y;
   }
}


wxLayoutLine *
wxLayoutLine::Break(CoordType xpos)
{
   wxASSERT(xpos >= 0);
   
   if(xpos == 0)
   { // insert an empty line before this one
      wxLayoutLine *prev = new wxLayoutLine(m_Previous);
      if(m_Previous == NULL)
      {  // We were in first line, need to link in new empty line
         // before this.
         prev->m_Next = this;
         m_Previous = prev;
         m_Previous->m_Height = GetHeight(); // this is a wild guess
      }
      MoveLines(+1);
      if(m_Next)
         m_Next->RecalculatePositions(1);
      return this;
   }
   
   CoordType offset;
   wxLOiterator i = FindObject(xpos, &offset);
   if(i == NULLIT)
      // must be at the end of the line then
      return new wxLayoutLine(this);
   // split this line:

   wxLayoutLine *newLine = new wxLayoutLine(this);
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
      m_Next->RecalculatePositions(2);
   return newLine;
}
   

void
wxLayoutLine::MergeNextLine(void)
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
   RecalculatePositions(1);
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
            if( isspace(((wxLayoutObjectText*)*i)->GetText()[offset]))
               return column;
            else
            {
               offset--;
               column--;
            }
         }while(offset != -1);
      }
      else
         column -= (**i).GetLength();
      // This is both "else" and what has to be done after checking
      // all positions of the text object:
      i--;
      offset = (**i).GetLength();
   }while(i != NULLIT);
   return -1;
}
   

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   
   The wxLayoutList object
   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutList::wxLayoutList()
{
   m_DefaultSetting = NULL;
   m_FirstLine = NULL;
   InternalClear();
}

wxLayoutList::~wxLayoutList()
{
   InternalClear();
}

void
wxLayoutList::InternalClear(void)
{
   while(m_FirstLine)
      m_FirstLine = m_FirstLine->DeleteLine(false);

   if(m_DefaultSetting)
   {
      delete m_DefaultSetting;
      m_DefaultSetting = NULL;
   }

   m_CursorPos = wxPoint(0,0);
   m_CursorScreenPos = wxPoint(0,0);
   m_CursorSize = wxPoint(0,0);
   m_FirstLine = new wxLayoutLine(NULL); // empty first line
   m_CursorLine = m_FirstLine;
}

void
wxLayoutList::SetFont(int family, int size, int style, int weight,
                      int underline, wxColour const *fg,
                      wxColour const *bg)
{
   if(family != -1)    m_FontFamily = family;
   if(size != -1)      m_FontPtSize = size;
   if(style != -1)     m_FontStyle = style;
   if(weight != -1)    m_FontWeight = weight;
   if(underline != -1) m_FontUnderline = underline != 0;

   if(fg != NULL)     m_ColourFG = fg;
   if(bg != NULL)     m_ColourBG = bg;
   
   Insert(
      new wxLayoutObjectCmd(m_FontPtSize,m_FontFamily,m_FontStyle,m_FontWeight,m_FontUnderline,
                            m_ColourFG, m_ColourBG));
}

void
wxLayoutList::SetFont(int family, int size, int style, int weight,
                      int underline, char const *fg, char const *bg)

{
   wxColour const
      * cfg = NULL,
      * cbg = NULL;

   if( fg )
      cfg = wxTheColourDatabase->FindColour(fg);
   if( bg )
      cbg = wxTheColourDatabase->FindColour(bg);
   
   SetFont(family,size,style,weight,underline,cfg,cbg);
}

void
wxLayoutList::Clear(int family, int size, int style, int weight,
                    int /* underline */, char const *fg, char const *bg)
{
   InternalClear();
   
   // set defaults
   m_FontPtSize = size;
   m_FontUnderline = false;
   m_FontFamily = family;
   m_FontStyle = style;
   m_FontWeight = weight;
   m_ColourFG = wxTheColourDatabase->FindColour(fg);
   m_ColourBG = wxTheColourDatabase->FindColour(bg);

   if(! m_ColourFG) m_ColourFG = wxBLACK;
   if(! m_ColourBG) m_ColourBG = wxWHITE;
   
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
   wxLayoutLine *line = m_FirstLine;
   while(line && line->GetLineNumber() != p.y)
      ;
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
         return false;
      }
      else
      {
         if(m_CursorPos.x > m_CursorLine->GetLength())
            m_CursorPos.x = m_CursorLine->GetLength();
         return true;
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
         return false;
      }
      else
      {
         if(m_CursorPos.x > m_CursorLine->GetLength())
            m_CursorPos.x = m_CursorLine->GetLength();
         return true;
      }
   }
}

bool
wxLayoutList::MoveCursorHorizontally(int n)
{
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
   m_CursorLine->Insert(m_CursorPos.x, text);
   m_CursorPos.x += text.Length();
   return true;
}

bool
wxLayoutList::Insert(wxLayoutObject *obj)
{
   wxASSERT(m_CursorLine);
   m_CursorLine->Insert(m_CursorPos.x, obj);
   m_CursorPos.x += obj->GetLength();
   return true;
}

bool
wxLayoutList::LineBreak(void)
{
   wxASSERT(m_CursorLine);

   bool setFirst = (m_CursorLine == m_FirstLine && m_CursorPos.x == 0);
   m_CursorLine = m_CursorLine->Break(m_CursorPos.x);
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
            m_CursorLine->MergeNextLine();
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
      m_CursorLine = m_CursorLine->DeleteLine(true);
      n--;
      if(line == m_FirstLine) m_FirstLine = m_CursorLine;
      wxASSERT(m_FirstLine);
      wxASSERT(m_CursorLine);
   }
   m_CursorLine->RecalculatePositions(2);
   return n;
}

void
wxLayoutList::Recalculate(wxDC &dc, CoordType bottom) const
{
   wxLayoutLine *line = m_FirstLine;

   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   m_DefaultSetting->Layout(dc);
   while(line)
   {
      line->RecalculatePosition(); // so we don't need to do it all the time
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
}

void
wxLayoutList::Layout(wxDC &dc, CoordType bottom) const
{
   wxLayoutLine *line = m_FirstLine;

   // first, make sure everything is calculated - this might not be
   // needed, optimise it later
   m_DefaultSetting->Layout(dc);
   while(line)
   {
      if(line == m_CursorLine)
         line->Layout(dc, (wxPoint *)&m_CursorScreenPos, (wxPoint *)&m_CursorSize, m_CursorPos.x);
      else
         line->Layout(dc);
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
   // can only be 0 if we are on the first line and have no next line
   wxASSERT(m_CursorSize.x != 0 || (m_CursorLine &&
                                    m_CursorLine->GetNextLine() == NULL &&
                                    m_CursorLine == m_FirstLine));
}

void
wxLayoutList::Draw(wxDC &dc, wxPoint const &offset,
                   CoordType top, CoordType bottom) const
{
   wxLayoutLine *line = m_FirstLine;

   Layout(dc, bottom);
   m_DefaultSetting->Draw(dc, wxPoint(0,0));
   while(line)
   {
      // only draw if between top and bottom:
      if((top == -1 || line->GetPosition().y >= top))
         line->Draw(dc, offset);
      // little condition to speed up redrawing:
      if(bottom != -1 && line->GetPosition().y > bottom) break;
      line = line->GetNextLine();
   }
   // can only be 0 if we are on the first line and have no next line
   wxASSERT(m_CursorSize.x != 0 || (m_CursorLine &&
                                    m_CursorLine->GetNextLine() == NULL &&
                                    m_CursorLine == m_FirstLine));
}

wxLayoutObject *
wxLayoutList::FindObjectScreen(wxDC &dc, wxPoint const pos, wxPoint *cursorPos)
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
      line->Layout(dc);
      line = line->GetNextLine();
   }
   if(line == NULL) return NULL; // not found
   if(cursorPos) cursorPos->y = line->GetLineNumber();
   // Now, find the object in the line:
   wxLOiterator i = line->FindObjectScreen(dc, pos.x, & cursorPos->x);
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

   wxPoint max(0,0);
   
   // find last line:
   while(line)
   {
      if(line->GetWidth() > max.x) max.x = line->GetWidth();
      last = line;
      line = line->GetNextLine();
   }

   max.y = last->GetPosition().y + last->GetHeight();
   return max;
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
   dc.SetLogicalFunction(wxXOR);
   if(active)
      dc.DrawRectangle(coords.x, coords.y, m_CursorSize.x,
                       m_CursorSize.y);
   else
      dc.DrawLine(coords.x, coords.y+m_CursorSize.y-1,
                  coords.x+m_CursorSize.x, coords.y+m_CursorSize.y-1);
   dc.SetLogicalFunction(wxCOPY);
   dc.SetBrush(wxNullBrush);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   wxLayoutPrintout

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

wxLayoutPrintout::wxLayoutPrintout(wxLayoutList *llist,
                                   wxString const & title)
:wxPrintout(title)
{
   m_llist = llist;
   m_title = title;
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
   // This sets a left/top origin of 10% and 20%:
   m_Offset = wxPoint(m_PageWidth/10, m_PageHeight/20);

   // This is the length of the printable area.
   m_PrintoutHeight = m_PageHeight - (int) (m_PageHeight * 0.1);
   m_PrintoutHeight = (int)( m_PrintoutHeight / scale); // we want to use the real paper height
   
   
   m_NumOfPages = (int)( m_llist->GetSize().y / (float)(m_PrintoutHeight) + 0.5);

   // This is a crude hack to get it right for very small
   // printouts. No idea why this is required, I thought +0.5 would do 
   // the job. :-(
   if(m_NumOfPages == 0 && m_llist->GetSize().y > 0)
      m_NumOfPages = 1;
   *minPage = 1;
   *maxPage = m_NumOfPages;

   *selPageFrom = 1;
   *selPageTo = m_NumOfPages;
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


