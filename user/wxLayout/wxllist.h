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

#include   "wx/wx.h"
#include   "wx/print.h"
#include   "wx/printdlg.h"
#include   "wx/generic/printps.h"
#include   "wx/generic/prntdlgg.h"

// skip the following defines if embedded in M application
#ifdef   M_BASEDIR
#   ifdef   DEBUG
//#      define   WXLAYOUT_DEBUG
#   endif
#else
    // for testing only:
#   define WXLAYOUT_DEBUG
    // The wxLayout classes can be compiled with std::string instead of wxString
    //#   define USE_STD_STRING
#   define WXMENU_LAYOUT_LCLICK     1111
#   define WXMENU_LAYOUT_RCLICK    1112
#   define WXMENU_LAYOUT_DBLCLICK   1113
#endif

#ifdef USE_STD_STRING
#   include   <string>
    typedef   std::string String;
#   define    Str(str)(str.c_str())
#else
    typedef   wxString String;
#   define    Str(str) str
#endif

#define   WXLO_DEFAULTFONTSIZE 12

/// Types of currently supported layout objects.
enum wxLayoutObjectType
{ WXLO_TYPE_INVALID = 0, WXLO_TYPE_TEXT, WXLO_TYPE_CMD, WXLO_TYPE_ICON, WXLO_TYPE_LINEBREAK };

/// Type used for coordinates in drawing.
typedef long CoordType;

class wxLayoutList;
class wxLayoutObjectBase;

class wxDC;
class wxColour;
class wxFont;

/** The base class defining the interface to each object which can be
    part of the layout. Each object needs to draw itself and calculate 
    its size.
*/
class wxLayoutObjectBase
{
public:
   struct UserData
   {
      virtual ~UserData() { }
   };

   /// return the type of this object
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_INVALID; } ;
   /** Calculates the position etc of an object.
       @param dc the wxDC to draw on
       @param position where to draw the top left corner
       @param baseLine the baseline for alignment, from top of box
   */
   virtual void Layout(wxDC & dc,
                       wxPoint position,
                       CoordType baseLine) {}

   /** Draws an object.
       @param dc the wxDC to draw on
       @param translation to be added to coordinates
   */
   virtual void Draw(wxDC & dc, wxPoint const &translate) {}

   /** Calculates and returns the size of the object. 
       @param baseLine pointer where to store the baseline position of 
       this object (i.e. the height from the top of the box to the
       baseline)
       @return the size of the object's box in pixels
   */
   virtual wxPoint GetSize(CoordType * baseLine = NULL) const
      { return wxPoint(0,0); }

   /** Calculates and returns the position of the object.
       @return the size of the object's box in pixels
   */
   virtual wxPoint GetPosition(void) const { return m_Position; }

   /// returns the number of cursor positions occupied by this object
   virtual CoordType CountPositions(void) const { return 1; }

   /// constructor
   wxLayoutObjectBase() { m_UserData = NULL; }
   /// delete the user data
   virtual ~wxLayoutObjectBase() { delete m_UserData; }

#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   /// query whether coordinates have changed since last drawing
   virtual bool IsDirty(void) const { return true; }  
   
   /** Tells the object about some user data. This data is associated
       with the object and will be deleted at destruction time.
   */
   void   SetUserData(UserData *data) { m_UserData = data; }
   /** Return the user data. */
   void * GetUserData(void) const { return m_UserData; }
   
private:
   /// optional data for application's use
   UserData *m_UserData;
protected:
   wxPoint m_Position;
};

/// Define a list type of wxLayoutObjectBase pointers.
KBLIST_DEFINE(wxLayoutObjectList, wxLayoutObjectBase);


/// object for text block
class wxLayoutObjectText : public wxLayoutObjectBase
{
public:
   wxLayoutObjectText(const String &txt);

   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_TEXT; }
   virtual void Layout(wxDC &dc, wxPoint position, CoordType
                       baseLine);
   
   virtual void Draw(wxDC &dc, wxPoint const &translate);
   /** This returns the height and in baseLine the position of the
       text's baseline within it's box. This is needed to properly
       align text objects.
   */
   virtual wxPoint GetSize(CoordType *baseLine = NULL) const;

#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   virtual CoordType CountPositions(void) const { return strlen(m_Text.c_str()); }
   virtual bool IsDirty(void) const { return m_IsDirty; }  

   // for editing:
   String & GetText(void) { return m_Text; }
   void SetText(String const &text) { m_Text = text; }

private:
   String m_Text;
   /// size of the box containing text
   long   m_Width, m_Height;
   /// the position of the baseline counted from the top of the box
   long   m_BaseLine;
   /// coordinates have changed
   bool m_IsDirty;
};

/// icon/pictures:
class wxLayoutObjectIcon : public wxLayoutObjectBase
{
public:
   wxLayoutObjectIcon(wxIcon *icon);
   wxLayoutObjectIcon(wxIcon const &icon);

   ~wxLayoutObjectIcon() { delete m_Icon; }

   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_ICON; }
   virtual void Layout(wxDC &dc, wxPoint position, CoordType baseLine);
   virtual void Draw(wxDC &dc, wxPoint const &translate);

   virtual wxPoint GetSize(CoordType *baseLine = NULL) const;
   virtual bool IsDirty(void) const { return m_IsDirty; }  

private:
   wxIcon *m_Icon;
   /// coordinates have changed
   bool m_IsDirty;
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
   virtual void Draw(wxDC &dc, wxPoint const &translate);
   virtual void Layout(wxDC &dc, wxPoint position, CoordType baseLine);
   wxLayoutObjectCmd(int size, int family, int style, int weight,
                bool underline,
                wxColour const *fg, wxColour const *bg);
   ~wxLayoutObjectCmd();
   /// caller must free pointer:
   wxLayoutStyleInfo *GetStyle(void) const ;
   /// return the background colour for setting colour of window
   wxColour const *GetBGColour(void) const { return m_ColourBG; }
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


class wxLayoutPrintout;

class wxLayoutMargins
{
public:
   wxLayoutMargins() { top = left = 0; bottom = right = -1; }
   int top;
   int left;
   int bottom;
   int right;
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
   /// adds a text object
   void AddText(String const &txt);
   /// adds a line break
   void LineBreak(void);
   /// sets font parameters
   void SetFont(int family, int size, int style,
                int weight, int underline,
                wxColour const *fg,
                wxColour const *bg);
   /// sets font parameters, colours by name
   void SetFont(int family=-1, int size = -1, int style=-1,
                int weight=-1, int underline = -1,
                char const *fg = NULL,
                char const *bg = NULL);
   /// changes to the next larger font size
   inline void SetFontLarger(void)
      { SetFont(-1,(12*m_FontPtSize)/10); }
   /// changes to the next smaller font size
   inline void SetFontSmaller(void)
      { SetFont(-1,(10*m_FontPtSize)/12); }
   
   /// set font family
   inline void SetFontFamily(int family) { SetFont(family); }
   /// set font size
   inline void SetFontSize(int size) { SetFont(-1,size); }
   /// set font style
   inline void SetFontStyle(int style) { SetFont(-1,-1,style); }
   /// set font weight
   inline void SetFontWeight(int weight) { SetFont(-1,-1,-1,weight); }
   /// toggle underline flag
   inline void SetFontUnderline(bool ul) { SetFont(-1,-1,-1,-1,(int)ul); }
   /// set font colours by name
   inline void SetFontColour(char const *fg, char const *bg = NULL) { SetFont(-1,-1,-1,-1,-1,fg,bg); }
      
   
   /** Re-layouts the list on a DC.
       @param dc the dc to layout for
       @param margins if not NULL, use these top and left margins
   */
   void Layout(wxDC &dc, wxLayoutMargins *margins = NULL);
                            
  /** Draw the list on a given DC.
      @param dc the dc to layout for
      @param fromLine the first graphics line from where to draw
      @param toLine the last line at which to draw
      @param start if != iterator(NULL) start drawing from here
   */
   void Draw(wxDC &dc,
             CoordType fromLine = -1,
             CoordType toLine = -1,
             iterator start = iterator(NULL),
             wxPoint const &translate = wxPoint(0,0));

   /** Deletes at least to the end of line and redraws */
   void EraseAndDraw(wxDC &dc, iterator start = iterator(NULL));
   
   /** Finds the object occupying a certain screen position.
       @return pointer to wxLayoutObjectBase or NULL if not found
   */
   wxLayoutObjectBase *Find(wxPoint coords) const;
   
#ifdef WXLAYOUT_DEBUG
   void Debug(void);
   void ShowCurrentObject();
#endif

   /// dirty?
   bool IsDirty() const { return m_bModified; }
   bool CursorMoved(void) const { return m_CursorMoved; }  

   /// called after the contents is saved, for example
   void ResetDirty() { m_bModified = FALSE; }

   /// for access by wxLayoutWindow:
   void GetSize(CoordType *max_x, CoordType *max_y,
                CoordType *lineHeight);

   
   /**@name Functionality for editing */
   //@{
   /// set list editable or read only
   void SetEditable(bool editable = true) { m_Editable = editable; }
   /// return true if list is editable
   bool IsEditable(void) const { return m_Editable; }
   /// move cursor, returns true if it could move to the desired position
   bool MoveCursor(int dx = 0, int dy = 0);
   void SetCursor(wxPoint const &p) { m_CursorPos = p; }
   void DrawCursor(wxDC &dc, bool erase = false);
   
   /// Get current cursor position cursor coords
   wxPoint GetCursor(void) const { return m_CursorPos; }
   /// Gets graphical coordinates of cursor
   wxPoint GetCursorCoords(void) const { return m_CursorCoords; }
   
   /// delete one or more cursor positions
   void Delete(CoordType count = 1);
   void Insert(String const &text);
   void Insert(wxLayoutObjectBase *obj);
   void Clear(int family = wxROMAN, int size=WXLO_DEFAULTFONTSIZE, int style=wxNORMAL, int weight=wxNORMAL,
                    int underline=0, char const *fg="black", char const *bg="white");

   /// return a pointer to the default settings (dangerous, why?) FIXME:
   wxLayoutObjectCmd const *GetDefaults(void) const { return m_DefaultSetting ; }

   wxLayoutObjectList::iterator FindCurrentObject(CoordType *offset = NULL);
   // get the length of the line with the object pointed to by i, offs 
   // only used to decide whether we are before or after linebreak
   CoordType GetLineLength(wxLayoutObjectList::iterator i,
                           CoordType offs = 0);
   wxLayoutPrintout *MakePrintout(wxString const &name);

   /// Return maximum X,Y coordinates
   wxPoint GetSize(void) const { return wxPoint(m_MaxX, m_MaxY); }
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
   /// cursor moved
   bool m_CursorMoved;

   /// needs saving (i.e., was modified?)
   bool m_bModified;

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
   /// where is the text cursor (column,line):
   wxPoint   m_CursorPos;
   /// where to draw the cursor
   wxPoint   m_CursorCoords;
   /// how large to draw it
   wxPoint   m_CursorSize;
   /// object iterator for current cursor position:
   iterator  m_CursorObject;
   /// position of cursor within m_CursorObject:
   int       m_CursorOffset;
   
   /// to store content overwritten by cursor
   wxMemoryDC m_CursorMemDC;

   /// which is the last line
   CoordType m_MaxLine;
   /// can we edit it?
   bool      m_Editable;
   /// find the object to the cursor position and returns the offset
   /// in there
   wxLayoutObjectList::iterator FindObjectCursor(wxPoint *cpos, CoordType *offset = NULL);
private:
   /// Resets the font settings etc to default values
   void ResetSettings(wxDC &dc);
   /// calculates current cursor coordinates, called in Layout()
   void CalculateCursor(wxDC &dc);
   /// remembers the last cursor position for which FindObjectCursor was called
   wxPoint m_FoundCursor;
   /// remembers the iterator to the object related to m_FoundCursor
   wxLayoutObjectList::iterator m_FoundIterator;
};

class wxLayoutPrintout: public wxPrintout
{
 public:
   wxLayoutPrintout(wxLayoutList &llist, wxString const & title =
                    "wxLayout Printout");
   bool OnPrintPage(int page);
   bool HasPage(int page);
   bool OnBeginDocument(int startPage, int endPage);
   void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int
                    *selPageTo);
   void OnPreparePrinting(void);
protected:
   virtual void DrawHeader(wxDC &dc, wxPoint topleft, wxPoint bottomright, int pageno);
                           
private:
   wxLayoutList *m_llist;
   wxString      m_title;
   int           m_PageHeight, m_PageWidth;
   // how much we actually print per page
   int           m_PrintoutHeight;
   wxLayoutMargins m_Margins;
   int           m_NumOfPages;
};

#endif // WXLLIST_H
