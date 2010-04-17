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

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
    const char feature[] )
{
    fprintf( stderr, "Missing implementation in %s(%d) of %s\n", fileName, lineNumber, feature );
}

wxPoint wxGetMousePosition()
{
    // return wxQtPoint( QCursor::pos() );
    return wxPoint();
}

void wxGetMousePosition( int *x, int *y )
{
    *x = 0;
    *y = 0;
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
}

void wxEndBusyCursor()
{
}


wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    return NULL;
}

wxWindow *wxFindWindowAtPointer(wxPoint& pt)
{
    return NULL;
}

bool wxGetKeyState(wxKeyCode key)
{
    return false;
}

int wxDisplayDepth()
{
    return 0;
}

void wxDisplaySize(int *width, int *height)
{
    *width = 0;
    *height = 0;
}

void wxDisplaySizeMM(int *width, int *height)
{
    *width = 0;
    *height = 0;
}

void wxBell()
{
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    *x = 0;
    *y = 0;
    *width = 0;
    *height = 0;
}

wxWindow *wxGetActiveWindow()
{
    return NULL;
}

bool wxColourDisplay()
{
    return true;
}

