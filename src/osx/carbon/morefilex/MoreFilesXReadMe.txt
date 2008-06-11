MoreFilesX

Copyright (c) 1992-2002 Apple Computer, Inc.
All rights reserved.

________________________________________________________________________________

About MoreFilesX

MoreFilesX is a collection of useful high-level File Manager routines that use the HFS Plus APIs introduced in Mac OS 9.0 wherever possible.

While many of the routines in MoreFilesX are based on the older MoreFiles sample code (which used the older File Manager APIs), by using the HFS Plus APIs, the routines in MoreFilesX have several advantages over the older MoreFiles code:

¥ The routines are simpler to understand because the high-level HFS Plus APIs are more powerful.

¥ The routines support the features of the HFS Plus volume format such as long Unicode filenames and files larger than 2GB.

¥ In many cases, the routines execute more efficiently than code that uses the older File Manager APIs -- especially on non-HFS volumes.

¥ The routines use Apple's standard exception and assertion macros (the require, check, and verify macros) which improves readability and error handling, and which provides easy debug builds -- just add #define DEBUG 1 and every exception causes an assertion.

¥ The routines are thread safe. There are no global or static variables so multiple program threads can use MoreFilesX routines safely.

If you are writing new Carbon applications for Mac OS X that call the File Manager, you should use MoreFilesX -- not MoreFiles. If you're porting existing applications to Mac OS X and those applications use routines from MoreFiles, you should consider switching to the routines in MoreFilesX.

The routines were designed for applications running in the Mac OS X Carbon environment. All of the routines will work under Mac OS 9 if you define BuildingMoreFilesXForMacOS9 to 1. Doing that removes code that calls Mac OS X only APIs. MoreFilesX cannot be used in pre-Mac OS 9 system releases.

The routines in MoreFilesX have been tested (but not stress-tested) and are fully documented.

________________________________________________________________________________

Files in the MoreFilesX Package

MoreFilesX.c - the source code for MoreFilesX.

MoreFilesX.h - the header files and complete documentation for the routines included in MoreFilesX.

________________________________________________________________________________

How to use MoreFilesX

You can compile the code and link it into your programs. You can cut and paste portions of it into your programs. You can use it as an example. Since MoreFilesX is sample code, many routines are there simply to show you how to use the File Manager APIs. If a routine does more or less than what you want, you can have the source so you can modify it to do exactly you want it to do. Feel free to rip MoreFilesX off and modify its code in whatever ways you find work best for you.

You'll also notice that all routines that make other File Manager calls return an OSErr or OSStatus result. I always check for error results and you should too.

________________________________________________________________________________

Documentation

The documentation for the routines can be found in the header files. There, you'll find function prototypes, and a description of each call that includes a complete listing of all input and output parameters. For example, here's the function prototype and documentation for one of the routines, FSPathMakeFSSpec.

OSStatus
FSPathMakeFSSpec(
	UInt8 *path,
	FSSpec *spec,
	Boolean *isDirectory);	/* can be NULL */

/*
	The FSPathMakeFSSpec function converts a pathname to an FSSpec.
	
	path				--> A pointer to a C String that is the pathname. The
							format of the pathname you must supply can be
							determined with the Gestalt selector gestaltFSAttr's
							gestaltFSUsesPOSIXPathsForConversion bit.
							If the gestaltFSUsesPOSIXPathsForConversion bit is
							clear, the pathname must be a Mac OS File Manager
							full pathname in a C string. If the
							gestaltFSUsesPOSIXPathsForConversion bit is set,
							the pathname must be a UTF8 encoded POSIX absolute
							pathname in a C string. In either case, the pathname
							returned by FSMakePath can be passed to
							FSPathMakeFSSpec.
	spec				<-- The FSSpec.
	isDirectory			<** An optional pointer to a Boolean.
							If not NULL, true will be returned in the Boolean
							if the specified path is a directory or false will
							be returned in the Boolean if the specified path is
							a file.
*/

What do those arrows in the documentation for each routine mean?
	
	--> The parameter is an input
	
	<-- The parameter is an output. The pointer to the variable
		where the output will be returned (must not be NULL).
	
	<** The parameter is an optional output. If it is not a
		NULL pointer, it points to the variable where the output
		will be returned. If it is a NULL pointer, the output will
		not be returned and will possibly let the routine and the
		File Manager do less work. If you don't need an optional output,
		don't ask for it.
	**> The parameter is an optional input. If it is not a
		NULL pointer, it points to the variable containing the
		input data. If it is a NULL pointer, the input is not used
		and will possibly let the routine and the File Manager
		do less work.

While most of the routines in MoreFilesX have plenty of comments to clarify what the code is doing, a few have very few comments in their code because they simply make a single File Manager call. For those routines, the routine description is the comment. If something isn't clear, take a look at File Manager documentation online at <http://developer.apple.com/techpubs/macosx/Carbon/Files/FileManager/File_Manager/index.html>.

The methodology behind Apple's standard exception and assertion macros is clearly explained in Sean Parent's article "Living in an Exceptional World" develop, The Apple Technical Journal, August 1992 <http://developer.apple.com/dev/techsupport/develop/issue11toc.shtml>. Don't let the fact that this article is 10 years old fool you -- this is highly recommended reading.

________________________________________________________________________________

Release Notes

v1.0	Jan 25, 2002
First Release.
	
v1.0.1	Aug 23, 2002
[2850624]  Fixed C++ compile errors and Project Builder warnings.
[2853901]  Updated standard disclaimer.
[2853905]  Fixed #if test around header includes.
[3016251]  Changed FSMoveRenameObjectUnicode to not use the Temporary folder because it isn't available on NFS volumes.

________________________________________________________________________________

Bug Reports and Enhancement Requests

To file bug reports and enhancement requests, please go to <http://developer.apple.com/bugreporter/> and include "MoreFilesX Sample Code" in the title of your message.

Yes, we know that some of the routines available in MoreFiles still aren't in MoreFilesX. They were omitted due to time constraints.

________________________________________________________________________________
