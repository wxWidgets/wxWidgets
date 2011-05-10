///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/menu.cpp
// Purpose:     wxMenu unit test
// Author:      wxWidgets team
// Created:     2010-11-10
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/menu.h"
#include <stdarg.h>

// ----------------------------------------------------------------------------
// helper
// ----------------------------------------------------------------------------

namespace
{

enum
{
    MenuTestCase_Foo = 10000,
    MenuTestCase_Bar,
    MenuTestCase_First
};

void PopulateMenu(wxMenu* menu, const wxString& name,  size_t& itemcount)
{
    // Start at item 1 to make it human-readable ;)
    for (int n=1; n<6; ++n, ++itemcount)
    {
        wxString label = name; label << n;
        menu->Append(MenuTestCase_First + itemcount, label, label + " help string");
    }
}

void RecursivelyCountMenuItems(const wxMenu* menu, size_t& count)
{
    CPPUNIT_ASSERT( menu );

    count += menu->GetMenuItemCount();
    for (size_t n=0; n < menu->GetMenuItemCount(); ++n)
    {
        wxMenuItem* item = menu->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            RecursivelyCountMenuItems(item->GetSubMenu(), count);
        }
    }
}

} // anon namespace


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MenuTestCase : public CppUnit::TestCase
{
public:
    MenuTestCase() {}

    virtual void setUp() { CreateFrame(); }
    virtual void tearDown() { m_frame->Destroy(); }

private:
    CPPUNIT_TEST_SUITE( MenuTestCase );
        CPPUNIT_TEST( FindInMenubar );
        CPPUNIT_TEST( FindInMenu );
        CPPUNIT_TEST( Count );
        CPPUNIT_TEST( Labels );
        CPPUNIT_TEST( RadioItems );
    CPPUNIT_TEST_SUITE_END();

    void CreateFrame();

    void FindInMenubar();
    void FindInMenu();
    void Count();
    void Labels();
    void RadioItems();

    wxFrame* m_frame;

    // Holds the number of menuitems contained in all the menus
    size_t m_itemCount;

    // Store here the id of a known submenu item, to be searched for later
    int m_submenuItemId;

    // and a sub-submenu item
    int m_subsubmenuItemId;

    wxArrayString m_menuLabels;

    DECLARE_NO_COPY_CLASS(MenuTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MenuTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MenuTestCase, "MenuTestCase" );

void MenuTestCase::CreateFrame()
{
    m_frame = new wxFrame(NULL, wxID_ANY, "test frame");

    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *subMenu = new wxMenu;
    wxMenu *subsubMenu = new wxMenu;

    size_t itemcount = 0;

    PopulateMenu(subsubMenu, "Subsubmenu item ", itemcount);

    // Store one of its IDs for later
    m_subsubmenuItemId = MenuTestCase_First + itemcount - 2;

    PopulateMenu(subMenu, "Submenu item ", itemcount);

    // Store one of its IDs for later
    m_submenuItemId = MenuTestCase_First + itemcount - 2;

    subMenu->AppendSubMenu(subsubMenu, "Subsubmen&u", "Test a subsubmenu");

    PopulateMenu(fileMenu, "Filemenu item ", itemcount);

    fileMenu->Append(MenuTestCase_Foo, "&Foo\tCtrl-F", "Test item to be found");


    PopulateMenu(helpMenu, "Helpmenu item ", itemcount);
    helpMenu->Append(MenuTestCase_Bar, "Bar");
    helpMenu->AppendSubMenu(subMenu, "Sub&menu", "Test a submenu");

    // +2 for "Foo" and "Bar", +2 for the 2 submenus
    m_itemCount = itemcount + 4;

    // Use an arraystring here, to help with future tests
    m_menuLabels.Add("&File");
    m_menuLabels.Add("&Help");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, m_menuLabels[0]);
    menuBar->Append(helpMenu, m_menuLabels[1]);
    m_frame->SetMenuBar(menuBar);
}

void MenuTestCase::FindInMenubar()
{
    wxMenuBar* bar = m_frame->GetMenuBar();

    // Find by name:
    CPPUNIT_ASSERT( bar->FindMenu("File") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenu("&File") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenu("&Fail") == wxNOT_FOUND );

    // Find by menu name plus item name:
    CPPUNIT_ASSERT( bar->FindMenuItem("File", "Foo") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenuItem("&File", "&Foo") != wxNOT_FOUND );
    // and using the menu label
    int index = bar->FindMenu("&File");
    CPPUNIT_ASSERT( index != wxNOT_FOUND );
    wxString menulabel = bar->GetMenuLabel(index);
    CPPUNIT_ASSERT( bar->FindMenuItem(menulabel, "&Foo") != wxNOT_FOUND );
    // and title
    wxString menutitle = bar->GetMenu(index)->GetTitle();
    CPPUNIT_ASSERT( bar->FindMenuItem(menutitle, "&Foo") != wxNOT_FOUND );

    // Find by position:
    for (size_t n=0; n < bar->GetMenuCount(); ++n)
    {
        CPPUNIT_ASSERT( bar->GetMenu(n) );
    }

    // Find by id:
    wxMenu* menu = NULL;
    wxMenuItem* item = NULL;
    item = bar->FindItem(MenuTestCase_Foo, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
    // Check that the correct menu was found
    CPPUNIT_ASSERT( menu->FindChildItem(MenuTestCase_Foo) );

    // Find submenu item:
    item = bar->FindItem(m_submenuItemId, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
    // and, for completeness, a subsubmenu one:
    item = bar->FindItem(m_subsubmenuItemId, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
}

void MenuTestCase::FindInMenu()
{
    wxMenuBar* bar = m_frame->GetMenuBar();

    // Find by name:
    wxMenu* menuFind = bar->GetMenu(0);
    CPPUNIT_ASSERT( menuFind->FindItem("Foo") != wxNOT_FOUND );
    CPPUNIT_ASSERT( menuFind->FindItem("&Foo") != wxNOT_FOUND );
    // and for submenus
    wxMenu* menuHelp = bar->GetMenu(1);
    CPPUNIT_ASSERT( menuHelp->FindItem("Submenu") != wxNOT_FOUND );
    CPPUNIT_ASSERT( menuHelp->FindItem("Sub&menu") != wxNOT_FOUND );

    // Find by position:
    size_t n;
    for (n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        CPPUNIT_ASSERT( menuHelp->FindItemByPosition(n) );
    }

    // Find by id:
    CPPUNIT_ASSERT( menuHelp->FindItem(MenuTestCase_Bar) );
    CPPUNIT_ASSERT( menuHelp->FindItem(MenuTestCase_Foo) == NULL );

    for (n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        size_t locatedAt;
        wxMenuItem* itemByPos = menuHelp->FindItemByPosition(n);
        CPPUNIT_ASSERT( itemByPos );
        wxMenuItem* itemById = menuHelp->FindChildItem(itemByPos->GetId(), &locatedAt);
        CPPUNIT_ASSERT_EQUAL( itemByPos, itemById );
        CPPUNIT_ASSERT_EQUAL( locatedAt, n );
    }

    // Find submenu item:
    for (n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        wxMenuItem* item = menuHelp->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            wxMenu* submenu;
            wxMenuItem* submenuItem = menuHelp->FindItem(m_submenuItemId, &submenu);
            CPPUNIT_ASSERT( submenuItem );
            CPPUNIT_ASSERT( item->GetSubMenu() == submenu );
        }
    }
}

void MenuTestCase::Count()
{
    wxMenuBar* bar = m_frame->GetMenuBar();
    // I suppose you could call this "counting menubars" :)
    CPPUNIT_ASSERT( bar );

    CPPUNIT_ASSERT_EQUAL( bar->GetMenuCount(), 2 );

    size_t count = 0;
    for (size_t n=0; n < bar->GetMenuCount(); ++n)
    {
        RecursivelyCountMenuItems(bar->GetMenu(n), count);
    }
    CPPUNIT_ASSERT_EQUAL( count, m_itemCount );
}

void MenuTestCase::Labels()
{
    wxMenuBar* bar = m_frame->GetMenuBar();
    CPPUNIT_ASSERT( bar );
    wxMenu* filemenu;
    wxMenuItem* itemFoo = bar->FindItem(MenuTestCase_Foo, &filemenu);
    CPPUNIT_ASSERT( itemFoo );
    CPPUNIT_ASSERT( filemenu );

    // These return labels including mnemonics/accelerators:

    // wxMenuBar
    CPPUNIT_ASSERT_EQUAL( "&File", bar->GetMenuLabel(0) );
    CPPUNIT_ASSERT_EQUAL( "&Foo\tCtrl-F", bar->GetLabel(MenuTestCase_Foo) );

    // wxMenu
    CPPUNIT_ASSERT_EQUAL( "&File", filemenu->GetTitle() );
    CPPUNIT_ASSERT_EQUAL( "&Foo\tCtrl-F", filemenu->GetLabel(MenuTestCase_Foo) );

    // wxMenuItem
    CPPUNIT_ASSERT_EQUAL( "&Foo\tCtrl-F", itemFoo->GetItemLabel() );

    // These return labels stripped of mnemonics/accelerators:

    // wxMenuBar
    CPPUNIT_ASSERT_EQUAL( "File", bar->GetMenuLabelText(0) );

    // wxMenu
    CPPUNIT_ASSERT_EQUAL( "Foo", filemenu->GetLabelText(MenuTestCase_Foo) );

    // wxMenuItem
    CPPUNIT_ASSERT_EQUAL( "Foo", itemFoo->GetItemLabelText() );
    CPPUNIT_ASSERT_EQUAL( "Foo", wxMenuItem::GetLabelText("&Foo\tCtrl-F") );
}

void MenuTestCase::RadioItems()
{
    wxMenuBar * const bar = m_frame->GetMenuBar();
    wxMenu * const menu = new wxMenu;
    bar->Append(menu, "&Radio");

    // Adding consecutive radio items creates a radio group.
    menu->AppendRadioItem(MenuTestCase_First, "Radio 0");
    menu->AppendRadioItem(MenuTestCase_First + 1, "Radio 1");

    // First item of a radio group is checked by default.
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First) );

    // Checking the second one make the first one unchecked however.
    menu->Check(MenuTestCase_First + 1, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First) );
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 1) );

    // Adding more radio items after a separator creates another radio group...
    menu->AppendSeparator();
    menu->AppendRadioItem(MenuTestCase_First + 2, "Radio 2");
    menu->AppendRadioItem(MenuTestCase_First + 3, "Radio 3");
    menu->AppendRadioItem(MenuTestCase_First + 4, "Radio 4");

    // ... which is independent from the first one.
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 2) );

    menu->Check(MenuTestCase_First + 3, true);
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 3) );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 2) );
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 1) );


    // Insert an item in the middle of an existing radio group.
    menu->InsertRadioItem(4, MenuTestCase_First + 5, "Radio 5");
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 3) );

    menu->Check( MenuTestCase_First + 5, true );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 3) );


    // Prepend a couple of items before the first group.
    menu->PrependRadioItem(MenuTestCase_First + 6, "Radio 6");
    menu->PrependRadioItem(MenuTestCase_First + 7, "Radio 7");
    menu->Check(MenuTestCase_First + 7, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 1) );


    // Check that the last radio group still works as expected.
    menu->Check(MenuTestCase_First + 4, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 5) );
}
