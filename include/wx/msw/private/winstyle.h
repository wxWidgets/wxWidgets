///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/winstyle.h
// Purpose:     Small helper class for updating MSW windows style
// Author:      Vadim Zeitlin
// Created:     2017-12-09
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_WINSTYLE_H_
#define _WX_MSW_PRIVATE_WINSTYLE_H_

// ----------------------------------------------------------------------------
// wxMSWWinLongUpdater
// ----------------------------------------------------------------------------

/*
    This class is not used directly, but either as wxMSWWinStyleUpdater or
    wxMSWWinExStyleUpdater, both of which inherit from it and can be used like
    this:

    void SomeFunction()
    {
        wxMSWWinStyleUpdater updateStyle(GetHwndOf(m_win));
        if ( some-condition )
            updateStyle.TurnOn(XX_YYY);

        // Style update happens here implicitly -- or call Apply().
    }
 */
class wxMSWWinLongUpdater
{
public:
    // Get the current style.
    LONG_PTR Get() const
    {
        return m_styleCurrent;
    }

    // Check if the given style bit(s) is (are all) currently turned on.
    bool IsOn(LONG_PTR style) const
    {
        return (m_styleCurrent & style) == style;
    }

    // Turn on some bit(s) in the style.
    wxMSWWinLongUpdater& TurnOn(LONG_PTR on)
    {
        m_style |= on;
        return *this;
    }

    // Turn off some bit(s) in the style.
    wxMSWWinLongUpdater& TurnOff(LONG_PTR off)
    {
        m_style &= ~off;
        return *this;
    }

    // Turn some bit(s) on or off depending on the condition.
    wxMSWWinLongUpdater& TurnOnOrOff(bool cond, LONG_PTR style)
    {
        return cond ? TurnOn(style) : TurnOff(style);
    }

    // Perform the style update (only if necessary, i.e. if the style really
    // changed).
    //
    // Notice that if this method is not called, it's still done from the dtor,
    // so it's just a convenient way to do it sooner and avoid having to create
    // a new scope for ensuring that the dtor runs at the right place, but
    // otherwise is equivalent to do this.
    bool Apply()
    {
        if ( m_style == m_styleCurrent )
            return false;

        ::SetWindowLongPtr(m_hwnd, m_gwlSlot, m_style);

        m_styleCurrent = m_style;

        return true;
    }

    ~wxMSWWinLongUpdater()
    {
        Apply();
    }

protected:
    // Create the object for updating the style or extended style of the given
    // window.
    //
    // Ctor is protected, this class can only be used as wxMSWWinStyleUpdater
    // or wxMSWWinExStyleUpdater.
    wxMSWWinLongUpdater(HWND hwnd, int gwlSlot)
        : m_hwnd(hwnd),
          m_gwlSlot(gwlSlot),
          m_styleCurrent(::GetWindowLongPtr(hwnd, gwlSlot)),
          m_style(m_styleCurrent)
    {
    }

private:
    const HWND m_hwnd;
    const int m_gwlSlot;

    LONG_PTR m_styleCurrent;
    LONG_PTR m_style;

    wxDECLARE_NO_COPY_CLASS(wxMSWWinLongUpdater);
};

// A variant of wxMSWWinLongUpdater which updates the extended style.
class wxMSWWinStyleUpdater : public wxMSWWinLongUpdater
{
public:
    explicit wxMSWWinStyleUpdater(HWND hwnd)
        : wxMSWWinLongUpdater(hwnd, GWL_STYLE)
    {
    }
};

// A variant of wxMSWWinLongUpdater which updates the extended style.
class wxMSWWinExStyleUpdater : public wxMSWWinLongUpdater
{
public:
    explicit wxMSWWinExStyleUpdater(HWND hwnd)
        : wxMSWWinLongUpdater(hwnd, GWL_EXSTYLE)
    {
    }
};

#endif // _WX_MSW_PRIVATE_WINSTYLE_H_
