/*-*- c++ -*-********************************************************
 * wxLayoutList.h - a formatted text rendering engine for wxWindows *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifndef WXLLIST_H
#define WXLLIST_H

#ifdef __GNUG__
#   pragma interface "wxllist.h"
#endif

#include   "kbList.h"

#include   <wx/wx.h>

// skip the following defines if embedded in M application
#ifndef   MCONFIG_H
// for testing only:
#   define WXLAYOUT_DEBUG
#   define USE_STD_STRING
#endif

#ifdef USE_STD_STRING
#   include   <string>
    typedef   std::string String;
#   define    Str(str)(str.c_str())
#else
    typedef   wxString String;
#   define    Str(str) str
#endif

enum wxLayoutObjectType { WXLO_TYPE_INVALID, WXLO_TYPE_TEXT, WXLO_TYPE_CMD, WXLO_TYPE_ICON, WXLO_TYPE_LINEBREAK };

typedef long CoordType;

class wxLayoutList;
class wxLayoutObjectBase;

KBLIST_DEFINE(wxLayoutObjectList, wxLayoutObjectBase);
KBLIST_DEFINE(wxLayoutOLinesList, wxLayoutObjectList::iterator);


class wxLayoutObjectBase
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_INVALID; } ;
   /** Draws an object.
       @param dc the wxDC to draw on
       @param position where to draw the top left corner
       @param baseLine the baseline for alignment, from top of box
       @draw if set to false, do not draw but just calculate sizes
   */
   virtual void Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                     bool draw = true) {};

   virtual wxPoint GetSize(CoordType *baseLine) const { return
                                                           wxPoint(0,0); };
   /// returns the number of cursor positions occupied by this object
   virtual CoordType CountPositions(void) const { return 1; }

   wxLayoutObjectBase() { m_UserData = NULL; }
   virtual ~wxLayoutObjectBase() { if(m_UserData) delete m_UserData; }
#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   void   SetUserData(void *data) { m_UserData = data; }
   void * GetUserData(void) const { return m_UserData; }
private:
   /// optional data for application's use
   void * m_UserData;
};

/// object for text block
class wxLayoutObjectText : public wxLayoutObjectBase
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_TEXT; }
   virtual void Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                     bool draw = true);
   /** This returns the height and in baseLine the position of the
       text's baseline within it's box. This is needed to properly
       align text objects.
   */
   virtual wxPoint GetSize(CoordType *baseLine) const;
#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   wxLayoutObjectText(const String &txt);
   virtual CoordType CountPositions(void) const { return strlen(m_Text.c_str()); }

   // for editing:
   String & GetText(void) { return m_Text; }
   void SetText(String const &text) { m_Text = text; }
private:
   String m_Text;
   /// size of the box containing text
   long   m_Width, m_Height;
   /// the position of the baseline counted from the top of the box
   long   m_BaseLine;
};

/// icon/pictures:
class wxLayoutObjectIcon : public wxLayoutObjectBase
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_ICON; }
   virtual void Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                     bool draw = true);
   virtual wxPoint GetSize(CoordType *baseLine) const;
   wxLayoutObjectIcon(wxIcon *icon);
private:
   wxIcon * m_Icon;
};

/// for export to html:
struct wxLayoutStyleInfo
{
   int  size, family, style, weight;
   bool underline;
   unsigned fg_red, fg_green, fg_blue;
   unsigned bg_red, bg_green, bg_blue;
};

/// pseudo-object executing a formatting command in Draw()
class wxLayoutObjectCmd : public wxLayoutObjectBase
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_CMD; }
   virtual void Draw(wxDC &dc, wxPoint position, CoordType baseLine,
                     bool draw = true);
   wxLayoutObjectCmd(int size, int family, int style, int weight,
                bool underline,
                wxColour const *fg, wxColour const *bg);
   ~wxLayoutObjectCmd();
   // caller must free pointer:
   wxLayoutStyleInfo *GetStyle(void) const ;
private:
   /// the font to use
   wxFont *m_font;
   /// foreground colour
   wxColour const *m_ColourFG;
   /// background colour
   wxColour const *m_ColourBG;
};

/// this object doesn't do anything at all
class wxLayoutObjectLineBreak : public wxLayoutObjectBase
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_LINEBREAK; }
};


/**
   This class provides a high level abstraction to the wxFText
   classes.
   It handles most of the character events with its own callback
   functions, providing an editing ability. All events which cannot be
   handled get passed to the parent window's handlers.
*/
class wxLayoutList : public wxLayoutObjectList
{
public:
   wxLayoutList();
   
   /// Destructor.
   ~wxLayoutList();

   /// adds an object:
   void AddObject(wxLayoutObjectBase *obj);
   void AddText(String const &txt);

   void LineBreak(void);
   void SetFont(int family, int size, int style,
                int weight, int underline,
                wxColour const *fg,
                wxColour const *bg);
   void SetFont(int family=-1, int size = -1, int style=-1,
                int weight=-1, int underline = -1,
                char const *fg = NULL,
                char const *bg = NULL);
   inline void SetFontFamily(int family) { SetFont(family); }
   inline void SetFontSize(int size) { SetFont(-1,size); }
   inline void SetFontStyle(int style) { SetFont(-1,-1,style); }
   inline void SetFontWeight(int weight) { SetFont(-1,-1,-1,weight); }
   inline void SetFontUnderline(bool ul) { SetFont(-1,-1,-1,-1,(int)ul); }
   inline void SetFontColour(char const *fg, char const *bg = NULL) { SetFont(-1,-1,-1,-1,-1,fg,bg); }
      
   
   /** Draw the list on a given DC.
       @param findObject if true, return the object occupying the
       position specified by coords
       @param coords position where to find the object
       @return if findObject == true, the object or NULL
   */
   wxLayoutObjectBase *Draw(wxDC &dc, bool findObject = false,
                       wxPoint const &coords = wxPoint(0,0));

#ifdef WXLAYOUT_DEBUG
   void Debug(void);
#endif

   
   /// for access by wxLayoutWindow:
   void GetSize(CoordType *max_x, CoordType *max_y,
                CoordType *lineHeight);

   /**@name Functionality for editing */
   //@{
   /// set list editable or read only
   void SetEditable(bool editable = true) { m_Editable = true; }
   /// move cursor
   void MoveCursor(int dx = 0, int dy = 0);
   void SetCursor(wxPoint const &p) { m_CursorPosition = p; }
   /// delete one or more cursor positions
   void Delete(CoordType count = 1);
   void Insert(String const &text);
   void Insert(wxLayoutObjectBase *obj);
   void Clear(int family = wxROMAN, int size=12, int style=wxNORMAL, int weight=wxNORMAL,
                    int underline=0, char const *fg="black", char const *bg="white");

   //@}
protected:
   /// font parameters:
   int m_FontFamily, m_FontStyle, m_FontWeight;
   int m_FontPtSize;
   bool m_FontUnderline;
   /// colours:
   wxColour const * m_ColourFG;
   wxColour const * m_ColourBG;
   /// the default setting:
   wxLayoutObjectCmd *m_DefaultSetting;
   
   /// needs recalculation?
   bool m_dirty;

   // the currently updated line:
   /// where do we draw next:
   wxPoint   m_Position;
   /// the height of the current line:
   CoordType m_LineHeight;
   /// maximum drawn x position so far
   CoordType m_MaxX;
   /// maximum drawn y position:
   CoordType m_MaxY;

   //---- this is needed for editing:
   /// where is the text cursor:
   wxPoint   m_CursorPosition;
   /// which is the last line
   CoordType m_MaxLine;
   /// can we edit it?
   bool      m_Editable;
   /// find the object to the cursor position and returns the offset
   /// in there
   wxLayoutObjectList::iterator FindObjectCursor(wxPoint const &cpos, CoordType *offset = NULL);
   wxLayoutObjectList::iterator FindCurrentObject(CoordType *offset = NULL);
   // get the length of the line with the object pointed to by i
   CoordType GetLineLength(wxLayoutObjectList::iterator i);
   
};

#endif // WXLLIST_H
