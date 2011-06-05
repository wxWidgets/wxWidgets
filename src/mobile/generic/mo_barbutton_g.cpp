/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_barbutton_g.cpp
// Purpose:     wxMoBarButton class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/generic/barbutton.h"
#include "wx/mobile/generic/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(wxMoBarButtonArray);


#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoBarButton, wxEvtHandler)

BEGIN_EVENT_TABLE(wxMoBarButton, wxEvtHandler)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxMoBarButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& label1,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& WXUNUSED(validator),
                      const wxString& WXUNUSED(name))
{
    wxString label(label1);
    if (label.empty() && IsStockId(id, wxMO_BARBUTTON))
    {
        label = GetStockLabel(id);
    }

    SetStyle(style);
    SetLabel(label);
    SetParent(parent);
    m_rect = wxRect(pos, size);

    if (style & wxBBU_BORDERED)
    {
        // TODO: find out how to calculate the correct margin around the button
        m_marginX = 16;
    }

    if (style & wxBBU_DONE)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_DONE_BUTTON_BG));
        SetSelectionColour(GetBackgroundColour());
    }

    //SetInitialSize(size);

    return true;
}

bool wxMoBarButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap1,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& WXUNUSED(validator),
                      const wxString& WXUNUSED(name))
{
    wxBitmap bitmap(bitmap1);
    if (!bitmap.IsOk() && IsStockId(id, wxMO_BARBUTTON))
    {
        bitmap = GetStockBitmap(id, wxMO_BARBUTTON);
    }

    SetStyle(style);
    SetNormalBitmap(bitmap);
    SetParent(parent);
    m_rect = wxRect(pos, size);

    if (style & wxBBU_BORDERED)
    {
        // TODO: find out how to calculate the correct margin around the button
        m_marginX = 16;
    }

    if (style & wxBBU_DONE)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_DONE_BUTTON_BG));
        SetSelectionColour(GetBackgroundColour());
    }

    //SetInitialSize(size);

    return true;
}

wxMoBarButton::~wxMoBarButton()
{
}

void wxMoBarButton::Init()
{
    m_id = -1;
    m_enabled = true;
    m_selected = false;
    m_highlighted = false;
    m_imageId = -1;
    m_style = 0;
    m_parent = NULL;
    m_marginX = 2;
    m_marginY = 2;

    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BG));
    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_TEXT));
    SetBorderColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BORDER));
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));
}

void wxMoBarButton::Copy(const wxMoBarButton& item)
{
    m_normalBitmap = item.m_normalBitmap;
    m_disabledBitmap = item.m_disabledBitmap;
    m_selectedBitmap = item.m_selectedBitmap;
    m_highlightedBitmap = item.m_highlightedBitmap;
    m_rect = item.m_rect;
    m_label = item.m_label;
    m_badge = item.m_badge;
    m_id = item.m_id;
    m_imageId = item.m_imageId;
    m_enabled = item.m_enabled;
    m_selected = item.m_selected;
    m_highlighted = item.m_highlighted;
    m_style = item.m_style;

    m_borderColour = item.m_borderColour;
    m_backgroundColour = item.m_backgroundColour;
    m_foregroundColour = item.m_foregroundColour;
    m_selectionColour = item.m_selectionColour;
    m_borderColour = item.m_borderColour;

    m_font = item.m_font;
    m_parent = item.m_parent;
    m_marginX = item.m_marginX;
    m_marginY = item.m_marginY;
}

wxSize wxMoBarButton::DoGetBestSize() const
{
    if (GetParent())
    {
        wxClientDC dc((wxWindow*) GetParent());
        return GetBestButtonSize(dc);
    }
    else
    {
        wxScreenDC dc;
        return GetBestButtonSize(dc);
    }
}

wxSize wxMoBarButton::GetBestButtonSize(wxDC& dc) const
{
    wxSize sz(10, 10);

    if (GetId() == wxID_FIXEDSPACE)
    {
        return GetSize();
    }
    else if (GetId() == wxID_FLEXIBLESPACE)
    {
        return wxSize(0, 0);
    }

    wxCoord labelWidth = 0, labelHeight = 0;
    if (!GetLabel().IsEmpty())
    {
        dc.SetFont(GetFont());
        dc.GetTextExtent(GetLabel(), & labelWidth, & labelHeight);
    }

    if (GetNormalBitmap().IsOk() && !GetLabel().IsEmpty())
    {
        sz.x = labelWidth + 2 * GetMarginX();
        sz.y = labelHeight + 2 * GetMarginY();
        
        sz.x = wxMax(sz.x, GetNormalBitmap().GetWidth() + 2 * GetMarginX());
        sz.y = sz.y + GetNormalBitmap().GetHeight() + GetMarginY();
    }
    else if (GetNormalBitmap().IsOk() && GetLabel().IsEmpty())
    {
        sz.x = GetNormalBitmap().GetWidth() + 2 * GetMarginX();
        sz.y = GetNormalBitmap().GetHeight() + 2 * GetMarginY();
    }
    else if (!GetLabel().IsEmpty())
    {
        sz.x = labelWidth + 2 * GetMarginX();
        sz.y = labelHeight + 2 * GetMarginY();
    }

    if (GetStyle() + wxBBU_BACK)
        sz.x += (wxMO_BACK_BUTTON_ARROW_SIZE - GetMarginX());

    wxSize actualSize = sz;
    if (GetSize().x > 0)
        actualSize.x = GetSize().x;
    if (GetSize().y > 0)
        actualSize.y = GetSize().y;

    return actualSize;
}

wxSize wxMoBarButton::IPGetDefaultSize() const
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;
        dc.SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

        // the size of a standard button in the dialog units is 50x14,
        // translate this to pixels
        // NB1: the multipliers come from the Windows convention
        // NB2: the extra +1/+2 were needed to get the size be the same as the
        //      size of the buttons in the standard dialog - I don't know how
        //      this happens, but on my system this size is 75x23 in pixels and
        //      23*8 isn't even divisible by 14... Would be nice to understand
        //      why these constants are needed though!
        s_sizeBtn.x = (50 * (dc.GetCharWidth() + 1))/4;
        s_sizeBtn.y = ((14 * dc.GetCharHeight()) + 2)/8;
    }

    return s_sizeBtn;
}

bool wxMoBarButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);

    bool processed = ProcessEvent(event);
    if (!processed && GetParent())
        processed = GetParent()->ProcessEvent(event);
    return processed;
}

void wxMoBarButton::Draw(wxDC& dc, int flags)
{
    wxRect rect(GetRect());

    int state = 0;
    if (!GetEnabled())
        state = wxCTRL_STATE_DISABLED;
    else if (GetHighlighted() ||
        (flags & wxMO_BUTTON_HIGHLIGHTED))
        state = wxCTRL_STATE_HIGHLIGHTED;
    else if (GetSelected() || (flags & wxMO_BUTTON_SELECTED))
        state = wxCTRL_STATE_SELECTED;
    else
        state = wxCTRL_STATE_NORMAL;

    if (GetStyle() & wxBBU_BACK)
        flags |= wxMO_BUTTON_BACK;

    bool drawButtonBackground = false;

    if ((GetStyle() & (wxBBU_BORDERED|wxBBU_DONE)) ||
        (state & (wxCTRL_STATE_SELECTED|wxCTRL_STATE_HIGHLIGHTED)))
        drawButtonBackground = true;

    if (drawButtonBackground)
    {
        wxColour backgroundColour =  *wxBLACK;
        if (GetParent())
            backgroundColour = GetParent()->GetBackgroundColour();

        wxColour buttonBackgroundColour(GetBackgroundColour());
        if (state == wxCTRL_STATE_SELECTED && GetSelectionColour().IsOk())
            buttonBackgroundColour = GetSelectionColour();
        wxMoRenderer::DrawButtonBackground(GetParent(), dc, backgroundColour, buttonBackgroundColour, m_borderColour, rect, state, flags);
    }

    wxCoord labelWidth = 0, labelHeight = 0;
    if (!GetLabel().IsEmpty())
    {
        dc.SetFont(GetFont());
        dc.GetTextExtent(GetLabel(), & labelWidth, & labelHeight);
    }

    if (GetNormalBitmap().Ok())
    {
        wxBitmap bitmap;
        
        if (state & wxCTRL_STATE_HIGHLIGHTED)
            bitmap = GetHighlightedBitmap();
        else if (state & wxCTRL_STATE_SELECTED)
            bitmap = GetSelectedBitmap();

        if (!bitmap.IsOk())
            bitmap = GetNormalBitmap();

        if (bitmap.IsOk())
        {
            int bitmapX = rect.x + (rect.GetWidth() - bitmap.GetWidth())/2;
            int bitmapY = rect.y + (rect.GetHeight() - bitmap.GetHeight())/2;

            if (!GetLabel().IsEmpty())
                bitmapY = rect.y + GetMarginY();

            dc.DrawBitmap(bitmap, bitmapX, bitmapY, true);
        }
    }
    if (!GetLabel().IsEmpty())
    {
        wxCoord w, h;
        dc.SetFont(m_font);
        dc.GetTextExtent(GetLabel(), & w, & h);
        dc.SetTextForeground(m_foregroundColour);
        
        int textX = rect.x + (rect.GetWidth() - w)/2;
        int textY = rect.y + (rect.GetHeight() - h)/2;
        
        if (GetNormalBitmap().IsOk())
            textY = rect.y + rect.height - GetMarginY() - h;
        
        if (GetStyle() & wxBBU_BACK)
            textX += wxMO_BACK_BUTTON_ARROW_SIZE/2;
        
        dc.DrawText(GetLabel(), textX, textY);
    }
    if (!GetBadge().IsEmpty())
    {
        wxCoord w, h;
        dc.SetFont(m_font);
        dc.GetTextExtent(GetBadge(), & w, & h);

        wxColour badgeColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TAB_BADGE_BG));
        dc.SetBrush(wxBrush(badgeColour));
        dc.SetPen(wxPen(m_foregroundColour));
        dc.SetTextForeground(m_foregroundColour);

        int marginX = 2;

        int dimension = wxMax(w, h);

        int radius = int(0.05 + double(dimension)/2.0) + marginX;
        radius = wxMax(5, radius);
        radius = wxMin(12, radius);
        dc.DrawCircle(rect.x + rect.GetWidth() - radius, rect.y + radius, radius);
        
        int textX = rect.x + rect.GetWidth() - 2*radius + (2*radius - w)/2;
        int textY = rect.y + radius - (h/2);
        
        dc.DrawText(GetBadge(), textX, textY);
    }
}

void wxMoBarButton::ProcessMouseEvent(wxMouseEvent& WXUNUSED(event))
{
    // Processed by the particular class using the bar button
}

// function called when any of the bitmaps changes
void wxMoBarButton::OnSetLabel()
{
}

// Bar buttons

#include "wx/mobile/generic/bitmaps/stock_action_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_add_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_bookmarks_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_camera_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_compose_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_fastforward_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_organize_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_pause_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_play_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_refresh_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_reply_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_rewind_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_search_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_stop_20x20.inc"
#include "wx/mobile/generic/bitmaps/stock_trash_20x20.inc"

// Tab buttons

#include "wx/mobile/generic/bitmaps/stock_bookmarks_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_contacts_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_downloads_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_favourites_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_featured_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_more_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_mostrecent_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_mostviewed_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_recent_24x24.inc"
#include "wx/mobile/generic/bitmaps/stock_search_24x24.inc"

struct wxMoBarButtonStockItem
{
    wxWindowID      m_id;
    const wxChar*   m_label;
    int             m_barType;
    char**          m_bitmap;
    int             m_bitmapSize;
};

#define wxMO_BITMAP(bitmap) (char**) bitmap

#define MO_STOCK_TEXT(id, label, barType) { (wxWindowID) id, label, barType, NULL, 0 }
#define MO_STOCK_BMP(id, label, barType, bitmap) { (wxWindowID) id, label, barType, (char**) bitmap, sizeof(bitmap) }

static wxMoBarButtonStockItem wxMoBarButtonStockItems[] =
{
    // Bar button items

    MO_STOCK_TEXT(wxID_OK,             _("OK"),         wxMO_BARBUTTON),
    MO_STOCK_BMP (wxID_SEARCH,         _("Search"),     wxMO_BARBUTTON, stock_search_20x20_png),
    MO_STOCK_TEXT(wxID_FLEXIBLESPACE,  _(""),           wxMO_BARBUTTON),
    MO_STOCK_TEXT(wxID_FIXEDSPACE,     _(""),           wxMO_BARBUTTON),
    MO_STOCK_BMP (wxID_COMPOSE,        _("Compose"),    wxMO_BARBUTTON, stock_compose_20x20_png),
    MO_STOCK_BMP (wxID_REPLY,          _("Reply"),      wxMO_BARBUTTON, stock_reply_20x20_png),
    MO_STOCK_BMP (wxID_ACTION,         _("Action"),     wxMO_BARBUTTON, stock_action_20x20_png),
    MO_STOCK_BMP (wxID_ORGANIZE,       _("Organize"),   wxMO_BARBUTTON, stock_organize_20x20_png),
    MO_STOCK_BMP (wxID_BOOKMARKS,      _("Bookmarks"),  wxMO_BARBUTTON, stock_bookmarks_20x20_png),
    MO_STOCK_BMP (wxID_CAMERA,         _("Camera"),     wxMO_BARBUTTON, stock_camera_20x20_png),
    MO_STOCK_BMP (wxID_TRASH,          _("Trash"),      wxMO_BARBUTTON, stock_trash_20x20_png),
    MO_STOCK_BMP (wxID_PLAY,           _("Play"),       wxMO_BARBUTTON, stock_play_20x20_png),
    MO_STOCK_BMP (wxID_PAUSE,          _("Pause"),      wxMO_BARBUTTON, stock_pause_20x20_png),
    MO_STOCK_BMP (wxID_REWIND,         _("Rewind"),     wxMO_BARBUTTON, stock_rewind_20x20_png),
    MO_STOCK_BMP (wxID_FASTFORWARD,    _("Fast Forward"),wxMO_BARBUTTON, stock_fastforward_20x20_png),
    MO_STOCK_TEXT(wxID_CANCEL,         _("Cancel"),     wxMO_BARBUTTON),
    MO_STOCK_TEXT(wxID_SAVE,           _("Save"),       wxMO_BARBUTTON),
    MO_STOCK_TEXT(wxID_EDIT,           _("Edit"),       wxMO_BARBUTTON),
    MO_STOCK_BMP (wxID_ADD,            _("Add"),        wxMO_BARBUTTON, stock_add_20x20_png),
    MO_STOCK_BMP (wxID_REFRESH,        _("Refresh"),    wxMO_BARBUTTON, stock_refresh_20x20_png),
    MO_STOCK_BMP (wxID_STOP,           _("Stop"),       wxMO_BARBUTTON, stock_stop_20x20_png),


    // Tab bar items

    MO_STOCK_BMP (wxID_MORE,            _("More"),       wxMO_TABBUTTON, stock_more_24x24_png),
    MO_STOCK_BMP (wxID_BOOKMARKS,       _("Bookmarks"),  wxMO_TABBUTTON, stock_bookmarks_24x24_png),
    MO_STOCK_BMP (wxID_SEARCH,          _("Search"),     wxMO_TABBUTTON, stock_search_24x24_png),
    MO_STOCK_BMP (wxID_FAVORITES,       _("Favorites"),  wxMO_TABBUTTON, stock_favourites_24x24_png),
    MO_STOCK_BMP (wxID_FEATURED,        _("Featured"),   wxMO_TABBUTTON, stock_featured_24x24_png),
    MO_STOCK_BMP (wxID_TOPRATED,        _("Top rated"),  wxMO_TABBUTTON, stock_favourites_24x24_png),
    MO_STOCK_BMP (wxID_RECENTS,         _("Recent"),     wxMO_TABBUTTON, stock_recent_24x24_png),
    MO_STOCK_BMP (wxID_CONTACTS,        _("Contacts"),   wxMO_TABBUTTON, stock_contacts_24x24_png),
    MO_STOCK_BMP (wxID_HISTORY,         _("History"),    wxMO_TABBUTTON, stock_recent_24x24_png),
    MO_STOCK_BMP (wxID_DOWNLOADS,       _("Downloads"),  wxMO_TABBUTTON, stock_downloads_24x24_png),
    MO_STOCK_BMP (wxID_MOSTRECENT,      _("Most recent"), wxMO_TABBUTTON, stock_mostrecent_24x24_png),
    MO_STOCK_BMP (wxID_MOSTVIEWED,      _("Most viewed"), wxMO_TABBUTTON, stock_mostviewed_24x24_png)
};

// Is this a stock item?
bool wxMoBarButton::IsStockId(int id, int barType)
{
    size_t i;
    size_t arr = sizeof(wxMoBarButtonStockItems)/sizeof(wxMoBarButtonStockItem);
    for (i = 0; i < arr; i++)
    {
        if (wxMoBarButtonStockItems[i].m_id == id && (wxMoBarButtonStockItems[i].m_barType & barType))
            return true;
    }

    return false;
}

// Get the stock label for the given identifier
wxString wxMoBarButton::GetStockLabel(int id)
{
    size_t i;
    size_t arr = sizeof(wxMoBarButtonStockItems)/sizeof(wxMoBarButtonStockItem);
    for (i = 0; i < arr; i++)
    {
        if (wxMoBarButtonStockItems[i].m_id == id)
        {
            wxString label = wxMoBarButtonStockItems[i].m_label;
            return label;
        }
    }

    return wxEmptyString;
}

// Get the stock bitmap for the given identifier
wxBitmap wxMoBarButton::GetStockBitmap(int id, int barType)
{
    wxBitmap bitmap;

    size_t i;
    size_t arr = sizeof(wxMoBarButtonStockItems)/sizeof(wxMoBarButtonStockItem);
    for (i = 0; i < arr; i++)
    {
        if (wxMoBarButtonStockItems[i].m_id == id && (wxMoBarButtonStockItems[i].m_barType & barType))
        {
            if (wxMoBarButtonStockItems[i].m_bitmap)
                bitmap = GetBitmapFromData((const unsigned char *) wxMoBarButtonStockItems[i].m_bitmap, wxMoBarButtonStockItems[i].m_bitmapSize);
            return bitmap;
        }
    }

    return bitmap;
}

// Get the stock help string for the given identifier
wxString wxMoBarButton::GetStockHelpString(int WXUNUSED(id))
{
    // Probably won't implement if iPhone doesn't support help strings
    wxString helpString;
    return helpString;
}

// Get a bitmap, such as a PNG, from raw data
wxBitmap wxMoBarButton::GetBitmapFromData(const unsigned char *data, int length)
{
    wxMemoryInputStream is(data, length);
    return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
}
