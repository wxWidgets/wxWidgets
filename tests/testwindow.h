///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testwindow.h
// Purpose:     Unit test helper for comparing wxWindow pointers.
// Author:      Vadim Zeitlin
// Copyright:   (c) 2019 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TESTWINDOW_H_
#define _WX_TESTS_TESTWINDOW_H_

#include "wx/window.h"

// We need to wrap wxWindow* in a class as specializing StringMaker for
// wxWindow* doesn't seem to work.
class wxWindowPtr
{
public:
    explicit wxWindowPtr(wxWindow* win) : m_win(win) {}

    wxString Dump() const
    {
        if ( !m_win )
            return "(no window)";

        wxString s = m_win->GetClassInfo()->GetClassName();
        const wxString& label = m_win->GetLabel();
        if ( !label.empty() )
        {
            s += wxString::Format(" (label=\"%s\")", label);
        }

        return s;
    }

private:
    friend bool operator==(wxWindowPtr wp1, wxWindowPtr wp2)
    {
        return wp1.m_win == wp2.m_win;
    }

    wxWindow* const m_win;
};

// Macro providing more information about the current focus if comparison
// fails.
#define CHECK_FOCUS_IS(w) CHECK(wxWindowPtr(wxWindow::FindFocus()) == wxWindowPtr(w))

namespace Catch
{
    template <>
    struct StringMaker<wxWindowPtr>
    {
        static std::string convert(const wxWindowPtr window)
        {
            return window.Dump().ToStdString();
        }
    };
}

#endif // _WX_TESTS_TESTWINDOW_H_
