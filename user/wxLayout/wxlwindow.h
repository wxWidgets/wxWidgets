/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifndef WXLWINDOW_H
#define WXLWINDOW_H

#ifdef __GNUG__
#   pragma interface "wxlwindow.h"
#endif

#include   <wx/wx.h>

#include   "wxllist.h"

class wxLayoutWindow : public wxScrolledWindow
{
public:
   wxLayoutWindow(wxWindow *parent);

   wxLayoutList & GetLayoutList(void) { return m_llist; }

   //virtual void OnDraw(wxDC &dc);
   void OnPaint(wxPaintEvent &WXUNUSED(event));
   virtual void OnMouse(wxMouseEvent& event);
   virtual void OnChar(wxKeyEvent& event);
   void UpdateScrollbars(void);
   void Print(void);
   void Erase(void)
      { m_llist.Clear(); Clear(); }
   void SetEventId(int id) { m_EventId = id; }
private:
   int m_EventId;
   /// the layout list to be displayed
   wxLayoutList m_llist;
   /// have we already set the scrollbars?
   bool m_ScrollbarsSet;
   /// if we want to find an object:
   wxPoint m_FindPos;
   wxLayoutObjectBase *m_FoundObject;

   DECLARE_EVENT_TABLE()
};

#endif
