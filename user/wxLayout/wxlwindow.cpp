/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998, 1999 by Karsten Ballüder (Ballueder@usa.net)           * 
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
#   include "wxlparser.h"
#endif

#include <ctype.h>
#include <wx/clipbrd.h>

#define WXLO_XOFFSET   4
#define WXLO_YOFFSET   4

BEGIN_EVENT_TABLE(wxLayoutWindow,wxScrolledWindow)
   EVT_PAINT    (wxLayoutWindow::OnPaint)
   EVT_CHAR     (wxLayoutWindow::OnChar)
   EVT_LEFT_DOWN(wxLayoutWindow::OnLeftMouseClick)
   EVT_RIGHT_DOWN(wxLayoutWindow::OnRightMouseClick)
   EVT_LEFT_DCLICK(wxLayoutWindow::OnMouseDblClick)
   EVT_MENU_RANGE(WXLOWIN_MENU_FIRST, WXLOWIN_MENU_LAST, wxLayoutWindow::OnMenu)
   EVT_SET_FOCUS(wxLayoutWindow::OnSetFocus)
   EVT_KILL_FOCUS(wxLayoutWindow::OnKillFocus)
END_EVENT_TABLE()

wxLayoutWindow::wxLayoutWindow(wxWindow *parent)
   : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                      wxHSCROLL | wxVSCROLL | wxBORDER)

{
   m_Editable = false;
   m_doSendEvents = false;
   m_ViewStartX = 0; m_ViewStartY = 0;
   m_DoPopupMenu = true;
   m_PopupMenu = MakeFormatMenu();
   m_memDC = new wxMemoryDC;
   m_bitmap = new wxBitmap(4,4);
   m_bitmapSize = wxPoint(4,4);
   m_llist = new wxLayoutList();
   SetWrapMargin(0);
   wxPoint max = m_llist->GetSize();
   SetScrollbars(10, 20 /*lineHeight*/, max.x/10+1, max.y/20+1);
   EnableScrolling(true,true);
   m_maxx = max.x; m_maxy = max.y;
   SetCursor(wxCURSOR_IBEAM);
   SetDirty();
}

wxLayoutWindow::~wxLayoutWindow()
{
   delete m_memDC; // deletes bitmap automatically (?)
   delete m_bitmap;
   delete m_llist;
   delete m_PopupMenu; 
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
wxLayoutWindow::OnMouse(int eventId, wxMouseEvent& event)
{
   wxPaintDC dc( this );
   PrepareDC( dc );     
   SetFocus();
   
   wxPoint findPos;
   findPos.x = dc.DeviceToLogicalX(event.GetX());
   findPos.y = dc.DeviceToLogicalY(event.GetY());

   findPos.x -= WXLO_XOFFSET;
   findPos.y -= WXLO_YOFFSET;

   if(findPos.x < 0) findPos.x = 0;
   if(findPos.y < 0) findPos.y = 0;
   
#ifdef WXLAYOUT_DEBUG
   wxLogDebug("wxLayoutWindow::OnMouse: (%d, %d) -> (%d, %d)",
              event.GetX(), event.GetY(), findPos.x, findPos.y);
#endif

   m_ClickPosition = findPos;
   wxPoint cursorPos;
   wxLayoutObject *obj = m_llist->FindObjectScreen(dc, findPos, &cursorPos);

#ifdef WXLAYOUT_DEBUG
   if(obj)
      wxLogDebug("wxLayoutWindow::OnMouse: Found object of type %d.",
                 obj->GetType());
   else
      wxLogDebug("wxLayoutWindow::OnMouse: Found no object.");
#endif

   // always move cursor to mouse click:
   if(obj && eventId == WXLOWIN_MENU_LCLICK)
   {
      m_llist->MoveCursorTo(cursorPos);
      DoPaint(false); 
   }
   if(!m_doSendEvents) // nothing to do
      return;

   // only do the menu if activated, editable and not on a clickable object
   if(eventId == WXLOWIN_MENU_RCLICK
      && IsEditable()
      && (! obj || (obj && obj->GetUserData() == NULL))
      )
   {
      PopupMenu(m_PopupMenu, event.GetX(), event.GetY());
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

/*
 * Some simple keyboard handling.
 */
void
wxLayoutWindow::OnChar(wxKeyEvent& event)
{
   if(!IsEditable()) // do nothing
   {
      event.Skip();
      return;
   }
   
   long keyCode = event.KeyCode();

   /* First, handle control keys */
   if(event.ControlDown() && ! event.AltDown())
   {
      switch(event.KeyCode())
      {
      case WXK_DELETE :
      case 'd':
         m_llist->Delete(1);
         break;
      case 'y':
         m_llist->DeleteLines(1);
         break;
      case 'h': // like backspace
         if(m_llist->MoveCursorHorizontally(-1)) m_llist->Delete(1);
         break;
      case 'u':
         m_llist->DeleteToBeginOfLine();
         break;
      case 'k':
         m_llist->DeleteToEndOfLine();
         break;
#ifdef WXLAYOUT_DEBUG
      case WXK_F1:
         m_llist->SetFont(-1,-1,-1,-1,true);  // underlined
         break;
#endif
      default:
         ;
      }
   }
   // ALT only:
   else if( event.AltDown() && ! event.ControlDown() )
   {
      switch(event.KeyCode())
      {
      case WXK_DELETE:
      case 'd':
         m_llist->DeleteWord();
         break;
      default:
         ;
      }
   }
   // no control keys:
   else if ( ! event.AltDown() && ! event.ControlDown())
   {
      switch(event.KeyCode())
      {
      case WXK_RIGHT:
         m_llist->MoveCursorHorizontally(1);
         break;
      case WXK_LEFT:
         m_llist->MoveCursorHorizontally(-1);
         break;
      case WXK_UP:
         m_llist->MoveCursorVertically(-1);
         break;
      case WXK_DOWN:
         m_llist->MoveCursorVertically(1);
         break;
      case WXK_PRIOR:
         m_llist->MoveCursorVertically(-20);
         break;
      case WXK_NEXT:
         m_llist->MoveCursorVertically(20);
         break;
      case WXK_HOME:
         m_llist->MoveCursorToBeginOfLine();
         break;
      case WXK_END:
         m_llist->MoveCursorToEndOfLine();
         break;
      case WXK_DELETE :
         m_llist->Delete(1);
         break;
      case WXK_BACK: // backspace
         if(m_llist->MoveCursorHorizontally(-1)) m_llist->Delete(1);
         break;
      case WXK_RETURN:
         if(m_WrapMargin > 0)
            m_llist->WrapLine(m_WrapMargin);
         m_llist->LineBreak();
         break;
      default:
         if((!(event.ControlDown() || event.AltDown() || event.MetaDown()))
            && (keyCode < 256 && keyCode >= 32)
            )
         {
            wxString tmp;
            tmp += keyCode;
            if(m_WrapMargin > 0 && isspace(keyCode))
                m_llist->WrapLine(m_WrapMargin);
            m_llist->Insert(tmp);
         }
         break;
      }
   }
   SetDirty();
   DoPaint(true); // paint and scroll to cursor
}

void
wxLayoutWindow::OnPaint( wxPaintEvent &WXUNUSED(event))  // or: OnDraw(wxDC& dc)
{
   m_ScrollToCursor = false;
   InternalPaint();
}

void
wxLayoutWindow::DoPaint(bool scrollToCursor)
{
   m_ScrollToCursor = scrollToCursor;
#ifdef __WXGTK__
   InternalPaint();
#else
   Refresh();
#endif
}

void
wxLayoutWindow::InternalPaint(void)
{
   wxPaintDC dc( this );
   PrepareDC( dc );

   int x0,y0,x1,y1, dx, dy;

   // Calculate where the top of the visible area is:
   ViewStart(&x0,&y0);
   GetScrollPixelsPerUnit(&dx, &dy);
   x0 *= dx; y0 *= dy;

   // Get the size of the visible window:
   GetClientSize(&x1,&y1);
   wxASSERT(x1 > 0);

   wxASSERT(y1 > 0);

   // Maybe we need to change the scrollbar sizes or positions,
   // so layout the list and check:
   if(IsDirty())
      m_llist->Layout(dc);
   // this is needed even when only the cursor moved
   m_llist->Layout(dc,y0+y1);
   
   if(IsDirty())
      ResizeScrollbars();
   
   /* Make sure that the scrollbars are at a position so that the
      cursor is visible if we are editing. */
      /** Scroll so that cursor is visible! */
   if(IsEditable() && m_ScrollToCursor)
   {
      wxPoint cc = m_llist->GetCursorScreenPos();
      if(cc.x < x0 || cc.y < y0
         || cc.x >= x0+(9*x1)/10 || cc.y >= y0+(9*y1/10))  // (9*x)/10 ==  90%
      {
         int nx, ny;
         nx = cc.x - x1/2; if(nx < 0) nx = 0;
         ny = cc.y - y1/2; if(ny < 0) ny = 0;
         Scroll(nx/dx,ny/dy); // new view start
         x0 = nx; y0 = ny;
      }
   }
   
   /* Check whether the window has grown, if so, we need to reallocate 
      the bitmap to be larger. */
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
   // Device origins on the memDC are suspect, we translate manually
   // with the translate parameter of Draw().
   m_memDC->SetDeviceOrigin(0,0);
   m_memDC->Clear();

   // The offsets give the window a tiny border on the left and top, looks nice.
   wxPoint offset(-x0+WXLO_XOFFSET,-y0+WXLO_YOFFSET);
   m_llist->Draw(*m_memDC,offset);
   if(IsEditable())
      m_llist->DrawCursor(*m_memDC,m_HaveFocus,offset);
   // Now copy everything to the screen:
   dc.Blit(x0,y0,x1,y1,m_memDC,0,0,wxCOPY,FALSE);

   
   ResetDirty();
}

// change the range and position of scroll bars
void
wxLayoutWindow::ResizeScrollbars(bool exact)
{
   wxPoint max = m_llist->GetSize();

   if(max.x > m_maxx || max.y > m_maxy
      || max.x < (7*m_maxx)/10 || max.y << (7*m_maxy)/10
      || exact)
   {
      if(! exact)  // add an extra 20% to the sizes to avoid future updates
      {
         max.x = (12*max.x)/10;  // 12/20 = 120%
         max.y = (12*max.y)/10;
      }
      ViewStart(&m_ViewStartX, &m_ViewStartY);
      SetScrollbars(10, 20, max.x/10+1,max.y/20+1,m_ViewStartX,m_ViewStartY,true);
      m_maxx = max.x; m_maxy = max.y;
   }
}

void
wxLayoutWindow::Paste(void)
{
   // Read some text
   if (wxTheClipboard->Open())
   {
      wxTextDataObject data;
      if (wxTheClipboard->IsSupported(wxDF_TEXT))
      {
         wxTheClipboard->GetData(&data);
         wxLayoutImportText( m_llist, data.GetText());
      }  
      wxTheClipboard->Close();
   }
}


wxMenu *
wxLayoutWindow::MakeFormatMenu()
{
   wxMenu *m = new wxMenu(_("Layout Menu"));

   m->Append(WXLOWIN_MENU_LARGER   ,_("&Larger"),_("Switch to larger font."), false);
   m->Append(WXLOWIN_MENU_SMALLER  ,_("&Smaller"),_("Switch to smaller font."), false);
   m->AppendSeparator();
   m->Append(WXLOWIN_MENU_UNDERLINE_ON, _("&Underline on"),_("Activate underline mode."), false);
   m->Append(WXLOWIN_MENU_UNDERLINE_OFF,_("&Underline off"),_("Deactivate underline mode."), false);
   m->Append(WXLOWIN_MENU_BOLD_ON      ,_("&Bold on"),_("Activate bold mode."), false);
   m->Append(WXLOWIN_MENU_BOLD_OFF     ,_("&Bold off"),_("Deactivate bold mode."), false);
   m->Append(WXLOWIN_MENU_ITALICS_ON   ,_("&Italics on"),_("Activate italics mode."), false);
   m->Append(WXLOWIN_MENU_ITALICS_OFF  ,_("&Italics off"),_("Deactivate italics mode."), false);
   m->AppendSeparator();
   m->Append(WXLOWIN_MENU_ROMAN     ,_("&Roman"),_("Switch to roman font."), false);
   m->Append(WXLOWIN_MENU_TYPEWRITER,_("&Typewriter"),_("Switch to typewriter font."), false);
   m->Append(WXLOWIN_MENU_SANSSERIF ,_("&Sans Serif"),_("Switch to sans serif font."), false);
   return m;
}

void wxLayoutWindow::OnMenu(wxCommandEvent& event)
{
   switch (event.GetId())
   {
   case WXLOWIN_MENU_LARGER:
      m_llist->SetFontLarger(); break;
   case WXLOWIN_MENU_SMALLER:
      m_llist->SetFontSmaller(); break;
   case WXLOWIN_MENU_UNDERLINE_ON:
      m_llist->SetFontUnderline(true); break;
   case WXLOWIN_MENU_UNDERLINE_OFF:
      m_llist->SetFontUnderline(false); break;
   case WXLOWIN_MENU_BOLD_ON:
      m_llist->SetFontWeight(wxBOLD); break;
   case WXLOWIN_MENU_BOLD_OFF:
      m_llist->SetFontWeight(wxNORMAL); break;
   case WXLOWIN_MENU_ITALICS_ON:
      m_llist->SetFontStyle(wxITALIC); break;
   case WXLOWIN_MENU_ITALICS_OFF:
      m_llist->SetFontStyle(wxNORMAL); break;
   case WXLOWIN_MENU_ROMAN:
      m_llist->SetFontFamily(wxROMAN); break;
   case WXLOWIN_MENU_TYPEWRITER:
      m_llist->SetFontFamily(wxFIXED); break;
   case WXLOWIN_MENU_SANSSERIF:
      m_llist->SetFontFamily(wxSWISS); break;
   }
}

void
wxLayoutWindow::OnSetFocus(wxFocusEvent &ev)
{
   m_HaveFocus = true;
   DoPaint(); // to repaint the cursor
}

void
wxLayoutWindow::OnKillFocus(wxFocusEvent &ev)
{
   m_HaveFocus = false;
   DoPaint(); // to repaint the cursor
}
