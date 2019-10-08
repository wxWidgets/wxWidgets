//////////////////////////////////////////////////////////////////////////////
// Name:        wx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created:     2019-10-07
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DESKTOPENV_H_BASE_
#define _WX_DESKTOPENV_H_BASE_

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxDesktopEnvBase
{
public:
    wxDesktopEnvBase() { }
    virtual ~wxDesktopEnvBase() { }
    virtual bool MoveToRecycleBin(wxString &path);
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
