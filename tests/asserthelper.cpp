///////////////////////////////////////////////////////////////////////////////
// Name:        tests/asserthelper.cpp
// Purpose:     Helper functions for cppunit
// Author:      Steven Lamerton
// Created:     2010-07-23
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#include "asserthelper.h"

std::ostream& operator<<(std::ostream& os, const wxColour& c)
{
    os << c.GetAsString(wxC2S_HTML_SYNTAX);

    return os;
}

std::ostream& operator<<(std::ostream& os, const wxSize& s)
{
    os << s.x << "*" << s.y;

    return os;
}

std::ostream& operator<<(std::ostream& os, const wxFont& f)
{
    os << f.GetNativeFontInfoUserDesc();

    return os;
}

std::ostream& operator<<(std::ostream& os, const wxPoint& p)
{
    os << "(" << p.x << ", " << p.y << ")";

    return os;
}

std::ostream& operator<<(std::ostream& os, const wxRect& r)
{
    os << "{"
        << r.x << ", " << r.y << "  " << r.width << "*" << r.height
       << "}";
    return os;
}
