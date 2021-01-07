///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testdate.h
// Purpose:     Unit test helpers for dealing with wxDateTime.
// Author:      Vadim Zeitlin
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TESTDATE_H_
#define _WX_TESTS_TESTDATE_H_

#include "wx/datetime.h"

#include <ostream>

// need this to be able to use CPPUNIT_ASSERT_EQUAL with wxDateTime objects
inline std::ostream& operator<<(std::ostream& ostr, const wxDateTime& dt)
{
    ostr << dt.FormatISOCombined(' ');

    return ostr;
}

// need this to be able to use CPPUNIT_ASSERT_EQUAL with wxDateSpan objects
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

#endif // _WX_TESTS_TESTDATE_H_
