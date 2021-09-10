//////////////////////////////////////////////////////////////////////////////
// Name:        wx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DESKTOPENV_H_BASE_
#define _WX_DESKTOPENV_H_BASE_

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxDesktopEnvBase
{
public:
    static bool MoveToRecycleBin(const wxString &path);
protected:
    wxDECLARE_NO_COPY_CLASS(wxDesktopEnvBase);
};

#if defined (__WXMSW__)
#include "wx/msw/desktopenv.h"
#elif defined(__WXGTK20__)
#include "wx/gtk/desktopenv.h"
#elif defined(__WXOSX__)
#include "wx/osx/desktopenv.h"
#else
#include "wx/generic/desktopenv.h"
#endif

#endif
