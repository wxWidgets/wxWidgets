/*-*- c++ -*-********************************************************
 * wxLwindow.h : a scrolled Window for displaying/entering rich text*
 *                                                                  *
 * (C) 1998, 1999 by Karsten Ballüder (Ballueder@usa.net)           *
 *                                                                  *
 * $Id$
 *******************************************************************/

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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
#       include <wx/msw/private.h>
#   endif

#   include "wxlwindow.h"
#   include "wxlparser.h"
#endif

#include <wx/clipbrd.h>
#include <wx/textctrl.h>
#include <wx/dataobj.h>

#ifdef WXLAYOUT_USE_CARET
#   include <wx/caret.h>
#endif // WXLAYOUT_USE_CARET

#include <ctype.h>

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#ifdef WXLAYOUT_DEBUG
#  define   WXLO_DEBUG(x)      wxLogDebug x
#else
#  define WXLO_DEBUG(x)
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/// offsets to put a nice frame around text
#define WXLO_XOFFSET   4
#define WXLO_YOFFSET   4

/// offset to the right and bottom for when to redraw scrollbars
#define   WXLO_ROFFSET   20
#define   WXLO_BOFFSET   20

/// the size of one scrollbar page in pixels
static const int X_SCROLL_PAGE = 10;
static const int Y_SCROLL_PAGE = 20;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

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

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxLayoutWindow
// ----------------------------------------------------------------------------

wxLayoutWindow::wxLayoutWindow(wxWindow *parent)
              : wxScrolledWindow(parent, -1,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxHSCROLL | wxVSCROLL |
                                 wxBORDER |
                                 wxWANTS_CHARS)

{
   SetStatusBar(NULL); // don't use statusbar
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
   SetScrollbars(X_SCROLL_PAGE, Y_SCROLL_PAGE,
                 max.x / X_SCROLL_PAGE + 1, max.y / Y_SCROLL_PAGE + 1);
   EnableScrolling(true, true);
   m_maxx = max.x + X_SCROLL_PAGE;
   m_maxy = max.y + Y_SCROLL_PAGE;
   m_Selecting = false;

#ifdef WXLAYOUT_USE_CARET
   // FIXME cursor size shouldn't be hardcoded
   wxCaret *caret = new wxCaret(this, 2, 20);
   SetCaret(caret);
   m_llist->SetCaret(caret);
   caret->Show();
#endif // WXLAYOUT_USE_CARET

   SetCursorVisibility(-1);
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

void
wxLayoutWindow::Clear(int family,
                      int size,
                      int style,
                      int weight,
                      int underline,
                      wxColour *fg,
                      wxColour *bg)
{
   GetLayoutList()->Clear(family,size,style,weight,underline,fg,bg);
   SetBackgroundColour(GetLayoutList()->GetDefaults()->GetBGColour());
   ResizeScrollbars(true);
   SetDirty();
   SetModified(false);

   DoPaint((wxRect *)NULL);
}

void
wxLayoutWindow::OnMouse(int eventId, wxMouseEvent& event)
{
   wxClientDC dc( this );
   PrepareDC( dc );
   if ( eventId != WXLOWIN_MENU_MOUSEMOVE )
   {
       // moving the mouse in a window shouldn't give it the focus!
      SetFocus();
   }

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
   wxLayoutObject::UserData *u = obj ? obj->GetUserData() : NULL;

   //has the mouse only been moved?
   if(eventId == WXLOWIN_MENU_MOUSEMOVE)
   {
      // found is only true if we are really over an object, not just
      // behind it
      if(found && u && ! m_Selecting)
      {
         if(!m_HandCursor)
            SetCursor(wxCURSOR_HAND);
         m_HandCursor = TRUE;
         if(m_StatusBar && m_StatusFieldLabel != -1)
         {
            const wxString &label = u->GetLabel();
            if(label.Length())
               m_StatusBar->SetStatusText(label,
                                          m_StatusFieldLabel);
         }
      }
      else
      {
         if(m_HandCursor)
            SetCursor(wxCURSOR_IBEAM);
         m_HandCursor = FALSE;
         if(m_StatusBar && m_StatusFieldLabel != -1)
            m_StatusBar->SetStatusText("", m_StatusFieldLabel);
      }
      if(event.LeftIsDown())
      {
         if(! m_Selecting)
         {
            m_llist->StartSelection();
            m_Selecting = true;
            DoPaint(FALSE);
         }
         else
         {
            m_llist->ContinueSelection(cursorPos);
            DoPaint(FALSE);
         }
      }
      if(m_Selecting && ! event.LeftIsDown())
      {
         m_llist->EndSelection(cursorPos);
         m_Selecting = false;
         DoPaint(FALSE);
      }
      if(u) u->DecRef();
      return;
   }

   // always move cursor to mouse click:
   if(eventId == WXLOWIN_MENU_LCLICK)
   {
      m_llist->MoveCursorTo(cursorPos);

      // Calculate where the top of the visible area is:
      int x0, y0;
      ViewStart(&x0,&y0);
      int dx, dy;
      GetScrollPixelsPerUnit(&dx, &dy);
      x0 *= dx; y0 *= dy;

      wxPoint offset(-x0+WXLO_XOFFSET, -y0+WXLO_YOFFSET);
      m_llist->UpdateCursorScreenPos(dc, true, offset);

      if(m_CursorVisibility == -1)
         m_CursorVisibility = 1;

      // VZ: this should be unnecessary because mouse can only click on a
      //     visible part of the canvas
#if 0
      ScrollToCursor();
#endif // 0

#ifdef __WXGTK__
      DoPaint(FALSE); // DoPaint suppresses flicker under GTK
#endif // wxGTK
   }

   if(!m_doSendEvents) // nothing to do
   {
      if(u) u->DecRef();
      return;
   }

   // only do the menu if activated, editable and not on a clickable object
   if(eventId == WXLOWIN_MENU_RCLICK
      && IsEditable()
      && (! obj || u == NULL))
   {
      PopupMenu(m_PopupMenu, m_ClickPosition.x, m_ClickPosition.y);
      if(u) u->DecRef();
      return;
   }

   if(u) u->DecRef();
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
   int keyCode = event.KeyCode();

#ifdef WXLAYOUT_DEBUG
   if(keyCode == WXK_F1)
   {
      m_llist->Debug();
      return;
   }
#endif

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

   // If needed, make cursor visible:
   if(m_CursorVisibility == -1)
      m_CursorVisibility = 1;

   /* These two nested switches work like this:
      The first one processes all non-editing keycodes, to move the
      cursor, etc. It's default will process all keycodes causing
      modifications to the buffer, but only if editing is allowed.
   */
   bool ctrlDown = event.ControlDown();
   switch(keyCode)
   {
   case WXK_RIGHT:
      if ( ctrlDown )
         m_llist->MoveCursorWord(1);
      else
         m_llist->MoveCursorHorizontally(1);
      break;
   case WXK_LEFT:
      if ( ctrlDown )
         m_llist->MoveCursorWord(-1);
      else
         m_llist->MoveCursorHorizontally(-1);
      break;
   case WXK_UP:
      m_llist->MoveCursorVertically(-1);
      break;
   case WXK_DOWN:
      m_llist->MoveCursorVertically(1);
      break;
   case WXK_PRIOR:
      m_llist->MoveCursorVertically(-Y_SCROLL_PAGE);
      break;
   case WXK_NEXT:
      m_llist->MoveCursorVertically(Y_SCROLL_PAGE);
      break;
   case WXK_HOME:
      if ( ctrlDown )
         m_llist->MoveCursorTo(wxPoint(0, 0));
      else
         m_llist->MoveCursorToBeginOfLine();
      break;
   case WXK_END:
      if ( ctrlDown )
         m_llist->MoveCursorTo(m_llist->GetSize());
      else
         m_llist->MoveCursorToEndOfLine();
      break;

   default:
      if(keyCode == 'c' && ctrlDown)
      {
         // this should work even in read-only mode
         Copy();
      }
      else if( IsEditable() )
      {
         /* First, handle control keys */
         if(event.ControlDown() && ! event.AltDown())
         {
            switch(keyCode)
            {
            case WXK_INSERT:
               Copy();
               break;
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
            case 'x':
               Cut();
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
            case WXK_INSERT:
               if(event.ShiftDown())
                  Paste();
               break;
            case WXK_DELETE :
               if(event.ShiftDown())
                  Cut();
               else
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
                  if(m_WrapMargin > 0 && isspace(keyCode))
                     m_llist->WrapLine(m_WrapMargin);
                  m_llist->Insert((char)keyCode);
               }
               break;
            }
         }
         SetDirty();
         SetModified();
      }// if(IsEditable())
   }// first switch()
   if(m_Selecting)
   {
      if(event.ShiftDown())
         m_llist->ContinueSelection();
      else
      {
         m_llist->EndSelection();
         m_Selecting = false;
      }
   }

   // we must call ResizeScrollbars() before ScrollToCursor(), otherwise the
   // ne cursor position might be outside the current scrolllbar range
   ResizeScrollbars();
   ScrollToCursor();

   // refresh the screen
   DoPaint(m_llist->GetUpdateRect());
}

void
wxLayoutWindow::OnKeyUp(wxKeyEvent& event)
{
   if(event.KeyCode() == WXK_SHIFT && m_llist->IsSelecting())
   {
      m_llist->EndSelection();
      m_Selecting = false;
   }
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

   WXLO_DEBUG(("ScrollToCursor: ViewStart is %d/%d", x0, y0));

   // Get the size of the visible window:
   GetClientSize(&x1,&y1);

   // notice that the client size may be (0, 0)...
   wxASSERT(x1 >= 0 && y1 >= 0);

   // VZ: I think this is false - if you do it here, ResizeScrollbars() won't
   //     call SetScrollbars() later
#if 0
   // As we have the values anyway, use them to avoid unnecessary scrollbar
   // updates.
   if(x1 > m_maxx) m_maxx = x1;
   if(y1 > m_maxy) m_maxy = y1;
#endif // 0

   // Make sure that the scrollbars are at a position so that the cursor is
   // visible if we are editing
   WXLO_DEBUG(("m_ScrollToCursor = %d", (int) m_ScrollToCursor));
   wxPoint cc = m_llist->GetCursorScreenPos(*m_memDC);

   // the cursor should be completely visible in both directions
   wxPoint cs(m_llist->GetCursorSize());
   int nx = -1,
       ny = -1;
   if ( cc.x < x0 || cc.x >= x0 + x1 - cs.x )
   {
      nx = cc.x - x1/2;
      if ( nx < 0 )
         nx = 0;
   }

   if ( cc.y < y0 || cc.y >= y0 + y1 - cs.y )
   {
      ny = cc.y - y1/2;
      if ( ny < 0) 
         ny = 0;
   }

   if ( nx != -1 || ny != -1 )
   {
      // set new view start
      Scroll(nx == -1 ? -1 : (nx+dx-1)/dx, ny == -1 ? -1 : (ny+dy-1)/dy);

      // avoid recursion
      m_ScrollToCursor = false;
   }
}

void
wxLayoutWindow::OnPaint( wxPaintEvent &WXUNUSED(event))
{
   wxRect region = GetUpdateRegion().GetBox();
   InternalPaint(&region);
}

void
wxLayoutWindow::DoPaint(const wxRect *updateRect)
{
#ifndef __WXMSW__
   InternalPaint(updateRect);
#else
   Refresh(FALSE, updateRect); // Causes bad flicker under wxGTK!!!

   if ( !::UpdateWindow(GetHwnd()) )
      wxLogLastError("UpdateWindow");
#endif
}

void
wxLayoutWindow::InternalPaint(const wxRect *updateRect)
{
   wxPaintDC dc( this );
   PrepareDC( dc );

#ifdef WXLAYOUT_USE_CARET
   // hide the caret before drawing anything
   GetCaret()->Hide();
#endif // WXLAYOUT_USE_CARET

   int x0,y0,x1,y1, dx, dy;

   // Calculate where the top of the visible area is:
   ViewStart(&x0,&y0);
   GetScrollPixelsPerUnit(&dx, &dy);
   x0 *= dx; y0 *= dy;

   // Get the size of the visible window:
   GetClientSize(&x1,&y1);
   wxASSERT(x1 >= 0);
   wxASSERT(y1 >= 0);

   if(updateRect)
   {
      WXLO_DEBUG(("Update rect: %ld,%ld / %ld,%ld",
                  updateRect->x, updateRect->y,
                  updateRect->x+updateRect->width,
                  updateRect->y+updateRect->height));
   }
   if(IsDirty())
   {
      m_llist->Layout(dc);
      ResizeScrollbars();
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
#if WXLO_DEBUG_URECT
   if(updateRect)
   {
      WXLO_DEBUG(("Update rect: %ld,%ld / %ld,%ld",
                  updateRect->x, updateRect->y,
                  updateRect->x+updateRect->width,
                  updateRect->y+updateRect->height));
   }
#endif

   // Device origins on the memDC are suspect, we translate manually
   // with the translate parameter of Draw().
   wxPoint offset(-x0+WXLO_XOFFSET,-y0+WXLO_YOFFSET);
   m_llist->Draw(*m_memDC,offset, y0, y0+y1);

   // We start calculating a new update rect before drawing the
   // cursor, so that the cursor coordinates get included in the next
   // update rectangle (although they are drawn on the memDC, this is
   // needed to erase it):
   m_llist->InvalidateUpdateRect();
   if(m_CursorVisibility != 0)
   {
      m_llist->UpdateCursorScreenPos(dc, true, offset);
      m_llist->DrawCursor(*m_memDC,
                          m_HaveFocus && IsEditable(), // draw a thick
                          // cursor for    editable windows with focus
                          offset);
   }

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

#ifdef WXLAYOUT_USE_CARET
   // show the caret back after everything is redrawn
   m_caret->Show();
#endif // WXLAYOUT_USE_CARET

   ResetDirty();
   m_ScrollToCursor = false;

   if ( m_StatusBar && m_StatusFieldCursor != -1 )
   {
      static wxPoint s_oldCursorPos(-1, -1);

      wxPoint pos(m_llist->GetCursorPos());

      // avoid unnecessary status bar refreshes
      if ( pos != s_oldCursorPos )
      {
         s_oldCursorPos = pos;

         wxString label;
         label.Printf(_("Ln:%d Col:%d"), pos.y + 1, pos.x + 1);
         m_StatusBar->SetStatusText(label, m_StatusFieldCursor);
      }
   }
}

// change the range and position of scrollbars
void
wxLayoutWindow::ResizeScrollbars(bool exact)
{
   wxPoint max = m_llist->GetSize();

   WXLO_DEBUG(("ResizeScrollbars: max size = (%ld, %ld)",
               (long int)max.x, (long int) max.y));

   if( max.x > m_maxx - WXLO_ROFFSET || max.y > m_maxy - WXLO_BOFFSET || exact )
   {
      if ( !exact )
      {
         // add an extra bit to the sizes to avoid future updates
         max.x += WXLO_ROFFSET;
         max.y += WXLO_BOFFSET;
      }

      ViewStart(&m_ViewStartX, &m_ViewStartY);
      SetScrollbars(X_SCROLL_PAGE, Y_SCROLL_PAGE,
                    max.x / X_SCROLL_PAGE + 1, max.y / Y_SCROLL_PAGE + 1,
                    m_ViewStartX, m_ViewStartY,
                    true);

      m_maxx = max.x + X_SCROLL_PAGE;
      m_maxy = max.y + Y_SCROLL_PAGE;
   }
}

void
wxLayoutWindow::Paste(void)
{
   // Read some text
   if (wxTheClipboard->Open())
   {
#if wxUSE_PRIVATE_CLIPBOARD_FORMAT
      wxLayoutDataObject wxldo;
      if (wxTheClipboard->IsSupported( wxldo.GetFormat() ))
      {
         wxTheClipboard->GetData(&wxldo);
         {
         }
         //FIXME: missing functionality m_llist->Insert(wxldo.GetList());
      }
      else
#endif
      {
         wxTextDataObject data;
         if (wxTheClipboard->IsSupported( data.GetFormat() ))
         {
            wxTheClipboard->GetData(&data);
            wxString text = data.GetText();
            wxLayoutImportText( m_llist, text);
         }
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
}

bool
wxLayoutWindow::Copy(bool invalidate)
{
   // Calling GetSelection() will automatically do an EndSelection()
   // on the list, but we need to take a note of it, too:
   if(m_Selecting)
   {
      m_Selecting = false;
      m_llist->EndSelection();
   }

   wxLayoutDataObject wldo;
   wxLayoutList *llist = m_llist->GetSelection(&wldo, invalidate);
   if(! llist)
      return FALSE;
   // Export selection as text:
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

   // The exporter always appends a newline, so we chop it off if it
   // is there:
   {
      size_t len = text.Length();
      if(len > 2 && text[len-2] ==  '\r') // Windows
         text = text.Mid(0,len-2);
      else if(len > 1 && text[len-1] == '\n')
         text = text.Mid(0,len-1);
   }


   if (wxTheClipboard->Open())
   {
      wxTextDataObject *data = new wxTextDataObject( text );
      bool  rc = wxTheClipboard->SetData( data );
#if wxUSE_PRIVATE_CLIPBOARD_FORMAT
      rc |= wxTheClipboard->AddData( &wldo );
#endif
      wxTheClipboard->Close();
      return rc;
   }

   return FALSE;
}

bool
wxLayoutWindow::Cut(void)
{
   if(Copy(false)) // do not invalidate selection after copy
   {
      m_llist->DeleteSelection();
      return TRUE;
   }
   else
      return FALSE;
}
bool
wxLayoutWindow::Find(const wxString &needle,
                     wxPoint * fromWhere)
{
   wxPoint found;

   if(fromWhere == NULL)
      found = m_llist->FindText(needle, m_llist->GetCursorPos());
   else
      found = m_llist->FindText(needle, *fromWhere);
   if(found.x != -1)
   {
      if(fromWhere)
      {
         *fromWhere = found;
         fromWhere->x ++;
      }
      m_llist->MoveCursorTo(found);
      ScrollToCursor();
      return true;
   }
   return false;
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
   ev.Skip();
}

void
wxLayoutWindow::OnKillFocus(wxFocusEvent &ev)
{
   m_HaveFocus = false;
   ev.Skip();
}
