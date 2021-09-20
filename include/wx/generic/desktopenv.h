//////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef  _WX_GENERIC_DESKTOPENV_H
#define  _WX_GENERIC_DESKTOPENV_H

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    static bool MoveToRecycleBin(const wxString &path);
};

#endif
