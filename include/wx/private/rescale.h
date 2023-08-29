///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/rescale.h
// Purpose:     Helpers for rescaling coordinates
// Author:      Vadim Zeitlin
// Created:     2021-07-13
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_RESCALE_H_
#define _WX_PRIVATE_RESCALE_H_

#include "wx/gdicmn.h"
#include "wx/math.h"

#ifdef __WINDOWS__
    // Required in order to use wxMulDivInt32().
    #include "wx/msw/wrapwin.h"
#endif

// wxRescaleCoord is used to scale the components of the given wxSize by the
// ratio between 2 scales, with rounding. It doesn't scale the components
// if they're set to -1 (wxDefaultCoord), as this value is special in wxSize.
//
// The way it's used is special because we want to ensure there is no confusion
// between the scale being converted from and the scale being converted to, so
// instead of just using a single function, we use an intermediate object,
// which is not supposed to be used directly, but is only returned by From() in
// order to allow calling To() on it.
//
// Another complication is that we want this to work for both wxSize and
// wxPoint, as well as for just plain coordinate values, so wxRescaleCoord() is
// an overloaded function and the helper classes are templates, with their
// template parameter being either wxSize, wxPoint or int.

namespace wxPrivate
{

template <typename T> class wxRescaleCoordWithValue;

template <typename T>
class wxRescaleCoordWithFrom
{
public:
    T To(wxSize newScale) const
    {
        T value(m_value);

        if ( value.x != wxDefaultCoord )
            value.x = wxMulDivInt32(value.x, newScale.x, m_oldScale.x);

        if ( value.y != wxDefaultCoord )
            value.y = wxMulDivInt32(value.y, newScale.y, m_oldScale.y);

        return value;
    }

    T To(int newScaleX, int newScaleY) const
    {
        return To(wxSize(newScaleX, newScaleY));
    }

private:
    wxRescaleCoordWithFrom(T value, wxSize oldScale)
        : m_value(value), m_oldScale(oldScale)
    {
    }

    const T m_value;
    const wxSize m_oldScale;

    // Only it can create objects of this class.
    friend class wxRescaleCoordWithValue<T>;
};

// Specialization for just a single value.
template <>
class wxRescaleCoordWithFrom<int>
{
public:
    int To(wxSize newScale) const
    {
        return m_value == wxDefaultCoord
                ? wxDefaultCoord
                : wxMulDivInt32(m_value, newScale.x, m_oldScale.x);
    }

private:
    wxRescaleCoordWithFrom(int value, wxSize oldScale)
        : m_value(value), m_oldScale(oldScale)
    {
    }

    const int m_value;
    const wxSize m_oldScale;

    // Only it can create objects of this class.
    friend class wxRescaleCoordWithValue<int>;
};

template <typename T>
class wxRescaleCoordWithValue
{
public:
    explicit wxRescaleCoordWithValue(T value)
        : m_value(value)
    {
    }

    wxRescaleCoordWithFrom<T> From(wxSize oldScale)
    {
        return wxRescaleCoordWithFrom<T>(m_value, oldScale);
    }

    wxRescaleCoordWithFrom<T> From(int oldScaleX, int oldScaleY)
    {
        return From(wxSize(oldScaleX, oldScaleY));
    }

private:
    const T m_value;
};

} // namespace wxPrivate

inline wxPrivate::wxRescaleCoordWithValue<int> wxRescaleCoord(int coord)
{
    return wxPrivate::wxRescaleCoordWithValue<int>(coord);
}

inline wxPrivate::wxRescaleCoordWithValue<wxSize> wxRescaleCoord(wxSize sz)
{
    return wxPrivate::wxRescaleCoordWithValue<wxSize>(sz);
}

inline wxPrivate::wxRescaleCoordWithValue<wxSize> wxRescaleCoord(int x, int y)
{
    return wxPrivate::wxRescaleCoordWithValue<wxSize>(wxSize(x, y));
}

inline wxPrivate::wxRescaleCoordWithValue<wxPoint> wxRescaleCoord(wxPoint pt)
{
    return wxPrivate::wxRescaleCoordWithValue<wxPoint>(pt);
}

#endif // _WX_PRIVATE_RESCALE_H_
