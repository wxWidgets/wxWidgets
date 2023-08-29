/////////////////////////////////////////////////////////////////////////////
// Name:        funcmacro_env.h
// Purpose:     Environment function and macro group docs
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@defgroup group_funcmacro_env Environment
@ingroup group_funcmacro

These functions allow access to get or change the values of environment
variables in a portable way. They are currently implemented under Win32 and
POSIX-like systems (Unix).

Remember that these functions add/change/delete environment variables of the
current process only. Child processes copy the environment variables of the parent,
but do not share them (a wxSetEnv() in the parent process won't change the value
returned by wxGetEnv() in the child process and vice versa).

For more advanced interprocess communication techniques see @ref overview_ipc.

*/

