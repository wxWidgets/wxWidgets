/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/desktopenv.h
// Purpose:     wxDesktopEnv class
// Author:      Igor Korot
// Modified by:
// Created:     17/11/2015
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_DESKTOPENV_H_
#define _WX_GTK_DESKTOPENV_H_

// ----------------------------------------------------------------------------
// Desktop Environment
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    wxDesktopEnv();
    virtual ~wxDesktopEnv() wxOVERRIDE { };
    virtual bool MoveToRecycleBin(wxString &fileName) wxOVERRIDE;
};

#endif
