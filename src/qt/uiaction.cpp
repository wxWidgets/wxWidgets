/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/uiaction.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <QtTest/QtTestGui>
#include <QApplication>
#include <QWidget>

#include "wx/qt/defs.h"
#include "wx/qt/private/utils.h"
#include "wx/uiaction.h"
#include "wx/qt/private/converter.h"


#if wxUSE_UIACTIONSIMULATOR

using namespace Qt;
using namespace QTest;

// Apparently {mouse,key}Event() functions signature has changed from QWidget
// to QWindow at some time during Qt5, but we don't know when exactly. We do
// know that they take QWindow for 5.3 and, presumably, later versions (but not
// for whichever version this code was originally written for).
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
inline QWindow* argForEvents(QWidget* w) { return w->windowHandle(); }
#else
inline QWidget* argForEvents(QWidget* w) { return w; }
#endif

static MouseButton ConvertMouseButton( int button )
{
    MouseButton qtButton;

    switch ( button )
    {
        case wxMOUSE_BTN_LEFT:
            qtButton = LeftButton;
            break;

        case wxMOUSE_BTN_MIDDLE:
            qtButton = MiddleButton;
            break;

        case wxMOUSE_BTN_RIGHT:
            qtButton = RightButton;
            break;

        case wxMOUSE_BTN_AUX1:
            qtButton = XButton1;
            break;

        case wxMOUSE_BTN_AUX2:
            qtButton = XButton2;
            break;

        default:
            wxFAIL_MSG( "Unsupported mouse button" );
            qtButton = NoButton;
            break;
    }
    return qtButton;
}


static bool SimulateMouseButton( MouseAction mouseAction, MouseButton mouseButton )
{
    QPoint mousePosition = QCursor::pos();
    QWidget *widget = QApplication::widgetAt( mousePosition );
    if ( widget != NULL )
        mouseEvent( mouseAction, argForEvents(widget), mouseButton, NoModifier, mousePosition );

    // If we found a widget then we successfully simulated an event:

    return widget != NULL;
}

static bool SimulateKeyboardKey( KeyAction keyAction, Key key )
{
    QWidget *widget = QApplication::focusWidget();
    if ( widget != NULL )
        keyEvent( keyAction, argForEvents(widget), key );

    // If we found a widget then we successfully simulated an event:

    return widget != NULL;
}

bool wxUIActionSimulator::MouseDown( int button )
{
    return SimulateMouseButton( MousePress, ConvertMouseButton( button ));
}

bool wxUIActionSimulator::MouseUp(int button)
{
    return SimulateMouseButton( MouseRelease, ConvertMouseButton( button ));
}

bool wxUIActionSimulator::MouseMove(long x, long y)
{
    QCursor::setPos( x, y );

    return true;
}

bool wxUIActionSimulator::DoKey(int keyCode, int modifiers, bool isDown)
{
    Qt::KeyboardModifiers qtmodifiers;
    enum Key key;

    key = (enum Key) wxQtConvertKeyCode( keyCode, modifiers, qtmodifiers );

    wxCHECK_MSG(key, false, wxT("No current key conversion equivalent in Qt"));
    KeyAction keyAction = isDown ? Press : Release;
    return SimulateKeyboardKey( keyAction, key );
}

#endif // wxUSE_UIACTIONSIMULATOR

