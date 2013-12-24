global oldDelimiters
global variables
global variablesRef
global bakefilesXML
global bakefilesXMLRef
global projectFile

global osxBuildFolder

property test : false

-- retrieves the files from the xml node including optional conditions
on parseSources(theName, theElement, theVariables, theConditions)
	set AppleScript's text item delimiters to " "
	set allElements to {}
	repeat with currElement in XML contents of theElement
		if class of currElement is text then
			set allElements to allElements & (every text item of currElement)
		else
			if class of currElement is XML element and XML tag of currElement is "if" then
				if (cond of XML attributes of currElement is in theConditions) then
					set allElements to allElements & (every text item of (item 1 of XML contents of currElement))
				end if
			end if
		end if
	end repeat
	set AppleScript's text item delimiters to oldDelimiters
	copy {varname:theName, entries:allElements} to end of theVariables
end parseSources

on parseEntry(theElement, theVariables, theConditions)
	set theName to var of XML attributes of theElement
	parseSources(theName, theElement, theVariables, theConditions)
end parseEntry

on parseLib(theElement, theVariables, theConditions)
	set theName to |id| of XML attributes of theElement
	repeat with currElement in XML contents of theElement
		if class of currElement is XML element and XML tag of currElement is "sources" then
			parseSources(theName, currElement, theVariables, theConditions)
		end if
	end repeat
end parseLib

on parseNode(anElement, theVariables, theConditions)
	if class of anElement is XML element and �
		XML tag of anElement is "set" then
		parseEntry(anElement, theVariables, theConditions)
	else
		if class of anElement is XML element and �
			XML tag of anElement is "lib" then
			parseLib(anElement, theVariables, theConditions)
		end if
	end if
end parseNode

-- iterates through the entire xml tree and populates the variables
on parseFiles(theXML, theVariables, theConditions)
	if class of theXML is XML element or class of theXML is XML document then
		repeat with anElement in XML contents of theXML
			try
				parseNode(anElement, theVariables, theConditions)
			on error number -1728
				-- ignore this error
			end try
		end repeat
	else if class of theXML is list then
		repeat with anElement in theXML
			try
				parseNode(anElement, theVariables, theConditions)
			on error number -1728
				-- ignore this error
			end try
		end repeat
	end if
end parseFiles

-- gets the entries of the variable named theName
on getVar(theName)
	repeat with anElement in variablesRef
		if (varname of anElement is theName) then
			return entries of anElement
		end if
	end repeat
end getVar

-- adds sources from fileList to a group named container
on addNode(theContainer, fileList)
	tell application "Xcode"
		tell project 1
			set theTargets to targets
			repeat with listItem in fileList
				if (listItem starts with "$(") then
					set AppleScript's text item delimiters to ""
					set variableName to (characters 3 through ((length of listItem) - 1) of listItem) as text
					set AppleScript's text item delimiters to oldDelimiters
					my addNode(theContainer, my getVar(variableName))
				else
					set AppleScript's text item delimiters to "/"
					set currPath to every text item in listItem
					set currFile to "../../" & listItem
					set currFileName to (item -1 in currPath)
					set currGroup to (items 1 through -2 in currPath as text)
					set AppleScript's text item delimiters to oldDelimiters
					try
						set theGroup to group theContainer
					on error
						tell root group
							make new group with properties {name:theContainer}
						end tell
					end try
					tell group theContainer
						try
							set theGroup to group named currGroup
						on error
							make new group with properties {name:currGroup}
						end try
						tell group currGroup
							set newFile to make new file reference with properties {name:currFileName, path:currFile, path type:project relative}
							repeat with theTarget in theTargets
								add newFile to (get compile sources phase of theTarget)
							end repeat
						end tell
					end tell
				end if
			end repeat
		end tell
	end tell
end addNode

-- retrieves contents of file at posixFilePath
on readFile(posixFilePath)
	set fileRef to open for access POSIX file posixFilePath
	set theData to read fileRef
	close access fileRef
	return theData
end readFile

on init()
	tell application "Xcode"
		quit
	end tell
	set variablesRef to a reference to variables
	set bakefilesXMLRef to a reference to bakefilesXML
	set oldDelimiters to AppleScript's text item delimiters
	tell application "Finder"
		set osxBuildFolder to POSIX path of ((folder of (path to me)) as Unicode text)
	end tell
end init

-- reads the files list and evaluates the conditions
on readFilesList(theFiles, theConditions)
	set variables to {}
	repeat with theFile in theFiles
		set bakefilesXML to parse XML (readFile(osxBuildFolder & theFile))
		parseFiles(bakefilesXMLRef, variablesRef, theConditions)
	end repeat
end readFilesList

-- creates a new project file from the respective template
on instantiateProject(theProject)
	set projectName to projectName of theProject
	set template to (osxBuildFolder & projectName & "_in.xcodeproj")
	set projectFile to (osxBuildFolder & projectName & ".xcodeproj")
	tell application "Finder"
		if exists projectFile as POSIX file then
			set templateContentFile to (osxBuildFolder & projectName & "_in.xcodeproj/project.pbxproj")
			set projectContentFile to (osxBuildFolder & projectName & ".xcodeproj/project.pbxproj")
			try
				tell me
					do shell script "rm -f " & quoted form of projectContentFile
				end tell
			end try
			try
				tell me
					do shell script "cp " & quoted form of templateContentFile & " " & quoted form of projectContentFile
				end tell
			end try
		else
			set duplicateProject to duplicate (template as POSIX file) with replace
			set name of duplicateProject to (projectName & ".xcodeproj")
		end if
	end tell
end instantiateProject

-- adds the source files of the nodes of theProject to the xcode project
on populateProject(theProject)
	tell application "Xcode"
		open projectFile
	end tell
	repeat with theNode in nodes of theProject
		-- reopen xcode for each pass, as otherwise the undomanager
		-- happens to crash quite frequently
		addNode(label of theNode, entries of theNode)
	end repeat
	tell application "Xcode"
		quit
	end tell
	do shell script (osxBuildFolder as text) & "fix_xcode_ids.py \"" & (POSIX path of projectFile as Unicode text) & "\""
	-- reopen again to let Xcode sort identifiers
	tell application "Xcode"
		open projectFile
	end tell
	tell application "Xcode"
		quit
	end tell
end populateProject

on makeProject(theProject)
	instantiateProject(theProject)
	readFilesList(bklfiles of theProject, conditions of theProject)
	populateProject(theProject)
end makeProject

-- main

init()
set theProject to {projectName:"", conditions:{}, bklfiles:{�
	"../bakefiles/files.bkl", "../bakefiles/regex.bkl", "../bakefiles/tiff.bkl", "../bakefiles/png.bkl", "../bakefiles/jpeg.bkl", "../bakefiles/scintilla.bkl", "../bakefiles/expat.bkl"}, nodes:{�
	{label:"base", entries:{"$(BASE_SRC)"}}, �
	{label:"base", entries:{"$(BASE_AND_GUI_SRC)"}}, �
	{label:"core", entries:{"$(CORE_SRC)"}}, �
	{label:"net", entries:{"$(NET_SRC)"}}, �
	{label:"adv", entries:{"$(ADVANCED_SRC)"}}, �
	{label:"webview", entries:{"$(WEBVIEW_SRC)"}}, �
	{label:"media", entries:{"$(MEDIA_SRC)"}}, �
	{label:"html", entries:{"$(HTML_SRC)"}}, �
	{label:"xrc", entries:{"$(XRC_SRC)"}}, �
	{label:"xml", entries:{"$(XML_SRC)"}}, �
	{label:"opengl", entries:{"$(OPENGL_SRC)"}}, �
	{label:"aui", entries:{"$(AUI_SRC)"}}, �
	{label:"ribbon", entries:{"$(RIBBON_SRC)"}}, �
	{label:"propgrid", entries:{"$(PROPGRID_SRC)"}}, �
	{label:"richtext", entries:{"$(RICHTEXT_SRC)"}}, �
	{label:"stc", entries:{"$(STC_SRC)"}}, �
	{label:"libtiff", entries:{"$(wxtiff)"}}, �
	{label:"libjpeg", entries:{"$(wxjpeg)"}}, �
	{label:"libpng", entries:{"$(wxpng)"}}, �
	{label:"libregex", entries:{"$(wxregex)"}}, �
	{label:"libscintilla", entries:{"$(wxscintilla)"}}, �
	{label:"libexpat", entries:{"$(wxexpat)"}} �
		}}
set conditions of theProject to {"PLATFORM_MACOSX=='1'", "TOOLKIT=='OSX_CARBON'", "WXUNIV=='0'", "USE_GUI=='1' and WXUNIV=='0'"}
set projectName of theProject to "wxcarbon"
makeProject(theProject)

set conditions of theProject to {"PLATFORM_MACOSX=='1'", "TOOLKIT=='OSX_COCOA'", "WXUNIV=='0'", "USE_GUI=='1' and WXUNIV=='0'"}
set projectName of theProject to "wxcocoa"

makeProject(theProject)
set conditions of theProject to {"PLATFORM_MACOSX=='1'", "TOOLKIT=='OSX_IPHONE'", "WXUNIV=='0'", "USE_GUI=='1' and WXUNIV=='0'"}
set projectName of theProject to "wxiphone"
makeProject(theProject)
