///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created:     2019-10-07
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWidgets licence
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
