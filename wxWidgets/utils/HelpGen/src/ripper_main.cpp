/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef WIN32
#include <io.h>
#endif

#include <stdio.h>

#include "markup.h"    // get_HTML_markup_tags() will be used

#include "docripper.h"
#include "cjparser.h"  // C++/Java will be parsed here

/***** Main funciton *****/

#ifdef WIN32

// NOTE:: under Windows this generator parses all .h files
//        int the current directory

#include "direct.h"


void main(int argc, char** argv)
{
	cout << "C++/JAVA Source Documentation Generator (\"wxDocRipper\")" << endl
		 << "(C) 1998, Aleksandras Gluchovas (mailto:alex@soften.ktu.lt)" 
		 << endl << endl;


	RipperDocGen gen;

	// set up target script
	gen.SetScriptMarkupTags( get_HTML_markup_tags() );

	// setup source langauge 
	CJSourceParser* pParser = new CJSourceParser();

	gen.Init( pParser );

	// read process all files in the current directory

	struct _finddata_t c_file; // NT-specific?
    long hFile;

	hFile = _findfirst( "*.h", &c_file );
	int total = 0;

	while( hFile != -1L )
	{
		gen.ProcessFile( c_file.name );
		++total;

		if ( _findnext( hFile,  &c_file ) == -1L )
			break;
	}
  
 
	if ( total )
	{
		cout << endl
			 << "*** storing source documenation into ./srcref.html ***" 
			 << endl << endl;

		if ( !gen.SaveDocument( "srcref.html" ) )

			cout << "\nERROR: document cannot be saved" << endl;
	}
	else
	{
		 cout << "\nno .h files found in this directory - You must be running Windows now :-)"
			  << endl;
		 return;
	}


	printf( "\nTotal %d file(s) processed, done.\n", total );
}


#else

// NOTE:: on platfroms other then Windows this generator parses all files
//        given from the command line

int main(int argc, char** argv)
{
	cout << "C++/JAVA Source Documentation Generator (\"wxDocRipper\")" << endl
		 << "(C) 1998, Aleksandras Gluchovas (mailto:alex@soften.ktu.lt)" 
		 << endl << endl;

	if ( argc < 2 )
	{
		cout << "Usage: list of files with .h, .hpp, .cpp or .java extentions" 
			 << endl;
		return 1;
	}

	int from = 1, no_dump = 0;

	if ( strcmp( argv[1], "-x" ) == 0 )
	{
		from = 2;
		no_dump = 1;
	}

	RipperDocGen gen;

	// set up target script
	gen.SetScriptMarkupTags( get_HTML_markup_tags() );

	// setup source langauge 
	CJSourceParser* pParser = new CJSourceParser();

	gen.Init( pParser );

	for( int i = from; i != argc; ++i )
	
		gen.ProcessFile( argv[i] );

	if ( !no_dump )
	{
		cout << endl
			 << "*** storing source documenation into ./srcref.html ***" 
			 << endl << endl;

		if ( !gen.SaveDocument( "srcref.html" ) )

			cout << "\nERROR: document cannot be saved" << endl;
	}

	printf( "\nTotal %d file(s) processed, done.\n", argc-from );

	return 0;
}

#endif
