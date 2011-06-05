/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_navbar_g.cpp
// Purpose:     wxMoNavigationBar class
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

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/navbar.h"
#include "wx/mobile/generic/utils.h"

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationBarNameStr[] = wxT("NavigationBar");

IMPLEMENT_DYNAMIC_CLASS(wxMoNavigationBar, wxControl)

BEGIN_EVENT_TABLE(wxMoNavigationBar, wxControl)
    EVT_PAINT(wxMoNavigationBar::OnPaint)
    EVT_SIZE(wxMoNavigationBar::OnSize)
    EVT_MOUSE_EVENTS(wxMoNavigationBar::OnMouseEvent)
    EVT_BUTTON(wxID_BACK, wxMoNavigationBar::OnBack)
    EVT_ERASE_BACKGROUND(wxMoNavigationBar::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNavigationBarEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHING)

bool wxMoNavigationBar::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style|wxBORDER_NONE, validator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    wxColour buttonColour;
    if (style & wxNAVBAR_BLACK_OPAQUE_BG)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_BLACK_OPAQUE_BG));
        buttonColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_BLACK_BUTTON_BG);
    }
    else if (style & wxNAVBAR_BLACK_TRANSLUCENT_BG)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_BLACK_TRANSLUCENT_BG));
        buttonColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_BLACK_BUTTON_BG);
    }
    else    
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_NORMAL_BG));
        buttonColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_NORMAL_BUTTON_BG);
    }

    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_TEXT));
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NAVBAR_BUTTON));

    m_backButton.SetMarginX(10);
    m_backButton.SetStyle(wxBBU_BORDERED|wxBBU_BACK);
    m_backButton.SetId(wxID_BACK);
    m_backButton.SetBackgroundColour(buttonColour);

    SetInitialSize(size);

    return true;
}

wxMoNavigationBar::~wxMoNavigationBar()
{
    ClearItems();
}

void wxMoNavigationBar::Init()
{
    m_buttonMarginX = 10;
    m_buttonMarginY = 8;
    m_buttonMarginInterItemY = 4;
    m_endMargin = 4;
    m_interButtonSpacing = 2;
}

wxSize wxMoNavigationBar::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(wxMoNavigationBar *, this));
    dc.SetFont(GetFont());

    // Pixels between labels and edges
    // int spacing = 5;

    // TODO
    int width = 100;
    int height = 44; // normal navigation bar height
    
    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

bool wxMoNavigationBar::SendClickEvent(wxMoBarButton* button)
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
    event.SetEventObject(button);

    if (!button->ProcessEvent(event))
        return GetEventHandler()->ProcessEvent(event);
    else
        return true;
}

bool wxMoNavigationBar::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }
    Refresh();
    return true;
}

bool wxMoNavigationBar::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();
    return true;
}

bool wxMoNavigationBar::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoNavigationBar::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);
    Refresh();

    return true;
}

void wxMoNavigationBar::OnBack(wxCommandEvent& WXUNUSED(event))
{
    wxMoNavigationItem* backItem = GetBackItem();
    if (backItem)
    {
        wxMoNavigationItem* oldItem = PopItem();
        if (oldItem)
        {
            delete oldItem;
        }
    }
}

void wxMoNavigationBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    DrawBackground(dc);
    DrawItems(dc);
}

void wxMoNavigationBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoNavigationBar::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect winRect(GetRect());

    wxRect rect1, rect2;
    if (GetLeftButton())
        rect1 = GetLeftButton()->GetRect();
    if (GetRightButton())
        rect2 = GetRightButton()->GetRect();

    if (GetLeftButton() && m_buttonPressDetector.ProcessMouseEvent(this, rect1, event, GetLeftButton()->GetId()))
    {
        SendClickEvent(GetLeftButton());
        Refresh();
    }
    else if (GetRightButton() && m_buttonPressDetector.ProcessMouseEvent(this, rect2, event, GetLeftButton()->GetId()))
    {
        SendClickEvent(GetRightButton());
        Refresh();
    }
    else if ((event.LeftUp() || event.Leaving()) && (wxWindow::GetCapture() == this))
    {
        m_buttonPressDetector.SetButtonState(0);
        m_buttonPressDetector.SetButtonId(0);
        ReleaseMouse();
    }
}

void wxMoNavigationBar::OnSize(wxSizeEvent& WXUNUSED(event))
{
    PositionButtons();
    Refresh();
}

// Pushes an item onto the stack.
bool wxMoNavigationBar::PushItem(wxMoNavigationItem* item)
{
    {
        wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_PUSHING,
            GetId(), GetTopItem(), item);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent))
        {
            if (!navEvent.IsAllowed())
                return false;
        }
    }

    m_items.Add(item);            
    PositionButtons();

    {
        wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_PUSHED,
            GetId(), GetTopItem(), item);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxMoNavigationItem* wxMoNavigationBar::PopItem()
{
    if (m_items.GetCount() > 0)
    {
        wxMoNavigationItem* backItem = GetBackItem();
        wxMoNavigationItem* topItem = GetTopItem();
        if (topItem && backItem)
        {
            {
                wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_POPPING,
                    GetId(), GetTopItem(), backItem);
                navEvent.SetEventObject(this);
                navEvent.Allow();
                if (GetEventHandler()->ProcessEvent(navEvent))
                {
                    if (!navEvent.IsAllowed())
                        return NULL;
                }
            }
            
            m_items.RemoveAt(m_items.GetCount()-1);
            PositionButtons();
            
            {
                wxNavigationBarEvent navEvent(wxEVT_COMMAND_NAVBAR_POPPED,
                    GetId(), topItem, backItem);
                navEvent.SetEventObject(this);
                GetEventHandler()->ProcessEvent(navEvent);
            }
            
            return topItem;
        }
    }
    return NULL;
}

// Returns the top item.
wxMoNavigationItem* wxMoNavigationBar::GetTopItem() const
{
    if (m_items.GetCount() > 0)
    {
        wxMoNavigationItem* item = m_items[m_items.GetCount()-1];
        return item;
    }
    else
        return NULL;
}

// Returns the back item.
wxMoNavigationItem* wxMoNavigationBar::GetBackItem() const
{
    if (m_items.GetCount() > 1)
    {
        wxMoNavigationItem* item = m_items[m_items.GetCount()-2];
        return item;
    }
    else
        return NULL;
}

// Sets the item stack.
void wxMoNavigationBar::SetItems(const wxMoNavigationItemArray& items)
{
    // TODO: delete old items?

    m_items = items;

    Refresh();
}

// Clears the item stack, deleting the items.
void wxMoNavigationBar::ClearItems(bool deleteItems)
{
    size_t i;
    size_t count = m_items.GetCount();
    for (i = 0; i < count; i++)
    {
        wxMoNavigationItem* item = m_items[0];
        m_items.RemoveAt(0);

        if (deleteItems)
            delete item;
    }
}

wxMoBarButton* wxMoNavigationBar::GetLeftButton()
{
    wxMoNavigationItem* item = GetTopItem();
    if (item)
    {
        if (item->GetLeftButton())
            return item->GetLeftButton();
        else if (GetBackItem())
        {
            if (GetBackItem()->GetBackButton())
                return GetBackItem()->GetBackButton();
            else
            {
                return & m_backButton;
            }
        }
    }
    return NULL;
}

wxMoBarButton* wxMoNavigationBar::GetRightButton()
{
    wxMoNavigationItem* item = GetTopItem();
    if (item)
    {
        if (item->GetRightButton())
            return item->GetRightButton();
    }
    return NULL;
}

wxString wxMoNavigationBar::GetCurrentTitle()
{
    wxMoNavigationItem* item = GetTopItem();
    if (item)
        return item->GetTitle();
    return wxEmptyString;

}

void wxMoNavigationBar::PositionButtons()
{
    wxMoBarButton* leftButton = GetLeftButton();
    wxMoBarButton* rightButton = GetRightButton();
    if (leftButton == & m_backButton)
    {
        if (GetBackItem())
            m_backButton.SetLabel(GetBackItem()->GetTitle());
    }

    wxScreenDC dc;
    wxSize barSize = GetClientSize();

    if (leftButton)
    {
        leftButton->SetSize(GetBestButtonSize(dc, * leftButton));
        leftButton->SetPosition(wxPoint(GetEndMargin(), (barSize.y - leftButton->GetSize().y)/2));
    }

    if (rightButton)
    {
        rightButton->SetSize(GetBestButtonSize(dc, * rightButton));
        rightButton->SetPosition(wxPoint(barSize.x - rightButton->GetSize().x - GetEndMargin(), (barSize.y - rightButton->GetSize().y)/2));
    }

    Refresh();
}

// Gets the best size for this button
wxSize wxMoNavigationBar::GetBestButtonSize(wxDC& dc, const wxMoBarButton& item) const
{
    wxSize sz = item.GetBestButtonSize(dc);
    sz.y = 30;
    return sz;
}

// Draw the title
void wxMoNavigationBar::DrawTitle(wxDC& dc)
{
    wxString title = GetCurrentTitle();
    if (!title.IsEmpty())
    {
        dc.SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NAVBAR_TITLE));
        
        int w, h;
        dc.GetTextExtent(title, & w, & h);

        dc.SetBackgroundMode(wxTRANSPARENT);

        wxSize sz = GetClientSize();

        int x = (sz.x - w)/2;
        int y = (sz.y - h)/2;

        int buttonRight = 0;
        if (GetLeftButton())
            buttonRight = GetLeftButton()->GetRect().GetRight();
        x = wxMax(buttonRight+6, x);

        dc.SetTextForeground(wxMoSystemSettings::GetColour(wxMO_COLOUR_GREY_SHADOW));
        dc.DrawText(title, x, y-2);

        dc.SetTextForeground(GetForegroundColour());
        dc.DrawText(title, x, y);
    }
}

// Draw all items
void wxMoNavigationBar::DrawItems(wxDC& dc)
{
    if (GetLeftButton())
        DrawButton(dc, * GetLeftButton());

    if (GetRightButton())
        DrawButton(dc, * GetRightButton());

    DrawTitle(dc);
}

void wxMoNavigationBar::DrawButton(wxDC& dc, wxMoBarButton& item)
{
    int buttonFlags = wxMO_BUTTON_TWO_TONE|wxMO_BUTTON_NO_BACKGROUND;

    if (item.GetHighlighted() ||
        (m_buttonPressDetector.GetButtonState() == wxCTRL_STATE_HIGHLIGHTED && item.GetId() == m_buttonPressDetector.GetButtonId()))
    {
        buttonFlags |= wxMO_BUTTON_HIGHLIGHTED;
    }

    item.Draw(dc, buttonFlags);
}

void wxMoNavigationBar::DrawBackground(wxDC& dc)
{
    wxRect rect(wxPoint(0, 0), GetClientSize());
    wxMoRenderer::DrawButtonBackground(this, dc, wxColour(), GetBackgroundColour(), wxColour(), rect, wxCTRL_STATE_NORMAL,
        wxMO_BUTTON_NO_BORDER|wxMO_BUTTON_TWO_TONE);

    dc.SetPen(wxPen(wxMoSystemSettings::GetColour(wxMO_COLOUR_GREY_SHADOW)));
    dc.DrawLine(0, 0, rect.width, 0);

    dc.SetPen(*wxBLACK);
    dc.DrawLine(0, rect.height-1, rect.width, rect.height-1);
}

int wxMoNavigationBar::HitTest(const wxPoint& pt, wxMoBarButton** button)
{
    * button = NULL;

    if (GetLeftButton() && GetLeftButton()->GetRect().Contains(pt))
    {
        * button = GetLeftButton();
        return 1;
    }

    if (GetRightButton() && GetRightButton()->GetRect().Contains(pt))
    {
        * button = GetRightButton();
        return 1;
    }

    if (GetRect().Contains(pt))
        return 0;

    return -1;
}
