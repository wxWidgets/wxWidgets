///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/menu.cpp
// Purpose:     wxMenu unit test
// Author:      wxWidgets team
// Created:     2010-11-10
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_MENUBAR


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/menu.h"
#include "wx/scopedptr.h"
#include "wx/translation.h"
#include "wx/uiaction.h"

#include <stdarg.h>

// ----------------------------------------------------------------------------
// helper
// ----------------------------------------------------------------------------

namespace
{

enum
{
    MenuTestCase_Foo = 10000,
    MenuTestCase_SelectAll,
    MenuTestCase_Bar,
    MenuTestCase_ExtraAccel,
    MenuTestCase_ExtraAccels,
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

    virtual void setUp() wxOVERRIDE { CreateFrame(); }
    virtual void tearDown() wxOVERRIDE { m_frame->Destroy(); }

private:
    CPPUNIT_TEST_SUITE( MenuTestCase );
        CPPUNIT_TEST( FindInMenubar );
        CPPUNIT_TEST( FindInMenu );
        CPPUNIT_TEST( EnableTop );
        CPPUNIT_TEST( Count );
        CPPUNIT_TEST( Labels );
#if wxUSE_INTL
        CPPUNIT_TEST( TranslatedMnemonics );
#endif // wxUSE_INTL
        CPPUNIT_TEST( RadioItems );
        CPPUNIT_TEST( RemoveAdd );
        CPPUNIT_TEST( ChangeBitmap );
        WXUISIM_TEST( Events );
    CPPUNIT_TEST_SUITE_END();

    void CreateFrame();

    void FindInMenubar();
    void FindInMenu();
    void EnableTop();
    void Count();
    void Labels();
#if wxUSE_INTL
    void TranslatedMnemonics();
#endif // wxUSE_INTL
    void RadioItems();
    void RemoveAdd();
    void ChangeBitmap();
    void Events();

    wxFrame* m_frame;

    // Holds the number of menuitems contained in all the menus
    size_t m_itemCount;

    // Store here the id of a known submenu item, to be searched for later
    int m_submenuItemId;

    // and a sub-submenu item
    int m_subsubmenuItemId;

    wxArrayString m_menuLabels;

    // The menu containing the item with MenuTestCase_Bar id.
    wxMenu* m_menuWithBar;

    wxDECLARE_NO_COPY_CLASS(MenuTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MenuTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MenuTestCase, "MenuTestCase" );

void MenuTestCase::CreateFrame()
{
    m_frame = new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY, "test frame");

    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *subMenu = new wxMenu;
    wxMenu *subsubMenu = new wxMenu;

    m_itemCount = 0;

    PopulateMenu(subsubMenu, "Subsubmenu item ", m_itemCount);

    // Store one of its IDs for later
    m_subsubmenuItemId = MenuTestCase_First + m_itemCount - 2;

    PopulateMenu(subMenu, "Submenu item ", m_itemCount);

    // Store one of its IDs for later
    m_submenuItemId = MenuTestCase_First + m_itemCount - 2;

    subMenu->AppendSubMenu(subsubMenu, "Subsubmen&u", "Test a subsubmenu");
    m_itemCount++;

    // Check GetTitle() returns the correct string _before_ appending to the bar
    fileMenu->SetTitle("&Foo\tCtrl-F");
    CPPUNIT_ASSERT_EQUAL( "&Foo\tCtrl-F", fileMenu->GetTitle() );

    PopulateMenu(fileMenu, "Filemenu item ", m_itemCount);

    fileMenu->Append(MenuTestCase_Foo, "&Foo\tCtrl-F", "Test item to be found");
    m_itemCount++;
    fileMenu->Append(MenuTestCase_SelectAll, "Select &all\tCtrl-A",
                     "Accelerator conflicting with wxTextCtrl");
    m_itemCount++;

    // Test adding an extra accelerator to the item before adding it to the menu.
    wxAcceleratorEntry entry;

    wxMenuItem* const
        extraAccel = new wxMenuItem(fileMenu, MenuTestCase_ExtraAccel, "Extra accels");

    CHECK( entry.FromString("Ctrl-U") );
    extraAccel->SetAccel(&entry);

    CHECK( entry.FromString("Ctrl-V") );
    extraAccel->AddExtraAccel(entry);

    fileMenu->Append(extraAccel);
    m_itemCount++;

    // And now also test adding 2 of them after creating the initial menu item.
    wxMenuItem* const
        extraAccels = fileMenu->Append(MenuTestCase_ExtraAccels, "Extra accels 2");
    m_itemCount++;

    CHECK( entry.FromString("Ctrl-T") );
    extraAccels->AddExtraAccel(entry);

    CHECK(entry.FromString("Shift-Ctrl-W"));
    extraAccels->AddExtraAccel(entry);

    CHECK(entry.FromString("Ctrl-W"));
    extraAccels->SetAccel(&entry);

    PopulateMenu(helpMenu, "Helpmenu item ", m_itemCount);
    helpMenu->Append(MenuTestCase_Bar, "Bar\tF1");
    m_itemCount++;
    m_menuWithBar = helpMenu;
    helpMenu->AppendSubMenu(subMenu, "Sub&menu", "Test a submenu");
    m_itemCount++;

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
    CPPUNIT_ASSERT( !menuHelp->FindItem(MenuTestCase_Foo) );

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

void MenuTestCase::EnableTop()
{
    wxMenuBar* const bar = m_frame->GetMenuBar();
    CPPUNIT_ASSERT( bar->IsEnabledTop(0) );
    bar->EnableTop( 0, false );
    CPPUNIT_ASSERT( !bar->IsEnabledTop(0) );
    bar->EnableTop( 0, true );
    CPPUNIT_ASSERT( bar->IsEnabledTop(0) );
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

#if wxUSE_INTL

static wxString
GetTranslatedString(const wxTranslations& trans, const wxString& s)
{
    const wxString* t = trans.GetTranslatedString(s);
    return t ? *t : s;
}

void MenuTestCase::TranslatedMnemonics()
{
    // Check that appended mnemonics are correctly stripped;
    // see https://github.com/wxWidgets/wxWidgets/issues/16736
    wxTranslations trans;
    trans.SetLanguage(wxLANGUAGE_JAPANESE);
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix("./intl");
    CPPUNIT_ASSERT( trans.AddCatalog("internat") );

    // Check the translation is being used:
    CPPUNIT_ASSERT( wxString("&File") != GetTranslatedString(trans, "&File") );

    wxString filemenu = m_frame->GetMenuBar()->GetMenuLabel(0);
    CPPUNIT_ASSERT_EQUAL
    (
         wxStripMenuCodes(GetTranslatedString(trans, "&File"), wxStrip_Menu),
         wxStripMenuCodes(GetTranslatedString(trans, filemenu), wxStrip_Menu)
    );

    // Test strings that have shortcuts. Duplicate non-mnemonic translations
    // exist for both "Edit" and "View", for ease of comparison
    CPPUNIT_ASSERT_EQUAL
    (
         GetTranslatedString(trans, "Edit"),
         wxStripMenuCodes(GetTranslatedString(trans, "E&dit\tCtrl+E"), wxStrip_Menu)
    );

    // "Vie&w" also has a space before the (&W)
    CPPUNIT_ASSERT_EQUAL
    (
         GetTranslatedString(trans, "View"),
         wxStripMenuCodes(GetTranslatedString(trans, "Vie&w\tCtrl+V"), wxStrip_Menu)
    );

    // Test a 'normal' mnemonic too: the translation is "Preten&d"
    CPPUNIT_ASSERT_EQUAL
    (
         "Pretend",
         wxStripMenuCodes(GetTranslatedString(trans, "B&ogus"), wxStrip_Menu)
    );
}
#endif // wxUSE_INTL

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

    // Subsequent items in a group are not checked.
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 1) );

#ifdef __WXQT__
    WARN("Radio check test does not work under Qt");
#else
    // Checking the second one make the first one unchecked however.
    menu->Check(MenuTestCase_First + 1, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First) );
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 1) );
    menu->Check(MenuTestCase_First, true);
#endif

    // Adding more radio items after a separator creates another radio group...
    menu->AppendSeparator();
    menu->AppendRadioItem(MenuTestCase_First + 2, "Radio 2");
    menu->AppendRadioItem(MenuTestCase_First + 3, "Radio 3");
    menu->AppendRadioItem(MenuTestCase_First + 4, "Radio 4");

    // ... which is independent from the first one.
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First) );
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 2) );

#ifdef __WXQT__
    WARN("Radio check test does not work under Qt");
#else
    menu->Check(MenuTestCase_First + 3, true);
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 3) );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 2) );

    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First) );
    menu->Check(MenuTestCase_First + 2, true);
#endif

    // Insert an item in the middle of an existing radio group.
    menu->InsertRadioItem(4, MenuTestCase_First + 5, "Radio 5");
    CPPUNIT_ASSERT( menu->IsChecked(MenuTestCase_First + 2) );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 5) );

#ifdef __WXQT__
    WARN("Radio check test does not work under Qt");
#else
    menu->Check( MenuTestCase_First + 5, true );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 3) );

    menu->Check( MenuTestCase_First + 3, true );
#endif

    // Prepend a couple of items before the first group.
    menu->PrependRadioItem(MenuTestCase_First + 6, "Radio 6");
    menu->PrependRadioItem(MenuTestCase_First + 7, "Radio 7");
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 6) );
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 7) );

#ifdef __WXQT__
    WARN("Radio check test does not work under Qt");
#else
    menu->Check(MenuTestCase_First + 7, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 1) );


    // Check that the last radio group still works as expected.
    menu->Check(MenuTestCase_First + 4, true);
    CPPUNIT_ASSERT( !menu->IsChecked(MenuTestCase_First + 5) );
#endif
}

void MenuTestCase::RemoveAdd()
{
    wxMenuBar* bar = m_frame->GetMenuBar();

    wxMenu* menu0 = bar->GetMenu(0);
    wxMenu* menu1 = bar->GetMenu(1);
    wxMenuItem* item = new wxMenuItem(menu0, MenuTestCase_Foo + 100, "t&ext\tCtrl-E");
    menu0->Insert(0, item);
    CPPUNIT_ASSERT( menu0->FindItemByPosition(0) == item );
    menu0->Remove(item);
    CPPUNIT_ASSERT( menu0->FindItemByPosition(0) != item );
    menu1->Insert(0, item);
    CPPUNIT_ASSERT( menu1->FindItemByPosition(0) == item );
    menu1->Remove(item);
    CPPUNIT_ASSERT( menu1->FindItemByPosition(0) != item );
    menu0->Insert(0, item);
    CPPUNIT_ASSERT( menu0->FindItemByPosition(0) == item );
    menu0->Delete(item);
}

void MenuTestCase::ChangeBitmap()
{
    wxMenu *menu = new wxMenu;

    wxMenuItem *item = new wxMenuItem(menu, wxID_ANY, "Item");
    menu->Append(item);

    // On Windows Vista (and later) calling SetBitmap, *after* the menu
    // item has already been added, used to result in a stack overflow:
    // [Do]SetBitmap can call GetHBitmapForMenu which will call SetBitmap
    // again etc...
    item->SetBitmap( wxBitmap(1, 1) );


    // Force owner drawn usage by having a bitmap that's wider than the
    // standard size. This results in rearranging the parent menu which
    // hasn't always worked properly and lead to a null pointer exception.
    item->SetBitmap( wxBitmap(512, 1) );

    wxDELETE(menu);
}

#if wxUSE_UIACTIONSIMULATOR

// In C++98 this class can't be defined inside Events() method, unfortunately,
// as its OnMenu() method wouldn't be usable with template Bind() then.
class MenuEventHandler : public wxEvtHandler
{
public:
    MenuEventHandler(wxWindow* win)
        : m_win(win)
    {
        m_win->Bind(wxEVT_MENU, &MenuEventHandler::OnMenu, this);

        m_event = NULL;
    }

    virtual ~MenuEventHandler()
    {
        m_win->Unbind(wxEVT_MENU, &MenuEventHandler::OnMenu, this);

        delete m_event;
    }

    // Check that we received an event with the given ID and return the event
    // object if we did (otherwise fail the test and return NULL).
    const wxObject* CheckGot(int expectedId)
    {
        if ( !m_event )
        {
            FAIL("Event not generated");
            return NULL;
        }

        CHECK( m_event->GetId() == expectedId );

        const wxObject* const src = m_event->GetEventObject();

        delete m_event;
        m_event = NULL;

        return src;
    }

    bool GotEvent() const
    {
        return m_event != NULL;
    }

private:
    void OnMenu(wxCommandEvent& event)
    {
        CHECK( !m_event );

        m_event = static_cast<wxCommandEvent*>(event.Clone());
    }

    wxWindow* const m_win;
    wxCommandEvent* m_event;
};

#endif // wxUSE_UIACTIONSIMULATOR

void MenuTestCase::Events()
{
#if wxUSE_UIACTIONSIMULATOR
    MenuEventHandler handler(m_frame);

    // Invoke the accelerator.
    m_frame->Show();
    m_frame->SetFocus();
    wxYield();

    wxUIActionSimulator sim;
    sim.KeyDown(WXK_F1);
    sim.KeyUp(WXK_F1);
    wxYield();

    INFO("Expecting event for F1");
    if ( const wxObject* const src = handler.CheckGot(MenuTestCase_Bar) )
    {
        CHECK( wxString(src->GetClassInfo()->GetClassName()) == "wxMenu" );
        CHECK( src == m_menuWithBar );
    }

    // Invoke another accelerator, it should also work.
    sim.Char('A', wxMOD_CONTROL);
    wxYield();

    INFO("Expecting event for Ctrl-A");
    handler.CheckGot(MenuTestCase_SelectAll);

    // Invoke accelerator and extra accelerators, all of them should work.
    sim.Char('U', wxMOD_CONTROL);
    wxYield();
    INFO("Expecting event for Ctrl-U");
    handler.CheckGot(MenuTestCase_ExtraAccel);

    sim.Char('V', wxMOD_CONTROL);
    wxYield();
    INFO("Expecting event for Ctrl-V");
    handler.CheckGot(MenuTestCase_ExtraAccel);

    sim.Char('W', wxMOD_CONTROL);
    wxYield();
    INFO("Expecting event for Ctrl-W");
    handler.CheckGot(MenuTestCase_ExtraAccels);

    sim.Char('T', wxMOD_CONTROL);
    wxYield();
    INFO("Expecting event for Ctrl-T");
    handler.CheckGot(MenuTestCase_ExtraAccels);

    sim.Char('W', wxMOD_CONTROL | wxMOD_SHIFT);
    wxYield();
    INFO("Expecting event for Ctrl-Shift-W");
    handler.CheckGot(MenuTestCase_ExtraAccels);

    // Now create a text control which uses the same accelerator for itself and
    // check that when the text control has focus, the accelerator does _not_
    // work.
    wxTextCtrl* const text = new wxTextCtrl(m_frame, wxID_ANY, "Testing");
    text->SetFocus();

    sim.Char('A', wxMOD_CONTROL);
    wxYield();

    CHECK( !handler.GotEvent() );
#endif // wxUSE_UIACTIONSIMULATOR
}

namespace
{

void VerifyAccelAssigned( wxString labelText, int keycode )
{
    const wxScopedPtr<wxAcceleratorEntry> entry(
        wxAcceleratorEntry::Create( labelText )
    );

    CHECK( entry );
    CHECK( entry->GetKeyCode() == keycode );
}

struct key
{
    int      keycode;
    wxString name;
    bool     skip;
};
key modKeys[] =
{
    { wxACCEL_NORMAL, "Normal", false },
    { wxACCEL_CTRL,   "Ctrl",   false },
    { wxACCEL_SHIFT,  "Shift",  false },
    { wxACCEL_ALT,    "Alt",    false }
};
/*
 The keys marked as skip below are not supported as accelerator
 keys on GTK.
 */
key specialKeys[] =
{
    { WXK_F1,               "WXK_F1",               false },
    { WXK_F2,               "WXK_F2",               false },
    { WXK_F3,               "WXK_F3",               false },
    { WXK_F4,               "WXK_F4",               false },
    { WXK_F5,               "WXK_F5",               false },
    { WXK_F6,               "WXK_F6",               false },
    { WXK_F7,               "WXK_F7",               false },
    { WXK_F8,               "WXK_F8",               false },
    { WXK_F9,               "WXK_F9",               false },
    { WXK_F10,              "WXK_F10",              false },
    { WXK_F11,              "WXK_F11",              false },
    { WXK_F12,              "WXK_F12",              false },
    { WXK_F13,              "WXK_F13",              false },
    { WXK_F14,              "WXK_F14",              false },
    { WXK_F15,              "WXK_F15",              false },
    { WXK_F16,              "WXK_F16",              false },
    { WXK_F17,              "WXK_F17",              false },
    { WXK_F18,              "WXK_F18",              false },
    { WXK_F19,              "WXK_F19",              false },
    { WXK_F20,              "WXK_F20",              false },
    { WXK_F21,              "WXK_F21",              false },
    { WXK_F22,              "WXK_F22",              false },
    { WXK_F23,              "WXK_F23",              false },
    { WXK_F24,              "WXK_F24",              false },
    { WXK_INSERT,           "WXK_INSERT",           false },
    { WXK_DELETE,           "WXK_DELETE",           false },
    { WXK_UP,               "WXK_UP",               false },
    { WXK_DOWN,             "WXK_DOWN",             false },
    { WXK_PAGEUP,           "WXK_PAGEUP",           false },
    { WXK_PAGEDOWN,         "WXK_PAGEDOWN",         false },
    { WXK_LEFT,             "WXK_LEFT",             false },
    { WXK_RIGHT,            "WXK_RIGHT",            false },
    { WXK_HOME,             "WXK_HOME",             false },
    { WXK_END,              "WXK_END",              false },
    { WXK_RETURN,           "WXK_RETURN",           false },
    { WXK_BACK,             "WXK_BACK",             false },
    { WXK_TAB,              "WXK_TAB",              true },
    { WXK_ESCAPE,           "WXK_ESCAPE",           false },
    { WXK_SPACE,            "WXK_SPACE",            false },
    { WXK_MULTIPLY,         "WXK_MULTIPLY",         false },
    { WXK_ADD,              "WXK_ADD",              true },
    { WXK_SEPARATOR,        "WXK_SEPARATOR",        true },
    { WXK_SUBTRACT,         "WXK_SUBTRACT",         true },
    { WXK_DECIMAL,          "WXK_DECIMAL",          true },
    { WXK_DIVIDE,           "WXK_DIVIDE",           true },
    { WXK_CANCEL,           "WXK_CANCEL",           false },
    { WXK_CLEAR,            "WXK_CLEAR",            false },
    { WXK_MENU,             "WXK_MENU",             false },
    { WXK_PAUSE,            "WXK_PAUSE",            false },
    { WXK_CAPITAL,          "WXK_CAPITAL",          true },
    { WXK_SELECT,           "WXK_SELECT",           false },
    { WXK_PRINT,            "WXK_PRINT",            false },
    { WXK_EXECUTE,          "WXK_EXECUTE",          false },
    { WXK_SNAPSHOT,         "WXK_SNAPSHOT",         true },
    { WXK_HELP,             "WXK_HELP",             false },
    { WXK_NUMLOCK,          "WXK_NUMLOCK",          true },
    { WXK_SCROLL,           "WXK_SCROLL",           true },
    { WXK_NUMPAD_INSERT,    "WXK_NUMPAD_INSERT",    false },
    { WXK_NUMPAD_DELETE,    "WXK_NUMPAD_DELETE",    false },
    { WXK_NUMPAD_SPACE,     "WXK_NUMPAD_SPACE",     false },
    { WXK_NUMPAD_TAB,       "WXK_NUMPAD_TAB",       true },
    { WXK_NUMPAD_ENTER,     "WXK_NUMPAD_ENTER",     false },
    { WXK_NUMPAD_F1,        "WXK_NUMPAD_F1",        false },
    { WXK_NUMPAD_F2,        "WXK_NUMPAD_F2",        false },
    { WXK_NUMPAD_F3,        "WXK_NUMPAD_F3",        false },
    { WXK_NUMPAD_F4,        "WXK_NUMPAD_F4",        false },
    { WXK_NUMPAD_HOME,      "WXK_NUMPAD_HOME",      false },
    { WXK_NUMPAD_LEFT,      "WXK_NUMPAD_LEFT",      false },
    { WXK_NUMPAD_UP,        "WXK_NUMPAD_UP",        false },
    { WXK_NUMPAD_RIGHT,     "WXK_NUMPAD_RIGHT",     false },
    { WXK_NUMPAD_DOWN,      "WXK_NUMPAD_DOWN",      false },
    { WXK_NUMPAD_PAGEUP,    "WXK_NUMPAD_PAGEUP",    false },
    { WXK_NUMPAD_PAGEDOWN,  "WXK_NUMPAD_PAGEDOWN",  false },
    { WXK_NUMPAD_END,       "WXK_NUMPAD_END",       false },
    { WXK_NUMPAD_BEGIN,     "WXK_NUMPAD_BEGIN",     false },
    { WXK_NUMPAD_EQUAL,     "WXK_NUMPAD_EQUAL",     false },
    { WXK_NUMPAD_MULTIPLY,  "WXK_NUMPAD_MULTIPLY",  false },
    { WXK_NUMPAD_ADD,       "WXK_NUMPAD_ADD",       false },
    { WXK_NUMPAD_SEPARATOR, "WXK_NUMPAD_SEPARATOR", false },
    { WXK_NUMPAD_SUBTRACT,  "WXK_NUMPAD_SUBTRACT",  false },
    { WXK_NUMPAD_DECIMAL,   "WXK_NUMPAD_DECIMAL",   false },
    { WXK_NUMPAD_DIVIDE,    "WXK_NUMPAD_DIVIDE",    false },
    { WXK_NUMPAD0,          "WXK_NUMPAD0",          false },
    { WXK_NUMPAD1,          "WXK_NUMPAD1",          false },
    { WXK_NUMPAD2,          "WXK_NUMPAD2",          false },
    { WXK_NUMPAD3,          "WXK_NUMPAD3",          false },
    { WXK_NUMPAD4,          "WXK_NUMPAD4",          false },
    { WXK_NUMPAD5,          "WXK_NUMPAD5",          false },
    { WXK_NUMPAD6,          "WXK_NUMPAD6",          false },
    { WXK_NUMPAD7,          "WXK_NUMPAD7",          false },
    { WXK_NUMPAD8,          "WXK_NUMPAD8",          false },
    { WXK_NUMPAD9,          "WXK_NUMPAD9",          false },
    { WXK_WINDOWS_LEFT,     "WXK_WINDOWS_LEFT",     true },
    { WXK_WINDOWS_RIGHT,    "WXK_WINDOWS_RIGHT",    true },
    { WXK_WINDOWS_MENU,     "WXK_WINDOWS_MENU",     false },
    { WXK_COMMAND,          "WXK_COMMAND",          true }
};

}

TEST_CASE( "wxMenuItemAccelEntry", "[menu][accelentry]" )
{
    wxMenu* menu = new wxMenu;

    menu->Append( wxID_ANY, "Test" );
    wxMenuItem* item = menu->FindItemByPosition( 0 );

    SECTION( "Modifier keys" )
    {
        for ( unsigned i = 0; i < WXSIZEOF(modKeys); i++ )
        {
            const key& k = modKeys[i];

            INFO( wxString::Format( "Modifier: %s",  k.name ) );
            wxAcceleratorEntry accelEntry( k.keycode, 'A' , wxID_ANY, item );
            item->SetAccel( &accelEntry );

            wxString labelText = item->GetItemLabel();
            INFO( wxString::Format( "Label text: %s", labelText ) );

            VerifyAccelAssigned( labelText, 'A' );
        }
    }

    SECTION( "Special keys" )
    {
        for ( unsigned i = 0; i < WXSIZEOF(specialKeys); i++ )
        {
            const key& k = specialKeys[i];

            if( k.skip )
                continue;

            INFO( wxString::Format( "Keycode: %s",  k.name ) );
            wxAcceleratorEntry accelEntry( wxACCEL_CTRL, k.keycode, wxID_ANY, item );
            item->SetAccel( &accelEntry );

            wxString labelText = item->GetItemLabel();
            INFO( wxString::Format( "Label text: %s", labelText ) );

            VerifyAccelAssigned( labelText, k.keycode );
        }
    }
}

#endif
