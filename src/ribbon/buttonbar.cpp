///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/buttonbar.cpp
// Purpose:     Ribbon control similar to a tool bar
// Author:      Peter Cawley
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
    EVT_DPI_CHANGED(wxRibbonButtonBar::OnDPIChanged)
wxEND_EVENT_TABLE()

class wxRibbonButtonBarButtonSizeInfo
{
public:
    bool isSupported;
    wxSize size;
    wxRect normalRegion;
    wxRect dropdownRegion;
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
        imageIndexLarge = -1;
        imageIndexSmall = -1;
        imageIndexLargeDisabled = -1;
        imageIndexSmallDisabled = -1;
    }

    // Store bundle indices in parent's bundle vectors
    void SetBundleIndices(int largeIdx, int smallIdx, int largeDisabledIdx, int smallDisabledIdx)
    {
        imageIndexLarge = largeIdx;
        imageIndexSmall = smallIdx;
        imageIndexLargeDisabled = largeDisabledIdx;
        imageIndexSmallDisabled = smallDisabledIdx;
    }

    wxRibbonButtonBarButtonInstance NewInstance()
    {
        wxRibbonButtonBarButtonInstance i;
        i.base = this;
        return i;
    }

    wxRibbonButtonBarButtonState GetLargestSize()
    {
        if ( sizes[wxRIBBON_BUTTONBAR_BUTTON_LARGE].isSupported
           && maxSizeClass >= wxRIBBON_BUTTONBAR_BUTTON_LARGE )
        {
            return wxRIBBON_BUTTONBAR_BUTTON_LARGE;
        }
        if ( sizes[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM].isSupported
           && maxSizeClass >= wxRIBBON_BUTTONBAR_BUTTON_MEDIUM )
        {
            return wxRIBBON_BUTTONBAR_BUTTON_MEDIUM;
        }
        wxASSERT(sizes[wxRIBBON_BUTTONBAR_BUTTON_SMALL].isSupported);
        return wxRIBBON_BUTTONBAR_BUTTON_SMALL;
    }

    bool GetSmallerSize(
        wxRibbonButtonBarButtonState* size, int n = 1)
    {
        for ( ; n > 0; --n )
        {
            switch ( *size )
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                if ( sizes[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM].isSupported
                   && minSizeClass <= wxRIBBON_BUTTONBAR_BUTTON_MEDIUM )
                {
                    *size = wxRIBBON_BUTTONBAR_BUTTON_MEDIUM;
                    break;
                }
                wxFALLTHROUGH;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
                if ( sizes[wxRIBBON_BUTTONBAR_BUTTON_SMALL].isSupported
                   && minSizeClass <= wxRIBBON_BUTTONBAR_BUTTON_SMALL )
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
    wxString helpString;
    wxCoord textMinWidth[3];

    // Indices into parent wxRibbonButtonBar's wxWithImages collections
    int imageIndexLarge;
    int imageIndexSmall;
    int imageIndexLargeDisabled;
    int imageIndexSmallDisabled;

    wxRibbonButtonBarButtonSizeInfo sizes[3];
    wxRibbonButtonBarButtonState minSizeClass;
    wxRibbonButtonBarButtonState maxSizeClass;
    wxClientDataContainer clientData;
    int id;
    wxRibbonButtonKind kind;
    long state;
};

class wxRibbonButtonBarLayout
{
public:
    wxSize overallSize;
    std::vector<wxRibbonButtonBarButtonInstance> buttons;

    void CalculateOverallSize()
    {
        overallSize = wxSize(0, 0);
        for ( auto& instance : buttons )
        {
            wxSize size = instance.base->sizes[instance.size].size;
            int right = instance.position.x + size.GetWidth();
            int bottom = instance.position.y + size.GetHeight();
            if ( right > overallSize.GetWidth() )
            {
                overallSize.SetWidth(right);
            }
            if ( bottom > overallSize.GetHeight() )
            {
                overallSize.SetHeight(bottom);
            }
        }
    }

    wxRibbonButtonBarButtonInstance* FindSimilarInstance(
        wxRibbonButtonBarButtonInstance* inst)
    {
        if ( inst == nullptr )
        {
            return nullptr;
        }
        for ( auto& instance : buttons )
        {
            if ( instance.base == inst->base )
            {
                return &instance;
            }
        }
        return nullptr;
    }
};

wxRibbonButtonBar::wxRibbonButtonBar()
{
    m_layoutsValid = false;
    CommonInit(0);
}

wxRibbonButtonBar::wxRibbonButtonBar(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    m_layoutsValid = false;

    CommonInit(style);
}

wxRibbonButtonBar::~wxRibbonButtonBar()
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        delete button;
    }
    m_buttons.Clear();

    count = m_layouts.GetCount();
    for ( i = 0; i < count; ++i )
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
    if ( !wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE) )
    {
        return false;
    }

    CommonInit(style);
    return true;
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddButton(
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind)
{
    return AddButton(buttonId, label, bitmap, wxBitmapBundle(), wxBitmapBundle(),
        wxBitmapBundle(), kind, helpString);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddDropdownButton(
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return AddButton(buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_DROPDOWN);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddToggleButton(
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return AddButton(buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_TOGGLE);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddHybridButton(
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return AddButton(buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_HYBRID);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::AddButton(
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxBitmapBundle& bitmapSmall,
                const wxBitmapBundle& bitmapDisabled,
                const wxBitmapBundle& bitmapSmallDisabled,
                wxRibbonButtonKind kind,
                const wxString& helpString)
{
    return InsertButton(GetButtonCount(), buttonId, label, bitmap,
        bitmapSmall, bitmapDisabled, bitmapSmallDisabled, kind, helpString);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertButton(
                size_t pos,
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxBitmapBundle& bitmapSmall,
                const wxBitmapBundle& bitmapDisabled,
                const wxBitmapBundle& bitmapSmallDisabled,
                wxRibbonButtonKind kind,
                const wxString& helpString)
{
    wxASSERT(bitmap.IsOk() || bitmapSmall.IsOk());

    if ( m_buttons.IsEmpty() )
    {
        if ( bitmap.IsOk() )
        {
            m_bitmapSizeLarge = bitmap.GetDefaultSize();
            if ( !bitmapSmall.IsOk() )
            {
                m_bitmapSizeSmall = m_bitmapSizeLarge;
                m_bitmapSizeSmall *= 0.5;
            }
        }
        if ( bitmapSmall.IsOk() )
        {
            m_bitmapSizeSmall = bitmapSmall.GetDefaultSize();
            if ( !bitmap.IsOk() )
            {
                m_bitmapSizeLarge = m_bitmapSizeSmall;
                m_bitmapSizeLarge *= 2.0;
            }
        }
    }

    // Add bundles to our vectors and remember indices
    int idxLarge = -1;
    int idxSmall = -1;
    int idxLargeDisabled = -1;
    int idxSmallDisabled = -1;

    if ( bitmap.IsOk() )
    {
        idxLarge = m_bundlesLarge.size();
        m_bundlesLarge.push_back(bitmap);

        idxLargeDisabled = m_bundlesLargeDisabled.size();
        if ( bitmapDisabled.IsOk() )
        {
            m_bundlesLargeDisabled.push_back(bitmapDisabled);
        }
        else
        {
            // Generate disabled bitmap from normal one
            wxBitmap bmp = bitmap.GetBitmap(m_bitmapSizeLarge);
            m_bundlesLargeDisabled.push_back(wxBitmapBundle::FromBitmap(MakeDisabledBitmap(bmp)));
        }
    }

    if ( bitmapSmall.IsOk() )
    {
        idxSmall = m_bundlesSmall.size();
        m_bundlesSmall.push_back(bitmapSmall);

        idxSmallDisabled = m_bundlesSmallDisabled.size();
        if ( bitmapSmallDisabled.IsOk() )
        {
            m_bundlesSmallDisabled.push_back(bitmapSmallDisabled);
        }
        else
        {
            // Generate disabled bitmap from normal one
            wxBitmap bmp = bitmapSmall.GetBitmap(m_bitmapSizeSmall);
            m_bundlesSmallDisabled.push_back(wxBitmapBundle::FromBitmap(MakeDisabledBitmap(bmp)));
        }
    }
    else if ( bitmap.IsOk() )
    {
        // No dedicated small bitmap was provided, so derive one from the large
        // bundle. The derived bitmap must have the small default size; otherwise
        // wxBitmapBundle::GetBitmap() would keep returning bitmaps whose logical
        // size is the large default size and the "small" buttons would be drawn
        // too big.
        const wxSize sizeSmallPhys = ToPhys(FromDIP(m_bitmapSizeSmall));
        wxBitmap smallBmp = bitmap.GetBitmap(sizeSmallPhys);
        // Tag the derived bitmap with the small logical size (its scale factor
        // is simply physical/logical) so that the resulting bundle's default
        // size is correct and small buttons are not drawn at the large size.
        smallBmp.SetScaleFactor(static_cast<double>(sizeSmallPhys.y) /
                                m_bitmapSizeSmall.y);

        idxSmall = m_bundlesSmall.size();
        m_bundlesSmall.push_back(wxBitmapBundle::FromBitmap(smallBmp));

        idxSmallDisabled = m_bundlesSmallDisabled.size();
        m_bundlesSmallDisabled.push_back(
            wxBitmapBundle::FromBitmap(MakeDisabledBitmap(smallBmp)));
    }

    wxRibbonButtonBarButtonBase* base = new wxRibbonButtonBarButtonBase;
    base->id = buttonId;
    base->label = label;
    base->SetBundleIndices(idxLarge, idxSmall, idxLargeDisabled, idxSmallDisabled);
    base->kind = kind;
    base->helpString = helpString;
    base->state = 0;
    base->textMinWidth[0] = 0;
    base->textMinWidth[1] = 0;
    base->textMinWidth[2] = 0;
    base->minSizeClass = wxRIBBON_BUTTONBAR_BUTTON_SMALL;
    base->maxSizeClass = wxRIBBON_BUTTONBAR_BUTTON_LARGE;

    wxInfoDC tempDc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);

    m_buttons.Insert(base, pos);
    m_layoutsValid = false;
    return base;
}


void
wxRibbonButtonBar::SetItemClientObject(wxRibbonButtonBarButtonBase* item,
                                       wxClientData* data)
{
    wxCHECK_RET( item, "Can't associate client object with an invalid item" );

    item->clientData.SetClientObject(data);
}

wxClientData*
wxRibbonButtonBar::GetItemClientObject(const wxRibbonButtonBarButtonBase* item) const
{
    wxCHECK_MSG( item, nullptr, "Can't get client object for an invalid item" );

    return item->clientData.GetClientObject();
}

void
wxRibbonButtonBar::SetItemClientData(wxRibbonButtonBarButtonBase* item,
                                     void* data)
{
    wxCHECK_RET( item, "Can't associate client data with an invalid item" );

    item->clientData.SetClientData(data);
}

void*
wxRibbonButtonBar::GetItemClientData(const wxRibbonButtonBarButtonBase* item) const
{
    wxCHECK_MSG( item, nullptr, "Can't get client data for an invalid item" );

    return item->clientData.GetClientData();
}


wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertButton(
                size_t pos,
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind)
{
    return InsertButton(pos, buttonId, label, bitmap, wxBitmapBundle(),
        wxBitmapBundle(), wxBitmapBundle(), kind, helpString);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertDropdownButton(
                size_t pos,
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return InsertButton(pos, buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_DROPDOWN);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertToggleButton(
                size_t pos,
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return InsertButton(pos, buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_TOGGLE);
}

wxRibbonButtonBarButtonBase* wxRibbonButtonBar::InsertHybridButton(
                size_t pos,
                int buttonId,
                const wxString& label,
                const wxBitmapBundle& bitmap,
                const wxString& helpString)
{
    return InsertButton(pos, buttonId, label, bitmap, helpString,
        wxRIBBON_BUTTON_HYBRID);
}

void wxRibbonButtonBar::FetchButtonSizeInfo(wxRibbonButtonBarButtonBase* button,
        wxRibbonButtonBarButtonState size, wxReadOnlyDC& dc)
{
    wxRibbonButtonBarButtonSizeInfo& info = button->sizes[size];
    if ( m_art )
    {
        const wxSize bmpSizeLarge = FromDIP(m_bitmapSizeLarge);
        const wxSize bmpSizeSmall = FromDIP(m_bitmapSizeSmall);

        info.isSupported = m_art->GetButtonBarButtonSize(dc, this,
            button->kind, size, button->label, button->textMinWidth[size],
            bmpSizeLarge, bmpSizeSmall, &info.size,
            &info.normalRegion, &info.dropdownRegion);
    }
    else
        info.isSupported = false;
}

size_t wxRibbonButtonBar::GetButtonCount() const
{
    return m_buttons.GetCount();
}

bool wxRibbonButtonBar::Realize()
{
    if ( !m_layoutsValid )
    {
        MakeLayouts();
        m_layoutsValid = true;
    }
    return true;
}

void wxRibbonButtonBar::ClearButtons()
{
    m_layoutsValid = false;
    size_t count = m_buttons.GetCount();
    size_t i;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        delete button;
    }
    m_buttons.Clear();
    Realize();
}

bool wxRibbonButtonBar::DeleteButton(int buttonId)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if ( button->id == buttonId )
        {
            m_layoutsValid = false;
            m_buttons.RemoveAt(i);
            if ( m_hoveredButton && m_hoveredButton->base == button )
                m_hoveredButton = nullptr;
            if ( m_activeButton  && m_activeButton->base  == button )
                m_activeButton = nullptr;
            delete button;
            Realize();
            Refresh();
            return true;
        }
    }
    return false;
}

void wxRibbonButtonBar::EnableButton(int buttonId, bool enable)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if ( button->id == buttonId )
        {
            if ( enable )
            {
                if ( button->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED )
                {
                    button->state &= ~wxRIBBON_BUTTONBAR_BUTTON_DISABLED;
                    Refresh();
                }
            }
            else
            {
                if ( (button->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0 )
                {
                    button->state |= wxRIBBON_BUTTONBAR_BUTTON_DISABLED;
                    Refresh();
                }
            }
            return;
        }
    }
}

void wxRibbonButtonBar::ToggleButton(int buttonId, bool checked)
{
    size_t count = m_buttons.GetCount();
    size_t i;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if ( button->id == buttonId )
        {
            if ( checked )
            {
                if ( (button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED) == 0 )
                {
                    button->state |= wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                    Refresh();
                }
            }
            else
            {
                if ( button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED )
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
                int buttonId,
                const wxBitmapBundle& bitmap,
                const wxBitmapBundle& bitmapSmall,
                const wxBitmapBundle& bitmapDisabled,
                const wxBitmapBundle& bitmapSmallDisabled)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;

    // Update the bundles in our vectors
    if ( bitmap.IsOk() && base->imageIndexLarge >= 0 )
    {
        m_bundlesLarge[base->imageIndexLarge] = bitmap;

        if ( bitmapDisabled.IsOk() )
        {
            m_bundlesLargeDisabled[base->imageIndexLargeDisabled] = bitmapDisabled;
        }
        else
        {
            wxBitmap bmp = bitmap.GetBitmap(m_bitmapSizeLarge);
            m_bundlesLargeDisabled[base->imageIndexLargeDisabled] =
                wxBitmapBundle::FromBitmap(MakeDisabledBitmap(bmp));
        }
    }

    if ( bitmapSmall.IsOk() && base->imageIndexSmall >= 0 )
    {
        m_bundlesSmall[base->imageIndexSmall] = bitmapSmall;

        if ( bitmapSmallDisabled.IsOk() )
        {
            m_bundlesSmallDisabled[base->imageIndexSmallDisabled] = bitmapSmallDisabled;
        }
        else
        {
            wxBitmap bmp = bitmapSmall.GetBitmap(m_bitmapSizeSmall);
            m_bundlesSmallDisabled[base->imageIndexSmallDisabled] =
                wxBitmapBundle::FromBitmap(MakeDisabledBitmap(bmp));
        }
    }

    Refresh();
}

void wxRibbonButtonBar::SetButtonText(int buttonId, const wxString& label)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;
    base->label = label;

    wxInfoDC tempDc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);
    m_layoutsValid = false;
    Refresh();
}

void wxRibbonButtonBar::SetButtonTextMinWidth(int buttonId,
                int minWidthMedium, int minWidthLarge)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;
    base->textMinWidth[0] = 0;
    base->textMinWidth[1] = minWidthMedium;
    base->textMinWidth[2] = minWidthLarge;
    wxInfoDC tempDc(this);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);
    m_layoutsValid = false;
}

void wxRibbonButtonBar::SetButtonTextMinWidth(
                int buttonId, const wxString& label)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;
    wxInfoDC tempDc(this);
    base->textMinWidth[wxRIBBON_BUTTONBAR_BUTTON_MEDIUM] =
        m_art->GetButtonBarButtonTextWidth(
        tempDc, label, base->kind, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
    base->textMinWidth[wxRIBBON_BUTTONBAR_BUTTON_LARGE] =
        m_art->GetButtonBarButtonTextWidth(
        tempDc, label, base->kind, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
    FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);
    m_layoutsValid = false;
}

void wxRibbonButtonBar::SetButtonMinSizeClass(int buttonId,
                wxRibbonButtonBarButtonState minSizeClass)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;
    if ( base->maxSizeClass < minSizeClass )
    {
        wxFAIL_MSG("Button minimum size is larger than maximum size");
        return;
    }
    base->minSizeClass = minSizeClass;
    m_layoutsValid = false;
}

void wxRibbonButtonBar::SetButtonMaxSizeClass(int buttonId,
                wxRibbonButtonBarButtonState maxSizeClass)
{
    wxRibbonButtonBarButtonBase* base = GetItemById(buttonId);
    if ( base == nullptr )
        return;
    if ( base->minSizeClass > maxSizeClass )
    {
        wxFAIL_MSG("Button maximum size is smaller than minimum size");
        return;
    }
    base->maxSizeClass = maxSizeClass;
    m_layoutsValid = false;
}

void wxRibbonButtonBar::SetArtProvider(wxRibbonArtProvider* art)
{
    if ( art == m_art )
    {
        return;
    }

    wxRibbonControl::SetArtProvider(art);

    // There is no need to do anything else when the art provider is reset to
    // null during our destruction and this actually results in problems during
    // program shutdown due to trying to get DPI of the already destroyed TLW
    // parent.
    if ( !art )
        return;

    wxInfoDC tempDc(this);
    size_t btnCount = m_buttons.Count();
    size_t btnI;
    for ( btnI = 0; btnI < btnCount; ++btnI )
    {
        wxRibbonButtonBarButtonBase* base = m_buttons.Item(btnI);

        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);
    }

    m_layoutsValid = false;
    Realize();
}

bool wxRibbonButtonBar::IsSizingContinuous() const
{
    return false;
}

wxSize wxRibbonButtonBar::DoGetNextSmallerSize(wxOrientation direction,
                                             wxSize result) const
{
    size_t nLayouts = m_layouts.GetCount();
    size_t i;
    for ( i = 0; i < nLayouts; ++i )
    {
        wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
        wxSize size = layout->overallSize;
        switch ( direction )
        {
        case wxHORIZONTAL:
            if ( size.x < result.x && size.y <= result.y )
            {
                result.x = size.x;
                break;
            }
            else
                continue;
        case wxVERTICAL:
            if ( size.x <= result.x && size.y < result.y )
            {
                result.y = size.y;
                break;
            }
            else
                continue;
        case wxBOTH:
            if ( size.x < result.x && size.y < result.y )
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
    size_t nLayouts = m_layouts.GetCount();
    size_t i = nLayouts;
    while ( i > 0 )
    {
        --i;
        wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
        wxSize size = layout->overallSize;
        switch ( direction )
        {
        case wxHORIZONTAL:
            if ( size.x > result.x && size.y <= result.y )
            {
                result.x = size.x;
                break;
            }
            else
                continue;
        case wxVERTICAL:
            if ( size.x <= result.x && size.y > result.y )
            {
                result.y = size.y;
                break;
            }
            else
                continue;
        case wxBOTH:
            if ( size.x > result.x && size.y > result.y )
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

    size_t btnCount = m_buttons.size();
    bool rerealize = false;
    for ( size_t btnI = 0; btnI < btnCount; ++btnI )
    {
        wxRibbonButtonBarButtonBase& btn = *m_buttons.Item(btnI);
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

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_currentLayout);

    // Calculate DPI-scaled bitmap sizes (must match layout calculation)
    const double scale = GetDPIScaleFactor();
    const wxSize scaledLarge = m_bitmapSizeLarge * scale;
    const wxSize scaledSmall = m_bitmapSizeSmall * scale;

    for ( auto& button : layout->buttons )
    {
        wxRibbonButtonBarButtonBase* base = button.base;
        wxRect rect(button.position + m_layoutOffset, base->sizes[button.size].size);

        wxBitmap bitmap, bitmapSmall;

        bool disabled = (base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) != 0;

        if ( base->imageIndexLarge >= 0 )
        {
            int idx = disabled ? base->imageIndexLargeDisabled : base->imageIndexLarge;
            if ( idx >= 0 && idx < (int)m_bundlesLarge.size() )
            {
                const wxBitmapBundle& bundle = disabled ?
                    m_bundlesLargeDisabled[idx] : m_bundlesLarge[idx];
                bitmap = bundle.GetBitmap(scaledLarge);
            }
        }

        if ( base->imageIndexSmall >= 0 )
        {
            int idx = disabled ? base->imageIndexSmallDisabled : base->imageIndexSmall;
            if ( idx >= 0 && idx < (int)m_bundlesSmall.size() )
            {
                const wxBitmapBundle& bundle = disabled ?
                    m_bundlesSmallDisabled[idx] : m_bundlesSmall[idx];
                bitmapSmall = bundle.GetBitmap(scaledSmall);
            }
        }

        m_art->DrawButtonBarButton(dc, this, rect, base->kind,
            base->state | button.size, base->label, bitmap, bitmapSmall);
     }
}

wxBitmap wxRibbonButtonBar::GetButtonBitmap(int imageIndex, bool large) const
{
    if ( imageIndex < 0 )
        return wxNullBitmap;

    const wxVector<wxBitmapBundle>& bundles = large ? m_bundlesLarge : m_bundlesSmall;
    if ( imageIndex >= (int)bundles.size() )
        return wxNullBitmap;

    return bundles[imageIndex].GetBitmapFor(this);
}

void wxRibbonButtonBar::OnSize(wxSizeEvent& evt)
{
    wxSize newSize = evt.GetSize();
    size_t layoutCount = m_layouts.GetCount();
    size_t layoutI;
    m_currentLayout = layoutCount - 1;
    for ( layoutI = 0; layoutI < layoutCount; ++layoutI )
    {
        wxSize layoutSize = m_layouts.Item(layoutI)->overallSize;
        if ( layoutSize.x <= newSize.x && layoutSize.y <= newSize.y )
        {
            m_layoutOffset.x = (newSize.x - layoutSize.x) / 2;
            m_layoutOffset.y = (newSize.y - layoutSize.y) / 2;
            m_currentLayout = layoutI;
            break;
        }
    }
    m_hoveredButton = m_layouts.Item(m_currentLayout)->FindSimilarInstance(m_hoveredButton);
    Refresh();
}

void wxRibbonButtonBar::CommonInit(long WXUNUSED(style))
{
    // This can initialize it to nullptr when we're called from the default ctor,
    // but will set it to the correct value when used from non-default ctor or
    // Create() later.
    m_ribbonBar = GetAncestorRibbonBar();

    m_bitmapSizeLarge = wxSize(32, 32);
    m_bitmapSizeSmall = wxSize(16, 16);

    wxRibbonButtonBarLayout* placeholderLayout = new wxRibbonButtonBarLayout;
    placeholderLayout->overallSize = wxSize(20, 20);
    m_layouts.Add(placeholderLayout);
    m_currentLayout = 0;
    m_layoutOffset = wxPoint(0, 0);
    m_hoveredButton = nullptr;
    m_activeButton = nullptr;
    m_lockActiveState = false;
    m_showTooltipsForDisabled = false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void wxRibbonButtonBar::SetShowToolTipsForDisabled(bool show)
{
    m_showTooltipsForDisabled = show;
}

bool wxRibbonButtonBar::GetShowToolTipsForDisabled() const
{
    return m_showTooltipsForDisabled;
}

wxSize wxRibbonButtonBar::GetMinSize() const
{
    return m_layouts.Last()->overallSize;
}

wxSize wxRibbonButtonBar::DoGetBestSize() const
{
    return m_layouts.Item(0)->overallSize;
}

void wxRibbonButtonBar::MakeLayouts()
{
    if ( m_layoutsValid || m_art == nullptr )
    {
        return;
    }
    {
        // Clear existing layouts
        if ( m_hoveredButton )
        {
            m_hoveredButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
            m_hoveredButton = nullptr;
        }
        if ( m_activeButton )
        {
            m_activeButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
            m_activeButton = nullptr;
        }
        size_t count = m_layouts.GetCount();
        size_t i;
        for ( i = 0; i < count; ++i )
        {
            wxRibbonButtonBarLayout* layout = m_layouts.Item(i);
            delete layout;
        }
        m_layouts.Clear();
    }
    size_t btnCount = m_buttons.Count();
    size_t btnI;

    // Determine available height:
    // 1 large button or, if not found, 3 medium or small buttons
    int availableHeight = 0;
    bool largeButtonFound = false;
    for ( btnI = 0; btnI < btnCount; ++btnI )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(btnI);
        wxRibbonButtonBarButtonState sizeClass = button->GetLargestSize();
        availableHeight = wxMax(availableHeight,
                                 button->sizes[sizeClass].size.GetHeight());
        if ( sizeClass == wxRIBBON_BUTTONBAR_BUTTON_LARGE )
            largeButtonFound = true;
    }
    if ( !largeButtonFound )
        availableHeight *= 3;

    int stackedWidth = 0;
    {
        // Best layout : all buttons large, stacking horizontally,
        //               small buttons small, stacked vertically
        wxRibbonButtonBarLayout* layout = new wxRibbonButtonBarLayout;
        wxPoint cursor(0, 0);
        for ( btnI = 0; btnI < btnCount; ++btnI )
        {
            wxRibbonButtonBarButtonBase* button = m_buttons.Item(btnI);
            wxRibbonButtonBarButtonInstance instance = button->NewInstance();
            instance.position = cursor;
            instance.size = button->GetLargestSize();
            wxSize& size = button->sizes[instance.size].size;

            if ( instance.size < wxRIBBON_BUTTONBAR_BUTTON_LARGE )
            {
                stackedWidth = wxMax(stackedWidth, size.GetWidth());
                if ( cursor.y + size.GetHeight() >= availableHeight )
                {
                    cursor.y = 0;
                    cursor.x += stackedWidth;
                    stackedWidth = 0;
                }
                else
                {
                    cursor.y += size.GetHeight();
                }
            }
            else
            {
                if ( cursor.y != 0 )
                {
                    cursor.y = 0;
                    cursor.x += stackedWidth;
                    stackedWidth = 0;
                    instance.position = cursor;
                }
                cursor.x += size.GetWidth();
            }
            layout->buttons.push_back(instance);
        }
        layout->CalculateOverallSize();
        m_layouts.Add(layout);
    }
    if ( btnCount >= 2 )
    {
        // Collapse the rightmost buttons and stack them vertically
        // if they are not already small. If rightmost buttons can't
        // be collapsed because "minSizeClass" is set, try it again
        // starting from second rightmost button and so on.
        size_t iLast = btnCount;
        while ( iLast-- > 0 )
        {
            TryCollapseLayout(m_layouts.Last(), iLast, &iLast,
                              wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
        }

        iLast = btnCount;
        while ( iLast-- > 0 )
        {
            TryCollapseLayout(m_layouts.Last(), iLast, &iLast,
                              wxRIBBON_BUTTONBAR_BUTTON_SMALL);
        }
    }
}

void wxRibbonButtonBar::TryCollapseLayout(wxRibbonButtonBarLayout* original,
                                          size_t firstBtn, size_t* lastButton,
                                          wxRibbonButtonBarButtonState targetSize)
{
    size_t btnCount = m_buttons.Count();
    size_t btnI;
    int usedHeight = 0;
    int usedWidth = 0;
    int originalColumnWidth = 0;
    int availableWidth = 0;
    int availableHeight = original->overallSize.GetHeight();

    // Search for button range from right which should be
    // collapsed into a column of small buttons.
    for ( btnI = firstBtn + 1; btnI > 0; /* decrement is inside loop */ )
    {
        --btnI;
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(btnI);
        wxRibbonButtonBarButtonState largeSizeClass = button->GetLargestSize();
        wxSize largeSize = button->sizes[largeSizeClass].size;
        int tAvailableWidth = availableWidth;

        originalColumnWidth = wxMax(originalColumnWidth,
                                      largeSize.GetWidth());

        // Top button in column: add column width to available width
        if ( original->buttons.at(btnI).position.y == 0 )
        {
            tAvailableWidth += originalColumnWidth;
            originalColumnWidth = 0;
        }

        wxRibbonButtonBarButtonState smallSizeClass = largeSizeClass;
        if ( largeSizeClass > targetSize )
        {
            if ( !button->GetSmallerSize(&smallSizeClass,
                                       smallSizeClass - targetSize) )
            {
                // Large button that cannot shrink: stop search
                ++btnI;
                break;
            }
        }
        wxSize smallSize = button->sizes[smallSizeClass].size;
        int tUsedHeight = usedHeight + smallSize.GetHeight();
        int tUsedWidth = wxMax(usedWidth, smallSize.GetWidth());

        // Height is full: stop search
        if ( tUsedHeight > availableHeight )
        {
            ++btnI;
            break;
        }
        else
        {
            usedHeight = tUsedHeight;
            usedWidth = tUsedWidth;
            availableWidth = tAvailableWidth;
        }
    }

    // Layout got wider than before or no suitable button found: abort
    if ( btnI >= firstBtn || usedWidth >= availableWidth )
    {
        return;
    }
    if ( lastButton != nullptr )
    {
        *lastButton = btnI;
    }

    wxRibbonButtonBarLayout* layout = new wxRibbonButtonBarLayout;
    WX_APPEND_ARRAY(layout->buttons, original->buttons);
    wxPoint cursor(layout->buttons.at(btnI).position);

    cursor.y = 0;
    for ( ; btnI <= firstBtn; ++btnI )
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.at(btnI);
        if ( instance.size > targetSize )
        {
            instance.base->GetSmallerSize(&instance.size,
                                          instance.size - targetSize);
        }
        instance.position = cursor;
        cursor.y += instance.base->sizes[instance.size].size.GetHeight();
    }

    int xAdjust = availableWidth - usedWidth;

    // Adjust x coords of buttons right of shrinked column
    for ( ; btnI < btnCount; ++btnI )
    {
        wxRibbonButtonBarButtonInstance& instance = layout->buttons.at(btnI);
        instance.position.x -= xAdjust;
    }

    layout->CalculateOverallSize();

    // Sanity check
    if ( layout->overallSize.GetWidth() >= original->overallSize.GetWidth() ||
        layout->overallSize.GetHeight() > original->overallSize.GetHeight() )
    {
        delete layout;
        wxFAIL_MSG("Layout collapse resulted in increased size");
        return;
    }

    // If height isn't preserved (i.e. it is reduced), then the minimum
    // size for the button bar will decrease, preventing the original
    // layout from being used (in some cases).
    // If neither "minSizeClass" nor "maxSizeClass" is set, this is
    // only required when the first button is involved in a collapse but
    // if small, medium and large buttons as well as min/max size classes
    // are involved this is always a good idea.
    layout->overallSize.SetHeight(original->overallSize.GetHeight());

    m_layouts.Add(layout);
}

void wxRibbonButtonBar::OnMouseMove(wxMouseEvent& evt)
{
    wxPoint cursor(evt.GetPosition());
    wxRibbonButtonBarButtonInstance* newHovered = nullptr;
    wxRibbonButtonBarButtonInstance* tooltipButton = nullptr;
    long newHoveredState = 0;

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_currentLayout);
    for ( auto& instance : layout->buttons )
    {
        wxRibbonButtonBarButtonSizeInfo& size = instance.base->sizes[instance.size];
        wxRect btnRect;
        btnRect.SetTopLeft(m_layoutOffset + instance.position);
        btnRect.SetSize(size.size);
        if ( btnRect.Contains(cursor) )
        {
            if ( (instance.base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0 )
            {
                tooltipButton = &instance;
                newHovered = &instance;
                newHoveredState = instance.base->state;
                newHoveredState &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
                wxPoint offset(cursor);
                offset -= btnRect.GetTopLeft();
                if ( size.normalRegion.Contains(offset) )
                {
                    newHoveredState |= wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED;
                }
                if ( size.dropdownRegion.Contains(offset) )
                {
                    newHoveredState |= wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_HOVERED;
                }
                break;
            }
            else if ( m_showTooltipsForDisabled )
            {
                tooltipButton = &instance;
            }
        }
    }

#if wxUSE_TOOLTIPS
    if ( tooltipButton == nullptr && GetToolTip() )
    {
        UnsetToolTip();
    }
    if ( tooltipButton )
    {
        if ( tooltipButton != m_hoveredButton &&
                !(tooltipButton->size & wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE) )
            SetToolTip(tooltipButton->base->helpString);
    }
#else
    wxUnusedVar(tooltipButton);
#endif

    if ( newHovered != m_hoveredButton || (m_hoveredButton != nullptr &&
        newHoveredState != m_hoveredButton->base->state) )
    {
        if ( m_hoveredButton != nullptr )
        {
            m_hoveredButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
        }
        m_hoveredButton = newHovered;
        if ( m_hoveredButton != nullptr )
        {
            m_hoveredButton->base->state = newHoveredState;
        }
        Refresh(false);
    }

    if ( m_activeButton && !m_lockActiveState )
    {
        long newActiveState = m_activeButton->base->state;
        newActiveState &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
        wxRibbonButtonBarButtonSizeInfo& size =
            m_activeButton->base->sizes[m_activeButton->size];
        wxRect btnRect;
        btnRect.SetTopLeft(m_layoutOffset + m_activeButton->position);
        btnRect.SetSize(size.size);
        if ( btnRect.Contains(cursor) )
        {
            wxPoint offset(cursor);
            offset -= btnRect.GetTopLeft();
            if ( size.normalRegion.Contains(offset) )
            {
                newActiveState |= wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE;
            }
            if ( size.dropdownRegion.Contains(offset) )
            {
                newActiveState |= wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE;
            }
        }
        if ( newActiveState != m_activeButton->base->state )
        {
            m_activeButton->base->state = newActiveState;
            Refresh(false);
        }
    }
}

void wxRibbonButtonBar::OnMouseDown(wxMouseEvent& evt)
{
    wxPoint cursor(evt.GetPosition());
    m_activeButton = nullptr;

    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_currentLayout);
    for ( auto& instance : layout->buttons )
    {
        wxRibbonButtonBarButtonSizeInfo& size = instance.base->sizes[instance.size];
        wxRect btnRect;
        btnRect.SetTopLeft(m_layoutOffset + instance.position);
        btnRect.SetSize(size.size);
        if ( btnRect.Contains(cursor) )
        {
            if ( (instance.base->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) == 0 )
            {
                m_activeButton = &instance;
                cursor -= btnRect.GetTopLeft();
                long state = 0;
                if ( size.normalRegion.Contains(cursor) )
                {
                    state = wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE;
                }
                else if ( size.dropdownRegion.Contains(cursor) )
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

    if ( m_activeButton )
    {
        wxRibbonButtonBarButtonSizeInfo& size =
            m_activeButton->base->sizes[m_activeButton->size];
        wxRect btnRect;
        btnRect.SetTopLeft(m_layoutOffset + m_activeButton->position);
        btnRect.SetSize(size.size);
        if ( btnRect.Contains(cursor) )
        {
            int id = m_activeButton->base->id;
            cursor -= btnRect.GetTopLeft();
            wxEventType eventType;
            do
            {
                if ( size.normalRegion.Contains(cursor) )
                    eventType = wxEVT_RIBBONBUTTONBAR_CLICKED;
                else if ( size.dropdownRegion.Contains(cursor) )
                    eventType = wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED;
                else
                    break;
                wxRibbonButtonBarEvent notification(eventType, id);
                if ( m_activeButton->base->kind == wxRIBBON_BUTTON_TOGGLE )
                {
                    m_activeButton->base->state ^=
                        wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                    notification.SetInt(m_activeButton->base->state &
                        wxRIBBON_BUTTONBAR_BUTTON_TOGGLED);
                }
                notification.SetEventObject(this);
                notification.SetBar(this);
                notification.SetButton(m_activeButton->base);
                m_lockActiveState = true;
                ProcessWindowEvent(notification);
                m_lockActiveState = false;

                wxStaticCast(m_parent, wxRibbonPanel)->HideIfExpanded();
            } while ( false );
            if ( m_activeButton ) // may have been NULLed by event handler
            {
                m_activeButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
                m_activeButton = nullptr;
            }
            Refresh(false);
        }
    }
}

void wxRibbonButtonBar::OnMouseEnter(wxMouseEvent& evt)
{
    if ( m_activeButton && !evt.LeftIsDown() )
    {
        m_activeButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
        m_activeButton = nullptr;
    }
}

void wxRibbonButtonBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    bool repaint = false;
    if ( m_hoveredButton != nullptr )
    {
        m_hoveredButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK;
        m_hoveredButton = nullptr;
        repaint = true;
    }
    if ( m_activeButton != nullptr && !m_lockActiveState )
    {
        m_activeButton->base->state &= ~wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
        repaint = true;
    }
    if ( repaint )
        Refresh(false);
}

wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetActiveItem() const
{
    return m_activeButton == nullptr ? nullptr : m_activeButton->base;
}


wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetHoveredItem() const
{
    return m_hoveredButton == nullptr ? nullptr : m_hoveredButton->base;
}


wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetItem(size_t n) const
{
    wxCHECK_MSG(n < m_buttons.GetCount(), nullptr, "wxRibbonButtonBar item's index is out of bound");
    return m_buttons.Item(n);
}

wxRibbonButtonBarButtonBase *wxRibbonButtonBar::GetItemById(int buttonId) const
{
    size_t count = m_buttons.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxRibbonButtonBarButtonBase* button = m_buttons.Item(i);
        if ( button->id == buttonId )
            return button;
    }

    return nullptr;

}

int wxRibbonButtonBar::GetItemId(wxRibbonButtonBarButtonBase *item) const
{
    wxCHECK_MSG(item != nullptr, wxNOT_FOUND, "wxRibbonButtonBar item should not be null");
    return item->id;
}

wxRect wxRibbonButtonBar::GetItemRect(int buttonId) const
{
    wxRibbonButtonBarLayout* layout = m_layouts.Item(m_currentLayout);
    for ( auto& instance : layout->buttons )
    {
        wxRibbonButtonBarButtonBase* button = instance.base;

        if ( button->id == buttonId )
        {
            wxRibbonButtonBarButtonSizeInfo& size = button->sizes[instance.size];
            wxRect btnRect;
            btnRect.SetTopLeft(m_layoutOffset + instance.position);
            btnRect.SetSize(size.size);

            return btnRect;
        }
    }
    return wxRect();
}

bool wxRibbonButtonBarEvent::PopupMenu(wxMenu* menu)
{
    wxPoint pos = wxDefaultPosition;
    if ( m_bar->m_activeButton )
    {
        wxRibbonButtonBarButtonSizeInfo& size =
            m_bar->m_activeButton->base->sizes[m_bar->m_activeButton->size];
        wxRect btnRect;
        btnRect.SetTopLeft(m_bar->m_layoutOffset +
            m_bar->m_activeButton->position);
        btnRect.SetSize(size.size);
        pos = btnRect.GetBottomLeft();
        pos.y++;
    }
    return m_bar->PopupMenu(menu, pos);
}

void wxRibbonButtonBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    // Recalculate all button sizes with new DPI scale, then rebuild layouts
    wxInfoDC tempDc(this);
    size_t btnCount = m_buttons.GetCount();
    for ( size_t btnI = 0; btnI < btnCount; ++btnI )
    {
        wxRibbonButtonBarButtonBase* base = m_buttons.Item(btnI);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_SMALL, tempDc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, tempDc);
        FetchButtonSizeInfo(base, wxRIBBON_BUTTONBAR_BUTTON_LARGE, tempDc);
    }

    m_layoutsValid = false;
    Realize();
    Refresh();
    event.Skip();
}

#endif // wxUSE_RIBBON
