/////////////////////////////////////////////////////////////////////////////
// Name:        tabg.cpp
// Purpose:     Generic tabbed dialogs
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tabg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_TAB_DIALOG

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "wx/tab.h"

// not defined: use old, square tab implementation (fills in tabs)
// defined: use new, rounded tab implementation (doesn't colour in tabs)
// #define wxUSE_NEW_METHOD

IMPLEMENT_DYNAMIC_CLASS(wxTabControl, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxTabLayer, wxList)

wxTabControl::wxTabControl(wxTabView *v)
{
  m_view = v;
  m_isSelected = FALSE;
  m_offsetX = 0;
  m_offsetY = 0;
  m_width = 0;
  m_height = 0;
  m_id = 0;
  m_rowPosition = 0;
  m_colPosition = 0;
}

wxTabControl::~wxTabControl(void)
{
}

void wxTabControl::OnDraw(wxDC& dc, bool lastInRow)
{
    // Old, but in some ways better (drawing opaque tabs)
#ifndef wxUSE_NEW_METHOD
  if (!m_view)
    return;

  // Top-left of tab view area
  int viewX = m_view->GetViewRect().x;
  int viewY = m_view->GetViewRect().y;

  // Top-left of tab control
  int tabX = GetX() + viewX;
  int tabY = GetY() + viewY;
  int tabHeightInc = 0;
  if (m_isSelected)
  {
    tabHeightInc = (m_view->GetTabSelectionHeight() - m_view->GetTabHeight());
    tabY -= tabHeightInc;
  }

  dc.SetPen(*wxTRANSPARENT_PEN);

  // Draw grey background
  if (m_view->GetTabStyle() & wxTAB_STYLE_COLOUR_INTERIOR)
  {
    dc.SetBrush(*m_view->GetBackgroundBrush());

    // Add 1 because the pen is transparent. Under Motif, may be different.
#ifdef __WXMOTIF__
    dc.DrawRectangle(tabX, tabY, (GetWidth()+1), (GetHeight() + tabHeightInc));
#else
    dc.DrawRectangle(tabX, tabY, (GetWidth()+1), (GetHeight() + 1 + tabHeightInc));
#endif
  }

  // Draw highlight and shadow
  dc.SetPen(*m_view->GetHighlightPen());

  // Calculate the top of the tab beneath. It's the height of the tab, MINUS
  // a bit if the tab below happens to be selected. Check.
  wxTabControl *tabBeneath = NULL;
  int subtractThis = 0;
  if (GetColPosition() > 0)
    tabBeneath = m_view->FindTabControlForPosition(GetColPosition() - 1, GetRowPosition());
  if (tabBeneath && tabBeneath->IsSelected())
    subtractThis = (m_view->GetTabSelectionHeight() - m_view->GetTabHeight());

  // Vertical highlight: if first tab, draw to bottom of view
  if (tabX == m_view->GetViewRect().x && (m_view->GetTabStyle() & wxTAB_STYLE_DRAW_BOX))
    dc.DrawLine(tabX, tabY, tabX, (m_view->GetViewRect().y + m_view->GetViewRect().height));
  else if (tabX == m_view->GetViewRect().x)
    // Not box drawing, just to top of view.
    dc.DrawLine(tabX, tabY, tabX, (m_view->GetViewRect().y));
  else
    dc.DrawLine(tabX, tabY, tabX, (tabY + GetHeight() + tabHeightInc - subtractThis));

  dc.DrawLine(tabX, tabY, (tabX + GetWidth()), tabY);
  dc.SetPen(*m_view->GetShadowPen());

  // Test if we're outside the right-hand edge of the view area
  if (((tabX + GetWidth()) >= m_view->GetViewRect().x + m_view->GetViewRect().width) && (m_view->GetTabStyle() & wxTAB_STYLE_DRAW_BOX))
  {
    int bottomY = m_view->GetViewRect().y + m_view->GetViewRect().height + GetY() + m_view->GetTabHeight() + m_view->GetTopMargin();
    // Add a tab height since we wish to draw to the bottom of the view.
    dc.DrawLine((tabX + GetWidth()), tabY,
      (tabX + GetWidth()), bottomY);

    // Calculate the far-right of the view, since we don't wish to
    // draw inside that
    int rightOfView = m_view->GetViewRect().x + m_view->GetViewRect().width + 1;

    // Draw the horizontal bit to connect to the view rectangle
    dc.DrawLine((wxMax((tabX + GetWidth() - m_view->GetHorizontalTabOffset()), rightOfView)), (bottomY-1),
      (tabX + GetWidth()), (bottomY-1));

    // Draw black line to emphasize shadow
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine((tabX + GetWidth() + 1), (tabY+1),
      (tabX + GetWidth() + 1), bottomY);

    // Draw the horizontal bit to connect to the view rectangle
    dc.DrawLine((wxMax((tabX + GetWidth() - m_view->GetHorizontalTabOffset()), rightOfView)), (bottomY),
      (tabX + GetWidth() + 1), (bottomY));
  }
  else
  {
    if (lastInRow)
    {
      // 25/5/97 UNLESS it's less than the max number of positions in this row

      int topY = m_view->GetViewRect().y - m_view->GetTopMargin();

      int maxPositions = ((wxTabLayer *)m_view->GetLayers().Nth(0)->Data())->Number();

      // Only down to the bottom of the tab, not to the top of the view
      if ( GetRowPosition() < (maxPositions - 1) )
        topY = tabY + GetHeight() + tabHeightInc;

#ifdef __WXMOTIF__
      topY -= 1;
#endif

      // Shadow
      dc.DrawLine((tabX + GetWidth()), tabY, (tabX + GetWidth()), topY);
      // Draw black line to emphasize shadow
      dc.SetPen(*wxBLACK_PEN);
      dc.DrawLine((tabX + GetWidth() + 1), (tabY+1), (tabX + GetWidth() + 1),
         topY);
    }
    else
    {
      // Calculate the top of the tab beneath. It's the height of the tab, MINUS
      // a bit if the tab below (and next col along) happens to be selected. Check.
      wxTabControl *tabBeneath = NULL;
      int subtractThis = 0;
      if (GetColPosition() > 0)
        tabBeneath = m_view->FindTabControlForPosition(GetColPosition() - 1, GetRowPosition() + 1);
      if (tabBeneath && tabBeneath->IsSelected())
        subtractThis = (m_view->GetTabSelectionHeight() - m_view->GetTabHeight());

#ifdef __WXMOTIF__
      subtractThis += 1;
#endif

      // Draw only to next tab down.
      dc.DrawLine((tabX + GetWidth()), tabY,
         (tabX + GetWidth()), (tabY + GetHeight() + tabHeightInc - subtractThis));

      // Draw black line to emphasize shadow
      dc.SetPen(*wxBLACK_PEN);
      dc.DrawLine((tabX + GetWidth() + 1), (tabY+1), (tabX + GetWidth() + 1),
         (tabY + GetHeight() + tabHeightInc - subtractThis));
    }
  }

  // Draw centered text
  int textY = tabY + m_view->GetVerticalTabTextSpacing() + tabHeightInc;

  if (m_isSelected)
    dc.SetFont(* m_view->GetSelectedTabFont());
  else
    dc.SetFont(* GetFont());

  wxColour col(m_view->GetTextColour());
  dc.SetTextForeground(col);
  dc.SetBackgroundMode(wxTRANSPARENT);
  long textWidth, textHeight;
  dc.GetTextExtent(GetLabel(), &textWidth, &textHeight);

  int textX = (int)(tabX + (GetWidth() - textWidth)/2.0);
  if (textX < (tabX + 2))
    textX = (tabX + 2);

  dc.SetClippingRegion(tabX, tabY, GetWidth(), GetHeight());
  dc.DrawText(GetLabel(), textX, textY);
  dc.DestroyClippingRegion();

  if (m_isSelected)
  {
    dc.SetPen(*m_view->GetHighlightPen());

    // Draw white highlight from the tab's left side to the left hand edge of the view
    dc.DrawLine(m_view->GetViewRect().x, (tabY + GetHeight() + tabHeightInc),
     tabX, (tabY + GetHeight() + tabHeightInc));

    // Draw white highlight from the tab's right side to the right hand edge of the view
    dc.DrawLine((tabX + GetWidth()), (tabY + GetHeight() + tabHeightInc),
     m_view->GetViewRect().x + m_view->GetViewRect().width, (tabY + GetHeight() + tabHeightInc));
  }
#else
    // New HEL version with rounder tabs

    if (!m_view) return;

    int tabInc   = 0;
    if (m_isSelected)
    {
        tabInc = m_view->GetTabSelectionHeight() - m_view->GetTabHeight();
    }
    int tabLeft  = GetX() + m_view->GetViewRect().x;
    int tabTop   = GetY() + m_view->GetViewRect().y - tabInc;
    int tabRight = tabLeft + m_view->GetTabWidth();
    int left     = m_view->GetViewRect().x;
    int top      = tabTop + m_view->GetTabHeight() + tabInc;
    int right    = left + m_view->GetViewRect().width;
    int bottom   = top + m_view->GetViewRect().height;

    if (m_isSelected)
    {
        // TAB is selected - draw TAB and the View's full outline

        dc.SetPen(*(m_view->GetHighlightPen()));
        wxPoint pnts[10];
        int n = 0;
        pnts[n].x = left;            pnts[n++].y = bottom;
        pnts[n].x = left;             pnts[n++].y = top;
        pnts[n].x = tabLeft;         pnts[n++].y = top;
        pnts[n].x = tabLeft;            pnts[n++].y = tabTop + 2;
        pnts[n].x = tabLeft + 2;        pnts[n++].y = tabTop;
        pnts[n].x = tabRight - 1;    pnts[n++].y = tabTop;
        dc.DrawLines(n, pnts);
        if (!lastInRow)
        {
            dc.DrawLine(
                    (tabRight + 2),
                    top,
                    right,
                    top
                    );
        }

        dc.SetPen(*(m_view->GetShadowPen()));
        dc.DrawLine(
                tabRight,
                tabTop + 2,
                tabRight,
                top
                );
        dc.DrawLine(
                right,
                top,
                right,
                bottom
                );
        dc.DrawLine(
                right,
                bottom,
                left,
                bottom
                );

        dc.SetPen(*wxBLACK_PEN);
        dc.DrawPoint(
                tabRight,
                tabTop + 1
                );
        dc.DrawPoint(
                tabRight + 1,
                tabTop + 2
                );
        if (lastInRow)
        {
            dc.DrawLine(
                tabRight + 1,
                bottom,
                tabRight + 1,
                tabTop + 1
                );
        }
        else
        {
            dc.DrawLine(
                tabRight + 1,
                tabTop + 2,
                tabRight + 1,
                top
                );
            dc.DrawLine(
                right + 1,
                top,
                right + 1,
                bottom + 1
                );
        }
        dc.DrawLine(
                right + 1,
                bottom + 1,
                left + 1,
                bottom + 1
                );
    }
    else
    {
        // TAB is not selected - just draw TAB outline and RH edge
        // if the TAB is the last in the row

        int maxPositions = ((wxTabLayer*)m_view->GetLayers().Nth(0)->Data())->Number();
        wxTabControl* tabBelow = 0;
        wxTabControl* tabBelowRight = 0;
        if (GetColPosition() > 0)
        {
            tabBelow = m_view->FindTabControlForPosition(
                        GetColPosition() - 1,
                        GetRowPosition()
                        );
        }
        if (!lastInRow && GetColPosition() > 0)
        {
            tabBelowRight = m_view->FindTabControlForPosition(
                        GetColPosition() - 1,
                        GetRowPosition() + 1
                        );
        }

        float raisedTop = top - m_view->GetTabSelectionHeight() +
                            m_view->GetTabHeight();

        dc.SetPen(*(m_view->GetHighlightPen()));
        wxPoint pnts[10];
        int n = 0;

        pnts[n].x = tabLeft;

        if (tabBelow && tabBelow->IsSelected())
        {
            pnts[n++].y = (long)raisedTop;
        }
        else
        {
            pnts[n++].y = top;
        }
        pnts[n].x = tabLeft;            pnts[n++].y = tabTop + 2;
        pnts[n].x = tabLeft + 2;        pnts[n++].y = tabTop;
        pnts[n].x = tabRight - 1;    pnts[n++].y = tabTop;
        dc.DrawLines(n, pnts);

        dc.SetPen(*(m_view->GetShadowPen()));
        if (GetRowPosition() >= maxPositions - 1)
        {
            dc.DrawLine(
                    tabRight,
                    (tabTop + 2),
                    tabRight,
                    bottom
                    );
            dc.DrawLine(
                    tabRight,
                    bottom,
                    (tabRight - m_view->GetHorizontalTabOffset()),
                    bottom
                    );
        }
        else
        {
            if (tabBelowRight && tabBelowRight->IsSelected())
            {
                dc.DrawLine(
                        tabRight,
                        (long)raisedTop,
                        tabRight,
                        tabTop + 1
                        );
            }
            else
            {
                dc.DrawLine(
                        tabRight,
                        top - 1,
                        tabRight,
                        tabTop + 1
                        );
            }
        }

        dc.SetPen(*wxBLACK_PEN);
        dc.DrawPoint(
                tabRight,
                tabTop + 1
                );
        dc.DrawPoint(
                tabRight + 1,
                tabTop + 2
                );
        if (GetRowPosition() >= maxPositions - 1)
        {
            // draw right hand edge to bottom of view
            dc.DrawLine(
                    tabRight + 1,
                    bottom + 1,
                    tabRight + 1,
                    tabTop + 2
                    );
            dc.DrawLine(
                    tabRight + 1,
                    bottom + 1,
                    (tabRight - m_view->GetHorizontalTabOffset()),
                    bottom + 1
                    );
        }
        else
        {
            // draw right hand edge of TAB
            if (tabBelowRight && tabBelowRight->IsSelected())
            {
                dc.DrawLine(
                        tabRight + 1,
                        (long)(raisedTop - 1),
                        tabRight + 1,
                        tabTop + 2
                        );
            }
            else
            {
                dc.DrawLine(
                        tabRight + 1,
                        top - 1,
                        tabRight + 1,
                        tabTop + 2
                        );
            }
        }
    }

    // Draw centered text
    dc.SetPen(*wxBLACK_PEN);
    if (m_isSelected)
    {
        dc.SetFont(*(m_view->GetSelectedTabFont()));
    }
    else
    {
        dc.SetFont(*(GetFont()));
    }

    wxColour col(m_view->GetTextColour());
    dc.SetTextForeground(col);
    dc.SetBackgroundMode(wxTRANSPARENT);
    long textWidth, textHeight;
    dc.GetTextExtent(GetLabel(), &textWidth, &textHeight);

    float textX = (tabLeft + tabRight - textWidth) / 2;
    float textY = (tabInc + tabTop + m_view->GetVerticalTabTextSpacing());

    dc.DrawText(GetLabel(), (long)textX, (long)textY);
#endif
}

bool wxTabControl::HitTest(int x, int y) const
{
  // Top-left of tab control
  int tabX1 = GetX() + m_view->GetViewRect().x;
  int tabY1 = GetY() + m_view->GetViewRect().y;

  // Bottom-right
  int tabX2 = tabX1 + GetWidth();
  int tabY2 = tabY1 + GetHeight();

  if (x >= tabX1 && y >= tabY1 && x <= tabX2 && y <= tabY2)
    return TRUE;
  else
    return FALSE;
}

IMPLEMENT_DYNAMIC_CLASS(wxTabView, wxObject)

wxTabView::wxTabView(long style)
{
  m_noTabs = 0;
  m_tabStyle = style;
  m_tabSelection = -1;
  m_tabHeight = 20;
  m_tabSelectionHeight = m_tabHeight + 2;
  m_tabWidth = 80;
  m_tabHorizontalOffset = 10;
  m_tabHorizontalSpacing = 2;
  m_tabVerticalTextSpacing = 3;
  m_topMargin = 5;
  m_tabViewRect.x = 20;
  m_tabViewRect.y = 20;
  m_tabViewRect.width = 300;
  m_tabViewRect.x = 300;
  m_highlightColour = *wxWHITE;
  m_shadowColour = wxColour(128, 128, 128);
  m_backgroundColour = *wxLIGHT_GREY;
  m_textColour = *wxBLACK;
  m_highlightPen = wxWHITE_PEN;
  m_shadowPen = wxGREY_PEN;
  m_backgroundPen = wxLIGHT_GREY_PEN;
  m_backgroundBrush = wxLIGHT_GREY_BRUSH;
  m_tabFont = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);
  m_tabSelectedFont = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);
  m_window = (wxWindow *) NULL;
}

wxTabView::~wxTabView()
{
  ClearTabs(TRUE);
}

// Automatically positions tabs
// TODO: this should just add the tab to a list, and then
// a layout function (e.g. Realize) should be called when all tabs have been added.
// The view rect could easily change as the view window is resized.
wxTabControl *wxTabView::AddTab(int id, const wxString& label, wxTabControl *existingTab)
{
  // First, find which layer we should be adding to.
  wxNode *node = m_layers.Last();
  if (!node)
  {
    wxTabLayer *newLayer = new wxTabLayer;
    node = m_layers.Append(newLayer);
  }
  // Check if adding another tab control would go off the
  // right-hand edge of the layer.
  wxTabLayer *tabLayer = (wxTabLayer *)node->Data();
  wxNode *lastTabNode = tabLayer->Last();
  if (lastTabNode)
  {
    wxTabControl *lastTab = (wxTabControl *)lastTabNode->Data();
    // Start another layer (row).
    // Tricky choice: can't just check if will be overlapping the edge, because
    // this happens anyway for 2nd and subsequent rows.
    // Should check this for 1st row, and then subsequent rows should not exceed 1st
    // in length.
    if (((tabLayer == m_layers.First()->Data()) && ((lastTab->GetX() + 2*lastTab->GetWidth() + GetHorizontalTabSpacing())
              > GetViewRect().width)) ||
        ((tabLayer != m_layers.First()->Data()) && (tabLayer->Number() == ((wxTabLayer *)m_layers.First()->Data())->Number())))
    {
      tabLayer = new wxTabLayer;
      m_layers.Append(tabLayer);
      lastTabNode = (wxNode *) NULL;
    }
  }
  int layer = m_layers.Number() - 1;

  wxTabControl *tabControl = existingTab;
  if (!existingTab)
    tabControl = OnCreateTabControl();
  tabControl->SetRowPosition(tabLayer->Number());
  tabControl->SetColPosition(layer);

  wxTabControl *lastTab = (wxTabControl *) NULL;
  if (lastTabNode)
    lastTab = (wxTabControl *)lastTabNode->Data();

  // Top of new tab
  int verticalOffset = (- GetTopMargin()) - ((layer+1)*GetTabHeight());
  // Offset from view top-left
  int horizontalOffset = 0;
  if (!lastTab)
    horizontalOffset = layer*GetHorizontalTabOffset();
  else
    horizontalOffset = lastTab->GetX() + GetTabWidth() + GetHorizontalTabSpacing();

  tabControl->SetPosition(horizontalOffset, verticalOffset);
  tabControl->SetSize(GetTabWidth(), GetTabHeight());
  tabControl->SetId(id);
  tabControl->SetLabel(label);
  tabControl->SetFont(* GetTabFont());

  tabLayer->Append(tabControl);
  m_noTabs ++;

  return tabControl;
}

// Remove the tab without deleting the window
bool wxTabView::RemoveTab(int id)
{
  wxNode *layerNode = m_layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();
      if (tab->GetId() == id)
      {
        if (id == m_tabSelection)
          m_tabSelection = -1;
        delete tab;
        delete tabNode;
        m_noTabs --;

        // The layout has changed
        LayoutTabs();
        return TRUE;
      }
      tabNode = tabNode->Next();
    }
    layerNode = layerNode->Next();
  }
  return FALSE;
}

bool wxTabView::SetTabText(int id, const wxString& label)
{
    wxTabControl* control = FindTabControlForId(id);
    if (!control)
      return FALSE;
    control->SetLabel(label);
    return TRUE;
}

wxString wxTabView::GetTabText(int id) const
{
    wxTabControl* control = FindTabControlForId(id);
    if (!control)
      return wxEmptyString;
    else
      return control->GetLabel();
}

// Returns the total height of the tabs component -- this may be several
// times the height of a tab, if there are several tab layers (rows).
int wxTabView::GetTotalTabHeight()
{
  int minY = 0;

  wxNode *layerNode = m_layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();

      if (tab->GetY() < minY)
        minY = tab->GetY();

      tabNode = tabNode->Next();
    }
    layerNode = layerNode->Next();
  }

  return - minY;
}

void wxTabView::ClearTabs(bool deleteTabs)
{
  wxNode *layerNode = m_layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();
      if (deleteTabs)
        delete tab;
      wxNode *next = tabNode->Next();
      delete tabNode;
      tabNode = next;
    }
    wxNode *nextLayerNode = layerNode->Next();
    delete layer;
    delete layerNode;
    layerNode = nextLayerNode;
  }
  m_noTabs = 0;
  m_tabSelection = -1;
}


// Layout tabs (optional, e.g. if resizing window)
void wxTabView::LayoutTabs(void)
{
  // Make a list of the tab controls, deleting the wxTabLayers.
  wxList controls;

  wxNode *layerNode = m_layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();
      controls.Append(tab);
      wxNode *next = tabNode->Next();
      delete tabNode;
      tabNode = next;
    }
    wxNode *nextLayerNode = layerNode->Next();
    delete layer;
    delete layerNode;
    layerNode = nextLayerNode;
  }

  wxTabControl *lastTab = (wxTabControl *) NULL;

  wxTabLayer *currentLayer = new wxTabLayer;
  m_layers.Append(currentLayer);

  wxNode *node = controls.First();
  while (node)
  {
    wxTabControl *tabControl = (wxTabControl *)node->Data();
    if (lastTab)
    {
      // Start another layer (row).
      // Tricky choice: can't just check if will be overlapping the edge, because
      // this happens anyway for 2nd and subsequent rows.
      // Should check this for 1st row, and then subsequent rows should not exceed 1st
      // in length.
      if (((currentLayer == m_layers.First()->Data()) && ((lastTab->GetX() + 2*lastTab->GetWidth() + GetHorizontalTabSpacing())
                > GetViewRect().width)) ||
          ((currentLayer != m_layers.First()->Data()) && (currentLayer->Number() == ((wxTabLayer *)m_layers.First()->Data())->Number())))
     {
       currentLayer = new wxTabLayer;
       m_layers.Append(currentLayer);
       lastTab = (wxTabControl *) NULL;
     }
    }

    int layer = m_layers.Number() - 1;

    tabControl->SetRowPosition(currentLayer->Number());
    tabControl->SetColPosition(layer);

    // Top of new tab
    int verticalOffset = (- GetTopMargin()) - ((layer+1)*GetTabHeight());
    // Offset from view top-left
    int horizontalOffset = 0;
    if (!lastTab)
      horizontalOffset = layer*GetHorizontalTabOffset();
    else
      horizontalOffset = lastTab->GetX() + GetTabWidth() + GetHorizontalTabSpacing();

    tabControl->SetPosition(horizontalOffset, verticalOffset);
    tabControl->SetSize(GetTabWidth(), GetTabHeight());

    currentLayer->Append(tabControl);
    lastTab = tabControl;

    node = node->Next();
  }

  // Move the selected tab to the bottom
  wxTabControl *control = FindTabControlForId(m_tabSelection);
  if (control)
    MoveSelectionTab(control);

}

// Draw all tabs
void wxTabView::Draw(wxDC& dc)
{
        // Don't draw anything if there are no tabs.
        if (GetNumberOfTabs() == 0)
          return;

    // Draw top margin area (beneath tabs and above view area)
    if (GetTabStyle() & wxTAB_STYLE_COLOUR_INTERIOR)
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*GetBackgroundBrush());

        // Add 1 because the pen is transparent. Under Motif, may be different.
        dc.DrawRectangle(
                m_tabViewRect.x,
                (m_tabViewRect.y - m_topMargin),
                (m_tabViewRect.width + 1),
                (m_topMargin + 1)
                );
    }

    // Draw layers in reverse order
    wxNode *node = m_layers.Last();
    while (node)
    {
        wxTabLayer *layer = (wxTabLayer *)node->Data();
        wxNode *node2 = layer->First();
        while (node2)
        {
            wxTabControl *control = (wxTabControl *)node2->Data();
            control->OnDraw(dc, (node2->Next() == NULL));
            node2 = node2->Next();
        }

        node = node->Previous();
    }


#ifndef wxUSE_NEW_METHOD
    if (GetTabStyle() & wxTAB_STYLE_DRAW_BOX)
    {
        dc.SetPen(* GetShadowPen());

        // Draw bottom line
        dc.DrawLine(
                (GetViewRect().x + 1),
                (GetViewRect().y + GetViewRect().height),
                (GetViewRect().x + GetViewRect().width + 1),
                (GetViewRect().y + GetViewRect().height)
                );

        // Draw right line
        dc.DrawLine(
                (GetViewRect().x + GetViewRect().width),
                (GetViewRect().y - GetTopMargin() + 1),
                (GetViewRect().x + GetViewRect().width),
                (GetViewRect().y + GetViewRect().height)
                );

        dc.SetPen(* wxBLACK_PEN);

        // Draw bottom line
        dc.DrawLine(
                (GetViewRect().x),
                (GetViewRect().y + GetViewRect().height + 1),
#if defined(__WXMOTIF__)
                (GetViewRect().x + GetViewRect().width + 1),
#else
                (GetViewRect().x + GetViewRect().width + 2),
#endif

                (GetViewRect().y + GetViewRect().height + 1)
                );

        // Draw right line
        dc.DrawLine(
                (GetViewRect().x + GetViewRect().width + 1),
                (GetViewRect().y - GetTopMargin()),
                (GetViewRect().x + GetViewRect().width + 1),
                (GetViewRect().y + GetViewRect().height + 1)
                );
    }
#endif
}

// Process mouse event, return FALSE if we didn't process it
bool wxTabView::OnEvent(wxMouseEvent& event)
{
  if (!event.LeftDown())
    return FALSE;

  wxCoord x, y;
  event.GetPosition(&x, &y);

  wxTabControl *hitControl = (wxTabControl *) NULL;

  wxNode *node = m_layers.First();
  while (node)
  {
    wxTabLayer *layer = (wxTabLayer *)node->Data();
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *control = (wxTabControl *)node2->Data();
      if (control->HitTest((int)x, (int)y))
      {
        hitControl = control;
        node = (wxNode *) NULL;
        node2 = (wxNode *) NULL;
      }
      else
        node2 = node2->Next();
    }

    if (node)
      node = node->Next();
  }

  if (!hitControl)
    return FALSE;

  wxTabControl *currentTab = FindTabControlForId(m_tabSelection);

  if (hitControl == currentTab)
    return FALSE;

  ChangeTab(hitControl);

  return TRUE;
}

bool wxTabView::ChangeTab(wxTabControl *control)
{
  wxTabControl *currentTab = FindTabControlForId(m_tabSelection);
  int oldTab = -1;
  if (currentTab)
    oldTab = currentTab->GetId();

  if (control == currentTab)
    return TRUE;

  if (m_layers.Number() == 0)
    return FALSE;

  if (!OnTabPreActivate(control->GetId(), oldTab))
    return FALSE;

  // Move the tab to the bottom
  MoveSelectionTab(control);

  if (currentTab)
    currentTab->SetSelected(FALSE);

  control->SetSelected(TRUE);
  m_tabSelection = control->GetId();

  OnTabActivate(control->GetId(), oldTab);

  // Leave window refresh for the implementing window

  return TRUE;
}

// Move the selected tab to the bottom layer, if necessary,
// without calling app activation code
bool wxTabView::MoveSelectionTab(wxTabControl *control)
{
  if (m_layers.Number() == 0)
    return FALSE;

  wxTabLayer *firstLayer = (wxTabLayer *)m_layers.First()->Data();

  // Find what column this tab is at, so we can swap with the one at the bottom.
  // If we're on the bottom layer, then no need to swap.
  if (!firstLayer->Member(control))
  {
    // Do a swap
    int col = 0;
    wxNode *thisNode = FindTabNodeAndColumn(control, &col);
    if (!thisNode)
      return FALSE;
    wxNode *otherNode = firstLayer->Nth(col);
    if (!otherNode)
      return FALSE;

    // If this is already in the bottom layer, return now
    if (otherNode == thisNode)
      return TRUE;

    wxTabControl *otherTab = (wxTabControl *)otherNode->Data();

    // We now have pointers to the tab to be changed to,
    // and the tab on the first layer. Swap tab structures and
    // position details.

    int thisX = control->GetX();
    int thisY = control->GetY();
    int thisColPos = control->GetColPosition();
    int otherX = otherTab->GetX();
    int otherY = otherTab->GetY();
    int otherColPos = otherTab->GetColPosition();

    control->SetPosition(otherX, otherY);
    control->SetColPosition(otherColPos);
    otherTab->SetPosition(thisX, thisY);
    otherTab->SetColPosition(thisColPos);

    // Swap the data for the nodes
    thisNode->SetData(otherTab);
    otherNode->SetData(control);
  }
  return TRUE;
}

// Called when a tab is activated
void wxTabView::OnTabActivate(int /*activateId*/, int /*deactivateId*/)
{
}

void wxTabView::SetHighlightColour(const wxColour& col)
{
  m_highlightColour = col;
  m_highlightPen = wxThePenList->FindOrCreatePen(col, 1, wxSOLID);
}

void wxTabView::SetShadowColour(const wxColour& col)
{
  m_shadowColour = col;
  m_shadowPen = wxThePenList->FindOrCreatePen(col, 1, wxSOLID);
}

void wxTabView::SetBackgroundColour(const wxColour& col)
{
  m_backgroundColour = col;
  m_backgroundPen = wxThePenList->FindOrCreatePen(col, 1, wxSOLID);
  m_backgroundBrush = wxTheBrushList->FindOrCreateBrush(col, wxSOLID);
}

void wxTabView::SetTabSelection(int sel, bool activateTool)
{
  if ( sel==m_tabSelection )
    return;

  int oldSel = m_tabSelection;
  wxTabControl *control = FindTabControlForId(sel);
  wxTabControl *oldControl = FindTabControlForId(m_tabSelection);

  if (!OnTabPreActivate(sel, oldSel))
    return;

  if (control)
    control->SetSelected((sel != -1)); // TODO ??
  else if (sel != -1)
  {
    wxFAIL_MSG(_("Could not find tab for id"));
    return;
  }

  if (oldControl)
    oldControl->SetSelected(FALSE);

  m_tabSelection = sel;

  if (control)
    MoveSelectionTab(control);

  if (activateTool)
    OnTabActivate(sel, oldSel);
}

// Find tab control for id
wxTabControl *wxTabView::FindTabControlForId(int id) const
{
  wxNode *node1 = m_layers.First();
  while (node1)
  {
    wxTabLayer *layer = (wxTabLayer *)node1->Data();
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *control = (wxTabControl *)node2->Data();
      if (control->GetId() == id)
        return control;
      node2 = node2->Next();
    }
    node1 = node1->Next();
  }
  return (wxTabControl *) NULL;
}

// Find tab control for layer, position (starting from zero)
wxTabControl *wxTabView::FindTabControlForPosition(int layer, int position) const
{
  wxNode *node1 = m_layers.Nth(layer);
  if (!node1)
    return (wxTabControl *) NULL;
  wxTabLayer *tabLayer = (wxTabLayer *)node1->Data();
  wxNode *node2 = tabLayer->Nth(position);
  if (!node2)
    return (wxTabControl *) NULL;
  return (wxTabControl *)node2->Data();
}

// Find the node and the column at which this control is positioned.
wxNode *wxTabView::FindTabNodeAndColumn(wxTabControl *control, int *col) const
{
  wxNode *node1 = m_layers.First();
  while (node1)
  {
    wxTabLayer *layer = (wxTabLayer *)node1->Data();
    int c = 0;
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *cnt = (wxTabControl *)node2->Data();
      if (cnt == control)
      {
        *col = c;
        return node2;
      }
      node2 = node2->Next();
      c ++;
    }
    node1 = node1->Next();
  }
  return (wxNode *) NULL;
}

int wxTabView::CalculateTabWidth(int noTabs, bool adjustView)
{
  m_tabWidth = (int)((m_tabViewRect.width - ((noTabs - 1)*GetHorizontalTabSpacing()))/noTabs);
  if (adjustView)
  {
    m_tabViewRect.width = noTabs*m_tabWidth + ((noTabs-1)*GetHorizontalTabSpacing());
  }
  return m_tabWidth;
}

/*
 * wxTabbedDialog
 */

IMPLEMENT_CLASS(wxTabbedDialog, wxDialog)

BEGIN_EVENT_TABLE(wxTabbedDialog, wxDialog)
    EVT_CLOSE(wxTabbedDialog::OnCloseWindow)
    EVT_MOUSE_EVENTS(wxTabbedDialog::OnMouseEvent)
    EVT_PAINT(wxTabbedDialog::OnPaint)
END_EVENT_TABLE()

wxTabbedDialog::wxTabbedDialog(wxWindow *parent, wxWindowID id,
    const wxString& title,
    const wxPoint& pos, const wxSize& size,
    long windowStyle, const wxString& name):
   wxDialog(parent, id, title, pos, size, windowStyle, name)
{
  m_tabView = (wxTabView *) NULL;
}

wxTabbedDialog::~wxTabbedDialog(void)
{
  if (m_tabView)
    delete m_tabView;
}

void wxTabbedDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event) )
{
  Destroy();
}

void wxTabbedDialog::OnMouseEvent(wxMouseEvent& event )
{
  if (m_tabView)
    m_tabView->OnEvent(event);
}

void wxTabbedDialog::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if (m_tabView)
        m_tabView->Draw(dc);
}

/*
 * wxTabbedPanel
 */

IMPLEMENT_CLASS(wxTabbedPanel, wxPanel)

BEGIN_EVENT_TABLE(wxTabbedPanel, wxPanel)
    EVT_MOUSE_EVENTS(wxTabbedPanel::OnMouseEvent)
    EVT_PAINT(wxTabbedPanel::OnPaint)
END_EVENT_TABLE()

wxTabbedPanel::wxTabbedPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos,
   const wxSize& size, long windowStyle, const wxString& name):
   wxPanel(parent, id, pos, size, windowStyle, name)
{
  m_tabView = (wxTabView *) NULL;
}

wxTabbedPanel::~wxTabbedPanel(void)
{
  delete m_tabView;
}

void wxTabbedPanel::OnMouseEvent(wxMouseEvent& event)
{
  if (m_tabView)
    m_tabView->OnEvent(event);
}

void wxTabbedPanel::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if (m_tabView)
        m_tabView->Draw(dc);
}

/*
 * wxPanelTabView
 */

IMPLEMENT_CLASS(wxPanelTabView, wxTabView)

wxPanelTabView::wxPanelTabView(wxPanel *pan, long style): wxTabView(style), m_tabWindows(wxKEY_INTEGER)
{
  m_panel = pan;
  m_currentWindow = (wxWindow *) NULL;

  if (m_panel->IsKindOf(CLASSINFO(wxTabbedDialog)))
    ((wxTabbedDialog *)m_panel)->SetTabView(this);
  else if (m_panel->IsKindOf(CLASSINFO(wxTabbedPanel)))
    ((wxTabbedPanel *)m_panel)->SetTabView(this);

  SetWindow(m_panel);
}

wxPanelTabView::~wxPanelTabView(void)
{
  ClearWindows(TRUE);
}

// Called when a tab is activated
void wxPanelTabView::OnTabActivate(int activateId, int deactivateId)
{
  if (!m_panel)
    return;

  wxWindow *oldWindow = ((deactivateId == -1) ? 0 : GetTabWindow(deactivateId));
  wxWindow *newWindow = GetTabWindow(activateId);

  if (oldWindow)
    oldWindow->Show(FALSE);
  if (newWindow)
    newWindow->Show(TRUE);

  m_panel->Refresh();
}


void wxPanelTabView::AddTabWindow(int id, wxWindow *window)
{
  m_tabWindows.Append((long)id, window);
  window->Show(FALSE);
}

wxWindow *wxPanelTabView::GetTabWindow(int id) const
{
  wxNode *node = m_tabWindows.Find((long)id);
  if (!node)
    return (wxWindow *) NULL;
  return (wxWindow *)node->Data();
}

void wxPanelTabView::ClearWindows(bool deleteWindows)
{
  if (deleteWindows)
    m_tabWindows.DeleteContents(TRUE);
  m_tabWindows.Clear();
  m_tabWindows.DeleteContents(FALSE);
}

void wxPanelTabView::ShowWindowForTab(int id)
{
  wxWindow *newWindow = GetTabWindow(id);
  if (newWindow == m_currentWindow)
    return;
  if (m_currentWindow)
    m_currentWindow->Show(FALSE);
  newWindow->Show(TRUE);
  newWindow->Refresh();
}

#endif // wxUSE_TAB_DIALOG
