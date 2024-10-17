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
#ifndef QT_WIDGETS_LIB
#define QT_WIDGETS_LIB
#endif

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

    virtual bool MouseClick(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseDblClick(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool DoKey(int keycode, int modifiers, bool isDown) override;

private:
    // This class has no public ctors, use Get() instead.
    wxUIActionSimulatorQtImpl() { m_mousePosition = QCursor::pos(); }

    // Simulating mouse clicks with one or more modifiers only works if the modifier(s)
    // is/are passed along with the mouse click. We just put SaveModifierForMouseClicks()
    // inside DoKey() to remember which modifier(s) is/are currently pressed and pass that
    // information to SimulateMouseButton() so code like the following works as expected:
    //
    //    sim.KeyDown(WXK_SHIFT);
    //    sim.MouseClick();
    //    sim.KeyUp(WXK_SHIFT);
    //
    void SaveModifierForMouseClicks(int keycode, bool isDown)
    {
        switch ( keycode )
        {
            default:
                wxFALLTHROUGH;

            case WXK_SHIFT:
                isDown ? m_modifiers |= wxMOD_SHIFT
                       : m_modifiers &= ~wxMOD_SHIFT;
                break;
            case WXK_ALT:
                isDown ? m_modifiers |= wxMOD_ALT
                       : m_modifiers &= ~wxMOD_ALT;
                break;
            case WXK_CONTROL:
                isDown ? m_modifiers |= wxMOD_CONTROL
                       : m_modifiers &= ~wxMOD_CONTROL;
                break;
        }
    }

    int m_modifiers = wxMOD_NONE; // for mouse clicks only
    QPoint m_mousePosition;

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorQtImpl);
};

static Qt::KeyboardModifiers ConvertToQtModifiers(int modifiers)
{
    Qt::KeyboardModifiers qtmodifiers = Qt::NoModifier;

    if ( modifiers & wxMOD_SHIFT )
        qtmodifiers |= Qt::ShiftModifier;
    if ( modifiers & wxMOD_ALT )
        qtmodifiers |= Qt::AltModifier;
    if ( modifiers & wxMOD_CONTROL )
        qtmodifiers |= Qt::ControlModifier;

    return qtmodifiers;
}

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

static bool SimulateKeyboardKey( KeyAction keyAction, Key key, int modifiers )
{
    QWidget *widget = QApplication::focusWidget();

    if ( !widget )
        return false;

    // I don't know if this is a bug in QTest or not, but simulating Shift+Char
    // always produces a lowercase of that char! (which must be in uppercase)
    if ( modifiers == wxMOD_SHIFT && key < 256 )
    {
        const QChar qChar(key);
        if ( qChar.isLetter() )
        {
            widget->windowHandle() != nullptr ?
                sendKeyEvent( keyAction, widget->windowHandle(), key,
                              QString(qChar.toUpper()), Qt::ShiftModifier ) :
                sendKeyEvent( keyAction, widget, key,
                              QString(qChar.toUpper()), Qt::ShiftModifier );
            return true;
        }
    }

    widget->windowHandle() != nullptr ?
        keyEvent( keyAction, widget->windowHandle(), key, ConvertToQtModifiers(modifiers) ) :
        keyEvent( keyAction, widget, key, ConvertToQtModifiers(modifiers) );

    // If we found a widget then we successfully simulated an event:

    return true;
}

bool wxUIActionSimulatorQtImpl::MouseDown(int button)
{
    return SimulateMouseButton( MousePress, ConvertMouseButton( button ),
                                m_mousePosition, ConvertToQtModifiers( m_modifiers ) );
}

bool wxUIActionSimulatorQtImpl::MouseUp(int button)
{
    return SimulateMouseButton( MouseRelease, ConvertMouseButton( button ),
                                m_mousePosition, ConvertToQtModifiers( m_modifiers ) );
}

bool wxUIActionSimulatorQtImpl::MouseMove(long x, long y)
{
    m_mousePosition = QPoint(x, y);

    return SimulateMouseButton( QTest::MouseMove, NoButton, m_mousePosition );
}

bool wxUIActionSimulatorQtImpl::MouseClick(int button)
{
    return SimulateMouseButton( QTest::MouseClick, ConvertMouseButton( button ),
                                m_mousePosition, ConvertToQtModifiers( m_modifiers ) );
}

bool wxUIActionSimulatorQtImpl::MouseDblClick(int button)
{
    return SimulateMouseButton( QTest::MouseDClick, ConvertMouseButton( button ), m_mousePosition );
}

bool wxUIActionSimulatorQtImpl::DoKey(int keyCode, int modifiers, bool isDown)
{
    SaveModifierForMouseClicks(keyCode, isDown);

    Qt::KeyboardModifiers qtmodifiers;
    enum Key key = (enum Key) wxQtConvertKeyCode( keyCode, modifiers, qtmodifiers );

    wxCHECK_MSG(key, false, wxT("No current key conversion equivalent in Qt"));
    KeyAction keyAction = isDown ? Press : Release;
    return SimulateKeyboardKey( keyAction, key, modifiers );
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
