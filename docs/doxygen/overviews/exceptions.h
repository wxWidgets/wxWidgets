/////////////////////////////////////////////////////////////////////////////
// Name:        exceptions.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_exceptions C++ Exceptions

@li @ref overview_exceptions_introduction
@li @ref overview_exceptions_strategies
@li @ref overview_exceptions_tech


<hr>


@section overview_exceptions_introduction Introduction

wxWidgets had been started long before the exceptions were introduced in C++ so
it is not very surprising that it is not built around using them as some more
modern C++ libraries are. For instance, the library doesn't throw exceptions to
signal about the errors. Moreover, up to (and including) the version 2.4 of
wxWidgets, even using the exceptions in the user code was dangerous because the
library code wasn't exception-safe and so an exception propagating through it
could result in memory and/or resource leaks, and also not very convenient.

However the recent wxWidgets versions are exception-friendly. This means that
while the library still doesn't use the exceptions by itself, it should be now
safe to use the exceptions in the user code and the library tries to help you
with this.


@section overview_exceptions_strategies Strategies for exceptions handling

There are several choice for using the exceptions in wxWidgets programs. First
of all, you may not use them at all. As stated above, the library doesn't throw
any exceptions by itself and so you don't have to worry about exceptions at all
unless your own code throws them. This is, of course, the simplest solution but
may be not the best one to deal with all possible errors.

The next simplest strategy is to only use exceptions inside non-GUI code, i.e.
never let unhandled exceptions escape the event handler in which it happened.
In this case using exceptions in wxWidgets programs is not different from using
them in any other C++ program.

Things get more interesting if you decide to let (at least some) exceptions
escape from the event handler in which they occurred. Such exceptions will be
caught by wxWidgets and the special wxApp::OnExceptionInMainLoop() method will
be called from the @c catch clause. This allows you to decide in a single place
what to do about such exceptions: you may want to handle the exception somehow
or terminate the program. In this sense, OnExceptionInMainLoop() is equivalent
to putting a @c try/catch block around the entire @c main() function body in
the traditional console programs. However notice that, as its name indicates,
this method won't help you with the exceptions thrown before the main loop is
started or after it is over, so you may still want to have @c try/catch in your
overridden wxApp::OnInit() and wxApp::OnExit() methods too, otherwise
wxApp::OnUnhandledException() will be called.

Finally, notice that even if you decide to not let any exceptions escape in
this way, this still may happen unexpectedly in a program using exceptions as a
result of a bug. So consider always overriding OnExceptionInMainLoop() in your
wxApp-derived class if you use exceptions in your program, whether you expect
it to be called or not. In the latter case you may simple re-throw the
exception and let it bubble up to OnUnhandledException() as well.

To summarize, when you use exceptions in your code, you may handle them in the
following places, in order of priority:
    -# In a @c try/catch block inside an event handler.
    -# In wxApp::OnExceptionInMainLoop().
    -# In wxApp::OnUnhandledException().

In the first two cases you may decide whether you want to handle the exception
and continue execution or to exit the program. In the last one the program is
about to exit already so you can just try to save any unsaved data and notify
the user about the problem (while being careful not to throw any more
exceptions as otherwise @c std::terminate() will be called).


@section overview_exceptions_tech Technicalities

To use any kind of exception support in the library you need to build it
with @c wxUSE_EXCEPTIONS set to 1. It is turned on by default but you may
wish to check @c include/wx/msw/setup.h file under Windows or run @c configure
with explicit @c --enable-exceptions argument under Unix.

On the other hand, if you do not plan to use exceptions, setting this
flag to 0 or using @c --disable-exceptions could result in a leaner and
slightly faster library.

As for any other library feature, there is a sample (@c except)
showing how to use it. Please look at its sources for further information.

*/
