/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998-1999 by Karsten Ballüder (karsten@phy.hw.ac.uk)         *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifndef WXLWINDOW_H
#define WXLWINDOW_H

#ifdef __GNUG__
#   pragma interface "wxlwindow.h"
#endif

#ifndef USE_PCH
#  include   <wx/wx.h>
#endif

#include "wxllist.h"

#ifndef WXLOWIN_MENU_FIRST
#   define WXLOWIN_MENU_FIRST 12000
#endif


#define wxUSE_PRIVATE_CLIPBOARD_FORMAT 0

enum
{
   WXLOWIN_MENU_LARGER = WXLOWIN_MENU_FIRST,
   WXLOWIN_MENU_SMALLER,
   WXLOWIN_MENU_UNDERLINE,
   WXLOWIN_MENU_BOLD,
   WXLOWIN_MENU_ITALICS,
   WXLOWIN_MENU_ROMAN,
   WXLOWIN_MENU_TYPEWRITER,
   WXLOWIN_MENU_SANSSERIF,
   WXLOWIN_MENU_RCLICK,
   WXLOWIN_MENU_DBLCLICK,
   WXLOWIN_MENU_MDOWN,
   WXLOWIN_MENU_LDOWN,
   WXLOWIN_MENU_LCLICK = WXLOWIN_MENU_LDOWN,
   WXLOWIN_MENU_LUP,
   WXLOWIN_MENU_MOUSEMOVE,
   WXLOWIN_MENU_LAST = WXLOWIN_MENU_MOUSEMOVE
};

/**
   This class is a rich text editing widget.
*/
class wxLayoutWindow : public wxScrolledWindow
{
public:
   /** Constructor.
       @param parent parent window to display this panel in
   */
   wxLayoutWindow(wxWindow *parent);

   /// Destructor.
   virtual ~wxLayoutWindow();

   /**@name Editing functionality */
   //@{
   /// Clears the window and sets default parameters.
   void Clear(int family = wxROMAN,
              int size=12,
              int style=wxNORMAL,
              int weight=wxNORMAL,
              int underline=0,
              wxColour *fg=NULL,
              wxColour *bg=NULL);

   /// override base class virtual to also refresh the scrollbar position
   virtual void Refresh(bool eraseBackground = TRUE,
                        const wxRect *rect = (const wxRect *)NULL);

   /** Sets a background image, only used on screen, not on printouts.
       @param bitmap a pointer to a wxBitmap or NULL to remove it
   */
   void SetBackgroundBitmap(wxBitmap *bitmap = NULL)
      {
         if(m_BGbitmap) delete m_BGbitmap;
         m_BGbitmap = bitmap;
      }
   /// Enable or disable editing, i.e. processing of keystrokes.
   void SetEditable(bool toggle)
      { m_Editable = toggle; SetCursorVisibility(toggle); }
   /// Query whether list can be edited by user.
   bool IsEditable(void) const { return m_Editable; }
   /** Sets cursor visibility, visible=1, invisible=0,
       visible-on-demand=-1, to hide it until moved.
       @param visibility -1,0 or 1
       @return the old visibility
   */
   inline int SetCursorVisibility(int visibility = -1)
      { int v =m_CursorVisibility;
      m_CursorVisibility = visibility; return v;}

   /// Pastes text from clipboard.
   void Paste(bool usePrimarySelection = FALSE);
   /** Copies selection to clipboard.
       @param invalidate used internally, see wxllist.h for details
   */
   bool Copy(bool invalidate = true);
   /// Copies selection to clipboard and deletes it.
   bool Cut(void);
   //@}

#ifdef M_BASEDIR
   /// find string in buffer
   bool Find(const wxString &needle,
             wxPoint * fromWhere = NULL,
             const wxString &configPath = "MsgViewFindString");
   /// find the same string again
   bool FindAgain(void);
#endif
   

   void EnablePopup(bool enable = true) { m_DoPopupMenu = enable; }

   /** Sets the wrap margin.
       @param margin set this to 0 to disable it
   */
   void SetWrapMargin(CoordType margin) { m_WrapMargin = margin; }

   /** Redraws the window.
       Internally, this stores the parameter and calls a refresh on
       wxMSW, draws directly on wxGTK.
   */
   void RequestUpdate(const wxRect *updateRect = NULL);

   /// if exact == false, assume 50% extra size for the future
   void ResizeScrollbars(bool exact = false);  // don't change this to true!

   /// if the flag is true, we send events when user clicks on embedded objects
   inline void SetMouseTracking(bool doIt = true) { m_doSendEvents = doIt; }

   /* Returns a pointer to the wxLayoutList object.
      @return the list
   */
   wxLayoutList * GetLayoutList(void) { return m_llist; }

   /**@name Callbacks */
   //@{
   void OnSize(wxSizeEvent &event);
   void OnPaint(wxPaintEvent &event);
   void OnChar(wxKeyEvent& event);
   void OnKeyUp(wxKeyEvent& event);
   void OnUpdateMenuUnderline(wxUpdateUIEvent& event);
   void OnUpdateMenuBold(wxUpdateUIEvent& event);
   void OnUpdateMenuItalic(wxUpdateUIEvent& event);
   void OnMenu(wxCommandEvent& event);
   void OnLeftMouseDown(wxMouseEvent& event)   { OnMouse(WXLOWIN_MENU_LDOWN, event); }
   void OnLeftMouseUp(wxMouseEvent& event)     { OnMouse(WXLOWIN_MENU_LUP, event); }
   void OnRightMouseClick(wxMouseEvent& event) { OnMouse(WXLOWIN_MENU_RCLICK, event); }
   void OnMiddleMouseDown(wxMouseEvent& event) { OnMouse(WXLOWIN_MENU_MDOWN, event); }
   void OnMouseDblClick(wxMouseEvent& event)   { OnMouse(WXLOWIN_MENU_DBLCLICK, event); }
   void OnMouseMove(wxMouseEvent &event)       { OnMouse(WXLOWIN_MENU_MOUSEMOVE, event) ; }
   void OnSetFocus(wxFocusEvent &ev);
   void OnKillFocus(wxFocusEvent &ev);
   //@}

   /// Creates a wxMenu for use as a format popup.
   static wxMenu * MakeFormatMenu(void);
   /// Redraws the window, used by RequestUpdate() or OnPaint().
   void InternalPaint(const wxRect *updateRect);

   /** Tell window to update a wxStatusBar with UserData labels and
       cursor positions.
       @param bar wxStatusBar pointer
       @param labelfield field to use in statusbar for URLs/userdata labels, or -1 to disable
       @param cursorfield field to use for cursor position, or -1 to disable
   */
   void SetStatusBar(class wxStatusBar *bar,
                       int labelfield = -1,
                       int cursorfield = -1)
      {
         m_StatusBar = bar; m_StatusFieldLabel = labelfield;
         m_StatusFieldCursor = cursorfield;
      }
#ifndef __WXMSW__
   /// Enable or disable focus follow mode under non-MSW
   void SetFocusFollowMode(bool enable = TRUE)
      { m_FocusFollowMode = enable; }
#endif

   /**@name Modified flag handling, will not get reset by list unless
      in Clear() */
   //@{
   /// Set dirty flag.
   void SetModified(bool modified = TRUE) { m_Modified = modified; }
   /// Query whether window needs redrawing.
   bool IsModified(void) const { return m_Modified; }
   //@}

   /**@name Dirty flag handling for optimisations.
    Normally one should only need to call SetDirty(), e.g. when
    manipulating the wxLayoutList directly, so the window will update
    itself. ResetDirty() and IsDirty() should only be used
    internally. */
   //@{
   /// Set dirty flag.
   void SetDirty(void) { m_Dirty = true; m_Modified = true; }
   /// Query whether window needs redrawing.
   bool IsDirty(void) const { return m_Dirty; }
   /// Reset dirty flag.
   void ResetDirty(void) { m_Dirty = false; }
   //@}

   
protected:
   /// generic function for mouse events processing
   void OnMouse(int eventId, wxMouseEvent& event);
   /// as the name says
   void ScrollToCursor(void);
   /// for sending events
   wxWindow *m_Parent;
   /// Shall we send events?
   bool m_doSendEvents;
   /// Where does the current view start?
   int m_ViewStartX; int m_ViewStartY;
   /// Do we currently have the focus?
   bool m_HaveFocus;
   /// do we handle clicks of the right mouse button?
   bool m_DoPopupMenu;
   /// Should InternalPaint() scroll to cursor (VZ: seems unused any more)
   bool m_ScrollToCursor;
   /// Do we currently have a non-standard cursor?
   bool m_HandCursor;
   /// the menu
   wxMenu * m_PopupMenu;
   /// for derived classes, set when mouse is clicked
   wxPoint m_ClickPosition;
   /// for scrollbar calculations:
   int m_maxx;
   int m_maxy;
   int m_lineHeight;

   /// do we have the corresponding scrollbar?
   bool m_hasHScrollbar,
        m_hasVScrollbar;

   /** Visibility parameter for cursor. 0/1 as expected, -1: visible
       on demand.
   */
   int m_CursorVisibility;

   bool SetAutoDeleteSelection(bool enable = TRUE)
   {
      bool old = m_AutoDeleteSelection;
      m_AutoDeleteSelection = enable;
      return old;
   }
private:
   /// The layout list to be displayed.
   wxLayoutList *m_llist;
   /// Can user edit the window?
   bool m_Editable;
   /// Are we currently building a selection with the keyboard?
   bool m_Selecting;
   /// wrap margin
   CoordType    m_WrapMargin;
   /// Has list changed since last redraw, e.g. in size?
   bool m_Dirty;
   /// Has the list ever been modified?
   bool m_Modified;
   wxMemoryDC  *m_memDC;
   wxBitmap    *m_bitmap;
   wxPoint      m_bitmapSize;
   /// A frame's statusbar to update
   class wxStatusBar *m_StatusBar;
   /// statusbar field for labels
   int          m_StatusFieldLabel;
   /// statusbar field for cursor positions
   int          m_StatusFieldCursor;
   /// a pointer to a bitmap for the background
   wxBitmap    *m_BGbitmap;
   /**@name Some configuration options */
   //@{
   /// Do we want to auto-replace the selection with new text?
   bool         m_AutoDeleteSelection;
#ifndef __WXMSW__
   /// Do we want the focus to follow the mouse?
   bool m_FocusFollowMode;
#endif
   /// For finding text and finding it again:
   wxString m_FindString;
//@}
   DECLARE_EVENT_TABLE()
};

#endif
