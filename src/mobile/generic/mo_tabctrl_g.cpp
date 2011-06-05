/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/tabctrl.cpp
// Purpose:     wxMoTabCtrl class
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

#include "wx/mobile/generic/tabctrl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

#if !wxUSE_TAB_DIALOG
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxMoTabCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoTabCtrl, wxControl)
    EVT_PAINT(wxMoTabCtrl::OnPaint)
    EVT_MOUSE_EVENTS(wxMoTabCtrl::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxMoTabCtrl::OnSysColourChanged)
    EVT_SIZE(wxMoTabCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxMoTabCtrl::OnEraseBackground)
END_EVENT_TABLE()

wxMoTabCtrl::wxMoTabCtrl()
{
    Init();
}

bool wxMoTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;
    
    if ((style & (wxTAB_TEXT|wxTAB_BITMAP)) == 0)
        style |= wxTAB_TEXT|wxTAB_BITMAP;

    wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name);
    
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    
    wxFont font = wxMoSystemSettings::GetFont(wxMO_FONT_TAB);
    SetFont(font);

#if 0
    if ((style & wxTAB_TEXT) && (style & wxTAB_BITMAP))
        m_buttonStrip.SetFaceType(wxMoButtonStrip::TextAndBitmapButton);
    else if (style & wxTAB_TEXT) 
        m_buttonStrip.SetFaceType(wxMoButtonStrip::TextButton);
    else
        m_buttonStrip.SetFaceType(wxMoButtonStrip::BitmapButton);
#endif

    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TAB_BG));
    SetButtonBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TAB_BUTTON_BG));
   
    SetInitialSize(size);
    
    return true;
}

wxMoTabCtrl::~wxMoTabCtrl()
{
}

void wxMoTabCtrl::Init()
{
    m_buttonStrip.SetControlType(wxMoButtonStrip::TabBar);
    m_buttonStrip.SetLayoutStyle(wxMoButtonStrip::LayoutEvenButtons);
    m_buttonStrip.SetSelectionStyle(wxMoButtonStrip::SingleSelection);    
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxMoTabCtrl::SendCommand(wxEventType eventType, int selection)
{
    wxTabEvent event(eventType, m_windowId);

    event.SetEventObject( this );
    event.SetEventType(eventType);
    event.SetSelection(selection);

    if (ProcessEvent(event))
    {
        if (!event.IsAllowed())
            return false;
    }
    
    return true;
}

// Responds to colour changes, and passes event on to children.
void wxMoTabCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // m_backgroundColour = wxSystemSettings::GetColour(wxSYS_WINDOW_BACKGROUND);

    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

// Delete all items
bool wxMoTabCtrl::DeleteAllItems()
{
    m_buttonStrip.Clear();
    Refresh();
    return true;
}

// Delete an item
bool wxMoTabCtrl::DeleteItem(int item)
{
    bool success = m_buttonStrip.DeleteButton(item);
    
    m_buttonStrip.ComputeSizes(this);

    InvalidateBestSize();

    return success;
}

// Get the selection
int wxMoTabCtrl::GetSelection() const
{
    return m_buttonStrip.GetSelection();
}

// Get the tab with the current keyboard focus
int wxMoTabCtrl::GetCurFocus() const
{
    // TODO
    return -1;
}

// Get the number of items
int wxMoTabCtrl::GetItemCount() const
{
    return m_buttonStrip.GetCount();
}

// Get the rect corresponding to the tab
bool wxMoTabCtrl::GetItemRect(int WXUNUSED(item), wxRect& WXUNUSED(rect)) const
{
    // TODO
    return false;
}

// Get the number of rows
int wxMoTabCtrl::GetRowCount() const
{
    return 1;
}

// Get the item text
wxString wxMoTabCtrl::GetItemText(int idx) const
{
    wxMoBarButton* item = m_buttonStrip.GetButton(idx);
    if (item)
        return item->GetLabel();
    else
        return wxEmptyString;
}

// Get the item image
int wxMoTabCtrl::GetItemImage(int idx) const
{
    wxMoBarButton* item = m_buttonStrip.GetButton(idx);
    if (item)
        return item->GetImageId();
    else
        return -1;
}

#if 0
// Get the item data
void* wxMoTabCtrl::GetItemData(int WXUNUSED(idx)) const
{
    // TODO
    return NULL;
}

// Set the data for an item
bool wxMoTabCtrl::SetItemData(int WXUNUSED(item), void* WXUNUSED(data))
{
    // TODO
    return false;
}

#endif

// Hit test
int wxMoTabCtrl::HitTest(const wxPoint& WXUNUSED(pt), long& WXUNUSED(flags))
{
    // TODO
    return 0;
}

// Insert an item
bool wxMoTabCtrl::InsertItem(int item, const wxString& text, int imageId)
{
    int idx = m_buttonStrip.InsertBitmapButton(this, 0, item, imageId, text);
    OnInsertItem(m_buttonStrip.GetButton(idx));

    m_buttonStrip.ComputeSizes(this);

    InvalidateBestSize();

    return true;
}

// Insert an item, passing a bitmap.
bool wxMoTabCtrl::InsertItem(int item, const wxString& text, const wxBitmap& bitmap)
{
    int idx = m_buttonStrip.InsertBitmapButton(this, 0, item, bitmap, text);
    OnInsertItem(m_buttonStrip.GetButton(idx));
    m_buttonStrip.ComputeSizes(this);

    InvalidateBestSize();

    return true;
}

// Add an item
bool wxMoTabCtrl::AddItem(const wxString& text, int imageId)
{
    int idx;
    if (imageId == -1)
        idx = m_buttonStrip.AddTextButton(this, 0, text);
    else
        idx = m_buttonStrip.AddBitmapButton(this, 0, imageId, text);

    OnInsertItem(m_buttonStrip.GetButton(idx));

    m_buttonStrip.ComputeSizes(this);

    InvalidateBestSize();

    return true;
}

// Add an item, passing a bitmap.
bool wxMoTabCtrl::AddItem(const wxString& text, const wxBitmap& bitmap)
{
    int idx = m_buttonStrip.AddBitmapButton(this, 0, bitmap, text);
    OnInsertItem(m_buttonStrip.GetButton(idx));

    m_buttonStrip.ComputeSizes(this);

    InvalidateBestSize();

    return true;
}

void wxMoTabCtrl::OnInsertItem(wxMoBarButton* button)
{
    button->SetStyle(wxBBU_PLAIN);
    button->SetFont(GetFont());
    button->SetBackgroundColour(m_buttonStrip.GetButtonBackgroundColour());
    button->SetSelectionColour(m_buttonStrip.GetButtonSelectionColour());
    button->SetBorderColour(m_buttonStrip.GetBorderColour());
}

// Set the selection
int wxMoTabCtrl::SetSelection(int item)
{
    // Return index of previously selected item
    int oldSel = GetSelection();
    if (item != oldSel)
    {
        if (SendCommand(wxEVT_COMMAND_TAB_SEL_CHANGING, item))
        {
            if (m_buttonStrip.SetSelection(item))
            {
                Refresh();
                SendCommand(wxEVT_COMMAND_TAB_SEL_CHANGED, item);
            }
        }
    }

    return oldSel;
}

// Set the selection, without generating events
int wxMoTabCtrl::ChangeSelection(int item)
{
    int oldSel = GetSelection();
    if (item != oldSel)
    {
        if (m_buttonStrip.SetSelection(item))
            Refresh();
    }

    return oldSel;
}

// Set the image list
void wxMoTabCtrl::SetImageList(wxImageList* imageList)
{
    m_buttonStrip.SetImageList(imageList);
}

// Assign (own) the image list
void wxMoTabCtrl::AssignImageList(wxImageList* imageList)
{
    m_buttonStrip.AssignImageList(imageList);
}

// Set the text for an item
bool wxMoTabCtrl::SetItemText(int idx, const wxString& text)
{
    wxMoBarButton* item = m_buttonStrip.GetButton(idx);
    if (item)
    {
        item->SetLabel(text);
        return true;
    }
    else
        return false;
}

// Set the image for an item
bool wxMoTabCtrl::SetItemImage(int idx, int image)
{
    wxMoBarButton* item = m_buttonStrip.GetButton(idx);
    if (item)
    {
        if (image > wxID_LOWEST && image < wxID_EXTENDED_HIGHEST)
        {
            wxBitmap bitmap = wxMoBarButton::GetStockBitmap(image, wxMO_TABBUTTON);
            wxString label = wxMoBarButton::GetStockLabel(image);
            if (bitmap.IsOk())
            {
                item->GetNormalBitmap() = bitmap;
                item->SetImageId(image);
                if (!label.IsEmpty())
                    item->SetLabel(label);
                return true;
            }
        }
        else
        {
            if (GetImageList())
            {
                wxBitmap bitmap = GetImageList()->GetBitmap(image);
                item->GetNormalBitmap() = bitmap;
                item->SetImageId(image);
                return true;
            }
        }
    }
    return false;
}

// Set the size for a fixed-width tab control
void wxMoTabCtrl::SetItemSize(const wxSize& WXUNUSED(size))
{
    // TODO
}

// Set the padding between tabs
void wxMoTabCtrl::SetPadding(const wxSize& WXUNUSED(padding))
{
    // TODO
}

wxSize wxMoTabCtrl::DoGetBestSize() const
{
    // Note that a tab bar is typically about 49 pixels high.

    wxSize sz = m_buttonStrip.GetBestSize();
    CacheBestSize(sz);
    return sz;
}

void wxMoTabCtrl::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect rect = wxRect(wxPoint(0, 0), GetSize()); 
    int pressed = m_buttonStrip.ProcessMouseEvent(this, rect, event);
    if (pressed != -1 && pressed != GetSelection())
    {
        SetSelection(pressed);
    }
}

void wxMoTabCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxRect rect(wxPoint(0, 0), GetSize());
    m_buttonStrip.Draw(this, rect, dc);
}

void wxMoTabCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoTabCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    m_buttonStrip.ComputeSizes(this);
    Refresh();
}

void wxMoTabCtrl::SetBorderColour(const wxColour &colour)
{
    m_buttonStrip.SetBorderColour(colour);
    Refresh();
}

wxColour wxMoTabCtrl::GetBorderColour() const
{
    return m_buttonStrip.GetBorderColour();
}

bool wxMoTabCtrl::SetBackgroundColour(const wxColour &colour)
{
    wxControl::SetBackgroundColour(colour);
    m_buttonStrip.SetBackgroundColour(colour);
    Refresh();

    return true;
}

wxColour wxMoTabCtrl::GetBackgroundColour() const
{
    return m_buttonStrip.GetBackgroundColour();
}

bool wxMoTabCtrl::SetButtonBackgroundColour(const wxColour &colour)
{
    m_buttonStrip.SetButtonBackgroundColour(colour);
    m_buttonStrip.SetButtonSelectionColour(colour);

    return true;
}

wxColour wxMoTabCtrl::GetButtonBackgroundColour() const
{
    return m_buttonStrip.GetButtonBackgroundColour();
}

bool wxMoTabCtrl::SetForegroundColour(const wxColour &colour)
{
    wxControl::SetForegroundColour(colour);
    m_buttonStrip.SetLabelColour(colour);
    Refresh();

    return true;
}

wxColour wxMoTabCtrl::GetForegroundColour() const
{
    return GetForegroundColour();
}

bool wxMoTabCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);
    m_buttonStrip.SetLabelFont(font);
    InvalidateBestSize();
    Refresh();

    return true;
}

wxFont wxMoTabCtrl::GetFont() const
{
    return m_buttonStrip.GetLabelFont();
}

// Set a text badge for the given item
bool wxMoTabCtrl::SetBadge(int item, const wxString& badge)
{
    wxMoBarButton* button = m_buttonStrip.GetButton(item);
    if (button)
    {
        button->SetBadge(badge);
        return true;
    }
    else
        return false;
}

// Get the text badge for the given item
wxString wxMoTabCtrl::GetBadge(int item) const
{
    wxMoBarButton* button = m_buttonStrip.GetButton(item);
    if (button)
        return button->GetBadge();
    else
        return wxEmptyString;
}


