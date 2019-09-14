///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Modified by:
// Created:     17.11.15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_DESKTOPENV_H_
#define _WX_OSX_DESKTOPENV_H_

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    wxDesktopEnv();
    virtual ~wxDesktopEnv() wxOVERRIDE;
    virtual bool MoveToRecycleBin(wxString &fileName) wxOVERRIDE;
};

#endif
