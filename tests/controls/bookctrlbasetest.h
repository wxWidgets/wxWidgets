///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bookctrlbasetest.cpp
// Purpose:     wxBookCtrlBase unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_CONTROLS_BOOKCTRLBASETEST_H_
#define _WX_TESTS_CONTROLS_BOOKCTRLBASETEST_H_

class BookCtrlBaseTestCase
{
public:
    BookCtrlBaseTestCase() { }
    virtual ~BookCtrlBaseTestCase() { }

protected:
    // this function must be overridden by the derived classes to return the
    // text entry object we're testing, typically this is done by creating a
    // control implementing wxBookCtrlBase interface in setUp() virtual method and
    // just returning it from here
    virtual wxBookCtrlBase *GetBase() const = 0;

    virtual wxEventType GetChangedEvent() const = 0;

    virtual wxEventType GetChangingEvent() const = 0;

    // this should be inserted in the derived class CPPUNIT_TEST_SUITE
    // definition to run all wxBookCtrlBase tests as part of it
    #define wxBOOK_CTRL_BASE_TESTS() \
        CPPUNIT_TEST( Selection ); \
        CPPUNIT_TEST( Text ); \
        CPPUNIT_TEST( PageManagement ); \
        CPPUNIT_TEST( ChangeEvents )

    void Selection();
    void Text();
    void PageManagement();
    void ChangeEvents();

    //You need to add CPPUNIT_TEST( Image ) specifically if you want it to be
    //tested as only wxNotebook and wxTreebook support images correctly
    void Image();

    //Call this from the setUp function of a specific test to add panels to
    //the ctrl.
    void AddPanels();

    // Override this to call Realize() on the toolbar in the wxToolbook test.
    virtual void Realize() { }

    wxPanel* m_panel1;
    wxPanel* m_panel2;
    wxPanel* m_panel3;

    wxImageList* m_list;

private:
    wxDECLARE_NO_COPY_CLASS(BookCtrlBaseTestCase);
};

#endif // _WX_TESTS_CONTROLS_BOOKCTRLBASETEST_H_
