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

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#  pragma hdrstop
#endif


#include "Mpch.h"
#ifdef M_BASEDIR
#   ifndef USE_PCH
#     include "Mcommon.h"
#     include "gui/wxMenuDefs.h"
#     include "gui/wxMApp.h"
#   endif // USE_PCH
#   include "gui/wxlwindow.h"
#   include "gui/wxlparser.h"
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

#include <wx/clipbrd.h>
#include <wx/textctrl.h>
#include <wx/dataobj.h>

#include <ctype.h>

#ifdef WXLAYOUT_DEBUG
#  define   WXLO_DEBUG(x)      wxLogDebug x
#else
#  define WXLO_DEBUG(x)
#endif

/// offsets to put a nice frame around text
#define WXLO_XOFFSET   4
#define WXLO_YOFFSET   4

/// offset to the right and bottom for when to redraw scrollbars
#define   WXLO_ROFFSET   20
#define   WXLO_BOFFSET   20

BEGIN_EVENT_TABLE(wxLayoutWindow,wxScrolledWindow)
   EVT_PAINT    (wxLayoutWindow::OnPaint)
   EVT_CHAR     (wxLayoutWindow::OnChar)
   EVT_KEY_UP   (wxLayoutWindow::OnKeyUp)
   EVT_LEFT_DOWN(wxLayoutWindow::OnLeftMouseClick)
   EVT_RIGHT_DOWN(wxLayoutWindow::OnRightMouseClick)
   EVT_LEFT_DCLICK(wxLayoutWindow::OnMouseDblClick)
   EVT_MOTION    (wxLayoutWindow::OnMouseMove)
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
   m_BGbitmap = NULL;
   m_ScrollToCursor = false;
   SetWrapMargin(0);
   wxPoint max = m_llist->GetSize();
   SetScrollbars(10, 20 /*lineHeight*/, max.x/10+1, max.y/20+1);
   EnableScrolling(true,true);
   m_maxx = max.x; m_maxy = max.y;
   m_Selecting = false;
   SetCursor(wxCURSOR_IBEAM);
   SetDirty();
}

wxLayoutWindow::~wxLayoutWindow()
{
   delete m_memDC; // deletes bitmap automatically (?)
   delete m_bitmap;
   delete m_llist;
   delete m_PopupMenu;
   SetBackgroundBitmap(NULL);
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

   m_ClickPosition = wxPoint(event.GetX(), event.GetY());

   wxPoint cursorPos;
   bool found;
   wxLayoutObject *obj = m_llist->FindObjectScreen(dc, findPos,
                                                   &cursorPos, &found);

   //has the mouse only been moved?
   if(eventId == WXLOWIN_MENU_MOUSEMOVE)
   {
      // found is only true if we are really over an object, not just
      // behind it
      if(found && obj && obj->GetUserData() != NULL)
      {
         if(!m_HandCursor)
            SetCursor(wxCURSOR_HAND);
         m_HandCursor = TRUE;
      }
      else
      {
         if(m_HandCursor)
            SetCursor(wxCURSOR_IBEAM);
         m_HandCursor = FALSE;
      }
      return;
   }

   // always move cursor to mouse click:
   if(obj && eventId == WXLOWIN_MENU_LCLICK)
   {
      m_llist->MoveCursorTo(cursorPos);
      m_ScrollToCursor = true;
      Refresh();
   }
   if(!m_doSendEvents) // nothing to do
      return;

   // only do the menu if activated, editable and not on a clickable object
   if(eventId == WXLOWIN_MENU_RCLICK
      && IsEditable()
      && (! obj || (obj && obj->GetUserData() == NULL))
      )
   {
      PopupMenu(m_PopupMenu, m_ClickPosition.x, m_ClickPosition.y);
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
#ifdef WXLAYOUT_DEBUG
   if(event.KeyCode() == WXK_F1)
   {
      m_llist->Debug();
      return;
   }
#endif
   
   long keyCode = event.KeyCode();
   if(m_Selecting && ! event.ShiftDown())
   {
      m_llist->EndSelection();
      m_Selecting = false;
   }
   else
      if(! m_Selecting && event.ShiftDown())
      {
         switch(keyCode)
         {
         case WXK_UP:
         case WXK_DOWN:
         case WXK_RIGHT:
         case WXK_LEFT:
         case WXK_PRIOR:
         case WXK_NEXT:
         case WXK_HOME:
         case WXK_END:
            m_Selecting = true;
            m_llist->StartSelection();
            break;
         default:
            ;
         }
      }
   
   if(!IsEditable()) // do nothing
   {
      switch(keyCode)
      {
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
      default:
         ;
      }
      return;
   }
   
   /* First, handle control keys */
   if(event.ControlDown() && ! event.AltDown())
   {
      switch(keyCode)
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
      case 'v':
         Paste();
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
      switch(keyCode)
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
      switch(keyCode)
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
      case WXK_INSERT:
         if(event.ShiftDown())
            Paste();
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
   SetModified();
   m_ScrollToCursor = true;
   //DoPaint(true); // paint and scroll to cursor
   wxRect r = *m_llist->GetUpdateRect();
   Refresh( FALSE, &r);
}

void
wxLayoutWindow::OnKeyUp(wxKeyEvent& event)
{
   if(event.KeyCode() == WXK_SHIFT && m_llist->IsSelecting())
      m_llist->EndSelection();
   event.Skip();
}


void
wxLayoutWindow::ScrollToCursor(void)
{
   wxClientDC dc( this );
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
   // As we have the values anyway, use them to avoid unnecessary
   // scrollbar updates.
   if(x1 > m_maxx) m_maxx = x1;  
   if(y1 > m_maxy) m_maxy = y1;
   /* Make sure that the scrollbars are at a position so that the
      cursor is visible if we are editing. */
      /** Scroll so that cursor is visible! */
   WXLO_DEBUG(("m_ScrollToCursor = %d", (int) m_ScrollToCursor));
   if(IsEditable() && m_ScrollToCursor)
   {
      wxPoint cc = m_llist->GetCursorScreenPos(*m_memDC);
      if(cc.x < x0 || cc.y < y0
         || cc.x >= x0+(9*x1)/10 || cc.y >= y0+(9*y1/10))  // (9*x)/10 ==  90%
      {
         int nx, ny;
         nx = cc.x - x1/2; if(nx < 0) nx = 0;
         ny = cc.y - y1/2; if(ny < 0) ny = 0;
         Scroll(nx/dx,ny/dy); // new view start
         x0 = nx; y0 = ny;
         m_ScrollToCursor = false; // avoid recursion
         Refresh(FALSE);  /// Re-entering this function!
      }
   }
}

void
wxLayoutWindow::OnPaint( wxPaintEvent &WXUNUSED(event))  // or: OnDraw(wxDC& dc)
{
   wxRect region = GetUpdateRegion().GetBox();
   InternalPaint(& region);
}

void
wxLayoutWindow::DoPaint(const wxRect *updateRect)
{
#ifdef __WXGTK__
   InternalPaint(updateRect);
#else
   Refresh(FALSE, updateRect); // Causes bad flicker under wxGTK!!!
#endif
}

void
wxLayoutWindow::InternalPaint(const wxRect *updateRect)
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
   // As we have the values anyway, use them to avoid unnecessary
   // scrollbar updates.
   if(x1 > m_maxx) m_maxx = x1;  
   if(y1 > m_maxy) m_maxy = y1;


   //m_llist->InvalidateUpdateRect();
   //const wxRect *r = m_llist->GetUpdateRect();
   WXLO_DEBUG(("Update rect: %ld,%ld / %ld,%ld",
               updateRect->x, updateRect->y,
               updateRect->x+updateRect->width,
               updateRect->y+updateRect->height));

#if 0
   //FIXME: we should never need to call Layout at all because the
   //       list does it automatically.
// Maybe we need to change the scrollbar sizes or positions,
   // so layout the list and check:
   if(IsDirty())
      m_llist->Layout(dc);
   wxLogDebug("Update rect after calling Layout: %ld,%ld / %ld,%ld",
              r->x, r->y, r->x+r->width, r->y+r->height);
   // this is needed even when only the cursor moved
   m_llist->Layout(dc,y0+y1);
   wxLogDebug("Update rect after calling Layout again: %ld,%ld / %ld,%ld",
              r->x, r->y, r->x+r->width, r->y+r->height);
#endif
   
   if(IsDirty())
      ResizeScrollbars();
   
   
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
   m_memDC->SetBrush(wxBrush(m_llist->GetDefaults()->GetBGColour(),wxSOLID));
   m_memDC->SetPen(wxPen(m_llist->GetDefaults()->GetBGColour(),
                         0,wxTRANSPARENT));                               
   m_memDC->SetLogicalFunction(wxCOPY);

   /* Either fill the background with the background bitmap, or clear
      it. */
   if(m_BGbitmap)
   {
      CoordType
         y, x,
         w = m_BGbitmap->GetWidth(),
         h = m_BGbitmap->GetHeight();
      for(y = 0; y < y1; y+=h)
         for(x = 0; x < x1; x+=w)
            m_memDC->DrawBitmap(*m_BGbitmap, x, y);
      m_memDC->SetBackgroundMode(wxTRANSPARENT);
   }
   else
   {
      // clear the background: (must not be done if we use the update rectangle!)
      m_memDC->SetBackgroundMode(wxSOLID);
      m_memDC->DrawRectangle(0,0,x1, y1);
   }

   
   /* This is the important bit: we tell the list to draw itself: */
   WXLO_DEBUG(("Update rect: %ld,%ld / %ld,%ld",
               updateRect->x, updateRect->y,
               updateRect->x+updateRect->width,
               updateRect->y+updateRect->height)); 
   
   wxPoint offset(-x0+WXLO_XOFFSET,-y0+WXLO_YOFFSET);
   m_llist->Draw(*m_memDC,offset, y0, y0+y1);
   // We start calculating a new update rect before drawing the
   // cursor, so that the cursor coordinates get included in the next
   // update rectangle:
   m_llist->InvalidateUpdateRect(); 
   if(IsEditable()) //we draw the cursor
      m_llist->DrawCursor(*m_memDC,m_HaveFocus,offset);

// Now copy everything to the screen:
#if 0
   // This somehow doesn't work, but even the following bit with the
   // whole rect at once is still a bit broken I think.
   wxRegionIterator ri ( GetUpdateRegion() );
   if(ri)
      while(ri)
      {
         WXLO_DEBUG(("UpdateRegion: %ld,%ld, %ld,%ld",
                     ri.GetX(),ri.GetY(),ri.GetW(),ri.GetH()));
         dc.Blit(x0+ri.GetX(),y0+ri.GetY(),ri.GetW(),ri.GetH(),
                 m_memDC,ri.GetX(),ri.GetY(),wxCOPY,FALSE);
         ri++;
      }
   else
#endif
   {
      // FIXME: Trying to copy only the changed parts, but it does not seem
      // to work:
//      x0 = updateRect->x; y0 = updateRect->y;
//      if(updateRect->height < y1)
//         y1 = updateRect->height;
//      y1 += WXLO_YOFFSET; //FIXME might not be needed
      dc.Blit(x0,y0,x1,y1,m_memDC,0,0,wxCOPY,FALSE);
   }

   ResetDirty();
   m_ScrollToCursor = false;
}

// change the range and position of scrollbars
void
wxLayoutWindow::ResizeScrollbars(bool exact)
{
   wxPoint max = m_llist->GetSize();
   
   if(max.x > m_maxx || max.y > m_maxy
      || max.x > m_maxx-WXLO_ROFFSET || max.y > m_maxy-WXLO_BOFFSET
      || exact)
   {
      if(! exact) 
      {
         // add an extra bit to the sizes to avoid future updates
         max.x = max.x+WXLO_ROFFSET;  
         max.y = max.y+WXLO_BOFFSET;
      }
      ViewStart(&m_ViewStartX, &m_ViewStartY);
      SetScrollbars(10, 20, max.x/10+1,max.y/20+1,m_ViewStartX,m_ViewStartY,true);
      m_maxx = max.x; m_maxy = max.y;
   }
}

void
wxLayoutWindow::Paste(void)
{
   wxString text;
   // Read some text
   if (wxTheClipboard->Open())
   {
      wxTextDataObject data;
      if (wxTheClipboard->IsSupported( data.GetFormat() ))
      {
         wxTheClipboard->GetData(&data);
         text += data.GetText();
      }  
      wxTheClipboard->Close();
   }
#if 0
   /* My attempt to get the primary selection, but it does not
      work. :-( */
   if(text.Length() == 0)
   {
      wxTextCtrl tmp_tctrl(this,-1);
      tmp_tctrl.Paste();
      text += tmp_tctrl.GetValue();
   }
#endif
   wxLayoutImportText( m_llist, text);
}

bool
wxLayoutWindow::Copy(void)
{
   wxLayoutList *llist = m_llist->GetSelection();
   if(! llist)
      return FALSE;

   wxString text;
   wxLayoutExportObject *export;
   wxLayoutExportStatus status(llist);
   while((export = wxLayoutExport( &status, WXLO_EXPORT_AS_TEXT)) != NULL)
   {
      if(export->type == WXLO_EXPORT_TEXT)
         text << *(export->content.text);
      delete export;
   }
   delete llist;
   
   // Read some text
   if (wxTheClipboard->Open())
   {
      wxTextDataObject *data = new wxTextDataObject( text );
      bool  rc = wxTheClipboard->SetData( data );
      wxTheClipboard->Close();
      return rc;
   }
   return FALSE;
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
//FIXME   DoPaint(); // to repaint the cursor
}

void
wxLayoutWindow::OnKillFocus(wxFocusEvent &ev)
{
   m_HaveFocus = false;
//FIXME   DoPaint(); // to repaint the cursor
}
