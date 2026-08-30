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
    // control implementing wxBookCtrlBase interface in the ctor and just
    // returning it from here
    virtual wxBookCtrlBase *GetBase() const = 0;

    virtual wxEventType GetChangedEvent() const = 0;

    virtual wxEventType GetChangingEvent() const = 0;

    // Some wxBookCtrlBase-derived classes strip mnemonics and don't return
    // them from their GetPageText(), allow them to just return true from here.
    virtual bool HasBrokenMnemonics() const { return false; }

    void Selection();
    void Text();
    void PageManagement();
    void ChangeEvents();

    //You need to use wxBOOK_CTRL_BASE_TEST_CASE() with Image explicitly if you
    //want it to be tested as only wxNotebook and wxTreebook support images
    //correctly
    void Image();

    //Call this from the ctor of a specific test to add panels to the ctrl.
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

// Use this macro in the test file of a class deriving from
// BookCtrlBaseTestCase to define the test cases running all wxBookCtrlBase
// tests for it, e.g.
//
//      wxBOOK_CTRL_BASE_TESTS(NotebookTestCase, "Notebook", "[notebook]");
//
#define wxBOOK_CTRL_BASE_TEST_CASE(testclass, prefix, name, tags) \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, name, tags)

#define wxBOOK_CTRL_BASE_TESTS(testclass, prefix, tags)                     \
    wxBOOK_CTRL_BASE_TEST_CASE(testclass, prefix, Selection, tags)          \
    wxBOOK_CTRL_BASE_TEST_CASE(testclass, prefix, Text, tags)               \
    wxBOOK_CTRL_BASE_TEST_CASE(testclass, prefix, PageManagement, tags)     \
    wxBOOK_CTRL_BASE_TEST_CASE(testclass, prefix, ChangeEvents, tags)       \
    struct EatNextSemicolonInBookCtrlBaseTests

#endif // _WX_TESTS_CONTROLS_BOOKCTRLBASETEST_H_
