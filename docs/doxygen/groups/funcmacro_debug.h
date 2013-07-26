/////////////////////////////////////////////////////////////////////////////
// Name:        funcmacro_debug.h
// Purpose:     Debugging function and macro group docs
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@defgroup group_funcmacro_debug Debugging macros
@ingroup group_funcmacro

Useful macros and functions for error checking and defensive programming.

Starting with wxWidgets 2.9.1, debugging support in wxWidgets is always
compiled in by default, you need to explicitly define ::wxDEBUG_LEVEL as 0 to
disable it completely. However, by default debugging macros are dormant in the
release builds, i.e. when the main program is compiled with the standard @c
NDEBUG symbol being defined. You may explicitly activate the debugging checks
in the release build by calling wxSetAssertHandler() with a custom function if
needed.

When debugging support is active, failure of both wxASSERT() and wxCHECK()
macros conditions result in a debug alert. When debugging support is inactive
or turned off entirely at compilation time, wxASSERT() and wxFAIL() macros
don't do anything while wxCHECK() still checks its condition and returns if it
fails, even if no alerts are shown to the user.

Finally, the compile time assertions don't happen during the run-time but
result in the compilation error messages if the condition they check fail.
They are always enabled and are not affected by ::wxDEBUG_LEVEL.

Related class group: @ref group_class_debugging.

*/

