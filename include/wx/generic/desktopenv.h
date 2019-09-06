//////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/desktopenv.h
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

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    wxDesktopEnv() { }
    virtual ~wxDesktopEnv() { }
    virtual bool MoveFileToRecycleBin(const wxString &fileName) wxOVERRIDE;
protected:
    wxDECLARE_NO_COPY_CLASS(wxDesktopEnvBase);
};

#endif
