/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/utils.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QCursor>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

#ifndef WX_PRECOMP
    #include "wx/cursor.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/utils.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"


void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] )
{
    // Make it look similar to the assert messages:

    fprintf( stderr, "%s(%u): Missing implementation of \"%s\"\n", fileName, lineNumber, feature );
}

void wxQtFillMouseButtons( Qt::MouseButtons buttons, wxMouseState *state )
{
    state->SetLeftDown( buttons.testFlag( Qt::LeftButton ) );
    state->SetRightDown( buttons.testFlag( Qt::RightButton ) );
    state->SetMiddleDown( buttons.testFlag( Qt::MiddleButton ) );
    state->SetAux1Down( buttons.testFlag( Qt::XButton1 ) );
    state->SetAux2Down( buttons.testFlag( Qt::XButton2 ) );
}

#if wxUSE_GUI
wxPoint wxGetMousePosition()
{
    return wxQtConvertPoint( QCursor::pos() );
}

void wxGetMousePosition( int *x, int *y )
{
    wxPoint position = wxGetMousePosition();

    *x = position.x;
    *y = position.y;
}
#endif

#if wxUSE_GUI
wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    wxQtFillMouseButtons( QApplication::mouseButtons(), &ms );

    return ms;
}
#endif


wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    /* Another option is to use QApplication::topLevelAt()
     * but that gives the QWidget so the wxWindow list must
     * be traversed comparing with this, or use the pointer from
     * a wxQtWidget/wxQtFrame to the window, but they have
     * no standard interface to return that. */
    return wxGenericFindWindowAtPoint( pt );
}

wxWindow *wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxQtConvertPoint( QCursor::pos() );

    return wxFindWindowAtPoint( pt );
}

bool wxGetKeyState(wxKeyCode key)
{
    /* FIXME: Qt doesn't provide a method to check the state of keys others
     * than modifiers (shift, control, alt, meta). A platform-specific method
     * is needed, probably one per platform Qt runs on. */
    switch ( key )
    {
        case WXK_CONTROL:
            return QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
        case WXK_SHIFT:
            return QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
        case WXK_ALT:
            return QApplication::keyboardModifiers().testFlag(Qt::AltModifier);
        case WXK_WINDOWS_LEFT:
            return QApplication::keyboardModifiers().testFlag(Qt::MetaModifier);
        default:
            wxMISSING_IMPLEMENTATION( "wxGetKeyState for non-modifiers keys" );
            return false;
    }
}

void wxBell()
{
    QApplication::beep();
}

wxWindow *wxGetActiveWindow()
{
    QWidget *w = QApplication::activeWindow();

    wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetLast();
    while (node)
    {
        wxWindow* win = node->GetData();
        if ( win->GetHandle() == w )
            return win;

        node = node->GetPrevious();
    }

    return nullptr;
}

bool wxLaunchDefaultApplication(const wxString& path, int WXUNUSED( flags ) )
{
    return QDesktopServices::openUrl( QUrl::fromLocalFile( wxQtConvertString( path ) ) );
}
