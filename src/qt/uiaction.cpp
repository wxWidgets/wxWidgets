/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/uiaction.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/private/uiaction.h"

// Apparently {mouse,key}Event() functions signature has changed from QWidget to
// QWindow at some time during Qt5.  Fortunately, we can continue to use the API
// taking QWidget by defining QT_WIDGETS_LIB before including the test headers.
#define QT_WIDGETS_LIB

#include <QtTest/QtTestGui>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "wx/qt/defs.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"


using namespace Qt;
using namespace QTest;

class wxUIActionSimulatorQtImpl : public wxUIActionSimulatorImpl
{
public:
    // Returns a pointer to the global simulator object: as it's stateless, we
    // can reuse the same one without having to allocate it on the heap all the
    // time.
    static wxUIActionSimulatorQtImpl* Get()
    {
        static wxUIActionSimulatorQtImpl s_impl;
        return &s_impl;
    }

    virtual bool MouseMove(long x, long y) override;
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool DoKey(int keycode, int modifiers, bool isDown) override;

private:
    // This class has no public ctors, use Get() instead.
    wxUIActionSimulatorQtImpl() { m_mousePosition = QCursor::pos(); }

    QPoint m_mousePosition;

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorQtImpl);
};

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


static bool SimulateMouseButton( MouseAction mouseAction,
                                 MouseButton mouseButton,
                                 QPoint mousePosition,
                                 Qt::KeyboardModifiers modifiers = Qt::NoModifier )
{
    QWidget *widget = QApplication::widgetAt( mousePosition );

    if ( !widget )
        return false;

    const QPoint pos = widget->mapFromGlobal(mousePosition);

    // Notice that windowHandle() returns a valid handle for native widgets only.
    widget->windowHandle() != nullptr ?
        mouseEvent( mouseAction, widget->windowHandle(), mouseButton, modifiers, pos ) :
        mouseEvent( mouseAction, widget, mouseButton, modifiers, pos );

    // If we found a widget then we successfully simulated an event:

    return true;
}

static bool SimulateKeyboardKey( KeyAction keyAction, Key key )
{
    QWidget *widget = QApplication::focusWidget();

    if ( !widget )
        return false;

    widget->windowHandle() != nullptr ?
        keyEvent( keyAction, widget->windowHandle(), key ) :
        keyEvent( keyAction, widget, key );

    // If we found a widget then we successfully simulated an event:

    return true;
}

bool wxUIActionSimulatorQtImpl::MouseDown(int button)
{
    return SimulateMouseButton( MousePress, ConvertMouseButton( button ), m_mousePosition );
}

bool wxUIActionSimulatorQtImpl::MouseUp(int button)
{
    return SimulateMouseButton( MouseRelease, ConvertMouseButton( button ), m_mousePosition );
}

bool wxUIActionSimulatorQtImpl::MouseMove(long x, long y)
{
    m_mousePosition = QPoint(x, y);

    return SimulateMouseButton( QTest::MouseMove, NoButton, m_mousePosition );
}

bool wxUIActionSimulatorQtImpl::DoKey(int keyCode, int modifiers, bool isDown)
{
    Qt::KeyboardModifiers qtmodifiers;
    enum Key key;

    key = (enum Key) wxQtConvertKeyCode( keyCode, modifiers, qtmodifiers );

    wxCHECK_MSG(key, false, wxT("No current key conversion equivalent in Qt"));
    KeyAction keyAction = isDown ? Press : Release;
    return SimulateKeyboardKey( keyAction, key );
}


wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(wxUIActionSimulatorQtImpl::Get())
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    // We can use a static wxUIActionSimulatorQtImpl object because it's
    // stateless, so no need to delete it.
}

#endif // wxUSE_UIACTIONSIMULATOR
