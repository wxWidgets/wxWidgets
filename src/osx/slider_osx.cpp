/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/slider_osx.cpp
// Purpose:     wxSlider
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"
#include "wx/osx/private.h"
#include "wx/utils.h"

wxBEGIN_EVENT_TABLE(wxSlider, wxControl)
wxEND_EVENT_TABLE()

namespace
{
// The dimensions of the different styles of sliders (from Aqua document)
#if wxOSX_USE_COCOA
constexpr int wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS = 28;
constexpr int wxSLIDER_DIMENSIONACROSS               = 28;
#else
constexpr int wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS = 24;
constexpr int wxSLIDER_DIMENSIONACROSS               = 18;
#endif

// Distance between slider and text
constexpr int DISTANCE_SLIDER_TEXT = 5;

// Returns the best dimension of the slider perpendicular to the slider's moving direction
int GetBestSliderAcrossDimension(wxSlider const* slider)
{
    return (slider->HasFlag(wxSL_AUTOTICKS) ?
            wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS : wxSLIDER_DIMENSIONACROSS);
}
// Returns the slider's text position perpendicular to the slider's moving direction
/*
 * When the slider's size perpendicular to the slider's moving direction becomes
 * smaller than the best size, the formula for determining the text position is
 * not valid any more. Therefore, the text is aligned under these conditions with
 * respect to the lower (horizontal slider orientation), respectively to the
 * right (vertical slider orientation) border of the slider.
 * Remarks: There is still a small glitch in calculating the position, namely when
 *          wxWidgets' calculation determines a size of zero in the perpendicular
 *          dimension. In this case the native control's dimension is still not zero
 *          because wxWidgets adds some insets. So, the slider is still visible
 *          while wxWidgets thinks it is already invisible. In this case the
 *          position calculation is also wrong as it is based on wxWidgets's data.
 */
int GetTextAcrossPosition(wxSlider const* slider, wxSize const& textDimension)
{
    const wxSize size     = slider->GetSize();
    const wxSize bestSize = slider->GetBestSize();


    if (slider->HasFlag(wxSL_VERTICAL))
    {
        if (size.x >= bestSize.x)
            return GetBestSliderAcrossDimension(slider)+DISTANCE_SLIDER_TEXT;
        else
            return wxMax(0,size.x-textDimension.x);
    }
    else if (size.y >= bestSize.y)
    {
        return GetBestSliderAcrossDimension(slider)+DISTANCE_SLIDER_TEXT;
    }
    else
    {
        return wxMax(0,size.y-textDimension.y);
    }
}

}

// NB: The default orientation for a slider is horizontal; however, if the user specifies
// some slider styles but doesn't specify the orientation we have to assume he wants a
// horizontal one. Therefore in this file when testing for the slider's orientation
// vertical is tested for if this is not set then we use the horizontal one
// e.g., if (HasFlag(wxSL_VERTICAL)) {} else { horizontal case }.

wxSlider::wxSlider()
{
    m_pageSize = 1;
    m_lineSize = 1;
    m_rangeMax = 0;
    m_rangeMin = 0;
    m_tickFreq = 0;

    m_macMinimumStatic = nullptr;
    m_macMaximumStatic = nullptr;
    m_macValueStatic = nullptr;
}

bool wxSlider::Create(wxWindow *parent,
    wxWindowID id,
    int value, int minValue, int maxValue,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxValidator& validator,
    const wxString& name)
{    
    DontCreatePeer();
    
    m_macMinimumStatic = nullptr;
    m_macMaximumStatic = nullptr;
    m_macValueStatic = nullptr;

    m_lineSize = 1;
    m_tickFreq = 0;

    m_rangeMax = maxValue;
    m_rangeMin = minValue;

    m_pageSize = (int)((maxValue - minValue) / 10);

    // our styles are redundant: wxSL_LEFT/RIGHT imply wxSL_VERTICAL and
    // wxSL_TOP/BOTTOM imply wxSL_HORIZONTAL, but for backwards compatibility
    // reasons we can't really change it, instead try to infer the orientation
    // from the flags given to us here
    switch ( style & (wxSL_LEFT | wxSL_RIGHT | wxSL_TOP | wxSL_BOTTOM) )
    {
        case wxSL_LEFT:
        case wxSL_RIGHT:
            style |= wxSL_VERTICAL;
            break;

        case wxSL_TOP:
        case wxSL_BOTTOM:
            style |= wxSL_HORIZONTAL;
            break;

        case 0:
        default:
            // no specific direction, do we have at least the orientation?
            if ( !(style & (wxSL_HORIZONTAL | wxSL_VERTICAL)) )
                // no: choose default
                style |= wxSL_BOTTOM | wxSL_HORIZONTAL;
            break;
    }

    wxASSERT_MSG( !(style & wxSL_VERTICAL) || !(style & wxSL_HORIZONTAL),
        wxT("incompatible slider direction and orientation") );

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetPeer(wxWidgetImpl::CreateSlider( this, parent, id, value, minValue, maxValue, pos, size, style, GetExtraStyle() ));

    if (style & wxSL_MIN_MAX_LABELS)
    {
        m_macMinimumStatic = new wxStaticText( parent, wxID_ANY, wxEmptyString );
        m_macMaximumStatic = new wxStaticText( parent, wxID_ANY, wxEmptyString );
    }

    if (style & wxSL_VALUE_LABEL)
    {
        m_macValueStatic = new wxStaticText( parent, wxID_ANY, wxEmptyString );
    }

    SetRange(minValue, maxValue);
    SetValue(value);

    MacPostControlCreate(pos, size);

    return true;
}

wxSlider::~wxSlider()
{
    // this is a special case, as we had to add windows as siblings we are
    // responsible for their disposal, but only if we are not part of a DestroyAllChildren
    if ( m_parent && !m_parent->IsBeingDeleted() )
    {
        delete m_macMinimumStatic;
        delete m_macMaximumStatic;
        delete m_macValueStatic;
    }
}

int wxSlider::GetValue() const
{
    // We may need to invert the value returned by the widget
    return ValueInvertOrNot( GetPeer()->GetValue() ) ;
}

void wxSlider::SetValue(int value)
{
    // We only invert for the setting of the actual native widget
    GetPeer()->SetValue( ValueInvertOrNot( value ) );

    if ( m_macValueStatic )
    {
        wxString valueString;

        // In case the passed value is outside the slider's range,
        // macOS has bound the value to a valid value;
        // use this value also for the value string
        valueString.Printf( "%d", GetValue() );
        m_macValueStatic->SetLabel( valueString );

        // center the value
        const wxSize sliderSize = GetSize();
        const wxSize valueSize  = GetTextExtent(valueString);

        if (HasFlag(wxSL_VERTICAL))
            m_macValueStatic->Move(GetPosition().x + GetTextAcrossPosition(this,valueSize), GetPosition().y + (sliderSize.GetHeight() / 2) - (valueSize.GetHeight() / 2));
        else
            m_macValueStatic->Move(GetPosition().x + (sliderSize.GetWidth() / 2) - (valueSize.GetWidth() / 2), GetPosition().y + GetTextAcrossPosition(this,valueSize));
    }
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    // Changing the range preserves the value of the native control but may
    // change our logical value if we're inverting the native value to get it
    // as ValueInvertOrNot() depends on the range so preserve it before
    // changing the range.
    const int valueOld = GetValue();

    wxString valueString;

    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    GetPeer()->SetMinimum( m_rangeMin );
    GetPeer()->SetMaximum( m_rangeMax );

    if (m_macMinimumStatic)
    {
        valueString.Printf( "%d", ValueInvertOrNot( m_rangeMin ) );
        m_macMinimumStatic->SetLabel( valueString );
    }

    if (m_macMaximumStatic)
    {
        valueString.Printf( "%d", ValueInvertOrNot( m_rangeMax ) );
        m_macMaximumStatic->SetLabel( valueString );
    }

    // Use our preserved value (see above),
    // SetValue(int) also does bounds checking, the value may
    // be changed so that it is within the new range
    SetValue(valueOld);
}

// For trackbars only
void wxSlider::DoSetTickFreq(int n)
{
    // TODO
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // TODO
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // TODO
}

void wxSlider::ClearTicks()
{
    // TODO
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    // TODO
}

int wxSlider::GetLineSize() const
{
    // TODO
    return m_lineSize;
}

int wxSlider::GetSelEnd() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelStart() const
{
    // TODO
    return 0;
}

void wxSlider::SetSelection(int WXUNUSED(minPos), int WXUNUSED(maxPos))
{
    // TODO
}

void wxSlider::SetThumbLength(int WXUNUSED(len))
{
    // TODO
}

int wxSlider::GetThumbLength() const
{
    // TODO
    return 0;
}

void wxSlider::SetTick(int WXUNUSED(tickPos))
{
    // TODO
}

void wxSlider::Command(wxCommandEvent &event)
{
    SetValue(event.GetInt());
    ProcessCommand(event);
}

void wxSlider::TriggerScrollEvent( wxEventType scrollEvent)
{
    // Whatever the native value is, we may need to invert it for calling
    // SetValue and putting the possibly inverted value in the event
    int value = ValueInvertOrNot( GetPeer()->GetValue() );

    SetValue( value );

    wxScrollEvent event( scrollEvent, m_windowId );
    event.SetPosition( value );
    event.SetEventObject( this );
    HandleWindowEvent( event );

    wxCommandEvent cevent( wxEVT_SLIDER, m_windowId );
    cevent.SetInt( value );
    cevent.SetEventObject( this );
    HandleWindowEvent( cevent );
}

bool wxSlider::OSXHandleClicked( double WXUNUSED(timestampsec) )
{
    TriggerScrollEvent(wxEVT_SCROLL_THUMBRELEASE);

    return true;
}

// This is overloaded in wxSlider so that the proper width/height will always be used
// for the slider different values would cause redrawing and mouse detection problems
//
void wxSlider::DoSetSizeHints( int minW, int minH,
    int maxW, int maxH,
    int WXUNUSED(incW), int WXUNUSED(incH) )
{
    wxSize size = GetBestSize();

    if (HasFlag(wxSL_VERTICAL))
    {
        SetMinSize( wxSize(size.x,minH) );
        SetMaxSize( wxSize(size.x,maxH) );
    }
    else
    {
        SetMinSize( wxSize(minW,size.y) );
        SetMaxSize( wxSize(maxW,size.y) );
    }
}

wxSize wxSlider::DoGetBestSize() const
{
    wxSize size;
    int textwidth, textheight;
    int mintwidth, mintheight;
    int maxtwidth, maxtheight;

    textwidth = textheight = 0;
    mintwidth = mintheight = 0;
    maxtwidth = maxtheight = 0;

    if (HasFlag(wxSL_LABELS))
    {
        wxString text;

        // Get maximum text label width and height
        text.Printf( "%d", ValueInvertOrNot( m_rangeMin ) );
        GetTextExtent(text, &mintwidth, &mintheight);
        text.Printf( "%d", ValueInvertOrNot( m_rangeMax ) );
        GetTextExtent(text, &maxtwidth, &maxtheight);

        textheight = wxMax(mintheight,maxtheight);
        textwidth  = wxMax(mintwidth, maxtwidth);
    }

    if (HasFlag(wxSL_VERTICAL))
    {
        size.x = GetBestSliderAcrossDimension(this);
        size.y = 150;

        if (HasFlag(wxSL_LABELS))
        {
            size.x += DISTANCE_SLIDER_TEXT + textwidth;
            size.y += (mintheight / 2) + (maxtheight / 2);
        }

        // to let the ticks look good the width of the control has to have an even number,
        // otherwise, the ticks are not centered with respect to the slider line
        if (HasFlag(wxSL_AUTOTICKS) && ((size.x%2) != 0))
            size.x += 1;
    }
    else
    {
        size.x = 150;
        size.y = GetBestSliderAcrossDimension(this);

        if (HasFlag(wxSL_LABELS))
        {
            size.y += DISTANCE_SLIDER_TEXT + textheight;
            size.x += (mintwidth / 2) + (maxtwidth / 2);
        }

        // to let the ticks look good the height of the control has to have an even number,
        // otherwise, the ticks are not centered with respect to the slider line
        if (HasFlag(wxSL_AUTOTICKS) && ((size.y%2) != 0))
            size.y += 1;
    }

    return size;
}

void wxSlider::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    if ( w == -1 || h == -1 ||
            (!(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) && (x == -1 || y == -1)) )
    {
        const wxRect currentRect = GetRect();
        if ( !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        {
            if ( x == -1 )
                x = currentRect.x;
            if ( y == -1 )
                y = currentRect.y;
        }

        if ( w == -1 )
            w = currentRect.width;
        if ( h == -1 )
            h = currentRect.height;
    }

    // Labels have this control's parent as their parent;
    // to adjust the labels relative to the slider
    // slider's GetPosition() method is used, therefore,
    // set the slider's position before adjusting the labels
    wxControl::DoSetSize( x, y, w, h, sizeFlags );

    if (HasFlag(wxSL_LABELS))
    {
        const int value = GetValue();

        wxSize rangeMaxSize, rangeMinSize, valueSize; // text size of the labels

        wxString text;

        // Get label dimensions
        text.Printf("%d", ValueInvertOrNot(m_rangeMin));
        rangeMinSize = GetTextExtent(text);
        text.Printf("%d", ValueInvertOrNot(m_rangeMax));
        rangeMaxSize = GetTextExtent(text);
        text.Printf("%d", value);
        valueSize = GetTextExtent(text);

        wxSize const textSize = wxSize(wxMax(rangeMaxSize.x, rangeMinSize.x), wxMax(rangeMaxSize.y, rangeMinSize.y));

        int relativeAcrossTextPosition = GetTextAcrossPosition(this,textSize);

        if (HasFlag(wxSL_VERTICAL))
        {
            if ( m_macMinimumStatic )
                m_macMinimumStatic->Move(GetPosition().x + relativeAcrossTextPosition, GetPosition().y + h - rangeMinSize.y);
            if ( m_macMaximumStatic )
                m_macMaximumStatic->Move(GetPosition().x + relativeAcrossTextPosition, GetPosition().y + 0);
            if ( m_macValueStatic )
                m_macValueStatic->Move(GetPosition().x + relativeAcrossTextPosition, GetPosition().y + (h / 2) - (valueSize.y / 2));
        }
        else
        {
            if ( m_macMinimumStatic )
                m_macMinimumStatic->Move(GetPosition().x, GetPosition().y + relativeAcrossTextPosition);
            if ( m_macMaximumStatic )
                 m_macMaximumStatic->Move(GetPosition().x + w - rangeMaxSize.x, GetPosition().y + relativeAcrossTextPosition);
            if ( m_macValueStatic )
                m_macValueStatic->Move(GetPosition().x + (w / 2) - (valueSize.x / 2), GetPosition().y + relativeAcrossTextPosition);
        }
    }
}

// Common processing to invert slider values based on wxSL_INVERSE
int wxSlider::ValueInvertOrNot(int value) const
{
    int result = 0;

    if (HasFlag(wxSL_VERTICAL))
    {
        // The reason for the backwards logic is that Mac's vertical sliders are
        // inverted compared to Windows and GTK, hence we want inversion to be the
        // default, and if wxSL_INVERSE is set, then we do not invert (use native)
        if (HasFlag(wxSL_INVERSE))
            result = value;
        else
            result = (m_rangeMax + m_rangeMin) - value;
    }
    else // normal logic applies to HORIZONTAL sliders
    {
        result = wxSliderBase::ValueInvertOrNot(value);
    }

    return result;
}

#endif // wxUSE_SLIDER
