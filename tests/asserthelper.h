///////////////////////////////////////////////////////////////////////////////
// Name:        tests/asserthelper.h
// Purpose:     Ensure that wx types are printed correctly when assertions fail
// Author:      Steven Lamerton, Vadim Zeitlin
// Created:     2010-07-23
// Copyright:   (c) 2009-2026 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_ASSERTHELPER_H_
#define _WX_TESTS_ASSERTHELPER_H_

#include <ostream>

#include "wx/datetime.h"

inline std::ostream& operator<<(std::ostream& ostr, const wxDateTime& dt)
{
    ostr << dt.FormatISOCombined(' ');

    return ostr;
}

inline std::ostream& operator<<(std::ostream& ostr, const wxDateSpan& span)
{
    ostr << span.GetYears() << "Y, "
         << span.GetMonths() << "M, "
         << span.GetWeeks() << "W, "
         << span.GetDays() << "D";

    return ostr;
}

inline std::ostream& operator<<(std::ostream& ostr, const wxTimeSpan& span)
{
    ostr << span.GetWeeks() << "W, "
         << span.GetDays() << "D, "
         << span.GetHours() << ":"
         << span.GetMinutes() << ":"
         << span.GetSeconds() << "."
         << span.GetMilliseconds();

    return ostr;
}

// The rest is valid only in the GUI tests.
#if wxUSE_GUI

#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/font.h"

// Operators used to show the values of the corresponding types when comparing
// them in the unit tests fails.
std::ostream& operator<<(std::ostream& os, const wxColour& c);
std::ostream& operator<<(std::ostream& os, const wxSize& s);
std::ostream& operator<<(std::ostream& os, const wxFont& f);
std::ostream& operator<<(std::ostream& os, const wxPoint& p);
std::ostream& operator<<(std::ostream& os, const wxRect& r);

#endif // wxUSE_GUI

#endif // _WX_TESTS_ASSERTHELPER_H_
