---------------------------------------------------------------------------------
-- Name:        docs/mac/SetXMLCreator.applescript
-- Purpose:     Sets the creator types of the XML files
-- Author:      Ryan Wilcox
-- Modified by:
-- Created:     2004-03-30
-- RCS-ID:      $Id$
-- Copyright:   (c) 2004 Ryan Wilcox
-- Licence:     wxWindows licence
--
-- Press the run button and select the file you need (or, alternatively, save the
-- script as an application drag-and-drop the files on top of it).
---------------------------------------------------------------------------------

on run
    set myFile to choose file
    open ({myFile})
end run


on open (fileList)
    
    repeat with each in fileList
        
        tell application "Finder"
            if name of each contains "M5" or name of each contains "M7" or name of each contains "M8" then
                set creator type of each to "CWIE"
                set file type of each to "TEXT"
                
                log "set"
            end if
            
        end tell
    end repeat
end open

