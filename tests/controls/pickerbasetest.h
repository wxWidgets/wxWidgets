///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/pickerbasetest.cpp
// Purpose:     wxPickerBase unit test
// Author:      Steven Lamerton
// Created:     2010-08-07
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_CONTROLS_PICKERBASETEST_H_
#define _WX_TESTS_CONTROLS_PICKERBASETEST_H_

class PickerBaseTestCase
{
public:
    PickerBaseTestCase() { }
    virtual ~PickerBaseTestCase() { }

protected:
    // this function must be overridden by the derived classes to return the
    // text entry object we're testing, typically this is done by creating a
    // control implementing wxPickerBase interface in the ctor and just
    // returning it from here
    virtual wxPickerBase *GetBase() const = 0;

    void Margin();
    void Proportion();
    void Growable();
    void Controls();

private:
    wxDECLARE_NO_COPY_CLASS(PickerBaseTestCase);
};

// Use this macro in the test file of a class deriving from PickerBaseTestCase
// to define the test cases running all wxPickerBase tests for it, e.g.
//
//      wxPICKER_BASE_TESTS(DirPickerCtrlTestCase, "DirPickerCtrl",
//                          "[dirpicker][picker]");
//
#define wxPICKER_BASE_TESTS(testclass, prefix, tags)                    \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, Margin, tags)             \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, Proportion, tags)         \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, Growable, tags)           \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, Controls, tags)           \
    struct EatNextSemicolonInPickerBaseTests

#endif // _WX_TESTS_CONTROLS_PICKERBASETEST_H_
