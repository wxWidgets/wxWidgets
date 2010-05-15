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
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

#include <QtGui/QCursor>

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] )
{
    // Make it look similar to the assert messages:

    fprintf( stderr, "%s(%d): Missing implementation of \"%s\"\n", fileName, lineNumber, feature );
}

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

wxMouseState wxGetMouseState()
{
    return wxMouseState();
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxEndBusyCursor()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}


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
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    *width = 0;
    *height = 0;
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    *width = 0;
    *height = 0;
}

void wxBell()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    *x = 0;
    *y = 0;
    *width = 0;
    *height = 0;
}

wxWindow *wxGetActiveWindow()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}

bool wxColourDisplay()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return true;
}

bool wxLaunchDefaultApplication(const wxString& path, int flags)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}
