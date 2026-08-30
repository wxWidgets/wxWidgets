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
        FAIL( "unknown event type" );

    msg += " event at line ";
    msg += wxString::Format("%d", line).mb_str();

    INFO("wrong key code in " + msg);
    CHECK(ev.GetKeyCode() == desc.m_keycode);

    if ( desc.m_keycode < WXK_START )
    {
        // For Latin-1 our key code is the same as Unicode character value.
        INFO("wrong Unicode key in " + msg);
        CHECK((char)ev.GetUnicodeKey() == (char)desc.m_keycode);
    }
    else // Special key
    {
        // Key codes above WXK_START don't correspond to printable characters.
        INFO("wrong non-zero Unicode key in " + msg);
        CHECK((int)ev.GetUnicodeKey() == 0);
    }

    INFO("wrong modifiers in " + msg);
    CHECK(ev.GetModifiers() == desc.m_mods);
}

// Call TestEvent() passing it the line number from where it was called: this
// is useful for interpreting the assert failure messages.
#define ASSERT_KEY_EVENT_IS( ev, desc ) TestEvent(__LINE__, ev, desc)

} // anonymous namespace

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class KeyboardEventTestCase
{
public:
    KeyboardEventTestCase();
    ~KeyboardEventTestCase();

protected:
    KeyboardTestWindow *m_win;

    wxDECLARE_NO_COPY_CLASS(KeyboardEventTestCase);
};

KeyboardEventTestCase::KeyboardEventTestCase()
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

KeyboardEventTestCase::~KeyboardEventTestCase()
{
    m_win->Destroy();
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::NormalLetter",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

#ifdef __WXQT__
    WARN("FIXME! doesn't work like the other ports.");
#else
    wxUIActionSimulator sim;
    sim.Char('a');
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(), 'A' );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(), 'a' );

    CHECK( m_win->GetKeyUpCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(), 'A' );
#endif
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::NormalSpecial",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;
    sim.Char(WXK_END);
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(), WXK_END );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(), WXK_END );

    CHECK( m_win->GetKeyUpCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(), WXK_END );
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::CtrlLetter",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

#ifdef __WXQT__
    WARN("FIXME! doesn't work like the other ports.");
#else
    wxUIActionSimulator sim;
    sim.Char('z', wxMOD_CONTROL);
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc('Z', wxMOD_CONTROL) );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc('\x1a', wxMOD_CONTROL) );

    CHECK( m_win->GetKeyUpCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc('Z', wxMOD_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_CONTROL) );
#endif
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::CtrlSpecial",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;
    sim.Char(WXK_PAGEUP, wxMOD_CONTROL);
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );

    CHECK( m_win->GetKeyUpCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc(WXK_PAGEUP, wxMOD_CONTROL) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_CONTROL) );
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::ShiftLetter",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;
    sim.Char('Q', wxMOD_SHIFT);
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc('Q', wxMOD_SHIFT) );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc('Q', wxMOD_SHIFT) );

    CHECK( m_win->GetKeyUpCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc('Q', wxMOD_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_SHIFT) );
}

TEST_CASE_METHOD(KeyboardEventTestCase, "KeyboardEvent::ShiftSpecial",
                 "[keyboard]")
{
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;
    sim.Char(WXK_F3, wxMOD_SHIFT);
    wxYield();

    CHECK( m_win->GetKeyDownCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(0),
                         ModKeyDown(WXK_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyDownEvent(1),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );

    CHECK( m_win->GetCharCount() == 1 );
    ASSERT_KEY_EVENT_IS( m_win->GetCharEvent(),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );

    CHECK( m_win->GetKeyUpCount() == 2 );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(0),
                         KeyDesc(WXK_F3, wxMOD_SHIFT) );
    ASSERT_KEY_EVENT_IS( m_win->GetKeyUpEvent(1),
                         ModKeyUp(WXK_SHIFT) );
}

#endif // wxUSE_UIACTIONSIMULATOR
