///////////////////////////////////////////////////////////////////////////////
// Name:        tests/asserthelper.cpp
// Purpose:     Helper functions for cppunit
// Author:      Steven Lamerton
// Created:     2010-07-23
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "asserthelper.h"

namespace
{
    std::ostream& operator<<(std::ostream& os, const ColourChannel& cc)
    {
        os.width(2);
        os.fill('0');
        os << static_cast<int>(cc.m_value);
        return os;
    }
} // anonymous namespace

std::ostream& operator<<(std::ostream& os, const wxColour& c)
{
    os << std::hex << std::noshowbase
       << "("
       << ColourChannel(c.Red()) << ", "
       << ColourChannel(c.Green()) << ", "
       << ColourChannel(c.Blue());

    if ( const unsigned char a = c.Alpha() )
    {
        os << ", " << ColourChannel(a);
    }

    os << ")";

    return os;
}

std::ostream& operator<<(std::ostream& os, const wxSize& s)
{
    os << s.x << "x" << s.y;

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
