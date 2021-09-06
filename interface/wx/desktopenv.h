//////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/desktopenv.h
// Purpose:     wxDesktopEnv class documentation
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

/**
@class wxDesktopEnv

wxDesktopEnv contains the function for moving the file/folder to Trash
or Recycle Bin. The call to this function is synchronous, which means that
application will wait for the move to finish.

@since 3.1.5
This conversation was marked as resolved by oneeyeman1
*/
class wxDesktopEnv
{
public:
    /**
    Moves the file/folder to the Recycle Bin/Trash. The file name should be an absolute path

    @returns true on success
    */
    static bool MoveToRecycleBin(const wxString &path);
};
