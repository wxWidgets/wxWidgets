///////////////////////////////////////////////////////////////////////////////
// Name:        wx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Modified by:
// Created:     17.11.15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DESKTOPENV_H_BASE_
#define _WX_DESKTOPENV_H_BASE_

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxDesktopEnvBase
{
public:
    wxDesktopEnvBase() { }
    virtual ~wxDesktopEnvBase() { }
    virtual bool MoveFileToRecycleBin(const wxString &fileName);
protected:
    wxDECLARE_NO_COPY_CLASS(wxDesktopEnvBase);
};

#if defined (__WXMSW__)
    #include "wx/msw/desktopenv.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/desktopenv.h"
#endif

#endif
