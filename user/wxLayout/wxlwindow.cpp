/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/

#ifdef __GNUG__
#   pragma implementation "wxlwindow.h"
#endif

//#include "Mpch.h"

#ifdef M_BASEDIR
#   ifndef USE_PCH
#     include "Mcommon.h"
#     include "gui/wxMenuDefs.h"
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
#  define   WXL_VAR(x)           cerr << #x " = " << x ;

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
END_EVENT_TABLE()

wxLayoutWindow::wxLayoutWindow(wxWindow *parent)
   : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                      wxHSCROLL | wxVSCROLL | wxBORDER)

{
   m_ScrollbarsSet = false;
   m_doSendEvents = false;
   m_ViewStartX = 0; m_ViewStartY = 0;
   m_DoPopupMenu = true;
   m_PopupMenu = NULL;
   
   CoordType
      max_x, max_y, lineHeight;
   m_llist.GetSize(&max_x, &max_y, &lineHeight);
   SetScrollbars(10, lineHeight, max_x/10+1, max_y/lineHeight+1);
   EnableScrolling(true,true);
}

#ifdef __WXMSW__
long
wxLayoutWindow::MSWGetDlgCode()
{
   // if we don't return this, we won't get OnChar() events
   return DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTMESSAGE;
}
#endif //MSW

void
wxLayoutWindow::Update(void)
{
   wxClientDC  dc(this);
   PrepareDC(dc);
   if(IsDirty())
   {
      DoPaint(dc);
      UpdateScrollbars();
      ResetDirty();
   }
   m_llist.DrawCursor(dc);
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

   if(eventId == WXLOWIN_MENU_RCLICK && m_DoPopupMenu && m_llist.IsEditable())
   {
      // when does this menu get freed?
      // how do we handle toggling? FIXME
      PopupMenu(MakeFormatMenu(), event.GetX(), event.GetY());
      return;
   }
   // find the object at this position
   wxLayoutObjectBase *obj = m_llist.Find(findPos);
   if(obj)
   {
      wxCommandEvent commandEvent(wxEVENT_TYPE_MENU_COMMAND, eventId);
      commandEvent.SetEventObject( this );
      commandEvent.SetClientData((char *)obj);
      GetEventHandler()->ProcessEvent(commandEvent);
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
   wxPoint p;
   CoordType help;
   
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
      p = m_llist.GetCursor();
      p.x = 0;
      m_llist.SetCursor(p);
      break;
   case WXK_END:
      p = m_llist.GetCursor();
      p.x = m_llist.GetLineLength(m_llist.FindCurrentObject(NULL));
      m_llist.SetCursor(p);
      break;
   case WXK_DELETE :
      if(event.ControlDown()) // delete to end of line
      {
         help = m_llist.GetLineLength(
            m_llist.FindCurrentObject(NULL))
            - m_llist.GetCursor().x;
         m_llist.Delete(help ? help : 1);
      }
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
#endif
      
   default:
      if(keyCode < 256 && keyCode >= 32)
      {
         String tmp;
         tmp += keyCode;
         m_llist.Insert(tmp);
      }
      break;
   }

   /** Scroll so that cursor is visible! */
   int x0,y0,x1,y1,ux,uy;
   ViewStart(&x0,&y0);
   GetScrollPixelsPerUnit(&ux,&uy);
   x0*=ux; y0*=uy;
   GetClientSize(&x1,&y1);

   wxPoint cc = m_llist.GetCursorCoords();
   int nx = x0, ny = y0;
   // when within 10% of borders, scroll to center
   if(cc.y > y0+(9*y1)/10)
      ny = cc.y - y1/5;
   else if (cc.y < y0+y1/10)
   {
      ny = cc.y-y1/2;
      if(ny < 0) ny = 0;
   }
   if(cc.x > x0+(9*x1)/10)
      nx = cc.x - x1/5;
   else if (cc.x < x0+x1/10)
   {
      nx = cc.x-x1/2;
      if(nx < 0) nx = 0;
   }
   Scroll(nx,ny);
   
   Update();
}

void
wxLayoutWindow::OnPaint( wxPaintEvent &WXUNUSED(event))  // or: OnDraw(wxDC& dc)
{
   wxPaintDC dc( this );
   PrepareDC( dc );

   DoPaint(dc);

// wxGTK: wxMemoryDC broken?
#if 0
   int x0,y0,x1,y1;
   ViewStart(&x0,&y0);
   GetSize(&x1,&y1);
   WXL_VAR(x0); WXL_VAR(y0);
   WXL_VAR(x1); WXL_VAR(y1);
   
   wxMemoryDC(memdc);
   wxBitmap bm(x1,y1);
   memdc.SelectObject(bm);

   // make temporary copy and edit this
   memdc.SetDeviceOrigin(x0,y0);
   memdc.Blit(x0,y0,x1,y1,&dc,x0,y0,wxCOPY,FALSE);
   DoPaint(memdc);
   // blit it back
   dc.Blit(x0,y0,x1,y1,&memdc,x0,y0,wxCOPY,FALSE);
#endif
   
}

// does the actual painting
void
wxLayoutWindow::DoPaint(wxDC &dc)
{
   m_llist.EraseAndDraw(dc);
   m_llist.DrawCursor(dc);
   // FIXME: not strictly correct, this does only work for changes behind
   //   the cursor position, not complete redraws

   if(! m_ScrollbarsSet)
   {
      m_ScrollbarsSet = true; // avoid recursion
      UpdateScrollbars();
   }
}

void
wxLayoutWindow::UpdateScrollbars(void)
{
   CoordType
      max_x, max_y, lineHeight;

   ViewStart(&m_ViewStartX, &m_ViewStartY);
   m_llist.GetSize(&max_x, &max_y, &lineHeight);
   SetScrollbars(10, lineHeight, max_x/10+1, max_y/lineHeight+1,m_ViewStartX,m_ViewStartY,true);
   //EnableScrolling(true,true);
   //Scroll(m_ViewStartX, m_ViewStartY);
}

void
wxLayoutWindow::Print(void)
{
   wxPostScriptDC   dc("layout.ps",true,this);
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
