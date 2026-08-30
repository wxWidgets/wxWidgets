///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/itemcontainertest.h
// Purpose:     wxItemContainer unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_CONTROLS_ITEMCONTAINERTEST_H_
#define _WX_TESTS_CONTROLS_ITEMCONTAINERTEST_H_

class ItemContainerTestCase
{
public:
    ItemContainerTestCase() { }
    virtual ~ItemContainerTestCase() { }

protected:
    // this function must be overridden by the derived classes to return the
    // text entry object we're testing, typically this is done by creating a
    // control implementing wxItemContainer interface in the ctor and just
    // returning it from here
    virtual wxItemContainer *GetContainer() const = 0;

    // and this one must be overridden to return the window which implements
    // wxItemContainer interface -- usually it will return the same pointer as
    // GetContainer(), just as a different type
    virtual wxWindow *GetContainerWindow() const = 0;

    void Append();
    void Insert();
    void Count();
    void ItemSelection();
    void FindString();
    void ClientData();
    void VoidData();
    void Set();
    void SetSelection();
    void SetString();
    void SelectionAfterDelete();
#if wxUSE_UIACTIONSIMULATOR
    virtual void SimSelect();
#endif

private:
    wxDECLARE_NO_COPY_CLASS(ItemContainerTestCase);
};

// Use this macro in the test file of a class deriving from
// ItemContainerTestCase to define the test cases running all wxItemContainer
// tests for it, e.g.
//
//      wxITEM_CONTAINER_TESTS(ChoiceTestCase, "Choice",
//                             "[choice][item-container]");
//
#define wxITEM_CONTAINER_TEST_CASE(testclass, prefix, name, tags) \
    wxTEST_CASE_FOR_METHOD(testclass, prefix, name, tags)

#if wxUSE_UIACTIONSIMULATOR
    #define wxITEM_CONTAINER_UISIM_TESTS(testclass, prefix, tags)     \
        wxITEM_CONTAINER_TEST_CASE(testclass, prefix, SimSelect, tags)
#else
    #define wxITEM_CONTAINER_UISIM_TESTS(testclass, prefix, tags)
#endif

#define wxITEM_CONTAINER_TESTS(testclass, prefix, tags)                     \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, Append, tags)             \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, Insert, tags)             \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, Count, tags)              \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, ItemSelection, tags)      \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, FindString, tags)         \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, ClientData, tags)         \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, VoidData, tags)           \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, Set, tags)                \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, SetSelection, tags)       \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, SetString, tags)          \
    wxITEM_CONTAINER_TEST_CASE(testclass, prefix, SelectionAfterDelete, tags) \
    wxITEM_CONTAINER_UISIM_TESTS(testclass, prefix, tags)                   \
    struct EatNextSemicolonInItemContainerTests

#endif // _WX_TESTS_CONTROLS_ITEMCONTAINERTEST_H_
