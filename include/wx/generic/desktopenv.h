//////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Modified by:
// Created:     17.11.15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_DESKTOPENV_H
#define _WX_GENERIC_DESKTOPENV_H

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    wxDesktopEnv() { }
    virtual ~wxDesktopEnv() { }
    virtual bool MoveToRecycleBin(wxString &path) wxOVERRIDE;
};

#endif
