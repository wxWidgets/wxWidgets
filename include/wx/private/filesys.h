/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/filesys.h
// Purpose:     wxFileSystem private helpers
// Author:      wxWidgets team
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_FILESYS_H_
#define _WX_PRIVATE_FILESYS_H_

#include "wx/wxcrt.h"

// Return the start of the rightmost wxFileSystem location, ignoring anchors and
// Windows drive colons embedded in file URLs.
inline int wxFindFileSystemRightLocationStart(const wxString& location,
                                              int *lenOut = nullptr)
{
    int len = location.length();

    int i;
    for ( i = len - 1; i >= 0; i-- )
    {
        if ( location[i] == wxT('#') )
            len = i;

        if ( location[i] != wxT(':') )
            continue;

        // C: on Windows.
        if ( i == 1 )
            continue;

        if ( i >= 2 && wxIsalpha(location[i - 1]) &&
             location[i - 2] == wxT('/') )
        {
            continue;
        }

        break;
    }

    if ( lenOut )
        *lenOut = len;

    return i + 1;
}

#endif // _WX_PRIVATE_FILESYS_H_
