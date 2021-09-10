//////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DESKTOPENV_H_
#define _WX_MSW_DESKTOPENV_H_

// ----------------------------------------------------------------------------
// Desktop Environment
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    static bool MoveToRecycleBin(const wxString &path);
protected:
};

#endif
