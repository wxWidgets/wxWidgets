/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/converter.cpp
// Author:      Peter Most, Kolya Kosenko, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
//              (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QFont>

#if wxUSE_DATETIME
    #include "wx/datetime.h"
    #include <QtCore/QDate>
    #include <QtCore/QTime>
#endif // wxUSE_DATETIME

#include "wx/qt/private/converter.h"


#if wxUSE_DATETIME

wxDateTime wxQtConvertDate(const QDate& date)
{
    if ( !date.isNull() )
        return wxDateTime(date.day(),
            static_cast<wxDateTime::Month>(date.month() - 1),
            date.year(), 0, 0, 0, 0);
    else
        return wxDateTime();
}

QDate wxQtConvertDate(const wxDateTime& date)
{
    if ( date.IsValid() )
        return QDate(date.GetYear(), date.GetMonth() + 1, date.GetDay());
    else
        return QDate();
}

wxDateTime wxQtConvertTime(const QTime& time)
{
    if ( !time.isNull() )
        return wxDateTime(time.hour(), time.minute(), time.second(), time.msec());
    else
        return wxDateTime();
}

QTime wxQtConvertTime(const wxDateTime& time)
{
    if ( time.IsValid() )
        return QTime(time.GetHour(), time.GetMinute(), time.GetSecond(), time.GetMillisecond());
    else
        return QTime();
}

#endif // wxUSE_DATETIME

Qt::Orientation wxQtConvertOrientation( long style, wxOrientation defaultOrientation )
{
    if (( style & ( wxHORIZONTAL | wxVERTICAL )) == 0 )
        style |= defaultOrientation;

    switch ( style & ( wxHORIZONTAL | wxVERTICAL ))
    {
        case wxHORIZONTAL:
            return Qt::Horizontal;

        case wxVERTICAL:
            return Qt::Vertical;
    }
    wxFAIL_MSG( "Unreachable code!" );
    return static_cast< Qt::Orientation >( -1 );
}


wxOrientation wxQtConvertOrientation( Qt::Orientation qtOrientation )
{
    switch ( qtOrientation )
    {
        case Qt::Horizontal:
            return wxHORIZONTAL;

        case Qt::Vertical:
            return wxVERTICAL;
    }
    wxFAIL_MSG( "Unreachable code!" );
    return static_cast< wxOrientation >( -1 );
}

/* Auxiliar function for key events. Returns the wx keycode for a qt one.
 * The event is needed to check it flags (numpad key or not) */
wxKeyCode wxQtConvertKeyCode( int key, Qt::KeyboardModifiers modifiers )
{
    /* First treat common ranges and then handle specific values
     * The macro takes Qt first and last codes and the first wx code
     * to make the conversion */
    #define WXQT_KEY_GROUP( firstQT, lastQT, firstWX ) \
        if ( key >= firstQT && key <= lastQT ) \
            return (wxKeyCode)(key - (firstQT - firstWX));

    if ( modifiers.testFlag( Qt::KeypadModifier ) )
    {
        // This is a numpad event
        WXQT_KEY_GROUP( Qt::Key_0, Qt::Key_9, WXK_NUMPAD0 )
        WXQT_KEY_GROUP( Qt::Key_F1, Qt::Key_F4, WXK_NUMPAD_F1 )
        WXQT_KEY_GROUP( Qt::Key_Left, Qt::Key_Down, WXK_NUMPAD_LEFT )

        // * + , - . /
        WXQT_KEY_GROUP( Qt::Key_Asterisk, Qt::Key_Slash, WXK_NUMPAD_MULTIPLY )

        switch (key)
        {
            case Qt::Key_Space:
                return WXK_NUMPAD_SPACE;
            case Qt::Key_Tab:
                return WXK_NUMPAD_TAB;
            case Qt::Key_Enter:
                return WXK_NUMPAD_ENTER;
            case Qt::Key_Home:
                return WXK_NUMPAD_HOME;
            case Qt::Key_PageUp:
                return WXK_NUMPAD_PAGEUP;
            case Qt::Key_PageDown:
                return WXK_NUMPAD_PAGEDOWN;
            case Qt::Key_End:
                return WXK_NUMPAD_END;
            case Qt::Key_Insert:
                return WXK_NUMPAD_INSERT;
            case Qt::Key_Delete:
                return WXK_NUMPAD_DELETE;
            case Qt::Key_Clear:
                return WXK_NUMPAD_BEGIN;
            case Qt::Key_Equal:
                return WXK_NUMPAD_EQUAL;
        }

        // All other possible numpads button have no equivalent in wx
        return (wxKeyCode)0;
    }

    // ASCII (basic and extended) values are the same in Qt and wx
    WXQT_KEY_GROUP( 32, 255, 32 );

    // Arrow keys
    WXQT_KEY_GROUP( Qt::Key_Left, Qt::Key_Down, WXK_LEFT )

    // F-keys (Note: Qt has up to F35, wx up to F24)
    WXQT_KEY_GROUP( Qt::Key_F1, Qt::Key_F24, WXK_F1 )

    // * + , - . /
    WXQT_KEY_GROUP( Qt::Key_Asterisk, Qt::Key_Slash, WXK_MULTIPLY )

    // Special keys in wx. Seems most appropriate to map to LaunchX
    WXQT_KEY_GROUP( Qt::Key_Launch0, Qt::Key_LaunchF, WXK_SPECIAL1 )

    // All other cases
    switch ( key )
    {
        case Qt::Key_Backspace:
            return WXK_BACK;
        case Qt::Key_Tab:
            return WXK_TAB;
        case Qt::Key_Return:
            return WXK_RETURN;
        case Qt::Key_Escape:
            return WXK_ESCAPE;
        case Qt::Key_Cancel:
            return WXK_CANCEL;
        case Qt::Key_Clear:
            return WXK_CLEAR;
        case Qt::Key_Shift:
            return WXK_SHIFT;
        case Qt::Key_Alt:
            return WXK_ALT;
        case Qt::Key_Control:
            return WXK_CONTROL;
        case Qt::Key_Menu:
            return WXK_MENU;
        case Qt::Key_Pause:
            return WXK_PAUSE;
        case Qt::Key_CapsLock:
            return WXK_CAPITAL;
        case Qt::Key_End:
            return WXK_END;
        case Qt::Key_Home:
            return WXK_HOME;
        case Qt::Key_Select:
            return WXK_SELECT;
        case Qt::Key_SysReq:
            return WXK_PRINT;
        case Qt::Key_Execute:
            return WXK_EXECUTE;
        case Qt::Key_Insert:
            return WXK_INSERT;
        case Qt::Key_Delete:
            return WXK_DELETE;
        case Qt::Key_Help:
            return WXK_HELP;
        case Qt::Key_NumLock:
            return WXK_NUMLOCK;
        case Qt::Key_ScrollLock:
            return WXK_SCROLL;
        case Qt::Key_PageUp:
            return WXK_PAGEUP;
        case Qt::Key_PageDown:
            return WXK_PAGEDOWN;
        case Qt::Key_Meta:
            return WXK_WINDOWS_LEFT;
    }

    // Missing wx-codes: WXK_START, WXK_LBUTTON, WXK_RBUTTON, WXK_MBUTTON
    // WXK_SPECIAL(17-20), WXK_WINDOWS_RIGHT, WXK_WINDOWS_MENU, WXK_COMMAND
    // WXK_SNAPSHOT

    return (wxKeyCode)0;

    #undef WXQT_KEY_GROUP
}

void wxQtFillKeyboardModifiers( Qt::KeyboardModifiers modifiers, wxKeyboardState *state )
{
    state->SetControlDown( modifiers.testFlag( Qt::ControlModifier ) );
    state->SetShiftDown( modifiers.testFlag( Qt::ShiftModifier ) );
    state->SetAltDown( modifiers.testFlag( Qt::AltModifier ) );
    state->SetMetaDown( modifiers.testFlag( Qt::MetaModifier ) );
}


int wxQtConvertKeyCode( int key, int WXUNUSED(modifiers), Qt::KeyboardModifiers &qtmodifiers )
{
    /* First treat common ranges and then handle specific values
     * The macro takes Qt first and last codes and the first wx code
     * to make the conversion */
    #define QTWX_KEY_GROUP( firstWX, lastWX, firstQT ) \
        if ( key >= firstWX && key <= lastWX ) \
            return (int)(key - (firstWX - firstQT));

    qtmodifiers |= Qt::KeypadModifier;
    // This is a numpad event
    QTWX_KEY_GROUP( WXK_NUMPAD0, WXK_NUMPAD9, Qt::Key_0 )
    QTWX_KEY_GROUP( WXK_NUMPAD_F1, WXK_NUMPAD_F4, Qt::Key_F1 )
    QTWX_KEY_GROUP( WXK_NUMPAD_LEFT, WXK_NUMPAD_DOWN, Qt::Key_Left )

    // * + , - . /
    QTWX_KEY_GROUP( WXK_NUMPAD_MULTIPLY, WXK_NUMPAD_DIVIDE, Qt::Key_Asterisk )

    switch (key)
    {
        case WXK_NUMPAD_SPACE:
            return Qt::Key_Space;
        case WXK_NUMPAD_TAB:
            return Qt::Key_Tab;
        case WXK_NUMPAD_ENTER:
            return Qt::Key_Enter;
        case WXK_NUMPAD_HOME:
            return Qt::Key_Home;
        case WXK_NUMPAD_PAGEUP:
            return Qt::Key_PageUp;
        case WXK_NUMPAD_PAGEDOWN:
            return Qt::Key_PageDown;
        case WXK_NUMPAD_END:
            return Qt::Key_End;
        case WXK_NUMPAD_INSERT:
            return Qt::Key_Insert;
        case WXK_NUMPAD_DELETE:
            return Qt::Key_Delete;
        case WXK_NUMPAD_BEGIN:
            return Qt::Key_Clear;
        case WXK_NUMPAD_EQUAL:
            return Qt::Key_Equal;
    }

    qtmodifiers &= ~Qt::KeypadModifier;

    // ASCII letters are only supported in uppercase in Qt
    QTWX_KEY_GROUP( 0x61, 0x7a, 0x41 );

    // ASCII (basic) values are the same in Qt and wx
    QTWX_KEY_GROUP( 32, 126, 32 );

    // ASCII (extended) values are the same in Qt but has holes
    // see qasciikey.cpp char QTest::keyToAscii(Qt::Key key)
    // WARNING: not translated as there is an assert in keyToAscii

    // Arrow keys
    QTWX_KEY_GROUP( WXK_LEFT, WXK_DOWN, Qt::Key_Left )

    // F-keys (Note: Qt has up to F35, wx up to F24)
    QTWX_KEY_GROUP( WXK_F1, WXK_F24, Qt::Key_F1 )

    // * + , - . /
    QTWX_KEY_GROUP( WXK_MULTIPLY, WXK_DIVIDE, Qt::Key_Asterisk )

    // Special keys in wx. Seems most appropriate to map to LaunchX
    QTWX_KEY_GROUP( WXK_SPECIAL1, WXK_SPECIAL20, Qt::Key_Launch0 )

    // All other cases
    switch ( key )
    {
        case WXK_BACK:
            return Qt::Key_Backspace;
        case WXK_TAB:
            return Qt::Key_Tab;
        case WXK_RETURN:
            return Qt::Key_Return;
        case WXK_ESCAPE:
            return Qt::Key_Escape;
        case WXK_CANCEL:
            return Qt::Key_Cancel;
        case WXK_CLEAR:
            return Qt::Key_Clear;
        case WXK_SHIFT:
            return Qt::Key_Shift;
        case WXK_ALT:
            return Qt::Key_Alt;
        case WXK_CONTROL:
            return Qt::Key_Control;
        case WXK_MENU:
            return Qt::Key_Menu;
        case WXK_PAUSE:
            return Qt::Key_Pause;
        case WXK_CAPITAL:
            return Qt::Key_CapsLock;
        case WXK_END:
            return Qt::Key_End;
        case WXK_HOME:
            return Qt::Key_Home;
        case WXK_SELECT:
            return Qt::Key_Select;
        case WXK_PRINT:
            return Qt::Key_SysReq;
        case WXK_EXECUTE:
            return Qt::Key_Execute;
        case WXK_INSERT:
            return Qt::Key_Insert;
        case WXK_DELETE:
            return Qt::Key_Delete;
        case WXK_HELP:
            return Qt::Key_Help;
        case WXK_NUMLOCK:
            return Qt::Key_NumLock;
        case WXK_SCROLL:
            return Qt::Key_ScrollLock;
        case WXK_PAGEUP:
            return Qt::Key_PageUp;
        case WXK_PAGEDOWN:
            return Qt::Key_PageDown;
        case WXK_WINDOWS_LEFT:
            return Qt::Key_Meta;
    }

    // Missing wx-codes: WXK_START, WXK_LBUTTON, WXK_RBUTTON, WXK_MBUTTON
    // WXK_SPECIAL(17-20), WXK_WINDOWS_RIGHT, WXK_WINDOWS_MENU, WXK_COMMAND
    // WXK_SNAPSHOT

    return (wxKeyCode)0;

    #undef QTWX_KEY_GROUP
}
