///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/keyboard.cpp
// Purpose:     Test keyboard events
// Author:      Vadim Zeitlin
// Created:     2010-09-05
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_UIACTIONSIMULATOR

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/uiaction.h"
#include "wx/vector.h"

#include "waitfor.h"

namespace
{

// ----------------------------------------------------------------------------
// test window verifying the event generation
// ----------------------------------------------------------------------------

class KeyboardTestWindow : public wxWindow
{
public:
    KeyboardTestWindow(wxWindow *parent)
        : wxWindow(parent, wxID_ANY, wxPoint(0, 0), parent->GetClientSize())
    {
        Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(KeyboardTestWindow::OnKeyDown));
        Connect(wxEVT_CHAR, wxKeyEventHandler(KeyboardTestWindow::OnChar));
        Connect(wxEVT_KEY_UP, wxKeyEventHandler(KeyboardTestWindow::OnKeyUp));
    }

    unsigned GetKeyDownCount() const { return m_keyDownEvents.size(); }
    unsigned GetCharCount() const { return m_charEvents.size(); }
    unsigned GetKeyUpCount() const { return m_keyUpEvents.size(); }

    const wxKeyEvent& GetKeyDownEvent(unsigned n = 0) const
    {
        return m_keyDownEvents[n];
    }
    const wxKeyEvent& GetCharEvent(unsigned n = 0) const
    {
        return m_charEvents[n];
    }
    const wxKeyEvent& GetKeyUpEvent(unsigned n = 0) const
    {
        return m_keyUpEvents[n];
    }

    void ClearEvents()
    {
        m_keyDownEvents =
        m_charEvents =
        m_keyUpEvents = wxVector<wxKeyEvent>();
    }

private:
    void OnKeyDown(wxKeyEvent& event)
    {
        m_keyDownEvents.push_back(event);
        event.Skip();
    }

    void OnChar(wxKeyEvent& event)
    {
        m_charEvents.push_back(event);
        event.Skip();
    }

    void OnKeyUp(wxKeyEvent& event)
    {
        m_keyUpEvents.push_back(event);
        event.Skip();
    }

    wxVector<wxKeyEvent> m_keyDownEvents,
                         m_charEvents,
                         m_keyUpEvents;


    wxDECLARE_NO_COPY_CLASS(KeyboardTestWindow);
};

// Object describing the (main fields of) keyboard event.
struct KeyDesc
{
    KeyDesc(int keycode, int mods = 0)
        : m_keycode(keycode),
          m_mods(mods)
    {
    }

    int m_keycode;
    int m_mods;
};

// Helper for ModKeyDown().
int GetModForKey(int keycode)
{
    switch ( keycode )
    {
        case WXK_CONTROL:   return wxMOD_CONTROL;
        case WXK_SHIFT:     return wxMOD_SHIFT;
        case WXK_ALT:       return wxMOD_ALT;
        default:
            wxFAIL_MSG( "Unknown modifier key" );
    }

    return wxMOD_NONE;
}

// Helper function to allow writing just ModKeyDown(WXK_CONTROL) instead of
// more verbose KeyDesc(WXK_CONTROL, wxMOD_CONTROL).
KeyDesc ModKeyDown(int keycode)
{
    return KeyDesc(keycode, GetModForKey(keycode));
}

// Another helper provided for symmetry with ModKeyDown() only.
KeyDesc ModKeyUp(int keycode)
{
    return KeyDesc(keycode);
}

// Verify that the event object corresponds to our idea of what it should be.
void TestEvent(int line, const wxKeyEvent& ev, const KeyDesc& desc)
{
    // Construct the message we'll display if an assert fails.
    std::string msg;
    const wxEventType t = ev.GetEventType();
    if ( t == wxEVT_KEY_DOWN )
        msg = "key down";
    else if ( t == wxEVT_CHAR )
        msg = "char";
    else if ( t == wxEVT_KEY_UP )
        msg = "key up";
    else
        CPPUNIT_FAIL( "unknown event type" );

    msg += " event at line ";
    msg += wxString::Format("%d", line).mb_str();


    CPPUNIT_ASSERT_EQUAL_MESSAGE( "wrong key code in " + msg,
                                  desc.m_keycode,
                                  ev.GetKeyCode() );

    if ( desc.m_keycode < WXK_START )
    {
        // For Latin-1 our key code is the same as Unicode character value.
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "wrong Unicode key in " + msg,
                                      (char)desc.m_keycode,
                                      (char)ev.GetUnicodeKey() );
    }
    else // Special key
    {
        // Key codes above WXK_START don't correspond to printable characters.
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "wrong non-zero Unicode key in " + msg,
                                      0,
                                      (int)ev.GetUnicodeKey() );
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "wrong modifiers in " + msg,
                                  desc.m_mods,
                                  ev.GetModifiers() );
}

// Call TestEvent() passing it the line number from where it was called: this
// is useful for interpreting the assert failure messages.
#define ASSERT_KEY_EVENT_IS( ev, desc ) TestEvent(__LINE__, ev, desc)

} // anonymous namespace

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class KeyboardEventTestCase : public CppUnit::TestCase
{
public:
    KeyboardEventTestCase() {}

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( KeyboardEventTestCase );
        WXUISIM_TEST( NormalLetter );
        WXUISIM_TEST( NormalSpecial );
        WXUISIM_TEST( CtrlLetter );
        WXUISIM_TEST( CtrlSpecial );
        WXUISIM_TEST( ShiftLetter );
        WXUISIM_TEST( ShiftSpecial );
    CPPUNIT_TEST_SUITE_END();

    void NormalLetter();
    void NormalSpecial();
    void CtrlLetter();
    void CtrlSpecial();
    void ShiftLetter();
    void ShiftSpecial();

    KeyboardTestWindow *m_win;

    wxDECLARE_NO_COPY_CLASS(KeyboardEventTestCase);
};

wxREGISTER_UNIT_TEST(KeyboardEvent);

void KeyboardEventTestCase::setUp()
{
    m_win = new KeyboardTestWindow(wxTheApp->GetTopWindow());
    wxYield();
    m_win->SetFocus();

    YieldForAWhile(10); // needed to show the new window

    // The window might get some key up events when it's being shown if the key
    // was pressed when the program was started and released after the window
    // was shown, e.g. this does happen in practice when launching the test
    // from command line. Simply discard all the spurious events so far.
    m_win->ClearEvents();
}

void KeyboardEventTestCase::tearDown()
{
    m_win->Destroy();
}

void KeyboardEventTestCase::NormalLetter()
{
#ifdef __WXQT__
    WARN("FIXME! doesn't work like the other ports.");
#else
    wxUIActionSimulator sim;
    sim.Char('a');
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(), 'A' );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(), 'a' );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(), 'A' );
#endif
}

void KeyboardEventTestCase::NormalSpecial()
{
    wxUIActionSimulator sim;
    sim.Char(WXK_END);
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(), WXK_END );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(), WXK_END );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(), WXK_END );
}

void KeyboardEventTestCase::CtrlLetter()
{
#ifdef __WXQT__
    WARN("FIXME! doesn't work like the other ports.");
#else
    wxUIActionSimulator sim;
    sim.Char('z', wxMOD_CONTROL);
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc('Z', wxMOD_CONTROL) );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc('\x1a', wxMOD_CONTROL) );

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc('Z', wxMOD_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_CONTROL) );
#endif
}

void KeyboardEventTestCase::CtrlSpecial()
{
    wxUIActionSimulator sim;
    sim.Char(WXK_PAGEUP, wxMOD_CONTROL);
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_CONTROL) );
}

void KeyboardEventTestCase::ShiftLetter()
{
    wxUIActionSimulator sim;
    sim.Char('Q', wxMOD_SHIFT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc('Q', wxMOD_SHIFT) );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc('Q', wxMOD_SHIFT) );

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc('Q', wxMOD_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_SHIFT) );
}

void KeyboardEventTestCase::ShiftSpecial()
{
    wxUIActionSimulator sim;
    sim.Char(WXK_F3, wxMOD_SHIFT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyDownCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );

    CPPUNIT_ASSERT_EQUAL( 1, m_win->GetCharCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );

    CPPUNIT_ASSERT_EQUAL( 2, m_win->GetKeyUpCount() );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_SHIFT) );
}

#endif // wxUSE_UIACTIONSIMULATOR
