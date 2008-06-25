-----------------------------------------------------------------------------
-- Name:        docs/mac/M5build.applescript
-- Purpose:     Automatic build of projects with CodeWarrior 5
-- Author:      Gilles Depeyrot
-- Modified by:
-- Created:     06.10.2001
-- RCS-ID:      $Id$
-- Copyright:   (c) 2001 Gilles Depeyrot
-- Licence:     wxWindows licence
-----------------------------------------------------------------------------
--
-- This AppleScript automatically recurses through the selected folder looking for
-- and building CodeWarrior projects.
-- To use this script, simply open it with the 'Script Editor' and run it.
--

--
-- Suffix used to recognize CodeWarrior project files
--
property gProjectSuffix : "M5.mcp"

--
-- Values used to create the log file
--
property gEol : "
"
property gSeparator : "-------------------------------------------------------------------------------" & gEol

--
-- Project and build success count
--
set theProjectCount to 0
set theProjectSuccessCount to 0

--
-- Default log file name
--
set theDate to (day of (current date)) & "/" & GetMonthIndex(current date) & "/" & (year of (current date))
set theLogFileName to "build-" & theDate & ".log"

--
-- Ask the user to select the wxWindows samples folder
--
set theFolder to choose folder with prompt "Select the folder in which to build the projects"

--
-- Ask the user to choose the build log file
--
set theLogFile to choose file name with prompt "Save the build log file" default name theLogFileName

--
-- Open the log file to record the build log
--
set theLogFileRef to open for access theLogFile with write permission

--
-- Write log file header
--
write gSeparator starting at 0 to theLogFileRef
write "Build log" & gEol to theLogFileRef
write gSeparator to theLogFileRef
write "start on " & ((current date) as string) & gEol to theLogFileRef
write gSeparator to theLogFileRef
write "building projects in '" & (theFolder as string) & "'" & gEol to theLogFileRef
write gSeparator to theLogFileRef

--
-- Build or Rebuild targets?
--
set theText to "Build or rebuild projects?"
set theBuild to button returned of (display dialog theText buttons {"Cancel", "Build", "Rebuild"} default button "Rebuild" with icon note)
if theBuild is not equal to "Cancel" then
	--
	-- Build which targets?
	--
	set theText to theBuild & " Classic or Carbon targets?"
	set theType to button returned of (display dialog theText buttons {"Cancel", "Classic", "Carbon"} default button "Carbon" with icon note)
	if theType is not equal to "Cancel" then
		--
		-- Build Debug or Release targets?
		--
		set theText to theBuild & " " & theType & " Debug or " & theType & " Release targets?"
		set theOption to button returned of (display dialog theText buttons {"Cancel", "Release", "Debug"} default button "Debug" with icon note)
		if theOption is not equal to "Cancel" then
			set theTarget to theType & " " & theOption
			
			write "building project targets '" & theTarget & "'" & gEol to theLogFileRef
			write gSeparator to theLogFileRef
			
			BuildProjects(theLogFileRef, theFolder, theTarget, theBuild is equal to "Rebuild")
			
		end if
	end if
end if

--
-- Write log file footer
--
write "successful build of " & theProjectSuccessCount & " projects out of " & theProjectCount & gEol to theLogFileRef
write gSeparator to theLogFileRef
write "end on " & ((current date) as string) & gEol to theLogFileRef
write gSeparator to theLogFileRef
--
-- Close the log file
--
close access theLogFileRef

--
-- BuildProjects
--
on BuildProjects(inLogFileRef, inFolder, inTarget, inRebuild)
	global theProjectCount, theProjectSuccessCount
	
	tell application "Finder" to update inFolder
	
	try
		tell application "Finder" to set theProject to ((the first file of inFolder whose name ends with gProjectSuffix) as string)
	on error
		set theProject to ""
	end try
	
	if theProject is not "" then
		set theProjectCount to theProjectCount + 1
		
		write "building project '" & theProject & "'" & gEol to inLogFileRef
		
		tell application "CodeWarrior IDE 4.0.4"
			--
			-- Open the project in CodeWarrior
			--
			open theProject
			--
			-- Change to the requested target
			--
			Set Current Target inTarget
			--
			-- Remove object code if rebuild requested
			--
			if inRebuild then
				Remove Binaries
			end if
			--
			-- Build/Rebuild the selected target
			--
			set theBuildInfo to Make Project with ExternalEditor
			--
			-- Close the project
			--
			Close Project
		end tell
		--
		-- Report errors to build log file
		--
		write gEol to inLogFileRef
		ReportBuildInfo(inLogFileRef, theBuildInfo)
		write gSeparator to inLogFileRef
	end if
	
	tell application "Finder" to set theSubFolders to every folder of inFolder whose name does not end with " Data"
	repeat with theFolder in theSubFolders
		BuildProjects(inLogFileRef, theFolder, inTarget, inRebuild)
	end repeat
	
end BuildProjects

--
-- ReportBuildInfo
--
on ReportBuildInfo(inLogFileRef, inBuildInfo)
	global theProjectCount, theProjectSuccessCount
	
	set theErrorCount to 0
	set theWarningCount to 0
	
	repeat with theInfo in inBuildInfo
		tell application "CodeWarrior IDE 4.0.4"
			set theKind to ((messageKind of theInfo) as string)
			
			tell me to write "*** " & theKind & " *** " & message of theInfo & gEol to inLogFileRef
			try
				set theFile to ((file of theInfo) as string)
			on error
				set theFile to ""
			end try
			if theFile is not "" then
				tell me to write theFile & " line " & lineNumber of theInfo & gEol to inLogFileRef
			end if
			tell me to write gEol to inLogFileRef
		end tell
		
		if MessageKindIsError(theKind) then
			set theErrorCount to theErrorCount + 1
		else
			set theWarningCount to theWarningCount + 1
		end if
	end repeat
	
	if theErrorCount is 0 then
		set theProjectSuccessCount to theProjectSuccessCount + 1
		write "build succeeded with " & theWarningCount & " warning(s)" & gEol to inLogFileRef
	else
		write "build failed with " & theErrorCount & " error(s) and " & theWarningCount & " warning(s)" & gEol to inLogFileRef
	end if
end ReportBuildInfo

--
-- MessageKindIsError
--
on MessageKindIsError(inKind)
	if inKind is "compiler error" or inKind is "linker error" or inKind is "generic error" then
		return true
	else
		return false
	end if
end MessageKindIsError

--
-- GetMonthIndex
--
on GetMonthIndex(inDate)
	set theMonth to the month of inDate
	set theMonthList to {January, February, March, April, May, June, July, August, September, October, November, December}
	repeat with i from 1 to the number of items in theMonthList
		if theMonth is item i of theMonthList then
			return i
		end if
	end repeat
end GetMonthIndex
