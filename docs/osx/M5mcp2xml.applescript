-----------------------------------------------------------------------------
-- Name:        docs/mac/M5mcp2xml.applescript
-- Purpose:     Automatic export of CodeWarrior 5 projects to XML files
-- Author:      Gilles Depeyrot
-- Modified by:
-- Created:     28.11.2001
-- RCS-ID:      $Id$
-- Copyright:   (c) 2001 Gilles Depeyrot
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------
--
-- This AppleScript automatically recurses through the selected folder looking for
-- and exporting CodeWarrior projects to xml files.
-- To use this script, simply open it with the 'Script Editor' and run it.
--

--
-- Suffix used to recognize CodeWarrior project files
--
property gProjectSuffix : "M5.mcp"

--
-- Project and build success count
--
set theProjectCount to 0
set theProjectSuccessCount to 0

--
-- Ask the user to select the wxWindows samples folder
--
set theFolder to choose folder with prompt "Select the wxWindows folder"

ExportProjects(theFolder)

tell me to display dialog "Exported " & theProjectSuccessCount & " projects out of " & theProjectCount

--
-- ExportProjects
--
on ExportProjects(inFolder)
	global theProjectCount, theProjectSuccessCount
	
	tell application "Finder" to update inFolder
	
	try
		tell application "Finder" to set theProject to ((the first file of inFolder whose name ends with gProjectSuffix) as string)
	on error
		set theProject to ""
	end try
	
	if theProject is not "" then
		set theProjectCount to theProjectCount + 1
		
		-- save the current text delimiters
		set theDelimiters to my text item delimiters
		
		-- replace the ".mcp" extension with ".xml"
		set my text item delimiters to "."
		set theList to (every text item of theProject)
		set theList to (items 1 thru -2 of theList)
		set theExport to (theList as string) & ".xml"
		
		-- restore the text delimiters
		set my text item delimiters to theDelimiters
		
		tell application "CodeWarrior IDE 4.0.4"
			--
			-- Open the project in CodeWarrior
			--
			open theProject
			--
			-- Export the selected project
			--
			try
				export project document 1 in theExport
				set theProjectSuccessCount to theProjectSuccessCount + 1
			on error number errnum
				tell me to display dialog "Error " & errnum & " exporting " & theExport
			end try
			--
			-- Close the project
			--
			Close Project
		end tell
	end if
	
	tell application "Finder" to set theSubFolders to every folder of inFolder whose name does not end with " Data"
	repeat with theFolder in theSubFolders
		ExportProjects(theFolder)
	end repeat
	
end ExportProjects
