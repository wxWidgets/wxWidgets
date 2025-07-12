///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbasetest.cpp
// Purpose:     Common wxListCtrl and wxListView tests
// Author:      Steven Lamerton
// Created:     2010-07-20
// Copyright:   (c) 2008,2025 Vadim Zeitlin <vadim@wxwidgets.org>,
//              (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_CONTROLS_LISTBASETEST_H_
#define _WX_TESTS_CONTROLS_LISTBASETEST_H_

class ListBaseTestCase
{
public:
    ListBaseTestCase() { }
    virtual ~ListBaseTestCase() { }

protected:
    virtual wxListCtrl *GetList() const = 0;

    void ColumnsOrder();
    void ItemRect();
    void ItemText();
    void ChangeMode();
    void MultiSelect();
    void ItemClick();
    void KeyDown();
    void DeleteItems();
    void InsertItem();
    void Find();
    void Visible();
    void ItemFormatting();
    void EditLabel();
    void ImageList();
    void HitTest();
    void Sort();

    wxDECLARE_NO_COPY_CLASS(ListBaseTestCase);
};

// In the macros below, ClassName is the name of the class (without "wx"
// prefix), i.e. an identifier, and ClassTag is the string containing the
// tag to be used in the test registration macro.

// Define a test case delegating to ListBaseTestCase.
#define wxLIST_TEST_CASE(ClassName, ClassTag, TestName) \
    TEST_CASE_METHOD(ClassName ## TestCase, \
                     #ClassName "::" #TestName, \
                     ClassTag) \
    { \
        TestName(); \
    }

// Define all common test cases.
#define wxLIST_BASE_TESTS(ClassName, TagName) \
    wxLIST_TEST_CASE(ClassName, TagName, ColumnsOrder) \
    wxLIST_TEST_CASE(ClassName, TagName, ItemRect) \
    wxLIST_TEST_CASE(ClassName, TagName, ItemText) \
    wxLIST_TEST_CASE(ClassName, TagName, ChangeMode) \
    wxLIST_TEST_CASE(ClassName, TagName, ItemClick) \
    wxLIST_TEST_CASE(ClassName, TagName, KeyDown) \
    wxLIST_TEST_CASE(ClassName, TagName, MultiSelect) \
    wxLIST_TEST_CASE(ClassName, TagName, DeleteItems) \
    wxLIST_TEST_CASE(ClassName, TagName, InsertItem) \
    wxLIST_TEST_CASE(ClassName, TagName, Find) \
    wxLIST_TEST_CASE(ClassName, TagName, Visible) \
    wxLIST_TEST_CASE(ClassName, TagName, ItemFormatting) \
    wxLIST_TEST_CASE(ClassName, TagName, EditLabel) \
    wxLIST_TEST_CASE(ClassName, TagName, ImageList) \
    wxLIST_TEST_CASE(ClassName, TagName, HitTest) \
    wxLIST_TEST_CASE(ClassName, TagName, Sort)

#endif
