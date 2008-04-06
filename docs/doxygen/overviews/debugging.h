/////////////////////////////////////////////////////////////////////////////
// Name:        debugging.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_debugging Debugging

Classes, functions and macros: wxDebugContext, wxObject, wxLog,
                                @ref group_funcmacro_log, @ref group_funcmacro_debug

Various classes, functions and macros are provided in wxWidgets to help you debug
your application. Most of these are only available if you compile both wxWidgets,
your application and @e all libraries that use wxWidgets with the __WXDEBUG__ symbol
defined. You can also test the __WXDEBUG__ symbol in your own applications to execute
code that should be active only in debug mode.

@li @ref overview_debugging_dbgctx
@li @ref overview_debugging_dbgmacros
@li @ref overview_debugging_logging
@li @ref overview_debugging_dbgctx2


<hr>


@section overview_debugging_dbgctx wxDebugContext

wxDebugContext is a class that never gets instantiated, but ties together
various static functions and variables. It allows you to dump all objects to that stream,
write statistics about object allocation, and check memory for errors.

It is good practice to define a wxObject::Dump member function for each class you derive
from a wxWidgets class, so that wxDebugContext::Dump can call it and
give valuable information about the state of the application.

If you have difficulty tracking down a memory leak, recompile
in debugging mode and call wxDebugContext::Dump and wxDebugContext::PrintStatistics at
appropriate places. They will tell you what objects have not yet been
deleted, and what kinds of object they are. In fact, in debug mode wxWidgets will automatically
detect memory leaks when your application is about to exit, and if there are any leaks,
will give you information about the problem. (How much information depends on the operating system
and compiler -- some systems don't allow all memory logging to be enabled). See the
memcheck sample for example of usage.

For wxDebugContext to do its work, the @e new and @e delete operators for wxObject
have been redefined to store extra information about dynamically allocated objects
(but not statically declared objects).

This slows down a debugging version of an application, but can
find difficult-to-detect memory leaks (objects are not
deallocated), overwrites (writing past the end of your object) and
underwrites (writing to memory in front of the object).

If debugging mode is on and the symbols wxUSE_GLOBAL_MEMORY_OPERATORS and
wxUSE_DEBUG_NEW_ALWAYS are set to 1 in setup.h, 'new' is defined to be:

@code
#define new new(__FILE__,__LINE__)
@endcode

All occurrences of 'new' in wxWidgets and your own application will use
the overridden form of the operator with two extra arguments. This means that
the debugging output (and error messages reporting memory problems) will tell you what
file and on what line you allocated the object. Unfortunately not all
compilers allow this definition to work properly, but most do.



@section overview_debugging_dbgmacros Debug macros

You should also use @ref group_funcmacro_debug as part of a 'defensive programming'
strategy, scattering wxASSERTs liberally to test for problems in your code as early as
possible.
Forward thinking will save a surprising amount of time in the long run.

#wxASSERT is used to pop up an error message box when a condition
is not @true. You can also use #wxASSERT_MSG to supply your
own helpful error message. For example:

@code
void MyClass::MyFunction(wxObject* object)
{
    wxASSERT_MSG( (object != NULL), "object should not be NULL in MyFunction!" );

    ...
};
@endcode

The message box allows you to continue execution or abort the program. If you are running
the application inside a debugger, you will be able to see exactly where the problem was.



@section overview_debugging_logging Logging functions

You can use the wxLogDebug and wxLogTrace functions to output debugging information in
debug mode; it will do nothing for non-debugging code.



@section overview_debugging_dbgctx2 wxDebugContext overview

Class: wxDebugContext

wxDebugContext is a class for performing various debugging and memory tracing operations.

This class has only static data and function members, and there should be
no instances. Probably the most useful members are SetFile (for directing output
to a file, instead of the default standard error or debugger output);
Dump (for dumping the dynamically allocated objects) and PrintStatistics
(for dumping information about allocation of objects). You can also call
Check to check memory blocks for integrity.

Here's an example of use. The SetCheckpoint ensures that only the
allocations done after the checkpoint will be dumped.

@code
wxDebugContext::SetCheckpoint();

wxDebugContext::SetFile("c:\\temp\\debug.log");

wxString *thing = new wxString;

char *ordinaryNonObject = new char[1000];

wxDebugContext::Dump();
wxDebugContext::PrintStatistics();
@endcode

You can use wxDebugContext if __WXDEBUG__ is defined, or you can use it
at any other time (if wxUSE_DEBUG_CONTEXT is set to 1 in setup.h). It is not disabled
in non-debug mode because you may not wish to recompile wxWidgets and your entire application
just to make use of the error logging facility.

@note wxDebugContext::SetFile has a problem at present, so use the default stream instead.
      Eventually the logging will be done through the wxLog facilities instead.

*/

