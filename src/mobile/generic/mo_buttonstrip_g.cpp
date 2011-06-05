/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_buttonstrip_g.cpp
// Purpose:     wxMoButtonStrip helper class for tab control,
//              toolbar and segmented control.
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/mobile/generic/buttonstrip.h"
#include "wx/mobile/generic/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoButtonStrip, wxObject)

#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

wxMoButtonStrip::~wxMoButtonStrip()
{
    if (m_ownsImageList)
        delete m_imageList;
    m_imageList = NULL;
}

void wxMoButtonStrip::Init()
{
    m_controlType = TabBar;
    m_faceType = TextButton;
    m_selectionStyle = NoSelection;
    m_layoutStyle = LayoutEvenButtons;

    m_buttonMarginX = 2;
    m_buttonMarginY = 2;
    m_buttonMarginInterItemY = 2;
    m_endMargin = 2;
    m_interButtonSpacing = 2;
    m_labelFont = wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON);
    m_labelColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_TEXT);
    m_buttonBackgroundColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BG);
    m_backgroundColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_NORMAL_BG);
    m_borderColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_BUTTON_BORDER);
    m_buttonSelectionColour = m_buttonBackgroundColour;

    m_imageList = NULL;
    m_ownsImageList = false;

    m_selection = -1;

    m_buttonState = 0;
    m_buttonForState = -1;
}

void wxMoButtonStrip::Draw(wxWindow* win, const wxRect& WXUNUSED(rect), wxDC& dc)
{
    DrawBackground(win, dc);

    size_t i;
    for (i = 0; i < GetCount(); i++)
    {
        wxMoBarButton& item = m_buttonStrip[i];
        if (item.GetId() != wxID_FLEXIBLESPACE && item.GetId() != wxID_FIXEDSPACE)
            DrawButton(win, dc, item, (int) i);
    }
}

void wxMoButtonStrip::DrawButton(wxWindow* win, wxDC& dc, wxMoBarButton& item, int i)
{
    int buttonFlags = wxMO_BUTTON_TWO_TONE;

    wxRect rect(item.GetRect());
    if (GetControlType() == SegmentedCtrl)
    {
        rect.y = m_endMargin;
        rect.height = win->GetSize().y - 2*m_endMargin;

        if (i < (int) (GetCount() -1))
            buttonFlags |= wxMO_BUTTON_STRAIGHT_RIGHT_EDGE;
        if (i > 0)
            buttonFlags |= wxMO_BUTTON_STRAIGHT_LEFT_EDGE;
    }
    else if (GetControlType() == ToolBar || GetControlType() == TabBar)
    {
        // Don't attempt to draw the control background for each button,
        // since we draw on top of a toolbar or tab background with no rounded edges
        buttonFlags |= wxMO_BUTTON_NO_BACKGROUND;
    }

    if (item.GetHighlighted() ||
        (m_buttonState == wxCTRL_STATE_HIGHLIGHTED && i == m_buttonForState))
    {
        buttonFlags |= wxMO_BUTTON_HIGHLIGHTED;
    }
    else if (item.GetSelected() || (m_selection == i))
    {
        buttonFlags |= wxMO_BUTTON_SELECTED;
    }

    item.Draw(dc, buttonFlags);
}

void wxMoButtonStrip::DrawBackground(wxWindow* win, wxDC& dc)
{
    if (GetControlType() == TabBar || GetControlType() == ToolBar)
    {
        wxRect rect(wxPoint(0, 0), win->GetSize());
        wxMoRenderer::DrawButtonBackground(win, dc, wxColour(), GetBackgroundColour(), wxColour(), rect, wxCTRL_STATE_NORMAL,
            wxMO_BUTTON_TWO_TONE|wxMO_BUTTON_NO_BORDER);
    }
    else if (GetControlType() == SegmentedCtrl)
    {
        dc.SetBackground(win->GetParent()->GetBackgroundColour());
        dc.Clear();
    }
}

int wxMoButtonStrip::HitTest(const wxPoint& pt) const
{
    size_t i;
    for (i = 0; i < m_buttonStrip.GetCount(); i++)
    {
        if (m_buttonStrip[i].GetRect().Contains(pt))
            return (int) i;
    }
    return -1;
}

int wxMoButtonStrip::ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    int hit = HitTest(event.GetPosition());

    bool shouldRefresh = false;

    if (event.LeftUp())
    {
        if (wxWindow::GetCapture() == win)
        {
            win->ReleaseMouse();
        }
        if (m_buttonState == wxCTRL_STATE_HIGHLIGHTED)
        {
            int pressed = m_buttonForState;
            m_buttonState = wxCTRL_STATE_NORMAL;
            m_buttonForState = -1;

            win->Refresh();
            return pressed;
        }
    }
    else if (event.LeftIsDown() && rect.Contains(pt))
    {
        if (wxWindow::GetCapture() != win)
        {
            win->CaptureMouse();
        }

        if (m_buttonState != wxCTRL_STATE_HIGHLIGHTED && hit != -1)
        {
            m_buttonState = wxCTRL_STATE_HIGHLIGHTED;
            m_buttonForState = hit;
            shouldRefresh = true;
        }
    }
    else if (event.LeftIsDown() && !rect.Contains(pt))
    {
        if (m_buttonState == wxCTRL_STATE_HIGHLIGHTED)
        {
            m_buttonState = wxCTRL_STATE_NORMAL;
            m_buttonForState = -1;
            shouldRefresh = true;
        }
    }
    else if (event.Leaving())
    {
        m_buttonState = wxCTRL_STATE_NORMAL;
        m_buttonForState = -1;
        if (wxWindow::GetCapture() == win)
        {
            win->ReleaseMouse();
        }
    }

    if (shouldRefresh)
        win->Refresh();

    return -1;
}

int wxMoButtonStrip::AddTextButton(wxWindow* parent, int id, const wxString& text, const wxString& badge)
{
    return InsertTextButton(parent, id, -1, text, badge);
}

int wxMoButtonStrip::AddBitmapButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxString& text, const wxString& badge)
{
    return InsertBitmapButton(parent, id, -1, bitmap, text, badge);
}

int wxMoButtonStrip::AddBitmapButton(wxWindow* parent, int id, int imageIndex, const wxString& text, const wxString& badge)
{
    return InsertBitmapButton(parent, id, -1, imageIndex, text, badge);
}

int wxMoButtonStrip::AddFlexibleSeparator()
{
    return InsertFlexibleSeparator(-1);
}

int wxMoButtonStrip::AddFixedSeparator(int width)
{
    return InsertFixedSeparator(-1, width);
}

int wxMoButtonStrip::InsertTextButton(wxWindow* parent, int id, int insertBefore, const wxString& text, const wxString& badge)
{
    wxMoBarButton item;
    item.SetLabel(text);
    item.SetBadge(badge);
    item.SetId(id);
    item.SetMarginX(GetButtonMarginX());
    item.SetMarginY(GetButtonMarginY());
    item.SetParent(parent);

    int style = 0;

    // Make the assumption that text-only toolbar buttons have borders
    if (GetControlType() == ToolBar)
        style |= wxBBU_BORDERED;

    if (id == wxID_DONE)
        style |= wxBBU_DONE;

    item.SetStyle(style);

    if (insertBefore == -1)
    {
        m_buttonStrip.Add(item);
        return (int) (m_buttonStrip.GetCount()-1);
    }
    else
    {
        m_buttonStrip.Insert(item, insertBefore);
        return insertBefore;
    }
}

int wxMoButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, const wxBitmap& bitmap1, const wxString& text1, const wxString& badge)
{
    wxBitmap bitmap(bitmap1);
    wxString text(text1);

    int barType = (GetControlType() == TabBar) ? wxMO_TABBUTTON : wxMO_BARBUTTON;

    if (!bitmap.IsOk() && wxMoBarButton::IsStockId(id, barType))
    {
        bitmap = wxMoBarButton::GetStockBitmap(id, barType);
        if (text.IsEmpty())
            text = wxMoBarButton::GetStockLabel(id);
    }

    wxMoBarButton item;
    item.GetNormalBitmap() = bitmap;
    item.SetLabel(text);
    item.SetBadge(badge);
    item.SetId(id);
    item.SetMarginX(GetButtonMarginX());
    item.SetMarginY(GetButtonMarginY());
    item.SetParent(parent);

    int style = 0;
    if (!bitmap.IsOk())
    {
        // Make the assumption that text-only toolbar buttons have borders
        if (GetControlType() == ToolBar)
            style |= wxBBU_BORDERED;
        
        if (id == wxID_DONE)
            style |= wxBBU_DONE;
    }
    item.SetStyle(style);

    if (insertBefore == -1)
    {
        m_buttonStrip.Add(item);
        return (int) (m_buttonStrip.GetCount()-1);
    }
    else
    {
        m_buttonStrip.Insert(item, insertBefore);
        return insertBefore;
    }
}

int wxMoButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, int imageIndex, const wxString& text, const wxString& badge)
{
    int barType = (GetControlType() == TabBar) ? wxMO_TABBUTTON : wxMO_BARBUTTON;

    wxBitmap bitmap;

    wxString label(text);

    // Can use either the id or imageIndex to indicate a system image.

    int barId = -1;
    if (imageIndex > wxID_LOWEST && imageIndex < wxID_EXTENDED_HIGHEST && wxMoBarButton::IsStockId(imageIndex, barType))
        barId = imageIndex;
    else if (id > wxID_LOWEST && id < wxID_EXTENDED_HIGHEST && wxMoBarButton::IsStockId(id, barType))
        barId = id;

    if (barId != -1)
    {
        bitmap = wxMoBarButton::GetStockBitmap(barId, barType);
        wxString stockLabel = wxMoBarButton::GetStockLabel(barId);
        if (!stockLabel.IsEmpty() && label.IsEmpty())
            label = stockLabel;
    }
    else
    {
        wxASSERT(GetImageList() != NULL);

        if (GetImageList())
            bitmap = GetImageList()->GetBitmap(imageIndex);
    }

    if (!bitmap.IsOk())
        return -1;

    wxMoBarButton item;
    item.GetNormalBitmap() = bitmap;
    item.SetLabel(label);
    item.SetBadge(badge);
    item.SetId(id);
    item.SetImageId(imageIndex);
    item.SetMarginX(GetButtonMarginX());
    item.SetMarginY(GetButtonMarginY());
    item.SetParent(parent);
    
    if (insertBefore == -1)
    {
        m_buttonStrip.Add(item);
        return (int) (m_buttonStrip.GetCount()-1);
    }
    else
    {
        m_buttonStrip.Insert(item, insertBefore);
        return insertBefore;
    }
}

int wxMoButtonStrip::InsertFlexibleSeparator(int insertBefore)
{
    wxMoBarButton item;
    item.SetId(wxID_FLEXIBLESPACE);
    item.SetSize(wxSize(0, 10));
    
    if (insertBefore == -1)
    {
        m_buttonStrip.Add(item);
        return (int) (m_buttonStrip.GetCount()-1);
    }
    else
    {
        m_buttonStrip.Insert(item, insertBefore);
        return insertBefore;
    }
}

int wxMoButtonStrip::InsertFixedSeparator(int insertBefore, int width)
{
    wxMoBarButton item;
    item.SetId(wxID_FIXEDSPACE);
    item.SetSize(wxSize(width, 10));
    
    if (insertBefore == -1)
    {
        m_buttonStrip.Add(item);
        return (int) (m_buttonStrip.GetCount()-1);
    }
    else
    {
        m_buttonStrip.Insert(item, insertBefore);
        return insertBefore;
    }
}

bool wxMoButtonStrip::DeleteButton(size_t idx)
{
    wxASSERT(idx >= 0 && idx < m_buttonStrip.GetCount());

    if (idx >= 0 && idx < m_buttonStrip.GetCount())
    {
        m_buttonStrip.RemoveAt(idx);
        return true;
    }
    else
        return false;
   
}

bool wxMoButtonStrip::SetButtonText(size_t idx, const wxString& text)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->SetLabel(text);
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::SetButtonBadge(size_t idx, const wxString& badge)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->SetBadge(badge);
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::SetButtonNormalBitmap(size_t idx, const wxBitmap& bitmap)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->GetNormalBitmap() = bitmap;
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::SetButtonDisabledBitmap(size_t idx, const wxBitmap& bitmap)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->GetDisabledBitmap() = bitmap;
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::SetButtonSelectedBitmap(size_t idx, const wxBitmap& bitmap)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->GetSelectedBitmap() = bitmap;
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::SetButtonHighlightedBitmap(size_t idx, const wxBitmap& bitmap)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->GetHighlightedBitmap() = bitmap;
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::EnableButton(size_t idx, bool enable)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->SetEnabled(enable);
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::IsButtonEnabled(size_t idx) const
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        return item->GetEnabled();
    }
    else
        return false;
}

bool wxMoButtonStrip::SelectButton(size_t idx, bool selected)
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        item->SetSelected(selected);
        return true;
    }
    else
        return false;
}

bool wxMoButtonStrip::IsButtonSelected(size_t idx) const
{
    wxMoBarButton* item = GetButton(idx);
    if (item)
    {
        return item->GetSelected();
    }
    else
        return false;
}

wxMoBarButton* wxMoButtonStrip::GetButton(size_t idx) const
{
    wxASSERT(idx >= 0 && idx < m_buttonStrip.GetCount());

    if (idx >= 0 && idx < m_buttonStrip.GetCount())
    {
        return & m_buttonStrip[idx];
    }
    else
        return false;
}

int wxMoButtonStrip::FindIndexForId(int id)
{
    size_t i;
    for (i = 0; i < m_buttonStrip.GetCount(); i++)
    {
        if (m_buttonStrip[i].GetId() == id)
            return (int) i;
    }
    return -1;
}

void wxMoButtonStrip::Clear()
{
    m_buttonStrip.Clear();
}

bool wxMoButtonStrip::SetSelection(int sel)
{
    if (m_selection != sel)
    {
        m_selection = sel;
        return true;
    }
    else
        return false;
}

wxSize wxMoButtonStrip::GetBestButtonSize(wxDC& dc, const wxMoBarButton& item) const
{
    return item.GetBestButtonSize(dc);
}

wxSize wxMoButtonStrip::GetBestSize() const
{
    wxSize sz(0, 0);

    wxScreenDC dc;

    wxSize maxButtonSize, totalIncludingMargins;
    wxSize totalButtonSize = GetTotalButtonSize(maxButtonSize, totalIncludingMargins);

    if (m_layoutStyle == LayoutEvenButtons || m_layoutStyle == LayoutStretchingSeparators)
    {
        sz.x = totalButtonSize.x + ((GetCount()-1) * m_interButtonSpacing);
        sz.y = maxButtonSize.y;
    }
    else if (m_layoutStyle == LayoutEvenSeparators)
    {
        sz.x = (maxButtonSize.x * GetCount()) + ((GetCount()-1) * m_interButtonSpacing);
        sz.y = maxButtonSize.y;
    }
    
    // Left and right margins between button and control edge
    sz.x += 2*m_endMargin;

    // Top and bottom margins between button and control edge
    sz.y += 2*m_endMargin;

    return sz;
}

// Gets the space taking up by just the buttons.
wxSize wxMoButtonStrip::GetTotalButtonSize(wxSize& maxButtonSize, wxSize& totalIncludingMargins) const
{
    totalIncludingMargins = wxSize(0, 0);
    wxSize sz(0, 0);
    maxButtonSize = sz;

    wxScreenDC dc;

    size_t i;
    for (i = 0; i < GetCount(); i++)
    {
        const wxMoBarButton& item = m_buttonStrip[i];

        wxSize buttonSize = GetBestButtonSize(dc, item);

        if (buttonSize.y > sz.y)
            sz.y = buttonSize.y;

        if (buttonSize.x > maxButtonSize.x)
            maxButtonSize.x = buttonSize.x;

        if (buttonSize.y > maxButtonSize.y)
            maxButtonSize.y = buttonSize.y;

        sz.x += buttonSize.x;
        totalIncludingMargins.x += buttonSize.x;
        if (item.GetId() != wxID_FLEXIBLESPACE && item.GetId() != wxID_FIXEDSPACE)
            totalIncludingMargins.x += m_interButtonSpacing;
    }

    totalIncludingMargins.y += 2*GetButtonMarginY();
    totalIncludingMargins.x += 2*GetEndMargin();

    return sz;
}

bool wxMoButtonStrip::ComputeSizes(wxWindow* win)
{
    wxSize bestSize = GetBestSize();
    wxSize maxButtonSize(0, 0);
    wxSize totalButtonSizeIncludingMargins;
    wxSize totalButtonSize = GetTotalButtonSize(maxButtonSize, totalButtonSizeIncludingMargins);

    wxSize windowSize = win->GetClientSize();

    int interButtonSpacing = m_interButtonSpacing;

    if (m_layoutStyle == LayoutEvenButtons || m_layoutStyle == LayoutEvenSeparators)
    {
        // Does simple layout, stretching the buttons or separators
        if (GetCount() > 1)
        {
            if (m_layoutStyle == LayoutEvenButtons)
            {
                totalButtonSize.x = GetCount() * maxButtonSize.x;

                // Distribute the space between all buttons
                int widthForButtons = windowSize.x - 2*m_endMargin - (GetCount()-1)*m_interButtonSpacing;
                int newButtonWidth = wxMax(0, widthForButtons/(int) GetCount());
                if (newButtonWidth > maxButtonSize.x)
                    maxButtonSize.x = newButtonWidth;
            }
            else if (m_layoutStyle == LayoutEvenSeparators)
            {
                // Distribute the space between all button spacers
                int widthLeftOver = windowSize.x - totalButtonSize.x - 2*m_endMargin;
                if (widthLeftOver > 0)
                    interButtonSpacing = widthLeftOver/(GetCount()-1);
            }
        }
        
        int currentX = m_endMargin;
        int currentY = (windowSize.y - totalButtonSize.y)/2;
        
        wxScreenDC dc;
        size_t i;
        for (i = 0; i < GetCount(); i++)
        {
            wxMoBarButton& item = m_buttonStrip[i];
            
            wxSize buttonSize = GetBestButtonSize(dc, item);
            
            if (m_layoutStyle == LayoutEvenSeparators)
                item.GetRect() = wxRect(currentX, currentY, buttonSize.x, totalButtonSize.y);
            else
                item.GetRect() = wxRect(currentX, currentY, maxButtonSize.x, totalButtonSize.y);
            
            currentX += item.GetRect().width;
            currentX += interButtonSpacing;
        }
    }
    else if (m_layoutStyle == LayoutStretchingSeparators)
    {
        // Does toolbar-style layout, taking into account stretching
        // or fixed separators
        int remainingSpace =  windowSize.x - totalButtonSizeIncludingMargins.x;

        // Count the number of flexible separators
        int flexibleSeparatorCount = 0;
        size_t i;
        for (i = 0; i < GetCount(); i++)
        {
            wxMoBarButton& item = m_buttonStrip[i];
            if (item.GetId() == wxID_FLEXIBLESPACE)
            {
                flexibleSeparatorCount ++;
            }
        }

        int spacingPerFlexibleSeparator = 0;
        if (flexibleSeparatorCount > 0 && remainingSpace > 0)
            spacingPerFlexibleSeparator = remainingSpace/flexibleSeparatorCount;

        int currentX = m_endMargin;
        
        wxScreenDC dc;
        for (i = 0; i < GetCount(); i++)
        {
            wxMoBarButton& item = m_buttonStrip[i];
            if (item.GetId() == wxID_FLEXIBLESPACE)
            {
                wxSize buttonSize = item.GetSize();
                
                int currentY = (windowSize.y - buttonSize.y)/2;
                item.GetRect() = wxRect(currentX, currentY, spacingPerFlexibleSeparator, buttonSize.y);
                
                currentX += item.GetRect().width;
            }
            else
            {           
                wxSize buttonSize = item.GetSize();
                wxSize bestButtonSize = GetBestButtonSize(dc, item);
                if (buttonSize.y <= 0)
                    buttonSize.y = bestButtonSize.y;
                if (buttonSize.x <= 0)
                    buttonSize.x = bestButtonSize.x;
                
                int currentY = (windowSize.y - buttonSize.y)/2;
                item.GetRect() = wxRect(currentX, currentY, buttonSize.x, buttonSize.y);
                
                currentX += item.GetRect().width;

                if (item.GetId() != wxID_FIXEDSPACE)
                    currentX += interButtonSpacing;
            }
        }
    }
        
    return true;
}

void wxMoButtonStrip::DoResize(wxWindow* win)
{
    ComputeSizes(win);
    win->Refresh();
}
