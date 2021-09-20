//////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/desktopenv.h
// Purpose:     wxDesktopEnv class documentation
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
@class wxDesktopEnv

wxDesktopEnv contains the function for moving the file/folder to Trash
or Recycle Bin.

@since 3.1.6
*/
class wxDesktopEnv
{
public:
    /**
    Moves the file/folder to the Recycle Bin/Trash.

    @returns true if the file/folder was successfully moved to the Recycle Bin/Trash.
    */
    static bool MoveToRecycleBin(const wxString &path);

    /**
    Gets the list of filenames inside the Recycle Bin/Trash
    */
    static bool GetFilesInRecycleBin(std::vector<wxString> &files);

    /**
    Restore the file from the Recycle Bin/Trash.
    This function is currently implemented on MSW and GTK

    @returns true if the file/folder was successfully restored from the Recycle Bin/Trash.
    */
    static bool RestoreFromRecycleBin(const wxString &path);
};
