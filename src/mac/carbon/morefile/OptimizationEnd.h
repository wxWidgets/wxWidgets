/*
	File:		OptimizationEnd.h

	Contains:	Defines that let you make MoreFiles code more efficient.

	Version:	MoreFiles

	Copyright:	© 1992-2001 by Apple Computer, Inc., all rights reserved.

	You may incorporate this sample code into your applications without
	restriction, though the sample code has been provided "AS IS" and the
	responsibility for its operation is 100% yours.  However, what you are
	not permitted to do is to redistribute the source as "DSC Sample Code"
	after having made changes. If you're going to re-distribute the source,
	we require that you make it clear in the source that the code was
	descended from Apple Sample Code, but that you've made changes.

	File Ownership:

		DRI:				Apple Macintosh Developer Technical Support

		Other Contact:		Apple Macintosh Developer Technical Support
							<http://developer.apple.com/bugreporter/>

		Technology:			DTS Sample Code

	Writers:

		(JL)	Jim Luther

	Change History (most recent first):

		 <1>	  2/7/01	JL		first checked in
*/

/*
	The Optimization changes to MoreFiles source and header files, along with
	this file and Optimization.h, let you optimize the code produced by MoreFiles
	in several ways.
	
	Original changes supplied by Fabrizio Oddone
*/


#if	__USEPRAGMAINTERNAL
	#if defined(__MWERKS__)
		#pragma internal reset
	#endif
#endif


#if	__WANTPASCALELIMINATION
	#ifndef __COMPILINGMOREFILES
		#undef pascal
	#endif
#endif
