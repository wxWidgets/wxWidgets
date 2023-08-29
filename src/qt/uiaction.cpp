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

#include <QtTest/QtTestGui>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "wx/qt/defs.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"


using namespace Qt;
using namespace QTest;

// Apparently {mouse,key}Event() functions signature has changed from QWidget
// to QWindow at some time during Qt5, but we don't know when exactly. We do
// know that they take QWindow for 5.2 and, presumably, later versions (but not
// for whichever version this code was originally written for).
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
inline QWindow* argForEvents(QWidget* w) { return w->windowHandle(); }
#else
inline QWidget* argForEvents(QWidget* w) { return w; }
#endif

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
    wxUIActionSimulatorQtImpl() { }

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


static bool SimulateMouseButton( MouseAction mouseAction, MouseButton mouseButton )
{
    QPoint mousePosition = QCursor::pos();
    QWidget *widget = QApplication::widgetAt( mousePosition );
    if ( widget != nullptr )
        mouseEvent( mouseAction, argForEvents(widget), mouseButton, NoModifier, mousePosition );

    // If we found a widget then we successfully simulated an event:

    return widget != nullptr;
}

static bool SimulateKeyboardKey( KeyAction keyAction, Key key )
{
    QWidget *widget = QApplication::focusWidget();
    if ( widget != nullptr )
        keyEvent( keyAction, argForEvents(widget), key );

    // If we found a widget then we successfully simulated an event:

    return widget != nullptr;
}

bool wxUIActionSimulatorQtImpl::MouseDown( int button )
{
    return SimulateMouseButton( MousePress, ConvertMouseButton( button ));
}

bool wxUIActionSimulatorQtImpl::MouseUp(int button)
{
    return SimulateMouseButton( MouseRelease, ConvertMouseButton( button ));
}

bool wxUIActionSimulatorQtImpl::MouseMove(long x, long y)
{
    QCursor::setPos( x, y );

    return true;
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
