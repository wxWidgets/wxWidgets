/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/window.h
// Purpose:     misc wxWindow helpers
// Author:      Vaclav Slavik
// Created:     2010-01-21
// Copyright:   (c) 2010 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WINDOW_H_
#define _WX_PRIVATE_WINDOW_H_

#include "wx/gdicmn.h"
#include "wx/dynlib.h"

namespace wxPrivate
{

// Windows' computes dialog units using average character width over upper-
// and lower-case ASCII alphabet and not using the average character width
// metadata stored in the font; see
// http://support.microsoft.com/default.aspx/kb/145994 for detailed discussion.
//
// This helper function computes font dimensions in the same way. It works with
// either wxDC or wxWindow argument.
template<typename T>
inline wxSize GetAverageASCIILetterSize(const T& of_what)
{
    const wxStringCharType *TEXT_TO_MEASURE =
        wxS("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    wxSize s = of_what.GetTextExtent(TEXT_TO_MEASURE);
    s.x = (s.x / 26 + 1) / 2;
    return s;
}

namespace
{

inline bool SupportsPerMonitorDPI()
{
    static bool s_checkDPI =
#if defined(__WXMSW__) && wxUSE_DYNLIB_CLASS
        // Only check the DPI when GetDpiForWindow is available because the old
        // method (GetDeviceCaps) is a lot slower (about 1500 times).
        // And when GetDpiForWindow is not available (for example older Windows
        // versions), per-monitor DPI (V2) is also not available.
        wxLoadedDLL("user32.dll").HasSymbol("GetDpiForWindow");
#else
        false;
#endif
    return s_checkDPI;
}

}

template <typename T>
class DpiDependentValue
{
public:
    // Explicit initialization is needed if T is a primitive type.
    DpiDependentValue()
        : m_value(), m_dpi()
    { }

    bool HasChanged(const wxWindowBase* win)
    {
        if ( win && SupportsPerMonitorDPI() )
        {
            const wxSize dpi = win->GetDPI();
            if ( dpi != m_dpi )
            {
                m_dpi = dpi;
                return true;
            }
        }

        // Ensure that we return true the first time we're called,
        // asuming that the value will always be set to a non-default value.
        return m_value == T();
    }

    void SetAtNewDPI(const T& value)
    {
        m_value = value;
    }

    T& Get()
    {
        return m_value;
    }

private:
    T m_value;
    wxSize m_dpi;
};

} // namespace wxPrivate

#endif // _WX_PRIVATE_WINDOW_H_
