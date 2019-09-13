///////////////////////////////////////////////////////////////////////////////
// Name:        desktopenv.h
// Purpose:     interface of wxDesktopEnv
// Author:      wxWidgets team
// Licence:     wxWindows licence
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
        Moves the file to the Recycle Bin/Trash. The file name should be an absolute path
        
        @return true on success
    */
    bool MoveFileToRecycleBin(const wxString &fileName);
};
