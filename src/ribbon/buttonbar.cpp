///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/buttonbar.cpp
// Purpose:     Ribbon control similar to a tool bar
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-01
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/buttonbar.h"

#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonButtonBar, wxRibbonControl)

BEGIN_EVENT_TABLE(wxRibbonButtonBar, wxRibbonControl)
    EVT_ERASE_BACKGROUND(wxRibbonButtonBar::OnEraseBackground)
    EVT_PAINT(wxRibbonButtonBar::OnPaint)
    EVT_SIZE(wxRibbonButtonBar::OnSize)
END_EVENT_TABLE()

class wxRibbonButtonBarButtonSizeInfo
{
public:
    bool is_supported;
    wxSize size;
    wxRect normal_region;
    wxRect dropdown_region;
};

class wxRibbonButtonBarButtonInstance
{
public:
    wxPoint position;
    wxRibbonButtonBarButtonBase* base;
    wxRibbonButtonBarButtonState size;
};

class wxRibbonButtonBarButtonBase
{
public:
    wxRibbonButtonBarButtonInstance NewInstance()
    {
        wxRibbonButtonBarButtonInstance i;
        i.base = this;
        return i;
    }

    wxRibbonButtonBarButtonState GetLargestSize()
    {
        if(sizes[wxRIBBON_BUTTONBAR_BUTTON_LARGE].is_supported)
            return wxRIBBON_BUTTONBAR_BUTTON_LARGE;
        if(sizes[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM].is_supported)
            return wxRIBBON_BUTTONBAR_BUTTON_MEDIUM;
        wxASSERT(sizes[wxRIBBON_BUTTONBAR_BUTTON_SMALL].is_supported);
        return wxRIBBON_BUTTONBAR_BUTTON_SMALL;
    }

    wxString label;
    wxString help_string;
    wxBitmap bitmap_large;
    wxBitmap bitmap_large_disabled;
    wxBitmap bitmap_small;
    wxBitmap bitmap_small_disabled;
    wxRibbonButtonBarButtonSizeInfo sizes[3];
    wxObject* client_data;
    int id;
    wxRibbonButtonBarButtonKind kind;
    long state;
};

WX_DECLARE_OBJARRAY(wxRibbonButtonBarButtonInstance, wxArrayRibbonButtonBarButtonInstance);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayRibbonButtonBarButtonInstance);

class wxRibbonButtonBarLayout
{
public:
    wxSize overall_size;
    wxArrayRibbonButtonBarButtonInstance buttons;
};

wxRibbonButtonBar::wxRibbonButtonBar()
{
    m_layouts_valid = false;
}

wxRibbonButtonBar::wxRibbonButtonBar(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    m_layouts_valid = false;

    CommonInit(style);
}

wxRibbonButtonBar::~wxRibbonButtonBar()
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        delete button;
    }
    m_buttons.Clear();

    count = m_layouts.GetCount();
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
        delete layout;
    }
    m_layouts.Clear();
}

bool wxRibbonButtonBar::Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if(!wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE))
    {
        return false;
    }

    CommonInit(style);
    return true;
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string,
                wxRibbonButtonBarButtonKind kind)
{
    return AddButton(button_id, label, bitmap, wxNullBitmap, wxNullBitmap,
        wxNullBitmap, kind, help_string);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddDropdownButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return AddButton(button_id, label, bitmap, help_string,
        wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddHybridButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return AddButton(button_id, label, bitmap, help_string,
        wxRIBBON_BUTTONBAR_BUTTON_HYBRID);
}
    
wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxBitmap& bitmap_small,
                const wxBitmap& bitmap_disabled,
                const wxBitmap& bitmap_small_disabled,
                wxRibbonButtonBarButtonKind kind,
                const wxString& help_string,
                wxObject* client_data)
{
    wxASSERT(bitmap.IsOk() || bitmap_small.IsOk());
    if(m_buttons.IsEmpty())
    {
        if(bitmap.IsOk())
        {
            m_bitmap_size_large = bitmap.GetSize();
            if(!bitmap_small.IsOk())
            {
                m_bitmap_size_small = m_bitmap_size_large;
                m_bitmap_size_small *= 0.5;
            }
        }
        if(bitmap_small.IsOk())
        {
            m_bitmap_size_small = bitmap_small.GetSize();
            if(!bitmap.IsOk())
            {
                m_bitmap_size_large = m_bitmap_size_small;
                m_bitmap_size_large *= 2.0;
            }
        }
    }

    wxRibbonButtonBarButtonBase* base = new wxRibbonButtonBarButtonBase;
    base->id = button_id;
    base->label = label;
    base->bitmap_large = bitmap;
    if(!base->bitmap_large.IsOk())
    {
        base->bitmap_large = MakeResizedBitmap(base->bitmap_small,
            m_bitmap_size_large);
    }
    else if(base->bitmap_large.GetSize() != m_bitmap_size_large)
    {
        base->bitmap_large = MakeResizedBitmap(base->bitmap_large,
            m_bitmap_size_large);
    }
    base->bitmap_small = bitmap_small;
    if(!base->bitmap_small.IsOk())
    {
        base->bitmap_small = MakeResizedBitmap(base->bitmap_large,
            m_bitmap_size_small);
    }
    else if(base->bitmap_small.GetSize() != m_bitmap_size_small)
    {
        base->bitmap_small = MakeResizedBitmap(base->bitmap_small,
            m_bitmap_size_small);
    }
    base->bitmap_large_disabled = bitmap_disabled;
    if(!base->bitmap_large_disabled.IsOk())
    {
        base->bitmap_large_disabled = MakeDisabledBitmap(base->bitmap_large);
    }
    base->bitmap_small_disabled = bitmap_small_disabled;
    if(!base->bitmap_small_disabled.IsOk())
    {
        base->bitmap_small_disabled = MakeDisabledBitmap(base->bitmap_small);
    }
    base->kind = kind;
    base->help_string = help_string;
    base->client_data = client_data;
    base->state = 0;

    wxMemoryDC temp_dc;
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);

    // TODO
    m_buttons.Add(base);
    m_layouts_valid = false;
    return base;
}

void wxRibbonButtonBar::FetchButtonSizeInfo(wxRibbonButtonBarButtonBase* button,
        wxRibbonButtonBarButtonState size, wxDC& dc)
{
    wxRibbonButtonBarButtonSizeInfo& info = button->sizes[size];
    info.is_supported = m_art->GetButtonBarButtonSize(dc, this,
        button->kind, size, button->label, m_bitmap_size_large,
        m_bitmap_size_small, &info.size, &info.normal_region,
        &info.dropdown_region);
}

wxBitmap wxRibbonButtonBar::MakeResizedBitmap(const wxBitmap& original, wxSize size)
{
    wxImage img(original.ConvertToImage());
    img.Rescale(size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH);
    return wxBitmap(img);
}

wxBitmap wxRibbonButtonBar::MakeDisabledBitmap(const wxBitmap& original)
{
    wxImage img(original.ConvertToImage());
    return wxBitmap(img.ConvertToGreyscale());
}

bool wxRibbonButtonBar::Realize()
{
    if(!m_layouts_valid)
    {
        MakeLayouts();
        m_layouts_valid = true;
    }
    return true;
}

void wxRibbonButtonBar::ClearButtons()
{
    m_layouts_valid = false;
    size_t count = m_buttons.GetCount();
    size_t i;
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        delete button;
    }
    m_buttons.Clear();
    Realize();
}

bool wxRibbonButtonBar::DeleteButton(int button_id)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if(button->id == button_id)
        {
            m_layouts_valid = false;
            m_buttons.RemoveAt(i);
            Realize();
            Refresh();
            return true;
        }
    }
    return false;
}

void wxRibbonButtonBar::EnableButton(int button_id, bool enable)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if(button->id == button_id)
        {
            if(enable)
            {
                if(button->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED)
                {
                    button->state &= ~wxRIBBON_BUTTONBAR_BUTTON_DISABLED;
                    Refresh();
                }
            }
            else
            {
                if((button->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0)
                {
                    button->state |= wxRIBBON_BUTTONBAR_BUTTON_DISABLED;
                    Refresh();
                }
            }
            return;
        }
    }
}

void wxRibbonButtonBar::SetArtProvider(wxRibbonArtProvider* art)
{
    if(art == m_art)
    {
        return;
    }

    wxRibbonControl::SetArtProvider(art);

    wxMemoryDC temp_dc;
    size_t btn_count = m_buttons.Count();
    size_t btn_i;
    for(btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonBase* base = m_buttons.Item(btn_i);

        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);
    }

    m_layouts_valid = false;
    Realize();
}

bool wxRibbonButtonBar::IsSizingContinuous() const
{
    return false;
}

wxSize wxRibbonButtonBar::GetNextSmallerSize(wxOrientation direction) const
{
    wxSize result = GetSize();
    size_t nlayouts = m_layouts.GetCount();
    size_t i;
    for(i = 0; i < nlayouts; ++i)
    {
        wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
        wxSize size = layout->overall_size;
        switch(direction)
        {
        case wxHORIZONTAL:
            if(size.x < result.x && size.y <= result.y)
            {
                result.x = size.x;
                break;
            }
            else
                continue;
        case wxVERTICAL:
            if(size.x <= result.x && size.y < result.y)
            {
                result.y = size.y;
                break;
            }
            else
                continue;
        case wxBOTH:
            if(size.x < result.x && size.y < result.y)
            {
                result = size;
                break;
            }
            else
                continue;
        }
        break;
    }
    return result;
}

wxSize wxRibbonButtonBar::GetNextLargerSize(wxOrientation direction) const
{
    wxSize result = GetSize();
    size_t nlayouts = m_layouts.GetCount();
    size_t i = nlayouts;
    while(i > 0)
    {
        --i;
        wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
        wxSize size = layout->overall_size;
        switch(direction)
        {
        case wxHORIZONTAL:
            if(size.x > result.x && size.y <= result.y)
            {
                result.x = size.x;
                break;
            }
            else
                continue;
        case wxVERTICAL:
            if(size.x <= result.x && size.y > result.y)
            {
                result.y = size.y;
                break;
            }
            else
                continue;
        case wxBOTH:
            if(size.x > result.x && size.y > result.y)
            {
                result = size;
                break;
            }
            else
                continue;
        }
        break;
    }
    return result;
}

void wxRibbonButtonBar::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonButtonBar::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    m_art->DrawButtonBarBackground(dc, this, GetSize());

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_current_layout);

    size_t btn_count = layout->buttons.Count();
    size_t btn_i;
    for(btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& button = layout->buttons.Item(btn_i);
        wxRibbonButtonBarButtonBase* base = button.base;

        wxBitmap* bitmap = &base->bitmap_large;
        wxBitmap* bitmap_small = &base->bitmap_small;
        if(base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED)
        {
            bitmap = &base->bitmap_large_disabled;
            bitmap_small = &base->bitmap_small_disabled;
        }
        wxRect rect(button.position, base->sizes[button.size].size);
        
        m_art->DrawButtonBarButton(dc, this, rect, base->kind,
            base->state | button.size, base->label, *bitmap, *bitmap_small);
    }
}

void wxRibbonButtonBar::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}

void wxRibbonButtonBar::CommonInit(long WXUNUSED(style))
{
    m_bitmap_size_large = wxSize(32, 32);
    m_bitmap_size_small = wxSize(16, 16);

    wxRibbonButtonBarLayout* placeholder_layout = new wxRibbonButtonBarLayout;
    placeholder_layout->overall_size = wxSize(20, 20);
    m_layouts.Add(placeholder_layout);
    m_current_layout = 0;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
};

wxSize wxRibbonButtonBar::GetMinSize() const
{
    return m_layouts.Last()->overall_size;
}

wxSize wxRibbonButtonBar::DoGetBestSize() const
{
    return m_layouts.Item(0)->overall_size;
}

void wxRibbonButtonBar::MakeLayouts()
{
    if(m_layouts_valid)
    {
        return;
    }
    {
        // Clear existing layouts
        size_t count = m_layouts.GetCount();
        size_t i;
        for(i = 0; i < count; ++i)
        {
            wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
            delete layout;
        }
        m_layouts.Clear();
    }
    size_t btn_count = m_buttons.Count();
    size_t btn_i;
    {
        // Best layout : all buttons large, stacking horizontally
        wxRibbonButtonBarLayout* layout = new wxRibbonButtonBarLayout;
        wxPoint cursor(0, 0);
        layout->overall_size.SetHeight(0);
        for(btn_i = 0; btn_i < btn_count; ++btn_i)
        {
            wxRibbonButtonBarButtonBase* button = m_buttons.Item(btn_i);
            wxRibbonButtonBarButtonInstance instance  = button->NewInstance();
            instance.position = cursor;
            instance.size = button->GetLargestSize();
            wxSize& size = button->sizes[instance.size].size;
            cursor.x += size.GetWidth();
            layout->overall_size.SetHeight(wxMax(layout->overall_size.GetHeight(),
                size.GetHeight()));
            layout->buttons.Add(instance);
        }
        layout->overall_size.SetWidth(cursor.x);
        m_layouts.Add(layout);
    }
}

#endif // wxUSE_RIBBON
