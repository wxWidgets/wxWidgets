/////////////////////////////////////////////////////////////////////////////
// Name:        setup.h
// Purpose:     Configuration for the library
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETUP_H_
#define _WX_SETUP_H_

/*
 * General features
 *
 */

#define wxUSE_CONSTRAINTS       1
                                // Use constraints mechanism
#define wxUSE_CONFIG           1
                                // Use wxConfig, with CreateConfig in wxApp
#define _WX_GOODCOMPILER__
                                // gcc can have problems, but Windows compilers
                                // are generally OK.
#define WXWIN_COMPATIBILITY  1
                                // Compatibility with 1.66 API.
                                // Level 0: no backward compatibility, all new features
                                // Level 1: wxDC, OnSize (etc.) compatibility, but
                                //          some new features such as event tables

#define wxUSE_POSTSCRIPT  1
                        	// 0 for no PostScript device context
#define wxUSE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent
#define wxUSE_METAFILE    1
                                // 0 for no Metafile and metafile device context
#define wxUSE_IPC         1
                                // 0 for no interprocess comms
#define wxUSE_HELP        1
                                // 0 for no help facility
#define wxUSE_RESOURCES   1
                        	// 0 for no wxGetResource/wxWriteResource
#define wxUSE_CLIPBOARD   1
                        	// 0 for no clipboard functions
#define wxUSE_SPLINES     1
                            	// 0 for no splines
#define wxUSE_XFIG_SPLINE_CODE  1
                           	// 1 for XFIG spline code, 0 for AIAI spline code.
// AIAI spline code is slower, but freer of copyright issues.
                            	// 0 for no splines

#define wxUSE_TOOLBAR     1
                            // Use toolbars
#define wxUSE_DRAG_AND_DROP 1
                        	// 0 for no drag and drop

#define wxUSE_WX_RESOURCES        1
                                  // Use .wxr resource mechanism (requires PrologIO library)

#define wxUSE_DOC_VIEW_ARCHITECTURE 1
                                  // Set to 0 to disable document/view architecture
#define wxUSE_PRINTING_ARCHITECTURE  1
                                    // Set to 0 to disable print/preview architecture code
#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1
                                    // Set to 0 to disable PostScript print/preview architecture code
                                    // under Windows (just use Windows printing).
#define wxUSE_DYNAMIC_CLASSES     1
                                  // If 1, enables provision of run-time type information.
                                  // NOW MANDATORY: don't change.
#define wxUSE_MEMORY_TRACING      1
                                  // If 1, enables debugging versions of wxObject::new and
                                  // wxObject::delete *IF* WXDEBUG is also defined.
                                  // WARNING: this code may not work with all architectures, especially
                                  // if alignment is an issue.
#define wxUSE_DEBUG_CONTEXT       1
                                  // If 1, enables wxDebugContext, for
                                  // writing error messages to file, etc. 
                                  // If WXDEBUG is not defined, will still use
                                  // normal memory operators.
                                  // It's recommended to set this to 1,
                                  // since you may well need to output
                                  // an error log in a production
                                  // version (or non-debugging beta)
#define wxUSE_GLOBAL_MEMORY_OPERATORS 0
                                  // In debug mode, cause new and delete to be redefined globally.
                                  // If this causes problems (e.g. link errors), set this to 0.

#define wxUSE_DEBUG_NEW_ALWAYS 1
                                  // In debug mode, causes new to be defined to
                                  // be WXDEBUG_NEW (see object.h).
                                  // If this causes problems (e.g. link errors), set this to 0.

#define REMOVE_UNUSED_ARG 1
                                  // Set this to 0 if your compiler can't cope
                                  // with omission of prototype parameters.

#define wxUSE_C_MAIN 0
                                  // Set to 1 to use main.c instead of main.cpp (UNIX only)

#define wxUSE_ODBC                   0
                                    // Define 1 to use ODBC classes

#define wxUSE_IOSTREAMH     0
                                  // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                  // but you can't mix them. Set to 1 for <iostream.h>,
                                  // 0 for <iostream>

#define wxUSE_WXCONFIG      1
                                  // if enabled, compiles built-in OS independent wxConfig
                                  // class and it's file (any platform) and registry (Win)
                                  // based implementations
/*
 * Finer detail
 *
 */

#define wxUSE_APPLE_IEEE	  1
				// if enabled, the float codec written by Apple
				// will be used to write, in a portable way,
				// float on the disk

#endif
    // _WX_SETUP_H_
