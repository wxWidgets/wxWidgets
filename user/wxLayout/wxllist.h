/*-*- c++ -*-********************************************************
 * wxLayoutList.h - a formatted text rendering engine for wxWindows *
 *                                                                  *
 * (C) 1999 by Karsten Ballüder (Ballueder@usa.net)                 *
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
#ifndef   M_BASEDIR
#   define WXMENU_LAYOUT_LCLICK     1111
#   define WXMENU_LAYOUT_RCLICK     1112
#   define WXMENU_LAYOUT_DBLCLICK   1113
#endif

#ifdef   __WXDEBUG__
#   define   WXLAYOUT_DEBUG
#endif

#ifdef WXLAYOUT_DEBUG
#   define WXLO_TRACE(x)   wxLogDebug(x)
#else
#   define WXLO_TRACE(x)   
#endif



#ifndef WXLO_DEFAULTFONTSIZE
#   define WXLO_DEFAULTFONTSIZE 12
#endif


/// Types of currently supported layout objects.
enum wxLayoutObjectType
{
   /// illegal object type, should never appear
   WXLO_TYPE_INVALID = 0,
   /// text object, containing normal text
   WXLO_TYPE_TEXT,
   /// command object, containing font or colour changes
   WXLO_TYPE_CMD,
   /// icon object, any kind of image
   WXLO_TYPE_ICON
};

/// Type used for coordinates in drawing. Must be signed.
typedef long CoordType;

// Forward declarations.
class wxLayoutList;
class wxLayoutObject;
class wxDC;
class wxColour;
class wxFont;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   The wxLayout objects which make up the lines.

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
                                                                               
/** The base class defining the interface to each object which can be
    part of the layout. Each object needs to draw itself and calculate 
    its size.
*/
class wxLayoutObject
{
public:
   /// This structure can be used to contain data associated with the object.
   struct UserData
   {
      virtual ~UserData() { }
   };

   /// return the type of this object
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_INVALID; }
   /** Calculates the size of an object.
       @param dc the wxDC to draw on
   */
   virtual void Layout(wxDC &) = 0;

   /** Draws an object.
       @param dc the wxDC to draw on
       @param coords where to draw the baseline of the object.
   */
   virtual void Draw(wxDC & /* dc */, wxPoint const & /* coords */)  { }

   /** Calculates and returns the size of the object. 
       @param top where to store height above baseline
       @param bottom where to store height below baseline
       @return the size of the object's box in pixels
   */
   virtual wxPoint GetSize(CoordType * top, CoordType *bottom) const
      { *top = 0; *bottom = 0; return wxPoint(0,0); }

   /// Return just the width of the object on the screen.
   virtual CoordType GetWidth(void) const { return 0; }
   /// returns the number of cursor positions occupied by this object
   virtual CoordType GetLength(void) const { return 1; }
   /** Returns the cursor offset relating to the screen x position
       relative to begin of object.
       @param dc the wxDC to use for calculations
       @param xpos relative x position from head of object
       @return cursor coordinate offset
   */
   virtual CoordType GetOffsetScreen(wxDC &dc, CoordType xpos) const { return 0; }

   /// constructor
   wxLayoutObject() { m_UserData = NULL; }
   /// delete the user data
   virtual ~wxLayoutObject() { if(m_UserData) delete m_UserData; }

#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   /** Tells the object about some user data. This data is associated
       with the object and will be deleted at destruction time.
   */
   void   SetUserData(UserData *data) { m_UserData = data; }
   /** Return the user data. */
   void * GetUserData(void) const { return m_UserData; }
   
private:
   /// optional data for application's use
   UserData *m_UserData;
};

/// Define a list type of wxLayoutObject pointers.
KBLIST_DEFINE(wxLayoutObjectList, wxLayoutObject);

/// An illegal iterator to save typing.
#define NULLIT (wxLayoutObjectList::iterator(NULL))
/// The iterator type.
#define wxLOiterator   wxLayoutObjectList::iterator

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   wxLayoutObjectText

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** This class implements a wxLayoutObject holding plain text.
 */
class wxLayoutObjectText : public wxLayoutObject
{
public:
   wxLayoutObjectText(const wxString &txt);

   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_TEXT; }
   virtual void Layout(wxDC &dc);
   virtual void Draw(wxDC &dc, wxPoint const &coords);
   /** Calculates and returns the size of the object. 
       @param top where to store height above baseline
       @param bottom where to store height below baseline
       @return the size of the object's box in pixels
   */
   virtual wxPoint GetSize(CoordType * top, CoordType *bottom) const;
   /// Return just the width of the object on the screen.
   virtual CoordType GetWidth(void) const { return m_Width; }
   /** Returns the cursor offset relating to the screen x position
       relative to begin of object.
       @param dc the wxDC to use for calculations
       @param xpos relative x position from head of object
       @return cursor coordinate offset
   */
   virtual CoordType GetOffsetScreen(wxDC &dc, CoordType xpos) const;


#ifdef WXLAYOUT_DEBUG
   virtual void Debug(void);
#endif

   virtual CoordType GetLength(void) const { return strlen(m_Text.c_str()); }

   // for editing:
   wxString & GetText(void) { return m_Text; }
   void SetText(wxString const &text) { m_Text = text; }

private:
   wxString m_Text;
   /// size of the box containing text
   long   m_Width, m_Height;
   /// Height above baseline.
   long   m_Top;
   /// Height below baseline.
   long   m_Bottom;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   wxLayoutObjectIcon

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** This class implements a wxLayoutObject holding a graphic.
 */
class wxLayoutObjectIcon : public wxLayoutObject
{
public:
   wxLayoutObjectIcon(wxBitmap *icon);
   wxLayoutObjectIcon(wxBitmap const &icon);

   ~wxLayoutObjectIcon() { delete m_Icon; }

   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_ICON; }
   virtual void Layout(wxDC &dc);
   virtual void Draw(wxDC &dc, wxPoint const &coords);

   /** Calculates and returns the size of the object. 
       @param top where to store height above baseline
       @param bottom where to store height below baseline
       @return the size of the object's box in pixels
   */
   virtual wxPoint GetSize(CoordType * top, CoordType *bottom) const;
   /// Return just the width of the object on the screen.
   virtual CoordType GetWidth(void) const { return m_Icon->GetWidth(); }

private:
   wxBitmap *m_Icon;
};

/// for export to html:
struct wxLayoutStyleInfo
{
   wxLayoutStyleInfo()
      {
         family = -1; // this marks the styleinfo as uninitialised
      }
   int  size, family, style, weight;
   bool underline;
   unsigned fg_red, fg_green, fg_blue;
   unsigned bg_red, bg_green, bg_blue;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   wxLayoutObjectCmd

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** This class implements a wxLayoutObject holding style change commands.
 */
class wxLayoutObjectCmd : public wxLayoutObject
{
public:
   virtual wxLayoutObjectType GetType(void) const { return WXLO_TYPE_CMD; }
   virtual void Layout(wxDC &dc);
   virtual void Draw(wxDC &dc, wxPoint const &coords);
   wxLayoutObjectCmd(int size, int family, int style, int weight,
                bool underline,
                wxColour const *fg, wxColour const *bg);
   ~wxLayoutObjectCmd();
   /** Stores the current style in the styleinfo structure */
   void GetStyle(wxLayoutStyleInfo *si) const;
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

   The wxLayoutLine object

   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** This class represents a single line of objects to be displayed.
    It knows its height and total size and whether it needs to be
    redrawn or not.
    It has pointers to its first and next line so it can automatically 
    update them as needed.
*/
class wxLayoutLine
{
public:
   /** Constructor.
       @param prev pointer to previous line or NULL
       @param next pointer to following line or NULL
   */
   wxLayoutLine(wxLayoutLine *prev);
   /** This function inserts a new object at cursor position xpos.
       @param xpos where to insert new object
       @param obj  the object to insert
       @return true if that xpos existed and the object was inserted
   */
   bool Insert(CoordType xpos, wxLayoutObject *obj);
   
   /** This function inserts text at cursor position xpos.
       @param xpos where to insert
       @param text  the text to insert
       @return true if that xpos existed and the object was inserted
   */
   bool Insert(CoordType xpos, wxString text);

   /** This function appends an object to the line.
       @param obj  the object to insert
   */
   void Append(wxLayoutObject * obj)
      {
         wxASSERT(obj);
         m_ObjectList.push_back(obj);
         m_Length += obj->GetLength();
      }

   /** This function appens the next line to this, i.e. joins the two
       lines into one.
   */
   void MergeNextLine(void);

   /** This function deletes npos cursor positions from position xpos.
       @param xpos where to delete
       @param npos how many positions
       @return number of positions still to be deleted
   */
   CoordType Delete(CoordType xpos, CoordType npos);

   /** This function breaks the line at a given cursor position.
       @param xpos where to break it
       @return pointer to the new line object replacing the old one
   */
   wxLayoutLine *Break(CoordType xpos);

   /** Deletes the next word from this position, including leading
       whitespace.
       This function does not delete over font changes, i.e. a word
       with formatting instructions in the middle of it is treated as
       two (three actually!) words. In fact, if the cursor is on a non-text object, that 
       one is treated as a word.
       @param xpos from where to delete
       @return true if a word was deleted
   */
   bool DeleteWord(CoordType npos);

   /** Finds a suitable position left to the given column to break the 
       line.
       @param column we want to break the line to the left of this
       @return column for breaking line or -1 if no suitable location found
   */
   CoordType GetWrapPosition(CoordType column);
   
   /** Finds the object which covers the cursor position xpos in this
       line.
       @param xpos the column number
       @param offset where to store the difference between xpos and
       the object's head
       @return iterator to the object or NULLIT
   */
   wxLayoutObjectList::iterator FindObject(CoordType xpos, CoordType
                                           *offset) const ;

   /** Finds the object which covers the screen position xpos in this
       line.
       @param dc the wxDC to use for calculations
       @param xpos the screen x coordinate
       @param offset where to store the difference between xpos and
       the object's head
       @return iterator to the object or NULLIT
   */
   wxLayoutObjectList::iterator FindObjectScreen(wxDC &dc,
                                                 CoordType xpos,
                                                 CoordType *offset) const ;

   /** Get the first object in the list. This is used by the wxlparser 
       functions to export the list.
       @return iterator to the first object
   */
   wxLayoutObjectList::iterator GetFirstObject(void)
      {
         return m_ObjectList.begin();
      }
       
   /** Deletes this line, returns pointer to next line.
       @param update If true, update all following lines.
   */
   wxLayoutLine *DeleteLine(bool update);

   /**@name Cursor Management */
   //@{
   /** Return the line number of this line.
       @return the line number
   */
   CoordType GetLineNumber(void) const { return m_LineNumber; }
   /** Return the length of the line.
       @return line lenght in cursor positions
   */
   CoordType GetLength(void) const { return m_Length; }
   //@}

   /**@name Drawing and Layout */
   //@{
   /** Draws the line on a wxDC.
       @param dc the wxDC to draw on
       @param offset an optional offset to shift printout
   */
   void Draw(wxDC &dc, const wxPoint &offset = wxPoint(0,0)) const;
   
   /** Recalculates the positions of objects and the height of the
       line.
       @param dc the wxDC to draw on
       @param cursorPos if not NULL, set cursor screen position in there
       @param cursorSize if not cursorPos != NULL, set cursor size in there
       @param cx if cursorPos != NULL, the cursor x position
   */
   void Layout(wxDC &dc,
               wxPoint *cursorPos = NULL,
               wxPoint *cursorSize = NULL,
               int cx = 0);
   /** This function finds an object belonging to a given cursor
       position. It assumes that Layout() has been called before.
       @param dc the wxDC to use for calculations
       @param xpos screen x position
       @return pointer to the object
   */
   wxLayoutObject * FindObjectScreen(wxDC &dc, CoordType xpos);
   //@}

   /**@name List traversal */
   //@{
   /// Returns pointer to next line.
   wxLayoutLine *GetNextLine(void) const { return m_Next; }
   /// Returns pointer to previous line.
   wxLayoutLine *GetPreviousLine(void) const { return m_Previous; }
   /// Sets the link to the next line.
   void SetNext(wxLayoutLine *next)
      { m_Next = next; if(next) next->m_Previous = this; }
   /// Sets the link to the previous line.
   void SetPrevious(wxLayoutLine *previous)
      { m_Previous = previous; if(previous) previous->m_Next = this; }
   //@}

   /// Returns the position of this line on the canvas.
   wxPoint GetPosition(void) const { return m_Position; }
   /// Returns the height of this line.
   CoordType GetHeight(void) const { return m_Height; }
   /// Returns the width of this line.
   CoordType GetWidth(void) const { return m_Width; }
   /** This will recalculate the position and size of this line.
       If called recursively it will abort if the position of an
       object is unchanged, assuming that none of the following
       objects need to move.
       @param recurse if greater 0 then it will be used as the
       minimum(!) recursion level, continue with all lines till the end of
       the list or until the coordinates no longer changed.
   */
   void RecalculatePositions(int recurse = 0);
   /// Recalculates the position of this line on the canvas.
   wxPoint RecalculatePosition(void);
private:
   /// Destructor is private. Use DeleteLine() to remove it.
   ~wxLayoutLine();

   /**@name Functions to let the lines synchronise with each other. */
   //@{
   /** Sets the height of this line. Will mark following lines as
       dirty.
       @param height new height
   */
   void SetHeight(CoordType height)
      { m_Height = height; RecalculatePositions(true); }

   /** Moves the linenumbers one on, because a line has been inserted
       or deleted.
       @param delta either +1 or -1
    */
   void MoveLines(int delta)
      {
         m_LineNumber += delta;
         if(m_Next) m_Next->MoveLines(delta);
      }
   //@}
private:
   /// The line number.
   CoordType m_LineNumber;
   /// The line length in cursor positions.
   CoordType m_Length;
   /// The total height of the line.
   CoordType m_Height;
   /// The total width of the line on screen.
   CoordType m_Width;
   /// The baseline for drawing objects
   CoordType m_BaseLine;
   /// The position on the canvas.
   wxPoint   m_Position;
   /// The list of objects
   wxLayoutObjectList m_ObjectList;
   /// Have we been changed since the last layout?
   bool m_Dirty;
   /// Pointer to previous line if it exists.
   wxLayoutLine *m_Previous;
   /// Pointer to next line if it exists.
   wxLayoutLine *m_Next;
   /// Just to suppress gcc compiler warnings.
   friend class dummy;
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   
   The wxLayoutList object
   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** The wxLayoutList is a list of wxLayoutLine objects. It provides a
    higher level of abstraction for the text and can generally be considered 
    as representing "the text". 
 */
class wxLayoutList
{
public:
   /// Constructor.
   wxLayoutList();
   /// Destructor.
   ~wxLayoutList();

   /// Clear the list.
   void Clear(int family = wxROMAN,
              int size=WXLO_DEFAULTFONTSIZE,
              int style=wxNORMAL,
              int weight=wxNORMAL,
              int underline=0,
              char const *fg="black",
              char const *bg="white");

   /**@name Cursor Management */
   //@{
   /** Set new cursor position.
       @param p new position
       @return bool if it could be set
   */
   bool MoveCursorTo(wxPoint const &p);
   /** Move cursor up or down.
       @param n
       @return bool if it could be moved
   */
   bool MoveCursorVertically(int n);
   /** Move cursor left or right.
       @param n
       @return bool if it could be moved
   */
   bool MoveCursorHorizontally(int n);

   /// Move cursor to end of line.
   void MoveCursorToEndOfLine(void)
      {
         wxASSERT(m_CursorLine);
         MoveCursorHorizontally(m_CursorLine->GetLength()-m_CursorPos.x); 
      }
   
   /// Move cursor to begin of line.
   void MoveCursorToBeginOfLine(void)
      { MoveCursorHorizontally(-m_CursorPos.x); }

   /// Returns current cursor position.
   wxPoint GetCursorPos(void) const { return m_CursorPos; }
   //@}

   /**@name Editing functions.
    All of these functions return true on success and false on
    failure. */
   //@{
   /// Insert text at current cursor position.
   bool Insert(wxString const &text);
   /// Insert some other object at current cursor position.
   bool Insert(wxLayoutObject *obj);
   /// Inserts a linebreak at current cursor position.
   bool LineBreak(void);
   /** Wraps the current line. Searches to the left of the cursor to
       break the line. Does nothing if the cursor position is before
       the break position parameter.
       @param column the break position for the line, maximum length
       @return true if line got broken
   */
   bool WrapLine(CoordType column);
   /** This function deletes npos cursor positions.
       @param npos how many positions
       @return true if everything got deleted
   */
   bool Delete(CoordType npos);

   /** Delete the next n lines.
       @param n how many lines to delete
       @return how many it could not delete
   */
   int DeleteLines(int n);
   
   /// Delete to end of line.
   void DeleteToEndOfLine(void)
      {
         wxASSERT(m_CursorLine);
         Delete(m_CursorLine->GetLength()-m_CursorPos.x);
      }
   /// Delete to begin of line.
   void DeleteToBeginOfLine(void)
      {
         wxASSERT(m_CursorLine);
         CoordType n = m_CursorPos.x;
#ifdef WXLAYOUT_DEBUG
         wxASSERT(MoveCursorHorizontally(-n));
#else
         MoveCursorHorizontally(-n);
#endif
         Delete(n);
      }

   /** Delete the next word.
   */
   void DeleteWord(void)
      {
         wxASSERT(m_CursorLine);
         m_CursorLine->DeleteWord(m_CursorPos.x);
      }

   //@}

   /**@name Formatting options */
   //@{
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
   /**
      Returns a pointer to the default settings.
      This is only valid temporarily and should not be stored
      anywhere.
      @return the default settings of the list
   */
   wxLayoutObjectCmd const *GetDefaults(void) const { return m_DefaultSetting ; }
   //@}

   /**@name Drawing */
   //@{
   /** Draws the complete list on a wxDC.
       @param dc the wxDC to draw on
       @param offset an optional offset to shift printout
       @param top optional y coordinate where to start drawing
       @param bottom optional y coordinate where to stop drawing
   */
   void Draw(wxDC &dc, const wxPoint &offset = wxPoint(0,0),
             CoordType top = -1, CoordType bottom = -1) const;

   /** Calculates new layout for the list, like Draw() but does not
       actually draw it.
       @param dc the wxDC to draw on
       @param bottom optional y coordinate where to stop calculating
   */
   void Layout(wxDC &dc, CoordType bottom = -1) const;

   /** Calculates new sizes for everything in the list, like Layout()
       but this is needed after the list got changed.
       @param dc the wxDC to draw on
       @param bottom optional y coordinate where to stop calculating
   */
   void Recalculate(wxDC &dc, CoordType bottom = -1) const;
   
   /** Returns the size of the list in screen coordinates.
       The return value only makes sense after the list has been
       drawn.
       @return a wxPoint holding the maximal x/y coordinates used for
       drawing
   */
   wxPoint GetSize(void) const;

   /** Returns the cursor position on the screen.
       @return cursor position in pixels
   */
   wxPoint GetCursorScreenPos(void) const
      { return m_CursorScreenPos; }
   
   /** Draws the cursor.
       @param active If true, draw a bold cursor to mark window as
       active.
       @param translate optional translation of cursor coords on screen
   */
   void DrawCursor(wxDC &dc,
                   bool active = true,
                   const wxPoint & translate = wxPoint(0,0));

   /** This function finds an object belonging to a given screen
       position. It assumes that Layout() has been called before.
       @param pos screen position
       @param cursorPos if non NULL, store cursor position in there
       @return pointer to the object
   */
   wxLayoutObject * FindObjectScreen(wxDC &dc,
                                     wxPoint const pos,
                                     wxPoint *cursorPos = NULL);

   //@}

   /**@name For exporting one object after another. */
   //@{
   /** Returns a pointer to the first line in the list. */
   wxLayoutLine *GetFirstLine(void)
      {
         wxASSERT(m_FirstLine);
         return m_FirstLine;
      }
   //@}
private:
   /// Clear the list.
   void InternalClear(void);
   
   /// The list of lines.
   wxLayoutLine *m_FirstLine;
   /**@name Cursor Management */
   //@{
   /// Where the text cursor (column,line) is.
   wxPoint   m_CursorPos;
   /// The size of the cursor.
   wxPoint   m_CursorSize;
   /// Where the cursor should be drawn.
   wxPoint   m_CursorScreenPos;
   /// The line where the cursor is.
   wxLayoutLine *m_CursorLine;
   //@}   

   /** @name Font parameters. */
   //@{
   int m_FontFamily, m_FontStyle, m_FontWeight;
   int m_FontPtSize;
   bool m_FontUnderline;
   /// colours:
   wxColour const * m_ColourFG;
   wxColour const * m_ColourBG;
   /// the default setting:
   wxLayoutObjectCmd *m_DefaultSetting;
   //@}
};




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   
   The wxLayoutPrintout object for printing within the wxWindows print
   framework.
   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** This class implements a wxPrintout for printing a wxLayoutList within
    the wxWindows printing framework.
 */
class wxLayoutPrintout: public wxPrintout
{
public:
   /** Constructor.
       @param llist pointer to the wxLayoutList to be printed
       @param title title for PS file or windows
   */
   wxLayoutPrintout(wxLayoutList *llist,
                    wxString const & title =
                    "wxLayout Printout");
   /** Function which prints the n-th page.
       @param page the page number to print
       @return bool true if we are not at end of document yet
   */
   bool OnPrintPage(int page);
   /** Checks whether page exists in document.
       @param page number of page
       @return true if page exists
   */
   bool HasPage(int page);

   /** Gets called from wxWindows to find out which pages are existing.
       I'm not totally sure about the parameters though.
       @param minPage the first page in the document
       @param maxPage the last page in the document
       @param selPageFrom the first page to be printed
       @param selPageTo the last page to be printed
   */
   void GetPageInfo(int *minPage, int *maxPage,
                    int *selPageFrom, int *selPageTo);
protected:
   /** This little function scales the DC so that the printout has
       roughly the same size as the output on screen.
       @param dc the wxDC to scale
       @return the scale that was applied
   */
   float ScaleDC(wxDC *dc);

   /* no longer used
     virtual void DrawHeader(wxDC &dc, wxPoint topleft, wxPoint bottomright, int pageno);
   */                      
private:
   /// The list to print.
   wxLayoutList *m_llist;
   /// Title for PS file or window.
   wxString      m_title;
   /// The real paper size.
   int           m_PageHeight, m_PageWidth;
   /// How much we actually print per page.
   int           m_PrintoutHeight;
   /// How many pages we need to print.
   int           m_NumOfPages;
   /// Top left corner where we start printing.
   wxPoint       m_Offset;
};


#endif // WXLLIST_H
