/////////////////////////////////////////////////////////////////////////////
// Name:        dlghndlr.cpp
// Purpose:     Dialog handler
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if wxUSE_IOSTREAMH
#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#else
#include <strstream>
#endif

#include "reseditr.h"
#include "winprop.h"
#include "dlghndlr.h"
#include "edlist.h"

IMPLEMENT_CLASS(wxResourceEditorDialogHandler, wxEvtHandler)
IMPLEMENT_CLASS(wxResourceEditorControlHandler, wxEvtHandler)

BEGIN_EVENT_TABLE(wxResourceEditorDialogHandler, wxEvtHandler)
	EVT_PAINT(wxResourceEditorDialogHandler::OnPaint)
	EVT_MOUSE_EVENTS(wxResourceEditorDialogHandler::OnMouseEvent)
	EVT_SIZE(wxResourceEditorDialogHandler::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxResourceEditorControlHandler, wxEvtHandler)
	EVT_MOUSE_EVENTS(wxResourceEditorControlHandler::OnMouseEvent)
END_EVENT_TABLE()

/*
 * Dialog box event handler
 */

wxResourceEditorDialogHandler::wxResourceEditorDialogHandler(wxPanel *dialog, wxItemResource *resource,
   wxEvtHandler *oldHandler, wxResourceManager *manager)
{
  handlerDialog = dialog;
  handlerResource = resource;
  handlerOldHandler = oldHandler;
  resourceManager = manager;

  dragMode = wxDRAG_MODE_NONE;
  dragType = wxDRAG_TYPE_NONE;
  dragItem = NULL;
  firstDragX = 0;
  firstDragY = 0;
  oldDragX = 0;
  oldDragY = 0;
  dragTolerance = 3;
  checkTolerance = TRUE;
  m_mouseCaptured = FALSE;
//  m_treeItem = 0;
}

wxResourceEditorDialogHandler::~wxResourceEditorDialogHandler(void)
{
}

void wxResourceEditorDialogHandler::OnItemSelect(wxControl *item, bool select)
{
  if (select)
    resourceManager->AddSelection(item);
  else
    resourceManager->RemoveSelection(item);
}

void wxResourceEditorDialogHandler::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(handlerDialog);

	PaintSelectionHandles(dc);
}

// Add event handlers for all children
void wxResourceEditorDialogHandler::AddChildHandlers(void)
{
  wxNode *node = handlerDialog->GetChildren().First();
  while ( node )
  {
	wxControl *child = (wxControl *)node->Data();
	wxEvtHandler *childHandler = child->GetEventHandler();
	if ( child->IsKindOf(CLASSINFO(wxControl)) && childHandler == child )
		child->PushEventHandler(new wxResourceEditorControlHandler(child, childHandler));
	node = node->Next();
  }
}

void wxResourceEditorDialogHandler::OnLeftClick(int x, int y, int keys)
{
  if (keys & wxKEY_CTRL)
  {
    wxResourceManager::GetCurrentResourceManager()->EditWindow(handlerDialog);
    return;
  }

  // Deselect all items if click on panel
  if (wxResourceManager::GetCurrentResourceManager()->GetEditorControlList()->GetSelection() == RESED_POINTER)
  {
    int needsRefresh = 0;
    wxNode *node = handlerDialog->GetChildren().First();
    while (node)
    {
      wxControl *item = (wxControl *)node->Data();
	  wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
      if (item->IsKindOf(CLASSINFO(wxControl)) && childHandler->IsSelected())
      {
        needsRefresh ++;
        OnItemSelect(item, FALSE);
        childHandler->SelectItem(FALSE);
      }
      node = node->Next();
    }
    if (needsRefresh > 0)
    {
	  wxClientDC dc(handlerDialog);
      dc.Clear();
      handlerDialog->Refresh();
    }
    return;
  }

  // Round down to take account of dialog units
  wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(handlerDialog);
  if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
  {
        wxPoint pt = handlerDialog->ConvertPixelsToDialog(wxPoint(x, y));

        // Convert back so we've rounded down
        pt = handlerDialog->ConvertDialogToPixels(pt);
        pt = handlerDialog->ConvertPixelsToDialog(pt);
        pt = handlerDialog->ConvertDialogToPixels(pt);
        x = pt.x;
        y = pt.y;
  }

  switch (wxResourceManager::GetCurrentResourceManager()->GetEditorControlList()->GetSelection())
  {
        case RESED_BUTTON:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxButton", x, y);
          break;
        case RESED_BMPBUTTON:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxBitmapButton", x, y, TRUE);
          break;
        case RESED_STATICTEXT:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxStaticText", x, y);
          break;
        case RESED_STATICBMP:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxStaticBitmap", x, y, TRUE);
          break;
        case RESED_TEXTCTRL_SINGLE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxTextCtrl (single-line)", x, y);
          break;
        case RESED_TEXTCTRL_MULTIPLE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxTextCtrl (multi-line)", x, y);
          break;
        case RESED_CHOICE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxChoice", x, y);
          break;
        case RESED_COMBOBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxComboBox", x, y);
          break;
        case RESED_CHECKBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxCheckBox", x, y);
          break;
        case RESED_RADIOBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxRadioBox", x, y);
          break;
        case RESED_RADIOBUTTON:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxRadioButton", x, y);
          break;
        case RESED_LISTBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxListBox", x, y);
          break;
        case RESED_SLIDER:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxSlider", x, y);
          break;
        case RESED_GAUGE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxGauge", x, y);
          break;
        case RESED_STATICBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxStaticBox", x, y);
          break;
        case RESED_SCROLLBAR:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxScrollBar", x, y);
          break;
        default:
          break;
  }

  // Now switch pointer on.
  if (wxResourceManager::GetCurrentResourceManager()->GetEditorControlList()->GetSelection() != RESED_POINTER)
  {
    wxResourceManager::GetCurrentResourceManager()->GetEditorControlList()->SetItemState(RESED_POINTER, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  }
}

void wxResourceEditorDialogHandler::OnRightClick(int x, int y, int WXUNUSED(keys))
{
  wxMenu *menu = resourceManager->GetPopupMenu();
  menu->SetClientData((char *)handlerDialog);
  handlerDialog->PopupMenu(menu, x, y);
}

void wxResourceEditorDialogHandler::OnItemLeftClick(wxControl *item, int WXUNUSED(x), int WXUNUSED(y), int keys)
{
  if (keys & wxKEY_CTRL)
  {
    wxResourceManager::GetCurrentResourceManager()->EditWindow(item);
    return;
  }

/*
  // If this is a wxStaticBox and the pointer isn't an arrow, chances
  // are that we really meant to place an item on the panel.
  // Fake this event.
  if ((item->GetClassInfo() == CLASSINFO(wxStaticBox)) && resourceManager->GetEditorPalette()->currentlySelected != PALETTE_ARROW)
  {
    OnLeftClick(x,  y, keys);
    return;
  }
*/
  
  wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();

  if (childHandler->IsSelected())
  {
    childHandler->SelectItem(FALSE);
    OnItemSelect(item, FALSE);

    wxClientDC dc(handlerDialog);
    dc.Clear();
    handlerDialog->Refresh();
  }
  else
  {
    childHandler->SelectItem(TRUE);
    OnItemSelect(item, TRUE);
    
    // Deselect other items if shift is not pressed
    int needsRefresh = 0;
    if (!(keys & wxKEY_SHIFT))
    {
      wxNode *node = item->GetParent()->GetChildren().First();
      while (node)
      {
        wxControl *child = (wxControl *)node->Data();
        wxResourceEditorControlHandler *childHandler2 = (wxResourceEditorControlHandler *)child->GetEventHandler();
        if (child->IsKindOf(CLASSINFO(wxControl)) && childHandler2->IsSelected() && child != item)
        {
          childHandler2->SelectItem(FALSE);
          OnItemSelect(child, FALSE);
          needsRefresh ++;
        }
        node = node->Next();
      }
    }

    wxClientDC dc(handlerDialog);
    childHandler->DrawSelectionHandles(dc);

    if (needsRefresh > 0)
    {
      dc.Clear();
      handlerDialog->Refresh();
    }
  }
}

void wxResourceEditorDialogHandler::OnItemRightClick(wxControl *item, int x, int y, int WXUNUSED(keys))
{
/*
  if (keys & wxKEY_CTRL)
  {
    wxDebugMsg("Item %s, selected = %d\n", item->GetName(), item->IsSelected());
    return;
  }
*/

  wxMenu *menu = resourceManager->GetPopupMenu();
  menu->SetClientData((char *)item);
  handlerDialog->PopupMenu(menu, x, y);
}

// Under Windows 95, you can resize a panel interactively depending on
// window styles.
void wxResourceEditorDialogHandler::OnSize(wxSizeEvent& event)
{
    // Update the associated resource
    int w, h;
    handlerDialog->GetClientSize(& w, & h);

    wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(handlerDialog);
    if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
    {
        wxSize sz = handlerDialog->ConvertPixelsToDialog(wxSize(w, h));
        w = sz.x; h = sz.y;
    }
    resource->SetSize(resource->GetX(), resource->GetY(), w, h);
}

// An event outside any items: may be a drag event.
void wxResourceEditorDialogHandler::OnMouseEvent(wxMouseEvent& event)
{
  if (GetEvtHandlerEnabled())
  {
    // If we're dragging an item or selection handle,
    // continue dragging.
    if (dragMode != wxDRAG_MODE_NONE)
    {
      ProcessItemEvent(dragItem, event, dragType);
      return;
    }
  
    long x, y;
    event.Position(&x, &y);

    // Find which selection handle we're on, if any
    wxNode *node = handlerDialog->GetChildren().First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win->IsKindOf(CLASSINFO(wxControl)))
      {
        wxControl *item = (wxControl *)win;
        wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
        int selHandle = childHandler->SelectionHandleHitTest(x, y);
        if (selHandle > 0)
        {
          ProcessItemEvent(item, event, selHandle);
          return;
        }
      }
      node = node->Next();
    }

    // We're not on an item or selection handle.
    // so... check for a left or right click event
    // to send to the application.
    int keys = 0;
    if (event.ShiftDown()) keys = keys | wxKEY_SHIFT;
    if (event.ControlDown()) keys = keys | wxKEY_CTRL;

    if (event.LeftUp())
    {
        if (m_mouseCaptured)
        {
            handlerDialog->ReleaseMouse();
            m_mouseCaptured = FALSE;
        }

        OnLeftClick(x, y, keys);
    }
    else if (event.RightDown())
    {
        if (m_mouseCaptured)
        {
            handlerDialog->ReleaseMouse();
            m_mouseCaptured = FALSE;
        }

        OnRightClick(x, y, keys);
    }
    else if (event.LeftDClick())
    {
        if (m_mouseCaptured)
        {
            handlerDialog->ReleaseMouse();
            m_mouseCaptured = FALSE;
        }
        wxResourceManager::GetCurrentResourceManager()->EditWindow(handlerDialog);
    }
  }
  else
	event.Skip();
}

void wxResourceEditorDialogHandler::OnItemEvent(wxControl *item, wxMouseEvent& event)
{
  if (!GetEvtHandlerEnabled())
    return;

  // Not a selection handle event: just a normal item event.
  // Transform to panel coordinates.
  int x, y;
  item->GetPosition(&x, &y);

  event.m_x = event.m_x + x;
  event.m_y = event.m_y + y;
  
  ProcessItemEvent(item, event, dragType);
}

void wxResourceEditorDialogHandler::ProcessItemEvent(wxControl *item, wxMouseEvent& event, int selectionHandle)
{
  wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();
  
  long x, y;
  event.Position(&x, &y);
  int keys = 0;
  if (event.ShiftDown()) keys = keys | wxKEY_SHIFT;
  if (event.ControlDown()) keys = keys | wxKEY_CTRL;
  bool dragging = event.Dragging();
  if (dragging)
  {
    int dx = (int)abs((x - firstDragX));
    int dy = (int)abs((y - firstDragY));
    if (checkTolerance && (dx <= dragTolerance) && (dy <= dragTolerance))
    {
      return;
    }
    else
      // If we've ignored the tolerance once, then ALWAYS ignore
      // tolerance in this drag, even if we come back within
      // the tolerance range.
      {
        checkTolerance = FALSE;
      }
  }
  if (event.LeftDClick())
  {
      if (m_mouseCaptured)
      {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
      }

      wxResourceManager::GetCurrentResourceManager()->EditWindow(item);
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_START_LEFT)
  {
    dragMode = wxDRAG_MODE_CONTINUE_LEFT;
	wxClientDC dc(handlerDialog);
    childHandler->OnDragBegin(x, y, keys, dc, selectionHandle);
    oldDragX = x; oldDragY = y;
    if (!m_mouseCaptured)
    {
        handlerDialog->CaptureMouse();
        m_mouseCaptured = TRUE;
    }
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_CONTINUE_LEFT)
  { 
	wxClientDC dc(handlerDialog);
    childHandler->OnDragContinue(FALSE, oldDragX, oldDragY, keys, dc, selectionHandle);
    childHandler->OnDragContinue(TRUE, x, y, keys, dc, selectionHandle);
    oldDragX = x; oldDragY = y;
  }
  else if (event.LeftUp() && dragItem && dragMode == wxDRAG_MODE_CONTINUE_LEFT)
  {
	wxClientDC dc(handlerDialog);
    dragMode = wxDRAG_MODE_NONE;
    checkTolerance = TRUE;

    childHandler->OnDragContinue(FALSE, oldDragX, oldDragY, keys, dc, selectionHandle);
    childHandler->OnDragEnd(x, y, keys, dc, selectionHandle);

    dragItem = NULL;
    dragType = wxDRAG_TYPE_NONE;

    if (m_mouseCaptured)
    {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
    }
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_START_RIGHT)
  {
	wxClientDC dc(handlerDialog);
    dragMode = wxDRAG_MODE_CONTINUE_RIGHT;
    childHandler->OnDragBegin(x, y, keys, dc, selectionHandle);
    oldDragX = x; oldDragY = y;

    if (!m_mouseCaptured)
    {
        handlerDialog->CaptureMouse();
        m_mouseCaptured = TRUE;
    }
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_CONTINUE_RIGHT)
  { 
    oldDragX = x; oldDragY = y;
  }
  // Obsolete; no longer try to right-drag
  else if (event.RightUp() && dragItem && dragMode == wxDRAG_MODE_CONTINUE_RIGHT)
  {
    dragMode = wxDRAG_MODE_NONE;
    checkTolerance = TRUE;
    dragItem = NULL;
    dragType = wxDRAG_TYPE_NONE;

    if (m_mouseCaptured)
    {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
    }
  }
  else if (event.IsButton())
  {
    checkTolerance = TRUE;

    if (event.LeftDown())
    {
      dragItem = item;
      dragMode = wxDRAG_MODE_START_LEFT;
      firstDragX = x;
      firstDragY = y;
      dragType = selectionHandle;

      if (!m_mouseCaptured)
      {
        handlerDialog->CaptureMouse();
        m_mouseCaptured = TRUE;
      }
    }
    else if (event.RightDown())
    {
      if (m_mouseCaptured)
      {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
      }

      if (item)
        childHandler->OnRightClick(x, y, keys);
      else
        OnRightClick(x, y, keys);

      dragItem = NULL; dragMode = wxDRAG_MODE_NONE; dragType = wxDRAG_TYPE_NONE;

      /*
      dragItem = item;
      dragMode = wxDRAG_MODE_START_RIGHT;
      firstDragX = x;
      firstDragY = y;
      dragType = selectionHandle;

      if (!m_mouseCaptured)
      {
        handlerDialog->CaptureMouse();
        m_mouseCaptured = TRUE;
      }
      */
    }
    else if (event.LeftUp())
    {
      if (dragItem)
        childHandler->OnLeftClick(x, y, keys);
      else
        OnLeftClick(x, y, keys);

      dragItem = NULL; dragMode = wxDRAG_MODE_NONE; dragType = wxDRAG_TYPE_NONE;

      if (m_mouseCaptured)
      {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
      }
    }
    else if (event.RightUp())
    {
      /*
      if (dragItem)
        childHandler->OnRightClick(x, y, keys);
      else
        OnRightClick(x, y, keys);

      dragItem = NULL; dragMode = wxDRAG_MODE_NONE; dragType = wxDRAG_TYPE_NONE;

      if (m_mouseCaptured)
      {
        handlerDialog->ReleaseMouse();
        m_mouseCaptured = FALSE;
      }
      */
    }
  }
}

// Calls DrawSelectionHandles for all items if
// edit mode is on.
void wxResourceEditorDialogHandler::PaintSelectionHandles(wxDC& dc)
{
  if (!GetEvtHandlerEnabled())
    return;

  dc.BeginDrawing();

  wxNode *node = handlerDialog->GetChildren().First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxControl)))
    {
      wxControl *item = (wxControl *)win;
      wxResourceEditorControlHandler *childHandler = (wxResourceEditorControlHandler *)item->GetEventHandler();

      // Don't draw handles for an item that's being moved: it'll
      // smear.
      if (childHandler->IsSelected() && (item != dragItem))
        childHandler->DrawSelectionHandles(dc);
    }
    node = node->Next();
  }
  dc.EndDrawing();
}

/*
 * Event handler for controls
 */

int wxResourceEditorControlHandler::dragOffsetX = 0;
int wxResourceEditorControlHandler::dragOffsetY = 0;

wxResourceEditorControlHandler::wxResourceEditorControlHandler(wxControl *control,
   wxEvtHandler *oldHandler)
{
  handlerControl = control;
  handlerOldHandler = oldHandler;

  handleSize = 6;
  handleMargin = 1;
  isSelected = FALSE;
  dragOffsetX = 0;
  dragOffsetY = 0;
//  m_treeItem = 0;
}

wxResourceEditorControlHandler::~wxResourceEditorControlHandler(void)
{
}

/*
 * Manipulation and drawing of items in Edit Mode
 */
 
void wxResourceEditorControlHandler::SelectItem(bool select)
{
  isSelected = select;
}

// Returns TRUE or FALSE
bool wxResourceEditorControlHandler::HitTest(int x, int y)
{
  int xpos, ypos, width, height;
  handlerControl->GetPosition(&xpos, &ypos);
  handlerControl->GetSize(&width, &height);

  return ((x >= xpos) && (x <= (xpos + width)) && (y >= ypos) && (y <= (ypos + height)));
}

// Calculate position of the 8 handles
void wxResourceEditorControlHandler::CalcSelectionHandles(int *hx, int *hy)
{
  int xpos, ypos, width, height;
  handlerControl->GetPosition(&xpos, &ypos);
  handlerControl->GetSize(&width, &height);
  int middleX = (xpos + (width/2));
  int middleY = (ypos + (height/2));

  // Start from top middle, clockwise.
/*
  7      0      1

  6             2

  5      4      3
*/

  hx[0] = (int)(middleX - (handleSize/2));
  hy[0] = ypos - handleSize - handleMargin;

  hx[1] = xpos + width + handleMargin;
  hy[1] = ypos - handleSize - handleMargin;

  hx[2] = xpos + width + handleMargin;
  hy[2] = (int)(middleY - (handleSize/2));

  hx[3] = xpos + width + handleMargin;
  hy[3] = ypos + height + handleMargin;

  hx[4] = (int)(middleX - (handleSize/2));
  hy[4] = ypos + height + handleMargin;

  hx[5] = xpos - handleSize - handleMargin;
  hy[5] = ypos + height + handleMargin;

  hx[6] = xpos - handleSize - handleMargin;
  hy[6] = (int)(middleY - (handleSize/2));

  hx[7] = xpos - handleSize - handleMargin;
  hy[7] = ypos - handleSize - handleMargin;
}

// Returns 0 (no hit), 1 - 8 for which selection handle
// (clockwise from top middle)
int wxResourceEditorControlHandler::SelectionHandleHitTest(int x, int y)
{
  // Handle positions
  int hx[8];
  int hy[8];
  CalcSelectionHandles(hx, hy);

  int i;
  for (i = 0; i < 8; i++)
  {
    if ((x >= hx[i]) && (x <= (hx[i] + handleSize)) && (y >= hy[i]) && (y <= (hy[i] + handleSize)))
      return (i + 1);
  }
  return 0;
}

void wxResourceEditorControlHandler::DrawSelectionHandles(wxDC& dc, bool WXUNUSED(erase))
{
  dc.SetOptimization(FALSE);

  dc.SetLogicalFunction(wxCOPY);
  dc.SetPen(* wxBLACK_PEN);
  dc.SetBrush(* wxBLACK_BRUSH);

  dc.SetOptimization(TRUE);

  // Handle positions
  int hx[8];
  int hy[8];
  CalcSelectionHandles(hx, hy);

  int i;
  for (i = 0; i < 8; i++)
  {
    dc.DrawRectangle(hx[i], hy[i], handleSize, handleSize);
  }
}

void wxResourceEditorControlHandler::DrawBoundingBox(wxDC& dc, int x, int y, int w, int h)
{
  dc.DrawRectangle(x, y, w, h);
}

// If selectionHandle is zero, not dragging the selection handle.
void wxResourceEditorControlHandler::OnDragBegin(int x, int y, int WXUNUSED(keys), wxDC& dc, int selectionHandle)
{
  int xpos, ypos, width, height;
  handlerControl->GetPosition(&xpos, &ypos);
  handlerControl->GetSize(&width, &height);

  dc.BeginDrawing();

//  dc.DestroyClippingRegion();

  wxPanel *panel = (wxPanel *)handlerControl->GetParent();

  // Erase selection handles
//  DrawSelectionHandles(dc, TRUE);

  dc.SetOptimization(FALSE);

  dc.SetLogicalFunction(wxINVERT);

  wxPen pen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(pen);
  dc.SetBrush(* wxTRANSPARENT_BRUSH);

  dc.SetOptimization(TRUE);

  if (selectionHandle > 0)
  {
    panel->Refresh();

    DrawBoundingBox(dc, xpos, ypos, width, height);
  }
  else
  {
    panel->Refresh();

    dragOffsetX = (x - xpos);
    dragOffsetY = (y - ypos);

      DrawBoundingBox(dc, xpos, ypos, width, height);

      // Also draw bounding boxes for other selected items
      wxNode *node = panel->GetChildren().First();
      while (node)
      {
        wxWindow *win = (wxWindow *)node->Data();
        if (win->IsKindOf(CLASSINFO(wxControl)))
        {
          wxControl *item = (wxControl *)win;
		  wxResourceEditorControlHandler *handler = (wxResourceEditorControlHandler *)item->GetEventHandler();
          if ((item != handlerControl) && handler->IsSelected())
          {
            int x1, y1, w1, h1;
            item->GetPosition(&x1, &y1);
            item->GetSize(&w1, &h1);
            handler->DrawBoundingBox(dc, x1, y1, w1, h1);
          }
        }
        node = node->Next();
      }
  }
  dc.EndDrawing();
}

void wxResourceEditorControlHandler::OnDragContinue(bool WXUNUSED(paintIt), int x, int y, int WXUNUSED(keys), wxDC& dc, int selectionHandle)
{
  wxPanel *panel = (wxPanel *)handlerControl->GetParent();
  int xpos, ypos, width, height;
  handlerControl->GetPosition(&xpos, &ypos);
  handlerControl->GetSize(&width, &height);
  
  if (selectionHandle > 0)
  {
/*
  8      1      2

  7             3

  6      5      4
*/

    int x1, y1, width1, height1;
  
    switch (selectionHandle)
    {
      case 1:
        x1 = xpos;
        y1 = y;
        width1 = width;
        height1 = (ypos + height) - y;
        break;
      case 5:
        x1 = xpos;
        y1 = ypos;
        width1 = width;
        height1 = (y - ypos);
        break;
      case 3:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = height;
        break;
      case 7:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = height;
        break;
      case 2:
        x1 = xpos;
        y1 = y;
        width1 = (x - xpos);
        height1 = (ypos + height) - y;
        break;
      case 4:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = (y - ypos);
        break;
      case 6:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = y - ypos;
        break;
      case 8:
        x1 = x;
        y1 = y;
        width1 = (xpos + width) - x;
        height1 = (ypos + height) - y;
        break;
    }
    dc.BeginDrawing();

    dc.SetLogicalFunction(wxINVERT);
    wxPen pen(wxColour(0, 0, 0), 1, wxDOT);
    dc.SetPen(pen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    DrawBoundingBox(dc, x1, y1, width1, height1);

    dc.EndDrawing();
  }
  else
  {
      dc.BeginDrawing();
      dc.SetLogicalFunction(wxINVERT);
      wxPen pen(wxColour(0, 0, 0), 1, wxDOT);
      dc.SetPen(pen);
      dc.SetBrush(* wxTRANSPARENT_BRUSH);

      DrawBoundingBox(dc, (int)(x - dragOffsetX), (int)(y - dragOffsetY), width, height);

      // Also draw bounding boxes for other selected items
      wxNode *node = panel->GetChildren().First();
      while (node)
      {
        wxWindow *win = (wxWindow *)node->Data();
        if (win->IsKindOf(CLASSINFO(wxControl)))
        {
          wxControl *item = (wxControl *)win;
		  wxResourceEditorControlHandler *handler = (wxResourceEditorControlHandler *)item->GetEventHandler();
          if ((item != handlerControl) && handler->IsSelected())
          {
            int x1, y1, w1, h1;
            item->GetPosition(&x1, &y1);
            item->GetSize(&w1, &h1);
            int x2 = (int)(x1 + (x - dragOffsetX) - xpos);
            int y2 = (int)(y1 + (y - dragOffsetY) - ypos);
            handler->DrawBoundingBox(dc, x2, y2, w1, h1);
          }
        }
        node = node->Next();
      }
      dc.EndDrawing();
  }
}

void wxResourceEditorControlHandler::OnDragEnd(int x, int y, int WXUNUSED(keys), wxDC& dc, int selectionHandle)
{
  wxPanel *panel = (wxPanel *)handlerControl->GetParent();

  dc.BeginDrawing();

  int xpos, ypos, width, height;
  handlerControl->GetPosition(&xpos, &ypos);
  handlerControl->GetSize(&width, &height);

  wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(handlerControl);
  wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(handlerControl->GetParent());

  if (selectionHandle > 0)
  {
    int x1, y1, width1, height1;
  
    switch (selectionHandle)
    {
      case 1:
        x1 = xpos;
        y1 = y;
        width1 = width;
        height1 = (ypos + height) - y;
        break;
      case 5:
        x1 = xpos;
        y1 = ypos;
        width1 = width;
        height1 = (y - ypos);
        break;
      case 3:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = height;
        break;
      case 7:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = height;
        break;
      case 2:
        x1 = xpos;
        y1 = y;
        width1 = (x - xpos);
        height1 = (ypos + height) - y;
        break;
      case 4:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = (y - ypos);
        break;
      case 6:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = y - ypos;
        break;
      case 8:
        x1 = x;
        y1 = y;
        width1 = (xpos + width) - x;
        height1 = (ypos + height) - y;
        break;
    }
    // Update the associated resource
    // We need to convert to dialog units if this is not a dialog or panel, but
    // the parent resource specifies dialog units.
    int resourceX = x1;
    int resourceY = y1;
    int resourceWidth = width1;
    int resourceHeight = height1;

    if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
    {
        wxPoint pt = handlerControl->GetParent()->ConvertPixelsToDialog(wxPoint(x1, y1));
        wxSize sz = handlerControl->GetParent()->ConvertPixelsToDialog(wxSize(width1, height1));

        // Convert back so we've rounded down
        sz = handlerControl->GetParent()->ConvertDialogToPixels(sz);
        sz = handlerControl->GetParent()->ConvertPixelsToDialog(sz);
        resourceWidth = sz.x; resourceHeight = sz.y;

        sz = handlerControl->GetParent()->ConvertDialogToPixels(sz);
        width1 = sz.x;
        height1 = sz.y;

        pt = handlerControl->GetParent()->ConvertDialogToPixels(pt);
        pt = handlerControl->GetParent()->ConvertPixelsToDialog(pt);
        resourceX = pt.x; resourceY = pt.y;

        pt = handlerControl->GetParent()->ConvertDialogToPixels(pt);
        x1 = pt.x;
        y1 = pt.y;
    }
    handlerControl->SetSize(x1, y1, width1, height1);
    resource->SetSize(resourceX, resourceY, resourceWidth, resourceHeight);
  }
  else
  {
    // Correction 31/12/98. We need to round down the values to take into account
    // the fact that several pixels map to the same dialog unit.

    int newX = (int)(x - dragOffsetX);
    int newY = (int)(y - dragOffsetY);
    int resourceX = newX;
    int resourceY = newY;

    // Update the associated resource
    if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
    {
        wxPoint pt = handlerControl->GetParent()->ConvertPixelsToDialog(wxPoint(newX, newY));
        pt = handlerControl->GetParent()->ConvertDialogToPixels(pt);
        pt = handlerControl->GetParent()->ConvertPixelsToDialog(pt);
        resourceX = pt.x; resourceY = pt.y;
        pt = handlerControl->GetParent()->ConvertDialogToPixels(pt);

        // Having converted it several times, we know it'll map to dialog units exactly.
        newX = pt.x;
        newY = pt.y;
    }
    handlerControl->Move(newX, newY);
    OldOnMove(newX, newY);

    resource->SetSize(resourceX, resourceY, resource->GetWidth(), resource->GetHeight());

    // Also move other selected items
    wxNode *node = panel->GetChildren().First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win->IsKindOf(CLASSINFO(wxControl)))
      {
        wxControl *item = (wxControl *)win;
        wxResourceEditorControlHandler *handler = (wxResourceEditorControlHandler *)item->GetEventHandler();
        if ((item != handlerControl) && handler->IsSelected())
        {
          int x1, y1;
          item->GetPosition(&x1, &y1);
          int x2 = (int)(x1 + (x - dragOffsetX) - xpos);
          int y2 = (int)(y1 + (y - dragOffsetY) - ypos);

          // Update the associated resource
          resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(item);
          if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
          {
            wxPoint pt = item->GetParent()->ConvertPixelsToDialog(wxPoint(x2, y2));
            pt = item->GetParent()->ConvertDialogToPixels(pt);
            pt = item->GetParent()->ConvertPixelsToDialog(pt);

            resourceX = pt.x; resourceY = pt.y;
            pt = handlerControl->GetParent()->ConvertDialogToPixels(pt);

            // Having converted it several times, we know it'll map to dialog units exactly
            x2 = pt.x;
            y2 = pt.y;
          }

          item->Move(x2, y2);
          ((wxResourceEditorControlHandler *)item->GetEventHandler())->OldOnMove(x2, y2);
          ((wxResourceEditorControlHandler *)item->GetEventHandler())->DrawSelectionHandles(dc);

          resource->SetSize(resourceX, resourceY, resource->GetWidth(), resource->GetHeight());

        }
      }
      node = node->Next();
    }
  }
  dc.SetOptimization(FALSE);

  dc.SetLogicalFunction(wxCOPY);
  dc.SetPen(* wxBLACK_PEN);
  dc.SetBrush(* wxBLACK_BRUSH);

  dc.SetOptimization(TRUE);

  // Force it to repaint the selection handles (if any)
  // since the panel thinks we're still within a drag and
  // won't paint the handles.
  if (IsSelected())
    DrawSelectionHandles(dc);

  dc.EndDrawing();

  panel->Refresh();
}

// These functions call OnItemEvent, OnItemMove and OnItemSize
// by default.
void wxResourceEditorControlHandler::OnMouseEvent(wxMouseEvent& event)
{
/*
  if ((event.m_eventType == wxEVENT_TYPE_LEFT_DCLICK) ||
      (event.m_eventType == wxEVENT_TYPE_RIGHT_DCLICK))
    return;
*/
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;
  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  if ( !panelHandler->GetEvtHandlerEnabled() )
  {
	event.Skip();
	return;
  }
    
  panelHandler->OnItemEvent(handlerControl, event);
}

void wxResourceEditorControlHandler::OldOnMove(int x, int y)
{
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;

  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  panelHandler->OnItemMove(handlerControl, x, y);
}

void wxResourceEditorControlHandler::OldOnSize(int w, int h)
{
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;

  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  panelHandler->OnItemSize(handlerControl, w, h);
}

void wxResourceEditorControlHandler::OnSelect(bool select)
{
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;

  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  panelHandler->OnItemSelect(handlerControl, select);
}

void wxResourceEditorControlHandler::OnLeftClick(int x, int y, int keys)
{
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;

  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  panelHandler->OnItemLeftClick(handlerControl, x, y, keys);
}

void wxResourceEditorControlHandler::OnRightClick(int x, int y, int keys)
{
  wxWindow *panel = handlerControl->GetParent();
  if ( !panel->GetEventHandler()->IsKindOf(CLASSINFO(wxResourceEditorDialogHandler)) )
	return;

  wxResourceEditorDialogHandler *panelHandler = (wxResourceEditorDialogHandler *)panel->GetEventHandler();
  panelHandler->OnItemRightClick(handlerControl, x, y, keys);
}


