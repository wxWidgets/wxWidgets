/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/converter.cpp
// Author:      Peter Most, Kolya Kosenko
// Copyright:   (c) Peter Most
//              (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gdicmn.h"
#include "wx/gdicmn.h"

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtCore/QSize>

#if wxUSE_DATETIME
    #include "wx/datetime.h"
    #include <QtCore/QDate>
#endif // wxUSE_DATETIME

wxPoint wxQtConvertPoint( const QPoint &point )
{
    if (point.isNull())
        return wxDefaultPosition;

    return wxPoint( point.x(), point.y() );
}

QPoint wxQtConvertPoint( const wxPoint &point )
{
    if (point == wxDefaultPosition)
        return QPoint();

    return QPoint( point.x, point.y );
}


QRect wxQtConvertRect( const wxRect &rect )
{
    return QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() );
}

wxRect wxQtConvertRect( const QRect &rect )
{
    return wxRect( rect.x(), rect.y(), rect.width(), rect.height() );
}

// TODO: Check whether QString::toStdString/QString::toStdWString might be faster

wxString wxQtConvertString( const QString &str )
{
    return wxString( qPrintable( str ));
}

QString  wxQtConvertString( const wxString &str )
{
    return QString( str.utf8_str() );
}




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

#endif // wxUSE_DATETIME

wxSize wxQtConvertSize( const QSize  &size )
{
    if (size.isNull())
        return wxDefaultSize;

    return wxSize(size.width(), size.height());
}

QSize  wxQtConvertSize( const wxSize &size )
{
    if (size == wxDefaultSize)
        return QSize();

    return QSize(size.GetWidth(), size.GetHeight());
}

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
wxKeyCode wxQtConvertKeyCode( int key, const Qt::KeyboardModifiers modifiers )
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
