/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998, 1999 by Karsten Ballüder (Ballueder@usa.net)        * 
 *                                                                  *
 * $Id$
 *******************************************************************/

#ifdef __GNUG__
#   pragma implementation "wxlwindow.h"
#endif

//#include "Mpch.h"

#ifdef M_PREFIX
#   ifndef USE_PCH
#     include "Mcommon.h"
#     include "gui/wxMenuDefs.h"
#     include "gui/wxMApp.h"
#   endif // USE_PCH
#   include "gui/wxlwindow.h"
#else
#   ifdef   __WXMSW__
#       include <windows.h>

#       undef FindWindow
#       undef GetCharWidth
#       undef StartDoc
#   endif

#   include "wxlwindow.h"
#   define TRACEMESSAGE(x)
#endif
#  define   WXL_VAR(x)           { wxString s; s << #x " = " << x; wxLogDebug(s); }

BEGIN_EVENT_TABLE(wxLayoutWindow,wxScrolledWindow)
   EVT_PAINT    (wxLayoutWindow::OnPaint)
   EVT_CHAR     (wxLayoutWindow::OnChar)

   EVT_LEFT_DOWN(wxLayoutWindow::OnLeftMouseClick)
   EVT_RIGHT_DOWN(wxLayoutWindow::OnRightMouseClick)
   EVT_LEFT_DCLICK(wxLayoutWindow::OnMouseDblClick)
   EVT_MENU(WXLOWIN_MENU_LARGER, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_SMALLER, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_UNDERLINE, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_BOLD, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_ITALICS, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_ROMAN, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_TYPEWRITER, wxLayoutWindow::OnMenu)
   EVT_MENU(WXLOWIN_MENU_SANSSERIF, wxLayoutWindow::OnMenu)
   EVT_SET_FOCUS(wxLayoutWindow::OnSetFocus)
   EVT_KILL_FOCUS(wxLayoutWindow::OnKillFocus)
END_EVENT_TABLE()

wxLayoutWindow::wxLayoutWindow(wxWindow *parent)
   : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                      wxHSCROLL | wxVSCROLL | wxBORDER)

{
   m_doSendEvents = false;
   m_ViewStartX = 0; m_ViewStartY = 0;
   m_DoPopupMenu = true;
   m_PopupMenu = NULL;
   m_memDC = new wxMemoryDC;
   m_bitmap = new wxBitmap(4,4);
   m_bitmapSize = wxPoint(4,4);
   
   CoordType
      max_x, max_y, lineHeight;
   m_llist.GetSize(&max_x, &max_y, &lineHeight);
   SetScrollbars(10, lineHeight, max_x/10+1, max_y/lineHeight+1);
   EnableScrolling(true,true);
   m_maxx = max_x; m_maxy = max_y; m_lineHeight = lineHeight;
}

wxLayoutWindow::~wxLayoutWindow()
{
   delete m_memDC; // deletes bitmap automatically (?)
   delete m_bitmap;
   if(m_PopupMenu) delete m_PopupMenu; 
}

#ifdef __WXMSW__
long
wxLayoutWindow::MSWGetDlgCode()
{
   // if we don't return this, we won't get OnChar() events for TABs and ENTER
   return DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTMESSAGE;
}
#endif //MSW

void
wxLayoutWindow::Update(void)
{
   if(IsDirty())
   {
      UpdateScrollbars();
      DoPaint();
   }
   else
      DoPaint(true); // only the cursor
   return;
}

void
wxLayoutWindow::OnMouse(int eventId, wxMouseEvent& event)
{
   if(!m_doSendEvents) // nothing to do
      return;

   wxPaintDC dc( this );
   PrepareDC( dc );     
   SetFocus();
   
   wxPoint findPos;
   findPos.x = dc.DeviceToLogicalX(event.GetX());
   findPos.y = dc.DeviceToLogicalY(event.GetY());

   TRACEMESSAGE(("wxLayoutWindow::OnMouse: (%d, %d) -> (%d, %d)",
                 event.GetX(), event.GetY(), findPos.x, findPos.y));

   m_ClickPosition = findPos;
   wxLayoutObjectBase *obj = m_llist.Find(findPos);

   // only do the menu if activated, editable and not on a clickable object
   if(eventId == WXLOWIN_MENU_RCLICK
      && m_DoPopupMenu
      && m_llist.IsEditable()
      && obj && obj->GetUserData() == NULL)
   {
      // when does this menu get freed?
      // how do we handle toggling? FIXME
      PopupMenu(MakeFormatMenu(), event.GetX(), event.GetY());
      return;
   }
   // find the object at this position
   if(obj)
   {
      wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, eventId);
      commandEvent.SetEventObject( this );
      commandEvent.SetClientData((char *)obj);
      GetEventHandler()->ProcessEvent(commandEvent);
   }
}

void
wxLayoutWindow::DeleteToEndOfLine(void)
{
   int help = m_llist.GetLineLength(
      m_llist.GetCurrentObject())
      - m_llist.GetCursor().x;
   m_llist.Delete(help>1 ? help-1 : 1);
}

void
wxLayoutWindow::GotoEndOfLine(void)
{
   wxPoint p = m_llist.GetCursor();
   p.x = m_llist.GetLineLength(m_llist.GetCurrentObject());
   if(p.x > 0) p.x --; // do not count the linebreak
   m_llist.SetCursor(p);
}

void
wxLayoutWindow::GotoBeginOfLine(void)
{
   wxPoint p = m_llist.GetCursor();
   p.x = 0;
   m_llist.SetCursor(p);
}

void
wxLayoutWindow::DeleteLine(void)
{
   GotoBeginOfLine();
   DeleteToEndOfLine();
   m_llist.Delete(1); // newline
}

void
wxLayoutWindow::DeleteToBeginOfLine(void)
{
   wxPoint p = m_llist.GetCursor();
   int count = p.x;
   if(count > 0)
   {
      p.x = 0;
      m_llist.SetCursor(p);
      m_llist.Delete(count);
   }
}


void
wxLayoutWindow::ScrollToCursor(void)
{
   /** Scroll so that cursor is visible! */
   int x0,y0,x1,y1,ux,uy;
   ViewStart(&x0,&y0);
   GetScrollPixelsPerUnit(&ux,&uy);
   x0*=ux; y0*=uy;
   GetClientSize(&x1,&y1);
   
   wxPoint cc = m_llist.GetCursorCoords();
   
   if(cc.x < x0 || cc.y < y0
      || cc.x >= x0+(9*x1)/10 || cc.y >= y0+(9*y1/10))  // (9*x)/10 ==  90%
   {
      int nx, ny;
      nx = cc.x - (8*x1)/10; if(nx < 0) nx = 0;
      ny = cc.y - (8*y1)/10; if(ny < 0) ny = 0;
      Scroll(nx/ux,ny/uy);
   }
}

/*
 * some simple keyboard handling
 */
void
wxLayoutWindow::OnChar(wxKeyEvent& event)
{
   if(! m_llist.IsEditable()) // do nothing
   {
      event.Skip();
      return;
   }
   
   long keyCode = event.KeyCode();

   /* First, handle control keys */
   if(event.ControlDown())
   {
      switch(event.KeyCode())
      {
      case WXK_DELETE :
      case 'k':
         DeleteToEndOfLine(); break;
      case 'd':
         m_llist.Delete(1); break;
      case 'y':
         DeleteLine(); break;
      case 'h': // like backspace
         if(m_llist.MoveCursor(-1))
            m_llist.Delete(1);
         break;
      case 'u':
         DeleteToBeginOfLine(); break;
      default:
         ;
      }
   }
   else // no control keys
   {
      switch(event.KeyCode())
      {
      case WXK_RIGHT:
         m_llist.MoveCursor(1);
         break;
      case WXK_LEFT:
         m_llist.MoveCursor(-1);
         break;
      case WXK_UP:
         m_llist.MoveCursor(0,-1);
         break;
      case WXK_DOWN:
         m_llist.MoveCursor(0,1);
         break;
      case WXK_PRIOR:
         m_llist.MoveCursor(0,-20);
         break;
      case WXK_NEXT:
         m_llist.MoveCursor(0,20);
         break;
      case WXK_HOME:
         GotoBeginOfLine();
         break;
      case WXK_END:
         GotoEndOfLine();
         break;
      case WXK_DELETE :
         if(event.ControlDown()) // delete to end of line
            DeleteToEndOfLine();
         else 
            m_llist.Delete(1);
         break;
      case WXK_BACK: // backspace
         if(m_llist.MoveCursor(-1)) {
            m_llist.Delete(1);
         }
         break;
      case WXK_RETURN:
         m_llist.LineBreak();
         break;

#ifdef WXLAYOUT_DEBUG
      case WXK_F1:
         m_llist.Debug();
         break;
      case WXK_F2:
         m_llist.WrapLine();
         break;
#endif
      
      default:
         if((!(event.ControlDown() || event.AltDown() || event.MetaDown()))
            && (keyCode < 256 && keyCode >= 32)
            )
         {
            String tmp;
            tmp += keyCode;
            m_llist.Insert(tmp);
            m_llist.WrapLine();
         }
         break;
      }
   }

   ScrollToCursor();
   Update();
   ScrollToCursor();
}

void
wxLayoutWindow::OnPaint( wxPaintEvent &WXUNUSED(event))  // or: OnDraw(wxDC& dc)
{
   DoPaint();
}

void
wxLayoutWindow::DoPaint(bool cursorOnly)  // or: OnDraw(wxDC& dc)
{
   wxPaintDC dc( this );
   PrepareDC( dc );

   // wxGTK: wxMemoryDC broken? YES!!
   int x0,y0,x1,y1, dx, dy;
   ViewStart(&x0,&y0);
   GetClientSize(&x1,&y1);                 // this is the size of the visible window
   wxASSERT(x1 > 0);
   wxASSERT(y1 > 0);
   GetScrollPixelsPerUnit(&dx, &dy);
   x0 *= dx; y0 *= dy;
   //FIXME: trying an offset for small border:
   wxPoint offset(-x0+4,-y0+4);

   //Blit() doesn't work on scrolled window!
   // So we have to draw the cursor on the memdc.
   //if(! cursorOnly)
   {
      if(x1 > m_bitmapSize.x || y1 > m_bitmapSize.y)
      {
         wxASSERT(m_bitmapSize.x > 0);
         wxASSERT(m_bitmapSize.y > 0);

         m_memDC->SelectObject(wxNullBitmap);
         delete m_bitmap;
         m_bitmapSize = wxPoint(x1,y1);
         m_bitmap = new wxBitmap(x1,y1);
         m_memDC->SelectObject(*m_bitmap);
      }
      m_memDC->SetDeviceOrigin(0,0);
      m_memDC->Clear();
      if(IsDirty() || m_llist.CursorMoved())
         m_llist.Layout(dc);
      
      m_llist.EraseAndDraw(*m_memDC,
                           wxLayoutObjectList::iterator(NULL),offset);
      m_llist.DrawCursor(*m_memDC,false,offset);
      dc.Blit(x0,y0,x1,y1,m_memDC,0,0,wxCOPY,FALSE);
   }

   //FIXME obsolete? ResetDirty();
   UpdateScrollbars();
}

// change the range and position of scroll bars
void
wxLayoutWindow::UpdateScrollbars(bool exact)
{
   CoordType
      max_x, max_y, lineHeight;
   
   m_llist.GetSize(&max_x, &max_y, &lineHeight);

   if(max_x > m_maxx || max_y > m_maxy || exact)
   {
      if(! exact)  // add an extra 50% to the sizes to avoid future updates
      {
         max_x = (3*max_x)/2;
         max_y = (3*max_y)/2;
      }
      ViewStart(&m_ViewStartX, &m_ViewStartY);
      SetScrollbars(10, 20, max_x/10+1,max_y/20+1,m_ViewStartX,m_ViewStartY,true);
      m_maxx = max_x; m_maxy = max_y;
   }
}

void
wxLayoutWindow::Print(wxDC &dc)
{
   if (dc.Ok() && dc.StartDoc((char *)_("Printing message...")))
   {
      //dc.SetUserScale(1.0, 1.0);
      m_llist.Draw(dc);
      dc.EndDoc();
   }
}

wxMenu *
wxLayoutWindow::MakeFormatMenu()
{
   if(m_PopupMenu)
      return m_PopupMenu;
   
   wxMenu *m = new wxMenu();

   m->Append(WXLOWIN_MENU_LARGER   ,_("&Larger"),_("Switch to larger font."), false);
   m->Append(WXLOWIN_MENU_SMALLER  ,_("&Smaller"),_("Switch to smaller font."), false);
   m->AppendSeparator();
   m->Append(WXLOWIN_MENU_UNDERLINE,_("&Underline"),_("Toggle underline mode."), true);
   m->Append(WXLOWIN_MENU_BOLD     ,_("&Bold"),_("Toggle bold mode."), true);
   m->Append(WXLOWIN_MENU_ITALICS  ,_("&Italics"),_("Toggle italics mode."), true);
   m->AppendSeparator();
   m->Append(WXLOWIN_MENU_ROMAN     ,_("&Roman"),_("Toggle underline mode."), false);
   m->Append(WXLOWIN_MENU_TYPEWRITER,_("&Typewriter"),_("Toggle bold mode."), false);
   m->Append(WXLOWIN_MENU_SANSSERIF ,_("&Sans Serif"),_("Toggle italics mode."), false);

   return m_PopupMenu = m;
}

void wxLayoutWindow::OnMenu(wxCommandEvent& event)
{
   if(! m_llist.IsEditable())
      return;
   
   switch (event.GetId())
   {
   case WXLOWIN_MENU_LARGER:
      m_llist.SetFontLarger();
      break;
   case WXLOWIN_MENU_SMALLER:
      m_llist.SetFontSmaller();
      break;
   case WXLOWIN_MENU_UNDERLINE:
      m_llist.SetFontUnderline(
         m_PopupMenu->IsChecked(WXLOWIN_MENU_UNDERLINE) ? false : true
            );
         break;
   case WXLOWIN_MENU_BOLD:
      m_llist.SetFontWeight(
         m_PopupMenu->IsChecked(WXLOWIN_MENU_BOLD) ? wxNORMAL : wxBOLD
            );
   case WXLOWIN_MENU_ITALICS:
      m_llist.SetFontStyle(
         m_PopupMenu->IsChecked(WXLOWIN_MENU_ITALICS) ? wxNORMAL : wxITALIC
            );
      break;
   case WXLOWIN_MENU_ROMAN:
      m_llist.SetFontFamily(wxROMAN); break;
   case WXLOWIN_MENU_TYPEWRITER:
      m_llist.SetFontFamily(wxFIXED); break;
   case WXLOWIN_MENU_SANSSERIF:
      m_llist.SetFontFamily(wxSWISS); break;
   }
}

void
wxLayoutWindow::OnSetFocus(wxFocusEvent &ev)
{
   m_llist.SetBoldCursor(true);
   DoPaint(true);
}

void
wxLayoutWindow::OnKillFocus(wxFocusEvent &ev)
{
   m_llist.SetBoldCursor(false);
   Update();
}
