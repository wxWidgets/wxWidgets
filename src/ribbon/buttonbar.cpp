///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/buttonbar.cpp
// Purpose:     Ribbon control similar to a tool bar
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-01
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/panel.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/art.h"
#include "wx/dcbuffer.h"
#include "wx/imaglist.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxDEFINE_EVENT(wxEVT_RIBBONBUTTONBAR_CLICKED, wxRibbonButtonBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, wxRibbonButtonBarEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxRibbonButtonBarEvent, wxCommandEvent);
wxIMPLEMENT_CLASS(wxRibbonButtonBar, wxRibbonControl);

wxBEGIN_EVENT_TABLE(wxRibbonButtonBar, wxRibbonControl)
    EVT_ERASE_BACKGROUND(wxRibbonButtonBar::OnEraseBackground)
    EVT_ENTER_WINDOW(wxRibbonButtonBar::OnMouseEnter)
    EVT_LEAVE_WINDOW(wxRibbonButtonBar::OnMouseLeave)
    EVT_MOTION(wxRibbonButtonBar::OnMouseMove)
    EVT_PAINT(wxRibbonButtonBar::OnPaint)
    EVT_SIZE(wxRibbonButtonBar::OnSize)
    EVT_LEFT_DOWN(wxRibbonButtonBar::OnMouseDown)
    EVT_LEFT_DCLICK(wxRibbonButtonBar::OnMouseDown)
    EVT_LEFT_UP(wxRibbonButtonBar::OnMouseUp)
wxEND_EVENT_TABLE()

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

namespace
{

wxBitmap MakeResizedBitmap(const wxBitmap& original, wxSize size)
{
    double scale = original.GetScaleFactor();
    if (scale > 1.0)
        scale = 2.0;

    wxImage img(original.ConvertToImage());
    img.Rescale(int(scale * size.GetWidth()), int(scale * size.GetHeight()), wxIMAGE_QUALITY_HIGH);
    return wxBitmap(img, -1, scale);
}

wxBitmap MakeDisabledBitmap(const wxBitmap& original)
{
    wxImage img(original.ConvertToImage());
    return wxBitmap(img.ConvertToGreyscale(), -1, original.GetScaleFactor());
}

} // anonymous namespace

class wxRibbonButtonBarButtonBase
{
public:
    wxRibbonButtonBarButtonBase()
    {
        barButtonImageListPos =
        barButtonSmallImageListPos = -1;
    }

    void SetBitmaps(wxRibbonBar* ribbon,
                    wxSize bitmap_size_large,
                    wxSize bitmap_size_small,
                    wxBitmap bitmap_large,
                    wxBitmap bitmap_large_disabled,
                    wxBitmap bitmap_small,
                    wxBitmap bitmap_small_disabled)
    {
        if(!bitmap_large.IsOk())
        {
            bitmap_large = MakeResizedBitmap(bitmap_small, bitmap_size_large);
        }
        else if(bitmap_large.GetLogicalSize() != bitmap_size_large)
        {
            bitmap_large = MakeResizedBitmap(bitmap_large, bitmap_size_large);
        }

        if(!bitmap_small.IsOk())
        {
            bitmap_small = MakeResizedBitmap(bitmap_large, bitmap_size_small);
        }
        else if(bitmap_small.GetLogicalSize() != bitmap_size_small)
        {
            bitmap_small = MakeResizedBitmap(bitmap_small, bitmap_size_small);
        }

        if(!bitmap_large_disabled.IsOk())
        {
            bitmap_large_disabled = MakeDisabledBitmap(bitmap_large);
        }

        if(!bitmap_small_disabled.IsOk())
        {
            bitmap_small_disabled = MakeDisabledBitmap(bitmap_small);
        }

        if ( bitmap_large.IsOk() )
        {
            wxImageList* const
                buttonImageList = ribbon->GetButtonImageList(bitmap_size_large);

            barButtonImageListPos = buttonImageList->Add(bitmap_large);
            buttonImageList->Add(bitmap_large_disabled);
        }

        wxImageList* const
            buttonSmallImageList = ribbon->GetButtonImageList(bitmap_size_small);

        barButtonSmallImageListPos = buttonSmallImageList->Add(bitmap_small);
        buttonSmallImageList->Add(bitmap_small_disabled);
    }

    void GetBitmaps(wxRibbonBar* ribbon,
                    wxSize bitmap_size_large,
                    wxSize bitmap_size_small,
                    wxBitmap& bitmap,
                    wxBitmap& bitmap_small) const
    {
        wxImageList* buttonImageList = ribbon->GetButtonImageList(bitmap_size_large);
        wxImageList* buttonSmallImageList = ribbon->GetButtonImageList(bitmap_size_small);

        int pos = barButtonImageListPos;
        int pos_small = barButtonSmallImageListPos;

        if (state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED)
        {
            // Disabled buttons are stored after the normal ones.
            pos++;
            pos_small++;
        }

        bitmap = buttonImageList->GetBitmap(pos);
        bitmap_small = buttonSmallImageList->GetBitmap(pos_small);
    }

    wxRibbonButtonBarButtonInstance NewInstance()
    {
        wxRibbonButtonBarButtonInstance i;
        i.base = this;
        return i;
    }

    wxRibbonButtonBarButtonState GetLargestSize()
    {
        if(sizes[wxRIBBON_BUTTONBAR_BUTTON_LARGE].is_supported
           && max_size_class >= wxRIBBON_BUTTONBAR_BUTTON_LARGE)
        {
            return wxRIBBON_BUTTONBAR_BUTTON_LARGE;
        }
        if(sizes[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM].is_supported
           && max_size_class >= wxRIBBON_BUTTONBAR_BUTTON_MEDIUM)
        {
            return wxRIBBON_BUTTONBAR_BUTTON_MEDIUM;
        }
        wxASSERT(sizes[wxRIBBON_BUTTONBAR_BUTTON_SMALL].is_supported);
        return wxRIBBON_BUTTONBAR_BUTTON_SMALL;
    }

    bool GetSmallerSize(
        wxRibbonButtonBarButtonState* size, int n = 1)
    {
        for(; n > 0; --n)
        {
            switch(*size)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                if(sizes[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM].is_supported
                   && min_size_class <= wxRIBBON_BUTTONBAR_BUTTON_MEDIUM)
                {
                    *size = wxRIBBON_BUTTONBAR_BUTTON_MEDIUM;
                    break;
                }
                wxFALLTHROUGH;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
                if(sizes[wxRIBBON_BUTTONBAR_BUTTON_SMALL].is_supported
                   && min_size_class <= wxRIBBON_BUTTONBAR_BUTTON_SMALL)
                {
                    *size = wxRIBBON_BUTTONBAR_BUTTON_SMALL;
                    break;
                }
                wxFALLTHROUGH;
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
            default:
                return false;
            }
        }
        return true;
    }

    wxString label;
    wxString help_string;
    wxCoord text_min_width[3];

    // Index of the bitmap in the wxRibbonBar normal image list. Notice that
    // the disabled bitmap is in the next position, so this one is always even.
    int barButtonImageListPos;

    // Same thing for the small bitmap index in the small image list.
    int barButtonSmallImageListPos;

    wxRibbonButtonBarButtonSizeInfo sizes[3];
    wxRibbonButtonBarButtonState min_size_class;
    wxRibbonButtonBarButtonState max_size_class;
    wxClientDataContainer client_data;
    int id;
    wxRibbonButtonKind kind;
    long state;
};

WX_DECLARE_OBJARRAY(wxRibbonButtonBarButtonInstance, wxArrayRibbonButtonBarButtonInstance);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayRibbonButtonBarButtonInstance)

class wxRibbonButtonBarLayout
{
public:
    wxSize overall_size;
    wxArrayRibbonButtonBarButtonInstance buttons;

    void CalculateOverallSize()
    {
        overall_size = wxSize(0, 0);
        size_t btn_count = buttons.Count();
        size_t btn_i;
        for(btn_i = 0; btn_i < btn_count; ++btn_i)
        {
            wxRibbonButtonBarButtonInstance& instance = buttons.Item(btn_i);
            wxSize size = instance.base->sizes[instance.size].size;
            int right = instance.position.x + size.GetWidth();
            int bottom = instance.position.y + size.GetHeight();
            if(right > overall_size.GetWidth())
            {
                overall_size.SetWidth(right);
            }
            if(bottom > overall_size.GetHeight())
            {
                overall_size.SetHeight(bottom);
            }
        }
    }

    wxRibbonButtonBarButtonInstance* FindSimilarInstance(
        wxRibbonButtonBarButtonInstance* inst)
    {
        if(inst == nullptr)
        {
            return nullptr;
        }
        size_t btn_count = buttons.Count();
        size_t btn_i;
        for(btn_i = 0; btn_i < btn_count; ++btn_i)
        {
            wxRibbonButtonBarButtonInstance& instance = buttons.Item(btn_i);
            if(instance.base == inst->base)
            {
                return &instance;
            }
        }
        return nullptr;
    }
};

wxRibbonButtonBar::wxRibbonButtonBar()
{
    m_layouts_valid = false;
    CommonInit (0);
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
                wxRibbonButtonKind kind)
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
        wxRIBBON_BUTTON_DROPDOWN);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddToggleButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return AddButton(button_id, label, bitmap, help_string,
        wxRIBBON_BUTTON_TOGGLE);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddHybridButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return AddButton(button_id, label, bitmap, help_string,
        wxRIBBON_BUTTON_HYBRID);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxBitmap& bitmap_small,
                const wxBitmap& bitmap_disabled,
                const wxBitmap& bitmap_small_disabled,
                wxRibbonButtonKind kind,
                const wxString& help_string)
{
    return InsertButton(GetButtonCount(), button_id, label, bitmap,
        bitmap_small, bitmap_disabled,bitmap_small_disabled, kind, help_string);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertButton(
                size_t pos,
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxBitmap& bitmap_small,
                const wxBitmap& bitmap_disabled,
                const wxBitmap& bitmap_small_disabled,
                wxRibbonButtonKind kind,
                const wxString& help_string)
{
    wxASSERT(bitmap.IsOk() || bitmap_small.IsOk());
    if(m_buttons.IsEmpty())
    {
        if(bitmap.IsOk())
        {
            m_bitmap_size_large = bitmap.GetLogicalSize();
            if(!bitmap_small.IsOk())
            {
                m_bitmap_size_small = m_bitmap_size_large;
                m_bitmap_size_small *= 0.5;
            }
        }
        if(bitmap_small.IsOk())
        {
            m_bitmap_size_small = bitmap_small.GetLogicalSize();
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
    base->SetBitmaps(m_ribbonBar, m_bitmap_size_large, m_bitmap_size_small,
                     bitmap, bitmap_disabled, bitmap_small, bitmap_small_disabled);
    base->kind = kind;
    base->help_string = help_string;
    base->state = 0;
    base->text_min_width[0] = 0;
    base->text_min_width[1] = 0;
    base->text_min_width[2] = 0;
    base->min_size_class = wxRIBBON_BUTTONBAR_BUTTON_SMALL;
    base->max_size_class = wxRIBBON_BUTTONBAR_BUTTON_LARGE;

    wxClientDC temp_dc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);

    m_buttons.Insert(base, pos);
    m_layouts_valid = false;
    return base;
}


void
wxRibbonButtonBar::SetItemClientObject(wxRibbonButtonBarButtonBase* item,
                                       wxClientData* data)
{
    wxCHECK_RET( item, "Can't associate client object with an invalid item" );

    item->client_data.SetClientObject(data);
}

wxClientData*
wxRibbonButtonBar::GetItemClientObject(const wxRibbonButtonBarButtonBase* item) const
{
    wxCHECK_MSG( item, nullptr, "Can't get client object for an invalid item" );

    return item->client_data.GetClientObject();
}

void
wxRibbonButtonBar::SetItemClientData(wxRibbonButtonBarButtonBase* item,
                                     void* data)
{
    wxCHECK_RET( item, "Can't associate client data with an invalid item" );

    item->client_data.SetClientData(data);
}

void*
wxRibbonButtonBar::GetItemClientData(const wxRibbonButtonBarButtonBase* item) const
{
    wxCHECK_MSG( item, nullptr, "Can't get client data for an invalid item" );

    return item->client_data.GetClientData();
}


wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertButton(
                size_t pos,
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string,
                wxRibbonButtonKind kind)
{
    return InsertButton(pos, button_id, label, bitmap, wxNullBitmap,
        wxNullBitmap, wxNullBitmap, kind, help_string);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertDropdownButton(
                size_t pos,
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return InsertButton(pos, button_id, label, bitmap, help_string,
        wxRIBBON_BUTTON_DROPDOWN);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertToggleButton(
                size_t pos,
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return InsertButton(pos, button_id, label, bitmap, help_string,
        wxRIBBON_BUTTON_TOGGLE);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertHybridButton(
                size_t pos,
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string)
{
    return InsertButton(pos, button_id, label, bitmap, help_string,
        wxRIBBON_BUTTON_HYBRID);
}

void wxRibbonButtonBar::FetchButtonSizeInfo(wxRibbonButtonBarButtonBase* button,
        wxRibbonButtonBarButtonState size, wxDC& dc)
{
    wxRibbonButtonBarButtonSizeInfo& info = button->sizes[size];
    if(m_art)
    {
        info.is_supported = m_art->GetButtonBarButtonSize(dc, this,
            button->kind, size, button->label, button->text_min_width[size],
            m_bitmap_size_large, m_bitmap_size_small, &info.size,
            &info.normal_region, &info.dropdown_region);
    }
    else
        info.is_supported = false;
}

size_t wxRibbonButtonBar::GetButtonCount() const
{
    return m_buttons.GetCount();
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
            if (m_hovered_button && m_hovered_button->base == button)
                m_hovered_button = nullptr;
            if (m_active_button  && m_active_button->base  == button)
                m_active_button = nullptr;
            delete button;
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

void wxRibbonButtonBar::ToggleButton(int button_id, bool checked)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for(i = 0; i < count; ++i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if(button->id == button_id)
        {
            if(checked)
            {
                if((button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED) == 0)
                {
                    button->state |= wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                    Refresh();
                }
            }
            else
            {
                if(button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED)
                {
                    button->state &= ~wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                    Refresh();
                }
            }
            return;
        }
    }
}

void wxRibbonButtonBar::SetButtonIcon(
                int button_id,
                const wxBitmap& bitmap,
                const wxBitmap& bitmap_small,
                const wxBitmap& bitmap_disabled,
                const wxBitmap& bitmap_small_disabled)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    base->SetBitmaps(m_ribbonBar, m_bitmap_size_large, m_bitmap_size_small,
                     bitmap, bitmap_disabled, bitmap_small, bitmap_small_disabled);
    Refresh();
}

void wxRibbonButtonBar::SetButtonText(int button_id, const wxString& label)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    base->label = label;

    wxClientDC temp_dc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);
    m_layouts_valid = false;
    Refresh();
}

void wxRibbonButtonBar::SetButtonTextMinWidth(int button_id,
                int min_width_medium, int min_width_large)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    base->text_min_width[0] = 0;
    base->text_min_width[1] = min_width_medium;
    base->text_min_width[2] = min_width_large;
    wxClientDC temp_dc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);
    m_layouts_valid = false;
}

void wxRibbonButtonBar::SetButtonTextMinWidth(
                int button_id, const wxString& label)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    wxClientDC temp_dc(this);
    base->text_min_width[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM] =
        m_art->GetButtonBarButtonTextWidth(
        temp_dc, label, base->kind, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
    base->text_min_width[wxRIBBON_BUTTONBAR_BUTTON_LARGE] =
        m_art->GetButtonBarButtonTextWidth(
        temp_dc, label, base->kind, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, temp_dc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, temp_dc);
    m_layouts_valid = false;
}

void wxRibbonButtonBar::SetButtonMinSizeClass(int button_id,
                wxRibbonButtonBarButtonState min_size_class)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    if(base->max_size_class < min_size_class)
    {
        wxFAIL_MSG("Button minimum size is larger than maximum size");
        return;
    }
    base->min_size_class = min_size_class;
    m_layouts_valid = false;
}

void wxRibbonButtonBar::SetButtonMaxSizeClass(int button_id,
                wxRibbonButtonBarButtonState max_size_class)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(button_id);
    if(base == nullptr)
        return;
    if(base->min_size_class > max_size_class)
    {
        wxFAIL_MSG("Button maximum size is smaller than minimum size");
        return;
    }
    base->max_size_class = max_size_class;
    m_layouts_valid = false;
}

void wxRibbonButtonBar::SetArtProvider(wxRibbonArtProvider* art)
{
    if(art == m_art)
    {
        return;
    }

    wxRibbonControl::SetArtProvider(art);

    // There is no need to do anything else when the art provider is reset to
    // null during our destruction and this actually results in problems during
    // program shutdown due to trying to get DPI of the already destroyed TLW
    // parent.
    if (!art)
        return;

    wxClientDC temp_dc(this);
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

wxSize wxRibbonButtonBar::DoGetNextSmallerSize(wxOrientation direction,
                                             wxSize result) const
{
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

wxSize wxRibbonButtonBar::DoGetNextLargerSize(wxOrientation direction,
                                            wxSize result) const
{
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

void wxRibbonButtonBar::UpdateWindowUI(long flags)
{
    wxWindowBase::UpdateWindowUI(flags);

    // don't waste time updating state of tools in a hidden toolbar
    if ( !IsShown() )
        return;

    size_t btn_count = m_buttons.size();
    bool rerealize = false;
    for ( size_t btn_i = 0; btn_i < btn_count; ++btn_i )
    {
        wxRibbonButtonBarButtonBase& btn = *m_buttons.Item(btn_i);
        int id = btn.id;

        wxUpdateUIEvent event(id);
        event.SetEventObject(this);

        if ( ProcessWindowEvent(event) )
        {
            if ( event.GetSetEnabled() )
                EnableButton(id, event.GetEnabled());
            if ( event.GetSetChecked() )
                ToggleButton(id, event.GetChecked());
            if ( event.GetSetText() )
            {
                btn.label = event.GetText();
                rerealize = true;
            }
        }
    }

    if ( rerealize )
        Realize();
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
        wxRect rect(button.position + m_layout_offset, base->sizes[button.size].size);

        wxBitmap bitmap, bitmap_small;
        base->GetBitmaps(m_ribbonBar,
                         m_bitmap_size_large, m_bitmap_size_small, bitmap,
                         bitmap_small);
        m_art->DrawButtonBarButton(dc, this, rect, base->kind,
            base->state | button.size, base->label, bitmap, bitmap_small);
     }
}

void wxRibbonButtonBar::OnSize(wxSizeEvent& evt)
{
    wxSize new_size = evt.GetSize();
    size_t layout_count = m_layouts.GetCount();
    size_t layout_i;
    m_current_layout = layout_count - 1;
    for(layout_i = 0; layout_i < layout_count; ++layout_i)
    {
        wxSize layout_size = m_layouts.Item(layout_i)->overall_size;
        if(layout_size.x <= new_size.x && layout_size.y <= new_size.y)
        {
            m_layout_offset.x = (new_size.x - layout_size.x) / 2;
            m_layout_offset.y = (new_size.y - layout_size.y) / 2;
            m_current_layout = layout_i;
            break;
        }
    }
    m_hovered_button = m_layouts.Item(m_current_layout)->FindSimilarInstance(m_hovered_button);
    Refresh();
}

void wxRibbonButtonBar::CommonInit(long WXUNUSED(style))
{
    // This can initialize it to nullptr when we're called from the default ctor,
    // but will set it to the correct value when used from non-default ctor or
    // Create() later.
    m_ribbonBar = GetAncestorRibbonBar();

    m_bitmap_size_large = wxSize(32, 32);
    m_bitmap_size_small = wxSize(16, 16);

    wxRibbonButtonBarLayout* placeholder_layout = new wxRibbonButtonBarLayout;
    placeholder_layout->overall_size = wxSize(20, 20);
    m_layouts.Add(placeholder_layout);
    m_current_layout = 0;
    m_layout_offset = wxPoint(0, 0);
    m_hovered_button = nullptr;
    m_active_button = nullptr;
    m_lock_active_state = false;
    m_show_tooltips_for_disabled = false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void wxRibbonButtonBar::SetShowToolTipsForDisabled(bool show)
{
    m_show_tooltips_for_disabled = show;
}

bool wxRibbonButtonBar::GetShowToolTipsForDisabled() const
{
    return m_show_tooltips_for_disabled;
}

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
    if(m_layouts_valid || m_art == nullptr)
    {
        return;
    }
    {
        // Clear existing layouts
        if(m_hovered_button)
        {
            m_hovered_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
            m_hovered_button = nullptr;
        }
        if(m_active_button)
        {
            m_active_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
            m_active_button = nullptr;
        }
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

    // Determine available height:
    // 1 large button or, if not found, 3 medium or small buttons
    int available_height = 0;
    bool large_button_found = false;
    for(btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(btn_i);
        wxRibbonButtonBarButtonState size_class = button->GetLargestSize();
        available_height = wxMax(available_height,
                                 button->sizes[size_class].size.GetHeight());
        if(size_class == wxRIBBON_BUTTONBAR_BUTTON_LARGE)
            large_button_found = true;
    }
    if(!large_button_found)
        available_height *= 3;

    int stacked_width = 0;
    {
        // Best layout : all buttons large, stacking horizontally,
        //               small buttons small, stacked vertically
        wxRibbonButtonBarLayout* layout = new wxRibbonButtonBarLayout;
        wxPoint cursor(0, 0);
        layout->overall_size.SetHeight(0);
        for(btn_i = 0; btn_i < btn_count; ++btn_i)
        {
            wxRibbonButtonBarButtonBase* button = m_buttons.Item(btn_i);
            wxRibbonButtonBarButtonInstance instance = button->NewInstance();
            instance.position = cursor;
            instance.size = button->GetLargestSize();
            wxSize& size = button->sizes[instance.size].size;

            if(instance.size < wxRIBBON_BUTTONBAR_BUTTON_LARGE)
            {
                stacked_width = wxMax(stacked_width, size.GetWidth());
                if(cursor.y + size.GetHeight() >= available_height)
                {
                    cursor.y = 0;
                    cursor.x += stacked_width;
                    stacked_width = 0;
                }
                else
                {
                    cursor.y += size.GetHeight();
                }
            }
            else
            {
                if(cursor.y != 0)
                {
                    cursor.y = 0;
                    cursor.x += stacked_width;
                    stacked_width = 0;
                    instance.position = cursor;
                }
                cursor.x += size.GetWidth();
            }
            layout->buttons.Add(instance);
        }
        layout->overall_size.SetHeight(available_height);
        layout->overall_size.SetWidth(cursor.x + stacked_width);
        m_layouts.Add(layout);
    }
    if(btn_count >= 2)
    {
        // Collapse the rightmost buttons and stack them vertically
        // if they are not already small. If rightmost buttons can't
        // be collapsed because "min_size_class" is set, try it again
        // starting from second rightmost button and so on.
        size_t iLast = btn_count;
        while(iLast-- > 0)
        {
            TryCollapseLayout(m_layouts.Last(), iLast, &iLast,
                              wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        }

        // TODO: small buttons are not implemented yet in 
        //       art_msw.cpp:2581 and will be invisible
        /*iLast = btn_count;
        while(iLast-- > 0)
        {
            TryCollapseLayout(m_layouts.Last(), iLast, &iLast,
                              wxRIBBON_BUTTONBAR_BUTTON_SMALL);
        }*/
    }
}

void wxRibbonButtonBar::TryCollapseLayout(wxRibbonButtonBarLayout* original,
                                          size_t first_btn, size_t* last_button,
                                          wxRibbonButtonBarButtonState target_size)
{
    size_t btn_count = m_buttons.Count();
    size_t btn_i;
    int used_height = 0;
    int used_width = 0;
    int original_column_width = 0;
    int available_width = 0;
    int available_height = original->overall_size.GetHeight();

    // Search for button range from right which should be
    // collapsed into a column of small buttons.
    for(btn_i = first_btn + 1; btn_i > 0; /* decrement is inside loop */)
    {
        --btn_i;
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(btn_i);
        wxRibbonButtonBarButtonState large_size_class = button->GetLargestSize();
        wxSize large_size = button->sizes[large_size_class].size;
        int t_available_width = available_width;

        original_column_width = wxMax(original_column_width,
                                      large_size.GetWidth());

        // Top button in column: add column width to available width
        if(original->buttons.Item(btn_i).position.y == 0)
        {
            t_available_width += original_column_width;
            original_column_width = 0;
        }

        wxRibbonButtonBarButtonState small_size_class = large_size_class;
        if(large_size_class > target_size)
        {
            if(!button->GetSmallerSize(&small_size_class,
                                       small_size_class - target_size))
            {
                // Large button that cannot shrink: stop search
                ++btn_i;
                break;
            }
        }
        wxSize small_size = button->sizes[small_size_class].size;
        int t_used_height = used_height + small_size.GetHeight();
        int t_used_width = wxMax(used_width, small_size.GetWidth());

        // Height is full: stop search
        if(t_used_height > available_height)
        {
            ++btn_i;
            break;
        }
        else
        {
            used_height = t_used_height;
            used_width = t_used_width;
            available_width = t_available_width;
        }
    }

    // Layout got wider than before or no suitable button found: abort
    if(btn_i >= first_btn || used_width >= available_width)
    {
        return;
    }
    if(last_button != nullptr)
    {
        *last_button = btn_i;
    }

    wxRibbonButtonBarLayout* layout = new wxRibbonButtonBarLayout;
    WX_APPEND_ARRAY(layout->buttons, original->buttons);
    wxPoint cursor(layout->buttons.Item(btn_i).position);

    cursor.y = 0;
    for(; btn_i <= first_btn; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.Item(btn_i);
        if(instance.size > target_size)
        {
            instance.base->GetSmallerSize(&instance.size,
                                          instance.size - target_size);
        }
        instance.position = cursor;
        cursor.y += instance.base->sizes[instance.size].size.GetHeight();
    }

    int x_adjust = available_width - used_width;

    // Adjust x coords of buttons right of shrinked column
    for(; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.Item(btn_i);
        instance.position.x -= x_adjust;
    }

    layout->CalculateOverallSize();

    // Sanity check
    if(layout->overall_size.GetWidth() >= original->overall_size.GetWidth() ||
        layout->overall_size.GetHeight() > original->overall_size.GetHeight())
    {
        delete layout;
        wxFAIL_MSG("Layout collapse resulted in increased size");
        return;
    }

    // If height isn't preserved (i.e. it is reduced), then the minimum
    // size for the button bar will decrease, preventing the original
    // layout from being used (in some cases).
    // If neither "min_size_class" nor "max_size_class" is set, this is
    // only required when the first button is involved in a collapse but
    // if small, medium and large buttons as well as min/max size classes
    // are involved this is always a good idea.
    layout->overall_size.SetHeight(original->overall_size.GetHeight());

    m_layouts.Add(layout);
}

void wxRibbonButtonBar::OnMouseMove(wxMouseEvent& evt)
{
    wxPoint cursor(evt.GetPosition());
    wxRibbonButtonBarButtonInstance* new_hovered = nullptr;
    wxRibbonButtonBarButtonInstance* tooltipButton = nullptr;
    long new_hovered_state = 0;

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_current_layout);
    size_t btn_count = layout->buttons.Count();
    size_t btn_i;
    for(btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.Item(btn_i);
        wxRibbonButtonBarButtonSizeInfo& size = instance.base->sizes[instance.size];
        wxRect btn_rect;
        btn_rect.SetTopLeft(m_layout_offset + instance.position);
        btn_rect.SetSize(size.size);
        if(btn_rect.Contains(cursor))
        {
            if((instance.base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0)
            {
                tooltipButton = &instance;
                new_hovered = &instance;
                new_hovered_state = instance.base->state;
                new_hovered_state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
                wxPoint offset(cursor);
                offset -= btn_rect.GetTopLeft();
                if(size.normal_region.Contains(offset))
                {
                    new_hovered_state |= wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED;
                }
                if(size.dropdown_region.Contains(offset))
                {
                    new_hovered_state |= wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_HOVERED;
                }
                break;
            }
            else if (m_show_tooltips_for_disabled)
            {
                tooltipButton = &instance;
            }
        }
    }

#if wxUSE_TOOLTIPS
    if(tooltipButton == nullptr && GetToolTip())
    {
        UnsetToolTip();
    }
    if(tooltipButton)
    {
        if (tooltipButton != m_hovered_button &&
                !(tooltipButton->size & wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE))
            SetToolTip(tooltipButton->base->help_string);
    }
#else
    wxUnusedVar(tooltipButton);
#endif

    if(new_hovered != m_hovered_button || (m_hovered_button != nullptr &&
        new_hovered_state != m_hovered_button->base->state))
    {
        if(m_hovered_button != nullptr)
        {
            m_hovered_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
        }
        m_hovered_button = new_hovered;
        if(m_hovered_button != nullptr)
        {
            m_hovered_button->base->state = new_hovered_state;
        }
        Refresh(false);
    }

    if(m_active_button && !m_lock_active_state)
    {
        long new_active_state = m_active_button->base->state;
        new_active_state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
        wxRibbonButtonBarButtonSizeInfo& size =
            m_active_button->base->sizes[m_active_button->size];
        wxRect btn_rect;
        btn_rect.SetTopLeft(m_layout_offset + m_active_button->position);
        btn_rect.SetSize(size.size);
        if(btn_rect.Contains(cursor))
        {
            wxPoint offset(cursor);
            offset -= btn_rect.GetTopLeft();
            if(size.normal_region.Contains(offset))
            {
                new_active_state |= wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE;
            }
            if(size.dropdown_region.Contains(offset))
            {
                new_active_state |= wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE;
            }
        }
        if(new_active_state != m_active_button->base->state)
        {
            m_active_button->base->state = new_active_state;
            Refresh(false);
        }
    }
}

void wxRibbonButtonBar::OnMouseDown(wxMouseEvent& evt)
{
    wxPoint cursor(evt.GetPosition());
    m_active_button = nullptr;

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_current_layout);
    size_t btn_count = layout->buttons.Count();
    size_t btn_i;
    for(btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.Item(btn_i);
        wxRibbonButtonBarButtonSizeInfo& size = instance.base->sizes[instance.size];
        wxRect btn_rect;
        btn_rect.SetTopLeft(m_layout_offset + instance.position);
        btn_rect.SetSize(size.size);
        if(btn_rect.Contains(cursor))
        {
            if((instance.base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0)
            {
                m_active_button = &instance;
                cursor -= btn_rect.GetTopLeft();
                long state = 0;
                if(size.normal_region.Contains(cursor))
                {
                    state = wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE;
                }
                else if(size.dropdown_region.Contains(cursor))
                {
                    state = wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE;
                    UnsetToolTip();
                }
                instance.base->state |= state;
                Refresh(false);
                break;
            }
        }
    }
}

void wxRibbonButtonBar::OnMouseUp(wxMouseEvent& evt)
{
    wxPoint cursor(evt.GetPosition());

    if(m_active_button)
    {
        wxRibbonButtonBarButtonSizeInfo& size =
            m_active_button->base->sizes[m_active_button->size];
        wxRect btn_rect;
        btn_rect.SetTopLeft(m_layout_offset + m_active_button->position);
        btn_rect.SetSize(size.size);
        if(btn_rect.Contains(cursor))
        {
            int id = m_active_button->base->id;
            cursor -= btn_rect.GetTopLeft();
            wxEventType event_type;
            do
            {
                if(size.normal_region.Contains(cursor))
                    event_type = wxEVT_RIBBONBUTTONBAR_CLICKED;
                else if(size.dropdown_region.Contains(cursor))
                    event_type = wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED;
                else
                    break;
                wxRibbonButtonBarEvent notification(event_type, id);
                if(m_active_button->base->kind == wxRIBBON_BUTTON_TOGGLE)
                {
                    m_active_button->base->state ^=
                        wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                    notification.SetInt(m_active_button->base->state &
                        wxRIBBON_BUTTONBAR_BUTTON_TOGGLED);
                }
                notification.SetEventObject(this);
                notification.SetBar(this);
                notification.SetButton(m_active_button->base);
                m_lock_active_state = true;
                ProcessWindowEvent(notification);
                m_lock_active_state = false;

                wxStaticCast(m_parent, wxRibbonPanel)->HideIfExpanded();
            } while(false);
            if(m_active_button) // may have been NULLed by event handler
            {
                m_active_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
                m_active_button = nullptr;
            }
            Refresh(false);
        }
    }
}

void wxRibbonButtonBar::OnMouseEnter(wxMouseEvent& evt)
{
    if(m_active_button && !evt.LeftIsDown())
    {
        m_active_button = nullptr;
    }
}

void wxRibbonButtonBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    bool repaint = false;
    if(m_hovered_button != nullptr)
    {
        m_hovered_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
        m_hovered_button = nullptr;
        repaint = true;
    }
    if(m_active_button != nullptr && !m_lock_active_state)
    {
        m_active_button->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
        repaint = true;
    }
    if(repaint)
        Refresh(false);
}

wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetActiveItem() const
{
    return m_active_button == nullptr ? nullptr : m_active_button->base;
}


wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetHoveredItem() const
{
    return m_hovered_button == nullptr ? nullptr : m_hovered_button->base;
}


wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetItem(size_t n) const
{
    wxCHECK_MSG(n < m_buttons.GetCount(), nullptr, "wxRibbonButtonBar item's index is out of bound");
    return m_buttons.Item(n);
}

wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetItemById(int button_id) const
{
    size_t count = m_buttons.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if ( button->id == button_id )
            return button;
    }

    return nullptr;

}

int wxRibbonButtonBar::GetItemId(wxRibbonButtonBarButtonBase *item) const
{
    wxCHECK_MSG(item != nullptr, wxNOT_FOUND, "wxRibbonButtonBar item should not be null");
    return item->id;
}

wxRect wxRibbonButtonBar::GetItemRect(int button_id)const
{
    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_current_layout);
    size_t btn_count = layout->buttons.Count();
    size_t btn_i;

    for (btn_i = 0; btn_i < btn_count; ++btn_i)
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.Item(btn_i);
        wxRibbonButtonBarButtonBase* button = instance.base;

        if (button->id == button_id)
        {
            wxRibbonButtonBarButtonSizeInfo& size = button->sizes[instance.size];
            wxRect btn_rect;
            btn_rect.SetTopLeft(m_layout_offset + instance.position);
            btn_rect.SetSize(size.size);

            return btn_rect;
        }
    }
    return wxRect();
}

bool wxRibbonButtonBarEvent::PopupMenu(wxMenu* menu)
{
    wxPoint pos = wxDefaultPosition;
    if(m_bar->m_active_button)
    {
        wxRibbonButtonBarButtonSizeInfo& size =
            m_bar->m_active_button->base->sizes[m_bar->m_active_button->size];
        wxRect btn_rect;
        btn_rect.SetTopLeft(m_bar->m_layout_offset +
            m_bar->m_active_button->position);
        btn_rect.SetSize(size.size);
        pos = btn_rect.GetBottomLeft();
        pos.y++;
    }
    return m_bar->PopupMenu(menu, pos);
}

#endif // wxUSE_RIBBON
