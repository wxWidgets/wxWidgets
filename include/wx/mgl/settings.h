/////////////////////////////////////////////////////////////////////////////
// Name:        settings.h
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSETTINGSH__
#define __GTKSETTINGSH__

#ifdef __GNUG__
#pragma interface "settings.h"
#endif

#include "wx/defs.h"
#include "wx/gdicmn.h"
#include "wx/pen.h"
#include "wx/font.h"

// FIXME_MGL - probably belongs to wxUniversal

class wxSystemSettings: public wxObject
{
public:
    wxSystemSettings() {}

    static void Init() {}
    static void Done() {}

    // Get a system colour
    static wxColour GetSystemColour(int index);

    // Get a system font
    static wxFont GetSystemFont(int index);

    // Get a system metric, e.g. scrollbar size
    static int GetSystemMetric(int index);
    
    // Return true if the port has certain feature
    static bool GetCapability(int index);
};

#endif
    // __GTKSETTINGSH__
