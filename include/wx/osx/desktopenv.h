///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Modified by:
// Created:     17.11.15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DESKTOPENV_H_
#define _WX_DESKTOPENV_H_

class WXDLLIMPEXP_CORE wxDesktopEnv : public wxDesktopEnvBase
{
public:
    wxDesktopEnv();
    virtual ~wxDesktopEnv();
    virtual bool MoveFileToRecycleBin(const wxString &fileName);
private:
    bool MoveDirectoryToTrash(const wxString &dirName);
    bool MoveFileToTrash(const wxString &fileName);
};

#endif
