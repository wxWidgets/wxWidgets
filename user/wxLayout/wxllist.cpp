/*-*- c++ -*-********************************************************
 * wxFTCanvas: a canvas for editing formatted text                  *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$       *
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
*/

#ifdef __GNUG__
#pragma implementation "wxllist.h"
#endif

#include   "wxllist.h"
#include   "iostream.h"

#include   <wx/dc.h>
#include   <wx/postscrp.h>
#include   <wx/print.h>

#define   BASELINESTRETCH   12

#define   VAR(x)   cerr << #x"=" << x << endl;
#define   DBG_POINT(p)   cerr << #p << ": " << p.x << ',' << p.y << endl
#define   TRACE(f)   cerr << #f":" << endl;

#ifdef WXLAYOUT_DEBUG
static const char *_t[] = { "invalid", "text", "cmd", "icon",
                            "linebreak"};

void
wxLayoutObjectBase::Debug(void)
{
   CoordType bl = 0;
   cerr << _t[GetType()] << ": size=" << GetSize(&bl).x << ","
        << GetSize(&bl).y << " bl=" << bl; 
}
#endif

//-------------------------- wxLayoutObjectText

wxLayoutObjectText::wxLayoutObjectText(const String &txt)
{
   m_Text = txt;
   m_Width = 0;
   m_Height = 0;
}


wxPoint
wxLayoutObjectText::GetSize(CoordType *baseLine) const
{
   if(baseLine) *baseLine = m_BaseLine;
   return wxPoint(m_Width, m_Height);
}

void
wxLayoutObjectText::Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                         bool draw)
{
   long descent = 0l;
   dc.GetTextExtent(Str(m_Text),&m_Width, &m_Height, &descent);
   //FIXME: wxGTK does not set descent to a descent value yet.
   if(descent == 0)
      descent = (2*m_Height)/10;  // crude fix
   m_BaseLine = m_Height - descent;
   position.y += baseLine-m_BaseLine;
   if(draw)
      dc.DrawText(Str(m_Text),position.x,position.y);
#   ifdef   WXLAYOUT_DEBUG
//   dc.DrawRectangle(position.x, position.y, m_Width, m_Height);
#   endif
}

#ifdef WXLAYOUT_DEBUG
void
wxLayoutObjectText::Debug(void)
{
   wxLayoutObjectBase::Debug();
   cerr << " `" << m_Text << '\'';
}
#endif

//-------------------------- wxLayoutObjectIcon

wxLayoutObjectIcon::wxLayoutObjectIcon(wxIcon *icon)
{
   m_Icon = icon;
}

void
wxLayoutObjectIcon::Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                         bool draw)
{
   position.y += baseLine - m_Icon->GetHeight();
   if(draw)
      dc.DrawIcon(m_Icon,position.x,position.y);
}

wxPoint
wxLayoutObjectIcon::GetSize(CoordType *baseLine) const
{
   wxASSERT(baseLine);
   *baseLine = m_Icon->GetHeight();
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
wxLayoutObjectCmd::Draw(wxDC &dc, wxPoint position, CoordType lineHeight,
                        bool draw)
{
   wxASSERT(m_font);
   // this get called even when draw==false, so that recalculation
   // uses right font sizes
   dc.SetFont(m_font);
   if(m_ColourFG)
      dc.SetTextForeground(*m_ColourFG);
   if(m_ColourBG)
      dc.SetTextBackground(*m_ColourBG);
}

//-------------------------- wxwxLayoutList

wxLayoutList::wxLayoutList()
{
   m_DefaultSetting = NULL;
   Clear();
}

wxLayoutList::~wxLayoutList()
{
   if(m_DefaultSetting)
      delete m_DefaultSetting;
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
   if(underline != -1) m_FontUnderline = underline;

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
   wxASSERT(max_x); wxASSERT(max_y); wxASSERT(lineHeight);
   *max_x = m_MaxX;
   *max_y = m_MaxY;
   *lineHeight = m_LineHeight;
}

wxLayoutObjectBase *
wxLayoutList::Draw(wxDC &dc, bool findObject, wxPoint const &findCoords)
{
   wxLayoutObjectList::iterator i;

   // in case we need to look for an object
   wxLayoutObjectBase *foundObject = NULL;
   
   // first object in current line
   wxLayoutObjectList::iterator headOfLine;

   // do we need to recalculate current line?
   bool recalculate = false;

   // do we calculate or draw? Either true or false.
   bool draw = false;
   // drawing parameters:
   wxPoint position = wxPoint(0,0);
   wxPoint position_HeadOfLine;
   CoordType baseLine = m_FontPtSize;
   CoordType baseLineSkip = (BASELINESTRETCH * baseLine)/10;

   // we trace the objects' cursor positions so we can draw the cursor
   wxPoint cursor = wxPoint(0,0);
   // the cursor position inside the object
   CoordType cursorOffset = 0;
   // object under cursor
   wxLayoutObjectList::iterator cursorObject = FindCurrentObject(&cursorOffset);
   
   // queried from each object:
   wxPoint       size = wxPoint(0,0);
   CoordType     objBaseLine = baseLine;
   wxLayoutObjectType type;

   // used temporarily
   wxLayoutObjectText *tobj = NULL;


   // this is needed for printing to a printer:
   // only interesting for printer/PS output
   int pageWidth, pageHeight;   //GetSize() still needs int at the moment
   struct
   {
      int top, bottom, left, right;
   } margins;

   if(
#ifdef __WXMSW__
      dc.IsKindOf(CLASSINFO(wxPrinterDC)) ||
#endif
      dc.IsKindOf(CLASSINFO(wxPostScriptDC)))
   {
      VAR(wxThePrintSetupData);
      
      dc.GetSize(&pageWidth, &pageHeight);
      dc.StartDoc(_("Printing..."));
      dc.StartPage();
      margins.top = (1*pageHeight)/10;    // 10%
      margins.bottom = (9*pageHeight)/10; // 90%
      margins.left = (1*pageWidth)/10;
      margins.right = (9*pageWidth)/10;
   }
   else
   {
      margins.top = 0; margins.left = 0;
      margins.right = -1;
      margins.bottom = -1;
   }
   position.y = margins.right;
   position.x = margins.left;
   
   VAR(findObject); VAR(findCoords.x); VAR(findCoords.y);
   // if the cursorobject is a cmd, we need to find the first
   // printable object:
   while(cursorObject != end()
         && (*cursorObject)->GetType() == WXLO_TYPE_CMD)
      cursorObject++;

   headOfLine = begin();
   position_HeadOfLine = position;

   // setting up the default:
   dc.SetTextForeground( *wxBLACK );
   dc.SetTextBackground( *wxWHITE );
   dc.SetBackgroundMode( wxSOLID ); // to enable setting of text background
   dc.SetFont( *wxNORMAL_FONT );


   //FIXME: who frees the brush, how long does it need to exist?
   if(m_DefaultSetting)
      m_DefaultSetting->Draw(dc,wxPoint(0,0),0,true);

   // we calculate everything for drawing a line, then rewind to the
   // begin of line and actually draw it
   i = begin();
   for(;;)
   {
      recalculate = false;

      if(i == end())
         break;
      type = (*i)->GetType();

      // to initialise sizes of objects, we need to call Draw
      (*i)->Draw(dc, position, baseLine, draw);

      // update coordinates for next object:
      size = (*i)->GetSize(&objBaseLine);
      if(findObject && draw)  // we need to look for an object
      {
         if(findCoords.y >= position.y
            && findCoords.y <= position.y+size.y
            && findCoords.x >= position.x
            && findCoords.x <= position.x+size.x)
         {
            foundObject = *i;
            findObject = false; // speeds things up
         }
      }
      // draw the cursor
      if(m_Editable && draw && i == cursorObject)
      {
         if(type == WXLO_TYPE_TEXT) // special treatment
         {
            long descent = 0l; long width, height;
            tobj = (wxLayoutObjectText *)*i;
            String  str = tobj->GetText();
            VAR(m_CursorPosition.x); VAR(cursor.x);
            str = str.substr(0, cursorOffset);
            VAR(str);
            dc.GetTextExtent(Str(str), &width,&height, &descent);
            VAR(height);
            VAR(width); VAR(descent);
            dc.DrawLine(position.x+width,
                        position.y+(baseLineSkip-height),
                        position.x+width, position.y+baseLineSkip);
         }
         else
         {
            if(type == WXLO_TYPE_LINEBREAK)
               dc.DrawLine(0, position.y+baseLineSkip, 0, position.y+2*baseLineSkip);
            else
            {
               if(size.x == 0)
               {
                  if(size.y == 0)
                     dc.DrawLine(position.x, position.y, position.x, position.y+baseLineSkip);
                  else
                     dc.DrawLine(position.x, position.y, position.x, position.y+size.y);
               }
               else
                  dc.DrawRectangle(position.x, position.y, size.x, size.y);
            }
         }
      }

      // calculate next object's position:
      position.x += size.x;
      if(position.x > m_MaxX)
         m_MaxX = position.x;
      
      // do we need to increase the line's height?
      if(size.y > baseLineSkip)
      {
         baseLineSkip = size.y;
         recalculate = true;
      }
      if(objBaseLine > baseLine)
      {
         baseLine = objBaseLine;
         recalculate = true;
      }

      // now check whether we have finished handling this line:
      if(type == WXLO_TYPE_LINEBREAK || i == tail())
      {
         if(recalculate)  // do this line again
         {
            position.x = position_HeadOfLine.x;
            i = headOfLine;
            continue;
         }

         if(! draw) // finished calculating sizes
         {
            // if the this line needs to go onto a new page, we need
            // to change pages before drawing it:
            if(margins.bottom != -1 && position.y > margins.bottom)
            {
               dc.EndPage();
               position_HeadOfLine.y = margins.top;
               dc.StartPage();
            }
            // do this line again, this time drawing it
            position = position_HeadOfLine;
            draw = true;
            i = headOfLine;
            continue;
         }
         else // we have drawn a line, so continue calculating next one
            draw = false;
      }

      // is it a linebreak?
      if(type == WXLO_TYPE_LINEBREAK || i == tail())
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
      i++;
   }
   dc.EndDoc();
   m_MaxY = position.y;
   return foundObject;
}

#ifdef WXLAYOUT_DEBUG
void
wxLayoutList::Debug(void)
{
   CoordType               offs;
   wxLayoutObjectList::iterator i;

   cerr <<
      "------------------------debug start-------------------------" << endl;
   for(i = begin(); i != end(); i++)
   {
      (*i)->Debug();
      cerr << endl;
   }
   cerr <<
      "-----------------------debug end----------------------------"
        << endl;
   // show current object:
   cerr << "Cursor: "
        << m_CursorPosition.x << ','
        << m_CursorPosition.y;
   
   i = FindCurrentObject(&offs);
   cerr << " line length: " << GetLineLength(i) << "  ";
   if(i == end())
   {
      cerr << "<<no object found>>" << endl;
      return;  // FIXME we should set cursor position to maximum allowed
      // value then
   }
   if((*i)->GetType() == WXLO_TYPE_TEXT)
   {
      cerr << " \"" << ((wxLayoutObjectText *)(*i))->GetText() << "\", offs: "
           << offs << endl;
   }
   else
      cerr << ' ' << _t[(*i)->GetType()] << endl;

}
#endif

/******************** editing stuff ********************/

// don't change this, I know how to optimise this and will do it real 
// soon (KB)

wxLayoutObjectList::iterator 
wxLayoutList::FindObjectCursor(wxPoint const &cpos, CoordType *offset)
{
   wxPoint cursor = wxPoint(0,0);  // runs along the objects
   CoordType width;
   wxLayoutObjectList::iterator i;

#ifdef WXLAYOUT_DEBUG
   cerr << "Looking for object at " << cpos.x << ',' << cpos.y <<
      endl;
#endif
   for(i = begin(); i != end() && cursor.y <= cpos.y; i++)
   {
      width = 0;
      if((*i)->GetType() == WXLO_TYPE_LINEBREAK)
      {
         if(cpos.y == cursor.y)
         {
            --i;
            if(offset)
               *offset = (*i)->CountPositions();
            return i;
         }
         cursor.x = 0; cursor.y ++;
      }
      else
         cursor.x += (width = (*i)->CountPositions());
      if(cursor.y == cpos.y && (cursor.x > cpos.x ||
                                ((*i)->GetType() != WXLO_TYPE_CMD && cursor.x == cpos.x))
         ) // found it ?   
      {
         if(offset)
            *offset = cpos.x-(cursor.x-width);  // 0==cursor before
         // the object
#ifdef WXLAYOUT_DEBUG
         cerr << "   found object at " << cursor.x-width << ',' <<
            cursor.y << ", type:" << _t[(*i)->GetType()] <<endl;
#endif   
         return i;
      }
   }
#ifdef WXLAYOUT_DEBUG
   cerr << "   not found" << endl;
#endif
   return end(); // not found
}

wxLayoutObjectList::iterator 
wxLayoutList::FindCurrentObject(CoordType *offset)
{
   wxLayoutObjectList::iterator obj = end();

   obj = FindObjectCursor(m_CursorPosition, offset);
   if(obj == end())  // not ideal yet
   {
      obj = tail();
      if(obj != end()) // tail really exists
         *offset = (*obj)->CountPositions(); // at the end of it
   }
   return obj;
}

void
wxLayoutList::MoveCursor(int dx, int dy)
{
   CoordType offs, lineLength;
   wxLayoutObjectList::iterator i;


   if(dy > 0 && m_CursorPosition.y < m_MaxLine)
      m_CursorPosition.y += dy;
   else if(dy < 0 && m_CursorPosition.y > 0)
      m_CursorPosition.y += dy; // dy is negative
   if(m_CursorPosition.y < 0)
      m_CursorPosition.y = 0;
   else if (m_CursorPosition.y > m_MaxLine)
      m_CursorPosition.y = m_MaxLine;
   
   while(dx > 0)
   {
      i = FindCurrentObject(&offs);
      lineLength = GetLineLength(i);
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
            break; // cannot move there
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
            lineLength = GetLineLength(i);
            m_CursorPosition.x = lineLength;
            dx++;
            continue;
         }
         else
            break; // cannot move left any more
      }
   }
   // final adjustment:
   i = FindCurrentObject(&offs);
   lineLength = GetLineLength(i);
   if(m_CursorPosition.x > lineLength)
      m_CursorPosition.x = lineLength;
      
#ifdef   WXLAYOUT_DEBUG
   i = FindCurrentObject(&offs);
   cerr << "Cursor: "
        << m_CursorPosition.x << ','
        << m_CursorPosition.y;

   if(i == end())
   {
      cerr << "<<no object found>>" << endl;
      return;  // FIXME we should set cursor position to maximum allowed
      // value then
   }
   if((*i)->GetType() == WXLO_TYPE_TEXT)
   {
      cerr << " \"" << ((wxLayoutObjectText *)(*i))->GetText() << "\", offs: "
           << offs << endl;
   }
   else
      cerr << ' ' << _t[(*i)->GetType()] << endl;
#endif
}

void
wxLayoutList::Delete(CoordType count)
{
   TRACE(Delete);

   if(!m_Editable)
      return;

   VAR(count);

   CoordType offs, len;
   wxLayoutObjectList::iterator i;
      
   do
   {
      i  = FindCurrentObject(&offs);
      if(i == end())
         return;
#ifdef WXLAYOUT_DEBUG
      cerr << "trying to delete: " << _t[(*i)->GetType()] << endl;
#endif
      if((*i)->GetType() == WXLO_TYPE_LINEBREAK)
         m_MaxLine--;
      if((*i)->GetType() == WXLO_TYPE_TEXT)
      {
         wxLayoutObjectText *tobj = (wxLayoutObjectText *)*i;
         len = tobj->CountPositions();
         // If we find the end of a text object, this means that we
         // have to delete from the object following it.
         if(offs == len)
         {
            i++;
            if((*i)->GetType() == WXLO_TYPE_TEXT)
            {
               offs = 0;  // delete from begin of next string
               tobj = (wxLayoutObjectText *)*i;
               len = tobj->CountPositions();
            }
            else
            {
               erase(i);
               return;
            }
         }
         if(len <= count) // delete this object
         {
            count -= len;
            erase(i);
         }
         else
         {
            len = count;
            VAR(offs); VAR(len);
            tobj->GetText().erase(offs,len);
            return; // we are done
         }
      }
      else // delete the object
      {
         len = (*i)->CountPositions();
         erase(i); // after this, i is the iterator for the following object
         if(count > len)
            count -= len;
         else
            count = 0;
      }
   }
   while(count && i != end());      
}

void
wxLayoutList::Insert(wxLayoutObjectBase *obj)
{
   wxASSERT(obj);
   CoordType offs;
   wxLayoutObjectList::iterator i = FindCurrentObject(&offs);

   TRACE(Insert(obj));

   if(i == end())
      push_back(obj);
   else
   {      
      // do we have to split a text object?
      if((*i)->GetType() == WXLO_TYPE_TEXT && offs != 0 && offs != (*i)->CountPositions())
      {
         wxLayoutObjectText *tobj = (wxLayoutObjectText *) *i;
#ifdef WXLAYOUT_DEBUG
         cerr << "text: '" << tobj->GetText() << "'" << endl;
         VAR(offs);
#endif
         String left = tobj->GetText().substr(0,offs); // get part before cursor
         VAR(left);
         tobj->GetText() = tobj->GetText().substr(offs,(*i)->CountPositions()-offs); // keeps the right half
         VAR(tobj->GetText());
         insert(i,obj);
         insert(i,new wxLayoutObjectText(left)); // inserts before
      }
      else
      {
         wxLayoutObjectList::iterator j = i; // we want to apend after this object
         j++;
         if(j != end())
            insert(j, obj);
         else
            push_back(obj);
      }  
   }
   m_CursorPosition.x += obj->CountPositions();
   if(obj->GetType() == WXLO_TYPE_LINEBREAK)
      m_MaxLine++;
}

void
wxLayoutList::Insert(String const &text)
{
   wxLayoutObjectText *tobj = NULL;
   TRACE(Insert(text));

   if(! m_Editable)
      return;

   CoordType offs;
   wxLayoutObjectList::iterator i = FindCurrentObject(&offs);

   if(i != end() && (*i)->GetType() == WXLO_TYPE_TEXT)
   {  // insert into an existing text object:
      TRACE(inserting into existing object);
      tobj = (wxLayoutObjectText *)*i ;
      wxASSERT(tobj);
      tobj->GetText().insert(offs,text);
   }
   else      // check whether the previous object is text:
   {
      wxLayoutObjectList::iterator j = i;
      j--;
      TRACE(checking previous object);
      if(0 && j != end() && (*j)->GetType() == WXLO_TYPE_TEXT)
      {
         tobj = (wxLayoutObjectText *)*i;
         wxASSERT(tobj);
         tobj->GetText()+=text;
      }
      else  // insert a new text object
      {
         TRACE(creating new object);
         Insert(new wxLayoutObjectText(text));  //FIXME not too optimal, slow
         return;  // position gets incremented in Insert(obj)
      }
   }
   m_CursorPosition.x += strlen(text.c_str());
}

CoordType
wxLayoutList::GetLineLength(wxLayoutObjectList::iterator i)
{
   if(i == end())
      return 0;

   CoordType len = 0;

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
   wxLayoutObjectList::iterator i = begin();

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

   m_Position = wxPoint(0,0);
   m_CursorPosition = wxPoint(0,0);
   m_MaxLine = 0;
   m_LineHeight = (BASELINESTRETCH*m_FontPtSize)/10;
   m_MaxX = 0; m_MaxY = 0;

   
   if(m_DefaultSetting)
      delete m_DefaultSetting;
   m_DefaultSetting = new
      wxLayoutObjectCmd(m_FontPtSize,m_FontFamily,m_FontStyle,
                        m_FontWeight,m_FontUnderline,
                        m_ColourFG, m_ColourBG);
}
