///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbasetest.cpp
// Purpose:     Base class for wxListCtrl and wxListView tests
// Author:      Steven Lamerton
// Created:     2010-07-20
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>,
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

    #define wxLIST_BASE_TESTS() \
        CPPUNIT_TEST( ColumnsOrder ); \
        CPPUNIT_TEST( ItemRect ); \
        CPPUNIT_TEST( ItemText ); \
        CPPUNIT_TEST( ChangeMode ); \
        WXUISIM_TEST( ItemClick ); \
        WXUISIM_TEST( KeyDown ); \
        CPPUNIT_TEST( DeleteItems ); \
        CPPUNIT_TEST( InsertItem ); \
        CPPUNIT_TEST( Find ); \
        CPPUNIT_TEST( Visible ); \
        CPPUNIT_TEST( ItemFormatting ); \
        WXUISIM_TEST( EditLabel ); \
        CPPUNIT_TEST( ImageList ); \
        CPPUNIT_TEST( Sort )

    void ColumnsOrder();
    void ItemRect();
    void ItemText();
    void ChangeMode();
    void ItemClick();
    void KeyDown();
    void DeleteItems();
    void InsertItem();
    void Find();
    void Visible();
    void ItemFormatting();
    void EditLabel();
    void ImageList();
    void Sort();

    wxDECLARE_NO_COPY_CLASS(ListBaseTestCase);
};

#endif
