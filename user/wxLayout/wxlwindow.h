/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998,1999 by Karsten Ballüder (Ballueder@usa.net)            *
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

enum
{
   WXLOWIN_MENU_LARGER = WXLOWIN_MENU_FIRST,
   WXLOWIN_MENU_SMALLER,
   WXLOWIN_MENU_UNDERLINE_ON,
   WXLOWIN_MENU_UNDERLINE_OFF,
   WXLOWIN_MENU_BOLD_ON,
   WXLOWIN_MENU_BOLD_OFF,
   WXLOWIN_MENU_ITALICS_ON,
   WXLOWIN_MENU_ITALICS_OFF,
   WXLOWIN_MENU_ROMAN,
   WXLOWIN_MENU_TYPEWRITER,
   WXLOWIN_MENU_SANSSERIF,
   WXLOWIN_MENU_RCLICK,
   WXLOWIN_MENU_LCLICK,
   WXLOWIN_MENU_DBLCLICK,
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
              wxColour *bg=NULL)
      {
         GetLayoutList()->Clear(family,size,style,weight,underline,fg,bg);
         SetBackgroundColour(GetLayoutList()->GetDefaults()->GetBGColour());
         ResizeScrollbars(true);
         SetDirty();
         SetModified(false);
         wxRect r;
         int w,h;
         r.x = r.y = 0; GetSize(&w,&h);
         r.width = w;
         r.height = h;
         DoPaint(&r);
      }
   /** Sets a background image, only used on screen, not on printouts.
       @param bitmap a pointer to a wxBitmap or NULL to remove it
   */
   void SetBackgroundBitmap(wxBitmap *bitmap = NULL)
      {
         if(m_BGbitmap) delete m_BGbitmap;
         m_BGbitmap = bitmap;
      }
   /// Enable or disable editing, i.e. processing of keystrokes.
   void SetEditable(bool toggle) { m_Editable = toggle; }
   /// Query whether list can be edited by user.
   bool IsEditable(void) const { return m_Editable; }
   /// Pastes text from clipboard.
   void Paste(void);
   
   //@}
   
   void EnablePopup(bool enable = true) { m_DoPopupMenu = enable; }

   /** Sets the wrap margin.
       @param margin set this to 0 to disable it
   */
   void SetWrapMargin(CoordType margin) { m_WrapMargin = margin; }
   
   /** Redraws the window.
       Internally, this stores the parameter and calls a refresh on
       wxMSW, draws directly on wxGTK.
   */
   void DoPaint(const wxRect *updateRect);

#ifdef __WXMSW__
   virtual long MSWGetDlgCode();
#endif //MSW

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
   void OnPaint(wxPaintEvent &event);
   void OnChar(wxKeyEvent& event);
   void OnMenu(wxCommandEvent& event);
   void OnLeftMouseClick(wxMouseEvent& event)  { OnMouse(WXLOWIN_MENU_LCLICK, event); }
   void OnRightMouseClick(wxMouseEvent& event) { OnMouse(WXLOWIN_MENU_RCLICK, event); }
   void OnMouseDblClick(wxMouseEvent& event)   { OnMouse(WXLOWIN_MENU_DBLCLICK, event); }
   void OnMouseMove(wxMouseEvent &event)       { OnMouse(WXLOWIN_MENU_MOUSEMOVE, event) ; }
   void OnSetFocus(wxFocusEvent &ev);
   void OnKillFocus(wxFocusEvent &ev);
   //@}

   /// Creates a wxMenu for use as a format popup.
   static wxMenu * MakeFormatMenu(void);
   /**@name Dirty flag handling for optimisations. */
   //@{
   /// Set dirty flag.
   void SetDirty(void) { m_Dirty = true; }
   /// Query whether window needs redrawing.
   bool IsDirty(void) const { return m_Dirty; }
   /// Reset dirty flag.
   void ResetDirty(void) { m_Dirty = false; }
   //@}
   /// Redraws the window, used by DoPaint() or OnPaint().
   void InternalPaint(const wxRect *updateRect);

   /// Has list been modified/edited?
   bool IsModified(void) const { return m_Modified; }
   /// Mark list as modified or unchanged.
   void SetModified(bool modified = true) { m_Modified = modified; }

protected:   
   /// generic function for mouse events processing
   void OnMouse(int eventId, wxMouseEvent& event);

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
   /// Should InternalPaint() scroll to cursor.
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
private:
   /// The layout list to be displayed.
   wxLayoutList *m_llist;
   /// Can user edit the window?
   bool m_Editable;
   /// Are we currently building a selection with the keyboard?
   bool m_Selecting;
   /// wrap margin
   CoordType    m_WrapMargin;
   /// Is list dirty (for redraws, internal use)?
   bool         m_Dirty;
   /// Has list been edited?
   bool         m_Modified;
   wxMemoryDC  *m_memDC;
   wxBitmap    *m_bitmap;
   wxPoint      m_bitmapSize;
   /// a pointer to a bitmap for the background
   wxBitmap    *m_BGbitmap;
   DECLARE_EVENT_TABLE()
};

#endif
