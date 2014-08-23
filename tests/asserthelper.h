///////////////////////////////////////////////////////////////////////////////
// Name:        tests/asserthelper.h
// Purpose:     Helper functions for cppunit
// Author:      Steven Lamerton
// Created:     2010-07-23
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_ASSERTHELPER_H_
#define _WX_TESTS_ASSERTHELPER_H_

#include <ostream>
#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/font.h"

namespace
{
    // by default colour components values are output incorrectly because they
    // are unsigned chars, define a small helper struct which formats them in
    // a more useful way
    struct ColourChannel
    {
        ColourChannel(unsigned char value) : m_value(value) { }

        unsigned char m_value;
    };

    std::ostream& operator<<(std::ostream& os, const ColourChannel& cc);

} // anonymous namespace

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxColour objects
std::ostream& operator<<(std::ostream& os, const wxColour& c);

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxSize objects
std::ostream& operator<<(std::ostream& os, const wxSize& s);

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxFont objects
std::ostream& operator<<(std::ostream& os, const wxFont& f);

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxPoint objects
std::ostream& operator<<(std::ostream& os, const wxPoint& p);

#endif
