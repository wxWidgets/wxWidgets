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
    // control implementing wxPickerBase interface in setUp() virtual method and
    // just returning it from here
    virtual wxPickerBase *GetBase() const = 0;

    // this should be inserted in the derived class CPPUNIT_TEST_SUITE
    // definition to run all wxPickerBase tests as part of it
    #define wxPICKER_BASE_TESTS() \
        CPPUNIT_TEST( Margin ); \
        CPPUNIT_TEST( Proportion ); \
        CPPUNIT_TEST( Growable ); \
        CPPUNIT_TEST( Controls )

    void Margin();
    void Proportion();
    void Growable();
    void Controls();

private:
    wxDECLARE_NO_COPY_CLASS(PickerBaseTestCase);
};

#endif // _WX_TESTS_CONTROLS_PICKERBASETEST_H_
