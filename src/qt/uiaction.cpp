/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/uiaction.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/defs.h"
#include "wx/qt/utils.h"
#include "wx/uiaction.h"

#include <QtTest/QtTestGui>

#if wxUSE_UIACTIONSIMULATOR

using namespace Qt;
using namespace QTest;

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
        mouseEvent( mouseAction, widget, mouseButton, NoModifier, mousePosition );

    // If we found a widget then we successfully simulated an event:

    return widget != NULL;
}

static bool SimulateKeyboardKey( KeyAction keyAction, Key key )
{
    QWidget *widget = QApplication::focusWidget();
    if ( widget != NULL )
        keyEvent( keyAction, widget, key );

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
    enum Key key;

    switch ( keyCode )
    {
        case WXK_PAGEUP:
            key = Key_PageUp;
            break;
    }
    KeyAction keyAction = isDown ? Press : Release;
    return SimulateKeyboardKey( keyAction, key );
}

#endif // wxUSE_UIACTIONSIMULATOR

