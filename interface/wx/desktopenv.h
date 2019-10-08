///////////////////////////////////////////////////////////////////////////////
// Name:        desktopenv.h
// Purpose:     wxDesktopEnv class interface
// Author:      Igor Korot
// Created      2019-10-07
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxDesktopEnv
 
    wxDesktopEnv contains the function for moving the files/folder to Trash
    or Recycle Bin. The call to this function is synchronous, which means that
    application will wait for the move to finish
 
    @since 3.1.3
*/
class wxDesktopEnv
{
public:
    /**
        Default constructor.
     */
    wxDesktopEnv();
    
    /**
        Moves the file to the Recycle Bin/Trash.
        
        @return true on success
    */
    bool MoveToRecycleBin(wxString &fileName);
};
