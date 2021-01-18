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
@itemdef{WXWEBREQUEST_BACKEND,
         This variable can be set to override the choice of the default backend
         used by wxWebRequest, see wxWebSession::New(). Most common use is to
         set it to @c "CURL" to force using libcurl-based implementation under
         MSW or macOS platforms where the native implementation would be chosen
         by default.}
*/

