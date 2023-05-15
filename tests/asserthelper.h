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

// Operators used to show the values of the corresponding types when comparing
// them in the unit tests fails.
std::ostream& operator<<(std::ostream& os, const wxColour& c);
std::ostream& operator<<(std::ostream& os, const wxSize& s);
std::ostream& operator<<(std::ostream& os, const wxFont& f);
std::ostream& operator<<(std::ostream& os, const wxPoint& p);
std::ostream& operator<<(std::ostream& os, const wxRect& r);

#endif
