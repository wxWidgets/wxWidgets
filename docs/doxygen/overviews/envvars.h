/////////////////////////////////////////////////////////////////////////////
// Name:        envvars.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_envvars Environment Variables

This section describes all environment variables that affect execution of
wxWidgets programs.

@beginDefList
@itemdef{WXTRACE,
        (Debug build only.)
        This variable can be set to a comma-separated list of trace masks used in
        wxLogTrace calls; wxLog::AddTraceMask is called for every mask
        in the list during wxWidgets initialization.}
@itemdef{WXPREFIX,
        (Unix only.)
        Overrides installation prefix. Normally, the prefix
        is hard-coded and is the same as the value passed to @c configure via
        the @c \--prefix switch when compiling the library (typically
        @c /usr/local or @c /usr). You can set WXPREFIX if you are for example
        distributing a binary version of an application and you don't know in advance
        where it will be installed.}
@itemdef{WX_NO_ABI_CHECK,
         If set to @c 1, disables the ABI check done during the program startup
         to verify that the library and the application were compiled using the
         same compiler, same or compatible compiler version and same build
         options. This variable should @e not be used lightly, usually the ABI
         mismatch is a real problem which will result in crashes and other
         misbehaviour. However sometimes newer compiler versions, not yet
         released when the code checking for their ABI in wxWidgets was
         written, preserve the ABI but still are rejected by the ABI check. In
         this case, the preferred course of action is to update to the latest
         wxWidgets version but if this is difficult or impossible, this
         environment variable can be used as a temporary workaround.}
*/

