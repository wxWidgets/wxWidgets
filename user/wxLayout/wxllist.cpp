/*-*- c++ -*-********************************************************
 * wxllist: wxLayoutList, a layout engine for text and graphics     *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/

/*
  - each Object knows its size and how to draw itself
  - the list is responsible for calculating positions
  - the draw coordinates for each object are the top left corner
  - coordinates only get calculated when things get redrawn
  - The cursor position is the position before an object, i.e. if the
    buffer starts with a text-object, cursor 0,0 is just before the
    first character. For all non-text objects, the cursor positions
    are 0==before or 1==behind. So that all non-text objects count as
    one cursor position.
  - Linebreaks are at the end of a line, that is a line like "abc\n"
    is four cursor positions long. This makes sure that cursor
    positions are "as expected", i.e. in "abc\ndef" the 'd' would be
    at positions (x=0,y=1).
*/

/*
  TODO:

  - cursor redraw problems
  - blinking cursor
  - mouse click positions cursor
  - selection (SetMark(), GetSelection())
  - DND acceptance of text / clipboard support
  - wxlwindow: formatting menu: problem with checked/unchecked consistency gtk bug?
*/

/*
  Known wxGTK bugs:
  - MaxX()/MaxY() don't get set
*/

 
#ifdef __GNUG__
#pragma implementation "wxllist.h"
#endif

//#include "Mpch.h"
#ifdef M_BASEDIR
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

#define   BASELINESTRETCH   12

// This should never really get created
#define   WXLLIST_TEMPFILE   "__wxllist.tmp"

#ifdef WXLAYOUT_DEBUG
static const char *g_aTypeStrings[] = 
{ 
   "invalid", "text", "cmd", "icon", "linebreak"
};
   
#  define   wxLayoutDebug        wxLogDebug
#  define   WXL_VAR(x)           cerr << #x " = " << x << endl;
#  define   WXL_DBG_POINT(p)     wxLayoutDebug(#p ": (%d, %d)", p.x, p.y)
#  define   WXL_TRACE(f)         wxLayoutDebug(#f ": ")
#  define   TypeString(t)        g_aTypeStrings[t]

void
wxLayoutObjectBase::Debug(void)
{
   CoordType bl = 0;
   wxLayoutDebug("%s: size = %dx%d, pos=%d,%d, bl = %d",
                 TypeString(GetType()), GetSize(&bl).x,
                 GetSize(&bl).y,
                 GetPosition().x, GetPosition().y, bl); 
}

#else 
#  define   WXL_VAR(x)   
#  define   WXL_DBG_POINT(p)   
#  define   WXL_TRACE(f)
#  define   ShowCurrentObject()
#  define   TypeString(t)        ""
inline void wxLayoutDebug(const char *, ...) { }
#endif


//-------------------------- wxLayoutObjectText

wxLayoutObjectText::wxLayoutObjectText(const String &txt)
{
   m_Text = txt;
   m_Width = 0;
   m_Height = 0;
   m_Position = wxPoint(-1,-1);
}


wxPoint
wxLayoutObjectText::GetSize(CoordType *baseLine) const
{
   if(baseLine) *baseLine = m_BaseLine;
   return wxPoint(m_Width, m_Height);
}

void
wxLayoutObjectText::Draw(wxDC &dc, wxPoint const &translate)
{
   dc.DrawText(Str(m_Text), m_Position.x + translate.x, m_Position.y+translate.y);
   m_IsDirty = false;
}


void
wxLayoutObjectText::Layout(wxDC &dc, wxPoint position, CoordType baseLine)
{
   long descent = 0l;

   if(m_Position.x != position.x || m_Position.y != position.y)
      m_IsDirty = true;
   
   m_Position = position;
   dc.GetTextExtent(Str(m_Text),&m_Width, &m_Height, &descent);
   m_BaseLine = m_Height - descent;
   if(m_Position.x != position.x || m_Position.y != position.y)
      m_IsDirty = true;
}

#ifdef WXLAYOUT_DEBUG
void
wxLayoutObjectText::Debug(void)
{
   wxLayoutObjectBase::Debug();
   wxLayoutDebug(" `%s`", m_Text.c_str());
}
#endif

//-------------------------- wxLayoutObjectIcon

wxLayoutObjectIcon::wxLayoutObjectIcon(wxIcon const &icon)
{
   m_Position = wxPoint(-1,-1);
   m_Icon = new wxIcon(icon);
}

wxLayoutObjectIcon::wxLayoutObjectIcon(wxIcon *icon)
{
   m_Icon = icon;
}

void
wxLayoutObjectIcon::Draw(wxDC &dc, wxPoint const &translate)
{
   dc.DrawIcon(*m_Icon,m_Position.x+translate.x, m_Position.y+translate.y);
}

void
wxLayoutObjectIcon::Layout(wxDC &dc, wxPoint position, CoordType baseLine)
{
   if(m_Position.x != position.x || m_Position.y != position.y)
      m_IsDirty = true;
   m_Position = position;
}

wxPoint
wxLayoutObjectIcon::GetSize(CoordType *baseLine) const
{
   if(baseLine)   *baseLine = m_Icon->GetHeight();
   return wxPoint(m_Icon->GetWidth(), m_Icon->GetHeight());
}

//-------------------------- wxLayoutObjectCmd


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

wxLayoutStyleInfo *
wxLayoutObjectCmd::GetStyle(void) const
{
   wxLayoutStyleInfo *si = new wxLayoutStyleInfo();


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

   return si;
}

void
wxLayoutObjectCmd::Draw(wxDC &dc, wxPoint const &translate)
{
   wxASSERT(m_font);
   dc.SetFont(*m_font);
   if(m_ColourFG)
      dc.SetTextForeground(*m_ColourFG);
   if(m_ColourBG)
      dc.SetTextBackground(*m_ColourBG);
}
void
wxLayoutObjectCmd::Layout(wxDC &dc, wxPoint p, CoordType baseline)
{
   m_Position = p; // required so we can find the right object for cursor
   // this get called, so that recalculation uses right font sizes
   Draw(dc,wxPoint(0,0));
}

//-------------------------- wxLayoutList

wxLayoutList::wxLayoutList()
{
   m_DefaultSetting = NULL;
   m_WrapMargin = -1;
   m_Editable = FALSE;

   Clear();
}

wxLayoutList::~wxLayoutList()
{
   if(m_DefaultSetting)
      delete m_DefaultSetting;
   // no deletion of objects, they are owned by the list
}

void
wxLayoutList::LineBreak(void)
{
   Insert(new wxLayoutObjectLineBreak);
//   m_CursorPos.x = 0; m_CursorPos.y++;
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


/// for access by wxLayoutWindow:
void
wxLayoutList::GetSize(CoordType *max_x, CoordType *max_y,
                      CoordType *lineHeight)
{
   
   if(max_x) *max_x = m_MaxX;
   if(max_y) *max_y = m_MaxY;
   if(lineHeight) *lineHeight = m_LineHeight;
}

void
wxLayoutList::ResetSettings(wxDC &dc)
{
   // setting up the default:
   dc.SetTextForeground( *wxBLACK );
   dc.SetTextBackground( *wxWHITE );
   dc.SetBackgroundMode( wxSOLID ); // to enable setting of text background
   dc.SetFont( *wxNORMAL_FONT );
   if(m_DefaultSetting)
      m_DefaultSetting->Draw(dc,wxPoint(0,0));
}

void
wxLayoutList::Layout(wxDC &dc, wxLayoutMargins *margins)
{
   iterator i;

   // first object in current line
   wxLayoutObjectList::iterator headOfLine;
   // where we draw next
   wxPoint position, position_HeadOfLine;
   // size of last object
   wxPoint size;
   CoordType baseLine = m_FontPtSize;
   CoordType baseLineSkip = (BASELINESTRETCH * baseLine)/10;
   CoordType objBaseLine = baseLine;
   wxLayoutObjectType type;
   
   // we need to count cursor positions
   wxPoint cursorPos = wxPoint(0,0);
   
   if(margins)
   {
      position.y = margins->top;
      position.x = margins->left;
   }
   else
   {
      position.y = 0;
      position.x = 0;
   }
   
   ResetSettings(dc);
   
   i = begin();
   headOfLine = i;
   position_HeadOfLine = position;

   do
   {
      if(i == end())
         return;
   
      type = (*i)->GetType();
      (*i)->Layout(dc, position, baseLine);
      size = (*i)->GetSize(&objBaseLine);
      // calculate next object's position:
      position.x += size.x;
   
      // do we need to increase the line's height?
      if(size.y > baseLineSkip)
      {
         baseLineSkip = size.y;
         i = headOfLine; position = position_HeadOfLine;
         continue;
      }
      if(objBaseLine > baseLine)
      {
         baseLine = objBaseLine;
         i = headOfLine; position = position_HeadOfLine;
         continue;
      }

      // when we reach here, the coordinates are valid, this part of
      // the loop gets run only once per object
      if(position.x > m_MaxX)
         m_MaxX = position.x;
      if(type == WXLO_TYPE_LINEBREAK)
      {
         cursorPos.x = 0; cursorPos.y ++;
      }
      else
         cursorPos.x += (**i).CountPositions();
      
      // now check whether we have finished handling this line:
      if(type == WXLO_TYPE_LINEBREAK && i != tail()) 
      {
         position.x = margins ? margins->left : 0;
         position.y += baseLineSkip;
         baseLine = m_FontPtSize;
         objBaseLine = baseLine; // not all objects set it
         baseLineSkip = (BASELINESTRETCH * baseLine)/10;
         headOfLine = i;
         headOfLine++;
         position_HeadOfLine = position;
      }
      if(i == m_CursorObject)
         CalculateCursor(dc);
      i++;
   }
   while(i != end());
   m_MaxY = position.y + baseLineSkip;
}

void
wxLayoutList::Draw(wxDC &dc,
                   CoordType fromLine, CoordType toLine,
                   iterator start,
                   wxPoint const &translate)
{
   Layout(dc); // FIXME just for now

   ResetSettings(dc);

   wxLayoutObjectList::iterator i;
   
   if(start == iterator(NULL))
      start = begin();
   else // we need to restore font settings
   {
      for( i = begin() ; i != start; i++)
         if((**i).GetType() == WXLO_TYPE_CMD)
            (**i).Draw(dc,translate);  // apply font settings
   }
      
   while( start != end() && (**start).GetPosition().y < fromLine)
   {
      if((**start).GetType() == WXLO_TYPE_CMD)
         (**start).Draw(dc,translate);  // apply font settings
      start++;
   }
   for( i = start ;
        i != end() && (toLine == -1 || (**i).GetPosition().y < toLine) ;
        i++ )
      (*i)->Draw(dc,translate);
}

/** Erase at least to end of line */
void
wxLayoutList::EraseAndDraw(wxDC &dc, iterator start)
{
   //look for begin of line
   while(start != end() && start != begin() && (**start).GetType() !=
         WXLO_TYPE_LINEBREAK)
      start--;
   if(start == iterator(NULL))
      start = begin();
   if(start == iterator(NULL))
      return;
   
   wxPoint p = (**start).GetPosition();

   //FIXME: wxGTK: MaxX()/MaxY() broken
   //WXL_VAR(dc.MaxX()); WXL_VAR(dc.MaxY());

   dc.SetBrush(wxBrush(*m_ColourBG, wxSOLID));
   dc.SetPen(wxPen(*m_ColourBG,0,wxTRANSPARENT));
   dc.DrawRectangle(p.x,p.y,2000,2000); //dc.MaxX(),dc.MaxY());
   Draw(dc,-1,-1,start,wxPoint(0,0));
   //dc.DrawRectangle(p.x,p.y,2000,2000); //dc.MaxX(),dc.MaxY());
}


void
wxLayoutList::CalculateCursor(wxDC &dc)
{
   CoordType width, height, descent;
   CoordType baseLineSkip = 20; //FIXME

   if( m_CursorObject == iterator(NULL))  // empty list
   {
      m_CursorCoords = wxPoint(0,0);
      m_CursorSize = wxPoint(2,baseLineSkip);
      m_CursorMoved = false; // coords are valid
      return;
   }
   wxLayoutObjectBase &obj = **m_CursorObject;

   m_CursorCoords = obj.GetPosition();
   if(obj.GetType() == WXLO_TYPE_TEXT)
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *)&obj;
      String & str = tobj->GetText();
      String sstr = str.substr(0,m_CursorOffset);
      dc.GetTextExtent(sstr,&width,&height,&descent);
      m_CursorCoords = wxPoint(m_CursorCoords.x+width,
                               m_CursorCoords.y);
      m_CursorSize = wxPoint(2,height);
   }
   else if(obj.GetType() == WXLO_TYPE_LINEBREAK)
   {
      if(m_CursorOffset == 1) // behind linebreak
         m_CursorCoords = wxPoint(0, m_CursorCoords.y + baseLineSkip);
      //m_CursorCoords = wxPoint(0, m_CursorCoords.y);
      m_CursorSize = wxPoint(2,baseLineSkip);
   }
   else
   {
      // this is not necessarily the most "beautiful" solution:
      //cursorPosition = wxPoint(position.x, position.y);
      //cursorSize = wxPoint(size.x > 0 ? size.x : 1,size.y > 0 ? size.y : baseLineSkip);
      m_CursorCoords = wxPoint(m_CursorCoords.x+obj.GetSize().x, m_CursorCoords.y);
      m_CursorSize = wxPoint(2, obj.GetSize().y);
      if(m_CursorSize.y < 1) m_CursorSize.y = baseLineSkip;
   }
   m_CursorMoved = false; // coords are valid
}

void
wxLayoutList::DrawCursor(wxDC &dc, bool erase)
{
   if(! m_Editable)
      return;
   
   if(erase)
   {
      //dc.SetBrush(*wxWHITE_BRUSH);
      //dc.SetPen(wxPen(*wxWHITE,1,wxSOLID));
      //dc.DrawRectangle(m_CursorCoords.x, m_CursorCoords.y, m_CursorSize.x, m_CursorSize.y);
      dc.Blit(m_CursorCoords.x, m_CursorCoords.y, m_CursorSize.x,
              m_CursorSize.y, &m_CursorMemDC,
              0, 0, 0, 0);
   }
   else
   {
      if(IsDirty() || CursorMoved())
      {
         DrawCursor(dc,true);
         Layout(dc);
      }
      // Save background:
      wxBitmap bm(m_CursorSize.x+1,m_CursorSize.y+1);
      m_CursorMemDC.SelectObject(bm);
      m_CursorMemDC.Blit(0, 0, m_CursorSize.x, m_CursorSize.y,
                         &dc, m_CursorCoords.x,
                         m_CursorCoords.y, 0, 0);
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetPen(wxPen(*wxBLACK,1,wxSOLID));
      dc.DrawRectangle(m_CursorCoords.x, m_CursorCoords.y,
                       m_CursorSize.x, m_CursorSize.y);
   }
}







#ifdef WXLAYOUT_DEBUG
void
wxLayoutList::Debug(void)
{
   wxLayoutObjectList::iterator i;

   wxLayoutDebug("------------------------ debug start ------------------------"); 
   for(i = begin(); i != end(); i++)
      (*i)->Debug();
   wxLayoutDebug("-------------------------- list end -------------------------");
   
   // show current object:
   ShowCurrentObject();
   wxLayoutDebug("------------------------- debug end -------------------------");
}

void
wxLayoutList::ShowCurrentObject()
{
   wxLayoutDebug("CursorPos (%d, %d)", (int) m_CursorPos.x, (int) m_CursorPos.y);
   wxLayoutDebug("CursorOffset = %d", (int) m_CursorOffset);
   wxLayoutDebug("CursorObject = %p", m_CursorObject);
   if(m_CursorObject == iterator(NULL))
      wxLayoutDebug("<<no object found>>");
   else
   {
      if((*m_CursorObject)->GetType() == WXLO_TYPE_TEXT)
         wxLayoutDebug(" \"%s\", offs: %d",
                       ((wxLayoutObjectText *)(*m_CursorObject))->GetText().c_str(),
                       m_CursorOffset);
      else
         wxLayoutDebug(" %s", TypeString((*m_CursorObject)->GetType()));
   }
   wxLayoutDebug("Line length: %d", GetLineLength(m_CursorObject));

}

#endif

/******************** editing stuff ********************/

// don't change this, I know how to optimise this and will do it real 
// soon (KB)

/*
 * FindObjectCursor:
 * Finds the object belonging to a given cursor position cpos and
 * returns an iterator to that object and stores the relative cursor
 * position in offset.
 *
 * For linebreaks, the offset can be 0=before or 1=after.
 *
 * If the cpos coordinates don't exist, they are modified.
 */

wxLayoutObjectList::iterator 
wxLayoutList::FindObjectCursor(wxPoint *cpos, CoordType *offset)
{
   wxPoint object = wxPoint(0,0);  // runs along the objects
   CoordType width = 0;
   wxLayoutObjectList::iterator i, begin_it;
   int go_up;
   
//#ifdef WXLAYOUT_DEBUG
//   wxLayoutDebug("Looking for object at (%d, %d)", cpos->x, cpos->y);
//#endif

   // optimisation: compare to last looked at object:
   if(cpos->y > m_FoundCursor.y || (cpos->y == m_FoundCursor.y &&
                                    cpos->x >= m_FoundCursor.x))
      go_up = 1;
   else
      go_up = 0;

   //broken at the moment
   //begin_it = m_FoundIterator;
   //m_FoundCursor = *cpos;
   begin_it = begin();
   go_up = 1;
   for(i = begin_it; i != end() && object.y <= cpos->y; )
   {
      width = (**i).CountPositions();
      if(cpos->y == object.y) // a possible candidate
      {
         if((**i).GetType() ==WXLO_TYPE_LINEBREAK)
         {
            if(cpos->x == object.x)
            {
               if(offset) *offset = 0;
               return m_FoundIterator = i;
            }
            if(offset) *offset=1;
            cpos->x = object.x;
            return m_FoundIterator = i;
         }
         if(cpos->x >= object.x && cpos->x <= object.x+width) // overlap
         {
            if(offset) *offset = cpos->x-object.x;
//#ifdef WXLAYOUT_DEBUG
//            wxLayoutDebug("   found object at (%d, %d), type: %s",
//                          object.x,  object.y, TypeString((*i)->GetType()));
//#endif      
            return m_FoundIterator = i;
         }
      }
      // no overlap, increment coordinates
      object.x += width;
      if((**i).GetType() == WXLO_TYPE_LINEBREAK)
      {
         object.x = 0;
         object.y++;
      }
      if(go_up)
         i++;
      else
         i--;
   }//for
//#ifdef WXLAYOUT_DEBUG
//   wxLayoutDebug("   not found");
//#endif
// return last object, coordinates of that one:
   i = tail();
   if(i == end())
      return m_FoundIterator = i;
   if((**i).GetType()==WXLO_TYPE_LINEBREAK)
   {
      if(offset)
         *offset = 1;
      return m_FoundIterator = i;
   }
   cpos->x = object.x; // would be the coordinate of next object
   cpos->y = object.y;
   cpos->x += width; // last object's width
   if(*offset)  *offset = cpos->x-object.x;
   return m_FoundIterator = i; // not found
}

bool
wxLayoutList::MoveCursor(int dx, int dy)
{
   CoordType diff;

   m_CursorMoved = true;  
   
   enum { up, down} direction;

   wxPoint newPos = wxPoint(m_CursorPos.x + dx,
                            m_CursorPos.y + dy);

   // check for bounds
   //if(newPos.x < 0) newPos.x = 0;
   if(newPos.y < 0) newPos.y = 0;
   else if(newPos.y > m_MaxLine) newPos.y = m_MaxLine;

   if(newPos.y > m_CursorPos.y ||
      newPos.y == m_CursorPos.y &&
      newPos.x >= m_CursorPos.x)
      direction = down;
   else
      direction = up;

   wxASSERT(m_CursorObject);
   // now move cursor forwards until at the new position:

   // first, go to the right line:
   while(newPos.y != m_CursorPos.y)
   {
      if(direction == down)
      {
         m_CursorPos.x +=
            (**m_CursorObject).CountPositions() - m_CursorOffset;
         if(m_CursorObject == tail())
            break;  // can't go any further
         if((**m_CursorObject).GetType() == WXLO_TYPE_LINEBREAK
            && m_CursorOffset == 0)
         {
            m_CursorPos.y++; m_CursorPos.x = 0;
         }
         m_CursorObject ++; m_CursorOffset = 0;
      }
      else // up
      {
         if(m_CursorObject == begin())
            break;  // can't go any further

         if((**m_CursorObject).GetType() == WXLO_TYPE_LINEBREAK &&
            m_CursorOffset == 1)
         {
            m_CursorPos.y--;
            m_CursorPos.x = GetLineLength(m_CursorObject);
         }
         m_CursorPos.x -= m_CursorOffset;
         m_CursorObject --; m_CursorOffset = (**m_CursorObject).CountPositions();
      }
   }
   if(newPos.y != m_CursorPos.y) // reached begin/end of list,
      newPos.y = m_CursorPos.y;  // exited by break
      
   // now line is right, go to right column:
   direction = newPos.x >= m_CursorPos.x ? down : up;
   while(newPos.x != m_CursorPos.x)
   {
      if(direction == down)
      {
         diff = newPos.x - m_CursorPos.x;
         if(diff > (**m_CursorObject).CountPositions()-m_CursorOffset)
         {
            m_CursorPos.x +=
               (**m_CursorObject).CountPositions()-m_CursorOffset;
            if((**m_CursorObject).GetType() == WXLO_TYPE_LINEBREAK &&
               m_CursorOffset == 0)
               m_CursorPos = wxPoint(0, m_CursorPos.y+1);
            if(m_CursorObject == tail())
               break; // cannot go further
            m_CursorObject++; m_CursorOffset = 0;
         }
         else
         {
            if((**m_CursorObject).GetType() == WXLO_TYPE_LINEBREAK)
            {
               newPos.y++; newPos.x -= m_CursorPos.x+1;
               m_CursorPos = wxPoint(0,m_CursorPos.y+1);
            }
            else
               m_CursorPos.x += diff;
            m_CursorOffset += diff;
         }
      }
      else // up
      {
         if(m_CursorPos.x == 0 && m_CursorOffset == 1 &&
            (**m_CursorObject).GetType() == WXLO_TYPE_LINEBREAK) // can we go further up?
         {
            m_CursorPos.y--;
            newPos.y--; 
            m_CursorOffset = 0;
            m_CursorPos.x = GetLineLength(m_CursorObject)-1;
            newPos.x += m_CursorPos.x+1;
            continue;
         }
         diff = m_CursorPos.x - newPos.x;
         if(diff >= m_CursorOffset)
         {
            if(m_CursorObject == begin())
            {
               m_CursorOffset = 0;
               m_CursorPos.x = 0;
               break; // cannot go further
            }
            m_CursorObject--;
            m_CursorPos.x -= m_CursorOffset;
            m_CursorOffset = (**m_CursorObject).CountPositions();
         }
         else
         {
            m_CursorPos.x -= diff;
            m_CursorOffset -= diff;
         }
      }
   }
   return true; // FIXME: when return what?
}

void
wxLayoutList::Delete(CoordType count)
{
   WXL_TRACE(Delete);

   if(!m_Editable)
      return;

   m_bModified = true;
   
   CoordType offs = 0;
   wxLayoutObjectList::iterator i;
      
   do
   {
      i  = m_CursorObject;
   startover: // ugly, but easiest way to do it
      if(i == end())
         return; // we cannot delete anything more

/* Here we need to treat linebreaks differently.
   If m_CursorOffset==0 we are before the linebreak, otherwise behind.  */
      if((*i)->GetType() == WXLO_TYPE_LINEBREAK)
      {
         if(m_CursorOffset == 0)
         {
            m_MaxLine--;
            erase(i);
            m_CursorObject = i; // new i!
            m_CursorOffset = 0; // Pos unchanged
            count--;
            continue; // we're done
         }
         else // delete the object behind the linebreak
         {
            i++; // we increment and continue as normal
            m_CursorOffset=0;
            goto startover; 
         }
      }
      else if((*i)->GetType() == WXLO_TYPE_TEXT)
      {
         wxLayoutObjectText *tobj = (wxLayoutObjectText *)*i;
         CoordType len = tobj->CountPositions();
// If we find the end of a text object, this means that we
// have to delete from the object following it.
         if(len == m_CursorOffset)
         {
            i++;
            m_CursorOffset = 0;
            goto startover;
         }
         else if(len <= count) // delete this object
         {
            count -= len;
            erase(i);
            m_CursorObject = i;
            m_CursorOffset = 0;
            continue; 
         }
         else
         {
            len = count;
            tobj->GetText().erase(m_CursorOffset,len);
            // cursor unchanged
            return; // we are done
         }
      }
      else// all other objects: delete the object
// this only works as expected  if the non-text object has 0/1
// as offset values. Not tested with "longer" objects.
      {
         CoordType len = (*i)->CountPositions();
         if(offs == 0)
         {
            count = count > len ? count -= len : 0;
            erase(i); // after this, i is the iterator for the
                      // following object
            m_CursorObject = i;
            m_CursorOffset = 0;
            continue;
         }
         else // delete the following object
         {
            i++; // we increment and continue as normal
            m_CursorOffset=0;
            goto startover; 
         }
      }
   }
   while(count && i != end());      
}

void
wxLayoutList::Insert(wxLayoutObjectBase *obj)
{
   wxCHECK_RET( obj, "no object to insert" );

   m_bModified = true;

   wxLayoutObjectList::iterator i = m_CursorObject;

   if(i != iterator(NULL) && (*obj).GetType() == WXLO_TYPE_LINEBREAK)
   {
      m_CursorPos.x = 0; m_CursorPos.y ++;
   }
   
   if(i == end())
   {
      push_back(obj);
      m_CursorObject = tail();
   }
   else if(m_CursorOffset == 0)
   {
      insert(i,obj);
      m_CursorObject = i;
   }
   // do we have to split a text object?
   else if((*i)->GetType() == WXLO_TYPE_TEXT && m_CursorOffset != (*i)->CountPositions())
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
      String left = tobj->GetText().substr(0,m_CursorOffset); // get part before cursor
      tobj->GetText() = tobj->GetText().substr(m_CursorOffset,(*i)->CountPositions()-m_CursorOffset); // keeps the right half
      insert(i,obj);
      m_CursorObject = i; // == obj
      insert(i,new wxLayoutObjectText(left)); // inserts before
   }
   else
   {
      // all other cases, append after object:
      wxLayoutObjectList::iterator j = i; // we want to apend after this object
      j++;
      if(j != end())
      {
         insert(j, obj);
         m_CursorObject = j;
      }
      else
      {
         push_back(obj);
         m_CursorObject = tail();
      }
   }

   if(obj->GetType() != WXLO_TYPE_LINEBREAK) // handled separately above
      m_CursorPos.x += obj->CountPositions();
   // applies also for linebreak:
   m_CursorOffset = obj->CountPositions();
   
   if(obj->GetType() == WXLO_TYPE_LINEBREAK)
      m_MaxLine++;
   m_CursorMoved = true;
}

void
wxLayoutList::Insert(String const &text)
{
   wxLayoutObjectText *tobj = NULL;
   wxLayoutObjectList::iterator j;

//   WXL_TRACE(Insert(text));

   if(! m_Editable)
      return;

   m_bModified = true;

   wxLayoutObjectList::iterator i = m_CursorObject;

   if(i == end())
   {
      Insert(new wxLayoutObjectText(text));
      return;
   }

   switch((**i).GetType())
   {
   case WXLO_TYPE_TEXT:
// insert into an existing text object:
      tobj = (wxLayoutObjectText *)*i ;
      wxASSERT(tobj);
      tobj->GetText().insert(m_CursorOffset,text);
      m_CursorObject = i;
      m_CursorOffset = m_CursorOffset + text.length();
      m_CursorPos.x += text.length();
      break;
   case WXLO_TYPE_LINEBREAK:
   default:
      j = i;
      if(m_CursorOffset == 0) // try to append to previous object
      {
         j--;
         if(j != end() && (**j).GetType() == WXLO_TYPE_TEXT)
         {
            tobj = (wxLayoutObjectText *)*j;
            tobj->GetText()+=text;
            m_CursorObject = j;
            m_CursorOffset = (**j).CountPositions();
            m_CursorPos.x += text.length();
         }
         else
         {
            insert(i,new wxLayoutObjectText(text));
            m_CursorObject = i;
            m_CursorOffset = (**i).CountPositions();
            m_CursorPos.x += m_CursorOffset;
         }
      }
      else // offset == 1 : cursor after linebreak
      {
         j++;
         m_CursorObject = j;
         m_CursorOffset = 0;
         if(j != end() && (**j).GetType() == WXLO_TYPE_TEXT)
         {
            tobj = (wxLayoutObjectText *)*j;
            tobj->GetText()=text+tobj->GetText();
            m_CursorOffset = text.length();
            m_CursorPos.x += m_CursorOffset;
         }
         else
         {
            if(j == end())
            {
               push_back(new wxLayoutObjectText(text));
               m_CursorObject = tail();
               m_CursorOffset = (**m_CursorObject).CountPositions();
               m_CursorPos.x += text.length();
            }
            else
            {
               insert(j,new wxLayoutObjectText(text));
               m_CursorObject = j;
               m_CursorOffset = (**j).CountPositions();
               m_CursorPos.x += text.length();
            }
         }
      }
      break;
   }
   m_CursorMoved = true;
}

CoordType
wxLayoutList::GetLineLength(wxLayoutObjectList::iterator i, CoordType offs)
{
   if(i == end())
      return 0;

   CoordType len = 0;

   if(offs == 0 && (**i).GetType() == WXLO_TYPE_LINEBREAK)
      i--;
         
// search backwards for beginning of line:
   while(i != begin() && (*i)->GetType() != WXLO_TYPE_LINEBREAK)
      i--;
   if((*i)->GetType() == WXLO_TYPE_LINEBREAK)
      i++;
// now we can start counting:
   while(i != end() && (*i)->GetType() != WXLO_TYPE_LINEBREAK)
   {
      len += (*i)->CountPositions();
      i++;
   }
   len++; // one extra for the linebreak
   return len;
}

void
wxLayoutList::Clear(int family, int size, int style, int weight,
                    int underline, char const *fg, char const *bg)
{
   m_bModified = true;
   m_CursorMoved = true;
   m_dirty = true;  // force redraw/recalc
   wxLayoutObjectList::iterator i = begin();

   wxBitmap bm(1,1);
   m_CursorMemDC.SelectObject(bm);

   while(i != end()) // == while valid
      erase(i);

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
   
   m_Position = wxPoint(0,0);
   m_CursorPos = wxPoint(0,0);
   m_CursorObject = iterator(NULL);
   m_CursorOffset = 0;
   m_CursorSize = wxPoint(2,(BASELINESTRETCH*m_FontPtSize)/10);
   
   m_MaxLine = 1;
   m_LineHeight = (BASELINESTRETCH*m_FontPtSize)/10;
   m_MaxX = 0; m_MaxY = 0;


   m_FoundCursor = wxPoint(0,0);
   m_FoundIterator = begin();
   
   if(m_DefaultSetting)
      delete m_DefaultSetting;

   m_DefaultSetting = new
      wxLayoutObjectCmd(m_FontPtSize,m_FontFamily,m_FontStyle,
                        m_FontWeight,m_FontUnderline,
                        m_ColourFG, m_ColourBG);
}


wxLayoutObjectBase *
wxLayoutList::Find(wxPoint coords) const
{
   wxLayoutObjectList::iterator i = begin();

   wxPoint topleft, bottomright;
   
   while(i != end()) // == while valid
   {
      wxLayoutObjectBase *object = *i;
      topleft = object->GetPosition();
      if(coords.y >= topleft.y && coords.x >= topleft.x)
      {
         bottomright = topleft;
         bottomright.x += object->GetSize().x;
         bottomright.y += object->GetSize().y;
         if(coords.x <= bottomright.x && coords.y <= bottomright.y)
            return *i;
      }
      i++;
   }
   return NULL;
}


void
wxLayoutList::SetWrapMargin(long n)
{
   m_WrapMargin = n;
}

void
wxLayoutList::WrapLine(void)
{
   wxASSERT(m_CursorObject);

   iterator i = m_CursorObject;

   if(!DoWordWrap() || !i ) // empty list
      return;
   int cursorpos = m_CursorPos.x, cpos, offset;
   
   if(cursorpos < m_WrapMargin)
      return;

   // else: break line

   // find the right object to break:
   // is it the current one?

   i = m_CursorObject;
   cpos = cursorpos-m_CursorOffset;
   while(i != begin() && cpos >= m_WrapMargin)
   {
      i--;
      cpos -= (**i).CountPositions();
   }
   // now i is the object to break and cpos its position

   offset = m_WrapMargin - cpos;
   wxASSERT(offset <= (**i).CountPositions());

   // split it
   if((**i).GetType() == WXLO_TYPE_TEXT)
   {
      wxLayoutObjectText &t = *(wxLayoutObjectText *)*i;
      for(; offset > 0; offset--)
         if(t.GetText().c_str()[offset] == ' ' || t.GetText().c_str()[offset] == '\t')
         {
            String left = t.GetText().substr(0,offset-1); // get part before cursor
            t.GetText() = t.GetText().substr(offset,t.CountPositions()-offset); // keeps the right halve
            insert(i,new wxLayoutObjectLineBreak);
            insert(i,new wxLayoutObjectText(left)); // inserts before
            break;
         }
      if(offset == 0)
      {
         // only insert a line break if there  isn't already one
         iterator j = i; j--;
         if(j && j != begin() && (**j).GetType() != WXLO_TYPE_LINEBREAK)
            insert(i,new wxLayoutObjectLineBreak);
         else
            return; // do nothing
      }
   }
   else
      insert(i,new wxLayoutObjectLineBreak);
   m_MaxLine++;
   m_CursorPos.y++;
   m_CursorPos.x -= offset;
   m_CursorOffset -= offset;
}
/******************** printing stuff ********************/

wxLayoutPrintout::wxLayoutPrintout(wxLayoutList &llist,
                                   wxString const & title)
:wxPrintout(title)
{
   m_llist = &llist;
   m_title = title;
}

bool wxLayoutPrintout::OnPrintPage(int page)
{
   wxDC *dc = GetDC();
   if (dc)
   {
      DrawHeader(*dc,wxPoint(m_Margins.left,m_Margins.top/2),wxPoint(m_Margins.right,m_Margins.top),page);
      int top, bottom;
      top = (page - 1)*m_PrintoutHeight;
      bottom = top + m_PrintoutHeight;
      // SetDeviceOrigin() doesn't work here, so we need to manually
      // translate all coordinates.
      wxPoint translate(m_Margins.left,-top+m_Margins.top);
      m_llist->Draw(*dc,top,bottom,wxLayoutObjectList::iterator(NULL),translate);
      return true;
   }
   else
      return false;
}

bool wxLayoutPrintout::OnBeginDocument(int startPage, int endPage)
{
   if (!wxPrintout::OnBeginDocument(startPage, endPage))
    return false;

  return true;
}

void
wxLayoutPrintout::OnPreparePrinting(void)
{
   
}


void wxLayoutPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
   // ugly hack to get number of pages
#ifdef __WXMSW__
   wxPrinterDC psdc("","",WXLLIST_TEMPFILE,false);
#else
   wxPostScriptDC psdc(WXLLIST_TEMPFILE,false);
#endif
   psdc.GetSize(&m_PageWidth, &m_PageHeight); // that's all we need it for

   // We do 5% margins on top and bottom, and a 5% high header line.
   m_Margins.top = m_PageHeight / 10 ;      // 10%, half of it header
   m_Margins.bottom = m_PageHeight - m_PageHeight / 20;   // 95%
   // On the sides we reserve 10% each for the margins.
   m_Margins.left = m_PageWidth / 10;
   m_Margins.right = m_PageWidth - m_PageWidth / 10;

   // This is the length of the printable area.
   m_PrintoutHeight = m_PageHeight - (int) (m_PageHeight * 0.15); 

   //FIXME this is wrong but not used at the moment
   m_PageWidth = m_Margins.right - m_Margins.left;

   m_NumOfPages = (int)( m_llist->GetSize().y / (float)(m_PrintoutHeight) + 0.5);
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


void
wxLayoutPrintout::DrawHeader(wxDC &dc,
                             wxPoint topleft, wxPoint bottomright,
                             int pageno)
{
   // make backups of all essential parameters
   wxBrush &brush = dc.GetBrush();
   wxPen   &pen = dc.GetPen();
   wxFont  &font = dc.GetFont(),
           *myfont;
   
   dc.SetBrush(*wxWHITE_BRUSH);
   dc.SetPen(wxPen(*wxBLACK,0,wxSOLID));
   dc.DrawRoundedRectangle(topleft.x,
                           topleft.y,bottomright.x-topleft.x,
                           bottomright.y-topleft.y);  
   dc.SetBrush(*wxBLACK_BRUSH);
   myfont = new wxFont((WXLO_DEFAULTFONTSIZE*12)/10,wxSWISS,wxNORMAL,wxBOLD,false,"Helvetica");
   dc.SetFont(*myfont);

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

   delete myfont;
}


wxLayoutPrintout *
wxLayoutList::MakePrintout(wxString const &name)
{
   return new wxLayoutPrintout(*this,name);
}
