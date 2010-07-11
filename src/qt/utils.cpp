/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/utils.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/utils.h"
#include "wx/cursor.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

#include <QtGui/QCursor>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] )
{
    // Make it look similar to the assert messages:

    fprintf( stderr, "%s(%d): Missing implementation of \"%s\"\n", fileName, lineNumber, feature );
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
    return wxMouseState();
}
#endif


wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}

wxWindow *wxFindWindowAtPointer(wxPoint& pt)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}

bool wxGetKeyState(wxKeyCode key)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}

int wxDisplayDepth()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
}

void wxDisplaySize(int *width, int *height)
{
    *width = QApplication::desktop()->width();
    *height = QApplication::desktop()->height();
}

void wxDisplaySizeMM(int *width, int *height)
{
    *width = QApplication::desktop()->widthMM();
    *height = QApplication::desktop()->heightMM();
}

void wxBell()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    QRect r = QApplication::desktop()->availableGeometry();

    *x = r.x();
    *y = r.y();
    *width = r.width();
    *height = r.height();
}

wxWindow *wxGetActiveWindow()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}

bool wxColourDisplay()
{
    return QApplication::desktop()->depth() > 1;
}

bool wxLaunchDefaultApplication(const wxString& path, int flags)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}
