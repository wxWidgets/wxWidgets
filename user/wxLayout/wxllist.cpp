/*-*- c++ -*-********************************************************
 * wxFTCanvas: a canvas for editing formatted text                  *
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
  - during redraw each line gets iterated over twice, first just
    calculating baselines and positions, second to actually draw it
  - the cursor position is the position before an object, i.e. if the
    buffer starts with a text-object, cursor 0,0 is just before the
    first character

  - the cursor position and size must be decided at layout/draw time
    or the fonts will be wrong
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
#   include   <wx/postscrp.h>
#   include   <wx/print.h>
#   include   <wx/log.h>
#endif

#define   BASELINESTRETCH   12

#ifdef WXLAYOUT_DEBUG
static const char *g_aTypeStrings[] = 
{ 
   "invalid", "text", "cmd", "icon", "linebreak"
};
   
#  define   wxLayoutDebug        wxLogDebug
#  define   WXL_VAR(x)           cerr << #x " = " << x << endl;
#  define   WXL_DBG_POINT(p)     wxLogDebug(#p ": (%d, %d)", p.x, p.y)
#  define   WXL_TRACE(f)         wxLogDebug(#f ": ")
#  define   TypeString(t)        g_aTypeStrings[t]

void
wxLayoutObjectBase::Debug(void)
{
   CoordType bl = 0;
   wxLogDebug("%s: size = %dx%d, bl = %d",
              TypeString(GetType()), GetSize(&bl).x, GetSize(&bl).y, bl); 
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
wxLayoutObjectText::Draw(wxDC &dc)
{
   dc.DrawText(Str(m_Text), m_Position.x, m_Position.y);
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
   wxLogDebug(" `%s`", m_Text.c_str());
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
wxLayoutObjectIcon::Draw(wxDC &dc)
{
   dc.DrawIcon(m_Icon,m_Position.x, m_Position.y);
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
wxLayoutObjectCmd::Draw(wxDC &dc)
{
   wxASSERT(m_font);
   dc.SetFont(m_font);
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
   Draw(dc);
}

//-------------------------- wxLayoutList

wxLayoutList::wxLayoutList()
{
   m_DefaultSetting = NULL;
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
   m_CursorPosition.x = 0; m_CursorPosition.y++;
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
      m_DefaultSetting->Draw(dc);
}

void
wxLayoutList::Layout(wxDC &dc)
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
   
   wxLayoutObjectBase *cursorObject = NULL; // let's find it again
   
   struct
   {
      int top, bottom, left, right;
   } margins;

   margins.top = 0; margins.left = 0;
   margins.right = -1;
   margins.bottom = -1;
   
   position.y = margins.top;
   position.x = margins.left;

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
         position.x = margins.left;
         position.y += baseLineSkip;
         baseLine = m_FontPtSize;
         objBaseLine = baseLine; // not all objects set it
         baseLineSkip = (BASELINESTRETCH * baseLine)/10;
         headOfLine = i;
         headOfLine++;
         position_HeadOfLine = position;
      }
      if(cursorObject == NULL && cursorPos.y == m_CursorPosition.y) // look for cursor
      {
         if(cursorPos.x >= m_CursorPosition.x &&
            m_CursorPosition.x-cursorPos.x+(**i).CountPositions()) // cursor is in current object
         {
            cursorObject = *i;
            CalculateCursor(dc);
         }
      }
      i++;
   }
   while(i != end());
   m_MaxY = position.y + baseLineSkip;
}

void
wxLayoutList::Draw(wxDC &dc,
                   CoordType fromLine, CoordType toLine,
                   iterator start)
{
   Layout(dc); // FIXME just for now

   ResetSettings(dc);

   wxLayoutObjectList::iterator i;
   if(start == iterator(NULL))
      start = begin();

   while( i != end() && (**i).GetPosition().y < fromLine)
      i++;
   for( i = start ;
        i != end() && (toLine == -1 || (**i).GetPosition().y < toLine) ;
        i++ )
      (*i)->Draw(dc);
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

   WXL_VAR(p.x);WXL_VAR(p.y);
   //FIXME: wxGTK: MaxX()/MaxY() broken
   //WXL_VAR(dc.MaxX()); WXL_VAR(dc.MaxY());
   dc.SetBrush(*wxWHITE_BRUSH);
   dc.SetPen(wxPen(*wxWHITE,0,wxTRANSPARENT));
   dc.DrawRectangle(p.x,p.y,2000,2000); //dc.MaxX(),dc.MaxY());
   Draw(dc,-1,-1,start);
   //dc.DrawRectangle(p.x,p.y,2000,2000); //dc.MaxX(),dc.MaxY());
}


void
wxLayoutList::CalculateCursor(wxDC &dc)
{
   CoordType width, height, descent;
   CoordType baseLineSkip = 20; //FIXME

   CoordType offset;
   wxLayoutObjectBase &obj = **FindCurrentObject(&offset);

   WXL_VAR(offset);
   DrawCursor(dc,true); // erase it
   m_CursorCoords = obj.GetPosition();
   WXL_VAR(m_CursorCoords.x); 
   if(obj.GetType() == WXLO_TYPE_TEXT)
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *)&obj;
      String & str = tobj->GetText();
      String sstr = str.substr(0,offset);
      WXL_VAR(sstr);
      dc.GetTextExtent(sstr,&width,&height,&descent);
      WXL_VAR(width);
      m_CursorCoords = wxPoint(m_CursorCoords.x+width,
                               m_CursorCoords.y);
      m_CursorSize = wxPoint(2,height);
   }
   else if(obj.GetType() == WXLO_TYPE_LINEBREAK)
   {
      m_CursorCoords = wxPoint(0, m_CursorCoords.y);
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
   WXL_VAR(m_CursorCoords.x); 
   m_CursorMoved = false; // coords are valid
}

void
wxLayoutList::DrawCursor(wxDC &dc, bool erase)
{

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
      wxBitmap bm(m_CursorSize.x,m_CursorSize.y);
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
   CoordType               offs;
   wxLayoutObjectList::iterator i;

   wxLogDebug("------------------------debug start-------------------------"); 
   for(i = begin(); i != end(); i++)
      (*i)->Debug();
   wxLogDebug("-----------------------debug end----------------------------");
   
   // show current object:
   ShowCurrentObject();
   i = FindCurrentObject(&offs);
   wxLogDebug(" line length: %l", (long int) GetLineLength(i,offs));
   if(i == end())
   {
      wxLogDebug("<<no object found>>");
      return;  // FIXME we should set cursor position to maximum allowed
      // value then
   }
   if((*i)->GetType() == WXLO_TYPE_TEXT)
      wxLogDebug(" \"%s\", offs=%d",((wxLayoutObjectText *)(*i))->GetText().c_str(), (int) offs);
   else
      wxLogDebug(g_aTypeStrings[(*i)->GetType()]);

}

void
wxLayoutList::ShowCurrentObject()
{
   CoordType offs;
   wxLayoutObjectList::iterator i = FindCurrentObject(&offs);

   wxLayoutDebug("Cursor is at (%d, %d)",
                 m_CursorPosition.x, m_CursorPosition.y);

   i = FindCurrentObject(&offs);
   wxLogDebug(" Line length: %d", GetLineLength(i));

   if(i == end())
   {
      wxLogDebug("<<no object found>>");
      return;  // FIXME we should set cursor position to maximum allowed
// value then
   }
   if((*i)->GetType() == WXLO_TYPE_TEXT)
      wxLogDebug(" \"%s\", offs: %d",
                 ((wxLayoutObjectText *)(*i))->GetText().c_str(), offs);
   else
      wxLogDebug(" %s", TypeString((*i)->GetType()));
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

wxLayoutObjectList::iterator 
wxLayoutList::FindCurrentObject(CoordType *offset)
{
   wxLayoutObjectList::iterator obj = end();

   obj = FindObjectCursor(&m_CursorPosition, offset);
   if(obj == end())  // not ideal yet
   {
      obj = tail();
      if(obj != end()) // tail really exists
         *offset = (*obj)->CountPositions(); // at the end of it
   }
   return obj;
}

bool
wxLayoutList::MoveCursor(int dx, int dy)
{
   CoordType offs, lineLength;
   wxLayoutObjectList::iterator i;

   m_CursorMoved = true;  
   
   bool rc = true; // have we moved?

   if(dy > 0 && m_CursorPosition.y < m_MaxLine)
      m_CursorPosition.y += dy;
   else if(dy < 0 && m_CursorPosition.y > 0)
      m_CursorPosition.y += dy; // dy is negative
   if(m_CursorPosition.y < 0)
   {
      m_CursorPosition.y = 0;
      rc = false;
   }
   else if (m_CursorPosition.y > m_MaxLine)
   {
      m_CursorPosition.y = m_MaxLine;
      rc = false;
   }
   
   while(dx > 0)
   {
      i = FindCurrentObject(&offs);
      lineLength = GetLineLength(i,offs);
      if(m_CursorPosition.x < lineLength)
      {
         m_CursorPosition.x ++;
         dx--;
         continue;
      }
      else
      {
         if(m_CursorPosition.y < m_MaxLine)
         {
            m_CursorPosition.y++;
            m_CursorPosition.x = 0;
            dx--;
         }
         else
         {
            rc = false;
            break; // cannot move there
         }
      }
   }
   while(dx < 0)
   {
      if(m_CursorPosition.x > 0)
      {
         m_CursorPosition.x --;
         dx++;
      }
      else
      {
         if(m_CursorPosition.y > 0)
         {
            m_CursorPosition.y --;
            m_CursorPosition.x = 0;
            i = FindCurrentObject(&offs);
            lineLength = GetLineLength(i,offs);
            m_CursorPosition.x = lineLength;
            dx++;
            continue;
         }
         else
         {
            rc = false;
            break; // cannot move left any more
         }
      }
   }
// final adjustment:
   i = FindCurrentObject(&offs);
   lineLength = GetLineLength(i,offs);
   if(m_CursorPosition.x > lineLength)
   {
      m_CursorPosition.x = lineLength;
      rc = false;
   }
#ifdef   WXLAYOUT_DEBUG
   ShowCurrentObject();
#endif
   return rc;
}

void
wxLayoutList::Delete(CoordType count)
{
   WXL_TRACE(Delete);

   if(!m_Editable)
      return;

   WXL_VAR(count);

   m_bModified = true;
   
   CoordType offs;
   wxLayoutObjectList::iterator i;
      
   do
   {
      i  = FindCurrentObject(&offs);
   startover: // ugly, but easiest way to do it
      if(i == end())
         return; // we cannot delete anything more

/* Here we need to treat linebreaks differently.
   If offs==0 we are before the linebreak, otherwise behind.  */
      if((*i)->GetType() == WXLO_TYPE_LINEBREAK)
      {
         if(offs == 0)
         {
            m_MaxLine--;
            erase(i);
            count--;
            continue; // we're done
         }
         else // delete the object behind the linebreak
         {
            i++; // we increment and continue as normal
            offs=0;
            goto startover; 
         }
      }
      else if((*i)->GetType() == WXLO_TYPE_TEXT)
      {
         wxLayoutObjectText *tobj = (wxLayoutObjectText *)*i;
         CoordType len = tobj->CountPositions();
// If we find the end of a text object, this means that we
// have to delete from the object following it.
         if(len == offs)
         {
            i++;
            offs = 0;
            goto startover;
         }
         else if(len <= count) // delete this object
         {
            count -= len;
            erase(i);
            continue; 
         }
         else
         {
            len = count;
            tobj->GetText().erase(offs,len);
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
            erase(i); // after this, i is the iterator for the following object
            continue;
         }
         else // delete the following object
         {
            i++; // we increment and continue as normal
            offs=0;
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

   CoordType offs;
   wxLayoutObjectList::iterator i = FindCurrentObject(&offs);

//   WXL_TRACE(Insert(obj));

   if(i == end())
      push_back(obj);
   else if(offs == 0)
      insert(i,obj);
// do we have to split a text object?
   else if((*i)->GetType() == WXLO_TYPE_TEXT && offs != (*i)->CountPositions())
   {
      wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
#ifdef WXLAYOUT_DEBUG
      wxLayoutDebug("text: %s", tobj->GetText().c_str());
      WXL_VAR(offs);
#endif
      String left = tobj->GetText().substr(0,offs); // get part before cursor
      WXL_VAR(left.c_str());
      tobj->GetText() = tobj->GetText().substr(offs,(*i)->CountPositions()-offs); // keeps the right half
      WXL_VAR(tobj->GetText().c_str());
      insert(i,obj);
      insert(i,new wxLayoutObjectText(left)); // inserts before
   }
   else
   {
// all other cases, append after object:
      wxLayoutObjectList::iterator j = i; // we want to apend after this object
      j++;
      if(j != end())
         insert(j, obj);
      else
         push_back(obj);
   }
   
   m_CursorPosition.x += obj->CountPositions();
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

   CoordType offs;
   wxLayoutObjectList::iterator i = FindCurrentObject(&offs);

   if(i == end())
   {
      Insert(new wxLayoutObjectText(text));
      return;
   }

   switch((**i).GetType())
   {
   case WXLO_TYPE_TEXT:
// insert into an existing text object:
      WXL_TRACE(inserting into existing object);
      tobj = (wxLayoutObjectText *)*i ;
      wxASSERT(tobj);
      tobj->GetText().insert(offs,text);
      break;
   case WXLO_TYPE_LINEBREAK:
   default:
      j = i;
      if(offs == 0) // try to append to previous object
      {
         j--;
         if(j != end() && (**j).GetType() == WXLO_TYPE_TEXT)
         {
            tobj = (wxLayoutObjectText *)*j;
            tobj->GetText()+=text;
         }
         else
            insert(i,new wxLayoutObjectText(text));
      }
      else // cursor after linebreak
      {
         j++;
         if(j != end() && (**j).GetType() == WXLO_TYPE_TEXT)
         {
            tobj = (wxLayoutObjectText *)*j;
            tobj->GetText()=text+tobj->GetText();
         }
         else
         {
            if(j == end())
               push_back(new wxLayoutObjectText(text));
            else
               insert(j,new wxLayoutObjectText(text));
         }
      }
      break;
   }
   m_CursorPosition.x += strlen(text.c_str());
   m_CursorMoved = true;
}

CoordType
wxLayoutList::GetLineLength(wxLayoutObjectList::iterator i, CoordType offs)
{
   if(i == end())
      return 0;

   CoordType len = 0;

   if(offs == 0 && (**i).GetType() == WXLO_TYPE_LINEBREAK)
// we are before a linebrak
      return 0;
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
   m_CursorPosition = wxPoint(0,0);
   m_MaxLine = 0;
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


/******************** printing stuff ********************/

bool wxLayoutPrintout::OnPrintPage(int page)
{
   wxDC *dc = GetDC();
   int top, bottom,width,height;
   if (dc)
   {
      dc->GetSize(&width, &height);
      
      top = (page - 1) * (9*height)/10;
      bottom = top + (9*height)/10;

      if( top >= m_llist->GetSize().y)
         return false;
      m_llist->Draw(*dc,top,bottom);
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
   wxPostScriptDC psdc("tmp.ps",false);
   int width,height;
   psdc.GetSize(&width, &height); // that's all we need it for
      
   
   // This code doesn't work, because we don't have a DC yet.
   // How on earth are we supposed to calculate the number of pages then?

   *minPage = 0;
   *maxPage = (int)( m_llist->GetSize().y / (float)(0.9*height) + 0.5);

   *selPageFrom = 1;
   *selPageTo = *maxPage;

   m_maxPage = *maxPage;
   
}

bool wxLayoutPrintout::HasPage(int pageNum)
{
   if(m_maxPage != -1)
      return pageNum <= m_maxPage;
   return true;
}


wxLayoutPrintout *
wxLayoutList::MakePrintout(wxString const &name)
{
   return new wxLayoutPrintout(*this,name);
}
