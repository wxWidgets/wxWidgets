/////////////////////////////////////////////////////////////////////////////
// Name:        strategies.h
// Purpose:     Strategies page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page page_strategies Programming strategies

 This chapter is intended to list strategies that may be useful when
 writing and debugging wxWidgets programs. If you have any good tips,
 please submit them for inclusion here.

 @li @ref page_strategies_reducingerr
 @li @ref page_strategies_portability
 @li @ref page_strategies_debug


 <hr>


 @section page_strategies_reducingerr Strategies for reducing programming errors

 @subsection page_strategies_reducingerr_useassert Use ASSERT

 It is good practice to use ASSERT statements liberally, that check for conditions
 that should or should not hold, and print out appropriate error messages.

 These can be compiled out of a non-debugging version of wxWidgets
 and your application. Using ASSERT is an example of `defensive programming':
 it can alert you to problems later on.

 See wxASSERT for more info.

 @subsection page_strategies_reducingerr_usewxstring Use wxString in preference to character arrays

 Using wxString can be much safer and more convenient than using wxChar *.

 You can reduce the possibility of memory leaks substantially, and it is much more
 convenient to use the overloaded operators than functions such as @c strcmp.
 wxString won't add a significant overhead to your program; the overhead is compensated
 for by easier manipulation (which means less code).

 The same goes for other data types: use classes wherever possible.



 @section page_strategies_portability Strategies for portability

 @subsection page_strategies_portability_usesizers Use sizers

 Don't use absolute panel item positioning if you can avoid it. Different GUIs have
 very differently sized panel items. Consider using the @ref sizer_overview instead.

 @subsection page_strategies_portability_useresources Use wxWidgets resource files

 Use .xrc (wxWidgets resource files) where possible, because they can be easily changed
 independently of source code. See the @ref xrc_overview for more info.



 @section page_strategies_debug Strategies for debugging

 @subsection page_strategies_debug_positivethinking Positive thinking

 It is common to blow up the problem in one's imagination, so that it seems to threaten
 weeks, months or even years of work. The problem you face may seem insurmountable:
 but almost never is. Once you have been programming for some time, you will be able
 to remember similar incidents that threw you into the depths of despair. But
 remember, you always solved the problem, somehow!

 Perseverance is often the key, even though a seemingly trivial problem
 can take an apparently inordinate amount of time to solve. In the end,
 you will probably wonder why you worried so much. That's not to say it
 isn't painful at the time. Try not to worry -- there are many more important
 things in life.

 @subsection page_strategies_debug_simplifyproblem Simplify the problem

 Reduce the code exhibiting the problem to the smallest program possible
 that exhibits the problem. If it is not possible to reduce a large and
 complex program to a very small program, then try to ensure your code
 doesn't hide the problem (you may have attempted to minimize the problem
 in some way: but now you want to expose it).

 With luck, you can add a small amount of code that causes the program
 to go from functioning to non-functioning state. This should give a clue
 to the problem. In some cases though, such as memory leaks or wrong
 deallocation, this can still give totally spurious results!

 @subsection page_strategies_debug_usedebugger Use a debugger

 This sounds like facetious advice, but it is surprising how often people
 don't use a debugger. Often it is an overhead to install or learn how to
 use a debugger, but it really is essential for anything but the most
 trivial programs.

 @subsection page_strategies_debug_uselogging Use logging functions

 There is a variety of logging functions that you can use in your program:
 see @ref logfunctions.

 Using tracing statements may be more convenient than using the debugger
 in some circumstances (such as when your debugger doesn't support a lot
 of debugging code, or you wish to print a bunch of variables).

 @subsection page_strategies_debug_usedebuggingfacilities Use the wxWidgets debugging facilities

 You can use wxDebugContext to check for
 memory leaks and corrupt memory: in fact in debugging mode, wxWidgets will
 automatically check for memory leaks at the end of the program if wxWidgets is suitably
 configured. Depending on the operating system and compiler, more or less
 specific information about the problem will be logged.

 You should also use @ref debugmacros as part of a `defensive programming' strategy,
 scattering wxASSERTs liberally to test for problems in your code as early as possible. 
 Forward thinking will save a surprising amount of time in the long run.

 See the @ref debugging_overview for further information.

*/
